#ifndef THANDOR_WORLD_TERRAIN_HEIGHT_H
#define THANDOR_WORLD_TERRAIN_HEIGHT_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/terrain/height. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00508000 */
int TerrainHeightBand_TestAroundWorldPoint (FieldGridRadiusUnits radiusWorldUnits,Q12 referenceHeightQ12,Q12 worldXQ12, Q12 worldYQ12,FieldGridAsset *fieldGrid);

/* 0x00508920 */
int TerrainAuxHeightThreshold_TestAroundWorldPoint (FieldGridRadiusUnits radiusWorldUnits,Q12 referenceHeightQ12,Q12 worldXQ12, Q12 worldYQ12,FieldGridAsset *fieldGrid);

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
longlong TerrainTriangle_IntersectRayDistanceCf (Q12 rayDeltaZQ12,Q12 gridRayDelta0Q12,Q12 gridRayDelta1Q12,Q12 rayOriginZQ12, Q12 cornerHeight0Q12,Q12 cornerHeight1Q12,Q12 cornerHeight2Q12, Q12 cornerHeight3Q12,Q12 cellLocalCoord1Q12,Q12 cellLocalCoord0Q12);

/* 0x005049E0 */
undefined8 TerrainRay_AdvanceGridTraversalCf (Q12 rayEndCoord0Q12,Q12 rayEndCoord1Q12,Q12 rayStartCoord0Q12,Q12 rayStartCoord1Q12, undefined4 param_5,undefined4 param_6,Q12 currentGridCoord0Q12,Q12 currentGridCoord1Q12);

/* 0x00507AB0 */
void TerrainHeightBand_TestWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00507BA0 */
undefined8 TerrainHeightBand_TestWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00507C80 */
void TerrainHeightBand_TestWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00507D60 */
void TerrainHeightBand_TestWedge3(TerrainDirectionalScanStep scanStep,byte *cell);

/* 0x00507E40 */
undefined8 TerrainHeightBand_TestWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00507F20 */
void TerrainHeightBand_TestWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508470 */
void TerrainAuxHeightThreshold_TestWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508540 */
void TerrainAuxHeightThreshold_TestWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508600 */
void TerrainAuxHeightThreshold_TestWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005086C0 */
void TerrainAuxHeightThreshold_TestWedge3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508790 */
void TerrainAuxHeightThreshold_TestWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508850 */
void TerrainAuxHeightThreshold_TestWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005077F0 */
undefined8 TerrainHeightBand_TestDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00507860 */
undefined8 TerrainHeightBand_TestDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005078E0 */
undefined8 TerrainHeightBand_TestDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00507950 */
undefined8 TerrainHeightBand_TestDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005079C0 */
undefined8 TerrainHeightBand_TestDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00507A40 */
undefined8 TerrainHeightBand_TestDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005081D0 */
void TerrainAuxHeightThreshold_TestDirection0 (TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508240 */
void TerrainAuxHeightThreshold_TestDirection1 (TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x005082B0 */
void TerrainAuxHeightThreshold_TestDirection2 (TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508320 */
void TerrainAuxHeightThreshold_TestDirection3 (TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508390 */
void TerrainAuxHeightThreshold_TestDirection4 (TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

/* 0x00508400 */
void TerrainAuxHeightThreshold_TestDirection5 (TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

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
