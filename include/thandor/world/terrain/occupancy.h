#ifndef THANDOR_WORLD_TERRAIN_OCCUPANCY_H
#define THANDOR_WORLD_TERRAIN_OCCUPANCY_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/terrain/occupancy. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00507460 */
void TerrainOccupancyBit2_MarkAroundWorldPoint (FieldGridRadiusUnits radiusWorldUnits,Q12 worldXQ12,Q12 worldYQ12, FieldGridOccupancyByteIndex occupancyByteOffset,FieldGridAsset *fieldGrid);

/* 0x00507610 */
ulonglong TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint (int param_1,Q12 worldXQ12,Q12 worldYQ12,FieldGridAsset *fieldGrid);

/* 0x005138F0 */
FieldGridRuntimeFlags TerrainOccupancyMask_ResolveRuntimeClassFlags (FieldGridRuntimeFlags baseRuntimeFlags,FieldGridRegionMask secondaryOccupancyMask, FieldGridRegionMask primaryOccupancyMask,char runtimeClassIndex);

/* 0x005070A0 */
void TerrainOccupancyBit2_MarkWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00507140 */
void TerrainOccupancyBit2_MarkWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005071E0 */
void TerrainOccupancyBit2_MarkWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00507280 */
void TerrainOccupancyBit2_MarkWedge3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00507320 */
void TerrainOccupancyBit2_MarkWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005073C0 */
void TerrainOccupancyBit2_MarkWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00506EA0 */
void TerrainOccupancyBit2_MarkDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00506EF0 */
void TerrainOccupancyBit2_MarkDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00506F50 */
void TerrainOccupancyBit2_MarkDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00506FA0 */
void TerrainOccupancyBit2_MarkDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00506FF0 */
void TerrainOccupancyBit2_MarkDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00507050 */
void TerrainOccupancyBit2_MarkDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

#endif /* THANDOR_WORLD_TERRAIN_OCCUPANCY_H */
