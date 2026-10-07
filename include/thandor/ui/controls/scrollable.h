/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/controls/scrollable.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CONTROLS_SCROLLABLE_H
#define THANDOR_UI_CONTROLS_SCROLLABLE_H

#include <thandor/ui/controls/types.h>
#include <thandor/core/contracts.h>

/* Scrollbar pieces in g_UiWindowTextureSource (UiScrollableControl_RefreshChildAndScrollThumbs): the arrow
   buttons give the bar thickness and arrow length, a thumb is at least two thumb pieces long. */
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW = 0x5A;
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW = 0x5E;
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_HORIZONTAL_THUMB = 0xC0;
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_VERTICAL_THUMB = 0xC2;

/* More g_UiWindowTextureSource pieces (UiScrollableControl_DrawFrameContentAndScrollbars and the list row
   drawing). Each pressed/active piece follows its normal piece at +8 (0x5A -> 0x62 ... 0x61 -> 0x69); the
   thumb caps have their active variant at +4 (0xC0 -> 0xC4 ... 0xC3 -> 0xC7). */
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_HORIZONTAL_ARROW_RIGHT = 0x5B;
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_HORIZONTAL_THUMB_MIDDLE = 0x5C;
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_HORIZONTAL_TRACK = 0x5D;
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW_DOWN = 0x5F;
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_VERTICAL_THUMB_MIDDLE = 0x60;
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_VERTICAL_TRACK = 0x61;
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_PRESSED_OFFSET = 8;
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_HORIZONTAL_THUMB_END = 0xC1;
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_VERTICAL_THUMB_END = 0xC3;
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_THUMB_ACTIVE_OFFSET = 4;
/* Frame styles: four corners (top-left, top-right, bottom-left, bottom-right), then the top, left, right and
   bottom edges. */
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_FRAME_A_FIRST = 0x6A;
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_FRAME_B_FIRST = 0x72;
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_INTERIOR = 0x7A;
/* Row highlight of the selected list row: plain bar, or left cap / middle / right cap when focused. */
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_ROW_HIGHLIGHT = 0x82;
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_ROW_FOCUS_LEFT = 0x83;
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_ROW_FOCUS_MIDDLE = 0x84;
inline constexpr int32_t UI_WINDOW_SUBRESOURCE_ROW_FOCUS_RIGHT = 0x85;

/* UiScrollableControl scrollStateFlags bits beyond UiScrollableStateFlags: the interior fill (0x100 draws
   UI_WINDOW_SUBRESOURCE_INTERIOR, 0x200 alone draws subresource 0), the two frame styles, the right-button
   drag (panning) and whether that drag started inside the content view (then the hit test does not pass
   the pointer to the content). */
inline constexpr int32_t UI_SCROLL_FILL_INTERIOR_TEXTURED = 0x100;
inline constexpr int32_t UI_SCROLL_FILL_INTERIOR = 0x200;
inline constexpr int32_t UI_SCROLL_FRAME_STYLE_A = 0x400;
inline constexpr int32_t UI_SCROLL_FRAME_STYLE_B = 0x800;
/* NOTE: UiScrollableControl_BeginSecondaryScrollInteraction sets 0x1000 and clears it again a few instructions
   later (as in the original), so the pointer wheel's test of it never sees it set. */
inline constexpr int32_t UI_SCROLL_SECONDARY_INTERACTION_ACTIVE = 0x1000;
inline constexpr int32_t UI_SCROLL_SECONDARY_PANNING_CONTENT = 0x4000;
/* All UI_SCROLL_HORIZONTAL_*_ACTIVE / UI_SCROLL_VERTICAL_*_ACTIVE part bits. */
inline constexpr int32_t UI_SCROLL_HORIZONTAL_PARTS_ACTIVE = 0x1F0000;
inline constexpr int32_t UI_SCROLL_VERTICAL_PARTS_ACTIVE = 0x1F000000;
/* Cursor frames of a panning drag: all directions, vertical only, horizontal only. */
inline constexpr int32_t UI_SCROLL_CURSOR_FRAME_PAN = 1;
inline constexpr int32_t UI_SCROLL_CURSOR_FRAME_PAN_VERTICAL = 4;
inline constexpr int32_t UI_SCROLL_CURSOR_FRAME_PAN_HORIZONTAL = 5;
/* scrollStateFlags bits 4..7: the bar positions a control allows (the bar bits shifted left by 4), read by
   UiScrollableControl layout */
inline constexpr int32_t UI_SCROLL_ALLOWED_HORIZONTAL_BARS = 0x30;
inline constexpr int32_t UI_SCROLL_ALLOWED_VERTICAL_BARS = 0xC0;

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

UiScrollableViewportSize UiScrollableControl_GetViewportSize(UiScrollableControl *control);

void UiScrollableControl_RefreshChildAndScrollThumbs(UiScrollableControl *control);

void UiScrollableControl_ClampOffsetsToViewport
          (UiPixelCoordinate targetBottom,UiPixelCoordinate targetRight,UiPixelCoordinate targetTop,
          UiPixelCoordinate targetLeft,UiScrollableControl *control);

extern UiNodeVtable g_UiScrollableControlVtable;
extern UiNodeVtable g_UiListControlVtable;

extern UiNodeVtable g_UiTextListControlVtable;

#endif /* THANDOR_UI_CONTROLS_SCROLLABLE_H */
