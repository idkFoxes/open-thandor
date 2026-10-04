/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/controls/gauge.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CONTROLS_GAUGE_H
#define THANDOR_UI_CONTROLS_GAUGE_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/controls/gauge. */

/* Functions are grouped by semantic ownership. */

void UiHorizontalGaugeControl_DrawFrameFillAndLabel
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiHorizontalGaugeControl *control);

GraphicsCursorFrameIndex UiHorizontalGaugeControl_PointerMoveBusyCursor
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

extern UiNodeVtable g_UiHorizontalGaugeControlVtable;

void UiHorizontalGaugeControl_UpdateRuntimeRangeAndDraw
          (int clipBottom,int clipRight,int clipTop,int clipLeft,UiHorizontalGaugeControl *control);

extern UiNodeVtable g_UiTransferProgressGaugeVtable; /* UiHorizontalGaugeControl subclass of the transfer progress gauge */

#endif /* THANDOR_UI_CONTROLS_GAUGE_H */
