/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/field_raycast.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* Field-grid ray casts: distance along a ray to the terrain surface, the secondary (water) surface and the
   terrain triangles. */

#include <thandor/world/terrain/field_raycast.h>
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
static Bool8 TerrainTriangle_DivideHitNumerators
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
static Bool8 TerrainTriangle_IntersectRayCorner3Triangle
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
static Bool8 TerrainTriangle_IntersectRayCorner0Triangle
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
Bool8 TerrainTriangle_IntersectRayDistance
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
   column. Returns true when the current cell already contains the ray end or no step is possible (no
   row crossing and no column movement), false with the next cell and corner in g_TerrainRayNext* otherwise.
   On a true return g_TerrainRayNext* hold the original's leftover results too: coord0 = end0 - cur0 for the
   destination-cell exit, cur0 with cell - 0x80 / cur1 - one cell for the no-column-movement exit.
*/
Bool8 TerrainRay_AdvanceGridTraversal
          (Q12 rayEndCoord0Q12,Q12 rayEndCoord1Q12,Q12 rayStartCoord0Q12,Q12 rayStartCoord1Q12,
          FieldGridRowStrideBytes rowStrideBytes,FieldGridCell *currentCell,Q12 currentGridCoord0Q12
          ,Q12 currentGridCoord1Q12)

{
  /* Besides the boolean result the original returns the next cell and the next grid corner (coord0 /
     coord1); they are published in g_TerrainRayNext*. */
  uint8_t *cell = (uint8_t *)currentCell;
  int delta0;
  int delta1;

  g_TerrainRayNextCell = currentCell;
  g_TerrainRayNextCoord0Q12 = currentGridCoord0Q12;
  g_TerrainRayNextCoord1Q12 = currentGridCoord1Q12;
  delta1 = rayEndCoord1Q12 - currentGridCoord1Q12;
  delta0 = rayEndCoord0Q12 - currentGridCoord0Q12;
  /* end and current coordinate are compared as true signed values; the checks against one cell use the
     wrapped differences */
  if (rayEndCoord1Q12 >= currentGridCoord1Q12 && rayEndCoord0Q12 >= currentGridCoord0Q12 &&
      delta1 <= FIELD_GRID_CELL_Q12 && delta0 <= FIELD_GRID_CELL_Q12) {
    /* already in the destination cell: returns true with coord0 = end0 - cur0 (cell and coord1
       unchanged). The raycasts pass that coord0 on as materialOrCellIndex of their miss result. */
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
    /* returns true after cell and coord1 were already moved one column back; coord0 = cur0 */
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
