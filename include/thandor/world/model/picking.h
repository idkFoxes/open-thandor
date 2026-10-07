/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/model/picking.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_MODEL_PICKING_H
#define THANDOR_WORLD_MODEL_PICKING_H

#include <thandor/core/types.h>
#include <thandor/ui/frontend/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/world/model/types.h>
#include <thandor/core/contracts.h>

/* nearest distance of a ray that hit nothing (ModelRuntime_RaycastCandidateListNearest) */
inline constexpr int MODEL_RAYCAST_NO_HIT_DISTANCE = 0x7fffffff;

extern int32_t g_ModelRaycastMaximumDistance;

extern GraphicsFixedVec3 g_ModelRaycastOrigin; /* Q12 world-space ray origin */

extern GraphicsFixedVec3 g_ModelRaycastWorldDirectionQ28; /* Q28 world-space ray direction */

Bool8 ModelRuntimeNode_HitTestProjectedBoundsAndChildren
          (int pointerY,int pointerX,ModelRuntimeNode *modelNode,
          FrontendModelPointerHitContext *context,uint32_t *outDistanceQ12);

Q12 ModelNodeRuntime_RaycastHierarchyNearest
          (ModelRuntimeNode *modelNodeRuntime,ModelRuntimeNode **outNearestModelNode);

Bool8 ModelRuntime_RaycastCandidateListNearest
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,Q12 maximumDistanceQ12,Q12 originZQ12
          ,Q12 originYQ12,Q12 originXQ12,WorldOwnerRuntimeClassId requiredOwnerId,
          ModelRuntimeNode *excludedNode,WorldRuntimeContext *worldRuntime,Q12 *outNearestDistanceQ12,
          ModelRuntimeNode **outNearestModelNode);

#endif /* THANDOR_WORLD_MODEL_PICKING_H */
