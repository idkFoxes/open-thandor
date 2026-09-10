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
void __fastcall
ModelRender_DrawMeshGroupsWithTemporaryTransform
          (undefined4 param_1,undefined4 param_2,Q12 facingThresholdQ12,
          ModelMeshGroupAddress32 meshGroup,ModelRuntimeNode *modelNode)

{
  AngleTurn32 AVar1;
  AngleTurn32 AVar2;
  AngleTurn32 AVar3;
  sdword sVar4;
  sdword sVar5;
  sdword sVar6;
  GraphicsWorldCoordinateQ12 GVar7;
  sdword sVar8;
  sdword sVar9;
  sdword sVar10;
  GraphicsWorldCoordinateQ12 GVar11;
  sdword sVar12;
  sdword sVar13;
  sdword sVar14;
  GraphicsWorldCoordinateQ12 GVar15;
  uint extraout_EAX;
  uint extraout_EAX_00;
  uint uVar16;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int incomingEcxValue;
  undefined4 extraout_EDX;
  undefined4 extraout_EDX_00;
  undefined4 extraout_EDX_01;
  int *meshGroup_00;
  
  uVar16 = *(uint *)(meshGroup + 0xc);
  incomingEcxValue = *(int *)(meshGroup + 4);
  AVar1 = (modelNode->modelPayload).worldRotationAngle0;
  AVar2 = (modelNode->modelPayload).worldRotationAngle1;
  AVar3 = (modelNode->modelPayload).worldRotationAngle2;
  sVar4 = (modelNode->worldTransform).basisRow0[0];
  sVar5 = (modelNode->worldTransform).basisRow0[1];
  sVar6 = (modelNode->worldTransform).basisRow0[2];
  GVar7 = (modelNode->worldTransform).translation.x;
  sVar8 = (modelNode->worldTransform).basisRow1[0];
  sVar9 = (modelNode->worldTransform).basisRow1[1];
  sVar10 = (modelNode->worldTransform).basisRow1[2];
  GVar11 = (modelNode->worldTransform).translation.y;
  sVar12 = (modelNode->worldTransform).basisRow2[0];
  sVar13 = (modelNode->worldTransform).basisRow2[1];
  sVar14 = (modelNode->worldTransform).basisRow2[2];
  GVar15 = (modelNode->worldTransform).translation.z;
  if ((uVar16 & 1) != 0) {
    ModelNodeRuntime_BuildViewFacingRotation(modelNode);
    uVar16 = extraout_EAX;
    incomingEcxValue = extraout_ECX;
    param_2 = extraout_EDX;
  }
  if ((uVar16 & 2) != 0) {
    ModelNodeRuntime_BuildBillboardRotation(modelNode);
    incomingEcxValue = extraout_ECX_00;
    param_2 = extraout_EDX_00;
  }
  meshGroup_00 = (int *)(meshGroup + 0x20);
  uVar16 = (modelNode->modelPayload).meshGroupMask;
  for (; incomingEcxValue != 0; incomingEcxValue = incomingEcxValue + -1) {
    if ((meshGroup_00[1] & uVar16) != 0) {
      ModelRender_SubmitMeshTriangles
                (incomingEcxValue,param_2,facingThresholdQ12,(ModelMeshGroupAddress32)meshGroup_00,
                 modelNode);
      uVar16 = extraout_EAX_00;
      incomingEcxValue = extraout_ECX_01;
      param_2 = extraout_EDX_01;
    }
    meshGroup_00 = (int *)((int)meshGroup_00 + *meshGroup_00);
  }
  (modelNode->worldTransform).translation.z = GVar15;
  (modelNode->worldTransform).basisRow2[2] = sVar14;
  (modelNode->worldTransform).basisRow2[1] = sVar13;
  (modelNode->worldTransform).basisRow2[0] = sVar12;
  (modelNode->worldTransform).translation.y = GVar11;
  (modelNode->worldTransform).basisRow1[2] = sVar10;
  (modelNode->worldTransform).basisRow1[1] = sVar9;
  (modelNode->worldTransform).basisRow1[0] = sVar8;
  (modelNode->worldTransform).translation.x = GVar7;
  (modelNode->worldTransform).basisRow0[2] = sVar6;
  (modelNode->worldTransform).basisRow0[1] = sVar5;
  (modelNode->worldTransform).basisRow0[0] = sVar4;
  (modelNode->modelPayload).worldRotationAngle2 = AVar3;
  (modelNode->modelPayload).worldRotationAngle1 = AVar2;
  (modelNode->modelPayload).worldRotationAngle0 = AVar1;
  return;
}

/* Address: 0x004BE1F0.
   Ownership: graphics/render/model.
   Purpose: Handles model render draw mesh groups alternate path.
   Local calls: ModelRender_SubmitMeshTrianglesAlternatePath.
   Cross-module calls: ModelNodeRuntime_BuildViewFacingRotation [world/model/hierarchy],
   ModelNodeRuntime_BuildBillboardRotation [world/model/hierarchy].
*/
undefined4 __fastcall
ModelRender_DrawMeshGroupsAlternatePath
          (undefined4 param_1,undefined4 param_2,int param_3,ModelRuntimeNode *param_4)

{
  undefined4 in_EAX;
  uint extraout_EAX;
  uint extraout_EAX_00;
  uint uVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int iVar2;
  undefined4 extraout_EDX;
  undefined4 extraout_EDX_00;
  undefined4 extraout_EDX_01;
  int *piVar3;
  
  uVar1 = *(uint *)(param_3 + 0xc);
  iVar2 = *(int *)(param_3 + 4);
  if ((uVar1 & 1) != 0) {
    ModelNodeRuntime_BuildViewFacingRotation(param_4);
    uVar1 = extraout_EAX;
    iVar2 = extraout_ECX;
    param_2 = extraout_EDX;
  }
  if ((uVar1 & 2) != 0) {
    ModelNodeRuntime_BuildBillboardRotation(param_4);
    iVar2 = extraout_ECX_00;
    param_2 = extraout_EDX_00;
  }
  piVar3 = (int *)(param_3 + 0x20);
  uVar1 = (param_4->modelPayload).meshGroupMask;
  for (; iVar2 != 0; iVar2 = iVar2 + -1) {
    if ((piVar3[1] & uVar1) != 0) {
      ModelRender_SubmitMeshTrianglesAlternatePath(iVar2,param_2,(int)piVar3,param_4);
      uVar1 = extraout_EAX_00;
      iVar2 = extraout_ECX_01;
      param_2 = extraout_EDX_01;
    }
    piVar3 = (int *)((int)piVar3 + *piVar3);
  }
  return in_EAX;
}

/* Address: 0x0050A5C0.
   Ownership: graphics/render/model.
   Purpose: Handles model projected bounds accumulate hierarchy recursive.
   Local calls: ModelProjectedBounds_AccumulateNode.
*/
undefined4 __fastcall
ModelProjectedBounds_AccumulateHierarchyRecursive
          (undefined4 param_1,undefined4 param_2,int *param_3,int param_4)

{
  undefined4 in_EAX;
  int extraout_ECX;
  int iVar1;
  undefined4 extraout_EDX;
  undefined4 extraout_EDX_00;
  undefined4 uVar2;
  
  ModelProjectedBounds_AccumulateNode(param_1,param_2,param_3,param_4);
  uVar2 = extraout_EDX;
  for (iVar1 = *(int *)(param_4 + 200); iVar1 != 0; iVar1 = iVar1 + -1) {
    if (*(int *)(param_4 + 0xcc) != 0) {
      ModelProjectedBounds_AccumulateHierarchyRecursive
                (iVar1,uVar2,param_3,*(int *)(param_4 + 0xcc));
      iVar1 = extraout_ECX;
      uVar2 = extraout_EDX_00;
    }
    param_4 = param_4 + 4;
  }
  return in_EAX;
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
undefined8
ModelRender_PrepareProjectedVertex
          (ModelRuntimeNode *modelNode,ModelMeshGroupAddress32 meshGroup,GraphicsFixedVec3 *vertex)

{
  longlong lVar1;
  undefined4 in_EAX;
  PackedArgb32 PVar2;
  uint uVar3;
  uint extraout_ECX;
  undefined4 in_EDX;
  GraphicsFixedVec3 *viewPoint;
  GraphicsFixedVec3 *surfaceNormalQ12;
  GraphicsProjectedPointEdxEax8 projectedScreenCoordinatePair;
  GraphicsWorldCoordinateQ12 GVar4;
  longlong scaledVertexCoordinateProduct;
  GraphicsWorldCoordinateQ12 savedVertexZQ12;
  GraphicsWorldCoordinateQ12 savedVertexYQ12;
  
  uVar3 = *(uint *)(meshGroup + 0x34) & 0x8e00;
  if (vertex[4].x == -0x80000000) {
    GVar4 = vertex->x;
    savedVertexYQ12 = vertex->y;
    savedVertexZQ12 = vertex->z;
    uVar3 = (uint)modelNode->renderDepthBiasOrState >> 1;
    if (-1 < vertex->z) {
      if (vertex->z != 0) {
        vertex->z = vertex->z + uVar3;
      }
      vertex->z = vertex->z + uVar3;
    }
    if ((modelNode->runtimeFlags & 0x800) != 0) {
      scaledVertexCoordinateProduct = (longlong)vertex->x * (longlong)modelNode->modelScaleQ12;
      vertex->x = (int)((ulonglong)scaledVertexCoordinateProduct >> 0x20) << 0x14 |
                  (uint)scaledVertexCoordinateProduct >> 0xc;
      lVar1 = (longlong)vertex->y * (longlong)modelNode->modelScaleQ12;
      vertex->y = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
      lVar1 = (longlong)vertex->z * (longlong)modelNode->modelScaleQ12;
      vertex->z = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
    }
    FixedTransform_ApplyPoint
              ((GraphicsFixedVec3 *)&vertex[2].z,vertex,
               (GraphicsFixedMatrix3x4 *)&g_ModelViewCompositeTransform);
    projectedScreenCoordinatePair = Graphics_ProjectViewPoint(viewPoint);
    vertex->z = savedVertexZQ12;
    vertex->y = savedVertexYQ12;
    vertex->x = GVar4;
    vertex[4].x = (int)projectedScreenCoordinatePair;
    vertex[4].y = (int)(projectedScreenCoordinatePair >> 0x20);
    uVar3 = extraout_ECX;
  }
  else if ((uVar3 == vertex[4].z) && ((*(uint *)(meshGroup + 0x34) & 0x8000) == 0)) {
    return CONCAT44(in_EDX,in_EAX);
  }
  vertex[4].z = uVar3;
  PVar2 = modelNode->tintArgb;
  surfaceNormalQ12 = (GraphicsFixedVec3 *)&vertex[1].y;
  if ((uVar3 & 0x200) != 0) {
    vertex[3].z = PVar2;
    return CONCAT44(in_EDX,in_EAX);
  }
  if ((uVar3 & 0x800) == 0) {
    if ((uVar3 & 0x8000) != 0) {
      surfaceNormalQ12 = (GraphicsFixedVec3 *)(meshGroup + 0x24);
    }
    PVar2 = ModelRender_ComputeVertexIntensityDefaultPath
                      (vertex[2].y,&vertex[2].z,0x4cb1a0,g_SceneBoundsFixed.bound5,
                       g_SceneBoundsFixed.bound4,
                       (GraphicsFixedVec3 *)&g_ModelAuxiliaryForwardDirectionLocal,PVar2,
                       surfaceNormalQ12);
    vertex[3].z = PVar2;
    return CONCAT44(in_EDX,in_EAX);
  }
  PVar2 = ModelRender_ComputeVertexIntensityScaledPath
                    (vertex[2].y,&vertex[2].z,
                     ((modelNode->modelPayload).modelResource)->lightingScaleQ12,
                     g_SceneBoundsFixed.bound7,g_SceneBoundsFixed.bound6,
                     (GraphicsFixedVec3 *)&g_ModelAuxiliaryForwardDirectionLocal,PVar2,vertex);
  vertex[3].z = PVar2;
  return CONCAT44(in_EDX,in_EAX);
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
undefined8 __fastcall
ModelRender_SubmitTriangle
          (undefined4 incomingEcxValue,undefined4 preservedEdxValue,Q12 facingThresholdQ12,
          GraphicsTriangleInput *triangle,ModelRuntimeNode *modelNode)

{
  GraphicsProjectedVertexSource *vertex;
  int iVar1;
  uint uVar2;
  uint uVar3;
  GraphicsTextureSet *pGVar4;
  GraphicsPaletteAsset *pGVar5;
  undefined4 in_EAX;
  uint uVar6;
  GraphicsFixedVec3 *vertex_00;
  int extraout_ECX;
  GraphicsSubresourceIndex extraout_ECX_00;
  GraphicsSubresourceIndex GVar7;
  GraphicsSubresourceIndex extraout_ECX_01;
  int iVar8;
  bool bVar9;
  undefined8 uVar10;
  GraphicsTextureSetEntry *textureEntry;
  
  uVar10 = ModelRender_ComputeFacingDotQ12(triangle);
  if (((triangle->renderFlags & 0x400) != 0) || ((int)uVar10 < facingThresholdQ12)) {
    vertex = triangle->vertex0;
    ModelRender_PrepareProjectedVertex
              (modelNode,(ModelMeshGroupAddress32)triangle,(GraphicsFixedVec3 *)vertex);
    uVar10 = ModelRender_PrepareProjectedVertex
                       (modelNode,(ModelMeshGroupAddress32)triangle,vertex_00);
    uVar10 = ModelRender_PrepareProjectedVertex
                       (modelNode,(ModelMeshGroupAddress32)triangle,
                        (GraphicsFixedVec3 *)((ulonglong)uVar10 >> 0x20));
    iVar8 = (int)((ulonglong)uVar10 >> 0x20);
    iVar1 = *(int *)(iVar8 + 0x30);
    if (((g_ProjectionClipRect.minX <= vertex->screenX) ||
        ((g_ProjectionClipRect.minX <= *(int *)(extraout_ECX + 0x30) ||
         (g_ProjectionClipRect.minX <= iVar1)))) &&
       ((vertex->screenX < g_ProjectionClipRect.maxX ||
        ((*(int *)(extraout_ECX + 0x30) < g_ProjectionClipRect.maxX ||
         (iVar1 < g_ProjectionClipRect.maxX)))))) {
      uVar6 = vertex->screenY;
      uVar2 = *(uint *)(extraout_ECX + 0x34);
      uVar3 = *(uint *)(iVar8 + 0x34);
      if (((g_ProjectionClipRect.minY <= (int)uVar6) ||
          ((g_ProjectionClipRect.minY <= (int)uVar2 || (g_ProjectionClipRect.minY <= (int)uVar3))))
         && ((bVar9 = uVar6 < (uint)g_ProjectionClipRect.maxY,
             (int)uVar6 < g_ProjectionClipRect.maxY ||
             ((bVar9 = uVar2 < (uint)g_ProjectionClipRect.maxY,
              (int)uVar2 < g_ProjectionClipRect.maxY ||
              (bVar9 = uVar3 < (uint)g_ProjectionClipRect.maxY,
              (int)uVar3 < g_ProjectionClipRect.maxY)))))) {
        GraphicsPrimitiveQueue_AppendTriangle
                  (triangle->renderFlags,triangle,triangle->vertex2,triangle->vertex1,
                   triangle->vertex0,g_ActivePrimitiveQueue);
        if (!bVar9) {
          GraphicsPrimitiveQueue_SetVertexColors
                    (triangle->vertex2->reserved2C,triangle->vertex1->reserved2C,
                     triangle->vertex0->reserved2C,g_ActivePrimitiveQueue);
          pGVar4 = (modelNode->modelPayload).textureSet;
          textureEntry = (GraphicsTextureSetEntry *)0x0;
          if ((pGVar4 != (GraphicsTextureSet *)0x0) &&
             (triangle->subresourceIndex < pGVar4->subresourceCount)) {
            textureEntry = pGVar4->entries +
                           triangle->subresourceIndex + modelNode->textureSubresourceBaseIndex;
          }
          pGVar5 = (modelNode->modelPayload).paletteAsset;
          if ((pGVar5 == (GraphicsPaletteAsset *)0x0) ||
             (uVar6 = triangle->renderFlags & 0x1ff, pGVar5->paletteBankCount <= uVar6)) {
            GraphicsPrimitiveQueue_SetMaterial(0xffffffff,textureEntry,g_ActivePrimitiveQueue);
            GVar7 = triangle->subresourceIndex;
            if ((modelNode->runtimeFlags & 0x80) != 0) {
              if (GVar7 == modelNode->primaryAnimatedSubresourceIndex) {
                GraphicsPrimitiveQueue_OffsetTextureCoordinates
                          (modelNode->primaryTextureOffsetV,modelNode->primaryTextureOffsetU,
                           g_ActivePrimitiveQueue);
                GVar7 = extraout_ECX_01;
              }
              if (((modelNode->runtimeFlags & 0x400) != 0) &&
                 (GVar7 == modelNode->secondaryAnimatedSubresourceIndex)) {
                GraphicsPrimitiveQueue_OffsetTextureCoordinates
                          (modelNode->secondaryTextureOffsetV,modelNode->secondaryTextureOffsetU,
                           g_ActivePrimitiveQueue);
              }
            }
          }
          else {
            GraphicsPrimitiveQueue_SetMaterial
                      (pGVar5->paletteEntries[uVar6].argb8888,textureEntry,g_ActivePrimitiveQueue);
            GVar7 = triangle->subresourceIndex;
            if ((modelNode->runtimeFlags & 0x80) != 0) {
              if (GVar7 == modelNode->primaryAnimatedSubresourceIndex) {
                GraphicsPrimitiveQueue_OffsetTextureCoordinates
                          (modelNode->primaryTextureOffsetV,modelNode->primaryTextureOffsetU,
                           g_ActivePrimitiveQueue);
                GVar7 = extraout_ECX_00;
              }
              if (((modelNode->runtimeFlags & 0x400) != 0) &&
                 (GVar7 == modelNode->secondaryAnimatedSubresourceIndex)) {
                GraphicsPrimitiveQueue_OffsetTextureCoordinates
                          (modelNode->secondaryTextureOffsetV,modelNode->secondaryTextureOffsetU,
                           g_ActivePrimitiveQueue);
                return CONCAT44(preservedEdxValue,in_EAX);
              }
            }
          }
        }
      }
    }
  }
  return CONCAT44(preservedEdxValue,in_EAX);
}

/* Address: 0x004BDC20.
   Ownership: graphics/render/model.
   Purpose: Prepares model-local view directions, invalidates cached projected vertices, and submits every triangle
   in one mesh record. Typed parameters: p3 meshGroup→ModelMeshGroupAddress32_V345. Calling convention, complete
   VariableStorage serialization, function bytes, control flow, globals, locals, and executable data remain
   unchanged.
   Local calls: ModelRender_PrepareViewDirections, ModelRender_SubmitTriangle.
*/
void __fastcall
ModelRender_SubmitMeshTriangles
          (undefined4 incomingEcxValue,undefined4 preservedEdxValue,Q12 facingThresholdQ12,
          ModelMeshGroupAddress32 meshGroup,ModelRuntimeNode *modelNode)

{
  int extraout_ECX;
  int iVar1;
  int extraout_ECX_00;
  GraphicsTriangleInput *triangle;
  undefined8 uVar2;
  
  ModelRender_PrepareViewDirections(modelNode);
  triangle = (GraphicsTriangleInput *)(meshGroup + 0x20);
  for (iVar1 = extraout_ECX; iVar1 != 0; iVar1 = iVar1 + -1) {
    triangle->subresourceIndex = 0x80000000;
    triangle = (GraphicsTriangleInput *)&triangle[1].textureV0;
  }
  iVar1 = *(int *)(meshGroup + 0xc);
  while (iVar1 != 0) {
    uVar2 = ModelRender_SubmitTriangle(iVar1,modelNode,facingThresholdQ12,triangle,modelNode);
    modelNode = (ModelRuntimeNode *)((ulonglong)uVar2 >> 0x20);
    triangle = (GraphicsTriangleInput *)&triangle[1].textureV0;
    iVar1 = extraout_ECX_00 + -1;
  }
  return;
}

/* Address: 0x004BE180.
   Ownership: graphics/render/model.
   Purpose: Handles model render submit mesh triangles alternate path.
   Local calls: ModelRender_PrepareViewDirections, ModelRender_SubmitTriangleAlternatePath.
*/
void __fastcall
ModelRender_SubmitMeshTrianglesAlternatePath
          (undefined4 param_1,undefined4 param_2,int param_3,ModelRuntimeNode *param_4)

{
  int extraout_ECX;
  int iVar1;
  int extraout_ECX_00;
  GraphicsTriangleInput *pGVar2;
  undefined8 uVar3;
  
  ModelRender_PrepareViewDirections(param_4);
  pGVar2 = (GraphicsTriangleInput *)(param_3 + 0x20);
  for (iVar1 = extraout_ECX; iVar1 != 0; iVar1 = iVar1 + -1) {
    pGVar2->subresourceIndex = 0x80000000;
    pGVar2 = (GraphicsTriangleInput *)&pGVar2[1].textureV0;
  }
  if (*(int *)(param_3 + 0xc) != 0) {
    do {
      uVar3 = ModelRender_SubmitTriangleAlternatePath(pGVar2,(int)param_4);
      param_4 = (ModelRuntimeNode *)((ulonglong)uVar3 >> 0x20);
      pGVar2 = (GraphicsTriangleInput *)&pGVar2[1].textureV0;
    } while (extraout_ECX_00 != 1);
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
undefined8
ModelRender_PrepareProjectedVertexAlternatePath(int param_1,int param_2,GraphicsFixedVec3 *param_3)

{
  uint uVar1;
  undefined4 in_EAX;
  PackedArgb32 PVar2;
  uint uVar3;
  uint extraout_ECX;
  undefined4 in_EDX;
  GraphicsFixedVec3 *viewPoint;
  GraphicsFixedVec3 *pGVar4;
  GraphicsProjectedPointEdxEax8 GVar5;
  
  uVar3 = *(uint *)(param_2 + 0x34) & 0x8e00;
  if (param_3[4].x == -0x80000000) {
    FixedTransform_ApplyPoint
              ((GraphicsFixedVec3 *)&param_3[2].z,param_3,
               (GraphicsFixedMatrix3x4 *)&g_ModelViewCompositeTransform);
    if (viewPoint->z < (int)g_ProjectionScaleFixed) {
LAB_004bd788:
      param_3[4].x = 0x7fffffff;
      return CONCAT44(in_EDX,in_EAX);
    }
    GVar5 = Graphics_ProjectViewPoint(viewPoint);
    param_3[4].x = (int)GVar5;
    param_3[4].y = (int)(GVar5 >> 0x20);
    uVar3 = extraout_ECX;
  }
  else {
    if (param_3[4].x == 0x7fffffff) goto LAB_004bd788;
    if ((uVar3 == param_3[4].z) && ((*(uint *)(param_2 + 0x34) & 0x8000) == 0)) {
      return CONCAT44(in_EDX,in_EAX);
    }
  }
  uVar1 = *(uint *)(param_1 + 0x58);
  param_3[4].z = uVar3;
  if ((uVar3 & 0x200) == 0) {
    pGVar4 = (GraphicsFixedVec3 *)&param_3[1].y;
    if ((uVar3 & 0x8000) != 0) {
      pGVar4 = (GraphicsFixedVec3 *)(param_2 + 0x24);
    }
    PVar2 = ModelRender_ComputeNearbyLightPackedVertexColorAlternatePath
                      (param_3[2].y,(GraphicsFixedVec3 *)&param_3[2].z,g_SceneBoundsFixed.bound5,
                       uVar1,pGVar4);
    param_3[3].z = PVar2;
    return CONCAT44(in_EDX,in_EAX);
  }
  param_3[3].z = uVar1 | 0xffffff;
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x004BDFB0.
   Ownership: graphics/render/model.
   Purpose: Handles model render submit triangle alternate path.
   Local calls: ModelRender_PrepareProjectedVertexAlternatePath.
   Cross-module calls: GraphicsPrimitiveQueue_AppendTriangle [graphics/render/primitives],
   GraphicsPrimitiveQueue_SetVertexColors [graphics/render/primitives], GraphicsPrimitiveQueue_SetMaterial
   [graphics/render/primitives].
*/
undefined8 ModelRender_SubmitTriangleAlternatePath(GraphicsTriangleInput *param_1,int param_2)

{
  GraphicsProjectedVertexSource *pGVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  undefined4 in_EAX;
  uint uVar5;
  GraphicsFixedVec3 *extraout_ECX;
  int extraout_ECX_00;
  undefined4 in_EDX;
  int iVar6;
  undefined1 in_CF;
  bool bVar7;
  undefined8 uVar8;
  GraphicsTextureSetEntry *textureEntry;
  
  pGVar1 = param_1->vertex0;
  ModelRender_PrepareProjectedVertexAlternatePath(param_2,(int)param_1,(GraphicsFixedVec3 *)pGVar1);
  if (!(bool)in_CF) {
    uVar8 = ModelRender_PrepareProjectedVertexAlternatePath(param_2,(int)param_1,extraout_ECX);
    if (!(bool)in_CF) {
      uVar8 = ModelRender_PrepareProjectedVertexAlternatePath
                        (param_2,(int)param_1,(GraphicsFixedVec3 *)((ulonglong)uVar8 >> 0x20));
      iVar6 = (int)((ulonglong)uVar8 >> 0x20);
      if (!(bool)in_CF) {
        iVar2 = *(int *)(iVar6 + 0x30);
        if ((((g_ProjectionClipRect.minX <= pGVar1->screenX) ||
             (g_ProjectionClipRect.minX <= *(int *)(extraout_ECX_00 + 0x30))) ||
            (g_ProjectionClipRect.minX <= iVar2)) &&
           (((pGVar1->screenX < g_ProjectionClipRect.maxX ||
             (*(int *)(extraout_ECX_00 + 0x30) < g_ProjectionClipRect.maxX)) ||
            (iVar2 < g_ProjectionClipRect.maxX)))) {
          uVar5 = pGVar1->screenY;
          uVar3 = *(uint *)(extraout_ECX_00 + 0x34);
          uVar4 = *(uint *)(iVar6 + 0x34);
          if ((((g_ProjectionClipRect.minY <= (int)uVar5) ||
               (g_ProjectionClipRect.minY <= (int)uVar3)) ||
              (g_ProjectionClipRect.minY <= (int)uVar4)) &&
             (((bVar7 = uVar5 < (uint)g_ProjectionClipRect.maxY,
               (int)uVar5 < g_ProjectionClipRect.maxY ||
               (bVar7 = uVar3 < (uint)g_ProjectionClipRect.maxY,
               (int)uVar3 < g_ProjectionClipRect.maxY)) ||
              (bVar7 = uVar4 < (uint)g_ProjectionClipRect.maxY,
              (int)uVar4 < g_ProjectionClipRect.maxY)))) {
            GraphicsPrimitiveQueue_AppendTriangle
                      (param_1->renderFlags,param_1,param_1->vertex2,param_1->vertex1,
                       param_1->vertex0,g_ActivePrimitiveQueue);
            if (!bVar7) {
              GraphicsPrimitiveQueue_SetVertexColors
                        (param_1->vertex2->reserved2C,param_1->vertex1->reserved2C,
                         param_1->vertex0->reserved2C,g_ActivePrimitiveQueue);
              uVar5 = param_1->subresourceIndex;
              textureEntry = (GraphicsTextureSetEntry *)0x0;
              if ((uVar5 != 0xffffffff) && (uVar5 < *(uint *)(*(int *)(param_2 + 0x34) + 4))) {
                textureEntry = (GraphicsTextureSetEntry *)
                               (*(int *)(param_2 + 0x34) + 8 +
                               (uVar5 + *(int *)(param_2 + 0x50)) * 0x20);
              }
              iVar6 = *(int *)(param_2 + 0x30);
              if ((iVar6 != 0) &&
                 (uVar5 = param_1->renderFlags & 0xffff01ff, uVar5 < *(uint *)(iVar6 + 0xb0))) {
                GraphicsPrimitiveQueue_SetMaterial
                          (*(PackedArgb32 *)(iVar6 + 0x204 + uVar5 * 8),textureEntry,
                           g_ActivePrimitiveQueue);
                return CONCAT44(in_EDX,in_EAX);
              }
              GraphicsPrimitiveQueue_SetMaterial(0,textureEntry,g_ActivePrimitiveQueue);
            }
          }
        }
      }
    }
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x0050A4A0.
   Ownership: graphics/render/model.
   Purpose: Handles model projected bounds accumulate node.
   Local calls: ModelProjectedBounds_ExpandWithCurrentScratchPoint.
   Cross-module calls: FixedTransform_Compose [core/math/fixed].
*/
void __fastcall
ModelProjectedBounds_AccumulateNode(undefined4 param_1,undefined4 param_2,int *param_3,int param_4)

{
  int iVar1;
  GraphicsWorldCoordinateQ12 GVar2;
  undefined8 uVar3;
  
  iVar1 = *(int *)(param_4 + 0x40);
  if ((*(int *)(iVar1 + 0xd8) != 0) && ((*(uint *)(iVar1 + 0x20c) & 4) == 0)) {
    FixedTransform_Compose
              (&g_GraphicsTransformScratchMatrix3x4,(GraphicsFixedMatrix3x4 *)(param_4 + 0x70),
               &g_ViewProjectionMatrixFixed);
    g_GraphicsTransformInputScratchVec3.x = *(GraphicsWorldCoordinateQ12 *)(iVar1 + 0xc0);
    GVar2 = *(GraphicsWorldCoordinateQ12 *)(iVar1 + 0xc4);
    g_GraphicsTransformInputScratchVec3.y = *(GraphicsWorldCoordinateQ12 *)(iVar1 + 200);
    g_GraphicsTransformInputScratchVec3.z = *(GraphicsWorldCoordinateQ12 *)(iVar1 + 0xd0);
    ModelProjectedBounds_ExpandWithCurrentScratchPoint(param_3);
    g_GraphicsTransformInputScratchVec3.x = GVar2;
    uVar3 = ModelProjectedBounds_ExpandWithCurrentScratchPoint(param_3);
    g_GraphicsTransformInputScratchVec3.x = (GraphicsWorldCoordinateQ12)uVar3;
    g_GraphicsTransformInputScratchVec3.y = *(GraphicsWorldCoordinateQ12 *)(iVar1 + 0xcc);
    ModelProjectedBounds_ExpandWithCurrentScratchPoint(param_3);
    g_GraphicsTransformInputScratchVec3.x = GVar2;
    uVar3 = ModelProjectedBounds_ExpandWithCurrentScratchPoint(param_3);
    g_GraphicsTransformInputScratchVec3.x = (GraphicsWorldCoordinateQ12)uVar3;
    g_GraphicsTransformInputScratchVec3.y = *(GraphicsWorldCoordinateQ12 *)(iVar1 + 200);
    g_GraphicsTransformInputScratchVec3.z = *(GraphicsWorldCoordinateQ12 *)(iVar1 + 0xd4);
    ModelProjectedBounds_ExpandWithCurrentScratchPoint(param_3);
    g_GraphicsTransformInputScratchVec3.x = GVar2;
    uVar3 = ModelProjectedBounds_ExpandWithCurrentScratchPoint(param_3);
    g_GraphicsTransformInputScratchVec3.x = (GraphicsWorldCoordinateQ12)uVar3;
    g_GraphicsTransformInputScratchVec3.y = *(GraphicsWorldCoordinateQ12 *)(iVar1 + 0xcc);
    ModelProjectedBounds_ExpandWithCurrentScratchPoint(param_3);
    g_GraphicsTransformInputScratchVec3.x = GVar2;
    ModelProjectedBounds_ExpandWithCurrentScratchPoint(param_3);
  }
  return;
}

/* Address: 0x004BD7E0.
   Ownership: graphics/render/model.
   Purpose: Returns the Q12 dot product between a triangle direction vector and the prepared view direction.
   Cross-module calls: FixedVec3_DotQ12 [core/math/fixed].
*/
undefined8 ModelRender_ComputeFacingDotQ12(GraphicsTriangleInput *modelNodeRuntime)

{
  sdword facingDotQ12;
  undefined4 extraout_EDX;
  
  facingDotQ12 = FixedVec3_DotQ12((GraphicsFixedVec3 *)&modelNodeRuntime->planeNormalXQ12,
                                  (GraphicsFixedVec3 *)&g_ModelViewDirectionLocal);
  return CONCAT44(extraout_EDX,facingDotQ12);
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
          (PackedArgb32 param_1,GraphicsFixedVec3 *param_2,PackedArgb32 param_3,PackedArgb32 param_4
          ,GraphicsFixedVec3 *param_5)

{
  uint uVar1;
  PackedRgb24 PVar2;
  longlong lVar3;
  short sVar4;
  short sVar5;
  short sVar6;
  short sVar7;
  ushort uVar8;
  sdword sVar9;
  int extraout_ECX;
  GraphicsShadingRecordCount GVar10;
  uint extraout_EDX;
  uint uVar11;
  int iVar12;
  GraphicsShadingRuntimeRecord *shadingRecord1;
  undefined1 uVar14;
  undefined1 uVar15;
  undefined8 uVar13;
  undefined8 extraout_MM0;
  undefined1 uVar16;
  undefined1 uVar17;
  ulonglong uVar18;
  
  FixedTransform_ApplyDirection
            ((GraphicsFixedVec3 *)0x4cc704,param_5,
             (GraphicsFixedMatrix3x4 *)&g_ModelViewCompositeTransform);
  uVar15 = (undefined1)(param_3 >> 0x10);
  uVar14 = (undefined1)(param_3 >> 8);
  uVar16 = (undefined1)(param_4 >> 0x18);
  uVar8 = CONCAT11(uVar16,uVar16);
  uVar17 = (undefined1)(param_4 >> 0x10);
  uVar16 = (undefined1)(param_4 >> 8);
  shadingRecord1 = g_GraphicsShadingNearbyRecords;
  uVar13 = pmulhw(CONCAT26(0x1fff,CONCAT24((ushort)(CONCAT15(uVar15,CONCAT14(uVar15,param_3)) >>
                                                   0x23),
                                           CONCAT22(CONCAT11(uVar14,uVar14) >> 3,
                                                    CONCAT11((char)param_3,(char)param_3) >> 3))),
                  CONCAT26(uVar8 >> 3,
                           CONCAT24((ushort)(CONCAT35(CONCAT21(uVar8,uVar17),
                                                      CONCAT14(uVar17,param_4)) >> 0x20) >> 3,
                                    CONCAT22(CONCAT11(uVar16,uVar16) >> 3,
                                             CONCAT11((char)param_4,(char)param_4) >> 3))));
  uVar18 = (ulonglong)param_1;
  for (GVar10 = g_GraphicsShadingNearbyRecordCount; GVar10 != 0; GVar10 = GVar10 - 1) {
    if (shadingRecord1->targetRadiusQ12 != 0) {
      DAT_004cc6f8 = shadingRecord1->worldXQ12 - param_2->x;
      DAT_004cc6fc = shadingRecord1->worldYQ12 - param_2->y;
      DAT_004cc700 = shadingRecord1->worldZQ12 - param_2->z;
      lVar3 = (longlong)DAT_004cc6fc * (longlong)DAT_004cc6fc +
              (longlong)DAT_004cc6f8 * (longlong)DAT_004cc6f8 +
              (longlong)DAT_004cc700 * (longlong)DAT_004cc700;
      uVar1 = (uint)shadingRecord1->squaredRadiusQ24;
      if ((lVar3 < (longlong)shadingRecord1->squaredRadiusQ24) &&
         (uVar11 = (uint)lVar3 * 8,
         (((int)((ulonglong)lVar3 >> 0x20) << 3 | (uint)lVar3 >> 0x1d) +
          *(int *)((int)&shadingRecord1->squaredRadiusQ24 + 4) + (uint)CARRY4(uVar11,uVar1) &
         0xfffff) != 0 || uVar11 + uVar1 >> 0x14 != 0)) {
        FixedVec3_NormalizeQ28
                  ((GraphicsFixedVec3 *)&DAT_004cc6f8,(GraphicsFixedVec3 *)&DAT_004cc6f8);
        sVar9 = FixedVec3_DotQ12((GraphicsFixedVec3 *)&DAT_004cc6f8,(GraphicsFixedVec3 *)0x4cc704);
        iVar12 = 0;
        if (-1 < sVar9) {
          iVar12 = sVar9;
        }
        PVar2 = shadingRecord1->packedColorRgbActive;
        uVar14 = (undefined1)(PVar2 >> 0x18);
        uVar8 = CONCAT11(uVar14,uVar14);
        uVar15 = (undefined1)(PVar2 >> 0x10);
        uVar14 = (undefined1)(PVar2 >> 8);
        lVar3 = (longlong)(int)((longlong)(ulonglong)extraout_EDX / (longlong)extraout_ECX) *
                (longlong)iVar12;
        uVar13 = pmulhw(CONCAT26(uVar8 >> 2,
                                 CONCAT24((ushort)(CONCAT35(CONCAT21(uVar8,uVar15),
                                                            CONCAT14(uVar15,PVar2)) >> 0x20) >> 2,
                                          CONCAT22(CONCAT11(uVar14,uVar14) >> 2,
                                                   CONCAT11((char)PVar2,(char)PVar2) >> 2))),
                        *(undefined8 *)
                         (&g_PackedLightingLookupTable +
                         ((uint)lVar3 >> 0x1c | (int)((ulonglong)lVar3 >> 0x20) << 4) * 8));
        uVar13 = paddsw(extraout_MM0,uVar13);
      }
    }
    param_1 = (PackedArgb32)uVar18;
    shadingRecord1 = shadingRecord1 + 1;
  }
  uVar14 = (undefined1)(param_1 >> 0x18);
  uVar8 = CONCAT11(uVar14,uVar14);
  uVar15 = (undefined1)(param_1 >> 0x10);
  uVar14 = (undefined1)(param_1 >> 8);
  uVar13 = pmulhw(uVar13,CONCAT26(uVar8 >> 2,
                                  CONCAT24((ushort)CONCAT31(CONCAT21(uVar8,uVar15),uVar15) >> 2,
                                           CONCAT22(CONCAT11(uVar14,uVar14) >> 2,
                                                    CONCAT11((char)param_1,(char)param_1) >> 2))));
  sVar4 = (short)uVar13;
  sVar5 = (short)((ulonglong)uVar13 >> 0x10);
  sVar6 = (short)((ulonglong)uVar13 >> 0x20);
  sVar7 = (short)((ulonglong)uVar13 >> 0x30);
  return CONCAT13((0 < sVar7) * (sVar7 < 0x100) * (char)((ulonglong)uVar13 >> 0x30) - (0xff < sVar7)
                  ,CONCAT12((0 < sVar6) * (sVar6 < 0x100) * (char)((ulonglong)uVar13 >> 0x20) -
                            (0xff < sVar6),
                            CONCAT11((0 < sVar5) * (sVar5 < 0x100) *
                                     (char)((ulonglong)uVar13 >> 0x10) - (0xff < sVar5),
                                     (0 < sVar4) * (sVar4 < 0x100) * (char)uVar13 - (0xff < sVar4)))
                 );
}

/* Address: 0x0050A430.
   Ownership: graphics/render/model.
   Purpose: Handles model projected bounds expand with current scratch point.
   Cross-module calls: FixedTransform_ApplyPoint [core/math/fixed], Graphics_ProjectViewPoint
   [graphics/core/runtime].
*/
undefined8 ModelProjectedBounds_ExpandWithCurrentScratchPoint(int *param_1)

{
  undefined4 in_EAX;
  int iVar1;
  undefined4 in_EDX;
  int iVar2;
  GraphicsProjectedPointEdxEax8 GVar3;
  
  FixedTransform_ApplyPoint
            (&g_GraphicsTransformOutputScratchVec3,&g_GraphicsTransformInputScratchVec3,
             &g_GraphicsTransformScratchMatrix3x4);
  if ((int)g_ProjectionScaleFixed < g_GraphicsTransformOutputScratchVec3.z) {
    GVar3 = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
    iVar1 = (int)GVar3 >> 0xc;
    iVar2 = (int)((longlong)GVar3 >> 0x2c);
    if (iVar1 < *param_1) {
      *param_1 = iVar1;
    }
    if (iVar2 < param_1[1]) {
      param_1[1] = iVar2;
    }
    if (param_1[2] < iVar1) {
      param_1[2] = iVar1;
    }
    if (param_1[3] < iVar2) {
      param_1[3] = iVar2;
    }
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x004BD800.
   Ownership: graphics/render/model.
   Purpose: Composes the model and view transforms, derives the camera-facing direction, and transforms the primary
   and auxiliary directions into model-local space.
   Cross-module calls: FixedTransform_Compose [core/math/fixed], FixedMath_VectorToAngles3Regs [core/math/fixed],
   FixedMath_WriteDirectionQ28 [core/math/fixed], FixedTransform_ApplyTransposeDirection [core/math/fixed].
*/
void ModelRender_PrepareViewDirections(ModelRuntimeNode *modelNodeRuntime)

{
  int extraout_EAX;
  int extraout_ECX;
  AngleTurn32 azimuthAngle;
  int extraout_EDX;
  AngleTurn32 elevationAngle;
  GraphicsFixedMatrix3x4 *transformA;
  
  transformA = &modelNodeRuntime->worldTransform;
  FixedTransform_Compose
            ((GraphicsFixedMatrix3x4 *)&g_ModelViewCompositeTransform,transformA,
             &g_ViewProjectionMatrixFixed);
  FixedMath_VectorToAngles3Regs
            (extraout_EDX - g_ViewOriginFixed.z,extraout_ECX - g_ViewOriginFixed.y,
             extraout_EAX - g_ViewOriginFixed.x);
  FixedMath_WriteDirectionQ28
            ((GraphicsFixedVec3 *)&g_ModelViewDirectionWorld,elevationAngle,azimuthAngle);
  FixedTransform_ApplyTransposeDirection
            ((GraphicsFixedVec3 *)&g_ModelViewDirectionLocal,transformA,
             (GraphicsFixedVec3 *)&g_ModelViewDirectionWorld);
  FixedTransform_ApplyTransposeDirection
            ((GraphicsFixedVec3 *)&g_ModelAuxiliaryForwardDirectionLocal,transformA,
             &g_AuxiliaryForwardDirectionFixed);
  return;
}
