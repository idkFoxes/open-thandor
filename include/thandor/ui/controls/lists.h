/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/controls/lists.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CONTROLS_LISTS_H
#define THANDOR_UI_CONTROLS_LISTS_H

#include <thandor/core/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/ui/frontend/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/controls/lists. */

/* UiListControl.listStateFlags / UiTimedListControl.listStateAndDelay: bits 24..31 count down the frames until
   a deferred list action is queued (tick callbacks). */
#define UI_LIST_COUNTDOWN_SHIFT 24
#define UI_LIST_COUNTDOWN_ONE 0x1000000
#define UI_LIST_COUNTDOWN_MASK 0xff000000
#define UI_LIST_FLAGS_MASK 0xffffff

/* Functions are grouped by semantic ownership. */

Bool8 UiListControl_HandleKeyboardNavigation
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiListControl *control);

void UiPointerList_RefreshSelectionAndQueueAction(UiPointerListControl *control);

void UiPointerList_SelectTextListIndex(UiListRowIndex index,UiPointerListControl *control);

void UiListControl_SelectRowFromPointer
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiListControl *control);

void UiPointerList_SortByDwordPairFieldDescending
          (UiPointerListFieldByteOffset fieldOffset,UiPointerListControl *control);

void UiPointerList_SortByDwordFieldAscending(UiPointerListFieldByteOffset fieldOffset,UiPointerListControl *control);

UiListRowIndex UiPointerList_GetSelectedIndex(UiPointerListControl *control);

void UiListControl_DrawRowsAndSelection(int clipBottom,int clipRight,int clipTop,int clipLeft,UiListControl *control);

void UiListControl_TickActivationPulse(UiListControl *control);

void UiListControl_UnsuppressIfActionId(UiActionId actionId,UiListControl *control);

void UiListControl_SuppressIfActionId(UiActionId actionId,UiListControl *control);

void UiPointerList_InitializeColumnLayout(UiListRowCount rowCount,Ptr32<void> *rowPointers,UiPointerListControl *control);

void UiPointerList_SelectColumnListIndex(UiListRowIndex index,UiPointerListControl *control);

UiListRowIndex UiPointerList_GetSelectedIndexAndConfirmed(UiPointerListControl *control,Bool8 *outConfirmed);

void UiPointerList_SortByExpandedTextFieldAscending
          (UiPointerListFieldByteOffset fieldOffset,UiPointerListControl *control);

void UiTextListControl_DrawRowsAndSelection
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiTextListControl *control);

void UiTextListControl_SelectRowFromPointer
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiTextListControl *control);

Bool8 UiTextListControl_HandleKeyboardNavigationAndSearch
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiTextListControl *control);

void UiTextListControl_TickActivationPulse(UiTextListControl *control);

void UiTextListControl_UnsuppressIfActionId(UiActionId actionId,UiTextListControl *control);

void UiTextListControl_SuppressIfActionId(UiActionId actionId,UiTextListControl *control);

void UiPointerList_InitializeMeasuredTextRows(UiListRowCount rowCount,Ptr32<void> *rowPointers,UiPointerListControl *control);

int UiPointerList_CompareExpandedText(uint16_t *rightText,uint16_t *leftText);

extern UiNodeVtable g_UiListOffsetControlVtable;

extern const UiFrameDelayFrames g_UiListActivationPulseFrames; /* UiFrameDelayFrames, 8: frames of the activation pulse after Enter on a list/text list before its action is queued (src/ui/controls/lists.c, text.c). */
extern const uint32_t g_UiListTextStyle;

#endif /* THANDOR_UI_CONTROLS_LISTS_H */
