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
   drawClipped of g_UiPanelControlVtable: draws the panel's optional tiled background (UI_ROOT_TILED_BACKGROUND)
   and frame (UI_ROOT_FRAME); UI_ROOT_ALTERNATE_BACKGROUND switches to the second background and adds the
   second frame on top. Then draws the children. A frame is four corners and four tiled edges between them.
*/
void UiPanelControl_DrawOptionalTiledBackgroundFrameAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPanelControl *control)

{
  uint32_t cornerWidth;
  GraphicsSubresourceIndex subresource;
  uint32_t cornerHeight;
  int bottomEdgeY;
  int rightEdgeX;
  bool beginAccessFailed;
  GraphicsTextureLogicalSize cornerSize;

  if ((control->root.rootFlags & (UI_ROOT_TILED_BACKGROUND | UI_ROOT_FRAME)) != 0) {
    beginAccessFailed = g_GraphicsFramebufferBeginAccess();
    if (!beginAccessFailed) {
      if ((control->root.rootFlags & UI_ROOT_TILED_BACKGROUND) != 0) {
        subresource = UI_WINDOW_SUBRESOURCE_WINDOW_INTERIOR;
        if ((control->root.rootFlags & UI_ROOT_ALTERNATE_BACKGROUND) != 0) {
          subresource = UI_WINDOW_SUBRESOURCE_ALTERNATE_INTERIOR;
        }
        UiWindow_BlitTiledInterior
                  (clipBottom,clipRight,clipTop,clipLeft,subresource,control->root.base.layoutHeight,
                   control->root.base.layoutWidth,0,0,control);
      }
      if ((control->root.rootFlags & UI_ROOT_FRAME) != 0) {
        /* the bottom-right corner gives the corner size */
        cornerSize = g_GraphicsTextureSourceGetLogicalSize
                               (UI_WINDOW_SUBRESOURCE_WINDOW_FRAME + UI_WINDOW_FRAME_BOTTOM_RIGHT,
                                g_UiWindowTextureSource);
        cornerHeight = cornerSize.logicalHeightPixels;
        cornerWidth = cornerSize.logicalWidthPixels;
        rightEdgeX = control->root.base.layoutWidth - cornerWidth;
        bottomEdgeY = control->root.base.layoutHeight - cornerHeight;
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipBottom,clipRight,clipTop,clipLeft,control->root.base.top,control->root.base.left,
                   UI_WINDOW_SUBRESOURCE_WINDOW_FRAME + UI_WINDOW_FRAME_TOP_LEFT,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipBottom,clipRight,clipTop,clipLeft,control->root.base.top,rightEdgeX + control->root.base.left,
                   UI_WINDOW_SUBRESOURCE_WINDOW_FRAME + UI_WINDOW_FRAME_TOP_RIGHT,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipBottom,clipRight,clipTop,clipLeft,bottomEdgeY + control->root.base.top,control->root.base.left,
                   UI_WINDOW_SUBRESOURCE_WINDOW_FRAME + UI_WINDOW_FRAME_BOTTOM_LEFT,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipBottom,clipRight,clipTop,clipLeft,bottomEdgeY + control->root.base.top,
                   rightEdgeX + control->root.base.left,
                   UI_WINDOW_SUBRESOURCE_WINDOW_FRAME + UI_WINDOW_FRAME_BOTTOM_RIGHT,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        UiWindow_BlitTiledHorizontalEdge
                  (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_WINDOW_FRAME + UI_WINDOW_FRAME_TOP,
                   rightEdgeX,0,cornerWidth,control);
        UiWindow_BlitTiledVerticalEdge
                  (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_WINDOW_FRAME + UI_WINDOW_FRAME_LEFT,
                   bottomEdgeY,cornerHeight,0,control);
        UiWindow_BlitTiledVerticalEdge
                  (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_WINDOW_FRAME + UI_WINDOW_FRAME_RIGHT,
                   bottomEdgeY,cornerHeight,rightEdgeX,control);
        UiWindow_BlitTiledHorizontalEdge
                  (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_WINDOW_FRAME + UI_WINDOW_FRAME_BOTTOM,
                   rightEdgeX,bottomEdgeY,cornerWidth,control);
      }
      if ((control->root.rootFlags & UI_ROOT_ALTERNATE_BACKGROUND) != 0) {
        cornerSize = g_GraphicsTextureSourceGetLogicalSize
                               (UI_WINDOW_SUBRESOURCE_ALTERNATE_FRAME + UI_WINDOW_FRAME_BOTTOM_RIGHT,
                                g_UiWindowTextureSource);
        cornerHeight = cornerSize.logicalHeightPixels;
        cornerWidth = cornerSize.logicalWidthPixels;
        rightEdgeX = control->root.base.layoutWidth - cornerWidth;
        bottomEdgeY = control->root.base.layoutHeight - cornerHeight;
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipBottom,clipRight,clipTop,clipLeft,control->root.base.top,control->root.base.left,
                   UI_WINDOW_SUBRESOURCE_ALTERNATE_FRAME + UI_WINDOW_FRAME_TOP_LEFT,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipBottom,clipRight,clipTop,clipLeft,control->root.base.top,rightEdgeX + control->root.base.left,
                   UI_WINDOW_SUBRESOURCE_ALTERNATE_FRAME + UI_WINDOW_FRAME_TOP_RIGHT,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipBottom,clipRight,clipTop,clipLeft,bottomEdgeY + control->root.base.top,control->root.base.left,
                   UI_WINDOW_SUBRESOURCE_ALTERNATE_FRAME + UI_WINDOW_FRAME_BOTTOM_LEFT,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipBottom,clipRight,clipTop,clipLeft,bottomEdgeY + control->root.base.top,
                   rightEdgeX + control->root.base.left,
                   UI_WINDOW_SUBRESOURCE_ALTERNATE_FRAME + UI_WINDOW_FRAME_BOTTOM_RIGHT,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        UiWindow_BlitTiledHorizontalEdge
                  (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_ALTERNATE_FRAME + UI_WINDOW_FRAME_TOP,
                   rightEdgeX,0,cornerWidth,control);
        UiWindow_BlitTiledVerticalEdge
                  (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_ALTERNATE_FRAME + UI_WINDOW_FRAME_LEFT,
                   bottomEdgeY,cornerHeight,0,control);
        UiWindow_BlitTiledVerticalEdge
                  (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_ALTERNATE_FRAME + UI_WINDOW_FRAME_RIGHT,
                   bottomEdgeY,cornerHeight,rightEdgeX,control);
        UiWindow_BlitTiledHorizontalEdge
                  (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_ALTERNATE_FRAME + UI_WINDOW_FRAME_BOTTOM,
                   rightEdgeX,bottomEdgeY,cornerWidth,control);
      }
      g_GraphicsFramebufferEndAccess();
    }
  }
  UiContainer_DrawIntersectingChildren(clipBottom,clipRight,clipTop,clipLeft,(UiNodeBase *)control);
  return;
}


/* Address: 0x004B4D40.
   drawClipped of g_UiResizableWindowControlVtable: draws the window chrome selected by rootFlags (tiled
   background, frame, title bar with the centred title text, close button top left, maximize/restore button
   top right) and then the children. Title bar and buttons use their inactive pieces while the window is not
   in the front root, the buttons their armed pieces while pressed under the pointer.
*/
void UiResizableWindowControl_DrawFrameTitleAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiResizableWindowControl *control)

{
  uint32_t cornerWidthOrSubresource;
  uint32_t cornerHeight;
  int edgeOffset;
  int rightEdgeX;
  bool beginAccessFailed;
  uint16_t *titleText;
  GraphicsTextureLogicalSize textureSize;
  GraphicsTextureLogicalSize rightCapSize;

  beginAccessFailed = g_GraphicsFramebufferBeginAccess();
  if (!beginAccessFailed) {
    if ((control->root.rootFlags & UI_ROOT_TILED_BACKGROUND) != 0) {
      UiWindow_BlitTiledInterior
                (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_WINDOW_INTERIOR,
                 control->root.base.layoutHeight,control->root.base.layoutWidth,0,0,control);
    }
    if ((control->root.rootFlags & UI_ROOT_FRAME) != 0) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize
                              (UI_WINDOW_SUBRESOURCE_WINDOW_FRAME + UI_WINDOW_FRAME_BOTTOM_RIGHT,
                               g_UiWindowTextureSource);
      cornerHeight = textureSize.logicalHeightPixels;
      cornerWidthOrSubresource = textureSize.logicalWidthPixels;
      rightEdgeX = control->root.base.layoutWidth - cornerWidthOrSubresource;
      edgeOffset = control->root.base.layoutHeight - cornerHeight; /* bottom corner row */
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,control->root.base.top,control->root.base.left,
                 UI_WINDOW_SUBRESOURCE_WINDOW_FRAME + UI_WINDOW_FRAME_TOP_LEFT,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,control->root.base.top,rightEdgeX + control->root.base.left,
                 UI_WINDOW_SUBRESOURCE_WINDOW_FRAME + UI_WINDOW_FRAME_TOP_RIGHT,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,edgeOffset + control->root.base.top,control->root.base.left,
                 UI_WINDOW_SUBRESOURCE_WINDOW_FRAME + UI_WINDOW_FRAME_BOTTOM_LEFT,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,edgeOffset + control->root.base.top,
                 rightEdgeX + control->root.base.left,UI_WINDOW_SUBRESOURCE_WINDOW_FRAME + UI_WINDOW_FRAME_BOTTOM_RIGHT,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      UiWindow_BlitTiledHorizontalEdge
                (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_WINDOW_FRAME + UI_WINDOW_FRAME_TOP,
                 rightEdgeX,0,cornerWidthOrSubresource,control);
      UiWindow_BlitTiledVerticalEdge
                (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_WINDOW_FRAME + UI_WINDOW_FRAME_LEFT,
                 edgeOffset,cornerHeight,0,control);
      UiWindow_BlitTiledVerticalEdge
                (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_WINDOW_FRAME + UI_WINDOW_FRAME_RIGHT,
                 edgeOffset,cornerHeight,rightEdgeX,control);
      UiWindow_BlitTiledHorizontalEdge
                (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_WINDOW_FRAME + UI_WINDOW_FRAME_BOTTOM,
                 rightEdgeX,edgeOffset,cornerWidthOrSubresource,control);
    }
    if ((control->root.rootFlags & UI_ROOT_TITLE_BAR) != 0) {
      cornerWidthOrSubresource = UI_WINDOW_SUBRESOURCE_TITLE_BAR;
      if ((control->root.base.nodeFlags & UI_NODE_IN_FRONT_ROOT) == 0) {
        cornerWidthOrSubresource = UI_WINDOW_SUBRESOURCE_TITLE_BAR_INACTIVE;
      }
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,control->root.base.top,control->root.base.left,
                 cornerWidthOrSubresource,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      textureSize = g_GraphicsTextureSourceGetLogicalSize(cornerWidthOrSubresource,g_UiWindowTextureSource);
      edgeOffset = control->root.base.layoutWidth;
      rightCapSize = g_GraphicsTextureSourceGetLogicalSize
                               (cornerWidthOrSubresource + UI_WINDOW_TITLE_BAR_RIGHT,g_UiWindowTextureSource);
      edgeOffset = edgeOffset - rightCapSize.logicalWidthPixels; /* x of the right cap */
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,control->root.base.top,edgeOffset + control->root.base.left,
                 cornerWidthOrSubresource + UI_WINDOW_TITLE_BAR_RIGHT,g_UiWindowTextureSource,g_FramebufferAccess);
      UiWindow_BlitTiledHorizontalEdge
                (clipBottom,clipRight,clipTop,clipLeft,cornerWidthOrSubresource + UI_WINDOW_TITLE_BAR_MIDDLE,
                 edgeOffset,0,textureSize.logicalWidthPixels,control);
      titleText = TextResource_Resolve(control->titleTextResourceId);
      RichTextCommandStream_DrawSingleLine
                (clipBottom,clipRight,clipTop,clipLeft,g_UiResizableWindowTitleTextStyle,titleText,
                 g_UiResizableWindowTitleTextTopOffset + control->root.base.top,
                 (control->root.base.layoutWidth >> 1) + control->root.base.left);
    }
    if ((control->root.rootFlags & UI_ROOT_CLOSE_BUTTON) != 0) {
      cornerWidthOrSubresource = UI_WINDOW_SUBRESOURCE_CLOSE_BUTTON;
      if ((control->root.rootFlags & UI_ROOT_CLOSE_ARMED) != 0) {
        cornerWidthOrSubresource = UI_WINDOW_SUBRESOURCE_CLOSE_BUTTON_ARMED;
      }
      if ((control->root.base.nodeFlags & UI_NODE_IN_FRONT_ROOT) == 0) {
        cornerWidthOrSubresource = UI_WINDOW_SUBRESOURCE_CLOSE_BUTTON_INACTIVE;
      }
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,control->root.base.top,control->root.base.left,
                 cornerWidthOrSubresource,
                 g_UiWindowTextureSource,g_FramebufferAccess);
    }
    if ((control->root.rootFlags & UI_ROOT_MAXIMIZE_BUTTON) != 0) {
      cornerWidthOrSubresource = UI_WINDOW_SUBRESOURCE_MAXIMIZE_BUTTON;
      if ((control->root.rootFlags & UI_ROOT_MAXIMIZE_ARMED) != 0) {
        cornerWidthOrSubresource = UI_WINDOW_SUBRESOURCE_MAXIMIZE_BUTTON_ARMED;
      }
      if ((control->root.base.nodeFlags & UI_NODE_IN_FRONT_ROOT) == 0) {
        cornerWidthOrSubresource = UI_WINDOW_SUBRESOURCE_MAXIMIZE_BUTTON_INACTIVE;
      }
      if ((control->root.rootFlags & UI_ROOT_MAXIMIZED) != 0) {
        cornerWidthOrSubresource = cornerWidthOrSubresource + UI_WINDOW_RESTORE_BUTTON_OFFSET;
      }
      edgeOffset = control->root.base.layoutWidth;
      textureSize = g_GraphicsTextureSourceGetLogicalSize(cornerWidthOrSubresource,g_UiWindowTextureSource);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,control->root.base.top,
                 (edgeOffset - textureSize.logicalWidthPixels) + control->root.base.left,cornerWidthOrSubresource,
                 g_UiWindowTextureSource,
                 g_FramebufferAccess);
    }
    g_GraphicsFramebufferEndAccess();
  }
  UiContainer_DrawIntersectingChildren(clipBottom,clipRight,clipTop,clipLeft,(UiNodeBase *)control);
  return;
}


/* Address: 0x004B3420.
   drawClipped of g_UiTitledWindowControlVtable: draws the group-box frame with the title text set into its
   top edge (between two caps, centred when titleFlags bit 0 is set, otherwise after the top-left corner),
   then the children. The top edge is tiled left of the title only when it is centred.
*/
void UiTitledWindowControl_DrawFrameTitleAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiTitledWindowControl *control)

{
  uint32_t cornerWidth;
  uint16_t *commandStream;
  int bottomEdgeY;
  uint32_t titleCapX;
  int rightEdgeOrCursorX;
  bool beginAccessFailed;
  RichTextExtent titleExtent;
  uint16_t *titleText;
  GraphicsTextureLogicalSize textureSize;
  int savedRightEdgeX;

  beginAccessFailed = g_GraphicsFramebufferBeginAccess();
  if (!beginAccessFailed) {
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,control->base.top,control->base.left,
               UI_WINDOW_SUBRESOURCE_TITLED_BOX_FRAME + UI_WINDOW_FRAME_TOP_LEFT,
               g_UiWindowTextureSource,g_FramebufferAccess);
    rightEdgeOrCursorX = control->base.layoutWidth;
    bottomEdgeY = control->base.layoutHeight;
    textureSize = g_GraphicsTextureSourceGetLogicalSize
                            (UI_WINDOW_SUBRESOURCE_TITLED_BOX_FRAME + UI_WINDOW_FRAME_BOTTOM_RIGHT,
                             g_UiWindowTextureSource);
    cornerWidth = textureSize.logicalWidthPixels;
    rightEdgeOrCursorX = rightEdgeOrCursorX - cornerWidth;
    bottomEdgeY = bottomEdgeY - textureSize.logicalHeightPixels;
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,control->base.top,rightEdgeOrCursorX + control->base.left,
               UI_WINDOW_SUBRESOURCE_TITLED_BOX_FRAME + UI_WINDOW_FRAME_TOP_RIGHT,
               g_UiWindowTextureSource,g_FramebufferAccess);
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,bottomEdgeY + control->base.top,control->base.left,
               UI_WINDOW_SUBRESOURCE_TITLED_BOX_FRAME + UI_WINDOW_FRAME_BOTTOM_LEFT,
               g_UiWindowTextureSource,g_FramebufferAccess);
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,bottomEdgeY + control->base.top,
               rightEdgeOrCursorX + control->base.left,
               UI_WINDOW_SUBRESOURCE_TITLED_BOX_FRAME + UI_WINDOW_FRAME_BOTTOM_RIGHT,
               g_UiWindowTextureSource,g_FramebufferAccess);
    textureSize = g_GraphicsTextureSourceGetLogicalSize
                            (UI_WINDOW_SUBRESOURCE_TITLED_BOX_FRAME + UI_WINDOW_FRAME_TOP_LEFT,
                             g_UiWindowTextureSource);
    savedRightEdgeX = rightEdgeOrCursorX;
    UiWindow_BlitTiledVerticalEdge
              (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_TITLED_BOX_FRAME + UI_WINDOW_FRAME_LEFT,
               bottomEdgeY,textureSize.logicalHeightPixels,0,control);
    UiWindow_BlitTiledVerticalEdge
              (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_TITLED_BOX_FRAME + UI_WINDOW_FRAME_RIGHT,
               bottomEdgeY,textureSize.logicalHeightPixels,rightEdgeOrCursorX,control);
    UiWindow_BlitTiledHorizontalEdge
              (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_TITLED_BOX_FRAME + UI_WINDOW_FRAME_BOTTOM,
               rightEdgeOrCursorX,bottomEdgeY,cornerWidth,control);
    titleText = TextResource_Resolve(control->titleTextResourceId);
    commandStream = titleText;
    titleExtent = RichTextCommandStream_MeasureLine(g_UiWindowTitleTextStyle,commandStream);
    titleCapX = cornerWidth;
    if ((control->titleFlags & UI_TITLED_WINDOW_CENTERED_TITLE) != 0) {
      /* centred title: the top edge runs from the corner to the left cap */
      rightEdgeOrCursorX = control->base.layoutWidth;
      textureSize = g_GraphicsTextureSourceGetLogicalSize
                              (UI_WINDOW_SUBRESOURCE_TITLED_BOX_TITLE_LEFT,g_UiWindowTextureSource);
      titleCapX = ((int)(rightEdgeOrCursorX - titleExtent.widthPixels) >> 1) - textureSize.logicalWidthPixels;
      UiWindow_BlitTiledHorizontalEdge
                (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_TITLED_BOX_FRAME + UI_WINDOW_FRAME_TOP,
                 titleCapX,0,cornerWidth,control);
    }
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,control->base.top,titleCapX + control->base.left,
               UI_WINDOW_SUBRESOURCE_TITLED_BOX_TITLE_LEFT,g_UiWindowTextureSource,g_FramebufferAccess);
    textureSize = g_GraphicsTextureSourceGetLogicalSize
                            (UI_WINDOW_SUBRESOURCE_TITLED_BOX_TITLE_LEFT,g_UiWindowTextureSource);
    rightEdgeOrCursorX = titleCapX + textureSize.logicalWidthPixels;
    RichTextCommandStream_DrawSingleLine
              (clipBottom,clipRight,clipTop,clipLeft,g_UiWindowTitleTextStyle,commandStream,
               control->base.top,rightEdgeOrCursorX + control->base.left);
    titleExtent = RichTextCommandStream_MeasureLine(g_UiWindowTitleTextStyle,commandStream);
    rightEdgeOrCursorX = rightEdgeOrCursorX + titleExtent.widthPixels;
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,control->base.top,rightEdgeOrCursorX + control->base.left,
               UI_WINDOW_SUBRESOURCE_TITLED_BOX_TITLE_RIGHT,g_UiWindowTextureSource,g_FramebufferAccess);
    textureSize = g_GraphicsTextureSourceGetLogicalSize
                            (UI_WINDOW_SUBRESOURCE_TITLED_BOX_TITLE_RIGHT,g_UiWindowTextureSource);
    /* the top edge from the right cap to the top-right corner */
    UiWindow_BlitTiledHorizontalEdge
              (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_TITLED_BOX_FRAME + UI_WINDOW_FRAME_TOP,
               savedRightEdgeX,0,rightEdgeOrCursorX + textureSize.logicalWidthPixels,control);
    g_GraphicsFramebufferEndAccess();
  }
  UiContainer_DrawIntersectingChildren(clipBottom,clipRight,clipTop,clipLeft,(UiNodeBase *)control);
  return;
}


/* Address: 0x004AF890.
   Runs one complete UI frame (events, frame ticks, queued actions, draw, present) from code that may or may
   not hold the UI frame lock, e.g. modal loops and the fatal-error box: the lock is released for the frame
   and taken again afterwards only when it was held on entry.
*/
void __cdecl UiFrame_ProcessAndPresentWithLockTransition(void)

{
  bool lockWasHeld;
  
  /* the try-acquire takes a free lock, so both paths release it before the frame */
  lockWasHeld = g_SpinLockTryAcquire(g_UiRuntimeFrameLock);
  if (!lockWasHeld) {
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
void UiFrame_ProcessAndPresent(void)

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
   Shows page pageIndex of a page stack (tabbed dialog pages): the visible page is the stack's only child
   (firstChild), so switching replaces that link, moving the keyboard focus out of the old page and into the
   new one, and redraws. Out-of-range indices and the already shown page are ignored.
*/
void UiPageStack_SetActiveIndex(UiPageIndex pageIndex,UiPageStackControl *stack)

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
   nonRightRelease of g_UiResizableWindowControlVtable: ends a move or resize and completes a button press.
   Releasing over the armed close button closes the window, but only when the press was a double click
   (or a middle-button press), like the control-menu box of old Windows versions. Releasing over the
   armed maximize button toggles between the full framebuffer and the saved rectangle.
*/
void UiResizableWindowControl_EndMoveResizeAndHandleWindowActions
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiResizableWindowControl *control)

{
  int32_t topOrRight;
  int32_t rightOrBottom;
  int32_t restoredBottom;
  uint32_t framebufferHeight;

  if ((control->root.rootFlags & UI_ROOT_MOVING) != 0) {
    g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_ARROW);
  }
  if (((control->root.rootFlags & UI_ROOT_CLOSE_ARMED) != 0) &&
     ((control->root.base.nodeFlags & UI_NODE_REPEAT_OR_DOUBLE_CLICK) != 0)) {
    UiActionQueue_Enqueue(UI_ACTION_CLOSE_ROOT,control);
  }
  if ((control->root.rootFlags & UI_ROOT_MAXIMIZE_ARMED) != 0) {
    control->root.rootFlags = control->root.rootFlags ^ UI_ROOT_MAXIMIZED;
    if ((control->root.rootFlags & UI_ROOT_MAXIMIZED) == 0) {
      /* restore the rectangle saved when it was maximized */
      topOrRight = control->restoredTop;
      control->root.base.left = control->restoredLeft;
      rightOrBottom = control->restoredRight;
      restoredBottom = control->restoredBottom;
      control->root.base.top = topOrRight;
      control->root.base.bottom = restoredBottom;
      control->root.base.right = rightOrBottom;
      UiContainer_LayoutWithOptionalWindowHeaderOffset(control);
      UiRootStack_InvalidateAll();
      control->root.rootFlags = control->root.rootFlags & ~UI_ROOT_POINTER_STATE;
      return;
    }
    /* maximize: save the rectangle and cover the whole framebuffer */
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
  control->root.rootFlags = control->root.rootFlags & ~UI_ROOT_POINTER_STATE;
  return;
}


/* Address: 0x004B5770.
   keyboardEvent of g_UiResizableWindowControlVtable: Alt+C closes the window (with a close button), Alt+Z
   toggles maximize (with a maximize button), both returning CF clear; every other key goes to the default
   focus-moving handler. The key events of Keyboard_OnKeyDown carry letters as KEYBOARD_KEY_CODE_CHAR
   (0x30000 + code), so the plain 'c' / 'z' compared here never arrive and the hotkeys do not fire.
*/
bool UiResizableWindowControl_HandleWindowHotkeys
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiResizableWindowControl *control)

{
  int32_t topOrRight;
  int32_t rightOrBottom;
  int32_t restoredBottom;
  uint32_t framebufferHeight;
  bool delegateResult;

  if ((keyboardStateMask & KEYBOARD_STATE_ALT) != 0) {
    if (((control->root.rootFlags & UI_ROOT_CLOSE_BUTTON) != 0) && (keyCode == 'c')) {
      UiActionQueue_Enqueue(UI_ACTION_CLOSE_ROOT,control);
      return false;
    }
    if (((control->root.rootFlags & UI_ROOT_MAXIMIZE_BUTTON) != 0) && (keyCode == 'z')) {
      control->root.rootFlags = control->root.rootFlags ^ UI_ROOT_MAXIMIZED;
      if ((control->root.rootFlags & UI_ROOT_MAXIMIZED) == 0) {
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
   Opens a dialog or screen: puts the serialized UI tree root on top of the root stack. Its rectangle is
   computed from the framebuffer size and its anchors, its callbacks are attached and its tree pointers
   relocated. The previous top root loses UI_NODE_IN_FRONT_ROOT (windows draw as inactive), the new one is
   laid out, gets the flag and the initial keyboard focus; pointer capture and tooltip are reset.
*/
void UiRootStack_Push(UiRootCallbacks *callbacks,UiRootNode *root)

{
  int64_t edgeAnchorPixelProductQ31;
  UiRootNode *oldFrontRoot;
  int64_t currentAnchorPixelProductQ31;
  int64_t anchorPixelProductQ31;
  
  /* each edge = (framebuffer extent * anchorQ31) >> 31 + offset, i.e. a fraction of the screen plus pixels */
  anchorPixelProductQ31 = (uint64_t)g_FramebufferWidth * (uint64_t)(root->base).rightAnchorQ31;
  (root->base).right =
       (FIXED_PRODUCT_SHR(anchorPixelProductQ31, 31))
       + (root->base).rightOffset;
  currentAnchorPixelProductQ31 =
       (uint64_t)g_FramebufferHeight * (uint64_t)(root->base).bottomAnchorQ31;
  (root->base).bottom =
       (FIXED_PRODUCT_SHR(currentAnchorPixelProductQ31, 31)) + (root->base).bottomOffset;
  edgeAnchorPixelProductQ31 = (uint64_t)g_FramebufferWidth * (uint64_t)(root->base).leftAnchorQ31;
  (root->base).left =
       (FIXED_PRODUCT_SHR(edgeAnchorPixelProductQ31, 31)) +
       (root->base).leftOffset;
  edgeAnchorPixelProductQ31 = (uint64_t)g_FramebufferHeight * (uint64_t)(root->base).topAnchorQ31;
  (root->base).top =
       (FIXED_PRODUCT_SHR(edgeAnchorPixelProductQ31, 31)) +
       (root->base).topOffset;
  root->callbacks = callbacks;
  (root->base).nextSibling = UI_NODE_NONE;
  /* the serialized tree links are offsets from the root: relocate by the root's address */
  UiSerializedTree_Relocate((SerializedImageRelocationDelta)root,&root->base);
  oldFrontRoot = g_UiRootNode;
  LOCK();
  g_UiRootNode = root;
  UNLOCK();
  root->previousRoot = oldFrontRoot;
  if (oldFrontRoot != UI_ROOT_STACK_END) {
    (oldFrontRoot->base).nextSibling = &root->base;
    (*((oldFrontRoot->base).vtable)->applyFlags)(0,~UI_NODE_IN_FRONT_ROOT,&oldFrontRoot->base);
  }
  (*((root->base).vtable)->layout)(&root->base);
  (*((root->base).vtable)->applyFlags)(UI_NODE_IN_FRONT_ROOT,0xffffffff,&root->base);
  UiKeyboardFocus_SelectInitial(&root->base);
  g_UiPointerCaptureTarget = UI_NODE_NONE;
  g_UiPointerCaptureButton = UI_POINTER_CAPTURE_NONE;
  g_UiTooltipState.targetNode = NULL;
  return;
}


/* Address: 0x004B1110.
   Closes the dialog or screen that contains root (any node of it may be passed): its close callback may
   veto (CF set, returned). Otherwise the root below becomes the top again with UI_NODE_IN_FRONT_ROOT and its
   initial focus, pointer capture and hover are reset and the whole screen is redrawn. The closed root is
   assumed to be the top one: only g_UiRootNode is replaced.
*/
bool UiRootStack_Pop(UiRootNode *root)

{
  bool closeCallbackVetoed;
  UiRootNode *belowRoot;
  UiNodeBase *parentCursor;
  
  parentCursor = (root->base).parent;
  while (parentCursor != UI_NODE_NONE) {
    root = (UiRootNode *)(root->base).parent;
    parentCursor = (root->base).parent;
  }
  belowRoot = root->previousRoot;
  closeCallbackVetoed = false;
  if (root->callbacks->vetoClose != NULL) {
    closeCallbackVetoed = root->callbacks->vetoClose(root);
  }
  if (closeCallbackVetoed) {
    UiNode_InvalidateRoot(&belowRoot->base);
    return true;
  }
  g_UiKeyboardFocusNode = UI_NODE_NONE;
  g_UiRootNode = belowRoot;
  if (belowRoot != UI_ROOT_STACK_END) {
    (belowRoot->base).nextSibling = UI_NODE_NONE;
    (*((belowRoot->base).vtable)->applyFlags)(UI_NODE_IN_FRONT_ROOT,0xffffffff,&belowRoot->base);
    UiKeyboardFocus_SelectInitial(&belowRoot->base);
  }
  g_UiPointerCaptureTarget = UI_NODE_NONE;
  g_UiPointerCaptureButton = UI_POINTER_CAPTURE_NONE;
  g_UiImageControlHoverTarget = NULL;
  UiRootStack_InvalidateAll();
  return false;
}


/* Address: 0x004B2790.
   relocate of g_UiWindowControlVtable, the same as UiFramedTextButtonControl_Relocate: an inset-framed
   control (UI_BUTTON_FRAME_INSET) grows its layout offsets by g_UiWindowFrameInset on every side, so the
   frame lies outside the authored box; then the children are relocated.
*/
void UiWindowControl_RelocateWithFrameInset(UiSerializedRelocationDelta relocationDelta,UiWindowControl *control)

{
  int frameInset;

  frameInset = g_UiWindowFrameInset;
  if ((control->selectable.stateFlags & UI_BUTTON_FRAME_INSET) != 0) {
    control->selectable.base.leftOffset = control->selectable.base.leftOffset - g_UiWindowFrameInset;
    control->selectable.base.topOffset = control->selectable.base.topOffset - frameInset;
    control->selectable.base.rightOffset = control->selectable.base.rightOffset + frameInset;
    control->selectable.base.bottomOffset = control->selectable.base.bottomOffset + frameInset;
  }
  UiContainer_RelocateChildren(relocationDelta,(UiNodeBase *)control);
  return;
}


/* Address: 0x004B36C0.
   layout of g_UiTitledWindowControlVtable: lays out the children inside the frame, i.e. with the rectangle
   shrunk by the top-left corner (or the title height when that is taller) and the bottom-right corner,
   then restores the rectangle and grows layoutWidth/layoutHeight back to the full box.
*/
void UiTitledWindowControl_LayoutFrameTitleAndChildren(UiTitledWindowControl *control)

{
  uint32_t leftInset;
  uint32_t titleHeightOrRightInset;
  uint32_t topInset;
  uint32_t bottomInset;
  RichTextExtent titleExtent;
  uint16_t *titleText;
  GraphicsTextureLogicalSize cornerSize;

  titleText = TextResource_Resolve(control->titleTextResourceId);
  titleExtent = RichTextCommandStream_MeasureLine(g_UiWindowTitleTextStyle,titleText);
  titleHeightOrRightInset = titleExtent.heightPixels;
  cornerSize = g_GraphicsTextureSourceGetLogicalSize
                         (UI_WINDOW_SUBRESOURCE_TITLED_BOX_FRAME + UI_WINDOW_FRAME_TOP_LEFT,g_UiWindowTextureSource);
  leftInset = cornerSize.logicalWidthPixels;
  topInset = cornerSize.logicalHeightPixels;
  if ((int)cornerSize.logicalHeightPixels < (int)titleHeightOrRightInset) {
    topInset = titleHeightOrRightInset;
  }
  control->base.left = control->base.left + leftInset;
  control->base.top = control->base.top + topInset;
  cornerSize = g_GraphicsTextureSourceGetLogicalSize
                         (UI_WINDOW_SUBRESOURCE_TITLED_BOX_FRAME + UI_WINDOW_FRAME_BOTTOM_RIGHT,
                          g_UiWindowTextureSource);
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
   hitTest of g_UiFillPanelControlVtable: like UiContainer_HitTestChildren, but the container itself is never
   hit (UI_NODE_NONE instead), so the pointer passes through the panel to what lies below it.
*/
UiNodeBase * UiFillPanelControl_HitTestChildrenOnly
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control)

{
  UiNodeBase *hitNode;

  hitNode = UiContainer_HitTestChildren(pointerY,pointerX,control);
  if (hitNode == control) {
    hitNode = UI_NODE_NONE;
  }
  return hitNode;
}


/* Address: 0x004B3C80.
   drawClipped of g_UiHorizontalGaugeControlVtable (progress bar): draws the track, a fill proportional to
   (value - minimumValue) / (maximumValue - minimumValue) with value clamped to maximumValue, and with
   gaugeFlags bit 0 the percentage centred on top. The fill is left out while it would be narrower than
   its two caps. Children are not drawn.
*/
void UiHorizontalGaugeControl_DrawFrameFillAndLabel
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiHorizontalGaugeControl *control)

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
  GraphicsTextureLogicalSize textureSize;
  
  beginAccessFailed = g_GraphicsFramebufferBeginAccess();
  if (!beginAccessFailed) {
    rightCapXOrFillMin = control->base.layoutWidth;
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,control->base.top,control->base.left,
               UI_WINDOW_SUBRESOURCE_GAUGE_LEFT,g_UiWindowTextureSource,g_FramebufferAccess);
    textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_GAUGE_LEFT,g_UiWindowTextureSource);
    leftCapWidth = textureSize.logicalWidthPixels;
    /* the right cap is assumed to be as wide as the left one */
    rightCapXOrFillMin = rightCapXOrFillMin - leftCapWidth;
    UiWindow_BlitTiledHorizontalEdge
              (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_GAUGE_TRACK,rightCapXOrFillMin,0,
               leftCapWidth,&control->base);
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,control->base.top,rightCapXOrFillMin + control->base.left,
               UI_WINDOW_SUBRESOURCE_GAUGE_RIGHT,g_UiWindowTextureSource,g_FramebufferAccess);
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
      /* DIV, ADD EDX,EDX, ADC EAX,0 in the original: rounds up only when the remainder has bit 31 set */
      divisionRemainder = (uint32_t)(scaledFillProduct % (uint64_t)rangeProgressOrPercent);
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_GAUGE_FILL_LEFT,
                                                          g_UiWindowTextureSource);
      fillEndX = ((int)(scaledFillProduct / rangeProgressOrPercent) +
                  (uint32_t)CARRY4(divisionRemainder,
                                   divisionRemainder) + leftCapWidth) - textureSize.logicalWidthPixels;
      rightCapXOrFillMin = textureSize.logicalWidthPixels + leftCapWidth;
      rangeProgressOrPercent = progressOrRange;
      if (rightCapXOrFillMin <= fillEndX) {
        UiWindow_BlitTiledHorizontalEdge
                  (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_GAUGE_FILL,fillEndX,0,
                   rightCapXOrFillMin,&control->base);
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipBottom,clipRight,clipTop,clipLeft,control->base.top,leftCapWidth + control->base.left,
                   UI_WINDOW_SUBRESOURCE_GAUGE_FILL_LEFT,g_UiWindowTextureSource,g_FramebufferAccess);
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipBottom,clipRight,clipTop,clipLeft,control->base.top,fillEndX + control->base.left,
                   UI_WINDOW_SUBRESOURCE_GAUGE_FILL_RIGHT,g_UiWindowTextureSource,g_FramebufferAccess);
      }
    }
    if ((control->gaugeFlags & UI_HORIZONTAL_GAUGE_SHOW_PERCENT) != 0) {
      /* the percentage of the clamped progress (0 when no fill was computed), as "100%" or two digits
         without a leading zero */
      progressOrRange = control->maximumValue - control->minimumValue;
      if (progressOrRange == 0) {
        progressOrRange = 1;
      }
      divisionRemainder = (uint32_t)(((uint64_t)rangeProgressOrPercent * 100) % (uint64_t)progressOrRange);
      rangeProgressOrPercent = (int)(((uint64_t)rangeProgressOrPercent * 100) / (uint64_t)progressOrRange) +
                               (uint32_t)CARRY4(divisionRemainder,divisionRemainder);
      if (rangeProgressOrPercent == 100) {
        g_UiWindowPercentTextUtf16[0] = '1';
        g_UiWindowPercentTextUtf16[1] = '0';
        g_UiWindowPercentTextUtf16[2] = '0';
        g_UiWindowPercentTextUtf16[3] = '%';
        g_UiWindowPercentTextUtf16[4] = 0;
      }
      else {
        g_UiWindowPercentTextUtf16[1] = (short)((uint64_t)rangeProgressOrPercent % 10) + '0';
        g_UiWindowPercentTextUtf16[0] = (short)((uint64_t)rangeProgressOrPercent / 10) + '0';
        g_UiWindowPercentTextUtf16[2] = '%';
        g_UiWindowPercentTextUtf16[3] = 0;
      }
      commandStream = g_UiWindowPercentTextUtf16;
      if (g_UiWindowPercentTextUtf16[0] == '0') {
        commandStream = g_UiWindowPercentTextUtf16 + 1;
      }
      RichTextCommandStream_DrawSingleLine
                (clipBottom,clipRight,clipTop,clipLeft,g_UiHorizontalGaugeLabelTextStyle,
                 commandStream,g_UiHorizontalGaugeLabelTopInset + control->base.top,
                 ((uint32_t)control->base.layoutWidth >> 1) + control->base.left);
    }
    g_GraphicsFramebufferEndAccess();
  }
  return;
}


/* Address: 0x004B46A0.
   relocate of g_UiLayoutContainerControlVtable (the page stack, UiPageStackControl): turns the page links
   from image offsets into pointers, relocates every page's tree by making it the stack's firstChild in
   turn, and leaves page 0 as the shown page. Like the other page-stack methods it assumes at least one page.
*/
void UiLayoutContainerControl_RelocateChildren(UiSerializedRelocationDelta relocationDelta,UiPageStackControl *control)

{
  UiPageCount remainingCount;
  UiNodeBase **pageSlot;
  UiNodeBase **pageCursor;

  pageSlot = &control->pages;
  remainingCount = control->pageCount;
  do {
    if (*pageSlot != UI_NODE_NONE) {
      *pageSlot = (UiNodeBase *)((uint8_t *)*pageSlot + relocationDelta);
    }
    pageSlot = pageSlot + 1;
    remainingCount--;
  } while (remainingCount != 0);
  pageCursor = &control->pages;
  remainingCount = control->pageCount;
  do {
    control->base.firstChild = *pageCursor;
    UiContainer_RelocateChildren(relocationDelta,&control->base);
    pageCursor = pageCursor + 1;
    remainingCount--;
  } while (remainingCount != 0);
  control->base.firstChild = control->pages;
  return;
}


/* Address: 0x004B4700.
   layout of g_UiLayoutContainerControlVtable: lays out every page of the page stack, hidden ones included,
   by making each the stack's firstChild in turn; the shown page is restored afterwards.
*/
void UiLayoutContainerControl_LayoutChildren(UiPageStackControl *control)

{
  UiPageCount remainingCount;
  UiNodeBase **pageCursor;
  UiNodeBase *shownPage;

  pageCursor = &control->pages;
  shownPage = control->base.firstChild;
  remainingCount = control->pageCount;
  do {
    control->base.firstChild = *pageCursor;
    UiContainer_LayoutChildren(&control->base);
    pageCursor = pageCursor + 1;
    remainingCount--;
  } while (remainingCount != 0);
  control->base.firstChild = shownPage;
  return;
}


/* Address: 0x004B4790.
   hitTest of g_UiLayoutContainerControlVtable (the page stack): hit-tests the shown page like
   UiContainer_HitTestChildren, but the stack itself is never hit (UI_NODE_NONE instead).
*/
UiNodeBase * UiLayoutContainerControl_HitTestChildrenOnly
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control)

{
  UiNodeBase *hitNode;

  hitNode = UiContainer_HitTestChildren(pointerY,pointerX,control);
  if (hitNode == control) {
    hitNode = UI_NODE_NONE;
  }
  return hitNode;
}


/* Address: 0x004B47B0.
   suppressActionId of g_UiLayoutContainerControlVtable: suppresses the controls carrying actionId on every
   page of the page stack, hidden ones included (each page is made firstChild in turn).
*/
void UiLayoutContainerControl_SuppressActionIdRecursive(UiActionId actionId,UiPageStackControl *control)

{
  UiPageCount remainingCount;
  UiNodeBase **pageCursor;
  UiNodeBase *shownPage;

  pageCursor = &control->pages;
  shownPage = control->base.firstChild;
  remainingCount = control->pageCount;
  do {
    control->base.firstChild = *pageCursor;
    UiContainer_SuppressActionId(actionId,&control->base);
    pageCursor = pageCursor + 1;
    remainingCount--;
  } while (remainingCount != 0);
  control->base.firstChild = shownPage;
  return;
}


/* Address: 0x004B4800.
   unsuppressActionId of g_UiLayoutContainerControlVtable: the counterpart of
   UiLayoutContainerControl_SuppressActionIdRecursive for every page of the page stack.
*/
void UiLayoutContainerControl_UnsuppressActionIdRecursive(UiActionId actionId,UiPageStackControl *control)

{
  UiPageCount remainingCount;
  UiNodeBase **pageCursor;
  UiNodeBase *shownPage;

  pageCursor = &control->pages;
  shownPage = control->base.firstChild;
  remainingCount = control->pageCount;
  do {
    control->base.firstChild = *pageCursor;
    UiContainer_UnsuppressActionId(actionId,&control->base);
    pageCursor = pageCursor + 1;
    remainingCount--;
  } while (remainingCount != 0);
  control->base.firstChild = shownPage;
  return;
}


/* Address: 0x004B4D10.
   relocate of g_UiResizableWindowControlVtable: relocates the children, then makes the window a keyboard
   focus target only when it has a close or maximize button, i.e. hotkeys for
   UiResizableWindowControl_HandleWindowHotkeys.
*/
void UiResizableWindowControl_RelocateAndRefreshInteractionState
          (UiSerializedRelocationDelta relocationDelta,UiResizableWindowControl *control)

{
  UiContainer_RelocateChildren(relocationDelta,(UiNodeBase *)control);
  if ((control->root.rootFlags & (UI_ROOT_CLOSE_BUTTON | UI_ROOT_MAXIMIZE_BUTTON)) == 0) {
    control->root.base.nodeFlags =
         control->root.base.nodeFlags & ~(UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_PREFERRED_FOCUS_TARGET);
  }
  else {
    control->root.base.nodeFlags = control->root.base.nodeFlags | UI_NODE_FALLBACK_FOCUS_TARGET;
  }
  return;
}


/* Address: 0x004B53E0.
   nonRightDrag of g_UiResizableWindowControlVtable: while moving, shifts the window by the pointer's
   movement since the grab; while resizing, moves the grabbed edges to the pointer, keeping the window at
   least UI_WINDOW_MINIMUM_SIZE wide and high, and relays it out when the rectangle changed. While the close
   or maximize button is pressed it only tracks whether the pointer is still over it (armed).
*/
void UiResizableWindowControl_UpdateMoveOrResize
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
  GraphicsTextureLogicalSize buttonSize;

  offsetXOrEdge = pointerX - control->root.base.left;
  offsetYOrEdge = pointerY - control->root.base.top;
  if ((control->root.rootFlags & UI_ROOT_MOVING) == 0) {
    if ((control->root.rootFlags & UI_ROOT_CLOSE_PRESSED) == 0) {
      if ((control->root.rootFlags & UI_ROOT_MAXIMIZE_PRESSED) == 0) {
        if ((control->root.rootFlags & UI_ROOT_RESIZING) != 0) {
          /* the grabbed edges follow the pointer (back to absolute coordinates), the others stay */
          interactionFlags = control->root.rootFlags;
          offsetXOrEdge = offsetXOrEdge + control->root.base.left;
          offsetYOrEdge = offsetYOrEdge + control->root.base.top;
          newTopOrOldRight = offsetYOrEdge;
          if ((interactionFlags & UI_ROOT_RESIZE_MOVES_TOP) == 0) {
            newTopOrOldRight = control->root.base.top;
          }
          if ((interactionFlags & UI_ROOT_RESIZE_MOVES_BOTTOM) == 0) {
            offsetYOrEdge = control->root.base.bottom;
          }
          newLeftOrOldBottom = offsetXOrEdge;
          if ((interactionFlags & UI_ROOT_RESIZE_MOVES_LEFT) == 0) {
            newLeftOrOldBottom = control->root.base.left;
          }
          if ((interactionFlags & UI_ROOT_RESIZE_MOVES_RIGHT) == 0) {
            offsetXOrEdge = control->root.base.right;
          }
          control->dragAnchorXOrPendingRight = offsetXOrEdge;
          control->dragAnchorYOrPendingBottom = offsetYOrEdge;
          /* too narrow or too low: push the grabbed edge(s) back by the shortfall */
          offsetXOrEdge = (offsetXOrEdge - newLeftOrOldBottom) + -UI_WINDOW_MINIMUM_SIZE;
          if (offsetXOrEdge < 0) {
            if ((interactionFlags & UI_ROOT_RESIZE_MOVES_RIGHT) != 0) {
              control->dragAnchorXOrPendingRight = control->dragAnchorXOrPendingRight - offsetXOrEdge;
            }
            if ((interactionFlags & UI_ROOT_RESIZE_MOVES_LEFT) != 0) {
              newLeftOrOldBottom = newLeftOrOldBottom + offsetXOrEdge;
            }
          }
          offsetXOrEdge = (offsetYOrEdge - newTopOrOldRight) + -UI_WINDOW_MINIMUM_SIZE;
          if (offsetXOrEdge < 0) {
            if ((interactionFlags & UI_ROOT_RESIZE_MOVES_BOTTOM) != 0) {
              control->dragAnchorYOrPendingBottom = control->dragAnchorYOrPendingBottom - offsetXOrEdge;
            }
            if ((interactionFlags & UI_ROOT_RESIZE_MOVES_TOP) != 0) {
              newTopOrOldRight = newTopOrOldRight + offsetXOrEdge;
            }
          }
          newRight = control->dragAnchorXOrPendingRight;
          newBottom = control->dragAnchorYOrPendingBottom;
          /* exchange old and new edges (XCHG in the original); relayout only when one changed */
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
          if (offsetXOrEdge != control->root.base.left || offsetYOrEdge != control->root.base.top ||
              newTopOrOldRight != control->root.base.right || newLeftOrOldBottom != control->root.base.bottom) {
            UiRootStack_InvalidateAll();
            control->root.base.vtable->layout((UiNodeBase *)control);
            UiNode_InvalidateRoot((UiNodeBase *)control);
          }
        }
      }
      else {
        /* maximize button pressed: armed while the pointer is on its opaque pixels (top right) */
        newTopOrOldRight = control->root.base.layoutWidth;
        buttonSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_MAXIMIZE_BUTTON,
                                                           g_UiWindowTextureSource);
        overButton = g_GraphicsTextureSourceTestOpaquePixel
                          (offsetYOrEdge,offsetXOrEdge,0,newTopOrOldRight - buttonSize.logicalWidthPixels,
                           UI_WINDOW_SUBRESOURCE_MAXIMIZE_BUTTON,g_UiWindowTextureSource);
        if (overButton) {
          if ((control->root.rootFlags & UI_ROOT_MAXIMIZE_ARMED) != 0) {
            return;
          }
          control->root.rootFlags = control->root.rootFlags | UI_ROOT_MAXIMIZE_ARMED;
        }
        else {
          if ((control->root.rootFlags & UI_ROOT_MAXIMIZE_ARMED) == 0) {
            return;
          }
          control->root.rootFlags = control->root.rootFlags & ~UI_ROOT_MAXIMIZE_ARMED;
        }
        UiNode_InvalidateRoot((UiNodeBase *)control);
      }
    }
    else {
      /* close button pressed: armed while the pointer is on its opaque pixels (top left) */
      overButton = g_GraphicsTextureSourceTestOpaquePixel
                        (offsetYOrEdge,offsetXOrEdge,0,0,UI_WINDOW_SUBRESOURCE_CLOSE_BUTTON,g_UiWindowTextureSource);
      if (overButton) {
        if ((control->root.rootFlags & UI_ROOT_CLOSE_ARMED) != 0) {
          return;
        }
        control->root.rootFlags = control->root.rootFlags | UI_ROOT_CLOSE_ARMED;
      }
      else {
        if ((control->root.rootFlags & UI_ROOT_CLOSE_ARMED) == 0) {
          return;
        }
        control->root.rootFlags = control->root.rootFlags & ~UI_ROOT_CLOSE_ARMED;
      }
      /* result unused */
      g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_CLOSE_BUTTON,g_UiWindowTextureSource);
      UiNode_InvalidateRoot((UiNodeBase *)control);
    }
  }
  else {
    /* moving: dragAnchor* hold the grab point relative to the window */
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
   layout of g_UiNodeVtable_004BC570 (image toggles of the in-game resource panel): lays out the children
   relative to the parent's rectangle instead of the control's own by swapping the parent's edges in for
   the call; afterwards the own rectangle is restored and its size stored as layoutWidth/layoutHeight.
*/
void UiImageControl_LayoutChildrenToParent(UiImageControl *control)

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
void UiFrame_FlushInputAndResetPendingTicks(void)

{
  g_KeyboardFlushEvents();
  g_PointerFlushEvents();
  g_UiPendingFrameTicks = 0;
  return;
}


/* Address: 0x004AF950.
   Modal UI loop: with the pointer capture released and stale input flushed, runs whole UI frames until the
   root stack is empty (the last window closed), then presents one more frame with the tooltip cleared.
   No caller in the recovered code (reached only through the function map).
*/
void UiFrame_RunUntilRootClosedAndPresentFinalFrame(void)

{
  g_UiPointerCaptureTarget = UI_NODE_NONE;
  g_UiPointerCaptureButton = UI_POINTER_CAPTURE_NONE;
  UiFrame_FlushInputAndResetPendingTicks();
  do {
    UiKeyboard_DispatchPendingEvents();
    UiPointer_DispatchPendingEvents();
    UiFrame_Update(0); /* 0: pump messages once, do not wait for a frame tick */
    UiActionQueue_DispatchPending();
    UiFrame_Draw();
    g_GraphicsFramebufferPresent(g_FramebufferAccess);
  } while (g_UiRootNode != UI_ROOT_STACK_END);
  g_UiTooltipState.targetNode = NULL;
  UiTooltip_Draw(g_FramebufferHeight,g_FramebufferWidth,0,0);
  g_GraphicsFramebufferPresent(g_FramebufferAccess);
  return;
}

/* Address: 0x004AF9D0.
   Moves an open root (window) to the top of the root stack: unlinks it from its position, links it above
   the current front root, gives it the initial keyboard focus, moves the in-front flag from the old front
   root to it and invalidates both. Always returns false (CF clear).
*/
bool UiRootStack_BringToFront(UiRootNode *root)

{
  UiRootNode *belowRoot;
  UiRootNode *oldFrontRoot;
  UiRootNode *nextRootLink;
  UiNodeBase *nextFrontRootLink;

  oldFrontRoot = g_UiRootNode;
  nextRootLink = (UiRootNode *)root->base.nextSibling;
  belowRoot = root->previousRoot;
  if (nextRootLink != UI_ROOT_STACK_END) {
    nextRootLink->previousRoot = belowRoot;
  }
  if (belowRoot != UI_ROOT_STACK_END) {
    belowRoot->base.nextSibling = &nextRootLink->base;
  }
  nextFrontRootLink = g_UiRootNode->base.nextSibling;
  root->previousRoot = g_UiRootNode;
  root->base.nextSibling = nextFrontRootLink;
  g_UiRootNode->base.nextSibling = &root->base;
  g_UiRootNode = root;
  UiKeyboardFocus_SelectInitial(&root->base);
  (*oldFrontRoot->base.vtable->applyFlags)(0,~UI_NODE_IN_FRONT_ROOT,&oldFrontRoot->base);
  (*root->base.vtable->applyFlags)(UI_NODE_IN_FRONT_ROOT,0xffffffff,&root->base);
  UiNode_InvalidateRoot(&oldFrontRoot->base);
  UiNode_InvalidateRoot(&root->base);
  return false;
}


/* Address: 0x004B0F30.
   Loads what every window needs: the frame graphics (engine\win.gfx, engine\winclass.gfx) and their texts
   (texte\winclass.str as text page 1), installs the root-stack actions as action-handler page 0 and starts
   with an empty root stack. A missing file is fatal.
*/
void UiWindowResources_Init(void)

{
  GraphicsTextureSourceAsset *loadedTexture;
  uint32_t textureLoadError;
  uint32_t checkedValue;
  uint32_t localeBlockOrError;
  bool pageLoaded;

  loadedTexture = g_GraphicsTextureSourceLoadPackageAsset((uint16_t *)g_UiWindowTexturePathUtf16,&textureLoadError);
  checkedValue = FatalError_ExitIfFailed(loadedTexture != NULL ? (uint32_t)loadedTexture : textureLoadError,
                                          loadedTexture == NULL);
  g_UiWindowTextureSource = (GraphicsTextureSourceAsset *)checkedValue;
  loadedTexture = g_GraphicsTextureSourceLoadPackageAsset(g_UiWindowClassTexturePathUtf16,&textureLoadError);
  checkedValue = FatalError_ExitIfFailed(loadedTexture != NULL ? (uint32_t)loadedTexture : textureLoadError,
                                          loadedTexture == NULL);
  g_UiWindowClassTextureSource = (GraphicsTextureSourceAsset *)checkedValue;
  pageLoaded = TextResourcePage_Load(1,(uint16_t *)g_UiWindowClassTextPathUtf16,&localeBlockOrError);
  FatalError_ExitIfFailed(localeBlockOrError,!pageLoaded);
  UiActionHandlers_SetPage(0,(UiActionHandlerPage *)&g_UiRootStackActionHandlerPage);
  g_UiRootNode = UI_ROOT_STACK_END;
  return;
}


/* Address: 0x004B1240.
   After a display mode change (UiDisplayModeAction_ApplyPendingMode, FrontendDisplaySettings_ApplyMode):
   recomputes the rectangle of every open root from the new framebuffer size, its Q31 anchors and pixel
   offsets (as UiRootStack_Push does) and lays it out again, from the front root down. Assumes at least one
   open root.
*/
void UiRootStack_Relayout(void)

{
  int64_t edgeAnchorPixelProductQ31;
  UiRootNode *rootNode;
  int64_t currentAnchorPixelProductQ31;
  int64_t anchorPixelProductQ31;
  
  rootNode = g_UiRootNode;
  do {
    /* each edge = (framebuffer extent * anchorQ31) >> 31 + offset */
    anchorPixelProductQ31 =
         (uint64_t)g_FramebufferWidth * (uint64_t)(rootNode->base).rightAnchorQ31;
    (rootNode->base).right =
         (FIXED_PRODUCT_SHR(anchorPixelProductQ31, 31)) +
         (rootNode->base).rightOffset;
    currentAnchorPixelProductQ31 =
         (uint64_t)g_FramebufferHeight * (uint64_t)(rootNode->base).bottomAnchorQ31;
    (rootNode->base).bottom =
         (FIXED_PRODUCT_SHR(currentAnchorPixelProductQ31, 31)) + (rootNode->base).bottomOffset;
    edgeAnchorPixelProductQ31 = (uint64_t)g_FramebufferWidth * (uint64_t)(rootNode->base).leftAnchorQ31;
    (rootNode->base).left =
         (FIXED_PRODUCT_SHR(edgeAnchorPixelProductQ31, 31)) +
         (rootNode->base).leftOffset;
    edgeAnchorPixelProductQ31 = (uint64_t)g_FramebufferHeight * (uint64_t)(rootNode->base).topAnchorQ31;
    (rootNode->base).top =
         (FIXED_PRODUCT_SHR(edgeAnchorPixelProductQ31, 31)) +
         (rootNode->base).topOffset;
    (*((rootNode->base).vtable)->layout)(&rootNode->base);
    rootNode = rootNode->previousRoot;
  } while (rootNode != UI_ROOT_STACK_END);
  return;
}


/* Address: 0x004B3EE0.
   pointerMove of g_UiHorizontalGaugeControlVtable: returns the busy cursor as cursor frame, so a progress
   bar under the pointer shows it where the caller applies the frame (the in-game and scenario hover code
   pass it to g_GraphicsCursorSetFrame; the generic pointer-move dispatch ignores it).
*/
GraphicsCursorFrameIndex UiHorizontalGaugeControl_PointerMoveBusyCursor
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control)

{
  return GRAPHICS_CURSOR_FRAME_BUSY;
}


/* Address: 0x004B4740.
   applyFlags of g_UiLayoutContainerControlVtable: applies the node-flag masks (UiNode_ApplyFlagsRecursive)
   to every page of the page stack, hidden ones included, by making each the stack's firstChild in turn.
*/
void UiLayoutContainerControl_ApplyFlagsRecursive
          (UiNodeFlagMask setMask,UiNodeFlagMask retainMask,UiPageStackControl *control)

{
  UiPageCount remainingCount;
  UiNodeBase **pageCursor;
  UiNodeBase *shownPage;

  pageCursor = &control->pages;
  shownPage = control->base.firstChild;
  remainingCount = control->pageCount;
  do {
    control->base.firstChild = *pageCursor;
    UiNode_ApplyFlagsRecursive(setMask,retainMask,&control->base);
    pageCursor = pageCursor + 1;
    remainingCount--;
  } while (remainingCount != 0);
  control->base.firstChild = shownPage;
  return;
}


/* Address: 0x004B5120.
   nonRightPress of g_UiResizableWindowControlVtable: a press on the close or maximize button (its opaque
   pixels) marks that button pressed. Otherwise, unless maximized, a press on the border of a resizable
   window (outside the inner area left by the frame corners) starts a resize of the grabbed edge or corner,
   and a press in the top UiWindowMoveHandleWidth rows of a movable window starts a move with the move cursor.
*/
void UiResizableWindowControl_BeginMoveResizeOrWindowAction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiResizableWindowControl *control)

{
  int localX;
  uint32_t localY;
  uint32_t resizeFlags;
  int extentLimit;
  bool hitOpaque;
  GraphicsTextureLogicalSize textureSize;

  localX = pointerX - control->root.base.left;
  localY = pointerY - control->root.base.top;
  if (((control->root.rootFlags & UI_ROOT_CLOSE_BUTTON) != 0) &&
     (hitOpaque = g_GraphicsTextureSourceTestOpaquePixel
                        (localY,localX,0,0,UI_WINDOW_SUBRESOURCE_CLOSE_BUTTON,g_UiWindowTextureSource),
     hitOpaque)) {
    control->root.rootFlags = control->root.rootFlags | UI_ROOT_CLOSE_PRESSED;
    return;
  }
  if ((control->root.rootFlags & UI_ROOT_MAXIMIZE_BUTTON) != 0) {
    extentLimit = control->root.base.layoutWidth;
    textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_MAXIMIZE_BUTTON,g_UiWindowTextureSource);
    hitOpaque = g_GraphicsTextureSourceTestOpaquePixel
                      (localY,localX,0,extentLimit - textureSize.logicalWidthPixels,
                       UI_WINDOW_SUBRESOURCE_MAXIMIZE_BUTTON,g_UiWindowTextureSource);
    if (hitOpaque) {
      control->root.rootFlags = control->root.rootFlags | UI_ROOT_MAXIMIZE_PRESSED;
      return;
    }
  }
  if ((control->root.rootFlags & UI_ROOT_MAXIMIZED) == 0) {
    if ((control->root.rootFlags & UI_ROOT_RESIZABLE) != 0) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize
                              (UI_WINDOW_SUBRESOURCE_WINDOW_FRAME + UI_WINDOW_FRAME_TOP_LEFT,g_UiWindowTextureSource);
      if ((((localX < (int)textureSize.logicalWidthPixels) || ((int)localY < (int)textureSize.logicalHeightPixels))
          || ((int)(control->root.base.layoutWidth - textureSize.logicalWidthPixels) <= localX)) ||
         ((int)(control->root.base.layoutHeight - textureSize.logicalHeightPixels) <= (int)localY)) {
        /* which border: within g_UiWindowResizeBorderThickness of a side (corners take precedence) */
        extentLimit = control->root.base.layoutHeight - g_UiWindowResizeBorderThickness;
        if (localX < g_UiWindowResizeBorderThickness) {
          resizeFlags = UI_ROOT_RESIZE_TOP_LEFT | UI_ROOT_RESIZING;
          if ((g_UiWindowResizeBorderThickness <= (int)localY) &&
             (resizeFlags = UI_ROOT_RESIZE_BOTTOM_LEFT | UI_ROOT_RESIZING, (int)localY < extentLimit)) {
            resizeFlags = UI_ROOT_RESIZE_LEFT | UI_ROOT_RESIZING;
          }
        }
        else if (localX < control->root.base.layoutWidth - g_UiWindowResizeBorderThickness) {
          resizeFlags = UI_ROOT_RESIZE_TOP | UI_ROOT_RESIZING;
          if (g_UiWindowResizeBorderThickness <= (int)localY) {
            resizeFlags = UI_ROOT_RESIZE_BOTTOM | UI_ROOT_RESIZING;
          }
        }
        else {
          resizeFlags = UI_ROOT_RESIZE_TOP_RIGHT | UI_ROOT_RESIZING;
          if ((g_UiWindowResizeBorderThickness <= (int)localY) &&
             (resizeFlags = UI_ROOT_RESIZE_BOTTOM_RIGHT | UI_ROOT_RESIZING, (int)localY < extentLimit)) {
            resizeFlags = UI_ROOT_RESIZE_RIGHT | UI_ROOT_RESIZING;
          }
        }
        /* clears the UI_ROOT_RESIZE_EDGES byte; signed, because the unsigned ~UI_ROOT_RESIZE_EDGES changes the
           generated code of this build */
        control->root.rootFlags = control->root.rootFlags & (int)~UI_ROOT_RESIZE_EDGES;
        control->root.rootFlags = control->root.rootFlags | resizeFlags;
        return;
      }
    }
    if (((control->root.rootFlags & UI_ROOT_MOVABLE) != 0) && (localY < g_UiWindowMoveHandleWidth)) {
      control->root.rootFlags = control->root.rootFlags | UI_ROOT_MOVING;
      control->dragAnchorXOrPendingRight = localX;
      control->dragAnchorYOrPendingBottom = localY;
      g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_MOVE);
    }
  }
  return;
}


/* Address: 0x004B5660.
   pointerMove of g_UiResizableWindowControlVtable: returns the cursor frame for the pointer position, a
   resize cursor over the border of a resizable, non-maximized window (the same border zones as
   UiResizableWindowControl_BeginMoveResizeOrWindowAction), otherwise the arrow.
*/
GraphicsCursorFrameIndex UiResizableWindowControl_QueryResizeCursorCode
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiResizableWindowControl *control)

{
  GraphicsCursorFrameIndex cursorFrame;
  int bottomBorderY;
  int localY;
  int localX;
  GraphicsTextureLogicalSize cornerSize;

  cursorFrame = GRAPHICS_CURSOR_FRAME_ARROW;
  if (((control->root.rootFlags & UI_ROOT_RESIZABLE) != 0) && ((control->root.rootFlags & UI_ROOT_MAXIMIZED) == 0)) {
    localX = pointerX - control->root.base.left;
    if ((control->root.base.left <= pointerX) &&
       (((localY = pointerY - control->root.base.top, control->root.base.top <= pointerY &&
         (localX < control->root.base.layoutWidth)) && (localY < control->root.base.layoutHeight)))) {
      cornerSize = g_GraphicsTextureSourceGetLogicalSize
                             (UI_WINDOW_SUBRESOURCE_WINDOW_FRAME + UI_WINDOW_FRAME_TOP_LEFT,g_UiWindowTextureSource);
      if ((((localX < (int)cornerSize.logicalWidthPixels) || (localY < (int)cornerSize.logicalHeightPixels)) ||
          ((int)(control->root.base.layoutWidth - cornerSize.logicalWidthPixels) <= localX)) ||
         (cursorFrame = GRAPHICS_CURSOR_FRAME_ARROW,
         (int)(control->root.base.layoutHeight - cornerSize.logicalHeightPixels) <= localY)) {
        bottomBorderY = control->root.base.layoutHeight - g_UiWindowResizeBorderThickness;
        if (localX < g_UiWindowResizeBorderThickness) {
          cursorFrame = GRAPHICS_CURSOR_FRAME_SIZE_NWSE;
          if ((g_UiWindowResizeBorderThickness <= localY) &&
             (cursorFrame = GRAPHICS_CURSOR_FRAME_SIZE_NESW, localY < bottomBorderY)) {
            return GRAPHICS_CURSOR_FRAME_SIZE_WE;
          }
        }
        else {
          if (localX < control->root.base.layoutWidth - g_UiWindowResizeBorderThickness) {
            return GRAPHICS_CURSOR_FRAME_SIZE_NS;
          }
          cursorFrame = GRAPHICS_CURSOR_FRAME_SIZE_NESW;
          if ((g_UiWindowResizeBorderThickness <= localY) &&
             (cursorFrame = GRAPHICS_CURSOR_FRAME_SIZE_NWSE, localY < bottomBorderY)) {
            cursorFrame = GRAPHICS_CURSOR_FRAME_SIZE_WE;
          }
        }
      }
    }
  }
  return cursorFrame;
}


/* Address: 0x00569A80.
   Picks a grid for itemCount items, returned as EDX = rows, EAX = columns: up to 4 items in one row, up to
   4 * maxRows items in rows of 4, more in maxRows rows (fewer when the last rows would stay empty) of as
   many columns as needed.
*/
UiGridDimensionsEdxEax8 UiGrid_ComputeDimensionsPacked(UiControlCount maxRows,UiControlCount itemCount)

{
  uint32_t columnCount;
  uint32_t rowCount;
  
  rowCount = 1;
  columnCount = itemCount;
  if (4 < itemCount) {
    if (maxRows << 2 < itemCount) {
      /* the mask mirrors the SHL by 2 of the comparison */
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
   The single-column counterpart of UiGrid_ComputeDimensionsPacked: EDX = itemCount rows, EAX = 1 column.
*/
UiGridDimensionsEdxEax8 UiGrid_OneColumnDimensionsPacked(UiControlCount itemCount)

{
  return ((UiGridDimensionsEdxEax8)itemCount << 32) | 1;
}


/* Address: 0x004B0940.
   suppressActionId of the plain containers (g_UiPanelControlVtable, g_UiTitledWindowControlVtable,
   g_UiResizableWindowControlVtable and most other container vtables): passes the request on to every child;
   the controls that carry an action id suppress themselves when it matches.
*/
void UiContainer_SuppressActionId(UiActionId actionId,UiNodeBase *control)

{
  UiNodeBase *childNode;

  for (childNode = control->firstChild; childNode != UI_NODE_NONE;
      childNode = childNode->nextSibling) {
    childNode->vtable->suppressActionId(actionId,childNode);
  }
  return;
}


/* Address: 0x004B0990.
   unsuppressActionId of the same container vtables as UiContainer_SuppressActionId: passes the request on to
   every child.
*/
void UiContainer_UnsuppressActionId(UiActionId actionId,UiNodeBase *control)

{
  UiNodeBase *childNode;

  for (childNode = control->firstChild; childNode != UI_NODE_NONE;
      childNode = childNode->nextSibling) {
    childNode->vtable->unsuppressActionId(actionId,childNode);
  }
  return;
}


/* Address: 0x004B1420.
   Relocates a UI tree loaded from a serialized image: for each node of the sibling chain from firstNode
   that is still unrelocated (layoutWidth -1, reset to 0 here) the sibling/child/parent links are turned
   from image offsets into pointers by adding imageDelta, the transient click and focus flags are cleared,
   and the node's own relocate method runs (containers relocate their children from there).
*/
void UiSerializedTree_Relocate(SerializedImageRelocationDelta imageDelta,UiNodeBase *firstNode)

{
  for (; (firstNode != UI_NODE_NONE && (firstNode->layoutWidth == -1));
      firstNode = firstNode->nextSibling) {
    firstNode->layoutWidth = ~firstNode->layoutWidth;
    if (firstNode->nextSibling != UI_NODE_NONE) {
      firstNode->nextSibling = (UiNodeBase *)((uint8_t *)firstNode->nextSibling + imageDelta);
    }
    if (firstNode->firstChild != UI_NODE_NONE) {
      firstNode->firstChild = (UiNodeBase *)((uint8_t *)firstNode->firstChild + imageDelta);
    }
    if (firstNode->parent != UI_NODE_NONE) {
      firstNode->parent = (UiNodeBase *)((uint8_t *)firstNode->parent + imageDelta);
    }
    firstNode->nodeFlags =
         firstNode->nodeFlags & ~(UI_NODE_REPEAT_OR_DOUBLE_CLICK|UI_NODE_HAS_KEYBOARD_FOCUS);
    firstNode->vtable->relocate(imageDelta,firstNode);
  }
  return;
}


/* Address: 0x004B4850.
   Gives the keyboard focus, if nothing has it, to the first focus target below root (depth first), e.g. when
   a page becomes active.
*/
void UiNodeSubtree_AcquireKeyboardFocusDefaults(UiNodeBase *root)

{
  UiNodeBase *node;

  for (node = root->firstChild; node != UI_NODE_NONE; node = node->nextSibling) {
    UiKeyboardFocus_AcquireIfNone(node);
    UiNodeSubtree_AcquireKeyboardFocusDefaults(node);
  }
  return;
}


/* Address: 0x004B4890.
   Takes the keyboard focus away from every node below root (depth first; see UiKeyboardFocus_ReleaseNode)
   before that subtree, e.g. a page, is deactivated.
*/
void UiNodeSubtree_ReleaseKeyboardFocus(UiNodeBase *root)

{
  UiNodeBase *node;

  for (node = root->firstChild; node != UI_NODE_NONE; node = node->nextSibling) {
    UiKeyboardFocus_ReleaseNode(node);
    UiNodeSubtree_ReleaseKeyboardFocus(node);
  }
  return;
}


/* Address: 0x004B50D0.
   layout of g_UiResizableWindowControlVtable (also called after maximize/restore): lays out the children
   below the title bar when the window has one (UI_ROOT_TITLE_BAR), i.e. with top moved down by the bar
   height for the call, and adds that height back to layoutHeight afterwards.
*/
void UiContainer_LayoutWithOptionalWindowHeaderOffset(UiResizableWindowControl *control)

{
  uint32_t headerHeight;
  GraphicsTextureLogicalSize headerSize;

  if ((control->root.rootFlags & UI_ROOT_TITLE_BAR) == 0) {
    UiContainer_LayoutChildren((UiNodeBase *)control);
  }
  else {
    headerSize = g_GraphicsTextureSourceGetLogicalSize
                           (UI_WINDOW_SUBRESOURCE_TITLE_BAR + UI_WINDOW_TITLE_BAR_MIDDLE,g_UiWindowTextureSource);
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
void UiFrame_Update(UiStopMessageCode stopMessageCode)

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
    if (g_UiPointerCaptureTarget != UI_NODE_NONE) {
      g_UiPointerCaptureTarget->vtable->tick(g_UiPointerCaptureTarget);
    }
    /* the focus node ticks only once when it also holds the pointer capture */
    if ((g_UiKeyboardFocusNode != UI_NODE_NONE) &&
       (g_UiKeyboardFocusNode != g_UiPointerCaptureTarget)) {
      g_UiKeyboardFocusNode->vtable->tick(g_UiKeyboardFocusNode);
    }
    UiTooltip_TickCountdown();
  }
  g_DirectInputMouseRefreshCountdown--;
  if (g_DirectInputMouseRefreshCountdown == 0) {
    g_DirectInputMouseRefreshCountdown = UI_FRAME_DIRECT_INPUT_REFRESH_INTERVAL;
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
void UiFrame_Draw(void)

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
      /* drawClipped takes (bottom, right, top, left, node) */
      (*((root->base).vtable)->drawClipped)(clipBottom,clipRight,clipTop,clipLeft,&root->base);
    }
  }
  UiTooltip_Draw(g_FramebufferHeight,g_FramebufferWidth,0,0);
  return;
}


/* Eligible siblings from `child` on, hit-tested last first. The recursion first checks every remaining
   sibling for eligibility and only then calls hitTest on the way back, like the original, which pushes
   all eligible children before popping them. Returns UI_NODE_NONE when none claims the pointer. */
static UiNodeBase *UiContainer_HitTestEligibleSiblings
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *child) {
  UiNodeBase *hit;

  for (; child != UI_NODE_NONE; child = child->nextSibling) {
    if ((((child->nodeFlags & UI_NODE_ALLOW_CHILD_HIT_TEST_OUTSIDE_BOUNDS) != 0) ||
         ((child->left <= pointerX && child->top <= pointerY) &&
          (pointerX < child->right && pointerY < child->bottom))) &&
        ((child->nodeFlags & UI_NODE_SUPPRESSED) == 0)) {
      hit = UiContainer_HitTestEligibleSiblings(pointerY,pointerX,child->nextSibling);
      if (hit != UI_NODE_NONE) {
        return hit;
      }
      return child->vtable->hitTest(pointerY,pointerX,child);
    }
  }
  return UI_NODE_NONE;
}

/* Address: 0x004B0800.
   Finds the UI node under the pointer: every non-suppressed child that contains the pointer (or may be hit
   outside its bounds) is asked via its hitTest method, the last sibling (drawn on top) first. Returns the
   first hit, or the container itself when no child claims the pointer.
*/
UiNodeBase * UiContainer_HitTestChildren(UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control)

{
  /* Rewritten from the assembly (0x004B0800): eligible children are pushed on the machine stack
     in sibling order and hit-tested in reverse (topmost first); Ghidra lost the pushed nodes.
     Like the original there is no limit on the number of eligible children (the helper recurses). */
  UiNodeBase *hit;

  hit = UiContainer_HitTestEligibleSiblings(pointerY,pointerX,control->firstChild);
  if (hit != UI_NODE_NONE) {
    return hit;
  }
  return control;
}


/* Address: 0x004B13B0.
   Tiles a piece of g_UiWindowTextureSource over the rectangle (tileLeft, tileTop)..(tileRight, tileBottom),
   given relative to node (a UiNodeBase), clipped to the clip rectangle. Like
   GraphicsTextureSource_BlitTiledSourceAlpha, which it forwards to, it takes the bottom/right values first.
   Called by the window, panel and button draw methods.
*/
void UiWindow_BlitTiledInterior(UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,GraphicsSubresourceIndex subresource,UiPixelCoordinate tileBottom,
          UiPixelCoordinate tileRight,UiPixelCoordinate tileTop,UiPixelCoordinate tileLeft,
          void *node)

{
  g_GraphicsTextureSourceBlitTiledSourceAlpha
            (clipBottom,clipRight,clipTop,clipLeft,tileBottom + ((UiNodeBase *)node)->top,
             tileRight + ((UiNodeBase *)node)->left,tileTop + ((UiNodeBase *)node)->top,
             tileLeft + ((UiNodeBase *)node)->left,subresource,g_UiWindowTextureSource,
             g_FramebufferAccess);
  return;
}


/* Address: 0x004B0510.
   relocate of the plain containers (g_UiPanelControlVtable, g_UiTitledWindowControlVtable and most other
   container vtables): relocates the children like UiSerializedTree_Relocate does for a root's siblings.
   Every child still unrelocated (layoutWidth -1, reset to 0 here) gets its sibling/child/parent links turned
   from image offsets into pointers and its transient click and focus flags cleared, then relocates its own
   subtree through its relocate method.
*/
void UiContainer_RelocateChildren(UiSerializedRelocationDelta relocationDelta,UiNodeBase *control)

{
  UiNodeBase *childNode;

  for (childNode = control->firstChild;
      (childNode != UI_NODE_NONE && (childNode->layoutWidth == -1));
      childNode = childNode->nextSibling) {
    childNode->layoutWidth = ~childNode->layoutWidth;
    if (childNode->nextSibling != UI_NODE_NONE) {
      childNode->nextSibling = (UiNodeBase *)((uint8_t *)childNode->nextSibling + relocationDelta);
    }
    if (childNode->firstChild != UI_NODE_NONE) {
      childNode->firstChild = (UiNodeBase *)((uint8_t *)childNode->firstChild + relocationDelta);
    }
    if (childNode->parent != UI_NODE_NONE) {
      childNode->parent = (UiNodeBase *)((uint8_t *)childNode->parent + relocationDelta);
    }
    childNode->nodeFlags =
         childNode->nodeFlags & ~(UI_NODE_REPEAT_OR_DOUBLE_CLICK|UI_NODE_HAS_KEYBOARD_FOCUS);
    childNode->vtable->relocate(relocationDelta,childNode);
  }
  return;
}


/* Address: 0x004B05B0.
   drawClipped of g_UiLayoutContainerControlVtable and the tail of the container draw methods: draws each
   child whose rectangle intersects the clip rectangle, first child first (later siblings on top). The clip
   rectangle is passed on unchanged.
*/
void UiContainer_DrawIntersectingChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiNodeBase *control)

{
  UiNodeBase *childNode;

  for (childNode = control->firstChild; childNode != UI_NODE_NONE;
      childNode = childNode->nextSibling) {
    if ((((childNode->left <= clipRight) && (childNode->top <= clipBottom)) &&
        (clipLeft < childNode->right)) && (clipTop < childNode->bottom)) {
      childNode->vtable->drawClipped(clipBottom,clipRight,clipTop,clipLeft,childNode);
    }
  }
  return;
}


/* Address: 0x004B1350.
   Tiles a piece of g_UiWindowTextureSource downwards from tileTop to tileBottom in one column at tileLeft
   (relative to node, a UiNodeBase), one piece wide (INT32_MIN as right edge), clipped to the clip rectangle:
   the vertical edges of window and button frames. Bottom/right values come first, as in
   GraphicsTextureSource_BlitTiledSourceAlpha.
*/
void UiWindow_BlitTiledVerticalEdge(UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,GraphicsSubresourceIndex subresource,UiPixelCoordinate tileBottom,
          UiPixelCoordinate tileTop,UiPixelCoordinate tileLeft,void *node)

{
  g_GraphicsTextureSourceBlitTiledSourceAlpha
            (clipBottom,clipRight,clipTop,clipLeft,tileBottom + ((UiNodeBase *)node)->top,INT32_MIN,
             tileTop + ((UiNodeBase *)node)->top,tileLeft + ((UiNodeBase *)node)->left,subresource,
             g_UiWindowTextureSource,g_FramebufferAccess);
  return;
}


/* Address: 0x004B0640.
   Default layout of a container: stores its own width/height, then places every child. Each child edge is
   (parent extent * anchorQ31) >> 31 + offset from the parent's left/top, i.e. a fraction of the parent plus
   a pixel offset; then the child lays out its own children.
*/
void UiContainer_LayoutChildren(UiNodeBase *control)

{
  UiNodeBase *childNode;
  int64_t leftOrTopAnchorProduct;
  int rightEdge;
  int bottomEdge;
  int leftOrTopEdge;
  int64_t bottomAnchorProduct;
  int64_t rightAnchorProduct;
  
  childNode = control->firstChild;
  control->layoutWidth = control->right - control->left;
  control->layoutHeight = control->bottom - control->top;
  for (; childNode != UI_NODE_NONE; childNode = childNode->nextSibling) {
    rightAnchorProduct =
         (uint64_t)(uint32_t)control->layoutWidth * (uint64_t)childNode->rightAnchorQ31;
    rightEdge =
         (FIXED_PRODUCT_SHR(rightAnchorProduct, 31)
         ) + childNode->rightOffset + control->left;
    childNode->right = rightEdge;
    childNode->layoutWidth = rightEdge; /* minus the left edge below */
    bottomAnchorProduct =
         (uint64_t)(uint32_t)control->layoutHeight * (uint64_t)childNode->bottomAnchorQ31;
    bottomEdge =
         (FIXED_PRODUCT_SHR(bottomAnchorProduct, 31)) + childNode->bottomOffset + control->top;
    childNode->bottom = bottomEdge;
    childNode->layoutHeight = bottomEdge;
    leftOrTopAnchorProduct = (uint64_t)(uint32_t)control->layoutWidth * (uint64_t)childNode->leftAnchorQ31;
    leftOrTopEdge = (FIXED_PRODUCT_SHR(leftOrTopAnchorProduct, 31)) +
                    childNode->leftOffset + control->left;
    childNode->left = leftOrTopEdge;
    childNode->layoutWidth = childNode->layoutWidth - leftOrTopEdge;
    leftOrTopAnchorProduct = (uint64_t)(uint32_t)control->layoutHeight * (uint64_t)childNode->topAnchorQ31;
    leftOrTopEdge = (FIXED_PRODUCT_SHR(leftOrTopAnchorProduct, 31)) +
                    childNode->topOffset + control->top;
    childNode->top = leftOrTopEdge;
    childNode->layoutHeight = childNode->layoutHeight - leftOrTopEdge;
    childNode->vtable->layout(childNode);
  }
  return;
}


/* Address: 0x004B12F0.
   Tiles a piece of g_UiWindowTextureSource rightwards from tileLeft to tileRight in one row at tileTop
   (relative to node, a UiNodeBase), one piece high (INT32_MIN as bottom edge), clipped to the clip
   rectangle: horizontal frame edges, title bars and gauge tracks. Bottom/right values come first, as in
   GraphicsTextureSource_BlitTiledSourceAlpha.
*/
void UiWindow_BlitTiledHorizontalEdge
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,GraphicsSubresourceIndex subresource,
          UiPixelCoordinate tileRight,UiPixelCoordinate tileTop,UiPixelCoordinate tileLeft,void *node)

{
  g_GraphicsTextureSourceBlitTiledSourceAlpha
            (clipBottom,clipRight,clipTop,clipLeft,INT32_MIN,
             tileRight + ((UiNodeBase *)node)->left,tileTop + ((UiNodeBase *)node)->top,
             tileLeft + ((UiNodeBase *)node)->left,subresource,g_UiWindowTextureSource,
             g_FramebufferAccess);
  return;
}


/* Address: 0x004B14D0.
   Marks the whole screen for redraw: drops the collected dirty rectangles and invalidates every root on the UI
   root stack, top to bottom. Does nothing while invalidation is suppressed.
*/
void UiRootStack_InvalidateAll(void)

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

