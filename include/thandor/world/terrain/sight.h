/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/terrain/sight.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_TERRAIN_SIGHT_H
#define THANDOR_WORLD_TERRAIN_SIGHT_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

void TerrainProjectedOcclusion_AccumulateMaskAroundWorldPoint
          (uint64_t occupancyMaskBits,FieldGridRadiusUnits radiusWorldUnits,Q12 referenceHeightQ12,
          Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid);

void TerrainProjectedOcclusion_TraceWedge0(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

void TerrainProjectedOcclusion_TraceWedge1(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

void TerrainProjectedOcclusion_TraceWedge2(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

void TerrainProjectedOcclusion_TraceWedge3(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

void TerrainProjectedOcclusion_TraceWedge4(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

void TerrainProjectedOcclusion_TraceWedge5(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

void TerrainProjectedOcclusion_ScanDirection0(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

void TerrainProjectedOcclusion_ScanDirection1(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

void TerrainProjectedOcclusion_ScanDirection2(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

void TerrainProjectedOcclusion_ScanDirection3(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

void TerrainProjectedOcclusion_ScanDirection4(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

void TerrainProjectedOcclusion_ScanDirection5(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

#endif /* THANDOR_WORLD_TERRAIN_SIGHT_H */
