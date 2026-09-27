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
   Ownership: ui/controls/lists.
   Purpose: Handles ui timed list control handle keyboard navigation carry-flag result.
   Local calls: UiScrollableControl_QueryContentSizeRegs, UiTimedListTree_CountRecordArrayAndNestedChildren,
   UiScrollableControl_ClampOffsetsToViewport.
   Cross-module calls: UiNode_DefaultKeyboardEventMoveFocusNextCf [ui/controls/input].
*/
bool __thandor_cf_preserve_eax_ecx_edx
UiTimedListControl_HandleKeyboardNavigationCf
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
  dword nestedRecordCount;
  UiTimedListTreeRecord16 *lastRecord;
  uint siblingIndex;
  int rowAccumulator;
  int rowIndex;
  int stepCounter;
  bool handled;
  UiScrollableContentDimensionsEdxEax8 contentSize;

  /* A record block is a header record (count, parent block, parent record, ANCESTOR_BOUNDARY flag)
     followed by count row records; an expanded row (flags 1|2) links its child block. */
  stepCounter = 1;
  previousSelection = control->selectedRecord;
  if ((keyCode & 0xffff0000) == 0) {
UiTimedListKeyboard_DelegateUnhandledEvent:
    handled = UiNode_DefaultKeyboardEventMoveFocusNextCf(keyboardStateMask,keyCode,&control->base);
    return handled;
  }
  if (keyCode == 0x10010) {
    control->selectedRecord = control->recordTree + 1;
    goto UiTimedListKeyboard_CommitSelectionAndScheduleAction;
  }
  if (keyCode == 0x10018) {
    childBlock = control->recordTree;
    do {
      lastRecord = childBlock + (int)childBlock->recordCountOrRowPayload00;
      control->selectedRecord = lastRecord;
      if (((lastRecord->recordFlags0C & 1) == 0) || ((lastRecord->recordFlags0C & 2) == 0)) break;
      childBlock = lastRecord->nestedRecordBlockOrParentLink08;
    } while (childBlock != (UiTimedListTreeRecord16 *)0x0);
    goto UiTimedListKeyboard_CommitSelectionAndScheduleAction;
  }
  if (keyCode == 0x10012) {
    contentSize = UiScrollableControl_QueryContentSizeRegs((UiScrollableControl *)(control->base).parent);
    rowAccumulator = (int)((contentSize >> 0x20) / ZEXT48(control->rowHeight));
    stepCounter = rowAccumulator + -1;
    if (rowAccumulator < 1) {
      stepCounter = 1;
    }
UiTimedListKeyboard_MoveSelectionBackwardLoop:
    do {
      stepRecord = control->selectedRecord;
      recordCursor = stepRecord - 1;
      control->selectedRecord = recordCursor;
      if ((stepRecord[-1].recordFlags0C & UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY) == 0) {
        while ((((recordCursor->recordFlags0C & 1U) != 0 && ((recordCursor->recordFlags0C & 2U) != 0)) &&
               (recordCursor = recordCursor->nestedRecordBlockOrParentLink08,
               recordCursor != (UiTimedListTreeRecord16 *)0x0))) {
          recordCursor = recordCursor + (int)recordCursor->recordCountOrRowPayload00;
          control->selectedRecord = recordCursor;
        }
      }
      else {
        recordCursor = stepRecord[-1].nestedRecordBlockOrParentLink08;
        control->selectedRecord = stepRecord;
        if (recordCursor != (UiTimedListTreeRecord16 *)0x0) {
          control->selectedRecord = recordCursor;
        }
      }
      stepCounter = stepCounter + -1;
    } while (stepCounter != 0);
  }
  else {
    if (keyCode == 0x1001a) {
      contentSize = UiScrollableControl_QueryContentSizeRegs((UiScrollableControl *)(control->base).parent);
      rowAccumulator = (int)((contentSize >> 0x20) / ZEXT48(control->rowHeight));
      stepCounter = rowAccumulator + -1;
      if (rowAccumulator < 1) {
        stepCounter = 1;
      }
    }
    else {
      if (keyCode == 0x10011) goto UiTimedListKeyboard_MoveSelectionBackwardLoop;
      if (keyCode != 0x10019) {
        if (keyCode == 0x10014) {
          if ((previousSelection->recordFlags0C & 1) == 0) {
            return false;
          }
          if ((previousSelection->recordFlags0C & 2) == 0) {
            return false;
          }
          if (control->recordSelectionCallback == 0) {
            return false;
          }
          (*control->recordSelectionCallback)
                    (previousSelection,(UiTimedListRuntimeExtendedView88 *)control);
          return false;
        }
        if (keyCode == 0x10016) {
          if ((previousSelection->recordFlags0C & 1) == 0) {
            return false;
          }
          if ((previousSelection->recordFlags0C & 2) != 0) {
            return false;
          }
          if (control->recordSelectionCallback == 0) {
            return false;
          }
          (*control->recordSelectionCallback)
                    (previousSelection,(UiTimedListRuntimeExtendedView88 *)control);
          return false;
        }
        goto UiTimedListKeyboard_DelegateUnhandledEvent;
      }
    }
    do {
      startRecord = control->selectedRecord;
      siblingIndex = 0;
      scanRecord = startRecord;
      if ((((startRecord->recordFlags0C & 1) == 0) || ((startRecord->recordFlags0C & 2) == 0)) ||
         ((recordCursor = startRecord->nestedRecordBlockOrParentLink08,
          recordCursor == (UiTimedListTreeRecord16 *)0x0 || (recordCursor->recordCountOrRowPayload00 == 0))))
      {
        do {
          do {
            stepRecord = scanRecord;
            siblingIndex = siblingIndex + 1;
            scanRecord = stepRecord - 1;
          } while ((stepRecord[-1].recordFlags0C & UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY) == 0);
          control->selectedRecord = control->selectedRecord + 1;
          if (siblingIndex < stepRecord[-1].recordCountOrRowPayload00)
          goto UiTimedListKeyboard_ForwardTraversalStepComplete;
          scanRecord = stepRecord[-1].nestedRecordBlockOrParentLink08;
          siblingIndex = 0;
          control->selectedRecord = scanRecord;
        } while (scanRecord != (UiTimedListTreeRecord16 *)0x0);
        control->selectedRecord = startRecord;
      }
      else {
        control->selectedRecord = recordCursor + 1;
      }
UiTimedListKeyboard_ForwardTraversalStepComplete:
      stepCounter = stepCounter + -1;
    } while (stepCounter != 0);
  }
UiTimedListKeyboard_CommitSelectionAndScheduleAction:
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
            countRecord != (UiTimedListTreeRecord16 *)0x0));
    rowIndex = rowIndex * (int)control->rowHeight;
    UiScrollableControl_ClampOffsetsToViewport
              ((int)control->rowHeight + rowIndex + 1,(control->base).rightOffset,rowIndex,0,
               (UiScrollableControl *)(control->base).parent);
    actionDelayFrames = g_UiTimedListActionDelayFrames;
    control->listStateAndDelay = control->listStateAndDelay | UI_TIMED_LIST_ACTION_DELAY_PENDING;
    control->listStateAndDelay = control->listStateAndDelay & 0xffffff;
    control->listStateAndDelay = control->listStateAndDelay | actionDelayFrames << 0x18;
  }
  return false;
}


/* Address: 0x004BB100.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004BA590[12]@004BA590.
   Local calls: UiScrollableControl_QueryContentSizeRegs, UiScrollableControl_ClampOffsetsToViewport.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime], UiNode_DefaultKeyboardEventMoveFocusNextCf
   [ui/controls/input].
*/
bool __thandor_cf_preserve_eax_ecx_edx
UiListControl_HandleKeyboardNavigationCf
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiListControl *control)

{
  void **previousSelectedSlot;
  void **newSelectedSlot;
  int rowValue;
  uint targetRowIndex;
  bool handled;
  UiScrollableContentDimensionsEdxEax8 contentSize;
  
  previousSelectedSlot = control->selectedRowSlot;
  if ((keyCode & 0xffff0000) != 0) {
    if (keyCode == 0x10001) {
      control->listStateFlags = control->listStateFlags | UI_LIST_SELECTION_CONFIRMED;
      UiActionQueue_Enqueue(control->actionId,control);
    }
    else if (keyCode == 0x10010) {
      control->selectedRowSlot = control->rowSlots;
    }
    else if (keyCode == 0x10018) {
      control->selectedRowSlot = control->rowSlots + (control->rowCount - 1);
    }
    else if (keyCode == 0x10012) {
      contentSize = UiScrollableControl_QueryContentSizeRegs
                        ((UiScrollableControl *)(control->base).parent);
      rowValue = ((uint)((int)control->selectedRowSlot - (int)control->rowSlots) >> 2) -
              ((int)((contentSize >> 0x20) / (ulonglong)control->rowHeight) + -1);
      if (rowValue < 0) {
        rowValue = 0;
      }
      control->selectedRowSlot = control->rowSlots + rowValue;
    }
    else if (keyCode == 0x1001a) {
      contentSize = UiScrollableControl_QueryContentSizeRegs
                        ((UiScrollableControl *)(control->base).parent);
      targetRowIndex = ((uint)((int)control->selectedRowSlot - (int)control->rowSlots) >> 2) +
              (int)((contentSize >> 0x20) / (ulonglong)control->rowHeight) + -1;
      if (control->rowCount <= targetRowIndex) {
        targetRowIndex = control->rowCount - 1;
      }
      control->selectedRowSlot = control->rowSlots + targetRowIndex;
    }
    else if (keyCode == 0x10011) {
      if (control->rowSlots <= control->selectedRowSlot + -1) {
        control->selectedRowSlot = control->selectedRowSlot + -1;
      }
    }
    else {
      if (keyCode != 0x10019) goto UiListControl_DelegateUnhandledKeyboardEvent;
      if (((uint)((int)control->selectedRowSlot - (int)control->rowSlots) >> 2) + 1 <
          control->rowCount) {
        control->selectedRowSlot = control->selectedRowSlot + 1;
      }
    }
    newSelectedSlot = control->selectedRowSlot;
    if (newSelectedSlot != previousSelectedSlot) {
      if (((control->listStateFlags & UI_LIST_PLAY_SELECTION_SOUND) != 0) &&
         (control->activationSound != (DirectSoundVoiceSet *)0x0)) {
        (*g_SoundPlayOneShot)(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
      }
      rowValue = ((uint)((int)newSelectedSlot - (int)control->rowSlots) >> 2) * control->rowHeight;
      UiScrollableControl_ClampOffsetsToViewport
                (rowValue + control->rowHeight + 1,(control->base).rightOffset,rowValue,0,
                 (UiScrollableControl *)(control->base).parent);
      rowValue = g_UiListActivationPulseFrames;
      control->listStateFlags = control->listStateFlags | UI_LIST_DEFERRED_ACTION_PENDING;
      control->listStateFlags = control->listStateFlags & 0xffffff;
      control->listStateFlags = control->listStateFlags | rowValue << 0x18;
    }
    return false;
  }
UiListControl_DelegateUnhandledKeyboardEvent:
  handled = UiNode_DefaultKeyboardEventMoveFocusNextCf(keyboardStateMask,keyCode,&control->base);
  return handled;
}


/* Address: 0x004BB480.
   Ownership: ui/controls/lists.
   Purpose: Recomputes content height from count and row height, invokes the child layout callback, reselects the
   current pointer-derived index, then queues the action ID at +0x5C.
   Local calls: UiPointerList_GetSelectedIndexVariantBCf, UiPointerList_SelectIndexVariantB.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime].
*/
void __thandor_preserve_eax_edx
UiPointerList_RefreshSelectionAndQueueAction(UiPointerListControl *control)

{
  UiNodeBase *parentNode;
  UiListRowIndexEaxCf5 selectedIndex;
  UiNodeVtable *parentVtable;
  
  parentNode = (control->base).parent;
  parentVtable = parentNode->vtable;
  (control->base).bottomOffset = control->rowHeight * control->rowCount + 1;
  (*parentVtable->layout)(parentNode);
  selectedIndex = UiPointerList_GetSelectedIndexVariantBCf(control);
  UiPointerList_SelectIndexVariantB(selectedIndex.rowIndex,control);
  UiActionQueue_Enqueue(control->actionId,control);
  return;
}


/* Address: 0x004B87A0.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B7920[4]@004B7920.
   Local calls: UiScrollableControl_RefreshChildAndScrollThumbs.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
UiScrollableControl_BeginPrimaryScrollInteraction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiScrollableControl *control)

{
  dword trackStartOffset;
  int horizontalTrackEnd;
  int localY;
  int localXOrTrackBottom;
  GraphicsTextureSizeEaxEdxCf9 textureSize;
  
  localXOrTrackBottom = pointerX - (control->base).left;
  localY = pointerY - (control->base).top;
  if ((control->scrollStateFlags &
      (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) == 0) {
UiScrollableControl_TryVerticalScrollbarInteraction:
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) == 0) {
      return;
    }
    textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
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
    textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
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
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
      if ((int)(localY - trackStartOffset) < (int)textureSize.logicalHeightPixels) {
        control->scrollStateFlags =
             control->scrollStateFlags |
             (UI_SCROLL_VERTICAL_DECREMENT_ACTIVE|UI_SCROLL_PRIMARY_INTERACTION_ACTIVE);
        goto UiScrollableControl_InvalidateAfterPrimaryScrollInteraction;
      }
      control->scrollOffsetY = control->scrollOffsetY + ((int)control->viewportHeight >> 1);
      control->scrollStateFlags =
           control->scrollStateFlags | UI_SCROLL_VERTICAL_TRACK_BEFORE_THUMB_ACTIVE;
    }
    else {
      if (localY < control->verticalThumbBottom) {
        control->pointerAnchorY = localY - control->verticalThumbTop;
        control->scrollStateFlags = control->scrollStateFlags | UI_SCROLL_VERTICAL_THUMB_ACTIVE;
        goto UiScrollableControl_InvalidateAfterPrimaryScrollInteraction;
      }
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
      if (localXOrTrackBottom - localY <= (int)textureSize.logicalHeightPixels) {
        control->scrollStateFlags =
             control->scrollStateFlags |
             (UI_SCROLL_VERTICAL_INCREMENT_ACTIVE|UI_SCROLL_PRIMARY_INTERACTION_ACTIVE);
        goto UiScrollableControl_InvalidateAfterPrimaryScrollInteraction;
      }
      control->scrollOffsetY = control->scrollOffsetY - ((int)control->viewportHeight >> 1);
      control->scrollStateFlags =
           control->scrollStateFlags | UI_SCROLL_VERTICAL_TRACK_AFTER_THUMB_ACTIVE;
    }
  }
  else {
    textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
    if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM) == 0) {
      if ((-1 < localY) && (localY < (int)textureSize.logicalHeightPixels))
      goto UiScrollableControl_BeginHorizontalScrollbarInteraction;
      goto UiScrollableControl_TryVerticalScrollbarInteraction;
    }
    if (((control->base).layoutHeight <= localY) ||
       (localY < (int)((control->base).layoutHeight - textureSize.logicalHeightPixels)))
    goto UiScrollableControl_TryVerticalScrollbarInteraction;
UiScrollableControl_BeginHorizontalScrollbarInteraction:
    horizontalTrackEnd = (control->base).layoutWidth;
    textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
    if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_RIGHT) != 0) {
      horizontalTrackEnd = horizontalTrackEnd - textureSize.logicalWidthPixels;
    }
    trackStartOffset = 0;
    if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) != 0) {
      trackStartOffset = textureSize.logicalWidthPixels;
    }
    if ((localXOrTrackBottom < (int)trackStartOffset) || (horizontalTrackEnd <= localXOrTrackBottom))
    goto UiScrollableControl_TryVerticalScrollbarInteraction;
    if (localXOrTrackBottom < control->horizontalThumbLeft) {
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
      if ((int)(localXOrTrackBottom - trackStartOffset) < (int)textureSize.logicalWidthPixels) {
        control->scrollStateFlags =
             control->scrollStateFlags |
             (UI_SCROLL_HORIZONTAL_DECREMENT_ACTIVE|UI_SCROLL_PRIMARY_INTERACTION_ACTIVE);
        goto UiScrollableControl_InvalidateAfterPrimaryScrollInteraction;
      }
      control->scrollOffsetX = control->scrollOffsetX + ((int)control->viewportWidth >> 1);
      control->scrollStateFlags =
           control->scrollStateFlags | UI_SCROLL_HORIZONTAL_TRACK_BEFORE_THUMB_ACTIVE;
    }
    else {
      if (localXOrTrackBottom < control->horizontalThumbRight) {
        control->pointerAnchorX = localXOrTrackBottom - control->horizontalThumbLeft;
        control->scrollStateFlags = control->scrollStateFlags | UI_SCROLL_HORIZONTAL_THUMB_ACTIVE;
        goto UiScrollableControl_InvalidateAfterPrimaryScrollInteraction;
      }
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
      if (horizontalTrackEnd - localXOrTrackBottom <= (int)textureSize.logicalWidthPixels) {
        control->scrollStateFlags =
             control->scrollStateFlags |
             (UI_SCROLL_HORIZONTAL_INCREMENT_ACTIVE|UI_SCROLL_PRIMARY_INTERACTION_ACTIVE);
        goto UiScrollableControl_InvalidateAfterPrimaryScrollInteraction;
      }
      control->scrollOffsetX = control->scrollOffsetX - ((int)control->viewportWidth >> 1);
      control->scrollStateFlags =
           control->scrollStateFlags | UI_SCROLL_HORIZONTAL_TRACK_AFTER_THUMB_ACTIVE;
    }
  }
  UiScrollableControl_RefreshChildAndScrollThumbs(control);
UiScrollableControl_InvalidateAfterPrimaryScrollInteraction:
  UiNode_InvalidateRoot(&control->base);
  return;
}


/* Address: 0x004B8A20.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B7920[5]@004B7920.
   Local calls: UiScrollableControl_RefreshChildAndScrollThumbs.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_preserve_eax
UiScrollableControl_EndPrimaryScrollInteraction
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
  control->scrollStateFlags = control->scrollStateFlags & 0xe0e0dfff;
  UiNode_InvalidateRoot(&control->base);
  return;
}


/* Address: 0x004B8BF0.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B7920[8]@004B7920.
   Local calls: UiScrollableControl_RefreshChildAndScrollThumbs.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
UiScrollableControl_UpdatePrimaryScrollDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiScrollableControl *control)

{
  int thumbOffsetOrLocalX;
  int anchorOrLocalY;
  dword arrowSize;
  int trackLength;
  dword arrowStart;
  GraphicsTextureSizeEaxEdxCf9 textureSize;
  
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
    textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
    thumbOffsetOrLocalX = ((pointerX - thumbOffsetOrLocalX) - anchorOrLocalY) - textureSize.logicalWidthPixels;
    trackLength = trackLength + textureSize.logicalWidthPixels * -2;
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) != 0) {
        thumbOffsetOrLocalX = thumbOffsetOrLocalX - textureSize.logicalWidthPixels;
      }
      trackLength = trackLength - textureSize.logicalWidthPixels;
    }
    control->scrollOffsetX =
         (UiPixelOffset)
         (((longlong)(int)(control->contentWidth - control->viewportWidth) * (longlong)thumbOffsetOrLocalX) /
         (longlong)-((trackLength - control->horizontalThumbRight) + control->horizontalThumbLeft));
    goto UiScrollableControl_RefreshAfterPrimaryDragUpdate;
  }
  if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_THUMB_ACTIVE) != 0) {
    thumbOffsetOrLocalX = (control->base).top;
    anchorOrLocalY = control->pointerAnchorY;
    trackLength = (control->base).layoutHeight;
    textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
    thumbOffsetOrLocalX = ((pointerY - thumbOffsetOrLocalX) - anchorOrLocalY) - textureSize.logicalHeightPixels;
    trackLength = trackLength + textureSize.logicalHeightPixels * -2;
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
      if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) != 0) {
        thumbOffsetOrLocalX = thumbOffsetOrLocalX - textureSize.logicalHeightPixels;
      }
      trackLength = trackLength - textureSize.logicalHeightPixels;
    }
    control->scrollOffsetY =
         (UiPixelOffset)
         (((longlong)(int)(control->contentHeight - control->viewportHeight) * (longlong)thumbOffsetOrLocalX) /
         (longlong)-((trackLength - control->verticalThumbBottom) + control->verticalThumbTop));
    goto UiScrollableControl_RefreshAfterPrimaryDragUpdate;
  }
  if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_DECREMENT_ACTIVE) == 0) {
    if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_INCREMENT_ACTIVE) == 0) {
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_DECREMENT_ACTIVE) == 0) {
        if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_INCREMENT_ACTIVE) == 0) {
          return;
        }
        anchorOrLocalY = pointerY - (control->base).top;
        thumbOffsetOrLocalX = pointerX - (control->base).left;
        textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
        arrowSize = textureSize.logicalHeightPixels;
        if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) == 0) {
          if ((thumbOffsetOrLocalX < (control->base).layoutWidth) &&
             ((int)((control->base).layoutWidth - textureSize.logicalWidthPixels) <= thumbOffsetOrLocalX))
          goto UiScrollableControl_ValidateBottomArrowHover;
        }
        else if ((-1 < thumbOffsetOrLocalX) && (thumbOffsetOrLocalX < (int)textureSize.logicalWidthPixels)) {
UiScrollableControl_ValidateBottomArrowHover:
          thumbOffsetOrLocalX = (control->base).layoutHeight;
          if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM) != 0) {
            textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
            thumbOffsetOrLocalX = thumbOffsetOrLocalX - textureSize.logicalHeightPixels;
          }
          if ((anchorOrLocalY < thumbOffsetOrLocalX) && ((int)(thumbOffsetOrLocalX - arrowSize) <= anchorOrLocalY))
          goto UiScrollableControl_SetArrowHoverActive;
        }
      }
      else {
        anchorOrLocalY = pointerY - (control->base).top;
        thumbOffsetOrLocalX = pointerX - (control->base).left;
        textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
        arrowSize = textureSize.logicalHeightPixels;
        if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) == 0) {
          if ((thumbOffsetOrLocalX < (control->base).layoutWidth) &&
             ((int)((control->base).layoutWidth - textureSize.logicalWidthPixels) <= thumbOffsetOrLocalX))
          goto UiScrollableControl_ValidateTopArrowHover;
        }
        else if ((-1 < thumbOffsetOrLocalX) && (thumbOffsetOrLocalX < (int)textureSize.logicalWidthPixels)) {
UiScrollableControl_ValidateTopArrowHover:
          arrowStart = 0;
          if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) != 0) {
            textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
            arrowStart = textureSize.logicalHeightPixels;
          }
          if (((int)arrowStart <= anchorOrLocalY) && (anchorOrLocalY < (int)(arrowStart + arrowSize)))
          goto UiScrollableControl_SetArrowHoverActive;
        }
      }
    }
    else {
      anchorOrLocalY = pointerY - (control->base).top;
      thumbOffsetOrLocalX = pointerX - (control->base).left;
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
      arrowSize = textureSize.logicalWidthPixels;
      if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) == 0) {
        if ((anchorOrLocalY < (control->base).layoutHeight) &&
           ((int)((control->base).layoutHeight - textureSize.logicalHeightPixels) <= anchorOrLocalY))
        goto UiScrollableControl_ValidateRightArrowHover;
      }
      else if ((-1 < anchorOrLocalY) && (anchorOrLocalY < (int)textureSize.logicalHeightPixels)) {
UiScrollableControl_ValidateRightArrowHover:
        anchorOrLocalY = (control->base).layoutWidth;
        if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_RIGHT) != 0) {
          textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
          anchorOrLocalY = anchorOrLocalY - textureSize.logicalWidthPixels;
        }
        if ((thumbOffsetOrLocalX < anchorOrLocalY) && ((int)(anchorOrLocalY - arrowSize) <= thumbOffsetOrLocalX))
        goto UiScrollableControl_SetArrowHoverActive;
      }
    }
  }
  else {
    anchorOrLocalY = pointerY - (control->base).top;
    thumbOffsetOrLocalX = pointerX - (control->base).left;
    textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
    arrowSize = textureSize.logicalWidthPixels;
    if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) == 0) {
      if ((anchorOrLocalY < (control->base).layoutHeight) &&
         ((int)((control->base).layoutHeight - textureSize.logicalHeightPixels) <= anchorOrLocalY))
      goto UiScrollableControl_ValidateLeftArrowHover;
    }
    else if ((-1 < anchorOrLocalY) && (anchorOrLocalY < (int)textureSize.logicalHeightPixels)) {
UiScrollableControl_ValidateLeftArrowHover:
      arrowStart = 0;
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) != 0) {
        textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
        arrowStart = textureSize.logicalWidthPixels;
      }
      if (((int)arrowStart <= thumbOffsetOrLocalX) && (thumbOffsetOrLocalX < (int)(arrowStart + arrowSize))) {
UiScrollableControl_SetArrowHoverActive:
        if ((control->scrollStateFlags & UI_SCROLL_PRIMARY_INTERACTION_ACTIVE) != 0) {
          return;
        }
        control->scrollStateFlags = control->scrollStateFlags | UI_SCROLL_PRIMARY_INTERACTION_ACTIVE
        ;
        goto UiScrollableControl_RefreshAfterPrimaryDragUpdate;
      }
    }
  }
  if ((control->scrollStateFlags & UI_SCROLL_PRIMARY_INTERACTION_ACTIVE) == 0) {
    return;
  }
  control->scrollStateFlags = control->scrollStateFlags & ~UI_SCROLL_PRIMARY_INTERACTION_ACTIVE;
UiScrollableControl_RefreshAfterPrimaryDragUpdate:
  UiScrollableControl_RefreshChildAndScrollThumbs(control);
  UiNode_InvalidateRoot(&control->base);
  return;
}


/* Address: 0x004B8F90.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B7920[9]@004B7920.
   Local calls: UiScrollableControl_RefreshChildAndScrollThumbs.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
UiScrollableControl_UpdateSecondaryScrollDrag
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
  (*g_PointerSetPosition)(control->pointerAnchorY,control->pointerAnchorX);
  return;
}


/* Address: 0x004B9070.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B7920[16]@004B7920.
   Local calls: UiScrollableControl_RefreshChildAndScrollThumbs.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_void_preserve_ecx_edx
UiScrollableControl_TickAutoScroll(UiScrollableControl *control)

{
  UiPixelOffset horizontalScrollStep;
  UiPixelOffset verticalScrollStep;
  
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
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B7920[17]@004B7920.
   Local calls: UiScrollableControl_RefreshChildAndScrollThumbs.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
UiScrollableControl_HandlePointerWheel
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiScrollableControl *control)

{
  UiNodeBase *firstChildNode;
  int scrollStep;
  UiNodeBase *contentChild;
  
  if (((((control->scrollStateFlags &
         (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) &&
       ((control->scrollStateFlags & 0x3000) == 0)) &&
      (((control->base).nodeFlags & UI_NODE_SUPPRESSED) == 0)) &&
     (firstChildNode = (control->base).firstChild, wheelDelta != 0)) {
    scrollStep = g_UiScrollWheelDefaultStep;
    if ((firstChildNode != (UiNodeBase *)0xffffffff) &&
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
   Ownership: ui/controls/lists.
   Purpose: Selects an in-range pointer-list entry by storing base + index*4 at +0x60 and invalidates the
   corresponding fixed-height row. EAX, EDX, and flags remain governed by the original code.
   Local calls: UiScrollableControl_ClampOffsetsToViewport.
*/
void __thandor_void_preserve_eax_ecx_edx
UiPointerList_SelectIndexVariantA(UiListRowIndex index,UiPointerListControl *control)

{
  int clipBottom;
  
  if (index < control->rowCount) {
    control->selectedRowSlot = control->rowSlots + index;
    clipBottom = control->rowHeight * index;
    UiScrollableControl_ClampOffsetsToViewport
              (clipBottom + 1 + control->rowHeight,(control->base).rightOffset,clipBottom,0,
               (UiScrollableControl *)(control->base).parent);
  }
  return;
}


/* Address: 0x004BB020.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004BA590[4]@004BA590.
   Local calls: UiScrollableControl_ClampOffsetsToViewport.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
UiListControl_SelectRowFromPointer
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiListControl *control)

{
  uint rowIndex;
  int clipBottom;
  sdword *topEdgeField;
  
  topEdgeField = &(control->base).top;
  if ((((*topEdgeField <= pointerY) && (pointerY - *topEdgeField < (control->base).layoutHeight)) &&
      ((control->base).left <= pointerX)) && (pointerX < (control->base).right)) {
    rowIndex = (uint)(pointerY - *topEdgeField) / control->rowHeight;
    if (rowIndex < control->rowCount) {
      control->listStateFlags = control->listStateFlags | UI_LIST_SELECTION_CONFIRMED;
      if ((((control->base).nodeFlags & UI_NODE_REPEAT_OR_DOUBLE_CLICK) != 0) ||
         (control->listStateFlags = control->listStateFlags & ~UI_LIST_SELECTION_CONFIRMED,
         control->rowSlots + rowIndex != control->selectedRowSlot)) {
        control->selectedRowSlot = control->rowSlots + rowIndex;
        clipBottom = rowIndex * control->rowHeight;
        UiScrollableControl_ClampOffsetsToViewport
                  (clipBottom + 1 + control->rowHeight,(control->base).rightOffset,clipBottom,0,
                   (UiScrollableControl *)(control->base).parent);
        UiActionQueue_Enqueue(control->actionId,control);
        if (((control->listStateFlags & UI_LIST_PLAY_SELECTION_SOUND) != 0) &&
           (control->activationSound != (DirectSoundVoiceSet *)0x0)) {
          (*g_SoundPlayOneShot)(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
        }
      }
    }
  }
  return;
}


/* Address: 0x004BB7A0.
   Ownership: ui/controls/lists.
   Purpose: Bubble-sorts in descending unsigned lexicographic order by two consecutive dwords at fieldOffset, then
   restores selection and invalidates its row. Typed parameters: p0 fieldOffset→UiPointerListFieldByteOffset_V342.
   Calling convention, exact VariableStorage serialization, function body bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Local calls: UiScrollableControl_ClampOffsetsToViewport.
*/
void __thandor_void_preserve_eax_ecx_edx
UiPointerList_SortByDwordPairFieldDescending
          (UiPointerListFieldByteOffset fieldOffset,UiPointerListControl *control)

{
  void *swapEntry;
  void *selectedEntry;
  uint leftKey;
  uint rightKey;
  int innerCountOrRowTop;
  int outerCount;
  UiListRowCount rowsRemaining;
  void **pivotSlot;
  void **scanSlot;
  
  scanSlot = control->rowSlots;
  if (scanSlot != (void **)0x0) {
    innerCountOrRowTop = control->rowCount - 1;
    if ((innerCountOrRowTop != 0) && (-1 < innerCountOrRowTop)) {
      selectedEntry = *control->selectedRowSlot;
      pivotSlot = scanSlot;
      outerCount = innerCountOrRowTop;
      do {
        do {
          scanSlot = scanSlot + 1;
          leftKey = *(uint *)((int)*pivotSlot + fieldOffset);
          rightKey = *(uint *)((int)*scanSlot + fieldOffset);
          if ((leftKey <= rightKey) &&
             ((leftKey < rightKey ||
              (((uint *)((int)*pivotSlot + fieldOffset))[1] <=
               ((uint *)((int)*scanSlot + fieldOffset))[1])))) {
            LOCK();
            swapEntry = *scanSlot;
            *scanSlot = *pivotSlot;
            UNLOCK();
            *pivotSlot = swapEntry;
          }
          innerCountOrRowTop = innerCountOrRowTop + -1;
        } while (innerCountOrRowTop != 0);
        scanSlot = pivotSlot + 1;
        innerCountOrRowTop = outerCount + -1;
        pivotSlot = scanSlot;
        outerCount = innerCountOrRowTop;
      } while (innerCountOrRowTop != 0);
      scanSlot = control->rowSlots;
      rowsRemaining = control->rowCount;
      innerCountOrRowTop = 0;
      do {
        if (selectedEntry == *scanSlot)
        goto 
        UiPointerList_SortByDwordPairFieldDescending_CommitResolvedSelectedRowSlotAndClampViewport;
        innerCountOrRowTop = innerCountOrRowTop + control->rowHeight;
        scanSlot = scanSlot + 1;
        rowsRemaining = rowsRemaining - 1;
      } while (rowsRemaining != 0);
      scanSlot = control->rowSlots;
UiPointerList_SortByDwordPairFieldDescending_CommitResolvedSelectedRowSlotAndClampViewport:
      control->selectedRowSlot = scanSlot;
      UiScrollableControl_ClampOffsetsToViewport
                (innerCountOrRowTop + 1 + control->rowHeight,(control->base).rightOffset,innerCountOrRowTop,0,
                 (UiScrollableControl *)(control->base).parent);
    }
  }
  return;
}


/* Address: 0x004BB8A0.
   Ownership: ui/controls/lists.
   Purpose: Bubble-sorts in ascending unsigned order by one dword at fieldOffset, then restores selection and
   invalidates its row. Typed parameters: p0 fieldOffset→UiPointerListFieldByteOffset_V342. Calling convention,
   exact VariableStorage serialization, function body bytes, control flow, globals, locals, and executable data
   remain unchanged.
   Local calls: UiScrollableControl_ClampOffsetsToViewport.
*/
void __thandor_void_preserve_eax_ecx_edx
UiPointerList_SortByDwordFieldAscending
          (UiPointerListFieldByteOffset fieldOffset,UiPointerListControl *control)

{
  void *swapEntry;
  void *selectedEntry;
  int innerCountOrRowTop;
  int outerCount;
  UiListRowCount rowsRemaining;
  void **pivotSlot;
  void **scanSlot;
  
  scanSlot = control->rowSlots;
  if (scanSlot != (void **)0x0) {
    innerCountOrRowTop = control->rowCount - 1;
    if ((innerCountOrRowTop != 0) && (-1 < innerCountOrRowTop)) {
      selectedEntry = *control->selectedRowSlot;
      pivotSlot = scanSlot;
      outerCount = innerCountOrRowTop;
      do {
        do {
          scanSlot = scanSlot + 1;
          if (*(uint *)((int)*scanSlot + fieldOffset) <= *(uint *)((int)*pivotSlot + fieldOffset)) {
            LOCK();
            swapEntry = *scanSlot;
            *scanSlot = *pivotSlot;
            UNLOCK();
            *pivotSlot = swapEntry;
          }
          innerCountOrRowTop = innerCountOrRowTop + -1;
        } while (innerCountOrRowTop != 0);
        scanSlot = pivotSlot + 1;
        innerCountOrRowTop = outerCount + -1;
        pivotSlot = scanSlot;
        outerCount = innerCountOrRowTop;
      } while (innerCountOrRowTop != 0);
      scanSlot = control->rowSlots;
      rowsRemaining = control->rowCount;
      innerCountOrRowTop = 0;
      do {
        if (selectedEntry == *scanSlot)
        goto UiPointerList_SortByDwordFieldAscending_CommitResolvedSelectedRowSlotAndClampViewport;
        innerCountOrRowTop = innerCountOrRowTop + control->rowHeight;
        scanSlot = scanSlot + 1;
        rowsRemaining = rowsRemaining - 1;
      } while (rowsRemaining != 0);
      scanSlot = control->rowSlots;
UiPointerList_SortByDwordFieldAscending_CommitResolvedSelectedRowSlotAndClampViewport:
      control->selectedRowSlot = scanSlot;
      UiScrollableControl_ClampOffsetsToViewport
                (innerCountOrRowTop + 1 + control->rowHeight,(control->base).rightOffset,innerCountOrRowTop,0,
                 (UiScrollableControl *)(control->base).parent);
    }
  }
  return;
}


/* Address: 0x004BBCD0.
   Ownership: ui/controls/lists.
   Purpose: Handles ui timed list control select row from pointer.
   Local calls: UiTimedListControl_SelectRecordAndScrollIntoView.
   Cross-module calls: RichTextCommandStream_MeasureRegs [assets/text/richtext], UiActionQueue_Enqueue
   [ui/core/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
UiTimedListControl_SelectRowFromPointer
          (int pointerButton,int pointerY,int pointerX,UiNodeBase *control)

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
  if (header == (UiTimedListTreeRecord16 *)0x0) {
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
    record = record + 1;
    remaining = remaining - 1;
    if ((((record[-1].recordFlags0C & 1) != 0) && ((record[-1].recordFlags0C & 2) != 0)) &&
        (record[-1].nestedRecordBlockOrParentLink08 != (UiTimedListTreeRecord16 *)0x0) &&
        (depth < TREE_DEPTH_LIMIT)) {
      savedRecord[depth] = record;
      savedRemaining[depth] = remaining;
      depth = depth + 1;
      header = record[-1].nestedRecordBlockOrParentLink08;
      remaining = (int)header->recordCountOrRowPayload00;
      record = header + 1;
    }
    while (remaining == 0) {
      if (depth == 0) {
        return;
      }
      depth = depth - 1;
      record = savedRecord[depth];
      remaining = savedRemaining[depth];
    }
  }
  indent = depth * (int)list->observedDrawParameter80;
  x = x - indent;
  if (x < 0) {
    /* Inside the indentation: the expand/collapse icon of the row. */
    if (((record->recordFlags0C & 1) != 0) && (indent != 0) &&
        (*g_GraphicsTextureSourceTestOpaquePixel)
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
         RichTextCommandStream_MeasureRegs(g_UiListTextStyle,(word *)record->recordCountOrRowPayload00);
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
      ((record->recordFlags0C & 1) != 0) && (list->base.recordSelectionCallback != 0)) {
    list->base.recordSelectionCallback(record,list);
  }
  return;
}


/* Address: 0x0040FF70.
   Ownership: ui/controls/lists.
   Purpose: Finds a timed-list tree record by label.
*/
UiTimedListTreeRecord16 * __thandor_eax_preserve_ecx_edx
UiTimedListTree_FindRecordByLabel(word *labelUtf16,UiTimedListTreeRecord16 *recordBlock)

{
  word labelChar;
  int scanRemaining;
  int compareRemaining;
  dword recordsRemaining;
  word *recordLabelCursor;
  word *queryLabelCursor;
  bool charsEqual;
  
  scanRemaining = 0x100;
  recordLabelCursor = labelUtf16;
  do {
    if (scanRemaining == 0) break;
    scanRemaining = scanRemaining + -1;
    labelChar = *recordLabelCursor;
    recordLabelCursor = recordLabelCursor + 1;
  } while (labelChar != 0);
  recordsRemaining = recordBlock->recordCountOrRowPayload00;
  do {
    if (recordsRemaining == 0) {
      return (UiTimedListTreeRecord16 *)0x0;
    }
    recordBlock = recordBlock + 1;
    charsEqual = false;
    compareRemaining = 0x100 - scanRemaining;
    recordLabelCursor = (word *)recordBlock->recordCountOrRowPayload00;
    queryLabelCursor = labelUtf16;
    do {
      if (compareRemaining == 0) break;
      compareRemaining = compareRemaining + -1;
      charsEqual = *recordLabelCursor == *queryLabelCursor;
      recordLabelCursor = recordLabelCursor + 1;
      queryLabelCursor = queryLabelCursor + 1;
    } while (charsEqual);
    if (charsEqual) {
      return recordBlock;
    }
    recordsRemaining = recordsRemaining - 1;
  } while( true );
}

/* Address: 0x0040FFE0.
   Ownership: ui/controls/lists.
   Purpose: Builds a timed-list directory record block and exposes the recovered carry/error contract.
*/
Recovered0040FFE0EaxCf5 __thandor_eax_cf_preserve_ecx_edx
UiTimedListTree_BuildDirectoryRecordBlockCf(word *pathUtf16)

{
  dword *recordCursor;
  dword leafCodeUnitPair;
  dword *entryStride;
  dword *scanCursor;
  dword *outputRecords;
  uint driveLetter;
  EngineDriveTypeCode driveType;
  dword *remainingBytes;
  int scanRemaining;
  dword *bytesAfterLabel;
  uint scanValue;
  dword directoryEntryCount;
  byte *driveLetterCursor;
  dword *leaf;
  dword *labelWriteCursor;
  uint *labelCursor;
  uint *scanPointer;
  uint *nextScanPointer;
  bool mediaCheckResult;
  ArenaShrinkEaxCf5 shrinkResult;
  Recovered0040FFE0EaxCf5 result;
  Recovered0040FFE0EaxCf5 failureResult;
  ArenaAllocEaxCf5 allocResult;
  ArenaLargestAllocationEaxEcxCf9 largestBlock;
  FileSystemEnumerationEaxEcxCf9 enumResult;
  DriveLetterEnumerationEaxEcx8 driveEnum;
  
  if (*pathUtf16 == 0) {
    allocResult = (*g_MemoryApi.alloc)(0x220);
    outputRecords = (dword *)allocResult.eax;
    if (!allocResult.carry) {
      *outputRecords = 1;
      outputRecords[1] = 0;
      outputRecords[2] = 0;
      outputRecords[3] = 0x80000000;
      outputRecords[4] = (dword)(outputRecords + 8);
      outputRecords[5] = 0x27;
      outputRecords[6] = 0;
      outputRecords[7] = 1;
      (*g_LocaleCopyDefaultComputerLabelUtf16)((word *)(outputRecords + 8));
      return THANDOR_BITCAST(qword, Recovered0040FFE0EaxCf5, ((THANDOR_BITCAST(ArenaAllocEaxCf5, qword, allocResult) & 0xFFFFFFFFFFull) & 0xffffffff));
    }
  }
  else if ((pathUtf16[3] == 0) || (pathUtf16[2] == 0)) {
    driveEnum = (*g_FileSystemEnumerateDriveLetters)((byte *)THANDOR_ADDR(g_UiTimedListDriveLetters,0));
    directoryEntryCount = driveEnum.driveCount;
    allocResult = (*g_MemoryApi.alloc)(driveEnum.driveCountMirror * 0x210 + 0x10);
    outputRecords = (dword *)allocResult.eax;
    if (!allocResult.carry) {
      result.recordBlockOrError = outputRecords + 4;
      *outputRecords = directoryEntryCount;
      outputRecords[1] = 0;
      outputRecords[2] = 0;
      outputRecords[3] = 0x80000000;
      labelCursor = ((dword *)result.recordBlockOrError) + directoryEntryCount * 4;
      driveLetterCursor = (byte *)THANDOR_ADDR(g_UiTimedListDriveLetters,0);
      do {
        driveLetter = (uint)*driveLetterCursor;
        *(dword *)result.recordBlockOrError = (dword)labelCursor;
        scanValue = driveLetter;
        driveType = (*g_FileSystemGetDriveTypeCode)(driveLetter);
        ((dword *)result.recordBlockOrError)[1] = (dword)driveType;
        ((dword *)result.recordBlockOrError)[2] = 0;
        ((dword *)result.recordBlockOrError)[3] = 0;
        u________0040ff58[0] = (wchar_t)driveLetter;
        *labelCursor = driveLetter;
        ((word *)((int)labelCursor + 2))[0] = 0x3a;
        ((word *)((int)labelCursor + 2))[1] = 0x5b;
        ((word *)((int)labelCursor + 6))[0] = 0x5d;
        ((word *)((int)labelCursor + 6))[1] = 0;
        mediaCheckResult = (*g_FileSystemCheckDriveMediaReady)(scanValue);
        if (mediaCheckResult) {
LAB_004102e2:
          ((word *)((int)labelCursor + 6))[0] = 0x5d;
          ((word *)((int)labelCursor + 6))[1] = 0;
        }
        else {
          enumResult = (*g_FileSystemEnumerateDirectoryOrVolumeEntriesCf)
                             (FILESYSTEM_ENUMERATE_VOLUME_LABEL,0xffffffff,0x1f8,
                              (byte *)((int)labelCursor + 6),(byte *)u________0040ff58);
          if (enumResult.carry) goto LAB_004102e2;
          if (enumResult.entryCount == 0) {
            ((word *)((int)labelCursor + 6))[0] = 0x5d;
            ((word *)((int)labelCursor + 6))[1] = 0;
          }
          else {
            scanRemaining = 0x100;
            scanPointer = labelCursor;
            do {
              nextScanPointer = scanPointer;
              if (scanRemaining == 0) break;
              scanRemaining = scanRemaining + -1;
              nextScanPointer = (uint *)((int)scanPointer + 2);
              scanValue = *scanPointer;
              scanPointer = nextScanPointer;
            } while ((word)scanValue != 0);
            ((word *)((int)nextScanPointer + -2))[0] = 0x5d;
            ((word *)((int)nextScanPointer + -2))[1] = 0;
          }
          enumResult = (*g_FileSystemEnumerateDirectoryOrVolumeEntriesCf)
                             (FILESYSTEM_ENUMERATE_DIRECTORIES,0xffffffff,0x200,
                              (byte *)g_UiTimedListRecordPathScratch.codeUnits,
                              (byte *)u________0040ff58);
          if (enumResult.carry) goto LAB_004102e2;
          if (enumResult.entryCount != 0) {
            ((dword *)result.recordBlockOrError)[3] = ((dword *)result.recordBlockOrError)[3] | 1;
          }
        }
        labelCursor = labelCursor + 0x80;
        result.recordBlockOrError = (dword *)((dword *)result.recordBlockOrError) + 4;
        driveLetterCursor = driveLetterCursor + 1;
        directoryEntryCount = directoryEntryCount - 1;
        if (directoryEntryCount == 0) {
          return THANDOR_BITCAST(qword, Recovered0040FFE0EaxCf5, ((THANDOR_BITCAST(ArenaAllocEaxCf5, qword, allocResult) & 0xFFFFFFFFFFull) & 0xffffffff));
        }
      } while( true );
    }
  }
  else {
    WidePath_SplitParentAndLeaf
              (g_UiTimedListRecordPathScratch.codeUnits,g_UiTimedListCombinedPathScratch.codeUnits,
               pathUtf16);
    WidePath_CombineDirectoryAndLeaf
              (g_UiTimedListRecordPathScratch.codeUnits,(word *)THANDOR_ADDR(g_WildcardAllFilesUtf16,0),
               g_UiTimedListCombinedPathScratch.codeUnits);
    largestBlock = (*g_MemoryApi.allocLargestFreeBlock)();
    outputRecords = (dword *)largestBlock.allocationOrError;
    if (!largestBlock.carry) {
      enumResult = (*g_FileSystemEnumerateDirectoryOrVolumeEntriesCf)
                         (FILESYSTEM_ENUMERATE_DIRECTORIES,0xffffffff,largestBlock.blockSizeOrSentinel,
                          (byte *)outputRecords,(byte *)g_UiTimedListRecordPathScratch.codeUnits);
      directoryEntryCount = enumResult.entryCount;
      entryStride = (dword *)enumResult.recordSizeBytes;
      result.recordBlockOrError = entryStride;
      if (!enumResult.carry) {
        shrinkResult = (*g_MemoryApi.shrinkInPlace)((int)entryStride * directoryEntryCount,outputRecords);
        result.recordBlockOrError = (dword *)shrinkResult.scratchOrError;
        if (!shrinkResult.carry) {
          largestBlock = (*g_MemoryApi.allocLargestFreeBlock)();
          result.recordBlockOrError = (dword *)largestBlock.allocationOrError;
          if (!largestBlock.carry) {
            scanRemaining = directoryEntryCount + 1;
            scanCursor = (dword *)0x14;
            remainingBytes = (dword *)(largestBlock.blockSizeOrSentinel + scanRemaining * -0x10);
            if ((uint)(scanRemaining * 0x10) <= largestBlock.blockSizeOrSentinel && remainingBytes != (dword *)0x0) {
              *(dword *)result.recordBlockOrError = directoryEntryCount;
              ((dword *)result.recordBlockOrError)[1] = 0;
              ((dword *)result.recordBlockOrError)[2] = 0;
              ((dword *)result.recordBlockOrError)[3] = 0x80000000;
              labelWriteCursor = ((dword *)result.recordBlockOrError) + scanRemaining * 4;
              leaf = outputRecords;
              recordCursor = result.recordBlockOrError;
              for (; directoryEntryCount != 0; directoryEntryCount = directoryEntryCount - 1) {
                recordCursor[4] = (dword)labelWriteCursor;
                recordCursor[5] = 0x26;
                recordCursor[6] = 0;
                recordCursor[7] = 0;
                WidePath_CombineDirectoryAndLeaf
                          (g_UiTimedListRecordPathScratch.codeUnits,(word *)leaf,
                           g_UiTimedListCombinedPathScratch.codeUnits);
                WidePath_CombineDirectoryAndLeaf
                          (g_UiTimedListSecondaryPathScratch.codeUnits,(word *)THANDOR_ADDR(g_WildcardAllFilesUtf16,0),
                           g_UiTimedListRecordPathScratch.codeUnits);
                enumResult = (*g_FileSystemEnumerateDirectoryOrVolumeEntriesCf)
                                   (FILESYSTEM_ENUMERATE_DIRECTORIES,0xffffffff,0x200,
                                    (byte *)g_UiTimedListRecordPathScratch.codeUnits,
                                    (byte *)g_UiTimedListSecondaryPathScratch.codeUnits);
                if ((!enumResult.carry) && (enumResult.entryCount != 0)) {
                  recordCursor[7] = recordCursor[7] | 1;
                }
                scanRemaining = 0x100;
                scanCursor = leaf;
                do {
                  if (scanRemaining == 0) break;
                  scanRemaining = scanRemaining + -1;
                  leafCodeUnitPair = *scanCursor;
                  scanCursor = (dword *)((int)scanCursor + 2);
                } while ((word)leafCodeUnitPair != 0);
                scanCursor = (dword *)(0x102U - scanRemaining & 0xfffffffe);
                bytesAfterLabel = (dword *)((int)remainingBytes - (int)scanCursor);
                if ((remainingBytes < scanCursor || bytesAfterLabel == (dword *)0x0) ||
                   (remainingBytes = (dword *)((int)bytesAfterLabel - (int)scanCursor),
                   bytesAfterLabel < scanCursor || remainingBytes == (dword *)0x0)) goto LAB_004101a4;
                scanCursor = leaf;
                for (scanValue = 0x102U - scanRemaining >> 1; scanValue != 0; scanValue = scanValue - 1) {
                  *labelWriteCursor = *scanCursor;
                  scanCursor = scanCursor + 1;
                  labelWriteCursor = labelWriteCursor + 1;
                }
                leaf = (dword *)((int)leaf + (int)entryStride);
                recordCursor = recordCursor + 4;
              }
              shrinkResult = (*g_MemoryApi.shrinkInPlace)
                                 ((int)labelWriteCursor + (0x200 - (int)result.recordBlockOrError),
                                  result.recordBlockOrError);
              scanCursor = (dword *)shrinkResult.scratchOrError;
              if (!shrinkResult.carry) {
                (*g_MemoryApi.free)(outputRecords);
                result.carry = false;
                return result;
              }
            }
LAB_004101a4:
            (*g_MemoryApi.free)(result.recordBlockOrError);
            result.recordBlockOrError = scanCursor;
          }
        }
      }
      (*g_MemoryApi.free)(outputRecords);
      outputRecords = result.recordBlockOrError;
    }
  }
  failureResult.carry = true;
  failureResult.recordBlockOrError = outputRecords;
  return failureResult;
}

/* Address: 0x00410380.
   Ownership: ui/controls/lists.
   Purpose: Builds the timed-list directory hierarchy.
*/
UiTimedListDirectoryHierarchyEaxEdxCf9 __thandor_eax_edx_cf_preserve_ecx
UiTimedListTree_BuildDirectoryHierarchyCf(word *selectedPathUtf16)

{
  ushort labelDifference;
  undefined2 compareCodeUnit;
  UiTimedListTreeRecord16 *rootBlock;
  int copyRemaining;
  dword recordsRemaining;
  int levelCount;
  UiTimedListTreeRecord16 *unaff_EBP;
  UiTimedListTreeRecord16 *linkRecord;
  WidePathBuffer256 *pathCopyCursor;
  UiTimedListTreeRecord16 *parentBlock;
  Recovered0040FFE0EaxCf5 builtBlock;
  UiTimedListDirectoryHierarchyEaxEdxCf9 result;
  UiTimedListDirectoryHierarchyEaxEdxCf9 failureResult;
  UiTimedListTreeRecord16 *recordCursor;
  
  levelCount = 0;
  while( true ) {
    pathCopyCursor = &g_UiTimedListHierarchyPathScratch;
    for (copyRemaining = 0x80; copyRemaining != 0; copyRemaining = copyRemaining + -1) {
      pathCopyCursor->firstTwoCodeUnits = *(dword *)selectedPathUtf16;
      selectedPathUtf16 = (word *)(selectedPathUtf16 + 2);
      pathCopyCursor = (WidePathBuffer256 *)(&pathCopyCursor->firstTwoCodeUnits + 1);
    }
    if ((g_UiTimedListHierarchyPathScratch.codeUnits[3] == 0) ||
       (g_UiTimedListHierarchyPathScratch.codeUnits[2] == 0)) break;
    builtBlock = UiTimedListTree_BuildDirectoryRecordBlockCf
                       (g_UiTimedListHierarchyPathScratch.codeUnits);
    result.rootRecordBlockOrError = builtBlock.recordBlockOrError;
    if (builtBlock.carry) goto joined_r0x00410484;
    WidePath_SplitParentAndLeaf
              (g_UiTimedListRecordPathScratch.codeUnits,
               g_UiTimedListHierarchyParentPathScratch.codeUnits,
               g_UiTimedListHierarchyPathScratch.codeUnits);
    UiTimedListTree_FindRecordByLabel
              (g_UiTimedListRecordPathScratch.codeUnits,result.rootRecordBlockOrError);
    levelCount = levelCount + 1;
    selectedPathUtf16 = (word *)&g_UiTimedListHierarchyParentPathScratch;
    unaff_EBP = result.rootRecordBlockOrError;
  }
  builtBlock = UiTimedListTree_BuildDirectoryRecordBlockCf(g_UiTimedListHierarchyPathScratch.codeUnits);
  rootBlock = builtBlock.recordBlockOrError;
  result.rootRecordBlockOrError = rootBlock;
  if (!builtBlock.carry) {
    recordsRemaining = rootBlock->recordCountOrRowPayload00;
    compareCodeUnit = g_UiTimedListHierarchyPathScratch.codeUnits[0];
    recordCursor = rootBlock;
    goto LAB_0041040e;
  }
  goto joined_r0x00410484;
  while( true ) {
    compareCodeUnit = labelDifference ^ *(ushort *)recordCursor->recordCountOrRowPayload00;
    recordsRemaining = recordsRemaining - 1;
    if (recordsRemaining == 0) break;
LAB_0041040e:
    recordCursor = recordCursor + 1;
    labelDifference = compareCodeUnit ^ *(ushort *)recordCursor->recordCountOrRowPayload00;
    if ((labelDifference & 0xdf) == 0) {
      levelCount = levelCount + 1;
      g_UiTimedListHierarchyPathScratch.codeUnits[0] = 0;
      builtBlock = UiTimedListTree_BuildDirectoryRecordBlockCf
                         (g_UiTimedListHierarchyPathScratch.codeUnits);
      result.rootRecordBlockOrError = builtBlock.recordBlockOrError;
      if (!builtBlock.carry) {
        linkRecord = result.rootRecordBlockOrError + 1;
        parentBlock = result.rootRecordBlockOrError;
        do {
          if (rootBlock != (UiTimedListTreeRecord16 *)0x0) {
            rootBlock->rowPayload04 = (dword)parentBlock;
            rootBlock->nestedRecordBlockOrParentLink08 = linkRecord;
          }
          if (linkRecord != (UiTimedListTreeRecord16 *)0x0) {
            linkRecord->recordFlags0C =
                 linkRecord->recordFlags0C | UI_TIMED_LIST_RECORD_ENABLES_NESTED_CHILD_TRAVERSAL;
            linkRecord->nestedRecordBlockOrParentLink08 = rootBlock;
          }
          levelCount = levelCount + -1;
          linkRecord = recordCursor;
          parentBlock = rootBlock;
        } while (levelCount != 0);
        result.selectedRecordOrNull = recordCursor;
        result.carry = false;
        return result;
      }
      goto joined_r0x00410484;
    }
  }
  result.rootRecordBlockOrError = (UiTimedListTreeRecord16 *)0x0;
joined_r0x00410484:
  for (; levelCount != 0; levelCount = levelCount + -1) {
    (*g_MemoryApi.free)(unaff_EBP);
  }
  failureResult.selectedRecordOrNull = (UiTimedListTreeRecord16 *)0x0;
  failureResult.rootRecordBlockOrError = result.rootRecordBlockOrError;
  failureResult.carry = true;
  return failureResult;
}

/* Address: 0x004104B0.
   Ownership: ui/controls/lists.
   Purpose: Recursively frees a timed-list record block while testing containment.
*/
bool __thandor_cf_preserve_eax_ecx_edx
UiTimedListTree_FreeRecordBlockRecursiveAndTestContainsCf
          (UiTimedListTreeRecord16 *targetRecord,UiTimedListTreeRecord16 *recordBlock)

{
  UiTimedListTreeRecord16 *recordCursor;
  dword recordsRemaining;
  int containsCount;
  bool childContains;
  
  containsCount = 0;
  if (recordBlock != (UiTimedListTreeRecord16 *)0x0) {
    recordCursor = recordBlock;
    for (recordsRemaining = recordBlock->recordCountOrRowPayload00; recordsRemaining != 0; recordsRemaining = recordsRemaining - 1) {
      if (recordCursor + 1 == targetRecord) {
        containsCount = containsCount + 1;
      }
      if ((((recordCursor[1].recordFlags0C & UI_TIMED_LIST_RECORD_OBSERVED_BIT0) != 0) &&
          ((recordCursor[1].recordFlags0C & UI_TIMED_LIST_RECORD_ENABLES_NESTED_CHILD_TRAVERSAL) != 0)) &&
         (childContains = UiTimedListTree_FreeRecordBlockRecursiveAndTestContainsCf
                            (targetRecord,recordCursor[1].nestedRecordBlockOrParentLink08), childContains)) {
        containsCount = containsCount + 1;
      }
      recordCursor = recordCursor + 1;
    }
  }
  (*g_MemoryApi.free)(recordBlock);
  return containsCount != 0;
}

/* Address: 0x00410520.
   Ownership: ui/controls/lists.
   Purpose: Attaches a directory record block to the timed-list hierarchy.
*/
bool __thandor_cf_preserve_ecx_edx
UiTimedListTree_AttachDirectoryRecordBlockCf(UiTimedListTreeRecord16 *record)

{
  dword *directory;
  UiTimedListTreeRecord16 *linkedRecord;
  int copyRemaining;
  UiTimedListTreeRecord16 *scanRecord;
  UiTimedListTreeRecord16 *previousRecord;
  dword *recordPathSourceDwords;
  dword *combinedPathSourceDwords;
  dword *recordPathScratchDestDwords;
  dword *combinedPathScratchDestDwords;
  Recovered0040FFE0EaxCf5 builtBlock;
  
  recordPathSourceDwords = (dword *)record->recordCountOrRowPayload00;
  recordPathScratchDestDwords = (dword *)&g_UiTimedListRecordPathScratch;
  for (copyRemaining = 0x80; copyRemaining != 0; copyRemaining = copyRemaining + -1) {
    *recordPathScratchDestDwords = *recordPathSourceDwords;
    recordPathSourceDwords = recordPathSourceDwords + 1;
    recordPathScratchDestDwords = recordPathScratchDestDwords + 1;
  }
  if (((record[-1].recordFlags0C & UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY) == 0) ||
     (record[-1].rowPayload04 != 0)) {
    linkedRecord = record;
    if (g_UiTimedListRecordPathScratch.codeUnits[1] == 0x3a) {
      g_UiTimedListRecordPathScratch.codeUnits[2] = 0;
      WidePath_CombineDirectoryAndLeaf
                (g_UiTimedListHierarchyParentPathScratch.codeUnits,(word *)THANDOR_ADDR(g_WildcardAllFilesUtf16,0),
                 g_UiTimedListRecordPathScratch.codeUnits);
    }
    else {
      while( true ) {
        do {
          scanRecord = linkedRecord;
          linkedRecord = scanRecord + -1;
        } while ((scanRecord[-1].recordFlags0C & UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY) == 0);
        linkedRecord = scanRecord[-1].nestedRecordBlockOrParentLink08;
        if (linkedRecord == (UiTimedListTreeRecord16 *)0x0) {
          return true;
        }
        directory = (dword *)linkedRecord->recordCountOrRowPayload00;
        if (*(word *)((int)directory + 2) == 0x3a) break;
        WidePath_CombineDirectoryAndLeaf
                  (g_UiTimedListCombinedPathScratch.codeUnits,
                   g_UiTimedListRecordPathScratch.codeUnits,(word *)directory);
        combinedPathSourceDwords = (dword *)&g_UiTimedListCombinedPathScratch;
        combinedPathScratchDestDwords = (dword *)&g_UiTimedListRecordPathScratch;
        for (copyRemaining = 0x80; copyRemaining != 0; copyRemaining = copyRemaining + -1) {
          *combinedPathScratchDestDwords = *combinedPathSourceDwords;
          combinedPathSourceDwords = combinedPathSourceDwords + 1;
          combinedPathScratchDestDwords = combinedPathScratchDestDwords + 1;
        }
      }
      g_UiTimedListCombinedPathScratch.firstTwoCodeUnits = *directory;
      THANDOR_PART(dword, g_UiTimedListCombinedPathScratch, 4) = 0;
      WidePath_CombineDirectoryAndLeaf
                (g_UiTimedListSecondaryPathScratch.codeUnits,
                 g_UiTimedListRecordPathScratch.codeUnits,g_UiTimedListCombinedPathScratch.codeUnits
                );
      WidePath_CombineDirectoryAndLeaf
                (g_UiTimedListHierarchyParentPathScratch.codeUnits,(word *)THANDOR_ADDR(g_WildcardAllFilesUtf16,0),
                 g_UiTimedListSecondaryPathScratch.codeUnits);
    }
  }
  else {
    g_UiTimedListHierarchyParentPathScratch.firstTwoCodeUnits = 0x3a0061;
    THANDOR_PART(dword, g_UiTimedListHierarchyParentPathScratch, 4) = 0;
  }
  builtBlock = UiTimedListTree_BuildDirectoryRecordBlockCf
                    (g_UiTimedListHierarchyParentPathScratch.codeUnits);
  linkedRecord = builtBlock.recordBlockOrError;
  if (builtBlock.carry) {
    return true;
  }
  record->nestedRecordBlockOrParentLink08 = linkedRecord;
  linkedRecord->nestedRecordBlockOrParentLink08 = record;
  do {
    previousRecord = record + -1;
    scanRecord = record + -1;
    record = previousRecord;
  } while ((scanRecord->recordFlags0C & UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY) == 0);
  linkedRecord->rowPayload04 = (dword)previousRecord;
  return false;
}

/* Address: 0x00410670.
   Ownership: ui/controls/lists.
   Purpose: Toggles expansion for a timed-list directory record.
*/
void __thandor_void_preserve_eax_ecx
UiTimedListControl_ToggleDirectoryRecordExpansion
          (UiTimedListTreeRecord16 *record,UiTimedListRuntimeExtendedView88 *control)

{
  UiTimedListTreeRecord16 *targetRecord;
  bool cfResult;
  
  targetRecord = UiTimedListControl_GetSelectedRecord(control);
  if ((record->recordFlags0C & UI_TIMED_LIST_RECORD_OBSERVED_BIT0) != 0) {
    if ((record->recordFlags0C & UI_TIMED_LIST_RECORD_ENABLES_NESTED_CHILD_TRAVERSAL) != 0) {
      cfResult = UiTimedListTree_FreeRecordBlockRecursiveAndTestContainsCf
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
    cfResult = UiTimedListTree_AttachDirectoryRecordBlockCf(record);
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
   Ownership: ui/controls/lists.
   Purpose: Builds the recovered path for a timed-list tree record.
*/
bool __thandor_cf_preserve_ecx_edx
UiTimedListTree_BuildRecordPathCf(dword *outputPathDwords,UiTimedListTreeRecord16 *record)

{
  dword *directory;
  int copyRemaining;
  UiTimedListTreeRecord16 *scanRecord;
  dword *recordPathSourceDwords;
  dword *combinedPathSourceDwords;
  dword *recordPathScratchDestDwords;
  dword *combinedPathScratchDestDwords;
  
  recordPathScratchDestDwords = (dword *)&g_UiTimedListRecordPathScratch;
  recordPathSourceDwords = (dword *)record->recordCountOrRowPayload00;
  copyRemaining = 0x80;
  if (((record[-1].recordFlags0C & UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY) == 0) ||
     (record[-1].rowPayload04 != 0)) {
    for (; copyRemaining != 0; copyRemaining = copyRemaining + -1) {
      *recordPathScratchDestDwords = *recordPathSourceDwords;
      recordPathSourceDwords = (dword *)(recordPathSourceDwords + 1);
      recordPathScratchDestDwords = recordPathScratchDestDwords + 1;
    }
    if (g_UiTimedListRecordPathScratch.codeUnits[1] == 0x3a) {
      g_UiTimedListRecordPathScratch.codeUnits[2] = 0;
      recordPathSourceDwords = (dword *)&g_UiTimedListRecordPathScratch;
    }
    else {
      while( true ) {
        do {
          scanRecord = record;
          record = scanRecord + -1;
        } while ((scanRecord[-1].recordFlags0C & UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY) == 0);
        record = scanRecord[-1].nestedRecordBlockOrParentLink08;
        if (record == (UiTimedListTreeRecord16 *)0x0) {
          return true;
        }
        directory = (dword *)record->recordCountOrRowPayload00;
        if (*(word *)((int)directory + 2) == 0x3a) break;
        WidePath_CombineDirectoryAndLeaf
                  (g_UiTimedListCombinedPathScratch.codeUnits,
                   g_UiTimedListRecordPathScratch.codeUnits,(word *)directory);
        combinedPathSourceDwords = (dword *)&g_UiTimedListCombinedPathScratch;
        combinedPathScratchDestDwords = (dword *)&g_UiTimedListRecordPathScratch;
        for (copyRemaining = 0x80; copyRemaining != 0; copyRemaining = copyRemaining + -1) {
          *combinedPathScratchDestDwords = *combinedPathSourceDwords;
          combinedPathSourceDwords = combinedPathSourceDwords + 1;
          combinedPathScratchDestDwords = combinedPathScratchDestDwords + 1;
        }
      }
      g_UiTimedListCombinedPathScratch.firstTwoCodeUnits = *directory;
      THANDOR_PART(dword, g_UiTimedListCombinedPathScratch, 4) = 0;
      WidePath_CombineDirectoryAndLeaf
                (g_UiTimedListSecondaryPathScratch.codeUnits,
                 g_UiTimedListRecordPathScratch.codeUnits,g_UiTimedListCombinedPathScratch.codeUnits
                );
      recordPathSourceDwords = (dword *)&g_UiTimedListSecondaryPathScratch;
    }
  }
  for (copyRemaining = 0x80; copyRemaining != 0; copyRemaining = copyRemaining + -1) {
    *outputPathDwords = *recordPathSourceDwords;
    recordPathSourceDwords = (dword *)(recordPathSourceDwords + 1);
    outputPathDwords = outputPathDwords + 1;
  }
  return false;
}

/* Address: 0x004B11C0.
   Ownership: ui/controls/lists.
   Purpose: Traverses a sibling list and forwards the action ID through vtable slot +0x3C, restoring matching
   controls. Kept distinct from player IDs, command opcodes, and resource identifiers. Typed parameters: p0
   actionId→UiActionId_V338. Calling convention, storage, body bytes, control flow, and executable data remain
   unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
UiNodeList_UnsuppressActionId(UiActionId actionId,UiNodeBase *firstNode)

{
  for (; firstNode != (UiNodeBase *)0xffffffff; firstNode = firstNode->nextSibling) {
    (*firstNode->vtable->unsuppressActionId)(actionId,firstNode);
  }
  return;
}


/* Address: 0x004B1200.
   Ownership: ui/controls/lists.
   Purpose: Traverses a sibling list and forwards the action ID through vtable slot +0x38, suppressing matching
   controls. Kept distinct from player IDs, command opcodes, and resource identifiers. Typed parameters: p0
   actionId→UiActionId_V338. Calling convention, storage, body bytes, control flow, and executable data remain
   unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
UiNodeList_SuppressActionId(UiActionId actionId,UiNodeBase *firstNode)

{
  for (; firstNode != (UiNodeBase *)0xffffffff; firstNode = firstNode->nextSibling) {
    (*firstNode->vtable->suppressActionId)(actionId,firstNode);
  }
  return;
}


/* Address: 0x004B2550.
   Ownership: ui/controls/lists.
   Purpose: Handles keyboard activation for selectable controls, optionally plays the derived control's keyboard
   sound, toggles or sets selected state, queues actionId, invalidates the root, and returns consumption through
   CF.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime], UiNode_InvalidateRoot [ui/core/runtime],
   UiNode_DefaultKeyboardEventMoveFocusNextCf [ui/controls/input].
*/
bool __thandor_cf_preserve_eax_ecx_edx
UiSelectableControl_KeyboardEventCf
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiSoundSelectableControl *control)

{
  bool handled;
  UiSelectableStateFlags activationKeyBindingFlag;
  
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) != 0)
  goto UiSelectableControl_DelegateUnhandledKeyboardEvent;
  if (keyCode == 0x20) {
    if ((&(control->selectable).base != g_UiKeyboardFocusNode) ||
       (((control->selectable).stateFlags & UI_SELECTABLE_IGNORE_FOCUSED_SPACE_ACTIVATION) != 0))
    goto UiSelectableControl_DelegateUnhandledKeyboardEvent;
  }
  else {
    if ((keyCode & 0xffff0000) == 0) goto UiSelectableControl_DelegateUnhandledKeyboardEvent;
    if (keyCode == 0x10001) {
      activationKeyBindingFlag = (control->selectable).stateFlags & 4;
    }
    else {
      if (keyCode != 0x10000) goto UiSelectableControl_DelegateUnhandledKeyboardEvent;
      activationKeyBindingFlag = (control->selectable).stateFlags & 8;
    }
    if (activationKeyBindingFlag == 0) goto UiSelectableControl_DelegateUnhandledKeyboardEvent;
  }
  if (((control->selectable).stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) == 0) {
    if ((((control->selectable).stateFlags & 0x80) != 0) &&
       (control->activationSound != (DirectSoundVoiceSet *)0x0)) {
      (*g_SoundPlayOneShot)(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
    }
    UiActionQueue_Enqueue((control->selectable).actionId,control);
    UiNode_InvalidateRoot(&(control->selectable).base);
    return false;
  }
  if (((control->selectable).stateFlags & UI_SELECTABLE_TOGGLE_ON_ACTIVATION) != 0) {
    if ((((control->selectable).stateFlags & 0x80) != 0) &&
       (control->activationSound != (DirectSoundVoiceSet *)0x0)) {
      (*g_SoundPlayOneShot)(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
    }
    (control->selectable).stateFlags =
         (control->selectable).stateFlags ^ UI_SELECTABLE_SELECTED_OR_CHECKED;
    UiActionQueue_Enqueue((control->selectable).actionId,control);
    UiNode_InvalidateRoot(&(control->selectable).base);
    return false;
  }
  if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
    if ((((control->selectable).stateFlags & 0x80) != 0) &&
       (control->activationSound != (DirectSoundVoiceSet *)0x0)) {
      (*g_SoundPlayOneShot)(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
    }
    (control->selectable).stateFlags =
         (control->selectable).stateFlags | UI_SELECTABLE_SELECTED_OR_CHECKED;
    UiActionQueue_Enqueue((control->selectable).actionId,control);
    UiNode_InvalidateRoot(&(control->selectable).base);
    return false;
  }
UiSelectableControl_DelegateUnhandledKeyboardEvent:
  handled = UiNode_DefaultKeyboardEventMoveFocusNextCf
                      (keyboardStateMask,keyCode,&(control->selectable).base);
  return handled;
}


/* Address: 0x004B26E0.
   Ownership: ui/controls/lists.
   Purpose: When actionId matches control->actionId, sets nodeFlags bit 0x08 and updates focus/activation state for
   the newly suppressed control.
   Cross-module calls: UiKeyboardFocus_ReleaseNode [ui/controls/input].
*/
void __thandor_void_preserve_eax_ecx_edx
UiSelectableControl_SuppressIfActionId(UiActionId actionId,UiSelectableControl *control)

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
   Ownership: ui/controls/lists.
   Purpose: When actionId matches control->actionId, clears nodeFlags bit 0x08 and updates focus/activation state
   for the restored control.
   Cross-module calls: UiKeyboardFocus_AcquireIfNone [ui/controls/input].
*/
void __thandor_void_preserve_eax_ecx_edx
UiSelectableControl_UnsuppressIfActionId(UiActionId actionId,UiSelectableControl *control)

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
   Ownership: ui/controls/lists.
   Purpose: Variadic group test. CF=0 when at least one non-suppressed control has selected bit 0x02; CF=1 when
   none does. Kept distinct from player IDs, command opcodes, and resource identifiers. Typed parameters: p0
   controlCount→UiControlCount_V338. Calling convention, storage, body bytes, control flow, and executable data
   remain unchanged.
*/
UiSelectableNodeEaxEcxCf9 __thandor_eax_ecx_cf_preserve_edx
UiSelectableGroup_NoneVisibleSelectedCf(UiControlCount controlCount,...)

{
  int controlAddress;
  uint controlIndex;
  int controlPointerByteOffset;
  UiSelectableNodeEaxEcxCf9 noneSelectedResult;
  UiSelectableNodeEaxEcxCf9 selectedResult;
  
  controlPointerByteOffset = 0;
  controlIndex = 0;
  while ((controlAddress = *(int *)((byte *)(&controlCount + 1) + controlPointerByteOffset),
         (*(uint *)(controlAddress + 0x48) & 8) != 0 || ((*(uint *)(controlAddress + 0x4c) & 2) == 0))) {
    controlIndex = controlIndex + 1;
    controlPointerByteOffset = controlPointerByteOffset + 4;
    if (controlCount <= controlIndex) {
      noneSelectedResult.node = (UiNodeBase *)controlAddress;
      noneSelectedResult.carry = true;
      return noneSelectedResult;
    }
  }
  selectedResult.controlIndexOrCount = controlIndex;
  selectedResult.node = (UiNodeBase *)controlAddress;
  selectedResult.carry = false;
  return selectedResult;
}


/* Address: 0x004B2D70.
   Ownership: ui/controls/lists.
   Purpose: Variadic group test that ignores node visibility. CF=0 when any listed control has selected bit 0x02;
   CF=1 when none does. Kept distinct from player IDs, command opcodes, and resource identifiers. Typed parameters:
   p0 controlCount→UiControlCount_V338. Calling convention, storage, body bytes, control flow, and executable data
   remain unchanged.
*/
UiSelectableGroupIndexEcxCf5 __thandor_eax_ecx_cf_preserve_edx
UiSelectableGroup_NoneSelectedCf(UiControlCount controlCount,...)

{
  uint controlIndex;
  int controlPointerByteOffset;
  UiSelectableGroupIndexEcxCf5 noneSelectedResult;
  UiSelectableGroupIndexEcxCf5 selectedResult;
  
  controlPointerByteOffset = 0;
  controlIndex = 0;
  do {
    if ((*(uint *)(*(int *)((byte *)(&controlCount + 1) + controlPointerByteOffset) + 0x4c) & 2) != 0) {
      selectedResult.carryNoneSelected = false;
      selectedResult.selectedIndexOrCount = controlIndex;
      return selectedResult;
    }
    controlIndex = controlIndex + 1;
    controlPointerByteOffset = controlPointerByteOffset + 4;
  } while (controlIndex < controlCount);
  noneSelectedResult.carryNoneSelected = true;
  return noneSelectedResult;
}


/* Address: 0x004B2DA0.
   Ownership: ui/controls/lists.
   Purpose: Variadic exclusive-selection helper. Sets selected bit 0x02 only on the chosen control, clears it on
   the remaining controls, and invalidates every listed control. Kept distinct from player IDs, command opcodes,
   and resource identifiers. Typed parameters: p0 controlCount→UiControlCount_V338. Calling convention, storage,
   body bytes, control flow, and executable data remain unchanged.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
UiSelectableGroup_SelectExclusive(UiControlCount controlCount,UiNodeBase *selectedControl,...)

{
  UiSelectableControl *node;
  uint controlIndex;
  int controlPointerByteOffset;

  controlPointerByteOffset = 0;
  controlIndex = 0;
  do {
    node = *(UiSelectableControl **)((byte *)(&selectedControl + 1) + controlPointerByteOffset);
    if (&node->base == selectedControl) {
      node->stateFlags = node->stateFlags | UI_SELECTABLE_SELECTED_OR_CHECKED;
    }
    else {
      node->stateFlags = node->stateFlags & ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    }
    UiNode_InvalidateRoot(&node->base);
    controlIndex = controlIndex + 1;
    controlPointerByteOffset = controlPointerByteOffset + 4;
  } while (controlIndex < controlCount);
  return;
}


/* Address: 0x004B2DE0.
   Ownership: ui/controls/lists.
   Purpose: Tests one selectable control. CF=1 only when nodeFlags bit 0x08 is clear and stateFlags bit 0x02 is
   set; otherwise CF=0.
*/
byte __thandor_cf_preserve_eax_ecx_edx
UiSelectableControl_IsSelectedCf(UiSelectableControl *control)

{
  if ((((control->base).nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     ((control->stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0)) {
    return 1;
  }
  return 0;
}


/* Address: 0x004B2E10.
   Ownership: ui/controls/lists.
   Purpose: Clears selected bit 0x02, sets it when the boolean argument is nonzero, then invalidates the control
   root. Typed parameters: p0 selected→UiBooleanState32_V342. Calling convention, exact VariableStorage
   serialization, function body bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
UiSelectableControl_SetSelected(UiBooleanState32 selected,UiSelectableControl *control)

{
  control->stateFlags = control->stateFlags & ~UI_SELECTABLE_SELECTED_OR_CHECKED;
  if (selected != 0) {
    control->stateFlags = control->stateFlags | UI_SELECTABLE_SELECTED_OR_CHECKED;
  }
  UiNode_InvalidateRoot(&control->base);
  return;
}


/* Address: 0x004B4920.
   Ownership: ui/controls/lists.
   Purpose: CF=0 when base.firstChild matches an entry in the page array; CF=1 when the active child is absent.
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
UiPageStack_ActivePageNotInListCf(UiPageStackControl *stack)

{
  uint pageIndex;
  bool notFound;
  StatusValueEaxCf5 result;
  
  pageIndex = 0;
  do {
    notFound = false;
    if ((stack->base).firstChild == (&stack->pages)[pageIndex]) goto LAB_004b4944;
    pageIndex = pageIndex + 1;
  } while (pageIndex < stack->pageCount);
  notFound = true;
LAB_004b4944:
  result.carry = notFound;
  result.valueOrError = pageIndex;
  return result;
}


/* Address: 0x004B7970.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B7920[0]@004B7920.
   Cross-module calls: UiContainer_RelocateChildren [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiScrollableControl_RelocateChildren
          (UiSerializedRelocationDelta relocationDelta,UiScrollableControl *control)

{
  UiNodeBase *contentChild;
  UiPixelExtent contentHeight;
  
  UiContainer_RelocateChildren(relocationDelta,&control->base);
  contentChild = (control->base).firstChild;
  if (contentChild != (UiNodeBase *)0xffffffff) {
    contentHeight = contentChild->bottomOffset;
    control->contentWidth = contentChild->rightOffset;
    control->contentHeight = contentHeight;
    control->scrollOffsetX = 0;
    control->scrollOffsetY = 0;
  }
  return;
}


/* Address: 0x004B79D0.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B7920[2]@004B7920.
   Cross-module calls: UiWindow_BlitTiledHorizontalEdge [ui/controls/layout], UiWindow_BlitTiledVerticalEdge
   [ui/controls/layout], UiWindow_BlitTiledInterior [ui/controls/layout], UiContainer_DrawIntersectingChildren
   [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiScrollableControl_DrawFrameContentAndScrollbars
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiScrollableControl *control)

{
  dword arrowLength;
  GraphicsSubresourceIndex subresource;
  dword tileEnd;
  dword horizontalBarLeft;
  int trackStart;
  int edgeScratch;
  int contentTop;
  int barOffset;
  int contentBottom;
  int contentRight;
  int trackEnd;
  bool accessFailed;
  GraphicsTextureSizeEaxEdxCf9 textureSize;
  dword capSize;
  
  accessFailed = (*g_GraphicsFramebufferBeginAccess)();
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
        textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
        arrowLength = textureSize.logicalWidthPixels;
        contentTop = contentTop + textureSize.logicalHeightPixels;
      }
      else {
        trackStart = contentBottom;
        textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
        arrowLength = textureSize.logicalWidthPixels;
        barOffset = contentBottom - textureSize.logicalHeightPixels;
        contentBottom = trackStart - textureSize.logicalHeightPixels;
      }
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
      horizontalBarLeft = tileEnd;
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) != 0) {
        horizontalBarLeft = textureSize.logicalWidthPixels;
      }
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_RIGHT) != 0) {
        contentRight = contentRight - textureSize.logicalWidthPixels;
      }
      if (((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_DECREMENT_ACTIVE) == 0) ||
         ((control->scrollStateFlags & UI_SCROLL_PRIMARY_INTERACTION_ACTIVE) == 0)) {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,barOffset + (control->base).top,
                   horizontalBarLeft + (control->base).left,0x5a,g_UiWindowTextureSource,g_FramebufferAccess);
        tileEnd = capSize;
      }
      else {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,barOffset + (control->base).top,
                   horizontalBarLeft + (control->base).left,0x62,g_UiWindowTextureSource,g_FramebufferAccess);
        tileEnd = capSize;
      }
      trackStart = horizontalBarLeft + arrowLength;
      trackEnd = contentRight - arrowLength;
      if (((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_INCREMENT_ACTIVE) == 0) ||
         ((control->scrollStateFlags & UI_SCROLL_PRIMARY_INTERACTION_ACTIVE) == 0)) {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,barOffset + (control->base).top,
                   trackEnd + (control->base).left,0x5b,g_UiWindowTextureSource,g_FramebufferAccess);
        contentRight = edgeScratch;
      }
      else {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,barOffset + (control->base).top,
                   trackEnd + (control->base).left,99,g_UiWindowTextureSource,g_FramebufferAccess);
        contentRight = edgeScratch;
      }
      if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_TRACK_BEFORE_THUMB_ACTIVE) == 0) {
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x5d,control->horizontalThumbLeft,barOffset,
                   trackStart,control);
      }
      else {
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x65,control->horizontalThumbLeft,barOffset,
                   trackStart,control);
      }
      edgeScratch = control->horizontalThumbRight;
      if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_TRACK_AFTER_THUMB_ACTIVE) == 0) {
        if (edgeScratch < clipRight) {
          edgeScratch = clipRight;
        }
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,edgeScratch,0x5d,trackEnd,barOffset,trackStart,control);
      }
      else {
        if (edgeScratch < clipRight) {
          edgeScratch = clipRight;
        }
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,edgeScratch,0x65,trackEnd,barOffset,trackStart,control);
      }
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0xc0,g_UiWindowTextureSource);
      capSize = textureSize.logicalWidthPixels;
      edgeScratch = control->horizontalThumbLeft;
      trackStart = control->horizontalThumbRight;
      if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_THUMB_ACTIVE) == 0) {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,barOffset + (control->base).top,
                   edgeScratch + (control->base).left,0xc0,g_UiWindowTextureSource,g_FramebufferAccess);
        trackStart = trackStart - capSize;
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,barOffset + (control->base).top,
                   trackStart + (control->base).left,0xc1,g_UiWindowTextureSource,g_FramebufferAccess);
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x5c,trackStart,barOffset,edgeScratch + capSize,control);
      }
      else {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,barOffset + (control->base).top,
                   edgeScratch + (control->base).left,0xc4,g_UiWindowTextureSource,g_FramebufferAccess);
        trackStart = trackStart - capSize;
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,barOffset + (control->base).top,
                   trackStart + (control->base).left,0xc5,g_UiWindowTextureSource,g_FramebufferAccess);
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,100,trackStart,barOffset,edgeScratch + capSize,control);
      }
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      edgeScratch = contentBottom;
      barOffset = contentTop;
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_RIGHT) == 0) {
        capSize = tileEnd;
        textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
        arrowLength = textureSize.logicalHeightPixels;
        capSize = capSize + textureSize.logicalWidthPixels;
      }
      else {
        trackStart = contentRight;
        capSize = tileEnd;
        textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
        arrowLength = textureSize.logicalHeightPixels;
        tileEnd = contentRight - textureSize.logicalWidthPixels;
        contentRight = trackStart - textureSize.logicalWidthPixels;
      }
      if (((control->scrollStateFlags & UI_SCROLL_VERTICAL_DECREMENT_ACTIVE) == 0) ||
         ((control->scrollStateFlags & UI_SCROLL_PRIMARY_INTERACTION_ACTIVE) == 0)) {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,contentTop + (control->base).top,
                   tileEnd + (control->base).left,0x5e,g_UiWindowTextureSource,g_FramebufferAccess);
      }
      else {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,contentTop + (control->base).top,
                   tileEnd + (control->base).left,0x66,g_UiWindowTextureSource,g_FramebufferAccess);
      }
      trackStart = contentTop + arrowLength;
      trackEnd = contentBottom - arrowLength;
      if (((control->scrollStateFlags & UI_SCROLL_VERTICAL_INCREMENT_ACTIVE) == 0) ||
         ((control->scrollStateFlags & UI_SCROLL_PRIMARY_INTERACTION_ACTIVE) == 0)) {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,trackEnd + (control->base).top,
                   tileEnd + (control->base).left,0x5f,g_UiWindowTextureSource,g_FramebufferAccess);
        contentBottom = edgeScratch;
        contentTop = barOffset;
      }
      else {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,trackEnd + (control->base).top,
                   tileEnd + (control->base).left,0x67,g_UiWindowTextureSource,g_FramebufferAccess);
        contentBottom = edgeScratch;
        contentTop = barOffset;
      }
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_TRACK_BEFORE_THUMB_ACTIVE) == 0) {
        UiWindow_BlitTiledVerticalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x61,control->verticalThumbTop,trackStart,
                   tileEnd,control);
      }
      else {
        UiWindow_BlitTiledVerticalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x69,control->verticalThumbTop,trackStart,
                   tileEnd,control);
      }
      edgeScratch = control->verticalThumbBottom;
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_TRACK_AFTER_THUMB_ACTIVE) == 0) {
        if (edgeScratch < clipBottom) {
          edgeScratch = clipBottom;
        }
        UiWindow_BlitTiledVerticalEdge
                  (clipTop,clipLeft,edgeScratch,clipRight,0x61,trackEnd,trackStart,tileEnd,control);
      }
      else {
        if (edgeScratch < clipBottom) {
          edgeScratch = clipBottom;
        }
        UiWindow_BlitTiledVerticalEdge
                  (clipTop,clipLeft,edgeScratch,clipRight,0x69,trackEnd,trackStart,tileEnd,control);
      }
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0xc2,g_UiWindowTextureSource);
      arrowLength = textureSize.logicalHeightPixels;
      edgeScratch = control->verticalThumbTop;
      barOffset = control->verticalThumbBottom;
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_THUMB_ACTIVE) == 0) {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,edgeScratch + (control->base).top,
                   tileEnd + (control->base).left,0xc2,g_UiWindowTextureSource,g_FramebufferAccess);
        barOffset = barOffset - arrowLength;
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,barOffset + (control->base).top,
                   tileEnd + (control->base).left,0xc3,g_UiWindowTextureSource,g_FramebufferAccess);
        UiWindow_BlitTiledVerticalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x60,barOffset,edgeScratch + arrowLength,tileEnd,control);
        tileEnd = capSize;
      }
      else {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,edgeScratch + (control->base).top,
                   tileEnd + (control->base).left,0xc6,g_UiWindowTextureSource,g_FramebufferAccess);
        barOffset = barOffset - arrowLength;
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,barOffset + (control->base).top,
                   tileEnd + (control->base).left,199,g_UiWindowTextureSource,g_FramebufferAccess);
        UiWindow_BlitTiledVerticalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x68,barOffset,edgeScratch + arrowLength,tileEnd,control);
        tileEnd = capSize;
      }
    }
    if ((control->scrollStateFlags & 0x400) != 0) {
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x6a,g_UiWindowTextureSource);
      capSize = textureSize.logicalWidthPixels;
      contentBottom = contentBottom - textureSize.logicalHeightPixels;
      contentRight = contentRight - capSize;
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,contentTop + (control->base).top,
                 tileEnd + (control->base).left,0x6a,g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,contentTop + (control->base).top,
                 contentRight + (control->base).left,0x6b,g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,contentBottom + (control->base).top,
                 tileEnd + (control->base).left,0x6c,g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,contentBottom + (control->base).top,
                 contentRight + (control->base).left,0x6d,g_UiWindowTextureSource,g_FramebufferAccess);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x6e,contentRight,contentTop,tileEnd + capSize,control);
      contentTop = contentTop + textureSize.logicalHeightPixels;
      edgeScratch = (tileEnd + capSize) - capSize;
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x6f,contentBottom,contentTop,edgeScratch,control);
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x70,contentBottom,contentTop,contentRight,control);
      tileEnd = edgeScratch + capSize;
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x71,contentRight,contentBottom,tileEnd,control);
    }
    if ((control->scrollStateFlags & 0x800) != 0) {
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x72,g_UiWindowTextureSource);
      capSize = textureSize.logicalWidthPixels;
      contentBottom = contentBottom - textureSize.logicalHeightPixels;
      contentRight = contentRight - capSize;
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,contentTop + (control->base).top,
                 tileEnd + (control->base).left,0x72,g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,contentTop + (control->base).top,
                 contentRight + (control->base).left,0x73,g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,contentBottom + (control->base).top,
                 tileEnd + (control->base).left,0x74,g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,contentBottom + (control->base).top,
                 contentRight + (control->base).left,0x75,g_UiWindowTextureSource,g_FramebufferAccess);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x76,contentRight,contentTop,tileEnd + capSize,control);
      contentTop = contentTop + textureSize.logicalHeightPixels;
      edgeScratch = (tileEnd + capSize) - capSize;
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x77,contentBottom,contentTop,edgeScratch,control);
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x78,contentBottom,contentTop,contentRight,control);
      tileEnd = edgeScratch + capSize;
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x79,contentRight,contentBottom,tileEnd,control);
    }
    if ((control->scrollStateFlags & 0x300) != 0) {
      subresource = 0;
      if ((control->scrollStateFlags & 0x100) != 0) {
        subresource = 0x7a;
      }
      UiWindow_BlitTiledInterior
                (clipTop,clipLeft,clipBottom,clipRight,subresource,contentBottom,contentRight,contentTop,tileEnd,control
                );
    }
    (*g_GraphicsFramebufferEndAccess)();
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
   Ownership: ui/controls/lists.
   Purpose: Rebuilds the scrollable child rectangle, content extents, horizontal and vertical scrollbar visibility,
   track bounds, thumb geometry, and clamped content offsets from the control rectangle and active child
   dimensions.
*/
void __thandor_void_preserve_eax_ecx_edx
UiScrollableControl_RebuildViewportAndScrollbars(UiScrollableControl *control)

{
  UiNodeBase *contentChild;
  UiPixelExtent childWidth;
  UiPixelExtent childHeight;
  uint minThumbLength;
  dword arrowSize;
  uint thumbLength;
  UiPixelOffset offsetX;
  UiPixelOffset offsetY;
  UiPixelExtent availableHeight;
  int verticalExtent;
  UiPixelExtent availableWidth;
  int horizontalExtent;
  GraphicsTextureSizeEaxEdxCf9 textureSize;
  
  contentChild = (control->base).firstChild;
  availableWidth = (control->base).right - (control->base).left;
  availableHeight = (control->base).bottom - (control->base).top;
  control->scrollStateFlags =
       control->scrollStateFlags &
       ~(UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT|
         UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP);
  (control->base).layoutWidth = availableWidth;
  (control->base).layoutHeight = availableHeight;
  if (contentChild != (UiNodeBase *)0xffffffff) {
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
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x6a,g_UiWindowTextureSource);
      control->contentOriginX = control->contentOriginX + textureSize.logicalWidthPixels;
      control->contentOriginY = control->contentOriginY + textureSize.logicalHeightPixels;
      availableWidth = availableWidth + textureSize.logicalWidthPixels * -2;
      availableHeight = availableHeight + textureSize.logicalHeightPixels * -2;
    }
    if ((control->scrollStateFlags & 0x800) != 0) {
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x72,g_UiWindowTextureSource);
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
    control->scrollStateFlags =
         control->scrollStateFlags &
         (control->scrollStateFlags >> 4 |
         ~(UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT|
           UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP));
    if (((control->scrollStateFlags & 0xffffffcf) != 0) &&
       ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0)) {
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
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
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
      if ((int)(verticalExtent - textureSize.logicalHeightPixels) < 0) {
        if (((control->scrollStateFlags &
             (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) == 0) &&
           ((control->scrollStateFlags & 0xffffffcf) != 0)) {
          textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
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
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
      control->viewportHeight = control->viewportHeight - textureSize.logicalHeightPixels;
      if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) != 0) {
        control->contentOriginY = control->contentOriginY + textureSize.logicalHeightPixels;
      }
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
      control->viewportWidth = control->viewportWidth - textureSize.logicalWidthPixels;
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) != 0) {
        control->contentOriginX = control->contentOriginX + textureSize.logicalWidthPixels;
      }
    }
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
    (*contentChild->vtable->layout)(contentChild);
    control->horizontalThumbLeft = 0;
    control->verticalThumbTop = 0;
    control->horizontalThumbRight = 0;
    control->verticalThumbBottom = 0;
    if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) != 0) {
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
      control->verticalThumbTop = control->verticalThumbTop + textureSize.logicalHeightPixels;
      control->verticalThumbBottom = control->verticalThumbBottom + textureSize.logicalHeightPixels;
    }
    if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) != 0) {
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
      control->horizontalThumbLeft = control->horizontalThumbLeft + textureSize.logicalWidthPixels;
      control->horizontalThumbRight = control->horizontalThumbRight + textureSize.logicalWidthPixels;
    }
    horizontalExtent = (control->base).layoutWidth;
    verticalExtent = (control->base).layoutHeight;
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
      arrowSize = textureSize.logicalWidthPixels;
      control->horizontalThumbLeft = control->horizontalThumbLeft + arrowSize;
      control->horizontalThumbRight = control->horizontalThumbRight + arrowSize;
      horizontalExtent = horizontalExtent + arrowSize * -2;
      verticalExtent = verticalExtent - textureSize.logicalHeightPixels;
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
      arrowSize = textureSize.logicalHeightPixels;
      control->verticalThumbTop = control->verticalThumbTop + arrowSize;
      control->verticalThumbBottom = control->verticalThumbBottom + arrowSize;
      verticalExtent = verticalExtent + arrowSize * -2;
      horizontalExtent = horizontalExtent - textureSize.logicalWidthPixels;
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
      thumbLength = (uint)(((longlong)(int)control->viewportWidth * (longlong)horizontalExtent) /
                    (longlong)(int)control->contentWidth);
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0xc0,g_UiWindowTextureSource);
      minThumbLength = textureSize.logicalWidthPixels * 2;
      if (thumbLength < minThumbLength) {
        thumbLength = minThumbLength;
      }
      control->horizontalThumbRight = control->horizontalThumbRight + thumbLength;
      horizontalExtent = (int)(((longlong)(int)-control->scrollOffsetX * (longlong)(int)(horizontalExtent - thumbLength)) /
                    (longlong)(int)(control->contentWidth - control->viewportWidth));
      control->horizontalThumbLeft = control->horizontalThumbLeft + horizontalExtent;
      control->horizontalThumbRight = control->horizontalThumbRight + horizontalExtent;
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      thumbLength = (uint)(((longlong)(int)control->viewportHeight * (longlong)verticalExtent) /
                    (longlong)(int)control->contentHeight);
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0xc2,g_UiWindowTextureSource);
      minThumbLength = textureSize.logicalHeightPixels * 2;
      if (thumbLength < minThumbLength) {
        thumbLength = minThumbLength;
      }
      control->verticalThumbBottom = control->verticalThumbBottom + thumbLength;
      horizontalExtent = (int)(((longlong)(int)-control->scrollOffsetY * (longlong)(int)(verticalExtent - thumbLength)) /
                    (longlong)(int)(control->contentHeight - control->viewportHeight));
      control->verticalThumbTop = control->verticalThumbTop + horizontalExtent;
      control->verticalThumbBottom = control->verticalThumbBottom + horizontalExtent;
    }
  }
  return;
}


/* Address: 0x004B8AC0.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B7920[10]@004B7920.
*/
GraphicsCursorFrameIndex __thandor_eax_preserve_ecx_edx
UiScrollableControl_QueryPointerRegion
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiScrollableControl *control)

{
  GraphicsCursorFrameIndex cursorFrame;
  
  cursorFrame = 0;
  if (((control->scrollStateFlags & 0x4000) != 0) &&
     ((control->scrollStateFlags &
      (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT|
       UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0)) {
    cursorFrame = 1;
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) == 0) {
      cursorFrame = 4;
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) == 0) {
      cursorFrame = 5;
    }
  }
  return cursorFrame;
}


/* Address: 0x004B8B10.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B7920[6]@004B7920.
*/
void __thandor_void_preserve_ecx_edx
UiScrollableControl_BeginSecondaryScrollInteraction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiScrollableControl *control)

{
  int localPointerX;
  int localPointerY;
  dword cursorFrame;
  
  g_CursorUseOverridePosition = g_CursorUseOverridePosition + 1;
  control->scrollStateFlags = control->scrollStateFlags | 0x1000;
  control->pointerAnchorX = pointerX;
  control->pointerAnchorY = pointerY;
  localPointerX = pointerX - (control->base).left;
  localPointerY = pointerY - (control->base).top;
  control->scrollStateFlags = control->scrollStateFlags & 0xe0e0cfff;
  if (((((int)control->contentOriginX <= localPointerX) &&
       ((int)control->contentOriginY <= localPointerY)) &&
      ((int)(localPointerX - control->viewportWidth) < (int)control->contentOriginX)) &&
     (((int)(localPointerY - control->viewportHeight) < (int)control->contentOriginY &&
      (control->scrollStateFlags = control->scrollStateFlags | 0x4000,
      (control->scrollStateFlags &
      (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT|
       UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0)))) {
    cursorFrame = 1;
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) == 0) {
      cursorFrame = 4;
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) == 0) {
      cursorFrame = 5;
    }
    (*g_GraphicsCursorSetFrame)(cursorFrame);
  }
  return;
}


/* Address: 0x004B8BC0.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B7920[7]@004B7920.
*/
void UiScrollableControl_EndSecondaryScrollInteraction
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiScrollableControl *control)

{
  g_CursorUseOverridePosition = 0;
  control->scrollStateFlags = control->scrollStateFlags & 0xffffafff;
  (*g_GraphicsCursorSetFrame)(0);
  return;
}


/* Address: 0x004B9000.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B7920[11]@004B7920.
   Cross-module calls: UiContainer_HitTestChildren [ui/controls/layout].
*/
UiNodeBase * __thandor_eax_preserve_ecx_edx
UiScrollableControl_HitTestContentAndScrollbars
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiScrollableControl *control)

{
  int localPointerX;
  int localPointerY;
  
  localPointerX = pointerX - (control->base).left;
  localPointerY = pointerY - (control->base).top;
  if (((((control->scrollStateFlags & 0x4000) == 0) &&
       ((int)control->contentOriginX <= localPointerX)) &&
      ((int)control->contentOriginY <= localPointerY)) &&
     (((int)(localPointerX - control->viewportWidth) < (int)control->contentOriginX &&
      ((int)(localPointerY - control->viewportHeight) < (int)control->contentOriginY)))) {
    control = (UiScrollableControl *)UiContainer_HitTestChildren(pointerY,pointerX,&control->base);
  }
  return &control->base;
}


/* Address: 0x004BA4E0.
   Ownership: ui/controls/lists.
   Purpose: Handles ui pointer list get row slots variant a.
*/
void ** UiPointerList_GetRowSlotsVariantA(UiPointerListControl *control)

{
  return control->rowSlots;
}


/* Address: 0x004BA560.
   Ownership: ui/controls/lists.
   Purpose: Returns (+0x60 - +0x50)/4 in EAX. CF mirrors control flag 0x04: clear when absent and set when present.
*/
UiListRowIndex UiPointerList_GetSelectedIndexVariantACf(UiPointerListControl *control)

{
  UiListRowIndex selectedRowIndex;
  
  selectedRowIndex = (int)control->selectedRowSlot - (int)control->rowSlots >> 2;
  if ((control->listStateFlags & UI_LIST_SELECTION_CONFIRMED) == 0) {
    return selectedRowIndex;
  }
  return selectedRowIndex;
}

/* Address: 0x004BADE0.
   Ownership: ui/controls/lists.
   Purpose: Handles ui list control draw rows and selection.
   Cross-module calls: UiWindow_BlitTiledHorizontalEdge [ui/controls/layout], RichTextCommandStream_MeasureRegs
   [assets/text/richtext], RichTextCommandStream_DrawSingleLine [assets/text/richtext].
*/
void __thandor_void_preserve_eax_ecx_edx
UiListControl_DrawRowsAndSelection
          (int clipTop,int clipLeft,int clipBottom,int clipRight,UiListControl *control)

{
  int columnWidth;
  int rowTop;
  byte *rowRecord;
  dword lastRowIndex;
  int columnX;
  void **lastRowSlot;
  int widthOrColumnCount;
  UiListColumn *column;
  void **rowSlot;
  word *commandStream;
  bool accessFailed;
  RichTextExtentRegs textExtent;
  GraphicsTextureSizeEaxEdxCf9 textureSize;
  
  if (control->rowCount != 0) {
    rowTop = (clipBottom - (control->base).top) / (int)control->rowHeight;
    if (rowTop < 0) {
      rowTop = 0;
    }
    rowSlot = control->rowSlots + rowTop;
    lastRowIndex =
         (dword)(((clipTop - (control->base).top) + (int)control->rowHeight) / (int)control->rowHeight);
    rowTop = rowTop * (int)control->rowHeight;
    if (control->rowCount <= lastRowIndex) {
      lastRowIndex = control->rowCount - 1;
    }
    lastRowSlot = control->rowSlots + (int)lastRowIndex;
    if (rowSlot <= lastRowSlot) {
      accessFailed = (*g_GraphicsFramebufferBeginAccess)();
      if (!accessFailed) {
        do {
          if (rowSlot == control->selectedRowSlot) {
            widthOrColumnCount = (control->base).layoutWidth;
            if (((control->base).nodeFlags & UI_NODE_HAS_KEYBOARD_FOCUS) == 0) {
              UiWindow_BlitTiledHorizontalEdge
                        (clipTop,clipLeft,clipBottom,clipRight,0x82,widthOrColumnCount,rowTop,0,control);
            }
            else {
              textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x83,g_UiWindowTextureSource);
              widthOrColumnCount = widthOrColumnCount - textureSize.logicalWidthPixels;
              UiWindow_BlitTiledHorizontalEdge
                        (clipTop,clipLeft,clipBottom,clipRight,0x84,widthOrColumnCount,rowTop,
                         textureSize.logicalWidthPixels,control);
              (*g_GraphicsTextureSourceBlitSourceAlpha)
                        (clipTop,clipLeft,clipBottom,clipRight,rowTop + (control->base).top,
                         (control->base).left,0x83,g_UiWindowTextureSource,g_FramebufferAccess);
              (*g_GraphicsTextureSourceBlitSourceAlpha)
                        (clipTop,clipLeft,clipBottom,clipRight,rowTop + (control->base).top,
                         widthOrColumnCount + (control->base).left,0x85,g_UiWindowTextureSource,
                         g_FramebufferAccess);
            }
          }
          widthOrColumnCount = control->columnCount;
          rowRecord = (byte *)*rowSlot;
          if (widthOrColumnCount != 0) {
            columnX = 3;
            column = control->columns;
            do {
              columnWidth = column->width;
              if (columnWidth < 0) {
                columnX = columnX - columnWidth;
                commandStream = (word *)(rowRecord + column->rowTextOffset);
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
                           (word *)(rowRecord + column->rowTextOffset),
                           rowTop + 1 + (control->base).top,(columnX - columnWidth) + (control->base).left);
              }
              column = column + 1;
              widthOrColumnCount = widthOrColumnCount + -1;
            } while (widthOrColumnCount != 0);
          }
          rowSlot = rowSlot + 1;
          rowTop = (int)control->rowHeight + rowTop;
        } while (rowSlot <= lastRowSlot);
        (*g_GraphicsFramebufferEndAccess)();
      }
    }
  }
  return;
}


/* Address: 0x004BB310.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004BA590[16]@004BA590.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime].
*/
void __thandor_preserve_eax UiListControl_TickActivationPulse(UiListControl *control)

{
  if (((control->listStateFlags & UI_LIST_DEFERRED_ACTION_PENDING) != 0) &&
     (control->listStateFlags = control->listStateFlags - 0x1000000,
     (control->listStateFlags & 0xff000000) == 0)) {
    control->listStateFlags = control->listStateFlags & 0xfffff9;
    UiActionQueue_Enqueue(control->actionId,control);
  }
  return;
}


/* Address: 0x004BB350.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004BA590[15]@004BA590.
   Cross-module calls: UiContainer_UnsuppressActionId [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiListControl_UnsuppressIfActionId(UiActionId actionId,UiListControl *control)

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
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004BA590[14]@004BA590.
   Cross-module calls: UiContainer_SuppressActionId [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiListControl_SuppressIfActionId(UiActionId actionId,UiListControl *control)

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
   Ownership: ui/controls/lists.
   Purpose: Initializes a pointer-list control and derives content width from the absolute values of its fixed
   column offsets before clearing scroll state and requesting parent layout.
   Cross-module calls: FontGlyph_GetLogicalSizeForStyleRegs [assets/text/resources].
*/
void __thandor_void_preserve_eax_ecx_edx
UiPointerList_InitializeColumnLayout
          (UiListRowCount rowCount,void **rowPointers,UiPointerListControl *control)

{
  int columnWidth;
  UiNodeVtable *parentVtable;
  dword columnsRemaining;
  UiNodeBase *parent;
  UiPixelExtent computedRowHeight;
  int totalWidth;
  UiListColumn *column;
  FontGlyphSizeEaxEdxCf9 glyphSize;

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
  for (; columnsRemaining != 0; columnsRemaining = columnsRemaining - 1) {
    columnWidth = column->width;
    if (columnWidth < 0) {
      columnWidth = -columnWidth;
    }
    totalWidth = columnWidth + totalWidth;
    column = column + 1;
  }
  parent = (control->base).parent;
  parentVtable = parent->vtable;
  (control->base).rightOffset = totalWidth;
  (control->base).leftOffset = 0;
  (control->base).topOffset = 0;
  (*parentVtable->layout)(parent);
  return;
}


/* Address: 0x004BB460.
   Ownership: ui/controls/lists.
   Purpose: EXACT_DUPLICATE_UI_POINTER_LIST_ROW_SLOT_GETTER_VARIANT_B.
*/
void ** UiPointerList_GetRowSlotsVariantB(UiPointerListControl *control)

{
  return control->rowSlots;
}


/* Address: 0x004BB9E0.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004BB990[0]@004BB990.
   Cross-module calls: UiContainer_RelocateChildren [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiTimedListControl_RelocateChildren
          (UiSerializedRelocationDelta relocationDelta,UiTimedListControl *control)

{
  UiContainer_RelocateChildren(relocationDelta,&control->base);
  return;
}


/* Address: 0x004BBA00.
   Ownership: ui/controls/lists.
   Purpose: Handles ui timed list control draw rows and selection.
   Cross-module calls: RichTextCommandStream_MeasureRegs [assets/text/richtext], UiWindow_BlitTiledHorizontalEdge
   [ui/controls/layout], RichTextCommandStream_DrawSingleLine [assets/text/richtext].
*/
void __thandor_void_preserve_eax_ecx_edx
UiTimedListControl_DrawRowsAndSelection
          (int clipTop,int clipLeft,int clipBottom,int clipRight,UiNodeBase *control)

{
  /* Rewritten from the assembly (0x004BBA00-0x004BBCCC). Expanded records push (record,
     remaining) on the machine stack and descend; the tree connector columns test the saved
     remaining counts of the parent levels. The decompiler kept only one level. Argument
     positions follow the original pushes. */
  enum { TREE_DEPTH_LIMIT = 64 };
  UiTimedListRuntimeExtendedView88 *list = (UiTimedListRuntimeExtendedView88 *)control;
  UiTimedListTreeRecord16 *savedRecord[TREE_DEPTH_LIMIT];
  dword savedRemaining[TREE_DEPTH_LIMIT];
  UiTimedListTreeRecord16 *header;
  UiTimedListTreeRecord16 *record;
  dword remaining;
  int depth;
  int rowY;
  int x;
  int y;
  int level;
  GraphicsTextureSourceAsset *rowTexture;

  if ((*g_GraphicsFramebufferBeginAccess)()) {
    return;
  }
  header = list->base.recordTree;
  if ((header != (UiTimedListTreeRecord16 *)0x0) && (header->recordCountOrRowPayload00 != 0)) {
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
            (*g_GraphicsTextureSourceBlitSourceAlpha)
                      (clipTop,clipLeft,clipBottom,clipRight,y,x,list->observedDrawParameter74,
                       rowTexture,g_FramebufferAccess);
          }
          x = x + (int)list->observedDrawParameter80;
        }
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,y,x,
                   (remaining <= 1) ? list->observedDrawParameter7C : list->observedDrawParameter78,
                   rowTexture,g_FramebufferAccess);
        if ((record->recordFlags0C & 1) != 0) {
          (*g_GraphicsTextureSourceBlitSourceAlpha)
                    (clipTop,clipLeft,clipBottom,clipRight,y,x,
                     ((record->recordFlags0C & 2) != 0) ? list->observedDrawParameter70 :
                                                          list->base.observedDrawParameter6C,
                     rowTexture,g_FramebufferAccess);
        }
        x = x + (int)list->observedDrawParameter80;
      }
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,y,x,record->rowPayload04,rowTexture,
                 g_FramebufferAccess);
      x = x + (int)list->observedDrawParameter84 - control->left;
      if (record == list->base.selectedRecord) {
        RichTextExtentRegs extent =
             RichTextCommandStream_MeasureRegs(g_UiListTextStyle,(word *)record->recordCountOrRowPayload00);
        int width = (int)extent.widthPixels + 6;
        if ((control->nodeFlags & 4) != 0) {
          GraphicsTextureSizeEaxEdxCf9 cap =
               (*g_GraphicsTextureSourceGetLogicalSize)(0x83,g_UiWindowTextureSource);
          int capWidth = (int)cap.logicalWidthPixels;
          int endX = width - capWidth + x;
          UiWindow_BlitTiledHorizontalEdge
                    (clipTop,clipLeft,clipBottom,clipRight,0x84,endX,rowY,capWidth + x,control);
          (*g_GraphicsTextureSourceBlitSourceAlpha)
                    (clipTop,clipLeft,clipBottom,clipRight,rowY + control->top,x + control->left,
                     0x83,g_UiWindowTextureSource,g_FramebufferAccess);
          (*g_GraphicsTextureSourceBlitSourceAlpha)
                    (clipTop,clipLeft,clipBottom,clipRight,rowY + control->top,endX + control->left,
                     0x85,g_UiWindowTextureSource,g_FramebufferAccess);
        }
        else {
          UiWindow_BlitTiledHorizontalEdge
                    (clipTop,clipLeft,clipBottom,clipRight,0x82,width + x,rowY,x,control);
        }
      }
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,g_UiListTextStyle,
                 (word *)record->recordCountOrRowPayload00,rowY + 1 + control->top,
                 x + 3 + control->left);
      rowY = rowY + (int)list->base.rowHeight;
      record = record + 1;
      remaining = remaining - 1;
      if (((record[-1].recordFlags0C & 1) != 0) && ((record[-1].recordFlags0C & 2) != 0) &&
          (record[-1].nestedRecordBlockOrParentLink08 != (UiTimedListTreeRecord16 *)0x0) &&
          (depth < TREE_DEPTH_LIMIT)) {
        UiTimedListTreeRecord16 *children = record[-1].nestedRecordBlockOrParentLink08;
        savedRecord[depth] = record;
        savedRemaining[depth] = remaining;
        depth = depth + 1;
        remaining = children->recordCountOrRowPayload00;
        record = children + 1;
      }
      while ((remaining == 0) && (depth != 0)) {
        depth = depth - 1;
        record = savedRecord[depth];
        remaining = savedRemaining[depth];
      }
    }
  }
  (*g_GraphicsFramebufferEndAccess)();
  return;
}


/* Address: 0x004BC180.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004BB990[16]@004BB990.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime].
*/
void __thandor_preserve_eax UiTimedListControl_TickActionDelay(UiTimedListControl *control)

{
  if (((control->listStateAndDelay & UI_TIMED_LIST_ACTION_DELAY_PENDING) != 0) &&
     (control->listStateAndDelay = control->listStateAndDelay - 0x1000000,
     (control->listStateAndDelay & 0xff000000) == 0)) {
    control->listStateAndDelay = control->listStateAndDelay & 0xfffffd;
    UiActionQueue_Enqueue(control->actionId,control);
  }
  return;
}


/* Address: 0x004BC3F0.
   Ownership: ui/controls/lists.
   Purpose: Returns the selected timed-list record.
*/
UiTimedListTreeRecord16 *
UiTimedListControl_GetSelectedRecord(UiTimedListRuntimeExtendedView88 *control)

{
  return (control->base).selectedRecord;
}

/* Address: 0x004BC460.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004BC410[0]@004BC410; g_UiNodeVtable_00517F10[0]@00517F10.
   Cross-module calls: UiContainer_RelocateChildren [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiListOffsetControl_RelocateAndApplyDeferredOffset
          (UiSerializedRelocationDelta relocationDelta,UiListOffsetControl *control)

{
  UiContainer_RelocateChildren(relocationDelta,&control->base);
  if ((control->labelFlags & 0x20) != 0) {
    control->text = (word *)((int)control->text + relocationDelta);
    control->labelFlags = control->labelFlags & 0xffffffdf;
  }
  return;
}


/* Address: 0x00516580.
   Ownership: ui/controls/lists.
   Purpose: Draws the inherited sprite state and catalog-specific numeric/status overlays derived from
   runtimeDisplayValueQ4 and active game-state tables.
   Cross-module calls: RichTextCommandStream_MeasureRegs [assets/text/richtext],
   RichTextCommandStream_DrawSingleLine [assets/text/richtext].
*/
void __thandor_void_preserve_eax_ecx_edx
UiCatalogEntryControl_DrawClipped
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiCatalogEntryControl *control)

{
  ArmyRuntimeSlot *slotArmyRuntime;
  dword subresourceOrTextLength;
  int recordIndexOrPercent;
  int assetCountOrPercent;
  FactionArmyAssetCount assetSlotIndex;
  int factionIndexOrPercent;
  bool accessFailed;
  RichTextExtentRegs textExtent;
  dword backgroundSubresource;
  GraphicsTextureSourceAsset *spriteTextureSource;
  SoftwareFramebufferAccess *framebuffer;
  PckArmyAssetIdCatalog catalogArmyAssetId;
  UiPackedTextStyle overlayTextStyle;
  ModelRuntimeNode *modelNode;
  ArmyRuntimeSlot *armyRuntime;
  ModelRuntimeNode *modelNodePrimary;
  
  if ((((control->command).sprite.selectable.base.nodeFlags & UI_NODE_SUPPRESSED) != 0) ||
     (((((control->command).sprite.selectable.stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0
       && (((control->command).sprite.selectable.stateFlags & 0x400) != 0)) ||
      (accessFailed = (*g_GraphicsFramebufferBeginAccess)(), accessFailed)))) {
    return;
  }
  spriteTextureSource = (control->command).sprite.primaryTextureSource;
  framebuffer = g_FramebufferAccess;
  if (((control->command).sprite.selectable.stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
    subresourceOrTextLength = (control->command).sprite.normalSubresourceStartOrDescriptor;
  }
  else {
    if ((((control->command).sprite.selectable.stateFlags & 0x80) == 0) &&
       (((control->command).sprite.selectable.stateFlags & 0x800) != 0)) {
      spriteTextureSource = (control->command).sprite.alternateTextureSource;
    }
    subresourceOrTextLength = (control->command).sprite.selectedSubresourceStart;
    if (((control->command).sprite.selectable.stateFlags & 0x40) != 0) {
      backgroundSubresource = (control->command).sprite.normalSubresourceStartOrDescriptor;
      if (((control->command).sprite.selectable.stateFlags & 0x80) != 0) {
        backgroundSubresource = backgroundSubresource + (control->command).sprite.animationFrameOffset;
      }
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,(control->command).sprite.selectable.base.top
                 ,(control->command).sprite.selectable.base.left,backgroundSubresource,
                 (control->command).sprite.primaryTextureSource,g_FramebufferAccess);
    }
  }
  if (((control->command).sprite.selectable.stateFlags & 0x80) != 0) {
    subresourceOrTextLength = subresourceOrTextLength + (control->command).sprite.animationFrameOffset;
  }
  (*g_GraphicsTextureSourceBlitSourceAlpha)
            (clipTop,clipLeft,clipBottom,clipRight,(control->command).sprite.selectable.base.top,
             (control->command).sprite.selectable.base.left,subresourceOrTextLength,spriteTextureSource,framebuffer);
  factionIndexOrPercent = (g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex;
  if ((int)g_GameFactionRuntimeImage.records[factionIndexOrPercent].xeniteCurrentQ4 <
      (int)control->runtimeDisplayValueQ4) {
    overlayTextStyle = 0x1050000;
  }
  else {
    overlayTextStyle = 0x1040000;
  }
  g_UiCatalogEntryRichTextScratchUtf16[0] = 0x20;
  g_UiCatalogEntryRichTextScratchUtf16[1] = 0;
  subresourceOrTextLength = (*g_WideNumberFormatUtf16)
                    (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,control->runtimeDisplayValueQ4 >> 4,
                     g_UiCatalogEntryRichTextScratchUtf16 + 1);
  *(undefined4 *)((int)g_UiCatalogEntryRichTextScratchUtf16 + subresourceOrTextLength + 2) = 0x20;
  textExtent = RichTextCommandStream_MeasureRegs(0x1000000,g_UiCatalogEntryRichTextScratchUtf16);
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,overlayTextStyle,g_UiCatalogEntryRichTextScratchUtf16,
             ((control->command).sprite.selectable.base.bottom - textExtent.heightPixels) + -2,
             ((int)((control->command).sprite.selectable.base.layoutWidth - textExtent.widthPixels) >> 1)
             + (control->command).sprite.selectable.base.left);
  recordIndexOrPercent = 0x29;
  do {
    if ((int)control - (int)g_InGameRuntimeRoot ==
        g_UiCatalogGroup42OffsetTables[g_UiCatalogGroup42ColumnCount][recordIndexOrPercent]) {
      assetCountOrPercent = 0;
      catalogArmyAssetId = g_UiCatalogGroup42Records[recordIndexOrPercent]->armyAssetId;
      for (assetSlotIndex = g_GameFactionRuntimeImage.records[factionIndexOrPercent].secondaryArmyAssetCount; assetSlotIndex != 0;
          assetSlotIndex = assetSlotIndex - 1) {
        if (g_UiCatalogGroup42Records[recordIndexOrPercent] ==
            *(UiCommandRuntimeRecordPrefix **)(factionIndexOrPercent * 0x740 + THANDOR_ADDR(g_GameFactionRuntimeImage,0xdc) + assetSlotIndex * 4)) {
          assetCountOrPercent = assetCountOrPercent + 1;
        }
      }
      if (assetCountOrPercent != 0) {
        g_UiCatalogEntryRichTextScratchUtf16[0] = 0x20;
        subresourceOrTextLength = (*g_WideNumberFormatUtf16)
                          (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,assetCountOrPercent,
                           g_UiCatalogEntryRichTextScratchUtf16 + 1);
        factionIndexOrPercent = (control->command).sprite.selectable.base.top;
        *(undefined4 *)((int)g_UiCatalogEntryRichTextScratchUtf16 + subresourceOrTextLength + 2) = 0x20;
        RichTextCommandStream_DrawSingleLine
                  (clipTop,clipLeft,clipBottom,clipRight,overlayTextStyle,
                   g_UiCatalogEntryRichTextScratchUtf16,factionIndexOrPercent + 2,
                   (control->command).sprite.selectable.base.left);
      }
      factionIndexOrPercent = -1;
      for (modelNodePrimary =
                (ModelRuntimeNode *)(g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
          modelNodePrimary != (ModelRuntimeNode *)0x0;
          modelNodePrimary = (ModelRuntimeNode *)(modelNodePrimary->common).nextNode) {
        if (((modelNodePrimary->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
            (armyRuntime = (modelNodePrimary->runtimePayload).armyRuntime,
            ((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C == 0xb))
           && (((armyRuntime->articulatedContact).fallbackPosition0Q12 == 1 &&
               (((((g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex ==
                   (armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex &&
                  (catalogArmyAssetId == armyRuntime->classState60)) &&
                 (recordIndexOrPercent = (int)(((longlong)(int)armyRuntime->ownerValue64 * 100) /
                               (longlong)(int)armyRuntime->ownerValue68), factionIndexOrPercent <= recordIndexOrPercent)) &&
                (overlayTextStyle = 0x1040000, factionIndexOrPercent = recordIndexOrPercent, (armyRuntime->runtimeFlags & 1) != 0)))))) {
          overlayTextStyle = 0x1050000;
        }
      }
      if (-1 < factionIndexOrPercent) {
        g_UiCatalogEntryRichTextScratchUtf16[0] = 0x20;
        subresourceOrTextLength = (*g_WideNumberFormatUtf16)
                          (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,factionIndexOrPercent,
                           g_UiCatalogEntryRichTextScratchUtf16 + 1);
        *(undefined2 *)((int)g_UiCatalogEntryRichTextScratchUtf16 + subresourceOrTextLength + 2) = 0x25;
        *(undefined4 *)((int)g_UiCatalogEntryRichTextScratchUtf16 + subresourceOrTextLength + 4) = 0x20;
        textExtent = RichTextCommandStream_MeasureRegs(0x1000000,g_UiCatalogEntryRichTextScratchUtf16);
        RichTextCommandStream_DrawSingleLine
                  (clipTop,clipLeft,clipBottom,clipRight,overlayTextStyle,
                   g_UiCatalogEntryRichTextScratchUtf16,
                   (control->command).sprite.selectable.base.top + 2,
                   (control->command).sprite.selectable.base.right - textExtent.widthPixels);
      }
      goto UiCatalogEntryControl_DrawClipped_EndFramebufferAccessAndReturn;
    }
    recordIndexOrPercent = recordIndexOrPercent + -1;
  } while (-1 < recordIndexOrPercent);
  recordIndexOrPercent = 0x2f;
  while ((int)control - (int)g_InGameRuntimeRoot !=
         g_UiCatalogGroup48OffsetTables[g_UiCatalogGroup48ColumnCount][recordIndexOrPercent]) {
    recordIndexOrPercent = recordIndexOrPercent + -1;
    if (recordIndexOrPercent < 0) goto UiCatalogEntryControl_DrawClipped_EndFramebufferAccessAndReturn;
  }
  assetCountOrPercent = 0;
  catalogArmyAssetId = g_UiCatalogGroup48Records[recordIndexOrPercent]->armyAssetId;
  for (assetSlotIndex = g_GameFactionRuntimeImage.records[factionIndexOrPercent].secondaryArmyAssetCount; assetSlotIndex != 0;
      assetSlotIndex = assetSlotIndex - 1) {
    if (g_UiCatalogGroup48Records[recordIndexOrPercent] ==
        *(UiCommandRuntimeRecordPrefix **)(factionIndexOrPercent * 0x740 + THANDOR_ADDR(g_GameFactionRuntimeImage,0xdc) + assetSlotIndex * 4)) {
      assetCountOrPercent = assetCountOrPercent + 1;
    }
  }
  if (assetCountOrPercent != 0) {
    g_UiCatalogEntryRichTextScratchUtf16[0] = 0x20;
    subresourceOrTextLength = (*g_WideNumberFormatUtf16)
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,assetCountOrPercent,
                       g_UiCatalogEntryRichTextScratchUtf16 + 1);
    factionIndexOrPercent = (control->command).sprite.selectable.base.top;
    *(undefined4 *)((int)g_UiCatalogEntryRichTextScratchUtf16 + subresourceOrTextLength + 2) = 0x20;
    RichTextCommandStream_DrawSingleLine
              (clipTop,clipLeft,clipBottom,clipRight,overlayTextStyle,g_UiCatalogEntryRichTextScratchUtf16,
               factionIndexOrPercent + 2,(control->command).sprite.selectable.base.left);
  }
  factionIndexOrPercent = (g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex;
  recordIndexOrPercent = -1;
  for (modelNode = (ModelRuntimeNode *)(g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
      modelNode != (ModelRuntimeNode *)0x0;
      modelNode = (ModelRuntimeNode *)(modelNode->common).nextNode) {
    if (modelNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
      slotArmyRuntime = (modelNode->runtimePayload).armyRuntime;
      if (((slotArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C == 0x16) {
        if ((((slotArmyRuntime->articulatedContact).terrainContactMode ==
              ARMY_TERRAIN_CONTACT_ADVANCE_ACTIVE_CONTACT_AND_RELEASE) &&
            (factionIndexOrPercent == (slotArmyRuntime->linkedEntityRuntime->common).ownership.ownerIndex)) &&
           ((catalogArmyAssetId == slotArmyRuntime->classState60 &&
            ((assetCountOrPercent = (int)(((longlong)(int)slotArmyRuntime->ownerValue64 * 100) /
                           (longlong)(int)slotArmyRuntime->ownerValue68), recordIndexOrPercent <= assetCountOrPercent &&
             (overlayTextStyle = 0x1040000, recordIndexOrPercent = assetCountOrPercent, (slotArmyRuntime->runtimeFlags & 1) != 0)))))) {
          overlayTextStyle = 0x1050000;
        }
      }
      else if ((((((slotArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C == 0xd)
                && ((slotArmyRuntime->articulatedContact).fallbackPosition0Q12 == 1)) &&
               (factionIndexOrPercent == (slotArmyRuntime->linkedEntityRuntime->common).ownership.ownerIndex)) &&
              (((catalogArmyAssetId == slotArmyRuntime->classState60 &&
                (assetCountOrPercent = (int)(((longlong)(int)slotArmyRuntime->ownerValue64 * 100) /
                              (longlong)(int)slotArmyRuntime->ownerValue68), recordIndexOrPercent <= assetCountOrPercent)) &&
               (overlayTextStyle = 0x1040000, recordIndexOrPercent = assetCountOrPercent, (slotArmyRuntime->runtimeFlags & 1) != 0)))) {
        overlayTextStyle = 0x1050000;
      }
    }
  }
  if (-1 < recordIndexOrPercent) {
    g_UiCatalogEntryRichTextScratchUtf16[0] = 0x20;
    subresourceOrTextLength = (*g_WideNumberFormatUtf16)
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,recordIndexOrPercent,
                       g_UiCatalogEntryRichTextScratchUtf16 + 1);
    *(undefined2 *)((int)g_UiCatalogEntryRichTextScratchUtf16 + subresourceOrTextLength + 2) = 0x25;
    *(undefined4 *)((int)g_UiCatalogEntryRichTextScratchUtf16 + subresourceOrTextLength + 4) = 0x20;
    textExtent = RichTextCommandStream_MeasureRegs(0x1000000,g_UiCatalogEntryRichTextScratchUtf16);
    RichTextCommandStream_DrawSingleLine
              (clipTop,clipLeft,clipBottom,clipRight,overlayTextStyle,g_UiCatalogEntryRichTextScratchUtf16,
               (control->command).sprite.selectable.base.top + 2,
               (control->command).sprite.selectable.base.right - textExtent.widthPixels);
  }
UiCatalogEntryControl_DrawClipped_EndFramebufferAccessAndReturn:
  (*g_GraphicsFramebufferEndAccess)();
  return;
}


/* Address: 0x00516B90.
   Ownership: ui/controls/lists.
   Purpose: Maps a hovered catalog control to one of two verified static catalog tables, updates the active catalog
   selection, and returns the pointer cursor identifier.
   Cross-module calls: InGameSelectionDetailPanel_Rebuild [ui/ingame/runtime].
*/
GraphicsCursorFrameIndex __thandor_eax_preserve_ecx_edx
UiCatalogEntryControl_PointerMove
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiCatalogEntryControl *control)

{
  GraphicsCursorFrameIndex cursorFrame;
  int recordIndex;
  int group48Index;
  
  if (((control->command).sprite.selectable.base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    recordIndex = 0x29;
    do {
      if ((int)control - (int)g_InGameRuntimeRoot ==
          g_UiCatalogGroup42OffsetTables[g_UiCatalogGroup42ColumnCount][recordIndex]) {
        g_UiHoverSelectionRecord = g_UiCatalogGroup42Records[recordIndex];
        InGameSelectionDetailPanel_Rebuild();
        goto UiCatalogEntryControl_PointerMove_ReturnCursorCodeAfterHoverResolution;
      }
      recordIndex = recordIndex + -1;
    } while (-1 < recordIndex);
    group48Index = 0x2f;
    do {
      if ((int)control - (int)g_InGameRuntimeRoot ==
          g_UiCatalogGroup48OffsetTables[g_UiCatalogGroup48ColumnCount][group48Index]) {
        g_UiHoverSelectionRecord = g_UiCatalogGroup48Records[group48Index];
        InGameSelectionDetailPanel_Rebuild();
        break;
      }
      group48Index = group48Index + -1;
    } while (-1 < group48Index);
  }
UiCatalogEntryControl_PointerMove_ReturnCursorCodeAfterHoverResolution:
  cursorFrame = 10;
  if ((g_KeyboardStateMask & 0xc) != 0) {
    cursorFrame = 0xc;
  }
  return cursorFrame;
}


/* Address: 0x00516C50.
   Ownership: ui/controls/lists.
   Purpose: Handles ui catalog entry control non right release.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime], UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
UiCatalogEntryControl_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiCatalogEntryControl *control)

{
  UiSelectableStateFlags *stateFlagsField;
  dword activationInputState;
  
  activationInputState = g_KeyboardStateMask;
  if ((((control->command).sprite.selectable.base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     (((control->command).sprite.selectable.stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0)) {
    stateFlagsField = &(control->command).sprite.selectable.stateFlags;
    *stateFlagsField = *stateFlagsField & ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    (control->command).activationInputState = activationInputState;
    if ((((control->command).sprite.selectable.stateFlags & 0x200) != 0) &&
       ((control->command).sprite.activationSoundId != 0)) {
      (*g_SoundPlayOneShot)
                (g_UiSoundGainQ15,g_UiSoundGainQ15,
                 (DirectSoundVoiceSet *)(control->command).sprite.activationSoundId);
    }
    UiActionQueue_Enqueue((control->command).sprite.selectable.actionId,control);
    UiNode_InvalidateRoot((UiNodeBase *)control);
  }
  return;
}


/* Address: 0x004BC360.
   Ownership: ui/controls/lists.
   Purpose: Handles ui timed list control select record and scroll into view.
   Local calls: UiTimedListTree_CountRecordArrayAndNestedChildren, UiScrollableControl_ClampOffsetsToViewport.
*/
void __thandor_void_preserve_eax_ecx_edx
UiTimedListControl_SelectRecordAndScrollIntoView
          (UiTimedListTreeRecord16 *selectedRecord,UiTimedListRuntimeExtendedView88 *control)

{
  dword nestedRecordCount;
  int rowAccumulator;
  int rowIndex;
  UiTimedListTreeRecord16 *previousRecord;
  
  rowAccumulator = 0;
  (control->base).selectedRecord = selectedRecord;
  do {
    previousRecord = selectedRecord + -1;
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
          selectedRecord != (UiTimedListTreeRecord16 *)0x0));
  rowIndex = rowIndex * (control->base).rowHeight;
  UiScrollableControl_ClampOffsetsToViewport
            (rowIndex + 1 + (control->base).rowHeight,(control->base).base.rightOffset,rowIndex,0,
             (UiScrollableControl *)(control->base).base.parent);
  return;
}


/* Address: 0x004BB4E0.
   Ownership: ui/controls/lists.
   Purpose: Selects an in-range pointer-list entry by storing base + index*4 at +0x60 and invalidates the
   corresponding row using the variant-B geometry calculation.
   Local calls: UiScrollableControl_ClampOffsetsToViewport.
*/
void __thandor_void_preserve_eax_ecx_edx
UiPointerList_SelectIndexVariantB(UiListRowIndex index,UiPointerListControl *control)

{
  int clipBottom;
  
  if (index < control->rowCount) {
    control->selectedRowSlot = control->rowSlots + index;
    clipBottom = control->rowHeight * index;
    UiScrollableControl_ClampOffsetsToViewport
              (clipBottom + 1 + control->rowHeight,(control->base).rightOffset,clipBottom,0,
               (UiScrollableControl *)(control->base).parent);
  }
  return;
}


/* Address: 0x004BB540.
   Ownership: ui/controls/lists.
   Purpose: Returns (+0x60 - +0x50)/4 in EAX. CF mirrors control flag 0x04 exactly.
*/
UiListRowIndexEaxCf5 __thandor_eax_cf_preserve_ecx_edx
UiPointerList_GetSelectedIndexVariantBCf(UiPointerListControl *control)

{
  UiListRowIndex selectedRowIndex;
  UiListRowIndexEaxCf5 unconfirmedResult;
  UiListRowIndexEaxCf5 confirmedResult;
  
  selectedRowIndex = (int)control->selectedRowSlot - (int)control->rowSlots >> 2;
  if ((control->listStateFlags & UI_LIST_SELECTION_CONFIRMED) == 0) {
    unconfirmedResult.carry = false;
    unconfirmedResult.rowIndex = selectedRowIndex;
    return unconfirmedResult;
  }
  confirmedResult.carry = true;
  confirmedResult.rowIndex = selectedRowIndex;
  return confirmedResult;
}


/* Address: 0x004B9460.
   Ownership: ui/controls/lists.
   Purpose: Returns the verified scrollable-content width and height fields through EAX and EDX when the control
   uses the expected scrollable vtable, otherwise returns zero for both dimensions.
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
  return CONCAT44(contentHeight,contentWidth);
}

/* Address: 0x004BC1C0.
   Ownership: ui/controls/lists.
   Purpose: Sets the timed-list record tree and recomputes layout.
*/
void __thandor_void_preserve_eax_ecx_edx
UiTimedListControl_SetRecordTreeAndRecomputeLayout
          (UiTimedListTreeRecord16 *recordTree,UiTimedListRuntimeExtendedView88 *control)

{
  /* Rewritten from the assembly (0x004BC1C0): expanded records with children push their position
     on the machine stack and descend; the decompiler kept only one level. */
  enum { TREE_DEPTH_LIMIT = 64 };
  UiTimedListTreeRecord16 *savedRecord[TREE_DEPTH_LIMIT];
  dword savedRemaining[TREE_DEPTH_LIMIT];
  UiTimedListTreeRecord16 *record;
  UiNodeBase *parent;
  FontGlyphSizeEaxEdxCf9 glyph;
  dword remaining;
  uint widest;
  uint width;
  int depth;

  glyph = FontGlyph_GetLogicalSizeActiveRegs(0);
  remaining = (recordTree == (UiTimedListTreeRecord16 *)0x0) ? 0 : recordTree->recordCountOrRowPayload00;
  (control->base).rowHeight = glyph.lineHeight + 1;
  (control->base).rowCount = remaining;
  (control->base).recordTree = recordTree;
  record = recordTree + 1;
  (control->base).selectedRecord = record;
  widest = 0;
  depth = 0;
  while (remaining != 0) {
    RichTextExtentRegs extent =
         RichTextCommandStream_MeasureRegs(g_UiListTextStyle,(word *)record->recordCountOrRowPayload00);
    width = extent.widthPixels + control->observedDrawParameter84 +
            control->observedDrawParameter80 * (dword)depth;
    record = record + 1;
    remaining = remaining - 1;
    if (widest < width) {
      widest = width;
    }
    if (((record[-1].recordFlags0C & UI_TIMED_LIST_RECORD_OBSERVED_BIT0) != 0) &&
        ((record[-1].recordFlags0C & UI_TIMED_LIST_RECORD_ENABLES_NESTED_CHILD_TRAVERSAL) != 0) &&
        (record[-1].nestedRecordBlockOrParentLink08 != (UiTimedListTreeRecord16 *)0x0) &&
        (depth < TREE_DEPTH_LIMIT)) {
      UiTimedListTreeRecord16 *children = record[-1].nestedRecordBlockOrParentLink08;
      savedRecord[depth] = record;
      savedRemaining[depth] = remaining;
      depth = depth + 1;
      remaining = children->recordCountOrRowPayload00;
      record = children + 1;
      (control->base).rowCount = (control->base).rowCount + remaining;
    }
    while ((remaining == 0) && (depth != 0)) {
      depth = depth - 1;
      record = savedRecord[depth];
      remaining = savedRemaining[depth];
    }
  }
  parent = (control->base).base.parent;
  (control->base).base.rightOffset = widest + 6;
  (control->base).base.leftOffset = 0;
  (control->base).base.topOffset = 0;
  (control->base).base.bottomOffset = (control->base).rowCount * (control->base).rowHeight + 1;
  (*parent->vtable->layout)(parent);
  return;
}

/* Address: 0x004BC2F0.
   Ownership: ui/controls/lists.
   Purpose: Returns the timed-list record tree.
*/
UiTimedListTreeRecord16 * UiTimedListControl_GetRecordTree(UiTimedListControl *control)

{
  return control->recordTree;
}

/* Address: 0x004BC310.
   Ownership: ui/controls/lists.
   Purpose: Handles ui timed list tree count record array and nested children.
*/
dword __thandor_eax_preserve_ecx_edx
UiTimedListTree_CountRecordArrayAndNestedChildren(UiTimedListTreeRecord16 *recordBlock)

{
  dword nestedRecordCount;
  dword totalCount;
  dword recordsRemaining;
  
  totalCount = 0;
  if (recordBlock != (UiTimedListTreeRecord16 *)0x0) {
    totalCount = recordBlock->recordCountOrRowPayload00;
    recordsRemaining = totalCount;
    do {
      if ((recordBlock[1].recordFlags0C & UI_TIMED_LIST_RECORD_ENABLES_NESTED_CHILD_TRAVERSAL) != 0)
      {
        nestedRecordCount = UiTimedListTree_CountRecordArrayAndNestedChildren
                          (recordBlock[1].nestedRecordBlockOrParentLink08);
        totalCount = totalCount + nestedRecordCount;
      }
      recordsRemaining = recordsRemaining - 1;
      recordBlock = recordBlock + 1;
    } while (recordsRemaining != 0);
  }
  return totalCount;
}


/* Address: 0x004B9170.
   Ownership: ui/controls/lists.
   Purpose: Refreshes the active child rectangle from the current scroll offsets and recomputes the enabled
   scrollbar tracks and thumb positions without repeating the complete viewport-selection pass.
*/
void __thandor_void_preserve_eax_ecx_edx
UiScrollableControl_RefreshChildAndScrollThumbs(UiScrollableControl *control)

{
  UiNodeBase *contentChild;
  UiPixelExtent childWidth;
  UiPixelExtent childHeight;
  uint minThumbLength;
  dword arrowSize;
  uint thumbLength;
  UiPixelOffset offsetX;
  UiPixelOffset offsetY;
  int horizontalExtent;
  int verticalExtent;
  GraphicsTextureSizeEaxEdxCf9 textureSize;
  
  contentChild = (control->base).firstChild;
  if (contentChild != (UiNodeBase *)0xffffffff) {
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
    (*contentChild->vtable->layout)(contentChild);
    control->horizontalThumbLeft = 0;
    control->verticalThumbTop = 0;
    control->horizontalThumbRight = 0;
    control->verticalThumbBottom = 0;
    if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) != 0) {
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
      control->verticalThumbTop = control->verticalThumbTop + textureSize.logicalHeightPixels;
      control->verticalThumbBottom = control->verticalThumbBottom + textureSize.logicalHeightPixels;
    }
    if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) != 0) {
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
      control->horizontalThumbLeft = control->horizontalThumbLeft + textureSize.logicalWidthPixels;
      control->horizontalThumbRight = control->horizontalThumbRight + textureSize.logicalWidthPixels;
    }
    horizontalExtent = (control->base).layoutWidth;
    verticalExtent = (control->base).layoutHeight;
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
      arrowSize = textureSize.logicalWidthPixels;
      control->horizontalThumbLeft = control->horizontalThumbLeft + arrowSize;
      control->horizontalThumbRight = control->horizontalThumbRight + arrowSize;
      horizontalExtent = horizontalExtent + arrowSize * -2;
      verticalExtent = verticalExtent - textureSize.logicalHeightPixels;
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
      arrowSize = textureSize.logicalHeightPixels;
      control->verticalThumbTop = control->verticalThumbTop + arrowSize;
      control->verticalThumbBottom = control->verticalThumbBottom + arrowSize;
      verticalExtent = verticalExtent + arrowSize * -2;
      horizontalExtent = horizontalExtent - textureSize.logicalWidthPixels;
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
      thumbLength = (uint)(((longlong)(int)control->viewportWidth * (longlong)horizontalExtent) /
                    (longlong)(int)control->contentWidth);
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0xc0,g_UiWindowTextureSource);
      minThumbLength = textureSize.logicalWidthPixels * 2;
      if (thumbLength < minThumbLength) {
        thumbLength = minThumbLength;
      }
      control->horizontalThumbRight = control->horizontalThumbRight + thumbLength;
      horizontalExtent = (int)(((longlong)(int)-control->scrollOffsetX * (longlong)(int)(horizontalExtent - thumbLength)) /
                   (longlong)(int)(control->contentWidth - control->viewportWidth));
      control->horizontalThumbLeft = control->horizontalThumbLeft + horizontalExtent;
      control->horizontalThumbRight = control->horizontalThumbRight + horizontalExtent;
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      thumbLength = (uint)(((longlong)(int)control->viewportHeight * (longlong)verticalExtent) /
                    (longlong)(int)control->contentHeight);
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0xc2,g_UiWindowTextureSource);
      minThumbLength = textureSize.logicalHeightPixels * 2;
      if (thumbLength < minThumbLength) {
        thumbLength = minThumbLength;
      }
      control->verticalThumbBottom = control->verticalThumbBottom + thumbLength;
      horizontalExtent = (int)(((longlong)(int)-control->scrollOffsetY * (longlong)(int)(verticalExtent - thumbLength)) /
                   (longlong)(int)(control->contentHeight - control->viewportHeight));
      control->verticalThumbTop = control->verticalThumbTop + horizontalExtent;
      control->verticalThumbBottom = control->verticalThumbBottom + horizontalExtent;
    }
  }
  return;
}


/* Address: 0x004B9490.
   Ownership: ui/controls/lists.
   Purpose: Clamps enabled horizontal and vertical scroll offsets to the visible viewport, relayouts when an offset
   changes, and invalidates the owning UI root. Typed parameters: p0 clipTop→UiPixelCoordinate_V297, p1
   clipLeft→UiPixelCoordinate_V297, p2 clipBottom→UiPixelCoordinate_V297, p3 clipRight→UiPixelCoordinate_V297.
   Calling convention, parameter storage, body bytes, control flow, globals, locals, and executable data remain
   unchanged.
   Local calls: UiScrollableControl_RefreshChildAndScrollThumbs.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
UiScrollableControl_ClampOffsetsToViewport
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiScrollableControl *control)

{
  char changeCount;
  int viewLeftOrOverflow;
  int viewTop;
  int viewBottom;
  int viewRight;
  int horizontalOverflow;
  
  if ((control->base).vtable == &g_UiScrollableControlVtable) {
    viewLeftOrOverflow = -control->scrollOffsetX;
    viewTop = -control->scrollOffsetY;
    changeCount = '\0';
    viewRight = control->viewportWidth + viewLeftOrOverflow;
    viewBottom = control->viewportHeight + viewTop;
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
      horizontalOverflow = viewRight - clipLeft;
      changeCount = viewRight < clipLeft;
      if ((bool)changeCount) {
        control->scrollOffsetX = control->scrollOffsetX + horizontalOverflow;
        viewLeftOrOverflow = viewLeftOrOverflow - horizontalOverflow;
      }
      if (viewLeftOrOverflow - clipRight != 0 && clipRight <= viewLeftOrOverflow) {
        changeCount = changeCount + '\x01';
        control->scrollOffsetX = control->scrollOffsetX + (viewLeftOrOverflow - clipRight);
      }
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      viewLeftOrOverflow = viewBottom - clipTop;
      if (viewBottom < clipTop) {
        control->scrollOffsetY = control->scrollOffsetY + viewLeftOrOverflow;
        viewTop = viewTop - viewLeftOrOverflow;
        changeCount = changeCount + '\x01';
      }
      if (viewTop - clipBottom != 0 && clipBottom <= viewTop) {
        changeCount = changeCount + '\x01';
        control->scrollOffsetY = control->scrollOffsetY + (viewTop - clipBottom);
      }
    }
    if (changeCount != '\0') {
      UiScrollableControl_RefreshChildAndScrollThumbs(control);
    }
    UiNode_InvalidateRoot(&control->base);
  }
  return;
}

