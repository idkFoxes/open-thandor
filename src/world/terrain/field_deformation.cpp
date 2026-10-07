/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/field_deformation.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* Field-grid deformation in play: the radial crater/mound of an effect and the height set at a world point
   (levelling under a unit), with the neighbour refresh. */

#include <thandor/world/terrain/field_deformation.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

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
  firstCell = fieldGrid->cells + (int32_t)(minRow * rowLength) + minColumn;
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

/* The cell rule of the flatten brush, for the hexagon walk of hex_scan.h (formerly 12 functions
   TerrainHeightDelta_ApplyWedge0..5 and _ApplyDirection0..5): levels the cell to g_TerrainScanReferenceHeight
   and moves the removed height into waterSurfaceDelta, so the water surface stays where it was (Original quirk:
   also for cells without water, whose waterSurfaceDelta is negative). */
static void TerrainHeightDelta_LevelCell(FieldGridCell *cell)

{
  int heightAdjustmentQ12;

  heightAdjustmentQ12 = g_TerrainScanReferenceHeight - cell->terrainHeight;
  cell->terrainHeight = cell->terrainHeight + heightAdjustmentQ12;
  cell->waterSurfaceDelta = cell->waterSurfaceDelta - heightAdjustmentQ12;
}

/* Terrain shaping at a world point (used by armies whose class shapes the ground under them): sets the
   grid vertex nearest to (worldX, worldY) to the height worldZ (moving its water surface by the opposite
   amount, so the water level stays) and levels the six hexagon sectors around the vertex
   (TerrainHexScan_AllSectors with TerrainHeightDelta_LevelCell) to the same height; heightDeltaSourceValue / 0x240 (clamped to 1..255) limits
   those scans. Nothing happens on a border vertex or a vertex under water. (The original also returns a failure
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
  FieldGridCell *vertexCell;
  FieldGridCoordinates gridCoordinates;
  uint32_t targetRow;
  uint32_t targetColumn;

  if (fieldGrid == nullptr) {
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
  /* Original quirk: the dirty bit is set with a literal 1 before the bounds check, so also when nothing is
     applied */
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
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
  /* Original quirk: only the vertex cell is tested (edge, water above it); the walked cells are levelled
     whatever their water */
  if (Any(vertexCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) || 0 < vertexCell->waterSurfaceDelta) {
    return;
  }
  heightDeltaQ12 = g_TerrainScanReferenceHeight - vertexCell->terrainHeight;
  vertexCell->terrainHeight = vertexCell->terrainHeight + heightDeltaQ12;
  vertexCell->waterSurfaceDelta = vertexCell->waterSurfaceDelta - heightDeltaQ12;
  /* the six sectors around the vertex cell (not visited again), with the row stride
     g_TerrainScanRowStrideBytes */
  TerrainHexScan_AllSectors(vertexCell,TerrainHexScan_MarkPolicy([](FieldGridCell *cell) {
                              TerrainHeightDelta_LevelCell(cell);
                            }));
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
         (cell->flagsAndMaterial & ~FIELD_CELL_MATERIAL_ID_MASK) |
         FieldCell_FromRawWord(static_cast<uint32_t>(terrainMaterialIndexOrNegativeSentinel));
  }
}
