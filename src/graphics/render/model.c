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
    lanes[i] = (short)((((packed >> (8 * i)) & 0xff) * 0x101) >> shift);
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
    lanes[i] = (short)(sum > 0x7fff ? 0x7fff : sum < -0x8000 ? -0x8000 : sum);
  }
}

/* packuswb mm,mm + movd */
static PackedArgb32 ModelLighting_PackUnsigned(const short lanes[4])
{
  uint32_t packed = 0;
  int i;
  for (i = 0; i < 4; i++) {
    int v = lanes[i] < 0 ? 0 : lanes[i] > 0xff ? 0xff : lanes[i];
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
    lanes.uw[i] = (uint16_t)((((packed >> (8 * i)) & 0xff) * 0x101) >> shift);
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
  uint32_t groupFlagsOrMask;
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
  groupFlagsOrMask = ((ModelMeshGroupHeader *)meshGroup)->groupFlags;
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
  if ((groupFlagsOrMask & MODEL_MESH_GROUP_FACE_VIEWER) != 0) {
    ModelNodeRuntime_BuildViewFacingRotation(modelNode);
  }
  if ((groupFlagsOrMask & MODEL_MESH_GROUP_BILLBOARD) != 0) {
    ModelNodeRuntime_BuildBillboardRotation(modelNode);
  }
  meshRecord = (ModelMeshHeader *)((ModelMeshGroupHeader *)meshGroup + 1);
  groupFlagsOrMask = (modelNode->modelPayload).meshGroupMask;
  for (; remainingMeshCount != 0; remainingMeshCount--) {
    if ((meshRecord->groupMask & groupFlagsOrMask) != 0) {
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
  uint32_t groupFlagsOrMask;
  int remainingMeshCount;
  ModelMeshHeader *meshRecord;

  groupFlagsOrMask = ((ModelMeshGroupHeader *)meshGroup)->groupFlags;
  remainingMeshCount = ((ModelMeshGroupHeader *)meshGroup)->meshCount;
  if ((groupFlagsOrMask & MODEL_MESH_GROUP_FACE_VIEWER) != 0) {
    ModelNodeRuntime_BuildViewFacingRotation(modelNode);
  }
  if ((groupFlagsOrMask & MODEL_MESH_GROUP_BILLBOARD) != 0) {
    ModelNodeRuntime_BuildBillboardRotation(modelNode);
  }
  meshRecord = (ModelMeshHeader *)((ModelMeshGroupHeader *)meshGroup + 1);
  groupFlagsOrMask = (modelNode->modelPayload).meshGroupMask;
  for (; remainingMeshCount != 0; remainingMeshCount--) {
    if ((meshRecord->groupMask & groupFlagsOrMask) != 0) {
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
  GraphicsProjectedPointEdxEax8 projectedScreenCoordinatePair;
  GraphicsWorldCoordinateQ12 savedVertexXQ12;
  int64_t scaledVertexCoordinateProduct;
  GraphicsWorldCoordinateQ12 savedVertexZQ12;
  GraphicsWorldCoordinateQ12 savedVertexYQ12;

  triangleRenderFlags = ((GraphicsTriangleInput *)triangle)->renderFlags;
  if (vertex[4].x == MODEL_VERTEX_NOT_PROJECTED) {
    savedVertexXQ12 = vertex->x;
    savedVertexYQ12 = vertex->y;
    savedVertexZQ12 = vertex->z;
    depthBiasHalf = (uint32_t)modelNode->renderDepthBiasOrState >> 1;
    /* half the depth bias for z == 0, the full bias for z > 0 */
    if (-1 < vertex->z) {
      if (vertex->z != 0) {
        vertex->z = vertex->z + depthBiasHalf;
      }
      vertex->z = vertex->z + depthBiasHalf;
    }
    if ((modelNode->runtimeFlags & MODEL_RUNTIME_FLAG_APPLY_SCALE) != 0) {
      /* 64-bit product >> 12 (IMUL + SHRD) */
      scaledVertexCoordinateProduct = (int64_t)vertex->x * (int64_t)modelNode->modelScaleQ12;
      vertex->x = (int)((uint64_t)scaledVertexCoordinateProduct >> 0x20) << 0x14 |
                  (uint32_t)scaledVertexCoordinateProduct >> 0xc;
      scaledCoordinateProduct = (int64_t)vertex->y * (int64_t)modelNode->modelScaleQ12;
      vertex->y = (int)((uint64_t)scaledCoordinateProduct >> 0x20) << 0x14 | (uint32_t)scaledCoordinateProduct >> 0xc;
      scaledCoordinateProduct = (int64_t)vertex->z * (int64_t)modelNode->modelScaleQ12;
      vertex->z = (int)((uint64_t)scaledCoordinateProduct >> 0x20) << 0x14 | (uint32_t)scaledCoordinateProduct >> 0xc;
    }
    FixedTransform_ApplyPoint
              ((GraphicsFixedVec3 *)&vertex[2].z,vertex,
               (GraphicsFixedMatrix3x4 *)&g_ModelViewCompositeTransform);
    projectedScreenCoordinatePair =
         THANDOR_BITCAST(GraphicsProjectedPointPair, GraphicsProjectedPointEdxEax8,
                         Graphics_ProjectViewPoint((GraphicsFixedVec3 *)&vertex[2].z));
    vertex->z = savedVertexZQ12;
    vertex->y = savedVertexYQ12;
    vertex->x = savedVertexXQ12;
    vertex[4].x = (int)projectedScreenCoordinatePair;
    vertex[4].y = (int)(projectedScreenCoordinatePair >> 0x20);
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
                      (vertex[2].y,&vertex[2].z,THANDOR_ADDR(g_ModelDistanceAttenuationMmx,0),g_SceneBoundsFixed.bound5,
                       g_SceneBoundsFixed.bound4,
                       (GraphicsFixedVec3 *)&g_ModelAuxiliaryForwardDirectionLocal,vertexColor,
                       surfaceNormalQ12);
    vertex[3].z = vertexColor;
    return;
  }
  /* the scaled path gets the vertex position as its normal (LEA EDX,[ESI] at 0x004BD636) */
  vertexColor = ModelRender_ComputeVertexIntensityScaledPath
                    (vertex[2].y,&vertex[2].z,
                     ((modelNode->modelPayload).modelResource)->lightingScaleQ12,
                     g_SceneBoundsFixed.bound7,g_SceneBoundsFixed.bound6,
                     (GraphicsFixedVec3 *)&g_ModelAuxiliaryForwardDirectionLocal,vertexColor,vertex);
  vertex[3].z = vertexColor;
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
  GraphicsSubresourceIndex triangleSubresource;
  int32_t facingDotQ12;
  uint32_t paletteBankIndex;
  bool appendFailed;
  GraphicsTextureSetEntry *textureEntry;

  facingDotQ12 = ModelRender_ComputeFacingDotQ12(triangle);
  if ((triangle->renderFlags & MODEL_TRIANGLE_DOUBLE_SIDED) != 0 || facingDotQ12 < facingThresholdQ12) {
    firstVertex = triangle->vertex0;
    secondVertex = triangle->vertex1;
    thirdVertex = triangle->vertex2;
    ModelRender_PrepareProjectedVertex
              (modelNode,(ModelMeshGroupAddress32)triangle,(GraphicsFixedVec3 *)firstVertex);
    ModelRender_PrepareProjectedVertex
              (modelNode,(ModelMeshGroupAddress32)triangle,(GraphicsFixedVec3 *)secondVertex);
    ModelRender_PrepareProjectedVertex
              (modelNode,(ModelMeshGroupAddress32)triangle,(GraphicsFixedVec3 *)thirdVertex);
    if ((g_ProjectionClipRect.minX <= firstVertex->screenX || g_ProjectionClipRect.minX <= secondVertex->screenX ||
         g_ProjectionClipRect.minX <= thirdVertex->screenX) &&
        (firstVertex->screenX < g_ProjectionClipRect.maxX || secondVertex->screenX < g_ProjectionClipRect.maxX ||
         thirdVertex->screenX < g_ProjectionClipRect.maxX)) {
      if ((g_ProjectionClipRect.minY <= firstVertex->screenY || g_ProjectionClipRect.minY <= secondVertex->screenY ||
           g_ProjectionClipRect.minY <= thirdVertex->screenY) &&
          (firstVertex->screenY < g_ProjectionClipRect.maxY || secondVertex->screenY < g_ProjectionClipRect.maxY ||
           thirdVertex->screenY < g_ProjectionClipRect.maxY)) {
        appendFailed = GraphicsPrimitiveQueue_AppendTriangle
                          (triangle->renderFlags,triangle,triangle->vertex2,triangle->vertex1,
                           triangle->vertex0,g_ActivePrimitiveQueue);
        if (!appendFailed) {
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
          nodePaletteAsset = (modelNode->modelPayload).paletteAsset;
          if ((nodePaletteAsset == NULL) ||
             (paletteBankIndex = triangle->renderFlags & MODEL_TRIANGLE_PALETTE_BANK_MASK,
              nodePaletteAsset->paletteBankCount <= paletteBankIndex)) {
            GraphicsPrimitiveQueue_SetMaterial(0xffffffff,textureEntry,g_ActivePrimitiveQueue);
            triangleSubresource = triangle->subresourceIndex;
            if ((modelNode->runtimeFlags & MODEL_RUNTIME_FLAG_PRIMARY_TEXTURE_SCROLL) != 0) {
              if (triangleSubresource == modelNode->primaryAnimatedSubresourceIndex) {
                GraphicsPrimitiveQueue_OffsetTextureCoordinates
                          (modelNode->primaryTextureOffsetV,modelNode->primaryTextureOffsetU,
                           g_ActivePrimitiveQueue);
              }
              if (((modelNode->runtimeFlags & MODEL_RUNTIME_FLAG_SECONDARY_TEXTURE_SCROLL) != 0) &&
                 (triangleSubresource == modelNode->secondaryAnimatedSubresourceIndex)) {
                GraphicsPrimitiveQueue_OffsetTextureCoordinates
                          (modelNode->secondaryTextureOffsetV,modelNode->secondaryTextureOffsetU,
                           g_ActivePrimitiveQueue);
              }
            }
          }
          else {
            GraphicsPrimitiveQueue_SetMaterial
                      (nodePaletteAsset->paletteEntries[paletteBankIndex].argb8888,textureEntry,g_ActivePrimitiveQueue);
            triangleSubresource = triangle->subresourceIndex;
            if ((modelNode->runtimeFlags & MODEL_RUNTIME_FLAG_PRIMARY_TEXTURE_SCROLL) != 0) {
              if (triangleSubresource == modelNode->primaryAnimatedSubresourceIndex) {
                GraphicsPrimitiveQueue_OffsetTextureCoordinates
                          (modelNode->primaryTextureOffsetV,modelNode->primaryTextureOffsetU,
                           g_ActivePrimitiveQueue);
              }
              if (((modelNode->runtimeFlags & MODEL_RUNTIME_FLAG_SECONDARY_TEXTURE_SCROLL) != 0) &&
                 (triangleSubresource == modelNode->secondaryAnimatedSubresourceIndex)) {
                GraphicsPrimitiveQueue_OffsetTextureCoordinates
                          (modelNode->secondaryTextureOffsetV,modelNode->secondaryTextureOffsetU,
                           g_ActivePrimitiveQueue);
                return;
              }
            }
          }
        }
      }
    }
  }
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
               (GraphicsFixedMatrix3x4 *)&g_ModelViewCompositeTransform);
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
  vertex[3].z = materialPackedColor | 0xffffff;
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
  uint32_t subresourceOrPaletteBank;
  bool rejected;
  GraphicsTextureSetEntry *textureEntry;

  firstVertex = triangle->vertex0;
  secondVertex = triangle->vertex1;
  thirdVertex = triangle->vertex2;
  rejected = ModelRender_PrepareProjectedVertexAlternatePath
                    (modelNode,triangle,(GraphicsFixedVec3 *)firstVertex);
  if (!rejected) {
    rejected = ModelRender_PrepareProjectedVertexAlternatePath
                      (modelNode,triangle,(GraphicsFixedVec3 *)secondVertex);
    if (!rejected) {
      rejected = ModelRender_PrepareProjectedVertexAlternatePath
                        (modelNode,triangle,(GraphicsFixedVec3 *)thirdVertex);
      if (!rejected) {
        if ((g_ProjectionClipRect.minX <= firstVertex->screenX ||
             g_ProjectionClipRect.minX <= secondVertex->screenX ||
             g_ProjectionClipRect.minX <= thirdVertex->screenX) &&
            (firstVertex->screenX < g_ProjectionClipRect.maxX ||
             secondVertex->screenX < g_ProjectionClipRect.maxX ||
             thirdVertex->screenX < g_ProjectionClipRect.maxX)) {
          if ((g_ProjectionClipRect.minY <= firstVertex->screenY ||
               g_ProjectionClipRect.minY <= secondVertex->screenY ||
               g_ProjectionClipRect.minY <= thirdVertex->screenY) &&
              (firstVertex->screenY < g_ProjectionClipRect.maxY ||
               secondVertex->screenY < g_ProjectionClipRect.maxY ||
               thirdVertex->screenY < g_ProjectionClipRect.maxY)) {
            rejected = GraphicsPrimitiveQueue_AppendTriangle
                              (triangle->renderFlags,triangle,triangle->vertex2,triangle->vertex1,
                               triangle->vertex0,g_ActivePrimitiveQueue);
            if (!rejected) {
              GraphicsPrimitiveQueue_SetVertexColors
                        (triangle->vertex2->vertexColorArgb,triangle->vertex1->vertexColorArgb,
                         triangle->vertex0->vertexColorArgb,g_ActivePrimitiveQueue);
              nodeTextureSet = (modelNode->modelPayload).textureSet;
              subresourceOrPaletteBank = triangle->subresourceIndex;
              textureEntry = NULL;
              /* unlike ModelRender_SubmitTriangle there is no NULL check of the texture set */
              if (subresourceOrPaletteBank != 0xffffffff &&
                  subresourceOrPaletteBank < nodeTextureSet->subresourceCount) {
                textureEntry = nodeTextureSet->entries + subresourceOrPaletteBank +
                               modelNode->textureSubresourceBaseIndex;
              }
              nodePaletteAsset = (modelNode->modelPayload).paletteAsset;
              /* the original masks with 0xFFFF01FF here (0x1FF in ModelRender_SubmitTriangle) */
              if ((nodePaletteAsset != NULL) &&
                 (subresourceOrPaletteBank = triangle->renderFlags & 0xffff01ff,
                  subresourceOrPaletteBank < nodePaletteAsset->paletteBankCount)) {
                GraphicsPrimitiveQueue_SetMaterial
                          (nodePaletteAsset->paletteEntries[subresourceOrPaletteBank].alternateModulationColorArgb,
                           textureEntry,g_ActivePrimitiveQueue);
                return;
              }
              GraphicsPrimitiveQueue_SetMaterial(0,textureEntry,g_ActivePrimitiveQueue);
            }
          }
        }
      }
    }
  }
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
                                  (GraphicsFixedVec3 *)&g_ModelViewDirectionLocal);
  return facingDotQ12;
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
  uint32_t squareOrRemainderLow;
  int axisDelta;
  uint32_t radiusOrSquareLow;
  GraphicsShadingRecordCount remainingRecords;
  uint32_t remainderLowOrDivisor;
  GraphicsShadingRuntimeRecord *shadingRecord;
  uint64_t directionalLanes;
  uint64_t accumulatedLanes;
  uint64_t lightLanes;

  lightFacingDotQ12 = FixedVec3_DotQ12(lightDirectionQ12,surfaceNormalQ12);
  directionalLanes =
       pmulhw(ModelLighting_UnpackBytesMmx(scenePackedColor1,2),
              *(uint64_t *)(distanceAttenuationTable + (lightFacingDotQ12 >> 0x15) * 8));
  shadingRecord = g_GraphicsShadingNearbyRecords;
  accumulatedLanes =
       pmulhw(ModelLighting_AddWordsMmx(directionalLanes,ModelLighting_UnpackBytesMmx(scenePackedColor0,4)),
              ModelLighting_UnpackBytesMmx(materialPackedColor,2));
  for (remainingRecords = g_GraphicsShadingNearbyRecordCount; remainingRecords != 0; remainingRecords--) {
    if (shadingRecord->targetRadiusQ12 != 0) {
      /* r^2 - dx^2 - dy^2 - dz^2 in 64 bits; the light reaches the vertex while it stays >= 0 */
      radiusOrSquareLow = (uint32_t)shadingRecord->squaredRadiusQ24;
      remainderHigh = *vertexPositionQ12 - shadingRecord->worldXQ12;
      axisDistanceSquared = (int64_t)remainderHigh * (int64_t)remainderHigh;
      squareOrRemainderLow = (uint32_t)axisDistanceSquared;
      remainderLowOrDivisor = radiusOrSquareLow - squareOrRemainderLow;
      /* high dword of r^2 */
      remainderHigh = (((int *)&shadingRecord->squaredRadiusQ24)[1] -
               (int)((uint64_t)axisDistanceSquared >> 0x20)) - (uint32_t)(radiusOrSquareLow < squareOrRemainderLow);
      if (-1 < remainderHigh) {
        axisDelta = vertexPositionQ12[1] - shadingRecord->worldYQ12;
        axisDistanceSquared = (int64_t)axisDelta * (int64_t)axisDelta;
        radiusOrSquareLow = (uint32_t)axisDistanceSquared;
        squareOrRemainderLow = remainderLowOrDivisor - radiusOrSquareLow;
        remainderHigh = (remainderHigh - (int)((uint64_t)axisDistanceSquared >> 0x20)) -
                        (uint32_t)(remainderLowOrDivisor < radiusOrSquareLow);
        if (-1 < remainderHigh) {
          axisDelta = vertexPositionQ12[2] - shadingRecord->worldZQ12;
          axisDistanceSquared = (int64_t)axisDelta * (int64_t)axisDelta;
          radiusOrSquareLow = (uint32_t)axisDistanceSquared;
          remainderHigh = (remainderHigh - (int)((uint64_t)axisDistanceSquared >> 0x20)) -
                          (uint32_t)(squareOrRemainderLow < radiusOrSquareLow);
          if (-1 < remainderHigh) {
            lightPackedColor = shadingRecord->packedColorRgbActive;
            /* divisor r^2 >> 12 (SHRD) */
            remainderLowOrDivisor = ((int *)&shadingRecord->squaredRadiusQ24)[1] << 0x14 |
                     (uint32_t)shadingRecord->squaredRadiusQ24 >> 0xc;
            if (remainderLowOrDivisor != 0) {
              lightLanes =
                   pmulhw(ModelLighting_UnpackBytesMmx(lightPackedColor,2),
                          g_PackedLightingLookupTable[(remainderHigh * 0x8000000 |
                                                       (squareOrRemainderLow - radiusOrSquareLow) >> 5) /
                                                      remainderLowOrDivisor]);
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
   triangles: the directional weight comes from g_ModelLightingScaleMmxMultiplierTable, indexed by the facing dot
   divided by the model resource's lightingScaleQ12 (>> 9).
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
  uint32_t squareOrRemainderLow;
  int axisDelta;
  uint32_t radiusOrSquareLow;
  GraphicsShadingRecordCount remainingRecords;
  uint32_t remainderLowOrDivisor;
  GraphicsShadingRuntimeRecord *shadingRecord;
  uint64_t directionalLanes;
  uint64_t accumulatedLanes;
  uint64_t resultLanes;
  uint64_t lightLanes;

  lightFacingDotQ12 = FixedVec3_DotQ12(lightDirectionQ12,surfaceNormalQ12);
  directionalLanes =
       pmulhw(ModelLighting_UnpackBytesMmx(scenePackedColor1,2),
              *(uint64_t *)
               (&g_ModelLightingScaleMmxMultiplierTable + (lightFacingDotQ12 / lightingScaleQ12 >> 9) * 8));
  shadingRecord = g_GraphicsShadingNearbyRecords;
  accumulatedLanes =
       pmulhw(ModelLighting_AddWordsMmx(directionalLanes,ModelLighting_UnpackBytesMmx(scenePackedColor0,4)),
              ModelLighting_UnpackBytesMmx(materialPackedColor,2));
  for (remainingRecords = g_GraphicsShadingNearbyRecordCount; remainingRecords != 0; remainingRecords--) {
    if (shadingRecord->targetRadiusQ12 != 0) {
      /* r^2 - dx^2 - dy^2 - dz^2 in 64 bits; the light reaches the vertex while it stays >= 0 */
      radiusOrSquareLow = (uint32_t)shadingRecord->squaredRadiusQ24;
      remainderHigh = *vertexPositionQ12 - shadingRecord->worldXQ12;
      axisDistanceSquared = (int64_t)remainderHigh * (int64_t)remainderHigh;
      squareOrRemainderLow = (uint32_t)axisDistanceSquared;
      remainderLowOrDivisor = radiusOrSquareLow - squareOrRemainderLow;
      /* high dword of r^2 */
      remainderHigh = (((int *)&shadingRecord->squaredRadiusQ24)[1] -
               (int)((uint64_t)axisDistanceSquared >> 0x20)) - (uint32_t)(radiusOrSquareLow < squareOrRemainderLow);
      if (-1 < remainderHigh) {
        axisDelta = vertexPositionQ12[1] - shadingRecord->worldYQ12;
        axisDistanceSquared = (int64_t)axisDelta * (int64_t)axisDelta;
        radiusOrSquareLow = (uint32_t)axisDistanceSquared;
        squareOrRemainderLow = remainderLowOrDivisor - radiusOrSquareLow;
        remainderHigh = (remainderHigh - (int)((uint64_t)axisDistanceSquared >> 0x20)) -
                        (uint32_t)(remainderLowOrDivisor < radiusOrSquareLow);
        if (-1 < remainderHigh) {
          axisDelta = vertexPositionQ12[2] - shadingRecord->worldZQ12;
          axisDistanceSquared = (int64_t)axisDelta * (int64_t)axisDelta;
          radiusOrSquareLow = (uint32_t)axisDistanceSquared;
          remainderHigh = (remainderHigh - (int)((uint64_t)axisDistanceSquared >> 0x20)) -
                          (uint32_t)(squareOrRemainderLow < radiusOrSquareLow);
          if (-1 < remainderHigh) {
            lightPackedColor = shadingRecord->packedColorRgbActive;
            /* divisor r^2 >> 12 (SHRD) */
            remainderLowOrDivisor = ((int *)&shadingRecord->squaredRadiusQ24)[1] << 0x14 |
                     (uint32_t)shadingRecord->squaredRadiusQ24 >> 0xc;
            if (remainderLowOrDivisor != 0) {
              lightLanes =
                   pmulhw(ModelLighting_UnpackBytesMmx(lightPackedColor,2),
                          g_PackedLightingLookupTable[(remainderHigh * 0x8000000 |
                                                       (squareOrRemainderLow - radiusOrSquareLow) >> 5) /
                                                      remainderLowOrDivisor]);
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

  scenePackedColor0 = scenePackedColor0 | 0xff000000;
  FixedTransform_ApplyDirection
            (&g_ModelLightingTransformedSurfaceNormalScratch,surfaceNormalQ12,
             (GraphicsFixedMatrix3x4 *)&g_ModelViewCompositeTransform);
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
    pixelX = projectedPoint.projectedX >> 0xc;
    pixelY = projectedPoint.projectedY >> 0xc;
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
            ((GraphicsFixedMatrix3x4 *)&g_ModelViewCompositeTransform,nodeWorldTransform,
             &g_ViewProjectionMatrixFixed);
  viewAngles = FixedMath_VectorToAngles3Regs
                    (nodeWorldZ - g_ViewOriginFixed.z,nodeWorldY - g_ViewOriginFixed.y,
                     nodeWorldX - g_ViewOriginFixed.x);
  FixedMath_WriteDirectionQ28
            ((GraphicsFixedVec3 *)&g_ModelViewDirectionWorld,viewAngles.elevationAngle,viewAngles.azimuthAngle);
  FixedTransform_ApplyTransposeDirection
            ((GraphicsFixedVec3 *)&g_ModelViewDirectionLocal,nodeWorldTransform,
             (GraphicsFixedVec3 *)&g_ModelViewDirectionWorld);
  FixedTransform_ApplyTransposeDirection
            ((GraphicsFixedVec3 *)&g_ModelAuxiliaryForwardDirectionLocal,nodeWorldTransform,
             &g_AuxiliaryForwardDirectionFixed);
}

