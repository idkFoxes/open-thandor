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
void __thandor_void_preserve_eax_ecx_edx
TerrainRegionCollection_CollectConnectedCellsRecursive
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
   Ownership: world/terrain/editing.
   Purpose: Clears the selected player material edit buffer, converts world coordinates to a field cell, captures
   the original and replacement material bytes, marks the field dirty, and starts matching-region propagation. It
   is separate from FactionRuntimeIndex, PlayerRuntimeId, network-player identity, and PCK-backed asset
   identifiers. Typed parameters: p4 worldYQ12→Q12, p5 worldXQ12→Q12. Nearby but non-identical semantic domains
   were explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Local calls: TerrainMaterialEdit_PropagateMatchingRegionReplacement.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainMaterialEdit_SeedMatchingRegionReplacement
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
  for (countOrGridX = fieldGridAsset->gridWidth * fieldGridAsset->gridHeight; countOrGridX != 0; countOrGridX = countOrGridX + -1) {
    *editPlaneCursor = 0;
    editPlaneCursor = editPlaneCursor + 1;
  }
  countOrGridX = worldXQ12 >> 0xc;
  if ((((-1 < countOrGridX) && (gridY = worldYQ12 >> 0xc, -1 < gridY)) && (countOrGridX < (int)fieldGridAsset->gridWidth))
     && (gridY < (int)fieldGridAsset->gridHeight)) {
    referenceMaterial = fieldGridAsset->cells[gridY * fieldGridAsset->gridWidth + countOrGridX].flagsAndMaterial &
            FIELD_CELL_MATERIAL_ID_MASK;
    editPlaneCursor = playerBlock->terrainMaterialEditPlane808C;
    if (referenceMaterial != replacementMaterialByte) {
      fieldGridAsset->runtimeStateFlags = fieldGridAsset->runtimeStateFlags | 1;
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
   Ownership: world/terrain/editing.
   Purpose: Clears the selected player material edit buffer, converts world coordinates to a field cell, records
   the requested material byte, marks the field dirty, and starts propagation through connected non-target cells.
   It is separate from FactionRuntimeIndex, PlayerRuntimeId, network-player identity, and PCK-backed asset
   identifiers. Typed parameters: p4 worldYQ12→Q12, p5 worldXQ12→Q12. Nearby but non-identical semantic domains
   were explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Local calls: TerrainMaterialEdit_PropagateNonTargetRegionReplacement.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainMaterialEdit_SeedNonTargetRegionReplacement
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
  for (countOrGridX = fieldGridAsset->gridWidth * fieldGridAsset->gridHeight; countOrGridX != 0; countOrGridX = countOrGridX + -1) {
    *editPlaneCursor = 0;
    editPlaneCursor = editPlaneCursor + 1;
  }
  countOrGridX = worldXQ12 >> 0xc;
  if ((((-1 < countOrGridX) && (gridY = worldYQ12 >> 0xc, -1 < gridY)) && (countOrGridX < (int)fieldGridAsset->gridWidth))
     && (gridY < (int)fieldGridAsset->gridHeight)) {
    editPlaneCursor = playerBlock->terrainMaterialEditPlane808C;
    if ((fieldGridAsset->cells[gridY * fieldGridAsset->gridWidth + countOrGridX].flagsAndMaterial &
        FIELD_CELL_MATERIAL_ID_MASK) != referenceMaterialByte) {
      fieldGridAsset->runtimeStateFlags = fieldGridAsset->runtimeStateFlags | 1;
      g_TerrainMaterialEditReferenceMaterialByte = referenceMaterialByte;
      g_TerrainMaterialEditFieldGrid = fieldGridAsset;
      g_TerrainMaterialEditDeltaBuffer = editPlaneCursor;
      TerrainMaterialEdit_PropagateNonTargetRegionReplacement(gridY,countOrGridX);
    }
  }
  return;
}


/* Address: 0x005616D0.
   Ownership: world/terrain/editing.
   Purpose: Commits per-cell terrain-height edit deltas from a player/runtime edit buffer into the FieldGrid and
   recomputes affected triangle normals and directional lighting.
   Cross-module calls: FieldGridCell_RecomputeTriangleNormalAngles [world/terrain/grid],
   FieldGridCell_ComputeDirectionalLightColor [world/terrain/grid].
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainEditBuffer_CommitHeightDeltasAndRefreshLighting
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
  fieldGridAsset->runtimeStateFlags = fieldGridAsset->runtimeStateFlags | 1;
  rowStrideBytes = widthCells * 0x80;
  fieldCell = fieldGridAsset->cells;
  do {
    heightDelta = *heightDeltaCursor;
    if (heightDelta != 0) {
      fieldCell->terrainHeight = fieldCell->terrainHeight - heightDelta;
      fieldCell->waterSurfaceDelta = fieldCell->waterSurfaceDelta + heightDelta;
      *heightDeltaCursor = -*heightDeltaCursor;
      if ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,fieldCell);
        FieldGridCell_ComputeDirectionalLightColor(fieldCell);
        if (((fieldCell[-1].flagsAndMaterial & 0x88006000) == 0) && (heightDeltaCursor[-1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,fieldCell + -1);
          FieldGridCell_ComputeDirectionalLightColor(fieldCell + -1);
        }
        if (((fieldCell[1].flagsAndMaterial & 0x88006000) == 0) && (heightDeltaCursor[1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,fieldCell + 1);
          FieldGridCell_ComputeDirectionalLightColor(fieldCell + 1);
        }
        fieldCell = fieldCell + -widthCells;
        if ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,fieldCell);
          FieldGridCell_ComputeDirectionalLightColor(fieldCell);
        }
        if ((fieldCell[1].flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,fieldCell + 1);
          FieldGridCell_ComputeDirectionalLightColor(fieldCell + 1);
        }
        fieldCell = fieldCell + widthCells * 2 + -1;
        if ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,fieldCell);
          FieldGridCell_ComputeDirectionalLightColor(fieldCell);
        }
        cell = fieldCell + 1;
        if ((fieldCell[1].flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell);
          FieldGridCell_ComputeDirectionalLightColor(cell);
        }
        fieldCell = cell + -widthCells;
      }
    }
    fieldCell = fieldCell + 1;
    heightDeltaCursor = heightDeltaCursor + 1;
    remainingCount = remainingCount + -1;
  } while (remainingCount != 0);
  return;
}


/* Address: 0x00561830.
   Ownership: world/terrain/editing.
   Purpose: Copies the low material byte from every 0x80-byte field cell into the selected player terrain-edit
   buffer at runtime offset +0x808C. It is separate from FactionRuntimeIndex, PlayerRuntimeId, network-player
   identity, and PCK-backed asset identifiers.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainEditBuffer_CopyCellMaterialBytes
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
    remainingCount = remainingCount + -1;
  } while (remainingCount != 0);
  return;
}


/* Address: 0x00561930.
   Ownership: world/terrain/editing.
   Purpose: It is separate from FactionRuntimeIndex, PlayerRuntimeId, network-player identity, and PCK-backed asset
   identifiers.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainEditBuffer_SubtractCurrentCellMaterialBytes
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
    remainingCount = remainingCount + -1;
  } while (remainingCount != 0);
  return;
}


/* Address: 0x005619A0.
   Ownership: world/terrain/editing.
   Purpose: Commits per-cell flags/material edit deltas from the corresponding player/runtime edit buffer into
   FieldGridCell flags/material state.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainEditBuffer_CommitFlagsAndMaterialDeltas
          (uint32_t commandArg0,uint32_t commandArg1,uint32_t commandArg2,uint32_t commandArg3)

{
  FieldGridAsset *fieldGridAsset;
  int remainingCount;
  FieldGridCell *fieldCell;
  uint32_t *materialDeltaCursor;
  
  fieldGridAsset = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  materialDeltaCursor = g_SelectionPlayerRuntimeBlockPointers[commandArg0]->terrainMaterialEditPlane808C;
  remainingCount = fieldGridAsset->gridWidth * fieldGridAsset->gridHeight;
  *(uint32_t *)(fieldGridAsset[-1].sourcePath + 0x1a) = *(uint32_t *)(fieldGridAsset[-1].sourcePath + 0x1a) | 1;
  fieldCell = fieldGridAsset->cells;
  do {
    fieldCell->flagsAndMaterial = fieldCell->flagsAndMaterial + *materialDeltaCursor;
    *materialDeltaCursor = -*materialDeltaCursor;
    fieldCell = fieldCell + 1;
    materialDeltaCursor = materialDeltaCursor + 1;
    remainingCount = remainingCount + -1;
  } while (remainingCount != 0);
  return;
}


/* Address: 0x00561DC0.
   Ownership: world/terrain/editing.
   Purpose: Replaces each selected-player height-buffer value with fieldCell.terrainHeight minus the previous
   buffer value. It is separate from FactionRuntimeIndex, PlayerRuntimeId, network-player identity, and PCK-backed
   asset identifiers.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainEditBuffer_ConvertHeightsToDeltas
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
    remainingCount = remainingCount + -1;
  } while (remainingCount != 0);
  return;
}


/* Address: 0x00513790.
   Visit step of the mining-region flood fill (TerrainRegionCollection_CollectConnectedCellsRecursive):
   counts and marks the cell as visited and, when its extraction descriptor matches requiredOccupancyMask,
   moves the descriptor and the extracting model's offset out of the cell into the region collection (at
   most TERRAIN_REGION_COLLECTION_CAPACITY entries), so the per-tick mining pays each extractor once.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainRegionCollection_RecordConnectedCell
          (FieldGridRegionMask requiredOccupancyMask,FieldGridCell *cell)

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
   Ownership: world/terrain/editing.
   Purpose: Recursively replaces a connected region whose material equals the captured original byte, updates the
   paired delta buffer, and propagates through neighboring rows and columns. Typed parameters: p0
   gridY→FieldGridCellCoordinate_V331, p1 gridX→FieldGridCellCoordinate_V331. Calling convention, parameter
   storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_eax_preserve_ecx_edx
TerrainMaterialEdit_PropagateMatchingRegionReplacement
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
      (widthOrColumn = *(int *)(g_TerrainMaterialEditFieldGrid + 0xb8),
      gridY < *(int *)(g_TerrainMaterialEditFieldGrid + 0xbc))) && (gridX < widthOrColumn)) {
    cellIndexOrColumn = gridY * widthOrColumn + gridX;
    rightDeltaCursor = (int *)(g_TerrainMaterialEditDeltaBuffer + cellIndexOrColumn * 4);
    cellIndexOrColumn = cellIndexOrColumn * 0x80 + 0x200 + g_TerrainMaterialEditFieldGrid;
    cellMaterial = *(uint32_t *)(cellIndexOrColumn + 0x50) & 0xff;
    columnOrMaterialDelta = gridX;
    leftCellAddress = cellIndexOrColumn;
    leftDeltaCursor = rightDeltaCursor;
    if (g_TerrainMaterialEditReferenceMaterialByte == cellMaterial) {
      do {
        spanStartColumn = columnOrMaterialDelta;
        columnOrMaterialDelta = cellMaterial - g_TerrainMaterialEditReplacementMaterialByte;
        *(int *)(leftCellAddress + 0x50) = *(int *)(leftCellAddress + 0x50) - columnOrMaterialDelta;
        *leftDeltaCursor = *leftDeltaCursor + columnOrMaterialDelta;
        if (spanStartColumn < 1) break;
        cellMaterial = *(uint32_t *)(leftCellAddress + -0x30) & 0xff;
        columnOrMaterialDelta = spanStartColumn + -1;
        leftCellAddress = leftCellAddress + -0x80;
        leftDeltaCursor = leftDeltaCursor + -1;
      } while (referenceMaterial == cellMaterial);
      LOCK();
      UNLOCK();
      while( true ) {
        gridX = gridX + 1;
        rightDeltaCursor = rightDeltaCursor + 1;
        if ((widthOrColumn <= gridX) || (cellMaterial = *(uint32_t *)(cellIndexOrColumn + 0xd0) & 0xff, referenceMaterial != cellMaterial)) break;
        columnOrMaterialDelta = cellMaterial - g_TerrainMaterialEditReplacementMaterialByte;
        *(int *)(cellIndexOrColumn + 0xd0) = *(int *)(cellIndexOrColumn + 0xd0) - columnOrMaterialDelta;
        *rightDeltaCursor = *rightDeltaCursor + columnOrMaterialDelta;
        cellIndexOrColumn = cellIndexOrColumn + 0x80;
      }
      widthOrColumn = spanStartColumn;
      do {
        cellIndexOrColumn = widthOrColumn + 1;
        TerrainMaterialEdit_PropagateMatchingRegionReplacement(gridY + -1,widthOrColumn);
        widthOrColumn = cellIndexOrColumn;
      } while (cellIndexOrColumn <= gridX);
      widthOrColumn = spanStartColumn + -1;
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
   Ownership: world/terrain/editing.
   Purpose: Recursively replaces connected cells whose material differs from the requested target byte, updates the
   paired delta buffer, and propagates through neighboring rows and columns. Typed parameters: p0
   gridY→FieldGridCellCoordinate_V331, p1 gridX→FieldGridCellCoordinate_V331. Calling convention, parameter
   storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_eax_preserve_ecx_edx
TerrainMaterialEdit_PropagateNonTargetRegionReplacement
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
      (widthOrColumn = *(int *)(g_TerrainMaterialEditFieldGrid + 0xb8),
      gridY < *(int *)(g_TerrainMaterialEditFieldGrid + 0xbc))) && (gridX < widthOrColumn)) {
    cellIndexOrColumn = gridY * widthOrColumn + gridX;
    rightDeltaCursor = (int *)(g_TerrainMaterialEditDeltaBuffer + cellIndexOrColumn * 4);
    cellIndexOrColumn = cellIndexOrColumn * 0x80 + 0x200 + g_TerrainMaterialEditFieldGrid;
    cellMaterial = *(uint32_t *)(cellIndexOrColumn + 0x50) & 0xff;
    columnOrMaterialDelta = gridX;
    leftCellAddress = cellIndexOrColumn;
    leftDeltaCursor = rightDeltaCursor;
    if (g_TerrainMaterialEditReferenceMaterialByte != cellMaterial) {
      do {
        spanStartColumn = columnOrMaterialDelta;
        *(int *)(leftCellAddress + 0x50) = *(int *)(leftCellAddress + 0x50) - (cellMaterial - referenceMaterial);
        *leftDeltaCursor = *leftDeltaCursor + (cellMaterial - referenceMaterial);
        if (spanStartColumn < 1) break;
        cellMaterial = *(uint32_t *)(leftCellAddress + -0x30) & 0xff;
        columnOrMaterialDelta = spanStartColumn + -1;
        leftCellAddress = leftCellAddress + -0x80;
        leftDeltaCursor = leftDeltaCursor + -1;
      } while (referenceMaterial != cellMaterial);
      LOCK();
      UNLOCK();
      while( true ) {
        gridX = gridX + 1;
        rightDeltaCursor = rightDeltaCursor + 1;
        if ((widthOrColumn <= gridX) || (cellMaterial = *(uint32_t *)(cellIndexOrColumn + 0xd0) & 0xff, referenceMaterial == cellMaterial)) break;
        columnOrMaterialDelta = cellMaterial - referenceMaterial;
        *(int *)(cellIndexOrColumn + 0xd0) = *(int *)(cellIndexOrColumn + 0xd0) - columnOrMaterialDelta;
        *rightDeltaCursor = *rightDeltaCursor + columnOrMaterialDelta;
        cellIndexOrColumn = cellIndexOrColumn + 0x80;
      }
      widthOrColumn = spanStartColumn;
      do {
        cellIndexOrColumn = widthOrColumn + 1;
        TerrainMaterialEdit_PropagateNonTargetRegionReplacement(gridY + -1,widthOrColumn);
        widthOrColumn = cellIndexOrColumn;
      } while (cellIndexOrColumn <= gridX);
      widthOrColumn = spanStartColumn + -1;
      do {
        cellIndexOrColumn = widthOrColumn + 1;
        TerrainMaterialEdit_PropagateNonTargetRegionReplacement(gridY + 1,widthOrColumn);
        widthOrColumn = cellIndexOrColumn;
      } while (cellIndexOrColumn < gridX);
    }
  }
  return;
}

