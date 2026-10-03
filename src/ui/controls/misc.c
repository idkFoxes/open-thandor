/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/misc.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/misc.h>
#include <thandor/thandor.h>

/* Implementation ownership: ui/controls/misc. */

/* Address: 0x00422910.
   frameUpdate of g_UiDisplaySettingsRootCallbacks (the display settings dialog): when the colour bias or
   colour scale slider has moved, stores the new values, rebuilds the pixel packing tables at once (a live
   preview), refreshes which mode buttons are available and rewrites the two number readouts.
*/
void UiDisplaySettingsRoot_RefreshModeSelection(UiRootNode *root)

{
  UiAnchorFractionQ31 colorBiasQ16;
  UiAnchorFractionQ31 colorScaleQ16;
  UiDisplaySettingsApplyButton *applyButton;

  applyButton = (UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton);
  /* the slider values (UiRangeSliderControl.value, +0x58) */
  colorBiasQ16 = ((UiRangeSliderControl *)DISPLAY_SETTINGS_UI(root,colorBiasSlider))->value;
  colorScaleQ16 = ((UiRangeSliderControl *)DISPLAY_SETTINGS_UI(root,colorScaleSlider))->value;
  if ((colorBiasQ16 != applyButton->selectedColorBiasQ16) ||
      (colorScaleQ16 != applyButton->selectedColorScaleQ16)) {
    applyButton->selectedColorBiasQ16 = colorBiasQ16;
    applyButton->selectedColorScaleQ16 = colorScaleQ16;
    g_SoftwareBuildPixelPackTables(colorScaleQ16,colorBiasQ16);
    UiDisplayModeSelection_RefreshEnumeratedOptions
              (applyButton->selectedAdapterIndex,
               (UiNodeBase *)applyButton->selectedBitsPerPixel,
               applyButton->selectedHeight,
               applyButton->selectedWidth,&root->base);
    UiDisplaySettingsRoot_FormatColorReadouts(root);
  }
  return;
}


/* Address: 0x00423C40.
   Handler of the four colour-depth buttons (actions 0x201..0x204, g_UiDisplayModeSelectionActionHandlers20[1..4])
   of the display settings dialog: selects the button's bit depth, keeps the selected adapter and resolution
   and refreshes the available buttons. Each option button keeps its value in the dword 8 bytes before it
   (UiDisplayModeOptionPrefix.modeValue). UiDisplayModeAction_UpdateAdapterSelection is its mirror image for the
   adapter buttons.
*/
void UiDisplayModeAction_UpdateColorDepthSelection(UiNodeBase *sourceNode)

{
  UiNodeBase *displaySettingsRoot;
  UiDisplaySettingsApplyButton *applyButton;

  displaySettingsRoot = UiNode_GetRoot(sourceNode);
  applyButton = (UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(displaySettingsRoot,applyButton);
  UiDisplayModeSelection_RefreshEnumeratedOptions
            ((FrontendDisplayAdapterIndex)applyButton->selectedAdapterIndex,
             (UiNodeBase *)DISPLAY_MODE_OPTION_PREFIX(sourceNode).modeValue,
             (FrontendDisplayDimensionPixels)applyButton->selectedHeight,
             (FrontendDisplayDimensionPixels)applyButton->selectedWidth,displaySettingsRoot);
  return;
}


/* Address: 0x00423C80.
   Handler of the eight resolution buttons (actions 0x205..0x20C, g_UiDisplayModeSelectionActionHandlers20[5..12])
   of the display settings dialog: selects the button's resolution (height 12 bytes and width 8 bytes before
   the button), keeps the selected adapter and bit depth and refreshes the available buttons.
*/
void UiDisplayModeAction_UpdateResolutionSelection(UiNodeBase *sourceNode)

{
  UiNodeBase *displaySettingsRoot;
  UiDisplaySettingsApplyButton *applyButton;

  displaySettingsRoot = UiNode_GetRoot(sourceNode);
  applyButton = (UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(displaySettingsRoot,applyButton);
  UiDisplayModeSelection_RefreshEnumeratedOptions
            ((FrontendDisplayAdapterIndex)applyButton->selectedAdapterIndex,
             (struct UiNodeBase *)applyButton->selectedBitsPerPixel,
             DISPLAY_MODE_OPTION_PREFIX(sourceNode).resolutionHeight,
             DISPLAY_MODE_OPTION_PREFIX(sourceNode).modeValue,displaySettingsRoot);
  return;
}


/* Address: 0x00423CB0.
   Handler of the five adapter buttons (actions 0x20F..0x213, g_UiDisplayModeSelectionActionHandlers20[15..19])
   of the display settings dialog: selects the button's adapter (the dword 8 bytes before the button), keeps
   the selected resolution and bit depth and refreshes the available buttons. See
   UiDisplayModeAction_UpdateColorDepthSelection.
*/
void UiDisplayModeAction_UpdateAdapterSelection(UiNodeBase *sourceNode)

{
  UiNodeBase *displaySettingsRoot;
  UiDisplaySettingsApplyButton *applyButton;

  displaySettingsRoot = UiNode_GetRoot(sourceNode);
  applyButton = (UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(displaySettingsRoot,applyButton);
  UiDisplayModeSelection_RefreshEnumeratedOptions
            (DISPLAY_MODE_OPTION_PREFIX(sourceNode).modeValue,
             (struct UiNodeBase *)applyButton->selectedBitsPerPixel,
             (FrontendDisplayDimensionPixels)applyButton->selectedHeight,
             (FrontendDisplayDimensionPixels)applyButton->selectedWidth,displaySettingsRoot);
  return;
}


/* Address: 0x00424590.
   Revert action (UI_DISPLAY_MODE_ACTION_REVERT, g_UiDisplayModeSelectionActionHandlers20[13]) of the "keep
   the new display mode?" dialog, from its button or from the expired countdown: closes the dialog, switches
   back to the previous display mode stored in it (a failure is fatal), lays out every open root for the
   restored framebuffer size and opens the display settings dialog again. The name is misleading: nothing is
   applied.
*/
void UiDisplayModeAction_RevertAndReopenSettings(UiNodeBase *sourceNode)

{
  int64_t scaledAnchor;
  UiRootNode *root;
  uint32_t modeError;
  uint32_t adapterIndex;
  uint32_t bitsPerPixel;
  uint32_t modeHeight;
  uint32_t modeWidth;
  UiFourValueDialogCountdownText *countdownText;

  root = (UiRootNode *)UiNode_GetRoot(sourceNode);
  countdownText = (UiFourValueDialogCountdownText *)FOUR_VALUE_DIALOG_UI(root,countdownMessageText);
  modeWidth = countdownText->previousWidth;
  modeHeight = countdownText->previousHeight;
  bitsPerPixel = countdownText->previousBitsPerPixel;
  adapterIndex = countdownText->previousAdapterIndex;
  UiRootStack_Pop(root);
  g_CursorVisibilityToken--;
  UiFrame_ProcessAndPresentWithLockTransition();
  if (!g_GraphicsSetDisplayMode(adapterIndex,bitsPerPixel,modeHeight,modeWidth,&modeError)) {
    FatalError_ExitIfFailed(modeError,true);
  }
  /* the loop of UiRootStack_Relayout, inlined: edge = framebuffer size * anchor (Q31) + offset */
  root = g_UiRootNode;
  do {
    scaledAnchor = (uint64_t)g_FramebufferWidth * (uint64_t)(root->base).rightAnchorQ31;
    (root->base).right =
         (FIXED_PRODUCT_SHR(scaledAnchor, 31)) + (root->base).rightOffset;
    scaledAnchor = (uint64_t)g_FramebufferHeight * (uint64_t)(root->base).bottomAnchorQ31;
    (root->base).bottom =
         (FIXED_PRODUCT_SHR(scaledAnchor, 31)) + (root->base).bottomOffset;
    scaledAnchor = (uint64_t)g_FramebufferWidth * (uint64_t)(root->base).leftAnchorQ31;
    (root->base).left =
         (FIXED_PRODUCT_SHR(scaledAnchor, 31)) + (root->base).leftOffset;
    scaledAnchor = (uint64_t)g_FramebufferHeight * (uint64_t)(root->base).topAnchorQ31;
    (root->base).top =
         (FIXED_PRODUCT_SHR(scaledAnchor, 31)) + (root->base).topOffset;
    (*((root->base).vtable)->layout)(&root->base);
    root = root->previousRoot;
  } while (root != (UiRootNode *)UI_NODE_NONE);
  g_CursorVisibilityToken++;
  UiDisplaySettings_OpenAndPopulateModeSelection();
  return;
}


/* Address: 0x004BC8B0.
   nonRightDrag of the image control (g_UiNodeVtable_004BC570): only for an image in persistent activation
   mode, whose children act like a menu. Moving onto another child hands the pointer over: the new child
   gets a synthetic press and the drag, becomes activeChild, and the previous one gets a synthetic drag and
   release far outside (UI_POINTER_FAR_OUTSIDE). A drag over the current child is simply forwarded.
*/
void UiImageControl_NonRightDrag(UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiImageControl *control)

{
  UiNodeVtable *hitVtable;
  UiImageControl *hitControl;
  UiNodeBase *previousActiveChild;

  if (((control->selectable).stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) == 0) {
    return;
  }
  hitControl = (UiImageControl *)UiImageControl_HitTestOpaque(pointerY,pointerX,control);
  /* Off the image's own pixels PRESSED_ON_IMAGE is cleared. Over the image itself or over nothing the
     current child only gets the synthetic drag and release; it stays activeChild (as in the original). */
  if (hitControl != control) {
    (control->selectable).stateFlags &= ~UI_IMAGE_CONTROL_PRESSED_ON_IMAGE;
  }
  if (hitControl == control || hitControl == (UiImageControl *)UI_NODE_NONE) {
    previousActiveChild = control->activeChild;
  }
  else {
    if (hitControl == (UiImageControl *)control->activeChild) {
      (*((hitControl->selectable).base.vtable)->nonRightDrag)
                (wheelDelta,pointerY,pointerX,(UiNodeBase *)hitControl);
      UiRootStack_InvalidateAll();
      return;
    }
    hitVtable = (hitControl->selectable).base.vtable;
    /* The handlers preserve EAX/EDX: the original keeps passing the hit child and its vtable, and
       stores that child as the new activeChild (the decompiler lost both). */
    hitVtable->nonRightPress(0,UI_POINTER_FAR_OUTSIDE,UI_POINTER_FAR_OUTSIDE,(UiNodeBase *)hitControl);
    hitVtable->nonRightDrag(wheelDelta,pointerY,pointerX,(UiNodeBase *)hitControl);
    /* swap in the new active child (an XCHG in the original) */
    previousActiveChild = control->activeChild;
    control->activeChild = (UiNodeBase *)hitControl;
  }
  if (previousActiveChild != NULL) {
    previousActiveChild->vtable->nonRightDrag
              (0,UI_POINTER_FAR_OUTSIDE,UI_POINTER_FAR_OUTSIDE,previousActiveChild);
    previousActiveChild->vtable->nonRightRelease
              (0,UI_POINTER_FAR_OUTSIDE,UI_POINTER_FAR_OUTSIDE,previousActiveChild);
  }
  UiRootStack_InvalidateAll();
  return;
}


/* Address: 0x004BCB50.
   tick of the image control (g_UiNodeVtable_004BC570): when the right mouse button goes down (latched in
   UI_IMAGE_CONTROL_RIGHT_BUTTON_LATCHED until it is released), an opaque child under the cursor gets a
   release, press and drag at the current cursor position, so that it re-evaluates the pointer;
   UI_IMAGE_CONTROL_PRESS_STARTED is cleared then.
*/
void UiImageControl_TickHover(UiImageControl *control)

{
  UiSelectableStateFlags *clearStateFlagsField;
  UiNodeVtable *hitChildVtable;
  UiImageControl *hitControl;
  UiSelectableStateFlags *stateFlagsField;
  UiSelectableStateFlags *hoverStateFlagsField;

  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (((control->selectable).stateFlags & UI_IMAGE_CONTROL_RIGHT_BUTTON_LATCHED) == 0) {
      if ((g_CursorButtonState & RIGHT) != 0) {
        hitControl = (UiImageControl *)
                     UiImageControl_HitTestOpaque(g_CursorOverrideY,g_CursorOverrideX,control);
        stateFlagsField = &(control->selectable).stateFlags;
        *stateFlagsField = *stateFlagsField | UI_IMAGE_CONTROL_RIGHT_BUTTON_LATCHED;
        if ((hitControl != control) && (hitControl != (UiImageControl *)UI_NODE_NONE)) {
          hitChildVtable = (hitControl->selectable).base.vtable;
          hoverStateFlagsField = &(control->selectable).stateFlags;
          *hoverStateFlagsField = *hoverStateFlagsField & ~UI_IMAGE_CONTROL_PRESS_STARTED;
          hitChildVtable->nonRightRelease
                    (g_CursorWheelDelta,g_CursorOverrideY,g_CursorOverrideX,(UiNodeBase *)hitControl);
          /* EAX (the hovered child) and ECX (its vtable) survive the handler calls. */
          hitChildVtable->nonRightPress
                    (g_CursorWheelDelta,g_CursorOverrideY,g_CursorOverrideX,(UiNodeBase *)hitControl);
          hitChildVtable->nonRightDrag
                    (g_CursorWheelDelta,g_CursorOverrideY,g_CursorOverrideX,(UiNodeBase *)hitControl);
        }
      }
    }
    else if ((g_CursorButtonState & RIGHT) == 0) {
      clearStateFlagsField = &(control->selectable).stateFlags;
      *clearStateFlagsField = *clearStateFlagsField & ~UI_IMAGE_CONTROL_RIGHT_BUTTON_LATCHED;
    }
  }
  return;
}


/* Address: 0x00423B30.
   Apply action (UI_DISPLAY_MODE_ACTION_APPLY, g_UiDisplayModeSelectionActionHandlers20[0]) of the display
   settings dialog: closes the dialog and, when the selected mode differs from the current one, switches to
   it. If the switch fails, the current mode is restored (a failure there is fatal) and the error is
   reported; otherwise every root is laid out again and the "keep the new display mode?" dialog opens with
   the previous mode, which it restores unless the player confirms.
*/
void UiDisplayModeAction_ApplyPendingMode(UiNodeBase *sourceNode)

{
  uint32_t pendingBitsPerPixel;
  uint32_t pendingAdapterIndex;
  UiRootNode *root;
  uint32_t pendingWidth;
  uint32_t pendingHeight;
  uint32_t currentBitsPerPixel;
  uint32_t pendingModeError;
  uint32_t restoreModeError;
  uint32_t currentAdapterIndex;
  uint32_t currentWidth;
  uint32_t currentHeight;
  UiDisplaySettingsApplyButton *applyButton;

  root = (UiRootNode *)UiNode_GetRoot(sourceNode);
  applyButton = (UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton);
  pendingWidth = applyButton->selectedWidth;
  pendingHeight = applyButton->selectedHeight;
  pendingBitsPerPixel = applyButton->selectedBitsPerPixel;
  currentBitsPerPixel = g_SoftwarePixelFormatConfig.redBitCount + g_SoftwarePixelFormatConfig.greenBitCount +
            g_SoftwarePixelFormatConfig.blueBitCount;
  pendingAdapterIndex = applyButton->selectedAdapterIndex;
  UiRootStack_Pop(root);
  if (pendingWidth == g_FramebufferWidth && pendingHeight == g_FramebufferHeight &&
      pendingBitsPerPixel == currentBitsPerPixel && pendingAdapterIndex == g_ActiveGraphicsAdapterIndex) {
    return;
  }
  g_CursorVisibilityToken--;
  UiFrame_ProcessAndPresentWithLockTransition();
  /* the current mode, read before the switch changes it */
  currentAdapterIndex = g_ActiveGraphicsAdapterIndex;
  currentHeight = g_FramebufferHeight;
  currentWidth = g_FramebufferWidth;
  if (!g_GraphicsSetDisplayMode(pendingAdapterIndex,pendingBitsPerPixel,pendingHeight,pendingWidth,
                                &pendingModeError)) {
    if (!g_GraphicsSetDisplayMode(currentAdapterIndex,currentBitsPerPixel,currentHeight,currentWidth,
                                  &restoreModeError)) {
      FatalError_ExitIfFailed(restoreModeError,true);
    }
    g_CursorVisibilityToken++;
    FatalError_ReportIfFailed(pendingModeError,true);
    return;
  }
  UiRootStack_Relayout();
  g_CursorVisibilityToken++;
  UiRuntime_OpenFourValueDialog(currentAdapterIndex,currentBitsPerPixel,currentHeight,currentWidth);
  return;
}


/* Address: 0x00423C00.
   Cancel action (UI_DISPLAY_MODE_ACTION_CANCEL, g_UiDisplayModeSelectionActionHandlers20[14]) of the display
   settings dialog: closes it and rebuilds the pixel packing tables from the colour bias and scale the
   dialog opened with, undoing the slider preview.
*/
void UiDisplayModeAction_CancelAndRebuildPixelPacking(UiNodeBase *sourceNode)

{
  int32_t colorBiasQ16;
  int32_t colorScaleQ16;
  UiNodeBase *displaySettingsRoot;
  UiDisplaySettingsApplyButton *applyButton;

  displaySettingsRoot = UiNode_GetRoot(sourceNode);
  applyButton = (UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(displaySettingsRoot,applyButton);
  colorBiasQ16 = applyButton->originalColorBiasQ16;
  colorScaleQ16 = applyButton->originalColorScaleQ16;
  UiRootStack_Pop((UiRootNode *)sourceNode); /* the button, not the root, as in the original */
  g_SoftwareBuildPixelPackTables(colorScaleQ16,colorBiasQ16);
  return;
}


/* Address: 0x004242D0.
   frameUpdate of g_UiFourValueDialogRootCallbacks (the "keep the new display mode?" dialog): every
   UI_DISPLAY_MODE_COUNTDOWN_STEP_TICKS frame updates the shown countdown drops by one; at zero the revert
   action is queued, otherwise the new number is written into the message.
*/
void UiFourValueDialog_TickCountdownAndRequestClose(UiRootNode *root)

{
  int32_t *stepTicksField;
  int32_t *countdownField;
  UiFourValueDialogCountdownText *countdownText;

  countdownText = (UiFourValueDialogCountdownText *)FOUR_VALUE_DIALOG_UI(root,countdownMessageText);
  stepTicksField = &countdownText->stepTicks;
  *stepTicksField = *stepTicksField - 1;
  if (*stepTicksField == 0) {
    countdownText->stepTicks =
         UI_DISPLAY_MODE_COUNTDOWN_STEP_TICKS;
    countdownField = &countdownText->countdown;
    *countdownField = *countdownField - 1;
    if (*countdownField == 0) {
      UiActionQueue_Enqueue(UI_DISPLAY_MODE_ACTION_REVERT,root);
    }
    else {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,
                 countdownText->countdown,
                 countdownText->countdownTextUtf16);
    }
  }
  return;
}


/* Thumb offset along the track for UiRangeSliderControl_DrawTrackAndThumb: value (clamped to
   minimumValue..maximumValue) scaled from the range onto freeTrackLength, rounded to the nearest pixel;
   measured from the other end when invert is set. */
static uint32_t UiRangeSliderControl_ThumbOffset(const UiRangeSliderControl *control,uint32_t freeTrackLength,
                                                 bool invert)
{
  int32_t rangeMax;
  int32_t clampedValue;
  uint32_t range;
  uint32_t valueOffset;
  uint64_t scaledOffset;

  rangeMax = control->maximumValue;
  clampedValue = control->value;
  if (rangeMax < control->value) {
    clampedValue = rangeMax;
  }
  range = rangeMax - control->minimumValue;
  if (range == 0) {
    range = 1;
  }
  valueOffset = clampedValue - control->minimumValue;
  if ((int)valueOffset < 0) {
    valueOffset = 0;
  }
  if (invert) {
    valueOffset = range - valueOffset;
  }
  /* offset * free track length / range, plus one when twice the remainder exceeds the range */
  scaledOffset = (uint64_t)valueOffset * (uint64_t)freeTrackLength;
  return (uint32_t)(int)(scaledOffset / range) +
         (uint32_t)(range < (uint32_t)((int)(scaledOffset % (uint64_t)range) * 2));
}

/* Address: 0x004B3F40.
   drawClipped of the range slider (g_UiRangeSliderControlVtable): draws the track from three
   g_UiWindowTextureSource pieces (start cap, tiled middle, end cap) and the thumb at the position of value
   within minimumValue..maximumValue, rounded to the nearest pixel. Horizontal or vertical after
   UI_RANGE_SLIDER_VERTICAL; a suppressed slider uses the greyed pieces.
*/
void UiRangeSliderControl_DrawTrackAndThumb
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiRangeSliderControl *control)

{
  uint32_t subresourceBase;
  int edgeLength;
  uint32_t thumbOffset;
  GraphicsTextureLogicalSize textureSize;

  if (g_GraphicsFramebufferBeginAccess()) {
    return; /* framebuffer access failed */
  }
  subresourceBase = UI_RANGE_SLIDER_SUBRESOURCE_BASE;
  if ((control->base.nodeFlags & UI_NODE_SUPPRESSED) != 0) {
    subresourceBase = UI_RANGE_SLIDER_SUBRESOURCE_BASE_SUPPRESSED;
  }
  if ((control->sliderFlags & UI_RANGE_SLIDER_VERTICAL) != 0) {
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,control->base.top,control->base.left,
               subresourceBase + UI_RANGE_SLIDER_SUBRESOURCE_VERTICAL_OFFSET,
               g_UiWindowTextureSource,g_FramebufferAccess);
    textureSize = g_GraphicsTextureSourceGetLogicalSize(subresourceBase + UI_RANGE_SLIDER_SUBRESOURCE_VERTICAL_OFFSET,
                                                        g_UiWindowTextureSource);
    edgeLength = control->base.layoutHeight - textureSize.logicalHeightPixels;
    UiWindow_BlitTiledVerticalEdge
              (clipBottom,clipRight,clipTop,clipLeft,
               subresourceBase + UI_RANGE_SLIDER_SUBRESOURCE_VERTICAL_OFFSET + UI_RANGE_SLIDER_PIECE_TRACK,
               edgeLength,textureSize.logicalHeightPixels,0,&control->base);
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,edgeLength + control->base.top,control->base.left,
               subresourceBase + UI_RANGE_SLIDER_SUBRESOURCE_VERTICAL_OFFSET + UI_RANGE_SLIDER_PIECE_END_CAP,
               g_UiWindowTextureSource,g_FramebufferAccess);
    /* the thumb; vertical sliders have their maximum at the top unless reversed */
    textureSize = g_GraphicsTextureSourceGetLogicalSize
              (subresourceBase + UI_RANGE_SLIDER_SUBRESOURCE_VERTICAL_OFFSET + UI_RANGE_SLIDER_PIECE_THUMB,
               g_UiWindowTextureSource);
    thumbOffset = UiRangeSliderControl_ThumbOffset
              (control,control->base.layoutHeight - textureSize.logicalHeightPixels,
               (control->sliderFlags & UI_RANGE_SLIDER_REVERSED) == 0);
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,thumbOffset + control->base.top,control->base.left,
               subresourceBase + UI_RANGE_SLIDER_SUBRESOURCE_VERTICAL_OFFSET + UI_RANGE_SLIDER_PIECE_THUMB,
               g_UiWindowTextureSource,g_FramebufferAccess);
  }
  else {
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,control->base.top,control->base.left,subresourceBase,
               g_UiWindowTextureSource,g_FramebufferAccess);
    textureSize = g_GraphicsTextureSourceGetLogicalSize(subresourceBase,g_UiWindowTextureSource);
    edgeLength = control->base.layoutWidth - textureSize.logicalWidthPixels;
    UiWindow_BlitTiledHorizontalEdge
              (clipBottom,clipRight,clipTop,clipLeft,subresourceBase + UI_RANGE_SLIDER_PIECE_TRACK,edgeLength,0,
               textureSize.logicalWidthPixels,&control->base);
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,control->base.top,edgeLength + control->base.left,
               subresourceBase + UI_RANGE_SLIDER_PIECE_END_CAP,
               g_UiWindowTextureSource,g_FramebufferAccess);
    /* the thumb */
    textureSize = g_GraphicsTextureSourceGetLogicalSize(subresourceBase + UI_RANGE_SLIDER_PIECE_THUMB,
                                                        g_UiWindowTextureSource);
    thumbOffset = UiRangeSliderControl_ThumbOffset
              (control,control->base.layoutWidth - textureSize.logicalWidthPixels,
               (control->sliderFlags & UI_RANGE_SLIDER_REVERSED) != 0);
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,control->base.top,thumbOffset + control->base.left,
               subresourceBase + UI_RANGE_SLIDER_PIECE_THUMB,g_UiWindowTextureSource,g_FramebufferAccess);
  }
  g_GraphicsFramebufferEndAccess();
  return;
}


/* Address: 0x004B41C0.
   nonRightPress of the range slider (g_UiRangeSliderControlVtable): a press inside the slider, within the
   thumb's cross size (its height for a horizontal slider, its width for a vertical one), starts a thumb
   drag and plays the click sound when UI_RANGE_SLIDER_CLICK_SOUND is set.
*/
void UiRangeSliderControl_BeginThumbDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiRangeSliderControl *control)

{
  int localX;
  int localY;
  GraphicsTextureLogicalSize thumbSize;

  if ((control->base.nodeFlags & UI_NODE_SUPPRESSED) != 0) {
    return;
  }
  if (pointerX < control->base.left || pointerY < control->base.top) {
    return;
  }
  localX = pointerX - control->base.left;
  localY = pointerY - control->base.top;
  if (localX >= control->base.layoutWidth || localY >= control->base.layoutHeight) {
    return;
  }
  if ((control->sliderFlags & UI_RANGE_SLIDER_VERTICAL) == 0) {
    thumbSize = g_GraphicsTextureSourceGetLogicalSize(UI_RANGE_SLIDER_SUBRESOURCE_HORIZONTAL_THUMB,
                                                      g_UiWindowTextureSource);
    if ((int)thumbSize.logicalHeightPixels <= localY) {
      return;
    }
  }
  else {
    thumbSize = g_GraphicsTextureSourceGetLogicalSize(UI_RANGE_SLIDER_SUBRESOURCE_VERTICAL_THUMB,
                                                      g_UiWindowTextureSource);
    if ((int)thumbSize.logicalWidthPixels <= localX) {
      return;
    }
  }
  control->sliderFlags = control->sliderFlags | UI_RANGE_SLIDER_DRAGGING;
  if (((control->sliderFlags & UI_RANGE_SLIDER_CLICK_SOUND) != 0) && (control->clickSound != NULL)) {
    g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->clickSound,NULL);
  }
  return;
}


/* Address: 0x004B4280.
   nonRightRelease of the range slider (g_UiRangeSliderControlVtable): ends a thumb drag and plays the click
   sound when UI_RANGE_SLIDER_CLICK_SOUND is set and the slider is not suppressed.
*/
void UiRangeSliderControl_EndThumbDrag
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
               UiRangeSliderControl *control)

{
  control->sliderFlags = control->sliderFlags & ~UI_RANGE_SLIDER_DRAGGING;
  if ((((control->base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
       ((control->sliderFlags & UI_RANGE_SLIDER_CLICK_SOUND) != 0)) && (control->clickSound != NULL)) {
    g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->clickSound,NULL);
  }
  return;
}


/* Address: 0x004B45F0.
   suppressActionId of the range slider (g_UiRangeSliderControlVtable): a slider with this action id is
   greyed out (UI_NODE_SUPPRESSED), gives up the keyboard focus and is redrawn.
*/
void UiRangeSliderControl_SuppressIfActionId(UiActionId actionId,UiRangeSliderControl *control)

{
  if (actionId == control->actionId) {
    control->base.nodeFlags = control->base.nodeFlags | UI_NODE_SUPPRESSED;
    UiKeyboardFocus_ReleaseNode(&control->base);
    UiNode_InvalidateRoot(&control->base);
  }
  return;
}


/* Address: 0x004B4620.
   unsuppressActionId of the range slider (g_UiRangeSliderControlVtable): a slider with this action id is
   enabled again, takes the keyboard focus if nobody has it and is redrawn.
*/
void UiRangeSliderControl_UnsuppressIfActionId(UiActionId actionId,UiRangeSliderControl *control)

{
  if (actionId == control->actionId) {
    control->base.nodeFlags = control->base.nodeFlags & ~UI_NODE_SUPPRESSED;
    UiKeyboardFocus_AcquireIfNone(&control->base);
    UiNode_InvalidateRoot(&control->base);
  }
  return;
}


/* Address: 0x004BC5C0.
   drawClipped of the image control (g_UiNodeVtable_004BC570): in persistent activation mode the children
   are drawn first, then the image itself: alternateSubresource while selected, else normalSubresource. An
   image with UI_IMAGE_CONTROL_ALTERNATE_HIT_SHAPE is only drawn while selected.
*/
void UiImageControl_DrawClipped(UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiImageControl *control)

{
  bool accessFailed;
  GraphicsSubresourceIndex subresource;

  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (((control->selectable).stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) != 0) {
      UiContainer_DrawIntersectingChildren
                (clipBottom,clipRight,clipTop,clipLeft,(UiNodeBase *)control);
    }
    if ((((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) ||
       (((control->selectable).stateFlags & UI_IMAGE_CONTROL_ALTERNATE_HIT_SHAPE) == 0)) {
      accessFailed = g_GraphicsFramebufferBeginAccess();
      if (!accessFailed) {
        if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
          subresource = control->normalSubresource;
        }
        else {
          subresource = control->alternateSubresource;
        }
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipBottom,clipRight,clipTop,clipLeft,(control->selectable).base.top,
                   (control->selectable).base.left,subresource,control->textureSource,g_FramebufferAccess);
        g_GraphicsFramebufferEndAccess();
      }
    }
  }
  return;
}


/* Address: 0x004BC6E0.
   nonRightPress of the image control (g_UiNodeVtable_004BC570): plays the pointer sound
   (UI_IMAGE_CONTROL_POINTER_SOUND, unless the image is already OPEN), drops the active child and the hover
   target, then toggles: a press on an opaque pixel of an already selected image clears
   UI_IMAGE_CONTROL_PRESS_STATE_BITS, any other press sets them.
*/
void UiImageControl_NonRightPress(UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiImageControl *control)

{
  UiSelectableStateFlags *pressStateFlagsField;
  bool opaqueHit;
  UiSelectableStateFlags *stateFlagsField;

  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) != 0) {
    return;
  }
  if (((control->selectable).stateFlags & UI_IMAGE_CONTROL_OPEN) == 0 &&
      ((control->selectable).stateFlags & UI_IMAGE_CONTROL_POINTER_SOUND) != 0 &&
      control->pointerActivationSound != NULL) {
    g_SoundPlayOneShot
              (g_UiSoundGainQ15,g_UiSoundGainQ15,
               control->pointerActivationSound,NULL);
  }
  opaqueHit = false;
  if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
    if (((control->selectable).stateFlags & UI_IMAGE_CONTROL_ALTERNATE_HIT_SHAPE) == 0) {
      opaqueHit = g_GraphicsTextureSourceTestOpaquePixel
                        (pointerY,pointerX,(control->selectable).base.top,
                         (control->selectable).base.left,control->normalSubresource,
                         control->textureSource);
    }
    else {
      opaqueHit = g_GraphicsTextureSourceTestOpaquePixel
                        (pointerY,pointerX,(control->selectable).base.top,
                         (control->selectable).base.left,control->alternateSubresource,
                         control->textureSource);
    }
  }
  control->activeChild = NULL;
  g_UiImageControlHoverTarget = NULL;
  if (opaqueHit) {
    /* Pressing an already selected image on an opaque pixel clears its selected/armed state. */
    stateFlagsField = &(control->selectable).stateFlags;
    *stateFlagsField = *stateFlagsField & ~UI_IMAGE_CONTROL_PRESS_STATE_BITS;
  }
  else {
    pressStateFlagsField = &(control->selectable).stateFlags;
    *pressStateFlagsField = *pressStateFlagsField | UI_IMAGE_CONTROL_PRESS_STATE_BITS;
  }
  UiNode_InvalidateRoot((UiNodeBase *)control);
  return;
}


/* Address: 0x004BC7E0.
   nonRightRelease of the image control (g_UiNodeVtable_004BC570). A release while
   UI_IMAGE_CONTROL_PRESSED_ON_IMAGE is set keeps it open: UI_IMAGE_CONTROL_OPEN, and the image becomes
   g_UiImageControlHoverTarget. Otherwise an active child gets the release first, and the image stays open
   only if it had one and OPEN was not yet set or PRESS_STARTED is set; else it closes: hover target
   cleared, UI_IMAGE_CONTROL_HOVER_STATE_BITS cleared and the pointer sound played (POINTER_SOUND).
*/
void UiImageControl_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiImageControl *control)

{
  UiSelectableStateFlags *hoverStateFlagsField;
  UiNodeBase *previousActiveChild;
  UiSelectableStateFlags *stateFlagsField;
  UiNodeVtable *activeChildVtable;
  bool preserveHover;

  previousActiveChild = control->activeChild;
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    preserveHover = ((control->selectable).stateFlags & UI_IMAGE_CONTROL_PRESSED_ON_IMAGE) != 0;
    if (!preserveHover) {
      if (previousActiveChild != NULL) {
        activeChildVtable = previousActiveChild->vtable;
        control->activeChild = NULL;
        activeChildVtable->nonRightRelease(wheelDelta,pointerY,pointerX,previousActiveChild);
        preserveHover = ((control->selectable).stateFlags & UI_IMAGE_CONTROL_OPEN) == 0 ||
                        ((control->selectable).stateFlags & UI_IMAGE_CONTROL_PRESS_STARTED) != 0;
      }
      if (!preserveHover) {
        g_UiImageControlHoverTarget = NULL;
        stateFlagsField = &(control->selectable).stateFlags;
        *stateFlagsField = *stateFlagsField & ~UI_IMAGE_CONTROL_HOVER_STATE_BITS;
        if (((control->selectable).stateFlags & UI_IMAGE_CONTROL_POINTER_SOUND) != 0 &&
            control->pointerActivationSound != NULL) {
          g_SoundPlayOneShot
                    (g_UiSoundGainQ15,g_UiSoundGainQ15,
                     control->pointerActivationSound,NULL);
        }
      }
    }
    if (preserveHover) {
      hoverStateFlagsField = &(control->selectable).stateFlags;
      *hoverStateFlagsField = *hoverStateFlagsField | UI_IMAGE_CONTROL_OPEN;
      g_UiImageControlHoverTarget = control;
      hoverStateFlagsField = &(control->selectable).stateFlags;
      *hoverStateFlagsField = *hoverStateFlagsField & ~UI_IMAGE_CONTROL_PRESSED_ON_IMAGE;
    }
  }
  UiNode_InvalidateRoot((UiNodeBase *)control);
  return;
}


/* Address: 0x004BD2A0.
   Re-tints a world model (army, effect or shot) after its runtime state bits changed: the tint chosen by
   ModelRuntimeNode_GetStateTintArgb from runtimeFlags 0x04/0x08/0x10 is applied to the whole hierarchy only when it
   differs from the tint the model already has.
*/
void ModelNodeRuntime_RefreshStateTint(ModelRuntimeNode *modelNode)

{
  PackedArgb32 tintArgb;
  
  tintArgb = ModelRuntimeNode_GetStateTintArgb(modelNode);
  if (tintArgb != modelNode->tintArgb) {
    ModelNodeRuntime_ApplyTintRecursive(tintArgb,modelNode);
  }
  return;
}


/* Address: 0x00517E30.
   drawClipped of the transfer progress gauge (g_UiNodeVtable_00517DE0) shown while the player snapshots
   are exchanged at session start: on the host (or in a local game) the range is the outgoing byte count
   and the value the smallest progress any client has reported (transferProgressBytes of player blocks 1..n); on a
   client it is the received byte count and the bytes received so far. Draws nothing unless a transfer is
   running and not yet complete.
*/
void UiHorizontalGaugeControl_UpdateRuntimeRangeAndDraw
          (int clipBottom,int clipRight,int clipTop,int clipLeft,UiHorizontalGaugeControl *control)

{
  UiTransferPayloadByteCount receivedTotal;
  uint32_t minimumProgress;
  uint32_t receivedDone;
  int remainingPlayers;
  FrontendPlayerRuntimeRecord *playerRecord;

  playerRecord = g_FrontendPlayerRuntimeBlocks;
  receivedTotal = g_UiTransferMailbox.receivedByteCount;
  minimumProgress = g_UiTransferMailbox.outgoingByteCount;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    if (g_UiTransferMailbox.outgoingByteCount == 0) {
      return;
    }
    remainingPlayers = g_FrontendPlayerRuntimeBlockCount - 1;
    if (remainingPlayers == 0) {
      return;
    }
    control->minimumValue = 0;
    control->maximumValue = minimumProgress;
    /* the clients follow the host's own block 0 */
    do {
      if ((int)playerRecord[1].transferProgressBytes < (int)minimumProgress) {
        minimumProgress = playerRecord[1].transferProgressBytes;
      }
      remainingPlayers--;
      playerRecord = playerRecord + 1;
    } while (remainingPlayers != 0);
    control->value = minimumProgress;
    if (control->maximumValue <= minimumProgress) {
      return;
    }
  }
  else {
    if ((g_UiTransferMailbox.receivedByteCount == 0) &&
       (g_UiTransferMailbox.receivedRemainingBytes == 0)) {
      return;
    }
    receivedDone = g_UiTransferMailbox.receivedByteCount - g_UiTransferMailbox.receivedRemainingBytes;
    control->minimumValue = 0;
    control->maximumValue = receivedTotal;
    control->value = receivedDone;
    if (receivedTotal <= receivedDone) {
      return;
    }
  }
  UiHorizontalGaugeControl_DrawFrameFillAndLabel(clipBottom,clipRight,clipTop,clipLeft,control);
  return;
}


/* One step of the sorted insert in UiDisplaySettings_OpenAndPopulateModeSelection: a value below the slot
   takes the slot and the displaced slot value moves on to the next slot; otherwise the value itself moves on.
   Returns the value that continues to the next slot. */
static DisplayModeScratchWord UiDisplaySettings_InsertIntoSortedSlot(DisplayModeScratchWord *slot,
                                                                     DisplayModeScratchWord value)
{
  DisplayModeScratchWord carriedValue;

  carriedValue = value;
  if (value < *slot) {
    carriedValue = *slot;
    *slot = value;
  }
  return carriedValue;
}

/* Address: 0x00423600.
   Opens the display settings dialog (only when more than one display mode was enumerated): copies
   g_UiDisplaySettingsRootTemplate to the heap, records the current mode and colour bias/scale as both the
   selected and the original values, installs its action handlers and pushes it. The option buttons are then
   labelled with the enumerated values in ascending order: up to 4 distinct bit depths, 8 resolutions and 5
   adapters (a sorted insert into the g_UiDisplayModeDistinctValueScratch slots, 0xFFFFFFFF = empty).
   Reopened by UiDisplayModeAction_RevertAndReopenSettings. The original sets CF when the
   allocation fails; that caller ignores it.
*/
void UiDisplaySettings_OpenAndPopulateModeSelection(void)

{
  uint32_t framebufferWidth;
  uint32_t framebufferHeight;
  UiRootFlags activeAdapterIndex;
  int32_t colorScaleQ16;
  int32_t colorBiasQ16;
  UiRootNode *root;
  uint32_t insertValue;
  int copyCount;
  int redGreenBits;
  UiNodeFlags colorDepthBits;
  GraphicsDisplayModeCount remainingModes;
  uint32_t *copyCursor;
  const uint32_t *templateCursor;
  GraphicsDisplayMode *displayMode;
  UiDisplaySettingsApplyButton *applyButton;

  if (g_GraphicsDisplayModeCount <= 1) {
    return;
  }
  if (g_MemoryApi.alloc(sizeof(DisplaySettingsUiImage),(void **)&root) != 0) {
    return;
  }
  /* copy the template, one dword per step */
  templateCursor = (const uint32_t *)&g_UiDisplaySettingsRootTemplate;
  copyCursor = (uint32_t *)root;
  for (copyCount = sizeof(DisplaySettingsUiImage) / 4; copyCount != 0; copyCount--) {
    *copyCursor = *templateCursor;
    templateCursor = templateCursor + 1;
    copyCursor = copyCursor + 1;
  }
  activeAdapterIndex = g_ActiveGraphicsAdapterIndex;
  framebufferHeight = g_FramebufferHeight;
  framebufferWidth = g_FramebufferWidth;
  colorDepthBits = g_SoftwarePixelFormatConfig.redBitCount + g_SoftwarePixelFormatConfig.greenBitCount +
           g_SoftwarePixelFormatConfig.blueBitCount;
  /* applyButton tail: the selected mode tuple and the original one start out equal */
  applyButton = (UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton);
  applyButton->selectedWidth = framebufferWidth;
  applyButton->selectedHeight = framebufferHeight;
  applyButton->selectedBitsPerPixel = colorDepthBits;
  applyButton->selectedAdapterIndex = activeAdapterIndex;
  applyButton->originalWidth = framebufferWidth;
  applyButton->originalHeight = framebufferHeight;
  applyButton->originalBitsPerPixel = colorDepthBits;
  applyButton->originalAdapterIndex = activeAdapterIndex;
  colorBiasQ16 = g_SoftwareColorBiasQ16;
  colorScaleQ16 = g_SoftwareColorScaleQ16;
  applyButton->selectedColorBiasQ16 = colorBiasQ16;
  applyButton->selectedColorScaleQ16 = colorScaleQ16;
  applyButton->originalColorBiasQ16 = colorBiasQ16;
  applyButton->originalColorScaleQ16 = colorScaleQ16;
  ((UiRangeSliderControl *)DISPLAY_SETTINGS_UI(root,colorBiasSlider))->value = colorBiasQ16;
  ((UiRangeSliderControl *)DISPLAY_SETTINGS_UI(root,colorScaleSlider))->value = colorScaleQ16;
  /* The two readouts show the number buffers kept in the tail of colorBiasValueText. */
  ((UiSingleLineTextControl *)DISPLAY_SETTINGS_UI(root,colorBiasValueText))->text =
       ((UiDisplaySettingsValueReadout *)DISPLAY_SETTINGS_UI(root,colorBiasValueText))->colorBiasTextUtf16;
  ((UiSingleLineTextControl *)DISPLAY_SETTINGS_UI(root,colorScaleValueText))->text =
       ((UiDisplaySettingsValueReadout *)DISPLAY_SETTINGS_UI(root,colorBiasValueText))->colorScaleTextUtf16;
  UiDisplaySettingsRoot_FormatColorReadouts(root);
  UiActionHandlers_SetPage(UI_DISPLAY_MODE_ACTION_HANDLER_PAGE,
                           (UiActionHandlerPage *)&g_UiDisplayModeSelectionActionHandlers20);
  UiRootStack_Push(&g_UiDisplaySettingsRootCallbacks,root);

  /* distinct bit depths, ascending */
  g_UiDisplayModeDistinctValueScratch[0] = UI_DISPLAY_MODE_NONE;
  g_UiDisplayModeDistinctValueScratch[1] = UI_DISPLAY_MODE_NONE;
  g_UiDisplayModeDistinctValueScratch[2] = UI_DISPLAY_MODE_NONE;
  g_UiDisplayModeDistinctValueScratch[3] = UI_DISPLAY_MODE_NONE;
  displayMode = g_GraphicsDisplayModes;
  for (remainingModes = g_GraphicsDisplayModeCount; remainingModes != 0; remainingModes--) {
    insertValue = displayMode->bitsPerPixel;
    if (insertValue != g_UiDisplayModeDistinctValueScratch[0] && insertValue != g_UiDisplayModeDistinctValueScratch[1] &&
        insertValue != g_UiDisplayModeDistinctValueScratch[2] && insertValue != g_UiDisplayModeDistinctValueScratch[3]) {
      insertValue = UiDisplaySettings_InsertIntoSortedSlot(&g_UiDisplayModeDistinctValueScratch[0],insertValue);
      insertValue = UiDisplaySettings_InsertIntoSortedSlot(&g_UiDisplayModeDistinctValueScratch[1],insertValue);
      insertValue = UiDisplaySettings_InsertIntoSortedSlot(&g_UiDisplayModeDistinctValueScratch[2],insertValue);
      UiDisplaySettings_InsertIntoSortedSlot(&g_UiDisplayModeDistinctValueScratch[3],insertValue);
    }
    displayMode = displayMode + 1;
  }
  /* Each option button's mode value(s) sit in its <button>_prefix, the dwords just before the button (read
     back by the action callbacks as UiDisplayModeOptionPrefix.modeValue / .resolutionHeight). */
  DISPLAY_SETTINGS_UI(root,colorDepthOption1_prefix)->modeValue = g_UiDisplayModeDistinctValueScratch[0];
  DISPLAY_SETTINGS_UI(root,colorDepthOption2_prefix)->modeValue = g_UiDisplayModeDistinctValueScratch[1];
  DISPLAY_SETTINGS_UI(root,colorDepthOption3_prefix)->modeValue = g_UiDisplayModeDistinctValueScratch[2];
  DISPLAY_SETTINGS_UI(root,colorDepthOption4_prefix)->modeValue = g_UiDisplayModeDistinctValueScratch[3];

  /* distinct resolutions, keyed width << 16 | height so that they sort by width, then height */
  g_UiDisplayModeDistinctValueScratch[0] = UI_DISPLAY_MODE_NONE;
  g_UiDisplayModeDistinctValueScratch[1] = UI_DISPLAY_MODE_NONE;
  g_UiDisplayModeDistinctValueScratch[2] = UI_DISPLAY_MODE_NONE;
  g_UiDisplayModeDistinctValueScratch[3] = UI_DISPLAY_MODE_NONE;
  g_UiDisplayModeDistinctValueScratch[4] = UI_DISPLAY_MODE_NONE;
  g_UiDisplayModeDistinctValueScratch[5] = UI_DISPLAY_MODE_NONE;
  g_UiDisplayModeDistinctValueScratch[6] = UI_DISPLAY_MODE_NONE;
  g_UiDisplayModeDistinctValueScratch[7] = UI_DISPLAY_MODE_NONE;
  displayMode = g_GraphicsDisplayModes;
  for (remainingModes = g_GraphicsDisplayModeCount; remainingModes != 0; remainingModes--) {
    insertValue = displayMode->width * UI_DISPLAY_MODE_WIDTH_SCALE + displayMode->height;
    if (insertValue != g_UiDisplayModeDistinctValueScratch[0] && insertValue != g_UiDisplayModeDistinctValueScratch[1] &&
        insertValue != g_UiDisplayModeDistinctValueScratch[2] && insertValue != g_UiDisplayModeDistinctValueScratch[3] &&
        insertValue != g_UiDisplayModeDistinctValueScratch[4] && insertValue != g_UiDisplayModeDistinctValueScratch[5] &&
        insertValue != g_UiDisplayModeDistinctValueScratch[6] && insertValue != g_UiDisplayModeDistinctValueScratch[7]) {
      insertValue = UiDisplaySettings_InsertIntoSortedSlot(&g_UiDisplayModeDistinctValueScratch[0],insertValue);
      insertValue = UiDisplaySettings_InsertIntoSortedSlot(&g_UiDisplayModeDistinctValueScratch[1],insertValue);
      insertValue = UiDisplaySettings_InsertIntoSortedSlot(&g_UiDisplayModeDistinctValueScratch[2],insertValue);
      insertValue = UiDisplaySettings_InsertIntoSortedSlot(&g_UiDisplayModeDistinctValueScratch[3],insertValue);
      insertValue = UiDisplaySettings_InsertIntoSortedSlot(&g_UiDisplayModeDistinctValueScratch[4],insertValue);
      insertValue = UiDisplaySettings_InsertIntoSortedSlot(&g_UiDisplayModeDistinctValueScratch[5],insertValue);
      insertValue = UiDisplaySettings_InsertIntoSortedSlot(&g_UiDisplayModeDistinctValueScratch[6],insertValue);
      UiDisplaySettings_InsertIntoSortedSlot(&g_UiDisplayModeDistinctValueScratch[7],insertValue);
    }
    displayMode = displayMode + 1;
  }
  /* Resolution buttons: width at -8, height at -0xC. */
  DISPLAY_SETTINGS_UI(root,resolutionOption1_prefix)->modeValue = g_UiDisplayModeDistinctValueScratch[0] >> 16;
  DISPLAY_SETTINGS_UI(root,resolutionOption1_prefix)->resolutionHeight = g_UiDisplayModeDistinctValueScratch[0] & UI_DISPLAY_MODE_HEIGHT_MASK;
  DISPLAY_SETTINGS_UI(root,resolutionOption2_prefix)->modeValue = g_UiDisplayModeDistinctValueScratch[1] >> 16;
  DISPLAY_SETTINGS_UI(root,resolutionOption2_prefix)->resolutionHeight = g_UiDisplayModeDistinctValueScratch[1] & UI_DISPLAY_MODE_HEIGHT_MASK;
  DISPLAY_SETTINGS_UI(root,resolutionOption3_prefix)->modeValue = g_UiDisplayModeDistinctValueScratch[2] >> 16;
  DISPLAY_SETTINGS_UI(root,resolutionOption3_prefix)->resolutionHeight = g_UiDisplayModeDistinctValueScratch[2] & UI_DISPLAY_MODE_HEIGHT_MASK;
  DISPLAY_SETTINGS_UI(root,resolutionOption4_prefix)->modeValue = g_UiDisplayModeDistinctValueScratch[3] >> 16;
  DISPLAY_SETTINGS_UI(root,resolutionOption4_prefix)->resolutionHeight = g_UiDisplayModeDistinctValueScratch[3] & UI_DISPLAY_MODE_HEIGHT_MASK;
  DISPLAY_SETTINGS_UI(root,resolutionOption5_prefix)->modeValue = g_UiDisplayModeDistinctValueScratch[4] >> 16;
  DISPLAY_SETTINGS_UI(root,resolutionOption5_prefix)->resolutionHeight = g_UiDisplayModeDistinctValueScratch[4] & UI_DISPLAY_MODE_HEIGHT_MASK;
  DISPLAY_SETTINGS_UI(root,resolutionOption6_prefix)->modeValue = g_UiDisplayModeDistinctValueScratch[5] >> 16;
  DISPLAY_SETTINGS_UI(root,resolutionOption6_prefix)->resolutionHeight = g_UiDisplayModeDistinctValueScratch[5] & UI_DISPLAY_MODE_HEIGHT_MASK;
  DISPLAY_SETTINGS_UI(root,resolutionOption7_prefix)->modeValue = g_UiDisplayModeDistinctValueScratch[6] >> 16;
  DISPLAY_SETTINGS_UI(root,resolutionOption7_prefix)->resolutionHeight = g_UiDisplayModeDistinctValueScratch[6] & UI_DISPLAY_MODE_HEIGHT_MASK;
  DISPLAY_SETTINGS_UI(root,resolutionOption8_prefix)->modeValue = g_UiDisplayModeDistinctValueScratch[7] >> 16;
  DISPLAY_SETTINGS_UI(root,resolutionOption8_prefix)->resolutionHeight = g_UiDisplayModeDistinctValueScratch[7] & UI_DISPLAY_MODE_HEIGHT_MASK;

  /* distinct adapters */
  g_UiDisplayModeDistinctValueScratch[0] = UI_DISPLAY_MODE_NONE;
  g_UiDisplayModeDistinctValueScratch[1] = UI_DISPLAY_MODE_NONE;
  g_UiDisplayModeDistinctValueScratch[2] = UI_DISPLAY_MODE_NONE;
  g_UiDisplayModeDistinctValueScratch[3] = UI_DISPLAY_MODE_NONE;
  g_UiDisplayModeDistinctValueScratch[4] = UI_DISPLAY_MODE_NONE;
  displayMode = g_GraphicsDisplayModes;
  for (remainingModes = g_GraphicsDisplayModeCount; remainingModes != 0; remainingModes--) {
    insertValue = displayMode->adapterIndex;
    if (insertValue != g_UiDisplayModeDistinctValueScratch[0] && insertValue != g_UiDisplayModeDistinctValueScratch[1] &&
        insertValue != g_UiDisplayModeDistinctValueScratch[2] && insertValue != g_UiDisplayModeDistinctValueScratch[3] &&
        insertValue != g_UiDisplayModeDistinctValueScratch[4]) {
      insertValue = UiDisplaySettings_InsertIntoSortedSlot(&g_UiDisplayModeDistinctValueScratch[0],insertValue);
      insertValue = UiDisplaySettings_InsertIntoSortedSlot(&g_UiDisplayModeDistinctValueScratch[1],insertValue);
      insertValue = UiDisplaySettings_InsertIntoSortedSlot(&g_UiDisplayModeDistinctValueScratch[2],insertValue);
      insertValue = UiDisplaySettings_InsertIntoSortedSlot(&g_UiDisplayModeDistinctValueScratch[3],insertValue);
      UiDisplaySettings_InsertIntoSortedSlot(&g_UiDisplayModeDistinctValueScratch[4],insertValue);
    }
    displayMode = displayMode + 1;
  }
  /* Adapter buttons: adapter index at -8. */
  DISPLAY_SETTINGS_UI(root,adapterOption1_prefix)->modeValue = g_UiDisplayModeDistinctValueScratch[0];
  DISPLAY_SETTINGS_UI(root,adapterOption2_prefix)->modeValue = g_UiDisplayModeDistinctValueScratch[1];
  DISPLAY_SETTINGS_UI(root,adapterOption3_prefix)->modeValue = g_UiDisplayModeDistinctValueScratch[2];
  DISPLAY_SETTINGS_UI(root,adapterOption4_prefix)->modeValue = g_UiDisplayModeDistinctValueScratch[3];
  DISPLAY_SETTINGS_UI(root,adapterOption5_prefix)->modeValue = g_UiDisplayModeDistinctValueScratch[4];
  /* the bit depth travels in a UiNodeBase * parameter slot of that function */
  redGreenBits = g_SoftwarePixelFormatConfig.redBitCount + g_SoftwarePixelFormatConfig.greenBitCount;
  UiDisplayModeSelection_RefreshEnumeratedOptions
            (g_ActiveGraphicsAdapterIndex,
             (UiNodeBase *)(redGreenBits + g_SoftwarePixelFormatConfig.blueBitCount),g_FramebufferHeight,
             g_FramebufferWidth,(UiNodeBase *)root);
  UiRootStack_InvalidateAll();
  return;
}


/* Address: 0x004BC9B0.
   Hit test of an image control: only opaque pixels of its current image count, so irregular shapes react
   precisely. A miss clears UI_IMAGE_CONTROL_PRESSED_ON_IMAGE and, in persistent activation mode, passes the
   test on to the children. Returns the hit node or UI_NODE_NONE.
*/
UiNodeBase * UiImageControl_HitTestOpaque(UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiImageControl *control)

{
  UiNodeBase *hitNode;
  bool opaqueHit;

  if ((control->selectable.base.nodeFlags & UI_NODE_SUPPRESSED) != 0) {
    return UI_NODE_NONE;
  }
  if ((control->selectable.stateFlags & UI_IMAGE_CONTROL_ALTERNATE_HIT_SHAPE) == 0) {
    opaqueHit = g_GraphicsTextureSourceTestOpaquePixel
                      (pointerY,pointerX,control->selectable.base.top,
                       control->selectable.base.left,control->normalSubresource,
                       control->textureSource);
  }
  else {
    opaqueHit = g_GraphicsTextureSourceTestOpaquePixel
                      (pointerY,pointerX,control->selectable.base.top,
                       control->selectable.base.left,control->alternateSubresource,
                       control->textureSource);
  }
  if (opaqueHit) {
    return (UiNodeBase *)control;
  }
  control->selectable.stateFlags &= ~UI_IMAGE_CONTROL_PRESSED_ON_IMAGE;
  if ((control->selectable.stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) == 0) {
    return UI_NODE_NONE;
  }
  /* UiContainer_HitTestChildren returns the control itself when no child is hit; that only counts
     while g_UiImageControlHoverTarget is NULL */
  hitNode = UiContainer_HitTestChildren(pointerY,pointerX,(UiNodeBase *)control);
  if (hitNode == (UiNodeBase *)control && g_UiImageControlHoverTarget != NULL) {
    return UI_NODE_NONE;
  }
  return hitNode;
}


/* Address: 0x00423D70.
   Refreshes the display settings dialog for a selected mode (adapterIndex, bit depth, height, width): every
   colour-depth, resolution and adapter button whose combination with the other selected values was not
   enumerated (GraphicsDisplayMode_IsEnumerated) is suppressed, the others are enabled; the buttons matching
   the selection are selected in their groups; the tuple is stored as the selected one, and the apply button
   is suppressed while it and the colour bias/scale equal the values the dialog opened with.
   The bit depth arrives in the pointer-typed selectedModeValue parameter.
*/
void UiDisplayModeSelection_RefreshEnumeratedOptions
          (FrontendDisplayAdapterIndex adapterIndex,UiNodeBase *selectedModeValue,
          FrontendDisplayDimensionPixels modeHeight,FrontendDisplayDimensionPixels modeWidth,
          UiNodeBase *displaySettingsRoot)

{
  /* Rewritten from the assembly (0x00423D70): every option button is a 0x68-byte node; the dwords
     just before each button hold its mode value(s). The decompiled struct indexing picked wrong
     fields, so no resolution button was ever marked as selected. */
  static const unsigned depthButtons[4] = {offsetof(DisplaySettingsUiImage,colorDepthOption1),
      offsetof(DisplaySettingsUiImage,colorDepthOption2),offsetof(DisplaySettingsUiImage,colorDepthOption3),
      offsetof(DisplaySettingsUiImage,colorDepthOption4)};
  static const unsigned sizeButtons[8] = {offsetof(DisplaySettingsUiImage,resolutionOption1),
      offsetof(DisplaySettingsUiImage,resolutionOption2),offsetof(DisplaySettingsUiImage,resolutionOption3),
      offsetof(DisplaySettingsUiImage,resolutionOption4),offsetof(DisplaySettingsUiImage,resolutionOption5),
      offsetof(DisplaySettingsUiImage,resolutionOption6),offsetof(DisplaySettingsUiImage,resolutionOption7),
      offsetof(DisplaySettingsUiImage,resolutionOption8)};
  static const unsigned adapterButtons[5] = {offsetof(DisplaySettingsUiImage,adapterOption1),
      offsetof(DisplaySettingsUiImage,adapterOption2),offsetof(DisplaySettingsUiImage,adapterOption3),
      offsetof(DisplaySettingsUiImage,adapterOption4),offsetof(DisplaySettingsUiImage,adapterOption5)};
  uint8_t *root = (uint8_t *)displaySettingsRoot;
  UiDisplaySettingsApplyButton *applyButton = (UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton);
  uint32_t bitsPerPixel = (uint32_t)(uintptr_t)selectedModeValue;
  void *selected = NULL;
  bool modeMissing; /* GraphicsDisplayMode_IsEnumerated returns true when the mode was not enumerated */
  int i;

  for (i = 0; i < 4; i++) {
    uint32_t depth = DISPLAY_MODE_OPTION_PREFIX(root + depthButtons[i]).modeValue;
    modeMissing = GraphicsDisplayMode_IsEnumerated(adapterIndex,depth,modeHeight,modeWidth);
    if (modeMissing) {
      UiNodeList_SuppressActionId(UI_DISPLAY_MODE_ACTION_FIRST_COLOR_DEPTH + i,displaySettingsRoot);
    }
    else {
      UiNodeList_UnsuppressActionId(UI_DISPLAY_MODE_ACTION_FIRST_COLOR_DEPTH + i,displaySettingsRoot);
    }
    if (bitsPerPixel == depth) {
      /* The original stores EAX (the width) instead of the button for the fourth depth. */
      selected = (i == 3) ? (void *)(uintptr_t)modeWidth : (void *)(root + depthButtons[i]);
    }
  }
  UiSelectableGroup_SelectExclusive(4,selected,
      DISPLAY_SETTINGS_UI(displaySettingsRoot,colorDepthOption4),
      DISPLAY_SETTINGS_UI(displaySettingsRoot,colorDepthOption3),
      DISPLAY_SETTINGS_UI(displaySettingsRoot,colorDepthOption2),
      DISPLAY_SETTINGS_UI(displaySettingsRoot,colorDepthOption1));
  for (i = 0; i < 8; i++) {
    uint32_t height = DISPLAY_MODE_OPTION_PREFIX(root + sizeButtons[i]).resolutionHeight;
    uint32_t width = DISPLAY_MODE_OPTION_PREFIX(root + sizeButtons[i]).modeValue;
    modeMissing = GraphicsDisplayMode_IsEnumerated(adapterIndex,bitsPerPixel,height,width);
    if (modeMissing) {
      UiNodeList_SuppressActionId(UI_DISPLAY_MODE_ACTION_FIRST_RESOLUTION + i,displaySettingsRoot);
    }
    else {
      UiNodeList_UnsuppressActionId(UI_DISPLAY_MODE_ACTION_FIRST_RESOLUTION + i,displaySettingsRoot);
    }
    if ((modeWidth == width) && (modeHeight == height)) {
      selected = root + sizeButtons[i];
    }
  }
  UiSelectableGroup_SelectExclusive(8,selected,
      DISPLAY_SETTINGS_UI(displaySettingsRoot,resolutionOption8),
      DISPLAY_SETTINGS_UI(displaySettingsRoot,resolutionOption7),
      DISPLAY_SETTINGS_UI(displaySettingsRoot,resolutionOption6),
      DISPLAY_SETTINGS_UI(displaySettingsRoot,resolutionOption5),
      DISPLAY_SETTINGS_UI(displaySettingsRoot,resolutionOption4),
      DISPLAY_SETTINGS_UI(displaySettingsRoot,resolutionOption3),
      DISPLAY_SETTINGS_UI(displaySettingsRoot,resolutionOption2),
      DISPLAY_SETTINGS_UI(displaySettingsRoot,resolutionOption1));
  for (i = 0; i < 5; i++) {
    uint32_t adapter = DISPLAY_MODE_OPTION_PREFIX(root + adapterButtons[i]).modeValue;
    modeMissing = GraphicsDisplayMode_IsEnumerated(adapter,bitsPerPixel,modeHeight,modeWidth);
    if (modeMissing) {
      UiNodeList_SuppressActionId(UI_DISPLAY_MODE_ACTION_FIRST_ADAPTER + i,displaySettingsRoot);
    }
    else {
      UiNodeList_UnsuppressActionId(UI_DISPLAY_MODE_ACTION_FIRST_ADAPTER + i,displaySettingsRoot);
    }
    if (adapterIndex == adapter) {
      selected = root + adapterButtons[i];
    }
  }
  UiSelectableGroup_SelectExclusive(5,selected,
      DISPLAY_SETTINGS_UI(displaySettingsRoot,adapterOption5),
      DISPLAY_SETTINGS_UI(displaySettingsRoot,adapterOption4),
      DISPLAY_SETTINGS_UI(displaySettingsRoot,adapterOption3),
      DISPLAY_SETTINGS_UI(displaySettingsRoot,adapterOption2),
      DISPLAY_SETTINGS_UI(displaySettingsRoot,adapterOption1));
  applyButton->selectedWidth = modeWidth;
  applyButton->selectedHeight = modeHeight;
  applyButton->selectedBitsPerPixel = bitsPerPixel;
  applyButton->selectedAdapterIndex = adapterIndex;
  if ((modeWidth == (uint32_t)applyButton->originalWidth) &&
      (modeHeight == (uint32_t)applyButton->originalHeight) &&
      (bitsPerPixel == applyButton->originalBitsPerPixel) &&
      (adapterIndex == applyButton->originalAdapterIndex) &&
      (applyButton->selectedColorBiasQ16 ==
       applyButton->originalColorBiasQ16) &&
      (applyButton->selectedColorScaleQ16 ==
       applyButton->originalColorScaleQ16)) {
    UiNodeList_SuppressActionId(UI_DISPLAY_MODE_ACTION_APPLY,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(UI_DISPLAY_MODE_ACTION_APPLY,displaySettingsRoot);
  }
  return;
}

