/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/controls/window.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CONTROLS_WINDOW_H
#define THANDOR_UI_CONTROLS_WINDOW_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/controls/window. */

/* Functions are grouped by semantic ownership. */

void UiWindowControl_DrawFramedTextAndChrome
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiWindowControl *control);

extern UiNodeVtable g_UiWindowControlVtable;

#endif /* THANDOR_UI_CONTROLS_WINDOW_H */
