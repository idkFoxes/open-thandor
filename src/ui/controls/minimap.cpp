/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/minimap.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/minimap.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

/* Weights of the bilinear scaler below, indexed by the 8-bit fraction between two source pixels: the first
   pixel's weight is g_UiScalerFirstPixelWeights, the second's g_UiScalerSecondPixelWeights, all four lanes
   equal. Precomputed tables in the original. */
static SoftwareBgraWordLanes g_UiScalerFirstPixelWeights[256];
static SoftwareBgraWordLanes g_UiScalerSecondPixelWeights[256];

/* Implementation ownership: ui/controls/minimap. */

/* Not in the original (it carried the tables precomputed): builds the scaler weights. Fractions 0..63 take
   only the first pixel (0x4000), 64..191 blend in steps t = 2 * (fraction - 64) of 0x4040 / 256 (the pair
   sums to 0x4040 or 0x403F, not 0x4000), 192..255 take only the second pixel (0x4000). This reproduces every
   entry of the original tables. Called once at startup. */
void UiScaler_BuildPixelWeightTables()
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
    g_UiScalerFirstPixelWeights[fraction].blue = firstWeight;
    g_UiScalerFirstPixelWeights[fraction].green = firstWeight;
    g_UiScalerFirstPixelWeights[fraction].red = firstWeight;
    g_UiScalerFirstPixelWeights[fraction].alpha = firstWeight;
    g_UiScalerSecondPixelWeights[fraction].blue = secondWeight;
    g_UiScalerSecondPixelWeights[fraction].green = secondWeight;
    g_UiScalerSecondPixelWeights[fraction].red = secondWeight;
    g_UiScalerSecondPixelWeights[fraction].alpha = secondWeight;
  }
}

/* MMX lane helpers for the bilinear scaler below (lanes are little-endian 16-bit words). */

/* PUNPCKLBW mm,mm then PSRLW mm,shift: byte i of pixel becomes word lane i = (byte * 0x101) >> shift. */
static __inline uint64_t UiScaler_UnpackBytesToWordLanes(uint32_t pixel,int shift) {
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
static __inline uint64_t UiScaler_AddWordLanes(uint64_t left,uint64_t right) {
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
static __inline uint32_t UiScaler_ShiftAndPackWordLanes(uint64_t lanes,int shift) {
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
static __inline PackedArgb32 UiScaler_BlendBilinear
          (PackedArgb32 topLeft,PackedArgb32 topRight,PackedArgb32 bottomLeft,PackedArgb32 bottomRight,
          int columnWeight,int rowWeight) {
  uint64_t topRow;
  uint64_t bottomRow;

  topRow = UiScaler_AddWordLanes
                     (pmulhw(UiScaler_UnpackBytesToWordLanes(topLeft,2),
                             g_UiScalerFirstPixelWeights[columnWeight]),
                      pmulhw(UiScaler_UnpackBytesToWordLanes(topRight,2),
                             g_UiScalerSecondPixelWeights[columnWeight]));
  bottomRow = UiScaler_AddWordLanes
                        (pmulhw(UiScaler_UnpackBytesToWordLanes(bottomLeft,2),
                                g_UiScalerFirstPixelWeights[columnWeight]),
                         pmulhw(UiScaler_UnpackBytesToWordLanes(bottomRight,2),
                                g_UiScalerSecondPixelWeights[columnWeight]));
  return UiScaler_ShiftAndPackWordLanes
                   (UiScaler_AddWordLanes(pmulhw(topRow,g_UiScalerFirstPixelWeights[rowWeight]),
                                          pmulhw(bottomRow,g_UiScalerSecondPixelWeights[rowWeight])),2);
}

/* One screen pixel of UiSelectionGeometryControl_DrawClipped: the 2x2 texels around the Q12 source position
   (sourceU, sourceV) of a sourceWidth x sourceHeight 32-bit texture (pixel data at pixelDataOffset from the
   asset; texels outside it count as 0), bilinearly blended by the position's fractions. */
static PackedArgb32 UiSelectionGeometryControl_SampleBilinear
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
             *(PackedArgb32 *)((uint8_t *)sourceTexture + texelIndex * 4 + pixelDataOffset);
      }
      if ((-1 < nextColumn) && (nextColumn < sourceWidth)) {
        sourcePixelSample1 =
             *(PackedArgb32 *)((uint8_t *)sourceTexture + texelIndex * 4 + pixelDataOffset + 4);
      }
    }
    if (((-1 < sourceRow + 1) && (sourceRow + 1 < sourceHeight)) && (sourceColumn < sourceWidth)) {
      if (-1 < sourceColumn) {
        sourcePixelSample2 =
             *(PackedArgb32 *)((uint8_t *)sourceTexture + (texelIndex + sourceWidth) * 4 + pixelDataOffset);
      }
      if ((-1 < nextColumn) && (nextColumn < sourceWidth)) {
        sourcePixelSample3 =
             *(PackedArgb32 *)((uint8_t *)sourceTexture + (texelIndex + sourceWidth) * 4 + pixelDataOffset + 4);
      }
    }
  }
  return UiScaler_BlendBilinear(sourcePixelSample0,sourcePixelSample1,sourcePixelSample2,sourcePixelSample3,
                                (int)(sourceU & Q12_FRACTION_MASK) >> 4,(int)(sourceV & Q12_FRACTION_MASK) >> 4);
}

/* drawClipped slot of g_UiSelectionGeometryControlVtable. Fills the node (clipped) with its texture,
   rotated by rotationAngle and scaled by sampleScaleQ12 about sourceOrigin: every screen pixel is mapped
   back to a Q12 source position and bilinearly filtered from the 2x2 texels around it (texels outside the
   texture count as 0), for 16- and 32-bit framebuffers. Only direct-colour subresources (negative
   paletteIndex) are drawn.
*/
void UiSelectionGeometryControl_DrawClipped
          (int clipBottom,int clipRight,int clipTop,int clipLeft,UiSelectionGeometryControl *control)

{
  GraphicsTextureSourceAsset *sourceTexture;
  AssetRelativeOffset subresourceTable;
  int sourceWidth;
  int sourceHeight;
  int pixelDataOffset;
  int64_t rotationProductA;
  int64_t rotationProductB;
  int64_t rotationProductC;
  int cosTerm;
  uint32_t sinTerm;
  uint32_t stepTermU;
  uint32_t stepTermV;
  int clipWidth;
  int clipHeight;
  uint32_t pixelStepU;
  uint32_t pixelStepV;
  uint32_t rowStepU;
  uint64_t rowStepUWide; /* rowStepU zero-extended, so its high half is always 0 */
  int rowStepUHigh;
  int rowStepV;
  uint64_t sourceStartU; /* a zero-extended 32-bit value, so its high half is always 0 */
  int sourceStartUHigh;
  int sourceStartV;
  uint32_t sourceU;
  uint32_t sourceV;
  uint32_t rowStartU;
  uint32_t rowStartV;
  uint8_t *destPixel;
  uint8_t *destRowStart;
  int remainingColumns;
  int remainingRows;
  Bool8 framebufferUnavailable;

  /* intersect the clip rectangle with the node */
  if (clipLeft < (control->base).left) {
    clipLeft = (control->base).left;
  }
  if (clipTop < (control->base).top) {
    clipTop = (control->base).top;
  }
  if ((control->base).right < clipRight) {
    clipRight = (control->base).right;
  }
  if ((control->base).bottom < clipBottom) {
    clipBottom = (control->base).bottom;
  }
  clipWidth = clipRight - clipLeft;
  clipHeight = clipBottom - clipTop;
  if ((clipWidth == 0) || (clipRight < clipLeft) || (clipHeight == 0) || (clipBottom < clipTop) ||
      (control->textureSource == nullptr)) {
    return;
  }
  rotationProductA = (int64_t)control->sampleScaleQ12 * (int64_t)g_FixedSineQ28[FIXED_SINE_TABLE_COS + control->rotationAngle];
  cosTerm = -(FIXED_PRODUCT_SHR(rotationProductA, Q28_SHIFT));
  rotationProductA = (int64_t)control->sampleScaleQ12 * (int64_t)g_FixedSineQ28[FIXED_SINE_TABLE_SIN + control->rotationAngle];
  sinTerm = FIXED_PRODUCT_SHR(rotationProductA, Q28_SHIFT);
  /* scale*(sin, cos) mapped through the field-grid lattice factors: products by the column factor are taken
     >> Q20_SHIFT, those by the row factor >> (Q20_SHIFT + 1) (half a row, the lattice skew) */
  rotationProductA = (int64_t)(int)sinTerm * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
  rotationProductB = (int64_t)cosTerm * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
  stepTermU = FIXED_PRODUCT_SHR(rotationProductB, Q20_SHIFT + 1);
  rotationProductB = (int64_t)cosTerm * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
  rotationProductC = (int64_t)(int)-sinTerm * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
  stepTermV = FIXED_PRODUCT_SHR(rotationProductC, Q20_SHIFT + 1);
  /* per-row texture step */
  rowStepU = (FIXED_PRODUCT_SHR(rotationProductB, Q20_SHIFT)) - stepTermV;
  rowStepV = stepTermV * 2;
  rowStepUWide = (uint64_t)rowStepU;
  sourceStartU = (uint64_t)
           (control->sourceOriginXQ12 -
           (rowStepU * (((control->base).top + (control->base).bottom >> 1) - clipTop) +
           ((FIXED_PRODUCT_SHR(rotationProductA, Q20_SHIFT)) -
            stepTermU) *
           (((control->base).left + (control->base).right >> 1) - clipLeft)));
  /* per-pixel texture step (u and v) */
  pixelStepU =
       (FIXED_PRODUCT_SHR(rotationProductA, Q20_SHIFT)) - stepTermU;
  pixelStepV = stepTermU * 2;
  sourceStartV = control->sourceOriginYQ12 -
           (rowStepV * (((control->base).top + (control->base).bottom >> 1) - clipTop) +
           stepTermU * 2 * (((control->base).left + (control->base).right >> 1) - clipLeft));
  sourceTexture = control->textureSource;
  subresourceTable = (sourceTexture->tableDescriptor).subresourceTableOffset;
  /* fields of the first subresource record (asset + subresourceTableOffset) */
  if (*(int *)((uint8_t *)sourceTexture + subresourceTable + GFX_SUBRESOURCE_PALETTE_INDEX) >= 0) {
    return;
  }
  sourceWidth =
       *(int *)((uint8_t *)sourceTexture + subresourceTable + GFX_SUBRESOURCE_PIXEL_WIDTH);
  sourceHeight =
       *(int *)((uint8_t *)sourceTexture + subresourceTable + GFX_SUBRESOURCE_PIXEL_HEIGHT);
  pixelDataOffset =
       *(int *)((uint8_t *)sourceTexture + subresourceTable + GFX_SUBRESOURCE_PIXEL_OFFSET);
  framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
  if (framebufferUnavailable) {
    return;
  }
  rowStepUHigh = (int)(rowStepUWide >> 32);
  sourceU = (uint32_t)sourceStartU;
  sourceStartUHigh = (int)(sourceStartU >> 32);
  remainingRows = clipHeight;
  remainingColumns = clipWidth;
  /* the original also had a 16-bit framebuffer path (packed through the 565/555 MMX constants) */
  destPixel = g_FramebufferAccess->pixels +
            (int32_t)(g_FramebufferRowStrideBytes * clipTop) + clipLeft * 4;
  sourceV = sourceStartUHigh + sourceStartV;
  rowStartU = sourceU;
  rowStartV = sourceV;
  destRowStart = destPixel;
  do {
    do {
      *(PackedArgb32 *)destPixel =
           UiSelectionGeometryControl_SampleBilinear
                     (sourceTexture,pixelDataOffset,sourceWidth,sourceHeight,sourceU,sourceV);
      sourceU = sourceU + (int)pixelStepU;
      sourceV = sourceV + pixelStepV;
      destPixel = destPixel + 4;
      remainingColumns = remainingColumns - 1;
    } while (remainingColumns != 0);
    sourceU = rowStartU + (int)rowStepUWide;
    sourceV = rowStartV + rowStepUHigh + rowStepV;
    destPixel = destRowStart + g_FramebufferRowStrideBytes;
    remainingRows = remainingRows - 1;
    rowStartU = sourceU;
    rowStartV = sourceV;
    remainingColumns = clipWidth;
    destRowStart = destPixel;
  } while (remainingRows != 0);
  g_GraphicsFramebufferEndAccess();
}

/* nonRightPress slot of g_UiSelectionGeometryControlVtable. Maps the clicked screen point back into
   texture space with the same rotation/scale as UiSelectionGeometryControl_DrawClipped, stores it in
   selectedSourceXQ12/YQ12 and queues actionId so the handler can read the picked source position.
*/
void UiSelectionGeometryControl_ConvertPointerAndEnqueueAction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSelectionGeometryControl *control)

{
  int cosTerm;
  int boundsLeft;
  int boundsRight;
  int boundsTop;
  int boundsBottom;
  int64_t rotationProductA;
  int64_t rotationProductB;
  int64_t rotationProductC;
  uint32_t sinTerm;
  uint32_t stepTermX;
  uint32_t stepTermY;

  rotationProductA = (int64_t)control->sampleScaleQ12 * (int64_t)g_FixedSineQ28[FIXED_SINE_TABLE_COS + control->rotationAngle];
  cosTerm = -(FIXED_PRODUCT_SHR(rotationProductA, Q28_SHIFT));
  rotationProductA = (int64_t)control->sampleScaleQ12 * (int64_t)g_FixedSineQ28[FIXED_SINE_TABLE_SIN + control->rotationAngle];
  sinTerm = FIXED_PRODUCT_SHR(rotationProductA, Q28_SHIFT);
  /* scale*(sin, cos) mapped through the field-grid lattice factors: products by the column factor are taken
     >> Q20_SHIFT, those by the row factor >> (Q20_SHIFT + 1) (half a row, the lattice skew) */
  rotationProductA = (int64_t)(int)sinTerm * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
  rotationProductB = (int64_t)cosTerm * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
  stepTermX = FIXED_PRODUCT_SHR(rotationProductB, Q20_SHIFT + 1);
  rotationProductB = (int64_t)cosTerm * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
  rotationProductC = (int64_t)(int)-sinTerm * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
  stepTermY = FIXED_PRODUCT_SHR(rotationProductC, Q20_SHIFT + 1);
  boundsLeft = (control->base).left;
  boundsRight = (control->base).right;
  boundsTop = (control->base).top;
  boundsBottom = (control->base).bottom;
  control->selectedSourceXQ12 =
       control->sourceOriginXQ12 -
       (((FIXED_PRODUCT_SHR(rotationProductB, Q20_SHIFT)) - stepTermY) *
        (((control->base).top + (control->base).bottom >> 1) - pointerY) +
       ((FIXED_PRODUCT_SHR(rotationProductA, Q20_SHIFT)) - stepTermX) *
       (((control->base).left + (control->base).right >> 1) - pointerX));
  control->selectedSourceYQ12 =
       (control->sourceOriginYQ12 - stepTermX * 2 * ((boundsLeft + boundsRight >> 1) - pointerX)) -
       stepTermY * 2 * ((boundsTop + boundsBottom >> 1) - pointerY);
  UiActionQueue_Enqueue(control->actionId,control);
}

UiNodeVtable g_UiSelectionGeometryControlVtable = {
        .relocate = UI_SLOT(UiContainer_RelocateChildren),
        .method04 = UI_SLOT(UiNode_DefaultMethod04_NoOp),
        .drawClipped = UI_SLOT(UiSelectionGeometryControl_DrawClipped),
        .layout = UI_SLOT(UiContainer_LayoutChildren),
        .nonRightPress = UI_SLOT(UiSelectionGeometryControl_ConvertPointerAndEnqueueAction),
        .nonRightRelease = UI_SLOT(UiNode_DefaultNonRightRelease),
        .rightPress = UI_SLOT(UiNode_ForwardRightPressToParent),
        .rightRelease = UI_SLOT(UiNode_DefaultRightRelease),
        .nonRightDrag = UI_SLOT(UiNode_DefaultNonRightDrag),
        .rightDrag = UI_SLOT(UiNode_DefaultRightDrag),
        .pointerMove = UI_SLOT(UiNode_DefaultPointerMove),
        .hitTest = UI_SLOT(UiContainer_HitTestChildren),
        .keyboardEvent = UI_SLOT(UiNode_DefaultKeyboardEventMoveFocusNext),
        .applyFlags = UI_SLOT(UiNode_ApplyFlagsRecursive),
        .suppressActionId = UI_SLOT(UiContainer_SuppressActionId),
        .unsuppressActionId = UI_SLOT(UiContainer_UnsuppressActionId),
        .tick = UI_SLOT(UiNode_DefaultTick),
        .pointerWheel = UI_SLOT(UiNode_ForwardPointerWheelToParent),
};
