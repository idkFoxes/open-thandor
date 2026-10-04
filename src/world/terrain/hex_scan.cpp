/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/hex_scan.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* Shared state and set-up of the hexagonal radius scans around a world point (sight, overlay marking): the
   step limit from the radius, the nearest grid vertex and the scan globals. */

#include <thandor/world/terrain/hex_scan.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

uint32_t g_TerrainScanRowStrideBytes = 0;

uint32_t g_TerrainScanStepLimit = 0;

TerrainScanSelectorUnion g_TerrainScanSharedSelectorValue = {0};

uint32_t g_TerrainScanReferenceHeight = 0;

/* Shared start of the three world-point scans below: sets g_TerrainScanStepLimit from the radius (one step per
   TERRAIN_SCAN_RADIUS_PER_STEP world units, at least 1, at most TERRAIN_SCAN_STEP_LIMIT_MAX). */
void TerrainProjectedScan_SetStepLimitFromRadius(FieldGridRadiusUnits radiusWorldUnits)

{
  g_TerrainScanStepLimit = (uint32_t)radiusWorldUnits / TERRAIN_SCAN_RADIUS_PER_STEP;
  if (g_TerrainScanStepLimit == 0) {
    g_TerrainScanStepLimit = 1;
  }
  else if (TERRAIN_SCAN_STEP_LIMIT_MAX < g_TerrainScanStepLimit) {
    g_TerrainScanStepLimit = TERRAIN_SCAN_STEP_LIMIT_MAX;
  }
}

/* Shared by the three world-point scans below: picks the vertex of the triangulated grid cell nearest to the Q12
   grid position from its Q12 fractions (0x1000 = one cell). */
void TerrainProjectedScan_SelectNearestGridVertex
          (FieldGridCoordinates gridCoordinates,uint32_t *gridRowOut,uint32_t *gridColumnOut)

{
  uint32_t baseColumn;
  uint32_t columnFraction;
  uint32_t rowFraction;
  uint32_t fractionSum;
  uint32_t gridRow;
  uint32_t gridColumn;

  baseColumn = gridCoordinates.columnQ12 >> 12;
  gridRow = gridCoordinates.rowQ12 >> 12;
  columnFraction = (uint32_t)(gridCoordinates.columnQ12 & Q12_FRACTION_MASK);
  rowFraction = (uint32_t)(gridCoordinates.rowQ12 & Q12_FRACTION_MASK);
  fractionSum = rowFraction + columnFraction * 2;
  gridColumn = baseColumn;
  if (fractionSum < FIELD_GRID_CELL_Q12) {
    if (FIELD_GRID_CELL_Q12 - 1 < columnFraction + rowFraction * 2) {
      gridRow++;
    }
  }
  else if (fractionSum < FIELD_GRID_TWO_CELLS_Q12 + 1) {
    gridColumn = baseColumn + 1;
    if (columnFraction < rowFraction) {
      gridRow++;
      gridColumn = baseColumn;
    }
  }
  else {
    gridColumn = baseColumn + 1;
    if (FIELD_GRID_TWO_CELLS_Q12 - 1 < columnFraction + rowFraction * 2) {
      gridRow++;
    }
  }
  *gridRowOut = gridRow;
  *gridColumnOut = gridColumn;
}
