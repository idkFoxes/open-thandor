/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/render/model.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/render/model.h>

/* Implementation ownership: graphics/render/model. */

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
  uint uVar1;
  AngleTurn32 AVar2;
  AngleTurn32 AVar3;
  AngleTurn32 AVar4;
  sdword sVar5;
  sdword sVar6;
  sdword sVar7;
  GraphicsWorldCoordinateQ12 GVar8;
  sdword sVar9;
  sdword sVar10;
  sdword sVar11;
  GraphicsWorldCoordinateQ12 GVar12;
  sdword sVar13;
  sdword sVar14;
  sdword sVar15;
  GraphicsWorldCoordinateQ12 GVar16;
  int iVar17;
  int *meshGroup_00;
  
  uVar1 = *(uint *)(meshGroup + 0xc);
  iVar17 = *(int *)(meshGroup + 4);
  AVar2 = (modelNode->modelPayload).worldRotationAngle0;
  AVar3 = (modelNode->modelPayload).worldRotationAngle1;
  AVar4 = (modelNode->modelPayload).worldRotationAngle2;
  sVar5 = (modelNode->worldTransform).basisRow0[0];
  sVar6 = (modelNode->worldTransform).basisRow0[1];
  sVar7 = (modelNode->worldTransform).basisRow0[2];
  GVar8 = (modelNode->worldTransform).translation.x;
  sVar9 = (modelNode->worldTransform).basisRow1[0];
  sVar10 = (modelNode->worldTransform).basisRow1[1];
  sVar11 = (modelNode->worldTransform).basisRow1[2];
  GVar12 = (modelNode->worldTransform).translation.y;
  sVar13 = (modelNode->worldTransform).basisRow2[0];
  sVar14 = (modelNode->worldTransform).basisRow2[1];
  sVar15 = (modelNode->worldTransform).basisRow2[2];
  GVar16 = (modelNode->worldTransform).translation.z;
  if ((uVar1 & 1) != 0) {
    ModelNodeRuntime_BuildViewFacingRotation(modelNode);
  }
  if ((uVar1 & 2) != 0) {
    ModelNodeRuntime_BuildBillboardRotation(modelNode);
  }
  meshGroup_00 = (int *)(meshGroup + 0x20);
  uVar1 = (modelNode->modelPayload).meshGroupMask;
  for (; iVar17 != 0; iVar17 = iVar17 + -1) {
    if ((meshGroup_00[1] & uVar1) != 0) {
      ModelRender_SubmitMeshTriangles
                (facingThresholdQ12,(ModelMeshGroupAddress32)meshGroup_00,modelNode);
    }
    meshGroup_00 = (int *)((int)meshGroup_00 + *meshGroup_00);
  }
  (modelNode->worldTransform).translation.z = GVar16;
  (modelNode->worldTransform).basisRow2[2] = sVar15;
  (modelNode->worldTransform).basisRow2[1] = sVar14;
  (modelNode->worldTransform).basisRow2[0] = sVar13;
  (modelNode->worldTransform).translation.y = GVar12;
  (modelNode->worldTransform).basisRow1[2] = sVar11;
  (modelNode->worldTransform).basisRow1[1] = sVar10;
  (modelNode->worldTransform).basisRow1[0] = sVar9;
  (modelNode->worldTransform).translation.x = GVar8;
  (modelNode->worldTransform).basisRow0[2] = sVar7;
  (modelNode->worldTransform).basisRow0[1] = sVar6;
  (modelNode->worldTransform).basisRow0[0] = sVar5;
  (modelNode->modelPayload).worldRotationAngle2 = AVar4;
  (modelNode->modelPayload).worldRotationAngle1 = AVar3;
  (modelNode->modelPayload).worldRotationAngle0 = AVar2;
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
  uint uVar1;
  int iVar2;
  int *meshGroup_00;
  
  uVar1 = *(uint *)(meshGroup + 0xc);
  iVar2 = *(int *)(meshGroup + 4);
  if ((uVar1 & 1) != 0) {
    ModelNodeRuntime_BuildViewFacingRotation(modelNode);
  }
  if ((uVar1 & 2) != 0) {
    ModelNodeRuntime_BuildBillboardRotation(modelNode);
  }
  meshGroup_00 = (int *)(meshGroup + 0x20);
  uVar1 = (modelNode->modelPayload).meshGroupMask;
  for (; iVar2 != 0; iVar2 = iVar2 + -1) {
    if ((meshGroup_00[1] & uVar1) != 0) {
      ModelRender_SubmitMeshTrianglesAlternatePath((ModelMeshGroupAddress32)meshGroup_00,modelNode);
    }
    meshGroup_00 = (int *)((int)meshGroup_00 + *meshGroup_00);
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
  dword dVar1;
  
  ModelProjectedBounds_AccumulateNode(bounds,modelNode);
  for (dVar1 = modelNode->childCount; dVar1 != 0; dVar1 = dVar1 - 1) {
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
  uint uVar1;
  longlong lVar2;
  PackedArgb32 PVar3;
  uint uVar4;
  GraphicsFixedVec3 *surfaceNormalQ12;
  GraphicsProjectedPointEdxEax8 projectedScreenCoordinatePair;
  GraphicsWorldCoordinateQ12 GVar5;
  longlong scaledVertexCoordinateProduct;
  GraphicsWorldCoordinateQ12 savedVertexZQ12;
  GraphicsWorldCoordinateQ12 savedVertexYQ12;
  
  uVar1 = *(uint *)(meshGroup + 0x34);
  if (vertex[4].x == -0x80000000) {
    GVar5 = vertex->x;
    savedVertexYQ12 = vertex->y;
    savedVertexZQ12 = vertex->z;
    uVar4 = (uint)modelNode->renderDepthBiasOrState >> 1;
    if (-1 < vertex->z) {
      if (vertex->z != 0) {
        vertex->z = vertex->z + uVar4;
      }
      vertex->z = vertex->z + uVar4;
    }
    if ((modelNode->runtimeFlags & 0x800) != 0) {
      scaledVertexCoordinateProduct = (longlong)vertex->x * (longlong)modelNode->modelScaleQ12;
      vertex->x = (int)((ulonglong)scaledVertexCoordinateProduct >> 0x20) << 0x14 |
                  (uint)scaledVertexCoordinateProduct >> 0xc;
      lVar2 = (longlong)vertex->y * (longlong)modelNode->modelScaleQ12;
      vertex->y = (int)((ulonglong)lVar2 >> 0x20) << 0x14 | (uint)lVar2 >> 0xc;
      lVar2 = (longlong)vertex->z * (longlong)modelNode->modelScaleQ12;
      vertex->z = (int)((ulonglong)lVar2 >> 0x20) << 0x14 | (uint)lVar2 >> 0xc;
    }
    FixedTransform_ApplyPoint
              ((GraphicsFixedVec3 *)&vertex[2].z,vertex,
               (GraphicsFixedMatrix3x4 *)&g_ModelViewCompositeTransform);
    projectedScreenCoordinatePair =
         (GraphicsProjectedPointEdxEax8)Graphics_ProjectViewPoint((GraphicsFixedVec3 *)&vertex[2].z)
    ;
    vertex->z = savedVertexZQ12;
    vertex->y = savedVertexYQ12;
    vertex->x = GVar5;
    vertex[4].x = (int)projectedScreenCoordinatePair;
    vertex[4].y = (int)(projectedScreenCoordinatePair >> 0x20);
  }
  else if (((uVar1 & 0x8e00) == vertex[4].z) && ((uVar1 & 0x8000) == 0)) {
    return;
  }
  vertex[4].z = uVar1 & 0x8e00;
  PVar3 = modelNode->tintArgb;
  surfaceNormalQ12 = (GraphicsFixedVec3 *)&vertex[1].y;
  if ((uVar1 & 0x200) != 0) {
    vertex[3].z = PVar3;
    return;
  }
  if ((uVar1 & 0x800) == 0) {
    if ((uVar1 & 0x8000) != 0) {
      surfaceNormalQ12 = (GraphicsFixedVec3 *)(meshGroup + 0x24);
    }
    PVar3 = ModelRender_ComputeVertexIntensityDefaultPath
                      (vertex[2].y,&vertex[2].z,0x4cb1a0,g_SceneBoundsFixed.bound5,
                       g_SceneBoundsFixed.bound4,
                       (GraphicsFixedVec3 *)&g_ModelAuxiliaryForwardDirectionLocal,PVar3,
                       surfaceNormalQ12);
    vertex[3].z = PVar3;
    return;
  }
  PVar3 = ModelRender_ComputeVertexIntensityScaledPath
                    (vertex[2].y,&vertex[2].z,
                     ((modelNode->modelPayload).modelResource)->lightingScaleQ12,
                     g_SceneBoundsFixed.bound7,g_SceneBoundsFixed.bound6,
                     (GraphicsFixedVec3 *)&g_ModelAuxiliaryForwardDirectionLocal,PVar3,vertex);
  vertex[3].z = PVar3;
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
  GraphicsProjectedVertexSource *vertex;
  GraphicsProjectedVertexSource *vertex_00;
  GraphicsProjectedVertexSource *vertex_01;
  GraphicsTextureSet *pGVar1;
  GraphicsPaletteAsset *pGVar2;
  GraphicsSubresourceIndex GVar3;
  sdword sVar4;
  uint uVar5;
  bool bVar6;
  GraphicsTextureSetEntry *textureEntry;
  
  sVar4 = ModelRender_ComputeFacingDotQ12(triangle);
  if (((triangle->renderFlags & 0x400) != 0) || (sVar4 < facingThresholdQ12)) {
    vertex = triangle->vertex0;
    vertex_00 = triangle->vertex1;
    vertex_01 = triangle->vertex2;
    ModelRender_PrepareProjectedVertex
              (modelNode,(ModelMeshGroupAddress32)triangle,(GraphicsFixedVec3 *)vertex);
    ModelRender_PrepareProjectedVertex
              (modelNode,(ModelMeshGroupAddress32)triangle,(GraphicsFixedVec3 *)vertex_00);
    ModelRender_PrepareProjectedVertex
              (modelNode,(ModelMeshGroupAddress32)triangle,(GraphicsFixedVec3 *)vertex_01);
    if (((g_ProjectionClipRect.minX <= vertex->screenX) ||
        ((g_ProjectionClipRect.minX <= vertex_00->screenX ||
         (g_ProjectionClipRect.minX <= vertex_01->screenX)))) &&
       ((vertex->screenX < g_ProjectionClipRect.maxX ||
        ((vertex_00->screenX < g_ProjectionClipRect.maxX ||
         (vertex_01->screenX < g_ProjectionClipRect.maxX)))))) {
      if (((g_ProjectionClipRect.minY <= vertex->screenY) ||
          ((g_ProjectionClipRect.minY <= vertex_00->screenY ||
           (g_ProjectionClipRect.minY <= vertex_01->screenY)))) &&
         ((vertex->screenY < g_ProjectionClipRect.maxY ||
          ((vertex_00->screenY < g_ProjectionClipRect.maxY ||
           (vertex_01->screenY < g_ProjectionClipRect.maxY)))))) {
        bVar6 = GraphicsPrimitiveQueue_AppendTriangle
                          (triangle->renderFlags,triangle,triangle->vertex2,triangle->vertex1,
                           triangle->vertex0,g_ActivePrimitiveQueue);
        if (!bVar6) {
          GraphicsPrimitiveQueue_SetVertexColors
                    (triangle->vertex2->vertexColorArgb,triangle->vertex1->vertexColorArgb,
                     triangle->vertex0->vertexColorArgb,g_ActivePrimitiveQueue);
          pGVar1 = (modelNode->modelPayload).textureSet;
          textureEntry = (GraphicsTextureSetEntry *)0x0;
          if ((pGVar1 != (GraphicsTextureSet *)0x0) &&
             (triangle->subresourceIndex < pGVar1->subresourceCount)) {
            textureEntry = pGVar1->entries +
                           triangle->subresourceIndex + modelNode->textureSubresourceBaseIndex;
          }
          pGVar2 = (modelNode->modelPayload).paletteAsset;
          if ((pGVar2 == (GraphicsPaletteAsset *)0x0) ||
             (uVar5 = triangle->renderFlags & 0x1ff, pGVar2->paletteBankCount <= uVar5)) {
            GraphicsPrimitiveQueue_SetMaterial(0xffffffff,textureEntry,g_ActivePrimitiveQueue);
            GVar3 = triangle->subresourceIndex;
            if ((modelNode->runtimeFlags & 0x80) != 0) {
              if (GVar3 == modelNode->primaryAnimatedSubresourceIndex) {
                GraphicsPrimitiveQueue_OffsetTextureCoordinates
                          (modelNode->primaryTextureOffsetV,modelNode->primaryTextureOffsetU,
                           g_ActivePrimitiveQueue);
              }
              if (((modelNode->runtimeFlags & 0x400) != 0) &&
                 (GVar3 == modelNode->secondaryAnimatedSubresourceIndex)) {
                GraphicsPrimitiveQueue_OffsetTextureCoordinates
                          (modelNode->secondaryTextureOffsetV,modelNode->secondaryTextureOffsetU,
                           g_ActivePrimitiveQueue);
              }
            }
          }
          else {
            GraphicsPrimitiveQueue_SetMaterial
                      (pGVar2->paletteEntries[uVar5].argb8888,textureEntry,g_ActivePrimitiveQueue);
            GVar3 = triangle->subresourceIndex;
            if ((modelNode->runtimeFlags & 0x80) != 0) {
              if (GVar3 == modelNode->primaryAnimatedSubresourceIndex) {
                GraphicsPrimitiveQueue_OffsetTextureCoordinates
                          (modelNode->primaryTextureOffsetV,modelNode->primaryTextureOffsetU,
                           g_ActivePrimitiveQueue);
              }
              if (((modelNode->runtimeFlags & 0x400) != 0) &&
                 (GVar3 == modelNode->secondaryAnimatedSubresourceIndex)) {
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
  int iVar1;
  GraphicsTriangleInput *triangle;
  
  iVar1 = *(int *)(meshGroup + 8);
  ModelRender_PrepareViewDirections(modelNode);
  triangle = (GraphicsTriangleInput *)(meshGroup + 0x20);
  for (; iVar1 != 0; iVar1 = iVar1 + -1) {
    triangle->subresourceIndex = 0x80000000;
    triangle = (GraphicsTriangleInput *)&triangle[1].textureV0;
  }
  for (iVar1 = *(int *)(meshGroup + 0xc); iVar1 != 0; iVar1 = iVar1 + -1) {
    ModelRender_SubmitTriangle(facingThresholdQ12,triangle,modelNode);
    triangle = (GraphicsTriangleInput *)&triangle[1].textureV0;
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
  int iVar1;
  GraphicsTriangleInput *triangle;
  
  iVar1 = *(int *)(meshGroup + 8);
  ModelRender_PrepareViewDirections(modelNode);
  triangle = (GraphicsTriangleInput *)(meshGroup + 0x20);
  for (; iVar1 != 0; iVar1 = iVar1 + -1) {
    triangle->subresourceIndex = 0x80000000;
    triangle = (GraphicsTriangleInput *)&triangle[1].textureV0;
  }
  for (iVar1 = *(int *)(meshGroup + 0xc); iVar1 != 0; iVar1 = iVar1 + -1) {
    ModelRender_SubmitTriangleAlternatePath(triangle,modelNode);
    triangle = (GraphicsTriangleInput *)&triangle[1].textureV0;
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
  uint uVar1;
  uint materialPackedColor;
  PackedArgb32 PVar2;
  GraphicsFixedVec3 *surfaceNormalQ12;
  GraphicsProjectedPointPair GVar3;
  
  uVar1 = triangle->renderFlags;
  if (vertex[4].x == -0x80000000) {
    FixedTransform_ApplyPoint
              ((GraphicsFixedVec3 *)&vertex[2].z,vertex,
               (GraphicsFixedMatrix3x4 *)&g_ModelViewCompositeTransform);
    if (vertex[3].y < (int)g_ProjectionScaleFixed) {
LAB_004bd788:
      vertex[4].x = 0x7fffffff;
      return true;
    }
    GVar3 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)&vertex[2].z);
    vertex[4].x = GVar3.projectedX;
    vertex[4].y = GVar3.projectedY;
  }
  else {
    if (vertex[4].x == 0x7fffffff) goto LAB_004bd788;
    if (((uVar1 & 0x8e00) == vertex[4].z) && ((uVar1 & 0x8000) == 0)) {
      return false;
    }
  }
  materialPackedColor = modelNode->tintArgb;
  vertex[4].z = uVar1 & 0x8e00;
  if ((uVar1 & 0x200) == 0) {
    surfaceNormalQ12 = (GraphicsFixedVec3 *)&vertex[1].y;
    if ((uVar1 & 0x8000) != 0) {
      surfaceNormalQ12 = (GraphicsFixedVec3 *)&triangle->planeNormalXQ12;
    }
    PVar2 = ModelRender_ComputeNearbyLightPackedVertexColorAlternatePath
                      (vertex[2].y,(GraphicsFixedVec3 *)&vertex[2].z,g_SceneBoundsFixed.bound5,
                       materialPackedColor,surfaceNormalQ12);
    vertex[3].z = PVar2;
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
  GraphicsProjectedVertexSource *vertex;
  GraphicsProjectedVertexSource *vertex_00;
  GraphicsProjectedVertexSource *vertex_01;
  GraphicsTextureSet *pGVar1;
  GraphicsPaletteAsset *pGVar2;
  uint uVar3;
  bool bVar4;
  GraphicsTextureSetEntry *textureEntry;
  
  vertex = triangle->vertex0;
  vertex_00 = triangle->vertex1;
  vertex_01 = triangle->vertex2;
  bVar4 = ModelRender_PrepareProjectedVertexAlternatePath
                    (modelNode,triangle,(GraphicsFixedVec3 *)vertex);
  if (!bVar4) {
    bVar4 = ModelRender_PrepareProjectedVertexAlternatePath
                      (modelNode,triangle,(GraphicsFixedVec3 *)vertex_00);
    if (!bVar4) {
      bVar4 = ModelRender_PrepareProjectedVertexAlternatePath
                        (modelNode,triangle,(GraphicsFixedVec3 *)vertex_01);
      if (!bVar4) {
        if ((((g_ProjectionClipRect.minX <= vertex->screenX) ||
             (g_ProjectionClipRect.minX <= vertex_00->screenX)) ||
            (g_ProjectionClipRect.minX <= vertex_01->screenX)) &&
           (((vertex->screenX < g_ProjectionClipRect.maxX ||
             (vertex_00->screenX < g_ProjectionClipRect.maxX)) ||
            (vertex_01->screenX < g_ProjectionClipRect.maxX)))) {
          if ((((g_ProjectionClipRect.minY <= vertex->screenY) ||
               (g_ProjectionClipRect.minY <= vertex_00->screenY)) ||
              (g_ProjectionClipRect.minY <= vertex_01->screenY)) &&
             (((vertex->screenY < g_ProjectionClipRect.maxY ||
               (vertex_00->screenY < g_ProjectionClipRect.maxY)) ||
              (vertex_01->screenY < g_ProjectionClipRect.maxY)))) {
            bVar4 = GraphicsPrimitiveQueue_AppendTriangle
                              (triangle->renderFlags,triangle,triangle->vertex2,triangle->vertex1,
                               triangle->vertex0,g_ActivePrimitiveQueue);
            if (!bVar4) {
              GraphicsPrimitiveQueue_SetVertexColors
                        (triangle->vertex2->vertexColorArgb,triangle->vertex1->vertexColorArgb,
                         triangle->vertex0->vertexColorArgb,g_ActivePrimitiveQueue);
              pGVar1 = (modelNode->modelPayload).textureSet;
              uVar3 = triangle->subresourceIndex;
              textureEntry = (GraphicsTextureSetEntry *)0x0;
              if ((uVar3 != 0xffffffff) && (uVar3 < pGVar1->subresourceCount)) {
                textureEntry = pGVar1->entries + uVar3 + modelNode->textureSubresourceBaseIndex;
              }
              pGVar2 = (modelNode->modelPayload).paletteAsset;
              if ((pGVar2 != (GraphicsPaletteAsset *)0x0) &&
                 (uVar3 = triangle->renderFlags & 0xffff01ff, uVar3 < pGVar2->paletteBankCount)) {
                GraphicsPrimitiveQueue_SetMaterial
                          (pGVar2->paletteEntries[uVar3].alternateModulationColorArgb,textureEntry,
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
  ModelResourceHitTestAndRenderView210 *pMVar1;
  GraphicsWorldCoordinateQ12 GVar2;
  GraphicsWorldCoordinateQ12 GVar3;
  
  pMVar1 = (modelNode->modelPayload).modelResource;
  if ((pMVar1->boundingRadiusQ12 != 0) &&
     ((pMVar1->hitTestFlags20C & MODEL_RESOURCE_DISABLE_PROJECTED_HIT_TEST) == 0)) {
    FixedTransform_Compose
              (&g_GraphicsTransformScratchMatrix3x4,&modelNode->worldTransform,
               &g_ViewProjectionMatrixFixed);
    GVar2 = pMVar1->localBoundsX0Q12;
    GVar3 = pMVar1->localBoundsX1Q12;
    g_GraphicsTransformInputScratchVec3.y = pMVar1->localBoundsY0Q12;
    g_GraphicsTransformInputScratchVec3.z = pMVar1->localBoundsZ0Q12;
    g_GraphicsTransformInputScratchVec3.x = GVar2;
    ModelProjectedBounds_ExpandWithCurrentScratchPoint(bounds);
    g_GraphicsTransformInputScratchVec3.x = GVar3;
    ModelProjectedBounds_ExpandWithCurrentScratchPoint(bounds);
    g_GraphicsTransformInputScratchVec3.y = pMVar1->localBoundsY1Q12;
    g_GraphicsTransformInputScratchVec3.x = GVar2;
    ModelProjectedBounds_ExpandWithCurrentScratchPoint(bounds);
    g_GraphicsTransformInputScratchVec3.x = GVar3;
    ModelProjectedBounds_ExpandWithCurrentScratchPoint(bounds);
    g_GraphicsTransformInputScratchVec3.y = pMVar1->localBoundsY0Q12;
    g_GraphicsTransformInputScratchVec3.z = pMVar1->localBoundsZ1Q12;
    g_GraphicsTransformInputScratchVec3.x = GVar2;
    ModelProjectedBounds_ExpandWithCurrentScratchPoint(bounds);
    g_GraphicsTransformInputScratchVec3.x = GVar3;
    ModelProjectedBounds_ExpandWithCurrentScratchPoint(bounds);
    g_GraphicsTransformInputScratchVec3.y = pMVar1->localBoundsY1Q12;
    g_GraphicsTransformInputScratchVec3.x = GVar2;
    ModelProjectedBounds_ExpandWithCurrentScratchPoint(bounds);
    g_GraphicsTransformInputScratchVec3.x = GVar3;
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
ModelRender_ComputeFacingDotQ12(GraphicsTriangleInput *modelNodeRuntime)

{
  sdword facingDotQ12;
  
  facingDotQ12 = FixedVec3_DotQ12((GraphicsFixedVec3 *)&modelNodeRuntime->planeNormalXQ12,
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
  PackedRgb24 PVar1;
  longlong lVar2;
  short sVar3;
  short sVar4;
  short sVar5;
  short sVar6;
  ushort uVar7;
  ushort uVar8;
  byte mm0PackedValue0ByteLane3;
  sdword sVar9;
  int iVar10;
  uint uVar11;
  int iVar12;
  uint uVar13;
  GraphicsShadingRecordCount GVar14;
  uint uVar15;
  GraphicsShadingRuntimeRecord *shadingRecord1;
  undefined1 uVar17;
  undefined1 uVar18;
  undefined8 mm0PackedValue0;
  undefined8 uVar16;
  undefined1 uVar19;
  undefined1 uVar20;
  undefined1 uVar21;
  undefined1 uVar22;
  undefined8 mm4PackedValue0;
  
  sVar9 = FixedVec3_DotQ12(lightDirectionQ12,surfaceNormalQ12);
  mm0PackedValue0ByteLane3 = (byte)(scenePackedColor1 >> 0x18);
  uVar18 = (undefined1)(scenePackedColor1 >> 0x10);
  uVar17 = (undefined1)(scenePackedColor1 >> 8);
  uVar19 = (undefined1)(scenePackedColor0 >> 0x18);
  uVar7 = CONCAT11(uVar19,uVar19);
  uVar20 = (undefined1)(scenePackedColor0 >> 0x10);
  uVar19 = (undefined1)(scenePackedColor0 >> 8);
  uVar21 = (undefined1)(materialPackedColor >> 0x18);
  uVar8 = CONCAT11(uVar21,uVar21);
  uVar22 = (undefined1)(materialPackedColor >> 0x10);
  uVar21 = (undefined1)(materialPackedColor >> 8);
  mm0PackedValue0 =
       pmulhw(CONCAT26(CONCAT11(mm0PackedValue0ByteLane3,mm0PackedValue0ByteLane3) >> 2,
                       CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm0PackedValue0ByteLane3,
                                                                    mm0PackedValue0ByteLane3),uVar18
                                                          ),CONCAT14(uVar18,scenePackedColor1)) >>
                                        0x20) >> 2,
                                CONCAT22(CONCAT11(uVar17,uVar17) >> 2,
                                         CONCAT11((char)scenePackedColor1,(char)scenePackedColor1)
                                         >> 2))),
              *(undefined8 *)(distanceAttenuationTable + (sVar9 >> 0x15) * 8));
  shadingRecord1 = g_GraphicsShadingNearbyRecords;
  uVar16 = pmulhw(CONCAT26((short)((ulonglong)mm0PackedValue0 >> 0x30) + (uVar7 >> 4),
                           CONCAT24((short)((ulonglong)mm0PackedValue0 >> 0x20) +
                                    ((ushort)(CONCAT35(CONCAT21(uVar7,uVar20),
                                                       CONCAT14(uVar20,scenePackedColor0)) >> 0x20)
                                    >> 4),CONCAT22((short)((ulonglong)mm0PackedValue0 >> 0x10) +
                                                   (CONCAT11(uVar19,uVar19) >> 4),
                                                   (short)mm0PackedValue0 +
                                                   (CONCAT11((char)scenePackedColor0,
                                                             (char)scenePackedColor0) >> 4)))),
                  CONCAT26(uVar8 >> 2,
                           CONCAT24((ushort)(CONCAT35(CONCAT21(uVar8,uVar22),
                                                      CONCAT14(uVar22,materialPackedColor)) >> 0x20)
                                    >> 2,CONCAT22(CONCAT11(uVar21,uVar21) >> 2,
                                                  CONCAT11((char)materialPackedColor,
                                                           (char)materialPackedColor) >> 2))));
  for (GVar14 = g_GraphicsShadingNearbyRecordCount; GVar14 != 0; GVar14 = GVar14 - 1) {
    if (shadingRecord1->targetRadiusQ12 != 0) {
      uVar13 = (uint)shadingRecord1->squaredRadiusQ24;
      iVar10 = *vertexPositionQ12 - shadingRecord1->worldXQ12;
      lVar2 = (longlong)iVar10 * (longlong)iVar10;
      uVar11 = (uint)lVar2;
      uVar15 = uVar13 - uVar11;
      iVar10 = (*(int *)((int)&shadingRecord1->squaredRadiusQ24 + 4) -
               (int)((ulonglong)lVar2 >> 0x20)) - (uint)(uVar13 < uVar11);
      if (-1 < iVar10) {
        iVar12 = vertexPositionQ12[1] - shadingRecord1->worldYQ12;
        lVar2 = (longlong)iVar12 * (longlong)iVar12;
        uVar13 = (uint)lVar2;
        uVar11 = uVar15 - uVar13;
        iVar10 = (iVar10 - (int)((ulonglong)lVar2 >> 0x20)) - (uint)(uVar15 < uVar13);
        if (-1 < iVar10) {
          iVar12 = vertexPositionQ12[2] - shadingRecord1->worldZQ12;
          lVar2 = (longlong)iVar12 * (longlong)iVar12;
          uVar13 = (uint)lVar2;
          iVar10 = (iVar10 - (int)((ulonglong)lVar2 >> 0x20)) - (uint)(uVar11 < uVar13);
          if (-1 < iVar10) {
            PVar1 = shadingRecord1->packedColorRgbActive;
            uVar15 = *(int *)((int)&shadingRecord1->squaredRadiusQ24 + 4) << 0x14 |
                     (uint)shadingRecord1->squaredRadiusQ24 >> 0xc;
            if (uVar15 != 0) {
              uVar17 = (undefined1)(PVar1 >> 0x18);
              uVar7 = CONCAT11(uVar17,uVar17);
              uVar18 = (undefined1)(PVar1 >> 0x10);
              uVar17 = (undefined1)(PVar1 >> 8);
              mm4PackedValue0 =
                   pmulhw(CONCAT26(uVar7 >> 2,
                                   CONCAT24((ushort)(CONCAT35(CONCAT21(uVar7,uVar18),
                                                              CONCAT14(uVar18,PVar1)) >> 0x20) >> 2,
                                            CONCAT22(CONCAT11(uVar17,uVar17) >> 2,
                                                     CONCAT11((char)PVar1,(char)PVar1) >> 2))),
                          *(undefined8 *)
                           (&g_PackedLightingLookupTable +
                           ((iVar10 * 0x8000000 | uVar11 - uVar13 >> 5) / uVar15) * 8));
              uVar16 = paddusb(uVar16,mm4PackedValue0);
            }
          }
        }
      }
    }
    shadingRecord1 = shadingRecord1 + 1;
  }
  uVar17 = (undefined1)(vertexPackedColor >> 0x18);
  uVar7 = CONCAT11(uVar17,uVar17);
  uVar18 = (undefined1)(vertexPackedColor >> 0x10);
  uVar17 = (undefined1)(vertexPackedColor >> 8);
  uVar16 = pmulhw(uVar16,CONCAT26(uVar7 >> 2,
                                  CONCAT24((ushort)(CONCAT35(CONCAT21(uVar7,uVar18),
                                                             CONCAT14(uVar18,vertexPackedColor)) >>
                                                   0x20) >> 2,
                                           CONCAT22(CONCAT11(uVar17,uVar17) >> 2,
                                                    CONCAT11((char)vertexPackedColor,
                                                             (char)vertexPackedColor) >> 2))));
  sVar3 = (short)uVar16;
  sVar4 = (short)((ulonglong)uVar16 >> 0x10);
  sVar5 = (short)((ulonglong)uVar16 >> 0x20);
  sVar6 = (short)((ulonglong)uVar16 >> 0x30);
  return CONCAT13((0 < sVar6) * (sVar6 < 0x100) * (char)((ulonglong)uVar16 >> 0x30) - (0xff < sVar6)
                  ,CONCAT12((0 < sVar5) * (sVar5 < 0x100) * (char)((ulonglong)uVar16 >> 0x20) -
                            (0xff < sVar5),
                            CONCAT11((0 < sVar4) * (sVar4 < 0x100) *
                                     (char)((ulonglong)uVar16 >> 0x10) - (0xff < sVar4),
                                     (0 < sVar3) * (sVar3 < 0x100) * (char)uVar16 - (0xff < sVar3)))
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
  PackedRgb24 PVar1;
  longlong lVar2;
  short sVar3;
  short sVar4;
  short sVar5;
  short sVar6;
  ushort uVar7;
  ushort uVar8;
  byte mm0PackedValue0ByteLane3;
  sdword sVar9;
  int iVar10;
  uint uVar11;
  int iVar12;
  uint uVar13;
  GraphicsShadingRecordCount GVar14;
  uint uVar15;
  GraphicsShadingRuntimeRecord *shadingRecord1;
  undefined1 uVar17;
  undefined1 uVar18;
  undefined8 mm0PackedValue0;
  undefined8 mm0PackedValue1;
  undefined8 uVar16;
  undefined1 uVar19;
  undefined1 uVar20;
  undefined1 uVar21;
  undefined1 uVar22;
  undefined8 mm4PackedValue0;
  
  sVar9 = FixedVec3_DotQ12(lightDirectionQ12,surfaceNormalQ12);
  mm0PackedValue0ByteLane3 = (byte)(scenePackedColor1 >> 0x18);
  uVar18 = (undefined1)(scenePackedColor1 >> 0x10);
  uVar17 = (undefined1)(scenePackedColor1 >> 8);
  uVar19 = (undefined1)(scenePackedColor0 >> 0x18);
  uVar7 = CONCAT11(uVar19,uVar19);
  uVar20 = (undefined1)(scenePackedColor0 >> 0x10);
  uVar19 = (undefined1)(scenePackedColor0 >> 8);
  uVar21 = (undefined1)(materialPackedColor >> 0x18);
  uVar8 = CONCAT11(uVar21,uVar21);
  uVar22 = (undefined1)(materialPackedColor >> 0x10);
  uVar21 = (undefined1)(materialPackedColor >> 8);
  mm0PackedValue0 =
       pmulhw(CONCAT26(CONCAT11(mm0PackedValue0ByteLane3,mm0PackedValue0ByteLane3) >> 2,
                       CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm0PackedValue0ByteLane3,
                                                                    mm0PackedValue0ByteLane3),uVar18
                                                          ),CONCAT14(uVar18,scenePackedColor1)) >>
                                        0x20) >> 2,
                                CONCAT22(CONCAT11(uVar17,uVar17) >> 2,
                                         CONCAT11((char)scenePackedColor1,(char)scenePackedColor1)
                                         >> 2))),
              *(undefined8 *)
               (&g_ModelLightingScaleMmxMultiplierTable + (sVar9 / lightingScaleQ12 >> 9) * 8));
  shadingRecord1 = g_GraphicsShadingNearbyRecords;
  mm0PackedValue1 =
       pmulhw(CONCAT26((short)((ulonglong)mm0PackedValue0 >> 0x30) + (uVar7 >> 4),
                       CONCAT24((short)((ulonglong)mm0PackedValue0 >> 0x20) +
                                ((ushort)(CONCAT35(CONCAT21(uVar7,uVar20),
                                                   CONCAT14(uVar20,scenePackedColor0)) >> 0x20) >> 4
                                ),CONCAT22((short)((ulonglong)mm0PackedValue0 >> 0x10) +
                                           (CONCAT11(uVar19,uVar19) >> 4),
                                           (short)mm0PackedValue0 +
                                           (CONCAT11((char)scenePackedColor0,(char)scenePackedColor0
                                                    ) >> 4)))),
              CONCAT26(uVar8 >> 2,
                       CONCAT24((ushort)(CONCAT35(CONCAT21(uVar8,uVar22),
                                                  CONCAT14(uVar22,materialPackedColor)) >> 0x20) >>
                                2,CONCAT22(CONCAT11(uVar21,uVar21) >> 2,
                                           CONCAT11((char)materialPackedColor,
                                                    (char)materialPackedColor) >> 2))));
  for (GVar14 = g_GraphicsShadingNearbyRecordCount; GVar14 != 0; GVar14 = GVar14 - 1) {
    if (shadingRecord1->targetRadiusQ12 != 0) {
      uVar13 = (uint)shadingRecord1->squaredRadiusQ24;
      iVar10 = *vertexPositionQ12 - shadingRecord1->worldXQ12;
      lVar2 = (longlong)iVar10 * (longlong)iVar10;
      uVar11 = (uint)lVar2;
      uVar15 = uVar13 - uVar11;
      iVar10 = (*(int *)((int)&shadingRecord1->squaredRadiusQ24 + 4) -
               (int)((ulonglong)lVar2 >> 0x20)) - (uint)(uVar13 < uVar11);
      if (-1 < iVar10) {
        iVar12 = vertexPositionQ12[1] - shadingRecord1->worldYQ12;
        lVar2 = (longlong)iVar12 * (longlong)iVar12;
        uVar13 = (uint)lVar2;
        uVar11 = uVar15 - uVar13;
        iVar10 = (iVar10 - (int)((ulonglong)lVar2 >> 0x20)) - (uint)(uVar15 < uVar13);
        if (-1 < iVar10) {
          iVar12 = vertexPositionQ12[2] - shadingRecord1->worldZQ12;
          lVar2 = (longlong)iVar12 * (longlong)iVar12;
          uVar13 = (uint)lVar2;
          iVar10 = (iVar10 - (int)((ulonglong)lVar2 >> 0x20)) - (uint)(uVar11 < uVar13);
          if (-1 < iVar10) {
            PVar1 = shadingRecord1->packedColorRgbActive;
            uVar15 = *(int *)((int)&shadingRecord1->squaredRadiusQ24 + 4) << 0x14 |
                     (uint)shadingRecord1->squaredRadiusQ24 >> 0xc;
            if (uVar15 != 0) {
              uVar17 = (undefined1)(PVar1 >> 0x18);
              uVar7 = CONCAT11(uVar17,uVar17);
              uVar18 = (undefined1)(PVar1 >> 0x10);
              uVar17 = (undefined1)(PVar1 >> 8);
              mm4PackedValue0 =
                   pmulhw(CONCAT26(uVar7 >> 2,
                                   CONCAT24((ushort)(CONCAT35(CONCAT21(uVar7,uVar18),
                                                              CONCAT14(uVar18,PVar1)) >> 0x20) >> 2,
                                            CONCAT22(CONCAT11(uVar17,uVar17) >> 2,
                                                     CONCAT11((char)PVar1,(char)PVar1) >> 2))),
                          *(undefined8 *)
                           (&g_PackedLightingLookupTable +
                           ((iVar10 * 0x8000000 | uVar11 - uVar13 >> 5) / uVar15) * 8));
              mm0PackedValue1 = paddusb(mm0PackedValue1,mm4PackedValue0);
            }
          }
        }
      }
    }
    shadingRecord1 = shadingRecord1 + 1;
  }
  uVar17 = (undefined1)(vertexPackedColor >> 0x18);
  uVar7 = CONCAT11(uVar17,uVar17);
  uVar18 = (undefined1)(vertexPackedColor >> 0x10);
  uVar17 = (undefined1)(vertexPackedColor >> 8);
  uVar16 = pmulhw(mm0PackedValue1,
                  CONCAT26(uVar7 >> 2,
                           CONCAT24((ushort)(CONCAT35(CONCAT21(uVar7,uVar18),
                                                      CONCAT14(uVar18,vertexPackedColor)) >> 0x20)
                                    >> 2,CONCAT22(CONCAT11(uVar17,uVar17) >> 2,
                                                  CONCAT11((char)vertexPackedColor,
                                                           (char)vertexPackedColor) >> 2))));
  sVar3 = (short)uVar16;
  sVar4 = (short)((ulonglong)uVar16 >> 0x10);
  sVar5 = (short)((ulonglong)uVar16 >> 0x20);
  sVar6 = (short)((ulonglong)uVar16 >> 0x30);
  return CONCAT13((0 < sVar6) * (sVar6 < 0x100) * (char)((ulonglong)uVar16 >> 0x30) - (0xff < sVar6)
                  ,CONCAT12((0 < sVar5) * (sVar5 < 0x100) * (char)((ulonglong)uVar16 >> 0x20) -
                            (0xff < sVar5),
                            CONCAT11((0 < sVar4) * (sVar4 < 0x100) *
                                     (char)((ulonglong)uVar16 >> 0x10) - (0xff < sVar4),
                                     (0 < sVar3) * (sVar3 < 0x100) * (char)uVar16 - (0xff < sVar3)))
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
  int iVar1;
  uint uVar2;
  PackedRgb24 PVar3;
  longlong lVar4;
  short sVar5;
  short sVar6;
  short sVar7;
  short sVar8;
  ushort uVar9;
  sdword sVar10;
  GraphicsShadingRecordCount GVar11;
  uint uVar12;
  int iVar13;
  GraphicsShadingRuntimeRecord *shadingRecord1;
  undefined1 uVar15;
  undefined1 uVar16;
  undefined8 uVar14;
  undefined8 extraout_MM0;
  undefined1 uVar17;
  undefined1 uVar18;
  ulonglong uVar19;
  
  FixedTransform_ApplyDirection
            (&g_ModelLightingTransformedSurfaceNormalScratch,surfaceNormalQ12,
             (GraphicsFixedMatrix3x4 *)&g_ModelViewCompositeTransform);
  uVar16 = (undefined1)(scenePackedColor0 >> 0x10);
  uVar15 = (undefined1)(scenePackedColor0 >> 8);
  uVar17 = (undefined1)(materialPackedColor >> 0x18);
  uVar9 = CONCAT11(uVar17,uVar17);
  uVar18 = (undefined1)(materialPackedColor >> 0x10);
  uVar17 = (undefined1)(materialPackedColor >> 8);
  shadingRecord1 = g_GraphicsShadingNearbyRecords;
  uVar14 = pmulhw(CONCAT26(0x1fff,CONCAT24((ushort)(CONCAT15(uVar16,CONCAT14(uVar16,
                                                  scenePackedColor0)) >> 0x23),
                                           CONCAT22(CONCAT11(uVar15,uVar15) >> 3,
                                                    CONCAT11((char)scenePackedColor0,
                                                             (char)scenePackedColor0) >> 3))),
                  CONCAT26(uVar9 >> 3,
                           CONCAT24((ushort)(CONCAT35(CONCAT21(uVar9,uVar18),
                                                      CONCAT14(uVar18,materialPackedColor)) >> 0x20)
                                    >> 3,CONCAT22(CONCAT11(uVar17,uVar17) >> 3,
                                                  CONCAT11((char)materialPackedColor,
                                                           (char)materialPackedColor) >> 3))));
  uVar19 = (ulonglong)vertexPackedColor;
  for (GVar11 = g_GraphicsShadingNearbyRecordCount; GVar11 != 0; GVar11 = GVar11 - 1) {
    if (shadingRecord1->targetRadiusQ12 != 0) {
      g_ModelLightingVertexToLightVectorScratch.x = shadingRecord1->worldXQ12 - vertexPositionQ12->x
      ;
      g_ModelLightingVertexToLightVectorScratch.y = shadingRecord1->worldYQ12 - vertexPositionQ12->y
      ;
      g_ModelLightingVertexToLightVectorScratch.z = shadingRecord1->worldZQ12 - vertexPositionQ12->z
      ;
      lVar4 = (longlong)g_ModelLightingVertexToLightVectorScratch.y *
              (longlong)g_ModelLightingVertexToLightVectorScratch.y +
              (longlong)g_ModelLightingVertexToLightVectorScratch.x *
              (longlong)g_ModelLightingVertexToLightVectorScratch.x +
              (longlong)g_ModelLightingVertexToLightVectorScratch.z *
              (longlong)g_ModelLightingVertexToLightVectorScratch.z;
      iVar1 = *(int *)((int)&shadingRecord1->squaredRadiusQ24 + 4);
      uVar2 = (uint)shadingRecord1->squaredRadiusQ24;
      if (lVar4 < (longlong)shadingRecord1->squaredRadiusQ24) {
        uVar12 = (uint)lVar4 * 8;
        uVar12 = (((int)((ulonglong)lVar4 >> 0x20) << 3 | (uint)lVar4 >> 0x1d) + iVar1 +
                 (uint)CARRY4(uVar12,uVar2)) * 0x1000 | uVar12 + uVar2 >> 0x14;
        if (uVar12 != 0) {
          FixedVec3_NormalizeQ28
                    (&g_ModelLightingVertexToLightVectorScratch,
                     &g_ModelLightingVertexToLightVectorScratch);
          sVar10 = FixedVec3_DotQ12(&g_ModelLightingVertexToLightVectorScratch,
                                    &g_ModelLightingTransformedSurfaceNormalScratch);
          iVar13 = 0;
          if (-1 < sVar10) {
            iVar13 = sVar10;
          }
          PVar3 = shadingRecord1->packedColorRgbActive;
          uVar15 = (undefined1)(PVar3 >> 0x18);
          uVar9 = CONCAT11(uVar15,uVar15);
          uVar16 = (undefined1)(PVar3 >> 0x10);
          uVar15 = (undefined1)(PVar3 >> 8);
          lVar4 = (longlong)
                  (int)((longlong)(ulonglong)((iVar1 << 0x11 | uVar2 >> 0xf) * 9) /
                       (longlong)(int)uVar12) * (longlong)iVar13;
          uVar14 = pmulhw(CONCAT26(uVar9 >> 2,
                                   CONCAT24((ushort)(CONCAT35(CONCAT21(uVar9,uVar16),
                                                              CONCAT14(uVar16,PVar3)) >> 0x20) >> 2,
                                            CONCAT22(CONCAT11(uVar15,uVar15) >> 2,
                                                     CONCAT11((char)PVar3,(char)PVar3) >> 2))),
                          *(undefined8 *)
                           (&g_PackedLightingLookupTable +
                           ((uint)lVar4 >> 0x1c | (int)((ulonglong)lVar4 >> 0x20) << 4) * 8));
          uVar14 = paddsw(extraout_MM0,uVar14);
        }
      }
    }
    vertexPackedColor = (PackedArgb32)uVar19;
    shadingRecord1 = shadingRecord1 + 1;
  }
  uVar15 = (undefined1)(vertexPackedColor >> 0x18);
  uVar9 = CONCAT11(uVar15,uVar15);
  uVar16 = (undefined1)(vertexPackedColor >> 0x10);
  uVar15 = (undefined1)(vertexPackedColor >> 8);
  uVar14 = pmulhw(uVar14,CONCAT26(uVar9 >> 2,
                                  CONCAT24((ushort)CONCAT31(CONCAT21(uVar9,uVar16),uVar16) >> 2,
                                           CONCAT22(CONCAT11(uVar15,uVar15) >> 2,
                                                    CONCAT11((char)vertexPackedColor,
                                                             (char)vertexPackedColor) >> 2))));
  sVar5 = (short)uVar14;
  sVar6 = (short)((ulonglong)uVar14 >> 0x10);
  sVar7 = (short)((ulonglong)uVar14 >> 0x20);
  sVar8 = (short)((ulonglong)uVar14 >> 0x30);
  return CONCAT13((0 < sVar8) * (sVar8 < 0x100) * (char)((ulonglong)uVar14 >> 0x30) - (0xff < sVar8)
                  ,CONCAT12((0 < sVar7) * (sVar7 < 0x100) * (char)((ulonglong)uVar14 >> 0x20) -
                            (0xff < sVar7),
                            CONCAT11((0 < sVar6) * (sVar6 < 0x100) *
                                     (char)((ulonglong)uVar14 >> 0x10) - (0xff < sVar6),
                                     (0 < sVar5) * (sVar5 < 0x100) * (char)uVar14 - (0xff < sVar5)))
                 );
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
  int iVar1;
  int iVar2;
  GraphicsProjectedPointPair GVar3;
  
  FixedTransform_ApplyPoint
            (&g_GraphicsTransformOutputScratchVec3,&g_GraphicsTransformInputScratchVec3,
             &g_GraphicsTransformScratchMatrix3x4);
  if ((int)g_ProjectionScaleFixed < g_GraphicsTransformOutputScratchVec3.z) {
    GVar3 = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
    iVar1 = GVar3.projectedX >> 0xc;
    iVar2 = GVar3.projectedY >> 0xc;
    if (iVar1 < bounds->minX) {
      bounds->minX = iVar1;
    }
    if (iVar2 < bounds->minY) {
      bounds->minY = iVar2;
    }
    if (bounds->maxX < iVar1) {
      bounds->maxX = iVar1;
    }
    if (bounds->maxY < iVar2) {
      bounds->maxY = iVar2;
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
  int iVar1;
  int iVar2;
  int iVar3;
  GraphicsFixedMatrix3x4 *transformA;
  FixedMathVectorAnglesRegs8 FVar4;
  
  iVar1 = (modelNodeRuntime->worldTransform).translation.x;
  iVar2 = (modelNodeRuntime->worldTransform).translation.y;
  iVar3 = (modelNodeRuntime->worldTransform).translation.z;
  transformA = &modelNodeRuntime->worldTransform;
  FixedTransform_Compose
            ((GraphicsFixedMatrix3x4 *)&g_ModelViewCompositeTransform,transformA,
             &g_ViewProjectionMatrixFixed);
  FVar4 = FixedMath_VectorToAngles3Regs
                    (iVar3 - g_ViewOriginFixed.z,iVar2 - g_ViewOriginFixed.y,
                     iVar1 - g_ViewOriginFixed.x);
  FixedMath_WriteDirectionQ28((GraphicsFixedVec3 *)&g_ModelViewDirectionWorld,FVar4.edx,FVar4.ecx);
  FixedTransform_ApplyTransposeDirection
            ((GraphicsFixedVec3 *)&g_ModelViewDirectionLocal,transformA,
             (GraphicsFixedVec3 *)&g_ModelViewDirectionWorld);
  FixedTransform_ApplyTransposeDirection
            ((GraphicsFixedVec3 *)&g_ModelAuxiliaryForwardDirectionLocal,transformA,
             &g_AuxiliaryForwardDirectionFixed);
  return;
}

