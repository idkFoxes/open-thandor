/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/scrollable.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/scrollable.h>
#include <thandor/thandor.h>
#include <stdarg.h>

/* Module data. */

/* int32_t, 14: pixels scrolled per mouse-wheel step in a scrollable control whose child is not a list (UiScrollableControl wheel handler). */
static const int32_t g_UiScrollWheelDefaultStep = 14;

/* int32_t, 15: pixels per mouse-wheel step when the scrollable control's child is a list/text list/timed list control. */
static const int32_t g_UiScrollWheelListStep = 15;

/* Left-button press on a scroll frame (g_UiScrollableControlVtable nonRightPress): finds the scrollbar part
   under the pointer and starts that interaction. An arrow is held (auto-repeat in
   UiScrollableControl_TickAutoScroll), a thumb is grabbed for dragging, a track click pages by half a view
   at once and again on release. Pointer outside both bars: nothing happens.
*/
void UiScrollableControl_BeginPrimaryScrollInteraction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiScrollableControl *control)

{
  uint32_t trackStartOffset;
  int horizontalTrackEnd;
  int localY;
  int localX;
  int verticalTrackBottom;
  Bool8 horizontalBarHit;
  GraphicsTextureLogicalSize textureSize;

  localX = pointerX - (control->base).left;
  localY = pointerY - (control->base).top;
  /* Horizontal bar first: the pointer must be inside the bar and between the vertical bar(s). */
  horizontalBarHit = false;
  if ((control->scrollStateFlags &
      (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
    textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW,g_UiWindowTextureSource);
    if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM) == 0) {
      horizontalBarHit = (-1 < localY) && (localY < (int)textureSize.logicalHeightPixels);
    }
    else {
      horizontalBarHit =
           (localY < (control->base).layoutHeight) &&
           ((int)((control->base).layoutHeight - textureSize.logicalHeightPixels) <= localY);
    }
    if (horizontalBarHit) {
      horizontalTrackEnd = (control->base).layoutWidth;
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW,g_UiWindowTextureSource);
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_RIGHT) != 0) {
        horizontalTrackEnd = horizontalTrackEnd - textureSize.logicalWidthPixels;
      }
      trackStartOffset = 0;
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) != 0) {
        trackStartOffset = textureSize.logicalWidthPixels;
      }
      horizontalBarHit =
           ((int)trackStartOffset <= localX) && (localX < horizontalTrackEnd);
    }
  }
  if (!horizontalBarHit) {
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) == 0) {
      return;
    }
    textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW,g_UiWindowTextureSource);
    if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_RIGHT) == 0) {
      if (localX < 0) {
        return;
      }
      if ((int)textureSize.logicalWidthPixels <= localX) {
        return;
      }
    }
    else {
      if ((control->base).layoutWidth <= localX) {
        return;
      }
      if (localX < (int)((control->base).layoutWidth - textureSize.logicalWidthPixels)) {
        return;
      }
    }
    verticalTrackBottom = (control->base).layoutHeight;
    textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW,g_UiWindowTextureSource);
    if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM) != 0) {
      verticalTrackBottom = verticalTrackBottom - textureSize.logicalHeightPixels;
    }
    trackStartOffset = 0;
    if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) != 0) {
      trackStartOffset = textureSize.logicalHeightPixels;
    }
    if (localY < (int)trackStartOffset) {
      return;
    }
    if (verticalTrackBottom <= localY) {
      return;
    }
    if (localY < control->verticalThumbTop) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW,g_UiWindowTextureSource);
      if ((int)(localY - trackStartOffset) < (int)textureSize.logicalHeightPixels) {
        control->scrollStateFlags =
             control->scrollStateFlags |
             (UI_SCROLL_VERTICAL_DECREMENT_ACTIVE|UI_SCROLL_PRIMARY_INTERACTION_ACTIVE);
        UiNode_InvalidateRoot(&control->base);
        return;
      }
      control->scrollOffsetY = control->scrollOffsetY + ((int)control->viewportHeight >> 1);
      control->scrollStateFlags =
           control->scrollStateFlags | UI_SCROLL_VERTICAL_TRACK_BEFORE_THUMB_ACTIVE;
    }
    else {
      if (localY < control->verticalThumbBottom) {
        control->pointerAnchorY = localY - control->verticalThumbTop;
        control->scrollStateFlags = control->scrollStateFlags | UI_SCROLL_VERTICAL_THUMB_ACTIVE;
        UiNode_InvalidateRoot(&control->base);
        return;
      }
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW,g_UiWindowTextureSource);
      if (verticalTrackBottom - localY <= (int)textureSize.logicalHeightPixels) {
        control->scrollStateFlags =
             control->scrollStateFlags |
             (UI_SCROLL_VERTICAL_INCREMENT_ACTIVE|UI_SCROLL_PRIMARY_INTERACTION_ACTIVE);
        UiNode_InvalidateRoot(&control->base);
        return;
      }
      control->scrollOffsetY = control->scrollOffsetY - ((int)control->viewportHeight >> 1);
      control->scrollStateFlags =
           control->scrollStateFlags | UI_SCROLL_VERTICAL_TRACK_AFTER_THUMB_ACTIVE;
    }
  }
  else if (localX < control->horizontalThumbLeft) {
    textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW,g_UiWindowTextureSource);
    if ((int)(localX - trackStartOffset)< (int)textureSize.logicalWidthPixels) {
      control->scrollStateFlags =
           control->scrollStateFlags |
           (UI_SCROLL_HORIZONTAL_DECREMENT_ACTIVE|UI_SCROLL_PRIMARY_INTERACTION_ACTIVE);
      UiNode_InvalidateRoot(&control->base);
      return;
    }
    control->scrollOffsetX = control->scrollOffsetX + ((int)control->viewportWidth >> 1);
    control->scrollStateFlags =
         control->scrollStateFlags | UI_SCROLL_HORIZONTAL_TRACK_BEFORE_THUMB_ACTIVE;
  }
  else {
    if (localX < control->horizontalThumbRight) {
      control->pointerAnchorX = localX - control->horizontalThumbLeft;
      control->scrollStateFlags = control->scrollStateFlags | UI_SCROLL_HORIZONTAL_THUMB_ACTIVE;
      UiNode_InvalidateRoot(&control->base);
      return;
    }
    textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW,g_UiWindowTextureSource);
    if (horizontalTrackEnd - localX <=(int)textureSize.logicalWidthPixels) {
      control->scrollStateFlags =
           control->scrollStateFlags |
           (UI_SCROLL_HORIZONTAL_INCREMENT_ACTIVE|UI_SCROLL_PRIMARY_INTERACTION_ACTIVE);
      UiNode_InvalidateRoot(&control->base);
      return;
    }
    control->scrollOffsetX = control->scrollOffsetX - ((int)control->viewportWidth >> 1);
    control->scrollStateFlags =
         control->scrollStateFlags | UI_SCROLL_HORIZONTAL_TRACK_AFTER_THUMB_ACTIVE;
  }
  /* Track clicks page by half a viewport. */
  UiScrollableControl_RefreshChildAndScrollThumbs(control);
  UiNode_InvalidateRoot(&control->base);
}

/* Left-button release on a scroll frame (g_UiScrollableControlVtable nonRightRelease): a held track pages by
   another half view, then every scrollbar interaction ends and the frame is redrawn.
*/
void UiScrollableControl_EndPrimaryScrollInteraction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiScrollableControl *control)

{
  if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_TRACK_BEFORE_THUMB_ACTIVE) != 0) {
    control->scrollOffsetX = control->scrollOffsetX + ((int)control->viewportWidth >> 1);
    UiScrollableControl_RefreshChildAndScrollThumbs(control);
  }
  if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_TRACK_AFTER_THUMB_ACTIVE) != 0) {
    control->scrollOffsetX = control->scrollOffsetX - ((int)control->viewportWidth >> 1);
    UiScrollableControl_RefreshChildAndScrollThumbs(control);
  }
  if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_TRACK_BEFORE_THUMB_ACTIVE) != 0) {
    control->scrollOffsetY = control->scrollOffsetY + ((int)control->viewportHeight >> 1);
    UiScrollableControl_RefreshChildAndScrollThumbs(control);
  }
  if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_TRACK_AFTER_THUMB_ACTIVE) != 0) {
    control->scrollOffsetY = control->scrollOffsetY - ((int)control->viewportHeight >> 1);
    UiScrollableControl_RefreshChildAndScrollThumbs(control);
  }
  control->scrollStateFlags = control->scrollStateFlags &
       ~(UI_SCROLL_VERTICAL_PARTS_ACTIVE|UI_SCROLL_HORIZONTAL_PARTS_ACTIVE|UI_SCROLL_PRIMARY_INTERACTION_ACTIVE);
  UiNode_InvalidateRoot(&control->base);
}

/* Left-button drag on a scroll frame (g_UiScrollableControlVtable nonRightDrag): a grabbed thumb follows the
   pointer (the scroll offset is the thumb position scaled from the free track to the scroll range); a held
   arrow repeats (UI_SCROLL_PRIMARY_INTERACTION_ACTIVE) only while the pointer stays on it. A held track
   ignores the drag.
*/
void UiScrollableControl_UpdatePrimaryScrollDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiScrollableControl *control)

{
  int thumbOffset;
  int pointerAnchor;
  int localX;
  int localY;
  uint32_t arrowSize;
  int trackLength;
  uint32_t arrowStart;
  int arrowEnd;
  Bool8 inArrowBar;
  Bool8 arrowHovered;
  GraphicsTextureLogicalSize textureSize;

  if ((control->scrollStateFlags &
      (UI_SCROLL_VERTICAL_TRACK_AFTER_THUMB_ACTIVE|UI_SCROLL_VERTICAL_TRACK_BEFORE_THUMB_ACTIVE|
       UI_SCROLL_HORIZONTAL_TRACK_AFTER_THUMB_ACTIVE|UI_SCROLL_HORIZONTAL_TRACK_BEFORE_THUMB_ACTIVE)
      ) != 0) {
    return;
  }
  if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_THUMB_ACTIVE) != 0) {
    pointerAnchor = control->pointerAnchorX;
    trackLength = (control->base).layoutWidth;
    textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW,g_UiWindowTextureSource);
    thumbOffset = ((pointerX - (control->base).left) - pointerAnchor) - textureSize.logicalWidthPixels;
    trackLength = trackLength + textureSize.logicalWidthPixels * -2;
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW,g_UiWindowTextureSource);
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) != 0) {
        thumbOffset = thumbOffset - textureSize.logicalWidthPixels;
      }
      trackLength = trackLength - textureSize.logicalWidthPixels;
    }
    control->scrollOffsetX =
         (UiPixelOffset)
         (((int64_t)(int)(control->contentWidth - control->viewportWidth) * (int64_t)thumbOffset) /
         (int64_t)-((trackLength - control->horizontalThumbRight) + control->horizontalThumbLeft));
    UiScrollableControl_RefreshChildAndScrollThumbs(control);
    UiNode_InvalidateRoot(&control->base);
    return;
  }
  if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_THUMB_ACTIVE) != 0) {
    pointerAnchor = control->pointerAnchorY;
    trackLength = (control->base).layoutHeight;
    textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW,g_UiWindowTextureSource);
    thumbOffset = ((pointerY - (control->base).top) - pointerAnchor) - textureSize.logicalHeightPixels;
    trackLength = trackLength + textureSize.logicalHeightPixels * -2;
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW,
                                                          g_UiWindowTextureSource);
      if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) != 0) {
        thumbOffset = thumbOffset - textureSize.logicalHeightPixels;
      }
      trackLength = trackLength - textureSize.logicalHeightPixels;
    }
    control->scrollOffsetY =
         (UiPixelOffset)
         (((int64_t)(int)(control->contentHeight - control->viewportHeight) * (int64_t)thumbOffset) /
         (int64_t)-((trackLength - control->verticalThumbBottom) + control->verticalThumbTop));
    UiScrollableControl_RefreshChildAndScrollThumbs(control);
    UiNode_InvalidateRoot(&control->base);
    return;
  }
  /* A held arrow button: repeat (PRIMARY_INTERACTION_ACTIVE) only while the pointer stays on it. */
  localY = pointerY - (control->base).top;
  localX = pointerX - (control->base).left;
  arrowHovered = false;
  if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_DECREMENT_ACTIVE) != 0) {
    textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW,g_UiWindowTextureSource);
    arrowSize = textureSize.logicalWidthPixels;
    if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) == 0) {
      inArrowBar = (localY < (control->base).layoutHeight) &&
                   ((int)((control->base).layoutHeight - textureSize.logicalHeightPixels) <= localY);
    }
    else {
      inArrowBar = (-1 < localY) && (localY < (int)textureSize.logicalHeightPixels);
    }
    if (inArrowBar) {
      arrowStart = 0;
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) != 0) {
        textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW,
                                                            g_UiWindowTextureSource);
        arrowStart = textureSize.logicalWidthPixels;
      }
      arrowHovered = ((int)arrowStart <= localX) &&
                     (localX < (int)(arrowStart + arrowSize));
    }
  }
  else if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_INCREMENT_ACTIVE) != 0) {
    textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW,g_UiWindowTextureSource);
    arrowSize = textureSize.logicalWidthPixels;
    if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) == 0) {
      inArrowBar = (localY < (control->base).layoutHeight) &&
                   ((int)((control->base).layoutHeight - textureSize.logicalHeightPixels) <= localY);
    }
    else {
      inArrowBar = (-1 < localY) && (localY < (int)textureSize.logicalHeightPixels);
    }
    if (inArrowBar) {
      arrowEnd = (control->base).layoutWidth;
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_RIGHT) != 0) {
        textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW,
                                                            g_UiWindowTextureSource);
        arrowEnd = arrowEnd - textureSize.logicalWidthPixels;
      }
      arrowHovered = (localX < arrowEnd) && ((int)(arrowEnd - arrowSize) <= localX);
    }
  }
  else if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_DECREMENT_ACTIVE) != 0) {
    textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW,g_UiWindowTextureSource);
    arrowSize = textureSize.logicalHeightPixels;
    if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) == 0) {
      inArrowBar = (localX < (control->base).layoutWidth) &&
                   ((int)((control->base).layoutWidth - textureSize.logicalWidthPixels) <= localX);
    }
    else {
      inArrowBar = (-1 < localX) && (localX < (int)textureSize.logicalWidthPixels);
    }
    if (inArrowBar) {
      arrowStart = 0;
      if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) != 0) {
        textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW,
                                                            g_UiWindowTextureSource);
        arrowStart = textureSize.logicalHeightPixels;
      }
      arrowHovered = ((int)arrowStart <= localY) && (localY < (int)(arrowStart + arrowSize));
    }
  }
  else if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_INCREMENT_ACTIVE) != 0) {
    textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW,g_UiWindowTextureSource);
    arrowSize = textureSize.logicalHeightPixels;
    if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) == 0) {
      inArrowBar = (localX < (control->base).layoutWidth) &&
                   ((int)((control->base).layoutWidth - textureSize.logicalWidthPixels) <= localX);
    }
    else {
      inArrowBar = (-1 < localX) && (localX < (int)textureSize.logicalWidthPixels);
    }
    if (inArrowBar) {
      arrowEnd = (control->base).layoutHeight;
      if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM) != 0) {
        textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW,
                                                            g_UiWindowTextureSource);
        arrowEnd = arrowEnd - textureSize.logicalHeightPixels;
      }
      arrowHovered = (localY < arrowEnd) && ((int)(arrowEnd - arrowSize) <= localY);
    }
  }
  else {
    return;
  }
  if (arrowHovered) {
    if ((control->scrollStateFlags & UI_SCROLL_PRIMARY_INTERACTION_ACTIVE) != 0) {
      return;
    }
    control->scrollStateFlags = control->scrollStateFlags | UI_SCROLL_PRIMARY_INTERACTION_ACTIVE;
  }
  else {
    if ((control->scrollStateFlags & UI_SCROLL_PRIMARY_INTERACTION_ACTIVE) == 0) {
      return;
    }
    control->scrollStateFlags = control->scrollStateFlags & ~UI_SCROLL_PRIMARY_INTERACTION_ACTIVE;
  }
  UiScrollableControl_RefreshChildAndScrollThumbs(control);
  UiNode_InvalidateRoot(&control->base);
}

/* Right-button drag on a scroll frame (g_UiScrollableControlVtable rightDrag): pans the content by the
   pointer movement since the press (on the axes that have a bar) and puts the pointer back to the press
   position, so the pointer stays in place while the content moves.
*/
void UiScrollableControl_UpdateSecondaryScrollDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiScrollableControl *control)

{
  int pointerDeltaX;
  int pointerDeltaY;
  
  pointerDeltaX = pointerX - control->pointerAnchorX;
  pointerDeltaY = pointerY - control->pointerAnchorY;
  if ((control->scrollStateFlags &
      (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
    control->scrollOffsetX = control->scrollOffsetX - pointerDeltaX;
  }
  if ((control->scrollStateFlags & (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT))
      != 0) {
    control->scrollOffsetY = control->scrollOffsetY - pointerDeltaY;
  }
  UiScrollableControl_RefreshChildAndScrollThumbs(control);
  UiNode_InvalidateRoot(&control->base);
  g_PointerSetPosition(control->pointerAnchorY,control->pointerAnchorX);
}

/* Per-frame tick of a scroll frame (g_UiScrollableControlVtable tick): while an arrow is held under the
   pointer, scrolls by autoScrollStepX/Y in the arrow's direction and redraws.
*/
void UiScrollableControl_TickAutoScroll(UiScrollableControl *control)

{
  if ((control->scrollStateFlags & UI_SCROLL_PRIMARY_INTERACTION_ACTIVE) != 0) {
    if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_DECREMENT_ACTIVE) != 0) {
      control->scrollOffsetX = control->scrollOffsetX + control->autoScrollStepX;
    }
    if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_INCREMENT_ACTIVE) != 0) {
      control->scrollOffsetX = control->scrollOffsetX - control->autoScrollStepX;
    }
    if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_DECREMENT_ACTIVE) != 0) {
      control->scrollOffsetY = control->scrollOffsetY + control->autoScrollStepY;
    }
    if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_INCREMENT_ACTIVE) != 0) {
      control->scrollOffsetY = control->scrollOffsetY - control->autoScrollStepY;
    }
    UiScrollableControl_RefreshChildAndScrollThumbs(control);
    UiNode_InvalidateRoot(&control->base);
  }
}

/* Mouse wheel over a scroll frame (g_UiScrollableControlVtable pointerWheel): scrolls vertically by
   wheelDelta steps, g_UiScrollWheelListStep pixels per step when the content is a list control, else
   g_UiScrollWheelDefaultStep. Ignored without a vertical bar, during a button interaction or when suppressed.
*/
void UiScrollableControl_HandlePointerWheel
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiScrollableControl *control)

{
  UiNodeBase *firstChildNode;
  int scrollStep;

  if ((control->scrollStateFlags & (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) == 0) {
    return;
  }
  if ((control->scrollStateFlags &
      (UI_SCROLL_PRIMARY_INTERACTION_ACTIVE|UI_SCROLL_SECONDARY_INTERACTION_ACTIVE)) != 0) {
    return;
  }
  if (((control->base).nodeFlags & UI_NODE_SUPPRESSED) != 0) {
    return;
  }
  if (wheelDelta == 0) {
    return;
  }
  firstChildNode = (control->base).firstChild;
  scrollStep = g_UiScrollWheelDefaultStep;
  if ((firstChildNode != UI_NODE_NONE) &&
     ((firstChildNode->vtable == &g_UiTextListControlVtable) ||
      (firstChildNode->vtable == &g_UiListControlVtable))) {
    scrollStep = g_UiScrollWheelListStep;
  }
  control->scrollOffsetY = control->scrollOffsetY + wheelDelta * scrollStep;
  UiScrollableControl_RefreshChildAndScrollThumbs(control);
  UiNode_InvalidateRoot(&control->base);
}

/* Relocation of a scroll frame loaded from a serialized UI tree (g_UiScrollableControlVtable relocate):
   relocates the children, takes the content size from the content child's right/bottom offsets and scrolls
   back to the origin.
*/
void UiScrollableControl_RelocateChildren(UiSerializedRelocationDelta relocationDelta,UiScrollableControl *control)

{
  UiNodeBase *contentChild;
  UiPixelExtent contentHeight;
  
  UiContainer_RelocateChildren(relocationDelta,&control->base);
  contentChild = (control->base).firstChild;
  if (contentChild != UI_NODE_NONE) {
    contentHeight = contentChild->bottomOffset;
    control->contentWidth = contentChild->rightOffset;
    control->contentHeight = contentHeight;
    control->scrollOffsetX = 0;
    control->scrollOffsetY = 0;
  }
}

/* The part of a scroll frame not yet taken by bars and frame pieces, relative to the control. */
typedef struct UiScrollFrameContentRect {
  uint32_t left;
  int top;
  int right;
  int bottom;
} UiScrollFrameContentRect;

/* An arrow piece shows pressed while its active flag and the primary interaction flag are both set. */
static Bool8 UiScrollableControl_IsArrowPressed(const UiScrollableControl *control,uint32_t arrowActiveFlag)
{
  return ((control->scrollStateFlags & arrowActiveFlag) != 0) &&
         ((control->scrollStateFlags & UI_SCROLL_PRIMARY_INTERACTION_ACTIVE) != 0);
}

/* Draws the horizontal scroll bar (at the top or bottom) and takes its height from the content rect. */
static void UiScrollableControl_DrawHorizontalScrollbar
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiScrollableControl *control,UiScrollFrameContentRect *content)
{
  GraphicsTextureLogicalSize textureSize;
  uint32_t arrowLength;
  uint32_t barLeft;
  uint32_t thumbCapWidth;
  int barTop;
  int barRight;
  int trackStart;
  int trackEnd;
  int trackAfterThumbClipLeft;
  int thumbLeft;
  int thumbEndLeft;
  GraphicsSubresourceIndex subresource;

  if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM) == 0) {
    barTop = 0;
    textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW,
                                                        g_UiWindowTextureSource);
    arrowLength = textureSize.logicalWidthPixels;
    content->top = content->top + textureSize.logicalHeightPixels;
  }
  else {
    textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW,
                                                        g_UiWindowTextureSource);
    arrowLength = textureSize.logicalWidthPixels;
    barTop = content->bottom - textureSize.logicalHeightPixels;
    content->bottom = content->bottom - textureSize.logicalHeightPixels;
  }
  /* The bar leaves room for a vertical bar on either side. */
  textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW,g_UiWindowTextureSource);
  barLeft = content->left;
  if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) != 0) {
    barLeft = textureSize.logicalWidthPixels;
  }
  barRight = content->right;
  if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_RIGHT) != 0) {
    barRight = content->right - textureSize.logicalWidthPixels;
  }

  subresource = UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW;
  if (UiScrollableControl_IsArrowPressed(control,UI_SCROLL_HORIZONTAL_DECREMENT_ACTIVE)) {
    subresource = UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW + UI_WINDOW_SUBRESOURCE_PRESSED_OFFSET;
  }
  g_GraphicsTextureSourceBlitSourceAlpha
            (clipBottom,clipRight,clipTop,clipLeft,barTop + (control->base).top,barLeft + (control->base).left,
             subresource,g_UiWindowTextureSource,g_FramebufferAccess);
  trackStart = barLeft + arrowLength;
  trackEnd = barRight - arrowLength;
  subresource = UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW_RIGHT;
  if (UiScrollableControl_IsArrowPressed(control,UI_SCROLL_HORIZONTAL_INCREMENT_ACTIVE)) {
    subresource = UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW_RIGHT + UI_WINDOW_SUBRESOURCE_PRESSED_OFFSET;
  }
  g_GraphicsTextureSourceBlitSourceAlpha
            (clipBottom,clipRight,clipTop,clipLeft,barTop + (control->base).top,trackEnd + (control->base).left,
             subresource,g_UiWindowTextureSource,g_FramebufferAccess);

  /* Track before and after the thumb; both are tiled from the track start. */
  subresource = UI_WINDOW_SUBRESOURCE_HORIZONTAL_TRACK;
  if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_TRACK_BEFORE_THUMB_ACTIVE) != 0) {
    subresource = UI_WINDOW_SUBRESOURCE_HORIZONTAL_TRACK + UI_WINDOW_SUBRESOURCE_PRESSED_OFFSET;
  }
  UiWindow_BlitTiledHorizontalEdge
            (clipBottom,clipRight,clipTop,clipLeft,subresource,control->horizontalThumbLeft,barTop,trackStart,control);
  trackAfterThumbClipLeft = control->horizontalThumbRight;
  if (trackAfterThumbClipLeft < clipLeft) {
    trackAfterThumbClipLeft = clipLeft;
  }
  subresource = UI_WINDOW_SUBRESOURCE_HORIZONTAL_TRACK;
  if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_TRACK_AFTER_THUMB_ACTIVE) != 0) {
    subresource = UI_WINDOW_SUBRESOURCE_HORIZONTAL_TRACK + UI_WINDOW_SUBRESOURCE_PRESSED_OFFSET;
  }
  UiWindow_BlitTiledHorizontalEdge
            (clipBottom,clipRight,clipTop,trackAfterThumbClipLeft,subresource,trackEnd,barTop,trackStart,control);

  /* Thumb: start cap, end cap, tiled middle. */
  textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_THUMB,
                                                      g_UiWindowTextureSource);
  thumbCapWidth = textureSize.logicalWidthPixels;
  thumbLeft = control->horizontalThumbLeft;
  thumbEndLeft = control->horizontalThumbRight - thumbCapWidth;
  if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_THUMB_ACTIVE) == 0) {
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,barTop + (control->base).top,
               thumbLeft + (control->base).left,UI_WINDOW_SUBRESOURCE_HORIZONTAL_THUMB,g_UiWindowTextureSource,
               g_FramebufferAccess);
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,barTop + (control->base).top,
               thumbEndLeft + (control->base).left,UI_WINDOW_SUBRESOURCE_HORIZONTAL_THUMB_END,g_UiWindowTextureSource,
               g_FramebufferAccess);
    UiWindow_BlitTiledHorizontalEdge
              (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_HORIZONTAL_THUMB_MIDDLE,thumbEndLeft,
               barTop,thumbLeft + thumbCapWidth,control);
  }
  else {
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,barTop + (control->base).top,
               thumbLeft + (control->base).left,
               UI_WINDOW_SUBRESOURCE_HORIZONTAL_THUMB + UI_WINDOW_SUBRESOURCE_THUMB_ACTIVE_OFFSET,
               g_UiWindowTextureSource,g_FramebufferAccess);
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,barTop + (control->base).top,
               thumbEndLeft + (control->base).left,
               UI_WINDOW_SUBRESOURCE_HORIZONTAL_THUMB_END + UI_WINDOW_SUBRESOURCE_THUMB_ACTIVE_OFFSET,
               g_UiWindowTextureSource,g_FramebufferAccess);
    UiWindow_BlitTiledHorizontalEdge
              (clipBottom,clipRight,clipTop,clipLeft,
               UI_WINDOW_SUBRESOURCE_HORIZONTAL_THUMB_MIDDLE + UI_WINDOW_SUBRESOURCE_PRESSED_OFFSET,thumbEndLeft,
               barTop,thumbLeft + thumbCapWidth,control);
  }
}

/* Draws the vertical scroll bar (at the left or right) and takes its width from the content rect. */
static void UiScrollableControl_DrawVerticalScrollbar
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiScrollableControl *control,UiScrollFrameContentRect *content)
{
  GraphicsTextureLogicalSize textureSize;
  uint32_t arrowLength;
  uint32_t barLeft;
  uint32_t thumbCapHeight;
  int trackStart;
  int trackEnd;
  int trackAfterThumbClipTop;
  int thumbTop;
  int thumbEndTop;
  GraphicsSubresourceIndex subresource;

  textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW,g_UiWindowTextureSource);
  arrowLength = textureSize.logicalHeightPixels;
  if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_RIGHT) == 0) {
    barLeft = content->left;
    content->left = content->left + textureSize.logicalWidthPixels;
  }
  else {
    barLeft = content->right - textureSize.logicalWidthPixels;
    content->right = content->right - textureSize.logicalWidthPixels;
  }

  subresource = UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW;
  if (UiScrollableControl_IsArrowPressed(control,UI_SCROLL_VERTICAL_DECREMENT_ACTIVE)) {
    subresource = UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW + UI_WINDOW_SUBRESOURCE_PRESSED_OFFSET;
  }
  g_GraphicsTextureSourceBlitSourceAlpha
            (clipBottom,clipRight,clipTop,clipLeft,content->top + (control->base).top,barLeft + (control->base).left,
             subresource,g_UiWindowTextureSource,g_FramebufferAccess);
  trackStart = content->top + arrowLength;
  trackEnd = content->bottom - arrowLength;
  subresource = UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW_DOWN;
  if (UiScrollableControl_IsArrowPressed(control,UI_SCROLL_VERTICAL_INCREMENT_ACTIVE)) {
    subresource = UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW_DOWN + UI_WINDOW_SUBRESOURCE_PRESSED_OFFSET;
  }
  g_GraphicsTextureSourceBlitSourceAlpha
            (clipBottom,clipRight,clipTop,clipLeft,trackEnd + (control->base).top,barLeft + (control->base).left,
             subresource,g_UiWindowTextureSource,g_FramebufferAccess);

  /* Track before and after the thumb; both are tiled from the track start. */
  subresource = UI_WINDOW_SUBRESOURCE_VERTICAL_TRACK;
  if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_TRACK_BEFORE_THUMB_ACTIVE) != 0) {
    subresource = UI_WINDOW_SUBRESOURCE_VERTICAL_TRACK + UI_WINDOW_SUBRESOURCE_PRESSED_OFFSET;
  }
  UiWindow_BlitTiledVerticalEdge
            (clipBottom,clipRight,clipTop,clipLeft,subresource,control->verticalThumbTop,trackStart,barLeft,control);
  trackAfterThumbClipTop = control->verticalThumbBottom;
  if (trackAfterThumbClipTop < clipTop) {
    trackAfterThumbClipTop = clipTop;
  }
  subresource = UI_WINDOW_SUBRESOURCE_VERTICAL_TRACK;
  if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_TRACK_AFTER_THUMB_ACTIVE) != 0) {
    subresource = UI_WINDOW_SUBRESOURCE_VERTICAL_TRACK + UI_WINDOW_SUBRESOURCE_PRESSED_OFFSET;
  }
  UiWindow_BlitTiledVerticalEdge
            (clipBottom,clipRight,trackAfterThumbClipTop,clipLeft,subresource,trackEnd,trackStart,barLeft,control);

  /* Thumb: start cap, end cap, tiled middle. */
  textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_VERTICAL_THUMB,g_UiWindowTextureSource);
  thumbCapHeight = textureSize.logicalHeightPixels;
  thumbTop = control->verticalThumbTop;
  thumbEndTop = control->verticalThumbBottom - thumbCapHeight;
  if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_THUMB_ACTIVE) == 0) {
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,thumbTop + (control->base).top,
               barLeft + (control->base).left,UI_WINDOW_SUBRESOURCE_VERTICAL_THUMB,g_UiWindowTextureSource,
               g_FramebufferAccess);
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,thumbEndTop + (control->base).top,
               barLeft + (control->base).left,UI_WINDOW_SUBRESOURCE_VERTICAL_THUMB_END,g_UiWindowTextureSource,
               g_FramebufferAccess);
    UiWindow_BlitTiledVerticalEdge
              (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_VERTICAL_THUMB_MIDDLE,thumbEndTop,
               thumbTop + thumbCapHeight,barLeft,control);
  }
  else {
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,thumbTop + (control->base).top,
               barLeft + (control->base).left,
               UI_WINDOW_SUBRESOURCE_VERTICAL_THUMB + UI_WINDOW_SUBRESOURCE_THUMB_ACTIVE_OFFSET,
               g_UiWindowTextureSource,g_FramebufferAccess);
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,thumbEndTop + (control->base).top,
               barLeft + (control->base).left,
               UI_WINDOW_SUBRESOURCE_VERTICAL_THUMB_END + UI_WINDOW_SUBRESOURCE_THUMB_ACTIVE_OFFSET,
               g_UiWindowTextureSource,g_FramebufferAccess);
    UiWindow_BlitTiledVerticalEdge
              (clipBottom,clipRight,clipTop,clipLeft,
               UI_WINDOW_SUBRESOURCE_VERTICAL_THUMB_MIDDLE + UI_WINDOW_SUBRESOURCE_PRESSED_OFFSET,thumbEndTop,
               thumbTop + thumbCapHeight,barLeft,control);
  }
}

/* Draws one frame style around the content rect (four corners, then top, left, right and bottom edges, all
   pieces relative to firstSubresource) and shrinks the rect by the frame. */
static void UiScrollableControl_DrawFrameStyle
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiScrollableControl *control,GraphicsSubresourceIndex firstSubresource,
          UiScrollFrameContentRect *content)
{
  GraphicsTextureLogicalSize textureSize;
  uint32_t cornerWidth;

  textureSize = g_GraphicsTextureSourceGetLogicalSize(firstSubresource,g_UiWindowTextureSource);
  cornerWidth = textureSize.logicalWidthPixels;
  content->bottom = content->bottom - textureSize.logicalHeightPixels;
  content->right = content->right - cornerWidth;
  g_GraphicsTextureSourceBlitSourceAlpha
            (clipBottom,clipRight,clipTop,clipLeft,content->top + (control->base).top,
             content->left + (control->base).left,firstSubresource,g_UiWindowTextureSource,g_FramebufferAccess);
  g_GraphicsTextureSourceBlitSourceAlpha
            (clipBottom,clipRight,clipTop,clipLeft,content->top + (control->base).top,
             content->right + (control->base).left,firstSubresource + UI_WINDOW_FRAME_TOP_RIGHT,
             g_UiWindowTextureSource,g_FramebufferAccess);
  g_GraphicsTextureSourceBlitSourceAlpha
            (clipBottom,clipRight,clipTop,clipLeft,content->bottom + (control->base).top,
             content->left + (control->base).left,firstSubresource + UI_WINDOW_FRAME_BOTTOM_LEFT,
             g_UiWindowTextureSource,g_FramebufferAccess);
  g_GraphicsTextureSourceBlitSourceAlpha
            (clipBottom,clipRight,clipTop,clipLeft,content->bottom + (control->base).top,
             content->right + (control->base).left,firstSubresource + UI_WINDOW_FRAME_BOTTOM_RIGHT,
             g_UiWindowTextureSource,g_FramebufferAccess);
  UiWindow_BlitTiledHorizontalEdge
            (clipBottom,clipRight,clipTop,clipLeft,firstSubresource + UI_WINDOW_FRAME_TOP,
             content->right,content->top,content->left + cornerWidth,control);
  content->top = content->top + textureSize.logicalHeightPixels;
  UiWindow_BlitTiledVerticalEdge
            (clipBottom,clipRight,clipTop,clipLeft,firstSubresource + UI_WINDOW_FRAME_LEFT,
             content->bottom,content->top,content->left,control);
  UiWindow_BlitTiledVerticalEdge
            (clipBottom,clipRight,clipTop,clipLeft,firstSubresource + UI_WINDOW_FRAME_RIGHT,
             content->bottom,content->top,content->right,control);
  content->left = content->left + cornerWidth;
  UiWindow_BlitTiledHorizontalEdge
            (clipBottom,clipRight,clipTop,clipLeft,firstSubresource + UI_WINDOW_FRAME_BOTTOM,
             content->right,content->bottom,content->left,control);
}

/* Draws a scroll frame (g_UiScrollableControlVtable drawClipped) from g_UiWindowTextureSource pieces: the
   horizontal and vertical bars (arrows, track and thumb, pressed/active pieces while held), the optional
   frame style and interior fill, then the content child clipped to the remaining view.
*/
void UiScrollableControl_DrawFrameContentAndScrollbars
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiScrollableControl *control)

{
  GraphicsSubresourceIndex subresource;
  UiScrollFrameContentRect content;
  int viewLeft;
  int viewTop;
  int viewRight;
  int viewBottom;

  if (g_GraphicsFramebufferBeginAccess()) {
    return;
  }
  content.left = 0;
  content.top = 0;
  content.right = (control->base).layoutWidth;
  content.bottom = (control->base).layoutHeight;
  if ((control->scrollStateFlags & (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
    UiScrollableControl_DrawHorizontalScrollbar(clipBottom,clipRight,clipTop,clipLeft,control,&content);
  }
  if ((control->scrollStateFlags & (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
    UiScrollableControl_DrawVerticalScrollbar(clipBottom,clipRight,clipTop,clipLeft,control,&content);
  }
  if ((control->scrollStateFlags & UI_SCROLL_FRAME_STYLE_A) != 0) {
    UiScrollableControl_DrawFrameStyle
              (clipBottom,clipRight,clipTop,clipLeft,control,UI_WINDOW_SUBRESOURCE_FRAME_A_FIRST,&content);
  }
  if ((control->scrollStateFlags & UI_SCROLL_FRAME_STYLE_B) != 0) {
    UiScrollableControl_DrawFrameStyle
              (clipBottom,clipRight,clipTop,clipLeft,control,UI_WINDOW_SUBRESOURCE_FRAME_B_FIRST,&content);
  }
  if ((control->scrollStateFlags & (UI_SCROLL_FILL_INTERIOR|UI_SCROLL_FILL_INTERIOR_TEXTURED)) != 0) {
    subresource = 0;
    if ((control->scrollStateFlags & UI_SCROLL_FILL_INTERIOR_TEXTURED) != 0) {
      subresource = UI_WINDOW_SUBRESOURCE_INTERIOR;
    }
    UiWindow_BlitTiledInterior
              (clipBottom,clipRight,clipTop,clipLeft,subresource,content.bottom,content.right,content.top,
               content.left,control);
  }
  g_GraphicsFramebufferEndAccess();
  viewLeft = content.left + (control->base).left;
  viewTop = content.top + (control->base).top;
  viewRight = content.right + (control->base).left;
  viewBottom = content.bottom + (control->base).top;
  if (viewLeft < clipLeft) {
    viewLeft = clipLeft;
  }
  if (viewTop < clipTop) {
    viewTop = clipTop;
  }
  if (clipRight < viewRight) {
    viewRight = clipRight;
  }
  if (clipBottom < viewBottom) {
    viewBottom = clipBottom;
  }
  UiContainer_DrawIntersectingChildren(viewBottom,viewRight,viewTop,viewLeft,&control->base);
}

/* Layout of a scroll frame (list boxes, text views): the single child is the content, its size is taken from
   its right/bottom offsets. Decides which scroll bars are needed (a bar can reduce the room for the content
   and so make the other one necessary; bits 4..7 of scrollStateFlags say which bar positions are allowed),
   clamps the scroll offsets so no empty space shows, places the content (shifted by the scroll offsets, the
   optional border and a left/top bar) and computes the thumb positions from the scroll offsets.
   The win.gfx subresources used for sizes: 0x6A/0x72 border styles (flags 0x400/0x800), 0x5A horizontal bar
   arrow, 0x5E vertical bar arrow, 0xC0/0xC2 horizontal/vertical thumb caps (half the minimum thumb).
*/
void UiScrollableControl_RebuildViewportAndScrollbars(UiScrollableControl *control)

{
  UiNodeBase *contentChild;
  UiPixelExtent childWidth;
  UiPixelExtent childHeight;
  uint32_t minThumbLength;
  uint32_t arrowSize;
  uint32_t thumbLength;
  UiPixelOffset offsetX;
  UiPixelOffset offsetY;
  UiPixelExtent availableHeight;
  int verticalExtent;
  UiPixelExtent availableWidth;
  int horizontalExtent;
  GraphicsTextureLogicalSize textureSize;
  
  contentChild = (control->base).firstChild;
  availableWidth = (control->base).right - (control->base).left;
  availableHeight = (control->base).bottom - (control->base).top;
  control->scrollStateFlags =
       control->scrollStateFlags &
       ~(UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT|
         UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP);
  (control->base).layoutWidth = availableWidth;
  (control->base).layoutHeight = availableHeight;
  if (contentChild != UI_NODE_NONE) {
    offsetX = control->scrollOffsetY;
    horizontalExtent = (control->base).top;
    contentChild->left = contentChild->leftOffset + control->scrollOffsetX + (control->base).left;
    contentChild->top = contentChild->topOffset + offsetX + horizontalExtent;
    childWidth = contentChild->rightOffset;
    childHeight = contentChild->bottomOffset;
    control->contentWidth = childWidth;
    control->contentHeight = childHeight;
    offsetX = control->scrollOffsetY;
    horizontalExtent = (control->base).top;
    contentChild->right = childWidth + control->scrollOffsetX + (control->base).left;
    contentChild->bottom = childHeight + offsetX + horizontalExtent;
    control->contentOriginX = 0;
    control->contentOriginY = 0;
    if ((control->scrollStateFlags & UI_SCROLL_FRAME_STYLE_A) != 0) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FRAME_A_FIRST,g_UiWindowTextureSource);
      control->contentOriginX = control->contentOriginX + textureSize.logicalWidthPixels;
      control->contentOriginY = control->contentOriginY + textureSize.logicalHeightPixels;
      availableWidth = availableWidth + textureSize.logicalWidthPixels * -2;
      availableHeight = availableHeight + textureSize.logicalHeightPixels * -2;
    }
    if ((control->scrollStateFlags & UI_SCROLL_FRAME_STYLE_B) != 0) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FRAME_B_FIRST,g_UiWindowTextureSource);
      control->contentOriginX = control->contentOriginX + textureSize.logicalWidthPixels;
      control->contentOriginY = control->contentOriginY + textureSize.logicalHeightPixels;
      availableWidth = availableWidth + textureSize.logicalWidthPixels * -2;
      availableHeight = availableHeight + textureSize.logicalHeightPixels * -2;
    }
    control->viewportWidth = availableWidth;
    control->viewportHeight = availableHeight;
    horizontalExtent = availableWidth - control->contentWidth;
    if (horizontalExtent < 0) {
      control->scrollStateFlags =
           control->scrollStateFlags |
           (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP);
    }
    verticalExtent = availableHeight - control->contentHeight;
    if (verticalExtent < 0) {
      control->scrollStateFlags =
           control->scrollStateFlags |
           (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT);
    }
    /* keep only the bar positions allowed by bits 4..7 */
    control->scrollStateFlags =
         control->scrollStateFlags &
         (control->scrollStateFlags >> 4 |
         ~(UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT|
           UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP));
    /* a vertical bar narrows the view: maybe a horizontal bar is needed now, and vice versa. The masks
       ~UI_SCROLL_ALLOWED_* (0xffffffcf/0xffffff3f, as in the original) are practically always nonzero; the
       allowed bits themselves were probably meant. The mask above filters disallowed bars again anyway. */
    if (((control->scrollStateFlags & ~UI_SCROLL_ALLOWED_HORIZONTAL_BARS) != 0) &&
       ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0)) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW,g_UiWindowTextureSource);
      horizontalExtent = horizontalExtent - textureSize.logicalWidthPixels;
      if (horizontalExtent < 0) {
        control->scrollStateFlags =
             control->scrollStateFlags |
             (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP);
      }
    }
    if (((control->scrollStateFlags & ~UI_SCROLL_ALLOWED_VERTICAL_BARS) != 0) &&
       ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0)) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW,
                                                          g_UiWindowTextureSource);
      if ((int)(verticalExtent - textureSize.logicalHeightPixels) < 0) {
        if (((control->scrollStateFlags &
             (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) == 0) &&
           ((control->scrollStateFlags & ~UI_SCROLL_ALLOWED_HORIZONTAL_BARS) != 0)) {
          textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW,
                                                              g_UiWindowTextureSource);
          if ((int)(horizontalExtent - textureSize.logicalWidthPixels) < 0) {
            control->scrollStateFlags =
                 control->scrollStateFlags |
                 (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP);
          }
        }
        control->scrollStateFlags =
             control->scrollStateFlags |
             (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT);
      }
    }
    control->scrollStateFlags =
         control->scrollStateFlags &
         (control->scrollStateFlags >> 4 |
         ~(UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT|
           UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP));
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW,
                                                          g_UiWindowTextureSource);
      control->viewportHeight = control->viewportHeight - textureSize.logicalHeightPixels;
      if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) != 0) {
        control->contentOriginY = control->contentOriginY + textureSize.logicalHeightPixels;
      }
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW,g_UiWindowTextureSource);
      control->viewportWidth = control->viewportWidth - textureSize.logicalWidthPixels;
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) != 0) {
        control->contentOriginX = control->contentOriginX + textureSize.logicalWidthPixels;
      }
    }
    /* clamp the scroll offsets (0 or negative) so the content does not end inside the view */
    offsetX = control->scrollOffsetX;
    offsetY = control->scrollOffsetY;
    contentChild = (control->base).firstChild;
    verticalExtent = (control->contentWidth - control->viewportWidth) + offsetX;
    horizontalExtent = (control->contentHeight - control->viewportHeight) + offsetY;
    if (verticalExtent < 0) {
      control->scrollOffsetX = control->scrollOffsetX - verticalExtent;
      contentChild->left = contentChild->left - verticalExtent;
      contentChild->right = contentChild->right - verticalExtent;
      offsetX = offsetX - verticalExtent;
    }
    if (horizontalExtent < 0) {
      control->scrollOffsetY = control->scrollOffsetY - horizontalExtent;
      contentChild->top = contentChild->top - horizontalExtent;
      contentChild->bottom = contentChild->bottom - horizontalExtent;
      offsetY = offsetY - horizontalExtent;
    }
    if (-1 < (int)offsetX) {
      control->scrollOffsetX = 0;
      contentChild->left = contentChild->left - offsetX;
      contentChild->right = contentChild->right - offsetX;
    }
    if (-1 < (int)offsetY) {
      control->scrollOffsetY = 0;
      contentChild->top = contentChild->top - offsetY;
      contentChild->bottom = contentChild->bottom - offsetY;
    }
    offsetX = control->contentOriginX;
    offsetY = control->contentOriginY;
    contentChild->left = contentChild->left + offsetX;
    contentChild->top = contentChild->top + offsetY;
    contentChild->right = contentChild->right + offsetX;
    contentChild->bottom = contentChild->bottom + offsetY;
    contentChild->vtable->layout(contentChild);
    /* thumbs: length = view / content of the track (at least two caps), position from the offset */
    control->horizontalThumbLeft = 0;
    control->verticalThumbTop = 0;
    control->horizontalThumbRight = 0;
    control->verticalThumbBottom = 0;
    if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) != 0) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW,
                                                          g_UiWindowTextureSource);
      control->verticalThumbTop = control->verticalThumbTop + textureSize.logicalHeightPixels;
      control->verticalThumbBottom = control->verticalThumbBottom + textureSize.logicalHeightPixels;
    }
    if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) != 0) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW,g_UiWindowTextureSource);
      control->horizontalThumbLeft = control->horizontalThumbLeft + textureSize.logicalWidthPixels;
      control->horizontalThumbRight = control->horizontalThumbRight + textureSize.logicalWidthPixels;
    }
    horizontalExtent = (control->base).layoutWidth;
    verticalExtent = (control->base).layoutHeight;
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW,
                                                          g_UiWindowTextureSource);
      arrowSize = textureSize.logicalWidthPixels;
      control->horizontalThumbLeft = control->horizontalThumbLeft + arrowSize;
      control->horizontalThumbRight = control->horizontalThumbRight + arrowSize;
      horizontalExtent = horizontalExtent + arrowSize * -2;
      verticalExtent = verticalExtent - textureSize.logicalHeightPixels;
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW,g_UiWindowTextureSource);
      arrowSize = textureSize.logicalHeightPixels;
      control->verticalThumbTop = control->verticalThumbTop + arrowSize;
      control->verticalThumbBottom = control->verticalThumbBottom + arrowSize;
      verticalExtent = verticalExtent + arrowSize * -2;
      horizontalExtent = horizontalExtent - textureSize.logicalWidthPixels;
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
      thumbLength = (uint32_t)(((int64_t)(int)control->viewportWidth * (int64_t)horizontalExtent) /
                    (int64_t)(int)control->contentWidth);
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_THUMB,
                                                          g_UiWindowTextureSource);
      minThumbLength = textureSize.logicalWidthPixels * 2;
      if (thumbLength < minThumbLength) {
        thumbLength = minThumbLength;
      }
      control->horizontalThumbRight = control->horizontalThumbRight + thumbLength;
      horizontalExtent = (int)(((int64_t)(int)-control->scrollOffsetX * (int64_t)(int)(horizontalExtent - thumbLength)) /
                    (int64_t)(int)(control->contentWidth - control->viewportWidth));
      control->horizontalThumbLeft = control->horizontalThumbLeft + horizontalExtent;
      control->horizontalThumbRight = control->horizontalThumbRight + horizontalExtent;
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      thumbLength = (uint32_t)(((int64_t)(int)control->viewportHeight * (int64_t)verticalExtent) /
                    (int64_t)(int)control->contentHeight);
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_VERTICAL_THUMB,g_UiWindowTextureSource);
      minThumbLength = textureSize.logicalHeightPixels * 2;
      if (thumbLength < minThumbLength) {
        thumbLength = minThumbLength;
      }
      control->verticalThumbBottom = control->verticalThumbBottom + thumbLength;
      horizontalExtent = (int)(((int64_t)(int)-control->scrollOffsetY * (int64_t)(int)(verticalExtent - thumbLength)) /
                    (int64_t)(int)(control->contentHeight - control->viewportHeight));
      control->verticalThumbTop = control->verticalThumbTop + horizontalExtent;
      control->verticalThumbBottom = control->verticalThumbBottom + horizontalExtent;
    }
  }
}

/* Cursor of a scroll frame (g_UiScrollableControlVtable pointerMove): while a right-button pan that started
   inside the content is active, the pan cursor for the axes that can scroll; otherwise the arrow.
*/
GraphicsCursorFrameIndex UiScrollableControl_QueryPointerRegion
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiScrollableControl *control)

{
  GraphicsCursorFrameIndex cursorFrame;
  
  cursorFrame = GRAPHICS_CURSOR_FRAME_ARROW;
  if (((control->scrollStateFlags & UI_SCROLL_SECONDARY_PANNING_CONTENT) != 0) &&
     ((control->scrollStateFlags &
      (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT|
       UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0)) {
    cursorFrame = UI_SCROLL_CURSOR_FRAME_PAN;
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) == 0) {
      cursorFrame = UI_SCROLL_CURSOR_FRAME_PAN_VERTICAL;
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) == 0) {
      cursorFrame = UI_SCROLL_CURSOR_FRAME_PAN_HORIZONTAL;
    }
  }
  return cursorFrame;
}

/* Right-button press on a scroll frame (g_UiScrollableControlVtable rightPress): starts panning. Pins the
   cursor (g_CursorUseOverridePosition), remembers the press position as the pan anchor and, when the press is
   inside the content view and the frame has a bar, shows the pan cursor for the axes that can scroll.
*/
void UiScrollableControl_BeginSecondaryScrollInteraction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiScrollableControl *control)

{
  int localPointerX;
  int localPointerY;
  uint32_t cursorFrame;
  
  g_CursorUseOverridePosition++;
  control->scrollStateFlags = control->scrollStateFlags | UI_SCROLL_SECONDARY_INTERACTION_ACTIVE;
  control->pointerAnchorX = pointerX;
  control->pointerAnchorY = pointerY;
  localPointerX = pointerX - (control->base).left;
  localPointerY = pointerY - (control->base).top;
  control->scrollStateFlags = control->scrollStateFlags &
       ~(UI_SCROLL_VERTICAL_PARTS_ACTIVE|UI_SCROLL_HORIZONTAL_PARTS_ACTIVE|UI_SCROLL_PRIMARY_INTERACTION_ACTIVE|
         UI_SCROLL_SECONDARY_INTERACTION_ACTIVE); /* clears the bit set above too, as in the original */
  /* Press outside the content view: no panning of the content. */
  if (localPointerX < (int)control->contentOriginX) {
    return;
  }
  if (localPointerY < (int)control->contentOriginY) {
    return;
  }
  if ((int)control->contentOriginX <= (int)(localPointerX - control->viewportWidth)) {
    return;
  }
  if ((int)control->contentOriginY <= (int)(localPointerY - control->viewportHeight)) {
    return;
  }
  control->scrollStateFlags = control->scrollStateFlags | UI_SCROLL_SECONDARY_PANNING_CONTENT;
  if ((control->scrollStateFlags &
      (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT|
       UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) == 0) {
    return;
  }
  cursorFrame = UI_SCROLL_CURSOR_FRAME_PAN;
  if ((control->scrollStateFlags &
      (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) == 0) {
    cursorFrame = UI_SCROLL_CURSOR_FRAME_PAN_VERTICAL;
  }
  if ((control->scrollStateFlags &
      (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) == 0) {
    cursorFrame = UI_SCROLL_CURSOR_FRAME_PAN_HORIZONTAL;
  }
  g_GraphicsCursorSetFrame(cursorFrame);
}

/* Right-button release on a scroll frame (g_UiScrollableControlVtable rightRelease): ends panning, releases
   the pinned cursor and restores the arrow cursor.
*/
void UiScrollableControl_EndSecondaryScrollInteraction
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
               UiScrollableControl *control)

{
  g_CursorUseOverridePosition = 0;
  control->scrollStateFlags = control->scrollStateFlags &
       ~(UI_SCROLL_SECONDARY_PANNING_CONTENT|UI_SCROLL_SECONDARY_INTERACTION_ACTIVE);
  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_ARROW);
}

/* Hit test of a scroll frame (g_UiScrollableControlVtable hitTest): a pointer inside the content view hits
   the content's children, anywhere else (bars, frame) or during a right-button pan of the content the frame
   itself.
*/
UiNodeBase * UiScrollableControl_HitTestContentAndScrollbars
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiScrollableControl *control)

{
  int localPointerX;
  int localPointerY;
  
  localPointerX = pointerX - (control->base).left;
  localPointerY = pointerY - (control->base).top;
  if (((((control->scrollStateFlags & UI_SCROLL_SECONDARY_PANNING_CONTENT) == 0) &&
       ((int)control->contentOriginX <= localPointerX)) &&
      ((int)control->contentOriginY <= localPointerY)) &&
     (((int)(localPointerX - control->viewportWidth) < (int)control->contentOriginX &&
      ((int)(localPointerY - control->viewportHeight) < (int)control->contentOriginY)))) {
    control = UiNode_As<UiScrollableControl>(UiContainer_HitTestChildren(pointerY,pointerX,&control->base));
  }
  return &control->base;
}

/* Returns the size of the view of a scroll frame, or 0/0 when control is not a scroll frame. The lists use it
   on their parent to page by a view's height.
*/
UiScrollableViewportSize UiScrollableControl_GetViewportSize(UiScrollableControl *control)

{
  UiScrollableViewportSize viewportSize;

  viewportSize.width = 0;
  viewportSize.height = 0;
  if ((control->base).vtable == &g_UiScrollableControlVtable) {
    viewportSize.width = control->viewportWidth;
    viewportSize.height = control->viewportHeight;
  }
  return viewportSize;
}

/* Places the scrolled content (the first child) at the current scroll offsets, clamps the offsets so the
   content neither ends inside the viewport nor starts after its origin, lays the content out and
   recomputes the thumb rectangles of the enabled scrollbars (thumb length proportional to the visible
   part, at least two thumb pieces). Scrolled content has offsets <= 0.
*/
void UiScrollableControl_RefreshChildAndScrollThumbs(UiScrollableControl *control)

{
  UiNodeBase *contentChild;
  UiPixelExtent childWidth;
  UiPixelExtent childHeight;
  uint32_t minThumbLength;
  uint32_t arrowSize;
  uint32_t thumbLength;
  UiPixelOffset offsetX;
  UiPixelOffset offsetY;
  /* both are reused as temporaries: first the control's top and the overflow past the content end, later
     the horizontal/vertical track lengths and the thumb positions */
  int horizontalExtent;
  int verticalExtent;
  GraphicsTextureLogicalSize textureSize;

  contentChild = (control->base).firstChild;
  if (contentChild != UI_NODE_NONE) {
    /* offsetX temporarily holds the vertical offset here */
    offsetX = control->scrollOffsetY;
    horizontalExtent = (control->base).top;
    contentChild->left = contentChild->leftOffset + control->scrollOffsetX + (control->base).left;
    contentChild->top = contentChild->topOffset + offsetX + horizontalExtent;
    childWidth = contentChild->rightOffset;
    childHeight = contentChild->bottomOffset;
    control->contentWidth = childWidth;
    control->contentHeight = childHeight;
    offsetX = control->scrollOffsetY;
    horizontalExtent = (control->base).top;
    contentChild->right = childWidth + control->scrollOffsetX + (control->base).left;
    contentChild->bottom = childHeight + offsetX + horizontalExtent;
    offsetX = control->scrollOffsetX;
    offsetY = control->scrollOffsetY;
    contentChild = (control->base).firstChild;
    verticalExtent = (control->contentWidth - control->viewportWidth) + offsetX;
    horizontalExtent = (control->contentHeight - control->viewportHeight) + offsetY;
    if (verticalExtent < 0) {
      control->scrollOffsetX = control->scrollOffsetX - verticalExtent;
      contentChild->left = contentChild->left - verticalExtent;
      contentChild->right = contentChild->right - verticalExtent;
      offsetX = offsetX - verticalExtent;
    }
    if (horizontalExtent < 0) {
      control->scrollOffsetY = control->scrollOffsetY - horizontalExtent;
      contentChild->top = contentChild->top - horizontalExtent;
      contentChild->bottom = contentChild->bottom - horizontalExtent;
      offsetY = offsetY - horizontalExtent;
    }
    if (-1 < (int)offsetX) {
      control->scrollOffsetX = 0;
      contentChild->left = contentChild->left - offsetX;
      contentChild->right = contentChild->right - offsetX;
    }
    if (-1 < (int)offsetY) {
      control->scrollOffsetY = 0;
      contentChild->top = contentChild->top - offsetY;
      contentChild->bottom = contentChild->bottom - offsetY;
    }
    offsetX = control->contentOriginX;
    offsetY = control->contentOriginY;
    contentChild->left = contentChild->left + offsetX;
    contentChild->top = contentChild->top + offsetY;
    contentChild->right = contentChild->right + offsetX;
    contentChild->bottom = contentChild->bottom + offsetY;
    contentChild->vtable->layout(contentChild);
    control->horizontalThumbLeft = 0;
    control->verticalThumbTop = 0;
    control->horizontalThumbRight = 0;
    control->verticalThumbBottom = 0;
    /* the thumb rectangles start at the bar positions: a bar at the top/left shifts the other bar's thumb */
    if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) != 0) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW,
                                                          g_UiWindowTextureSource);
      control->verticalThumbTop = control->verticalThumbTop + textureSize.logicalHeightPixels;
      control->verticalThumbBottom = control->verticalThumbBottom + textureSize.logicalHeightPixels;
    }
    if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) != 0) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW,g_UiWindowTextureSource);
      control->horizontalThumbLeft = control->horizontalThumbLeft + textureSize.logicalWidthPixels;
      control->horizontalThumbRight = control->horizontalThumbRight + textureSize.logicalWidthPixels;
    }
    /* track lengths: the control size minus both arrows and the other bar's thickness */
    horizontalExtent = (control->base).layoutWidth;
    verticalExtent = (control->base).layoutHeight;
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW,
                                                          g_UiWindowTextureSource);
      arrowSize = textureSize.logicalWidthPixels;
      control->horizontalThumbLeft = control->horizontalThumbLeft + arrowSize;
      control->horizontalThumbRight = control->horizontalThumbRight + arrowSize;
      horizontalExtent = horizontalExtent + arrowSize * -2;
      verticalExtent = verticalExtent - textureSize.logicalHeightPixels;
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW,g_UiWindowTextureSource);
      arrowSize = textureSize.logicalHeightPixels;
      control->verticalThumbTop = control->verticalThumbTop + arrowSize;
      control->verticalThumbBottom = control->verticalThumbBottom + arrowSize;
      verticalExtent = verticalExtent + arrowSize * -2;
      horizontalExtent = horizontalExtent - textureSize.logicalWidthPixels;
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
      thumbLength = (uint32_t)(((int64_t)(int)control->viewportWidth * (int64_t)horizontalExtent) /
                    (int64_t)(int)control->contentWidth);
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_THUMB,
                                                          g_UiWindowTextureSource);
      minThumbLength = textureSize.logicalWidthPixels * 2;
      if (thumbLength < minThumbLength) {
        thumbLength = minThumbLength;
      }
      control->horizontalThumbRight = control->horizontalThumbRight + thumbLength;
      /* thumb position = scrolled share of the free track */
      horizontalExtent = (int)(((int64_t)(int)-control->scrollOffsetX * (int64_t)(int)(horizontalExtent - thumbLength)) /
                   (int64_t)(int)(control->contentWidth - control->viewportWidth));
      control->horizontalThumbLeft = control->horizontalThumbLeft + horizontalExtent;
      control->horizontalThumbRight = control->horizontalThumbRight + horizontalExtent;
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      thumbLength = (uint32_t)(((int64_t)(int)control->viewportHeight * (int64_t)verticalExtent) /
                    (int64_t)(int)control->contentHeight);
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_VERTICAL_THUMB,g_UiWindowTextureSource);
      minThumbLength = textureSize.logicalHeightPixels * 2;
      if (thumbLength < minThumbLength) {
        thumbLength = minThumbLength;
      }
      control->verticalThumbBottom = control->verticalThumbBottom + thumbLength;
      horizontalExtent = (int)(((int64_t)(int)-control->scrollOffsetY * (int64_t)(int)(verticalExtent - thumbLength)) /
                   (int64_t)(int)(control->contentHeight - control->viewportHeight));
      control->verticalThumbTop = control->verticalThumbTop + horizontalExtent;
      control->verticalThumbBottom = control->verticalThumbBottom + horizontalExtent;
    }
  }
}

/* Scrolls a scrollable control just far enough that the target rectangle (content coordinates, e.g. a
   selected list row) is visible, on the axes that have a scroll bar: first so its right/bottom edge is
   inside the view, then so its left/top edge is (that one wins when the target is larger than the view).
   Relayouts when an offset changed and redraws. Does nothing unless control really is a
   g_UiScrollableControlVtable node (callers pass their parent without checking).
*/
void UiScrollableControl_ClampOffsetsToViewport
          (UiPixelCoordinate targetBottom,UiPixelCoordinate targetRight,UiPixelCoordinate targetTop,
          UiPixelCoordinate targetLeft,UiScrollableControl *control)

{
  char changeCount;
  int viewLeft;
  int viewTop;
  int viewBottom;
  int viewRight;
  int horizontalOverflow;
  int verticalOverflow;

  if ((control->base).vtable == &g_UiScrollableControlVtable) {
    /* the visible content rectangle; the scroll offsets are the negated view position */
    viewLeft = -control->scrollOffsetX;
    viewTop = -control->scrollOffsetY;
    changeCount = 0;
    viewRight = control->viewportWidth + viewLeft;
    viewBottom = control->viewportHeight + viewTop;
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
      horizontalOverflow = viewRight - targetRight;
      if (viewRight < targetRight) {
        changeCount++;
        control->scrollOffsetX = control->scrollOffsetX + horizontalOverflow;
        viewLeft = viewLeft - horizontalOverflow;
      }
      if (viewLeft - targetLeft != 0 && targetLeft <= viewLeft) {
        changeCount++;
        control->scrollOffsetX = control->scrollOffsetX + (viewLeft - targetLeft);
      }
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      verticalOverflow = viewBottom - targetBottom;
      if (viewBottom < targetBottom) {
        control->scrollOffsetY = control->scrollOffsetY + verticalOverflow;
        viewTop = viewTop - verticalOverflow;
        changeCount++;
      }
      if (viewTop - targetTop != 0 && targetTop <= viewTop) {
        changeCount++;
        control->scrollOffsetY = control->scrollOffsetY + (viewTop - targetTop);
      }
    }
    if (changeCount != 0) {
      UiScrollableControl_RefreshChildAndScrollThumbs(control);
    }
    UiNode_InvalidateRoot(&control->base);
  }
}

UiNodeVtable g_UiScrollableControlVtable = {
    .relocate = UI_SLOT(UiScrollableControl_RelocateChildren),
    .method04 = UI_SLOT(UiNode_DefaultMethod04_NoOp),
    .drawClipped = UI_SLOT(UiScrollableControl_DrawFrameContentAndScrollbars),
    .layout = UI_SLOT(UiScrollableControl_RebuildViewportAndScrollbars),
    .nonRightPress = UI_SLOT(UiScrollableControl_BeginPrimaryScrollInteraction),
    .nonRightRelease = UI_SLOT(UiScrollableControl_EndPrimaryScrollInteraction),
    .rightPress = UI_SLOT(UiScrollableControl_BeginSecondaryScrollInteraction),
    .rightRelease = UI_SLOT(UiScrollableControl_EndSecondaryScrollInteraction),
    .nonRightDrag = UI_SLOT(UiScrollableControl_UpdatePrimaryScrollDrag),
    .rightDrag = UI_SLOT(UiScrollableControl_UpdateSecondaryScrollDrag),
    .pointerMove = UI_SLOT(UiScrollableControl_QueryPointerRegion),
    .hitTest = UI_SLOT(UiScrollableControl_HitTestContentAndScrollbars),
    .keyboardEvent = UI_SLOT(UiNode_DefaultKeyboardEventMoveFocusNext),
    .applyFlags = UI_SLOT(UiNode_ApplyFlagsRecursive),
    .suppressActionId = UI_SLOT(UiContainer_SuppressActionId),
    .unsuppressActionId = UI_SLOT(UiContainer_UnsuppressActionId),
    .tick = UI_SLOT(UiScrollableControl_TickAutoScroll),
    .pointerWheel = UI_SLOT(UiScrollableControl_HandlePointerWheel)};

UiNodeVtable g_UiListControlVtable = {
    .relocate = UI_SLOT(UiContainer_RelocateChildren),
    .method04 = UI_SLOT(UiNode_DefaultMethod04_NoOp),
    .drawClipped = UI_SLOT(UiListControl_DrawRowsAndSelection),
    .layout = UI_SLOT(UiContainer_LayoutChildren),
    .nonRightPress = UI_SLOT(UiListControl_SelectRowFromPointer),
    .nonRightRelease = UI_SLOT(UiNode_DefaultNonRightRelease),
    .rightPress = UI_SLOT(UiNode_ForwardRightPressToParent),
    .rightRelease = UI_SLOT(UiNode_DefaultRightRelease),
    .nonRightDrag = UI_SLOT(UiNode_DefaultNonRightDrag),
    .rightDrag = UI_SLOT(UiNode_DefaultRightDrag),
    .pointerMove = UI_SLOT(UiNode_DefaultPointerMove),
    .hitTest = UI_SLOT(UiContainer_HitTestChildren),
    .keyboardEvent = UI_SLOT(UiListControl_HandleKeyboardNavigation),
    .applyFlags = UI_SLOT(UiNode_ApplyFlagsRecursive),
    .suppressActionId = UI_SLOT(UiListControl_SuppressIfActionId),
    .unsuppressActionId = UI_SLOT(UiListControl_UnsuppressIfActionId),
    .tick = UI_SLOT(UiListControl_TickActivationPulse),
    .pointerWheel = UI_SLOT(UiNode_ForwardPointerWheelToParent)};

UiNodeVtable g_UiTextListControlVtable = {
    .relocate = UI_SLOT(UiContainer_RelocateChildren),
    .method04 = UI_SLOT(UiNode_DefaultMethod04_NoOp),
    .drawClipped = UI_SLOT(UiTextListControl_DrawRowsAndSelection),
    .layout = UI_SLOT(UiContainer_LayoutChildren),
    .nonRightPress = UI_SLOT(UiTextListControl_SelectRowFromPointer),
    .nonRightRelease = UI_SLOT(UiNode_DefaultNonRightRelease),
    .rightPress = UI_SLOT(UiNode_ForwardRightPressToParent),
    .rightRelease = UI_SLOT(UiNode_DefaultRightRelease),
    .nonRightDrag = UI_SLOT(UiNode_DefaultNonRightDrag),
    .rightDrag = UI_SLOT(UiNode_DefaultRightDrag),
    .pointerMove = UI_SLOT(UiNode_DefaultPointerMove),
    .hitTest = UI_SLOT(UiContainer_HitTestChildren),
    .keyboardEvent = UI_SLOT(UiTextListControl_HandleKeyboardNavigationAndSearch),
    .applyFlags = UI_SLOT(UiNode_ApplyFlagsRecursive),
    .suppressActionId = UI_SLOT(UiListControl_SuppressIfActionId),
    .unsuppressActionId = UI_SLOT(UiListControl_UnsuppressIfActionId),
    .tick = UI_SLOT(UiTextListControl_TickActivationPulse),
    .pointerWheel = UI_SLOT(UiNode_ForwardPointerWheelToParent)};
