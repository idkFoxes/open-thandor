/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/tree_list.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/tree_list.h>
#include <thandor/thandor.h>
#include <stdarg.h>

/* Module data. */

static uint8_t g_UiTimedListDriveLetters[32] = {0};

static WidePathBuffer256 g_UiTimedListRecordPathScratch = {0};

static WidePathBuffer256 g_UiTimedListCombinedPathScratch = {0};

static WidePathBuffer256 g_UiTimedListSecondaryPathScratch = {0};

static WidePathBuffer256 g_UiTimedListHierarchyPathScratch = {0};

static WidePathBuffer256 g_UiTimedListHierarchyParentPathScratch = {0};

static uint16_t g_WildcardAllFilesUtf16[4] = {'*', '.', '*', 0}; /* L"*.*" */

static uint16_t g_UiTimedListDriveWildcardUtf16[7] = {'?', ':', '\\', '*', '.', '*', 0}; /* L"?:\\*.*" */

static const UiFrameDelayFrames g_UiTimedListActionDelayFrames = 8;

/* Implementation ownership: ui/controls/tree_list. */

/* Keyboard handler of the tree list (g_UiTimedListControlVtable keyboardEvent): Home/End, Up/Down and
   Page Up/Down walk the visible rows of the record tree, Left/Right collapse/expand the selected row through
   recordSelectionCallback. A moved selection is scrolled into view and its action queued after
   g_UiTimedListActionDelayFrames frames (UiTimedListControl_TickActionDelay). Other keys go to the default
   focus handling; the keys handled here return false.
*/
Bool8 UiTimedListControl_HandleKeyboardNavigation
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
  Bool8 handled;
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

/* Left-button press on the tree list (g_UiTimedListControlVtable nonRightPress): walks the visible rows to
   the one under the pointer. A hit on the expand/collapse icon in the indentation (opaque pixel) calls
   recordSelectionCallback; a hit on the row icon or label (up to 6 pixels past the text) selects the row,
   scrolls it into view and queues the list's action, and a double click also calls recordSelectionCallback
   for a directory row.
*/
void UiTimedListControl_SelectRowFromPointer(int pointerButton,int pointerY,int pointerX,UiNodeBase *control)

{
  /* Expanded records with children save their position (savedRecord/savedRemaining, at most
     TREE_DEPTH_LIMIT levels) and descend, so every level of the tree is walked. */
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

/* Returns the row of recordBlock whose label equals labelUtf16 (exact, case-sensitive compare of at most
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
  Bool8 charsEqual;

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

/* Builds one level of the directory tree for the tree list: for an empty path the root block with the
   single "computer" row; for a drive root ("X:" or "X:\") the list of drives, labelled "X:[volume label]"
   with their drive type as icon; otherwise the subdirectories of pathUtf16. The block (header record, rows,
   then the 0x200-byte labels) is allocated from the arena; a row gets flag bit 0 when it has
   subdirectories, i.e. can be expanded. Returns true with the block in *outRecordBlock, or false on failure
   (allocation or enumeration; *outRecordBlock is then left unchanged).
*/
Bool8 UiTimedListTree_BuildDirectoryRecordBlock(uint16_t *pathUtf16,UiTimedListTreeRecord **outRecordBlock)

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
    if (g_MemoryApi.alloc(2 * sizeof(UiTimedListTreeRecord) + UI_TIMED_LIST_LABEL_BYTES,(void **)&recordCursor) == 0) {
      recordCursor[0].countOrLabelText = 1;
      recordCursor[0].parentBlockOrIcon = 0;
      recordCursor[0].childBlockOrParentRecord = NULL;
      recordCursor[0].flags = UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY;
      recordCursor[1].countOrLabelText = (uintptr_t)(recordCursor + 2);
      recordCursor[1].parentBlockOrIcon = UI_TIMED_LIST_ICON_COMPUTER;
      recordCursor[1].childBlockOrParentRecord = NULL;
      recordCursor[1].flags = UI_TIMED_LIST_RECORD_EXPANDABLE;
      g_LocaleCopyDefaultComputerLabelUtf16((uint16_t *)(recordCursor + 2));
      *outRecordBlock = recordCursor;
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
      driveRow[-1].countOrLabelText = directoryEntryCount;
      driveRow[-1].parentBlockOrIcon = 0;
      driveRow[-1].childBlockOrParentRecord = NULL;
      driveRow[-1].flags = UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY;
      labelCursor = (uint32_t *)(driveRow + directoryEntryCount);
      driveLetterCursor = (uint8_t *)THANDOR_ADDR(g_UiTimedListDriveLetters,0);
      do {
        driveLetter = (uint32_t)*driveLetterCursor;
        driveRow->countOrLabelText = (uintptr_t)labelCursor;
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
            recordCursor = (UiTimedListTreeRecord *)recordBlock;
            recordCursor->countOrLabelText = directoryEntryCount;
            recordCursor->parentBlockOrIcon = 0;
            recordCursor->childBlockOrParentRecord = NULL;
            recordCursor->flags = UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY;
            labelWriteCursor = (uint32_t *)(recordCursor + recordCount);
            leaf = outputRecords;
            for (; directoryEntryCount != 0; directoryEntryCount--) {
              recordCursor[1].countOrLabelText = (uintptr_t)labelWriteCursor;
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
                    ((uint32_t)((uintptr_t)labelWriteCursor - (uintptr_t)recordBlock) + UI_TIMED_LIST_LABEL_BYTES,
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
static Bool8 UiTimedListTree_AbandonDirectoryHierarchy
          (UiTimedListTreeRecord **levelStack,int levelStackTop,int levelCount,
          UiTimedListTreeRecord **outSelectedRecord)
{
  for (; levelCount != 0; levelCount--) {
    g_MemoryApi.free(levelStack[--levelStackTop]);
  }
  *outSelectedRecord = NULL;
  return false;
}

/* Builds the directory tree from the root ("computer") down to selectedPathUtf16: one record block per path
   level (UiTimedListTree_BuildDirectoryRecordBlock), each linked to its parent block and opened (expanded)
   from the parent row that names it. Returns true with the root block in *outRootBlock and the row of the
   selected directory in *outSelectedRecord, or false on failure (then the blocks built so far are freed,
   see UiTimedListTree_AbandonDirectoryHierarchy; *outSelectedRecord is set to NULL, *outRootBlock left
   unchanged). No caller found
   in src/.
*/
Bool8 UiTimedListTree_BuildDirectoryHierarchy
          (uint16_t *selectedPathUtf16,UiTimedListTreeRecord **outRootBlock,
          UiTimedListTreeRecord **outSelectedRecord)

{
  /* levelStack keeps one (record, block) pair per level and gives them back in reverse order
     when linking the levels. A path buffer holds at most 256
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
      levelBlock->parentBlockOrIcon = (uintptr_t)parentBlock;
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

/* Frees a record block and every expanded child block below it (collapsing a directory row) and returns
   true when targetRecord (the list's selected row) was one of the freed rows, so the caller can move the
   selection to the collapsed row.
*/
Bool8 UiTimedListTree_FreeRecordBlockRecursiveAndTestContains
          (UiTimedListTreeRecord *targetRecord,UiTimedListTreeRecord *recordBlock)

{
  UiTimedListTreeRecord *recordCursor;
  uint32_t recordsRemaining;
  int containsCount;
  Bool8 childContains;
  
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

/* Expands a directory row of the tree: builds the full path of the row (its label joined to the labels of
   its ancestor rows, a drive row gives "X:"), enumerates its subdirectories into a new record block and
   links the block below the row (the block header points back to the row and to the row's own block).
   The root "computer" row lists the drives. Returns true when the path or the block cannot be built.
*/
Bool8 UiTimedListTree_AttachDirectoryRecordBlock(UiTimedListTreeRecord *record)

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
  childBlock->parentBlockOrIcon = (uintptr_t)UiTimedListTree_FindBlockHeader(record);
  return false;
}

/* Expands or collapses a directory row of the tree list (the directory browser's recordSelectionCallback),
   then recomputes the list layout and scrolls the selection into view. Collapsing a branch that contained
   the selected row moves the selection to the collapsed row and queues the list's action. No caller found
   in src/.
*/
void UiTimedListControl_ToggleDirectoryRecordExpansion
          (UiTimedListTreeRecord *record,UiTimedListTreeControl *control)

{
  UiTimedListTreeRecord *targetRecord;
  Bool8 selectionWasInBranch;
  Bool8 attachFailed;

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

/* Writes the full path of a tree row to outputPathDwords (256 code units): the row label joined to the
   labels of its ancestor rows; a drive row gives "X:", the root "computer" row its label unchanged. Returns
   true when the ancestor chain does not end in a drive row. No caller found in src/.
*/
Bool8 UiTimedListTree_BuildRecordPath(uint32_t *outputPathDwords,UiTimedListTreeRecord *record)

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

/* Relocation of the tree list loaded from a serialized UI tree (g_UiTimedListControlVtable relocate): only
   relocates the children like any container.
*/
void UiTimedListControl_RelocateChildren(UiSerializedRelocationDelta relocationDelta,UiTimedListControl *control)

{
  UiContainer_RelocateChildren(relocationDelta,&control->base);
  return;
}

/* Draws the tree list (g_UiTimedListControlVtable drawClipped): for every visible row the connector lines
   of its ancestor levels, its branch and expand/collapse icons, its row icon, the highlight behind the
   selected row's label (with end caps while the list has the keyboard focus) and the label.
*/
void UiTimedListControl_DrawRowsAndSelection(int clipBottom,int clipRight,int clipTop,int clipLeft,UiNodeBase *control)

{
  /* Expanded records save (record, remaining) in savedRecord/savedRemaining and descend; the tree
     connector columns test the saved remaining counts of the parent levels. Argument order of the
     draw calls follows the original. */
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

/* Per-frame tick of the tree list (g_UiTimedListControlVtable tick): counts down the deferred action of a
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

/* Returns the selected row of the tree list. Called by UiTimedListControl_ToggleDirectoryRecordExpansion.
*/
UiTimedListTreeRecord *
UiTimedListControl_GetSelectedRecord(UiTimedListTreeControl *control)

{
  return (control->base).selectedRecord;
}

/* Selects a row of the tree list and scrolls it into view: counts the visible rows above it (walking back
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

/* Gives the tree list a new record tree (or NULL) and selects its first row: row height from the list font,
   row count over all expanded blocks, width from the widest indented row label plus icon; then the parent
   scroll frame is laid out again for the new content size.
*/
void UiTimedListControl_SetRecordTreeAndRecomputeLayout
          (UiTimedListTreeRecord *recordTree,UiTimedListTreeControl *control)

{
  /* Expanded records with children save their position (savedRecord/savedRemaining) and descend,
     so every level of the tree is walked. */
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
  remaining = (recordTree == NULL) ? 0 : (uintptr_t)recordTree->countOrLabelText;
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

/* Returns the root record block of the tree list. No caller found in src/.
*/
UiTimedListTreeRecord * UiTimedListControl_GetRecordTree(UiTimedListControl *control)

{
  return control->recordTree;
}

/* Counts the visible rows of a record block: its rows plus, recursively, the rows of every expanded child
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

UiNodeVtable g_UiTimedListControlVtable = {
    .relocate = THANDOR_FN(UiTimedListControl_RelocateChildren),
    .method04 = THANDOR_FN(UiNode_DefaultMethod04_NoOp),
    .drawClipped = THANDOR_FN(UiTimedListControl_DrawRowsAndSelection),
    .layout = THANDOR_FN(UiContainer_LayoutChildren),
    .nonRightPress = THANDOR_FN(UiTimedListControl_SelectRowFromPointer),
    .nonRightRelease = THANDOR_FN(UiNode_DefaultNonRightRelease),
    .rightPress = THANDOR_FN(UiNode_ForwardRightPressToParent),
    .rightRelease = THANDOR_FN(UiNode_DefaultRightRelease),
    .nonRightDrag = THANDOR_FN(UiNode_DefaultNonRightDrag),
    .rightDrag = THANDOR_FN(UiNode_DefaultRightDrag),
    .pointerMove = THANDOR_FN(UiNode_DefaultPointerMove),
    .hitTest = THANDOR_FN(UiContainer_HitTestChildren),
    .keyboardEvent = THANDOR_FN(UiTimedListControl_HandleKeyboardNavigation),
    .applyFlags = THANDOR_FN(UiNode_ApplyFlagsRecursive),
    .suppressActionId = THANDOR_FN(UiContainer_SuppressActionId),
    .unsuppressActionId = THANDOR_FN(UiContainer_UnsuppressActionId),
    .tick = THANDOR_FN(UiTimedListControl_TickActionDelay),
    .pointerWheel = THANDOR_FN(UiNode_ForwardPointerWheelToParent)};
