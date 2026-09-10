#include <thandor/world/model/hierarchy.h>

/* Implementation ownership: world/model/hierarchy. */

/* Address: 0x004BD1F0.
   Ownership: world/model/hierarchy.
   Purpose: Derives a packed ARGB tint from model state flags and the global tint lookup table, then propagates the
   changed tint through the model runtime hierarchy.
   Local calls: ModelNodeRuntime_ApplyTintRecursive.
*/
void ModelNodeRuntime_UpdateStateTintRecursive(ModelRuntimeNode *modelNodeRuntime)

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
undefined8 __fastcall
ModelNodeRuntime_RebuildTransformsFromRoot
          (undefined4 param_1,undefined4 param_2,ModelRuntimeNode *modelNodeRuntime)

{
  undefined4 extraout_EAX;
  undefined4 extraout_EAX_00;
  undefined4 uVar1;
  ModelRuntimeNode *parentNode;
  
  if (modelNodeRuntime->parentNode == (ModelRuntimeNode *)0x0) {
    ModelNodeRuntime_ComposeChildTransformsRecursive(modelNodeRuntime);
    uVar1 = extraout_EAX_00;
  }
  else {
    ModelNodeRuntime_ComposeChildTransformsRecursive(modelNodeRuntime->parentNode);
    uVar1 = extraout_EAX;
  }
  return CONCAT44(param_2,uVar1);
}

/* Address: 0x0051DB80.
   Ownership: world/model/hierarchy.
   Purpose: Invokes the recursive faction-technology variant pass on the attached model runtime hierarchy. It is
   distinct from FrontendPlayerIndex_V306, PlayerRuntimeId, active-faction masks or codes, and PCK-backed
   ArmyAssetId, ModelDefinitionId, and TechnologyId domains.
   Local calls: ModelRuntimeHierarchy_ApplyFactionTechnologyVariantsRecursive.
*/
undefined8 __fastcall
ModelRuntimeHierarchy_ApplyFactionTechnologyVariants
          (undefined4 param_1,undefined4 param_2,FactionRuntimeIndex factionIndex,
          ArmyRuntimeSlot *modelRuntimeHolder)

{
  undefined8 technologyVariantResultPair;
  
  technologyVariantResultPair =
       ModelRuntimeHierarchy_ApplyFactionTechnologyVariantsRecursive
                 (factionIndex,modelRuntimeHolder->definitionOrAsset);
  return technologyVariantResultPair;
}

/* Address: 0x004BD310.
   Ownership: world/model/hierarchy.
   Purpose: Transforms every vertex in each model record, accumulates global minimum and maximum coordinates, then
   recursively processes every child model node. Typed parameters: p2 modelNode→ModelRuntimeNode *. Calling
   convention, complete VariableStorage serialization, function bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Cross-module calls: FixedTransform_ApplyPoint [core/math/fixed].
*/
void ModelNodeRuntime_AccumulateTransformedBoundsRecursive(ModelRuntimeNode *modelNode)

{
  ModelResourceHitTestAndRenderView210 *pMVar1;
  dword extraout_ECX;
  dword dVar2;
  int iVar3;
  int extraout_EDX;
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
      dVar2 = extraout_ECX;
      iVar3 = extraout_EDX;
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
void ModelNodeRuntime_BuildViewFacingRotation(ModelRuntimeNode *modelNodeRuntime)

{
  dword viewFacingAngle16;
  FixedMathVectorComponent32 y;
  undefined8 rotatedViewDirectionPairQ12;
  
  rotatedViewDirectionPairQ12 =
       FixedTransform_ApplyEulerRotationToVectorRegs
                 ((modelNodeRuntime->worldTransform).translation.z - g_ViewOriginFixed.z,
                  (modelNodeRuntime->worldTransform).translation.y - g_ViewOriginFixed.y,
                  (modelNodeRuntime->worldTransform).translation.x - g_ViewOriginFixed.x,0,
                  (modelNodeRuntime->modelPayload).worldRotationAngle1,
                  (modelNodeRuntime->modelPayload).worldRotationAngle0 - 0x8000);
  viewFacingAngle16 =
       FixedMath_Atan2Angle16(y,(FixedMathVectorComponent32)rotatedViewDirectionPairQ12);
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
void ModelNodeRuntime_BuildBillboardRotation(ModelRuntimeNode *modelNodeRuntime)

{
  int extraout_ECX;
  uint angle0;
  int extraout_EDX;
  
  FixedMath_VectorToAngles3Regs
            ((modelNodeRuntime->worldTransform).translation.z - g_ViewOriginFixed.z,
             (modelNodeRuntime->worldTransform).translation.y - g_ViewOriginFixed.y,
             (modelNodeRuntime->worldTransform).translation.x - g_ViewOriginFixed.x);
  angle0 = extraout_ECX + 0x8000U & 0xffff;
  FixedTransform_BuildRotationBasis(&modelNodeRuntime->worldTransform,angle0,-extraout_EDX,angle0);
  return;
}

/* Address: 0x004BE9D0.
   Ownership: world/model/hierarchy.
   Purpose: Recursively recomputes the node bounding radius at +0x54. The result is the maximum of the base model
   radius at model +0xD8 and each child translation-vector length plus that child radius.
   Cross-module calls: FixedMath_LengthVec3 [core/math/fixed].
*/
undefined8 ModelNodeRuntime_RecomputeSubtreeBoundingRadius(ModelRuntimeNode *modelNodeRuntime)

{
  undefined4 in_EAX;
  dword dVar1;
  uint uVar2;
  dword extraout_ECX;
  undefined4 in_EDX;
  int extraout_EDX;
  uint uVar3;
  ModelRuntimeNode *modelNode1;
  undefined8 uVar4;
  
  uVar3 = ((modelNodeRuntime->modelPayload).modelResource)->boundingRadiusQ12;
  modelNode1 = modelNodeRuntime;
  for (dVar1 = modelNodeRuntime->childCount; dVar1 != 0; dVar1 = dVar1 - 1) {
    if (modelNode1->childNodes[0] != (ModelRuntimeNode *)0x0) {
      uVar4 = ModelNodeRuntime_RecomputeSubtreeBoundingRadius(modelNode1->childNodes[0]);
      dVar1 = FixedMath_LengthVec3((GraphicsFixedVec3 *)((int)((ulonglong)uVar4 >> 0x20) + 0x18));
      uVar2 = dVar1 + *(int *)(extraout_EDX + 0x54);
      dVar1 = extraout_ECX;
      if (uVar3 < uVar2) {
        uVar3 = uVar2;
      }
    }
    modelNode1 = (ModelRuntimeNode *)&(modelNode1->common).nextNode;
  }
  modelNodeRuntime->subtreeBoundingRadiusQ12 = uVar3;
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x004BEA30.
   Ownership: world/model/hierarchy.
   Purpose: Typed parameters: p2 intervalRadiusQ14→DepthIntervalRadius32_V343. Calling convention, complete
   VariableStorage serialization, function bytes, control flow, globals, locals, and executable data remain
   unchanged.
   Cross-module calls: DepthInterval_BuildBinMask [graphics/render/primitives].
*/
undefined4 __fastcall
ModelNodeRuntime_UpdateDepthBinMasks
          (undefined4 param_1,undefined4 param_2,DepthIntervalRadius32 intervalRadiusQ14,
          ModelRuntimeNode *modelNodeRuntime)

{
  undefined4 in_EAX;
  DepthBinMaskEaxPreservedEdxCarrier64 depthBinMaskResultPair;
  GraphicsWorldCoordinateQ12 centerDepth;
  
  if (intervalRadiusQ14 < modelNodeRuntime->subtreeBoundingRadiusQ12) {
    intervalRadiusQ14 = modelNodeRuntime->subtreeBoundingRadiusQ12;
  }
  centerDepth = (modelNodeRuntime->worldTransform).translation.y;
  depthBinMaskResultPair =
       DepthInterval_BuildBinMask
                 (intervalRadiusQ14,(modelNodeRuntime->worldTransform).translation.x);
  modelNodeRuntime->depthBinMaskNear = (ModelDepthBinMask)depthBinMaskResultPair;
  depthBinMaskResultPair = DepthInterval_BuildBinMask(intervalRadiusQ14,centerDepth);
  modelNodeRuntime->depthBinMaskFar = (ModelDepthBinMask)depthBinMaskResultPair;
  return in_EAX;
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
undefined8 __thiscall
ModelNodeRuntime_TransformLocalPointRegs
          (undefined4 param_1_00,ModelPackedPointRecord *localPointRecord,
          ModelRuntimeNode *modelNodeRuntime)

{
  FixedTransform_ApplyPoint
            ((GraphicsFixedVec3 *)&g_ModelTransformOutputX,&localPointRecord->localPosition,
             &modelNodeRuntime->worldTransform);
  return CONCAT44(g_ModelTransformOutputZ,g_ModelTransformOutputX);
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
uint ModelNodeRuntime_ComputeRelativeDirectionAngle
               (ModelRuntimeNode *modelNodeRuntime,AngleTurn32 param_2,AngleTurn32 param_3)

{
  AngleTurn32 rotationAngle1;
  uint uVar1;
  FixedMathVectorComponent32 y;
  int extraout_ECX;
  uint rotationAngle0;
  undefined8 rotatedDirectionPairQ12;
  
  uVar1 = -(modelNodeRuntime->modelPayload).worldRotationAngle2;
  rotationAngle1 = (modelNodeRuntime->modelPayload).worldRotationAngle1;
  rotationAngle0 = uVar1 & 0xffff;
  rotatedDirectionPairQ12 =
       FixedTransform_RotateDirectionScaledRegs
                 (rotationAngle1,rotationAngle0,0x1000,param_2,param_3,rotationAngle0,rotationAngle1
                  ,(modelNodeRuntime->modelPayload).worldRotationAngle0 + 0x8000 + uVar1 & 0xffff);
  FixedMath_VectorToAngles3Regs
            ((FixedMathVectorComponent32)((ulonglong)rotatedDirectionPairQ12 >> 0x20),y,
             (FixedMathVectorComponent32)rotatedDirectionPairQ12);
  return extraout_ECX + (modelNodeRuntime->modelPayload).localRotationAngle2 & 0xffff;
}

/* Address: 0x0050A7A0.
   Ownership: world/model/hierarchy.
   Purpose: CF clear returns EAX distance metric; CF set reports miss.
   Cross-module calls: FixedTransform_Compose [core/math/fixed], FixedTransform_ApplyPoint [core/math/fixed],
   Graphics_ProjectViewPoint [graphics/core/runtime], GraphicsProjectedPoint_IsInsideTriangleCf
   [graphics/render/projection], FixedMath_Length3 [core/math/fixed].
*/
dword ModelRuntimeNode_HitTestProjectedBoundsAndChildrenCf
                (int pointerY,int pointerX,ModelRuntimeNode *modelNode,
                FrontendModelPointerContextRuntimeState118 *context)

{
  ModelResourceHitTestAndRenderView210 *pMVar1;
  GraphicsWorldCoordinateQ12 GVar2;
  ModelRuntimeNode *modelNode_00;
  byte bVar3;
  GraphicsFixedMatrix3x4 *extraout_EAX;
  GraphicsFixedMatrix3x4 *extraout_EAX_00;
  GraphicsFixedMatrix3x4 *extraout_EAX_01;
  GraphicsFixedMatrix3x4 *extraout_EAX_02;
  GraphicsFixedMatrix3x4 *extraout_EAX_03;
  GraphicsFixedMatrix3x4 *extraout_EAX_04;
  GraphicsFixedMatrix3x4 *extraout_EAX_05;
  GraphicsFixedMatrix3x4 *extraout_EAX_06;
  GraphicsFixedMatrix3x4 *extraout_EAX_07;
  GraphicsFixedMatrix3x4 *extraout_EAX_08;
  GraphicsFixedMatrix3x4 *extraout_EAX_09;
  GraphicsFixedMatrix3x4 *extraout_EAX_10;
  GraphicsFixedMatrix3x4 *extraout_EAX_11;
  dword dVar4;
  GraphicsFixedMatrix3x4 *transformA;
  dword extraout_EDX;
  int iVar5;
  bool bVar6;
  GraphicsProjectedPointEdxEax8 GVar7;
  
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
    bVar6 = (int)g_ProjectionScaleFixed <= g_GraphicsTransformOutputScratchVec3.z;
    if (bVar6) {
      GVar7 = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
      g_ModelProjectedBoundsCornerScratch8[0].x = (int)GVar7 >> 0xc;
      g_ModelProjectedBoundsCornerScratch8[0].y =
           (GraphicsProjectedCoordinate)((longlong)GVar7 >> 0x2c);
    }
    bVar3 = !bVar6;
    g_GraphicsTransformInputScratchVec3.x = GVar2;
    FixedTransform_ApplyPoint
              (&g_GraphicsTransformOutputScratchVec3,&g_GraphicsTransformInputScratchVec3,
               &g_GraphicsTransformScratchMatrix3x4);
    if (g_GraphicsTransformOutputScratchVec3.z < (int)g_ProjectionScaleFixed) {
      bVar3 = bVar3 | 2;
    }
    else {
      GVar7 = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
      g_ModelProjectedBoundsCornerScratch8[1].x = (int)GVar7 >> 0xc;
      g_ModelProjectedBoundsCornerScratch8[1].y =
           (GraphicsProjectedCoordinate)((longlong)GVar7 >> 0x2c);
    }
    g_GraphicsTransformInputScratchVec3.x = pMVar1->localBoundsX0Q12;
    g_GraphicsTransformInputScratchVec3.y = pMVar1->localBoundsY1Q12;
    FixedTransform_ApplyPoint
              (&g_GraphicsTransformOutputScratchVec3,&g_GraphicsTransformInputScratchVec3,
               &g_GraphicsTransformScratchMatrix3x4);
    if (g_GraphicsTransformOutputScratchVec3.z < (int)g_ProjectionScaleFixed) {
      bVar3 = bVar3 | 4;
    }
    else {
      GVar7 = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
      g_ModelProjectedBoundsCornerScratch8[2].x = (int)GVar7 >> 0xc;
      g_ModelProjectedBoundsCornerScratch8[2].y =
           (GraphicsProjectedCoordinate)((longlong)GVar7 >> 0x2c);
    }
    g_GraphicsTransformInputScratchVec3.x = GVar2;
    FixedTransform_ApplyPoint
              (&g_GraphicsTransformOutputScratchVec3,&g_GraphicsTransformInputScratchVec3,
               &g_GraphicsTransformScratchMatrix3x4);
    if (g_GraphicsTransformOutputScratchVec3.z < (int)g_ProjectionScaleFixed) {
      bVar3 = bVar3 | 8;
    }
    else {
      GVar7 = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
      g_ModelProjectedBoundsCornerScratch8[3].x = (int)GVar7 >> 0xc;
      g_ModelProjectedBoundsCornerScratch8[3].y =
           (GraphicsProjectedCoordinate)((longlong)GVar7 >> 0x2c);
    }
    g_GraphicsTransformInputScratchVec3.x = pMVar1->localBoundsX0Q12;
    g_GraphicsTransformInputScratchVec3.y = pMVar1->localBoundsY0Q12;
    g_GraphicsTransformInputScratchVec3.z = pMVar1->localBoundsZ1Q12;
    FixedTransform_ApplyPoint
              (&g_GraphicsTransformOutputScratchVec3,&g_GraphicsTransformInputScratchVec3,
               &g_GraphicsTransformScratchMatrix3x4);
    if (g_GraphicsTransformOutputScratchVec3.z < (int)g_ProjectionScaleFixed) {
      bVar3 = bVar3 | 0x10;
    }
    else {
      GVar7 = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
      g_ModelProjectedBoundsCornerScratch8[4].x = (int)GVar7 >> 0xc;
      g_ModelProjectedBoundsCornerScratch8[4].y =
           (GraphicsProjectedCoordinate)((longlong)GVar7 >> 0x2c);
    }
    g_GraphicsTransformInputScratchVec3.x = GVar2;
    FixedTransform_ApplyPoint
              (&g_GraphicsTransformOutputScratchVec3,&g_GraphicsTransformInputScratchVec3,
               &g_GraphicsTransformScratchMatrix3x4);
    if (g_GraphicsTransformOutputScratchVec3.z < (int)g_ProjectionScaleFixed) {
      bVar3 = bVar3 | 0x20;
    }
    else {
      GVar7 = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
      g_ModelProjectedBoundsCornerScratch8[5].x = (int)GVar7 >> 0xc;
      g_ModelProjectedBoundsCornerScratch8[5].y =
           (GraphicsProjectedCoordinate)((longlong)GVar7 >> 0x2c);
    }
    g_GraphicsTransformInputScratchVec3.x = pMVar1->localBoundsX0Q12;
    g_GraphicsTransformInputScratchVec3.y = pMVar1->localBoundsY1Q12;
    FixedTransform_ApplyPoint
              (&g_GraphicsTransformOutputScratchVec3,&g_GraphicsTransformInputScratchVec3,
               &g_GraphicsTransformScratchMatrix3x4);
    if (g_GraphicsTransformOutputScratchVec3.z < (int)g_ProjectionScaleFixed) {
      bVar3 = bVar3 | 0x40;
    }
    else {
      GVar7 = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
      g_ModelProjectedBoundsCornerScratch8[6].x = (int)GVar7 >> 0xc;
      g_ModelProjectedBoundsCornerScratch8[6].y =
           (GraphicsProjectedCoordinate)((longlong)GVar7 >> 0x2c);
    }
    g_GraphicsTransformInputScratchVec3.x = GVar2;
    FixedTransform_ApplyPoint
              (&g_GraphicsTransformOutputScratchVec3,&g_GraphicsTransformInputScratchVec3,
               &g_GraphicsTransformScratchMatrix3x4);
    if (g_GraphicsTransformOutputScratchVec3.z < (int)g_ProjectionScaleFixed) {
      bVar3 = bVar3 | 0x80;
      transformA = extraout_EAX;
    }
    else {
      GVar7 = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
      transformA = (GraphicsFixedMatrix3x4 *)((int)GVar7 >> 0xc);
      g_ModelProjectedBoundsCornerScratch8[7].y =
           (GraphicsProjectedCoordinate)((longlong)GVar7 >> 0x2c);
      g_ModelProjectedBoundsCornerScratch8[7].x = (GraphicsProjectedCoordinate)transformA;
    }
    bVar6 = false;
    if ((((((bVar3 & 7) == 0) &&
          (GraphicsProjectedPoint_IsInsideTriangleCf
                     (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 2,
                      g_ModelProjectedBoundsCornerScratch8 + 1,g_ModelProjectedBoundsCornerScratch8)
          , transformA = extraout_EAX_00, bVar6)) ||
         ((bVar6 = false, (bVar3 & 0xe) == 0 &&
          (GraphicsProjectedPoint_IsInsideTriangleCf
                     (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 2,
                      g_ModelProjectedBoundsCornerScratch8 + 1,
                      g_ModelProjectedBoundsCornerScratch8 + 3), transformA = extraout_EAX_01, bVar6
          )))) || ((bVar6 = false, (bVar3 & 0x70) == 0 &&
                   (GraphicsProjectedPoint_IsInsideTriangleCf
                              (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 6,
                               g_ModelProjectedBoundsCornerScratch8 + 5,
                               g_ModelProjectedBoundsCornerScratch8 + 4),
                   transformA = extraout_EAX_02, bVar6)))) ||
       (((((bVar6 = false, (bVar3 & 0xe0) == 0 &&
           (GraphicsProjectedPoint_IsInsideTriangleCf
                      (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 6,
                       g_ModelProjectedBoundsCornerScratch8 + 5,
                       g_ModelProjectedBoundsCornerScratch8 + 7), transformA = extraout_EAX_03,
           bVar6)) ||
          ((((bVar6 = false, (bVar3 & 0x15) == 0 &&
             (GraphicsProjectedPoint_IsInsideTriangleCf
                        (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 2,
                         g_ModelProjectedBoundsCornerScratch8 + 4,
                         g_ModelProjectedBoundsCornerScratch8), transformA = extraout_EAX_04, bVar6)
             ) || ((bVar6 = false, (bVar3 & 0x54) == 0 &&
                   (GraphicsProjectedPoint_IsInsideTriangleCf
                              (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 2,
                               g_ModelProjectedBoundsCornerScratch8 + 6,
                               g_ModelProjectedBoundsCornerScratch8 + 4),
                   transformA = extraout_EAX_05, bVar6)))) ||
           ((bVar6 = false, (bVar3 & 0x2a) == 0 &&
            (GraphicsProjectedPoint_IsInsideTriangleCf
                       (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 3,
                        g_ModelProjectedBoundsCornerScratch8 + 5,
                        g_ModelProjectedBoundsCornerScratch8 + 1), transformA = extraout_EAX_06,
            bVar6)))))) ||
         ((bVar6 = false, (bVar3 & 0xa8) == 0 &&
          (GraphicsProjectedPoint_IsInsideTriangleCf
                     (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 3,
                      g_ModelProjectedBoundsCornerScratch8 + 7,
                      g_ModelProjectedBoundsCornerScratch8 + 5), transformA = extraout_EAX_07, bVar6
          )))) || ((((bVar6 = false, (bVar3 & 0x13) == 0 &&
                     (GraphicsProjectedPoint_IsInsideTriangleCf
                                (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 4,
                                 g_ModelProjectedBoundsCornerScratch8 + 1,
                                 g_ModelProjectedBoundsCornerScratch8), transformA = extraout_EAX_08
                     , bVar6)) ||
                    ((bVar6 = false, (bVar3 & 0x32) == 0 &&
                     (GraphicsProjectedPoint_IsInsideTriangleCf
                                (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 1,
                                 g_ModelProjectedBoundsCornerScratch8 + 5,
                                 g_ModelProjectedBoundsCornerScratch8 + 4),
                     transformA = extraout_EAX_09, bVar6)))) ||
                   (((bVar6 = false, (bVar3 & 0x4c) == 0 &&
                     (GraphicsProjectedPoint_IsInsideTriangleCf
                                (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 3,
                                 g_ModelProjectedBoundsCornerScratch8 + 6,
                                 g_ModelProjectedBoundsCornerScratch8 + 2),
                     transformA = extraout_EAX_10, bVar6)) ||
                    ((bVar6 = false, (bVar3 & 200) == 0 &&
                     (GraphicsProjectedPoint_IsInsideTriangleCf
                                (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 3,
                                 g_ModelProjectedBoundsCornerScratch8 + 6,
                                 g_ModelProjectedBoundsCornerScratch8 + 7),
                     transformA = extraout_EAX_11, bVar6)))))))))) {
      if ((context->contextFlags & 0x80000) != 0) {
        dVar4 = FixedMath_Length3(((pMVar1->localBoundsZ0Q12 + pMVar1->localBoundsZ1Q12 >> 1) +
                                  (modelNode->worldTransform).translation.z) -
                                  context->hitReferenceWorldZQ12,
                                  ((pMVar1->localBoundsY0Q12 + pMVar1->localBoundsY1Q12 >> 1) +
                                  (modelNode->worldTransform).translation.y) -
                                  context->hitReferenceWorldYQ12,
                                  ((pMVar1->localBoundsX0Q12 + pMVar1->localBoundsX1Q12 >> 1) +
                                  (modelNode->worldTransform).translation.x) -
                                  context->hitReferenceWorldXQ12);
        return dVar4;
      }
      dVar4 = FixedMath_Length3((modelNode->worldTransform).translation.z -
                                context->hitReferenceWorldZQ12,
                                (modelNode->worldTransform).translation.y -
                                context->hitReferenceWorldYQ12,
                                (modelNode->worldTransform).translation.x -
                                context->hitReferenceWorldXQ12);
      return dVar4;
    }
  }
  dVar4 = modelNode->childCount;
  iVar5 = 0;
  while( true ) {
    if (dVar4 == 0) {
      return (dword)transformA;
    }
    modelNode_00 = *(ModelRuntimeNode **)((int)modelNode->childNodes + iVar5);
    bVar6 = false;
    if ((modelNode_00 != (ModelRuntimeNode *)0x0) &&
       (transformA = (GraphicsFixedMatrix3x4 *)
                     ModelRuntimeNode_HitTestProjectedBoundsAndChildrenCf
                               (pointerY,pointerX,modelNode_00,context), dVar4 = extraout_EDX,
       !bVar6)) break;
    iVar5 = iVar5 + 4;
    dVar4 = dVar4 - 1;
  }
  return (dword)transformA;
}

/* Address: 0x0050B1D0.
   Ownership: world/model/hierarchy.
   Purpose: Returns nearest Q12 ray distance with CF set. EDX is the nearest ModelRuntimeNode side channel on
   success; CF clear returns 0x7fffffff.
   Cross-module calls: FixedTransform_BuildRotationBasis [core/math/fixed], FixedTransform_ApplyPoint
   [core/math/fixed], ModelMesh_IntersectTriangleRayDistanceCf [assets/model/definitions].
*/
ModelRaycastNearestHitCfRegisterResult
ModelNodeRuntime_RaycastHierarchyNearestCf(ModelRuntimeNode *modelNodeRuntime)

{
  GraphicsFixedVec3 **ppGVar1;
  int iVar2;
  ModelResourceHitTestAndRenderView210 *pMVar3;
  longlong lVar4;
  longlong lVar5;
  uint uVar6;
  ModelRaycastNearestHitCfRegisterResult MVar7;
  int iVar8;
  int extraout_EAX;
  Q12 QVar9;
  int iVar10;
  int extraout_ECX;
  int extraout_ECX_00;
  dword extraout_ECX_01;
  dword dVar11;
  int iVar12;
  ModelRuntimeNode *modelNode1;
  int extraout_EDX;
  ModelPackedGeometryRecordCount extraout_EDX_00;
  ModelPackedGeometryRecordCount MVar13;
  ModelMeshGroupCount MVar14;
  Q12 QVar15;
  ModelMeshGroupRelativeOffset *pMVar16;
  ModelRaycastTriangleDescriptor *triangle;
  bool bVar17;
  ModelRaycastNearestHitCfRegisterResult MVar18;
  
  iVar8 = (modelNodeRuntime->worldTransform).translation.x - g_ModelRaycastOriginX;
  iVar10 = (modelNodeRuntime->worldTransform).translation.y - g_ModelRaycastOriginY;
  iVar12 = (modelNodeRuntime->worldTransform).translation.z - g_ModelRaycastOriginZ;
  iVar2 = modelNodeRuntime->subtreeBoundingRadiusQ12;
  lVar4 = (longlong)iVar10 * (longlong)g_ModelRaycastWorldDirectionYQ28 +
          (longlong)iVar8 * (longlong)g_ModelRaycastWorldDirectionXQ28 +
          (longlong)iVar12 * (longlong)g_ModelRaycastWorldDirectionZQ28;
  modelNode1 = (ModelRuntimeNode *)((int)((ulonglong)lVar4 >> 0x20) << 4 | (uint)lVar4 >> 0x1c);
  if ((-iVar2 <= (int)modelNode1) && ((int)modelNode1 < g_ModelRaycastMaximumDistance + iVar2)) {
    lVar4 = (longlong)(int)modelNode1;
    lVar5 = (longlong)(int)modelNode1;
    modelNode1 = (ModelRuntimeNode *)((ulonglong)((longlong)iVar8 * (longlong)iVar8) >> 0x20);
    lVar4 = ((longlong)iVar2 * (longlong)iVar2 + lVar4 * lVar5) - (longlong)iVar8 * (longlong)iVar8;
    if (-1 < lVar4) {
      modelNode1 = (ModelRuntimeNode *)((ulonglong)((longlong)iVar10 * (longlong)iVar10) >> 0x20);
      lVar4 = lVar4 - (longlong)iVar10 * (longlong)iVar10;
      if ((-1 < lVar4) &&
         (modelNode1 = (ModelRuntimeNode *)
                       ((ulonglong)((longlong)iVar12 * (longlong)iVar12) >> 0x20),
         -1 < (int)(((int)((ulonglong)lVar4 >> 0x20) - (int)modelNode1) -
                   (uint)((uint)lVar4 < (uint)((longlong)iVar12 * (longlong)iVar12))))) {
        uVar6 = -(modelNodeRuntime->modelPayload).worldRotationAngle2;
        FixedTransform_BuildRotationBasis
                  (&g_GraphicsTransformScratchMatrix3x4,uVar6 & 0xffff,
                   (modelNodeRuntime->modelPayload).worldRotationAngle1,
                   (modelNodeRuntime->modelPayload).worldRotationAngle0 + 0x8000 + uVar6 & 0xffff);
        pMVar3 = (modelNodeRuntime->modelPayload).modelResource;
        g_GraphicsTransformScratchMatrix3x4.translation.x = 0;
        g_GraphicsTransformScratchMatrix3x4.translation.y = 0;
        g_GraphicsTransformScratchMatrix3x4.translation.z = 0;
        pMVar16 = &pMVar3->firstMeshGroupRelativeOffset;
        MVar14 = pMVar3->meshGroupCount;
        if ((*(int *)pMVar3->reservedEC_1FF == 0) || (MVar14 = MVar14 - 1, MVar14 != 0)) {
          while (MVar14 = MVar14 - 1, MVar14 != 0) {
            pMVar16 = (ModelMeshGroupRelativeOffset *)((int)pMVar16 + *pMVar16);
          }
        }
        g_ModelRaycastOriginX =
             g_ModelRaycastOriginX - (modelNodeRuntime->worldTransform).translation.x;
        g_ModelRaycastOriginY =
             g_ModelRaycastOriginY - (modelNodeRuntime->worldTransform).translation.y;
        g_ModelRaycastOriginZ =
             g_ModelRaycastOriginZ - (modelNodeRuntime->worldTransform).translation.z;
        FixedTransform_ApplyPoint
                  ((GraphicsFixedVec3 *)&g_ModelRaycastLocalOriginX,
                   (GraphicsFixedVec3 *)&g_ModelRaycastOriginX,&g_GraphicsTransformScratchMatrix3x4)
        ;
        g_ModelRaycastOriginX = g_ModelRaycastOriginX + extraout_EAX;
        g_ModelRaycastOriginY = g_ModelRaycastOriginY + extraout_ECX;
        g_ModelRaycastOriginZ = g_ModelRaycastOriginZ + extraout_EDX;
        FixedTransform_ApplyPoint
                  ((GraphicsFixedVec3 *)&g_ModelRaycastLocalDirectionXQ28,
                   (GraphicsFixedVec3 *)&g_ModelRaycastWorldDirectionXQ28,
                   &g_GraphicsTransformScratchMatrix3x4);
        triangle = (ModelRaycastTriangleDescriptor *)(pMVar16 + 8);
        QVar15 = 0x7fffffff;
        for (MVar13 = pMVar16[1]; MVar13 != 0; MVar13 = MVar13 - 1) {
          ppGVar1 = &triangle->vertex1;
          bVar17 = false;
          triangle = (ModelRaycastTriangleDescriptor *)
                     (triangle[*(int *)(triangle->reservedVertex0Metadata04_0B + 4)].
                      reservedVertex2Metadata1C_23 + 4);
          if (*ppGVar1 != (GraphicsFixedVec3 *)0x0) {
            do {
              QVar9 = ModelMesh_IntersectTriangleRayDistanceCf(triangle);
              if ((bVar17) && (QVar9 <= QVar15)) {
                QVar15 = QVar9;
              }
              bVar17 = (ModelRaycastTriangleDescriptor *)0xffffffbf < triangle;
              triangle = triangle + 1;
              MVar13 = extraout_EDX_00;
            } while (extraout_ECX_00 != 1);
          }
        }
        MVar7.nearestModelNode = modelNodeRuntime;
        MVar7.nearestDistanceQ12 = QVar15;
        modelNode1 = (ModelRuntimeNode *)0x0;
        for (dVar11 = modelNodeRuntime->childCount; dVar11 != 0; dVar11 = dVar11 - 1) {
          bVar17 = false;
          if (modelNodeRuntime->childNodes[dVar11 - 1] != (ModelRuntimeNode *)0x0) {
            MVar18 = ModelNodeRuntime_RaycastHierarchyNearestCf
                               (modelNodeRuntime->childNodes[dVar11 - 1]);
            modelNode1 = MVar18.nearestModelNode;
            dVar11 = extraout_ECX_01;
            if ((bVar17) && (MVar18.nearestDistanceQ12 < MVar7.nearestDistanceQ12)) {
              MVar7 = MVar18;
            }
          }
        }
        if (MVar7.nearestDistanceQ12 != 0x7fffffff) {
          return MVar7;
        }
      }
    }
  }
  MVar18.nearestModelNode = modelNode1;
  MVar18.nearestDistanceQ12 = 0x7fffffff;
  return MVar18;
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
undefined8
ModelNodeRuntime_InstantiateLinkedChildrenRecursiveCf
          (FactionRuntimeIndex factionIndex,GraphicsPaletteAsset *paletteAsset,
          GraphicsTextureSet *textureSet,ModelRuntimeSlot *modelRuntimeSlot,
          ModelDefinitionHierarchyNodeAddress32 definitionNode,WorldRuntimeContext *worldRuntime)

{
  ModelLinkedDefinitionListAddress32 linkedDefinitionList;
  undefined4 in_EAX;
  PckModelDefinitionIdCatalog childDefinitionId;
  ModelRuntimeSlot *modelRuntimeSlot_00;
  undefined4 extraout_ECX;
  int extraout_ECX_00;
  undefined4 in_EDX;
  int iVar1;
  ModelRuntimeAttachmentIndex attachmentIndex;
  int extraout_EDX;
  undefined1 uVar2;
  
  if (*(int *)(definitionNode + 8) != 0) {
    iVar1 = 0;
    do {
      uVar2 = 0;
      linkedDefinitionList =
           *(ModelLinkedDefinitionListAddress32 *)(definitionNode + 0xc + iVar1 * 4);
      childDefinitionId =
           ModelDefinition_SelectFactionUnlockedLinkedIdCf(factionIndex,linkedDefinitionList);
      modelRuntimeSlot_00 =
           (ModelRuntimeSlot *)
           ModelRuntimePool_RepairDeferredChild
                     (extraout_ECX,attachmentIndex,paletteAsset,textureSet,attachmentIndex,
                      childDefinitionId,modelRuntimeSlot,worldRuntime);
      if ((bool)uVar2) {
ModelNodeRuntime_InstantiateLinkedChildrenRecursive_ReturnAfterRepairOrRecursiveFailure:
        return CONCAT44(in_EDX,in_EAX);
      }
      ModelNodeRuntime_InstantiateLinkedChildrenRecursiveCf
                (factionIndex,paletteAsset,textureSet,modelRuntimeSlot_00,linkedDefinitionList,
                 worldRuntime);
      if ((bool)uVar2)
      goto ModelNodeRuntime_InstantiateLinkedChildrenRecursive_ReturnAfterRepairOrRecursiveFailure;
      iVar1 = extraout_EDX + 1;
    } while (extraout_ECX_00 != 1);
  }
  return CONCAT44(in_EDX,in_EAX);
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
void ModelRuntimeHierarchy_SetCommandTargetRecursive
               (ModelCommandTarget0 commandTarget0,ModelCommandTarget1 commandTarget1,
               ModelRuntimeNode *modelNode)

{
  dword extraout_ECX;
  dword dVar1;
  ModelCommandTarget0 extraout_EDX;
  
  dVar1 = modelNode->childCount;
  (modelNode->modelPayload).textureSet = (GraphicsTextureSet *)commandTarget1;
  (modelNode->modelPayload).paletteAsset = (GraphicsPaletteAsset *)commandTarget0;
  for (; dVar1 != 0; dVar1 = dVar1 - 1) {
    if (modelNode->childNodes[0] != (ModelRuntimeNode *)0x0) {
      ModelRuntimeHierarchy_SetCommandTargetRecursive
                (commandTarget0,commandTarget1,modelNode->childNodes[0]);
      dVar1 = extraout_ECX;
      commandTarget0 = extraout_EDX;
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
undefined8
ModelRuntimeHierarchy_ClearMatchingTargetRecursive(RuntimeToken targetRuntimeId,int *modelRuntime)

{
  undefined4 in_EAX;
  int extraout_ECX;
  undefined4 in_EDX;
  
  if (modelRuntime != (int *)0x0) {
    if ((*(int *)(*modelRuntime + 0x4c) == 0xd) && (targetRuntimeId == modelRuntime[0x1b])) {
      modelRuntime[0x1b] = 0;
    }
    if (modelRuntime[3] != 0) {
      do {
        targetRuntimeId =
             ModelRuntimeHierarchy_ClearMatchingTargetRecursive
                       (targetRuntimeId,(int *)modelRuntime[0x50]);
        modelRuntime = modelRuntime + 8;
      } while (extraout_ECX != 1);
    }
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x0051C100.
   Ownership: world/model/hierarchy.
   Purpose: Walks the model runtime hierarchy and applies flag mask 0x418 to each node whose existing runtime flags
   do not contain bit 0x08.
*/
void ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive(undefined4 contextArg,int *modelRuntime)

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
int ModelRuntimeHierarchy_SumMetric3C(int *modelRuntimeRoot)

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
ModelRuntimeAttachmentCollectionRegisterPair
ModelRuntimeHierarchy_CollectAttachmentDescriptorsRecursive
          (ModelRuntimeSlot *modelRuntime,MdlSerializedNodeHeader38 *definitionNode)

{
  uint uVar1;
  uint attachmentKind;
  int iVar2;
  MdlChildCount childCountRemaining;
  uint in_EDX;
  uint childIndex;
  ModelAttachmentTransformRecord *attachmentTransformCursor;
  undefined4 unaff_EDI;
  ModelRuntimeAttachmentCollectionRegisterPair recursiveCollectionResult;
  AssetRecordByteCount definitionAssetBase;
  
  if ((definitionNode->nodeFlags & 0xf) != 0) {
    return (ulonglong)in_EDX << 0x20;
  }
  childCountRemaining = definitionNode->childCount;
  definitionAssetBase = (definitionNode->spriteAssetReference).savedId;
  childIndex = 0;
  do {
    if (childCountRemaining == 0) {
      return CONCAT44(in_EDX,unaff_EDI);
    }
    attachmentTransformCursor =
         (ModelAttachmentTransformRecord *)
         (definitionAssetBase + *(int *)(definitionAssetBase + 0xe4));
    for (iVar2 = *(int *)(definitionAssetBase + 0xe8); iVar2 != 0; iVar2 = iVar2 + -1) {
      attachmentKind = attachmentTransformCursor->packedKindAndSelector & 0xf;
      if (((attachmentKind == 0) || (attachmentKind == 1)) &&
         (childIndex == attachmentTransformCursor->packedKindAndSelector >> 4)) {
        recursiveCollectionResult =
             ModelRuntimeHierarchy_CollectAttachmentDescriptorsRecursive
                       (modelRuntime,
                        (MdlSerializedNodeHeader38 *)
                        definitionNode->childSerializedOffsets[childIndex]);
        recursiveCollectionResult._4_4_ = (int)(recursiveCollectionResult >> 0x20);
        if (((int)recursiveCollectionResult == 0) &&
           (uVar1 = modelRuntime->attachmentCount0C, uVar1 < 6)) {
          modelRuntime->attachmentCount0C = modelRuntime->attachmentCount0C + 1;
          modelRuntime->attachments140[uVar1].sourceTransform04 = attachmentTransformCursor;
        }
        goto ModelRuntimeHierarchy_CollectAttachmentDescriptors_AdvanceAfterChildResolution;
      }
      attachmentTransformCursor = attachmentTransformCursor + 1;
    }
    recursiveCollectionResult._4_4_ = childIndex - 1;
ModelRuntimeHierarchy_CollectAttachmentDescriptors_AdvanceAfterChildResolution:
    childIndex = recursiveCollectionResult._4_4_ + 1;
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
longlong __fastcall
ModelNodeRuntime_CreateHierarchyRecursiveCf
          (undefined4 param_1,uint param_2,GraphicsPaletteAsset *paletteAsset,
          GraphicsTextureSet *textureSet,ModelRuntimeSlot *modelRuntime,
          MdlSerializedNodeHeader38 *definitionNode,WorldRuntimeContext *worldRuntime)

{
  AngleTurn32 AVar1;
  AngleTurn32 AVar2;
  ArmyRuntimeSlot *pAVar3;
  ModelResourceHitTestAndRenderView210 *pMVar4;
  Q12 QVar5;
  SpriteAssetHeader *pSVar6;
  Q12 QVar7;
  SerializedRelativeByteOffset SVar8;
  ModelRuntimeNode *modelNode1;
  uint uVar9;
  ModelRuntimeNode *modelNode2;
  int iVar10;
  dword dVar11;
  uint uVar12;
  ModelAttachmentTransformRecord *pMVar13;
  bool bVar14;
  longlong lVar15;
  
  bVar14 = false;
  if ((definitionNode->nodeFlags & 0xf) != 0) {
    return (ulonglong)param_2 << 0x20;
  }
  modelNode1 = (ModelRuntimeNode *)WorldObjectArray_AllocateFreeRecordCf(worldRuntime);
  modelNode2 = modelNode1;
  if (bVar14) {
ModelNodeRuntime_CreateHierarchyRecursive_ReturnAllocationFailure:
    return CONCAT44(param_2,modelNode2);
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
  modelNode1->textureSubresourceBaseIndex = 0;
  dVar11 = (modelRuntime->definitionOrSavedId).savedIdOrOffset;
  modelNode1->tintArgb = 0xffffffff;
  if ((*(uint *)(dVar11 + 0x68) & 0x10) != 0) {
    modelNode1->runtimeFlags = modelNode1->runtimeFlags | 0x10;
  }
  if ((*(uint *)(dVar11 + 0x68) & 0x20) != 0) {
    modelNode1->runtimeFlags = modelNode1->runtimeFlags | 0x200;
  }
  if ((*(uint *)(dVar11 + 0x68) & 0x40) == 0) {
    modelNode1->runtimeFlags = modelNode1->runtimeFlags | 0x100;
  }
  pMVar4 = (ModelResourceHitTestAndRenderView210 *)
           (definitionNode->spriteAssetReference).spriteAsset;
  (modelNode1->modelPayload).paletteAsset = paletteAsset;
  QVar5 = pMVar4->boundingRadiusQ12;
  (modelNode1->modelPayload).textureSet = textureSet;
  modelNode1->subtreeBoundingRadiusQ12 = QVar5;
  (modelNode1->modelPayload).modelResource = pMVar4;
  modelNode1->shadingRecord = (GraphicsShadingRuntimeRecord *)0x0;
  modelNode1->modelRuntimeLinkOrSavedOffset = (void *)0x0;
  modelNode1->renderDepthBiasOrState = 0;
  dVar11 = definitionNode->childCount;
  pSVar6 = (definitionNode->spriteAssetReference).spriteAsset;
  uVar12 = 0;
  modelNode1->childCount = dVar11;
  modelNode1->parentNode = (ModelRuntimeNode *)0x0;
  do {
    if (dVar11 == 0) {
      return CONCAT44(param_2,modelNode1);
    }
    pMVar13 = (ModelAttachmentTransformRecord *)
              ((pSVar6->registryHeader).common.buildMetadata.assetRelativeAddressAnchor28 +
              *(int *)(pSVar6->reservedBC_1FF + 0x28) + -0x28);
    for (iVar10 = *(int *)(pSVar6->reservedBC_1FF + 0x2c); iVar10 != 0; iVar10 = iVar10 + -1) {
      uVar9 = pMVar13->packedKindAndSelector & 0xf;
      if (((uVar9 == 0) || (uVar9 == 1)) &&
         (uVar9 = pMVar13->packedKindAndSelector >> 4, bVar14 = uVar12 < uVar9, uVar12 == uVar9)) {
        lVar15 = ModelNodeRuntime_CreateHierarchyRecursiveCf
                           (iVar10,uVar12,paletteAsset,textureSet,modelRuntime,
                            (MdlSerializedNodeHeader38 *)
                            definitionNode->childSerializedOffsets[uVar12],worldRuntime);
        uVar12 = (uint)((ulonglong)lVar15 >> 0x20);
        modelNode2 = (ModelRuntimeNode *)lVar15;
        if (bVar14) goto ModelNodeRuntime_CreateHierarchyRecursive_ReturnAllocationFailure;
        modelNode1->childNodes[uVar12] = modelNode2;
        if (modelNode2 == (ModelRuntimeNode *)0x0) {
          uVar9 = modelRuntime->attachmentCount0C;
          if (uVar9 < 6) {
            modelRuntime->attachmentCount0C = modelRuntime->attachmentCount0C + 1;
            modelRuntime->attachments140[uVar9].sourceTransform04 = pMVar13;
            modelRuntime->attachments140[uVar9].childNodeIndex0C = uVar12;
            modelRuntime->attachments140[uVar9].parentModelNodeOrSavedOffset08 = modelNode1;
            SVar8 = definitionNode->childSerializedOffsets[uVar12];
            modelRuntime->attachments140[uVar9].childModelRuntimeOrSavedOffset00 =
                 (ModelRuntimeSlot *)0x0;
            AVar1 = *(AngleTurn32 *)(SVar8 + 8);
            AVar2 = *(AngleTurn32 *)(SVar8 + 0xc);
            modelRuntime->attachments140[uVar9].childLocalRotationAngle2 =
                 *(AngleTurn32 *)(SVar8 + 0x10);
            modelRuntime->attachments140[uVar9].childLocalRotationAngle1 = AVar2;
            modelRuntime->attachments140[uVar9].childLocalRotationAngle0 = AVar1;
          }
        }
        else {
          modelNode2->parentNode = modelNode1;
          QVar5 = pMVar13->localTranslationYQ12;
          QVar7 = pMVar13->localTranslationZQ12;
          (modelNode2->modelPayload).localTranslationXQ12 = pMVar13->localTranslationXQ12;
          (modelNode2->modelPayload).localTranslationYQ12 = QVar5;
          (modelNode2->modelPayload).localTranslationZQ12 = QVar7;
        }
        goto ModelNodeRuntime_CreateHierarchyRecursive_AdvanceAfterChildResolution;
      }
      pMVar13 = pMVar13 + 1;
    }
    modelNode1->childNodes[uVar12] = (ModelRuntimeNode *)0x0;
ModelNodeRuntime_CreateHierarchyRecursive_AdvanceAfterChildResolution:
    uVar12 = uVar12 + 1;
    dVar11 = dVar11 - 1;
  } while( true );
}

/* Address: 0x005294E0.
   Ownership: world/model/hierarchy.
   Purpose: Recursively releases child pointers at +0xCC for the exact count at +0xC8, scans the parent object at
   +0xC4 for references back to the current node and clears each match, then calls the existing node-release
   helper. EAX and companion register state are preserved.
   Cross-module calls: WorldRuntime_UnlinkNodeFromOwnerListD8 [world/runtime/core].
*/
undefined8 ModelRuntimeNode_ReleaseRecursiveAndDetachParent(ModelRuntimeNode *node)

{
  undefined4 in_EAX;
  dword extraout_ECX;
  dword childrenRemaining;
  dword dVar1;
  undefined4 in_EDX;
  ModelRuntimeNode *modelNode1;
  
  modelNode1 = node;
  for (childrenRemaining = node->childCount; childrenRemaining != 0;
      childrenRemaining = childrenRemaining - 1) {
    if (modelNode1->childNodes[0] != (ModelRuntimeNode *)0x0) {
      ModelRuntimeNode_ReleaseRecursiveAndDetachParent(modelNode1->childNodes[0]);
      childrenRemaining = extraout_ECX;
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
  WorldRuntime_UnlinkNodeFromOwnerListD8((WorldRuntimeNode *)node);
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x0052A100.
   Ownership: world/model/hierarchy.
   Purpose: Recursively accumulates maximum hierarchy metrics and shot-definition category masks into the owning
   army runtime derived-selection fields.
   Cross-module calls: ShotDefinition_ComputeSelectionRange [assets/shot/catalog].
*/
undefined8 ModelRuntimeHierarchy_AccumulateDerivedSelectionMetrics(int *modelRuntime)

{
  int *piVar1;
  int iVar2;
  ShotDefinition *definition;
  undefined4 in_EAX;
  uint uVar3;
  dword dVar4;
  int iVar5;
  int extraout_ECX;
  int iVar6;
  undefined4 in_EDX;
  
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
      iVar6 = extraout_ECX;
    }
    modelRuntime = modelRuntime + 8;
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x0052A690.
   Ownership: world/model/hierarchy.
   Purpose: Recursively computes the Q12 hierarchy scale ratio consumed by the existing model-runtime metric query
   wrappers. Recursively computes the hierarchy scale ratio. EAX carries the computed Q12 ratio and EDX carries Q12
   unity 0x1000; the nominal 8-byte return type preserves the verified EDX:EAX register pair without introducing a
   structure-return pointer. Unrelated to draw scale (that is node+0xC0 in ModelRender_PrepareProjectedVertex).
*/
ModelRuntimeScaleRatioRegisterPairQ12
ModelRuntimeHierarchy_ComputeScaleRatioQ12Regs(ModelRuntimeSlot *modelRuntime)

{
  ModelRuntimeSlot *modelRuntime_00;
  dword extraout_ECX;
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
      attachmentsRemaining = extraout_ECX;
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
void ModelNodeRuntime_SmoothYawTowardTarget
               (ModelRuntimeNode *modelNodeRuntime,ArmyRuntimeSlot *smoothingState,
               AngleTurn32 targetYawAngle16)

{
  void *pvVar1;
  int iVar2;
  AngleTurn32 AVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  
  uVar4 = (modelNodeRuntime->modelPayload).localRotationAngle2;
  pvVar1 = smoothingState->definitionOrAsset;
  uVar7 = targetYawAngle16 - uVar4 & 0xffff;
  uVar5 = (smoothingState->movementControl).turnVelocityAngle16 * g_InGameSimulationStepTicks;
  if (uVar7 < 0x8001) {
    if (-1 < (int)uVar5) {
      if (uVar7 <= uVar5) {
ModelNodeRuntime_SmoothYawTowardTarget_SnapToTargetAndClearTurnVelocity:
        AVar3 = (modelNodeRuntime->modelPayload).localRotationAngle2;
        (smoothingState->movementControl).turnVelocityAngle16 = 0;
        if (targetYawAngle16 == AVar3) {
          return;
        }
        modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
        (modelNodeRuntime->modelPayload).localRotationAngle2 = targetYawAngle16;
        return;
      }
      uVar4 = uVar4 + uVar5;
      iVar2 = *(int *)((int)pvVar1 + 0x10);
      iVar6 = (smoothingState->movementControl).turnVelocityAngle16 +
              g_InGameSimulationStepTicks * *(int *)((int)pvVar1 + 0x1c);
      (smoothingState->movementControl).turnVelocityAngle16 = iVar2;
      if (iVar6 < iVar2) {
        (smoothingState->movementControl).turnVelocityAngle16 = iVar6;
      }
      goto ModelNodeRuntime_SmoothYawTowardTarget_CommitStepAndReturnToleranceCarryStatus;
    }
  }
  else if ((int)uVar5 < 1) {
    if (uVar5 + 0x10000 <= uVar7)
    goto ModelNodeRuntime_SmoothYawTowardTarget_SnapToTargetAndClearTurnVelocity;
    uVar4 = uVar4 + uVar5;
    iVar2 = *(int *)((int)pvVar1 + 0x10);
    iVar6 = (smoothingState->movementControl).turnVelocityAngle16 -
            g_InGameSimulationStepTicks * *(int *)((int)pvVar1 + 0x1c);
    (smoothingState->movementControl).turnVelocityAngle16 = -iVar2;
    if (-iVar2 < iVar6) {
      (smoothingState->movementControl).turnVelocityAngle16 = iVar6;
    }
    goto ModelNodeRuntime_SmoothYawTowardTarget_CommitStepAndReturnToleranceCarryStatus;
  }
  (smoothingState->movementControl).turnVelocityAngle16 = 0;
ModelNodeRuntime_SmoothYawTowardTarget_CommitStepAndReturnToleranceCarryStatus:
  modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
  (modelNodeRuntime->modelPayload).localRotationAngle2 = uVar4 & 0xffff;
  uVar4 = (uVar4 & 0xffff) - targetYawAngle16 & 0xffff;
  if ((0x3ff < uVar4) && (uVar4 < 0xfc01)) {
    return;
  }
  return;
}

/* Address: 0x0052AC00.
   Ownership: world/model/hierarchy.
   Purpose: Clamps the target pitch to the definition bounds, smooths the model pitch field at offset 0x28 with
   bounded acceleration and deceleration, then marks the transform dirty.
*/
void ModelNodeRuntime_SmoothPitchTowardTarget
               (ModelRuntimeNode *modelNodeRuntime,ArmyRuntimeSlot *smoothingState,
               AngleTurn32 targetPitchAngle16)

{
  void *pvVar1;
  AngleTurn32 AVar2;
  uint uVar3;
  int iVar4;
  ArmyMovementStateFlags AVar5;
  ArmyMovementStateFlags AVar6;
  
  AVar2 = (modelNodeRuntime->modelPayload).localRotationAngle1;
  pvVar1 = smoothingState->definitionOrAsset;
  if (*(int *)((int)pvVar1 + 0x28) < (int)targetPitchAngle16) {
    targetPitchAngle16 = *(AngleTurn32 *)((int)pvVar1 + 0x28);
  }
  if ((int)targetPitchAngle16 < *(int *)((int)pvVar1 + 0x24)) {
    targetPitchAngle16 = *(AngleTurn32 *)((int)pvVar1 + 0x24);
  }
  iVar4 = smoothingState->movementStateFlags * g_InGameSimulationStepTicks;
  if (targetPitchAngle16 == AVar2) {
ModelNodeRuntime_SmoothPitchTowardTarget_SnapToTargetAndClearPitchVelocity:
    AVar2 = (modelNodeRuntime->modelPayload).localRotationAngle1;
    smoothingState->movementStateFlags = 0;
    if (targetPitchAngle16 != AVar2) {
      modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
      (modelNodeRuntime->modelPayload).localRotationAngle1 = targetPitchAngle16;
    }
    return;
  }
  if ((int)targetPitchAngle16 < (int)AVar2) {
    if (iVar4 < 1) {
      if (iVar4 <= (int)(targetPitchAngle16 - AVar2))
      goto ModelNodeRuntime_SmoothPitchTowardTarget_SnapToTargetAndClearPitchVelocity;
      AVar2 = AVar2 + iVar4;
      iVar4 = *(int *)((int)pvVar1 + 0x14);
      AVar6 = smoothingState->movementStateFlags -
              g_InGameSimulationStepTicks * *(int *)((int)pvVar1 + 0x20);
      smoothingState->movementStateFlags = -iVar4;
      if (-iVar4 < (int)AVar6) {
        smoothingState->movementStateFlags = AVar6;
      }
      goto ModelNodeRuntime_SmoothPitchTowardTarget_CommitStepAndReturnToleranceCarryStatus;
    }
  }
  else if (-1 < iVar4) {
    if (iVar4 < (int)(targetPitchAngle16 - AVar2)) {
      AVar2 = AVar2 + iVar4;
      AVar6 = *(ArmyMovementStateFlags *)((int)pvVar1 + 0x14);
      AVar5 = smoothingState->movementStateFlags +
              g_InGameSimulationStepTicks * *(int *)((int)pvVar1 + 0x20);
      smoothingState->movementStateFlags = AVar6;
      if ((int)AVar5 < (int)AVar6) {
        smoothingState->movementStateFlags = AVar5;
      }
      goto ModelNodeRuntime_SmoothPitchTowardTarget_CommitStepAndReturnToleranceCarryStatus;
    }
    goto ModelNodeRuntime_SmoothPitchTowardTarget_SnapToTargetAndClearPitchVelocity;
  }
  smoothingState->movementStateFlags = 0;
ModelNodeRuntime_SmoothPitchTowardTarget_CommitStepAndReturnToleranceCarryStatus:
  modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
  (modelNodeRuntime->modelPayload).localRotationAngle1 = AVar2;
  uVar3 = AVar2 - targetPitchAngle16 & 0xffff;
  if (uVar3 < 0x400) {
    return;
  }
  if (0xfc00 < uVar3) {
    return;
  }
  return;
}

/* Address: 0x004BD1A0.
   Ownership: world/model/hierarchy.
   Purpose: Stores one packed tint on the current model runtime node and recursively applies it to every non-null
   child in the exact child pointer array.
*/
void ModelNodeRuntime_ApplyTintRecursive(PackedArgb32 tintArgb,ModelRuntimeNode *modelNode)

{
  PackedArgb32 extraout_EAX;
  dword extraout_ECX;
  dword childrenRemaining;
  
  childrenRemaining = modelNode->childCount;
  modelNode->tintArgb = tintArgb;
  for (; childrenRemaining != 0; childrenRemaining = childrenRemaining - 1) {
    if (modelNode->childNodes[0] != (ModelRuntimeNode *)0x0) {
      ModelNodeRuntime_ApplyTintRecursive(tintArgb,modelNode->childNodes[0]);
      tintArgb = extraout_EAX;
      childrenRemaining = extraout_ECX;
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
void ModelNodeRuntime_ComposeChildTransformsRecursive(ModelRuntimeNode *modelNodeRuntime)

{
  ModelRuntimeNode *modelNodeRuntime_00;
  AngleTurn32 extraout_ECX;
  GraphicsFixedMatrix3x4 *transform;
  uint childIndex;
  FixedEulerPairEdxEax8 extractedEulerAngles;
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
        extractedEulerAngles = FixedTransform_ExtractEulerAnglesRegs(transform);
        (modelNodeRuntime_00->modelPayload).worldRotationAngle2 = (AngleTurn32)extractedEulerAngles;
        (modelNodeRuntime_00->modelPayload).worldRotationAngle0 = extraout_ECX;
        (modelNodeRuntime_00->modelPayload).worldRotationAngle1 =
             (AngleTurn32)(extractedEulerAngles >> 0x20);
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
undefined8
ModelRuntimeHierarchy_ApplyFactionTechnologyVariantsRecursive
          (FactionRuntimeIndex factionIndex,int *modelRuntime)

{
  undefined4 in_EAX;
  PckModelDefinitionIdCatalog definitionId;
  ModelDefinitionRecordPrefix *modelDefinition1;
  int iVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  int iVar2;
  undefined4 in_EDX;
  dword *extraout_EDX;
  dword *factionTechnologyMasks;
  bool bVar3;
  undefined8 uVar4;
  
  iVar2 = *modelRuntime;
  iVar1 = 6;
  factionTechnologyMasks = g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits;
  while( true ) {
    bVar3 = false;
    if ((*(PckModelDefinitionIdCatalog *)(iVar2 + 0x238) != 0) &&
       (definitionId = ModelDefinition_IsFactionTechnologyUnlockedCf
                                 (factionTechnologyMasks,
                                  *(PckModelDefinitionIdCatalog *)(iVar2 + 0x238)),
       iVar1 = extraout_ECX, factionTechnologyMasks = extraout_EDX, !bVar3)) break;
    iVar2 = iVar2 + 4;
    iVar1 = iVar1 + -1;
    if (iVar1 == 0) {
ModelRuntimeHierarchy_ApplyFactionTechnologyVariantsRecursive_RecurseChildrenAfterVariantResolution:
      for (iVar2 = modelRuntime[3]; iVar2 != 0; iVar2 = iVar2 + -1) {
        if ((int *)modelRuntime[0x50] != (int *)0x0) {
          uVar4 = ModelRuntimeHierarchy_ApplyFactionTechnologyVariantsRecursive
                            (factionIndex,(int *)modelRuntime[0x50]);
          factionIndex = (FactionRuntimeIndex)((ulonglong)uVar4 >> 0x20);
          iVar2 = extraout_ECX_00;
        }
        modelRuntime = modelRuntime + 8;
      }
      return CONCAT44(in_EDX,in_EAX);
    }
  }
  modelDefinition1 = ModelDefinitionRegistry_FindByIdWithErrorCf(definitionId);
  iVar2 = *modelRuntime;
  *modelRuntime = (int)modelDefinition1;
  modelRuntime[0xf] =
       (int)(((longlong)modelRuntime[0xf] * (longlong)(int)modelDefinition1[8].byteSize) /
            (longlong)*(int *)(iVar2 + 0x60));
  ArmyRuntime_RebuildDerivedSelectionMetrics((ArmyRuntimeSlot *)modelRuntime[2]);
  goto 
  ModelRuntimeHierarchy_ApplyFactionTechnologyVariantsRecursive_RecurseChildrenAfterVariantResolution
  ;
}
