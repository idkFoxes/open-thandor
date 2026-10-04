/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/terrain/water_relaxation.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_TERRAIN_WATER_RELAXATION_H
#define THANDOR_WORLD_TERRAIN_WATER_RELAXATION_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

void TerrainGrid_RunDirectionalRelaxationPasses(FrontendPlayerRuntimeId playerRuntimeId,uint32_t reservedZero,
          TerrainRelaxationPassCount passCount,TerrainRelaxationMode mode);

void TerrainGrid_RelaxNeighborHeightsForwardWithSignGate(FieldGridAsset *fieldGrid);

void TerrainGrid_RelaxNeighborHeightsReverseWithSignGate(FieldGridAsset *fieldGrid);

void TerrainGrid_RelaxNeighborHeightsForward(FieldGridAsset *fieldGrid);

void TerrainGrid_RelaxNeighborHeightsReverse(FieldGridAsset *fieldGrid);

#endif /* THANDOR_WORLD_TERRAIN_WATER_RELAXATION_H */
