/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/terrain/fog_visibility.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_TERRAIN_FOG_VISIBILITY_H
#define THANDOR_WORLD_TERRAIN_FOG_VISIBILITY_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* g_TerrainByteClampLookup (TerrainByteClampLookup_Initialize): 256 rows of 256 bytes, row = a cell's
   occupancy byte, column = its runtime byte visibilityLightingIndex. Each row moves the runtime byte by one fade step towards the
   row's target level (the levels FieldGrid_ClassifyCellFlagsToRuntimeByte writes directly): rows 0x00..0x7F
   by bit 0 (clear -> NONE, set -> FULL), 0x80 and 0x82 -> PERSISTENT, 0x81 and 0x83..0xFF -> FULL. */
#define TERRAIN_BYTE_CLAMP_LOOKUP_BYTES 0x10000
#define TERRAIN_RUNTIME_BYTE_FADE_STEP 0x15
#define TERRAIN_RUNTIME_BYTE_LEVEL_NONE 0x00
#define TERRAIN_RUNTIME_BYTE_LEVEL_PERSISTENT 0x87 /* only the persistent occupancy bit 7 */
#define TERRAIN_RUNTIME_BYTE_LEVEL_FULL 0xff

extern uint8_t *g_TerrainByteClampLookup;

void FieldGrid_ApplyByteClampLookupToCells(FieldGridByteOffset factionIndex,FieldGridAsset *fieldGrid);

void FieldGrid_ClassifyCellFlagsToRuntimeByte(FieldGridByteOffset factionSlot,FieldGridAsset *fieldGrid);

Bool8 TerrainByteClampLookup_Initialize(uint32_t *outError);

#endif /* THANDOR_WORLD_TERRAIN_FOG_VISIBILITY_H */
