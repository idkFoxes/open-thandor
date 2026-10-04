/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/placement_tests.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/terrain/placement_tests.h>
#include <thandor/thandor.h>
#include <thandor/world/terrain/hex_scan.h>

/* Module data. */

static const int32_t g_TerrainHeightBandMaximumDelta = 1024;

static const int32_t g_TerrainHeightBandMinimumDelta = -1024;

/* int32_t minimum (triangle1NormalAngles >> 16) for the auxiliary height/placement scans (0x3000) */
static const int32_t g_TerrainAuxHeightMinimum = 12288;

/* Implementation ownership: world/terrain/placement_tests. */

/* Cell tests of the hexagon walks (the map-edge test is done by TerrainHexScan_EndsAt); true = the cell fails. */

/* Height band: a flooded cell (waterSurfaceDelta > 0) or a height outside
   [g_TerrainHeightBandMinimumDelta, g_TerrainHeightBandMaximumDelta] relative to g_TerrainScanReferenceHeight. */
static Bool8 TerrainHeightBand_CellFails(const FieldGridCell *cell)

{
  int relativeHeightQ12;

  relativeHeightQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight;
  if (0 < cell->waterSurfaceDelta) {
    return true;
  }
  return ((int)g_TerrainHeightBandMaximumDelta < relativeHeightQ12) ||
         (relativeHeightQ12 < (int)g_TerrainHeightBandMinimumDelta);
}


/* Water-surface contact: a negative waterSurfaceDelta (Original quirk: the opposite sign of the height-band test)
   or a triangle1NormalAngles high word below g_TerrainAuxHeightMinimum. */
static Bool8 TerrainAuxHeightThreshold_CellFails(const FieldGridCell *cell)

{
  return (cell->waterSurfaceDelta < 0) ||
         ((int)cell->triangle1NormalAngles >> 16 < (int)g_TerrainAuxHeightMinimum);
}


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
  /* Original quirk: (X, Y) is passed into FieldGrid_WorldToGridQ12(worldY, worldX). */
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

  if (fieldGrid == nullptr) {
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
  /* the six sector walks, starting at C+1, C+1-W, C-W, C-1, C-1+W, C+W (W = grid width); a map-edge cell
     fails, and the first failing cell ends the scan (Original quirk: later sectors are not evaluated) */
  return TerrainHexScan_AllSectors(centerCell,TerrainHexScan_TestPolicy([](FieldGridCell *fieldCell) {
                                     return TerrainHeightBand_CellFails(fieldCell);
                                   }));
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

  if (fieldGrid == nullptr) {
    return true;
  }
  if (!TerrainScan_BeginAroundWorldPoint(radiusWorldUnits,referenceHeightQ12,worldXQ12,worldYQ12,fieldGrid,
                                         &centerCellIndex)) {
    return true;
  }
  /* Original quirk: the centre cell's threshold test reads triangle0NormalAngles, the sector walks
     (TerrainAuxHeightThreshold_CellFails) read triangle1NormalAngles. */
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
  /* the six sector walks, starting at C+1, C+1-W, C-W, C-1, C-1+W, C+W (W = grid width); a map-edge cell
     fails, and the first failing cell ends the scan (Original quirk: later sectors are not evaluated) */
  return TerrainHexScan_AllSectors(centerCell,TerrainHexScan_TestPolicy([](FieldGridCell *fieldCell) {
                                     return TerrainAuxHeightThreshold_CellFails(fieldCell);
                                   }));
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
