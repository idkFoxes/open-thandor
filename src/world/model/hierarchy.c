/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/model/hierarchy.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/model/hierarchy.h>
#include <thandor/thandor.h>

static void ModelRuntimeHierarchy_MarkDestroyedFrom(ModelRuntimeSlot *node);

/* Implementation ownership: world/model/hierarchy. */

/* Address: 0x004BD1F0.
   Fades the model's tint one step toward the target its state flags ask for and applies it to the whole
   hierarchy (called by the army terrainStateRefresh maintenance phase in gameplay/army/runtime.c). Targets:
   flag 4 white and opaque; else flag 8 with 0x10 white and transparent, flag 8 alone grey 0x87 and opaque,
   neither black and transparent; flag 0x1000 always makes it transparent. The step limit comes from the
   intensity clamp table (GraphicsIntensityClampTable_Initialize).
*/
void ModelNodeRuntime_UpdateStateTintRecursive(ModelRuntimeNode *modelNodeRuntime)

{
  uint8_t *clampTable;
  uint8_t clampedColorByte;
  uint8_t clampedAlphaByte;
  uint32_t flagsOrPreviousTint;
  int colorIntensity;
  PackedArgb32 tintArgb;
  int alphaIntensity;
  
  colorIntensity = 255;
  alphaIntensity = 255;
  flagsOrPreviousTint = modelNodeRuntime->runtimeFlags;
  if ((flagsOrPreviousTint & TERRAIN_OCCUPANCY_FLAG_PRESENT) == 0) {
    colorIntensity = 0;
    alphaIntensity = 0;
    if ((flagsOrPreviousTint & TERRAIN_OCCUPANCY_FLAG_SEEN_BEFORE) != 0) {
      colorIntensity = 255;
      alphaIntensity = 0;
      if ((flagsOrPreviousTint & TERRAIN_OCCUPANCY_FLAG_NOT_REMEMBERED) == 0) {
        colorIntensity = 135;
        alphaIntensity = 255;
      }
    }
  }
  if ((flagsOrPreviousTint & MODEL_NODE_FLAG_FORCE_TRANSPARENT) != 0) {
    alphaIntensity = 0;
  }
  flagsOrPreviousTint = modelNodeRuntime->tintArgb;
  /* The clamp table is 64-KiB aligned: the original puts the target intensity in AL/BL and the previous
     tint byte in AH/BH, i.e. indexes it with (previous << 8) | target. */
  clampTable = (uint8_t *)g_GraphicsIntensityClampTableBase;
  clampedColorByte = clampTable[((flagsOrPreviousTint >> 16) & 0xff) << 8 | (uint32_t)colorIntensity];
  clampedAlphaByte = clampTable[(flagsOrPreviousTint >> 24) << 8 | (uint32_t)alphaIntensity];
  tintArgb = (uint32_t)clampedAlphaByte << 24 | (uint32_t)clampedColorByte << 16 | (uint32_t)clampedColorByte << 8 |
             (uint32_t)clampedColorByte;
  /* The original compares with the previous tint shifted right by 16 (CMP EDX,ECX at 0x004BD27F), so the new
     tint is applied on practically every call, not only when it changed. */
  if (tintArgb != flagsOrPreviousTint >> 16) {
    ModelNodeRuntime_ApplyTintRecursive(tintArgb,modelNodeRuntime);
  }
  return;
}


/* Address: 0x004BE360.
   Recomputes the world transforms of a model hierarchy after its local animation, aim, recoil or
   translation changed: composes from the node's parent if it has one, else from the node itself. Only one
   level is climbed, so callers pass a root node or a direct child of it.
*/
void ModelNodeRuntime_RebuildTransformsFromRoot(ModelRuntimeNode *modelNodeRuntime)

{
  if (modelNodeRuntime->parentNode == NULL) {
    ModelNodeRuntime_ComposeChildTransformsRecursive(modelNodeRuntime);
  }
  else {
    ModelNodeRuntime_ComposeChildTransformsRecursive(modelNodeRuntime->parentNode);
  }
}


/* Address: 0x0051D870.
   Gives a model node and all its descendants the palette and texture set. Only
   called by itself in the executable; the faction code uses
   ModelRuntimeHierarchy_SetPaletteAndTextureSetNonNullRecursive.
*/
void ModelRuntimeHierarchy_SetPaletteAndTextureSetRecursive
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,ModelRuntimeNode *node)

{
  uint32_t childrenRemaining;

  if (node != NULL) {
    node->modelPayload.textureSet = textureSet;
    node->modelPayload.paletteAsset = paletteAsset;
    for (childrenRemaining = node->childCount; childrenRemaining != 0; childrenRemaining = childrenRemaining - 1) {
      ModelRuntimeHierarchy_SetPaletteAndTextureSetRecursive
                (paletteAsset,textureSet,node->childNodes[0]);
      /* the original steps the node pointer by 4 bytes, so childNodes[0] walks through all children */
      node = (ModelRuntimeNode *)((uint32_t *)node + 1);
    }
  }
  return;
}

/* Address: 0x0051DB80.
   Switches the models of an army to the variants its faction's technology selects: runs
   ModelRuntimeHierarchy_ApplyFactionTechnologyVariantsRecursive on the army's model runtime hierarchy.
*/
void ModelRuntimeHierarchy_ApplyFactionTechnologyVariants(FactionRuntimeIndex factionIndex,ArmyRuntimeSlot *armyRuntime)

{
  ModelRuntimeHierarchy_ApplyFactionTechnologyVariantsRecursive
            (factionIndex,(int *)(armyRuntime->modelRuntimeOrSavedOffset).modelRuntime);
}


/* Address: 0x004BD310.
   Grows the global model bounding box (g_ModelBoundsMinimum/Maximum X/Y/Z) by every mesh vertex of the node,
   transformed by the node's world transform, and then by all its descendants. The caller seeds the box first.
*/
void ModelNodeRuntime_AccumulateTransformedBoundsRecursive(ModelRuntimeNode *modelNode)

{
  ModelResource *resourceView;
  uint32_t childrenRemaining;
  int vertexCountOrChildIndex;
  GraphicsFixedVec3 *point;
  uint8_t *geometryRecord;
  ModelPackedGeometryRecordCount geometryRecordsRemaining;
  
  resourceView = modelNode->modelPayload.modelResource;
  if (resourceView->meshGroupCount != 0) {
    geometryRecord = (uint8_t *)(resourceView + 1) + 16;
    /* geometry record: +0x00 byte size of the record, +0x08 vertex count, +0x20 vertices (0x40 bytes each) */
    for (geometryRecordsRemaining = resourceView->packedGeometryRecordCount; geometryRecordsRemaining != 0;
        geometryRecordsRemaining--) {
      point = (GraphicsFixedVec3 *)(geometryRecord + 32);
      for (vertexCountOrChildIndex = *(int *)(geometryRecord + 8); vertexCountOrChildIndex != 0;
          vertexCountOrChildIndex--) {
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
        point = (GraphicsFixedVec3 *)((uint8_t *)point + 64); /* the next vertex */
      }
      geometryRecord = geometryRecord + *(int *)geometryRecord;
    }
  }
  vertexCountOrChildIndex = 0;
  for (childrenRemaining = modelNode->childCount; childrenRemaining != 0; childrenRemaining--) {
    if (modelNode->childNodes[vertexCountOrChildIndex] != NULL) {
      ModelNodeRuntime_AccumulateTransformedBoundsRecursive(modelNode->childNodes[vertexCountOrChildIndex]);
    }
    vertexCountOrChildIndex++;
  }
  return;
}


/* Address: 0x004BD8D0.
   Turns a mesh group towards the camera around the model's own up axis (mesh group flag 1 in
   ModelRender_DrawMeshGroupsWithTemporaryTransform): rotates the camera-to-node vector into the model's frame
   (keeping its two stored rotation angles) and replaces the third angle by the view direction plus a quarter turn.
*/
void ModelNodeRuntime_BuildViewFacingRotation(ModelRuntimeNode *modelNodeRuntime)

{
  uint32_t viewFacingAngle16;
  FixedVectorQ12 viewRelativeVector;

  viewRelativeVector = FixedTransform_RotateVectorByEulerAngles
                    (modelNodeRuntime->worldTransform.translation.x - g_ViewOriginFixed.x,
                     modelNodeRuntime->worldTransform.translation.y - g_ViewOriginFixed.y,
                     modelNodeRuntime->worldTransform.translation.z - g_ViewOriginFixed.z,0,
                     modelNodeRuntime->modelPayload.worldRotationAngle1,
                     modelNodeRuntime->modelPayload.worldRotationAngle0 - FIXED_ANGLE16_HALF_TURN);
  viewFacingAngle16 = FixedMath_Atan2Angle16(viewRelativeVector.yQ12,viewRelativeVector.xQ12);
  FixedTransform_BuildRotationBasis
            (&modelNodeRuntime->worldTransform,viewFacingAngle16 + FIXED_ANGLE16_QUARTER_TURN & FIXED_ANGLE16_MASK,
             modelNodeRuntime->modelPayload.worldRotationAngle1,
             modelNodeRuntime->modelPayload.worldRotationAngle0);
  return;
}


/* Address: 0x004BD950.
   Turns a mesh group fully towards the camera, a billboard (mesh group flag 2 in
   ModelRender_DrawMeshGroupsWithTemporaryTransform): the rotation basis is built from the direction of the
   camera-to-node vector (azimuth + half turn, negated elevation).
*/
void ModelNodeRuntime_BuildBillboardRotation(ModelRuntimeNode *modelNodeRuntime)

{
  uint32_t angle0;
  FixedVectorAngles viewAngles;

  viewAngles = FixedMath_VectorToAngles
                    (modelNodeRuntime->worldTransform.translation.z - g_ViewOriginFixed.z,
                     modelNodeRuntime->worldTransform.translation.y - g_ViewOriginFixed.y,
                     modelNodeRuntime->worldTransform.translation.x - g_ViewOriginFixed.x);
  angle0 = viewAngles.azimuthAngle + FIXED_ANGLE16_HALF_TURN & FIXED_ANGLE16_MASK;
  FixedTransform_BuildRotationBasis(&modelNodeRuntime->worldTransform,angle0,-viewAngles.elevationAngle,angle0);
  return;
}


/* Address: 0x004BE9D0.
   Recomputes the bounding radius of a model node's subtree, children first: the largest of the node's own
   model radius and, per child, the child's distance from the node plus the child's subtree radius. Read by
   rendering, the selection overlay and ModelNodeRuntime_UpdateDepthBinMasks.
*/
void ModelNodeRuntime_RecomputeSubtreeBoundingRadius(ModelRuntimeNode *modelNodeRuntime)

{
  ModelRuntimeNode *childNode;
  uint32_t childDistance;
  uint32_t childExtent;
  uint32_t childrenRemaining;
  uint32_t maximumRadius;
  ModelRuntimeNode *childSlotCursor;

  maximumRadius = modelNodeRuntime->modelPayload.modelResource->boundingRadiusQ12;
  childSlotCursor = modelNodeRuntime;
  for (childrenRemaining = modelNodeRuntime->childCount; childrenRemaining != 0; childrenRemaining--) {
    childNode = childSlotCursor->childNodes[0];
    if (childNode != NULL) {
      ModelNodeRuntime_RecomputeSubtreeBoundingRadius(childNode);
      childDistance = FixedMath_LengthVec3
                        ((GraphicsFixedVec3 *)
                         &childNode->modelPayload.localTranslationXQ12);
      childExtent = childDistance + childNode->subtreeBoundingRadiusQ12;
      if (maximumRadius < childExtent) {
        maximumRadius = childExtent;
      }
    }
    /* steps the cursor by one dword, i.e. to the next childNodes[] entry */
    childSlotCursor = (ModelRuntimeNode *)((uint32_t *)childSlotCursor + 1);
  }
  modelNodeRuntime->subtreeBoundingRadiusQ12 = maximumRadius;
  return;
}


/* Address: 0x004BEA30.
   Updates the coarse position bins of a model node after it moved: one bit mask along world x and one along
   world y, each covering the node's position +- the larger of minimumRadius (the army's placement radius)
   and the subtree bounding radius. Placement and combat test these masks before exact distance checks.
*/
void ModelNodeRuntime_UpdateDepthBinMasks(DepthIntervalRadius32 minimumRadius,ModelRuntimeNode *modelNodeRuntime)

{
  DepthBinMask32 binMask;
  GraphicsWorldCoordinateQ12 centerY;

  if (minimumRadius < modelNodeRuntime->subtreeBoundingRadiusQ12) {
    minimumRadius = modelNodeRuntime->subtreeBoundingRadiusQ12;
  }
  /* y is read first: the original pushes the arguments of both calls before the first one */
  centerY = modelNodeRuntime->worldTransform.translation.y;
  binMask = DepthInterval_BuildBinMask
                    (minimumRadius,modelNodeRuntime->worldTransform.translation.x);
  modelNodeRuntime->depthBinMaskNear = binMask;
  binMask = DepthInterval_BuildBinMask(minimumRadius,centerY);
  modelNodeRuntime->depthBinMaskFar = binMask;
  return;
}


/* Address: 0x004BEB80.
   Transforms a model-local point record (anchor, launch or marker point) into world coordinates through the
   node's world transform. The point is written to g_ModelTransformOutputX..Z and also returned.
*/
ModelWorldPoint
ModelNodeRuntime_TransformLocalPoint
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
   Converts a world direction (elevation, azimuth) into the frame of a model node for aiming turrets and weapons
   (ArmyRuntimeClass_UpdateSingleBarrelTurret/B, ArmyRuntimeWeapon_UpdateTargetAimAndFireAttachments):
   rotates a unit vector by the inverse of the node's world rotation and returns its angles, the yaw made
   relative by adding the node's local rotation angle 2 (+0x2C).
*/

ModelRelativeDirectionAngles ModelNodeRuntime_ComputeRelativeDirectionAngle
          (ModelRuntimeNode *modelNodeRuntime,AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle)

{
  uint32_t negatedAngle2;
  FixedVectorAngles directionAngles;
  FixedVectorQ12 rotatedDirection;
  ModelRelativeDirectionAngles relativeAngles;

  negatedAngle2 = -modelNodeRuntime->modelPayload.worldRotationAngle2;
  rotatedDirection = FixedTransform_RotateScaledDirection
                    (Q12_ONE,elevationAngle,azimuthAngle,negatedAngle2 & FIXED_ANGLE16_MASK,
                     modelNodeRuntime->modelPayload.worldRotationAngle1,
                     modelNodeRuntime->modelPayload.worldRotationAngle0 + FIXED_ANGLE16_HALF_TURN +
                     negatedAngle2 & FIXED_ANGLE16_MASK)
  ;
  directionAngles = FixedMath_VectorToAngles(rotatedDirection.zQ12,rotatedDirection.yQ12,rotatedDirection.xQ12);
  relativeAngles.relativeYawAngle =
       directionAngles.azimuthAngle + modelNodeRuntime->modelPayload.localRotationAngle2 & FIXED_ANGLE16_MASK;
  relativeAngles.relativePitchAngle = directionAngles.elevationAngle;
  return relativeAngles;
}


/* Address: 0x0050A7A0.
   Pointer hit test of a model hierarchy (FrontendModelPointerContext_FindBestEligibleModelHitTarget): projects the
   eight corners of the node's local bounding box and tests the pointer against the twelve triangles of its faces
   (faces with a corner behind the near plane are skipped). On a hit returns the distance from the context's
   reference point to the node (to the box centre with HIT_DISTANCE_TO_BOUNDS_CENTER); otherwise the children are tested
   in order. Returns true on a hit and stores the distance in *outDistanceQ12; returns false (and leaves
   *outDistanceQ12 unchanged) when neither the node nor a child was hit.
*/
bool ModelRuntimeNode_HitTestProjectedBoundsAndChildren
          (int pointerY,int pointerX,ModelRuntimeNode *modelNode,
          FrontendModelPointerHitContext *context,uint32_t *outDistanceQ12)

{
  ModelResource *resourceView;
  GraphicsWorldCoordinateQ12 boundsX1;
  ModelRuntimeNode *childNode;
  uint8_t clippedCornerMask;
  GraphicsFixedMatrix3x4 *transformA;
  uint32_t childrenRemaining;
  int childByteOffset;
  bool cornerVisibleOrHit;
  GraphicsProjectedPointPair projectedCorner;

  resourceView = modelNode->modelPayload.modelResource;
  transformA = &modelNode->worldTransform;
  if ((resourceView->boundingRadiusQ12 != 0) &&
     ((resourceView->hitTestFlags20C & MODEL_RESOURCE_DISABLE_PROJECTED_HIT_TEST) == 0)) {
    FixedTransform_Compose
              (&g_GraphicsTransformScratchMatrix3x4,transformA,&g_ViewProjectionMatrixFixed);
    /* corner i = (x i&1, y i&2, z i&4) of the bounds; bit i of clippedCornerMask: corner i behind the near
       plane (z < g_ProjectionScaleFixed), else g_ModelProjectedBoundsCornerScratch8[i] holds its screen point */
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
      g_ModelProjectedBoundsCornerScratch8[0].x = projectedCorner.projectedX >> Q12_SHIFT;
      g_ModelProjectedBoundsCornerScratch8[0].y = projectedCorner.projectedY >> Q12_SHIFT;
    }
    clippedCornerMask = !cornerVisibleOrHit;
    g_GraphicsTransformInputScratchVec3.x = boundsX1;
    FixedTransform_ApplyPoint
              (&g_GraphicsTransformOutputScratchVec3,&g_GraphicsTransformInputScratchVec3,
               &g_GraphicsTransformScratchMatrix3x4);
    if (g_GraphicsTransformOutputScratchVec3.z < (int)g_ProjectionScaleFixed) {
      clippedCornerMask = clippedCornerMask | MODEL_BOUNDS_CORNER_BIT(1);
    }
    else {
      projectedCorner = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
      g_ModelProjectedBoundsCornerScratch8[1].x = projectedCorner.projectedX >> Q12_SHIFT;
      g_ModelProjectedBoundsCornerScratch8[1].y = projectedCorner.projectedY >> Q12_SHIFT;
    }
    g_GraphicsTransformInputScratchVec3.x = resourceView->localBoundsX0Q12;
    g_GraphicsTransformInputScratchVec3.y = resourceView->localBoundsY1Q12;
    FixedTransform_ApplyPoint
              (&g_GraphicsTransformOutputScratchVec3,&g_GraphicsTransformInputScratchVec3,
               &g_GraphicsTransformScratchMatrix3x4);
    if (g_GraphicsTransformOutputScratchVec3.z < (int)g_ProjectionScaleFixed) {
      clippedCornerMask = clippedCornerMask | MODEL_BOUNDS_CORNER_BIT(2);
    }
    else {
      projectedCorner = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
      g_ModelProjectedBoundsCornerScratch8[2].x = projectedCorner.projectedX >> Q12_SHIFT;
      g_ModelProjectedBoundsCornerScratch8[2].y = projectedCorner.projectedY >> Q12_SHIFT;
    }
    g_GraphicsTransformInputScratchVec3.x = boundsX1;
    FixedTransform_ApplyPoint
              (&g_GraphicsTransformOutputScratchVec3,&g_GraphicsTransformInputScratchVec3,
               &g_GraphicsTransformScratchMatrix3x4);
    if (g_GraphicsTransformOutputScratchVec3.z < (int)g_ProjectionScaleFixed) {
      clippedCornerMask = clippedCornerMask | MODEL_BOUNDS_CORNER_BIT(3);
    }
    else {
      projectedCorner = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
      g_ModelProjectedBoundsCornerScratch8[3].x = projectedCorner.projectedX >> Q12_SHIFT;
      g_ModelProjectedBoundsCornerScratch8[3].y = projectedCorner.projectedY >> Q12_SHIFT;
    }
    g_GraphicsTransformInputScratchVec3.x = resourceView->localBoundsX0Q12;
    g_GraphicsTransformInputScratchVec3.y = resourceView->localBoundsY0Q12;
    g_GraphicsTransformInputScratchVec3.z = resourceView->localBoundsZ1Q12;
    FixedTransform_ApplyPoint
              (&g_GraphicsTransformOutputScratchVec3,&g_GraphicsTransformInputScratchVec3,
               &g_GraphicsTransformScratchMatrix3x4);
    if (g_GraphicsTransformOutputScratchVec3.z < (int)g_ProjectionScaleFixed) {
      clippedCornerMask = clippedCornerMask | MODEL_BOUNDS_CORNER_BIT(4);
    }
    else {
      projectedCorner = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
      g_ModelProjectedBoundsCornerScratch8[4].x = projectedCorner.projectedX >> Q12_SHIFT;
      g_ModelProjectedBoundsCornerScratch8[4].y = projectedCorner.projectedY >> Q12_SHIFT;
    }
    g_GraphicsTransformInputScratchVec3.x = boundsX1;
    FixedTransform_ApplyPoint
              (&g_GraphicsTransformOutputScratchVec3,&g_GraphicsTransformInputScratchVec3,
               &g_GraphicsTransformScratchMatrix3x4);
    if (g_GraphicsTransformOutputScratchVec3.z < (int)g_ProjectionScaleFixed) {
      clippedCornerMask = clippedCornerMask | MODEL_BOUNDS_CORNER_BIT(5);
    }
    else {
      projectedCorner = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
      g_ModelProjectedBoundsCornerScratch8[5].x = projectedCorner.projectedX >> Q12_SHIFT;
      g_ModelProjectedBoundsCornerScratch8[5].y = projectedCorner.projectedY >> Q12_SHIFT;
    }
    transformA = (GraphicsFixedMatrix3x4 *)resourceView->localBoundsX0Q12;
    g_GraphicsTransformInputScratchVec3.y = resourceView->localBoundsY1Q12;
    g_GraphicsTransformInputScratchVec3.x = (GraphicsWorldCoordinateQ12)transformA;
    FixedTransform_ApplyPoint
              (&g_GraphicsTransformOutputScratchVec3,&g_GraphicsTransformInputScratchVec3,
               &g_GraphicsTransformScratchMatrix3x4);
    if (g_GraphicsTransformOutputScratchVec3.z < (int)g_ProjectionScaleFixed) {
      clippedCornerMask = clippedCornerMask | MODEL_BOUNDS_CORNER_BIT(6);
    }
    else {
      projectedCorner = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
      transformA = (GraphicsFixedMatrix3x4 *)(projectedCorner.projectedX >> Q12_SHIFT);
      g_ModelProjectedBoundsCornerScratch8[6].y = projectedCorner.projectedY >> Q12_SHIFT;
      g_ModelProjectedBoundsCornerScratch8[6].x = (GraphicsProjectedCoordinate)transformA;
    }
    g_GraphicsTransformInputScratchVec3.x = boundsX1;
    FixedTransform_ApplyPoint
              (&g_GraphicsTransformOutputScratchVec3,&g_GraphicsTransformInputScratchVec3,
               &g_GraphicsTransformScratchMatrix3x4);
    if (g_GraphicsTransformOutputScratchVec3.z < (int)g_ProjectionScaleFixed) {
      clippedCornerMask = clippedCornerMask | MODEL_BOUNDS_CORNER_BIT(7);
    }
    else {
      projectedCorner = Graphics_ProjectViewPoint(&g_GraphicsTransformOutputScratchVec3);
      transformA = (GraphicsFixedMatrix3x4 *)(projectedCorner.projectedX >> Q12_SHIFT);
      g_ModelProjectedBoundsCornerScratch8[7].y = projectedCorner.projectedY >> Q12_SHIFT;
      g_ModelProjectedBoundsCornerScratch8[7].x = (GraphicsProjectedCoordinate)transformA;
    }
    if (((((((clippedCornerMask & MODEL_BOUNDS_TRIANGLE_CORNERS(2,1,0)) == 0) &&
           (cornerVisibleOrHit = GraphicsProjectedPoint_IsInsideTriangle
                              (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 2,
                               g_ModelProjectedBoundsCornerScratch8 + 1,
                               g_ModelProjectedBoundsCornerScratch8), cornerVisibleOrHit)) ||
          (((clippedCornerMask & MODEL_BOUNDS_TRIANGLE_CORNERS(2,1,3)) == 0 &&
           (cornerVisibleOrHit = GraphicsProjectedPoint_IsInsideTriangle
                              (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 2,
                               g_ModelProjectedBoundsCornerScratch8 + 1,
                               g_ModelProjectedBoundsCornerScratch8 + 3), cornerVisibleOrHit)))) ||
         (((clippedCornerMask & MODEL_BOUNDS_TRIANGLE_CORNERS(6,5,4)) == 0 &&
          (cornerVisibleOrHit = GraphicsProjectedPoint_IsInsideTriangle
                             (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 6,
                              g_ModelProjectedBoundsCornerScratch8 + 5,
                              g_ModelProjectedBoundsCornerScratch8 + 4), cornerVisibleOrHit)))) ||
        (((((clippedCornerMask & MODEL_BOUNDS_TRIANGLE_CORNERS(6,5,7)) == 0 &&
           (cornerVisibleOrHit = GraphicsProjectedPoint_IsInsideTriangle
                              (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 6,
                               g_ModelProjectedBoundsCornerScratch8 + 5,
                               g_ModelProjectedBoundsCornerScratch8 + 7), cornerVisibleOrHit)) ||
          ((((clippedCornerMask & MODEL_BOUNDS_TRIANGLE_CORNERS(2,4,0)) == 0 &&
            (cornerVisibleOrHit = GraphicsProjectedPoint_IsInsideTriangle
                               (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 2,
                                g_ModelProjectedBoundsCornerScratch8 + 4,
                                g_ModelProjectedBoundsCornerScratch8), cornerVisibleOrHit)) ||
           (((clippedCornerMask & MODEL_BOUNDS_TRIANGLE_CORNERS(2,6,4)) == 0 &&
            (cornerVisibleOrHit = GraphicsProjectedPoint_IsInsideTriangle
                               (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 2,
                                g_ModelProjectedBoundsCornerScratch8 + 6,
                                g_ModelProjectedBoundsCornerScratch8 + 4), cornerVisibleOrHit)))))) ||
         ((((clippedCornerMask & MODEL_BOUNDS_TRIANGLE_CORNERS(3,5,1)) == 0 &&
           (cornerVisibleOrHit = GraphicsProjectedPoint_IsInsideTriangle
                              (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 3,
                               g_ModelProjectedBoundsCornerScratch8 + 5,
                               g_ModelProjectedBoundsCornerScratch8 + 1), cornerVisibleOrHit)) ||
          (((clippedCornerMask & MODEL_BOUNDS_TRIANGLE_CORNERS(3,7,5)) == 0 &&
           (cornerVisibleOrHit = GraphicsProjectedPoint_IsInsideTriangle
                              (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 3,
                               g_ModelProjectedBoundsCornerScratch8 + 7,
                               g_ModelProjectedBoundsCornerScratch8 + 5), cornerVisibleOrHit)))))))) ||
       (((((clippedCornerMask & MODEL_BOUNDS_TRIANGLE_CORNERS(4,1,0)) == 0 &&
          (cornerVisibleOrHit = GraphicsProjectedPoint_IsInsideTriangle
                             (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 4,
                              g_ModelProjectedBoundsCornerScratch8 + 1,
                              g_ModelProjectedBoundsCornerScratch8), cornerVisibleOrHit)) ||
         (((clippedCornerMask & MODEL_BOUNDS_TRIANGLE_CORNERS(1,5,4)) == 0 &&
          (cornerVisibleOrHit = GraphicsProjectedPoint_IsInsideTriangle
                             (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 1,
                              g_ModelProjectedBoundsCornerScratch8 + 5,
                              g_ModelProjectedBoundsCornerScratch8 + 4), cornerVisibleOrHit)))) ||
        ((((clippedCornerMask & MODEL_BOUNDS_TRIANGLE_CORNERS(3,6,2)) == 0 &&
          (cornerVisibleOrHit = GraphicsProjectedPoint_IsInsideTriangle
                             (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 3,
                              g_ModelProjectedBoundsCornerScratch8 + 6,
                              g_ModelProjectedBoundsCornerScratch8 + 2), cornerVisibleOrHit)) ||
         (((clippedCornerMask & MODEL_BOUNDS_TRIANGLE_CORNERS(3,6,7)) == 0 &&
          (cornerVisibleOrHit = GraphicsProjectedPoint_IsInsideTriangle
                             (pointerY,pointerX,g_ModelProjectedBoundsCornerScratch8 + 3,
                              g_ModelProjectedBoundsCornerScratch8 + 6,
                              g_ModelProjectedBoundsCornerScratch8 + 7), cornerVisibleOrHit)))))))) {
      if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_HIT_DISTANCE_TO_BOUNDS_CENTER) != 0) {
        *outDistanceQ12 =
             FixedMath_Length3(((resourceView->localBoundsZ0Q12 + resourceView->localBoundsZ1Q12 >> 1) +
                               modelNode->worldTransform.translation.z) -
                               context->hitReferenceWorldZQ12,
                               ((resourceView->localBoundsY0Q12 + resourceView->localBoundsY1Q12 >> 1) +
                               modelNode->worldTransform.translation.y) -
                               context->hitReferenceWorldYQ12,
                               ((resourceView->localBoundsX0Q12 + resourceView->localBoundsX1Q12 >> 1) +
                               modelNode->worldTransform.translation.x) -
                               context->hitReferenceWorldXQ12);
        return true;
      }
      *outDistanceQ12 =
           FixedMath_Length3(modelNode->worldTransform.translation.z -
                             context->hitReferenceWorldZQ12,
                             modelNode->worldTransform.translation.y -
                             context->hitReferenceWorldYQ12,
                             modelNode->worldTransform.translation.x -
                             context->hitReferenceWorldXQ12);
      return true;
    }
  }
  childByteOffset = 0;
  for (childrenRemaining = modelNode->childCount; childrenRemaining != 0;
       childrenRemaining = childrenRemaining - 1) {
    childNode = *(ModelRuntimeNode **)((int)modelNode->childNodes + childByteOffset);
    if (childNode != NULL) {
      if (ModelRuntimeNode_HitTestProjectedBoundsAndChildren
                         (pointerY,pointerX,childNode,context,outDistanceQ12)) {
        return true;
      }
    }
    childByteOffset = childByteOffset + 4;
  }
  return false;
}


/* Address: 0x0050B1D0.
   Ray test of a model hierarchy against the ray in g_ModelRaycastOriginX/Y/Z and
   g_ModelRaycastWorldDirectionX/Y/ZQ28 (ModelRuntime_RaycastCandidateListNearest): when the ray passes the node's
   bounding sphere within
   g_ModelRaycastMaximumDistance, it is moved into the node's frame and tested against every triangle of the
   node's mesh group, then the children are tested. Returns the nearest hit distance and stores the nearest
   node in *outNearestModelNode; returns MODEL_RAYCAST_NO_HIT_DISTANCE when nothing was hit (a hit never has
   that distance).
   Original quirk: on a miss *outNearestModelNode still receives the EDX scratch value of the original (a
   product high word, NULL, or what the last child test left there); ModelRuntime_RaycastCandidateListNearest
   can pick it up.
*/
Q12 ModelNodeRuntime_RaycastHierarchyNearest
          (ModelRuntimeNode *modelNodeRuntime,ModelRuntimeNode **outNearestModelNode)

{
  int *triangleCountField;
  ModelResource *resourceView;
  int64_t projectionOrDiscriminant;
  int64_t projectedDistanceWide;
  uint32_t negatedAngle2;
  int deltaXOrNodeY;
  int deltaYOrNodeZ;
  int trianglesRemaining;
  uint32_t childrenRemaining;
  int deltaZ;
  ModelRaycastNearestNodeOrScratch4 edxCarrier;
  ModelPackedGeometryRecordCount meshRecordsRemaining;
  ModelMeshGroupCount meshGroupsRemaining;
  int radiusNodeXOrNearest;
  ModelMeshGroupRelativeOffset *meshGroupCursor;
  ModelRaycastTriangleDescriptor *triangle;
  ModelRuntimeNode *nearestModelNode;
  Q12 triangleDistanceQ12;
  Q12 childDistanceQ12;

  deltaXOrNodeY = modelNodeRuntime->worldTransform.translation.x - g_ModelRaycastOriginX;
  deltaYOrNodeZ = modelNodeRuntime->worldTransform.translation.y - g_ModelRaycastOriginY;
  deltaZ = modelNodeRuntime->worldTransform.translation.z - g_ModelRaycastOriginZ;
  radiusNodeXOrNearest = modelNodeRuntime->subtreeBoundingRadiusQ12;
  projectionOrDiscriminant = (int64_t)deltaYOrNodeZ * (int64_t)(int)g_ModelRaycastWorldDirectionYQ28 +
          (int64_t)deltaXOrNodeY * (int64_t)(int)g_ModelRaycastWorldDirectionXQ28 +
          (int64_t)deltaZ * (int64_t)(int)g_ModelRaycastWorldDirectionZQ28;
  edxCarrier.scratchSigned =
       FIXED_PRODUCT_SHR(projectionOrDiscriminant,Q28_SHIFT);
  if ((-radiusNodeXOrNearest <= edxCarrier.scratchSigned) &&
     (edxCarrier.scratchSigned < g_ModelRaycastMaximumDistance + radiusNodeXOrNearest)) {
    projectionOrDiscriminant = (int64_t)edxCarrier.scratchSigned;
    projectedDistanceWide = (int64_t)edxCarrier.scratchSigned;
    edxCarrier.scratchSigned = FIXED_MUL_HIGH(deltaXOrNodeY,deltaXOrNodeY);
    projectionOrDiscriminant =
         ((int64_t)radiusNodeXOrNearest * (int64_t)radiusNodeXOrNearest +
          projectionOrDiscriminant * projectedDistanceWide) -
            (int64_t)deltaXOrNodeY * (int64_t)deltaXOrNodeY;
    if (-1 < projectionOrDiscriminant) {
      edxCarrier.scratchSigned = FIXED_MUL_HIGH(deltaYOrNodeZ,deltaYOrNodeZ);
      projectionOrDiscriminant = projectionOrDiscriminant - (int64_t)deltaYOrNodeZ * (int64_t)deltaYOrNodeZ;
      if ((-1 < projectionOrDiscriminant) &&
         (edxCarrier.scratchSigned = FIXED_MUL_HIGH(deltaZ,deltaZ)
         , -1 < (int)(((int)((uint64_t)projectionOrDiscriminant >> 32) - edxCarrier.scratchSigned) -
                     (uint32_t)((uint32_t)projectionOrDiscriminant < (uint32_t)((int64_t)deltaZ * (int64_t)deltaZ))))) {
        negatedAngle2 = -modelNodeRuntime->modelPayload.worldRotationAngle2;
        FixedTransform_BuildRotationBasis
                  (&g_GraphicsTransformScratchMatrix3x4,negatedAngle2 & FIXED_ANGLE16_MASK,
                   modelNodeRuntime->modelPayload.worldRotationAngle1,
                   modelNodeRuntime->modelPayload.worldRotationAngle0 + FIXED_ANGLE16_HALF_TURN +
                   negatedAngle2 & FIXED_ANGLE16_MASK);
        resourceView = modelNodeRuntime->modelPayload.modelResource;
        g_GraphicsTransformScratchMatrix3x4.translation.x = 0;
        g_GraphicsTransformScratchMatrix3x4.translation.y = 0;
        g_GraphicsTransformScratchMatrix3x4.translation.z = 0;
        meshGroupCursor = &resourceView->firstMeshGroupRelativeOffset;
        radiusNodeXOrNearest = modelNodeRuntime->worldTransform.translation.x;
        deltaXOrNodeY = modelNodeRuntime->worldTransform.translation.y;
        meshGroupsRemaining = resourceView->meshGroupCount;
        deltaYOrNodeZ = modelNodeRuntime->worldTransform.translation.z;
        /* the mesh group to test: the last one, or the one before it when the model has a shadow mesh group */
        if (resourceView->shadowMeshGroupOffset == 0 ||
            (meshGroupsRemaining = meshGroupsRemaining - 1, meshGroupsRemaining != 0)) {
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
        radiusNodeXOrNearest = MODEL_RAYCAST_NO_HIT_DISTANCE;
        for (meshRecordsRemaining = meshGroupCursor[1]; meshRecordsRemaining != 0; meshRecordsRemaining--) {
          /* triangle points at a ModelMeshHeader here: skip it and its vertex records */
          triangleCountField = &((ModelMeshHeader *)triangle)->triangleCount;
          triangle = (ModelRaycastTriangleDescriptor *)
                     ((uint8_t *)((ModelMeshHeader *)triangle + 1) +
                      ((ModelMeshHeader *)triangle)->vertexCount * MODEL_MESH_RECORD_SIZE);
          for (trianglesRemaining = *triangleCountField; trianglesRemaining != 0; trianglesRemaining--) {
            if (ModelMesh_IntersectTriangleRayDistance(triangle,&triangleDistanceQ12) &&
                (triangleDistanceQ12 <= radiusNodeXOrNearest)) {
              radiusNodeXOrNearest = triangleDistanceQ12;
            }
            triangle = triangle + 1;
          }
        }
        edxCarrier.nearestModelNode = NULL;
        nearestModelNode = modelNodeRuntime;
        for (childrenRemaining = modelNodeRuntime->childCount; childrenRemaining != 0; childrenRemaining--) {
          if (modelNodeRuntime->childNodes[childrenRemaining - 1] != NULL) {
            childDistanceQ12 = ModelNodeRuntime_RaycastHierarchyNearest
                               (modelNodeRuntime->childNodes[childrenRemaining - 1],
                                &edxCarrier.nearestModelNode);
            if ((childDistanceQ12 != MODEL_RAYCAST_NO_HIT_DISTANCE) &&
                (childDistanceQ12 < radiusNodeXOrNearest)) {
              radiusNodeXOrNearest = childDistanceQ12;
              nearestModelNode = edxCarrier.nearestModelNode;
            }
          }
        }
        if (radiusNodeXOrNearest != MODEL_RAYCAST_NO_HIT_DISTANCE) {
          *outNearestModelNode = nearestModelNode;
          return radiusNodeXOrNearest;
        }
      }
    }
  }
  *outNearestModelNode = edxCarrier.nearestModelNode;
  return MODEL_RAYCAST_NO_HIT_DISTANCE;
}


/* Address: 0x0051B650.
   Builds the child models of a new model hierarchy from its MDL definition node: every linked definition list
   yields the variant the faction's technology selects, which is created in the matching child slot and then
   built the same way. CF set (true) when a child cannot be created.
*/
bool ModelNodeRuntime_InstantiateLinkedChildrenRecursive
          (FactionRuntimeIndex factionIndex,GraphicsPaletteAsset *paletteAsset,
          GraphicsTextureSet *textureSet,ModelRuntimeSlot *modelRuntimeSlot,
          ModelDefinitionHierarchyNodeAddress32 definitionNode,WorldRuntimeContext *worldRuntime)

{
  ModelLinkedDefinitionListAddress32 linkedDefinitionList;
  PckModelDefinitionIdCatalog childDefinitionId;
  int linksRemaining;
  ModelRuntimeAttachmentIndex childSlotIndex;
  bool childFailed;
  ModelRuntimeSlot *childModelRuntime;

  /* definition node: +8 link count, +0xC the linked definition lists */
  linksRemaining = *(int *)(definitionNode + 8);
  if (linksRemaining != 0) {
    childSlotIndex = 0;
    do {
      linkedDefinitionList =
           *(ModelLinkedDefinitionListAddress32 *)(definitionNode + 12 + childSlotIndex * 4);
      childDefinitionId =
           ModelDefinition_SelectFactionUnlockedLinkedId(factionIndex,linkedDefinitionList);
      if (!ModelRuntimePool_RepairDeferredChild
                        (paletteAsset,textureSet,childSlotIndex,childDefinitionId,
                         modelRuntimeSlot,worldRuntime,&childModelRuntime)) {
        return true;
      }
      childFailed = ModelNodeRuntime_InstantiateLinkedChildrenRecursive
                        (factionIndex,paletteAsset,textureSet,childModelRuntime,
                         linkedDefinitionList,worldRuntime);
      if (childFailed) {
        return true;
      }
      childSlotIndex++;
      linksRemaining--;
    } while (linksRemaining != 0);
  }
  return false;
}


/* Address: 0x0051BEC0.
   Gives a model node and all its descendants a new palette and texture set; used when two factions merge
   and the absorbed faction's models take the survivor's colours. Unlike
   ModelRuntimeHierarchy_SetPaletteAndTextureSetRecursive it expects a non-NULL node and skips empty child
   slots itself.
*/
void ModelRuntimeHierarchy_SetPaletteAndTextureSetNonNullRecursive
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,
          ModelRuntimeNode *modelNode)

{
  uint32_t childrenRemaining;

  childrenRemaining = modelNode->childCount;
  modelNode->modelPayload.textureSet = textureSet;
  modelNode->modelPayload.paletteAsset = paletteAsset;
  for (; childrenRemaining != 0; childrenRemaining--) {
    if (modelNode->childNodes[0] != NULL) {
      ModelRuntimeHierarchy_SetPaletteAndTextureSetNonNullRecursive
                (paletteAsset,textureSet,modelNode->childNodes[0]);
    }
    /* steps the cursor by one dword, i.e. to the next childNodes[] entry */
    modelNode = (ModelRuntimeNode *)((uint32_t *)modelNode + 1);
  }
  return;
}


/* Address: 0x0051BF30.
   Walks a model runtime hierarchy and clears the target (+0x6C) of every model of class 13 (definition +0x4C)
   that points at targetRuntimeId, so no model keeps aiming at a destroyed object. The dword view: [0] model
   definition, [3] attachment count, [0x1B] target, [0x50 + 8*i] attached child model runtime.
*/
void ModelRuntimeHierarchy_ClearMatchingTargetRecursive(RuntimeToken targetRuntimeId,int *modelRuntime)

{
  int childrenRemaining;
  
  if (modelRuntime != NULL) {
    childrenRemaining = ((ModelRuntimeSlot *)modelRuntime)->attachmentCount;
    if (((ModelRuntimeSlot *)modelRuntime)->definitionOrSavedId.runtimeDefinition->runtimeClassId ==
        MODEL_RUNTIME_CLASS_13 &&
        targetRuntimeId == ((ModelRuntimeSlot *)modelRuntime)->classLinkState.armyLinkOrState.classState) {
      ((ModelRuntimeSlot *)modelRuntime)->classLinkState.armyLinkOrState.classState = 0;
    }
    for (; childrenRemaining != 0; childrenRemaining--) {
      ModelRuntimeHierarchy_ClearMatchingTargetRecursive
                (targetRuntimeId,
                 (int *)((ModelRuntimeSlot *)modelRuntime)->attachments[0].childModelRuntimeOrSavedOffset);
      modelRuntime = modelRuntime + 8; /* next 0x20-byte attachment entry */
    }
  }
  return;
}


/* Address: 0x0051C100.
   Marks every not yet destroyed node of the army's model hierarchy (root model runtime at +0x00) as destroyed, dismantling
   and non-regenerating: sets runtime flags 0x418 (0x400 | 0x10 | 0x08) on each node that does not have flag
   0x08 yet. The world context is not used.
*/
void ModelRuntimeHierarchy_MarkDestroyedRecursive(WorldRuntimeContext *contextArg,ArmyRuntimeSlot *armyRuntime)

{
  /* Rewritten from the assembly (0x0051C100-0x0051C162). */
  (void)contextArg;
  ModelRuntimeHierarchy_MarkDestroyedFrom(armyRuntime->modelRuntimeOrSavedOffset.modelRuntime);
}


/* Model runtime nodes keep their child count at +0x0C and child pointers at +0x140 + 32*i (null
   slots are skipped); the original walks this tree depth-first with frames on the machine stack. */
static int ModelRuntimeHierarchy_SumArmourFrom(ModelRuntimeSlot *node)
{
  int sum = node->health;
  int childCount = node->attachmentCount;
  int i;
  for (i = 0; i < childCount; i++) {
    ModelRuntimeSlot *child = node->attachments[i].childModelRuntimeOrSavedOffset;
    if (child != NULL) {
      sum = sum + ModelRuntimeHierarchy_SumArmourFrom(child);
    }
  }
  return sum;
}

/* Body of ModelRuntimeHierarchy_MarkDestroyedRecursive: ORs 0x418 into the runtime flags (+0xEC) of
   node unless flag 0x08 is already set, then recurses into the non-NULL children (same layout as above). */
static void ModelRuntimeHierarchy_MarkDestroyedFrom(ModelRuntimeSlot *node)
{
  int childCount;
  int childIndex;
  if ((node->classState.stateFlags & ARMY_RUNTIME_FLAG_DESTROYED) == 0) {
    node->classState.stateFlags = node->classState.stateFlags | (ARMY_MODEL_STATE_NO_REGENERATION | ARMY_MODEL_STATE_DISMANTLING | ARMY_RUNTIME_FLAG_DESTROYED);
  }
  childCount = node->attachmentCount;
  for (childIndex = 0; childIndex < childCount; childIndex++) {
    ModelRuntimeSlot *child = node->attachments[childIndex].childModelRuntimeOrSavedOffset;
    if (child != NULL) {
      ModelRuntimeHierarchy_MarkDestroyedFrom(child);
    }
  }
}

/* Address: 0x0051C1F0.
   Returns the armour of a model hierarchy (shown in the in-game selection detail): the sum of the current
   armour points (runtime +0x3C) of every node, walked depth-first.
*/
int ModelRuntimeHierarchy_SumArmour(int *modelRuntimeRoot)

{
  /* Rewritten from the assembly (0x0051C1F0-0x0051C23F). */
  return ModelRuntimeHierarchy_SumArmourFrom((ModelRuntimeSlot *)(uintptr_t)*modelRuntimeRoot);
}


/* Address: 0x00528C20.
   Collects the attachment points of a runtime model from its serialized MDL definition node (nodes whose
   nodeFlags low nibble is not 0 return NULL and are not walked). Per child slot the first transform record of
   kind 0 or 1 naming that slot is searched in the definition's sprite asset; after recursing into the child
   definition, a NULL result from there stores the record in the next of the six attachments[] entries.
   Returns the caller's EDI (modelRuntimeContinuityEdi) otherwise.
*/
ModelRuntimeSlot * ModelRuntimeHierarchy_CollectAttachmentDescriptorsRecursive
          (ModelRuntimeSlot *modelRuntimeContinuityEdi,ModelRuntimeSlot *modelRuntime,
          MdlSerializedNodeHeader *definitionNode)

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
    return NULL;
  }
  childCountRemaining = definitionNode->childCount;
  definitionAssetBase = definitionNode->spriteAssetReference.savedId;
  childIndex = 0;
  for (; childCountRemaining != 0; childCountRemaining--) {
    attachmentTransformCursor =
         (ModelAttachmentTransformRecord *)
         (definitionAssetBase +
         ((ModelResource *)definitionAssetBase)->packedLookupTableRelativeOffset);
    for (transformRecordsRemaining =
             ((ModelResource *)definitionAssetBase)->packedLookupTableEntryCount;
        transformRecordsRemaining != 0;
        transformRecordsRemaining--) {
      /* packedKindAndSelector: kind in bits 0..3, child slot index above */
      attachmentKind = attachmentTransformCursor->packedKindAndSelector & 0xf;
      if (((attachmentKind == 0) || (attachmentKind == 1)) &&
         (childIndex == attachmentTransformCursor->packedKindAndSelector >> 4)) {
        THANDOR_PART(uint32_t, recursiveCollectionResult, 0) =
             ModelRuntimeHierarchy_CollectAttachmentDescriptorsRecursive
                       (modelRuntimeContinuityEdi,modelRuntime,
                        (MdlSerializedNodeHeader *)
                        definitionNode->childSerializedOffsets[childIndex]);
        if (((ModelRuntimeSlot *)recursiveCollectionResult == NULL) &&
           (attachmentSlot = modelRuntime->attachmentCount, attachmentSlot < 6)) {
          modelRuntime->attachmentCount++;
          modelRuntime->attachments[attachmentSlot].sourceTransform = attachmentTransformCursor;
        }
        break;
      }
      attachmentTransformCursor++;
    }
    /* The original advances the child index only when an attachment transform matched
       (DEC EDX before the shared INC EDX when none did). */
    if (transformRecordsRemaining != 0) {
      childIndex++;
    }
  }
  return modelRuntimeContinuityEdi;
}


/* Address: 0x00528E90.
   Builds the runtime node tree of a model from its serialized MDL node tree: allocates a world node per
   definition node, copies the local rotation and the mesh resource, and places each child at the translation
   of the attachment transform record (kind 0 or 1) that names its slot. Definition nodes whose low nibble of
   nodeFlags is set are not instantiated (NULL); for such a child an attachment point is recorded in the model
   runtime so that another model can be attached there later. Returns true with the new node (NULL for a
   non-instantiated definition node) in *outNode, or false when a world node could not be allocated (the
   original returned FATAL_ERROR_GENERAL_FAILURE as its failure value; *outNode is then left unchanged).
*/
bool ModelNodeRuntime_CreateHierarchyRecursive
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,
          ModelRuntimeSlot *modelRuntime,MdlSerializedNodeHeader *definitionNode,
          WorldRuntimeContext *worldRuntime,ModelRuntimeNode **outNode)

{
  AngleTurn32 rotationAngleA;
  AngleTurn32 rotationAngleB;
  ArmyRuntimeSlot *ownerArmy;
  ModelResource *resourceView;
  Q12 radiusOrTranslationY;
  Q12 translationZ;
  SerializedRelativeByteOffset childDefinitionOffset;
  ModelRuntimeNode *newNode;
  uint32_t attachmentKindOrSlot;
  ModelPackedLookupTableEntryCount transformRecordsRemaining;
  uint32_t definitionOrChildrenRemaining;
  uint32_t childIndex;
  ModelAttachmentTransformRecord *attachmentTransform;
  ModelRuntimeNode *childNode;

  if ((definitionNode->nodeFlags & 0xf) != 0) {
    *outNode = NULL;
    return true;
  }
  newNode = (ModelRuntimeNode *)WorldObjectArray_AllocateFreeRecord(worldRuntime);
  if (newNode == NULL) {
    return false; /* world object pool exhausted */
  }
  newNode->ownerClassId = WORLD_OWNER_RUNTIME_MODEL;
  newNode->modelPayload.localTranslationXQ12 = 0;
  newNode->modelPayload.localTranslationYQ12 = 0;
  newNode->modelPayload.localTranslationZQ12 = 0;
  rotationAngleA = definitionNode->localRotationAngle1;
  rotationAngleB = definitionNode->localRotationAngle2;
  newNode->modelPayload.localRotationAngle0 = definitionNode->localRotationAngle0;
  newNode->modelPayload.localRotationAngle1 = rotationAngleA;
  newNode->modelPayload.localRotationAngle2 = rotationAngleB;
  newNode->modelPayload.meshGroupMask = UINT32_MAX;
  ownerArmy = modelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime;
  newNode->runtimePayload.modelRuntime = modelRuntime;
  newNode->runtimeFlags = newNode->runtimeFlags | 1;
  /* 0x20: the army belongs to a faction other than 0 */
  if (ownerArmy->factionIndex != 0) {
    newNode->runtimeFlags = newNode->runtimeFlags | MODEL_NODE_FLAG_FACTION_OWNED;
  }
  /* the four bytes of textureSubresourceBaseIndex are cleared one by one */
  ((uint8_t *)&newNode->textureSubresourceBaseIndex)[0] = 0;
  ((uint8_t *)&newNode->textureSubresourceBaseIndex)[1] = 0;
  ((uint8_t *)&newNode->textureSubresourceBaseIndex)[2] = 0;
  ((uint8_t *)&newNode->textureSubresourceBaseIndex)[3] = 0;
  definitionOrChildrenRemaining = modelRuntime->definitionOrSavedId.savedIdOrOffset;
  newNode->tintArgb = 0xffffffff;
  /* model definition flags (+0x68) 0x10, 0x20 and not 0x40 become node flags 0x10, 0x200 and 0x100 */
  if ((((ModelDefinition *)definitionOrChildrenRemaining)->modelFlags & MODEL_DEFINITION_FLAG_NOT_REMEMBERED) != 0) {
    newNode->runtimeFlags = newNode->runtimeFlags | TERRAIN_OCCUPANCY_FLAG_NOT_REMEMBERED;
  }
  if ((((ModelDefinition *)definitionOrChildrenRemaining)->modelFlags & MODEL_DEFINITION_FLAG_DRAW_BEFORE_TERRAIN) != 0) {
    newNode->runtimeFlags = newNode->runtimeFlags | MODEL_NODE_FLAG_DRAW_BEFORE_TERRAIN;
  }
  if ((((ModelDefinition *)definitionOrChildrenRemaining)->modelFlags & MODEL_DEFINITION_FLAG_NO_SHADING_PASS) == 0) {
    newNode->runtimeFlags = newNode->runtimeFlags | MODEL_NODE_FLAG_SHADING_PASS;
  }
  resourceView = definitionNode->spriteAssetReference.modelResource;
  newNode->modelPayload.paletteAsset = paletteAsset;
  radiusOrTranslationY = resourceView->boundingRadiusQ12;
  newNode->modelPayload.textureSet = textureSet;
  newNode->subtreeBoundingRadiusQ12 = radiusOrTranslationY;
  newNode->modelPayload.modelResource = resourceView;
  newNode->shadingRecord = NULL;
  newNode->modelRuntimeLinkOrSavedOffset = NULL;
  newNode->renderDepthBiasOrState = 0;
  definitionOrChildrenRemaining = definitionNode->childCount;
  resourceView = definitionNode->spriteAssetReference.modelResource;
  childIndex = 0;
  newNode->childCount = definitionOrChildrenRemaining;
  newNode->parentNode = NULL;
  for (; definitionOrChildrenRemaining != 0; definitionOrChildrenRemaining--) {
    attachmentTransform = (ModelAttachmentTransformRecord *)
              ((uint8_t *)resourceView + resourceView->packedLookupTableRelativeOffset);
    /* the first transform record of kind 0 or 1 whose selector (bits 4..31) is this child slot */
    for (transformRecordsRemaining = resourceView->packedLookupTableEntryCount; transformRecordsRemaining != 0;
        transformRecordsRemaining--) {
      attachmentKindOrSlot = attachmentTransform->packedKindAndSelector & 0xf;
      if ((attachmentKindOrSlot == 0 || attachmentKindOrSlot == 1) &&
          childIndex == attachmentTransform->packedKindAndSelector >> 4) {
        if (!ModelNodeRuntime_CreateHierarchyRecursive
                (paletteAsset,textureSet,modelRuntime,
                 (MdlSerializedNodeHeader *)definitionNode->childSerializedOffsets[childIndex],worldRuntime,
                 &childNode)) {
          return false;
        }
        newNode->childNodes[childIndex] = childNode;
        if (childNode == NULL) {
          /* an attachment point: record where the child model will hang */
          attachmentKindOrSlot = modelRuntime->attachmentCount;
          if (attachmentKindOrSlot < MODEL_RUNTIME_ATTACHMENT_CAPACITY) {
            modelRuntime->attachmentCount++;
            modelRuntime->attachments[attachmentKindOrSlot].sourceTransform = attachmentTransform;
            modelRuntime->attachments[attachmentKindOrSlot].childNodeIndex = childIndex;
            modelRuntime->attachments[attachmentKindOrSlot].parentModelNodeOrSavedOffset = newNode;
            childDefinitionOffset = definitionNode->childSerializedOffsets[childIndex];
            modelRuntime->attachments[attachmentKindOrSlot].childModelRuntimeOrSavedOffset = NULL;
            rotationAngleA = ((MdlSerializedNodeHeader *)childDefinitionOffset)->localRotationAngle0;
            rotationAngleB = ((MdlSerializedNodeHeader *)childDefinitionOffset)->localRotationAngle1;
            modelRuntime->attachments[attachmentKindOrSlot].childLocalRotationAngle2 =
                 ((MdlSerializedNodeHeader *)childDefinitionOffset)->localRotationAngle2;
            modelRuntime->attachments[attachmentKindOrSlot].childLocalRotationAngle1 = rotationAngleB;
            modelRuntime->attachments[attachmentKindOrSlot].childLocalRotationAngle0 = rotationAngleA;
          }
        }
        else {
          childNode->parentNode = newNode;
          radiusOrTranslationY = attachmentTransform->localTranslationYQ12;
          translationZ = attachmentTransform->localTranslationZQ12;
          childNode->modelPayload.localTranslationXQ12 = attachmentTransform->localTranslationXQ12;
          childNode->modelPayload.localTranslationYQ12 = radiusOrTranslationY;
          childNode->modelPayload.localTranslationZQ12 = translationZ;
        }
        break;
      }
      attachmentTransform++;
    }
    if (transformRecordsRemaining == 0) {
      /* no attachment transform for this child */
      newNode->childNodes[childIndex] = NULL;
    }
    childIndex++;
  }
  *outNode = newNode;
  return true;
}


/* Address: 0x005294E0.
   Frees a model node and its whole subtree: releases the children first, clears the parent's childNodes[]
   entries that point at this node and finally unlinks the node from its world owner list.
*/
void ModelRuntimeNode_ReleaseRecursiveAndDetachParent(ModelRuntimeNode *node)

{
  uint32_t childrenRemaining;
  uint32_t parentSlotsRemaining;
  ModelRuntimeNode *childSlotCursor;
  
  childSlotCursor = node;
  for (childrenRemaining = node->childCount; childrenRemaining != 0; childrenRemaining--) {
    if (childSlotCursor->childNodes[0] != NULL) {
      ModelRuntimeNode_ReleaseRecursiveAndDetachParent(childSlotCursor->childNodes[0]);
    }
    /* steps the cursor by one dword, i.e. to the next childNodes[] entry */
    childSlotCursor = (ModelRuntimeNode *)((uint32_t *)childSlotCursor + 1);
  }
  childSlotCursor = node->parentNode;
  if (childSlotCursor != NULL) {
    for (parentSlotsRemaining = childSlotCursor->childCount; parentSlotsRemaining != 0; parentSlotsRemaining--) {
      if (childSlotCursor->childNodes[0] == node) {
        childSlotCursor->childNodes[0] = NULL;
      }
      childSlotCursor = (ModelRuntimeNode *)((uint32_t *)childSlotCursor + 1);
    }
  }
  WorldRuntime_UnlinkOwnerListNode((WorldOwnerListNode *)node);
  return;
}


/* Address: 0x0052A100.
   Folds one model runtime and its attached children into the owning army's selection figures (cleared by
   ArmyRuntime_RebuildDerivedSelectionMetrics): maxima at army +0x90, +0x44 and +0x48, the largest shot selection
   range at +0x4C and, for armed models, the shot's impact damage per target class summed into army +0x100[8].
   The dword view: [0] model definition, [1] linked runtime, [2] army, [3] attachment count, [0x3B] class
   state flags (+0xEC), [0x50 + 8*i] attached child model runtime.
*/
void ModelRuntimeHierarchy_AccumulateDerivedSelectionMetrics(int *modelRuntime)

{
  int linkedRuntime;
  ShotDefinition *definition;
  uint32_t metricValue;
  uint32_t selectionRange;
  int armyOrCategoryIndex;
  int definitionArmyOrRemaining;
  
  definitionArmyOrRemaining = *modelRuntime;
  armyOrCategoryIndex = (int)((ModelRuntimeSlot *)modelRuntime)->ownerArmyRuntimeOrSavedOffset.armyRuntime;
  linkedRuntime = (int)((ModelRuntimeSlot *)modelRuntime)->rootModelNodeOrSavedOffset.modelNode;
  /* a switched-off model counts with the definition's alternative value at +0x1A4 */
  if ((((ModelRuntimeSlot *)modelRuntime)->classState.stateFlags & ARMY_MODEL_STATE_SWITCHED_OFF) == 0) {
    metricValue = ((ModelDefinition *)definitionArmyOrRemaining)->visibilityRadius;
  }
  else {
    metricValue = ((ModelDefinition *)definitionArmyOrRemaining)->switchedOffVisibilityRadius;
  }
  if (((ArmyRuntimeSlot *)armyOrCategoryIndex)->occupancyMarkRadius <
      ((ModelDefinition *)definitionArmyOrRemaining)->occupancyMarkRadius) {
    ((ArmyRuntimeSlot *)armyOrCategoryIndex)->occupancyMarkRadius =
         ((ModelDefinition *)definitionArmyOrRemaining)->occupancyMarkRadius;
  }
  if (((ArmyRuntimeSlot *)armyOrCategoryIndex)->visibilityRadius < metricValue) {
    ((ArmyRuntimeSlot *)armyOrCategoryIndex)->visibilityRadius = metricValue;
  }
  metricValue = (((ModelRuntimeNode *)linkedRuntime)->worldTransform.translation.z -
                ((ArmyRuntimeSlot *)armyOrCategoryIndex)->modelNodeRuntime->worldTransform.translation.z) +
                ((ModelDefinition *)definitionArmyOrRemaining)->visibilityHeightOffset;
  if (((ArmyRuntimeSlot *)armyOrCategoryIndex)->visibilityHeightOffset < metricValue) {
    ((ArmyRuntimeSlot *)armyOrCategoryIndex)->visibilityHeightOffset = metricValue;
  }
  /* the model's shot definition, used when reloadTicks is non-zero */
  definition = ((ModelDefinition *)definitionArmyOrRemaining)->shotDefinitionReference.definition;
  if (((ModelDefinition *)definitionArmyOrRemaining)->reloadTicks != 0) {
    selectionRange = ShotDefinition_ComputeSelectionRange(definition);
    definitionArmyOrRemaining = (int)((ModelRuntimeSlot *)modelRuntime)->ownerArmyRuntimeOrSavedOffset.armyRuntime;
    armyOrCategoryIndex = 7;
    if ((int)((ArmyRuntimeSlot *)definitionArmyOrRemaining)->weaponRangeQ12 < (int)selectionRange) {
      ((ArmyRuntimeSlot *)definitionArmyOrRemaining)->weaponRangeQ12 = selectionRange;
    }
    do {
      ((ArmyRuntimeSlot *)definitionArmyOrRemaining)->targetClassShotDamage[armyOrCategoryIndex] +=
           definition->targetClassImpactDamageQ12[armyOrCategoryIndex];
      armyOrCategoryIndex--;
    } while (-1 < armyOrCategoryIndex);
  }
  for (definitionArmyOrRemaining = ((ModelRuntimeSlot *)modelRuntime)->attachmentCount;
       definitionArmyOrRemaining != 0; definitionArmyOrRemaining--) {
    if (((ModelRuntimeSlot *)modelRuntime)->attachments[0].childModelRuntimeOrSavedOffset != NULL) {
      ModelRuntimeHierarchy_AccumulateDerivedSelectionMetrics
                ((int *)((ModelRuntimeSlot *)modelRuntime)->attachments[0].childModelRuntimeOrSavedOffset);
    }
    modelRuntime = modelRuntime + 8; /* next 0x20-byte attachment entry */
  }
  return;
}


/* Address: 0x0052A690.
   Condition of a model hierarchy as a Q12 ratio: the node's armour points (+0x3C) relative to its
   definition's maximum (+0x60), multiplied by the average of 1.0 and the ratios of all attached child
   hierarchies; Q12_ONE is full condition. Unrelated to the draw scale at node +0xC0.
*/
Q12 ModelRuntimeHierarchy_ComputeConditionRatioQ12(ModelRuntimeSlot *modelRuntime)

{
  ModelRuntimeSlot *childModelRuntime;
  uint32_t attachmentsRemaining;
  int accumulatedHierarchyScaleQ12;
  ModelRuntimeSlot *attachmentDescriptorCursor;
  int scaleSampleCount;
  Q12 childConditionRatioQ12;

  accumulatedHierarchyScaleQ12 = Q12_ONE;
  scaleSampleCount = 1;
  attachmentDescriptorCursor = modelRuntime;
  for (attachmentsRemaining = modelRuntime->attachmentCount; attachmentsRemaining != 0;
      attachmentsRemaining--) {
    childModelRuntime = attachmentDescriptorCursor->attachments[0].childModelRuntimeOrSavedOffset;
    if (childModelRuntime != NULL) {
      childConditionRatioQ12 = ModelRuntimeHierarchy_ComputeConditionRatioQ12(childModelRuntime);
      accumulatedHierarchyScaleQ12 = accumulatedHierarchyScaleQ12 + childConditionRatioQ12;
      scaleSampleCount++;
    }
    /* steps the cursor by one 0x20-byte attachments[] entry */
    attachmentDescriptorCursor =
         (ModelRuntimeSlot *)((uint8_t *)attachmentDescriptorCursor + sizeof(ModelRuntimeAttachmentDescriptor));
  }
  return (Q12)(((int64_t)(int)modelRuntime->health * (int64_t)accumulatedHierarchyScaleQ12) /
               (int64_t)(scaleSampleCount *
                         (int)modelRuntime->definitionOrSavedId.runtimeDefinition->maximumHealth));
}


/* Address: 0x0052A6F0.
   Energy demand of a model and its directly attached models (value +0xF4): totalQ4 is the whole demand,
   activeQ4 only the part of models not switched off (stateFlags bit 0). Attached models count only when the
   definition has flag 0x80 at +0x68; the walk is one level deep, not recursive.
*/
ModelHierarchyEnergyDemand
ModelRuntimeHierarchy_ComputeEnergyDemand(ModelRuntimeSlot *modelRuntime)

{
  uint32_t activeMetricTotal;
  uint32_t attachmentsRemaining;
  uint32_t totalMetric;
  ModelRuntimeSlot *currentChildModelRuntime;
  uint32_t childMetric;
  ModelHierarchyEnergyDemand energyDemand;

  totalMetric = modelRuntime->classState.energyLoadQ4;
  attachmentsRemaining = modelRuntime->attachmentCount;
  activeMetricTotal = 0;
  if ((modelRuntime->classState.stateFlags & ARMY_MODEL_STATE_SWITCHED_OFF) == 0) {
    activeMetricTotal = totalMetric;
  }
  if ((modelRuntime->definitionOrSavedId.runtimeDefinition->modelFlags &
       MODEL_DEFINITION_FLAG_COUNT_ATTACHED_ENERGY) != 0) {
    for (; attachmentsRemaining != 0; attachmentsRemaining--) {
      currentChildModelRuntime = modelRuntime->attachments[0].childModelRuntimeOrSavedOffset;
      if (currentChildModelRuntime != NULL) {
        childMetric = currentChildModelRuntime->classState.energyLoadQ4;
        if ((currentChildModelRuntime->classState.stateFlags & ARMY_MODEL_STATE_SWITCHED_OFF) == 0) {
          activeMetricTotal = activeMetricTotal + childMetric;
        }
        totalMetric = totalMetric + childMetric;
      }
      /* steps the cursor by one 0x20-byte attachments[] entry */
      modelRuntime = (ModelRuntimeSlot *)((uint8_t *)modelRuntime + sizeof(ModelRuntimeAttachmentDescriptor));
    }
  }
  energyDemand.activeQ4 = activeMetricTotal;
  energyDemand.totalQ4 = totalMetric;
  return energyDemand;
}

/* Address: 0x0052AAC0.
   Turns a weapon or turret node's yaw (localRotationAngle2) toward targetYawAngle16 over the shorter way, for
   the army aim updates (ArmyRuntimeClass_UpdateSingleBarrelTurret/B,
   ArmyRuntimeWeapon_UpdateTargetAimAndFireAttachments): the turn velocity grows by the weapon definition's
   acceleration up to its rate limit and is reset when it points away; the target is taken exactly once it is
   within one step. Returns true while the remaining difference exceeds +-MODEL_AIM_TOLERANCE_ANGLE16 (still
   outside the aim tolerance), false once the yaw is within it or on the target.
*/

bool ModelNodeRuntime_SmoothYawTowardTarget
          (ModelRuntimeNode *modelNodeRuntime,ModelRuntimeWeaponAimStateView *smoothingState,
          AngleTurn32 targetYawAngle16)

{
  ArmyWeaponDefinitionView *aimDefinition;
  int turnRateLimit;
  AngleTurn32 currentYawAngle;
  uint32_t yawAngle;
  uint32_t yawStep;
  int acceleratedVelocity;
  uint32_t yawDelta;
  bool snapToTarget;
  uint32_t remainingYawDelta;

  yawAngle = modelNodeRuntime->modelPayload.localRotationAngle2;
  aimDefinition = smoothingState->modelDefinition;
  yawDelta = targetYawAngle16 - yawAngle & FIXED_ANGLE16_MASK;
  yawStep = smoothingState->yawTurnVelocityAngle16 * g_InGameSimulationStepTicks;
  snapToTarget = false;
  if (yawDelta < FIXED_ANGLE16_HALF_TURN + 1) {
    /* target ahead in the positive direction */
    if ((int)yawStep < 0) {
      smoothingState->yawTurnVelocityAngle16 = 0; /* turning away: stop */
    }
    else if (yawDelta <= yawStep) {
      snapToTarget = true;
    }
    else {
      yawAngle = yawAngle + yawStep;
      turnRateLimit = aimDefinition->yawTurnRateLimitAnglePerTick;
      acceleratedVelocity = smoothingState->yawTurnVelocityAngle16 +
              g_InGameSimulationStepTicks * aimDefinition->yawTurnRateAccelerationAnglePerTick;
      smoothingState->yawTurnVelocityAngle16 = turnRateLimit;
      if (acceleratedVelocity < turnRateLimit) {
        smoothingState->yawTurnVelocityAngle16 = acceleratedVelocity;
      }
    }
  }
  else if (0 < (int)yawStep) {
    smoothingState->yawTurnVelocityAngle16 = 0; /* turning away: stop */
  }
  else if (yawStep + FIXED_ANGLE16_FULL_TURN <= yawDelta) {
    snapToTarget = true;
  }
  else {
    yawAngle = yawAngle + yawStep;
    turnRateLimit = aimDefinition->yawTurnRateLimitAnglePerTick;
    acceleratedVelocity = smoothingState->yawTurnVelocityAngle16 -
            g_InGameSimulationStepTicks * aimDefinition->yawTurnRateAccelerationAnglePerTick;
    smoothingState->yawTurnVelocityAngle16 = -turnRateLimit;
    if (-turnRateLimit < acceleratedVelocity) {
      smoothingState->yawTurnVelocityAngle16 = acceleratedVelocity;
    }
  }
  if (snapToTarget) {
    /* the target is reached within this step */
    currentYawAngle = modelNodeRuntime->modelPayload.localRotationAngle2;
    smoothingState->yawTurnVelocityAngle16 = 0;
    if (targetYawAngle16 != currentYawAngle) {
      modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
      modelNodeRuntime->modelPayload.localRotationAngle2 = targetYawAngle16;
    }
  }
  else {
    modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
    modelNodeRuntime->modelPayload.localRotationAngle2 = yawAngle & FIXED_ANGLE16_MASK;
    remainingYawDelta = (yawAngle & 0xffff) - targetYawAngle16 & FIXED_ANGLE16_MASK;
    if (MODEL_AIM_TOLERANCE_ANGLE16 < remainingYawDelta &&
        remainingYawDelta < FIXED_ANGLE16_FULL_TURN - MODEL_AIM_TOLERANCE_ANGLE16) {
      return true; /* still outside the aim tolerance */
    }
  }
  return false;
}


/* Address: 0x0052AC00.
   Pitch counterpart of ModelNodeRuntime_SmoothYawTowardTarget (same callers): clamps the target to the weapon
   definition's pitch range, then moves localRotationAngle1 toward it with the same accelerate/limit/stop rules,
   without wrap-around. Returns the clamped target pitch when the node is on it (reached within this step or
   already there), otherwise the remaining difference (pitch - clamped target) & FIXED_ANGLE16_MASK.
   Original quirk: the callers compare this value with the unclamped target pitch to decide "aimed", so a
   remaining difference that happens to equal the target also counts (the original also returned the in/out of
   aim tolerance flag, which nobody reads).
*/

uint32_t ModelNodeRuntime_SmoothPitchTowardTarget
          (ModelRuntimeNode *modelNodeRuntime,ModelRuntimeWeaponAimStateView *smoothingState,
          AngleTurn32 targetPitchAngle16)

{
  ArmyWeaponDefinitionView *aimDefinition;
  uint32_t pitchAngle;
  int pitchStepOrRateLimit;
  int acceleratedVelocity;
  bool snapToTarget;
  uint32_t clampedTargetOrRemaining;

  pitchAngle = modelNodeRuntime->modelPayload.localRotationAngle1;
  aimDefinition = smoothingState->modelDefinition;
  clampedTargetOrRemaining = targetPitchAngle16;
  if ((int)aimDefinition->maximumPitchAngle < (int)targetPitchAngle16) {
    clampedTargetOrRemaining = aimDefinition->maximumPitchAngle;
  }
  if ((int)clampedTargetOrRemaining < (int)aimDefinition->minimumPitchAngle) {
    clampedTargetOrRemaining = aimDefinition->minimumPitchAngle;
  }
  pitchStepOrRateLimit = smoothingState->pitchTurnVelocityAngle16 * g_InGameSimulationStepTicks;
  snapToTarget = true; /* already there, or reached within this step */
  if (clampedTargetOrRemaining != pitchAngle) {
    if ((int)pitchAngle <= (int)clampedTargetOrRemaining) {
      /* target above */
      if (pitchStepOrRateLimit < 0) {
        smoothingState->pitchTurnVelocityAngle16 = 0; /* moving away: stop */
        snapToTarget = false;
      }
      else if (pitchStepOrRateLimit < (int)(clampedTargetOrRemaining - pitchAngle)) {
        pitchAngle = pitchAngle + pitchStepOrRateLimit;
        pitchStepOrRateLimit = aimDefinition->pitchTurnRateLimitAnglePerTick;
        acceleratedVelocity = smoothingState->pitchTurnVelocityAngle16 +
                g_InGameSimulationStepTicks * aimDefinition->pitchTurnRateAccelerationAnglePerTick;
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
    else if ((int)(clampedTargetOrRemaining - pitchAngle) < pitchStepOrRateLimit) {
      pitchAngle = pitchAngle + pitchStepOrRateLimit;
      pitchStepOrRateLimit = aimDefinition->pitchTurnRateLimitAnglePerTick;
      acceleratedVelocity = smoothingState->pitchTurnVelocityAngle16 -
              g_InGameSimulationStepTicks * aimDefinition->pitchTurnRateAccelerationAnglePerTick;
      smoothingState->pitchTurnVelocityAngle16 = -pitchStepOrRateLimit;
      if (-pitchStepOrRateLimit < acceleratedVelocity) {
        smoothingState->pitchTurnVelocityAngle16 = acceleratedVelocity;
      }
      snapToTarget = false;
    }
  }
  if (!snapToTarget) {
    modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
    modelNodeRuntime->modelPayload.localRotationAngle1 = pitchAngle;
    clampedTargetOrRemaining = pitchAngle - clampedTargetOrRemaining & FIXED_ANGLE16_MASK;
  }
  else {
    pitchAngle = modelNodeRuntime->modelPayload.localRotationAngle1;
    smoothingState->pitchTurnVelocityAngle16 = 0;
    if (clampedTargetOrRemaining != pitchAngle) {
      modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
      modelNodeRuntime->modelPayload.localRotationAngle1 = clampedTargetOrRemaining;
    }
  }
  return clampedTargetOrRemaining;
}


/* Address: 0x004BD1A0.
   Sets the packed ARGB tint of a model node and of all its descendants (the state tint of a whole model,
   see ModelNodeRuntime_UpdateStateTintRecursive).
*/
void ModelNodeRuntime_ApplyTintRecursive(PackedArgb32 tintArgb,ModelRuntimeNode *modelNode)

{
  uint32_t childrenRemaining;
  
  childrenRemaining = modelNode->childCount;
  modelNode->tintArgb = tintArgb;
  for (; childrenRemaining != 0; childrenRemaining--) {
    if (modelNode->childNodes[0] != NULL) {
      ModelNodeRuntime_ApplyTintRecursive(tintArgb,modelNode->childNodes[0]);
    }
    /* steps the cursor by one dword, i.e. to the next childNodes[] entry */
    modelNode = (ModelRuntimeNode *)((uint32_t *)modelNode + 1);
  }
  return;
}


/* Address: 0x004BE390.
   Computes the world transforms of a model hierarchy: a root node first gets its rotation basis from its world
   angles; then every child's world transform = parent world transform x child local transform, the child's world
   Euler angles are extracted from it, the parent's tint is inherited, and the child's subtree is processed.
*/
void ModelNodeRuntime_ComposeChildTransformsRecursive(ModelRuntimeNode *modelNodeRuntime)

{
  ModelRuntimeNode *currentChild;
  uint32_t childIndex;
  FixedRollAzimuthElevation childEulerAngles;
  PackedArgb32 inheritedTintArgb;

  childIndex = 0;
  if (modelNodeRuntime->parentNode == NULL) {
    FixedTransform_BuildRotationBasis
              (&modelNodeRuntime->worldTransform,
               modelNodeRuntime->modelPayload.worldRotationAngle2,
               modelNodeRuntime->modelPayload.worldRotationAngle1,
               modelNodeRuntime->modelPayload.worldRotationAngle0);
  }
  if (modelNodeRuntime->childCount != 0) {
    do {
      currentChild = modelNodeRuntime->childNodes[childIndex];
      childIndex++;
      if (currentChild != NULL) {
        /* the scratch matrix plus the translation globals form the child's local transform */
        FixedTransform_BuildRotationBasis
                  ((GraphicsFixedMatrix3x4 *)&g_ModelTransformScratchMatrix,
                   currentChild->modelPayload.localRotationAngle2,
                   currentChild->modelPayload.localRotationAngle1,
                   currentChild->modelPayload.localRotationAngle0);
        g_ModelTransformTranslationX = currentChild->modelPayload.localTranslationXQ12;
        g_ModelTransformTranslationY = currentChild->modelPayload.localTranslationYQ12;
        g_ModelTransformTranslationZ = currentChild->modelPayload.localTranslationZQ12;
        FixedTransform_Compose
                  (&currentChild->worldTransform,
                   (GraphicsFixedMatrix3x4 *)&g_ModelTransformScratchMatrix,
                   &modelNodeRuntime->worldTransform);
        childEulerAngles = FixedTransform_ExtractEulerAngles(&currentChild->worldTransform);
        currentChild->modelPayload.worldRotationAngle2 = childEulerAngles.rollAngle;
        currentChild->modelPayload.worldRotationAngle0 = childEulerAngles.azimuthAngle;
        currentChild->modelPayload.worldRotationAngle1 = childEulerAngles.elevationAngle;
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
   Switches every node of a model hierarchy to the first of the (up to six) variant definitions listed in its
   definition (+0x238) that the faction's technology unlocks. The armour points (+0x3C) are rescaled to the new
   definition's maximum (+0x60) so the condition stays the same, and the army's derived metrics are rebuilt.
*/
void ModelRuntimeHierarchy_ApplyFactionTechnologyVariantsRecursive(FactionRuntimeIndex factionIndex,int *modelRuntime)

{
  PckModelDefinitionIdCatalog modelDefinitionId;
  int variantsRemaining;
  int variantCursorOrRemaining;
  bool variantLocked;
  ModelDefinitionRecordPrefix *variantDefinition;

  /* modelRuntime is a ModelRuntimeSlot; [0] its definition */
  variantCursorOrRemaining = *modelRuntime;
  variantsRemaining = MODEL_TECHNOLOGY_VARIANT_COUNT;
  do {
    /* the cursor steps 4 bytes per variant, so element 0 is the current variant */
    modelDefinitionId =
         ((ModelDefinition *)variantCursorOrRemaining)->variantModelDefinitionIds[0];
    /* ModelDefinition_IsFactionTechnologyLocked returns true (CF set) when the variant is NOT unlocked */
    if ((modelDefinitionId != 0) &&
       (variantLocked = ModelDefinition_IsFactionTechnologyLocked
                          (g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits,
                           modelDefinitionId), !variantLocked)) {
      /* swap to this variant; armour points scale with the new maximum */
      /* always found: an unknown id counts as locked */
      variantDefinition = ModelDefinitionRegistry_FindById(modelDefinitionId);
      variantCursorOrRemaining = *modelRuntime;
      *modelRuntime = (int)variantDefinition;
      ((ModelRuntimeSlot *)modelRuntime)->health =
           (int)(((int64_t)(int)((ModelRuntimeSlot *)modelRuntime)->health *
                 (int64_t)(int)((ModelDefinition *)variantDefinition)->maximumHealth) /
                (int64_t)(int)((ModelDefinition *)variantCursorOrRemaining)->maximumHealth);
      ArmyRuntime_RebuildDerivedSelectionMetrics
                (((ModelRuntimeSlot *)modelRuntime)->ownerArmyRuntimeOrSavedOffset.armyRuntime);
      break;
    }
    variantCursorOrRemaining = variantCursorOrRemaining + 4;
    variantsRemaining--;
  } while (variantsRemaining != 0);
  for (variantCursorOrRemaining = ((ModelRuntimeSlot *)modelRuntime)->attachmentCount;
       variantCursorOrRemaining != 0; variantCursorOrRemaining--) {
    if (((ModelRuntimeSlot *)modelRuntime)->attachments[0].childModelRuntimeOrSavedOffset != NULL) {
      ModelRuntimeHierarchy_ApplyFactionTechnologyVariantsRecursive
                (factionIndex,(int *)((ModelRuntimeSlot *)modelRuntime)->attachments[0].childModelRuntimeOrSavedOffset);
    }
    modelRuntime = modelRuntime + 8; /* next 0x20-byte attachment entry */
  }
  return;
}

