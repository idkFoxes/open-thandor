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

/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x005137F0 */
void TerrainRegionCollection_CollectConnectedCellsRecursive
          (FieldGridRegionMask requiredCellFlags,FieldGridRowStrideBytes rowStrideBytes,
          FieldGridCell *cell);

/* 0x00561A10 */
void TerrainMaterialEdit_SeedMatchingRegionReplacement
          (FrontendPlayerIndex playerIndex,TerrainMaterialByteValue replacementMaterialByte,
          Q12 worldYQ12,Q12 worldXQ12);

/* 0x00561AE0 */
void TerrainMaterialEdit_SeedNonTargetRegionReplacement
          (FrontendPlayerIndex playerIndex,TerrainMaterialByteValue referenceMaterialByte,
          Q12 worldYQ12,Q12 worldXQ12);

/* 0x005616D0 */
void TerrainEditBuffer_CommitHeightDeltasAndRefreshLighting
          (uint32_t playerRuntimeId,uint32_t unusedCommandValue1,uint32_t unusedCommandValue2,
          uint32_t unusedCommandValue3);

/* 0x00561830 */
void TerrainEditBuffer_CopyCellMaterialBytes
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedZero0,uint32_t reservedZero1,
          uint32_t reservedZero2);

/* 0x00561930 */
void TerrainEditBuffer_SubtractCurrentCellMaterialBytes
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedZero0,uint32_t reservedZero1,
          uint32_t reservedZero2);

/* 0x005619A0 */
void TerrainEditBuffer_CommitFlagsAndMaterialDeltas
          (uint32_t playerRuntimeId,uint32_t unusedCommandValue1,uint32_t unusedCommandValue2,
          uint32_t unusedCommandValue3);

/* 0x00561DC0 */
void TerrainEditBuffer_ConvertHeightsToDeltas
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedZero0,uint32_t reservedZero1,
          uint32_t reservedZero2);

/* 0x00513790 */
void TerrainRegionCollection_RecordConnectedCell(FieldGridRegionMask requiredOccupancyMask,FieldGridCell *cell);

/* 0x00571600 */
void TerrainMaterialEdit_PropagateMatchingRegionReplacement
          (FieldGridCellCoordinate gridY,FieldGridCellCoordinate gridX);

/* 0x00571730 */
void TerrainMaterialEdit_PropagateNonTargetRegionReplacement
          (FieldGridCellCoordinate gridY,FieldGridCellCoordinate gridX);

#endif /* THANDOR_WORLD_TERRAIN_EDITING_H */
