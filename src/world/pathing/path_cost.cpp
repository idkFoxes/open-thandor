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

/* State of one GridPathCost_PropagateWeightedHexNeighbors run (the queue storage and the pass boundary are the
   g_GridPathCostQueue* globals). */
typedef struct GridPathCostQueueState {
  GridScratchCell **readCursor;
  GridScratchCell **writeCursor;
  GridPathPassCount remainingPasses;
  GridScratchCell *originCell;
  uint32_t scratchWidth;
} GridPathCostQueueState;

/* Module data. */

GridScratchCell **g_GridPathCostQueueBegin = nullptr;

GridScratchCell **g_GridPathCostQueueEnd = nullptr;

static GridScratchCell **g_GridPathCostQueuePassBoundary = nullptr;

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
Bool8 GridPathCost_BacktrackBestHexRoute
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
static Bool8 GridPathCost_OriginOrNeighborReached(GridScratchCell *originCell,uint32_t scratchWidth)
{
  GridScratchCell *rowAboveCell;

  rowAboveCell = originCell - scratchWidth;
  return (originCell->pathCost < GRID_PATH_COST_UNREACHED) || (rowAboveCell->pathCost < GRID_PATH_COST_UNREACHED) ||
         (rowAboveCell[1].pathCost < GRID_PATH_COST_UNREACHED) || (originCell[-1].pathCost < GRID_PATH_COST_UNREACHED) ||
         (originCell[1].pathCost < GRID_PATH_COST_UNREACHED) ||
         (originCell[scratchWidth - 1].pathCost < GRID_PATH_COST_UNREACHED) ||
         (originCell[scratchWidth].pathCost < GRID_PATH_COST_UNREACHED);
}

/* Next queued cell that is not already visited, or NULL when propagation ends: at an empty queue, or at the end
   of a pass once the origin (or a neighbour of it) has been reached or the passes are used up. */
static GridScratchCell *GridPathCost_DequeueUnvisitedCell(GridPathCostQueueState *queue)
{
  GridScratchCell *cell;

  do {
    while ((queue->readCursor != queue->writeCursor) && (queue->readCursor >= g_GridPathCostQueuePassBoundary)) {
      /* end of a pass */
      g_GridPathCostQueuePassBoundary = g_GridPathCostQueuePassBoundary + GRID_PATH_COST_QUEUE_PASS_ENTRIES;
      if (GridPathCost_OriginOrNeighborReached(queue->originCell,queue->scratchWidth)) {
        return nullptr;
      }
      queue->remainingPasses--;
      if (queue->remainingPasses == 0) {
        return nullptr;
      }
    }
    if (queue->readCursor == queue->writeCursor) {
      return nullptr;
    }
    cell = *queue->readCursor;
    queue->readCursor++;
  } while ((cell->stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0);
  return cell;
}

/* One neighbour step from a cell of cost currentCost: skipped when the neighbour is blocked (bit 31) or has the
   mover's faction presence bit and a blocking band; step cost 4, 3 with the faction presence bit, 12 when it
   also has a high-cost band. A cheaper cost is stored and the neighbour queued while the queue has room. */
static void GridPathCost_RelaxNeighbor(GridPathCostQueueState *queue,GridScratchCell *neighborCell,
          GridPathCost currentCost)
{
  GridScratchStateMask neighborState;
  uint32_t neighborCost;

  neighborState = neighborCell->stateMask;
  neighborCost = currentCost + GRID_PATH_STEP_COST;
  if ((-1 < (int)neighborState) &&
     (((g_GridPathEntityClassMask & neighborState) == 0) || ((g_GridPathBlockingMask & neighborState) == 0))) {
    if ((g_GridPathEntityClassMask & neighborState) != 0) {
      neighborCost = currentCost + GRID_PATH_STEP_COST_OWN_FACTION;
      if ((g_GridPathHighCostMask & neighborState) != 0) {
        neighborCost = currentCost + GRID_PATH_STEP_COST_HIGH;
      }
    }
    if ((queue->writeCursor < g_GridPathCostQueueEnd) && (neighborCost < neighborCell->pathCost)) {
      *queue->writeCursor = neighborCell;
      neighborCell->pathCost = neighborCost;
      queue->writeCursor++;
    }
  }
}

/* Fills GridScratchCell.pathCost outwards from the cell (startRow, startColumn), which gets cost 0, over the six
   hex neighbours with a FIFO queue (g_GridPathCostQueueBegin..End), lowering a neighbour's cost whenever a
   cheaper step is found. The queue is processed in passes of GRID_PATH_COST_QUEUE_PASS_ENTRIES entries; after a
   pass it stops once originCell (the mover's cell) or one of its neighbours has a cost, or after remainingPasses
   passes, or when the queue runs empty.
*/
void GridPathCost_PropagateWeightedHexNeighbors(GridPathPassCount remainingPasses,GridScratchCell *originCell,
          FieldGridCellCoordinate startRow,FieldGridCellCoordinate startColumn)

{
  GridPathCostQueueState queue;
  GridScratchCell *startCell;
  GridScratchCell *currentCell;
  GridPathCost currentCost;
  uint32_t scratchWidth;

  scratchWidth = g_GridScratchWidth;
  queue.scratchWidth = scratchWidth;
  queue.remainingPasses = remainingPasses;
  queue.originCell = originCell;
  queue.readCursor = g_GridPathCostQueueBegin;
  queue.writeCursor = g_GridPathCostQueueBegin + 1;
  startCell = g_GridScratchPrimary + (int32_t)(startRow * g_GridScratchWidth) + startColumn;
  *g_GridPathCostQueueBegin = startCell;
  g_GridPathCostQueuePassBoundary = queue.readCursor;
  startCell->pathCost = 0;
  g_GridPathCostQueuePassBoundary = g_GridPathCostQueuePassBoundary + GRID_PATH_COST_QUEUE_PASS_ENTRIES;
  while ((currentCell = GridPathCost_DequeueUnvisitedCell(&queue)) != nullptr) {
    /* the six neighbours: two in the row above, right, left, two in the row below */
    currentCost = currentCell->pathCost;
    GridPathCost_RelaxNeighbor(&queue,currentCell - scratchWidth,currentCost);
    GridPathCost_RelaxNeighbor(&queue,currentCell - scratchWidth + 1,currentCost);
    GridPathCost_RelaxNeighbor(&queue,currentCell + 1,currentCost);
    GridPathCost_RelaxNeighbor(&queue,currentCell - 1,currentCost);
    GridPathCost_RelaxNeighbor(&queue,currentCell - 1 + scratchWidth,currentCost);
    GridPathCost_RelaxNeighbor(&queue,currentCell + scratchWidth,currentCost);
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
Bool8 GridPathCost_RelocateFromBlockedCell
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
