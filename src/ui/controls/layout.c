#include <thandor/ui/controls/layout.h>

/* Implementation ownership: ui/controls/layout. */

/* Address: 0x004B49A0.
   Ownership: ui/controls/layout.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B4950[2]@004B4950.
   Local calls: UiWindow_BlitTiledInterior, UiWindow_BlitTiledHorizontalEdge, UiWindow_BlitTiledVerticalEdge,
   UiContainer_DrawIntersectingChildren.
*/
void UiPanelControl_DrawOptionalTiledBackgroundFrameAndChildren
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,UiNodeBase *control)

{
  GraphicsSubresourceIndex subresource;
  UiPixelCoordinate tileEnd;
  UiPixelCoordinate tileEnd_00;
  UiPixelCoordinate tileEnd_01;
  UiPixelCoordinate tileEnd_02;
  UiPixelCoordinate tileStart;
  UiPixelCoordinate tileStart_00;
  UiPixelCoordinate tileStart_01;
  UiPixelCoordinate tileStart_02;
  int iVar1;
  int iVar2;
  bool bVar3;
  qword qVar4;
  
  bVar3 = false;
  if (((uint)control[1].nextSibling & 3) != 0) {
    (*g_GraphicsFramebufferBeginAccess)();
    if (!bVar3) {
      if (((uint)control[1].nextSibling & 1) != 0) {
        subresource = 0;
        if (((uint)control[1].nextSibling & 0x200) != 0) {
          subresource = 0x2c;
        }
        UiWindow_BlitTiledInterior
                  (clipTop,clipLeft,clipBottom,clipRight,subresource,control->layoutHeight,
                   control->layoutWidth,0,0,control);
      }
      if (((uint)control[1].nextSibling & 2) != 0) {
        qVar4 = (*g_GraphicsTextureSourceGetLogicalSize)(0x13,g_UiWindowTextureSource);
        iVar2 = control->layoutWidth - (int)qVar4;
        iVar1 = control->layoutHeight - (int)(qVar4 >> 0x20);
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,control->top,control->left,0x10,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,control->top,iVar2 + control->left,0x11,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,iVar1 + control->top,control->left,0x12,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,iVar1 + control->top,iVar2 + control->left,
                   0x13,g_UiWindowTextureSource,g_FramebufferAccess);
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x14,iVar2,0,tileEnd,control);
        UiWindow_BlitTiledVerticalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x15,iVar1,tileStart,0,control);
        UiWindow_BlitTiledVerticalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x16,iVar1,tileStart_00,iVar2,control);
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x17,iVar2,iVar1,tileEnd_00,control);
      }
      if (((uint)control[1].nextSibling & 0x200) != 0) {
        qVar4 = (*g_GraphicsTextureSourceGetLogicalSize)(0x27,g_UiWindowTextureSource);
        iVar2 = control->layoutWidth - (int)qVar4;
        iVar1 = control->layoutHeight - (int)(qVar4 >> 0x20);
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,control->top,control->left,0x24,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,control->top,iVar2 + control->left,0x25,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,iVar1 + control->top,control->left,0x26,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,iVar1 + control->top,iVar2 + control->left,
                   0x27,g_UiWindowTextureSource,g_FramebufferAccess);
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x28,iVar2,0,tileEnd_01,control);
        UiWindow_BlitTiledVerticalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x29,iVar1,tileStart_01,0,control);
        UiWindow_BlitTiledVerticalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x2a,iVar1,tileStart_02,iVar2,control);
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x2b,iVar2,iVar1,tileEnd_02,control);
      }
      (*g_GraphicsFramebufferEndAccess)();
    }
  }
  UiContainer_DrawIntersectingChildren(clipTop,clipLeft,clipBottom,clipRight,control);
  return;
}

/* Address: 0x004B4D40.
   Ownership: ui/controls/layout.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B4CC0[2]@004B4CC0.
   Local calls: UiWindow_BlitTiledInterior, UiWindow_BlitTiledHorizontalEdge, UiWindow_BlitTiledVerticalEdge,
   UiContainer_DrawIntersectingChildren.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_DrawSingleLine
   [assets/text/richtext].
*/
void UiResizableWindowControl_DrawFrameTitleAndChildren
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,UiNodeBase *control)

{
  word *commandStream;
  UiPixelCoordinate tileEnd;
  UiPixelCoordinate tileEnd_00;
  dword dVar1;
  dword arg0;
  int extraout_ECX;
  dword arg6;
  int extraout_ECX_00;
  dword arg6_00;
  UiPixelCoordinate tileStart;
  UiPixelCoordinate tileStart_00;
  int iVar2;
  int tileStart_01;
  undefined1 in_CF;
  qword qVar3;
  qword qVar4;
  
  (*g_GraphicsFramebufferBeginAccess)();
  if (!(bool)in_CF) {
    if (((uint)control[1].nextSibling & 1) != 0) {
      UiWindow_BlitTiledInterior
                (clipTop,clipLeft,clipBottom,clipRight,0,control->layoutHeight,control->layoutWidth,
                 0,0,control);
    }
    if (((uint)control[1].nextSibling & 2) != 0) {
      qVar3 = (*g_GraphicsTextureSourceGetLogicalSize)(0x13,g_UiWindowTextureSource);
      tileStart_01 = control->layoutWidth - (int)qVar3;
      iVar2 = control->layoutHeight - (int)(qVar3 >> 0x20);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,control->top,control->left,0x10,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,control->top,tileStart_01 + control->left,
                 0x11,g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,iVar2 + control->top,control->left,0x12,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,iVar2 + control->top,
                 tileStart_01 + control->left,0x13,g_UiWindowTextureSource,g_FramebufferAccess);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x14,tileStart_01,0,tileEnd,control);
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x15,iVar2,tileStart,0,control);
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x16,iVar2,tileStart_00,tileStart_01,control)
      ;
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x17,tileStart_01,iVar2,tileEnd_00,control);
    }
    if (((uint)control[1].nextSibling & 4) != 0) {
      dVar1 = 10;
      if ((control->nodeFlags & 1) == 0) {
        dVar1 = 0xb;
      }
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,control->top,control->left,dVar1,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      qVar3 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0,g_UiWindowTextureSource);
      iVar2 = control->layoutWidth;
      qVar4 = (*g_GraphicsTextureSourceGetLogicalSize)(extraout_ECX + 4,g_UiWindowTextureSource);
      iVar2 = iVar2 - (int)qVar4;
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,control->top,iVar2 + control->left,arg6,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,extraout_ECX_00 - 2,iVar2,0,
                 (UiPixelCoordinate)qVar3,control);
      commandStream = TextResource_Resolve((TextResourceId)control[1].vtable);
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,g_UiResizableWindowTitleTextStyle,
                 commandStream,g_UiResizableWindowTitleTextTopOffset + control->top,
                 (control->layoutWidth >> 1) + control->left);
    }
    if (((uint)control[1].nextSibling & 8) != 0) {
      dVar1 = 1;
      if (((uint)control[1].nextSibling & 0x80000) != 0) {
        dVar1 = 3;
      }
      if ((control->nodeFlags & 1) == 0) {
        dVar1 = 2;
      }
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,control->top,control->left,dVar1,
                 g_UiWindowTextureSource,g_FramebufferAccess);
    }
    if (((uint)control[1].nextSibling & 0x10) != 0) {
      dVar1 = 4;
      if (((uint)control[1].nextSibling & 0x100000) != 0) {
        dVar1 = 6;
      }
      if ((control->nodeFlags & 1) == 0) {
        dVar1 = 5;
      }
      if (((uint)control[1].nextSibling & 0x80) != 0) {
        dVar1 = dVar1 + 3;
      }
      iVar2 = control->layoutWidth;
      qVar3 = (*g_GraphicsTextureSourceGetLogicalSize)(dVar1,g_UiWindowTextureSource);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,control->top,
                 (iVar2 - (int)qVar3) + control->left,arg6_00,g_UiWindowTextureSource,
                 g_FramebufferAccess);
    }
    (*g_GraphicsFramebufferEndAccess)();
  }
  UiContainer_DrawIntersectingChildren(clipTop,clipLeft,clipBottom,clipRight,control);
  return;
}

/* Address: 0x004B3420.
   Ownership: ui/controls/layout.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B33D0[2]@004B33D0.
   Local calls: UiWindow_BlitTiledVerticalEdge, UiWindow_BlitTiledHorizontalEdge,
   UiContainer_DrawIntersectingChildren.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_MeasureRegs
   [assets/text/richtext], RichTextCommandStream_DrawSingleLine [assets/text/richtext].
*/
void UiTitledWindowControl_DrawFrameTitleAndChildren
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,UiNodeBase *control)

{
  word *commandStream;
  int extraout_ECX;
  int extraout_ECX_00;
  UiPixelCoordinate edgeX;
  UiPixelCoordinate edgeX_00;
  UiPixelCoordinate edgeY;
  int extraout_ECX_01;
  int extraout_ECX_02;
  UiPixelCoordinate tileStart;
  int iVar1;
  int iVar2;
  undefined1 in_CF;
  qword qVar3;
  RichTextExtentRegs RVar4;
  int tileStart_00;
  
  (*g_GraphicsFramebufferBeginAccess)();
  if (!(bool)in_CF) {
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,control->top,control->left,0x36,
               g_UiWindowTextureSource,g_FramebufferAccess);
    iVar2 = control->layoutWidth;
    qVar3 = (*g_GraphicsTextureSourceGetLogicalSize)(0x39,g_UiWindowTextureSource);
    iVar1 = (int)qVar3;
    iVar2 = iVar2 - iVar1;
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,control->top,iVar2 + control->left,0x37,
               g_UiWindowTextureSource,g_FramebufferAccess);
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,extraout_ECX + control->top,control->left,0x38,
               g_UiWindowTextureSource,g_FramebufferAccess);
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,extraout_ECX_00 + control->top,
               iVar2 + control->left,0x39,g_UiWindowTextureSource,g_FramebufferAccess);
    qVar3 = (*g_GraphicsTextureSourceGetLogicalSize)(0x36,g_UiWindowTextureSource);
    tileStart_00 = iVar2;
    UiWindow_BlitTiledVerticalEdge
              (clipTop,clipLeft,clipBottom,clipRight,0x3b,edgeX,(UiPixelCoordinate)(qVar3 >> 0x20),0
               ,control);
    UiWindow_BlitTiledVerticalEdge
              (clipTop,clipLeft,clipBottom,clipRight,0x3c,edgeX_00,tileStart,iVar2,control);
    UiWindow_BlitTiledHorizontalEdge
              (clipTop,clipLeft,clipBottom,clipRight,0x3d,iVar2,edgeY,iVar1,control);
    commandStream = TextResource_Resolve((TextResourceId)control[1].firstChild);
    RichTextCommandStream_MeasureRegs(g_UiWindowTitleTextStyle,commandStream);
    if (((uint)control[1].nextSibling & 1) != 0) {
      qVar3 = (*g_GraphicsTextureSourceGetLogicalSize)(0x3e,g_UiWindowTextureSource);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x3a,extraout_ECX_01 - (int)qVar3,0,iVar1,
                 control);
      iVar1 = extraout_ECX_02;
    }
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,control->top,iVar1 + control->left,0x3e,
               g_UiWindowTextureSource,g_FramebufferAccess);
    qVar3 = (*g_GraphicsTextureSourceGetLogicalSize)(0x3e,g_UiWindowTextureSource);
    iVar1 = iVar1 + (int)qVar3;
    RichTextCommandStream_DrawSingleLine
              (clipTop,clipLeft,clipBottom,clipRight,g_UiWindowTitleTextStyle,commandStream,
               control->top,iVar1 + control->left);
    RVar4 = RichTextCommandStream_MeasureRegs(g_UiWindowTitleTextStyle,commandStream);
    iVar1 = iVar1 + RVar4.widthPixels;
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,control->top,iVar1 + control->left,0x3f,
               g_UiWindowTextureSource,g_FramebufferAccess);
    qVar3 = (*g_GraphicsTextureSourceGetLogicalSize)(0x3f,g_UiWindowTextureSource);
    UiWindow_BlitTiledHorizontalEdge
              (clipTop,clipLeft,clipBottom,clipRight,0x3a,tileStart_00,0,iVar1 + (int)qVar3,control)
    ;
    (*g_GraphicsFramebufferEndAccess)();
  }
  UiContainer_DrawIntersectingChildren(clipTop,clipLeft,clipBottom,clipRight,control);
  return;
}

/* Address: 0x004AF890.
   Ownership: ui/controls/layout.
   Purpose: Processes keyboard and pointer events, updates, dispatches actions, draws, and presents one UI frame
   while preserving the caller's lock-transition state through try-acquire/release/reacquire operations.
   Local calls: UiFrame_Update, UiFrame_Draw.
   Cross-module calls: UiKeyboard_DispatchPendingEvents [ui/controls/input], UiPointer_DispatchPendingEvents
   [ui/controls/input], UiActionQueue_DispatchPending [ui/core/runtime].
*/
void __cdecl UiFrame_ProcessAndPresentWithLockTransition(void)

{
  undefined1 in_CF;
  
  (*g_SpinLockTryAcquire)(g_UiRuntimeFrameLock);
  if (!(bool)in_CF) {
    (*g_SpinLockRelease)(g_UiRuntimeFrameLock);
    UiKeyboard_DispatchPendingEvents();
    UiPointer_DispatchPendingEvents();
    UiFrame_Update(0);
    UiActionQueue_DispatchPending();
    UiFrame_Draw();
    (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
    return;
  }
  (*g_SpinLockRelease)(g_UiRuntimeFrameLock);
  UiKeyboard_DispatchPendingEvents();
  UiPointer_DispatchPendingEvents();
  UiFrame_Update(0);
  UiActionQueue_DispatchPending();
  UiFrame_Draw();
  (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
  (*g_SpinLockAcquire)(g_UiRuntimeFrameLock);
  return;
}

/* Address: 0x004AF920.
   Ownership: ui/controls/layout.
   Purpose: Processes keyboard and pointer events, updates the active UI, dispatches queued actions, draws, and
   presents one complete frame without changing the external synchronization state.
   Local calls: UiFrame_Update, UiFrame_Draw.
   Cross-module calls: UiKeyboard_DispatchPendingEvents [ui/controls/input], UiPointer_DispatchPendingEvents
   [ui/controls/input], UiActionQueue_DispatchPending [ui/core/runtime].
*/
void __cdecl UiFrame_ProcessAndPresent(void)

{
  UiKeyboard_DispatchPendingEvents();
  UiPointer_DispatchPendingEvents();
  UiFrame_Update(0);
  UiActionQueue_DispatchPending();
  UiFrame_Draw();
  (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
  return;
}

/* Address: 0x004B48D0.
   Ownership: ui/controls/layout.
   Purpose: Selects a page by index when it is in range and differs from the active child. Deactivates the old
   subtree, activates the new subtree, and invalidates the stack root.
   Local calls: UiNodeSubtree_ReleaseKeyboardFocus, UiNodeSubtree_AcquireKeyboardFocusDefaults.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void UiPageStack_SetActiveIndex(UiPageIndex pageIndex,UiPageStackControl *stack)

{
  UiNodeBase *extraout_EDX;
  
  if ((pageIndex < stack->pageCount) && ((&stack->pages)[pageIndex] != (stack->base).firstChild)) {
    UiNodeSubtree_ReleaseKeyboardFocus(&stack->base);
    (stack->base).firstChild = extraout_EDX;
    UiNodeSubtree_AcquireKeyboardFocusDefaults(&stack->base);
    UiNode_InvalidateRoot(&stack->base);
  }
  return;
}

/* Address: 0x004B52D0.
   Ownership: ui/controls/layout.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B4CC0[5]@004B4CC0.
   Local calls: UiContainer_LayoutWithOptionalWindowHeaderOffset, UiRootStack_InvalidateAll.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime], UiNode_InvalidateRoot [ui/core/runtime].
*/
void __fastcall
UiResizableWindowControl_EndMoveResizeAndHandleWindowActions(undefined4 param_1,undefined4 param_2)

{
  sdword sVar1;
  sdword sVar2;
  sdword sVar3;
  dword dVar4;
  UiNodeBase *in_stack_00000010;
  
  if (((uint)in_stack_00000010[1].nextSibling & 0x2000) != 0) {
    (*g_GraphicsCursorSetFrame)(0);
  }
  if ((((uint)in_stack_00000010[1].nextSibling & 0x80000) != 0) &&
     ((in_stack_00000010->nodeFlags & UI_NODE_REPEAT_OR_DOUBLE_CLICK) != 0)) {
    UiActionQueue_Enqueue(0,in_stack_00000010);
  }
  if (((uint)in_stack_00000010[1].nextSibling & 0x100000) != 0) {
    in_stack_00000010[1].nextSibling = (UiNodeBase *)((uint)in_stack_00000010[1].nextSibling ^ 0x80)
    ;
    if (((uint)in_stack_00000010[1].nextSibling & 0x80) == 0) {
      sVar1 = in_stack_00000010[1].right;
      in_stack_00000010->left = in_stack_00000010[1].top;
      sVar2 = in_stack_00000010[1].bottom;
      sVar3 = in_stack_00000010[1].leftOffset;
      in_stack_00000010->top = sVar1;
      in_stack_00000010->bottom = sVar3;
      in_stack_00000010->right = sVar2;
      UiContainer_LayoutWithOptionalWindowHeaderOffset(in_stack_00000010);
      UiRootStack_InvalidateAll();
      goto UiResizableWindowControl_ClearInteractionStateAndReturn;
    }
    in_stack_00000010[1].top = in_stack_00000010->left;
    sVar1 = in_stack_00000010->right;
    sVar2 = in_stack_00000010->bottom;
    in_stack_00000010[1].right = in_stack_00000010->top;
    in_stack_00000010[1].leftOffset = sVar2;
    in_stack_00000010[1].bottom = sVar1;
    dVar4 = g_FramebufferHeight;
    in_stack_00000010->right = g_FramebufferWidth;
    in_stack_00000010->left = 0;
    in_stack_00000010->top = 0;
    in_stack_00000010->bottom = dVar4;
    UiContainer_LayoutWithOptionalWindowHeaderOffset(in_stack_00000010);
  }
  UiNode_InvalidateRoot(in_stack_00000010);
UiResizableWindowControl_ClearInteractionStateAndReturn:
  in_stack_00000010[1].nextSibling =
       (UiNodeBase *)((uint)in_stack_00000010[1].nextSibling & 0xe787ff);
  return;
}

/* Address: 0x004B5770.
   Ownership: ui/controls/layout.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B4CC0[12]@004B4CC0.
   Local calls: UiContainer_LayoutWithOptionalWindowHeaderOffset, UiRootStack_InvalidateAll.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime], UiActionQueue_Enqueue [ui/core/runtime],
   UiNode_DefaultKeyboardEventMoveFocusNextCf [ui/controls/input].
*/
void UiResizableWindowControl_HandleWindowHotkeysCf
               (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
               UiNodeBase *control)

{
  sdword sVar1;
  sdword sVar2;
  sdword sVar3;
  dword dVar4;
  
  if ((keyboardStateMask & 0x30) != 0) {
    if ((((uint)control[1].nextSibling & 8) == 0) || (keyCode != 99)) {
      if ((((uint)control[1].nextSibling & 0x10) == 0) || (keyCode != 0x7a))
      goto UiResizableWindowControl_DelegateUnhandledWindowHotkey;
      control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling ^ 0x80);
      if (((uint)control[1].nextSibling & 0x80) == 0) {
        sVar1 = control[1].right;
        control->left = control[1].top;
        sVar2 = control[1].bottom;
        sVar3 = control[1].leftOffset;
        control->top = sVar1;
        control->bottom = sVar3;
        control->right = sVar2;
        UiContainer_LayoutWithOptionalWindowHeaderOffset(control);
        UiRootStack_InvalidateAll();
      }
      else {
        control[1].top = control->left;
        sVar1 = control->right;
        sVar2 = control->bottom;
        control[1].right = control->top;
        control[1].leftOffset = sVar2;
        control[1].bottom = sVar1;
        dVar4 = g_FramebufferHeight;
        control->right = g_FramebufferWidth;
        control->left = 0;
        control->top = 0;
        control->bottom = dVar4;
        UiContainer_LayoutWithOptionalWindowHeaderOffset(control);
        UiNode_InvalidateRoot(control);
      }
    }
    else {
      UiActionQueue_Enqueue(0,control);
    }
    return;
  }
UiResizableWindowControl_DelegateUnhandledWindowHotkey:
  UiNode_DefaultKeyboardEventMoveFocusNextCf(keyboardStateMask,keyCode,control);
  return;
}

/* Address: 0x004B1000.
   Ownership: ui/controls/layout.
   Purpose: Resolves a serialized root rectangle, stores its callback table, relocates its tree, pushes it above
   the active root, suppresses the previous root, lays out and activates the new root, initializes focus, and
   clears capture/tooltip state.
   Local calls: UiSerializedTree_Relocate.
   Cross-module calls: UiKeyboardFocus_SelectInitial [ui/controls/input].
*/
void UiRootStack_Push(UiRootCallbacks *callbacks,UiRootNode *root)

{
  longlong lVar1;
  UiRootNode *control;
  longlong currentAnchorPixelProductQ31;
  longlong anchorPixelProductQ31;
  
  anchorPixelProductQ31 = (ulonglong)g_FramebufferWidth * (ulonglong)(root->base).rightAnchorQ31;
  (root->base).right =
       ((int)((ulonglong)anchorPixelProductQ31 >> 0x20) << 1 | (uint)anchorPixelProductQ31 >> 0x1f)
       + (root->base).rightOffset;
  currentAnchorPixelProductQ31 =
       (ulonglong)g_FramebufferHeight * (ulonglong)(root->base).bottomAnchorQ31;
  (root->base).bottom =
       ((int)((ulonglong)currentAnchorPixelProductQ31 >> 0x20) << 1 |
       (uint)currentAnchorPixelProductQ31 >> 0x1f) + (root->base).bottomOffset;
  lVar1 = (ulonglong)g_FramebufferWidth * (ulonglong)(root->base).leftAnchorQ31;
  (root->base).left =
       ((int)((ulonglong)lVar1 >> 0x20) << 1 | (uint)lVar1 >> 0x1f) + (root->base).leftOffset;
  lVar1 = (ulonglong)g_FramebufferHeight * (ulonglong)(root->base).topAnchorQ31;
  (root->base).top =
       ((int)((ulonglong)lVar1 >> 0x20) << 1 | (uint)lVar1 >> 0x1f) + (root->base).topOffset;
  root->callbacks = callbacks;
  (root->base).nextSibling = (UiNodeBase *)0xffffffff;
  UiSerializedTree_Relocate((SerializedImageRelocationDelta)root,&root->base);
  control = g_UiRootNode;
  LOCK();
  g_UiRootNode = root;
  UNLOCK();
  root->previousRoot = control;
  if (control != (UiRootNode *)0xffffffff) {
    (control->base).nextSibling = &root->base;
    (*((control->base).vtable)->applyFlags)(0,0xfffffffe,&control->base);
  }
  (*((root->base).vtable)->layout)(&root->base);
  (*((root->base).vtable)->applyFlags)(1,0xffffffff,&root->base);
  UiKeyboardFocus_SelectInitial(&root->base);
  g_UiPointerCaptureTarget = (UiNodeBase *)0xffffffff;
  g_UiPointerCaptureButton = UI_POINTER_CAPTURE_NONE;
  g_UiTooltipState.targetNode = (UiNodeBase *)0x0;
  return;
}

/* Address: 0x004B1110.
   Ownership: ui/controls/layout.
   Purpose: Finds the containing root and invokes callbacks->closeCf when present. CF set vetoes removal and
   invalidates the previous root; CF clear restores the previous root, focus, capture state, and redraw state.
   Local calls: UiRootStack_InvalidateAll.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime], UiKeyboardFocus_SelectInitial [ui/controls/input].
*/
void UiRootStack_PopCf(UiRootNode *root)

{
  undefined1 closeCallbackVetoed;
  UiRootNode *node;
  UiNodeBase *parentCursor;
  
  parentCursor = (root->base).parent;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    root = (UiRootNode *)(root->base).parent;
    parentCursor = (root->base).parent;
  }
  node = root->previousRoot;
  closeCallbackVetoed = false;
  if (root->callbacks->closeCf != (UiRootCloseCallbackCf *)0x0) {
    (*root->callbacks->closeCf)(root);
  }
  if ((bool)closeCallbackVetoed) {
    UiNode_InvalidateRoot(&node->base);
    return;
  }
  g_UiKeyboardFocusNode = (UiNodeBase *)0xffffffff;
  g_UiRootNode = node;
  if (node != (UiRootNode *)0xffffffff) {
    (node->base).nextSibling = (UiNodeBase *)0xffffffff;
    (*((node->base).vtable)->applyFlags)(1,0xffffffff,&node->base);
    UiKeyboardFocus_SelectInitial(&node->base);
  }
  g_UiPointerCaptureTarget = (UiNodeBase *)0xffffffff;
  g_UiPointerCaptureButton = UI_POINTER_CAPTURE_NONE;
  g_UiImageControlHoverTarget = (UiImageControl *)0x0;
  UiRootStack_InvalidateAll();
  return;
}

/* Address: 0x004B2790.
   Ownership: ui/controls/layout.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B2740[0]@004B2740.
   Local calls: UiContainer_RelocateChildren.
*/
void UiWindowControl_RelocateWithFrameInset
               (UiSerializedRelocationDelta relocationDelta,UiNodeBase *control)

{
  int frameInset;
  
  frameInset = g_UiWindowFrameInset;
  if (((uint)control[1].nextSibling & 4) != 0) {
    control->leftOffset = control->leftOffset - g_UiWindowFrameInset;
    control->topOffset = control->topOffset - frameInset;
    control->rightOffset = control->rightOffset + frameInset;
    control->bottomOffset = control->bottomOffset + frameInset;
  }
  UiContainer_RelocateChildren(relocationDelta,control);
  return;
}

/* Address: 0x004B36C0.
   Ownership: ui/controls/layout.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B33D0[3]@004B33D0.
   Local calls: UiContainer_LayoutChildren.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_MeasureRegs
   [assets/text/richtext].
*/
void UiTitledWindowControl_LayoutFrameTitleAndChildren(UiNodeBase *control)

{
  word *commandStream;
  int extraout_EAX;
  int extraout_EDX;
  qword qVar1;
  int iVar2;
  GraphicsTextureSourceAsset *pGVar3;
  int iVar4;
  
  commandStream = TextResource_Resolve((TextResourceId)control[1].firstChild);
  RichTextCommandStream_MeasureRegs(g_UiWindowTitleTextStyle,commandStream);
  iVar4 = 0x36;
  qVar1 = (*g_GraphicsTextureSourceGetLogicalSize)(0x36,g_UiWindowTextureSource);
  iVar2 = (int)(qVar1 >> 0x20);
  if (iVar2 < iVar4) {
    iVar2 = iVar4;
  }
  control->left = control->left + (int)qVar1;
  control->top = control->top + iVar2;
  iVar2 = 0x39;
  pGVar3 = g_UiWindowTextureSource;
  qVar1 = (*g_GraphicsTextureSourceGetLogicalSize)(0x39,g_UiWindowTextureSource);
  control->right = control->right - (int)qVar1;
  control->bottom = control->bottom - (int)(qVar1 >> 0x20);
  UiContainer_LayoutChildren(control);
  control->right = control->right + extraout_EAX;
  control->bottom = control->bottom + extraout_EDX;
  control->layoutWidth = control->layoutWidth + extraout_EAX;
  control->layoutHeight = control->layoutHeight + extraout_EDX;
  control->left = control->left - iVar2;
  control->top = control->top - (int)pGVar3;
  control->layoutWidth = control->layoutWidth + iVar2;
  control->layoutHeight =
       (sdword)((pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 +
               control->layoutHeight + -0x28);
  return;
}

/* Address: 0x004B3C00.
   Ownership: ui/controls/layout.
   Purpose: Calls the shared child hit-test routine for one container vtable. A result equal to the container
   itself is converted to the 0xFFFFFFFF no-hit sentinel.
   Local calls: UiContainer_HitTestChildren.
*/
UiNodeBase *
UiContainer_HitTestChildrenOrNoneA
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control)

{
  UiNodeBase *hitNode;
  
  hitNode = UiContainer_HitTestChildren(pointerY,pointerX,control);
  if (hitNode == control) {
    hitNode = (UiNodeBase *)0xffffffff;
  }
  return hitNode;
}

/* Address: 0x004B3C80.
   Ownership: ui/controls/layout.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B3C20[2]@004B3C20.
   Local calls: UiWindow_BlitTiledHorizontalEdge.
   Cross-module calls: RichTextCommandStream_DrawSingleLine [assets/text/richtext].
*/
undefined8 __fastcall
UiHorizontalGaugeControl_DrawFrameFillAndLabel
          (undefined4 param_1,undefined4 param_2,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,
          UiNodeBase *control)

{
  undefined4 in_EAX;
  int tileEnd;
  UiNodeBase *pUVar1;
  uint uVar2;
  int tileEnd_00;
  uint uVar3;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int tileStart;
  int extraout_ECX_02;
  uint uVar4;
  word *commandStream;
  undefined1 in_CF;
  qword qVar5;
  
  (*g_GraphicsFramebufferBeginAccess)();
  if (!(bool)in_CF) {
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,control->top,control->left,0x7c,
               g_UiWindowTextureSource,g_FramebufferAccess);
    qVar5 = (*g_GraphicsTextureSourceGetLogicalSize)(0x7c,g_UiWindowTextureSource);
    tileEnd = (int)qVar5;
    UiWindow_BlitTiledHorizontalEdge
              (clipTop,clipLeft,clipBottom,clipRight,0x7d,extraout_ECX - tileEnd,0,tileEnd,control);
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,control->top,extraout_ECX_00 + control->left,
               0x7e,g_UiWindowTextureSource,g_FramebufferAccess);
    pUVar1 = (UiNodeBase *)control[1].vtable;
    if (control[1].parent < pUVar1) {
      pUVar1 = control[1].parent;
    }
    uVar2 = (int)pUVar1 - (int)control[1].firstChild;
    uVar3 = 0;
    if (uVar2 != 0 && (int)control[1].firstChild <= (int)pUVar1) {
      qVar5 = (*g_GraphicsTextureSourceGetLogicalSize)(0x7f,g_UiWindowTextureSource);
      tileStart = extraout_ECX_01 - (int)qVar5;
      tileEnd_00 = (int)qVar5 + tileEnd;
      uVar3 = uVar2;
      if (tileEnd_00 <= tileStart) {
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x80,tileStart,0,tileEnd_00,control);
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,control->top,tileEnd + control->left,0x7f,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,control->top,
                   extraout_ECX_02 + control->left,0x81,g_UiWindowTextureSource,g_FramebufferAccess)
        ;
      }
    }
    if (((uint)control[1].nextSibling & 1) != 0) {
      uVar2 = (int)control[1].parent - (int)control[1].firstChild;
      if (uVar2 == 0) {
        uVar2 = 1;
      }
      uVar4 = (uint)(((ulonglong)uVar3 * 100) % (ulonglong)uVar2);
      uVar3 = (int)(((ulonglong)uVar3 * 100) / (ulonglong)uVar2) + (uint)CARRY4(uVar4,uVar4);
      if (uVar3 == 100) {
        g_UiWindowPercentTextUtf16[0] = 0x31;
        g_UiWindowPercentTextUtf16[1] = 0x30;
        g_UiWindowPercentTextUtf16[2] = 0x30;
        g_UiWindowPercentTextUtf16[3] = 0x25;
        g_UiWindowPercentTextUtf16[4] = 0;
      }
      else {
        g_UiWindowPercentTextUtf16[1] = (short)((ulonglong)uVar3 % 10) + 0x30;
        g_UiWindowPercentTextUtf16[0] = (short)((ulonglong)uVar3 / 10) + 0x30;
        g_UiWindowPercentTextUtf16[2] = 0x25;
        g_UiWindowPercentTextUtf16[3] = 0;
      }
      commandStream = g_UiWindowPercentTextUtf16;
      if (g_UiWindowPercentTextUtf16[0] == 0x30) {
        commandStream = g_UiWindowPercentTextUtf16 + 1;
      }
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,g_UiHorizontalGaugeLabelTextStyle,
                 commandStream,g_UiHorizontalGaugeLabelTopInset + control->top,
                 ((uint)control->layoutWidth >> 1) + control->left);
    }
    (*g_GraphicsFramebufferEndAccess)();
  }
  return CONCAT44(param_2,in_EAX);
}

/* Address: 0x004B46A0.
   Ownership: ui/controls/layout.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B4650[0]@004B4650.
   Local calls: UiContainer_RelocateChildren.
*/
void UiLayoutContainerControl_RelocateChildren
               (UiSerializedRelocationDelta relocationDelta,UiNodeBase *control)

{
  UiNodeBase *pUVar1;
  int extraout_ECX;
  UiNodeBase **ppUVar2;
  int extraout_EDX;
  UiNodeBase **childPointerCursor;
  
  ppUVar2 = &control[1].firstChild;
  pUVar1 = control[1].nextSibling;
  do {
    if (*ppUVar2 != (UiNodeBase *)0xffffffff) {
      *ppUVar2 = (UiNodeBase *)((int)&(*ppUVar2)->nextSibling + relocationDelta);
    }
    ppUVar2 = ppUVar2 + 1;
    pUVar1 = (UiNodeBase *)((int)&pUVar1[-1].nodeFlags + 3);
  } while (pUVar1 != (UiNodeBase *)0x0);
  childPointerCursor = &control[1].firstChild;
  do {
    control->firstChild = *childPointerCursor;
    UiContainer_RelocateChildren(relocationDelta,control);
    childPointerCursor = (UiNodeBase **)(extraout_EDX + 4);
  } while (extraout_ECX != 1);
  control->firstChild = control[1].firstChild;
  return;
}

/* Address: 0x004B4700.
   Ownership: ui/controls/layout.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B4650[3]@004B4650.
   Local calls: UiContainer_LayoutChildren.
*/
void UiLayoutContainerControl_LayoutChildren(UiNodeBase *control)

{
  int extraout_ECX;
  int extraout_EDX;
  UiNodeBase **childPointerCursor;
  UiNodeBase *savedFirstChild;
  
  childPointerCursor = &control[1].firstChild;
  savedFirstChild = control->firstChild;
  do {
    control->firstChild = *childPointerCursor;
    UiContainer_LayoutChildren(control);
    childPointerCursor = (UiNodeBase **)(extraout_EDX + 4);
  } while (extraout_ECX != 1);
  control->firstChild = savedFirstChild;
  return;
}

/* Address: 0x004B4790.
   Ownership: ui/controls/layout.
   Purpose: Calls the shared child hit-test routine for a second container vtable. A result equal to the container
   itself is converted to the 0xFFFFFFFF no-hit sentinel.
   Local calls: UiContainer_HitTestChildren.
*/
UiNodeBase *
UiContainer_HitTestChildrenOrNoneB
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control)

{
  UiNodeBase *hitNode;
  
  hitNode = UiContainer_HitTestChildren(pointerY,pointerX,control);
  if (hitNode == control) {
    hitNode = (UiNodeBase *)0xffffffff;
  }
  return hitNode;
}

/* Address: 0x004B47B0.
   Ownership: ui/controls/layout.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B4650[14]@004B4650.
   Local calls: UiContainer_SuppressActionId.
*/
void UiLayoutContainerControl_SuppressActionIdRecursive(UiActionId actionId,UiNodeBase *control)

{
  int extraout_ECX;
  int extraout_EDX;
  UiNodeBase **childPointerCursor;
  UiNodeBase *savedFirstChild;
  
  childPointerCursor = &control[1].firstChild;
  savedFirstChild = control->firstChild;
  do {
    control->firstChild = *childPointerCursor;
    UiContainer_SuppressActionId(actionId,control);
    childPointerCursor = (UiNodeBase **)(extraout_EDX + 4);
  } while (extraout_ECX != 1);
  control->firstChild = savedFirstChild;
  return;
}

/* Address: 0x004B4800.
   Ownership: ui/controls/layout.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B4650[15]@004B4650.
   Local calls: UiContainer_UnsuppressActionId.
*/
void UiLayoutContainerControl_UnsuppressActionIdRecursive(UiActionId actionId,UiNodeBase *control)

{
  int extraout_ECX;
  int extraout_EDX;
  UiNodeBase **childPointerCursor;
  UiNodeBase *savedFirstChild;
  
  childPointerCursor = &control[1].firstChild;
  savedFirstChild = control->firstChild;
  do {
    control->firstChild = *childPointerCursor;
    UiContainer_UnsuppressActionId(actionId,control);
    childPointerCursor = (UiNodeBase **)(extraout_EDX + 4);
  } while (extraout_ECX != 1);
  control->firstChild = savedFirstChild;
  return;
}

/* Address: 0x004B4D10.
   Ownership: ui/controls/layout.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B4CC0[0]@004B4CC0.
   Local calls: UiContainer_RelocateChildren.
*/
void UiResizableWindowControl_RelocateAndRefreshInteractionState
               (UiSerializedRelocationDelta relocationDelta,UiNodeBase *control)

{
  UiContainer_RelocateChildren(relocationDelta,control);
  if (((uint)control[1].nextSibling & 0x18) == 0) {
    control->nodeFlags =
         control->nodeFlags & ~(UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_PREFERRED_FOCUS_TARGET);
  }
  else {
    control->nodeFlags = control->nodeFlags | UI_NODE_FALLBACK_FOCUS_TARGET;
  }
  return;
}

/* Address: 0x004B53E0.
   Ownership: ui/controls/layout.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B4CC0[8]@004B4CC0.
   Local calls: UiRootStack_InvalidateAll.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void UiResizableWindowControl_UpdateMoveOrResize
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiNodeBase *control)

{
  UiNodeBase *pUVar1;
  sdword sVar2;
  sdword sVar3;
  int extraout_ECX;
  qword qVar9;
  int iVar4;
  int iVar5;
  int extraout_EDX;
  int iVar6;
  int iVar7;
  bool bVar8;
  
  iVar4 = pointerX - control->left;
  iVar6 = pointerY - control->top;
  if (((uint)control[1].nextSibling & 0x2000) == 0) {
    bVar8 = false;
    if (((uint)control[1].nextSibling & 0x800) == 0) {
      if (((uint)control[1].nextSibling & 0x1000) == 0) {
        if (((uint)control[1].nextSibling & 0x4000) != 0) {
          pUVar1 = control[1].nextSibling;
          iVar4 = iVar4 + control->left;
          iVar6 = iVar6 + control->top;
          iVar7 = iVar6;
          if (((uint)pUVar1 & 0x83000000) == 0) {
            iVar7 = control->top;
          }
          if (((uint)pUVar1 & 0x38000000) == 0) {
            iVar6 = control->bottom;
          }
          iVar5 = iVar4;
          if (((uint)pUVar1 & 0xe0000000) == 0) {
            iVar5 = control->left;
          }
          if (((uint)pUVar1 & 0xe000000) == 0) {
            iVar4 = control->right;
          }
          control[1].topOffset = iVar4;
          control[1].rightOffset = iVar6;
          iVar4 = (iVar4 - iVar5) + -0x40;
          if (iVar4 < 0) {
            if (((uint)pUVar1 & 0xe000000) != 0) {
              control[1].topOffset = control[1].topOffset - iVar4;
            }
            if (((uint)pUVar1 & 0xe0000000) != 0) {
              iVar5 = iVar5 + iVar4;
            }
          }
          iVar4 = (iVar6 - iVar7) + -0x40;
          if (iVar4 < 0) {
            if (((uint)pUVar1 & 0x38000000) != 0) {
              control[1].rightOffset = control[1].rightOffset - iVar4;
            }
            if (((uint)pUVar1 & 0x83000000) != 0) {
              iVar7 = iVar7 + iVar4;
            }
          }
          sVar2 = control[1].topOffset;
          sVar3 = control[1].rightOffset;
          LOCK();
          iVar4 = control->left;
          control->left = iVar5;
          UNLOCK();
          LOCK();
          iVar6 = control->top;
          control->top = iVar7;
          UNLOCK();
          LOCK();
          iVar7 = control->right;
          control->right = sVar2;
          UNLOCK();
          LOCK();
          iVar5 = control->bottom;
          control->bottom = sVar3;
          UNLOCK();
          if ((((iVar4 != control->left) || (iVar6 != control->top)) || (iVar7 != control->right))
             || (iVar5 != control->bottom)) {
            UiRootStack_InvalidateAll();
            (*control->vtable->layout)(control);
            UiNode_InvalidateRoot(control);
          }
        }
      }
      else {
        qVar9._0_4_ = (*g_GraphicsTextureSourceGetLogicalSize)(4,g_UiWindowTextureSource);
        bVar8 = (uint)qVar9._4_4_ < (uint)(qword)qVar9;
        (*g_GraphicsTextureSourceTestOpaquePixel)
                  (iVar6,iVar4,0,(int)qVar9._4_4_ - (int)(qword)qVar9,4,g_UiWindowTextureSource);
        if (bVar8) {
          if (((uint)control[1].nextSibling & 0x100000) != 0) {
            return;
          }
          control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling | 0x100000);
        }
        else {
          if (((uint)control[1].nextSibling & 0x100000) == 0) {
            return;
          }
          control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling & 0xffefffff);
        }
        UiNode_InvalidateRoot(control);
      }
    }
    else {
      (*g_GraphicsTextureSourceTestOpaquePixel)(iVar6,iVar4,0,0,1,g_UiWindowTextureSource);
      if (bVar8) {
        if (((uint)control[1].nextSibling & 0x80000) != 0) {
          return;
        }
        control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling | 0x80000);
      }
      else {
        if (((uint)control[1].nextSibling & 0x80000) == 0) {
          return;
        }
        control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling & 0xfff7ffff);
      }
      (*g_GraphicsTextureSourceGetLogicalSize)(1,g_UiWindowTextureSource);
      UiNode_InvalidateRoot(control);
    }
  }
  else if ((iVar6 != control[1].rightOffset) || (iVar4 != control[1].topOffset)) {
    UiRootStack_InvalidateAll();
    control->left = control->left + extraout_ECX;
    control->top = control->top + extraout_EDX;
    control->right = control->right + extraout_ECX;
    control->bottom = control->bottom + extraout_EDX;
    (*control->vtable->layout)(control);
    UiNode_InvalidateRoot(control);
  }
  return;
}

/* Address: 0x004BC660.
   Ownership: ui/controls/layout.
   Purpose: Temporarily substitutes the parent rectangle while laying out children, restores the control rectangle,
   and stores the parent width and height as the resolved layout size.
   Local calls: UiContainer_LayoutChildren.
*/
void UiImageControl_LayoutChildrenToParent(UiImageControl *control)

{
  sdword *psVar1;
  int iVar2;
  int iVar3;
  UiNodeBase *pUVar4;
  sdword sVar5;
  sdword sVar6;
  sdword sVar7;
  int extraout_ECX;
  int extraout_EDX;
  
  pUVar4 = (control->selectable).base.parent;
  sVar5 = pUVar4->top;
  sVar6 = pUVar4->right;
  sVar7 = pUVar4->bottom;
  LOCK();
  (control->selectable).base.left = pUVar4->left;
  UNLOCK();
  LOCK();
  (control->selectable).base.top = sVar5;
  UNLOCK();
  LOCK();
  psVar1 = &(control->selectable).base.right;
  iVar2 = *psVar1;
  *psVar1 = sVar6;
  UNLOCK();
  LOCK();
  psVar1 = &(control->selectable).base.bottom;
  iVar3 = *psVar1;
  *psVar1 = sVar7;
  UNLOCK();
  UiContainer_LayoutChildren((UiNodeBase *)control);
  (control->selectable).base.left = extraout_ECX;
  (control->selectable).base.top = extraout_EDX;
  (control->selectable).base.right = iVar2;
  (control->selectable).base.bottom = iVar3;
  (control->selectable).base.layoutWidth = iVar2 - extraout_ECX;
  (control->selectable).base.layoutHeight = iVar3 - extraout_EDX;
  return;
}

/* Address: 0x004AF3B0.
   Ownership: ui/controls/layout.
   Purpose: Flushes keyboard and pointer input through their installed service slots, then clears the pending UI
   frame-tick counter.
*/
void __cdecl UiFrame_FlushInputAndResetPendingTicks(void)

{
  (*g_KeyboardFlushEvents)();
  (*g_PointerFlushEvents)();
  g_UiPendingFrameTicks = 0;
  return;
}

/* Address: 0x004AF9D0.
   Ownership: ui/controls/layout.
   Purpose: Unlinks a root from its current position, inserts it above the active root, refreshes focus and
   active/suppressed flags, and invalidates the previous and promoted roots.
   Cross-module calls: UiKeyboardFocus_SelectInitial [ui/controls/input], UiNode_InvalidateRoot [ui/core/runtime].
*/
void UiRootStack_BringToFront(UiRootNode *root)

{
  UiRootNode *pUVar1;
  UiRootNode *control;
  UiNodeBase *nextRootLink;
  UiRootNode *detachedPreviousRoot;
  UiNodeBase *nextFrontRootLink;
  
  control = g_UiRootNode;
  nextRootLink = (root->base).nextSibling;
  pUVar1 = root->previousRoot;
  if (nextRootLink != (UiNodeBase *)0xffffffff) {
    nextRootLink[1].parent = &pUVar1->base;
  }
  if (pUVar1 != (UiRootNode *)0xffffffff) {
    (pUVar1->base).nextSibling = nextRootLink;
  }
  nextFrontRootLink = (g_UiRootNode->base).nextSibling;
  root->previousRoot = g_UiRootNode;
  (root->base).nextSibling = nextFrontRootLink;
  (g_UiRootNode->base).nextSibling = &root->base;
  g_UiRootNode = root;
  UiKeyboardFocus_SelectInitial(&root->base);
  (*((control->base).vtable)->applyFlags)(0,0xfffffffe,&control->base);
  (*((root->base).vtable)->applyFlags)(1,0xffffffff,&root->base);
  UiNode_InvalidateRoot(&control->base);
  UiNode_InvalidateRoot(&root->base);
  return;
}

/* Address: 0x004B0F30.
   Ownership: ui/controls/layout.
   Purpose: Loads engine\win.gfx and engine\winclass.gfx, registers texte\winclass.str as string-table page 1,
   installs UI action-handler page zero, and resets the UI root stack to its 0xFFFFFFFF sentinel.
   Cross-module calls: TextResourcePage_Load [assets/text/resources], UiActionHandlers_SetPageCf [ui/core/runtime].
*/
void UiWindowResources_Init(void)

{
  dword in_ECX;
  dword arg0;
  dword in_EDX;
  undefined8 uVar1;
  
  (*g_GraphicsTextureSourceLoadPackageAsset)(in_ECX,in_EDX,(word *)u_engine_win_gfx_004b0f06);
  uVar1 = (*g_FatalErrorPrimaryDispatchCf)();
  g_UiWindowTextureSource = (GraphicsTextureSourceAsset *)uVar1;
  (*g_GraphicsTextureSourceLoadPackageAsset)
            (arg0,(dword)((ulonglong)uVar1 >> 0x20),(word *)(u__engine_winclass_gfx_004b0eb6 + 1));
  g_UiWindowClassTextureSource = (GraphicsTextureSourceAsset *)(*g_FatalErrorPrimaryDispatchCf)();
  TextResourcePage_Load(1,(word *)u_texte_winclass_str_004b0ee0);
  (*g_FatalErrorPrimaryDispatchCf)();
  UiActionHandlers_SetPageCf(0,(UiActionHandlerPage *)&g_UiRootStackActionHandlerPage);
  g_UiRootNode = (UiRootNode *)0xffffffff;
  return;
}

/* Address: 0x004B1240.
   Ownership: ui/controls/layout.
   Purpose: Recomputes every UiRootNode rectangle from framebuffer dimensions, fixed offsets, and Q31 anchors,
   invokes each root layout method, and follows previousRoot toward the back of the stack.
*/
void UiRootStack_Relayout(void)

{
  longlong lVar1;
  UiRootNode *control;
  longlong currentAnchorPixelProductQ31;
  longlong anchorPixelProductQ31;
  
  control = g_UiRootNode;
  do {
    anchorPixelProductQ31 =
         (ulonglong)g_FramebufferWidth * (ulonglong)(control->base).rightAnchorQ31;
    (control->base).right =
         ((int)((ulonglong)anchorPixelProductQ31 >> 0x20) << 1 | (uint)anchorPixelProductQ31 >> 0x1f
         ) + (control->base).rightOffset;
    currentAnchorPixelProductQ31 =
         (ulonglong)g_FramebufferHeight * (ulonglong)(control->base).bottomAnchorQ31;
    (control->base).bottom =
         ((int)((ulonglong)currentAnchorPixelProductQ31 >> 0x20) << 1 |
         (uint)currentAnchorPixelProductQ31 >> 0x1f) + (control->base).bottomOffset;
    lVar1 = (ulonglong)g_FramebufferWidth * (ulonglong)(control->base).leftAnchorQ31;
    (control->base).left =
         ((int)((ulonglong)lVar1 >> 0x20) << 1 | (uint)lVar1 >> 0x1f) + (control->base).leftOffset;
    lVar1 = (ulonglong)g_FramebufferHeight * (ulonglong)(control->base).topAnchorQ31;
    (control->base).top =
         ((int)((ulonglong)lVar1 >> 0x20) << 1 | (uint)lVar1 >> 0x1f) + (control->base).topOffset;
    (*((control->base).vtable)->layout)(&control->base);
    control = control->previousRoot;
  } while (control != (UiRootNode *)0xffffffff);
  return;
}

/* Address: 0x004B3EE0.
   Ownership: ui/controls/layout.
   Purpose: Shared UiNodeVtable pointer-move callback that consumes the three pointer-move arguments and returns
   constant code 6. The current callers ignore EAX, so the name preserves the verified constant rather than
   assigning an unproven status meaning.
*/
GraphicsCursorFrameIndex
UiContainer_PointerMoveReturnCode6
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control)

{
  return 6;
}

/* Address: 0x004B4740.
   Ownership: ui/controls/layout.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B4650[13]@004B4650.
   Cross-module calls: UiNode_ApplyFlagsRecursive [ui/core/runtime].
*/
void UiLayoutContainerControl_ApplyFlagsRecursive
               (UiNodeFlagMask setMask,UiNodeFlagMask retainMask,UiNodeBase *control)

{
  int extraout_ECX;
  int extraout_EDX;
  UiNodeBase **childPointerCursor;
  UiNodeBase *savedFirstChild;
  
  childPointerCursor = &control[1].firstChild;
  savedFirstChild = control->firstChild;
  do {
    control->firstChild = *childPointerCursor;
    UiNode_ApplyFlagsRecursive(setMask,retainMask,control);
    childPointerCursor = (UiNodeBase **)(extraout_EDX + 4);
  } while (extraout_ECX != 1);
  control->firstChild = savedFirstChild;
  return;
}

/* Address: 0x004B5120.
   Ownership: ui/controls/layout.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B4CC0[4]@004B4CC0.
*/
void UiResizableWindowControl_BeginMoveResizeOrWindowAction
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiNodeBase *control)

{
  int arg1;
  uint arg0;
  uint uVar1;
  qword qVar4;
  int iVar2;
  bool bVar3;
  
  arg1 = pointerX - control->left;
  arg0 = pointerY - control->top;
  bVar3 = false;
  if ((((uint)control[1].nextSibling & 8) != 0) &&
     ((*g_GraphicsTextureSourceTestOpaquePixel)(arg0,arg1,0,0,1,g_UiWindowTextureSource), bVar3)) {
    control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling | 0x800);
    return;
  }
  if (((uint)control[1].nextSibling & 0x10) != 0) {
    qVar4._0_4_ = (*g_GraphicsTextureSourceGetLogicalSize)(4,g_UiWindowTextureSource);
    bVar3 = (uint)qVar4._4_4_ < (uint)(qword)qVar4;
    (*g_GraphicsTextureSourceTestOpaquePixel)
              (arg0,arg1,0,(int)qVar4._4_4_ - (int)(qword)qVar4,4,g_UiWindowTextureSource);
    if (bVar3) {
      control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling | 0x1000);
      return;
    }
  }
  if (((uint)control[1].nextSibling & 0x80) == 0) {
    if (((uint)control[1].nextSibling & 0x40) != 0) {
      qVar4 = (*g_GraphicsTextureSourceGetLogicalSize)(0x10,g_UiWindowTextureSource);
      iVar2 = (int)(qVar4 >> 0x20);
      if ((((arg1 < (int)qVar4) || ((int)arg0 < iVar2)) ||
          (control->layoutWidth - (int)qVar4 <= arg1)) ||
         (control->layoutHeight - iVar2 <= (int)arg0)) {
        iVar2 = control->layoutHeight - g_UiWindowResizeBorderThickness;
        if (arg1 < g_UiWindowResizeBorderThickness) {
          uVar1 = 0x80004000;
          if ((g_UiWindowResizeBorderThickness <= (int)arg0) &&
             (uVar1 = 0x20004000, (int)arg0 < iVar2)) {
            uVar1 = 0x40004000;
          }
        }
        else if (arg1 < control->layoutWidth - g_UiWindowResizeBorderThickness) {
          uVar1 = 0x1004000;
          if (g_UiWindowResizeBorderThickness <= (int)arg0) {
            uVar1 = 0x10004000;
          }
        }
        else {
          uVar1 = 0x2004000;
          if ((g_UiWindowResizeBorderThickness <= (int)arg0) &&
             (uVar1 = 0x8004000, (int)arg0 < iVar2)) {
            uVar1 = 0x4004000;
          }
        }
        control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling & 0xffffff);
        control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling | uVar1);
        return;
      }
    }
    if ((((uint)control[1].nextSibling & 0x20) != 0) && (arg0 < g_UiWindowMoveHandleWidth)) {
      control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling | 0x2000);
      control[1].topOffset = arg1;
      control[1].rightOffset = arg0;
      (*g_GraphicsCursorSetFrame)(1);
    }
  }
  return;
}

/* Address: 0x004B5660.
   Ownership: ui/controls/layout.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B4CC0[10]@004B4CC0.
*/
GraphicsCursorFrameIndex
UiResizableWindowControl_QueryResizeCursorCode
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control)

{
  GraphicsCursorFrameIndex GVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  qword qVar5;
  
  GVar1 = 0;
  if ((((uint)control[1].nextSibling & 0x40) != 0) && (((uint)control[1].nextSibling & 0x80) == 0))
  {
    iVar4 = pointerX - control->left;
    if ((control->left <= pointerX) &&
       (((iVar3 = pointerY - control->top, control->top <= pointerY &&
         (iVar4 < control->layoutWidth)) && (iVar3 < control->layoutHeight)))) {
      qVar5 = (*g_GraphicsTextureSourceGetLogicalSize)(0x10,g_UiWindowTextureSource);
      iVar2 = (int)(qVar5 >> 0x20);
      if ((((iVar4 < (int)qVar5) || (iVar3 < iVar2)) || (control->layoutWidth - (int)qVar5 <= iVar4)
          ) || (GVar1 = 0, control->layoutHeight - iVar2 <= iVar3)) {
        iVar2 = control->layoutHeight - g_UiWindowResizeBorderThickness;
        if (iVar4 < g_UiWindowResizeBorderThickness) {
          GVar1 = 2;
          if ((g_UiWindowResizeBorderThickness <= iVar3) && (GVar1 = 3, iVar3 < iVar2)) {
            return 5;
          }
        }
        else {
          if (iVar4 < control->layoutWidth - g_UiWindowResizeBorderThickness) {
            return 4;
          }
          GVar1 = 3;
          if ((g_UiWindowResizeBorderThickness <= iVar3) && (GVar1 = 2, iVar3 < iVar2)) {
            GVar1 = 5;
          }
        }
      }
    }
  }
  return GVar1;
}

/* Address: 0x00569A80.
   Ownership: ui/controls/layout.
   Purpose: Computes a compact grid for itemCount with at most maxRows. Typed parameters: p0
   maxRows→UiControlCount_V338, p1 itemCount→UiControlCount_V338. Nearby but non-identical semantic domains were
   explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals, locals, and
   executable data remain unchanged.
*/
UiGridDimensionsEdxEax8
UiGrid_ComputeDimensionsPacked(UiControlCount maxRows,UiControlCount itemCount)

{
  dword columnCount;
  uint rowCount;
  
  rowCount = 1;
  columnCount = itemCount;
  if (4 < itemCount) {
    if (maxRows << 2 < itemCount) {
      rowCount = maxRows & 0x3fffffff;
      columnCount = (itemCount - 1) / rowCount + 1;
      do {
        if ((rowCount - 1) * columnCount < itemCount) break;
        rowCount = rowCount - 1;
      } while (rowCount != 0);
    }
    else {
      columnCount = 4;
      rowCount = itemCount + 3 >> 2;
    }
  }
  return CONCAT44(rowCount,columnCount);
}

/* Address: 0x00569AE0.
   Ownership: ui/controls/layout.
   Purpose: Returns one column in EAX and itemCount rows in EDX. Typed parameters: p0
   itemCount→UiControlCount_V338. Nearby but non-identical semantic domains were explicitly deferred. Calling
   convention, parameter storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
UiGridDimensionsEdxEax8 UiGrid_OneColumnDimensionsPacked(UiControlCount itemCount)

{
  return CONCAT44(itemCount,1);
}

/* Address: 0x004B0940.
   Ownership: ui/controls/layout.
   Purpose: Handles ui container suppress action id.
*/
void UiContainer_SuppressActionId(UiActionId actionId,UiNodeBase *control)

{
  UiNodeBase *control_00;
  UiActionId extraout_ECX;
  
  for (control_00 = control->firstChild; control_00 != (UiNodeBase *)0xffffffff;
      control_00 = control_00->nextSibling) {
    (*control_00->vtable->suppressActionId)(actionId,control_00);
    actionId = extraout_ECX;
  }
  return;
}

/* Address: 0x004B0990.
   Ownership: ui/controls/layout.
   Purpose: Handles ui container unsuppress action id.
*/
void UiContainer_UnsuppressActionId(UiActionId actionId,UiNodeBase *control)

{
  UiNodeBase *control_00;
  UiActionId extraout_ECX;
  
  for (control_00 = control->firstChild; control_00 != (UiNodeBase *)0xffffffff;
      control_00 = control_00->nextSibling) {
    (*control_00->vtable->unsuppressActionId)(actionId,control_00);
    actionId = extraout_ECX;
  }
  return;
}

/* Address: 0x004B1420.
   Ownership: ui/controls/layout.
   Purpose: Walks a serialized sibling chain while layoutWidth is -1, converts next/child/parent image offsets to
   pointers, clears transient node flags, and invokes each node's relocate method. Typed parameters: p0
   imageDelta→SerializedImageRelocationDelta_V343. Calling convention, complete VariableStorage serialization,
   function bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void UiSerializedTree_Relocate(SerializedImageRelocationDelta imageDelta,UiNodeBase *firstNode)

{
  int extraout_ECX;
  int currentImageDelta;
  
  currentImageDelta = imageDelta;
  for (; (firstNode != (UiNodeBase *)0xffffffff && (firstNode->layoutWidth == -1));
      firstNode = firstNode->nextSibling) {
    firstNode->layoutWidth = ~firstNode->layoutWidth;
    if (firstNode->nextSibling != (UiNodeBase *)0xffffffff) {
      firstNode->nextSibling =
           (UiNodeBase *)((int)&firstNode->nextSibling->nextSibling + currentImageDelta);
    }
    if (firstNode->firstChild != (UiNodeBase *)0xffffffff) {
      firstNode->firstChild =
           (UiNodeBase *)((int)&firstNode->firstChild->nextSibling + currentImageDelta);
    }
    if (firstNode->parent != (UiNodeBase *)0xffffffff) {
      firstNode->parent = (UiNodeBase *)((int)&firstNode->parent->nextSibling + currentImageDelta);
    }
    firstNode->nodeFlags =
         firstNode->nodeFlags & ~(UI_NODE_REPEAT_OR_DOUBLE_CLICK|UI_NODE_HAS_KEYBOARD_FOCUS);
    (*firstNode->vtable->relocate)(imageDelta,firstNode);
    currentImageDelta = extraout_ECX;
  }
  return;
}

/* Address: 0x004B4850.
   Ownership: ui/controls/layout.
   Purpose: Recursively traverses every child and sibling below root using the 0xFFFFFFFF UI sentinel. Each visited
   node is passed to UiKeyboardFocus_AcquireIfNone, allowing the first eligible node to claim focus when no focus
   currently exists.
   Cross-module calls: UiKeyboardFocus_AcquireIfNone [ui/controls/input].
*/
void UiNodeSubtree_AcquireKeyboardFocusDefaults(UiNodeBase *root)

{
  UiNodeBase *node;
  UiNodeBase *root_00;
  undefined4 *extraout_EDX;
  
  node = root->firstChild;
  while (node != (UiNodeBase *)0xffffffff) {
    UiKeyboardFocus_AcquireIfNone(node);
    UiNodeSubtree_AcquireKeyboardFocusDefaults(root_00);
    node = (UiNodeBase *)*extraout_EDX;
  }
  return;
}

/* Address: 0x004B4890.
   Ownership: ui/controls/layout.
   Purpose: Recursively traverses every child and sibling below root using the 0xFFFFFFFF UI sentinel. Each visited
   node is passed to UiKeyboardFocus_ReleaseNode so an active focus node is advanced or cleared before its page
   subtree is deactivated.
   Cross-module calls: UiKeyboardFocus_ReleaseNode [ui/controls/input].
*/
void UiNodeSubtree_ReleaseKeyboardFocus(UiNodeBase *root)

{
  UiNodeBase *node;
  UiNodeBase *root_00;
  undefined4 *extraout_EDX;
  
  node = root->firstChild;
  while (node != (UiNodeBase *)0xffffffff) {
    UiKeyboardFocus_ReleaseNode(node);
    UiNodeSubtree_ReleaseKeyboardFocus(root_00);
    node = (UiNodeBase *)*extraout_EDX;
  }
  return;
}

/* Address: 0x004B50D0.
   Ownership: ui/controls/layout.
   Purpose: Lays out the container's children. When node flag 0x04 is set, it temporarily offsets top by the
   logical height of window subresource 0x0C, restores top, and adds that height to layoutHeight.
   Local calls: UiContainer_LayoutChildren.
*/
undefined8 UiContainer_LayoutWithOptionalWindowHeaderOffset(UiNodeBase *control)

{
  undefined4 in_EAX;
  undefined4 in_EDX;
  int extraout_EDX;
  int extraout_EDX_00;
  
  if (((uint)control[1].nextSibling & 4) == 0) {
    UiContainer_LayoutChildren(control);
  }
  else {
    (*g_GraphicsTextureSourceGetLogicalSize)(0xc,g_UiWindowTextureSource);
    control->top = control->top + extraout_EDX;
    UiContainer_LayoutChildren(control);
    control->top = control->top - extraout_EDX_00;
    control->layoutHeight = control->layoutHeight + extraout_EDX_00;
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x004AF680.
   Ownership: ui/controls/layout.
   Purpose: Pumps messages under the UI lock, invokes the active UiRootNode frame callback, ticks capture/focus
   nodes, and advances the delayed tooltip countdown. Typed parameters: p0 stopMessageCode→UiStopMessageCode_V343.
   Calling convention, complete VariableStorage serialization, function bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Cross-module calls: UiTree_AdvanceSpriteButtonAnimations [ui/controls/buttons], UiTooltip_TickCountdown
   [ui/controls/text], DirectInputMouse_RefreshDeviceIfIdle [platform/input/devices].
*/
void UiFrame_Update(UiStopMessageCode stopMessageCode)

{
  dword pendingFrameTicks;
  int extraout_ECX;
  bool hadPendingFrameTicks;
  UiRootNode *rootNode;
  UiRootCallbacks *rootCallbacks;
  
  (*g_SpinLockAcquire)(g_UiRuntimeFrameLock);
  do {
    pendingFrameTicks = (*g_Win32PumpMessages)();
    if (pendingFrameTicks == stopMessageCode) break;
  } while (pendingFrameTicks == g_UiPendingFrameTicks);
  hadPendingFrameTicks = g_UiPendingFrameTicks != 0;
  g_UiPendingFrameTicks = pendingFrameTicks;
  if (hadPendingFrameTicks) {
    do {
      rootNode = g_UiRootNode;
      if (g_UiRootNode != (UiRootNode *)0xffffffff) {
        UiTree_AdvanceSpriteButtonAnimations(&g_UiRootNode->base);
        rootCallbacks = rootNode->callbacks;
        if (rootCallbacks->frameUpdate != (UiRootFrameCallback *)0x0) {
          (*rootCallbacks->frameUpdate)(rootNode);
        }
      }
      if (g_UiPointerCaptureTarget != (UiNodeBase *)0xffffffff) {
        (*g_UiPointerCaptureTarget->vtable->tick)(g_UiPointerCaptureTarget);
      }
      if ((g_UiKeyboardFocusNode != (UiNodeBase *)0xffffffff) &&
         (g_UiKeyboardFocusNode != g_UiPointerCaptureTarget)) {
        (*g_UiKeyboardFocusNode->vtable->tick)(g_UiKeyboardFocusNode);
      }
      UiTooltip_TickCountdown();
    } while (extraout_ECX != 1);
  }
  g_DirectInputMouseRefreshCountdown = g_DirectInputMouseRefreshCountdown - 1;
  if (g_DirectInputMouseRefreshCountdown == 0) {
    g_DirectInputMouseRefreshCountdown = 0x30;
    DirectInputMouse_RefreshDeviceIfIdle();
  }
  (*g_SpinLockReleaseAndInvoke)
            ((SpinLockReleaseCallbackProc *)g_UiRuntimePostUnlockCallback,g_UiRuntimeFrameLock);
  return;
}

/* Address: 0x004AF7E0.
   Ownership: ui/controls/layout.
   Purpose: Draws the root stack from back to front using each node's drawClipped method, then draws the visible
   tooltip.
   Cross-module calls: UiTooltip_Draw [ui/controls/text].
*/
void UiFrame_Draw(void)

{
  int clipRight;
  int rootsRemaining;
  int extraout_ECX;
  int clipBottom;
  dword clipTop;
  dword clipLeft;
  UiRootNode *control;
  UiRootNode *rootCursor;
  
  if (g_UiRootNode != (UiRootNode *)0xffffffff) {
    rootsRemaining = 0;
    rootCursor = g_UiRootNode;
    do {
      control = rootCursor;
      rootsRemaining = rootsRemaining + 1;
      rootCursor = control->previousRoot;
    } while (control->previousRoot != (UiRootNode *)0xffffffff);
    do {
      clipRight = (control->base).left;
      clipBottom = (control->base).top;
      clipLeft = (control->base).right;
      clipTop = (control->base).bottom;
      if (clipRight < 0) {
        clipRight = 0;
      }
      if (clipBottom < 0) {
        clipBottom = 0;
      }
      if ((int)g_FramebufferWidth < (int)clipLeft) {
        clipLeft = g_FramebufferWidth;
      }
      if ((int)g_FramebufferHeight < (int)clipTop) {
        clipTop = g_FramebufferHeight;
      }
      if ((clipRight < (int)clipLeft) && (clipBottom < (int)clipTop)) {
        (*((control->base).vtable)->drawClipped)
                  (clipTop,clipLeft,clipBottom,clipRight,&control->base);
        rootsRemaining = extraout_ECX;
      }
      rootsRemaining = rootsRemaining + -1;
    } while (rootsRemaining != 0);
    UiTooltip_Draw(g_FramebufferHeight,g_FramebufferWidth,0,0);
  }
  return;
}

/* Address: 0x004B0800.
   Ownership: ui/controls/layout.
   Purpose: Tests eligible children in reverse visual order and calls their hitTest method.
*/
UiNodeBase *
UiContainer_HitTestChildren
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control)

{
  UiPixelCoordinate extraout_ECX;
  UiNodeBase *pUVar1;
  int unaff_EBP;
  int iVar2;
  undefined8 uVar3;
  
  pUVar1 = control->firstChild;
  iVar2 = 0;
  if (pUVar1 != (UiNodeBase *)0xffffffff) {
    do {
      if ((((pUVar1->nodeFlags & UI_NODE_ALLOW_CHILD_HIT_TEST_OUTSIDE_BOUNDS) != 0) ||
          ((((pUVar1->left <= pointerX && (pUVar1->top <= pointerY)) && (pointerX < pUVar1->right))
           && (pointerY < pUVar1->bottom)))) && ((pUVar1->nodeFlags & UI_NODE_SUPPRESSED) == 0)) {
        iVar2 = iVar2 + 1;
      }
      pUVar1 = pUVar1->nextSibling;
    } while (pUVar1 != (UiNodeBase *)0xffffffff);
    while (iVar2 != 0) {
      iVar2 = iVar2 + -1;
      uVar3 = (**(code **)(*(int *)(unaff_EBP + 0xc) + 0x2c))(pointerY,pointerX);
      pointerY = (UiPixelCoordinate)((ulonglong)uVar3 >> 0x20);
      pointerX = extraout_ECX;
      if ((UiNodeBase *)uVar3 != (UiNodeBase *)0xffffffff) {
        return (UiNodeBase *)uVar3;
      }
    }
  }
  return control;
}

/* Address: 0x004B13B0.
   Ownership: ui/controls/layout.
   Purpose: Forwards a tiled source-alpha interior blit using the shared UI window texture and framebuffer,
   translating all four tile coordinates by the node's resolved left and top edges. It selects an existing resource
   facet and does not imply sprite, model, or effect identity. Typed parameters: p4
   subresource→GraphicsSubresourceIndex_V338. Calling convention, storage, body bytes, control flow, and executable
   data remain unchanged. Typed parameters: p0 clipTop→UiPixelCoordinate_V297, p1 clipLeft→UiPixelCoordinate_V297,
   p2 clipBottom→UiPixelCoordinate_V297, p3 clipRight→UiPixelCoordinate_V297.
*/
void UiWindow_BlitTiledInterior
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,GraphicsSubresourceIndex subresource,
               UiPixelCoordinate tileTop,UiPixelCoordinate tileLeft,UiPixelCoordinate tileBottom,
               UiPixelCoordinate tileRight,void *node)

{
  (*g_GraphicsTextureSourceBlitTiledSourceAlpha)
            (clipTop,clipLeft,clipBottom,clipRight,tileTop + *(int *)((int)node + 0x14),
             tileLeft + *(int *)((int)node + 0x10),tileBottom + *(int *)((int)node + 0x14),
             tileRight + *(int *)((int)node + 0x10),subresource,g_UiWindowTextureSource,
             g_FramebufferAccess);
  return;
}

/* Address: 0x004B0510.
   Ownership: ui/controls/layout.
   Purpose: Relocates serialized next/child/parent pointers for unresolved children, clears transient node flag
   bits 0x04 and 0x80, and calls each child's vtable relocate method.
*/
void UiContainer_RelocateChildren(UiSerializedRelocationDelta relocationDelta,UiNodeBase *control)

{
  UiNodeBase *control_00;
  int extraout_ECX;
  
  for (control_00 = control->firstChild;
      (control_00 != (UiNodeBase *)0xffffffff && (control_00->layoutWidth == -1));
      control_00 = control_00->nextSibling) {
    control_00->layoutWidth = ~control_00->layoutWidth;
    if (control_00->nextSibling != (UiNodeBase *)0xffffffff) {
      control_00->nextSibling =
           (UiNodeBase *)((int)&control_00->nextSibling->nextSibling + relocationDelta);
    }
    if (control_00->firstChild != (UiNodeBase *)0xffffffff) {
      control_00->firstChild =
           (UiNodeBase *)((int)&control_00->firstChild->nextSibling + relocationDelta);
    }
    if (control_00->parent != (UiNodeBase *)0xffffffff) {
      control_00->parent = (UiNodeBase *)((int)&control_00->parent->nextSibling + relocationDelta);
    }
    control_00->nodeFlags =
         control_00->nodeFlags & ~(UI_NODE_REPEAT_OR_DOUBLE_CLICK|UI_NODE_HAS_KEYBOARD_FOCUS);
    (*control_00->vtable->relocate)(relocationDelta,control_00);
    relocationDelta = extraout_ECX;
  }
  return;
}

/* Address: 0x004B05B0.
   Ownership: ui/controls/layout.
   Purpose: Handles ui container draw intersecting children.
*/
void UiContainer_DrawIntersectingChildren
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,UiNodeBase *control)

{
  UiNodeBase *control_00;
  int extraout_ECX;
  int extraout_EDX;
  
  for (control_00 = control->firstChild; control_00 != (UiNodeBase *)0xffffffff;
      control_00 = control_00->nextSibling) {
    if ((((control_00->left <= clipLeft) && (control_00->top <= clipTop)) &&
        (clipRight < control_00->right)) && (clipBottom < control_00->bottom)) {
      (*control_00->vtable->drawClipped)(clipTop,clipLeft,clipBottom,clipRight,control_00);
      clipRight = extraout_ECX;
      clipBottom = extraout_EDX;
    }
  }
  return;
}

/* Address: 0x004B1350.
   Ownership: ui/controls/layout.
   Purpose: Forwards a tiled source-alpha blit using the shared UI window texture and framebuffer. Root-relative
   X/Y endpoints are derived from the node, while the other tile axis uses sentinel 0x80000000. It selects an
   existing resource facet and does not imply sprite, model, or effect identity. Typed parameters: p4
   subresource→GraphicsSubresourceIndex_V338. Calling convention, storage, body bytes, control flow, and executable
   data remain unchanged.
*/
void UiWindow_BlitTiledVerticalEdge
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,GraphicsSubresourceIndex subresource,
               UiPixelCoordinate edgeX,UiPixelCoordinate tileStart,UiPixelCoordinate tileEnd,
               void *node)

{
  (*g_GraphicsTextureSourceBlitTiledSourceAlpha)
            (clipTop,clipLeft,clipBottom,clipRight,edgeX + *(int *)((int)node + 0x14),-0x80000000,
             tileStart + *(int *)((int)node + 0x14),tileEnd + *(int *)((int)node + 0x10),subresource
             ,g_UiWindowTextureSource,g_FramebufferAccess);
  return;
}

/* Address: 0x004B0640.
   Ownership: ui/controls/layout.
   Purpose: Resolves each child rectangle from fixed offsets plus unsigned Q31 parent-relative anchors, stores
   width/height, then invokes the child layout method.
*/
void UiContainer_LayoutChildren(UiNodeBase *control)

{
  UiNodeBase *control_00;
  longlong lVar1;
  int computedEdgeCoordinate;
  int currentEdgeCoordinate;
  int iVar2;
  longlong currentAnchorPixelProductQ31;
  longlong anchorPixelProductQ31;
  
  control_00 = control->firstChild;
  control->layoutWidth = control->right - control->left;
  control->layoutHeight = control->bottom - control->top;
  for (; control_00 != (UiNodeBase *)0xffffffff; control_00 = control_00->nextSibling) {
    anchorPixelProductQ31 =
         (ulonglong)(uint)control->layoutWidth * (ulonglong)control_00->rightAnchorQ31;
    computedEdgeCoordinate =
         ((int)((ulonglong)anchorPixelProductQ31 >> 0x20) << 1 | (uint)anchorPixelProductQ31 >> 0x1f
         ) + control_00->rightOffset + control->left;
    control_00->right = computedEdgeCoordinate;
    control_00->layoutWidth = computedEdgeCoordinate;
    currentAnchorPixelProductQ31 =
         (ulonglong)(uint)control->layoutHeight * (ulonglong)control_00->bottomAnchorQ31;
    currentEdgeCoordinate =
         ((int)((ulonglong)currentAnchorPixelProductQ31 >> 0x20) << 1 |
         (uint)currentAnchorPixelProductQ31 >> 0x1f) + control_00->bottomOffset + control->top;
    control_00->bottom = currentEdgeCoordinate;
    control_00->layoutHeight = currentEdgeCoordinate;
    lVar1 = (ulonglong)(uint)control->layoutWidth * (ulonglong)control_00->leftAnchorQ31;
    iVar2 = ((int)((ulonglong)lVar1 >> 0x20) << 1 | (uint)lVar1 >> 0x1f) + control_00->leftOffset +
            control->left;
    control_00->left = iVar2;
    control_00->layoutWidth = control_00->layoutWidth - iVar2;
    lVar1 = (ulonglong)(uint)control->layoutHeight * (ulonglong)control_00->topAnchorQ31;
    iVar2 = ((int)((ulonglong)lVar1 >> 0x20) << 1 | (uint)lVar1 >> 0x1f) + control_00->topOffset +
            control->top;
    control_00->top = iVar2;
    control_00->layoutHeight = control_00->layoutHeight - iVar2;
    (*control_00->vtable->layout)(control_00);
  }
  return;
}

/* Address: 0x004B12F0.
   Ownership: ui/controls/layout.
   Purpose: Forwards a tiled source-alpha blit using the shared UI window texture and framebuffer. Root-relative
   X/Y endpoints are derived from the node, while one tile axis uses sentinel 0x80000000. It selects an existing
   resource facet and does not imply sprite, model, or effect identity. Typed parameters: p4
   subresource→GraphicsSubresourceIndex_V338. Calling convention, storage, body bytes, control flow, and executable
   data remain unchanged.
*/
void UiWindow_BlitTiledHorizontalEdge
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,GraphicsSubresourceIndex subresource,
               UiPixelCoordinate tileStart,UiPixelCoordinate edgeY,UiPixelCoordinate tileEnd,
               void *node)

{
  (*g_GraphicsTextureSourceBlitTiledSourceAlpha)
            (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,
             tileStart + *(int *)((int)node + 0x10),edgeY + *(int *)((int)node + 0x14),
             tileEnd + *(int *)((int)node + 0x10),subresource,g_UiWindowTextureSource,
             g_FramebufferAccess);
  return;
}

/* Address: 0x004B14D0.
   Ownership: ui/controls/layout.
   Purpose: Clears the dirty-rectangle count and appends every UiRootNode in the active stack when invalidation is
   not suppressed.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void UiRootStack_InvalidateAll(void)

{
  UiRootNode *node;
  int extraout_EDX;
  
  if (g_UiInvalidationSuppressed == 0) {
    g_UiDirtyRectCount = 0;
    node = g_UiRootNode;
    while (node != (UiRootNode *)0xffffffff) {
      UiNode_InvalidateRoot(&node->base);
      node = *(UiRootNode **)(extraout_EDX + 0x54);
    }
  }
  return;
}
