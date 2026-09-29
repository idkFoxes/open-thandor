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

/* FieldGridCell.flagsAndMaterial bits of TerrainProjectedVertex_TransformProjectAndShadeVariantA (a field cell is
   also its TerrainProjectedVertexWorkRecord): set when point A (projectedPointA, the terrain point) or point B
   (projectedPointB, the offset secondary point) is not beyond the near plane and got no screen position. */
#define TERRAIN_VERTEX_POINT_A_NOT_PROJECTED 0x200000
#define TERRAIN_VERTEX_POINT_B_NOT_PROJECTED 0x4000000
/* The other projectionFlags bits of the terrain vertex pass: the low byte is the soil material (0xFF = no
   terrain at this vertex, nothing is drawn); per projected point one bit per side of g_ProjectionClipRect the
   point lies on the inner side of (x >= minX, y >= minY, x < maxX, y < maxY), so the OR over a triangle's
   vertices tells whether its screen bounding box can overlap the clip rectangle; SECONDARY_VISIBLE is set on
   the vertices of a queued secondary-surface triangle and makes TerrainProjectedVertex_TransformProjectAndShadeVariantB
   project point B again. */
#define TERRAIN_VERTEX_MATERIAL_MASK 0xff
#define TERRAIN_VERTEX_MATERIAL_NONE 0xff
#define TERRAIN_VERTEX_POINT_A_INSIDE_MIN_X 0x20000
#define TERRAIN_VERTEX_POINT_A_INSIDE_MIN_Y 0x40000
#define TERRAIN_VERTEX_POINT_A_INSIDE_MAX_X 0x80000
#define TERRAIN_VERTEX_POINT_A_INSIDE_MAX_Y 0x100000
#define TERRAIN_VERTEX_POINT_B_INSIDE_MIN_X 0x400000
#define TERRAIN_VERTEX_POINT_B_INSIDE_MIN_Y 0x800000
#define TERRAIN_VERTEX_POINT_B_INSIDE_MAX_X 0x1000000
#define TERRAIN_VERTEX_POINT_B_INSIDE_MAX_Y 0x2000000
#define TERRAIN_VERTEX_SECONDARY_VISIBLE 0x10000000
/* The four side bits of a point, and those plus its NOT_PROJECTED bit (all five bits of the point). */
#define TERRAIN_VERTEX_POINT_A_SIDE_BITS                                                                   \
  (TERRAIN_VERTEX_POINT_A_INSIDE_MIN_X | TERRAIN_VERTEX_POINT_A_INSIDE_MIN_Y | TERRAIN_VERTEX_POINT_A_INSIDE_MAX_X | \
   TERRAIN_VERTEX_POINT_A_INSIDE_MAX_Y) /* 0x1e0000 */
#define TERRAIN_VERTEX_POINT_A_BITS (TERRAIN_VERTEX_POINT_A_SIDE_BITS | TERRAIN_VERTEX_POINT_A_NOT_PROJECTED)
#define TERRAIN_VERTEX_POINT_B_SIDE_BITS                                                                   \
  (TERRAIN_VERTEX_POINT_B_INSIDE_MIN_X | TERRAIN_VERTEX_POINT_B_INSIDE_MIN_Y | TERRAIN_VERTEX_POINT_B_INSIDE_MAX_X | \
   TERRAIN_VERTEX_POINT_B_INSIDE_MAX_Y) /* 0x3c00000 */
#define TERRAIN_VERTEX_POINT_B_BITS (TERRAIN_VERTEX_POINT_B_SIDE_BITS | TERRAIN_VERTEX_POINT_B_NOT_PROJECTED)
/* Render context flag of TerrainProjectedGrid_TransformShadeAndQueue. The context is the world runtime and this is
   the bit named WORLD_RUNTIME_FLAG_FIELD_GRID_DIRTY there: FrontendModelPointerContext_RenderWorldViewQueuesClipped
   sets it after a frame that covered the whole view, every camera change clears it. While it is set (and the field
   grid is unchanged) the row spans and point-A projections of the previous frame are reused. */
#define TERRAIN_RENDER_REUSE_PROJECTION 0x800
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00506CD0 */
void TerrainProjectedOcclusion_AccumulateMaskAroundWorldPoint
          (uint64_t occupancyMaskBits,FieldGridRadiusUnits radiusWorldUnits,Q12 referenceHeightQ12,
          Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid);

/* 0x005099D0 */
bool FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint
          (FieldCellFlagMask cellFlagMask,TerrainOverlayCellRuntimeValue cellValue,
          FieldGridRadiusUnits radiusWorldUnits,Q12 worldYQ12,Q12 worldXQ12,
          FieldGridAsset *fieldGrid);

/* 0x0050A190 */
bool FieldGridTerrainOverlayVariantB_ApplyAroundWorldPoint
          (FieldCellFlagMask cellFlagMask,TerrainOverlayCellRuntimeValue cellValue,
          FieldGridRadiusUnits radiusWorldUnits,Q12 worldYQ12,Q12 worldXQ12,
          FieldGridAsset *fieldGrid);

/* 0x00500F50 */
void TerrainProjectedGrid_TransformShadeAndQueue
          (FieldGridAsset *fieldGrid,FrontendModelPointerContextRuntimeState17C *renderContext);

/* 0x005066D0 */
void TerrainProjectedOcclusion_TraceWedge0(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005067D0 */
void TerrainProjectedOcclusion_TraceWedge1(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005068D0 */
void TerrainProjectedOcclusion_TraceWedge2(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005069D0 */
void TerrainProjectedOcclusion_TraceWedge3(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00506AD0 */
void TerrainProjectedOcclusion_TraceWedge4(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00506BD0 */
void TerrainProjectedOcclusion_TraceWedge5(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00509580 */
void FieldGridTerrainOverlayVariantA_ApplyWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509640 */
void FieldGridTerrainOverlayVariantA_ApplyWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x005096F0 */
void FieldGridTerrainOverlayVariantA_ApplyWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x005097A0 */
void FieldGridTerrainOverlayVariantA_ApplyWedge3(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509860 */
void FieldGridTerrainOverlayVariantA_ApplyWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509910 */
void FieldGridTerrainOverlayVariantA_ApplyWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509DD0 */
void FieldGridTerrainOverlayVariantB_ApplyWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509E70 */
void FieldGridTerrainOverlayVariantB_ApplyWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509F10 */
void FieldGridTerrainOverlayVariantB_ApplyWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509FB0 */
void FieldGridTerrainOverlayVariantB_ApplyWedge3(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x0050A050 */
void FieldGridTerrainOverlayVariantB_ApplyWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x0050A0F0 */
void FieldGridTerrainOverlayVariantB_ApplyWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00500CE0 */
void TerrainProjectedQuad_QueueAsTwoTrianglesRegs
          (uint32_t rowStrideBytes,TerrainProjectedVertexWorkRecord *topLeftVertex,
          FrontendModelPointerContextRuntimeState17C *renderContext);

/* 0x005004A0 */
void TerrainProjectedVertex_TransformProjectAndShadeVariantA(TerrainProjectedVertexWorkRecord *vertex);

/* 0x005006A0 */
void TerrainProjectedVertex_TransformProjectAndShadeVariantB(TerrainProjectedVertexWorkRecord *vertex);

/* 0x00500820 */
void TerrainProjectedTriangle_ClipInterpolateAndQueueTextured
          (int surfacePacketIndex,TerrainProjectedVertexWorkRecord *vertex2,
          TerrainProjectedVertexWorkRecord *vertex1,TerrainProjectedVertexWorkRecord *vertex0,
          FrontendModelPointerContextRuntimeState17C *renderContext);

/* 0x00500D30 */
void TerrainProjectedGrid_ClipRowSpansAgainstPlane(FieldGridAsset *fieldGrid,GraphicsFixedVec3 *planeNormal);

/* 0x005063B0 */
void TerrainProjectedOcclusion_ScanDirection0(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00506430 */
void TerrainProjectedOcclusion_ScanDirection1(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005064C0 */
void TerrainProjectedOcclusion_ScanDirection2(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00506540 */
void TerrainProjectedOcclusion_ScanDirection3(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005065C0 */
void TerrainProjectedOcclusion_ScanDirection4(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00506650 */
void TerrainProjectedOcclusion_ScanDirection5(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00509320 */
void FieldGridTerrainOverlayVariantA_ApplyDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509380 */
void FieldGridTerrainOverlayVariantA_ApplyDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x005093F0 */
void FieldGridTerrainOverlayVariantA_ApplyDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509450 */
void FieldGridTerrainOverlayVariantA_ApplyDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x005094B0 */
void FieldGridTerrainOverlayVariantA_ApplyDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509520 */
void FieldGridTerrainOverlayVariantA_ApplyDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509B90 */
void FieldGridTerrainOverlayVariantB_ApplyDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509BF0 */
void FieldGridTerrainOverlayVariantB_ApplyDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509C50 */
void FieldGridTerrainOverlayVariantB_ApplyDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509CB0 */
void FieldGridTerrainOverlayVariantB_ApplyDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509D10 */
void FieldGridTerrainOverlayVariantB_ApplyDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

/* 0x00509D70 */
void FieldGridTerrainOverlayVariantB_ApplyDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

#endif /* THANDOR_WORLD_TERRAIN_PROJECTION_H */
