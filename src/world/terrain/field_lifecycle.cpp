/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/field_lifecycle.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* Field-grid lifecycle: runtime cell initialisation after a load, cell lookup pointers, whole-grid resets
   and the asset image written back from the runtime state. */

#include <thandor/world/terrain/field_lifecycle.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Load-time cell setup: marks the field surface dirty and gives every cell a random animation phase (masked to
   the bit width stored just before the terrain surface packet table), a terrain direction record chosen by the
   low nibbles of its world X/Y, a white overlay colour and random material variant bits 8-10. Then it rebuilds
   the four map-edge flags on the outermost ring of cells, which neighbour loops test before stepping outside.
*/
void FieldGrid_InitializeRuntimeCellsAndBoundaryFlags(FieldGridAsset *fieldGrid)

{
  FieldGridDimension cellsPerRow;
  uint32_t phaseRandomValue;
  uint32_t materialVariantRandomBits;
  FieldGridDimension columnsRemaining;
  FieldGridDimension rowsRemaining;
  FieldGridDimension gridWidth;
  FieldGridCell *cell;
  FieldGridCell *rowFirstCell;
  Q12 cellWorldXQ12;
  Q12 cellWorldYQ12;
  uint32_t phaseSeedBitWidth;

  phaseSeedBitWidth = *(uint32_t *)((uint8_t *)g_TerrainSurfacePacketTablePayload - TERRAIN_PACKET_TABLE_HEADER_BYTES);
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
  cellsPerRow = fieldGrid->gridWidth;
  cell = fieldGrid->cells;
  for (rowsRemaining = fieldGrid->gridHeight; rowsRemaining != 0; rowsRemaining--) {
    for (columnsRemaining = cellsPerRow; columnsRemaining != 0; columnsRemaining--) {
      cellWorldXQ12 = cell->worldX;
      cellWorldYQ12 = cell->worldY;
      /* the edge flags are rebuilt from scratch, the debug mark starts cleared */
      cell->flagsAndMaterial =
           cell->flagsAndMaterial & ~(FIELD_CELL_GRID_EDGE_MASK | FIELD_CELL_DEBUG_MARKED);
      phaseRandomValue = Random_NextPrimary();
      cell->flagsAndMaterial = cell->flagsAndMaterial & ~FIELD_CELL_RANDOM_VARIANT_MASK;
      cell->surfacePacketIndex = phaseRandomValue & (1 << ((uint8_t)phaseSeedBitWidth & 31)) - 1U;
      /* 16x16 tiling of the 256 direction records over the world */
      /* 5f-format: FieldGridCell.persistedAux54 (direction record address in a 32-bit FLD field) */
      cell->persistedAux54 =
           Thandor_PointerToU32(
           g_TerrainDirectionRecordTable256 + (int32_t)(cellWorldYQ12 & 0xfU) + (int32_t)((cellWorldXQ12 & 0xfU) * 16));
      cell->armyRuntimeSavedOffset = 0;
      materialVariantRandomBits = Random_NextPrimary();
      cell->overlayColor = 0xffffffff; /* ARGB opaque white */
      cell->flagsAndMaterial =
           cell->flagsAndMaterial | materialVariantRandomBits & FIELD_CELL_RANDOM_VARIANT_MASK;
      cell++;
    }
  }
  gridWidth = fieldGrid->gridWidth;
  cell = fieldGrid->cells;
  for (columnsRemaining = gridWidth; columnsRemaining != 0; columnsRemaining--) {
    cell->flagsAndMaterial = cell->flagsAndMaterial | FIELD_CELL_FIRST_ROW_BOUNDARY;
    cell++;
  }
  rowFirstCell = fieldGrid->cells;
  for (rowsRemaining = fieldGrid->gridHeight; rowsRemaining != 0; rowsRemaining--) {
    rowFirstCell->flagsAndMaterial = rowFirstCell->flagsAndMaterial | FIELD_CELL_FIRST_COLUMN_BOUNDARY;
    rowFirstCell[gridWidth - 1].flagsAndMaterial =
         rowFirstCell[gridWidth - 1].flagsAndMaterial | FIELD_CELL_LAST_COLUMN_BOUNDARY;
    rowFirstCell = rowFirstCell + gridWidth;
  }
  /* the row cursor ran one row past the grid: step back to the first cell of the last row */
  cell = rowFirstCell - gridWidth;
  for (columnsRemaining = gridWidth; columnsRemaining != 0; columnsRemaining--) {
    cell->flagsAndMaterial = cell->flagsAndMaterial | FIELD_CELL_LAST_ROW_BOUNDARY;
    cell++;
  }
}

/* Marks the field surface dirty and re-binds every cell's terrain direction record (persistedAux54) from the low nibbles
   of its world X/Y, the same 16x16 tiling FieldGrid_InitializeRuntimeCellsAndBoundaryFlags uses. The secondary
   terrain resource load calls this instead of the full initialization, so the other cell state is kept.
*/
void FieldGrid_RebuildCellLookupPointers(FieldGridAsset *fieldGrid)

{
  FieldGridDimension columnsRemaining;
  FieldGridDimension rowsRemaining;
  FieldGridCell *currentCell;
  FieldGridDimension gridWidth;

  rowsRemaining = fieldGrid->gridHeight;
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
  gridWidth = fieldGrid->gridWidth;
  currentCell = fieldGrid->cells;
  columnsRemaining = gridWidth;
  do {
    do {
      /* 5f-format: FieldGridCell.persistedAux54 */
      currentCell->persistedAux54 =
           Thandor_PointerToU32(
           g_TerrainDirectionRecordTable256 +
           (int32_t)(currentCell->worldY & 0xfU) + (int32_t)((currentCell->worldX & 0xfU) * 16));
      currentCell++;
      columnsRemaining--;
    } while (columnsRemaining != 0);
    rowsRemaining--;
    columnsRemaining = gridWidth;
  } while (rowsRemaining != 0);
}

/* Clears the debug mark (FIELD_CELL_DEBUG_MARKED, bit 15) in every cell of the grid. No caller
   found in src/ or the image tables, and no code in the game sets the mark.
*/
void FieldGrid_ClearDebugMarkInAllCells(FieldGridAsset *fieldGrid)

{
  int cellsRemaining;
  FieldGridCell *currentCell;
  
  cellsRemaining = fieldGrid->gridWidth * fieldGrid->gridHeight;
  currentCell = fieldGrid->cells;
  do {
    currentCell->flagsAndMaterial = currentCell->flagsAndMaterial & ~FIELD_CELL_DEBUG_MARKED;
    currentCell++;
    cellsRemaining--;
  } while (cellsRemaining != 0);
}

/* Sets the overlay colour (ARGB, FieldGridCell.overlayColor) of every field-grid cell to one value.
*/
void FieldGrid_SetAllCellOverlayColors(PackedArgb32 argbColor,FieldGridAsset *fieldGrid)

{
  int cellsRemaining;
  FieldGridCell *currentCell;

  cellsRemaining = fieldGrid->gridWidth * fieldGrid->gridHeight;
  currentCell = fieldGrid->cells;
  do {
    currentCell->overlayColor = argbColor;
    currentCell++;
    cellsRemaining--;
  } while (cellsRemaining != 0);
}

/* Map editor save of the field grid: copies the loaded asset image (its size is the second dword) into a
   temporary block, resets the runtime-only cell state (normals to straight up, derived dwords and occupancy
   to 0, runtime flag bits cleared), rebuilds fieldFlags as the set of used material ids and writes the block to
   g_LevelResourcePathScratchUtf16. Returns true on success (the temporary block is freed; the free's own
   status is ignored), or false with the allocation or write error in *outError. Called by
   InGameUiCommand_SaveFieldAndLevelAssetImages.
*/
Bool8 FieldGrid_SaveAssetImageFromRuntimeState(uint32_t *sourceImageDwords,uint32_t *outError)

{
  FieldGridAsset *fieldGridImageCopy;
  uint32_t imageSizeBytes;
  uint32_t dwordsLeft;
  int cellsRemaining;
  int occupancyBytesLeft;
  uint32_t *copyDestinationDwords;
  FieldGridCellSaveImageView *fieldGridCellSaveView;
  uint8_t *occupancyBytes;
  uint32_t allocError;
  uint32_t writeError;

  imageSizeBytes = sourceImageDwords[1];
  allocError = g_MemoryApi.alloc(imageSizeBytes,(void **)&fieldGridImageCopy);
  if (allocError != 0) {
    *outError = allocError;
    return false;
  }
  copyDestinationDwords = (uint32_t *)fieldGridImageCopy;
  for (dwordsLeft = imageSizeBytes >> 2; dwordsLeft != 0; dwordsLeft--) {
    *copyDestinationDwords = *sourceImageDwords;
    sourceImageDwords++;
    copyDestinationDwords++;
  }
  fieldGridCellSaveView = (FieldGridCellSaveImageView *)fieldGridImageCopy->cells;
  fieldGridImageCopy->fieldFlags = 0;
  cellsRemaining = fieldGridImageCopy->gridWidth * fieldGridImageCopy->gridHeight;
  do {
    fieldGridCellSaveView->surfacePacketIndex = 0;
    fieldGridCellSaveView->triangle0NormalAngles = FIXED_ANGLE16_QUARTER_TURN << 16; /* elevation: straight up */
    fieldGridCellSaveView->groundScreenX = 0;
    fieldGridCellSaveView->groundScreenY = 0;
    fieldGridCellSaveView->groundViewX = 0;
    fieldGridCellSaveView->groundViewY = 0;
    fieldGridCellSaveView->groundViewZ = 0;
    fieldGridCellSaveView->secondarySurfaceScreenX = 0;
    fieldGridCellSaveView->secondarySurfaceScreenY = 0;
    fieldGridCellSaveView->secondarySurfaceViewX = 0;
    fieldGridCellSaveView->secondarySurfaceViewY = 0;
    fieldGridCellSaveView->secondarySurfaceViewZ = 0;
    /* keep the material byte, the resource-support, map-edge and fluid-exclusion bits (0xe80078ff) */
    fieldGridCellSaveView->flagsAndMaterial =
         fieldGridCellSaveView->flagsAndMaterial &
         (FIELD_CELL_MATERIAL_ID_MASK | FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK | FIELD_CELL_GRID_EDGE_MASK |
          FIELD_CELL_FLUID_RECEIVER_EXCLUDED | FIELD_CELL_FLUID_SOURCE_EXCLUDED);
    fieldGridCellSaveView->persistedAux54 = 0;
    fieldGridCellSaveView->groundDirectionalLightColor = 0;
    fieldGridCellSaveView->secondarySurfaceDirectionalLightColor = 0;
    fieldGridCellSaveView->shadedGroundColor = 0;
    fieldGridCellSaveView->shadedSecondarySurfaceColor = 0;
    fieldGridCellSaveView->visibilityLightingIndex = 0;
    /* one bit per material id in use */
    fieldGridImageCopy->fieldFlags =
         fieldGridImageCopy->fieldFlags |
         1 << ((uint8_t)fieldGridCellSaveView->flagsAndMaterial & 31);
    occupancyBytes = (uint8_t *)&fieldGridCellSaveView->occupancyMask;
    for (occupancyBytesLeft = 8; occupancyBytesLeft != 0; occupancyBytesLeft--) {
      *occupancyBytes = 0;
      occupancyBytes++;
    }
    fieldGridCellSaveView++;
    cellsRemaining--;
  } while (cellsRemaining != 0);
  writeError = FileSystem_WriteBufferToPath
                    ((fieldGridImageCopy->common).allocationSizeBytes,fieldGridImageCopy,
                     g_LevelResourcePathScratchUtf16);
  g_MemoryApi.free(fieldGridImageCopy);
  if (writeError != 0) {
    *outError = writeError;
    return false;
  }
  return true;
}

/* Checks a field grid image right after it was loaded (level file or the host's level transfer) or before the
   terrain loader uses it; loadedByteCount bytes are valid at fieldGrid. The original trusts the image; bounded
   here because the dimensions come from level files and from the network: the header must have been loaded,
   the asset's own size (common.allocationSizeBytes, which the savegame and the level transfer copy) must not
   exceed loadedByteCount, both sides need at least FIELD_GRID_MIN_SIDE_CELLS cells (the map-edge ring that
   stops the cell walkers, around an interior), and header + width * height cells must fit the asset size.
   Every stock grid passes: all are exactly header + cells long, with 8k + 3 (59..139) cells per side. Logs
   one line and returns false otherwise.
*/
Bool8 FieldGrid_ValidateLoadedImage(const FieldGridAsset *fieldGrid,uint32_t loadedByteCount)

{
  uint64_t requiredBytes;

  if (loadedByteCount < FIELD_GRID_HEADER_BYTES) {
    Thandor_Log("FieldGrid_ValidateLoadedImage: rejected field grid of %u bytes (no header)",loadedByteCount);
    return false;
  }
  requiredBytes = FIELD_GRID_HEADER_BYTES +
                  (uint64_t)fieldGrid->gridWidth * fieldGrid->gridHeight * sizeof(FieldGridCell);
  if (((fieldGrid->common).allocationSizeBytes > loadedByteCount) ||
      (fieldGrid->gridWidth < FIELD_GRID_MIN_SIDE_CELLS) || (fieldGrid->gridHeight < FIELD_GRID_MIN_SIDE_CELLS) ||
      (requiredBytes > (fieldGrid->common).allocationSizeBytes)) {
    Thandor_Log("FieldGrid_ValidateLoadedImage: rejected field grid %ux%u (asset size %u, loaded %u bytes)",
                fieldGrid->gridWidth,fieldGrid->gridHeight,(fieldGrid->common).allocationSizeBytes,
                loadedByteCount);
    return false;
  }
  return true;
}

/* Package_LoadEntry for a field grid (.fld): additionally validates the loaded image
   (FieldGrid_ValidateLoadedImage). A rejected image is freed and reported like a failed load, with
   FATAL_ERROR_FIELD_ASSET_INVALID in *outErrorCode. Valid grids load exactly as with Package_LoadEntry.
*/
FieldGridAsset *FieldGrid_LoadValidated(uint16_t *path,uint32_t *outErrorCode)

{
  void *loadedEntry;
  uint32_t loadedByteCount;
  uint32_t loadErrorCode;

  loadedEntry = Package_LoadEntryWithSize(path,&loadedByteCount,&loadErrorCode);
  if (loadedEntry == NULL) {
    Thandor_Log("FieldGrid_LoadValidated failed: \"%ls\" (error 0x%08X)",(wchar_t *)path,loadErrorCode);
    if (outErrorCode != NULL) {
      *outErrorCode = loadErrorCode;
    }
    return NULL;
  }
  if (!FieldGrid_ValidateLoadedImage((FieldGridAsset *)loadedEntry,loadedByteCount)) {
    g_MemoryApi.free(loadedEntry);
    if (outErrorCode != NULL) {
      *outErrorCode = FATAL_ERROR_FIELD_ASSET_INVALID;
    }
    return NULL;
  }
  return (FieldGridAsset *)loadedEntry;
}
