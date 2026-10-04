/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/controls/tree_list.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CONTROLS_TREE_LIST_H
#define THANDOR_UI_CONTROLS_TREE_LIST_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/controls/tree_list. */

/* Directory tree records (UiTimedListTree_BuildDirectoryRecordBlock): parentBlockOrIcon is the row icon in the
   list's rowTextureSource (drive rows use their EngineDriveTypeCode); each label buffer is 0x200 bytes. */
#define UI_TIMED_LIST_ICON_DIRECTORY 0x26
#define UI_TIMED_LIST_ICON_COMPUTER 0x27
#define UI_TIMED_LIST_LABEL_BYTES 0x200
/* ASCII letter compare ignoring case: clears bit 5 (0x20) of the XOR of two code units */
#define UI_TIMED_LIST_CASE_FOLD_MASK 0xdf

/* Functions are grouped by semantic ownership. */

Bool8 UiTimedListControl_HandleKeyboardNavigation
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiTimedListControl *control);

void UiTimedListControl_SelectRowFromPointer(int pointerButton,int pointerY,int pointerX,UiNodeBase *control);

void UiTimedListControl_RelocateChildren(UiSerializedRelocationDelta relocationDelta,UiTimedListControl *control);

void UiTimedListControl_DrawRowsAndSelection(int clipBottom,int clipRight,int clipTop,int clipLeft,UiNodeBase *control);

void UiTimedListControl_TickActionDelay(UiTimedListControl *control);

void UiTimedListControl_SelectRecordAndScrollIntoView
          (UiTimedListTreeRecord *selectedRecord,UiTimedListTreeControl *control);

uint32_t UiTimedListTree_CountRecordArrayAndNestedChildren(UiTimedListTreeRecord *recordBlock);

UiTimedListTreeRecord * UiTimedListTree_FindRecordByLabel(uint16_t *labelUtf16,UiTimedListTreeRecord *recordBlock);

Bool8 UiTimedListTree_BuildDirectoryRecordBlock(uint16_t *pathUtf16,UiTimedListTreeRecord **outRecordBlock);

Bool8 UiTimedListTree_BuildDirectoryHierarchy
          (uint16_t *selectedPathUtf16,UiTimedListTreeRecord **outRootBlock,
          UiTimedListTreeRecord **outSelectedRecord);

Bool8 UiTimedListTree_FreeRecordBlockRecursiveAndTestContains
          (UiTimedListTreeRecord *targetRecord,UiTimedListTreeRecord *recordBlock);

Bool8 UiTimedListTree_AttachDirectoryRecordBlock(UiTimedListTreeRecord *record);

void UiTimedListControl_ToggleDirectoryRecordExpansion
          (UiTimedListTreeRecord *record,UiTimedListTreeControl *control);

Bool8 UiTimedListTree_BuildRecordPath(uint32_t *outputPathDwords,UiTimedListTreeRecord *record);

void UiTimedListControl_SetRecordTreeAndRecomputeLayout
          (UiTimedListTreeRecord *recordTree,UiTimedListTreeControl *control);

UiTimedListTreeRecord * UiTimedListControl_GetRecordTree(UiTimedListControl *control);

UiTimedListTreeRecord *
UiTimedListControl_GetSelectedRecord(UiTimedListTreeControl *control);

extern UiNodeVtable g_UiTimedListControlVtable;

#endif /* THANDOR_UI_CONTROLS_TREE_LIST_H */
