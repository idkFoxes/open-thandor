/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/model/hierarchy.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/model/hierarchy.h>

/* Implementation ownership: world/model/hierarchy. */

/* Address: 0x004BD1F0.
   Ownership: world/model/hierarchy.
   Purpose: Derives a packed ARGB tint from model state flags and the global tint lookup table, then propagates the
   changed tint through the model runtime hierarchy.
   Local calls: ModelNodeRuntime_ApplyTintRecursive.
*/
void __thandor_void_preserve_eax_ecx_edx
ModelNodeRuntime_UpdateStateTintRecursive(ModelRuntimeNode *modelNodeRuntime)

{
  undefined1 uVar1;
  uint uVar2;
  int iVar3;
  PackedArgb32 tintArgb;
  int iVar4;
  
  iVar3 = 0xff;
  iVar4 = 0xff;
  uVar2 = modelNodeRuntime->runtimeFlags;
  if ((uVar2 & 4) == 0) {
    iVar3 = 0;
    iVar4 = 0;
    if ((uVar2 & 8) != 0) {
      iVar3 = 0xff;
      iVar4 = 0;
      if ((uVar2 & 0x10) == 0) {
        iVar3 = 0x87;
        iVar4 = 0xff;
      }
    }
  }
  if ((uVar2 & 0x1000) != 0) {
    iVar4 = 0;
  }
  uVar2 = modelNodeRuntime->tintArgb;
  uVar1 = *(undefined1 *)
           CONCAT22((short)((uint)(iVar3 + g_GraphicsIntensityClampTableBase) >> 0x10),
                    CONCAT11((char)(uVar2 >> 0x10),(char)(iVar3 + g_GraphicsIntensityClampTableBase)
                            ));
  tintArgb = CONCAT31(CONCAT21(CONCAT11(*(undefined1 *)
                                         CONCAT22((short)((uint)(iVar4 + 
                                                  g_GraphicsIntensityClampTableBase) >> 0x10),
                                                  CONCAT11((char)(uVar2 >> 0x18),
                                                           (char)(iVar4 + 
                                                  g_GraphicsIntensityClampTableBase))),uVar1),uVar1)
                      ,uVar1);
  if (tintArgb != uVar2 >> 0x10) {
    ModelNodeRuntime_ApplyTintRecursive(tintArgb,modelNodeRuntime);
  }
  return;
}


/* Address: 0x004BE360.
   Ownership: world/model/hierarchy.
   Purpose: Starts recursive model-transform rebuilding from the current node or its recorded parent/root pointer.
   Fire-chain root: rebuilds the world transform chain before kind-2 launch points are transformed. Role: Starts a
   full model-hierarchy transform rebuild at the root node. Inputs: Root ModelRuntimeNode after local animation,
   aim, recoil or translation changes. Outputs: Consistent world transforms for root and every child.
   Local calls: ModelNodeRuntime_ComposeChildTransformsRecursive.
*/
void __thandor_void_preserve_eax_ecx_edx
ModelNodeRuntime_RebuildTransformsFromRoot(ModelRuntimeNode *modelNodeRuntime)

{
  ModelRuntimeNode *parentNode;
  
  if (modelNodeRuntime->parentNode == (ModelRuntimeNode *)0x0) {
    ModelNodeRuntime_ComposeChildTransformsRecursive(modelNodeRuntime);
  }
  else {
    ModelNodeRuntime_ComposeChildTransformsRecursive(modelNodeRuntime->parentNode);
  }
  return;
}


/* Address: 0x0051D870.
   Ownership: world/model/hierarchy.
   Purpose: Recursively applies palette and texture-set state through a model hierarchy.
*/
void __thandor_void_preserve_eax_ecx_edx
ModelRuntimeHierarchy_SetPaletteAndTextureSetRecursive
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,ModelRuntimeNode *node)

{
  dword dVar1;
  
  if (node != (ModelRuntimeNode *)0x0) {
    (node->modelPayload).textureSet = textureSet;
    (node->modelPayload).paletteAsset = paletteAsset;
    for (dVar1 = node->childCount; dVar1 != 0; dVar1 = dVar1 - 1) {
      ModelRuntimeHierarchy_SetPaletteAndTextureSetRecursive
                (paletteAsset,textureSet,node->childNodes[0]);
      node = (ModelRuntimeNode *)&(node->common).nextNode;
    }
  }
  return;
}

/* Address: 0x0051DB80.
   Ownership: world/model/hierarchy.
   Purpose: Invokes the recursive faction-technology variant pass on the attached model runtime hierarchy. It is
   distinct from FrontendPlayerIndex_V306, PlayerRuntimeId, active-faction masks or codes, and PCK-backed
   ArmyAssetId, ModelDefinitionId, and TechnologyId domains.
   Local calls: ModelRuntimeHierarchy_ApplyFactionTechnologyVariantsRecursive.
*/
void __thandor_void_preserve_eax_ecx_edx
ModelRuntimeHierarchy_ApplyFactionTechnologyVariants
          (FactionRuntimeIndex factionIndex,ArmyRuntimeSlot *modelRuntimeHolder)

{
  ModelRuntimeHierarchy_ApplyFactionTechnologyVariantsRecursive
            (factionIndex,(int *)(modelRuntimeHolder->modelRuntimeOrSavedOffset).modelRuntime);
  return;
}


/* Address: 0x004BD310.
   Ownership: world/model/hierarchy.
   Purpose: Transforms every vertex in each model record, accumulates global minimum and maximum coordinates, then
   recursively processes every child model node. Typed parameters: p2 modelNode→ModelRuntimeNode *. Calling
   convention, complete VariableStorage serialization, function bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Cross-module calls: FixedTransform_ApplyPoint [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
ModelNodeRuntime_AccumulateTransformedBoundsRecursive(ModelRuntimeNode *modelNode)

{
  ModelResourceHitTestAndRenderView210 *pMVar1;
  dword dVar2;
  int iVar3;
  GraphicsFixedVec3 *point;
  byte *pbVar4;
  ModelPackedGeometryRecordCount MVar5;
  
  pMVar1 = (modelNode->modelPayload).modelResource;
  if (pMVar1->meshGroupCount != 0) {
    pbVar4 = pMVar1[1].reserved00_AF + 0x10;
    for (MVar5 = pMVar1->packedGeometryRecordCount; MVar5 != 0; MVar5 = MVar5 - 1) {
      point = (GraphicsFixedVec3 *)(pbVar4 + 0x20);
      for (iVar3 = *(int *)(pbVar4 + 8); iVar3 != 0; iVar3 = iVar3 + -1) {
        FixedTransform_ApplyPoint
                  ((GraphicsFixedVec3 *)&g_ModelBoundsTransformedPointX,point,
                   &modelNode->worldTransform);
        if (g_ModelBoundsTransformedPointX < g_ModelBoundsMinimumX) {
          g_ModelBoundsMinimumX = g_ModelBoundsTransformedPointX;
        }
        else if (g_ModelBoundsMaximumX < g_ModelBoundsTransformedPointX) {
          g_ModelBoundsMaximumX = g_ModelBoundsTransformedPointX;
        }
        if (g_ModelBoundsTransformedPointY < g_ModelBoundsMinimumY) {
          g_ModelBoundsMinimumY = g_ModelBoundsTransformedPointY;
        }
        else if (g_ModelBoundsMaximumY < g_ModelBoundsTransformedPointY) {
          g_ModelBoundsMaximumY = g_ModelBoundsTransformedPointY;
        }
        if (g_ModelBoundsTransformedPointZ < g_ModelBoundsMinimumZ) {
          g_ModelBoundsMinimumZ = g_ModelBoundsTransformedPointZ;
        }
        else if (g_ModelBoundsMaximumZ < g_ModelBoundsTransformedPointZ) {
          g_ModelBoundsMaximumZ = g_ModelBoundsTransformedPointZ;
        }
        point = (GraphicsFixedVec3 *)&point[5].y;
      }
      pbVar4 = pbVar4 + *(int *)pbVar4;
    }
  }
  iVar3 = 0;
  for (dVar2 = modelNode->childCount; dVar2 != 0; dVar2 = dVar2 - 1) {
    if (modelNode->childNodes[iVar3] != (ModelRuntimeNode *)0x0) {
      ModelNodeRuntime_AccumulateTransformedBoundsRecursive(modelNode->childNodes[iVar3]);
    }
    iVar3 = iVar3 + 1;
  }
  return;
}


/* Address: 0x004BD8D0.
   Ownership: world/model/hierarchy.
   Purpose: Builds the model rotation basis from the node position, current camera origin, stored orientation, and
   the calculated view-facing angle.
   Cross-module calls: FixedTransform_ApplyEulerRotationToVectorRegs [core/math/fixed], FixedMath_Atan2Angle16
   [core/math/fixed], FixedTransform_BuildRotationBasis [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
ModelNodeRuntime_BuildViewFacingRotation(ModelRuntimeNode *modelNodeRuntime)

{
  dword viewFacingAngle16;
  FixedVectorEaxEcxEdx12 FVar1;
  
  FVar1 = FixedTransform_ApplyEulerRotationToVectorRegs
                    ((modelNodeRuntime->worldTransform).translation.z - g_ViewOriginFixed.z,
                     (modelNodeRuntime->worldTransform).translation.y - g_ViewOriginFixed.y,
                     (modelNodeRuntime->worldTransform).translation.x - g_ViewOriginFixed.x,0,
                     (modelNodeRuntime->modelPayload).worldRotationAngle1,
                     (modelNodeRuntime->modelPayload).worldRotationAngle0 - 0x8000);
  viewFacingAngle16 = FixedMath_Atan2Angle16(FVar1.yQ12,FVar1.xQ12);
  FixedTransform_BuildRotationBasis
            (&modelNodeRuntime->worldTransform,viewFacingAngle16 + 0x4000 & 0xffff,
             (modelNodeRuntime->modelPayload).worldRotationAngle1,
             (modelNodeRuntime->modelPayload).worldRotationAngle0);
  return;
}


/* Address: 0x004BD950.
   Ownership: world/model/hierarchy.
   Purpose: Derives camera-relative vector angles and constructs the alternate billboard-style model rotation
   basis.
   Cross-module calls: FixedMath_VectorToAngles3Regs [core/math/fixed], FixedTransform_BuildRotationBasis
   [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
ModelNodeRuntime_BuildBillboardRotation(ModelRuntimeNode *modelNodeRuntime)

{
  uint angle0;
  FixedMathVectorAnglesRegs8 FVar1;
  
  FVar1 = FixedMath_VectorToAngles3Regs
                    ((modelNodeRuntime->worldTransform).translation.z - g_ViewOriginFixed.z,
                     (modelNodeRuntime->worldTransform).translation.y - g_ViewOriginFixed.y,
                     (modelNodeRuntime->worldTransform).translation.x - g_ViewOriginFixed.x);
  angle0 = FVar1.ecx + 0x8000 & 0xffff;
  FixedTransform_BuildRotationBasis(&modelNodeRuntime->worldTransform,angle0,-FVar1.edx,angle0);
  return;
}


/* Address: 0x004BE9D0.
   Ownership: world/model/hierarchy.
   Purpose: Recursively recomputes the node bounding radius at +0x54. The result is the maximum of the base model
   radius at model +0xD8 and each child translation-vector length plus that child radius.
   Cross-module calls: FixedMath_LengthVec3 [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
ModelNodeRuntime_RecomputeSubtreeBoundingRadius(ModelRuntimeNode *modelNodeRuntime)

{
  ModelRuntimeNode *modelNodeRuntime_00;
  dword dVar1;
  uint uVar2;
  dword dVar3;
  uint uVar4;
  ModelRuntimeNode *modelNode1;
  
  uVar4 = ((modelNodeRuntime->modelPayload).modelResource)->boundingRadiusQ12;
  modelNode1 = modelNodeRuntime;
  for (dVar3 = modelNodeRuntime->childCount; dVar3 != 0; dVar3 = dVar3 - 1) {
    modelNodeRuntime_00 = modelNode1->childNodes[0];
    if (modelNodeRuntime_00 != (ModelRuntimeNode *)0x0) {
      ModelNodeRuntime_RecomputeSubtreeBoundingRadius(modelNodeRuntime_00);
      dVar1 = FixedMath_LengthVec3
                        ((GraphicsFixedVec3 *)
                         &(modelNodeRuntime_00->modelPayload).localTranslationXQ12);
      uVar2 = dVar1 + modelNodeRuntime_00->subtreeBoundingRadiusQ12;
      if (uVar4 < uVar2) {
        uVar4 = uVar2;
      }
    }
    modelNode1 = (ModelRuntimeNode *)&(modelNode1->common).nextNode;
  }
  modelNodeRuntime->subtreeBoundingRadiusQ12 = uVar4;
  return;
}


/* Address: 0x004BEA30.
   Ownership: world/model/hierarchy.
   Purpose: Typed parameters: p2 intervalRadiusQ14→DepthIntervalRadius32_V343. Calling convention, complete
   VariableStorage serialization, function bytes, control flow, globals, locals, and executable data remain
   unchanged.
   Cross-module calls: DepthInterval_BuildBinMask [graphics/render/primitives].
*/
void __thandor_preserve_eax
ModelNodeRuntime_UpdateDepthBinMasks
          (DepthIntervalRadius32 intervalRadiusQ14,ModelRuntimeNode *modelNodeRuntime)

{
  DepthBinMask32 DVar1;
  GraphicsWorldCoordinateQ12 centerDepth;
  
  if (intervalRadiusQ14 < modelNodeRuntime->subtreeBoundingRadiusQ12) {
    intervalRadiusQ14 = modelNodeRuntime->subtreeBoundingRadiusQ12;
  }
  centerDepth = (modelNodeRuntime->worldTransform).translation.y;
  DVar1 = DepthInterval_BuildBinMask
                    (intervalRadiusQ14,(modelNodeRuntime->worldTransform).translation.x);
  modelNodeRuntime->depthBinMaskNear = DVar1;
  DVar1 = DepthInterval_BuildBinMask(intervalRadiusQ14,centerDepth);
  modelNodeRuntime->depthBinMaskFar = DVar1;
  return;
}


/* Address: 0x004BEB80.
   Ownership: world/model/hierarchy.
   Purpose: Transforms the local point at source +0x04 through the model-node fixed transform at +0x70 and returns
   the transformed vector through the engine register convention. Local Q12 point -> world via the node's fixed
   transform; the fire chain uses it on kind-2 launch records. Typed parameters: p1
   localPointRecord→ModelLocalPointRecordAddress32_V345. Calling convention, complete VariableStorage
   serialization, function bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: FixedTransform_ApplyPoint [core/math/fixed].
*/
ModelLocalPointRegs12
ModelNodeRuntime_TransformLocalPointRegs
          (ModelPackedPointRecord *localPointRecord,ModelRuntimeNode *modelNodeRuntime)

{
  ModelLocalPointRegs12 MVar1;
  
  FixedTransform_ApplyPoint
            ((GraphicsFixedVec3 *)&g_ModelTransformOutputX,&localPointRecord->localPosition,
             &modelNodeRuntime->worldTransform);
  MVar1.ecx = g_ModelTransformOutputY;
  MVar1.eax = g_ModelTransformOutputX;
  MVar1.edx = g_ModelTransformOutputZ;
  return MVar1;
}


/* Address: 0x004BEBC0.
   Ownership: world/model/hierarchy.
   Purpose: Builds and rotates a scaled direction using node orientation fields, converts the transformed vector
   back to angles, adds the node angle at +0x2C, and returns the wrapped relative direction angle. Typed
   parameters: p0 modelNodeRuntime→ModelRuntimeNode *. Calling convention, complete VariableStorage serialization,
   function bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: FixedTransform_RotateDirectionScaledRegs [core/math/fixed], FixedMath_VectorToAngles3Regs
   [core/math/fixed].
*/

ModelRelativeDirectionAnglesEaxEdx8 __thandor_eax_edx_cf_preserve_ecx
ModelNodeRuntime_ComputeRelativeDirectionAngle
          (ModelRuntimeNode *modelNodeRuntime,AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle)

{
  uint uVar1;
  FixedMathVectorAnglesRegs8 FVar2;
  FixedVectorEaxEcxEdx12 FVar3;
  
  uVar1 = -(modelNodeRuntime->modelPayload).worldRotationAngle2;
  FVar3 = FixedTransform_RotateDirectionScaledRegs
                    (0x1000,elevationAngle,azimuthAngle,uVar1 & 0xffff,
                     (modelNodeRuntime->modelPayload).worldRotationAngle1,
                     (modelNodeRuntime->modelPayload).worldRotationAngle0 + 0x8000 + uVar1 & 0xffff)
  ;
  FVar2 = FixedMath_VectorToAngles3Regs(FVar3.zQ12,FVar3.yQ12,FVar3.xQ12);
  return (ModelRelativeDirectionAnglesEaxEdx8)
         (CONCAT44(FVar2.edx,FVar2.ecx + (modelNodeRuntime->modelPayload).localRotationAngle2) &
         0xffffffff0000ffff);
}


/* Address: 0x0050A7A0.
   Ownership: world/model/hierarchy.
   Purpose: CF clear returns EAX distance metric; CF set reports miss.
   Cross-module calls: FixedTransform_Compose [core/math/fixed], FixedTransform_ApplyPoint [core/math/fixed],
   Graphics_ProjectViewPoint [graphics/core/runtime], GraphicsProjectedPoint_IsInsideTriangleCf
   [graphics/render/projection], FixedMath_Length3 [core/math/fixed].
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ModelRuntimeNode_HitTestProjectedBoundsAndChildrenCf
          (int pointerY,int pointerX,ModelRuntimeNode *modelNode,
          FrontendModelPointerContextRuntimeState118 *context)

{
  ModelResourceHitTestAndRenderView210 *pMVar1;
  GraphicsWorldCoordinateQ12 GVar2;
  ModelRuntimeNode *modelNode_00;
  StatusValueEaxCf5 SVar3;
  byte bVar4;
  GraphicsFixedMatrix3x4 *transformA;
  dword dVar5;
  int iVar6;
  bool bVar7;
  GraphicsProjectedPointPair GVar8;
  StatusValueEaxCf5 SVar9;
  StatusValueEaxCf5 SVar10;
  
  pMVar1 = (modelNode->modelPayload).modelResource;
  transformA = &modelNode->worldTransform;
  if ((pMVar1->boundingRadiusQ12 != 0) &&
     ((pMVar1->hitTestFlags20C & MODEL_RESOURCE_DISABLE_PROJECTED_HIT_TEST) == 0)) {
    FixedTransform_Compose
              (&g_GraphicsTransformScratchMatrix3x4,transformA,&g_ViewProjectionMatrixFixed);
    g_GraphicsTransformInputScratchVec3.x = pMVar1->localBoundsX0Q12;
    GVar2 = pMVar1->localBoundsX1Q12;
    g_GraphicsTransformInputScratchVec3.y = pMVar1->localBoundsY0Q12;
    g_GraphicsTransformInputScratchVec3.z = pMVar1->localBoundsZ0Q12;
    FixedTransform_ApplyPoint
              (&g_GraphicsTransformOutputScratchVec3,&g_GraphicsTransformInputScratchVec3,
               &g_GraphicsTransformScratchMatrix3x4);
    bVar7 = (int)g_ProjectionScaleFixed <= g_GraphicsTransformOutputScratchVec3.z;
    if (bVar7) {
      GVar8 = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
      g_ModelProjectedBoundsCornerScratch8[0].x = GVar8.projectedX >> 0xc;
      g_ModelProjectedBoundsCornerScratch8[0].y = GVar8.projectedY >> 0xc;
    }
    bVar4 = !bVar7;
    g_GraphicsTransformInputScratchVec3.x = GVar2;
    FixedTransform_ApplyPoint
              (&g_GraphicsTransformOutputScratchVec3,&g_GraphicsTransformInputScratchVec3,
               &g_GraphicsTransformScratchMatrix3x4);
    if (g_GraphicsTransformOutputScratchVec3.z < (int)g_ProjectionScaleFixed) {
      bVar4 = bVar4 | 2;
    }
    else {
      GVar8 = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
      g_ModelProjectedBoundsCornerScratch8[1].x = GVar8.projectedX >> 0xc;
      g_ModelProjectedBoundsCornerScratch8[1].y = GVar8.projectedY >> 0xc;
    }
    g_GraphicsTransformInputScratchVec3.x = pMVar1->localBoundsX0Q12;
    g_GraphicsTransformInputScratchVec3.y = pMVar1->localBoundsY1Q12;
    FixedTransform_ApplyPoint
              (&g_GraphicsTransformOutputScratchVec3,&g_GraphicsTransformInputScratchVec3,
               &g_GraphicsTransformScratchMatrix3x4);
    if (g_GraphicsTransformOutputScratchVec3.z < (int)g_ProjectionScaleFixed) {
      bVar4 = bVar4 | 4;
    }
    else {
      GVar8 = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
      g_ModelProjectedBoundsCornerScratch8[2].x = GVar8.projectedX >> 0xc;
      g_ModelProjectedBoundsCornerScratch8[2].y = GVar8.projectedY >> 0xc;
    }
    g_GraphicsTransformInputScratchVec3.x = GVar2;
    FixedTransform_ApplyPoint
              (&g_GraphicsTransformOutputScratchVec3,&g_GraphicsTransformInputScratchVec3,
               &g_GraphicsTransformScratchMatrix3x4);
    if (g_GraphicsTransformOutputScratchVec3.z < (int)g_ProjectionScaleFixed) {
      bVar4 = bVar4 | 8;
    }
    else {
      GVar8 = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
      g_ModelProjectedBoundsCornerScratch8[3].x = GVar8.projectedX >> 0xc;
      g_ModelProjectedBoundsCornerScratch8[3].y = GVar8.projectedY >> 0xc;
    }
    g_GraphicsTransformInputScratchVec3.x = pMVar1->localBoundsX0Q12;
    g_GraphicsTransformInputScratchVec3.y = pMVar1->localBoundsY0Q12;
    g_GraphicsTransformInputScratchVec3.z = pMVar1->localBoundsZ1Q12;
    FixedTransform_ApplyPoint
              (&g_GraphicsTransformOutputScratchVec3,&g_GraphicsTransformInputScratchVec3,
               &g_GraphicsTransformScratchMatrix3x4);
    if (g_GraphicsTransformOutputScratchVec3.z < (int)g_ProjectionScaleFixed) {
      bVar4 = bVar4 | 0x10;
    }
    else {
      GVar8 = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
      g_ModelProjectedBoundsCornerScratch8[4].x = GVar8.projectedX >> 0xc;
      g_ModelProjectedBoundsCornerScratch8[4].y = GVar8.projectedY >> 0xc;
    }
    g_GraphicsTransformInputScratchVec3.x = GVar2;
    FixedTransform_ApplyPoint
              (&g_GraphicsTransformOutputScratchVec3,&g_GraphicsTransformInputScratchVec3,
               &g_GraphicsTransformScratchMatrix3x4);
    if (g_GraphicsTransformOutputScratchVec3.z < (int)g_ProjectionScaleFixed) {
      bVar4 = bVar4 | 0x20;
    }
    else {
      GVar8 = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
      g_ModelProjectedBoundsCornerScratch8[5].x = GVar8.projectedX >> 0xc;
      g_ModelProjectedBoundsCornerScratch8[5].y = GVar8.projectedY >> 0xc;
    }
    transformA = (GraphicsFixedMatrix3x4 *)pMVar1->localBoundsX0Q12;
    g_GraphicsTransformInputScratchVec3.y = pMVar1->localBoundsY1Q12;
    g_GraphicsTransformInputScratchVec3.x = (GraphicsWorldCoordinateQ12)transformA;
    FixedTransform_ApplyPoint
              (&g_GraphicsTransformOutputScratchVec3,&g_GraphicsTransformInputScratchVec3,
               &g_GraphicsTransformScratchMatrix3x4);
    if (g_GraphicsTransformOutputScratchVec3.z < (int)g_ProjectionScaleFixed) {
      bVar4 = bVar4 | 0x40;
    }
    else {
      GVar8 = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
      transformA = (GraphicsFixedMatrix3x4 *)(GVar8.projectedX >> 0xc);
      g_ModelProjectedBoundsCornerScratch8[6].y = GVar8.projectedY >> 0xc;
      g_ModelProjectedBoundsCornerScratch8[6].x = (GraphicsProjectedCoordinate)transformA;
    }
    g_GraphicsTransformInputScratchVec3.x = GVar2;
    FixedTransform_ApplyPoint
              (&g_GraphicsTransformOutputScratchVec3,&g_GraphicsTransformInputScratchVec3,
               &g_GraphicsTransformScratchMatrix3x4);
    if (g_GraphicsTransformOutputScratchVec3.z < (int)g_ProjectionScaleFixed) {
      bVar4 = bVar4 | 0x80;
    }
    else {
      GVar8 = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
      transformA = (GraphicsFixedMatrix3x4 *)(GVar8.projectedX >> 0xc);
      g_ModelProjectedBoundsCornerScratch8[7].y = GVar8.projectedY >> 0xc;
      g_ModelProjectedBoundsCornerScratch8[7].x = (GraphicsProjectedCoordinate)transformA;
    }
    if (((((((bVar4 & 7) == 0) &&
           (bVar7 = GraphicsProjectedPoint_IsInsideTriangleCf
                              (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 2,
                               g_ModelProjectedBoundsCornerScratch8 + 1,
                               g_ModelProjectedBoundsCornerScratch8), bVar7)) ||
          (((bVar4 & 0xe) == 0 &&
           (bVar7 = GraphicsProjectedPoint_IsInsideTriangleCf
                              (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 2,
                               g_ModelProjectedBoundsCornerScratch8 + 1,
                               g_ModelProjectedBoundsCornerScratch8 + 3), bVar7)))) ||
         (((bVar4 & 0x70) == 0 &&
          (bVar7 = GraphicsProjectedPoint_IsInsideTriangleCf
                             (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 6,
                              g_ModelProjectedBoundsCornerScratch8 + 5,
                              g_ModelProjectedBoundsCornerScratch8 + 4), bVar7)))) ||
        (((((bVar4 & 0xe0) == 0 &&
           (bVar7 = GraphicsProjectedPoint_IsInsideTriangleCf
                              (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 6,
                               g_ModelProjectedBoundsCornerScratch8 + 5,
                               g_ModelProjectedBoundsCornerScratch8 + 7), bVar7)) ||
          ((((bVar4 & 0x15) == 0 &&
            (bVar7 = GraphicsProjectedPoint_IsInsideTriangleCf
                               (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 2,
                                g_ModelProjectedBoundsCornerScratch8 + 4,
                                g_ModelProjectedBoundsCornerScratch8), bVar7)) ||
           (((bVar4 & 0x54) == 0 &&
            (bVar7 = GraphicsProjectedPoint_IsInsideTriangleCf
                               (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 2,
                                g_ModelProjectedBoundsCornerScratch8 + 6,
                                g_ModelProjectedBoundsCornerScratch8 + 4), bVar7)))))) ||
         ((((bVar4 & 0x2a) == 0 &&
           (bVar7 = GraphicsProjectedPoint_IsInsideTriangleCf
                              (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 3,
                               g_ModelProjectedBoundsCornerScratch8 + 5,
                               g_ModelProjectedBoundsCornerScratch8 + 1), bVar7)) ||
          (((bVar4 & 0xa8) == 0 &&
           (bVar7 = GraphicsProjectedPoint_IsInsideTriangleCf
                              (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 3,
                               g_ModelProjectedBoundsCornerScratch8 + 7,
                               g_ModelProjectedBoundsCornerScratch8 + 5), bVar7)))))))) ||
       (((((bVar4 & 0x13) == 0 &&
          (bVar7 = GraphicsProjectedPoint_IsInsideTriangleCf
                             (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 4,
                              g_ModelProjectedBoundsCornerScratch8 + 1,
                              g_ModelProjectedBoundsCornerScratch8), bVar7)) ||
         (((bVar4 & 0x32) == 0 &&
          (bVar7 = GraphicsProjectedPoint_IsInsideTriangleCf
                             (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 1,
                              g_ModelProjectedBoundsCornerScratch8 + 5,
                              g_ModelProjectedBoundsCornerScratch8 + 4), bVar7)))) ||
        ((((bVar4 & 0x4c) == 0 &&
          (bVar7 = GraphicsProjectedPoint_IsInsideTriangleCf
                             (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 3,
                              g_ModelProjectedBoundsCornerScratch8 + 6,
                              g_ModelProjectedBoundsCornerScratch8 + 2), bVar7)) ||
         (((bVar4 & 200) == 0 &&
          (bVar7 = GraphicsProjectedPoint_IsInsideTriangleCf
                             (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 3,
                              g_ModelProjectedBoundsCornerScratch8 + 6,
                              g_ModelProjectedBoundsCornerScratch8 + 7), bVar7)))))))) {
      if ((context->contextFlags & 0x80000) != 0) {
        SVar9.valueOrError =
             FixedMath_Length3(((pMVar1->localBoundsZ0Q12 + pMVar1->localBoundsZ1Q12 >> 1) +
                               (modelNode->worldTransform).translation.z) -
                               context->hitReferenceWorldZQ12,
                               ((pMVar1->localBoundsY0Q12 + pMVar1->localBoundsY1Q12 >> 1) +
                               (modelNode->worldTransform).translation.y) -
                               context->hitReferenceWorldYQ12,
                               ((pMVar1->localBoundsX0Q12 + pMVar1->localBoundsX1Q12 >> 1) +
                               (modelNode->worldTransform).translation.x) -
                               context->hitReferenceWorldXQ12);
        SVar9.carry = false;
        return SVar9;
      }
      SVar10.valueOrError =
           FixedMath_Length3((modelNode->worldTransform).translation.z -
                             context->hitReferenceWorldZQ12,
                             (modelNode->worldTransform).translation.y -
                             context->hitReferenceWorldYQ12,
                             (modelNode->worldTransform).translation.x -
                             context->hitReferenceWorldXQ12);
      SVar10.carry = false;
      return SVar10;
    }
  }
  dVar5 = modelNode->childCount;
  iVar6 = 0;
  do {
    if (dVar5 == 0) {
      SVar3.carry = true;
      SVar3.valueOrError = (dword)transformA;
      return SVar3;
    }
    modelNode_00 = *(ModelRuntimeNode **)((int)modelNode->childNodes + iVar6);
    if (modelNode_00 != (ModelRuntimeNode *)0x0) {
      SVar10 = ModelRuntimeNode_HitTestProjectedBoundsAndChildrenCf
                         (pointerY,pointerX,modelNode_00,context);
      transformA = (GraphicsFixedMatrix3x4 *)SVar10.valueOrError;
      if (!SVar10.carry) {
        return SVar10;
      }
    }
    iVar6 = iVar6 + 4;
    dVar5 = dVar5 - 1;
  } while( true );
}


/* Address: 0x0050B1D0.
   Ownership: world/model/hierarchy.
   Purpose: Returns nearest Q12 ray distance with CF set. EDX is the nearest ModelRuntimeNode side channel on
   success; CF clear returns 0x7fffffff.
   Cross-module calls: FixedTransform_BuildRotationBasis [core/math/fixed], FixedTransform_ApplyPoint
   [core/math/fixed], ModelMesh_IntersectTriangleRayDistanceCf [assets/model/definitions].
*/
ModelRaycastNearestHitEaxEdxCf9 __thandor_eax_edx_cf_preserve_ecx
ModelNodeRuntime_RaycastHierarchyNearestCf(ModelRuntimeNode *modelNodeRuntime)

{
  GraphicsFixedVec3 **ppGVar1;
  ModelResourceHitTestAndRenderView210 *pMVar2;
  longlong lVar3;
  longlong lVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  GraphicsFixedVec3 *pGVar8;
  dword dVar9;
  int iVar10;
  ModelRaycastNearestNodeOrScratch4 edxCarrier;
  ModelPackedGeometryRecordCount MVar11;
  ModelMeshGroupCount MVar12;
  int iVar13;
  ModelMeshGroupRelativeOffset *pMVar14;
  ModelRaycastTriangleDescriptor *triangle;
  ModelRuntimeNode *nearestModelNode;
  TerrainDistanceEaxCf5 TVar15;
  ModelRaycastNearestHitEaxEdxCf9 MVar16;
  ModelRaycastNearestHitEaxEdxCf9 MVar17;
  
  iVar6 = (modelNodeRuntime->worldTransform).translation.x - g_ModelRaycastOriginX;
  iVar7 = (modelNodeRuntime->worldTransform).translation.y - g_ModelRaycastOriginY;
  iVar10 = (modelNodeRuntime->worldTransform).translation.z - g_ModelRaycastOriginZ;
  iVar13 = modelNodeRuntime->subtreeBoundingRadiusQ12;
  lVar3 = (longlong)iVar7 * (longlong)g_ModelRaycastWorldDirectionYQ28 +
          (longlong)iVar6 * (longlong)g_ModelRaycastWorldDirectionXQ28 +
          (longlong)iVar10 * (longlong)g_ModelRaycastWorldDirectionZQ28;
  edxCarrier.scratchSigned = (int)((ulonglong)lVar3 >> 0x20) << 4 | (uint)lVar3 >> 0x1c;
  if ((-iVar13 <= edxCarrier.scratchSigned) &&
     (edxCarrier.scratchSigned < g_ModelRaycastMaximumDistance + iVar13)) {
    lVar3 = (longlong)edxCarrier.scratchSigned;
    lVar4 = (longlong)edxCarrier.scratchSigned;
    edxCarrier.scratchSigned = (int)((ulonglong)((longlong)iVar6 * (longlong)iVar6) >> 0x20);
    lVar3 = ((longlong)iVar13 * (longlong)iVar13 + lVar3 * lVar4) -
            (longlong)iVar6 * (longlong)iVar6;
    if (-1 < lVar3) {
      edxCarrier.scratchSigned = (int)((ulonglong)((longlong)iVar7 * (longlong)iVar7) >> 0x20);
      lVar3 = lVar3 - (longlong)iVar7 * (longlong)iVar7;
      if ((-1 < lVar3) &&
         (edxCarrier.scratchSigned = (int)((ulonglong)((longlong)iVar10 * (longlong)iVar10) >> 0x20)
         , -1 < (int)(((int)((ulonglong)lVar3 >> 0x20) - edxCarrier.scratchSigned) -
                     (uint)((uint)lVar3 < (uint)((longlong)iVar10 * (longlong)iVar10))))) {
        uVar5 = -(modelNodeRuntime->modelPayload).worldRotationAngle2;
        FixedTransform_BuildRotationBasis
                  (&g_GraphicsTransformScratchMatrix3x4,uVar5 & 0xffff,
                   (modelNodeRuntime->modelPayload).worldRotationAngle1,
                   (modelNodeRuntime->modelPayload).worldRotationAngle0 + 0x8000 + uVar5 & 0xffff);
        pMVar2 = (modelNodeRuntime->modelPayload).modelResource;
        g_GraphicsTransformScratchMatrix3x4.translation.x = 0;
        g_GraphicsTransformScratchMatrix3x4.translation.y = 0;
        g_GraphicsTransformScratchMatrix3x4.translation.z = 0;
        pMVar14 = &pMVar2->firstMeshGroupRelativeOffset;
        iVar13 = (modelNodeRuntime->worldTransform).translation.x;
        iVar6 = (modelNodeRuntime->worldTransform).translation.y;
        MVar12 = pMVar2->meshGroupCount;
        iVar7 = (modelNodeRuntime->worldTransform).translation.z;
        if ((*(int *)pMVar2->reservedEC_1FF == 0) || (MVar12 = MVar12 - 1, MVar12 != 0)) {
          while (MVar12 = MVar12 - 1, MVar12 != 0) {
            pMVar14 = (ModelMeshGroupRelativeOffset *)((int)pMVar14 + *pMVar14);
          }
        }
        g_ModelRaycastOriginX = g_ModelRaycastOriginX - iVar13;
        g_ModelRaycastOriginY = g_ModelRaycastOriginY - iVar6;
        g_ModelRaycastOriginZ = g_ModelRaycastOriginZ - iVar7;
        FixedTransform_ApplyPoint
                  ((GraphicsFixedVec3 *)&g_ModelRaycastLocalOriginX,
                   (GraphicsFixedVec3 *)&g_ModelRaycastOriginX,&g_GraphicsTransformScratchMatrix3x4)
        ;
        g_ModelRaycastOriginX = g_ModelRaycastOriginX + iVar13;
        g_ModelRaycastOriginY = g_ModelRaycastOriginY + iVar6;
        g_ModelRaycastOriginZ = g_ModelRaycastOriginZ + iVar7;
        FixedTransform_ApplyPoint
                  ((GraphicsFixedVec3 *)&g_ModelRaycastLocalDirectionXQ28,
                   (GraphicsFixedVec3 *)&g_ModelRaycastWorldDirectionXQ28,
                   &g_GraphicsTransformScratchMatrix3x4);
        triangle = (ModelRaycastTriangleDescriptor *)(pMVar14 + 8);
        iVar13 = 0x7fffffff;
        for (MVar11 = pMVar14[1]; MVar11 != 0; MVar11 = MVar11 - 1) {
          ppGVar1 = &triangle->vertex1;
          triangle = (ModelRaycastTriangleDescriptor *)
                     (triangle[*(int *)(triangle->reservedVertex0Metadata04_0B + 4)].
                      reservedVertex2Metadata1C_23 + 4);
          for (pGVar8 = *ppGVar1; pGVar8 != (GraphicsFixedVec3 *)0x0;
              pGVar8 = (GraphicsFixedVec3 *)((int)&pGVar8[-1].z + 3)) {
            TVar15 = ModelMesh_IntersectTriangleRayDistanceCf(triangle);
            if ((TVar15.carry) && (TVar15.distanceQ12 <= iVar13)) {
              iVar13 = TVar15.distanceQ12;
            }
            triangle = triangle + 1;
          }
        }
        edxCarrier.nearestModelNode = (ModelRuntimeNode *)0x0;
        nearestModelNode = modelNodeRuntime;
        for (dVar9 = modelNodeRuntime->childCount; dVar9 != 0; dVar9 = dVar9 - 1) {
          if (modelNodeRuntime->childNodes[dVar9 - 1] != (ModelRuntimeNode *)0x0) {
            MVar16 = ModelNodeRuntime_RaycastHierarchyNearestCf
                               (modelNodeRuntime->childNodes[dVar9 - 1]);
            edxCarrier = MVar16.edxCarrier;
            if ((MVar16.carry) && (MVar16.nearestDistanceQ12 < iVar13)) {
              iVar13 = MVar16.nearestDistanceQ12;
              nearestModelNode = edxCarrier.nearestModelNode;
            }
          }
        }
        if (iVar13 != 0x7fffffff) {
          MVar16.edxCarrier.nearestModelNode = nearestModelNode;
          MVar16.nearestDistanceQ12 = iVar13;
          MVar16.carry = true;
          return MVar16;
        }
      }
    }
  }
  MVar17.edxCarrier.nearestModelNode = edxCarrier.nearestModelNode;
  MVar17.nearestDistanceQ12 = 0x7fffffff;
  MVar17.carry = false;
  return MVar17;
}


/* Address: 0x0051B650.
   Ownership: world/model/hierarchy.
   Purpose: Walks the child-definition list, resolves each faction-unlocked linked model identifier, instantiates
   the matching runtime child, and recursively builds its descendants, aborting through carry on failure. Role:
   Recursively instantiates MDL-linked child model definitions. Inputs: Parent ModelRuntimeNode, linked-model
   descriptors and runtime selection context. Outputs: Attached child ModelRuntimeNode trees; deferred links may be
   repaired later. Edges: Calls model selection/creation, ModelRuntimePool_RepairDeferredChild and itself
   recursively.
   Cross-module calls: ModelDefinition_SelectFactionUnlockedLinkedIdCf [assets/model/definitions],
   ModelRuntimePool_RepairDeferredChild [world/model/runtime].
*/
bool __thandor_cf_preserve_eax_ecx_edx
ModelNodeRuntime_InstantiateLinkedChildrenRecursiveCf
          (FactionRuntimeIndex factionIndex,GraphicsPaletteAsset *paletteAsset,
          GraphicsTextureSet *textureSet,ModelRuntimeSlot *modelRuntimeSlot,
          ModelDefinitionHierarchyNodeAddress32 definitionNode,WorldRuntimeContext *worldRuntime)

{
  ModelLinkedDefinitionListAddress32 linkedDefinitionList;
  PckModelDefinitionIdCatalog childDefinitionId;
  int iVar1;
  ModelRuntimeAttachmentIndex attachmentIndex_00;
  ModelRuntimeAttachmentIndex attachmentIndex;
  bool bVar2;
  ModelNodeCreateEaxCf5 MVar3;
  
  iVar1 = *(int *)(definitionNode + 8);
  if (iVar1 != 0) {
    attachmentIndex_00 = 0;
    do {
      linkedDefinitionList =
           *(ModelLinkedDefinitionListAddress32 *)(definitionNode + 0xc + attachmentIndex_00 * 4);
      childDefinitionId =
           ModelDefinition_SelectFactionUnlockedLinkedIdCf(factionIndex,linkedDefinitionList);
      MVar3 = ModelRuntimePool_RepairDeferredChild
                        (paletteAsset,textureSet,attachmentIndex_00,childDefinitionId,
                         modelRuntimeSlot,worldRuntime);
      if (MVar3.carry) {
        return true;
      }
      bVar2 = ModelNodeRuntime_InstantiateLinkedChildrenRecursiveCf
                        (factionIndex,paletteAsset,textureSet,(ModelRuntimeSlot *)MVar3.modelNode,
                         linkedDefinitionList,worldRuntime);
      if (bVar2) {
        return true;
      }
      attachmentIndex_00 = attachmentIndex_00 + 1;
      iVar1 = iVar1 + -1;
    } while (iVar1 != 0);
  }
  return false;
}


/* Address: 0x0051BEC0.
   Ownership: world/model/hierarchy.
   Purpose: Stores the two command-target values in one model runtime node and recursively propagates them through
   every child hierarchy entry. Typed parameters: p4 modelNode→ModelRuntimeNode *. Calling convention, complete
   VariableStorage serialization, function bytes, control flow, globals, locals, and executable data remain
   unchanged. Typed parameters: p2 commandTarget0→ModelCommandTarget0_V344, p3
   commandTarget1→ModelCommandTarget1_V344. Calling convention, complete VariableStorage serialization, function
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
ModelRuntimeHierarchy_SetCommandTargetRecursive
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,
          ModelRuntimeNode *modelNode)

{
  dword dVar1;
  
  dVar1 = modelNode->childCount;
  (modelNode->modelPayload).textureSet = textureSet;
  (modelNode->modelPayload).paletteAsset = paletteAsset;
  for (; dVar1 != 0; dVar1 = dVar1 - 1) {
    if (modelNode->childNodes[0] != (ModelRuntimeNode *)0x0) {
      ModelRuntimeHierarchy_SetCommandTargetRecursive
                (paletteAsset,textureSet,modelNode->childNodes[0]);
    }
    modelNode = (ModelRuntimeNode *)&(modelNode->common).nextNode;
  }
  return;
}


/* Address: 0x0051BF30.
   Ownership: world/model/hierarchy.
   Purpose: Typed parameters: p2 targetRuntimeId→RuntimeToken. Nearby but non-identical semantic domains were
   explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals, locals, and
   executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
ModelRuntimeHierarchy_ClearMatchingTargetRecursive(RuntimeToken targetRuntimeId,int *modelRuntime)

{
  int iVar1;
  
  if (modelRuntime != (int *)0x0) {
    iVar1 = modelRuntime[3];
    if ((*(int *)(*modelRuntime + 0x4c) == 0xd) && (targetRuntimeId == modelRuntime[0x1b])) {
      modelRuntime[0x1b] = 0;
    }
    for (; iVar1 != 0; iVar1 = iVar1 + -1) {
      ModelRuntimeHierarchy_ClearMatchingTargetRecursive(targetRuntimeId,(int *)modelRuntime[0x50]);
      modelRuntime = modelRuntime + 8;
    }
  }
  return;
}


/* Address: 0x0051C100.
   Ownership: world/model/hierarchy.
   Purpose: Walks the model runtime hierarchy and applies flag mask 0x418 to each node whose existing runtime flags
   do not contain bit 0x08.
*/
void __thandor_void_preserve_eax_ecx_edx
ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive
          (WorldRuntimeContext *contextArg,int *modelRuntime)

{
  int iVar1;
  int unaff_EBP;
  int unaff_ESI;
  int iVar2;
  int iVar3;
  
  iVar1 = 0;
  iVar2 = *modelRuntime;
  do {
    if ((*(uint *)(iVar2 + 0xec) & 8) == 0) {
      *(uint *)(iVar2 + 0xec) = *(uint *)(iVar2 + 0xec) | 0x418;
    }
    iVar1 = iVar1 + 1;
    iVar3 = unaff_ESI;
    do {
      while (unaff_EBP == 0) {
        iVar1 = iVar1 + -1;
        if (iVar1 == 0) {
          return;
        }
      }
      unaff_EBP = unaff_EBP + -1;
      unaff_ESI = iVar3 + 0x20;
      iVar2 = *(int *)(iVar3 + 0x140);
      iVar3 = unaff_ESI;
    } while (iVar2 == 0);
  } while( true );
}


/* Address: 0x0051C1F0.
   Ownership: world/model/hierarchy.
   Purpose: Traverses the model runtime hierarchy and sums the signed dword stored at runtime-node offset 0x3C.
*/
int __thandor_eax_preserve_ecx_edx ModelRuntimeHierarchy_SumMetric3C(int *modelRuntimeRoot)

{
  int iVar1;
  int iVar2;
  int unaff_EBP;
  int unaff_EDI;
  int iVar3;
  int iVar4;
  
  iVar2 = 0;
  iVar1 = 0;
  iVar3 = *modelRuntimeRoot;
  do {
    iVar1 = iVar1 + *(int *)(iVar3 + 0x3c);
    iVar2 = iVar2 + 1;
    iVar4 = unaff_EDI;
    do {
      while (unaff_EBP == 0) {
        iVar2 = iVar2 + -1;
        if (iVar2 == 0) {
          return iVar1;
        }
      }
      unaff_EBP = unaff_EBP + -1;
      unaff_EDI = iVar4 + 0x20;
      iVar3 = *(int *)(iVar4 + 0x140);
      iVar4 = unaff_EDI;
    } while (iVar3 == 0);
  } while( true );
}


/* Address: 0x00528C20.
   Ownership: world/model/hierarchy.
   Purpose: Recursively walks model-definition children whose low-nibble mode is zero, matches type-zero and type-
   one attachment descriptors by child channel index, and stores up to six descriptor pointers in the verified
   runtime attachment list. Two stack arguments are authoritative from RET 0x08. The EDX:EAX result remains a
   nominal register-pair domain rather than a hidden structure return. Walks the hierarchy collecting NULL-child
   (empty-stem) attach records into the 6-slot attachments140[] with rotation angles from the CHILD DEFINITION
   record +8/+0xC/+0x10 — not from the transform record (W2 closure). Role: Traverses the runtime model hierarchy
   and collects serialized SPR attachment descriptors.
*/
ModelRuntimeSlot * __thandor_eax_preserve_ecx_edx
ModelRuntimeHierarchy_CollectAttachmentDescriptorsRecursive
          (ModelRuntimeSlot *modelRuntimeContinuityEdi,ModelRuntimeSlot *modelRuntime,
          MdlSerializedNodeHeader38 *definitionNode)

{
  uint uVar1;
  uint attachmentKind;
  int iVar2;
  MdlChildCount childCountRemaining;
  uint childIndex;
  ModelAttachmentTransformRecord *attachmentTransformCursor;
  ModelRuntimeAttachmentCollectionRegisterPair recursiveCollectionResult;
  AssetRecordByteCount definitionAssetBase;
  
  if ((definitionNode->nodeFlags & 0xf) != 0) {
    return (ModelRuntimeSlot *)0x0;
  }
  childCountRemaining = definitionNode->childCount;
  definitionAssetBase = (definitionNode->spriteAssetReference).savedId;
  childIndex = 0;
  do {
    if (childCountRemaining == 0) {
      return modelRuntimeContinuityEdi;
    }
    attachmentTransformCursor =
         (ModelAttachmentTransformRecord *)
         (definitionAssetBase + *(int *)(definitionAssetBase + 0xe4));
    for (iVar2 = *(int *)(definitionAssetBase + 0xe8); iVar2 != 0; iVar2 = iVar2 + -1) {
      attachmentKind = attachmentTransformCursor->packedKindAndSelector & 0xf;
      if (((attachmentKind == 0) || (attachmentKind == 1)) &&
         (childIndex == attachmentTransformCursor->packedKindAndSelector >> 4)) {
        recursiveCollectionResult._0_4_ =
             ModelRuntimeHierarchy_CollectAttachmentDescriptorsRecursive
                       (modelRuntimeContinuityEdi,modelRuntime,
                        (MdlSerializedNodeHeader38 *)
                        definitionNode->childSerializedOffsets[childIndex]);
        if (((ModelRuntimeSlot *)recursiveCollectionResult == (ModelRuntimeSlot *)0x0) &&
           (uVar1 = modelRuntime->attachmentCount0C, uVar1 < 6)) {
          modelRuntime->attachmentCount0C = modelRuntime->attachmentCount0C + 1;
          modelRuntime->attachments140[uVar1].sourceTransform04 = attachmentTransformCursor;
        }
        goto ModelRuntimeHierarchy_CollectAttachmentDescriptors_AdvanceAfterChildResolution;
      }
      attachmentTransformCursor = attachmentTransformCursor + 1;
    }
    childIndex = childIndex - 1;
ModelRuntimeHierarchy_CollectAttachmentDescriptors_AdvanceAfterChildResolution:
    childIndex = childIndex + 1;
    childCountRemaining = childCountRemaining - 1;
  } while( true );
}


/* Address: 0x00528E90.
   Ownership: world/model/hierarchy.
   Purpose: Allocates and initializes a world-object model node, recursively creates child hierarchy nodes from
   definition records, links their transforms, and records unresolved children for later repair. Role: Creates one
   ModelRuntimeNode hierarchy from a model definition and its visual-node tree. Inputs: ModelDefinition visual
   nodes, SpriteAsset references, local rotations/translations and child offsets. Outputs: Runtime nodes with
   parent/child links and copied local model state. Edges: Called by
   ModelRuntimePool_CreateInstanceByDefinitionIdCf.
   Cross-module calls: WorldObjectArray_AllocateFreeRecordCf [world/runtime/core].
*/
ModelNodeCreateEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ModelNodeRuntime_CreateHierarchyRecursiveCf
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,
          ModelRuntimeSlot *modelRuntime,MdlSerializedNodeHeader38 *definitionNode,
          WorldRuntimeContext *worldRuntime)

{
  AngleTurn32 AVar1;
  AngleTurn32 AVar2;
  ArmyRuntimeSlot *pAVar3;
  ModelResourceHitTestAndRenderView210 *pMVar4;
  Q12 QVar5;
  Q12 QVar6;
  SerializedRelativeByteOffset SVar7;
  ModelRuntimeNode *modelNode1;
  uint uVar8;
  ModelRuntimeNode *modelNode2;
  ModelPackedLookupTableEntryCount MVar9;
  dword dVar10;
  uint uVar11;
  ModelAttachmentTransformRecord *pMVar12;
  WorldObjectRecordEaxCf5 WVar13;
  ModelNodeCreateEaxCf5 MVar14;
  ModelNodeCreateEaxCf5 MVar15;
  
  if ((definitionNode->nodeFlags & 0xf) != 0) {
    MVar14.modelNode = (ModelRuntimeNode *)0x0;
    MVar14.carry = false;
    return MVar14;
  }
  WVar13 = WorldObjectArray_AllocateFreeRecordCf(worldRuntime);
  modelNode1 = (ModelRuntimeNode *)WVar13.recordOrError;
  modelNode2 = modelNode1;
  if (WVar13.carry) {
ModelNodeRuntime_CreateHierarchyRecursive_ReturnAllocationFailure:
    MVar15.carry = true;
    MVar15.modelNode = modelNode2;
    return MVar15;
  }
  modelNode1->ownerClassId = MODEL_RUNTIME_CLASS_00;
  (modelNode1->modelPayload).localTranslationXQ12 = 0;
  (modelNode1->modelPayload).localTranslationYQ12 = 0;
  (modelNode1->modelPayload).localTranslationZQ12 = 0;
  AVar1 = definitionNode->localRotationAngle1;
  AVar2 = definitionNode->localRotationAngle2;
  (modelNode1->modelPayload).localRotationAngle0 = definitionNode->localRotationAngle0;
  (modelNode1->modelPayload).localRotationAngle1 = AVar1;
  (modelNode1->modelPayload).localRotationAngle2 = AVar2;
  (modelNode1->modelPayload).meshGroupMask = 0xffffffff;
  pAVar3 = (modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime;
  (modelNode1->runtimePayload).modelRuntime = modelRuntime;
  modelNode1->runtimeFlags = modelNode1->runtimeFlags | 1;
  if (pAVar3->factionIndex != 0) {
    modelNode1->runtimeFlags = modelNode1->runtimeFlags | 0x20;
  }
  *(byte *)&modelNode1->textureSubresourceBaseIndex = 0;
  *(byte *)((int)&modelNode1->textureSubresourceBaseIndex + 1) = 0;
  *(byte *)((int)&modelNode1->textureSubresourceBaseIndex + 2) = 0;
  *(byte *)((int)&modelNode1->textureSubresourceBaseIndex + 3) = 0;
  dVar10 = (modelRuntime->definitionOrSavedId).savedIdOrOffset;
  modelNode1->tintArgb = 0xffffffff;
  if ((*(uint *)(dVar10 + 0x68) & 0x10) != 0) {
    modelNode1->runtimeFlags = modelNode1->runtimeFlags | 0x10;
  }
  if ((*(uint *)(dVar10 + 0x68) & 0x20) != 0) {
    modelNode1->runtimeFlags = modelNode1->runtimeFlags | 0x200;
  }
  if ((*(uint *)(dVar10 + 0x68) & 0x40) == 0) {
    modelNode1->runtimeFlags = modelNode1->runtimeFlags | 0x100;
  }
  pMVar4 = (definitionNode->spriteAssetReference).modelResource;
  (modelNode1->modelPayload).paletteAsset = paletteAsset;
  QVar5 = pMVar4->boundingRadiusQ12;
  (modelNode1->modelPayload).textureSet = textureSet;
  modelNode1->subtreeBoundingRadiusQ12 = QVar5;
  (modelNode1->modelPayload).modelResource = pMVar4;
  modelNode1->shadingRecord = (GraphicsShadingRuntimeRecord *)0x0;
  modelNode1->modelRuntimeLinkOrSavedOffset = (void *)0x0;
  modelNode1->renderDepthBiasOrState = 0;
  dVar10 = definitionNode->childCount;
  pMVar4 = (definitionNode->spriteAssetReference).modelResource;
  uVar11 = 0;
  modelNode1->childCount = dVar10;
  modelNode1->parentNode = (ModelRuntimeNode *)0x0;
  do {
    if (dVar10 == 0) {
      return (ModelNodeCreateEaxCf5)((uint5)WVar13 & 0xffffffff);
    }
    pMVar12 = (ModelAttachmentTransformRecord *)
              (pMVar4->reserved00_AF + pMVar4->packedLookupTableRelativeOffset);
    for (MVar9 = pMVar4->packedLookupTableEntryCount; MVar9 != 0; MVar9 = MVar9 - 1) {
      uVar8 = pMVar12->packedKindAndSelector & 0xf;
      if (((uVar8 == 0) || (uVar8 == 1)) && (uVar11 == pMVar12->packedKindAndSelector >> 4)) {
        MVar14 = ModelNodeRuntime_CreateHierarchyRecursiveCf
                           (paletteAsset,textureSet,modelRuntime,
                            (MdlSerializedNodeHeader38 *)
                            definitionNode->childSerializedOffsets[uVar11],worldRuntime);
        modelNode2 = MVar14.modelNode;
        if (MVar14.carry) goto ModelNodeRuntime_CreateHierarchyRecursive_ReturnAllocationFailure;
        modelNode1->childNodes[uVar11] = modelNode2;
        if (modelNode2 == (ModelRuntimeNode *)0x0) {
          uVar8 = modelRuntime->attachmentCount0C;
          if (uVar8 < 6) {
            modelRuntime->attachmentCount0C = modelRuntime->attachmentCount0C + 1;
            modelRuntime->attachments140[uVar8].sourceTransform04 = pMVar12;
            modelRuntime->attachments140[uVar8].childNodeIndex0C = uVar11;
            modelRuntime->attachments140[uVar8].parentModelNodeOrSavedOffset08 = modelNode1;
            SVar7 = definitionNode->childSerializedOffsets[uVar11];
            modelRuntime->attachments140[uVar8].childModelRuntimeOrSavedOffset00 =
                 (ModelRuntimeSlot *)0x0;
            AVar1 = *(AngleTurn32 *)(SVar7 + 8);
            AVar2 = *(AngleTurn32 *)(SVar7 + 0xc);
            modelRuntime->attachments140[uVar8].childLocalRotationAngle2 =
                 *(AngleTurn32 *)(SVar7 + 0x10);
            modelRuntime->attachments140[uVar8].childLocalRotationAngle1 = AVar2;
            modelRuntime->attachments140[uVar8].childLocalRotationAngle0 = AVar1;
          }
        }
        else {
          modelNode2->parentNode = modelNode1;
          QVar5 = pMVar12->localTranslationYQ12;
          QVar6 = pMVar12->localTranslationZQ12;
          (modelNode2->modelPayload).localTranslationXQ12 = pMVar12->localTranslationXQ12;
          (modelNode2->modelPayload).localTranslationYQ12 = QVar5;
          (modelNode2->modelPayload).localTranslationZQ12 = QVar6;
        }
        goto ModelNodeRuntime_CreateHierarchyRecursive_AdvanceAfterChildResolution;
      }
      pMVar12 = pMVar12 + 1;
    }
    modelNode1->childNodes[uVar11] = (ModelRuntimeNode *)0x0;
ModelNodeRuntime_CreateHierarchyRecursive_AdvanceAfterChildResolution:
    uVar11 = uVar11 + 1;
    dVar10 = dVar10 - 1;
  } while( true );
}


/* Address: 0x005294E0.
   Ownership: world/model/hierarchy.
   Purpose: Recursively releases child pointers at +0xCC for the exact count at +0xC8, scans the parent object at
   +0xC4 for references back to the current node and clears each match, then calls the existing node-release
   helper. EAX and companion register state are preserved.
   Cross-module calls: WorldRuntime_UnlinkNodeFromOwnerListD8 [world/runtime/core].
*/
void __thandor_void_preserve_eax_ecx_edx
ModelRuntimeNode_ReleaseRecursiveAndDetachParent(ModelRuntimeNode *node)

{
  dword childrenRemaining;
  dword dVar1;
  ModelRuntimeNode *modelNode1;
  
  modelNode1 = node;
  for (childrenRemaining = node->childCount; childrenRemaining != 0;
      childrenRemaining = childrenRemaining - 1) {
    if (modelNode1->childNodes[0] != (ModelRuntimeNode *)0x0) {
      ModelRuntimeNode_ReleaseRecursiveAndDetachParent(modelNode1->childNodes[0]);
    }
    modelNode1 = (ModelRuntimeNode *)&(modelNode1->common).nextNode;
  }
  modelNode1 = node->parentNode;
  if (modelNode1 != (ModelRuntimeNode *)0x0) {
    for (dVar1 = modelNode1->childCount; dVar1 != 0; dVar1 = dVar1 - 1) {
      if (modelNode1->childNodes[0] == node) {
        modelNode1->childNodes[0] = (ModelRuntimeNode *)0x0;
      }
      modelNode1 = (ModelRuntimeNode *)&(modelNode1->common).nextNode;
    }
  }
  WorldRuntime_UnlinkNodeFromOwnerListD8((WorldOwnerListNode100 *)node);
  return;
}


/* Address: 0x0052A100.
   Ownership: world/model/hierarchy.
   Purpose: Recursively accumulates maximum hierarchy metrics and shot-definition category masks into the owning
   army runtime derived-selection fields.
   Cross-module calls: ShotDefinition_ComputeSelectionRange [assets/shot/catalog].
*/
void __thandor_void_preserve_eax_ecx_edx
ModelRuntimeHierarchy_AccumulateDerivedSelectionMetrics(int *modelRuntime)

{
  int *piVar1;
  int iVar2;
  ShotDefinition *definition;
  uint uVar3;
  dword dVar4;
  int iVar5;
  int iVar6;
  
  iVar6 = *modelRuntime;
  iVar5 = modelRuntime[2];
  iVar2 = modelRuntime[1];
  if ((modelRuntime[0x3b] & 1U) == 0) {
    uVar3 = *(uint *)(iVar6 + 0x48);
  }
  else {
    uVar3 = *(uint *)(iVar6 + 0x1a4);
  }
  if (*(uint *)(iVar5 + 0x90) < *(uint *)(iVar6 + 0x27c)) {
    *(uint *)(iVar5 + 0x90) = *(uint *)(iVar6 + 0x27c);
  }
  if (*(uint *)(iVar5 + 0x44) < uVar3) {
    *(uint *)(iVar5 + 0x44) = uVar3;
  }
  uVar3 = (*(int *)(iVar2 + 0x9c) - *(int *)(*(int *)(iVar5 + 4) + 0x9c)) + *(int *)(iVar6 + 0x70);
  if (*(uint *)(iVar5 + 0x48) < uVar3) {
    *(uint *)(iVar5 + 0x48) = uVar3;
  }
  definition = *(ShotDefinition **)(iVar6 + 0x2c);
  if (*(int *)(iVar6 + 0x30) != 0) {
    dVar4 = ShotDefinition_ComputeSelectionRange(definition);
    iVar6 = modelRuntime[2];
    iVar5 = 7;
    if (*(int *)(iVar6 + 0x4c) < (int)dVar4) {
      *(dword *)(iVar6 + 0x4c) = dVar4;
    }
    do {
      piVar1 = (int *)(iVar6 + 0x100 + iVar5 * 4);
      *piVar1 = *piVar1 + definition->targetClassImpactDamageQ12[iVar5];
      iVar5 = iVar5 + -1;
    } while (-1 < iVar5);
  }
  for (iVar6 = modelRuntime[3]; iVar6 != 0; iVar6 = iVar6 + -1) {
    if ((int *)modelRuntime[0x50] != (int *)0x0) {
      ModelRuntimeHierarchy_AccumulateDerivedSelectionMetrics((int *)modelRuntime[0x50]);
    }
    modelRuntime = modelRuntime + 8;
  }
  return;
}


/* Address: 0x0052A690.
   Ownership: world/model/hierarchy.
   Purpose: Recursively computes the Q12 hierarchy scale ratio consumed by the existing model-runtime metric query
   wrappers. Recursively computes the hierarchy scale ratio. EAX carries the computed Q12 ratio and EDX carries Q12
   unity 0x1000; the nominal 8-byte return type preserves the verified EDX:EAX register pair without introducing a
   structure-return pointer. Unrelated to draw scale (that is node+0xC0 in ModelRender_PrepareProjectedVertex).
*/
ModelRuntimeScaleRatioRegisterPairQ12 __thandor_eax_edx_cf_preserve_ecx
ModelRuntimeHierarchy_ComputeScaleRatioQ12Regs(ModelRuntimeSlot *modelRuntime)

{
  ModelRuntimeSlot *modelRuntime_00;
  dword attachmentsRemaining;
  int accumulatedHierarchyScaleQ12;
  ModelRuntimeSlot *attachmentDescriptorCursor;
  int scaleSampleCount;
  ModelRuntimeScaleRatioRegisterPairQ12 childScaleRatioPairQ12;
  
  accumulatedHierarchyScaleQ12 = 0x1000;
  scaleSampleCount = 1;
  attachmentDescriptorCursor = modelRuntime;
  for (attachmentsRemaining = modelRuntime->attachmentCount0C; attachmentsRemaining != 0;
      attachmentsRemaining = attachmentsRemaining - 1) {
    modelRuntime_00 = attachmentDescriptorCursor->attachments140[0].childModelRuntimeOrSavedOffset00
    ;
    if (modelRuntime_00 != (ModelRuntimeSlot *)0x0) {
      childScaleRatioPairQ12 = ModelRuntimeHierarchy_ComputeScaleRatioQ12Regs(modelRuntime_00);
      accumulatedHierarchyScaleQ12 = accumulatedHierarchyScaleQ12 + (int)childScaleRatioPairQ12;
      scaleSampleCount = scaleSampleCount + 1;
    }
    attachmentDescriptorCursor =
         (ModelRuntimeSlot *)(attachmentDescriptorCursor->reserved10_37 + 0x10);
  }
  return CONCAT44(0x1000,(int)(((longlong)(int)modelRuntime->definitionValue60_3C *
                               (longlong)accumulatedHierarchyScaleQ12) /
                              (longlong)
                              (scaleSampleCount *
                              *(int *)((modelRuntime->definitionOrSavedId).savedIdOrOffset + 0x60)))
                 );
}


/* Address: 0x0052A6F0.
   Ownership: world/model/hierarchy.
   Purpose: Recursively computes the active and total hierarchy metrics returned through the verified register-
   result wrapper. Computes the active hierarchy metric in EAX and the total hierarchy metric in EDX. The nominal
   8-byte return type records the verified register pair without changing the calling ABI.
   [RESOURCE_FUEL_ENERGY_CAPACITY_SEPARATION_CLOSURE] Computes active and total model-hierarchy Energy demand from
   ModelRuntimeSlot.energyDemandQ4. Active total excludes nodes marked unpowered via classStateEC bit 0x1.
*/
ModelRuntimeActiveTotalMetricRegisterPair
ModelRuntimeHierarchy_ComputeActiveAndTotalMetricsRegs(ModelRuntimeSlot *modelRuntime)

{
  dword activeMetricTotal;
  dword attachmentsRemaining;
  dword totalMetric;
  ModelRuntimeSlot *currentChildModelRuntime;
  dword childMetric;
  
  totalMetric = (modelRuntime->classState).definitionDerivedValueF4;
  attachmentsRemaining = modelRuntime->attachmentCount0C;
  activeMetricTotal = 0;
  if (((modelRuntime->classState).classStateEC & 1) == 0) {
    activeMetricTotal = totalMetric;
  }
  if ((*(uint *)((modelRuntime->definitionOrSavedId).savedIdOrOffset + 0x68) & 0x80) != 0) {
    for (; attachmentsRemaining != 0; attachmentsRemaining = attachmentsRemaining - 1) {
      currentChildModelRuntime = modelRuntime->attachments140[0].childModelRuntimeOrSavedOffset00;
      if (currentChildModelRuntime != (ModelRuntimeSlot *)0x0) {
        childMetric = (currentChildModelRuntime->classState).definitionDerivedValueF4;
        if (((currentChildModelRuntime->classState).classStateEC & 1) == 0) {
          activeMetricTotal = activeMetricTotal + childMetric;
        }
        totalMetric = totalMetric + childMetric;
      }
      modelRuntime = (ModelRuntimeSlot *)(modelRuntime->reserved10_37 + 0x10);
    }
  }
  return CONCAT44(totalMetric,activeMetricTotal);
}

/* Address: 0x0052AAC0.
   Ownership: world/model/hierarchy.
   Purpose: Smooths the model yaw field at offset 0x2C toward the target angle with bounded acceleration and
   deceleration, then marks the transform dirty.
*/

ModelSmoothEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ModelNodeRuntime_SmoothYawTowardTarget
          (ModelRuntimeNode *modelNodeRuntime,ModelRuntimeWeaponAimStateView200 *smoothingState,
          AngleTurn32 targetYawAngle16)

{
  ArmyWeaponDefinitionView68 *pAVar1;
  int iVar2;
  AngleTurn32 AVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  ModelSmoothEaxCf5 MVar8;
  ModelSmoothEaxCf5 MVar9;
  
  uVar4 = (modelNodeRuntime->modelPayload).localRotationAngle2;
  pAVar1 = smoothingState->modelDefinition;
  uVar7 = targetYawAngle16 - uVar4 & 0xffff;
  uVar5 = smoothingState->yawTurnVelocityAngle16 * g_InGameSimulationStepTicks;
  if (uVar7 < 0x8001) {
    if ((int)uVar5 < 0) {
LAB_0052abb0:
      smoothingState->yawTurnVelocityAngle16 = 0;
    }
    else {
      if (uVar7 <= uVar5) {
ModelNodeRuntime_SmoothYawTowardTarget_SnapToTargetAndClearTurnVelocity:
        AVar3 = (modelNodeRuntime->modelPayload).localRotationAngle2;
        smoothingState->yawTurnVelocityAngle16 = 0;
        MVar8.eax = targetYawAngle16;
        if (targetYawAngle16 != AVar3) {
          modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
          (modelNodeRuntime->modelPayload).localRotationAngle2 = targetYawAngle16;
        }
        goto LAB_0052abf0;
      }
      uVar4 = uVar4 + uVar5;
      iVar2 = pAVar1->yawTurnRateLimitAnglePerTick10;
      iVar6 = smoothingState->yawTurnVelocityAngle16 +
              g_InGameSimulationStepTicks * pAVar1->yawTurnRateAccelerationAnglePerTick1C;
      smoothingState->yawTurnVelocityAngle16 = iVar2;
      if (iVar6 < iVar2) {
        smoothingState->yawTurnVelocityAngle16 = iVar6;
      }
    }
  }
  else {
    if (0 < (int)uVar5) goto LAB_0052abb0;
    if (uVar5 + 0x10000 <= uVar7)
    goto ModelNodeRuntime_SmoothYawTowardTarget_SnapToTargetAndClearTurnVelocity;
    uVar4 = uVar4 + uVar5;
    iVar2 = pAVar1->yawTurnRateLimitAnglePerTick10;
    iVar6 = smoothingState->yawTurnVelocityAngle16 -
            g_InGameSimulationStepTicks * pAVar1->yawTurnRateAccelerationAnglePerTick1C;
    smoothingState->yawTurnVelocityAngle16 = -iVar2;
    if (-iVar2 < iVar6) {
      smoothingState->yawTurnVelocityAngle16 = iVar6;
    }
  }
  modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
  (modelNodeRuntime->modelPayload).localRotationAngle2 = uVar4 & 0xffff;
  MVar8.eax = (uVar4 & 0xffff) - targetYawAngle16 & 0xffff;
  if ((0x3ff < MVar8.eax) && (MVar8.eax < 0xfc01)) {
    MVar8.carry = true;
    return MVar8;
  }
LAB_0052abf0:
  MVar9.carry = false;
  MVar9.eax = MVar8.eax;
  return MVar9;
}


/* Address: 0x0052AC00.
   Ownership: world/model/hierarchy.
   Purpose: Clamps the target pitch to the definition bounds, smooths the model pitch field at offset 0x28 with
   bounded acceleration and deceleration, then marks the transform dirty.
*/

ModelSmoothEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ModelNodeRuntime_SmoothPitchTowardTarget
          (ModelRuntimeNode *modelNodeRuntime,ModelRuntimeWeaponAimStateView200 *smoothingState,
          AngleTurn32 targetPitchAngle16)

{
  ArmyWeaponDefinitionView68 *pAVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  ModelSmoothEaxCf5 MVar5;
  ModelSmoothEaxCf5 MVar6;
  
  uVar2 = (modelNodeRuntime->modelPayload).localRotationAngle1;
  pAVar1 = smoothingState->modelDefinition;
  MVar5.eax = targetPitchAngle16;
  if ((int)pAVar1->maximumPitchAngle28 < (int)targetPitchAngle16) {
    MVar5.eax = pAVar1->maximumPitchAngle28;
  }
  if ((int)MVar5.eax < (int)pAVar1->minimumPitchAngle24) {
    MVar5.eax = pAVar1->minimumPitchAngle24;
  }
  iVar3 = smoothingState->pitchTurnVelocityAngle16 * g_InGameSimulationStepTicks;
  if (MVar5.eax != uVar2) {
    if ((int)uVar2 <= (int)MVar5.eax) {
      if (iVar3 < 0) {
LAB_0052ad00:
        smoothingState->pitchTurnVelocityAngle16 = 0;
      }
      else {
        if ((int)(MVar5.eax - uVar2) <= iVar3)
        goto ModelNodeRuntime_SmoothPitchTowardTarget_SnapToTargetAndClearPitchVelocity;
        uVar2 = uVar2 + iVar3;
        iVar3 = pAVar1->pitchTurnRateLimitAnglePerTick14;
        iVar4 = smoothingState->pitchTurnVelocityAngle16 +
                g_InGameSimulationStepTicks * pAVar1->pitchTurnRateAccelerationAnglePerTick20;
        smoothingState->pitchTurnVelocityAngle16 = iVar3;
        if (iVar4 < iVar3) {
          smoothingState->pitchTurnVelocityAngle16 = iVar4;
        }
      }
ModelNodeRuntime_SmoothPitchTowardTarget_CommitStepAndReturnToleranceCarryStatus:
      modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
      (modelNodeRuntime->modelPayload).localRotationAngle1 = uVar2;
      MVar5.eax = uVar2 - MVar5.eax & 0xffff;
      if ((0x3ff < MVar5.eax) && (MVar5.eax < 0xfc01)) {
        MVar5.carry = true;
        return MVar5;
      }
      goto LAB_0052ad40;
    }
    if (0 < iVar3) goto LAB_0052ad00;
    if ((int)(MVar5.eax - uVar2) < iVar3) {
      uVar2 = uVar2 + iVar3;
      iVar3 = pAVar1->pitchTurnRateLimitAnglePerTick14;
      iVar4 = smoothingState->pitchTurnVelocityAngle16 -
              g_InGameSimulationStepTicks * pAVar1->pitchTurnRateAccelerationAnglePerTick20;
      smoothingState->pitchTurnVelocityAngle16 = -iVar3;
      if (-iVar3 < iVar4) {
        smoothingState->pitchTurnVelocityAngle16 = iVar4;
      }
      goto ModelNodeRuntime_SmoothPitchTowardTarget_CommitStepAndReturnToleranceCarryStatus;
    }
  }
ModelNodeRuntime_SmoothPitchTowardTarget_SnapToTargetAndClearPitchVelocity:
  uVar2 = (modelNodeRuntime->modelPayload).localRotationAngle1;
  smoothingState->pitchTurnVelocityAngle16 = 0;
  if (MVar5.eax != uVar2) {
    modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
    (modelNodeRuntime->modelPayload).localRotationAngle1 = MVar5.eax;
  }
LAB_0052ad40:
  MVar6.carry = false;
  MVar6.eax = MVar5.eax;
  return MVar6;
}


/* Address: 0x004BD1A0.
   Ownership: world/model/hierarchy.
   Purpose: Stores one packed tint on the current model runtime node and recursively applies it to every non-null
   child in the exact child pointer array.
*/
void __thandor_void_preserve_eax_ecx_edx
ModelNodeRuntime_ApplyTintRecursive(PackedArgb32 tintArgb,ModelRuntimeNode *modelNode)

{
  dword childrenRemaining;
  
  childrenRemaining = modelNode->childCount;
  modelNode->tintArgb = tintArgb;
  for (; childrenRemaining != 0; childrenRemaining = childrenRemaining - 1) {
    if (modelNode->childNodes[0] != (ModelRuntimeNode *)0x0) {
      ModelNodeRuntime_ApplyTintRecursive(tintArgb,modelNode->childNodes[0]);
    }
    modelNode = (ModelRuntimeNode *)&(modelNode->common).nextNode;
  }
  return;
}


/* Address: 0x004BE390.
   Ownership: world/model/hierarchy.
   Purpose: Builds the current rotation basis when needed, composes every child transform with its parent, extracts
   child Euler angles, propagates tint state, and recurses. Parent transform x child local basis -> child world
   transform; extracts child eulers, propagates tint, recurses — the stock hierarchy composition the viewer's
   KitHierarchyComposer mirrors. Role: Composes child local transforms with the parent world transform recursively.
   Inputs: Parent world transform and each child local translation/rotation. Outputs: Updated child world matrices,
   world positions and orientation values.
   Cross-module calls: FixedTransform_BuildRotationBasis [core/math/fixed], FixedTransform_Compose
   [core/math/fixed], FixedTransform_ExtractEulerAnglesRegs [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
ModelNodeRuntime_ComposeChildTransformsRecursive(ModelRuntimeNode *modelNodeRuntime)

{
  ModelRuntimeNode *modelNodeRuntime_00;
  uint childIndex;
  FixedEulerPairEdxEax8 extractedEulerAngles;
  FixedEulerAnglesEaxEcxEdx12 FVar1;
  ModelRuntimeNode *childNode;
  PackedArgb32 inheritedTintArgb;
  
  childIndex = 0;
  if (modelNodeRuntime->parentNode == (ModelRuntimeNode *)0x0) {
    FixedTransform_BuildRotationBasis
              (&modelNodeRuntime->worldTransform,
               (modelNodeRuntime->modelPayload).worldRotationAngle2,
               (modelNodeRuntime->modelPayload).worldRotationAngle1,
               (modelNodeRuntime->modelPayload).worldRotationAngle0);
  }
  if (modelNodeRuntime->childCount != 0) {
    do {
      modelNodeRuntime_00 = modelNodeRuntime->childNodes[childIndex];
      childIndex = childIndex + 1;
      if (modelNodeRuntime_00 != (ModelRuntimeNode *)0x0) {
        FixedTransform_BuildRotationBasis
                  ((GraphicsFixedMatrix3x4 *)&g_ModelTransformScratchMatrix,
                   (modelNodeRuntime_00->modelPayload).localRotationAngle2,
                   (modelNodeRuntime_00->modelPayload).localRotationAngle1,
                   (modelNodeRuntime_00->modelPayload).localRotationAngle0);
        g_ModelTransformTranslationX = (modelNodeRuntime_00->modelPayload).localTranslationXQ12;
        g_ModelTransformTranslationY = (modelNodeRuntime_00->modelPayload).localTranslationYQ12;
        g_ModelTransformTranslationZ = (modelNodeRuntime_00->modelPayload).localTranslationZQ12;
        FixedTransform_Compose
                  (&modelNodeRuntime_00->worldTransform,
                   (GraphicsFixedMatrix3x4 *)&g_ModelTransformScratchMatrix,
                   &modelNodeRuntime->worldTransform);
        FVar1 = FixedTransform_ExtractEulerAnglesRegs(&modelNodeRuntime_00->worldTransform);
        (modelNodeRuntime_00->modelPayload).worldRotationAngle2 = FVar1.eaxAngle;
        (modelNodeRuntime_00->modelPayload).worldRotationAngle0 = FVar1.ecxAngle;
        (modelNodeRuntime_00->modelPayload).worldRotationAngle1 = FVar1.edxAngle;
        g_ModelTransformTranslationX = 0;
        g_ModelTransformTranslationY = 0;
        g_ModelTransformTranslationZ = 0;
        inheritedTintArgb = modelNodeRuntime->tintArgb;
        modelNodeRuntime_00->runtimeFlags = modelNodeRuntime_00->runtimeFlags | 1;
        modelNodeRuntime_00->tintArgb = inheritedTintArgb;
        ModelNodeRuntime_ComposeChildTransformsRecursive(modelNodeRuntime_00);
      }
    } while (childIndex < modelNodeRuntime->childCount);
  }
  return;
}


/* Address: 0x0052AEA0.
   Ownership: world/model/hierarchy.
   Purpose: Recursively selects the first faction-unlocked variant among six definition identifiers, swaps the
   model definition while preserving the scaled current metric, and rebuilds derived hierarchy metrics. It is
   distinct from FrontendPlayerIndex_V306, PlayerRuntimeId, active-faction masks or codes, and PCK-backed
   ArmyAssetId, ModelDefinitionId, and TechnologyId domains.
   Cross-module calls: ModelDefinition_IsFactionTechnologyUnlockedCf [assets/model/definitions],
   ModelDefinitionRegistry_FindByIdWithErrorCf [assets/model/definitions],
   ArmyRuntime_RebuildDerivedSelectionMetrics [gameplay/army/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
ModelRuntimeHierarchy_ApplyFactionTechnologyVariantsRecursive
          (FactionRuntimeIndex factionIndex,int *modelRuntime)

{
  PckModelDefinitionIdCatalog modelDefinitionId;
  int iVar1;
  int iVar2;
  bool bVar3;
  ModelDefinitionLookupEaxCf5 MVar4;
  
  iVar2 = *modelRuntime;
  iVar1 = 6;
  while ((modelDefinitionId = *(PckModelDefinitionIdCatalog *)(iVar2 + 0x238),
         modelDefinitionId == 0 ||
         (bVar3 = ModelDefinition_IsFactionTechnologyUnlockedCf
                            (g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits,
                             modelDefinitionId), bVar3))) {
    iVar2 = iVar2 + 4;
    iVar1 = iVar1 + -1;
    if (iVar1 == 0) {
ModelRuntimeHierarchy_ApplyFactionTechnologyVariantsRecursive_RecurseChildrenAfterVariantResolution:
      for (iVar2 = modelRuntime[3]; iVar2 != 0; iVar2 = iVar2 + -1) {
        if ((int *)modelRuntime[0x50] != (int *)0x0) {
          ModelRuntimeHierarchy_ApplyFactionTechnologyVariantsRecursive
                    (factionIndex,(int *)modelRuntime[0x50]);
        }
        modelRuntime = modelRuntime + 8;
      }
      return;
    }
  }
  MVar4 = ModelDefinitionRegistry_FindByIdWithErrorCf(modelDefinitionId);
  iVar2 = *modelRuntime;
  *modelRuntime = (int)MVar4.modelDefinition;
  modelRuntime[0xf] =
       (int)(((longlong)modelRuntime[0xf] * (longlong)(int)MVar4.modelDefinition[8].byteSize) /
            (longlong)*(int *)(iVar2 + 0x60));
  ArmyRuntime_RebuildDerivedSelectionMetrics((ArmyRuntimeSlot *)modelRuntime[2]);
  goto 
  ModelRuntimeHierarchy_ApplyFactionTechnologyVariantsRecursive_RecurseChildrenAfterVariantResolution
  ;
}

