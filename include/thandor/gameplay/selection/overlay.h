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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00568300 */
void __thandor_void_preserve_eax_ecx_edx
InGameWorldOverlay_RebuildOrReleaseTransientMarkersCf
          (GraphicsBooleanState releaseMode,WorldRuntimeContext *worldRuntime);

/* 0x0052F0C0 */
void __thandor_void_preserve_eax_ecx_edx
SelectionOverlay_RenderSelectedArmyMetrics
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight);

/* 0x0052F1B0 */
void __thandor_void_preserve_eax_ecx_edx
SelectionOverlay_RenderArmyMetricsForEntity
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,GameEntityRuntime *entity);

/* 0x0052F2A0 */
void __thandor_void_preserve_eax_ecx_edx
SelectionOverlay_DrawBoundsFrame
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate frameCoordinate0A,
          UiPixelCoordinate frameCoordinate1A,UiPixelCoordinate frameCoordinate0B,
          UiPixelCoordinate frameCoordinate1B);

/* 0x0052F490 */
void __thandor_void_preserve_eax_ecx_edx
SelectionOverlay_DrawMarkerADForFieldGridTerrainPoints
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,int markerPointCount,int *gridCoordinatePairs,
          FieldGridAsset *fieldGrid);

/* 0x0052F5A0 */
void __thandor_void_preserve_eax_ecx_edx
SelectionOverlay_DrawMarkerACForWorldSurfacePoint
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,int useTopSurface,Q12 worldCoordinate0Q12,
          Q12 worldCoordinate1Q12,FieldGridAsset *fieldGrid);

/* 0x0052F680 */
void __thandor_void_preserve_eax_ecx_edx
SelectionOverlay_DrawMarkerAEForVisibleProjectedGridVertices
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,FieldGridAsset *fieldGrid);

/* 0x0052F780 */
void __thandor_void_preserve_eax_ecx_edx
SelectionOverlay_DrawMarkerAFB0ForProjectedVertexStateFlags
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,FieldGridAsset *fieldGrid);

/* 0x0052F8C0 */
void __thandor_void_preserve_eax_ecx_edx
SelectionOverlay_DrawMarkerB1B2ForProjectedVertexMask1800
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,byte markerBitIndex,FieldGridAsset *fieldGrid);

/* 0x0052FA20 */
void __thandor_void_preserve_eax_ecx_edx
SelectionOverlay_DrawMarkerAFForProjectedVertexFlag8000
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,FieldGridAsset *fieldGrid);

/* 0x00560020 */
void __thandor_void_preserve_eax_ecx_edx
SelectionMarkerCoordinates_ApplyType3
          (SelectionMarkerIndex selectionIndex,SelectionMarkerCoordinateValue32 valueC,
          SelectionMarkerCoordinateValue32 valueB,SelectionMarkerCoordinateValue32 valueA);

/* 0x00560050 */
void __thandor_void_preserve_eax_ecx_edx
SelectionMarkerCoordinates_ApplyType4
          (SelectionMarkerIndex selectionIndex,SelectionMarkerCoordinateValue32 valueC,
          SelectionMarkerCoordinateValue32 valueB,SelectionMarkerCoordinateValue32 valueA);

/* 0x00560080 */
void __thandor_void_preserve_eax_ecx_edx
SelectionMarkerCoordinates_ApplyType5
          (SelectionMarkerIndex selectionIndex,SelectionMarkerCoordinateValue32 valueC,
          SelectionMarkerCoordinateValue32 valueB,SelectionMarkerCoordinateValue32 valueA);

/* 0x005600B0 */
void __thandor_void_preserve_eax_ecx_edx
SelectionMarkerCoordinates_ApplyType6
          (SelectionMarkerIndex selectionIndex,SelectionMarkerCoordinateValue32 valueC,
          SelectionMarkerCoordinateValue32 valueB,SelectionMarkerCoordinateValue32 valueA);

/* 0x005600E0 */
void __thandor_void_preserve_eax_ecx_edx
SelectionMarkerCoordinates_ApplyType7
          (SelectionMarkerIndex selectionIndex,SelectionMarkerCoordinateValue32 valueC,
          SelectionMarkerCoordinateValue32 valueB,SelectionMarkerCoordinateValue32 valueA);

/* 0x00568210 */
void __thandor_void_preserve_eax_ecx_edx
InGameWorldOverlay_EnsureTransientEffectMarkerAtPoint
          (Q12 scaleQ12,void *sourceWorldNode,Q12 worldYQ12,Q12 worldXQ12,void *effectDefinition,
          void *inGameRuntime);

#endif /* THANDOR_GAMEPLAY_SELECTION_OVERLAY_H */
