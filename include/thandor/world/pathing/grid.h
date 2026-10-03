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
#define GRID_PATH_NEAREST_SEARCH_RADIUS 16         /* GridPathCost_RelocateFromBlockedCell scans +-16 cells */
/* bytes of the cost queue buffer (g_GridPathCostQueueBegin..End): one pass worth of cell pointers per
   propagation pass (0x180000) */
#define GRID_PATH_COST_QUEUE_BYTES \
  (GRID_PATH_PROPAGATION_PASSES * GRID_PATH_COST_QUEUE_PASS_ENTRIES * sizeof(GridScratchCell *))
/* GridReachability_MarkOpenRegionRecursive stops at blocked cells, terrain classes 28..30, low bands 0..6 and
   visited cells */
#define GRID_REACHABILITY_OPEN_STOP_MASK 0xf0007f01

PathingDestination
EntityPathing_ResolveDestinationAndRebuildRoutes
          (UQ12 targetWorldYQ12,UQ12 targetWorldXQ12,GameEntityRuntime *routeEntityRuntime,
          WorldRuntimeContext *worldRuntime);

bool GridReachability_RebuildConnectedRegionAroundWorldPoint
          (FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12);

void GridScratch_RebuildTerrainAndRuntimeClassificationMasks(WorldRuntimeContext *worldRuntime);

bool GridScratch_AllocateForFieldGrid(FieldGridAsset *fieldGrid,uint32_t *outError);

void GridScratch_ReleaseBuffers(void);

void GridScratch_PropagateFieldOccupancyMaskNeighborhood(FieldGridAsset *fieldGrid);

bool GridScratch_TestProjectedCellMaskBands(Q12 worldYQ12,Q12 worldXQ12,uint8_t lowBandIndex,uint8_t highBandIndex);

WorldPositionXY
EntityPathing_RebuildOverlappingGroupRoutes
          (UQ12 targetWorldY,UQ12 targetWorldX,GameEntityRuntime *routeEntityRuntime,
          WorldRuntimeContext *worldRuntime);

void GridFootprint_ClearTraversalFlagsAroundWorldPoint
          (FieldGridRadiusUnits radiusWorldUnits,Q12 worldYQ12,Q12 worldXQ12);

WorldPositionXY EntityPathing_UpdateRouteSegment
          (UQ12 targetWorldYQ12,UQ12 targetWorldXQ12,GameEntityRuntime *sourceRouteEntityRuntime,
          EntityPathingRouteEntityRuntimeView *routeEntityRuntime);

bool GridPathCost_BacktrackBestHexRoute
          (FieldGridRegionMask callerBlockingMask,FieldGridCellCoordinate startRow,
          FieldGridCellCoordinate startColumn,GridScratchCell *startCell,FieldGridCellCoordinate *outRow,
          FieldGridCellCoordinate *outColumn,FieldGridRegionMask *outRouteStateMask);

void GridPathRegion_MarkUnreachableFromCell
          (GridPathUnreachableReferenceRow32 referenceRow,
          GridPathUnreachableReferenceColumn32 referenceColumn,FieldGridCellCoordinate row,
          FieldGridCellCoordinate column,FieldGridCellCoordinate *outRow,FieldGridCellCoordinate *outColumn);

void __cdecl GridScratch_CopyPrimaryToSecondary(void);

void GridScratch_SwapPrimarySecondary(void);

void GridScratch_FloodFillConnectedCells
          (GridScratchStateMask traversalMask,uint32_t rowStrideBytes,GridScratchCell *currentCell);

void GridPathCost_PropagateWeightedHexNeighbors(GridPathPassCount remainingPasses,GridScratchCell *originCell,
          FieldGridCellCoordinate startRow,FieldGridCellCoordinate startColumn);

void GridScratch_ResetTraversalFlagsAndCosts(void);

GridPathBestUnreachableCell GridPathRegion_MarkUnreachableRecursive
          (uint32_t rowStrideBytes,GridScratchCell *currentCell,GridPathCost bestCost,
          uint32_t bestCellByteOffset);

int GridFootprint_ClearTraversalFlagsDiagonalNegative
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,uint32_t *scratchRecord);

int GridFootprint_ClearTraversalFlagsDiagonalPositive
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,uint32_t *scratchRecord);

void GridReachability_MarkOpenRegionRecursive(uint32_t rowStrideBytes,GridScratchCell *currentCell);

void GridReachability_ClearCostedRegionRecursive(uint32_t rowStrideBytes,GridScratchCell *currentCell);

bool GridPathCost_RelocateFromBlockedCell
          (FieldGridCellCoordinate cellRow,FieldGridCellCoordinate cellColumn,FieldGridCellCoordinate *outRow,
          FieldGridCellCoordinate *outColumn);

bool GridPathLine_TestHexSegmentBlocked(FieldGridRegionMask callerBlockingMask,FieldGridCellCoordinate startRow,
          FieldGridCellCoordinate startColumn,GridScratchCell *startCell,GridScratchCell *endCell);

#endif /* THANDOR_WORLD_PATHING_GRID_H */
