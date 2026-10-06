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
#include <thandor/graphics/resources/texture.h>
#include "software_raster.h"

/* Module data. */

static uint32_t g_SoftwarePixelIntensityToNativeColorLut256[256] = {};

/* 1.0 in Q14 in each of the four word lanes (the original's 0x4000400040004000 MMX constant) */
static const short g_SoftwareBlendUnityWordLanesQ14[4] = {0x4000, 0x4000, 0x4000, 0x4000};

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
static void SoftwareTexture_BuildIntensityLut()
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

/* Step 1 of SoftwareTexture_BilinearBlendScaleSubresources, on its own for the GPU draw list (graphics/core/draw2d.cpp
   draws the scale on the GPU): blendedSourcePixels = per-pixel cross-fade of subresource B to A through the factor
   image blendFactorPixels, eight pixels per step (SoftwareTexture_CrossFadeByte), over B's pixel count rounded down
   to a multiple of 8 (do-while: fewer than 8 pixels run 2^32 times, as in the original). The caller checks the
   asset and that both entries are paletted. */
void SoftwareTexture_CrossFadeSubresources
          (uint64_t *blendedSourcePixels,uint64_t *blendFactorPixels,
          GraphicsSubresourceIndex sourceSubresourceIndexA,GraphicsSubresourceIndex sourceSubresourceIndexB,
          const GraphicsTextureSourceAsset *asset)
{
  const short *unity = g_SoftwareBlendUnityWordLanesQ14;
  const GraphicsTextureSourceEntry *entries = GraphicsTextureSource_Entries(asset);
  const GraphicsTextureSourceEntry *entryA = &entries[sourceSubresourceIndexA];
  const GraphicsTextureSourceEntry *entryB = &entries[sourceSubresourceIndexB];
  const uint8_t *sourceA = GraphicsTextureSource_Bytes(asset) + entryA->dataOffset;
  const uint8_t *sourceB = GraphicsTextureSource_Bytes(asset) + entryB->dataOffset;
  /* both images are 8-bit pixels, handled in blocks of eight (one uint64_t each) */
  const uint8_t *factor = reinterpret_cast<const uint8_t *>(blendFactorPixels);
  uint8_t *blended = reinterpret_cast<uint8_t *>(blendedSourcePixels);
  uint32_t blocks = (entryB->pixelHeight * entryB->pixelWidth) >> 3;
  int lane;

  do {
    for (lane = 0; lane < 8; lane++) {
      blended[lane] = SoftwareTexture_CrossFadeByte(sourceA[lane], sourceB[lane], factor[lane], unity[lane & 3]);
    }
    sourceA += 8;
    sourceB += 8;
    factor += 8;
    blended += 8;
  } while (--blocks != 0);
}

/* Draws the cross-fade of two 8-bit subresources of a texture source, scaled to
   destinationWidth x destinationHeight at (destinationLeft, destinationTop) of the software
   framebuffer (32 bit), as grey levels. Called by
   UiSoftwareTexturePreviewControl_DrawScaledTextureAndChildren (ui/controls/panels.cpp).
   1. blendedSourcePixels = per-pixel cross-fade of B (sourceSubresourceIndexB) to A through the
      factor image blendFactorPixels, eight pixels per step (SoftwareTexture_CrossFadeSubresources).
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
  /* the A3 slot type (GraphicsGreyScaleImageProc) passes the asset and the framebuffer as int * */
  const GraphicsTextureSourceAsset *asset = reinterpret_cast<const GraphicsTextureSourceAsset *>(graphicsTextureAsset);
  const SoftwareFramebufferAccess *framebuffer = reinterpret_cast<const SoftwareFramebufferAccess *>(framebufferAccess);
  const GraphicsTextureSourceEntry *entries;
  const GraphicsTextureSourceEntry *entryA;
  const GraphicsTextureSourceEntry *entryB;
  uint8_t *destinationRow;
  uint32_t sourceWidth;
  uint32_t sourceHeight;
  uint32_t stepX;
  uint32_t stepY;
  uint32_t yFixed;
  uint32_t rowsLeft;

  if (asset == nullptr || asset->common.magic != ASSET_MAGIC_GFX ||
      sourceSubresourceIndexB >= asset->tableDescriptor.subresourceCount ||
      sourceSubresourceIndexA >= asset->tableDescriptor.subresourceCount) {
    return;
  }
  entries = GraphicsTextureSource_Entries(asset);
  entryA = &entries[sourceSubresourceIndexA];
  entryB = &entries[sourceSubresourceIndexB];
  if (entryB->paletteIndex < 0 || entryA->paletteIndex < 0) {
    return;
  }
  sourceWidth = entryB->pixelWidth;
  sourceHeight = entryB->pixelHeight;

  /* 1. cross-fade B -> A */
  SoftwareTexture_CrossFadeSubresources(blendedSourcePixels, blendFactorPixels, sourceSubresourceIndexA,
                                        sourceSubresourceIndexB, asset);

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
    const uint8_t *row = reinterpret_cast<const uint8_t *>(blendedSourcePixels) + (yFixed >> 8) * sourceWidth;
    short upperWeight = (short)g_SoftwareBilinearInverseFactors[yFixed & 0xff].blue;
    short lowerWeight = (short)g_SoftwareBilinearForwardFactors[yFixed & 0xff].blue;
    uint32_t xFixed = 0;
    uint32_t column = 0;
    do {
      uint32_t color = g_SoftwarePixelIntensityToNativeColorLut256[
          SoftwareTexture_SampleIntensity(row, sourceWidth, xFixed, upperWeight, lowerWeight)];
      Thandor_StoreU32(destinationRow + static_cast<size_t>(column) * 4u, color);
      xFixed += stepX;
    } while (++column != destinationWidth);
    yFixed += stepY;
    destinationRow += (int)framebuffer->width * 4;
  } while (--rowsLeft != 0);
}

/* ---- The minimap (A1): rotated bilinear scaler, moved from ui/controls/minimap.cpp ---- */

/* Weights of the bilinear scaler below, indexed by the 8-bit fraction between two source pixels: the first
   pixel's weight is g_SoftwareMinimapFirstPixelWeights, the second's g_SoftwareMinimapSecondPixelWeights, all four lanes
   equal. Precomputed tables in the original. */
static SoftwareBgraWordLanes g_SoftwareMinimapFirstPixelWeights[256];
static SoftwareBgraWordLanes g_SoftwareMinimapSecondPixelWeights[256];

/* Not in the original (it carried the tables precomputed): builds the scaler weights. Fractions 0..63 take
   only the first pixel (0x4000), 64..191 blend in steps t = 2 * (fraction - 64) of 0x4040 / 256 (the pair
   sums to 0x4040 or 0x403F, not 0x4000), 192..255 take only the second pixel (0x4000). This reproduces every
   entry of the original tables. Called once at startup. */
void SoftwareMinimap_BuildPixelWeightTables()
{
  int fraction;
  int blendStep;
  uint16_t firstWeight;
  uint16_t secondWeight;

  for (fraction = 0; fraction < 256; fraction++) {
    if (fraction < 64) {
      firstWeight = 0x4000;
      secondWeight = 0;
    }
    else if (fraction < 192) {
      blendStep = (fraction - 64) * 2;
      firstWeight = (uint16_t)(((256 - blendStep) * 0x4040) >> 8);
      secondWeight = (uint16_t)((blendStep * 0x4040) >> 8);
    }
    else {
      firstWeight = 0;
      secondWeight = 0x4000;
    }
    g_SoftwareMinimapFirstPixelWeights[fraction].blue = firstWeight;
    g_SoftwareMinimapFirstPixelWeights[fraction].green = firstWeight;
    g_SoftwareMinimapFirstPixelWeights[fraction].red = firstWeight;
    g_SoftwareMinimapFirstPixelWeights[fraction].alpha = firstWeight;
    g_SoftwareMinimapSecondPixelWeights[fraction].blue = secondWeight;
    g_SoftwareMinimapSecondPixelWeights[fraction].green = secondWeight;
    g_SoftwareMinimapSecondPixelWeights[fraction].red = secondWeight;
    g_SoftwareMinimapSecondPixelWeights[fraction].alpha = secondWeight;
  }
}

/* MMX lane helpers for the minimap scaler below (lanes are little-endian 16-bit words). */

/* PUNPCKLBW mm,mm then PSRLW mm,shift: byte i of pixel becomes word lane i = (byte * 0x101) >> shift. */
static inline uint64_t SoftwareMinimap_UnpackBytesToWordLanes(uint32_t pixel,int shift) {
  uint64_t lanes;
  int lane;

  lanes = 0;
  for (lane = 0; lane < 4; lane++) {
    lanes = lanes |
            (uint64_t)(uint16_t)((uint16_t)(((pixel >> (lane * 8)) & 0xff) * COLOR_CHANNEL_TO_WORD_LANE) >> shift) << (lane * 16);
  }
  return lanes;
}

/* PADDW: lane-wise wrapping 16-bit add. */
static inline uint64_t SoftwareMinimap_AddWordLanes(uint64_t left,uint64_t right) {
  uint64_t sum;
  int shift;

  sum = 0;
  for (shift = 0; shift < 64; shift += 16) {
    sum = sum | (uint64_t)(uint16_t)((uint16_t)(left >> shift) + (uint16_t)(right >> shift)) << shift;
  }
  return sum;
}

/* PSRLW mm,shift then PACKUSWB (low dword): each lane shifted right, saturated to an unsigned
   byte. The logical shift leaves every lane non-negative, so only the 0xFF clamp applies. */
static inline uint32_t SoftwareMinimap_ShiftAndPackWordLanes(uint64_t lanes,int shift) {
  uint32_t packed;
  uint16_t laneValue;
  int lane;

  packed = 0;
  for (lane = 0; lane < 4; lane++) {
    laneValue = (uint16_t)(lanes >> (lane * 16)) >> shift;
    packed = packed | (uint32_t)(0xff < laneValue ? 0xff : laneValue) << (lane * 8);
  }
  return packed;
}

/* Bilinear blend of a 2x2 texel quad with the scaler weight tables (256 steps per axis):
   rows blended across the column fraction, then across the row fraction, then >> 2 and packed. */
static inline PackedArgb32 SoftwareMinimap_BlendBilinear
          (PackedArgb32 topLeft,PackedArgb32 topRight,PackedArgb32 bottomLeft,PackedArgb32 bottomRight,
          int columnWeight,int rowWeight) {
  uint64_t topRow;
  uint64_t bottomRow;

  topRow = SoftwareMinimap_AddWordLanes
                     (pmulhw(SoftwareMinimap_UnpackBytesToWordLanes(topLeft,2),
                             g_SoftwareMinimapFirstPixelWeights[columnWeight]),
                      pmulhw(SoftwareMinimap_UnpackBytesToWordLanes(topRight,2),
                             g_SoftwareMinimapSecondPixelWeights[columnWeight]));
  bottomRow = SoftwareMinimap_AddWordLanes
                        (pmulhw(SoftwareMinimap_UnpackBytesToWordLanes(bottomLeft,2),
                                g_SoftwareMinimapFirstPixelWeights[columnWeight]),
                         pmulhw(SoftwareMinimap_UnpackBytesToWordLanes(bottomRight,2),
                                g_SoftwareMinimapSecondPixelWeights[columnWeight]));
  return SoftwareMinimap_ShiftAndPackWordLanes
                   (SoftwareMinimap_AddWordLanes(pmulhw(topRow,g_SoftwareMinimapFirstPixelWeights[rowWeight]),
                                          pmulhw(bottomRow,g_SoftwareMinimapSecondPixelWeights[rowWeight])),2);
}

/* One screen pixel of SoftwareTexture_DrawMinimapBilinear32: the 2x2 texels around the Q12 source position
   (sourceU, sourceV) of a sourceWidth x sourceHeight 32-bit texture (pixel data at pixelDataOffset from the
   asset; texels outside it count as 0), bilinearly blended by the position's fractions. */
static PackedArgb32 SoftwareMinimap_SampleBilinear
          (GraphicsTextureSourceAsset *sourceTexture,int pixelDataOffset,int sourceWidth,int sourceHeight,
          uint32_t sourceU,uint32_t sourceV)
{
  int sourceRow;
  int sourceColumn;
  int nextColumn;
  int texelIndex;
  PackedArgb32 sourcePixelSample0; /* texel (column, row) */
  PackedArgb32 sourcePixelSample1; /* texel (column + 1, row) */
  PackedArgb32 sourcePixelSample2; /* texel (column, row + 1) */
  PackedArgb32 sourcePixelSample3; /* texel (column + 1, row + 1) */

  sourcePixelSample0 = 0;
  sourcePixelSample1 = 0;
  sourcePixelSample2 = 0;
  sourcePixelSample3 = 0;
  sourceRow = (int)sourceV >> Q12_SHIFT;
  sourceColumn = (int)sourceU >> Q12_SHIFT;
  texelIndex = sourceWidth * sourceRow + sourceColumn;
  nextColumn = sourceColumn + 1;
  if (sourceRow < sourceHeight) {
    if ((-1 < sourceRow) && (sourceColumn < sourceWidth)) {
      if (-1 < sourceColumn) {
        sourcePixelSample0 =
             Thandor_LoadU32(GraphicsTextureSource_Bytes(sourceTexture) + texelIndex * 4 + pixelDataOffset);
      }
      if ((-1 < nextColumn) && (nextColumn < sourceWidth)) {
        sourcePixelSample1 =
             Thandor_LoadU32(GraphicsTextureSource_Bytes(sourceTexture) + texelIndex * 4 + pixelDataOffset + 4);
      }
    }
    if (((-1 < sourceRow + 1) && (sourceRow + 1 < sourceHeight)) && (sourceColumn < sourceWidth)) {
      if (-1 < sourceColumn) {
        sourcePixelSample2 =
             Thandor_LoadU32(GraphicsTextureSource_Bytes(sourceTexture) + (texelIndex + sourceWidth) * 4 + pixelDataOffset);
      }
      if ((-1 < nextColumn) && (nextColumn < sourceWidth)) {
        sourcePixelSample3 =
             Thandor_LoadU32(GraphicsTextureSource_Bytes(sourceTexture) + (texelIndex + sourceWidth) * 4 + pixelDataOffset + 4);
      }
    }
  }
  return SoftwareMinimap_BlendBilinear(sourcePixelSample0,sourcePixelSample1,sourcePixelSample2,sourcePixelSample3,
                                (int)(sourceU & Q12_FRACTION_MASK) >> 4,(int)(sourceV & Q12_FRACTION_MASK) >> 4);
}

/* Software implementation of g_GraphicsMinimapDraw (the pixel loop of UiSelectionGeometryControl_DrawClipped):
   width x height opaque pixels at (destX, destY) of a 32-bit framebuffer, each a bilinear sample of
   subresource 0 (ARGB8888 texels; outside it 0) at the Q12 position that starts at (startU, startV) and
   advances by the pixel step along a row and by the row step from row to row. The loops are do-while, as in the
   original (the caller never passes a size of 0). The row pitch is framebuffer->width pixels. */
void SoftwareTexture_DrawMinimapBilinear32
          (int32_t destY,int32_t destX,int32_t height,int32_t width,uint32_t startU,uint32_t startV,
          uint32_t pixelStepU,uint32_t pixelStepV,uint32_t rowStepU,uint32_t rowStepV,
          GraphicsTextureSourceAsset *texture,SoftwareFramebufferAccess *framebuffer)
{
  const GraphicsTextureSourceEntry *firstRecord;
  int sourceWidth;
  int sourceHeight;
  int pixelDataOffset;
  uint32_t rowStrideBytes;
  uint32_t sourceU;
  uint32_t sourceV;
  uint32_t rowStartU;
  uint32_t rowStartV;
  uint8_t *destPixel;
  uint8_t *destRowStart;
  int remainingColumns;
  int remainingRows;

  /* fields of the first subresource record (asset + subresourceTableOffset) */
  firstRecord = GraphicsTextureSource_Entries(texture);
  sourceWidth = static_cast<int>(firstRecord->pixelWidth);
  sourceHeight = static_cast<int>(firstRecord->pixelHeight);
  pixelDataOffset = static_cast<int>(firstRecord->dataOffset);
  rowStrideBytes = framebuffer->width * 4;
  /* the original also had a 16-bit framebuffer path (packed through the 565/555 MMX constants) */
  destPixel = framebuffer->pixels + (int32_t)(rowStrideBytes * destY) + destX * 4;
  sourceU = startU;
  sourceV = startV;
  rowStartU = sourceU;
  rowStartV = sourceV;
  destRowStart = destPixel;
  remainingRows = height;
  remainingColumns = width;
  do {
    do {
      Thandor_StoreU32(destPixel,
           SoftwareMinimap_SampleBilinear(texture,pixelDataOffset,sourceWidth,sourceHeight,sourceU,sourceV));
      sourceU = sourceU + pixelStepU;
      sourceV = sourceV + pixelStepV;
      destPixel = destPixel + 4;
      remainingColumns = remainingColumns - 1;
    } while (remainingColumns != 0);
    sourceU = rowStartU + rowStepU;
    sourceV = rowStartV + rowStepV;
    destPixel = destRowStart + rowStrideBytes;
    remainingRows = remainingRows - 1;
    rowStartU = sourceU;
    rowStartV = sourceV;
    remainingColumns = width;
    destRowStart = destPixel;
  } while (remainingRows != 0);
}
