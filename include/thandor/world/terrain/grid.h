/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/terrain/grid.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_TERRAIN_GRID_H
#define THANDOR_WORLD_TERRAIN_GRID_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/terrain/grid. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* Field-grid cell flag bits (FieldGridCell.flagsAndMaterial, +0x50) beyond the generated
   FieldCellPackedFlagsAndMaterial enum. FieldGrid_InitializeRuntimeCellsAndBoundaryFlags sets the four
   map-edge bits on the outermost ring of cells; neighbour loops test them before touching a neighbour. */
#define FIELD_CELL_LAST_ROW_BOUNDARY 0x80000000u
#define FIELD_CELL_GRID_EDGE_MASK                                                                  \
  (FIELD_CELL_LAST_ROW_BOUNDARY | FIELD_CELL_LAST_COLUMN_BOUNDARY | FIELD_CELL_FIRST_ROW_BOUNDARY | \
   FIELD_CELL_FIRST_COLUMN_BOUNDARY) /* 0x88006000 */

/* World plane to field-grid coordinates (FieldGrid_WorldToGridQ12 and the samplers that inline it): the grid
   is a triangular lattice, grid columns per world unit in Q20 (about 1 / 0.5625) and grid rows per world unit in
   Q20, negative because rows grow towards -Y (about -2.05). The column is then skewed by half the row. */
#define FIELD_GRID_WORLD_X_TO_COLUMN_Q20 0x1c6e9c
#define FIELD_GRID_WORLD_Y_TO_ROW_Q20 (-0x20c8cc)

/* FieldGridCell.occupancyMask (+0x70) holds one occupancy byte per faction slot 0..7 (the tick wheel
   indexes it with WorldRuntimeContext.activeFactionRuntimeIndex). Bit meanings inside a byte as far as
   the tick-wheel code shows them: */
#define FIELD_CELL_OCCUPANCY_BIT0 0x01            /* set/cleared grid-wide for one faction by the Bit0 helpers */
#define FIELD_CELL_OCCUPANCY_REBUILT_BITS 0x7f    /* bits 0..6: cleared before every occupancy rebuild */
#define FIELD_CELL_OCCUPANCY_PERSISTENT_BIT 0x80  /* bit 7: survives the rebuild clear */
#define FIELD_CELL_OCCUPANCY_PRESENCE_BITS 0xf9   /* bits that count as "faction present" (1 and 2 excluded) */
#define FIELD_CELL_OCCUPANCY_CURRENT_PRESENCE_BITS 0x79 /* the presence bits without the persistent bit 7
                                                           (FieldGrid_ClassifyCellFlagsToRuntimeByte, minimap) */
/* a byte mask moved into the faction slot's byte of the 64-bit occupancyMask */
#define FIELD_CELL_OCCUPANCY_SLOT_MASK(bits,factionSlot) ((uint64_t)(bits) << ((factionSlot) * 8))

/* FieldGridCell.runtime60_6B spans +0x60..+0x6B; code indexes through it past its end, so these are the
   array indices of the fields it reaches. */
#define FIELD_CELL_RUNTIME60_INDEX_RUNTIME_BYTE68 8      /* +0x68: runtime class byte */
#define FIELD_CELL_RUNTIME60_INDEX_OCCUPANCY_MASK 0x10   /* +0x70: occupancyMask byte 0 */

/* FieldGridAsset.runtimeStateFlags bit 0: set by every height/cell edit, cleared by the projection pass
   (projection.c) after it rebuilt the terrain surface. */
#define FIELD_GRID_RUNTIME_SURFACE_DIRTY 0x01
/* FieldGrid_RaycastTerrainSurfaceDistance / ..SecondarySurfaceDistance: at most this many cell steps per ray
   (the counter is decremented before the first step, so 1023 cells are visited), and the miss distance. */
#define FIELD_GRID_RAYCAST_MAX_STEPS 1024
#define FIELD_GRID_RAYCAST_MISS_DISTANCE 0x7fffffff
/* One grid cell in Q12 grid coordinates; masking with ~(FIELD_GRID_CELL_Q12 - 1) keeps the cell origin. */
#define FIELD_GRID_CELL_Q12 0x1000
/* occupancy bits 0 and 1 of a faction byte: positioned sounds only play in cells where one of them is set
   (TerrainGrid_TestProjectedCellMaskBits01) */
#define FIELD_CELL_OCCUPANCY_BITS01 0x03
/* occupancy bit 1, set within an army's radius by TerrainOccupancyBit2_MarkAroundWorldPoint (the "Bit2" in the
   TerrainOccupancyBit2_* names is the mask value 2) */
#define FIELD_CELL_OCCUPANCY_BIT1 0x02

/* 0x00505930 */
void FieldGrid_ApplyRadialTerrainHeightDeltaAndRefreshSurface
          (TerrainMaterialIndex terrainMaterialIndexOrNegativeSentinel,
          FieldGridRadiusUnits radiusWorldUnits,Q12 terrainHeightDeltaAmplitudeQ12,
          Q12 centerWorldYQ12,Q12 centerWorldXQ12,FieldGridAsset *fieldGrid);

/* 0x00562330 */
void TerrainGrid_RunDirectionalRelaxationPasses(FrontendPlayerRuntimeId playerRuntimeId,uint32_t reservedZero,
          TerrainRelaxationPassCount passCount,TerrainRelaxationMode mode);

/* 0x005610A0 */
void FieldGrid_ApplyPositiveCellDeltas(PlayerRuntimeId playerRuntimeId,Q12 anchorRowQ12,Q12 anchorColumnQ12,
          PackedFieldGridDeltaXY16 packedDragDeltaXY16);

/* 0x005613C0 */
void FieldGrid_ApplyNegativeCellDeltas(PlayerRuntimeId playerRuntimeId,Q12 anchorRowQ12,Q12 anchorColumnQ12,
          PackedFieldGridDeltaXY16 packedDragDeltaXY16);

/* 0x00561C10 */
void FieldGrid_RebuildLocalInfluenceState
          (PlayerRuntimeId playerRuntimeId,FieldGridCommandReservedValue reservedCommandValue,
          Q12 gridRowQ12,Q12 gridColumnQ12);

/* 0x00505620 */
void FieldGrid_RecomputeInteriorTriangleNormalAngles(FieldGridAsset *fieldGrid);

/* 0x00505700 */
void FieldGrid_RecomputeInteriorDirectionalLighting
          (AngleTurn32 lightElevationAngle,AngleTurn32 lightAzimuthAngle,FieldGridAsset *fieldGrid);

/* 0x005090E0 */
void FieldGrid_ApplyHeightAtWorldPointAndRefreshNeighbors
          (TerrainHeightBrushDeltaSource heightDeltaSourceValue,Q12 worldZQ12,Q12 worldYQ12,
          Q12 worldXQ12,FieldGridAsset *fieldGrid);

/* 0x005618A0 */
void FieldGrid_ApplyLocalCellUpdate
          (PlayerRuntimeId playerRuntimeId,FieldGridTransitionValue transitionValue,Q12 gridRowQ12,
          Q12 gridColumnQ12);

/* 0x00562390 */
void FieldGrid_ApplyEncodedCellUpdate(PlayerRuntimeId playerRuntimeId,Q12 gridRowQ12,Q12 gridColumnQ12,
          PackedFieldGridDeltaXY16 packedDragDeltaXY16);

/* 0x005623D0 */
void FieldGrid_ApplyMaskDFFFFFFF
          (PlayerRuntimeId playerRuntimeId,FieldGridRegionMask setMask,Q12 gridRowQ12,Q12 gridColumnQ12);

/* 0x00562410 */
void FieldGrid_ApplyMaskBFFFFFFF
          (PlayerRuntimeId playerRuntimeId,FieldGridRegionMask setMask,Q12 gridRowQ12,Q12 gridColumnQ12);

/* 0x00562450 */
void FieldGrid_ApplyCallerMask
          (PlayerRuntimeId playerRuntimeId,FieldGridMaterialBitIndex materialBitIndex,Q12 gridRowQ12,
          Q12 gridColumnQ12);

/* 0x004FEA80 */
TerrainPointResult
FieldGrid_GetNearestTerrainPoint(Q12 worldY,Q12 worldX,FieldGridAsset *field);

/* 0x004FEB10 */
SurfacePointResult
FieldGrid_GetNearestTopSurfacePoint(Q12 worldY,Q12 worldX,FieldGridAsset *field);

/* 0x004FEBA0 */
int32_t FieldGrid_GetNearestWaterDelta(Q12 worldY,Q12 worldX,FieldGridAsset *field);

/* 0x004FEC10 */
HeightSampleResult FieldGrid_InterpolateTerrainHeight(Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid);

/* 0x004FED50 */
int32_t FieldGrid_InterpolateWaterDelta(Q12 worldY,Q12 worldX,FieldGridAsset *field);

/* 0x004FEE90 */
HeightSampleResult FieldGrid_InterpolateWaterSurfaceHeight(Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid);

/* 0x004FEFF0 */
HeightSampleResult FieldGrid_InterpolateTopSurfaceHeight(Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid);

/* 0x004FF1A0 */
HeightNormalSampleResult FieldGrid_InterpolateTerrainHeightAndNormal(Q12 worldY,Q12 worldX,FieldGridAsset *field);

/* 0x004FF3D0 */
HeightNormalSampleResult FieldGrid_InterpolateTerrainHeightAndTriangle0Normal
          (Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid);

/* 0x004FF600 */
HeightNormalSampleResult FieldGrid_InterpolateTerrainHeightAndTriangle1Normal
          (Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid);

/* 0x004FF830 */
HeightNormalSampleResult FieldGrid_SampleInterpolatedTerrainHeightAndNormalAnglesRegs
          (GraphicsWorldCoordinateQ12 worldYQ12,GraphicsWorldCoordinateQ12 worldXQ12,
          FieldGridAsset *fieldGrid);

/* 0x004FFB80 */
bool FieldGrid_TestWorldPointBlocked
          (FieldGridByteOffset factionSlot,Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid
          );

/* 0x00503C90 */
void FieldGrid_InitializeRuntimeCellsAndBoundaryFlags(FieldGridAsset *fieldGrid);

/* 0x00503DB0 */
void FieldGrid_RebuildCellLookupPointers(FieldGridAsset *fieldGrid);

/* 0x00503E20 */
void FieldGrid_ApplyByteClampLookupToCells(FieldGridByteOffset factionIndex,FieldGridAsset *fieldGrid);

/* 0x00503E80 */
void FieldGrid_ClassifyCellFlagsToRuntimeByte(FieldGridByteOffset factionSlot,FieldGridAsset *fieldGrid);

/* 0x00503EE0 */
void TerrainDirectionTable_AdvanceAndRebuildVectors(void);

/* 0x00504B10 */
TerrainRaycastResult FieldGrid_RaycastTerrainSurfaceDistance
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,Q12 rayScaleQ12,Q12 rayOriginZQ12,
          Q12 rayOriginYQ12,Q12 rayOriginXQ12,FieldGridAsset *fieldGrid);

/* 0x00504CA0 */
TerrainRaycastResult FieldGrid_RaycastSecondarySurfaceDistance
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,Q12 rayScaleQ12,Q12 rayOriginZQ12,
          Q12 rayOriginYQ12,Q12 rayOriginXQ12,FieldGridAsset *fieldGrid);

/* 0x00504E60 */
TerrainRaycastResult FieldGrid_RaycastTerrainTrianglesAlongDirection
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,FixedMathScale32 rayScaleQ12,
          Q12 rayOriginZQ12,Q12 rayOriginYQ12,Q12 rayOriginXQ12,FieldGridAsset *fieldGrid);

/* 0x00505120 */
void FieldGrid_ClearOccupancyMaskBits0To6AllCells(FieldGridAsset *fieldGrid);

/* 0x00505240 */
void FieldGrid_SetOccupancyMaskByteBit0AllCells
          (FieldGridOccupancyByteIndex occupancyMaskByteIndex,FieldGridAsset *fieldGrid);

/* 0x00505290 */
void FieldGrid_ClearOccupancyMaskByteBit0AllCells
          (FieldGridOccupancyByteIndex occupancyMaskByteIndex,FieldGridAsset *fieldGrid);

/* 0x00507580 */
bool TerrainGrid_TestProjectedCellMaskBits01(Q12 worldYQ12,Q12 worldXQ12,WorldRuntimeContext *worldRuntime);

/* 0x005092A0 */
void FieldGrid_ClearCellFlag8000AcrossGrid(FieldGridAsset *fieldGrid);

/* 0x005092E0 */
void FieldGrid_SetAllCellOverlayColors(PackedArgb32 argbColor,FieldGridAsset *fieldGrid);

/* 0x00532B60 */
StatusResult FieldGrid_SaveAssetImageFromRuntimeState(uint32_t *sourceImageDwords);

/* 0x00561050 */
void FieldGrid_ClearPlayerScratchPlane
          (PlayerRuntimeId playerRuntimeId,FieldGridCommandReservedValue reservedCommandValue,
          Q12 reservedWorldYQ12,Q12 reservedWorldXQ12);

/* 0x00561BB0 */
void FieldGrid_ResetLocalInfluenceState
          (PlayerRuntimeId playerRuntimeId,FieldGridCommandReservedValue reservedCommandValue,
          Q12 reservedWorldYQ12,Q12 reservedWorldXQ12);

/* 0x00571EC0 */
void FieldGrid_ApplyEncodedUpdateCore(FieldGridHeightDeltaUnits heightDeltaUnits,Q12 gridRowQ12,Q12 gridColumnQ12,
          FieldGridAsset *fieldGrid);

/* 0x005058A0 */
void FieldGridCell_ApplyRadialTerrainHeightDeltaAndMaterial(TerrainMaterialIndex terrainMaterialIndexOrNegativeSentinel,
          FieldGridRadiusUnits radiusWorldUnits,Q12 terrainHeightDeltaAmplitudeQ12,
          Q12 centerWorldYQ12,Q12 centerWorldXQ12,FieldGridCell *cell);

/* 0x00505AA0 */
void TerrainGrid_RelaxNeighborHeightsForwardWithSignGate(FieldGridAsset *fieldGrid);

/* 0x00505BE0 */
void TerrainGrid_RelaxNeighborHeightsReverseWithSignGate(FieldGridAsset *fieldGrid);

/* 0x00505D30 */
void TerrainGrid_RelaxNeighborHeightsForward(FieldGridAsset *fieldGrid);

/* 0x00505E60 */
void TerrainGrid_RelaxNeighborHeightsReverse(FieldGridAsset *fieldGrid);

/* 0x00571090 */
void FieldGrid_ProcessHorizontalSpan(Q12 sourceRowQ12,Q12 sourceColumnQ12,FieldGridHeightDeltaUnits heightDeltaUnits,
          FieldGridRadiusUnits radiusUnits,Q12 centerRowQ12,Q12 centerColumnQ12,
          FieldGridAccumulatorValue *accumulatorPlane,FieldGridAsset *fieldGrid);

/* 0x00571250 */
void FieldGrid_ProcessVerticalSpan(FieldGridHeightDeltaUnits heightDeltaUnits,FieldGridRadiusUnits radiusUnits,
          Q12 centerRowQ12,Q12 centerColumnQ12,FieldGridAccumulatorValue *accumulatorPlane,
          FieldGridAsset *fieldGrid);

/* 0x005713E0 */
void FieldGrid_ApplySingleCellTransition(FieldGridTransitionValue transitionValue,Q12 gridRowQ12,Q12 gridColumnQ12,
          FieldGridAsset *fieldGrid);

/* 0x00571860 */
void FieldGrid_ApplyRectangularTransition(Q12 gridRowQ12,Q12 gridColumnQ12,FieldGridAsset *fieldGrid);

/* 0x004FEA50 */
FieldGridCoordinatesEaxEdx8 FieldGrid_WorldToGridQ12(Q12 worldY,Q12 worldX);

/* 0x00571FE0 */
void FieldGrid_ApplyMaskedRegionCore
          (FieldGridRegionMask preserveMask,FieldGridRegionMask setMask,Q12 gridRowQ12,Q12 gridColumnQ12,
          FieldGridAsset *fieldGrid);

/* 0x005052E0 */
void FieldGridCell_RecomputeTriangleNormalAngles(FieldGridRowStrideBytes rowStrideBytes,FieldGridCell *cell);

/* 0x00505690 */
void FieldGridCell_ComputeDirectionalLightColor(FieldGridCell *cell);

#endif /* THANDOR_WORLD_TERRAIN_GRID_H */
