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

/* 0x00507460 */
void __thandor_void_preserve_eax_ecx_edx
TerrainOccupancyBit2_MarkAroundWorldPoint
          (FieldGridRadiusUnits radiusWorldUnits,Q12 worldXQ12,Q12 worldYQ12,
          FieldGridOccupancyByteIndex occupancyByteOffset,FieldGridAsset *fieldGrid);

/* 0x00507610 */
dword __thandor_void_preserve_eax_ecx
TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
          (Q12 neighborhoodRadiusQ12,Q12 worldXQ12,Q12 worldYQ12,FieldGridAsset *fieldGrid);

/* 0x005138F0 */
TerrainOccupancyResolvedMasksRegs12
TerrainOccupancyMask_ResolveRuntimeClassFlags
          (FieldGridRuntimeFlags baseRuntimeFlags,FieldGridRegionMask secondaryOccupancyMask,
          FieldGridRegionMask primaryOccupancyMask,char runtimeClassIndex);

/* 0x005070A0 */
void __thandor_void_preserve_eax_ecx_edx
TerrainOccupancyBit2_MarkWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00507140 */
void __thandor_void_preserve_eax_ecx_edx
TerrainOccupancyBit2_MarkWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005071E0 */
void __thandor_void_preserve_eax_ecx_edx
TerrainOccupancyBit2_MarkWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00507280 */
void __thandor_void_preserve_eax_ecx_edx
TerrainOccupancyBit2_MarkWedge3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00507320 */
void __thandor_void_preserve_eax_ecx_edx
TerrainOccupancyBit2_MarkWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005073C0 */
void __thandor_void_preserve_eax_ecx_edx
TerrainOccupancyBit2_MarkWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00506EA0 */
void __thandor_void_preserve_eax_ecx_edx
TerrainOccupancyBit2_MarkDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00506EF0 */
void __thandor_void_preserve_eax_ecx_edx
TerrainOccupancyBit2_MarkDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00506F50 */
void __thandor_void_preserve_eax_ecx_edx
TerrainOccupancyBit2_MarkDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00506FA0 */
void __thandor_void_preserve_eax_ecx_edx
TerrainOccupancyBit2_MarkDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00506FF0 */
void __thandor_void_preserve_eax_ecx_edx
TerrainOccupancyBit2_MarkDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00507050 */
void __thandor_void_preserve_eax_ecx_edx
TerrainOccupancyBit2_MarkDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

#endif /* THANDOR_WORLD_TERRAIN_OCCUPANCY_H */
