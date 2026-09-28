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

  /* the slider values (UiRangeSliderControl.value, +0x58) */
  colorBiasQ16 = ((UiRangeSliderControl *)DISPLAY_SETTINGS_UI(root,colorBiasSlider))->value;
  colorScaleQ16 = ((UiRangeSliderControl *)DISPLAY_SETTINGS_UI(root,colorScaleSlider))->value;
  if ((colorBiasQ16 != ((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->selectedColorBiasQ16) ||
      (colorScaleQ16 != ((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->selectedColorScaleQ16)) {
    ((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->selectedColorBiasQ16 = colorBiasQ16;
    ((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->selectedColorScaleQ16 = colorScaleQ16;
    g_SoftwareBuildPixelPackTables(colorScaleQ16,colorBiasQ16);
    UiDisplayModeSelection_RefreshEnumeratedOptions
              (((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->selectedAdapterIndex,
               (UiNodeBase *)((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->selectedBitsPerPixel,
               ((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->selectedHeight,
               ((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->selectedWidth,&root->base);
    UiRuntime_FormatSignedValues140And144(root);
  }
  return;
}


/* Address: 0x00423C40.
   Handler of the four colour-depth buttons (actions 0x201..0x204, g_UiDisplayModeSelectionActionHandlers20[1..4])
   of the display settings dialog, despite its name: selects the button's bit depth, keeps the selected
   adapter and resolution and refreshes the available buttons. Each option button keeps its value in the
   dword 8 bytes before it (UiDisplayModeOptionPrefix.modeValue). UiDisplayModeAction_UpdateColorDepthSelection is its
   mirror image for the adapter buttons; the two names are swapped.
*/
void UiDisplayModeAction_UpdateAdapterSelection(UiNodeBase *sourceNode)

{
  UiNodeBase *displaySettingsRoot;

  displaySettingsRoot = UiNode_GetRoot(sourceNode);
  UiDisplayModeSelection_RefreshEnumeratedOptions
            ((FrontendDisplayAdapterIndex)((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(displaySettingsRoot,applyButton))->selectedAdapterIndex,
             (UiNodeBase *)((UiDisplayModeOptionPrefix *)sourceNode)[-1].modeValue,
             (FrontendDisplayDimensionPixels)((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(displaySettingsRoot,applyButton))->selectedHeight,
             (FrontendDisplayDimensionPixels)((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(displaySettingsRoot,applyButton))->selectedWidth,displaySettingsRoot)
  ;
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

  displaySettingsRoot = UiNode_GetRoot(sourceNode);
  UiDisplayModeSelection_RefreshEnumeratedOptions
            ((FrontendDisplayAdapterIndex)((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(displaySettingsRoot,applyButton))->selectedAdapterIndex,
             (struct UiNodeBase *)((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(displaySettingsRoot,applyButton))->selectedBitsPerPixel,
             ((UiDisplayModeOptionPrefix *)sourceNode)[-1].resolutionHeight,((UiDisplayModeOptionPrefix *)sourceNode)[-1].modeValue,displaySettingsRoot);
  return;
}


/* Address: 0x00423CB0.
   Handler of the five adapter buttons (actions 0x20F..0x213, g_UiDisplayModeSelectionActionHandlers20[15..19])
   of the display settings dialog, despite its name: selects the button's adapter (the dword 8 bytes before
   the button), keeps the selected resolution and bit depth and refreshes the available buttons. See
   UiDisplayModeAction_UpdateAdapterSelection, whose name it has swapped.
*/
void UiDisplayModeAction_UpdateColorDepthSelection(UiNodeBase *sourceNode)

{
  UiNodeBase *displaySettingsRoot;

  displaySettingsRoot = UiNode_GetRoot(sourceNode);
  UiDisplayModeSelection_RefreshEnumeratedOptions
            (((UiDisplayModeOptionPrefix *)sourceNode)[-1].modeValue,(struct UiNodeBase *)((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(displaySettingsRoot,applyButton))->selectedBitsPerPixel,
             (FrontendDisplayDimensionPixels)((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(displaySettingsRoot,applyButton))->selectedHeight,
             (FrontendDisplayDimensionPixels)((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(displaySettingsRoot,applyButton))->selectedWidth,displaySettingsRoot)
  ;
  return;
}


/* Address: 0x00424590.
   Revert action (UI_DISPLAY_MODE_ACTION_REVERT, g_UiDisplayModeSelectionActionHandlers20[13]) of the "keep
   the new display mode?" dialog, from its button or from the expired countdown: closes the dialog, switches
   back to the previous display mode stored in it (a failure is fatal), lays out every open root for the
   restored framebuffer size and opens the display settings dialog again. The name is misleading: nothing is
   applied.
*/
void UiDisplayModeAction_ApplyFourValueDialogAndReopenSettings(UiNodeBase *sourceNode)

{
  int64_t scaledAnchor;
  UiRootNode *root;
  DisplayModeResult modeResult;
  uint32_t adapterIndex;
  uint32_t bitsPerPixel;
  uint32_t modeHeight;
  uint32_t modeWidth;

  root = (UiRootNode *)UiNode_GetRoot(sourceNode);
  modeWidth = ((UiFourValueDialogCountdownText *)FOUR_VALUE_DIALOG_UI(root,countdownMessageText))->previousWidth;
  modeHeight = ((UiFourValueDialogCountdownText *)FOUR_VALUE_DIALOG_UI(root,countdownMessageText))->previousHeight;
  bitsPerPixel = ((UiFourValueDialogCountdownText *)FOUR_VALUE_DIALOG_UI(root,countdownMessageText))->previousBitsPerPixel;
  adapterIndex = ((UiFourValueDialogCountdownText *)FOUR_VALUE_DIALOG_UI(root,countdownMessageText))->previousAdapterIndex;
  UiRootStack_Pop(root);
  g_CursorVisibilityToken--;
  UiFrame_ProcessAndPresentWithLockTransition();
  modeResult = g_GraphicsSetDisplayMode(adapterIndex,bitsPerPixel,modeHeight,modeWidth);
  FatalError_ExitIfFailed(modeResult.valueOrError,modeResult.failed);
  /* the loop of UiRootStack_Relayout, inlined: edge = framebuffer size * anchor (Q31) + offset */
  root = g_UiRootNode;
  do {
    scaledAnchor = (uint64_t)g_FramebufferWidth * (uint64_t)(root->base).rightAnchorQ31;
    (root->base).right =
         ((int)((uint64_t)scaledAnchor >> 0x20) << 1 | (uint32_t)scaledAnchor >> 0x1f) + (root->base).rightOffset;
    scaledAnchor = (uint64_t)g_FramebufferHeight * (uint64_t)(root->base).bottomAnchorQ31;
    (root->base).bottom =
         ((int)((uint64_t)scaledAnchor >> 0x20) << 1 | (uint32_t)scaledAnchor >> 0x1f) + (root->base).bottomOffset;
    scaledAnchor = (uint64_t)g_FramebufferWidth * (uint64_t)(root->base).leftAnchorQ31;
    (root->base).left =
         ((int)((uint64_t)scaledAnchor >> 0x20) << 1 | (uint32_t)scaledAnchor >> 0x1f) + (root->base).leftOffset;
    scaledAnchor = (uint64_t)g_FramebufferHeight * (uint64_t)(root->base).topAnchorQ31;
    (root->base).top =
         ((int)((uint64_t)scaledAnchor >> 0x20) << 1 | (uint32_t)scaledAnchor >> 0x1f) + (root->base).topOffset;
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
  UiNodeBase *newActiveChild;
  UiNodeBase *previousActiveChild;
  UiSelectableStateFlags *stateFlagsField;

  if (((control->selectable).stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) == 0) {
    return;
  }
  hitControl = (UiImageControl *)UiImageControl_HitTestOpaque(pointerY,pointerX,control);
  /* Off the image's own pixels bit 0x200 is cleared. Over the image itself or over nothing the current
     child only gets the synthetic drag and release; it stays activeChild (as in the original). */
  if ((hitControl == control) ||
     (stateFlagsField = &(control->selectable).stateFlags,
     *stateFlagsField = *stateFlagsField & 0xfffffdff, hitControl == (UiImageControl *)UI_NODE_NONE))
  {
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
    newActiveChild = (UiNodeBase *)hitControl;
    /* XCHG in the original */
    LOCK();
    previousActiveChild = control->activeChild;
    control->activeChild = newActiveChild;
    UNLOCK();
  }
  if (previousActiveChild != NULL) {
    previousActiveChild->vtable->nonRightDrag(0,UI_POINTER_FAR_OUTSIDE,UI_POINTER_FAR_OUTSIDE,previousActiveChild);
    previousActiveChild->vtable->nonRightRelease(0,UI_POINTER_FAR_OUTSIDE,UI_POINTER_FAR_OUTSIDE,previousActiveChild);
  }
  UiRootStack_InvalidateAll();
  return;
}


/* Address: 0x004BCB50.
   tick of the image control (g_UiNodeVtable_004BC570): when the right mouse button goes down (latched in
   state bit 0x100 until it is released), an opaque child under the cursor gets a release, press and drag at
   the current cursor position, so that it re-evaluates the pointer; state bit 0x800 is cleared then.
*/
void UiImageControl_TickHover(UiImageControl *control)

{
  UiSelectableStateFlags *clearStateFlagsField;
  UiNodeVtable *hitChildVtable;
  UiImageControl *hitControl;
  UiSelectableStateFlags *stateFlagsField;
  UiSelectableStateFlags *hoverStateFlagsField;

  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (((control->selectable).stateFlags & 0x100) == 0) {
      if ((g_CursorButtonState & RIGHT) != 0) {
        hitControl = (UiImageControl *)
                     UiImageControl_HitTestOpaque(g_CursorOverrideY,g_CursorOverrideX,control);
        stateFlagsField = &(control->selectable).stateFlags;
        *stateFlagsField = *stateFlagsField | 0x100;
        if ((hitControl != control) && (hitControl != (UiImageControl *)UI_NODE_NONE)) {
          hitChildVtable = (hitControl->selectable).base.vtable;
          hoverStateFlagsField = &(control->selectable).stateFlags;
          *hoverStateFlagsField = *hoverStateFlagsField & 0xfffff7ff;
          hitChildVtable->nonRightRelease
                    (g_CursorWheelDelta,g_CursorOverrideY,g_CursorOverrideX,(UiNodeBase *)hitControl
                    );
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
      *clearStateFlagsField = *clearStateFlagsField & 0xfffffeff;
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
  uint32_t pendingModeWidth;
  uint32_t pendingModeHeight;
  uint32_t currentBitsPerPixel;
  DisplayModeResult pendingModeResult;
  DisplayModeResult restoreModeResult;
  uint32_t currentAdapterIndex;
  uint32_t pendingWidthOrCurrentHeight;
  uint32_t pendingHeightOrCurrentWidth;

  root = (UiRootNode *)UiNode_GetRoot(sourceNode);
  pendingWidthOrCurrentHeight = ((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->selectedWidth;
  pendingHeightOrCurrentWidth = ((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->selectedHeight;
  pendingBitsPerPixel = ((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->selectedBitsPerPixel;
  currentBitsPerPixel = g_SoftwarePixelFormatConfig.redBitCount + g_SoftwarePixelFormatConfig.greenBitCount +
            g_SoftwarePixelFormatConfig.blueBitCount;
  pendingAdapterIndex = ((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->selectedAdapterIndex;
  UiRootStack_Pop(root);
  if ((((pendingWidthOrCurrentHeight != g_FramebufferWidth) || (pendingHeightOrCurrentWidth != g_FramebufferHeight)) || (pendingBitsPerPixel != currentBitsPerPixel)) ||
     (pendingAdapterIndex != g_ActiveGraphicsAdapterIndex)) {
    g_CursorVisibilityToken--;
    UiFrame_ProcessAndPresentWithLockTransition();
    /* The original passes the dialog's width and height (ECX, EDX) to the hook; the decompile
       lost them to uninitialized locals. */
    pendingModeWidth = pendingWidthOrCurrentHeight;
    pendingModeHeight = pendingHeightOrCurrentWidth;
    currentAdapterIndex = g_ActiveGraphicsAdapterIndex;
    pendingWidthOrCurrentHeight = g_FramebufferHeight;
    pendingHeightOrCurrentWidth = g_FramebufferWidth;
    pendingModeResult = g_GraphicsSetDisplayMode(pendingAdapterIndex,pendingBitsPerPixel,pendingModeHeight,pendingModeWidth);
    if (pendingModeResult.failed) {
      restoreModeResult = g_GraphicsSetDisplayMode(currentAdapterIndex,currentBitsPerPixel,pendingWidthOrCurrentHeight,pendingHeightOrCurrentWidth);
      FatalError_ExitIfFailed(restoreModeResult.valueOrError,restoreModeResult.failed);
      g_CursorVisibilityToken++;
      FatalError_ReportIfFailed(pendingModeResult.valueOrError,true);
      return;
    }
    UiRootStack_Relayout();
    g_CursorVisibilityToken++;
    UiRuntime_OpenFourValueDialog(currentAdapterIndex,currentBitsPerPixel,pendingWidthOrCurrentHeight,pendingHeightOrCurrentWidth);
  }
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

  displaySettingsRoot = UiNode_GetRoot(sourceNode);
  colorBiasQ16 = ((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(displaySettingsRoot,applyButton))->originalColorBiasQ16;
  colorScaleQ16 = ((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(displaySettingsRoot,applyButton))->originalColorScaleQ16;
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

  stepTicksField = &((UiFourValueDialogCountdownText *)FOUR_VALUE_DIALOG_UI(root,countdownMessageText))->stepTicks;
  *stepTicksField = *stepTicksField - 1;
  if (*stepTicksField == 0) {
    ((UiFourValueDialogCountdownText *)FOUR_VALUE_DIALOG_UI(root,countdownMessageText))->stepTicks =
         UI_DISPLAY_MODE_COUNTDOWN_STEP_TICKS;
    countdownField = &((UiFourValueDialogCountdownText *)FOUR_VALUE_DIALOG_UI(root,countdownMessageText))->countdown;
    *countdownField = *countdownField - 1;
    if (*countdownField == 0) {
      UiActionQueue_Enqueue(UI_DISPLAY_MODE_ACTION_REVERT,root);
    }
    else {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,
                 ((UiFourValueDialogCountdownText *)FOUR_VALUE_DIALOG_UI(root,countdownMessageText))->countdown,
                 ((UiFourValueDialogCountdownText *)FOUR_VALUE_DIALOG_UI(root,countdownMessageText))->countdownTextUtf16);
    }
  }
  return;
}


/* Address: 0x004B3F40.
   drawClipped of the range slider (g_UiRangeSliderControlVtable): draws the track from three
   g_UiWindowTextureSource pieces (start cap, tiled middle, end cap) and the thumb at the position of value
   within minimumValue..maximumValue, rounded to the nearest pixel. Horizontal or vertical after
   UI_RANGE_SLIDER_VERTICAL; a suppressed slider uses the greyed pieces.
*/
void UiRangeSliderControl_DrawTrackAndThumb
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiRangeSliderControl *control)

{
  int32_t rangeMax;
  uint64_t scaledOffset;
  uint32_t rangeOrValueOffset;
  int32_t clampedValue;
  uint32_t subresourceBase;
  int edgeLength;
  uint32_t valueOffsetOrRange;
  bool accessFailed;
  TextureSizeResult textureSize;

  accessFailed = g_GraphicsFramebufferBeginAccess();
  if (!accessFailed) {
    subresourceBase = UI_RANGE_SLIDER_SUBRESOURCE_BASE;
    if ((control->base.nodeFlags & UI_NODE_SUPPRESSED) != 0) {
      subresourceBase = UI_RANGE_SLIDER_SUBRESOURCE_BASE_SUPPRESSED;
    }
    if ((control->sliderFlags & UI_RANGE_SLIDER_VERTICAL) != 0) {
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,control->base.top,control->base.left,
                 subresourceBase + UI_RANGE_SLIDER_SUBRESOURCE_VERTICAL_OFFSET,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      textureSize = g_GraphicsTextureSourceGetLogicalSize(subresourceBase + UI_RANGE_SLIDER_SUBRESOURCE_VERTICAL_OFFSET,
                                                          g_UiWindowTextureSource);
      edgeLength = control->base.layoutHeight - textureSize.logicalHeightPixels;
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,subresourceBase + UI_RANGE_SLIDER_SUBRESOURCE_VERTICAL_OFFSET + 1,
                 edgeLength,textureSize.logicalHeightPixels,0,&control->base);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,edgeLength + control->base.top,control->base.left,
                 subresourceBase + UI_RANGE_SLIDER_SUBRESOURCE_VERTICAL_OFFSET + 2,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      /* the thumb */
      textureSize = g_GraphicsTextureSourceGetLogicalSize(subresourceBase + UI_RANGE_SLIDER_SUBRESOURCE_VERTICAL_OFFSET + 3,
                                                          g_UiWindowTextureSource);
      rangeMax = control->maximumValue;
      clampedValue = control->value;
      if (rangeMax < control->value) {
        clampedValue = rangeMax;
      }
      rangeOrValueOffset = rangeMax - control->minimumValue;
      if (rangeOrValueOffset == 0) {
        rangeOrValueOffset = 1;
      }
      valueOffsetOrRange = clampedValue - control->minimumValue;
      if ((int)valueOffsetOrRange < 0) {
        valueOffsetOrRange = 0;
      }
      /* vertical sliders have their maximum at the top unless reversed */
      if ((control->sliderFlags & UI_RANGE_SLIDER_REVERSED) == 0) {
        valueOffsetOrRange = rangeOrValueOffset - valueOffsetOrRange;
      }
      /* thumb position = offset * free track length / range, rounded */
      scaledOffset = (uint64_t)valueOffsetOrRange * (uint64_t)(control->base.layoutHeight - textureSize.logicalHeightPixels);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,
                 (int)(scaledOffset / rangeOrValueOffset) + (uint32_t)(rangeOrValueOffset < (uint32_t)((int)(scaledOffset % (uint64_t)rangeOrValueOffset) * 2))
                 + control->base.top,control->base.left,subresourceBase + UI_RANGE_SLIDER_SUBRESOURCE_VERTICAL_OFFSET + 3,
                 g_UiWindowTextureSource,g_FramebufferAccess
                );
      g_GraphicsFramebufferEndAccess();
      return;
    }
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipTop,clipLeft,clipBottom,clipRight,control->base.top,control->base.left,subresourceBase,
               g_UiWindowTextureSource,g_FramebufferAccess);
    textureSize = g_GraphicsTextureSourceGetLogicalSize(subresourceBase,g_UiWindowTextureSource);
    edgeLength = control->base.layoutWidth - textureSize.logicalWidthPixels;
    UiWindow_BlitTiledHorizontalEdge
              (clipTop,clipLeft,clipBottom,clipRight,subresourceBase + 1,edgeLength,0,textureSize.logicalWidthPixels,
               &control->base);
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipTop,clipLeft,clipBottom,clipRight,control->base.top,edgeLength + control->base.left,subresourceBase + 2,
               g_UiWindowTextureSource,g_FramebufferAccess);
    /* the thumb */
    textureSize = g_GraphicsTextureSourceGetLogicalSize(subresourceBase + 3,g_UiWindowTextureSource);
    rangeMax = control->maximumValue;
    clampedValue = control->value;
    if (rangeMax < control->value) {
      clampedValue = rangeMax;
    }
    rangeOrValueOffset = clampedValue - control->minimumValue;
    if ((int)rangeOrValueOffset < 0) {
      rangeOrValueOffset = 0;
    }
    valueOffsetOrRange = rangeMax - control->minimumValue;
    if (valueOffsetOrRange == 0) {
      valueOffsetOrRange = 1;
    }
    if ((control->sliderFlags & UI_RANGE_SLIDER_REVERSED) != 0) {
      rangeOrValueOffset = valueOffsetOrRange - rangeOrValueOffset;
    }
    /* thumb position = offset * free track length / range, rounded */
    scaledOffset = (uint64_t)rangeOrValueOffset * (uint64_t)(control->base.layoutWidth - textureSize.logicalWidthPixels);
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipTop,clipLeft,clipBottom,clipRight,control->base.top,
               (int)(scaledOffset / valueOffsetOrRange) + (uint32_t)(valueOffsetOrRange < (uint32_t)((int)(scaledOffset % (uint64_t)valueOffsetOrRange) * 2)) +
               control->base.left,subresourceBase + 3,g_UiWindowTextureSource,g_FramebufferAccess);
    g_GraphicsFramebufferEndAccess();
  }
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
  TextureSizeResult thumbSize;

  if (((((control->base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
       (localX = pointerX - control->base.left, control->base.left <= pointerX)) &&
      (localY = pointerY - control->base.top, control->base.top <= pointerY)) &&
     ((localX < control->base.layoutWidth && (localY < control->base.layoutHeight)))) {
    if ((control->sliderFlags & UI_RANGE_SLIDER_VERTICAL) == 0) {
      thumbSize = g_GraphicsTextureSourceGetLogicalSize(UI_RANGE_SLIDER_SUBRESOURCE_HORIZONTAL_THUMB,g_UiWindowTextureSource);
      if ((int)thumbSize.logicalHeightPixels <= localY) {
        return;
      }
      control->sliderFlags = control->sliderFlags | UI_RANGE_SLIDER_DRAGGING;
    }
    else {
      thumbSize = g_GraphicsTextureSourceGetLogicalSize(UI_RANGE_SLIDER_SUBRESOURCE_VERTICAL_THUMB,g_UiWindowTextureSource);
      if ((int)thumbSize.logicalWidthPixels <= localX) {
        return;
      }
      control->sliderFlags = control->sliderFlags | UI_RANGE_SLIDER_DRAGGING;
    }
    if (((control->sliderFlags & UI_RANGE_SLIDER_CLICK_SOUND) != 0) && (control->clickSound != NULL)) {
      g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->clickSound);
    }
  }
  return;
}


/* Address: 0x004B4280.
   nonRightRelease of the range slider (g_UiRangeSliderControlVtable): ends a thumb drag and plays the click
   sound when UI_RANGE_SLIDER_CLICK_SOUND is set and the slider is not suppressed.
*/
void UiRangeSliderControl_EndThumbDrag
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiRangeSliderControl *control)

{
  control->sliderFlags = control->sliderFlags & ~UI_RANGE_SLIDER_DRAGGING;
  if ((((control->base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
       ((control->sliderFlags & UI_RANGE_SLIDER_CLICK_SOUND) != 0)) && (control->clickSound != NULL)) {
    g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->clickSound);
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
void UiImageControl_DrawClipped(UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiImageControl *control)

{
  bool accessFailed;
  GraphicsSubresourceIndex subresource;

  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (((control->selectable).stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) != 0) {
      UiContainer_DrawIntersectingChildren
                (clipTop,clipLeft,clipBottom,clipRight,(UiNodeBase *)control);
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
                  (clipTop,clipLeft,clipBottom,clipRight,(control->selectable).base.top,
                   (control->selectable).base.left,subresource,control->textureSource,g_FramebufferAccess);
        g_GraphicsFramebufferEndAccess();
      }
    }
  }
  return;
}


/* Address: 0x004BC6E0.
   nonRightPress of the image control (g_UiNodeVtable_004BC570): plays the pointer sound (state bit 0x20,
   unless bit 0x400 is set), drops the active child and the hover target, then toggles: a press on an
   opaque pixel of an already selected image clears the press state bits (0xA03), any other press sets them.
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
  if (((((control->selectable).stateFlags & 0x400) == 0) &&
      (((control->selectable).stateFlags & 0x20) != 0)) && (control->pointerActivationSoundId != 0))
  {
    g_SoundPlayOneShot
              (g_UiSoundGainQ15,g_UiSoundGainQ15,
               (DirectSoundVoiceSet *)control->pointerActivationSoundId);
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
    *stateFlagsField = *stateFlagsField & 0xfffff5fc;
  }
  else {
    pressStateFlagsField = &(control->selectable).stateFlags;
    *pressStateFlagsField = *pressStateFlagsField | 0xa03;
  }
  UiNode_InvalidateRoot((UiNodeBase *)control);
  return;
}


/* Address: 0x004BC7E0.
   nonRightRelease of the image control (g_UiNodeVtable_004BC570). A release while state bit 0x200 is set
   (still pressed on the image) keeps it open: bit 0x400, and the image becomes g_UiImageControlHoverTarget.
   Otherwise an active child gets the release first, and the image stays open only if it had one and bit
   0x400 was not yet set or bit 0x800 is set; else it closes: hover target cleared,
   UI_IMAGE_CONTROL_HOVER_STATE_BITS cleared and the pointer sound played (bit 0x20).
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
    preserveHover = ((control->selectable).stateFlags & 0x200) != 0;
    if (!preserveHover) {
      if (previousActiveChild != NULL) {
        activeChildVtable = previousActiveChild->vtable;
        control->activeChild = NULL;
        activeChildVtable->nonRightRelease(wheelDelta,pointerY,pointerX,previousActiveChild);
        preserveHover = (((control->selectable).stateFlags & 0x400) == 0) ||
                        (((control->selectable).stateFlags & 0x800) != 0);
      }
      if (!preserveHover) {
        g_UiImageControlHoverTarget = NULL;
        stateFlagsField = &(control->selectable).stateFlags;
        *stateFlagsField = *stateFlagsField & ~UI_IMAGE_CONTROL_HOVER_STATE_BITS;
        if ((((control->selectable).stateFlags & 0x20) != 0) &&
           (control->pointerActivationSoundId != 0)) {
          g_SoundPlayOneShot
                    (g_UiSoundGainQ15,g_UiSoundGainQ15,
                     (DirectSoundVoiceSet *)control->pointerActivationSoundId);
        }
      }
    }
    if (preserveHover) {
      hoverStateFlagsField = &(control->selectable).stateFlags;
      *hoverStateFlagsField = *hoverStateFlagsField | 0x400;
      g_UiImageControlHoverTarget = control;
      hoverStateFlagsField = &(control->selectable).stateFlags;
      *hoverStateFlagsField = *hoverStateFlagsField & 0xfffffdff;
    }
  }
  UiNode_InvalidateRoot((UiNodeBase *)control);
  return;
}


/* Address: 0x004BD2A0.
   Re-tints a world model (army, effect or shot) after its runtime state bits changed: the tint chosen by
   UiNode_GetStateTintArgb from runtimeFlags 0x04/0x08/0x10 is applied to the whole hierarchy only when it
   differs from the tint the model already has.
*/
void ModelNodeRuntime_RefreshStateTint(ModelRuntimeNode *modelNode)

{
  PackedArgb32 tintArgb;
  
  tintArgb = UiNode_GetStateTintArgb((UiNodeBase *)modelNode);
  if (tintArgb != modelNode->tintArgb) {
    ModelNodeRuntime_ApplyTintRecursive(tintArgb,modelNode);
  }
  return;
}


/* Address: 0x00517E30.
   drawClipped of the transfer progress gauge (g_UiNodeVtable_00517DE0) shown while the player snapshots
   are exchanged at session start: on the host (or in a local game) the range is the outgoing byte count
   and the value the smallest progress any client has reported (runtimeState70 of player blocks 1..n); on a
   client it is the received byte count and the bytes received so far. Draws nothing unless a transfer is
   running and not yet complete.
*/
void UiHorizontalGaugeControl_UpdateRuntimeRangeAndDraw
          (int clipTop,int clipLeft,int clipBottom,int clipRight,UiHorizontalGaugeControl *control)

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
      if ((int)playerRecord[1].runtimeState70 < (int)minimumProgress) {
        minimumProgress = playerRecord[1].runtimeState70;
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
  UiHorizontalGaugeControl_DrawFrameFillAndLabel(clipTop,clipLeft,clipBottom,clipRight,control);
  return;
}


/* Address: 0x00423600.
   Opens the display settings dialog (only when more than one display mode was enumerated): copies
   g_UiDisplaySettingsRootTemplate to the heap, records the current mode and colour bias/scale as both the
   selected and the original values, installs its action handlers and pushes it. The option buttons are then
   labelled with the enumerated values in ascending order: up to 4 distinct bit depths, 8 resolutions and 5
   adapters (a sorted insert into the g_UiDisplayModeDistinctValueScratch slots, 0xFFFFFFFF = empty).
   Reopened by UiDisplayModeAction_ApplyFourValueDialogAndReopenSettings. The original sets CF when the
   allocation fails; that caller ignores it.
*/
void UiDisplaySettings_OpenAndPopulateModeSelection(void)

{
  DisplayModeScratchWord adapterOption1Or4;
  DisplayModeScratchWord adapterOption2;
  uint32_t framebufferWidth;
  uint32_t framebufferHeight;
  UiRootFlags activeAdapterIndex;
  int32_t colorScaleQ16;
  int32_t colorBiasQ16;
  UiRootNode *root;
  uint32_t insertValueA;
  uint32_t insertValueB;
  int copyCountOrRgBits;
  UiNodeFlags colorDepthBits;
  GraphicsDisplayModeCount remainingModes;
  uint32_t *copyCursor;
  uint32_t lowWordValue;
  uint32_t *templateCursor;
  GraphicsDisplayMode *displayMode;
  ArenaAllocResult allocResult;
  
  if (1 < g_GraphicsDisplayModeCount) {
    allocResult = g_MemoryApi.alloc(0xbd4); /* the dialog template's size */
    root = (UiRootNode *)allocResult.payloadOrError;
    if (allocResult.failed) {
      return;
    }
    /* REP MOVSD of the template, 0xBD4 / 4 dwords */
    templateCursor = (uint32_t *)THANDOR_ADDR(g_UiDisplaySettingsRootTemplate,0);
    copyCursor = (uint32_t *)root;
    for (copyCountOrRgBits = 0x2f5; activeAdapterIndex = g_ActiveGraphicsAdapterIndex, framebufferHeight = g_FramebufferHeight,
        framebufferWidth = g_FramebufferWidth, copyCountOrRgBits != 0; copyCountOrRgBits--) {
      *copyCursor = *templateCursor;
      templateCursor = templateCursor + 1;
      copyCursor = copyCursor + 1;
    }
    colorDepthBits = g_SoftwarePixelFormatConfig.redBitCount + g_SoftwarePixelFormatConfig.greenBitCount +
             g_SoftwarePixelFormatConfig.blueBitCount;
    /* applyButton tail: the selected mode tuple and the original one start out equal */
    ((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->selectedWidth = g_FramebufferWidth;
    ((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->selectedHeight = framebufferHeight;
    ((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->selectedBitsPerPixel = colorDepthBits;
    ((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->selectedAdapterIndex = activeAdapterIndex;
    ((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->originalWidth = framebufferWidth;
    ((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->originalHeight = framebufferHeight;
    ((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->originalBitsPerPixel = colorDepthBits;
    ((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->originalAdapterIndex = activeAdapterIndex;
    colorBiasQ16 = g_SoftwareColorBiasQ16;
    colorScaleQ16 = g_SoftwareColorScaleQ16;
    ((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->selectedColorBiasQ16 = g_SoftwareColorBiasQ16;
    ((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->selectedColorScaleQ16 = colorScaleQ16;
    ((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->originalColorBiasQ16 = colorBiasQ16;
    ((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->originalColorScaleQ16 = colorScaleQ16;
    ((UiRangeSliderControl *)DISPLAY_SETTINGS_UI(root,colorBiasSlider))->value = colorBiasQ16;
    ((UiRangeSliderControl *)DISPLAY_SETTINGS_UI(root,colorScaleSlider))->value = colorScaleQ16;
    /* The two readouts show the number buffers kept in the tail of colorBiasValueText. */
    ((UiSingleLineTextControl *)DISPLAY_SETTINGS_UI(root,colorBiasValueText))->text =
         ((UiDisplaySettingsValueReadout *)DISPLAY_SETTINGS_UI(root,colorBiasValueText))->colorBiasTextUtf16;
    ((UiSingleLineTextControl *)DISPLAY_SETTINGS_UI(root,colorScaleValueText))->text =
         ((UiDisplaySettingsValueReadout *)DISPLAY_SETTINGS_UI(root,colorBiasValueText))->colorScaleTextUtf16;
    UiRuntime_FormatSignedValues140And144(root);
    /* handler page 2 serves the action ids 0x200.. */
    UiActionHandlers_SetPage(2,(UiActionHandlerPage *)&g_UiDisplayModeSelectionActionHandlers20);
    UiRootStack_Push(&g_UiDisplaySettingsRootCallbacks,root);
    /* distinct bit depths, ascending (the XCHG swaps of the original appear as LOCK/UNLOCK) */
    g_UiDisplayModeDistinctValueScratch0 = 0xffffffff;
    g_UiDisplayModeDistinctValueScratch1 = 0xffffffff;
    g_UiDisplayModeDistinctValueScratch2 = 0xffffffff;
    g_UiDisplayModeDistinctValueScratch3 = 0xffffffff;
    remainingModes = g_GraphicsDisplayModeCount;
    displayMode = g_GraphicsDisplayModes;
    do {
      insertValueA = displayMode->bitsPerPixel;
      if ((((insertValueA != g_UiDisplayModeDistinctValueScratch0) &&
           (insertValueA != g_UiDisplayModeDistinctValueScratch1)) &&
          (insertValueA != g_UiDisplayModeDistinctValueScratch2)) &&
         (insertValueA != g_UiDisplayModeDistinctValueScratch3)) {
        insertValueB = insertValueA;
        if (insertValueA < g_UiDisplayModeDistinctValueScratch0) {
          LOCK();
          UNLOCK();
          insertValueB = g_UiDisplayModeDistinctValueScratch0;
          g_UiDisplayModeDistinctValueScratch0 = insertValueA;
        }
        insertValueA = insertValueB;
        if (insertValueB < g_UiDisplayModeDistinctValueScratch1) {
          LOCK();
          UNLOCK();
          insertValueA = g_UiDisplayModeDistinctValueScratch1;
          g_UiDisplayModeDistinctValueScratch1 = insertValueB;
        }
        insertValueB = insertValueA;
        if (insertValueA < g_UiDisplayModeDistinctValueScratch2) {
          LOCK();
          UNLOCK();
          insertValueB = g_UiDisplayModeDistinctValueScratch2;
          g_UiDisplayModeDistinctValueScratch2 = insertValueA;
        }
        if (insertValueB < g_UiDisplayModeDistinctValueScratch3) {
          LOCK();
          UNLOCK();
          g_UiDisplayModeDistinctValueScratch3 = insertValueB;
        }
      }
      displayMode = displayMode + 1;
      remainingModes = remainingModes - 1;
    } while (remainingModes != 0);
    /* Each option button's mode value(s) sit in the dwords just before the button (read back by the
       action callbacks as UiDisplayModeOptionPrefix.modeValue / .resolutionHeight). */
    ((UiDisplayModeOptionPrefix *)DISPLAY_SETTINGS_UI(root,colorDepthOption1))[-1].modeValue = g_UiDisplayModeDistinctValueScratch0;
    ((UiDisplayModeOptionPrefix *)DISPLAY_SETTINGS_UI(root,colorDepthOption2))[-1].modeValue = g_UiDisplayModeDistinctValueScratch1;
    ((UiDisplayModeOptionPrefix *)DISPLAY_SETTINGS_UI(root,colorDepthOption3))[-1].modeValue = g_UiDisplayModeDistinctValueScratch2;
    ((UiDisplayModeOptionPrefix *)DISPLAY_SETTINGS_UI(root,colorDepthOption4))[-1].modeValue = g_UiDisplayModeDistinctValueScratch3;
    g_UiDisplayModeDistinctValueScratch0 = 0xffffffff;
    g_UiDisplayModeDistinctValueScratch1 = 0xffffffff;
    g_UiDisplayModeDistinctValueScratch2 = 0xffffffff;
    g_UiDisplayModeDistinctValueScratch3 = 0xffffffff;
    g_UiDisplayModeDistinctValueScratch4 = 0xffffffff;
    g_UiDisplayModeDistinctValueScratch5 = 0xffffffff;
    g_UiDisplayModeDistinctValueScratch6 = 0xffffffff;
    g_UiDisplayModeDistinctValueScratch7 = 0xffffffff;
    remainingModes = g_GraphicsDisplayModeCount;
    displayMode = g_GraphicsDisplayModes;
    /* distinct resolutions, keyed width << 16 | height so that they sort by width, then height */
    do {
      insertValueA = displayMode->width * 0x10000 + displayMode->height;
      if ((((insertValueA != g_UiDisplayModeDistinctValueScratch0) &&
           (insertValueA != g_UiDisplayModeDistinctValueScratch1)) &&
          ((insertValueA != g_UiDisplayModeDistinctValueScratch2 &&
           ((insertValueA != g_UiDisplayModeDistinctValueScratch3 &&
            (insertValueA != g_UiDisplayModeDistinctValueScratch4)))))) &&
         ((insertValueA != g_UiDisplayModeDistinctValueScratch5 &&
          ((insertValueA != g_UiDisplayModeDistinctValueScratch6 &&
           (insertValueA != g_UiDisplayModeDistinctValueScratch7)))))) {
        insertValueB = insertValueA;
        if (insertValueA < g_UiDisplayModeDistinctValueScratch0) {
          LOCK();
          UNLOCK();
          insertValueB = g_UiDisplayModeDistinctValueScratch0;
          g_UiDisplayModeDistinctValueScratch0 = insertValueA;
        }
        insertValueA = insertValueB;
        if (insertValueB < g_UiDisplayModeDistinctValueScratch1) {
          LOCK();
          UNLOCK();
          insertValueA = g_UiDisplayModeDistinctValueScratch1;
          g_UiDisplayModeDistinctValueScratch1 = insertValueB;
        }
        insertValueB = insertValueA;
        if (insertValueA < g_UiDisplayModeDistinctValueScratch2) {
          LOCK();
          UNLOCK();
          insertValueB = g_UiDisplayModeDistinctValueScratch2;
          g_UiDisplayModeDistinctValueScratch2 = insertValueA;
        }
        insertValueA = insertValueB;
        if (insertValueB < g_UiDisplayModeDistinctValueScratch3) {
          LOCK();
          UNLOCK();
          insertValueA = g_UiDisplayModeDistinctValueScratch3;
          g_UiDisplayModeDistinctValueScratch3 = insertValueB;
        }
        insertValueB = insertValueA;
        if (insertValueA < g_UiDisplayModeDistinctValueScratch4) {
          LOCK();
          UNLOCK();
          insertValueB = g_UiDisplayModeDistinctValueScratch4;
          g_UiDisplayModeDistinctValueScratch4 = insertValueA;
        }
        insertValueA = insertValueB;
        if (insertValueB < g_UiDisplayModeDistinctValueScratch5) {
          LOCK();
          UNLOCK();
          insertValueA = g_UiDisplayModeDistinctValueScratch5;
          g_UiDisplayModeDistinctValueScratch5 = insertValueB;
        }
        insertValueB = insertValueA;
        if (insertValueA < g_UiDisplayModeDistinctValueScratch6) {
          LOCK();
          UNLOCK();
          insertValueB = g_UiDisplayModeDistinctValueScratch6;
          g_UiDisplayModeDistinctValueScratch6 = insertValueA;
        }
        if (insertValueB < g_UiDisplayModeDistinctValueScratch7) {
          LOCK();
          UNLOCK();
          g_UiDisplayModeDistinctValueScratch7 = insertValueB;
        }
      }
      displayMode = displayMode + 1;
      remainingModes = remainingModes - 1;
    } while (remainingModes != 0);
    /* Resolution buttons: width at -8, height at -0xC. */
    ((UiDisplayModeOptionPrefix *)DISPLAY_SETTINGS_UI(root,resolutionOption1))[-1].modeValue = g_UiDisplayModeDistinctValueScratch0 >> 16;
    ((UiDisplayModeOptionPrefix *)DISPLAY_SETTINGS_UI(root,resolutionOption1))[-1].resolutionHeight = g_UiDisplayModeDistinctValueScratch0 & 0xffff;
    ((UiDisplayModeOptionPrefix *)DISPLAY_SETTINGS_UI(root,resolutionOption2))[-1].modeValue = g_UiDisplayModeDistinctValueScratch1 >> 16;
    ((UiDisplayModeOptionPrefix *)DISPLAY_SETTINGS_UI(root,resolutionOption2))[-1].resolutionHeight = g_UiDisplayModeDistinctValueScratch1 & 0xffff;
    ((UiDisplayModeOptionPrefix *)DISPLAY_SETTINGS_UI(root,resolutionOption3))[-1].modeValue = g_UiDisplayModeDistinctValueScratch2 >> 16;
    ((UiDisplayModeOptionPrefix *)DISPLAY_SETTINGS_UI(root,resolutionOption3))[-1].resolutionHeight = g_UiDisplayModeDistinctValueScratch2 & 0xffff;
    ((UiDisplayModeOptionPrefix *)DISPLAY_SETTINGS_UI(root,resolutionOption4))[-1].modeValue = g_UiDisplayModeDistinctValueScratch3 >> 16;
    ((UiDisplayModeOptionPrefix *)DISPLAY_SETTINGS_UI(root,resolutionOption4))[-1].resolutionHeight = g_UiDisplayModeDistinctValueScratch3 & 0xffff;
    ((UiDisplayModeOptionPrefix *)DISPLAY_SETTINGS_UI(root,resolutionOption5))[-1].modeValue = g_UiDisplayModeDistinctValueScratch4 >> 16;
    ((UiDisplayModeOptionPrefix *)DISPLAY_SETTINGS_UI(root,resolutionOption5))[-1].resolutionHeight = g_UiDisplayModeDistinctValueScratch4 & 0xffff;
    insertValueA = g_UiDisplayModeDistinctValueScratch5 & 0xffff;
    ((UiDisplayModeOptionPrefix *)DISPLAY_SETTINGS_UI(root,resolutionOption6))[-1].modeValue = g_UiDisplayModeDistinctValueScratch5 >> 16;
    ((UiDisplayModeOptionPrefix *)DISPLAY_SETTINGS_UI(root,resolutionOption6))[-1].resolutionHeight = insertValueA;
    lowWordValue = g_UiDisplayModeDistinctValueScratch6 & 0xffff;
    ((UiDisplayModeOptionPrefix *)DISPLAY_SETTINGS_UI(root,resolutionOption7))[-1].modeValue = g_UiDisplayModeDistinctValueScratch6 >> 16;
    ((UiDisplayModeOptionPrefix *)DISPLAY_SETTINGS_UI(root,resolutionOption7))[-1].resolutionHeight = lowWordValue;
    lowWordValue = g_UiDisplayModeDistinctValueScratch7 & 0xffff;
    ((UiDisplayModeOptionPrefix *)DISPLAY_SETTINGS_UI(root,resolutionOption8))[-1].modeValue = g_UiDisplayModeDistinctValueScratch7 >> 16;
    ((UiDisplayModeOptionPrefix *)DISPLAY_SETTINGS_UI(root,resolutionOption8))[-1].resolutionHeight = lowWordValue;
    g_UiDisplayModeDistinctValueScratch0 = 0xffffffff;
    g_UiDisplayModeDistinctValueScratch1 = 0xffffffff;
    g_UiDisplayModeDistinctValueScratch2 = 0xffffffff;
    g_UiDisplayModeDistinctValueScratch3 = 0xffffffff;
    g_UiDisplayModeDistinctValueScratch4 = 0xffffffff;
    remainingModes = g_GraphicsDisplayModeCount;
    displayMode = g_GraphicsDisplayModes;
    /* distinct adapters */
    do {
      insertValueA = displayMode->adapterIndex;
      if ((((insertValueA != g_UiDisplayModeDistinctValueScratch0) &&
           (insertValueA != g_UiDisplayModeDistinctValueScratch1)) &&
          (insertValueA != g_UiDisplayModeDistinctValueScratch2)) &&
         ((insertValueA != g_UiDisplayModeDistinctValueScratch3 &&
          (insertValueA != g_UiDisplayModeDistinctValueScratch4)))) {
        insertValueB = insertValueA;
        if (insertValueA < g_UiDisplayModeDistinctValueScratch0) {
          LOCK();
          UNLOCK();
          insertValueB = g_UiDisplayModeDistinctValueScratch0;
          g_UiDisplayModeDistinctValueScratch0 = insertValueA;
        }
        insertValueA = insertValueB;
        if (insertValueB < g_UiDisplayModeDistinctValueScratch1) {
          LOCK();
          UNLOCK();
          insertValueA = g_UiDisplayModeDistinctValueScratch1;
          g_UiDisplayModeDistinctValueScratch1 = insertValueB;
        }
        insertValueB = insertValueA;
        if (insertValueA < g_UiDisplayModeDistinctValueScratch2) {
          LOCK();
          UNLOCK();
          insertValueB = g_UiDisplayModeDistinctValueScratch2;
          g_UiDisplayModeDistinctValueScratch2 = insertValueA;
        }
        insertValueA = insertValueB;
        if (insertValueB < g_UiDisplayModeDistinctValueScratch3) {
          LOCK();
          UNLOCK();
          insertValueA = g_UiDisplayModeDistinctValueScratch3;
          g_UiDisplayModeDistinctValueScratch3 = insertValueB;
        }
        if (insertValueA < g_UiDisplayModeDistinctValueScratch4) {
          LOCK();
          UNLOCK();
          g_UiDisplayModeDistinctValueScratch4 = insertValueA;
        }
      }
      adapterOption2 = g_UiDisplayModeDistinctValueScratch2;
      adapterOption1Or4 = g_UiDisplayModeDistinctValueScratch1;
      displayMode = displayMode + 1;
      remainingModes = remainingModes - 1;
    } while (remainingModes != 0);
    /* Adapter buttons: adapter index at -8. */
    ((UiDisplayModeOptionPrefix *)DISPLAY_SETTINGS_UI(root,adapterOption1))[-1].modeValue = g_UiDisplayModeDistinctValueScratch0;
    ((UiDisplayModeOptionPrefix *)DISPLAY_SETTINGS_UI(root,adapterOption2))[-1].modeValue = adapterOption1Or4;
    ((UiDisplayModeOptionPrefix *)DISPLAY_SETTINGS_UI(root,adapterOption3))[-1].modeValue = adapterOption2;
    adapterOption1Or4 = g_UiDisplayModeDistinctValueScratch4;
    copyCountOrRgBits = g_SoftwarePixelFormatConfig.redBitCount + g_SoftwarePixelFormatConfig.greenBitCount;
    ((UiDisplayModeOptionPrefix *)DISPLAY_SETTINGS_UI(root,adapterOption4))[-1].modeValue = g_UiDisplayModeDistinctValueScratch3;
    ((UiDisplayModeOptionPrefix *)DISPLAY_SETTINGS_UI(root,adapterOption5))[-1].modeValue = adapterOption1Or4;
    UiDisplayModeSelection_RefreshEnumeratedOptions
              (g_ActiveGraphicsAdapterIndex,
               (UiNodeBase *)(copyCountOrRgBits + g_SoftwarePixelFormatConfig.blueBitCount),g_FramebufferHeight
               ,g_FramebufferWidth,(UiNodeBase *)root);
    UiRootStack_InvalidateAll();
  }
  return;
}


/* Address: 0x004BC9B0.
   Hit test of an image control: only opaque pixels of its current image count, so irregular shapes react
   precisely. A miss clears state bit 0x200 and, in persistent activation mode, passes the test on to the
   children. Returns the hit node or UI_NODE_NONE.
*/
UiNodeBase * UiImageControl_HitTestOpaque(UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiImageControl *control)

{
  UiImageControl *hitNode;
  bool opaqueHit;
  UiSelectableStateFlags *stateFlagsField;
  
  hitNode = (UiImageControl *)UI_NODE_NONE;
  if ((control->selectable.base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    /* state bit 0x40 selects the alternate image as the hit mask */
    if ((control->selectable.stateFlags & 0x40) == 0) {
      opaqueHit = g_GraphicsTextureSourceTestOpaquePixel
                        (pointerY,pointerX,control->selectable.base.top,
                         control->selectable.base.left,control->normalSubresource,
                         control->textureSource);
      if (opaqueHit) {
        return (UiNodeBase *)control;
      }
    }
    else {
      opaqueHit = g_GraphicsTextureSourceTestOpaquePixel
                        (pointerY,pointerX,control->selectable.base.top,
                         control->selectable.base.left,control->alternateSubresource,
                         control->textureSource);
      if (opaqueHit) {
        return (UiNodeBase *)control;
      }
    }
    stateFlagsField = &control->selectable.stateFlags;
    *stateFlagsField = *stateFlagsField & 0xfffffdff;
    hitNode = (UiImageControl *)UI_NODE_NONE;
    /* UiContainer_HitTestChildren returns the control itself when no child is hit; that only counts
       while g_UiImageControlHoverTarget is NULL */
    if ((((control->selectable.stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) != 0) &&
        (hitNode = (UiImageControl *)
                   UiContainer_HitTestChildren(pointerY,pointerX,(UiNodeBase *)control),
        hitNode == control)) && (g_UiImageControlHoverTarget != NULL)) {
      hitNode = (UiImageControl *)UI_NODE_NONE;
    }
  }
  return (UiNodeBase *)hitNode;
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
  uint32_t bitsPerPixel = (uint32_t)(uintptr_t)selectedModeValue;
  void *selected = NULL;
  bool modeMissing; /* GraphicsDisplayMode_IsEnumerated returns true when the mode was not enumerated */
  int i;

  for (i = 0; i < 4; i++) {
    uint32_t depth = ((UiDisplayModeOptionPrefix *)(root + depthButtons[i]))[-1].modeValue;
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
    uint32_t height = ((UiDisplayModeOptionPrefix *)(root + sizeButtons[i]))[-1].resolutionHeight;
    uint32_t width = ((UiDisplayModeOptionPrefix *)(root + sizeButtons[i]))[-1].modeValue;
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
    uint32_t adapter = ((UiDisplayModeOptionPrefix *)(root + adapterButtons[i]))[-1].modeValue;
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
  ((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->selectedWidth = modeWidth;
  ((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->selectedHeight = modeHeight;
  ((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->selectedBitsPerPixel = bitsPerPixel;
  ((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->selectedAdapterIndex = adapterIndex;
  if ((modeWidth == (uint32_t)((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->originalWidth) &&
      (modeHeight == (uint32_t)((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->originalHeight) &&
      (bitsPerPixel == ((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->originalBitsPerPixel) &&
      (adapterIndex == ((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->originalAdapterIndex) &&
      (((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->selectedColorBiasQ16 ==
       ((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->originalColorBiasQ16) &&
      (((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->selectedColorScaleQ16 ==
       ((UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton))->originalColorScaleQ16)) {
    UiNodeList_SuppressActionId(UI_DISPLAY_MODE_ACTION_APPLY,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(UI_DISPLAY_MODE_ACTION_APPLY,displaySettingsRoot);
  }
  return;
}

