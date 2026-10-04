/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/terrain/sight.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_TERRAIN_SIGHT_H
#define THANDOR_WORLD_TERRAIN_SIGHT_H

#include <thandor/core/types.h>
#include <thandor/world/terrain/types.h>
#include <thandor/core/contracts.h>

void TerrainProjectedOcclusion_AccumulateMaskAroundWorldPoint
          (uint64_t occupancyMaskBits,FieldGridRadiusUnits radiusWorldUnits,Q12 referenceHeightQ12,
          Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid);

#endif /* THANDOR_WORLD_TERRAIN_SIGHT_H */
