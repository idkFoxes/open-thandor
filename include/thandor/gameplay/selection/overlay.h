#ifndef THANDOR_GAMEPLAY_SELECTION_OVERLAY_H
#define THANDOR_GAMEPLAY_SELECTION_OVERLAY_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/selection/overlay. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00568300 */
void InGameWorldOverlay_RebuildOrReleaseTransientMarkersCf (GraphicsBooleanState releaseMode,void *inGameRuntime);

/* 0x0052F0C0 */
undefined8 SelectionOverlay_RenderSelectedArmyMetrics (UiPixelCoordinate param_1,UiPixelCoordinate param_2,UiPixelCoordinate param_3, UiPixelCoordinate param_4);

/* 0x0052F1B0 */
void __fastcall SelectionOverlay_RenderArmyMetricsForEntity (undefined4 param_1,undefined4 param_2,UiPixelCoordinate param_3,UiPixelCoordinate param_4 ,UiPixelCoordinate param_5,UiPixelCoordinate param_6,int param_7);

/* 0x0052F2A0 */
undefined8 SelectionOverlay_DrawBoundsFrame (sdword param_1,sdword param_2,sdword param_3,sdword param_4,uint param_5,int param_6, uint param_7,int param_8);

/* 0x0052F490 */
undefined8 SelectionOverlay_DrawMarkerADForFieldGridTerrainPoints (sdword param_1,sdword param_2,sdword param_3,sdword param_4,int param_5,int *param_6, FieldGridAsset *param_7);

/* 0x0052F5A0 */
void __fastcall SelectionOverlay_DrawMarkerACForWorldSurfacePoint (undefined4 param_1,undefined4 param_2,sdword param_3,sdword param_4,sdword param_5, sdword param_6,int param_7,Q12 param_8,Q12 param_9,FieldGridAsset *param_10);

/* 0x0052F680 */
undefined8 SelectionOverlay_DrawMarkerAEForVisibleProjectedGridVertices (sdword param_1,sdword param_2,sdword param_3,sdword param_4,int param_5);

/* 0x0052F780 */
undefined8 SelectionOverlay_DrawMarkerAFB0ForProjectedVertexStateFlags (sdword param_1,sdword param_2,sdword param_3,sdword param_4,int param_5);

/* 0x0052F8C0 */
undefined8 SelectionOverlay_DrawMarkerB1B2ForProjectedVertexMask1800 (sdword param_1,sdword param_2,sdword param_3,sdword param_4,byte param_5,int param_6);

/* 0x0052FA20 */
void SelectionOverlay_DrawMarkerAFForProjectedVertexFlag8000 (sdword param_1,sdword param_2,sdword param_3,sdword param_4,int param_5);

/* 0x00560020 */
undefined4 SelectionMarkerCoordinates_ApplyType3 (SelectionMarkerIndex selectionIndex,SelectionMarkerCoordinateValue32 valueC, SelectionMarkerCoordinateValue32 valueB,SelectionMarkerCoordinateValue32 valueA);

/* 0x00560050 */
undefined4 SelectionMarkerCoordinates_ApplyType4 (SelectionMarkerIndex selectionIndex,SelectionMarkerCoordinateValue32 valueC, SelectionMarkerCoordinateValue32 valueB,SelectionMarkerCoordinateValue32 valueA);

/* 0x00560080 */
undefined4 SelectionMarkerCoordinates_ApplyType5 (SelectionMarkerIndex selectionIndex,SelectionMarkerCoordinateValue32 valueC, SelectionMarkerCoordinateValue32 valueB,SelectionMarkerCoordinateValue32 valueA);

/* 0x005600B0 */
undefined4 SelectionMarkerCoordinates_ApplyType6 (SelectionMarkerIndex selectionIndex,SelectionMarkerCoordinateValue32 valueC, SelectionMarkerCoordinateValue32 valueB,SelectionMarkerCoordinateValue32 valueA);

/* 0x005600E0 */
undefined4 SelectionMarkerCoordinates_ApplyType7 (SelectionMarkerIndex selectionIndex,SelectionMarkerCoordinateValue32 valueC, SelectionMarkerCoordinateValue32 valueB,SelectionMarkerCoordinateValue32 valueA);

/* 0x00568210 */
void InGameWorldOverlay_EnsureTransientEffectMarkerAtPoint (Q12 scaleQ12,void *sourceWorldNode,Q12 worldYQ12,Q12 worldXQ12, void *effectDefinition,void *inGameRuntime);

#endif /* THANDOR_GAMEPLAY_SELECTION_OVERLAY_H */
