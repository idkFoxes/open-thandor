/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/selection/panel_metrics.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_SELECTION_PANEL_METRICS_H
#define THANDOR_GAMEPLAY_SELECTION_PANEL_METRICS_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/selection/panel_metrics. */

/* Functions are grouped by semantic ownership. */

void SelectionPanel_RenderArmyRuntimeMetrics
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate panelBottom,UiPixelCoordinate panelRight,
          UiPixelCoordinate panelTop,UiPixelCoordinate panelLeft,
          RuntimeModelFactionPrefix *runtimeEntry);

#endif /* THANDOR_GAMEPLAY_SELECTION_PANEL_METRICS_H */
