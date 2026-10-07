/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/resources/pcx_read.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/resources/pcx.h>
#include <thandor/thandor.h>
#include <string.h>

/* Replaces export 2 of engine\pcx.fnc (module offset 0x3B0, header check at 0x320). The original decoder
   accepted RLE-encoded 8 bits-per-pixel files with 1 plane (paletted) or 3 planes (direct colour) and built a
   GFX-style image record ('gfx' magic, file times, computer label, palette, image header, pixels). Its only
   caller (PcxPreview_Load64x64PaletteAndPixels) rejects every record that is not paletted, so only the
   1-plane path is reproduced here; 3-plane files are rejected right away (the original either failed on them
   or returned a direct-colour record, image header +0x08 = -1, that the caller rejected). */

/* PCX file layout (ZSoft) as far as the decoder reads it */
static constexpr int PCX_HEADER_BYTES = 0x80;
static constexpr int PCX_HEADER_MANUFACTURER = 0x00; /* must be 0x0A; the version byte (+0x01) is not checked */
static constexpr int PCX_HEADER_ENCODING = 0x02; /* must be 1 (RLE) */
static constexpr int PCX_HEADER_BITS_PER_PIXEL = 0x03; /* must be 8 */
static constexpr int PCX_HEADER_MIN_CORNER = 0x04; /* Xmin, Ymin (words) */
static constexpr int PCX_HEADER_MAX_CORNER = 0x08; /* Xmax, Ymax (words) */
static constexpr int PCX_HEADER_PLANES = 0x41; /* 1 accepted here (3 = direct colour, see above) */
static constexpr int PCX_HEADER_BYTES_PER_LINE = 0x42; /* word, bytes of one decoded scanline of one plane */
static constexpr int PCX_MANUFACTURER_ZSOFT = 0x0A;
static constexpr int PCX_ENCODING_RLE = 1;
/* The 256-colour palette at the end of the file: marker byte 0x0C, then 256 RGB triplets */
static constexpr int PCX_PALETTE_MARKER = 0x0C;
static constexpr auto PCX_PALETTE_TRAILER_BYTES = 1 + PCX_PALETTE_COLOR_COUNT * 3;
/* RLE: a byte 0xC0..0xFF repeats the next byte (its low 6 bits) times; any other byte is a literal pixel */
static constexpr int PCX_RLE_RUN_FLAGS = 0xC0;
static constexpr int PCX_RLE_RUN_LENGTH_MASK = 0x3F;


static uint32_t Pcx_ReadDword(const uint8_t *bytes)

{
  return (uint32_t)bytes[0] | (uint32_t)bytes[1] << 8 | (uint32_t)bytes[2] << 16 | (uint32_t)bytes[3] << 24;
}


/* Decodes an 8-bit, 1-plane, RLE-encoded PCX file with a 256-colour palette at its end. Accepts exactly the
   files the original decoder decoded as a paletted image:
   - at least 0x381 bytes (the 128-byte header plus the 769-byte palette trailer; compared as a signed
     count), manufacturer 0x0A, encoding 1, 8 bits per pixel, 1 plane; the version byte is not checked;
   - the byte 769 bytes before the end is the palette marker 0x0C;
   - the RLE data (from offset 0x80, never reading into the palette trailer) fills height scanlines of
     bytes-per-line bytes each, a run never crossing the end of a scanline. Data left over after the last
     scanline is ignored.
   Width is Xmax - Xmin + 1 and height Ymax - Ymin + 1, each taken modulo 65536 (so 1..65536).
   Original quirk: the size is computed by subtracting the packed corner dwords (Xmin | Ymin << 16 from
   Xmax | Ymax << 16), so an Xmax below Xmin borrows one from the height.
   Each decoded scanline contributes its first width bytes; padding beyond width (bytes-per-line > width) is
   dropped. */
bool Pcx_DecodeIndexed8(const uint8_t *fileBytes,uint32_t fileByteCount,PcxIndexedImage *outImage)

{
  const uint8_t *paletteTrailer;
  const uint8_t *encodedCursor;
  int32_t encodedBytesLeft;
  uint32_t cornerExtent;
  uint32_t bytesPerLine;
  uint32_t lineBufferBytes;
  uint64_t pixelCount;
  uint8_t *lineBuffer;
  void *block;
  uint8_t *pixelCursor;
  uint32_t lineFilled;
  uint32_t rowsLeft;
  uint32_t runLength;
  uint8_t code;
  int colorIndex;

  outImage->pixels = nullptr;
  if ((int32_t)fileByteCount < PCX_HEADER_BYTES + PCX_PALETTE_TRAILER_BYTES) {
    return false;
  }
  if (fileBytes[PCX_HEADER_MANUFACTURER] != PCX_MANUFACTURER_ZSOFT ||
      fileBytes[PCX_HEADER_ENCODING] != PCX_ENCODING_RLE || fileBytes[PCX_HEADER_BITS_PER_PIXEL] != 8 ||
      fileBytes[PCX_HEADER_PLANES] != 1) {
    return false;
  }
  cornerExtent = Pcx_ReadDword(fileBytes + PCX_HEADER_MAX_CORNER) - Pcx_ReadDword(fileBytes + PCX_HEADER_MIN_CORNER);
  outImage->width = (cornerExtent & 0xFFFF) + 1;
  outImage->height = (cornerExtent >> 16) + 1;
  bytesPerLine = (uint32_t)fileBytes[PCX_HEADER_BYTES_PER_LINE] | (uint32_t)fileBytes[PCX_HEADER_BYTES_PER_LINE + 1] << 8;

  paletteTrailer = fileBytes + fileByteCount - PCX_PALETTE_TRAILER_BYTES;
  if (*paletteTrailer != PCX_PALETTE_MARKER) {
    return false;
  }
  /* the file stores red, green, blue; the original kept each colour as the dword 0xFFRRGGBB with a second,
     zero dword (8 bytes per colour) */
  for (colorIndex = 0; colorIndex < PCX_PALETTE_COLOR_COUNT; colorIndex++) {
    outImage->paletteColors[colorIndex] = 0xFF000000u | (uint32_t)paletteTrailer[1 + colorIndex * 3] << 16 |
                                          (uint32_t)paletteTrailer[2 + colorIndex * 3] << 8 |
                                          (uint32_t)paletteTrailer[3 + colorIndex * 3];
  }

  /* The original allocated (width * height + 0xA23) & ~3 bytes for the record in 32 bits, so a picture too
     large for that failed on allocation (or overflowed); reject it here. */
  pixelCount = (uint64_t)outImage->width * outImage->height;
  if (pixelCount > 0x7FFFFFFF) {
    return false;
  }
  /* The original decoded into a line buffer of bytes-per-line bytes. With 0 bytes per line the scanline
     never completes (the counter wraps) and decoding fails once the data runs out (after writing past the
     empty buffer). With fewer bytes per line than width it copied width bytes, the excess being whatever
     followed the line buffer in the heap; here the buffer covers width and the excess reads as 0. */
  if (bytesPerLine == 0) {
    return false;
  }
  lineBufferBytes = bytesPerLine < outImage->width ? outImage->width : bytesPerLine;
  if (g_MemoryApi.alloc((uint32_t)pixelCount,&block) != 0) {
    outImage->pixels = nullptr;
    return false;
  }
  outImage->pixels = static_cast<uint8_t *>(block);
  if (g_MemoryApi.alloc(lineBufferBytes,&block) != 0) {
    Pcx_FreeIndexed8(outImage);
    return false;
  }
  lineBuffer = static_cast<uint8_t *>(block);
  memset(lineBuffer,0,lineBufferBytes);

  encodedCursor = fileBytes + PCX_HEADER_BYTES;
  encodedBytesLeft = (int32_t)fileByteCount - (PCX_HEADER_BYTES + PCX_PALETTE_TRAILER_BYTES);
  pixelCursor = outImage->pixels;
  for (rowsLeft = outImage->height; rowsLeft != 0; rowsLeft--) {
    lineFilled = 0;
    do {
      code = *encodedCursor;
      if (code < PCX_RLE_RUN_FLAGS) {
        if (encodedBytesLeft < 1) goto fail;
        lineBuffer[lineFilled] = code;
        lineFilled++;
        encodedCursor++;
        encodedBytesLeft--;
      }
      else {
        /* a run of length 0 consumes its two bytes and writes nothing */
        runLength = code & PCX_RLE_RUN_LENGTH_MASK;
        if (runLength > bytesPerLine - lineFilled || encodedBytesLeft < 2) goto fail;
        memset(lineBuffer + lineFilled,encodedCursor[1],runLength);
        lineFilled += runLength;
        encodedCursor += 2;
        encodedBytesLeft -= 2;
      }
    } while (lineFilled != bytesPerLine);
    memcpy(pixelCursor,lineBuffer,outImage->width);
    pixelCursor += outImage->width;
  }
  /* the original also failed when freeing the line buffer failed */
  if (g_MemoryApi.free(lineBuffer) != 0) {
    Pcx_FreeIndexed8(outImage);
    return false;
  }
  return true;

fail:
  g_MemoryApi.free(lineBuffer);
  Pcx_FreeIndexed8(outImage);
  return false;
}


/* Releases the pixels of an image decoded by Pcx_DecodeIndexed8 (NULL pixels are ignored). */
void Pcx_FreeIndexed8(PcxIndexedImage *image)

{
  if (image->pixels != nullptr) {
    g_MemoryApi.free(image->pixels);
    image->pixels = nullptr;
  }
}
