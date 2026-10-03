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
#define FIELD_CELL_OCCUPANCY_EXPLORED_BITS 0xf8   /* bits 3..7: the faction has explored the cell (exploration score) */
/* a byte mask moved into the faction slot's byte of the 64-bit occupancyMask */
#define FIELD_CELL_OCCUPANCY_SLOT_MASK(bits,factionSlot) ((uint64_t)(bits) << ((factionSlot) * 8))
/* the faction slot's occupancy byte of a cell (an lvalue) */
#define FIELD_CELL_OCCUPANCY_BYTE(cell,factionSlot) (((uint8_t *)&(cell)->occupancyMask)[factionSlot])
/* the cell byteOffset bytes away from cell; byteOffset is usually +-the row stride (one grid row) */
#define FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,byteOffset) ((FieldGridCell *)((uint8_t *)(cell) + (byteOffset)))

/* FieldGridCell.visibilityLightingIndex (+0x68) as FieldGrid_ClassifyCellFlagsToRuntimeByte sets it; the
   projection pass indexes g_PackedLightingLookupTable with it, FIELD_CELL_LIGHTING_VISIBLE selects the dynamic
   lights instead. */
#define FIELD_CELL_LIGHTING_VISIBLE 0xff     /* a current presence bit of the faction is set */
#define FIELD_CELL_LIGHTING_EXPLORED 0x87    /* only the persistent occupancy bit 7 is set */
#define FIELD_CELL_LIGHTING_UNEXPLORED 0x00

/* FieldGridAsset.runtimeStateFlags bit 0: set by every height/cell edit, cleared by the projection pass
   (projection.c) after it rebuilt the terrain surface. */
#define FIELD_GRID_RUNTIME_SURFACE_DIRTY 0x01
/* FieldGrid_RaycastTerrainSurfaceDistance / ..SecondarySurfaceDistance: at most this many cell steps per ray
   (the counter is decremented before the first step, so 1023 cells are visited), and the miss distance. */
#define FIELD_GRID_RAYCAST_MAX_STEPS 1024
#define FIELD_GRID_RAYCAST_MISS_DISTANCE 0x7fffffff
/* One grid cell in Q12 grid coordinates; masking with ~(FIELD_GRID_CELL_Q12 - 1) keeps the cell origin. */
#define FIELD_GRID_CELL_Q12 0x1000
/* Lattice cell of a Q12 grid position (the samplers after FieldGrid_WorldToGridQ12): with the Q12 fractions f
   (column) and g (row), f + 2g and 2f + g compared with one and two cells (FIELD_GRID_CELL_Q12,
   FIELD_GRID_TWO_CELLS_Q12) pick the cell of the triangle the point lies in. */
#define FIELD_GRID_TWO_CELLS_Q12 0x2000
/* gridWidth as the original recovers it from the row stride (gridWidth * sizeof(FieldGridCell), SHL 7 then
   SHR 7): the 25 width bits that survive the stride multiply */
#define FIELD_GRID_ROW_STRIDE_WIDTH_MASK 0x1ffffff
/* sqrt(3) in Q12 (7094): FieldGrid_ApplyRadialTerrainHeightDeltaAndRefreshSurface scales the radius by it for the X
   half-extent of the box around the circle */
#define FIELD_GRID_SQRT3_Q12 0x1bb6
/* Editor drag brushes (FieldGrid_ApplyEncodedUpdateCore, FieldGrid_ProcessHorizontalSpan/VerticalSpan): one drag
   unit moves a height or water level by 64 (Q12) and widens a brush radius by 64 world units; the radius is
   clamped to FIELD_GRID_EDIT_BRUSH_RADIUS_MAX. */
#define FIELD_GRID_EDIT_DRAG_UNIT_Q12 0x40
#define FIELD_GRID_EDIT_BRUSH_RADIUS_MAX 0x5000
/* occupancy bits 0 and 1 of a faction byte: positioned sounds only play in cells where one of them is set
   (TerrainGrid_TestProjectedCellMaskBits01) */
#define FIELD_CELL_OCCUPANCY_BITS01 0x03
/* occupancy bit 1, set within an army's radius by TerrainOccupancyBit2_MarkAroundWorldPoint (the "Bit2" in the
   TerrainOccupancyBit2_* names is the mask value 2) */
#define FIELD_CELL_OCCUPANCY_BIT1 0x02
/* Two FieldGridCells in bytes: the byte-addressed reverse water relaxation passes step two rows (upper
   neighbour row to lower neighbour row) as rowLength * this, and back over the two border cells at a row end. */
#define FIELD_GRID_TWO_CELLS_BYTES (2 * sizeof(FieldGridCell))
/* Z of the unnormalised cell normal (FieldGridCell_RecomputeTriangleNormalAngles), 3 * 2048^2: the sum of the
   squared X offsets of the six lattice neighbours for a cell spacing of 2048 world units, so a plane's X/Y tilt
   sums come out against it roughly to scale (the real spacing is 2305, FIELD_GRID_WORLD_COLUMN_STEP_X). */
#define FIELD_GRID_NORMAL_Z_COMPONENT 0xc00000
/* Height-drag brush falloff (FieldGrid_ProcessHorizontalSpan/VerticalSpan): the decompiled 64-bit
   distance * FIXED_ANGLE16_HALF_TURN keeps bits 0..48 of the sign-extended distance and shifts them right by 17
   to get the product's high dword (a plain 64-bit multiply compiles differently). */
#define FIELD_GRID_ANGLE_PRODUCT_HIGH_BITS_MASK 0x1ffffffffffffU

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
void FieldGrid_SetCellFluidReceiverExcluded
          (PlayerRuntimeId playerRuntimeId,FieldGridRegionMask setMask,Q12 gridRowQ12,Q12 gridColumnQ12);

/* 0x00562410 */
void FieldGrid_SetCellFluidSourceExcluded
          (PlayerRuntimeId playerRuntimeId,FieldGridRegionMask setMask,Q12 gridRowQ12,Q12 gridColumnQ12);

/* 0x00562450 */
void FieldGrid_SetCellResourceSupportFlag
          (PlayerRuntimeId playerRuntimeId,FieldGridMaterialBitIndex materialBitIndex,Q12 gridRowQ12,
          Q12 gridColumnQ12);

/* 0x004FEA80 */
bool FieldGrid_GetNearestTerrainPoint(Q12 worldY,Q12 worldX,FieldGridAsset *field,FixedVectorQ12 *outPoint);

/* 0x004FEB10 */
bool FieldGrid_GetNearestTopSurfacePoint(Q12 worldY,Q12 worldX,FieldGridAsset *field,FixedVectorQ12 *outPoint);

/* 0x004FEBA0 */
int32_t FieldGrid_GetNearestWaterDelta(Q12 worldY,Q12 worldX,FieldGridAsset *field);

/* 0x004FEC10 */
bool FieldGrid_InterpolateTerrainHeight(Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid,Q12 *outHeightQ12);

/* 0x004FED50 */
int32_t FieldGrid_InterpolateWaterDelta(Q12 worldY,Q12 worldX,FieldGridAsset *field);

/* 0x004FEE90 */
bool FieldGrid_InterpolateWaterSurfaceHeight(Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid,Q12 *outHeightQ12);

/* 0x004FEFF0 */
bool FieldGrid_InterpolateTopSurfaceHeight(Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid,Q12 *outHeightQ12);

/* 0x004FF1A0 */
bool FieldGrid_InterpolateTerrainHeightAndNormal
          (Q12 worldY,Q12 worldX,FieldGridAsset *field,Q12 *outHeightQ12,uint32_t *outPackedNormalAngles);

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
bool FieldGrid_RaycastTerrainSurfaceDistance
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,Q12 rayScaleQ12,Q12 rayOriginZQ12,
          Q12 rayOriginYQ12,Q12 rayOriginXQ12,FieldGridAsset *fieldGrid,Q12 *outDistanceQ12,
          uint32_t *outMaterialIndex);

/* 0x00504CA0 */
bool FieldGrid_RaycastSecondarySurfaceDistance
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,Q12 rayScaleQ12,Q12 rayOriginZQ12,
          Q12 rayOriginYQ12,Q12 rayOriginXQ12,FieldGridAsset *fieldGrid,Q12 *outDistanceQ12);

/* 0x00504E60 */
bool FieldGrid_RaycastTerrainTrianglesAlongDirection
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,FixedMathScale32 rayScaleQ12,
          Q12 rayOriginZQ12,Q12 rayOriginYQ12,Q12 rayOriginXQ12,FieldGridAsset *fieldGrid,
          Q12 *outDistanceQ12);

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
void FieldGrid_ClearDebugMarkInAllCells(FieldGridAsset *fieldGrid);

/* 0x005092E0 */
void FieldGrid_SetAllCellOverlayColors(PackedArgb32 argbColor,FieldGridAsset *fieldGrid);

/* 0x00532B60 */
bool FieldGrid_SaveAssetImageFromRuntimeState(uint32_t *sourceImageDwords,uint32_t *outError);

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
FieldGridCoordinates FieldGrid_WorldToGridQ12(Q12 worldY,Q12 worldX);

/* 0x00571FE0 */
void FieldGrid_ApplyMaskedRegionCore
          (FieldGridRegionMask preserveMask,FieldGridRegionMask setMask,Q12 gridRowQ12,Q12 gridColumnQ12,
          FieldGridAsset *fieldGrid);

/* 0x005052E0 */
void FieldGridCell_RecomputeTriangleNormalAngles(FieldGridRowStrideBytes rowStrideBytes,FieldGridCell *cell);

/* 0x00505690 */
void FieldGridCell_ComputeDirectionalLightColor(FieldGridCell *cell);

#endif /* THANDOR_WORLD_TERRAIN_GRID_H */
