/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/render/model_draw.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* Model hierarchy drawing: view culling, level of detail and projection of the nodes of a model runtime
   hierarchy before their meshes are submitted. */

#include <thandor/graphics/render/model_draw.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

static GraphicsFixedVec3 g_GraphicsDirectionWorld = {0};

int32_t g_ModelLodDepthThresholdQ8 = 65536;

GraphicsFixedVec3 g_ModelCullViewRelative = {0};

/* Part of ModelRuntime_CullAndRenderHierarchyRecursive: the node passed the four side planes with its own
   radius; g_ModelCullViewRelative holds its view-relative position. Projects it and, when it lies fully in
   front of the near plane, collects the nearby shading records and draws the mesh group picked by depth.
   Returns false when the node's depth is not beyond the near plane (the walk then also skips its children). */
static Bool8 ModelRuntime_ProjectAndDrawNode(ModelRuntimeNode *modelNodeRuntime)

{
  ModelResource *renderView;
  uint32_t viewDistance;
  uint32_t boundingRadius;
  uint32_t meshGroupCount;
  ModelMeshGroupRelativeOffset *meshGroup;
  Q12 projectedRadiusScale;

  renderView = modelNodeRuntime->modelPayload.modelResource;
  viewDistance = FixedMath_Length3(g_ModelCullViewRelative.z,g_ModelCullViewRelative.y,g_ModelCullViewRelative.x);
  boundingRadius = renderView->boundingRadiusQ12;
  /* radius / distance in Q28, 1.0 when the view origin is inside the bounding sphere */
  if ((int)viewDistance < (int)boundingRadius) {
    projectedRadiusScale = Q28_ONE;
  }
  else {
    /* unsigned division */
    projectedRadiusScale = (Q12)(((uint64_t)boundingRadius << 28) / (uint64_t)viewDistance);
  }
  FixedTransform_ApplyPoint
            (&g_ModelCullViewRelative,
             &modelNodeRuntime->worldTransform.translation,
             &g_ViewProjectionMatrixFixed);
  renderView = modelNodeRuntime->modelPayload.modelResource;
  /* g_ModelCullViewRelative.z is now the view depth; g_ProjectionScaleFixed is the near plane */
  if (g_ModelCullViewRelative.z <= g_ProjectionScaleFixed) {
    return false;
  }
  if (g_ModelCullViewRelative.z - g_ProjectionScaleFixed != renderView->boundingRadiusQ12 &&
      renderView->boundingRadiusQ12 <= g_ModelCullViewRelative.z - g_ProjectionScaleFixed) {
    modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | MODEL_NODE_FLAG_RENDERED;
    g_GraphicsShadingNearbyRecordCount = 0;
    GraphicsShadingRuntime_CollectNearbyRecords
              (renderView->boundingRadiusQ12,g_ModelCullViewRelative.z,
               g_ModelCullViewRelative.y,g_ModelCullViewRelative.x);
    renderView = modelNodeRuntime->modelPayload.modelResource;
    meshGroupCount = renderView->meshGroupCount;
    meshGroup = &renderView->firstMeshGroupRelativeOffset;
    /* level of detail: the next mesh group beyond g_ModelLodDepthThresholdQ8, the third beyond twice that depth
       (each group starts with the offset to the next) */
    if ((uint32_t)g_ModelLodDepthThresholdQ8 < (uint32_t)g_ModelCullViewRelative.z && 1 < meshGroupCount) {
      meshGroup = (ModelMeshGroupRelativeOffset *)((uint8_t *)meshGroup + *meshGroup);
      if ((uint32_t)g_ModelLodDepthThresholdQ8 < (uint32_t)(g_ModelCullViewRelative.z >> 1) &&
          2 < meshGroupCount) {
        meshGroup = (ModelMeshGroupRelativeOffset *)((uint8_t *)meshGroup + *meshGroup);
      }
    }
    ModelRender_DrawMeshGroupsWithTemporaryTransform
              (projectedRadiusScale,(ModelMeshGroupAddress32)meshGroup,modelNodeRuntime);
  }
  return true;
}

/* Part of ModelRuntime_CullAndRenderHierarchyRecursive: culls the node (view-relative position in
   g_ModelCullViewRelative) against the four side planes of the view frustum and draws it when it passes.
   A plane distance above the subtree radius means the whole subtree is outside: returns false and the walk
   ends here. Above only the node radius, just the node is culled and the children are still visited. */
static Bool8 ModelRuntime_CullAndDrawNode(ModelRuntimeNode *modelNodeRuntime)

{
  int subtreeRadius;
  int nodeRadius;
  int planeIndex;
  int32_t planeDistance;

  subtreeRadius = modelNodeRuntime->subtreeBoundingRadiusQ12 + modelNodeRuntime->renderDepthBiasOrState;
  nodeRadius = modelNodeRuntime->modelPayload.modelResource->boundingRadiusQ12 +
               modelNodeRuntime->renderDepthBiasOrState;
  for (planeIndex = 0; planeIndex < 4; planeIndex++) {
    planeDistance = FixedVec3_DotQ28(g_FrustumPlaneNormalFixed_0 + planeIndex,
                                     &g_ModelCullViewRelative);
    if (subtreeRadius < planeDistance) {
      return false;
    }
    if (nodeRadius < planeDistance) {
      return true;
    }
  }
  return ModelRuntime_ProjectAndDrawNode(modelNodeRuntime);
}

/* Renders a model node and its children for the main view: clears the node's MODEL_NODE_FLAG_RENDERED, culls it
   against the four side planes of the view frustum and the near plane, and draws it when it lies fully in front
   of the near plane, picking the level of detail by depth. A node outside a plane by more than its subtree radius
   ends the walk; otherwise the children are visited even when the node itself was culled. Called for every
   model by the offscreen preview renderer (src/graphics/render/projection.c) and by the frontend/in-game world
   view (src/ui/frontend/runtime.c).
*/
void ModelRuntime_CullAndRenderHierarchyRecursive(ModelRuntimeNode *modelNodeRuntime)

{
  uint32_t childrenRemaining;
  int childIndex;

  if (modelNodeRuntime == nullptr) {
    return;
  }
  modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags & ~MODEL_NODE_FLAG_RENDERED;
  g_ModelCullViewRelative.x =
       modelNodeRuntime->worldTransform.translation.x - g_ViewOriginFixed.x;
  g_ModelCullViewRelative.y =
       modelNodeRuntime->worldTransform.translation.y - g_ViewOriginFixed.y;
  g_ModelCullViewRelative.z =
       modelNodeRuntime->worldTransform.translation.z - g_ViewOriginFixed.z;
  if (!ModelRuntime_CullAndDrawNode(modelNodeRuntime)) {
    return;
  }
  childIndex = 0;
  for (childrenRemaining = modelNodeRuntime->childCount; childrenRemaining != 0; childrenRemaining--) {
    if (modelNodeRuntime->childNodes[childIndex] != nullptr) {
      ModelRuntime_CullAndRenderHierarchyRecursive(modelNodeRuntime->childNodes[childIndex]);
    }
    childIndex++;
  }
}

/* Alternate model renderer of the frontend/in-game world view (src/ui/frontend/runtime.c, chosen when the
   pointer context compares hits by metric only): draws a node and all its children without culling. The centre
   of the node's local bounds is transformed into g_ModelCullViewRelative to collect the nearby shading
   records; every drawn node gets MODEL_NODE_FLAG_RENDERED.
*/
void ModelRuntime_RenderHierarchyRecursiveAlternatePath(ModelRuntimeNode *modelNode)

{
  ModelResource *modelResourceView;
  uint32_t boundsDiagonalLength;
  uint32_t childrenRemaining;
  int childIndex;

  /* The original read the resource before the NULL test; read after it here, so the compiler cannot drop the
     test (a dereference before it lets it assume a non-NULL node) */
  if (modelNode != nullptr) {
    modelResourceView = modelNode->modelPayload.modelResource;
    modelNode->runtimeFlags = modelNode->runtimeFlags | MODEL_NODE_FLAG_RENDERED;
    /* g_GraphicsDirectionWorld only serves as scratch vector here */
    g_GraphicsDirectionWorld.x = (modelResourceView->localBoundsX0Q12 + modelResourceView->localBoundsX1Q12) >> 1;
    g_GraphicsDirectionWorld.y = (modelResourceView->localBoundsY0Q12 + modelResourceView->localBoundsY1Q12) >> 1;
    g_GraphicsDirectionWorld.z = (modelResourceView->localBoundsZ0Q12 + modelResourceView->localBoundsZ1Q12) >> 1;
    FixedTransform_ApplyPoint
              (&g_ModelCullViewRelative,&g_GraphicsDirectionWorld,
               &g_ViewProjectionMatrixFixed);
    boundsDiagonalLength =
         FixedMath_Length3(modelResourceView->localBoundsZ1Q12 - modelResourceView->localBoundsZ0Q12,
                           modelResourceView->localBoundsY1Q12 - modelResourceView->localBoundsY0Q12,
                           modelResourceView->localBoundsX1Q12 - modelResourceView->localBoundsX0Q12);
    GraphicsShadingRuntime_CollectNearbyRecords
              ((int)boundsDiagonalLength >> 1,g_ModelCullViewRelative.z,g_ModelCullViewRelative.y,
               g_ModelCullViewRelative.x);
    ModelRender_DrawMeshGroupsAlternatePath(modelNode->runtimeStateA0,modelNode);
    childIndex = 0;
    for (childrenRemaining = modelNode->childCount; childrenRemaining != 0; childrenRemaining--) {
      if (modelNode->childNodes[childIndex] != nullptr) {
        ModelRuntime_RenderHierarchyRecursiveAlternatePath(modelNode->childNodes[childIndex]);
      }
      childIndex++;
    }
  }
}
