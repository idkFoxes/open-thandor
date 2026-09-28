/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/editing.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/terrain/editing.h>
#include <thandor/thandor.h>

/* Implementation ownership: world/terrain/editing. */

/* Address: 0x005137F0.
   Scanline flood fill over the field grid: records (TerrainRegionCollection_RecordConnectedCell, which also marks
   them visited) the horizontal run of cells around cell that carry one of requiredCellFlags, then recurses into
   matching cells of the rows above and below that run. Edge-ring and already visited cells stop the fill.
*/
void TerrainRegionCollection_CollectConnectedCellsRecursive
          (FieldGridRegionMask requiredCellFlags,FieldGridRowStrideBytes rowStrideBytes,
          FieldGridCell *cell)

{
  FieldGridCell *rowCellCursor;
  FieldGridCell *spanStartCell;
  FieldGridCell *aboveRowCell;
  FieldGridCell *cursorOrSpanEndCell;

  cursorOrSpanEndCell = cell;
  do {
    spanStartCell = cursorOrSpanEndCell;
    TerrainRegionCollection_RecordConnectedCell(requiredCellFlags,spanStartCell);
    if ((spanStartCell[-1].flagsAndMaterial & requiredCellFlags) == 0) break;
    cursorOrSpanEndCell = spanStartCell - 1;
  } while ((spanStartCell[-1].flagsAndMaterial & TERRAIN_REGION_STOP_FLAGS) == 0);
  while ((cursorOrSpanEndCell = cell + 1, (cell[1].flagsAndMaterial & requiredCellFlags) != 0 &&
         ((cell[1].flagsAndMaterial & TERRAIN_REGION_STOP_FLAGS) == 0))) {
    TerrainRegionCollection_RecordConnectedCell(requiredCellFlags,cursorOrSpanEndCell);
    cell = cursorOrSpanEndCell;
  }
  /* row above: from the span start up to the column of the cell that ended the span */
  aboveRowCell = (FieldGridCell *)((int)spanStartCell - rowStrideBytes);
  do {
    if (((aboveRowCell->flagsAndMaterial & TERRAIN_REGION_STOP_FLAGS) == 0) &&
       ((aboveRowCell->flagsAndMaterial & requiredCellFlags) != 0)) {
      TerrainRegionCollection_CollectConnectedCellsRecursive
                (requiredCellFlags,rowStrideBytes,aboveRowCell);
    }
    aboveRowCell = aboveRowCell + 1;
  } while (aboveRowCell <= (FieldGridCell *)((int)cursorOrSpanEndCell - rowStrideBytes));
  /* row below: from one cell left of the span start up to the span's last cell
     (runtime0C_3F - 0xC is the cell's own address) */
  rowCellCursor = (FieldGridCell *)(spanStartCell[-1].runtime0C_3F + rowStrideBytes - 0xc);
  do {
    if (((rowCellCursor->flagsAndMaterial & TERRAIN_REGION_STOP_FLAGS) == 0) &&
       ((rowCellCursor->flagsAndMaterial & requiredCellFlags) != 0)) {
      TerrainRegionCollection_CollectConnectedCellsRecursive
                (requiredCellFlags,rowStrideBytes,rowCellCursor);
    }
    rowCellCursor = rowCellCursor + 1;
  } while (rowCellCursor < (FieldGridCell *)(cursorOrSpanEndCell->runtime0C_3F + rowStrideBytes - 0xc));
  return;
}


/* Address: 0x00561A10.
   Editor command INGAME_COMMAND_EDITOR_REPLACE_MATCHING (0x28E0; InGameUiCommand_* material mode, fill tool):
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
  int countOrGridX;
  int gridY;
  uint32_t *editPlaneCursor;

  playerBlock = g_SelectionPlayerRuntimeBlockPointers[playerIndex];
  fieldGridAsset = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  editPlaneCursor = playerBlock->terrainMaterialEditPlane808C;
  for (countOrGridX = fieldGridAsset->gridWidth * fieldGridAsset->gridHeight; countOrGridX != 0; countOrGridX--) {
    *editPlaneCursor = 0;
    editPlaneCursor = editPlaneCursor + 1;
  }
  /* the caller passes grid coordinates in Q12 */
  countOrGridX = worldXQ12 >> 12;
  if ((((-1 < countOrGridX) && (gridY = worldYQ12 >> 12, -1 < gridY)) && (countOrGridX < (int)fieldGridAsset->gridWidth))
     && (gridY < (int)fieldGridAsset->gridHeight)) {
    referenceMaterial = fieldGridAsset->cells[gridY * fieldGridAsset->gridWidth + countOrGridX].flagsAndMaterial &
            FIELD_CELL_MATERIAL_ID_MASK;
    editPlaneCursor = playerBlock->terrainMaterialEditPlane808C;
    if (referenceMaterial != replacementMaterialByte) {
      fieldGridAsset->runtimeStateFlags = fieldGridAsset->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
      g_TerrainMaterialEditReplacementMaterialByte = replacementMaterialByte;
      g_TerrainMaterialEditFieldGrid = fieldGridAsset;
      g_TerrainMaterialEditDeltaBuffer = editPlaneCursor;
      g_TerrainMaterialEditReferenceMaterialByte = referenceMaterial;
      TerrainMaterialEdit_PropagateMatchingRegionReplacement(gridY,countOrGridX);
    }
  }
  return;
}


/* Address: 0x00561AE0.
   Editor command INGAME_COMMAND_EDITOR_REPLACE_NON_TARGET (0x29B0; material mode, second fill tool): gives
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
  int countOrGridX;
  int gridY;
  uint32_t *editPlaneCursor;

  playerBlock = g_SelectionPlayerRuntimeBlockPointers[playerIndex];
  fieldGridAsset = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  editPlaneCursor = playerBlock->terrainMaterialEditPlane808C;
  for (countOrGridX = fieldGridAsset->gridWidth * fieldGridAsset->gridHeight; countOrGridX != 0; countOrGridX--) {
    *editPlaneCursor = 0;
    editPlaneCursor = editPlaneCursor + 1;
  }
  /* the caller passes grid coordinates in Q12 */
  countOrGridX = worldXQ12 >> 12;
  if ((((-1 < countOrGridX) && (gridY = worldYQ12 >> 12, -1 < gridY)) && (countOrGridX < (int)fieldGridAsset->gridWidth))
     && (gridY < (int)fieldGridAsset->gridHeight)) {
    editPlaneCursor = playerBlock->terrainMaterialEditPlane808C;
    if ((fieldGridAsset->cells[gridY * fieldGridAsset->gridWidth + countOrGridX].flagsAndMaterial &
        FIELD_CELL_MATERIAL_ID_MASK) != referenceMaterialByte) {
      fieldGridAsset->runtimeStateFlags = fieldGridAsset->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
      g_TerrainMaterialEditReferenceMaterialByte = referenceMaterialByte;
      g_TerrainMaterialEditFieldGrid = fieldGridAsset;
      g_TerrainMaterialEditDeltaBuffer = editPlaneCursor;
      TerrainMaterialEdit_PropagateNonTargetRegionReplacement(gridY,countOrGridX);
    }
  }
  return;
}


/* Address: 0x005616D0.
   Editor command INGAME_COMMAND_EDITOR_COMMIT_HEIGHTS (0x25A0; U key in height mode): toggles the last height
   edit. Each cell with a delta in the player's height plane is lowered by it (the water surface keeps its
   level), the delta is negated so the next call redoes the edit, and the normals and lighting of the cell and
   its lattice neighbours are recomputed. commandArg0 is the player runtime id.
*/
void TerrainEditBuffer_CommitHeightDeltasAndRefreshLighting
          (uint32_t commandArg0,uint32_t commandArg1,uint32_t commandArg2,uint32_t commandArg3)

{
  FieldGridAsset *fieldGridAsset;
  FieldGridDimension widthCells;
  int heightDelta;
  int remainingCount;
  int rowStrideBytes;
  FieldGridCell *cell;
  FieldGridCell *fieldCell;
  int *heightDeltaCursor;

  fieldGridAsset = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  heightDeltaCursor = g_SelectionPlayerRuntimeBlockPointers[commandArg0]->terrainHeightScratchPlane8088;
  widthCells = fieldGridAsset->gridWidth;
  remainingCount = widthCells * fieldGridAsset->gridHeight;
  fieldGridAsset->runtimeStateFlags = fieldGridAsset->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
  rowStrideBytes = widthCells * 0x80; /* 0x80-byte FieldGridCell records */
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


/* Address: 0x00561830.
   Editor command INGAME_COMMAND_EDITOR_COPY_MATERIALS (0x2700; start of a material brush stroke): saves the
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

  fieldGridAsset = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  materialCursor = (TerrainMaterialIndex *)
           g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->terrainMaterialEditPlane808C;
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


/* Address: 0x00561930.
   Editor command INGAME_COMMAND_EDITOR_SUBTRACT_MATERIALS (0x2800; end of a material brush stroke): subtracts
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

  fieldGridAsset = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  materialDeltaCursor = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->terrainMaterialEditPlane808C;
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


/* Address: 0x005619A0.
   Editor command INGAME_COMMAND_EDITOR_COMMIT_MATERIALS (0x2870; U key in material mode): toggles the last
   material edit by adding the player's material deltas to the cells and negating them, so the next call
   redoes the edit. commandArg0 is the player runtime id.
*/
void TerrainEditBuffer_CommitFlagsAndMaterialDeltas
          (uint32_t commandArg0,uint32_t commandArg1,uint32_t commandArg2,uint32_t commandArg3)

{
  FieldGridAsset *fieldGridAsset;
  int remainingCount;
  FieldGridCell *fieldCell;
  uint32_t *materialDeltaCursor;

  fieldGridAsset = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  materialDeltaCursor = g_SelectionPlayerRuntimeBlockPointers[commandArg0]->terrainMaterialEditPlane808C;
  remainingCount = fieldGridAsset->gridWidth * fieldGridAsset->gridHeight;
  /* The original ORs 1 into the dword 0x14C bytes before the field grid asset (OR [ESI-0x14C],1) instead of
     its runtimeStateFlags at +0xB4, so the surface is not marked dirty here; kept as in the original. */
  *(uint32_t *)((uint8_t *)fieldGridAsset - 0x14c) = *(uint32_t *)((uint8_t *)fieldGridAsset - 0x14c) | 1;
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


/* Address: 0x00561DC0.
   Editor command INGAME_COMMAND_EDITOR_HEIGHTS_TO_DELTAS (0x2C90; end of a height brush stroke): replaces each
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

  fieldGridAsset = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  heightCursor = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->terrainHeightScratchPlane8088;
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


/* Address: 0x00513790.
   Visit step of the mining-region flood fill (TerrainRegionCollection_CollectConnectedCellsRecursive):
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
  extractionDescriptor = cell->resourceExtractionDescriptor7C;
  cell->flagsAndMaterial = cell->flagsAndMaterial | FIELD_CELL_CONNECTED_REGION_VISITED;
  if ((requiredOccupancyMask & extractionDescriptor) != 0) {
    cell->resourceExtractionDescriptor7C = 0;
    /* XCHG: take the model offset and clear it in one instruction */
    LOCK();
    savedArmyOffset = cell->armyRuntimeSavedOffset6C;
    cell->armyRuntimeSavedOffset6C = 0;
    entriesBase = g_TerrainRegionCollectionEntries;
    UNLOCK();
    if (storedCount < TERRAIN_REGION_COLLECTION_CAPACITY) {
      g_TerrainRegionCollectionStoredCount++;
      *(uint32_t *)(g_TerrainRegionCollectionEntries + storedCount * 8) = extractionDescriptor;
      *(ArmyRuntimeSavedOffset *)(entriesBase + 4 + storedCount * 8) = savedArmyOffset;
    }
  }
}


/* Address: 0x00571600.
   Scanline flood fill of TerrainMaterialEdit_SeedMatchingRegionReplacement: from cell (gridY, gridX) gives
   the run of cells whose material is g_TerrainMaterialEditReferenceMaterialByte the replacement material
   (adding old - new to g_TerrainMaterialEditDeltaBuffer), then recurses into the previous row over the run's
   columns and into the next row shifted one column left (triangular lattice neighbours).
   The globals hold the field grid and the buffer as plain addresses (cast to FieldGridAsset / FieldGridCell
   where used).
*/
void TerrainMaterialEdit_PropagateMatchingRegionReplacement(FieldGridCellCoordinate gridY,FieldGridCellCoordinate gridX)

{
  uint32_t referenceMaterial;
  int cellIndexOrColumn;
  uint32_t cellMaterial;
  int columnOrMaterialDelta;
  int widthOrColumn;
  int leftCellAddress;
  int *leftDeltaCursor;
  int *rightDeltaCursor;
  int spanStartColumn;

  referenceMaterial = g_TerrainMaterialEditReferenceMaterialByte;
  if ((((-1 < gridY) && (-1 < gridX)) &&
      (widthOrColumn = (int)((FieldGridAsset *)g_TerrainMaterialEditFieldGrid)->gridWidth,
      gridY < (int)((FieldGridAsset *)g_TerrainMaterialEditFieldGrid)->gridHeight)) && (gridX < widthOrColumn)) {
    cellIndexOrColumn = gridY * widthOrColumn + gridX;
    rightDeltaCursor = (int *)(g_TerrainMaterialEditDeltaBuffer + cellIndexOrColumn * 4);
    cellIndexOrColumn = (int)&((FieldGridAsset *)g_TerrainMaterialEditFieldGrid)->cells[cellIndexOrColumn];
    cellMaterial = (uint32_t)((FieldGridCell *)cellIndexOrColumn)->flagsAndMaterial & FIELD_CELL_MATERIAL_ID_MASK;
    columnOrMaterialDelta = gridX;
    leftCellAddress = cellIndexOrColumn;
    leftDeltaCursor = rightDeltaCursor;
    if (g_TerrainMaterialEditReferenceMaterialByte == cellMaterial) {
      /* the start cell and the matching cells to its left */
      do {
        spanStartColumn = columnOrMaterialDelta;
        columnOrMaterialDelta = cellMaterial - g_TerrainMaterialEditReplacementMaterialByte;
        ((FieldGridCell *)leftCellAddress)->flagsAndMaterial = ((FieldGridCell *)leftCellAddress)->flagsAndMaterial - columnOrMaterialDelta;
        *leftDeltaCursor = *leftDeltaCursor + columnOrMaterialDelta;
        if (spanStartColumn < 1) break;
        cellMaterial = (uint32_t)((FieldGridCell *)leftCellAddress)[-1].flagsAndMaterial & FIELD_CELL_MATERIAL_ID_MASK;
        columnOrMaterialDelta = spanStartColumn - 1;
        leftCellAddress = leftCellAddress - 0x80;
        leftDeltaCursor = leftDeltaCursor - 1;
      } while (referenceMaterial == cellMaterial);
      /* the matching cells to its right; gridX ends one past the run */
      while( true ) {
        gridX = gridX + 1;
        rightDeltaCursor = rightDeltaCursor + 1;
        if ((widthOrColumn <= gridX) || (cellMaterial = (uint32_t)((FieldGridCell *)cellIndexOrColumn)[1].flagsAndMaterial & FIELD_CELL_MATERIAL_ID_MASK, referenceMaterial != cellMaterial)) break;
        columnOrMaterialDelta = cellMaterial - g_TerrainMaterialEditReplacementMaterialByte;
        ((FieldGridCell *)cellIndexOrColumn)[1].flagsAndMaterial = ((FieldGridCell *)cellIndexOrColumn)[1].flagsAndMaterial - columnOrMaterialDelta;
        *rightDeltaCursor = *rightDeltaCursor + columnOrMaterialDelta;
        cellIndexOrColumn = cellIndexOrColumn + 0x80;
      }
      widthOrColumn = spanStartColumn;
      do {
        cellIndexOrColumn = widthOrColumn + 1;
        TerrainMaterialEdit_PropagateMatchingRegionReplacement(gridY - 1,widthOrColumn);
        widthOrColumn = cellIndexOrColumn;
      } while (cellIndexOrColumn <= gridX);
      widthOrColumn = spanStartColumn - 1;
      do {
        cellIndexOrColumn = widthOrColumn + 1;
        TerrainMaterialEdit_PropagateMatchingRegionReplacement(gridY + 1,widthOrColumn);
        widthOrColumn = cellIndexOrColumn;
      } while (cellIndexOrColumn < gridX);
    }
  }
  return;
}


/* Address: 0x00571730.
   Scanline flood fill of TerrainMaterialEdit_SeedNonTargetRegionReplacement: the same walk as
   TerrainMaterialEdit_PropagateMatchingRegionReplacement, but over the cells whose material differs from
   g_TerrainMaterialEditReferenceMaterialByte, which all receive that material (old - new goes to
   g_TerrainMaterialEditDeltaBuffer).
*/
void TerrainMaterialEdit_PropagateNonTargetRegionReplacement
          (FieldGridCellCoordinate gridY,FieldGridCellCoordinate gridX)

{
  uint32_t referenceMaterial;
  int cellIndexOrColumn;
  uint32_t cellMaterial;
  int columnOrMaterialDelta;
  int widthOrColumn;
  int leftCellAddress;
  int *leftDeltaCursor;
  int *rightDeltaCursor;
  int spanStartColumn;

  referenceMaterial = g_TerrainMaterialEditReferenceMaterialByte;
  if ((((-1 < gridY) && (-1 < gridX)) &&
      (widthOrColumn = (int)((FieldGridAsset *)g_TerrainMaterialEditFieldGrid)->gridWidth,
      gridY < (int)((FieldGridAsset *)g_TerrainMaterialEditFieldGrid)->gridHeight)) && (gridX < widthOrColumn)) {
    cellIndexOrColumn = gridY * widthOrColumn + gridX;
    rightDeltaCursor = (int *)(g_TerrainMaterialEditDeltaBuffer + cellIndexOrColumn * 4);
    cellIndexOrColumn = (int)&((FieldGridAsset *)g_TerrainMaterialEditFieldGrid)->cells[cellIndexOrColumn];
    cellMaterial = (uint32_t)((FieldGridCell *)cellIndexOrColumn)->flagsAndMaterial & FIELD_CELL_MATERIAL_ID_MASK;
    columnOrMaterialDelta = gridX;
    leftCellAddress = cellIndexOrColumn;
    leftDeltaCursor = rightDeltaCursor;
    if (g_TerrainMaterialEditReferenceMaterialByte != cellMaterial) {
      do {
        spanStartColumn = columnOrMaterialDelta;
        ((FieldGridCell *)leftCellAddress)->flagsAndMaterial = ((FieldGridCell *)leftCellAddress)->flagsAndMaterial - (cellMaterial - referenceMaterial);
        *leftDeltaCursor = *leftDeltaCursor + (cellMaterial - referenceMaterial);
        if (spanStartColumn < 1) break;
        cellMaterial = (uint32_t)((FieldGridCell *)leftCellAddress)[-1].flagsAndMaterial & FIELD_CELL_MATERIAL_ID_MASK;
        columnOrMaterialDelta = spanStartColumn - 1;
        leftCellAddress = leftCellAddress - 0x80;
        leftDeltaCursor = leftDeltaCursor - 1;
      } while (referenceMaterial != cellMaterial);
      while( true ) {
        gridX = gridX + 1;
        rightDeltaCursor = rightDeltaCursor + 1;
        if ((widthOrColumn <= gridX) || (cellMaterial = (uint32_t)((FieldGridCell *)cellIndexOrColumn)[1].flagsAndMaterial & FIELD_CELL_MATERIAL_ID_MASK, referenceMaterial == cellMaterial)) break;
        columnOrMaterialDelta = cellMaterial - referenceMaterial;
        ((FieldGridCell *)cellIndexOrColumn)[1].flagsAndMaterial = ((FieldGridCell *)cellIndexOrColumn)[1].flagsAndMaterial - columnOrMaterialDelta;
        *rightDeltaCursor = *rightDeltaCursor + columnOrMaterialDelta;
        cellIndexOrColumn = cellIndexOrColumn + 0x80;
      }
      widthOrColumn = spanStartColumn;
      do {
        cellIndexOrColumn = widthOrColumn + 1;
        TerrainMaterialEdit_PropagateNonTargetRegionReplacement(gridY - 1,widthOrColumn);
        widthOrColumn = cellIndexOrColumn;
      } while (cellIndexOrColumn <= gridX);
      widthOrColumn = spanStartColumn - 1;
      do {
        cellIndexOrColumn = widthOrColumn + 1;
        TerrainMaterialEdit_PropagateNonTargetRegionReplacement(gridY + 1,widthOrColumn);
        widthOrColumn = cellIndexOrColumn;
      } while (cellIndexOrColumn < gridX);
    }
  }
  return;
}

