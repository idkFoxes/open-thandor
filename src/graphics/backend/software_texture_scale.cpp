/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/backend/software_texture_scale.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* Bilinear down-scaling of texture subresources for the software renderer (texture quality setting). */

#include <thandor/graphics/backend/software_texture_scale.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include "software_raster.h"

/* Module data. */

static uint32_t g_SoftwarePixelIntensityToNativeColorLut256[256] = {0};

static const uint64_t g_SoftwareBlendUnityWordLanesQ14 = 0x4000400040004000ull;

/* One byte of the cross-fade in SoftwareTexture_BilinearBlendScaleSubresources: both images and the
   factor are widened to (c * 0x101) >> 2 (PUNPCKLBW + PSRLW 2), then
   (b * (unity - f) + a * f) >> 16 per product (PMULHW), >> 4 (PSRLW) and saturated (PACKUSWB). */
static uint8_t SoftwareTexture_CrossFadeByte(uint8_t a, uint8_t b, uint8_t factor, short unity)
{
    short wideA = (short)((a * COLOR_CHANNEL_TO_WORD_LANE) >> 2);
    short wideB = (short)((b * COLOR_CHANNEL_TO_WORD_LANE) >> 2);
    short wideFactor = (short)((factor * COLOR_CHANNEL_TO_WORD_LANE) >> 2);
    uint16_t sum = (uint16_t)(Raster_MulHigh(wideB, (short)(unity - wideFactor)) + Raster_MulHigh(wideA, wideFactor));
    sum = (uint16_t)(sum >> 4);
    return (uint8_t)(sum > ARGB8888_CHANNEL_MAX ? ARGB8888_CHANNEL_MAX : sum);
}

/* Fills g_SoftwarePixelIntensityToNativeColorLut256 with native grey pixels of the current
   framebuffer format. The table runs from white down to black: entry i is intensity 255 - i. */
static void SoftwareTexture_BuildIntensityLut(void)
{
    const SoftwarePixelFormatConfig *format = &g_SoftwarePixelFormatConfig;
    uint32_t entry;
    for (entry = 0; entry < 256; entry++) {
        uint32_t intensity = 255 - entry;
        g_SoftwarePixelIntensityToNativeColorLut256[entry] =
            ((intensity >> ((8 - format->redBitCount) & 31)) << (format->redShift & 31)) |
            ((intensity >> ((8 - format->greenBitCount) & 31)) << (format->greenShift & 31)) |
            ((intensity >> ((8 - format->blueBitCount) & 31)) << (format->blueShift & 31));
    }
}

/* Bilinear sample of an 8-bit image at column xFixed (8.8 fixed point) between `row` and the row
   below it. Horizontal: the two neighbours, widened like SoftwareTexture_CrossFadeByte, weighted by
   g_SoftwareBilinearPackedInterpolationWeights256[fraction] (PMADDWD, high half kept). Vertical:
   the two results times the first lane of the row weights (PMULHW), summed, >> 2, clamped to 255. */
static uint32_t SoftwareTexture_SampleIntensity(const uint8_t *row, uint32_t sourceWidth, uint32_t xFixed,
                                                short upperWeight, short lowerWeight)
{
    const short *weights = g_SoftwareBilinearPackedInterpolationWeights256[xFixed & 0xff];
    const uint8_t *upper = row + (xFixed >> 8);
    const uint8_t *lower = upper + sourceWidth;
    uint32_t upperSum = (uint32_t)(((upper[0] * COLOR_CHANNEL_TO_WORD_LANE) >> 2) * weights[0] + ((upper[1] * COLOR_CHANNEL_TO_WORD_LANE) >> 2) * weights[1]);
    uint32_t lowerSum = (uint32_t)(((lower[0] * COLOR_CHANNEL_TO_WORD_LANE) >> 2) * weights[0] + ((lower[1] * COLOR_CHANNEL_TO_WORD_LANE) >> 2) * weights[1]);
    uint16_t sum = (uint16_t)(Raster_MulHigh((short)(upperSum >> 16), upperWeight) +
                      Raster_MulHigh((short)(lowerSum >> 16), lowerWeight));
    uint32_t intensity = (uint32_t)(sum >> 2);
    return intensity > ARGB8888_CHANNEL_MAX ? ARGB8888_CHANNEL_MAX : intensity;
}

/* Draws the cross-fade of two 8-bit subresources of a texture source, scaled to
   destinationWidth x destinationHeight at (destinationLeft, destinationTop) of the software
   framebuffer (32 bit), as grey levels. Called by
   UiSoftwareTexturePreviewControl_DrawScaledTextureAndChildren (ui/controls/panels.cpp).
   1. blendedSourcePixels = per-pixel cross-fade of B (sourceSubresourceIndexB) to A through the
      factor image blendFactorPixels, eight pixels per step (SoftwareTexture_CrossFadeByte).
   2. g_SoftwarePixelIntensityToNativeColorLut256 is rebuilt for the current pixel format.
   3. Each destination pixel is a bilinear sample of the blended image (8.8 fixed-point steps
      (size - 1) * 256 / (destinationSize - 1)), looked up in that table.
   Nothing is drawn unless the asset is a texture source, both indices are valid and both entries
   are paletted (paletteIndex >= 0). Original quirks kept: B's size is compared with itself, so A
   is assumed to be as large as B; the loops are do-while, so fewer than 8 source pixels or a zero
   destination size run 2^32 times, and a destination size of 1 divides by zero; the scale reads
   one row below the blended image.
*/
void SoftwareTexture_BilinearBlendScaleSubresources
          (GraphicsPixelDimension destinationHeight,GraphicsPixelDimension destinationWidth,
          GraphicsScreenCoordinate destinationTop,GraphicsScreenCoordinate destinationLeft,
          uint64_t *blendedSourcePixels,uint64_t *blendFactorPixels,
          GraphicsSubresourceIndex sourceSubresourceIndexA,
          GraphicsSubresourceIndex sourceSubresourceIndexB,int *graphicsTextureAsset,
          int *framebufferAccess)
{
  const GraphicsTextureSourceAsset *asset = (const GraphicsTextureSourceAsset *)graphicsTextureAsset;
  const SoftwareFramebufferAccess *framebuffer = (const SoftwareFramebufferAccess *)framebufferAccess;
  const short *unity = (const short *)&g_SoftwareBlendUnityWordLanesQ14;
  const GraphicsTextureSourceEntry *entries;
  const GraphicsTextureSourceEntry *entryA;
  const GraphicsTextureSourceEntry *entryB;
  const uint8_t *sourceA;
  const uint8_t *sourceB;
  const uint8_t *factor;
  uint8_t *blended;
  uint8_t *destinationRow;
  uint32_t sourceWidth;
  uint32_t sourceHeight;
  uint32_t blocks;
  uint32_t stepX;
  uint32_t stepY;
  uint32_t yFixed;
  uint32_t rowsLeft;
  int lane;

  if (asset == nullptr || asset->common.magic != ASSET_MAGIC_GFX ||
      sourceSubresourceIndexB >= asset->tableDescriptor.subresourceCount ||
      sourceSubresourceIndexA >= asset->tableDescriptor.subresourceCount) {
    return;
  }
  entries = (const GraphicsTextureSourceEntry *)((const uint8_t *)asset +
                                                 asset->tableDescriptor.subresourceTableOffset);
  entryA = &entries[sourceSubresourceIndexA];
  entryB = &entries[sourceSubresourceIndexB];
  if (entryB->paletteIndex < 0 || entryA->paletteIndex < 0) {
    return;
  }
  sourceWidth = entryB->pixelWidth;
  sourceHeight = entryB->pixelHeight;

  /* 1. cross-fade B -> A */
  sourceA = (const uint8_t *)asset + entryA->dataOffset;
  sourceB = (const uint8_t *)asset + entryB->dataOffset;
  factor = (const uint8_t *)blendFactorPixels;
  blended = (uint8_t *)blendedSourcePixels;
  blocks = (sourceHeight * sourceWidth) >> 3;
  do {
    for (lane = 0; lane < 8; lane++) {
      blended[lane] = SoftwareTexture_CrossFadeByte(sourceA[lane], sourceB[lane], factor[lane], unity[lane & 3]);
    }
    sourceA += 8;
    sourceB += 8;
    factor += 8;
    blended += 8;
  } while (--blocks != 0);

  /* 2. grey levels of the current pixel format */
  SoftwareTexture_BuildIntensityLut();

  /* 3. bilinear scale into the framebuffer */
  stepX = (uint32_t)(((unsigned long long)(sourceWidth - 1) << 8) / (uint32_t)(destinationWidth - 1));
  stepY = (uint32_t)(((unsigned long long)(sourceHeight - 1) << 8) / (uint32_t)(destinationHeight - 1));
  /* framebuffer->width is the row pitch in pixels; 4 bytes per pixel (the original also drew 2-byte pixels) */
  destinationRow = framebuffer->pixels + (destinationTop * (int)framebuffer->width + destinationLeft) * 4;
  yFixed = 0;
  rowsLeft = destinationHeight;
  do {
    const uint8_t *row = (const uint8_t *)blendedSourcePixels + (yFixed >> 8) * sourceWidth;
    short upperWeight = (short)g_SoftwareBilinearInverseFactors[yFixed & 0xff].blue;
    short lowerWeight = (short)g_SoftwareBilinearForwardFactors[yFixed & 0xff].blue;
    uint32_t xFixed = 0;
    uint32_t column = 0;
    do {
      uint32_t color = g_SoftwarePixelIntensityToNativeColorLut256[
          SoftwareTexture_SampleIntensity(row, sourceWidth, xFixed, upperWeight, lowerWeight)];
      ((uint32_t *)destinationRow)[column] = color;
      xFixed += stepX;
    } while (++column != destinationWidth);
    yFixed += stepY;
    destinationRow += (int)framebuffer->width * 4;
  } while (--rowsLeft != 0);
}
