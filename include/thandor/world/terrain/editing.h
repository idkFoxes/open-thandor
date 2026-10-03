/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/terrain/editing.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_TERRAIN_EDITING_H
#define THANDOR_WORLD_TERRAIN_EDITING_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/terrain/editing. */

/* Cells that stop TerrainRegionCollection_CollectConnectedCellsRecursive: the map-edge ring and cells already
   collected (FieldGridCell.flagsAndMaterial; = 0x88016000). */
#define TERRAIN_REGION_STOP_FLAGS (FIELD_CELL_GRID_EDGE_MASK | FIELD_CELL_CONNECTED_REGION_VISITED)
/* Capacity of g_TerrainRegionCollectionEntries (8-byte records: extraction descriptor, model offset); further
   extractors of a region are cleared but not recorded (TerrainRegionCollection_RecordConnectedCell). */
#define TERRAIN_REGION_COLLECTION_CAPACITY 2048
/* Bytes before the FieldGridAsset of the dword the material edit sets its dirty bit in (original quirk; the
   intended target is FieldGridAsset.runtimeStateFlags) */
#define TERRAIN_EDIT_STRAY_DIRTY_FLAG_BACK_OFFSET 0x14c

/* Functions are grouped by semantic ownership. */

void TerrainRegionCollection_CollectConnectedCellsRecursive
          (FieldGridRegionMask requiredCellFlags,FieldGridRowStrideBytes rowStrideBytes,
          FieldGridCell *cell);

void TerrainMaterialEdit_SeedMatchingRegionReplacement
          (FrontendPlayerIndex playerIndex,TerrainMaterialByteValue replacementMaterialByte,
          Q12 worldYQ12,Q12 worldXQ12);

void TerrainMaterialEdit_SeedNonTargetRegionReplacement
          (FrontendPlayerIndex playerIndex,TerrainMaterialByteValue referenceMaterialByte,
          Q12 worldYQ12,Q12 worldXQ12);

void TerrainEditBuffer_CommitHeightDeltasAndRefreshLighting
          (uint32_t playerRuntimeId,uint32_t unusedCommandValue1,uint32_t unusedCommandValue2,
          uint32_t unusedCommandValue3);

void TerrainEditBuffer_CopyCellMaterialBytes
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedZero0,uint32_t reservedZero1,
          uint32_t reservedZero2);

void TerrainEditBuffer_SubtractCurrentCellMaterialBytes
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedZero0,uint32_t reservedZero1,
          uint32_t reservedZero2);

void TerrainEditBuffer_CommitFlagsAndMaterialDeltas
          (uint32_t playerRuntimeId,uint32_t unusedCommandValue1,uint32_t unusedCommandValue2,
          uint32_t unusedCommandValue3);

void TerrainEditBuffer_ConvertHeightsToDeltas
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedZero0,uint32_t reservedZero1,
          uint32_t reservedZero2);

void TerrainRegionCollection_RecordConnectedCell(FieldGridRegionMask requiredOccupancyMask,FieldGridCell *cell);

void TerrainMaterialEdit_PropagateMatchingRegionReplacement
          (FieldGridCellCoordinate gridY,FieldGridCellCoordinate gridX);

void TerrainMaterialEdit_PropagateNonTargetRegionReplacement
          (FieldGridCellCoordinate gridY,FieldGridCellCoordinate gridX);

#endif /* THANDOR_WORLD_TERRAIN_EDITING_H */
