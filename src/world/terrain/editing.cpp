/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/editing.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/terrain/editing.h>
#include <thandor/thandor.h>
#include <thandor/world/terrain/field_edit_commands.h>

/* Module data. */

static FieldGridAsset *g_TerrainMaterialEditFieldGrid = nullptr;

static int *g_TerrainMaterialEditDeltaBuffer = nullptr; /* the player's material plane, one int per cell */

static uint32_t g_TerrainMaterialEditReferenceMaterialByte = 0;

static uint32_t g_TerrainMaterialEditReplacementMaterialByte = 0;

TerrainRegionCollectionCount g_TerrainRegionCollectionStoredCount = 0;

TerrainRegionCollectionCount g_TerrainRegionCollectionVisitedCount = 0;

uintptr_t g_TerrainRegionCollectionEntries = 0;

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
  while (((FieldCell_RawBits(spanStartCell[-1].flagsAndMaterial) & requiredCellFlags) != 0) &&
         ((FieldCell_RawBits(spanStartCell[-1].flagsAndMaterial) & TERRAIN_REGION_STOP_FLAGS) == 0)) {
    spanStartCell = spanStartCell - 1;
    TerrainRegionCollection_RecordConnectedCell(requiredCellFlags,spanStartCell);
  }
  /* the matching cells to its right; spanStopCell ends as the first cell right of the span */
  spanStopCell = cell + 1;
  while (((FieldCell_RawBits(spanStopCell->flagsAndMaterial) & requiredCellFlags) != 0) &&
         ((FieldCell_RawBits(spanStopCell->flagsAndMaterial) & TERRAIN_REGION_STOP_FLAGS) == 0)) {
    TerrainRegionCollection_RecordConnectedCell(requiredCellFlags,spanStopCell);
    spanStopCell = spanStopCell + 1;
  }
  /* row above: from the span start up to the column of the cell that ended the span */
  for (aboveRowCell = FieldGridCell_AtByteOffset(spanStartCell,-rowStrideBytes);
       aboveRowCell <= FieldGridCell_AtByteOffset(spanStopCell,-rowStrideBytes);
       aboveRowCell = aboveRowCell + 1) {
    if (((FieldCell_RawBits(aboveRowCell->flagsAndMaterial) & TERRAIN_REGION_STOP_FLAGS) == 0) &&
       ((FieldCell_RawBits(aboveRowCell->flagsAndMaterial) & requiredCellFlags) != 0)) {
      TerrainRegionCollection_CollectConnectedCellsRecursive
                (requiredCellFlags,rowStrideBytes,aboveRowCell);
    }
  }
  /* row below: from one cell left of the span start up to the span's last cell */
  for (belowRowCell = FieldGridCell_AtByteOffset(spanStartCell - 1,rowStrideBytes);
       belowRowCell < FieldGridCell_AtByteOffset(spanStopCell,rowStrideBytes);
       belowRowCell = belowRowCell + 1) {
    if (((FieldCell_RawBits(belowRowCell->flagsAndMaterial) & TERRAIN_REGION_STOP_FLAGS) == 0) &&
       ((FieldCell_RawBits(belowRowCell->flagsAndMaterial) & requiredCellFlags) != 0)) {
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
  FieldGridAsset *fieldGridAsset;
  TerrainMaterialIndex referenceMaterial;
  int remainingCount;
  int gridX;
  int gridY;
  uint32_t *editPlaneCursor;

  editPlaneCursor = FieldGridEdit_PlayerMaterialPlane(playerIndex);
  if (editPlaneCursor == nullptr) {
    return;
  }
  /* The original keeps the whole command value; bounded here to the material byte because it comes from any
     network peer and a larger value would carry into the cell flags (the edge ring) during the fill. */
  replacementMaterialByte &= ToBits(FIELD_CELL_MATERIAL_ID_MASK);
  fieldGridAsset = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
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
  referenceMaterial = FieldCell_MaterialId(fieldGridAsset->cells[(int32_t)(gridY * fieldGridAsset->gridWidth + gridX)].flagsAndMaterial);
  if (referenceMaterial != replacementMaterialByte) {
    fieldGridAsset->runtimeStateFlags = fieldGridAsset->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
    g_TerrainMaterialEditReplacementMaterialByte = replacementMaterialByte;
    g_TerrainMaterialEditFieldGrid = fieldGridAsset;
    g_TerrainMaterialEditDeltaBuffer = reinterpret_cast<int *>(FieldGridEdit_PlayerMaterialPlane(playerIndex));
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
  FieldGridAsset *fieldGridAsset;
  int remainingCount;
  int gridX;
  int gridY;
  uint32_t *editPlaneCursor;

  editPlaneCursor = FieldGridEdit_PlayerMaterialPlane(playerIndex);
  if (editPlaneCursor == nullptr) {
    return;
  }
  /* The original keeps the whole command value; bounded here to the material byte because it comes from any
     network peer and a larger value would carry into the cell flags (the edge ring) during the fill. */
  referenceMaterialByte &= ToBits(FIELD_CELL_MATERIAL_ID_MASK);
  fieldGridAsset = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
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
  if (ToBits(fieldGridAsset->cells[(int32_t)(gridY * fieldGridAsset->gridWidth + gridX)].flagsAndMaterial &
             FIELD_CELL_MATERIAL_ID_MASK) != referenceMaterialByte) {
    fieldGridAsset->runtimeStateFlags = fieldGridAsset->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
    g_TerrainMaterialEditReferenceMaterialByte = referenceMaterialByte;
    g_TerrainMaterialEditFieldGrid = fieldGridAsset;
    g_TerrainMaterialEditDeltaBuffer = reinterpret_cast<int *>(FieldGridEdit_PlayerMaterialPlane(playerIndex));
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

  heightDeltaCursor = FieldGridEdit_PlayerHeightPlane(playerRuntimeId);
  if (heightDeltaCursor == nullptr) {
    return;
  }
  fieldGridAsset = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  widthCells = fieldGridAsset->gridWidth;
  remainingCount = widthCells * fieldGridAsset->gridHeight;
  fieldGridAsset->runtimeStateFlags = fieldGridAsset->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
  rowStrideBytes = widthCells * sizeof(FieldGridCell); /* 0x80-byte FieldGridCell records */
  fieldCell = fieldGridAsset->cells;
  /* count >= 16: the world field grid passed FieldGrid_ValidateLoadedImage (sides >= FIELD_GRID_MIN_SIDE_CELLS) on load */
  do {
    heightDelta = *heightDeltaCursor;
    if (heightDelta != 0) {
      fieldCell->terrainHeight = fieldCell->terrainHeight - heightDelta;
      fieldCell->waterSurfaceDelta = fieldCell->waterSurfaceDelta + heightDelta;
      *heightDeltaCursor = -*heightDeltaCursor;
      if (!Any(fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK)) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,fieldCell);
        FieldGridCell_ComputeDirectionalLightColor(fieldCell);
        /* left and right neighbours; one with its own delta is refreshed on its own turn */
        if ((!Any(fieldCell[-1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK)) && (heightDeltaCursor[-1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,fieldCell - 1);
          FieldGridCell_ComputeDirectionalLightColor(fieldCell - 1);
        }
        if ((!Any(fieldCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK)) && (heightDeltaCursor[1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,fieldCell + 1);
          FieldGridCell_ComputeDirectionalLightColor(fieldCell + 1);
        }
        /* the two neighbours in the previous row (same column and the one to the right) */
        fieldCell = fieldCell + -(int32_t)widthCells;
        if (!Any(fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,fieldCell);
          FieldGridCell_ComputeDirectionalLightColor(fieldCell);
        }
        if (!Any(fieldCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,fieldCell + 1);
          FieldGridCell_ComputeDirectionalLightColor(fieldCell + 1);
        }
        /* the two neighbours in the next row (the one to the left and same column) */
        fieldCell = fieldCell + widthCells * 2 + -1;
        if (!Any(fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,fieldCell);
          FieldGridCell_ComputeDirectionalLightColor(fieldCell);
        }
        cell = fieldCell + 1;
        if (!Any(fieldCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell);
          FieldGridCell_ComputeDirectionalLightColor(cell);
        }
        fieldCell = cell + -(int32_t)widthCells;
      }
    }
    fieldCell = fieldCell + 1;
    heightDeltaCursor = heightDeltaCursor + 1;
    remainingCount--;
  } while (remainingCount != 0);
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

  materialCursor = reinterpret_cast<TerrainMaterialIndex *>(FieldGridEdit_PlayerMaterialPlane(playerRuntimeId)); /* the plane's uint32_t words as material indices */
  if (materialCursor == nullptr) {
    return;
  }
  fieldGridAsset = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  remainingCount = fieldGridAsset->gridWidth * fieldGridAsset->gridHeight;
  fieldCell = fieldGridAsset->cells;
  /* count >= 16: the world field grid passed FieldGrid_ValidateLoadedImage (sides >= FIELD_GRID_MIN_SIDE_CELLS) on load */
  do {
    *materialCursor = FieldCell_MaterialId(fieldCell->flagsAndMaterial);
    fieldCell = fieldCell + 1;
    materialCursor = materialCursor + 1;
    remainingCount--;
  } while (remainingCount != 0);
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

  materialDeltaCursor = FieldGridEdit_PlayerMaterialPlane(playerRuntimeId);
  if (materialDeltaCursor == nullptr) {
    return;
  }
  fieldGridAsset = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  remainingCount = fieldGridAsset->gridWidth * fieldGridAsset->gridHeight;
  fieldCell = fieldGridAsset->cells;
  /* count >= 16: the world field grid passed FieldGrid_ValidateLoadedImage (sides >= FIELD_GRID_MIN_SIDE_CELLS) on load */
  do {
    *materialDeltaCursor = *materialDeltaCursor - FieldCell_MaterialId(fieldCell->flagsAndMaterial);
    fieldCell = fieldCell + 1;
    materialDeltaCursor = materialDeltaCursor + 1;
    remainingCount--;
  } while (remainingCount != 0);
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

  materialDeltaCursor = FieldGridEdit_PlayerMaterialPlane(playerRuntimeId);
  if (materialDeltaCursor == nullptr) {
    return;
  }
  fieldGridAsset = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  remainingCount = fieldGridAsset->gridWidth * fieldGridAsset->gridHeight;
  /* Original quirk: the original ORs 1 into the dword 0x14C bytes before the FieldGridAsset (meant as
     runtimeStateFlags, FIELD_GRID_RUNTIME_SURFACE_DIRTY), i.e. into the previous arena block, so the surface is
     not marked dirty here. That stray write is left out: it corrupts whatever block precedes the grid, and the
     command is editor-only (no determinism check runs the editor). The surface stays not marked dirty, as in the
     original. */
  fieldCell = fieldGridAsset->cells;
  /* count >= 16: the world field grid passed FieldGrid_ValidateLoadedImage (sides >= FIELD_GRID_MIN_SIDE_CELLS) on load */
  do {
    /* The original adds the delta to the whole dword; the edge-ring bits are kept here because the plane
       can hold any values when the commands come out of order from a network peer (valid deltas only
       change the material byte). */
    fieldCell->flagsAndMaterial =
         FieldCell_FromRawWord(((FieldCell_RawBits(fieldCell->flagsAndMaterial) + *materialDeltaCursor) &
                                ~FieldCell_RawBits(FIELD_CELL_GRID_EDGE_MASK)) |
                               (FieldCell_RawBits(fieldCell->flagsAndMaterial) & FieldCell_RawBits(FIELD_CELL_GRID_EDGE_MASK)));
    *materialDeltaCursor = -*materialDeltaCursor;
    fieldCell = fieldCell + 1;
    materialDeltaCursor = materialDeltaCursor + 1;
    remainingCount--;
  } while (remainingCount != 0);
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

  heightCursor = FieldGridEdit_PlayerHeightPlane(playerRuntimeId);
  if (heightCursor == nullptr) {
    return;
  }
  fieldGridAsset = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  remainingCount = fieldGridAsset->gridWidth * fieldGridAsset->gridHeight;
  fieldCell = fieldGridAsset->cells;
  /* count >= 16: the world field grid passed FieldGrid_ValidateLoadedImage (sides >= FIELD_GRID_MIN_SIDE_CELLS) on load */
  do {
    *heightCursor = fieldCell->terrainHeight - *heightCursor;
    fieldCell = fieldCell + 1;
    heightCursor = heightCursor + 1;
    remainingCount--;
  } while (remainingCount != 0);
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
  uintptr_t entriesBase;

  storedCount = g_TerrainRegionCollectionStoredCount;
  g_TerrainRegionCollectionVisitedCount++;
  extractionDescriptor = cell->resourceExtractionDescriptor;
  cell->flagsAndMaterial = cell->flagsAndMaterial | FIELD_CELL_CONNECTED_REGION_VISITED;
  if ((requiredOccupancyMask & extractionDescriptor) != 0) {
    cell->resourceExtractionDescriptor = 0;
    /* take the model offset and clear it */
    savedArmyOffset = cell->armyRuntimeSavedOffset;
    cell->armyRuntimeSavedOffset = 0;
    entriesBase = (uintptr_t)g_TerrainRegionCollectionEntries;
    if (storedCount < TERRAIN_REGION_COLLECTION_CAPACITY) {
      g_TerrainRegionCollectionStoredCount++;
      *reinterpret_cast<uint32_t *>(entriesBase + storedCount * 8) = extractionDescriptor; /* the entries' address is kept as an integer */
      *reinterpret_cast<ArmyRuntimeSavedOffset *>(entriesBase + 4 + storedCount * 8) = savedArmyOffset;
    }
  }
}


/* Step of the material fills below: gives cell its new material by subtracting materialDelta (old - new
   material) and adds the delta to the cell's slot in the undo delta buffer. */
static void TerrainMaterialEdit_ApplyCellMaterialDelta(FieldGridCell *cell,int *deltaSlot,int materialDelta)
{
  cell->flagsAndMaterial = FromBits<FieldCellPackedFlagsAndMaterial>(FieldCell_RawWord(cell->flagsAndMaterial) - materialDelta);
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
  fieldGrid = g_TerrainMaterialEditFieldGrid;
  if ((gridY < 0) || (gridX < 0)) {
    return;
  }
  gridWidth = (int)fieldGrid->gridWidth;
  if ((gridY >= (int)fieldGrid->gridHeight) || (gridX >= gridWidth)) {
    return;
  }
  cellIndex = gridY * gridWidth + gridX;
  rightDeltaCursor = g_TerrainMaterialEditDeltaBuffer + cellIndex;
  rightCell = &fieldGrid->cells[cellIndex];
  cellMaterial = FieldCell_RawBits(rightCell->flagsAndMaterial) & ToBits(FIELD_CELL_MATERIAL_ID_MASK);
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
    cellMaterial = FieldCell_RawBits(leftCell[-1].flagsAndMaterial) & ToBits(FIELD_CELL_MATERIAL_ID_MASK);
    if (referenceMaterial != cellMaterial) break;
    spanStartColumn = spanStartColumn - 1;
    leftCell = leftCell - 1;
    leftDeltaCursor = leftDeltaCursor - 1;
    TerrainMaterialEdit_ApplyCellMaterialDelta
              (leftCell,leftDeltaCursor,(int)(cellMaterial - g_TerrainMaterialEditReplacementMaterialByte));
  }
  /* the matching cells to its right; spanStopColumn ends one past the run */
  for (spanStopColumn = gridX + 1; spanStopColumn < gridWidth; spanStopColumn = spanStopColumn + 1) {
    cellMaterial = FieldCell_RawBits(rightCell[1].flagsAndMaterial) & ToBits(FIELD_CELL_MATERIAL_ID_MASK);
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
  fieldGrid = g_TerrainMaterialEditFieldGrid;
  if ((gridY < 0) || (gridX < 0)) {
    return;
  }
  gridWidth = (int)fieldGrid->gridWidth;
  if ((gridY >= (int)fieldGrid->gridHeight) || (gridX >= gridWidth)) {
    return;
  }
  cellIndex = gridY * gridWidth + gridX;
  rightDeltaCursor = g_TerrainMaterialEditDeltaBuffer + cellIndex;
  rightCell = &fieldGrid->cells[cellIndex];
  cellMaterial = FieldCell_RawBits(rightCell->flagsAndMaterial) & ToBits(FIELD_CELL_MATERIAL_ID_MASK);
  if (referenceMaterial == cellMaterial) {
    return;
  }
  /* the start cell and the non-target cells to its left */
  spanStartColumn = gridX;
  leftCell = rightCell;
  leftDeltaCursor = rightDeltaCursor;
  TerrainMaterialEdit_ApplyCellMaterialDelta(leftCell,leftDeltaCursor,(int)(cellMaterial - referenceMaterial));
  while (spanStartColumn > 0) {
    cellMaterial = FieldCell_RawBits(leftCell[-1].flagsAndMaterial) & ToBits(FIELD_CELL_MATERIAL_ID_MASK);
    if (referenceMaterial == cellMaterial) break;
    spanStartColumn = spanStartColumn - 1;
    leftCell = leftCell - 1;
    leftDeltaCursor = leftDeltaCursor - 1;
    TerrainMaterialEdit_ApplyCellMaterialDelta(leftCell,leftDeltaCursor,(int)(cellMaterial - referenceMaterial));
  }
  /* the non-target cells to its right; spanStopColumn ends one past the run */
  for (spanStopColumn = gridX + 1; spanStopColumn < gridWidth; spanStopColumn = spanStopColumn + 1) {
    cellMaterial = FieldCell_RawBits(rightCell[1].flagsAndMaterial) & ToBits(FIELD_CELL_MATERIAL_ID_MASK);
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

