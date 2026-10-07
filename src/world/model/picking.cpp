/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/model/picking.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* Model picking: the pointer hit test against the projected bounds of a model hierarchy and the nearest ray
   hit on its mesh triangles. */

#include <thandor/world/model/picking.h>
#include <thandor/thandor.h>

/* Module data. */

static GraphicsProjectedPoint2i g_ModelProjectedBoundsCornerScratch8[8] = {};

int32_t g_ModelRaycastMaximumDistance = 0;

/* Q12 world-space ray origin */
GraphicsFixedVec3 g_ModelRaycastOrigin = {};

/* Q28 world-space ray direction */
GraphicsFixedVec3 g_ModelRaycastWorldDirectionQ28 = {};

/* Transforms one bounds corner by g_GraphicsTransformScratchMatrix3x4. Returns its clip bit when it lies behind
   the near plane (z < g_ProjectionScaleFixed); otherwise stores its screen point in
   g_ModelProjectedBoundsCornerScratch8[cornerIndex] and returns 0. */
static uint8_t ModelBounds_ProjectCorner
          (int cornerIndex,GraphicsWorldCoordinateQ12 x,GraphicsWorldCoordinateQ12 y,GraphicsWorldCoordinateQ12 z)

{
  GraphicsProjectedPointPair projectedCorner;

  g_GraphicsTransformInputScratchVec3.x = x;
  g_GraphicsTransformInputScratchVec3.y = y;
  g_GraphicsTransformInputScratchVec3.z = z;
  FixedTransform_ApplyPoint
            (&g_GraphicsTransformOutputScratchVec3,&g_GraphicsTransformInputScratchVec3,
             &g_GraphicsTransformScratchMatrix3x4);
  if (g_GraphicsTransformOutputScratchVec3.z < (int)g_ProjectionScaleFixed) {
    return (uint8_t)MODEL_BOUNDS_CORNER_BIT(cornerIndex);
  }
  projectedCorner = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
  g_ModelProjectedBoundsCornerScratch8[cornerIndex].x = projectedCorner.projectedX >> Q12_SHIFT;
  g_ModelProjectedBoundsCornerScratch8[cornerIndex].y = projectedCorner.projectedY >> Q12_SHIFT;
  return 0;
}

/* Projects the eight corners of the resource's local bounds; corner i = (x i&1, y i&2, z i&4). Returns the mask
   of corners behind the near plane (bit i = corner i). */
static uint8_t ModelResource_ProjectBoundsCorners(ModelResource *resourceView)

{
  GraphicsWorldCoordinateQ12 x0 = resourceView->localBoundsX0Q12;
  GraphicsWorldCoordinateQ12 x1 = resourceView->localBoundsX1Q12;
  GraphicsWorldCoordinateQ12 y0 = resourceView->localBoundsY0Q12;
  GraphicsWorldCoordinateQ12 y1 = resourceView->localBoundsY1Q12;
  GraphicsWorldCoordinateQ12 z0 = resourceView->localBoundsZ0Q12;
  GraphicsWorldCoordinateQ12 z1 = resourceView->localBoundsZ1Q12;
  uint8_t clippedCornerMask;

  clippedCornerMask = ModelBounds_ProjectCorner(0,x0,y0,z0);
  clippedCornerMask |= ModelBounds_ProjectCorner(1,x1,y0,z0);
  clippedCornerMask |= ModelBounds_ProjectCorner(2,x0,y1,z0);
  clippedCornerMask |= ModelBounds_ProjectCorner(3,x1,y1,z0);
  clippedCornerMask |= ModelBounds_ProjectCorner(4,x0,y0,z1);
  clippedCornerMask |= ModelBounds_ProjectCorner(5,x1,y0,z1);
  clippedCornerMask |= ModelBounds_ProjectCorner(6,x0,y1,z1);
  clippedCornerMask |= ModelBounds_ProjectCorner(7,x1,y1,z1);
  return clippedCornerMask;
}

/* The twelve triangles of the bounds box faces, in the order the original tests them. */
static const uint8_t g_ModelBoundsHitTriangleCorners[12][3] = {
  {2,1,0},{2,1,3},{6,5,4},{6,5,7},{2,4,0},{2,6,4},
  {3,5,1},{3,7,5},{4,1,0},{1,5,4},{3,6,2},{3,6,7}
};

/* True when the pointer lies inside one of the projected box triangles that has no corner behind the near
   plane; stops at the first hit. */
static bool ModelBounds_PointerHitsProjectedBox(int pointerY,int pointerX,uint8_t clippedCornerMask)

{
  int triangleIndex;
  const uint8_t *corners;

  for (triangleIndex = 0; triangleIndex < 12; triangleIndex++) {
    corners = g_ModelBoundsHitTriangleCorners[triangleIndex];
    if (((clippedCornerMask & MODEL_BOUNDS_TRIANGLE_CORNERS(corners[0],corners[1],corners[2])) == 0) &&
        GraphicsProjectedPoint_IsInsideTriangle
                  (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + corners[0],
                   g_ModelProjectedBoundsCornerScratch8 + corners[1],
                   g_ModelProjectedBoundsCornerScratch8 + corners[2])) {
      return true;
    }
  }
  return false;
}

/* Pointer hit test of a model hierarchy (FrontendModelPointerContext_FindBestEligibleModelHitTarget): projects the
   eight corners of the node's local bounding box and tests the pointer against the twelve triangles of its faces
   (faces with a corner behind the near plane are skipped). On a hit returns the distance from the context's
   reference point to the node (to the box centre with HIT_DISTANCE_TO_BOUNDS_CENTER); otherwise the children are tested
   in order. Returns true on a hit and stores the distance in *outDistanceQ12; returns false (and leaves
   *outDistanceQ12 unchanged) when neither the node nor a child was hit.
*/
bool ModelRuntimeNode_HitTestProjectedBoundsAndChildren
          (int pointerY,int pointerX,ModelRuntimeNode *modelNode,
          FrontendModelPointerHitContext *context,uint32_t *outDistanceQ12)

{
  ModelResource *resourceView;
  ModelRuntimeNode *childNode;
  uint8_t clippedCornerMask;
  uint32_t childrenRemaining;
  int childIndex;

  resourceView = modelNode->modelPayload.modelResource;
  if ((resourceView->boundingRadiusQ12 != 0) &&
     ((resourceView->hitTestFlags20C & MODEL_RESOURCE_DISABLE_PROJECTED_HIT_TEST) == 0)) {
    FixedTransform_Compose
              (&g_GraphicsTransformScratchMatrix3x4,&modelNode->worldTransform,&g_ViewProjectionMatrixFixed);
    clippedCornerMask = ModelResource_ProjectBoundsCorners(resourceView);
    if (ModelBounds_PointerHitsProjectedBox(pointerY,pointerX,clippedCornerMask)) {
      if (Any(context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_HIT_DISTANCE_TO_BOUNDS_CENTER)) {
        *outDistanceQ12 =
             FixedMath_Length3((((resourceView->localBoundsZ0Q12 + resourceView->localBoundsZ1Q12) >> 1) +
                               modelNode->worldTransform.translation.z) -
                               context->hitReferenceWorldZQ12,
                               (((resourceView->localBoundsY0Q12 + resourceView->localBoundsY1Q12) >> 1) +
                               modelNode->worldTransform.translation.y) -
                               context->hitReferenceWorldYQ12,
                               (((resourceView->localBoundsX0Q12 + resourceView->localBoundsX1Q12) >> 1) +
                               modelNode->worldTransform.translation.x) -
                               context->hitReferenceWorldXQ12);
        return true;
      }
      *outDistanceQ12 =
           FixedMath_Length3(modelNode->worldTransform.translation.z -
                             context->hitReferenceWorldZQ12,
                             modelNode->worldTransform.translation.y -
                             context->hitReferenceWorldYQ12,
                             modelNode->worldTransform.translation.x -
                             context->hitReferenceWorldXQ12);
      return true;
    }
  }
  childIndex = 0;
  for (childrenRemaining = modelNode->childCount; childrenRemaining != 0;
       childrenRemaining = childrenRemaining - 1) {
    childNode = modelNode->childNodes[childIndex];
    if (childNode != nullptr) {
      if (ModelRuntimeNode_HitTestProjectedBoundsAndChildren
                         (pointerY,pointerX,childNode,context,outDistanceQ12)) {
        return true;
      }
    }
    childIndex++;
  }
  return false;
}

/* A miss of ModelNodeRuntime_RaycastHierarchyNearest before the mesh test. The original leaves its scratch
   value (scratchValue: a projected distance or a product high word) in the "nearest node" output; stored as
   NULL here because an integer is no node pointer, and every caller uses the node only after a hit
   (gameplay/army/combat.cpp, world/shots/flight.cpp), so the result is the same. */
static Q12 ModelNodeRuntime_RaycastMissWithScratchNode
          (ModelRuntimeNode **outNearestModelNode,int scratchValue)

{
  (void)scratchValue;
  *outNearestModelNode = nullptr;
  return MODEL_RAYCAST_NO_HIT_DISTANCE;
}

/* The mesh group ModelNodeRuntime_RaycastHierarchyNearest tests: the last one, or the one before it when the
   model has a shadow mesh group. */
static ModelMeshGroupRelativeOffset *ModelResource_FindRaycastMeshGroup(ModelResource *resourceView)

{
  ModelMeshGroupRelativeOffset *meshGroupCursor;
  ModelMeshGroupCount meshGroupsToSkip;

  meshGroupCursor = &resourceView->firstMeshGroupRelativeOffset;
  meshGroupsToSkip = resourceView->meshGroupCount - 1;
  if (resourceView->shadowMeshGroupOffset != 0 && meshGroupsToSkip != 0) {
    meshGroupsToSkip--;
  }
  for (; meshGroupsToSkip != 0; meshGroupsToSkip--) {
    meshGroupCursor = reinterpret_cast<ModelMeshGroupRelativeOffset *>(reinterpret_cast<uint8_t *>(meshGroupCursor) + *meshGroupCursor); /* each group starts with its byte size */
  }
  return meshGroupCursor;
}

/* Ray test of a model hierarchy against the ray in g_ModelRaycastOrigin and
   g_ModelRaycastWorldDirectionQ28 (ModelRuntime_RaycastCandidateListNearest): when the ray passes the node's
   bounding sphere within
   g_ModelRaycastMaximumDistance, it is moved into the node's frame and tested against every triangle of the
   node's mesh group, then the children are tested. Returns the nearest hit distance and stores the nearest
   node in *outNearestModelNode; returns MODEL_RAYCAST_NO_HIT_DISTANCE when nothing was hit (a hit never has
   that distance).
   On a miss *outNearestModelNode receives NULL (the original: a leftover scratch value, see
   ModelNodeRuntime_RaycastMissWithScratchNode); ModelRuntime_RaycastCandidateListNearest can pick it up.
*/
Q12 ModelNodeRuntime_RaycastHierarchyNearest
          (ModelRuntimeNode *modelNodeRuntime,ModelRuntimeNode **outNearestModelNode)

{
  int *triangleCountField;
  ModelResource *resourceView;
  int64_t projectionWide;
  int64_t discriminant;
  int projectedDistance;
  uint32_t negatedAngle2;
  int deltaX;
  int deltaY;
  int deltaZ;
  int boundingRadius;
  int nodeX;
  int nodeY;
  int nodeZ;
  int trianglesRemaining;
  uint32_t childrenRemaining;
  ModelPackedGeometryRecordCount meshRecordsRemaining;
  ModelMeshGroupRelativeOffset *meshGroupCursor;
  ModelRaycastTriangleDescriptor *triangle;
  ModelMeshHeader *meshHeader;
  ModelRuntimeNode *nearestModelNode;
  ModelRuntimeNode *childNearestModelNode;
  Q12 nearestDistanceQ12;
  Q12 triangleDistanceQ12;
  Q12 childDistanceQ12;

  /* bounding sphere test: the projection of the node centre onto the ray must lie within the ray's reach
     (widened by the radius) and the ray must pass the centre closer than the radius */
  deltaX = modelNodeRuntime->worldTransform.translation.x - g_ModelRaycastOrigin.x;
  deltaY = modelNodeRuntime->worldTransform.translation.y - g_ModelRaycastOrigin.y;
  deltaZ = modelNodeRuntime->worldTransform.translation.z - g_ModelRaycastOrigin.z;
  boundingRadius = modelNodeRuntime->subtreeBoundingRadiusQ12;
  projectionWide = (int64_t)deltaY * (int64_t)g_ModelRaycastWorldDirectionQ28.y +
          (int64_t)deltaX * (int64_t)g_ModelRaycastWorldDirectionQ28.x +
          (int64_t)deltaZ * (int64_t)g_ModelRaycastWorldDirectionQ28.z;
  projectedDistance = FIXED_PRODUCT_SHR(projectionWide,Q28_SHIFT);
  if ((projectedDistance < -boundingRadius) ||
      (g_ModelRaycastMaximumDistance + boundingRadius <= projectedDistance)) {
    return ModelNodeRuntime_RaycastMissWithScratchNode(outNearestModelNode,projectedDistance);
  }
  discriminant = ((int64_t)boundingRadius * (int64_t)boundingRadius +
                  (int64_t)projectedDistance * (int64_t)projectedDistance) -
                 (int64_t)deltaX * (int64_t)deltaX;
  if (discriminant < 0) {
    return ModelNodeRuntime_RaycastMissWithScratchNode(outNearestModelNode,FIXED_MUL_HIGH(deltaX,deltaX));
  }
  discriminant = discriminant - (int64_t)deltaY * (int64_t)deltaY;
  if (discriminant < 0) {
    return ModelNodeRuntime_RaycastMissWithScratchNode(outNearestModelNode,FIXED_MUL_HIGH(deltaY,deltaY));
  }
  if (discriminant - (int64_t)deltaZ * (int64_t)deltaZ < 0) {
    return ModelNodeRuntime_RaycastMissWithScratchNode(outNearestModelNode,FIXED_MUL_HIGH(deltaZ,deltaZ));
  }

  /* move the ray into the node's frame */
  negatedAngle2 = -modelNodeRuntime->modelPayload.worldRotationAngle2;
  FixedTransform_BuildRotationBasis
            (&g_GraphicsTransformScratchMatrix3x4,negatedAngle2 & FIXED_ANGLE16_MASK,
             modelNodeRuntime->modelPayload.worldRotationAngle1,
             modelNodeRuntime->modelPayload.worldRotationAngle0 + FIXED_ANGLE16_HALF_TURN +
             negatedAngle2 & FIXED_ANGLE16_MASK);
  resourceView = modelNodeRuntime->modelPayload.modelResource;
  g_GraphicsTransformScratchMatrix3x4.translation.x = 0;
  g_GraphicsTransformScratchMatrix3x4.translation.y = 0;
  g_GraphicsTransformScratchMatrix3x4.translation.z = 0;
  nodeX = modelNodeRuntime->worldTransform.translation.x;
  nodeY = modelNodeRuntime->worldTransform.translation.y;
  nodeZ = modelNodeRuntime->worldTransform.translation.z;
  /* The original walks meshGroupCount - 1 groups unchecked, i.e. 0xFFFFFFFF of them for a node without mesh
     groups (reachable: the sphere test above uses the subtree radius); bounded here because such a node has no
     triangles: its mesh test is skipped. */
  meshGroupCursor = nullptr;
  if (resourceView->meshGroupCount != 0) {
    meshGroupCursor = ModelResource_FindRaycastMeshGroup(resourceView);
  }
  g_ModelRaycastOrigin.x = g_ModelRaycastOrigin.x - nodeX;
  g_ModelRaycastOrigin.y = g_ModelRaycastOrigin.y - nodeY;
  g_ModelRaycastOrigin.z = g_ModelRaycastOrigin.z - nodeZ;
  FixedTransform_ApplyPoint
            (&g_ModelRaycastLocalOrigin,&g_ModelRaycastOrigin,&g_GraphicsTransformScratchMatrix3x4);
  g_ModelRaycastOrigin.x = g_ModelRaycastOrigin.x + nodeX;
  g_ModelRaycastOrigin.y = g_ModelRaycastOrigin.y + nodeY;
  g_ModelRaycastOrigin.z = g_ModelRaycastOrigin.z + nodeZ;
  FixedTransform_ApplyPoint
            (&g_ModelRaycastLocalDirectionQ28,&g_ModelRaycastWorldDirectionQ28,
             &g_GraphicsTransformScratchMatrix3x4);

  /* every triangle of every mesh of the mesh group */
  nearestDistanceQ12 = MODEL_RAYCAST_NO_HIT_DISTANCE;
  triangle = nullptr;
  meshRecordsRemaining = 0;
  if (meshGroupCursor != nullptr) {
    triangle = reinterpret_cast<ModelRaycastTriangleDescriptor *>(meshGroupCursor + 8); /* the group's first mesh header */
    meshRecordsRemaining = meshGroupCursor[1];
  }
  for (; meshRecordsRemaining != 0; meshRecordsRemaining--) {
    /* triangle points at a ModelMeshHeader here: skip it and its vertex records */
    meshHeader = reinterpret_cast<ModelMeshHeader *>(triangle);
    triangleCountField = &meshHeader->triangleCount;
    triangle = reinterpret_cast<ModelRaycastTriangleDescriptor *>
               (reinterpret_cast<uint8_t *>(meshHeader + 1) + meshHeader->vertexCount * MODEL_MESH_RECORD_SIZE);
    for (trianglesRemaining = *triangleCountField; trianglesRemaining != 0; trianglesRemaining--) {
      if (ModelMesh_IntersectTriangleRayDistance(triangle,&triangleDistanceQ12) &&
          (triangleDistanceQ12 <= nearestDistanceQ12)) {
        nearestDistanceQ12 = triangleDistanceQ12;
      }
      triangle = triangle + 1;
    }
  }

  /* the children, last slot first */
  childNearestModelNode = nullptr;
  nearestModelNode = modelNodeRuntime;
  for (childrenRemaining = modelNodeRuntime->childCount; childrenRemaining != 0; childrenRemaining--) {
    if (modelNodeRuntime->childNodes[childrenRemaining - 1] != nullptr) {
      childDistanceQ12 = ModelNodeRuntime_RaycastHierarchyNearest
                         (modelNodeRuntime->childNodes[childrenRemaining - 1],&childNearestModelNode);
      if ((childDistanceQ12 != MODEL_RAYCAST_NO_HIT_DISTANCE) && (childDistanceQ12 < nearestDistanceQ12)) {
        nearestDistanceQ12 = childDistanceQ12;
        nearestModelNode = childNearestModelNode;
      }
    }
  }
  if (nearestDistanceQ12 == MODEL_RAYCAST_NO_HIT_DISTANCE) {
    /* NULL, or what the last child test stored (also NULL on its miss) */
    *outNearestModelNode = childNearestModelNode;
    return MODEL_RAYCAST_NO_HIT_DISTANCE;
  }
  *outNearestModelNode = nearestModelNode;
  return nearestDistanceQ12;
}

/* Casts a ray from the origin in the direction (elevationAngle, azimuthAngle), at most maximumDistanceQ12 long,
   against the models in worldRuntime's owner list whose owner class is requiredOwnerId, skipping excludedNode and
   ray-transparent models (MODEL_NODE_FLAG_RAY_TRANSPARENT) and pre-filtering by the depth bin masks of the X and Y
   ranges the ray can reach. Returns true when a model was hit. *outNearestDistanceQ12 always receives the
   nearest distance (MODEL_RAYCAST_NO_HIT_DISTANCE on a miss) and *outNearestModelNode the nearest hit node.
   On a miss *outNearestModelNode is NULL (the original: NULL or a leftover scratch value of the last missing
   hierarchy test); callers only use it after a hit. Used by the army combat code (src/gameplay/army/combat.cpp) and the shot
   updates (src/world/shots/flight.cpp).
*/
bool ModelRuntime_RaycastCandidateListNearest
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,Q12 maximumDistanceQ12,Q12 originZQ12
          ,Q12 originYQ12,Q12 originXQ12,WorldOwnerRuntimeClassId requiredOwnerId,
          ModelRuntimeNode *excludedNode,WorldRuntimeContext *worldRuntime,Q12 *outNearestDistanceQ12,
          ModelRuntimeNode **outNearestModelNode)

{
  ModelRuntimeNode *modelNodeRuntime;
  DepthBinMask32 rayXBinMask;
  DepthBinMask32 rayYBinMask;
  int bestDistanceQ12;
  ModelRuntimeNode *nearestModelNode;
  Q12 hierarchyDistanceQ12;
  ModelRuntimeNode *hierarchyNearestNode;

  g_ModelRaycastOrigin.x = originXQ12;
  g_ModelRaycastOrigin.y = originYQ12;
  g_ModelRaycastOrigin.z = originZQ12;
  g_ModelRaycastMaximumDistance = maximumDistanceQ12;
  rayXBinMask = DepthInterval_BuildBinMask(maximumDistanceQ12,originXQ12);
  rayYBinMask = DepthInterval_BuildBinMask(maximumDistanceQ12,originYQ12);
  FixedMath_WriteDirectionQ28
            (&g_ModelRaycastWorldDirectionQ28,elevationAngle,azimuthAngle);
  nearestModelNode = nullptr;
  bestDistanceQ12 = MODEL_RAYCAST_NO_HIT_DISTANCE;
  for (modelNodeRuntime = WorldNode_View<ModelRuntimeNode>(worldRuntime->ownerListHead.get());
      modelNodeRuntime != nullptr;
      modelNodeRuntime = WorldNode_View<ModelRuntimeNode>(modelNodeRuntime->common.nextNode.get())) {
    if (modelNodeRuntime != excludedNode && modelNodeRuntime->ownerClassId == requiredOwnerId &&
        !Any(modelNodeRuntime->runtimeFlags & MODEL_NODE_FLAG_RAY_TRANSPARENT) &&
        DepthBinMasks_Overlap(modelNodeRuntime->depthBinMaskFar,modelNodeRuntime->depthBinMaskNear,
                              rayYBinMask,rayXBinMask)) {
      hierarchyDistanceQ12 = ModelNodeRuntime_RaycastHierarchyNearest(modelNodeRuntime,&hierarchyNearestNode);
      if (hierarchyDistanceQ12 <= bestDistanceQ12) {
        bestDistanceQ12 = hierarchyDistanceQ12;
        nearestModelNode = hierarchyNearestNode;
      }
    }
  }
  *outNearestModelNode = nearestModelNode;
  *outNearestDistanceQ12 = bestDistanceQ12;
  return bestDistanceQ12 != MODEL_RAYCAST_NO_HIT_DISTANCE;
}
