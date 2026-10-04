/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/pathing/path_cost.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_PATHING_PATH_COST_H
#define THANDOR_WORLD_PATHING_PATH_COST_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

Bool8 GridPathCost_BacktrackBestHexRoute
          (FieldGridRegionMask callerBlockingMask,FieldGridCellCoordinate startRow,
          FieldGridCellCoordinate startColumn,GridScratchCell *startCell,FieldGridCellCoordinate *outRow,
          FieldGridCellCoordinate *outColumn,FieldGridRegionMask *outRouteStateMask);

void GridPathCost_PropagateWeightedHexNeighbors(GridPathPassCount remainingPasses,GridScratchCell *originCell,
          FieldGridCellCoordinate startRow,FieldGridCellCoordinate startColumn);

Bool8 GridPathCost_RelocateFromBlockedCell
          (FieldGridCellCoordinate cellRow,FieldGridCellCoordinate cellColumn,FieldGridCellCoordinate *outRow,
          FieldGridCellCoordinate *outColumn);

extern GridScratchCell **g_GridPathCostQueueBegin;

extern GridScratchCell **g_GridPathCostQueueEnd;

extern uint32_t g_GridPathEntityClassMask;

extern uint32_t g_GridPathBlockingMask;

extern uint32_t g_GridPathHighCostMask;

#endif /* THANDOR_WORLD_PATHING_PATH_COST_H */
