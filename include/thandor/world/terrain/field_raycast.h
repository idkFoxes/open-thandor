/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/terrain/field_raycast.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_TERRAIN_FIELD_RAYCAST_H
#define THANDOR_WORLD_TERRAIN_FIELD_RAYCAST_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

Bool8 FieldGrid_RaycastTerrainSurfaceDistance
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,Q12 rayScaleQ12,Q12 rayOriginZQ12,
          Q12 rayOriginYQ12,Q12 rayOriginXQ12,FieldGridAsset *fieldGrid,Q12 *outDistanceQ12,
          uint32_t *outMaterialIndex);

Bool8 FieldGrid_RaycastSecondarySurfaceDistance
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,Q12 rayScaleQ12,Q12 rayOriginZQ12,
          Q12 rayOriginYQ12,Q12 rayOriginXQ12,FieldGridAsset *fieldGrid,Q12 *outDistanceQ12);

Bool8 FieldGrid_RaycastTerrainTrianglesAlongDirection
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,FixedMathScale32 rayScaleQ12,
          Q12 rayOriginZQ12,Q12 rayOriginYQ12,Q12 rayOriginXQ12,FieldGridAsset *fieldGrid,
          Q12 *outDistanceQ12);

#endif /* THANDOR_WORLD_TERRAIN_FIELD_RAYCAST_H */
