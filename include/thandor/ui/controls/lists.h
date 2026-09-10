#ifndef THANDOR_UI_CONTROLS_LISTS_H
#define THANDOR_UI_CONTROLS_LISTS_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/controls/lists. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x004BBE60 */
void UiTimedListControl_HandleKeyboardNavigationCf (dword keyCode,dword stateMask,UiNodeBase *control);

/* 0x004BB100 */
void UiListControl_HandleKeyboardNavigationCf (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode, UiListControl *control);

/* 0x004BB480 */
void UiPointerList_RefreshSelectionAndQueueAction(UiPointerListControl *control);

/* 0x004B87A0 */
void UiScrollableControl_BeginPrimaryScrollInteraction (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiScrollableControl *control);

/* 0x004B8A20 */
void __fastcall UiScrollableControl_EndPrimaryScrollInteraction(undefined4 param_1,undefined4 param_2);

/* 0x004B8BF0 */
void UiScrollableControl_UpdatePrimaryScrollDrag (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiScrollableControl *control);

/* 0x004B8F90 */
void UiScrollableControl_UpdateSecondaryScrollDrag (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiScrollableControl *control);

/* 0x004B9070 */
void UiScrollableControl_TickAutoScroll(UiScrollableControl *control);

/* 0x004B90E0 */
void UiScrollableControl_HandlePointerWheel (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiScrollableControl *control);

/* 0x004BA500 */
void UiPointerList_SelectIndexVariantA(UiListRowIndex index,UiPointerListControl *control);

/* 0x004BB020 */
void UiListControl_SelectRowFromPointer (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiListControl *control);

/* 0x004BB7A0 */
undefined8 UiPointerList_SortByDwordPairFieldDescending (UiPointerListFieldByteOffset fieldOffset,UiPointerListControl *control);

/* 0x004BB8A0 */
undefined8 UiPointerList_SortByDwordFieldAscending (UiPointerListFieldByteOffset fieldOffset,UiPointerListControl *control);

/* 0x004BBCD0 */
void UiTimedListControl_SelectRowFromPointer (int pointerButton,int pointerY,int pointerX,UiNodeBase *control);

/* 0x004B11C0 */
void UiNodeList_UnsuppressActionId(UiActionId actionId,UiNodeBase *firstNode);

/* 0x004B1200 */
void UiNodeList_SuppressActionId(UiActionId actionId,UiNodeBase *firstNode);

/* 0x004B2550 */
void UiSelectableControl_KeyboardEventCf (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode, UiSelectableControl *control);

/* 0x004B26E0 */
void UiSelectableControl_SuppressIfActionId(UiActionId actionId,UiSelectableControl *control);

/* 0x004B2710 */
void UiSelectableControl_UnsuppressIfActionId(UiActionId actionId,UiSelectableControl *control);

/* 0x004B2D30 */
void UiSelectableGroup_NoneVisibleSelectedCf(UiControlCount controlCount);

/* 0x004B2D70 */
void UiSelectableGroup_NoneSelectedCf(UiControlCount controlCount);

/* 0x004B2DA0 */
undefined4 UiSelectableGroup_SelectExclusive(UiControlCount controlCount,UiNodeBase *selectedControl);

/* 0x004B2DE0 */
void UiSelectableControl_IsSelectedCf(UiSelectableControl *control);

/* 0x004B2E10 */
void UiSelectableControl_SetSelected(UiBooleanState32 selected,UiSelectableControl *control);

/* 0x004B4920 */
void UiPageStack_ActivePageNotInListCf(UiPageStackControl *stack);

/* 0x004B7970 */
void UiScrollableControl_RelocateChildren (UiSerializedRelocationDelta relocationDelta,UiScrollableControl *control);

/* 0x004B79D0 */
void UiScrollableControl_DrawFrameContentAndScrollbars (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiScrollableControl *control);

/* 0x004B8310 */
undefined8 UiScrollableControl_RebuildViewportAndScrollbars(UiScrollableControl *control);

/* 0x004B8AC0 */
GraphicsCursorFrameIndex UiScrollableControl_QueryPointerRegion (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiScrollableControl *control);

/* 0x004B8B10 */
void UiScrollableControl_BeginSecondaryScrollInteraction (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiScrollableControl *control);

/* 0x004B8BC0 */
void UiScrollableControl_EndSecondaryScrollInteraction(void);

/* 0x004B9000 */
UiNodeBase * UiScrollableControl_HitTestContentAndScrollbars (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiScrollableControl *control);

/* 0x004BA4E0 */
undefined4 UiPointerList_GetRowSlotsVariantA(int param_1);

/* 0x004BA560 */
UiListRowIndex UiPointerList_GetSelectedIndexVariantACf(UiPointerListControl *control);

/* 0x004BADE0 */
void UiListControl_DrawRowsAndSelection (int clipTop,int clipLeft,int clipBottom,int clipRight,UiNodeBase *control);

/* 0x004BB310 */
void UiListControl_TickActivationPulse(UiListControl *control);

/* 0x004BB350 */
void UiListControl_UnsuppressIfActionId(UiActionId actionId,UiListControl *control);

/* 0x004BB380 */
void UiListControl_SuppressIfActionId(UiActionId actionId,UiListControl *control);

/* 0x004BB3B0 */
void UiPointerList_InitializeColumnLayout (UiListRowCount rowCount,void **rowPointers,UiPointerListControl *control);

/* 0x004BB460 */
undefined4 UiPointerList_GetRowSlotsVariantB(int param_1);

/* 0x004BB9E0 */
void UiTimedListControl_RelocateChildren (UiSerializedRelocationDelta relocationDelta,UiTimedListControl *control);

/* 0x004BBA00 */
void UiTimedListControl_DrawRowsAndSelection (int clipTop,int clipLeft,int clipBottom,int clipRight,UiNodeBase *control);

/* 0x004BC180 */
void UiTimedListControl_TickActionDelay(UiTimedListControl *control);

/* 0x004BC460 */
void UiListOffsetControl_RelocateAndApplyDeferredOffset (UiSerializedRelocationDelta relocationDelta,UiNodeBase *control);

/* 0x00516580 */
void UiCatalogEntryControl_DrawClipped (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiCatalogEntryControl *control);

/* 0x00516B90 */
GraphicsCursorFrameIndex UiCatalogEntryControl_PointerMove (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiCatalogEntryControl *control);

/* 0x00516C50 */
void UiCatalogEntryControl_NonRightRelease (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiCatalogEntryControl *control);

/* 0x004BC360 */
void UiTimedListControl_SelectRecordAndScrollIntoView (UiTimedListTreeRecord16 *selectedRecord,UiTimedListRuntimeExtendedView88 *control);

/* 0x004BB4E0 */
undefined8 UiPointerList_SelectIndexVariantB(UiListRowIndex index,UiPointerListControl *control);

/* 0x004BB540 */
UiListRowIndex UiPointerList_GetSelectedIndexVariantBCf(UiPointerListControl *control);

/* 0x004B9460 */
UiScrollableContentDimensionsEdxEax8 UiScrollableControl_QueryContentSizeRegs(UiScrollableControl *control);

/* 0x004BC310 */
dword UiTimedListTree_CountRecordArrayAndNestedChildren(UiTimedListTreeRecord16 *recordBlock);

/* 0x004B9170 */
undefined8 UiScrollableControl_RefreshChildAndScrollThumbs(UiScrollableControl *control);

/* 0x004B9490 */
void UiScrollableControl_ClampOffsetsToViewport (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiScrollableControl *control);

#endif /* THANDOR_UI_CONTROLS_LISTS_H */
