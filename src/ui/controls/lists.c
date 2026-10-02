/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/lists.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/lists.h>
#include <thandor/thandor.h>

/* Implementation ownership: ui/controls/lists. */

/* Address: 0x004BBE60.
   Keyboard handler of the tree list (g_UiTimedListControlVtable keyboardEvent): Home/End, Up/Down and
   Page Up/Down walk the visible rows of the record tree, Left/Right collapse/expand the selected row through
   recordSelectionCallback. A moved selection is scrolled into view and its action queued after
   g_UiTimedListActionDelayFrames frames (UiTimedListControl_TickActionDelay). Other keys go to the default
   focus handling; the keys handled here return false (CF clear).
*/
bool UiTimedListControl_HandleKeyboardNavigation
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiTimedListControl *control)

{
  UiTimedListTreeRecord *startRecord;
  UiTimedListTreeRecord *scanRecord;
  UiFrameDelayFrames actionDelayFrames;
  UiTimedListTreeRecord *childBlock;
  UiTimedListTreeRecord *recordCursor;
  UiTimedListTreeRecord *stepRecord;
  UiTimedListTreeRecord *previousSelection;
  UiTimedListTreeRecord *countRecord;
  uint32_t nestedRecordCount;
  UiTimedListTreeRecord *lastRecord;
  uint32_t siblingIndex;
  int visibleRows;
  int rowAccumulator;
  int rowIndex;
  int stepCounter;
  bool handled;
  UiScrollableViewportSize viewportSize;

  /* A record block is a header record (count, parent block, parent record, ANCESTOR_BOUNDARY flag)
     followed by count row records; an expanded row (flags 1|2) links its child block. */
  stepCounter = 1;
  previousSelection = control->selectedRecord;
  if ((keyCode & KEYBOARD_KEY_CODE_FAMILY_MASK) == 0) { /* plain characters */
    handled = UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
    return handled;
  }
  switch (keyCode) {
  case KEYBOARD_KEY_CODE_HOME:
    control->selectedRecord = control->recordTree + 1;
    break;
  case KEYBOARD_KEY_CODE_END:
    childBlock = control->recordTree;
    do {
      lastRecord = childBlock + (int)childBlock->countOrLabelText;
      control->selectedRecord = lastRecord;
      if (((lastRecord->flags & UI_TIMED_LIST_RECORD_EXPANDABLE) == 0) ||
          ((lastRecord->flags & UI_TIMED_LIST_RECORD_EXPANDED) == 0)) break;
      childBlock = lastRecord->childBlockOrParentRecord;
    } while (childBlock != NULL);
    break;
  case KEYBOARD_KEY_CODE_PAGE_UP:
  case KEYBOARD_KEY_CODE_UP:
    if (keyCode == KEYBOARD_KEY_CODE_PAGE_UP) {
      /* Page: one step less than the rows visible in the viewport (unsigned 32-bit DIV).
         NOTE: exactly one visible row gives 0 steps, which the do/while wraps like the original. */
      viewportSize = UiScrollableControl_GetViewportSize((UiScrollableControl *)(control->base).parent);
      visibleRows = (int)(viewportSize.height / control->rowHeight);
      stepCounter = visibleRows - 1;
      if (visibleRows < 1) {
        stepCounter = 1;
      }
    }
    /* Step backward: the previous row, descending into the last row of expanded blocks, or up to
       the parent row at a block start. */
    do {
      stepRecord = control->selectedRecord;
      recordCursor = stepRecord - 1;
      control->selectedRecord = recordCursor;
      if ((stepRecord[-1].flags & UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY) == 0) {
        while ((recordCursor->flags & UI_TIMED_LIST_RECORD_EXPANDABLE) != 0 &&
               (recordCursor->flags & UI_TIMED_LIST_RECORD_EXPANDED) != 0 &&
               recordCursor->childBlockOrParentRecord != NULL) {
          childBlock = recordCursor->childBlockOrParentRecord;
          recordCursor = childBlock + (int)childBlock->countOrLabelText;
          control->selectedRecord = recordCursor;
        }
      }
      else {
        recordCursor = stepRecord[-1].childBlockOrParentRecord;
        control->selectedRecord = stepRecord;
        if (recordCursor != NULL) {
          control->selectedRecord = recordCursor;
        }
      }
      stepCounter--;
    } while (stepCounter != 0);
    break;
  case KEYBOARD_KEY_CODE_PAGE_DOWN:
  case KEYBOARD_KEY_CODE_DOWN:
    if (keyCode == KEYBOARD_KEY_CODE_PAGE_DOWN) {
      viewportSize = UiScrollableControl_GetViewportSize((UiScrollableControl *)(control->base).parent);
      visibleRows = (int)(viewportSize.height / control->rowHeight);
      stepCounter = visibleRows - 1;
      if (visibleRows < 1) {
        stepCounter = 1;
      }
    }
    /* Step forward: into the first row of an expanded non-empty block, else the next sibling of
       the row or of its nearest ancestor that has one (stay put at the end). */
    do {
      startRecord = control->selectedRecord;
      siblingIndex = 0;
      scanRecord = startRecord;
      childBlock = NULL;
      if ((startRecord->flags & UI_TIMED_LIST_RECORD_EXPANDABLE) != 0 &&
          (startRecord->flags & UI_TIMED_LIST_RECORD_EXPANDED) != 0) {
        childBlock = startRecord->childBlockOrParentRecord;
      }
      if (childBlock != NULL && childBlock->countOrLabelText != 0) {
        control->selectedRecord = childBlock + 1;
      }
      else {
        do {
          do {
            stepRecord = scanRecord;
            siblingIndex++;
            scanRecord = stepRecord - 1;
          } while ((stepRecord[-1].flags & UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY) == 0);
          control->selectedRecord = control->selectedRecord + 1;
          if (siblingIndex < stepRecord[-1].countOrLabelText) break;
          scanRecord = stepRecord[-1].childBlockOrParentRecord;
          siblingIndex = 0;
          control->selectedRecord = scanRecord;
          if (scanRecord == NULL) {
            control->selectedRecord = startRecord;
          }
        } while (scanRecord != NULL);
      }
      stepCounter--;
    } while (stepCounter != 0);
    break;
  case KEYBOARD_KEY_CODE_LEFT: /* collapse an expanded row */
    if ((previousSelection->flags & UI_TIMED_LIST_RECORD_EXPANDABLE) == 0) {
      return false;
    }
    if ((previousSelection->flags & UI_TIMED_LIST_RECORD_EXPANDED) == 0) {
      return false;
    }
    if (control->recordSelectionCallback == 0) {
      return false;
    }
    control->recordSelectionCallback
              (previousSelection,(UiTimedListTreeControl *)control);
    return false;
  case KEYBOARD_KEY_CODE_RIGHT: /* expand a collapsed directory row */
    if ((previousSelection->flags & UI_TIMED_LIST_RECORD_EXPANDABLE) == 0) {
      return false;
    }
    if ((previousSelection->flags & UI_TIMED_LIST_RECORD_EXPANDED) != 0) {
      return false;
    }
    if (control->recordSelectionCallback == 0) {
      return false;
    }
    control->recordSelectionCallback
              (previousSelection,(UiTimedListTreeControl *)control);
    return false;
  default:
    handled = UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
    return handled;
  }
  countRecord = control->selectedRecord;
  rowAccumulator = 0;
  if (countRecord != previousSelection) {
    /* Row index of the new selection: walk back over the rows above it (adding the rows of expanded
       blocks) and up through the parent rows until the root header. */
    do {
      rowIndex = rowAccumulator;
      if ((countRecord[-1].flags & UI_TIMED_LIST_RECORD_EXPANDED) != 0) {
        nestedRecordCount = UiTimedListTree_CountRecordArrayAndNestedChildren
                          (countRecord[-1].childBlockOrParentRecord);
        rowIndex = rowAccumulator + nestedRecordCount;
      }
      countRecord = countRecord - 1;
      rowAccumulator = rowIndex + 1;
      if ((countRecord->flags & UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY) != 0) {
        countRecord = countRecord->childBlockOrParentRecord;
      }
    } while (countRecord != NULL);
    rowIndex = rowIndex * (int)control->rowHeight;
    UiScrollableControl_ClampOffsetsToViewport
              ((int)control->rowHeight + rowIndex + 1,(control->base).rightOffset,rowIndex,0,
               (UiScrollableControl *)(control->base).parent);
    actionDelayFrames = g_UiTimedListActionDelayFrames;
    control->listStateAndDelay = control->listStateAndDelay | UI_TIMED_LIST_ACTION_DELAY_PENDING;
    control->listStateAndDelay = control->listStateAndDelay & UI_LIST_FLAGS_MASK;
    control->listStateAndDelay = control->listStateAndDelay | actionDelayFrames << UI_LIST_COUNTDOWN_SHIFT;
  }
  return false;
}


/* Address: 0x004BB100.
   Keyboard handler of the column list (g_UiListControlVtable keyboardEvent): Enter confirms the selection
   and queues the list's action at once; Home/End, Up/Down and Page Up/Down (one row less than the view
   holds) move the selection, play the selection sound, scroll the row into view and queue the action after
   g_UiListActivationPulseFrames frames (UiListControl_TickActivationPulse). Other keys go to the default
   focus handling; the keys handled here return false (CF clear).
*/
bool UiListControl_HandleKeyboardNavigation
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiListControl *control)

{
  void **previousSelectedSlot;
  void **newSelectedSlot;
  int rowValue;
  uint32_t targetRowIndex;
  bool handled;
  UiScrollableViewportSize viewportSize;
  
  previousSelectedSlot = control->selectedRowSlot;
  if ((keyCode & KEYBOARD_KEY_CODE_FAMILY_MASK) != 0) { /* not a plain character */
    if (keyCode == KEYBOARD_KEY_CODE_ENTER) {
      control->listStateFlags = control->listStateFlags | UI_LIST_SELECTION_CONFIRMED;
      UiActionQueue_Enqueue(control->actionId,control);
    }
    else if (keyCode == KEYBOARD_KEY_CODE_HOME) {
      control->selectedRowSlot = control->rowSlots;
    }
    else if (keyCode == KEYBOARD_KEY_CODE_END) {
      control->selectedRowSlot = control->rowSlots + (control->rowCount - 1);
    }
    else if (keyCode == KEYBOARD_KEY_CODE_PAGE_UP) {
      viewportSize = UiScrollableControl_GetViewportSize((UiScrollableControl *)(control->base).parent);
      rowValue = ((uint32_t)((int)control->selectedRowSlot - (int)control->rowSlots) >> 2) -
              ((int)(viewportSize.height / control->rowHeight) - 1);
      if (rowValue < 0) {
        rowValue = 0;
      }
      control->selectedRowSlot = control->rowSlots + rowValue;
    }
    else if (keyCode == KEYBOARD_KEY_CODE_PAGE_DOWN) {
      viewportSize = UiScrollableControl_GetViewportSize((UiScrollableControl *)(control->base).parent);
      targetRowIndex = ((uint32_t)((int)control->selectedRowSlot - (int)control->rowSlots) >> 2) +
              (int)(viewportSize.height / control->rowHeight) - 1;
      if (control->rowCount <= targetRowIndex) {
        targetRowIndex = control->rowCount - 1;
      }
      control->selectedRowSlot = control->rowSlots + targetRowIndex;
    }
    else if (keyCode == KEYBOARD_KEY_CODE_UP) {
      if (control->rowSlots <= control->selectedRowSlot - 1) {
        control->selectedRowSlot = control->selectedRowSlot - 1;
      }
    }
    else if (keyCode == KEYBOARD_KEY_CODE_DOWN) {
      if (((uint32_t)((int)control->selectedRowSlot - (int)control->rowSlots) >> 2) + 1 <
          control->rowCount) {
        control->selectedRowSlot = control->selectedRowSlot + 1;
      }
    }
    else {
      handled = UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
      return handled;
    }
    newSelectedSlot = control->selectedRowSlot;
    if (newSelectedSlot != previousSelectedSlot) {
      if (((control->listStateFlags & UI_LIST_PLAY_SELECTION_SOUND) != 0) &&
         (control->activationSound != NULL)) {
        g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound,NULL);
      }
      rowValue = ((uint32_t)((int)newSelectedSlot - (int)control->rowSlots) >> 2) * control->rowHeight;
      UiScrollableControl_ClampOffsetsToViewport
                (rowValue + control->rowHeight + 1,(control->base).rightOffset,rowValue,0,
                 (UiScrollableControl *)(control->base).parent);
      rowValue = g_UiListActivationPulseFrames;
      control->listStateFlags = control->listStateFlags | UI_LIST_DEFERRED_ACTION_PENDING;
      control->listStateFlags = control->listStateFlags & UI_LIST_FLAGS_MASK;
      control->listStateFlags = control->listStateFlags | rowValue << UI_LIST_COUNTDOWN_SHIFT;
    }
    return false;
  }
  handled = UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
  return handled;
}


/* Address: 0x004BB480.
   After the rows of a pointer list changed: sets the content height to rowCount rows (+1 pixel), lets the
   parent re-layout (scroll range), re-applies the current selection so it stays visible and queues the
   list's action so its owner refreshes.
*/
void UiPointerList_RefreshSelectionAndQueueAction(UiPointerListControl *control)

{
  UiNodeBase *parentNode;
  UiListRowIndex selectedIndex;
  UiNodeVtable *parentVtable;

  parentNode = control->base.parent;
  parentVtable = parentNode->vtable;
  control->base.bottomOffset = control->rowHeight * control->rowCount + 1;
  parentVtable->layout(parentNode);
  selectedIndex = UiPointerList_GetSelectedIndexAndConfirmed(control,NULL);
  UiPointerList_SelectColumnListIndex(selectedIndex,control);
  UiActionQueue_Enqueue(control->actionId,control);
  return;
}


/* Address: 0x004B87A0.
   Left-button press on a scroll frame (g_UiScrollableControlVtable nonRightPress): finds the scrollbar part
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
  bool horizontalBarHit;
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
  return;
}


/* Address: 0x004B8A20.
   Left-button release on a scroll frame (g_UiScrollableControlVtable nonRightRelease): a held track pages by
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
  return;
}


/* Address: 0x004B8BF0.
   Left-button drag on a scroll frame (g_UiScrollableControlVtable nonRightDrag): a grabbed thumb follows the
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
  bool inArrowBar;
  bool arrowHovered;
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
  return;
}


/* Address: 0x004B8F90.
   Right-button drag on a scroll frame (g_UiScrollableControlVtable rightDrag): pans the content by the
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
  return;
}


/* Address: 0x004B9070.
   Per-frame tick of a scroll frame (g_UiScrollableControlVtable tick): while an arrow is held under the
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
  return;
}


/* Address: 0x004B90E0.
   Mouse wheel over a scroll frame (g_UiScrollableControlVtable pointerWheel): scrolls vertically by
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
     ((firstChildNode->vtable == &g_UiTimedListControlVtable) ||
      (firstChildNode->vtable == &g_UiTextListControlVtable) ||
      (firstChildNode->vtable == &g_UiListControlVtable))) {
    scrollStep = g_UiScrollWheelListStep;
  }
  control->scrollOffsetY = control->scrollOffsetY + wheelDelta * scrollStep;
  UiScrollableControl_RefreshChildAndScrollThumbs(control);
  UiNode_InvalidateRoot(&control->base);
  return;
}


/* Address: 0x004BA500.
   Selects row index of a pointer list (without queueing its action) and scrolls the list's scrollable
   parent so the row is visible. Out-of-range indices are ignored.
*/
void UiPointerList_SelectTextListIndex(UiListRowIndex index,UiPointerListControl *control)

{
  int rowTop;
  
  if (index < control->rowCount) {
    control->selectedRowSlot = control->rowSlots + index;
    rowTop = control->rowHeight * index;
    UiScrollableControl_ClampOffsetsToViewport
              (rowTop + 1 + control->rowHeight,(control->base).rightOffset,rowTop,0,
               (UiScrollableControl *)(control->base).parent);
  }
  return;
}


/* Address: 0x004BB020.
   Left-button press on the column list (g_UiListControlVtable nonRightPress): selects the row under the
   pointer, scrolls it into view, queues the list's action and plays the selection sound. A double click
   also marks the selection confirmed (UI_LIST_SELECTION_CONFIRMED) and re-queues even for the same row; a
   single click on the already selected row does nothing.
*/
void UiListControl_SelectRowFromPointer
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiListControl *control)

{
  uint32_t rowIndex;
  int rowTop;
  int32_t *topEdgeField;
  
  topEdgeField = &(control->base).top;
  if ((((*topEdgeField <= pointerY) && (pointerY - *topEdgeField < (control->base).layoutHeight)) &&
      ((control->base).left <= pointerX)) && (pointerX < (control->base).right)) {
    rowIndex = (uint32_t)(pointerY - *topEdgeField) / control->rowHeight;
    if (rowIndex < control->rowCount) {
      control->listStateFlags = control->listStateFlags | UI_LIST_SELECTION_CONFIRMED;
      if (((control->base).nodeFlags & UI_NODE_REPEAT_OR_DOUBLE_CLICK) == 0) {
        /* Single click: not confirmed, and the already selected row does nothing. */
        control->listStateFlags = control->listStateFlags & ~UI_LIST_SELECTION_CONFIRMED;
        if (control->rowSlots + rowIndex == control->selectedRowSlot) {
          return;
        }
      }
      control->selectedRowSlot = control->rowSlots + rowIndex;
      rowTop = rowIndex * control->rowHeight;
      UiScrollableControl_ClampOffsetsToViewport
                (rowTop + 1 + control->rowHeight,(control->base).rightOffset,rowTop,0,
                 (UiScrollableControl *)(control->base).parent);
      UiActionQueue_Enqueue(control->actionId,control);
      if (((control->listStateFlags & UI_LIST_PLAY_SELECTION_SOUND) != 0) &&
         (control->activationSound != NULL)) {
        g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound,NULL);
      }
    }
  }
  return;
}


/* Address: 0x004BB7A0.
   Sorts the rows of a pointer list by a 64-bit key at fieldOffset in each row entry (high dword first, then
   low dword; unsigned, descending) with an exchange sort, then selects the previously selected entry again
   and scrolls it into view. Equal keys are swapped too, so the sort is not stable.
*/
void UiPointerList_SortByDwordPairFieldDescending
          (UiPointerListFieldByteOffset fieldOffset,UiPointerListControl *control)

{
  void *swapEntry;
  void *selectedEntry;
  const uint32_t *pivotKey;
  const uint32_t *scanKey;
  int passLength;
  int compareCount;
  int selectedRowTop;
  UiListRowCount rowsRemaining;
  void **pivotSlot;
  void **scanSlot;

  if (control->rowSlots == NULL) {
    return;
  }
  passLength = control->rowCount - 1;
  if (passLength <= 0) {
    return;
  }
  selectedEntry = *control->selectedRowSlot;
  /* Each pass moves the largest remaining key to pivotSlot. */
  pivotSlot = control->rowSlots;
  for (; passLength != 0; passLength--) {
    scanSlot = pivotSlot;
    for (compareCount = passLength; compareCount != 0; compareCount--) {
      scanSlot++;
      pivotKey = (const uint32_t *)((uint8_t *)*pivotSlot + fieldOffset);
      scanKey = (const uint32_t *)((uint8_t *)*scanSlot + fieldOffset);
      if ((pivotKey[0] <= scanKey[0]) && ((pivotKey[0] < scanKey[0]) || (pivotKey[1] <= scanKey[1]))) {
        swapEntry = *scanSlot;
        *scanSlot = *pivotSlot;
        *pivotSlot = swapEntry;
      }
    }
    pivotSlot++;
  }
  scanSlot = control->rowSlots;
  selectedRowTop = 0;
  for (rowsRemaining = control->rowCount; rowsRemaining != 0; rowsRemaining--) {
    if (selectedEntry == *scanSlot) break;
    selectedRowTop = selectedRowTop + control->rowHeight;
    scanSlot++;
  }
  if (rowsRemaining == 0) {
    /* Not found: select the first row (the row top stays past the last row, as in the original). */
    scanSlot = control->rowSlots;
  }
  control->selectedRowSlot = scanSlot;
  UiScrollableControl_ClampOffsetsToViewport
            (selectedRowTop + 1 + control->rowHeight,control->base.rightOffset,selectedRowTop,0,
             (UiScrollableControl *)control->base.parent);
  return;
}


/* Address: 0x004BB8A0.
   Sorts the rows of a pointer list by the unsigned dword at fieldOffset in each row entry with an exchange
   sort, then selects the previously selected entry again and scrolls it into view. Equal keys are swapped
   too, so the sort is not stable. Called by the scenario catalog (src/assets/scenario/catalog.c, field 0x50).
*/
void UiPointerList_SortByDwordFieldAscending(UiPointerListFieldByteOffset fieldOffset,UiPointerListControl *control)

{
  void *swapEntry;
  void *selectedEntry;
  int passLength;
  int compareCount;
  int selectedRowTop;
  UiListRowCount rowsRemaining;
  void **pivotSlot;
  void **scanSlot;

  if (control->rowSlots == NULL) {
    return;
  }
  passLength = control->rowCount - 1;
  if (passLength <= 0) {
    return;
  }
  selectedEntry = *control->selectedRowSlot;
  /* Each pass moves the smallest remaining key to pivotSlot. */
  pivotSlot = control->rowSlots;
  for (; passLength != 0; passLength--) {
    scanSlot = pivotSlot;
    for (compareCount = passLength; compareCount != 0; compareCount--) {
      scanSlot++;
      if (*(uint32_t *)((uint8_t *)*scanSlot + fieldOffset) <= *(uint32_t *)((uint8_t *)*pivotSlot + fieldOffset)) {
        swapEntry = *scanSlot;
        *scanSlot = *pivotSlot;
        *pivotSlot = swapEntry;
      }
    }
    pivotSlot++;
  }
  scanSlot = control->rowSlots;
  selectedRowTop = 0;
  for (rowsRemaining = control->rowCount; rowsRemaining != 0; rowsRemaining--) {
    if (selectedEntry == *scanSlot) break;
    selectedRowTop = selectedRowTop + control->rowHeight;
    scanSlot++;
  }
  if (rowsRemaining == 0) {
    /* Not found: select the first row (the row top stays past the last row, as in the original). */
    scanSlot = control->rowSlots;
  }
  control->selectedRowSlot = scanSlot;
  UiScrollableControl_ClampOffsetsToViewport
            (selectedRowTop + 1 + control->rowHeight,(control->base).rightOffset,selectedRowTop,0,
             (UiScrollableControl *)(control->base).parent);
  return;
}


/* Address: 0x004BBCD0.
   Left-button press on the tree list (g_UiTimedListControlVtable nonRightPress): walks the visible rows to
   the one under the pointer. A hit on the expand/collapse icon in the indentation (opaque pixel) calls
   recordSelectionCallback; a hit on the row icon or label (up to 6 pixels past the text) selects the row,
   scrolls it into view and queues the list's action, and a double click also calls recordSelectionCallback
   for a directory row.
*/
void UiTimedListControl_SelectRowFromPointer(int pointerButton,int pointerY,int pointerX,UiNodeBase *control)

{
  /* Rewritten from the assembly (0x004BBCD0-0x004BBE56): expanded records with children push
     their position on the machine stack and descend; the decompiler kept only one level. */
  enum { TREE_DEPTH_LIMIT = 64 };
  UiTimedListTreeControl *list = (UiTimedListTreeControl *)control;
  UiTimedListTreeRecord *savedRecord[TREE_DEPTH_LIMIT];
  int savedRemaining[TREE_DEPTH_LIMIT];
  UiTimedListTreeRecord *record;
  UiTimedListTreeRecord *header;
  int remaining;
  int depth;
  int x;
  int y;
  int indent;

  header = list->base.recordTree;
  if (header == NULL) {
    return;
  }
  remaining = (int)header->countOrLabelText;
  x = pointerX - control->left;
  if (x < 0) {
    return;
  }
  y = pointerY - control->top;
  if ((y < 0) || (remaining == 0)) {
    return;
  }
  record = header + 1;
  depth = 0;
  /* Walk the visible rows until the pointer row is reached. */
  for (y = y - (int)list->base.rowHeight; y > 0; y = y - (int)list->base.rowHeight) {
    record++;
    remaining--;
    if ((((record[-1].flags & UI_TIMED_LIST_RECORD_EXPANDABLE) != 0) &&
         ((record[-1].flags & UI_TIMED_LIST_RECORD_EXPANDED) != 0)) &&
        (record[-1].childBlockOrParentRecord != NULL) &&
        (depth < TREE_DEPTH_LIMIT)) {
      savedRecord[depth] = record;
      savedRemaining[depth] = remaining;
      depth++;
      header = record[-1].childBlockOrParentRecord;
      remaining = (int)header->countOrLabelText;
      record = header + 1;
    }
    while (remaining == 0) {
      if (depth == 0) {
        return;
      }
      depth--;
      record = savedRecord[depth];
      remaining = savedRemaining[depth];
    }
  }
  indent = depth * (int)list->indentPixelsPerLevel;
  x = x - indent;
  if (x < 0) {
    /* Inside the indentation: the expand/collapse icon of the row. */
    if (((record->flags & UI_TIMED_LIST_RECORD_EXPANDABLE) != 0) && (indent != 0) &&
        g_GraphicsTextureSourceTestOpaquePixel
                  (y + (int)list->base.rowHeight,x + (int)list->indentPixelsPerLevel,0,0,
                   list->base.collapsedIconSubresource,list->base.rowTextureSource) &&
        (list->base.recordSelectionCallback != 0)) {
      list->base.recordSelectionCallback(record,list);
    }
    return;
  }
  x = x - (int)list->iconColumnPixels;
  if (x > 0) {
    RichTextExtent extent =
         RichTextCommandStream_MeasureLine(g_UiListTextStyle,(uint16_t *)record->countOrLabelText);
    x = x - (int)extent.widthPixels;
    if (x > 6) {
      return;
    }
  }
  if (record != list->base.selectedRecord) {
    UiTimedListControl_SelectRecordAndScrollIntoView(record,list);
    UiActionQueue_Enqueue(list->base.actionId,control);
  }
  if (((control->nodeFlags & UI_NODE_REPEAT_OR_DOUBLE_CLICK) != 0) &&
      ((record->flags & UI_TIMED_LIST_RECORD_EXPANDABLE) != 0) && (list->base.recordSelectionCallback != 0)) {
    list->base.recordSelectionCallback(record,list);
  }
  return;
}


/* Address: 0x0040FF70.
   Returns the row of recordBlock whose label equals labelUtf16 (exact, case-sensitive compare of at most
   256 code units including the terminator), or NULL. Used by UiTimedListTree_BuildDirectoryHierarchy to find
   the directory row of each path level.
*/
UiTimedListTreeRecord * UiTimedListTree_FindRecordByLabel(uint16_t *labelUtf16,UiTimedListTreeRecord *recordBlock)

{
  uint16_t labelChar;
  int scanRemaining;
  int compareRemaining;
  uint32_t recordsRemaining;
  uint16_t *recordLabelCursor;
  uint16_t *queryLabelCursor;
  bool charsEqual;

  /* Length of the query label including its terminator, at most 256 code units. */
  scanRemaining = 256;
  queryLabelCursor = labelUtf16;
  while (scanRemaining != 0) {
    scanRemaining--;
    labelChar = *queryLabelCursor;
    queryLabelCursor++;
    if (labelChar == 0) break;
  }
  for (recordsRemaining = recordBlock->countOrLabelText; recordsRemaining != 0; recordsRemaining--) {
    recordBlock++;
    charsEqual = false;
    compareRemaining = 256 - scanRemaining;
    recordLabelCursor = (uint16_t *)recordBlock->countOrLabelText;
    queryLabelCursor = labelUtf16;
    while (compareRemaining != 0) {
      compareRemaining--;
      charsEqual = *recordLabelCursor == *queryLabelCursor;
      recordLabelCursor++;
      queryLabelCursor++;
      if (!charsEqual) break;
    }
    if (charsEqual) {
      return recordBlock;
    }
  }
  return NULL;
}

/* Address: 0x0040FFE0.
   Builds one level of the directory tree for the tree list: for an empty path the root block with the
   single "computer" row; for a drive root ("X:" or "X:\") the list of drives, labelled "X:[volume label]"
   with their drive type as icon; otherwise the subdirectories of pathUtf16. The block (header record, rows,
   then the 0x200-byte labels) is allocated from the arena; a row gets flag bit 0 when it has
   subdirectories, i.e. can be expanded. Returns true with the block in *outRecordBlock, or false on failure
   (allocation or enumeration; *outRecordBlock is then left unchanged).
*/
bool UiTimedListTree_BuildDirectoryRecordBlock(uint16_t *pathUtf16,UiTimedListTreeRecord **outRecordBlock)

{
  UiTimedListTreeRecord *recordCursor;
  uint32_t leafCodeUnitPair;
  uint32_t labelCodeUnitPair;
  uint32_t entryStride;
  uint32_t *scanCursor;
  uint32_t *outputRecords;
  UiTimedListTreeRecord *driveRow;
  uint32_t driveLetter;
  EngineDriveTypeCode driveType;
  uint32_t remainingBytes;
  uint32_t labelBytes;
  uint32_t copyDwords;
  int recordCount;
  int scanRemaining;
  uint32_t bytesAfterLabel;
  uint32_t directoryEntryCount;
  uint8_t *driveLetterCursor;
  uint32_t *leaf;
  uint32_t *labelWriteCursor;
  uint32_t *labelCursor;
  uint32_t *scanPointer;
  uint32_t *nextScanPointer;
  void *recordBlock;
  uint32_t largestBlockSize;
  
  if (*pathUtf16 == 0) {
    /* header, the "computer" row and its label */
    if (g_MemoryApi.alloc(2 * sizeof(UiTimedListTreeRecord) + UI_TIMED_LIST_LABEL_BYTES,(void **)&outputRecords) == 0) {
      *outputRecords = 1;
      outputRecords[1] = 0;
      outputRecords[2] = 0;
      outputRecords[3] = UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY;
      outputRecords[4] = (uint32_t)(outputRecords + 8);
      outputRecords[5] = UI_TIMED_LIST_ICON_COMPUTER;
      outputRecords[6] = 0;
      outputRecords[7] = UI_TIMED_LIST_RECORD_EXPANDABLE;
      g_LocaleCopyDefaultComputerLabelUtf16((uint16_t *)(outputRecords + 8));
      *outRecordBlock = (UiTimedListTreeRecord *)outputRecords;
      return true;
    }
  }
  else if ((pathUtf16[3] == 0) || (pathUtf16[2] == 0)) {
    directoryEntryCount =
         g_FileSystemEnumerateDriveLetters((uint8_t *)THANDOR_ADDR(g_UiTimedListDriveLetters,0));
    /* header, then a row and a label per drive */
    if (g_MemoryApi.alloc
              (directoryEntryCount * (sizeof(UiTimedListTreeRecord) + UI_TIMED_LIST_LABEL_BYTES) +
               sizeof(UiTimedListTreeRecord),(void **)&outputRecords) == 0) {
      driveRow = (UiTimedListTreeRecord *)outputRecords + 1;
      *outputRecords = directoryEntryCount;
      outputRecords[1] = 0;
      outputRecords[2] = 0;
      outputRecords[3] = UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY;
      labelCursor = (uint32_t *)(driveRow + directoryEntryCount);
      driveLetterCursor = (uint8_t *)THANDOR_ADDR(g_UiTimedListDriveLetters,0);
      do {
        driveLetter = (uint32_t)*driveLetterCursor;
        driveRow->countOrLabelText = (uint32_t)labelCursor;
        driveType = g_FileSystemGetDriveTypeCode(driveLetter);
        driveRow->parentBlockOrIcon = (uint32_t)driveType;
        driveRow->childBlockOrParentRecord = NULL;
        driveRow->flags = 0;
        g_UiTimedListDriveWildcardUtf16[0] = (wchar_t)driveLetter; /* the pattern L"?:\*.*" */
        *labelCursor = driveLetter;
        ((uint16_t *)labelCursor)[1] = ':';
        ((uint16_t *)labelCursor)[2] = '[';
        ((uint16_t *)labelCursor)[3] = ']';
        ((uint16_t *)labelCursor)[4] = 0;
        /* Label "X:[<volume label>]"; a drive whose media check reports true is closed as "X:[]". */
        if (g_FileSystemCheckDriveMediaReady(driveLetter)) {
          ((uint16_t *)labelCursor)[3] = ']';
          ((uint16_t *)labelCursor)[4] = 0;
        }
        else {
          if (g_FileSystemEnumerateDirectoryOrVolumeEntries
                (FILESYSTEM_ENUMERATE_VOLUME_LABEL,0xffffffff,504, /* the label buffer after "X:[" */
                 (uint8_t *)((uint16_t *)labelCursor + 3),(uint8_t *)g_UiTimedListDriveWildcardUtf16) == 0) {
            ((uint16_t *)labelCursor)[3] = ']';
            ((uint16_t *)labelCursor)[4] = 0;
          }
          else {
            scanRemaining = 256;
            scanPointer = labelCursor;
            do {
              nextScanPointer = scanPointer;
              if (scanRemaining == 0) break;
              scanRemaining--;
              nextScanPointer = (uint32_t *)((uint16_t *)scanPointer + 1);
              labelCodeUnitPair = *scanPointer;
              scanPointer = nextScanPointer;
            } while ((uint16_t)labelCodeUnitPair != 0);
            ((uint16_t *)nextScanPointer)[-1] = ']';
            ((uint16_t *)nextScanPointer)[0] = 0;
          }
          if (g_FileSystemEnumerateDirectoryOrVolumeEntries
                (FILESYSTEM_ENUMERATE_DIRECTORIES,0xffffffff,512,
                 (uint8_t *)g_UiTimedListRecordPathScratch.codeUnits,
                 (uint8_t *)g_UiTimedListDriveWildcardUtf16) != 0) {
            driveRow->flags = driveRow->flags | UI_TIMED_LIST_RECORD_EXPANDABLE;
          }
        }
        labelCursor = labelCursor + UI_TIMED_LIST_LABEL_BYTES / 4;
        driveRow++;
        driveLetterCursor++;
        directoryEntryCount--;
      } while (directoryEntryCount != 0);
      *outRecordBlock = (UiTimedListTreeRecord *)outputRecords;
      return true;
    }
  }
  else {
    WidePath_SplitParentAndLeaf
              (g_UiTimedListRecordPathScratch.codeUnits,g_UiTimedListCombinedPathScratch.codeUnits,
               pathUtf16);
    WidePath_CombineDirectoryAndLeaf
              (g_UiTimedListRecordPathScratch.codeUnits,(uint16_t *)THANDOR_ADDR(g_WildcardAllFilesUtf16,0),
               g_UiTimedListCombinedPathScratch.codeUnits);
    if (g_MemoryApi.allocLargestFreeBlock((void **)&outputRecords,&largestBlockSize) == 0) {
      directoryEntryCount = g_FileSystemEnumerateDirectoryOrVolumeEntries
                         (FILESYSTEM_ENUMERATE_DIRECTORIES,0xffffffff,largestBlockSize,
                          (uint8_t *)outputRecords,(uint8_t *)g_UiTimedListRecordPathScratch.codeUnits);
      entryStride = FILESYSTEM_ENUMERATION_RECORD_BYTES;
      if (g_MemoryApi.shrinkInPlace(entryStride * directoryEntryCount,outputRecords) == 0) {
        if (g_MemoryApi.allocLargestFreeBlock(&recordBlock,&largestBlockSize) == 0) {
          recordCount = directoryEntryCount + 1; /* header and rows */
          remainingBytes = largestBlockSize - (uint32_t)recordCount * sizeof(UiTimedListTreeRecord);
          if ((uint32_t)(recordCount * sizeof(UiTimedListTreeRecord)) <= largestBlockSize &&
              remainingBytes != 0) {
            *(uint32_t *)recordBlock = directoryEntryCount;
            ((uint32_t *)recordBlock)[1] = 0;
            ((uint32_t *)recordBlock)[2] = 0;
            ((uint32_t *)recordBlock)[3] = UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY;
            labelWriteCursor = ((uint32_t *)recordBlock) + recordCount * 4;
            leaf = outputRecords;
            recordCursor = recordBlock;
            for (; directoryEntryCount != 0; directoryEntryCount--) {
              recordCursor[1].countOrLabelText = (uint32_t)labelWriteCursor;
              recordCursor[1].parentBlockOrIcon = UI_TIMED_LIST_ICON_DIRECTORY;
              recordCursor[1].childBlockOrParentRecord = NULL;
              recordCursor[1].flags = 0;
              WidePath_CombineDirectoryAndLeaf
                        (g_UiTimedListRecordPathScratch.codeUnits,(uint16_t *)leaf,
                         g_UiTimedListCombinedPathScratch.codeUnits);
              WidePath_CombineDirectoryAndLeaf
                        (g_UiTimedListSecondaryPathScratch.codeUnits,(uint16_t *)THANDOR_ADDR(g_WildcardAllFilesUtf16,
                                  0),
                         g_UiTimedListRecordPathScratch.codeUnits);
              if (g_FileSystemEnumerateDirectoryOrVolumeEntries
                    (FILESYSTEM_ENUMERATE_DIRECTORIES,0xffffffff,512,
                     (uint8_t *)g_UiTimedListRecordPathScratch.codeUnits,
                     (uint8_t *)g_UiTimedListSecondaryPathScratch.codeUnits) != 0) {
                recordCursor[1].flags = recordCursor[1].flags | UI_TIMED_LIST_RECORD_EXPANDABLE;
              }
              scanRemaining = 256;
              scanCursor = leaf;
              do {
                if (scanRemaining == 0) break;
                scanRemaining--;
                leafCodeUnitPair = *scanCursor;
                scanCursor = (uint32_t *)((uint16_t *)scanCursor + 1);
              } while ((uint16_t)leafCodeUnitPair != 0);
              /* Stop when the label no longer fits; the label size is taken off the remaining bytes
                 twice, as in the original. */
              labelBytes = (258U - scanRemaining) & ~1U;
              bytesAfterLabel = remainingBytes - labelBytes;
              if (remainingBytes < labelBytes || bytesAfterLabel == 0) break;
              remainingBytes = bytesAfterLabel - labelBytes;
              if (bytesAfterLabel < labelBytes || remainingBytes == 0) break;
              scanCursor = leaf;
              for (copyDwords = (258U - scanRemaining) >> 1; copyDwords != 0; copyDwords--) {
                *labelWriteCursor = *scanCursor;
                scanCursor++;
                labelWriteCursor++;
              }
              leaf = (uint32_t *)((uint8_t *)leaf + entryStride);
              recordCursor++;
            }
            /* The loop only ends early (entries left) when the labels no longer fit. */
            if (directoryEntryCount == 0) {
              if (g_MemoryApi.shrinkInPlace
                    ((int)labelWriteCursor + (UI_TIMED_LIST_LABEL_BYTES - (int)recordBlock),
                     recordBlock) == 0) {

                g_MemoryApi.free(outputRecords);
                *outRecordBlock = (UiTimedListTreeRecord *)recordBlock;
                return true;
              }
            }
          }
          g_MemoryApi.free(recordBlock);
        }
      }
      g_MemoryApi.free(outputRecords);
    }
  }
  return false;
}

/* Copies a whole path buffer (WIDE_PATH_MAX_CODE_UNITS code units, as dwords) into
   g_UiTimedListHierarchyPathScratch. */
static void UiTimedListTree_LoadHierarchyPath(const uint16_t *sourcePathUtf16)
{
  const uint32_t *sourceDwords;
  uint32_t *destDwords;
  int copyRemaining;

  sourceDwords = (const uint32_t *)sourcePathUtf16;
  destDwords = (uint32_t *)&g_UiTimedListHierarchyPathScratch;
  for (copyRemaining = WIDE_PATH_MAX_CODE_UNITS / 2; copyRemaining != 0; copyRemaining--) {
    *destDwords = *sourceDwords;
    sourceDwords++;
    destDwords++;
  }
}

/* Failure exit of UiTimedListTree_BuildDirectoryHierarchy: frees levelCount entries from the top of the
   level stack, clears *outSelectedRecord and returns false.
   Original quirk: one pop per level (not per (record, block) pair), so this frees the top blocks and
   interleaved found-record pointers, not every pushed block. */
static bool UiTimedListTree_AbandonDirectoryHierarchy
          (UiTimedListTreeRecord **levelStack,int levelStackTop,int levelCount,
          UiTimedListTreeRecord **outSelectedRecord)
{
  for (; levelCount != 0; levelCount--) {
    g_MemoryApi.free(levelStack[--levelStackTop]);
  }
  *outSelectedRecord = NULL;
  return false;
}

/* Address: 0x00410380.
   Builds the directory tree from the root ("computer") down to selectedPathUtf16: one record block per path
   level (UiTimedListTree_BuildDirectoryRecordBlock), each linked to its parent block and opened (expanded)
   from the parent row that names it. Returns true with the root block in *outRootBlock and the row of the
   selected directory in *outSelectedRecord, or false on failure (then the blocks built so far are freed,
   see UiTimedListTree_AbandonDirectoryHierarchy; *outSelectedRecord is set to NULL, *outRootBlock left
   unchanged). No caller found
   in src/ or image_data.c (only the function map).
*/
bool UiTimedListTree_BuildDirectoryHierarchy
          (uint16_t *selectedPathUtf16,UiTimedListTreeRecord **outRootBlock,
          UiTimedListTreeRecord **outSelectedRecord)

{
  /* The original keeps one (record, block) pair per level on the machine stack (PUSH record,
     PUSH block) and pops them again when linking the levels. A path buffer holds at most 256
     code units, so there are at most 255 path levels plus the root-level match. */
  UiTimedListTreeRecord *levelStack[514];
  int levelStackTop;
  uint32_t recordsRemaining;
  int levelCount;
  uint16_t firstCodeUnit;
  UiTimedListTreeRecord *levelBlock;
  UiTimedListTreeRecord *parentBlock;
  UiTimedListTreeRecord *recordCursor;
  UiTimedListTreeRecord *builtBlock;

  levelCount = 0;
  levelStackTop = 0;
  UiTimedListTree_LoadHierarchyPath(selectedPathUtf16);
  while ((g_UiTimedListHierarchyPathScratch.codeUnits[3] != 0) &&
         (g_UiTimedListHierarchyPathScratch.codeUnits[2] != 0)) {
    if (levelStackTop >= 512) {
      /* Only reachable when splitting stops shortening the path; the original then pushes until
         its stack overflows. */
      return UiTimedListTree_AbandonDirectoryHierarchy
                       (levelStack,levelStackTop,levelCount,outSelectedRecord);
    }
    if (!UiTimedListTree_BuildDirectoryRecordBlock
                       (g_UiTimedListHierarchyPathScratch.codeUnits,&builtBlock)) {
      return UiTimedListTree_AbandonDirectoryHierarchy
                       (levelStack,levelStackTop,levelCount,outSelectedRecord);
    }
    WidePath_SplitParentAndLeaf
              (g_UiTimedListRecordPathScratch.codeUnits,
               g_UiTimedListHierarchyParentPathScratch.codeUnits,
               g_UiTimedListHierarchyPathScratch.codeUnits);
    levelStack[levelStackTop++] =
         UiTimedListTree_FindRecordByLabel
                   (g_UiTimedListRecordPathScratch.codeUnits,builtBlock);
    levelStack[levelStackTop++] = builtBlock;
    levelCount++;
    UiTimedListTree_LoadHierarchyPath(g_UiTimedListHierarchyParentPathScratch.codeUnits);
  }
  /* Root level: find the record whose label starts with the drive letter (case-insensitive). */
  if (!UiTimedListTree_BuildDirectoryRecordBlock(g_UiTimedListHierarchyPathScratch.codeUnits,&builtBlock)) {
    return UiTimedListTree_AbandonDirectoryHierarchy(levelStack,levelStackTop,levelCount,outSelectedRecord);
  }
  levelBlock = builtBlock;
  recordsRemaining = levelBlock->countOrLabelText;
  firstCodeUnit = g_UiTimedListHierarchyPathScratch.codeUnits[0];
  recordCursor = levelBlock + 1;
  while (((firstCodeUnit ^ *(uint16_t *)recordCursor->countOrLabelText) & UI_TIMED_LIST_CASE_FOLD_MASK) != 0) {
    recordCursor++;
    recordsRemaining--;
    if (recordsRemaining == 0) {
      return UiTimedListTree_AbandonDirectoryHierarchy(levelStack,levelStackTop,levelCount,outSelectedRecord);
    }
  }
  levelStack[levelStackTop++] = recordCursor;
  levelStack[levelStackTop++] = levelBlock;
  levelCount++;
  g_UiTimedListHierarchyPathScratch.codeUnits[0] = 0;
  if (!UiTimedListTree_BuildDirectoryRecordBlock(g_UiTimedListHierarchyPathScratch.codeUnits,&builtBlock)) {
    return UiTimedListTree_AbandonDirectoryHierarchy(levelStack,levelStackTop,levelCount,outSelectedRecord);
  }
  /* Link each level block to its parent block and to the parent record that opens it,
     from the drive list down to the selected path. */
  parentBlock = builtBlock;
  recordCursor = parentBlock + 1;
  do {
    levelBlock = levelStack[--levelStackTop];
    if (levelBlock != NULL) {
      levelBlock->parentBlockOrIcon = (uint32_t)parentBlock;
      levelBlock->childBlockOrParentRecord = recordCursor;
    }
    if (recordCursor != NULL) {
      recordCursor->flags = recordCursor->flags | UI_TIMED_LIST_RECORD_EXPANDED;
      recordCursor->childBlockOrParentRecord = levelBlock;
    }
    parentBlock = levelBlock;
    recordCursor = levelStack[--levelStackTop];
    levelCount--;
  } while (levelCount != 0);
  *outRootBlock = builtBlock;
  *outSelectedRecord = recordCursor;
  return true;
}

/* Address: 0x004104B0.
   Frees a record block and every expanded child block below it (collapsing a directory row) and tells in
   CF whether targetRecord (the list's selected row) was one of the freed rows, so the caller can move the
   selection to the collapsed row.
*/
bool UiTimedListTree_FreeRecordBlockRecursiveAndTestContains
          (UiTimedListTreeRecord *targetRecord,UiTimedListTreeRecord *recordBlock)

{
  UiTimedListTreeRecord *recordCursor;
  uint32_t recordsRemaining;
  int containsCount;
  bool childContains;
  
  containsCount = 0;
  if (recordBlock != NULL) {
    recordCursor = recordBlock;
    for (recordsRemaining = recordBlock->countOrLabelText; recordsRemaining != 0; recordsRemaining--) {
      if (recordCursor + 1 == targetRecord) {
        containsCount++;
      }
      if (((recordCursor[1].flags & UI_TIMED_LIST_RECORD_EXPANDABLE) != 0) &&
          ((recordCursor[1].flags & UI_TIMED_LIST_RECORD_EXPANDED) != 0)) {
        childContains = UiTimedListTree_FreeRecordBlockRecursiveAndTestContains
                          (targetRecord,recordCursor[1].childBlockOrParentRecord);
        if (childContains) {
          containsCount++;
        }
      }
      recordCursor++;
    }
  }
  g_MemoryApi.free(recordBlock);
  return containsCount != 0;
}

/* Copies a whole wide path buffer (WIDE_PATH_MAX_CODE_UNITS code units) two code units at a time. */
static void UiTimedListTree_CopyPathDwords(uint32_t *destDwords,const uint32_t *sourceDwords)
{
  int copyRemaining;

  for (copyRemaining = WIDE_PATH_MAX_CODE_UNITS / 2; copyRemaining != 0; copyRemaining--) {
    *destDwords = *sourceDwords;
    sourceDwords++;
    destDwords++;
  }
}

/* Returns the block header of the record block that contains row: the nearest record before it that carries
   the ancestor-boundary flag. */
static UiTimedListTreeRecord *UiTimedListTree_FindBlockHeader(UiTimedListTreeRecord *row)
{
  UiTimedListTreeRecord *header;

  header = row - 1;
  while ((header->flags & UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY) == 0) {
    header--;
  }
  return header;
}

/* Address: 0x00410520.
   Expands a directory row of the tree: builds the full path of the row (its label joined to the labels of
   its ancestor rows, a drive row gives "X:"), enumerates its subdirectories into a new record block and
   links the block below the row (the block header points back to the row and to the row's own block).
   The root "computer" row lists the drives. Returns true when the path or the block cannot be built.
*/
bool UiTimedListTree_AttachDirectoryRecordBlock(UiTimedListTreeRecord *record)

{
  UiTimedListTreeRecord *ancestorRecord;
  UiTimedListTreeRecord *childBlock;
  const uint16_t *ancestorLabel;

  /* Copies the full path buffer size from the label, also past its terminator. */
  UiTimedListTree_CopyPathDwords
            ((uint32_t *)&g_UiTimedListRecordPathScratch,(const uint32_t *)record->countOrLabelText);
  if (((record[-1].flags & UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY) == 0) ||
     (record[-1].parentBlockOrIcon != 0)) {
    if (g_UiTimedListRecordPathScratch.codeUnits[1] == ':') {
      g_UiTimedListRecordPathScratch.codeUnits[2] = 0;
      WidePath_CombineDirectoryAndLeaf
                (g_UiTimedListHierarchyParentPathScratch.codeUnits,(uint16_t *)THANDOR_ADDR(g_WildcardAllFilesUtf16,0),
                 g_UiTimedListRecordPathScratch.codeUnits);
    }
    else {
      /* Prepend the labels of the ancestor rows up to (not including) the drive row. */
      ancestorRecord = UiTimedListTree_FindBlockHeader(record)->childBlockOrParentRecord;
      while ((ancestorRecord != NULL) && (((uint16_t *)ancestorRecord->countOrLabelText)[1] != ':')) {
        WidePath_CombineDirectoryAndLeaf
                  (g_UiTimedListCombinedPathScratch.codeUnits,
                   g_UiTimedListRecordPathScratch.codeUnits,(uint16_t *)ancestorRecord->countOrLabelText);
        UiTimedListTree_CopyPathDwords
                  ((uint32_t *)&g_UiTimedListRecordPathScratch,(const uint32_t *)&g_UiTimedListCombinedPathScratch);
        ancestorRecord = UiTimedListTree_FindBlockHeader(ancestorRecord)->childBlockOrParentRecord;
      }
      if (ancestorRecord == NULL) {
        return true;
      }
      ancestorLabel = (const uint16_t *)ancestorRecord->countOrLabelText;
      g_UiTimedListCombinedPathScratch.firstTwoCodeUnits = *(const uint32_t *)ancestorLabel;
      g_UiTimedListCombinedPathScratch.codeUnits[2] = 0; /* terminator (and one spare zero) after "x:" */
      g_UiTimedListCombinedPathScratch.codeUnits[3] = 0;
      WidePath_CombineDirectoryAndLeaf
                (g_UiTimedListSecondaryPathScratch.codeUnits,
                 g_UiTimedListRecordPathScratch.codeUnits,g_UiTimedListCombinedPathScratch.codeUnits);
      WidePath_CombineDirectoryAndLeaf
                (g_UiTimedListHierarchyParentPathScratch.codeUnits,(uint16_t *)THANDOR_ADDR(g_WildcardAllFilesUtf16,0),
                 g_UiTimedListSecondaryPathScratch.codeUnits);
    }
  }
  else {
    g_UiTimedListHierarchyParentPathScratch.firstTwoCodeUnits = L':' << 16 | L'a'; /* "a:": list the drives */
    g_UiTimedListHierarchyParentPathScratch.codeUnits[2] = 0;
    g_UiTimedListHierarchyParentPathScratch.codeUnits[3] = 0;
  }
  if (!UiTimedListTree_BuildDirectoryRecordBlock
                    (g_UiTimedListHierarchyParentPathScratch.codeUnits,&childBlock)) {
    return true;
  }
  record->childBlockOrParentRecord = childBlock;
  childBlock->childBlockOrParentRecord = record;
  childBlock->parentBlockOrIcon = (uint32_t)UiTimedListTree_FindBlockHeader(record);
  return false;
}

/* Address: 0x00410670.
   Expands or collapses a directory row of the tree list (the directory browser's recordSelectionCallback),
   then recomputes the list layout and scrolls the selection into view. Collapsing a branch that contained
   the selected row moves the selection to the collapsed row and queues the list's action. No caller found
   in src/ or image_data.c (only the function map).
*/
void UiTimedListControl_ToggleDirectoryRecordExpansion
          (UiTimedListTreeRecord *record,UiTimedListTreeControl *control)

{
  UiTimedListTreeRecord *targetRecord;
  bool selectionWasInBranch;
  bool attachFailed;

  targetRecord = UiTimedListControl_GetSelectedRecord(control);
  if ((record->flags & UI_TIMED_LIST_RECORD_EXPANDABLE) != 0) {
    if ((record->flags & UI_TIMED_LIST_RECORD_EXPANDED) != 0) {
      selectionWasInBranch = UiTimedListTree_FreeRecordBlockRecursiveAndTestContains
                        (targetRecord,record->childBlockOrParentRecord);
      if (selectionWasInBranch) {
        UiActionQueue_Enqueue((control->base).actionId,control);
        targetRecord = record;
      }
      record->flags = record->flags & ~UI_TIMED_LIST_RECORD_EXPANDED;
      UiTimedListControl_SetRecordTreeAndRecomputeLayout((control->base).recordTree,control);
      UiTimedListControl_SelectRecordAndScrollIntoView(targetRecord,control);
      return;
    }
    attachFailed = UiTimedListTree_AttachDirectoryRecordBlock(record);
    if (!attachFailed) {
      record->flags = record->flags | UI_TIMED_LIST_RECORD_EXPANDED;
      UiTimedListControl_SetRecordTreeAndRecomputeLayout((control->base).recordTree,control);
      UiTimedListControl_SelectRecordAndScrollIntoView(targetRecord,control);
    }
  }
  return;
}

/* Address: 0x00410700.
   Writes the full path of a tree row to outputPathDwords (256 code units): the row label joined to the
   labels of its ancestor rows; a drive row gives "X:", the root "computer" row its label unchanged. CF set
   when the ancestor chain does not end in a drive row. No caller found in src/ or image_data.c (only the
   function map).
*/
bool UiTimedListTree_BuildRecordPath(uint32_t *outputPathDwords,UiTimedListTreeRecord *record)

{
  uint32_t *directory;
  UiTimedListTreeRecord *ancestorRecord;
  const uint32_t *recordPathSourceDwords;

  recordPathSourceDwords = (const uint32_t *)record->countOrLabelText;
  if (((record[-1].flags & UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY) == 0) ||
     (record[-1].parentBlockOrIcon != 0)) {
    /* Copies the full path buffer size from the label, also past its terminator. */
    UiTimedListTree_CopyPathDwords((uint32_t *)&g_UiTimedListRecordPathScratch,recordPathSourceDwords);
    if (g_UiTimedListRecordPathScratch.codeUnits[1] == ':') {
      g_UiTimedListRecordPathScratch.codeUnits[2] = 0;
      recordPathSourceDwords = (const uint32_t *)&g_UiTimedListRecordPathScratch;
    }
    else {
      /* Prepend the labels of the ancestor rows up to (not including) the drive row. */
      ancestorRecord = UiTimedListTree_FindBlockHeader(record)->childBlockOrParentRecord;
      while ((ancestorRecord != NULL) && (((uint16_t *)ancestorRecord->countOrLabelText)[1] != ':')) {
        WidePath_CombineDirectoryAndLeaf
                  (g_UiTimedListCombinedPathScratch.codeUnits,
                   g_UiTimedListRecordPathScratch.codeUnits,(uint16_t *)ancestorRecord->countOrLabelText);
        UiTimedListTree_CopyPathDwords
                  ((uint32_t *)&g_UiTimedListRecordPathScratch,(const uint32_t *)&g_UiTimedListCombinedPathScratch);
        ancestorRecord = UiTimedListTree_FindBlockHeader(ancestorRecord)->childBlockOrParentRecord;
      }
      if (ancestorRecord == NULL) {
        return true;
      }
      directory = (uint32_t *)ancestorRecord->countOrLabelText;
      g_UiTimedListCombinedPathScratch.firstTwoCodeUnits = *directory;
      g_UiTimedListCombinedPathScratch.codeUnits[2] = 0; /* terminator (and one spare zero) after "x:" */
      g_UiTimedListCombinedPathScratch.codeUnits[3] = 0;
      WidePath_CombineDirectoryAndLeaf
                (g_UiTimedListSecondaryPathScratch.codeUnits,
                 g_UiTimedListRecordPathScratch.codeUnits,g_UiTimedListCombinedPathScratch.codeUnits);
      recordPathSourceDwords = (const uint32_t *)&g_UiTimedListSecondaryPathScratch;
    }
  }
  UiTimedListTree_CopyPathDwords(outputPathDwords,recordPathSourceDwords);
  return false;
}

/* Address: 0x004B11C0.
   Re-enables the controls bound to actionId among firstNode and its following siblings: each node's
   unsuppressActionId method clears UI_NODE_SUPPRESSED when the action matches (containers recurse).
*/
void UiNodeList_UnsuppressActionId(UiActionId actionId,UiNodeBase *firstNode)

{
  for (; firstNode != UI_NODE_NONE; firstNode = firstNode->nextSibling) {
    firstNode->vtable->unsuppressActionId(actionId,firstNode);
  }
  return;
}


/* Address: 0x004B1200.
   Disables (greys out) the controls bound to actionId among firstNode and its following siblings: each
   node's suppressActionId method sets UI_NODE_SUPPRESSED when the action matches (containers recurse).
*/
void UiNodeList_SuppressActionId(UiActionId actionId,UiNodeBase *firstNode)

{
  for (; firstNode != UI_NODE_NONE; firstNode = firstNode->nextSibling) {
    firstNode->vtable->suppressActionId(actionId,firstNode);
  }
  return;
}


/* Address: 0x004B2550.
   Keyboard handler shared by the buttons, check boxes and similar selectable controls (keyboardEvent of
   g_UiSpriteButtonControlVtable, g_UiWindowControlVtable, g_UiNodeVtable_004B1D80, _004BC570, _005162C0,
   _00516310 and _00516530). Space on the focused control, or Enter / Escape when the control binds them,
   activates it: a push button queues its action, a toggle flips its selected state, a radio-style control
   gets selected; optionally with the activation sound. Other keys go to the default focus handling.
*/
bool UiSelectableControl_KeyboardEvent(UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiSoundSelectableControl *control)

{
  bool handled;
  bool activates;

  /* Space activates the focused control (unless disabled); Enter/Escape activate it when the state flags
     bind them. Everything else goes to the default focus handling. */
  activates = false;
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (keyCode == KEYBOARD_KEY_CODE_SPACE) {
      activates = (&(control->selectable).base == g_UiKeyboardFocusNode) &&
                  (((control->selectable).stateFlags & UI_SELECTABLE_IGNORE_FOCUSED_SPACE_ACTIVATION) == 0);
    }
    else if (keyCode == KEYBOARD_KEY_CODE_ENTER) {
      activates = ((control->selectable).stateFlags & UI_SELECTABLE_ACTIVATE_ON_ENTER) != 0;
    }
    else if (keyCode == KEYBOARD_KEY_CODE_ESCAPE) {
      activates = ((control->selectable).stateFlags & UI_SELECTABLE_ACTIVATE_ON_ESCAPE) != 0;
    }
  }
  if (!activates) {
    handled = UiNode_DefaultKeyboardEventMoveFocusNext
                        (keyboardStateMask,keyCode,&(control->selectable).base);
    return handled;
  }
  if (((control->selectable).stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) == 0) {
    if ((((control->selectable).stateFlags & UI_SELECTABLE_PLAY_KEYBOARD_SOUND) != 0) &&
       (control->activationSound != NULL)) {
      g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound,NULL);
    }
    UiActionQueue_Enqueue((control->selectable).actionId,control);
    UiNode_InvalidateRoot(&(control->selectable).base);
    return false;
  }
  if (((control->selectable).stateFlags & UI_SELECTABLE_TOGGLE_ON_ACTIVATION) != 0) {
    if ((((control->selectable).stateFlags & UI_SELECTABLE_PLAY_KEYBOARD_SOUND) != 0) &&
       (control->activationSound != NULL)) {
      g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound,NULL);
    }
    (control->selectable).stateFlags =
         (control->selectable).stateFlags ^ UI_SELECTABLE_SELECTED_OR_CHECKED;
    UiActionQueue_Enqueue((control->selectable).actionId,control);
    UiNode_InvalidateRoot(&(control->selectable).base);
    return false;
  }
  if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
    if ((((control->selectable).stateFlags & UI_SELECTABLE_PLAY_KEYBOARD_SOUND) != 0) &&
       (control->activationSound != NULL)) {
      g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound,NULL);
    }
    (control->selectable).stateFlags =
         (control->selectable).stateFlags | UI_SELECTABLE_SELECTED_OR_CHECKED;
    UiActionQueue_Enqueue((control->selectable).actionId,control);
    UiNode_InvalidateRoot(&(control->selectable).base);
    return false;
  }
  /* Persistent, non-toggling control that is already selected: not consumed. */
  handled = UiNode_DefaultKeyboardEventMoveFocusNext
                      (keyboardStateMask,keyCode,&(control->selectable).base);
  return handled;
}


/* Address: 0x004B26E0.
   Disables (greys out) a selectable control bound to actionId: sets UI_NODE_SUPPRESSED and gives up the
   keyboard focus if it had it. suppressActionId of g_UiGraphicsAdapterTextButtonVtable,
   g_UiSpriteButtonControlVtable, g_UiWindowControlVtable,
   g_UiNumericPairTextButtonVtable, g_UiPayloadPairTextButtonVtable and g_UiNodeVtable_004B1D80, _004B2CE0,
   _004BC570, _005162C0, _00516310, _00516530.
*/
void UiSelectableControl_SuppressIfActionId(UiActionId actionId,UiSelectableControl *control)

{
  UiNodeFlags *controlNodeFlags;
  
  if (actionId == control->actionId) {
    controlNodeFlags = &(control->base).nodeFlags;
    *controlNodeFlags = *controlNodeFlags | UI_NODE_SUPPRESSED;
    UiKeyboardFocus_ReleaseNode(&control->base);
  }
  return;
}


/* Address: 0x004B2710.
   Re-enables a selectable control bound to actionId: clears UI_NODE_SUPPRESSED and takes the keyboard focus
   when no node has it. unsuppressActionId of g_UiGraphicsAdapterTextButtonVtable,
   g_UiSpriteButtonControlVtable, g_UiWindowControlVtable,
   g_UiNumericPairTextButtonVtable, g_UiPayloadPairTextButtonVtable and g_UiNodeVtable_004B1D80, _004B2CE0,
   _004BC570, _005162C0, _00516310, _00516530.
*/
void UiSelectableControl_UnsuppressIfActionId(UiActionId actionId,UiSelectableControl *control)

{
  UiNodeFlags *controlNodeFlags;
  
  if (actionId == control->actionId) {
    controlNodeFlags = &(control->base).nodeFlags;
    *controlNodeFlags = *controlNodeFlags & ~UI_NODE_SUPPRESSED;
    UiKeyboardFocus_AcquireIfNone(&control->base);
  }
  return;
}


/* Address: 0x004B2D30.
   Finds the first enabled (not suppressed), selected control of a group (controlCount control pointers
   follow controlCount on the stack). Returns true when there is one, with its node in *outNode and its
   index in *outIndex; false when none is. Both out-parameters are optional (NULL) and written in either case.
   Original quirk: when none is selected, *outNode is the last control of the group and *outIndex is
   controlCount (the original's EAX/ECX after the loop); some callers use them without testing the result.
*/
bool UiSelectableGroup_FindVisibleSelected
          (UiNodeBase **outNode,uint32_t *outIndex,UiControlCount controlCount,...)

{
  UiSelectableControl **controlSlots;
  UiSelectableControl *control;
  uint32_t controlIndex;
  bool found;

  /* The control pointers follow controlCount on the stack. */
  controlSlots = (UiSelectableControl **)(&controlCount + 1);
  controlIndex = 0;
  found = true;
  control = controlSlots[0];
  while (((control->base.nodeFlags & UI_NODE_SUPPRESSED) != 0) ||
         ((control->stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0)) {
    controlIndex++;
    if (controlCount <= controlIndex) {
      found = false;
      break;
    }
    control = controlSlots[controlIndex];
  }
  if (outNode != NULL) {
    *outNode = (UiNodeBase *)control;
  }
  if (outIndex != NULL) {
    *outIndex = controlIndex;
  }
  return found;
}


/* Address: 0x004B2D70.
   Returns the index of the first selected control of a group (controlCount control pointers follow on the
   stack), suppressed ones included, or controlCount when none is selected. Called by the frontend scenario
   page (src/ui/frontend/scenario.c).
*/
uint32_t UiSelectableGroup_SelectedIndex(UiControlCount controlCount,...)

{
  uint32_t controlIndex;
  int controlPointerByteOffset;

  controlPointerByteOffset = 0;
  controlIndex = 0;
  do {
    if (((*(UiSelectableControl **)((uint8_t *)(&controlCount + 1) + controlPointerByteOffset))->stateFlags &
         UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
      return controlIndex;
    }
    controlIndex++;
    controlPointerByteOffset = controlPointerByteOffset + 4;
  } while (controlIndex < controlCount);
  return controlIndex;
}


/* Address: 0x004B2DA0.
   Radio-button behaviour for a group (controlCount control pointers follow selectedControl on the stack):
   selects selectedControl, deselects all other group members and redraws them.
*/
void UiSelectableGroup_SelectExclusive(UiControlCount controlCount,UiNodeBase *selectedControl,...)

{
  UiSelectableControl *node;
  uint32_t controlIndex;
  int controlPointerByteOffset;

  controlPointerByteOffset = 0;
  controlIndex = 0;
  do {
    node = *(UiSelectableControl **)((uint8_t *)(&selectedControl + 1) + controlPointerByteOffset);
    if (&node->base == selectedControl) {
      node->stateFlags = node->stateFlags | UI_SELECTABLE_SELECTED_OR_CHECKED;
    }
    else {
      node->stateFlags = node->stateFlags & ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    }
    UiNode_InvalidateRoot(&node->base);
    controlIndex++;
    controlPointerByteOffset = controlPointerByteOffset + 4;
  } while (controlIndex < controlCount);
  return;
}


/* Address: 0x004B2DE0.
   Tells (in CF) whether a selectable control counts as selected/checked: only a visible (not suppressed)
   control can be.
*/
uint8_t UiSelectableControl_IsSelected(UiSelectableControl *control)

{
  if ((((control->base).nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     ((control->stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0)) {
    return 1;
  }
  return 0;
}


/* Address: 0x004B2E10.
   Sets or clears the selected/checked state of a selectable control (checkbox, radio or toggle button)
   from code, without queueing its action, and redraws it.
*/
void UiSelectableControl_SetSelected(UiBooleanState32 selected,UiSelectableControl *control)

{
  control->stateFlags = control->stateFlags & ~UI_SELECTABLE_SELECTED_OR_CHECKED;
  if (selected != 0) {
    control->stateFlags = control->stateFlags | UI_SELECTABLE_SELECTED_OR_CHECKED;
  }
  UiNode_InvalidateRoot(&control->base);
  return;
}


/* Address: 0x004B4920.
   Looks up the page stack's shown page (its first child) in its page array and returns the page's index;
   when the shown page is none of the stack's pages it returns pageCount (1 for an empty stack).
*/
uint32_t UiPageStack_ActivePageIndex(UiPageStackControl *stack)

{
  uint32_t pageIndex;

  /* Page 0 is always compared, even when pageCount is 0 (do/while as in the original). */
  pageIndex = 0;
  do {
    if ((stack->base).firstChild == (&stack->pages)[pageIndex]) break;
    pageIndex++;
  } while (pageIndex < stack->pageCount);
  return pageIndex;
}


/* Address: 0x004B7970.
   Relocation of a scroll frame loaded from a serialized UI tree (g_UiScrollableControlVtable relocate):
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
  return;
}


/* The part of a scroll frame not yet taken by bars and frame pieces, relative to the control. */
typedef struct UiScrollFrameContentRect {
  uint32_t left;
  int top;
  int right;
  int bottom;
} UiScrollFrameContentRect;

/* An arrow piece shows pressed while its active flag and the primary interaction flag are both set. */
static bool UiScrollableControl_IsArrowPressed(const UiScrollableControl *control,uint32_t arrowActiveFlag)
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

/* Address: 0x004B79D0.
   Draws a scroll frame (g_UiScrollableControlVtable drawClipped) from g_UiWindowTextureSource pieces: the
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



/* Address: 0x004B8310.
   Layout of a scroll frame (list boxes, text views): the single child is the content, its size is taken from
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
  return;
}


/* Address: 0x004B8AC0.
   Cursor of a scroll frame (g_UiScrollableControlVtable pointerMove): while a right-button pan that started
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


/* Address: 0x004B8B10.
   Right-button press on a scroll frame (g_UiScrollableControlVtable rightPress): starts panning. Pins the
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
  return;
}


/* Address: 0x004B8BC0.
   Right-button release on a scroll frame (g_UiScrollableControlVtable rightRelease): ends panning, releases
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
  return;
}


/* Address: 0x004B9000.
   Hit test of a scroll frame (g_UiScrollableControlVtable hitTest): a pointer inside the content view hits
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
    control = (UiScrollableControl *)UiContainer_HitTestChildren(pointerY,pointerX,&control->base);
  }
  return &control->base;
}


/* Address: 0x004BA4E0.
   Returns the row pointer array of a pointer list (identical to UiPointerList_GetColumnListRowSlots). No
   caller found in src/ or image_data.c (only the function map).
*/
void ** UiPointerList_GetTextListRowSlots(UiPointerListControl *control)

{
  return control->rowSlots;
}


/* Address: 0x004BA560.
   Returns the index of the selected row of a pointer list. The original also reports
   UI_LIST_SELECTION_CONFIRMED in CF (CLC 0x004BA57F / STC 0x004BA587), which this C signature does not carry
   (both branches return the index). No caller reads it: FrontendNetworkSetupPage_InitializeBackendMode
   (0x0054C29C) passes EAX straight to the backend call, FrontendNetworkSetup_OpenSelectedBackend (0x0054D4AD) overwrites
   CF with a CMP.
*/
UiListRowIndex UiPointerList_GetSelectedIndex(UiPointerListControl *control)

{
  UiListRowIndex selectedRowIndex;
  
  selectedRowIndex = control->selectedRowSlot - control->rowSlots;
  if ((control->listStateFlags & UI_LIST_SELECTION_CONFIRMED) == 0) {
    return selectedRowIndex;
  }
  return selectedRowIndex;
}

/* Address: 0x004BADE0.
   Draws the visible rows of the column list (g_UiListControlVtable drawClipped): the highlight bar behind
   the selected row (with end caps while the list has the keyboard focus), then each column's text of the
   row record. A column with a negative width is right-aligned in |width| pixels.
*/
void UiListControl_DrawRowsAndSelection(int clipBottom,int clipRight,int clipTop,int clipLeft,UiListControl *control)

{
  int columnWidth;
  int firstRowIndex;
  int rowTop;
  uint8_t *rowRecord;
  uint32_t lastRowIndex;
  int columnX;
  void **lastRowSlot;
  int highlightWidth;
  int columnsRemaining;
  UiListColumn *column;
  void **rowSlot;
  uint16_t *commandStream;
  bool accessFailed;
  RichTextExtent textExtent;
  GraphicsTextureLogicalSize textureSize;

  if (control->rowCount != 0) {
    firstRowIndex = (clipTop - (control->base).top) / (int)control->rowHeight;
    if (firstRowIndex < 0) {
      firstRowIndex = 0;
    }
    rowSlot = control->rowSlots + firstRowIndex;
    lastRowIndex =
         (uint32_t)(((clipBottom - (control->base).top) + (int)control->rowHeight) / (int)control->rowHeight);
    rowTop = firstRowIndex * (int)control->rowHeight;
    if (control->rowCount <= lastRowIndex) {
      lastRowIndex = control->rowCount - 1;
    }
    lastRowSlot = control->rowSlots + (int)lastRowIndex;
    if (rowSlot <= lastRowSlot) {
      accessFailed = g_GraphicsFramebufferBeginAccess();
      if (!accessFailed) {
        for (; rowSlot <= lastRowSlot; rowSlot++) {
          if (rowSlot == control->selectedRowSlot) {
            highlightWidth = (control->base).layoutWidth;
            if (((control->base).nodeFlags & UI_NODE_HAS_KEYBOARD_FOCUS) == 0) {
              UiWindow_BlitTiledHorizontalEdge
                        (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_ROW_HIGHLIGHT,highlightWidth,
                         rowTop,0,control);
            }
            else {
              textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_ROW_FOCUS_LEFT,
                                                                  g_UiWindowTextureSource);
              highlightWidth = highlightWidth - textureSize.logicalWidthPixels;
              UiWindow_BlitTiledHorizontalEdge
                        (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_ROW_FOCUS_MIDDLE,
                         highlightWidth,rowTop,
                         textureSize.logicalWidthPixels,control);
              g_GraphicsTextureSourceBlitSourceAlpha
                        (clipBottom,clipRight,clipTop,clipLeft,rowTop + (control->base).top,
                         (control->base).left,UI_WINDOW_SUBRESOURCE_ROW_FOCUS_LEFT,g_UiWindowTextureSource,
                         g_FramebufferAccess);
              g_GraphicsTextureSourceBlitSourceAlpha
                        (clipBottom,clipRight,clipTop,clipLeft,rowTop + (control->base).top,
                         highlightWidth + (control->base).left,UI_WINDOW_SUBRESOURCE_ROW_FOCUS_RIGHT,
                         g_UiWindowTextureSource,
                         g_FramebufferAccess);
            }
          }
          rowRecord = (uint8_t *)*rowSlot;
          columnX = 3; /* text inset */
          column = control->columns;
          for (columnsRemaining = control->columnCount; columnsRemaining != 0; columnsRemaining--) {
            columnWidth = column->width;
            if (columnWidth < 0) {
              columnX = columnX - columnWidth;
              commandStream = (uint16_t *)(rowRecord + column->rowTextOffset);
              textExtent = RichTextCommandStream_MeasureLine(g_UiListTextStyle,commandStream);
              RichTextCommandStream_DrawSingleLine
                        (clipBottom,clipRight,clipTop,clipLeft,g_UiListTextStyle,commandStream,
                         rowTop + 1 + (control->base).top,
                         (columnX - (textExtent.widthPixels + 6)) + (control->base).left);
            }
            else {
              columnX = columnX + columnWidth;
              RichTextCommandStream_DrawSingleLine
                        (clipBottom,clipRight,clipTop,clipLeft,g_UiListTextStyle,
                         (uint16_t *)(rowRecord + column->rowTextOffset),
                         rowTop + 1 + (control->base).top,(columnX - columnWidth) + (control->base).left);
            }
            column++;
          }
          rowTop = (int)control->rowHeight + rowTop;
        }
        g_GraphicsFramebufferEndAccess();
      }
    }
  }
  return;
}


/* Address: 0x004BB310.
   Per-frame tick of the column list (g_UiListControlVtable tick): counts down the deferred action of a
   keyboard selection change and queues the list's action when it reaches zero (clearing the pending and
   confirmed flags).
*/
void UiListControl_TickActivationPulse(UiListControl *control)

{
  if ((control->listStateFlags & UI_LIST_DEFERRED_ACTION_PENDING) == 0) {
    return;
  }
  control->listStateFlags = control->listStateFlags - UI_LIST_COUNTDOWN_ONE;
  if ((control->listStateFlags & UI_LIST_COUNTDOWN_MASK) == 0) {
    control->listStateFlags =
         control->listStateFlags &
         (UI_LIST_FLAGS_MASK & ~(UI_LIST_DEFERRED_ACTION_PENDING|UI_LIST_SELECTION_CONFIRMED));
    UiActionQueue_Enqueue(control->actionId,control);
  }
  return;
}


/* Address: 0x004BB350.
   Re-enables the column list when it is bound to actionId (g_UiListControlVtable unsuppressActionId), then
   passes the request on to its children like any container.
*/
void UiListControl_UnsuppressIfActionId(UiActionId actionId,UiListControl *control)

{
  UiNodeFlags *controlNodeFlags;
  
  if (actionId == control->actionId) {
    controlNodeFlags = &(control->base).nodeFlags;
    *controlNodeFlags = *controlNodeFlags & ~UI_NODE_SUPPRESSED;
  }
  UiContainer_UnsuppressActionId(actionId,&control->base);
  return;
}


/* Address: 0x004BB380.
   Disables the column list when it is bound to actionId (g_UiListControlVtable suppressActionId), then
   passes the request on to its children like any container. Unlike the selectable controls it keeps the
   keyboard focus.
*/
void UiListControl_SuppressIfActionId(UiActionId actionId,UiListControl *control)

{
  UiNodeFlags *controlNodeFlags;
  
  if (actionId == control->actionId) {
    controlNodeFlags = &(control->base).nodeFlags;
    *controlNodeFlags = *controlNodeFlags | UI_NODE_SUPPRESSED;
  }
  UiContainer_SuppressActionId(actionId,&control->base);
  return;
}


/* Address: 0x004BB3B0.
   Fills a pointer list with rowCount rows (rowPointers, one record pointer per row) and selects row 0.
   The list's size follows its content: one list-font line plus 1 pixel per row, and the sum of the column
   widths (negative widths count by their magnitude) plus 6 pixels; the parent (the scrollable frame) is
   laid out again for the new size.
*/
void UiPointerList_InitializeColumnLayout(UiListRowCount rowCount,void **rowPointers,UiPointerListControl *control)

{
  int columnWidth;
  UiNodeVtable *parentVtable;
  uint32_t columnsRemaining;
  UiNodeBase *parent;
  UiPixelExtent computedRowHeight;
  int totalWidth;
  UiListColumn *column;
  uint32_t lineHeight;

  FontGlyph_GetLogicalSizeForStyle(g_UiListTextStyle,0,&lineHeight);
  computedRowHeight = lineHeight + 1;
  control->rowHeight = computedRowHeight;
  control->rowCount = rowCount;
  control->rowSlots = rowPointers;
  control->selectedRowSlot = rowPointers;
  totalWidth = 6;
  /* The columns follow the 0x64-byte pointer-list prefix: view the control as the full UiListControl. */
  columnsRemaining = ((UiListControl *)control)->columnCount;
  (control->base).bottomOffset = computedRowHeight * rowCount + 1;
  column = ((UiListControl *)control)->columns;
  for (; columnsRemaining != 0; columnsRemaining--) {
    columnWidth = column->width;
    if (columnWidth < 0) {
      columnWidth = -columnWidth;
    }
    totalWidth = columnWidth + totalWidth;
    column++;
  }
  parent = (control->base).parent;
  parentVtable = parent->vtable;
  (control->base).rightOffset = totalWidth;
  (control->base).leftOffset = 0;
  (control->base).topOffset = 0;
  parentVtable->layout(parent);
  return;
}


/* Address: 0x004BB460.
   Returns the row pointer array of a pointer list (identical to UiPointerList_GetTextListRowSlots). No
   caller found in src/ or image_data.c (only the function map).
*/
void ** UiPointerList_GetColumnListRowSlots(UiPointerListControl *control)

{
  return control->rowSlots;
}


/* Address: 0x004BB9E0.
   Relocation of the tree list loaded from a serialized UI tree (g_UiTimedListControlVtable relocate): only
   relocates the children like any container.
*/
void UiTimedListControl_RelocateChildren(UiSerializedRelocationDelta relocationDelta,UiTimedListControl *control)

{
  UiContainer_RelocateChildren(relocationDelta,&control->base);
  return;
}


/* Address: 0x004BBA00.
   Draws the tree list (g_UiTimedListControlVtable drawClipped): for every visible row the connector lines
   of its ancestor levels, its branch and expand/collapse icons, its row icon, the highlight behind the
   selected row's label (with end caps while the list has the keyboard focus) and the label.
*/
void UiTimedListControl_DrawRowsAndSelection(int clipBottom,int clipRight,int clipTop,int clipLeft,UiNodeBase *control)

{
  /* Rewritten from the assembly (0x004BBA00-0x004BBCCC). Expanded records push (record,
     remaining) on the machine stack and descend; the tree connector columns test the saved
     remaining counts of the parent levels. The decompiler kept only one level. Argument
     positions follow the original pushes. */
  enum { TREE_DEPTH_LIMIT = 64 };
  UiTimedListTreeControl *list = (UiTimedListTreeControl *)control;
  UiTimedListTreeRecord *savedRecord[TREE_DEPTH_LIMIT];
  uint32_t savedRemaining[TREE_DEPTH_LIMIT];
  UiTimedListTreeRecord *header;
  UiTimedListTreeRecord *record;
  uint32_t remaining;
  int depth;
  int rowY;
  int x;
  int y;
  int level;
  GraphicsTextureSourceAsset *rowTexture;

  if (g_GraphicsFramebufferBeginAccess()) {
    return;
  }
  header = list->base.recordTree;
  if ((header != NULL) && (header->countOrLabelText != 0)) {
    rowTexture = list->base.rowTextureSource;
    remaining = header->countOrLabelText;
    record = header + 1;
    depth = 0;
    rowY = 0;
    while (remaining != 0) {
      x = control->left;
      y = rowY + control->top;
      if (depth != 0) {
        /* Connector columns of the ancestor levels: a vertical line where that level still has
           entries below. The root level's saved count is not drawn. */
        for (level = 1; level < depth; level++) {
          if (savedRemaining[level] != 0) {
            g_GraphicsTextureSourceBlitSourceAlpha
                      (clipBottom,clipRight,clipTop,clipLeft,y,x,list->ancestorConnectorSubresource,
                       rowTexture,g_FramebufferAccess);
          }
          x = x + (int)list->indentPixelsPerLevel;
        }
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipBottom,clipRight,clipTop,clipLeft,y,x,
                   (remaining <= 1) ? list->lastRowConnectorSubresource : list->siblingConnectorSubresource,
                   rowTexture,g_FramebufferAccess);
        if ((record->flags & UI_TIMED_LIST_RECORD_EXPANDABLE) != 0) {
          g_GraphicsTextureSourceBlitSourceAlpha
                    (clipBottom,clipRight,clipTop,clipLeft,y,x,
                     ((record->flags & UI_TIMED_LIST_RECORD_EXPANDED) != 0) ? list->expandedIconSubresource :
                                                          list->base.collapsedIconSubresource,
                     rowTexture,g_FramebufferAccess);
        }
        x = x + (int)list->indentPixelsPerLevel;
      }
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,y,x,record->parentBlockOrIcon,rowTexture,
                 g_FramebufferAccess);
      x = x + (int)list->iconColumnPixels - control->left;
      if (record == list->base.selectedRecord) {
        RichTextExtent extent =
             RichTextCommandStream_MeasureLine(g_UiListTextStyle,(uint16_t *)record->countOrLabelText);
        int width = (int)extent.widthPixels + 6;
        if ((control->nodeFlags & UI_NODE_HAS_KEYBOARD_FOCUS) != 0) {
          GraphicsTextureLogicalSize cap =
               g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_ROW_FOCUS_LEFT,g_UiWindowTextureSource);
          int capWidth = (int)cap.logicalWidthPixels;
          int endX = width - capWidth + x;
          UiWindow_BlitTiledHorizontalEdge
                    (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_ROW_FOCUS_MIDDLE,endX,rowY,
                     capWidth + x,control);
          g_GraphicsTextureSourceBlitSourceAlpha
                    (clipBottom,clipRight,clipTop,clipLeft,rowY + control->top,x + control->left,
                     UI_WINDOW_SUBRESOURCE_ROW_FOCUS_LEFT,g_UiWindowTextureSource,g_FramebufferAccess);
          g_GraphicsTextureSourceBlitSourceAlpha
                    (clipBottom,clipRight,clipTop,clipLeft,rowY + control->top,endX + control->left,
                     UI_WINDOW_SUBRESOURCE_ROW_FOCUS_RIGHT,g_UiWindowTextureSource,g_FramebufferAccess);
        }
        else {
          UiWindow_BlitTiledHorizontalEdge
                    (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_ROW_HIGHLIGHT,width + x,rowY,x,
                     control);
        }
      }
      RichTextCommandStream_DrawSingleLine
                (clipBottom,clipRight,clipTop,clipLeft,g_UiListTextStyle,
                 (uint16_t *)record->countOrLabelText,rowY + 1 + control->top,
                 x + 3 + control->left);
      rowY = rowY + (int)list->base.rowHeight;
      record++;
      remaining--;
      if (((record[-1].flags & UI_TIMED_LIST_RECORD_EXPANDABLE) != 0) &&
          ((record[-1].flags & UI_TIMED_LIST_RECORD_EXPANDED) != 0) &&
          (record[-1].childBlockOrParentRecord != NULL) &&
          (depth < TREE_DEPTH_LIMIT)) {
        UiTimedListTreeRecord *children = record[-1].childBlockOrParentRecord;
        savedRecord[depth] = record;
        savedRemaining[depth] = remaining;
        depth++;
        remaining = children->countOrLabelText;
        record = children + 1;
      }
      while ((remaining == 0) && (depth != 0)) {
        depth--;
        record = savedRecord[depth];
        remaining = savedRemaining[depth];
      }
    }
  }
  g_GraphicsFramebufferEndAccess();
  return;
}


/* Address: 0x004BC180.
   Per-frame tick of the tree list (g_UiTimedListControlVtable tick): counts down the deferred action of a
   keyboard selection change and queues the list's action when it reaches zero.
*/
void UiTimedListControl_TickActionDelay(UiTimedListControl *control)

{
  if ((control->listStateAndDelay & UI_TIMED_LIST_ACTION_DELAY_PENDING) == 0) {
    return;
  }
  control->listStateAndDelay = control->listStateAndDelay - UI_LIST_COUNTDOWN_ONE;
  if ((control->listStateAndDelay & UI_LIST_COUNTDOWN_MASK) == 0) {
    control->listStateAndDelay =
         control->listStateAndDelay & (UI_LIST_FLAGS_MASK & ~UI_TIMED_LIST_ACTION_DELAY_PENDING);
    UiActionQueue_Enqueue(control->actionId,control);
  }
  return;
}


/* Address: 0x004BC3F0.
   Returns the selected row of the tree list. Called by UiTimedListControl_ToggleDirectoryRecordExpansion.
*/
UiTimedListTreeRecord *
UiTimedListControl_GetSelectedRecord(UiTimedListTreeControl *control)

{
  return (control->base).selectedRecord;
}

/* Address: 0x004BC460.
   Relocation of a wrapped text control loaded from a serialized UI tree (relocate of
   g_UiListOffsetControlVtable and g_UiCommandVisibilityWrappedTextVtable): relocates the children and, when
   UI_LABEL_TEXT_NEEDS_RELOCATION marks the text pointer as a serialized offset, turns it into a pointer once.
*/
void UiWrappedTextControl_RelocateAndApplyDeferredOffset
          (UiSerializedRelocationDelta relocationDelta,UiWrappedTextControl *control)

{
  UiContainer_RelocateChildren(relocationDelta,&control->base);
  if ((control->labelFlags & UI_LABEL_TEXT_NEEDS_RELOCATION) != 0) {
    control->text = (uint16_t *)((uint8_t *)control->text + relocationDelta);
    control->labelFlags = control->labelFlags & ~UI_LABEL_TEXT_NEEDS_RELOCATION;
  }
  return;
}


/* How many entries of the faction's secondary army-asset list are the given catalog record. */
static int UiCatalogEntryControl_CountOwnedAssets(int factionIndex,const UiCommandRuntimeRecordPrefix *catalogRecord)
{
  FactionArmyAssetCount assetSlotIndex;
  int assetCount;

  assetCount = 0;
  for (assetSlotIndex = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount;
       assetSlotIndex != 0; assetSlotIndex--) {
    if (catalogRecord ==
        *(UiCommandRuntimeRecordPrefix **)
         (factionIndex * sizeof(GameFactionRuntimeRecord) +
          THANDOR_ADDR(g_GameFactionRuntimeImage,offsetof(GameFactionRuntimeRecord,secondaryArmyAssetPointersOrIds) - 4) +
          assetSlotIndex * 4)) {
      assetCount++;
    }
  }
  return assetCount;
}

/* Draws the owned count " n " at the top left of the entry when it is not zero. */
static void UiCatalogEntryControl_DrawOwnedCount
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiCatalogEntryControl *control,UiPackedTextStyle textStyle,int assetCount)
{
  uint32_t textLength;
  int entryTop;

  if (assetCount == 0) {
    return;
  }
  g_UiCatalogEntryRichTextScratchUtf16[0] = ' ';
  textLength = g_WideNumberFormatUtf16
                    (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,assetCount,g_UiCatalogEntryRichTextScratchUtf16 + 1);
  entryTop = (control->command).sprite.selectable.base.top;
  /* ' ' and the terminator */
  *(uint32_t *)((uint8_t *)g_UiCatalogEntryRichTextScratchUtf16 + textLength + 2) = ' ';
  RichTextCommandStream_DrawSingleLine
            (clipBottom,clipRight,clipTop,clipLeft,textStyle,g_UiCatalogEntryRichTextScratchUtf16,entryTop + 2,
             (control->command).sprite.selectable.base.left);
}

/* Takes a building model's progress (elapsed * 100 / required ticks) when it is at least the best so far;
   the text style then follows that model (alert colour when it is switched off). */
static void UiCatalogEntryControl_TakeBuildProgress
          (const ModelRuntimeSlot *model,int *bestPercent,UiPackedTextStyle *textStyle)
{
  int percent;

  percent = (int)(((int64_t)(int)(model->classLinkState).classState64 * 100) /
                  (int64_t)(int)(model->classLinkState).classState68);
  if (*bestPercent <= percent) {
    *textStyle = UI_CATALOG_TEXT_STYLE_NORMAL;
    *bestPercent = percent;
    if (((model->classState).stateFlags & ARMY_MODEL_STATE_SWITCHED_OFF) != 0) {
      *textStyle = UI_CATALOG_TEXT_STYLE_ALERT;
    }
  }
}

/* Highest build progress of this group-42 asset among the active faction's class-11 factories, -1 when none
   builds it. */
static int UiCatalogEntryControl_FindGroup42BuildPercent
          (PckArmyAssetIdCatalog catalogArmyAssetId,UiPackedTextStyle *textStyle)
{
  ModelRuntimeNode *modelNode;
  ModelRuntimeSlot *factoryModelRuntime;
  int bestPercent;

  bestPercent = -1;
  for (modelNode = (ModelRuntimeNode *)(g_InGameRuntimeRoot->worldRuntime).ownerListHead; modelNode != NULL;
      modelNode = (ModelRuntimeNode *)(modelNode->common).nextNode) {
    if (modelNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
      continue;
    }
    factoryModelRuntime = (modelNode->runtimePayload).modelRuntime;
    if ((factoryModelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_11) &&
        ((factoryModelRuntime->classState).behaviorState == ARMY_FACTORY_STATE_BUILDING) &&
        ((g_InGameRuntimeRoot->worldRuntime).activeFactionRuntimeIndex ==
         factoryModelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex) &&
        (catalogArmyAssetId == (factoryModelRuntime->classLinkState).modelLinkOrState.classState)) {
      UiCatalogEntryControl_TakeBuildProgress(factoryModelRuntime,&bestPercent,textStyle);
    }
  }
  return bestPercent;
}

/* Highest build progress of this group-48 asset among the active faction's class-22 pads and class-13
   factories, -1 when none builds it. */
static int UiCatalogEntryControl_FindGroup48BuildPercent
          (PckArmyAssetIdCatalog catalogArmyAssetId,UiPackedTextStyle *textStyle)
{
  ModelRuntimeNode *modelNode;
  ModelRuntimeSlot *slotModelRuntime;
  int factionIndex;
  int bestPercent;

  factionIndex = (g_InGameRuntimeRoot->worldRuntime).activeFactionRuntimeIndex;
  bestPercent = -1;
  for (modelNode = (ModelRuntimeNode *)(g_InGameRuntimeRoot->worldRuntime).ownerListHead; modelNode != NULL;
      modelNode = (ModelRuntimeNode *)(modelNode->common).nextNode) {
    if (modelNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
      continue;
    }
    slotModelRuntime = (modelNode->runtimePayload).modelRuntime;
    if (slotModelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_22) {
      /* class-22 pad (ModelRuntimeLinkedChildSpawnAndBuildView): +0xAC == 1 while it builds, +0x60/+0x64/+0x68
         the selected secondary asset and its elapsed / required build ticks */
      if (((slotModelRuntime->classState).classStateAC == 1) &&
          (factionIndex == slotModelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex) &&
          (catalogArmyAssetId == (slotModelRuntime->classLinkState).modelLinkOrState.classState)) {
        UiCatalogEntryControl_TakeBuildProgress(slotModelRuntime,&bestPercent,textStyle);
      }
    }
    else if ((slotModelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_13) &&
             ((slotModelRuntime->classState).behaviorState == ARMY_FACTORY_STATE_BUILDING) &&
             (factionIndex == slotModelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex) &&
             (catalogArmyAssetId == (slotModelRuntime->classLinkState).modelLinkOrState.classState)) {
      UiCatalogEntryControl_TakeBuildProgress(slotModelRuntime,&bestPercent,textStyle);
    }
  }
  return bestPercent;
}

/* Draws the build progress " n% " at the top right of the entry when some model builds the asset. */
static void UiCatalogEntryControl_DrawBuildPercent
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiCatalogEntryControl *control,UiPackedTextStyle textStyle,int percent)
{
  uint32_t textLength;
  RichTextExtent textExtent;

  if (percent <= -1) {
    return;
  }
  g_UiCatalogEntryRichTextScratchUtf16[0] = ' ';
  textLength = g_WideNumberFormatUtf16
                    (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,percent,g_UiCatalogEntryRichTextScratchUtf16 + 1);
  *(uint16_t *)((uint8_t *)g_UiCatalogEntryRichTextScratchUtf16 + textLength + 2) = '%';
  /* ' ' and the terminator */
  *(uint32_t *)((uint8_t *)g_UiCatalogEntryRichTextScratchUtf16 + textLength + 4) = ' ';
  textExtent = RichTextCommandStream_MeasureLine(UI_CATALOG_TEXT_STYLE_MEASURE,g_UiCatalogEntryRichTextScratchUtf16);
  RichTextCommandStream_DrawSingleLine
            (clipBottom,clipRight,clipTop,clipLeft,textStyle,g_UiCatalogEntryRichTextScratchUtf16,
             (control->command).sprite.selectable.base.top + 2,
             (control->command).sprite.selectable.base.right - textExtent.widthPixels);
}

/* Address: 0x00516580.
   Draws a build catalog entry of the in-game command panel (g_UiNodeVtable_00516530 drawClipped): the
   sprite button, its price (runtimeDisplayValueQ4 in whole units, in the alert colour when the active
   faction's xenite does not cover it), how many of this army asset the faction already owns (top left) and
   the highest build progress (elapsed / required ticks at model runtime +0x64 / +0x68) among the faction's
   factories (class 11, 13) and pads (class 22) currently building this asset (top right, alert colour when that
   model is switched off). The entry is looked up by its
   offset in the in-game root in the group-42 and group-48 catalog tables.
*/
void UiCatalogEntryControl_DrawClipped
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiCatalogEntryControl *control)

{
  uint32_t spriteSubresource;
  uint32_t textLength;
  int recordIndex;
  int factionIndex;
  int buildPercent;
  PckArmyAssetIdCatalog catalogArmyAssetId;
  RichTextExtent textExtent;
  uint32_t backgroundSubresource;
  GraphicsTextureSourceAsset *spriteTextureSource;
  SoftwareFramebufferAccess *framebuffer;
  UiPackedTextStyle overlayTextStyle;

  if (((control->command).sprite.selectable.base.nodeFlags & UI_NODE_SUPPRESSED) != 0) {
    return;
  }
  if ((((control->command).sprite.selectable.stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) &&
      (((control->command).sprite.selectable.stateFlags & UI_SPRITE_BUTTON_SELECTED_ONLY) != 0)) {
    return;
  }
  if (g_GraphicsFramebufferBeginAccess()) {
    return;
  }
  spriteTextureSource = (control->command).sprite.primaryTextureSource;
  framebuffer = g_FramebufferAccess;
  if (((control->command).sprite.selectable.stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
    spriteSubresource = (control->command).sprite.normalSubresourceStartOrDescriptor;
  }
  else {
    if ((((control->command).sprite.selectable.stateFlags & UI_SPRITE_BUTTON_ANIMATED) == 0) &&
       (((control->command).sprite.selectable.stateFlags & UI_SPRITE_BUTTON_ALTERNATE_SELECTED_TEXTURE) != 0)) {
      spriteTextureSource = (control->command).sprite.alternateTextureSource;
    }
    spriteSubresource = (control->command).sprite.selectedSubresourceStart;
    if (((control->command).sprite.selectable.stateFlags & UI_SPRITE_BUTTON_NORMAL_UNDER_SELECTED) != 0) {
      backgroundSubresource = (control->command).sprite.normalSubresourceStartOrDescriptor;
      if (((control->command).sprite.selectable.stateFlags & UI_SPRITE_BUTTON_ANIMATED) != 0) {
        backgroundSubresource = backgroundSubresource + (control->command).sprite.animationFrameOffset;
      }
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,(control->command).sprite.selectable.base.top,
                 (control->command).sprite.selectable.base.left,backgroundSubresource,
                 (control->command).sprite.primaryTextureSource,g_FramebufferAccess);
    }
  }
  if (((control->command).sprite.selectable.stateFlags & UI_SPRITE_BUTTON_ANIMATED) != 0) {
    spriteSubresource = spriteSubresource + (control->command).sprite.animationFrameOffset;
  }
  g_GraphicsTextureSourceBlitSourceAlpha
            (clipBottom,clipRight,clipTop,clipLeft,(control->command).sprite.selectable.base.top,
             (control->command).sprite.selectable.base.left,spriteSubresource,spriteTextureSource,framebuffer);
  factionIndex = (g_InGameRuntimeRoot->worldRuntime).activeFactionRuntimeIndex;
  if ((int)g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 <
      (int)control->runtimeDisplayValueQ4) {
    overlayTextStyle = UI_CATALOG_TEXT_STYLE_ALERT;
  }
  else {
    overlayTextStyle = UI_CATALOG_TEXT_STYLE_NORMAL;
  }
  g_UiCatalogEntryRichTextScratchUtf16[0] = ' ';
  g_UiCatalogEntryRichTextScratchUtf16[1] = 0;
  textLength = g_WideNumberFormatUtf16
                    (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,control->runtimeDisplayValueQ4 >> 4,
                     g_UiCatalogEntryRichTextScratchUtf16 + 1);
  /* ' ' and the terminator */
  *(uint32_t *)((uint8_t *)g_UiCatalogEntryRichTextScratchUtf16 + textLength + 2) = ' ';
  textExtent = RichTextCommandStream_MeasureLine(UI_CATALOG_TEXT_STYLE_MEASURE,g_UiCatalogEntryRichTextScratchUtf16);
  RichTextCommandStream_DrawSingleLine
            (clipBottom,clipRight,clipTop,clipLeft,overlayTextStyle,g_UiCatalogEntryRichTextScratchUtf16,
             ((control->command).sprite.selectable.base.bottom - textExtent.heightPixels) - 2,
             ((int)((control->command).sprite.selectable.base.layoutWidth - textExtent.widthPixels) >> 1)
             + (control->command).sprite.selectable.base.left);
  for (recordIndex = 42 - 1; recordIndex >= 0; recordIndex--) {
    if ((uint8_t *)control - (uint8_t *)g_InGameRuntimeRoot ==
        g_UiCatalogGroup42OffsetTables[g_UiCatalogGroup42ColumnCount][recordIndex]) {
      catalogArmyAssetId = g_UiCatalogGroup42Records[recordIndex]->armyAssetId;
      UiCatalogEntryControl_DrawOwnedCount
                (clipBottom,clipRight,clipTop,clipLeft,control,overlayTextStyle,
                 UiCatalogEntryControl_CountOwnedAssets(factionIndex,g_UiCatalogGroup42Records[recordIndex]));
      buildPercent = UiCatalogEntryControl_FindGroup42BuildPercent(catalogArmyAssetId,&overlayTextStyle);
      UiCatalogEntryControl_DrawBuildPercent
                (clipBottom,clipRight,clipTop,clipLeft,control,overlayTextStyle,buildPercent);
      g_GraphicsFramebufferEndAccess();
      return;
    }
  }
  for (recordIndex = 48 - 1; recordIndex >= 0; recordIndex--) {
    if ((uint8_t *)control - (uint8_t *)g_InGameRuntimeRoot ==
        g_UiCatalogGroup48OffsetTables[g_UiCatalogGroup48ColumnCount][recordIndex]) {
      catalogArmyAssetId = g_UiCatalogGroup48Records[recordIndex]->armyAssetId;
      UiCatalogEntryControl_DrawOwnedCount
                (clipBottom,clipRight,clipTop,clipLeft,control,overlayTextStyle,
                 UiCatalogEntryControl_CountOwnedAssets(factionIndex,g_UiCatalogGroup48Records[recordIndex]));
      buildPercent = UiCatalogEntryControl_FindGroup48BuildPercent(catalogArmyAssetId,&overlayTextStyle);
      UiCatalogEntryControl_DrawBuildPercent
                (clipBottom,clipRight,clipTop,clipLeft,control,overlayTextStyle,buildPercent);
      g_GraphicsFramebufferEndAccess();
      return;
    }
  }
  g_GraphicsFramebufferEndAccess();
}


/* Address: 0x00516B90.
   Pointer over a build catalog entry (g_UiNodeVtable_00516530 pointerMove): finds the entry in the group-42
   or group-48 catalog tables, makes its record the hover selection and rebuilds the selection detail panel,
   so the panel describes the hovered asset. Returns cursor frame 10, or 12 while Ctrl is held.
*/
GraphicsCursorFrameIndex UiCatalogEntryControl_PointerMove
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiCatalogEntryControl *control)

{
  GraphicsCursorFrameIndex cursorFrame;
  int recordIndex;
  int group48Index;
  
  if (((control->command).sprite.selectable.base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    recordIndex = 42 - 1; /* the last group-42 record */
    do {
      if ((uint8_t *)control - (uint8_t *)g_InGameRuntimeRoot ==
          g_UiCatalogGroup42OffsetTables[g_UiCatalogGroup42ColumnCount][recordIndex]) {
        g_UiHoverSelectionRecord = g_UiCatalogGroup42Records[recordIndex];
        InGameSelectionDetailPanel_Rebuild();
        break;
      }
      recordIndex--;
    } while (-1 < recordIndex);
    if (recordIndex < 0) {
      group48Index = 48 - 1; /* the last group-48 record */
      do {
        if ((uint8_t *)control - (uint8_t *)g_InGameRuntimeRoot ==
            g_UiCatalogGroup48OffsetTables[g_UiCatalogGroup48ColumnCount][group48Index]) {
          g_UiHoverSelectionRecord = g_UiCatalogGroup48Records[group48Index];
          InGameSelectionDetailPanel_Rebuild();
          break;
        }
        group48Index--;
      } while (-1 < group48Index);
    }
  }
  cursorFrame = 10;
  if ((g_KeyboardStateMask & KEYBOARD_STATE_CTRL) != 0) {
    cursorFrame = 12;
  }
  return cursorFrame;
}


/* Address: 0x00516C50.
   Left-button release on a pressed build catalog entry (g_UiNodeVtable_00516530 nonRightRelease): releases
   the button, records the modifier keys held (g_KeyboardStateMask into activationInputState) for the action
   handler, plays the activation sound and queues the entry's action.
*/
void UiCatalogEntryControl_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiCatalogEntryControl *control)

{
  UiSelectableStateFlags *stateFlagsField;
  uint32_t activationInputState;
  
  activationInputState = g_KeyboardStateMask;
  if ((((control->command).sprite.selectable.base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     (((control->command).sprite.selectable.stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0)) {
    stateFlagsField = &(control->command).sprite.selectable.stateFlags;
    *stateFlagsField = *stateFlagsField & ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    (control->command).activationInputState = activationInputState;
    if ((((control->command).sprite.selectable.stateFlags & UI_SPRITE_BUTTON_ACTIVATION_SOUND) != 0) &&
       ((control->command).sprite.activationSound != NULL)) {
      g_SoundPlayOneShot
                (g_UiSoundGainQ15,g_UiSoundGainQ15,
                 (control->command).sprite.activationSound,NULL);
    }
    UiActionQueue_Enqueue((control->command).sprite.selectable.actionId,control);
    UiNode_InvalidateRoot((UiNodeBase *)control);
  }
  return;
}


/* Address: 0x004BC360.
   Selects a row of the tree list and scrolls it into view: counts the visible rows above it (walking back
   through its block and up through the ancestor rows, adding the rows of expanded blocks) to get its y.
   Does not queue the list's action.
*/
void UiTimedListControl_SelectRecordAndScrollIntoView
          (UiTimedListTreeRecord *selectedRecord,UiTimedListTreeControl *control)

{
  uint32_t nestedRecordCount;
  int rowAccumulator;
  int rowIndex;
  UiTimedListTreeRecord *countRecord;

  rowAccumulator = 0;
  (control->base).selectedRecord = selectedRecord;
  countRecord = selectedRecord;
  do {
    rowIndex = rowAccumulator;
    if ((countRecord[-1].flags & UI_TIMED_LIST_RECORD_EXPANDED) != 0) {
      nestedRecordCount = UiTimedListTree_CountRecordArrayAndNestedChildren
                        (countRecord[-1].childBlockOrParentRecord);
      rowIndex = rowAccumulator + nestedRecordCount;
    }
    countRecord = countRecord - 1;
    rowAccumulator = rowIndex + 1;
    if ((countRecord->flags & UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY) != 0) {
      /* block header: continue above the parent row (NULL at the root block) */
      countRecord = countRecord->childBlockOrParentRecord;
    }
  } while (countRecord != NULL);
  rowIndex = rowIndex * (control->base).rowHeight;
  UiScrollableControl_ClampOffsetsToViewport
            (rowIndex + 1 + (control->base).rowHeight,(control->base).base.rightOffset,rowIndex,0,
             (UiScrollableControl *)(control->base).base.parent);
  return;
}


/* Address: 0x004BB4E0.
   Selects row index of a pointer list (without queueing its action) and scrolls the list's scrollable
   parent so the row is visible; the same as UiPointerList_SelectTextListIndex. Out-of-range indices are
   ignored.
*/
void UiPointerList_SelectColumnListIndex(UiListRowIndex index,UiPointerListControl *control)

{
  int rowTop;
  
  if (index < control->rowCount) {
    control->selectedRowSlot = control->rowSlots + index;
    rowTop = control->rowHeight * index;
    UiScrollableControl_ClampOffsetsToViewport
              (rowTop + 1 + control->rowHeight,control->base.rightOffset,rowTop,0,
               (UiScrollableControl *)control->base.parent);
  }
  return;
}


/* Address: 0x004BB540.
   Returns the index of the selected row of a pointer list. *outConfirmed (optional, may be NULL) tells
   whether the selection was confirmed (UI_LIST_SELECTION_CONFIRMED, set by a double click on the row).
*/
UiListRowIndex UiPointerList_GetSelectedIndexAndConfirmed(UiPointerListControl *control,bool *outConfirmed)

{
  if (outConfirmed != NULL) {
    *outConfirmed = (control->listStateFlags & UI_LIST_SELECTION_CONFIRMED) != 0;
  }
  return control->selectedRowSlot - control->rowSlots;
}


/* Address: 0x004B9460.
   Returns the size of the view of a scroll frame, or 0/0 when control is not a scroll frame. The lists use it
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

/* Address: 0x004BC1C0.
   Gives the tree list a new record tree (or NULL) and selects its first row: row height from the list font,
   row count over all expanded blocks, width from the widest indented row label plus icon; then the parent
   scroll frame is laid out again for the new content size.
*/
void UiTimedListControl_SetRecordTreeAndRecomputeLayout
          (UiTimedListTreeRecord *recordTree,UiTimedListTreeControl *control)

{
  /* Rewritten from the assembly (0x004BC1C0): expanded records with children push their position
     on the machine stack and descend; the decompiler kept only one level. */
  enum { TREE_DEPTH_LIMIT = 64 };
  UiTimedListTreeRecord *savedRecord[TREE_DEPTH_LIMIT];
  uint32_t savedRemaining[TREE_DEPTH_LIMIT];
  UiTimedListTreeRecord *record;
  UiNodeBase *parent;
  uint32_t fontLineHeight;
  uint32_t remaining;
  uint32_t widest;
  uint32_t width;
  int depth;

  FontGlyph_GetLogicalSizeActiveFont(0,&fontLineHeight);
  remaining = (recordTree == NULL) ? 0 : recordTree->countOrLabelText;
  (control->base).rowHeight = fontLineHeight + 1;
  (control->base).rowCount = remaining;
  (control->base).recordTree = recordTree;
  record = recordTree + 1;
  (control->base).selectedRecord = record;
  widest = 0;
  depth = 0;
  while (remaining != 0) {
    RichTextExtent extent =
         RichTextCommandStream_MeasureLine(g_UiListTextStyle,(uint16_t *)record->countOrLabelText);
    width = extent.widthPixels + control->iconColumnPixels +
            control->indentPixelsPerLevel * (uint32_t)depth;
    record++;
    remaining--;
    if (widest < width) {
      widest = width;
    }
    if (((record[-1].flags & UI_TIMED_LIST_RECORD_EXPANDABLE) != 0) &&
        ((record[-1].flags & UI_TIMED_LIST_RECORD_EXPANDED) != 0) &&
        (record[-1].childBlockOrParentRecord != NULL) &&
        (depth < TREE_DEPTH_LIMIT)) {
      UiTimedListTreeRecord *children = record[-1].childBlockOrParentRecord;
      savedRecord[depth] = record;
      savedRemaining[depth] = remaining;
      depth++;
      remaining = children->countOrLabelText;
      record = children + 1;
      (control->base).rowCount = (control->base).rowCount + remaining;
    }
    while ((remaining == 0) && (depth != 0)) {
      depth--;
      record = savedRecord[depth];
      remaining = savedRemaining[depth];
    }
  }
  parent = (control->base).base.parent;
  (control->base).base.rightOffset = widest + 6;
  (control->base).base.leftOffset = 0;
  (control->base).base.topOffset = 0;
  (control->base).base.bottomOffset = (control->base).rowCount * (control->base).rowHeight + 1;
  parent->vtable->layout(parent);
  return;
}

/* Address: 0x004BC2F0.
   Returns the root record block of the tree list. No caller found in src/ or image_data.c (only the
   function map).
*/
UiTimedListTreeRecord * UiTimedListControl_GetRecordTree(UiTimedListControl *control)

{
  return control->recordTree;
}

/* Address: 0x004BC310.
   Counts the visible rows of a record block: its rows plus, recursively, the rows of every expanded child
   block (0 for NULL). Used to turn a row position into a row index for scrolling.
*/
uint32_t UiTimedListTree_CountRecordArrayAndNestedChildren(UiTimedListTreeRecord *recordBlock)

{
  uint32_t nestedRecordCount;
  uint32_t totalCount;
  uint32_t recordsRemaining;
  
  totalCount = 0;
  if (recordBlock != NULL) {
    totalCount = recordBlock->countOrLabelText;
    recordsRemaining = totalCount;
    do {
      if ((recordBlock[1].flags & UI_TIMED_LIST_RECORD_EXPANDED) != 0) {
        nestedRecordCount = UiTimedListTree_CountRecordArrayAndNestedChildren
                          (recordBlock[1].childBlockOrParentRecord);
        totalCount = totalCount + nestedRecordCount;
      }
      recordsRemaining--;
      recordBlock++;
    } while (recordsRemaining != 0);
  }
  return totalCount;
}


/* Address: 0x004B9170.
   Places the scrolled content (the first child) at the current scroll offsets, clamps the offsets so the
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
  return;
}


/* Address: 0x004B9490.
   Scrolls a scrollable control just far enough that the target rectangle (content coordinates, e.g. a
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
  return;
}

