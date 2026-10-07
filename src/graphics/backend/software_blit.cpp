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

/* Clips and draws one source subresource into a four-byte framebuffer (source-alpha blit, see
   docs/software_raster.md "Blits"). Alpha 0 is skipped, alpha 0xFF is converted through
   g_SoftwarePixelPackTables and written, anything else is blended in 8-bit lanes. A
   paletted texel uses the entry's second dword (+4) for everything: the alpha test, the blend colour, and the
   opaque write, which converts it through the pack tables again. Always returns false.
*/
bool SoftwareTextureSource_BlitSourceAlpha32(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
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
    uint32_t *pixel = reinterpret_cast<uint32_t *>(region.pixels + y * region.pixelStride); /* ARGB row */
    for (x = 0; x < region.width; x++, texel += region.texelBytes, pixel++) {
      uint32_t color = region.palette != nullptr ? Blit_PalettePixel(&region, *texel) : Thandor_LoadU32(texel);
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
bool SoftwareTextureSource_BlitHalfSourceRgb32(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
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
    uint32_t *pixel = reinterpret_cast<uint32_t *>(region.pixels + y * region.pixelStride); /* ARGB row */
    for (x = 0; x < region.width; x++, texel += region.texelBytes, pixel++) {
      uint32_t color = region.palette != nullptr ? Blit_PalettePixel(&region, *texel) : Thandor_LoadU32(texel);
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
  /* both tables read flat: row f holds the four word lanes at [f * 4 + lane] */
  const short *firstWeights = reinterpret_cast<const short *>(g_SoftwareBilinearInverseFactors);
  const short *secondWeights = reinterpret_cast<const short *>(g_SoftwareBilinearForwardFactors);
  const unsigned long long clampMask = g_SoftwareBilinearPackedByteClampMask;
  uint8_t *asset = GraphicsTextureSource_Bytes(sourceAsset);
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
  entry = GraphicsTextureSource_Entries(sourceAsset) + subresourceIndex;
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
  destinationRow = SoftwareFramebuffer_Pixels32(framebuffer) + (int32_t)(destinationY * pitchPixels + destinationX);
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
      Thandor_StoreU64(out, ((((unsigned long long)pixels[0] << 32) | pixels[0]) & clampMask) |
                                ((unsigned long long)pixels[1] << 32));
      out += 2;
    }
    destinationRow = destinationRow + pitchPixels;
    fy = fy + stepY;
    sourceRow = sourceBase + (fy >> 8) * sourceWidth * 4;
  }
}

/* Clips and draws one source subresource into a four-byte framebuffer, each source channel multiplied by the
   matching channel of modulationArgb8888 first (Blit_Modulate, see docs/software_raster.md "Blits"), then drawn
   with the source-alpha rules. Unlike
   BlitSourceAlpha32, a paletted texel uses the entry's ARGB colour (+0). Quirk: the modulated alpha is at most
   0xFE, so the opaque branch is never taken. Always returns false.
*/
bool SoftwareTextureSource_BlitModulatedSourceAlpha32
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
    uint32_t *pixel = reinterpret_cast<uint32_t *>(region.pixels + y * region.pixelStride); /* ARGB row */
    for (x = 0; x < region.width; x++, texel += region.texelBytes, pixel++) {
      uint32_t argb = Blit_Modulate(region.palette != nullptr ? Blit_PaletteColor(&region, *texel) : Thandor_LoadU32(texel),
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
    uint32_t *pixel = SoftwareFramebuffer_Pixels32(framebuffer) + y * (int)framebuffer->width + rectMinX;
    for (x = rectMinX; x < rectMaxX; x++, pixel++) {
      *pixel = Blit_IsOpaque(argb8888) ? opaque : Blit_BlendArgb32(argb8888, *pixel);
    }
  }
}

/* Software implementation of g_GraphicsFillColumnSegments (the pixel loop of the results graph columns,
   ui/frontend/results.cpp): from (drawX, topY) downwards, segmentHeights[i] pixels of packedColors[i] for each
   segment whose height is not 0 (a do-while per segment, as in the original). No clipping; the row pitch is
   framebuffer->width pixels. */
void SoftwareFramebuffer_FillColumnSegments32(GraphicsScreenCoordinate topY,GraphicsScreenCoordinate drawX,
          uint32_t segmentCount,const int32_t *segmentHeights,const uint32_t *packedColors,
          SoftwareFramebufferAccess *framebuffer)

{
  uint8_t *pixelCursor;
  uint32_t strideBytes;
  uint32_t segment;
  int32_t remaining;

  strideBytes = framebuffer->width * 4;
  pixelCursor = framebuffer->pixels + (int32_t)((topY * framebuffer->width + drawX) * 4);
  for (segment = 0; segment < segmentCount; segment++) {
    remaining = segmentHeights[segment];
    if (remaining != 0) {
      for (; remaining != 0; remaining--) {
        Thandor_StoreU32(pixelCursor, packedColors[segment]);
        pixelCursor = pixelCursor + strideBytes;
      }
    }
  }
}
