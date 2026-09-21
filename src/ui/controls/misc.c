/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/misc.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/misc.h>

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
  UiAnchorFractionQ31 arg1;
  UiAnchorFractionQ31 arg0;
  
  arg1 = root[0x1f].base.rightOffset;
  arg0 = root[0x1d].base.right;
  if ((arg1 != root[3].base.rightAnchorQ31) || (arg0 != root[3].base.bottomAnchorQ31)) {
    root[3].base.rightAnchorQ31 = arg1;
    root[3].base.bottomAnchorQ31 = arg0;
    (*g_SoftwareBuildPixelPackTables)(arg0,arg1);
    UiDisplayModeSelection_RefreshEnumeratedOptions
              (root[3].base.topAnchorQ31,(UiNodeBase *)root[3].base.leftAnchorQ31,
               root[3].base.bottomOffset,root[3].base.rightOffset,&root->base);
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
  longlong lVar1;
  UiRootNode *pUVar2;
  DisplayModeEaxCf5 DVar3;
  dword arg0;
  dword arg1;
  dword arg2;
  dword arg3;
  
  pUVar2 = (UiRootNode *)UiNode_GetRoot(sourceNode);
  arg3 = pUVar2[4].base.top;
  arg2 = pUVar2[4].base.right;
  arg1 = pUVar2[4].base.bottom;
  arg0 = pUVar2[4].base.leftOffset;
  UiRootStack_PopCf(pUVar2);
  g_CursorVisibilityToken = g_CursorVisibilityToken + -1;
  UiFrame_ProcessAndPresentWithLockTransition();
  DVar3 = (*g_GraphicsDisplayModeHook)(arg0,arg1,arg2,arg3);
  (*g_FatalErrorPrimaryDispatchCf)(DVar3.eax,DVar3.carry);
  pUVar2 = g_UiRootNode;
  do {
    lVar1 = (ulonglong)g_FramebufferWidth * (ulonglong)(pUVar2->base).rightAnchorQ31;
    (pUVar2->base).right =
         ((int)((ulonglong)lVar1 >> 0x20) << 1 | (uint)lVar1 >> 0x1f) + (pUVar2->base).rightOffset;
    lVar1 = (ulonglong)g_FramebufferHeight * (ulonglong)(pUVar2->base).bottomAnchorQ31;
    (pUVar2->base).bottom =
         ((int)((ulonglong)lVar1 >> 0x20) << 1 | (uint)lVar1 >> 0x1f) + (pUVar2->base).bottomOffset;
    lVar1 = (ulonglong)g_FramebufferWidth * (ulonglong)(pUVar2->base).leftAnchorQ31;
    (pUVar2->base).left =
         ((int)((ulonglong)lVar1 >> 0x20) << 1 | (uint)lVar1 >> 0x1f) + (pUVar2->base).leftOffset;
    lVar1 = (ulonglong)g_FramebufferHeight * (ulonglong)(pUVar2->base).topAnchorQ31;
    (pUVar2->base).top =
         ((int)((ulonglong)lVar1 >> 0x20) << 1 | (uint)lVar1 >> 0x1f) + (pUVar2->base).topOffset;
    (*((pUVar2->base).vtable)->layout)(&pUVar2->base);
    pUVar2 = pUVar2->previousRoot;
  } while (pUVar2 != (UiRootNode *)0xffffffff);
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
  UiNodeVtable *pUVar1;
  UiImageControl *control_00;
  UiNodeBase *control_01;
  UiNodeBase *newActiveChild;
  UiNodeBase *control_02;
  undefined8 uVar2;
  UiSelectableStateFlags *stateFlagsField;
  
  if (((control->selectable).stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) == 0) {
    return;
  }
  control_00 = (UiImageControl *)UiImageControl_HitTestOpaque(pointerY,pointerX,control);
  if ((control_00 == control) ||
     (stateFlagsField = &(control->selectable).stateFlags,
     *stateFlagsField = *stateFlagsField & 0xfffffdff, control_00 == (UiImageControl *)0xffffffff))
  {
    control_02 = control->activeChild;
  }
  else {
    if (control_00 == (UiImageControl *)control->activeChild) {
      (*((control_00->selectable).base.vtable)->nonRightDrag)
                (wheelDelta,pointerY,pointerX,(UiNodeBase *)control_00);
      goto UiImageControl_InvalidateAfterNonRightDrag;
    }
    pUVar1 = (control_00->selectable).base.vtable;
    (*pUVar1->nonRightPress)(0,0x70000000,0x70000000,(UiNodeBase *)control_00);
    newActiveChild = (UiNodeBase *)(*pUVar1->nonRightDrag)(wheelDelta,pointerY,pointerX,control_01);
    LOCK();
    control_02 = control->activeChild;
    control->activeChild = newActiveChild;
    UNLOCK();
  }
  if (control_02 != (UiNodeBase *)0x0) {
    uVar2 = (*control_02->vtable->nonRightDrag)(0,0x70000000,0x70000000,control_02);
    (**(code **)((int)((ulonglong)uVar2 >> 0x20) + 0x14))(0,0x70000000,0x70000000,(int)uVar2);
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
  UiSelectableStateFlags *pUVar1;
  UiNodeVtable *pUVar2;
  UiImageControl *control_00;
  undefined4 uVar3;
  int extraout_ECX;
  UiSelectableStateFlags *stateFlagsField;
  UiNodeVtable *hoveredControlVtable;
  UiSelectableStateFlags *hoverStateFlagsField;
  
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (((control->selectable).stateFlags & 0x100) == 0) {
      if ((g_CursorButtonState & 4) != 0) {
        control_00 = (UiImageControl *)
                     UiImageControl_HitTestOpaque(g_CursorOverrideY,g_CursorOverrideX,control);
        stateFlagsField = &(control->selectable).stateFlags;
        *stateFlagsField = *stateFlagsField | 0x100;
        if ((control_00 != control) && (control_00 != (UiImageControl *)0xffffffff)) {
          pUVar2 = (control_00->selectable).base.vtable;
          hoverStateFlagsField = &(control->selectable).stateFlags;
          *hoverStateFlagsField = *hoverStateFlagsField & 0xfffff7ff;
          (*pUVar2->nonRightRelease)
                    (g_CursorWheelDelta,g_CursorOverrideY,g_CursorOverrideX,(UiNodeBase *)control_00
                    );
          uVar3 = (*pUVar2->nonRightPress)
                            (g_CursorWheelDelta,g_CursorOverrideY,g_CursorOverrideX,control_00);
          (**(code **)(extraout_ECX + 0x20))
                    (g_CursorWheelDelta,g_CursorOverrideY,g_CursorOverrideX,uVar3);
        }
      }
    }
    else if ((g_CursorButtonState & 4) == 0) {
      pUVar1 = &(control->selectable).stateFlags;
      *pUVar1 = *pUVar1 & 0xfffffeff;
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
  dword arg1;
  dword arg0;
  UiRootNode *root;
  dword arg3;
  dword arg2;
  dword arg1_00;
  DisplayModeEaxCf5 DVar1;
  DisplayModeEaxCf5 DVar2;
  uint arg0_00;
  dword dVar3;
  dword dVar4;
  
  root = (UiRootNode *)UiNode_GetRoot(sourceNode);
  dVar3 = root[3].base.rightOffset;
  dVar4 = root[3].base.bottomOffset;
  arg1 = root[3].base.leftAnchorQ31;
  arg1_00 = g_SoftwarePixelFormatConfig.redBitCount + g_SoftwarePixelFormatConfig.greenBitCount +
            g_SoftwarePixelFormatConfig.blueBitCount;
  arg0 = root[3].base.topAnchorQ31;
  UiRootStack_PopCf(root);
  if ((((dVar3 != g_FramebufferWidth) || (dVar4 != g_FramebufferHeight)) || (arg1 != arg1_00)) ||
     (arg0 != g_ActiveGraphicsAdapterIndex)) {
    g_CursorVisibilityToken = g_CursorVisibilityToken + -1;
    UiFrame_ProcessAndPresentWithLockTransition();
    arg0_00 = g_ActiveGraphicsAdapterIndex;
    dVar3 = g_FramebufferHeight;
    dVar4 = g_FramebufferWidth;
    DVar1 = (*g_GraphicsDisplayModeHook)(arg0,arg1,arg2,arg3);
    if (DVar1.carry) {
      DVar2 = (*g_GraphicsDisplayModeHook)(arg0_00,arg1_00,dVar3,dVar4);
      (*g_FatalErrorPrimaryDispatchCf)(DVar2.eax,DVar2.carry);
      g_CursorVisibilityToken = g_CursorVisibilityToken + 1;
      (*g_FatalErrorRuntimeDispatchCf)(DVar1.eax,true);
      return;
    }
    UiRootStack_Relayout();
    g_CursorVisibilityToken = g_CursorVisibilityToken + 1;
    UiRuntime_OpenFourValueDialogCf(arg0_00,arg1_00,dVar3,dVar4);
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
  sdword arg1;
  sdword arg0;
  UiNodeBase *pUVar1;
  
  pUVar1 = UiNode_GetRoot(sourceNode);
  arg1 = pUVar1[4].rightOffset;
  arg0 = pUVar1[4].bottomOffset;
  UiRootStack_PopCf((UiRootNode *)sourceNode);
  (*g_SoftwareBuildPixelPackTables)(arg0,arg1);
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
  
  countdownTicksField = &root[4].base.left;
  *countdownTicksField = *countdownTicksField + -1;
  if (*countdownTicksField == 0) {
    root[4].base.left = 0x14;
    countdownNumberField = &root[4].base.vtable;
    *countdownNumberField = (UiNodeVtable *)((int)&(*countdownNumberField)[-1].pointerWheel + 3);
    if (*countdownNumberField == (UiNodeVtable *)0x0) {
      UiActionQueue_Enqueue(0x20d,root);
    }
    else {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(sdword)root[4].base.vtable,
                 (word *)&root[4].base.topOffset);
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
  UiNodeBase *pUVar1;
  ulonglong uVar2;
  uint uVar3;
  UiNodeBase *pUVar4;
  dword arg6;
  int iVar5;
  uint uVar6;
  bool bVar7;
  GraphicsTextureSizeEaxEdxCf9 GVar8;
  
  bVar7 = (*g_GraphicsFramebufferBeginAccess)();
  if (!bVar7) {
    arg6 = 0xac;
    if ((control->nodeFlags & UI_NODE_SUPPRESSED) != 0) {
      arg6 = 0xb0;
    }
    if (((uint)control[1].nextSibling & 1) != 0) {
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,control->top,control->left,arg6 + 8,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      GVar8 = (*g_GraphicsTextureSourceGetLogicalSize)(arg6 + 8,g_UiWindowTextureSource);
      iVar5 = control->layoutHeight - GVar8.logicalHeightPixels;
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,arg6 + 9,iVar5,GVar8.logicalHeightPixels,0,
                 control);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,iVar5 + control->top,control->left,arg6 + 10,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      GVar8 = (*g_GraphicsTextureSourceGetLogicalSize)(arg6 + 0xb,g_UiWindowTextureSource);
      pUVar1 = control[1].parent;
      pUVar4 = (UiNodeBase *)control[1].vtable;
      if ((int)pUVar1 < (int)control[1].vtable) {
        pUVar4 = pUVar1;
      }
      uVar3 = (int)pUVar1 - (int)control[1].firstChild;
      if (uVar3 == 0) {
        uVar3 = 1;
      }
      uVar6 = (int)pUVar4 - (int)control[1].firstChild;
      if ((int)uVar6 < 0) {
        uVar6 = 0;
      }
      if (((uint)control[1].nextSibling & 8) == 0) {
        uVar6 = uVar3 - uVar6;
      }
      uVar2 = (ulonglong)uVar6 * (ulonglong)(control->layoutHeight - GVar8.logicalHeightPixels);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,
                 (int)(uVar2 / uVar3) + (uint)(uVar3 < (uint)((int)(uVar2 % (ulonglong)uVar3) * 2))
                 + control->top,control->left,arg6 + 0xb,g_UiWindowTextureSource,g_FramebufferAccess
                );
      (*g_GraphicsFramebufferEndAccess)();
      return;
    }
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,control->top,control->left,arg6,
               g_UiWindowTextureSource,g_FramebufferAccess);
    GVar8 = (*g_GraphicsTextureSourceGetLogicalSize)(arg6,g_UiWindowTextureSource);
    iVar5 = control->layoutWidth - GVar8.logicalWidthPixels;
    UiWindow_BlitTiledHorizontalEdge
              (clipTop,clipLeft,clipBottom,clipRight,arg6 + 1,iVar5,0,GVar8.logicalWidthPixels,
               control);
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,control->top,iVar5 + control->left,arg6 + 2,
               g_UiWindowTextureSource,g_FramebufferAccess);
    GVar8 = (*g_GraphicsTextureSourceGetLogicalSize)(arg6 + 3,g_UiWindowTextureSource);
    pUVar1 = control[1].parent;
    pUVar4 = (UiNodeBase *)control[1].vtable;
    if ((int)pUVar1 < (int)control[1].vtable) {
      pUVar4 = pUVar1;
    }
    uVar3 = (int)pUVar4 - (int)control[1].firstChild;
    if ((int)uVar3 < 0) {
      uVar3 = 0;
    }
    uVar6 = (int)pUVar1 - (int)control[1].firstChild;
    if (uVar6 == 0) {
      uVar6 = 1;
    }
    if (((uint)control[1].nextSibling & 8) != 0) {
      uVar3 = uVar6 - uVar3;
    }
    uVar2 = (ulonglong)uVar3 * (ulonglong)(control->layoutWidth - GVar8.logicalWidthPixels);
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,control->top,
               (int)(uVar2 / uVar6) + (uint)(uVar6 < (uint)((int)(uVar2 % (ulonglong)uVar6) * 2)) +
               control->left,arg6 + 3,g_UiWindowTextureSource,g_FramebufferAccess);
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
  int iVar1;
  int iVar2;
  GraphicsTextureSizeEaxEdxCf9 GVar3;
  
  if (((((control->nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
       (iVar1 = pointerX - control->left, control->left <= pointerX)) &&
      (iVar2 = pointerY - control->top, control->top <= pointerY)) &&
     ((iVar1 < control->layoutWidth && (iVar2 < control->layoutHeight)))) {
    if (((uint)control[1].nextSibling & 1) == 0) {
      GVar3 = (*g_GraphicsTextureSourceGetLogicalSize)(0xaf,g_UiWindowTextureSource);
      if ((int)GVar3.logicalHeightPixels <= iVar2) {
        return;
      }
      control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling | 2);
    }
    else {
      GVar3 = (*g_GraphicsTextureSourceGetLogicalSize)(0xb7,g_UiWindowTextureSource);
      if ((int)GVar3.logicalWidthPixels <= iVar1) {
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
  bool bVar1;
  GraphicsSubresourceIndex arg6;
  
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (((control->selectable).stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) != 0) {
      UiContainer_DrawIntersectingChildren
                (clipTop,clipLeft,clipBottom,clipRight,(UiNodeBase *)control);
    }
    if ((((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) ||
       (((control->selectable).stateFlags & 0x40) == 0)) {
      bVar1 = (*g_GraphicsFramebufferBeginAccess)();
      if (!bVar1) {
        if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
          arg6 = control->normalSubresource;
        }
        else {
          arg6 = control->alternateSubresource;
        }
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,(control->selectable).base.top,
                   (control->selectable).base.left,arg6,control->textureSource,g_FramebufferAccess);
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
  UiSelectableStateFlags *pUVar1;
  bool opaquePixelHit;
  bool bVar2;
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
      bVar2 = (*g_GraphicsTextureSourceTestOpaquePixel)
                        (pointerY,pointerX,(control->selectable).base.top,
                         (control->selectable).base.left,control->normalSubresource,
                         control->textureSource);
    }
    else {
      bVar2 = (*g_GraphicsTextureSourceTestOpaquePixel)
                        (pointerY,pointerX,(control->selectable).base.top,
                         (control->selectable).base.left,control->alternateSubresource,
                         control->textureSource);
    }
    if (bVar2) {
      control->activeChild = (UiNodeBase *)0x0;
      g_UiImageControlHoverTarget = (UiImageControl *)0x0;
      stateFlagsField = &(control->selectable).stateFlags;
      *stateFlagsField = *stateFlagsField & 0xfffff5fc;
      goto UiImageControl_InvalidateAfterNonRightPress;
    }
  }
  control->activeChild = (UiNodeBase *)0x0;
  g_UiImageControlHoverTarget = (UiImageControl *)0x0;
  pUVar1 = &(control->selectable).stateFlags;
  *pUVar1 = *pUVar1 | 0xa03;
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
  UiSelectableStateFlags *pUVar1;
  UiNodeBase *control_00;
  UiSelectableStateFlags *stateFlagsField;
  UiNodeVtable *activeChildVtable;
  
  control_00 = control->activeChild;
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) != 0)
  goto UiImageControl_InvalidateAfterNonRightRelease;
  if (((control->selectable).stateFlags & 0x200) == 0) {
    if (control_00 != (UiNodeBase *)0x0) {
      activeChildVtable = control_00->vtable;
      control->activeChild = (UiNodeBase *)0x0;
      (*activeChildVtable->nonRightRelease)(wheelDelta,pointerY,pointerX,control_00);
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
    pUVar1 = &(control->selectable).stateFlags;
    *pUVar1 = *pUVar1 | 0x400;
    g_UiImageControlHoverTarget = control;
    pUVar1 = &(control->selectable).stateFlags;
    *pUVar1 = *pUVar1 & 0xfffffdff;
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
  UiTransferPayloadByteCount UVar1;
  UiNodeBase *pUVar2;
  UiNodeVtable *pUVar3;
  int iVar4;
  FrontendPlayerRuntimeRecord *pFVar5;
  
  pFVar5 = g_FrontendPlayerRuntimeBlocks;
  UVar1 = g_UiTransferMailbox.receivedByteCount;
  pUVar2 = (UiNodeBase *)g_UiTransferMailbox.outgoingByteCount;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    if (g_UiTransferMailbox.outgoingByteCount == 0) {
      return;
    }
    iVar4 = g_FrontendPlayerRuntimeBlockCount - 1;
    if (iVar4 == 0) {
      return;
    }
    control[1].firstChild = (UiNodeBase *)0x0;
    control[1].parent = pUVar2;
    do {
      if ((int)pFVar5[1].runtimeState70 < (int)pUVar2) {
        pUVar2 = (UiNodeBase *)pFVar5[1].runtimeState70;
      }
      iVar4 = iVar4 + -1;
      pFVar5 = pFVar5 + 1;
    } while (iVar4 != 0);
    control[1].vtable = (UiNodeVtable *)pUVar2;
    if (control[1].parent <= pUVar2) {
      return;
    }
  }
  else {
    if ((g_UiTransferMailbox.receivedByteCount == 0) &&
       (g_UiTransferMailbox.receivedRemainingBytes == 0)) {
      return;
    }
    pUVar3 = (UiNodeVtable *)
             (g_UiTransferMailbox.receivedByteCount - g_UiTransferMailbox.receivedRemainingBytes);
    control[1].firstChild = (UiNodeBase *)0x0;
    control[1].parent = (UiNodeBase *)UVar1;
    control[1].vtable = pUVar3;
    if (UVar1 <= pUVar3) {
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
  DisplayModeScratchWord DVar1;
  DisplayModeScratchWord DVar2;
  dword dVar3;
  dword dVar4;
  UiRootFlags UVar5;
  sdword sVar6;
  sdword sVar7;
  UiRootNode *root;
  uint uVar8;
  uint uVar9;
  int iVar10;
  UiNodeFlags UVar11;
  GraphicsDisplayModeCount GVar12;
  UiRootNode *pUVar13;
  UiNodeVtable *pUVar14;
  undefined4 *puVar15;
  GraphicsDisplayMode *pGVar16;
  ArenaAllocEaxCf5 AVar17;
  
  if (1 < g_GraphicsDisplayModeCount) {
    AVar17 = (*g_MemoryApi.alloc)(0xbd4);
    root = (UiRootNode *)AVar17.eax;
    if (AVar17.carry) {
      return;
    }
    puVar15 = (undefined4 *)0x4229b4;
    pUVar13 = root;
    for (iVar10 = 0x2f5; UVar5 = g_ActiveGraphicsAdapterIndex, dVar4 = g_FramebufferHeight,
        dVar3 = g_FramebufferWidth, iVar10 != 0; iVar10 = iVar10 + -1) {
      (pUVar13->base).nextSibling = (UiNodeBase *)*puVar15;
      puVar15 = puVar15 + 1;
      pUVar13 = (UiRootNode *)&(pUVar13->base).firstChild;
    }
    UVar11 = g_SoftwarePixelFormatConfig.redBitCount + g_SoftwarePixelFormatConfig.greenBitCount +
             g_SoftwarePixelFormatConfig.blueBitCount;
    root[3].base.rightOffset = g_FramebufferWidth;
    root[3].base.bottomOffset = dVar4;
    root[3].base.leftAnchorQ31 = UVar11;
    root[3].base.topAnchorQ31 = UVar5;
    root[3].base.layoutWidth = dVar3;
    root[3].base.layoutHeight = dVar4;
    root[3].base.nodeFlags = UVar11;
    root[3].rootFlags = UVar5;
    sVar7 = g_SoftwareColorBiasQ16;
    sVar6 = g_SoftwareColorScaleQ16;
    root[3].base.rightAnchorQ31 = g_SoftwareColorBiasQ16;
    root[3].base.bottomAnchorQ31 = sVar6;
    root[3].callbacks = (UiRootCallbacks *)sVar7;
    root[3].previousRoot = (UiRootNode *)sVar6;
    root[0x1f].base.rightOffset = sVar7;
    root[0x1d].base.right = sVar6;
    root[0x21].base.topAnchorQ31 = (UiAnchorFractionQ31)&root[0x22].base.firstChild;
    root[0x20].base.leftAnchorQ31 = (UiAnchorFractionQ31)&root[0x21].base.bottomAnchorQ31;
    UiRuntime_FormatSignedValues140And144(root);
    UiActionHandlers_SetPageCf(2,(UiActionHandlerPage *)&g_UiDisplayModeSelectionActionHandlers20);
    UiRootStack_Push(&g_UiDisplaySettingsRootCallbacks,root);
    g_UiDisplayModeDistinctValueScratch0 = 0xffffffff;
    g_UiDisplayModeDistinctValueScratch1 = 0xffffffff;
    g_UiDisplayModeDistinctValueScratch2 = 0xffffffff;
    g_UiDisplayModeDistinctValueScratch3 = 0xffffffff;
    GVar12 = g_GraphicsDisplayModeCount;
    pGVar16 = g_GraphicsDisplayModes;
    do {
      uVar8 = pGVar16->bitsPerPixel;
      if ((((uVar8 != g_UiDisplayModeDistinctValueScratch0) &&
           (uVar8 != g_UiDisplayModeDistinctValueScratch1)) &&
          (uVar8 != g_UiDisplayModeDistinctValueScratch2)) &&
         (uVar8 != g_UiDisplayModeDistinctValueScratch3)) {
        uVar9 = uVar8;
        if (uVar8 < g_UiDisplayModeDistinctValueScratch0) {
          LOCK();
          UNLOCK();
          uVar9 = g_UiDisplayModeDistinctValueScratch0;
          g_UiDisplayModeDistinctValueScratch0 = uVar8;
        }
        uVar8 = uVar9;
        if (uVar9 < g_UiDisplayModeDistinctValueScratch1) {
          LOCK();
          UNLOCK();
          uVar8 = g_UiDisplayModeDistinctValueScratch1;
          g_UiDisplayModeDistinctValueScratch1 = uVar9;
        }
        uVar9 = uVar8;
        if (uVar8 < g_UiDisplayModeDistinctValueScratch2) {
          LOCK();
          UNLOCK();
          uVar9 = g_UiDisplayModeDistinctValueScratch2;
          g_UiDisplayModeDistinctValueScratch2 = uVar8;
        }
        if (uVar9 < g_UiDisplayModeDistinctValueScratch3) {
          LOCK();
          UNLOCK();
          g_UiDisplayModeDistinctValueScratch3 = uVar9;
        }
      }
      pGVar16 = pGVar16 + 1;
      GVar12 = GVar12 - 1;
    } while (GVar12 != 0);
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
    GVar12 = g_GraphicsDisplayModeCount;
    pGVar16 = g_GraphicsDisplayModes;
    do {
      uVar8 = pGVar16->width * 0x10000 + pGVar16->height;
      if ((((uVar8 != g_UiDisplayModeDistinctValueScratch0) &&
           (uVar8 != g_UiDisplayModeDistinctValueScratch1)) &&
          ((uVar8 != g_UiDisplayModeDistinctValueScratch2 &&
           ((uVar8 != g_UiDisplayModeDistinctValueScratch3 &&
            (uVar8 != g_UiDisplayModeDistinctValueScratch4)))))) &&
         ((uVar8 != g_UiDisplayModeDistinctValueScratch5 &&
          ((uVar8 != g_UiDisplayModeDistinctValueScratch6 &&
           (uVar8 != g_UiDisplayModeDistinctValueScratch7)))))) {
        uVar9 = uVar8;
        if (uVar8 < g_UiDisplayModeDistinctValueScratch0) {
          LOCK();
          UNLOCK();
          uVar9 = g_UiDisplayModeDistinctValueScratch0;
          g_UiDisplayModeDistinctValueScratch0 = uVar8;
        }
        uVar8 = uVar9;
        if (uVar9 < g_UiDisplayModeDistinctValueScratch1) {
          LOCK();
          UNLOCK();
          uVar8 = g_UiDisplayModeDistinctValueScratch1;
          g_UiDisplayModeDistinctValueScratch1 = uVar9;
        }
        uVar9 = uVar8;
        if (uVar8 < g_UiDisplayModeDistinctValueScratch2) {
          LOCK();
          UNLOCK();
          uVar9 = g_UiDisplayModeDistinctValueScratch2;
          g_UiDisplayModeDistinctValueScratch2 = uVar8;
        }
        uVar8 = uVar9;
        if (uVar9 < g_UiDisplayModeDistinctValueScratch3) {
          LOCK();
          UNLOCK();
          uVar8 = g_UiDisplayModeDistinctValueScratch3;
          g_UiDisplayModeDistinctValueScratch3 = uVar9;
        }
        uVar9 = uVar8;
        if (uVar8 < g_UiDisplayModeDistinctValueScratch4) {
          LOCK();
          UNLOCK();
          uVar9 = g_UiDisplayModeDistinctValueScratch4;
          g_UiDisplayModeDistinctValueScratch4 = uVar8;
        }
        uVar8 = uVar9;
        if (uVar9 < g_UiDisplayModeDistinctValueScratch5) {
          LOCK();
          UNLOCK();
          uVar8 = g_UiDisplayModeDistinctValueScratch5;
          g_UiDisplayModeDistinctValueScratch5 = uVar9;
        }
        uVar9 = uVar8;
        if (uVar8 < g_UiDisplayModeDistinctValueScratch6) {
          LOCK();
          UNLOCK();
          uVar9 = g_UiDisplayModeDistinctValueScratch6;
          g_UiDisplayModeDistinctValueScratch6 = uVar8;
        }
        if (uVar9 < g_UiDisplayModeDistinctValueScratch7) {
          LOCK();
          UNLOCK();
          g_UiDisplayModeDistinctValueScratch7 = uVar9;
        }
      }
      pGVar16 = pGVar16 + 1;
      GVar12 = GVar12 - 1;
    } while (GVar12 != 0);
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
    uVar8 = g_UiDisplayModeDistinctValueScratch5 & 0xffff;
    root[0x11].base.nodeFlags = g_UiDisplayModeDistinctValueScratch5 >> 0x10;
    root[0x11].base.layoutHeight = uVar8;
    pUVar13 = (UiRootNode *)(g_UiDisplayModeDistinctValueScratch6 & 0xffff);
    root[0x13].base.nextSibling = (UiNodeBase *)(g_UiDisplayModeDistinctValueScratch6 >> 0x10);
    root[0x12].previousRoot = pUVar13;
    pUVar14 = (UiNodeVtable *)(g_UiDisplayModeDistinctValueScratch7 & 0xffff);
    root[0x14].base.left = g_UiDisplayModeDistinctValueScratch7 >> 0x10;
    root[0x14].base.vtable = pUVar14;
    g_UiDisplayModeDistinctValueScratch0 = 0xffffffff;
    g_UiDisplayModeDistinctValueScratch1 = 0xffffffff;
    g_UiDisplayModeDistinctValueScratch2 = 0xffffffff;
    g_UiDisplayModeDistinctValueScratch3 = 0xffffffff;
    g_UiDisplayModeDistinctValueScratch4 = 0xffffffff;
    GVar12 = g_GraphicsDisplayModeCount;
    pGVar16 = g_GraphicsDisplayModes;
    do {
      uVar8 = pGVar16->adapterIndex;
      if ((((uVar8 != g_UiDisplayModeDistinctValueScratch0) &&
           (uVar8 != g_UiDisplayModeDistinctValueScratch1)) &&
          (uVar8 != g_UiDisplayModeDistinctValueScratch2)) &&
         ((uVar8 != g_UiDisplayModeDistinctValueScratch3 &&
          (uVar8 != g_UiDisplayModeDistinctValueScratch4)))) {
        uVar9 = uVar8;
        if (uVar8 < g_UiDisplayModeDistinctValueScratch0) {
          LOCK();
          UNLOCK();
          uVar9 = g_UiDisplayModeDistinctValueScratch0;
          g_UiDisplayModeDistinctValueScratch0 = uVar8;
        }
        uVar8 = uVar9;
        if (uVar9 < g_UiDisplayModeDistinctValueScratch1) {
          LOCK();
          UNLOCK();
          uVar8 = g_UiDisplayModeDistinctValueScratch1;
          g_UiDisplayModeDistinctValueScratch1 = uVar9;
        }
        uVar9 = uVar8;
        if (uVar8 < g_UiDisplayModeDistinctValueScratch2) {
          LOCK();
          UNLOCK();
          uVar9 = g_UiDisplayModeDistinctValueScratch2;
          g_UiDisplayModeDistinctValueScratch2 = uVar8;
        }
        uVar8 = uVar9;
        if (uVar9 < g_UiDisplayModeDistinctValueScratch3) {
          LOCK();
          UNLOCK();
          uVar8 = g_UiDisplayModeDistinctValueScratch3;
          g_UiDisplayModeDistinctValueScratch3 = uVar9;
        }
        if (uVar8 < g_UiDisplayModeDistinctValueScratch4) {
          LOCK();
          UNLOCK();
          g_UiDisplayModeDistinctValueScratch4 = uVar8;
        }
      }
      DVar2 = g_UiDisplayModeDistinctValueScratch2;
      DVar1 = g_UiDisplayModeDistinctValueScratch1;
      pGVar16 = pGVar16 + 1;
      GVar12 = GVar12 - 1;
    } while (GVar12 != 0);
    root[0x15].base.leftOffset = g_UiDisplayModeDistinctValueScratch0;
    root[0x16].base.leftAnchorQ31 = DVar1;
    root[0x17].base.layoutWidth = DVar2;
    DVar1 = g_UiDisplayModeDistinctValueScratch4;
    iVar10 = g_SoftwarePixelFormatConfig.redBitCount + g_SoftwarePixelFormatConfig.greenBitCount;
    root[0x18].callbacks = (UiRootCallbacks *)g_UiDisplayModeDistinctValueScratch3;
    root[0x1a].base.parent = (UiNodeBase *)DVar1;
    UiDisplayModeSelection_RefreshEnumeratedOptions
              (g_ActiveGraphicsAdapterIndex,
               (UiNodeBase *)(iVar10 + g_SoftwarePixelFormatConfig.blueBitCount),g_FramebufferHeight
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
  bool bVar1;
  UiSelectableStateFlags *stateFlagsField;
  
  hitNode = (UiImageControl *)0xffffffff;
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (((control->selectable).stateFlags & 0x40) == 0) {
      bVar1 = (*g_GraphicsTextureSourceTestOpaquePixel)
                        (pointerY,pointerX,(control->selectable).base.top,
                         (control->selectable).base.left,control->normalSubresource,
                         control->textureSource);
      if (bVar1) {
        return (UiNodeBase *)control;
      }
    }
    else {
      bVar1 = (*g_GraphicsTextureSourceTestOpaquePixel)
                        (pointerY,pointerX,(control->selectable).base.top,
                         (control->selectable).base.left,control->alternateSubresource,
                         control->textureSource);
      if (bVar1) {
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
  bool bVar1;
  UiNodeBase *local_20;
  
  bVar1 = GraphicsDisplayMode_IsEnumeratedCf
                    (adapterIndex,displaySettingsRoot[8].right,modeHeight,modeWidth);
  if (bVar1) {
    UiNodeList_SuppressActionId(0x201,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x201,displaySettingsRoot);
  }
  if (selectedModeValue == (UiNodeBase *)displaySettingsRoot[8].right) {
    local_20 = (UiNodeBase *)&displaySettingsRoot[8].leftOffset;
  }
  bVar1 = GraphicsDisplayMode_IsEnumeratedCf
                    (adapterIndex,displaySettingsRoot[9].topAnchorQ31,modeHeight,modeWidth);
  if (bVar1) {
    UiNodeList_SuppressActionId(0x202,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x202,displaySettingsRoot);
  }
  if (selectedModeValue == (UiNodeBase *)displaySettingsRoot[9].topAnchorQ31) {
    local_20 = (UiNodeBase *)&displaySettingsRoot[9].bottomAnchorQ31;
  }
  bVar1 = GraphicsDisplayMode_IsEnumeratedCf
                    (adapterIndex,(FrontendColorDepthBits)displaySettingsRoot[0xb].firstChild,
                     modeHeight,modeWidth);
  if (bVar1) {
    UiNodeList_SuppressActionId(0x203,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x203,displaySettingsRoot);
  }
  if (selectedModeValue == displaySettingsRoot[0xb].firstChild) {
    local_20 = (UiNodeBase *)&displaySettingsRoot[0xb].vtable;
  }
  bVar1 = GraphicsDisplayMode_IsEnumeratedCf
                    (adapterIndex,displaySettingsRoot[0xc].leftOffset,modeHeight,modeWidth);
  if (bVar1) {
    UiNodeList_SuppressActionId(0x204,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x204,displaySettingsRoot);
  }
  if (selectedModeValue == (UiNodeBase *)displaySettingsRoot[0xc].leftOffset) {
    local_20 = (UiNodeBase *)modeWidth;
  }
  UiSelectableGroup_SelectExclusive(4,local_20);
  bVar1 = GraphicsDisplayMode_IsEnumeratedCf
                    (adapterIndex,(FrontendColorDepthBits)selectedModeValue,
                     displaySettingsRoot[0xd].rightAnchorQ31,
                     displaySettingsRoot[0xd].bottomAnchorQ31);
  if (bVar1) {
    UiNodeList_SuppressActionId(0x205,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x205,displaySettingsRoot);
  }
  if ((modeWidth == displaySettingsRoot[0xd].bottomAnchorQ31) &&
     (modeHeight == displaySettingsRoot[0xd].rightAnchorQ31)) {
    local_20 = (UiNodeBase *)&displaySettingsRoot[0xd].layoutHeight;
  }
  bVar1 = GraphicsDisplayMode_IsEnumeratedCf
                    (adapterIndex,(FrontendColorDepthBits)selectedModeValue,
                     (FrontendDisplayDimensionPixels)displaySettingsRoot[0xf].parent,
                     (FrontendDisplayDimensionPixels)displaySettingsRoot[0xf].vtable);
  if (bVar1) {
    UiNodeList_SuppressActionId(0x206,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x206,displaySettingsRoot);
  }
  if (((UiNodeVtable *)modeWidth == displaySettingsRoot[0xf].vtable) &&
     ((UiNodeBase *)modeHeight == displaySettingsRoot[0xf].parent)) {
    local_20 = (UiNodeBase *)&displaySettingsRoot[0xf].top;
  }
  bVar1 = GraphicsDisplayMode_IsEnumeratedCf
                    (adapterIndex,(FrontendColorDepthBits)selectedModeValue,
                     displaySettingsRoot[0x10].topOffset,displaySettingsRoot[0x10].rightOffset);
  if (bVar1) {
    UiNodeList_SuppressActionId(0x207,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x207,displaySettingsRoot);
  }
  if ((modeWidth == displaySettingsRoot[0x10].rightOffset) &&
     (modeHeight == displaySettingsRoot[0x10].topOffset)) {
    local_20 = (UiNodeBase *)&displaySettingsRoot[0x10].leftAnchorQ31;
  }
  bVar1 = GraphicsDisplayMode_IsEnumeratedCf
                    (adapterIndex,(FrontendColorDepthBits)selectedModeValue,
                     displaySettingsRoot[0x11].layoutWidth,displaySettingsRoot[0x11].layoutHeight);
  if (bVar1) {
    UiNodeList_SuppressActionId(0x208,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x208,displaySettingsRoot);
  }
  if ((modeWidth == displaySettingsRoot[0x11].layoutHeight) &&
     (modeHeight == displaySettingsRoot[0x11].layoutWidth)) {
    local_20 = displaySettingsRoot + 0x12;
  }
  bVar1 = GraphicsDisplayMode_IsEnumeratedCf
                    (adapterIndex,(FrontendColorDepthBits)selectedModeValue,
                     displaySettingsRoot[0x13].left,displaySettingsRoot[0x13].top);
  if (bVar1) {
    UiNodeList_SuppressActionId(0x209,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x209,displaySettingsRoot);
  }
  if ((modeWidth == displaySettingsRoot[0x13].top) && (modeHeight == displaySettingsRoot[0x13].left)
     ) {
    local_20 = (UiNodeBase *)&displaySettingsRoot[0x13].bottom;
  }
  bVar1 = GraphicsDisplayMode_IsEnumeratedCf
                    (adapterIndex,(FrontendColorDepthBits)selectedModeValue,
                     displaySettingsRoot[0x14].bottomOffset,displaySettingsRoot[0x14].leftAnchorQ31)
  ;
  if (bVar1) {
    UiNodeList_SuppressActionId(0x20a,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x20a,displaySettingsRoot);
  }
  if ((modeWidth == displaySettingsRoot[0x14].leftAnchorQ31) &&
     (modeHeight == displaySettingsRoot[0x14].bottomOffset)) {
    local_20 = (UiNodeBase *)&displaySettingsRoot[0x14].rightAnchorQ31;
  }
  bVar1 = GraphicsDisplayMode_IsEnumeratedCf
                    (adapterIndex,(FrontendColorDepthBits)selectedModeValue,
                     displaySettingsRoot[0x15].nodeFlags,
                     (FrontendDisplayDimensionPixels)displaySettingsRoot[0x16].nextSibling);
  if (bVar1) {
    UiNodeList_SuppressActionId(0x20b,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x20b,displaySettingsRoot);
  }
  if (((UiNodeBase *)modeWidth == displaySettingsRoot[0x16].nextSibling) &&
     (modeHeight == displaySettingsRoot[0x15].nodeFlags)) {
    local_20 = (UiNodeBase *)&displaySettingsRoot[0x16].parent;
  }
  bVar1 = GraphicsDisplayMode_IsEnumeratedCf
                    (adapterIndex,(FrontendColorDepthBits)selectedModeValue,
                     displaySettingsRoot[0x17].right,displaySettingsRoot[0x17].bottom);
  if (bVar1) {
    UiNodeList_SuppressActionId(0x20c,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x20c,displaySettingsRoot);
  }
  if ((modeWidth == displaySettingsRoot[0x17].bottom) &&
     (modeHeight == displaySettingsRoot[0x17].right)) {
    local_20 = (UiNodeBase *)&displaySettingsRoot[0x17].topOffset;
  }
  UiSelectableGroup_SelectExclusive(8,local_20);
  bVar1 = GraphicsDisplayMode_IsEnumeratedCf
                    (displaySettingsRoot[0x18].rightAnchorQ31,
                     (FrontendColorDepthBits)selectedModeValue,modeHeight,modeWidth);
  if (bVar1) {
    UiNodeList_SuppressActionId(0x20f,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x20f,displaySettingsRoot);
  }
  if (adapterIndex == displaySettingsRoot[0x18].rightAnchorQ31) {
    local_20 = (UiNodeBase *)&displaySettingsRoot[0x18].layoutWidth;
  }
  bVar1 = GraphicsDisplayMode_IsEnumeratedCf
                    ((FrontendDisplayAdapterIndex)displaySettingsRoot[0x1a].parent,
                     (FrontendColorDepthBits)selectedModeValue,modeHeight,modeWidth);
  if (bVar1) {
    UiNodeList_SuppressActionId(0x210,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x210,displaySettingsRoot);
  }
  if ((UiNodeBase *)adapterIndex == displaySettingsRoot[0x1a].parent) {
    local_20 = (UiNodeBase *)&displaySettingsRoot[0x1a].left;
  }
  bVar1 = GraphicsDisplayMode_IsEnumeratedCf
                    (displaySettingsRoot[0x1b].topOffset,(FrontendColorDepthBits)selectedModeValue,
                     modeHeight,modeWidth);
  if (bVar1) {
    UiNodeList_SuppressActionId(0x211,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x211,displaySettingsRoot);
  }
  if (adapterIndex == displaySettingsRoot[0x1b].topOffset) {
    local_20 = (UiNodeBase *)&displaySettingsRoot[0x1b].bottomOffset;
  }
  bVar1 = GraphicsDisplayMode_IsEnumeratedCf
                    (displaySettingsRoot[0x1c].layoutWidth,(FrontendColorDepthBits)selectedModeValue
                     ,modeHeight,modeWidth);
  if (bVar1) {
    UiNodeList_SuppressActionId(0x212,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x212,displaySettingsRoot);
  }
  if (adapterIndex == displaySettingsRoot[0x1c].layoutWidth) {
    local_20 = (UiNodeBase *)&displaySettingsRoot[0x1c].nodeFlags;
  }
  bVar1 = GraphicsDisplayMode_IsEnumeratedCf
                    (displaySettingsRoot[0x1e].left,(FrontendColorDepthBits)selectedModeValue,
                     modeHeight,modeWidth);
  if (bVar1) {
    UiNodeList_SuppressActionId(0x213,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x213,displaySettingsRoot);
  }
  if (adapterIndex == displaySettingsRoot[0x1e].left) {
    local_20 = (UiNodeBase *)&displaySettingsRoot[0x1e].right;
  }
  UiSelectableGroup_SelectExclusive(5,local_20);
  displaySettingsRoot[4].nextSibling = (UiNodeBase *)modeWidth;
  displaySettingsRoot[4].firstChild = (UiNodeBase *)modeHeight;
  displaySettingsRoot[4].parent = selectedModeValue;
  displaySettingsRoot[4].vtable = (UiNodeVtable *)adapterIndex;
  if ((((modeWidth == displaySettingsRoot[4].right) && (modeHeight == displaySettingsRoot[4].bottom)
       ) && (selectedModeValue == (UiNodeBase *)displaySettingsRoot[4].leftOffset)) &&
     (((adapterIndex == displaySettingsRoot[4].topOffset &&
       (displaySettingsRoot[4].left == displaySettingsRoot[4].rightOffset)) &&
      (displaySettingsRoot[4].top == displaySettingsRoot[4].bottomOffset)))) {
    UiNodeList_SuppressActionId(0x200,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x200,displaySettingsRoot);
  }
  return;
}

