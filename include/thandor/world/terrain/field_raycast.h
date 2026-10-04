/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/terrain/field_raycast.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_TERRAIN_FIELD_RAYCAST_H
#define THANDOR_WORLD_TERRAIN_FIELD_RAYCAST_H

#include <thandor/core/types.h>
#include <thandor/world/terrain/types.h>
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

Bool8 TerrainTriangle_IntersectRayDistance
          (Q12 rayDeltaZQ12,Q12 gridRayDelta0Q12,Q12 gridRayDelta1Q12,Q12 rayOriginZQ12,
          Q12 cornerHeight0Q12,Q12 cornerHeight1Q12,Q12 cornerHeight2Q12,Q12 cornerHeight3Q12,
          Q12 cellLocalCoord1Q12,Q12 cellLocalCoord0Q12,Q12 *outDistanceQ12);

/* Extra results of TerrainRay_AdvanceGridTraversal: next cell and grid corner. */
extern FieldGridCell *g_TerrainRayNextCell;

extern Q12 g_TerrainRayNextCoord0Q12;

extern Q12 g_TerrainRayNextCoord1Q12;

Bool8 TerrainRay_AdvanceGridTraversal
          (Q12 rayEndCoord0Q12,Q12 rayEndCoord1Q12,Q12 rayStartCoord0Q12,Q12 rayStartCoord1Q12,
          FieldGridRowStrideBytes rowStrideBytes,FieldGridCell *currentCell,Q12 currentGridCoord0Q12
          ,Q12 currentGridCoord1Q12);

#endif /* THANDOR_WORLD_TERRAIN_FIELD_RAYCAST_H */
