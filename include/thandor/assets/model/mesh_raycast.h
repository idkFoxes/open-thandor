/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/model/mesh_raycast.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_MODEL_MESH_RAYCAST_H
#define THANDOR_ASSETS_MODEL_MESH_RAYCAST_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/model/mesh_raycast. */

/* Functions are grouped by semantic ownership. */

Bool8 ModelMesh_IntersectTriangleRayDistance(ModelRaycastTriangleDescriptor *triangle,Q12 *outDistanceQ12);

extern GraphicsFixedVec3 g_ModelRaycastLocalOrigin; /* Q12 ray origin in the tested node's frame */
extern GraphicsFixedVec3 g_ModelRaycastLocalDirectionQ28; /* Q28 ray direction in the tested node's frame */

#endif /* THANDOR_ASSETS_MODEL_MESH_RAYCAST_H */
