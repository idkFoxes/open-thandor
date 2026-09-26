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
   Ownership: ui/controls/misc.
   Purpose: Binary entry is anchored by g_CodePointerTable_004229A0[1]@004229A0. UiRootCallbacks root callback with
   one stack argument.
   Local calls: UiDisplayModeSelection_RefreshEnumeratedOptions.
   Cross-module calls: UiRuntime_FormatSignedValues140And144 [ui/core/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
UiDisplaySettingsRoot_RefreshModeSelection(UiRootNode *root)

{
  UiAnchorFractionQ31 colorBiasQ16;
  UiAnchorFractionQ31 colorScaleQ16;
  
  colorBiasQ16 = DISPLAY_SETTINGS_UI_FIELD(root,colorBiasSlider,0x58,sdword);
  colorScaleQ16 = DISPLAY_SETTINGS_UI_FIELD(root,colorScaleSlider,0x58,sdword);
  if ((colorBiasQ16 != DISPLAY_SETTINGS_UI_FIELD(root,applyButton,0x6C,dword)) || (colorScaleQ16 != DISPLAY_SETTINGS_UI_FIELD(root,applyButton,0x70,dword))) {
    DISPLAY_SETTINGS_UI_FIELD(root,applyButton,0x6C,dword) = colorBiasQ16;
    DISPLAY_SETTINGS_UI_FIELD(root,applyButton,0x70,dword) = colorScaleQ16;
    (*g_SoftwareBuildPixelPackTables)(colorScaleQ16,colorBiasQ16);
    UiDisplayModeSelection_RefreshEnumeratedOptions
              (DISPLAY_SETTINGS_UI_FIELD(root,applyButton,0x68,dword),(UiNodeBase *)DISPLAY_SETTINGS_UI_FIELD(root,applyButton,0x64,dword),
               DISPLAY_SETTINGS_UI_FIELD(root,applyButton,0x60,sdword),DISPLAY_SETTINGS_UI_FIELD(root,applyButton,0x5C,sdword),&root->base);
    UiRuntime_FormatSignedValues140And144(root);
  }
  return;
}


/* Address: 0x00423C40.
   Ownership: ui/controls/misc.
   Purpose: Binary entry is anchored by g_CodePointerTable_00423588[1]@00423588;
   g_CodePointerTable_00423588[2]@00423588; g_CodePointerTable_00423588[3]@00423588;
   g_CodePointerTable_00423588[4]@00423588. Display-mode selection action callback.
   Local calls: UiDisplayModeSelection_RefreshEnumeratedOptions.
   Cross-module calls: UiNode_GetRoot [ui/core/runtime].
*/
void __thandor_preserve_eax UiDisplayModeAction_UpdateAdapterSelection(UiNodeBase *sourceNode)

{
  UiNodeBase *displaySettingsRoot;
  
  displaySettingsRoot = UiNode_GetRoot(sourceNode);
  UiDisplayModeSelection_RefreshEnumeratedOptions
            ((FrontendDisplayAdapterIndex)displaySettingsRoot[4].vtable,
             (UiNodeBase *)sourceNode[-1].layoutHeight,
             (FrontendDisplayDimensionPixels)displaySettingsRoot[4].firstChild,
             (FrontendDisplayDimensionPixels)displaySettingsRoot[4].nextSibling,displaySettingsRoot)
  ;
  return;
}


/* Address: 0x00423C80.
   Ownership: ui/controls/misc.
   Purpose: Binary entry is anchored by g_CodePointerTable_00423588[5]@00423588;
   g_CodePointerTable_00423588[6]@00423588; g_CodePointerTable_00423588[7]@00423588;
   g_CodePointerTable_00423588[8]@00423588; g_CodePointerTable_00423588[9]@00423588;
   g_CodePointerTable_00423588[10]@00423588; g_CodePointerTable_00423588[11]@00423588;
   g_CodePointerTable_00423588[12]@00423588. Display-mode selection action callback.
   Local calls: UiDisplayModeSelection_RefreshEnumeratedOptions.
   Cross-module calls: UiNode_GetRoot [ui/core/runtime].
*/
void __thandor_preserve_eax UiDisplayModeAction_UpdateResolutionSelection(UiNodeBase *sourceNode)

{
  UiNodeBase *displaySettingsRoot;
  
  displaySettingsRoot = UiNode_GetRoot(sourceNode);
  UiDisplayModeSelection_RefreshEnumeratedOptions
            ((FrontendDisplayAdapterIndex)displaySettingsRoot[4].vtable,
             displaySettingsRoot[4].parent,sourceNode[-1].layoutWidth,sourceNode[-1].layoutHeight,
             displaySettingsRoot);
  return;
}


/* Address: 0x00423CB0.
   Ownership: ui/controls/misc.
   Purpose: Binary entry is anchored by g_CodePointerTable_00423588[15]@00423588;
   g_CodePointerTable_00423588[16]@00423588; g_CodePointerTable_00423588[17]@00423588;
   g_CodePointerTable_00423588[18]@00423588; g_CodePointerTable_00423588[19]@00423588. Display-mode selection
   action callback.
   Local calls: UiDisplayModeSelection_RefreshEnumeratedOptions.
   Cross-module calls: UiNode_GetRoot [ui/core/runtime].
*/
void __thandor_preserve_eax UiDisplayModeAction_UpdateColorDepthSelection(UiNodeBase *sourceNode)

{
  UiNodeBase *displaySettingsRoot;
  
  displaySettingsRoot = UiNode_GetRoot(sourceNode);
  UiDisplayModeSelection_RefreshEnumeratedOptions
            (sourceNode[-1].layoutHeight,displaySettingsRoot[4].parent,
             (FrontendDisplayDimensionPixels)displaySettingsRoot[4].firstChild,
             (FrontendDisplayDimensionPixels)displaySettingsRoot[4].nextSibling,displaySettingsRoot)
  ;
  return;
}


/* Address: 0x00424590.
   Ownership: ui/controls/misc.
   Purpose: Display-mode selection action callback.
   Local calls: UiDisplaySettings_OpenAndPopulateModeSelectionCf.
   Cross-module calls: UiNode_GetRoot [ui/core/runtime], UiRootStack_PopCf [ui/controls/layout],
   UiFrame_ProcessAndPresentWithLockTransition [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiDisplayModeAction_ApplyFourValueDialogAndReopenSettings(UiNodeBase *sourceNode)

{
  longlong scaledAnchor;
  UiRootNode *root;
  DisplayModeEaxCf5 modeResult;
  dword adapterIndex;
  dword bitsPerPixel;
  dword modeHeight;
  dword modeWidth;
  
  root = (UiRootNode *)UiNode_GetRoot(sourceNode);
  modeWidth = FOUR_VALUE_DIALOG_UI_FIELD(root,countdownMessageText,0x64,sdword);
  modeHeight = FOUR_VALUE_DIALOG_UI_FIELD(root,countdownMessageText,0x68,sdword);
  bitsPerPixel = FOUR_VALUE_DIALOG_UI_FIELD(root,countdownMessageText,0x6C,sdword);
  adapterIndex = FOUR_VALUE_DIALOG_UI_FIELD(root,countdownMessageText,0x70,sdword);
  UiRootStack_PopCf(root);
  g_CursorVisibilityToken = g_CursorVisibilityToken + -1;
  UiFrame_ProcessAndPresentWithLockTransition();
  modeResult = (*g_GraphicsDisplayModeHook)(adapterIndex,bitsPerPixel,modeHeight,modeWidth);
  (*g_FatalErrorPrimaryDispatchCf)(modeResult.eax,modeResult.carry);
  root = g_UiRootNode;
  do {
    scaledAnchor = (ulonglong)g_FramebufferWidth * (ulonglong)(root->base).rightAnchorQ31;
    (root->base).right =
         ((int)((ulonglong)scaledAnchor >> 0x20) << 1 | (uint)scaledAnchor >> 0x1f) + (root->base).rightOffset;
    scaledAnchor = (ulonglong)g_FramebufferHeight * (ulonglong)(root->base).bottomAnchorQ31;
    (root->base).bottom =
         ((int)((ulonglong)scaledAnchor >> 0x20) << 1 | (uint)scaledAnchor >> 0x1f) + (root->base).bottomOffset;
    scaledAnchor = (ulonglong)g_FramebufferWidth * (ulonglong)(root->base).leftAnchorQ31;
    (root->base).left =
         ((int)((ulonglong)scaledAnchor >> 0x20) << 1 | (uint)scaledAnchor >> 0x1f) + (root->base).leftOffset;
    scaledAnchor = (ulonglong)g_FramebufferHeight * (ulonglong)(root->base).topAnchorQ31;
    (root->base).top =
         ((int)((ulonglong)scaledAnchor >> 0x20) << 1 | (uint)scaledAnchor >> 0x1f) + (root->base).topOffset;
    (*((root->base).vtable)->layout)(&root->base);
    root = root->previousRoot;
  } while (root != (UiRootNode *)0xffffffff);
  g_CursorVisibilityToken = g_CursorVisibilityToken + 1;
  UiDisplaySettings_OpenAndPopulateModeSelectionCf();
  return;
}


/* Address: 0x004BC8B0.
   Ownership: ui/controls/misc.
   Purpose: Opaque-hit-tests the image and descendants during capture, transitions activeChild with synthetic
   release/press/drag events, and invalidates the root stack.
   Local calls: UiImageControl_HitTestOpaque.
   Cross-module calls: UiRootStack_InvalidateAll [ui/controls/layout].
*/
void __thandor_preserve_eax_edx
UiImageControl_NonRightDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
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
  if ((hitControl == control) ||
     (stateFlagsField = &(control->selectable).stateFlags,
     *stateFlagsField = *stateFlagsField & 0xfffffdff, hitControl == (UiImageControl *)0xffffffff))
  {
    previousActiveChild = control->activeChild;
  }
  else {
    if (hitControl == (UiImageControl *)control->activeChild) {
      (*((hitControl->selectable).base.vtable)->nonRightDrag)
                (wheelDelta,pointerY,pointerX,(UiNodeBase *)hitControl);
      goto UiImageControl_InvalidateAfterNonRightDrag;
    }
    hitVtable = (hitControl->selectable).base.vtable;
    /* The handlers preserve EAX/EDX: the original keeps passing the hit child and its vtable, and
       stores that child as the new activeChild (the decompiler lost both). */
    (*hitVtable->nonRightPress)(0,0x70000000,0x70000000,(UiNodeBase *)hitControl);
    (*hitVtable->nonRightDrag)(wheelDelta,pointerY,pointerX,(UiNodeBase *)hitControl);
    newActiveChild = (UiNodeBase *)hitControl;
    LOCK();
    previousActiveChild = control->activeChild;
    control->activeChild = newActiveChild;
    UNLOCK();
  }
  if (previousActiveChild != (UiNodeBase *)0x0) {
    (*previousActiveChild->vtable->nonRightDrag)(0,0x70000000,0x70000000,previousActiveChild);
    (*previousActiveChild->vtable->nonRightRelease)(0,0x70000000,0x70000000,previousActiveChild);
  }
UiImageControl_InvalidateAfterNonRightDrag:
  UiRootStack_InvalidateAll();
  return;
}


/* Address: 0x004BCB50.
   Ownership: ui/controls/misc.
   Purpose: While pointer polling is active, reevaluates opaque hover state and synthesizes child release, press,
   and drag transitions.
   Local calls: UiImageControl_HitTestOpaque.
*/
void __thandor_void_preserve_eax_ecx UiImageControl_TickHover(UiImageControl *control)

{
  UiSelectableStateFlags *clearStateFlagsField;
  UiNodeVtable *hitChildVtable;
  UiImageControl *hitControl;
  UiSelectableStateFlags *stateFlagsField;
  UiNodeVtable *hoveredControlVtable;
  UiSelectableStateFlags *hoverStateFlagsField;
  
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (((control->selectable).stateFlags & 0x100) == 0) {
      if ((g_CursorButtonState & 4) != 0) {
        hitControl = (UiImageControl *)
                     UiImageControl_HitTestOpaque(g_CursorOverrideY,g_CursorOverrideX,control);
        stateFlagsField = &(control->selectable).stateFlags;
        *stateFlagsField = *stateFlagsField | 0x100;
        if ((hitControl != control) && (hitControl != (UiImageControl *)0xffffffff)) {
          hitChildVtable = (hitControl->selectable).base.vtable;
          hoverStateFlagsField = &(control->selectable).stateFlags;
          *hoverStateFlagsField = *hoverStateFlagsField & 0xfffff7ff;
          (*hitChildVtable->nonRightRelease)
                    (g_CursorWheelDelta,g_CursorOverrideY,g_CursorOverrideX,(UiNodeBase *)hitControl
                    );
          /* EAX (the hovered child) and ECX (its vtable) survive the handler calls. */
          (*hitChildVtable->nonRightPress)
                    (g_CursorWheelDelta,g_CursorOverrideY,g_CursorOverrideX,(UiNodeBase *)hitControl);
          (*hitChildVtable->nonRightDrag)
                    (g_CursorWheelDelta,g_CursorOverrideY,g_CursorOverrideX,(UiNodeBase *)hitControl);
        }
      }
    }
    else if ((g_CursorButtonState & 4) == 0) {
      clearStateFlagsField = &(control->selectable).stateFlags;
      *clearStateFlagsField = *clearStateFlagsField & 0xfffffeff;
    }
  }
  return;
}


/* Address: 0x00423B30.
   Ownership: ui/controls/misc.
   Purpose: Binary entry is anchored by g_CodePointerTable_00423588[0]@00423588. Display-mode selection action
   callback.
   Cross-module calls: UiNode_GetRoot [ui/core/runtime], UiRootStack_PopCf [ui/controls/layout],
   UiFrame_ProcessAndPresentWithLockTransition [ui/controls/layout], UiRootStack_Relayout [ui/controls/layout],
   UiRuntime_OpenFourValueDialogCf [ui/core/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
UiDisplayModeAction_ApplyPendingMode(UiNodeBase *sourceNode)

{
  dword pendingBitsPerPixel;
  dword pendingAdapterIndex;
  UiRootNode *root;
  dword pendingModeWidth;
  dword pendingModeHeight;
  dword currentBitsPerPixel;
  DisplayModeEaxCf5 pendingModeResult;
  DisplayModeEaxCf5 restoreModeResult;
  uint currentAdapterIndex;
  dword pendingWidthOrCurrentHeight;
  dword pendingHeightOrCurrentWidth;
  
  root = (UiRootNode *)UiNode_GetRoot(sourceNode);
  pendingWidthOrCurrentHeight = DISPLAY_SETTINGS_UI_FIELD(root,applyButton,0x5C,sdword);
  pendingHeightOrCurrentWidth = DISPLAY_SETTINGS_UI_FIELD(root,applyButton,0x60,sdword);
  pendingBitsPerPixel = DISPLAY_SETTINGS_UI_FIELD(root,applyButton,0x64,dword);
  currentBitsPerPixel = g_SoftwarePixelFormatConfig.redBitCount + g_SoftwarePixelFormatConfig.greenBitCount +
            g_SoftwarePixelFormatConfig.blueBitCount;
  pendingAdapterIndex = DISPLAY_SETTINGS_UI_FIELD(root,applyButton,0x68,dword);
  UiRootStack_PopCf(root);
  if ((((pendingWidthOrCurrentHeight != g_FramebufferWidth) || (pendingHeightOrCurrentWidth != g_FramebufferHeight)) || (pendingBitsPerPixel != currentBitsPerPixel)) ||
     (pendingAdapterIndex != g_ActiveGraphicsAdapterIndex)) {
    g_CursorVisibilityToken = g_CursorVisibilityToken + -1;
    UiFrame_ProcessAndPresentWithLockTransition();
    /* The original passes the dialog's width and height (ECX, EDX) to the hook; the decompile
       lost them to uninitialized locals. */
    pendingModeWidth = pendingWidthOrCurrentHeight;
    pendingModeHeight = pendingHeightOrCurrentWidth;
    currentAdapterIndex = g_ActiveGraphicsAdapterIndex;
    pendingWidthOrCurrentHeight = g_FramebufferHeight;
    pendingHeightOrCurrentWidth = g_FramebufferWidth;
    pendingModeResult = (*g_GraphicsDisplayModeHook)(pendingAdapterIndex,pendingBitsPerPixel,pendingModeHeight,pendingModeWidth);
    if (pendingModeResult.carry) {
      restoreModeResult = (*g_GraphicsDisplayModeHook)(currentAdapterIndex,currentBitsPerPixel,pendingWidthOrCurrentHeight,pendingHeightOrCurrentWidth);
      (*g_FatalErrorPrimaryDispatchCf)(restoreModeResult.eax,restoreModeResult.carry);
      g_CursorVisibilityToken = g_CursorVisibilityToken + 1;
      (*g_FatalErrorRuntimeDispatchCf)(pendingModeResult.eax,true);
      return;
    }
    UiRootStack_Relayout();
    g_CursorVisibilityToken = g_CursorVisibilityToken + 1;
    UiRuntime_OpenFourValueDialogCf(currentAdapterIndex,currentBitsPerPixel,pendingWidthOrCurrentHeight,pendingHeightOrCurrentWidth);
  }
  return;
}


/* Address: 0x00423C00.
   Ownership: ui/controls/misc.
   Purpose: Binary entry is anchored by g_CodePointerTable_00423588[14]@00423588. Display-mode selection action
   callback.
   Cross-module calls: UiNode_GetRoot [ui/core/runtime], UiRootStack_PopCf [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiDisplayModeAction_CancelAndRebuildPixelPacking(UiNodeBase *sourceNode)

{
  sdword colorBiasQ16;
  sdword colorScaleQ16;
  UiNodeBase *displaySettingsRoot;
  
  displaySettingsRoot = UiNode_GetRoot(sourceNode);
  colorBiasQ16 = displaySettingsRoot[4].rightOffset;
  colorScaleQ16 = displaySettingsRoot[4].bottomOffset;
  UiRootStack_PopCf((UiRootNode *)sourceNode);
  (*g_SoftwareBuildPixelPackTables)(colorScaleQ16,colorBiasQ16);
  return;
}


/* Address: 0x004242D0.
   Ownership: ui/controls/misc.
   Purpose: Binary entry is anchored by g_CodePointerTable_00424324[1]@00424324. UiRootCallbacks root callback with
   one stack argument.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
UiFourValueDialog_TickCountdownAndRequestClose(UiRootNode *root)

{
  sdword *countdownTicksField;
  UiNodeVtable **countdownNumberField;
  
  countdownTicksField = &FOUR_VALUE_DIALOG_UI_FIELD(root,countdownMessageText,0x60,sdword);
  *countdownTicksField = *countdownTicksField + -1;
  if (*countdownTicksField == 0) {
    FOUR_VALUE_DIALOG_UI_FIELD(root,countdownMessageText,0x60,sdword) = 0x14;
    countdownNumberField = &FOUR_VALUE_DIALOG_UI_FIELD(root,countdownMessageText,0x5C,struct UiNodeVtable *);
    *countdownNumberField = (UiNodeVtable *)((int)&(*countdownNumberField)[-1].pointerWheel + 3);
    if (*countdownNumberField == (UiNodeVtable *)0x0) {
      UiActionQueue_Enqueue(0x20d,root);
    }
    else {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(sdword)FOUR_VALUE_DIALOG_UI_FIELD(root,countdownMessageText,0x5C,struct UiNodeVtable *),
                 (word *)&FOUR_VALUE_DIALOG_UI_FIELD(root,countdownMessageText,0x74,sdword));
    }
  }
  return;
}


/* Address: 0x004B3F40.
   Ownership: ui/controls/misc.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B3EF0[2]@004B3EF0.
   Cross-module calls: UiWindow_BlitTiledVerticalEdge [ui/controls/layout], UiWindow_BlitTiledHorizontalEdge
   [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiRangeSliderControl_DrawTrackAndThumb
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control)

{
  UiNodeBase *rangeMax;
  ulonglong scaledOffset;
  uint rangeOrValueOffset;
  UiNodeBase *clampedValue;
  dword subresourceBase;
  int edgeLength;
  uint valueOffsetOrRange;
  bool accessFailed;
  GraphicsTextureSizeEaxEdxCf9 textureSize;
  
  accessFailed = (*g_GraphicsFramebufferBeginAccess)();
  if (!accessFailed) {
    subresourceBase = 0xac;
    if ((control->nodeFlags & UI_NODE_SUPPRESSED) != 0) {
      subresourceBase = 0xb0;
    }
    if (((uint)control[1].nextSibling & 1) != 0) {
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,control->top,control->left,subresourceBase + 8,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(subresourceBase + 8,g_UiWindowTextureSource);
      edgeLength = control->layoutHeight - textureSize.logicalHeightPixels;
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,subresourceBase + 9,edgeLength,textureSize.logicalHeightPixels,0,
                 control);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,edgeLength + control->top,control->left,subresourceBase + 10,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(subresourceBase + 0xb,g_UiWindowTextureSource);
      rangeMax = control[1].parent;
      clampedValue = (UiNodeBase *)control[1].vtable;
      if ((int)rangeMax < (int)control[1].vtable) {
        clampedValue = rangeMax;
      }
      rangeOrValueOffset = (int)rangeMax - (int)control[1].firstChild;
      if (rangeOrValueOffset == 0) {
        rangeOrValueOffset = 1;
      }
      valueOffsetOrRange = (int)clampedValue - (int)control[1].firstChild;
      if ((int)valueOffsetOrRange < 0) {
        valueOffsetOrRange = 0;
      }
      if (((uint)control[1].nextSibling & 8) == 0) {
        valueOffsetOrRange = rangeOrValueOffset - valueOffsetOrRange;
      }
      scaledOffset = (ulonglong)valueOffsetOrRange * (ulonglong)(control->layoutHeight - textureSize.logicalHeightPixels);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,
                 (int)(scaledOffset / rangeOrValueOffset) + (uint)(rangeOrValueOffset < (uint)((int)(scaledOffset % (ulonglong)rangeOrValueOffset) * 2))
                 + control->top,control->left,subresourceBase + 0xb,g_UiWindowTextureSource,g_FramebufferAccess
                );
      (*g_GraphicsFramebufferEndAccess)();
      return;
    }
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,control->top,control->left,subresourceBase,
               g_UiWindowTextureSource,g_FramebufferAccess);
    textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(subresourceBase,g_UiWindowTextureSource);
    edgeLength = control->layoutWidth - textureSize.logicalWidthPixels;
    UiWindow_BlitTiledHorizontalEdge
              (clipTop,clipLeft,clipBottom,clipRight,subresourceBase + 1,edgeLength,0,textureSize.logicalWidthPixels,
               control);
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,control->top,edgeLength + control->left,subresourceBase + 2,
               g_UiWindowTextureSource,g_FramebufferAccess);
    textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(subresourceBase + 3,g_UiWindowTextureSource);
    rangeMax = control[1].parent;
    clampedValue = (UiNodeBase *)control[1].vtable;
    if ((int)rangeMax < (int)control[1].vtable) {
      clampedValue = rangeMax;
    }
    rangeOrValueOffset = (int)clampedValue - (int)control[1].firstChild;
    if ((int)rangeOrValueOffset < 0) {
      rangeOrValueOffset = 0;
    }
    valueOffsetOrRange = (int)rangeMax - (int)control[1].firstChild;
    if (valueOffsetOrRange == 0) {
      valueOffsetOrRange = 1;
    }
    if (((uint)control[1].nextSibling & 8) != 0) {
      rangeOrValueOffset = valueOffsetOrRange - rangeOrValueOffset;
    }
    scaledOffset = (ulonglong)rangeOrValueOffset * (ulonglong)(control->layoutWidth - textureSize.logicalWidthPixels);
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,control->top,
               (int)(scaledOffset / valueOffsetOrRange) + (uint)(valueOffsetOrRange < (uint)((int)(scaledOffset % (ulonglong)valueOffsetOrRange) * 2)) +
               control->left,subresourceBase + 3,g_UiWindowTextureSource,g_FramebufferAccess);
    (*g_GraphicsFramebufferEndAccess)();
  }
  return;
}


/* Address: 0x004B41C0.
   Ownership: ui/controls/misc.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B3EF0[4]@004B3EF0.
*/
void __thandor_void_preserve_eax_ecx_edx
UiRangeSliderControl_BeginThumbDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control)

{
  int localX;
  int localY;
  GraphicsTextureSizeEaxEdxCf9 thumbSize;
  
  if (((((control->nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
       (localX = pointerX - control->left, control->left <= pointerX)) &&
      (localY = pointerY - control->top, control->top <= pointerY)) &&
     ((localX < control->layoutWidth && (localY < control->layoutHeight)))) {
    if (((uint)control[1].nextSibling & 1) == 0) {
      thumbSize = (*g_GraphicsTextureSourceGetLogicalSize)(0xaf,g_UiWindowTextureSource);
      if ((int)thumbSize.logicalHeightPixels <= localY) {
        return;
      }
      control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling | 2);
    }
    else {
      thumbSize = (*g_GraphicsTextureSourceGetLogicalSize)(0xb7,g_UiWindowTextureSource);
      if ((int)thumbSize.logicalWidthPixels <= localX) {
        return;
      }
      control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling | 2);
    }
    if ((((uint)control[1].nextSibling & 4) != 0) && (control[1].right != 0)) {
      (*g_SoundPlayOneShot)
                (g_UiSoundGainQ15,g_UiSoundGainQ15,(DirectSoundVoiceSet *)control[1].right);
    }
  }
  return;
}


/* Address: 0x004B4280.
   Ownership: ui/controls/misc.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B3EF0[5]@004B3EF0.
*/
void UiRangeSliderControl_EndThumbDrag
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiNodeBase *control)

{
  control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling & 0xfffffffd);
  if ((((control->nodeFlags & UI_NODE_SUPPRESSED) == 0) && (((uint)control[1].nextSibling & 4) != 0)
      ) && (control[1].right != 0)) {
    (*g_SoundPlayOneShot)(g_UiSoundGainQ15,g_UiSoundGainQ15,(DirectSoundVoiceSet *)control[1].right)
    ;
  }
  return;
}


/* Address: 0x004B45F0.
   Ownership: ui/controls/misc.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B3EF0[14]@004B3EF0.
   Cross-module calls: UiKeyboardFocus_ReleaseNode [ui/controls/input], UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
UiRangeSliderControl_SuppressIfActionId(UiActionId actionId,UiNodeBase *control)

{
  if (actionId == control[1].top) {
    control->nodeFlags = control->nodeFlags | UI_NODE_SUPPRESSED;
    UiKeyboardFocus_ReleaseNode(control);
    UiNode_InvalidateRoot(control);
  }
  return;
}


/* Address: 0x004B4620.
   Ownership: ui/controls/misc.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B3EF0[15]@004B3EF0.
   Cross-module calls: UiKeyboardFocus_AcquireIfNone [ui/controls/input], UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
UiRangeSliderControl_UnsuppressIfActionId(UiActionId actionId,UiNodeBase *control)

{
  if (actionId == control[1].top) {
    control->nodeFlags = control->nodeFlags & ~UI_NODE_SUPPRESSED;
    UiKeyboardFocus_AcquireIfNone(control);
    UiNode_InvalidateRoot(control);
  }
  return;
}


/* Address: 0x004BC5C0.
   Ownership: ui/controls/misc.
   Purpose: Handles ui image control draw clipped.
   Cross-module calls: UiContainer_DrawIntersectingChildren [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiImageControl_DrawClipped
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
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
       (((control->selectable).stateFlags & 0x40) == 0)) {
      accessFailed = (*g_GraphicsFramebufferBeginAccess)();
      if (!accessFailed) {
        if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
          subresource = control->normalSubresource;
        }
        else {
          subresource = control->alternateSubresource;
        }
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,(control->selectable).base.top,
                   (control->selectable).base.left,subresource,control->textureSource,g_FramebufferAccess);
        (*g_GraphicsFramebufferEndAccess)();
      }
    }
  }
  return;
}


/* Address: 0x004BC6E0.
   Ownership: ui/controls/misc.
   Purpose: Handles left/middle press using opaque-pixel testing, optional pointer sound, image state changes,
   active-child tracking, and root invalidation.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
UiImageControl_NonRightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiImageControl *control)

{
  UiSelectableStateFlags *pressStateFlagsField;
  bool opaquePixelHit;
  bool opaqueHit;
  UiSelectableStateFlags *stateFlagsField;
  
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) != 0) {
    return;
  }
  if (((((control->selectable).stateFlags & 0x400) == 0) &&
      (((control->selectable).stateFlags & 0x20) != 0)) && (control->pointerActivationSoundId != 0))
  {
    (*g_SoundPlayOneShot)
              (g_UiSoundGainQ15,g_UiSoundGainQ15,
               (DirectSoundVoiceSet *)control->pointerActivationSoundId);
  }
  if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
    if (((control->selectable).stateFlags & 0x40) == 0) {
      opaqueHit = (*g_GraphicsTextureSourceTestOpaquePixel)
                        (pointerY,pointerX,(control->selectable).base.top,
                         (control->selectable).base.left,control->normalSubresource,
                         control->textureSource);
    }
    else {
      opaqueHit = (*g_GraphicsTextureSourceTestOpaquePixel)
                        (pointerY,pointerX,(control->selectable).base.top,
                         (control->selectable).base.left,control->alternateSubresource,
                         control->textureSource);
    }
    if (opaqueHit) {
      control->activeChild = (UiNodeBase *)0x0;
      g_UiImageControlHoverTarget = (UiImageControl *)0x0;
      stateFlagsField = &(control->selectable).stateFlags;
      *stateFlagsField = *stateFlagsField & 0xfffff5fc;
      goto UiImageControl_InvalidateAfterNonRightPress;
    }
  }
  control->activeChild = (UiNodeBase *)0x0;
  g_UiImageControlHoverTarget = (UiImageControl *)0x0;
  pressStateFlagsField = &(control->selectable).stateFlags;
  *pressStateFlagsField = *pressStateFlagsField | 0xa03;
UiImageControl_InvalidateAfterNonRightPress:
  UiNode_InvalidateRoot((UiNodeBase *)control);
  return;
}


/* Address: 0x004BC7E0.
   Ownership: ui/controls/misc.
   Purpose: Releases the active child when present, updates image hover/armed state, optionally plays the pointer
   sound, and invalidates the control.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
UiImageControl_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiImageControl *control)

{
  UiSelectableStateFlags *hoverStateFlagsField;
  UiNodeBase *previousActiveChild;
  UiSelectableStateFlags *stateFlagsField;
  UiNodeVtable *activeChildVtable;
  
  previousActiveChild = control->activeChild;
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) != 0)
  goto UiImageControl_InvalidateAfterNonRightRelease;
  if (((control->selectable).stateFlags & 0x200) == 0) {
    if (previousActiveChild != (UiNodeBase *)0x0) {
      activeChildVtable = previousActiveChild->vtable;
      control->activeChild = (UiNodeBase *)0x0;
      (*activeChildVtable->nonRightRelease)(wheelDelta,pointerY,pointerX,previousActiveChild);
      if ((((control->selectable).stateFlags & 0x400) == 0) ||
         (((control->selectable).stateFlags & 0x800) != 0))
      goto UiImageControl_PreserveHoverAfterNonRightRelease;
    }
    g_UiImageControlHoverTarget = (UiImageControl *)0x0;
    stateFlagsField = &(control->selectable).stateFlags;
    *stateFlagsField = *stateFlagsField & 0xfffff9fc;
    if ((((control->selectable).stateFlags & 0x20) != 0) && (control->pointerActivationSoundId != 0)
       ) {
      (*g_SoundPlayOneShot)
                (g_UiSoundGainQ15,g_UiSoundGainQ15,
                 (DirectSoundVoiceSet *)control->pointerActivationSoundId);
    }
  }
  else {
UiImageControl_PreserveHoverAfterNonRightRelease:
    hoverStateFlagsField = &(control->selectable).stateFlags;
    *hoverStateFlagsField = *hoverStateFlagsField | 0x400;
    g_UiImageControlHoverTarget = control;
    hoverStateFlagsField = &(control->selectable).stateFlags;
    *hoverStateFlagsField = *hoverStateFlagsField & 0xfffffdff;
  }
UiImageControl_InvalidateAfterNonRightRelease:
  UiNode_InvalidateRoot((UiNodeBase *)control);
  return;
}


/* Address: 0x004BD2A0.
   Ownership: ui/controls/misc.
   Purpose: Obtains the current UI state tint and propagates it through the attached model hierarchy when it
   differs from the stored model tint.
   Cross-module calls: UiNode_GetStateTintArgb [ui/core/runtime], ModelNodeRuntime_ApplyTintRecursive
   [world/model/hierarchy].
*/
void __thandor_preserve_eax UiModelControl_RefreshStateTint(ModelRuntimeNode *control)

{
  PackedArgb32 tintArgb;
  
  tintArgb = UiNode_GetStateTintArgb((UiNodeBase *)control);
  if (tintArgb != control->tintArgb) {
    ModelNodeRuntime_ApplyTintRecursive(tintArgb,control);
  }
  return;
}


/* Address: 0x00517E30.
   Ownership: ui/controls/misc.
   Purpose: Handles ui horizontal gauge control update runtime range and draw.
   Cross-module calls: UiHorizontalGaugeControl_DrawFrameFillAndLabel [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiHorizontalGaugeControl_UpdateRuntimeRangeAndDraw
          (int clipTop,int clipLeft,int clipBottom,int clipRight,UiNodeBase *control)

{
  UiTransferPayloadByteCount receivedTotal;
  UiNodeBase *minimumProgress;
  UiNodeVtable *receivedDone;
  int remainingPlayers;
  FrontendPlayerRuntimeRecord *playerRecord;
  
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  receivedTotal = g_UiTransferMailbox.receivedByteCount;
  minimumProgress = (UiNodeBase *)g_UiTransferMailbox.outgoingByteCount;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    if (g_UiTransferMailbox.outgoingByteCount == 0) {
      return;
    }
    remainingPlayers = g_FrontendPlayerRuntimeBlockCount - 1;
    if (remainingPlayers == 0) {
      return;
    }
    control[1].firstChild = (UiNodeBase *)0x0;
    control[1].parent = minimumProgress;
    do {
      if ((int)playerRecord[1].runtimeState70 < (int)minimumProgress) {
        minimumProgress = (UiNodeBase *)playerRecord[1].runtimeState70;
      }
      remainingPlayers = remainingPlayers + -1;
      playerRecord = playerRecord + 1;
    } while (remainingPlayers != 0);
    control[1].vtable = (UiNodeVtable *)minimumProgress;
    if (control[1].parent <= minimumProgress) {
      return;
    }
  }
  else {
    if ((g_UiTransferMailbox.receivedByteCount == 0) &&
       (g_UiTransferMailbox.receivedRemainingBytes == 0)) {
      return;
    }
    receivedDone = (UiNodeVtable *)
             (g_UiTransferMailbox.receivedByteCount - g_UiTransferMailbox.receivedRemainingBytes);
    control[1].firstChild = (UiNodeBase *)0x0;
    control[1].parent = (UiNodeBase *)receivedTotal;
    control[1].vtable = receivedDone;
    if (receivedTotal <= receivedDone) {
      return;
    }
  }
  UiHorizontalGaugeControl_DrawFrameFillAndLabel(clipTop,clipLeft,clipBottom,clipRight,control);
  return;
}


/* Address: 0x00423600.
   Ownership: ui/controls/misc.
   Purpose: Opens and populates the display-mode selection UI; CF carries success/failure.
   Local calls: UiDisplayModeSelection_RefreshEnumeratedOptions.
   Cross-module calls: UiRuntime_FormatSignedValues140And144 [ui/core/runtime], UiActionHandlers_SetPageCf
   [ui/core/runtime], UiRootStack_Push [ui/controls/layout], UiRootStack_InvalidateAll [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx UiDisplaySettings_OpenAndPopulateModeSelectionCf(void)

{
  DisplayModeScratchWord adapterOption1Or4;
  DisplayModeScratchWord adapterOption2;
  dword framebufferWidth;
  dword framebufferHeight;
  UiRootFlags activeAdapterIndex;
  sdword colorScaleQ16;
  sdword colorBiasQ16;
  UiRootNode *root;
  uint insertValueA;
  uint insertValueB;
  int copyCountOrRgBits;
  UiNodeFlags colorDepthBits;
  GraphicsDisplayModeCount remainingModes;
  UiRootNode *copyCursorOrLowWord;
  UiNodeVtable *lowWordValue;
  undefined4 *templateCursor;
  GraphicsDisplayMode *displayMode;
  ArenaAllocEaxCf5 allocResult;
  
  if (1 < g_GraphicsDisplayModeCount) {
    allocResult = (*g_MemoryApi.alloc)(0xbd4);
    root = (UiRootNode *)allocResult.eax;
    if (allocResult.carry) {
      return;
    }
    templateCursor = (undefined4 *)THANDOR_ADDR(g_UiDisplaySettingsRootTemplate,0);
    copyCursorOrLowWord = root;
    for (copyCountOrRgBits = 0x2f5; activeAdapterIndex = g_ActiveGraphicsAdapterIndex, framebufferHeight = g_FramebufferHeight,
        framebufferWidth = g_FramebufferWidth, copyCountOrRgBits != 0; copyCountOrRgBits = copyCountOrRgBits + -1) {
      (copyCursorOrLowWord->base).nextSibling = (UiNodeBase *)*templateCursor;
      templateCursor = templateCursor + 1;
      copyCursorOrLowWord = (UiRootNode *)&(copyCursorOrLowWord->base).firstChild;
    }
    colorDepthBits = g_SoftwarePixelFormatConfig.redBitCount + g_SoftwarePixelFormatConfig.greenBitCount +
             g_SoftwarePixelFormatConfig.blueBitCount;
    root[3].base.rightOffset = g_FramebufferWidth;
    root[3].base.bottomOffset = framebufferHeight;
    root[3].base.leftAnchorQ31 = colorDepthBits;
    root[3].base.topAnchorQ31 = activeAdapterIndex;
    root[3].base.layoutWidth = framebufferWidth;
    root[3].base.layoutHeight = framebufferHeight;
    root[3].base.nodeFlags = colorDepthBits;
    root[3].rootFlags = activeAdapterIndex;
    colorBiasQ16 = g_SoftwareColorBiasQ16;
    colorScaleQ16 = g_SoftwareColorScaleQ16;
    root[3].base.rightAnchorQ31 = g_SoftwareColorBiasQ16;
    root[3].base.bottomAnchorQ31 = colorScaleQ16;
    root[3].callbacks = (UiRootCallbacks *)colorBiasQ16;
    root[3].previousRoot = (UiRootNode *)colorScaleQ16;
    root[0x1f].base.rightOffset = colorBiasQ16;
    root[0x1d].base.right = colorScaleQ16;
    root[0x21].base.topAnchorQ31 = (UiAnchorFractionQ31)&root[0x22].base.firstChild;
    root[0x20].base.leftAnchorQ31 = (UiAnchorFractionQ31)&root[0x21].base.bottomAnchorQ31;
    UiRuntime_FormatSignedValues140And144(root);
    UiActionHandlers_SetPageCf(2,(UiActionHandlerPage *)&g_UiDisplayModeSelectionActionHandlers20);
    UiRootStack_Push(&g_UiDisplaySettingsRootCallbacks,root);
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
    root[7].base.left = g_UiDisplayModeDistinctValueScratch0;
    root[8].base.leftOffset = g_UiDisplayModeDistinctValueScratch1;
    root[9].base.leftAnchorQ31 = g_UiDisplayModeDistinctValueScratch2;
    root[10].base.layoutWidth = g_UiDisplayModeDistinctValueScratch3;
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
    root[0xb].callbacks = (UiRootCallbacks *)(g_UiDisplayModeDistinctValueScratch0 >> 0x10);
    root[0xb].rootFlags = g_UiDisplayModeDistinctValueScratch0 & 0xffff;
    root[0xd].base.parent = (UiNodeBase *)(g_UiDisplayModeDistinctValueScratch1 >> 0x10);
    root[0xd].base.firstChild = (UiNodeBase *)(g_UiDisplayModeDistinctValueScratch1 & 0xffff);
    root[0xe].base.right = g_UiDisplayModeDistinctValueScratch2 >> 0x10;
    root[0xe].base.top = g_UiDisplayModeDistinctValueScratch2 & 0xffff;
    root[0xf].base.rightOffset = g_UiDisplayModeDistinctValueScratch3 >> 0x10;
    root[0xf].base.topOffset = g_UiDisplayModeDistinctValueScratch3 & 0xffff;
    root[0x10].base.rightAnchorQ31 = g_UiDisplayModeDistinctValueScratch4 >> 0x10;
    root[0x10].base.topAnchorQ31 = g_UiDisplayModeDistinctValueScratch4 & 0xffff;
    insertValueA = g_UiDisplayModeDistinctValueScratch5 & 0xffff;
    root[0x11].base.nodeFlags = g_UiDisplayModeDistinctValueScratch5 >> 0x10;
    root[0x11].base.layoutHeight = insertValueA;
    copyCursorOrLowWord = (UiRootNode *)(g_UiDisplayModeDistinctValueScratch6 & 0xffff);
    root[0x13].base.nextSibling = (UiNodeBase *)(g_UiDisplayModeDistinctValueScratch6 >> 0x10);
    root[0x12].previousRoot = copyCursorOrLowWord;
    lowWordValue = (UiNodeVtable *)(g_UiDisplayModeDistinctValueScratch7 & 0xffff);
    root[0x14].base.left = g_UiDisplayModeDistinctValueScratch7 >> 0x10;
    root[0x14].base.vtable = lowWordValue;
    g_UiDisplayModeDistinctValueScratch0 = 0xffffffff;
    g_UiDisplayModeDistinctValueScratch1 = 0xffffffff;
    g_UiDisplayModeDistinctValueScratch2 = 0xffffffff;
    g_UiDisplayModeDistinctValueScratch3 = 0xffffffff;
    g_UiDisplayModeDistinctValueScratch4 = 0xffffffff;
    remainingModes = g_GraphicsDisplayModeCount;
    displayMode = g_GraphicsDisplayModes;
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
    root[0x15].base.leftOffset = g_UiDisplayModeDistinctValueScratch0;
    root[0x16].base.leftAnchorQ31 = adapterOption1Or4;
    root[0x17].base.layoutWidth = adapterOption2;
    adapterOption1Or4 = g_UiDisplayModeDistinctValueScratch4;
    copyCountOrRgBits = g_SoftwarePixelFormatConfig.redBitCount + g_SoftwarePixelFormatConfig.greenBitCount;
    root[0x18].callbacks = (UiRootCallbacks *)g_UiDisplayModeDistinctValueScratch3;
    root[0x1a].base.parent = (UiNodeBase *)adapterOption1Or4;
    UiDisplayModeSelection_RefreshEnumeratedOptions
              (g_ActiveGraphicsAdapterIndex,
               (UiNodeBase *)(copyCountOrRgBits + g_SoftwarePixelFormatConfig.blueBitCount),g_FramebufferHeight
               ,g_FramebufferWidth,(UiNodeBase *)root);
    UiRootStack_InvalidateAll();
  }
  return;
}


/* Address: 0x004BC9B0.
   Ownership: ui/controls/misc.
   Purpose: Returns the image control only for an opaque texture-source pixel, optionally descends into children,
   and clears the transient opaque-hit state when no pixel matches.
   Cross-module calls: UiContainer_HitTestChildren [ui/controls/layout].
*/
UiNodeBase * __thandor_eax_preserve_ecx_edx
UiImageControl_HitTestOpaque
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiImageControl *control)

{
  UiImageControl *hitNode;
  bool opaquePixelHit;
  bool opaqueHit;
  UiSelectableStateFlags *stateFlagsField;
  
  hitNode = (UiImageControl *)0xffffffff;
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (((control->selectable).stateFlags & 0x40) == 0) {
      opaqueHit = (*g_GraphicsTextureSourceTestOpaquePixel)
                        (pointerY,pointerX,(control->selectable).base.top,
                         (control->selectable).base.left,control->normalSubresource,
                         control->textureSource);
      if (opaqueHit) {
        return (UiNodeBase *)control;
      }
    }
    else {
      opaqueHit = (*g_GraphicsTextureSourceTestOpaquePixel)
                        (pointerY,pointerX,(control->selectable).base.top,
                         (control->selectable).base.left,control->alternateSubresource,
                         control->textureSource);
      if (opaqueHit) {
        return (UiNodeBase *)control;
      }
    }
    stateFlagsField = &(control->selectable).stateFlags;
    *stateFlagsField = *stateFlagsField & 0xfffffdff;
    hitNode = (UiImageControl *)0xffffffff;
    if (((((control->selectable).stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) != 0) &&
        (hitNode = (UiImageControl *)
                   UiContainer_HitTestChildren(pointerY,pointerX,(UiNodeBase *)control),
        hitNode == control)) && (g_UiImageControlHoverTarget != (UiImageControl *)0x0)) {
      hitNode = (UiImageControl *)0xffffffff;
    }
  }
  return (UiNodeBase *)hitNode;
}


/* Address: 0x00423D70.
   Ownership: ui/controls/misc.
   Purpose: Queries the enumerated graphics modes, suppresses or restores display-mode actions, updates the
   selected resolution and bit-depth groups, stores the selected tuple, and gates the apply action when the
   selection is unchanged.
   Cross-module calls: GraphicsDisplayMode_IsEnumeratedCf [graphics/backend/directdraw],
   UiNodeList_SuppressActionId [ui/controls/lists], UiNodeList_UnsuppressActionId [ui/controls/lists],
   UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
UiDisplayModeSelection_RefreshEnumeratedOptions
          (FrontendDisplayAdapterIndex adapterIndex,UiNodeBase *selectedModeValue,
          FrontendDisplayDimensionPixels modeHeight,FrontendDisplayDimensionPixels modeWidth,
          UiNodeBase *displaySettingsRoot)

{
  /* Rewritten from the assembly (0x00423D70): every option button is a 0x68-byte node; the dwords
     just before each button hold its mode value(s). The decompiled struct indexing picked wrong
     fields, so no resolution button was ever marked as selected. */
  static const unsigned depthButtons[4] = {0x280,0x2e8,0x350,0x3b8};
  static const unsigned sizeButtons[8] = {0x420,0x488,0x4f0,0x558,0x5c0,0x628,0x690,0x6f8};
  static const unsigned adapterButtons[5] = {0x760,0x7c8,0x830,0x898,0x900};
  byte *root = (byte *)displaySettingsRoot;
  dword bitsPerPixel = (dword)(uintptr_t)selectedModeValue;
  void *selected = (void *)0;
  bool enumerated;
  int i;

#define DISPLAY_MODE_FIELD(offset) (*(dword *)(root + (offset)))
  for (i = 0; i < 4; i++) {
    dword depth = DISPLAY_MODE_FIELD(depthButtons[i] - 8);
    enumerated = GraphicsDisplayMode_IsEnumeratedCf(adapterIndex,depth,modeHeight,modeWidth);
    if (enumerated) {
      UiNodeList_SuppressActionId(0x201 + i,displaySettingsRoot);
    }
    else {
      UiNodeList_UnsuppressActionId(0x201 + i,displaySettingsRoot);
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
    dword height = DISPLAY_MODE_FIELD(sizeButtons[i] - 0xc);
    dword width = DISPLAY_MODE_FIELD(sizeButtons[i] - 8);
    enumerated = GraphicsDisplayMode_IsEnumeratedCf(adapterIndex,bitsPerPixel,height,width);
    if (enumerated) {
      UiNodeList_SuppressActionId(0x205 + i,displaySettingsRoot);
    }
    else {
      UiNodeList_UnsuppressActionId(0x205 + i,displaySettingsRoot);
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
    dword adapter = DISPLAY_MODE_FIELD(adapterButtons[i] - 8);
    enumerated = GraphicsDisplayMode_IsEnumeratedCf(adapter,bitsPerPixel,modeHeight,modeWidth);
    if (enumerated) {
      UiNodeList_SuppressActionId(0x20f + i,displaySettingsRoot);
    }
    else {
      UiNodeList_UnsuppressActionId(0x20f + i,displaySettingsRoot);
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
  DISPLAY_MODE_FIELD(0x130) = modeWidth;
  DISPLAY_MODE_FIELD(0x134) = modeHeight;
  DISPLAY_MODE_FIELD(0x138) = bitsPerPixel;
  DISPLAY_MODE_FIELD(0x13c) = adapterIndex;
  if ((modeWidth == DISPLAY_MODE_FIELD(0x148)) && (modeHeight == DISPLAY_MODE_FIELD(0x14c)) &&
      (bitsPerPixel == DISPLAY_MODE_FIELD(0x150)) && (adapterIndex == DISPLAY_MODE_FIELD(0x154)) &&
      (DISPLAY_MODE_FIELD(0x140) == DISPLAY_MODE_FIELD(0x158)) &&
      (DISPLAY_MODE_FIELD(0x144) == DISPLAY_MODE_FIELD(0x15c))) {
    UiNodeList_SuppressActionId(0x200,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x200,displaySettingsRoot);
  }
#undef DISPLAY_MODE_FIELD
  return;
}

