/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/controls/slider.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CONTROLS_SLIDER_H
#define THANDOR_UI_CONTROLS_SLIDER_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/controls/slider. */

/* Functions are grouped by semantic ownership. */

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

#endif /* THANDOR_UI_CONTROLS_SLIDER_H */
