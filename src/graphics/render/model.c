/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/render/model.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/render/model.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Implementation ownership: graphics/render/model. */


/* MMX lane helpers for the rewritten lighting routines (Intel SDM semantics). These are C helpers, not
   functions of the original executable. */

/* movd + punpcklbw mm,mm + psrlw mm,shift: each byte b becomes the word (b * 0x101) >> shift. */
static void ModelLighting_UnpackBytes(uint32_t packed, int shift, short lanes[4])
{
  int i;
  for (i = 0; i < 4; i++) {
    lanes[i] = (short)((((packed >> (8 * i)) & ARGB8888_CHANNEL_MASK) * COLOR_CHANNEL_TO_WORD_LANE) >> shift);
  }
}

/* pmulhw */
static void ModelLighting_MulHigh(short lanes[4], const short factors[4])
{
  int i;
  for (i = 0; i < 4; i++) {
    lanes[i] = (short)(((int)lanes[i] * (int)factors[i]) >> 16);
  }
}

/* paddsw */
static void ModelLighting_AddSaturate(short lanes[4], const short addends[4])
{
  int i;
  for (i = 0; i < 4; i++) {
    int sum = (int)lanes[i] + (int)addends[i];
    lanes[i] = (short)(sum > 32767 ? 32767 : sum < -32768 ? -32768 : sum);
  }
}

/* packuswb mm,mm + movd */
static PackedArgb32 ModelLighting_PackUnsigned(const short lanes[4])
{
  uint32_t packed = 0;
  int i;
  for (i = 0; i < 4; i++) {
    int v = lanes[i] < 0 ? 0 : lanes[i] > ARGB8888_CHANNEL_MAX ? ARGB8888_CHANNEL_MAX : lanes[i];
    packed |= (uint32_t)v << (8 * i);
  }
  return packed;
}

/* The same lane operations on 64-bit MMX register images (ThandorMmx, core/ghidra.h). */

/* movd + punpcklbw mm,mm + psrlw mm,shift: byte k of packed becomes word lane k = (b * 0x101) >> shift. */
static __inline uint64_t ModelLighting_UnpackBytesMmx(uint32_t packed, int shift)
{
  ThandorMmx lanes;
  int i;
  for (i = 0; i < 4; i++) {
    lanes.uw[i] = (uint16_t)((((packed >> (8 * i)) & ARGB8888_CHANNEL_MASK) * COLOR_CHANNEL_TO_WORD_LANE) >> shift);
  }
  return lanes.q;
}

/* paddw (wrapping word add) */
static __inline uint64_t ModelLighting_AddWordsMmx(uint64_t a, uint64_t b)
{
  ThandorMmx x, y, r;
  int i;
  x.q = a;
  y.q = b;
  for (i = 0; i < 4; i++) {
    r.uw[i] = (uint16_t)(x.uw[i] + y.uw[i]);
  }
  return r.q;
}

/* packuswb mm,mm + movd */
static __inline PackedArgb32 ModelLighting_PackUnsignedMmx(uint64_t lanes)
{
  ThandorMmx x;
  x.q = lanes;
  return ModelLighting_PackUnsigned(x.sw);
}


/* Address: 0x004BDC90.
   Draws the chosen level-of-detail mesh group of a model node (from ModelRuntime_CullAndRenderHierarchyRecursive).
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
  groupFlags = ((ModelMeshGroupHeader *)meshGroup)->groupFlags;
  remainingMeshCount = ((ModelMeshGroupHeader *)meshGroup)->meshCount;
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
  meshRecord = (ModelMeshHeader *)((ModelMeshGroupHeader *)meshGroup + 1);
  nodeMeshGroupMask = (modelNode->modelPayload).meshGroupMask;
  for (; remainingMeshCount != 0; remainingMeshCount--) {
    if ((meshRecord->groupMask & nodeMeshGroupMask) != 0) {
      ModelRender_SubmitMeshTriangles
                (facingThresholdQ12,(ModelMeshGroupAddress32)meshRecord,modelNode);
    }
    meshRecord = (ModelMeshHeader *)((uint8_t *)meshRecord + meshRecord->byteSize);
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


/* Address: 0x004BE1F0.
   The same mesh group draw for the alternate model renderer (ModelRuntime_RenderHierarchyRecursiveAlternatePath,
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

  groupFlags = ((ModelMeshGroupHeader *)meshGroup)->groupFlags;
  remainingMeshCount = ((ModelMeshGroupHeader *)meshGroup)->meshCount;
  if ((groupFlags & MODEL_MESH_GROUP_FACE_VIEWER) != 0) {
    ModelNodeRuntime_BuildViewFacingRotation(modelNode);
  }
  if ((groupFlags & MODEL_MESH_GROUP_BILLBOARD) != 0) {
    ModelNodeRuntime_BuildBillboardRotation(modelNode);
  }
  meshRecord = (ModelMeshHeader *)((ModelMeshGroupHeader *)meshGroup + 1);
  nodeMeshGroupMask = (modelNode->modelPayload).meshGroupMask;
  for (; remainingMeshCount != 0; remainingMeshCount--) {
    if ((meshRecord->groupMask & nodeMeshGroupMask) != 0) {
      ModelRender_SubmitMeshTrianglesAlternatePath((ModelMeshGroupAddress32)meshRecord,modelNode);
    }
    meshRecord = (ModelMeshHeader *)((uint8_t *)meshRecord + meshRecord->byteSize);
  }
}


/* Address: 0x0050A5C0.
   Grows bounds by the projected bounding boxes of a model node and all its descendants, for the selection frame
   of SelectionOverlay_RenderSelectedArmyMetrics and the other overlay code in gameplay/selection/overlay.
*/
void ModelProjectedBounds_AccumulateHierarchyRecursive(ModelProjectedBoundsPixels *bounds,ModelRuntimeNode *modelNode)

{
  uint32_t remainingChildCount;

  ModelProjectedBounds_AccumulateNode(bounds,modelNode);
  for (remainingChildCount = modelNode->childCount; remainingChildCount != 0; remainingChildCount--) {
    if (modelNode->childNodes[0] != NULL) {
      ModelProjectedBounds_AccumulateHierarchyRecursive(bounds,modelNode->childNodes[0]);
    }
    /* moves the node pointer by one dword, so childNodes[0] reads the next child slot */
    modelNode = (ModelRuntimeNode *)&(modelNode->common).nextNode;
  }
}


/* Address: 0x004BD4B0.
   Transforms and projects one mesh vertex for ModelRender_SubmitTriangle the first time a triangle of this draw
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

  triangleRenderFlags = ((GraphicsTriangleInput *)triangle)->renderFlags;
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
      /* 64-bit product >> 12 (IMUL + SHRD) */
      scaledCoordinateProduct = (int64_t)vertex->x * (int64_t)modelNode->modelScaleQ12;
      vertex->x = FIXED_PRODUCT_SHR(scaledCoordinateProduct, Q12_SHIFT);
      scaledCoordinateProduct = (int64_t)vertex->y * (int64_t)modelNode->modelScaleQ12;
      vertex->y = FIXED_PRODUCT_SHR(scaledCoordinateProduct, Q12_SHIFT);
      scaledCoordinateProduct = (int64_t)vertex->z * (int64_t)modelNode->modelScaleQ12;
      vertex->z = FIXED_PRODUCT_SHR(scaledCoordinateProduct, Q12_SHIFT);
    }
    FixedTransform_ApplyPoint
              ((GraphicsFixedVec3 *)&vertex[2].z,vertex,
               &g_ModelViewCompositeTransform);
    projectedScreenPoint = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)&vertex[2].z);
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
  surfaceNormalQ12 = (GraphicsFixedVec3 *)&vertex[1].y;
  if ((triangleRenderFlags & MODEL_TRIANGLE_UNLIT) != 0) {
    vertex[3].z = vertexColor;
    return;
  }
  if ((triangleRenderFlags & MODEL_TRIANGLE_LIGHTING_SCALED) == 0) {
    if ((triangleRenderFlags & MODEL_TRIANGLE_FLAT_SHADED) != 0) {
      /* the triangle's plane normal */
      surfaceNormalQ12 = (GraphicsFixedVec3 *)&((GraphicsTriangleInput *)triangle)->planeNormalXQ12;
    }
    vertexColor = ModelRender_ComputeVertexIntensityDefaultPath
                      (vertex[2].y,&vertex[2].z,THANDOR_ADDR(g_ModelLightingMmxMultiplierRows[MODEL_DISTANCE_ATTENUATION_ROW0],0),g_SceneBoundsFixed.bound5,
                       g_SceneBoundsFixed.bound4,
                       &g_ModelAuxiliaryForwardDirectionLocal,vertexColor,
                       surfaceNormalQ12);
    vertex[3].z = vertexColor;
    return;
  }
  /* the scaled path gets the vertex position as its normal (LEA EDX,[ESI] at 0x004BD636) */
  vertexColor = ModelRender_ComputeVertexIntensityScaledPath
                    (vertex[2].y,&vertex[2].z,
                     ((modelNode->modelPayload).modelResource)->lightingScaleQ12,
                     g_SceneBoundsFixed.bound7,g_SceneBoundsFixed.bound6,
                     &g_ModelAuxiliaryForwardDirectionLocal,vertexColor,vertex);
  vertex[3].z = vertexColor;
}


/* True unless all three projected vertices lie beyond the same edge of g_ProjectionClipRect (shared by
   ModelRender_SubmitTriangle and ModelRender_SubmitTriangleAlternatePath). */
static bool ModelRender_TriangleOverlapsClipRect
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

/* Address: 0x004BD9B0.
   Submits one mesh triangle of ModelRender_SubmitMeshTriangles: skips it when it faces away (facing dot not
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
  bool appendFailed;
  GraphicsTextureSetEntry *textureEntry;

  facingDotQ12 = ModelRender_ComputeFacingDotQ12(triangle);
  if ((triangle->renderFlags & MODEL_TRIANGLE_DOUBLE_SIDED) == 0 && facingDotQ12 >= facingThresholdQ12) {
    return;
  }
  firstVertex = triangle->vertex0;
  secondVertex = triangle->vertex1;
  thirdVertex = triangle->vertex2;
  ModelRender_PrepareProjectedVertex
            (modelNode,(ModelMeshGroupAddress32)triangle,(GraphicsFixedVec3 *)firstVertex);
  ModelRender_PrepareProjectedVertex
            (modelNode,(ModelMeshGroupAddress32)triangle,(GraphicsFixedVec3 *)secondVertex);
  ModelRender_PrepareProjectedVertex
            (modelNode,(ModelMeshGroupAddress32)triangle,(GraphicsFixedVec3 *)thirdVertex);
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
  textureEntry = NULL;
  if ((nodeTextureSet != NULL) &&
     (triangle->subresourceIndex < nodeTextureSet->subresourceCount)) {
    textureEntry = nodeTextureSet->entries +
                   triangle->subresourceIndex + modelNode->textureSubresourceBaseIndex;
  }
  /* the palette bank's colour, opaque white without a palette or with a bank out of range */
  materialColor = ARGB8888_OPAQUE_WHITE;
  nodePaletteAsset = (modelNode->modelPayload).paletteAsset;
  if (nodePaletteAsset != NULL) {
    paletteBankIndex = triangle->renderFlags & MODEL_TRIANGLE_PALETTE_BANK_MASK;
    if (paletteBankIndex < nodePaletteAsset->paletteBankCount) {
      materialColor = nodePaletteAsset->paletteEntries[paletteBankIndex].argb8888;
    }
  }
  GraphicsPrimitiveQueue_SetMaterial(materialColor,textureEntry,g_ActivePrimitiveQueue);
  ModelRender_ApplyTextureScroll(modelNode,triangle->subresourceIndex);
}


/* Address: 0x004BDC20.
   Draws one mesh of ModelRender_DrawMeshGroupsWithTemporaryTransform: prepares the model-space view directions,
   marks the mesh's vertices as not projected for this draw, then submits its triangles (stored after the
   vertices) through ModelRender_SubmitTriangle.
*/
void ModelRender_SubmitMeshTriangles
          (Q12 facingThresholdQ12,ModelMeshGroupAddress32 meshGroup,ModelRuntimeNode *modelNode)

{
  int remainingCount;
  GraphicsTriangleInput *recordCursor;

  remainingCount = ((ModelMeshHeader *)meshGroup)->vertexCount;
  ModelRender_PrepareViewDirections(modelNode);
  recordCursor = (GraphicsTriangleInput *)((ModelMeshHeader *)meshGroup + 1);
  for (; remainingCount != 0; remainingCount--) {
    recordCursor->subresourceIndex = MODEL_VERTEX_NOT_PROJECTED; /* vertex +0x30: projected X */
    recordCursor = (GraphicsTriangleInput *)((uint8_t *)recordCursor + MODEL_MESH_RECORD_SIZE);
  }
  for (remainingCount = ((ModelMeshHeader *)meshGroup)->triangleCount; remainingCount != 0; remainingCount--) {
    ModelRender_SubmitTriangle(facingThresholdQ12,recordCursor,modelNode);
    recordCursor = (GraphicsTriangleInput *)((uint8_t *)recordCursor + MODEL_MESH_RECORD_SIZE);
  }
}


/* Address: 0x004BE180.
   Draws one mesh of ModelRender_DrawMeshGroupsAlternatePath: the same as ModelRender_SubmitMeshTriangles, but
   through ModelRender_SubmitTriangleAlternatePath.
*/
void ModelRender_SubmitMeshTrianglesAlternatePath(ModelMeshGroupAddress32 meshGroup,ModelRuntimeNode *modelNode)

{
  int remainingCount;
  GraphicsTriangleInput *recordCursor;

  remainingCount = ((ModelMeshHeader *)meshGroup)->vertexCount;
  ModelRender_PrepareViewDirections(modelNode);
  recordCursor = (GraphicsTriangleInput *)((ModelMeshHeader *)meshGroup + 1);
  for (; remainingCount != 0; remainingCount--) {
    recordCursor->subresourceIndex = MODEL_VERTEX_NOT_PROJECTED; /* vertex +0x30: projected X */
    recordCursor = (GraphicsTriangleInput *)((uint8_t *)recordCursor + MODEL_MESH_RECORD_SIZE);
  }
  for (remainingCount = ((ModelMeshHeader *)meshGroup)->triangleCount; remainingCount != 0; remainingCount--) {
    ModelRender_SubmitTriangleAlternatePath(recordCursor,modelNode);
    recordCursor = (GraphicsTriangleInput *)((uint8_t *)recordCursor + MODEL_MESH_RECORD_SIZE);
  }
}


/* Address: 0x004BD6B0.
   Vertex preparation of the alternate model renderer (ModelRender_SubmitTriangleAlternatePath): transforms and
   projects the vertex once per draw, but rejects it (CF set, projected X = 0x7FFFFFFF) when it lies in front of
   the near plane (view z < g_ProjectionScaleFixed). The colour is lit per vertex by nearby lights
   (ModelRender_ComputeNearbyLightPackedVertexColorAlternatePath), or white with the tint's alpha when unlit,
   and cached like in ModelRender_PrepareProjectedVertex.
*/
bool ModelRender_PrepareProjectedVertexAlternatePath
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
              ((GraphicsFixedVec3 *)&vertex[2].z,vertex,
               &g_ModelViewCompositeTransform);
    if (vertex[3].y < (int)g_ProjectionScaleFixed) {
      vertex[4].x = MODEL_VERTEX_NEAR_CLIPPED;
      return true;
    }
    projectedPoint = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)&vertex[2].z);
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
    surfaceNormalQ12 = (GraphicsFixedVec3 *)&vertex[1].y;
    if ((triangleRenderFlags & MODEL_TRIANGLE_FLAT_SHADED) != 0) {
      surfaceNormalQ12 = (GraphicsFixedVec3 *)&triangle->planeNormalXQ12;
    }
    vertexColor = ModelRender_ComputeNearbyLightPackedVertexColorAlternatePath
                      (vertex[2].y,(GraphicsFixedVec3 *)&vertex[2].z,g_SceneBoundsFixed.bound5,
                       materialPackedColor,surfaceNormalQ12);
    vertex[3].z = vertexColor;
    return false;
  }
  vertex[3].z = materialPackedColor | ARGB8888_RGB_MASK;
  return false;
}


/* Address: 0x004BDFB0.
   Triangle submission of the alternate model renderer (from ModelRender_SubmitMeshTrianglesAlternatePath):
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
  bool appendFailed;
  GraphicsTextureSetEntry *textureEntry;

  firstVertex = triangle->vertex0;
  secondVertex = triangle->vertex1;
  thirdVertex = triangle->vertex2;
  /* a vertex in front of the near plane drops the triangle; the remaining vertices are not prepared */
  if (ModelRender_PrepareProjectedVertexAlternatePath(modelNode,triangle,(GraphicsFixedVec3 *)firstVertex) ||
      ModelRender_PrepareProjectedVertexAlternatePath(modelNode,triangle,(GraphicsFixedVec3 *)secondVertex) ||
      ModelRender_PrepareProjectedVertexAlternatePath(modelNode,triangle,(GraphicsFixedVec3 *)thirdVertex)) {
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
  textureEntry = NULL;
  /* unlike ModelRender_SubmitTriangle there is no NULL check of the texture set */
  if (subresourceIndex != UINT32_MAX &&
      subresourceIndex < nodeTextureSet->subresourceCount) {
    textureEntry = nodeTextureSet->entries + subresourceIndex +
                   modelNode->textureSubresourceBaseIndex;
  }
  /* the palette bank's alternate modulation colour, 0 without a palette or with a bank out of range */
  modulationColor = 0;
  nodePaletteAsset = (modelNode->modelPayload).paletteAsset;
  if (nodePaletteAsset != NULL) {
    /* the original masks with 0xFFFF01FF here (0x1FF in ModelRender_SubmitTriangle) */
    paletteBankIndex = triangle->renderFlags & MODEL_TRIANGLE_PALETTE_BANK_WIDE_MASK;
    if (paletteBankIndex < nodePaletteAsset->paletteBankCount) {
      modulationColor = nodePaletteAsset->paletteEntries[paletteBankIndex].alternateModulationColorArgb;
    }
  }
  GraphicsPrimitiveQueue_SetMaterial(modulationColor,textureEntry,g_ActivePrimitiveQueue);
}


/* Address: 0x0050A4A0.
   Grows bounds by the screen projection of the eight corners of a model node's local bounding box (for
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


/* Address: 0x004BD7E0.
   Back-face measure of a triangle for ModelRender_SubmitTriangle: the Q12 dot product of its plane normal
   (+0x24) with the model-space view direction from ModelRender_PrepareViewDirections.
*/
int32_t ModelRender_ComputeFacingDotQ12(GraphicsTriangleInput *triangle)

{
  int32_t facingDotQ12;

  facingDotQ12 = FixedVec3_DotQ12((GraphicsFixedVec3 *)&triangle->planeNormalXQ12,
                                  &g_ModelViewDirectionLocal);
  return facingDotQ12;
}


/* Original quirk (0x004CC742 PMULHW MM0,qword ptr [EDX + EAX*8]): ModelRender_ComputeVertexIntensityDefaultPath
   reads its directional weight at distanceAttenuationTable (0x004CB1A0, row MODEL_DISTANCE_ATTENUATION_ROW0 of
   g_ModelLightingMmxMultiplierRows) + (dot >> 21) * 8 without a range check. FixedVec3_DotQ12 (0x004856B0) returns
   the low dword of the 64-bit sum >> 12 (SHRD), so dot is any int32 and the row offset any value in -1024..1023:
   the original reads 8 bytes anywhere in 0x004C91A0..0x004CD1A0, while the table covers only offsets -136..682.
   With the Q28 light direction (g_ModelAuxiliaryForwardDirectionLocal) and a normal of L units (Q12) the offset is
   floor(128 * L * cos): it leaves the table for L * cos < -1.0625 or > 5.33 and the dot wraps from |L * cos| >= 8
   (normals that are not unit length in the model data, or a node transform whose basis lengthens the direction).
   ModelLighting_ReadOriginalImageQword rebuilds those bytes from the variables that hold the original addresses
   today (live values) and the original machine code and 0x90 filler between them (constant, copied from the
   original executable's .text; nothing writes there), so the result does not depend on the linker's layout.
   Each original byte comes from its own range, so a qword spanning two variables (0x004CAD50, 0x004CC700) is
   assembled like the original read. */
#define MODEL_LIGHTING_MMX_ROWS_ORIGINAL_ADDRESS 0x004CAD60u

/* original 0x004CAD58..0x004CAD60: 0x90 filler between g_GraphicsShadingNearbyRecordCount and the table */
static const uint8_t s_ModelLightingOriginalFiller004CAD58[0x8] = {
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 /* 004CAD58 */
};
/* original 0x004CC710..0x004CCE00: machine code of 0x004CC710 (ModelRender_ComputeVertexIntensityDefaultPath),
   0x004CC820 (ModelRender_ComputeVertexIntensityScaledPath) and the following shading routines up to 0x004CCDEB,
   then 0x90 filler up to 0x004CCE00 */
static const uint8_t s_ModelLightingOriginalCode004CC710[0x6F0] = {
  0x53, 0x51, 0x52, 0x57, 0x56, 0x55, 0x89, 0xE5, 0xFF, 0x75, 0x38, 0xFF, 0x75, 0x30, 0xE8, 0x8D, /* 004CC710 */
  0x8F, 0xFB, 0xFF, 0x0F, 0x6E, 0x45, 0x2C, 0xC1, 0xF8, 0x15, 0x0F, 0x60, 0xC0, 0x8B, 0x55, 0x24, /* 004CC720 */
  0x0F, 0x71, 0xD0, 0x02, 0x0F, 0x6E, 0x55, 0x28, 0x0F, 0x6E, 0x5D, 0x34, 0x0F, 0x60, 0xD2, 0x0F, /* 004CC730 */
  0x60, 0xDB, 0x0F, 0xE5, 0x04, 0xC2, 0x0F, 0x71, 0xD2, 0x04, 0x0F, 0x71, 0xD3, 0x02, 0x8B, 0x0D, /* 004CC740 */
  0x54, 0xAD, 0x4C, 0x00, 0x0F, 0xFD, 0xC2, 0xBF, 0x54, 0x6D, 0x4C, 0x00, 0x0F, 0xE5, 0xC3, 0x0F, /* 004CC750 */
  0x6E, 0x6D, 0x1C, 0x85, 0xC9, 0x0F, 0x84, 0x8C, 0x00, 0x00, 0x00, 0x8B, 0x75, 0x20, 0x90, 0x90, /* 004CC760 */
  0x83, 0x7F, 0x20, 0x00, 0x0F, 0x84, 0x70, 0x00, 0x00, 0x00, 0x51, 0x8B, 0x86, 0x00, 0x00, 0x00, /* 004CC770 */
  0x00, 0x8B, 0x5F, 0x10, 0x8B, 0x4F, 0x14, 0x2B, 0x87, 0x00, 0x00, 0x00, 0x00, 0xF7, 0xE8, 0x2B, /* 004CC780 */
  0xD8, 0x1B, 0xCA, 0x78, 0x54, 0x8B, 0x86, 0x04, 0x00, 0x00, 0x00, 0x2B, 0x87, 0x04, 0x00, 0x00, /* 004CC790 */
  0x00, 0xF7, 0xE8, 0x2B, 0xD8, 0x1B, 0xCA, 0x78, 0x40, 0x8B, 0x86, 0x08, 0x00, 0x00, 0x00, 0x2B, /* 004CC7A0 */
  0x87, 0x08, 0x00, 0x00, 0x00, 0xF7, 0xE8, 0x2B, 0xD8, 0x1B, 0xCA, 0x78, 0x2C, 0x0F, 0xA4, 0xD9, /* 004CC7B0 */
  0x1B, 0x8B, 0x57, 0x10, 0x8B, 0x5F, 0x14, 0x8B, 0xC1, 0x0F, 0x6E, 0x67, 0x0C, 0x0F, 0xA4, 0xD3, /* 004CC7C0 */
  0x14, 0x74, 0x16, 0x0F, 0x60, 0xE4, 0x33, 0xD2, 0x0F, 0x71, 0xD4, 0x02, 0xF7, 0xF3, 0x0F, 0xE5, /* 004CC7D0 */
  0x24, 0xC5, 0x80, 0xDE, 0x41, 0x00, 0x0F, 0xDC, 0xC4, 0x59, 0x81, 0xC7, 0x40, 0x00, 0x00, 0x00, /* 004CC7E0 */
  0x49, 0x0F, 0x85, 0x79, 0xFF, 0xFF, 0xFF, 0x0F, 0x60, 0xED, 0x0F, 0x71, 0xD5, 0x02, 0x0F, 0xE5, /* 004CC7F0 */
  0xC5, 0x0F, 0x67, 0xC0, 0x0F, 0x7E, 0xC0, 0x89, 0xEC, 0x5D, 0x5E, 0x5F, 0x5A, 0x59, 0x5B, 0xC2, /* 004CC800 */
  0x20, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, /* 004CC810 */
  0x53, 0x51, 0x52, 0x57, 0x56, 0x55, 0x89, 0xE5, 0xFF, 0x75, 0x38, 0xFF, 0x75, 0x30, 0xE8, 0x7D, /* 004CC820 */
  0x8E, 0xFB, 0xFF, 0x99, 0xF7, 0x7D, 0x24, 0x0F, 0x6E, 0x45, 0x2C, 0xC1, 0xF8, 0x09, 0x0F, 0x60, /* 004CC830 */
  0xC0, 0x0F, 0x71, 0xD0, 0x02, 0x0F, 0x6E, 0x55, 0x28, 0x0F, 0x6E, 0x5D, 0x34, 0x0F, 0x60, 0xD2, /* 004CC840 */
  0x0F, 0x60, 0xDB, 0x0F, 0xE5, 0x04, 0xC5, 0xB0, 0xC2, 0x4C, 0x00, 0x0F, 0x71, 0xD2, 0x04, 0x0F, /* 004CC850 */
  0x71, 0xD3, 0x02, 0x8B, 0x0D, 0x54, 0xAD, 0x4C, 0x00, 0x0F, 0xFD, 0xC2, 0xBF, 0x54, 0x6D, 0x4C, /* 004CC860 */
  0x00, 0x0F, 0xE5, 0xC3, 0x0F, 0x6E, 0x6D, 0x1C, 0x85, 0xC9, 0x0F, 0x84, 0x97, 0x00, 0x00, 0x00, /* 004CC870 */
  0x8B, 0x75, 0x20, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, /* 004CC880 */
  0x83, 0x7F, 0x20, 0x00, 0x0F, 0x84, 0x70, 0x00, 0x00, 0x00, 0x51, 0x8B, 0x86, 0x00, 0x00, 0x00, /* 004CC890 */
  0x00, 0x8B, 0x5F, 0x10, 0x8B, 0x4F, 0x14, 0x2B, 0x87, 0x00, 0x00, 0x00, 0x00, 0xF7, 0xE8, 0x2B, /* 004CC8A0 */
  0xD8, 0x1B, 0xCA, 0x78, 0x54, 0x8B, 0x86, 0x04, 0x00, 0x00, 0x00, 0x2B, 0x87, 0x04, 0x00, 0x00, /* 004CC8B0 */
  0x00, 0xF7, 0xE8, 0x2B, 0xD8, 0x1B, 0xCA, 0x78, 0x40, 0x8B, 0x86, 0x08, 0x00, 0x00, 0x00, 0x2B, /* 004CC8C0 */
  0x87, 0x08, 0x00, 0x00, 0x00, 0xF7, 0xE8, 0x2B, 0xD8, 0x1B, 0xCA, 0x78, 0x2C, 0x0F, 0xA4, 0xD9, /* 004CC8D0 */
  0x1B, 0x8B, 0x57, 0x10, 0x8B, 0x5F, 0x14, 0x8B, 0xC1, 0x0F, 0x6E, 0x67, 0x0C, 0x0F, 0xA4, 0xD3, /* 004CC8E0 */
  0x14, 0x74, 0x16, 0x0F, 0x60, 0xE4, 0x33, 0xD2, 0x0F, 0x71, 0xD4, 0x02, 0xF7, 0xF3, 0x0F, 0xE5, /* 004CC8F0 */
  0x24, 0xC5, 0x80, 0xDE, 0x41, 0x00, 0x0F, 0xDC, 0xC4, 0x59, 0x81, 0xC7, 0x40, 0x00, 0x00, 0x00, /* 004CC900 */
  0x49, 0x0F, 0x85, 0x79, 0xFF, 0xFF, 0xFF, 0x0F, 0x60, 0xED, 0x0F, 0x71, 0xD5, 0x02, 0x0F, 0xE5, /* 004CC910 */
  0xC5, 0x0F, 0x67, 0xC0, 0x0F, 0x7E, 0xC0, 0x89, 0xEC, 0x5D, 0x5E, 0x5F, 0x5A, 0x59, 0x5B, 0xC2, /* 004CC920 */
  0x20, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, /* 004CC930 */
  0x53, 0x51, 0x52, 0x57, 0x56, 0x55, 0x89, 0xE5, 0x81, 0x4D, 0x24, 0x00, 0x00, 0x00, 0xFF, 0x68, /* 004CC940 */
  0x50, 0xD4, 0x4B, 0x00, 0xFF, 0x75, 0x2C, 0x68, 0x04, 0xC7, 0x4C, 0x00, 0xE8, 0xAF, 0x85, 0xFB, /* 004CC950 */
  0xFF, 0x0F, 0x6E, 0x45, 0x24, 0x0F, 0x6E, 0x5D, 0x28, 0x0F, 0x60, 0xC0, 0x0F, 0x60, 0xDB, 0x0F, /* 004CC960 */
  0x71, 0xD0, 0x03, 0x0F, 0x71, 0xD3, 0x03, 0x8B, 0x0D, 0x54, 0xAD, 0x4C, 0x00, 0xBF, 0x54, 0x6D, /* 004CC970 */
  0x4C, 0x00, 0x0F, 0xE5, 0xC3, 0x0F, 0x6E, 0x6D, 0x1C, 0x85, 0xC9, 0x0F, 0x84, 0xDD, 0x00, 0x00, /* 004CC980 */
  0x00, 0x8B, 0x75, 0x20, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, /* 004CC990 */
  0x83, 0x7F, 0x20, 0x00, 0x0F, 0x84, 0xB7, 0x00, 0x00, 0x00, 0x51, 0x8B, 0x87, 0x00, 0x00, 0x00, /* 004CC9A0 */
  0x00, 0x2B, 0x86, 0x00, 0x00, 0x00, 0x00, 0xA3, 0xF8, 0xC6, 0x4C, 0x00, 0xF7, 0xE8, 0x8B, 0xD8, /* 004CC9B0 */
  0x8B, 0xCA, 0x8B, 0x87, 0x04, 0x00, 0x00, 0x00, 0x2B, 0x86, 0x04, 0x00, 0x00, 0x00, 0xA3, 0xFC, /* 004CC9C0 */
  0xC6, 0x4C, 0x00, 0xF7, 0xE8, 0x03, 0xD8, 0x13, 0xCA, 0x8B, 0x87, 0x08, 0x00, 0x00, 0x00, 0x2B, /* 004CC9D0 */
  0x86, 0x08, 0x00, 0x00, 0x00, 0xA3, 0x00, 0xC7, 0x4C, 0x00, 0xF7, 0xE8, 0x03, 0xD8, 0x13, 0xCA, /* 004CC9E0 */
  0x8B, 0x57, 0x14, 0x8B, 0x47, 0x10, 0x3B, 0xCA, 0x7F, 0x66, 0x75, 0x04, 0x3B, 0xD8, 0x73, 0x60, /* 004CC9F0 */
  0x0F, 0xA4, 0xD9, 0x03, 0xC1, 0xE3, 0x03, 0x03, 0xD8, 0x13, 0xCA, 0x0F, 0xA4, 0xC2, 0x11, 0x0F, /* 004CCA00 */
  0xA4, 0xD9, 0x0C, 0x74, 0x4B, 0x8D, 0x14, 0xD2, 0x68, 0xF8, 0xC6, 0x4C, 0x00, 0x68, 0xF8, 0xC6, /* 004CCA10 */
  0x4C, 0x00, 0xE8, 0x79, 0x8D, 0xFB, 0xFF, 0x68, 0x04, 0xC7, 0x4C, 0x00, 0x68, 0xF8, 0xC6, 0x4C, /* 004CCA20 */
  0x00, 0xE8, 0x7A, 0x8C, 0xFB, 0xFF, 0x33, 0xDB, 0x85, 0xC0, 0x78, 0x02, 0x8B, 0xD8, 0x8B, 0xC2, /* 004CCA30 */
  0x0F, 0x6E, 0x67, 0x0C, 0x0F, 0x60, 0xE4, 0x33, 0xD2, 0x0F, 0x71, 0xD4, 0x02, 0xF7, 0xF9, 0xF7, /* 004CCA40 */
  0xEB, 0x0F, 0xAC, 0xD0, 0x1C, 0x0F, 0xE5, 0x24, 0xC5, 0x80, 0xDE, 0x41, 0x00, 0x0F, 0xED, 0xC4, /* 004CCA50 */
  0x59, 0x81, 0xC7, 0x40, 0x00, 0x00, 0x00, 0x49, 0x0F, 0x85, 0x32, 0xFF, 0xFF, 0xFF, 0x0F, 0x60, /* 004CCA60 */
  0xED, 0x0F, 0x71, 0xD5, 0x02, 0x0F, 0xE5, 0xC5, 0x0F, 0x67, 0xC0, 0x0F, 0x7E, 0xC0, 0x89, 0xEC, /* 004CCA70 */
  0x5D, 0x5E, 0x5F, 0x5A, 0x59, 0x5B, 0xC2, 0x14, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, /* 004CCA80 */
  0x53, 0x51, 0x52, 0x57, 0x56, 0x8B, 0x0D, 0x50, 0x6D, 0x4C, 0x00, 0xBF, 0x50, 0x2D, 0x4C, 0x00, /* 004CCA90 */
  0x85, 0xC9, 0x0F, 0x84, 0x8F, 0x00, 0x00, 0x00, 0x8B, 0xF2, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, /* 004CCAA0 */
  0x83, 0x7F, 0x20, 0x00, 0x0F, 0x84, 0x70, 0x00, 0x00, 0x00, 0x51, 0x8B, 0x86, 0x00, 0x00, 0x00, /* 004CCAB0 */
  0x00, 0x8B, 0x5F, 0x10, 0x8B, 0x4F, 0x14, 0x2B, 0x87, 0x00, 0x00, 0x00, 0x00, 0xF7, 0xE8, 0x2B, /* 004CCAC0 */
  0xD8, 0x1B, 0xCA, 0x78, 0x54, 0x8B, 0x86, 0x04, 0x00, 0x00, 0x00, 0x2B, 0x87, 0x04, 0x00, 0x00, /* 004CCAD0 */
  0x00, 0xF7, 0xE8, 0x2B, 0xD8, 0x1B, 0xCA, 0x78, 0x40, 0x8B, 0x86, 0x08, 0x00, 0x00, 0x00, 0x2B, /* 004CCAE0 */
  0x87, 0x08, 0x00, 0x00, 0x00, 0xF7, 0xE8, 0x2B, 0xD8, 0x1B, 0xCA, 0x78, 0x2C, 0x0F, 0xA4, 0xD9, /* 004CCAF0 */
  0x1B, 0x8B, 0x57, 0x10, 0x8B, 0x5F, 0x14, 0x8B, 0xC1, 0x0F, 0x6E, 0x67, 0x0C, 0x0F, 0xA4, 0xD3, /* 004CCB00 */
  0x14, 0x74, 0x16, 0x0F, 0x60, 0xE4, 0x33, 0xD2, 0x0F, 0x71, 0xD4, 0x02, 0xF7, 0xF3, 0x0F, 0xE5, /* 004CCB10 */
  0x24, 0xC5, 0x80, 0xDE, 0x41, 0x00, 0x0F, 0xDD, 0xC4, 0x59, 0x81, 0xC7, 0x40, 0x00, 0x00, 0x00, /* 004CCB20 */
  0x49, 0x0F, 0x85, 0x79, 0xFF, 0xFF, 0xFF, 0x5E, 0x5F, 0x5A, 0x59, 0x5B, 0xC3, 0x90, 0x90, 0x90, /* 004CCB30 */
  0x53, 0x51, 0x52, 0x57, 0x55, 0x89, 0xE5, 0x83, 0x7D, 0x20, 0x00, 0x74, 0x23, 0xBF, 0x50, 0xED, /* 004CCB40 */
  0x4B, 0x00, 0xB9, 0x00, 0x01, 0x00, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, /* 004CCB50 */
  0x83, 0x7F, 0x0C, 0x00, 0x74, 0x1A, 0x81, 0xC7, 0x40, 0x00, 0x00, 0x00, 0x49, 0x75, 0xF1, 0x90, /* 004CCB60 */
  0x33, 0xC0, 0xF9, 0x89, 0xEC, 0x5D, 0x5F, 0x5A, 0x59, 0x5B, 0xC2, 0x18, 0x00, 0x90, 0x90, 0x90, /* 004CCB70 */
  0x8B, 0x45, 0x1C, 0x8B, 0x4D, 0x20, 0x8B, 0x5D, 0x18, 0x89, 0x47, 0x20, 0x81, 0xE1, 0xFF, 0xFF, /* 004CCB80 */
  0xFF, 0x00, 0xF7, 0xE8, 0x89, 0x4F, 0x0C, 0x85, 0xDB, 0x75, 0x16, 0x89, 0x57, 0x14, 0x89, 0x47, /* 004CCB90 */
  0x10, 0xC7, 0x47, 0x18, 0x00, 0x00, 0x00, 0x00, 0xC7, 0x47, 0x1C, 0x00, 0x00, 0x00, 0x00, 0xEB, /* 004CCBA0 */
  0x18, 0xC7, 0x47, 0x14, 0x00, 0x00, 0x00, 0x00, 0xC7, 0x47, 0x10, 0x00, 0x00, 0x00, 0x00, 0x89, /* 004CCBB0 */
  0x5F, 0x18, 0xC7, 0x47, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x8B, 0x45, 0x2C, 0x8B, 0x4D, 0x28, 0x8B, /* 004CCBC0 */
  0x55, 0x24, 0x89, 0x87, 0x00, 0x00, 0x00, 0x00, 0x89, 0x8F, 0x04, 0x00, 0x00, 0x00, 0x89, 0x97, /* 004CCBD0 */
  0x08, 0x00, 0x00, 0x00, 0x8B, 0xC7, 0xF8, 0x89, 0xEC, 0x5D, 0x5F, 0x5A, 0x59, 0x5B, 0xC2, 0x18, /* 004CCBE0 */
  0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, /* 004CCBF0 */
  0x50, 0x57, 0x55, 0x89, 0xE5, 0x8B, 0x7D, 0x14, 0x33, 0xC0, 0x85, 0xFF, 0x74, 0x3C, 0x2B, 0x45, /* 004CCC00 */
  0x10, 0x79, 0x06, 0x83, 0x7F, 0x18, 0x00, 0x7D, 0x0E, 0x89, 0x47, 0x14, 0x89, 0x47, 0x10, 0x89, /* 004CCC10 */
  0x47, 0x0C, 0x89, 0x47, 0x20, 0xEB, 0x23, 0x83, 0x7F, 0x18, 0x00, 0x75, 0x08, 0x89, 0x47, 0x18, /* 004CCC20 */
  0x89, 0x47, 0x1C, 0xEB, 0x15, 0x51, 0x8B, 0xC8, 0x52, 0x87, 0x4F, 0x18, 0xF7, 0x6F, 0x1C, 0xF7, /* 004CCC30 */
  0xF9, 0x89, 0x47, 0x1C, 0x5A, 0x59, 0x85, 0xC0, 0x74, 0xCF, 0x89, 0xEC, 0x5D, 0x5F, 0x58, 0xC2, /* 004CCC40 */
  0x08, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, /* 004CCC50 */
  0x50, 0x51, 0x57, 0xBF, 0x50, 0xED, 0x4B, 0x00, 0xB9, 0x00, 0x10, 0x00, 0x00, 0x33, 0xC0, 0xFC, /* 004CCC60 */
  0xF3, 0xAB, 0x5F, 0x59, 0x58, 0xC3, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, /* 004CCC70 */
  0x50, 0x53, 0x51, 0x52, 0x57, 0x56, 0x55, 0x89, 0xE5, 0xBE, 0x50, 0xED, 0x4B, 0x00, 0x8B, 0x7D, /* 004CCC80 */
  0x20, 0xB9, 0x00, 0x01, 0x00, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, /* 004CCC90 */
  0x8B, 0x5E, 0x18, 0x83, 0x7E, 0x0C, 0x00, 0x74, 0x3B, 0x85, 0xDB, 0x74, 0x37, 0x8B, 0x46, 0x20, /* 004CCCA0 */
  0xF7, 0x6E, 0x1C, 0xF7, 0xFB, 0xF7, 0xE8, 0x01, 0x7E, 0x1C, 0x89, 0x56, 0x14, 0x89, 0x46, 0x10, /* 004CCCB0 */
  0x33, 0xD2, 0x85, 0xDB, 0x79, 0x13, 0x3B, 0x56, 0x1C, 0x7F, 0x19, 0x89, 0x56, 0x14, 0x89, 0x56, /* 004CCCC0 */
  0x10, 0x89, 0x56, 0x20, 0x89, 0x56, 0x0C, 0xEB, 0x05, 0x3B, 0x5E, 0x1C, 0x7D, 0x06, 0x89, 0x56, /* 004CCCD0 */
  0x1C, 0x89, 0x56, 0x18, 0x81, 0xC6, 0x40, 0x00, 0x00, 0x00, 0x49, 0x75, 0xB3, 0x89, 0xEC, 0x5D, /* 004CCCE0 */
  0x5E, 0x5F, 0x5A, 0x59, 0x5B, 0x58, 0xC2, 0x04, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, /* 004CCCF0 */
  0x50, 0x53, 0x51, 0x52, 0x57, 0x56, 0xBE, 0x50, 0xED, 0x4B, 0x00, 0xBF, 0x50, 0x2D, 0x4C, 0x00, /* 004CCD00 */
  0xBB, 0x00, 0x01, 0x00, 0x00, 0x33, 0xD2, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, /* 004CCD10 */
  0x83, 0x7E, 0x0C, 0x00, 0x74, 0x2F, 0x8D, 0x06, 0x8D, 0x0F, 0x68, 0xA0, 0x58, 0x48, 0x00, 0x50, /* 004CCD20 */
  0x51, 0xE8, 0x3A, 0x81, 0xFB, 0xFF, 0x8B, 0x46, 0x0C, 0x8B, 0x4E, 0x20, 0x89, 0x47, 0x0C, 0x89, /* 004CCD30 */
  0x4F, 0x20, 0x8B, 0x46, 0x10, 0x8B, 0x4E, 0x14, 0x42, 0x89, 0x47, 0x10, 0x89, 0x4F, 0x14, 0x81, /* 004CCD40 */
  0xC7, 0x40, 0x00, 0x00, 0x00, 0x81, 0xC6, 0x40, 0x00, 0x00, 0x00, 0x4B, 0x75, 0xC2, 0x89, 0x15, /* 004CCD50 */
  0x50, 0x6D, 0x4C, 0x00, 0x5E, 0x5F, 0x5A, 0x59, 0x5B, 0x58, 0xC3, 0x90, 0x90, 0x90, 0x90, 0x90, /* 004CCD60 */
  0x50, 0x53, 0x51, 0x52, 0x57, 0x56, 0x55, 0x89, 0xE5, 0xBE, 0x50, 0x2D, 0x4C, 0x00, 0xBF, 0x54, /* 004CCD70 */
  0x6D, 0x4C, 0x00, 0x8B, 0x1D, 0x50, 0x6D, 0x4C, 0x00, 0x33, 0xD2, 0x85, 0xDB, 0x74, 0x4A, 0x90, /* 004CCD80 */
  0x52, 0x8B, 0x45, 0x2C, 0x53, 0x2B, 0x06, 0xF7, 0xE8, 0x8B, 0xD8, 0x8B, 0x45, 0x28, 0x8B, 0xCA, /* 004CCD90 */
  0x2B, 0x46, 0x04, 0xF7, 0xE8, 0x03, 0xD8, 0x8B, 0x45, 0x24, 0x13, 0xCA, 0x2B, 0x46, 0x08, 0xF7, /* 004CCDA0 */
  0xE8, 0x03, 0xD8, 0x8B, 0x45, 0x20, 0x13, 0xCA, 0x03, 0x46, 0x20, 0xF7, 0xE8, 0x2B, 0xC3, 0x1B, /* 004CCDB0 */
  0xD1, 0x5B, 0x5A, 0x78, 0x0B, 0xB9, 0x10, 0x00, 0x00, 0x00, 0x42, 0xFC, 0xF3, 0xA5, 0xEB, 0x06, /* 004CCDC0 */
  0x81, 0xC6, 0x40, 0x00, 0x00, 0x00, 0x4B, 0x75, 0xB7, 0x89, 0x15, 0x54, 0xAD, 0x4C, 0x00, 0x89, /* 004CCDD0 */
  0xEC, 0x5D, 0x5E, 0x5F, 0x5A, 0x59, 0x5B, 0x58, 0xC2, 0x10, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, /* 004CCDE0 */
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 /* 004CCDF0 */
};
/* original 0x004CCFF0..0x004CD1A0: machine code of 0x004CCFF0 (GraphicsShadingRuntime_InitializeGeneratedTextureCf) */
static const uint8_t s_ModelLightingOriginalCode004CCFF0[0x1B0] = {
  0x53, 0x51, 0x52, 0x57, 0x56, 0x55, 0x89, 0xE5, 0x8B, 0x4D, 0x20, 0x03, 0xC9, 0x0F, 0xAF, 0xC9, /* 004CCFF0 */
  0x51, 0xFF, 0x15, 0x00, 0x20, 0x40, 0x00, 0x0F, 0x82, 0x88, 0x01, 0x00, 0x00, 0x8B, 0x55, 0x20, /* 004CD000 */
  0xC1, 0xE9, 0x02, 0xC1, 0xEA, 0x01, 0xA3, 0x30, 0xCE, 0x4C, 0x00, 0x03, 0xD0, 0x8B, 0xF8, 0x03, /* 004CD010 */
  0xD1, 0x33, 0xC0, 0x89, 0x15, 0x34, 0xCE, 0x4C, 0x00, 0xFC, 0xF3, 0xAB, 0x8B, 0x4D, 0x24, 0x8B, /* 004CD020 */
  0x75, 0x1C, 0x0F, 0xAF, 0xC9, 0x81, 0xC1, 0x20, 0x00, 0x00, 0x00, 0x0F, 0xAF, 0xCE, 0x81, 0xC1, /* 004CD030 */
  0x00, 0x0A, 0x00, 0x00, 0x51, 0xFF, 0x15, 0x00, 0x20, 0x40, 0x00, 0x0F, 0x82, 0x44, 0x01, 0x00, /* 004CD040 */
  0x00, 0x8B, 0xF8, 0xA3, 0x2C, 0xCE, 0x4C, 0x00, 0x8B, 0xD1, 0x8B, 0xD8, 0xC1, 0xE9, 0x02, 0x33, /* 004CD050 */
  0xC0, 0xFC, 0xF3, 0xAB, 0x8B, 0x4D, 0x24, 0x8B, 0xFB, 0x51, 0x0F, 0xAF, 0xC9, 0xC7, 0x87, 0x00, /* 004CD060 */
  0x00, 0x00, 0x00, 0x67, 0x66, 0x78, 0x00, 0x89, 0x97, 0x04, 0x00, 0x00, 0x00, 0x89, 0xB7, 0xB0, /* 004CD070 */
  0x00, 0x00, 0x00, 0xC7, 0x87, 0xB4, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xC7, 0x87, 0xB8, /* 004CD080 */
  0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0xB8, 0xFF, 0xFF, 0xFF, 0x00, 0x8D, 0x9F, 0x00, 0x02, /* 004CD090 */
  0x00, 0x00, 0xBA, 0x00, 0x01, 0x00, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, /* 004CD0A0 */
  0x89, 0x03, 0x05, 0x00, 0x00, 0x00, 0x01, 0x83, 0xC3, 0x08, 0x4A, 0x75, 0xF3, 0xBA, 0x00, 0x0A, /* 004CD0B0 */
  0x00, 0x00, 0x89, 0x35, 0x18, 0xCE, 0x4C, 0x00, 0x8B, 0xDE, 0x03, 0xFA, 0xC1, 0xE6, 0x05, 0x58, /* 004CD0C0 */
  0x03, 0xD6, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, /* 004CD0D0 */
  0x89, 0x87, 0x00, 0x00, 0x00, 0x00, 0x89, 0x87, 0x04, 0x00, 0x00, 0x00, 0xC7, 0x87, 0x08, 0x00, /* 004CD0E0 */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x89, 0x97, 0x0C, 0x00, 0x00, 0x00, 0xC7, 0x87, 0x10, 0x00, /* 004CD0F0 */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC7, 0x87, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 004CD100 */
  0x89, 0x87, 0x18, 0x00, 0x00, 0x00, 0x89, 0x87, 0x1C, 0x00, 0x00, 0x00, 0x03, 0xD1, 0x81, 0xC7, /* 004CD110 */
  0x20, 0x00, 0x00, 0x00, 0x4B, 0x75, 0xB9, 0x8B, 0x55, 0x20, 0xA3, 0x00, 0xCE, 0x4C, 0x00, 0x89, /* 004CD120 */
  0x15, 0x04, 0xCE, 0x4C, 0x00, 0x8B, 0xC8, 0xB8, 0x00, 0x00, 0x10, 0x00, 0xF7, 0xE2, 0xF7, 0xF1, /* 004CD130 */
  0xA3, 0x24, 0xCE, 0x4C, 0x00, 0xA3, 0x28, 0xCE, 0x4C, 0x00, 0xA1, 0x04, 0xCE, 0x4C, 0x00, 0x33, /* 004CD140 */
  0xC9, 0xC1, 0xF8, 0x01, 0x48, 0x2B, 0xC8, 0xC1, 0xE0, 0x0C, 0xC1, 0xE1, 0x0C, 0xA3, 0x40, 0xCE, /* 004CD150 */
  0x4C, 0x00, 0x89, 0x0D, 0x44, 0xCE, 0x4C, 0x00, 0xFF, 0x35, 0x2C, 0xCE, 0x4C, 0x00, 0xFF, 0x15, /* 004CD160 */
  0x34, 0x58, 0x48, 0x00, 0x72, 0x11, 0xA3, 0x38, 0xCE, 0x4C, 0x00, 0xF8, 0x89, 0xEC, 0x5D, 0x5E, /* 004CD170 */
  0x5F, 0x5A, 0x59, 0x5B, 0xC2, 0x0C, 0x00, 0x50, 0xFF, 0x35, 0x2C, 0xCE, 0x4C, 0x00, 0xFF, 0x15, /* 004CD180 */
  0x04, 0x20, 0x40, 0x00, 0x58, 0xF9, 0x89, 0xEC, 0x5D, 0x5E, 0x5F, 0x5A, 0x59, 0x5B, 0xC2, 0x0C /* 004CD190 */
};

/* One piece of the original address window: original bytes start..end live at bytes. */
typedef struct ModelLightingOriginalRange {
  uint32_t start;
  uint32_t end;
  const uint8_t *bytes;
} ModelLightingOriginalRange;

/* The original address window 0x004C6D54..0x004CD1A0 (whole variables listed, covers the default path's reach
   0x004C91A0..0x004CD1A0), in address order and without holes. */
#define MODEL_LIGHTING_ORIGINAL_WINDOW_START 0x004C6D54u
#define MODEL_LIGHTING_ORIGINAL_WINDOW_END 0x004CD1A0u
static const ModelLightingOriginalRange s_ModelLightingOriginalWindow[] = {
  {0x004C6D54, 0x004CAD54, (const uint8_t *)g_GraphicsShadingNearbyRecords},
  {0x004CAD54, 0x004CAD58, (const uint8_t *)&g_GraphicsShadingNearbyRecordCount},
  {0x004CAD58, 0x004CAD60, s_ModelLightingOriginalFiller004CAD58},
  {0x004CAD60, 0x004CC6F8, (const uint8_t *)g_ModelLightingMmxMultiplierRows},
  {0x004CC6F8, 0x004CC704, (const uint8_t *)&g_ModelLightingVertexToLightVectorScratch},
  {0x004CC704, 0x004CC710, (const uint8_t *)&g_ModelLightingTransformedSurfaceNormalScratch},
  {0x004CC710, 0x004CCE00, s_ModelLightingOriginalCode004CC710},
  {0x004CCE00, 0x004CCE04, (const uint8_t *)&g_GraphicsShadingTextureDimension},
  {0x004CCE04, 0x004CCE08, (const uint8_t *)&g_GraphicsShadingGridHalfSize},
  {0x004CCE08, 0x004CCE0C, (const uint8_t *)&g_GraphicsShadingGeneratedTexturePixelCursor},
  {0x004CCE0C, 0x004CCE10, (const uint8_t *)&g_GraphicsShadingGeneratedTextureTileX},
  {0x004CCE10, 0x004CCE14, (const uint8_t *)&g_GraphicsShadingGeneratedTextureTileY},
  {0x004CCE14, 0x004CCE18, (const uint8_t *)&g_GraphicsShadingGeneratedTextureSubresourceIndex},
  {0x004CCE18, 0x004CCE1C, (const uint8_t *)&g_GraphicsShadingSubresourceCount},
  {0x004CCE1C, 0x004CCE20, (const uint8_t *)&g_GraphicsShadingGeneratedTextureTileXQ20},
  {0x004CCE20, 0x004CCE24, (const uint8_t *)&g_GraphicsShadingGeneratedTextureTileYQ20},
  {0x004CCE24, 0x004CCE28, (const uint8_t *)&g_GraphicsShadingGridStepQ20},
  {0x004CCE28, 0x004CCE2C, (const uint8_t *)&g_GraphicsShadingGridStepQ20Current},
  {0x004CCE2C, 0x004CCE30, (const uint8_t *)&g_GraphicsShadingGeneratedAsset},
  {0x004CCE30, 0x004CCE34, (const uint8_t *)&g_GraphicsShadingGridScratch},
  {0x004CCE34, 0x004CCE38, (const uint8_t *)&g_GraphicsShadingGridScratchInterior},
  {0x004CCE38, 0x004CCE3C, (const uint8_t *)&g_GraphicsShadingTextureSet},
  {0x004CCE3C, 0x004CCE40, (const uint8_t *)&g_GraphicsShadingGeneratedTextureCompletedTraversalCount},
  {0x004CCE40, 0x004CCE44, (const uint8_t *)&g_GraphicsShadingPositiveGridOriginQ12},
  {0x004CCE44, 0x004CCE48, (const uint8_t *)&g_GraphicsShadingNegativeGridOriginQ12},
  {0x004CCE48, 0x004CCFF0, (const uint8_t *)&g_GeneratedTextureScratchRuntime},
  {0x004CCFF0, 0x004CD1A0, s_ModelLightingOriginalCode004CCFF0},
};

/* The variables must still have their original sizes for the window above. */
typedef char ModelLightingOriginalWindowSizeCheck
  [(sizeof g_GraphicsShadingNearbyRecords == 0x4000 && sizeof g_GraphicsShadingNearbyRecordCount == 4 &&
    sizeof g_ModelLightingMmxMultiplierRows == 0x1998 && sizeof g_ModelLightingVertexToLightVectorScratch == 0xC &&
    sizeof g_ModelLightingTransformedSurfaceNormalScratch == 0xC &&
    sizeof g_GraphicsShadingGeneratedTexturePixelCursor == 4 && sizeof g_GraphicsShadingGeneratedAsset == 4 &&
    sizeof g_GraphicsShadingGridScratch == 4 && sizeof g_GraphicsShadingGridScratchInterior == 4 &&
    sizeof g_GraphicsShadingTextureSet == 4 && sizeof g_GeneratedTextureScratchRuntime == 0x1A8) ? 1 : -1];

/* The 8 bytes the original read at originalAddress, little-endian. Inside the window they are exact; outside it
   (only ModelRender_ComputeVertexIntensityScaledPath gets there, see its quirk) the original read unrelated
   memory far from this table or faulted, which cannot be reproduced: those reads give 0. */
static uint64_t ModelLighting_ReadOriginalImageQword(uint32_t originalAddress)
{
  uint64_t value = 0;
  uint32_t byteIndex;
  size_t rangeIndex;

  if (originalAddress < MODEL_LIGHTING_ORIGINAL_WINDOW_START ||
      originalAddress > MODEL_LIGHTING_ORIGINAL_WINDOW_END - 8) {
    return 0;
  }
  for (byteIndex = 0; byteIndex < 8; byteIndex++) {
    uint32_t address = originalAddress + byteIndex;
    for (rangeIndex = 0; rangeIndex < sizeof s_ModelLightingOriginalWindow / sizeof s_ModelLightingOriginalWindow[0];
         rangeIndex++) {
      const ModelLightingOriginalRange *range = &s_ModelLightingOriginalWindow[rangeIndex];
      if (range->start <= address && address < range->end) {
        value |= (uint64_t)range->bytes[address - range->start] << (8 * byteIndex);
        break;
      }
    }
  }
  return value;
}

/* The qword at byteOffset from the start of g_ModelLightingMmxMultiplierRows as the original's PMULHW operand:
   inside the table a plain read, outside it the original bytes at that address (see the quirks). */
static uint64_t ModelLighting_ReadMultiplierQword(int32_t byteOffset)
{
  if (byteOffset >= 0 && byteOffset <= (int32_t)sizeof g_ModelLightingMmxMultiplierRows - 8) {
    return *(const uint64_t *)((const uint8_t *)g_ModelLightingMmxMultiplierRows + byteOffset);
  }
  return ModelLighting_ReadOriginalImageQword(MODEL_LIGHTING_MMX_ROWS_ORIGINAL_ADDRESS + (uint32_t)byteOffset);
}

/* distanceAttenuationTable[rowOffset] (ModelRender_ComputeVertexIntensityDefaultPath). */
static uint64_t ModelLighting_ReadDistanceAttenuationRow
          (GraphicsDistanceAttenuationTableAddress32 distanceAttenuationTable, int32_t rowOffset)
{
  return ModelLighting_ReadMultiplierQword
           ((int32_t)(distanceAttenuationTable -
                      (GraphicsDistanceAttenuationTableAddress32)(uintptr_t)g_ModelLightingMmxMultiplierRows) +
            rowOffset * 8);
}


/* Address: 0x004CC710.
   Lit colour of a mesh vertex for ModelRender_PrepareProjectedVertex, in MMX word lanes: the directional light
   (scenePackedColor1, weighted by the attenuation table entry for dot(lightDirection, normal) >> 21) plus a quarter
   of the ambient colour scenePackedColor0, times materialPackedColor (the node tint); then every nearby light of
   g_GraphicsShadingNearbyRecords whose sphere contains the vertex adds its colour weighted by
   g_PackedLightingLookupTable[((r^2 - d^2) >> 5) / (r^2 >> 12)]; the sum is modulated by the vertex's own colour.
*/
PackedArgb32
ModelRender_ComputeVertexIntensityDefaultPath
          (PackedArgb32 vertexPackedColor,int *vertexPositionQ12,
          GraphicsDistanceAttenuationTableAddress32 distanceAttenuationTable,
          PackedArgb32 scenePackedColor0,PackedArgb32 scenePackedColor1,
          GraphicsFixedVec3 *lightDirectionQ12,PackedArgb32 materialPackedColor,
          GraphicsFixedVec3 *surfaceNormalQ12)

{
  PackedRgb24 lightPackedColor;
  int64_t axisDistanceSquared;
  int32_t lightFacingDotQ12;
  int remainderHigh;
  uint32_t remainderLow;
  int axisDelta;
  uint32_t axisSquareLow;
  GraphicsShadingRecordCount remainingRecords;
  uint32_t lookupDivisor;
  GraphicsShadingRuntimeRecord *shadingRecord;
  uint64_t directionalLanes;
  uint64_t accumulatedLanes;
  uint64_t lightLanes;

  lightFacingDotQ12 = FixedVec3_DotQ12(lightDirectionQ12,surfaceNormalQ12);
  directionalLanes =
       pmulhw(ModelLighting_UnpackBytesMmx(scenePackedColor1,2),
              ModelLighting_ReadDistanceAttenuationRow(distanceAttenuationTable,lightFacingDotQ12 >> 21));
  shadingRecord = g_GraphicsShadingNearbyRecords;
  accumulatedLanes =
       pmulhw(ModelLighting_AddWordsMmx(directionalLanes,ModelLighting_UnpackBytesMmx(scenePackedColor0,4)),
              ModelLighting_UnpackBytesMmx(materialPackedColor,2));
  for (remainingRecords = g_GraphicsShadingNearbyRecordCount; remainingRecords != 0; remainingRecords--) {
    if (shadingRecord->targetRadiusQ12 != 0) {
      /* r^2 - dx^2 - dy^2 - dz^2 as a 64-bit subtraction on dword halves (remainderHigh:remainderLow, the low
         dword borrowing from the high one); the light reaches the vertex while remainderHigh stays >= 0 */
      remainderLow = (uint32_t)shadingRecord->squaredRadiusQ24;
      remainderHigh = ((int *)&shadingRecord->squaredRadiusQ24)[1];
      axisDelta = *vertexPositionQ12 - shadingRecord->worldXQ12;
      axisDistanceSquared = (int64_t)axisDelta * (int64_t)axisDelta;
      axisSquareLow = (uint32_t)axisDistanceSquared;
      remainderHigh = (remainderHigh - (int)((uint64_t)axisDistanceSquared >> 32)) -
                      (uint32_t)(remainderLow < axisSquareLow);
      remainderLow = remainderLow - axisSquareLow;
      if (-1 < remainderHigh) {
        axisDelta = vertexPositionQ12[1] - shadingRecord->worldYQ12;
        axisDistanceSquared = (int64_t)axisDelta * (int64_t)axisDelta;
        axisSquareLow = (uint32_t)axisDistanceSquared;
        remainderHigh = (remainderHigh - (int)((uint64_t)axisDistanceSquared >> 32)) -
                        (uint32_t)(remainderLow < axisSquareLow);
        remainderLow = remainderLow - axisSquareLow;
        if (-1 < remainderHigh) {
          axisDelta = vertexPositionQ12[2] - shadingRecord->worldZQ12;
          axisDistanceSquared = (int64_t)axisDelta * (int64_t)axisDelta;
          axisSquareLow = (uint32_t)axisDistanceSquared;
          remainderHigh = (remainderHigh - (int)((uint64_t)axisDistanceSquared >> 32)) -
                          (uint32_t)(remainderLow < axisSquareLow);
          remainderLow = remainderLow - axisSquareLow;
          if (-1 < remainderHigh) {
            lightPackedColor = shadingRecord->packedColorRgbActive;
            /* divisor r^2 >> 12 (SHRD) */
            lookupDivisor = ((int *)&shadingRecord->squaredRadiusQ24)[1] << (32 - Q12_SHIFT) |
                     (uint32_t)shadingRecord->squaredRadiusQ24 >> Q12_SHIFT;
            if (lookupDivisor != 0) {
              /* table index: (remainder >> 5) / (r^2 >> 12), low dword only */
              lightLanes =
                   pmulhw(ModelLighting_UnpackBytesMmx(lightPackedColor,2),
                          g_PackedLightingLookupTable[(remainderHigh * (1 << 27) | remainderLow >> 5) /
                                                      lookupDivisor]);
              /* PADDUSB (byte lanes) as in the original, although the lanes hold words. */
              accumulatedLanes = paddusb(accumulatedLanes,lightLanes);
            }
          }
        }
      }
    }
    shadingRecord = shadingRecord + 1;
  }
  accumulatedLanes = pmulhw(accumulatedLanes,ModelLighting_UnpackBytesMmx(vertexPackedColor,2));
  return ModelLighting_PackUnsignedMmx(accumulatedLanes);
}

/* Address: 0x004CC820.
   The same vertex lighting as ModelRender_ComputeVertexIntensityDefaultPath for MODEL_TRIANGLE_LIGHTING_SCALED
   triangles: the directional weight comes from g_ModelLightingMmxMultiplierRows at MODEL_LIGHTING_SCALE_ROW0 plus
   the facing dot divided by the model resource's lightingScaleQ12 (>> 9).
   Original quirk (0x004CC833 CDQ / IDIV [EBP+0x24] / SAR EAX,9 / PMULHW MM0,[EAX*8 + 0x004CC2B0]): no range check.
   The normal here is the vertex position (Q12, P units along the light), the light direction is Q28, so
   dot = 2^28 * P (low dword, wraps from |P| >= 8) and the row offset is floor(trunc(dot / s) / 512) with
   s = lightingScaleQ12 = S units * 4096, i.e. about 128 * P / S. The table holds offsets -682..136
   (P / S in -5.33..1.06). For |s| >= 4096 any wrapped dot gives offsets -1024..1023 (0x004CA2B0..0x004CE2B0);
   smaller |s| reach up to +-2^22 rows (+-32 MB), far outside the original image. s == 0 and
   dot == INT_MIN with s == -1 raise #DE in the original as in this C division. Out-of-table rows go through
   ModelLighting_ReadMultiplierQword: exact original bytes within 0x004C6D54..0x004CD1A0, 0 beyond it.
*/
PackedArgb32
ModelRender_ComputeVertexIntensityScaledPath
          (PackedArgb32 vertexPackedColor,int *vertexPositionQ12,Q12 lightingScaleQ12,
          PackedArgb32 scenePackedColor0,PackedArgb32 scenePackedColor1,
          GraphicsFixedVec3 *lightDirectionQ12,PackedArgb32 materialPackedColor,
          GraphicsFixedVec3 *surfaceNormalQ12)

{
  PackedRgb24 lightPackedColor;
  int64_t axisDistanceSquared;
  int32_t lightFacingDotQ12;
  int remainderHigh;
  uint32_t remainderLow;
  int axisDelta;
  uint32_t axisSquareLow;
  GraphicsShadingRecordCount remainingRecords;
  uint32_t lookupDivisor;
  GraphicsShadingRuntimeRecord *shadingRecord;
  uint64_t directionalLanes;
  uint64_t accumulatedLanes;
  uint64_t resultLanes;
  uint64_t lightLanes;

  lightFacingDotQ12 = FixedVec3_DotQ12(lightDirectionQ12,surfaceNormalQ12);
  directionalLanes =
       pmulhw(ModelLighting_UnpackBytesMmx(scenePackedColor1,2),
              ModelLighting_ReadMultiplierQword
                (((int32_t)MODEL_LIGHTING_SCALE_ROW0 + (lightFacingDotQ12 / lightingScaleQ12 >> 9)) * 8));
  shadingRecord = g_GraphicsShadingNearbyRecords;
  accumulatedLanes =
       pmulhw(ModelLighting_AddWordsMmx(directionalLanes,ModelLighting_UnpackBytesMmx(scenePackedColor0,4)),
              ModelLighting_UnpackBytesMmx(materialPackedColor,2));
  for (remainingRecords = g_GraphicsShadingNearbyRecordCount; remainingRecords != 0; remainingRecords--) {
    if (shadingRecord->targetRadiusQ12 != 0) {
      /* r^2 - dx^2 - dy^2 - dz^2 as a 64-bit subtraction on dword halves (remainderHigh:remainderLow, the low
         dword borrowing from the high one); the light reaches the vertex while remainderHigh stays >= 0 */
      remainderLow = (uint32_t)shadingRecord->squaredRadiusQ24;
      remainderHigh = ((int *)&shadingRecord->squaredRadiusQ24)[1];
      axisDelta = *vertexPositionQ12 - shadingRecord->worldXQ12;
      axisDistanceSquared = (int64_t)axisDelta * (int64_t)axisDelta;
      axisSquareLow = (uint32_t)axisDistanceSquared;
      remainderHigh = (remainderHigh - (int)((uint64_t)axisDistanceSquared >> 32)) -
                      (uint32_t)(remainderLow < axisSquareLow);
      remainderLow = remainderLow - axisSquareLow;
      if (-1 < remainderHigh) {
        axisDelta = vertexPositionQ12[1] - shadingRecord->worldYQ12;
        axisDistanceSquared = (int64_t)axisDelta * (int64_t)axisDelta;
        axisSquareLow = (uint32_t)axisDistanceSquared;
        remainderHigh = (remainderHigh - (int)((uint64_t)axisDistanceSquared >> 32)) -
                        (uint32_t)(remainderLow < axisSquareLow);
        remainderLow = remainderLow - axisSquareLow;
        if (-1 < remainderHigh) {
          axisDelta = vertexPositionQ12[2] - shadingRecord->worldZQ12;
          axisDistanceSquared = (int64_t)axisDelta * (int64_t)axisDelta;
          axisSquareLow = (uint32_t)axisDistanceSquared;
          remainderHigh = (remainderHigh - (int)((uint64_t)axisDistanceSquared >> 32)) -
                          (uint32_t)(remainderLow < axisSquareLow);
          remainderLow = remainderLow - axisSquareLow;
          if (-1 < remainderHigh) {
            lightPackedColor = shadingRecord->packedColorRgbActive;
            /* divisor r^2 >> 12 (SHRD) */
            lookupDivisor = ((int *)&shadingRecord->squaredRadiusQ24)[1] << (32 - Q12_SHIFT) |
                     (uint32_t)shadingRecord->squaredRadiusQ24 >> Q12_SHIFT;
            if (lookupDivisor != 0) {
              /* table index: (remainder >> 5) / (r^2 >> 12), low dword only */
              lightLanes =
                   pmulhw(ModelLighting_UnpackBytesMmx(lightPackedColor,2),
                          g_PackedLightingLookupTable[(remainderHigh * (1 << 27) | remainderLow >> 5) /
                                                      lookupDivisor]);
              /* PADDUSB (byte lanes) as in the original, although the lanes hold words. */
              accumulatedLanes = paddusb(accumulatedLanes,lightLanes);
            }
          }
        }
      }
    }
    shadingRecord = shadingRecord + 1;
  }
  resultLanes = pmulhw(accumulatedLanes,ModelLighting_UnpackBytesMmx(vertexPackedColor,2));
  return ModelLighting_PackUnsignedMmx(resultLanes);
}

/* Address: 0x004CC940.
   Vertex colour of the alternate model renderer (ModelRender_PrepareProjectedVertexAlternatePath): ambient
   scenePackedColor0 times the material colour, plus every nearby light whose sphere contains the vertex, weighted
   by g_PackedLightingLookupTable at (9 * r^2 / (8 * d^2 + r^2)) scaled by the facing of the view-space normal
   towards the light; the sum is modulated by the vertex's own colour.
*/
PackedArgb32
ModelRender_ComputeNearbyLightPackedVertexColorAlternatePath
          (PackedArgb32 vertexPackedColor,GraphicsFixedVec3 *vertexPositionQ12,
          PackedArgb32 scenePackedColor0,PackedArgb32 materialPackedColor,
          GraphicsFixedVec3 *surfaceNormalQ12)

{
  /* Rewritten from the assembly (0x004CC940-0x004CCA88); Ghidra dropped the MMX accumulator, so every
     light was added to an undefined register instead of the running color. */
  const short *lightingTable = (const short *)&g_PackedLightingLookupTable;
  const GraphicsShadingRuntimeRecord *record = g_GraphicsShadingNearbyRecords;
  GraphicsShadingRecordCount remaining;
  short color[4];
  short lanes[4];

  scenePackedColor0 = scenePackedColor0 | ARGB8888_ALPHA_MASK;
  FixedTransform_ApplyDirection
            (&g_ModelLightingTransformedSurfaceNormalScratch,surfaceNormalQ12,
             &g_ModelViewCompositeTransform);
  ModelLighting_UnpackBytes(scenePackedColor0,3,color);
  ModelLighting_UnpackBytes(materialPackedColor,3,lanes);
  ModelLighting_MulHigh(color,lanes);
  for (remaining = g_GraphicsShadingNearbyRecordCount; remaining != 0; remaining--, record++) {
    int64_t distanceSquared;
    int64_t radiusSquared;
    uint32_t divisor;
    uint32_t numerator;
    int32_t facing;
    int quotient;
    uint32_t tableIndex;

    if (record->targetRadiusQ12 == 0) {
      continue;
    }
    g_ModelLightingVertexToLightVectorScratch.x = record->worldXQ12 - vertexPositionQ12->x;
    g_ModelLightingVertexToLightVectorScratch.y = record->worldYQ12 - vertexPositionQ12->y;
    g_ModelLightingVertexToLightVectorScratch.z = record->worldZQ12 - vertexPositionQ12->z;
    distanceSquared =
         (int64_t)g_ModelLightingVertexToLightVectorScratch.x *
         g_ModelLightingVertexToLightVectorScratch.x +
         (int64_t)g_ModelLightingVertexToLightVectorScratch.y *
         g_ModelLightingVertexToLightVectorScratch.y +
         (int64_t)g_ModelLightingVertexToLightVectorScratch.z *
         g_ModelLightingVertexToLightVectorScratch.z;
    radiusSquared = (int64_t)record->squaredRadiusQ24;
    if (distanceSquared >= radiusSquared) {
      continue;
    }
    divisor = (uint32_t)((uint64_t)(distanceSquared * 8 + radiusSquared) >> 20);
    if (divisor == 0) {
      continue;
    }
    numerator = (uint32_t)((uint64_t)radiusSquared >> 15) * 9;
    FixedVec3_NormalizeQ28
              (&g_ModelLightingVertexToLightVectorScratch,&g_ModelLightingVertexToLightVectorScratch);
    facing = FixedVec3_DotQ12
                       (&g_ModelLightingVertexToLightVectorScratch,
                        &g_ModelLightingTransformedSurfaceNormalScratch);
    if (facing < 0) {
      facing = 0;
    }
    quotient = (int)((int64_t)numerator / (int)divisor);
    tableIndex = (uint32_t)(((int64_t)quotient * facing) >> 28);
    ModelLighting_UnpackBytes(record->packedColorRgbActive,2,lanes);
    ModelLighting_MulHigh(lanes,lightingTable + tableIndex * 4);
    ModelLighting_AddSaturate(color,lanes);
  }
  ModelLighting_UnpackBytes(vertexPackedColor,2,lanes);
  ModelLighting_MulHigh(color,lanes);
  return ModelLighting_PackUnsigned(color);
}


/* Address: 0x0050A430.
   Transforms the point in g_GraphicsTransformInputScratchVec3 with g_GraphicsTransformScratchMatrix3x4 and, when
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


/* Address: 0x004BD800.
   Per-mesh setup of ModelRender_SubmitMeshTriangles and ModelRender_SubmitMeshTrianglesAlternatePath: composes
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

