/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/terrain/field_sampling.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_TERRAIN_FIELD_SAMPLING_H
#define THANDOR_WORLD_TERRAIN_FIELD_SAMPLING_H

#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/world/terrain/types.h>
#include <thandor/core/contracts.h>

Bool8 FieldGrid_GetNearestTerrainPoint(Q12 worldY,Q12 worldX,FieldGridAsset *field,FixedVectorQ12 *outPoint);

Bool8 FieldGrid_GetNearestTopSurfacePoint(Q12 worldY,Q12 worldX,FieldGridAsset *field,FixedVectorQ12 *outPoint);

int32_t FieldGrid_GetNearestWaterDelta(Q12 worldY,Q12 worldX,FieldGridAsset *field);

Bool8 FieldGrid_InterpolateTerrainHeight(Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid,Q12 *outHeightQ12);

int32_t FieldGrid_InterpolateWaterDelta(Q12 worldY,Q12 worldX,FieldGridAsset *field);

Bool8 FieldGrid_InterpolateWaterSurfaceHeight(Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid,Q12 *outHeightQ12);

Bool8 FieldGrid_InterpolateTopSurfaceHeight(Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid,Q12 *outHeightQ12);

Bool8 FieldGrid_InterpolateTerrainHeightAndNormal
          (Q12 worldY,Q12 worldX,FieldGridAsset *field,Q12 *outHeightQ12,uint32_t *outPackedNormalAngles);

Bool8 FieldGrid_TestWorldPointBlocked
          (FieldGridByteOffset factionSlot,Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid
          );

FieldGridCoordinates FieldGrid_WorldToGridQ12(Q12 worldY,Q12 worldX);

extern const FieldGridInterpolationCallbackTable5 g_FieldGridInterpolationCallbacks5;

#endif /* THANDOR_WORLD_TERRAIN_FIELD_SAMPLING_H */
