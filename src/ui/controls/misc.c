#include <thandor/ui/controls/misc.h>

/* Implementation ownership: ui/controls/misc. */

/* Address: 0x00422910.
   Ownership: ui/controls/misc.
   Purpose: Binary entry is anchored by g_CodePointerTable_004229A0[1]@004229A0. UiRootCallbacks root callback with
   one stack argument.
   Local calls: UiDisplayModeSelection_RefreshEnumeratedOptions.
   Cross-module calls: UiRuntime_FormatSignedValues140And144 [ui/core/runtime].
*/
void UiDisplaySettingsRoot_RefreshModeSelection(UiRootNode *root)

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
void UiDisplayModeAction_UpdateAdapterSelection(UiNodeBase *sourceNode)

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
void UiDisplayModeAction_UpdateResolutionSelection(UiNodeBase *sourceNode)

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
void UiDisplayModeAction_UpdateColorDepthSelection(UiNodeBase *sourceNode)

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
void UiDisplayModeAction_ApplyFourValueDialogAndReopenSettings(UiNodeBase *sourceNode)

{
  longlong lVar1;
  UiRootNode *pUVar2;
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
  (*g_GraphicsDisplayModeHook)(arg0,arg1,arg2,arg3);
  (*g_FatalErrorPrimaryDispatchCf)();
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
void UiImageControl_NonRightDrag
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiImageControl *control)

{
  UiImageControl *control_00;
  UiNodeBase *newActiveChild;
  UiNodeBase *control_01;
  undefined8 childCallbackResultPair;
  UiSelectableStateFlags *stateFlagsField;
  
  if (((control->selectable).stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) == 0) {
    return;
  }
  control_00 = (UiImageControl *)UiImageControl_HitTestOpaque(pointerY,pointerX,control);
  if ((control_00 == control) ||
     (stateFlagsField = &(control->selectable).stateFlags,
     *stateFlagsField = *stateFlagsField & 0xfffffdff, control_00 == (UiImageControl *)0xffffffff))
  {
    control_01 = control->activeChild;
  }
  else {
    if (control_00 == (UiImageControl *)control->activeChild) {
      (*((control_00->selectable).base.vtable)->nonRightDrag)
                (wheelDelta,pointerY,pointerX,(UiNodeBase *)control_00);
      goto UiImageControl_InvalidateAfterNonRightDrag;
    }
    childCallbackResultPair =
         (*((control_00->selectable).base.vtable)->nonRightPress)
                   (0,0x70000000,0x70000000,(UiNodeBase *)control_00);
    newActiveChild =
         (UiNodeBase *)
         (**(code **)((int)((ulonglong)childCallbackResultPair >> 0x20) + 0x20))
                   (wheelDelta,pointerY,pointerX,(int)childCallbackResultPair);
    LOCK();
    control_01 = control->activeChild;
    control->activeChild = newActiveChild;
    UNLOCK();
  }
  if (control_01 != (UiNodeBase *)0x0) {
    childCallbackResultPair =
         (*control_01->vtable->nonRightDrag)(0,0x70000000,0x70000000,control_01);
    (**(code **)((int)((ulonglong)childCallbackResultPair >> 0x20) + 0x14))
              (0,0x70000000,0x70000000,(int)childCallbackResultPair);
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
void UiImageControl_TickHover(UiImageControl *control)

{
  UiSelectableStateFlags *pUVar1;
  UiImageControl *control_00;
  undefined4 pointerCallbackResult;
  int extraout_ECX;
  int extraout_ECX_00;
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
          hoveredControlVtable = (control_00->selectable).base.vtable;
          hoverStateFlagsField = &(control->selectable).stateFlags;
          *hoverStateFlagsField = *hoverStateFlagsField & 0xfffff7ff;
          pointerCallbackResult =
               (*hoveredControlVtable->nonRightRelease)
                         (g_CursorWheelDelta,g_CursorOverrideY,g_CursorOverrideX,
                          (UiNodeBase *)control_00);
          pointerCallbackResult =
               (**(code **)(extraout_ECX + 0x10))
                         (g_CursorWheelDelta,g_CursorOverrideY,g_CursorOverrideX,
                          pointerCallbackResult);
          (**(code **)(extraout_ECX_00 + 0x20))
                    (g_CursorWheelDelta,g_CursorOverrideY,g_CursorOverrideX,pointerCallbackResult);
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
void UiDisplayModeAction_ApplyPendingMode(UiNodeBase *sourceNode)

{
  uint arg1;
  uint arg0;
  UiRootNode *root;
  uint extraout_ECX;
  dword arg3;
  uint extraout_EDX;
  dword arg2;
  uint arg1_00;
  undefined1 uVar1;
  uint arg0_00;
  dword arg2_00;
  dword arg3_00;
  
  root = (UiRootNode *)UiNode_GetRoot(sourceNode);
  arg1 = root[3].base.leftAnchorQ31;
  arg1_00 = g_SoftwarePixelFormatConfig.redBitCount + g_SoftwarePixelFormatConfig.greenBitCount +
            g_SoftwarePixelFormatConfig.blueBitCount;
  arg0 = root[3].base.topAnchorQ31;
  UiRootStack_PopCf(root);
  uVar1 = extraout_ECX < g_FramebufferWidth;
  if ((((extraout_ECX != g_FramebufferWidth) ||
       (uVar1 = extraout_EDX < g_FramebufferHeight, extraout_EDX != g_FramebufferHeight)) ||
      (uVar1 = arg1 < arg1_00, arg1 != arg1_00)) ||
     (uVar1 = arg0 < g_ActiveGraphicsAdapterIndex, arg0 != g_ActiveGraphicsAdapterIndex)) {
    g_CursorVisibilityToken = g_CursorVisibilityToken + -1;
    UiFrame_ProcessAndPresentWithLockTransition();
    arg0_00 = g_ActiveGraphicsAdapterIndex;
    arg2_00 = g_FramebufferHeight;
    arg3_00 = g_FramebufferWidth;
    (*g_GraphicsDisplayModeHook)(arg0,arg1,arg2,arg3);
    if ((bool)uVar1) {
      (*g_GraphicsDisplayModeHook)(arg0_00,arg1_00,arg2_00,arg3_00);
      (*g_FatalErrorPrimaryDispatchCf)();
      g_CursorVisibilityToken = g_CursorVisibilityToken + 1;
      (*g_FatalErrorRuntimeDispatchCf)();
      return;
    }
    UiRootStack_Relayout();
    g_CursorVisibilityToken = g_CursorVisibilityToken + 1;
    UiRuntime_OpenFourValueDialogCf(arg0_00,arg1_00,arg2_00,arg3_00);
  }
  return;
}

/* Address: 0x00423C00.
   Ownership: ui/controls/misc.
   Purpose: Binary entry is anchored by g_CodePointerTable_00423588[14]@00423588. Display-mode selection action
   callback.
   Cross-module calls: UiNode_GetRoot [ui/core/runtime], UiRootStack_PopCf [ui/controls/layout].
*/
void UiDisplayModeAction_CancelAndRebuildPixelPacking(UiNodeBase *sourceNode)

{
  sdword arg1;
  sdword arg0;
  
  UiNode_GetRoot(sourceNode);
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
void UiFourValueDialog_TickCountdownAndRequestClose(UiRootNode *root)

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
void UiRangeSliderControl_DrawTrackAndThumb
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,UiNodeBase *control)

{
  UiNodeBase *pUVar1;
  ulonglong uVar2;
  uint uVar3;
  UiNodeBase *pUVar4;
  dword arg6;
  dword arg0;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  dword arg6_00;
  dword arg0_00;
  int extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  dword arg6_01;
  int extraout_EDX;
  int tileStart;
  int extraout_EDX_00;
  uint uVar5;
  undefined1 in_CF;
  qword qVar6;
  
  (*g_GraphicsFramebufferBeginAccess)();
  if (!(bool)in_CF) {
    arg6 = 0xac;
    if ((control->nodeFlags & UI_NODE_SUPPRESSED) != 0) {
      arg6 = 0xb0;
    }
    if (((uint)control[1].nextSibling & 1) != 0) {
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,control->top,control->left,arg6 + 8,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      qVar6 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0_00,g_UiWindowTextureSource);
      tileStart = (int)(qVar6 >> 0x20);
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,extraout_ECX_02 + 1,
                 control->layoutHeight - tileStart,tileStart,0,control);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,extraout_EDX_00 + control->top,control->left,
                 extraout_ECX_03 + 1,g_UiWindowTextureSource,g_FramebufferAccess);
      qVar6 = (*g_GraphicsTextureSourceGetLogicalSize)(extraout_ECX_04 + 1,g_UiWindowTextureSource);
      pUVar1 = control[1].parent;
      pUVar4 = (UiNodeBase *)control[1].vtable;
      if ((int)pUVar1 < (int)control[1].vtable) {
        pUVar4 = pUVar1;
      }
      uVar3 = (int)pUVar1 - (int)control[1].firstChild;
      if (uVar3 == 0) {
        uVar3 = 1;
      }
      uVar5 = (int)pUVar4 - (int)control[1].firstChild;
      if ((int)uVar5 < 0) {
        uVar5 = 0;
      }
      if (((uint)control[1].nextSibling & 8) == 0) {
        uVar5 = uVar3 - uVar5;
      }
      uVar2 = (ulonglong)uVar5 * (ulonglong)(uint)(control->layoutHeight - (int)(qVar6 >> 0x20));
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,
                 (int)(uVar2 / uVar3) + (uint)(uVar3 < (uint)((int)(uVar2 % (ulonglong)uVar3) * 2))
                 + control->top,control->left,arg6_01,g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsFramebufferEndAccess)();
      return;
    }
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,control->top,control->left,arg6,
               g_UiWindowTextureSource,g_FramebufferAccess);
    qVar6 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0,g_UiWindowTextureSource);
    UiWindow_BlitTiledHorizontalEdge
              (clipTop,clipLeft,clipBottom,clipRight,extraout_ECX + 1,
               control->layoutWidth - (int)qVar6,0,(int)qVar6,control);
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,control->top,extraout_EDX + control->left,
               extraout_ECX_00 + 1,g_UiWindowTextureSource,g_FramebufferAccess);
    qVar6 = (*g_GraphicsTextureSourceGetLogicalSize)(extraout_ECX_01 + 1,g_UiWindowTextureSource);
    pUVar1 = control[1].parent;
    pUVar4 = (UiNodeBase *)control[1].vtable;
    if ((int)pUVar1 < (int)control[1].vtable) {
      pUVar4 = pUVar1;
    }
    uVar3 = (int)pUVar4 - (int)control[1].firstChild;
    if ((int)uVar3 < 0) {
      uVar3 = 0;
    }
    uVar5 = (int)pUVar1 - (int)control[1].firstChild;
    if (uVar5 == 0) {
      uVar5 = 1;
    }
    if (((uint)control[1].nextSibling & 8) != 0) {
      uVar3 = uVar5 - uVar3;
    }
    uVar2 = (ulonglong)uVar3 * (ulonglong)(uint)(control->layoutWidth - (int)qVar6);
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,control->top,
               (int)(uVar2 / uVar5) + (uint)(uVar5 < (uint)((int)(uVar2 % (ulonglong)uVar5) * 2)) +
               control->left,arg6_00,g_UiWindowTextureSource,g_FramebufferAccess);
    (*g_GraphicsFramebufferEndAccess)();
  }
  return;
}

/* Address: 0x004B41C0.
   Ownership: ui/controls/misc.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B3EF0[4]@004B3EF0.
*/
void UiRangeSliderControl_BeginThumbDrag
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiNodeBase *control)

{
  int extraout_ECX;
  qword qVar1;
  int extraout_EDX;
  
  if (((((control->nodeFlags & UI_NODE_SUPPRESSED) == 0) && (control->left <= pointerX)) &&
      (control->top <= pointerY)) &&
     ((pointerX - control->left < control->layoutWidth &&
      (pointerY - control->top < control->layoutHeight)))) {
    if (((uint)control[1].nextSibling & 1) == 0) {
      (*g_GraphicsTextureSourceGetLogicalSize)(0xaf,g_UiWindowTextureSource);
      if (extraout_EDX <= extraout_ECX) {
        return;
      }
      control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling | 2);
    }
    else {
      qVar1._0_4_ = (*g_GraphicsTextureSourceGetLogicalSize)(0xb7,g_UiWindowTextureSource);
      if ((int)(qword)qVar1 <= qVar1._4_4_) {
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
void UiRangeSliderControl_EndThumbDrag(void)

{
  int in_stack_00000010;
  
  *(uint *)(in_stack_00000010 + 0x4c) = *(uint *)(in_stack_00000010 + 0x4c) & 0xfffffffd;
  if ((((*(uint *)(in_stack_00000010 + 0x48) & 8) == 0) &&
      ((*(uint *)(in_stack_00000010 + 0x4c) & 4) != 0)) && (*(int *)(in_stack_00000010 + 100) != 0))
  {
    (*g_SoundPlayOneShot)
              (g_UiSoundGainQ15,g_UiSoundGainQ15,*(DirectSoundVoiceSet **)(in_stack_00000010 + 100))
    ;
  }
  return;
}

/* Address: 0x004B45F0.
   Ownership: ui/controls/misc.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B3EF0[14]@004B3EF0.
   Cross-module calls: UiKeyboardFocus_ReleaseNode [ui/controls/input], UiNode_InvalidateRoot [ui/core/runtime].
*/
void UiRangeSliderControl_SuppressIfActionId(UiActionId actionId,UiNodeBase *control)

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
void UiRangeSliderControl_UnsuppressIfActionId(UiActionId actionId,UiNodeBase *control)

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
void UiImageControl_DrawClipped
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
      bVar1 = false;
      (*g_GraphicsFramebufferBeginAccess)();
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
void UiImageControl_NonRightPress
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiImageControl *control)

{
  UiSelectableStateFlags *pUVar1;
  bool opaquePixelHit;
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
    opaquePixelHit = false;
    if (((control->selectable).stateFlags & 0x40) == 0) {
      (*g_GraphicsTextureSourceTestOpaquePixel)
                (pointerY,pointerX,(control->selectable).base.top,(control->selectable).base.left,
                 control->normalSubresource,control->textureSource);
    }
    else {
      (*g_GraphicsTextureSourceTestOpaquePixel)
                (pointerY,pointerX,(control->selectable).base.top,(control->selectable).base.left,
                 control->alternateSubresource,control->textureSource);
    }
    if (opaquePixelHit) {
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
void UiImageControl_NonRightRelease
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiImageControl *control)

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
void UiModelControl_RefreshStateTint(ModelRuntimeNode *control)

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
void UiHorizontalGaugeControl_UpdateRuntimeRangeAndDraw
               (int clipTop,int clipLeft,int clipBottom,int clipRight,UiNodeBase *control)

{
  UiTransferPayloadByteCount UVar1;
  UiNodeBase *pUVar2;
  UiNodeVtable *pUVar3;
  int iVar4;
  FrontendPlayerRuntimeRecord *pFVar5;
  
  pFVar5 = (FrontendPlayerRuntimeRecord *)g_UiTransferMailbox.receivedRemainingBytes;
  UVar1 = g_UiTransferMailbox.receivedByteCount;
  pUVar2 = (UiNodeBase *)g_UiTransferMailbox.outgoingByteCount;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    if (g_UiTransferMailbox.outgoingByteCount == 0) {
      return;
    }
    pFVar5 = g_FrontendPlayerRuntimeBlocks + 1;
    iVar4 = g_FrontendPlayerRuntimeBlockCount - 1;
    if (iVar4 == 0) {
      return;
    }
    control[1].firstChild = (UiNodeBase *)0x0;
    control[1].parent = pUVar2;
    do {
      if ((int)pFVar5->runtimeState70 < (int)pUVar2) {
        pUVar2 = (UiNodeBase *)pFVar5->runtimeState70;
      }
      pFVar5 = pFVar5 + 1;
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
    control[1].vtable = (UiNodeVtable *)pUVar2;
    pUVar3 = (UiNodeVtable *)0x0;
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
  UiHorizontalGaugeControl_DrawFrameFillAndLabel
            (pUVar3,pFVar5,clipTop,clipLeft,clipBottom,clipRight,control);
  return;
}

/* Address: 0x00423600.
   Ownership: ui/controls/misc.
   Purpose: Opens and populates the display-mode selection UI; CF carries success/failure.
   Local calls: UiDisplayModeSelection_RefreshEnumeratedOptions.
   Cross-module calls: UiRuntime_FormatSignedValues140And144 [ui/core/runtime], UiActionHandlers_SetPageCf
   [ui/core/runtime], UiRootStack_Push [ui/controls/layout], UiRootStack_InvalidateAll [ui/controls/layout].
*/
void __cdecl UiDisplaySettings_OpenAndPopulateModeSelectionCf(void)

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
  uint extraout_ECX;
  uint uVar9;
  UiNodeFlags UVar10;
  GraphicsDisplayModeCount GVar11;
  int iVar12;
  UiRootNode *pUVar13;
  UiNodeVtable *pUVar14;
  undefined4 *puVar15;
  GraphicsDisplayMode *pGVar16;
  bool bVar17;
  
  bVar17 = g_GraphicsDisplayModeCount == 0;
  if (1 < g_GraphicsDisplayModeCount) {
    root = (*g_MemoryApi.alloc)(0xbd4);
    if (bVar17) {
      return;
    }
    puVar15 = (undefined4 *)0x4229b4;
    pUVar13 = root;
    for (uVar9 = extraout_ECX >> 2; UVar5 = g_ActiveGraphicsAdapterIndex,
        dVar4 = g_FramebufferHeight, dVar3 = g_FramebufferWidth, uVar9 != 0; uVar9 = uVar9 - 1) {
      (pUVar13->base).nextSibling = (UiNodeBase *)*puVar15;
      puVar15 = puVar15 + 1;
      pUVar13 = (UiRootNode *)&(pUVar13->base).firstChild;
    }
    UVar10 = g_SoftwarePixelFormatConfig.redBitCount + g_SoftwarePixelFormatConfig.greenBitCount +
             g_SoftwarePixelFormatConfig.blueBitCount;
    root[3].base.rightOffset = g_FramebufferWidth;
    root[3].base.bottomOffset = dVar4;
    root[3].base.leftAnchorQ31 = UVar10;
    root[3].base.topAnchorQ31 = UVar5;
    root[3].base.layoutWidth = dVar3;
    root[3].base.layoutHeight = dVar4;
    root[3].base.nodeFlags = UVar10;
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
    GVar11 = g_GraphicsDisplayModeCount;
    pGVar16 = g_GraphicsDisplayModes;
    do {
      uVar9 = pGVar16->bitsPerPixel;
      if ((((uVar9 != g_UiDisplayModeDistinctValueScratch0) &&
           (uVar9 != g_UiDisplayModeDistinctValueScratch1)) &&
          (uVar9 != g_UiDisplayModeDistinctValueScratch2)) &&
         (uVar9 != g_UiDisplayModeDistinctValueScratch3)) {
        uVar8 = uVar9;
        if (uVar9 < g_UiDisplayModeDistinctValueScratch0) {
          LOCK();
          UNLOCK();
          uVar8 = g_UiDisplayModeDistinctValueScratch0;
          g_UiDisplayModeDistinctValueScratch0 = uVar9;
        }
        uVar9 = uVar8;
        if (uVar8 < g_UiDisplayModeDistinctValueScratch1) {
          LOCK();
          UNLOCK();
          uVar9 = g_UiDisplayModeDistinctValueScratch1;
          g_UiDisplayModeDistinctValueScratch1 = uVar8;
        }
        uVar8 = uVar9;
        if (uVar9 < g_UiDisplayModeDistinctValueScratch2) {
          LOCK();
          UNLOCK();
          uVar8 = g_UiDisplayModeDistinctValueScratch2;
          g_UiDisplayModeDistinctValueScratch2 = uVar9;
        }
        if (uVar8 < g_UiDisplayModeDistinctValueScratch3) {
          LOCK();
          UNLOCK();
          g_UiDisplayModeDistinctValueScratch3 = uVar8;
        }
      }
      pGVar16 = pGVar16 + 1;
      GVar11 = GVar11 - 1;
    } while (GVar11 != 0);
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
    GVar11 = g_GraphicsDisplayModeCount;
    pGVar16 = g_GraphicsDisplayModes;
    do {
      uVar9 = pGVar16->width * 0x10000 + pGVar16->height;
      if ((((uVar9 != g_UiDisplayModeDistinctValueScratch0) &&
           (uVar9 != g_UiDisplayModeDistinctValueScratch1)) &&
          ((uVar9 != g_UiDisplayModeDistinctValueScratch2 &&
           ((uVar9 != g_UiDisplayModeDistinctValueScratch3 &&
            (uVar9 != g_UiDisplayModeDistinctValueScratch4)))))) &&
         ((uVar9 != g_UiDisplayModeDistinctValueScratch5 &&
          ((uVar9 != g_UiDisplayModeDistinctValueScratch6 &&
           (uVar9 != g_UiDisplayModeDistinctValueScratch7)))))) {
        uVar8 = uVar9;
        if (uVar9 < g_UiDisplayModeDistinctValueScratch0) {
          LOCK();
          UNLOCK();
          uVar8 = g_UiDisplayModeDistinctValueScratch0;
          g_UiDisplayModeDistinctValueScratch0 = uVar9;
        }
        uVar9 = uVar8;
        if (uVar8 < g_UiDisplayModeDistinctValueScratch1) {
          LOCK();
          UNLOCK();
          uVar9 = g_UiDisplayModeDistinctValueScratch1;
          g_UiDisplayModeDistinctValueScratch1 = uVar8;
        }
        uVar8 = uVar9;
        if (uVar9 < g_UiDisplayModeDistinctValueScratch2) {
          LOCK();
          UNLOCK();
          uVar8 = g_UiDisplayModeDistinctValueScratch2;
          g_UiDisplayModeDistinctValueScratch2 = uVar9;
        }
        uVar9 = uVar8;
        if (uVar8 < g_UiDisplayModeDistinctValueScratch3) {
          LOCK();
          UNLOCK();
          uVar9 = g_UiDisplayModeDistinctValueScratch3;
          g_UiDisplayModeDistinctValueScratch3 = uVar8;
        }
        uVar8 = uVar9;
        if (uVar9 < g_UiDisplayModeDistinctValueScratch4) {
          LOCK();
          UNLOCK();
          uVar8 = g_UiDisplayModeDistinctValueScratch4;
          g_UiDisplayModeDistinctValueScratch4 = uVar9;
        }
        uVar9 = uVar8;
        if (uVar8 < g_UiDisplayModeDistinctValueScratch5) {
          LOCK();
          UNLOCK();
          uVar9 = g_UiDisplayModeDistinctValueScratch5;
          g_UiDisplayModeDistinctValueScratch5 = uVar8;
        }
        uVar8 = uVar9;
        if (uVar9 < g_UiDisplayModeDistinctValueScratch6) {
          LOCK();
          UNLOCK();
          uVar8 = g_UiDisplayModeDistinctValueScratch6;
          g_UiDisplayModeDistinctValueScratch6 = uVar9;
        }
        if (uVar8 < g_UiDisplayModeDistinctValueScratch7) {
          LOCK();
          UNLOCK();
          g_UiDisplayModeDistinctValueScratch7 = uVar8;
        }
      }
      pGVar16 = pGVar16 + 1;
      GVar11 = GVar11 - 1;
    } while (GVar11 != 0);
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
    uVar9 = g_UiDisplayModeDistinctValueScratch5 & 0xffff;
    root[0x11].base.nodeFlags = g_UiDisplayModeDistinctValueScratch5 >> 0x10;
    root[0x11].base.layoutHeight = uVar9;
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
    GVar11 = g_GraphicsDisplayModeCount;
    pGVar16 = g_GraphicsDisplayModes;
    do {
      uVar9 = pGVar16->adapterIndex;
      if ((((uVar9 != g_UiDisplayModeDistinctValueScratch0) &&
           (uVar9 != g_UiDisplayModeDistinctValueScratch1)) &&
          (uVar9 != g_UiDisplayModeDistinctValueScratch2)) &&
         ((uVar9 != g_UiDisplayModeDistinctValueScratch3 &&
          (uVar9 != g_UiDisplayModeDistinctValueScratch4)))) {
        uVar8 = uVar9;
        if (uVar9 < g_UiDisplayModeDistinctValueScratch0) {
          LOCK();
          UNLOCK();
          uVar8 = g_UiDisplayModeDistinctValueScratch0;
          g_UiDisplayModeDistinctValueScratch0 = uVar9;
        }
        uVar9 = uVar8;
        if (uVar8 < g_UiDisplayModeDistinctValueScratch1) {
          LOCK();
          UNLOCK();
          uVar9 = g_UiDisplayModeDistinctValueScratch1;
          g_UiDisplayModeDistinctValueScratch1 = uVar8;
        }
        uVar8 = uVar9;
        if (uVar9 < g_UiDisplayModeDistinctValueScratch2) {
          LOCK();
          UNLOCK();
          uVar8 = g_UiDisplayModeDistinctValueScratch2;
          g_UiDisplayModeDistinctValueScratch2 = uVar9;
        }
        uVar9 = uVar8;
        if (uVar8 < g_UiDisplayModeDistinctValueScratch3) {
          LOCK();
          UNLOCK();
          uVar9 = g_UiDisplayModeDistinctValueScratch3;
          g_UiDisplayModeDistinctValueScratch3 = uVar8;
        }
        if (uVar9 < g_UiDisplayModeDistinctValueScratch4) {
          LOCK();
          UNLOCK();
          g_UiDisplayModeDistinctValueScratch4 = uVar9;
        }
      }
      DVar2 = g_UiDisplayModeDistinctValueScratch2;
      DVar1 = g_UiDisplayModeDistinctValueScratch1;
      pGVar16 = pGVar16 + 1;
      GVar11 = GVar11 - 1;
    } while (GVar11 != 0);
    root[0x15].base.leftOffset = g_UiDisplayModeDistinctValueScratch0;
    root[0x16].base.leftAnchorQ31 = DVar1;
    root[0x17].base.layoutWidth = DVar2;
    DVar1 = g_UiDisplayModeDistinctValueScratch4;
    iVar12 = g_SoftwarePixelFormatConfig.redBitCount + g_SoftwarePixelFormatConfig.greenBitCount;
    root[0x18].callbacks = (UiRootCallbacks *)g_UiDisplayModeDistinctValueScratch3;
    root[0x1a].base.parent = (UiNodeBase *)DVar1;
    UiDisplayModeSelection_RefreshEnumeratedOptions
              (g_ActiveGraphicsAdapterIndex,
               (UiNodeBase *)(iVar12 + g_SoftwarePixelFormatConfig.blueBitCount),g_FramebufferHeight
               ,g_FramebufferWidth,&root->base);
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
UiNodeBase *
UiImageControl_HitTestOpaque
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiImageControl *control)

{
  UiImageControl *hitNode;
  bool opaquePixelHit;
  UiSelectableStateFlags *stateFlagsField;
  
  hitNode = (UiImageControl *)0xffffffff;
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    opaquePixelHit = false;
    if (((control->selectable).stateFlags & 0x40) == 0) {
      (*g_GraphicsTextureSourceTestOpaquePixel)
                (pointerY,pointerX,(control->selectable).base.top,(control->selectable).base.left,
                 control->normalSubresource,control->textureSource);
      if (opaquePixelHit) {
        return (UiNodeBase *)control;
      }
    }
    else {
      (*g_GraphicsTextureSourceTestOpaquePixel)
                (pointerY,pointerX,(control->selectable).base.top,(control->selectable).base.left,
                 control->alternateSubresource,control->textureSource);
      if (opaquePixelHit) {
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
void UiDisplayModeSelection_RefreshEnumeratedOptions
               (FrontendDisplayAdapterIndex adapterIndex,UiNodeBase *selectedModeValue,
               FrontendDisplayDimensionPixels modeHeight,FrontendDisplayDimensionPixels modeWidth,
               UiNodeBase *displaySettingsRoot)

{
  FrontendDisplayDimensionPixels extraout_EAX;
  FrontendDisplayDimensionPixels extraout_EAX_00;
  FrontendDisplayDimensionPixels extraout_EAX_01;
  FrontendDisplayDimensionPixels extraout_EAX_02;
  FrontendDisplayDimensionPixels extraout_EAX_03;
  FrontendDisplayDimensionPixels extraout_EAX_04;
  UiNodeBase *extraout_EAX_05;
  UiNodeBase *extraout_EAX_06;
  uint extraout_EAX_07;
  uint extraout_EAX_08;
  UiNodeVtable *extraout_EAX_09;
  UiNodeVtable *extraout_EAX_10;
  UiNodeVtable *pUVar1;
  uint extraout_EAX_11;
  uint extraout_EAX_12;
  uint extraout_EAX_13;
  uint extraout_EAX_14;
  uint extraout_EAX_15;
  uint extraout_EAX_16;
  uint extraout_EAX_17;
  uint extraout_EAX_18;
  uint uVar2;
  UiNodeBase *extraout_EAX_19;
  UiNodeBase *extraout_EAX_20;
  int extraout_EAX_21;
  int extraout_EAX_22;
  int iVar3;
  FrontendDisplayDimensionPixels FVar4;
  FrontendDisplayDimensionPixels extraout_EAX_23;
  FrontendDisplayDimensionPixels extraout_EAX_24;
  FrontendDisplayDimensionPixels extraout_EAX_25;
  FrontendDisplayDimensionPixels extraout_EAX_26;
  FrontendDisplayDimensionPixels extraout_EAX_27;
  FrontendDisplayDimensionPixels extraout_EAX_28;
  FrontendDisplayDimensionPixels extraout_EAX_29;
  FrontendDisplayDimensionPixels extraout_EAX_30;
  UiNodeBase *pUVar5;
  FrontendDisplayAdapterIndex extraout_ECX;
  FrontendDisplayAdapterIndex extraout_ECX_00;
  FrontendDisplayAdapterIndex extraout_ECX_01;
  FrontendDisplayAdapterIndex extraout_ECX_02;
  FrontendDisplayAdapterIndex extraout_ECX_03;
  FrontendDisplayAdapterIndex extraout_ECX_04;
  FrontendDisplayAdapterIndex adapterIndex_00;
  FrontendDisplayAdapterIndex extraout_ECX_05;
  FrontendDisplayAdapterIndex extraout_ECX_06;
  FrontendDisplayAdapterIndex extraout_ECX_07;
  FrontendDisplayAdapterIndex extraout_ECX_08;
  FrontendDisplayAdapterIndex extraout_ECX_09;
  FrontendDisplayAdapterIndex extraout_ECX_10;
  FrontendDisplayAdapterIndex extraout_ECX_11;
  FrontendDisplayAdapterIndex extraout_ECX_12;
  FrontendDisplayAdapterIndex extraout_ECX_13;
  FrontendDisplayAdapterIndex extraout_ECX_14;
  FrontendDisplayAdapterIndex extraout_ECX_15;
  FrontendDisplayAdapterIndex extraout_ECX_16;
  FrontendDisplayAdapterIndex extraout_ECX_17;
  FrontendDisplayAdapterIndex extraout_ECX_18;
  FrontendDisplayAdapterIndex FVar6;
  uint extraout_ECX_19;
  uint extraout_ECX_20;
  UiNodeBase *extraout_ECX_21;
  UiNodeBase *extraout_ECX_22;
  uint extraout_ECX_23;
  uint extraout_ECX_24;
  uint extraout_ECX_25;
  uint extraout_ECX_26;
  int extraout_ECX_27;
  int extraout_ECX_28;
  UiNodeVtable *extraout_ECX_29;
  FrontendDisplayDimensionPixels extraout_EDX;
  FrontendDisplayDimensionPixels extraout_EDX_00;
  FrontendDisplayDimensionPixels extraout_EDX_01;
  FrontendDisplayDimensionPixels extraout_EDX_02;
  FrontendDisplayDimensionPixels extraout_EDX_03;
  FrontendDisplayDimensionPixels extraout_EDX_04;
  uint extraout_EDX_05;
  uint extraout_EDX_06;
  UiNodeBase *extraout_EDX_07;
  UiNodeBase *extraout_EDX_08;
  uint extraout_EDX_09;
  uint extraout_EDX_10;
  uint extraout_EDX_11;
  uint extraout_EDX_12;
  uint extraout_EDX_13;
  uint extraout_EDX_14;
  uint extraout_EDX_15;
  uint extraout_EDX_16;
  uint uVar7;
  UiNodeFlags extraout_EDX_17;
  UiNodeFlags extraout_EDX_18;
  UiNodeFlags UVar8;
  int extraout_EDX_19;
  int extraout_EDX_20;
  int iVar9;
  FrontendDisplayDimensionPixels height;
  FrontendDisplayDimensionPixels extraout_EDX_21;
  FrontendDisplayDimensionPixels extraout_EDX_22;
  FrontendDisplayDimensionPixels extraout_EDX_23;
  FrontendDisplayDimensionPixels extraout_EDX_24;
  FrontendDisplayDimensionPixels extraout_EDX_25;
  FrontendDisplayDimensionPixels extraout_EDX_26;
  FrontendDisplayDimensionPixels extraout_EDX_27;
  FrontendDisplayDimensionPixels extraout_EDX_28;
  FrontendDisplayDimensionPixels FVar10;
  UiNodeBase *extraout_EDX_29;
  bool bVar11;
  UiNodeBase *local_20;
  
  bVar11 = &stack0xffffffe4 < (undefined1 *)0x4;
  GraphicsDisplayMode_IsEnumeratedCf(adapterIndex,displaySettingsRoot[8].right,modeHeight,modeWidth)
  ;
  if (bVar11) {
    UiNodeList_SuppressActionId(0x201,displaySettingsRoot);
    FVar4 = extraout_EAX;
    FVar6 = extraout_ECX;
    FVar10 = extraout_EDX;
  }
  else {
    UiNodeList_UnsuppressActionId(0x201,displaySettingsRoot);
    FVar4 = extraout_EAX_00;
    FVar6 = extraout_ECX_00;
    FVar10 = extraout_EDX_00;
  }
  bVar11 = selectedModeValue < (UiNodeBase *)displaySettingsRoot[8].right;
  if (selectedModeValue == (UiNodeBase *)displaySettingsRoot[8].right) {
    local_20 = (UiNodeBase *)&displaySettingsRoot[8].leftOffset;
  }
  GraphicsDisplayMode_IsEnumeratedCf(FVar6,displaySettingsRoot[9].topAnchorQ31,FVar10,FVar4);
  if (bVar11) {
    UiNodeList_SuppressActionId(0x202,displaySettingsRoot);
    FVar4 = extraout_EAX_01;
    FVar6 = extraout_ECX_01;
    FVar10 = extraout_EDX_01;
  }
  else {
    UiNodeList_UnsuppressActionId(0x202,displaySettingsRoot);
    FVar4 = extraout_EAX_02;
    FVar6 = extraout_ECX_02;
    FVar10 = extraout_EDX_02;
  }
  bVar11 = selectedModeValue < (UiNodeBase *)displaySettingsRoot[9].topAnchorQ31;
  if (selectedModeValue == (UiNodeBase *)displaySettingsRoot[9].topAnchorQ31) {
    local_20 = (UiNodeBase *)&displaySettingsRoot[9].bottomAnchorQ31;
  }
  GraphicsDisplayMode_IsEnumeratedCf
            (FVar6,(FrontendColorDepthBits)displaySettingsRoot[0xb].firstChild,FVar10,FVar4);
  if (bVar11) {
    UiNodeList_SuppressActionId(0x203,displaySettingsRoot);
    FVar4 = extraout_EAX_03;
    FVar6 = extraout_ECX_03;
    FVar10 = extraout_EDX_03;
  }
  else {
    UiNodeList_UnsuppressActionId(0x203,displaySettingsRoot);
    FVar4 = extraout_EAX_04;
    FVar6 = extraout_ECX_04;
    FVar10 = extraout_EDX_04;
  }
  bVar11 = selectedModeValue < displaySettingsRoot[0xb].firstChild;
  if (selectedModeValue == displaySettingsRoot[0xb].firstChild) {
    local_20 = (UiNodeBase *)&displaySettingsRoot[0xb].vtable;
  }
  GraphicsDisplayMode_IsEnumeratedCf(FVar6,displaySettingsRoot[0xc].leftOffset,FVar10,FVar4);
  if (bVar11) {
    UiNodeList_SuppressActionId(0x204,displaySettingsRoot);
    pUVar5 = extraout_EAX_05;
  }
  else {
    UiNodeList_UnsuppressActionId(0x204,displaySettingsRoot);
    pUVar5 = extraout_EAX_06;
  }
  if (selectedModeValue == (UiNodeBase *)displaySettingsRoot[0xc].leftOffset) {
    local_20 = pUVar5;
  }
  UiSelectableGroup_SelectExclusive(4,local_20);
  bVar11 = (undefined1 *)0xffffffef < &stack0xffffffd0;
  GraphicsDisplayMode_IsEnumeratedCf
            (adapterIndex_00,(FrontendColorDepthBits)selectedModeValue,
             displaySettingsRoot[0xd].rightAnchorQ31,displaySettingsRoot[0xd].bottomAnchorQ31);
  if (bVar11) {
    UiNodeList_SuppressActionId(0x205,displaySettingsRoot);
    uVar2 = extraout_EAX_07;
    FVar6 = extraout_ECX_05;
    uVar7 = extraout_EDX_05;
  }
  else {
    UiNodeList_UnsuppressActionId(0x205,displaySettingsRoot);
    uVar2 = extraout_EAX_08;
    FVar6 = extraout_ECX_06;
    uVar7 = extraout_EDX_06;
  }
  bVar11 = uVar2 < displaySettingsRoot[0xd].bottomAnchorQ31;
  if ((uVar2 == displaySettingsRoot[0xd].bottomAnchorQ31) &&
     (bVar11 = uVar7 < displaySettingsRoot[0xd].rightAnchorQ31,
     uVar7 == displaySettingsRoot[0xd].rightAnchorQ31)) {
    local_20 = (UiNodeBase *)&displaySettingsRoot[0xd].layoutHeight;
  }
  GraphicsDisplayMode_IsEnumeratedCf
            (FVar6,(FrontendColorDepthBits)selectedModeValue,
             (FrontendDisplayDimensionPixels)displaySettingsRoot[0xf].parent,
             (FrontendDisplayDimensionPixels)displaySettingsRoot[0xf].vtable);
  if (bVar11) {
    UiNodeList_SuppressActionId(0x206,displaySettingsRoot);
    pUVar1 = extraout_EAX_09;
    FVar6 = extraout_ECX_07;
    pUVar5 = extraout_EDX_07;
  }
  else {
    UiNodeList_UnsuppressActionId(0x206,displaySettingsRoot);
    pUVar1 = extraout_EAX_10;
    FVar6 = extraout_ECX_08;
    pUVar5 = extraout_EDX_08;
  }
  bVar11 = pUVar1 < displaySettingsRoot[0xf].vtable;
  if ((pUVar1 == displaySettingsRoot[0xf].vtable) &&
     (bVar11 = pUVar5 < displaySettingsRoot[0xf].parent, pUVar5 == displaySettingsRoot[0xf].parent))
  {
    local_20 = (UiNodeBase *)&displaySettingsRoot[0xf].top;
  }
  GraphicsDisplayMode_IsEnumeratedCf
            (FVar6,(FrontendColorDepthBits)selectedModeValue,displaySettingsRoot[0x10].topOffset,
             displaySettingsRoot[0x10].rightOffset);
  if (bVar11) {
    UiNodeList_SuppressActionId(0x207,displaySettingsRoot);
    uVar2 = extraout_EAX_11;
    FVar6 = extraout_ECX_09;
    uVar7 = extraout_EDX_09;
  }
  else {
    UiNodeList_UnsuppressActionId(0x207,displaySettingsRoot);
    uVar2 = extraout_EAX_12;
    FVar6 = extraout_ECX_10;
    uVar7 = extraout_EDX_10;
  }
  bVar11 = uVar2 < (uint)displaySettingsRoot[0x10].rightOffset;
  if ((uVar2 == displaySettingsRoot[0x10].rightOffset) &&
     (bVar11 = uVar7 < (uint)displaySettingsRoot[0x10].topOffset,
     uVar7 == displaySettingsRoot[0x10].topOffset)) {
    local_20 = (UiNodeBase *)&displaySettingsRoot[0x10].leftAnchorQ31;
  }
  GraphicsDisplayMode_IsEnumeratedCf
            (FVar6,(FrontendColorDepthBits)selectedModeValue,displaySettingsRoot[0x11].layoutWidth,
             displaySettingsRoot[0x11].layoutHeight);
  if (bVar11) {
    UiNodeList_SuppressActionId(0x208,displaySettingsRoot);
    uVar2 = extraout_EAX_13;
    FVar6 = extraout_ECX_11;
    uVar7 = extraout_EDX_11;
  }
  else {
    UiNodeList_UnsuppressActionId(0x208,displaySettingsRoot);
    uVar2 = extraout_EAX_14;
    FVar6 = extraout_ECX_12;
    uVar7 = extraout_EDX_12;
  }
  bVar11 = uVar2 < (uint)displaySettingsRoot[0x11].layoutHeight;
  if ((uVar2 == displaySettingsRoot[0x11].layoutHeight) &&
     (bVar11 = uVar7 < (uint)displaySettingsRoot[0x11].layoutWidth,
     uVar7 == displaySettingsRoot[0x11].layoutWidth)) {
    local_20 = displaySettingsRoot + 0x12;
  }
  GraphicsDisplayMode_IsEnumeratedCf
            (FVar6,(FrontendColorDepthBits)selectedModeValue,displaySettingsRoot[0x13].left,
             displaySettingsRoot[0x13].top);
  if (bVar11) {
    UiNodeList_SuppressActionId(0x209,displaySettingsRoot);
    uVar2 = extraout_EAX_15;
    FVar6 = extraout_ECX_13;
    uVar7 = extraout_EDX_13;
  }
  else {
    UiNodeList_UnsuppressActionId(0x209,displaySettingsRoot);
    uVar2 = extraout_EAX_16;
    FVar6 = extraout_ECX_14;
    uVar7 = extraout_EDX_14;
  }
  bVar11 = uVar2 < (uint)displaySettingsRoot[0x13].top;
  if ((uVar2 == displaySettingsRoot[0x13].top) &&
     (bVar11 = uVar7 < (uint)displaySettingsRoot[0x13].left, uVar7 == displaySettingsRoot[0x13].left
     )) {
    local_20 = (UiNodeBase *)&displaySettingsRoot[0x13].bottom;
  }
  GraphicsDisplayMode_IsEnumeratedCf
            (FVar6,(FrontendColorDepthBits)selectedModeValue,displaySettingsRoot[0x14].bottomOffset,
             displaySettingsRoot[0x14].leftAnchorQ31);
  if (bVar11) {
    UiNodeList_SuppressActionId(0x20a,displaySettingsRoot);
    uVar2 = extraout_EAX_17;
    FVar6 = extraout_ECX_15;
    uVar7 = extraout_EDX_15;
  }
  else {
    UiNodeList_UnsuppressActionId(0x20a,displaySettingsRoot);
    uVar2 = extraout_EAX_18;
    FVar6 = extraout_ECX_16;
    uVar7 = extraout_EDX_16;
  }
  bVar11 = uVar2 < displaySettingsRoot[0x14].leftAnchorQ31;
  if ((uVar2 == displaySettingsRoot[0x14].leftAnchorQ31) &&
     (bVar11 = uVar7 < (uint)displaySettingsRoot[0x14].bottomOffset,
     uVar7 == displaySettingsRoot[0x14].bottomOffset)) {
    local_20 = (UiNodeBase *)&displaySettingsRoot[0x14].rightAnchorQ31;
  }
  GraphicsDisplayMode_IsEnumeratedCf
            (FVar6,(FrontendColorDepthBits)selectedModeValue,displaySettingsRoot[0x15].nodeFlags,
             (FrontendDisplayDimensionPixels)displaySettingsRoot[0x16].nextSibling);
  if (bVar11) {
    UiNodeList_SuppressActionId(0x20b,displaySettingsRoot);
    pUVar5 = extraout_EAX_19;
    FVar6 = extraout_ECX_17;
    UVar8 = extraout_EDX_17;
  }
  else {
    UiNodeList_UnsuppressActionId(0x20b,displaySettingsRoot);
    pUVar5 = extraout_EAX_20;
    FVar6 = extraout_ECX_18;
    UVar8 = extraout_EDX_18;
  }
  bVar11 = pUVar5 < displaySettingsRoot[0x16].nextSibling;
  if ((pUVar5 == displaySettingsRoot[0x16].nextSibling) &&
     (bVar11 = UVar8 < displaySettingsRoot[0x15].nodeFlags,
     UVar8 == displaySettingsRoot[0x15].nodeFlags)) {
    local_20 = (UiNodeBase *)&displaySettingsRoot[0x16].parent;
  }
  GraphicsDisplayMode_IsEnumeratedCf
            (FVar6,(FrontendColorDepthBits)selectedModeValue,displaySettingsRoot[0x17].right,
             displaySettingsRoot[0x17].bottom);
  if (bVar11) {
    UiNodeList_SuppressActionId(0x20c,displaySettingsRoot);
    iVar3 = extraout_EAX_21;
    iVar9 = extraout_EDX_19;
  }
  else {
    UiNodeList_UnsuppressActionId(0x20c,displaySettingsRoot);
    iVar3 = extraout_EAX_22;
    iVar9 = extraout_EDX_20;
  }
  if ((iVar3 == displaySettingsRoot[0x17].bottom) && (iVar9 == displaySettingsRoot[0x17].right)) {
    local_20 = (UiNodeBase *)&displaySettingsRoot[0x17].topOffset;
  }
  FVar4 = UiSelectableGroup_SelectExclusive(8,local_20);
  bVar11 = (undefined1 *)0xffffffdf < &stack0xffffffc0;
  GraphicsDisplayMode_IsEnumeratedCf
            (displaySettingsRoot[0x18].rightAnchorQ31,(FrontendColorDepthBits)selectedModeValue,
             height,FVar4);
  if (bVar11) {
    UiNodeList_SuppressActionId(0x20f,displaySettingsRoot);
    FVar4 = extraout_EAX_23;
    uVar2 = extraout_ECX_19;
    FVar10 = extraout_EDX_21;
  }
  else {
    UiNodeList_UnsuppressActionId(0x20f,displaySettingsRoot);
    FVar4 = extraout_EAX_24;
    uVar2 = extraout_ECX_20;
    FVar10 = extraout_EDX_22;
  }
  bVar11 = uVar2 < displaySettingsRoot[0x18].rightAnchorQ31;
  if (uVar2 == displaySettingsRoot[0x18].rightAnchorQ31) {
    local_20 = (UiNodeBase *)&displaySettingsRoot[0x18].layoutWidth;
  }
  GraphicsDisplayMode_IsEnumeratedCf
            ((FrontendDisplayAdapterIndex)displaySettingsRoot[0x1a].parent,
             (FrontendColorDepthBits)selectedModeValue,FVar10,FVar4);
  if (bVar11) {
    UiNodeList_SuppressActionId(0x210,displaySettingsRoot);
    FVar4 = extraout_EAX_25;
    pUVar5 = extraout_ECX_21;
    FVar10 = extraout_EDX_23;
  }
  else {
    UiNodeList_UnsuppressActionId(0x210,displaySettingsRoot);
    FVar4 = extraout_EAX_26;
    pUVar5 = extraout_ECX_22;
    FVar10 = extraout_EDX_24;
  }
  bVar11 = pUVar5 < displaySettingsRoot[0x1a].parent;
  if (pUVar5 == displaySettingsRoot[0x1a].parent) {
    local_20 = (UiNodeBase *)&displaySettingsRoot[0x1a].left;
  }
  GraphicsDisplayMode_IsEnumeratedCf
            (displaySettingsRoot[0x1b].topOffset,(FrontendColorDepthBits)selectedModeValue,FVar10,
             FVar4);
  if (bVar11) {
    UiNodeList_SuppressActionId(0x211,displaySettingsRoot);
    FVar4 = extraout_EAX_27;
    uVar2 = extraout_ECX_23;
    FVar10 = extraout_EDX_25;
  }
  else {
    UiNodeList_UnsuppressActionId(0x211,displaySettingsRoot);
    FVar4 = extraout_EAX_28;
    uVar2 = extraout_ECX_24;
    FVar10 = extraout_EDX_26;
  }
  bVar11 = uVar2 < (uint)displaySettingsRoot[0x1b].topOffset;
  if (uVar2 == displaySettingsRoot[0x1b].topOffset) {
    local_20 = (UiNodeBase *)&displaySettingsRoot[0x1b].bottomOffset;
  }
  GraphicsDisplayMode_IsEnumeratedCf
            (displaySettingsRoot[0x1c].layoutWidth,(FrontendColorDepthBits)selectedModeValue,FVar10,
             FVar4);
  if (bVar11) {
    UiNodeList_SuppressActionId(0x212,displaySettingsRoot);
    FVar4 = extraout_EAX_29;
    uVar2 = extraout_ECX_25;
    FVar10 = extraout_EDX_27;
  }
  else {
    UiNodeList_UnsuppressActionId(0x212,displaySettingsRoot);
    FVar4 = extraout_EAX_30;
    uVar2 = extraout_ECX_26;
    FVar10 = extraout_EDX_28;
  }
  bVar11 = uVar2 < (uint)displaySettingsRoot[0x1c].layoutWidth;
  if (uVar2 == displaySettingsRoot[0x1c].layoutWidth) {
    local_20 = (UiNodeBase *)&displaySettingsRoot[0x1c].nodeFlags;
  }
  GraphicsDisplayMode_IsEnumeratedCf
            (displaySettingsRoot[0x1e].left,(FrontendColorDepthBits)selectedModeValue,FVar10,FVar4);
  if (bVar11) {
    UiNodeList_SuppressActionId(0x213,displaySettingsRoot);
    iVar3 = extraout_ECX_27;
  }
  else {
    UiNodeList_UnsuppressActionId(0x213,displaySettingsRoot);
    iVar3 = extraout_ECX_28;
  }
  if (iVar3 == displaySettingsRoot[0x1e].left) {
    local_20 = (UiNodeBase *)&displaySettingsRoot[0x1e].right;
  }
  pUVar5 = (UiNodeBase *)UiSelectableGroup_SelectExclusive(5,local_20);
  displaySettingsRoot[4].nextSibling = pUVar5;
  displaySettingsRoot[4].firstChild = extraout_EDX_29;
  displaySettingsRoot[4].parent = selectedModeValue;
  displaySettingsRoot[4].vtable = extraout_ECX_29;
  if ((((pUVar5 == (UiNodeBase *)displaySettingsRoot[4].right) &&
       (extraout_EDX_29 == (UiNodeBase *)displaySettingsRoot[4].bottom)) &&
      (selectedModeValue == (UiNodeBase *)displaySettingsRoot[4].leftOffset)) &&
     (((extraout_ECX_29 == (UiNodeVtable *)displaySettingsRoot[4].topOffset &&
       (displaySettingsRoot[4].left == displaySettingsRoot[4].rightOffset)) &&
      (displaySettingsRoot[4].top == displaySettingsRoot[4].bottomOffset)))) {
    UiNodeList_SuppressActionId(0x200,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(0x200,displaySettingsRoot);
  }
  return;
}
