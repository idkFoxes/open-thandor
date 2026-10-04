/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/field_raycast.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* Field-grid ray casts: distance along a ray to the terrain surface, the secondary (water) surface and the
   terrain triangles. */

#include <thandor/world/terrain/field_raycast.h>
#include <thandor/world/terrain/visuals.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

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
  currentCell = (FieldGridCell *)((uint8_t *)&fieldGrid->cells[(int)rayStartColumnQ12 >> Q12_SHIFT] + (int32_t)(((int)rayStartRowQ12 >> Q12_SHIFT) * rowLength * sizeof(FieldGridCell)));
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
  currentCell = (FieldGridCell *)((uint8_t *)&fieldGrid->cells[(int)rayStartColumnQ12 >> Q12_SHIFT] + (int32_t)(((int)rayStartRowQ12 >> Q12_SHIFT) * rowLength * sizeof(FieldGridCell)));
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
