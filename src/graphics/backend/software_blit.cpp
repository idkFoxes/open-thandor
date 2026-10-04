/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/backend/software_blit.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* Software blits into the 32-bit framebuffer: texture source blits (alpha, half, bilinear stretch, integer
   scale, palette bank, saturated add, modulated), rectangle fill and the region copies. The original also had
   a 16-bit (RGB565) version of each blit and of the fill; they are gone with 16-bit colour. */

#include <thandor/graphics/backend/software_blit.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include "software_blit_helpers.h"

/* Module data. */

static const uint64_t g_SoftwareBilinearPackedByteClampMask = 0xFFFFFFFFull;

/* GraphicsFramebufferCopyRegionToOriginProc * hook slot, statically SoftwareFramebuffer_CopyRegionToOrigin (software.c). */
static GraphicsFramebufferCopyRegionToOriginProc *g_GraphicsFramebufferCopyRegionToOrigin = THANDOR_FN(SoftwareFramebuffer_CopyRegionToOrigin);

/* GraphicsFramebufferCopyOriginToRegionProc * hook slot, statically SoftwareFramebuffer_CopyOriginToRegion (software.c). */
static GraphicsFramebufferCopyOriginToRegionProc *g_GraphicsFramebufferCopyOriginToRegion = THANDOR_FN(SoftwareFramebuffer_CopyOriginToRegion);

/* Clips and draws one source subresource into a four-byte framebuffer (source-alpha blit, see
   docs/software_raster.md "Blits"). Alpha 0 is skipped, alpha 0xFF is converted through
   g_SoftwarePixelPackTables and written, anything else is blended in 8-bit lanes. A
   paletted texel uses the entry's second dword (+4) for everything: the alpha test, the blend colour, and the
   opaque write, which converts it through the pack tables again. Always returns false.
*/
Bool8 SoftwareTextureSource_BlitSourceAlpha32(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer)

{
  BlitRegion region;
  int x;
  int y;

  if (!Blit_SetupSubresource(sourceAsset, subresourceIndex, framebuffer, 4, drawX, drawY, clipMaxY, clipMaxX,
                             clipMinY, clipMinX, &region)) {
    return false;
  }
  for (y = 0; y < region.height; y++) {
    const uint8_t *texel = region.texels + y * region.texelStride;
    uint32_t *pixel = (uint32_t *)(region.pixels + y * region.pixelStride);
    for (x = 0; x < region.width; x++, texel += region.texelBytes, pixel++) {
      uint32_t color = region.palette != nullptr ? Blit_PalettePixel(&region, *texel) : *(const uint32_t *)texel;
      if (Blit_IsTransparent(color)) {
        continue;
      }
      *pixel = Blit_IsOpaque(color) ? Blit_ConvertArgb(color) : Blit_BlendArgb32(color, *pixel);
    }
  }
  return false;
}

/* Clips and draws one source subresource into a four-byte framebuffer, with the source RGB at half
   strength (see docs/software_raster.md "Blits"). Alpha 0 is skipped; every other alpha, 0xFF included, blends,
   so there is no opaque copy. Quirks kept from the original: a paletted texel uses the entry's second dword (+4)
   as its colour, like the other 32-bit blits, and only the paletted path halves the source ((c * 0x101) >> 3);
   the direct-colour path uses >> 2, i.e. it is an ordinary source-alpha blend whose alpha 0xFF still goes
   through the blend tables. Always returns false.
*/
Bool8 SoftwareTextureSource_BlitHalfSourceRgb32(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer)

{
  BlitRegion region;
  int sourceShift;
  int x;
  int y;

  if (!Blit_SetupSubresource(sourceAsset, subresourceIndex, framebuffer, 4, drawX, drawY, clipMaxY, clipMaxX,
                             clipMinY, clipMinX, &region)) {
    return false;
  }
  sourceShift = region.palette != nullptr ? 3 : 2;
  for (y = 0; y < region.height; y++) {
    const uint8_t *texel = region.texels + y * region.texelStride;
    uint32_t *pixel = (uint32_t *)(region.pixels + y * region.pixelStride);
    for (x = 0; x < region.width; x++, texel += region.texelBytes, pixel++) {
      uint32_t color = region.palette != nullptr ? Blit_PalettePixel(&region, *texel) : *(const uint32_t *)texel;
      if (Blit_IsTransparent(color)) {
        continue;
      }
      *pixel = Blit_PackLanes32(
          Blit_BlendLanes(Blit_ArgbLanes(color, sourceShift), Blit_ArgbLanes(*pixel, 2), color >> 24));
    }
  }
  return false;
}

/* Stretches one direct-color source subresource into a four-byte framebuffer using two-dimensional linear
   interpolation. The source entry must be direct color: paletteIndex == -1. The function computes 8-bit fractional
   source steps from (pixelWidth-1)/(destinationWidth-1) and (pixelHeight-1)/(destinationHeight-1). Four
   neighboring ARGB8888 pixels are blended horizontally and vertically through g_SoftwareBilinearForwardFactors and
   g_SoftwareBilinearInverseFactors. The routine performs no clipping and draws the destination width in
   pixel pairs (an odd last column is left out).
   The original divided by zero for a destination 1 pixel wide or high, wrote outside the framebuffer for a
   destination that does not fit, and read the right and lower neighbours one texel or row past the image at the
   last column and row. Bounded here because those were crashes and reads past the asset: a destination narrower
   than 2 pixels (nothing is drawn: no pixel pair) and one whose pixels would leave the framebuffer memory are
   skipped, a height of 1 uses step 0, and the neighbour past the last column or row is the texel itself. That
   neighbour's weight is always 0 there (the source position is then exactly the last texel), so every pixel is
   unchanged.
*/
void SoftwareTextureSource_StretchDirectColorBilinear32
          (GraphicsPixelDimension destinationHeight,GraphicsPixelDimension destinationWidth,
          GraphicsScreenCoordinate destinationY,GraphicsScreenCoordinate destinationX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer)

{
  /* The MMX lanes in plain C. Two destination pixels per step; each blends four ARGB8888 neighbours
     through the word tables g_SoftwareBilinearInverseFactors (weight of the left/upper neighbour) and
     g_SoftwareBilinearForwardFactors (weight of the right/lower one), as PMULHW does. */
  const short *firstWeights = (const short *)g_SoftwareBilinearInverseFactors;
  const short *secondWeights = (const short *)g_SoftwareBilinearForwardFactors;
  const unsigned long long clampMask = g_SoftwareBilinearPackedByteClampMask;
  uint8_t *asset = (uint8_t *)sourceAsset;
  GraphicsTextureSourceEntry *entry;
  uint8_t *sourceBase;
  uint8_t *sourceRow;
  uint32_t *destinationRow;
  uint32_t pitchPixels;
  uint32_t sourceWidth;
  uint32_t sourceHeight;
  uint32_t stepX;
  uint32_t stepY;
  uint32_t fy;
  uint32_t row;
  uint32_t pair;
  int64_t firstPixelIndex;
  int64_t endPixelIndex;

  /* only a "gfx" texture source; the entry table holds 0x20-byte GraphicsTextureSourceEntry records */
  if (((uint32_t)sourceAsset->common.magic != ASSET_MAGIC_GFX) ||
      (subresourceIndex >= sourceAsset->tableDescriptor.subresourceCount)) {
    return;
  }
  entry = (GraphicsTextureSourceEntry *)(asset + sourceAsset->tableDescriptor.subresourceTableOffset +
                                         subresourceIndex * sizeof(GraphicsTextureSourceEntry));
  /* paletteIndex -1: ARGB8888 texels */
  if (((uint32_t)framebuffer->bytesPerPixel != 4) || (entry->paletteIndex != -1)) {
    return;
  }
  pitchPixels = framebuffer->width;
  sourceWidth = entry->pixelWidth;
  sourceHeight = entry->pixelHeight;
  if (destinationWidth < 2 || destinationHeight == 0 || sourceWidth == 0 || sourceHeight == 0) {
    return;
  }
  /* the written pixels, first to one past the last, must lie in the framebuffer's width * height pixels */
  firstPixelIndex = (int64_t)destinationY * pitchPixels + destinationX;
  endPixelIndex = firstPixelIndex + (int64_t)(destinationHeight - 1) * pitchPixels + (destinationWidth & ~1u);
  if (firstPixelIndex < 0 || endPixelIndex > (int64_t)pitchPixels * framebuffer->height) {
    return;
  }
  destinationRow = (uint32_t *)framebuffer->pixels +
                   (int32_t)(destinationY * pitchPixels + destinationX);
  /* 8.8 fixed-point source steps */
  stepX = ((sourceWidth - 1) * 256) / (destinationWidth - 1);
  stepY = 0;
  if (destinationHeight > 1) {
    stepY = ((sourceHeight - 1) * 256) / (destinationHeight - 1);
  }
  sourceBase = asset + entry->dataOffset;
  sourceRow = sourceBase;
  fy = 0;
  for (row = destinationHeight; row != 0; row--) {
    uint32_t fx = 0;
    uint32_t *out = destinationRow;
    /* byte offset of the lower neighbour row (the row itself past the last row) */
    uint32_t lowerRowOffset = ((fy >> 8) + 1 < sourceHeight) ? sourceWidth * 4 : 0;
    for (pair = destinationWidth >> 1; pair != 0; pair--) {
      uint32_t pixels[2];
      int half;
      for (half = 0; half < 2; half++) {
        uint32_t x = fx >> 8;
        const uint8_t *p00 = sourceRow + x * 4;
        const uint8_t *p10 = sourceRow + lowerRowOffset + x * 4;
        /* lane offset of the right neighbour (the texel itself past the last column) */
        uint32_t right = (x + 1 < sourceWidth) ? 4 : 0;
        uint32_t wx = fx & 0xff;
        uint32_t wy = fy & 0xff;
        uint32_t pixel = 0;
        int lane;
        for (lane = 0; lane < 4; lane++) {
          int a = ((p00[lane] * COLOR_CHANNEL_TO_WORD_LANE) >> 2);
          int b = ((p00[lane + right] * COLOR_CHANNEL_TO_WORD_LANE) >> 2);
          int c = ((p10[lane] * COLOR_CHANNEL_TO_WORD_LANE) >> 2);
          int d = ((p10[lane + right] * COLOR_CHANNEL_TO_WORD_LANE) >> 2);
          short top = (short)(((a * firstWeights[(int32_t)(wx * 4 + lane)]) >> 16) + ((b * secondWeights[(int32_t)(wx * 4 + lane)]) >> 16));
          short bottom = (short)(((c * firstWeights[(int32_t)(wx * 4 + lane)]) >> 16) + ((d * secondWeights[(int32_t)(wx * 4 + lane)]) >> 16));
          short mixed = (short)(((top * firstWeights[(int32_t)(wy * 4 + lane)]) >> 16) +
                                ((bottom * secondWeights[(int32_t)(wy * 4 + lane)]) >> 16));
          int value = (unsigned short)mixed >> 2;
          if (value > ARGB8888_CHANNEL_MAX) value = ARGB8888_CHANNEL_MAX;
          pixel |= (uint32_t)value << (lane * 8);
        }
        pixels[half] = pixel;
        fx = fx + stepX;
      }
      /* PACKUSWB of the first pixel with itself duplicates it into both halves; PAND with the clamp mask,
         then POR with the second pixel shifted into the high half. */
      *(unsigned long long *)out =
           ((((unsigned long long)pixels[0] << 32) | pixels[0]) & clampMask) |
           ((unsigned long long)pixels[1] << 32);
      out += 2;
    }
    destinationRow = destinationRow + pitchPixels;
    fy = fy + stepY;
    sourceRow = sourceBase + (fy >> 8) * sourceWidth * 4;
  }
}

/* Draws one source subresource into a four-byte framebuffer at an integer scale (see
   docs/software_raster.md "Blits"). Every texel becomes an integerScale x integerScale block, and the entry's
   origin is scaled too. Nothing is clipped at the source: the whole scaled image is walked and each pixel is
   tested against the clip rectangle, which is clamped to the framebuffer. Alpha 0 is skipped, alpha 0xFF is
   written, anything else is blended in 8-bit lanes. Unlike BlitSourceAlpha32, a paletted texel here uses the
   layout of the original's 16-bit blits: it tests the alpha of the converted pixel (+4), blends the ARGB colour
   (+0), and writes +4 as it is (not converted again) when opaque. A direct texel is converted through
   g_SoftwarePixelPackTables when opaque. Quirks kept: every negative paletteIndex means ARGB texels, and the
   counters are do-while loops, so a scale or image size of 0 runs them 2^32 times.
*/
void SoftwareTextureSource_BlitIntegerScaledSourceAlpha32
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsIntegerScale integerScale,GraphicsSubresourceIndex subresourceIndex,
          GraphicsTextureSourceAsset *sourceAsset,SoftwareFramebufferAccess *framebuffer)

{
  BlitScaledImage image;
  const uint8_t *sourceRow;
  uint32_t sourceRowsLeft;
  int y;

  if (!Blit_SetupScaled(sourceAsset, subresourceIndex, framebuffer, 4, integerScale, drawX, drawY, clipMaxY,
                        clipMaxX, clipMinY, clipMinX, &image)) {
    return;
  }
  sourceRow = image.texels;
  y = image.top;
  sourceRowsLeft = image.height;
  do {
    uint32_t repeatRowsLeft = integerScale;
    do {
      if (BlitScaled_RowVisible(&image, y)) {
        const uint8_t *texel = sourceRow;
        uint32_t columnsLeft = image.width;
        int x = image.left;
        do {
          uint32_t blendColor;
          uint32_t color = BlitScaled_TexelColor(&image, texel, &blendColor);
          if (Blit_IsTransparent(color)) {
            x += (int)integerScale;
          }
          else {
            uint32_t repeatColumnsLeft = integerScale;
            do {
              if (BlitScaled_ColumnVisible(&image, x)) {
                uint32_t *pixel = (uint32_t *)BlitScaled_Pixel(framebuffer, 4, x, y);
                if (!Blit_IsOpaque(color)) {
                  *pixel = Blit_BlendArgb32(blendColor, *pixel);
                }
                else {
                  *pixel = image.palette != nullptr ? color : Blit_ConvertArgb(color);
                }
              }
              x++;
            } while (--repeatColumnsLeft != 0);
          }
          texel += image.texelBytes;
        } while (--columnsLeft != 0);
      }
      y++;
    } while (--repeatRowsLeft != 0);
    sourceRow += (int32_t)(image.width * image.texelBytes);
  } while (--sourceRowsLeft != 0);
}

/* BlitSourceAlpha32 with an explicit palette bank (see docs/software_raster.md "Blits"). A paletted
   subresource is drawn with paletteBankIndex instead of its own paletteIndex; the entry's paletteIndex must still
   be valid, and paletteBankIndex is only checked (unsigned, < paletteBankCount) after clipping. A direct-colour
   subresource ignores paletteBankIndex. The pixel operation is that of BlitSourceAlpha32 (the palette entry's +4
   dword used for everything).
*/
void SoftwareTextureSource_BlitSourceAlphaPaletteBank32
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          PaletteBankIndex paletteBankIndex,GraphicsSubresourceIndex subresourceIndex,
          GraphicsTextureSourceAsset *sourceAsset,SoftwareFramebufferAccess *framebuffer)

{
  BlitRegion region;
  int x;
  int y;

  if (!Blit_SetupSubresource(sourceAsset, subresourceIndex, framebuffer, 4, drawX, drawY, clipMaxY, clipMaxX,
                             clipMinY, clipMinX, &region)) {
    return;
  }
  if (region.palette != nullptr) {
    if (paletteBankIndex >= sourceAsset->tableDescriptor.paletteBankCount) {
      return;
    }
    /* the palette banks follow the asset header, 256 entries of 8 bytes each */
    region.palette =
        (const uint8_t *)sourceAsset + GFX_ASSET_HEADER_SIZE + paletteBankIndex * GFX_PALETTE_BANK_SIZE;
  }
  for (y = 0; y < region.height; y++) {
    const uint8_t *texel = region.texels + y * region.texelStride;
    uint32_t *pixel = (uint32_t *)(region.pixels + y * region.pixelStride);
    for (x = 0; x < region.width; x++, texel += region.texelBytes, pixel++) {
      uint32_t color = region.palette != nullptr ? Blit_PalettePixel(&region, *texel) : *(const uint32_t *)texel;
      if (Blit_IsTransparent(color)) {
        continue;
      }
      *pixel = Blit_IsOpaque(color) ? Blit_ConvertArgb(color) : Blit_BlendArgb32(color, *pixel);
    }
  }
}

/* Clips and adds one source subresource onto a four-byte framebuffer (saturated add). A texel whose RGB
   is 0 is skipped whatever its alpha; every other one is added byte by byte, clamped at 0xFF (Blit_AddArgb32).
   The alpha byte is summed and written as well. Quirk kept from the original: a paletted texel uses the entry's
   second dword (+4, the converted pixel), not its ARGB colour, both for the RGB-zero test and for the add.
   Always returns false.
*/
Bool8 SoftwareTextureSource_BlitSaturatedAddRgb32(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer)

{
  BlitRegion region;
  int x;
  int y;

  if (!Blit_SetupSubresource(sourceAsset, subresourceIndex, framebuffer, 4, drawX, drawY, clipMaxY, clipMaxX,
                             clipMinY, clipMinX, &region)) {
    return false;
  }
  for (y = 0; y < region.height; y++) {
    const uint8_t *texel = region.texels + y * region.texelStride;
    uint32_t *pixel = (uint32_t *)(region.pixels + y * region.pixelStride);
    for (x = 0; x < region.width; x++, texel += region.texelBytes, pixel++) {
      uint32_t argb = region.palette != nullptr ? Blit_PalettePixel(&region, *texel) : *(const uint32_t *)texel;
      if ((argb & ARGB8888_RGB_MASK) != 0) {
        *pixel = Blit_AddArgb32(argb, *pixel, 0);
      }
    }
  }
  return false;
}

/* Same as SoftwareTextureSource_BlitSaturatedAddRgb32, but the source lanes are halved (PSRLW 1 of
   c * 0x101) before the saturated add. The RGB-zero test uses the unhalved colour, and a paletted texel again
   uses the entry's second dword (+4). Always returns false.
*/
Bool8 SoftwareTextureSource_BlitHalfRgbSaturatedAdd32
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer)

{
  BlitRegion region;
  int x;
  int y;

  if (!Blit_SetupSubresource(sourceAsset, subresourceIndex, framebuffer, 4, drawX, drawY, clipMaxY, clipMaxX,
                             clipMinY, clipMinX, &region)) {
    return false;
  }
  for (y = 0; y < region.height; y++) {
    const uint8_t *texel = region.texels + y * region.texelStride;
    uint32_t *pixel = (uint32_t *)(region.pixels + y * region.pixelStride);
    for (x = 0; x < region.width; x++, texel += region.texelBytes, pixel++) {
      uint32_t argb = region.palette != nullptr ? Blit_PalettePixel(&region, *texel) : *(const uint32_t *)texel;
      if ((argb & ARGB8888_RGB_MASK) != 0) {
        *pixel = Blit_AddArgb32(argb, *pixel, 1);
      }
    }
  }
  return false;
}

/* Clips and draws one source subresource into a four-byte framebuffer, each source channel multiplied by the
   matching channel of modulationArgb8888 first (Blit_Modulate, see docs/software_raster.md "Blits"), then drawn
   with the source-alpha rules. Unlike
   BlitSourceAlpha32, a paletted texel uses the entry's ARGB colour (+0). Quirk: the modulated alpha is at most
   0xFE, so the opaque branch is never taken. Always returns false.
*/
Bool8 SoftwareTextureSource_BlitModulatedSourceAlpha32
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          PackedArgb32 modulationArgb8888,GraphicsSubresourceIndex subresourceIndex,
          GraphicsTextureSourceAsset *sourceAsset,SoftwareFramebufferAccess *framebuffer)

{
  BlitRegion region;
  int x;
  int y;

  if (!Blit_SetupSubresource(sourceAsset, subresourceIndex, framebuffer, 4, drawX, drawY, clipMaxY, clipMaxX,
                             clipMinY, clipMinX, &region)) {
    return false;
  }
  for (y = 0; y < region.height; y++) {
    const uint8_t *texel = region.texels + y * region.texelStride;
    uint32_t *pixel = (uint32_t *)(region.pixels + y * region.pixelStride);
    for (x = 0; x < region.width; x++, texel += region.texelBytes, pixel++) {
      uint32_t argb = Blit_Modulate(region.palette != nullptr ? Blit_PaletteColor(&region, *texel) : *(const uint32_t *)texel,
                                 modulationArgb8888);
      if (Blit_IsTransparent(argb)) {
        continue;
      }
      *pixel = Blit_IsOpaque(argb) ? Blit_ConvertArgb(argb) : Blit_BlendArgb32(argb, *pixel);
    }
  }
  return false;
}

/* Fills the intersection of [rectMinX, rectMaxX) x [rectMinY, rectMaxY), the framebuffer and the clip
   rectangle of a four-byte framebuffer with argb8888: alpha 0 draws nothing, alpha 0xFF writes the colour
   converted through g_SoftwarePixelPackTables, anything else is blended in 8-bit lanes (alpha lane included,
   see docs/software_raster.md "Blits").
*/
void SoftwareFramebuffer_FillRectArgb32(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate rectMaxY,GraphicsScreenCoordinate rectMaxX,
          GraphicsScreenCoordinate rectMinY,GraphicsScreenCoordinate rectMinX,PackedArgb32 argb8888,
          SoftwareFramebufferAccess *framebuffer)

{
  uint32_t opaque;
  int x;
  int y;

  if (framebuffer->bytesPerPixel != SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_32BIT ||
      !Blit_ClipRect(framebuffer, clipMaxY, clipMaxX, clipMinY, clipMinX, &rectMinX, &rectMinY, &rectMaxX,
                     &rectMaxY) ||
      Blit_IsTransparent(argb8888)) {
    return;
  }
  opaque = Blit_IsOpaque(argb8888) ? Blit_ConvertArgb(argb8888) : 0;
  for (y = rectMinY; y < rectMaxY; y++) {
    uint32_t *pixel = (uint32_t *)framebuffer->pixels + y * (int)framebuffer->width + rectMinX;
    for (x = rectMinX; x < rectMaxX; x++, pixel++) {
      *pixel = Blit_IsOpaque(argb8888) ? opaque : Blit_BlendArgb32(argb8888, *pixel);
    }
  }
}

/* Copies rowCount rows (> 0) of rowBytes bytes (> 0) from sourceRow to destRow, stepping each by its stride.
   Rows whose byte length is a multiple of 4 are copied in dwords, others byte by byte; both copy forwards. */
static void SoftwareFramebuffer_CopyRows(uint8_t *destRow,int destStrideBytes,const uint8_t *sourceRow,
          int sourceStrideBytes,uint32_t rowBytes,uint32_t rowCount)
{
  uint32_t i;

  if ((rowBytes & 3) != 0) {
    for (; rowCount != 0; rowCount--) {
      for (i = 0; i < rowBytes; i++) {
        destRow[i] = sourceRow[i];
      }
      sourceRow = sourceRow + sourceStrideBytes;
      destRow = destRow + destStrideBytes;
    }
  }
  else {
    for (; rowCount != 0; rowCount--) {
      for (i = 0; i < rowBytes >> 2; i++) {
        ((uint32_t *)destRow)[i] = ((const uint32_t *)sourceRow)[i];
      }
      sourceRow = sourceRow + sourceStrideBytes;
      destRow = destRow + destStrideBytes;
    }
  }
}

/* g_GraphicsFramebufferCopyRegionToOrigin: copies the copyWidth x copyHeight rectangle at
   (sourceX, sourceY) of source to the top-left corner of destination. Nothing is copied unless both have the
   same pixel size and destination is at least copyWidth x copyHeight. The rectangle is clipped to source; a
   negative source coordinate moves the destination start instead, so the copy stays aligned. Rows are copied in
   dwords when their byte length allows.
*/
void SoftwareFramebuffer_CopyRegionToOrigin(GraphicsPixelDimension copyHeight,GraphicsPixelDimension copyWidth,
          GraphicsScreenCoordinate sourceY,GraphicsScreenCoordinate sourceX,
          SoftwareFramebufferAccess *destination,SoftwareFramebufferAccess *source)

{
  SoftwareFramebufferPixelSize pixelBytes;
  int sourcePixelOffset;
  int destPixelOffset;
  int sourceStrideBytes;
  int destStrideBytes;
  uint32_t rowBytes;

  pixelBytes = source->bytesPerPixel;
  if (pixelBytes != destination->bytesPerPixel || (int)copyWidth > (int)destination->width ||
      (int)copyHeight > (int)destination->height) {
    return;
  }
  if (sourceY < 0) {
    sourcePixelOffset = 0;
    destPixelOffset = destination->width * -sourceY;
    copyHeight = copyHeight + sourceY;
    sourceY = 0;
  }
  else {
    destPixelOffset = 0;
    sourcePixelOffset = source->width * sourceY;
  }
  if (sourceX < 0) {
    destPixelOffset = destPixelOffset - sourceX;
    copyWidth = copyWidth + sourceX;
    sourceX = 0;
  }
  else {
    sourcePixelOffset = sourcePixelOffset + sourceX;
  }
  if ((int)source->width < (int)(sourceX + copyWidth)) {
    copyWidth = copyWidth - ((sourceX + copyWidth) - source->width);
  }
  if ((int)source->height < (int)(sourceY + copyHeight)) {
    copyHeight = copyHeight - ((sourceY + copyHeight) - source->height);
  }
  sourceStrideBytes = source->width * pixelBytes;
  destStrideBytes = destination->width * pixelBytes;
  rowBytes = pixelBytes * copyWidth;
  if ((0 < (int)copyHeight) && (0 < (int)rowBytes)) {
    SoftwareFramebuffer_CopyRows(destination->pixels + destPixelOffset * pixelBytes, destStrideBytes,
                                 source->pixels + sourcePixelOffset * pixelBytes, sourceStrideBytes, rowBytes,
                                 copyHeight);
  }
}

/* g_GraphicsFramebufferCopyOriginToRegion, the reverse of SoftwareFramebuffer_CopyRegionToOrigin:
   copies the copyWidth x copyHeight rectangle at the top-left corner of source to (destinationX, destinationY) of
   destination. Nothing is copied unless both have the same pixel size and source is at least copyWidth x
   copyHeight. The rectangle is clipped to destination; a negative destination coordinate moves the source start
   instead. Rows are copied in dwords when their byte length allows.
*/
void SoftwareFramebuffer_CopyOriginToRegion(GraphicsPixelDimension copyHeight,GraphicsPixelDimension copyWidth,
          GraphicsScreenCoordinate destinationY,GraphicsScreenCoordinate destinationX,
          SoftwareFramebufferAccess *source,SoftwareFramebufferAccess *destination)

{
  SoftwareFramebufferPixelSize pixelBytes;
  int destPixelOffset;
  int sourcePixelOffset;
  int destStrideBytes;
  int sourceStrideBytes;
  uint32_t rowBytes;

  pixelBytes = destination->bytesPerPixel;
  if (pixelBytes != source->bytesPerPixel || (int)copyWidth > (int)source->width ||
      (int)copyHeight > (int)source->height) {
    return;
  }
  if (destinationY < 0) {
    destPixelOffset = 0;
    sourcePixelOffset = source->width * -destinationY;
    copyHeight = copyHeight + destinationY;
    destinationY = 0;
  }
  else {
    sourcePixelOffset = 0;
    destPixelOffset = destination->width * destinationY;
  }
  if (destinationX < 0) {
    sourcePixelOffset = sourcePixelOffset - destinationX;
    copyWidth = copyWidth + destinationX;
    destinationX = 0;
  }
  else {
    destPixelOffset = destPixelOffset + destinationX;
  }
  if ((int)destination->width < (int)(destinationX + copyWidth)) {
    copyWidth = copyWidth - ((destinationX + copyWidth) - destination->width);
  }
  if ((int)destination->height < (int)(destinationY + copyHeight)) {
    copyHeight = copyHeight - ((destinationY + copyHeight) - destination->height);
  }
  destStrideBytes = destination->width * pixelBytes;
  sourceStrideBytes = source->width * pixelBytes;
  rowBytes = pixelBytes * copyWidth;
  if ((0 < (int)copyHeight) && (0 < (int)rowBytes)) {
    SoftwareFramebuffer_CopyRows(destination->pixels + destPixelOffset * pixelBytes, destStrideBytes,
                                 source->pixels + sourcePixelOffset * pixelBytes, sourceStrideBytes, rowBytes,
                                 copyHeight);
  }
}
