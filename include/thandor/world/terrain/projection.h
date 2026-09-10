#ifndef THANDOR_WORLD_TERRAIN_PROJECTION_H
#define THANDOR_WORLD_TERRAIN_PROJECTION_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/terrain/projection. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00506CD0 */
undefined8 __fastcall TerrainProjectedOcclusion_AccumulateMaskAroundWorldPoint (undefined4 incomingEcxValue,undefined4 preservedEdxValue, FieldGridRadiusUnits radiusWorldUnits,Q12 referenceHeightQ12,Q12 worldXQ12,Q12 worldYQ12, FieldGridAsset *fieldGrid);

/* 0x005099D0 */
void FieldGridTerrainOverlayVariantA_ApplyAroundWorldPointCf (FieldCellFlagMask cellFlagMask,TerrainOverlayCellRuntimeValue cellValue, FieldGridRadiusUnits radiusWorldUnits,Q12 worldXQ12,Q12 worldYQ12, FieldGridAsset *fieldGrid);

/* 0x0050A190 */
void FieldGridTerrainOverlayVariantB_ApplyAroundWorldPointCf (FieldCellFlagMask cellFlagMask,TerrainOverlayCellRuntimeValue cellValue, FieldGridRadiusUnits radiusWorldUnits,Q12 worldXQ12,Q12 worldYQ12, FieldGridAsset *fieldGrid);

/* 0x00500F50 */
undefined8 TerrainProjectedGrid_TransformShadeAndQueue(int param_1,GraphicsPrimitiveQueue *param_2);

/* 0x005066D0 */
void TerrainProjectedOcclusion_TraceWedge0 (TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20, TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005067D0 */
void TerrainProjectedOcclusion_TraceWedge1 (TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20, TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005068D0 */
void TerrainProjectedOcclusion_TraceWedge2 (TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20, TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005069D0 */
void TerrainProjectedOcclusion_TraceWedge3 (TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20, TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00506AD0 */
void TerrainProjectedOcclusion_TraceWedge4 (TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20, TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00506BD0 */
void TerrainProjectedOcclusion_TraceWedge5 (TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20, TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00509580 */
void FieldGridTerrainOverlayVariantA_ApplyWedge0 (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509640 */
void FieldGridTerrainOverlayVariantA_ApplyWedge1 (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x005096F0 */
void FieldGridTerrainOverlayVariantA_ApplyWedge2 (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x005097A0 */
void FieldGridTerrainOverlayVariantA_ApplyWedge3 (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509860 */
void FieldGridTerrainOverlayVariantA_ApplyWedge4 (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509910 */
void FieldGridTerrainOverlayVariantA_ApplyWedge5 (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509DD0 */
void FieldGridTerrainOverlayVariantB_ApplyWedge0 (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509E70 */
void FieldGridTerrainOverlayVariantB_ApplyWedge1 (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509F10 */
void FieldGridTerrainOverlayVariantB_ApplyWedge2 (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509FB0 */
void FieldGridTerrainOverlayVariantB_ApplyWedge3 (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x0050A050 */
void FieldGridTerrainOverlayVariantB_ApplyWedge4 (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x0050A0F0 */
void FieldGridTerrainOverlayVariantB_ApplyWedge5 (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00500CE0 */
undefined8 TerrainProjectedQuad_QueueAsTwoTrianglesRegs(GraphicsPrimitiveQueue *param_1);

/* 0x005004A0 */
undefined8 TerrainProjectedVertex_TransformProjectAndShadeVariantA(int param_1);

/* 0x005006A0 */
undefined8 TerrainProjectedVertex_TransformProjectAndShadeVariantB(int param_1);

/* 0x00500820 */
void TerrainProjectedTriangle_ClipInterpolateAndQueueTextured (int param_1,GraphicsProjectedVertexSource *param_2, GraphicsProjectedVertexSource *param_3,GraphicsProjectedVertexSource *param_4, GraphicsPrimitiveQueue *param_5);

/* 0x00500D30 */
undefined8 TerrainProjectedGrid_ClipRowSpansAgainstPlane(int param_1,int *param_2);

/* 0x005063B0 */
void TerrainProjectedOcclusion_ScanDirection0 (TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20, TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00506430 */
void TerrainProjectedOcclusion_ScanDirection1 (TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20, TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005064C0 */
void TerrainProjectedOcclusion_ScanDirection2 (TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20, TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00506540 */
void TerrainProjectedOcclusion_ScanDirection3 (TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20, TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005065C0 */
void TerrainProjectedOcclusion_ScanDirection4 (TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20, TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00506650 */
void TerrainProjectedOcclusion_ScanDirection5 (TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20, TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00509320 */
void FieldGridTerrainOverlayVariantA_ApplyDirection0 (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509380 */
void FieldGridTerrainOverlayVariantA_ApplyDirection1 (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x005093F0 */
void FieldGridTerrainOverlayVariantA_ApplyDirection2 (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509450 */
void FieldGridTerrainOverlayVariantA_ApplyDirection3 (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x005094B0 */
void FieldGridTerrainOverlayVariantA_ApplyDirection4 (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509520 */
void FieldGridTerrainOverlayVariantA_ApplyDirection5 (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509B90 */
void FieldGridTerrainOverlayVariantB_ApplyDirection0 (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509BF0 */
void FieldGridTerrainOverlayVariantB_ApplyDirection1 (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509C50 */
void FieldGridTerrainOverlayVariantB_ApplyDirection2 (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509CB0 */
void FieldGridTerrainOverlayVariantB_ApplyDirection3 (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509D10 */
void FieldGridTerrainOverlayVariantB_ApplyDirection4 (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509D70 */
void FieldGridTerrainOverlayVariantB_ApplyDirection5 (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

#endif /* THANDOR_WORLD_TERRAIN_PROJECTION_H */
