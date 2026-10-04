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

/* g_TerrainByteClampLookup (TerrainByteClampLookup_Initialize): 256 rows of 256 bytes, row = a cell's
   occupancy byte, column = its runtime byte visibilityLightingIndex. Each row moves the runtime byte by one fade step towards the
   row's target level (the levels FieldGrid_ClassifyCellFlagsToRuntimeByte writes directly): rows 0x00..0x7F
   by bit 0 (clear -> NONE, set -> FULL), 0x80 and 0x82 -> PERSISTENT, 0x81 and 0x83..0xFF -> FULL. */
#define TERRAIN_BYTE_CLAMP_LOOKUP_BYTES 0x10000
#define TERRAIN_RUNTIME_BYTE_FADE_STEP 0x15
#define TERRAIN_RUNTIME_BYTE_LEVEL_NONE 0x00
#define TERRAIN_RUNTIME_BYTE_LEVEL_PERSISTENT 0x87 /* only the persistent occupancy bit 7 */
#define TERRAIN_RUNTIME_BYTE_LEVEL_FULL 0xff

/* Submodule: world/terrain/occupancy. */
/* Functions are grouped by semantic ownership. */

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

void TerrainOccupancyBit2_MarkAroundWorldPoint(FieldGridRadiusUnits radiusWorldUnits,Q12 worldYQ12,Q12 worldXQ12,
          FieldGridOccupancyByteIndex occupancyByteOffset,FieldGridAsset *fieldGrid);

uint32_t TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
          (Q12 neighborhoodRadiusQ12,Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid);

TerrainOccupancyResolvedMasks
TerrainOccupancyMask_ResolveRuntimeClassFlags
          (FieldGridRuntimeFlags baseRuntimeFlags,FieldGridRegionMask secondaryOccupancyMask,
          FieldGridRegionMask primaryOccupancyMask,char activeFactionIndex);

void TerrainOccupancyBit2_MarkWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

void TerrainOccupancyBit2_MarkWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

void TerrainOccupancyBit2_MarkWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

void TerrainOccupancyBit2_MarkWedge3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

void TerrainOccupancyBit2_MarkWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

void TerrainOccupancyBit2_MarkWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

void TerrainOccupancyBit2_MarkDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

void TerrainOccupancyBit2_MarkDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

void TerrainOccupancyBit2_MarkDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

void TerrainOccupancyBit2_MarkDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

void TerrainOccupancyBit2_MarkDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

void TerrainOccupancyBit2_MarkDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

void FieldGrid_ApplyByteClampLookupToCells(FieldGridByteOffset factionIndex,FieldGridAsset *fieldGrid);

void FieldGrid_ClassifyCellFlagsToRuntimeByte(FieldGridByteOffset factionSlot,FieldGridAsset *fieldGrid);

void FieldGrid_ClearOccupancyMaskBits0To6AllCells(FieldGridAsset *fieldGrid);

void FieldGrid_SetOccupancyMaskByteBit0AllCells
          (FieldGridOccupancyByteIndex occupancyMaskByteIndex,FieldGridAsset *fieldGrid);

void FieldGrid_ClearOccupancyMaskByteBit0AllCells
          (FieldGridOccupancyByteIndex occupancyMaskByteIndex,FieldGridAsset *fieldGrid);

Bool8 TerrainGrid_TestProjectedCellMaskBits01(Q12 worldYQ12,Q12 worldXQ12,WorldRuntimeContext *worldRuntime);

extern uint8_t *g_TerrainByteClampLookup;

Bool8 TerrainByteClampLookup_Initialize(uint32_t *outError);

#endif /* THANDOR_WORLD_TERRAIN_OCCUPANCY_H */
