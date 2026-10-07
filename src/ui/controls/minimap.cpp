/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/minimap.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/minimap.h>
#include <thandor/core/bytes.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* drawClipped slot of g_UiSelectionGeometryControlVtable. Fills the node (clipped) with its texture,
   rotated by rotationAngle and scaled by sampleScaleQ12 about sourceOrigin: every screen pixel is mapped
   back to a Q12 source position and bilinearly filtered from the 2x2 texels around it (texels outside the
   texture count as 0). Only direct-colour subresources (negative paletteIndex) are drawn. The pixels are
   written by g_GraphicsMinimapDraw (software: SoftwareTexture_DrawMinimapBilinear32).
*/
void UiSelectionGeometryControl_DrawClipped
          (int clipBottom,int clipRight,int clipTop,int clipLeft,UiSelectionGeometryControl *control)

{
  GraphicsTextureSourceAsset *sourceTexture;
  AssetRelativeOffset subresourceTable;
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
  int rowStepV;
  uint64_t sourceStartU; /* a zero-extended 32-bit value, so its high half is always 0 */
  int sourceStartV;
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
  if (*reinterpret_cast<int *>(Thandor_Bytes(sourceTexture) + subresourceTable + GFX_SUBRESOURCE_PALETTE_INDEX) >= 0) {
    return;
  }
  framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
  if (framebufferUnavailable) {
    return;
  }
  /* the high halves of the zero-extended start and row step are 0 (the original added them to v) */
  g_GraphicsMinimapDraw
            (clipTop,clipLeft,clipHeight,clipWidth,(uint32_t)sourceStartU,(uint32_t)sourceStartV,pixelStepU,
             pixelStepV,rowStepU,(uint32_t)rowStepV,sourceTexture,g_FramebufferAccess);
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
