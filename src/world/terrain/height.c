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
   Ownership: world/terrain/height.
   Purpose: Converts a world point to the hexagonal grid, verifies the center cell against the configured relative-
   height band, and dispatches all six wedge tests within the bounded radius. Terrain-class placement test
   callback; CF carries acceptance and EAX carries the direct-call result. Typed parameters: p0
   radiusWorldUnits→FieldGridRadiusUnits. Calling convention, parameter storage, body bytes, control flow, globals,
   locals, and executable data remain unchanged.
   Local calls: TerrainHeightBand_TestWedge0, TerrainHeightBand_TestWedge1, TerrainHeightBand_TestWedge2,
   TerrainHeightBand_TestWedge3, TerrainHeightBand_TestWedge4, TerrainHeightBand_TestWedge5.
   Cross-module calls: FieldGrid_WorldToGridQ12 [world/terrain/grid].
*/
bool __thandor_void_preserve_ecx_edx
TerrainHeightBand_TestAroundWorldPoint
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
  FieldGridCell *centerOrWedgeCell;
  bool wedgeBlocked;
  FieldGridCoordinatesEaxEdx8 gridCoordinates;
  uint32_t cellRow;
  uint32_t cellColumn;
  
  if (fieldGrid != (FieldGridAsset *)0x0) {
    g_TerrainScanStepLimit = (uint32_t)radiusWorldUnits / 0x240;
    if (g_TerrainScanStepLimit == 0) {
      g_TerrainScanStepLimit = 1;
    }
    else if (0xff < g_TerrainScanStepLimit) {
      g_TerrainScanStepLimit = 0xff;
    }
    g_TerrainScanReferenceHeight = referenceHeightQ12;
    gridCoordinates = FieldGrid_WorldToGridQ12(worldXQ12,worldYQ12);
    baseColumn = gridCoordinates.columnQ12 >> 0xc;
    cellRow = gridCoordinates.rowQ12 >> 0xc;
    columnFractionQ12 = (uint32_t)(THANDOR_BITCAST(FieldGridCoordinatesEaxEdx8, uint64_t, gridCoordinates) & 0xfff00000fff);
    rowFractionQ12 = (uint32_t)((THANDOR_BITCAST(FieldGridCoordinatesEaxEdx8, uint64_t, gridCoordinates) & 0xfff00000fff) >> 0x20);
    fractionSumOrGridWidth = rowFractionQ12 + columnFractionQ12 * 2;
    cellColumn = baseColumn;
    if (fractionSumOrGridWidth < 0x1000) {
      if (0xfff < columnFractionQ12 + rowFractionQ12 * 2) {
        cellRow = cellRow + 1;
      }
    }
    else if (fractionSumOrGridWidth < 0x2001) {
      cellColumn = baseColumn + 1;
      if (columnFractionQ12 < rowFractionQ12) {
        cellRow = cellRow + 1;
        cellColumn = baseColumn;
      }
    }
    else {
      cellColumn = baseColumn + 1;
      if (0x1fff < columnFractionQ12 + rowFractionQ12 * 2) {
        cellRow = cellRow + 1;
      }
    }
    fractionSumOrGridWidth = fieldGrid->gridWidth;
    g_TerrainScanRowStrideBytes = fractionSumOrGridWidth * 0x80;
    if ((((-1 < (int)cellColumn) && (-1 < (int)cellRow)) && (cellRow < fieldGrid->gridHeight)) &&
       (cellColumn < (fractionSumOrGridWidth & 0x1ffffff))) {
      centerCellIndex = cellRow * (fractionSumOrGridWidth & 0x1ffffff) + cellColumn;
      if ((((fieldGrid->cells[centerCellIndex].flagsAndMaterial & 0x88006000) == 0) &&
          (relativeHeightQ12 = fieldGrid->cells[centerCellIndex].terrainHeight - g_TerrainScanReferenceHeight,
          fieldGrid->cells[centerCellIndex].waterSurfaceDelta < 1)) &&
         ((relativeHeightQ12 <= (int)g_TerrainHeightBandMaximumDelta && ((int)g_TerrainHeightBandMinimumDelta <= relativeHeightQ12))))
      {
        centerOrWedgeCell = (FieldGridCell *)
                 (fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                 centerCellIndex * 0x80 + -0x28);
        wedgeCell = centerOrWedgeCell + -fractionSumOrGridWidth;
        wedgeBlocked = TerrainHeightBand_TestWedge0(0,centerOrWedgeCell);
        if (!wedgeBlocked) {
          centerOrWedgeCell = wedgeCell + -1;
          wedgeBlocked = TerrainHeightBand_TestWedge1(0,wedgeCell);
          if (!wedgeBlocked) {
            wedgeCell = centerOrWedgeCell + (fractionSumOrGridWidth - 1);
            wedgeBlocked = TerrainHeightBand_TestWedge2(0,centerOrWedgeCell);
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
   Ownership: world/terrain/height.
   Purpose: Converts a world point to the hexagonal grid, validates the center cell auxiliary height, and
   dispatches all six threshold wedge tests within the bounded radius. Terrain-class placement test callback; CF
   carries acceptance and EAX carries the direct-call result. Typed parameters: p0
   radiusWorldUnits→FieldGridRadiusUnits. Calling convention, parameter storage, body bytes, control flow, globals,
   locals, and executable data remain unchanged.
   Local calls: TerrainAuxHeightThreshold_TestWedge0, TerrainAuxHeightThreshold_TestWedge1,
   TerrainAuxHeightThreshold_TestWedge2, TerrainAuxHeightThreshold_TestWedge3,
   TerrainAuxHeightThreshold_TestWedge4, TerrainAuxHeightThreshold_TestWedge5.
   Cross-module calls: FieldGrid_WorldToGridQ12 [world/terrain/grid].
*/
bool __thandor_void_preserve_ecx_edx
TerrainAuxHeightThreshold_TestAroundWorldPoint
          (FieldGridRadiusUnits radiusWorldUnits,Q12 referenceHeightQ12,Q12 worldXQ12,Q12 worldYQ12,
          FieldGridAsset *fieldGrid)

{
  uint32_t fractionSumOrGridWidth;
  uint32_t baseColumn;
  uint32_t columnFractionQ12;
  uint32_t rowFractionQ12;
  int centerCellIndex;
  FieldGridCell *wedgeCell;
  FieldGridCell *centerOrWedgeCell;
  bool wedgeBlocked;
  FieldGridCoordinatesEaxEdx8 gridCoordinates;
  uint32_t cellRow;
  uint32_t cellColumn;
  
  if (fieldGrid != (FieldGridAsset *)0x0) {
    g_TerrainScanStepLimit = (uint32_t)radiusWorldUnits / 0x240;
    if (g_TerrainScanStepLimit == 0) {
      g_TerrainScanStepLimit = 1;
    }
    else if (0xff < g_TerrainScanStepLimit) {
      g_TerrainScanStepLimit = 0xff;
    }
    g_TerrainScanReferenceHeight = referenceHeightQ12;
    gridCoordinates = FieldGrid_WorldToGridQ12(worldXQ12,worldYQ12);
    baseColumn = gridCoordinates.columnQ12 >> 0xc;
    cellRow = gridCoordinates.rowQ12 >> 0xc;
    columnFractionQ12 = (uint32_t)(THANDOR_BITCAST(FieldGridCoordinatesEaxEdx8, uint64_t, gridCoordinates) & 0xfff00000fff);
    rowFractionQ12 = (uint32_t)((THANDOR_BITCAST(FieldGridCoordinatesEaxEdx8, uint64_t, gridCoordinates) & 0xfff00000fff) >> 0x20);
    fractionSumOrGridWidth = rowFractionQ12 + columnFractionQ12 * 2;
    cellColumn = baseColumn;
    if (fractionSumOrGridWidth < 0x1000) {
      if (0xfff < columnFractionQ12 + rowFractionQ12 * 2) {
        cellRow = cellRow + 1;
      }
    }
    else if (fractionSumOrGridWidth < 0x2001) {
      cellColumn = baseColumn + 1;
      if (columnFractionQ12 < rowFractionQ12) {
        cellRow = cellRow + 1;
        cellColumn = baseColumn;
      }
    }
    else {
      cellColumn = baseColumn + 1;
      if (0x1fff < columnFractionQ12 + rowFractionQ12 * 2) {
        cellRow = cellRow + 1;
      }
    }
    fractionSumOrGridWidth = fieldGrid->gridWidth;
    g_TerrainScanRowStrideBytes = fractionSumOrGridWidth * 0x80;
    if ((((-1 < (int)cellColumn) && (-1 < (int)cellRow)) && (cellRow < fieldGrid->gridHeight)) &&
       (cellColumn < (fractionSumOrGridWidth & 0x1ffffff))) {
      centerCellIndex = cellRow * (fractionSumOrGridWidth & 0x1ffffff) + cellColumn;
      if ((((fieldGrid->cells[centerCellIndex].flagsAndMaterial & 0x88006000) == 0) &&
          (-1 < fieldGrid->cells[centerCellIndex].waterSurfaceDelta)) &&
         ((int)g_TerrainAuxHeightMinimum <= (int)fieldGrid->cells[centerCellIndex].triangle0NormalAngles >> 0x10))
      {
        centerOrWedgeCell = (FieldGridCell *)
                 (fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                 centerCellIndex * 0x80 + -0x28);
        wedgeCell = centerOrWedgeCell + -fractionSumOrGridWidth;
        wedgeBlocked = TerrainAuxHeightThreshold_TestWedge0(0,centerOrWedgeCell);
        if (!wedgeBlocked) {
          centerOrWedgeCell = wedgeCell + -1;
          wedgeBlocked = TerrainAuxHeightThreshold_TestWedge1(0,wedgeCell);
          if (!wedgeBlocked) {
            wedgeCell = centerOrWedgeCell + (fractionSumOrGridWidth - 1);
            wedgeBlocked = TerrainAuxHeightThreshold_TestWedge2(0,centerOrWedgeCell);
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
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

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
      /* the in-between cell is one row up from directionStartCell (fields +0x50 flags, +0x48 height,
         +0x4C water delta) */
      if ((*(uint32_t *)((int)directionStartCell + (0x50 - heightAdjustmentOrRowStride)) & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      adjacentHeightAdjustmentQ12 =
           g_TerrainScanReferenceHeight - *(int *)((int)directionStartCell + (0x48 - heightAdjustmentOrRowStride));
      adjacentHeightField = (int *)((int)directionStartCell + (0x48 - heightAdjustmentOrRowStride));
      *adjacentHeightField = *adjacentHeightField + adjacentHeightAdjustmentQ12;
      adjacentWaterDeltaField = (int *)((int)directionStartCell + (0x4c - heightAdjustmentOrRowStride));
      *adjacentWaterDeltaField = *adjacentWaterDeltaField - adjacentHeightAdjustmentQ12;
      cell = (FieldGridCell *)((int)directionStartCell + (0x80 - heightAdjustmentOrRowStride));
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      TerrainHeightDelta_ApplyDirection1
                (scanStep,(FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes));
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
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

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
                 (FieldGridCell *)((int)cell + (0x80 - g_TerrainScanRowStrideBytes)));
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      /* the in-between cell is the one above (flags +0x50, height +0x48, water delta +0x4C) */
      if ((*(uint32_t *)((int)cell + (0x50 - heightAdjustmentOrRowStride)) & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      adjacentHeightAdjustmentQ12 =
           g_TerrainScanReferenceHeight - *(int *)((int)cell + (0x48 - heightAdjustmentOrRowStride));
      adjacentHeightField = (int *)((int)cell + (0x48 - heightAdjustmentOrRowStride));
      *adjacentHeightField = *adjacentHeightField + adjacentHeightAdjustmentQ12;
      adjacentWaterDeltaField = (int *)((int)cell + (0x4c - heightAdjustmentOrRowStride));
      *adjacentWaterDeltaField = *adjacentWaterDeltaField - adjacentHeightAdjustmentQ12;
      directionStartCell = (FieldGridCell *)((int)cell + (-g_TerrainScanRowStrideBytes - heightAdjustmentOrRowStride));
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
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

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
                (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,(FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes));
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
      cell = (FieldGridCell *)((int)cell + (-0x80 - g_TerrainScanRowStrideBytes)); /* up and left */
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
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyWedge3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

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
      /* the in-between cell is the one below directionStartCell: runtime60_6B (+0x60) + stride - 0x10/-0x18/-0x14
         reaches its flags (+0x50), height (+0x48) and water delta (+0x4C) */
      if ((*(uint32_t *)(directionStartCell->runtime60_6B + heightAdjustmentOrRowStride - 0x10) &
           FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      adjacentHeightAdjustmentQ12 =
           g_TerrainScanReferenceHeight - *(int *)(directionStartCell->runtime60_6B + heightAdjustmentOrRowStride - 0x18);
      *(int *)(directionStartCell->runtime60_6B + heightAdjustmentOrRowStride - 0x18) =
           *(int *)(directionStartCell->runtime60_6B + heightAdjustmentOrRowStride - 0x18) + adjacentHeightAdjustmentQ12;
      *(int *)(directionStartCell->runtime60_6B + heightAdjustmentOrRowStride - 0x14) =
           *(int *)(directionStartCell->runtime60_6B + heightAdjustmentOrRowStride - 0x14) - adjacentHeightAdjustmentQ12;
      /* runtime0C_3F - 0xC is a cell's own address: directionStartCell[-1] one row down */
      cell = (FieldGridCell *)(directionStartCell[-1].runtime0C_3F + heightAdjustmentOrRowStride - 0xc);
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      TerrainHeightDelta_ApplyDirection4
                (scanStep,(FieldGridCell *)(cell->runtime0C_3F + g_TerrainScanRowStrideBytes - 0xc));
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
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int heightAdjustmentOrRowStride;
  int adjacentHeightAdjustmentQ12;
  uint8_t *currentCellRuntimeBase;

  if (scanStep < g_TerrainScanStepLimit) {
    while ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      heightAdjustmentOrRowStride = g_TerrainScanReferenceHeight - cell->terrainHeight;
      cell->terrainHeight = cell->terrainHeight + heightAdjustmentOrRowStride;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - heightAdjustmentOrRowStride;
      heightAdjustmentOrRowStride = g_TerrainScanRowStrideBytes;
      /* runtime0C_3F - 0xC is a cell's own address: the scan starts one row down and one cell left */
      TerrainHeightDelta_ApplyDirection4
                (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,
                 (FieldGridCell *)(cell[-1].runtime0C_3F + g_TerrainScanRowStrideBytes - 0xc));
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      /* the in-between cell is the one below: flags +0x50, height +0x48, water delta +0x4C */
      if ((*(uint32_t *)(cell->runtime60_6B + heightAdjustmentOrRowStride - 0x10) & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      adjacentHeightAdjustmentQ12 =
           g_TerrainScanReferenceHeight - *(int *)(cell->runtime60_6B + heightAdjustmentOrRowStride - 0x18);
      *(int *)(cell->runtime60_6B + heightAdjustmentOrRowStride - 0x18) =
           *(int *)(cell->runtime60_6B + heightAdjustmentOrRowStride - 0x18) + adjacentHeightAdjustmentQ12;
      *(int *)(cell->runtime60_6B + heightAdjustmentOrRowStride - 0x14) =
           *(int *)(cell->runtime60_6B + heightAdjustmentOrRowStride - 0x14) - adjacentHeightAdjustmentQ12;
      currentCellRuntimeBase = cell->runtime0C_3F;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      /* two rows down: the next diagonal cell one left of the direction 5 start */
      cell = (FieldGridCell *)(currentCellRuntimeBase + g_TerrainScanRowStrideBytes + heightAdjustmentOrRowStride - 0xc)
             - 1;
      TerrainHeightDelta_ApplyDirection5
                (scanStep,(FieldGridCell *)
                          (currentCellRuntimeBase + g_TerrainScanRowStrideBytes + heightAdjustmentOrRowStride - 0xc));
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
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *directionStartCell;
  int heightAdjustmentQ12;
  int adjacentHeightAdjustmentQ12;

  if (scanStep < g_TerrainScanStepLimit) {
    while ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      heightAdjustmentQ12 = g_TerrainScanReferenceHeight - cell->terrainHeight;
      cell->terrainHeight = cell->terrainHeight + heightAdjustmentQ12;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - heightAdjustmentQ12;
      /* runtime0C_3F - 0xC is a cell's own address: the scan starts one row down */
      TerrainHeightDelta_ApplyDirection5
                (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,
                 (FieldGridCell *)(cell->runtime0C_3F + g_TerrainScanRowStrideBytes - 0xc));
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
      cell = (FieldGridCell *)(cell[1].runtime0C_3F + g_TerrainScanRowStrideBytes - 0xc); /* below the right cell */
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
TerrainRayTriangleResult __thandor_eax_cf_preserve_ecx_edx
TerrainTriangle_IntersectRayDistance
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
bool __thandor_cf_preserve_eax
TerrainRay_AdvanceGridTraversal
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
    g_TerrainRayNextCell = (FieldGridCell *)(cell - 0x80);
    g_TerrainRayNextCoord1Q12 = currentGridCoord1Q12 - FIELD_GRID_CELL_Q12;
    return true;
  }
  /* next column: 0x80 bytes = one cell */
  if (delta1 < 0) {
    g_TerrainRayNextCell = (FieldGridCell *)(cell - 0x80);
    g_TerrainRayNextCoord1Q12 = currentGridCoord1Q12 - FIELD_GRID_CELL_Q12;
  }
  else {
    g_TerrainRayNextCell = (FieldGridCell *)(cell + 0x80);
    g_TerrainRayNextCoord1Q12 = currentGridCoord1Q12 + FIELD_GRID_CELL_Q12;
  }
  return false;
}


/* Address: 0x00507AB0.
   Ownership: world/terrain/height.
   Purpose: Tests both directional legs of terrain wedge 0, stops when a cell leaves the configured height band or
   becomes excluded, and preserves the verified carry-style failure path. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged. Typed parameters: p3
   cell→FieldGridCell *. Calling convention, complete VariableStorage serialization, function bytes, control flow,
   globals, locals, and executable data remain unchanged.
   Local calls: TerrainHeightBand_TestDirection0, TerrainHeightBand_TestDirection1.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int rowStrideBytes;
  int relativeHeightQ12;
  FieldGridCell *directionStartCell;
  bool directionFailed;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      if (((((cell->flagsAndMaterial & 0x88006000) != 0) ||
           (relativeHeightQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight, 0 < cell->waterSurfaceDelta)
           ) || ((int)g_TerrainHeightBandMaximumDelta < relativeHeightQ12)) ||
         (relativeHeightQ12 < (int)g_TerrainHeightBandMinimumDelta)) {
        return true;
      }
      directionStartCell = cell + 1;
      directionFailed = TerrainHeightBand_TestDirection0(scanStep + 4,directionStartCell);
      if (directionFailed) {
        return true;
      }
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return false;
      }
      if ((*(uint32_t *)((int)directionStartCell + (0x50 - rowStrideBytes)) & 0x88006000) != 0) {
        return true;
      }
      relativeHeightQ12 = *(int *)((int)directionStartCell + (0x48 - rowStrideBytes)) - g_TerrainScanReferenceHeight;
      if (0 < *(int *)((int)directionStartCell + (0x4c - rowStrideBytes))) {
        return true;
      }
      if ((int)g_TerrainHeightBandMaximumDelta < relativeHeightQ12) {
        return true;
      }
      if (relativeHeightQ12 < (int)g_TerrainHeightBandMinimumDelta) {
        return true;
      }
      cell = (FieldGridCell *)((int)directionStartCell + (0x80 - rowStrideBytes));
      scanStep = scanStep + 7;
      directionFailed = TerrainHeightBand_TestDirection1
                        (scanStep,(FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes));
      if (directionFailed) {
        return true;
      }
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return false;
}


/* Address: 0x00507BA0.
   Ownership: world/terrain/height.
   Purpose: Tests both directional legs of terrain wedge 1, stops when a cell leaves the configured height band or
   becomes excluded, and preserves the verified carry-style failure path. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainHeightBand_TestDirection1, TerrainHeightBand_TestDirection2.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int rowStrideBytes;
  int relativeHeightQ12;
  FieldGridCell *directionStartCell;
  bool directionFailed;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      if (((((cell->flagsAndMaterial & 0x88006000) != 0) ||
           (relativeHeightQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight, 0 < cell->waterSurfaceDelta)
           ) || ((int)g_TerrainHeightBandMaximumDelta < relativeHeightQ12)) ||
         (relativeHeightQ12 < (int)g_TerrainHeightBandMinimumDelta)) {
        return true;
      }
      directionFailed = TerrainHeightBand_TestDirection1
                        (scanStep + 4,
                         (FieldGridCell *)((int)cell + (0x80 - g_TerrainScanRowStrideBytes)));
      if (directionFailed) {
        return true;
      }
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return false;
      }
      if ((*(uint32_t *)((int)cell + (0x50 - rowStrideBytes)) & 0x88006000) != 0) {
        return true;
      }
      relativeHeightQ12 = *(int *)((int)cell + (0x48 - rowStrideBytes)) - g_TerrainScanReferenceHeight;
      if (0 < *(int *)((int)cell + (0x4c - rowStrideBytes))) {
        return true;
      }
      if ((int)g_TerrainHeightBandMaximumDelta < relativeHeightQ12) {
        return true;
      }
      if (relativeHeightQ12 < (int)g_TerrainHeightBandMinimumDelta) {
        return true;
      }
      directionStartCell = (FieldGridCell *)((int)cell + (-g_TerrainScanRowStrideBytes - rowStrideBytes));
      scanStep = scanStep + 7;
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
   Ownership: world/terrain/height.
   Purpose: Tests both directional legs of terrain wedge 2, stops when a cell leaves the configured height band or
   becomes excluded, and preserves the verified carry-style failure path. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged. Typed parameters: p3
   cell→FieldGridCell *. Calling convention, complete VariableStorage serialization, function bytes, control flow,
   globals, locals, and executable data remain unchanged.
   Local calls: TerrainHeightBand_TestDirection2, TerrainHeightBand_TestDirection3.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *directionStartCell;
  int relativeHeightQ12;
  bool directionFailed;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if (((((cell->flagsAndMaterial & 0x88006000) != 0) ||
           (relativeHeightQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight, 0 < cell->waterSurfaceDelta)
           ) || ((int)g_TerrainHeightBandMaximumDelta < relativeHeightQ12)) ||
         (relativeHeightQ12 < (int)g_TerrainHeightBandMinimumDelta)) {
        return true;
      }
      directionFailed = TerrainHeightBand_TestDirection2
                        (scanStep + 4,(FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes));
      if (directionFailed) {
        return true;
      }
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return false;
      }
      if ((cell[-1].flagsAndMaterial & 0x88006000) != 0) {
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
      directionStartCell = cell + -2;
      scanStep = scanStep + 7;
      cell = (FieldGridCell *)((int)cell + (-0x80 - g_TerrainScanRowStrideBytes));
      directionFailed = TerrainHeightBand_TestDirection3(scanStep,directionStartCell);
      if (directionFailed) {
        return true;
      }
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return false;
}


/* Address: 0x00507D60.
   Ownership: world/terrain/height.
   Purpose: Tests both directional legs of terrain wedge 3, stops when a cell leaves the configured height band or
   becomes excluded, and preserves the verified carry-style failure path. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainHeightBand_TestDirection3, TerrainHeightBand_TestDirection4.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestWedge3(TerrainDirectionalScanStep scanStep,uint8_t *cell)

{
  int rowStrideBytes;
  int relativeHeightQ12;
  FieldGridCell *directionStartCell;
  bool directionFailed;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      if (((((*(uint32_t *)(cell + 0x50) & 0x88006000) != 0) ||
           (0 < (int)*(PackedArgb32 *)(cell + 0x4c))) ||
          ((int)g_TerrainHeightBandMaximumDelta <
           (int)(*(FieldCellPersistedAux *)(cell + 0x48) - g_TerrainScanReferenceHeight))) ||
         ((int)(*(FieldCellPersistedAux *)(cell + 0x48) - g_TerrainScanReferenceHeight) <
          (int)g_TerrainHeightBandMinimumDelta)) {
        return true;
      }
      directionStartCell = (FieldGridCell *)(cell + -0x80);
      directionFailed = TerrainHeightBand_TestDirection3(scanStep + 4,directionStartCell);
      if (directionFailed) {
        return true;
      }
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return false;
      }
      if ((*(uint32_t *)(directionStartCell->runtime60_6B + rowStrideBytes + -0x10) & 0x88006000) != 0) {
        return true;
      }
      relativeHeightQ12 = *(int *)(directionStartCell->runtime60_6B + rowStrideBytes + -0x18) - g_TerrainScanReferenceHeight;
      if (0 < *(int *)(directionStartCell->runtime60_6B + rowStrideBytes + -0x14)) {
        return true;
      }
      if ((int)g_TerrainHeightBandMaximumDelta < relativeHeightQ12) {
        return true;
      }
      if (relativeHeightQ12 < (int)g_TerrainHeightBandMinimumDelta) {
        return true;
      }
      cell = directionStartCell[-1].runtime0C_3F + rowStrideBytes + -0xc;
      scanStep = scanStep + 7;
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
   Ownership: world/terrain/height.
   Purpose: Tests both directional legs of terrain wedge 4, stops when a cell leaves the configured height band or
   becomes excluded, and preserves the verified carry-style failure path. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainHeightBand_TestDirection4, TerrainHeightBand_TestDirection5.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  uint8_t *cellRuntimeBase;
  int rowStrideBytes;
  int relativeHeightQ12;
  bool directionFailed;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      if (((((cell->flagsAndMaterial & 0x88006000) != 0) ||
           (relativeHeightQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight, 0 < cell->waterSurfaceDelta)
           ) || ((int)g_TerrainHeightBandMaximumDelta < relativeHeightQ12)) ||
         (relativeHeightQ12 < (int)g_TerrainHeightBandMinimumDelta)) {
        return true;
      }
      directionFailed = TerrainHeightBand_TestDirection4
                        (scanStep + 4,
                         (FieldGridCell *)
                         (cell[-1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc));
      if (directionFailed) {
        return true;
      }
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return false;
      }
      if ((*(uint32_t *)(cell->runtime60_6B + rowStrideBytes + -0x10) & 0x88006000) != 0) {
        return true;
      }
      relativeHeightQ12 = *(int *)(cell->runtime60_6B + rowStrideBytes + -0x18) - g_TerrainScanReferenceHeight;
      if (0 < *(int *)(cell->runtime60_6B + rowStrideBytes + -0x14)) {
        return true;
      }
      if ((int)g_TerrainHeightBandMaximumDelta < relativeHeightQ12) {
        return true;
      }
      if (relativeHeightQ12 < (int)g_TerrainHeightBandMinimumDelta) {
        return true;
      }
      cellRuntimeBase = cell->runtime0C_3F;
      scanStep = scanStep + 7;
      cell = (FieldGridCell *)(cellRuntimeBase + g_TerrainScanRowStrideBytes + rowStrideBytes + -0xc) + -1;
      directionFailed = TerrainHeightBand_TestDirection5
                        (scanStep,(FieldGridCell *)
                                  (cellRuntimeBase + g_TerrainScanRowStrideBytes + rowStrideBytes + -0xc));
      if (directionFailed) {
        return true;
      }
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return false;
}


/* Address: 0x00507F20.
   Ownership: world/terrain/height.
   Purpose: Tests both directional legs of terrain wedge 5, stops when a cell leaves the configured height band or
   becomes excluded, and preserves the verified carry-style failure path. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged. Typed parameters: p3
   cell→FieldGridCell *. Calling convention, complete VariableStorage serialization, function bytes, control flow,
   globals, locals, and executable data remain unchanged.
   Local calls: TerrainHeightBand_TestDirection5, TerrainHeightBand_TestDirection0.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *directionStartCell;
  int relativeHeightQ12;
  bool directionFailed;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if (((((cell->flagsAndMaterial & 0x88006000) != 0) ||
           (relativeHeightQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight, 0 < cell->waterSurfaceDelta)
           ) || ((int)g_TerrainHeightBandMaximumDelta < relativeHeightQ12)) ||
         (relativeHeightQ12 < (int)g_TerrainHeightBandMinimumDelta)) {
        return true;
      }
      directionFailed = TerrainHeightBand_TestDirection5
                        (scanStep + 4,
                         (FieldGridCell *)(cell->runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc))
      ;
      if (directionFailed) {
        return true;
      }
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return false;
      }
      if ((cell[1].flagsAndMaterial & 0x88006000) != 0) {
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
      scanStep = scanStep + 7;
      cell = (FieldGridCell *)(cell[1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc);
      directionFailed = TerrainHeightBand_TestDirection0(scanStep,directionStartCell);
      if (directionFailed) {
        return true;
      }
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return false;
}


/* Address: 0x00508470.
   Ownership: world/terrain/height.
   Purpose: Tests both directional legs of terrain wedge 0 against the shared auxiliary-height threshold, stopping
   at excluded or invalid cells and preserving the verified carry-style failure path. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainAuxHeightThreshold_TestDirection0, TerrainAuxHeightThreshold_TestDirection1.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int rowStrideBytes;
  FieldGridCell *directionStartCell;
  bool directionFailed;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
         ((int)cell->triangle1NormalAngles >> 0x10 < (int)g_TerrainAuxHeightMinimum)) {
        return true;
      }
      directionStartCell = cell + 1;
      directionFailed = TerrainAuxHeightThreshold_TestDirection0(scanStep + 4,directionStartCell);
      if (directionFailed) {
        return true;
      }
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return false;
      }
      if ((*(uint32_t *)((int)directionStartCell + (0x50 - rowStrideBytes)) & 0x88006000) != 0) {
        return true;
      }
      if (*(int *)((int)directionStartCell + (0x4c - rowStrideBytes)) < 0) {
        return true;
      }
      if (*(int *)((int)directionStartCell + (0x78 - rowStrideBytes)) >> 0x10 < (int)g_TerrainAuxHeightMinimum) {
        return true;
      }
      cell = (FieldGridCell *)((int)directionStartCell + (0x80 - rowStrideBytes));
      scanStep = scanStep + 7;
      directionFailed = TerrainAuxHeightThreshold_TestDirection1
                        (scanStep,(FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes));
      if (directionFailed) {
        return true;
      }
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return false;
}


/* Address: 0x00508540.
   Ownership: world/terrain/height.
   Purpose: Tests both directional legs of terrain wedge 1 against the shared auxiliary-height threshold, stopping
   at excluded or invalid cells and preserving the verified carry-style failure path. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainAuxHeightThreshold_TestDirection1, TerrainAuxHeightThreshold_TestDirection2.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int rowStrideBytes;
  FieldGridCell *directionStartCell;
  bool directionFailed;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
         ((int)cell->triangle1NormalAngles >> 0x10 < (int)g_TerrainAuxHeightMinimum)) {
        return true;
      }
      directionFailed = TerrainAuxHeightThreshold_TestDirection1
                        (scanStep + 4,
                         (FieldGridCell *)((int)cell + (0x80 - g_TerrainScanRowStrideBytes)));
      if (directionFailed) {
        return true;
      }
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return false;
      }
      if ((*(uint32_t *)((int)cell + (0x50 - rowStrideBytes)) & 0x88006000) != 0) {
        return true;
      }
      if (*(int *)((int)cell + (0x4c - rowStrideBytes)) < 0) {
        return true;
      }
      if (*(int *)((int)cell + (0x78 - rowStrideBytes)) >> 0x10 < (int)g_TerrainAuxHeightMinimum) {
        return true;
      }
      directionStartCell = (FieldGridCell *)((int)cell + (-g_TerrainScanRowStrideBytes - rowStrideBytes));
      scanStep = scanStep + 7;
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
   Ownership: world/terrain/height.
   Purpose: Tests both directional legs of terrain wedge 2 against the shared auxiliary-height threshold, stopping
   at excluded or invalid cells and preserving the verified carry-style failure path. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainAuxHeightThreshold_TestDirection2, TerrainAuxHeightThreshold_TestDirection3.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *directionStartCell;
  bool directionFailed;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
         ((int)cell->triangle1NormalAngles >> 0x10 < (int)g_TerrainAuxHeightMinimum)) {
        return true;
      }
      directionFailed = TerrainAuxHeightThreshold_TestDirection2
                        (scanStep + 4,(FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes));
      if (directionFailed) {
        return true;
      }
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return false;
      }
      if ((cell[-1].flagsAndMaterial & 0x88006000) != 0) {
        return true;
      }
      if (cell[-1].waterSurfaceDelta < 0) {
        return true;
      }
      if ((int)cell[-1].triangle1NormalAngles >> 0x10 < (int)g_TerrainAuxHeightMinimum) {
        return true;
      }
      directionStartCell = cell + -2;
      scanStep = scanStep + 7;
      cell = (FieldGridCell *)((int)cell + (-0x80 - g_TerrainScanRowStrideBytes));
      directionFailed = TerrainAuxHeightThreshold_TestDirection3(scanStep,directionStartCell);
      if (directionFailed) {
        return true;
      }
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return false;
}


/* Address: 0x005086C0.
   Ownership: world/terrain/height.
   Purpose: Tests both directional legs of terrain wedge 3 against the shared auxiliary-height threshold, stopping
   at excluded or invalid cells and preserving the verified carry-style failure path. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainAuxHeightThreshold_TestDirection3, TerrainAuxHeightThreshold_TestDirection4.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestWedge3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int rowStrideBytes;
  FieldGridCell *directionStartCell;
  bool directionFailed;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
         ((int)cell->triangle1NormalAngles >> 0x10 < (int)g_TerrainAuxHeightMinimum)) {
        return true;
      }
      directionStartCell = cell + -1;
      directionFailed = TerrainAuxHeightThreshold_TestDirection3(scanStep + 4,directionStartCell);
      if (directionFailed) {
        return true;
      }
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return false;
      }
      if ((*(uint32_t *)(directionStartCell->runtime60_6B + rowStrideBytes + -0x10) & 0x88006000) != 0) {
        return true;
      }
      if (*(int *)(directionStartCell->runtime60_6B + rowStrideBytes + -0x14) < 0) {
        return true;
      }
      if (*(int *)(directionStartCell->runtime60_6B + rowStrideBytes + 0x18) >> 0x10 < (int)g_TerrainAuxHeightMinimum) {
        return true;
      }
      cell = (FieldGridCell *)(directionStartCell[-1].runtime0C_3F + rowStrideBytes + -0xc);
      scanStep = scanStep + 7;
      directionFailed = TerrainAuxHeightThreshold_TestDirection4
                        (scanStep,(FieldGridCell *)
                                  (cell->runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc));
      if (directionFailed) {
        return true;
      }
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return false;
}


/* Address: 0x00508790.
   Ownership: world/terrain/height.
   Purpose: Tests both directional legs of terrain wedge 4 against the shared auxiliary-height threshold, stopping
   at excluded or invalid cells and preserving the verified carry-style failure path. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainAuxHeightThreshold_TestDirection4, TerrainAuxHeightThreshold_TestDirection5.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  uint8_t *cellRuntimeBase;
  int rowStrideBytes;
  bool directionFailed;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
         ((int)cell->triangle1NormalAngles >> 0x10 < (int)g_TerrainAuxHeightMinimum)) {
        return true;
      }
      directionFailed = TerrainAuxHeightThreshold_TestDirection4
                        (scanStep + 4,
                         (FieldGridCell *)
                         (cell[-1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc));
      if (directionFailed) {
        return true;
      }
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return false;
      }
      if ((*(uint32_t *)(cell->runtime60_6B + rowStrideBytes + -0x10) & 0x88006000) != 0) {
        return true;
      }
      if (*(int *)(cell->runtime60_6B + rowStrideBytes + -0x14) < 0) {
        return true;
      }
      if (*(int *)(cell->runtime60_6B + rowStrideBytes + 0x18) >> 0x10 < (int)g_TerrainAuxHeightMinimum) {
        return true;
      }
      cellRuntimeBase = cell->runtime0C_3F;
      scanStep = scanStep + 7;
      cell = (FieldGridCell *)(cellRuntimeBase + g_TerrainScanRowStrideBytes + rowStrideBytes + -0xc) + -1;
      directionFailed = TerrainAuxHeightThreshold_TestDirection5
                        (scanStep,(FieldGridCell *)
                                  (cellRuntimeBase + g_TerrainScanRowStrideBytes + rowStrideBytes + -0xc));
      if (directionFailed) {
        return true;
      }
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return false;
}


/* Address: 0x00508850.
   Ownership: world/terrain/height.
   Purpose: Tests both directional legs of terrain wedge 5 against the shared auxiliary-height threshold, stopping
   at excluded or invalid cells and preserving the verified carry-style failure path. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainAuxHeightThreshold_TestDirection5, TerrainAuxHeightThreshold_TestDirection0.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *directionStartCell;
  bool directionFailed;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
         ((int)cell->triangle1NormalAngles >> 0x10 < (int)g_TerrainAuxHeightMinimum)) {
        return true;
      }
      directionFailed = TerrainAuxHeightThreshold_TestDirection5
                        (scanStep + 4,
                         (FieldGridCell *)(cell->runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc))
      ;
      if (directionFailed) {
        return true;
      }
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return false;
      }
      if ((cell[1].flagsAndMaterial & 0x88006000) != 0) {
        return true;
      }
      if (cell[1].waterSurfaceDelta < 0) {
        return true;
      }
      if ((int)cell[1].triangle1NormalAngles >> 0x10 < (int)g_TerrainAuxHeightMinimum) {
        return true;
      }
      directionStartCell = cell + 2;
      scanStep = scanStep + 7;
      cell = (FieldGridCell *)(cell[1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc);
      directionFailed = TerrainAuxHeightThreshold_TestDirection0(scanStep,directionStartCell);
      if (directionFailed) {
        return true;
      }
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return false;
}


/* Address: 0x005077F0.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 0 while cells are eligible, have no positive surface delta, and their
   terrain height relative to the shared origin stays inside the configured lower and upper band. Typed parameters:
   p2 scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function
   body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int terrainHeightDeltaQ12;
  
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if (((((cell->flagsAndMaterial & 0x88006000) != 0) ||
         (terrainHeightDeltaQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight,
         0 < cell->waterSurfaceDelta)) || ((int)g_TerrainHeightBandMaximumDelta < terrainHeightDeltaQ12))
       || (terrainHeightDeltaQ12 < (int)g_TerrainHeightBandMinimumDelta)) break;
    scanStep = scanStep + 4;
    cell = cell + 1;
  }
  return true;
}


/* Address: 0x00507860.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 1 while cells are eligible, have no positive surface delta, and their
   terrain height relative to the shared origin stays inside the configured lower and upper band. Typed parameters:
   p2 scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function
   body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int terrainHeightDeltaQ12;
  
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if (((((cell->flagsAndMaterial & 0x88006000) != 0) ||
         (terrainHeightDeltaQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight,
         0 < cell->waterSurfaceDelta)) || ((int)g_TerrainHeightBandMaximumDelta < terrainHeightDeltaQ12))
       || (terrainHeightDeltaQ12 < (int)g_TerrainHeightBandMinimumDelta)) break;
    scanStep = scanStep + 4;
    cell = (FieldGridCell *)((int)cell + (0x80 - g_TerrainScanRowStrideBytes));
  }
  return true;
}


/* Address: 0x005078E0.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 2 while cells are eligible, have no positive surface delta, and their
   terrain height relative to the shared origin stays inside the configured lower and upper band. Typed parameters:
   p2 scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function
   body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int terrainHeightDeltaQ12;
  
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if (((((cell->flagsAndMaterial & 0x88006000) != 0) ||
         (terrainHeightDeltaQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight,
         0 < cell->waterSurfaceDelta)) || ((int)g_TerrainHeightBandMaximumDelta < terrainHeightDeltaQ12))
       || (terrainHeightDeltaQ12 < (int)g_TerrainHeightBandMinimumDelta)) break;
    scanStep = scanStep + 4;
    cell = (FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes);
  }
  return true;
}


/* Address: 0x00507950.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 3 while cells are eligible, have no positive surface delta, and their
   terrain height relative to the shared origin stays inside the configured lower and upper band. Typed parameters:
   p2 scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function
   body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int terrainHeightDeltaQ12;
  
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if (((((cell->flagsAndMaterial & 0x88006000) != 0) ||
         (terrainHeightDeltaQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight,
         0 < cell->waterSurfaceDelta)) || ((int)g_TerrainHeightBandMaximumDelta < terrainHeightDeltaQ12))
       || (terrainHeightDeltaQ12 < (int)g_TerrainHeightBandMinimumDelta)) break;
    scanStep = scanStep + 4;
    cell = cell + -1;
  }
  return true;
}


/* Address: 0x005079C0.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 4 while cells are eligible, have no positive surface delta, and their
   terrain height relative to the shared origin stays inside the configured lower and upper band. Typed parameters:
   p2 scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function
   body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int terrainHeightDeltaQ12;
  
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if (((((cell->flagsAndMaterial & 0x88006000) != 0) ||
         (terrainHeightDeltaQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight,
         0 < cell->waterSurfaceDelta)) || ((int)g_TerrainHeightBandMaximumDelta < terrainHeightDeltaQ12))
       || (terrainHeightDeltaQ12 < (int)g_TerrainHeightBandMinimumDelta)) break;
    scanStep = scanStep + 4;
    cell = (FieldGridCell *)(cell[-1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc);
  }
  return true;
}


/* Address: 0x00507A40.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 5 while cells are eligible, have no positive surface delta, and their
   terrain height relative to the shared origin stays inside the configured lower and upper band. Typed parameters:
   p2 scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function
   body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int terrainHeightDeltaQ12;
  
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if (((((cell->flagsAndMaterial & 0x88006000) != 0) ||
         (terrainHeightDeltaQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight,
         0 < cell->waterSurfaceDelta)) || ((int)g_TerrainHeightBandMaximumDelta < terrainHeightDeltaQ12))
       || (terrainHeightDeltaQ12 < (int)g_TerrainHeightBandMinimumDelta)) break;
    scanStep = scanStep + 4;
    cell = (FieldGridCell *)(cell->runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc);
  }
  return true;
}


/* Address: 0x005081D0.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 0 while cells are eligible, have a nonnegative surface delta, and the
   signed high word of the auxiliary height field remains at or above the shared threshold. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
       ((int)cell->triangle1NormalAngles >> 0x10 < (int)g_TerrainAuxHeightMinimum)) break;
    scanStep = scanStep + 4;
    cell = cell + 1;
  }
  return true;
}


/* Address: 0x00508240.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 1 while cells are eligible, have a nonnegative surface delta, and the
   signed high word of the auxiliary height field remains at or above the shared threshold. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
       ((int)cell->triangle1NormalAngles >> 0x10 < (int)g_TerrainAuxHeightMinimum)) break;
    scanStep = scanStep + 4;
    cell = (FieldGridCell *)((int)cell + (0x80 - g_TerrainScanRowStrideBytes));
  }
  return true;
}


/* Address: 0x005082B0.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 2 while cells are eligible, have a nonnegative surface delta, and the
   signed high word of the auxiliary height field remains at or above the shared threshold. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
       ((int)cell->triangle1NormalAngles >> 0x10 < (int)g_TerrainAuxHeightMinimum)) break;
    scanStep = scanStep + 4;
    cell = (FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes);
  }
  return true;
}


/* Address: 0x00508320.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 3 while cells are eligible, have a nonnegative surface delta, and the
   signed high word of the auxiliary height field remains at or above the shared threshold. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
       ((int)cell->triangle1NormalAngles >> 0x10 < (int)g_TerrainAuxHeightMinimum)) break;
    scanStep = scanStep + 4;
    cell = cell + -1;
  }
  return true;
}


/* Address: 0x00508390.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 4 while cells are eligible, have a nonnegative surface delta, and the
   signed high word of the auxiliary height field remains at or above the shared threshold. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
       ((int)cell->triangle1NormalAngles >> 0x10 < (int)g_TerrainAuxHeightMinimum)) break;
    scanStep = scanStep + 4;
    cell = (FieldGridCell *)(cell[-1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc);
  }
  return true;
}


/* Address: 0x00508400.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 5 while cells are eligible, have a nonnegative surface delta, and the
   signed high word of the auxiliary height field remains at or above the shared threshold. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
       ((int)cell->triangle1NormalAngles >> 0x10 < (int)g_TerrainAuxHeightMinimum)) break;
    scanStep = scanStep + 4;
    cell = (FieldGridCell *)(cell->runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc);
  }
  return true;
}


/* Address: 0x00508AE0.
   Flatten brush, straight leg along direction 0 (C+1, right): levels each cell to g_TerrainScanReferenceHeight
   and takes the change out of waterSurfaceDelta so the water surface stays where it was. 4 scan steps per cell,
   until the step limit or a map-edge cell.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

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
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

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
      cell = (FieldGridCell *)((int)cell + (0x80 - g_TerrainScanRowStrideBytes)); /* 0x80 = one cell */
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00508BA0.
   Flatten brush, straight leg along direction 2 (C-W, up): levels each cell to g_TerrainScanReferenceHeight,
   keeping the water surface (see TerrainHeightDelta_ApplyDirection0).
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

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
      cell = (FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00508C00.
   Flatten brush, straight leg along direction 3 (C-1, left): levels each cell to g_TerrainScanReferenceHeight,
   keeping the water surface (see TerrainHeightDelta_ApplyDirection0).
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

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
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

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
      /* runtime0C_3F (+0x0C) - 0xC is a cell's own address */
      cell = (FieldGridCell *)(cell[-1].runtime0C_3F + g_TerrainScanRowStrideBytes - 0xc);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00508CC0.
   Flatten brush, straight leg along direction 5 (C+W, down): levels each cell to g_TerrainScanReferenceHeight,
   keeping the water surface (see TerrainHeightDelta_ApplyDirection0).
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

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
      /* runtime0C_3F (+0x0C) - 0xC is a cell's own address */
      cell = (FieldGridCell *)(cell->runtime0C_3F + g_TerrainScanRowStrideBytes - 0xc);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}

