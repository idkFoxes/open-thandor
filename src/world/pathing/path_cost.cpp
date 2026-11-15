/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/pathing/path_cost.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* Hex path costs on the scratch grid: the weighted neighbour propagation (a FIFO label-correcting queue over
   the hex lattice, not Dijkstra: a cell is queued again whenever a cheaper cost reaches it),
   the best-route backtrack and the relocation out of a blocked cell. */

#include <thandor/world/pathing/path_cost.h>
#include <thandor/thandor.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

GridScratchCell **g_GridPathCostQueueBegin = nullptr;

GridScratchCell **g_GridPathCostQueueEnd = nullptr;

uint32_t g_GridPathEntityClassMask = 0;

uint32_t g_GridPathBlockingMask = 0;

uint32_t g_GridPathHighCostMask = 0;

/* Returns the cheapest of the six hex neighbours of cell (two in the row above, left/right, two in the row
   below) whose path cost is below the cell's own, the first one on ties; NULL at a local minimum. */
static GridScratchCell *GridPathCost_FindCheaperHexNeighbor(GridScratchCell *cell,uint32_t scratchWidth)
{
  GridScratchCell *rowAboveCell;
  GridScratchCell *bestNeighborCell;
  uint32_t bestNeighborCost;

  rowAboveCell = cell - scratchWidth;
  bestNeighborCost = cell->pathCost;
  bestNeighborCell = nullptr;
  if (rowAboveCell->pathCost < bestNeighborCost) {
    bestNeighborCost = rowAboveCell->pathCost;
    bestNeighborCell = rowAboveCell;
  }
  if (rowAboveCell[1].pathCost < bestNeighborCost) {
    bestNeighborCost = rowAboveCell[1].pathCost;
    bestNeighborCell = rowAboveCell + 1;
  }
  if (rowAboveCell[scratchWidth - 1].pathCost < bestNeighborCost) {
    bestNeighborCost = rowAboveCell[scratchWidth - 1].pathCost;
    bestNeighborCell = rowAboveCell + (scratchWidth - 1);
  }
  if (rowAboveCell[scratchWidth + 1].pathCost < bestNeighborCost) {
    bestNeighborCost = rowAboveCell[scratchWidth + 1].pathCost;
    bestNeighborCell = rowAboveCell + scratchWidth + 1;
  }
  if (rowAboveCell[scratchWidth * 2 - 1].pathCost < bestNeighborCost) {
    bestNeighborCost = rowAboveCell[scratchWidth * 2 - 1].pathCost;
    bestNeighborCell = rowAboveCell + scratchWidth * 2 - 1;
  }
  if (rowAboveCell[scratchWidth * 2].pathCost < bestNeighborCost) {
    bestNeighborCell = rowAboveCell + scratchWidth * 2;
  }
  return bestNeighborCell;
}

/* Follows the propagated path costs downhill from startCell (the mover's cell, at startRow/startColumn) to the
   cheapest of the six neighbours, as long as that neighbour can still be seen from startCell in a straight line
   (GridPathLine_TestHexSegmentBlocked with callerBlockingMask, dropped once a high-cost cell is entered); at least
   one step is taken. Returns true when that cell is the cost origin itself (cost 0, the target was reached);
   otherwise returns false and writes the row/column of the farthest such cell to *outRow/*outColumn.
   *outRouteStateMask always receives the final blocking mask (callerBlockingMask, or 0 once a high-cost cell
   was entered).
*/
bool GridPathCost_BacktrackBestHexRoute
          (FieldGridRegionMask callerBlockingMask,FieldGridCellCoordinate startRow,
          FieldGridCellCoordinate startColumn,GridScratchCell *startCell,FieldGridCellCoordinate *outRow,
          FieldGridCellCoordinate *outColumn,FieldGridRegionMask *outRouteStateMask)

{
  uint32_t scratchWidth;
  GridScratchCell *currentCell;
  GridScratchCell *nextCell;
  uint32_t selectedCellIndex;

  scratchWidth = g_GridScratchWidth;
  /* step to the cheapest neighbour until a local minimum is reached or the straight line from startCell to
     that neighbour is blocked */
  nextCell = startCell;
  do {
    currentCell = nextCell;
    if ((currentCell->stateMask & g_GridPathHighCostMask) != 0) {
      callerBlockingMask = 0;
    }
    nextCell = GridPathCost_FindCheaperHexNeighbor(currentCell,scratchWidth);
  } while ((nextCell != nullptr) &&
           !GridPathLine_TestHexSegmentBlocked(callerBlockingMask,startRow,startColumn,startCell,nextCell));
  if ((nextCell != nullptr) && (currentCell == startCell)) {
    /* blocked on the first step: take that step anyway */
    currentCell = nextCell;
  }
  *outRouteStateMask = callerBlockingMask;
  if (currentCell->pathCost != 0) {
    selectedCellIndex = (uint32_t)(reinterpret_cast<uintptr_t>(currentCell) - reinterpret_cast<uintptr_t>(g_GridScratchPrimary)) >> 3;
    *outRow = selectedCellIndex / g_GridScratchWidth;
    *outColumn = selectedCellIndex % g_GridScratchWidth;
    return false;
  }
  return true;
}

/* True when originCell or one of its six hex neighbours already has a cost. */
static bool GridPathCost_OriginOrNeighborReached(GridScratchCell *originCell,uint32_t scratchWidth)
{
  GridScratchCell *rowAboveCell;

  rowAboveCell = originCell - scratchWidth;
  return (originCell->pathCost < GRID_PATH_COST_UNREACHED) || (rowAboveCell->pathCost < GRID_PATH_COST_UNREACHED) ||
         (rowAboveCell[1].pathCost < GRID_PATH_COST_UNREACHED) || (originCell[-1].pathCost < GRID_PATH_COST_UNREACHED) ||
         (originCell[1].pathCost < GRID_PATH_COST_UNREACHED) ||
         (originCell[scratchWidth - 1].pathCost < GRID_PATH_COST_UNREACHED) ||
         (originCell[scratchWidth].pathCost < GRID_PATH_COST_UNREACHED);
}

/* One neighbour step from a cell of cost currentCost: skipped when the neighbour is blocked (bit 31) or has the
   mover's faction presence bit and a blocking band; step cost 4, 3 with the faction presence bit, 12 when it
   also has a high-cost band. A cheaper cost is stored and the neighbour queued while the queue has room. The
   masks and the queue cursor are the caller's locals (g_GridPath*Mask, see GridPathCost_PropagateWeightedHexNeighbors),
   so the hot loop keeps them in registers. */
static inline void GridPathCost_RelaxNeighbor(GridScratchCell *neighborCell,GridPathCost currentCost,
          uint32_t entityClassMask,uint32_t blockingMask,uint32_t highCostMask,GridScratchCell **&writeCursor,
          GridScratchCell **queueEnd)
{
  uint32_t neighborState;
  uint32_t neighborCost;

  neighborState = (uint32_t)neighborCell->stateMask;
  if ((int)neighborState < 0) {
    return;
  }
  neighborCost = currentCost + GRID_PATH_STEP_COST;
  if ((entityClassMask & neighborState) != 0) {
    if ((blockingMask & neighborState) != 0) {
      return;
    }
    neighborCost = currentCost + GRID_PATH_STEP_COST_OWN_FACTION;
    if ((highCostMask & neighborState) != 0) {
      neighborCost = currentCost + GRID_PATH_STEP_COST_HIGH;
    }
  }
  if ((writeCursor < queueEnd) && (neighborCost < neighborCell->pathCost)) {
    *writeCursor = neighborCell;
    neighborCell->pathCost = neighborCost;
    writeCursor++;
  }
}

/* Fills GridScratchCell.pathCost outwards from the cell (startRow, startColumn), which gets cost 0, over the six
   hex neighbours with a FIFO queue (g_GridPathCostQueueBegin..End), lowering a neighbour's cost whenever a
   cheaper step is found. The queue is processed in passes of GRID_PATH_COST_QUEUE_PASS_ENTRIES entries; after a
   pass it stops once originCell (the mover's cell) or one of its neighbours has a cost, or after remainingPasses
   passes, or when the queue runs empty. Queued cells that are already visited (GRID_SCRATCH_TRAVERSAL_VISITED)
   are skipped.
   open-thandor: the original's queue state lived in memory (a state record and a global pass boundary); here it
   is in locals, with the same reads, writes and order, so every plan gives the same costs, faster (route plans
   are the largest single cost of a simulation step). */
void GridPathCost_PropagateWeightedHexNeighbors(GridPathPassCount remainingPasses,GridScratchCell *originCell,
          FieldGridCellCoordinate startRow,FieldGridCellCoordinate startColumn)

{
  DebugHook_ProfileScope(DEBUG_PROFILE_PROPAGATE);
  const uint32_t scratchWidth = g_GridScratchWidth;
  const uint32_t entityClassMask = g_GridPathEntityClassMask;
  const uint32_t blockingMask = g_GridPathBlockingMask;
  const uint32_t highCostMask = g_GridPathHighCostMask;
  GridScratchCell **const queueEnd = g_GridPathCostQueueEnd;
  GridScratchCell **readCursor;
  GridScratchCell **writeCursor;
  GridScratchCell **passBoundary;
  GridScratchCell *startCell;
  GridScratchCell *currentCell;
  GridPathCost currentCost;

  readCursor = g_GridPathCostQueueBegin;
  writeCursor = g_GridPathCostQueueBegin + 1;
  startCell = g_GridScratchPrimary + (int32_t)(startRow * g_GridScratchWidth) + startColumn;
  *g_GridPathCostQueueBegin = startCell;
  startCell->pathCost = 0;
  passBoundary = readCursor + GRID_PATH_COST_QUEUE_PASS_ENTRIES;
  for (;;) {
    /* the next queued cell that is not visited; at the end of a pass stop once the origin (or a neighbour of it)
       has a cost or the passes are used up */
    do {
      while ((readCursor != writeCursor) && (readCursor >= passBoundary)) {
        passBoundary = passBoundary + GRID_PATH_COST_QUEUE_PASS_ENTRIES;
        if (GridPathCost_OriginOrNeighborReached(originCell,scratchWidth)) {
          return;
        }
        remainingPasses--;
        if (remainingPasses == 0) {
          return;
        }
      }
      if (readCursor == writeCursor) {
        return;
      }
      currentCell = *readCursor;
      readCursor++;
    } while (((uint32_t)currentCell->stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0);
    /* the six neighbours: two in the row above, right, left, two in the row below */
    currentCost = currentCell->pathCost;
    GridPathCost_RelaxNeighbor(currentCell - scratchWidth,currentCost,entityClassMask,blockingMask,highCostMask,
                               writeCursor,queueEnd);
    GridPathCost_RelaxNeighbor(currentCell - scratchWidth + 1,currentCost,entityClassMask,blockingMask,highCostMask,
                               writeCursor,queueEnd);
    GridPathCost_RelaxNeighbor(currentCell + 1,currentCost,entityClassMask,blockingMask,highCostMask,writeCursor,
                               queueEnd);
    GridPathCost_RelaxNeighbor(currentCell - 1,currentCost,entityClassMask,blockingMask,highCostMask,writeCursor,
                               queueEnd);
    GridPathCost_RelaxNeighbor(currentCell - 1 + scratchWidth,currentCost,entityClassMask,blockingMask,highCostMask,
                               writeCursor,queueEnd);
    GridPathCost_RelaxNeighbor(currentCell + scratchWidth,currentCost,entityClassMask,blockingMask,highCostMask,
                               writeCursor,queueEnd);
  }
}

/* Hex distance on the skewed scratch grid: |dc| + |dr| when both deltas have the same sign, else the larger
   of the two. */
static int GridPathCost_HexDistance(int columnDelta,int rowDelta)

{
  int hexDistance;

  hexDistance = columnDelta;
  if (hexDistance < 0) {
    hexDistance = -hexDistance;
    if (rowDelta < 0) {
      hexDistance = hexDistance - rowDelta;
    }
    else if (hexDistance < rowDelta) {
      hexDistance = rowDelta;
    }
  }
  else if (rowDelta < 0) {
    if (hexDistance < -rowDelta) {
      hexDistance = -rowDelta;
    }
  }
  else {
    hexDistance = hexDistance + rowDelta;
  }
  return hexDistance;
}

/* Checks whether a mover can leave the scratch cell (cellRow, cellColumn): when the cell or one of its six hex
   neighbours is free of g_GridPathBlockingMask and GRID_SCRATCH_BLOCKED, returns false and leaves *outRow and
   *outColumn untouched. Otherwise returns true and writes the nearest (hex distance) free cell within +-16
   rows/columns, or the cell itself when there is none.
*/
bool GridPathCost_RelocateFromBlockedCell
          (FieldGridCellCoordinate cellRow,FieldGridCellCoordinate cellColumn,FieldGridCellCoordinate *outRow,
          FieldGridCellCoordinate *outColumn)

{
  int cellIndex;
  int minColumn;
  int scanColumn;
  int hexDistance;
  uint32_t maxColumn;
  int scanWidth;
  GridScratchStateMask blockedMask;
  uint32_t maxRow;
  int bestHexDistance;
  int searchRow;
  int rowsRemaining;
  GridScratchCell *scanCell;
  GridScratchCell *rowStartCell;
  GridScratchCell *rowAboveCell;
  int bestRow;
  int bestColumn;

  cellIndex = cellRow * g_GridScratchWidth + cellColumn;
  blockedMask = g_GridPathBlockingMask | GRID_SCRATCH_BLOCKED;
  /* the cell itself, then its six hex neighbours: above, above-right, left, right, below-left, below */
  rowAboveCell = g_GridScratchPrimary + cellIndex - g_GridScratchWidth;
  if ((g_GridScratchPrimary[cellIndex].stateMask & blockedMask) == 0 ||
      (rowAboveCell->stateMask & blockedMask) == 0 ||
      (rowAboveCell[1].stateMask & blockedMask) == 0 ||
      (rowAboveCell[g_GridScratchWidth - 1].stateMask & blockedMask) == 0 ||
      (rowAboveCell[g_GridScratchWidth + 1].stateMask & blockedMask) == 0 ||
      (rowAboveCell[g_GridScratchWidth * 2 - 1].stateMask & blockedMask) == 0 ||
      (rowAboveCell[g_GridScratchWidth * 2].stateMask & blockedMask) == 0) {
    /* open cell: both callers (EntityPathing_ResolveDestinationAndRebuildRoutes,
       EntityPathing_UpdateRouteSegment) read the cell only after a relocation */
    return false;
  }
  /* search window: +-GRID_PATH_NEAREST_SEARCH_RADIUS, clipped to the grid */
  minColumn = cellColumn - GRID_PATH_NEAREST_SEARCH_RADIUS;
  if (minColumn < 0) {
    minColumn = 0;
  }
  searchRow = cellRow - GRID_PATH_NEAREST_SEARCH_RADIUS;
  if (searchRow < 0) {
    searchRow = 0;
  }
  maxColumn = cellColumn + (uint32_t)GRID_PATH_NEAREST_SEARCH_RADIUS;
  if ((int)g_GridScratchWidth < (int)(cellColumn + (uint32_t)GRID_PATH_NEAREST_SEARCH_RADIUS)) {
    maxColumn = g_GridScratchWidth;
  }
  maxRow = cellRow + (uint32_t)GRID_PATH_NEAREST_SEARCH_RADIUS;
  if ((int)g_GridScratchHeight < (int)(cellRow + (uint32_t)GRID_PATH_NEAREST_SEARCH_RADIUS)) {
    maxRow = g_GridScratchHeight;
  }
  scanWidth = maxColumn - minColumn;
  rowsRemaining = maxRow - searchRow;
  if (scanWidth == 0 || minColumn > (int)maxColumn || rowsRemaining == 0 || searchRow > (int)maxRow) {
    *outRow = cellRow;
    *outColumn = cellColumn;
    return true;
  }
  bestHexDistance = INT32_MAX;
  rowStartCell = g_GridScratchPrimary + (int32_t)(searchRow * g_GridScratchWidth) + minColumn;
  for (; rowsRemaining != 0; rowsRemaining--) {
    scanCell = rowStartCell;
    for (scanColumn = minColumn; scanColumn != minColumn + scanWidth; scanColumn++) {
      if ((scanCell->stateMask & blockedMask) == 0) {
        hexDistance = GridPathCost_HexDistance(scanColumn - cellColumn,searchRow - cellRow);
        if (hexDistance < bestHexDistance) {
          bestHexDistance = hexDistance;
          bestRow = searchRow;
          bestColumn = scanColumn;
        }
      }
      scanCell++;
    }
    rowStartCell = rowStartCell + g_GridScratchWidth;
    searchRow++;
  }
  if (bestHexDistance < INT32_MAX) {
    *outRow = bestRow;
    *outColumn = bestColumn;
    return true;
  }
  *outRow = cellRow;
  *outColumn = cellColumn;
  return true;
}
