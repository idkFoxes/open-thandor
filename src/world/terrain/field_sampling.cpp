/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/field_sampling.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* Field-grid sampling: world to grid coordinates, nearest-point queries and the height, water and normal
   interpolation over the triangle lattice. */

#include <thandor/world/terrain/field_sampling.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

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
    return field->cells[(int32_t)(field->gridWidth * gridRowIndex + gridColumnIndex)].waterSurfaceDelta;
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
  cell = fieldGrid->cells + (int32_t)(gridRowIndex * gridWidth) + gridColumnIndex;
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
  cell = field->cells + (int32_t)(gridRowIndex * gridWidth) + gridColumnIndex;
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
  cell = fieldGrid->cells + (int32_t)(gridRowIndex * gridWidth) + gridColumnIndex;
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
  cell = fieldGrid->cells + (int32_t)(gridRowIndex * gridWidth) + gridColumnIndex;
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
  cell = fieldGrid->cells + (int32_t)(row * rowLength) + column;
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
       ((uint8_t *)&fieldGrid->cells[(int32_t)(fieldGrid->gridWidth * gridRowIndex + gridColumnIndex)].occupancyMask)[factionSlot];
  return (occupancyByte & FIELD_CELL_OCCUPANCY_PRESENCE_BITS) == 0;
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

const FieldGridInterpolationCallbackTable5 g_FieldGridInterpolationCallbacks5 = {
    .callbacks = {
        /* 0 */ THANDOR_SLOT(FieldGrid_InterpolateTerrainHeight),
        /* 1 */ THANDOR_SLOT(FieldGrid_InterpolateWaterSurfaceHeight),
        /* 2 */ THANDOR_SLOT(FieldGrid_InterpolateTerrainHeight),
        /* 3 */ THANDOR_SLOT(FieldGrid_InterpolateTerrainHeight),
        /* 4 */ THANDOR_SLOT(FieldGrid_InterpolateTopSurfaceHeight)
    }
};
