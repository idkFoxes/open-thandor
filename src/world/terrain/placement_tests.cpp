/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/placement_tests.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/terrain/placement_tests.h>
#include <thandor/thandor.h>

/* Module data. */

static const int32_t g_TerrainHeightBandMaximumDelta = 1024;

static const int32_t g_TerrainHeightBandMinimumDelta = -1024;

/* int32_t minimum (triangle1NormalAngles >> 16) for the auxiliary height/placement scans in world/terrain/height.c (0x3000) */
static const int32_t g_TerrainAuxHeightMinimum = 12288;

/* Implementation ownership: world/terrain/placement_tests. */

/* Shared set-up of the two hexagon placement tests below: sets g_TerrainScanStepLimit (radius /
   TERRAIN_SCAN_RADIUS_PER_STEP clamped to 1..TERRAIN_SCAN_STEP_LIMIT_MAX), g_TerrainScanReferenceHeight and
   g_TerrainScanRowStrideBytes, and maps the world point to the field cell containing it. Returns false when that
   cell lies off the grid, otherwise true with the cell's index in *outCenterCellIndex. */
static Bool8 TerrainScan_BeginAroundWorldPoint
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
   cell and checks the hexagon of radius radiusWorldUnits around it. Returns true (rejected) when a cell is
   a map-edge cell, lies under water, or its height relative to referenceHeightQ12 leaves
   [g_TerrainHeightBandMinimumDelta, g_TerrainHeightBandMaximumDelta]; also when fieldGrid is NULL or the point is
   off the grid.
*/
Bool8 TerrainHeightBand_TestAroundWorldPoint
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
   checks the hexagon of radius radiusWorldUnits around it. Returns true (rejected) when a cell is a map-edge
   cell, has a negative waterSurfaceDelta, or the high word of its packed normal angles is below
   g_TerrainAuxHeightMinimum; also when fieldGrid is NULL or the point is off the grid.
*/
Bool8 TerrainAuxHeightThreshold_TestAroundWorldPoint
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
  /* the centre cell's threshold test reads triangle0NormalAngles; the sector tests read
     triangle1NormalAngles. Both as in the original. */
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















/* Height-band test of one cell: true for a map-edge cell, a flooded cell (waterSurfaceDelta > 0) or a height
   outside [g_TerrainHeightBandMinimumDelta, g_TerrainHeightBandMaximumDelta] relative to
   g_TerrainScanReferenceHeight. */
static Bool8 TerrainHeightBand_IsCellOutside(const FieldGridCell *cell)

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
   tests of directions 0 and 1 that cover the sector. Returns true at the first cell outside the height
   band, false when the step limit is reached.
*/
Bool8 TerrainHeightBand_TestWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

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
Bool8 TerrainHeightBand_TestWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

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
Bool8 TerrainHeightBand_TestWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

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
Bool8 TerrainHeightBand_TestWedge3(TerrainDirectionalScanStep scanStep,uint8_t *cell)

{
  int rowStrideBytes;
  int relativeHeightQ12;
  FieldGridCell *directionStartCell;
  Bool8 directionFailed;

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
Bool8 TerrainHeightBand_TestWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

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
Bool8 TerrainHeightBand_TestWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

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
   straight tests of directions 0 and 1 that cover the sector. Returns true at the first failing cell,
   false when the step limit is reached.
*/
Bool8 TerrainAuxHeightThreshold_TestWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int rowStrideBytes;
  FieldGridCell *directionStartCell;
  Bool8 directionFailed;

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
      /* the in-between cell is one row up from directionStartCell (flagsAndMaterial, terrainHeight,
         waterSurfaceDelta, triangle1NormalAngles) */
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
Bool8 TerrainAuxHeightThreshold_TestWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int rowStrideBytes;
  FieldGridCell *directionStartCell;
  Bool8 directionFailed;

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
      /* the in-between cell is the one above (flagsAndMaterial, terrainHeight, waterSurfaceDelta,
         triangle1NormalAngles) */
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
Bool8 TerrainAuxHeightThreshold_TestWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *directionStartCell;
  Bool8 directionFailed;

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
Bool8 TerrainAuxHeightThreshold_TestWedge3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int rowStrideBytes;
  FieldGridCell *directionStartCell;
  Bool8 directionFailed;

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
Bool8 TerrainAuxHeightThreshold_TestWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  uint8_t *cellBytes;
  int rowStrideBytes;
  Bool8 directionFailed;

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
Bool8 TerrainAuxHeightThreshold_TestWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *directionStartCell;
  Bool8 directionFailed;

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


/* Height-band placement test, straight leg along direction 0 (C+1, right): returns true at the first
   cell that is a map-edge cell, lies under water (waterSurfaceDelta > 0) or whose height relative to
   g_TerrainScanReferenceHeight leaves [g_TerrainHeightBandMinimumDelta, g_TerrainHeightBandMaximumDelta]; false
   once the step limit is reached (4 scan steps per cell).
*/
Bool8 TerrainHeightBand_TestDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

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
Bool8 TerrainHeightBand_TestDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

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
Bool8 TerrainHeightBand_TestDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

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
Bool8 TerrainHeightBand_TestDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

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
Bool8 TerrainHeightBand_TestDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

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
Bool8 TerrainHeightBand_TestDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

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


/* Water-surface placement test, straight leg along direction 0 (C+1, right): returns true at the first
   cell that is a map-edge cell, has a negative waterSurfaceDelta or whose triangle1NormalAngles high word is below
   g_TerrainAuxHeightMinimum; false once the step limit is reached (4 scan steps per cell).
*/
Bool8 TerrainAuxHeightThreshold_TestDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

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
Bool8 TerrainAuxHeightThreshold_TestDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

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
Bool8 TerrainAuxHeightThreshold_TestDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

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
Bool8 TerrainAuxHeightThreshold_TestDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

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
Bool8 TerrainAuxHeightThreshold_TestDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

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
Bool8 TerrainAuxHeightThreshold_TestDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

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








/* Class vtables. */

const TerrainClassPlacementAndOverlayCallbackTable10 g_TerrainClassPlacementAndOverlayCallbacks10 = {
    .placementTests = {
        /* 0 */ THANDOR_FN(TerrainHeightBand_TestAroundWorldPoint),
        /* 1 */ THANDOR_FN(TerrainAuxHeightThreshold_TestAroundWorldPoint),
        /* 2 */ THANDOR_FN(TerrainHeightBand_TestAroundWorldPoint),
        /* 3 */ THANDOR_FN(TerrainHeightBand_TestAroundWorldPoint),
        /* 4 */ THANDOR_FN(TerrainHeightBand_TestAroundWorldPoint)
    },
    .overlayCallbacks = {
        /* 0 */ THANDOR_FN(FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint),
        /* 1 */ THANDOR_FN(FieldGridTerrainOverlayVariantB_ApplyAroundWorldPoint),
        /* 2 */ THANDOR_FN(FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint),
        /* 3 */ THANDOR_FN(FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint),
        /* 4 */ THANDOR_FN(FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint)
    }};
