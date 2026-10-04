/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/controls/minimap.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CONTROLS_MINIMAP_H
#define THANDOR_UI_CONTROLS_MINIMAP_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/controls/minimap. */

/* Functions are grouped by semantic ownership. */

void UiSelectionGeometryControl_DrawClipped
          (int clipBottom,int clipRight,int clipTop,int clipLeft,UiSelectionGeometryControl *control
          );

void UiSelectionGeometryControl_ConvertPointerAndEnqueueAction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSelectionGeometryControl *control);

/* Not in the original: builds the bilinear scaler weight tables (called once at startup). */
void UiScaler_BuildPixelWeightTables(void);

extern UiNodeVtable g_UiSelectionGeometryControlVtable;

#endif /* THANDOR_UI_CONTROLS_MINIMAP_H */
