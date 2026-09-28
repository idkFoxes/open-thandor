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
#define SELECTION_OVERLAY_MARKER_FLUID_RECEIVER_EXCLUDED 0xAF /* also the FIELD_CELL_INIT_CLEARED_UNRESOLVED_BIT15 marker */
#define SELECTION_OVERLAY_MARKER_FLUID_SOURCE_EXCLUDED 0xB0
#define SELECTION_OVERLAY_MARKER_SELECTED_RESOURCE 0xB1
#define SELECTION_OVERLAY_MARKER_OTHER_RESOURCE 0xB2
/* Initial g_ModelProjectedBoundsPixels: an empty (inverted) rectangle for ModelProjectedBounds_AccumulateHierarchyRecursive. */
#define SELECTION_OVERLAY_EMPTY_BOUNDS_MIN 0x10000
#define SELECTION_OVERLAY_EMPTY_BOUNDS_MAX (-0x10000)
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00568300 */
void InGameWorldOverlay_RebuildOrReleaseTransientMarkers
          (GraphicsBooleanState releaseMode,WorldRuntimeContext *worldRuntime);

/* 0x0052F0C0 */
void SelectionOverlay_RenderSelectedArmyMetrics
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight);

/* 0x0052F1B0 */
void SelectionOverlay_RenderArmyMetricsForEntity
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,GameEntityRuntime *entity);

/* 0x0052F2A0 */
void SelectionOverlay_DrawBoundsFrame(UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate cornerAY,UiPixelCoordinate cornerAX,
          UiPixelCoordinate cornerBY,UiPixelCoordinate cornerBX);

/* 0x0052F490 */
void SelectionOverlay_DrawMarkerADForFieldGridTerrainPoints
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,int markerPointCount,int *gridCoordinatePairs,
          FieldGridAsset *fieldGrid);

/* 0x0052F5A0 */
void SelectionOverlay_DrawMarkerACForWorldSurfacePoint
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,int useTopSurface,Q12 worldYQ12,Q12 worldXQ12,
          FieldGridAsset *fieldGrid);

/* 0x0052F680 */
void SelectionOverlay_DrawMarkerAEForVisibleProjectedGridVertices
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,FieldGridAsset *fieldGrid);

/* 0x0052F780 */
void SelectionOverlay_DrawMarkerAFB0ForProjectedVertexStateFlags
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,FieldGridAsset *fieldGrid);

/* 0x0052F8C0 */
void SelectionOverlay_DrawMarkerB1B2ForProjectedVertexMask1800
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,uint8_t selectedResourceIndex,FieldGridAsset *fieldGrid);

/* 0x0052FA20 */
void SelectionOverlay_DrawMarkerAFForProjectedVertexFlag8000
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,FieldGridAsset *fieldGrid);

/* 0x00560020 */
void SelectionMarkerCoordinates_ApplyType3(SelectionMarkerIndex playerId,SelectionMarkerCoordinateValue32 heading,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12);

/* 0x00560050 */
void SelectionMarkerCoordinates_ApplyType4(SelectionMarkerIndex playerId,SelectionMarkerCoordinateValue32 heading,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12);

/* 0x00560080 */
void SelectionMarkerCoordinates_ApplyType5(SelectionMarkerIndex playerId,SelectionMarkerCoordinateValue32 heading,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12);

/* 0x005600B0 */
void SelectionMarkerCoordinates_ApplyType6(SelectionMarkerIndex playerId,SelectionMarkerCoordinateValue32 heading,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12);

/* 0x005600E0 */
void SelectionMarkerCoordinates_ApplyType7(SelectionMarkerIndex playerId,SelectionMarkerCoordinateValue32 heading,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12);

/* 0x00568210 */
void InGameWorldOverlay_EnsureTransientEffectMarkerAtPoint
          (Q12 scaleQ12,void *sourceWorldNode,Q12 worldYQ12,Q12 worldXQ12,void *effectDefinition,
          void *inGameRuntime);

#endif /* THANDOR_GAMEPLAY_SELECTION_OVERLAY_H */
