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
  UiTimedListTreeRecord16 *startRecord;
  UiTimedListTreeRecord16 *scanRecord;
  UiFrameDelayFrames actionDelayFrames;
  UiTimedListTreeRecord16 *childBlock;
  UiTimedListTreeRecord16 *recordCursor;
  UiTimedListTreeRecord16 *stepRecord;
  UiTimedListTreeRecord16 *previousSelection;
  UiTimedListTreeRecord16 *countRecord;
  uint32_t nestedRecordCount;
  UiTimedListTreeRecord16 *lastRecord;
  uint32_t siblingIndex;
  int rowAccumulator;
  int rowIndex;
  int stepCounter;
  bool handled;
  UiScrollableContentDimensionsEdxEax8 contentSize;

  /* A record block is a header record (count, parent block, parent record, ANCESTOR_BOUNDARY flag)
     followed by count row records; an expanded row (flags 1|2) links its child block. */
  stepCounter = 1;
  previousSelection = control->selectedRecord;
  if ((keyCode & 0xffff0000) == 0) { /* plain characters */
    handled = UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
    return handled;
  }
  switch (keyCode) {
  case KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_HOME):
    control->selectedRecord = control->recordTree + 1;
    break;
  case KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_END):
    childBlock = control->recordTree;
    do {
      lastRecord = childBlock + (int)childBlock->recordCountOrRowPayload00;
      control->selectedRecord = lastRecord;
      if (((lastRecord->recordFlags0C & UI_TIMED_LIST_RECORD_OBSERVED_BIT0) == 0) || ((lastRecord->recordFlags0C & UI_TIMED_LIST_RECORD_ENABLES_NESTED_CHILD_TRAVERSAL) == 0)) break;
      childBlock = lastRecord->nestedRecordBlockOrParentLink08;
    } while (childBlock != NULL);
    break;
  case KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_PAGE_UP):
  case KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_UP):
    if (keyCode == KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_PAGE_UP)) {
      /* Page: one step less than the rows visible in the viewport (unsigned 32-bit DIV).
         NOTE: exactly one visible row gives 0 steps, which the do/while wraps like the original. */
      contentSize = UiScrollableControl_QueryContentSizeRegs((UiScrollableControl *)(control->base).parent);
      rowAccumulator = (int)((uint32_t)(contentSize >> 32) / control->rowHeight);
      stepCounter = rowAccumulator - 1;
      if (rowAccumulator < 1) {
        stepCounter = 1;
      }
    }
    /* Step backward: the previous row, descending into the last row of expanded blocks, or up to
       the parent row at a block start. */
    do {
      stepRecord = control->selectedRecord;
      recordCursor = stepRecord - 1;
      control->selectedRecord = recordCursor;
      if ((stepRecord[-1].recordFlags0C & UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY) == 0) {
        while ((((recordCursor->recordFlags0C & UI_TIMED_LIST_RECORD_OBSERVED_BIT0) != 0 && ((recordCursor->recordFlags0C & UI_TIMED_LIST_RECORD_ENABLES_NESTED_CHILD_TRAVERSAL) != 0)) &&
               (recordCursor = recordCursor->nestedRecordBlockOrParentLink08,
               recordCursor != NULL))) {
          recordCursor = recordCursor + (int)recordCursor->recordCountOrRowPayload00;
          control->selectedRecord = recordCursor;
        }
      }
      else {
        recordCursor = stepRecord[-1].nestedRecordBlockOrParentLink08;
        control->selectedRecord = stepRecord;
        if (recordCursor != NULL) {
          control->selectedRecord = recordCursor;
        }
      }
      stepCounter--;
    } while (stepCounter != 0);
    break;
  case KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_PAGE_DOWN):
  case KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_DOWN):
    if (keyCode == KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_PAGE_DOWN)) {
      contentSize = UiScrollableControl_QueryContentSizeRegs((UiScrollableControl *)(control->base).parent);
      rowAccumulator = (int)((uint32_t)(contentSize >> 32) / control->rowHeight);
      stepCounter = rowAccumulator - 1;
      if (rowAccumulator < 1) {
        stepCounter = 1;
      }
    }
    /* Step forward: into the first row of an expanded non-empty block, else the next sibling of
       the row or of its nearest ancestor that has one (stay put at the end). */
    do {
      startRecord = control->selectedRecord;
      siblingIndex = 0;
      scanRecord = startRecord;
      if ((((startRecord->recordFlags0C & UI_TIMED_LIST_RECORD_OBSERVED_BIT0) == 0) || ((startRecord->recordFlags0C & UI_TIMED_LIST_RECORD_ENABLES_NESTED_CHILD_TRAVERSAL) == 0)) ||
         ((recordCursor = startRecord->nestedRecordBlockOrParentLink08,
          recordCursor == NULL || (recordCursor->recordCountOrRowPayload00 == 0))))
      {
        while( true ) {
          do {
            stepRecord = scanRecord;
            siblingIndex++;
            scanRecord = stepRecord - 1;
          } while ((stepRecord[-1].recordFlags0C & UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY) == 0);
          control->selectedRecord = control->selectedRecord + 1;
          if (siblingIndex < stepRecord[-1].recordCountOrRowPayload00) break;
          scanRecord = stepRecord[-1].nestedRecordBlockOrParentLink08;
          siblingIndex = 0;
          control->selectedRecord = scanRecord;
          if (scanRecord == NULL) {
            control->selectedRecord = startRecord;
            break;
          }
        }
      }
      else {
        control->selectedRecord = recordCursor + 1;
      }
      stepCounter--;
    } while (stepCounter != 0);
    break;
  case KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_LEFT): /* collapse an expanded row */
    if ((previousSelection->recordFlags0C & UI_TIMED_LIST_RECORD_OBSERVED_BIT0) == 0) {
      return false;
    }
    if ((previousSelection->recordFlags0C & UI_TIMED_LIST_RECORD_ENABLES_NESTED_CHILD_TRAVERSAL) == 0) {
      return false;
    }
    if (control->recordSelectionCallback == 0) {
      return false;
    }
    control->recordSelectionCallback
              (previousSelection,(UiTimedListRuntimeExtendedView88 *)control);
    return false;
  case KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_RIGHT): /* expand a collapsed directory row */
    if ((previousSelection->recordFlags0C & UI_TIMED_LIST_RECORD_OBSERVED_BIT0) == 0) {
      return false;
    }
    if ((previousSelection->recordFlags0C & UI_TIMED_LIST_RECORD_ENABLES_NESTED_CHILD_TRAVERSAL) != 0) {
      return false;
    }
    if (control->recordSelectionCallback == 0) {
      return false;
    }
    control->recordSelectionCallback
              (previousSelection,(UiTimedListRuntimeExtendedView88 *)control);
    return false;
  default:
    handled = UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
    return handled;
  }
  countRecord = control->selectedRecord;
  rowAccumulator = 0;
  if (countRecord != previousSelection) {
    do {
      previousSelection = countRecord - 1;
      rowIndex = rowAccumulator;
      if ((countRecord[-1].recordFlags0C & UI_TIMED_LIST_RECORD_ENABLES_NESTED_CHILD_TRAVERSAL) != 0) {
        nestedRecordCount = UiTimedListTree_CountRecordArrayAndNestedChildren
                          (countRecord[-1].nestedRecordBlockOrParentLink08);
        rowIndex = rowAccumulator + nestedRecordCount;
      }
      countRecord = previousSelection;
      rowAccumulator = rowIndex + 1;
    } while (((countRecord->recordFlags0C & UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY) == 0) ||
            (countRecord = countRecord->nestedRecordBlockOrParentLink08,
            countRecord != NULL));
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
  UiScrollableContentDimensionsEdxEax8 contentSize;
  
  previousSelectedSlot = control->selectedRowSlot;
  if ((keyCode & 0xffff0000) != 0) { /* not a plain character */
    if (keyCode == KEYBOARD_KEY_CODE_ENTER) {
      control->listStateFlags = control->listStateFlags | UI_LIST_SELECTION_CONFIRMED;
      UiActionQueue_Enqueue(control->actionId,control);
    }
    else if (keyCode == KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_HOME)) {
      control->selectedRowSlot = control->rowSlots;
    }
    else if (keyCode == KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_END)) {
      control->selectedRowSlot = control->rowSlots + (control->rowCount - 1);
    }
    else if (keyCode == KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_PAGE_UP)) {
      contentSize = UiScrollableControl_QueryContentSizeRegs
                        ((UiScrollableControl *)(control->base).parent);
      rowValue = ((uint32_t)((int)control->selectedRowSlot - (int)control->rowSlots) >> 2) -
              ((int)((contentSize >> 32) / (uint64_t)control->rowHeight) - 1);
      if (rowValue < 0) {
        rowValue = 0;
      }
      control->selectedRowSlot = control->rowSlots + rowValue;
    }
    else if (keyCode == KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_PAGE_DOWN)) {
      contentSize = UiScrollableControl_QueryContentSizeRegs
                        ((UiScrollableControl *)(control->base).parent);
      targetRowIndex = ((uint32_t)((int)control->selectedRowSlot - (int)control->rowSlots) >> 2) +
              (int)((contentSize >> 32) / (uint64_t)control->rowHeight) - 1;
      if (control->rowCount <= targetRowIndex) {
        targetRowIndex = control->rowCount - 1;
      }
      control->selectedRowSlot = control->rowSlots + targetRowIndex;
    }
    else if (keyCode == KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_UP)) {
      if (control->rowSlots <= control->selectedRowSlot - 1) {
        control->selectedRowSlot = control->selectedRowSlot - 1;
      }
    }
    else if (keyCode == KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_DOWN)) {
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
        g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
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
  ListSelectionResult selectedIndex;
  UiNodeVtable *parentVtable;
  
  parentNode = control->base.parent;
  parentVtable = parentNode->vtable;
  control->base.bottomOffset = control->rowHeight * control->rowCount + 1;
  parentVtable->layout(parentNode);
  selectedIndex = UiPointerList_GetSelectedIndexVariantB(control);
  UiPointerList_SelectIndexVariantB(selectedIndex.rowIndex,control);
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
  int localXOrTrackBottom;
  bool horizontalBarHit;
  TextureSizeResult textureSize;

  localXOrTrackBottom = pointerX - (control->base).left;
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
           ((int)trackStartOffset <= localXOrTrackBottom) && (localXOrTrackBottom < horizontalTrackEnd);
    }
  }
  if (!horizontalBarHit) {
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) == 0) {
      return;
    }
    textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW,g_UiWindowTextureSource);
    if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_RIGHT) == 0) {
      if (localXOrTrackBottom < 0) {
        return;
      }
      if ((int)textureSize.logicalWidthPixels <= localXOrTrackBottom) {
        return;
      }
    }
    else {
      if ((control->base).layoutWidth <= localXOrTrackBottom) {
        return;
      }
      if (localXOrTrackBottom < (int)((control->base).layoutWidth - textureSize.logicalWidthPixels)) {
        return;
      }
    }
    localXOrTrackBottom = (control->base).layoutHeight;
    textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW,g_UiWindowTextureSource);
    if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM) != 0) {
      localXOrTrackBottom = localXOrTrackBottom - textureSize.logicalHeightPixels;
    }
    trackStartOffset = 0;
    if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) != 0) {
      trackStartOffset = textureSize.logicalHeightPixels;
    }
    if (localY < (int)trackStartOffset) {
      return;
    }
    if (localXOrTrackBottom <= localY) {
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
      if (localXOrTrackBottom - localY <= (int)textureSize.logicalHeightPixels) {
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
  else if (localXOrTrackBottom < control->horizontalThumbLeft) {
    textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW,g_UiWindowTextureSource);
    if ((int)(localXOrTrackBottom - trackStartOffset) < (int)textureSize.logicalWidthPixels) {
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
    if (localXOrTrackBottom < control->horizontalThumbRight) {
      control->pointerAnchorX = localXOrTrackBottom - control->horizontalThumbLeft;
      control->scrollStateFlags = control->scrollStateFlags | UI_SCROLL_HORIZONTAL_THUMB_ACTIVE;
      UiNode_InvalidateRoot(&control->base);
      return;
    }
    textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW,g_UiWindowTextureSource);
    if (horizontalTrackEnd - localXOrTrackBottom <= (int)textureSize.logicalWidthPixels) {
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
  int thumbOffsetOrLocalX;
  int anchorOrLocalY;
  uint32_t arrowSize;
  int trackLength;
  uint32_t arrowStart;
  int arrowEnd;
  bool inArrowBar;
  bool arrowHovered;
  TextureSizeResult textureSize;

  if ((control->scrollStateFlags &
      (UI_SCROLL_VERTICAL_TRACK_AFTER_THUMB_ACTIVE|UI_SCROLL_VERTICAL_TRACK_BEFORE_THUMB_ACTIVE|
       UI_SCROLL_HORIZONTAL_TRACK_AFTER_THUMB_ACTIVE|UI_SCROLL_HORIZONTAL_TRACK_BEFORE_THUMB_ACTIVE)
      ) != 0) {
    return;
  }
  if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_THUMB_ACTIVE) != 0) {
    thumbOffsetOrLocalX = (control->base).left;
    anchorOrLocalY = control->pointerAnchorX;
    trackLength = (control->base).layoutWidth;
    textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW,g_UiWindowTextureSource);
    thumbOffsetOrLocalX = ((pointerX - thumbOffsetOrLocalX) - anchorOrLocalY) - textureSize.logicalWidthPixels;
    trackLength = trackLength + textureSize.logicalWidthPixels * -2;
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW,g_UiWindowTextureSource);
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) != 0) {
        thumbOffsetOrLocalX = thumbOffsetOrLocalX - textureSize.logicalWidthPixels;
      }
      trackLength = trackLength - textureSize.logicalWidthPixels;
    }
    control->scrollOffsetX =
         (UiPixelOffset)
         (((int64_t)(int)(control->contentWidth - control->viewportWidth) * (int64_t)thumbOffsetOrLocalX) /
         (int64_t)-((trackLength - control->horizontalThumbRight) + control->horizontalThumbLeft));
    UiScrollableControl_RefreshChildAndScrollThumbs(control);
    UiNode_InvalidateRoot(&control->base);
    return;
  }
  if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_THUMB_ACTIVE) != 0) {
    thumbOffsetOrLocalX = (control->base).top;
    anchorOrLocalY = control->pointerAnchorY;
    trackLength = (control->base).layoutHeight;
    textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW,g_UiWindowTextureSource);
    thumbOffsetOrLocalX = ((pointerY - thumbOffsetOrLocalX) - anchorOrLocalY) - textureSize.logicalHeightPixels;
    trackLength = trackLength + textureSize.logicalHeightPixels * -2;
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW,g_UiWindowTextureSource);
      if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) != 0) {
        thumbOffsetOrLocalX = thumbOffsetOrLocalX - textureSize.logicalHeightPixels;
      }
      trackLength = trackLength - textureSize.logicalHeightPixels;
    }
    control->scrollOffsetY =
         (UiPixelOffset)
         (((int64_t)(int)(control->contentHeight - control->viewportHeight) * (int64_t)thumbOffsetOrLocalX) /
         (int64_t)-((trackLength - control->verticalThumbBottom) + control->verticalThumbTop));
    UiScrollableControl_RefreshChildAndScrollThumbs(control);
    UiNode_InvalidateRoot(&control->base);
    return;
  }
  /* A held arrow button: repeat (PRIMARY_INTERACTION_ACTIVE) only while the pointer stays on it. */
  anchorOrLocalY = pointerY - (control->base).top;
  thumbOffsetOrLocalX = pointerX - (control->base).left;
  arrowHovered = false;
  if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_DECREMENT_ACTIVE) != 0) {
    textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW,g_UiWindowTextureSource);
    arrowSize = textureSize.logicalWidthPixels;
    if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) == 0) {
      inArrowBar = (anchorOrLocalY < (control->base).layoutHeight) &&
                   ((int)((control->base).layoutHeight - textureSize.logicalHeightPixels) <= anchorOrLocalY);
    }
    else {
      inArrowBar = (-1 < anchorOrLocalY) && (anchorOrLocalY < (int)textureSize.logicalHeightPixels);
    }
    if (inArrowBar) {
      arrowStart = 0;
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) != 0) {
        textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW,g_UiWindowTextureSource);
        arrowStart = textureSize.logicalWidthPixels;
      }
      arrowHovered = ((int)arrowStart <= thumbOffsetOrLocalX) &&
                     (thumbOffsetOrLocalX < (int)(arrowStart + arrowSize));
    }
  }
  else if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_INCREMENT_ACTIVE) != 0) {
    textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW,g_UiWindowTextureSource);
    arrowSize = textureSize.logicalWidthPixels;
    if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) == 0) {
      inArrowBar = (anchorOrLocalY < (control->base).layoutHeight) &&
                   ((int)((control->base).layoutHeight - textureSize.logicalHeightPixels) <= anchorOrLocalY);
    }
    else {
      inArrowBar = (-1 < anchorOrLocalY) && (anchorOrLocalY < (int)textureSize.logicalHeightPixels);
    }
    if (inArrowBar) {
      arrowEnd = (control->base).layoutWidth;
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_RIGHT) != 0) {
        textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW,g_UiWindowTextureSource);
        arrowEnd = arrowEnd - textureSize.logicalWidthPixels;
      }
      arrowHovered = (thumbOffsetOrLocalX < arrowEnd) && ((int)(arrowEnd - arrowSize) <= thumbOffsetOrLocalX);
    }
  }
  else if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_DECREMENT_ACTIVE) != 0) {
    textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW,g_UiWindowTextureSource);
    arrowSize = textureSize.logicalHeightPixels;
    if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) == 0) {
      inArrowBar = (thumbOffsetOrLocalX < (control->base).layoutWidth) &&
                   ((int)((control->base).layoutWidth - textureSize.logicalWidthPixels) <= thumbOffsetOrLocalX);
    }
    else {
      inArrowBar = (-1 < thumbOffsetOrLocalX) && (thumbOffsetOrLocalX < (int)textureSize.logicalWidthPixels);
    }
    if (inArrowBar) {
      arrowStart = 0;
      if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) != 0) {
        textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW,g_UiWindowTextureSource);
        arrowStart = textureSize.logicalHeightPixels;
      }
      arrowHovered = ((int)arrowStart <= anchorOrLocalY) && (anchorOrLocalY < (int)(arrowStart + arrowSize));
    }
  }
  else if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_INCREMENT_ACTIVE) != 0) {
    textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW,g_UiWindowTextureSource);
    arrowSize = textureSize.logicalHeightPixels;
    if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) == 0) {
      inArrowBar = (thumbOffsetOrLocalX < (control->base).layoutWidth) &&
                   ((int)((control->base).layoutWidth - textureSize.logicalWidthPixels) <= thumbOffsetOrLocalX);
    }
    else {
      inArrowBar = (-1 < thumbOffsetOrLocalX) && (thumbOffsetOrLocalX < (int)textureSize.logicalWidthPixels);
    }
    if (inArrowBar) {
      arrowEnd = (control->base).layoutHeight;
      if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM) != 0) {
        textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW,g_UiWindowTextureSource);
        arrowEnd = arrowEnd - textureSize.logicalHeightPixels;
      }
      arrowHovered = (anchorOrLocalY < arrowEnd) && ((int)(arrowEnd - arrowSize) <= anchorOrLocalY);
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

  if (((((control->scrollStateFlags &
         (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) &&
       ((control->scrollStateFlags &
         (UI_SCROLL_PRIMARY_INTERACTION_ACTIVE|UI_SCROLL_SECONDARY_INTERACTION_ACTIVE)) == 0)) &&
      (((control->base).nodeFlags & UI_NODE_SUPPRESSED) == 0)) &&
     (firstChildNode = (control->base).firstChild, wheelDelta != 0)) {
    scrollStep = g_UiScrollWheelDefaultStep;
    if ((firstChildNode != UI_NODE_NONE) &&
       (((firstChildNode->vtable == &g_UiTimedListControlVtable ||
         (firstChildNode->vtable == &g_UiTextListControlVtable)) ||
        (firstChildNode->vtable == &g_UiListControlVtable)))) {
      scrollStep = g_UiScrollWheelListStep;
    }
    control->scrollOffsetY = control->scrollOffsetY + wheelDelta * scrollStep;
    UiScrollableControl_RefreshChildAndScrollThumbs(control);
    UiNode_InvalidateRoot(&control->base);
  }
  return;
}


/* Address: 0x004BA500.
   Selects row index of a pointer list (without queueing its action) and scrolls the list's scrollable
   parent so the row is visible. Out-of-range indices are ignored.
*/
void UiPointerList_SelectIndexVariantA(UiListRowIndex index,UiPointerListControl *control)

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
      if ((((control->base).nodeFlags & UI_NODE_REPEAT_OR_DOUBLE_CLICK) != 0) ||
         (control->listStateFlags = control->listStateFlags & ~UI_LIST_SELECTION_CONFIRMED,
         control->rowSlots + rowIndex != control->selectedRowSlot)) {
        control->selectedRowSlot = control->rowSlots + rowIndex;
        rowTop = rowIndex * control->rowHeight;
        UiScrollableControl_ClampOffsetsToViewport
                  (rowTop + 1 + control->rowHeight,(control->base).rightOffset,rowTop,0,
                   (UiScrollableControl *)(control->base).parent);
        UiActionQueue_Enqueue(control->actionId,control);
        if (((control->listStateFlags & UI_LIST_PLAY_SELECTION_SOUND) != 0) &&
           (control->activationSound != NULL)) {
          g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
        }
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
  uint32_t leftKey;
  uint32_t rightKey;
  int innerCountOrRowTop;
  int outerCount;
  UiListRowCount rowsRemaining;
  void **pivotSlot;
  void **scanSlot;
  
  scanSlot = control->rowSlots;
  if (scanSlot != NULL) {
    innerCountOrRowTop = control->rowCount - 1;
    if ((innerCountOrRowTop != 0) && (-1 < innerCountOrRowTop)) {
      selectedEntry = *control->selectedRowSlot;
      pivotSlot = scanSlot;
      outerCount = innerCountOrRowTop;
      /* each pass moves the largest remaining key to pivotSlot */
      do {
        do {
          scanSlot++;
          leftKey = *(uint32_t *)((int)*pivotSlot + fieldOffset);
          rightKey = *(uint32_t *)((int)*scanSlot + fieldOffset);
          if ((leftKey <= rightKey) &&
             ((leftKey < rightKey ||
              (((uint32_t *)((int)*pivotSlot + fieldOffset))[1] <=
               ((uint32_t *)((int)*scanSlot + fieldOffset))[1])))) {
            LOCK(); /* the original swaps with XCHG */
            swapEntry = *scanSlot;
            *scanSlot = *pivotSlot;
            UNLOCK();
            *pivotSlot = swapEntry;
          }
          innerCountOrRowTop--;
        } while (innerCountOrRowTop != 0);
        scanSlot = pivotSlot + 1;
        innerCountOrRowTop = outerCount - 1;
        pivotSlot = scanSlot;
        outerCount = innerCountOrRowTop;
      } while (innerCountOrRowTop != 0);
      scanSlot = control->rowSlots;
      rowsRemaining = control->rowCount;
      innerCountOrRowTop = 0;
      do {
        if (selectedEntry == *scanSlot) break;
        innerCountOrRowTop = innerCountOrRowTop + control->rowHeight;
        scanSlot++;
        rowsRemaining--;
      } while (rowsRemaining != 0);
      if (rowsRemaining == 0) {
        /* Not found: select the first row (the row top stays past the last row, as in the original). */
        scanSlot = control->rowSlots;
      }
      control->selectedRowSlot = scanSlot;
      UiScrollableControl_ClampOffsetsToViewport
                (innerCountOrRowTop + 1 + control->rowHeight,control->base.rightOffset,innerCountOrRowTop,0,
                 (UiScrollableControl *)control->base.parent);
    }
  }
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
  int innerCountOrRowTop;
  int outerCount;
  UiListRowCount rowsRemaining;
  void **pivotSlot;
  void **scanSlot;
  
  scanSlot = control->rowSlots;
  if (scanSlot != NULL) {
    innerCountOrRowTop = control->rowCount - 1;
    if ((innerCountOrRowTop != 0) && (-1 < innerCountOrRowTop)) {
      selectedEntry = *control->selectedRowSlot;
      pivotSlot = scanSlot;
      outerCount = innerCountOrRowTop;
      /* each pass moves the smallest remaining key to pivotSlot */
      do {
        do {
          scanSlot++;
          if (*(uint32_t *)((int)*scanSlot + fieldOffset) <= *(uint32_t *)((int)*pivotSlot + fieldOffset)) {
            LOCK(); /* the original swaps with XCHG */
            swapEntry = *scanSlot;
            *scanSlot = *pivotSlot;
            UNLOCK();
            *pivotSlot = swapEntry;
          }
          innerCountOrRowTop--;
        } while (innerCountOrRowTop != 0);
        scanSlot = pivotSlot + 1;
        innerCountOrRowTop = outerCount - 1;
        pivotSlot = scanSlot;
        outerCount = innerCountOrRowTop;
      } while (innerCountOrRowTop != 0);
      scanSlot = control->rowSlots;
      rowsRemaining = control->rowCount;
      innerCountOrRowTop = 0;
      do {
        if (selectedEntry == *scanSlot) break;
        innerCountOrRowTop = innerCountOrRowTop + control->rowHeight;
        scanSlot++;
        rowsRemaining--;
      } while (rowsRemaining != 0);
      if (rowsRemaining == 0) {
        /* Not found: select the first row (the row top stays past the last row, as in the original). */
        scanSlot = control->rowSlots;
      }
      control->selectedRowSlot = scanSlot;
      UiScrollableControl_ClampOffsetsToViewport
                (innerCountOrRowTop + 1 + control->rowHeight,(control->base).rightOffset,innerCountOrRowTop,0,
                 (UiScrollableControl *)(control->base).parent);
    }
  }
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
  UiTimedListRuntimeExtendedView88 *list = (UiTimedListRuntimeExtendedView88 *)control;
  UiTimedListTreeRecord16 *savedRecord[TREE_DEPTH_LIMIT];
  int savedRemaining[TREE_DEPTH_LIMIT];
  UiTimedListTreeRecord16 *record;
  UiTimedListTreeRecord16 *header;
  int remaining;
  int depth;
  int x;
  int y;
  int indent;

  header = list->base.recordTree;
  if (header == NULL) {
    return;
  }
  remaining = (int)header->recordCountOrRowPayload00;
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
  for (;;) {
    y = y - (int)list->base.rowHeight;
    if (y <= 0) {
      break;
    }
    record++;
    remaining--;
    if ((((record[-1].recordFlags0C & UI_TIMED_LIST_RECORD_OBSERVED_BIT0) != 0) && ((record[-1].recordFlags0C & UI_TIMED_LIST_RECORD_ENABLES_NESTED_CHILD_TRAVERSAL) != 0)) &&
        (record[-1].nestedRecordBlockOrParentLink08 != NULL) &&
        (depth < TREE_DEPTH_LIMIT)) {
      savedRecord[depth] = record;
      savedRemaining[depth] = remaining;
      depth++;
      header = record[-1].nestedRecordBlockOrParentLink08;
      remaining = (int)header->recordCountOrRowPayload00;
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
  indent = depth * (int)list->observedDrawParameter80;
  x = x - indent;
  if (x < 0) {
    /* Inside the indentation: the expand/collapse icon of the row. */
    if (((record->recordFlags0C & UI_TIMED_LIST_RECORD_OBSERVED_BIT0) != 0) && (indent != 0) &&
        g_GraphicsTextureSourceTestOpaquePixel
                  (y + (int)list->base.rowHeight,x + (int)list->observedDrawParameter80,0,0,
                   list->base.observedDrawParameter6C,list->base.rowTextureSource) &&
        (list->base.recordSelectionCallback != 0)) {
      list->base.recordSelectionCallback(record,list);
    }
    return;
  }
  x = x - (int)list->observedDrawParameter84;
  if (x > 0) {
    RichTextExtentRegs extent =
         RichTextCommandStream_MeasureRegs(g_UiListTextStyle,(uint16_t *)record->recordCountOrRowPayload00);
    x = x - (int)extent.widthPixels;
    if ((x > 0) && (x > 6)) {
      return;
    }
  }
  if (record != list->base.selectedRecord) {
    UiTimedListControl_SelectRecordAndScrollIntoView(record,list);
    UiActionQueue_Enqueue(list->base.actionId,control);
  }
  if (((control->nodeFlags & UI_NODE_REPEAT_OR_DOUBLE_CLICK) != 0) &&
      ((record->recordFlags0C & UI_TIMED_LIST_RECORD_OBSERVED_BIT0) != 0) && (list->base.recordSelectionCallback != 0)) {
    list->base.recordSelectionCallback(record,list);
  }
  return;
}


/* Address: 0x0040FF70.
   Returns the row of recordBlock whose label equals labelUtf16 (exact, case-sensitive compare of at most
   256 code units including the terminator), or NULL. Used by UiTimedListTree_BuildDirectoryHierarchy to find
   the directory row of each path level.
*/
UiTimedListTreeRecord16 * UiTimedListTree_FindRecordByLabel(uint16_t *labelUtf16,UiTimedListTreeRecord16 *recordBlock)

{
  uint16_t labelChar;
  int scanRemaining;
  int compareRemaining;
  uint32_t recordsRemaining;
  uint16_t *recordLabelCursor;
  uint16_t *queryLabelCursor;
  bool charsEqual;
  
  scanRemaining = 256;
  recordLabelCursor = labelUtf16;
  do {
    if (scanRemaining == 0) break;
    scanRemaining--;
    labelChar = *recordLabelCursor;
    recordLabelCursor++;
  } while (labelChar != 0);
  recordsRemaining = recordBlock->recordCountOrRowPayload00;
  do {
    if (recordsRemaining == 0) {
      return NULL;
    }
    recordBlock++;
    charsEqual = false;
    compareRemaining = 256 - scanRemaining;
    recordLabelCursor = (uint16_t *)recordBlock->recordCountOrRowPayload00;
    queryLabelCursor = labelUtf16;
    do {
      if (compareRemaining == 0) break;
      compareRemaining--;
      charsEqual = *recordLabelCursor == *queryLabelCursor;
      recordLabelCursor++;
      queryLabelCursor++;
    } while (charsEqual);
    if (charsEqual) {
      return recordBlock;
    }
    recordsRemaining--;
  } while( true );
}

/* Address: 0x0040FFE0.
   Builds one level of the directory tree for the tree list: for an empty path the root block with the
   single "computer" row; for a drive root ("X:" or "X:\") the list of drives, labelled "X:[volume label]"
   with their drive type as icon; otherwise the subdirectories of pathUtf16. The block (header record, rows,
   then the 0x200-byte labels) is allocated from the arena; a row gets flag bit 0 when it has
   subdirectories, i.e. can be expanded. CF set on failure (allocation or enumeration).
*/
DirectoryRecordBlockResult UiTimedListTree_BuildDirectoryRecordBlock(uint16_t *pathUtf16)

{
  uint32_t *recordCursor;
  uint32_t leafCodeUnitPair;
  uint32_t *entryStride;
  uint32_t *scanCursor;
  uint32_t *outputRecords;
  uint32_t driveLetter;
  EngineDriveTypeCode driveType;
  uint32_t *remainingBytes;
  int scanRemaining;
  uint32_t *bytesAfterLabel;
  uint32_t scanValue;
  uint32_t directoryEntryCount;
  uint8_t *driveLetterCursor;
  uint32_t *leaf;
  uint32_t *labelWriteCursor;
  uint32_t *labelCursor;
  uint32_t *scanPointer;
  uint32_t *nextScanPointer;
  bool mediaCheckResult;
  bool closeLabelEmpty;
  ArenaShrinkResult shrinkResult;
  DirectoryRecordBlockResult result;
  DirectoryRecordBlockResult failureResult;
  ArenaAllocResult allocResult;
  ArenaLargestAllocResult largestBlock;
  DirectoryEnumerationResult enumResult;
  DriveLetterEnumerationEaxEcx8 driveEnum;
  
  if (*pathUtf16 == 0) {
    allocResult = g_MemoryApi.alloc(0x220); /* header, one row, one label */
    outputRecords = (uint32_t *)allocResult.payloadOrError;
    if (!allocResult.failed) {
      *outputRecords = 1;
      outputRecords[1] = 0;
      outputRecords[2] = 0;
      outputRecords[3] = UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY;
      outputRecords[4] = (uint32_t)(outputRecords + 8);
      outputRecords[5] = UI_TIMED_LIST_ICON_COMPUTER;
      outputRecords[6] = 0;
      outputRecords[7] = UI_TIMED_LIST_RECORD_OBSERVED_BIT0;
      g_LocaleCopyDefaultComputerLabelUtf16((uint16_t *)(outputRecords + 8));
      return THANDOR_BITCAST(uint64_t, DirectoryRecordBlockResult, ((THANDOR_BITCAST(ArenaAllocResult, uint64_t, allocResult) & 0xFFFFFFFFFFull) & 0xffffffff));
    }
  }
  else if ((pathUtf16[3] == 0) || (pathUtf16[2] == 0)) {
    driveEnum = g_FileSystemEnumerateDriveLetters((uint8_t *)THANDOR_ADDR(g_UiTimedListDriveLetters,0));
    directoryEntryCount = driveEnum.driveCount;
    allocResult = g_MemoryApi.alloc(driveEnum.driveCountMirror * 0x210 + 0x10); /* header, then a row and a label per drive */
    outputRecords = (uint32_t *)allocResult.payloadOrError;
    if (!allocResult.failed) {
      result.recordBlockOrError = outputRecords + 4;
      *outputRecords = directoryEntryCount;
      outputRecords[1] = 0;
      outputRecords[2] = 0;
      outputRecords[3] = UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY;
      labelCursor = ((uint32_t *)result.recordBlockOrError) + directoryEntryCount * 4;
      driveLetterCursor = (uint8_t *)THANDOR_ADDR(g_UiTimedListDriveLetters,0);
      do {
        driveLetter = (uint32_t)*driveLetterCursor;
        *(uint32_t *)result.recordBlockOrError = (uint32_t)labelCursor;
        scanValue = driveLetter;
        driveType = g_FileSystemGetDriveTypeCode(driveLetter);
        ((uint32_t *)result.recordBlockOrError)[1] = (uint32_t)driveType;
        ((uint32_t *)result.recordBlockOrError)[2] = 0;
        ((uint32_t *)result.recordBlockOrError)[3] = 0;
        u________0040ff58[0] = (wchar_t)driveLetter; /* the pattern L"?:\*.*" */
        *labelCursor = driveLetter;
        ((uint16_t *)((int)labelCursor + 2))[0] = ':';
        ((uint16_t *)((int)labelCursor + 2))[1] = '[';
        ((uint16_t *)((int)labelCursor + 6))[0] = ']';
        ((uint16_t *)((int)labelCursor + 6))[1] = 0;
        /* Label "X:[<volume label>]"; any failure closes it as "X:[]" (a failing directory probe
           also truncates an already written volume label, as in the original). */
        mediaCheckResult = g_FileSystemCheckDriveMediaReady(scanValue);
        closeLabelEmpty = mediaCheckResult;
        if (!closeLabelEmpty) {
          enumResult = g_FileSystemEnumerateDirectoryOrVolumeEntries
                             (FILESYSTEM_ENUMERATE_VOLUME_LABEL,0xffffffff,0x1f8, /* the label buffer after "X:[" */
                              (uint8_t *)((int)labelCursor + 6),(uint8_t *)u________0040ff58);
          closeLabelEmpty = enumResult.failed;
          if (!closeLabelEmpty) {
            if (enumResult.entryCount == 0) {
              ((uint16_t *)((int)labelCursor + 6))[0] = ']';
              ((uint16_t *)((int)labelCursor + 6))[1] = 0;
            }
            else {
              scanRemaining = 256;
              scanPointer = labelCursor;
              do {
                nextScanPointer = scanPointer;
                if (scanRemaining == 0) break;
                scanRemaining--;
                nextScanPointer = (uint32_t *)((int)scanPointer + 2);
                scanValue = *scanPointer;
                scanPointer = nextScanPointer;
              } while ((uint16_t)scanValue != 0);
              ((uint16_t *)((int)nextScanPointer - 2))[0] = ']';
              ((uint16_t *)((int)nextScanPointer - 2))[1] = 0;
            }
            enumResult = g_FileSystemEnumerateDirectoryOrVolumeEntries
                               (FILESYSTEM_ENUMERATE_DIRECTORIES,0xffffffff,0x200,
                                (uint8_t *)g_UiTimedListRecordPathScratch.codeUnits,
                                (uint8_t *)u________0040ff58);
            closeLabelEmpty = enumResult.failed;
            if ((!closeLabelEmpty) && (enumResult.entryCount != 0)) {
              ((uint32_t *)result.recordBlockOrError)[3] = ((uint32_t *)result.recordBlockOrError)[3] | UI_TIMED_LIST_RECORD_OBSERVED_BIT0;
            }
          }
        }
        if (closeLabelEmpty) {
          ((uint16_t *)((int)labelCursor + 6))[0] = ']';
          ((uint16_t *)((int)labelCursor + 6))[1] = 0;
        }
        labelCursor = labelCursor + UI_TIMED_LIST_LABEL_BYTES / 4;
        result.recordBlockOrError = (uint32_t *)((uint32_t *)result.recordBlockOrError) + 4;
        driveLetterCursor++;
        directoryEntryCount--;
        if (directoryEntryCount == 0) {
          return THANDOR_BITCAST(uint64_t, DirectoryRecordBlockResult, ((THANDOR_BITCAST(ArenaAllocResult, uint64_t, allocResult) & 0xFFFFFFFFFFull) & 0xffffffff));
        }
      } while( true );
    }
  }
  else {
    WidePath_SplitParentAndLeaf
              (g_UiTimedListRecordPathScratch.codeUnits,g_UiTimedListCombinedPathScratch.codeUnits,
               pathUtf16);
    WidePath_CombineDirectoryAndLeaf
              (g_UiTimedListRecordPathScratch.codeUnits,(uint16_t *)THANDOR_ADDR(g_WildcardAllFilesUtf16,0),
               g_UiTimedListCombinedPathScratch.codeUnits);
    largestBlock = g_MemoryApi.allocLargestFreeBlock();
    outputRecords = (uint32_t *)largestBlock.allocationOrError;
    if (!largestBlock.failed) {
      enumResult = g_FileSystemEnumerateDirectoryOrVolumeEntries
                         (FILESYSTEM_ENUMERATE_DIRECTORIES,0xffffffff,largestBlock.blockSizeOrSentinel,
                          (uint8_t *)outputRecords,(uint8_t *)g_UiTimedListRecordPathScratch.codeUnits);
      directoryEntryCount = enumResult.entryCount;
      entryStride = (uint32_t *)enumResult.recordSizeBytes;
      result.recordBlockOrError = entryStride;
      if (!enumResult.failed) {
        shrinkResult = g_MemoryApi.shrinkInPlace((int)entryStride * directoryEntryCount,outputRecords);
        result.recordBlockOrError = (uint32_t *)shrinkResult.scratchOrError;
        if (!shrinkResult.failed) {
          largestBlock = g_MemoryApi.allocLargestFreeBlock();
          result.recordBlockOrError = (uint32_t *)largestBlock.allocationOrError;
          if (!largestBlock.failed) {
            scanRemaining = directoryEntryCount + 1;
            scanCursor = (uint32_t *)0x14;
            remainingBytes = (uint32_t *)(largestBlock.blockSizeOrSentinel + scanRemaining * -0x10);
            if ((uint32_t)(scanRemaining * 0x10) <= largestBlock.blockSizeOrSentinel && remainingBytes != NULL) {
              *(uint32_t *)result.recordBlockOrError = directoryEntryCount;
              ((uint32_t *)result.recordBlockOrError)[1] = 0;
              ((uint32_t *)result.recordBlockOrError)[2] = 0;
              ((uint32_t *)result.recordBlockOrError)[3] = UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY;
              labelWriteCursor = ((uint32_t *)result.recordBlockOrError) + scanRemaining * 4;
              leaf = outputRecords;
              recordCursor = result.recordBlockOrError;
              for (; directoryEntryCount != 0; directoryEntryCount--) {
                recordCursor[4] = (uint32_t)labelWriteCursor;
                recordCursor[5] = UI_TIMED_LIST_ICON_DIRECTORY;
                recordCursor[6] = 0;
                recordCursor[7] = 0;
                WidePath_CombineDirectoryAndLeaf
                          (g_UiTimedListRecordPathScratch.codeUnits,(uint16_t *)leaf,
                           g_UiTimedListCombinedPathScratch.codeUnits);
                WidePath_CombineDirectoryAndLeaf
                          (g_UiTimedListSecondaryPathScratch.codeUnits,(uint16_t *)THANDOR_ADDR(g_WildcardAllFilesUtf16,0),
                           g_UiTimedListRecordPathScratch.codeUnits);
                enumResult = g_FileSystemEnumerateDirectoryOrVolumeEntries
                                   (FILESYSTEM_ENUMERATE_DIRECTORIES,0xffffffff,0x200,
                                    (uint8_t *)g_UiTimedListRecordPathScratch.codeUnits,
                                    (uint8_t *)g_UiTimedListSecondaryPathScratch.codeUnits);
                if ((!enumResult.failed) && (enumResult.entryCount != 0)) {
                  recordCursor[7] = recordCursor[7] | UI_TIMED_LIST_RECORD_OBSERVED_BIT0;
                }
                scanRemaining = 256;
                scanCursor = leaf;
                do {
                  if (scanRemaining == 0) break;
                  scanRemaining--;
                  leafCodeUnitPair = *scanCursor;
                  scanCursor = (uint32_t *)((int)scanCursor + 2);
                } while ((uint16_t)leafCodeUnitPair != 0);
                scanCursor = (uint32_t *)(0x102U - scanRemaining & 0xfffffffe);
                bytesAfterLabel = (uint32_t *)((int)remainingBytes - (int)scanCursor);
                if ((remainingBytes < scanCursor || bytesAfterLabel == NULL) ||
                   (remainingBytes = (uint32_t *)((int)bytesAfterLabel - (int)scanCursor),
                   bytesAfterLabel < scanCursor || remainingBytes == NULL)) break;
                scanCursor = leaf;
                for (scanValue = 0x102U - scanRemaining >> 1; scanValue != 0; scanValue--) {
                  *labelWriteCursor = *scanCursor;
                  scanCursor++;
                  labelWriteCursor++;
                }
                leaf = (uint32_t *)((int)leaf + (int)entryStride);
                recordCursor = recordCursor + 4;
              }
              /* The loop only ends early (entries left) when the labels no longer fit. */
              if (directoryEntryCount == 0) {
                shrinkResult = g_MemoryApi.shrinkInPlace
                                   ((int)labelWriteCursor + (0x200 - (int)result.recordBlockOrError),
                                    result.recordBlockOrError);
                scanCursor = (uint32_t *)shrinkResult.scratchOrError;
                if (!shrinkResult.failed) {
                  g_MemoryApi.free(outputRecords);
                  result.failed = false;
                  return result;
                }
              }
            }
            g_MemoryApi.free(result.recordBlockOrError);
            result.recordBlockOrError = scanCursor;
          }
        }
      }
      g_MemoryApi.free(outputRecords);
      outputRecords = result.recordBlockOrError;
    }
  }
  failureResult.failed = true;
  failureResult.recordBlockOrError = outputRecords;
  return failureResult;
}

/* Address: 0x00410380.
   Builds the directory tree from the root ("computer") down to selectedPathUtf16: one record block per path
   level (UiTimedListTree_BuildDirectoryRecordBlock), each linked to its parent block and opened (expanded)
   from the parent row that names it. Returns the root block and the row of the selected directory, or CF
   set on failure (then the blocks built so far are freed, see the note at fail). No caller found in src/
   or image_data.c (only the function map).
*/
DirectoryHierarchyResult UiTimedListTree_BuildDirectoryHierarchy(uint16_t *selectedPathUtf16)

{
  /* The original keeps one (record, block) pair per level on the machine stack (PUSH record,
     PUSH block) and pops them again when linking the levels. A path buffer holds at most 256
     code units, so there are at most 255 path levels plus the root-level match. */
  UiTimedListTreeRecord16 *levelStack[0x202];
  int levelStackTop;
  int copyRemaining;
  uint32_t recordsRemaining;
  int levelCount;
  uint16_t firstCodeUnit;
  WidePathBuffer256 *pathCopyCursor;
  UiTimedListTreeRecord16 *levelBlock;
  UiTimedListTreeRecord16 *parentBlock;
  UiTimedListTreeRecord16 *recordCursor;
  DirectoryRecordBlockResult builtBlock;
  DirectoryHierarchyResult result;

  levelCount = 0;
  levelStackTop = 0;
  while( true ) {
    pathCopyCursor = &g_UiTimedListHierarchyPathScratch;
    for (copyRemaining = 0x80; copyRemaining != 0; copyRemaining--) { /* 256 code units */
      pathCopyCursor->firstTwoCodeUnits = *(uint32_t *)selectedPathUtf16;
      selectedPathUtf16 = (uint16_t *)(selectedPathUtf16 + 2);
      pathCopyCursor = (WidePathBuffer256 *)(&pathCopyCursor->firstTwoCodeUnits + 1);
    }
    if ((g_UiTimedListHierarchyPathScratch.codeUnits[3] == 0) ||
       (g_UiTimedListHierarchyPathScratch.codeUnits[2] == 0)) break;
    if (levelStackTop >= 0x200) {
      /* Only reachable when splitting stops shortening the path; the original then pushes until
         its stack overflows. */
      builtBlock.recordBlockOrError = NULL;
      goto fail;
    }
    builtBlock = UiTimedListTree_BuildDirectoryRecordBlock
                       (g_UiTimedListHierarchyPathScratch.codeUnits);
    if (builtBlock.failed) goto fail;
    WidePath_SplitParentAndLeaf
              (g_UiTimedListRecordPathScratch.codeUnits,
               g_UiTimedListHierarchyParentPathScratch.codeUnits,
               g_UiTimedListHierarchyPathScratch.codeUnits);
    levelStack[levelStackTop++] =
         UiTimedListTree_FindRecordByLabel
                   (g_UiTimedListRecordPathScratch.codeUnits,builtBlock.recordBlockOrError);
    levelStack[levelStackTop++] = builtBlock.recordBlockOrError;
    levelCount++;
    selectedPathUtf16 = (uint16_t *)&g_UiTimedListHierarchyParentPathScratch;
  }
  /* Root level: find the record whose label starts with the drive letter (case-insensitive). */
  builtBlock = UiTimedListTree_BuildDirectoryRecordBlock(g_UiTimedListHierarchyPathScratch.codeUnits);
  if (builtBlock.failed) goto fail;
  levelBlock = builtBlock.recordBlockOrError;
  recordsRemaining = levelBlock->recordCountOrRowPayload00;
  firstCodeUnit = g_UiTimedListHierarchyPathScratch.codeUnits[0];
  recordCursor = levelBlock + 1;
  while (((firstCodeUnit ^ *(uint16_t *)recordCursor->recordCountOrRowPayload00) & 0xdf) != 0) {
    recordCursor++;
    recordsRemaining--;
    if (recordsRemaining == 0) {
      builtBlock.recordBlockOrError = NULL;
      goto fail;
    }
  }
  levelStack[levelStackTop++] = recordCursor;
  levelStack[levelStackTop++] = levelBlock;
  levelCount++;
  g_UiTimedListHierarchyPathScratch.codeUnits[0] = 0;
  builtBlock = UiTimedListTree_BuildDirectoryRecordBlock(g_UiTimedListHierarchyPathScratch.codeUnits);
  if (builtBlock.failed) goto fail;
  /* Link each level block to its parent block and to the parent record that opens it,
     from the drive list down to the selected path. */
  parentBlock = builtBlock.recordBlockOrError;
  recordCursor = parentBlock + 1;
  do {
    levelBlock = levelStack[--levelStackTop];
    if (levelBlock != NULL) {
      levelBlock->rowPayload04 = (uint32_t)parentBlock;
      levelBlock->nestedRecordBlockOrParentLink08 = recordCursor;
    }
    if (recordCursor != NULL) {
      recordCursor->recordFlags0C =
           recordCursor->recordFlags0C | UI_TIMED_LIST_RECORD_ENABLES_NESTED_CHILD_TRAVERSAL;
      recordCursor->nestedRecordBlockOrParentLink08 = levelBlock;
    }
    parentBlock = levelBlock;
    recordCursor = levelStack[--levelStackTop];
    levelCount--;
  } while (levelCount != 0);
  result.rootRecordBlockOrError = builtBlock.recordBlockOrError;
  result.selectedRecordOrNull = recordCursor;
  result.failed = false;
  return result;
fail:
  /* As in the original: one pop per level (not per pair), so this frees the top blocks and
     interleaved found-record pointers, not every pushed block. */
  for (; levelCount != 0; levelCount--) {
    g_MemoryApi.free(levelStack[--levelStackTop]);
  }
  result.selectedRecordOrNull = NULL;
  result.rootRecordBlockOrError = builtBlock.recordBlockOrError;
  result.failed = true;
  return result;
}

/* Address: 0x004104B0.
   Frees a record block and every expanded child block below it (collapsing a directory row) and tells in
   CF whether targetRecord (the list's selected row) was one of the freed rows, so the caller can move the
   selection to the collapsed row.
*/
bool UiTimedListTree_FreeRecordBlockRecursiveAndTestContains
          (UiTimedListTreeRecord16 *targetRecord,UiTimedListTreeRecord16 *recordBlock)

{
  UiTimedListTreeRecord16 *recordCursor;
  uint32_t recordsRemaining;
  int containsCount;
  bool childContains;
  
  containsCount = 0;
  if (recordBlock != NULL) {
    recordCursor = recordBlock;
    for (recordsRemaining = recordBlock->recordCountOrRowPayload00; recordsRemaining != 0; recordsRemaining--) {
      if (recordCursor + 1 == targetRecord) {
        containsCount++;
      }
      if ((((recordCursor[1].recordFlags0C & UI_TIMED_LIST_RECORD_OBSERVED_BIT0) != 0) &&
          ((recordCursor[1].recordFlags0C & UI_TIMED_LIST_RECORD_ENABLES_NESTED_CHILD_TRAVERSAL) != 0)) &&
         (childContains = UiTimedListTree_FreeRecordBlockRecursiveAndTestContains
                            (targetRecord,recordCursor[1].nestedRecordBlockOrParentLink08), childContains)) {
        containsCount++;
      }
      recordCursor++;
    }
  }
  g_MemoryApi.free(recordBlock);
  return containsCount != 0;
}

/* Address: 0x00410520.
   Expands a directory row of the tree: builds the full path of the row (its label joined to the labels of
   its ancestor rows, a drive row gives "X:"), enumerates its subdirectories into a new record block and
   links the block below the row (the block header points back to the row and to the row's own block).
   The root "computer" row lists the drives. CF set when the path or the block cannot be built.
*/
bool UiTimedListTree_AttachDirectoryRecordBlock(UiTimedListTreeRecord16 *record)

{
  uint32_t *directory;
  UiTimedListTreeRecord16 *linkedRecord;
  int copyRemaining;
  UiTimedListTreeRecord16 *scanRecord;
  UiTimedListTreeRecord16 *previousRecord;
  uint32_t *recordPathSourceDwords;
  uint32_t *combinedPathSourceDwords;
  uint32_t *recordPathScratchDestDwords;
  uint32_t *combinedPathScratchDestDwords;
  DirectoryRecordBlockResult builtBlock;
  
  recordPathSourceDwords = (uint32_t *)record->recordCountOrRowPayload00;
  recordPathScratchDestDwords = (uint32_t *)&g_UiTimedListRecordPathScratch;
  for (copyRemaining = 0x80; copyRemaining != 0; copyRemaining--) {
    *recordPathScratchDestDwords = *recordPathSourceDwords;
    recordPathSourceDwords++;
    recordPathScratchDestDwords++;
  }
  if (((record[-1].recordFlags0C & UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY) == 0) ||
     (record[-1].rowPayload04 != 0)) {
    linkedRecord = record;
    if (g_UiTimedListRecordPathScratch.codeUnits[1] == ':') {
      g_UiTimedListRecordPathScratch.codeUnits[2] = 0;
      WidePath_CombineDirectoryAndLeaf
                (g_UiTimedListHierarchyParentPathScratch.codeUnits,(uint16_t *)THANDOR_ADDR(g_WildcardAllFilesUtf16,0),
                 g_UiTimedListRecordPathScratch.codeUnits);
    }
    else {
      while( true ) {
        do {
          scanRecord = linkedRecord;
          linkedRecord = scanRecord - 1;
        } while ((scanRecord[-1].recordFlags0C & UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY) == 0);
        linkedRecord = scanRecord[-1].nestedRecordBlockOrParentLink08;
        if (linkedRecord == NULL) {
          return true;
        }
        directory = (uint32_t *)linkedRecord->recordCountOrRowPayload00;
        if (((uint16_t *)directory)[1] == ':') break;
        WidePath_CombineDirectoryAndLeaf
                  (g_UiTimedListCombinedPathScratch.codeUnits,
                   g_UiTimedListRecordPathScratch.codeUnits,(uint16_t *)directory);
        combinedPathSourceDwords = (uint32_t *)&g_UiTimedListCombinedPathScratch;
        combinedPathScratchDestDwords = (uint32_t *)&g_UiTimedListRecordPathScratch;
        for (copyRemaining = 0x80; copyRemaining != 0; copyRemaining--) {
          *combinedPathScratchDestDwords = *combinedPathSourceDwords;
          combinedPathSourceDwords++;
          combinedPathScratchDestDwords++;
        }
      }
      g_UiTimedListCombinedPathScratch.firstTwoCodeUnits = *directory;
      THANDOR_PART(uint32_t, g_UiTimedListCombinedPathScratch, 4) = 0;
      WidePath_CombineDirectoryAndLeaf
                (g_UiTimedListSecondaryPathScratch.codeUnits,
                 g_UiTimedListRecordPathScratch.codeUnits,g_UiTimedListCombinedPathScratch.codeUnits
                );
      WidePath_CombineDirectoryAndLeaf
                (g_UiTimedListHierarchyParentPathScratch.codeUnits,(uint16_t *)THANDOR_ADDR(g_WildcardAllFilesUtf16,0),
                 g_UiTimedListSecondaryPathScratch.codeUnits);
    }
  }
  else {
    g_UiTimedListHierarchyParentPathScratch.firstTwoCodeUnits = 0x3a0061; /* "a:": list the drives */
    THANDOR_PART(uint32_t, g_UiTimedListHierarchyParentPathScratch, 4) = 0;
  }
  builtBlock = UiTimedListTree_BuildDirectoryRecordBlock
                    (g_UiTimedListHierarchyParentPathScratch.codeUnits);
  linkedRecord = builtBlock.recordBlockOrError;
  if (builtBlock.failed) {
    return true;
  }
  record->nestedRecordBlockOrParentLink08 = linkedRecord;
  linkedRecord->nestedRecordBlockOrParentLink08 = record;
  do {
    previousRecord = record - 1;
    scanRecord = record - 1;
    record = previousRecord;
  } while ((scanRecord->recordFlags0C & UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY) == 0);
  linkedRecord->rowPayload04 = (uint32_t)previousRecord;
  return false;
}

/* Address: 0x00410670.
   Expands or collapses a directory row of the tree list (the directory browser's recordSelectionCallback),
   then recomputes the list layout and scrolls the selection into view. Collapsing a branch that contained
   the selected row moves the selection to the collapsed row and queues the list's action. No caller found
   in src/ or image_data.c (only the function map).
*/
void UiTimedListControl_ToggleDirectoryRecordExpansion
          (UiTimedListTreeRecord16 *record,UiTimedListRuntimeExtendedView88 *control)

{
  UiTimedListTreeRecord16 *targetRecord;
  bool cfResult;
  
  targetRecord = UiTimedListControl_GetSelectedRecord(control);
  if ((record->recordFlags0C & UI_TIMED_LIST_RECORD_OBSERVED_BIT0) != 0) {
    if ((record->recordFlags0C & UI_TIMED_LIST_RECORD_ENABLES_NESTED_CHILD_TRAVERSAL) != 0) {
      cfResult = UiTimedListTree_FreeRecordBlockRecursiveAndTestContains
                        (targetRecord,record->nestedRecordBlockOrParentLink08);
      if (cfResult) {
        UiActionQueue_Enqueue((control->base).actionId,control);
        targetRecord = record;
      }
      record->recordFlags0C =
           record->recordFlags0C & ~UI_TIMED_LIST_RECORD_ENABLES_NESTED_CHILD_TRAVERSAL;
      UiTimedListControl_SetRecordTreeAndRecomputeLayout((control->base).recordTree,control);
      UiTimedListControl_SelectRecordAndScrollIntoView(targetRecord,control);
      return;
    }
    cfResult = UiTimedListTree_AttachDirectoryRecordBlock(record);
    if (!cfResult) {
      record->recordFlags0C =
           record->recordFlags0C | UI_TIMED_LIST_RECORD_ENABLES_NESTED_CHILD_TRAVERSAL;
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
bool UiTimedListTree_BuildRecordPath(uint32_t *outputPathDwords,UiTimedListTreeRecord16 *record)

{
  uint32_t *directory;
  int copyRemaining;
  UiTimedListTreeRecord16 *scanRecord;
  uint32_t *recordPathSourceDwords;
  uint32_t *combinedPathSourceDwords;
  uint32_t *recordPathScratchDestDwords;
  uint32_t *combinedPathScratchDestDwords;
  
  recordPathScratchDestDwords = (uint32_t *)&g_UiTimedListRecordPathScratch;
  recordPathSourceDwords = (uint32_t *)record->recordCountOrRowPayload00;
  copyRemaining = 0x80;
  if (((record[-1].recordFlags0C & UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY) == 0) ||
     (record[-1].rowPayload04 != 0)) {
    for (; copyRemaining != 0; copyRemaining--) {
      *recordPathScratchDestDwords = *recordPathSourceDwords;
      recordPathSourceDwords++;
      recordPathScratchDestDwords++;
    }
    if (g_UiTimedListRecordPathScratch.codeUnits[1] == ':') {
      g_UiTimedListRecordPathScratch.codeUnits[2] = 0;
      recordPathSourceDwords = (uint32_t *)&g_UiTimedListRecordPathScratch;
    }
    else {
      while( true ) {
        do {
          scanRecord = record;
          record = scanRecord - 1;
        } while ((scanRecord[-1].recordFlags0C & UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY) == 0);
        record = scanRecord[-1].nestedRecordBlockOrParentLink08;
        if (record == NULL) {
          return true;
        }
        directory = (uint32_t *)record->recordCountOrRowPayload00;
        if (((uint16_t *)directory)[1] == ':') break;
        WidePath_CombineDirectoryAndLeaf
                  (g_UiTimedListCombinedPathScratch.codeUnits,
                   g_UiTimedListRecordPathScratch.codeUnits,(uint16_t *)directory);
        combinedPathSourceDwords = (uint32_t *)&g_UiTimedListCombinedPathScratch;
        combinedPathScratchDestDwords = (uint32_t *)&g_UiTimedListRecordPathScratch;
        for (copyRemaining = 0x80; copyRemaining != 0; copyRemaining--) {
          *combinedPathScratchDestDwords = *combinedPathSourceDwords;
          combinedPathSourceDwords++;
          combinedPathScratchDestDwords++;
        }
      }
      g_UiTimedListCombinedPathScratch.firstTwoCodeUnits = *directory;
      THANDOR_PART(uint32_t, g_UiTimedListCombinedPathScratch, 4) = 0;
      WidePath_CombineDirectoryAndLeaf
                (g_UiTimedListSecondaryPathScratch.codeUnits,
                 g_UiTimedListRecordPathScratch.codeUnits,g_UiTimedListCombinedPathScratch.codeUnits
                );
      recordPathSourceDwords = (uint32_t *)&g_UiTimedListSecondaryPathScratch;
    }
  }
  for (copyRemaining = 0x80; copyRemaining != 0; copyRemaining--) {
    *outputPathDwords = *recordPathSourceDwords;
    recordPathSourceDwords++;
    outputPathDwords++;
  }
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
      g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
    }
    UiActionQueue_Enqueue((control->selectable).actionId,control);
    UiNode_InvalidateRoot(&(control->selectable).base);
    return false;
  }
  if (((control->selectable).stateFlags & UI_SELECTABLE_TOGGLE_ON_ACTIVATION) != 0) {
    if ((((control->selectable).stateFlags & UI_SELECTABLE_PLAY_KEYBOARD_SOUND) != 0) &&
       (control->activationSound != NULL)) {
      g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
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
      g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
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
   keyboard focus if it had it. suppressActionId of g_UiGraphicsAdapterTextButtonVtable, g_UiSpriteButtonControlVtable, g_UiWindowControlVtable,
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
   when no node has it. unsuppressActionId of g_UiGraphicsAdapterTextButtonVtable, g_UiSpriteButtonControlVtable, g_UiWindowControlVtable,
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
   Asks a group of selectable controls (controlCount control pointers follow on the stack) whether none of
   the enabled ones is selected: CF set when none is. Otherwise CF clear with the index (ECX) and node (EAX)
   of the first enabled, selected control.
*/
SelectableGroupNodeResult UiSelectableGroup_NoneVisibleSelected(UiControlCount controlCount,...)

{
  int controlAddress;
  uint32_t controlIndex;
  int controlPointerByteOffset;
  SelectableGroupNodeResult noneSelectedResult;
  SelectableGroupNodeResult selectedResult;
  
  controlPointerByteOffset = 0;
  controlIndex = 0;
  while ((controlAddress = *(int *)((uint8_t *)(&controlCount + 1) + controlPointerByteOffset),
         (((UiSelectableControl *)controlAddress)->base.nodeFlags & UI_NODE_SUPPRESSED) != 0 ||
         ((((UiSelectableControl *)controlAddress)->stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0))) {
    controlIndex++;
    controlPointerByteOffset = controlPointerByteOffset + 4;
    if (controlCount <= controlIndex) {
      noneSelectedResult.node = (UiNodeBase *)controlAddress;
      noneSelectedResult.noneSelected = true;
      return noneSelectedResult;
    }
  }
  selectedResult.controlIndexOrCount = controlIndex;
  selectedResult.node = (UiNodeBase *)controlAddress;
  selectedResult.noneSelected = false;
  return selectedResult;
}


/* Address: 0x004B2D70.
   Asks a group of selectable controls (controlCount control pointers follow on the stack) whether none is
   selected, suppressed ones included: CF set when none is, otherwise CF clear with the index (ECX) of the
   first selected control. Called by the frontend scenario page (src/ui/frontend/scenario.c).
*/
SelectableGroupIndexResult UiSelectableGroup_NoneSelected(UiControlCount controlCount,...)

{
  uint32_t controlIndex;
  int controlPointerByteOffset;
  SelectableGroupIndexResult noneSelectedResult;
  SelectableGroupIndexResult selectedResult;
  
  controlPointerByteOffset = 0;
  controlIndex = 0;
  do {
    if (((*(UiSelectableControl **)((uint8_t *)(&controlCount + 1) + controlPointerByteOffset))->stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
      selectedResult.noneSelected = false;
      selectedResult.selectedIndexOrCount = controlIndex;
      return selectedResult;
    }
    controlIndex++;
    controlPointerByteOffset = controlPointerByteOffset + 4;
  } while (controlIndex < controlCount);
  noneSelectedResult.noneSelected = true;
  return noneSelectedResult;
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
   Looks up the page stack's shown page (its first child) in its page array: returns the page's index with
   CF clear, or CF set (index past the last page) when the shown page is none of the stack's pages.
*/
PageStackSearchResult UiPageStack_ActivePageNotInList(UiPageStackControl *stack)

{
  uint32_t pageIndex;
  bool notFound;
  PageStackSearchResult result;
  
  /* Page 0 is always compared, even when pageCount is 0 (do/while as in the original). */
  pageIndex = 0;
  do {
    notFound = (stack->base).firstChild != (&stack->pages)[pageIndex];
    if (!notFound) break;
    pageIndex++;
  } while (pageIndex < stack->pageCount);
  result.notFound = notFound;
  result.pageIndex = pageIndex;
  return result;
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


/* Address: 0x004B79D0.
   Draws a scroll frame (g_UiScrollableControlVtable drawClipped) from g_UiWindowTextureSource pieces: the
   horizontal and vertical bars (arrows, track and thumb, pressed/active pieces while held), the optional
   frame style and interior fill, then the content child clipped to the remaining view.
*/
void UiScrollableControl_DrawFrameContentAndScrollbars
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiScrollableControl *control)

{
  uint32_t arrowLength;
  GraphicsSubresourceIndex subresource;
  uint32_t tileEnd;
  uint32_t horizontalBarLeft;
  int trackStart;
  int edgeScratch;
  int contentTop;
  int barOffset;
  int contentBottom;
  int contentRight;
  int trackEnd;
  bool accessFailed;
  TextureSizeResult textureSize;
  uint32_t capSize;
  
  accessFailed = g_GraphicsFramebufferBeginAccess();
  if (!accessFailed) {
    tileEnd = 0;
    contentTop = 0;
    contentRight = (control->base).layoutWidth;
    contentBottom = (control->base).layoutHeight;
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
      capSize = 0;
      contentTop = 0;
      edgeScratch = contentRight;
      if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM) == 0) {
        barOffset = 0;
        textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW,g_UiWindowTextureSource);
        arrowLength = textureSize.logicalWidthPixels;
        contentTop = contentTop + textureSize.logicalHeightPixels;
      }
      else {
        trackStart = contentBottom;
        textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW,g_UiWindowTextureSource);
        arrowLength = textureSize.logicalWidthPixels;
        barOffset = contentBottom - textureSize.logicalHeightPixels;
        contentBottom = trackStart - textureSize.logicalHeightPixels;
      }
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW,g_UiWindowTextureSource);
      horizontalBarLeft = tileEnd;
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) != 0) {
        horizontalBarLeft = textureSize.logicalWidthPixels;
      }
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_RIGHT) != 0) {
        contentRight = contentRight - textureSize.logicalWidthPixels;
      }
      if (((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_DECREMENT_ACTIVE) == 0) ||
         ((control->scrollStateFlags & UI_SCROLL_PRIMARY_INTERACTION_ACTIVE) == 0)) {
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,barOffset + (control->base).top,
                   horizontalBarLeft + (control->base).left,UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW,g_UiWindowTextureSource,g_FramebufferAccess);
        tileEnd = capSize;
      }
      else {
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,barOffset + (control->base).top,
                   horizontalBarLeft + (control->base).left,UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW + UI_WINDOW_SUBRESOURCE_PRESSED_OFFSET,g_UiWindowTextureSource,g_FramebufferAccess);
        tileEnd = capSize;
      }
      trackStart = horizontalBarLeft + arrowLength;
      trackEnd = contentRight - arrowLength;
      if (((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_INCREMENT_ACTIVE) == 0) ||
         ((control->scrollStateFlags & UI_SCROLL_PRIMARY_INTERACTION_ACTIVE) == 0)) {
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,barOffset + (control->base).top,
                   trackEnd + (control->base).left,UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW_RIGHT,g_UiWindowTextureSource,g_FramebufferAccess);
        contentRight = edgeScratch;
      }
      else {
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,barOffset + (control->base).top,
                   trackEnd + (control->base).left,UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW_RIGHT + UI_WINDOW_SUBRESOURCE_PRESSED_OFFSET,g_UiWindowTextureSource,g_FramebufferAccess);
        contentRight = edgeScratch;
      }
      if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_TRACK_BEFORE_THUMB_ACTIVE) == 0) {
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,UI_WINDOW_SUBRESOURCE_HORIZONTAL_TRACK,control->horizontalThumbLeft,barOffset,
                   trackStart,control);
      }
      else {
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,UI_WINDOW_SUBRESOURCE_HORIZONTAL_TRACK + UI_WINDOW_SUBRESOURCE_PRESSED_OFFSET,control->horizontalThumbLeft,barOffset,
                   trackStart,control);
      }
      edgeScratch = control->horizontalThumbRight;
      if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_TRACK_AFTER_THUMB_ACTIVE) == 0) {
        if (edgeScratch < clipRight) {
          edgeScratch = clipRight;
        }
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,edgeScratch,UI_WINDOW_SUBRESOURCE_HORIZONTAL_TRACK,trackEnd,barOffset,trackStart,control);
      }
      else {
        if (edgeScratch < clipRight) {
          edgeScratch = clipRight;
        }
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,edgeScratch,UI_WINDOW_SUBRESOURCE_HORIZONTAL_TRACK + UI_WINDOW_SUBRESOURCE_PRESSED_OFFSET,trackEnd,barOffset,trackStart,control);
      }
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_THUMB,g_UiWindowTextureSource);
      capSize = textureSize.logicalWidthPixels;
      edgeScratch = control->horizontalThumbLeft;
      trackStart = control->horizontalThumbRight;
      if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_THUMB_ACTIVE) == 0) {
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,barOffset + (control->base).top,
                   edgeScratch + (control->base).left,UI_WINDOW_SUBRESOURCE_HORIZONTAL_THUMB,g_UiWindowTextureSource,g_FramebufferAccess);
        trackStart = trackStart - capSize;
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,barOffset + (control->base).top,
                   trackStart + (control->base).left,UI_WINDOW_SUBRESOURCE_HORIZONTAL_THUMB_END,g_UiWindowTextureSource,g_FramebufferAccess);
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,UI_WINDOW_SUBRESOURCE_HORIZONTAL_THUMB_MIDDLE,trackStart,barOffset,edgeScratch + capSize,control);
      }
      else {
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,barOffset + (control->base).top,
                   edgeScratch + (control->base).left,UI_WINDOW_SUBRESOURCE_HORIZONTAL_THUMB + UI_WINDOW_SUBRESOURCE_THUMB_ACTIVE_OFFSET,g_UiWindowTextureSource,g_FramebufferAccess);
        trackStart = trackStart - capSize;
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,barOffset + (control->base).top,
                   trackStart + (control->base).left,UI_WINDOW_SUBRESOURCE_HORIZONTAL_THUMB_END + UI_WINDOW_SUBRESOURCE_THUMB_ACTIVE_OFFSET,g_UiWindowTextureSource,g_FramebufferAccess);
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,UI_WINDOW_SUBRESOURCE_HORIZONTAL_THUMB_MIDDLE + UI_WINDOW_SUBRESOURCE_PRESSED_OFFSET,trackStart,barOffset,edgeScratch + capSize,control);
      }
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      edgeScratch = contentBottom;
      barOffset = contentTop;
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_RIGHT) == 0) {
        capSize = tileEnd;
        textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW,g_UiWindowTextureSource);
        arrowLength = textureSize.logicalHeightPixels;
        capSize = capSize + textureSize.logicalWidthPixels;
      }
      else {
        trackStart = contentRight;
        capSize = tileEnd;
        textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW,g_UiWindowTextureSource);
        arrowLength = textureSize.logicalHeightPixels;
        tileEnd = contentRight - textureSize.logicalWidthPixels;
        contentRight = trackStart - textureSize.logicalWidthPixels;
      }
      if (((control->scrollStateFlags & UI_SCROLL_VERTICAL_DECREMENT_ACTIVE) == 0) ||
         ((control->scrollStateFlags & UI_SCROLL_PRIMARY_INTERACTION_ACTIVE) == 0)) {
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,contentTop + (control->base).top,
                   tileEnd + (control->base).left,UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW,g_UiWindowTextureSource,g_FramebufferAccess);
      }
      else {
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,contentTop + (control->base).top,
                   tileEnd + (control->base).left,UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW + UI_WINDOW_SUBRESOURCE_PRESSED_OFFSET,g_UiWindowTextureSource,g_FramebufferAccess);
      }
      trackStart = contentTop + arrowLength;
      trackEnd = contentBottom - arrowLength;
      if (((control->scrollStateFlags & UI_SCROLL_VERTICAL_INCREMENT_ACTIVE) == 0) ||
         ((control->scrollStateFlags & UI_SCROLL_PRIMARY_INTERACTION_ACTIVE) == 0)) {
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,trackEnd + (control->base).top,
                   tileEnd + (control->base).left,UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW_DOWN,g_UiWindowTextureSource,g_FramebufferAccess);
        contentBottom = edgeScratch;
        contentTop = barOffset;
      }
      else {
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,trackEnd + (control->base).top,
                   tileEnd + (control->base).left,UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW_DOWN + UI_WINDOW_SUBRESOURCE_PRESSED_OFFSET,g_UiWindowTextureSource,g_FramebufferAccess);
        contentBottom = edgeScratch;
        contentTop = barOffset;
      }
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_TRACK_BEFORE_THUMB_ACTIVE) == 0) {
        UiWindow_BlitTiledVerticalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,UI_WINDOW_SUBRESOURCE_VERTICAL_TRACK,control->verticalThumbTop,trackStart,
                   tileEnd,control);
      }
      else {
        UiWindow_BlitTiledVerticalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,UI_WINDOW_SUBRESOURCE_VERTICAL_TRACK + UI_WINDOW_SUBRESOURCE_PRESSED_OFFSET,control->verticalThumbTop,trackStart,
                   tileEnd,control);
      }
      edgeScratch = control->verticalThumbBottom;
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_TRACK_AFTER_THUMB_ACTIVE) == 0) {
        if (edgeScratch < clipBottom) {
          edgeScratch = clipBottom;
        }
        UiWindow_BlitTiledVerticalEdge
                  (clipTop,clipLeft,edgeScratch,clipRight,UI_WINDOW_SUBRESOURCE_VERTICAL_TRACK,trackEnd,trackStart,tileEnd,control);
      }
      else {
        if (edgeScratch < clipBottom) {
          edgeScratch = clipBottom;
        }
        UiWindow_BlitTiledVerticalEdge
                  (clipTop,clipLeft,edgeScratch,clipRight,UI_WINDOW_SUBRESOURCE_VERTICAL_TRACK + UI_WINDOW_SUBRESOURCE_PRESSED_OFFSET,trackEnd,trackStart,tileEnd,control);
      }
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_VERTICAL_THUMB,g_UiWindowTextureSource);
      arrowLength = textureSize.logicalHeightPixels;
      edgeScratch = control->verticalThumbTop;
      barOffset = control->verticalThumbBottom;
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_THUMB_ACTIVE) == 0) {
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,edgeScratch + (control->base).top,
                   tileEnd + (control->base).left,UI_WINDOW_SUBRESOURCE_VERTICAL_THUMB,g_UiWindowTextureSource,g_FramebufferAccess);
        barOffset = barOffset - arrowLength;
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,barOffset + (control->base).top,
                   tileEnd + (control->base).left,UI_WINDOW_SUBRESOURCE_VERTICAL_THUMB_END,g_UiWindowTextureSource,g_FramebufferAccess);
        UiWindow_BlitTiledVerticalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,UI_WINDOW_SUBRESOURCE_VERTICAL_THUMB_MIDDLE,barOffset,edgeScratch + arrowLength,tileEnd,control);
        tileEnd = capSize;
      }
      else {
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,edgeScratch + (control->base).top,
                   tileEnd + (control->base).left,UI_WINDOW_SUBRESOURCE_VERTICAL_THUMB + UI_WINDOW_SUBRESOURCE_THUMB_ACTIVE_OFFSET,g_UiWindowTextureSource,g_FramebufferAccess);
        barOffset = barOffset - arrowLength;
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,barOffset + (control->base).top,
                   tileEnd + (control->base).left,UI_WINDOW_SUBRESOURCE_VERTICAL_THUMB_END + UI_WINDOW_SUBRESOURCE_THUMB_ACTIVE_OFFSET,g_UiWindowTextureSource,g_FramebufferAccess);
        UiWindow_BlitTiledVerticalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,UI_WINDOW_SUBRESOURCE_VERTICAL_THUMB_MIDDLE + UI_WINDOW_SUBRESOURCE_PRESSED_OFFSET,barOffset,edgeScratch + arrowLength,tileEnd,control);
        tileEnd = capSize;
      }
    }
    if ((control->scrollStateFlags & UI_SCROLL_FRAME_STYLE_A) != 0) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FRAME_A_FIRST,g_UiWindowTextureSource);
      capSize = textureSize.logicalWidthPixels;
      contentBottom = contentBottom - textureSize.logicalHeightPixels;
      contentRight = contentRight - capSize;
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,contentTop + (control->base).top,
                 tileEnd + (control->base).left,UI_WINDOW_SUBRESOURCE_FRAME_A_FIRST,g_UiWindowTextureSource,g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,contentTop + (control->base).top,
                 contentRight + (control->base).left,UI_WINDOW_SUBRESOURCE_FRAME_A_FIRST + 1,g_UiWindowTextureSource,g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,contentBottom + (control->base).top,
                 tileEnd + (control->base).left,UI_WINDOW_SUBRESOURCE_FRAME_A_FIRST + 2,g_UiWindowTextureSource,g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,contentBottom + (control->base).top,
                 contentRight + (control->base).left,UI_WINDOW_SUBRESOURCE_FRAME_A_FIRST + 3,g_UiWindowTextureSource,g_FramebufferAccess);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,UI_WINDOW_SUBRESOURCE_FRAME_A_FIRST + 4,contentRight,contentTop,tileEnd + capSize,control);
      contentTop = contentTop + textureSize.logicalHeightPixels;
      edgeScratch = (tileEnd + capSize) - capSize;
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,UI_WINDOW_SUBRESOURCE_FRAME_A_FIRST + 5,contentBottom,contentTop,edgeScratch,control);
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,UI_WINDOW_SUBRESOURCE_FRAME_A_FIRST + 6,contentBottom,contentTop,contentRight,control);
      tileEnd = edgeScratch + capSize;
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,UI_WINDOW_SUBRESOURCE_FRAME_A_FIRST + 7,contentRight,contentBottom,tileEnd,control);
    }
    if ((control->scrollStateFlags & UI_SCROLL_FRAME_STYLE_B) != 0) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FRAME_B_FIRST,g_UiWindowTextureSource);
      capSize = textureSize.logicalWidthPixels;
      contentBottom = contentBottom - textureSize.logicalHeightPixels;
      contentRight = contentRight - capSize;
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,contentTop + (control->base).top,
                 tileEnd + (control->base).left,UI_WINDOW_SUBRESOURCE_FRAME_B_FIRST,g_UiWindowTextureSource,g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,contentTop + (control->base).top,
                 contentRight + (control->base).left,UI_WINDOW_SUBRESOURCE_FRAME_B_FIRST + 1,g_UiWindowTextureSource,g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,contentBottom + (control->base).top,
                 tileEnd + (control->base).left,UI_WINDOW_SUBRESOURCE_FRAME_B_FIRST + 2,g_UiWindowTextureSource,g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,contentBottom + (control->base).top,
                 contentRight + (control->base).left,UI_WINDOW_SUBRESOURCE_FRAME_B_FIRST + 3,g_UiWindowTextureSource,g_FramebufferAccess);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,UI_WINDOW_SUBRESOURCE_FRAME_B_FIRST + 4,contentRight,contentTop,tileEnd + capSize,control);
      contentTop = contentTop + textureSize.logicalHeightPixels;
      edgeScratch = (tileEnd + capSize) - capSize;
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,UI_WINDOW_SUBRESOURCE_FRAME_B_FIRST + 5,contentBottom,contentTop,edgeScratch,control);
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,UI_WINDOW_SUBRESOURCE_FRAME_B_FIRST + 6,contentBottom,contentTop,contentRight,control);
      tileEnd = edgeScratch + capSize;
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,UI_WINDOW_SUBRESOURCE_FRAME_B_FIRST + 7,contentRight,contentBottom,tileEnd,control);
    }
    if ((control->scrollStateFlags & (UI_SCROLL_FILL_INTERIOR|UI_SCROLL_FILL_INTERIOR_TEXTURED)) != 0) {
      subresource = 0;
      if ((control->scrollStateFlags & UI_SCROLL_FILL_INTERIOR_TEXTURED) != 0) {
        subresource = UI_WINDOW_SUBRESOURCE_INTERIOR;
      }
      UiWindow_BlitTiledInterior
                (clipTop,clipLeft,clipBottom,clipRight,subresource,contentBottom,contentRight,contentTop,tileEnd,control
                );
    }
    g_GraphicsFramebufferEndAccess();
    edgeScratch = tileEnd + (control->base).left;
    contentTop = contentTop + (control->base).top;
    contentRight = contentRight + (control->base).left;
    contentBottom = contentBottom + (control->base).top;
    if (edgeScratch < clipRight) {
      edgeScratch = clipRight;
    }
    if (contentTop < clipBottom) {
      contentTop = clipBottom;
    }
    if (clipLeft < contentRight) {
      contentRight = clipLeft;
    }
    if (clipTop < contentBottom) {
      contentBottom = clipTop;
    }
    UiContainer_DrawIntersectingChildren(contentBottom,contentRight,contentTop,edgeScratch,&control->base);
  }
  return;
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
  TextureSizeResult textureSize;
  
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
    if ((control->scrollStateFlags & 0x400) != 0) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize(0x6a,g_UiWindowTextureSource);
      control->contentOriginX = control->contentOriginX + textureSize.logicalWidthPixels;
      control->contentOriginY = control->contentOriginY + textureSize.logicalHeightPixels;
      availableWidth = availableWidth + textureSize.logicalWidthPixels * -2;
      availableHeight = availableHeight + textureSize.logicalHeightPixels * -2;
    }
    if ((control->scrollStateFlags & 0x800) != 0) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize(0x72,g_UiWindowTextureSource);
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
       0xffffffcf/0xffffff3f (as in the original) are practically always nonzero; 0x30/0xC0, the "allowed"
       bits, were probably meant. The mask above filters disallowed bars again anyway. */
    if (((control->scrollStateFlags & 0xffffffcf) != 0) &&
       ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0)) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize(0x5e,g_UiWindowTextureSource);
      horizontalExtent = horizontalExtent - textureSize.logicalWidthPixels;
      if (horizontalExtent < 0) {
        control->scrollStateFlags =
             control->scrollStateFlags |
             (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP);
      }
    }
    if (((control->scrollStateFlags & 0xffffff3f) != 0) &&
       ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0)) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize(0x5a,g_UiWindowTextureSource);
      if ((int)(verticalExtent - textureSize.logicalHeightPixels) < 0) {
        if (((control->scrollStateFlags &
             (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) == 0) &&
           ((control->scrollStateFlags & 0xffffffcf) != 0)) {
          textureSize = g_GraphicsTextureSourceGetLogicalSize(0x5e,g_UiWindowTextureSource);
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
      textureSize = g_GraphicsTextureSourceGetLogicalSize(0x5a,g_UiWindowTextureSource);
      control->viewportHeight = control->viewportHeight - textureSize.logicalHeightPixels;
      if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) != 0) {
        control->contentOriginY = control->contentOriginY + textureSize.logicalHeightPixels;
      }
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize(0x5e,g_UiWindowTextureSource);
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
      textureSize = g_GraphicsTextureSourceGetLogicalSize(0x5a,g_UiWindowTextureSource);
      control->verticalThumbTop = control->verticalThumbTop + textureSize.logicalHeightPixels;
      control->verticalThumbBottom = control->verticalThumbBottom + textureSize.logicalHeightPixels;
    }
    if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) != 0) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize(0x5e,g_UiWindowTextureSource);
      control->horizontalThumbLeft = control->horizontalThumbLeft + textureSize.logicalWidthPixels;
      control->horizontalThumbRight = control->horizontalThumbRight + textureSize.logicalWidthPixels;
    }
    horizontalExtent = (control->base).layoutWidth;
    verticalExtent = (control->base).layoutHeight;
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize(0x5a,g_UiWindowTextureSource);
      arrowSize = textureSize.logicalWidthPixels;
      control->horizontalThumbLeft = control->horizontalThumbLeft + arrowSize;
      control->horizontalThumbRight = control->horizontalThumbRight + arrowSize;
      horizontalExtent = horizontalExtent + arrowSize * -2;
      verticalExtent = verticalExtent - textureSize.logicalHeightPixels;
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize(0x5e,g_UiWindowTextureSource);
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
      textureSize = g_GraphicsTextureSourceGetLogicalSize(0xc0,g_UiWindowTextureSource);
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
      textureSize = g_GraphicsTextureSourceGetLogicalSize(0xc2,g_UiWindowTextureSource);
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
  if (((((int)control->contentOriginX <= localPointerX) &&
       ((int)control->contentOriginY <= localPointerY)) &&
      ((int)(localPointerX - control->viewportWidth) < (int)control->contentOriginX)) &&
     (((int)(localPointerY - control->viewportHeight) < (int)control->contentOriginY &&
      (control->scrollStateFlags = control->scrollStateFlags | UI_SCROLL_SECONDARY_PANNING_CONTENT,
      (control->scrollStateFlags &
      (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT|
       UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0)))) {
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
  return;
}


/* Address: 0x004B8BC0.
   Right-button release on a scroll frame (g_UiScrollableControlVtable rightRelease): ends panning, releases
   the pinned cursor and restores the arrow cursor.
*/
void UiScrollableControl_EndSecondaryScrollInteraction
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiScrollableControl *control)

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
   Returns the row pointer array of a pointer list (identical to UiPointerList_GetRowSlotsVariantB). No
   caller found in src/ or image_data.c (only the function map).
*/
void ** UiPointerList_GetRowSlotsVariantA(UiPointerListControl *control)

{
  return control->rowSlots;
}


/* Address: 0x004BA560.
   Returns the index of the selected row of a pointer list. The original also reports
   UI_LIST_SELECTION_CONFIRMED in CF (CLC 0x004BA57F / STC 0x004BA587), which this C signature does not carry
   (both branches return the index). No caller reads it: FrontendNetworkSetupPage_InitializeBackendMode
   (0x0054C29C) passes EAX straight to the backend call, FrontendUiAction200F_Handler (0x0054D4AD) overwrites
   CF with a CMP.
*/
UiListRowIndex UiPointerList_GetSelectedIndexVariantA(UiPointerListControl *control)

{
  UiListRowIndex selectedRowIndex;
  
  selectedRowIndex = (int)control->selectedRowSlot - (int)control->rowSlots >> 2;
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
void UiListControl_DrawRowsAndSelection(int clipTop,int clipLeft,int clipBottom,int clipRight,UiListControl *control)

{
  int columnWidth;
  int rowTop;
  uint8_t *rowRecord;
  uint32_t lastRowIndex;
  int columnX;
  void **lastRowSlot;
  int widthOrColumnCount;
  UiListColumn *column;
  void **rowSlot;
  uint16_t *commandStream;
  bool accessFailed;
  RichTextExtentRegs textExtent;
  TextureSizeResult textureSize;
  
  if (control->rowCount != 0) {
    rowTop = (clipBottom - (control->base).top) / (int)control->rowHeight;
    if (rowTop < 0) {
      rowTop = 0;
    }
    rowSlot = control->rowSlots + rowTop;
    lastRowIndex =
         (uint32_t)(((clipTop - (control->base).top) + (int)control->rowHeight) / (int)control->rowHeight);
    rowTop = rowTop * (int)control->rowHeight;
    if (control->rowCount <= lastRowIndex) {
      lastRowIndex = control->rowCount - 1;
    }
    lastRowSlot = control->rowSlots + (int)lastRowIndex;
    if (rowSlot <= lastRowSlot) {
      accessFailed = g_GraphicsFramebufferBeginAccess();
      if (!accessFailed) {
        do {
          if (rowSlot == control->selectedRowSlot) {
            widthOrColumnCount = (control->base).layoutWidth;
            if (((control->base).nodeFlags & UI_NODE_HAS_KEYBOARD_FOCUS) == 0) {
              UiWindow_BlitTiledHorizontalEdge
                        (clipTop,clipLeft,clipBottom,clipRight,UI_WINDOW_SUBRESOURCE_ROW_HIGHLIGHT,widthOrColumnCount,rowTop,0,control);
            }
            else {
              textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_ROW_FOCUS_LEFT,g_UiWindowTextureSource);
              widthOrColumnCount = widthOrColumnCount - textureSize.logicalWidthPixels;
              UiWindow_BlitTiledHorizontalEdge
                        (clipTop,clipLeft,clipBottom,clipRight,UI_WINDOW_SUBRESOURCE_ROW_FOCUS_MIDDLE,widthOrColumnCount,rowTop,
                         textureSize.logicalWidthPixels,control);
              g_GraphicsTextureSourceBlitSourceAlpha
                        (clipTop,clipLeft,clipBottom,clipRight,rowTop + (control->base).top,
                         (control->base).left,UI_WINDOW_SUBRESOURCE_ROW_FOCUS_LEFT,g_UiWindowTextureSource,g_FramebufferAccess);
              g_GraphicsTextureSourceBlitSourceAlpha
                        (clipTop,clipLeft,clipBottom,clipRight,rowTop + (control->base).top,
                         widthOrColumnCount + (control->base).left,UI_WINDOW_SUBRESOURCE_ROW_FOCUS_RIGHT,g_UiWindowTextureSource,
                         g_FramebufferAccess);
            }
          }
          widthOrColumnCount = control->columnCount;
          rowRecord = (uint8_t *)*rowSlot;
          if (widthOrColumnCount != 0) {
            columnX = 3; /* text inset */
            column = control->columns;
            do {
              columnWidth = column->width;
              if (columnWidth < 0) {
                columnX = columnX - columnWidth;
                commandStream = (uint16_t *)(rowRecord + column->rowTextOffset);
                textExtent = RichTextCommandStream_MeasureRegs(g_UiListTextStyle,commandStream);
                RichTextCommandStream_DrawSingleLine
                          (clipTop,clipLeft,clipBottom,clipRight,g_UiListTextStyle,commandStream,
                           rowTop + 1 + (control->base).top,
                           (columnX - (textExtent.widthPixels + 6)) + (control->base).left);
              }
              else {
                columnX = columnX + columnWidth;
                RichTextCommandStream_DrawSingleLine
                          (clipTop,clipLeft,clipBottom,clipRight,g_UiListTextStyle,
                           (uint16_t *)(rowRecord + column->rowTextOffset),
                           rowTop + 1 + (control->base).top,(columnX - columnWidth) + (control->base).left);
              }
              column++;
              widthOrColumnCount--;
            } while (widthOrColumnCount != 0);
          }
          rowSlot++;
          rowTop = (int)control->rowHeight + rowTop;
        } while (rowSlot <= lastRowSlot);
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
  if (((control->listStateFlags & UI_LIST_DEFERRED_ACTION_PENDING) != 0) &&
     (control->listStateFlags = control->listStateFlags - UI_LIST_COUNTDOWN_ONE,
     (control->listStateFlags & UI_LIST_COUNTDOWN_MASK) == 0)) {
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
  GlyphSizeResult glyphSize;

  glyphSize = FontGlyph_GetLogicalSizeForStyleRegs(g_UiListTextStyle,0);
  computedRowHeight = glyphSize.lineHeight + 1;
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
   Returns the row pointer array of a pointer list (identical to UiPointerList_GetRowSlotsVariantA). No
   caller found in src/ or image_data.c (only the function map).
*/
void ** UiPointerList_GetRowSlotsVariantB(UiPointerListControl *control)

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
void UiTimedListControl_DrawRowsAndSelection(int clipTop,int clipLeft,int clipBottom,int clipRight,UiNodeBase *control)

{
  /* Rewritten from the assembly (0x004BBA00-0x004BBCCC). Expanded records push (record,
     remaining) on the machine stack and descend; the tree connector columns test the saved
     remaining counts of the parent levels. The decompiler kept only one level. Argument
     positions follow the original pushes. */
  enum { TREE_DEPTH_LIMIT = 64 };
  UiTimedListRuntimeExtendedView88 *list = (UiTimedListRuntimeExtendedView88 *)control;
  UiTimedListTreeRecord16 *savedRecord[TREE_DEPTH_LIMIT];
  uint32_t savedRemaining[TREE_DEPTH_LIMIT];
  UiTimedListTreeRecord16 *header;
  UiTimedListTreeRecord16 *record;
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
  if ((header != NULL) && (header->recordCountOrRowPayload00 != 0)) {
    rowTexture = list->base.rowTextureSource;
    remaining = header->recordCountOrRowPayload00;
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
                      (clipTop,clipLeft,clipBottom,clipRight,y,x,list->observedDrawParameter74,
                       rowTexture,g_FramebufferAccess);
          }
          x = x + (int)list->observedDrawParameter80;
        }
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,y,x,
                   (remaining <= 1) ? list->observedDrawParameter7C : list->observedDrawParameter78,
                   rowTexture,g_FramebufferAccess);
        if ((record->recordFlags0C & UI_TIMED_LIST_RECORD_OBSERVED_BIT0) != 0) {
          g_GraphicsTextureSourceBlitSourceAlpha
                    (clipTop,clipLeft,clipBottom,clipRight,y,x,
                     ((record->recordFlags0C & UI_TIMED_LIST_RECORD_ENABLES_NESTED_CHILD_TRAVERSAL) != 0) ? list->observedDrawParameter70 :
                                                          list->base.observedDrawParameter6C,
                     rowTexture,g_FramebufferAccess);
        }
        x = x + (int)list->observedDrawParameter80;
      }
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,y,x,record->rowPayload04,rowTexture,
                 g_FramebufferAccess);
      x = x + (int)list->observedDrawParameter84 - control->left;
      if (record == list->base.selectedRecord) {
        RichTextExtentRegs extent =
             RichTextCommandStream_MeasureRegs(g_UiListTextStyle,(uint16_t *)record->recordCountOrRowPayload00);
        int width = (int)extent.widthPixels + 6;
        if ((control->nodeFlags & UI_NODE_HAS_KEYBOARD_FOCUS) != 0) {
          TextureSizeResult cap =
               g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_ROW_FOCUS_LEFT,g_UiWindowTextureSource);
          int capWidth = (int)cap.logicalWidthPixels;
          int endX = width - capWidth + x;
          UiWindow_BlitTiledHorizontalEdge
                    (clipTop,clipLeft,clipBottom,clipRight,UI_WINDOW_SUBRESOURCE_ROW_FOCUS_MIDDLE,endX,rowY,capWidth + x,control);
          g_GraphicsTextureSourceBlitSourceAlpha
                    (clipTop,clipLeft,clipBottom,clipRight,rowY + control->top,x + control->left,
                     UI_WINDOW_SUBRESOURCE_ROW_FOCUS_LEFT,g_UiWindowTextureSource,g_FramebufferAccess);
          g_GraphicsTextureSourceBlitSourceAlpha
                    (clipTop,clipLeft,clipBottom,clipRight,rowY + control->top,endX + control->left,
                     UI_WINDOW_SUBRESOURCE_ROW_FOCUS_RIGHT,g_UiWindowTextureSource,g_FramebufferAccess);
        }
        else {
          UiWindow_BlitTiledHorizontalEdge
                    (clipTop,clipLeft,clipBottom,clipRight,UI_WINDOW_SUBRESOURCE_ROW_HIGHLIGHT,width + x,rowY,x,control);
        }
      }
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,g_UiListTextStyle,
                 (uint16_t *)record->recordCountOrRowPayload00,rowY + 1 + control->top,
                 x + 3 + control->left);
      rowY = rowY + (int)list->base.rowHeight;
      record++;
      remaining--;
      if (((record[-1].recordFlags0C & UI_TIMED_LIST_RECORD_OBSERVED_BIT0) != 0) &&
          ((record[-1].recordFlags0C & UI_TIMED_LIST_RECORD_ENABLES_NESTED_CHILD_TRAVERSAL) != 0) &&
          (record[-1].nestedRecordBlockOrParentLink08 != NULL) &&
          (depth < TREE_DEPTH_LIMIT)) {
        UiTimedListTreeRecord16 *children = record[-1].nestedRecordBlockOrParentLink08;
        savedRecord[depth] = record;
        savedRemaining[depth] = remaining;
        depth++;
        remaining = children->recordCountOrRowPayload00;
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
  if (((control->listStateAndDelay & UI_TIMED_LIST_ACTION_DELAY_PENDING) != 0) &&
     (control->listStateAndDelay = control->listStateAndDelay - UI_LIST_COUNTDOWN_ONE,
     (control->listStateAndDelay & UI_LIST_COUNTDOWN_MASK) == 0)) {
    control->listStateAndDelay =
         control->listStateAndDelay & (UI_LIST_FLAGS_MASK & ~UI_TIMED_LIST_ACTION_DELAY_PENDING);
    UiActionQueue_Enqueue(control->actionId,control);
  }
  return;
}


/* Address: 0x004BC3F0.
   Returns the selected row of the tree list. Called by UiTimedListControl_ToggleDirectoryRecordExpansion.
*/
UiTimedListTreeRecord16 *
UiTimedListControl_GetSelectedRecord(UiTimedListRuntimeExtendedView88 *control)

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
    control->text = (uint16_t *)((int)control->text + relocationDelta);
    control->labelFlags = control->labelFlags & ~UI_LABEL_TEXT_NEEDS_RELOCATION;
  }
  return;
}


/* Address: 0x00516580.
   Draws a build catalog entry of the in-game command panel (g_UiNodeVtable_00516530 drawClipped): the
   sprite button, its price (runtimeDisplayValueQ4 in whole units, in the alert colour when the active
   faction's xenite does not cover it), how many of this army asset the faction already owns (top left) and
   the highest ownerValue64/ownerValue68 percentage among the faction's armies of this asset in a given
   model state (top right, alert colour when that army has runtimeFlags bit 0). The entry is looked up by its offset in the in-game root in the
   group-42 and group-48 catalog tables.
*/
void UiCatalogEntryControl_DrawClipped
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiCatalogEntryControl *control)

{
  ArmyRuntimeSlot *slotArmyRuntime;
  uint32_t subresourceOrTextLength;
  int recordIndexOrPercent;
  int assetCountOrPercent;
  FactionArmyAssetCount assetSlotIndex;
  int factionIndexOrPercent;
  bool accessFailed;
  RichTextExtentRegs textExtent;
  uint32_t backgroundSubresource;
  GraphicsTextureSourceAsset *spriteTextureSource;
  SoftwareFramebufferAccess *framebuffer;
  PckArmyAssetIdCatalog catalogArmyAssetId;
  UiPackedTextStyle overlayTextStyle;
  ModelRuntimeNode *modelNode;
  ArmyRuntimeSlot *armyRuntime;
  ModelRuntimeNode *modelNodePrimary;
  
  if ((((control->command).sprite.selectable.base.nodeFlags & UI_NODE_SUPPRESSED) != 0) ||
     (((((control->command).sprite.selectable.stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0
       && (((control->command).sprite.selectable.stateFlags & 0x400) != 0)) || /* hidden while not selected */
      (accessFailed = g_GraphicsFramebufferBeginAccess(), accessFailed)))) {
    return;
  }
  spriteTextureSource = (control->command).sprite.primaryTextureSource;
  framebuffer = g_FramebufferAccess;
  if (((control->command).sprite.selectable.stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
    subresourceOrTextLength = (control->command).sprite.normalSubresourceStartOrDescriptor;
  }
  else {
    if ((((control->command).sprite.selectable.stateFlags & UI_SPRITE_BUTTON_ANIMATED) == 0) &&
       (((control->command).sprite.selectable.stateFlags & 0x800) != 0)) {
      spriteTextureSource = (control->command).sprite.alternateTextureSource;
    }
    subresourceOrTextLength = (control->command).sprite.selectedSubresourceStart;
    if (((control->command).sprite.selectable.stateFlags & 0x40) != 0) { /* normal frame drawn underneath */
      backgroundSubresource = (control->command).sprite.normalSubresourceStartOrDescriptor;
      if (((control->command).sprite.selectable.stateFlags & UI_SPRITE_BUTTON_ANIMATED) != 0) {
        backgroundSubresource = backgroundSubresource + (control->command).sprite.animationFrameOffset;
      }
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,(control->command).sprite.selectable.base.top
                 ,(control->command).sprite.selectable.base.left,backgroundSubresource,
                 (control->command).sprite.primaryTextureSource,g_FramebufferAccess);
    }
  }
  if (((control->command).sprite.selectable.stateFlags & UI_SPRITE_BUTTON_ANIMATED) != 0) {
    subresourceOrTextLength = subresourceOrTextLength + (control->command).sprite.animationFrameOffset;
  }
  g_GraphicsTextureSourceBlitSourceAlpha
            (clipTop,clipLeft,clipBottom,clipRight,(control->command).sprite.selectable.base.top,
             (control->command).sprite.selectable.base.left,subresourceOrTextLength,spriteTextureSource,framebuffer);
  factionIndexOrPercent = (g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex;
  if ((int)g_GameFactionRuntimeImage.records[factionIndexOrPercent].xeniteCurrentQ4 <
      (int)control->runtimeDisplayValueQ4) {
    overlayTextStyle = UI_CATALOG_TEXT_STYLE_ALERT;
  }
  else {
    overlayTextStyle = UI_CATALOG_TEXT_STYLE_NORMAL;
  }
  g_UiCatalogEntryRichTextScratchUtf16[0] = ' ';
  g_UiCatalogEntryRichTextScratchUtf16[1] = 0;
  subresourceOrTextLength = g_WideNumberFormatUtf16
                    (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,control->runtimeDisplayValueQ4 >> 4,
                     g_UiCatalogEntryRichTextScratchUtf16 + 1);
  *(uint32_t *)((int)g_UiCatalogEntryRichTextScratchUtf16 + subresourceOrTextLength + 2) = ' '; /* ' ' and the terminator */
  textExtent = RichTextCommandStream_MeasureRegs(UI_CATALOG_TEXT_STYLE_MEASURE,g_UiCatalogEntryRichTextScratchUtf16);
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,overlayTextStyle,g_UiCatalogEntryRichTextScratchUtf16,
             ((control->command).sprite.selectable.base.bottom - textExtent.heightPixels) - 2,
             ((int)((control->command).sprite.selectable.base.layoutWidth - textExtent.widthPixels) >> 1)
             + (control->command).sprite.selectable.base.left);
  recordIndexOrPercent = 42 - 1; /* the last group-42 record */
  do {
    if ((int)control - (int)g_InGameRuntimeRoot ==
        g_UiCatalogGroup42OffsetTables[g_UiCatalogGroup42ColumnCount][recordIndexOrPercent]) {
      assetCountOrPercent = 0;
      catalogArmyAssetId = g_UiCatalogGroup42Records[recordIndexOrPercent]->armyAssetId;
      for (assetSlotIndex = g_GameFactionRuntimeImage.records[factionIndexOrPercent].secondaryArmyAssetCount; assetSlotIndex != 0;
          assetSlotIndex--) {
        if (g_UiCatalogGroup42Records[recordIndexOrPercent] ==
            *(UiCommandRuntimeRecordPrefix **)(factionIndexOrPercent * 0x740 + THANDOR_ADDR(g_GameFactionRuntimeImage,0xdc) + assetSlotIndex * 4)) {
          assetCountOrPercent++;
        }
      }
      if (assetCountOrPercent != 0) {
        g_UiCatalogEntryRichTextScratchUtf16[0] = ' ';
        subresourceOrTextLength = g_WideNumberFormatUtf16
                          (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,assetCountOrPercent,
                           g_UiCatalogEntryRichTextScratchUtf16 + 1);
        factionIndexOrPercent = (control->command).sprite.selectable.base.top;
        *(uint32_t *)((int)g_UiCatalogEntryRichTextScratchUtf16 + subresourceOrTextLength + 2) = ' '; /* ' ' and the terminator */
        RichTextCommandStream_DrawSingleLine
                  (clipTop,clipLeft,clipBottom,clipRight,overlayTextStyle,
                   g_UiCatalogEntryRichTextScratchUtf16,factionIndexOrPercent + 2,
                   (control->command).sprite.selectable.base.left);
      }
      factionIndexOrPercent = -1;
      for (modelNodePrimary =
                (ModelRuntimeNode *)(g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
          modelNodePrimary != NULL;
          modelNodePrimary = (ModelRuntimeNode *)(modelNodePrimary->common).nextNode) {
        if (((modelNodePrimary->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
            (armyRuntime = (modelNodePrimary->runtimePayload).armyRuntime,
            ((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C == 0xb))
           && (((armyRuntime->articulatedContact).fallbackPosition0Q12 == 1 &&
               (((((g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex ==
                   (armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex &&
                  (catalogArmyAssetId == armyRuntime->classState60)) &&
                 (recordIndexOrPercent = (int)(((int64_t)(int)armyRuntime->ownerValue64 * 100) /
                               (int64_t)(int)armyRuntime->ownerValue68), factionIndexOrPercent <= recordIndexOrPercent)) &&
                (overlayTextStyle = UI_CATALOG_TEXT_STYLE_NORMAL, factionIndexOrPercent = recordIndexOrPercent, (armyRuntime->runtimeFlags & 1) != 0)))))) {
          overlayTextStyle = UI_CATALOG_TEXT_STYLE_ALERT;
        }
      }
      if (-1 < factionIndexOrPercent) {
        g_UiCatalogEntryRichTextScratchUtf16[0] = ' ';
        subresourceOrTextLength = g_WideNumberFormatUtf16
                          (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,factionIndexOrPercent,
                           g_UiCatalogEntryRichTextScratchUtf16 + 1);
        *(uint16_t *)((int)g_UiCatalogEntryRichTextScratchUtf16 + subresourceOrTextLength + 2) = '%';
        *(uint32_t *)((int)g_UiCatalogEntryRichTextScratchUtf16 + subresourceOrTextLength + 4) = ' '; /* ' ' and the terminator */
        textExtent = RichTextCommandStream_MeasureRegs(UI_CATALOG_TEXT_STYLE_MEASURE,g_UiCatalogEntryRichTextScratchUtf16);
        RichTextCommandStream_DrawSingleLine
                  (clipTop,clipLeft,clipBottom,clipRight,overlayTextStyle,
                   g_UiCatalogEntryRichTextScratchUtf16,
                   (control->command).sprite.selectable.base.top + 2,
                   (control->command).sprite.selectable.base.right - textExtent.widthPixels);
      }
      g_GraphicsFramebufferEndAccess();
      return;
    }
    recordIndexOrPercent--;
  } while (-1 < recordIndexOrPercent);
  recordIndexOrPercent = 48 - 1; /* the last group-48 record */
  while ((int)control - (int)g_InGameRuntimeRoot !=
         g_UiCatalogGroup48OffsetTables[g_UiCatalogGroup48ColumnCount][recordIndexOrPercent]) {
    recordIndexOrPercent--;
    if (recordIndexOrPercent < 0) {
      g_GraphicsFramebufferEndAccess();
      return;
    }
  }
  assetCountOrPercent = 0;
  catalogArmyAssetId = g_UiCatalogGroup48Records[recordIndexOrPercent]->armyAssetId;
  for (assetSlotIndex = g_GameFactionRuntimeImage.records[factionIndexOrPercent].secondaryArmyAssetCount; assetSlotIndex != 0;
      assetSlotIndex--) {
    if (g_UiCatalogGroup48Records[recordIndexOrPercent] ==
        *(UiCommandRuntimeRecordPrefix **)(factionIndexOrPercent * 0x740 + THANDOR_ADDR(g_GameFactionRuntimeImage,0xdc) + assetSlotIndex * 4)) {
      assetCountOrPercent++;
    }
  }
  if (assetCountOrPercent != 0) {
    g_UiCatalogEntryRichTextScratchUtf16[0] = ' ';
    subresourceOrTextLength = g_WideNumberFormatUtf16
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,assetCountOrPercent,
                       g_UiCatalogEntryRichTextScratchUtf16 + 1);
    factionIndexOrPercent = (control->command).sprite.selectable.base.top;
    *(uint32_t *)((int)g_UiCatalogEntryRichTextScratchUtf16 + subresourceOrTextLength + 2) = ' '; /* ' ' and the terminator */
    RichTextCommandStream_DrawSingleLine
              (clipTop,clipLeft,clipBottom,clipRight,overlayTextStyle,g_UiCatalogEntryRichTextScratchUtf16,
               factionIndexOrPercent + 2,(control->command).sprite.selectable.base.left);
  }
  factionIndexOrPercent = (g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex;
  recordIndexOrPercent = -1;
  for (modelNode = (ModelRuntimeNode *)(g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
      modelNode != NULL;
      modelNode = (ModelRuntimeNode *)(modelNode->common).nextNode) {
    if (modelNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
      slotArmyRuntime = (modelNode->runtimePayload).armyRuntime;
      if (((slotArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C == 0x16) {
        if ((((slotArmyRuntime->articulatedContact).terrainContactMode ==
              ARMY_TERRAIN_CONTACT_ADVANCE_ACTIVE_CONTACT_AND_RELEASE) &&
            (factionIndexOrPercent == (slotArmyRuntime->linkedEntityRuntime->common).ownership.ownerIndex)) &&
           ((catalogArmyAssetId == slotArmyRuntime->classState60 &&
            ((assetCountOrPercent = (int)(((int64_t)(int)slotArmyRuntime->ownerValue64 * 100) /
                           (int64_t)(int)slotArmyRuntime->ownerValue68), recordIndexOrPercent <= assetCountOrPercent &&
             (overlayTextStyle = UI_CATALOG_TEXT_STYLE_NORMAL, recordIndexOrPercent = assetCountOrPercent, (slotArmyRuntime->runtimeFlags & 1) != 0)))))) {
          overlayTextStyle = UI_CATALOG_TEXT_STYLE_ALERT;
        }
      }
      else if ((((((slotArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C == 0xd)
                && ((slotArmyRuntime->articulatedContact).fallbackPosition0Q12 == 1)) &&
               (factionIndexOrPercent == (slotArmyRuntime->linkedEntityRuntime->common).ownership.ownerIndex)) &&
              (((catalogArmyAssetId == slotArmyRuntime->classState60 &&
                (assetCountOrPercent = (int)(((int64_t)(int)slotArmyRuntime->ownerValue64 * 100) /
                              (int64_t)(int)slotArmyRuntime->ownerValue68), recordIndexOrPercent <= assetCountOrPercent)) &&
               (overlayTextStyle = UI_CATALOG_TEXT_STYLE_NORMAL, recordIndexOrPercent = assetCountOrPercent, (slotArmyRuntime->runtimeFlags & 1) != 0)))) {
        overlayTextStyle = UI_CATALOG_TEXT_STYLE_ALERT;
      }
    }
  }
  if (-1 < recordIndexOrPercent) {
    g_UiCatalogEntryRichTextScratchUtf16[0] = ' ';
    subresourceOrTextLength = g_WideNumberFormatUtf16
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,recordIndexOrPercent,
                       g_UiCatalogEntryRichTextScratchUtf16 + 1);
    *(uint16_t *)((int)g_UiCatalogEntryRichTextScratchUtf16 + subresourceOrTextLength + 2) = '%';
    *(uint32_t *)((int)g_UiCatalogEntryRichTextScratchUtf16 + subresourceOrTextLength + 4) = ' '; /* ' ' and the terminator */
    textExtent = RichTextCommandStream_MeasureRegs(UI_CATALOG_TEXT_STYLE_MEASURE,g_UiCatalogEntryRichTextScratchUtf16);
    RichTextCommandStream_DrawSingleLine
              (clipTop,clipLeft,clipBottom,clipRight,overlayTextStyle,g_UiCatalogEntryRichTextScratchUtf16,
               (control->command).sprite.selectable.base.top + 2,
               (control->command).sprite.selectable.base.right - textExtent.widthPixels);
  }
  g_GraphicsFramebufferEndAccess();
  return;
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
      if ((int)control - (int)g_InGameRuntimeRoot ==
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
        if ((int)control - (int)g_InGameRuntimeRoot ==
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
    if ((((control->command).sprite.selectable.stateFlags & 0x200) != 0) && /* sound on release */
       ((control->command).sprite.activationSoundId != 0)) {
      g_SoundPlayOneShot
                (g_UiSoundGainQ15,g_UiSoundGainQ15,
                 (DirectSoundVoiceSet *)(control->command).sprite.activationSoundId);
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
          (UiTimedListTreeRecord16 *selectedRecord,UiTimedListRuntimeExtendedView88 *control)

{
  uint32_t nestedRecordCount;
  int rowAccumulator;
  int rowIndex;
  UiTimedListTreeRecord16 *previousRecord;
  
  rowAccumulator = 0;
  (control->base).selectedRecord = selectedRecord;
  do {
    previousRecord = selectedRecord - 1;
    rowIndex = rowAccumulator;
    if ((selectedRecord[-1].recordFlags0C & UI_TIMED_LIST_RECORD_ENABLES_NESTED_CHILD_TRAVERSAL) !=
        0) {
      nestedRecordCount = UiTimedListTree_CountRecordArrayAndNestedChildren
                        (selectedRecord[-1].nestedRecordBlockOrParentLink08);
      rowIndex = rowAccumulator + nestedRecordCount;
    }
    selectedRecord = previousRecord;
    rowAccumulator = rowIndex + 1;
  } while (((selectedRecord->recordFlags0C & UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY) == 0) ||
          (selectedRecord = selectedRecord->nestedRecordBlockOrParentLink08,
          selectedRecord != NULL));
  rowIndex = rowIndex * (control->base).rowHeight;
  UiScrollableControl_ClampOffsetsToViewport
            (rowIndex + 1 + (control->base).rowHeight,(control->base).base.rightOffset,rowIndex,0,
             (UiScrollableControl *)(control->base).base.parent);
  return;
}


/* Address: 0x004BB4E0.
   Selects row index of a pointer list (without queueing its action) and scrolls the list's scrollable
   parent so the row is visible; the same as UiPointerList_SelectIndexVariantA. Out-of-range indices are
   ignored.
*/
void UiPointerList_SelectIndexVariantB(UiListRowIndex index,UiPointerListControl *control)

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
   Returns the index of the selected row of a pointer list; CF (confirmed) is set when the selection was
   confirmed (UI_LIST_SELECTION_CONFIRMED, set by a double click on the row).
*/
ListSelectionResult UiPointerList_GetSelectedIndexVariantB(UiPointerListControl *control)

{
  UiListRowIndex selectedRowIndex;
  ListSelectionResult unconfirmedResult;
  ListSelectionResult confirmedResult;
  
  selectedRowIndex = ((int)control->selectedRowSlot - (int)control->rowSlots) >> 2;
  if ((control->listStateFlags & UI_LIST_SELECTION_CONFIRMED) == 0) {
    unconfirmedResult.confirmed = false;
    unconfirmedResult.rowIndex = selectedRowIndex;
    return unconfirmedResult;
  }
  confirmedResult.confirmed = true;
  confirmedResult.rowIndex = selectedRowIndex;
  return confirmedResult;
}


/* Address: 0x004B9460.
   Returns the size of the view of a scroll frame (viewportWidth in EAX, viewportHeight in EDX), or 0/0 when
   control is not a scroll frame. The lists use it on their parent to page by a view's height.
*/
UiScrollableContentDimensionsEdxEax8
UiScrollableControl_QueryContentSizeRegs(UiScrollableControl *control)

{
  UiPixelExtent contentWidth;
  UiPixelExtent contentHeight;
  
  contentWidth = 0;
  contentHeight = 0;
  if ((control->base).vtable == &g_UiScrollableControlVtable) {
    contentWidth = control->viewportWidth;
    contentHeight = control->viewportHeight;
  }
  /* EDX:EAX = height:width */
  return ((UiScrollableContentDimensionsEdxEax8)contentHeight << 32) |
         (UiScrollableContentDimensionsEdxEax8)contentWidth;
}

/* Address: 0x004BC1C0.
   Gives the tree list a new record tree (or NULL) and selects its first row: row height from the list font,
   row count over all expanded blocks, width from the widest indented row label plus icon; then the parent
   scroll frame is laid out again for the new content size.
*/
void UiTimedListControl_SetRecordTreeAndRecomputeLayout
          (UiTimedListTreeRecord16 *recordTree,UiTimedListRuntimeExtendedView88 *control)

{
  /* Rewritten from the assembly (0x004BC1C0): expanded records with children push their position
     on the machine stack and descend; the decompiler kept only one level. */
  enum { TREE_DEPTH_LIMIT = 64 };
  UiTimedListTreeRecord16 *savedRecord[TREE_DEPTH_LIMIT];
  uint32_t savedRemaining[TREE_DEPTH_LIMIT];
  UiTimedListTreeRecord16 *record;
  UiNodeBase *parent;
  GlyphSizeResult glyph;
  uint32_t remaining;
  uint32_t widest;
  uint32_t width;
  int depth;

  glyph = FontGlyph_GetLogicalSizeActiveRegs(0);
  remaining = (recordTree == NULL) ? 0 : recordTree->recordCountOrRowPayload00;
  (control->base).rowHeight = glyph.lineHeight + 1;
  (control->base).rowCount = remaining;
  (control->base).recordTree = recordTree;
  record = recordTree + 1;
  (control->base).selectedRecord = record;
  widest = 0;
  depth = 0;
  while (remaining != 0) {
    RichTextExtentRegs extent =
         RichTextCommandStream_MeasureRegs(g_UiListTextStyle,(uint16_t *)record->recordCountOrRowPayload00);
    width = extent.widthPixels + control->observedDrawParameter84 +
            control->observedDrawParameter80 * (uint32_t)depth;
    record++;
    remaining--;
    if (widest < width) {
      widest = width;
    }
    if (((record[-1].recordFlags0C & UI_TIMED_LIST_RECORD_OBSERVED_BIT0) != 0) &&
        ((record[-1].recordFlags0C & UI_TIMED_LIST_RECORD_ENABLES_NESTED_CHILD_TRAVERSAL) != 0) &&
        (record[-1].nestedRecordBlockOrParentLink08 != NULL) &&
        (depth < TREE_DEPTH_LIMIT)) {
      UiTimedListTreeRecord16 *children = record[-1].nestedRecordBlockOrParentLink08;
      savedRecord[depth] = record;
      savedRemaining[depth] = remaining;
      depth++;
      remaining = children->recordCountOrRowPayload00;
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
UiTimedListTreeRecord16 * UiTimedListControl_GetRecordTree(UiTimedListControl *control)

{
  return control->recordTree;
}

/* Address: 0x004BC310.
   Counts the visible rows of a record block: its rows plus, recursively, the rows of every expanded child
   block (0 for NULL). Used to turn a row position into a row index for scrolling.
*/
uint32_t UiTimedListTree_CountRecordArrayAndNestedChildren(UiTimedListTreeRecord16 *recordBlock)

{
  uint32_t nestedRecordCount;
  uint32_t totalCount;
  uint32_t recordsRemaining;
  
  totalCount = 0;
  if (recordBlock != NULL) {
    totalCount = recordBlock->recordCountOrRowPayload00;
    recordsRemaining = totalCount;
    do {
      if ((recordBlock[1].recordFlags0C & UI_TIMED_LIST_RECORD_ENABLES_NESTED_CHILD_TRAVERSAL) != 0)
      {
        nestedRecordCount = UiTimedListTree_CountRecordArrayAndNestedChildren
                          (recordBlock[1].nestedRecordBlockOrParentLink08);
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
  TextureSizeResult textureSize;

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
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW,g_UiWindowTextureSource);
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
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW,g_UiWindowTextureSource);
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
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_HORIZONTAL_THUMB,g_UiWindowTextureSource);
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
  int viewLeftOrOverflow;
  int viewTop;
  int viewBottom;
  int viewRight;
  int horizontalOverflow;
  
  if ((control->base).vtable == &g_UiScrollableControlVtable) {
    /* the visible content rectangle; the scroll offsets are the negated view position */
    viewLeftOrOverflow = -control->scrollOffsetX;
    viewTop = -control->scrollOffsetY;
    changeCount = 0;
    viewRight = control->viewportWidth + viewLeftOrOverflow;
    viewBottom = control->viewportHeight + viewTop;
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
      horizontalOverflow = viewRight - targetRight;
      changeCount = viewRight < targetRight;
      if ((bool)changeCount) {
        control->scrollOffsetX = control->scrollOffsetX + horizontalOverflow;
        viewLeftOrOverflow = viewLeftOrOverflow - horizontalOverflow;
      }
      if (viewLeftOrOverflow - targetLeft != 0 && targetLeft <= viewLeftOrOverflow) {
        changeCount++;
        control->scrollOffsetX = control->scrollOffsetX + (viewLeftOrOverflow - targetLeft);
      }
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      viewLeftOrOverflow = viewBottom - targetBottom;
      if (viewBottom < targetBottom) {
        control->scrollOffsetY = control->scrollOffsetY + viewLeftOrOverflow;
        viewTop = viewTop - viewLeftOrOverflow;
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

