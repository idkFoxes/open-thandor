/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/layout.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/layout.h>
#include <thandor/thandor.h>

/* Implementation ownership: ui/controls/layout. */

/* Address: 0x004B49A0.
   Ownership: ui/controls/layout.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B4950[2]@004B4950.
   Local calls: UiWindow_BlitTiledInterior, UiWindow_BlitTiledHorizontalEdge, UiWindow_BlitTiledVerticalEdge,
   UiContainer_DrawIntersectingChildren.
*/
void __thandor_void_preserve_eax_ecx_edx
UiPanelControl_DrawOptionalTiledBackgroundFrameAndChildren
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control)

{
  dword cornerWidth;
  GraphicsSubresourceIndex subresource;
  dword cornerHeight;
  int bottomEdgeY;
  int rightEdgeX;
  bool beginAccessFailed;
  GraphicsTextureSizeEaxEdxCf9 cornerSize;
  
  if (((uint)control[1].nextSibling & 3) != 0) {
    beginAccessFailed = (*g_GraphicsFramebufferBeginAccess)();
    if (!beginAccessFailed) {
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
        cornerSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x13,g_UiWindowTextureSource);
        cornerHeight = cornerSize.logicalHeightPixels;
        cornerWidth = cornerSize.logicalWidthPixels;
        rightEdgeX = control->layoutWidth - cornerWidth;
        bottomEdgeY = control->layoutHeight - cornerHeight;
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,control->top,control->left,0x10,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,control->top,rightEdgeX + control->left,0x11,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,bottomEdgeY + control->top,control->left,0x12,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,bottomEdgeY + control->top,rightEdgeX + control->left,
                   0x13,g_UiWindowTextureSource,g_FramebufferAccess);
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x14,rightEdgeX,0,cornerWidth,control);
        UiWindow_BlitTiledVerticalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x15,bottomEdgeY,cornerHeight,0,control);
        UiWindow_BlitTiledVerticalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x16,bottomEdgeY,cornerHeight,rightEdgeX,control);
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x17,rightEdgeX,bottomEdgeY,cornerWidth,control);
      }
      if (((uint)control[1].nextSibling & 0x200) != 0) {
        cornerSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x27,g_UiWindowTextureSource);
        cornerHeight = cornerSize.logicalHeightPixels;
        cornerWidth = cornerSize.logicalWidthPixels;
        rightEdgeX = control->layoutWidth - cornerWidth;
        bottomEdgeY = control->layoutHeight - cornerHeight;
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,control->top,control->left,0x24,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,control->top,rightEdgeX + control->left,0x25,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,bottomEdgeY + control->top,control->left,0x26,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,bottomEdgeY + control->top,rightEdgeX + control->left,
                   0x27,g_UiWindowTextureSource,g_FramebufferAccess);
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x28,rightEdgeX,0,cornerWidth,control);
        UiWindow_BlitTiledVerticalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x29,bottomEdgeY,cornerHeight,0,control);
        UiWindow_BlitTiledVerticalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x2a,bottomEdgeY,cornerHeight,rightEdgeX,control);
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x2b,rightEdgeX,bottomEdgeY,cornerWidth,control);
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
void __thandor_void_preserve_eax_ecx_edx
UiResizableWindowControl_DrawFrameTitleAndChildren
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control)

{
  dword cornerWidthOrSubresource;
  dword cornerHeight;
  int edgeOffset;
  int rightEdgeX;
  bool beginAccessFailed;
  TextResourceResolveEaxCf5 titleText;
  GraphicsTextureSizeEaxEdxCf9 textureSize;
  GraphicsTextureSizeEaxEdxCf9 rightCapSize;
  
  beginAccessFailed = (*g_GraphicsFramebufferBeginAccess)();
  if (!beginAccessFailed) {
    if (((uint)control[1].nextSibling & 1) != 0) {
      UiWindow_BlitTiledInterior
                (clipTop,clipLeft,clipBottom,clipRight,0,control->layoutHeight,control->layoutWidth,
                 0,0,control);
    }
    if (((uint)control[1].nextSibling & 2) != 0) {
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x13,g_UiWindowTextureSource);
      cornerHeight = textureSize.logicalHeightPixels;
      cornerWidthOrSubresource = textureSize.logicalWidthPixels;
      rightEdgeX = control->layoutWidth - cornerWidthOrSubresource;
      edgeOffset = control->layoutHeight - cornerHeight;
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,control->top,control->left,0x10,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,control->top,rightEdgeX + control->left,
                 0x11,g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,edgeOffset + control->top,control->left,0x12,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,edgeOffset + control->top,
                 rightEdgeX + control->left,0x13,g_UiWindowTextureSource,g_FramebufferAccess);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x14,rightEdgeX,0,cornerWidthOrSubresource,control);
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x15,edgeOffset,cornerHeight,0,control);
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x16,edgeOffset,cornerHeight,rightEdgeX,control);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x17,rightEdgeX,edgeOffset,cornerWidthOrSubresource,control);
    }
    if (((uint)control[1].nextSibling & 4) != 0) {
      cornerWidthOrSubresource = 10;
      if ((control->nodeFlags & 1) == 0) {
        cornerWidthOrSubresource = 0xb;
      }
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,control->top,control->left,cornerWidthOrSubresource,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(cornerWidthOrSubresource,g_UiWindowTextureSource);
      edgeOffset = control->layoutWidth;
      rightCapSize = (*g_GraphicsTextureSourceGetLogicalSize)(cornerWidthOrSubresource + 4,g_UiWindowTextureSource);
      edgeOffset = edgeOffset - rightCapSize.logicalWidthPixels;
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,control->top,edgeOffset + control->left,cornerWidthOrSubresource + 4,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,cornerWidthOrSubresource + 2,edgeOffset,0,textureSize.logicalWidthPixels,
                 control);
      titleText = TextResource_Resolve((TextResourceId)control[1].vtable);
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,g_UiResizableWindowTitleTextStyle,titleText.eax,
                 g_UiResizableWindowTitleTextTopOffset + control->top,
                 (control->layoutWidth >> 1) + control->left);
    }
    if (((uint)control[1].nextSibling & 8) != 0) {
      cornerWidthOrSubresource = 1;
      if (((uint)control[1].nextSibling & 0x80000) != 0) {
        cornerWidthOrSubresource = 3;
      }
      if ((control->nodeFlags & 1) == 0) {
        cornerWidthOrSubresource = 2;
      }
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,control->top,control->left,cornerWidthOrSubresource,
                 g_UiWindowTextureSource,g_FramebufferAccess);
    }
    if (((uint)control[1].nextSibling & 0x10) != 0) {
      cornerWidthOrSubresource = 4;
      if (((uint)control[1].nextSibling & 0x100000) != 0) {
        cornerWidthOrSubresource = 6;
      }
      if ((control->nodeFlags & 1) == 0) {
        cornerWidthOrSubresource = 5;
      }
      if (((uint)control[1].nextSibling & 0x80) != 0) {
        cornerWidthOrSubresource = cornerWidthOrSubresource + 3;
      }
      edgeOffset = control->layoutWidth;
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(cornerWidthOrSubresource,g_UiWindowTextureSource);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,control->top,
                 (edgeOffset - textureSize.logicalWidthPixels) + control->left,cornerWidthOrSubresource,g_UiWindowTextureSource,
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
void __thandor_void_preserve_eax_ecx_edx
UiTitledWindowControl_DrawFrameTitleAndChildren
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control)

{
  dword cornerWidth;
  word *commandStream;
  int bottomEdgeY;
  dword titleCapX;
  int rightEdgeOrCursorX;
  bool beginAccessFailed;
  RichTextExtentRegs titleExtent;
  TextResourceResolveEaxCf5 titleText;
  GraphicsTextureSizeEaxEdxCf9 textureSize;
  int savedRightEdgeX;
  
  beginAccessFailed = (*g_GraphicsFramebufferBeginAccess)();
  if (!beginAccessFailed) {
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,control->top,control->left,0x36,
               g_UiWindowTextureSource,g_FramebufferAccess);
    rightEdgeOrCursorX = control->layoutWidth;
    bottomEdgeY = control->layoutHeight;
    textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x39,g_UiWindowTextureSource);
    cornerWidth = textureSize.logicalWidthPixels;
    rightEdgeOrCursorX = rightEdgeOrCursorX - cornerWidth;
    bottomEdgeY = bottomEdgeY - textureSize.logicalHeightPixels;
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,control->top,rightEdgeOrCursorX + control->left,0x37,
               g_UiWindowTextureSource,g_FramebufferAccess);
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,bottomEdgeY + control->top,control->left,0x38,
               g_UiWindowTextureSource,g_FramebufferAccess);
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,bottomEdgeY + control->top,rightEdgeOrCursorX + control->left,0x39
               ,g_UiWindowTextureSource,g_FramebufferAccess);
    textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x36,g_UiWindowTextureSource);
    savedRightEdgeX = rightEdgeOrCursorX;
    UiWindow_BlitTiledVerticalEdge
              (clipTop,clipLeft,clipBottom,clipRight,0x3b,bottomEdgeY,textureSize.logicalHeightPixels,0,control)
    ;
    UiWindow_BlitTiledVerticalEdge
              (clipTop,clipLeft,clipBottom,clipRight,0x3c,bottomEdgeY,textureSize.logicalHeightPixels,rightEdgeOrCursorX,
               control);
    UiWindow_BlitTiledHorizontalEdge
              (clipTop,clipLeft,clipBottom,clipRight,0x3d,rightEdgeOrCursorX,bottomEdgeY,cornerWidth,control);
    titleText = TextResource_Resolve((TextResourceId)control[1].firstChild);
    commandStream = titleText.eax;
    titleExtent = RichTextCommandStream_MeasureRegs(g_UiWindowTitleTextStyle,commandStream);
    titleCapX = cornerWidth;
    if (((uint)control[1].nextSibling & 1) != 0) {
      rightEdgeOrCursorX = control->layoutWidth;
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x3e,g_UiWindowTextureSource);
      titleCapX = ((int)(rightEdgeOrCursorX - titleExtent.widthPixels) >> 1) - textureSize.logicalWidthPixels;
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x3a,titleCapX,0,cornerWidth,control);
    }
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,control->top,titleCapX + control->left,0x3e,
               g_UiWindowTextureSource,g_FramebufferAccess);
    textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x3e,g_UiWindowTextureSource);
    rightEdgeOrCursorX = titleCapX + textureSize.logicalWidthPixels;
    RichTextCommandStream_DrawSingleLine
              (clipTop,clipLeft,clipBottom,clipRight,g_UiWindowTitleTextStyle,commandStream,
               control->top,rightEdgeOrCursorX + control->left);
    titleExtent = RichTextCommandStream_MeasureRegs(g_UiWindowTitleTextStyle,commandStream);
    rightEdgeOrCursorX = rightEdgeOrCursorX + titleExtent.widthPixels;
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,control->top,rightEdgeOrCursorX + control->left,0x3f,
               g_UiWindowTextureSource,g_FramebufferAccess);
    textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x3f,g_UiWindowTextureSource);
    UiWindow_BlitTiledHorizontalEdge
              (clipTop,clipLeft,clipBottom,clipRight,0x3a,savedRightEdgeX,0,
               rightEdgeOrCursorX + textureSize.logicalWidthPixels,control);
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
  bool tryAcquireResult;
  
  tryAcquireResult = (*g_SpinLockTryAcquire)(g_UiRuntimeFrameLock);
  if (!tryAcquireResult) {
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
void __thandor_void_preserve_eax_ecx_edx UiFrame_ProcessAndPresent(void)

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
void __thandor_void_preserve_eax_ecx_edx
UiPageStack_SetActiveIndex(UiPageIndex pageIndex,UiPageStackControl *stack)

{
  UiNodeBase *pageNode;
  
  if ((pageIndex < stack->pageCount) &&
     (pageNode = (&stack->pages)[pageIndex], pageNode != (stack->base).firstChild)) {
    UiNodeSubtree_ReleaseKeyboardFocus(&stack->base);
    (stack->base).firstChild = pageNode;
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
void __thandor_void_preserve_eax_ecx_edx
UiResizableWindowControl_EndMoveResizeAndHandleWindowActions
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control)

{
  sdword topOrRight;
  sdword rightOrBottom;
  sdword restoredBottom;
  dword framebufferHeight;
  
  if (((uint)control[1].nextSibling & 0x2000) != 0) {
    (*g_GraphicsCursorSetFrame)(0);
  }
  if ((((uint)control[1].nextSibling & 0x80000) != 0) &&
     ((control->nodeFlags & UI_NODE_REPEAT_OR_DOUBLE_CLICK) != 0)) {
    UiActionQueue_Enqueue(0,control);
  }
  if (((uint)control[1].nextSibling & 0x100000) != 0) {
    control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling ^ 0x80);
    if (((uint)control[1].nextSibling & 0x80) == 0) {
      topOrRight = control[1].right;
      control->left = control[1].top;
      rightOrBottom = control[1].bottom;
      restoredBottom = control[1].leftOffset;
      control->top = topOrRight;
      control->bottom = restoredBottom;
      control->right = rightOrBottom;
      UiContainer_LayoutWithOptionalWindowHeaderOffset(control);
      UiRootStack_InvalidateAll();
      goto UiResizableWindowControl_ClearInteractionStateAndReturn;
    }
    control[1].top = control->left;
    topOrRight = control->right;
    rightOrBottom = control->bottom;
    control[1].right = control->top;
    control[1].leftOffset = rightOrBottom;
    control[1].bottom = topOrRight;
    framebufferHeight = g_FramebufferHeight;
    control->right = g_FramebufferWidth;
    control->left = 0;
    control->top = 0;
    control->bottom = framebufferHeight;
    UiContainer_LayoutWithOptionalWindowHeaderOffset(control);
  }
  UiNode_InvalidateRoot(control);
UiResizableWindowControl_ClearInteractionStateAndReturn:
  control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling & 0xe787ff);
  return;
}


/* Address: 0x004B5770.
   Ownership: ui/controls/layout.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B4CC0[12]@004B4CC0.
   Local calls: UiContainer_LayoutWithOptionalWindowHeaderOffset, UiRootStack_InvalidateAll.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime], UiActionQueue_Enqueue [ui/core/runtime],
   UiNode_DefaultKeyboardEventMoveFocusNextCf [ui/controls/input].
*/
bool __thandor_cf_preserve_eax_ecx_edx
UiResizableWindowControl_HandleWindowHotkeysCf
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiNodeBase *control)

{
  sdword topOrRight;
  sdword rightOrBottom;
  sdword restoredBottom;
  dword framebufferHeight;
  bool delegateResult;
  
  if ((keyboardStateMask & 0x30) != 0) {
    if ((((uint)control[1].nextSibling & 8) == 0) || (keyCode != 99)) {
      if ((((uint)control[1].nextSibling & 0x10) == 0) || (keyCode != 0x7a))
      goto UiResizableWindowControl_DelegateUnhandledWindowHotkey;
      control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling ^ 0x80);
      if (((uint)control[1].nextSibling & 0x80) == 0) {
        topOrRight = control[1].right;
        control->left = control[1].top;
        rightOrBottom = control[1].bottom;
        restoredBottom = control[1].leftOffset;
        control->top = topOrRight;
        control->bottom = restoredBottom;
        control->right = rightOrBottom;
        UiContainer_LayoutWithOptionalWindowHeaderOffset(control);
        UiRootStack_InvalidateAll();
      }
      else {
        control[1].top = control->left;
        topOrRight = control->right;
        rightOrBottom = control->bottom;
        control[1].right = control->top;
        control[1].leftOffset = rightOrBottom;
        control[1].bottom = topOrRight;
        framebufferHeight = g_FramebufferHeight;
        control->right = g_FramebufferWidth;
        control->left = 0;
        control->top = 0;
        control->bottom = framebufferHeight;
        UiContainer_LayoutWithOptionalWindowHeaderOffset(control);
        UiNode_InvalidateRoot(control);
      }
    }
    else {
      UiActionQueue_Enqueue(0,control);
    }
    return false;
  }
UiResizableWindowControl_DelegateUnhandledWindowHotkey:
  delegateResult = UiNode_DefaultKeyboardEventMoveFocusNextCf(keyboardStateMask,keyCode,control);
  return delegateResult;
}


/* Address: 0x004B1000.
   Ownership: ui/controls/layout.
   Purpose: Resolves a serialized root rectangle, stores its callback table, relocates its tree, pushes it above
   the active root, suppresses the previous root, lays out and activates the new root, initializes focus, and
   clears capture/tooltip state.
   Local calls: UiSerializedTree_Relocate.
   Cross-module calls: UiKeyboardFocus_SelectInitial [ui/controls/input].
*/
void __thandor_void_preserve_eax_ecx_edx
UiRootStack_Push(UiRootCallbacks *callbacks,UiRootNode *root)

{
  longlong edgeAnchorPixelProductQ31;
  UiRootNode *oldFrontRoot;
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
  edgeAnchorPixelProductQ31 = (ulonglong)g_FramebufferWidth * (ulonglong)(root->base).leftAnchorQ31;
  (root->base).left =
       ((int)((ulonglong)edgeAnchorPixelProductQ31 >> 0x20) << 1 | (uint)edgeAnchorPixelProductQ31 >> 0x1f) + (root->base).leftOffset;
  edgeAnchorPixelProductQ31 = (ulonglong)g_FramebufferHeight * (ulonglong)(root->base).topAnchorQ31;
  (root->base).top =
       ((int)((ulonglong)edgeAnchorPixelProductQ31 >> 0x20) << 1 | (uint)edgeAnchorPixelProductQ31 >> 0x1f) + (root->base).topOffset;
  root->callbacks = callbacks;
  (root->base).nextSibling = (UiNodeBase *)0xffffffff;
  UiSerializedTree_Relocate((SerializedImageRelocationDelta)root,&root->base);
  oldFrontRoot = g_UiRootNode;
  LOCK();
  g_UiRootNode = root;
  UNLOCK();
  root->previousRoot = oldFrontRoot;
  if (oldFrontRoot != (UiRootNode *)0xffffffff) {
    (oldFrontRoot->base).nextSibling = &root->base;
    (*((oldFrontRoot->base).vtable)->applyFlags)(0,0xfffffffe,&oldFrontRoot->base);
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
bool __thandor_cf_preserve_eax_ecx_edx UiRootStack_PopCf(UiRootNode *root)

{
  bool closeCallbackVetoed;
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
    closeCallbackVetoed = (*root->callbacks->closeCf)(root);
  }
  if (closeCallbackVetoed) {
    UiNode_InvalidateRoot(&node->base);
    return true;
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
  return false;
}


/* Address: 0x004B2790.
   Ownership: ui/controls/layout.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B2740[0]@004B2740.
   Local calls: UiContainer_RelocateChildren.
*/
void __thandor_void_preserve_eax_ecx_edx
UiWindowControl_RelocateWithFrameInset
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
void __thandor_void_preserve_eax_ecx_edx
UiTitledWindowControl_LayoutFrameTitleAndChildren(UiNodeBase *control)

{
  dword leftInset;
  dword titleHeightOrRightInset;
  dword topInset;
  dword bottomInset;
  RichTextExtentRegs titleExtent;
  TextResourceResolveEaxCf5 titleText;
  GraphicsTextureSizeEaxEdxCf9 cornerSize;
  
  titleText = TextResource_Resolve((TextResourceId)control[1].firstChild);
  titleExtent = RichTextCommandStream_MeasureRegs(g_UiWindowTitleTextStyle,titleText.eax);
  titleHeightOrRightInset = titleExtent.heightPixels;
  cornerSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x36,g_UiWindowTextureSource);
  leftInset = cornerSize.logicalWidthPixels;
  topInset = cornerSize.logicalHeightPixels;
  if ((int)cornerSize.logicalHeightPixels < (int)titleHeightOrRightInset) {
    topInset = titleHeightOrRightInset;
  }
  control->left = control->left + leftInset;
  control->top = control->top + topInset;
  cornerSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x39,g_UiWindowTextureSource);
  bottomInset = cornerSize.logicalHeightPixels;
  titleHeightOrRightInset = cornerSize.logicalWidthPixels;
  control->right = control->right - titleHeightOrRightInset;
  control->bottom = control->bottom - bottomInset;
  UiContainer_LayoutChildren(control);
  control->right = control->right + titleHeightOrRightInset;
  control->bottom = control->bottom + bottomInset;
  control->layoutWidth = control->layoutWidth + titleHeightOrRightInset;
  control->layoutHeight = control->layoutHeight + bottomInset;
  control->left = control->left - leftInset;
  control->top = control->top - topInset;
  control->layoutWidth = control->layoutWidth + leftInset;
  control->layoutHeight = control->layoutHeight + topInset;
  return;
}


/* Address: 0x004B3C00.
   Ownership: ui/controls/layout.
   Purpose: Calls the shared child hit-test routine for one container vtable. A result equal to the container
   itself is converted to the 0xFFFFFFFF no-hit sentinel.
   Local calls: UiContainer_HitTestChildren.
*/
UiNodeBase * __thandor_eax_preserve_ecx_edx
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
void __thandor_void_preserve_eax_ecx_edx
UiHorizontalGaugeControl_DrawFrameFillAndLabel
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control)

{
  ulonglong scaledFillProduct;
  dword leftCapWidth;
  UiNodeBase *clampedValue;
  uint progressOrRange;
  int rightCapXOrFillMin;
  uint rangeProgressOrPercent;
  int fillEndX;
  uint divisionRemainder;
  word *commandStream;
  bool beginAccessFailed;
  GraphicsTextureSizeEaxEdxCf9 textureSize;
  
  beginAccessFailed = (*g_GraphicsFramebufferBeginAccess)();
  if (!beginAccessFailed) {
    rightCapXOrFillMin = control->layoutWidth;
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,control->top,control->left,0x7c,
               g_UiWindowTextureSource,g_FramebufferAccess);
    textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x7c,g_UiWindowTextureSource);
    leftCapWidth = textureSize.logicalWidthPixels;
    rightCapXOrFillMin = rightCapXOrFillMin - leftCapWidth;
    UiWindow_BlitTiledHorizontalEdge
              (clipTop,clipLeft,clipBottom,clipRight,0x7d,rightCapXOrFillMin,0,leftCapWidth,control);
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,control->top,rightCapXOrFillMin + control->left,0x7e,
               g_UiWindowTextureSource,g_FramebufferAccess);
    clampedValue = (UiNodeBase *)control[1].vtable;
    if (control[1].parent < clampedValue) {
      clampedValue = control[1].parent;
    }
    progressOrRange = (int)clampedValue - (int)control[1].firstChild;
    rangeProgressOrPercent = 0;
    if (progressOrRange != 0 && (int)control[1].firstChild <= (int)clampedValue) {
      scaledFillProduct = (ulonglong)progressOrRange * (ulonglong)(rightCapXOrFillMin - leftCapWidth);
      rangeProgressOrPercent = (int)control[1].parent - (int)control[1].firstChild;
      if (rangeProgressOrPercent == 0) {
        rangeProgressOrPercent = 1;
      }
      divisionRemainder = (uint)(scaledFillProduct % (ulonglong)rangeProgressOrPercent);
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x7f,g_UiWindowTextureSource);
      fillEndX = ((int)(scaledFillProduct / rangeProgressOrPercent) + (uint)CARRY4(divisionRemainder,divisionRemainder) + leftCapWidth) -
                  textureSize.logicalWidthPixels;
      rightCapXOrFillMin = textureSize.logicalWidthPixels + leftCapWidth;
      rangeProgressOrPercent = progressOrRange;
      if (rightCapXOrFillMin <= fillEndX) {
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x80,fillEndX,0,rightCapXOrFillMin,control);
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,control->top,leftCapWidth + control->left,0x7f,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,control->top,fillEndX + control->left,0x81
                   ,g_UiWindowTextureSource,g_FramebufferAccess);
      }
    }
    if (((uint)control[1].nextSibling & 1) != 0) {
      progressOrRange = (int)control[1].parent - (int)control[1].firstChild;
      if (progressOrRange == 0) {
        progressOrRange = 1;
      }
      divisionRemainder = (uint)(((ulonglong)rangeProgressOrPercent * 100) % (ulonglong)progressOrRange);
      rangeProgressOrPercent = (int)(((ulonglong)rangeProgressOrPercent * 100) / (ulonglong)progressOrRange) + (uint)CARRY4(divisionRemainder,divisionRemainder);
      if (rangeProgressOrPercent == 100) {
        g_UiWindowPercentTextUtf16[0] = 0x31;
        g_UiWindowPercentTextUtf16[1] = 0x30;
        g_UiWindowPercentTextUtf16[2] = 0x30;
        g_UiWindowPercentTextUtf16[3] = 0x25;
        g_UiWindowPercentTextUtf16[4] = 0;
      }
      else {
        g_UiWindowPercentTextUtf16[1] = (short)((ulonglong)rangeProgressOrPercent % 10) + 0x30;
        g_UiWindowPercentTextUtf16[0] = (short)((ulonglong)rangeProgressOrPercent / 10) + 0x30;
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
  return;
}


/* Address: 0x004B46A0.
   Ownership: ui/controls/layout.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B4650[0]@004B4650.
   Local calls: UiContainer_RelocateChildren.
*/
void __thandor_void_preserve_eax_ecx_edx
UiLayoutContainerControl_RelocateChildren
          (UiSerializedRelocationDelta relocationDelta,UiNodeBase *control)

{
  UiNodeBase *remainingCount;
  UiNodeBase **childSlot;
  UiNodeBase **childPointerCursor;
  
  childSlot = &control[1].firstChild;
  remainingCount = control[1].nextSibling;
  do {
    if (*childSlot != (UiNodeBase *)0xffffffff) {
      *childSlot = (UiNodeBase *)((int)&(*childSlot)->nextSibling + relocationDelta);
    }
    childSlot = childSlot + 1;
    remainingCount = (UiNodeBase *)((int)&remainingCount[-1].nodeFlags + 3);
  } while (remainingCount != (UiNodeBase *)0x0);
  childPointerCursor = &control[1].firstChild;
  remainingCount = control[1].nextSibling;
  do {
    control->firstChild = *childPointerCursor;
    UiContainer_RelocateChildren(relocationDelta,control);
    childPointerCursor = childPointerCursor + 1;
    remainingCount = (UiNodeBase *)((int)&remainingCount[-1].nodeFlags + 3);
  } while (remainingCount != (UiNodeBase *)0x0);
  control->firstChild = control[1].firstChild;
  return;
}


/* Address: 0x004B4700.
   Ownership: ui/controls/layout.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B4650[3]@004B4650.
   Local calls: UiContainer_LayoutChildren.
*/
void __thandor_void_preserve_eax_ecx_edx
UiLayoutContainerControl_LayoutChildren(UiNodeBase *control)

{
  UiNodeBase *remainingCount;
  UiNodeBase **childPointerCursor;
  UiNodeBase *savedFirstChild;
  
  childPointerCursor = &control[1].firstChild;
  savedFirstChild = control->firstChild;
  remainingCount = control[1].nextSibling;
  do {
    control->firstChild = *childPointerCursor;
    UiContainer_LayoutChildren(control);
    childPointerCursor = childPointerCursor + 1;
    remainingCount = (UiNodeBase *)((int)&remainingCount[-1].nodeFlags + 3);
  } while (remainingCount != (UiNodeBase *)0x0);
  control->firstChild = savedFirstChild;
  return;
}


/* Address: 0x004B4790.
   Ownership: ui/controls/layout.
   Purpose: Calls the shared child hit-test routine for a second container vtable. A result equal to the container
   itself is converted to the 0xFFFFFFFF no-hit sentinel.
   Local calls: UiContainer_HitTestChildren.
*/
UiNodeBase * __thandor_eax_preserve_ecx_edx
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
void __thandor_void_preserve_eax_ecx_edx
UiLayoutContainerControl_SuppressActionIdRecursive(UiActionId actionId,UiNodeBase *control)

{
  UiNodeBase *remainingCount;
  UiNodeBase **childPointerCursor;
  UiNodeBase *savedFirstChild;
  
  childPointerCursor = &control[1].firstChild;
  savedFirstChild = control->firstChild;
  remainingCount = control[1].nextSibling;
  do {
    control->firstChild = *childPointerCursor;
    UiContainer_SuppressActionId(actionId,control);
    childPointerCursor = childPointerCursor + 1;
    remainingCount = (UiNodeBase *)((int)&remainingCount[-1].nodeFlags + 3);
  } while (remainingCount != (UiNodeBase *)0x0);
  control->firstChild = savedFirstChild;
  return;
}


/* Address: 0x004B4800.
   Ownership: ui/controls/layout.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B4650[15]@004B4650.
   Local calls: UiContainer_UnsuppressActionId.
*/
void __thandor_void_preserve_eax_ecx_edx
UiLayoutContainerControl_UnsuppressActionIdRecursive(UiActionId actionId,UiNodeBase *control)

{
  UiNodeBase *remainingCount;
  UiNodeBase **childPointerCursor;
  UiNodeBase *savedFirstChild;
  
  childPointerCursor = &control[1].firstChild;
  savedFirstChild = control->firstChild;
  remainingCount = control[1].nextSibling;
  do {
    control->firstChild = *childPointerCursor;
    UiContainer_UnsuppressActionId(actionId,control);
    childPointerCursor = childPointerCursor + 1;
    remainingCount = (UiNodeBase *)((int)&remainingCount[-1].nodeFlags + 3);
  } while (remainingCount != (UiNodeBase *)0x0);
  control->firstChild = savedFirstChild;
  return;
}


/* Address: 0x004B4D10.
   Ownership: ui/controls/layout.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B4CC0[0]@004B4CC0.
   Local calls: UiContainer_RelocateChildren.
*/
void __thandor_void_preserve_eax_ecx_edx
UiResizableWindowControl_RelocateAndRefreshInteractionState
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
void __thandor_void_preserve_eax_ecx_edx
UiResizableWindowControl_UpdateMoveOrResize
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control)

{
  UiNodeBase *interactionFlags;
  sdword newRight;
  sdword newBottom;
  int offsetXOrEdge;
  int newLeftOrOldBottom;
  int offsetYOrEdge;
  int newTopOrOldRight;
  bool overButton;
  GraphicsTextureSizeEaxEdxCf9 buttonSize;
  
  offsetXOrEdge = pointerX - control->left;
  offsetYOrEdge = pointerY - control->top;
  if (((uint)control[1].nextSibling & 0x2000) == 0) {
    if (((uint)control[1].nextSibling & 0x800) == 0) {
      if (((uint)control[1].nextSibling & 0x1000) == 0) {
        if (((uint)control[1].nextSibling & 0x4000) != 0) {
          interactionFlags = control[1].nextSibling;
          offsetXOrEdge = offsetXOrEdge + control->left;
          offsetYOrEdge = offsetYOrEdge + control->top;
          newTopOrOldRight = offsetYOrEdge;
          if (((uint)interactionFlags & 0x83000000) == 0) {
            newTopOrOldRight = control->top;
          }
          if (((uint)interactionFlags & 0x38000000) == 0) {
            offsetYOrEdge = control->bottom;
          }
          newLeftOrOldBottom = offsetXOrEdge;
          if (((uint)interactionFlags & 0xe0000000) == 0) {
            newLeftOrOldBottom = control->left;
          }
          if (((uint)interactionFlags & 0xe000000) == 0) {
            offsetXOrEdge = control->right;
          }
          control[1].topOffset = offsetXOrEdge;
          control[1].rightOffset = offsetYOrEdge;
          offsetXOrEdge = (offsetXOrEdge - newLeftOrOldBottom) + -0x40;
          if (offsetXOrEdge < 0) {
            if (((uint)interactionFlags & 0xe000000) != 0) {
              control[1].topOffset = control[1].topOffset - offsetXOrEdge;
            }
            if (((uint)interactionFlags & 0xe0000000) != 0) {
              newLeftOrOldBottom = newLeftOrOldBottom + offsetXOrEdge;
            }
          }
          offsetXOrEdge = (offsetYOrEdge - newTopOrOldRight) + -0x40;
          if (offsetXOrEdge < 0) {
            if (((uint)interactionFlags & 0x38000000) != 0) {
              control[1].rightOffset = control[1].rightOffset - offsetXOrEdge;
            }
            if (((uint)interactionFlags & 0x83000000) != 0) {
              newTopOrOldRight = newTopOrOldRight + offsetXOrEdge;
            }
          }
          newRight = control[1].topOffset;
          newBottom = control[1].rightOffset;
          LOCK();
          offsetXOrEdge = control->left;
          control->left = newLeftOrOldBottom;
          UNLOCK();
          LOCK();
          offsetYOrEdge = control->top;
          control->top = newTopOrOldRight;
          UNLOCK();
          LOCK();
          newTopOrOldRight = control->right;
          control->right = newRight;
          UNLOCK();
          LOCK();
          newLeftOrOldBottom = control->bottom;
          control->bottom = newBottom;
          UNLOCK();
          if ((((offsetXOrEdge != control->left) || (offsetYOrEdge != control->top)) || (newTopOrOldRight != control->right))
             || (newLeftOrOldBottom != control->bottom)) {
            UiRootStack_InvalidateAll();
            (*control->vtable->layout)(control);
            UiNode_InvalidateRoot(control);
          }
        }
      }
      else {
        newTopOrOldRight = control->layoutWidth;
        buttonSize = (*g_GraphicsTextureSourceGetLogicalSize)(4,g_UiWindowTextureSource);
        overButton = (*g_GraphicsTextureSourceTestOpaquePixel)
                          (offsetYOrEdge,offsetXOrEdge,0,newTopOrOldRight - buttonSize.logicalWidthPixels,4,g_UiWindowTextureSource)
        ;
        if (overButton) {
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
      overButton = (*g_GraphicsTextureSourceTestOpaquePixel)(offsetYOrEdge,offsetXOrEdge,0,0,1,g_UiWindowTextureSource);
      if (overButton) {
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
  else {
    offsetXOrEdge = offsetXOrEdge - control[1].topOffset;
    offsetYOrEdge = offsetYOrEdge - control[1].rightOffset;
    if ((offsetYOrEdge != 0) || (offsetXOrEdge != 0)) {
      UiRootStack_InvalidateAll();
      control->left = control->left + offsetXOrEdge;
      control->top = control->top + offsetYOrEdge;
      control->right = control->right + offsetXOrEdge;
      control->bottom = control->bottom + offsetYOrEdge;
      (*control->vtable->layout)(control);
      UiNode_InvalidateRoot(control);
    }
  }
  return;
}


/* Address: 0x004BC660.
   Ownership: ui/controls/layout.
   Purpose: Temporarily substitutes the parent rectangle while laying out children, restores the control rectangle,
   and stores the parent width and height as the resolved layout size.
   Local calls: UiContainer_LayoutChildren.
*/
void __thandor_void_preserve_eax_ecx_edx
UiImageControl_LayoutChildrenToParent(UiImageControl *control)

{
  sdword *edgeField;
  int savedLeft;
  int savedTop;
  int savedRight;
  int savedBottom;
  UiNodeBase *parentNode;
  sdword parentTop;
  sdword parentRight;
  sdword parentBottom;
  
  parentNode = (control->selectable).base.parent;
  parentTop = parentNode->top;
  parentRight = parentNode->right;
  parentBottom = parentNode->bottom;
  LOCK();
  edgeField = &(control->selectable).base.left;
  savedLeft = *edgeField;
  *edgeField = parentNode->left;
  UNLOCK();
  LOCK();
  edgeField = &(control->selectable).base.top;
  savedTop = *edgeField;
  *edgeField = parentTop;
  UNLOCK();
  LOCK();
  edgeField = &(control->selectable).base.right;
  savedRight = *edgeField;
  *edgeField = parentRight;
  UNLOCK();
  LOCK();
  edgeField = &(control->selectable).base.bottom;
  savedBottom = *edgeField;
  *edgeField = parentBottom;
  UNLOCK();
  UiContainer_LayoutChildren((UiNodeBase *)control);
  (control->selectable).base.left = savedLeft;
  (control->selectable).base.top = savedTop;
  (control->selectable).base.right = savedRight;
  (control->selectable).base.bottom = savedBottom;
  (control->selectable).base.layoutWidth = savedRight - savedLeft;
  (control->selectable).base.layoutHeight = savedBottom - savedTop;
  return;
}


/* Address: 0x004AF3B0.
   Ownership: ui/controls/layout.
   Purpose: Flushes keyboard and pointer input through their installed service slots, then clears the pending UI
   frame-tick counter.
*/
void __thandor_void_preserve_eax_ecx_edx UiFrame_FlushInputAndResetPendingTicks(void)

{
  (*g_KeyboardFlushEvents)();
  (*g_PointerFlushEvents)();
  g_UiPendingFrameTicks = 0;
  return;
}


/* Address: 0x004AF950.
   Ownership: ui/controls/layout.
   Purpose: Runs the UI frame loop until the root closes and presents the final frame.
*/
void __thandor_void_preserve_eax_ecx_edx UiFrame_RunUntilRootClosedAndPresentFinalFrame(void)

{
  g_UiPointerCaptureTarget = (UiNodeBase *)0xffffffff;
  g_UiPointerCaptureButton = UI_POINTER_CAPTURE_NONE;
  UiFrame_FlushInputAndResetPendingTicks();
  do {
    UiKeyboard_DispatchPendingEvents();
    UiPointer_DispatchPendingEvents();
    UiFrame_Update(0);
    UiActionQueue_DispatchPending();
    UiFrame_Draw();
    (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
  } while (g_UiRootNode != (UiRootNode *)0xffffffff);
  g_UiTooltipState.targetNode = (UiNodeBase *)0x0;
  UiTooltip_Draw(g_FramebufferHeight,g_FramebufferWidth,0,0);
  (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
  return;
}

/* Address: 0x004AF9D0.
   Ownership: ui/controls/layout.
   Purpose: Unlinks a root from its current position, inserts it above the active root, refreshes focus and
   active/suppressed flags, and invalidates the previous and promoted roots.
   Cross-module calls: UiKeyboardFocus_SelectInitial [ui/controls/input], UiNode_InvalidateRoot [ui/core/runtime].
*/
bool __thandor_cf_preserve_eax_ecx_edx UiRootStack_BringToFront(UiRootNode *root)

{
  UiRootNode *belowRoot;
  UiRootNode *oldFrontRoot;
  UiNodeBase *nextRootLink;
  UiRootNode *detachedPreviousRoot;
  UiNodeBase *nextFrontRootLink;
  
  oldFrontRoot = g_UiRootNode;
  nextRootLink = (root->base).nextSibling;
  belowRoot = root->previousRoot;
  if (nextRootLink != (UiNodeBase *)0xffffffff) {
    nextRootLink[1].parent = &belowRoot->base;
  }
  if (belowRoot != (UiRootNode *)0xffffffff) {
    (belowRoot->base).nextSibling = nextRootLink;
  }
  nextFrontRootLink = (g_UiRootNode->base).nextSibling;
  root->previousRoot = g_UiRootNode;
  (root->base).nextSibling = nextFrontRootLink;
  (g_UiRootNode->base).nextSibling = &root->base;
  g_UiRootNode = root;
  UiKeyboardFocus_SelectInitial(&root->base);
  (*((oldFrontRoot->base).vtable)->applyFlags)(0,0xfffffffe,&oldFrontRoot->base);
  (*((root->base).vtable)->applyFlags)(1,0xffffffff,&root->base);
  UiNode_InvalidateRoot(&oldFrontRoot->base);
  UiNode_InvalidateRoot(&root->base);
  return false;
}


/* Address: 0x004B0F30.
   Ownership: ui/controls/layout.
   Purpose: Loads engine\win.gfx and engine\winclass.gfx, registers texte\winclass.str as string-table page 1,
   installs UI action-handler page zero, and resets the UI root stack to its 0xFFFFFFFF sentinel.
   Cross-module calls: TextResourcePage_Load [assets/text/resources], UiActionHandlers_SetPageCf [ui/core/runtime].
*/
void __thandor_preserve_eax UiWindowResources_Init(void)

{
  GraphicsTextureSourceLoadEaxCf5 loadResult;
  FatalErrorEaxCf5 checkedResult;
  TextResourceLoadEaxCf5 pageLoadResult;
  
  loadResult = (*g_GraphicsTextureSourceLoadPackageAsset)((word *)u_engine_win_gfx_004b0f06);
  checkedResult = (*g_FatalErrorPrimaryDispatchCf)((dword)loadResult.eax,loadResult.carry);
  g_UiWindowTextureSource = (GraphicsTextureSourceAsset *)checkedResult.eax;
  loadResult = (*g_GraphicsTextureSourceLoadPackageAsset)((word *)(u__engine_winclass_gfx_004b0eb6 + 1));
  checkedResult = (*g_FatalErrorPrimaryDispatchCf)((dword)loadResult.eax,loadResult.carry);
  g_UiWindowClassTextureSource = (GraphicsTextureSourceAsset *)checkedResult.eax;
  pageLoadResult = TextResourcePage_Load(1,(word *)u_texte_winclass_str_004b0ee0);
  (*g_FatalErrorPrimaryDispatchCf)(pageLoadResult.errorOrValue,pageLoadResult.carry);
  UiActionHandlers_SetPageCf(0,(UiActionHandlerPage *)&g_UiRootStackActionHandlerPage);
  g_UiRootNode = (UiRootNode *)0xffffffff;
  return;
}


/* Address: 0x004B1240.
   Ownership: ui/controls/layout.
   Purpose: Recomputes every UiRootNode rectangle from framebuffer dimensions, fixed offsets, and Q31 anchors,
   invokes each root layout method, and follows previousRoot toward the back of the stack.
*/
void __thandor_preserve_eax_edx UiRootStack_Relayout(void)

{
  longlong edgeAnchorPixelProductQ31;
  UiRootNode *rootNode;
  longlong currentAnchorPixelProductQ31;
  longlong anchorPixelProductQ31;
  
  rootNode = g_UiRootNode;
  do {
    anchorPixelProductQ31 =
         (ulonglong)g_FramebufferWidth * (ulonglong)(rootNode->base).rightAnchorQ31;
    (rootNode->base).right =
         ((int)((ulonglong)anchorPixelProductQ31 >> 0x20) << 1 | (uint)anchorPixelProductQ31 >> 0x1f
         ) + (rootNode->base).rightOffset;
    currentAnchorPixelProductQ31 =
         (ulonglong)g_FramebufferHeight * (ulonglong)(rootNode->base).bottomAnchorQ31;
    (rootNode->base).bottom =
         ((int)((ulonglong)currentAnchorPixelProductQ31 >> 0x20) << 1 |
         (uint)currentAnchorPixelProductQ31 >> 0x1f) + (rootNode->base).bottomOffset;
    edgeAnchorPixelProductQ31 = (ulonglong)g_FramebufferWidth * (ulonglong)(rootNode->base).leftAnchorQ31;
    (rootNode->base).left =
         ((int)((ulonglong)edgeAnchorPixelProductQ31 >> 0x20) << 1 | (uint)edgeAnchorPixelProductQ31 >> 0x1f) + (rootNode->base).leftOffset;
    edgeAnchorPixelProductQ31 = (ulonglong)g_FramebufferHeight * (ulonglong)(rootNode->base).topAnchorQ31;
    (rootNode->base).top =
         ((int)((ulonglong)edgeAnchorPixelProductQ31 >> 0x20) << 1 | (uint)edgeAnchorPixelProductQ31 >> 0x1f) + (rootNode->base).topOffset;
    (*((rootNode->base).vtable)->layout)(&rootNode->base);
    rootNode = rootNode->previousRoot;
  } while (rootNode != (UiRootNode *)0xffffffff);
  return;
}


/* Address: 0x004B3EE0.
   Ownership: ui/controls/layout.
   Purpose: Shared UiNodeVtable pointer-move callback that consumes the three pointer-move arguments and returns
   constant code 6. The current callers ignore EAX, so the name preserves the verified constant rather than
   assigning an unproven status meaning.
*/
GraphicsCursorFrameIndex __thandor_eax_preserve_ecx_edx
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
void __thandor_void_preserve_eax_ecx_edx
UiLayoutContainerControl_ApplyFlagsRecursive
          (UiNodeFlagMask setMask,UiNodeFlagMask retainMask,UiNodeBase *control)

{
  UiNodeBase *remainingCount;
  UiNodeBase **childPointerCursor;
  UiNodeBase *savedFirstChild;
  
  childPointerCursor = &control[1].firstChild;
  savedFirstChild = control->firstChild;
  remainingCount = control[1].nextSibling;
  do {
    control->firstChild = *childPointerCursor;
    UiNode_ApplyFlagsRecursive(setMask,retainMask,control);
    childPointerCursor = childPointerCursor + 1;
    remainingCount = (UiNodeBase *)((int)&remainingCount[-1].nodeFlags + 3);
  } while (remainingCount != (UiNodeBase *)0x0);
  control->firstChild = savedFirstChild;
  return;
}


/* Address: 0x004B5120.
   Ownership: ui/controls/layout.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B4CC0[4]@004B4CC0.
*/
void __thandor_void_preserve_eax_ecx_edx
UiResizableWindowControl_BeginMoveResizeOrWindowAction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control)

{
  int localX;
  uint localY;
  uint resizeFlags;
  int extentLimit;
  bool hitOpaque;
  GraphicsTextureSizeEaxEdxCf9 textureSize;
  
  localX = pointerX - control->left;
  localY = pointerY - control->top;
  if ((((uint)control[1].nextSibling & 8) != 0) &&
     (hitOpaque = (*g_GraphicsTextureSourceTestOpaquePixel)(localY,localX,0,0,1,g_UiWindowTextureSource),
     hitOpaque)) {
    control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling | 0x800);
    return;
  }
  if (((uint)control[1].nextSibling & 0x10) != 0) {
    extentLimit = control->layoutWidth;
    textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(4,g_UiWindowTextureSource);
    hitOpaque = (*g_GraphicsTextureSourceTestOpaquePixel)
                      (localY,localX,0,extentLimit - textureSize.logicalWidthPixels,4,g_UiWindowTextureSource);
    if (hitOpaque) {
      control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling | 0x1000);
      return;
    }
  }
  if (((uint)control[1].nextSibling & 0x80) == 0) {
    if (((uint)control[1].nextSibling & 0x40) != 0) {
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x10,g_UiWindowTextureSource);
      if ((((localX < (int)textureSize.logicalWidthPixels) || ((int)localY < (int)textureSize.logicalHeightPixels))
          || ((int)(control->layoutWidth - textureSize.logicalWidthPixels) <= localX)) ||
         ((int)(control->layoutHeight - textureSize.logicalHeightPixels) <= (int)localY)) {
        extentLimit = control->layoutHeight - g_UiWindowResizeBorderThickness;
        if (localX < g_UiWindowResizeBorderThickness) {
          resizeFlags = 0x80004000;
          if ((g_UiWindowResizeBorderThickness <= (int)localY) &&
             (resizeFlags = 0x20004000, (int)localY < extentLimit)) {
            resizeFlags = 0x40004000;
          }
        }
        else if (localX < control->layoutWidth - g_UiWindowResizeBorderThickness) {
          resizeFlags = 0x1004000;
          if (g_UiWindowResizeBorderThickness <= (int)localY) {
            resizeFlags = 0x10004000;
          }
        }
        else {
          resizeFlags = 0x2004000;
          if ((g_UiWindowResizeBorderThickness <= (int)localY) &&
             (resizeFlags = 0x8004000, (int)localY < extentLimit)) {
            resizeFlags = 0x4004000;
          }
        }
        control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling & 0xffffff);
        control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling | resizeFlags);
        return;
      }
    }
    if ((((uint)control[1].nextSibling & 0x20) != 0) && (localY < g_UiWindowMoveHandleWidth)) {
      control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling | 0x2000);
      control[1].topOffset = localX;
      control[1].rightOffset = localY;
      (*g_GraphicsCursorSetFrame)(1);
    }
  }
  return;
}


/* Address: 0x004B5660.
   Ownership: ui/controls/layout.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B4CC0[10]@004B4CC0.
*/
GraphicsCursorFrameIndex __thandor_eax_preserve_ecx_edx
UiResizableWindowControl_QueryResizeCursorCode
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control)

{
  GraphicsCursorFrameIndex cursorFrame;
  int bottomBorderY;
  int localY;
  int localX;
  GraphicsTextureSizeEaxEdxCf9 cornerSize;
  
  cursorFrame = 0;
  if ((((uint)control[1].nextSibling & 0x40) != 0) && (((uint)control[1].nextSibling & 0x80) == 0))
  {
    localX = pointerX - control->left;
    if ((control->left <= pointerX) &&
       (((localY = pointerY - control->top, control->top <= pointerY &&
         (localX < control->layoutWidth)) && (localY < control->layoutHeight)))) {
      cornerSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x10,g_UiWindowTextureSource);
      if ((((localX < (int)cornerSize.logicalWidthPixels) || (localY < (int)cornerSize.logicalHeightPixels)) ||
          ((int)(control->layoutWidth - cornerSize.logicalWidthPixels) <= localX)) ||
         (cursorFrame = 0, (int)(control->layoutHeight - cornerSize.logicalHeightPixels) <= localY)) {
        bottomBorderY = control->layoutHeight - g_UiWindowResizeBorderThickness;
        if (localX < g_UiWindowResizeBorderThickness) {
          cursorFrame = 2;
          if ((g_UiWindowResizeBorderThickness <= localY) && (cursorFrame = 3, localY < bottomBorderY)) {
            return 5;
          }
        }
        else {
          if (localX < control->layoutWidth - g_UiWindowResizeBorderThickness) {
            return 4;
          }
          cursorFrame = 3;
          if ((g_UiWindowResizeBorderThickness <= localY) && (cursorFrame = 2, localY < bottomBorderY)) {
            cursorFrame = 5;
          }
        }
      }
    }
  }
  return cursorFrame;
}


/* Address: 0x00569A80.
   Ownership: ui/controls/layout.
   Purpose: Computes a compact grid for itemCount with at most maxRows. Typed parameters: p0
   maxRows→UiControlCount_V338, p1 itemCount→UiControlCount_V338. Nearby but non-identical semantic domains were
   explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals, locals, and
   executable data remain unchanged.
*/
UiGridDimensionsEdxEax8 __thandor_eax_edx_cf_preserve_ecx
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
UiGridDimensionsEdxEax8 __thandor_eax_edx_cf_preserve_ecx
UiGrid_OneColumnDimensionsPacked(UiControlCount itemCount)

{
  return CONCAT44(itemCount,1);
}


/* Address: 0x004B0940.
   Ownership: ui/controls/layout.
   Purpose: Handles ui container suppress action id.
*/
void __thandor_void_preserve_eax_ecx_edx
UiContainer_SuppressActionId(UiActionId actionId,UiNodeBase *control)

{
  UiNodeBase *childNode;
  
  for (childNode = control->firstChild; childNode != (UiNodeBase *)0xffffffff;
      childNode = childNode->nextSibling) {
    (*childNode->vtable->suppressActionId)(actionId,childNode);
  }
  return;
}


/* Address: 0x004B0990.
   Ownership: ui/controls/layout.
   Purpose: Handles ui container unsuppress action id.
*/
void __thandor_void_preserve_eax_ecx_edx
UiContainer_UnsuppressActionId(UiActionId actionId,UiNodeBase *control)

{
  UiNodeBase *childNode;
  
  for (childNode = control->firstChild; childNode != (UiNodeBase *)0xffffffff;
      childNode = childNode->nextSibling) {
    (*childNode->vtable->unsuppressActionId)(actionId,childNode);
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
void __thandor_void_preserve_eax_ecx
UiSerializedTree_Relocate(SerializedImageRelocationDelta imageDelta,UiNodeBase *firstNode)

{
  int currentImageDelta;
  
  for (; (firstNode != (UiNodeBase *)0xffffffff && (firstNode->layoutWidth == -1));
      firstNode = firstNode->nextSibling) {
    firstNode->layoutWidth = ~firstNode->layoutWidth;
    if (firstNode->nextSibling != (UiNodeBase *)0xffffffff) {
      firstNode->nextSibling =
           (UiNodeBase *)((int)&firstNode->nextSibling->nextSibling + imageDelta);
    }
    if (firstNode->firstChild != (UiNodeBase *)0xffffffff) {
      firstNode->firstChild = (UiNodeBase *)((int)&firstNode->firstChild->nextSibling + imageDelta);
    }
    if (firstNode->parent != (UiNodeBase *)0xffffffff) {
      firstNode->parent = (UiNodeBase *)((int)&firstNode->parent->nextSibling + imageDelta);
    }
    firstNode->nodeFlags =
         firstNode->nodeFlags & ~(UI_NODE_REPEAT_OR_DOUBLE_CLICK|UI_NODE_HAS_KEYBOARD_FOCUS);
    (*firstNode->vtable->relocate)(imageDelta,firstNode);
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
void __thandor_void_preserve_eax_ecx_edx
UiNodeSubtree_AcquireKeyboardFocusDefaults(UiNodeBase *root)

{
  UiNodeBase *node;
  
  for (node = root->firstChild; node != (UiNodeBase *)0xffffffff; node = node->nextSibling) {
    UiKeyboardFocus_AcquireIfNone(node);
    UiNodeSubtree_AcquireKeyboardFocusDefaults(node);
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
void __thandor_void_preserve_eax_ecx_edx UiNodeSubtree_ReleaseKeyboardFocus(UiNodeBase *root)

{
  UiNodeBase *node;
  
  for (node = root->firstChild; node != (UiNodeBase *)0xffffffff; node = node->nextSibling) {
    UiKeyboardFocus_ReleaseNode(node);
    UiNodeSubtree_ReleaseKeyboardFocus(node);
  }
  return;
}


/* Address: 0x004B50D0.
   Ownership: ui/controls/layout.
   Purpose: Lays out the container's children. When node flag 0x04 is set, it temporarily offsets top by the
   logical height of window subresource 0x0C, restores top, and adds that height to layoutHeight.
   Local calls: UiContainer_LayoutChildren.
*/
void __thandor_void_preserve_eax_ecx_edx
UiContainer_LayoutWithOptionalWindowHeaderOffset(UiNodeBase *control)

{
  dword headerHeight;
  GraphicsTextureSizeEaxEdxCf9 headerSize;
  
  if (((uint)control[1].nextSibling & 4) == 0) {
    UiContainer_LayoutChildren(control);
  }
  else {
    headerSize = (*g_GraphicsTextureSourceGetLogicalSize)(0xc,g_UiWindowTextureSource);
    headerHeight = headerSize.logicalHeightPixels;
    control->top = control->top + headerHeight;
    UiContainer_LayoutChildren(control);
    control->top = control->top - headerHeight;
    control->layoutHeight = control->layoutHeight + headerHeight;
  }
  return;
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
void __thandor_void_preserve_eax_ecx UiFrame_Update(UiStopMessageCode stopMessageCode)

{
  dword ticksToRun;
  UiRootNode *frontRoot;
  dword nextPendingTicks;
  dword pendingFrameTicks;
  bool hadPendingFrameTicks;
  UiRootNode *rootNode;
  UiRootCallbacks *rootCallbacks;
  
  (*g_SpinLockAcquire)(g_UiRuntimeFrameLock);
  do {
    pendingFrameTicks = (*(code *)g_Win32PumpMessages)();
    nextPendingTicks = pendingFrameTicks;
    ticksToRun = g_UiPendingFrameTicks;
    frontRoot = g_UiRootNode;
    if (pendingFrameTicks == stopMessageCode) break;
    nextPendingTicks = pendingFrameTicks;
  } while (pendingFrameTicks == g_UiPendingFrameTicks);
  for (; g_UiPendingFrameTicks = nextPendingTicks, g_UiRootNode = frontRoot, ticksToRun != 0; ticksToRun = ticksToRun - 1) {
    if (frontRoot != (UiRootNode *)0xffffffff) {
      UiTree_AdvanceSpriteButtonAnimations(&frontRoot->base);
      rootCallbacks = frontRoot->callbacks;
      if (rootCallbacks->frameUpdate != (UiRootFrameCallback *)0x0) {
        (*rootCallbacks->frameUpdate)(frontRoot);
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
    nextPendingTicks = g_UiPendingFrameTicks;
    frontRoot = g_UiRootNode;
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
void __thandor_void_preserve_eax_ecx_edx UiFrame_Draw(void)

{
  /* Rewritten from the assembly (0x004AF7E0): the original pushes every root on the machine stack
     while walking previousRoot down, then pops them to draw bottom to top. The decompiled loop drew
     the bottom root once per root and never the roots stacked above it (e.g. the end movie). */
  enum { ROOT_LIMIT = 64 };
  UiRootNode *roots[ROOT_LIMIT];
  UiRootNode *root;
  int count;
  int clipRight;
  int clipBottom;
  int clipLeft;
  int clipTop;

  if (g_UiRootNode == (UiRootNode *)0xffffffff) {
    return;
  }
  count = 0;
  for (root = g_UiRootNode; (root != (UiRootNode *)0xffffffff) && (count < ROOT_LIMIT);
       root = root->previousRoot) {
    roots[count] = root;
    count = count + 1;
  }
  while (count != 0) {
    count = count - 1;
    root = roots[count];
    clipRight = (root->base).left;
    clipBottom = (root->base).top;
    clipLeft = (root->base).right;
    clipTop = (root->base).bottom;
    if (clipRight < 0) {
      clipRight = 0;
    }
    if (clipBottom < 0) {
      clipBottom = 0;
    }
    if ((int)g_FramebufferWidth < clipLeft) {
      clipLeft = g_FramebufferWidth;
    }
    if ((int)g_FramebufferHeight < clipTop) {
      clipTop = g_FramebufferHeight;
    }
    if ((clipRight < clipLeft) && (clipBottom < clipTop)) {
      (*((root->base).vtable)->drawClipped)(clipTop,clipLeft,clipBottom,clipRight,&root->base);
    }
  }
  UiTooltip_Draw(g_FramebufferHeight,g_FramebufferWidth,0,0);
  return;
}


/* Address: 0x004B0800.
   Ownership: ui/controls/layout.
   Purpose: Tests eligible children in reverse visual order and calls their hitTest method.
*/
UiNodeBase * __thandor_eax_preserve_ecx_edx
UiContainer_HitTestChildren
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control)

{
  /* Rewritten from the assembly (0x004B0800): eligible children are pushed on the machine stack
     in sibling order and hit-tested in reverse (topmost first); Ghidra lost the pushed nodes. */
  UiNodeBase *eligible[256];
  UiNodeBase *child;
  UiNodeBase *hit;
  int count;

  count = 0;
  for (child = control->firstChild; child != (UiNodeBase *)0xffffffff; child = child->nextSibling) {
    if ((((child->nodeFlags & UI_NODE_ALLOW_CHILD_HIT_TEST_OUTSIDE_BOUNDS) != 0) ||
         ((child->left <= pointerX && child->top <= pointerY) &&
          (pointerX < child->right && pointerY < child->bottom))) &&
        ((child->nodeFlags & UI_NODE_SUPPRESSED) == 0) && count < 256) {
      eligible[count] = child;
      count = count + 1;
    }
  }
  while (count != 0) {
    count = count - 1;
    hit = (*eligible[count]->vtable->hitTest)(pointerY,pointerX,eligible[count]);
    if (hit != (UiNodeBase *)0xffffffff) {
      return hit;
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
void __thandor_void_preserve_eax_ecx_edx
UiWindow_BlitTiledInterior
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,GraphicsSubresourceIndex subresource,UiPixelCoordinate tileTop
          ,UiPixelCoordinate tileLeft,UiPixelCoordinate tileBottom,UiPixelCoordinate tileRight,
          void *node)

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
void __thandor_void_preserve_eax_ecx_edx
UiContainer_RelocateChildren(UiSerializedRelocationDelta relocationDelta,UiNodeBase *control)

{
  UiNodeBase *childNode;
  
  for (childNode = control->firstChild;
      (childNode != (UiNodeBase *)0xffffffff && (childNode->layoutWidth == -1));
      childNode = childNode->nextSibling) {
    childNode->layoutWidth = ~childNode->layoutWidth;
    if (childNode->nextSibling != (UiNodeBase *)0xffffffff) {
      childNode->nextSibling =
           (UiNodeBase *)((int)&childNode->nextSibling->nextSibling + relocationDelta);
    }
    if (childNode->firstChild != (UiNodeBase *)0xffffffff) {
      childNode->firstChild =
           (UiNodeBase *)((int)&childNode->firstChild->nextSibling + relocationDelta);
    }
    if (childNode->parent != (UiNodeBase *)0xffffffff) {
      childNode->parent = (UiNodeBase *)((int)&childNode->parent->nextSibling + relocationDelta);
    }
    childNode->nodeFlags =
         childNode->nodeFlags & ~(UI_NODE_REPEAT_OR_DOUBLE_CLICK|UI_NODE_HAS_KEYBOARD_FOCUS);
    (*childNode->vtable->relocate)(relocationDelta,childNode);
  }
  return;
}


/* Address: 0x004B05B0.
   Ownership: ui/controls/layout.
   Purpose: Handles ui container draw intersecting children.
*/
void __thandor_void_preserve_eax_ecx_edx
UiContainer_DrawIntersectingChildren
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control)

{
  UiNodeBase *childNode;
  
  for (childNode = control->firstChild; childNode != (UiNodeBase *)0xffffffff;
      childNode = childNode->nextSibling) {
    if ((((childNode->left <= clipLeft) && (childNode->top <= clipTop)) &&
        (clipRight < childNode->right)) && (clipBottom < childNode->bottom)) {
      (*childNode->vtable->drawClipped)(clipTop,clipLeft,clipBottom,clipRight,childNode);
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
void __thandor_void_preserve_eax_ecx_edx
UiWindow_BlitTiledVerticalEdge
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,GraphicsSubresourceIndex subresource,UiPixelCoordinate edgeX,
          UiPixelCoordinate tileStart,UiPixelCoordinate tileEnd,void *node)

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
void __thandor_void_preserve_eax_ecx_edx UiContainer_LayoutChildren(UiNodeBase *control)

{
  UiNodeBase *childNode;
  longlong edgeAnchorPixelProductQ31;
  int computedEdgeCoordinate;
  int currentEdgeCoordinate;
  int edgeCoordinate;
  longlong currentAnchorPixelProductQ31;
  longlong anchorPixelProductQ31;
  
  childNode = control->firstChild;
  control->layoutWidth = control->right - control->left;
  control->layoutHeight = control->bottom - control->top;
  for (; childNode != (UiNodeBase *)0xffffffff; childNode = childNode->nextSibling) {
    anchorPixelProductQ31 =
         (ulonglong)(uint)control->layoutWidth * (ulonglong)childNode->rightAnchorQ31;
    computedEdgeCoordinate =
         ((int)((ulonglong)anchorPixelProductQ31 >> 0x20) << 1 | (uint)anchorPixelProductQ31 >> 0x1f
         ) + childNode->rightOffset + control->left;
    childNode->right = computedEdgeCoordinate;
    childNode->layoutWidth = computedEdgeCoordinate;
    currentAnchorPixelProductQ31 =
         (ulonglong)(uint)control->layoutHeight * (ulonglong)childNode->bottomAnchorQ31;
    currentEdgeCoordinate =
         ((int)((ulonglong)currentAnchorPixelProductQ31 >> 0x20) << 1 |
         (uint)currentAnchorPixelProductQ31 >> 0x1f) + childNode->bottomOffset + control->top;
    childNode->bottom = currentEdgeCoordinate;
    childNode->layoutHeight = currentEdgeCoordinate;
    edgeAnchorPixelProductQ31 = (ulonglong)(uint)control->layoutWidth * (ulonglong)childNode->leftAnchorQ31;
    edgeCoordinate = ((int)((ulonglong)edgeAnchorPixelProductQ31 >> 0x20) << 1 | (uint)edgeAnchorPixelProductQ31 >> 0x1f) + childNode->leftOffset +
            control->left;
    childNode->left = edgeCoordinate;
    childNode->layoutWidth = childNode->layoutWidth - edgeCoordinate;
    edgeAnchorPixelProductQ31 = (ulonglong)(uint)control->layoutHeight * (ulonglong)childNode->topAnchorQ31;
    edgeCoordinate = ((int)((ulonglong)edgeAnchorPixelProductQ31 >> 0x20) << 1 | (uint)edgeAnchorPixelProductQ31 >> 0x1f) + childNode->topOffset +
            control->top;
    childNode->top = edgeCoordinate;
    childNode->layoutHeight = childNode->layoutHeight - edgeCoordinate;
    (*childNode->vtable->layout)(childNode);
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
void __thandor_void_preserve_eax_ecx_edx
UiWindow_BlitTiledHorizontalEdge
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,GraphicsSubresourceIndex subresource,
          UiPixelCoordinate tileStart,UiPixelCoordinate edgeY,UiPixelCoordinate tileEnd,void *node)

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
void __thandor_void_preserve_ecx_edx UiRootStack_InvalidateAll(void)

{
  UiRootNode *node;
  
  if (g_UiInvalidationSuppressed == 0) {
    g_UiDirtyRectCount = 0;
    for (node = g_UiRootNode; node != (UiRootNode *)0xffffffff; node = node->previousRoot) {
      UiNode_InvalidateRoot(&node->base);
    }
  }
  return;
}

