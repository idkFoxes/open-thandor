/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/grid.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/terrain/grid.h>
#include <thandor/world/terrain/visuals.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

static const uint64_t g_FieldGridOccupancyMmxHighBitMask = 0x8080808080808080ull;

/* Q28 unit vector */
static GraphicsFixedVec3 g_TerrainLightDirection = {0};

TerrainDirectionRecord g_TerrainDirectionRecordTable256[256] = {0};

/* Implementation ownership: world/terrain/grid. */

/* Deforms the terrain around a world point (crater/mound of an effect): clips the cell rectangle around the
   circle to the grid interior, applies FieldGridCell_ApplyRadialTerrainHeightDeltaAndMaterial to every cell in
   it, then recomputes the triangle normals and the directional light of the same rectangle. Called by effect
   maintenance slot 2 (EffectModelRuntimeMaintenance_UpdateLifecycleTintScaleAndTransitions) when a finished
   effect invokes its linked handler. (The original also returns a failure flag: clear after the edit, set for a
   non-positive radius or an empty rectangle; the only caller ignores it.)
*/
void FieldGrid_ApplyRadialTerrainHeightDeltaAndRefreshSurface
          (TerrainMaterialIndex terrainMaterialIndexOrNegativeSentinel,
          FieldGridRadiusUnits radiusWorldUnits,Q12 terrainHeightDeltaAmplitudeQ12,
          Q12 centerWorldYQ12,Q12 centerWorldXQ12,FieldGridAsset *fieldGrid)

{
  FieldGridDimension rowLength;
  uint32_t horizontalRadiusQ12;
  int leftWorldXQ12;
  int minColumn;
  int minRow;
  int maxColumnExclusive;
  int maxRowExclusive;
  int columnCount;
  int rowCount;
  int row;
  int column;
  FieldGridCell *firstCell;
  FieldGridCell *rowStart;
  FieldGridCell *cell;
  FieldGridCoordinates minCornerGrid;
  FieldGridCoordinates maxCornerGrid;

  if (radiusWorldUnits <= 0) {
    return;
  }
  /* radius * sqrt(3): the X half-extent of the box that the corners are taken from */
  horizontalRadiusQ12 = FIXED_MUL_SHR(radiusWorldUnits, FIELD_GRID_SQRT3_Q12, Q12_SHIFT);
  leftWorldXQ12 = centerWorldXQ12 - horizontalRadiusQ12;
  minCornerGrid = FieldGrid_WorldToGridQ12(centerWorldYQ12 + radiusWorldUnits,leftWorldXQ12);
  maxCornerGrid = FieldGrid_WorldToGridQ12
                    (centerWorldYQ12 + radiusWorldUnits + radiusWorldUnits * -2,leftWorldXQ12 + horizontalRadiusQ12 * 2);
  rowLength = fieldGrid->gridWidth;
  minColumn = minCornerGrid.columnQ12 >> Q12_SHIFT;
  minRow = minCornerGrid.rowQ12 >> Q12_SHIFT;
  maxColumnExclusive = (maxCornerGrid.columnQ12 >> Q12_SHIFT) + 1;
  maxRowExclusive = (maxCornerGrid.rowQ12 >> Q12_SHIFT) + 1;
  /* clip to the interior: the one-cell border ring is never edited */
  if ((int)rowLength <= maxColumnExclusive) {
    maxColumnExclusive = rowLength - 1;
  }
  if (minColumn < 1) {
    minColumn = 1;
  }
  if (minRow < 1) {
    minRow = 1;
  }
  if ((int)fieldGrid->gridHeight <= maxRowExclusive) {
    maxRowExclusive = fieldGrid->gridHeight - 1;
  }
  columnCount = maxColumnExclusive - minColumn;
  if (columnCount == 0 || maxColumnExclusive < minColumn) {
    return;
  }
  rowCount = maxRowExclusive - minRow;
  if (rowCount == 0 || maxRowExclusive < minRow) {
    return;
  }
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
  firstCell = fieldGrid->cells + minRow * rowLength + minColumn;
  rowStart = firstCell;
  for (row = 0; row < rowCount; row++) {
    cell = rowStart;
    for (column = 0; column < columnCount; column++) {
      FieldGridCell_ApplyRadialTerrainHeightDeltaAndMaterial
                (terrainMaterialIndexOrNegativeSentinel,radiusWorldUnits,
                 terrainHeightDeltaAmplitudeQ12,centerWorldYQ12,centerWorldXQ12,cell);
      cell++;
    }
    rowStart = rowStart + rowLength;
  }
  rowStart = firstCell;
  for (row = 0; row < rowCount; row++) {
    cell = rowStart;
    for (column = 0; column < columnCount; column++) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowLength * sizeof(FieldGridCell),cell); /* row stride in bytes */
      cell++;
    }
    rowStart = rowStart + rowLength;
  }
  rowStart = firstCell;
  for (row = 0; row < rowCount; row++) {
    cell = rowStart;
    for (column = 0; column < columnCount; column++) {
      FieldGridCell_ComputeDirectionalLightColor(cell);
      cell++;
    }
    rowStart = rowStart + rowLength;
  }
}


/* In-game command INGAME_COMMAND_TERRAIN_RELAXATION (0x3200, handler at INGAME_COMMAND_CODE_BASE + code):
   runs passCount pairs of forward and reverse water relaxation sweeps over the active field grid, the
   sign-gated pair or (mode bit 0 set) the ungated land-tool pair. Queued or called directly by
   InGameCommandRange_DispatchState0/1 (ui/ingame/commands.c) with 0x80 passes. passCount must not be 0.
*/
void TerrainGrid_RunDirectionalRelaxationPasses(FrontendPlayerRuntimeId playerRuntimeId,uint32_t reservedZero,
          TerrainRelaxationPassCount passCount,TerrainRelaxationMode mode)

{
  FieldGridAsset *fieldGrid;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  if ((mode & TERRAIN_RELAXATION_UNGATED_LAND_TOOL) == TERRAIN_RELAXATION_SIGN_GATED) {
    do {
      TerrainGrid_RelaxNeighborHeightsForwardWithSignGate(fieldGrid);
      TerrainGrid_RelaxNeighborHeightsReverseWithSignGate(fieldGrid);
      passCount--;
    } while (passCount != 0);
  }
  else {
    do {
      TerrainGrid_RelaxNeighborHeightsForward(fieldGrid);
      TerrainGrid_RelaxNeighborHeightsReverse(fieldGrid);
      passCount--;
    } while (passCount != 0);
  }
}


/* Shared by FieldGrid_ApplyPositiveCellDeltas and FieldGrid_ApplyNegativeCellDeltas after a cell's height changed:
   unless the cell lies on the grid edge, refreshes the normals and light of the cell and its six neighbours (the
   left and right one only while their scratch entry is 0). scratchEntry is the cell's entry in the scratch plane. */
static void FieldGridCell_RefreshChangedCellAndNeighbours(int rowStrideBytes,FieldGridCell *cell,
          const int *scratchEntry)

{
  FieldGridCell *rowAboveCell;
  FieldGridCell *rowBelowLeftCell;

  if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
    return;
  }
  FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell);
  FieldGridCell_ComputeDirectionalLightColor(cell);
  if (((cell[-1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) && (scratchEntry[-1] == 0)) {
    FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell - 1);
    FieldGridCell_ComputeDirectionalLightColor(cell - 1);
  }
  if (((cell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) && (scratchEntry[1] == 0)) {
    FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell + 1);
    FieldGridCell_ComputeDirectionalLightColor(cell + 1);
  }
  rowAboveCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes);
  if ((rowAboveCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
    FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,rowAboveCell);
    FieldGridCell_ComputeDirectionalLightColor(rowAboveCell);
  }
  if ((rowAboveCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
    FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,rowAboveCell + 1);
    FieldGridCell_ComputeDirectionalLightColor(rowAboveCell + 1);
  }
  rowBelowLeftCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell - 1,rowStrideBytes);
  if ((rowBelowLeftCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
    FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,rowBelowLeftCell);
    FieldGridCell_ComputeDirectionalLightColor(rowBelowLeftCell);
  }
  if ((rowBelowLeftCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
    FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,rowBelowLeftCell + 1);
    FieldGridCell_ComputeDirectionalLightColor(rowBelowLeftCell + 1);
  }
}


/* In-game command INGAME_COMMAND_EDITOR_RAISE_HEIGHTS (0x1F70, handler at INGAME_COMMAND_CODE_BASE + code),
   terrain-editor drag (mode G 0, C 0): pulls the terrain towards the anchor cell's height moved by the vertical
   drag distance, with a cosine falloff over the horizontal drag distance. The player's scratch plane holds the
   height change of the previous drag step: it is undone first, the plane is refilled with the current heights
   and FieldGrid_ProcessHorizontalSpan moves them towards the target around the anchor (or around every selected
   pair); the difference is applied again and kept in the plane. Water surfaces stay at their level
   (waterSurfaceDelta moves opposite to the terrain). Called directly or through the command queue by
   InGameUiCommand_UpdateInteractionByMode.
*/
void FieldGrid_ApplyPositiveCellDeltas(PlayerRuntimeId playerRuntimeId,Q12 anchorRowQ12,Q12 anchorColumnQ12,
          PackedFieldGridDeltaXY16 packedDragDeltaXY16)

{
  FieldGridAsset *fieldGrid;
  FieldGridDimension rowLength;
  int cellDelta;
  int targetHeight;
  uint32_t remainingPairCount;
  int cellCount;
  int remainingCellCount;
  int rowStrideBytes;
  FieldGridCell *firstCell;
  FieldGridCell *cell;
  SelectionPlayerPairRecord *pairRecord;
  int *scratchHeightCursor;
  Bool8 containsAnchorPair;
  int *accumulatorPlane;

  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  accumulatorPlane = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->terrainHeightScratchPlane;
  rowLength = fieldGrid->gridWidth;
  cellCount = rowLength * fieldGrid->gridHeight;
  rowStrideBytes = rowLength * sizeof(FieldGridCell);
  firstCell = fieldGrid->cells;
  /* undo the previous step and refill the plane with the current heights */
  cell = firstCell;
  scratchHeightCursor = accumulatorPlane;
  for (remainingCellCount = cellCount; remainingCellCount != 0; remainingCellCount--) {
    cellDelta = *scratchHeightCursor;
    if (cellDelta != 0) {
      cell->terrainHeight = cell->terrainHeight - cellDelta;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta + cellDelta;
      FieldGridCell_RefreshChangedCellAndNeighbours(rowStrideBytes,cell,scratchHeightCursor);
    }
    *scratchHeightCursor = cell->terrainHeight;
    cell++;
    scratchHeightCursor++;
  }
  /* the drag distances are packed as vertical << 16 | horizontal (signed 16-bit each) */
  containsAnchorPair = SelectionPlayerPairList_ContainsPair(anchorRowQ12,anchorColumnQ12,playerRuntimeId);
  if (containsAnchorPair) {
    FieldGrid_ProcessHorizontalSpan
              (anchorRowQ12,anchorColumnQ12,(int)packedDragDeltaXY16 >> 16,
               (int)(short)packedDragDeltaXY16,anchorRowQ12,anchorColumnQ12,accumulatorPlane,
               fieldGrid);
  }
  else {
    pairRecord = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->markedCells;
    for (remainingPairCount = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->markedCellCount;
        remainingPairCount != 0; remainingPairCount--) {
      FieldGrid_ProcessHorizontalSpan
                (anchorRowQ12,anchorColumnQ12,(int)packedDragDeltaXY16 >> 16,
                 (int)(short)packedDragDeltaXY16,pairRecord->pairValue,pairRecord->pairKey,accumulatorPlane,
                 fieldGrid);
      pairRecord++;
    }
  }
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
  /* apply the new heights and keep their difference in the plane for the next step */
  cell = firstCell;
  scratchHeightCursor = accumulatorPlane;
  for (remainingCellCount = cellCount; remainingCellCount != 0; remainingCellCount--) {
    LOCK();
    targetHeight = *scratchHeightCursor;
    *scratchHeightCursor = 0;
    UNLOCK();
    cellDelta = targetHeight - cell->terrainHeight;
    if (cellDelta != 0) {
      cell->terrainHeight = cell->terrainHeight + cellDelta;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - cellDelta;
      *scratchHeightCursor = cellDelta;
      FieldGridCell_RefreshChangedCellAndNeighbours(rowStrideBytes,cell,scratchHeightCursor);
    }
    cell++;
    scratchHeightCursor++;
  }
}


/* In-game command INGAME_COMMAND_EDITOR_LOWER_HEIGHTS (0x2290, handler at INGAME_COMMAND_CODE_BASE + code),
   terrain-editor drag (mode G 0, C 1): raises or lowers the terrain by the vertical drag distance, with a cosine
   falloff over the horizontal drag distance. The player's scratch plane holds the height change of the previous
   drag step: it is undone and cleared, then FieldGrid_ProcessVerticalSpan writes the new per-cell offsets around
   the anchor (or around every selected pair) and they are added to the terrain; water surfaces stay at their
   level. Called directly or through the command queue by InGameUiCommand_UpdateInteractionByMode.
*/
void FieldGrid_ApplyNegativeCellDeltas(PlayerRuntimeId playerRuntimeId,Q12 anchorRowQ12,Q12 anchorColumnQ12,
          PackedFieldGridDeltaXY16 packedDragDeltaXY16)

{
  FieldGridAsset *fieldGrid;
  FieldGridDimension rowLength;
  int cellDelta;
  uint32_t remainingPairCount;
  int cellCount;
  int remainingCellCount;
  int rowStrideBytes;
  FieldGridCell *firstCell;
  FieldGridCell *cell;
  SelectionPlayerPairRecord *pairRecord;
  int *scratchHeightCursor;
  Bool8 containsAnchorPair;
  int *accumulatorPlane;

  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  accumulatorPlane = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->terrainHeightScratchPlane;
  rowLength = fieldGrid->gridWidth;
  cellCount = rowLength * fieldGrid->gridHeight;
  rowStrideBytes = rowLength * sizeof(FieldGridCell);
  firstCell = fieldGrid->cells;
  /* undo and clear the previous step */
  cell = firstCell;
  scratchHeightCursor = accumulatorPlane;
  for (remainingCellCount = cellCount; remainingCellCount != 0; remainingCellCount--) {
    cellDelta = *scratchHeightCursor;
    if (cellDelta != 0) {
      cell->terrainHeight = cell->terrainHeight - cellDelta;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta + cellDelta;
      *scratchHeightCursor = 0;
      FieldGridCell_RefreshChangedCellAndNeighbours(rowStrideBytes,cell,scratchHeightCursor);
    }
    cell++;
    scratchHeightCursor++;
  }
  /* the drag distances are packed as vertical << 16 | horizontal (signed 16-bit each) */
  containsAnchorPair = SelectionPlayerPairList_ContainsPair(anchorRowQ12,anchorColumnQ12,playerRuntimeId);
  if (containsAnchorPair) {
    FieldGrid_ProcessVerticalSpan
              ((int)packedDragDeltaXY16 >> 16,(int)(short)packedDragDeltaXY16,anchorRowQ12,
               anchorColumnQ12,accumulatorPlane,fieldGrid);
  }
  else {
    pairRecord = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->markedCells;
    for (remainingPairCount = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->markedCellCount;
        remainingPairCount != 0; remainingPairCount--) {
      FieldGrid_ProcessVerticalSpan
                ((int)packedDragDeltaXY16 >> 16,(int)(short)packedDragDeltaXY16,pairRecord->pairValue,
                 pairRecord->pairKey,accumulatorPlane,fieldGrid);
      pairRecord++;
    }
  }
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
  /* apply the new offsets; they stay in the plane for the next step */
  cell = firstCell;
  scratchHeightCursor = accumulatorPlane;
  for (remainingCellCount = cellCount; remainingCellCount != 0; remainingCellCount--) {
    cellDelta = *scratchHeightCursor;
    if (cellDelta != 0) {
      cell->terrainHeight = cell->terrainHeight + cellDelta;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - cellDelta;
      FieldGridCell_RefreshChangedCellAndNeighbours(rowStrideBytes,cell,scratchHeightCursor);
    }
    cell++;
    scratchHeightCursor++;
  }
}


/* In-game command INGAME_COMMAND_EDITOR_REBUILD_INFLUENCE (0x2AE0, handler at INGAME_COMMAND_CODE_BASE + code),
   terrain-editor smoothing brush (mode G 0, C 2): smooths the cell at the given grid position (or at every
   selected pair) with FieldGrid_ApplyRectangularTransition, then refreshes normals and light wherever the height
   now differs from the player's scratch plane (filled with the heights by FieldGrid_ResetLocalInfluenceState
   when the stroke began). Called directly or through the command queue by
   InGameUiCommand_UpdateInteractionByMode.
*/
void FieldGrid_RebuildLocalInfluenceState
          (PlayerRuntimeId playerRuntimeId,FieldGridCommandReservedValue reservedCommandValue,
          Q12 gridRowQ12,Q12 gridColumnQ12)

{
  SelectionPlayerRuntimeBlock *playerBlock;
  FieldGridAsset *fieldGrid;
  FieldGridDimension rowLength;
  uint32_t remainingPairCount;
  int remainingCellCount;
  int rowStrideBytes;
  FieldGridCell *cell;
  FieldGridCell *scanCell;
  SelectionPlayerPairRecord *pairRecord;
  int *scratchHeightCursor;
  Bool8 containsAnchorPair;
  
  playerBlock = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId];
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  containsAnchorPair = SelectionPlayerPairList_ContainsPair(gridRowQ12,gridColumnQ12,playerRuntimeId);
  if (containsAnchorPair) {
    FieldGrid_ApplyRectangularTransition(gridRowQ12,gridColumnQ12,fieldGrid);
  }
  else {
    pairRecord = playerBlock->markedCells;
    for (remainingPairCount = playerBlock->markedCellCount; remainingPairCount != 0; remainingPairCount--) {
      FieldGrid_ApplyRectangularTransition(pairRecord->pairValue,pairRecord->pairKey,fieldGrid);
      pairRecord++;
    }
  }
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
  scratchHeightCursor = playerBlock->terrainHeightScratchPlane;
  rowLength = fieldGrid->gridWidth;
  remainingCellCount = rowLength * fieldGrid->gridHeight;
  rowStrideBytes = rowLength * sizeof(FieldGridCell);
  scanCell = fieldGrid->cells;
  /* neighbour refresh as in FieldGrid_ApplyPositiveCellDeltas */
  do {
    if ((*scratchHeightCursor != scanCell->terrainHeight) && ((scanCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0)) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell);
      FieldGridCell_ComputeDirectionalLightColor(scanCell);
      if (((scanCell[-1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) && (scratchHeightCursor[-1] == 0)) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell - 1);
        FieldGridCell_ComputeDirectionalLightColor(scanCell - 1);
      }
      if (((scanCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) && (scratchHeightCursor[1] == 0)) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell + 1);
        FieldGridCell_ComputeDirectionalLightColor(scanCell + 1);
      }
      scanCell = scanCell - rowLength; /* row above */
      if ((scanCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell);
        FieldGridCell_ComputeDirectionalLightColor(scanCell);
      }
      if ((scanCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell + 1);
        FieldGridCell_ComputeDirectionalLightColor(scanCell + 1);
      }
      scanCell = scanCell + rowLength * 2 - 1; /* row below, one to the left */
      if ((scanCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell);
        FieldGridCell_ComputeDirectionalLightColor(scanCell);
      }
      cell = scanCell + 1;
      if ((scanCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell);
        FieldGridCell_ComputeDirectionalLightColor(cell);
      }
      scanCell = cell - rowLength; /* back to the changed cell */
    }
    scanCell++;
    scratchHeightCursor++;
    remainingCellCount--;
  } while (remainingCellCount != 0);
}


/* Recomputes the packed normal angles of both terrain triangles of every interior cell (the one-cell
   border ring is skipped) after the heights changed, and marks the field grid dirty (runtimeStateFlags
   bit 0).
*/
void FieldGrid_RecomputeInteriorTriangleNormalAngles(FieldGridAsset *fieldGrid)

{
  FieldGridDimension rowLength;
  int columnsLeft;
  int rowsLeft;
  FieldGridCell *cell;
  FieldGridCell *cellCursor;

  if (fieldGrid != NULL) {
    fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
    rowLength = fieldGrid->gridWidth;
    rowsLeft = fieldGrid->gridHeight - 2;
    columnsLeft = rowLength - 2;
    /* cell (row 1, column 1) */
    cellCursor = &fieldGrid->cells[rowLength + 1];
    do {
      do {
        cell = cellCursor;
        FieldGridCell_RecomputeTriangleNormalAngles(rowLength * sizeof(FieldGridCell),cell); /* row stride in bytes */
        columnsLeft--;
        cellCursor = cell + 1;
      } while (columnsLeft != 0);
      columnsLeft = rowLength - 2;
      rowsLeft--;
      cellCursor = cell + 3; /* skip the right border cell of this row and the left one of the next */
    } while (rowsLeft != 0);
  }
}


/* Sets the terrain light direction (g_TerrainLightDirection, Q28) from an elevation and azimuth
   and relights every interior cell with it (border ring skipped); marks the field grid dirty.
*/
void FieldGrid_RecomputeInteriorDirectionalLighting
          (AngleTurn32 lightElevationAngle,AngleTurn32 lightAzimuthAngle,FieldGridAsset *fieldGrid)

{
  FieldGridDimension rowLength;
  int columnsLeft;
  int rowsLeft;
  FieldGridCell *cell;
  FieldGridCell *cellCursor;

  FixedMath_WriteDirectionQ28
            (&g_TerrainLightDirection,lightElevationAngle,lightAzimuthAngle);
  if (fieldGrid != NULL) {
    fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
    rowLength = fieldGrid->gridWidth;
    rowsLeft = fieldGrid->gridHeight - 2;
    columnsLeft = rowLength - 2;
    /* cell (row 1, column 1), see FieldGrid_RecomputeInteriorTriangleNormalAngles */
    cellCursor = &fieldGrid->cells[rowLength + 1];
    do {
      do {
        cell = cellCursor;
        FieldGridCell_ComputeDirectionalLightColor(cell);
        columnsLeft--;
        cellCursor = cell + 1;
      } while (columnsLeft != 0);
      columnsLeft = rowLength - 2;
      rowsLeft--;
      cellCursor = cell + 3;
    } while (rowsLeft != 0);
  }
}


/* Terrain shaping at a world point (used by armies whose class shapes the ground under them): sets the
   grid vertex nearest to (worldX, worldY) to the height worldZ (moving its water surface by the opposite
   amount, so the water level stays) and lets the six wedge scans around
   the vertex adapt the neighbouring terrain; heightDeltaSourceValue / 0x240 (clamped to 1..255) limits
   those scans. Border cells and cells under water are left alone. (The original also returns a failure
   flag: clear when the height was applied, set otherwise; this C version returns nothing. The only caller,
   ArmyRuntime_ClassCommandHandlerGroupA, ignores it: the code after the call joins the skip path and
   overwrites the flag.)
*/
void FieldGrid_ApplyHeightAtWorldPointAndRefreshNeighbors
          (TerrainHeightBrushDeltaSource heightDeltaSourceValue,Q12 worldZQ12,Q12 worldYQ12,
          Q12 worldXQ12,FieldGridAsset *fieldGrid)

{
  uint32_t baseColumn;
  uint32_t columnFractionQ12;
  uint32_t rowFractionQ12;
  uint32_t fractionSumQ12;
  uint32_t rowLength;
  int cellIndex;
  Q12 heightDeltaQ12;
  int rowStrideBytes;
  FieldGridCell *vertexCell;
  FieldGridCell *rightCell;
  FieldGridCell *upperRightCell;
  FieldGridCell *upperCell;
  FieldGridCell *leftCell;
  FieldGridCell *lowerLeftCell;
  FieldGridCoordinates gridCoordinates;
  uint32_t targetRow;
  uint32_t targetColumn;

  if (fieldGrid == NULL) {
    return;
  }
  g_TerrainScanStepLimit = heightDeltaSourceValue / TERRAIN_SCAN_RADIUS_PER_STEP;
  if (g_TerrainScanStepLimit == 0) {
    g_TerrainScanStepLimit = 1;
  }
  else if (255 < g_TerrainScanStepLimit) {
    g_TerrainScanStepLimit = 255;
  }
  g_TerrainScanReferenceHeight = worldZQ12;
  gridCoordinates = FieldGrid_WorldToGridQ12(worldYQ12,worldXQ12);
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
  baseColumn = gridCoordinates.columnQ12 >> Q12_SHIFT;
  targetRow = gridCoordinates.rowQ12 >> Q12_SHIFT;
  columnFractionQ12 = (uint32_t)(gridCoordinates.columnQ12 & Q12_FRACTION_MASK);
  rowFractionQ12 = (uint32_t)(gridCoordinates.rowQ12 & Q12_FRACTION_MASK);
  /* pick the nearest vertex of the triangulated cell from the Q12 fractions (0x1000 = one cell) */
  fractionSumQ12 = rowFractionQ12 + columnFractionQ12 * 2;
  targetColumn = baseColumn;
  if (fractionSumQ12 < FIELD_GRID_CELL_Q12) {
    if (FIELD_GRID_CELL_Q12 - 1 < columnFractionQ12 + rowFractionQ12 * 2) {
      targetRow++;
    }
  }
  else if (fractionSumQ12 < FIELD_GRID_TWO_CELLS_Q12 + 1) {
    targetColumn = baseColumn + 1;
    if (columnFractionQ12 < rowFractionQ12) {
      targetRow++;
      targetColumn = baseColumn;
    }
  }
  else {
    targetColumn = baseColumn + 1;
    if (FIELD_GRID_TWO_CELLS_Q12 - 1 < columnFractionQ12 + rowFractionQ12 * 2) {
      targetRow++;
    }
  }
  g_TerrainScanRowStrideBytes = fieldGrid->gridWidth << 7; /* 0x80-byte cells */
  rowLength = fieldGrid->gridWidth & FIELD_GRID_ROW_STRIDE_WIDTH_MASK;
  if ((int)targetColumn < 0 || (int)targetRow < 0 || fieldGrid->gridHeight <= targetRow ||
      rowLength <= targetColumn) {
    return;
  }
  cellIndex = targetRow * rowLength + targetColumn;
  vertexCell = &fieldGrid->cells[cellIndex];
  if ((vertexCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0 || 0 < vertexCell->waterSurfaceDelta) {
    return;
  }
  heightDeltaQ12 = g_TerrainScanReferenceHeight - vertexCell->terrainHeight;
  vertexCell->terrainHeight = vertexCell->terrainHeight + heightDeltaQ12;
  vertexCell->waterSurfaceDelta = vertexCell->waterSurfaceDelta - heightDeltaQ12;
  /* the six neighbours of vertex cell C, one per wedge: C+1, C+1-W, C-W, C-1, C-1+W, C+W (W = grid
     width, stepped by the byte stride g_TerrainScanRowStrideBytes) */
  rowStrideBytes = g_TerrainScanRowStrideBytes;
  rightCell = vertexCell + 1;
  upperRightCell = (FieldGridCell *)((uint8_t *)rightCell - rowStrideBytes);
  upperCell = upperRightCell - 1;
  leftCell = (FieldGridCell *)((uint8_t *)(upperCell - 1) + rowStrideBytes);
  lowerLeftCell = (FieldGridCell *)((uint8_t *)leftCell + rowStrideBytes);
  TerrainHeightDelta_ApplyWedge0(0,rightCell);
  TerrainHeightDelta_ApplyWedge1(0,upperRightCell);
  TerrainHeightDelta_ApplyWedge2(0,upperCell);
  TerrainHeightDelta_ApplyWedge3(0,leftCell);
  TerrainHeightDelta_ApplyWedge4(0,lowerLeftCell);
  TerrainHeightDelta_ApplyWedge5(0,lowerLeftCell + 1);
}


/* In-game command INGAME_COMMAND_EDITOR_PAINT_MATERIAL (0x2770, handler at INGAME_COMMAND_CODE_BASE + code),
   terrain-editor material brush (mode G 1): writes the material index transitionValue into the cell at the Q12
   grid row/column (or into the cell of every selected pair) with FieldGrid_ApplySingleCellTransition and marks
   the surface dirty. Called directly or through the command queue by InGameUiCommand_UpdateInteractionByMode.
*/
void FieldGrid_ApplyLocalCellUpdate
          (PlayerRuntimeId playerRuntimeId,FieldGridTransitionValue transitionValue,Q12 gridRowQ12,
          Q12 gridColumnQ12)

{
  SelectionPlayerRuntimeBlock *playerBlock;
  FieldGridAsset *fieldGrid;
  uint32_t remainingPairCount;
  SelectionPlayerPairRecord *pairRecord;
  Bool8 containsAnchorPair;
  
  playerBlock = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId];
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  containsAnchorPair = SelectionPlayerPairList_ContainsPair(gridRowQ12,gridColumnQ12,playerRuntimeId);
  if (containsAnchorPair) {
    FieldGrid_ApplySingleCellTransition(transitionValue,gridRowQ12,gridColumnQ12,fieldGrid);
  }
  else {
    pairRecord = playerBlock->markedCells;
    for (remainingPairCount = playerBlock->markedCellCount; remainingPairCount != 0; remainingPairCount--) {
      FieldGrid_ApplySingleCellTransition
                (transitionValue,pairRecord->pairValue,pairRecord->pairKey,fieldGrid);
      pairRecord++;
    }
  }
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
}


/* In-game command INGAME_COMMAND_EDITOR_SMOOTH (0x3260, handler at INGAME_COMMAND_CODE_BASE + code),
   terrain-editor water drag (mode G 2, E 0): marks the surface dirty and lets FieldGrid_ApplyEncodedUpdateCore
   move the water level of the cell at the Q12 grid row/column by the vertical drag distance (the signed high 16
   bits of packedDragDeltaXY16). Called directly or through the command queue by
   InGameUiCommand_UpdateInteractionByMode. (The command constant's name notwithstanding, nothing
   is smoothed here; FieldGrid_RebuildLocalInfluenceState is the smoothing brush.)
*/
void FieldGrid_ApplyEncodedCellUpdate(PlayerRuntimeId playerRuntimeId,Q12 gridRowQ12,Q12 gridColumnQ12,
          PackedFieldGridDeltaXY16 packedDragDeltaXY16)

{
  FieldGridAsset *fieldGrid;
  FieldGridRuntimeFlags *runtimeFlagsField;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  runtimeFlagsField = &fieldGrid->runtimeStateFlags;
  *runtimeFlagsField = *runtimeFlagsField | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
  FieldGrid_ApplyEncodedUpdateCore((int)packedDragDeltaXY16 >> 16,gridRowQ12,gridColumnQ12,fieldGrid);
}


/* In-game command INGAME_COMMAND_EDITOR_SET_RECEIVER_EXCLUDED (0x32A0, handler at INGAME_COMMAND_CODE_BASE +
   code), terrain-editor flag toggle (mode G 2, E 1): replaces FIELD_CELL_FLUID_RECEIVER_EXCLUDED of the cell at
   the Q12 grid row/column with setMask (0 or the flag, g_UiCommandTerrainMaskToggleValue) and marks the surface
   dirty. Called directly or through the command queue by InGameUiCommand_UpdateInteractionByMode.
*/
void FieldGrid_SetCellFluidReceiverExcluded
          (PlayerRuntimeId playerRuntimeId,FieldGridRegionMask setMask,Q12 gridRowQ12,Q12 gridColumnQ12)

{
  FieldGridAsset *fieldGrid;
  FieldGridRuntimeFlags *runtimeFlagsField;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  FieldGrid_ApplyMaskedRegionCore(~FIELD_CELL_FLUID_RECEIVER_EXCLUDED,setMask,gridRowQ12,gridColumnQ12,fieldGrid);
  runtimeFlagsField = &fieldGrid->runtimeStateFlags;
  *runtimeFlagsField = *runtimeFlagsField | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
}


/* In-game command INGAME_COMMAND_EDITOR_SET_SOURCE_EXCLUDED (0x32E0, handler at INGAME_COMMAND_CODE_BASE +
   code), terrain-editor flag toggle (mode G 2, E 2 and up): replaces FIELD_CELL_FLUID_SOURCE_EXCLUDED of the
   cell at the Q12 grid row/column with setMask (0 or the flag, g_UiCommandTerrainMaskToggleValue) and marks the
   surface dirty. Called directly or through the command queue by InGameUiCommand_UpdateInteractionByMode.
*/
void FieldGrid_SetCellFluidSourceExcluded
          (PlayerRuntimeId playerRuntimeId,FieldGridRegionMask setMask,Q12 gridRowQ12,Q12 gridColumnQ12)

{
  FieldGridAsset *fieldGrid;
  FieldGridRuntimeFlags *runtimeFlagsField;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  FieldGrid_ApplyMaskedRegionCore(~FIELD_CELL_FLUID_SOURCE_EXCLUDED,setMask,gridRowQ12,gridColumnQ12,fieldGrid);
  runtimeFlagsField = &fieldGrid->runtimeStateFlags;
  *runtimeFlagsField = *runtimeFlagsField | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
}


/* In-game command INGAME_COMMAND_EDITOR_APPLY_REGION_MASK (0x3320, handler at INGAME_COMMAND_CODE_BASE + code),
   terrain-editor resource brush (mode G 5): sets cell flag bit 11 + materialBitIndex (FIELD_CELL_XENITE_SUPPORT
   for 0, FIELD_CELL_TRITIUM_SUPPORT for 1) in the cell at the Q12 grid row/column, or clears it when bit 31 of
   materialBitIndex is set (g_UiCommandCallerMaskHighBit; the shift count only uses the low 5 bits), and marks
   the surface dirty. Called directly or through the command queue by InGameUiCommand_UpdateInteractionByMode.
*/
void FieldGrid_SetCellResourceSupportFlag
          (PlayerRuntimeId playerRuntimeId,FieldGridMaterialBitIndex materialBitIndex,Q12 gridRowQ12,
          Q12 gridColumnQ12)

{
  FieldGridAsset *fieldGrid;
  FieldGridRegionMask setMask;
  uint32_t preserveMask;
  FieldGridRuntimeFlags *runtimeFlagsField;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  setMask = FIELD_CELL_XENITE_SUPPORT << ((uint8_t)materialBitIndex & 31);
  preserveMask = setMask ^ 0xffffffff;
  if (materialBitIndex < 0) {
    setMask = 0;
  }
  FieldGrid_ApplyMaskedRegionCore(preserveMask,setMask,gridRowQ12,gridColumnQ12,fieldGrid);
  runtimeFlagsField = &fieldGrid->runtimeStateFlags;
  *runtimeFlagsField = *runtimeFlagsField | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
}


/* Snaps a world position to the nearest grid vertex (cell): *outPoint receives that cell's worldX, worldY and
   terrain height and true is returned. Outside the grid false is returned and *outPoint is the input position
   with height 0 (always written).
*/
Bool8 FieldGrid_GetNearestTerrainPoint(Q12 worldY,Q12 worldX,FieldGridAsset *field,FixedVectorQ12 *outPoint)

{
  int gridColumnIndex;
  int cellIndex;
  uint32_t gridHalfRowCoordinateQ12;
  int gridRowIndex;
  Q12 terrainHeightQ12;
  Bool8 outOfBounds;

  /* FieldGrid_WorldToGridQ12 inlined, then rounded (+0x800 = half a cell) to whole cells */
  gridHalfRowCoordinateQ12 =
       FIXED_MUL_SHR(worldY, FIELD_GRID_WORLD_Y_TO_ROW_Q20, Q20_SHIFT + 1);
  gridColumnIndex = (int)((FIXED_MUL_SHR(worldX, FIELD_GRID_WORLD_X_TO_COLUMN_Q20, Q20_SHIFT) - gridHalfRowCoordinateQ12) + FIELD_GRID_CELL_Q12 / 2)
          >> Q12_SHIFT;
  gridRowIndex = (int)(gridHalfRowCoordinateQ12 * 2 + FIELD_GRID_CELL_Q12 / 2) >> Q12_SHIFT;
  if ((gridColumnIndex < 0) || (gridRowIndex < 0) || ((int)field->gridWidth <= gridColumnIndex) ||
      ((int)field->gridHeight <= gridRowIndex)) {
    terrainHeightQ12 = 0;
    outOfBounds = true;
  }
  else {
    cellIndex = field->gridWidth * gridRowIndex + gridColumnIndex;
    worldX = field->cells[cellIndex].worldX;
    worldY = field->cells[cellIndex].worldY;
    terrainHeightQ12 = field->cells[cellIndex].terrainHeight;
    outOfBounds = false;
  }
  outPoint->xQ12 = worldX;
  outPoint->yQ12 = worldY;
  outPoint->zQ12 = terrainHeightQ12;
  return !outOfBounds;
}


/* Snaps a world position to the nearest grid vertex (cell) like FieldGrid_GetNearestTerrainPoint, but returns
   the height of the top surface there (terrainHeight + waterSurfaceDelta) in *outPoint and returns true.
   Outside the grid false is returned and *outPoint is the input position with height 0 (always written).
   Used by SelectionOverlay_DrawWorldPointMarker.
*/
Bool8 FieldGrid_GetNearestTopSurfacePoint(Q12 worldY,Q12 worldX,FieldGridAsset *field,FixedVectorQ12 *outPoint)

{
  int gridColumnIndex;
  int cellIndex;
  int surfaceHeightQ12;
  uint32_t gridHalfRowCoordinateQ12;
  int gridRowIndex;
  Bool8 outOfBounds;

  /* FieldGrid_WorldToGridQ12 inlined, then rounded (+0x800 = half a cell) to whole cells */
  gridHalfRowCoordinateQ12 =
       FIXED_MUL_SHR(worldY, FIELD_GRID_WORLD_Y_TO_ROW_Q20, Q20_SHIFT + 1);
  gridColumnIndex = (int)((FIXED_MUL_SHR(worldX, FIELD_GRID_WORLD_X_TO_COLUMN_Q20, Q20_SHIFT) - gridHalfRowCoordinateQ12) + FIELD_GRID_CELL_Q12 / 2)
          >> Q12_SHIFT;
  gridRowIndex = (int)(gridHalfRowCoordinateQ12 * 2 + FIELD_GRID_CELL_Q12 / 2) >> Q12_SHIFT;
  if ((gridColumnIndex < 0) || (gridRowIndex < 0) || ((int)field->gridWidth <= gridColumnIndex) ||
      ((int)field->gridHeight <= gridRowIndex)) {
    surfaceHeightQ12 = 0;
    outOfBounds = true;
  }
  else {
    cellIndex = field->gridWidth * gridRowIndex + gridColumnIndex;
    worldX = field->cells[cellIndex].worldX;
    worldY = field->cells[cellIndex].worldY;
    surfaceHeightQ12 = field->cells[cellIndex].waterSurfaceDelta + field->cells[cellIndex].terrainHeight;
    outOfBounds = false;
  }
  outPoint->xQ12 = worldX;
  outPoint->yQ12 = worldY;
  outPoint->zQ12 = surfaceHeightQ12;
  return !outOfBounds;
}


/* Water depth at the grid vertex nearest to a world position: the cell's signed waterSurfaceDelta (positive
   when the cell is under water). Outside the grid the result is meaningless (the rounded column index); the
   callers only ask for points inside the field
   (ArmyArticulatedRuntime_UpdateContactChildAndEffects, ArmyRuntime_UpdateTimedShotAndEffectEmitters).
*/
int32_t FieldGrid_GetNearestWaterDelta(Q12 worldY,Q12 worldX,FieldGridAsset *field)

{
  int32_t gridColumnIndex;
  uint32_t gridHalfRowCoordinateQ12;
  int gridRowIndex;

  /* FieldGrid_WorldToGridQ12 inlined, then rounded (+0x800 = half a cell) to whole cells */
  gridHalfRowCoordinateQ12 =
       FIXED_MUL_SHR(worldY, FIELD_GRID_WORLD_Y_TO_ROW_Q20, Q20_SHIFT + 1);
  gridColumnIndex = (int)((FIXED_MUL_SHR(worldX, FIELD_GRID_WORLD_X_TO_COLUMN_Q20, Q20_SHIFT) - gridHalfRowCoordinateQ12) + FIELD_GRID_CELL_Q12 / 2)
          >> Q12_SHIFT;
  gridRowIndex = (int)(gridHalfRowCoordinateQ12 * 2 + FIELD_GRID_CELL_Q12 / 2) >> Q12_SHIFT;
  if ((-1 < gridColumnIndex) && (-1 < gridRowIndex) && (gridColumnIndex < (int)field->gridWidth) &&
      (gridRowIndex < (int)field->gridHeight)) {
    return field->cells[field->gridWidth * gridRowIndex + gridColumnIndex].waterSurfaceDelta;
  }
  /* Original quirk: outside the grid the rounded column index is returned */
  return gridColumnIndex;
}


/* Terrain height at a world position, interpolated linearly over the grid triangle that contains it, stored
   as Q12 in *outHeightQ12. Returns false (and stores height 0) outside the grid or when the cell or its
   diagonal neighbour is a border cell. Entries 0, 2 and 3 of g_FieldGridInterpolationCallbacks5; also called directly by
   ArmyPlacementContact_ApplyTerrainHeight and the army movement code.
*/
Bool8 FieldGrid_InterpolateTerrainHeight(Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid,Q12 *outHeightQ12)

{
  uint32_t gridHalfRowCoordinateQ12;
  uint32_t gridColumnCoordinateQ12;
  int gridColumnIndex;
  int gridRowIndex;
  FieldGridDimension gridWidth;
  FieldGridCell *cell;
  uint32_t columnFractionQ12;
  uint32_t rowFractionQ12;
  int triangleDiagonalWeightQ12;
  int64_t weightedHeightAccumulator;

  gridHalfRowCoordinateQ12 =
       FIXED_MUL_SHR(worldYQ12, FIELD_GRID_WORLD_Y_TO_ROW_Q20, Q20_SHIFT + 1);
  gridColumnCoordinateQ12 =
       FIXED_MUL_SHR(worldXQ12, FIELD_GRID_WORLD_X_TO_COLUMN_Q20, Q20_SHIFT) - gridHalfRowCoordinateQ12;
  gridWidth = fieldGrid->gridWidth;
  gridColumnIndex = (int)gridColumnCoordinateQ12 >> Q12_SHIFT;
  gridRowIndex = (int)(gridHalfRowCoordinateQ12 * 2) >> Q12_SHIFT;
  if (gridColumnIndex < 0 || gridRowIndex < 0 || (int)gridWidth <= gridColumnIndex ||
      (int)fieldGrid->gridHeight <= gridRowIndex) {
    *outHeightQ12 = 0;
    return false;
  }
  /* cell addressing as explained in FieldGrid_InterpolateTopSurfaceHeight */
  cell = fieldGrid->cells + gridRowIndex * gridWidth + gridColumnIndex;
  columnFractionQ12 = gridColumnCoordinateQ12 & Q12_FRACTION_MASK;
  rowFractionQ12 = gridHalfRowCoordinateQ12 * 2 & Q12_FRACTION_MASK;
  if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0 ||
      (cell[gridWidth + 1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
    *outHeightQ12 = 0;
    return false;
  }
  triangleDiagonalWeightQ12 = (columnFractionQ12 + rowFractionQ12) - FIELD_GRID_CELL_Q12;
  if (columnFractionQ12 + rowFractionQ12 < FIELD_GRID_CELL_Q12) {
    weightedHeightAccumulator =
         (int64_t)cell[1].terrainHeight * (int64_t)(int)columnFractionQ12 +
         ((int64_t)cell[gridWidth].terrainHeight * (int64_t)(int)rowFractionQ12 -
          (int64_t)cell->terrainHeight * (int64_t)triangleDiagonalWeightQ12);
  }
  else {
    weightedHeightAccumulator =
         (int64_t)cell[gridWidth + 1].terrainHeight * (int64_t)triangleDiagonalWeightQ12 -
         ((int64_t)cell[gridWidth].terrainHeight * (int64_t)(int)(columnFractionQ12 - FIELD_GRID_CELL_Q12) +
          (int64_t)cell[1].terrainHeight * (int64_t)(int)(rowFractionQ12 - FIELD_GRID_CELL_Q12));
  }
  *outHeightQ12 = FIXED_PRODUCT_SHR(weightedHeightAccumulator, Q12_SHIFT);
  return true;
}


/* Water depth (waterSurfaceDelta) at a world position, interpolated linearly over the grid triangle that
   contains it like FieldGrid_InterpolateTerrainHeight. Returns 0 outside the grid or on a border cell (the
   original also signals failure there; the C prototype drops that). Called by the army movement code
   (ArmyRuntimeClass_UpdateArticulatedMovement, ..UpdateGroundMovementCollisionAndTrackAnimation,
   ..UpdateGroundMovementVariantA).
*/
int32_t FieldGrid_InterpolateWaterDelta(Q12 worldY,Q12 worldX,FieldGridAsset *field)

{
  uint32_t gridHalfRowCoordinateQ12;
  uint32_t gridColumnCoordinateQ12;
  int gridColumnIndex;
  int gridRowIndex;
  FieldGridDimension gridWidth;
  FieldGridCell *cell;
  uint32_t columnFractionQ12;
  uint32_t rowFractionQ12;
  int triangleDiagonalWeightQ12;
  int64_t weightedWaterDeltaAccumulator;

  gridHalfRowCoordinateQ12 =
       FIXED_MUL_SHR(worldY, FIELD_GRID_WORLD_Y_TO_ROW_Q20, Q20_SHIFT + 1);
  gridColumnCoordinateQ12 =
       FIXED_MUL_SHR(worldX, FIELD_GRID_WORLD_X_TO_COLUMN_Q20, Q20_SHIFT) - gridHalfRowCoordinateQ12;
  gridWidth = field->gridWidth;
  gridColumnIndex = (int)gridColumnCoordinateQ12 >> Q12_SHIFT;
  gridRowIndex = (int)(gridHalfRowCoordinateQ12 * 2) >> Q12_SHIFT;
  if (gridColumnIndex < 0 || gridRowIndex < 0 || (int)gridWidth <= gridColumnIndex ||
      (int)field->gridHeight <= gridRowIndex) {
    return 0;
  }
  /* cell addressing as in FieldGrid_InterpolateTopSurfaceHeight */
  cell = field->cells + gridRowIndex * gridWidth + gridColumnIndex;
  columnFractionQ12 = gridColumnCoordinateQ12 & Q12_FRACTION_MASK;
  rowFractionQ12 = gridHalfRowCoordinateQ12 * 2 & Q12_FRACTION_MASK;
  if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0 ||
      (cell[gridWidth + 1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
    return 0;
  }
  triangleDiagonalWeightQ12 = (columnFractionQ12 + rowFractionQ12) - FIELD_GRID_CELL_Q12;
  if (columnFractionQ12 + rowFractionQ12 < FIELD_GRID_CELL_Q12) {
    weightedWaterDeltaAccumulator =
         (int64_t)cell[1].waterSurfaceDelta * (int64_t)(int)columnFractionQ12 +
         ((int64_t)cell[gridWidth].waterSurfaceDelta * (int64_t)(int)rowFractionQ12 -
          (int64_t)cell->waterSurfaceDelta * (int64_t)triangleDiagonalWeightQ12);
  }
  else {
    weightedWaterDeltaAccumulator =
         (int64_t)cell[gridWidth + 1].waterSurfaceDelta * (int64_t)triangleDiagonalWeightQ12 -
         ((int64_t)cell[gridWidth].waterSurfaceDelta * (int64_t)(int)(columnFractionQ12 - FIELD_GRID_CELL_Q12) +
          (int64_t)cell[1].waterSurfaceDelta * (int64_t)(int)(rowFractionQ12 - FIELD_GRID_CELL_Q12));
  }
  return FIXED_PRODUCT_SHR(weightedWaterDeltaAccumulator, Q12_SHIFT);
}


/* Height of the water surface (terrainHeight + waterSurfaceDelta, also where waterSurfaceDelta is negative, i.e.
   the surface lies below the ground) at a world position, interpolated linearly over the grid triangle that
   contains it, stored as Q12 in *outHeightQ12. Returns false (and stores height 0) outside the grid or on a
   border cell. Entry 1 of
   g_FieldGridInterpolationCallbacks5; also called directly by
   ArmyPlacementContact_ApplyWaterSurfaceHeight.
*/
Bool8 FieldGrid_InterpolateWaterSurfaceHeight(Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid,Q12 *outHeightQ12)

{
  int gridColumnIndex;
  int gridRowIndex;
  Q12 gridColumnCoordinateQ12;
  uint32_t columnFractionQ12;
  Q12 gridHalfRowCoordinateQ12;
  uint32_t rowFractionQ12;
  Q12 triangleDiagonalWeightQ12;
  FieldGridDimension gridWidth;
  FieldGridCell *cell;
  int64_t weightedHeightAccumulator;

  gridHalfRowCoordinateQ12 =
       FIXED_MUL_SHR(worldYQ12, FIELD_GRID_WORLD_Y_TO_ROW_Q20, Q20_SHIFT + 1);
  gridColumnCoordinateQ12 =
       FIXED_MUL_SHR(worldXQ12, FIELD_GRID_WORLD_X_TO_COLUMN_Q20, Q20_SHIFT) - gridHalfRowCoordinateQ12;
  gridWidth = fieldGrid->gridWidth;
  gridColumnIndex = gridColumnCoordinateQ12 >> Q12_SHIFT;
  gridRowIndex = gridHalfRowCoordinateQ12 * 2 >> Q12_SHIFT;
  if ((gridColumnIndex < 0) || (gridRowIndex < 0) || ((int)gridWidth <= gridColumnIndex) ||
      ((int)fieldGrid->gridHeight <= gridRowIndex)) {
    *outHeightQ12 = 0;
    return false;
  }
  /* cell addressing as in FieldGrid_InterpolateTopSurfaceHeight */
  cell = fieldGrid->cells + gridRowIndex * gridWidth + gridColumnIndex;
  columnFractionQ12 = gridColumnCoordinateQ12 & Q12_FRACTION_MASK;
  rowFractionQ12 = gridHalfRowCoordinateQ12 * 2 & Q12_FRACTION_MASK;
  if (((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) ||
      ((cell[gridWidth + 1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0)) {
    *outHeightQ12 = 0;
    return false;
  }
  triangleDiagonalWeightQ12 = (columnFractionQ12 + rowFractionQ12) - FIELD_GRID_CELL_Q12;
  if (columnFractionQ12 + rowFractionQ12 < FIELD_GRID_CELL_Q12) {
    weightedHeightAccumulator =
         (int64_t)(cell[1].terrainHeight + cell[1].waterSurfaceDelta) * (int64_t)(int)columnFractionQ12 +
         ((int64_t)(cell[gridWidth].terrainHeight + cell[gridWidth].waterSurfaceDelta) *
          (int64_t)(int)rowFractionQ12 -
          (int64_t)(cell->terrainHeight + cell->waterSurfaceDelta) * (int64_t)triangleDiagonalWeightQ12);
  }
  else {
    weightedHeightAccumulator =
         (int64_t)(cell[gridWidth + 1].terrainHeight + cell[gridWidth + 1].waterSurfaceDelta) *
         (int64_t)triangleDiagonalWeightQ12 -
         ((int64_t)(cell[gridWidth].terrainHeight + cell[gridWidth].waterSurfaceDelta) *
          (int64_t)(int)(columnFractionQ12 - FIELD_GRID_CELL_Q12) +
          (int64_t)(cell[1].terrainHeight + cell[1].waterSurfaceDelta) *
          (int64_t)(int)(rowFractionQ12 - FIELD_GRID_CELL_Q12));
  }
  *outHeightQ12 = FIXED_PRODUCT_SHR(weightedHeightAccumulator, Q12_SHIFT);
  return true;
}


/* Height of the top surface (terrain plus water above it) at a world position, interpolated linearly
   over the grid triangle that contains it; one of the five height samplers of the field-grid
   interpolation table (entry 4 of g_FieldGridInterpolationCallbacks5). Stores the Q12 height in
   *outHeightQ12; returns false (and stores height 0) outside the grid or on a border cell.
*/
Bool8 FieldGrid_InterpolateTopSurfaceHeight(Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid,Q12 *outHeightQ12)

{
  uint32_t gridHalfRowCoordinateQ12;
  uint32_t gridColumnCoordinateQ12;
  int gridColumnIndex;
  int gridRowIndex;
  FieldGridDimension gridWidth;
  FieldGridCell *cell;
  uint32_t columnFractionQ12;
  uint32_t rowFractionQ12;
  int triangleDiagonalWeightQ12;
  int64_t weightedHeightAccumulator;
  int64_t weightedWaterDeltaAccumulator;
  uint32_t terrainHeightQ12;
  int waterDeltaQ12;

  /* FieldGrid_WorldToGridQ12 inlined; gridHalfRowCoordinateQ12 is half the row coordinate */
  gridHalfRowCoordinateQ12 =
       FIXED_MUL_SHR(worldYQ12, FIELD_GRID_WORLD_Y_TO_ROW_Q20, Q20_SHIFT + 1);
  gridColumnCoordinateQ12 =
       FIXED_MUL_SHR(worldXQ12, FIELD_GRID_WORLD_X_TO_COLUMN_Q20, Q20_SHIFT) - gridHalfRowCoordinateQ12;
  gridWidth = fieldGrid->gridWidth;
  gridColumnIndex = (int)gridColumnCoordinateQ12 >> Q12_SHIFT;
  gridRowIndex = (int)(gridHalfRowCoordinateQ12 * 2) >> Q12_SHIFT;
  if (gridColumnIndex < 0 || gridRowIndex < 0 || (int)gridWidth <= gridColumnIndex ||
      (int)fieldGrid->gridHeight <= gridRowIndex) {
    *outHeightQ12 = 0;
    return false;
  }
  /* the cells around (row, column): cell[0] this cell, cell[1] the right neighbour, cell[gridWidth] the cell
     below and cell[gridWidth + 1] the one diagonally below right. */
  cell = fieldGrid->cells + gridRowIndex * gridWidth + gridColumnIndex;
  columnFractionQ12 = gridColumnCoordinateQ12 & Q12_FRACTION_MASK;
  rowFractionQ12 = gridHalfRowCoordinateQ12 * 2 & Q12_FRACTION_MASK;
  if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0 ||
      (cell[gridWidth + 1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
    *outHeightQ12 = 0;
    return false;
  }
  /* fx + fy - 1: which of the cell's two triangles, and the weight of the far vertex */
  triangleDiagonalWeightQ12 = (columnFractionQ12 + rowFractionQ12) - FIELD_GRID_CELL_Q12;
  if (columnFractionQ12 + rowFractionQ12 < FIELD_GRID_CELL_Q12) {
    weightedHeightAccumulator =
         (int64_t)cell[1].terrainHeight * (int64_t)(int)columnFractionQ12 +
         ((int64_t)cell[gridWidth].terrainHeight * (int64_t)(int)rowFractionQ12 -
          (int64_t)cell->terrainHeight * (int64_t)triangleDiagonalWeightQ12);
    weightedWaterDeltaAccumulator =
         (int64_t)cell[1].waterSurfaceDelta * (int64_t)(int)columnFractionQ12 +
         ((int64_t)cell[gridWidth].waterSurfaceDelta * (int64_t)(int)rowFractionQ12 -
          (int64_t)cell->waterSurfaceDelta * (int64_t)triangleDiagonalWeightQ12);
  }
  else {
    weightedHeightAccumulator =
         (int64_t)cell[gridWidth + 1].terrainHeight * (int64_t)triangleDiagonalWeightQ12 -
         ((int64_t)cell[gridWidth].terrainHeight * (int64_t)(int)(columnFractionQ12 - FIELD_GRID_CELL_Q12) +
          (int64_t)cell[1].terrainHeight * (int64_t)(int)(rowFractionQ12 - FIELD_GRID_CELL_Q12));
    weightedWaterDeltaAccumulator =
         (int64_t)cell[gridWidth + 1].waterSurfaceDelta * (int64_t)triangleDiagonalWeightQ12 -
         ((int64_t)cell[gridWidth].waterSurfaceDelta * (int64_t)(int)(columnFractionQ12 - FIELD_GRID_CELL_Q12) +
          (int64_t)cell[1].waterSurfaceDelta * (int64_t)(int)(rowFractionQ12 - FIELD_GRID_CELL_Q12));
  }
  terrainHeightQ12 = FIXED_PRODUCT_SHR(weightedHeightAccumulator, Q12_SHIFT);
  waterDeltaQ12 = FIXED_PRODUCT_SHR(weightedWaterDeltaAccumulator, Q12_SHIFT);
  /* water only counts where its surface lies above the ground */
  if (waterDeltaQ12 >= 0) {
    terrainHeightQ12 = terrainHeightQ12 + waterDeltaQ12;
  }
  *outHeightQ12 = terrainHeightQ12;
  return true;
}


/* The grid triangle under a world position, shared by the interpolating lookups below. The grid square of
   `cell` is split along the diagonal from its right neighbour (cell[1]) to its lower neighbour
   (cell[rowLength]): columnFraction + rowFraction < 1 is the triangle (cell, right, lower), otherwise
   (lower-right, lower, right). */
typedef struct FieldGridTriangleLookup {
  FieldGridCell *cell; /* top-left cell of the grid square */
  FieldGridDimension rowLength;
  uint32_t columnFractionQ12;
  /* Also set when the lookup fails: the unmasked Q12 row coordinate outside the grid, the masked row fraction
     on a border cell. */
  uint32_t rowFractionQ12;
  int diagonalWeightQ12; /* columnFraction + rowFraction - 1 */
} FieldGridTriangleLookup;

/* Finds the grid square under (worldYQ12, worldXQ12) and the fractions inside it. Returns false outside the
   grid or when the square's top-left or lower-right corner is a border cell. */
static Bool8 FieldGrid_LocateInterpolationTriangle
          (Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid,FieldGridTriangleLookup *lookup)
{
  uint32_t rowQ12;
  uint32_t columnQ12;
  int row;
  int column;
  FieldGridDimension rowLength;
  FieldGridCell *cell;

  rowQ12 = FIXED_MUL_SHR(worldYQ12, FIELD_GRID_WORLD_Y_TO_ROW_Q20, Q20_SHIFT + 1);
  columnQ12 = (FIXED_MUL_SHR(worldXQ12, FIELD_GRID_WORLD_X_TO_COLUMN_Q20, Q20_SHIFT)) - rowQ12;
  rowQ12 = rowQ12 * 2;
  rowLength = fieldGrid->gridWidth;
  lookup->rowLength = rowLength;
  lookup->rowFractionQ12 = rowQ12;
  column = (int)columnQ12 >> Q12_SHIFT;
  if (column < 0) {
    return false;
  }
  row = (int)rowQ12 >> Q12_SHIFT;
  if (row < 0 || (int)rowLength <= column || (int)fieldGrid->gridHeight <= row) {
    return false;
  }
  cell = fieldGrid->cells + row * rowLength + column;
  lookup->columnFractionQ12 = columnQ12 & Q12_FRACTION_MASK;
  lookup->rowFractionQ12 = rowQ12 & Q12_FRACTION_MASK;
  if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0 ||
      (cell[rowLength + 1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
    return false;
  }
  lookup->cell = cell;
  lookup->diagonalWeightQ12 = (lookup->columnFractionQ12 + lookup->rowFractionQ12) - FIELD_GRID_CELL_Q12;
  return true;
}

/* Barycentric Q12 interpolation of one per-vertex value over the triangle of `lookup`: the values of the cell,
   its right, lower and lower-right neighbours (only the three of the triangle are used). */
static Q12 FieldGrid_InterpolateTriangleValue
          (const FieldGridTriangleLookup *lookup,int cellValue,int rightValue,int lowerValue,int lowerRightValue)
{
  int64_t weightedSum;

  if (lookup->columnFractionQ12 + lookup->rowFractionQ12 < FIELD_GRID_CELL_Q12) {
    weightedSum = (int64_t)rightValue * (int64_t)(int)lookup->columnFractionQ12 +
                  ((int64_t)lowerValue * (int64_t)(int)lookup->rowFractionQ12 -
                   (int64_t)cellValue * (int64_t)lookup->diagonalWeightQ12);
  }
  else {
    weightedSum = (int64_t)lowerRightValue * (int64_t)lookup->diagonalWeightQ12 -
                  ((int64_t)lowerValue * (int64_t)(int)(lookup->columnFractionQ12 - FIELD_GRID_CELL_Q12) +
                   (int64_t)rightValue * (int64_t)(int)(lookup->rowFractionQ12 - FIELD_GRID_CELL_Q12));
  }
  return FIXED_PRODUCT_SHR(weightedSum, Q12_SHIFT);
}

/* The unit normal stored as packed angles (elevation << 16 | azimuth), scaled by weightQ12. */
static FixedDirection FieldGrid_ScalePackedNormal(uint32_t packedNormalAngles,int weightQ12)
{
  return FixedMath_DirectionFromAnglesScaled
                   ((int)packedNormalAngles >> 16,packedNormalAngles & FIXED_ANGLE16_MASK,weightQ12);
}

/* Blends the packed vertex normals of the triangle of `lookup` with the interpolation weights (each normal
   scaled by FixedMath_DirectionFromAnglesScaled, summed and turned back into packed angles). Arguments as in
   FieldGrid_InterpolateTriangleValue. */
static uint32_t FieldGrid_BlendTriangleNormals
          (const FieldGridTriangleLookup *lookup,uint32_t cellAngles,uint32_t rightAngles,uint32_t lowerAngles,
          uint32_t lowerRightAngles)
{
  FixedDirection firstNormal;
  FixedDirection scaledNormal;
  int normalSumX;
  int normalSumY;
  int normalSumZ;
  FixedVectorAngles blendedNormalAngles;

  if (lookup->columnFractionQ12 + lookup->rowFractionQ12 < FIELD_GRID_CELL_Q12) {
    firstNormal = FieldGrid_ScalePackedNormal(rightAngles,lookup->columnFractionQ12);
    scaledNormal = FieldGrid_ScalePackedNormal(cellAngles,lookup->diagonalWeightQ12);
    normalSumX = firstNormal.x - scaledNormal.x;
    normalSumY = firstNormal.y - scaledNormal.y;
    normalSumZ = firstNormal.z - scaledNormal.z;
    scaledNormal = FieldGrid_ScalePackedNormal(lowerAngles,lookup->rowFractionQ12);
    blendedNormalAngles = FixedMath_VectorToAngles
                       (normalSumZ + scaledNormal.z,normalSumY + scaledNormal.y,normalSumX + scaledNormal.x);
  }
  else {
    firstNormal = FieldGrid_ScalePackedNormal(lowerRightAngles,lookup->diagonalWeightQ12);
    scaledNormal = FieldGrid_ScalePackedNormal(lowerAngles,lookup->columnFractionQ12 - FIELD_GRID_CELL_Q12);
    normalSumX = firstNormal.x - scaledNormal.x;
    normalSumY = firstNormal.y - scaledNormal.y;
    normalSumZ = firstNormal.z - scaledNormal.z;
    scaledNormal = FieldGrid_ScalePackedNormal(rightAngles,lookup->rowFractionQ12 - FIELD_GRID_CELL_Q12);
    blendedNormalAngles = FixedMath_VectorToAngles
                       (normalSumZ - scaledNormal.z,normalSumY - scaledNormal.y,normalSumX - scaledNormal.x);
  }
  return blendedNormalAngles.elevationAngle << 16 | blendedNormalAngles.azimuthAngle & FIXED_ANGLE16_MASK;
}

/* Terrain height and terrain normal at a world position: interpolates terrainHeight over the grid triangle
   that contains it (Q12, *outHeightQ12) and blends the three vertex normals (triangle0NormalAngles) with
   the same barycentric weights, stored as packed angles elevation << 16 | azimuth (*outPackedNormalAngles).
   Returns false (outputs untouched) outside the grid or on a border cell. Used by the articulated army contact
   code (ArmyArticulatedRuntime_UpdateLeftTerrainContact, ..RightTerrainContact and their siblings).
*/
Bool8 FieldGrid_InterpolateTerrainHeightAndNormal
          (Q12 worldY,Q12 worldX,FieldGridAsset *field,Q12 *outHeightQ12,uint32_t *outPackedNormalAngles)

{
  FieldGridTriangleLookup lookup;
  FieldGridCell *cell;
  FieldGridDimension rowLength;
  Q12 heightQ12;

  if (!FieldGrid_LocateInterpolationTriangle(worldY,worldX,field,&lookup)) {
    return false;
  }
  cell = lookup.cell;
  rowLength = lookup.rowLength;
  /* height as in FieldGrid_InterpolateTerrainHeight */
  heightQ12 = FieldGrid_InterpolateTriangleValue
                (&lookup,cell->terrainHeight,cell[1].terrainHeight,cell[rowLength].terrainHeight,
                 cell[rowLength + 1].terrainHeight);
  *outPackedNormalAngles = FieldGrid_BlendTriangleNormals
                (&lookup,cell->triangle0NormalAngles,cell[1].triangle0NormalAngles,
                 cell[rowLength].triangle0NormalAngles,cell[rowLength + 1].triangle0NormalAngles);
  *outHeightQ12 = heightQ12;
  return true;
}


/* Placement test at a world position: returns false when the nearest grid cell's occupancy byte of
   faction slot factionSlot has any FIELD_CELL_OCCUPANCY_PRESENCE_BITS set, true ("blocked") when the
   faction is not present there or the point is outside the grid. Called by
   ArmyPlacement_TestGridRuntimeAndFieldBlocking, ArmyPlacementCollision_TestCurrentRuntime
   and ..TestCandidateAndClearance with the owner army's faction index.
*/
Bool8 FieldGrid_TestWorldPointBlocked
          (FieldGridByteOffset factionSlot,Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid
          )

{
  int gridColumnIndex;
  uint32_t gridHalfRowCoordinateQ12;
  int gridRowIndex;
  uint8_t occupancyByte;

  /* FieldGrid_WorldToGridQ12 inlined, then rounded (+0x800 = half a cell) to whole cells */
  gridHalfRowCoordinateQ12 =
       FIXED_MUL_SHR(worldYQ12, FIELD_GRID_WORLD_Y_TO_ROW_Q20, Q20_SHIFT + 1);
  gridColumnIndex =
       (int)((FIXED_MUL_SHR(worldXQ12, FIELD_GRID_WORLD_X_TO_COLUMN_Q20, Q20_SHIFT) - gridHalfRowCoordinateQ12) + FIELD_GRID_CELL_Q12 / 2)
       >> Q12_SHIFT;
  gridRowIndex = (int)(gridHalfRowCoordinateQ12 * 2 + FIELD_GRID_CELL_Q12 / 2) >> Q12_SHIFT;
  if ((gridColumnIndex < 0) || (gridRowIndex < 0) || ((int)fieldGrid->gridWidth <= gridColumnIndex) ||
      ((int)fieldGrid->gridHeight <= gridRowIndex)) {
    return true;
  }
  occupancyByte =
       ((uint8_t *)&fieldGrid->cells[fieldGrid->gridWidth * gridRowIndex + gridColumnIndex].occupancyMask)[factionSlot];
  return (occupancyByte & FIELD_CELL_OCCUPANCY_PRESENCE_BITS) == 0;
}


/* Load-time cell setup: marks the field surface dirty and gives every cell a random animation phase (masked to
   the bit width stored just before the terrain surface packet table), a terrain direction record chosen by the
   low nibbles of its world X/Y, a white overlay colour and random material variant bits 8-10. Then it rebuilds
   the four map-edge flags on the outermost ring of cells, which neighbour loops test before stepping outside.
*/
void FieldGrid_InitializeRuntimeCellsAndBoundaryFlags(FieldGridAsset *fieldGrid)

{
  FieldGridDimension cellsPerRow;
  uint32_t phaseRandomValue;
  uint32_t materialVariantRandomBits;
  FieldGridDimension columnsRemaining;
  FieldGridDimension rowsRemaining;
  FieldGridDimension gridWidth;
  FieldGridCell *cell;
  FieldGridCell *rowFirstCell;
  Q12 cellWorldXQ12;
  Q12 cellWorldYQ12;
  uint32_t phaseSeedBitWidth;

  phaseSeedBitWidth = *(uint32_t *)((uint8_t *)g_TerrainSurfacePacketTablePayload - TERRAIN_PACKET_TABLE_HEADER_BYTES);
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
  cellsPerRow = fieldGrid->gridWidth;
  cell = fieldGrid->cells;
  for (rowsRemaining = fieldGrid->gridHeight; rowsRemaining != 0; rowsRemaining--) {
    for (columnsRemaining = cellsPerRow; columnsRemaining != 0; columnsRemaining--) {
      cellWorldXQ12 = cell->worldX;
      cellWorldYQ12 = cell->worldY;
      /* the edge flags are rebuilt from scratch, the debug mark starts cleared */
      cell->flagsAndMaterial =
           cell->flagsAndMaterial & ~(FIELD_CELL_GRID_EDGE_MASK | FIELD_CELL_DEBUG_MARKED);
      phaseRandomValue = Random_NextPrimary();
      cell->flagsAndMaterial = cell->flagsAndMaterial & ~FIELD_CELL_RANDOM_VARIANT_MASK;
      cell->surfacePacketIndex = phaseRandomValue & (1 << ((uint8_t)phaseSeedBitWidth & 31)) - 1U;
      /* 16x16 tiling of the 256 direction records over the world */
      cell->persistedAux54 =
           (FieldCellPersistedAux)
           (g_TerrainDirectionRecordTable256 + (cellWorldYQ12 & 0xfU) + (cellWorldXQ12 & 0xfU) * 16);
      cell->armyRuntimeSavedOffset = 0;
      materialVariantRandomBits = Random_NextPrimary();
      cell->overlayColor = 0xffffffff; /* ARGB opaque white */
      cell->flagsAndMaterial =
           cell->flagsAndMaterial | materialVariantRandomBits & FIELD_CELL_RANDOM_VARIANT_MASK;
      cell++;
    }
  }
  gridWidth = fieldGrid->gridWidth;
  cell = fieldGrid->cells;
  for (columnsRemaining = gridWidth; columnsRemaining != 0; columnsRemaining--) {
    cell->flagsAndMaterial = cell->flagsAndMaterial | FIELD_CELL_FIRST_ROW_BOUNDARY;
    cell++;
  }
  rowFirstCell = fieldGrid->cells;
  for (rowsRemaining = fieldGrid->gridHeight; rowsRemaining != 0; rowsRemaining--) {
    rowFirstCell->flagsAndMaterial = rowFirstCell->flagsAndMaterial | FIELD_CELL_FIRST_COLUMN_BOUNDARY;
    rowFirstCell[gridWidth - 1].flagsAndMaterial =
         rowFirstCell[gridWidth - 1].flagsAndMaterial | FIELD_CELL_LAST_COLUMN_BOUNDARY;
    rowFirstCell = rowFirstCell + gridWidth;
  }
  /* the row cursor ran one row past the grid: step back to the first cell of the last row */
  cell = rowFirstCell - gridWidth;
  for (columnsRemaining = gridWidth; columnsRemaining != 0; columnsRemaining--) {
    cell->flagsAndMaterial = cell->flagsAndMaterial | FIELD_CELL_LAST_ROW_BOUNDARY;
    cell++;
  }
}


/* Marks the field surface dirty and re-binds every cell's terrain direction record (persistedAux54) from the low nibbles
   of its world X/Y, the same 16x16 tiling FieldGrid_InitializeRuntimeCellsAndBoundaryFlags uses. The secondary
   terrain resource load calls this instead of the full initialization, so the other cell state is kept.
*/
void FieldGrid_RebuildCellLookupPointers(FieldGridAsset *fieldGrid)

{
  FieldGridDimension columnsRemaining;
  FieldGridDimension rowsRemaining;
  FieldGridCell *currentCell;
  FieldGridDimension gridWidth;

  rowsRemaining = fieldGrid->gridHeight;
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
  gridWidth = fieldGrid->gridWidth;
  currentCell = fieldGrid->cells;
  columnsRemaining = gridWidth;
  do {
    do {
      currentCell->persistedAux54 =
           (FieldCellPersistedAux)
           (g_TerrainDirectionRecordTable256 +
           (currentCell->worldY & 0xfU) + (currentCell->worldX & 0xfU) * 16);
      currentCell++;
      columnsRemaining--;
    } while (columnsRemaining != 0);
    rowsRemaining--;
    columnsRemaining = gridWidth;
  } while (rowsRemaining != 0);
}


/* For every field cell, maps the fog-of-war lighting index (visibilityLightingIndex) through the
   256x256 terrain clamp lookup, keyed by the cell's occupancy byte of the given faction, and writes the result
   back. Runs on tick-wheel cases 3 and 7 after the per-class terrain-state refresh callbacks.
*/
void FieldGrid_ApplyByteClampLookupToCells(FieldGridByteOffset factionIndex,FieldGridAsset *fieldGrid)

{
  uint8_t *clampLookup;
  FieldGridDimension columnsRemaining;
  FieldGridDimension rowsRemaining;
  FieldGridCell *currentCell;
  uint8_t mappedLightingIndex;
  FieldGridDimension gridWidth;

  gridWidth = fieldGrid->gridWidth;
  rowsRemaining = fieldGrid->gridHeight;
  currentCell = fieldGrid->cells;
  clampLookup = g_TerrainByteClampLookup;
  columnsRemaining = gridWidth;
  do {
    do {
      /* The lookup is 64-KiB aligned: the original puts the occupancy byte and the lighting index into the
         pointer's low word, i.e. indexes the table with (occupancy << 8) | lighting index. */
      mappedLightingIndex =
           clampLookup[(uint32_t)((uint8_t *)&currentCell->occupancyMask)[factionIndex] << 8 |
                       (uint32_t)currentCell->visibilityLightingIndex];
      currentCell->visibilityLightingIndex = mappedLightingIndex;
      currentCell = currentCell + 1;
      columnsRemaining = columnsRemaining - 1;
    } while (columnsRemaining != 0);
    rowsRemaining = rowsRemaining - 1;
    columnsRemaining = gridWidth;
  } while (rowsRemaining != 0);
  return;
}


/* Rebuilds every cell's fog-of-war lighting index (visibilityLightingIndex) from the occupancy byte of
   one faction slot (usually the active faction): FIELD_CELL_LIGHTING_VISIBLE when a current presence bit
   (FIELD_CELL_OCCUPANCY_CURRENT_PRESENCE_BITS) is set, FIELD_CELL_LIGHTING_EXPLORED when only the persistent
   bit 7 is, FIELD_CELL_LIGHTING_UNEXPLORED otherwise.
*/
void FieldGrid_ClassifyCellFlagsToRuntimeByte(FieldGridByteOffset factionSlot,FieldGridAsset *fieldGrid)

{
  uint8_t lightingIndex;
  uint8_t occupancyByte;
  int cellsRemaining;
  FieldGridCell *currentCell;

  cellsRemaining = fieldGrid->gridWidth * fieldGrid->gridHeight;
  currentCell = fieldGrid->cells;
  do {
    occupancyByte = ((uint8_t *)&currentCell->occupancyMask)[factionSlot];
    if ((occupancyByte & FIELD_CELL_OCCUPANCY_CURRENT_PRESENCE_BITS) != 0) {
      lightingIndex = FIELD_CELL_LIGHTING_VISIBLE;
    }
    else if ((occupancyByte & FIELD_CELL_OCCUPANCY_PERSISTENT_BIT) != 0) {
      lightingIndex = FIELD_CELL_LIGHTING_EXPLORED;
    }
    else {
      lightingIndex = FIELD_CELL_LIGHTING_UNEXPLORED;
    }
    currentCell->visibilityLightingIndex = lightingIndex;
    currentCell++;
    cellsRemaining--;
  } while (cellsRemaining != 0);
}


/* Animates the 256 terrain direction records (random rates and scales from TerrainVisualResources_LoadPrimary):
   both 16-bit angles advance by their rates, and the scaled sine/cosine of angle A and one component of
   angle B are stored, computed from the angles before this step.
*/
void TerrainDirectionTable_AdvanceAndRebuildVectors(void)

{
  uint32_t previousPackedAngles;
  TerrainDirectionRecordCount recordsRemaining;
  TerrainDirectionRecord *currentDirectionRecord;
  FixedSinCos scaledSinCosPair;
  FixedSinCos angleBScaledSinCosPair;

  currentDirectionRecord = g_TerrainDirectionRecordTable256;
  recordsRemaining = 256;
  do {
    previousPackedAngles = currentDirectionRecord->packedAngles;
    /* one 32-bit add advances both packed angles by rateA (low word) and rateB (high word); a carry out
       of angle A moves angle B by one more */
    currentDirectionRecord->packedAngles = currentDirectionRecord->packedAngles + *(int *)&currentDirectionRecord->rateA;
    scaledSinCosPair = FixedMath_SinCosScaled(previousPackedAngles & FIXED_ANGLE16_MASK,currentDirectionRecord->scaleA);
    currentDirectionRecord->angleAComponent0ScaledQ28 = scaledSinCosPair.cosValue;
    currentDirectionRecord->angleAComponent1ScaledQ28 = scaledSinCosPair.sinValue;
    angleBScaledSinCosPair =
         FixedMath_SinCosScaled((int)previousPackedAngles >> 16,currentDirectionRecord->scaleB);
    currentDirectionRecord->angleBComponent0ScaledQ28 = (uint32_t)angleBScaledSinCosPair.cosValue;
    currentDirectionRecord++;
    recordsRemaining--;
  } while (recordsRemaining != 0);
}


/* Casts a ray from a world point along (elevation, azimuth) scaled to rayScaleQ12 and walks the field grid cell by
   cell towards its end point, testing the two terrain triangles of each in-bounds cell. On a hit it returns true
   and stores the distance and the cell's material byte; a miss (or more than FIELD_GRID_RAYCAST_MAX_STEPS cells)
   returns false and stores FIELD_GRID_RAYCAST_MISS_DISTANCE as the distance (both outputs are always written;
   outMaterialIndex may be NULL). Used for line-of-fire tests and terrain picking.
*/
Bool8 FieldGrid_RaycastTerrainSurfaceDistance
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,Q12 rayScaleQ12,Q12 rayOriginZQ12,
          Q12 rayOriginYQ12,Q12 rayOriginXQ12,FieldGridAsset *fieldGrid,Q12 *outDistanceQ12,
          uint32_t *outMaterialIndex)

{
  FieldGridCell *currentCell;
  FieldGridDimension gridWidth;
  FieldGridDimension gridHeight;
  FieldGridDimension rowLength;
  int64_t rayEndColumnProduct;
  int64_t rayEndRowProduct;
  uint32_t rayStartRowQ12;
  int rayEndRowQ12;
  uint32_t rayStartColumnQ12;
  int rayEndColumnQ12;
  uint32_t rayStartHalfRowQ12;
  uint32_t rayEndHalfRowQ12;
  uint32_t currentColumnQ12;
  uint32_t currentRowQ12;
  int stepsRemaining;
  Bool8 traversalDone;
  FixedDirection rayDirection;

  gridWidth = fieldGrid->gridWidth;
  gridHeight = fieldGrid->gridHeight;
  /* FieldGrid_WorldToGridQ12 inlined for the ray start */
  rayStartHalfRowQ12 = FIXED_MUL_SHR(rayOriginYQ12, FIELD_GRID_WORLD_Y_TO_ROW_Q20, Q20_SHIFT + 1);
  rayStartColumnQ12 =
       (FIXED_MUL_SHR(rayOriginXQ12, FIELD_GRID_WORLD_X_TO_COLUMN_Q20, Q20_SHIFT)) - rayStartHalfRowQ12;
  rayStartRowQ12 = rayStartHalfRowQ12 * 2;
  rowLength = fieldGrid->gridWidth;
  /* &cells[row * rowLength + column], written as the original's byte arithmetic */
  currentCell = (FieldGridCell *)((uint8_t *)&fieldGrid->cells[(int)rayStartColumnQ12 >> Q12_SHIFT] + ((int)rayStartRowQ12 >> Q12_SHIFT) * rowLength * sizeof(FieldGridCell));
  rayDirection = FixedMath_DirectionFromAnglesScaled(elevationAngle,azimuthAngle,rayScaleQ12);
  /* ...and for the ray end */
  rayEndColumnProduct = (int64_t)(int)(rayDirection.x + rayOriginXQ12) * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
  rayEndRowProduct = (int64_t)(int)(rayDirection.y + rayOriginYQ12) * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
  rayEndHalfRowQ12 = FIXED_PRODUCT_SHR(rayEndRowProduct, Q20_SHIFT + 1);
  rayEndColumnQ12 = (FIXED_PRODUCT_SHR(rayEndColumnProduct, Q20_SHIFT)) - rayEndHalfRowQ12;
  rayEndRowQ12 = rayEndHalfRowQ12 * 2;
  currentColumnQ12 = rayStartColumnQ12 & ~(FIELD_GRID_CELL_Q12 - 1);
  currentRowQ12 = rayStartRowQ12 & ~(FIELD_GRID_CELL_Q12 - 1);
  for (stepsRemaining = FIELD_GRID_RAYCAST_MAX_STEPS - 1; stepsRemaining != 0; stepsRemaining--) {
    /* the cell and its right/lower neighbours must lie inside the grid */
    if ((-1 < (int)currentColumnQ12) && (-1 < (int)currentRowQ12) &&
        ((int)currentColumnQ12 < (int)((gridWidth - 1) * FIELD_GRID_CELL_Q12)) &&
        ((int)currentRowQ12 < (int)((gridHeight - 1) * FIELD_GRID_CELL_Q12))) {
      if (TerrainTriangle_IntersectRayDistance
                         (rayDirection.z,rayEndRowQ12 + rayStartHalfRowQ12 * -2,
                          rayEndColumnQ12 - rayStartColumnQ12,rayOriginZQ12,
                          currentCell[rowLength + 1].terrainHeight,currentCell[rowLength].terrainHeight,
                          currentCell[1].terrainHeight,currentCell->terrainHeight,
                          currentRowQ12 + rayStartHalfRowQ12 * -2,currentColumnQ12 - rayStartColumnQ12,
                          outDistanceQ12)) {
        if (outMaterialIndex != NULL) {
          *outMaterialIndex = currentCell->flagsAndMaterial & FIELD_CELL_MATERIAL_ID_MASK;
        }
        return true;
      }
    }
    traversalDone = TerrainRay_AdvanceGridTraversal
                       (rayEndRowQ12,rayEndColumnQ12,rayStartRowQ12,rayStartColumnQ12,
                        rowLength * sizeof(FieldGridCell),currentCell,currentRowQ12,currentColumnQ12);
    /* results of the step */
    currentCell = g_TerrainRayNextCell;
    currentColumnQ12 = g_TerrainRayNextCoord1Q12;
    currentRowQ12 = g_TerrainRayNextCoord0Q12;
    if (traversalDone) {
      break;
    }
  }
  /* Original quirk: a miss leaves the material output as the current row coordinate (or end row - current row
     when the traversal reached the ray end). Callers that copy the material unconditionally get this value, but
     none uses it on a miss (shots index their impact table only with a terrain distance within range). */
  if (outMaterialIndex != NULL) {
    *outMaterialIndex = currentRowQ12;
  }
  *outDistanceQ12 = FIELD_GRID_RAYCAST_MISS_DISTANCE;
  return false;
}


/* Same walk as FieldGrid_RaycastTerrainSurfaceDistance, but against the secondary (water) surface: each triangle
   corner is terrainHeight + waterSurfaceDelta. Returns true with the hit distance in *outDistanceQ12, or false with
   FIELD_GRID_RAYCAST_MISS_DISTANCE there on a miss (the original also returned the hit cell's material byte as a
   second result; no caller reads it).
*/
Bool8 FieldGrid_RaycastSecondarySurfaceDistance
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,Q12 rayScaleQ12,Q12 rayOriginZQ12,
          Q12 rayOriginYQ12,Q12 rayOriginXQ12,FieldGridAsset *fieldGrid,Q12 *outDistanceQ12)

{
  FieldGridCell *currentCell;
  FieldGridDimension gridWidth;
  FieldGridDimension gridHeight;
  FieldGridDimension rowLength;
  int64_t rayEndColumnProduct;
  int64_t rayEndRowProduct;
  uint32_t rayStartRowQ12;
  int rayEndRowQ12;
  uint32_t rayStartColumnQ12;
  int rayEndColumnQ12;
  uint32_t rayStartHalfRowQ12;
  uint32_t rayEndHalfRowQ12;
  uint32_t currentColumnQ12;
  uint32_t currentRowQ12;
  int stepsRemaining;
  Bool8 traversalDone;
  FixedDirection rayDirection;

  gridWidth = fieldGrid->gridWidth;
  gridHeight = fieldGrid->gridHeight;
  /* FieldGrid_WorldToGridQ12 inlined for the ray start */
  rayStartHalfRowQ12 = FIXED_MUL_SHR(rayOriginYQ12, FIELD_GRID_WORLD_Y_TO_ROW_Q20, Q20_SHIFT + 1);
  rayStartColumnQ12 =
       (FIXED_MUL_SHR(rayOriginXQ12, FIELD_GRID_WORLD_X_TO_COLUMN_Q20, Q20_SHIFT)) - rayStartHalfRowQ12;
  rayStartRowQ12 = rayStartHalfRowQ12 * 2;
  rowLength = fieldGrid->gridWidth;
  /* &cells[row * rowLength + column], written as the original's byte arithmetic */
  currentCell = (FieldGridCell *)((uint8_t *)&fieldGrid->cells[(int)rayStartColumnQ12 >> Q12_SHIFT] + ((int)rayStartRowQ12 >> Q12_SHIFT) * rowLength * sizeof(FieldGridCell));
  rayDirection = FixedMath_DirectionFromAnglesScaled(elevationAngle,azimuthAngle,rayScaleQ12);
  /* ...and for the ray end */
  rayEndColumnProduct = (int64_t)(int)(rayDirection.x + rayOriginXQ12) * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
  rayEndRowProduct = (int64_t)(int)(rayDirection.y + rayOriginYQ12) * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
  rayEndHalfRowQ12 = FIXED_PRODUCT_SHR(rayEndRowProduct, Q20_SHIFT + 1);
  rayEndColumnQ12 = (FIXED_PRODUCT_SHR(rayEndColumnProduct, Q20_SHIFT)) - rayEndHalfRowQ12;
  rayEndRowQ12 = rayEndHalfRowQ12 * 2;
  currentColumnQ12 = rayStartColumnQ12 & ~(FIELD_GRID_CELL_Q12 - 1);
  currentRowQ12 = rayStartRowQ12 & ~(FIELD_GRID_CELL_Q12 - 1);
  for (stepsRemaining = FIELD_GRID_RAYCAST_MAX_STEPS - 1; stepsRemaining != 0; stepsRemaining--) {
    /* the cell and its right/lower neighbours must lie inside the grid */
    if ((-1 < (int)currentColumnQ12) && (-1 < (int)currentRowQ12) &&
        ((int)currentColumnQ12 < (int)((gridWidth - 1) * FIELD_GRID_CELL_Q12)) &&
        ((int)currentRowQ12 < (int)((gridHeight - 1) * FIELD_GRID_CELL_Q12))) {
      if (TerrainTriangle_IntersectRayDistance
                         (rayDirection.z,rayEndRowQ12 + rayStartHalfRowQ12 * -2,
                          rayEndColumnQ12 - rayStartColumnQ12,rayOriginZQ12,
                          currentCell[rowLength + 1].terrainHeight +
                          currentCell[rowLength + 1].waterSurfaceDelta,
                          currentCell[rowLength].terrainHeight + currentCell[rowLength].waterSurfaceDelta,
                          currentCell[1].terrainHeight + currentCell[1].waterSurfaceDelta,
                          currentCell->waterSurfaceDelta + currentCell->terrainHeight,
                          currentRowQ12 + rayStartHalfRowQ12 * -2,currentColumnQ12 - rayStartColumnQ12,
                          outDistanceQ12)) {
        return true;
      }
    }
    traversalDone = TerrainRay_AdvanceGridTraversal
                       (rayEndRowQ12,rayEndColumnQ12,rayStartRowQ12,rayStartColumnQ12,
                        rowLength * sizeof(FieldGridCell),currentCell,currentRowQ12,currentColumnQ12);
    /* results of the step */
    currentCell = g_TerrainRayNextCell;
    currentColumnQ12 = g_TerrainRayNextCoord1Q12;
    currentRowQ12 = g_TerrainRayNextCoord0Q12;
    if (traversalDone) {
      break;
    }
  }
  *outDistanceQ12 = FIELD_GRID_RAYCAST_MISS_DISTANCE;
  return false;
}


/* Casts a ray of length rayScaleQ12 from a world point in the direction (elevation, azimuth) over the terrain
   triangles and returns true on the first hit, with its distance in *outDistanceQ12; false (output untouched)
   when the ray ends first or after FIELD_GRID_RAYCAST_MAX_STEPS - 1 cells. The original also returned the hit
   cell's material byte (or the traversal's last row coordinate on a miss) as a second result; no caller reads it. Cells
   outside the grid are clamped to the border. Used by GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy
   along the render context's view angles from a model's sample points, to find terrain between
   them and the viewer.
*/
Bool8 FieldGrid_RaycastTerrainTrianglesAlongDirection
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,FixedMathScale32 rayScaleQ12,
          Q12 rayOriginZQ12,Q12 rayOriginYQ12,Q12 rayOriginXQ12,FieldGridAsset *fieldGrid,
          Q12 *outDistanceQ12)

{
  FieldGridDimension rowLength;
  int64_t rayEndColumnProduct;
  int64_t rayEndHalfRowProduct;
  uint32_t rayStartRowQ12;
  uint32_t rayEndHalfRowQ12;
  int rayEndRowQ12;
  uint32_t rayStartColumnQ12;
  int rayEndColumnQ12;
  int cellColumnIndex;
  int rowClampOffset;
  int maxRowIndex;
  uint32_t rayStartHalfRowQ12;
  uint32_t currentColumnQ12;
  uint32_t currentRowQ12;
  int rowFromStartQ12;
  int cellRowIndex;
  int maxColumnIndex;
  int stepsRemaining;
  FieldGridCell *currentCell;
  FieldGridCell *sampleCell;
  int rowStrideBytes;
  Bool8 traversalDone;
  FixedDirection rayDirection;
  FieldCellPersistedAux cornerHeight0Q12;
  FieldCellPersistedAux cornerHeight1Q12;
  FieldCellPersistedAux cornerHeight2Q12;
  FieldCellPersistedAux cornerHeight3Q12;

  maxColumnIndex = fieldGrid->gridWidth - 1;
  maxRowIndex = fieldGrid->gridHeight - 1;
  rayStartHalfRowQ12 = FIXED_MUL_SHR(rayOriginYQ12, FIELD_GRID_WORLD_Y_TO_ROW_Q20, Q20_SHIFT + 1);
  rayStartColumnQ12 =
       (FIXED_MUL_SHR(rayOriginXQ12, FIELD_GRID_WORLD_X_TO_COLUMN_Q20, Q20_SHIFT)) - rayStartHalfRowQ12;
  rayStartRowQ12 = rayStartHalfRowQ12 * 2;
  rowLength = fieldGrid->gridWidth;
  rowStrideBytes = rowLength * sizeof(FieldGridCell);
  rayDirection = FixedMath_DirectionFromAnglesScaled(elevationAngle,azimuthAngle,rayScaleQ12);
  rayEndColumnProduct = (int64_t)(int)(rayDirection.x + rayOriginXQ12) * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
  rayEndHalfRowProduct = (int64_t)(int)(rayDirection.y + rayOriginYQ12) * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
  rayEndHalfRowQ12 = FIXED_PRODUCT_SHR(rayEndHalfRowProduct, Q20_SHIFT + 1);
  rayEndColumnQ12 = (FIXED_PRODUCT_SHR(rayEndColumnProduct, Q20_SHIFT)) - rayEndHalfRowQ12;
  rayEndRowQ12 = rayEndHalfRowQ12 * 2;
  currentColumnQ12 = rayStartColumnQ12 & ~(FIELD_GRID_CELL_Q12 - 1u);
  currentRowQ12 = rayStartRowQ12 & ~(FIELD_GRID_CELL_Q12 - 1u);
  currentCell = (FieldGridCell *)((uint8_t *)&fieldGrid->cells[(int)rayStartColumnQ12 >> Q12_SHIFT] + ((int)rayStartRowQ12 >> Q12_SHIFT) * rowStrideBytes);
  for (stepsRemaining = FIELD_GRID_RAYCAST_MAX_STEPS - 1; stepsRemaining != 0; stepsRemaining--) {
    rowFromStartQ12 = currentRowQ12 + rayStartHalfRowQ12 * -2;
    cellColumnIndex = (int)currentColumnQ12 >> Q12_SHIFT;
    cellRowIndex = (int)currentRowQ12 >> Q12_SHIFT;
    if (cellColumnIndex < 0) {
      sampleCell = currentCell + -cellColumnIndex;
      if (cellRowIndex < 0 || maxRowIndex <= cellRowIndex) {
        /* both clamped: the single corner cell */
        if (cellRowIndex < 0) {
          rowClampOffset = cellRowIndex;
        }
        else {
          rowClampOffset = cellRowIndex - maxRowIndex;
        }
        sampleCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(sampleCell,-rowClampOffset * rowStrideBytes);
        cornerHeight3Q12 = sampleCell->terrainHeight;
        cornerHeight2Q12 = sampleCell->terrainHeight;
        cornerHeight1Q12 = sampleCell->terrainHeight;
        cornerHeight0Q12 = sampleCell->terrainHeight;
      }
      else {
        cornerHeight3Q12 = sampleCell->terrainHeight;
        cornerHeight2Q12 = sampleCell[rowLength].terrainHeight;
        cornerHeight1Q12 = sampleCell->terrainHeight;
        cornerHeight0Q12 = sampleCell[rowLength].terrainHeight;
      }
    }
    else if (cellRowIndex < 0) {
      sampleCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(currentCell,-cellRowIndex * rowStrideBytes);
      if (cellColumnIndex < maxColumnIndex) {
        cornerHeight3Q12 = sampleCell->terrainHeight;
        cornerHeight2Q12 = sampleCell[1].terrainHeight;
        cornerHeight1Q12 = sampleCell->terrainHeight;
        cornerHeight0Q12 = sampleCell[1].terrainHeight;
      }
      else {
        sampleCell = sampleCell + -(cellColumnIndex - maxColumnIndex);
        cornerHeight3Q12 = sampleCell->terrainHeight;
        cornerHeight2Q12 = sampleCell->terrainHeight;
        cornerHeight1Q12 = sampleCell->terrainHeight;
        cornerHeight0Q12 = sampleCell->terrainHeight;
      }
    }
    else if (cellColumnIndex < maxColumnIndex) {
      if (cellRowIndex < maxRowIndex) {
        cornerHeight3Q12 = currentCell->terrainHeight;
        cornerHeight2Q12 = currentCell[1].terrainHeight;
        cornerHeight1Q12 = currentCell[rowLength].terrainHeight;
        cornerHeight0Q12 = currentCell[rowLength + 1].terrainHeight;
        sampleCell = currentCell;
      }
      else {
        sampleCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(currentCell,-(cellRowIndex - maxRowIndex) * rowStrideBytes);
        cornerHeight3Q12 = sampleCell->terrainHeight;
        cornerHeight2Q12 = sampleCell[1].terrainHeight;
        cornerHeight1Q12 = sampleCell->terrainHeight;
        cornerHeight0Q12 = sampleCell[1].terrainHeight;
      }
    }
    else {
      sampleCell = currentCell + -(cellColumnIndex - maxColumnIndex);
      if (maxRowIndex <= cellRowIndex) {
        /* both clamped: the single corner cell */
        sampleCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(sampleCell,-(cellRowIndex - maxRowIndex) * rowStrideBytes);
        cornerHeight3Q12 = sampleCell->terrainHeight;
        cornerHeight2Q12 = sampleCell->terrainHeight;
        cornerHeight1Q12 = sampleCell->terrainHeight;
        cornerHeight0Q12 = sampleCell->terrainHeight;
      }
      else {
        cornerHeight3Q12 = sampleCell->terrainHeight;
        cornerHeight2Q12 = sampleCell[rowLength].terrainHeight;
        cornerHeight1Q12 = sampleCell->terrainHeight;
        cornerHeight0Q12 = sampleCell[rowLength].terrainHeight;
      }
    }
    if (TerrainTriangle_IntersectRayDistance
                       (rayDirection.z,rayEndRowQ12 + rayStartHalfRowQ12 * -2,rayEndColumnQ12 - rayStartColumnQ12,
                        rayOriginZQ12,cornerHeight0Q12,cornerHeight1Q12,cornerHeight2Q12,
                        cornerHeight3Q12,rowFromStartQ12,currentColumnQ12 - rayStartColumnQ12,
                        outDistanceQ12)) {
      return true;
    }
    traversalDone = TerrainRay_AdvanceGridTraversal
                       (rayEndRowQ12,rayEndColumnQ12,rayStartRowQ12,rayStartColumnQ12,
                        rowStrideBytes,currentCell,currentRowQ12,currentColumnQ12);
    /* the step's results: next cell and its row/column coordinates */
    currentCell = g_TerrainRayNextCell;
    currentColumnQ12 = g_TerrainRayNextCoord1Q12;
    currentRowQ12 = g_TerrainRayNextCoord0Q12;
    if (traversalDone) {
      break;
    }
  }
  return false;
}


/* Clears the rebuilt bits 0..6 of every faction byte of every cell's occupancyMask, keeping bit 7. Head of
   tick-wheel case 7, before the per-class occupancy-rebuild callbacks repopulate the mask. The MMX original
   handles eight cells per step and then exactly three more per row, i.e. it assumes a row width of 8n + 3.
*/
void FieldGrid_ClearOccupancyMaskBits0To6AllCells(FieldGridAsset *fieldGrid)

{
  FieldGridOccupancyBlockCount eightCellBlocksPerRow;
  FieldGridOccupancyBlockCount cellBlocksRemaining;
  FieldGridOccupancyBlockCount blocksRemaining;
  FieldGridDimension rowsRemaining;
  FieldGridCell *blockBaseCell;
  uint64_t occupancyHighBitMask;
  FieldGridCell *currentEightCellBlock;

  /* FIELD_CELL_OCCUPANCY_PERSISTENT_BIT in all eight bytes (0x8080808080808080) */
  occupancyHighBitMask = g_FieldGridOccupancyMmxHighBitMask;
  rowsRemaining = fieldGrid->gridHeight;
  eightCellBlocksPerRow = fieldGrid->gridWidth >> 3;
  cellBlocksRemaining = eightCellBlocksPerRow;
  currentEightCellBlock = fieldGrid->cells;
  do {
    do {
      blockBaseCell = currentEightCellBlock;
      blockBaseCell->occupancyMask = blockBaseCell->occupancyMask & occupancyHighBitMask;
      blockBaseCell[1].occupancyMask = blockBaseCell[1].occupancyMask & occupancyHighBitMask;
      blockBaseCell[2].occupancyMask = blockBaseCell[2].occupancyMask & occupancyHighBitMask;
      blockBaseCell[3].occupancyMask = blockBaseCell[3].occupancyMask & occupancyHighBitMask;
      blockBaseCell[4].occupancyMask = blockBaseCell[4].occupancyMask & occupancyHighBitMask;
      blockBaseCell[5].occupancyMask = blockBaseCell[5].occupancyMask & occupancyHighBitMask;
      blockBaseCell[6].occupancyMask = blockBaseCell[6].occupancyMask & occupancyHighBitMask;
      blockBaseCell[7].occupancyMask = blockBaseCell[7].occupancyMask & occupancyHighBitMask;
      /* kept as a separate temporary, as in the original */
      blocksRemaining = cellBlocksRemaining - 1;
      cellBlocksRemaining = blocksRemaining;
      currentEightCellBlock = blockBaseCell + 8;
    } while (blocksRemaining != 0);
    /* the three trailing cells of the row, right after the last block */
    blockBaseCell[8].occupancyMask = blockBaseCell[8].occupancyMask & occupancyHighBitMask;
    blockBaseCell[9].occupancyMask = blockBaseCell[9].occupancyMask & occupancyHighBitMask;
    blockBaseCell[10].occupancyMask = blockBaseCell[10].occupancyMask & occupancyHighBitMask;
    rowsRemaining--;
    cellBlocksRemaining = eightCellBlocksPerRow;
    currentEightCellBlock = blockBaseCell + 11;
  } while (rowsRemaining != 0);
  return;
}

/* Sets FIELD_CELL_OCCUPANCY_BIT0 in one faction's occupancy byte of every cell. Tick-wheel case 7 calls it
   for the active faction when bit 3 of g_UiCommandRuntimeFlags is set, right after the rebuild clear, so
   every cell carries bit 0 for that faction during the rebuild.
*/
void FieldGrid_SetOccupancyMaskByteBit0AllCells
          (FieldGridOccupancyByteIndex occupancyMaskByteIndex,FieldGridAsset *fieldGrid)

{
  FieldGridDimension columnsRemaining;
  FieldGridDimension rowsRemaining;
  FieldGridCell *currentCell;
  FieldGridDimension gridWidth;

  gridWidth = fieldGrid->gridWidth;
  rowsRemaining = fieldGrid->gridHeight;
  currentCell = fieldGrid->cells;
  columnsRemaining = gridWidth;
  do {
    do {
      ((uint8_t *)&currentCell->occupancyMask)[occupancyMaskByteIndex] =
           ((uint8_t *)&currentCell->occupancyMask)[occupancyMaskByteIndex] |
           FIELD_CELL_OCCUPANCY_BIT0;
      currentCell++;
      columnsRemaining--;
    } while (columnsRemaining != 0);
    rowsRemaining--;
    columnsRemaining = gridWidth;
  } while (rowsRemaining != 0);
  return;
}


/* Counterpart of FieldGrid_SetOccupancyMaskByteBit0AllCells: clears FIELD_CELL_OCCUPANCY_BIT0 in one
   faction's occupancy byte of every cell.
*/
void FieldGrid_ClearOccupancyMaskByteBit0AllCells
          (FieldGridOccupancyByteIndex occupancyMaskByteIndex,FieldGridAsset *fieldGrid)

{
  FieldGridDimension columnsRemaining;
  FieldGridDimension rowsRemaining;
  FieldGridCell *currentCell;
  FieldGridDimension gridWidth;

  gridWidth = fieldGrid->gridWidth;
  rowsRemaining = fieldGrid->gridHeight;
  currentCell = fieldGrid->cells;
  columnsRemaining = gridWidth;
  do {
    do {
      ((uint8_t *)&currentCell->occupancyMask)[occupancyMaskByteIndex] =
           ((uint8_t *)&currentCell->occupancyMask)[occupancyMaskByteIndex] &
           (uint8_t)~FIELD_CELL_OCCUPANCY_BIT0;
      currentCell++;
      columnsRemaining--;
    } while (columnsRemaining != 0);
    rowsRemaining--;
    columnsRemaining = gridWidth;
  } while (rowsRemaining != 0);
}


/* Rounds a world point to the nearest field-grid cell and tests occupancy bits 0/1 of the active faction there.
   Returns false when one of them is set, true when the point is outside the grid or neither bit is
   set. Unit, shot and effect code play positioned sounds only when this returns false.
*/
Bool8 TerrainGrid_TestProjectedCellMaskBits01(Q12 worldYQ12,Q12 worldXQ12,WorldRuntimeContext *worldRuntime)

{
  FieldGridAsset *activeFieldGrid;
  int gridColumnIndex;
  uint32_t gridHalfRowCoordinateQ12;
  int gridRowIndex;
  uint8_t occupancyByte;

  activeFieldGrid = worldRuntime->fieldGrid;
  /* FieldGrid_WorldToGridQ12 inlined, then rounded (+0x800 = half a cell) to whole cells */
  gridHalfRowCoordinateQ12 = FIXED_MUL_SHR(worldYQ12, FIELD_GRID_WORLD_Y_TO_ROW_Q20, Q20_SHIFT + 1);
  gridColumnIndex = (int)((FIXED_MUL_SHR(worldXQ12, FIELD_GRID_WORLD_X_TO_COLUMN_Q20, Q20_SHIFT) - gridHalfRowCoordinateQ12) +
                 FIELD_GRID_CELL_Q12 / 2) >> Q12_SHIFT;
  gridRowIndex = (int)(gridHalfRowCoordinateQ12 * 2 + FIELD_GRID_CELL_Q12 / 2) >> Q12_SHIFT;
  if ((gridColumnIndex < 0) || (gridRowIndex < 0) || ((int)activeFieldGrid->gridWidth <= gridColumnIndex) ||
      ((int)activeFieldGrid->gridHeight <= gridRowIndex)) {
    return true;
  }
  occupancyByte =
       ((uint8_t *)&activeFieldGrid->cells[activeFieldGrid->gridWidth * gridRowIndex + gridColumnIndex].occupancyMask)
       [worldRuntime->activeFactionRuntimeIndex];
  return (occupancyByte & FIELD_CELL_OCCUPANCY_BITS01) == 0;
}


/* Clears the debug mark (FIELD_CELL_DEBUG_MARKED, bit 15) in every cell of the grid. No caller
   found in src/ or the image tables, and no code in the game sets the mark.
*/
void FieldGrid_ClearDebugMarkInAllCells(FieldGridAsset *fieldGrid)

{
  int cellsRemaining;
  FieldGridCell *currentCell;
  
  cellsRemaining = fieldGrid->gridWidth * fieldGrid->gridHeight;
  currentCell = fieldGrid->cells;
  do {
    currentCell->flagsAndMaterial = currentCell->flagsAndMaterial & ~FIELD_CELL_DEBUG_MARKED;
    currentCell++;
    cellsRemaining--;
  } while (cellsRemaining != 0);
}


/* Sets the overlay colour (ARGB, FieldGridCell.overlayColor) of every field-grid cell to one value.
*/
void FieldGrid_SetAllCellOverlayColors(PackedArgb32 argbColor,FieldGridAsset *fieldGrid)

{
  int cellsRemaining;
  FieldGridCell *currentCell;

  cellsRemaining = fieldGrid->gridWidth * fieldGrid->gridHeight;
  currentCell = fieldGrid->cells;
  do {
    currentCell->overlayColor = argbColor;
    currentCell++;
    cellsRemaining--;
  } while (cellsRemaining != 0);
}


/* Map editor save of the field grid: copies the loaded asset image (its size is the second dword) into a
   temporary block, resets the runtime-only cell state (normals to straight up, derived dwords and occupancy
   to 0, runtime flag bits cleared), rebuilds fieldFlags as the set of used material ids and writes the block to
   g_LevelResourcePathScratchUtf16. Returns true on success (the temporary block is freed; the free's own
   status is ignored), or false with the allocation or write error in *outError. Called by
   InGameUiCommand_SaveFieldAndLevelAssetImages.
*/
Bool8 FieldGrid_SaveAssetImageFromRuntimeState(uint32_t *sourceImageDwords,uint32_t *outError)

{
  FieldGridAsset *fieldGridImageCopy;
  uint32_t imageSizeBytes;
  uint32_t dwordsLeft;
  int cellsRemaining;
  int occupancyBytesLeft;
  uint32_t *copyDestinationDwords;
  FieldGridCellSaveImageView *fieldGridCellSaveView;
  uint8_t *occupancyBytes;
  uint32_t allocError;
  uint32_t writeError;

  imageSizeBytes = sourceImageDwords[1];
  allocError = g_MemoryApi.alloc(imageSizeBytes,(void **)&fieldGridImageCopy);
  if (allocError != 0) {
    *outError = allocError;
    return false;
  }
  copyDestinationDwords = (uint32_t *)fieldGridImageCopy;
  for (dwordsLeft = imageSizeBytes >> 2; dwordsLeft != 0; dwordsLeft--) {
    *copyDestinationDwords = *sourceImageDwords;
    sourceImageDwords++;
    copyDestinationDwords++;
  }
  fieldGridCellSaveView = (FieldGridCellSaveImageView *)fieldGridImageCopy->cells;
  fieldGridImageCopy->fieldFlags = 0;
  cellsRemaining = fieldGridImageCopy->gridWidth * fieldGridImageCopy->gridHeight;
  do {
    fieldGridCellSaveView->surfacePacketIndex = 0;
    fieldGridCellSaveView->triangle0NormalAngles = FIXED_ANGLE16_QUARTER_TURN << 16; /* elevation: straight up */
    fieldGridCellSaveView->groundScreenX = 0;
    fieldGridCellSaveView->groundScreenY = 0;
    fieldGridCellSaveView->groundViewX = 0;
    fieldGridCellSaveView->groundViewY = 0;
    fieldGridCellSaveView->groundViewZ = 0;
    fieldGridCellSaveView->secondarySurfaceScreenX = 0;
    fieldGridCellSaveView->secondarySurfaceScreenY = 0;
    fieldGridCellSaveView->secondarySurfaceViewX = 0;
    fieldGridCellSaveView->secondarySurfaceViewY = 0;
    fieldGridCellSaveView->secondarySurfaceViewZ = 0;
    /* keep the material byte, the resource-support, map-edge and fluid-exclusion bits (0xe80078ff) */
    fieldGridCellSaveView->flagsAndMaterial =
         fieldGridCellSaveView->flagsAndMaterial &
         (FIELD_CELL_MATERIAL_ID_MASK | FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK | FIELD_CELL_GRID_EDGE_MASK |
          FIELD_CELL_FLUID_RECEIVER_EXCLUDED | FIELD_CELL_FLUID_SOURCE_EXCLUDED);
    fieldGridCellSaveView->persistedAux54 = 0;
    fieldGridCellSaveView->groundDirectionalLightColor = 0;
    fieldGridCellSaveView->secondarySurfaceDirectionalLightColor = 0;
    fieldGridCellSaveView->shadedGroundColor = 0;
    fieldGridCellSaveView->shadedSecondarySurfaceColor = 0;
    fieldGridCellSaveView->visibilityLightingIndex = 0;
    /* one bit per material id in use */
    fieldGridImageCopy->fieldFlags =
         fieldGridImageCopy->fieldFlags |
         1 << ((uint8_t)fieldGridCellSaveView->flagsAndMaterial & 31);
    occupancyBytes = (uint8_t *)&fieldGridCellSaveView->occupancyMask;
    for (occupancyBytesLeft = 8; occupancyBytesLeft != 0; occupancyBytesLeft--) {
      *occupancyBytes = 0;
      occupancyBytes++;
    }
    fieldGridCellSaveView++;
    cellsRemaining--;
  } while (cellsRemaining != 0);
  writeError = FileSystem_WriteBufferToPath
                    ((fieldGridImageCopy->common).allocationSizeBytes,fieldGridImageCopy,
                     g_LevelResourcePathScratchUtf16);
  g_MemoryApi.free(fieldGridImageCopy);
  if (writeError != 0) {
    *outError = writeError;
    return false;
  }
  return true;
}


/* In-game command INGAME_COMMAND_EDITOR_CLEAR_SCRATCH (0x1F20, handler at INGAME_COMMAND_CODE_BASE + code):
   zeroes the player's terrain scratch plane (one dword per cell) at the start of a height drag (mode G 0,
   C 0/1), so that FieldGrid_ApplyPositiveCellDeltas / ..NegativeCellDeltas have no previous step to undo. The
   other payload dwords are unused. Called directly or through the command queue by
   InGameUiCommand_BeginInteractionByMode.
*/
void FieldGrid_ClearPlayerScratchPlane
          (PlayerRuntimeId playerRuntimeId,FieldGridCommandReservedValue reservedCommandValue,
          Q12 reservedWorldYQ12,Q12 reservedWorldXQ12)

{
  int cellsRemaining;
  int *scratchHeightCursor;
  FieldGridAsset *fieldGrid;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  scratchHeightCursor =
       g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->terrainHeightScratchPlane;
  for (cellsRemaining = fieldGrid->gridWidth * fieldGrid->gridHeight; cellsRemaining != 0;
      cellsRemaining--) {
    *scratchHeightCursor = 0;
    scratchHeightCursor++;
  }
}


/* In-game command INGAME_COMMAND_EDITOR_RESET_INFLUENCE (0x2A80, handler at INGAME_COMMAND_CODE_BASE + code):
   copies every cell's terrain height into the player's scratch plane at the start of a smoothing stroke
   (mode G 0, C 2), the reference FieldGrid_RebuildLocalInfluenceState compares against. The other payload
   dwords are unused. Called directly or through the command queue by InGameUiCommand_BeginInteractionByMode.
*/
void FieldGrid_ResetLocalInfluenceState
          (PlayerRuntimeId playerRuntimeId,FieldGridCommandReservedValue reservedCommandValue,
          Q12 reservedWorldYQ12,Q12 reservedWorldXQ12)

{
  int cellsRemaining;
  FieldGridCell *currentCell;
  int *scratchHeightCursor;
  FieldGridAsset *fieldGrid;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  scratchHeightCursor =
       g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->terrainHeightScratchPlane;
  cellsRemaining = fieldGrid->gridWidth * fieldGrid->gridHeight;
  currentCell = fieldGrid->cells;
  do {
    *scratchHeightCursor = currentCell->terrainHeight;
    currentCell++;
    scratchHeightCursor++;
    cellsRemaining--;
  } while (cellsRemaining != 0);
}


/* Moves the water level of the cell at Q12 grid row/column (gridRowQ12/gridColumnQ12) by -64 per
   heightDeltaUnits (the vertical drag distance, so dragging up raises it) and refreshes the normals and light of
   the cell and of its six neighbours that are not border cells. Called by FieldGrid_ApplyEncodedCellUpdate.
*/
void FieldGrid_ApplyEncodedUpdateCore(FieldGridHeightDeltaUnits heightDeltaUnits,Q12 gridRowQ12,Q12 gridColumnQ12,
          FieldGridAsset *fieldGrid)

{
  FieldGridDimension rowLength;
  int columnIndex;
  int rowIndex;
  int rowStrideBytes;
  FieldGridCell *cell;
  
  rowLength = fieldGrid->gridWidth;
  columnIndex = gridColumnQ12 >> Q12_SHIFT;
  rowIndex = gridRowQ12 >> Q12_SHIFT;
  if ((-1 < columnIndex) && (-1 < rowIndex) && (columnIndex < (int)rowLength) &&
      (rowIndex < (int)fieldGrid->gridHeight)) {
    rowStrideBytes = rowLength * sizeof(FieldGridCell);
    cell = fieldGrid->cells + columnIndex + rowIndex * rowLength;
    cell->waterSurfaceDelta = cell->waterSurfaceDelta + heightDeltaUnits * -FIELD_GRID_EDIT_DRAG_UNIT_Q12; /* 64 per unit */
    FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell);
    FieldGridCell_ComputeDirectionalLightColor(cell);
    if ((cell[-1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell - 1);
      FieldGridCell_ComputeDirectionalLightColor(cell - 1);
    }
    if ((cell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell + 1);
      FieldGridCell_ComputeDirectionalLightColor(cell + 1);
    }
    cell = cell - rowLength; /* row above */
    if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell);
      FieldGridCell_ComputeDirectionalLightColor(cell);
    }
    if ((cell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell + 1);
      FieldGridCell_ComputeDirectionalLightColor(cell + 1);
    }
    cell = cell + rowLength * 2 - 1; /* row below, one to the left */
    if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell);
      FieldGridCell_ComputeDirectionalLightColor(cell);
    }
    if ((cell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell + 1);
      FieldGridCell_ComputeDirectionalLightColor(cell + 1);
    }
  }
}


/* One cell of FieldGrid_ApplyRadialTerrainHeightDeltaAndRefreshSurface: when the cell lies strictly inside the
   circle, its height changes by amplitude * (distance^2 / radius^2 - 1) (a paraboloid, -amplitude at the centre,
   0 at the rim), cells with water (waterSurfaceDelta >= 0) keep their water level, and a non-negative
   terrainMaterialIndexOrNegativeSentinel replaces the material byte.
*/
void FieldGridCell_ApplyRadialTerrainHeightDeltaAndMaterial(TerrainMaterialIndex terrainMaterialIndexOrNegativeSentinel,
          FieldGridRadiusUnits radiusWorldUnits,Q12 terrainHeightDeltaAmplitudeQ12,
          Q12 centerWorldYQ12,Q12 centerWorldXQ12,FieldGridCell *cell)

{
  int deltaX;
  int deltaY;
  uint64_t distanceSquared;
  int distanceSquaredHigh;
  int64_t radiusSquared;
  int radiusSquaredHigh;
  uint32_t radiusSquaredLow;
  uint32_t radiusSquaredQ12;
  int64_t scaledDeltaProduct;
  uint32_t heightDeltaQ12;

  deltaX = centerWorldXQ12 - cell->worldX;
  deltaY = cell->worldY - centerWorldYQ12;
  distanceSquared = (int64_t)deltaY * (int64_t)deltaY + (int64_t)deltaX * (int64_t)deltaX;
  distanceSquaredHigh = (int)(distanceSquared >> 32);
  radiusSquared = (int64_t)radiusWorldUnits * (int64_t)radiusWorldUnits;
  radiusSquaredHigh = (int)((uint64_t)radiusSquared >> 32);
  radiusSquaredLow = (uint32_t)radiusSquared;
  /* only strictly inside the circle: distance^2 < radius^2 as a two-word compare (the low words signed) */
  if (radiusSquaredHigh < distanceSquaredHigh) {
    return;
  }
  if (distanceSquaredHigh == radiusSquaredHigh && (int)radiusSquaredLow <= (int)distanceSquared) {
    return;
  }
  radiusSquaredQ12 = radiusSquaredHigh << 20 | radiusSquaredLow >> 12;
  if (radiusSquaredQ12 == 0) {
    return;
  }
  scaledDeltaProduct = (int64_t)
          ((int)((int64_t)distanceSquared / (int64_t)(int)radiusSquaredQ12) - Q12_ONE) *
          (int64_t)terrainHeightDeltaAmplitudeQ12;
  heightDeltaQ12 = FIXED_PRODUCT_SHR(scaledDeltaProduct, Q12_SHIFT);
  cell->terrainHeight = cell->terrainHeight + heightDeltaQ12;
  if (-1 < cell->waterSurfaceDelta) {
    cell->waterSurfaceDelta = cell->waterSurfaceDelta - heightDeltaQ12;
  }
  if (-1 < terrainMaterialIndexOrNegativeSentinel) {
    cell->flagsAndMaterial =
         cell->flagsAndMaterial & ~FIELD_CELL_MATERIAL_ID_MASK |
         terrainMaterialIndexOrNegativeSentinel;
  }
}


/* Water flow pass A (tick-wheel case 1): scans the interior cells row by row and pulls the water surface
   (terrainHeight + waterSurfaceDelta) of the six hexagonal neighbours 1/8 of the way toward the source
   cell's surface. Sources with negative water or FIELD_CELL_FLUID_SOURCE_EXCLUDED are skipped, receivers
   with FIELD_CELL_FLUID_RECEIVER_EXCLUDED are left alone.
   The source is the centre cell itself (its waterSurfaceDelta); verified against the original
   machine code by OPEN_THANDOR_SELFTEST=relaxcmp.
*/
void TerrainGrid_RelaxNeighborHeightsForwardWithSignGate(FieldGridAsset *fieldGrid)

{
  int columnsRemaining;
  int rowsRemaining;
  int sourceSurfaceHeightQ12;
  FieldGridCell *rowStartCell;
  FieldGridCell *centerCell;
  FieldGridDimension gridWidth;
  
  gridWidth = fieldGrid->gridWidth;
  rowsRemaining = fieldGrid->gridHeight - 2;
  rowStartCell = fieldGrid->cells + gridWidth;
  do {
    columnsRemaining = gridWidth - 2;
    centerCell = rowStartCell;
    do {
      centerCell++;
      if ((-1 < centerCell->waterSurfaceDelta) &&
         ((centerCell->flagsAndMaterial & FIELD_CELL_FLUID_SOURCE_EXCLUDED) == 0)) {
        sourceSurfaceHeightQ12 = centerCell->waterSurfaceDelta + centerCell->terrainHeight;
        if ((centerCell[-gridWidth].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) ==
            0) {
          centerCell[-gridWidth].waterSurfaceDelta =
               centerCell[-gridWidth].waterSurfaceDelta -
               ((centerCell[-gridWidth].waterSurfaceDelta +
                centerCell[-gridWidth].terrainHeight) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((centerCell[1 - gridWidth].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED)
            == 0) {
          centerCell[1 - gridWidth].waterSurfaceDelta =
               centerCell[1 - gridWidth].waterSurfaceDelta -
               ((centerCell[1 - gridWidth].waterSurfaceDelta +
                centerCell[1 - gridWidth].terrainHeight) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((centerCell[gridWidth].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0
           ) {
          centerCell[gridWidth].waterSurfaceDelta =
               centerCell[gridWidth].waterSurfaceDelta -
               ((centerCell[gridWidth].waterSurfaceDelta +
                centerCell[gridWidth].terrainHeight) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((centerCell[gridWidth - 1].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED)
            == 0) {
          centerCell[gridWidth - 1].waterSurfaceDelta =
               centerCell[gridWidth - 1].waterSurfaceDelta -
               ((centerCell[gridWidth - 1].waterSurfaceDelta +
                centerCell[gridWidth - 1].terrainHeight) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((centerCell[-1].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          centerCell[-1].waterSurfaceDelta =
               centerCell[-1].waterSurfaceDelta -
               ((centerCell[-1].waterSurfaceDelta + centerCell[-1].terrainHeight) -
                sourceSurfaceHeightQ12 >> 3);
        }
        if ((centerCell[1].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          centerCell[1].waterSurfaceDelta =
               centerCell[1].waterSurfaceDelta -
               ((centerCell[1].waterSurfaceDelta + centerCell[1].terrainHeight) -
                sourceSurfaceHeightQ12 >> 3);
        }
      }
      columnsRemaining--;
    } while (columnsRemaining != 0);
    rowStartCell = centerCell + 2;
    rowsRemaining--;
  } while (rowsRemaining != 0);
  return;
}


/* Water flow pass B (tick-wheel case 5): the same neighbour relaxation as pass A, scanning the interior
   cells backwards from the bottom-right, so water spreads evenly in both directions over two ticks.
   Cells are addressed by raw byte offsets (cell size 0x80; +0x48 terrainHeight, +0x4C waterSurfaceDelta,
   +0x50 flagsAndMaterial; +/-0x80 is the next/previous cell).
   The source is the centre cell itself; verified by OPEN_THANDOR_SELFTEST=relaxcmp.
*/
void TerrainGrid_RelaxNeighborHeightsReverseWithSignGate(FieldGridAsset *fieldGrid)

{
  int *lowerNeighborWaterDelta;
  FieldGridDimension rowLength;
  int columnsRemaining;
  int rowsRemaining;
  int sourceSurfaceHeightQ12;
  int rowStartCellAddress;
  int centerCellAddress;
  int upperCellAddress;
  int *neighborWaterDelta;
  
  rowLength = fieldGrid->gridWidth;
  rowsRemaining = fieldGrid->gridHeight - 2;
  rowStartCellAddress =
       (int)fieldGrid + rowLength * -(int)sizeof(FieldGridCell) +
       (rowLength * fieldGrid->gridHeight + -1) * sizeof(FieldGridCell) + offsetof(FieldGridAsset,cells);
  do {
    columnsRemaining = rowLength - 2;
    centerCellAddress = rowStartCellAddress;
    do {
      centerCellAddress = centerCellAddress - (int)sizeof(FieldGridCell);
      if ((-1 < ((FieldGridCell *)centerCellAddress)->waterSurfaceDelta) &&
         (((uint32_t)((FieldGridCell *)centerCellAddress)->flagsAndMaterial & FIELD_CELL_FLUID_SOURCE_EXCLUDED) == 0)) {
        sourceSurfaceHeightQ12 =
             ((FieldGridCell *)centerCellAddress)->waterSurfaceDelta + ((FieldGridCell *)centerCellAddress)->terrainHeight;
        upperCellAddress = centerCellAddress + rowLength * -(int)sizeof(FieldGridCell);
        if (((uint32_t)((FieldGridCell *)upperCellAddress)->flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          neighborWaterDelta = &((FieldGridCell *)upperCellAddress)->waterSurfaceDelta;
          *neighborWaterDelta =
               *neighborWaterDelta -
               ((((FieldGridCell *)upperCellAddress)->waterSurfaceDelta + ((FieldGridCell *)upperCellAddress)->terrainHeight) - sourceSurfaceHeightQ12 >> 3);
        }
        if (((uint32_t)((FieldGridCell *)upperCellAddress)[1].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          ((FieldGridCell *)upperCellAddress)[1].waterSurfaceDelta =
               ((FieldGridCell *)upperCellAddress)[1].waterSurfaceDelta -
               ((((FieldGridCell *)upperCellAddress)[1].waterSurfaceDelta + ((FieldGridCell *)upperCellAddress)[1].terrainHeight) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((*(uint32_t *)(upperCellAddress + offsetof(FieldGridCell,flagsAndMaterial) + rowLength * FIELD_GRID_TWO_CELLS_BYTES) & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          lowerNeighborWaterDelta = (int *)(upperCellAddress + offsetof(FieldGridCell,waterSurfaceDelta) + rowLength * FIELD_GRID_TWO_CELLS_BYTES);
          *lowerNeighborWaterDelta = *lowerNeighborWaterDelta - ((*(int *)(upperCellAddress + offsetof(FieldGridCell,waterSurfaceDelta) + rowLength * FIELD_GRID_TWO_CELLS_BYTES) +
                               *(int *)(upperCellAddress + offsetof(FieldGridCell,terrainHeight) + rowLength * FIELD_GRID_TWO_CELLS_BYTES)) - sourceSurfaceHeightQ12 >> 3
                              );
        }
        if ((((FieldGridCell *)(upperCellAddress + rowLength * FIELD_GRID_TWO_CELLS_BYTES))[-1].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          lowerNeighborWaterDelta = &((FieldGridCell *)(upperCellAddress + rowLength * FIELD_GRID_TWO_CELLS_BYTES))[-1].waterSurfaceDelta;
          *lowerNeighborWaterDelta = *lowerNeighborWaterDelta - ((((FieldGridCell *)(upperCellAddress + rowLength * FIELD_GRID_TWO_CELLS_BYTES))[-1].waterSurfaceDelta +
                               ((FieldGridCell *)(upperCellAddress + rowLength * FIELD_GRID_TWO_CELLS_BYTES))[-1].terrainHeight) - sourceSurfaceHeightQ12 >>
                              3);
        }
        centerCellAddress = upperCellAddress + rowLength * sizeof(FieldGridCell);
        if (((uint32_t)((FieldGridCell *)centerCellAddress)[-1].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          ((FieldGridCell *)centerCellAddress)[-1].waterSurfaceDelta =
               ((FieldGridCell *)centerCellAddress)[-1].waterSurfaceDelta -
               ((((FieldGridCell *)centerCellAddress)[-1].waterSurfaceDelta + ((FieldGridCell *)centerCellAddress)[-1].terrainHeight
                ) - sourceSurfaceHeightQ12 >> 3);
        }
        if (((uint32_t)((FieldGridCell *)centerCellAddress)[1].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          ((FieldGridCell *)centerCellAddress)[1].waterSurfaceDelta =
               ((FieldGridCell *)centerCellAddress)[1].waterSurfaceDelta -
               ((((FieldGridCell *)centerCellAddress)[1].waterSurfaceDelta + ((FieldGridCell *)centerCellAddress)[1].terrainHeight) -
                sourceSurfaceHeightQ12 >> 3);
        }
      }
      columnsRemaining--;
    } while (columnsRemaining != 0);
    rowStartCellAddress = centerCellAddress + -(int)FIELD_GRID_TWO_CELLS_BYTES;
    rowsRemaining--;
  } while (rowsRemaining != 0);
  return;
}


/* Water relaxation, forward, without the sign gate: like pass A
   (TerrainGrid_RelaxNeighborHeightsForwardWithSignGate) it pulls the water surface of the six neighbours of
   every interior cell 1/8 of the way toward that cell's surface, but cells with negative water are sources too.
   FIELD_CELL_FLUID_SOURCE_EXCLUDED and FIELD_CELL_FLUID_RECEIVER_EXCLUDED are honoured. Run by
   TerrainGrid_RunDirectionalRelaxationPasses when mode bit 0 is set (the editor's land tool); verified against
   the original by OPEN_THANDOR_SELFTEST=relaxcmp. (cellBeforeSource is advanced at the top of the inner loop, so
   inside it it is the source cell.)
*/
void TerrainGrid_RelaxNeighborHeightsForward(FieldGridAsset *fieldGrid)

{
  int columnsRemaining;
  int rowsRemaining;
  int sourceSurfaceHeightQ12;
  FieldGridCell *sourceCell;
  FieldGridCell *cellBeforeSource;
  FieldGridDimension gridWidth;
  
  gridWidth = fieldGrid->gridWidth;
  rowsRemaining = fieldGrid->gridHeight - 2;
  sourceCell = fieldGrid->cells + gridWidth;
  do {
    columnsRemaining = gridWidth - 2;
    cellBeforeSource = sourceCell;
    do {
      cellBeforeSource = cellBeforeSource + 1;
      if ((cellBeforeSource->flagsAndMaterial & FIELD_CELL_FLUID_SOURCE_EXCLUDED) == 0) {
        sourceSurfaceHeightQ12 =
             cellBeforeSource->waterSurfaceDelta + cellBeforeSource->terrainHeight;
        if ((cellBeforeSource[-gridWidth].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) ==
            0) {
          cellBeforeSource[-gridWidth].waterSurfaceDelta =
               cellBeforeSource[-gridWidth].waterSurfaceDelta -
               ((cellBeforeSource[-gridWidth].waterSurfaceDelta +
                cellBeforeSource[-gridWidth].terrainHeight) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((cellBeforeSource[1 - gridWidth].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED)
            == 0) {
          cellBeforeSource[1 - gridWidth].waterSurfaceDelta =
               cellBeforeSource[1 - gridWidth].waterSurfaceDelta -
               ((cellBeforeSource[1 - gridWidth].waterSurfaceDelta +
                cellBeforeSource[1 - gridWidth].terrainHeight) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((cellBeforeSource[gridWidth].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0
           ) {
          cellBeforeSource[gridWidth].waterSurfaceDelta =
               cellBeforeSource[gridWidth].waterSurfaceDelta -
               ((cellBeforeSource[gridWidth].waterSurfaceDelta +
                cellBeforeSource[gridWidth].terrainHeight) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((cellBeforeSource[gridWidth - 1].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED)
            == 0) {
          cellBeforeSource[gridWidth - 1].waterSurfaceDelta =
               cellBeforeSource[gridWidth - 1].waterSurfaceDelta -
               ((cellBeforeSource[gridWidth - 1].waterSurfaceDelta +
                cellBeforeSource[gridWidth - 1].terrainHeight) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((cellBeforeSource[-1].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          cellBeforeSource[-1].waterSurfaceDelta =
               cellBeforeSource[-1].waterSurfaceDelta -
               ((cellBeforeSource[-1].waterSurfaceDelta + cellBeforeSource[-1].terrainHeight) -
                sourceSurfaceHeightQ12 >> 3);
        }
        if ((cellBeforeSource[1].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          cellBeforeSource[1].waterSurfaceDelta =
               cellBeforeSource[1].waterSurfaceDelta -
               ((cellBeforeSource[1].waterSurfaceDelta + cellBeforeSource[1].terrainHeight) -
                sourceSurfaceHeightQ12 >> 3);
        }
      }
      columnsRemaining = columnsRemaining + -1;
      cellBeforeSource = cellBeforeSource;
    } while (columnsRemaining != 0);
    sourceCell = cellBeforeSource + 2;
    rowsRemaining = rowsRemaining + -1;
  } while (rowsRemaining != 0);
  return;
}


/* Water relaxation, backward, without the sign gate: TerrainGrid_RelaxNeighborHeightsForward scanning the
   interior cells from the bottom-right, the partner pass of the land tool in
   TerrainGrid_RunDirectionalRelaxationPasses. Cells are addressed by raw byte offsets (cell size 0x80; +0x48
   terrainHeight, +0x4C waterSurfaceDelta, +0x50 flagsAndMaterial; 0x40000000 = FIELD_CELL_FLUID_SOURCE_EXCLUDED,
   0x20000000 = FIELD_CELL_FLUID_RECEIVER_EXCLUDED). Verified by OPEN_THANDOR_SELFTEST=relaxcmp.
*/
void TerrainGrid_RelaxNeighborHeightsReverse(FieldGridAsset *fieldGrid)

{
  int *lowerNeighborWaterDelta;
  FieldGridDimension rowLength;
  int columnsRemaining;
  int rowsRemaining;
  int sourceSurfaceHeightQ12;
  int sourceCellAddress;
  int cellAfterSourceAddress;
  int upperRowCellAddress;
  int *neighborWaterDelta;
  FieldGridDimension gridWidth;
  
  rowLength = fieldGrid->gridWidth;
  rowsRemaining = fieldGrid->gridHeight - 2;
  sourceCellAddress =
       (int)fieldGrid + rowLength * -(int)sizeof(FieldGridCell) +
       (rowLength * fieldGrid->gridHeight + -1) * sizeof(FieldGridCell) + offsetof(FieldGridAsset,cells);
  do {
    columnsRemaining = rowLength - 2;
    cellAfterSourceAddress = sourceCellAddress;
    do {
      cellAfterSourceAddress = cellAfterSourceAddress + -(int)sizeof(FieldGridCell);
      if (((uint32_t)((FieldGridCell *)cellAfterSourceAddress)->flagsAndMaterial & FIELD_CELL_FLUID_SOURCE_EXCLUDED) == 0) {
        sourceSurfaceHeightQ12 =
             ((FieldGridCell *)cellAfterSourceAddress)->waterSurfaceDelta + ((FieldGridCell *)cellAfterSourceAddress)->terrainHeight;
        upperRowCellAddress = cellAfterSourceAddress + rowLength * -(int)sizeof(FieldGridCell);
        if (((uint32_t)((FieldGridCell *)upperRowCellAddress)->flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          neighborWaterDelta = &((FieldGridCell *)upperRowCellAddress)->waterSurfaceDelta;
          *neighborWaterDelta =
               *neighborWaterDelta -
               ((((FieldGridCell *)upperRowCellAddress)->waterSurfaceDelta + ((FieldGridCell *)upperRowCellAddress)->terrainHeight) - sourceSurfaceHeightQ12 >> 3);
        }
        if (((uint32_t)((FieldGridCell *)upperRowCellAddress)[1].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          ((FieldGridCell *)upperRowCellAddress)[1].waterSurfaceDelta =
               ((FieldGridCell *)upperRowCellAddress)[1].waterSurfaceDelta -
               ((((FieldGridCell *)upperRowCellAddress)[1].waterSurfaceDelta + ((FieldGridCell *)upperRowCellAddress)[1].terrainHeight) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((*(uint32_t *)(upperRowCellAddress + offsetof(FieldGridCell,flagsAndMaterial) + rowLength * FIELD_GRID_TWO_CELLS_BYTES) & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          lowerNeighborWaterDelta = (int *)(upperRowCellAddress + offsetof(FieldGridCell,waterSurfaceDelta) + rowLength * FIELD_GRID_TWO_CELLS_BYTES);
          *lowerNeighborWaterDelta = *lowerNeighborWaterDelta - ((*(int *)(upperRowCellAddress + offsetof(FieldGridCell,waterSurfaceDelta) + rowLength * FIELD_GRID_TWO_CELLS_BYTES) +
                               *(int *)(upperRowCellAddress + offsetof(FieldGridCell,terrainHeight) + rowLength * FIELD_GRID_TWO_CELLS_BYTES)) - sourceSurfaceHeightQ12 >> 3
                              );
        }
        if ((((FieldGridCell *)(upperRowCellAddress + rowLength * FIELD_GRID_TWO_CELLS_BYTES))[-1].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          lowerNeighborWaterDelta = &((FieldGridCell *)(upperRowCellAddress + rowLength * FIELD_GRID_TWO_CELLS_BYTES))[-1].waterSurfaceDelta;
          *lowerNeighborWaterDelta = *lowerNeighborWaterDelta - ((((FieldGridCell *)(upperRowCellAddress + rowLength * FIELD_GRID_TWO_CELLS_BYTES))[-1].waterSurfaceDelta +
                               ((FieldGridCell *)(upperRowCellAddress + rowLength * FIELD_GRID_TWO_CELLS_BYTES))[-1].terrainHeight) - sourceSurfaceHeightQ12 >>
                              3);
        }
        cellAfterSourceAddress = upperRowCellAddress + rowLength * sizeof(FieldGridCell);
        if (((uint32_t)((FieldGridCell *)cellAfterSourceAddress)[-1].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          ((FieldGridCell *)cellAfterSourceAddress)[-1].waterSurfaceDelta =
               ((FieldGridCell *)cellAfterSourceAddress)[-1].waterSurfaceDelta -
               ((((FieldGridCell *)cellAfterSourceAddress)[-1].waterSurfaceDelta + ((FieldGridCell *)cellAfterSourceAddress)[-1].terrainHeight
                ) - sourceSurfaceHeightQ12 >> 3);
        }
        if (((uint32_t)((FieldGridCell *)cellAfterSourceAddress)[1].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          ((FieldGridCell *)cellAfterSourceAddress)[1].waterSurfaceDelta =
               ((FieldGridCell *)cellAfterSourceAddress)[1].waterSurfaceDelta -
               ((((FieldGridCell *)cellAfterSourceAddress)[1].waterSurfaceDelta + ((FieldGridCell *)cellAfterSourceAddress)[1].terrainHeight) -
                sourceSurfaceHeightQ12 >> 3);
        }
      }
      columnsRemaining = columnsRemaining + -1;
      cellAfterSourceAddress = cellAfterSourceAddress;
    } while (columnsRemaining != 0);
    sourceCellAddress = cellAfterSourceAddress + -(int)FIELD_GRID_TWO_CELLS_BYTES;
    rowsRemaining = rowsRemaining + -1;
  } while (rowsRemaining != 0);
  return;
}


/* Height-drag brush of FieldGrid_ApplyPositiveCellDeltas around one centre cell (Q12 grid row/column
   centerRowQ12/centerColumnQ12): the target height is the source cell's terrainHeight - 64 * heightDeltaUnits;
   every cell of accumulatorPlane (one height per cell) within radius = min(|64 * radiusUnits|, 0x5000) world
   units of the centre cell moves towards it by (1 + cos(pi * distance / (radius + 1))) / 2 of the difference,
   but never away from it (so overlapping brushes keep the strongest pull). The scanned box is the centre +/- 4 *
   radius in grid Q12 coordinates, clipped to the grid.
*/
void FieldGrid_ProcessHorizontalSpan(Q12 sourceRowQ12,Q12 sourceColumnQ12,FieldGridHeightDeltaUnits heightDeltaUnits,
          FieldGridRadiusUnits radiusUnits,Q12 centerRowQ12,Q12 centerColumnQ12,
          FieldGridAccumulatorValue *accumulatorPlane,FieldGridAsset *fieldGrid)

{
  FieldGridDimension rowLength;
  int64_t falloffProduct;
  uint32_t spanRadiusQ12;
  int boxMinColumnQ12;
  int boxMinRowQ12;
  int minColumn;
  int minRow;
  int maxColumn;
  int maxRow;
  int spanColumnCount;
  int firstCellIndex;
  int centerCellIndex;
  int centerX;
  int centerY;
  int sourceHeight;
  uint32_t cellDistance;
  int blendedHeight;
  int heightDifference;
  FieldGridCell *spanCell;
  int *accumulatorCursor;
  FieldGridCell *rowStartCell;
  int *rowStartAccumulator;
  int rowsRemaining;
  int columnsLeft;

  spanRadiusQ12 = radiusUnits * FIELD_GRID_EDIT_DRAG_UNIT_Q12;
  if ((int)spanRadiusQ12 < 0) {
    spanRadiusQ12 = radiusUnits * -FIELD_GRID_EDIT_DRAG_UNIT_Q12;
  }
  if (FIELD_GRID_EDIT_BRUSH_RADIUS_MAX < spanRadiusQ12) {
    spanRadiusQ12 = FIELD_GRID_EDIT_BRUSH_RADIUS_MAX;
  }
  boxMinColumnQ12 = centerColumnQ12 + spanRadiusQ12 * -4;
  boxMinRowQ12 = centerRowQ12 + spanRadiusQ12 * -4;
  maxColumn = (int)(boxMinColumnQ12 + Q12_FRACTION_MASK + spanRadiusQ12 * 8) >> Q12_SHIFT;
  maxRow = (int)(boxMinRowQ12 + Q12_FRACTION_MASK + spanRadiusQ12 * 8) >> Q12_SHIFT;
  minColumn = boxMinColumnQ12 >> Q12_SHIFT;
  if (minColumn < 0) {
    minColumn = 0;
  }
  minRow = boxMinRowQ12 >> Q12_SHIFT;
  if (minRow < 0) {
    minRow = 0;
  }
  if ((int)fieldGrid->gridWidth <= maxColumn) {
    maxColumn = fieldGrid->gridWidth - 1;
  }
  if ((int)fieldGrid->gridHeight <= maxRow) {
    maxRow = fieldGrid->gridHeight - 1;
  }
  if (maxColumn < minColumn || maxRow < minRow) {
    return;
  }
  spanColumnCount = (maxColumn - minColumn) + 1;
  firstCellIndex = minRow * fieldGrid->gridWidth + minColumn;
  rowLength = fieldGrid->gridWidth;
  centerCellIndex = (centerRowQ12 >> Q12_SHIFT) * rowLength + (centerColumnQ12 >> Q12_SHIFT);
  centerX = fieldGrid->cells[centerCellIndex].worldX;
  centerY = fieldGrid->cells[centerCellIndex].worldY;
  sourceHeight = fieldGrid->cells
          [(sourceRowQ12 >> Q12_SHIFT) * fieldGrid->gridWidth + (sourceColumnQ12 >> Q12_SHIFT)].terrainHeight;
  rowStartCell = fieldGrid->cells + firstCellIndex;
  rowStartAccumulator = accumulatorPlane + firstCellIndex;
  for (rowsRemaining = (maxRow - minRow) + 1; rowsRemaining != 0; rowsRemaining--) {
    spanCell = rowStartCell;
    accumulatorCursor = rowStartAccumulator;
    for (columnsLeft = spanColumnCount; columnsLeft != 0; columnsLeft--) {
      cellDistance = FixedMath_Length2(spanCell->worldY - centerY,spanCell->worldX - centerX);
      if (cellDistance <= spanRadiusQ12 + 1) {
        heightDifference = (sourceHeight + heightDeltaUnits * -FIELD_GRID_EDIT_DRAG_UNIT_Q12) - *accumulatorCursor;
        falloffProduct = (int64_t)
                (g_FixedSineQ28
                 [FIXED_SINE_TABLE_COS +
                  (int)((int64_t)
                        ((((int64_t)(int)cellDistance & FIELD_GRID_ANGLE_PRODUCT_HIGH_BITS_MASK) >> 17) << 32 |
                        (int64_t)(int)cellDistance * FIXED_ANGLE16_HALF_TURN & 0xffffffffU) / (int64_t)(int)(spanRadiusQ12 + 1))
                 ] + Q28_ONE) * (int64_t)heightDifference;
        blendedHeight = (FIXED_PRODUCT_SHR(falloffProduct, Q28_SHIFT + 1)) + *accumulatorCursor;
        /* only move towards the target, never back */
        if (heightDifference < 0) {
          if (blendedHeight < *accumulatorCursor) {
            *accumulatorCursor = blendedHeight;
          }
        }
        else if (0 < heightDifference && *accumulatorCursor < blendedHeight) {
          *accumulatorCursor = blendedHeight;
        }
      }
      spanCell++;
      accumulatorCursor++;
    }
    rowStartCell = rowStartCell + rowLength;
    rowStartAccumulator = rowStartAccumulator + rowLength;
  }
}


/* Raise/lower brush of FieldGrid_ApplyNegativeCellDeltas around one centre cell (Q12 grid row/column
   centerRowQ12/centerColumnQ12): adds (1 + cos(pi * distance / (radius + 1))) / 2 * offset, offset = -64 *
   heightDeltaUnits, to every accumulatorPlane entry within radius = min(|64 * radiusUnits|, 0x5000) world units
   of the centre cell, capping the sum at offset (so overlapping brushes do not add up beyond it). The scanned
   box is the centre +/- 2 * radius in grid Q12 coordinates, clipped to the grid.
*/
void FieldGrid_ProcessVerticalSpan(FieldGridHeightDeltaUnits heightDeltaUnits,FieldGridRadiusUnits radiusUnits,
          Q12 centerRowQ12,Q12 centerColumnQ12,FieldGridAccumulatorValue *accumulatorPlane,
          FieldGridAsset *fieldGrid)

{
  int targetOffset;
  FieldGridDimension rowLength;
  int64_t falloffProduct;
  uint32_t spanRadiusQ12;
  int boxMinColumnQ12;
  int boxMinRowQ12;
  int minColumn;
  int minRow;
  int maxColumn;
  int maxRow;
  int spanColumnCount;
  int firstCellIndex;
  int centerCellIndex;
  int centerX;
  int centerY;
  uint32_t cellDistance;
  int blendedValue;
  FieldGridCell *spanCell;
  int *accumulatorCursor;
  FieldGridCell *rowStartCell;
  int *rowStartAccumulator;
  int rowsRemaining;
  int columnsLeft;

  spanRadiusQ12 = radiusUnits * FIELD_GRID_EDIT_DRAG_UNIT_Q12;
  if ((int)spanRadiusQ12 < 0) {
    spanRadiusQ12 = radiusUnits * -FIELD_GRID_EDIT_DRAG_UNIT_Q12;
  }
  if (FIELD_GRID_EDIT_BRUSH_RADIUS_MAX < spanRadiusQ12) {
    spanRadiusQ12 = FIELD_GRID_EDIT_BRUSH_RADIUS_MAX;
  }
  targetOffset = heightDeltaUnits * -FIELD_GRID_EDIT_DRAG_UNIT_Q12;
  boxMinColumnQ12 = centerColumnQ12 + spanRadiusQ12 * -2;
  boxMinRowQ12 = centerRowQ12 + spanRadiusQ12 * -2;
  maxColumn = (int)(boxMinColumnQ12 + Q12_FRACTION_MASK + spanRadiusQ12 * 4) >> Q12_SHIFT;
  maxRow = (int)(boxMinRowQ12 + Q12_FRACTION_MASK + spanRadiusQ12 * 4) >> Q12_SHIFT;
  minColumn = boxMinColumnQ12 >> Q12_SHIFT;
  if (minColumn < 0) {
    minColumn = 0;
  }
  minRow = boxMinRowQ12 >> Q12_SHIFT;
  if (minRow < 0) {
    minRow = 0;
  }
  if ((int)fieldGrid->gridWidth <= maxColumn) {
    maxColumn = fieldGrid->gridWidth - 1;
  }
  if ((int)fieldGrid->gridHeight <= maxRow) {
    maxRow = fieldGrid->gridHeight - 1;
  }
  if (maxColumn < minColumn || maxRow < minRow) {
    return;
  }
  spanColumnCount = (maxColumn - minColumn) + 1;
  firstCellIndex = minRow * fieldGrid->gridWidth + minColumn;
  rowLength = fieldGrid->gridWidth;
  centerCellIndex = (centerRowQ12 >> Q12_SHIFT) * rowLength + (centerColumnQ12 >> Q12_SHIFT);
  centerX = fieldGrid->cells[centerCellIndex].worldX;
  centerY = fieldGrid->cells[centerCellIndex].worldY;
  rowStartCell = fieldGrid->cells + firstCellIndex;
  rowStartAccumulator = accumulatorPlane + firstCellIndex;
  for (rowsRemaining = (maxRow - minRow) + 1; rowsRemaining != 0; rowsRemaining--) {
    spanCell = rowStartCell;
    accumulatorCursor = rowStartAccumulator;
    for (columnsLeft = spanColumnCount; columnsLeft != 0; columnsLeft--) {
      cellDistance = FixedMath_Length2(spanCell->worldY - centerY,spanCell->worldX - centerX);
      if (cellDistance <= spanRadiusQ12 + 1) {
        falloffProduct = (int64_t)
                (g_FixedSineQ28
                 [FIXED_SINE_TABLE_COS +
                  (int)((int64_t)
                        ((((int64_t)(int)cellDistance & FIELD_GRID_ANGLE_PRODUCT_HIGH_BITS_MASK) >> 17) << 32 |
                        (int64_t)(int)cellDistance * FIXED_ANGLE16_HALF_TURN & 0xffffffffU) / (int64_t)(int)(spanRadiusQ12 + 1))
                 ] + Q28_ONE) * (int64_t)targetOffset;
        blendedValue = (FIXED_PRODUCT_SHR(falloffProduct, Q28_SHIFT + 1)) + *accumulatorCursor;
        /* cap the sum at targetOffset */
        if (targetOffset < 0) {
          if (blendedValue < targetOffset) {
            blendedValue = targetOffset;
          }
        }
        else if (targetOffset < blendedValue) {
          blendedValue = targetOffset;
        }
        *accumulatorCursor = blendedValue;
      }
      spanCell++;
      accumulatorCursor++;
    }
    rowStartCell = rowStartCell + rowLength;
    rowStartAccumulator = rowStartAccumulator + rowLength;
  }
}


/* Replaces the material byte of the cell at Q12 grid row/column (gridRowQ12/gridColumnQ12) with transitionValue,
   when the cell is inside the grid. Called by FieldGrid_ApplyLocalCellUpdate.
*/
void FieldGrid_ApplySingleCellTransition(FieldGridTransitionValue transitionValue,Q12 gridRowQ12,Q12 gridColumnQ12,
          FieldGridAsset *fieldGrid)

{
  int gridRowIndex;
  int gridColumnIndex;
  int cellIndex;

  gridRowIndex = gridRowQ12 >> Q12_SHIFT;
  gridColumnIndex = gridColumnQ12 >> Q12_SHIFT;
  if ((-1 < gridRowIndex) && (-1 < gridColumnIndex) && (gridRowIndex < (int)fieldGrid->gridHeight) &&
      (gridColumnIndex < (int)fieldGrid->gridWidth)) {
    cellIndex = gridRowIndex * fieldGrid->gridWidth + gridColumnIndex;
    fieldGrid->cells[cellIndex].flagsAndMaterial =
         fieldGrid->cells[cellIndex].flagsAndMaterial & ~FIELD_CELL_MATERIAL_ID_MASK | transitionValue;
  }
}


/* Smooths one cell: sets the terrain height of the cell at Q12 grid row/column (gridRowQ12/gridColumnQ12) to the
   average of its six neighbours (the water level stays); only rows 2..height-2 and columns 2..width-2 are
   touched. Called by FieldGrid_RebuildLocalInfluenceState.
*/
void FieldGrid_ApplyRectangularTransition(Q12 gridRowQ12,Q12 gridColumnQ12,FieldGridAsset *fieldGrid)

{
  Q12 *heightField;
  int gridRowIndex;
  int gridColumnIndex;
  int aboveCellIndex;
  int heightDelta;
  FieldGridDimension gridWidth;

  gridWidth = fieldGrid->gridWidth;
  gridRowIndex = gridRowQ12 >> Q12_SHIFT;
  gridColumnIndex = gridColumnQ12 >> Q12_SHIFT;
  if ((1 < gridRowIndex) && (1 < gridColumnIndex) && (gridRowIndex + 1 < (int)fieldGrid->gridHeight) &&
      (gridColumnIndex + 1 < (int)gridWidth)) {
    /* index of the neighbour above; the six terms are the neighbours above, above right, left, right, below
       left and below, the centre is cells[aboveCellIndex + gridWidth] */
    aboveCellIndex = (gridRowIndex - 1) * gridWidth + gridColumnIndex;
    heightDelta = (fieldGrid->cells[aboveCellIndex].terrainHeight +
             fieldGrid->cells[aboveCellIndex + 1].terrainHeight +
             fieldGrid->cells[aboveCellIndex + (gridWidth - 1)].terrainHeight +
             fieldGrid->cells[aboveCellIndex + gridWidth + 1].terrainHeight +
             fieldGrid->cells[aboveCellIndex + gridWidth * 2 - 1].terrainHeight
            + fieldGrid->cells[aboveCellIndex + gridWidth * 2].terrainHeight) / 6 -
            fieldGrid->cells[aboveCellIndex + gridWidth].terrainHeight;
    heightField = &fieldGrid->cells[aboveCellIndex + gridWidth].terrainHeight;
    *heightField = *heightField + heightDelta;
    heightField = &fieldGrid->cells[aboveCellIndex + gridWidth].waterSurfaceDelta;
    *heightField = *heightField - heightDelta;
  }
}


/* Converts a world-plane position to field-grid coordinates in Q12 (integer part = cell column/row,
   fraction = position inside the cell), returned as column and row. The triangular lattice makes the
   column shift by half a cell per row.
*/
FieldGridCoordinates FieldGrid_WorldToGridQ12(Q12 worldY,Q12 worldX)

{
  uint32_t gridHalfRowCoordinateQ12;
  FieldGridCoordinates gridCoordinates;

  /* Q12 * Q20 >> 21: half the row coordinate */
  gridHalfRowCoordinateQ12 =
       FIXED_MUL_SHR(worldY, FIELD_GRID_WORLD_Y_TO_ROW_Q20, Q20_SHIFT + 1);
  gridCoordinates.rowQ12 = gridHalfRowCoordinateQ12 * 2;
  gridCoordinates.columnQ12 =
       FIXED_MUL_SHR(worldX, FIELD_GRID_WORLD_X_TO_COLUMN_Q20, Q20_SHIFT) - gridHalfRowCoordinateQ12;
  return gridCoordinates;
}


/* Sets the flags of the cell at Q12 grid row/column (gridRowQ12/gridColumnQ12) to (flags & preserveMask) |
   setMask, when the cell is inside the grid. Shared by FieldGrid_SetCellFluidReceiverExcluded,
   FieldGrid_SetCellFluidSourceExcluded and FieldGrid_SetCellResourceSupportFlag.
*/
void FieldGrid_ApplyMaskedRegionCore
          (FieldGridRegionMask preserveMask,FieldGridRegionMask setMask,Q12 gridRowQ12,Q12 gridColumnQ12,
          FieldGridAsset *fieldGrid)

{
  int gridRowIndex;
  int gridColumnIndex;
  int cellIndex;

  gridRowIndex = gridRowQ12 >> Q12_SHIFT;
  gridColumnIndex = gridColumnQ12 >> Q12_SHIFT;
  if ((-1 < gridRowIndex) && (-1 < gridColumnIndex) && (gridRowIndex < (int)fieldGrid->gridHeight) &&
      (gridColumnIndex < (int)fieldGrid->gridWidth)) {
    cellIndex = gridRowIndex * fieldGrid->gridWidth + gridColumnIndex;
    fieldGrid->cells[cellIndex].flagsAndMaterial =
         preserveMask & fieldGrid->cells[cellIndex].flagsAndMaterial | setMask;
  }
}


/* Recomputes a cell's two vertex normals from its six lattice neighbours and stores them as packed
   (azimuth | elevation << 16) angle pairs: triangle0NormalAngles for the terrain surface and
   triangle1NormalAngles for the secondary surface (terrainHeight + waterSurfaceDelta). The lighting in
   FieldGridCell_ComputeDirectionalLightColor reads the first one.
*/
void FieldGridCell_RecomputeTriangleNormalAngles(FieldGridRowStrideBytes rowStrideBytes,FieldGridCell *cell)

{
  int rightDelta;
  int neighborDeltaA;
  int neighborDeltaB;
  int neighborDeltaC;
  int neighborDeltaD;
  int neighborDeltaE;
  int neighborDeltaF;
  FixedVectorAngles normalAngles;

  /* The six neighbours of the triangular lattice, addressed as raw byte offsets from the cell (0x80 bytes per
     cell, rowStrideBytes per row; worldX +0x40, worldY +0x44, terrainHeight +0x48, waterSurfaceDelta +0x4C):
       cell[1] right, cell[-1] left, +rowStrideBytes below, -rowStrideBytes above,
       +rowStrideBytes - 0x80 below-left, -rowStrideBytes + 0x80 above-right.
     Each normal is (-sum(dX * dH), -sum(dY * dH), 0xC00000) over the neighbours. First pass (terrain):
     rightDelta = right, A = below, B = above, C = left, D = below-left, E = above-right. Second pass (surface):
     A = right, B = below, C = above, D = left, E = below-left, F = above-right. */
  rightDelta = cell[1].terrainHeight - cell->terrainHeight;
  neighborDeltaA = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)->terrainHeight - cell->terrainHeight;
  neighborDeltaB = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)->terrainHeight - cell->terrainHeight;
  neighborDeltaC = cell[-1].terrainHeight - cell->terrainHeight;
  neighborDeltaD = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)[-1].terrainHeight - cell->terrainHeight;
  neighborDeltaE = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)[1].terrainHeight - cell->terrainHeight;
  normalAngles = FixedMath_VectorToAngles
                    (FIELD_GRID_NORMAL_Z_COMPONENT,((((-((FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)->worldY -
                                    cell->worldY) * neighborDeltaA) - (cell[1].worldY - cell->worldY) * rightDelta
                                 ) - (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)->worldY - cell->worldY)
                                     * neighborDeltaB) - (cell[-1].worldY - cell->worldY) * neighborDeltaC) -
                              (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)[-1].worldY - cell->worldY)
                              * neighborDeltaD) -
                              (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)[1].worldY - cell->worldY) * neighborDeltaE
                     ,((((-((FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)->worldX - cell->worldX) *
                           neighborDeltaA) - (cell[1].worldX - cell->worldX) * rightDelta) -
                        (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)->worldX - cell->worldX) * neighborDeltaB) -
                       (cell[-1].worldX - cell->worldX) * neighborDeltaC) -
                      (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)[-1].worldX - cell->worldX) * neighborDeltaD
                      ) - (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)[1].worldX - cell->worldX) * neighborDeltaE);
  cell->triangle0NormalAngles = normalAngles.azimuthAngle | normalAngles.elevationAngle << 16;
  neighborDeltaA = ((cell[1].terrainHeight + cell[1].waterSurfaceDelta) - cell->terrainHeight) -
          cell->waterSurfaceDelta;
  neighborDeltaB = ((FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)->terrainHeight +
           FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)->waterSurfaceDelta) - cell->terrainHeight) -
          cell->waterSurfaceDelta;
  neighborDeltaC = ((FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)->terrainHeight +
           FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)->waterSurfaceDelta) -
          cell->terrainHeight) - cell->waterSurfaceDelta;
  neighborDeltaD = ((cell[-1].terrainHeight + cell[-1].waterSurfaceDelta) - cell->terrainHeight) -
          cell->waterSurfaceDelta;
  neighborDeltaE = ((FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)[-1].terrainHeight +
           FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)[-1].waterSurfaceDelta) - cell->terrainHeight) -
          cell->waterSurfaceDelta;
  neighborDeltaF = ((FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)[1].terrainHeight +
           FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)[1].waterSurfaceDelta) -
          cell->terrainHeight) - cell->waterSurfaceDelta;
  normalAngles = FixedMath_VectorToAngles
                    (FIELD_GRID_NORMAL_Z_COMPONENT,((((-((FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)->worldY -
                                    cell->worldY) * neighborDeltaB) - (cell[1].worldY - cell->worldY) * neighborDeltaA
                                 ) - (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)->worldY - cell->worldY) * neighborDeltaC) -
                               (cell[-1].worldY - cell->worldY) * neighborDeltaD) -
                              (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)[-1].worldY - cell->worldY)
                              * neighborDeltaE) - (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)[1].worldY - cell->worldY) * neighborDeltaF
                     ,((((-((FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)->worldX - cell->worldX) *
                           neighborDeltaB) - (cell[1].worldX - cell->worldX) * neighborDeltaA) -
                        (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)->worldX - cell->worldX) * neighborDeltaC) -
                       (cell[-1].worldX - cell->worldX) * neighborDeltaD) -
                      (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)[-1].worldX - cell->worldX) * neighborDeltaE
                      ) - (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)[1].worldX - cell->worldX) * neighborDeltaF);
  cell->triangle1NormalAngles = normalAngles.azimuthAngle | normalAngles.elevationAngle << 16;
}


/* Diffuse terrain lighting for one cell: turns the terrain normal (triangle0NormalAngles) back into a Q28
   direction, dots it with the global light direction and looks the result up in the directional light colour
   table (groundDirectionalLightColor). The secondary surface always gets the one fixed secondary colour
   (secondarySurfaceDirectionalLightColor).
*/
void FieldGridCell_ComputeDirectionalLightColor(FieldGridCell *cell)

{
  FixedDirection normalDirection;
  PackedArgb32 directionalLightColor;

  /* packed as azimuth (low word) | elevation (high word) */
  normalDirection = FixedMath_DirectionFromAnglesQ28
                    ((int)cell->triangle0NormalAngles >> 16,cell->triangle0NormalAngles & FIXED_ANGLE16_MASK);
  /* the signed Q8 dot product (-256..256) indexes the table from its middle entry; the shaded ramp is the
     lower half */
  directionalLightColor =
       g_TerrainDirectionalLightColorLut
       [TERRAIN_DIRECTIONAL_LIGHT_LUT_ZERO_INDEX +
        (((int)((uint64_t)((int64_t)(int)normalDirection.x * (int64_t)g_TerrainLightDirection.x) >> 32) +
          (int)((uint64_t)((int64_t)(int)normalDirection.y * (int64_t)g_TerrainLightDirection.y) >> 32) +
          (int)((uint64_t)((int64_t)(int)normalDirection.z * (int64_t)g_TerrainLightDirection.z) >> 32)) >>
         16)];
  cell->secondarySurfaceDirectionalLightColor = g_TerrainDirectionalLightSecondaryColor;
  cell->groundDirectionalLightColor = directionalLightColor;
}


/* Class vtables. */

const FieldGridInterpolationCallbackTable5 g_FieldGridInterpolationCallbacks5 = {
    .callbacks = {
        /* 0 */ THANDOR_FN(FieldGrid_InterpolateTerrainHeight),
        /* 1 */ THANDOR_FN(FieldGrid_InterpolateWaterSurfaceHeight),
        /* 2 */ THANDOR_FN(FieldGrid_InterpolateTerrainHeight),
        /* 3 */ THANDOR_FN(FieldGrid_InterpolateTerrainHeight),
        /* 4 */ THANDOR_FN(FieldGrid_InterpolateTopSurfaceHeight)
    }
};
