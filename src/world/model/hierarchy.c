/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/model/hierarchy.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/model/hierarchy.h>
#include <thandor/thandor.h>

static void ModelRuntimeHierarchy_ApplyFlags418From(uint8_t *node);

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
  uint8_t *clampTable;
  uint8_t clampedColorByte;
  uint8_t clampedAlphaByte;
  uint32_t flagsOrPreviousTint;
  int colorIntensity;
  PackedArgb32 tintArgb;
  int alphaIntensity;
  
  colorIntensity = 0xff;
  alphaIntensity = 0xff;
  flagsOrPreviousTint = modelNodeRuntime->runtimeFlags;
  if ((flagsOrPreviousTint & 4) == 0) {
    colorIntensity = 0;
    alphaIntensity = 0;
    if ((flagsOrPreviousTint & 8) != 0) {
      colorIntensity = 0xff;
      alphaIntensity = 0;
      if ((flagsOrPreviousTint & 0x10) == 0) {
        colorIntensity = 0x87;
        alphaIntensity = 0xff;
      }
    }
  }
  if ((flagsOrPreviousTint & 0x1000) != 0) {
    alphaIntensity = 0;
  }
  flagsOrPreviousTint = modelNodeRuntime->tintArgb;
  /* The clamp table is 64-KiB aligned: the original puts the target intensity in AL/BL and the previous
     tint byte in AH/BH, i.e. indexes it with (previous << 8) | target. */
  clampTable = (uint8_t *)g_GraphicsIntensityClampTableBase;
  clampedColorByte = clampTable[((flagsOrPreviousTint >> 0x10) & 0xff) << 8 | (uint32_t)colorIntensity];
  clampedAlphaByte = clampTable[(flagsOrPreviousTint >> 0x18) << 8 | (uint32_t)alphaIntensity];
  tintArgb = (uint32_t)clampedAlphaByte << 0x18 | (uint32_t)clampedColorByte << 0x10 | (uint32_t)clampedColorByte << 8 |
             (uint32_t)clampedColorByte;
  if (tintArgb != flagsOrPreviousTint >> 0x10) {
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
  uint32_t childrenRemaining;
  
  if (node != (ModelRuntimeNode *)0x0) {
    (node->modelPayload).textureSet = textureSet;
    (node->modelPayload).paletteAsset = paletteAsset;
    for (childrenRemaining = node->childCount; childrenRemaining != 0; childrenRemaining = childrenRemaining - 1) {
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
  ModelResourceHitTestAndRenderView210 *resourceView;
  uint32_t childrenRemaining;
  int vertexCountOrChildIndex;
  GraphicsFixedVec3 *point;
  uint8_t *geometryRecord;
  ModelPackedGeometryRecordCount geometryRecordsRemaining;
  
  resourceView = (modelNode->modelPayload).modelResource;
  if (resourceView->meshGroupCount != 0) {
    geometryRecord = resourceView[1].reserved00_AF + 0x10;
    for (geometryRecordsRemaining = resourceView->packedGeometryRecordCount; geometryRecordsRemaining != 0; geometryRecordsRemaining = geometryRecordsRemaining - 1) {
      point = (GraphicsFixedVec3 *)(geometryRecord + 0x20);
      for (vertexCountOrChildIndex = *(int *)(geometryRecord + 8); vertexCountOrChildIndex != 0; vertexCountOrChildIndex = vertexCountOrChildIndex + -1) {
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
      geometryRecord = geometryRecord + *(int *)geometryRecord;
    }
  }
  vertexCountOrChildIndex = 0;
  for (childrenRemaining = modelNode->childCount; childrenRemaining != 0; childrenRemaining = childrenRemaining - 1) {
    if (modelNode->childNodes[vertexCountOrChildIndex] != (ModelRuntimeNode *)0x0) {
      ModelNodeRuntime_AccumulateTransformedBoundsRecursive(modelNode->childNodes[vertexCountOrChildIndex]);
    }
    vertexCountOrChildIndex = vertexCountOrChildIndex + 1;
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
  uint32_t viewFacingAngle16;
  FixedVectorEaxEcxEdx12 viewRelativeVector;
  
  viewRelativeVector = FixedTransform_ApplyEulerRotationToVectorRegs
                    ((modelNodeRuntime->worldTransform).translation.z - g_ViewOriginFixed.z,
                     (modelNodeRuntime->worldTransform).translation.y - g_ViewOriginFixed.y,
                     (modelNodeRuntime->worldTransform).translation.x - g_ViewOriginFixed.x,0,
                     (modelNodeRuntime->modelPayload).worldRotationAngle1,
                     (modelNodeRuntime->modelPayload).worldRotationAngle0 - 0x8000);
  viewFacingAngle16 = FixedMath_Atan2Angle16(viewRelativeVector.yQ12,viewRelativeVector.xQ12);
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
  uint32_t angle0;
  FixedVectorAngles viewAngles;
  
  viewAngles = FixedMath_VectorToAngles3Regs
                    ((modelNodeRuntime->worldTransform).translation.z - g_ViewOriginFixed.z,
                     (modelNodeRuntime->worldTransform).translation.y - g_ViewOriginFixed.y,
                     (modelNodeRuntime->worldTransform).translation.x - g_ViewOriginFixed.x);
  angle0 = viewAngles.azimuthAngle + 0x8000 & 0xffff;
  FixedTransform_BuildRotationBasis(&modelNodeRuntime->worldTransform,angle0,-viewAngles.elevationAngle,angle0);
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
  ModelRuntimeNode *childNode;
  uint32_t childDistance;
  uint32_t childExtent;
  uint32_t childrenRemaining;
  uint32_t maximumRadius;
  ModelRuntimeNode *childSlotCursor;
  
  maximumRadius = ((modelNodeRuntime->modelPayload).modelResource)->boundingRadiusQ12;
  childSlotCursor = modelNodeRuntime;
  for (childrenRemaining = modelNodeRuntime->childCount; childrenRemaining != 0; childrenRemaining = childrenRemaining - 1) {
    childNode = childSlotCursor->childNodes[0];
    if (childNode != (ModelRuntimeNode *)0x0) {
      ModelNodeRuntime_RecomputeSubtreeBoundingRadius(childNode);
      childDistance = FixedMath_LengthVec3
                        ((GraphicsFixedVec3 *)
                         &(childNode->modelPayload).localTranslationXQ12);
      childExtent = childDistance + childNode->subtreeBoundingRadiusQ12;
      if (maximumRadius < childExtent) {
        maximumRadius = childExtent;
      }
    }
    childSlotCursor = (ModelRuntimeNode *)&(childSlotCursor->common).nextNode;
  }
  modelNodeRuntime->subtreeBoundingRadiusQ12 = maximumRadius;
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
  DepthBinMask32 binMask;
  GraphicsWorldCoordinateQ12 centerDepth;
  
  if (intervalRadiusQ14 < modelNodeRuntime->subtreeBoundingRadiusQ12) {
    intervalRadiusQ14 = modelNodeRuntime->subtreeBoundingRadiusQ12;
  }
  centerDepth = (modelNodeRuntime->worldTransform).translation.y;
  binMask = DepthInterval_BuildBinMask
                    (intervalRadiusQ14,(modelNodeRuntime->worldTransform).translation.x);
  modelNodeRuntime->depthBinMaskNear = binMask;
  binMask = DepthInterval_BuildBinMask(intervalRadiusQ14,centerDepth);
  modelNodeRuntime->depthBinMaskFar = binMask;
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
ModelWorldPoint
ModelNodeRuntime_TransformLocalPointRegs
          (ModelPackedPointRecord *localPointRecord,ModelRuntimeNode *modelNodeRuntime)

{
  ModelWorldPoint transformedPoint;
  
  FixedTransform_ApplyPoint
            ((GraphicsFixedVec3 *)&g_ModelTransformOutputX,&localPointRecord->localPosition,
             &modelNodeRuntime->worldTransform);
  transformedPoint.yQ12 = g_ModelTransformOutputY;
  transformedPoint.xQ12 = g_ModelTransformOutputX;
  transformedPoint.zQ12 = g_ModelTransformOutputZ;
  return transformedPoint;
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
  uint32_t negatedAngle2;
  FixedVectorAngles directionAngles;
  FixedVectorEaxEcxEdx12 rotatedDirection;
  ModelRelativeDirectionAnglesEaxEdx8 relativeAngles;

  negatedAngle2 = -(modelNodeRuntime->modelPayload).worldRotationAngle2;
  rotatedDirection = FixedTransform_RotateDirectionScaledRegs
                    (0x1000,elevationAngle,azimuthAngle,negatedAngle2 & 0xffff,
                     (modelNodeRuntime->modelPayload).worldRotationAngle1,
                     (modelNodeRuntime->modelPayload).worldRotationAngle0 + 0x8000 + negatedAngle2 & 0xffff)
  ;
  directionAngles = FixedMath_VectorToAngles3Regs(rotatedDirection.zQ12,rotatedDirection.yQ12,rotatedDirection.xQ12);
  relativeAngles.relativeYawAngle =
       directionAngles.azimuthAngle + (modelNodeRuntime->modelPayload).localRotationAngle2 & 0xffff;
  relativeAngles.relativePitchAngle = directionAngles.elevationAngle;
  return relativeAngles;
}


/* Address: 0x0050A7A0.
   Ownership: world/model/hierarchy.
   Purpose: CF clear returns EAX distance metric; CF set reports miss.
   Cross-module calls: FixedTransform_Compose [core/math/fixed], FixedTransform_ApplyPoint [core/math/fixed],
   Graphics_ProjectViewPoint [graphics/core/runtime], GraphicsProjectedPoint_IsInsideTriangleCf
   [graphics/render/projection], FixedMath_Length3 [core/math/fixed].
*/
ModelHitTestResult __thandor_eax_cf_preserve_ecx_edx
ModelRuntimeNode_HitTestProjectedBoundsAndChildrenCf
          (int pointerY,int pointerX,ModelRuntimeNode *modelNode,
          FrontendModelPointerContextRuntimeState118 *context)

{
  ModelResourceHitTestAndRenderView210 *resourceView;
  GraphicsWorldCoordinateQ12 boundsX1;
  ModelRuntimeNode *childNode;
  ModelHitTestResult missResult;
  uint8_t clippedCornerMask;
  GraphicsFixedMatrix3x4 *transformA;
  uint32_t childrenRemaining;
  int childByteOffset;
  bool cornerVisibleOrHit;
  GraphicsProjectedPointPair projectedCorner;
  ModelHitTestResult boundsCenterHit;
  ModelHitTestResult hitOrChildResult;
  
  resourceView = (modelNode->modelPayload).modelResource;
  transformA = &modelNode->worldTransform;
  if ((resourceView->boundingRadiusQ12 != 0) &&
     ((resourceView->hitTestFlags20C & MODEL_RESOURCE_DISABLE_PROJECTED_HIT_TEST) == 0)) {
    FixedTransform_Compose
              (&g_GraphicsTransformScratchMatrix3x4,transformA,&g_ViewProjectionMatrixFixed);
    g_GraphicsTransformInputScratchVec3.x = resourceView->localBoundsX0Q12;
    boundsX1 = resourceView->localBoundsX1Q12;
    g_GraphicsTransformInputScratchVec3.y = resourceView->localBoundsY0Q12;
    g_GraphicsTransformInputScratchVec3.z = resourceView->localBoundsZ0Q12;
    FixedTransform_ApplyPoint
              (&g_GraphicsTransformOutputScratchVec3,&g_GraphicsTransformInputScratchVec3,
               &g_GraphicsTransformScratchMatrix3x4);
    cornerVisibleOrHit = (int)g_ProjectionScaleFixed <= g_GraphicsTransformOutputScratchVec3.z;
    if (cornerVisibleOrHit) {
      projectedCorner = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
      g_ModelProjectedBoundsCornerScratch8[0].x = projectedCorner.projectedX >> 0xc;
      g_ModelProjectedBoundsCornerScratch8[0].y = projectedCorner.projectedY >> 0xc;
    }
    clippedCornerMask = !cornerVisibleOrHit;
    g_GraphicsTransformInputScratchVec3.x = boundsX1;
    FixedTransform_ApplyPoint
              (&g_GraphicsTransformOutputScratchVec3,&g_GraphicsTransformInputScratchVec3,
               &g_GraphicsTransformScratchMatrix3x4);
    if (g_GraphicsTransformOutputScratchVec3.z < (int)g_ProjectionScaleFixed) {
      clippedCornerMask = clippedCornerMask | 2;
    }
    else {
      projectedCorner = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
      g_ModelProjectedBoundsCornerScratch8[1].x = projectedCorner.projectedX >> 0xc;
      g_ModelProjectedBoundsCornerScratch8[1].y = projectedCorner.projectedY >> 0xc;
    }
    g_GraphicsTransformInputScratchVec3.x = resourceView->localBoundsX0Q12;
    g_GraphicsTransformInputScratchVec3.y = resourceView->localBoundsY1Q12;
    FixedTransform_ApplyPoint
              (&g_GraphicsTransformOutputScratchVec3,&g_GraphicsTransformInputScratchVec3,
               &g_GraphicsTransformScratchMatrix3x4);
    if (g_GraphicsTransformOutputScratchVec3.z < (int)g_ProjectionScaleFixed) {
      clippedCornerMask = clippedCornerMask | 4;
    }
    else {
      projectedCorner = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
      g_ModelProjectedBoundsCornerScratch8[2].x = projectedCorner.projectedX >> 0xc;
      g_ModelProjectedBoundsCornerScratch8[2].y = projectedCorner.projectedY >> 0xc;
    }
    g_GraphicsTransformInputScratchVec3.x = boundsX1;
    FixedTransform_ApplyPoint
              (&g_GraphicsTransformOutputScratchVec3,&g_GraphicsTransformInputScratchVec3,
               &g_GraphicsTransformScratchMatrix3x4);
    if (g_GraphicsTransformOutputScratchVec3.z < (int)g_ProjectionScaleFixed) {
      clippedCornerMask = clippedCornerMask | 8;
    }
    else {
      projectedCorner = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
      g_ModelProjectedBoundsCornerScratch8[3].x = projectedCorner.projectedX >> 0xc;
      g_ModelProjectedBoundsCornerScratch8[3].y = projectedCorner.projectedY >> 0xc;
    }
    g_GraphicsTransformInputScratchVec3.x = resourceView->localBoundsX0Q12;
    g_GraphicsTransformInputScratchVec3.y = resourceView->localBoundsY0Q12;
    g_GraphicsTransformInputScratchVec3.z = resourceView->localBoundsZ1Q12;
    FixedTransform_ApplyPoint
              (&g_GraphicsTransformOutputScratchVec3,&g_GraphicsTransformInputScratchVec3,
               &g_GraphicsTransformScratchMatrix3x4);
    if (g_GraphicsTransformOutputScratchVec3.z < (int)g_ProjectionScaleFixed) {
      clippedCornerMask = clippedCornerMask | 0x10;
    }
    else {
      projectedCorner = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
      g_ModelProjectedBoundsCornerScratch8[4].x = projectedCorner.projectedX >> 0xc;
      g_ModelProjectedBoundsCornerScratch8[4].y = projectedCorner.projectedY >> 0xc;
    }
    g_GraphicsTransformInputScratchVec3.x = boundsX1;
    FixedTransform_ApplyPoint
              (&g_GraphicsTransformOutputScratchVec3,&g_GraphicsTransformInputScratchVec3,
               &g_GraphicsTransformScratchMatrix3x4);
    if (g_GraphicsTransformOutputScratchVec3.z < (int)g_ProjectionScaleFixed) {
      clippedCornerMask = clippedCornerMask | 0x20;
    }
    else {
      projectedCorner = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
      g_ModelProjectedBoundsCornerScratch8[5].x = projectedCorner.projectedX >> 0xc;
      g_ModelProjectedBoundsCornerScratch8[5].y = projectedCorner.projectedY >> 0xc;
    }
    transformA = (GraphicsFixedMatrix3x4 *)resourceView->localBoundsX0Q12;
    g_GraphicsTransformInputScratchVec3.y = resourceView->localBoundsY1Q12;
    g_GraphicsTransformInputScratchVec3.x = (GraphicsWorldCoordinateQ12)transformA;
    FixedTransform_ApplyPoint
              (&g_GraphicsTransformOutputScratchVec3,&g_GraphicsTransformInputScratchVec3,
               &g_GraphicsTransformScratchMatrix3x4);
    if (g_GraphicsTransformOutputScratchVec3.z < (int)g_ProjectionScaleFixed) {
      clippedCornerMask = clippedCornerMask | 0x40;
    }
    else {
      projectedCorner = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
      transformA = (GraphicsFixedMatrix3x4 *)(projectedCorner.projectedX >> 0xc);
      g_ModelProjectedBoundsCornerScratch8[6].y = projectedCorner.projectedY >> 0xc;
      g_ModelProjectedBoundsCornerScratch8[6].x = (GraphicsProjectedCoordinate)transformA;
    }
    g_GraphicsTransformInputScratchVec3.x = boundsX1;
    FixedTransform_ApplyPoint
              (&g_GraphicsTransformOutputScratchVec3,&g_GraphicsTransformInputScratchVec3,
               &g_GraphicsTransformScratchMatrix3x4);
    if (g_GraphicsTransformOutputScratchVec3.z < (int)g_ProjectionScaleFixed) {
      clippedCornerMask = clippedCornerMask | 0x80;
    }
    else {
      projectedCorner = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
      transformA = (GraphicsFixedMatrix3x4 *)(projectedCorner.projectedX >> 0xc);
      g_ModelProjectedBoundsCornerScratch8[7].y = projectedCorner.projectedY >> 0xc;
      g_ModelProjectedBoundsCornerScratch8[7].x = (GraphicsProjectedCoordinate)transformA;
    }
    if (((((((clippedCornerMask & 7) == 0) &&
           (cornerVisibleOrHit = GraphicsProjectedPoint_IsInsideTriangleCf
                              (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 2,
                               g_ModelProjectedBoundsCornerScratch8 + 1,
                               g_ModelProjectedBoundsCornerScratch8), cornerVisibleOrHit)) ||
          (((clippedCornerMask & 0xe) == 0 &&
           (cornerVisibleOrHit = GraphicsProjectedPoint_IsInsideTriangleCf
                              (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 2,
                               g_ModelProjectedBoundsCornerScratch8 + 1,
                               g_ModelProjectedBoundsCornerScratch8 + 3), cornerVisibleOrHit)))) ||
         (((clippedCornerMask & 0x70) == 0 &&
          (cornerVisibleOrHit = GraphicsProjectedPoint_IsInsideTriangleCf
                             (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 6,
                              g_ModelProjectedBoundsCornerScratch8 + 5,
                              g_ModelProjectedBoundsCornerScratch8 + 4), cornerVisibleOrHit)))) ||
        (((((clippedCornerMask & 0xe0) == 0 &&
           (cornerVisibleOrHit = GraphicsProjectedPoint_IsInsideTriangleCf
                              (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 6,
                               g_ModelProjectedBoundsCornerScratch8 + 5,
                               g_ModelProjectedBoundsCornerScratch8 + 7), cornerVisibleOrHit)) ||
          ((((clippedCornerMask & 0x15) == 0 &&
            (cornerVisibleOrHit = GraphicsProjectedPoint_IsInsideTriangleCf
                               (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 2,
                                g_ModelProjectedBoundsCornerScratch8 + 4,
                                g_ModelProjectedBoundsCornerScratch8), cornerVisibleOrHit)) ||
           (((clippedCornerMask & 0x54) == 0 &&
            (cornerVisibleOrHit = GraphicsProjectedPoint_IsInsideTriangleCf
                               (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 2,
                                g_ModelProjectedBoundsCornerScratch8 + 6,
                                g_ModelProjectedBoundsCornerScratch8 + 4), cornerVisibleOrHit)))))) ||
         ((((clippedCornerMask & 0x2a) == 0 &&
           (cornerVisibleOrHit = GraphicsProjectedPoint_IsInsideTriangleCf
                              (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 3,
                               g_ModelProjectedBoundsCornerScratch8 + 5,
                               g_ModelProjectedBoundsCornerScratch8 + 1), cornerVisibleOrHit)) ||
          (((clippedCornerMask & 0xa8) == 0 &&
           (cornerVisibleOrHit = GraphicsProjectedPoint_IsInsideTriangleCf
                              (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 3,
                               g_ModelProjectedBoundsCornerScratch8 + 7,
                               g_ModelProjectedBoundsCornerScratch8 + 5), cornerVisibleOrHit)))))))) ||
       (((((clippedCornerMask & 0x13) == 0 &&
          (cornerVisibleOrHit = GraphicsProjectedPoint_IsInsideTriangleCf
                             (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 4,
                              g_ModelProjectedBoundsCornerScratch8 + 1,
                              g_ModelProjectedBoundsCornerScratch8), cornerVisibleOrHit)) ||
         (((clippedCornerMask & 0x32) == 0 &&
          (cornerVisibleOrHit = GraphicsProjectedPoint_IsInsideTriangleCf
                             (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 1,
                              g_ModelProjectedBoundsCornerScratch8 + 5,
                              g_ModelProjectedBoundsCornerScratch8 + 4), cornerVisibleOrHit)))) ||
        ((((clippedCornerMask & 0x4c) == 0 &&
          (cornerVisibleOrHit = GraphicsProjectedPoint_IsInsideTriangleCf
                             (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 3,
                              g_ModelProjectedBoundsCornerScratch8 + 6,
                              g_ModelProjectedBoundsCornerScratch8 + 2), cornerVisibleOrHit)) ||
         (((clippedCornerMask & 200) == 0 &&
          (cornerVisibleOrHit = GraphicsProjectedPoint_IsInsideTriangleCf
                             (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 3,
                              g_ModelProjectedBoundsCornerScratch8 + 6,
                              g_ModelProjectedBoundsCornerScratch8 + 7), cornerVisibleOrHit)))))))) {
      if ((context->contextFlags & 0x80000) != 0) {
        boundsCenterHit.distanceQ12 =
             FixedMath_Length3(((resourceView->localBoundsZ0Q12 + resourceView->localBoundsZ1Q12 >> 1) +
                               (modelNode->worldTransform).translation.z) -
                               context->hitReferenceWorldZQ12,
                               ((resourceView->localBoundsY0Q12 + resourceView->localBoundsY1Q12 >> 1) +
                               (modelNode->worldTransform).translation.y) -
                               context->hitReferenceWorldYQ12,
                               ((resourceView->localBoundsX0Q12 + resourceView->localBoundsX1Q12 >> 1) +
                               (modelNode->worldTransform).translation.x) -
                               context->hitReferenceWorldXQ12);
        boundsCenterHit.missed = false;
        return boundsCenterHit;
      }
      hitOrChildResult.distanceQ12 =
           FixedMath_Length3((modelNode->worldTransform).translation.z -
                             context->hitReferenceWorldZQ12,
                             (modelNode->worldTransform).translation.y -
                             context->hitReferenceWorldYQ12,
                             (modelNode->worldTransform).translation.x -
                             context->hitReferenceWorldXQ12);
      hitOrChildResult.missed = false;
      return hitOrChildResult;
    }
  }
  childrenRemaining = modelNode->childCount;
  childByteOffset = 0;
  do {
    if (childrenRemaining == 0) {
      missResult.missed = true;
      missResult.distanceQ12 = (uint32_t)transformA;
      return missResult;
    }
    childNode = *(ModelRuntimeNode **)((int)modelNode->childNodes + childByteOffset);
    if (childNode != (ModelRuntimeNode *)0x0) {
      hitOrChildResult = ModelRuntimeNode_HitTestProjectedBoundsAndChildrenCf
                         (pointerY,pointerX,childNode,context);
      transformA = (GraphicsFixedMatrix3x4 *)hitOrChildResult.distanceQ12;
      if (!hitOrChildResult.missed) {
        return hitOrChildResult;
      }
    }
    childByteOffset = childByteOffset + 4;
    childrenRemaining = childrenRemaining - 1;
  } while( true );
}


/* Address: 0x0050B1D0.
   Ownership: world/model/hierarchy.
   Purpose: Returns nearest Q12 ray distance with CF set. EDX is the nearest ModelRuntimeNode side channel on
   success; CF clear returns 0x7fffffff.
   Cross-module calls: FixedTransform_BuildRotationBasis [core/math/fixed], FixedTransform_ApplyPoint
   [core/math/fixed], ModelMesh_IntersectTriangleRayDistanceCf [assets/model/definitions].
*/
ModelRaycastResult __thandor_eax_edx_cf_preserve_ecx
ModelNodeRuntime_RaycastHierarchyNearestCf(ModelRuntimeNode *modelNodeRuntime)

{
  GraphicsFixedVec3 **triangleCountField;
  ModelResourceHitTestAndRenderView210 *resourceView;
  int64_t projectionOrDiscriminant;
  int64_t projectedDistanceWide;
  uint32_t negatedAngle2;
  int deltaXOrNodeY;
  int deltaYOrNodeZ;
  GraphicsFixedVec3 *trianglesRemaining;
  uint32_t childrenRemaining;
  int deltaZ;
  ModelRaycastNearestNodeOrScratch4 edxCarrier;
  ModelPackedGeometryRecordCount meshRecordsRemaining;
  ModelMeshGroupCount meshGroupsRemaining;
  int radiusNodeXOrNearest;
  ModelMeshGroupRelativeOffset *meshGroupCursor;
  ModelRaycastTriangleDescriptor *triangle;
  ModelRuntimeNode *nearestModelNode;
  MeshRayTriangleResult triangleHit;
  ModelRaycastResult childOrNearestHit;
  ModelRaycastResult missResult;
  
  deltaXOrNodeY = (modelNodeRuntime->worldTransform).translation.x - g_ModelRaycastOriginX;
  deltaYOrNodeZ = (modelNodeRuntime->worldTransform).translation.y - g_ModelRaycastOriginY;
  deltaZ = (modelNodeRuntime->worldTransform).translation.z - g_ModelRaycastOriginZ;
  radiusNodeXOrNearest = modelNodeRuntime->subtreeBoundingRadiusQ12;
  projectionOrDiscriminant = (int64_t)deltaYOrNodeZ * (int64_t)(int)g_ModelRaycastWorldDirectionYQ28 +
          (int64_t)deltaXOrNodeY * (int64_t)(int)g_ModelRaycastWorldDirectionXQ28 +
          (int64_t)deltaZ * (int64_t)(int)g_ModelRaycastWorldDirectionZQ28;
  edxCarrier.scratchSigned = (int)((uint64_t)projectionOrDiscriminant >> 0x20) << 4 | (uint32_t)projectionOrDiscriminant >> 0x1c;
  if ((-radiusNodeXOrNearest <= edxCarrier.scratchSigned) &&
     (edxCarrier.scratchSigned < g_ModelRaycastMaximumDistance + radiusNodeXOrNearest)) {
    projectionOrDiscriminant = (int64_t)edxCarrier.scratchSigned;
    projectedDistanceWide = (int64_t)edxCarrier.scratchSigned;
    edxCarrier.scratchSigned = (int)((uint64_t)((int64_t)deltaXOrNodeY * (int64_t)deltaXOrNodeY) >> 0x20);
    projectionOrDiscriminant = ((int64_t)radiusNodeXOrNearest * (int64_t)radiusNodeXOrNearest + projectionOrDiscriminant * projectedDistanceWide) -
            (int64_t)deltaXOrNodeY * (int64_t)deltaXOrNodeY;
    if (-1 < projectionOrDiscriminant) {
      edxCarrier.scratchSigned = (int)((uint64_t)((int64_t)deltaYOrNodeZ * (int64_t)deltaYOrNodeZ) >> 0x20);
      projectionOrDiscriminant = projectionOrDiscriminant - (int64_t)deltaYOrNodeZ * (int64_t)deltaYOrNodeZ;
      if ((-1 < projectionOrDiscriminant) &&
         (edxCarrier.scratchSigned = (int)((uint64_t)((int64_t)deltaZ * (int64_t)deltaZ) >> 0x20)
         , -1 < (int)(((int)((uint64_t)projectionOrDiscriminant >> 0x20) - edxCarrier.scratchSigned) -
                     (uint32_t)((uint32_t)projectionOrDiscriminant < (uint32_t)((int64_t)deltaZ * (int64_t)deltaZ))))) {
        negatedAngle2 = -(modelNodeRuntime->modelPayload).worldRotationAngle2;
        FixedTransform_BuildRotationBasis
                  (&g_GraphicsTransformScratchMatrix3x4,negatedAngle2 & 0xffff,
                   (modelNodeRuntime->modelPayload).worldRotationAngle1,
                   (modelNodeRuntime->modelPayload).worldRotationAngle0 + 0x8000 + negatedAngle2 & 0xffff);
        resourceView = (modelNodeRuntime->modelPayload).modelResource;
        g_GraphicsTransformScratchMatrix3x4.translation.x = 0;
        g_GraphicsTransformScratchMatrix3x4.translation.y = 0;
        g_GraphicsTransformScratchMatrix3x4.translation.z = 0;
        meshGroupCursor = &resourceView->firstMeshGroupRelativeOffset;
        radiusNodeXOrNearest = (modelNodeRuntime->worldTransform).translation.x;
        deltaXOrNodeY = (modelNodeRuntime->worldTransform).translation.y;
        meshGroupsRemaining = resourceView->meshGroupCount;
        deltaYOrNodeZ = (modelNodeRuntime->worldTransform).translation.z;
        if ((*(int *)resourceView->reservedEC_1FF == 0) || (meshGroupsRemaining = meshGroupsRemaining - 1, meshGroupsRemaining != 0)) {
          while (meshGroupsRemaining = meshGroupsRemaining - 1, meshGroupsRemaining != 0) {
            meshGroupCursor = (ModelMeshGroupRelativeOffset *)((int)meshGroupCursor + *meshGroupCursor);
          }
        }
        g_ModelRaycastOriginX = g_ModelRaycastOriginX - radiusNodeXOrNearest;
        g_ModelRaycastOriginY = g_ModelRaycastOriginY - deltaXOrNodeY;
        g_ModelRaycastOriginZ = g_ModelRaycastOriginZ - deltaYOrNodeZ;
        FixedTransform_ApplyPoint
                  ((GraphicsFixedVec3 *)&g_ModelRaycastLocalOriginX,
                   (GraphicsFixedVec3 *)&g_ModelRaycastOriginX,&g_GraphicsTransformScratchMatrix3x4)
        ;
        g_ModelRaycastOriginX = g_ModelRaycastOriginX + radiusNodeXOrNearest;
        g_ModelRaycastOriginY = g_ModelRaycastOriginY + deltaXOrNodeY;
        g_ModelRaycastOriginZ = g_ModelRaycastOriginZ + deltaYOrNodeZ;
        FixedTransform_ApplyPoint
                  ((GraphicsFixedVec3 *)&g_ModelRaycastLocalDirectionXQ28,
                   (GraphicsFixedVec3 *)&g_ModelRaycastWorldDirectionXQ28,
                   &g_GraphicsTransformScratchMatrix3x4);
        triangle = (ModelRaycastTriangleDescriptor *)(meshGroupCursor + 8);
        radiusNodeXOrNearest = 0x7fffffff;
        for (meshRecordsRemaining = meshGroupCursor[1]; meshRecordsRemaining != 0; meshRecordsRemaining = meshRecordsRemaining - 1) {
          triangleCountField = &triangle->vertex1;
          triangle = (ModelRaycastTriangleDescriptor *)
                     (triangle[*(int *)(triangle->reservedVertex0Metadata04_0B + 4)].
                      reservedVertex2Metadata1C_23 + 4);
          for (trianglesRemaining = *triangleCountField; trianglesRemaining != (GraphicsFixedVec3 *)0x0;
              trianglesRemaining = (GraphicsFixedVec3 *)((int)&trianglesRemaining[-1].z + 3)) {
            triangleHit = ModelMesh_IntersectTriangleRayDistanceCf(triangle);
            if ((triangleHit.hit) && (triangleHit.distanceQ12 <= radiusNodeXOrNearest)) {
              radiusNodeXOrNearest = triangleHit.distanceQ12;
            }
            triangle = triangle + 1;
          }
        }
        edxCarrier.nearestModelNode = (ModelRuntimeNode *)0x0;
        nearestModelNode = modelNodeRuntime;
        for (childrenRemaining = modelNodeRuntime->childCount; childrenRemaining != 0; childrenRemaining = childrenRemaining - 1) {
          if (modelNodeRuntime->childNodes[childrenRemaining - 1] != (ModelRuntimeNode *)0x0) {
            childOrNearestHit = ModelNodeRuntime_RaycastHierarchyNearestCf
                               (modelNodeRuntime->childNodes[childrenRemaining - 1]);
            edxCarrier = childOrNearestHit.nearestNodeOrScratch;
            if ((childOrNearestHit.hit) && (childOrNearestHit.nearestDistanceQ12 < radiusNodeXOrNearest)) {
              radiusNodeXOrNearest = childOrNearestHit.nearestDistanceQ12;
              nearestModelNode = edxCarrier.nearestModelNode;
            }
          }
        }
        if (radiusNodeXOrNearest != 0x7fffffff) {
          childOrNearestHit.nearestNodeOrScratch.nearestModelNode = nearestModelNode;
          childOrNearestHit.nearestDistanceQ12 = radiusNodeXOrNearest;
          childOrNearestHit.hit = true;
          return childOrNearestHit;
        }
      }
    }
  }
  missResult.nearestNodeOrScratch.nearestModelNode = edxCarrier.nearestModelNode;
  missResult.nearestDistanceQ12 = 0x7fffffff;
  missResult.hit = false;
  return missResult;
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
  int linksRemaining;
  ModelRuntimeAttachmentIndex childSlotIndex;
  ModelRuntimeAttachmentIndex attachmentIndex;
  bool childFailed;
  ModelNodeCreateResult repairResult;
  
  linksRemaining = *(int *)(definitionNode + 8);
  if (linksRemaining != 0) {
    childSlotIndex = 0;
    do {
      linkedDefinitionList =
           *(ModelLinkedDefinitionListAddress32 *)(definitionNode + 0xc + childSlotIndex * 4);
      childDefinitionId =
           ModelDefinition_SelectFactionUnlockedLinkedIdCf(factionIndex,linkedDefinitionList);
      repairResult = ModelRuntimePool_RepairDeferredChild
                        (paletteAsset,textureSet,childSlotIndex,childDefinitionId,
                         modelRuntimeSlot,worldRuntime);
      if (repairResult.failed) {
        return true;
      }
      childFailed = ModelNodeRuntime_InstantiateLinkedChildrenRecursiveCf
                        (factionIndex,paletteAsset,textureSet,(ModelRuntimeSlot *)repairResult.modelNode,
                         linkedDefinitionList,worldRuntime);
      if (childFailed) {
        return true;
      }
      childSlotIndex = childSlotIndex + 1;
      linksRemaining = linksRemaining + -1;
    } while (linksRemaining != 0);
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
  uint32_t childrenRemaining;
  
  childrenRemaining = modelNode->childCount;
  (modelNode->modelPayload).textureSet = textureSet;
  (modelNode->modelPayload).paletteAsset = paletteAsset;
  for (; childrenRemaining != 0; childrenRemaining = childrenRemaining - 1) {
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
  int childrenRemaining;
  
  if (modelRuntime != (int *)0x0) {
    childrenRemaining = modelRuntime[3];
    if ((*(int *)(*modelRuntime + 0x4c) == 0xd) && (targetRuntimeId == modelRuntime[0x1b])) {
      modelRuntime[0x1b] = 0;
    }
    for (; childrenRemaining != 0; childrenRemaining = childrenRemaining + -1) {
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
  /* Rewritten from the assembly (0x0051C100-0x0051C162). */
  (void)contextArg;
  ModelRuntimeHierarchy_ApplyFlags418From((uint8_t *)(uintptr_t)*modelRuntime);
}


/* Address: 0x0051C1F0.
   Ownership: world/model/hierarchy.
   Purpose: Traverses the model runtime hierarchy and sums the signed dword stored at runtime-node offset 0x3C.
*/
/* Model runtime nodes keep their child count at +0x0C and child pointers at +0x140 + 32*i (null
   slots are skipped); the original walks this tree depth-first with frames on the machine stack. */
static int ModelRuntimeHierarchy_SumMetric3CFrom(uint8_t *node)
{
  int sum = *(int *)(node + 0x3c);
  int childCount = *(int *)(node + 0xc);
  int i;
  for (i = 0; i < childCount; i++) {
    uint8_t *child = *(uint8_t **)(node + 0x140 + i * 0x20);
    if (child != (uint8_t *)0x0) {
      sum = sum + ModelRuntimeHierarchy_SumMetric3CFrom(child);
    }
  }
  return sum;
}

static void ModelRuntimeHierarchy_ApplyFlags418From(uint8_t *node)
{
  int childCount;
  int i;
  if ((*(uint32_t *)(node + 0xec) & 8) == 0) {
    *(uint32_t *)(node + 0xec) = *(uint32_t *)(node + 0xec) | 0x418;
  }
  childCount = *(int *)(node + 0xc);
  for (i = 0; i < childCount; i++) {
    uint8_t *child = *(uint8_t **)(node + 0x140 + i * 0x20);
    if (child != (uint8_t *)0x0) {
      ModelRuntimeHierarchy_ApplyFlags418From(child);
    }
  }
}

int __thandor_eax_preserve_ecx_edx ModelRuntimeHierarchy_SumMetric3C(int *modelRuntimeRoot)

{
  /* Rewritten from the assembly (0x0051C1F0-0x0051C23F). */
  return ModelRuntimeHierarchy_SumMetric3CFrom((uint8_t *)(uintptr_t)*modelRuntimeRoot);
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
  uint32_t attachmentSlot;
  uint32_t attachmentKind;
  int transformRecordsRemaining;
  MdlChildCount childCountRemaining;
  uint32_t childIndex;
  ModelAttachmentTransformRecord *attachmentTransformCursor;
  ModelRuntimeAttachmentCollectionRegisterPair recursiveCollectionResult;
  AssetRecordByteCount definitionAssetBase;
  
  if ((definitionNode->nodeFlags & 0xf) != 0) {
    return (ModelRuntimeSlot *)0x0;
  }
  childCountRemaining = definitionNode->childCount;
  definitionAssetBase = (definitionNode->spriteAssetReference).savedId;
  childIndex = 0;
  for (; childCountRemaining != 0; childCountRemaining = childCountRemaining - 1) {
    attachmentTransformCursor =
         (ModelAttachmentTransformRecord *)
         (definitionAssetBase + *(int *)(definitionAssetBase + 0xe4));
    for (transformRecordsRemaining = *(int *)(definitionAssetBase + 0xe8); transformRecordsRemaining != 0; transformRecordsRemaining = transformRecordsRemaining + -1) {
      attachmentKind = attachmentTransformCursor->packedKindAndSelector & 0xf;
      if (((attachmentKind == 0) || (attachmentKind == 1)) &&
         (childIndex == attachmentTransformCursor->packedKindAndSelector >> 4)) {
        THANDOR_PART(uint32_t, recursiveCollectionResult, 0) =
             ModelRuntimeHierarchy_CollectAttachmentDescriptorsRecursive
                       (modelRuntimeContinuityEdi,modelRuntime,
                        (MdlSerializedNodeHeader38 *)
                        definitionNode->childSerializedOffsets[childIndex]);
        if (((ModelRuntimeSlot *)recursiveCollectionResult == (ModelRuntimeSlot *)0x0) &&
           (attachmentSlot = modelRuntime->attachmentCount0C, attachmentSlot < 6)) {
          modelRuntime->attachmentCount0C = modelRuntime->attachmentCount0C + 1;
          modelRuntime->attachments140[attachmentSlot].sourceTransform04 = attachmentTransformCursor;
        }
        break;
      }
      attachmentTransformCursor = attachmentTransformCursor + 1;
    }
    /* The original advances the child index only when an attachment transform matched
       (DEC EDX before the shared INC EDX when none did). */
    if (transformRecordsRemaining != 0) {
      childIndex = childIndex + 1;
    }
  }
  return modelRuntimeContinuityEdi;
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
ModelNodeCreateResult __thandor_eax_cf_preserve_ecx_edx
ModelNodeRuntime_CreateHierarchyRecursiveCf
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,
          ModelRuntimeSlot *modelRuntime,MdlSerializedNodeHeader38 *definitionNode,
          WorldRuntimeContext *worldRuntime)

{
  AngleTurn32 rotationAngleA;
  AngleTurn32 rotationAngleB;
  ArmyRuntimeSlot *ownerArmy;
  ModelResourceHitTestAndRenderView210 *resourceView;
  Q12 radiusOrTranslationY;
  Q12 translationZ;
  SerializedRelativeByteOffset childDefinitionOffset;
  ModelRuntimeNode *newNode;
  uint32_t attachmentKindOrSlot;
  ModelRuntimeNode *childOrFailedNode;
  ModelPackedLookupTableEntryCount transformRecordsRemaining;
  uint32_t definitionOrChildrenRemaining;
  uint32_t childIndex;
  ModelAttachmentTransformRecord *attachmentTransform;
  WorldObjectAllocResult allocationResult;
  ModelNodeCreateResult childResult;
  ModelNodeCreateResult failureResult;
  
  if ((definitionNode->nodeFlags & 0xf) != 0) {
    childResult.modelNode = (ModelRuntimeNode *)0x0;
    childResult.failed = false;
    return childResult;
  }
  allocationResult = WorldObjectArray_AllocateFreeRecordCf(worldRuntime);
  newNode = (ModelRuntimeNode *)allocationResult.recordOrError;
  childOrFailedNode = newNode;
  if (allocationResult.failed) {
    failureResult.failed = true;
    failureResult.modelNode = childOrFailedNode;
    return failureResult;
  }
  newNode->ownerClassId = WORLD_OWNER_RUNTIME_MODEL;
  (newNode->modelPayload).localTranslationXQ12 = 0;
  (newNode->modelPayload).localTranslationYQ12 = 0;
  (newNode->modelPayload).localTranslationZQ12 = 0;
  rotationAngleA = definitionNode->localRotationAngle1;
  rotationAngleB = definitionNode->localRotationAngle2;
  (newNode->modelPayload).localRotationAngle0 = definitionNode->localRotationAngle0;
  (newNode->modelPayload).localRotationAngle1 = rotationAngleA;
  (newNode->modelPayload).localRotationAngle2 = rotationAngleB;
  (newNode->modelPayload).meshGroupMask = 0xffffffff;
  ownerArmy = (modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime;
  (newNode->runtimePayload).modelRuntime = modelRuntime;
  newNode->runtimeFlags = newNode->runtimeFlags | 1;
  if (ownerArmy->factionIndex != 0) {
    newNode->runtimeFlags = newNode->runtimeFlags | 0x20;
  }
  *(uint8_t *)&newNode->textureSubresourceBaseIndex = 0;
  *(uint8_t *)((int)&newNode->textureSubresourceBaseIndex + 1) = 0;
  *(uint8_t *)((int)&newNode->textureSubresourceBaseIndex + 2) = 0;
  *(uint8_t *)((int)&newNode->textureSubresourceBaseIndex + 3) = 0;
  definitionOrChildrenRemaining = (modelRuntime->definitionOrSavedId).savedIdOrOffset;
  newNode->tintArgb = 0xffffffff;
  if ((*(uint32_t *)(definitionOrChildrenRemaining + 0x68) & 0x10) != 0) {
    newNode->runtimeFlags = newNode->runtimeFlags | 0x10;
  }
  if ((*(uint32_t *)(definitionOrChildrenRemaining + 0x68) & 0x20) != 0) {
    newNode->runtimeFlags = newNode->runtimeFlags | 0x200;
  }
  if ((*(uint32_t *)(definitionOrChildrenRemaining + 0x68) & 0x40) == 0) {
    newNode->runtimeFlags = newNode->runtimeFlags | 0x100;
  }
  resourceView = (definitionNode->spriteAssetReference).modelResource;
  (newNode->modelPayload).paletteAsset = paletteAsset;
  radiusOrTranslationY = resourceView->boundingRadiusQ12;
  (newNode->modelPayload).textureSet = textureSet;
  newNode->subtreeBoundingRadiusQ12 = radiusOrTranslationY;
  (newNode->modelPayload).modelResource = resourceView;
  newNode->shadingRecord = (GraphicsShadingRuntimeRecord *)0x0;
  newNode->modelRuntimeLinkOrSavedOffset = (void *)0x0;
  newNode->renderDepthBiasOrState = 0;
  definitionOrChildrenRemaining = definitionNode->childCount;
  resourceView = (definitionNode->spriteAssetReference).modelResource;
  childIndex = 0;
  newNode->childCount = definitionOrChildrenRemaining;
  newNode->parentNode = (ModelRuntimeNode *)0x0;
  for (; definitionOrChildrenRemaining != 0;
      definitionOrChildrenRemaining = definitionOrChildrenRemaining - 1) {
    attachmentTransform = (ModelAttachmentTransformRecord *)
              (resourceView->reserved00_AF + resourceView->packedLookupTableRelativeOffset);
    for (transformRecordsRemaining = resourceView->packedLookupTableEntryCount; transformRecordsRemaining != 0; transformRecordsRemaining = transformRecordsRemaining - 1) {
      attachmentKindOrSlot = attachmentTransform->packedKindAndSelector & 0xf;
      if (((attachmentKindOrSlot == 0) || (attachmentKindOrSlot == 1)) && (childIndex == attachmentTransform->packedKindAndSelector >> 4)) {
        childResult = ModelNodeRuntime_CreateHierarchyRecursiveCf
                           (paletteAsset,textureSet,modelRuntime,
                            (MdlSerializedNodeHeader38 *)
                            definitionNode->childSerializedOffsets[childIndex],worldRuntime);
        childOrFailedNode = childResult.modelNode;
        if (childResult.failed) {
          failureResult.failed = true;
          failureResult.modelNode = childOrFailedNode;
          return failureResult;
        }
        newNode->childNodes[childIndex] = childOrFailedNode;
        if (childOrFailedNode == (ModelRuntimeNode *)0x0) {
          attachmentKindOrSlot = modelRuntime->attachmentCount0C;
          if (attachmentKindOrSlot < 6) {
            modelRuntime->attachmentCount0C = modelRuntime->attachmentCount0C + 1;
            modelRuntime->attachments140[attachmentKindOrSlot].sourceTransform04 = attachmentTransform;
            modelRuntime->attachments140[attachmentKindOrSlot].childNodeIndex0C = childIndex;
            modelRuntime->attachments140[attachmentKindOrSlot].parentModelNodeOrSavedOffset08 = newNode;
            childDefinitionOffset = definitionNode->childSerializedOffsets[childIndex];
            modelRuntime->attachments140[attachmentKindOrSlot].childModelRuntimeOrSavedOffset00 =
                 (ModelRuntimeSlot *)0x0;
            rotationAngleA = *(AngleTurn32 *)(childDefinitionOffset + 8);
            rotationAngleB = *(AngleTurn32 *)(childDefinitionOffset + 0xc);
            modelRuntime->attachments140[attachmentKindOrSlot].childLocalRotationAngle2 =
                 *(AngleTurn32 *)(childDefinitionOffset + 0x10);
            modelRuntime->attachments140[attachmentKindOrSlot].childLocalRotationAngle1 = rotationAngleB;
            modelRuntime->attachments140[attachmentKindOrSlot].childLocalRotationAngle0 = rotationAngleA;
          }
        }
        else {
          childOrFailedNode->parentNode = newNode;
          radiusOrTranslationY = attachmentTransform->localTranslationYQ12;
          translationZ = attachmentTransform->localTranslationZQ12;
          (childOrFailedNode->modelPayload).localTranslationXQ12 = attachmentTransform->localTranslationXQ12;
          (childOrFailedNode->modelPayload).localTranslationYQ12 = radiusOrTranslationY;
          (childOrFailedNode->modelPayload).localTranslationZQ12 = translationZ;
        }
        break;
      }
      attachmentTransform = attachmentTransform + 1;
    }
    if (transformRecordsRemaining == 0) {
      /* no attachment transform for this child */
      newNode->childNodes[childIndex] = (ModelRuntimeNode *)0x0;
    }
    childIndex = childIndex + 1;
  }
  childResult.failed = false;
  childResult.modelNode = newNode;
  return childResult;
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
  uint32_t childrenRemaining;
  uint32_t parentSlotsRemaining;
  ModelRuntimeNode *childSlotCursor;
  
  childSlotCursor = node;
  for (childrenRemaining = node->childCount; childrenRemaining != 0;
      childrenRemaining = childrenRemaining - 1) {
    if (childSlotCursor->childNodes[0] != (ModelRuntimeNode *)0x0) {
      ModelRuntimeNode_ReleaseRecursiveAndDetachParent(childSlotCursor->childNodes[0]);
    }
    childSlotCursor = (ModelRuntimeNode *)&(childSlotCursor->common).nextNode;
  }
  childSlotCursor = node->parentNode;
  if (childSlotCursor != (ModelRuntimeNode *)0x0) {
    for (parentSlotsRemaining = childSlotCursor->childCount; parentSlotsRemaining != 0; parentSlotsRemaining = parentSlotsRemaining - 1) {
      if (childSlotCursor->childNodes[0] == node) {
        childSlotCursor->childNodes[0] = (ModelRuntimeNode *)0x0;
      }
      childSlotCursor = (ModelRuntimeNode *)&(childSlotCursor->common).nextNode;
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
  int *categoryDamageSlot;
  int linkedRuntime;
  ShotDefinition *definition;
  uint32_t metricValue;
  uint32_t selectionRange;
  int armyOrCategoryIndex;
  int definitionArmyOrRemaining;
  
  definitionArmyOrRemaining = *modelRuntime;
  armyOrCategoryIndex = modelRuntime[2];
  linkedRuntime = modelRuntime[1];
  if ((modelRuntime[0x3b] & 1U) == 0) {
    metricValue = *(uint32_t *)(definitionArmyOrRemaining + 0x48);
  }
  else {
    metricValue = *(uint32_t *)(definitionArmyOrRemaining + 0x1a4);
  }
  if (*(uint32_t *)(armyOrCategoryIndex + 0x90) < *(uint32_t *)(definitionArmyOrRemaining + 0x27c)) {
    *(uint32_t *)(armyOrCategoryIndex + 0x90) = *(uint32_t *)(definitionArmyOrRemaining + 0x27c);
  }
  if (*(uint32_t *)(armyOrCategoryIndex + 0x44) < metricValue) {
    *(uint32_t *)(armyOrCategoryIndex + 0x44) = metricValue;
  }
  metricValue = (*(int *)(linkedRuntime + 0x9c) - *(int *)(*(int *)(armyOrCategoryIndex + 4) + 0x9c)) + *(int *)(definitionArmyOrRemaining + 0x70);
  if (*(uint32_t *)(armyOrCategoryIndex + 0x48) < metricValue) {
    *(uint32_t *)(armyOrCategoryIndex + 0x48) = metricValue;
  }
  definition = *(ShotDefinition **)(definitionArmyOrRemaining + 0x2c);
  if (*(int *)(definitionArmyOrRemaining + 0x30) != 0) {
    selectionRange = ShotDefinition_ComputeSelectionRange(definition);
    definitionArmyOrRemaining = modelRuntime[2];
    armyOrCategoryIndex = 7;
    if (*(int *)(definitionArmyOrRemaining + 0x4c) < (int)selectionRange) {
      *(uint32_t *)(definitionArmyOrRemaining + 0x4c) = selectionRange;
    }
    do {
      categoryDamageSlot = (int *)(definitionArmyOrRemaining + 0x100 + armyOrCategoryIndex * 4);
      *categoryDamageSlot = *categoryDamageSlot + definition->targetClassImpactDamageQ12[armyOrCategoryIndex];
      armyOrCategoryIndex = armyOrCategoryIndex + -1;
    } while (-1 < armyOrCategoryIndex);
  }
  for (definitionArmyOrRemaining = modelRuntime[3]; definitionArmyOrRemaining != 0; definitionArmyOrRemaining = definitionArmyOrRemaining + -1) {
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
  ModelRuntimeSlot *childModelRuntime;
  uint32_t attachmentsRemaining;
  int accumulatedHierarchyScaleQ12;
  ModelRuntimeSlot *attachmentDescriptorCursor;
  int scaleSampleCount;
  ModelRuntimeScaleRatioRegisterPairQ12 childScaleRatioPairQ12;
  
  accumulatedHierarchyScaleQ12 = 0x1000;
  scaleSampleCount = 1;
  attachmentDescriptorCursor = modelRuntime;
  for (attachmentsRemaining = modelRuntime->attachmentCount0C; attachmentsRemaining != 0;
      attachmentsRemaining = attachmentsRemaining - 1) {
    childModelRuntime = attachmentDescriptorCursor->attachments140[0].childModelRuntimeOrSavedOffset00
    ;
    if (childModelRuntime != (ModelRuntimeSlot *)0x0) {
      childScaleRatioPairQ12 = ModelRuntimeHierarchy_ComputeScaleRatioQ12Regs(childModelRuntime);
      accumulatedHierarchyScaleQ12 = accumulatedHierarchyScaleQ12 + (int)childScaleRatioPairQ12;
      scaleSampleCount = scaleSampleCount + 1;
    }
    attachmentDescriptorCursor =
         (ModelRuntimeSlot *)(attachmentDescriptorCursor->reserved10_37 + 0x10);
  }
  /* EAX = the scale ratio, EDX = 0x1000 */
  return (uint64_t)0x1000 << 0x20 |
         (uint64_t)(uint32_t)(int)(((int64_t)(int)modelRuntime->definitionValue60_3C *
                             (int64_t)accumulatedHierarchyScaleQ12) /
                            (int64_t)
                            (scaleSampleCount *
                            *(int *)((modelRuntime->definitionOrSavedId).savedIdOrOffset + 0x60)));
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
  uint32_t activeMetricTotal;
  uint32_t attachmentsRemaining;
  uint32_t totalMetric;
  ModelRuntimeSlot *currentChildModelRuntime;
  uint32_t childMetric;
  
  totalMetric = (modelRuntime->classState).definitionDerivedValueF4;
  attachmentsRemaining = modelRuntime->attachmentCount0C;
  activeMetricTotal = 0;
  if (((modelRuntime->classState).classStateEC & 1) == 0) {
    activeMetricTotal = totalMetric;
  }
  if ((*(uint32_t *)((modelRuntime->definitionOrSavedId).savedIdOrOffset + 0x68) & 0x80) != 0) {
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
  return (uint64_t)totalMetric << 0x20 | (uint64_t)activeMetricTotal; /* EDX = total, EAX = active */
}

/* Address: 0x0052AAC0.
   Ownership: world/model/hierarchy.
   Purpose: Smooths the model yaw field at offset 0x2C toward the target angle with bounded acceleration and
   deceleration, then marks the transform dirty.
*/

AimSmoothResult __thandor_eax_cf_preserve_ecx_edx
ModelNodeRuntime_SmoothYawTowardTarget
          (ModelRuntimeNode *modelNodeRuntime,ModelRuntimeWeaponAimStateView200 *smoothingState,
          AngleTurn32 targetYawAngle16)

{
  ArmyWeaponDefinitionView68 *aimDefinition;
  int turnRateLimit;
  AngleTurn32 currentYawAngle;
  uint32_t yawAngle;
  uint32_t yawStep;
  int acceleratedVelocity;
  uint32_t yawDelta;
  bool snapToTarget;
  AimSmoothResult smoothResult;
  AimSmoothResult settledResult;
  
  yawAngle = (modelNodeRuntime->modelPayload).localRotationAngle2;
  aimDefinition = smoothingState->modelDefinition;
  yawDelta = targetYawAngle16 - yawAngle & 0xffff;
  yawStep = smoothingState->yawTurnVelocityAngle16 * g_InGameSimulationStepTicks;
  snapToTarget = false;
  if (yawDelta < 0x8001) {
    /* target ahead in the positive direction */
    if ((int)yawStep < 0) {
      smoothingState->yawTurnVelocityAngle16 = 0; /* turning away: stop */
    }
    else if (yawDelta <= yawStep) {
      snapToTarget = true;
    }
    else {
      yawAngle = yawAngle + yawStep;
      turnRateLimit = aimDefinition->yawTurnRateLimitAnglePerTick10;
      acceleratedVelocity = smoothingState->yawTurnVelocityAngle16 +
              g_InGameSimulationStepTicks * aimDefinition->yawTurnRateAccelerationAnglePerTick1C;
      smoothingState->yawTurnVelocityAngle16 = turnRateLimit;
      if (acceleratedVelocity < turnRateLimit) {
        smoothingState->yawTurnVelocityAngle16 = acceleratedVelocity;
      }
    }
  }
  else if (0 < (int)yawStep) {
    smoothingState->yawTurnVelocityAngle16 = 0; /* turning away: stop */
  }
  else if (yawStep + 0x10000 <= yawDelta) {
    snapToTarget = true;
  }
  else {
    yawAngle = yawAngle + yawStep;
    turnRateLimit = aimDefinition->yawTurnRateLimitAnglePerTick10;
    acceleratedVelocity = smoothingState->yawTurnVelocityAngle16 -
            g_InGameSimulationStepTicks * aimDefinition->yawTurnRateAccelerationAnglePerTick1C;
    smoothingState->yawTurnVelocityAngle16 = -turnRateLimit;
    if (-turnRateLimit < acceleratedVelocity) {
      smoothingState->yawTurnVelocityAngle16 = acceleratedVelocity;
    }
  }
  if (snapToTarget) {
    /* the target is reached within this step */
    currentYawAngle = (modelNodeRuntime->modelPayload).localRotationAngle2;
    smoothingState->yawTurnVelocityAngle16 = 0;
    smoothResult.value = targetYawAngle16;
    if (targetYawAngle16 != currentYawAngle) {
      modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
      (modelNodeRuntime->modelPayload).localRotationAngle2 = targetYawAngle16;
    }
  }
  else {
    modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
    (modelNodeRuntime->modelPayload).localRotationAngle2 = yawAngle & 0xffff;
    smoothResult.value = (yawAngle & 0xffff) - targetYawAngle16 & 0xffff;
    if ((0x3ff < smoothResult.value) && (smoothResult.value < 0xfc01)) {
      smoothResult.outsideTolerance = true; /* still outside the aim tolerance */
      return smoothResult;
    }
  }
  settledResult.outsideTolerance = false;
  settledResult.value = smoothResult.value;
  return settledResult;
}


/* Address: 0x0052AC00.
   Ownership: world/model/hierarchy.
   Purpose: Clamps the target pitch to the definition bounds, smooths the model pitch field at offset 0x28 with
   bounded acceleration and deceleration, then marks the transform dirty.
*/

AimSmoothResult __thandor_eax_cf_preserve_ecx_edx
ModelNodeRuntime_SmoothPitchTowardTarget
          (ModelRuntimeNode *modelNodeRuntime,ModelRuntimeWeaponAimStateView200 *smoothingState,
          AngleTurn32 targetPitchAngle16)

{
  ArmyWeaponDefinitionView68 *aimDefinition;
  uint32_t pitchAngle;
  int pitchStepOrRateLimit;
  int acceleratedVelocity;
  bool snapToTarget;
  AimSmoothResult clampedTargetResult;
  AimSmoothResult settledResult;
  
  pitchAngle = (modelNodeRuntime->modelPayload).localRotationAngle1;
  aimDefinition = smoothingState->modelDefinition;
  clampedTargetResult.value = targetPitchAngle16;
  if ((int)aimDefinition->maximumPitchAngle28 < (int)targetPitchAngle16) {
    clampedTargetResult.value = aimDefinition->maximumPitchAngle28;
  }
  if ((int)clampedTargetResult.value < (int)aimDefinition->minimumPitchAngle24) {
    clampedTargetResult.value = aimDefinition->minimumPitchAngle24;
  }
  pitchStepOrRateLimit = smoothingState->pitchTurnVelocityAngle16 * g_InGameSimulationStepTicks;
  snapToTarget = true; /* already there, or reached within this step */
  if (clampedTargetResult.value != pitchAngle) {
    if ((int)pitchAngle <= (int)clampedTargetResult.value) {
      /* target above */
      if (pitchStepOrRateLimit < 0) {
        smoothingState->pitchTurnVelocityAngle16 = 0; /* moving away: stop */
        snapToTarget = false;
      }
      else if (pitchStepOrRateLimit < (int)(clampedTargetResult.value - pitchAngle)) {
        pitchAngle = pitchAngle + pitchStepOrRateLimit;
        pitchStepOrRateLimit = aimDefinition->pitchTurnRateLimitAnglePerTick14;
        acceleratedVelocity = smoothingState->pitchTurnVelocityAngle16 +
                g_InGameSimulationStepTicks * aimDefinition->pitchTurnRateAccelerationAnglePerTick20;
        smoothingState->pitchTurnVelocityAngle16 = pitchStepOrRateLimit;
        if (acceleratedVelocity < pitchStepOrRateLimit) {
          smoothingState->pitchTurnVelocityAngle16 = acceleratedVelocity;
        }
        snapToTarget = false;
      }
    }
    else if (0 < pitchStepOrRateLimit) {
      smoothingState->pitchTurnVelocityAngle16 = 0; /* moving away: stop */
      snapToTarget = false;
    }
    else if ((int)(clampedTargetResult.value - pitchAngle) < pitchStepOrRateLimit) {
      pitchAngle = pitchAngle + pitchStepOrRateLimit;
      pitchStepOrRateLimit = aimDefinition->pitchTurnRateLimitAnglePerTick14;
      acceleratedVelocity = smoothingState->pitchTurnVelocityAngle16 -
              g_InGameSimulationStepTicks * aimDefinition->pitchTurnRateAccelerationAnglePerTick20;
      smoothingState->pitchTurnVelocityAngle16 = -pitchStepOrRateLimit;
      if (-pitchStepOrRateLimit < acceleratedVelocity) {
        smoothingState->pitchTurnVelocityAngle16 = acceleratedVelocity;
      }
      snapToTarget = false;
    }
  }
  if (!snapToTarget) {
    modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
    (modelNodeRuntime->modelPayload).localRotationAngle1 = pitchAngle;
    clampedTargetResult.value = pitchAngle - clampedTargetResult.value & 0xffff;
    if ((0x3ff < clampedTargetResult.value) && (clampedTargetResult.value < 0xfc01)) {
      clampedTargetResult.outsideTolerance = true; /* still outside the aim tolerance */
      return clampedTargetResult;
    }
  }
  else {
    pitchAngle = (modelNodeRuntime->modelPayload).localRotationAngle1;
    smoothingState->pitchTurnVelocityAngle16 = 0;
    if (clampedTargetResult.value != pitchAngle) {
      modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
      (modelNodeRuntime->modelPayload).localRotationAngle1 = clampedTargetResult.value;
    }
  }
  settledResult.outsideTolerance = false;
  settledResult.value = clampedTargetResult.value;
  return settledResult;
}


/* Address: 0x004BD1A0.
   Ownership: world/model/hierarchy.
   Purpose: Stores one packed tint on the current model runtime node and recursively applies it to every non-null
   child in the exact child pointer array.
*/
void __thandor_void_preserve_eax_ecx_edx
ModelNodeRuntime_ApplyTintRecursive(PackedArgb32 tintArgb,ModelRuntimeNode *modelNode)

{
  uint32_t childrenRemaining;
  
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
  ModelRuntimeNode *currentChild;
  uint32_t childIndex;
  FixedEulerPairEdxEax8 extractedEulerAngles;
  FixedEulerAnglesEaxEcxEdx12 childEulerAngles;
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
      currentChild = modelNodeRuntime->childNodes[childIndex];
      childIndex = childIndex + 1;
      if (currentChild != (ModelRuntimeNode *)0x0) {
        FixedTransform_BuildRotationBasis
                  ((GraphicsFixedMatrix3x4 *)&g_ModelTransformScratchMatrix,
                   (currentChild->modelPayload).localRotationAngle2,
                   (currentChild->modelPayload).localRotationAngle1,
                   (currentChild->modelPayload).localRotationAngle0);
        g_ModelTransformTranslationX = (currentChild->modelPayload).localTranslationXQ12;
        g_ModelTransformTranslationY = (currentChild->modelPayload).localTranslationYQ12;
        g_ModelTransformTranslationZ = (currentChild->modelPayload).localTranslationZQ12;
        FixedTransform_Compose
                  (&currentChild->worldTransform,
                   (GraphicsFixedMatrix3x4 *)&g_ModelTransformScratchMatrix,
                   &modelNodeRuntime->worldTransform);
        childEulerAngles = FixedTransform_ExtractEulerAnglesRegs(&currentChild->worldTransform);
        (currentChild->modelPayload).worldRotationAngle2 = childEulerAngles.eaxAngle;
        (currentChild->modelPayload).worldRotationAngle0 = childEulerAngles.ecxAngle;
        (currentChild->modelPayload).worldRotationAngle1 = childEulerAngles.edxAngle;
        g_ModelTransformTranslationX = 0;
        g_ModelTransformTranslationY = 0;
        g_ModelTransformTranslationZ = 0;
        inheritedTintArgb = modelNodeRuntime->tintArgb;
        currentChild->runtimeFlags = currentChild->runtimeFlags | 1;
        currentChild->tintArgb = inheritedTintArgb;
        ModelNodeRuntime_ComposeChildTransformsRecursive(currentChild);
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
  int variantsRemaining;
  int variantCursorOrRemaining;
  bool isUnlocked;
  ModelDefinitionResult lookupResult;
  
  variantCursorOrRemaining = *modelRuntime;
  variantsRemaining = 6;
  do {
    modelDefinitionId = *(PckModelDefinitionIdCatalog *)(variantCursorOrRemaining + 0x238);
    if ((modelDefinitionId != 0) &&
       (isUnlocked = ModelDefinition_IsFactionTechnologyUnlockedCf
                          (g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits,
                           modelDefinitionId), !isUnlocked)) {
      /* swap to this variant, keeping the scaled metric */
      lookupResult = ModelDefinitionRegistry_FindByIdWithErrorCf(modelDefinitionId);
      variantCursorOrRemaining = *modelRuntime;
      *modelRuntime = (int)lookupResult.modelDefinition;
      modelRuntime[0xf] =
           (int)(((int64_t)modelRuntime[0xf] * (int64_t)(int)lookupResult.modelDefinition[8].byteSize) /
                (int64_t)*(int *)(variantCursorOrRemaining + 0x60));
      ArmyRuntime_RebuildDerivedSelectionMetrics((ArmyRuntimeSlot *)modelRuntime[2]);
      break;
    }
    variantCursorOrRemaining = variantCursorOrRemaining + 4;
    variantsRemaining = variantsRemaining + -1;
  } while (variantsRemaining != 0);
  for (variantCursorOrRemaining = modelRuntime[3]; variantCursorOrRemaining != 0; variantCursorOrRemaining = variantCursorOrRemaining + -1) {
    if ((int *)modelRuntime[0x50] != (int *)0x0) {
      ModelRuntimeHierarchy_ApplyFactionTechnologyVariantsRecursive
                (factionIndex,(int *)modelRuntime[0x50]);
    }
    modelRuntime = modelRuntime + 8;
  }
  return;
}

