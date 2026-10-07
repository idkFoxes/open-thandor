/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/terrain/field_lifecycle.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_TERRAIN_FIELD_LIFECYCLE_H
#define THANDOR_WORLD_TERRAIN_FIELD_LIFECYCLE_H

#include <thandor/core/types.h>
#include <thandor/world/terrain/types.h>
#include <thandor/core/contracts.h>

void FieldGrid_InitializeRuntimeCellsAndBoundaryFlags(FieldGridAsset *fieldGrid);

void FieldGrid_RebuildCellLookupPointers(FieldGridAsset *fieldGrid);

void FieldGrid_SetAllCellOverlayColors(PackedArgb32 argbColor,FieldGridAsset *fieldGrid);

Bool8 FieldGrid_SaveAssetImageFromRuntimeState(uint32_t *sourceImageDwords,uint32_t *outError);

/* Smallest accepted field grid side in cells (FieldGrid_ValidateLoadedImage): the map-edge ring plus an
   interior; tools/data/fld.py uses the same bound. */
inline constexpr int FIELD_GRID_MIN_SIDE_CELLS = 4;

Bool8 FieldGrid_ValidateLoadedImage(const FieldGridAsset *fieldGrid,uint32_t loadedByteCount);

FieldGridAsset *FieldGrid_LoadValidated(uint16_t *path,uint32_t *outErrorCode);

#endif /* THANDOR_WORLD_TERRAIN_FIELD_LIFECYCLE_H */
