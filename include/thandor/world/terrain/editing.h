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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x005137F0 */
void __thandor_void_preserve_eax_ecx_edx
TerrainRegionCollection_CollectConnectedCellsRecursive
          (FieldGridRegionMask requiredOccupancyMask,FieldGridRowStrideBytes rowStrideBytes,
          FieldGridCell *cell);

/* 0x00561A10 */
void __thandor_void_preserve_eax_ecx_edx
TerrainMaterialEdit_SeedMatchingRegionReplacement
          (FrontendPlayerIndex playerIndex,TerrainMaterialByteValue replacementMaterialByte,
          Q12 worldYQ12,Q12 worldXQ12);

/* 0x00561AE0 */
void __thandor_void_preserve_eax_ecx_edx
TerrainMaterialEdit_SeedNonTargetRegionReplacement
          (FrontendPlayerIndex playerIndex,TerrainMaterialByteValue referenceMaterialByte,
          Q12 worldYQ12,Q12 worldXQ12);

/* 0x005616D0 */
void __thandor_void_preserve_eax_ecx_edx
TerrainEditBuffer_CommitHeightDeltasAndRefreshLighting
          (dword commandArg0,dword commandArg1,dword commandArg2,dword commandArg3);

/* 0x00561830 */
void __thandor_void_preserve_eax_ecx_edx
TerrainEditBuffer_CopyCellMaterialBytes
          (PlayerRuntimeId playerRuntimeId,dword reservedZero0,dword reservedZero1,
          dword reservedZero2);

/* 0x00561930 */
void __thandor_void_preserve_eax_ecx_edx
TerrainEditBuffer_SubtractCurrentCellMaterialBytes
          (PlayerRuntimeId playerRuntimeId,dword reservedZero0,dword reservedZero1,
          dword reservedZero2);

/* 0x005619A0 */
void __thandor_void_preserve_eax_ecx_edx
TerrainEditBuffer_CommitFlagsAndMaterialDeltas
          (dword commandArg0,dword commandArg1,dword commandArg2,dword commandArg3);

/* 0x00561DC0 */
void __thandor_void_preserve_eax_ecx_edx
TerrainEditBuffer_ConvertHeightsToDeltas
          (PlayerRuntimeId playerRuntimeId,dword reservedZero0,dword reservedZero1,
          dword reservedZero2);

/* 0x00513790 */
void __thandor_void_preserve_eax_ecx_edx
TerrainRegionCollection_RecordConnectedCell
          (FieldGridRegionMask requiredOccupancyMask,FieldGridCell *cell);

/* 0x00571600 */
void __thandor_eax_preserve_ecx_edx
TerrainMaterialEdit_PropagateMatchingRegionReplacement
          (FieldGridCellCoordinate gridY,FieldGridCellCoordinate gridX);

/* 0x00571730 */
void __thandor_eax_preserve_ecx_edx
TerrainMaterialEdit_PropagateNonTargetRegionReplacement
          (FieldGridCellCoordinate gridY,FieldGridCellCoordinate gridX);

#endif /* THANDOR_WORLD_TERRAIN_EDITING_H */
