/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/selection_overlay.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_SELECTION_OVERLAY_H
#define THANDOR_UI_INGAME_SELECTION_OVERLAY_H

#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/world/terrain/types.h>
#include <thandor/core/contracts.h>

/* Subresources of g_SelectionPanelTextureSource drawn by the SelectionOverlay_* functions: the eight pieces of
   the bounds frame and the map-view markers. */
#define SELECTION_OVERLAY_FRAME_TOP_LEFT 0xA4
#define SELECTION_OVERLAY_FRAME_TOP 0xA5
#define SELECTION_OVERLAY_FRAME_TOP_RIGHT 0xA6
#define SELECTION_OVERLAY_FRAME_LEFT 0xA7
#define SELECTION_OVERLAY_FRAME_RIGHT 0xA8
#define SELECTION_OVERLAY_FRAME_BOTTOM_LEFT 0xA9
#define SELECTION_OVERLAY_FRAME_BOTTOM 0xAA
#define SELECTION_OVERLAY_FRAME_BOTTOM_RIGHT 0xAB
#define SELECTION_OVERLAY_MARKER_WORLD_POINT 0xAC
#define SELECTION_OVERLAY_MARKER_GRID_POINT 0xAD
#define SELECTION_OVERLAY_MARKER_GRID_VERTEX 0xAE
#define SELECTION_OVERLAY_MARKER_FLUID_RECEIVER_EXCLUDED 0xAF /* also the FIELD_CELL_DEBUG_MARKED marker */
#define SELECTION_OVERLAY_MARKER_FLUID_SOURCE_EXCLUDED 0xB0
#define SELECTION_OVERLAY_MARKER_SELECTED_RESOURCE 0xB1
#define SELECTION_OVERLAY_MARKER_OTHER_RESOURCE 0xB2
/* Initial g_ModelProjectedBoundsPixels: an empty (inverted) rectangle for ModelProjectedBounds_AccumulateHierarchyRecursive. */
#define SELECTION_OVERLAY_EMPTY_BOUNDS_MIN 0x10000
#define SELECTION_OVERLAY_EMPTY_BOUNDS_MAX (-0x10000)

void SelectionOverlay_RenderSelectedArmyMetrics
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft);

void SelectionOverlay_RenderArmyMetricsForEntity
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,GameEntityRuntime *entity);

void SelectionOverlay_DrawBoundsFrame(UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate cornerAY,UiPixelCoordinate cornerAX,
          UiPixelCoordinate cornerBY,UiPixelCoordinate cornerBX);

void SelectionOverlay_DrawTerrainPointMarkers
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,int markerPointCount,int *gridCoordinatePairs,
          FieldGridAsset *fieldGrid);

void SelectionOverlay_DrawWorldPointMarker
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,int useTopSurface,Q12 worldYQ12,Q12 worldXQ12,
          FieldGridAsset *fieldGrid);

void SelectionOverlay_DrawGridVertexMarkers
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,FieldGridAsset *fieldGrid);

void SelectionOverlay_DrawFluidExclusionMarkers
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,FieldGridAsset *fieldGrid);

void SelectionOverlay_DrawResourceCellMarkers
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,uint8_t selectedResourceIndex,FieldGridAsset *fieldGrid);

void SelectionOverlay_DrawDebugMarkedCellMarkers
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,FieldGridAsset *fieldGrid);

#endif /* THANDOR_UI_INGAME_SELECTION_OVERLAY_H */
