/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/credits_mask.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* The 8-bit reveal mask of the credits and scenario screen transitions (SoftwareMaskBuffer_*): pattern
   advance, circular, diagonal and band regions. Not a renderer part; its callers are in ui. */

#include <thandor/ui/frontend/credits_mask.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* One tick of the credits screen's reveal mask (called from the frontend tick in ui/frontend/scenario.c while
   the credits page is open): pixels already revealed brighten by 0x1F, and the tick's position in a 100-tick
   cycle grows one of the reveal shapes (circles, diagonal wipes, horizontal bands, or everything), step by
   step. At the start of each cycle the mask is cleared and the two pattern counters (capped at 13) advance.
*/
void SoftwareMaskBuffer_AdvancePatternByPercentTick(SoftwareMaskRuntimeView *maskRuntime)

{
  uint32_t cycleTicks;
  int previousTick;
  uint32_t shapeStep;
  UiBooleanState32 reverseRows;
  GraphicsTextureLogicalSize logicalSize;
  
  previousTick = maskRuntime->tickCounter;
  maskRuntime->tickCounter++;
  if (maskRuntime->maskPixels != nullptr) {
    SoftwareMaskBuffer_AdvanceNonzeroPixelsSaturating31(maskRuntime);
    cycleTicks = previousTick + 20;
    shapeStep = cycleTicks % 100;
    if (shapeStep == 0) {
      if (maskRuntime->incomingSubresource != 0) {
        maskRuntime->outgoingSubresource++;
      }
      maskRuntime->incomingSubresource++;
      SoftwareMaskBuffer_Clear(maskRuntime);
      if (13 < maskRuntime->outgoingSubresource) {
        maskRuntime->outgoingSubresource = 13;
      }
      if (13 < maskRuntime->incomingSubresource) {
        maskRuntime->incomingSubresource = 13;
      }
    }
    else {
      /* the shape of this cycle; centres are (y, x) in mask pixels */
      switch(cycleTicks / 100) {
      case 1:
        SoftwareMaskBuffer_ApplyCircularRegionBit(0,80,160,shapeStep,maskRuntime);
        break;
      case 2:
        logicalSize = g_GraphicsTextureSourceGetLogicalSize(0,maskRuntime->textureSource);
        SoftwareMaskBuffer_ApplyDiagonalHalfPlaneBit
                  (logicalSize.logicalWidthPixels + logicalSize.logicalHeightPixels,shapeStep,maskRuntime);
        break;
      case 3:
      case 7:
        SoftwareMaskBuffer_ApplyDiagonalHalfPlaneBit(0,shapeStep,maskRuntime);
        break;
      case 4:
      case 9:
        /* any nonzero height selects the reversed band order */
        logicalSize = g_GraphicsTextureSourceGetLogicalSize(0,maskRuntime->textureSource);
        reverseRows = logicalSize.logicalHeightPixels;
        SoftwareMaskBuffer_ApplyHorizontalBandBit(reverseRows,shapeStep,maskRuntime);
        break;
      case 5:
        SoftwareMaskBuffer_ApplyHorizontalBandBit(0,shapeStep,maskRuntime);
        break;
      case 6:
        SoftwareMaskBuffer_ApplyCircularRegionBit(0,280,160,shapeStep,maskRuntime);
        break;
      case 8:
        SoftwareMaskBuffer_ApplyCircularRegionBit(0,32,320,shapeStep,maskRuntime);
        break;
      case 10:
      case 13:
        SoftwareMaskBuffer_ApplyCircularRegionBit(0,180,320,shapeStep,maskRuntime);
        break;
      case 11:
        SoftwareMaskBuffer_ApplyCircularRegionBit(1,180,320,shapeStep,maskRuntime);
        break;
      case 12:
        SoftwareMaskBuffer_SetAllPixelsBit(maskRuntime);
      }
    }
  }
  return;
}

/* Zeroes the one-byte-per-pixel mask buffer of a software mask (if it has one), sized by the logical
   width x height of its texture source, 64 bytes per step (eight MMX qword stores).
*/
void SoftwareMaskBuffer_Clear(SoftwareMaskRuntimeView *maskControl)

{
  uint64_t *maskQwordWriteCursor;
  uint32_t blocksRemaining;
  GraphicsTextureLogicalSize logicalSize;

  maskQwordWriteCursor = (uint64_t *)maskControl->maskPixels;
  if (maskQwordWriteCursor != nullptr) {
    logicalSize = g_GraphicsTextureSourceGetLogicalSize(0,maskControl->textureSource);
    /* 64-byte blocks; a mask of fewer than 64 pixels would wrap the count, as in the original */
    blocksRemaining = logicalSize.logicalHeightPixels * logicalSize.logicalWidthPixels >> 6;
    do {
      *maskQwordWriteCursor = 0;
      maskQwordWriteCursor[1] = 0;
      maskQwordWriteCursor[2] = 0;
      maskQwordWriteCursor[3] = 0;
      maskQwordWriteCursor[4] = 0;
      maskQwordWriteCursor[5] = 0;
      maskQwordWriteCursor[6] = 0;
      maskQwordWriteCursor[7] = 0;
      maskQwordWriteCursor = maskQwordWriteCursor + 8;
      blocksRemaining = blocksRemaining - 1;
    } while (blocksRemaining != 0);
  }
  return;
}

/* Called by SoftwareMaskBuffer_AdvancePatternByPercentTick once per tick: adds 0x1F to every nonzero byte of
   the software mask, saturating at 0xFF (PCMPEQB / PAND / PXOR / PADDUSB); zero bytes stay zero. The mask size
   is taken from g_GraphicsTextureSourceGetLogicalSize, and the buffer is processed in 32-byte blocks,
   width * height >> 5 of them (the remainder is left alone). Quirk kept:
   the block counter is a do-while loop, so fewer than 32 pixels means 2^32 blocks. Nothing happens when
   maskPixels is NULL.
*/
void SoftwareMaskBuffer_AdvanceNonzeroPixelsSaturating31(SoftwareMaskRuntimeView *maskRuntime)

{
  uint8_t *mask;
  uint32_t blocksLeft;
  GraphicsTextureLogicalSize logicalSize;
  int i;

  mask = maskRuntime->maskPixels;
  if (mask == nullptr) {
    return;
  }
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(0,maskRuntime->textureSource);
  blocksLeft = logicalSize.logicalHeightPixels * logicalSize.logicalWidthPixels >> 5;
  do {
    for (i = 0; i < 32; i++) {
      if (mask[i] != 0) {
        mask[i] = (uint8_t)(mask[i] < ARGB8888_CHANNEL_MAX - SOFTWARE_MASK_BRIGHTEN_STEP ? mask[i] + SOFTWARE_MASK_BRIGHTEN_STEP : ARGB8888_CHANNEL_MAX);
      }
    }
    mask += 32;
  } while (--blocksLeft != 0);
}

/* Reveal shape of SoftwareMaskBuffer_AdvancePatternByPercentTick: sets bit 0 of every mask pixel inside the circle
   of radius radiusStep * 28 around (centerX, centerY), i.e. a circle growing with the step. With invertSelection
   the radius is (width + height) - radiusStep * 28 (at least 0) and the pixels outside it are set, a shrinking
   hole. Distances are compared squared and unsigned.
*/
void SoftwareMaskBuffer_ApplyCircularRegionBit(UiBooleanState32 invertSelection,GraphicsScreenCoordinate centerY,
          GraphicsScreenCoordinate centerX,SoftwareMaskRadiusStep radiusStep,
          SoftwareMaskRuntimeView *maskRuntime)

{
  uint32_t maskWidth;
  int radiusPixels;
  uint32_t radiusSquared;
  int rowDistanceSquared;
  uint32_t distanceSquared;
  uint32_t rowsRemaining;
  uint32_t columnX;
  uint8_t *maskCursor;
  GraphicsTextureLogicalSize logicalSize;
  int rowY;
  Bool8 selected;

  logicalSize = g_GraphicsTextureSourceGetLogicalSize(0,maskRuntime->textureSource);
  rowsRemaining = logicalSize.logicalHeightPixels;
  maskWidth = logicalSize.logicalWidthPixels;
  maskCursor = maskRuntime->maskPixels;
  if (invertSelection != 0) {
    radiusPixels = radiusStep * -28 + maskWidth + rowsRemaining;
    if (radiusPixels < 0) {
      radiusPixels = 0;
    }
  }
  else {
    radiusPixels = radiusStep * 28;
  }
  radiusSquared = (uint32_t)(radiusPixels * radiusPixels);
  /* do-whiles kept: a zero width still visits one pixel per row, a zero height wraps the row counter */
  rowY = 0;
  do {
    rowDistanceSquared = (rowY - centerY) * (rowY - centerY);
    columnX = 0;
    do {
      distanceSquared = (columnX - centerX) * (columnX - centerX) + rowDistanceSquared;
      /* both tests include the circle's edge */
      if (invertSelection == 0) {
        selected = distanceSquared <= radiusSquared;
      }
      else {
        selected = radiusSquared <= distanceSquared;
      }
      if (selected) {
        *maskCursor = *maskCursor | 1;
      }
      columnX++;
      maskCursor++;
    } while (columnX < maskWidth);
    rowY++;
    rowsRemaining--;
  } while (rowsRemaining != 0);
}

/* Reveal shape of SoftwareMaskBuffer_AdvancePatternByPercentTick: a diagonal wipe. Sets bit 0 of every mask pixel
   with x + y < thresholdStep * 40 (from the top-left corner), or with invertSelection every pixel with
   x + y > (width + height) - thresholdStep * 40 (from the bottom-right corner).
*/
void SoftwareMaskBuffer_ApplyDiagonalHalfPlaneBit
          (UiBooleanState32 invertSelection,SoftwareMaskThresholdStep thresholdStep,
          SoftwareMaskRuntimeView *maskRuntime)

{
  uint32_t maskWidth;
  uint32_t columnsRemaining;
  int thresholdSum;
  int rowY;
  uint32_t rowsRemaining;
  int diagonalSum;
  uint8_t *maskCursor;
  GraphicsTextureLogicalSize logicalSize;
  
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(0,maskRuntime->textureSource);
  rowsRemaining = logicalSize.logicalHeightPixels;
  maskWidth = logicalSize.logicalWidthPixels;
  thresholdSum = thresholdStep * 40;
  maskCursor = maskRuntime->maskPixels;
  if (invertSelection != 0) {
    thresholdSum = thresholdStep * -40 + maskWidth + rowsRemaining;
  }
  /* diagonalSum is x + y of the current pixel */
  rowY = 0;
  columnsRemaining = maskWidth;
  diagonalSum = 0;
  if (invertSelection == 0) {
    do {
      do {
        if (diagonalSum < thresholdSum) {
          *maskCursor = *maskCursor | 1;
        }
        maskCursor++;
        columnsRemaining--;
        diagonalSum++;
      } while (columnsRemaining != 0);
      rowY++;
      rowsRemaining--;
      columnsRemaining = maskWidth;
      diagonalSum = rowY;
    } while (rowsRemaining != 0);
    return;
  }
  do {
    do {
      if (thresholdSum < diagonalSum) {
        *maskCursor = *maskCursor | 1;
      }
      maskCursor++;
      columnsRemaining--;
      diagonalSum++;
    } while (columnsRemaining != 0);
    rowY++;
    rowsRemaining--;
    columnsRemaining = maskWidth;
    diagonalSum = rowY;
  } while (rowsRemaining != 0);
  return;
}

/* Last reveal shape of SoftwareMaskBuffer_AdvancePatternByPercentTick: sets bit 0 of every mask pixel, 16 bytes
   per step (width * height >> 4 steps; the remainder is left alone). Quirk kept: a mask of fewer than 16 pixels
   makes the do-while counter wrap to 2^32 steps.
*/
void SoftwareMaskBuffer_SetAllPixelsBit(SoftwareMaskRuntimeView *maskControl)

{
  uint32_t maskBlocksRemaining;
  uint32_t *maskWordCursor;
  GraphicsTextureLogicalSize logicalSize;
  
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(0,maskControl->textureSource);
  maskWordCursor = (uint32_t *)maskControl->maskPixels;
  maskBlocksRemaining = logicalSize.logicalHeightPixels * logicalSize.logicalWidthPixels >> 4;
  do {
    *maskWordCursor = *maskWordCursor | ARGB8888_CHANNEL_ONES;
    maskWordCursor[1] = maskWordCursor[1] | ARGB8888_CHANNEL_ONES;
    maskWordCursor[2] = maskWordCursor[2] | ARGB8888_CHANNEL_ONES;
    maskWordCursor[3] = maskWordCursor[3] | ARGB8888_CHANNEL_ONES;
    maskWordCursor = maskWordCursor + 4;
    maskBlocksRemaining--;
  } while (maskBlocksRemaining != 0);
  return;
}

/* Reveal shape of SoftwareMaskBuffer_AdvancePatternByPercentTick: sets bit 0 of one horizontal band of 15 rows,
   band bandIndex - 1 from the top, or with reverseRows band 24 - bandIndex (from the bottom of 25 bands).
   bandIndex above 24, and bandIndex 0 top-down, set nothing. The band is filled in 16-byte steps
   (width * 15 >> 4 of them).
*/
void SoftwareMaskBuffer_ApplyHorizontalBandBit(UiBooleanState32 reverseRows,TerrainGridMaskIndex bandIndex,
          SoftwareMaskRuntimeView *maskRuntime)

{
  uint32_t bandBytes;
  uint32_t blocksLeft;
  int bandRow;
  uint32_t *maskWordCursor;
  GraphicsTextureLogicalSize logicalSize;

  logicalSize = g_GraphicsTextureSourceGetLogicalSize(0,maskRuntime->textureSource);
  if ((uint32_t)bandIndex >= 25) {
    return;
  }
  bandBytes = logicalSize.logicalWidthPixels * 15;
  if (reverseRows == 0) {
    bandRow = bandIndex - 1;
    if (bandRow < 0) {
      return;
    }
  }
  else {
    bandRow = 24 - bandIndex;
  }
  maskWordCursor = (uint32_t *)(maskRuntime->maskPixels + (int32_t)(bandRow * bandBytes));
  /* Original quirk: a do-while, so a band of fewer than 16 bytes wraps the counter to 2^32 steps */
  blocksLeft = bandBytes >> 4;
  do {
    *maskWordCursor = *maskWordCursor | ARGB8888_CHANNEL_ONES;
    maskWordCursor[1] = maskWordCursor[1] | ARGB8888_CHANNEL_ONES;
    maskWordCursor[2] = maskWordCursor[2] | ARGB8888_CHANNEL_ONES;
    maskWordCursor[3] = maskWordCursor[3] | ARGB8888_CHANNEL_ONES;
    maskWordCursor = maskWordCursor + 4;
    blocksLeft--;
  } while (blocksLeft != 0);
}
