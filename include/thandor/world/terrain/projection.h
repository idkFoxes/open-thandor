/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/terrain/projection.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_TERRAIN_PROJECTION_H
#define THANDOR_WORLD_TERRAIN_PROJECTION_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/terrain/projection. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00506CD0 */
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_AccumulateMaskAroundWorldPoint
          (ulonglong occupancyMaskBits,FieldGridRadiusUnits radiusWorldUnits,Q12 referenceHeightQ12,
          Q12 worldXQ12,Q12 worldYQ12,FieldGridAsset *fieldGrid);

/* 0x005099D0 */
bool __thandor_cf_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyAroundWorldPointCf
          (FieldCellFlagMask cellFlagMask,TerrainOverlayCellRuntimeValue cellValue,
          FieldGridRadiusUnits radiusWorldUnits,Q12 worldXQ12,Q12 worldYQ12,
          FieldGridAsset *fieldGrid);

/* 0x0050A190 */
bool __thandor_cf_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyAroundWorldPointCf
          (FieldCellFlagMask cellFlagMask,TerrainOverlayCellRuntimeValue cellValue,
          FieldGridRadiusUnits radiusWorldUnits,Q12 worldXQ12,Q12 worldYQ12,
          FieldGridAsset *fieldGrid);

/* 0x00500F50 */
void __thandor_void_preserve_eax_ecx_edx
TerrainProjectedGrid_TransformShadeAndQueue
          (FieldGridAsset *fieldGrid,FrontendModelPointerContextRuntimeState17C *renderContext);

/* 0x005066D0 */
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_TraceWedge0
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005067D0 */
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_TraceWedge1
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005068D0 */
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_TraceWedge2
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005069D0 */
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_TraceWedge3
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00506AD0 */
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_TraceWedge4
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00506BD0 */
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_TraceWedge5
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00509580 */
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyWedge0
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509640 */
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyWedge1
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x005096F0 */
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyWedge2
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x005097A0 */
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyWedge3
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509860 */
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyWedge4
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509910 */
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyWedge5
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509DD0 */
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyWedge0
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509E70 */
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyWedge1
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509F10 */
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyWedge2
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509FB0 */
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyWedge3
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x0050A050 */
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyWedge4
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x0050A0F0 */
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyWedge5
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00500CE0 */
void __thandor_void_preserve_ecx_edx
TerrainProjectedQuad_QueueAsTwoTrianglesRegs
          (dword rowStrideBytes,TerrainProjectedVertexWorkRecord *topLeftVertex,
          FrontendModelPointerContextRuntimeState17C *renderContext);

/* 0x005004A0 */
void __thandor_void_preserve_eax_ecx_edx
TerrainProjectedVertex_TransformProjectAndShadeVariantA(TerrainProjectedVertexWorkRecord *vertex);

/* 0x005006A0 */
void __thandor_void_preserve_eax_ecx_edx
TerrainProjectedVertex_TransformProjectAndShadeVariantB(TerrainProjectedVertexWorkRecord *vertex);

/* 0x00500820 */
void __thandor_void_preserve_eax_ecx_edx
TerrainProjectedTriangle_ClipInterpolateAndQueueTextured
          (int surfacePacketIndex,TerrainProjectedVertexWorkRecord *vertex2,
          TerrainProjectedVertexWorkRecord *vertex1,TerrainProjectedVertexWorkRecord *vertex0,
          FrontendModelPointerContextRuntimeState17C *renderContext);

/* 0x00500D30 */
void __thandor_void_preserve_eax_ecx_edx
TerrainProjectedGrid_ClipRowSpansAgainstPlane
          (FieldGridAsset *fieldGrid,GraphicsFixedVec3 *planeNormal);

/* 0x005063B0 */
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_ScanDirection0
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00506430 */
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_ScanDirection1
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005064C0 */
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_ScanDirection2
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00506540 */
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_ScanDirection3
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005065C0 */
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_ScanDirection4
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00506650 */
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_ScanDirection5
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00509320 */
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyDirection0
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509380 */
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyDirection1
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x005093F0 */
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyDirection2
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509450 */
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyDirection3
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x005094B0 */
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyDirection4
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509520 */
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyDirection5
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509B90 */
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyDirection0
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509BF0 */
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyDirection1
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509C50 */
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyDirection2
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509CB0 */
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyDirection3
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509D10 */
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyDirection4
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509D70 */
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyDirection5
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

#endif /* THANDOR_WORLD_TERRAIN_PROJECTION_H */
