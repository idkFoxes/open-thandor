/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/model/mesh_raycast.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/model/mesh_raycast.h>
#include <thandor/thandor.h>

/* Module data. */

/* Q12 ray origin in the tested node's frame */
GraphicsFixedVec3 g_ModelRaycastLocalOrigin = {};

/* Q28 ray direction in the tested node's frame */
GraphicsFixedVec3 g_ModelRaycastLocalDirectionQ28 = {};

/* Intersects the current model-space pick ray (g_ModelRaycastLocalOrigin*, g_ModelRaycastLocalDirection*Q28,
   limited to g_ModelRaycastMaximumDistance) with one triangle: first the plane distance along the ray (plane
   through the weighted centre (2*v0 + v1 + v2) / 4), then an inside test of the hit point against the edges.
   Returns true on a hit and stores the Q12 distance along the ray in *outDistanceQ12; returns false on a miss
   and leaves *outDistanceQ12 unchanged. Called for each triangle by ModelNodeRuntime_RaycastHierarchyNearest.
*/
Bool8 ModelMesh_IntersectTriangleRayDistance(ModelRaycastTriangleDescriptor *triangle,Q12 *outDistanceQ12)

{
  GraphicsFixedVec3 *vertex0;
  GraphicsFixedVec3 *vertex1;
  GraphicsFixedVec3 *vertex2;
  int normalX;
  int normalY;
  int normalZ;
  int64_t planeOffsetDot;
  int planeOffsetHigh;
  int halfPlaneOffsetHigh;
  int64_t directionProduct;
  int directionDot;
  int maximumDistanceHigh;
  Bool8 planeOutOfRange;
  Q12 hitDistanceQ12;
  int negVertex0X;
  int negVertex0Y;
  int negVertex0Z;
  int hitRelativeX;
  int hitRelativeY;
  int hitRelativeZ;
  int edge1X;
  int edge1Y;
  int edge1Z;
  int edge2X;
  int edge2Y;
  int edge2Z;
  int normalCrossEdge1X;
  int normalCrossEdge1Y;
  int normalCrossEdge1Z;
  int64_t hitDotNormalCrossEdge1;
  int64_t edge2DotNormalCrossEdge1;
  int64_t normalCrossHitX;
  int64_t normalCrossHitY;
  int64_t normalCrossHitZ;
  int64_t edge2DotNormalCrossHit;
  int64_t insideRemainder;
  Bool8 hitFound;

  normalX = triangle->planeNormalX << (Q28_SHIFT - Q12_SHIFT);
  normalY = triangle->planeNormalY << (Q28_SHIFT - Q12_SHIFT);
  normalZ = triangle->planeNormalZ << (Q28_SHIFT - Q12_SHIFT);
  vertex0 = triangle->vertex0;
  vertex1 = triangle->vertex1;
  vertex2 = triangle->vertex2;
  planeOffsetDot = (int64_t)normalY *
          (int64_t)(((vertex0->y * 2 + vertex1->y + vertex2->y) >> 2) - g_ModelRaycastLocalOrigin.y) +
          (int64_t)(((vertex0->x * 2 + vertex1->x + vertex2->x) >> 2) - g_ModelRaycastLocalOrigin.x) *
          (int64_t)normalX +
          (int64_t)(((vertex0->z * 2 + vertex1->z + vertex2->z) >> 2) - g_ModelRaycastLocalOrigin.z) *
          (int64_t)normalZ;
  planeOffsetHigh = (int)((uint64_t)planeOffsetDot >> 32);
  directionProduct = (int64_t)g_ModelRaycastLocalDirectionQ28.y * (int64_t)normalY +
          (int64_t)g_ModelRaycastLocalDirectionQ28.x * (int64_t)normalX +
          (int64_t)g_ModelRaycastLocalDirectionQ28.z * (int64_t)normalZ;
  directionDot = (int)FIXED_PRODUCT_SHR(directionProduct,Q28_SHIFT);
  if (directionDot == 0) {
    return false;
  }
  maximumDistanceHigh = FIXED_MUL_HIGH((int)g_ModelRaycastMaximumDistance,directionDot);
  halfPlaneOffsetHigh = planeOffsetHigh >> 1;
  /* Range test: the plane distance must lie within the maximum distance on the ray's side. */
  if (planeOffsetDot < 0) {
    planeOutOfRange = planeOffsetHigh < maximumDistanceHigh ||
        (halfPlaneOffsetHigh <= -directionDot && halfPlaneOffsetHigh <= directionDot);
  }
  else {
    planeOutOfRange = maximumDistanceHigh < planeOffsetHigh ||
        (-directionDot <= halfPlaneOffsetHigh && directionDot <= halfPlaneOffsetHigh);
  }
  if (planeOutOfRange) {
    return false;
  }
  hitDistanceQ12 = (int)(planeOffsetDot / (int64_t)directionDot); /* 64-by-32-bit signed division */

  /* hit point relative to vertex 0 */
  negVertex0X = -vertex0->x;
  hitRelativeX = FIXED_MUL_SHR(hitDistanceQ12,g_ModelRaycastLocalDirectionQ28.x,Q28_SHIFT) + g_ModelRaycastLocalOrigin.x + negVertex0X;
  negVertex0Y = -vertex0->y;
  hitRelativeY = FIXED_MUL_SHR(g_ModelRaycastLocalDirectionQ28.y,hitDistanceQ12,Q28_SHIFT) + g_ModelRaycastLocalOrigin.y + negVertex0Y;
  negVertex0Z = -vertex0->z;
  hitRelativeZ = FIXED_MUL_SHR(g_ModelRaycastLocalDirectionQ28.z,hitDistanceQ12,Q28_SHIFT) + g_ModelRaycastLocalOrigin.z + negVertex0Z;
  edge1X = negVertex0X + vertex1->x;
  edge1Y = negVertex0Y + vertex1->y;
  edge1Z = negVertex0Z + vertex1->z;
  edge2X = negVertex0X + vertex2->x;
  edge2Y = negVertex0Y + vertex2->y;
  edge2Z = negVertex0Z + vertex2->z;

  /* normal x edge1 (Q28 normal, so shifted back by 28) */
  normalCrossEdge1X = (int)FIXED_PRODUCT_SHR((int64_t)normalY * (int64_t)edge1Z - (int64_t)normalZ * (int64_t)edge1Y,Q28_SHIFT);
  normalCrossEdge1Y = (int)FIXED_PRODUCT_SHR((int64_t)normalZ * (int64_t)edge1X - (int64_t)normalX * (int64_t)edge1Z,Q28_SHIFT);
  normalCrossEdge1Z = (int)FIXED_PRODUCT_SHR((int64_t)normalX * (int64_t)edge1Y - (int64_t)normalY * (int64_t)edge1X,Q28_SHIFT);
  hitDotNormalCrossEdge1 = (int64_t)hitRelativeY * (int64_t)normalCrossEdge1Y +
                           (int64_t)hitRelativeX * (int64_t)normalCrossEdge1X +
                           (int64_t)hitRelativeZ * (int64_t)normalCrossEdge1Z;
  edge2DotNormalCrossEdge1 = (int64_t)edge2Y * (int64_t)normalCrossEdge1Y +
                             (int64_t)normalCrossEdge1X * (int64_t)edge2X +
                             (int64_t)edge2Z * (int64_t)normalCrossEdge1Z;

  /* normal x hit point, dotted with edge2 */
  normalCrossHitX = (int64_t)normalY * (int64_t)hitRelativeZ - (int64_t)normalZ * (int64_t)hitRelativeY;
  normalCrossHitY = (int64_t)normalZ * (int64_t)hitRelativeX - (int64_t)normalX * (int64_t)hitRelativeZ;
  normalCrossHitZ = (int64_t)normalX * (int64_t)hitRelativeY - (int64_t)normalY * (int64_t)hitRelativeX;
  edge2DotNormalCrossHit = (int64_t)edge2Y * (int64_t)(int)FIXED_PRODUCT_SHR(normalCrossHitY,Q28_SHIFT) +
                           (int64_t)edge2X * (int64_t)(int)FIXED_PRODUCT_SHR(normalCrossHitX,Q28_SHIFT) +
                           (int64_t)edge2Z * (int64_t)(int)FIXED_PRODUCT_SHR(normalCrossHitZ,Q28_SHIFT);

  /* Inside test: both barycentric dots and the remainder edge2DotNormalCrossEdge1 - (their sum) carry the sign
     of edge2DotNormalCrossEdge1 (the 64-bit subtraction wraps like the original's). */
  insideRemainder = (int64_t)((uint64_t)edge2DotNormalCrossEdge1 -
                              (uint64_t)(edge2DotNormalCrossHit + hitDotNormalCrossEdge1));
  if (edge2DotNormalCrossEdge1 < 0) {
    hitFound = edge2DotNormalCrossHit < 0 && hitDotNormalCrossEdge1 < 0 && insideRemainder < 0;
  }
  else {
    hitFound = edge2DotNormalCrossHit >= 0 && hitDotNormalCrossEdge1 >= 0 && insideRemainder >= 0;
  }
  if (!hitFound) {
    return false;
  }
  *outDistanceQ12 = hitDistanceQ12;
  return true;
}
