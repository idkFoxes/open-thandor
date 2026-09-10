#ifndef THANDOR_WORLD_TERRAIN_EDITING_H
#define THANDOR_WORLD_TERRAIN_EDITING_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/terrain/editing. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x005137F0 */
undefined8 __fastcall TerrainRegionCollection_CollectConnectedCellsRecursive (undefined4 incomingEcxValue,undefined4 preservedEdxValue, FieldGridRegionMask requiredOccupancyMask,FieldGridRowStrideBytes rowStrideBytes, FieldGridCell *cell);

/* 0x00561A10 */
void TerrainMaterialEdit_SeedMatchingRegionReplacement (FrontendPlayerIndex playerIndex,TerrainMaterialByteValue replacementMaterialByte, Q12 worldYQ12,Q12 worldXQ12);

/* 0x00561AE0 */
void TerrainMaterialEdit_SeedNonTargetRegionReplacement (FrontendPlayerIndex playerIndex,TerrainMaterialByteValue referenceMaterialByte, Q12 worldYQ12,Q12 worldXQ12);

/* 0x005616D0 */
void TerrainEditBuffer_CommitHeightDeltasAndRefreshLighting (dword commandArg0,dword commandArg1,dword commandArg2,dword commandArg3);

/* 0x00561830 */
void __fastcall TerrainEditBuffer_CopyCellMaterialBytes (undefined4 param_1,undefined4 param_2,FrontendPlayerIndex playerIndex);

/* 0x00561930 */
void __fastcall TerrainEditBuffer_SubtractCurrentCellMaterialBytes (undefined4 param_1,undefined4 param_2,FrontendPlayerIndex playerIndex);

/* 0x005619A0 */
void TerrainEditBuffer_CommitFlagsAndMaterialDeltas (dword commandArg0,dword commandArg1,dword commandArg2,dword commandArg3);

/* 0x00561DC0 */
void __fastcall TerrainEditBuffer_ConvertHeightsToDeltas (undefined4 param_1,undefined4 param_2,FrontendPlayerIndex playerIndex);

/* 0x00513790 */
void TerrainRegionCollection_RecordConnectedCell (FieldGridRegionMask requiredOccupancyMask,FieldGridCell *cell);

/* 0x00571600 */
void TerrainMaterialEdit_PropagateMatchingRegionReplacement (FieldGridCellCoordinate gridY,FieldGridCellCoordinate gridX);

/* 0x00571730 */
void TerrainMaterialEdit_PropagateNonTargetRegionReplacement (FieldGridCellCoordinate gridY,FieldGridCellCoordinate gridX);

#endif /* THANDOR_WORLD_TERRAIN_EDITING_H */
