/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/model/mesh_raycast.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_MODEL_MESH_RAYCAST_H
#define THANDOR_WORLD_MODEL_MESH_RAYCAST_H

#include <thandor/core/types.h>
#include <thandor/world/model/types.h>
#include <thandor/core/contracts.h>

Bool8 ModelMesh_IntersectTriangleRayDistance(ModelRaycastTriangleDescriptor *triangle,Q12 *outDistanceQ12);

extern GraphicsFixedVec3 g_ModelRaycastLocalOrigin; /* Q12 ray origin in the tested node's frame */
extern GraphicsFixedVec3 g_ModelRaycastLocalDirectionQ28; /* Q28 ray direction in the tested node's frame */

#endif /* THANDOR_WORLD_MODEL_MESH_RAYCAST_H */
