/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/sight.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* Line of sight over the terrain: the occlusion mask around a world point, traced along the six hexagon
   wedges and directions (fog-of-war visibility, not screen projection). */

#include <thandor/world/terrain/sight.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

/* Q12 height-delta scale per scan step: entry n = 4 / (n + 4) in Q12, rounded to nearest
   ((16384 + (n + 4) / 2) / (n + 4); checked against every entry) */
static const int32_t g_TerrainHeightDeltaScaleByStepQ12[256] = {
    /*   0 */ 4096, 3277, 2731, 2341, 2048, 1820, 1638, 1489, 1365, 1260, 1170, 1092, 1024, 964, 910, 862,
    /*  16 */ 819, 780, 745, 712, 683, 655, 630, 607, 585, 565, 546, 529, 512, 496, 482, 468,
    /*  32 */ 455, 443, 431, 420, 410, 400, 390, 381, 372, 364, 356, 349, 341, 334, 328, 321,
    /*  48 */ 315, 309, 303, 298, 293, 287, 282, 278, 273, 269, 264, 260, 256, 252, 248, 245,
    /*  64 */ 241, 237, 234, 231, 228, 224, 221, 218, 216, 213, 210, 207, 205, 202, 200, 197,
    /*  80 */ 195, 193, 191, 188, 186, 184, 182, 180, 178, 176, 174, 172, 171, 169, 167, 165,
    /*  96 */ 164, 162, 161, 159, 158, 156, 155, 153, 152, 150, 149, 148, 146, 145, 144, 142,
    /* 112 */ 141, 140, 139, 138, 137, 135, 134, 133, 132, 131, 130, 129, 128, 127, 126, 125,
    /* 128 */ 124, 123, 122, 121, 120, 120, 119, 118, 117, 116, 115, 115, 114, 113, 112, 111,
    /* 144 */ 111, 110, 109, 109, 108, 107, 106, 106, 105, 104, 104, 103, 102, 102, 101, 101,
    /* 160 */ 100, 99, 99, 98, 98, 97, 96, 96, 95, 95, 94, 94, 93, 93, 92, 92,
    /* 176 */ 91, 91, 90, 90, 89, 89, 88, 88, 87, 87, 86, 86, 85, 85, 84, 84,
    /* 192 */ 84, 83, 83, 82, 82, 82, 81, 81, 80, 80, 80, 79, 79, 78, 78, 78,
    /* 208 */ 77, 77, 77, 76, 76, 76, 75, 75, 74, 74, 74, 73, 73, 73, 72, 72,
    /* 224 */ 72, 72, 71, 71, 71, 70, 70, 70, 69, 69, 69, 69, 68, 68, 68, 67,
    /* 240 */ 67, 67, 67, 66, 66, 66, 66, 65, 65, 65, 65, 64, 64, 64, 64, 63};

/* Line-of-sight marking for one army (occupancy rebuild): from the grid vertex nearest to the world point, ORs
   occupancyMaskBits (the bits of the factions that share the army's sight) into every cell the terrain does not
   hide from an eye at referenceHeightQ12 within the radius. The centre cell is marked here, the rest by the six
   sector traces, each seeded with the height of the next sector's first cell. Nothing happens outside the grid or
   on a map-edge cell.
*/
void TerrainProjectedOcclusion_AccumulateMaskAroundWorldPoint
          (uint64_t occupancyMaskBits,FieldGridRadiusUnits radiusWorldUnits,Q12 referenceHeightQ12,
          Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid)

{
  int rowStrideBytes;
  int referenceHeight;
  uint32_t gridWidth;
  int centerCellIndex;
  FieldGridCell *wedgeCellA;
  FieldGridCell *wedgeCellB;
  FieldGridCoordinates gridCoordinates;
  uint32_t gridRow;
  uint32_t gridColumn;

  if (fieldGrid != nullptr) {
    TerrainProjectedScan_SetStepLimitFromRadius(radiusWorldUnits);
    g_TerrainScanReferenceHeight = referenceHeightQ12;
    gridCoordinates = FieldGrid_WorldToGridQ12(worldYQ12,worldXQ12);
    referenceHeight = g_TerrainScanReferenceHeight;
    TerrainProjectedScan_SelectNearestGridVertex(gridCoordinates,&gridRow,&gridColumn);
    g_TerrainScanRowStrideBytes = fieldGrid->gridWidth << 7; /* 0x80-byte cells */
    gridWidth = fieldGrid->gridWidth & FIELD_GRID_ROW_STRIDE_WIDTH_MASK;
    if ((-1 < (int)gridColumn) && (-1 < (int)gridRow) && (gridRow < fieldGrid->gridHeight) &&
        (gridColumn < gridWidth)) {
      centerCellIndex = gridRow * gridWidth + gridColumn;
      if ((fieldGrid->cells[centerCellIndex].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
        fieldGrid->cells[centerCellIndex].occupancyMask =
             fieldGrid->cells[centerCellIndex].occupancyMask | occupancyMaskBits;
        rowStrideBytes = g_TerrainScanRowStrideBytes;
        /* the six neighbours of centre cell C, one per sector: C+1, C+1-W, C-W, C-1, C-1+W, C+W (W = grid width;
           the first address is cells[centerCellIndex + 1]) */
        wedgeCellA = &fieldGrid->cells[centerCellIndex + 1];
        wedgeCellB = FIELD_GRID_CELL_AT_BYTE_OFFSET(wedgeCellA,-g_TerrainScanRowStrideBytes);
        TerrainProjectedOcclusion_TraceWedge0
                  (occupancyMaskBits,wedgeCellB->terrainHeight - referenceHeight,0,wedgeCellA);
        TerrainProjectedOcclusion_TraceWedge1
                  (occupancyMaskBits,wedgeCellB[-1].terrainHeight - referenceHeight,0,wedgeCellB);
        wedgeCellA = FIELD_GRID_CELL_AT_BYTE_OFFSET(wedgeCellB - 2,rowStrideBytes);
        TerrainProjectedOcclusion_TraceWedge2
                  (occupancyMaskBits,wedgeCellA->terrainHeight - referenceHeight,0,wedgeCellB - 1);
        wedgeCellB = FIELD_GRID_CELL_AT_BYTE_OFFSET(wedgeCellA,rowStrideBytes);
        TerrainProjectedOcclusion_TraceWedge3
                  (occupancyMaskBits,wedgeCellB->terrainHeight - referenceHeight,0,wedgeCellA);
        TerrainProjectedOcclusion_TraceWedge4
                  (occupancyMaskBits,wedgeCellB[1].terrainHeight - referenceHeight,0,wedgeCellB);
        /* the terrain height of C+1 (one row up from C+W+1), the first cell of sector 0 */
        TerrainProjectedOcclusion_TraceWedge5
                  (occupancyMaskBits,
                   FIELD_GRID_CELL_AT_BYTE_OFFSET(wedgeCellB + 2,-rowStrideBytes)->terrainHeight - referenceHeight,0,
                   wedgeCellB + 1);
      }
    }
  }
}

/* Line-of-sight marking for the sector between directions 0 (C+1) and 1 (C+1-W) of
   TerrainProjectedOcclusion_AccumulateMaskAroundWorldPoint. Walks the sector's spine (step C+2-W, scan step +7)
   with a running horizon like TerrainProjectedOcclusion_ScanDirection0, tests the direction-1 neighbour between
   two spine cells, and hands the current horizon to a straight leg along each bounding direction. The first spine
   cell is tested against its own unscaled height above the eye; unless it is visible, the legs start from the
   average of that height and the caller's projectedHeightThresholdQ20.
*/
void TerrainProjectedOcclusion_TraceWedge0(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int64_t scaledHeightProduct;
  int cellHeight;
  int rowStrideBytes;
  int neighborHeight;
  uint32_t projectedHeight;
  TerrainDirectionalScanStep legStep;
  TerrainProjectedHeightThresholdQ20 cellThreshold;
  FieldGridCell *adjacentCell;
  FieldGridCell *neighborCell;

  cellThreshold = cell->terrainHeight - g_TerrainScanReferenceHeight;
  if (scanStep < g_TerrainScanStepLimit) {
    projectedHeightThresholdQ20 = (int)(projectedHeightThresholdQ20 + cellThreshold) >> 1;
    while ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      cellHeight = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        cellHeight = cellHeight + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[scanStep];
      projectedHeight = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)cellThreshold <= (int)projectedHeight) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      adjacentCell = cell + 1;
      legStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      TerrainProjectedOcclusion_ScanDirection0
                (occupancyMaskBits,projectedHeightThresholdQ20,legStep,adjacentCell);
      if (g_TerrainScanStepLimit <= legStep) {
        return;
      }
      /* the direction-1 neighbour C+1-W */
      neighborCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(adjacentCell,-rowStrideBytes);
      if ((neighborCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      neighborHeight = neighborCell->terrainHeight;
      if (0 < neighborCell->waterSurfaceDelta) {
        neighborHeight = neighborHeight + neighborCell->waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(neighborHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[legStep];
      projectedHeight = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeight) {
        neighborCell->occupancyMask = neighborCell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeight;
      }
      cell = neighborCell + 1;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      TerrainProjectedOcclusion_ScanDirection1
                (occupancyMaskBits,projectedHeightThresholdQ20,scanStep,
                 FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-g_TerrainScanRowStrideBytes));
      cellThreshold = projectedHeightThresholdQ20;
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}

/* Line-of-sight marking for the sector between directions 1 (C+1-W) and 2 (C-W), built like
   TerrainProjectedOcclusion_TraceWedge0: spine step C+1-2W (scan step +7), the direction-2 neighbour between two
   spine cells, a straight leg along each bounding direction.
*/
void TerrainProjectedOcclusion_TraceWedge1(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int64_t scaledHeightProduct;
  int cellHeight;
  int rowStrideBytes;
  int neighborHeight;
  uint32_t projectedHeight;
  TerrainDirectionalScanStep legStep;
  TerrainProjectedHeightThresholdQ20 cellThreshold;
  FieldGridCell *adjacentCell;
  FieldGridCell *neighborCell;

  cellThreshold = cell->terrainHeight - g_TerrainScanReferenceHeight;
  if (scanStep < g_TerrainScanStepLimit) {
    projectedHeightThresholdQ20 = (int)(projectedHeightThresholdQ20 + cellThreshold) >> 1;
    while ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      cellHeight = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        cellHeight = cellHeight + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[scanStep];
      projectedHeight = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)cellThreshold <= (int)projectedHeight) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      legStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      TerrainProjectedOcclusion_ScanDirection1
                (occupancyMaskBits,projectedHeightThresholdQ20,legStep,
                 FIELD_GRID_CELL_AT_BYTE_OFFSET(cell + 1,-g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= legStep) {
        return;
      }
      /* the direction-2 neighbour C-W */
      neighborCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes);
      if ((neighborCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      neighborHeight = neighborCell->terrainHeight;
      if (0 < neighborCell->waterSurfaceDelta) {
        neighborHeight = neighborHeight + neighborCell->waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(neighborHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[legStep];
      projectedHeight = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeight) {
        neighborCell->occupancyMask = neighborCell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeight;
      }
      adjacentCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-g_TerrainScanRowStrideBytes - rowStrideBytes);
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      cell = adjacentCell + 1;
      TerrainProjectedOcclusion_ScanDirection2
                (occupancyMaskBits,projectedHeightThresholdQ20,scanStep,adjacentCell);
      cellThreshold = projectedHeightThresholdQ20;
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}

/* Line-of-sight marking for the sector between directions 2 (C-W) and 3 (C-1), built like
   TerrainProjectedOcclusion_TraceWedge0: spine step C-1-W (scan step +7), the direction-3 neighbour between two
   spine cells, a straight leg along each bounding direction.
*/
void TerrainProjectedOcclusion_TraceWedge2(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *adjacentCell;
  int64_t scaledHeightProduct;
  int cellHeight;
  int neighborHeight;
  uint32_t projectedHeight;
  TerrainDirectionalScanStep legStep;
  TerrainProjectedHeightThresholdQ20 cellThreshold;

  cellThreshold = cell->terrainHeight - g_TerrainScanReferenceHeight;
  if (scanStep < g_TerrainScanStepLimit) {
    projectedHeightThresholdQ20 = (int)(projectedHeightThresholdQ20 + cellThreshold) >> 1;
    while ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      cellHeight = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        cellHeight = cellHeight + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[scanStep];
      projectedHeight = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)cellThreshold <= (int)projectedHeight) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeight;
      }
      legStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      TerrainProjectedOcclusion_ScanDirection2
                (occupancyMaskBits,projectedHeightThresholdQ20,legStep,
                 FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= legStep) {
        return;
      }
      /* the direction-3 neighbour C-1 */
      if ((cell[-1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      neighborHeight = cell[-1].terrainHeight;
      if (0 < cell[-1].waterSurfaceDelta) {
        neighborHeight = neighborHeight + cell[-1].waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(neighborHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[legStep];
      projectedHeight = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeight) {
        cell[-1].occupancyMask = cell[-1].occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeight;
      }
      adjacentCell = cell - 2;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell - 1,-g_TerrainScanRowStrideBytes);
      TerrainProjectedOcclusion_ScanDirection3
                (occupancyMaskBits,projectedHeightThresholdQ20,scanStep,adjacentCell);
      cellThreshold = projectedHeightThresholdQ20;
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}

/* Line-of-sight marking for the sector between directions 3 (C-1) and 4 (C-1+W), built like
   TerrainProjectedOcclusion_TraceWedge0: spine step C-2+W (scan step +7), the direction-4 neighbour between two
   spine cells, a straight leg along each bounding direction.
*/
void TerrainProjectedOcclusion_TraceWedge3(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int64_t scaledHeightProduct;
  int rowStrideBytes;
  FieldCellPersistedAux cellHeight;
  int neighborHeight;
  uint32_t projectedHeight;
  TerrainDirectionalScanStep legStep;
  TerrainProjectedHeightThresholdQ20 cellThreshold;
  FieldGridCell *adjacentCell;
  FieldGridCell *neighborCell;

  cellThreshold = cell->terrainHeight - g_TerrainScanReferenceHeight;
  if (scanStep < g_TerrainScanStepLimit) {
    projectedHeightThresholdQ20 = (int)(projectedHeightThresholdQ20 + cellThreshold) >> 1;
    while ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      cellHeight = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        cellHeight = cellHeight + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(int)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[scanStep];
      projectedHeight = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)cellThreshold <= (int)projectedHeight) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      adjacentCell = cell - 1;
      legStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      TerrainProjectedOcclusion_ScanDirection3
                (occupancyMaskBits,projectedHeightThresholdQ20,legStep,adjacentCell);
      if (g_TerrainScanStepLimit <= legStep) {
        return;
      }
      /* the direction-4 neighbour C-1+W, one row stride on */
      neighborCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(adjacentCell,rowStrideBytes);
      if ((neighborCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      neighborHeight = neighborCell->terrainHeight;
      if (0 < neighborCell->waterSurfaceDelta) {
        neighborHeight = neighborHeight + neighborCell->waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(neighborHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[legStep];
      projectedHeight = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeight) {
        neighborCell->occupancyMask = neighborCell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeight;
      }
      /* C-2+W */
      cell = neighborCell - 1;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      TerrainProjectedOcclusion_ScanDirection4
                (occupancyMaskBits,projectedHeightThresholdQ20,scanStep,
                 FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,g_TerrainScanRowStrideBytes));
      cellThreshold = projectedHeightThresholdQ20;
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}

/* Line-of-sight marking for the sector between directions 4 (C-1+W) and 5 (C+W), built like
   TerrainProjectedOcclusion_TraceWedge0: spine step C-1+2W (scan step +7), the direction-5 neighbour between two
   spine cells, a straight leg along each bounding direction.
*/
void TerrainProjectedOcclusion_TraceWedge4(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int64_t scaledHeightProduct;
  int cellHeight;
  int rowStrideBytes;
  int neighborHeight;
  uint32_t projectedHeight;
  TerrainDirectionalScanStep legStep;
  TerrainProjectedHeightThresholdQ20 cellThreshold;
  FieldGridCell *neighborCell;
  FieldGridCell *twoRowsDownCell;

  cellThreshold = cell->terrainHeight - g_TerrainScanReferenceHeight;
  if (scanStep < g_TerrainScanStepLimit) {
    projectedHeightThresholdQ20 = (int)(projectedHeightThresholdQ20 + cellThreshold) >> 1;
    while ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      cellHeight = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        cellHeight = cellHeight + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[scanStep];
      projectedHeight = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)cellThreshold <= (int)projectedHeight) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      legStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      TerrainProjectedOcclusion_ScanDirection4
                (occupancyMaskBits,projectedHeightThresholdQ20,legStep,
                 FIELD_GRID_CELL_AT_BYTE_OFFSET(cell - 1,g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= legStep) {
        return;
      }
      /* the direction-5 neighbour C+W, one row stride on */
      neighborCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes);
      if ((neighborCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      neighborHeight = neighborCell->terrainHeight;
      if (0 < neighborCell->waterSurfaceDelta) {
        neighborHeight = neighborHeight + neighborCell->waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(neighborHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[legStep];
      projectedHeight = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeight) {
        neighborCell->occupancyMask = neighborCell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeight;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      /* two rows down: C+2W, the spine continues at C-1+2W */
      twoRowsDownCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(neighborCell,g_TerrainScanRowStrideBytes);
      cell = twoRowsDownCell - 1;
      TerrainProjectedOcclusion_ScanDirection5
                (occupancyMaskBits,projectedHeightThresholdQ20,scanStep,twoRowsDownCell);
      cellThreshold = projectedHeightThresholdQ20;
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}

/* Line-of-sight marking for the sector between directions 5 (C+W) and 0 (C+1), built like
   TerrainProjectedOcclusion_TraceWedge0: spine step C+1+W (scan step +7), the direction-0 neighbour between two
   spine cells, a straight leg along each bounding direction.
*/
void TerrainProjectedOcclusion_TraceWedge5(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *adjacentCell;
  int64_t scaledHeightProduct;
  int cellHeight;
  int neighborHeight;
  uint32_t projectedHeight;
  TerrainDirectionalScanStep legStep;
  TerrainProjectedHeightThresholdQ20 cellThreshold;

  cellThreshold = cell->terrainHeight - g_TerrainScanReferenceHeight;
  if (scanStep < g_TerrainScanStepLimit) {
    projectedHeightThresholdQ20 = (int)(projectedHeightThresholdQ20 + cellThreshold) >> 1;
    while ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      cellHeight = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        cellHeight = cellHeight + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[scanStep];
      projectedHeight = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)cellThreshold <= (int)projectedHeight) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeight;
      }
      legStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      TerrainProjectedOcclusion_ScanDirection5
                (occupancyMaskBits,projectedHeightThresholdQ20,legStep,
                 FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= legStep) {
        return;
      }
      /* the direction-0 neighbour C+1 */
      if ((cell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      neighborHeight = cell[1].terrainHeight;
      if (0 < cell[1].waterSurfaceDelta) {
        neighborHeight = neighborHeight + cell[1].waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(neighborHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[legStep];
      projectedHeight = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeight) {
        cell[1].occupancyMask = cell[1].occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeight;
      }
      adjacentCell = cell + 2;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell + 1,g_TerrainScanRowStrideBytes);
      TerrainProjectedOcclusion_ScanDirection0
                (occupancyMaskBits,projectedHeightThresholdQ20,scanStep,adjacentCell);
      cellThreshold = projectedHeightThresholdQ20;
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}

/* Line-of-sight leg along direction 0 (C+1, right): each cell's surface height (terrain plus positive water)
   above the eye (g_TerrainScanReferenceHeight) is scaled by the per-step table g_TerrainHeightDeltaScaleByStepQ12;
   a cell whose value reaches the highest value seen so far on this line is visible and gets occupancyMaskBits,
   and its value becomes the new horizon. 4 scan steps per cell, until the step limit or a map-edge cell.
*/
void TerrainProjectedOcclusion_ScanDirection0(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int64_t scaledHeightProduct;
  int cellHeight;
  uint32_t projectedHeightQ20;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      cellHeight = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        cellHeight = cellHeight + cell->waterSurfaceDelta;
      }
      /* the table holds one int per scan step; the product keeps bits 12..43 */
      scaledHeightProduct = (int64_t)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[scanStep];
      projectedHeightQ20 = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeightQ20) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightQ20;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      cell++;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}

/* Line-of-sight leg along direction 1 (C+1-W, up and right); works like TerrainProjectedOcclusion_ScanDirection0.
*/
void TerrainProjectedOcclusion_ScanDirection1(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int64_t scaledHeightProduct;
  int cellHeight;
  uint32_t projectedHeightQ20;

  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      cellHeight = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        cellHeight = cellHeight + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[scanStep];
      projectedHeightQ20 = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeightQ20) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightQ20;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell + 1,-g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}

/* Line-of-sight leg along direction 2 (C-W, up); works like TerrainProjectedOcclusion_ScanDirection0.
*/
void TerrainProjectedOcclusion_ScanDirection2(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int64_t scaledHeightProduct;
  int cellHeight;
  uint32_t projectedHeightQ20;

  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      cellHeight = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        cellHeight = cellHeight + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[scanStep];
      projectedHeightQ20 = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeightQ20) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightQ20;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}

/* Line-of-sight leg along direction 3 (C-1, left); works like TerrainProjectedOcclusion_ScanDirection0.
*/
void TerrainProjectedOcclusion_ScanDirection3(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int64_t scaledHeightProduct;
  int cellHeight;
  uint32_t projectedHeightQ20;

  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      cellHeight = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        cellHeight = cellHeight + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[scanStep];
      projectedHeightQ20 = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeightQ20) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightQ20;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      cell--;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}

/* Line-of-sight leg along direction 4 (C-1+W, down and left); works like TerrainProjectedOcclusion_ScanDirection0.
*/
void TerrainProjectedOcclusion_ScanDirection4(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int64_t scaledHeightProduct;
  int cellHeight;
  uint32_t projectedHeightQ20;

  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      cellHeight = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        cellHeight = cellHeight + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[scanStep];
      projectedHeightQ20 = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeightQ20) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightQ20;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell - 1,g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}

/* Line-of-sight leg along direction 5 (C+W, down); works like TerrainProjectedOcclusion_ScanDirection0.
*/
void TerrainProjectedOcclusion_ScanDirection5(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int64_t scaledHeightProduct;
  int cellHeight;
  uint32_t projectedHeightQ20;

  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      cellHeight = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        cellHeight = cellHeight + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[scanStep];
      projectedHeightQ20 = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeightQ20) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightQ20;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}
