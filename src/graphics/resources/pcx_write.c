/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/resources/pcx_write.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/resources/pcx.h>
#include <thandor/thandor.h>
#include <thandor/core/error/runtime.h>
#include <string.h>

/* Implementation ownership: graphics/resources/pcx (encoder). */

/*
PCX encoder, replacing export 3 (module offset 0x980) of engine\pcx.fnc together with its helper at module
offset 0x850 (canvas build). The output is byte for byte what the module produced.

File layout:
  - 128-byte header, zero except: manufacturer 0x0A, version 5, encoding 1 (RLE), 8 bits per pixel,
    xmin = ymin = 0, xmax = width - 1, ymax = height - 1, 72 x 72 DPI, planes 1 (palette source) or
    3 (direct-colour source), bytes per line = width rounded up to even, palette info 1. The 16-colour header
    palette, the screen size fields and the filler stay zero (no time, date or computer label is used).
  - RLE scan lines. A palette source has `height` lines of palette indices. A direct-colour source (every
    framebuffer capture: paletteIndex -1) has 3 * height lines: per image row the red, the green and the
    blue plane, each taken from bits 16..23 / 8..15 / 0..7 of the ARGB8888 pixel (alpha dropped, no colour
    reduction).
  - Palette source only: 0x0C followed by 256 RGB triples taken from the source's palette bank
    (8-byte entries; bytes 2, 1, 0 of each entry become R, G, B).

RLE: each scan line is encoded on its own. A byte equal to its successor starts a run: count byte 0xC0 + n
(n <= 63) followed by the value. A single byte below 0xC0 is stored as is, a single byte >= 0xC0 as 0xC1, value.
*/

#define PCX_HEADER_SIZE 0x80
#define PCX_PALETTE_MARKER 0x0C
/* Bytes the original reserved in the output block beyond the scan lines: the header, plus for a palette
   source the marker, 768 palette bytes and one more for the last palette entry, which it stored as a dword. */
#define PCX_RESERVE_DIRECT_COLOR PCX_HEADER_SIZE
#define PCX_RESERVE_PALETTE (PCX_HEADER_SIZE + 1 + 0x300 + 1)
/* Palette banks of a texture source asset start at +0x200, 256 entries of 8 bytes each. */
#define PCX_PALETTE_BANKS_OFFSET 0x200
#define PCX_PALETTE_BANK_SIZE 0x800
#define PCX_PALETTE_ENTRY_SIZE 8

static void Pcx_StoreU16(uint8_t *at,uint32_t value)
{
  at[0] = (uint8_t)value;
  at[1] = (uint8_t)(value >> 8);
}

static void Pcx_StoreU32(uint8_t *at,uint32_t value)
{
  Pcx_StoreU16(at,value);
  Pcx_StoreU16(at + 2,value >> 16);
}

/* Module offset 0x850: lays the source rectangle into a zeroed canvas of the PCX scan-line layout
   (bytesPerLine = width rounded up to even; a direct-colour row is three plane lines R, G, B). */
static void Pcx_BuildCanvas(uint8_t *canvas,const uint8_t *assetBase,const GraphicsTextureSourceEntry *entry,
                            uint32_t bytesPerLine,uint32_t planeCount)
{
  uint32_t rowStride = bytesPerLine * planeCount;
  uint8_t *rowStart = canvas + (uint32_t)entry->originY * rowStride + (uint32_t)entry->originX;
  uint32_t paddedLines;
  uint32_t line;
  uint32_t row;
  uint32_t column;

  memset(canvas,0,rowStride * entry->logicalHeight);
  if (planeCount == 3) {
    const uint32_t *sourcePixel = (const uint32_t *)(assetBase + entry->dataOffset);
    for (row = 0; row < entry->pixelHeight; row++) {
      for (column = 0; column < entry->pixelWidth; column++) {
        uint32_t argb = *sourcePixel++;
        rowStart[column] = (uint8_t)(argb >> 16);
        rowStart[bytesPerLine + column] = (uint8_t)(argb >> 8);
        rowStart[bytesPerLine * 2 + column] = (uint8_t)argb;
      }
      rowStart += rowStride;
    }
  }
  else {
    const uint8_t *sourceIndex = assetBase + entry->dataOffset;
    for (row = 0; row < entry->pixelHeight; row++) {
      memcpy(rowStart,sourceIndex,entry->pixelWidth);
      sourceIndex += entry->pixelWidth;
      rowStart += rowStride;
    }
  }
  /* Odd width: the padding byte of a line repeats the last pixel.
     Original quirk: this covers the first pixelHeight rows of the canvas (times three plane lines), counted
     from canvas row 0 rather than from originY, so with a sub-rectangle at originY > 0 or pixelHeight <
     logicalHeight some padding bytes keep 0 or copy a zero pixel. Captures always cover the whole canvas. */
  if ((entry->logicalWidth & 1) != 0) {
    paddedLines = entry->pixelHeight * planeCount;
    for (line = 1; line <= paddedLines; line++) {
      canvas[line * bytesPerLine - 1] = canvas[line * bytesPerLine - 2];
    }
  }
}

/* Encodes every scan line of the canvas. Returns false when the encoded lines would exceed *budget bytes
   (the original's output-block check); *budget is reduced by the bytes written. */
static bool Pcx_EncodeScanLines(uint8_t **cursor,int32_t *budget,const uint8_t *canvas,uint32_t lineCount,
                                uint32_t bytesPerLine)
{
  uint8_t *out = *cursor;
  const uint8_t *source = canvas;
  uint32_t line;

  for (line = 0; line < lineCount; line++) {
    uint32_t remaining = bytesPerLine;
    while (remaining != 0) {
      uint8_t value = source[0];
      if (remaining >= 2 && source[1] == value) {
        /* run: count byte 0xC0 + n, at most 63 */
        uint8_t *countByte = out;
        *budget -= 2;
        if (*budget < 0) {
          return false;
        }
        out[0] = 0xC0;
        out[1] = value;
        out += 2;
        while (remaining != 0 && *source == value && *countByte != 0xFF) {
          (*countByte)++;
          source++;
          remaining--;
        }
      }
      else if (value >= 0xC0) {
        *budget -= 2;
        if (*budget < 0) {
          return false;
        }
        out[0] = 0xC1;
        out[1] = value;
        out += 2;
        source++;
        remaining--;
      }
      else {
        *budget -= 1;
        if (*budget < 0) {
          return false;
        }
        *out++ = value;
        source++;
        remaining--;
      }
    }
  }
  *cursor = out;
  return true;
}

/* Module pcx.fnc export 3 (offset 0x980).
   Encodes subresource 0 of a GFX texture source (in practice a framebuffer capture) as a PCX file. On success
   returns true with the file in *outBytes (owned by the caller, release with g_MemoryApi.free) and its size in
   *outByteCount. On failure returns false with an error code in *outError: the code of a failed memory call, or
   FATAL_ERROR_GENERAL_FAILURE when the source is not a GFX asset or the encoded data does not fit the output
   block. The source asset is only read.
   Memory use as in the original: the canvas comes from alloc; the output is built in the largest free block
   (allocLargestFreeBlock), the canvas is freed and the output block is then shrunk to the file size.
   Original quirk: for a non-GFX source the original returned the address of its message "error: PCX: not a GFX
   file!" instead of an error code; FATAL_ERROR_GENERAL_FAILURE stands in for it (no caller reads the error).
   Original quirk: header dimensions are 16-bit (xmax/ymax computed as one dword (height << 16 | width) - 0x10001);
   zero width or height was not handled (the original's counted loops ran away) and is rejected here. */
bool Pcx_EncodeCapture(GraphicsCapturedTextureSourceAsset *capture,void **outBytes,uint32_t *outByteCount,
                       uint32_t *outError)
{
  const uint8_t *assetBase = (const uint8_t *)capture;
  const GraphicsTextureSourceEntry *entry;
  bool directColor;
  uint32_t planeCount;
  uint32_t bytesPerLine;
  uint32_t reserve;
  uint8_t *canvas;
  uint8_t *output;
  uint32_t blockSize;
  uint8_t *cursor;
  int32_t budget;
  uint32_t dimensions;
  uint32_t byteCount;
  uint32_t status;

  if ((capture->common).magic != ASSET_MAGIC_GFX) {
    *outError = FATAL_ERROR_GENERAL_FAILURE;
    return false;
  }
  entry = (const GraphicsTextureSourceEntry *)(assetBase + (capture->tableDescriptor).subresourceTableOffset);
  if (entry->logicalWidth == 0 || entry->logicalHeight == 0) {
    *outError = FATAL_ERROR_GENERAL_FAILURE;
    return false;
  }
  directColor = entry->paletteIndex == -1;
  planeCount = directColor ? 3 : 1;
  reserve = directColor ? PCX_RESERVE_DIRECT_COLOR : PCX_RESERVE_PALETTE;
  bytesPerLine = (entry->logicalWidth + 1) & ~1u;

  status = g_MemoryApi.alloc(bytesPerLine * entry->logicalHeight * planeCount,(void **)&canvas);
  if (status != 0) {
    *outError = status;
    return false;
  }
  Pcx_BuildCanvas(canvas,assetBase,entry,bytesPerLine,planeCount);

  status = g_MemoryApi.allocLargestFreeBlock((void **)&output,&blockSize);
  if (status != 0) {
    g_MemoryApi.free(canvas);
    *outError = status;
    return false;
  }
  budget = (int32_t)(blockSize - reserve);
  if (budget < 0) {
    g_MemoryApi.free(output);
    g_MemoryApi.free(canvas);
    *outError = FATAL_ERROR_GENERAL_FAILURE;
    return false;
  }

  memset(output,0,PCX_HEADER_SIZE);
  output[0] = 0x0A; /* manufacturer: ZSoft */
  output[1] = 5;    /* version 3.0 with palette */
  output[2] = 1;    /* RLE */
  output[3] = 8;    /* bits per pixel and plane */
  dimensions = entry->logicalHeight << 16 | entry->logicalWidth;
  Pcx_StoreU32(output + 0x08,dimensions - 0x10001); /* xmax, ymax */
  Pcx_StoreU16(output + 0x0C,72);                   /* horizontal DPI */
  Pcx_StoreU16(output + 0x0E,72);                   /* vertical DPI */
  output[0x41] = (uint8_t)planeCount;
  Pcx_StoreU16(output + 0x42,bytesPerLine);
  Pcx_StoreU16(output + 0x44,1);                    /* palette info: colour */
  /* Original quirk: it also stored the dimensions dword at offset 0xBA, inside the scan-line area; the scan
     lines overwrite it or it lies past the file end, so it never reaches the file and is left out. */

  cursor = output + PCX_HEADER_SIZE;
  if (!Pcx_EncodeScanLines(&cursor,&budget,canvas,entry->logicalHeight * planeCount,bytesPerLine)) {
    g_MemoryApi.free(output);
    g_MemoryApi.free(canvas);
    *outError = FATAL_ERROR_GENERAL_FAILURE;
    return false;
  }

  if (!directColor) {
    const uint8_t *paletteEntry = assetBase + PCX_PALETTE_BANKS_OFFSET +
                                  (uint32_t)entry->paletteIndex * PCX_PALETTE_BANK_SIZE;
    uint32_t colorIndex;
    *cursor++ = PCX_PALETTE_MARKER;
    for (colorIndex = 0; colorIndex < 256; colorIndex++) {
      cursor[0] = paletteEntry[2];
      cursor[1] = paletteEntry[1];
      cursor[2] = paletteEntry[0];
      cursor += 3;
      paletteEntry += PCX_PALETTE_ENTRY_SIZE;
    }
  }

  status = g_MemoryApi.free(canvas);
  if (status != 0) {
    g_MemoryApi.free(output);
    *outError = status;
    return false;
  }
  byteCount = (uint32_t)(cursor - output);
  status = g_MemoryApi.shrinkInPlace(byteCount,output);
  if (status != 0) {
    g_MemoryApi.free(output);
    *outError = status;
    return false;
  }
  *outBytes = output;
  *outByteCount = byteCount;
  return true;
}
