/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/tooltip.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/tooltip.h>
#include <thandor/thandor.h>

/* Module data. */

UiTooltipState g_UiTooltipState = {.countdownFrames = 8};

static const UiFrameDelayFrames g_UiTooltipDelayFrames = 12;

static const uint32_t g_UiTooltipTextStyle = 0;

/* Implementation ownership: ui/controls/tooltip. */

/* Per-frame tooltip delay: while the pointer rests on an enabled control (no button held), counts the
   delay down and prepares the tooltip text when it expires. While a node has captured the pointer or the
   hovered control is disabled, re-evaluates the hover target at the last pointer position instead.
*/
void UiTooltip_TickCountdown()

{
  if ((g_UiPointerCaptureTarget == UI_NODE_NONE) &&
     ((g_UiTooltipState.targetNode == nullptr ||
      (((g_UiTooltipState.targetNode)->nodeFlags & UI_NODE_SUPPRESSED) == 0)))) {
    if (g_UiTooltipState.countdownFrames != 0) {
      g_UiTooltipState.countdownFrames--;
      if (g_UiTooltipState.countdownFrames == 0) {
        UiTooltip_PrepareTargetText(g_UiTooltipState.targetNode);
      }
    }
    return;
  }
  UiTooltip_UpdateHoverTarget(g_UiTooltipState.pointerY,g_UiTooltipState.pointerX);
}

/* Draws the tooltip once its delay has expired: a one-line box (win.gfx left cap 0xBC, tiled middle 0xBD,
   right cap 0xBE) centred above the hovered control, kept inside its root window, and moved below the
   control when there is no room above. Drawn last in the frame, over everything. With no root open at all,
   the whole screen is darkened (ARGB 0x80000000: black at half alpha).
*/
void UiTooltip_Draw(UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
              UiPixelCoordinate clipLeft)

{
  int targetRight;
  UiNodeBase *tooltipTarget;
  UiNodeBase *rootNode;
  uint16_t *commandStream;
  uint32_t edgeTileWidth;
  int32_t frameLeft;
  int32_t frameRight;
  int frameWidth;
  int targetLeft;
  int middleEnd;
  int tileX;
  int frameTop;
  Bool8 framebufferUnavailable;
  RichTextExtent textExtent;
  uint16_t *resolvedText;
  GraphicsTextureLogicalSize tileSize;
  
  tooltipTarget = g_UiTooltipState.targetNode;
  if ((g_UiTooltipState.targetNode != nullptr) && (g_UiTooltipState.countdownFrames == 0)) {
    rootNode = UiNode_GetRoot(g_UiTooltipState.targetNode);
    /* the dword just before the node: a text resource id, or with UI_NODE_TOOLTIP_REFERENCE_DIRECT_UTF16 the
       text itself */
    commandStream = Thandor_U32ToPointer<uint16_t>(tooltipTarget[-1].nodeFlags); /* 5f-format: UI template tooltip prefix dword (node - 4) */
    if ((tooltipTarget->nodeFlags & UI_NODE_TOOLTIP_REFERENCE_DIRECT_UTF16) == 0) {
      resolvedText = TextResource_Resolve((TextResourceId)(uintptr_t)commandStream);
      commandStream = resolvedText;
    }
    textExtent = RichTextCommandStream_MeasureLine(g_UiTooltipTextStyle,commandStream);
    tileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_TOOLTIP_LEFT,g_UiWindowTextureSource);
    edgeTileWidth = tileSize.logicalWidthPixels;
    frameWidth = textExtent.widthPixels + edgeTileWidth * 2;
    targetLeft = tooltipTarget->left;
    frameTop = tooltipTarget->top - tileSize.logicalHeightPixels;
    targetRight = tooltipTarget->right;
    framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
    if (!framebufferUnavailable) {
      frameLeft = ((targetLeft + targetRight) - frameWidth) >> 1;
      if (rootNode == UI_NODE_NONE) {
        rootNode = tooltipTarget;
      }
      frameRight = frameWidth + frameLeft;
      if (frameLeft < rootNode->left) {
        frameRight = frameRight - (frameLeft - rootNode->left);
        frameLeft = rootNode->left;
      }
      if (rootNode->right < frameRight) {
        frameLeft = frameLeft - (frameRight - rootNode->right);
        frameRight = rootNode->right;
      }
      if (frameTop < rootNode->top) {
        frameTop = frameTop + tooltipTarget->layoutHeight + tileSize.logicalHeightPixels;
      }
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,frameTop,frameLeft,UI_WINDOW_SUBRESOURCE_TOOLTIP_LEFT,
                 g_UiWindowTextureSource,
                 g_FramebufferAccess);
      middleEnd = frameRight - edgeTileWidth;
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,frameTop,middleEnd,
                 UI_WINDOW_SUBRESOURCE_TOOLTIP_RIGHT,g_UiWindowTextureSource,
                 g_FramebufferAccess);
      if (clipRight < middleEnd) {
        middleEnd = clipRight;
      }
      tileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_TOOLTIP_MIDDLE,g_UiWindowTextureSource);
      tileX = edgeTileWidth + frameLeft;
      do {
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipBottom,middleEnd,clipTop,clipLeft,frameTop,tileX,
                   UI_WINDOW_SUBRESOURCE_TOOLTIP_MIDDLE,g_UiWindowTextureSource,
                   g_FramebufferAccess);
        tileX = tileX + tileSize.logicalWidthPixels;
      } while (tileX < middleEnd);
      RichTextCommandStream_DrawSingleLine
                (clipBottom,clipRight,clipTop,clipLeft,g_UiTooltipTextStyle,commandStream,frameTop + 3, /* text inset */
                 edgeTileWidth + frameLeft);
      g_GraphicsFramebufferEndAccess();
    }
  }
  if (g_UiRootNode == UI_ROOT_STACK_END) {
    framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
    if (!framebufferUnavailable) {
      g_GraphicsFramebufferFillRectArgb
                (g_FramebufferHeight,g_FramebufferWidth,0,0,g_FramebufferHeight,g_FramebufferWidth,0,0,
                 UI_TOOLTIP_NO_ROOT_DIM_ARGB,g_FramebufferAccess);
      g_GraphicsFramebufferEndAccess();
    }
  }
  return;
}

/* The tooltip-eligible node under the pointer in the top root, or NULL: none while a node holds the pointer
   capture, no root is open, the pointer is outside the root's box or the hit node is not eligible. */
static UiNodeBase *UiTooltip_FindEligibleNodeAt(UiPixelCoordinate pointerY,UiPixelCoordinate pointerX)

{
  UiNodeBase *hitTestNode;

  if ((g_UiPointerCaptureTarget != UI_NODE_NONE) || (g_UiRootNode == UI_ROOT_STACK_END)) {
    return nullptr;
  }
  if (((g_UiRootNode->base).left > pointerX) || ((g_UiRootNode->base).top > pointerY) ||
      (pointerX >= (g_UiRootNode->base).right) || (pointerY >= (g_UiRootNode->base).bottom)) {
    return nullptr;
  }
  hitTestNode = (*((g_UiRootNode->base).vtable)->hitTest)(pointerY,pointerX,&g_UiRootNode->base);
  if ((hitTestNode == UI_NODE_NONE) || ((hitTestNode->nodeFlags & UI_NODE_TOOLTIP_ELIGIBLE) == 0)) {
    return nullptr;
  }
  return hitTestNode;
}

/* Tracks which node the tooltip belongs to: remembers the pointer position and takes the node under the
   pointer in the top root (only while no button holds a capture, and only tooltip-eligible nodes). When the
   target changes, the tooltip delay starts over and the text of the previous target is prepared again.
*/
void UiTooltip_UpdateHoverTarget(UiPixelCoordinate pointerY,UiPixelCoordinate pointerX)

{
  UiNodeBase *previousTarget;
  UiNodeBase *hoveredNode;

  previousTarget = g_UiTooltipState.targetNode;
  g_UiTooltipState.pointerX = pointerX;
  g_UiTooltipState.pointerY = pointerY;
  g_UiTooltipState.targetNode = nullptr;
  hoveredNode = UiTooltip_FindEligibleNodeAt(pointerY,pointerX);
  if (hoveredNode != nullptr) {
    g_UiTooltipState.targetNode = hoveredNode;
    if (hoveredNode == previousTarget) {
      return;
    }
  }
  g_UiTooltipState.countdownFrames = g_UiTooltipDelayFrames;
  UiTooltip_PrepareTargetText(previousTarget);
}

/* Prepares the tooltip of node (NULL: nothing to do): the dword stored just before the node is its tooltip,
   either a UTF-16 text pointer (UI_NODE_TOOLTIP_REFERENCE_DIRECT_UTF16) or a text resource id. The text is
   measured in the tooltip style and the UI is redrawn.
*/
void UiTooltip_PrepareTargetText(UiNodeBase *node)

{
  uint16_t *commandStream;
  uint16_t *resolvedText;

  if (node != nullptr) {
    /* the last field of the (virtual) node before this one = the dword at node - 4 */
    commandStream = Thandor_U32ToPointer<uint16_t>(node[-1].nodeFlags); /* 5f-format: UI template tooltip prefix dword (node - 4) */
    if ((node->nodeFlags & UI_NODE_TOOLTIP_REFERENCE_DIRECT_UTF16) == 0) {
      resolvedText = TextResource_Resolve((TextResourceId)(uintptr_t)commandStream);
      commandStream = resolvedText;
    }
    RichTextCommandStream_MeasureLine(g_UiTooltipTextStyle,commandStream);
    /* the size of window piece 0xBC is queried but not used */
    g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_TOOLTIP_LEFT,g_UiWindowTextureSource);
    UiRootStack_InvalidateAll();
  }
  return;
}
