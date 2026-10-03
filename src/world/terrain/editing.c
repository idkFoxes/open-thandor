/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/editing.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/terrain/editing.h>
#include <thandor/thandor.h>

/* Implementation ownership: world/terrain/editing. */

/* Scanline flood fill over the field grid: records (TerrainRegionCollection_RecordConnectedCell, which also marks
   them visited) the horizontal run of cells around cell that carry one of requiredCellFlags, then recurses into
   matching cells of the rows above and below that run. Edge-ring and already visited cells stop the fill.
*/
void TerrainRegionCollection_CollectConnectedCellsRecursive
          (FieldGridRegionMask requiredCellFlags,FieldGridRowStrideBytes rowStrideBytes,
          FieldGridCell *cell)

{
  FieldGridCell *spanStartCell;
  FieldGridCell *spanStopCell;
  FieldGridCell *aboveRowCell;
  FieldGridCell *belowRowCell;

  /* the start cell and the matching cells to its left */
  spanStartCell = cell;
  TerrainRegionCollection_RecordConnectedCell(requiredCellFlags,spanStartCell);
  while (((spanStartCell[-1].flagsAndMaterial & requiredCellFlags) != 0) &&
         ((spanStartCell[-1].flagsAndMaterial & TERRAIN_REGION_STOP_FLAGS) == 0)) {
    spanStartCell = spanStartCell - 1;
    TerrainRegionCollection_RecordConnectedCell(requiredCellFlags,spanStartCell);
  }
  /* the matching cells to its right; spanStopCell ends as the first cell right of the span */
  spanStopCell = cell + 1;
  while (((spanStopCell->flagsAndMaterial & requiredCellFlags) != 0) &&
         ((spanStopCell->flagsAndMaterial & TERRAIN_REGION_STOP_FLAGS) == 0)) {
    TerrainRegionCollection_RecordConnectedCell(requiredCellFlags,spanStopCell);
    spanStopCell = spanStopCell + 1;
  }
  /* row above: from the span start up to the column of the cell that ended the span */
  for (aboveRowCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(spanStartCell,-rowStrideBytes);
       aboveRowCell <= FIELD_GRID_CELL_AT_BYTE_OFFSET(spanStopCell,-rowStrideBytes);
       aboveRowCell = aboveRowCell + 1) {
    if (((aboveRowCell->flagsAndMaterial & TERRAIN_REGION_STOP_FLAGS) == 0) &&
       ((aboveRowCell->flagsAndMaterial & requiredCellFlags) != 0)) {
      TerrainRegionCollection_CollectConnectedCellsRecursive
                (requiredCellFlags,rowStrideBytes,aboveRowCell);
    }
  }
  /* row below: from one cell left of the span start up to the span's last cell */
  for (belowRowCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(spanStartCell - 1,rowStrideBytes);
       belowRowCell < FIELD_GRID_CELL_AT_BYTE_OFFSET(spanStopCell,rowStrideBytes);
       belowRowCell = belowRowCell + 1) {
    if (((belowRowCell->flagsAndMaterial & TERRAIN_REGION_STOP_FLAGS) == 0) &&
       ((belowRowCell->flagsAndMaterial & requiredCellFlags) != 0)) {
      TerrainRegionCollection_CollectConnectedCellsRecursive
                (requiredCellFlags,rowStrideBytes,belowRowCell);
    }
  }
}


/* Editor command INGAME_COMMAND_EDITOR_REPLACE_MATCHING (0x28E0; InGameUiCommand_* material mode, fill tool):
   gives the connected region of cells that share the material of the clicked cell the replacement material.
   The player's material edit plane is cleared first and receives old - new per changed cell, so
   TerrainEditBuffer_CommitFlagsAndMaterialDeltas can undo the fill.
*/
void TerrainMaterialEdit_SeedMatchingRegionReplacement
          (FrontendPlayerIndex playerIndex,TerrainMaterialByteValue replacementMaterialByte,
          Q12 worldYQ12,Q12 worldXQ12)

{
  SelectionPlayerRuntimeBlock *playerBlock;
  FieldGridAsset *fieldGridAsset;
  TerrainMaterialIndex referenceMaterial;
  int remainingCount;
  int gridX;
  int gridY;
  uint32_t *editPlaneCursor;

  playerBlock = g_SelectionPlayerRuntimeBlockPointers[playerIndex];
  fieldGridAsset = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  editPlaneCursor = playerBlock->terrainMaterialEditPlane;
  for (remainingCount = fieldGridAsset->gridWidth * fieldGridAsset->gridHeight; remainingCount != 0;
       remainingCount--) {
    *editPlaneCursor = 0;
    editPlaneCursor = editPlaneCursor + 1;
  }
  /* the caller passes grid coordinates in Q12 */
  gridX = worldXQ12 >> 12;
  gridY = worldYQ12 >> 12;
  if ((gridX < 0) || (gridY < 0) || (gridX >= (int)fieldGridAsset->gridWidth) ||
      (gridY >= (int)fieldGridAsset->gridHeight)) {
    return;
  }
  referenceMaterial = fieldGridAsset->cells[gridY * fieldGridAsset->gridWidth + gridX].flagsAndMaterial &
          FIELD_CELL_MATERIAL_ID_MASK;
  if (referenceMaterial != replacementMaterialByte) {
    fieldGridAsset->runtimeStateFlags = fieldGridAsset->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
    g_TerrainMaterialEditReplacementMaterialByte = replacementMaterialByte;
    g_TerrainMaterialEditFieldGrid = fieldGridAsset;
    g_TerrainMaterialEditDeltaBuffer = playerBlock->terrainMaterialEditPlane;
    g_TerrainMaterialEditReferenceMaterialByte = referenceMaterial;
    TerrainMaterialEdit_PropagateMatchingRegionReplacement(gridY,gridX);
  }
}


/* Editor command INGAME_COMMAND_EDITOR_REPLACE_NON_TARGET (0x29B0; material mode, second fill tool): gives
   the connected region of cells that do not have referenceMaterialByte that material, i.e. fills up to the
   borders made of it. Like the matching fill it clears the player's material edit plane and records the
   undo deltas there.
*/
void TerrainMaterialEdit_SeedNonTargetRegionReplacement
          (FrontendPlayerIndex playerIndex,TerrainMaterialByteValue referenceMaterialByte,
          Q12 worldYQ12,Q12 worldXQ12)

{
  SelectionPlayerRuntimeBlock *playerBlock;
  FieldGridAsset *fieldGridAsset;
  int remainingCount;
  int gridX;
  int gridY;
  uint32_t *editPlaneCursor;

  playerBlock = g_SelectionPlayerRuntimeBlockPointers[playerIndex];
  fieldGridAsset = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  editPlaneCursor = playerBlock->terrainMaterialEditPlane;
  for (remainingCount = fieldGridAsset->gridWidth * fieldGridAsset->gridHeight; remainingCount != 0;
       remainingCount--) {
    *editPlaneCursor = 0;
    editPlaneCursor = editPlaneCursor + 1;
  }
  /* the caller passes grid coordinates in Q12 */
  gridX = worldXQ12 >> 12;
  gridY = worldYQ12 >> 12;
  if ((gridX < 0) || (gridY < 0) || (gridX >= (int)fieldGridAsset->gridWidth) ||
      (gridY >= (int)fieldGridAsset->gridHeight)) {
    return;
  }
  if ((fieldGridAsset->cells[gridY * fieldGridAsset->gridWidth + gridX].flagsAndMaterial &
      FIELD_CELL_MATERIAL_ID_MASK) != referenceMaterialByte) {
    fieldGridAsset->runtimeStateFlags = fieldGridAsset->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
    g_TerrainMaterialEditReferenceMaterialByte = referenceMaterialByte;
    g_TerrainMaterialEditFieldGrid = fieldGridAsset;
    g_TerrainMaterialEditDeltaBuffer = playerBlock->terrainMaterialEditPlane;
    TerrainMaterialEdit_PropagateNonTargetRegionReplacement(gridY,gridX);
  }
}


/* Editor command INGAME_COMMAND_EDITOR_COMMIT_HEIGHTS (0x25A0; U key in height mode): toggles the last height
   edit. Each cell with a delta in the player's height plane is lowered by it (the water surface keeps its
   level), the delta is negated so the next call redoes the edit, and the normals and lighting of the cell and
   its lattice neighbours are recomputed.
*/
void TerrainEditBuffer_CommitHeightDeltasAndRefreshLighting
          (uint32_t playerRuntimeId,uint32_t unusedCommandValue1,uint32_t unusedCommandValue2,
          uint32_t unusedCommandValue3)

{
  FieldGridAsset *fieldGridAsset;
  FieldGridDimension widthCells;
  int heightDelta;
  int remainingCount;
  int rowStrideBytes;
  FieldGridCell *cell;
  FieldGridCell *fieldCell;
  int *heightDeltaCursor;

  fieldGridAsset = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  heightDeltaCursor = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->terrainHeightScratchPlane;
  widthCells = fieldGridAsset->gridWidth;
  remainingCount = widthCells * fieldGridAsset->gridHeight;
  fieldGridAsset->runtimeStateFlags = fieldGridAsset->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
  rowStrideBytes = widthCells * sizeof(FieldGridCell); /* 0x80-byte FieldGridCell records */
  fieldCell = fieldGridAsset->cells;
  do {
    heightDelta = *heightDeltaCursor;
    if (heightDelta != 0) {
      fieldCell->terrainHeight = fieldCell->terrainHeight - heightDelta;
      fieldCell->waterSurfaceDelta = fieldCell->waterSurfaceDelta + heightDelta;
      *heightDeltaCursor = -*heightDeltaCursor;
      if ((fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,fieldCell);
        FieldGridCell_ComputeDirectionalLightColor(fieldCell);
        /* left and right neighbours; one with its own delta is refreshed on its own turn */
        if (((fieldCell[-1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) && (heightDeltaCursor[-1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,fieldCell - 1);
          FieldGridCell_ComputeDirectionalLightColor(fieldCell - 1);
        }
        if (((fieldCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) && (heightDeltaCursor[1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,fieldCell + 1);
          FieldGridCell_ComputeDirectionalLightColor(fieldCell + 1);
        }
        /* the two neighbours in the previous row (same column and the one to the right) */
        fieldCell = fieldCell + -widthCells;
        if ((fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,fieldCell);
          FieldGridCell_ComputeDirectionalLightColor(fieldCell);
        }
        if ((fieldCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,fieldCell + 1);
          FieldGridCell_ComputeDirectionalLightColor(fieldCell + 1);
        }
        /* the two neighbours in the next row (the one to the left and same column) */
        fieldCell = fieldCell + widthCells * 2 + -1;
        if ((fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,fieldCell);
          FieldGridCell_ComputeDirectionalLightColor(fieldCell);
        }
        cell = fieldCell + 1;
        if ((fieldCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell);
          FieldGridCell_ComputeDirectionalLightColor(cell);
        }
        fieldCell = cell + -widthCells;
      }
    }
    fieldCell = fieldCell + 1;
    heightDeltaCursor = heightDeltaCursor + 1;
    remainingCount--;
  } while (remainingCount != 0);
  return;
}


/* Editor command INGAME_COMMAND_EDITOR_COPY_MATERIALS (0x2700; start of a material brush stroke): saves the
   material byte of every cell in the player's material edit plane, so TerrainEditBuffer_SubtractCurrentCellMaterialBytes
   can turn it into undo deltas when the stroke ends.
*/
void TerrainEditBuffer_CopyCellMaterialBytes
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedZero0,uint32_t reservedZero1,
          uint32_t reservedZero2)

{
  FieldGridAsset *fieldGridAsset;
  int remainingCount;
  FieldGridCell *fieldCell;
  TerrainMaterialIndex *materialCursor;

  fieldGridAsset = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  materialCursor = (TerrainMaterialIndex *)
           g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->terrainMaterialEditPlane;
  remainingCount = fieldGridAsset->gridWidth * fieldGridAsset->gridHeight;
  fieldCell = fieldGridAsset->cells;
  do {
    *materialCursor = fieldCell->flagsAndMaterial & FIELD_CELL_MATERIAL_ID_MASK;
    fieldCell = fieldCell + 1;
    materialCursor = materialCursor + 1;
    remainingCount--;
  } while (remainingCount != 0);
  return;
}


/* Editor command INGAME_COMMAND_EDITOR_SUBTRACT_MATERIALS (0x2800; end of a material brush stroke): subtracts
   each cell's current material from the byte saved at the start of the stroke, leaving old - new per cell in
   the player's material edit plane for TerrainEditBuffer_CommitFlagsAndMaterialDeltas (undo).
*/
void TerrainEditBuffer_SubtractCurrentCellMaterialBytes
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedZero0,uint32_t reservedZero1,
          uint32_t reservedZero2)

{
  FieldGridAsset *fieldGridAsset;
  int remainingCount;
  FieldGridCell *fieldCell;
  uint32_t *materialDeltaCursor;

  fieldGridAsset = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  materialDeltaCursor = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->terrainMaterialEditPlane;
  remainingCount = fieldGridAsset->gridWidth * fieldGridAsset->gridHeight;
  fieldCell = fieldGridAsset->cells;
  do {
    *materialDeltaCursor = *materialDeltaCursor - (fieldCell->flagsAndMaterial & FIELD_CELL_MATERIAL_ID_MASK);
    fieldCell = fieldCell + 1;
    materialDeltaCursor = materialDeltaCursor + 1;
    remainingCount--;
  } while (remainingCount != 0);
  return;
}


/* Editor command INGAME_COMMAND_EDITOR_COMMIT_MATERIALS (0x2870; U key in material mode): toggles the last
   material edit by adding the player's material deltas to the cells and negating them, so the next call
   redoes the edit.
*/
void TerrainEditBuffer_CommitFlagsAndMaterialDeltas
          (uint32_t playerRuntimeId,uint32_t unusedCommandValue1,uint32_t unusedCommandValue2,
          uint32_t unusedCommandValue3)

{
  FieldGridAsset *fieldGridAsset;
  int remainingCount;
  FieldGridCell *fieldCell;
  uint32_t *materialDeltaCursor;

  fieldGridAsset = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  materialDeltaCursor = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->terrainMaterialEditPlane;
  remainingCount = fieldGridAsset->gridWidth * fieldGridAsset->gridHeight;
  /* The original ORs 1 into the dword 0x14C bytes before the field grid asset instead of
     its runtimeStateFlags, so the surface is not marked dirty here; kept as in the original. */
  *(uint32_t *)((uint8_t *)fieldGridAsset - TERRAIN_EDIT_STRAY_DIRTY_FLAG_BACK_OFFSET) =
       *(uint32_t *)((uint8_t *)fieldGridAsset - TERRAIN_EDIT_STRAY_DIRTY_FLAG_BACK_OFFSET) | 1;
  fieldCell = fieldGridAsset->cells;
  do {
    fieldCell->flagsAndMaterial = fieldCell->flagsAndMaterial + *materialDeltaCursor;
    *materialDeltaCursor = -*materialDeltaCursor;
    fieldCell = fieldCell + 1;
    materialDeltaCursor = materialDeltaCursor + 1;
    remainingCount--;
  } while (remainingCount != 0);
  return;
}


/* Editor command INGAME_COMMAND_EDITOR_HEIGHTS_TO_DELTAS (0x2C90; end of a height brush stroke): replaces each
   value of the player's height plane by the cell's terrain height minus that value, leaving the deltas that
   TerrainEditBuffer_CommitHeightDeltasAndRefreshLighting undoes.
*/
void TerrainEditBuffer_ConvertHeightsToDeltas
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedZero0,uint32_t reservedZero1,
          uint32_t reservedZero2)

{
  FieldGridAsset *fieldGridAsset;
  int remainingCount;
  FieldGridCell *fieldCell;
  int *heightCursor;

  fieldGridAsset = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  heightCursor = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->terrainHeightScratchPlane;
  remainingCount = fieldGridAsset->gridWidth * fieldGridAsset->gridHeight;
  fieldCell = fieldGridAsset->cells;
  do {
    *heightCursor = fieldCell->terrainHeight - *heightCursor;
    fieldCell = fieldCell + 1;
    heightCursor = heightCursor + 1;
    remainingCount--;
  } while (remainingCount != 0);
  return;
}


/* Visit step of the mining-region flood fill (TerrainRegionCollection_CollectConnectedCellsRecursive):
   counts and marks the cell as visited and, when its extraction descriptor matches requiredOccupancyMask,
   moves the descriptor and the extracting model's offset out of the cell into the region collection (at
   most TERRAIN_REGION_COLLECTION_CAPACITY entries), so the per-tick mining pays each extractor once.
*/
void TerrainRegionCollection_RecordConnectedCell(FieldGridRegionMask requiredOccupancyMask,FieldGridCell *cell)

{
  ArmyRuntimeSavedOffset savedArmyOffset;
  uint32_t extractionDescriptor;
  TerrainRegionCollectionCount storedCount;
  int entriesBase;

  storedCount = g_TerrainRegionCollectionStoredCount;
  g_TerrainRegionCollectionVisitedCount++;
  extractionDescriptor = cell->resourceExtractionDescriptor;
  cell->flagsAndMaterial = cell->flagsAndMaterial | FIELD_CELL_CONNECTED_REGION_VISITED;
  if ((requiredOccupancyMask & extractionDescriptor) != 0) {
    cell->resourceExtractionDescriptor = 0;
    /* take the model offset and clear it in one atomic step */
    LOCK();
    savedArmyOffset = cell->armyRuntimeSavedOffset;
    cell->armyRuntimeSavedOffset = 0;
    entriesBase = g_TerrainRegionCollectionEntries;
    UNLOCK();
    if (storedCount < TERRAIN_REGION_COLLECTION_CAPACITY) {
      g_TerrainRegionCollectionStoredCount++;
      *(uint32_t *)(g_TerrainRegionCollectionEntries + storedCount * 8) = extractionDescriptor;
      *(ArmyRuntimeSavedOffset *)(entriesBase + 4 + storedCount * 8) = savedArmyOffset;
    }
  }
}


/* Step of the material fills below: gives cell its new material by subtracting materialDelta (old - new
   material) and adds the delta to the cell's slot in the undo delta buffer. */
static void TerrainMaterialEdit_ApplyCellMaterialDelta(FieldGridCell *cell,int *deltaSlot,int materialDelta)
{
  cell->flagsAndMaterial = cell->flagsAndMaterial - materialDelta;
  *deltaSlot = *deltaSlot + materialDelta;
}


/* Scanline flood fill of TerrainMaterialEdit_SeedMatchingRegionReplacement: from cell (gridY, gridX) gives
   the run of cells whose material is g_TerrainMaterialEditReferenceMaterialByte the replacement material
   (adding old - new to g_TerrainMaterialEditDeltaBuffer), then recurses into the previous row over the run's
   columns and into the next row shifted one column left (triangular lattice neighbours).
   The globals hold the field grid and the buffer as plain addresses (cast to FieldGridAsset / FieldGridCell
   where used).
*/
void TerrainMaterialEdit_PropagateMatchingRegionReplacement(FieldGridCellCoordinate gridY,FieldGridCellCoordinate gridX)

{
  FieldGridAsset *fieldGrid;
  uint32_t referenceMaterial;
  int gridWidth;
  int cellIndex;
  uint32_t cellMaterial;
  int column;
  int spanStartColumn;
  int spanStopColumn;
  FieldGridCell *leftCell;
  FieldGridCell *rightCell;
  int *leftDeltaCursor;
  int *rightDeltaCursor;

  referenceMaterial = g_TerrainMaterialEditReferenceMaterialByte;
  fieldGrid = (FieldGridAsset *)g_TerrainMaterialEditFieldGrid;
  if ((gridY < 0) || (gridX < 0)) {
    return;
  }
  gridWidth = (int)fieldGrid->gridWidth;
  if ((gridY >= (int)fieldGrid->gridHeight) || (gridX >= gridWidth)) {
    return;
  }
  cellIndex = gridY * gridWidth + gridX;
  rightDeltaCursor = (int *)(g_TerrainMaterialEditDeltaBuffer + cellIndex * 4);
  rightCell = &fieldGrid->cells[cellIndex];
  cellMaterial = (uint32_t)rightCell->flagsAndMaterial & FIELD_CELL_MATERIAL_ID_MASK;
  if (referenceMaterial != cellMaterial) {
    return;
  }
  /* the start cell and the matching cells to its left */
  spanStartColumn = gridX;
  leftCell = rightCell;
  leftDeltaCursor = rightDeltaCursor;
  TerrainMaterialEdit_ApplyCellMaterialDelta
            (leftCell,leftDeltaCursor,(int)(cellMaterial - g_TerrainMaterialEditReplacementMaterialByte));
  while (spanStartColumn > 0) {
    cellMaterial = (uint32_t)leftCell[-1].flagsAndMaterial & FIELD_CELL_MATERIAL_ID_MASK;
    if (referenceMaterial != cellMaterial) break;
    spanStartColumn = spanStartColumn - 1;
    leftCell = leftCell - 1;
    leftDeltaCursor = leftDeltaCursor - 1;
    TerrainMaterialEdit_ApplyCellMaterialDelta
              (leftCell,leftDeltaCursor,(int)(cellMaterial - g_TerrainMaterialEditReplacementMaterialByte));
  }
  /* the matching cells to its right; spanStopColumn ends one past the run */
  for (spanStopColumn = gridX + 1; spanStopColumn < gridWidth; spanStopColumn = spanStopColumn + 1) {
    cellMaterial = (uint32_t)rightCell[1].flagsAndMaterial & FIELD_CELL_MATERIAL_ID_MASK;
    if (referenceMaterial != cellMaterial) break;
    rightCell = rightCell + 1;
    rightDeltaCursor = rightDeltaCursor + 1;
    TerrainMaterialEdit_ApplyCellMaterialDelta
              (rightCell,rightDeltaCursor,(int)(cellMaterial - g_TerrainMaterialEditReplacementMaterialByte));
  }
  /* previous row: columns spanStartColumn .. spanStopColumn */
  for (column = spanStartColumn; column <= spanStopColumn; column = column + 1) {
    TerrainMaterialEdit_PropagateMatchingRegionReplacement(gridY - 1,column);
  }
  /* next row: columns spanStartColumn - 1 .. spanStopColumn - 1 */
  for (column = spanStartColumn - 1; column < spanStopColumn; column = column + 1) {
    TerrainMaterialEdit_PropagateMatchingRegionReplacement(gridY + 1,column);
  }
}


/* Scanline flood fill of TerrainMaterialEdit_SeedNonTargetRegionReplacement: the same walk as
   TerrainMaterialEdit_PropagateMatchingRegionReplacement, but over the cells whose material differs from
   g_TerrainMaterialEditReferenceMaterialByte, which all receive that material (old - new goes to
   g_TerrainMaterialEditDeltaBuffer).
*/
void TerrainMaterialEdit_PropagateNonTargetRegionReplacement
          (FieldGridCellCoordinate gridY,FieldGridCellCoordinate gridX)

{
  FieldGridAsset *fieldGrid;
  uint32_t referenceMaterial;
  int gridWidth;
  int cellIndex;
  uint32_t cellMaterial;
  int column;
  int spanStartColumn;
  int spanStopColumn;
  FieldGridCell *leftCell;
  FieldGridCell *rightCell;
  int *leftDeltaCursor;
  int *rightDeltaCursor;

  referenceMaterial = g_TerrainMaterialEditReferenceMaterialByte;
  fieldGrid = (FieldGridAsset *)g_TerrainMaterialEditFieldGrid;
  if ((gridY < 0) || (gridX < 0)) {
    return;
  }
  gridWidth = (int)fieldGrid->gridWidth;
  if ((gridY >= (int)fieldGrid->gridHeight) || (gridX >= gridWidth)) {
    return;
  }
  cellIndex = gridY * gridWidth + gridX;
  rightDeltaCursor = (int *)(g_TerrainMaterialEditDeltaBuffer + cellIndex * 4);
  rightCell = &fieldGrid->cells[cellIndex];
  cellMaterial = (uint32_t)rightCell->flagsAndMaterial & FIELD_CELL_MATERIAL_ID_MASK;
  if (referenceMaterial == cellMaterial) {
    return;
  }
  /* the start cell and the non-target cells to its left */
  spanStartColumn = gridX;
  leftCell = rightCell;
  leftDeltaCursor = rightDeltaCursor;
  TerrainMaterialEdit_ApplyCellMaterialDelta(leftCell,leftDeltaCursor,(int)(cellMaterial - referenceMaterial));
  while (spanStartColumn > 0) {
    cellMaterial = (uint32_t)leftCell[-1].flagsAndMaterial & FIELD_CELL_MATERIAL_ID_MASK;
    if (referenceMaterial == cellMaterial) break;
    spanStartColumn = spanStartColumn - 1;
    leftCell = leftCell - 1;
    leftDeltaCursor = leftDeltaCursor - 1;
    TerrainMaterialEdit_ApplyCellMaterialDelta(leftCell,leftDeltaCursor,(int)(cellMaterial - referenceMaterial));
  }
  /* the non-target cells to its right; spanStopColumn ends one past the run */
  for (spanStopColumn = gridX + 1; spanStopColumn < gridWidth; spanStopColumn = spanStopColumn + 1) {
    cellMaterial = (uint32_t)rightCell[1].flagsAndMaterial & FIELD_CELL_MATERIAL_ID_MASK;
    if (referenceMaterial == cellMaterial) break;
    rightCell = rightCell + 1;
    rightDeltaCursor = rightDeltaCursor + 1;
    TerrainMaterialEdit_ApplyCellMaterialDelta(rightCell,rightDeltaCursor,(int)(cellMaterial - referenceMaterial));
  }
  /* previous row: columns spanStartColumn .. spanStopColumn */
  for (column = spanStartColumn; column <= spanStopColumn; column = column + 1) {
    TerrainMaterialEdit_PropagateNonTargetRegionReplacement(gridY - 1,column);
  }
  /* next row: columns spanStartColumn - 1 .. spanStopColumn - 1 */
  for (column = spanStartColumn - 1; column < spanStopColumn; column = column + 1) {
    TerrainMaterialEdit_PropagateNonTargetRegionReplacement(gridY + 1,column);
  }
}

