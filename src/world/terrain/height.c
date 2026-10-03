/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/height.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/terrain/height.h>
#include <thandor/thandor.h>

/* Implementation ownership: world/terrain/height. */

/* Shared set-up of the two hexagon placement tests below: sets g_TerrainScanStepLimit (radius /
   TERRAIN_SCAN_RADIUS_PER_STEP clamped to 1..TERRAIN_SCAN_STEP_LIMIT_MAX), g_TerrainScanReferenceHeight and
   g_TerrainScanRowStrideBytes, and maps the world point to the field cell containing it. Returns false when that
   cell lies off the grid, otherwise true with the cell's index in *outCenterCellIndex. */
static bool TerrainScan_BeginAroundWorldPoint
          (FieldGridRadiusUnits radiusWorldUnits,Q12 referenceHeightQ12,Q12 worldXQ12,Q12 worldYQ12,
          FieldGridAsset *fieldGrid,int *outCenterCellIndex)

{
  FieldGridCoordinates gridCoordinates;
  uint32_t baseColumn;
  uint32_t columnFractionQ12;
  uint32_t rowFractionQ12;
  uint32_t fractionSum;
  uint32_t cellRow;
  uint32_t cellColumn;
  uint32_t gridWidth;

  g_TerrainScanStepLimit = (uint32_t)radiusWorldUnits / TERRAIN_SCAN_RADIUS_PER_STEP;
  if (g_TerrainScanStepLimit == 0) {
    g_TerrainScanStepLimit = 1;
  }
  else if (TERRAIN_SCAN_STEP_LIMIT_MAX < g_TerrainScanStepLimit) {
    g_TerrainScanStepLimit = TERRAIN_SCAN_STEP_LIMIT_MAX;
  }
  g_TerrainScanReferenceHeight = referenceHeightQ12;
  gridCoordinates = FieldGrid_WorldToGridQ12(worldXQ12,worldYQ12);
  baseColumn = gridCoordinates.columnQ12 >> 12;
  cellRow = gridCoordinates.rowQ12 >> 12;
  columnFractionQ12 = (uint32_t)gridCoordinates.columnQ12 & Q12_FRACTION_MASK;
  rowFractionQ12 = (uint32_t)gridCoordinates.rowQ12 & Q12_FRACTION_MASK;
  fractionSum = rowFractionQ12 + columnFractionQ12 * 2;
  cellColumn = baseColumn;
  if (fractionSum < FIELD_GRID_CELL_Q12) {
    if (FIELD_GRID_CELL_Q12 - 1 < columnFractionQ12 + rowFractionQ12 * 2) {
      cellRow++;
    }
  }
  else if (fractionSum < FIELD_GRID_TWO_CELLS_Q12 + 1) {
    cellColumn = baseColumn + 1;
    if (columnFractionQ12 < rowFractionQ12) {
      cellRow++;
      cellColumn = baseColumn;
    }
  }
  else {
    cellColumn = baseColumn + 1;
    if (FIELD_GRID_TWO_CELLS_Q12 - 1 < columnFractionQ12 + rowFractionQ12 * 2) {
      cellRow++;
    }
  }
  gridWidth = fieldGrid->gridWidth;
  g_TerrainScanRowStrideBytes = gridWidth * sizeof(FieldGridCell);
  if ((int)cellColumn < 0 || (int)cellRow < 0 || fieldGrid->gridHeight <= cellRow ||
      (gridWidth & FIELD_GRID_ROW_STRIDE_WIDTH_MASK) <= cellColumn) {
    return false;
  }
  *outCenterCellIndex = cellRow * (gridWidth & FIELD_GRID_ROW_STRIDE_WIDTH_MASK) + cellColumn;
  return true;
}


/* Terrain placement test for every terrain class except 1 (g_TerrainClassPlacementAndOverlayCallbacks10
   .placementTests[0, 2..4], also called directly by the army placement code): maps the world point to its field
   cell and checks the hexagon of radius radiusWorldUnits around it. Returns true (CF set, rejected) when a cell is
   a map-edge cell, lies under water, or its height relative to referenceHeightQ12 leaves
   [g_TerrainHeightBandMinimumDelta, g_TerrainHeightBandMaximumDelta]; also when fieldGrid is NULL or the point is
   off the grid.
*/
bool TerrainHeightBand_TestAroundWorldPoint
          (FieldGridRadiusUnits radiusWorldUnits,Q12 referenceHeightQ12,Q12 worldXQ12,Q12 worldYQ12,
          FieldGridAsset *fieldGrid)

{
  int centerCellIndex;
  FieldGridCell *centerCell;
  int relativeHeightQ12;
  uint32_t gridWidth;
  FieldGridCell *sector0Start;
  FieldGridCell *sector1Start;
  FieldGridCell *sector2Start;
  FieldGridCell *sector3Start;

  if (fieldGrid == NULL) {
    return true;
  }
  if (!TerrainScan_BeginAroundWorldPoint(radiusWorldUnits,referenceHeightQ12,worldXQ12,worldYQ12,fieldGrid,
                                         &centerCellIndex)) {
    return true;
  }
  centerCell = &fieldGrid->cells[centerCellIndex];
  if ((centerCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
    return true;
  }
  relativeHeightQ12 = centerCell->terrainHeight - g_TerrainScanReferenceHeight;
  if (0 < centerCell->waterSurfaceDelta) {
    return true;
  }
  if (((int)g_TerrainHeightBandMaximumDelta < relativeHeightQ12) ||
      (relativeHeightQ12 < (int)g_TerrainHeightBandMinimumDelta)) {
    return true;
  }
  /* sector n starts at the centre's neighbour in direction n (W = grid width):
     C+1, C+1-W, C-W, C-1, C-1+W, C+W */
  gridWidth = fieldGrid->gridWidth;
  sector0Start = &fieldGrid->cells[centerCellIndex + 1];
  sector1Start = sector0Start - gridWidth;
  sector2Start = sector1Start - 1;
  sector3Start = sector2Start + (gridWidth - 1);
  return TerrainHeightBand_TestWedge0(0,sector0Start) ||
         TerrainHeightBand_TestWedge1(0,sector1Start) ||
         TerrainHeightBand_TestWedge2(0,sector2Start) ||
         TerrainHeightBand_TestWedge3(0,(uint8_t *)sector3Start) ||
         TerrainHeightBand_TestWedge4(0,sector3Start + gridWidth) ||
         TerrainHeightBand_TestWedge5(0,sector3Start + gridWidth + 1);
}


/* Terrain placement test for terrain class 1 / water-surface contact (g_TerrainClassPlacementAndOverlayCallbacks10
   .placementTests[1], also called directly by the army placement code): maps the world point to its field cell and
   checks the hexagon of radius radiusWorldUnits around it. Returns true (CF set, rejected) when a cell is a map-edge
   cell, has a negative waterSurfaceDelta, or the high word of its packed normal angles is below
   g_TerrainAuxHeightMinimum; also when fieldGrid is NULL or the point is off the grid.
*/
bool TerrainAuxHeightThreshold_TestAroundWorldPoint
          (FieldGridRadiusUnits radiusWorldUnits,Q12 referenceHeightQ12,Q12 worldXQ12,Q12 worldYQ12,
          FieldGridAsset *fieldGrid)

{
  int centerCellIndex;
  FieldGridCell *centerCell;
  uint32_t gridWidth;
  FieldGridCell *sector0Start;
  FieldGridCell *sector1Start;
  FieldGridCell *sector2Start;
  FieldGridCell *sector3Start;

  if (fieldGrid == NULL) {
    return true;
  }
  if (!TerrainScan_BeginAroundWorldPoint(radiusWorldUnits,referenceHeightQ12,worldXQ12,worldYQ12,fieldGrid,
                                         &centerCellIndex)) {
    return true;
  }
  /* the centre cell's threshold test reads triangle0NormalAngles (+0x08); the sector tests read
     triangle1NormalAngles (+0x78). Both as in the original. */
  centerCell = &fieldGrid->cells[centerCellIndex];
  if ((centerCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
    return true;
  }
  if (centerCell->waterSurfaceDelta < 0) {
    return true;
  }
  if ((int)centerCell->triangle0NormalAngles >> 16 < (int)g_TerrainAuxHeightMinimum) {
    return true;
  }
  /* sector n starts at the centre's neighbour in direction n (W = grid width):
     C+1, C+1-W, C-W, C-1, C-1+W, C+W */
  gridWidth = fieldGrid->gridWidth;
  sector0Start = &fieldGrid->cells[centerCellIndex + 1];
  sector1Start = sector0Start - gridWidth;
  sector2Start = sector1Start - 1;
  sector3Start = sector2Start + (gridWidth - 1);
  return TerrainAuxHeightThreshold_TestWedge0(0,sector0Start) ||
         TerrainAuxHeightThreshold_TestWedge1(0,sector1Start) ||
         TerrainAuxHeightThreshold_TestWedge2(0,sector2Start) ||
         TerrainAuxHeightThreshold_TestWedge3(0,sector3Start) ||
         TerrainAuxHeightThreshold_TestWedge4(0,sector3Start + gridWidth) ||
         TerrainAuxHeightThreshold_TestWedge5(0,sector3Start + gridWidth + 1);
}


/* Flatten brush step for one cell: levels it to g_TerrainScanReferenceHeight and moves the removed height into
   waterSurfaceDelta, so the water surface stays where it was. */
static void TerrainHeightDelta_LevelCell(FieldGridCell *cell)

{
  int heightAdjustmentQ12;

  heightAdjustmentQ12 = g_TerrainScanReferenceHeight - cell->terrainHeight;
  cell->terrainHeight = cell->terrainHeight + heightAdjustmentQ12;
  cell->waterSurfaceDelta = cell->waterSurfaceDelta - heightAdjustmentQ12;
}


/* Flatten brush, sector 0 of the hexagon around the brush vertex
   (FieldGrid_ApplyHeightAtWorldPointAndRefreshNeighbors): walks the sector's diagonal, levels each cell and the one between it and the next diagonal cell to
   g_TerrainScanReferenceHeight (the removed height goes into waterSurfaceDelta, so the water surface stays), and
   starts the straight scans of directions 0 and 1 that fill the sector. Stops at a map-edge cell or the radius.
*/
void TerrainHeightDelta_ApplyWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int rowStrideBytes;
  FieldGridCell *directionStartCell;
  FieldGridCell *betweenCell;

  while ((scanStep < g_TerrainScanStepLimit) && ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0)) {
    TerrainHeightDelta_LevelCell(cell);
    rowStrideBytes = g_TerrainScanRowStrideBytes;
    directionStartCell = cell + 1;
    TerrainHeightDelta_ApplyDirection0(scanStep + TERRAIN_SCAN_STEP_STRAIGHT,directionStartCell);
    if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
      return;
    }
    /* the in-between cell is one row up from directionStartCell */
    betweenCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(directionStartCell,-rowStrideBytes);
    if ((betweenCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
      return;
    }
    TerrainHeightDelta_LevelCell(betweenCell);
    cell = betweenCell + 1;
    scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
    TerrainHeightDelta_ApplyDirection1
              (scanStep,FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-g_TerrainScanRowStrideBytes));
  }
}


/* Flatten brush, sector 1: like TerrainHeightDelta_ApplyWedge0, levelling the sector's diagonal and starting the
   straight scans of directions 1 and 2.
*/
void TerrainHeightDelta_ApplyWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int rowStrideBytes;
  FieldGridCell *directionStartCell;
  FieldGridCell *betweenCell;

  while ((scanStep < g_TerrainScanStepLimit) && ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0)) {
    TerrainHeightDelta_LevelCell(cell);
    rowStrideBytes = g_TerrainScanRowStrideBytes;
    TerrainHeightDelta_ApplyDirection1
              (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,
               FIELD_GRID_CELL_AT_BYTE_OFFSET(cell + 1,-g_TerrainScanRowStrideBytes));
    if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
      return;
    }
    /* the in-between cell is the one above */
    betweenCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes);
    if ((betweenCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
      return;
    }
    TerrainHeightDelta_LevelCell(betweenCell);
    /* two rows up */
    directionStartCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-g_TerrainScanRowStrideBytes - rowStrideBytes);
    scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
    cell = directionStartCell + 1;
    TerrainHeightDelta_ApplyDirection2(scanStep,directionStartCell);
  }
}


/* Flatten brush, sector 2: like TerrainHeightDelta_ApplyWedge0, levelling the sector's diagonal and starting the
   straight scans of directions 2 and 3.
*/
void TerrainHeightDelta_ApplyWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *directionStartCell;
  int heightAdjustmentQ12;
  int adjacentHeightAdjustmentQ12;

  if (scanStep < g_TerrainScanStepLimit) {
    while ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      heightAdjustmentQ12 = g_TerrainScanReferenceHeight - cell->terrainHeight;
      cell->terrainHeight = cell->terrainHeight + heightAdjustmentQ12;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - heightAdjustmentQ12;
      TerrainHeightDelta_ApplyDirection2
                (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      if ((cell[-1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      adjacentHeightAdjustmentQ12 = g_TerrainScanReferenceHeight - cell[-1].terrainHeight;
      cell[-1].terrainHeight = cell[-1].terrainHeight + adjacentHeightAdjustmentQ12;
      cell[-1].waterSurfaceDelta = cell[-1].waterSurfaceDelta - adjacentHeightAdjustmentQ12;
      directionStartCell = cell - 2;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell - 1,-g_TerrainScanRowStrideBytes); /* up and left */
      TerrainHeightDelta_ApplyDirection3(scanStep,directionStartCell);
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
}


/* Flatten brush, sector 3: like TerrainHeightDelta_ApplyWedge0, levelling the sector's diagonal and starting the
   straight scans of directions 3 and 4.
*/
void TerrainHeightDelta_ApplyWedge3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int rowStrideBytes;
  FieldGridCell *directionStartCell;
  FieldGridCell *betweenCell;

  while ((scanStep < g_TerrainScanStepLimit) && ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0)) {
    TerrainHeightDelta_LevelCell(cell);
    rowStrideBytes = g_TerrainScanRowStrideBytes;
    directionStartCell = cell - 1;
    TerrainHeightDelta_ApplyDirection3(scanStep + TERRAIN_SCAN_STEP_STRAIGHT,directionStartCell);
    if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
      return;
    }
    /* the in-between cell is the one below directionStartCell (one row stride on) */
    betweenCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(directionStartCell,rowStrideBytes);
    if ((betweenCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
      return;
    }
    TerrainHeightDelta_LevelCell(betweenCell);
    cell = betweenCell - 1;
    scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
    TerrainHeightDelta_ApplyDirection4
              (scanStep,FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,g_TerrainScanRowStrideBytes));
  }
}


/* Flatten brush, sector 4: like TerrainHeightDelta_ApplyWedge0, levelling the sector's diagonal and starting the
   straight scans of directions 4 and 5.
*/
void TerrainHeightDelta_ApplyWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int rowStrideBytes;
  FieldGridCell *betweenCell;
  FieldGridCell *direction5StartCell;

  while ((scanStep < g_TerrainScanStepLimit) && ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0)) {
    TerrainHeightDelta_LevelCell(cell);
    rowStrideBytes = g_TerrainScanRowStrideBytes;
    /* the scan starts one row down and one cell left */
    TerrainHeightDelta_ApplyDirection4
              (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,
               FIELD_GRID_CELL_AT_BYTE_OFFSET(cell - 1,g_TerrainScanRowStrideBytes));
    if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
      return;
    }
    /* the in-between cell is the one below */
    betweenCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes);
    if ((betweenCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
      return;
    }
    TerrainHeightDelta_LevelCell(betweenCell);
    scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
    /* two rows down: the direction 5 start; the next diagonal cell is one left of it */
    direction5StartCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(betweenCell,g_TerrainScanRowStrideBytes);
    cell = direction5StartCell - 1;
    TerrainHeightDelta_ApplyDirection5(scanStep,direction5StartCell);
  }
}


/* Flatten brush, sector 5: like TerrainHeightDelta_ApplyWedge0, levelling the sector's diagonal and starting the
   straight scans of directions 5 and 0.
*/
void TerrainHeightDelta_ApplyWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *directionStartCell;
  int heightAdjustmentQ12;
  int adjacentHeightAdjustmentQ12;

  if (scanStep < g_TerrainScanStepLimit) {
    while ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      heightAdjustmentQ12 = g_TerrainScanReferenceHeight - cell->terrainHeight;
      cell->terrainHeight = cell->terrainHeight + heightAdjustmentQ12;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - heightAdjustmentQ12;
      /* the scan starts one row down */
      TerrainHeightDelta_ApplyDirection5
                (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,
                 FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      if ((cell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      adjacentHeightAdjustmentQ12 = g_TerrainScanReferenceHeight - cell[1].terrainHeight;
      cell[1].terrainHeight = cell[1].terrainHeight + adjacentHeightAdjustmentQ12;
      cell[1].waterSurfaceDelta = cell[1].waterSurfaceDelta - adjacentHeightAdjustmentQ12;
      directionStartCell = cell + 2;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell + 1,g_TerrainScanRowStrideBytes); /* below the right cell */
      TerrainHeightDelta_ApplyDirection0(scanStep,directionStartCell);
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
}


/* Low word of the 64-bit product rayCrossLocal * heightDelta (heightDelta taken as signed), high word in *outHigh,
   computed from 32-bit halves as in the original. */
static uint32_t TerrainTriangle_MulCrossByHeightDelta(uint64_t rayCrossLocal,uint32_t heightDelta,int *outHigh)

{
  uint64_t lowProduct;
  int crossHigh;

  crossHigh = (int)(rayCrossLocal >> 32);
  lowProduct = (uint64_t)heightDelta * (rayCrossLocal & UINT32_MAX);
  if ((int)heightDelta < 0) {
    *outHigh = (crossHigh * heightDelta - (int)rayCrossLocal) + (int)(lowProduct >> 32);
  }
  else {
    *outHigh = crossHigh * heightDelta + (int)(lowProduct >> 32);
  }
  return (uint32_t)lowProduct;
}


/* Hit position inside a triangle: the divisor is remaining + coord1 + coord0 (all high:low word pairs scaled by
   2^12), shifted back down by 12 bits. Returns false (outputs untouched) when the divisor's low word is zero.
   When the divisor does not fit a signed 32-bit word, divisor and both numerators are shifted down by another
   12 bits; then both numerators are divided by it (signed 64/32 division) into the coord1 and coord0 offsets. */
static bool TerrainTriangle_DivideHitNumerators
          (uint32_t remainingLow,int remainingHigh,uint32_t coord1Low,int coord1High,uint32_t coord0Low,
          int coord0High,int *outCoord1Offset,int *outCoord0Offset)

{
  uint32_t partialSumLow;
  uint32_t sumLow;
  uint32_t divisorLow;
  int divisorHigh;

  partialSumLow = remainingLow + coord1Low;
  sumLow = partialSumLow + coord0Low;
  divisorHigh = remainingHigh + coord1High + (uint32_t)(partialSumLow < remainingLow) + coord0High +
                (uint32_t)(sumLow < partialSumLow);
  divisorLow = (sumLow >> Q12_SHIFT) | divisorHigh * (1 << (32 - Q12_SHIFT));
  divisorHigh = divisorHigh >> Q12_SHIFT;
  if (divisorLow == 0) {
    return false;
  }
  if (((int)divisorLow < 0) ? (divisorHigh != -1) : (divisorHigh != 0)) {
    coord0Low = coord0Low >> Q12_SHIFT | coord0High * (1 << (32 - Q12_SHIFT));
    coord1Low = coord1Low >> Q12_SHIFT | coord1High * (1 << (32 - Q12_SHIFT));
    divisorLow = divisorLow >> Q12_SHIFT | divisorHigh * (1 << (32 - Q12_SHIFT));
    coord1High = coord1High >> Q12_SHIFT;
    coord0High = coord0High >> Q12_SHIFT;
  }
  *outCoord1Offset = (int)((int64_t)((uint64_t)(uint32_t)coord1High << 32 | (uint64_t)coord1Low) /
                           (int64_t)(int)divisorLow);
  *outCoord0Offset = (int)((int64_t)((uint64_t)(uint32_t)coord0High << 32 | (uint64_t)coord0Low) /
                           (int64_t)(int)divisorLow);
  return true;
}


/* First triangle of TerrainTriangle_IntersectRayDistance, based on corner 3 (local coordinates as given). Returns
   true with the distance in *outDistanceQ12 on a hit, false on a miss. */
static bool TerrainTriangle_IntersectRayCorner3Triangle
          (Q12 rayDeltaZQ12,Q12 gridRayDelta0Q12,Q12 gridRayDelta1Q12,Q12 rayOriginZQ12,
          Q12 cornerHeight1Q12,Q12 cornerHeight2Q12,Q12 cornerHeight3Q12,
          Q12 cellLocalCoord1Q12,Q12 cellLocalCoord0Q12,Q12 *outDistanceQ12)

{
  int64_t planeRate;
  int64_t planeOffset;
  uint64_t rayCrossLocal;
  uint32_t crossProductLow;
  int64_t originByDelta1;
  int64_t originByDelta0;
  uint32_t originShiftedLow;
  uint32_t rayZShiftedLow;
  uint32_t coord0PartialLow;
  uint32_t coord0NumeratorLow;
  int coord0NumeratorHigh;
  uint32_t coord1PartialLow;
  uint32_t coord1NumeratorLow;
  int coord1NumeratorHigh;
  uint32_t rateShiftedHigh;
  uint32_t rateShiftedLow;
  uint32_t remainingLow;
  int remainingHigh;
  int coord1Offset;
  int coord0Offset;
  int hitCoord1;
  int64_t coord0HeightProduct;
  int64_t coord1HeightProduct;
  int64_t worldXOffsetProduct;
  int64_t worldYOffsetProduct;

  planeRate = ((int64_t)(cornerHeight1Q12 - cornerHeight3Q12) * (int64_t)gridRayDelta0Q12 +
              (int64_t)(cornerHeight2Q12 - cornerHeight3Q12) * (int64_t)gridRayDelta1Q12) -
              ((int64_t)rayDeltaZQ12 << Q12_SHIFT);
  planeOffset = (int64_t)(cornerHeight1Q12 - cornerHeight3Q12) * (int64_t)cellLocalCoord1Q12 +
                (int64_t)(cornerHeight2Q12 - cornerHeight3Q12) * (int64_t)cellLocalCoord0Q12 +
                ((int64_t)(rayOriginZQ12 - cornerHeight3Q12) << Q12_SHIFT);
  /* the ray must cross the triangle's plane */
  if (planeOffset < 0) {
    if ((planeRate >= 0) || (planeOffset - planeRate < 0)) {
      return false;
    }
  }
  else if ((planeRate < 0) || (planeOffset - planeRate >= 0)) {
    return false;
  }
  /* test the edges: each numerator must have the sign of the plane rate */
  rayCrossLocal = (int64_t)gridRayDelta1Q12 * (int64_t)cellLocalCoord1Q12 -
                  (int64_t)cellLocalCoord0Q12 * (int64_t)gridRayDelta0Q12;
  crossProductLow = TerrainTriangle_MulCrossByHeightDelta
                              (rayCrossLocal,cornerHeight1Q12 - cornerHeight3Q12,&coord0NumeratorHigh);
  originByDelta1 = (int64_t)(rayOriginZQ12 - cornerHeight3Q12) * (int64_t)gridRayDelta1Q12;
  originShiftedLow = (uint32_t)originByDelta1 * Q12_ONE;
  coord0PartialLow = crossProductLow + originShiftedLow;
  rayZShiftedLow = (uint32_t)((int64_t)rayDeltaZQ12 * (int64_t)cellLocalCoord0Q12) * Q12_ONE;
  coord0NumeratorLow = coord0PartialLow + rayZShiftedLow;
  coord0NumeratorHigh = coord0NumeratorHigh + FIXED_PRODUCT_SHR(originByDelta1, 32 - Q12_SHIFT) +
                        (uint32_t)(coord0PartialLow < crossProductLow) +
                        FIXED_MUL_SHR(rayDeltaZQ12, cellLocalCoord0Q12, 32 - Q12_SHIFT) +
                        (uint32_t)(coord0NumeratorLow < coord0PartialLow);
  if ((coord0NumeratorHigh < 0) != (planeRate < 0)) {
    return false;
  }
  crossProductLow = TerrainTriangle_MulCrossByHeightDelta
                              (rayCrossLocal,cornerHeight3Q12 - cornerHeight2Q12,&coord1NumeratorHigh);
  originByDelta0 = (int64_t)(rayOriginZQ12 - cornerHeight3Q12) * (int64_t)gridRayDelta0Q12;
  originShiftedLow = (uint32_t)originByDelta0 * Q12_ONE;
  coord1PartialLow = crossProductLow + originShiftedLow;
  rayZShiftedLow = (uint32_t)((int64_t)rayDeltaZQ12 * (int64_t)cellLocalCoord1Q12) * Q12_ONE;
  rateShiftedHigh = FIXED_PRODUCT_SHR(planeRate, 32 - Q12_SHIFT);
  rateShiftedLow = (uint32_t)planeRate * Q12_ONE;
  coord1NumeratorLow = rayZShiftedLow + coord1PartialLow;
  coord1NumeratorHigh = FIXED_MUL_SHR(rayDeltaZQ12, cellLocalCoord1Q12, 32 - Q12_SHIFT) +
                        coord1NumeratorHigh + FIXED_PRODUCT_SHR(originByDelta0, 32 - Q12_SHIFT) +
                        (uint32_t)(coord1PartialLow < crossProductLow) +
                        (uint32_t)(coord1NumeratorLow < rayZShiftedLow);
  if ((coord1NumeratorHigh < 0) != ((int)rateShiftedHigh < 0)) {
    return false;
  }
  /* third edge: plane rate minus both numerators */
  remainingLow = (rateShiftedLow - coord1NumeratorLow) - coord0NumeratorLow;
  remainingHigh = (((rateShiftedHigh - coord1NumeratorHigh) - (uint32_t)(rateShiftedLow < coord1NumeratorLow)) -
                   coord0NumeratorHigh) - (uint32_t)(rateShiftedLow - coord1NumeratorLow < coord0NumeratorLow);
  if ((coord1NumeratorHigh < 0) != (remainingHigh < 0)) {
    return false;
  }
  /* inside the first triangle: intersection distance */
  if (!TerrainTriangle_DivideHitNumerators(remainingLow,remainingHigh,coord1NumeratorLow,coord1NumeratorHigh,
                                           coord0NumeratorLow,coord0NumeratorHigh,&coord1Offset,&coord0Offset)) {
    *outDistanceQ12 = 0;
    return true;
  }
  hitCoord1 = cellLocalCoord1Q12 + coord1Offset;
  coord0HeightProduct = (int64_t)coord0Offset * (int64_t)(cornerHeight2Q12 - cornerHeight3Q12);
  coord1HeightProduct = (int64_t)coord1Offset * (int64_t)(cornerHeight1Q12 - cornerHeight3Q12);
  /* grid offsets of the hit back to world units (inverse of FIELD_GRID_WORLD_*_Q20): X = (column + row / 2)
     * FIELD_GRID_WORLD_COLUMN_STEP_X / 0x1000, Y = row * FIELD_GRID_WORLD_ROW_STEP_Y / 0x1000 */
  worldXOffsetProduct = (int64_t)(hitCoord1 + (cellLocalCoord0Q12 + coord0Offset) * 2) * FIELD_GRID_WORLD_COLUMN_STEP_X;
  worldYOffsetProduct = (int64_t)hitCoord1 * FIELD_GRID_WORLD_ROW_STEP_Y;
  *outDistanceQ12 =
       FixedMath_Length3(((cornerHeight3Q12 + (FIXED_PRODUCT_SHR(coord0HeightProduct, Q12_SHIFT))) - rayOriginZQ12) +
                         (FIXED_PRODUCT_SHR(coord1HeightProduct, Q12_SHIFT)),
                         FIXED_PRODUCT_SHR(worldYOffsetProduct, 12),
                         FIXED_PRODUCT_SHR(worldXOffsetProduct, Q12_SHIFT + 1));
  return true;
}


/* Second triangle of TerrainTriangle_IntersectRayDistance, based on the far corner 0 (local coordinates shifted by
   one cell). Returns true with the distance in *outDistanceQ12 on a hit, false on a miss. */
static bool TerrainTriangle_IntersectRayCorner0Triangle
          (Q12 rayDeltaZQ12,Q12 gridRayDelta0Q12,Q12 gridRayDelta1Q12,Q12 rayOriginZQ12,
          Q12 cornerHeight0Q12,Q12 cornerHeight1Q12,Q12 cornerHeight2Q12,
          Q12 cellLocalCoord1Q12,Q12 cellLocalCoord0Q12,Q12 *outDistanceQ12)

{
  int farCoord0;
  int farCoord1;
  int64_t planeRate;
  uint32_t rateLow;
  int rateHigh;
  int64_t planeOffset;
  int planeOffsetHigh;
  int offsetMinusRateHigh;
  uint64_t rayCrossLocal;
  uint32_t crossProductLow;
  int64_t originByDelta1;
  int64_t originByDelta0;
  uint32_t coord0PartialLow;
  uint32_t coord0NumeratorLow;
  int coord0NumeratorHigh;
  uint32_t coord1PartialLow;
  uint32_t coord1NumeratorLow;
  int coord1NumeratorHigh;
  uint32_t rayZShiftedLow;
  uint32_t rateShiftedHigh;
  uint32_t rateShiftedLow;
  uint32_t remainingLow;
  int remainingHigh;
  int coord1Offset;
  int coord0Offset;
  int hitCoord1;
  int64_t coord0HeightProduct;
  int64_t coord1HeightProduct;
  int64_t worldXOffsetProduct;
  int64_t worldYOffsetProduct;

  farCoord0 = cellLocalCoord0Q12 + FIELD_GRID_CELL_Q12;
  farCoord1 = cellLocalCoord1Q12 + FIELD_GRID_CELL_Q12;
  planeRate = (int64_t)(cornerHeight2Q12 - cornerHeight0Q12) * (int64_t)gridRayDelta0Q12 +
              (int64_t)(cornerHeight1Q12 - cornerHeight0Q12) * (int64_t)gridRayDelta1Q12 +
              ((int64_t)rayDeltaZQ12 << Q12_SHIFT);
  rateLow = (uint32_t)planeRate;
  rateHigh = (int)((uint64_t)planeRate >> 32);
  planeOffset = (int64_t)(cornerHeight2Q12 - cornerHeight0Q12) * (int64_t)farCoord1 +
                (int64_t)(cornerHeight1Q12 - cornerHeight0Q12) * (int64_t)farCoord0 +
                ((int64_t)(cornerHeight0Q12 - rayOriginZQ12) << Q12_SHIFT);
  planeOffsetHigh = (int)((uint64_t)planeOffset >> 32);
  /* the ray must cross the triangle's plane (high word of planeOffset - planeRate) */
  offsetMinusRateHigh = (int)((planeOffsetHigh - rateHigh) - (uint32_t)((uint32_t)planeOffset < rateLow));
  if (planeOffset < 0) {
    if ((planeRate >= 0) || (offsetMinusRateHigh < 0)) {
      return false;
    }
  }
  else if ((planeRate < 0) || (offsetMinusRateHigh >= 0)) {
    return false;
  }
  /* test the edges: each numerator must have the sign of the plane rate */
  rayCrossLocal = (int64_t)gridRayDelta1Q12 * (int64_t)farCoord1 - (int64_t)farCoord0 * (int64_t)gridRayDelta0Q12;
  crossProductLow = TerrainTriangle_MulCrossByHeightDelta
                              (rayCrossLocal,cornerHeight0Q12 - cornerHeight2Q12,&coord0NumeratorHigh);
  originByDelta1 = (int64_t)(rayOriginZQ12 - cornerHeight0Q12) * (int64_t)gridRayDelta1Q12;
  coord0PartialLow = crossProductLow + (uint32_t)originByDelta1 * Q12_ONE;
  rayZShiftedLow = (uint32_t)((int64_t)rayDeltaZQ12 * (int64_t)farCoord0) * Q12_ONE;
  coord0NumeratorLow = coord0PartialLow + rayZShiftedLow;
  coord0NumeratorHigh = coord0NumeratorHigh + FIXED_PRODUCT_SHR(originByDelta1, 32 - Q12_SHIFT) +
                        (uint32_t)(coord0PartialLow < crossProductLow) +
                        FIXED_MUL_SHR(rayDeltaZQ12, farCoord0, 32 - Q12_SHIFT) +
                        (uint32_t)(coord0NumeratorLow < coord0PartialLow);
  if ((coord0NumeratorHigh < 0) != (planeRate < 0)) {
    return false;
  }
  crossProductLow = TerrainTriangle_MulCrossByHeightDelta
                              (rayCrossLocal,cornerHeight1Q12 - cornerHeight0Q12,&coord1NumeratorHigh);
  originByDelta0 = (int64_t)(rayOriginZQ12 - cornerHeight0Q12) * (int64_t)gridRayDelta0Q12;
  coord1PartialLow = crossProductLow + (uint32_t)originByDelta0 * Q12_ONE;
  rayZShiftedLow = (uint32_t)((int64_t)rayDeltaZQ12 * (int64_t)farCoord1) * Q12_ONE;
  rateShiftedHigh = rateHigh << Q12_SHIFT | rateLow >> (32 - Q12_SHIFT);
  rateShiftedLow = rateLow * Q12_ONE;
  coord1NumeratorLow = rayZShiftedLow + coord1PartialLow;
  coord1NumeratorHigh = FIXED_MUL_SHR(rayDeltaZQ12, farCoord1, 32 - Q12_SHIFT) +
                        coord1NumeratorHigh + FIXED_PRODUCT_SHR(originByDelta0, 32 - Q12_SHIFT) +
                        (uint32_t)(coord1PartialLow < crossProductLow) +
                        (uint32_t)(coord1NumeratorLow < rayZShiftedLow);
  if ((coord1NumeratorHigh < 0) != ((int)rateShiftedHigh < 0)) {
    return false;
  }
  /* third edge: plane rate minus both numerators */
  remainingLow = (rateShiftedLow - coord1NumeratorLow) - coord0NumeratorLow;
  remainingHigh = (((rateShiftedHigh - coord1NumeratorHigh) - (uint32_t)(rateShiftedLow < coord1NumeratorLow)) -
                   coord0NumeratorHigh) - (uint32_t)(rateShiftedLow - coord1NumeratorLow < coord0NumeratorLow);
  if ((coord1NumeratorHigh < 0) != (remainingHigh < 0)) {
    return false;
  }
  /* inside the second triangle: intersection distance */
  if (!TerrainTriangle_DivideHitNumerators(remainingLow,remainingHigh,coord1NumeratorLow,coord1NumeratorHigh,
                                           coord0NumeratorLow,coord0NumeratorHigh,&coord1Offset,&coord0Offset)) {
    *outDistanceQ12 = 0;
    return true;
  }
  hitCoord1 = farCoord1 - coord1Offset;
  coord0HeightProduct = (int64_t)coord0Offset * (int64_t)(cornerHeight1Q12 - cornerHeight0Q12);
  coord1HeightProduct = (int64_t)coord1Offset * (int64_t)(cornerHeight2Q12 - cornerHeight0Q12);
  /* back to world units as for the first triangle */
  worldXOffsetProduct = (int64_t)(hitCoord1 + (farCoord0 - coord0Offset) * 2) * FIELD_GRID_WORLD_COLUMN_STEP_X;
  worldYOffsetProduct = (int64_t)hitCoord1 * FIELD_GRID_WORLD_ROW_STEP_Y;
  *outDistanceQ12 =
       FixedMath_Length3(((cornerHeight0Q12 + (FIXED_PRODUCT_SHR(coord0HeightProduct, Q12_SHIFT))) - rayOriginZQ12) +
                         (FIXED_PRODUCT_SHR(coord1HeightProduct, Q12_SHIFT)),
                         FIXED_PRODUCT_SHR(worldYOffsetProduct, 12),
                         FIXED_PRODUCT_SHR(worldXOffsetProduct, Q12_SHIFT + 1));
  return true;
}


/* Terrain raycast step: intersects the ray segment (grid-space delta, Z origin and Z delta, relative to the cell)
   with the two triangles of one field-grid cell given its four corner heights, first the one based on corner 3,
   then the one based on corner 0 (the far corner, local coordinates shifted by one cell). A hit returns true with
   the world distance from the ray origin in *outDistanceQ12; a miss (also the quick reject when all four corners
   lie below the ray's lowest point) returns false and leaves *outDistanceQ12 untouched.
*/
bool TerrainTriangle_IntersectRayDistance
          (Q12 rayDeltaZQ12,Q12 gridRayDelta0Q12,Q12 gridRayDelta1Q12,Q12 rayOriginZQ12,
          Q12 cornerHeight0Q12,Q12 cornerHeight1Q12,Q12 cornerHeight2Q12,Q12 cornerHeight3Q12,
          Q12 cellLocalCoord1Q12,Q12 cellLocalCoord0Q12,Q12 *outDistanceQ12)

{
  Q12 lowestRayZQ12;

  lowestRayZQ12 = rayOriginZQ12;
  if (rayDeltaZQ12 < 0) {
    lowestRayZQ12 = rayOriginZQ12 + rayDeltaZQ12;
  }
  /* quick reject: all four corners at or below the ray's lowest point */
  if ((cornerHeight3Q12 <= lowestRayZQ12) && (cornerHeight2Q12 <= lowestRayZQ12) &&
      (cornerHeight1Q12 <= lowestRayZQ12) && (cornerHeight0Q12 <= lowestRayZQ12)) {
    return false;
  }
  if (TerrainTriangle_IntersectRayCorner3Triangle(rayDeltaZQ12,gridRayDelta0Q12,gridRayDelta1Q12,rayOriginZQ12,
                                                  cornerHeight1Q12,cornerHeight2Q12,cornerHeight3Q12,
                                                  cellLocalCoord1Q12,cellLocalCoord0Q12,outDistanceQ12)) {
    return true;
  }
  return TerrainTriangle_IntersectRayCorner0Triangle(rayDeltaZQ12,gridRayDelta0Q12,gridRayDelta1Q12,rayOriginZQ12,
                                                     cornerHeight0Q12,cornerHeight1Q12,cornerHeight2Q12,
                                                     cellLocalCoord1Q12,cellLocalCoord0Q12,outDistanceQ12);
}


FieldGridCell *g_TerrainRayNextCell;
Q12 g_TerrainRayNextCoord0Q12;
Q12 g_TerrainRayNextCoord1Q12;

/* One step of the terrain raycasts' cell walk (coord0 = grid row, coord1 = grid column, both Q12): moves to the
   next row when the ray segment start..end leaves the current cell through a row boundary, otherwise to the next
   column. Returns true (CF set) when the current cell already contains the ray end or no step is possible (no
   row crossing and no column movement), false with the next cell and corner in g_TerrainRayNext* otherwise.
   On a true return g_TerrainRayNext* hold the original's registers too: coord0 (EDX) = end0 - cur0 for the
   destination-cell exit, cur0 with cell - 0x80 / cur1 - one cell for the no-column-movement exit.
*/
bool TerrainRay_AdvanceGridTraversal
          (Q12 rayEndCoord0Q12,Q12 rayEndCoord1Q12,Q12 rayStartCoord0Q12,Q12 rayStartCoord1Q12,
          FieldGridRowStrideBytes rowStrideBytes,FieldGridCell *currentCell,Q12 currentGridCoord0Q12
          ,Q12 currentGridCoord1Q12)

{
  /* Rewritten from the assembly (0x005049E0-0x00504B04). Besides CF the original returns the next
     cell in ESI and the next grid corner in EDX (coord0) / ECX (coord1); the decompiler dropped all
     three, so callers never advanced their cell. They are published in g_TerrainRayNext*. */
  uint8_t *cell = (uint8_t *)currentCell;
  int delta0;
  int delta1;

  g_TerrainRayNextCell = currentCell;
  g_TerrainRayNextCoord0Q12 = currentGridCoord0Q12;
  g_TerrainRayNextCoord1Q12 = currentGridCoord1Q12;
  delta1 = rayEndCoord1Q12 - currentGridCoord1Q12;
  delta0 = rayEndCoord0Q12 - currentGridCoord0Q12;
  /* SUB + JL (0x005049EC/0x005049F1) is a true signed compare of end and current coordinate; the JG
     checks against one cell use the wrapped differences */
  if (rayEndCoord1Q12 >= currentGridCoord1Q12 && rayEndCoord0Q12 >= currentGridCoord0Q12 &&
      delta1 <= FIELD_GRID_CELL_Q12 && delta0 <= FIELD_GRID_CELL_Q12) {
    /* already in the destination cell: STC at 0x00504A05 with EDX still end0 - cur0 from 0x005049F1
       (ESI/ECX untouched). The raycasts leave EDX as materialOrCellIndex of their miss result. */
    g_TerrainRayNextCoord0Q12 = delta0;
    return true;
  }
  delta0 = rayEndCoord0Q12 - rayStartCoord0Q12;
  if (delta0 != 0) {
    /* the column where the ray crosses the next row boundary, scaled by delta0, must lie within
       [current column, current column + one cell] */
    int64_t limit = (int64_t)delta0 * FIELD_GRID_CELL_Q12;
    int64_t side;
    if (delta0 > 0) {
      side = (int64_t)(rayEndCoord1Q12 - rayStartCoord1Q12) *
             ((currentGridCoord0Q12 + FIELD_GRID_CELL_Q12) - rayStartCoord0Q12) +
             (int64_t)(rayStartCoord1Q12 - currentGridCoord1Q12) * delta0;
      if (side >= 0 && limit - side >= 0) {
        g_TerrainRayNextCell = (FieldGridCell *)(cell + rowStrideBytes);
        g_TerrainRayNextCoord0Q12 = currentGridCoord0Q12 + FIELD_GRID_CELL_Q12;
        return false;
      }
    }
    else {
      side = (int64_t)(rayEndCoord1Q12 - rayStartCoord1Q12) *
             (currentGridCoord0Q12 - rayStartCoord0Q12) +
             (int64_t)(rayStartCoord1Q12 - currentGridCoord1Q12) * delta0;
      if (side < 0 && limit - side < 0) {
        g_TerrainRayNextCell = (FieldGridCell *)(cell - rowStrideBytes);
        g_TerrainRayNextCoord0Q12 = currentGridCoord0Q12 - FIELD_GRID_CELL_Q12;
        return false;
      }
    }
  }
  delta1 = rayEndCoord1Q12 - rayStartCoord1Q12;
  if (delta1 == 0) {
    /* STC via 0x00504AEB after 0x00504ADC/0x00504AE2 already moved ESI/ECX one column back; EDX = cur0 */
    g_TerrainRayNextCell = (FieldGridCell *)cell - 1;
    g_TerrainRayNextCoord1Q12 = currentGridCoord1Q12 - FIELD_GRID_CELL_Q12;
    return true;
  }
  /* next column */
  if (delta1 < 0) {
    g_TerrainRayNextCell = (FieldGridCell *)cell - 1;
    g_TerrainRayNextCoord1Q12 = currentGridCoord1Q12 - FIELD_GRID_CELL_Q12;
  }
  else {
    g_TerrainRayNextCell = (FieldGridCell *)cell + 1;
    g_TerrainRayNextCoord1Q12 = currentGridCoord1Q12 + FIELD_GRID_CELL_Q12;
  }
  return false;
}


/* Height-band test of one cell: true for a map-edge cell, a flooded cell (waterSurfaceDelta > 0) or a height
   outside [g_TerrainHeightBandMinimumDelta, g_TerrainHeightBandMaximumDelta] relative to
   g_TerrainScanReferenceHeight. */
static bool TerrainHeightBand_IsCellOutside(const FieldGridCell *cell)

{
  int relativeHeightQ12;

  if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
    return true;
  }
  relativeHeightQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight;
  if (0 < cell->waterSurfaceDelta) {
    return true;
  }
  return ((int)g_TerrainHeightBandMaximumDelta < relativeHeightQ12) ||
         (relativeHeightQ12 < (int)g_TerrainHeightBandMinimumDelta);
}


/* Height-band placement test, sector 0 of the hexagon (see TerrainHeightBand_TestAroundWorldPoint): walks the
   sector's diagonal, tests each diagonal cell and the cell between it and the next one, and runs the straight
   tests of directions 0 and 1 that cover the sector. Returns true (CF set) at the first cell outside the height
   band, false when the step limit is reached.
*/
bool TerrainHeightBand_TestWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int rowStrideBytes;
  FieldGridCell *directionStartCell;
  FieldGridCell *betweenCell;

  while (scanStep < g_TerrainScanStepLimit) {
    rowStrideBytes = g_TerrainScanRowStrideBytes;
    if (TerrainHeightBand_IsCellOutside(cell)) {
      return true;
    }
    directionStartCell = cell + 1;
    if (TerrainHeightBand_TestDirection0(scanStep + TERRAIN_SCAN_STEP_STRAIGHT,directionStartCell)) {
      return true;
    }
    if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
      return false;
    }
    /* the in-between cell is one row up from directionStartCell */
    betweenCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(directionStartCell,-rowStrideBytes);
    if (TerrainHeightBand_IsCellOutside(betweenCell)) {
      return true;
    }
    cell = betweenCell + 1;
    scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
    if (TerrainHeightBand_TestDirection1(scanStep,FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-g_TerrainScanRowStrideBytes))) {
      return true;
    }
  }
  return false;
}


/* Height-band placement test, sector 1: like TerrainHeightBand_TestWedge0, running the straight tests of
   directions 1 and 2.
*/
bool TerrainHeightBand_TestWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int rowStrideBytes;
  FieldGridCell *directionStartCell;

  while (scanStep < g_TerrainScanStepLimit) {
    rowStrideBytes = g_TerrainScanRowStrideBytes;
    if (TerrainHeightBand_IsCellOutside(cell)) {
      return true;
    }
    if (TerrainHeightBand_TestDirection1
              (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,
               FIELD_GRID_CELL_AT_BYTE_OFFSET(cell + 1,-g_TerrainScanRowStrideBytes))) {
      return true;
    }
    if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
      return false;
    }
    /* the in-between cell is the one above */
    if (TerrainHeightBand_IsCellOutside(FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes))) {
      return true;
    }
    /* two rows up */
    directionStartCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-g_TerrainScanRowStrideBytes - rowStrideBytes);
    scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
    cell = directionStartCell + 1;
    if (TerrainHeightBand_TestDirection2(scanStep,directionStartCell)) {
      return true;
    }
  }
  return false;
}


/* Height-band placement test, sector 2: like TerrainHeightBand_TestWedge0, running the straight tests of
   directions 2 and 3.
*/
bool TerrainHeightBand_TestWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *directionStartCell;

  while (scanStep < g_TerrainScanStepLimit) {
    if (TerrainHeightBand_IsCellOutside(cell)) {
      return true;
    }
    if (TerrainHeightBand_TestDirection2
              (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-g_TerrainScanRowStrideBytes))) {
      return true;
    }
    if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
      return false;
    }
    /* the in-between cell is the left neighbour */
    if (TerrainHeightBand_IsCellOutside(cell - 1)) {
      return true;
    }
    directionStartCell = cell - 2;
    scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
    cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell - 1,-g_TerrainScanRowStrideBytes); /* up and left */
    if (TerrainHeightBand_TestDirection3(scanStep,directionStartCell)) {
      return true;
    }
  }
  return false;
}


/* Height-band placement test, sector 3: like TerrainHeightBand_TestWedge0, running the straight tests of
   directions 3 and 4.
*/
bool TerrainHeightBand_TestWedge3(TerrainDirectionalScanStep scanStep,uint8_t *cell)

{
  int rowStrideBytes;
  int relativeHeightQ12;
  FieldGridCell *directionStartCell;
  bool directionFailed;

  if (scanStep < g_TerrainScanStepLimit) {
    do {
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      /* cell is a byte pointer here */
      if ((((((uint32_t)((FieldGridCell *)cell)->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) ||
           (0 < ((FieldGridCell *)cell)->waterSurfaceDelta)) ||
          ((int)g_TerrainHeightBandMaximumDelta <
           (int)((uint32_t)((FieldGridCell *)cell)->terrainHeight - g_TerrainScanReferenceHeight))) ||
         ((int)((uint32_t)((FieldGridCell *)cell)->terrainHeight - g_TerrainScanReferenceHeight) <
          (int)g_TerrainHeightBandMinimumDelta)) {
        return true;
      }
      directionStartCell = (FieldGridCell *)cell - 1;
      directionFailed = TerrainHeightBand_TestDirection3(scanStep + TERRAIN_SCAN_STEP_STRAIGHT,directionStartCell);
      if (directionFailed) {
        return true;
      }
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return false;
      }
      /* the in-between cell is the one below directionStartCell (one row stride on) */
      if ((FIELD_GRID_CELL_AT_BYTE_OFFSET(directionStartCell,rowStrideBytes)->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return true;
      }
      relativeHeightQ12 = FIELD_GRID_CELL_AT_BYTE_OFFSET(directionStartCell,rowStrideBytes)->terrainHeight - g_TerrainScanReferenceHeight;
      if (0 < FIELD_GRID_CELL_AT_BYTE_OFFSET(directionStartCell,rowStrideBytes)->waterSurfaceDelta) {
        return true;
      }
      if ((int)g_TerrainHeightBandMaximumDelta < relativeHeightQ12) {
        return true;
      }
      if (relativeHeightQ12 < (int)g_TerrainHeightBandMinimumDelta) {
        return true;
      }
      /* directionStartCell[-1] one row down */
      cell = (uint8_t *)(directionStartCell - 1) + rowStrideBytes;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      directionFailed = TerrainHeightBand_TestDirection4
                        (scanStep,(FieldGridCell *)(cell + g_TerrainScanRowStrideBytes));
      if (directionFailed) {
        return true;
      }
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return false;
}


/* Height-band placement test, sector 4: like TerrainHeightBand_TestWedge0, running the straight tests of
   directions 4 and 5.
*/
bool TerrainHeightBand_TestWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int rowStrideBytes;
  FieldGridCell *betweenCell;
  FieldGridCell *direction5StartCell;

  while (scanStep < g_TerrainScanStepLimit) {
    rowStrideBytes = g_TerrainScanRowStrideBytes;
    if (TerrainHeightBand_IsCellOutside(cell)) {
      return true;
    }
    if (TerrainHeightBand_TestDirection4
              (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,
               FIELD_GRID_CELL_AT_BYTE_OFFSET(cell - 1,g_TerrainScanRowStrideBytes))) {
      return true;
    }
    if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
      return false;
    }
    /* the in-between cell is the one below (one row stride on) */
    betweenCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes);
    if (TerrainHeightBand_IsCellOutside(betweenCell)) {
      return true;
    }
    scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
    /* two rows down: the direction 5 start; the next diagonal cell is one left of it */
    direction5StartCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(betweenCell,g_TerrainScanRowStrideBytes);
    cell = direction5StartCell - 1;
    if (TerrainHeightBand_TestDirection5(scanStep,direction5StartCell)) {
      return true;
    }
  }
  return false;
}


/* Height-band placement test, sector 5: like TerrainHeightBand_TestWedge0, running the straight tests of
   directions 5 and 0.
*/
bool TerrainHeightBand_TestWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *directionStartCell;

  while (scanStep < g_TerrainScanStepLimit) {
    if (TerrainHeightBand_IsCellOutside(cell)) {
      return true;
    }
    if (TerrainHeightBand_TestDirection5
              (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,
               FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,g_TerrainScanRowStrideBytes))) {
      return true;
    }
    if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
      return false;
    }
    /* the in-between cell is the right neighbour */
    if (TerrainHeightBand_IsCellOutside(cell + 1)) {
      return true;
    }
    directionStartCell = cell + 2;
    scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
    cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell + 1,g_TerrainScanRowStrideBytes); /* below the right cell */
    if (TerrainHeightBand_TestDirection0(scanStep,directionStartCell)) {
      return true;
    }
  }
  return false;
}


/* Water-surface placement test, sector 0 of the hexagon (see TerrainAuxHeightThreshold_TestAroundWorldPoint):
   walks the sector's diagonal, tests each diagonal cell and the cell between it and the next one, and runs the
   straight tests of directions 0 and 1 that cover the sector. Returns true (CF set) at the first failing cell,
   false when the step limit is reached.
*/
bool TerrainAuxHeightThreshold_TestWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int rowStrideBytes;
  FieldGridCell *directionStartCell;
  bool directionFailed;

  if (scanStep < g_TerrainScanStepLimit) {
    do {
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      if ((((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) || (cell->waterSurfaceDelta < 0)) ||
         ((int)cell->triangle1NormalAngles >> 16 < (int)g_TerrainAuxHeightMinimum)) {
        return true;
      }
      directionStartCell = cell + 1;
      directionFailed = TerrainAuxHeightThreshold_TestDirection0(scanStep + TERRAIN_SCAN_STEP_STRAIGHT,directionStartCell);
      if (directionFailed) {
        return true;
      }
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return false;
      }
      /* the in-between cell is one row up from directionStartCell (flags +0x50, height +0x48, water delta
         +0x4C, normal angles +0x78) */
      if ((FIELD_GRID_CELL_AT_BYTE_OFFSET(directionStartCell,-rowStrideBytes)->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return true;
      }
      if (FIELD_GRID_CELL_AT_BYTE_OFFSET(directionStartCell,-rowStrideBytes)->waterSurfaceDelta < 0) {
        return true;
      }
      if ((int)FIELD_GRID_CELL_AT_BYTE_OFFSET(directionStartCell,-rowStrideBytes)->triangle1NormalAngles >> 16 < (int)g_TerrainAuxHeightMinimum) {
        return true;
      }
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(directionStartCell + 1,-rowStrideBytes);
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      directionFailed = TerrainAuxHeightThreshold_TestDirection1
                        (scanStep,FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-g_TerrainScanRowStrideBytes));
      if (directionFailed) {
        return true;
      }
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return false;
}


/* Water-surface placement test, sector 1: like TerrainAuxHeightThreshold_TestWedge0, running the straight tests
   of directions 1 and 2.
*/
bool TerrainAuxHeightThreshold_TestWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int rowStrideBytes;
  FieldGridCell *directionStartCell;
  bool directionFailed;

  if (scanStep < g_TerrainScanStepLimit) {
    do {
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      if ((((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) || (cell->waterSurfaceDelta < 0)) ||
         ((int)cell->triangle1NormalAngles >> 16 < (int)g_TerrainAuxHeightMinimum)) {
        return true;
      }
      directionFailed = TerrainAuxHeightThreshold_TestDirection1
                        (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,
                         FIELD_GRID_CELL_AT_BYTE_OFFSET(cell + 1,-g_TerrainScanRowStrideBytes));
      if (directionFailed) {
        return true;
      }
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return false;
      }
      /* the in-between cell is the one above (flags +0x50, height +0x48, water delta +0x4C, normal angles +0x78) */
      if ((FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return true;
      }
      if (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)->waterSurfaceDelta < 0) {
        return true;
      }
      if ((int)FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)->triangle1NormalAngles >> 16 < (int)g_TerrainAuxHeightMinimum) {
        return true;
      }
      directionStartCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-g_TerrainScanRowStrideBytes - rowStrideBytes);
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      cell = directionStartCell + 1;
      directionFailed = TerrainAuxHeightThreshold_TestDirection2(scanStep,directionStartCell);
      if (directionFailed) {
        return true;
      }
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return false;
}


/* Water-surface placement test, sector 2: like TerrainAuxHeightThreshold_TestWedge0, running the straight tests
   of directions 2 and 3.
*/
bool TerrainAuxHeightThreshold_TestWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *directionStartCell;
  bool directionFailed;

  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) || (cell->waterSurfaceDelta < 0)) ||
         ((int)cell->triangle1NormalAngles >> 16 < (int)g_TerrainAuxHeightMinimum)) {
        return true;
      }
      directionFailed = TerrainAuxHeightThreshold_TestDirection2
                        (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-g_TerrainScanRowStrideBytes));
      if (directionFailed) {
        return true;
      }
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return false;
      }
      if ((cell[-1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return true;
      }
      if (cell[-1].waterSurfaceDelta < 0) {
        return true;
      }
      if ((int)cell[-1].triangle1NormalAngles >> 16 < (int)g_TerrainAuxHeightMinimum) {
        return true;
      }
      directionStartCell = cell - 2;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell - 1,-g_TerrainScanRowStrideBytes);
      directionFailed = TerrainAuxHeightThreshold_TestDirection3(scanStep,directionStartCell);
      if (directionFailed) {
        return true;
      }
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return false;
}


/* Water-surface placement test, sector 3: like TerrainAuxHeightThreshold_TestWedge0, running the straight tests
   of directions 3 and 4.
*/
bool TerrainAuxHeightThreshold_TestWedge3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int rowStrideBytes;
  FieldGridCell *directionStartCell;
  bool directionFailed;

  if (scanStep < g_TerrainScanStepLimit) {
    do {
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      if ((((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) || (cell->waterSurfaceDelta < 0)) ||
         ((int)cell->triangle1NormalAngles >> 16 < (int)g_TerrainAuxHeightMinimum)) {
        return true;
      }
      directionStartCell = cell - 1;
      directionFailed = TerrainAuxHeightThreshold_TestDirection3(scanStep + TERRAIN_SCAN_STEP_STRAIGHT,directionStartCell);
      if (directionFailed) {
        return true;
      }
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return false;
      }
      /* the in-between cell is the one below directionStartCell (one row stride on) */
      if ((FIELD_GRID_CELL_AT_BYTE_OFFSET(directionStartCell,rowStrideBytes)->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return true;
      }
      if (FIELD_GRID_CELL_AT_BYTE_OFFSET(directionStartCell,rowStrideBytes)->waterSurfaceDelta < 0) {
        return true;
      }
      if ((int)FIELD_GRID_CELL_AT_BYTE_OFFSET(directionStartCell,rowStrideBytes)->triangle1NormalAngles >> 16 < (int)g_TerrainAuxHeightMinimum) {
        return true;
      }
      /* directionStartCell[-1] one row down */
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(directionStartCell - 1,rowStrideBytes);
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      directionFailed = TerrainAuxHeightThreshold_TestDirection4
                        (scanStep,FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,g_TerrainScanRowStrideBytes));
      if (directionFailed) {
        return true;
      }
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return false;
}


/* Water-surface placement test, sector 4: like TerrainAuxHeightThreshold_TestWedge0, running the straight tests
   of directions 4 and 5.
*/
bool TerrainAuxHeightThreshold_TestWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  uint8_t *cellBytes;
  int rowStrideBytes;
  bool directionFailed;

  if (scanStep < g_TerrainScanStepLimit) {
    do {
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      if ((((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) || (cell->waterSurfaceDelta < 0)) ||
         ((int)cell->triangle1NormalAngles >> 16 < (int)g_TerrainAuxHeightMinimum)) {
        return true;
      }
      directionFailed = TerrainAuxHeightThreshold_TestDirection4
                        (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,
                         FIELD_GRID_CELL_AT_BYTE_OFFSET(cell - 1,g_TerrainScanRowStrideBytes));
      if (directionFailed) {
        return true;
      }
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return false;
      }
      /* the in-between cell is the one below (one row stride on) */
      if ((FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return true;
      }
      if (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)->waterSurfaceDelta < 0) {
        return true;
      }
      if ((int)FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)->triangle1NormalAngles >> 16 < (int)g_TerrainAuxHeightMinimum) {
        return true;
      }
      /* two rows down: the next diagonal cell one left of the direction 5 start */
      cellBytes = (uint8_t *)cell;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      cell = (FieldGridCell *)(cellBytes + g_TerrainScanRowStrideBytes + rowStrideBytes) - 1;
      directionFailed = TerrainAuxHeightThreshold_TestDirection5
                        (scanStep,(FieldGridCell *)
                                  (cellBytes + g_TerrainScanRowStrideBytes + rowStrideBytes));
      if (directionFailed) {
        return true;
      }
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return false;
}


/* Water-surface placement test, sector 5: like TerrainAuxHeightThreshold_TestWedge0, running the straight tests
   of directions 5 and 0.
*/
bool TerrainAuxHeightThreshold_TestWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *directionStartCell;
  bool directionFailed;

  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) || (cell->waterSurfaceDelta < 0)) ||
         ((int)cell->triangle1NormalAngles >> 16 < (int)g_TerrainAuxHeightMinimum)) {
        return true;
      }
      directionFailed = TerrainAuxHeightThreshold_TestDirection5
                        (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,
                         FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,g_TerrainScanRowStrideBytes))
      ;
      if (directionFailed) {
        return true;
      }
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return false;
      }
      if ((cell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return true;
      }
      if (cell[1].waterSurfaceDelta < 0) {
        return true;
      }
      if ((int)cell[1].triangle1NormalAngles >> 16 < (int)g_TerrainAuxHeightMinimum) {
        return true;
      }
      directionStartCell = cell + 2;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell + 1,g_TerrainScanRowStrideBytes);
      directionFailed = TerrainAuxHeightThreshold_TestDirection0(scanStep,directionStartCell);
      if (directionFailed) {
        return true;
      }
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return false;
}


/* Height-band placement test, straight leg along direction 0 (C+1, right): returns true (CF set) at the first
   cell that is a map-edge cell, lies under water (waterSurfaceDelta > 0) or whose height relative to
   g_TerrainScanReferenceHeight leaves [g_TerrainHeightBandMinimumDelta, g_TerrainHeightBandMaximumDelta]; false
   once the step limit is reached (4 scan steps per cell).
*/
bool TerrainHeightBand_TestDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int terrainHeightDeltaQ12;

  while (scanStep < g_TerrainScanStepLimit) {
    if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
      return true;
    }
    terrainHeightDeltaQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight;
    if (0 < cell->waterSurfaceDelta) {
      return true;
    }
    if (((int)g_TerrainHeightBandMaximumDelta < terrainHeightDeltaQ12) ||
        (terrainHeightDeltaQ12 < (int)g_TerrainHeightBandMinimumDelta)) {
      return true;
    }
    scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
    cell++;
  }
  return false;
}


/* Height-band placement test, straight leg along direction 1 (C+1-W, up and right); see TerrainHeightBand_TestDirection0.
*/
bool TerrainHeightBand_TestDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int terrainHeightDeltaQ12;

  while (g_TerrainScanStepLimit > scanStep) {
    if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
      return true;
    }
    terrainHeightDeltaQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight;
    if ((0 < cell->waterSurfaceDelta) || ((int)g_TerrainHeightBandMaximumDelta < terrainHeightDeltaQ12) ||
        (terrainHeightDeltaQ12 < (int)g_TerrainHeightBandMinimumDelta)) {
      return true;
    }
    scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
    cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell + 1,-g_TerrainScanRowStrideBytes);
  }
  return false;
}


/* Height-band placement test, straight leg along direction 2 (C-W, up); see TerrainHeightBand_TestDirection0.
*/
bool TerrainHeightBand_TestDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int terrainHeightDeltaQ12;

  while (g_TerrainScanStepLimit > scanStep) {
    if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
      return true;
    }
    terrainHeightDeltaQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight;
    if ((0 < cell->waterSurfaceDelta) || ((int)g_TerrainHeightBandMaximumDelta < terrainHeightDeltaQ12) ||
        (terrainHeightDeltaQ12 < (int)g_TerrainHeightBandMinimumDelta)) {
      return true;
    }
    scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
    cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-g_TerrainScanRowStrideBytes);
  }
  return false;
}


/* Height-band placement test, straight leg along direction 3 (C-1, left); see TerrainHeightBand_TestDirection0.
*/
bool TerrainHeightBand_TestDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int terrainHeightDeltaQ12;

  while (g_TerrainScanStepLimit > scanStep) {
    if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
      return true;
    }
    terrainHeightDeltaQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight;
    if ((0 < cell->waterSurfaceDelta) || ((int)g_TerrainHeightBandMaximumDelta < terrainHeightDeltaQ12) ||
        (terrainHeightDeltaQ12 < (int)g_TerrainHeightBandMinimumDelta)) {
      return true;
    }
    scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
    cell--;
  }
  return false;
}


/* Height-band placement test, straight leg along direction 4 (C-1+W, down and left); see TerrainHeightBand_TestDirection0.
*/
bool TerrainHeightBand_TestDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int terrainHeightDeltaQ12;

  while (g_TerrainScanStepLimit > scanStep) {
    if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
      return true;
    }
    terrainHeightDeltaQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight;
    if ((0 < cell->waterSurfaceDelta) || ((int)g_TerrainHeightBandMaximumDelta < terrainHeightDeltaQ12) ||
        (terrainHeightDeltaQ12 < (int)g_TerrainHeightBandMinimumDelta)) {
      return true;
    }
    scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
    cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell - 1,g_TerrainScanRowStrideBytes);
  }
  return false;
}


/* Height-band placement test, straight leg along direction 5 (C+W, down); see TerrainHeightBand_TestDirection0.
*/
bool TerrainHeightBand_TestDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int terrainHeightDeltaQ12;

  while (g_TerrainScanStepLimit > scanStep) {
    if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
      return true;
    }
    terrainHeightDeltaQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight;
    if ((0 < cell->waterSurfaceDelta) || ((int)g_TerrainHeightBandMaximumDelta < terrainHeightDeltaQ12) ||
        (terrainHeightDeltaQ12 < (int)g_TerrainHeightBandMinimumDelta)) {
      return true;
    }
    scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
    cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,g_TerrainScanRowStrideBytes);
  }
  return false;
}


/* Water-surface placement test, straight leg along direction 0 (C+1, right): returns true (CF set) at the first
   cell that is a map-edge cell, has a negative waterSurfaceDelta or whose triangle1NormalAngles high word is below
   g_TerrainAuxHeightMinimum; false once the step limit is reached (4 scan steps per cell).
*/
bool TerrainAuxHeightThreshold_TestDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  while (g_TerrainScanStepLimit > scanStep) {
    if (((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) || (cell->waterSurfaceDelta < 0) ||
        ((int)cell->triangle1NormalAngles >> 16 < (int)g_TerrainAuxHeightMinimum)) {
      return true;
    }
    scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
    cell++;
  }
  return false;
}


/* Water-surface placement test, straight leg along direction 1 (C+1-W, up and right); see
   TerrainAuxHeightThreshold_TestDirection0.
*/
bool TerrainAuxHeightThreshold_TestDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  while (g_TerrainScanStepLimit > scanStep) {
    if (((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) || (cell->waterSurfaceDelta < 0) ||
        ((int)cell->triangle1NormalAngles >> 16 < (int)g_TerrainAuxHeightMinimum)) {
      return true;
    }
    scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
    cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell + 1,-g_TerrainScanRowStrideBytes);
  }
  return false;
}


/* Water-surface placement test, straight leg along direction 2 (C-W, up); see
   TerrainAuxHeightThreshold_TestDirection0.
*/
bool TerrainAuxHeightThreshold_TestDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  while (g_TerrainScanStepLimit > scanStep) {
    if (((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) || (cell->waterSurfaceDelta < 0) ||
        ((int)cell->triangle1NormalAngles >> 16 < (int)g_TerrainAuxHeightMinimum)) {
      return true;
    }
    scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
    cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-g_TerrainScanRowStrideBytes);
  }
  return false;
}


/* Water-surface placement test, straight leg along direction 3 (C-1, left); see
   TerrainAuxHeightThreshold_TestDirection0.
*/
bool TerrainAuxHeightThreshold_TestDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  while (g_TerrainScanStepLimit > scanStep) {
    if (((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) || (cell->waterSurfaceDelta < 0) ||
        ((int)cell->triangle1NormalAngles >> 16 < (int)g_TerrainAuxHeightMinimum)) {
      return true;
    }
    scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
    cell--;
  }
  return false;
}


/* Water-surface placement test, straight leg along direction 4 (C-1+W, down and left); see
   TerrainAuxHeightThreshold_TestDirection0.
*/
bool TerrainAuxHeightThreshold_TestDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  while (g_TerrainScanStepLimit > scanStep) {
    if (((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) || (cell->waterSurfaceDelta < 0) ||
        ((int)cell->triangle1NormalAngles >> 16 < (int)g_TerrainAuxHeightMinimum)) {
      return true;
    }
    scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
    cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell - 1,g_TerrainScanRowStrideBytes);
  }
  return false;
}


/* Water-surface placement test, straight leg along direction 5 (C+W, down); see
   TerrainAuxHeightThreshold_TestDirection0.
*/
bool TerrainAuxHeightThreshold_TestDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  while (g_TerrainScanStepLimit > scanStep) {
    if (((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) || (cell->waterSurfaceDelta < 0) ||
        ((int)cell->triangle1NormalAngles >> 16 < (int)g_TerrainAuxHeightMinimum)) {
      return true;
    }
    scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
    cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,g_TerrainScanRowStrideBytes);
  }
  return false;
}


/* Flatten brush, straight leg along direction 0 (C+1, right): levels each cell to g_TerrainScanReferenceHeight
   and takes the change out of waterSurfaceDelta so the water surface stays where it was. 4 scan steps per cell,
   until the step limit or a map-edge cell.
*/
void TerrainHeightDelta_ApplyDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int heightAdjustmentQ12;

  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      heightAdjustmentQ12 = g_TerrainScanReferenceHeight - cell->terrainHeight;
      cell->terrainHeight = cell->terrainHeight + heightAdjustmentQ12;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - heightAdjustmentQ12;
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      cell++;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Flatten brush, straight leg along direction 1 (C+1-W, up and right): levels each cell to
   g_TerrainScanReferenceHeight, keeping the water surface (see TerrainHeightDelta_ApplyDirection0).
*/
void TerrainHeightDelta_ApplyDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int heightAdjustmentQ12;

  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      heightAdjustmentQ12 = g_TerrainScanReferenceHeight - cell->terrainHeight;
      cell->terrainHeight = cell->terrainHeight + heightAdjustmentQ12;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - heightAdjustmentQ12;
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell + 1,-g_TerrainScanRowStrideBytes); /* 0x80 = one cell */
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Flatten brush, straight leg along direction 2 (C-W, up): levels each cell to g_TerrainScanReferenceHeight,
   keeping the water surface (see TerrainHeightDelta_ApplyDirection0).
*/
void TerrainHeightDelta_ApplyDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int heightAdjustmentQ12;

  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      heightAdjustmentQ12 = g_TerrainScanReferenceHeight - cell->terrainHeight;
      cell->terrainHeight = cell->terrainHeight + heightAdjustmentQ12;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - heightAdjustmentQ12;
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Flatten brush, straight leg along direction 3 (C-1, left): levels each cell to g_TerrainScanReferenceHeight,
   keeping the water surface (see TerrainHeightDelta_ApplyDirection0).
*/
void TerrainHeightDelta_ApplyDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int heightAdjustmentQ12;

  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      heightAdjustmentQ12 = g_TerrainScanReferenceHeight - cell->terrainHeight;
      cell->terrainHeight = cell->terrainHeight + heightAdjustmentQ12;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - heightAdjustmentQ12;
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      cell--;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Flatten brush, straight leg along direction 4 (C-1+W, down and left): levels each cell to
   g_TerrainScanReferenceHeight, keeping the water surface (see TerrainHeightDelta_ApplyDirection0).
*/
void TerrainHeightDelta_ApplyDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int heightAdjustmentQ12;

  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      heightAdjustmentQ12 = g_TerrainScanReferenceHeight - cell->terrainHeight;
      cell->terrainHeight = cell->terrainHeight + heightAdjustmentQ12;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - heightAdjustmentQ12;
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell - 1,g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Flatten brush, straight leg along direction 5 (C+W, down): levels each cell to g_TerrainScanReferenceHeight,
   keeping the water surface (see TerrainHeightDelta_ApplyDirection0).
*/
void TerrainHeightDelta_ApplyDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int heightAdjustmentQ12;

  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      heightAdjustmentQ12 = g_TerrainScanReferenceHeight - cell->terrainHeight;
      cell->terrainHeight = cell->terrainHeight + heightAdjustmentQ12;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - heightAdjustmentQ12;
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}

