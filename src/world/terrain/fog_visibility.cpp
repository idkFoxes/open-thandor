/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/fog_visibility.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* Fog of war per cell: the visibility lighting index classified from a faction's occupancy byte, and its
   fade towards the target level through the 256x256 byte clamp table. */

#include <thandor/world/terrain/fog_visibility.h>
#include <thandor/thandor.h>

/* Module data. */

uint8_t *g_TerrainByteClampLookup = nullptr;

/* For every field cell, maps the fog-of-war lighting index (visibilityLightingIndex) through the
   256x256 terrain clamp lookup, keyed by the cell's occupancy byte of the given faction, and writes the result
   back. Runs on tick-wheel cases 3 and 7 after the per-class terrain-state refresh callbacks.
*/
void FieldGrid_ApplyByteClampLookupToCells(FieldGridByteOffset factionIndex,FieldGridAsset *fieldGrid)

{
  uint8_t *clampLookup;
  FieldGridDimension columnsRemaining;
  FieldGridDimension rowsRemaining;
  FieldGridCell *currentCell;
  uint8_t mappedLightingIndex;
  FieldGridDimension gridWidth;

  /* for loops instead of the original's do-while (0 width or height: no pass instead of 2^32) */
  gridWidth = fieldGrid->gridWidth;
  currentCell = fieldGrid->cells;
  clampLookup = g_TerrainByteClampLookup;
  for (rowsRemaining = fieldGrid->gridHeight; rowsRemaining != 0; rowsRemaining--) {
    for (columnsRemaining = gridWidth; columnsRemaining != 0; columnsRemaining--) {
      /* The lookup is 64-KiB aligned: the original puts the occupancy byte and the lighting index into the
         pointer's low word, i.e. indexes the table with (occupancy << 8) | lighting index. */
      mappedLightingIndex =
           clampLookup[(uint32_t)FieldGridCell_OccupancyByte(currentCell,factionIndex) << 8 |
                       (uint32_t)currentCell->visibilityLightingIndex];
      currentCell->visibilityLightingIndex = mappedLightingIndex;
      currentCell = currentCell + 1;
    }
  }
}

/* Rebuilds every cell's fog-of-war lighting index (visibilityLightingIndex) from the occupancy byte of
   one faction slot (usually the active faction): FIELD_CELL_LIGHTING_VISIBLE when a current presence bit
   (FIELD_CELL_OCCUPANCY_CURRENT_PRESENCE_BITS) is set, FIELD_CELL_LIGHTING_EXPLORED when only the persistent
   bit 7 is, FIELD_CELL_LIGHTING_UNEXPLORED otherwise.
*/
void FieldGrid_ClassifyCellFlagsToRuntimeByte(FieldGridByteOffset factionSlot,FieldGridAsset *fieldGrid)

{
  uint8_t lightingIndex;
  uint8_t occupancyByte;
  uint32_t cellsRemaining;
  FieldGridCell *currentCell;

  /* for loop instead of the original's do-while (0 cells: no pass instead of 2^32) */
  currentCell = fieldGrid->cells;
  for (cellsRemaining = fieldGrid->gridWidth * fieldGrid->gridHeight; cellsRemaining != 0; cellsRemaining--) {
    occupancyByte = FieldGridCell_OccupancyByte(currentCell,factionSlot);
    if ((occupancyByte & FIELD_CELL_OCCUPANCY_CURRENT_PRESENCE_BITS) != 0) {
      lightingIndex = FIELD_CELL_LIGHTING_VISIBLE;
    }
    else if ((occupancyByte & FIELD_CELL_OCCUPANCY_PERSISTENT_BIT) != 0) {
      lightingIndex = FIELD_CELL_LIGHTING_EXPLORED;
    }
    else {
      lightingIndex = FIELD_CELL_LIGHTING_UNEXPLORED;
    }
    currentCell->visibilityLightingIndex = lightingIndex;
    currentCell++;
  }
}

/* Fills one 256-byte row of g_TerrainByteClampLookup: for every runtime byte 0..0xFF the byte moved one
   TERRAIN_RUNTIME_BYTE_FADE_STEP towards targetLevel, stopping at targetLevel from either side. Returns the
   position after the row. */
static uint8_t *TerrainByteClampLookup_FillRow(uint8_t *rowCursor,int targetLevel)

{
  int inputValue;

  for (inputValue = 0; inputValue < 256; inputValue++) {
    if (inputValue <= targetLevel) {
      if (inputValue + TERRAIN_RUNTIME_BYTE_FADE_STEP < targetLevel) {
        *rowCursor = (uint8_t)(inputValue + TERRAIN_RUNTIME_BYTE_FADE_STEP);
      }
      else {
        *rowCursor = (uint8_t)targetLevel;
      }
    }
    else if (inputValue - TERRAIN_RUNTIME_BYTE_FADE_STEP <= targetLevel) {
      *rowCursor = (uint8_t)targetLevel;
    }
    else {
      *rowCursor = (uint8_t)(inputValue - TERRAIN_RUNTIME_BYTE_FADE_STEP);
    }
    rowCursor++;
  }
  return rowCursor;
}

/* Builds g_TerrainByteClampLookup, the 64-KiB table FieldGrid_ApplyByteClampLookupToCells uses every few ticks to
   fade each cell's runtime byte (visibilityLightingIndex) one step (TERRAIN_RUNTIME_BYTE_FADE_STEP) towards the
   level its occupancy byte asks for (row targets: see TERRAIN_BYTE_CLAMP_LOOKUP_BYTES). The table is 64-KiB
   aligned so the original can index it with the two bytes as the low 16 address bits. Returns true on success; false when the allocation fails,
   with the allocator error in *outError.
*/
bool TerrainByteClampLookup_Initialize(uint32_t *outError)

{
  void *lookupAllocationBase;
  int rowPair;
  int row;
  uint8_t *lookupWriteCursor;
  uint32_t allocError;

  /* twice the size, so a 64-KiB aligned table fits inside */
  allocError = g_MemoryApi.alloc(TERRAIN_BYTE_CLAMP_LOOKUP_BYTES * 2,&lookupAllocationBase);
  if (allocError != 0) {
    *outError = allocError;
    return false;
  }
  lookupWriteCursor = reinterpret_cast<uint8_t *>((reinterpret_cast<uintptr_t>(lookupAllocationBase) + 0xffff) & ~(uintptr_t)0xffff); /* the next 64-KiB boundary */
  g_TerrainByteClampLookup = lookupWriteCursor;
  /* rows 0x00..0x7F in pairs: even rows fade to NONE, odd rows to FULL */
  for (rowPair = 0; rowPair < 64; rowPair++) {
    lookupWriteCursor = TerrainByteClampLookup_FillRow(lookupWriteCursor,TERRAIN_RUNTIME_BYTE_LEVEL_NONE);
    lookupWriteCursor = TerrainByteClampLookup_FillRow(lookupWriteCursor,TERRAIN_RUNTIME_BYTE_LEVEL_FULL);
  }
  /* row 0x80: PERSISTENT, row 0x81: FULL, row 0x82: PERSISTENT */
  lookupWriteCursor = TerrainByteClampLookup_FillRow(lookupWriteCursor,TERRAIN_RUNTIME_BYTE_LEVEL_PERSISTENT);
  lookupWriteCursor = TerrainByteClampLookup_FillRow(lookupWriteCursor,TERRAIN_RUNTIME_BYTE_LEVEL_FULL);
  lookupWriteCursor = TerrainByteClampLookup_FillRow(lookupWriteCursor,TERRAIN_RUNTIME_BYTE_LEVEL_PERSISTENT);
  /* rows 0x83..0xFF: FULL */
  for (row = 0x83; row < 0x100; row++) {
    lookupWriteCursor = TerrainByteClampLookup_FillRow(lookupWriteCursor,TERRAIN_RUNTIME_BYTE_LEVEL_FULL);
  }
  return true;
}
