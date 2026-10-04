/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/controls/gauge.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CONTROLS_GAUGE_H
#define THANDOR_UI_CONTROLS_GAUGE_H

#include <thandor/ui/controls/types.h>
#include <thandor/core/contracts.h>

void UiHorizontalGaugeControl_DrawFrameFillAndLabel
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiHorizontalGaugeControl *control);

void UiHorizontalGaugeControl_UpdateRuntimeRangeAndDraw
          (int clipBottom,int clipRight,int clipTop,int clipLeft,UiHorizontalGaugeControl *control);

extern UiNodeVtable g_UiTransferProgressGaugeVtable; /* UiHorizontalGaugeControl subclass of the transfer progress gauge */

#endif /* THANDOR_UI_CONTROLS_GAUGE_H */
