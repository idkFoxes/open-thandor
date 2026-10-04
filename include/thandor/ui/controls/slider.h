/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/controls/slider.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CONTROLS_SLIDER_H
#define THANDOR_UI_CONTROLS_SLIDER_H

#include <thandor/core/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/core/contracts.h>

void UiRangeSliderControl_DrawTrackAndThumb
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiRangeSliderControl *control);

void UiRangeSliderControl_BeginThumbDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiRangeSliderControl *control);

void UiRangeSliderControl_EndThumbDrag
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiRangeSliderControl *control);

void UiRangeSliderControl_SuppressIfActionId(UiActionId actionId,UiRangeSliderControl *control);

void UiRangeSliderControl_UnsuppressIfActionId(UiActionId actionId,UiRangeSliderControl *control);

extern UiNodeVtable g_UiRangeSliderControlVtable;

/* UiRangeSliderControl sliderFlags (UiRangeSliderControl_* in ui/controls/slider.cpp). */
#define UI_RANGE_SLIDER_VERTICAL 0x1
#define UI_RANGE_SLIDER_DRAGGING 0x2 /* thumb drag in progress */
#define UI_RANGE_SLIDER_CLICK_SOUND 0x4 /* play clickSound on press/release/key step */
#define UI_RANGE_SLIDER_REVERSED 0x8 /* maximum at the left/bottom */

/* Slider thumb pieces in g_UiWindowTextureSource; their size sets the usable track length. */
#define UI_RANGE_SLIDER_SUBRESOURCE_HORIZONTAL_THUMB 0xAF
#define UI_RANGE_SLIDER_SUBRESOURCE_VERTICAL_THUMB 0xB7

/* UiRangeSliderControl_DrawTrackAndThumb: each slider look is four pieces (start cap, tiled track, end cap,
   thumb) from this base; the suppressed look starts 4 pieces later, the vertical pieces 8 pieces later. */
#define UI_RANGE_SLIDER_SUBRESOURCE_BASE 0xAC
#define UI_RANGE_SLIDER_SUBRESOURCE_BASE_SUPPRESSED 0xB0
#define UI_RANGE_SLIDER_SUBRESOURCE_VERTICAL_OFFSET 8
#define UI_RANGE_SLIDER_PIECE_TRACK 1 /* piece offsets from the look's base; +0 is the start cap */
#define UI_RANGE_SLIDER_PIECE_END_CAP 2
#define UI_RANGE_SLIDER_PIECE_THUMB 3

Bool8 UiRangeSliderControl_HandleKeyboard
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiRangeSliderControl *control);

void UiRangeSliderControl_UpdateValueFromPointer
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiRangeSliderControl *control);

void UiRangeSliderControl_HandlePointerWheel
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiRangeSliderControl *control);

#endif /* THANDOR_UI_CONTROLS_SLIDER_H */
