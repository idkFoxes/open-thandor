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


/* MMX lane helpers for the rewritten lighting routines (Intel SDM semantics). */

/* movd + punpcklbw mm,mm + psrlw mm,shift: each byte b becomes the word (b * 0x101) >> shift. */
static void ModelLighting_UnpackBytes(uint packed, int shift, short lanes[4])
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
  uint packed = 0;
  int i;
  for (i = 0; i < 4; i++) {
    int v = lanes[i] < 0 ? 0 : lanes[i] > 0xff ? 0xff : lanes[i];
    packed |= (uint)v << (8 * i);
  }
  return packed;
}


/* Address: 0x004BDC90.
   Ownership: graphics/render/model.
   Purpose: Applies optional view-facing rotations, draws every enabled mesh group, and restores the model
   transform and position fields afterward. Typed parameters: p2 facingThresholdQ12→Q12. Nearby but non-identical
   semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes, control flow,
   globals, locals, and executable data remain unchanged. Typed parameters: p3
   meshGroup→ModelMeshGroupAddress32_V345.
   Local calls: ModelRender_SubmitMeshTriangles.
   Cross-module calls: ModelNodeRuntime_BuildViewFacingRotation [world/model/hierarchy],
   ModelNodeRuntime_BuildBillboardRotation [world/model/hierarchy].
*/
void __thandor_void_preserve_eax_ecx_edx
ModelRender_DrawMeshGroupsWithTemporaryTransform
          (Q12 facingThresholdQ12,ModelMeshGroupAddress32 meshGroup,ModelRuntimeNode *modelNode)

{
  uint groupFlagsOrMask;
  AngleTurn32 savedRotationAngle0;
  AngleTurn32 savedRotationAngle1;
  AngleTurn32 savedRotationAngle2;
  sdword savedBasis00;
  sdword savedBasis01;
  sdword savedBasis02;
  GraphicsWorldCoordinateQ12 savedTranslationX;
  sdword savedBasis10;
  sdword savedBasis11;
  sdword savedBasis12;
  GraphicsWorldCoordinateQ12 savedTranslationY;
  sdword savedBasis20;
  sdword savedBasis21;
  sdword savedBasis22;
  GraphicsWorldCoordinateQ12 savedTranslationZ;
  int remainingMeshCount;
  int *meshRecord;
  
  groupFlagsOrMask = *(uint *)(meshGroup + 0xc);
  remainingMeshCount = *(int *)(meshGroup + 4);
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
  if ((groupFlagsOrMask & 1) != 0) {
    ModelNodeRuntime_BuildViewFacingRotation(modelNode);
  }
  if ((groupFlagsOrMask & 2) != 0) {
    ModelNodeRuntime_BuildBillboardRotation(modelNode);
  }
  meshRecord = (int *)(meshGroup + 0x20);
  groupFlagsOrMask = (modelNode->modelPayload).meshGroupMask;
  for (; remainingMeshCount != 0; remainingMeshCount = remainingMeshCount + -1) {
    if ((meshRecord[1] & groupFlagsOrMask) != 0) {
      ModelRender_SubmitMeshTriangles
                (facingThresholdQ12,(ModelMeshGroupAddress32)meshRecord,modelNode);
    }
    meshRecord = (int *)((int)meshRecord + *meshRecord);
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
  return;
}


/* Address: 0x004BE1F0.
   Ownership: graphics/render/model.
   Purpose: Handles model render draw mesh groups alternate path.
   Local calls: ModelRender_SubmitMeshTrianglesAlternatePath.
   Cross-module calls: ModelNodeRuntime_BuildViewFacingRotation [world/model/hierarchy],
   ModelNodeRuntime_BuildBillboardRotation [world/model/hierarchy].
*/
void __thandor_void_preserve_eax_ecx_edx
ModelRender_DrawMeshGroupsAlternatePath
          (ModelMeshGroupAddress32 meshGroup,ModelRuntimeNode *modelNode)

{
  uint groupFlagsOrMask;
  int remainingMeshCount;
  int *meshRecord;
  
  groupFlagsOrMask = *(uint *)(meshGroup + 0xc);
  remainingMeshCount = *(int *)(meshGroup + 4);
  if ((groupFlagsOrMask & 1) != 0) {
    ModelNodeRuntime_BuildViewFacingRotation(modelNode);
  }
  if ((groupFlagsOrMask & 2) != 0) {
    ModelNodeRuntime_BuildBillboardRotation(modelNode);
  }
  meshRecord = (int *)(meshGroup + 0x20);
  groupFlagsOrMask = (modelNode->modelPayload).meshGroupMask;
  for (; remainingMeshCount != 0; remainingMeshCount = remainingMeshCount + -1) {
    if ((meshRecord[1] & groupFlagsOrMask) != 0) {
      ModelRender_SubmitMeshTrianglesAlternatePath((ModelMeshGroupAddress32)meshRecord,modelNode);
    }
    meshRecord = (int *)((int)meshRecord + *meshRecord);
  }
  return;
}


/* Address: 0x0050A5C0.
   Ownership: graphics/render/model.
   Purpose: Handles model projected bounds accumulate hierarchy recursive.
   Local calls: ModelProjectedBounds_AccumulateNode.
*/
void __thandor_void_preserve_eax_ecx
ModelProjectedBounds_AccumulateHierarchyRecursive
          (ModelProjectedBoundsPixels *bounds,ModelRuntimeNode *modelNode)

{
  dword remainingChildCount;
  
  ModelProjectedBounds_AccumulateNode(bounds,modelNode);
  for (remainingChildCount = modelNode->childCount; remainingChildCount != 0; remainingChildCount = remainingChildCount - 1) {
    if (modelNode->childNodes[0] != (ModelRuntimeNode *)0x0) {
      ModelProjectedBounds_AccumulateHierarchyRecursive(bounds,modelNode->childNodes[0]);
    }
    modelNode = (ModelRuntimeNode *)&(modelNode->common).nextNode;
  }
  return;
}


/* Address: 0x004BD4B0.
   Ownership: graphics/render/model.
   Purpose: Transforms and projects one model vertex on demand, caches screen coordinates and render flags, and
   computes the vertex intensity through the normal or alternate lighting path. THE per-instance draw-scale
   application: when node runtimeFlags (+0x4C) & 0x800, each vertex component is (v * modelScaleQ12(+0xC0)) >> 12
   before view transform + projection. No 0x3E8/milli constant exists in the mesh stamp path. Typed parameters: p2
   modelNode→ModelRuntimeNode *. Calling convention, complete VariableStorage serialization, function bytes,
   control flow, globals, locals, and executable data remain unchanged.
   Local calls: ModelRender_ComputeVertexIntensityDefaultPath, ModelRender_ComputeVertexIntensityScaledPath.
   Cross-module calls: FixedTransform_ApplyPoint [core/math/fixed], Graphics_ProjectViewPoint
   [graphics/core/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
ModelRender_PrepareProjectedVertex
          (ModelRuntimeNode *modelNode,ModelMeshGroupAddress32 meshGroup,GraphicsFixedVec3 *vertex)

{
  uint triangleRenderFlags;
  longlong scaledCoordinateProduct;
  PackedArgb32 vertexColor;
  uint depthBiasHalf;
  GraphicsFixedVec3 *surfaceNormalQ12;
  GraphicsProjectedPointEdxEax8 projectedScreenCoordinatePair;
  GraphicsWorldCoordinateQ12 savedVertexXQ12;
  longlong scaledVertexCoordinateProduct;
  GraphicsWorldCoordinateQ12 savedVertexZQ12;
  GraphicsWorldCoordinateQ12 savedVertexYQ12;
  
  triangleRenderFlags = *(uint *)(meshGroup + 0x34);
  if (vertex[4].x == -0x80000000) {
    savedVertexXQ12 = vertex->x;
    savedVertexYQ12 = vertex->y;
    savedVertexZQ12 = vertex->z;
    depthBiasHalf = (uint)modelNode->renderDepthBiasOrState >> 1;
    if (-1 < vertex->z) {
      if (vertex->z != 0) {
        vertex->z = vertex->z + depthBiasHalf;
      }
      vertex->z = vertex->z + depthBiasHalf;
    }
    if ((modelNode->runtimeFlags & 0x800) != 0) {
      scaledVertexCoordinateProduct = (longlong)vertex->x * (longlong)modelNode->modelScaleQ12;
      vertex->x = (int)((ulonglong)scaledVertexCoordinateProduct >> 0x20) << 0x14 |
                  (uint)scaledVertexCoordinateProduct >> 0xc;
      scaledCoordinateProduct = (longlong)vertex->y * (longlong)modelNode->modelScaleQ12;
      vertex->y = (int)((ulonglong)scaledCoordinateProduct >> 0x20) << 0x14 | (uint)scaledCoordinateProduct >> 0xc;
      scaledCoordinateProduct = (longlong)vertex->z * (longlong)modelNode->modelScaleQ12;
      vertex->z = (int)((ulonglong)scaledCoordinateProduct >> 0x20) << 0x14 | (uint)scaledCoordinateProduct >> 0xc;
    }
    FixedTransform_ApplyPoint
              ((GraphicsFixedVec3 *)&vertex[2].z,vertex,
               (GraphicsFixedMatrix3x4 *)&g_ModelViewCompositeTransform);
    projectedScreenCoordinatePair =
         THANDOR_BITCAST(GraphicsProjectedPointPair, GraphicsProjectedPointEdxEax8, Graphics_ProjectViewPoint((GraphicsFixedVec3 *)&vertex[2].z))
    ;
    vertex->z = savedVertexZQ12;
    vertex->y = savedVertexYQ12;
    vertex->x = savedVertexXQ12;
    vertex[4].x = (int)projectedScreenCoordinatePair;
    vertex[4].y = (int)(projectedScreenCoordinatePair >> 0x20);
  }
  else if (((triangleRenderFlags & 0x8e00) == vertex[4].z) && ((triangleRenderFlags & 0x8000) == 0)) {
    return;
  }
  vertex[4].z = triangleRenderFlags & 0x8e00;
  vertexColor = modelNode->tintArgb;
  surfaceNormalQ12 = (GraphicsFixedVec3 *)&vertex[1].y;
  if ((triangleRenderFlags & 0x200) != 0) {
    vertex[3].z = vertexColor;
    return;
  }
  if ((triangleRenderFlags & 0x800) == 0) {
    if ((triangleRenderFlags & 0x8000) != 0) {
      surfaceNormalQ12 = (GraphicsFixedVec3 *)(meshGroup + 0x24);
    }
    vertexColor = ModelRender_ComputeVertexIntensityDefaultPath
                      (vertex[2].y,&vertex[2].z,THANDOR_ADDR(g_ModelDistanceAttenuationMmx,0),g_SceneBoundsFixed.bound5,
                       g_SceneBoundsFixed.bound4,
                       (GraphicsFixedVec3 *)&g_ModelAuxiliaryForwardDirectionLocal,vertexColor,
                       surfaceNormalQ12);
    vertex[3].z = vertexColor;
    return;
  }
  vertexColor = ModelRender_ComputeVertexIntensityScaledPath
                    (vertex[2].y,&vertex[2].z,
                     ((modelNode->modelPayload).modelResource)->lightingScaleQ12,
                     g_SceneBoundsFixed.bound7,g_SceneBoundsFixed.bound6,
                     (GraphicsFixedVec3 *)&g_ModelAuxiliaryForwardDirectionLocal,vertexColor,vertex);
  vertex[3].z = vertexColor;
  return;
}


/* Address: 0x004BD9B0.
   Ownership: graphics/render/model.
   Purpose: Performs facing and viewport tests, prepares projected vertices, appends one triangle to the primitive
   queue, selects texture and material state, and applies configured texture-coordinate offsets.
   Local calls: ModelRender_ComputeFacingDotQ12, ModelRender_PrepareProjectedVertex.
   Cross-module calls: GraphicsPrimitiveQueue_AppendTriangle [graphics/render/primitives],
   GraphicsPrimitiveQueue_SetVertexColors [graphics/render/primitives], GraphicsPrimitiveQueue_SetMaterial
   [graphics/render/primitives], GraphicsPrimitiveQueue_OffsetTextureCoordinates [graphics/render/primitives].
*/
void __thandor_void_preserve_eax_ecx_edx
ModelRender_SubmitTriangle
          (Q12 facingThresholdQ12,GraphicsTriangleInput *triangle,ModelRuntimeNode *modelNode)

{
  GraphicsProjectedVertexSource *firstVertex;
  GraphicsProjectedVertexSource *secondVertex;
  GraphicsProjectedVertexSource *thirdVertex;
  GraphicsTextureSet *nodeTextureSet;
  GraphicsPaletteAsset *nodePaletteAsset;
  GraphicsSubresourceIndex triangleSubresource;
  sdword facingDotQ12;
  uint paletteBankIndex;
  bool appendFailed;
  GraphicsTextureSetEntry *textureEntry;
  
  facingDotQ12 = ModelRender_ComputeFacingDotQ12(triangle);
  if (((triangle->renderFlags & 0x400) != 0) || (facingDotQ12 < facingThresholdQ12)) {
    firstVertex = triangle->vertex0;
    secondVertex = triangle->vertex1;
    thirdVertex = triangle->vertex2;
    ModelRender_PrepareProjectedVertex
              (modelNode,(ModelMeshGroupAddress32)triangle,(GraphicsFixedVec3 *)firstVertex);
    ModelRender_PrepareProjectedVertex
              (modelNode,(ModelMeshGroupAddress32)triangle,(GraphicsFixedVec3 *)secondVertex);
    ModelRender_PrepareProjectedVertex
              (modelNode,(ModelMeshGroupAddress32)triangle,(GraphicsFixedVec3 *)thirdVertex);
    if (((g_ProjectionClipRect.minX <= firstVertex->screenX) ||
        ((g_ProjectionClipRect.minX <= secondVertex->screenX ||
         (g_ProjectionClipRect.minX <= thirdVertex->screenX)))) &&
       ((firstVertex->screenX < g_ProjectionClipRect.maxX ||
        ((secondVertex->screenX < g_ProjectionClipRect.maxX ||
         (thirdVertex->screenX < g_ProjectionClipRect.maxX)))))) {
      if (((g_ProjectionClipRect.minY <= firstVertex->screenY) ||
          ((g_ProjectionClipRect.minY <= secondVertex->screenY ||
           (g_ProjectionClipRect.minY <= thirdVertex->screenY)))) &&
         ((firstVertex->screenY < g_ProjectionClipRect.maxY ||
          ((secondVertex->screenY < g_ProjectionClipRect.maxY ||
           (thirdVertex->screenY < g_ProjectionClipRect.maxY)))))) {
        appendFailed = GraphicsPrimitiveQueue_AppendTriangle
                          (triangle->renderFlags,triangle,triangle->vertex2,triangle->vertex1,
                           triangle->vertex0,g_ActivePrimitiveQueue);
        if (!appendFailed) {
          GraphicsPrimitiveQueue_SetVertexColors
                    (triangle->vertex2->vertexColorArgb,triangle->vertex1->vertexColorArgb,
                     triangle->vertex0->vertexColorArgb,g_ActivePrimitiveQueue);
          nodeTextureSet = (modelNode->modelPayload).textureSet;
          textureEntry = (GraphicsTextureSetEntry *)0x0;
          if ((nodeTextureSet != (GraphicsTextureSet *)0x0) &&
             (triangle->subresourceIndex < nodeTextureSet->subresourceCount)) {
            textureEntry = nodeTextureSet->entries +
                           triangle->subresourceIndex + modelNode->textureSubresourceBaseIndex;
          }
          nodePaletteAsset = (modelNode->modelPayload).paletteAsset;
          if ((nodePaletteAsset == (GraphicsPaletteAsset *)0x0) ||
             (paletteBankIndex = triangle->renderFlags & 0x1ff, nodePaletteAsset->paletteBankCount <= paletteBankIndex)) {
            GraphicsPrimitiveQueue_SetMaterial(0xffffffff,textureEntry,g_ActivePrimitiveQueue);
            triangleSubresource = triangle->subresourceIndex;
            if ((modelNode->runtimeFlags & 0x80) != 0) {
              if (triangleSubresource == modelNode->primaryAnimatedSubresourceIndex) {
                GraphicsPrimitiveQueue_OffsetTextureCoordinates
                          (modelNode->primaryTextureOffsetV,modelNode->primaryTextureOffsetU,
                           g_ActivePrimitiveQueue);
              }
              if (((modelNode->runtimeFlags & 0x400) != 0) &&
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
            if ((modelNode->runtimeFlags & 0x80) != 0) {
              if (triangleSubresource == modelNode->primaryAnimatedSubresourceIndex) {
                GraphicsPrimitiveQueue_OffsetTextureCoordinates
                          (modelNode->primaryTextureOffsetV,modelNode->primaryTextureOffsetU,
                           g_ActivePrimitiveQueue);
              }
              if (((modelNode->runtimeFlags & 0x400) != 0) &&
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
  return;
}


/* Address: 0x004BDC20.
   Ownership: graphics/render/model.
   Purpose: Prepares model-local view directions, invalidates cached projected vertices, and submits every triangle
   in one mesh record. Typed parameters: p3 meshGroup→ModelMeshGroupAddress32_V345. Calling convention, complete
   VariableStorage serialization, function bytes, control flow, globals, locals, and executable data remain
   unchanged.
   Local calls: ModelRender_PrepareViewDirections, ModelRender_SubmitTriangle.
*/
void __thandor_void_preserve_eax_ecx_edx
ModelRender_SubmitMeshTriangles
          (Q12 facingThresholdQ12,ModelMeshGroupAddress32 meshGroup,ModelRuntimeNode *modelNode)

{
  int remainingCount;
  GraphicsTriangleInput *recordCursor;
  
  remainingCount = *(int *)(meshGroup + 8);
  ModelRender_PrepareViewDirections(modelNode);
  recordCursor = (GraphicsTriangleInput *)(meshGroup + 0x20);
  for (; remainingCount != 0; remainingCount = remainingCount + -1) {
    recordCursor->subresourceIndex = 0x80000000;
    recordCursor = (GraphicsTriangleInput *)&recordCursor[1].textureV0;
  }
  for (remainingCount = *(int *)(meshGroup + 0xc); remainingCount != 0; remainingCount = remainingCount + -1) {
    ModelRender_SubmitTriangle(facingThresholdQ12,recordCursor,modelNode);
    recordCursor = (GraphicsTriangleInput *)&recordCursor[1].textureV0;
  }
  return;
}


/* Address: 0x004BE180.
   Ownership: graphics/render/model.
   Purpose: Handles model render submit mesh triangles alternate path.
   Local calls: ModelRender_PrepareViewDirections, ModelRender_SubmitTriangleAlternatePath.
*/
void __thandor_void_preserve_eax_ecx_edx
ModelRender_SubmitMeshTrianglesAlternatePath
          (ModelMeshGroupAddress32 meshGroup,ModelRuntimeNode *modelNode)

{
  int remainingCount;
  GraphicsTriangleInput *recordCursor;
  
  remainingCount = *(int *)(meshGroup + 8);
  ModelRender_PrepareViewDirections(modelNode);
  recordCursor = (GraphicsTriangleInput *)(meshGroup + 0x20);
  for (; remainingCount != 0; remainingCount = remainingCount + -1) {
    recordCursor->subresourceIndex = 0x80000000;
    recordCursor = (GraphicsTriangleInput *)&recordCursor[1].textureV0;
  }
  for (remainingCount = *(int *)(meshGroup + 0xc); remainingCount != 0; remainingCount = remainingCount + -1) {
    ModelRender_SubmitTriangleAlternatePath(recordCursor,modelNode);
    recordCursor = (GraphicsTriangleInput *)&recordCursor[1].textureV0;
  }
  return;
}


/* Address: 0x004BD6B0.
   Ownership: graphics/render/model.
   Purpose: Handles model render prepare projected vertex alternate path.
   Local calls: ModelRender_ComputeNearbyLightPackedVertexColorAlternatePath.
   Cross-module calls: FixedTransform_ApplyPoint [core/math/fixed], Graphics_ProjectViewPoint
   [graphics/core/runtime].
*/
bool __thandor_cf_preserve_eax_ecx_edx
ModelRender_PrepareProjectedVertexAlternatePath
          (ModelRuntimeNode *modelNode,GraphicsTriangleInput *triangle,GraphicsFixedVec3 *vertex)

{
  uint triangleRenderFlags;
  uint materialPackedColor;
  PackedArgb32 vertexColor;
  GraphicsFixedVec3 *surfaceNormalQ12;
  GraphicsProjectedPointPair projectedPoint;
  
  triangleRenderFlags = triangle->renderFlags;
  if (vertex[4].x == -0x80000000) {
    FixedTransform_ApplyPoint
              ((GraphicsFixedVec3 *)&vertex[2].z,vertex,
               (GraphicsFixedMatrix3x4 *)&g_ModelViewCompositeTransform);
    if (vertex[3].y < (int)g_ProjectionScaleFixed) {
LAB_004bd788:
      vertex[4].x = 0x7fffffff;
      return true;
    }
    projectedPoint = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)&vertex[2].z);
    vertex[4].x = projectedPoint.projectedX;
    vertex[4].y = projectedPoint.projectedY;
  }
  else {
    if (vertex[4].x == 0x7fffffff) goto LAB_004bd788;
    if (((triangleRenderFlags & 0x8e00) == vertex[4].z) && ((triangleRenderFlags & 0x8000) == 0)) {
      return false;
    }
  }
  materialPackedColor = modelNode->tintArgb;
  vertex[4].z = triangleRenderFlags & 0x8e00;
  if ((triangleRenderFlags & 0x200) == 0) {
    surfaceNormalQ12 = (GraphicsFixedVec3 *)&vertex[1].y;
    if ((triangleRenderFlags & 0x8000) != 0) {
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
   Ownership: graphics/render/model.
   Purpose: Handles model render submit triangle alternate path.
   Local calls: ModelRender_PrepareProjectedVertexAlternatePath.
   Cross-module calls: GraphicsPrimitiveQueue_AppendTriangle [graphics/render/primitives],
   GraphicsPrimitiveQueue_SetVertexColors [graphics/render/primitives], GraphicsPrimitiveQueue_SetMaterial
   [graphics/render/primitives].
*/
void __thandor_void_preserve_eax_ecx_edx
ModelRender_SubmitTriangleAlternatePath(GraphicsTriangleInput *triangle,ModelRuntimeNode *modelNode)

{
  GraphicsProjectedVertexSource *firstVertex;
  GraphicsProjectedVertexSource *secondVertex;
  GraphicsProjectedVertexSource *thirdVertex;
  GraphicsTextureSet *nodeTextureSet;
  GraphicsPaletteAsset *nodePaletteAsset;
  uint subresourceOrPaletteBank;
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
        if ((((g_ProjectionClipRect.minX <= firstVertex->screenX) ||
             (g_ProjectionClipRect.minX <= secondVertex->screenX)) ||
            (g_ProjectionClipRect.minX <= thirdVertex->screenX)) &&
           (((firstVertex->screenX < g_ProjectionClipRect.maxX ||
             (secondVertex->screenX < g_ProjectionClipRect.maxX)) ||
            (thirdVertex->screenX < g_ProjectionClipRect.maxX)))) {
          if ((((g_ProjectionClipRect.minY <= firstVertex->screenY) ||
               (g_ProjectionClipRect.minY <= secondVertex->screenY)) ||
              (g_ProjectionClipRect.minY <= thirdVertex->screenY)) &&
             (((firstVertex->screenY < g_ProjectionClipRect.maxY ||
               (secondVertex->screenY < g_ProjectionClipRect.maxY)) ||
              (thirdVertex->screenY < g_ProjectionClipRect.maxY)))) {
            rejected = GraphicsPrimitiveQueue_AppendTriangle
                              (triangle->renderFlags,triangle,triangle->vertex2,triangle->vertex1,
                               triangle->vertex0,g_ActivePrimitiveQueue);
            if (!rejected) {
              GraphicsPrimitiveQueue_SetVertexColors
                        (triangle->vertex2->vertexColorArgb,triangle->vertex1->vertexColorArgb,
                         triangle->vertex0->vertexColorArgb,g_ActivePrimitiveQueue);
              nodeTextureSet = (modelNode->modelPayload).textureSet;
              subresourceOrPaletteBank = triangle->subresourceIndex;
              textureEntry = (GraphicsTextureSetEntry *)0x0;
              if ((subresourceOrPaletteBank != 0xffffffff) && (subresourceOrPaletteBank < nodeTextureSet->subresourceCount)) {
                textureEntry = nodeTextureSet->entries + subresourceOrPaletteBank + modelNode->textureSubresourceBaseIndex;
              }
              nodePaletteAsset = (modelNode->modelPayload).paletteAsset;
              if ((nodePaletteAsset != (GraphicsPaletteAsset *)0x0) &&
                 (subresourceOrPaletteBank = triangle->renderFlags & 0xffff01ff, subresourceOrPaletteBank < nodePaletteAsset->paletteBankCount)) {
                GraphicsPrimitiveQueue_SetMaterial
                          (nodePaletteAsset->paletteEntries[subresourceOrPaletteBank].alternateModulationColorArgb,textureEntry,
                           g_ActivePrimitiveQueue);
                return;
              }
              GraphicsPrimitiveQueue_SetMaterial(0,textureEntry,g_ActivePrimitiveQueue);
            }
          }
        }
      }
    }
  }
  return;
}


/* Address: 0x0050A4A0.
   Ownership: graphics/render/model.
   Purpose: Handles model projected bounds accumulate node.
   Local calls: ModelProjectedBounds_ExpandWithCurrentScratchPoint.
   Cross-module calls: FixedTransform_Compose [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
ModelProjectedBounds_AccumulateNode(ModelProjectedBoundsPixels *bounds,ModelRuntimeNode *modelNode)

{
  ModelResourceHitTestAndRenderView210 *resource;
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
  return;
}


/* Address: 0x004BD7E0.
   Ownership: graphics/render/model.
   Purpose: Returns the Q12 dot product between a triangle direction vector and the prepared view direction.
   Cross-module calls: FixedVec3_DotQ12 [core/math/fixed].
*/
sdword __thandor_eax_preserve_ecx_edx
ModelRender_ComputeFacingDotQ12(GraphicsTriangleInput *triangle)

{
  sdword facingDotQ12;
  
  facingDotQ12 = FixedVec3_DotQ12((GraphicsFixedVec3 *)&triangle->planeNormalXQ12,
                                  (GraphicsFixedVec3 *)&g_ModelViewDirectionLocal);
  return facingDotQ12;
}


/* Address: 0x004CC710.
   Ownership: graphics/render/model.
   Purpose: Computes the default model-vertex intensity from the prepared direction, bounds, material inputs, and
   fixed lighting vectors, then clamps and returns the resulting intensity value. Typed parameters: p2
   vertexPackedColor→PackedArgb32, p8 materialPackedColor→PackedArgb32. Nearby but non-identical semantic domains
   were explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals, locals, and
   executable data remain unchanged. Typed parameters: p5 scenePackedColor0→PackedArgb32, p6
   scenePackedColor1→PackedArgb32.
   Cross-module calls: FixedVec3_DotQ12 [core/math/fixed].
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
  longlong axisDistanceSquared;
  short resultLane0;
  short resultLane1;
  short resultLane2;
  short resultLane3;
  ushort colorLane3Word;
  ushort materialLane3Word;
  byte mm0PackedValue0ByteLane3;
  sdword lightFacingDotQ12;
  int remainderHigh;
  uint squareOrRemainderLow;
  int axisDelta;
  uint radiusOrSquareLow;
  GraphicsShadingRecordCount remainingRecords;
  uint remainderLowOrDivisor;
  GraphicsShadingRuntimeRecord *shadingRecord1;
  undefined1 colorByte3Or1;
  undefined1 colorByte2;
  undefined8 mm0PackedValue0;
  undefined8 accumulatedLanes;
  undefined1 sceneByte3Or1;
  undefined1 sceneByte2;
  undefined1 materialByte3Or1;
  undefined1 materialByte2;
  undefined8 mm4PackedValue0;
  
  lightFacingDotQ12 = FixedVec3_DotQ12(lightDirectionQ12,surfaceNormalQ12);
  mm0PackedValue0ByteLane3 = (byte)(scenePackedColor1 >> 0x18);
  colorByte2 = (undefined1)(scenePackedColor1 >> 0x10);
  colorByte3Or1 = (undefined1)(scenePackedColor1 >> 8);
  sceneByte3Or1 = (undefined1)(scenePackedColor0 >> 0x18);
  colorLane3Word = CONCAT11(sceneByte3Or1,sceneByte3Or1);
  sceneByte2 = (undefined1)(scenePackedColor0 >> 0x10);
  sceneByte3Or1 = (undefined1)(scenePackedColor0 >> 8);
  materialByte3Or1 = (undefined1)(materialPackedColor >> 0x18);
  materialLane3Word = CONCAT11(materialByte3Or1,materialByte3Or1);
  materialByte2 = (undefined1)(materialPackedColor >> 0x10);
  materialByte3Or1 = (undefined1)(materialPackedColor >> 8);
  mm0PackedValue0 =
       pmulhw(CONCAT26(CONCAT11(mm0PackedValue0ByteLane3,mm0PackedValue0ByteLane3) >> 2,
                       CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm0PackedValue0ByteLane3,
                                                                    mm0PackedValue0ByteLane3),colorByte2
                                                          ),CONCAT14(colorByte2,scenePackedColor1)) >>
                                        0x20) >> 2,
                                CONCAT22(CONCAT11(colorByte3Or1,colorByte3Or1) >> 2,
                                         CONCAT11((char)scenePackedColor1,(char)scenePackedColor1)
                                         >> 2))),
              *(undefined8 *)(distanceAttenuationTable + (lightFacingDotQ12 >> 0x15) * 8));
  shadingRecord1 = g_GraphicsShadingNearbyRecords;
  accumulatedLanes = pmulhw(CONCAT26((short)((ulonglong)mm0PackedValue0 >> 0x30) + (colorLane3Word >> 4),
                           CONCAT24((short)((ulonglong)mm0PackedValue0 >> 0x20) +
                                    ((ushort)(CONCAT35(CONCAT21(colorLane3Word,sceneByte2),
                                                       CONCAT14(sceneByte2,scenePackedColor0)) >> 0x20)
                                    >> 4),CONCAT22((short)((ulonglong)mm0PackedValue0 >> 0x10) +
                                                   (CONCAT11(sceneByte3Or1,sceneByte3Or1) >> 4),
                                                   (short)mm0PackedValue0 +
                                                   (CONCAT11((char)scenePackedColor0,
                                                             (char)scenePackedColor0) >> 4)))),
                  CONCAT26(materialLane3Word >> 2,
                           CONCAT24((ushort)(CONCAT35(CONCAT21(materialLane3Word,materialByte2),
                                                      CONCAT14(materialByte2,materialPackedColor)) >> 0x20)
                                    >> 2,CONCAT22(CONCAT11(materialByte3Or1,materialByte3Or1) >> 2,
                                                  CONCAT11((char)materialPackedColor,
                                                           (char)materialPackedColor) >> 2))));
  for (remainingRecords = g_GraphicsShadingNearbyRecordCount; remainingRecords != 0; remainingRecords = remainingRecords - 1) {
    if (shadingRecord1->targetRadiusQ12 != 0) {
      radiusOrSquareLow = (uint)shadingRecord1->squaredRadiusQ24;
      remainderHigh = *vertexPositionQ12 - shadingRecord1->worldXQ12;
      axisDistanceSquared = (longlong)remainderHigh * (longlong)remainderHigh;
      squareOrRemainderLow = (uint)axisDistanceSquared;
      remainderLowOrDivisor = radiusOrSquareLow - squareOrRemainderLow;
      remainderHigh = (*(int *)((int)&shadingRecord1->squaredRadiusQ24 + 4) -
               (int)((ulonglong)axisDistanceSquared >> 0x20)) - (uint)(radiusOrSquareLow < squareOrRemainderLow);
      if (-1 < remainderHigh) {
        axisDelta = vertexPositionQ12[1] - shadingRecord1->worldYQ12;
        axisDistanceSquared = (longlong)axisDelta * (longlong)axisDelta;
        radiusOrSquareLow = (uint)axisDistanceSquared;
        squareOrRemainderLow = remainderLowOrDivisor - radiusOrSquareLow;
        remainderHigh = (remainderHigh - (int)((ulonglong)axisDistanceSquared >> 0x20)) - (uint)(remainderLowOrDivisor < radiusOrSquareLow);
        if (-1 < remainderHigh) {
          axisDelta = vertexPositionQ12[2] - shadingRecord1->worldZQ12;
          axisDistanceSquared = (longlong)axisDelta * (longlong)axisDelta;
          radiusOrSquareLow = (uint)axisDistanceSquared;
          remainderHigh = (remainderHigh - (int)((ulonglong)axisDistanceSquared >> 0x20)) - (uint)(squareOrRemainderLow < radiusOrSquareLow);
          if (-1 < remainderHigh) {
            lightPackedColor = shadingRecord1->packedColorRgbActive;
            remainderLowOrDivisor = *(int *)((int)&shadingRecord1->squaredRadiusQ24 + 4) << 0x14 |
                     (uint)shadingRecord1->squaredRadiusQ24 >> 0xc;
            if (remainderLowOrDivisor != 0) {
              colorByte3Or1 = (undefined1)(lightPackedColor >> 0x18);
              colorLane3Word = CONCAT11(colorByte3Or1,colorByte3Or1);
              colorByte2 = (undefined1)(lightPackedColor >> 0x10);
              colorByte3Or1 = (undefined1)(lightPackedColor >> 8);
              mm4PackedValue0 =
                   pmulhw(CONCAT26(colorLane3Word >> 2,
                                   CONCAT24((ushort)(CONCAT35(CONCAT21(colorLane3Word,colorByte2),
                                                              CONCAT14(colorByte2,lightPackedColor)) >> 0x20) >> 2,
                                            CONCAT22(CONCAT11(colorByte3Or1,colorByte3Or1) >> 2,
                                                     CONCAT11((char)lightPackedColor,(char)lightPackedColor) >> 2))),
                          g_PackedLightingLookupTable[(remainderHigh * 0x8000000 | squareOrRemainderLow - radiusOrSquareLow >> 5) / remainderLowOrDivisor]);
              accumulatedLanes = paddusb(accumulatedLanes,mm4PackedValue0);
            }
          }
        }
      }
    }
    shadingRecord1 = shadingRecord1 + 1;
  }
  colorByte3Or1 = (undefined1)(vertexPackedColor >> 0x18);
  colorLane3Word = CONCAT11(colorByte3Or1,colorByte3Or1);
  colorByte2 = (undefined1)(vertexPackedColor >> 0x10);
  colorByte3Or1 = (undefined1)(vertexPackedColor >> 8);
  accumulatedLanes = pmulhw(accumulatedLanes,CONCAT26(colorLane3Word >> 2,
                                  CONCAT24((ushort)(CONCAT35(CONCAT21(colorLane3Word,colorByte2),
                                                             CONCAT14(colorByte2,vertexPackedColor)) >>
                                                   0x20) >> 2,
                                           CONCAT22(CONCAT11(colorByte3Or1,colorByte3Or1) >> 2,
                                                    CONCAT11((char)vertexPackedColor,
                                                             (char)vertexPackedColor) >> 2))));
  resultLane0 = (short)accumulatedLanes;
  resultLane1 = (short)((ulonglong)accumulatedLanes >> 0x10);
  resultLane2 = (short)((ulonglong)accumulatedLanes >> 0x20);
  resultLane3 = (short)((ulonglong)accumulatedLanes >> 0x30);
  return CONCAT13((0 < resultLane3) * (resultLane3 < 0x100) * (char)((ulonglong)accumulatedLanes >> 0x30) - (0xff < resultLane3)
                  ,CONCAT12((0 < resultLane2) * (resultLane2 < 0x100) * (char)((ulonglong)accumulatedLanes >> 0x20) -
                            (0xff < resultLane2),
                            CONCAT11((0 < resultLane1) * (resultLane1 < 0x100) *
                                     (char)((ulonglong)accumulatedLanes >> 0x10) - (0xff < resultLane1),
                                     (0 < resultLane0) * (resultLane0 < 0x100) * (char)accumulatedLanes - (0xff < resultLane0)))
                 );
}

/* Address: 0x004CC820.
   Ownership: graphics/render/model.
   Purpose: Storage remains one signed 32-bit word. Typed parameters: p4 lightingScaleQ12→Q12. Calling convention,
   storage, body bytes, control flow, and executable data remain unchanged. Typed parameters: p2
   vertexPackedColor→PackedArgb32, p8 materialPackedColor→PackedArgb32. Nearby but non-identical semantic domains
   were explicitly deferred.
   Cross-module calls: FixedVec3_DotQ12 [core/math/fixed].
*/
PackedArgb32
ModelRender_ComputeVertexIntensityScaledPath
          (PackedArgb32 vertexPackedColor,int *vertexPositionQ12,Q12 lightingScaleQ12,
          PackedArgb32 scenePackedColor0,PackedArgb32 scenePackedColor1,
          GraphicsFixedVec3 *lightDirectionQ12,PackedArgb32 materialPackedColor,
          GraphicsFixedVec3 *surfaceNormalQ12)

{
  PackedRgb24 lightPackedColor;
  longlong axisDistanceSquared;
  short resultLane0;
  short resultLane1;
  short resultLane2;
  short resultLane3;
  ushort colorLane3Word;
  ushort materialLane3Word;
  byte mm0PackedValue0ByteLane3;
  sdword lightFacingDotQ12;
  int remainderHigh;
  uint squareOrRemainderLow;
  int axisDelta;
  uint radiusOrSquareLow;
  GraphicsShadingRecordCount remainingRecords;
  uint remainderLowOrDivisor;
  GraphicsShadingRuntimeRecord *shadingRecord1;
  undefined1 colorByte3Or1;
  undefined1 colorByte2;
  undefined8 mm0PackedValue0;
  undefined8 mm0PackedValue1;
  undefined8 resultLanes;
  undefined1 sceneByte3Or1;
  undefined1 sceneByte2;
  undefined1 materialByte3Or1;
  undefined1 materialByte2;
  undefined8 mm4PackedValue0;
  
  lightFacingDotQ12 = FixedVec3_DotQ12(lightDirectionQ12,surfaceNormalQ12);
  mm0PackedValue0ByteLane3 = (byte)(scenePackedColor1 >> 0x18);
  colorByte2 = (undefined1)(scenePackedColor1 >> 0x10);
  colorByte3Or1 = (undefined1)(scenePackedColor1 >> 8);
  sceneByte3Or1 = (undefined1)(scenePackedColor0 >> 0x18);
  colorLane3Word = CONCAT11(sceneByte3Or1,sceneByte3Or1);
  sceneByte2 = (undefined1)(scenePackedColor0 >> 0x10);
  sceneByte3Or1 = (undefined1)(scenePackedColor0 >> 8);
  materialByte3Or1 = (undefined1)(materialPackedColor >> 0x18);
  materialLane3Word = CONCAT11(materialByte3Or1,materialByte3Or1);
  materialByte2 = (undefined1)(materialPackedColor >> 0x10);
  materialByte3Or1 = (undefined1)(materialPackedColor >> 8);
  mm0PackedValue0 =
       pmulhw(CONCAT26(CONCAT11(mm0PackedValue0ByteLane3,mm0PackedValue0ByteLane3) >> 2,
                       CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm0PackedValue0ByteLane3,
                                                                    mm0PackedValue0ByteLane3),colorByte2
                                                          ),CONCAT14(colorByte2,scenePackedColor1)) >>
                                        0x20) >> 2,
                                CONCAT22(CONCAT11(colorByte3Or1,colorByte3Or1) >> 2,
                                         CONCAT11((char)scenePackedColor1,(char)scenePackedColor1)
                                         >> 2))),
              *(undefined8 *)
               (&g_ModelLightingScaleMmxMultiplierTable + (lightFacingDotQ12 / lightingScaleQ12 >> 9) * 8));
  shadingRecord1 = g_GraphicsShadingNearbyRecords;
  mm0PackedValue1 =
       pmulhw(CONCAT26((short)((ulonglong)mm0PackedValue0 >> 0x30) + (colorLane3Word >> 4),
                       CONCAT24((short)((ulonglong)mm0PackedValue0 >> 0x20) +
                                ((ushort)(CONCAT35(CONCAT21(colorLane3Word,sceneByte2),
                                                   CONCAT14(sceneByte2,scenePackedColor0)) >> 0x20) >> 4
                                ),CONCAT22((short)((ulonglong)mm0PackedValue0 >> 0x10) +
                                           (CONCAT11(sceneByte3Or1,sceneByte3Or1) >> 4),
                                           (short)mm0PackedValue0 +
                                           (CONCAT11((char)scenePackedColor0,(char)scenePackedColor0
                                                    ) >> 4)))),
              CONCAT26(materialLane3Word >> 2,
                       CONCAT24((ushort)(CONCAT35(CONCAT21(materialLane3Word,materialByte2),
                                                  CONCAT14(materialByte2,materialPackedColor)) >> 0x20) >>
                                2,CONCAT22(CONCAT11(materialByte3Or1,materialByte3Or1) >> 2,
                                           CONCAT11((char)materialPackedColor,
                                                    (char)materialPackedColor) >> 2))));
  for (remainingRecords = g_GraphicsShadingNearbyRecordCount; remainingRecords != 0; remainingRecords = remainingRecords - 1) {
    if (shadingRecord1->targetRadiusQ12 != 0) {
      radiusOrSquareLow = (uint)shadingRecord1->squaredRadiusQ24;
      remainderHigh = *vertexPositionQ12 - shadingRecord1->worldXQ12;
      axisDistanceSquared = (longlong)remainderHigh * (longlong)remainderHigh;
      squareOrRemainderLow = (uint)axisDistanceSquared;
      remainderLowOrDivisor = radiusOrSquareLow - squareOrRemainderLow;
      remainderHigh = (*(int *)((int)&shadingRecord1->squaredRadiusQ24 + 4) -
               (int)((ulonglong)axisDistanceSquared >> 0x20)) - (uint)(radiusOrSquareLow < squareOrRemainderLow);
      if (-1 < remainderHigh) {
        axisDelta = vertexPositionQ12[1] - shadingRecord1->worldYQ12;
        axisDistanceSquared = (longlong)axisDelta * (longlong)axisDelta;
        radiusOrSquareLow = (uint)axisDistanceSquared;
        squareOrRemainderLow = remainderLowOrDivisor - radiusOrSquareLow;
        remainderHigh = (remainderHigh - (int)((ulonglong)axisDistanceSquared >> 0x20)) - (uint)(remainderLowOrDivisor < radiusOrSquareLow);
        if (-1 < remainderHigh) {
          axisDelta = vertexPositionQ12[2] - shadingRecord1->worldZQ12;
          axisDistanceSquared = (longlong)axisDelta * (longlong)axisDelta;
          radiusOrSquareLow = (uint)axisDistanceSquared;
          remainderHigh = (remainderHigh - (int)((ulonglong)axisDistanceSquared >> 0x20)) - (uint)(squareOrRemainderLow < radiusOrSquareLow);
          if (-1 < remainderHigh) {
            lightPackedColor = shadingRecord1->packedColorRgbActive;
            remainderLowOrDivisor = *(int *)((int)&shadingRecord1->squaredRadiusQ24 + 4) << 0x14 |
                     (uint)shadingRecord1->squaredRadiusQ24 >> 0xc;
            if (remainderLowOrDivisor != 0) {
              colorByte3Or1 = (undefined1)(lightPackedColor >> 0x18);
              colorLane3Word = CONCAT11(colorByte3Or1,colorByte3Or1);
              colorByte2 = (undefined1)(lightPackedColor >> 0x10);
              colorByte3Or1 = (undefined1)(lightPackedColor >> 8);
              mm4PackedValue0 =
                   pmulhw(CONCAT26(colorLane3Word >> 2,
                                   CONCAT24((ushort)(CONCAT35(CONCAT21(colorLane3Word,colorByte2),
                                                              CONCAT14(colorByte2,lightPackedColor)) >> 0x20) >> 2,
                                            CONCAT22(CONCAT11(colorByte3Or1,colorByte3Or1) >> 2,
                                                     CONCAT11((char)lightPackedColor,(char)lightPackedColor) >> 2))),
                          g_PackedLightingLookupTable[(remainderHigh * 0x8000000 | squareOrRemainderLow - radiusOrSquareLow >> 5) / remainderLowOrDivisor]);
              mm0PackedValue1 = paddusb(mm0PackedValue1,mm4PackedValue0);
            }
          }
        }
      }
    }
    shadingRecord1 = shadingRecord1 + 1;
  }
  colorByte3Or1 = (undefined1)(vertexPackedColor >> 0x18);
  colorLane3Word = CONCAT11(colorByte3Or1,colorByte3Or1);
  colorByte2 = (undefined1)(vertexPackedColor >> 0x10);
  colorByte3Or1 = (undefined1)(vertexPackedColor >> 8);
  resultLanes = pmulhw(mm0PackedValue1,
                  CONCAT26(colorLane3Word >> 2,
                           CONCAT24((ushort)(CONCAT35(CONCAT21(colorLane3Word,colorByte2),
                                                      CONCAT14(colorByte2,vertexPackedColor)) >> 0x20)
                                    >> 2,CONCAT22(CONCAT11(colorByte3Or1,colorByte3Or1) >> 2,
                                                  CONCAT11((char)vertexPackedColor,
                                                           (char)vertexPackedColor) >> 2))));
  resultLane0 = (short)resultLanes;
  resultLane1 = (short)((ulonglong)resultLanes >> 0x10);
  resultLane2 = (short)((ulonglong)resultLanes >> 0x20);
  resultLane3 = (short)((ulonglong)resultLanes >> 0x30);
  return CONCAT13((0 < resultLane3) * (resultLane3 < 0x100) * (char)((ulonglong)resultLanes >> 0x30) - (0xff < resultLane3)
                  ,CONCAT12((0 < resultLane2) * (resultLane2 < 0x100) * (char)((ulonglong)resultLanes >> 0x20) -
                            (0xff < resultLane2),
                            CONCAT11((0 < resultLane1) * (resultLane1 < 0x100) *
                                     (char)((ulonglong)resultLanes >> 0x10) - (0xff < resultLane1),
                                     (0 < resultLane0) * (resultLane0 < 0x100) * (char)resultLanes - (0xff < resultLane0)))
                 );
}

/* Address: 0x004CC940.
   Ownership: graphics/render/model.
   Purpose: Handles model render compute nearby light packed vertex color alternate path.
   Cross-module calls: FixedTransform_ApplyDirection [core/math/fixed], FixedVec3_NormalizeQ28 [core/math/fixed],
   FixedVec3_DotQ12 [core/math/fixed].
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
  for (remaining = g_GraphicsShadingNearbyRecordCount; remaining != 0; remaining = remaining - 1,
       record = record + 1) {
    longlong distanceSquared;
    longlong radiusSquared;
    uint divisor;
    uint numerator;
    sdword facing;
    int quotient;
    uint tableIndex;

    if (record->targetRadiusQ12 == 0) {
      continue;
    }
    g_ModelLightingVertexToLightVectorScratch.x = record->worldXQ12 - vertexPositionQ12->x;
    g_ModelLightingVertexToLightVectorScratch.y = record->worldYQ12 - vertexPositionQ12->y;
    g_ModelLightingVertexToLightVectorScratch.z = record->worldZQ12 - vertexPositionQ12->z;
    distanceSquared =
         (longlong)g_ModelLightingVertexToLightVectorScratch.x *
         g_ModelLightingVertexToLightVectorScratch.x +
         (longlong)g_ModelLightingVertexToLightVectorScratch.y *
         g_ModelLightingVertexToLightVectorScratch.y +
         (longlong)g_ModelLightingVertexToLightVectorScratch.z *
         g_ModelLightingVertexToLightVectorScratch.z;
    radiusSquared = (longlong)record->squaredRadiusQ24;
    if (distanceSquared >= radiusSquared) {
      continue;
    }
    divisor = (uint)((ulonglong)(distanceSquared * 8 + radiusSquared) >> 20);
    if (divisor == 0) {
      continue;
    }
    numerator = (uint)((ulonglong)radiusSquared >> 15) * 9;
    FixedVec3_NormalizeQ28
              (&g_ModelLightingVertexToLightVectorScratch,&g_ModelLightingVertexToLightVectorScratch);
    facing = FixedVec3_DotQ12
                       (&g_ModelLightingVertexToLightVectorScratch,
                        &g_ModelLightingTransformedSurfaceNormalScratch);
    if (facing < 0) {
      facing = 0;
    }
    quotient = (int)((longlong)numerator / (int)divisor);
    tableIndex = (uint)(((longlong)quotient * facing) >> 28);
    ModelLighting_UnpackBytes(record->packedColorRgbActive,2,lanes);
    ModelLighting_MulHigh(lanes,lightingTable + tableIndex * 4);
    ModelLighting_AddSaturate(color,lanes);
  }
  ModelLighting_UnpackBytes(vertexPackedColor,2,lanes);
  ModelLighting_MulHigh(color,lanes);
  return ModelLighting_PackUnsigned(color);
}


/* Address: 0x0050A430.
   Ownership: graphics/render/model.
   Purpose: Handles model projected bounds expand with current scratch point.
   Cross-module calls: FixedTransform_ApplyPoint [core/math/fixed], Graphics_ProjectViewPoint
   [graphics/core/runtime].
*/
void __thandor_preserve_eax_edx
ModelProjectedBounds_ExpandWithCurrentScratchPoint(ModelProjectedBoundsPixels *bounds)

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
  return;
}


/* Address: 0x004BD800.
   Ownership: graphics/render/model.
   Purpose: Composes the model and view transforms, derives the camera-facing direction, and transforms the primary
   and auxiliary directions into model-local space.
   Cross-module calls: FixedTransform_Compose [core/math/fixed], FixedMath_VectorToAngles3Regs [core/math/fixed],
   FixedMath_WriteDirectionQ28 [core/math/fixed], FixedTransform_ApplyTransposeDirection [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
ModelRender_PrepareViewDirections(ModelRuntimeNode *modelNodeRuntime)

{
  int nodeWorldX;
  int nodeWorldY;
  int nodeWorldZ;
  GraphicsFixedMatrix3x4 *transformA;
  FixedMathVectorAnglesRegs8 viewAngles;
  
  nodeWorldX = (modelNodeRuntime->worldTransform).translation.x;
  nodeWorldY = (modelNodeRuntime->worldTransform).translation.y;
  nodeWorldZ = (modelNodeRuntime->worldTransform).translation.z;
  transformA = &modelNodeRuntime->worldTransform;
  FixedTransform_Compose
            ((GraphicsFixedMatrix3x4 *)&g_ModelViewCompositeTransform,transformA,
             &g_ViewProjectionMatrixFixed);
  viewAngles = FixedMath_VectorToAngles3Regs
                    (nodeWorldZ - g_ViewOriginFixed.z,nodeWorldY - g_ViewOriginFixed.y,
                     nodeWorldX - g_ViewOriginFixed.x);
  FixedMath_WriteDirectionQ28((GraphicsFixedVec3 *)&g_ModelViewDirectionWorld,viewAngles.edx,viewAngles.ecx);
  FixedTransform_ApplyTransposeDirection
            ((GraphicsFixedVec3 *)&g_ModelViewDirectionLocal,transformA,
             (GraphicsFixedVec3 *)&g_ModelViewDirectionWorld);
  FixedTransform_ApplyTransposeDirection
            ((GraphicsFixedVec3 *)&g_ModelAuxiliaryForwardDirectionLocal,transformA,
             &g_AuxiliaryForwardDirectionFixed);
  return;
}

