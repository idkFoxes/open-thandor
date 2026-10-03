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

/* Scrollbar pieces in g_UiWindowTextureSource (UiScrollableControl_RefreshChildAndScrollThumbs): the arrow
   buttons give the bar thickness and arrow length, a thumb is at least two thumb pieces long. */
#define UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW 0x5A
#define UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW 0x5E
#define UI_WINDOW_SUBRESOURCE_HORIZONTAL_THUMB 0xC0
#define UI_WINDOW_SUBRESOURCE_VERTICAL_THUMB 0xC2

/* More g_UiWindowTextureSource pieces (UiScrollableControl_DrawFrameContentAndScrollbars and the list row
   drawing). Each pressed/active piece follows its normal piece at +8 (0x5A -> 0x62 ... 0x61 -> 0x69); the
   thumb caps have their active variant at +4 (0xC0 -> 0xC4 ... 0xC3 -> 0xC7). */
#define UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW_RIGHT 0x5B
#define UI_WINDOW_SUBRESOURCE_HORIZONTAL_THUMB_MIDDLE 0x5C
#define UI_WINDOW_SUBRESOURCE_HORIZONTAL_TRACK 0x5D
#define UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW_DOWN 0x5F
#define UI_WINDOW_SUBRESOURCE_VERTICAL_THUMB_MIDDLE 0x60
#define UI_WINDOW_SUBRESOURCE_VERTICAL_TRACK 0x61
#define UI_WINDOW_SUBRESOURCE_PRESSED_OFFSET 8
#define UI_WINDOW_SUBRESOURCE_HORIZONTAL_THUMB_END 0xC1
#define UI_WINDOW_SUBRESOURCE_VERTICAL_THUMB_END 0xC3
#define UI_WINDOW_SUBRESOURCE_THUMB_ACTIVE_OFFSET 4
/* Frame styles: four corners (top-left, top-right, bottom-left, bottom-right), then the top, left, right and
   bottom edges. */
#define UI_WINDOW_SUBRESOURCE_FRAME_A_FIRST 0x6A
#define UI_WINDOW_SUBRESOURCE_FRAME_B_FIRST 0x72
#define UI_WINDOW_SUBRESOURCE_INTERIOR 0x7A
/* Row highlight of the selected list row: plain bar, or left cap / middle / right cap when focused. */
#define UI_WINDOW_SUBRESOURCE_ROW_HIGHLIGHT 0x82
#define UI_WINDOW_SUBRESOURCE_ROW_FOCUS_LEFT 0x83
#define UI_WINDOW_SUBRESOURCE_ROW_FOCUS_MIDDLE 0x84
#define UI_WINDOW_SUBRESOURCE_ROW_FOCUS_RIGHT 0x85

/* UiScrollableControl scrollStateFlags bits beyond UiScrollableStateFlags: the interior fill (0x100 draws
   UI_WINDOW_SUBRESOURCE_INTERIOR, 0x200 alone draws subresource 0), the two frame styles, the right-button
   drag (panning) and whether that drag started inside the content view (then the hit test does not pass
   the pointer to the content). */
#define UI_SCROLL_FILL_INTERIOR_TEXTURED 0x100
#define UI_SCROLL_FILL_INTERIOR 0x200
#define UI_SCROLL_FRAME_STYLE_A 0x400
#define UI_SCROLL_FRAME_STYLE_B 0x800
/* NOTE: UiScrollableControl_BeginSecondaryScrollInteraction sets 0x1000 and clears it again a few instructions
   later (as in the original), so the pointer wheel's test of it never sees it set. */
#define UI_SCROLL_SECONDARY_INTERACTION_ACTIVE 0x1000
#define UI_SCROLL_SECONDARY_PANNING_CONTENT 0x4000
/* All UI_SCROLL_HORIZONTAL_*_ACTIVE / UI_SCROLL_VERTICAL_*_ACTIVE part bits. */
#define UI_SCROLL_HORIZONTAL_PARTS_ACTIVE 0x1F0000
#define UI_SCROLL_VERTICAL_PARTS_ACTIVE 0x1F000000
/* Cursor frames of a panning drag: all directions, vertical only, horizontal only. */
#define UI_SCROLL_CURSOR_FRAME_PAN 1
#define UI_SCROLL_CURSOR_FRAME_PAN_VERTICAL 4
#define UI_SCROLL_CURSOR_FRAME_PAN_HORIZONTAL 5
/* scrollStateFlags bits 4..7: the bar positions a control allows (the bar bits shifted left by 4), read by
   UiScrollableControl layout */
#define UI_SCROLL_ALLOWED_HORIZONTAL_BARS 0x30
#define UI_SCROLL_ALLOWED_VERTICAL_BARS 0xC0

/* UiListControl.listStateFlags / UiTimedListControl.listStateAndDelay: bits 24..31 count down the frames until
   a deferred list action is queued (tick callbacks). */
#define UI_LIST_COUNTDOWN_SHIFT 24
#define UI_LIST_COUNTDOWN_ONE 0x1000000
#define UI_LIST_COUNTDOWN_MASK 0xff000000
#define UI_LIST_FLAGS_MASK 0xffffff

/* UiSelectableControl stateFlags bits read by UiSelectableControl_KeyboardEvent: Enter / Escape also activate
   the control; 0x80 plays UiSoundSelectableControl.activationSound on a keyboard activation (the same bit
   is UI_SPRITE_BUTTON_ANIMATED for sprite buttons). */
#define UI_SELECTABLE_ACTIVATE_ON_ENTER 0x04
#define UI_SELECTABLE_ACTIVATE_ON_ESCAPE 0x08
#define UI_SELECTABLE_PLAY_KEYBOARD_SOUND 0x80

/* Directory tree records (UiTimedListTree_BuildDirectoryRecordBlock): parentBlockOrIcon is the row icon in the
   list's rowTextureSource (drive rows use their EngineDriveTypeCode); each label buffer is 0x200 bytes. */
#define UI_TIMED_LIST_ICON_DIRECTORY 0x26
#define UI_TIMED_LIST_ICON_COMPUTER 0x27
#define UI_TIMED_LIST_LABEL_BYTES 0x200
/* ASCII letter compare ignoring case: clears bit 5 (0x20) of the XOR of two code units */
#define UI_TIMED_LIST_CASE_FOLD_MASK 0xdf

/* UiCatalogEntryControl_DrawClipped: packed text styles of the overlays (price, count, percentage); the alert
   colour marks an unaffordable price or a flagged army. MEASURE is only used to measure the text. */
#define UI_CATALOG_TEXT_STYLE_NORMAL 0x1040000
#define UI_CATALOG_TEXT_STYLE_ALERT 0x1050000
#define UI_CATALOG_TEXT_STYLE_MEASURE 0x1000000

bool UiTimedListControl_HandleKeyboardNavigation
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiTimedListControl *control);

bool UiListControl_HandleKeyboardNavigation
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiListControl *control);

void UiPointerList_RefreshSelectionAndQueueAction(UiPointerListControl *control);

void UiScrollableControl_BeginPrimaryScrollInteraction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiScrollableControl *control);

void UiScrollableControl_EndPrimaryScrollInteraction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiScrollableControl *control);

void UiScrollableControl_UpdatePrimaryScrollDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiScrollableControl *control);

void UiScrollableControl_UpdateSecondaryScrollDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiScrollableControl *control);

void UiScrollableControl_TickAutoScroll(UiScrollableControl *control);

void UiScrollableControl_HandlePointerWheel
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiScrollableControl *control);

void UiPointerList_SelectTextListIndex(UiListRowIndex index,UiPointerListControl *control);

void UiListControl_SelectRowFromPointer
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiListControl *control);

void UiPointerList_SortByDwordPairFieldDescending
          (UiPointerListFieldByteOffset fieldOffset,UiPointerListControl *control);

void UiPointerList_SortByDwordFieldAscending(UiPointerListFieldByteOffset fieldOffset,UiPointerListControl *control);

void UiTimedListControl_SelectRowFromPointer(int pointerButton,int pointerY,int pointerX,UiNodeBase *control);

void UiNodeList_UnsuppressActionId(UiActionId actionId,UiNodeBase *firstNode);

void UiNodeList_SuppressActionId(UiActionId actionId,UiNodeBase *firstNode);

bool UiSelectableControl_KeyboardEvent(UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiSoundSelectableControl *control);

void UiSelectableControl_SuppressIfActionId(UiActionId actionId,UiSelectableControl *control);

void UiSelectableControl_UnsuppressIfActionId(UiActionId actionId,UiSelectableControl *control);

bool UiSelectableGroup_FindVisibleSelected
          (UiNodeBase **outNode,uint32_t *outIndex,UiControlCount controlCount,...);

uint32_t UiSelectableGroup_SelectedIndex(UiControlCount controlCount,...);

void UiSelectableGroup_SelectExclusive(UiControlCount controlCount,UiNodeBase *selectedControl,...);

uint8_t UiSelectableControl_IsSelected(UiSelectableControl *control);

void UiSelectableControl_SetSelected(UiBooleanState32 selected,UiSelectableControl *control);

uint32_t UiPageStack_ActivePageIndex(UiPageStackControl *stack);

void UiScrollableControl_RelocateChildren(UiSerializedRelocationDelta relocationDelta,UiScrollableControl *control);

void UiScrollableControl_DrawFrameContentAndScrollbars
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiScrollableControl *control);

void UiScrollableControl_RebuildViewportAndScrollbars(UiScrollableControl *control);

GraphicsCursorFrameIndex UiScrollableControl_QueryPointerRegion
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiScrollableControl *control);

void UiScrollableControl_BeginSecondaryScrollInteraction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiScrollableControl *control);

void UiScrollableControl_EndSecondaryScrollInteraction
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiScrollableControl *control);

UiNodeBase * UiScrollableControl_HitTestContentAndScrollbars
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiScrollableControl *control);

UiListRowIndex UiPointerList_GetSelectedIndex(UiPointerListControl *control);

void UiListControl_DrawRowsAndSelection(int clipBottom,int clipRight,int clipTop,int clipLeft,UiListControl *control);

void UiListControl_TickActivationPulse(UiListControl *control);

void UiListControl_UnsuppressIfActionId(UiActionId actionId,UiListControl *control);

void UiListControl_SuppressIfActionId(UiActionId actionId,UiListControl *control);

void UiPointerList_InitializeColumnLayout(UiListRowCount rowCount,void **rowPointers,UiPointerListControl *control);

void UiTimedListControl_RelocateChildren(UiSerializedRelocationDelta relocationDelta,UiTimedListControl *control);

void UiTimedListControl_DrawRowsAndSelection(int clipBottom,int clipRight,int clipTop,int clipLeft,UiNodeBase *control);

void UiTimedListControl_TickActionDelay(UiTimedListControl *control);

void UiWrappedTextControl_RelocateAndApplyDeferredOffset
          (UiSerializedRelocationDelta relocationDelta,UiWrappedTextControl *control);

void UiCatalogEntryControl_DrawClipped
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiCatalogEntryControl *control);

GraphicsCursorFrameIndex UiCatalogEntryControl_PointerMove
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiCatalogEntryControl *control);

void UiCatalogEntryControl_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiCatalogEntryControl *control);

void UiTimedListControl_SelectRecordAndScrollIntoView
          (UiTimedListTreeRecord *selectedRecord,UiTimedListTreeControl *control);

void UiPointerList_SelectColumnListIndex(UiListRowIndex index,UiPointerListControl *control);

UiListRowIndex UiPointerList_GetSelectedIndexAndConfirmed(UiPointerListControl *control,bool *outConfirmed);

UiScrollableViewportSize UiScrollableControl_GetViewportSize(UiScrollableControl *control);

uint32_t UiTimedListTree_CountRecordArrayAndNestedChildren(UiTimedListTreeRecord *recordBlock);

void UiScrollableControl_RefreshChildAndScrollThumbs(UiScrollableControl *control);

void UiScrollableControl_ClampOffsetsToViewport
          (UiPixelCoordinate targetBottom,UiPixelCoordinate targetRight,UiPixelCoordinate targetTop,
          UiPixelCoordinate targetLeft,UiScrollableControl *control);


UiTimedListTreeRecord * UiTimedListTree_FindRecordByLabel(uint16_t *labelUtf16,UiTimedListTreeRecord *recordBlock);

bool UiTimedListTree_BuildDirectoryRecordBlock(uint16_t *pathUtf16,UiTimedListTreeRecord **outRecordBlock);

bool UiTimedListTree_BuildDirectoryHierarchy
          (uint16_t *selectedPathUtf16,UiTimedListTreeRecord **outRootBlock,
          UiTimedListTreeRecord **outSelectedRecord);

bool UiTimedListTree_FreeRecordBlockRecursiveAndTestContains
          (UiTimedListTreeRecord *targetRecord,UiTimedListTreeRecord *recordBlock);

bool UiTimedListTree_AttachDirectoryRecordBlock(UiTimedListTreeRecord *record);

void UiTimedListControl_ToggleDirectoryRecordExpansion
          (UiTimedListTreeRecord *record,UiTimedListTreeControl *control);

bool UiTimedListTree_BuildRecordPath(uint32_t *outputPathDwords,UiTimedListTreeRecord *record);

void UiTimedListControl_SetRecordTreeAndRecomputeLayout
          (UiTimedListTreeRecord *recordTree,UiTimedListTreeControl *control);

UiTimedListTreeRecord * UiTimedListControl_GetRecordTree(UiTimedListControl *control);

UiTimedListTreeRecord *
UiTimedListControl_GetSelectedRecord(UiTimedListTreeControl *control);

extern UiNodeVtable g_UiScrollableControlVtable;
extern UiNodeVtable g_UiListControlVtable;
extern UiNodeVtable g_UiTimedListControlVtable;
extern UiNodeVtable g_UiListOffsetControlVtable;
extern UiNodeVtable g_UiCommandVisibilityWrappedTextVtable; /* 00517F10 g_UiCommandVisibilityWrappedTextVtable; followed by 0x90 code alignment fill */

extern uint32_t g_UiCatalogGroup48ColumnCount;
extern uint32_t g_UiCatalogGroup42ColumnCount;
extern int32_t *g_UiCatalogGroup48OffsetTables[9];
extern int32_t *g_UiCatalogGroup42OffsetTables[7];
extern int32_t g_UiCatalogGroup48OffsetsDefault[48];
extern int32_t g_UiCatalogGroup48Offsets5Columns[48];
extern int32_t g_UiCatalogGroup48Offsets6Columns[48];
extern int32_t g_UiCatalogGroup48Offsets7Columns[48];
extern int32_t g_UiCatalogGroup48Offsets8Columns[48];
extern int32_t g_UiCatalogGroup42OffsetsDefault[42];
extern int32_t g_UiCatalogGroup42Offsets5Columns[42];
extern int32_t g_UiCatalogGroup42Offsets6Columns[42];

#endif /* THANDOR_UI_CONTROLS_LISTS_H */
