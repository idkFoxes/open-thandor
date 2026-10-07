/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/terrain/placement_tests.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_TERRAIN_PLACEMENT_TESTS_H
#define THANDOR_WORLD_TERRAIN_PLACEMENT_TESTS_H

#include <thandor/core/types.h>
#include <thandor/world/terrain/types.h>
#include <thandor/core/contracts.h>

bool TerrainHeightBand_TestAroundWorldPoint
          (FieldGridRadiusUnits radiusWorldUnits,Q12 referenceHeightQ12,Q12 worldXQ12,Q12 worldYQ12,
          FieldGridAsset *fieldGrid);

bool TerrainAuxHeightThreshold_TestAroundWorldPoint
          (FieldGridRadiusUnits radiusWorldUnits,Q12 referenceHeightQ12,Q12 worldXQ12,Q12 worldYQ12,
          FieldGridAsset *fieldGrid);

extern const TerrainClassPlacementAndOverlayCallbackTable10 g_TerrainClassPlacementAndOverlayCallbacks10;

#endif /* THANDOR_WORLD_TERRAIN_PLACEMENT_TESTS_H */
