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
          UiPixelCoordinate clipRight,UiPanelControl *control)

{
  uint32_t cornerWidth;
  GraphicsSubresourceIndex subresource;
  uint32_t cornerHeight;
  int bottomEdgeY;
  int rightEdgeX;
  bool beginAccessFailed;
  TextureSizeResult cornerSize;
  
  if ((control->root.rootFlags & 3) != 0) {
    beginAccessFailed = g_GraphicsFramebufferBeginAccess();
    if (!beginAccessFailed) {
      if ((control->root.rootFlags & 1) != 0) {
        subresource = 0;
        if ((control->root.rootFlags & 0x200) != 0) {
          subresource = 0x2c;
        }
        UiWindow_BlitTiledInterior
                  (clipTop,clipLeft,clipBottom,clipRight,subresource,control->root.base.layoutHeight,
                   control->root.base.layoutWidth,0,0,control);
      }
      if ((control->root.rootFlags & 2) != 0) {
        cornerSize = g_GraphicsTextureSourceGetLogicalSize(0x13,g_UiWindowTextureSource);
        cornerHeight = cornerSize.logicalHeightPixels;
        cornerWidth = cornerSize.logicalWidthPixels;
        rightEdgeX = control->root.base.layoutWidth - cornerWidth;
        bottomEdgeY = control->root.base.layoutHeight - cornerHeight;
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,control->root.base.top,control->root.base.left,0x10,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,control->root.base.top,rightEdgeX + control->root.base.left,0x11,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,bottomEdgeY + control->root.base.top,control->root.base.left,0x12,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,bottomEdgeY + control->root.base.top,rightEdgeX + control->root.base.left,
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
      if ((control->root.rootFlags & 0x200) != 0) {
        cornerSize = g_GraphicsTextureSourceGetLogicalSize(0x27,g_UiWindowTextureSource);
        cornerHeight = cornerSize.logicalHeightPixels;
        cornerWidth = cornerSize.logicalWidthPixels;
        rightEdgeX = control->root.base.layoutWidth - cornerWidth;
        bottomEdgeY = control->root.base.layoutHeight - cornerHeight;
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,control->root.base.top,control->root.base.left,0x24,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,control->root.base.top,rightEdgeX + control->root.base.left,0x25,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,bottomEdgeY + control->root.base.top,control->root.base.left,0x26,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,bottomEdgeY + control->root.base.top,rightEdgeX + control->root.base.left,
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
      g_GraphicsFramebufferEndAccess();
    }
  }
  UiContainer_DrawIntersectingChildren(clipTop,clipLeft,clipBottom,clipRight,(UiNodeBase *)control);
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
          UiPixelCoordinate clipRight,UiResizableWindowControl *control)

{
  uint32_t cornerWidthOrSubresource;
  uint32_t cornerHeight;
  int edgeOffset;
  int rightEdgeX;
  bool beginAccessFailed;
  TextResolveResult titleText;
  TextureSizeResult textureSize;
  TextureSizeResult rightCapSize;
  
  beginAccessFailed = g_GraphicsFramebufferBeginAccess();
  if (!beginAccessFailed) {
    if ((control->root.rootFlags & 1) != 0) {
      UiWindow_BlitTiledInterior
                (clipTop,clipLeft,clipBottom,clipRight,0,control->root.base.layoutHeight,control->root.base.layoutWidth,
                 0,0,control);
    }
    if ((control->root.rootFlags & 2) != 0) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize(0x13,g_UiWindowTextureSource);
      cornerHeight = textureSize.logicalHeightPixels;
      cornerWidthOrSubresource = textureSize.logicalWidthPixels;
      rightEdgeX = control->root.base.layoutWidth - cornerWidthOrSubresource;
      edgeOffset = control->root.base.layoutHeight - cornerHeight;
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,control->root.base.top,control->root.base.left,0x10,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,control->root.base.top,rightEdgeX + control->root.base.left,
                 0x11,g_UiWindowTextureSource,g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,edgeOffset + control->root.base.top,control->root.base.left,0x12,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,edgeOffset + control->root.base.top,
                 rightEdgeX + control->root.base.left,0x13,g_UiWindowTextureSource,g_FramebufferAccess);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x14,rightEdgeX,0,cornerWidthOrSubresource,control);
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x15,edgeOffset,cornerHeight,0,control);
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x16,edgeOffset,cornerHeight,rightEdgeX,control);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x17,rightEdgeX,edgeOffset,cornerWidthOrSubresource,control);
    }
    if ((control->root.rootFlags & 4) != 0) {
      cornerWidthOrSubresource = 10;
      if ((control->root.base.nodeFlags & 1) == 0) {
        cornerWidthOrSubresource = 0xb;
      }
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,control->root.base.top,control->root.base.left,cornerWidthOrSubresource,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      textureSize = g_GraphicsTextureSourceGetLogicalSize(cornerWidthOrSubresource,g_UiWindowTextureSource);
      edgeOffset = control->root.base.layoutWidth;
      rightCapSize = g_GraphicsTextureSourceGetLogicalSize(cornerWidthOrSubresource + 4,g_UiWindowTextureSource);
      edgeOffset = edgeOffset - rightCapSize.logicalWidthPixels;
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,control->root.base.top,edgeOffset + control->root.base.left,cornerWidthOrSubresource + 4,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,cornerWidthOrSubresource + 2,edgeOffset,0,textureSize.logicalWidthPixels,
                 control);
      titleText = TextResource_Resolve(control->titleTextResourceId);
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,g_UiResizableWindowTitleTextStyle,titleText.text,
                 g_UiResizableWindowTitleTextTopOffset + control->root.base.top,
                 (control->root.base.layoutWidth >> 1) + control->root.base.left);
    }
    if ((control->root.rootFlags & 8) != 0) {
      cornerWidthOrSubresource = 1;
      if ((control->root.rootFlags & 0x80000) != 0) {
        cornerWidthOrSubresource = 3;
      }
      if ((control->root.base.nodeFlags & 1) == 0) {
        cornerWidthOrSubresource = 2;
      }
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,control->root.base.top,control->root.base.left,cornerWidthOrSubresource,
                 g_UiWindowTextureSource,g_FramebufferAccess);
    }
    if ((control->root.rootFlags & 0x10) != 0) {
      cornerWidthOrSubresource = 4;
      if ((control->root.rootFlags & 0x100000) != 0) {
        cornerWidthOrSubresource = 6;
      }
      if ((control->root.base.nodeFlags & 1) == 0) {
        cornerWidthOrSubresource = 5;
      }
      if ((control->root.rootFlags & 0x80) != 0) {
        cornerWidthOrSubresource = cornerWidthOrSubresource + 3;
      }
      edgeOffset = control->root.base.layoutWidth;
      textureSize = g_GraphicsTextureSourceGetLogicalSize(cornerWidthOrSubresource,g_UiWindowTextureSource);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,control->root.base.top,
                 (edgeOffset - textureSize.logicalWidthPixels) + control->root.base.left,cornerWidthOrSubresource,g_UiWindowTextureSource,
                 g_FramebufferAccess);
    }
    g_GraphicsFramebufferEndAccess();
  }
  UiContainer_DrawIntersectingChildren(clipTop,clipLeft,clipBottom,clipRight,(UiNodeBase *)control);
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
          UiPixelCoordinate clipRight,UiTitledWindowControl *control)

{
  uint32_t cornerWidth;
  uint16_t *commandStream;
  int bottomEdgeY;
  uint32_t titleCapX;
  int rightEdgeOrCursorX;
  bool beginAccessFailed;
  RichTextExtentRegs titleExtent;
  TextResolveResult titleText;
  TextureSizeResult textureSize;
  int savedRightEdgeX;
  
  beginAccessFailed = g_GraphicsFramebufferBeginAccess();
  if (!beginAccessFailed) {
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipTop,clipLeft,clipBottom,clipRight,control->base.top,control->base.left,0x36,
               g_UiWindowTextureSource,g_FramebufferAccess);
    rightEdgeOrCursorX = control->base.layoutWidth;
    bottomEdgeY = control->base.layoutHeight;
    textureSize = g_GraphicsTextureSourceGetLogicalSize(0x39,g_UiWindowTextureSource);
    cornerWidth = textureSize.logicalWidthPixels;
    rightEdgeOrCursorX = rightEdgeOrCursorX - cornerWidth;
    bottomEdgeY = bottomEdgeY - textureSize.logicalHeightPixels;
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipTop,clipLeft,clipBottom,clipRight,control->base.top,rightEdgeOrCursorX + control->base.left,0x37,
               g_UiWindowTextureSource,g_FramebufferAccess);
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipTop,clipLeft,clipBottom,clipRight,bottomEdgeY + control->base.top,control->base.left,0x38,
               g_UiWindowTextureSource,g_FramebufferAccess);
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipTop,clipLeft,clipBottom,clipRight,bottomEdgeY + control->base.top,rightEdgeOrCursorX + control->base.left,0x39
               ,g_UiWindowTextureSource,g_FramebufferAccess);
    textureSize = g_GraphicsTextureSourceGetLogicalSize(0x36,g_UiWindowTextureSource);
    savedRightEdgeX = rightEdgeOrCursorX;
    UiWindow_BlitTiledVerticalEdge
              (clipTop,clipLeft,clipBottom,clipRight,0x3b,bottomEdgeY,textureSize.logicalHeightPixels,0,control)
    ;
    UiWindow_BlitTiledVerticalEdge
              (clipTop,clipLeft,clipBottom,clipRight,0x3c,bottomEdgeY,textureSize.logicalHeightPixels,rightEdgeOrCursorX,
               control);
    UiWindow_BlitTiledHorizontalEdge
              (clipTop,clipLeft,clipBottom,clipRight,0x3d,rightEdgeOrCursorX,bottomEdgeY,cornerWidth,control);
    titleText = TextResource_Resolve(control->titleTextResourceId);
    commandStream = titleText.text;
    titleExtent = RichTextCommandStream_MeasureRegs(g_UiWindowTitleTextStyle,commandStream);
    titleCapX = cornerWidth;
    if ((control->titleFlags & 1) != 0) {
      rightEdgeOrCursorX = control->base.layoutWidth;
      textureSize = g_GraphicsTextureSourceGetLogicalSize(0x3e,g_UiWindowTextureSource);
      titleCapX = ((int)(rightEdgeOrCursorX - titleExtent.widthPixels) >> 1) - textureSize.logicalWidthPixels;
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x3a,titleCapX,0,cornerWidth,control);
    }
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipTop,clipLeft,clipBottom,clipRight,control->base.top,titleCapX + control->base.left,0x3e,
               g_UiWindowTextureSource,g_FramebufferAccess);
    textureSize = g_GraphicsTextureSourceGetLogicalSize(0x3e,g_UiWindowTextureSource);
    rightEdgeOrCursorX = titleCapX + textureSize.logicalWidthPixels;
    RichTextCommandStream_DrawSingleLine
              (clipTop,clipLeft,clipBottom,clipRight,g_UiWindowTitleTextStyle,commandStream,
               control->base.top,rightEdgeOrCursorX + control->base.left);
    titleExtent = RichTextCommandStream_MeasureRegs(g_UiWindowTitleTextStyle,commandStream);
    rightEdgeOrCursorX = rightEdgeOrCursorX + titleExtent.widthPixels;
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipTop,clipLeft,clipBottom,clipRight,control->base.top,rightEdgeOrCursorX + control->base.left,0x3f,
               g_UiWindowTextureSource,g_FramebufferAccess);
    textureSize = g_GraphicsTextureSourceGetLogicalSize(0x3f,g_UiWindowTextureSource);
    UiWindow_BlitTiledHorizontalEdge
              (clipTop,clipLeft,clipBottom,clipRight,0x3a,savedRightEdgeX,0,
               rightEdgeOrCursorX + textureSize.logicalWidthPixels,control);
    g_GraphicsFramebufferEndAccess();
  }
  UiContainer_DrawIntersectingChildren(clipTop,clipLeft,clipBottom,clipRight,(UiNodeBase *)control);
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
  
  tryAcquireResult = g_SpinLockTryAcquire(g_UiRuntimeFrameLock);
  if (!tryAcquireResult) {
    g_SpinLockRelease(g_UiRuntimeFrameLock);
    UiKeyboard_DispatchPendingEvents();
    UiPointer_DispatchPendingEvents();
    UiFrame_Update(0);
    UiActionQueue_DispatchPending();
    UiFrame_Draw();
    g_GraphicsFramebufferPresent(g_FramebufferAccess);
    return;
  }
  g_SpinLockRelease(g_UiRuntimeFrameLock);
  UiKeyboard_DispatchPendingEvents();
  UiPointer_DispatchPendingEvents();
  UiFrame_Update(0);
  UiActionQueue_DispatchPending();
  UiFrame_Draw();
  g_GraphicsFramebufferPresent(g_FramebufferAccess);
  g_SpinLockAcquire(g_UiRuntimeFrameLock);
  return;
}


/* Address: 0x004AF920.
   Runs one complete UI frame: dispatches pending keyboard and pointer events, runs the pending frame ticks,
   dispatches queued UI actions, draws and presents. Unlike UiFrame_ProcessAndPresentWithLockTransition it does
   not touch the UI frame lock itself.
*/
void __thandor_void_preserve_eax_ecx_edx UiFrame_ProcessAndPresent(void)

{
  UiKeyboard_DispatchPendingEvents();
  UiPointer_DispatchPendingEvents();
  UiFrame_Update(0); /* 0: pump messages once, do not wait for a frame tick */
  UiActionQueue_DispatchPending();
  UiFrame_Draw();
  g_GraphicsFramebufferPresent(g_FramebufferAccess);
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
          UiResizableWindowControl *control)

{
  int32_t topOrRight;
  int32_t rightOrBottom;
  int32_t restoredBottom;
  uint32_t framebufferHeight;
  
  if ((control->root.rootFlags & 0x2000) != 0) {
    g_GraphicsCursorSetFrame(0);
  }
  if (((control->root.rootFlags & 0x80000) != 0) &&
     ((control->root.base.nodeFlags & UI_NODE_REPEAT_OR_DOUBLE_CLICK) != 0)) {
    UiActionQueue_Enqueue(0,control);
  }
  if ((control->root.rootFlags & 0x100000) != 0) {
    control->root.rootFlags = control->root.rootFlags ^ 0x80;
    if ((control->root.rootFlags & 0x80) == 0) {
      topOrRight = control->restoredTop;
      control->root.base.left = control->restoredLeft;
      rightOrBottom = control->restoredRight;
      restoredBottom = control->restoredBottom;
      control->root.base.top = topOrRight;
      control->root.base.bottom = restoredBottom;
      control->root.base.right = rightOrBottom;
      UiContainer_LayoutWithOptionalWindowHeaderOffset(control);
      UiRootStack_InvalidateAll();
      control->root.rootFlags = control->root.rootFlags & 0xe787ff;
      return;
    }
    control->restoredLeft = control->root.base.left;
    topOrRight = control->root.base.right;
    rightOrBottom = control->root.base.bottom;
    control->restoredTop = control->root.base.top;
    control->restoredBottom = rightOrBottom;
    control->restoredRight = topOrRight;
    framebufferHeight = g_FramebufferHeight;
    control->root.base.right = g_FramebufferWidth;
    control->root.base.left = 0;
    control->root.base.top = 0;
    control->root.base.bottom = framebufferHeight;
    UiContainer_LayoutWithOptionalWindowHeaderOffset(control);
  }
  UiNode_InvalidateRoot((UiNodeBase *)control);
  control->root.rootFlags = control->root.rootFlags & 0xe787ff;
  return;
}


/* Address: 0x004B5770.
   Ownership: ui/controls/layout.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B4CC0[12]@004B4CC0.
   Local calls: UiContainer_LayoutWithOptionalWindowHeaderOffset, UiRootStack_InvalidateAll.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime], UiActionQueue_Enqueue [ui/core/runtime],
   UiNode_DefaultKeyboardEventMoveFocusNext [ui/controls/input].
*/
bool __thandor_cf_preserve_eax_ecx_edx
UiResizableWindowControl_HandleWindowHotkeys
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiResizableWindowControl *control)

{
  int32_t topOrRight;
  int32_t rightOrBottom;
  int32_t restoredBottom;
  uint32_t framebufferHeight;
  bool delegateResult;
  
  if ((keyboardStateMask & 0x30) != 0) {
    if (((control->root.rootFlags & 8) != 0) && (keyCode == 99)) {
      UiActionQueue_Enqueue(0,control);
      return false;
    }
    if (((control->root.rootFlags & 0x10) != 0) && (keyCode == 0x7a)) {
      control->root.rootFlags = control->root.rootFlags ^ 0x80;
      if ((control->root.rootFlags & 0x80) == 0) {
        topOrRight = control->restoredTop;
        control->root.base.left = control->restoredLeft;
        rightOrBottom = control->restoredRight;
        restoredBottom = control->restoredBottom;
        control->root.base.top = topOrRight;
        control->root.base.bottom = restoredBottom;
        control->root.base.right = rightOrBottom;
        UiContainer_LayoutWithOptionalWindowHeaderOffset(control);
        UiRootStack_InvalidateAll();
      }
      else {
        control->restoredLeft = control->root.base.left;
        topOrRight = control->root.base.right;
        rightOrBottom = control->root.base.bottom;
        control->restoredTop = control->root.base.top;
        control->restoredBottom = rightOrBottom;
        control->restoredRight = topOrRight;
        framebufferHeight = g_FramebufferHeight;
        control->root.base.right = g_FramebufferWidth;
        control->root.base.left = 0;
        control->root.base.top = 0;
        control->root.base.bottom = framebufferHeight;
        UiContainer_LayoutWithOptionalWindowHeaderOffset(control);
        UiNode_InvalidateRoot((UiNodeBase *)control);
      }
      return false;
    }
  }
  delegateResult = UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,(UiNodeBase *)control);
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
  int64_t edgeAnchorPixelProductQ31;
  UiRootNode *oldFrontRoot;
  int64_t currentAnchorPixelProductQ31;
  int64_t anchorPixelProductQ31;
  
  anchorPixelProductQ31 = (uint64_t)g_FramebufferWidth * (uint64_t)(root->base).rightAnchorQ31;
  (root->base).right =
       ((int)((uint64_t)anchorPixelProductQ31 >> 0x20) << 1 | (uint32_t)anchorPixelProductQ31 >> 0x1f)
       + (root->base).rightOffset;
  currentAnchorPixelProductQ31 =
       (uint64_t)g_FramebufferHeight * (uint64_t)(root->base).bottomAnchorQ31;
  (root->base).bottom =
       ((int)((uint64_t)currentAnchorPixelProductQ31 >> 0x20) << 1 |
       (uint32_t)currentAnchorPixelProductQ31 >> 0x1f) + (root->base).bottomOffset;
  edgeAnchorPixelProductQ31 = (uint64_t)g_FramebufferWidth * (uint64_t)(root->base).leftAnchorQ31;
  (root->base).left =
       ((int)((uint64_t)edgeAnchorPixelProductQ31 >> 0x20) << 1 | (uint32_t)edgeAnchorPixelProductQ31 >> 0x1f) + (root->base).leftOffset;
  edgeAnchorPixelProductQ31 = (uint64_t)g_FramebufferHeight * (uint64_t)(root->base).topAnchorQ31;
  (root->base).top =
       ((int)((uint64_t)edgeAnchorPixelProductQ31 >> 0x20) << 1 | (uint32_t)edgeAnchorPixelProductQ31 >> 0x1f) + (root->base).topOffset;
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
   Purpose: Finds the containing root and invokes callbacks->vetoClose when present. CF set vetoes removal and
   invalidates the previous root; CF clear restores the previous root, focus, capture state, and redraw state.
   Local calls: UiRootStack_InvalidateAll.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime], UiKeyboardFocus_SelectInitial [ui/controls/input].
*/
bool __thandor_cf_preserve_eax_ecx_edx UiRootStack_Pop(UiRootNode *root)

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
  if (root->callbacks->vetoClose != (UiRootCloseCallback *)0x0) {
    closeCallbackVetoed = root->callbacks->vetoClose(root);
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
          (UiSerializedRelocationDelta relocationDelta,UiWindowControl *control)

{
  int frameInset;
  
  frameInset = g_UiWindowFrameInset;
  if ((control->selectable.stateFlags & 4) != 0) {
    control->selectable.base.leftOffset = control->selectable.base.leftOffset - g_UiWindowFrameInset;
    control->selectable.base.topOffset = control->selectable.base.topOffset - frameInset;
    control->selectable.base.rightOffset = control->selectable.base.rightOffset + frameInset;
    control->selectable.base.bottomOffset = control->selectable.base.bottomOffset + frameInset;
  }
  UiContainer_RelocateChildren(relocationDelta,(UiNodeBase *)control);
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
UiTitledWindowControl_LayoutFrameTitleAndChildren(UiTitledWindowControl *control)

{
  uint32_t leftInset;
  uint32_t titleHeightOrRightInset;
  uint32_t topInset;
  uint32_t bottomInset;
  RichTextExtentRegs titleExtent;
  TextResolveResult titleText;
  TextureSizeResult cornerSize;
  
  titleText = TextResource_Resolve(control->titleTextResourceId);
  titleExtent = RichTextCommandStream_MeasureRegs(g_UiWindowTitleTextStyle,titleText.text);
  titleHeightOrRightInset = titleExtent.heightPixels;
  cornerSize = g_GraphicsTextureSourceGetLogicalSize(0x36,g_UiWindowTextureSource);
  leftInset = cornerSize.logicalWidthPixels;
  topInset = cornerSize.logicalHeightPixels;
  if ((int)cornerSize.logicalHeightPixels < (int)titleHeightOrRightInset) {
    topInset = titleHeightOrRightInset;
  }
  control->base.left = control->base.left + leftInset;
  control->base.top = control->base.top + topInset;
  cornerSize = g_GraphicsTextureSourceGetLogicalSize(0x39,g_UiWindowTextureSource);
  bottomInset = cornerSize.logicalHeightPixels;
  titleHeightOrRightInset = cornerSize.logicalWidthPixels;
  control->base.right = control->base.right - titleHeightOrRightInset;
  control->base.bottom = control->base.bottom - bottomInset;
  UiContainer_LayoutChildren((UiNodeBase *)control);
  control->base.right = control->base.right + titleHeightOrRightInset;
  control->base.bottom = control->base.bottom + bottomInset;
  control->base.layoutWidth = control->base.layoutWidth + titleHeightOrRightInset;
  control->base.layoutHeight = control->base.layoutHeight + bottomInset;
  control->base.left = control->base.left - leftInset;
  control->base.top = control->base.top - topInset;
  control->base.layoutWidth = control->base.layoutWidth + leftInset;
  control->base.layoutHeight = control->base.layoutHeight + topInset;
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
          UiPixelCoordinate clipRight,UiHorizontalGaugeControl *control)

{
  uint64_t scaledFillProduct;
  uint32_t leftCapWidth;
  uint32_t clampedValue;
  uint32_t progressOrRange;
  int rightCapXOrFillMin;
  uint32_t rangeProgressOrPercent;
  int fillEndX;
  uint32_t divisionRemainder;
  uint16_t *commandStream;
  bool beginAccessFailed;
  TextureSizeResult textureSize;
  
  beginAccessFailed = g_GraphicsFramebufferBeginAccess();
  if (!beginAccessFailed) {
    rightCapXOrFillMin = control->base.layoutWidth;
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipTop,clipLeft,clipBottom,clipRight,control->base.top,control->base.left,0x7c,
               g_UiWindowTextureSource,g_FramebufferAccess);
    textureSize = g_GraphicsTextureSourceGetLogicalSize(0x7c,g_UiWindowTextureSource);
    leftCapWidth = textureSize.logicalWidthPixels;
    rightCapXOrFillMin = rightCapXOrFillMin - leftCapWidth;
    UiWindow_BlitTiledHorizontalEdge
              (clipTop,clipLeft,clipBottom,clipRight,0x7d,rightCapXOrFillMin,0,leftCapWidth,&control->base);
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipTop,clipLeft,clipBottom,clipRight,control->base.top,rightCapXOrFillMin + control->base.left,0x7e,
               g_UiWindowTextureSource,g_FramebufferAccess);
    clampedValue = control->value;
    if (control->maximumValue < clampedValue) {
      clampedValue = control->maximumValue;
    }
    progressOrRange = clampedValue - control->minimumValue;
    rangeProgressOrPercent = 0;
    if (progressOrRange != 0 && (int)control->minimumValue <= (int)clampedValue) {
      scaledFillProduct = (uint64_t)progressOrRange * (uint64_t)(rightCapXOrFillMin - leftCapWidth);
      rangeProgressOrPercent = control->maximumValue - control->minimumValue;
      if (rangeProgressOrPercent == 0) {
        rangeProgressOrPercent = 1;
      }
      divisionRemainder = (uint32_t)(scaledFillProduct % (uint64_t)rangeProgressOrPercent);
      textureSize = g_GraphicsTextureSourceGetLogicalSize(0x7f,g_UiWindowTextureSource);
      fillEndX = ((int)(scaledFillProduct / rangeProgressOrPercent) + (uint32_t)CARRY4(divisionRemainder,divisionRemainder) + leftCapWidth) -
                  textureSize.logicalWidthPixels;
      rightCapXOrFillMin = textureSize.logicalWidthPixels + leftCapWidth;
      rangeProgressOrPercent = progressOrRange;
      if (rightCapXOrFillMin <= fillEndX) {
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x80,fillEndX,0,rightCapXOrFillMin,&control->base);
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,control->base.top,leftCapWidth + control->base.left,0x7f,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,control->base.top,fillEndX + control->base.left,0x81
                   ,g_UiWindowTextureSource,g_FramebufferAccess);
      }
    }
    if ((control->gaugeFlags & 1) != 0) {
      progressOrRange = control->maximumValue - control->minimumValue;
      if (progressOrRange == 0) {
        progressOrRange = 1;
      }
      divisionRemainder = (uint32_t)(((uint64_t)rangeProgressOrPercent * 100) % (uint64_t)progressOrRange);
      rangeProgressOrPercent = (int)(((uint64_t)rangeProgressOrPercent * 100) / (uint64_t)progressOrRange) + (uint32_t)CARRY4(divisionRemainder,divisionRemainder);
      if (rangeProgressOrPercent == 100) {
        g_UiWindowPercentTextUtf16[0] = 0x31;
        g_UiWindowPercentTextUtf16[1] = 0x30;
        g_UiWindowPercentTextUtf16[2] = 0x30;
        g_UiWindowPercentTextUtf16[3] = 0x25;
        g_UiWindowPercentTextUtf16[4] = 0;
      }
      else {
        g_UiWindowPercentTextUtf16[1] = (short)((uint64_t)rangeProgressOrPercent % 10) + 0x30;
        g_UiWindowPercentTextUtf16[0] = (short)((uint64_t)rangeProgressOrPercent / 10) + 0x30;
        g_UiWindowPercentTextUtf16[2] = 0x25;
        g_UiWindowPercentTextUtf16[3] = 0;
      }
      commandStream = g_UiWindowPercentTextUtf16;
      if (g_UiWindowPercentTextUtf16[0] == 0x30) {
        commandStream = g_UiWindowPercentTextUtf16 + 1;
      }
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,g_UiHorizontalGaugeLabelTextStyle,
                 commandStream,g_UiHorizontalGaugeLabelTopInset + control->base.top,
                 ((uint32_t)control->base.layoutWidth >> 1) + control->base.left);
    }
    g_GraphicsFramebufferEndAccess();
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
          (UiSerializedRelocationDelta relocationDelta,UiPageStackControl *control)

{
  UiPageCount remainingCount;
  UiNodeBase **childSlot;
  UiNodeBase **childPointerCursor;
  
  childSlot = &control->pages;
  remainingCount = control->pageCount;
  do {
    if (*childSlot != (UiNodeBase *)0xffffffff) {
      *childSlot = (UiNodeBase *)((int)&(*childSlot)->nextSibling + relocationDelta);
    }
    childSlot = childSlot + 1;
    remainingCount = remainingCount - 1;
  } while (remainingCount != 0);
  childPointerCursor = &control->pages;
  remainingCount = control->pageCount;
  do {
    control->base.firstChild = *childPointerCursor;
    UiContainer_RelocateChildren(relocationDelta,&control->base);
    childPointerCursor = childPointerCursor + 1;
    remainingCount = remainingCount - 1;
  } while (remainingCount != 0);
  control->base.firstChild = control->pages;
  return;
}


/* Address: 0x004B4700.
   Ownership: ui/controls/layout.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B4650[3]@004B4650.
   Local calls: UiContainer_LayoutChildren.
*/
void __thandor_void_preserve_eax_ecx_edx
UiLayoutContainerControl_LayoutChildren(UiPageStackControl *control)

{
  UiPageCount remainingCount;
  UiNodeBase **childPointerCursor;
  UiNodeBase *savedFirstChild;
  
  childPointerCursor = &control->pages;
  savedFirstChild = control->base.firstChild;
  remainingCount = control->pageCount;
  do {
    control->base.firstChild = *childPointerCursor;
    UiContainer_LayoutChildren(&control->base);
    childPointerCursor = childPointerCursor + 1;
    remainingCount = remainingCount - 1;
  } while (remainingCount != 0);
  control->base.firstChild = savedFirstChild;
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
UiLayoutContainerControl_SuppressActionIdRecursive(UiActionId actionId,UiPageStackControl *control)

{
  UiPageCount remainingCount;
  UiNodeBase **childPointerCursor;
  UiNodeBase *savedFirstChild;
  
  childPointerCursor = &control->pages;
  savedFirstChild = control->base.firstChild;
  remainingCount = control->pageCount;
  do {
    control->base.firstChild = *childPointerCursor;
    UiContainer_SuppressActionId(actionId,&control->base);
    childPointerCursor = childPointerCursor + 1;
    remainingCount = remainingCount - 1;
  } while (remainingCount != 0);
  control->base.firstChild = savedFirstChild;
  return;
}


/* Address: 0x004B4800.
   Ownership: ui/controls/layout.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B4650[15]@004B4650.
   Local calls: UiContainer_UnsuppressActionId.
*/
void __thandor_void_preserve_eax_ecx_edx
UiLayoutContainerControl_UnsuppressActionIdRecursive(UiActionId actionId,UiPageStackControl *control)

{
  UiPageCount remainingCount;
  UiNodeBase **childPointerCursor;
  UiNodeBase *savedFirstChild;
  
  childPointerCursor = &control->pages;
  savedFirstChild = control->base.firstChild;
  remainingCount = control->pageCount;
  do {
    control->base.firstChild = *childPointerCursor;
    UiContainer_UnsuppressActionId(actionId,&control->base);
    childPointerCursor = childPointerCursor + 1;
    remainingCount = remainingCount - 1;
  } while (remainingCount != 0);
  control->base.firstChild = savedFirstChild;
  return;
}


/* Address: 0x004B4D10.
   Ownership: ui/controls/layout.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B4CC0[0]@004B4CC0.
   Local calls: UiContainer_RelocateChildren.
*/
void __thandor_void_preserve_eax_ecx_edx
UiResizableWindowControl_RelocateAndRefreshInteractionState
          (UiSerializedRelocationDelta relocationDelta,UiResizableWindowControl *control)

{
  UiContainer_RelocateChildren(relocationDelta,(UiNodeBase *)control);
  if ((control->root.rootFlags & 0x18) == 0) {
    control->root.base.nodeFlags =
         control->root.base.nodeFlags & ~(UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_PREFERRED_FOCUS_TARGET);
  }
  else {
    control->root.base.nodeFlags = control->root.base.nodeFlags | UI_NODE_FALLBACK_FOCUS_TARGET;
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
          UiResizableWindowControl *control)

{
  uint32_t interactionFlags;
  int32_t newRight;
  int32_t newBottom;
  int offsetXOrEdge;
  int newLeftOrOldBottom;
  int offsetYOrEdge;
  int newTopOrOldRight;
  bool overButton;
  TextureSizeResult buttonSize;
  
  offsetXOrEdge = pointerX - control->root.base.left;
  offsetYOrEdge = pointerY - control->root.base.top;
  if ((control->root.rootFlags & 0x2000) == 0) {
    if ((control->root.rootFlags & 0x800) == 0) {
      if ((control->root.rootFlags & 0x1000) == 0) {
        if ((control->root.rootFlags & 0x4000) != 0) {
          interactionFlags = control->root.rootFlags;
          offsetXOrEdge = offsetXOrEdge + control->root.base.left;
          offsetYOrEdge = offsetYOrEdge + control->root.base.top;
          newTopOrOldRight = offsetYOrEdge;
          if ((interactionFlags & 0x83000000) == 0) {
            newTopOrOldRight = control->root.base.top;
          }
          if ((interactionFlags & 0x38000000) == 0) {
            offsetYOrEdge = control->root.base.bottom;
          }
          newLeftOrOldBottom = offsetXOrEdge;
          if ((interactionFlags & 0xe0000000) == 0) {
            newLeftOrOldBottom = control->root.base.left;
          }
          if ((interactionFlags & 0xe000000) == 0) {
            offsetXOrEdge = control->root.base.right;
          }
          control->dragAnchorXOrPendingRight = offsetXOrEdge;
          control->dragAnchorYOrPendingBottom = offsetYOrEdge;
          offsetXOrEdge = (offsetXOrEdge - newLeftOrOldBottom) + -0x40;
          if (offsetXOrEdge < 0) {
            if ((interactionFlags & 0xe000000) != 0) {
              control->dragAnchorXOrPendingRight = control->dragAnchorXOrPendingRight - offsetXOrEdge;
            }
            if ((interactionFlags & 0xe0000000) != 0) {
              newLeftOrOldBottom = newLeftOrOldBottom + offsetXOrEdge;
            }
          }
          offsetXOrEdge = (offsetYOrEdge - newTopOrOldRight) + -0x40;
          if (offsetXOrEdge < 0) {
            if ((interactionFlags & 0x38000000) != 0) {
              control->dragAnchorYOrPendingBottom = control->dragAnchorYOrPendingBottom - offsetXOrEdge;
            }
            if ((interactionFlags & 0x83000000) != 0) {
              newTopOrOldRight = newTopOrOldRight + offsetXOrEdge;
            }
          }
          newRight = control->dragAnchorXOrPendingRight;
          newBottom = control->dragAnchorYOrPendingBottom;
          LOCK();
          offsetXOrEdge = control->root.base.left;
          control->root.base.left = newLeftOrOldBottom;
          UNLOCK();
          LOCK();
          offsetYOrEdge = control->root.base.top;
          control->root.base.top = newTopOrOldRight;
          UNLOCK();
          LOCK();
          newTopOrOldRight = control->root.base.right;
          control->root.base.right = newRight;
          UNLOCK();
          LOCK();
          newLeftOrOldBottom = control->root.base.bottom;
          control->root.base.bottom = newBottom;
          UNLOCK();
          if ((((offsetXOrEdge != control->root.base.left) || (offsetYOrEdge != control->root.base.top)) || (newTopOrOldRight != control->root.base.right))
             || (newLeftOrOldBottom != control->root.base.bottom)) {
            UiRootStack_InvalidateAll();
            control->root.base.vtable->layout((UiNodeBase *)control);
            UiNode_InvalidateRoot((UiNodeBase *)control);
          }
        }
      }
      else {
        newTopOrOldRight = control->root.base.layoutWidth;
        buttonSize = g_GraphicsTextureSourceGetLogicalSize(4,g_UiWindowTextureSource);
        overButton = g_GraphicsTextureSourceTestOpaquePixel
                          (offsetYOrEdge,offsetXOrEdge,0,newTopOrOldRight - buttonSize.logicalWidthPixels,4,g_UiWindowTextureSource)
        ;
        if (overButton) {
          if ((control->root.rootFlags & 0x100000) != 0) {
            return;
          }
          control->root.rootFlags = control->root.rootFlags | 0x100000;
        }
        else {
          if ((control->root.rootFlags & 0x100000) == 0) {
            return;
          }
          control->root.rootFlags = control->root.rootFlags & 0xffefffff;
        }
        UiNode_InvalidateRoot((UiNodeBase *)control);
      }
    }
    else {
      overButton = g_GraphicsTextureSourceTestOpaquePixel(offsetYOrEdge,offsetXOrEdge,0,0,1,g_UiWindowTextureSource);
      if (overButton) {
        if ((control->root.rootFlags & 0x80000) != 0) {
          return;
        }
        control->root.rootFlags = control->root.rootFlags | 0x80000;
      }
      else {
        if ((control->root.rootFlags & 0x80000) == 0) {
          return;
        }
        control->root.rootFlags = control->root.rootFlags & 0xfff7ffff;
      }
      g_GraphicsTextureSourceGetLogicalSize(1,g_UiWindowTextureSource);
      UiNode_InvalidateRoot((UiNodeBase *)control);
    }
  }
  else {
    offsetXOrEdge = offsetXOrEdge - control->dragAnchorXOrPendingRight;
    offsetYOrEdge = offsetYOrEdge - control->dragAnchorYOrPendingBottom;
    if ((offsetYOrEdge != 0) || (offsetXOrEdge != 0)) {
      UiRootStack_InvalidateAll();
      control->root.base.left = control->root.base.left + offsetXOrEdge;
      control->root.base.top = control->root.base.top + offsetYOrEdge;
      control->root.base.right = control->root.base.right + offsetXOrEdge;
      control->root.base.bottom = control->root.base.bottom + offsetYOrEdge;
      control->root.base.vtable->layout((UiNodeBase *)control);
      UiNode_InvalidateRoot((UiNodeBase *)control);
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
  int32_t *edgeField;
  int savedLeft;
  int savedTop;
  int savedRight;
  int savedBottom;
  UiNodeBase *parentNode;
  int32_t parentTop;
  int32_t parentRight;
  int32_t parentBottom;
  
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
   Discards all buffered keyboard and pointer input and the frame ticks that piled up, so a UI loop that starts
   (or resumes after a movie, session or error box) neither reacts to stale input nor catches up on old ticks.
*/
void __thandor_void_preserve_eax_ecx_edx UiFrame_FlushInputAndResetPendingTicks(void)

{
  g_KeyboardFlushEvents();
  g_PointerFlushEvents();
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
    g_GraphicsFramebufferPresent(g_FramebufferAccess);
  } while (g_UiRootNode != (UiRootNode *)0xffffffff);
  g_UiTooltipState.targetNode = (UiNodeBase *)0x0;
  UiTooltip_Draw(g_FramebufferHeight,g_FramebufferWidth,0,0);
  g_GraphicsFramebufferPresent(g_FramebufferAccess);
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
  UiRootNode *nextRootLink;
  UiRootNode *detachedPreviousRoot;
  UiNodeBase *nextFrontRootLink;

  oldFrontRoot = g_UiRootNode;
  nextRootLink = (UiRootNode *)(root->base).nextSibling;
  belowRoot = root->previousRoot;
  if (nextRootLink != (UiRootNode *)0xffffffff) {
    nextRootLink->previousRoot = belowRoot;
  }
  if (belowRoot != (UiRootNode *)0xffffffff) {
    (belowRoot->base).nextSibling = &nextRootLink->base;
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
   Cross-module calls: TextResourcePage_Load [assets/text/resources], UiActionHandlers_SetPage [ui/core/runtime].
*/
void __thandor_preserve_eax UiWindowResources_Init(void)

{
  TextureSourceLoadResult loadResult;
  FatalErrorCheckResult checkedResult;
  TextPageLoadResult pageLoadResult;
  
  loadResult = g_GraphicsTextureSourceLoadPackageAsset((uint16_t *)u_engine_win_gfx_004b0f06);
  checkedResult = FatalError_ExitIfFailed((uint32_t)loadResult.textureSource,loadResult.failed);
  g_UiWindowTextureSource = (GraphicsTextureSourceAsset *)checkedResult.valueOrError;
  loadResult = g_GraphicsTextureSourceLoadPackageAsset(u_engine_winclass_gfx_004b0eb8);
  checkedResult = FatalError_ExitIfFailed((uint32_t)loadResult.textureSource,loadResult.failed);
  g_UiWindowClassTextureSource = (GraphicsTextureSourceAsset *)checkedResult.valueOrError;
  pageLoadResult = TextResourcePage_Load(1,(uint16_t *)u_texte_winclass_str_004b0ee0);
  FatalError_ExitIfFailed(pageLoadResult.errorOrValue,pageLoadResult.failed);
  UiActionHandlers_SetPage(0,(UiActionHandlerPage *)&g_UiRootStackActionHandlerPage);
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
  int64_t edgeAnchorPixelProductQ31;
  UiRootNode *rootNode;
  int64_t currentAnchorPixelProductQ31;
  int64_t anchorPixelProductQ31;
  
  rootNode = g_UiRootNode;
  do {
    anchorPixelProductQ31 =
         (uint64_t)g_FramebufferWidth * (uint64_t)(rootNode->base).rightAnchorQ31;
    (rootNode->base).right =
         ((int)((uint64_t)anchorPixelProductQ31 >> 0x20) << 1 | (uint32_t)anchorPixelProductQ31 >> 0x1f
         ) + (rootNode->base).rightOffset;
    currentAnchorPixelProductQ31 =
         (uint64_t)g_FramebufferHeight * (uint64_t)(rootNode->base).bottomAnchorQ31;
    (rootNode->base).bottom =
         ((int)((uint64_t)currentAnchorPixelProductQ31 >> 0x20) << 1 |
         (uint32_t)currentAnchorPixelProductQ31 >> 0x1f) + (rootNode->base).bottomOffset;
    edgeAnchorPixelProductQ31 = (uint64_t)g_FramebufferWidth * (uint64_t)(rootNode->base).leftAnchorQ31;
    (rootNode->base).left =
         ((int)((uint64_t)edgeAnchorPixelProductQ31 >> 0x20) << 1 | (uint32_t)edgeAnchorPixelProductQ31 >> 0x1f) + (rootNode->base).leftOffset;
    edgeAnchorPixelProductQ31 = (uint64_t)g_FramebufferHeight * (uint64_t)(rootNode->base).topAnchorQ31;
    (rootNode->base).top =
         ((int)((uint64_t)edgeAnchorPixelProductQ31 >> 0x20) << 1 | (uint32_t)edgeAnchorPixelProductQ31 >> 0x1f) + (rootNode->base).topOffset;
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
          (UiNodeFlagMask setMask,UiNodeFlagMask retainMask,UiPageStackControl *control)

{
  UiPageCount remainingCount;
  UiNodeBase **childPointerCursor;
  UiNodeBase *savedFirstChild;
  
  childPointerCursor = &control->pages;
  savedFirstChild = control->base.firstChild;
  remainingCount = control->pageCount;
  do {
    control->base.firstChild = *childPointerCursor;
    UiNode_ApplyFlagsRecursive(setMask,retainMask,&control->base);
    childPointerCursor = childPointerCursor + 1;
    remainingCount = remainingCount - 1;
  } while (remainingCount != 0);
  control->base.firstChild = savedFirstChild;
  return;
}


/* Address: 0x004B5120.
   Ownership: ui/controls/layout.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B4CC0[4]@004B4CC0.
*/
void __thandor_void_preserve_eax_ecx_edx
UiResizableWindowControl_BeginMoveResizeOrWindowAction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiResizableWindowControl *control)

{
  int localX;
  uint32_t localY;
  uint32_t resizeFlags;
  int extentLimit;
  bool hitOpaque;
  TextureSizeResult textureSize;
  
  localX = pointerX - control->root.base.left;
  localY = pointerY - control->root.base.top;
  if (((control->root.rootFlags & 8) != 0) &&
     (hitOpaque = g_GraphicsTextureSourceTestOpaquePixel(localY,localX,0,0,1,g_UiWindowTextureSource),
     hitOpaque)) {
    control->root.rootFlags = control->root.rootFlags | 0x800;
    return;
  }
  if ((control->root.rootFlags & 0x10) != 0) {
    extentLimit = control->root.base.layoutWidth;
    textureSize = g_GraphicsTextureSourceGetLogicalSize(4,g_UiWindowTextureSource);
    hitOpaque = g_GraphicsTextureSourceTestOpaquePixel
                      (localY,localX,0,extentLimit - textureSize.logicalWidthPixels,4,g_UiWindowTextureSource);
    if (hitOpaque) {
      control->root.rootFlags = control->root.rootFlags | 0x1000;
      return;
    }
  }
  if ((control->root.rootFlags & 0x80) == 0) {
    if ((control->root.rootFlags & 0x40) != 0) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize(0x10,g_UiWindowTextureSource);
      if ((((localX < (int)textureSize.logicalWidthPixels) || ((int)localY < (int)textureSize.logicalHeightPixels))
          || ((int)(control->root.base.layoutWidth - textureSize.logicalWidthPixels) <= localX)) ||
         ((int)(control->root.base.layoutHeight - textureSize.logicalHeightPixels) <= (int)localY)) {
        extentLimit = control->root.base.layoutHeight - g_UiWindowResizeBorderThickness;
        if (localX < g_UiWindowResizeBorderThickness) {
          resizeFlags = 0x80004000;
          if ((g_UiWindowResizeBorderThickness <= (int)localY) &&
             (resizeFlags = 0x20004000, (int)localY < extentLimit)) {
            resizeFlags = 0x40004000;
          }
        }
        else if (localX < control->root.base.layoutWidth - g_UiWindowResizeBorderThickness) {
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
        control->root.rootFlags = control->root.rootFlags & 0xffffff;
        control->root.rootFlags = control->root.rootFlags | resizeFlags;
        return;
      }
    }
    if (((control->root.rootFlags & 0x20) != 0) && (localY < g_UiWindowMoveHandleWidth)) {
      control->root.rootFlags = control->root.rootFlags | 0x2000;
      control->dragAnchorXOrPendingRight = localX;
      control->dragAnchorYOrPendingBottom = localY;
      g_GraphicsCursorSetFrame(1);
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
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiResizableWindowControl *control)

{
  GraphicsCursorFrameIndex cursorFrame;
  int bottomBorderY;
  int localY;
  int localX;
  TextureSizeResult cornerSize;
  
  cursorFrame = 0;
  if (((control->root.rootFlags & 0x40) != 0) && ((control->root.rootFlags & 0x80) == 0))
  {
    localX = pointerX - control->root.base.left;
    if ((control->root.base.left <= pointerX) &&
       (((localY = pointerY - control->root.base.top, control->root.base.top <= pointerY &&
         (localX < control->root.base.layoutWidth)) && (localY < control->root.base.layoutHeight)))) {
      cornerSize = g_GraphicsTextureSourceGetLogicalSize(0x10,g_UiWindowTextureSource);
      if ((((localX < (int)cornerSize.logicalWidthPixels) || (localY < (int)cornerSize.logicalHeightPixels)) ||
          ((int)(control->root.base.layoutWidth - cornerSize.logicalWidthPixels) <= localX)) ||
         (cursorFrame = 0, (int)(control->root.base.layoutHeight - cornerSize.logicalHeightPixels) <= localY)) {
        bottomBorderY = control->root.base.layoutHeight - g_UiWindowResizeBorderThickness;
        if (localX < g_UiWindowResizeBorderThickness) {
          cursorFrame = 2;
          if ((g_UiWindowResizeBorderThickness <= localY) && (cursorFrame = 3, localY < bottomBorderY)) {
            return 5;
          }
        }
        else {
          if (localX < control->root.base.layoutWidth - g_UiWindowResizeBorderThickness) {
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
  uint32_t columnCount;
  uint32_t rowCount;
  
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
  /* EDX:EAX = rows:columns */
  return ((UiGridDimensionsEdxEax8)rowCount << 32) | (UiGridDimensionsEdxEax8)columnCount;
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
  return ((UiGridDimensionsEdxEax8)itemCount << 32) | 1;
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
    childNode->vtable->suppressActionId(actionId,childNode);
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
    childNode->vtable->unsuppressActionId(actionId,childNode);
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
    firstNode->vtable->relocate(imageDelta,firstNode);
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
   Purpose: Lays out the resizable window's children. When window flag 0x04 (title bar) is set, it temporarily offsets top by the
   logical height of window subresource 0x0C, restores top, and adds that height to layoutHeight.
   Local calls: UiContainer_LayoutChildren.
*/
void __thandor_void_preserve_eax_ecx_edx
UiContainer_LayoutWithOptionalWindowHeaderOffset(UiResizableWindowControl *control)

{
  uint32_t headerHeight;
  TextureSizeResult headerSize;
  
  if ((control->root.rootFlags & 4) == 0) {
    UiContainer_LayoutChildren((UiNodeBase *)control);
  }
  else {
    headerSize = g_GraphicsTextureSourceGetLogicalSize(0xc,g_UiWindowTextureSource);
    headerHeight = headerSize.logicalHeightPixels;
    control->root.base.top = control->root.base.top + headerHeight;
    UiContainer_LayoutChildren((UiNodeBase *)control);
    control->root.base.top = control->root.base.top - headerHeight;
    control->root.base.layoutHeight = control->root.base.layoutHeight + headerHeight;
  }
  return;
}


/* Address: 0x004AF680.
   One UI frame step under the UI frame lock: pumps Win32 messages, then runs every pending frame tick
   (sprite-button animations and frame callback of the front root, tick of the pointer-capture and
   keyboard-focus nodes, tooltip countdown) and refreshes the DirectInput mouse every 48 calls.
*/
void __thandor_void_preserve_eax_ecx UiFrame_Update(UiStopMessageCode stopMessageCode)

{
  uint32_t ticksToRun;
  UiRootNode *frontRoot;
  UiRootCallbacks *rootCallbacks;

  g_SpinLockAcquire(g_UiRuntimeFrameLock);
  /* The original zeroes EAX before the loop and Win32_PumpMessages preserves EAX, so the value
     compared here is always 0: pump until a frame tick is pending, or once when stopMessageCode
     is 0 (every caller passes 0). The pending tick count is then consumed (reset to 0). */
  do {
    g_Win32PumpMessages();
  } while ((stopMessageCode != 0) && (g_UiPendingFrameTicks == 0));
  ticksToRun = g_UiPendingFrameTicks;
  g_UiPendingFrameTicks = 0;
  for (; ticksToRun != 0; ticksToRun--) {
    frontRoot = g_UiRootNode;
    if (frontRoot != UI_ROOT_STACK_END) {
      UiTree_AdvanceSpriteButtonAnimations(&frontRoot->base);
      rootCallbacks = frontRoot->callbacks;
      if (rootCallbacks->frameUpdate != NULL) {
        rootCallbacks->frameUpdate(frontRoot);
      }
    }
    if (g_UiPointerCaptureTarget != (UiNodeBase *)0xffffffff) {
      g_UiPointerCaptureTarget->vtable->tick(g_UiPointerCaptureTarget);
    }
    /* the focus node ticks only once when it also holds the pointer capture */
    if ((g_UiKeyboardFocusNode != (UiNodeBase *)0xffffffff) &&
       (g_UiKeyboardFocusNode != g_UiPointerCaptureTarget)) {
      g_UiKeyboardFocusNode->vtable->tick(g_UiKeyboardFocusNode);
    }
    UiTooltip_TickCountdown();
  }
  g_DirectInputMouseRefreshCountdown--;
  if (g_DirectInputMouseRefreshCountdown == 0) {
    g_DirectInputMouseRefreshCountdown = 48;
    DirectInputMouse_RefreshDeviceIfIdle();
  }
  g_SpinLockReleaseAndInvoke
            ((SpinLockReleaseCallbackProc *)g_UiRuntimePostUnlockCallback,g_UiRuntimeFrameLock);
  return;
}


/* Address: 0x004AF7E0.
   Draws the UI root stack from the bottom root up to the front root, each clipped to its rectangle within
   the framebuffer, and the tooltip on top.
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
  int clipLeft;
  int clipTop;
  int clipRight;
  int clipBottom;

  if (g_UiRootNode == UI_ROOT_STACK_END) {
    return;
  }
  count = 0;
  for (root = g_UiRootNode; (root != UI_ROOT_STACK_END) && (count < ROOT_LIMIT);
       root = root->previousRoot) {
    roots[count] = root;
    count++;
  }
  while (count != 0) {
    count--;
    root = roots[count];
    clipLeft = (root->base).left;
    clipTop = (root->base).top;
    clipRight = (root->base).right;
    clipBottom = (root->base).bottom;
    if (clipLeft < 0) {
      clipLeft = 0;
    }
    if (clipTop < 0) {
      clipTop = 0;
    }
    if ((int)g_FramebufferWidth < clipRight) {
      clipRight = g_FramebufferWidth;
    }
    if ((int)g_FramebufferHeight < clipBottom) {
      clipBottom = g_FramebufferHeight;
    }
    if ((clipLeft < clipRight) && (clipTop < clipBottom)) {
      /* drawClipped takes (bottom, right, top, left, node); the draw methods name these parameters
         clipTop, clipLeft, clipBottom, clipRight. */
      (*((root->base).vtable)->drawClipped)(clipBottom,clipRight,clipTop,clipLeft,&root->base);
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
    hit = eligible[count]->vtable->hitTest(pointerY,pointerX,eligible[count]);
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
  g_GraphicsTextureSourceBlitTiledSourceAlpha
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
    childNode->vtable->relocate(relocationDelta,childNode);
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
      childNode->vtable->drawClipped(clipTop,clipLeft,clipBottom,clipRight,childNode);
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
  g_GraphicsTextureSourceBlitTiledSourceAlpha
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
  int64_t edgeAnchorPixelProductQ31;
  int computedEdgeCoordinate;
  int currentEdgeCoordinate;
  int edgeCoordinate;
  int64_t currentAnchorPixelProductQ31;
  int64_t anchorPixelProductQ31;
  
  childNode = control->firstChild;
  control->layoutWidth = control->right - control->left;
  control->layoutHeight = control->bottom - control->top;
  for (; childNode != (UiNodeBase *)0xffffffff; childNode = childNode->nextSibling) {
    anchorPixelProductQ31 =
         (uint64_t)(uint32_t)control->layoutWidth * (uint64_t)childNode->rightAnchorQ31;
    computedEdgeCoordinate =
         ((int)((uint64_t)anchorPixelProductQ31 >> 0x20) << 1 | (uint32_t)anchorPixelProductQ31 >> 0x1f
         ) + childNode->rightOffset + control->left;
    childNode->right = computedEdgeCoordinate;
    childNode->layoutWidth = computedEdgeCoordinate;
    currentAnchorPixelProductQ31 =
         (uint64_t)(uint32_t)control->layoutHeight * (uint64_t)childNode->bottomAnchorQ31;
    currentEdgeCoordinate =
         ((int)((uint64_t)currentAnchorPixelProductQ31 >> 0x20) << 1 |
         (uint32_t)currentAnchorPixelProductQ31 >> 0x1f) + childNode->bottomOffset + control->top;
    childNode->bottom = currentEdgeCoordinate;
    childNode->layoutHeight = currentEdgeCoordinate;
    edgeAnchorPixelProductQ31 = (uint64_t)(uint32_t)control->layoutWidth * (uint64_t)childNode->leftAnchorQ31;
    edgeCoordinate = ((int)((uint64_t)edgeAnchorPixelProductQ31 >> 0x20) << 1 | (uint32_t)edgeAnchorPixelProductQ31 >> 0x1f) + childNode->leftOffset +
            control->left;
    childNode->left = edgeCoordinate;
    childNode->layoutWidth = childNode->layoutWidth - edgeCoordinate;
    edgeAnchorPixelProductQ31 = (uint64_t)(uint32_t)control->layoutHeight * (uint64_t)childNode->topAnchorQ31;
    edgeCoordinate = ((int)((uint64_t)edgeAnchorPixelProductQ31 >> 0x20) << 1 | (uint32_t)edgeAnchorPixelProductQ31 >> 0x1f) + childNode->topOffset +
            control->top;
    childNode->top = edgeCoordinate;
    childNode->layoutHeight = childNode->layoutHeight - edgeCoordinate;
    childNode->vtable->layout(childNode);
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
  g_GraphicsTextureSourceBlitTiledSourceAlpha
            (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,
             tileStart + *(int *)((int)node + 0x10),edgeY + *(int *)((int)node + 0x14),
             tileEnd + *(int *)((int)node + 0x10),subresource,g_UiWindowTextureSource,
             g_FramebufferAccess);
  return;
}


/* Address: 0x004B14D0.
   Marks the whole screen for redraw: drops the collected dirty rectangles and invalidates every root on the UI
   root stack, top to bottom. Does nothing while invalidation is suppressed.
*/
void __thandor_void_preserve_ecx_edx UiRootStack_InvalidateAll(void)

{
  UiRootNode *root;

  if (g_UiInvalidationSuppressed == 0) {
    g_UiDirtyRectCount = 0;
    for (root = g_UiRootNode; root != UI_ROOT_STACK_END; root = root->previousRoot) {
      UiNode_InvalidateRoot(&root->base);
    }
  }
  return;
}

