/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/terrain/height.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_TERRAIN_HEIGHT_H
#define THANDOR_WORLD_TERRAIN_HEIGHT_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/terrain/height. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00508000 */
bool __thandor_void_preserve_ecx_edx
TerrainHeightBand_TestAroundWorldPoint
          (FieldGridRadiusUnits radiusWorldUnits,Q12 referenceHeightQ12,Q12 worldXQ12,Q12 worldYQ12,
          FieldGridAsset *fieldGrid);

/* 0x00508920 */
bool __thandor_void_preserve_ecx_edx
TerrainAuxHeightThreshold_TestAroundWorldPoint
          (FieldGridRadiusUnits radiusWorldUnits,Q12 referenceHeightQ12,Q12 worldXQ12,Q12 worldYQ12,
          FieldGridAsset *fieldGrid);

/* 0x00508D20 */
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508DC0 */
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508E60 */
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508F00 */
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyWedge3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508FA0 */
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00509040 */
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00504520 */
TerrainDistanceEaxCf5 __thandor_eax_cf_preserve_ecx_edx
TerrainTriangle_IntersectRayDistanceCf
          (Q12 rayDeltaZQ12,Q12 gridRayDelta0Q12,Q12 gridRayDelta1Q12,Q12 rayOriginZQ12,
          Q12 cornerHeight0Q12,Q12 cornerHeight1Q12,Q12 cornerHeight2Q12,Q12 cornerHeight3Q12,
          Q12 cellLocalCoord1Q12,Q12 cellLocalCoord0Q12);

/* 0x005049E0 */
bool __thandor_cf_preserve_eax
TerrainRay_AdvanceGridTraversalCf
          (Q12 rayEndCoord0Q12,Q12 rayEndCoord1Q12,Q12 rayStartCoord0Q12,Q12 rayStartCoord1Q12,
          FieldGridRowStrideBytes rowStrideBytes,FieldGridCell *currentCell,Q12 currentGridCoord0Q12
          ,Q12 currentGridCoord1Q12);

/* 0x00507AB0 */
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00507BA0 */
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00507C80 */
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00507D60 */
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestWedge3(TerrainDirectionalScanStep scanStep,byte *cell);

/* 0x00507E40 */
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00507F20 */
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508470 */
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508540 */
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508600 */
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005086C0 */
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestWedge3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508790 */
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508850 */
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005077F0 */
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00507860 */
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005078E0 */
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00507950 */
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005079C0 */
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00507A40 */
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005081D0 */
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508240 */
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005082B0 */
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508320 */
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508390 */
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508400 */
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508AE0 */
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508B40 */
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508BA0 */
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508C00 */
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508C60 */
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508CC0 */
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

#endif /* THANDOR_WORLD_TERRAIN_HEIGHT_H */
