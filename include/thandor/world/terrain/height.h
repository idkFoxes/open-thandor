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

/* Hexagonal radius scans (TerrainHeightDelta_*, TerrainHeightBand_*, TerrainAuxHeightThreshold_*): the scan step
   counter grows by 4 per cell along a straight direction and by 7 (about 4 * sqrt(3)) per step along a wedge's
   diagonal, and a scan stops at g_TerrainScanStepLimit (radius / 0x240, clamped to 1..255). */
#define TERRAIN_SCAN_STEP_STRAIGHT 4
#define TERRAIN_SCAN_STEP_DIAGONAL 7
#define TERRAIN_SCAN_RADIUS_PER_STEP 0x240 /* world units per step: one cell (0x900) is 4 steps */
#define TERRAIN_SCAN_STEP_LIMIT_MAX 255

/* 0x00508000 */
bool TerrainHeightBand_TestAroundWorldPoint
          (FieldGridRadiusUnits radiusWorldUnits,Q12 referenceHeightQ12,Q12 worldXQ12,Q12 worldYQ12,
          FieldGridAsset *fieldGrid);

/* 0x00508920 */
bool TerrainAuxHeightThreshold_TestAroundWorldPoint
          (FieldGridRadiusUnits radiusWorldUnits,Q12 referenceHeightQ12,Q12 worldXQ12,Q12 worldYQ12,
          FieldGridAsset *fieldGrid);

/* 0x00508D20 */
void TerrainHeightDelta_ApplyWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508DC0 */
void TerrainHeightDelta_ApplyWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508E60 */
void TerrainHeightDelta_ApplyWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508F00 */
void TerrainHeightDelta_ApplyWedge3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508FA0 */
void TerrainHeightDelta_ApplyWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00509040 */
void TerrainHeightDelta_ApplyWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00504520 */
TerrainRayTriangleResult TerrainTriangle_IntersectRayDistance
          (Q12 rayDeltaZQ12,Q12 gridRayDelta0Q12,Q12 gridRayDelta1Q12,Q12 rayOriginZQ12,
          Q12 cornerHeight0Q12,Q12 cornerHeight1Q12,Q12 cornerHeight2Q12,Q12 cornerHeight3Q12,
          Q12 cellLocalCoord1Q12,Q12 cellLocalCoord0Q12);

/* ESI/EDX/ECX results of TerrainRay_AdvanceGridTraversal: next cell and grid corner. */
extern FieldGridCell *g_TerrainRayNextCell;
extern Q12 g_TerrainRayNextCoord0Q12;
extern Q12 g_TerrainRayNextCoord1Q12;

/* 0x005049E0 */
bool TerrainRay_AdvanceGridTraversal
          (Q12 rayEndCoord0Q12,Q12 rayEndCoord1Q12,Q12 rayStartCoord0Q12,Q12 rayStartCoord1Q12,
          FieldGridRowStrideBytes rowStrideBytes,FieldGridCell *currentCell,Q12 currentGridCoord0Q12
          ,Q12 currentGridCoord1Q12);

/* 0x00507AB0 */
bool TerrainHeightBand_TestWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00507BA0 */
bool TerrainHeightBand_TestWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00507C80 */
bool TerrainHeightBand_TestWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00507D60 */
bool TerrainHeightBand_TestWedge3(TerrainDirectionalScanStep scanStep,uint8_t *cell);

/* 0x00507E40 */
bool TerrainHeightBand_TestWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00507F20 */
bool TerrainHeightBand_TestWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508470 */
bool TerrainAuxHeightThreshold_TestWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508540 */
bool TerrainAuxHeightThreshold_TestWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508600 */
bool TerrainAuxHeightThreshold_TestWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005086C0 */
bool TerrainAuxHeightThreshold_TestWedge3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508790 */
bool TerrainAuxHeightThreshold_TestWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508850 */
bool TerrainAuxHeightThreshold_TestWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005077F0 */
bool TerrainHeightBand_TestDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00507860 */
bool TerrainHeightBand_TestDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005078E0 */
bool TerrainHeightBand_TestDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00507950 */
bool TerrainHeightBand_TestDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005079C0 */
bool TerrainHeightBand_TestDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00507A40 */
bool TerrainHeightBand_TestDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005081D0 */
bool TerrainAuxHeightThreshold_TestDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508240 */
bool TerrainAuxHeightThreshold_TestDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005082B0 */
bool TerrainAuxHeightThreshold_TestDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508320 */
bool TerrainAuxHeightThreshold_TestDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508390 */
bool TerrainAuxHeightThreshold_TestDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508400 */
bool TerrainAuxHeightThreshold_TestDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508AE0 */
void TerrainHeightDelta_ApplyDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508B40 */
void TerrainHeightDelta_ApplyDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508BA0 */
void TerrainHeightDelta_ApplyDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508C00 */
void TerrainHeightDelta_ApplyDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508C60 */
void TerrainHeightDelta_ApplyDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508CC0 */
void TerrainHeightDelta_ApplyDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

#endif /* THANDOR_WORLD_TERRAIN_HEIGHT_H */
