/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/terrain/field_deformation.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_TERRAIN_FIELD_DEFORMATION_H
#define THANDOR_WORLD_TERRAIN_FIELD_DEFORMATION_H

#include <thandor/core/types.h>
#include <thandor/world/terrain/types.h>
#include <thandor/core/contracts.h>

void FieldGrid_ApplyRadialTerrainHeightDeltaAndRefreshSurface
          (TerrainMaterialIndex terrainMaterialIndexOrNegativeSentinel,
          FieldGridRadiusUnits radiusWorldUnits,Q12 terrainHeightDeltaAmplitudeQ12,
          Q12 centerWorldYQ12,Q12 centerWorldXQ12,FieldGridAsset *fieldGrid);

void FieldGrid_ApplyHeightAtWorldPointAndRefreshNeighbors
          (TerrainHeightBrushDeltaSource heightDeltaSourceValue,Q12 worldZQ12,Q12 worldYQ12,
          Q12 worldXQ12,FieldGridAsset *fieldGrid);

void FieldGridCell_ApplyRadialTerrainHeightDeltaAndMaterial(TerrainMaterialIndex terrainMaterialIndexOrNegativeSentinel,
          FieldGridRadiusUnits radiusWorldUnits,Q12 terrainHeightDeltaAmplitudeQ12,
          Q12 centerWorldYQ12,Q12 centerWorldXQ12,FieldGridCell *cell);

void TerrainHeightDelta_ApplyWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

void TerrainHeightDelta_ApplyWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

void TerrainHeightDelta_ApplyWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

void TerrainHeightDelta_ApplyWedge3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

void TerrainHeightDelta_ApplyWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

void TerrainHeightDelta_ApplyWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

void TerrainHeightDelta_ApplyDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

void TerrainHeightDelta_ApplyDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

void TerrainHeightDelta_ApplyDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

void TerrainHeightDelta_ApplyDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

void TerrainHeightDelta_ApplyDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

void TerrainHeightDelta_ApplyDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell);

#endif /* THANDOR_WORLD_TERRAIN_FIELD_DEFORMATION_H */
