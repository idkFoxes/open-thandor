/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/terrain/occupancy.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_TERRAIN_OCCUPANCY_H
#define THANDOR_WORLD_TERRAIN_OCCUPANCY_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/terrain/occupancy. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint packs two bits per faction slot i: bit 2i+1 = present now,
   bit 2i = only the persistent occupancy bit (seen before). */
#define TERRAIN_OCCUPANCY_CLASS_PRESENT_BITS 0xaaaaaaaa
/* Model-node runtimeFlags bits read and produced by TerrainOccupancyMask_ResolveRuntimeClassFlags (callers clear
   0xC first; the selection overlay accepts PRESENT, or SEEN_BEFORE without NOT_REMEMBERED). */
#define TERRAIN_OCCUPANCY_FLAG_PRESENT 0x04
#define TERRAIN_OCCUPANCY_FLAG_SEEN_BEFORE 0x08
#define TERRAIN_OCCUPANCY_FLAG_NOT_REMEMBERED 0x10 /* set on shots and effects */
/* Rounding bias added to a Q12 radius before dividing by FIELD_GRID_WORLD_COLUMN_STEP_X (just under half a Q12
   unit, not half the cell step) */
#define TERRAIN_OCCUPANCY_RADIUS_ROUND_Q12 0x7ffU

/* 0x00507460 */
void TerrainOccupancyBit2_MarkAroundWorldPoint(FieldGridRadiusUnits radiusWorldUnits,Q12 worldYQ12,Q12 worldXQ12,
          FieldGridOccupancyByteIndex occupancyByteOffset,FieldGridAsset *fieldGrid);

/* 0x00507610 */
uint32_t TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
          (Q12 neighborhoodRadiusQ12,Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid);

/* 0x005138F0 */
TerrainOccupancyResolvedMasks
TerrainOccupancyMask_ResolveRuntimeClassFlags
          (FieldGridRuntimeFlags baseRuntimeFlags,FieldGridRegionMask secondaryOccupancyMask,
          FieldGridRegionMask primaryOccupancyMask,char activeFactionIndex);

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
