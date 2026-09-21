/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/controls/lists.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CONTROLS_LISTS_H
#define THANDOR_UI_CONTROLS_LISTS_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/controls/lists. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x004BBE60 */
bool __thandor_cf_preserve_eax_ecx_edx
UiTimedListControl_HandleKeyboardNavigationCf(dword keyCode,dword stateMask,UiNodeBase *control);

/* 0x004BB100 */
bool __thandor_cf_preserve_eax_ecx_edx
UiListControl_HandleKeyboardNavigationCf
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiListControl *control);

/* 0x004BB480 */
void __thandor_preserve_eax_edx
UiPointerList_RefreshSelectionAndQueueAction(UiPointerListControl *control);

/* 0x004B87A0 */
void __thandor_void_preserve_eax_ecx_edx
UiScrollableControl_BeginPrimaryScrollInteraction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiScrollableControl *control);

/* 0x004B8A20 */
void __thandor_preserve_eax
UiScrollableControl_EndPrimaryScrollInteraction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control);

/* 0x004B8BF0 */
void __thandor_void_preserve_eax_ecx_edx
UiScrollableControl_UpdatePrimaryScrollDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiScrollableControl *control);

/* 0x004B8F90 */
void __thandor_void_preserve_eax_ecx_edx
UiScrollableControl_UpdateSecondaryScrollDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiScrollableControl *control);

/* 0x004B9070 */
void __thandor_void_preserve_ecx_edx
UiScrollableControl_TickAutoScroll(UiScrollableControl *control);

/* 0x004B90E0 */
void __thandor_void_preserve_eax_ecx_edx
UiScrollableControl_HandlePointerWheel
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiScrollableControl *control);

/* 0x004BA500 */
void __thandor_void_preserve_eax_ecx_edx
UiPointerList_SelectIndexVariantA(UiListRowIndex index,UiPointerListControl *control);

/* 0x004BB020 */
void __thandor_void_preserve_eax_ecx_edx
UiListControl_SelectRowFromPointer
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiListControl *control);

/* 0x004BB7A0 */
void __thandor_void_preserve_eax_ecx_edx
UiPointerList_SortByDwordPairFieldDescending
          (UiPointerListFieldByteOffset fieldOffset,UiPointerListControl *control);

/* 0x004BB8A0 */
void __thandor_void_preserve_eax_ecx_edx
UiPointerList_SortByDwordFieldAscending
          (UiPointerListFieldByteOffset fieldOffset,UiPointerListControl *control);

/* 0x004BBCD0 */
void __thandor_void_preserve_eax_ecx_edx
UiTimedListControl_SelectRowFromPointer
          (int pointerButton,int pointerY,int pointerX,UiNodeBase *control);

/* 0x004B11C0 */
void __thandor_void_preserve_eax_ecx_edx
UiNodeList_UnsuppressActionId(UiActionId actionId,UiNodeBase *firstNode);

/* 0x004B1200 */
void __thandor_void_preserve_eax_ecx_edx
UiNodeList_SuppressActionId(UiActionId actionId,UiNodeBase *firstNode);

/* 0x004B2550 */
bool __thandor_cf_preserve_eax_ecx_edx
UiSelectableControl_KeyboardEventCf
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiSelectableControl *control);

/* 0x004B26E0 */
void __thandor_void_preserve_eax_ecx_edx
UiSelectableControl_SuppressIfActionId(UiActionId actionId,UiSelectableControl *control);

/* 0x004B2710 */
void __thandor_void_preserve_eax_ecx_edx
UiSelectableControl_UnsuppressIfActionId(UiActionId actionId,UiSelectableControl *control);

/* 0x004B2D30 */
UiSelectableNodeEaxEcxCf9 __thandor_eax_ecx_cf_preserve_edx
UiSelectableGroup_NoneVisibleSelectedCf(UiControlCount controlCount);

/* 0x004B2D70 */
UiSelectableGroupIndexEcxCf5 __thandor_eax_ecx_cf_preserve_edx
UiSelectableGroup_NoneSelectedCf(UiControlCount controlCount);

/* 0x004B2DA0 */
void __thandor_void_preserve_eax_ecx_edx
UiSelectableGroup_SelectExclusive(UiControlCount controlCount,UiNodeBase *selectedControl);

/* 0x004B2DE0 */
byte __thandor_cf_preserve_eax_ecx_edx
UiSelectableControl_IsSelectedCf(UiSelectableControl *control);

/* 0x004B2E10 */
void __thandor_void_preserve_eax_ecx_edx
UiSelectableControl_SetSelected(UiBooleanState32 selected,UiSelectableControl *control);

/* 0x004B4920 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
UiPageStack_ActivePageNotInListCf(UiPageStackControl *stack);

/* 0x004B7970 */
void __thandor_void_preserve_eax_ecx_edx
UiScrollableControl_RelocateChildren
          (UiSerializedRelocationDelta relocationDelta,UiScrollableControl *control);

/* 0x004B79D0 */
void __thandor_void_preserve_eax_ecx_edx
UiScrollableControl_DrawFrameContentAndScrollbars
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiScrollableControl *control);

/* 0x004B8310 */
void __thandor_void_preserve_eax_ecx_edx
UiScrollableControl_RebuildViewportAndScrollbars(UiScrollableControl *control);

/* 0x004B8AC0 */
GraphicsCursorFrameIndex __thandor_eax_preserve_ecx_edx
UiScrollableControl_QueryPointerRegion
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiScrollableControl *control);

/* 0x004B8B10 */
void __thandor_void_preserve_ecx_edx
UiScrollableControl_BeginSecondaryScrollInteraction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiScrollableControl *control);

/* 0x004B8BC0 */
void UiScrollableControl_EndSecondaryScrollInteraction
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiNodeBase *control);

/* 0x004B9000 */
UiNodeBase * __thandor_eax_preserve_ecx_edx
UiScrollableControl_HitTestContentAndScrollbars
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiScrollableControl *control);

/* 0x004BA4E0 */
void ** UiPointerList_GetRowSlotsVariantA(UiPointerListControl *control);

/* 0x004BA560 */
UiListRowIndex UiPointerList_GetSelectedIndexVariantACf(UiPointerListControl *control);

/* 0x004BADE0 */
void __thandor_void_preserve_eax_ecx_edx
UiListControl_DrawRowsAndSelection
          (int clipTop,int clipLeft,int clipBottom,int clipRight,UiNodeBase *control);

/* 0x004BB310 */
void __thandor_preserve_eax UiListControl_TickActivationPulse(UiListControl *control);

/* 0x004BB350 */
void __thandor_void_preserve_eax_ecx_edx
UiListControl_UnsuppressIfActionId(UiActionId actionId,UiListControl *control);

/* 0x004BB380 */
void __thandor_void_preserve_eax_ecx_edx
UiListControl_SuppressIfActionId(UiActionId actionId,UiListControl *control);

/* 0x004BB3B0 */
void __thandor_void_preserve_eax_ecx_edx
UiPointerList_InitializeColumnLayout
          (UiListRowCount rowCount,void **rowPointers,UiPointerListControl *control);

/* 0x004BB460 */
void ** UiPointerList_GetRowSlotsVariantB(UiPointerListControl *control);

/* 0x004BB9E0 */
void __thandor_void_preserve_eax_ecx_edx
UiTimedListControl_RelocateChildren
          (UiSerializedRelocationDelta relocationDelta,UiTimedListControl *control);

/* 0x004BBA00 */
void __thandor_void_preserve_eax_ecx_edx
UiTimedListControl_DrawRowsAndSelection
          (int clipTop,int clipLeft,int clipBottom,int clipRight,UiNodeBase *control);

/* 0x004BC180 */
void __thandor_preserve_eax UiTimedListControl_TickActionDelay(UiTimedListControl *control);

/* 0x004BC460 */
void __thandor_void_preserve_eax_ecx_edx
UiListOffsetControl_RelocateAndApplyDeferredOffset
          (UiSerializedRelocationDelta relocationDelta,UiNodeBase *control);

/* 0x00516580 */
void __thandor_void_preserve_eax_ecx_edx
UiCatalogEntryControl_DrawClipped
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiCatalogEntryControl *control);

/* 0x00516B90 */
GraphicsCursorFrameIndex __thandor_eax_preserve_ecx_edx
UiCatalogEntryControl_PointerMove
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiCatalogEntryControl *control);

/* 0x00516C50 */
void __thandor_void_preserve_eax_ecx_edx
UiCatalogEntryControl_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiCatalogEntryControl *control);

/* 0x004BC360 */
void __thandor_void_preserve_eax_ecx_edx
UiTimedListControl_SelectRecordAndScrollIntoView
          (UiTimedListTreeRecord16 *selectedRecord,UiTimedListRuntimeExtendedView88 *control);

/* 0x004BB4E0 */
void __thandor_void_preserve_eax_ecx_edx
UiPointerList_SelectIndexVariantB(UiListRowIndex index,UiPointerListControl *control);

/* 0x004BB540 */
UiListRowIndexEaxCf5 __thandor_eax_cf_preserve_ecx_edx
UiPointerList_GetSelectedIndexVariantBCf(UiPointerListControl *control);

/* 0x004B9460 */
UiScrollableContentDimensionsEdxEax8 UiScrollableControl_QueryContentSizeRegs(UiScrollableControl *control);

/* 0x004BC310 */
dword __thandor_eax_preserve_ecx_edx
UiTimedListTree_CountRecordArrayAndNestedChildren(UiTimedListTreeRecord16 *recordBlock);

/* 0x004B9170 */
void __thandor_void_preserve_eax_ecx_edx
UiScrollableControl_RefreshChildAndScrollThumbs(UiScrollableControl *control);

/* 0x004B9490 */
void __thandor_void_preserve_eax_ecx_edx
UiScrollableControl_ClampOffsetsToViewport
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiScrollableControl *control);


/* 0x0040FF70 */
UiTimedListTreeRecord16 * __thandor_eax_preserve_ecx_edx
UiTimedListTree_FindRecordByLabel(word *labelUtf16,UiTimedListTreeRecord16 *recordBlock);

/* 0x0040FFE0 */
Recovered0040FFE0EaxCf5 __thandor_eax_cf_preserve_ecx_edx
UiTimedListTree_BuildDirectoryRecordBlockCf(word *pathUtf16);

/* 0x00410380 */
UiTimedListDirectoryHierarchyEaxEdxCf9 __thandor_eax_edx_cf_preserve_ecx
UiTimedListTree_BuildDirectoryHierarchyCf(word *selectedPathUtf16);

/* 0x004104B0 */
bool __thandor_cf_preserve_eax_ecx_edx
UiTimedListTree_FreeRecordBlockRecursiveAndTestContainsCf
          (UiTimedListTreeRecord16 *targetRecord,UiTimedListTreeRecord16 *recordBlock);

/* 0x00410520 */
bool __thandor_cf_preserve_ecx_edx
UiTimedListTree_AttachDirectoryRecordBlockCf(UiTimedListTreeRecord16 *record);

/* 0x00410670 */
void __thandor_void_preserve_eax_ecx
UiTimedListControl_ToggleDirectoryRecordExpansion
          (UiTimedListTreeRecord16 *record,UiTimedListRuntimeExtendedView88 *control);

/* 0x00410700 */
bool __thandor_cf_preserve_ecx_edx
UiTimedListTree_BuildRecordPathCf(dword *outputPathDwords,UiTimedListTreeRecord16 *record);

/* 0x004BC1C0 */
void __thandor_void_preserve_eax_ecx_edx
UiTimedListControl_SetRecordTreeAndRecomputeLayout
          (UiTimedListTreeRecord16 *recordTree,UiTimedListRuntimeExtendedView88 *control);

/* 0x004BC2F0 */
UiTimedListTreeRecord16 * UiTimedListControl_GetRecordTree(UiTimedListControl *control);

/* 0x004BC3F0 */
UiTimedListTreeRecord16 *
UiTimedListControl_GetSelectedRecord(UiTimedListRuntimeExtendedView88 *control);

#endif /* THANDOR_UI_CONTROLS_LISTS_H */
