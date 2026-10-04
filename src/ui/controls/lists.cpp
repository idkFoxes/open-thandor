/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/lists.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/lists.h>
#include <thandor/thandor.h>
#include <stdarg.h>

/* Module data. */

/* UiFrameDelayFrames, 8: frames of the activation pulse after Enter on a list/text list before its action is queued (src/ui/controls/lists.c, text.c). */
const UiFrameDelayFrames g_UiListActivationPulseFrames = 8;

const uint32_t g_UiListTextStyle = 0;

static void *const g_Utf16StringCompareAsciiCaseInsensitiveFlags = THANDOR_FN(Utf16String_CompareAsciiCaseInsensitiveFlags);

/* 1 KiB expansion scratch of UiPointerList_CompareExpandedText */
static uint16_t g_UiPointerListExpandedLeftTextUtf16[512] = {0};

/* 1 KiB expansion scratch of UiPointerList_CompareExpandedText */
static uint16_t g_UiPointerListExpandedRightTextUtf16[512] = {0};

/* Implementation ownership: ui/controls/lists. */

/* True for the keys that move a list's selection (Home/End, Page Up/Down, Up/Down). */
static bool UiList_IsNavigationKey(UiKeyboardEventCode keyCode)
{
  return (keyCode == KEYBOARD_KEY_CODE_HOME) || (keyCode == KEYBOARD_KEY_CODE_END) ||
         (keyCode == KEYBOARD_KEY_CODE_PAGE_UP) || (keyCode == KEYBOARD_KEY_CODE_PAGE_DOWN) ||
         (keyCode == KEYBOARD_KEY_CODE_UP) || (keyCode == KEYBOARD_KEY_CODE_DOWN);
}

/* Keyboard handler of the column list (g_UiListControlVtable keyboardEvent): Enter confirms the selection
   and queues the list's action at once; Home/End, Up/Down and Page Up/Down (one row less than the view
   holds) move the selection, play the selection sound, scroll the row into view and queue the action after
   g_UiListActivationPulseFrames frames (UiListControl_TickActivationPulse). Other keys go to the default
   focus handling; the keys handled here return false.
*/
Bool8 UiListControl_HandleKeyboardNavigation
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiListControl *control)

{
  Ptr32<void> *previousSelectedSlot;
  Ptr32<void> *newSelectedSlot;
  int rowValue;
  uint32_t targetRowIndex;
  Bool8 handled;
  UiScrollableViewportSize viewportSize;
  
  previousSelectedSlot = control->selectedRowSlot;
  if ((keyCode & KEYBOARD_KEY_CODE_FAMILY_MASK) != 0) { /* not a plain character */
    if (keyCode == KEYBOARD_KEY_CODE_ENTER) {
      control->listStateFlags = control->listStateFlags | UI_LIST_SELECTION_CONFIRMED;
      UiActionQueue_Enqueue(control->actionId,control);
    }
    else if ((control->rowCount == 0) && UiList_IsNavigationKey(keyCode)) {
      /* The original computes row rowCount - 1 (End, Page Down) or the slot before rowSlots (Up, with
         rowSlots NULL) on an empty list and stores that meaningless slot; ignored here because on x64 the
         slot does not fit the 32-bit field (Thandor_Ptr32Overflow ends the game). */
      return false;
    }
    else if (keyCode == KEYBOARD_KEY_CODE_HOME) {
      control->selectedRowSlot = control->rowSlots;
    }
    else if (keyCode == KEYBOARD_KEY_CODE_END) {
      control->selectedRowSlot = control->rowSlots + (control->rowCount - 1);
    }
    else if (keyCode == KEYBOARD_KEY_CODE_PAGE_UP) {
      viewportSize = UiScrollableControl_GetViewportSize((UiScrollableControl *)(control->base).parent);
      rowValue = ((uint32_t)(((uintptr_t)control->selectedRowSlot - (uintptr_t)control->rowSlots) / sizeof(Ptr32<void>))) -
              ((int)(viewportSize.height / control->rowHeight) - 1);
      if (rowValue < 0) {
        rowValue = 0;
      }
      control->selectedRowSlot = control->rowSlots + rowValue;
    }
    else if (keyCode == KEYBOARD_KEY_CODE_PAGE_DOWN) {
      viewportSize = UiScrollableControl_GetViewportSize((UiScrollableControl *)(control->base).parent);
      targetRowIndex = ((uint32_t)(((uintptr_t)control->selectedRowSlot - (uintptr_t)control->rowSlots) / sizeof(Ptr32<void>))) +
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
      if (((uint32_t)(((uintptr_t)control->selectedRowSlot - (uintptr_t)control->rowSlots) / sizeof(Ptr32<void>))) + 1 <
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
         (control->activationSound != nullptr)) {
        g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound,nullptr);
      }
      rowValue = ((uint32_t)(((uintptr_t)newSelectedSlot - (uintptr_t)control->rowSlots) / sizeof(Ptr32<void>))) * control->rowHeight;
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

/* After the rows of a pointer list changed: sets the content height to rowCount rows (+1 pixel), lets the
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
  selectedIndex = UiPointerList_GetSelectedIndexAndConfirmed(control,nullptr);
  UiPointerList_SelectColumnListIndex(selectedIndex,control);
  UiActionQueue_Enqueue(control->actionId,control);
  return;
}

/* Left-button press on the column list (g_UiListControlVtable nonRightPress): selects the row under the
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
         (control->activationSound != nullptr)) {
        g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound,nullptr);
      }
    }
  }
  return;
}

/* Sorts the rows of a pointer list by a 64-bit key at fieldOffset in each row entry (high dword first, then
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
  Ptr32<void> *pivotSlot;
  Ptr32<void> *scanSlot;

  if (control->rowSlots == nullptr) {
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

/* Sorts the rows of a pointer list by the unsigned dword at fieldOffset in each row entry with an exchange
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
  Ptr32<void> *pivotSlot;
  Ptr32<void> *scanSlot;

  if (control->rowSlots == nullptr) {
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

/* Draws the visible rows of the column list (g_UiListControlVtable drawClipped): the highlight bar behind
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
  Ptr32<void> *lastRowSlot;
  int highlightWidth;
  int columnsRemaining;
  UiListColumn *column;
  Ptr32<void> *rowSlot;
  uint16_t *commandStream;
  Bool8 accessFailed;
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

/* Per-frame tick of the column list (g_UiListControlVtable tick): counts down the deferred action of a
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

/* Re-enables the column list when it is bound to actionId (g_UiListControlVtable unsuppressActionId), then
   passes the request on to its children like any container. Also the unsuppressActionId of the text list
   (g_UiTextListControlVtable; UiTextListControl has actionId at the same offset): the original has a copy
   for it (UiTextListControl_UnsuppressIfActionId).
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

/* Disables the column list when it is bound to actionId (g_UiListControlVtable suppressActionId), then
   passes the request on to its children like any container. Unlike the selectable controls it keeps the
   keyboard focus. Also the suppressActionId of the text list (g_UiTextListControlVtable; the original has a
   copy for it, UiTextListControl_SuppressIfActionId).
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

/* Fills a pointer list with rowCount rows (rowPointers, one record pointer per row) and selects row 0.
   The list's size follows its content: one list-font line plus 1 pixel per row, and the sum of the column
   widths (negative widths count by their magnitude) plus 6 pixels; the parent (the scrollable frame) is
   laid out again for the new size.
*/
void UiPointerList_InitializeColumnLayout(UiListRowCount rowCount,Ptr32<void> *rowPointers,UiPointerListControl *control)

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

/* Selects row index of a pointer list (without queueing its action) and scrolls the list's scrollable
   parent so the row is visible. Out-of-range indices are ignored. The original has a second copy for the
   text lists (UiPointerList_SelectTextListIndex).
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

/* Returns the index of the selected row of a pointer list. *outConfirmed (optional, may be NULL) tells
   whether the selection was confirmed (UI_LIST_SELECTION_CONFIRMED, set by a double click on the row).
   The original has a second copy for the text lists (UiPointerList_GetSelectedIndex) that reports the flag
   in a register no caller reads.
*/
UiListRowIndex UiPointerList_GetSelectedIndexAndConfirmed(UiPointerListControl *control,Bool8 *outConfirmed)

{
  if (outConfirmed != nullptr) {
    *outConfirmed = (control->listStateFlags & UI_LIST_SELECTION_CONFIRMED) != 0;
  }
  return control->selectedRowSlot - control->rowSlots;
}

UiNodeVtable g_UiListOffsetControlVtable = {
        .relocate = THANDOR_FN(UiWrappedTextControl_RelocateAndApplyDeferredOffset),
        .method04 = THANDOR_FN(UiNode_DefaultMethod04_NoOp),
        .drawClipped = THANDOR_FN(UiWrappedTextControl_DrawClipped),
        .layout = THANDOR_FN(UiContainer_LayoutChildren),
        .nonRightPress = THANDOR_FN(UiNode_DefaultNonRightPress),
        .nonRightRelease = THANDOR_FN(UiNode_DefaultNonRightRelease),
        .rightPress = THANDOR_FN(UiNode_ForwardRightPressToParent),
        .rightRelease = THANDOR_FN(UiNode_DefaultRightRelease),
        .nonRightDrag = THANDOR_FN(UiNode_DefaultNonRightDrag),
        .rightDrag = THANDOR_FN(UiNode_DefaultRightDrag),
        .pointerMove = THANDOR_FN(UiNode_DefaultPointerMove),
        .hitTest = THANDOR_FN(UiContainer_HitTestChildren),
        .keyboardEvent = THANDOR_FN(UiNode_DefaultKeyboardEventMoveFocusNext),
        .applyFlags = THANDOR_FN(UiNode_ApplyFlagsRecursive),
        .suppressActionId = THANDOR_FN(UiContainer_SuppressActionId),
        .unsuppressActionId = THANDOR_FN(UiContainer_UnsuppressActionId),
        .tick = THANDOR_FN(UiNode_DefaultTick),
        .pointerWheel = THANDOR_FN(UiNode_ForwardPointerWheelToParent)};

/* Tail of UiPointerList_SortByExpandedTextFieldAscending: selects the row that now holds selectedRecord (the first row if it
   is gone) and scrolls that row into view. */
static void UiPointerList_ReselectRecordAfterSort(void *selectedRecord,UiPointerListControl *control)

{
  Ptr32<void> *rowSlotCursor;
  UiListRowCount remainingRows;
  int selectedRowTop;

  rowSlotCursor = control->rowSlots;
  remainingRows = control->rowCount;
  selectedRowTop = 0;
  do {
    if (selectedRecord == *rowSlotCursor) break;
    selectedRowTop = selectedRowTop + control->rowHeight;
    rowSlotCursor++;
    remainingRows--;
  } while (remainingRows != 0);
  if (remainingRows == 0) {
    /* Not found: select the first row (the row top stays past the last row, as in the original). */
    rowSlotCursor = control->rowSlots;
  }
  control->selectedRowSlot = rowSlotCursor;
  UiScrollableControl_ClampOffsetsToViewport
            (selectedRowTop + 1 + control->rowHeight,(control->base).rightOffset,selectedRowTop,0,
             (UiScrollableControl *)(control->base).parent);
}

/* Sorts the rows of a pointer list in ascending order of the rich text found fieldOffset bytes into each row
   record (expanded, ASCII case-insensitive), with an exchange sort that moves the smallest remaining row to
   the front in each pass. The previously selected record stays selected and is scrolled into view. Called
   by the scenario catalogue (assets/scenario/catalog.c, field offset 0x74).
*/
void UiPointerList_SortByExpandedTextFieldAscending
          (UiPointerListFieldByteOffset fieldOffset,UiPointerListControl *control)

{
  void *swappedRecord;
  int lastRowIndex;
  int comparisonsLeft;
  int remainingPasses;
  Ptr32<void> *passAnchorSlot;
  Ptr32<void> *rowSlotCursor;
  int textOrder;
  void *selectedRecord;

  if (control->rowSlots != nullptr) {
    lastRowIndex = control->rowCount - 1;
    if ((lastRowIndex != 0) && (-1 < lastRowIndex)) {
      selectedRecord = *control->selectedRowSlot;
      passAnchorSlot = control->rowSlots;
      for (remainingPasses = lastRowIndex; remainingPasses != 0; remainingPasses--) {
        rowSlotCursor = passAnchorSlot;
        for (comparisonsLeft = remainingPasses; comparisonsLeft != 0; comparisonsLeft--) {
          rowSlotCursor++;
          textOrder = UiPointerList_CompareExpandedText
                            ((uint16_t *)((uint8_t *)*rowSlotCursor + fieldOffset),
                             (uint16_t *)((uint8_t *)*passAnchorSlot + fieldOffset));
          if (textOrder >= 0) {
            LOCK();
            swappedRecord = *rowSlotCursor;
            *rowSlotCursor = *passAnchorSlot;
            UNLOCK();
            *passAnchorSlot = swappedRecord;
          }
        }
        passAnchorSlot++;
      }
      UiPointerList_ReselectRecordAfterSort(selectedRecord,control);
    }
  }
}

/* Draws the visible rows of a text list (drawClipped slot of g_UiTextListControlVtable): each row's rich
   text in the list style, the selected row over a highlight bar as wide as its text plus 6 pixels (with end
   caps while the list has keyboard focus).
*/
void UiTextListControl_DrawRowsAndSelection
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiTextListControl *control)

{
  int firstVisibleRow;
  int rowTop;
  uint32_t lastVisibleRow;
  int highlightWidth;
  Ptr32<uint16_t> *lastRowSlot;
  Ptr32<uint16_t> *rowSlot;
  Bool8 framebufferUnavailable;
  RichTextExtent rowExtent;
  GraphicsTextureLogicalSize capSize;

  if (control->rowCount != 0) {
    firstVisibleRow = (clipTop - (control->base).top) / (int)control->rowHeight;
    if (firstVisibleRow < 0) {
      firstVisibleRow = 0;
    }
    rowSlot = control->rowTextSlots + firstVisibleRow;
    lastVisibleRow = (int)((clipBottom - (control->base).top) + control->rowHeight) / (int)control->rowHeight;
    rowTop = firstVisibleRow * control->rowHeight;
    if (control->rowCount <= lastVisibleRow) {
      lastVisibleRow = control->rowCount - 1;
    }
    lastRowSlot = control->rowTextSlots + lastVisibleRow;
    if (rowSlot <= lastRowSlot) {
      framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
      if (!framebufferUnavailable) {
        do {
          if (rowSlot == control->selectedRowSlot) {
            rowExtent = RichTextCommandStream_MeasureLine(g_UiListTextStyle,*rowSlot);
            highlightWidth = rowExtent.widthPixels + 6;
            if (((control->base).nodeFlags & UI_NODE_HAS_KEYBOARD_FOCUS) == 0) {
              UiWindow_BlitTiledHorizontalEdge
                        (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_LIST_SELECTION,highlightWidth,
                         rowTop,0,control);
            }
            else {
              capSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_LIST_FOCUS_SELECTION_LEFT,
                                                              g_UiWindowTextureSource);
              highlightWidth = highlightWidth - capSize.logicalWidthPixels;
              UiWindow_BlitTiledHorizontalEdge
                        (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_LIST_FOCUS_SELECTION_MIDDLE,
                         highlightWidth,rowTop,
                         capSize.logicalWidthPixels,control);
              g_GraphicsTextureSourceBlitSourceAlpha
                        (clipBottom,clipRight,clipTop,clipLeft,rowTop + (control->base).top,
                         (control->base).left,UI_WINDOW_SUBRESOURCE_LIST_FOCUS_SELECTION_LEFT,g_UiWindowTextureSource,
                         g_FramebufferAccess);
              g_GraphicsTextureSourceBlitSourceAlpha
                        (clipBottom,clipRight,clipTop,clipLeft,rowTop + (control->base).top,
                         highlightWidth + (control->base).left,UI_WINDOW_SUBRESOURCE_LIST_FOCUS_SELECTION_RIGHT,
                         g_UiWindowTextureSource,
                         g_FramebufferAccess);
            }
          }
          RichTextCommandStream_DrawSingleLine
                    (clipBottom,clipRight,clipTop,clipLeft,g_UiListTextStyle,*rowSlot,
                     rowTop + 1 + (control->base).top,(control->base).left + 3);
          rowSlot++;
          rowTop = rowTop + control->rowHeight;
        } while (rowSlot <= lastRowSlot);
        g_GraphicsFramebufferEndAccess();
      }
    }
  }
  return;
}

/* Primary button press on a text list (nonRightPress slot of g_UiTextListControlVtable): a click on the text
   of a row (its width plus 6 pixels) selects that row, scrolls it into view, queues the action and plays the
   selection sound when enabled. A double click also marks the selection confirmed and repeats the action for
   the already selected row; a single click on it does nothing.
*/
void UiTextListControl_SelectRowFromPointer
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiTextListControl *control)

{
  uint32_t rowIndex;
  int controlLeft;
  int rowTop;
  RichTextExtent rowExtent;
  Ptr32<uint16_t> *clickedRowSlot;

  if (((control->base).top > pointerY) || ((control->base).left > pointerX)) {
    return;
  }
  controlLeft = (control->base).left;
  rowIndex = (uint32_t)(pointerY - (control->base).top) / control->rowHeight;
  if (rowIndex >= control->rowCount) {
    return;
  }
  clickedRowSlot = control->rowTextSlots + rowIndex;
  rowExtent = RichTextCommandStream_MeasureLine(g_UiListTextStyle,*clickedRowSlot);
  if (pointerX - controlLeft >= (int)(rowExtent.widthPixels + 6)) {
    return;
  }
  control->listStateFlags = control->listStateFlags | UI_TEXT_LIST_SELECTION_CONFIRMED;
  if (((control->base).nodeFlags & UI_NODE_REPEAT_OR_DOUBLE_CLICK) == 0) {
    /* A single click: not confirmed, and nothing to do on the already selected row. */
    control->listStateFlags = control->listStateFlags & ~UI_TEXT_LIST_SELECTION_CONFIRMED;
    if (clickedRowSlot == control->selectedRowSlot) {
      return;
    }
  }
  control->selectedRowSlot = clickedRowSlot;
  rowTop = rowIndex * control->rowHeight;
  UiScrollableControl_ClampOffsetsToViewport
            (rowTop + 1 + control->rowHeight,(control->base).rightOffset,rowTop,0,
             (UiScrollableControl *)(control->base).parent);
  UiActionQueue_Enqueue(control->actionId,control);
  if (((control->listStateFlags & UI_TEXT_LIST_PLAY_SELECTION_SOUND) != 0) &&
     (control->activationSound != nullptr)) {
    g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound,nullptr);
  }
}

/* Keyboard handler of a text list (keyboardEvent slot of g_UiTextListControlVtable): with type search, a
   character selects the first row whose first code unit is not below it, ASCII case-insensitive (meant for
   sorted lists; the last row when there is none); Enter confirms the selection
   and queues the action; Home/End/Page Up/Page Down/Up/Down move the selection. A changed selection plays
   the selection sound when enabled, is scrolled into view and arms the deferred action, which
   UiTextListControl_TickActivationPulse queues after g_UiListActivationPulseFrames frames. Other keys go to
   UiNode_DefaultKeyboardEventMoveFocusNext. Returns false: consumed.
*/
Bool8 UiTextListControl_HandleKeyboardNavigationAndSearch
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiTextListControl *control)

{
  Ptr32<uint16_t> *previousSelectedSlot;
  Ptr32<uint16_t> *scanSlot;
  Ptr32<uint16_t> *selectedSlot;
  int pageUpRow;
  int selectedRowTop;
  int pulseFrames;
  uint32_t pageDownRow;
  UiListRowCount remainingRows;
  Ptr32<uint16_t> *candidateSlot;
  Bool8 rowBelowKey;
  UiScrollableViewportSize viewportSize;

  previousSelectedSlot = control->selectedRowSlot;
  if ((keyCode & KEYBOARD_KEY_CODE_FAMILY_MASK) == 0) {
    if (((keyboardStateMask & (KEYBOARD_STATE_CTRL | KEYBOARD_STATE_ALT)) != 0) ||
       ((control->listStateFlags & UI_TEXT_LIST_TYPE_SEARCH_ENABLED) == 0)) {
      return UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
    }
    remainingRows = control->rowCount;
    scanSlot = control->rowTextSlots;
    if (remainingRows != 0) {
      /* Stops at the first row that is not below the typed character, else at the last row. */
      do {
        candidateSlot = scanSlot;
        rowBelowKey = g_KeyboardAsciiCaseTransformCallbacks3.compareCaseInsensitiveFlags
                          (keyCode,*(uint32_t *)*candidateSlot);
        if (!rowBelowKey) break;
        remainingRows--;
        scanSlot = candidateSlot + 1;
      } while (remainingRows != 0);
      control->selectedRowSlot = candidateSlot;
    }
  }
  else if (keyCode == KEYBOARD_KEY_CODE_ENTER) {
    control->listStateFlags = control->listStateFlags | UI_TEXT_LIST_SELECTION_CONFIRMED;
    UiActionQueue_Enqueue(control->actionId,control);
  }
  else if ((control->rowCount == 0) && UiList_IsNavigationKey(keyCode)) {
    /* empty list: ignored as in UiListControl_HandleKeyboardNavigation (the original stores a slot that
       does not fit the 32-bit field on x64) */
    return false;
  }
  else if (keyCode == KEYBOARD_KEY_CODE_HOME) {
    control->selectedRowSlot = control->rowTextSlots;
  }
  else if (keyCode == KEYBOARD_KEY_CODE_END) {
    control->selectedRowSlot = control->rowTextSlots + (control->rowCount - 1);
  }
  else if (keyCode == KEYBOARD_KEY_CODE_PAGE_UP) {
    viewportSize = UiScrollableControl_GetViewportSize((UiScrollableControl *)(control->base).parent);
    pageUpRow = ((uint32_t)(((uintptr_t)control->selectedRowSlot - (uintptr_t)control->rowTextSlots) / sizeof(Ptr32<uint16_t>))) -
            ((int)(viewportSize.height / control->rowHeight) - 1);
    if (pageUpRow < 0) {
      pageUpRow = 0;
    }
    control->selectedRowSlot = control->rowTextSlots + pageUpRow;
  }
  else if (keyCode == KEYBOARD_KEY_CODE_PAGE_DOWN) {
    viewportSize = UiScrollableControl_GetViewportSize((UiScrollableControl *)(control->base).parent);
    pageDownRow = ((uint32_t)(((uintptr_t)control->selectedRowSlot - (uintptr_t)control->rowTextSlots) / sizeof(Ptr32<uint16_t>))) +
            (int)(viewportSize.height / control->rowHeight) - 1;
    if (control->rowCount <= pageDownRow) {
      pageDownRow = control->rowCount - 1;
    }
    control->selectedRowSlot = control->rowTextSlots + pageDownRow;
  }
  else if (keyCode == KEYBOARD_KEY_CODE_UP) {
    if (control->rowTextSlots <= control->selectedRowSlot - 1) {
      control->selectedRowSlot--;
    }
  }
  else if (keyCode == KEYBOARD_KEY_CODE_DOWN) {
    if (((uint32_t)(((uintptr_t)control->selectedRowSlot - (uintptr_t)control->rowTextSlots) / sizeof(Ptr32<uint16_t>))) + 1 <
        control->rowCount) {
      control->selectedRowSlot++;
    }
  }
  else {
    return UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
  }
  selectedSlot = control->selectedRowSlot;
  if (selectedSlot != previousSelectedSlot) {
    if (((control->listStateFlags & UI_TEXT_LIST_PLAY_SELECTION_SOUND) != 0) &&
       (control->activationSound != nullptr)) {
      g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound,nullptr);
    }
    selectedRowTop = ((uint32_t)(((uintptr_t)selectedSlot - (uintptr_t)control->rowTextSlots) / sizeof(Ptr32<uint16_t>))) * control->rowHeight;
    UiScrollableControl_ClampOffsetsToViewport
              (selectedRowTop + control->rowHeight + 1,(control->base).rightOffset,selectedRowTop,0,
               (UiScrollableControl *)(control->base).parent);
    /* arm the deferred action: the frame counter lives in the top byte */
    pulseFrames = g_UiListActivationPulseFrames;
    control->listStateFlags = control->listStateFlags | UI_TEXT_LIST_DEFERRED_ACTION_PENDING;
    control->listStateFlags = control->listStateFlags & UI_LIST_FLAGS_MASK;
    control->listStateFlags = control->listStateFlags | pulseFrames << UI_LIST_COUNTDOWN_SHIFT;
  }
  return false;
}

/* Per-frame tick of a text list (tick slot of g_UiTextListControlVtable): while a keyboard selection change
   is pending, counts its frame counter (top byte of listStateFlags) down and queues the list's action when it
   reaches 0, so quick key repeats queue only one action.
*/
void UiTextListControl_TickActivationPulse(UiTextListControl *control)

{
  if ((control->listStateFlags & UI_TEXT_LIST_DEFERRED_ACTION_PENDING) == 0) {
    return;
  }
  control->listStateFlags = control->listStateFlags - UI_STATE_FRAME_COUNTER_UNIT;
  if ((control->listStateFlags & UI_LIST_COUNTDOWN_MASK) == 0) {
    control->listStateFlags =
         control->listStateFlags &
         (UI_LIST_FLAGS_MASK & ~(UI_TEXT_LIST_DEFERRED_ACTION_PENDING|UI_TEXT_LIST_SELECTION_CONFIRMED));
    UiActionQueue_Enqueue(control->actionId,control);
  }
}

/* Fills a pointer list whose rows are rich-text strings (rowPointers, one per row) and selects row 0. The
   list's size follows its content: one list-font line plus 1 pixel per row, and the widest measured row
   plus 6 pixels; the parent (the scrollable frame) is laid out again for the new size.
*/
void UiPointerList_InitializeMeasuredTextRows(UiListRowCount rowCount,Ptr32<void> *rowPointers,UiPointerListControl *control)

{
  UiNodeBase *parentNode;
  UiPixelExtent rowHeightPixels;
  uint32_t maximumTextWidthPixels;
  RichTextExtent measuredTextExtent;
  uint32_t lineHeight;
  UiNodeVtable *parentVtable;

  FontGlyph_GetLogicalSizeForStyle(g_UiListTextStyle,0,&lineHeight);
  rowHeightPixels = lineHeight + 1;
  control->rowHeight = rowHeightPixels;
  control->rowCount = rowCount;
  control->rowSlots = rowPointers;
  maximumTextWidthPixels = 0;
  control->selectedRowSlot = rowPointers;
  (control->base).bottomOffset = rowHeightPixels * rowCount + 1;
  for (; rowCount != 0; rowCount--) {
    measuredTextExtent = RichTextCommandStream_MeasureLine(g_UiListTextStyle,(uint16_t *)*rowPointers);
    if (maximumTextWidthPixels < measuredTextExtent.widthPixels) {
      maximumTextWidthPixels = measuredTextExtent.widthPixels;
    }
    rowPointers++;
  }
  parentNode = (control->base).parent;
  parentVtable = parentNode->vtable;
  (control->base).rightOffset = maximumTextWidthPixels + 6;
  (control->base).leftOffset = 0;
  (control->base).topOffset = 0;
  parentVtable->layout(parentNode);
  return;
}

/* Compares two rich-text streams for the pointer-list sorts: both are expanded (nested streams inlined) into
   1 KiB scratch buffers and compared with Utf16String_CompareAsciiCaseInsensitiveFlags. Returns the
   comparator's order of leftText relative to rightText: -1 when less, 0 when equal, 1 when greater (a string
   that ends first compares as equal-or-greater, see the comparator). Called by
   UiPointerList_SortByExpandedTextFieldAscending.
*/
int UiPointerList_CompareExpandedText(uint16_t *rightText,uint16_t *leftText)

{
  RichTextCommandStream_CopyExpanded
            (UI_POINTER_LIST_COMPARE_SCRATCH_BYTES,g_UiPointerListExpandedLeftTextUtf16,leftText,nullptr);
  RichTextCommandStream_CopyExpanded
            (UI_POINTER_LIST_COMPARE_SCRATCH_BYTES,g_UiPointerListExpandedRightTextUtf16,rightText,nullptr);
  /* The order is the comparator's result, unchanged. */
  return (*(int (*)(uint16_t *,uint16_t *))g_Utf16StringCompareAsciiCaseInsensitiveFlags)
            (g_UiPointerListExpandedRightTextUtf16,g_UiPointerListExpandedLeftTextUtf16);
}
