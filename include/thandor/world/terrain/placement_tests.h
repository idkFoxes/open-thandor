/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/terrain/placement_tests.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_TERRAIN_PLACEMENT_TESTS_H
#define THANDOR_WORLD_TERRAIN_PLACEMENT_TESTS_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/terrain/placement_tests. */
/* Functions are grouped by semantic ownership. */

Bool8 TerrainHeightBand_TestAroundWorldPoint
          (FieldGridRadiusUnits radiusWorldUnits,Q12 referenceHeightQ12,Q12 worldXQ12,Q12 worldYQ12,
          FieldGridAsset *fieldGrid);

Bool8 TerrainAuxHeightThreshold_TestAroundWorldPoint
          (FieldGridRadiusUnits radiusWorldUnits,Q12 referenceHeightQ12,Q12 worldXQ12,Q12 worldYQ12,
          FieldGridAsset *fieldGrid);

Bool8 TerrainHeightBand_TestWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

Bool8 TerrainHeightBand_TestWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

Bool8 TerrainHeightBand_TestWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

Bool8 TerrainHeightBand_TestWedge3(TerrainDirectionalScanStep scanStep,uint8_t *cell);

Bool8 TerrainHeightBand_TestWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

Bool8 TerrainHeightBand_TestWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

Bool8 TerrainAuxHeightThreshold_TestWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

Bool8 TerrainAuxHeightThreshold_TestWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

Bool8 TerrainAuxHeightThreshold_TestWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

Bool8 TerrainAuxHeightThreshold_TestWedge3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

Bool8 TerrainAuxHeightThreshold_TestWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

Bool8 TerrainAuxHeightThreshold_TestWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

Bool8 TerrainHeightBand_TestDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

Bool8 TerrainHeightBand_TestDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

Bool8 TerrainHeightBand_TestDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

Bool8 TerrainHeightBand_TestDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

Bool8 TerrainHeightBand_TestDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

Bool8 TerrainHeightBand_TestDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

Bool8 TerrainAuxHeightThreshold_TestDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

Bool8 TerrainAuxHeightThreshold_TestDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

Bool8 TerrainAuxHeightThreshold_TestDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

Bool8 TerrainAuxHeightThreshold_TestDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

Bool8 TerrainAuxHeightThreshold_TestDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

Bool8 TerrainAuxHeightThreshold_TestDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

extern const TerrainClassPlacementAndOverlayCallbackTable10 g_TerrainClassPlacementAndOverlayCallbacks10;

#endif /* THANDOR_WORLD_TERRAIN_PLACEMENT_TESTS_H */
