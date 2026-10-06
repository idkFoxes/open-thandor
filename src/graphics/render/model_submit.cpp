/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/render/model_submit.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* Model mesh submission: projects the vertices of a node's mesh groups, clips and queues the triangles
   (default and alternate path), and accumulates the projected bounds of a hierarchy. */

#include <thandor/graphics/render/model_submit.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Mesh groups, mesh records, vertices and triangles are bytes of the loaded model asset, passed as addresses
   (ModelMeshGroupAddress32) or byte cursors: the reinterpret_casts in this file view those bytes as the record
   structs (ModelMeshGroupHeader, ModelMeshHeader, GraphicsTriangleInput, GraphicsFixedVec3 vertex words). */

/* Module data. */

GraphicsFixedVec3 g_GraphicsTransformInputScratchVec3 = {};

GraphicsFixedVec3 g_GraphicsTransformOutputScratchVec3 = {};

GraphicsFixedMatrix3x4 g_ModelViewCompositeTransform = {};

static GraphicsFixedVec3 g_ModelViewDirectionLocal = {};

static GraphicsFixedVec3 g_ModelViewDirectionWorld = {};

static GraphicsFixedVec3 g_ModelAuxiliaryForwardDirectionLocal = {};

GraphicsFixedMatrix3x4 g_GraphicsTransformScratchMatrix3x4 = {};

/* Draws the chosen level-of-detail mesh group of a model node (from ModelRuntime_CullAndRenderHierarchyRecursive).
   Group flag 1 turns the node towards the viewer and flag 2 makes it a billboard; the node's rotation angles and
   world transform are saved before and restored afterwards, so the turn only lasts for this draw. Every mesh
   whose mask (mesh +4) shares a bit with the node's meshGroupMask is submitted; facingThresholdQ12 is the
   back-face limit ModelRender_SubmitTriangle compares against (the caller derives it from radius / distance).
*/
void ModelRender_DrawMeshGroupsWithTemporaryTransform
          (Q12 facingThresholdQ12,ModelMeshGroupAddress32 meshGroup,ModelRuntimeNode *modelNode)

{
  uint32_t groupFlags;
  uint32_t nodeMeshGroupMask;
  AngleTurn32 savedRotationAngle0;
  AngleTurn32 savedRotationAngle1;
  AngleTurn32 savedRotationAngle2;
  int32_t savedBasis00;
  int32_t savedBasis01;
  int32_t savedBasis02;
  GraphicsWorldCoordinateQ12 savedTranslationX;
  int32_t savedBasis10;
  int32_t savedBasis11;
  int32_t savedBasis12;
  GraphicsWorldCoordinateQ12 savedTranslationY;
  int32_t savedBasis20;
  int32_t savedBasis21;
  int32_t savedBasis22;
  GraphicsWorldCoordinateQ12 savedTranslationZ;
  int remainingMeshCount;
  ModelMeshHeader *meshRecord;

  /* the ModelMeshHeader records follow the ModelMeshGroupHeader, each byteSize long */
  groupFlags = reinterpret_cast<ModelMeshGroupHeader *>(meshGroup)->groupFlags;
  remainingMeshCount = reinterpret_cast<ModelMeshGroupHeader *>(meshGroup)->meshCount;
  savedRotationAngle0 = (modelNode->modelPayload).worldRotationAngle0;
  savedRotationAngle1 = (modelNode->modelPayload).worldRotationAngle1;
  savedRotationAngle2 = (modelNode->modelPayload).worldRotationAngle2;
  savedBasis00 = (modelNode->worldTransform).basisRow0[0];
  savedBasis01 = (modelNode->worldTransform).basisRow0[1];
  savedBasis02 = (modelNode->worldTransform).basisRow0[2];
  savedTranslationX = (modelNode->worldTransform).translation.x;
  savedBasis10 = (modelNode->worldTransform).basisRow1[0];
  savedBasis11 = (modelNode->worldTransform).basisRow1[1];
  savedBasis12 = (modelNode->worldTransform).basisRow1[2];
  savedTranslationY = (modelNode->worldTransform).translation.y;
  savedBasis20 = (modelNode->worldTransform).basisRow2[0];
  savedBasis21 = (modelNode->worldTransform).basisRow2[1];
  savedBasis22 = (modelNode->worldTransform).basisRow2[2];
  savedTranslationZ = (modelNode->worldTransform).translation.z;
  if ((groupFlags & MODEL_MESH_GROUP_FACE_VIEWER) != 0) {
    ModelNodeRuntime_BuildViewFacingRotation(modelNode);
  }
  if ((groupFlags & MODEL_MESH_GROUP_BILLBOARD) != 0) {
    ModelNodeRuntime_BuildBillboardRotation(modelNode);
  }
  meshRecord = reinterpret_cast<ModelMeshHeader *>(reinterpret_cast<ModelMeshGroupHeader *>(meshGroup) + 1);
  nodeMeshGroupMask = (modelNode->modelPayload).meshGroupMask;
  for (; remainingMeshCount != 0; remainingMeshCount--) {
    if ((meshRecord->groupMask & nodeMeshGroupMask) != 0) {
      ModelRender_SubmitMeshTriangles
                (facingThresholdQ12,(ModelMeshGroupAddress32)meshRecord,modelNode);
    }
    meshRecord = reinterpret_cast<ModelMeshHeader *>(reinterpret_cast<uint8_t *>(meshRecord) + meshRecord->byteSize);
  }
  (modelNode->worldTransform).translation.z = savedTranslationZ;
  (modelNode->worldTransform).basisRow2[2] = savedBasis22;
  (modelNode->worldTransform).basisRow2[1] = savedBasis21;
  (modelNode->worldTransform).basisRow2[0] = savedBasis20;
  (modelNode->worldTransform).translation.y = savedTranslationY;
  (modelNode->worldTransform).basisRow1[2] = savedBasis12;
  (modelNode->worldTransform).basisRow1[1] = savedBasis11;
  (modelNode->worldTransform).basisRow1[0] = savedBasis10;
  (modelNode->worldTransform).translation.x = savedTranslationX;
  (modelNode->worldTransform).basisRow0[2] = savedBasis02;
  (modelNode->worldTransform).basisRow0[1] = savedBasis01;
  (modelNode->worldTransform).basisRow0[0] = savedBasis00;
  (modelNode->modelPayload).worldRotationAngle2 = savedRotationAngle2;
  (modelNode->modelPayload).worldRotationAngle1 = savedRotationAngle1;
  (modelNode->modelPayload).worldRotationAngle0 = savedRotationAngle0;
}

/* The same mesh group draw for the alternate model renderer (ModelRuntime_RenderHierarchyRecursiveAlternatePath,
   chosen by the world views with FRONTEND_MODEL_POINTER_CONTEXT_COMPARE_HITS_BY_METRIC_ONLY), without restoring
   the node transform and without a back-face limit: applies the facing / billboard rotation of group flags 1 / 2
   and submits every mesh that matches the node's meshGroupMask.
*/
void ModelRender_DrawMeshGroupsAlternatePath(ModelMeshGroupAddress32 meshGroup,ModelRuntimeNode *modelNode)

{
  uint32_t groupFlags;
  uint32_t nodeMeshGroupMask;
  int remainingMeshCount;
  ModelMeshHeader *meshRecord;

  groupFlags = reinterpret_cast<ModelMeshGroupHeader *>(meshGroup)->groupFlags;
  remainingMeshCount = reinterpret_cast<ModelMeshGroupHeader *>(meshGroup)->meshCount;
  if ((groupFlags & MODEL_MESH_GROUP_FACE_VIEWER) != 0) {
    ModelNodeRuntime_BuildViewFacingRotation(modelNode);
  }
  if ((groupFlags & MODEL_MESH_GROUP_BILLBOARD) != 0) {
    ModelNodeRuntime_BuildBillboardRotation(modelNode);
  }
  meshRecord = reinterpret_cast<ModelMeshHeader *>(reinterpret_cast<ModelMeshGroupHeader *>(meshGroup) + 1);
  nodeMeshGroupMask = (modelNode->modelPayload).meshGroupMask;
  for (; remainingMeshCount != 0; remainingMeshCount--) {
    if ((meshRecord->groupMask & nodeMeshGroupMask) != 0) {
      ModelRender_SubmitMeshTrianglesAlternatePath((ModelMeshGroupAddress32)meshRecord,modelNode);
    }
    meshRecord = reinterpret_cast<ModelMeshHeader *>(reinterpret_cast<uint8_t *>(meshRecord) + meshRecord->byteSize);
  }
}

/* Grows bounds by the projected bounding boxes of a model node and all its descendants, for the selection frame
   of SelectionOverlay_RenderSelectedArmyMetrics and the other overlay code in gameplay/selection/overlay.
*/
void ModelProjectedBounds_AccumulateHierarchyRecursive(ModelProjectedBoundsPixels *bounds,ModelRuntimeNode *modelNode)

{
  uint32_t remainingChildCount;

  ModelProjectedBounds_AccumulateNode(bounds,modelNode);
  for (remainingChildCount = modelNode->childCount; remainingChildCount != 0; remainingChildCount--) {
    if (modelNode->childNodes[0] != nullptr) {
      ModelProjectedBounds_AccumulateHierarchyRecursive(bounds,modelNode->childNodes[0]);
    }
    /* moves the node pointer by one dword, so childNodes[0] reads the next child slot */
    modelNode = reinterpret_cast<ModelRuntimeNode *>(&(modelNode->common).nextNode);
  }
}

/* Transforms and projects one mesh vertex for ModelRender_SubmitTriangle the first time a triangle of this draw
   uses it (projected X = MODEL_VERTEX_NOT_PROJECTED); the node's depth bias and, with
   MODEL_RUNTIME_FLAG_APPLY_SCALE, its draw scale ((v * modelScaleQ12) >> 12) are applied only for the projection.
   It then computes the vertex colour (+0x2C) for the triangle's lighting flags: the node tint when unlit, else
   the default or scaled lighting path with the vertex or (flat shaded) triangle normal. The colour is reused
   while the next triangle has the same MODEL_TRIANGLE_VERTEX_CACHE_FLAGS and is not flat shaded.
*/
void ModelRender_PrepareProjectedVertex
          (ModelRuntimeNode *modelNode,ModelMeshGroupAddress32 triangle,GraphicsFixedVec3 *vertex)

{
  /* vertex: [0] local position, +0x10 normal, +0x1C packed colour, +0x20 view position, +0x2C lit colour,
     +0x30/+0x34 projected X/Y, +0x38 the flags the lit colour was computed for */
  uint32_t triangleRenderFlags;
  int64_t scaledCoordinateProduct;
  PackedArgb32 vertexColor;
  uint32_t depthBiasHalf;
  GraphicsFixedVec3 *surfaceNormalQ12;
  GraphicsProjectedPointPair projectedScreenPoint;
  GraphicsWorldCoordinateQ12 savedVertexXQ12;
  GraphicsWorldCoordinateQ12 savedVertexZQ12;
  GraphicsWorldCoordinateQ12 savedVertexYQ12;

  triangleRenderFlags = reinterpret_cast<GraphicsTriangleInput *>(triangle)->renderFlags;
  if (vertex[4].x == MODEL_VERTEX_NOT_PROJECTED) {
    savedVertexXQ12 = vertex->x;
    savedVertexYQ12 = vertex->y;
    savedVertexZQ12 = vertex->z;
    depthBiasHalf = (uint32_t)modelNode->renderDepthBiasOrState >> 1;
    /* half the depth bias for z == 0, the full bias for z > 0 */
    if (vertex->z >= 0) {
      if (vertex->z != 0) {
        vertex->z = vertex->z + depthBiasHalf;
      }
      vertex->z = vertex->z + depthBiasHalf;
    }
    if ((modelNode->runtimeFlags & MODEL_RUNTIME_FLAG_APPLY_SCALE) != 0) {
      /* 64-bit product >> 12 */
      scaledCoordinateProduct = (int64_t)vertex->x * (int64_t)modelNode->modelScaleQ12;
      vertex->x = FIXED_PRODUCT_SHR(scaledCoordinateProduct, Q12_SHIFT);
      scaledCoordinateProduct = (int64_t)vertex->y * (int64_t)modelNode->modelScaleQ12;
      vertex->y = FIXED_PRODUCT_SHR(scaledCoordinateProduct, Q12_SHIFT);
      scaledCoordinateProduct = (int64_t)vertex->z * (int64_t)modelNode->modelScaleQ12;
      vertex->z = FIXED_PRODUCT_SHR(scaledCoordinateProduct, Q12_SHIFT);
    }
    FixedTransform_ApplyPoint
              (ModelVertex_ViewPosition(vertex),vertex,
               &g_ModelViewCompositeTransform);
    projectedScreenPoint = Graphics_ProjectViewPoint(ModelVertex_ViewPosition(vertex));
    vertex->z = savedVertexZQ12;
    vertex->y = savedVertexYQ12;
    vertex->x = savedVertexXQ12;
    vertex[4].x = projectedScreenPoint.projectedX;
    vertex[4].y = projectedScreenPoint.projectedY;
  }
  else if (((triangleRenderFlags & MODEL_TRIANGLE_VERTEX_CACHE_FLAGS) == vertex[4].z) &&
          ((triangleRenderFlags & MODEL_TRIANGLE_FLAT_SHADED) == 0)) {
    return;
  }
  vertex[4].z = triangleRenderFlags & MODEL_TRIANGLE_VERTEX_CACHE_FLAGS;
  vertexColor = modelNode->tintArgb;
  surfaceNormalQ12 = ModelVertex_Normal(vertex);
  if ((triangleRenderFlags & MODEL_TRIANGLE_UNLIT) != 0) {
    vertex[3].z = vertexColor;
    return;
  }
  if ((triangleRenderFlags & MODEL_TRIANGLE_LIGHTING_SCALED) == 0) {
    if ((triangleRenderFlags & MODEL_TRIANGLE_FLAT_SHADED) != 0) {
      /* the triangle's plane normal */
      surfaceNormalQ12 = ModelTriangle_PlaneNormal(reinterpret_cast<GraphicsTriangleInput *>(triangle));
    }
    vertexColor = ModelRender_ComputeVertexIntensityDefaultPath
                      (vertex[2].y,&vertex[2].z,THANDOR_ADDR(g_ModelLightingMmxMultiplierRows[MODEL_DISTANCE_ATTENUATION_ROW0],0),g_SceneBoundsFixed.bound5,
                       g_SceneBoundsFixed.bound4,
                       &g_ModelAuxiliaryForwardDirectionLocal,vertexColor,
                       surfaceNormalQ12);
    vertex[3].z = vertexColor;
    return;
  }
  /* the scaled path gets the vertex position as its normal, as in the original */
  vertexColor = ModelRender_ComputeVertexIntensityScaledPath
                    (vertex[2].y,&vertex[2].z,
                     ((modelNode->modelPayload).modelResource)->lightingScaleQ12,
                     g_SceneBoundsFixed.bound7,g_SceneBoundsFixed.bound6,
                     &g_ModelAuxiliaryForwardDirectionLocal,vertexColor,vertex);
  vertex[3].z = vertexColor;
}

/* True unless all three projected vertices lie beyond the same edge of g_ProjectionClipRect (shared by
   ModelRender_SubmitTriangle and ModelRender_SubmitTriangleAlternatePath). */
static Bool8 ModelRender_TriangleOverlapsClipRect
          (const GraphicsProjectedVertexSource *firstVertex,const GraphicsProjectedVertexSource *secondVertex,
           const GraphicsProjectedVertexSource *thirdVertex)
{
  if (!(g_ProjectionClipRect.minX <= firstVertex->screenX || g_ProjectionClipRect.minX <= secondVertex->screenX ||
        g_ProjectionClipRect.minX <= thirdVertex->screenX)) {
    return false;
  }
  if (!(firstVertex->screenX < g_ProjectionClipRect.maxX || secondVertex->screenX < g_ProjectionClipRect.maxX ||
        thirdVertex->screenX < g_ProjectionClipRect.maxX)) {
    return false;
  }
  if (!(g_ProjectionClipRect.minY <= firstVertex->screenY || g_ProjectionClipRect.minY <= secondVertex->screenY ||
        g_ProjectionClipRect.minY <= thirdVertex->screenY)) {
    return false;
  }
  return firstVertex->screenY < g_ProjectionClipRect.maxY || secondVertex->screenY < g_ProjectionClipRect.maxY ||
         thirdVertex->screenY < g_ProjectionClipRect.maxY;
}

/* Node flag 0x80 scrolls the texture coordinates of the just queued triangle when it uses the node's primary
   animated subresource; with flag 0x400 as well, also when it uses the secondary one. */
static void ModelRender_ApplyTextureScroll(const ModelRuntimeNode *modelNode,GraphicsSubresourceIndex triangleSubresource)
{
  if ((modelNode->runtimeFlags & MODEL_RUNTIME_FLAG_PRIMARY_TEXTURE_SCROLL) == 0) {
    return;
  }
  if (triangleSubresource == modelNode->primaryAnimatedSubresourceIndex) {
    GraphicsPrimitiveQueue_OffsetTextureCoordinates
              (modelNode->primaryTextureOffsetV,modelNode->primaryTextureOffsetU,g_ActivePrimitiveQueue);
  }
  if (((modelNode->runtimeFlags & MODEL_RUNTIME_FLAG_SECONDARY_TEXTURE_SCROLL) != 0) &&
      (triangleSubresource == modelNode->secondaryAnimatedSubresourceIndex)) {
    GraphicsPrimitiveQueue_OffsetTextureCoordinates
              (modelNode->secondaryTextureOffsetV,modelNode->secondaryTextureOffsetU,g_ActivePrimitiveQueue);
  }
}

/* Submits one mesh triangle of ModelRender_SubmitMeshTriangles: skips it when it faces away (facing dot not
   below facingThresholdQ12, unless MODEL_TRIANGLE_DOUBLE_SIDED), projects and lights its three vertices, drops it
   when all three lie beyond the same edge of g_ProjectionClipRect, and otherwise appends it to the active
   primitive queue with its vertex colours, texture (node texture set, subresource + node base index) and palette
   material colour (MODEL_TRIANGLE_PALETTE_BANK_MASK, 0xFFFFFFFF without one). Node flag 0x80 scrolls the
   texture coordinates of the node's one or (flag 0x400) two animated subresources.
*/
void ModelRender_SubmitTriangle(Q12 facingThresholdQ12,GraphicsTriangleInput *triangle,ModelRuntimeNode *modelNode)

{
  GraphicsProjectedVertexSource *firstVertex;
  GraphicsProjectedVertexSource *secondVertex;
  GraphicsProjectedVertexSource *thirdVertex;
  GraphicsTextureSet *nodeTextureSet;
  GraphicsPaletteAsset *nodePaletteAsset;
  int32_t facingDotQ12;
  uint32_t paletteBankIndex;
  PackedArgb32 materialColor;
  Bool8 appendFailed;
  GraphicsTextureSetEntry *textureEntry;

  facingDotQ12 = ModelRender_ComputeFacingDotQ12(triangle);
  if ((triangle->renderFlags & MODEL_TRIANGLE_DOUBLE_SIDED) == 0 && facingDotQ12 >= facingThresholdQ12) {
    return;
  }
  firstVertex = triangle->vertex0;
  secondVertex = triangle->vertex1;
  thirdVertex = triangle->vertex2;
  ModelRender_PrepareProjectedVertex
            (modelNode,(ModelMeshGroupAddress32)triangle,ModelVertex_Words(firstVertex));
  ModelRender_PrepareProjectedVertex
            (modelNode,(ModelMeshGroupAddress32)triangle,ModelVertex_Words(secondVertex));
  ModelRender_PrepareProjectedVertex
            (modelNode,(ModelMeshGroupAddress32)triangle,ModelVertex_Words(thirdVertex));
  if (!ModelRender_TriangleOverlapsClipRect(firstVertex,secondVertex,thirdVertex)) {
    return;
  }
  appendFailed = GraphicsPrimitiveQueue_AppendTriangle
                    (triangle->renderFlags,triangle,triangle->vertex2,triangle->vertex1,
                     triangle->vertex0,g_ActivePrimitiveQueue);
  if (appendFailed) {
    return;
  }
  GraphicsPrimitiveQueue_SetVertexColors
            (triangle->vertex2->vertexColorArgb,triangle->vertex1->vertexColorArgb,
             triangle->vertex0->vertexColorArgb,g_ActivePrimitiveQueue);
  nodeTextureSet = (modelNode->modelPayload).textureSet;
  textureEntry = nullptr;
  if ((nodeTextureSet != nullptr) &&
     (triangle->subresourceIndex < nodeTextureSet->subresourceCount)) {
    textureEntry = nodeTextureSet->entries +
                   triangle->subresourceIndex + modelNode->textureSubresourceBaseIndex;
  }
  /* the palette bank's colour, opaque white without a palette or with a bank out of range */
  materialColor = ARGB8888_OPAQUE_WHITE;
  nodePaletteAsset = (modelNode->modelPayload).paletteAsset;
  if (nodePaletteAsset != nullptr) {
    paletteBankIndex = triangle->renderFlags & MODEL_TRIANGLE_PALETTE_BANK_MASK;
    if (paletteBankIndex < nodePaletteAsset->paletteBankCount) {
      materialColor = nodePaletteAsset->paletteEntries[paletteBankIndex].argb8888;
    }
  }
  GraphicsPrimitiveQueue_SetMaterial(materialColor,textureEntry,g_ActivePrimitiveQueue);
  ModelRender_ApplyTextureScroll(modelNode,triangle->subresourceIndex);
}

/* Draws one mesh of ModelRender_DrawMeshGroupsWithTemporaryTransform: prepares the model-space view directions,
   marks the mesh's vertices as not projected for this draw, then submits its triangles (stored after the
   vertices) through ModelRender_SubmitTriangle.
*/
void ModelRender_SubmitMeshTriangles
          (Q12 facingThresholdQ12,ModelMeshGroupAddress32 meshGroup,ModelRuntimeNode *modelNode)

{
  int remainingCount;
  GraphicsTriangleInput *recordCursor;

  remainingCount = reinterpret_cast<ModelMeshHeader *>(meshGroup)->vertexCount;
  ModelRender_PrepareViewDirections(modelNode);
  recordCursor = reinterpret_cast<GraphicsTriangleInput *>(reinterpret_cast<ModelMeshHeader *>(meshGroup) + 1);
  for (; remainingCount != 0; remainingCount--) {
    recordCursor->subresourceIndex = MODEL_VERTEX_NOT_PROJECTED; /* vertex +0x30: projected X */
    recordCursor = reinterpret_cast<GraphicsTriangleInput *>(reinterpret_cast<uint8_t *>(recordCursor) + MODEL_MESH_RECORD_SIZE);
  }
  for (remainingCount = reinterpret_cast<ModelMeshHeader *>(meshGroup)->triangleCount; remainingCount != 0; remainingCount--) {
    ModelRender_SubmitTriangle(facingThresholdQ12,recordCursor,modelNode);
    recordCursor = reinterpret_cast<GraphicsTriangleInput *>(reinterpret_cast<uint8_t *>(recordCursor) + MODEL_MESH_RECORD_SIZE);
  }
}

/* Draws one mesh of ModelRender_DrawMeshGroupsAlternatePath: the same as ModelRender_SubmitMeshTriangles, but
   through ModelRender_SubmitTriangleAlternatePath.
*/
void ModelRender_SubmitMeshTrianglesAlternatePath(ModelMeshGroupAddress32 meshGroup,ModelRuntimeNode *modelNode)

{
  int remainingCount;
  GraphicsTriangleInput *recordCursor;

  remainingCount = reinterpret_cast<ModelMeshHeader *>(meshGroup)->vertexCount;
  ModelRender_PrepareViewDirections(modelNode);
  recordCursor = reinterpret_cast<GraphicsTriangleInput *>(reinterpret_cast<ModelMeshHeader *>(meshGroup) + 1);
  for (; remainingCount != 0; remainingCount--) {
    recordCursor->subresourceIndex = MODEL_VERTEX_NOT_PROJECTED; /* vertex +0x30: projected X */
    recordCursor = reinterpret_cast<GraphicsTriangleInput *>(reinterpret_cast<uint8_t *>(recordCursor) + MODEL_MESH_RECORD_SIZE);
  }
  for (remainingCount = reinterpret_cast<ModelMeshHeader *>(meshGroup)->triangleCount; remainingCount != 0; remainingCount--) {
    ModelRender_SubmitTriangleAlternatePath(recordCursor,modelNode);
    recordCursor = reinterpret_cast<GraphicsTriangleInput *>(reinterpret_cast<uint8_t *>(recordCursor) + MODEL_MESH_RECORD_SIZE);
  }
}

/* Vertex preparation of the alternate model renderer (ModelRender_SubmitTriangleAlternatePath): transforms and
   projects the vertex once per draw, but rejects it (returns true, projected X = 0x7FFFFFFF) when it lies in front of
   the near plane (view z < g_ProjectionScaleFixed). The colour is lit per vertex by nearby lights
   (ModelRender_ComputeNearbyLightPackedVertexColorAlternatePath), or white with the tint's alpha when unlit,
   and cached like in ModelRender_PrepareProjectedVertex.
*/
Bool8 ModelRender_PrepareProjectedVertexAlternatePath
          (ModelRuntimeNode *modelNode,GraphicsTriangleInput *triangle,GraphicsFixedVec3 *vertex)

{
  uint32_t triangleRenderFlags;
  uint32_t materialPackedColor;
  PackedArgb32 vertexColor;
  GraphicsFixedVec3 *surfaceNormalQ12;
  GraphicsProjectedPointPair projectedPoint;

  triangleRenderFlags = triangle->renderFlags;
  if (vertex[4].x == MODEL_VERTEX_NOT_PROJECTED) {
    FixedTransform_ApplyPoint
              (ModelVertex_ViewPosition(vertex),vertex,
               &g_ModelViewCompositeTransform);
    if (vertex[3].y < (int)g_ProjectionScaleFixed) {
      vertex[4].x = MODEL_VERTEX_NEAR_CLIPPED;
      return true;
    }
    projectedPoint = Graphics_ProjectViewPoint(ModelVertex_ViewPosition(vertex));
    vertex[4].x = projectedPoint.projectedX;
    vertex[4].y = projectedPoint.projectedY;
  }
  else {
    if (vertex[4].x == MODEL_VERTEX_NEAR_CLIPPED) {
      /* Already marked as behind the near plane (the original re-stores the same marker). */
      return true;
    }
    if (((triangleRenderFlags & MODEL_TRIANGLE_VERTEX_CACHE_FLAGS) == vertex[4].z) &&
        ((triangleRenderFlags & MODEL_TRIANGLE_FLAT_SHADED) == 0)) {
      return false;
    }
  }
  materialPackedColor = modelNode->tintArgb;
  vertex[4].z = triangleRenderFlags & MODEL_TRIANGLE_VERTEX_CACHE_FLAGS;
  if ((triangleRenderFlags & MODEL_TRIANGLE_UNLIT) == 0) {
    surfaceNormalQ12 = ModelVertex_Normal(vertex);
    if ((triangleRenderFlags & MODEL_TRIANGLE_FLAT_SHADED) != 0) {
      surfaceNormalQ12 = ModelTriangle_PlaneNormal(triangle);
    }
    vertexColor = ModelRender_ComputeNearbyLightPackedVertexColorAlternatePath
                      (vertex[2].y,ModelVertex_ViewPosition(vertex),g_SceneBoundsFixed.bound5,
                       materialPackedColor,surfaceNormalQ12);
    vertex[3].z = vertexColor;
    return false;
  }
  vertex[3].z = materialPackedColor | ARGB8888_RGB_MASK;
  return false;
}

/* Triangle submission of the alternate model renderer (from ModelRender_SubmitMeshTrianglesAlternatePath):
   no back-face test; the triangle is dropped when a vertex lies in front of the near plane or all three lie
   beyond the same edge of g_ProjectionClipRect. Otherwise it is queued with its vertex colours, texture and the
   palette's alternate modulation colour (0 without one).
*/
void ModelRender_SubmitTriangleAlternatePath(GraphicsTriangleInput *triangle,ModelRuntimeNode *modelNode)

{
  GraphicsProjectedVertexSource *firstVertex;
  GraphicsProjectedVertexSource *secondVertex;
  GraphicsProjectedVertexSource *thirdVertex;
  GraphicsTextureSet *nodeTextureSet;
  GraphicsPaletteAsset *nodePaletteAsset;
  GraphicsSubresourceIndex subresourceIndex;
  uint32_t paletteBankIndex;
  PackedArgb32 modulationColor;
  Bool8 appendFailed;
  GraphicsTextureSetEntry *textureEntry;

  firstVertex = triangle->vertex0;
  secondVertex = triangle->vertex1;
  thirdVertex = triangle->vertex2;
  /* a vertex in front of the near plane drops the triangle; the remaining vertices are not prepared */
  if (ModelRender_PrepareProjectedVertexAlternatePath(modelNode,triangle,ModelVertex_Words(firstVertex)) ||
      ModelRender_PrepareProjectedVertexAlternatePath(modelNode,triangle,ModelVertex_Words(secondVertex)) ||
      ModelRender_PrepareProjectedVertexAlternatePath(modelNode,triangle,ModelVertex_Words(thirdVertex))) {
    return;
  }
  if (!ModelRender_TriangleOverlapsClipRect(firstVertex,secondVertex,thirdVertex)) {
    return;
  }
  appendFailed = GraphicsPrimitiveQueue_AppendTriangle
                    (triangle->renderFlags,triangle,triangle->vertex2,triangle->vertex1,
                     triangle->vertex0,g_ActivePrimitiveQueue);
  if (appendFailed) {
    return;
  }
  GraphicsPrimitiveQueue_SetVertexColors
            (triangle->vertex2->vertexColorArgb,triangle->vertex1->vertexColorArgb,
             triangle->vertex0->vertexColorArgb,g_ActivePrimitiveQueue);
  nodeTextureSet = (modelNode->modelPayload).textureSet;
  subresourceIndex = triangle->subresourceIndex;
  textureEntry = nullptr;
  /* unlike ModelRender_SubmitTriangle there is no NULL check of the texture set */
  if (subresourceIndex != UINT32_MAX &&
      subresourceIndex < nodeTextureSet->subresourceCount) {
    textureEntry = nodeTextureSet->entries + subresourceIndex +
                   modelNode->textureSubresourceBaseIndex;
  }
  /* the palette bank's alternate modulation colour, 0 without a palette or with a bank out of range */
  modulationColor = 0;
  nodePaletteAsset = (modelNode->modelPayload).paletteAsset;
  if (nodePaletteAsset != nullptr) {
    /* the original masks with 0xFFFF01FF here (0x1FF in ModelRender_SubmitTriangle) */
    paletteBankIndex = triangle->renderFlags & MODEL_TRIANGLE_PALETTE_BANK_WIDE_MASK;
    if (paletteBankIndex < nodePaletteAsset->paletteBankCount) {
      modulationColor = nodePaletteAsset->paletteEntries[paletteBankIndex].alternateModulationColorArgb;
    }
  }
  GraphicsPrimitiveQueue_SetMaterial(modulationColor,textureEntry,g_ActivePrimitiveQueue);
}

/* Grows bounds by the screen projection of the eight corners of a model node's local bounding box (for
   ModelProjectedBounds_AccumulateHierarchyRecursive). Nodes without a bounding radius or with
   MODEL_RESOURCE_DISABLE_PROJECTED_HIT_TEST are skipped.
*/
void ModelProjectedBounds_AccumulateNode(ModelProjectedBoundsPixels *bounds,ModelRuntimeNode *modelNode)

{
  ModelResource *resource;
  GraphicsWorldCoordinateQ12 boundsX0Q12;
  GraphicsWorldCoordinateQ12 boundsX1Q12;

  resource = (modelNode->modelPayload).modelResource;
  if ((resource->boundingRadiusQ12 != 0) &&
     ((resource->hitTestFlags20C & MODEL_RESOURCE_DISABLE_PROJECTED_HIT_TEST) == 0)) {
    FixedTransform_Compose
              (&g_GraphicsTransformScratchMatrix3x4,&modelNode->worldTransform,
               &g_ViewProjectionMatrixFixed);
    boundsX0Q12 = resource->localBoundsX0Q12;
    boundsX1Q12 = resource->localBoundsX1Q12;
    g_GraphicsTransformInputScratchVec3.y = resource->localBoundsY0Q12;
    g_GraphicsTransformInputScratchVec3.z = resource->localBoundsZ0Q12;
    g_GraphicsTransformInputScratchVec3.x = boundsX0Q12;
    ModelProjectedBounds_ExpandWithCurrentScratchPoint(bounds);
    g_GraphicsTransformInputScratchVec3.x = boundsX1Q12;
    ModelProjectedBounds_ExpandWithCurrentScratchPoint(bounds);
    g_GraphicsTransformInputScratchVec3.y = resource->localBoundsY1Q12;
    g_GraphicsTransformInputScratchVec3.x = boundsX0Q12;
    ModelProjectedBounds_ExpandWithCurrentScratchPoint(bounds);
    g_GraphicsTransformInputScratchVec3.x = boundsX1Q12;
    ModelProjectedBounds_ExpandWithCurrentScratchPoint(bounds);
    g_GraphicsTransformInputScratchVec3.y = resource->localBoundsY0Q12;
    g_GraphicsTransformInputScratchVec3.z = resource->localBoundsZ1Q12;
    g_GraphicsTransformInputScratchVec3.x = boundsX0Q12;
    ModelProjectedBounds_ExpandWithCurrentScratchPoint(bounds);
    g_GraphicsTransformInputScratchVec3.x = boundsX1Q12;
    ModelProjectedBounds_ExpandWithCurrentScratchPoint(bounds);
    g_GraphicsTransformInputScratchVec3.y = resource->localBoundsY1Q12;
    g_GraphicsTransformInputScratchVec3.x = boundsX0Q12;
    ModelProjectedBounds_ExpandWithCurrentScratchPoint(bounds);
    g_GraphicsTransformInputScratchVec3.x = boundsX1Q12;
    ModelProjectedBounds_ExpandWithCurrentScratchPoint(bounds);
  }
}

/* Back-face measure of a triangle for ModelRender_SubmitTriangle: the Q12 dot product of its plane normal
   (planeNormalXQ12..planeNormalZQ12) with the model-space view direction from ModelRender_PrepareViewDirections.
*/
int32_t ModelRender_ComputeFacingDotQ12(GraphicsTriangleInput *triangle)

{
  int32_t facingDotQ12;

  facingDotQ12 = FixedVec3_DotQ12(ModelTriangle_PlaneNormal(triangle),
                                  &g_ModelViewDirectionLocal);
  return facingDotQ12;
}

/* Transforms the point in g_GraphicsTransformInputScratchVec3 with g_GraphicsTransformScratchMatrix3x4 and, when
   it lies beyond the near plane, grows bounds by its projected pixel position (Q12 >> 12). Used by
   ModelProjectedBounds_AccumulateNode for each bounding-box corner.
*/
void ModelProjectedBounds_ExpandWithCurrentScratchPoint(ModelProjectedBoundsPixels *bounds)

{
  int pixelX;
  int pixelY;
  GraphicsProjectedPointPair projectedPoint;

  FixedTransform_ApplyPoint
            (&g_GraphicsTransformOutputScratchVec3,&g_GraphicsTransformInputScratchVec3,
             &g_GraphicsTransformScratchMatrix3x4);
  if ((int)g_ProjectionScaleFixed < g_GraphicsTransformOutputScratchVec3.z) {
    projectedPoint = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
    pixelX = projectedPoint.projectedX >> Q12_SHIFT;
    pixelY = projectedPoint.projectedY >> Q12_SHIFT;
    if (pixelX < bounds->minX) {
      bounds->minX = pixelX;
    }
    if (pixelY < bounds->minY) {
      bounds->minY = pixelY;
    }
    if (bounds->maxX < pixelX) {
      bounds->maxX = pixelX;
    }
    if (bounds->maxY < pixelY) {
      bounds->maxY = pixelY;
    }
  }
}

/* Per-mesh setup of ModelRender_SubmitMeshTriangles and ModelRender_SubmitMeshTrianglesAlternatePath: composes
   the node's world transform with the view projection into g_ModelViewCompositeTransform, and brings the
   direction from the viewer to the node (for the back-face test) and the auxiliary forward direction (for the
   lighting) into model space.
*/
void ModelRender_PrepareViewDirections(ModelRuntimeNode *modelNodeRuntime)

{
  int nodeWorldX;
  int nodeWorldY;
  int nodeWorldZ;
  GraphicsFixedMatrix3x4 *nodeWorldTransform;
  FixedVectorAngles viewAngles;

  nodeWorldX = (modelNodeRuntime->worldTransform).translation.x;
  nodeWorldY = (modelNodeRuntime->worldTransform).translation.y;
  nodeWorldZ = (modelNodeRuntime->worldTransform).translation.z;
  nodeWorldTransform = &modelNodeRuntime->worldTransform;
  FixedTransform_Compose
            (&g_ModelViewCompositeTransform,nodeWorldTransform,
             &g_ViewProjectionMatrixFixed);
  viewAngles = FixedMath_VectorToAngles
                    (nodeWorldZ - g_ViewOriginFixed.z,nodeWorldY - g_ViewOriginFixed.y,
                     nodeWorldX - g_ViewOriginFixed.x);
  FixedMath_WriteDirectionQ28
            (&g_ModelViewDirectionWorld,viewAngles.elevationAngle,viewAngles.azimuthAngle);
  FixedTransform_ApplyTransposeDirection
            (&g_ModelViewDirectionLocal,nodeWorldTransform,
             &g_ModelViewDirectionWorld);
  FixedTransform_ApplyTransposeDirection
            (&g_ModelAuxiliaryForwardDirectionLocal,nodeWorldTransform,
             &g_AuxiliaryForwardDirectionFixed);
}
