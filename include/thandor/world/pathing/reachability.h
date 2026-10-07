/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/pathing/reachability.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_PATHING_REACHABILITY_H
#define THANDOR_WORLD_PATHING_REACHABILITY_H

#include <thandor/core/types.h>
#include <thandor/world/pathing/types.h>
#include <thandor/world/terrain/types.h>
#include <thandor/core/contracts.h>

bool GridReachability_RebuildConnectedRegionAroundWorldPoint
          (FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12);

void GridPathRegion_MarkUnreachableFromCell
          (GridPathUnreachableReferenceRow32 referenceRow,
          GridPathUnreachableReferenceColumn32 referenceColumn,FieldGridCellCoordinate row,
          FieldGridCellCoordinate column,FieldGridCellCoordinate *outRow,FieldGridCellCoordinate *outColumn);

GridPathBestUnreachableCell GridPathRegion_MarkUnreachableRecursive
          (uint32_t rowStrideBytes,GridScratchCell *currentCell,GridPathCost bestCost,
          uint32_t bestCellByteOffset);

void GridReachability_MarkOpenRegionRecursive(uint32_t rowStrideBytes,GridScratchCell *currentCell);

void GridReachability_ClearCostedRegionRecursive(uint32_t rowStrideBytes,GridScratchCell *currentCell);

bool GridPathLine_TestHexSegmentBlocked(FieldGridRegionMask callerBlockingMask,FieldGridCellCoordinate startRow,
          FieldGridCellCoordinate startColumn,GridScratchCell *startCell,GridScratchCell *endCell);

#endif /* THANDOR_WORLD_PATHING_REACHABILITY_H */
