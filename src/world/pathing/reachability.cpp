/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/pathing/reachability.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* Reachability on the scratch grid: connected regions around a world point, unreachable-region marking and
   the hex line-of-passage test of a segment. */

#include <thandor/world/pathing/reachability.h>
#include <thandor/thandor.h>

/* Module data. */

static uint32_t g_GridPathUnreachableRegionReferenceColumn = 0;

static uint32_t g_GridPathUnreachableRegionReferenceRow = 0;

/* True when the cell below rowAboveCell (rowAboveCell[scratchWidth]) is open, counted (inside the outer
   footprint) and marked visited, and at least one of its six hex neighbours has count 0 (outside the footprint). */
static bool GridReachability_IsMarkedRingEdgeCell(GridScratchCell *rowAboveCell,uint32_t scratchWidth)
{
  GridScratchCell *cell;

  cell = rowAboveCell + scratchWidth;
  if (((cell->stateMask & GRID_SCRATCH_BLOCKED) != 0) || (cell->pathCost == 0) ||
      ((cell->stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) == 0)) {
    return false;
  }
  return (rowAboveCell->pathCost == 0) || (rowAboveCell[1].pathCost == 0) ||
         (rowAboveCell[scratchWidth - 1].pathCost == 0) || (rowAboveCell[scratchWidth + 1].pathCost == 0) ||
         (rowAboveCell[scratchWidth * 2 - 1].pathCost == 0) || (rowAboveCell[scratchWidth * 2].pathCost == 0);
}

/* Tests whether an obstacle of radiusMetric at the world point would split the open area around it. Every
   scratch cell is marked visited with count 0; the footprint of 3 * radius is unmarked (its cells get a non-zero
   count), the open region around the point is flood-marked inside it, and the inner footprint of radius is
   unmarked again. The first marked cell on the outer footprint's edge has its piece of the ring cleared; any
   other marked edge cell left over means the ring fell apart and returns true. A point outside the grid
   also returns true.
*/
bool GridReachability_RebuildConnectedRegionAroundWorldPoint
          (FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12)

{
  uint32_t scratchWidth;
  uint32_t cellsToClear;
  uint32_t scaledRowTerm;
  int cellColumn;
  int cellRow;
  int cellsToScan;
  GridScratchCell *scratchCursor;
  uint32_t rowStrideBytes;
  bool moreBlocksRemain;

  cellsToClear = g_GridScratchWidth * g_GridScratchHeight;
  scratchCursor = g_GridScratchPrimary;
  do {
    scratchCursor->stateMask = scratchCursor->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor->pathCost = 0;
    scratchCursor[1].stateMask = scratchCursor[1].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[1].pathCost = 0;
    scratchCursor[2].stateMask = scratchCursor[2].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[2].pathCost = 0;
    scratchCursor[3].stateMask = scratchCursor[3].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[3].pathCost = 0;
    scratchCursor[4].stateMask = scratchCursor[4].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[4].pathCost = 0;
    scratchCursor[5].stateMask = scratchCursor[5].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[5].pathCost = 0;
    scratchCursor[6].stateMask = scratchCursor[6].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[6].pathCost = 0;
    scratchCursor[7].stateMask = scratchCursor[7].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[7].pathCost = 0;
    scratchCursor[8].stateMask = scratchCursor[8].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[8].pathCost = 0;
    scratchCursor[9].stateMask = scratchCursor[9].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[9].pathCost = 0;
    scratchCursor[10].stateMask = scratchCursor[10].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[10].pathCost = 0;
    scratchCursor[11].stateMask = scratchCursor[11].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[11].pathCost = 0;
    scratchCursor[12].stateMask = scratchCursor[12].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[12].pathCost = 0;
    scratchCursor[13].stateMask = scratchCursor[13].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[13].pathCost = 0;
    scratchCursor[14].stateMask = scratchCursor[14].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[14].pathCost = 0;
    scratchCursor[15].stateMask = scratchCursor[15].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[15].pathCost = 0;
    scratchCursor = scratchCursor + 16;
    moreBlocksRemain = 15 < cellsToClear;
    cellsToClear = cellsToClear - 16;
  } while (moreBlocksRemain && cellsToClear != 0);
  GridFootprint_ClearTraversalFlagsAroundWorldPoint(radiusMetric * 3,worldYQ12,worldXQ12);
  scratchWidth = g_GridScratchWidth;
  scaledRowTerm = (int)((uint64_t)((int64_t)worldYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 32) << 11 |
          (uint32_t)((int64_t)worldYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 21;
  cellColumn = (int)((((int)((uint64_t)((int64_t)worldXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 32) << 12 |
                 (uint32_t)((int64_t)worldXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 20) - scaledRowTerm) +
                 GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT;
  cellRow = (int)(scaledRowTerm * 2 + GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT;
  if ((cellColumn < 0) || (cellRow < 0) || ((int)g_GridScratchWidth <= cellColumn) ||
      ((int)g_GridScratchHeight <= cellRow)) {
    return true;
  }
  rowStrideBytes = g_GridScratchWidth * 8;
  GridReachability_MarkOpenRegionRecursive
            (rowStrideBytes,g_GridScratchPrimary + (int32_t)(cellRow * g_GridScratchWidth) + cellColumn);
  GridFootprint_ClearTraversalFlagsAroundWorldPoint(radiusMetric,worldYQ12,worldXQ12);
  /* find the first marked cell inside the outer footprint that has a neighbour outside it (count 0); the
     tested cell is scratchCursor[scratchWidth], the cursor sits on the row above it */
  scratchCursor = g_GridScratchPrimary + scratchWidth * 3;
  cellsToScan = (g_GridScratchHeight - 8) * g_GridScratchWidth;
  while (!GridReachability_IsMarkedRingEdgeCell(scratchCursor,scratchWidth)) {
    scratchCursor = scratchCursor + 1;
    cellsToScan--;
    if (cellsToScan == 0) {
      return false;
    }
  }
  GridReachability_ClearCostedRegionRecursive(rowStrideBytes,scratchCursor + scratchWidth);
  /* any other marked edge cell belongs to a separate piece of the ring */
  scratchCursor = g_GridScratchPrimary + scratchWidth * 3;
  cellsToScan = (g_GridScratchHeight - 8) * g_GridScratchWidth;
  /* Original quirk: a do/while, so a count of 0 runs it 2^32 times (kept as in the original; step 11). */
  do {
    if (GridReachability_IsMarkedRingEdgeCell(scratchCursor,scratchWidth)) {
      return true;
    }
    scratchCursor = scratchCursor + 1;
    cellsToScan--;
  } while (cellsToScan != 0);
  return false;
}

/* Used when the start cell (row, column) was not reached by the cost propagation from the reference (target)
   cell: flood-marks the unreached region around the start and returns the cell of that region with the smallest
   hex distance to the reference cell (written to *outRow/*outColumn), the closest the mover can get to the
   target.
*/
void GridPathRegion_MarkUnreachableFromCell
          (GridPathUnreachableReferenceRow32 referenceRow,
          GridPathUnreachableReferenceColumn32 referenceColumn,FieldGridCellCoordinate row,
          FieldGridCellCoordinate column,FieldGridCellCoordinate *outRow,FieldGridCellCoordinate *outColumn)

{
  uint32_t markedCellIndex;
  int rowBaseIndex;
  GridPathBestUnreachableCell recursionResult;

  g_GridPathUnreachableRegionReferenceColumn = referenceColumn;
  g_GridPathUnreachableRegionReferenceRow = referenceRow;
  rowBaseIndex = row * g_GridScratchWidth;
  /* best distance starts at INT_MAX, best cell at the start cell (as a byte offset) */
  recursionResult = GridPathRegion_MarkUnreachableRecursive
                    (g_GridScratchWidth << 3,g_GridScratchPrimary + rowBaseIndex + column,INT32_MAX
                     ,(rowBaseIndex + column) * 8);
  markedCellIndex = recursionResult.bestCellByteOffset >> 3;
  *outRow = (FieldGridCellCoordinate)(markedCellIndex / g_GridScratchWidth);
  *outColumn = (FieldGridCellCoordinate)(markedCellIndex % g_GridScratchWidth);
}

/* Scans from currentCell in direction (-1 left, +1 right) over unreached open cells, marking them visited, and
   returns the span boundary cell: a finite-cost or blocked (bit 31) cell, or a cell with the mover's faction
   presence bit and a blocking band. Original quirk: such a faction-blocked boundary cell is marked and unmarked
   again, so it ends with its visited bit cleared even if it was set before. */
static GridScratchCell *GridPathRegion_ScanUnreachedSpanEnd(GridScratchCell *currentCell,int direction)

{
  GridScratchCell *cell;
  GridScratchStateMask cellState;

  cell = currentCell + direction;
  cellState = cell->stateMask;
  while (!(cell->pathCost < GRID_PATH_COST_UNREACHED) && (int)cellState >= 0) {
    if (((g_GridPathEntityClassMask & cellState) != 0) && ((g_GridPathBlockingMask & cellState) != 0)) {
      cell->stateMask = cellState & ~GRID_SCRATCH_TRAVERSAL_VISITED;
      break;
    }
    cell->stateMask = cellState | GRID_SCRATCH_TRAVERSAL_VISITED;
    cell = cell + direction;
    cellState = cell->stateMask;
  }
  return cell;
}

/* True for a cell the region walk recurses into: unreached, neither blocked (bit 31) nor visited, and not
   a cell with the mover's faction presence bit and a blocking band. */
static bool GridPathRegion_IsUnvisitedUnreachedOpenCell(GridScratchCell *cell)

{
  GridScratchStateMask cellState;

  cellState = cell->stateMask;
  return GRID_PATH_COST_MAX_REACHED < cell->pathCost &&
         (cellState & (GRID_SCRATCH_BLOCKED | GRID_SCRATCH_TRAVERSAL_VISITED)) == 0 &&
         ((g_GridPathEntityClassMask & cellState) == 0 || (g_GridPathBlockingMask & cellState) == 0);
}

/* Scanline flood fill for GridPathRegion_MarkUnreachableFromCell: marks the horizontal run of unreached cells
   around currentCell as visited (stopping at reached, blocked or faction-blocked cells), keeps the run cell
   nearest (hex distance) to g_GridPathUnreachableRegionReference{Row,Column} as the best cell if it beats
   bestCost, and recurses into the unvisited open cells of the rows above and below. Returns the best distance
   and the best cell's byte offset in the scratch grid.
*/
GridPathBestUnreachableCell GridPathRegion_MarkUnreachableRecursive
          (uint32_t rowStrideBytes,GridScratchCell *currentCell,GridPathCost bestCost,
          uint32_t bestCellByteOffset)

{
  uint32_t spanByteOffset;
  GridPathCost referenceDistance;
  GridScratchCell *rightEndCell;
  int spanLength;
  uint32_t spanColumn;
  int columnDelta;
  GridPathCost columnDistance;
  GridScratchCell *leftEndCell;
  GridScratchCell *rowCursor;
  GridPathBestUnreachableCell bestResult;
  GridPathCost updatedBestCost;

  currentCell->stateMask = currentCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
  leftEndCell = GridPathRegion_ScanUnreachedSpanEnd(currentCell,-1);
  rightEndCell = GridPathRegion_ScanUnreachedSpanEnd(currentCell,1);
  /* hex distance from the span to the reference cell; spanByteOffset moves to the span cell nearest to it */
  spanByteOffset = (uint32_t)(reinterpret_cast<uint8_t *>(leftEndCell + 1) - reinterpret_cast<uint8_t *>(g_GridScratchPrimary));
  spanLength = (int)(rightEndCell - leftEndCell) - 2;
  spanColumn = (spanByteOffset >> 3) % g_GridScratchWidth;
  referenceDistance = (spanByteOffset >> 3) / g_GridScratchWidth - g_GridPathUnreachableRegionReferenceRow;
  if ((int)referenceDistance < 0) {
    referenceDistance = -referenceDistance;
    columnDistance = spanColumn - g_GridPathUnreachableRegionReferenceColumn;
    if ((int)columnDistance < 0) {
      columnDelta = columnDistance + spanLength;
      if (columnDelta < 0) {
        referenceDistance = referenceDistance - columnDelta;
        spanByteOffset = spanByteOffset + spanLength * 8;
      }
      else {
        spanByteOffset = spanByteOffset + (columnDelta - spanLength) * -8;
      }
    }
    else if ((int)referenceDistance < (int)columnDistance) {
      referenceDistance = columnDistance;
    }
  }
  else {
    columnDelta = spanColumn - g_GridPathUnreachableRegionReferenceColumn;
    if (columnDelta < 0) {
      columnDelta = columnDelta + spanLength;
      if (columnDelta < 0) {
        spanByteOffset = spanByteOffset + spanLength * 8;
        if ((int)referenceDistance < -columnDelta) {
          referenceDistance = -columnDelta;
        }
      }
      else {
        spanByteOffset = spanByteOffset + (columnDelta - spanLength) * -8;
      }
    }
    else {
      referenceDistance = referenceDistance + columnDelta;
    }
  }
  updatedBestCost = bestCost;
  if ((int)referenceDistance < (int)bestCost) {
    updatedBestCost = referenceDistance;
    bestCellByteOffset = spanByteOffset;
  }
  bestResult.bestCost = updatedBestCost;
  bestResult.bestCellByteOffset = bestCellByteOffset;
  /* recurse into the unreached, unvisited, open cells of the rows above and below the span */
  rowCursor = GridScratchCell_RowAbove(leftEndCell + 1,rowStrideBytes);
  do {
    if (GridPathRegion_IsUnvisitedUnreachedOpenCell(rowCursor)) {
      bestResult = GridPathRegion_MarkUnreachableRecursive
                         (rowStrideBytes,rowCursor,bestResult.bestCost,bestResult.bestCellByteOffset);
    }
    rowCursor++;
  } while (rowCursor <= GridScratchCell_RowAbove(rightEndCell,rowStrideBytes));
  rowCursor = GridScratchCell_RowBelow(leftEndCell,rowStrideBytes);
  do {
    if (GridPathRegion_IsUnvisitedUnreachedOpenCell(rowCursor)) {
      bestResult = GridPathRegion_MarkUnreachableRecursive
                         (rowStrideBytes,rowCursor,bestResult.bestCost,bestResult.bestCellByteOffset);
    }
    rowCursor++;
  } while (rowCursor < GridScratchCell_RowBelow(rightEndCell,rowStrideBytes));
  return bestResult;
}

/* Scanline flood fill for GridReachability_RebuildConnectedRegionAroundWorldPoint: marks the horizontal run of
   open cells around currentCell as visited, then recurses into the open cells of the hex-adjacent spans in the
   rows above and below. A cell is open when none of GRID_REACHABILITY_OPEN_STOP_MASK is set (blocked, terrain
   classes 28..30, low bands 0..6, already visited).
*/
void GridReachability_MarkOpenRegionRecursive(uint32_t rowStrideBytes,GridScratchCell *currentCell)

{
  GridScratchCell *leftStopCell;
  GridScratchCell *rightStopCell;
  GridScratchCell *prevRowCursor;
  GridScratchCell *prevRowEnd;
  GridScratchCell *nextRowCursor;
  GridScratchCell *nextRowEnd;

  currentCell->stateMask = currentCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
  for (leftStopCell = currentCell - 1; (leftStopCell->stateMask & GRID_REACHABILITY_OPEN_STOP_MASK) == 0;
       leftStopCell--) {
    leftStopCell->stateMask = leftStopCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
  }
  for (rightStopCell = currentCell + 1; (rightStopCell->stateMask & GRID_REACHABILITY_OPEN_STOP_MASK) == 0;
       rightStopCell++) {
    rightStopCell->stateMask = rightStopCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
  }
  /* row above: from the span's first cell up to the column of the right stop cell (inclusive); row below: from
     the column of the left stop cell up to the span's last cell. Neither range is ever empty, so testing before
     the first cell matches the original's do-while. */
  prevRowEnd = GridScratchCell_RowAbove(rightStopCell,rowStrideBytes);
  for (prevRowCursor = GridScratchCell_RowAbove(leftStopCell + 1,rowStrideBytes);
       prevRowCursor <= prevRowEnd; prevRowCursor++) {
    if ((prevRowCursor->stateMask & GRID_REACHABILITY_OPEN_STOP_MASK) == 0) {
      GridReachability_MarkOpenRegionRecursive(rowStrideBytes,prevRowCursor);
    }
  }
  nextRowEnd = GridScratchCell_RowBelow(rightStopCell,rowStrideBytes);
  for (nextRowCursor = GridScratchCell_RowBelow(leftStopCell,rowStrideBytes);
       nextRowCursor < nextRowEnd; nextRowCursor++) {
    if ((nextRowCursor->stateMask & GRID_REACHABILITY_OPEN_STOP_MASK) == 0) {
      GridReachability_MarkOpenRegionRecursive(rowStrideBytes,nextRowCursor);
    }
  }
}

/* Scanline flood fill that undoes GridReachability_MarkOpenRegionRecursive for one connected piece: clears the
   visited bit across the connected cells that are visited and inside the footprint (pathCost counter non-zero),
   with the same hex-adjacent recursion into the rows above and below.
*/
void GridReachability_ClearCostedRegionRecursive(uint32_t rowStrideBytes,GridScratchCell *currentCell)

{
  GridScratchCell *leftStopCell;
  GridScratchCell *rightStopCell;
  GridScratchCell *prevRowCursor;
  GridScratchCell *prevRowEnd;
  GridScratchCell *nextRowCursor;
  GridScratchCell *nextRowEnd;

  currentCell->stateMask = currentCell->stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
  for (leftStopCell = currentCell - 1;
       (leftStopCell->stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0 && leftStopCell->pathCost != 0;
       leftStopCell--) {
    leftStopCell->stateMask = leftStopCell->stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
  }
  for (rightStopCell = currentCell + 1;
       (rightStopCell->stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0 && rightStopCell->pathCost != 0;
       rightStopCell++) {
    rightStopCell->stateMask = rightStopCell->stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
  }
  /* same row ranges as GridReachability_MarkOpenRegionRecursive; neither is ever empty, so testing before the
     first cell matches the original's do-while */
  prevRowEnd = GridScratchCell_RowAbove(rightStopCell,rowStrideBytes);
  for (prevRowCursor = GridScratchCell_RowAbove(leftStopCell + 1,rowStrideBytes);
       prevRowCursor <= prevRowEnd; prevRowCursor++) {
    if ((prevRowCursor->stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0 && prevRowCursor->pathCost != 0) {
      GridReachability_ClearCostedRegionRecursive(rowStrideBytes,prevRowCursor);
    }
  }
  nextRowEnd = GridScratchCell_RowBelow(rightStopCell,rowStrideBytes);
  for (nextRowCursor = GridScratchCell_RowBelow(leftStopCell,rowStrideBytes);
       nextRowCursor < nextRowEnd; nextRowCursor++) {
    if ((nextRowCursor->stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0 && nextRowCursor->pathCost != 0) {
      GridReachability_ClearCostedRegionRecursive(rowStrideBytes,nextRowCursor);
    }
  }
}

/* True when a cell blocks a line segment: blocked (bit 31), lacking the mover's faction presence bit, or having
   a g_GridPathBlockingMask or callerBlockingMask bit. */
static bool GridPathLine_CellBlocksSegment(FieldGridRegionMask callerBlockingMask,GridScratchCell *cell)

{
  GridScratchStateMask cellState;

  cellState = cell->stateMask;
  return (int)cellState < 0 || (g_GridPathEntityClassMask & cellState) == 0 ||
         (g_GridPathBlockingMask & cellState) != 0 || (callerBlockingMask & cellState) != 0;
}

/* Downward segment: scans one column from columnStartCell up to columnEndCell (inclusive); true as soon as a
   cell other than startCell blocks the segment. */
static bool GridPathLine_ColumnBlocksSegment(FieldGridRegionMask callerBlockingMask,GridScratchCell *startCell,
          GridScratchCell *columnStartCell,GridScratchCell *columnEndCell)

{
  GridScratchCell *columnScanCell;

  columnScanCell = columnStartCell;
  while (columnScanCell == startCell || !GridPathLine_CellBlocksSegment(callerBlockingMask,columnScanCell)) {
    if (columnScanCell == columnEndCell) {
      return false;
    }
    columnScanCell = columnScanCell - g_GridScratchWidth;
  }
  return true;
}

/* Downward segment: moves lineCursor down the rows while the row error allows it, stopping at endCell.
   Returns the new cursor. */
static GridScratchCell *GridPathLine_AdvanceDownRows(GridScratchCell *lineCursor,GridScratchCell *endCell,
          int rowDelta,int columnDelta,int *rowError)

{
  while (*rowError < rowDelta) {
    lineCursor = lineCursor + g_GridScratchWidth;
    *rowError = *rowError + columnDelta;
    if (lineCursor == endCell) break;
  }
  return lineCursor;
}

/* Rasterises the straight line from startCell (at startRow, startColumn) to endCell over the scratch grid and
   returns true as soon as a cell other than startCell is blocked (bit 31), lacks the mover's faction
   presence bit (g_GridPathEntityClassMask), or has a g_GridPathBlockingMask or callerBlockingMask bit; false
   when the whole line is clear. The line is always walked left to right;
   upward lines step row by row, downward lines column by column.
*/
bool GridPathLine_TestHexSegmentBlocked(FieldGridRegionMask callerBlockingMask,FieldGridCellCoordinate startRow,
          FieldGridCellCoordinate startColumn,GridScratchCell *startCell,GridScratchCell *endCell)

{
  uint32_t endCellIndex;
  int rowDelta;
  int upwardStepThreshold;
  int columnsLeft;
  int columnDelta;
  int slopeError;
  int rowError;
  GridScratchCell *lineCursor;
  GridScratchCell *columnEndCell;

  endCellIndex = (uint32_t)(endCell - g_GridScratchPrimary);
  rowDelta = endCellIndex / g_GridScratchWidth - startRow;
  columnDelta = endCellIndex % g_GridScratchWidth - startColumn;
  lineCursor = startCell;
  if (columnDelta < 0) {
    rowDelta = -rowDelta;
    columnDelta = -columnDelta;
    lineCursor = endCell;
    endCell = startCell;
  }
  if (rowDelta < 0) {
    slopeError = 0;
    upwardStepThreshold = -rowDelta - columnDelta;
    while (lineCursor == startCell || !GridPathLine_CellBlocksSegment(callerBlockingMask,lineCursor)) {
      if (lineCursor == endCell) {
        return false;
      }
      if (slopeError < upwardStepThreshold) {
        /* step up one row only */
        slopeError = slopeError + columnDelta * 2;
        lineCursor = lineCursor - g_GridScratchWidth;
      }
      else {
        if (slopeError == upwardStepThreshold) {
          /* diagonal: step up one row, then right */
          slopeError = slopeError + columnDelta * 2;
          lineCursor = lineCursor - g_GridScratchWidth;
        }
        slopeError = slopeError + rowDelta * 2;
        lineCursor++;
      }
    }
    return true;
  }
  rowError = 0;
  slopeError = 0;
  columnsLeft = columnDelta;
  columnEndCell = lineCursor;
  lineCursor = GridPathLine_AdvanceDownRows(lineCursor,endCell,rowDelta,columnDelta,&rowError);
  rowError = rowError - rowDelta;
  /* scan each column from lineCursor up to columnEndCell, then move one column right; the rows are advanced
     again before every column except the last one */
  while (!GridPathLine_ColumnBlocksSegment(callerBlockingMask,startCell,lineCursor,columnEndCell)) {
    if (columnsLeft == 0) {
      return false;
    }
    columnsLeft--;
    lineCursor++;
    columnEndCell++;
    for (; slopeError <= -columnDelta; slopeError = slopeError + columnDelta) {
      columnEndCell = columnEndCell + g_GridScratchWidth;
    }
    slopeError = slopeError - rowDelta;
    if (columnsLeft != 0) {
      lineCursor = GridPathLine_AdvanceDownRows(lineCursor,endCell,rowDelta,columnDelta,&rowError);
    }
    rowError = rowError - rowDelta;
  }
  return true;
}
