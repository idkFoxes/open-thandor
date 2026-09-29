/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/pathing/grid.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_PATHING_GRID_H
#define THANDOR_WORLD_PATHING_GRID_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/pathing/grid. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* GridScratchCell.stateMask bits beyond the generated GridScratchStateMask enum. The scratch grid has 4x4
   cells per field cell. Bits 1..7 are set when faction slot 1..7 occupies the source field cell (bit n =
   slot n, from the FIELD_CELL_OCCUPANCY_PRESENCE_BITS of occupancy byte n); bits 8..23 are the radial
   distance bands written by the world/pathing/influence writers. */
#define GRID_SCRATCH_LOW_DISTANCE_BANDS 0x0000ff00u  /* bits 8..15 */
#define GRID_SCRATCH_HIGH_DISTANCE_BANDS 0x00ff0000u /* bits 16..23 */
#define GRID_SCRATCH_DISTANCE_BANDS (GRID_SCRATCH_LOW_DISTANCE_BANDS | GRID_SCRATCH_HIGH_DISTANCE_BANDS)
#define GRID_SCRATCH_BLOCKED 0x80000000u         /* bit 31: map-edge field cell; projected tests reject it */
#define GRID_SCRATCH_REBUILD_KEEP_BITS 0x00ffff01 /* visited bit and the distance bands survive the terrain rebuild */

/* Scratch-grid geometry and path costs (EntityPathing_* / GridPathCost_* / GridFootprint_* / GridInfluence_*).
   World -> scratch cell: the field-grid Q12 coordinate (FIELD_GRID_WORLD_X_TO_COLUMN_Q20 /
   FIELD_GRID_WORLD_Y_TO_ROW_Q20, column skewed by half the row) plus GRID_SCRATCH_INDEX_BIAS_Q12, shifted right
   by GRID_SCRATCH_CELL_SHIFT. Scratch cell -> world: index * GRID_SCRATCH_CELL_Q12 - GRID_SCRATCH_CELL_CENTER_Q12
   is the Q12 field-grid coordinate of the cell centre, scaled by FIELD_GRID_WORLD_COLUMN_STEP_X /
   FIELD_GRID_WORLD_ROW_STEP_Y. */
#define GRID_SCRATCH_CELL_Q12 0x400        /* one scratch cell (a quarter field cell) in Q12 field-grid units */
#define GRID_SCRATCH_CELL_SHIFT 10
#define GRID_SCRATCH_INDEX_BIAS_Q12 0x800  /* scratch index 0 lies two scratch cells before field-grid 0 */
#define GRID_SCRATCH_CELL_CENTER_Q12 0x600 /* bias minus half a scratch cell */
#define GRID_SCRATCH_COLUMN_WORLD_X 0x240  /* world X between neighbouring scratch columns (0x901 / 4) */
#define GRID_SCRATCH_HALF_COLUMN_WORLD_X 0x120 /* world X skew of the next scratch row */
#define GRID_SCRATCH_ROW_PAIR_WORLD_Y 999  /* world Y of two scratch rows: one step of the footprint/band walkers */
#define GRID_SCRATCH_ROW_ABOVE_WORLD_Y 499 /* world Y from a scratch row to the row above (rounded down) */
#define GRID_SCRATCH_ROW_BELOW_WORLD_Y 500 /* world Y from a scratch row to the row below */
#define GRID_FOOTPRINT_RADIUS_MARGIN 499   /* added to every footprint/influence radius before squaring */
#define GRID_SCRATCH_LOW_BAND0 0x100       /* lowest low-distance band bit; << grid class = that class's band */
#define GRID_SCRATCH_HIGH_BAND0 0x10000    /* lowest high-distance band bit */
/* GridScratchCell.pathCost: reset to GRID_PATH_COST_UNREACHED, 0 at the propagation origin; each hex step costs
   GRID_PATH_STEP_COST, less on the moving faction's own cells and more on its high-cost cells. */
#define GRID_PATH_COST_UNREACHED 0x7fffffff
#define GRID_PATH_COST_MAX_REACHED 0x7ffffffe
#define GRID_PATH_STEP_COST 4
#define GRID_PATH_STEP_COST_OWN_FACTION 3
#define GRID_PATH_STEP_COST_HIGH 12
#define GRID_PATH_PROPAGATION_PASSES 6             /* passes of GridPathCost_PropagateWeightedHexNeighbors */
#define GRID_PATH_COST_QUEUE_PASS_ENTRIES 0x10000  /* queue entries per propagation pass */
#define GRID_PATH_NEAREST_SEARCH_RADIUS 16         /* GridPathCost_FindNearestUnblockedCell scans +-16 cells */
/* GridReachability_MarkOpenRegionRecursive stops at blocked cells, terrain classes 28..30, low bands 0..6 and
   visited cells */
#define GRID_REACHABILITY_OPEN_STOP_MASK 0xf0007f01

/* 0x005349D0 */
PathingDestinationResult
EntityPathing_ResolveDestinationAndRebuildRoutes
          (UQ12 targetWorldYQ12,UQ12 targetWorldXQ12,GameEntityRuntime *routeEntityRuntime,
          WorldRuntimeContext *worldRuntime);

/* 0x00536500 */
bool GridReachability_RebuildConnectedRegionAroundWorldPoint
          (FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12);

/* 0x00533620 */
void GridScratch_RebuildTerrainAndRuntimeClassificationMasks(WorldRuntimeContext *worldRuntime);

/* 0x00533E70 */
bool GridScratch_TestRuntimePairReachabilityFromWorldPoint
          (WorldPointXYQ12 *sourceWorldPoint,GridReachabilityRuntimePair *targetRuntimePair);

/* 0x005332C0 */
GridScratchAllocResult GridScratch_AllocateForFieldGrid(FieldGridAsset *fieldGrid);

/* 0x00533360 */
void GridScratch_ReleaseBuffers(void);

/* 0x00533400 */
void GridScratch_PropagateFieldOccupancyMaskNeighborhood(FieldGridAsset *fieldGrid);

/* 0x00533BA0 */
bool GridScratch_TestProjectedCellMaskBands(Q12 worldYQ12,Q12 worldXQ12,uint8_t lowBandIndex,uint8_t highBandIndex);

/* 0x00536C90 */
WorldPositionXYEaxEdx8
EntityPathing_RebuildOverlappingGroupRoutes
          (UQ12 targetWorldY,UQ12 targetWorldX,GameEntityRuntime *routeEntityRuntime,
          WorldRuntimeContext *worldRuntime);

/* 0x00534F50 */
void GridFootprint_ClearTraversalFlagsAroundWorldPoint
          (FieldGridRadiusUnits radiusWorldUnits,Q12 worldYQ12,Q12 worldXQ12);

/* 0x005369A0 */
WorldPositionXYEaxEdx8 EntityPathing_UpdateRouteSegment
          (UQ12 targetWorldYQ12,UQ12 targetWorldXQ12,GameEntityRuntime *sourceRouteEntityRuntime,
          EntityPathingRouteEntityRuntimeView *routeEntityRuntime);

/* 0x00533D60 */
bool GridScratch_TestWorldPointReachability(uint32_t traversalMask,GraphicsWorldCoordinateQ12 sourceWorldYQ12,
          GraphicsWorldCoordinateQ12 sourceWorldXQ12,GraphicsWorldCoordinateQ12 targetWorldYQ12,
          GraphicsWorldCoordinateQ12 targetWorldXQ12);

/* 0x00534660 */
PathBacktrackResult GridPathCost_BacktrackBestHexRoute
          (FieldGridRegionMask callerBlockingMask,FieldGridCellCoordinate startRow,
          FieldGridCellCoordinate startColumn,GridScratchCell *startCell);

/* 0x00534960 */
GridPathMarkedRegionCellRegisterResult GridPathRegion_MarkUnreachableFromCell (GridPathUnreachableReferenceRow32 referenceRow, GridPathUnreachableReferenceColumn32 referenceColumn,FieldGridCellCoordinate row, FieldGridCellCoordinate column);

/* 0x005333B0 */
void __cdecl GridScratch_CopyPrimaryToSecondary(void);

/* 0x005333E0 */
void GridScratch_SwapPrimarySecondary(void);

/* 0x00533580 */
void GridScratch_FloodFillConnectedCellsRegs
          (GridScratchStateMask traversalMask,uint32_t rowStrideBytes,GridScratchCell *currentCell);

/* 0x00533C50 */
bool GridScratch_TestConnectedReachabilityRecursiveRegs
          (uint32_t traversalMask,uint32_t rowStrideBytes,uint32_t *currentCell,uint32_t *targetCell);

/* 0x00533EF0 */
void GridPathCost_PropagateWeightedHexNeighbors(GridPathPassCount remainingPasses,GridScratchCell *originCell,
          FieldGridCellCoordinate startRow,FieldGridCellCoordinate startColumn);

/* 0x00534200 */
void GridScratch_ResetTraversalFlagsAndCosts(void);

/* 0x00534780 */
GridPathUnreachableRecursiveEdiEdx8 GridPathRegion_MarkUnreachableRecursive
          (uint32_t rowStrideBytes,GridScratchCell *currentCell,GridPathCost bestCost,
          uint32_t bestCellByteOffset);

/* 0x00534E70 */
int GridFootprint_ClearTraversalFlagsDiagonalNegative
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,uint32_t *scratchRecord);

/* 0x00534EE0 */
int GridFootprint_ClearTraversalFlagsDiagonalPositive
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,uint32_t *scratchRecord);

/* 0x005363C0 */
void GridReachability_MarkOpenRegionRecursive(uint32_t rowStrideBytes,GridScratchCell *currentCell);

/* 0x00536440 */
void GridReachability_ClearCostedRegionRecursive(uint32_t rowStrideBytes,GridScratchCell *currentCell);

/* 0x005342F0 */
NearestCellResult GridPathCost_FindNearestUnblockedCell(FieldGridCellCoordinate cellRow,FieldGridCellCoordinate cellColumn);

/* 0x005344B0 */
bool GridPathLine_TestHexSegmentClear(FieldGridRegionMask callerBlockingMask,FieldGridCellCoordinate startRow,
          FieldGridCellCoordinate startColumn,GridScratchCell *startCell,GridScratchCell *endCell);

#endif /* THANDOR_WORLD_PATHING_GRID_H */
