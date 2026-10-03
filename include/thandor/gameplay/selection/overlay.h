/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/selection/overlay.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_SELECTION_OVERLAY_H
#define THANDOR_GAMEPLAY_SELECTION_OVERLAY_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/selection/overlay. */

/* Tints of the ghost army that previews a placement or command-mode command
   (InGameWorldOverlay_RebuildOrReleaseTransientMarkers). */
#define OVERLAY_PREVIEW_TINT_ARGB 0xCFFFFFFF /* translucent white */
#define OVERLAY_PREVIEW_TINT_MULTI_CANDIDATE_ARGB 0x4FFFFFFF /* fainter: the placement has several candidates */
#define OVERLAY_PREVIEW_TINT_BLOCKED_MASK 0xFF707070 /* darkens the preview when the placement would fail */
/* Capacities of g_InGameOwnedEntityTransientEffectMarkers and g_InGameCommandTargetTransientEffectMarkers. */
#define OVERLAY_OWNED_MARKER_CAPACITY 32
#define OVERLAY_COMMAND_TARGET_MARKER_CAPACITY 128
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
/* Functions are grouped by semantic ownership. */

void InGameWorldOverlay_RebuildOrReleaseTransientMarkers
          (GraphicsBooleanState releaseMode,WorldRuntimeContext *worldRuntime);

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

void SelectionMarkerCoordinates_ApplyType3(SelectionMarkerIndex playerId,SelectionMarkerCoordinateValue32 heading,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12);

void SelectionMarkerCoordinates_ApplyType4(SelectionMarkerIndex playerId,SelectionMarkerCoordinateValue32 heading,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12);

void SelectionMarkerCoordinates_ApplyType5(SelectionMarkerIndex playerId,SelectionMarkerCoordinateValue32 heading,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12);

void SelectionMarkerCoordinates_ApplyType6(SelectionMarkerIndex playerId,SelectionMarkerCoordinateValue32 heading,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12);

void SelectionMarkerCoordinates_ApplyType7(SelectionMarkerIndex playerId,SelectionMarkerCoordinateValue32 heading,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12);

void InGameWorldOverlay_EnsureTransientEffectMarkerAtPoint
          (Q12 scaleQ12,void *sourceWorldNode,Q12 worldYQ12,Q12 worldXQ12,void *effectDefinition,
          void *inGameRuntime);

extern int32_t g_InGamePendingPlacementArmyAsset;
extern uint32_t g_InGameCommandPreviewArmyAssetId;

/* Entries of g_InGamePointerModeHandlers (InGameSelection_SetAircraftPadTargetLane1/2,
   SelectionMarkerCoordinates_ApplyType3..7): four arguments. */
typedef void InGamePointerModeHandler
          (SelectionMarkerIndex selectionIndex,SelectionMarkerCoordinateValue32 valueC,
          SelectionMarkerCoordinateValue32 valueB,SelectionMarkerCoordinateValue32 valueA);
extern InGamePointerModeHandler *g_InGamePointerModeHandlers[8];

#endif /* THANDOR_GAMEPLAY_SELECTION_OVERLAY_H */
