/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/height.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/terrain/height.h>
#include <thandor/thandor.h>

/* Implementation ownership: world/terrain/height. */

/* Address: 0x00508000.
   Terrain placement test for every terrain class except 1 (g_TerrainClassPlacementAndOverlayCallbacks10
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
  uint32_t fractionSumOrGridWidth;
  uint32_t baseColumn;
  uint32_t columnFractionQ12;
  int relativeHeightQ12;
  uint32_t rowFractionQ12;
  int centerCellIndex;
  FieldGridCell *wedgeCell;
  FieldGridCell *otherWedgeCell;
  bool wedgeBlocked;
  FieldGridCoordinatesEaxEdx8 gridCoordinates;
  uint32_t cellRow;
  uint32_t cellColumn;

  if (fieldGrid != NULL) {
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
    columnFractionQ12 = (uint32_t)(THANDOR_BITCAST(FieldGridCoordinatesEaxEdx8, uint64_t, gridCoordinates) & 0xfff00000fff);
    rowFractionQ12 = (uint32_t)((THANDOR_BITCAST(FieldGridCoordinatesEaxEdx8, uint64_t, gridCoordinates) & 0xfff00000fff) >> 32);
    fractionSumOrGridWidth = rowFractionQ12 + columnFractionQ12 * 2;
    cellColumn = baseColumn;
    if (fractionSumOrGridWidth < 0x1000) {
      if (0xfff < columnFractionQ12 + rowFractionQ12 * 2) {
        cellRow++;
      }
    }
    else if (fractionSumOrGridWidth < 0x2001) {
      cellColumn = baseColumn + 1;
      if (columnFractionQ12 < rowFractionQ12) {
        cellRow++;
        cellColumn = baseColumn;
      }
    }
    else {
      cellColumn = baseColumn + 1;
      if (0x1fff < columnFractionQ12 + rowFractionQ12 * 2) {
        cellRow++;
      }
    }
    fractionSumOrGridWidth = fieldGrid->gridWidth;
    g_TerrainScanRowStrideBytes = fractionSumOrGridWidth * 0x80;
    if ((((-1 < (int)cellColumn) && (-1 < (int)cellRow)) && (cellRow < fieldGrid->gridHeight)) &&
       (cellColumn < (fractionSumOrGridWidth & 0x1ffffff))) {
      centerCellIndex = cellRow * (fractionSumOrGridWidth & 0x1ffffff) + cellColumn;
      if ((((fieldGrid->cells[centerCellIndex].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) &&
          (relativeHeightQ12 = fieldGrid->cells[centerCellIndex].terrainHeight - g_TerrainScanReferenceHeight,
          fieldGrid->cells[centerCellIndex].waterSurfaceDelta < 1)) &&
         ((relativeHeightQ12 <= (int)g_TerrainHeightBandMaximumDelta && ((int)g_TerrainHeightBandMinimumDelta <= relativeHeightQ12))))
      {
        /* sector n starts at the centre's neighbour in direction n (W = grid width):
           C+1, C+1-W, C-W, C-1, C-1+W, C+W */
        otherWedgeCell = &fieldGrid->cells[centerCellIndex + 1];
        wedgeCell = otherWedgeCell - fractionSumOrGridWidth;
        wedgeBlocked = TerrainHeightBand_TestWedge0(0,otherWedgeCell);
        if (!wedgeBlocked) {
          otherWedgeCell = wedgeCell - 1;
          wedgeBlocked = TerrainHeightBand_TestWedge1(0,wedgeCell);
          if (!wedgeBlocked) {
            wedgeCell = otherWedgeCell + (fractionSumOrGridWidth - 1);
            wedgeBlocked = TerrainHeightBand_TestWedge2(0,otherWedgeCell);
            if (!wedgeBlocked) {
              wedgeBlocked = TerrainHeightBand_TestWedge3(0,(uint8_t *)wedgeCell);
              if (!wedgeBlocked) {
                wedgeBlocked = TerrainHeightBand_TestWedge4(0,wedgeCell + fractionSumOrGridWidth);
                if ((!wedgeBlocked) && (wedgeBlocked = TerrainHeightBand_TestWedge5(0,wedgeCell + fractionSumOrGridWidth + 1), !wedgeBlocked)
                   ) {
                  return false;
                }
              }
            }
          }
        }
      }
    }
  }
  return true;
}


/* Address: 0x00508920.
   Terrain placement test for terrain class 1 / water-surface contact (g_TerrainClassPlacementAndOverlayCallbacks10
   .placementTests[1], also called directly by the army placement code): maps the world point to its field cell and
   checks the hexagon of radius radiusWorldUnits around it. Returns true (CF set, rejected) when a cell is a map-edge
   cell, has a negative waterSurfaceDelta, or the high word of its packed normal angles is below
   g_TerrainAuxHeightMinimum; also when fieldGrid is NULL or the point is off the grid.
*/
bool TerrainAuxHeightThreshold_TestAroundWorldPoint
          (FieldGridRadiusUnits radiusWorldUnits,Q12 referenceHeightQ12,Q12 worldXQ12,Q12 worldYQ12,
          FieldGridAsset *fieldGrid)

{
  uint32_t fractionSumOrGridWidth;
  uint32_t baseColumn;
  uint32_t columnFractionQ12;
  uint32_t rowFractionQ12;
  int centerCellIndex;
  FieldGridCell *wedgeCell;
  FieldGridCell *otherWedgeCell;
  bool wedgeBlocked;
  FieldGridCoordinatesEaxEdx8 gridCoordinates;
  uint32_t cellRow;
  uint32_t cellColumn;

  if (fieldGrid != NULL) {
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
    columnFractionQ12 = (uint32_t)(THANDOR_BITCAST(FieldGridCoordinatesEaxEdx8, uint64_t, gridCoordinates) & 0xfff00000fff);
    rowFractionQ12 = (uint32_t)((THANDOR_BITCAST(FieldGridCoordinatesEaxEdx8, uint64_t, gridCoordinates) & 0xfff00000fff) >> 32);
    fractionSumOrGridWidth = rowFractionQ12 + columnFractionQ12 * 2;
    cellColumn = baseColumn;
    if (fractionSumOrGridWidth < 0x1000) {
      if (0xfff < columnFractionQ12 + rowFractionQ12 * 2) {
        cellRow++;
      }
    }
    else if (fractionSumOrGridWidth < 0x2001) {
      cellColumn = baseColumn + 1;
      if (columnFractionQ12 < rowFractionQ12) {
        cellRow++;
        cellColumn = baseColumn;
      }
    }
    else {
      cellColumn = baseColumn + 1;
      if (0x1fff < columnFractionQ12 + rowFractionQ12 * 2) {
        cellRow++;
      }
    }
    fractionSumOrGridWidth = fieldGrid->gridWidth;
    g_TerrainScanRowStrideBytes = fractionSumOrGridWidth * 0x80;
    if ((((-1 < (int)cellColumn) && (-1 < (int)cellRow)) && (cellRow < fieldGrid->gridHeight)) &&
       (cellColumn < (fractionSumOrGridWidth & 0x1ffffff))) {
      centerCellIndex = cellRow * (fractionSumOrGridWidth & 0x1ffffff) + cellColumn;
      /* the centre cell's threshold test reads triangle0NormalAngles (+0x08); the sector tests read
         triangle1NormalAngles (+0x78). Both as in the original. */
      if ((((fieldGrid->cells[centerCellIndex].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) &&
          (-1 < fieldGrid->cells[centerCellIndex].waterSurfaceDelta)) &&
         ((int)g_TerrainAuxHeightMinimum <= (int)fieldGrid->cells[centerCellIndex].triangle0NormalAngles >> 16))
      {
        /* sector n starts at the centre's neighbour in direction n (W = grid width):
           C+1, C+1-W, C-W, C-1, C-1+W, C+W */
        otherWedgeCell = &fieldGrid->cells[centerCellIndex + 1];
        wedgeCell = otherWedgeCell - fractionSumOrGridWidth;
        wedgeBlocked = TerrainAuxHeightThreshold_TestWedge0(0,otherWedgeCell);
        if (!wedgeBlocked) {
          otherWedgeCell = wedgeCell - 1;
          wedgeBlocked = TerrainAuxHeightThreshold_TestWedge1(0,wedgeCell);
          if (!wedgeBlocked) {
            wedgeCell = otherWedgeCell + (fractionSumOrGridWidth - 1);
            wedgeBlocked = TerrainAuxHeightThreshold_TestWedge2(0,otherWedgeCell);
            if (!wedgeBlocked) {
              wedgeBlocked = TerrainAuxHeightThreshold_TestWedge3(0,wedgeCell);
              if (!wedgeBlocked) {
                wedgeBlocked = TerrainAuxHeightThreshold_TestWedge4(0,wedgeCell + fractionSumOrGridWidth);
                if ((!wedgeBlocked) &&
                   (wedgeBlocked = TerrainAuxHeightThreshold_TestWedge5(0,wedgeCell + fractionSumOrGridWidth + 1), !wedgeBlocked)) {
                  return false;
                }
              }
            }
          }
        }
      }
    }
  }
  return true;
}


/* Address: 0x00508D20.
   Flatten brush, sector 0 of the hexagon around the brush vertex
   (FieldGrid_ApplyHeightAtWorldPointAndRefreshNeighbors): walks the sector's diagonal, levels each cell and the one between it and the next diagonal cell to
   g_TerrainScanReferenceHeight (the removed height goes into waterSurfaceDelta, so the water surface stays), and
   starts the straight scans of directions 0 and 1 that fill the sector. Stops at a map-edge cell or the radius.
*/
void TerrainHeightDelta_ApplyWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int *adjacentWaterDeltaField;
  int heightAdjustmentOrRowStride;
  int adjacentHeightAdjustmentQ12;
  FieldGridCell *directionStartCell;
  int *adjacentHeightField;

  if (scanStep < g_TerrainScanStepLimit) {
    while ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      heightAdjustmentOrRowStride = g_TerrainScanReferenceHeight - cell->terrainHeight;
      cell->terrainHeight = cell->terrainHeight + heightAdjustmentOrRowStride;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - heightAdjustmentOrRowStride;
      heightAdjustmentOrRowStride = g_TerrainScanRowStrideBytes;
      directionStartCell = cell + 1;
      TerrainHeightDelta_ApplyDirection0(scanStep + TERRAIN_SCAN_STEP_STRAIGHT,directionStartCell);
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      /* the in-between cell is one row up from directionStartCell */
      if ((FIELD_GRID_CELL_AT_BYTE_OFFSET(directionStartCell,-heightAdjustmentOrRowStride)->flagsAndMaterial &
           FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      adjacentHeightAdjustmentQ12 =
           g_TerrainScanReferenceHeight -
           FIELD_GRID_CELL_AT_BYTE_OFFSET(directionStartCell,-heightAdjustmentOrRowStride)->terrainHeight;
      adjacentHeightField =
           &FIELD_GRID_CELL_AT_BYTE_OFFSET(directionStartCell,-heightAdjustmentOrRowStride)->terrainHeight;
      *adjacentHeightField = *adjacentHeightField + adjacentHeightAdjustmentQ12;
      adjacentWaterDeltaField =
           &FIELD_GRID_CELL_AT_BYTE_OFFSET(directionStartCell,-heightAdjustmentOrRowStride)->waterSurfaceDelta;
      *adjacentWaterDeltaField = *adjacentWaterDeltaField - adjacentHeightAdjustmentQ12;
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(directionStartCell + 1,-heightAdjustmentOrRowStride);
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      TerrainHeightDelta_ApplyDirection1
                (scanStep,FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
}


/* Address: 0x00508DC0.
   Flatten brush, sector 1: like TerrainHeightDelta_ApplyWedge0, levelling the sector's diagonal and starting the
   straight scans of directions 1 and 2.
*/
void TerrainHeightDelta_ApplyWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int *adjacentWaterDeltaField;
  int heightAdjustmentOrRowStride;
  int adjacentHeightAdjustmentQ12;
  FieldGridCell *directionStartCell;
  int *adjacentHeightField;

  if (scanStep < g_TerrainScanStepLimit) {
    while ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      heightAdjustmentOrRowStride = g_TerrainScanReferenceHeight - cell->terrainHeight;
      cell->terrainHeight = cell->terrainHeight + heightAdjustmentOrRowStride;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - heightAdjustmentOrRowStride;
      heightAdjustmentOrRowStride = g_TerrainScanRowStrideBytes;
      TerrainHeightDelta_ApplyDirection1
                (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,
                 FIELD_GRID_CELL_AT_BYTE_OFFSET(cell + 1,-g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      /* the in-between cell is the one above */
      if ((FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-heightAdjustmentOrRowStride)->flagsAndMaterial &
           FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      adjacentHeightAdjustmentQ12 =
           g_TerrainScanReferenceHeight - FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-heightAdjustmentOrRowStride)->terrainHeight;
      adjacentHeightField = &FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-heightAdjustmentOrRowStride)->terrainHeight;
      *adjacentHeightField = *adjacentHeightField + adjacentHeightAdjustmentQ12;
      adjacentWaterDeltaField = &FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-heightAdjustmentOrRowStride)->waterSurfaceDelta;
      *adjacentWaterDeltaField = *adjacentWaterDeltaField - adjacentHeightAdjustmentQ12;
      directionStartCell =
           FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-g_TerrainScanRowStrideBytes - heightAdjustmentOrRowStride);
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      cell = directionStartCell + 1;
      TerrainHeightDelta_ApplyDirection2(scanStep,directionStartCell);
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
}


/* Address: 0x00508E60.
   Flatten brush, sector 2: like TerrainHeightDelta_ApplyWedge0, levelling the sector's diagonal and starting the
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


/* Address: 0x00508F00.
   Flatten brush, sector 3: like TerrainHeightDelta_ApplyWedge0, levelling the sector's diagonal and starting the
   straight scans of directions 3 and 4.
*/
void TerrainHeightDelta_ApplyWedge3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int heightAdjustmentOrRowStride;
  int adjacentHeightAdjustmentQ12;
  FieldGridCell *directionStartCell;

  if (scanStep < g_TerrainScanStepLimit) {
    while ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      heightAdjustmentOrRowStride = g_TerrainScanReferenceHeight - cell->terrainHeight;
      cell->terrainHeight = cell->terrainHeight + heightAdjustmentOrRowStride;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - heightAdjustmentOrRowStride;
      heightAdjustmentOrRowStride = g_TerrainScanRowStrideBytes;
      directionStartCell = cell - 1;
      TerrainHeightDelta_ApplyDirection3(scanStep + TERRAIN_SCAN_STEP_STRAIGHT,directionStartCell);
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      /* the in-between cell is the one below directionStartCell (one row stride on) */
      if ((FIELD_GRID_CELL_AT_BYTE_OFFSET(directionStartCell,heightAdjustmentOrRowStride)->flagsAndMaterial &
           FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      adjacentHeightAdjustmentQ12 =
           g_TerrainScanReferenceHeight - FIELD_GRID_CELL_AT_BYTE_OFFSET(directionStartCell,heightAdjustmentOrRowStride)->terrainHeight;
      FIELD_GRID_CELL_AT_BYTE_OFFSET(directionStartCell,heightAdjustmentOrRowStride)->terrainHeight =
           FIELD_GRID_CELL_AT_BYTE_OFFSET(directionStartCell,heightAdjustmentOrRowStride)->terrainHeight + adjacentHeightAdjustmentQ12;
      FIELD_GRID_CELL_AT_BYTE_OFFSET(directionStartCell,heightAdjustmentOrRowStride)->waterSurfaceDelta =
           FIELD_GRID_CELL_AT_BYTE_OFFSET(directionStartCell,heightAdjustmentOrRowStride)->waterSurfaceDelta - adjacentHeightAdjustmentQ12;
      /* directionStartCell[-1] one row down */
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(directionStartCell - 1,heightAdjustmentOrRowStride);
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      TerrainHeightDelta_ApplyDirection4
                (scanStep,FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
}


/* Address: 0x00508FA0.
   Flatten brush, sector 4: like TerrainHeightDelta_ApplyWedge0, levelling the sector's diagonal and starting the
   straight scans of directions 4 and 5.
*/
void TerrainHeightDelta_ApplyWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int heightAdjustmentOrRowStride;
  int adjacentHeightAdjustmentQ12;
  uint8_t *currentCellBytes;

  if (scanStep < g_TerrainScanStepLimit) {
    while ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      heightAdjustmentOrRowStride = g_TerrainScanReferenceHeight - cell->terrainHeight;
      cell->terrainHeight = cell->terrainHeight + heightAdjustmentOrRowStride;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - heightAdjustmentOrRowStride;
      heightAdjustmentOrRowStride = g_TerrainScanRowStrideBytes;
      /* the scan starts one row down and one cell left */
      TerrainHeightDelta_ApplyDirection4
                (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,
                 FIELD_GRID_CELL_AT_BYTE_OFFSET(cell - 1,g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      /* the in-between cell is the one below: flags +0x50, height +0x48, water delta +0x4C */
      if ((FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,heightAdjustmentOrRowStride)->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      adjacentHeightAdjustmentQ12 =
           g_TerrainScanReferenceHeight - FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,heightAdjustmentOrRowStride)->terrainHeight;
      FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,heightAdjustmentOrRowStride)->terrainHeight =
           FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,heightAdjustmentOrRowStride)->terrainHeight + adjacentHeightAdjustmentQ12;
      FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,heightAdjustmentOrRowStride)->waterSurfaceDelta =
           FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,heightAdjustmentOrRowStride)->waterSurfaceDelta - adjacentHeightAdjustmentQ12;
      currentCellBytes = (uint8_t *)cell;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      /* two rows down: the next diagonal cell one left of the direction 5 start */
      cell = (FieldGridCell *)(currentCellBytes + g_TerrainScanRowStrideBytes + heightAdjustmentOrRowStride)
             - 1;
      TerrainHeightDelta_ApplyDirection5
                (scanStep,(FieldGridCell *)
                          (currentCellBytes + g_TerrainScanRowStrideBytes + heightAdjustmentOrRowStride));
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
}


/* Address: 0x00509040.
   Flatten brush, sector 5: like TerrainHeightDelta_ApplyWedge0, levelling the sector's diagonal and starting the
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


/* Address: 0x00504520.
   Terrain raycast step: intersects the ray segment (grid-space delta, Z origin and Z delta, relative to the cell)
   with the two triangles of one field-grid cell given its four corner heights, first the one based on corner 3,
   then the one based on corner 0 (the far corner, local coordinates shifted by one cell). A hit returns the world
   distance from the ray origin with CF clear; a miss (also the quick reject when all four corners lie below the
   ray's lowest point) returns CF set with a meaningless EAX.
*/
TerrainRayTriangleResult TerrainTriangle_IntersectRayDistance
          (Q12 rayDeltaZQ12,Q12 gridRayDelta0Q12,Q12 gridRayDelta1Q12,Q12 rayOriginZQ12,
          Q12 cornerHeight0Q12,Q12 cornerHeight1Q12,Q12 cornerHeight2Q12,Q12 cornerHeight3Q12,
          Q12 cellLocalCoord1Q12,Q12 cellLocalCoord0Q12)

{
  int64_t planeTermOrProductA;
  int64_t worldXOffsetProduct;
  int64_t worldYOffsetProduct;
  int64_t planeTermOrProductB;
  uint64_t rayCrossLocal;
  uint32_t heightDeltaOrLowWord;
  uint32_t productLowOrDivisor;
  uint32_t shiftedLowA;
  uint32_t productLowB;
  uint32_t shiftedLowB;
  uint32_t rateHighOrEdgeSumLow;
  int combinedHigh;
  Q12 lowestRayZQ12;
  uint32_t rateShiftedWord;
  int edgeHighOrCoord0;
  int edgeHighOrCoord1;
  int edgeHighC;
  int edgeHighD;
  uint32_t edgeSumLowA;
  uint32_t edgeSumLowB;
  uint32_t partialSumLow;
  TerrainRayTriangleResult rejectResult;
  TerrainRayTriangleResult firstTriangleHit;
  TerrainRayTriangleResult secondTriangleHit;
  TerrainRayTriangleResult secondTriangleZeroResult;
  TerrainRayTriangleResult firstTriangleZeroResult;
  
  lowestRayZQ12 = rayOriginZQ12;
  if (rayDeltaZQ12 < 0) {
    lowestRayZQ12 = rayOriginZQ12 + rayDeltaZQ12;
  }
  if ((((cornerHeight3Q12 <= lowestRayZQ12) && (cornerHeight2Q12 <= lowestRayZQ12)) &&
      (cornerHeight1Q12 <= lowestRayZQ12)) && (heightDeltaOrLowWord = cornerHeight2Q12, cornerHeight0Q12 <= lowestRayZQ12))
  goto TerrainTriangle_IntersectRayDistance_ReturnHeightOrEdgeRejectWithCarrySet;
  planeTermOrProductA = ((int64_t)(cornerHeight1Q12 - cornerHeight3Q12) * (int64_t)gridRayDelta0Q12 +
          (int64_t)(cornerHeight2Q12 - cornerHeight3Q12) * (int64_t)gridRayDelta1Q12) -
          ((int64_t)rayDeltaZQ12 << 0xc);
  planeTermOrProductB = (int64_t)(cornerHeight1Q12 - cornerHeight3Q12) * (int64_t)cellLocalCoord1Q12 +
          (int64_t)(cornerHeight2Q12 - cornerHeight3Q12) * (int64_t)cellLocalCoord0Q12 +
          ((int64_t)(rayOriginZQ12 - cornerHeight3Q12) << 0xc);
  if ((planeTermOrProductB < 0) ?
      ((planeTermOrProductA < 0) && (-1 < planeTermOrProductB - planeTermOrProductA)) :
      ((-1 < planeTermOrProductA) && (planeTermOrProductB - planeTermOrProductA < 0))) {
    /* the ray crosses the first triangle's plane: test its edges */
    rayCrossLocal = (int64_t)gridRayDelta1Q12 * (int64_t)cellLocalCoord1Q12 -
            (int64_t)cellLocalCoord0Q12 * (int64_t)gridRayDelta0Q12;
    edgeHighOrCoord0 = (int)(rayCrossLocal >> 0x20);
    heightDeltaOrLowWord = cornerHeight1Q12 - cornerHeight3Q12;
    if ((int)heightDeltaOrLowWord < 0) {
      planeTermOrProductB = (uint64_t)heightDeltaOrLowWord * (rayCrossLocal & 0xffffffff);
      productLowOrDivisor = (uint32_t)planeTermOrProductB;
      edgeHighOrCoord1 = (edgeHighOrCoord0 * heightDeltaOrLowWord - (int)rayCrossLocal) + (int)((uint64_t)planeTermOrProductB >> 0x20);
    }
    else {
      planeTermOrProductB = (uint64_t)heightDeltaOrLowWord * (rayCrossLocal & 0xffffffff);
      productLowOrDivisor = (uint32_t)planeTermOrProductB;
      edgeHighOrCoord1 = edgeHighOrCoord0 * heightDeltaOrLowWord + (int)((uint64_t)planeTermOrProductB >> 0x20);
    }
    planeTermOrProductB = (int64_t)(rayOriginZQ12 - cornerHeight3Q12) * (int64_t)gridRayDelta1Q12;
    heightDeltaOrLowWord = (uint32_t)planeTermOrProductB;
    shiftedLowA = heightDeltaOrLowWord * 0x1000;
    edgeSumLowA = productLowOrDivisor + shiftedLowA;
    productLowB = (uint32_t)((int64_t)rayDeltaZQ12 * (int64_t)cellLocalCoord0Q12);
    shiftedLowB = productLowB * 0x1000;
    edgeSumLowB = edgeSumLowA + shiftedLowB;
    edgeHighOrCoord1 = edgeHighOrCoord1 + ((int)((uint64_t)planeTermOrProductB >> 0x20) << 0xc | heightDeltaOrLowWord >> 0x14) +
             (uint32_t)CARRY4(productLowOrDivisor,shiftedLowA) +
             ((int)((uint64_t)((int64_t)rayDeltaZQ12 * (int64_t)cellLocalCoord0Q12) >> 0x20) <<
              0xc | productLowB >> 0x14) + (uint32_t)CARRY4(edgeSumLowA,shiftedLowB);
    /* each edge term must have the sign of the plane rate (planeTermOrProductA) */
    if ((edgeHighOrCoord1 < 0) ? (planeTermOrProductA < 0) : (-1 < planeTermOrProductA)) {
      heightDeltaOrLowWord = cornerHeight3Q12 - cornerHeight2Q12;
      if ((int)heightDeltaOrLowWord < 0) {
        planeTermOrProductB = (uint64_t)heightDeltaOrLowWord * (rayCrossLocal & 0xffffffff);
        productLowOrDivisor = (uint32_t)planeTermOrProductB;
        edgeHighOrCoord0 = (edgeHighOrCoord0 * heightDeltaOrLowWord - (int)rayCrossLocal) + (int)((uint64_t)planeTermOrProductB >> 0x20);
      }
      else {
        planeTermOrProductB = (uint64_t)heightDeltaOrLowWord * (rayCrossLocal & 0xffffffff);
        productLowOrDivisor = (uint32_t)planeTermOrProductB;
        edgeHighOrCoord0 = edgeHighOrCoord0 * heightDeltaOrLowWord + (int)((uint64_t)planeTermOrProductB >> 0x20);
      }
      planeTermOrProductB = (int64_t)(rayOriginZQ12 - cornerHeight3Q12) * (int64_t)gridRayDelta0Q12;
      heightDeltaOrLowWord = (uint32_t)planeTermOrProductB;
      shiftedLowA = heightDeltaOrLowWord * 0x1000;
      partialSumLow = productLowOrDivisor + shiftedLowA;
      productLowB = (uint32_t)((int64_t)rayDeltaZQ12 * (int64_t)cellLocalCoord1Q12);
      shiftedLowB = productLowB * 0x1000;
      rateHighOrEdgeSumLow = (int)((uint64_t)planeTermOrProductA >> 0x20) << 0xc | (uint32_t)planeTermOrProductA >> 0x14;
      rateShiftedWord = (uint32_t)planeTermOrProductA * 0x1000;
      edgeSumLowA = shiftedLowB + partialSumLow;
      edgeHighOrCoord0 = ((int)((uint64_t)((int64_t)rayDeltaZQ12 * (int64_t)cellLocalCoord1Q12) >> 0x20)
                << 0xc | productLowB >> 0x14) +
               edgeHighOrCoord0 + ((int)((uint64_t)planeTermOrProductB >> 0x20) << 0xc | heightDeltaOrLowWord >> 0x14) +
               (uint32_t)CARRY4(productLowOrDivisor,shiftedLowA) + (uint32_t)CARRY4(shiftedLowB,partialSumLow);
      if ((edgeHighOrCoord0 < 0) ? ((int)rateHighOrEdgeSumLow < 0) : (-1 < (int)rateHighOrEdgeSumLow)) {
        heightDeltaOrLowWord = (rateShiftedWord - edgeSumLowA) - edgeSumLowB;
        combinedHigh = (((rateHighOrEdgeSumLow - edgeHighOrCoord0) - (uint32_t)(rateShiftedWord < edgeSumLowA)) - edgeHighOrCoord1) -
                 (uint32_t)(rateShiftedWord - edgeSumLowA < edgeSumLowB);
        if ((edgeHighOrCoord0 < 0) ? (combinedHigh < 0) : (-1 < combinedHigh)) {
          /* inside the first triangle: intersection distance */
          combinedHigh = combinedHigh + edgeHighOrCoord0 + (uint32_t)CARRY4(heightDeltaOrLowWord,edgeSumLowA) + edgeHighOrCoord1 +
                   (uint32_t)CARRY4(heightDeltaOrLowWord + edgeSumLowA,edgeSumLowB);
          heightDeltaOrLowWord = heightDeltaOrLowWord + edgeSumLowA + edgeSumLowB >> 0xc | combinedHigh * 0x100000;
          combinedHigh = combinedHigh >> 0xc;
          if (heightDeltaOrLowWord == 0) {
            firstTriangleZeroResult.distanceQ12 = 0;
            firstTriangleZeroResult.missed = false;
            return firstTriangleZeroResult;
          }
          if ((int)heightDeltaOrLowWord < 0) {
            if (combinedHigh != -1) {
              edgeSumLowB = edgeSumLowB >> 0xc | edgeHighOrCoord1 * 0x100000;
              edgeSumLowA = edgeSumLowA >> 0xc | edgeHighOrCoord0 * 0x100000;
              heightDeltaOrLowWord = heightDeltaOrLowWord >> 0xc | combinedHigh << 0x14;
              edgeHighOrCoord0 = edgeHighOrCoord0 >> 0xc;
              edgeHighOrCoord1 = edgeHighOrCoord1 >> 0xc;
            }
          }
          else if (combinedHigh != 0) {
            edgeSumLowB = edgeSumLowB >> 0xc | edgeHighOrCoord1 * 0x100000;
            edgeSumLowA = edgeSumLowA >> 0xc | edgeHighOrCoord0 * 0x100000;
            heightDeltaOrLowWord = heightDeltaOrLowWord >> 0xc | combinedHigh << 0x14;
            edgeHighOrCoord0 = edgeHighOrCoord0 >> 0xc;
            edgeHighOrCoord1 = edgeHighOrCoord1 >> 0xc;
          }
          /* IDIV of the EDX:EAX pairs */
          edgeHighOrCoord0 = (int)((int64_t)((uint64_t)(uint32_t)edgeHighOrCoord0 << 0x20 | (uint64_t)edgeSumLowA) /
                                   (int64_t)(int)heightDeltaOrLowWord);
          combinedHigh = cellLocalCoord1Q12 + edgeHighOrCoord0;
          edgeHighOrCoord1 = (int)((int64_t)((uint64_t)(uint32_t)edgeHighOrCoord1 << 0x20 | (uint64_t)edgeSumLowB) /
                                   (int64_t)(int)heightDeltaOrLowWord);
          planeTermOrProductB = (int64_t)edgeHighOrCoord1 * (int64_t)(cornerHeight2Q12 - cornerHeight3Q12);
          planeTermOrProductA = (int64_t)edgeHighOrCoord0 * (int64_t)(cornerHeight1Q12 - cornerHeight3Q12);
          /* grid offsets of the hit back to world units (inverse of FIELD_GRID_WORLD_*_Q20): X = (column + row / 2)
             * 0x901 / 0x1000, Y = row * -1999 / 0x1000 */
          worldXOffsetProduct = (int64_t)(combinedHigh + (cellLocalCoord0Q12 + edgeHighOrCoord1) * 2) * 0x901;
          worldYOffsetProduct = (int64_t)combinedHigh * -1999;
          firstTriangleHit.distanceQ12 =
               FixedMath_Length3(((cornerHeight3Q12 +
                                  ((int)((uint64_t)planeTermOrProductB >> 0x20) << 0x14 | (uint32_t)planeTermOrProductB >> 0xc)
                                  ) - rayOriginZQ12) +
                                 ((int)((uint64_t)planeTermOrProductA >> 0x20) << 0x14 | (uint32_t)planeTermOrProductA >> 0xc),
                                 (int)((uint64_t)worldYOffsetProduct >> 0x20) << 0x14 | (uint32_t)worldYOffsetProduct >> 0xc,
                                 (int)((uint64_t)worldXOffsetProduct >> 0x20) << 0x13 | (uint32_t)worldXOffsetProduct >> 0xd);
          firstTriangleHit.missed = false;
          return firstTriangleHit;
        }
      }
    }
  }
  /* second triangle: local coordinates relative to the far corner (corner 0) */
  edgeHighOrCoord0 = cellLocalCoord0Q12 + FIELD_GRID_CELL_Q12;
  edgeHighOrCoord1 = cellLocalCoord1Q12 + FIELD_GRID_CELL_Q12;
  planeTermOrProductB = (int64_t)(cornerHeight2Q12 - cornerHeight0Q12) * (int64_t)gridRayDelta0Q12 +
          (int64_t)(cornerHeight1Q12 - cornerHeight0Q12) * (int64_t)gridRayDelta1Q12 +
          ((int64_t)rayDeltaZQ12 << 0xc);
  productLowOrDivisor = (uint32_t)planeTermOrProductB;
  combinedHigh = (int)((uint64_t)planeTermOrProductB >> 0x20);
  heightDeltaOrLowWord = (cornerHeight0Q12 - rayOriginZQ12) * 0x1000;
  planeTermOrProductA = (int64_t)(cornerHeight2Q12 - cornerHeight0Q12) * (int64_t)edgeHighOrCoord1 +
          (int64_t)(cornerHeight1Q12 - cornerHeight0Q12) * (int64_t)edgeHighOrCoord0 +
          ((int64_t)(cornerHeight0Q12 - rayOriginZQ12) << 0xc); /* low word = heightDeltaOrLowWord */
  edgeHighC = (int)((uint64_t)planeTermOrProductA >> 0x20);
  if (planeTermOrProductA < 0) {
    if ((-1 < planeTermOrProductB) || ((int)((edgeHighC - combinedHigh) - (uint32_t)((uint32_t)planeTermOrProductA < productLowOrDivisor)) < 0))
    goto TerrainTriangle_IntersectRayDistance_ReturnHeightOrEdgeRejectWithCarrySet;
  }
  else if ((planeTermOrProductB < 0) || (-1 < (int)((edgeHighC - combinedHigh) - (uint32_t)((uint32_t)planeTermOrProductA < productLowOrDivisor))))
  goto TerrainTriangle_IntersectRayDistance_ReturnHeightOrEdgeRejectWithCarrySet;
  rayCrossLocal = (int64_t)gridRayDelta1Q12 * (int64_t)edgeHighOrCoord1 -
          (int64_t)edgeHighOrCoord0 * (int64_t)gridRayDelta0Q12;
  edgeHighC = (int)(rayCrossLocal >> 0x20);
  heightDeltaOrLowWord = cornerHeight0Q12 - cornerHeight2Q12;
  if ((int)heightDeltaOrLowWord < 0) {
    planeTermOrProductA = (uint64_t)heightDeltaOrLowWord * (rayCrossLocal & 0xffffffff);
    shiftedLowA = (uint32_t)planeTermOrProductA;
    edgeHighD = (edgeHighC * heightDeltaOrLowWord - (int)rayCrossLocal) + (int)((uint64_t)planeTermOrProductA >> 0x20);
  }
  else {
    planeTermOrProductA = (uint64_t)heightDeltaOrLowWord * (rayCrossLocal & 0xffffffff);
    shiftedLowA = (uint32_t)planeTermOrProductA;
    edgeHighD = edgeHighC * heightDeltaOrLowWord + (int)((uint64_t)planeTermOrProductA >> 0x20);
  }
  planeTermOrProductA = (int64_t)(rayOriginZQ12 - cornerHeight0Q12) * (int64_t)gridRayDelta1Q12;
  productLowB = (uint32_t)planeTermOrProductA;
  shiftedLowB = productLowB * 0x1000;
  edgeSumLowB = shiftedLowA + shiftedLowB;
  edgeSumLowA = (uint32_t)((int64_t)rayDeltaZQ12 * (int64_t)edgeHighOrCoord0);
  heightDeltaOrLowWord = edgeSumLowA * 0x1000;
  rateHighOrEdgeSumLow = edgeSumLowB + heightDeltaOrLowWord;
  edgeHighD = edgeHighD + ((int)((uint64_t)planeTermOrProductA >> 0x20) << 0xc | productLowB >> 0x14) +
           (uint32_t)CARRY4(shiftedLowA,shiftedLowB) +
           ((int)((uint64_t)((int64_t)rayDeltaZQ12 * (int64_t)edgeHighOrCoord0) >> 0x20) << 0xc |
           edgeSumLowA >> 0x14) + (uint32_t)CARRY4(edgeSumLowB,heightDeltaOrLowWord);
  if (edgeHighD < 0) {
    if (-1 < planeTermOrProductB)
    goto TerrainTriangle_IntersectRayDistance_ReturnHeightOrEdgeRejectWithCarrySet;
  }
  else if (planeTermOrProductB < 0)
  goto TerrainTriangle_IntersectRayDistance_ReturnHeightOrEdgeRejectWithCarrySet;
  heightDeltaOrLowWord = cornerHeight1Q12 - cornerHeight0Q12;
  if ((int)heightDeltaOrLowWord < 0) {
    planeTermOrProductB = (uint64_t)heightDeltaOrLowWord * (rayCrossLocal & 0xffffffff);
    shiftedLowA = (uint32_t)planeTermOrProductB;
    edgeHighC = (edgeHighC * heightDeltaOrLowWord - (int)rayCrossLocal) + (int)((uint64_t)planeTermOrProductB >> 0x20);
  }
  else {
    planeTermOrProductB = (uint64_t)heightDeltaOrLowWord * (rayCrossLocal & 0xffffffff);
    shiftedLowA = (uint32_t)planeTermOrProductB;
    edgeHighC = edgeHighC * heightDeltaOrLowWord + (int)((uint64_t)planeTermOrProductB >> 0x20);
  }
  planeTermOrProductB = (int64_t)(rayOriginZQ12 - cornerHeight0Q12) * (int64_t)gridRayDelta0Q12;
  productLowB = (uint32_t)planeTermOrProductB;
  shiftedLowB = productLowB * 0x1000;
  partialSumLow = shiftedLowA + shiftedLowB;
  edgeSumLowA = (uint32_t)((int64_t)rayDeltaZQ12 * (int64_t)edgeHighOrCoord1);
  edgeSumLowB = edgeSumLowA * 0x1000;
  rateShiftedWord = combinedHigh << 0xc | productLowOrDivisor >> 0x14;
  productLowOrDivisor = productLowOrDivisor * 0x1000;
  heightDeltaOrLowWord = edgeSumLowB + partialSumLow;
  combinedHigh = ((int)((uint64_t)((int64_t)rayDeltaZQ12 * (int64_t)edgeHighOrCoord1) >> 0x20) << 0xc |
           edgeSumLowA >> 0x14) +
           edgeHighC + ((int)((uint64_t)planeTermOrProductB >> 0x20) << 0xc | productLowB >> 0x14) +
           (uint32_t)CARRY4(shiftedLowA,shiftedLowB) + (uint32_t)CARRY4(edgeSumLowB,partialSumLow);
  if ((combinedHigh < 0) ? ((int)rateShiftedWord < 0) : (-1 < (int)rateShiftedWord)) {
    shiftedLowA = (productLowOrDivisor - heightDeltaOrLowWord) - rateHighOrEdgeSumLow;
    edgeHighC = (((rateShiftedWord - combinedHigh) - (uint32_t)(productLowOrDivisor < heightDeltaOrLowWord)) - edgeHighD) -
             (uint32_t)(productLowOrDivisor - heightDeltaOrLowWord < rateHighOrEdgeSumLow);
    if ((combinedHigh < 0) ? (edgeHighC < 0) : (-1 < edgeHighC)) {
      /* inside the second triangle: intersection distance */
      edgeHighC = edgeHighC + combinedHigh + (uint32_t)CARRY4(shiftedLowA,heightDeltaOrLowWord) + edgeHighD +
               (uint32_t)CARRY4(shiftedLowA + heightDeltaOrLowWord,rateHighOrEdgeSumLow);
      productLowOrDivisor = shiftedLowA + heightDeltaOrLowWord + rateHighOrEdgeSumLow >> 0xc | edgeHighC * 0x100000;
      edgeHighC = edgeHighC >> 0xc;
      if (productLowOrDivisor == 0) {
        secondTriangleZeroResult.distanceQ12 = 0;
        secondTriangleZeroResult.missed = false;
        return secondTriangleZeroResult;
      }
      if ((int)productLowOrDivisor < 0) {
        if (edgeHighC != -1) {
          rateHighOrEdgeSumLow = rateHighOrEdgeSumLow >> 0xc | edgeHighD * 0x100000;
          heightDeltaOrLowWord = heightDeltaOrLowWord >> 0xc | combinedHigh * 0x100000;
          productLowOrDivisor = productLowOrDivisor >> 0xc | edgeHighC << 0x14;
          combinedHigh = combinedHigh >> 0xc;
          edgeHighD = edgeHighD >> 0xc;
        }
      }
      else if (edgeHighC != 0) {
        rateHighOrEdgeSumLow = rateHighOrEdgeSumLow >> 0xc | edgeHighD * 0x100000;
        heightDeltaOrLowWord = heightDeltaOrLowWord >> 0xc | combinedHigh * 0x100000;
        productLowOrDivisor = productLowOrDivisor >> 0xc | edgeHighC << 0x14;
        combinedHigh = combinedHigh >> 0xc;
        edgeHighD = edgeHighD >> 0xc;
      }
      /* IDIV of the EDX:EAX pairs */
      combinedHigh = (int)((int64_t)((uint64_t)(uint32_t)combinedHigh << 0x20 | (uint64_t)heightDeltaOrLowWord) /
                           (int64_t)(int)productLowOrDivisor);
      edgeHighOrCoord1 = edgeHighOrCoord1 - combinedHigh;
      edgeHighC = (int)((int64_t)((uint64_t)(uint32_t)edgeHighD << 0x20 | (uint64_t)rateHighOrEdgeSumLow) /
                        (int64_t)(int)productLowOrDivisor);
      planeTermOrProductB = (int64_t)edgeHighC * (int64_t)(cornerHeight1Q12 - cornerHeight0Q12);
      planeTermOrProductA = (int64_t)combinedHigh * (int64_t)(cornerHeight2Q12 - cornerHeight0Q12);
      /* back to world units as for the first triangle */
      worldXOffsetProduct = (int64_t)(edgeHighOrCoord1 + (edgeHighOrCoord0 - edgeHighC) * 2) * 0x901;
      worldYOffsetProduct = (int64_t)edgeHighOrCoord1 * -1999;
      secondTriangleHit.distanceQ12 =
           FixedMath_Length3(((cornerHeight0Q12 +
                              ((int)((uint64_t)planeTermOrProductB >> 0x20) << 0x14 | (uint32_t)planeTermOrProductB >> 0xc)) -
                             rayOriginZQ12) +
                             ((int)((uint64_t)planeTermOrProductA >> 0x20) << 0x14 | (uint32_t)planeTermOrProductA >> 0xc),
                             (int)((uint64_t)worldYOffsetProduct >> 0x20) << 0x14 | (uint32_t)worldYOffsetProduct >> 0xc,
                             (int)((uint64_t)worldXOffsetProduct >> 0x20) << 0x13 | (uint32_t)worldXOffsetProduct >> 0xd);
      secondTriangleHit.missed = false;
      return secondTriangleHit;
    }
  }
TerrainTriangle_IntersectRayDistance_ReturnHeightOrEdgeRejectWithCarrySet:
  rejectResult.missed = true;
  rejectResult.distanceQ12 = heightDeltaOrLowWord;
  return rejectResult;
}


FieldGridCell *g_TerrainRayNextCell;
Q12 g_TerrainRayNextCoord0Q12;
Q12 g_TerrainRayNextCoord1Q12;

/* Address: 0x005049E0.
   One step of the terrain raycasts' cell walk (coord0 = grid row, coord1 = grid column, both Q12): moves to the
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


/* Address: 0x00507AB0.
   Height-band placement test, sector 0 of the hexagon (see TerrainHeightBand_TestAroundWorldPoint): walks the
   sector's diagonal, tests each diagonal cell and the cell between it and the next one, and runs the straight
   tests of directions 0 and 1 that cover the sector. Returns true (CF set) at the first cell outside the height
   band, false when the step limit is reached.
*/
bool TerrainHeightBand_TestWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int rowStrideBytes;
  int relativeHeightQ12;
  FieldGridCell *directionStartCell;
  bool directionFailed;

  if (scanStep < g_TerrainScanStepLimit) {
    do {
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      if (((((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) ||
           (relativeHeightQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight, 0 < cell->waterSurfaceDelta)
           ) || ((int)g_TerrainHeightBandMaximumDelta < relativeHeightQ12)) ||
         (relativeHeightQ12 < (int)g_TerrainHeightBandMinimumDelta)) {
        return true;
      }
      directionStartCell = cell + 1;
      directionFailed = TerrainHeightBand_TestDirection0(scanStep + TERRAIN_SCAN_STEP_STRAIGHT,directionStartCell);
      if (directionFailed) {
        return true;
      }
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return false;
      }
      /* the in-between cell is one row up from directionStartCell (flags +0x50, height +0x48, water delta
         +0x4C) */
      if ((FIELD_GRID_CELL_AT_BYTE_OFFSET(directionStartCell,-rowStrideBytes)->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return true;
      }
      relativeHeightQ12 = FIELD_GRID_CELL_AT_BYTE_OFFSET(directionStartCell,-rowStrideBytes)->terrainHeight - g_TerrainScanReferenceHeight;
      if (0 < FIELD_GRID_CELL_AT_BYTE_OFFSET(directionStartCell,-rowStrideBytes)->waterSurfaceDelta) {
        return true;
      }
      if ((int)g_TerrainHeightBandMaximumDelta < relativeHeightQ12) {
        return true;
      }
      if (relativeHeightQ12 < (int)g_TerrainHeightBandMinimumDelta) {
        return true;
      }
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(directionStartCell + 1,-rowStrideBytes);
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      directionFailed = TerrainHeightBand_TestDirection1
                        (scanStep,FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-g_TerrainScanRowStrideBytes));
      if (directionFailed) {
        return true;
      }
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return false;
}


/* Address: 0x00507BA0.
   Height-band placement test, sector 1: like TerrainHeightBand_TestWedge0, running the straight tests of
   directions 1 and 2.
*/
bool TerrainHeightBand_TestWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int rowStrideBytes;
  int relativeHeightQ12;
  FieldGridCell *directionStartCell;
  bool directionFailed;

  if (scanStep < g_TerrainScanStepLimit) {
    do {
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      if (((((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) ||
           (relativeHeightQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight, 0 < cell->waterSurfaceDelta)
           ) || ((int)g_TerrainHeightBandMaximumDelta < relativeHeightQ12)) ||
         (relativeHeightQ12 < (int)g_TerrainHeightBandMinimumDelta)) {
        return true;
      }
      directionFailed = TerrainHeightBand_TestDirection1
                        (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,
                         FIELD_GRID_CELL_AT_BYTE_OFFSET(cell + 1,-g_TerrainScanRowStrideBytes));
      if (directionFailed) {
        return true;
      }
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return false;
      }
      /* the in-between cell is the one above (flags +0x50, height +0x48, water delta +0x4C) */
      if ((FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return true;
      }
      relativeHeightQ12 = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)->terrainHeight - g_TerrainScanReferenceHeight;
      if (0 < FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)->waterSurfaceDelta) {
        return true;
      }
      if ((int)g_TerrainHeightBandMaximumDelta < relativeHeightQ12) {
        return true;
      }
      if (relativeHeightQ12 < (int)g_TerrainHeightBandMinimumDelta) {
        return true;
      }
      directionStartCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-g_TerrainScanRowStrideBytes - rowStrideBytes);
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      cell = directionStartCell + 1;
      directionFailed = TerrainHeightBand_TestDirection2(scanStep,directionStartCell);
      if (directionFailed) {
        return true;
      }
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return false;
}


/* Address: 0x00507C80.
   Height-band placement test, sector 2: like TerrainHeightBand_TestWedge0, running the straight tests of
   directions 2 and 3.
*/
bool TerrainHeightBand_TestWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *directionStartCell;
  int relativeHeightQ12;
  bool directionFailed;

  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if (((((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) ||
           (relativeHeightQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight, 0 < cell->waterSurfaceDelta)
           ) || ((int)g_TerrainHeightBandMaximumDelta < relativeHeightQ12)) ||
         (relativeHeightQ12 < (int)g_TerrainHeightBandMinimumDelta)) {
        return true;
      }
      directionFailed = TerrainHeightBand_TestDirection2
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
      relativeHeightQ12 = cell[-1].terrainHeight - g_TerrainScanReferenceHeight;
      if (0 < cell[-1].waterSurfaceDelta) {
        return true;
      }
      if ((int)g_TerrainHeightBandMaximumDelta < relativeHeightQ12) {
        return true;
      }
      if (relativeHeightQ12 < (int)g_TerrainHeightBandMinimumDelta) {
        return true;
      }
      directionStartCell = cell - 2;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell - 1,-g_TerrainScanRowStrideBytes);
      directionFailed = TerrainHeightBand_TestDirection3(scanStep,directionStartCell);
      if (directionFailed) {
        return true;
      }
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return false;
}


/* Address: 0x00507D60.
   Height-band placement test, sector 3: like TerrainHeightBand_TestWedge0, running the straight tests of
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


/* Address: 0x00507E40.
   Height-band placement test, sector 4: like TerrainHeightBand_TestWedge0, running the straight tests of
   directions 4 and 5.
*/
bool TerrainHeightBand_TestWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  uint8_t *cellBytes;
  int rowStrideBytes;
  int relativeHeightQ12;
  bool directionFailed;

  if (scanStep < g_TerrainScanStepLimit) {
    do {
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      if (((((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) ||
           (relativeHeightQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight, 0 < cell->waterSurfaceDelta)
           ) || ((int)g_TerrainHeightBandMaximumDelta < relativeHeightQ12)) ||
         (relativeHeightQ12 < (int)g_TerrainHeightBandMinimumDelta)) {
        return true;
      }
      directionFailed = TerrainHeightBand_TestDirection4
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
      relativeHeightQ12 = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)->terrainHeight - g_TerrainScanReferenceHeight;
      if (0 < FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)->waterSurfaceDelta) {
        return true;
      }
      if ((int)g_TerrainHeightBandMaximumDelta < relativeHeightQ12) {
        return true;
      }
      if (relativeHeightQ12 < (int)g_TerrainHeightBandMinimumDelta) {
        return true;
      }
      /* two rows down: the next diagonal cell one left of the direction 5 start */
      cellBytes = (uint8_t *)cell;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      cell = (FieldGridCell *)(cellBytes + g_TerrainScanRowStrideBytes + rowStrideBytes) - 1;
      directionFailed = TerrainHeightBand_TestDirection5
                        (scanStep,(FieldGridCell *)
                                  (cellBytes + g_TerrainScanRowStrideBytes + rowStrideBytes));
      if (directionFailed) {
        return true;
      }
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return false;
}


/* Address: 0x00507F20.
   Height-band placement test, sector 5: like TerrainHeightBand_TestWedge0, running the straight tests of
   directions 5 and 0.
*/
bool TerrainHeightBand_TestWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *directionStartCell;
  int relativeHeightQ12;
  bool directionFailed;

  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if (((((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) ||
           (relativeHeightQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight, 0 < cell->waterSurfaceDelta)
           ) || ((int)g_TerrainHeightBandMaximumDelta < relativeHeightQ12)) ||
         (relativeHeightQ12 < (int)g_TerrainHeightBandMinimumDelta)) {
        return true;
      }
      directionFailed = TerrainHeightBand_TestDirection5
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
      relativeHeightQ12 = cell[1].terrainHeight - g_TerrainScanReferenceHeight;
      if (0 < cell[1].waterSurfaceDelta) {
        return true;
      }
      if ((int)g_TerrainHeightBandMaximumDelta < relativeHeightQ12) {
        return true;
      }
      if (relativeHeightQ12 < (int)g_TerrainHeightBandMinimumDelta) {
        return true;
      }
      directionStartCell = cell + 2;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell + 1,g_TerrainScanRowStrideBytes);
      directionFailed = TerrainHeightBand_TestDirection0(scanStep,directionStartCell);
      if (directionFailed) {
        return true;
      }
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return false;
}


/* Address: 0x00508470.
   Water-surface placement test, sector 0 of the hexagon (see TerrainAuxHeightThreshold_TestAroundWorldPoint):
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


/* Address: 0x00508540.
   Water-surface placement test, sector 1: like TerrainAuxHeightThreshold_TestWedge0, running the straight tests
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


/* Address: 0x00508600.
   Water-surface placement test, sector 2: like TerrainAuxHeightThreshold_TestWedge0, running the straight tests
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


/* Address: 0x005086C0.
   Water-surface placement test, sector 3: like TerrainAuxHeightThreshold_TestWedge0, running the straight tests
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


/* Address: 0x00508790.
   Water-surface placement test, sector 4: like TerrainAuxHeightThreshold_TestWedge0, running the straight tests
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


/* Address: 0x00508850.
   Water-surface placement test, sector 5: like TerrainAuxHeightThreshold_TestWedge0, running the straight tests
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


/* Address: 0x005077F0.
   Height-band placement test, straight leg along direction 0 (C+1, right): returns true (CF set) at the first
   cell that is a map-edge cell, lies under water (waterSurfaceDelta > 0) or whose height relative to
   g_TerrainScanReferenceHeight leaves [g_TerrainHeightBandMinimumDelta, g_TerrainHeightBandMaximumDelta]; false
   once the step limit is reached (4 scan steps per cell).
*/
bool TerrainHeightBand_TestDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int terrainHeightDeltaQ12;

  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if (((((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) ||
         (terrainHeightDeltaQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight,
         0 < cell->waterSurfaceDelta)) || ((int)g_TerrainHeightBandMaximumDelta < terrainHeightDeltaQ12))
       || (terrainHeightDeltaQ12 < (int)g_TerrainHeightBandMinimumDelta)) break;
    scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
    cell++;
  }
  return true;
}


/* Address: 0x00507860.
   Height-band placement test, straight leg along direction 1 (C+1-W, up and right); see TerrainHeightBand_TestDirection0.
*/
bool TerrainHeightBand_TestDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int terrainHeightDeltaQ12;

  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if (((((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) ||
         (terrainHeightDeltaQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight,
         0 < cell->waterSurfaceDelta)) || ((int)g_TerrainHeightBandMaximumDelta < terrainHeightDeltaQ12))
       || (terrainHeightDeltaQ12 < (int)g_TerrainHeightBandMinimumDelta)) break;
    scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
    cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell + 1,-g_TerrainScanRowStrideBytes);
  }
  return true;
}


/* Address: 0x005078E0.
   Height-band placement test, straight leg along direction 2 (C-W, up); see TerrainHeightBand_TestDirection0.
*/
bool TerrainHeightBand_TestDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int terrainHeightDeltaQ12;

  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if (((((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) ||
         (terrainHeightDeltaQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight,
         0 < cell->waterSurfaceDelta)) || ((int)g_TerrainHeightBandMaximumDelta < terrainHeightDeltaQ12))
       || (terrainHeightDeltaQ12 < (int)g_TerrainHeightBandMinimumDelta)) break;
    scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
    cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-g_TerrainScanRowStrideBytes);
  }
  return true;
}


/* Address: 0x00507950.
   Height-band placement test, straight leg along direction 3 (C-1, left); see TerrainHeightBand_TestDirection0.
*/
bool TerrainHeightBand_TestDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int terrainHeightDeltaQ12;

  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if (((((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) ||
         (terrainHeightDeltaQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight,
         0 < cell->waterSurfaceDelta)) || ((int)g_TerrainHeightBandMaximumDelta < terrainHeightDeltaQ12))
       || (terrainHeightDeltaQ12 < (int)g_TerrainHeightBandMinimumDelta)) break;
    scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
    cell--;
  }
  return true;
}


/* Address: 0x005079C0.
   Height-band placement test, straight leg along direction 4 (C-1+W, down and left); see TerrainHeightBand_TestDirection0.
*/
bool TerrainHeightBand_TestDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int terrainHeightDeltaQ12;

  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if (((((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) ||
         (terrainHeightDeltaQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight,
         0 < cell->waterSurfaceDelta)) || ((int)g_TerrainHeightBandMaximumDelta < terrainHeightDeltaQ12))
       || (terrainHeightDeltaQ12 < (int)g_TerrainHeightBandMinimumDelta)) break;
    scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
    cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell - 1,g_TerrainScanRowStrideBytes);
  }
  return true;
}


/* Address: 0x00507A40.
   Height-band placement test, straight leg along direction 5 (C+W, down); see TerrainHeightBand_TestDirection0.
*/
bool TerrainHeightBand_TestDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int terrainHeightDeltaQ12;

  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if (((((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) ||
         (terrainHeightDeltaQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight,
         0 < cell->waterSurfaceDelta)) || ((int)g_TerrainHeightBandMaximumDelta < terrainHeightDeltaQ12))
       || (terrainHeightDeltaQ12 < (int)g_TerrainHeightBandMinimumDelta)) break;
    scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
    cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,g_TerrainScanRowStrideBytes);
  }
  return true;
}


/* Address: 0x005081D0.
   Water-surface placement test, straight leg along direction 0 (C+1, right): returns true (CF set) at the first
   cell that is a map-edge cell, has a negative waterSurfaceDelta or whose triangle1NormalAngles high word is below
   g_TerrainAuxHeightMinimum; false once the step limit is reached (4 scan steps per cell).
*/
bool TerrainAuxHeightThreshold_TestDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if ((((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) || (cell->waterSurfaceDelta < 0)) ||
       ((int)cell->triangle1NormalAngles >> 16 < (int)g_TerrainAuxHeightMinimum)) break;
    scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
    cell++;
  }
  return true;
}


/* Address: 0x00508240.
   Water-surface placement test, straight leg along direction 1 (C+1-W, up and right); see
   TerrainAuxHeightThreshold_TestDirection0.
*/
bool TerrainAuxHeightThreshold_TestDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if ((((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) || (cell->waterSurfaceDelta < 0)) ||
       ((int)cell->triangle1NormalAngles >> 16 < (int)g_TerrainAuxHeightMinimum)) break;
    scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
    cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell + 1,-g_TerrainScanRowStrideBytes);
  }
  return true;
}


/* Address: 0x005082B0.
   Water-surface placement test, straight leg along direction 2 (C-W, up); see
   TerrainAuxHeightThreshold_TestDirection0.
*/
bool TerrainAuxHeightThreshold_TestDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if ((((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) || (cell->waterSurfaceDelta < 0)) ||
       ((int)cell->triangle1NormalAngles >> 16 < (int)g_TerrainAuxHeightMinimum)) break;
    scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
    cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-g_TerrainScanRowStrideBytes);
  }
  return true;
}


/* Address: 0x00508320.
   Water-surface placement test, straight leg along direction 3 (C-1, left); see
   TerrainAuxHeightThreshold_TestDirection0.
*/
bool TerrainAuxHeightThreshold_TestDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if ((((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) || (cell->waterSurfaceDelta < 0)) ||
       ((int)cell->triangle1NormalAngles >> 16 < (int)g_TerrainAuxHeightMinimum)) break;
    scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
    cell--;
  }
  return true;
}


/* Address: 0x00508390.
   Water-surface placement test, straight leg along direction 4 (C-1+W, down and left); see
   TerrainAuxHeightThreshold_TestDirection0.
*/
bool TerrainAuxHeightThreshold_TestDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if ((((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) || (cell->waterSurfaceDelta < 0)) ||
       ((int)cell->triangle1NormalAngles >> 16 < (int)g_TerrainAuxHeightMinimum)) break;
    scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
    cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell - 1,g_TerrainScanRowStrideBytes);
  }
  return true;
}


/* Address: 0x00508400.
   Water-surface placement test, straight leg along direction 5 (C+W, down); see
   TerrainAuxHeightThreshold_TestDirection0.
*/
bool TerrainAuxHeightThreshold_TestDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if ((((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) || (cell->waterSurfaceDelta < 0)) ||
       ((int)cell->triangle1NormalAngles >> 16 < (int)g_TerrainAuxHeightMinimum)) break;
    scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
    cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,g_TerrainScanRowStrideBytes);
  }
  return true;
}


/* Address: 0x00508AE0.
   Flatten brush, straight leg along direction 0 (C+1, right): levels each cell to g_TerrainScanReferenceHeight
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


/* Address: 0x00508B40.
   Flatten brush, straight leg along direction 1 (C+1-W, up and right): levels each cell to
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


/* Address: 0x00508BA0.
   Flatten brush, straight leg along direction 2 (C-W, up): levels each cell to g_TerrainScanReferenceHeight,
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


/* Address: 0x00508C00.
   Flatten brush, straight leg along direction 3 (C-1, left): levels each cell to g_TerrainScanReferenceHeight,
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


/* Address: 0x00508C60.
   Flatten brush, straight leg along direction 4 (C-1+W, down and left): levels each cell to
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


/* Address: 0x00508CC0.
   Flatten brush, straight leg along direction 5 (C+W, down): levels each cell to g_TerrainScanReferenceHeight,
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

