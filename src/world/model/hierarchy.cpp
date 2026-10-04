/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/model/hierarchy.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/model/hierarchy.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

int32_t g_ModelBoundsMinimumX = 0;

int32_t g_ModelBoundsMaximumX = 0;

int32_t g_ModelBoundsMinimumY = 0;

int32_t g_ModelBoundsMaximumY = 0;

int32_t g_ModelBoundsMinimumZ = 0;

int32_t g_ModelBoundsMaximumZ = 0;

static GraphicsFixedVec3 g_ModelBoundsTransformedPoint = {0};

GraphicsFixedMatrix3x4 g_ModelTransformScratchMatrix = {0};

/* Implementation ownership: world/model/hierarchy. */

/* Fades the model's tint one step toward the target its state flags ask for and applies it to the whole
   hierarchy (called by the army terrainStateRefresh maintenance phase in gameplay/army/class_dispatch.cpp). Targets:
   flag 4 white and opaque; else flag 8 with 0x10 white and transparent, flag 8 alone grey 0x87 and opaque,
   neither black and transparent; flag 0x1000 always makes it transparent. The step limit comes from the
   intensity clamp table (GraphicsIntensityClampTable_Initialize).
*/
void ModelNodeRuntime_UpdateStateTintRecursive(ModelRuntimeNode *modelNodeRuntime)

{
  uint8_t *clampTable;
  uint8_t clampedColorByte;
  uint8_t clampedAlphaByte;
  uint32_t runtimeFlags;
  uint32_t previousTint;
  int colorIntensity;
  PackedArgb32 tintArgb;
  int alphaIntensity;

  colorIntensity = 255;
  alphaIntensity = 255;
  runtimeFlags = modelNodeRuntime->runtimeFlags;
  if ((runtimeFlags & TERRAIN_OCCUPANCY_FLAG_PRESENT) == 0) {
    colorIntensity = 0;
    alphaIntensity = 0;
    if ((runtimeFlags & TERRAIN_OCCUPANCY_FLAG_SEEN_BEFORE) != 0) {
      colorIntensity = 255;
      alphaIntensity = 0;
      if ((runtimeFlags & TERRAIN_OCCUPANCY_FLAG_NOT_REMEMBERED) == 0) {
        colorIntensity = 135;
        alphaIntensity = 255;
      }
    }
  }
  if ((runtimeFlags & MODEL_NODE_FLAG_FORCE_TRANSPARENT) != 0) {
    alphaIntensity = 0;
  }
  previousTint = modelNodeRuntime->tintArgb;
  /* The clamp table is 64-KiB aligned: the target intensity is the low index byte and the previous tint
     byte the high one, i.e. it is indexed with (previous << 8) | target. */
  clampTable = (uint8_t *)g_GraphicsIntensityClampTableBase;
  clampedColorByte = clampTable[(int32_t)(((previousTint >> 16) & 0xff) << 8 | (uint32_t)colorIntensity)];
  clampedAlphaByte = clampTable[(int32_t)((previousTint >> 24) << 8 | (uint32_t)alphaIntensity)];
  tintArgb = (uint32_t)clampedAlphaByte << 24 | (uint32_t)clampedColorByte << 16 | (uint32_t)clampedColorByte << 8 |
             (uint32_t)clampedColorByte;
  /* The original compares with the previous tint shifted right by 16, so the new
     tint is applied on practically every call, not only when it changed. */
  if (tintArgb != previousTint >> 16) {
    ModelNodeRuntime_ApplyTintRecursive(tintArgb,modelNodeRuntime);
  }
}


/* Recomputes the world transforms of a model hierarchy after its local animation, aim, recoil or
   translation changed: composes from the node's parent if it has one, else from the node itself. Only one
   level is climbed, so callers pass a root node or a direct child of it.
*/
void ModelNodeRuntime_RebuildTransformsFromRoot(ModelRuntimeNode *modelNodeRuntime)

{
  if (modelNodeRuntime->parentNode == nullptr) {
    ModelNodeRuntime_ComposeChildTransformsRecursive(modelNodeRuntime);
  }
  else {
    ModelNodeRuntime_ComposeChildTransformsRecursive(modelNodeRuntime->parentNode);
  }
}


/* Gives a model node and all its descendants the palette and texture set. Only
   called by itself in the executable; the faction code uses
   ModelRuntimeHierarchy_SetPaletteAndTextureSetNonNullRecursive.
*/
void ModelRuntimeHierarchy_SetPaletteAndTextureSetRecursive
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,ModelRuntimeNode *node)

{
  uint32_t childrenRemaining;

  if (node != nullptr) {
    node->modelPayload.textureSet = textureSet;
    node->modelPayload.paletteAsset = paletteAsset;
    for (childrenRemaining = node->childCount; childrenRemaining != 0; childrenRemaining = childrenRemaining - 1) {
      ModelRuntimeHierarchy_SetPaletteAndTextureSetRecursive
                (paletteAsset,textureSet,node->childNodes[0]);
      /* the original steps the node pointer by 4 bytes, so childNodes[0] walks through all children */
      node = (ModelRuntimeNode *)((uint32_t *)node + 1);
    }
  }
}


/* Grows the global model bounding box (g_ModelBoundsMinimum/Maximum X/Y/Z) by every mesh vertex of the node,
   transformed by the node's world transform, and then by all its descendants. The caller seeds the box first.
*/
void ModelNodeRuntime_AccumulateTransformedBoundsRecursive(ModelRuntimeNode *modelNode)

{
  ModelResource *resourceView;
  uint32_t childrenRemaining;
  int verticesRemaining;
  int childIndex;
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
      for (verticesRemaining = *(int *)(geometryRecord + 8); verticesRemaining != 0; verticesRemaining--) {
        FixedTransform_ApplyPoint
                  (&g_ModelBoundsTransformedPoint,point,
                   &modelNode->worldTransform);
        if (g_ModelBoundsTransformedPoint.x < g_ModelBoundsMinimumX) {
          g_ModelBoundsMinimumX = g_ModelBoundsTransformedPoint.x;
        }
        else if (g_ModelBoundsMaximumX < g_ModelBoundsTransformedPoint.x) {
          g_ModelBoundsMaximumX = g_ModelBoundsTransformedPoint.x;
        }
        if (g_ModelBoundsTransformedPoint.y < g_ModelBoundsMinimumY) {
          g_ModelBoundsMinimumY = g_ModelBoundsTransformedPoint.y;
        }
        else if (g_ModelBoundsMaximumY < g_ModelBoundsTransformedPoint.y) {
          g_ModelBoundsMaximumY = g_ModelBoundsTransformedPoint.y;
        }
        if (g_ModelBoundsTransformedPoint.z < g_ModelBoundsMinimumZ) {
          g_ModelBoundsMinimumZ = g_ModelBoundsTransformedPoint.z;
        }
        else if (g_ModelBoundsMaximumZ < g_ModelBoundsTransformedPoint.z) {
          g_ModelBoundsMaximumZ = g_ModelBoundsTransformedPoint.z;
        }
        point = (GraphicsFixedVec3 *)((uint8_t *)point + 64); /* the next vertex */
      }
      geometryRecord = geometryRecord + *(int *)geometryRecord;
    }
  }
  childIndex = 0;
  for (childrenRemaining = modelNode->childCount; childrenRemaining != 0; childrenRemaining--) {
    if (modelNode->childNodes[childIndex] != nullptr) {
      ModelNodeRuntime_AccumulateTransformedBoundsRecursive(modelNode->childNodes[childIndex]);
    }
    childIndex++;
  }
}


/* Turns a mesh group towards the camera around the model's own up axis (mesh group flag 1 in
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
}


/* Turns a mesh group fully towards the camera, a billboard (mesh group flag 2 in
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
}


/* Recomputes the bounding radius of a model node's subtree, children first: the largest of the node's own
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
    if (childNode != nullptr) {
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
}


/* Updates the coarse position bins of a model node after it moved: one bit mask along world x and one along
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
}


/* Transforms a model-local point record (anchor, launch or marker point) into world coordinates through the
   node's world transform. The point is written to g_ModelTransformOutput and also returned.
*/
ModelWorldPoint
ModelNodeRuntime_TransformLocalPoint
          (ModelPackedPointRecord *localPointRecord,ModelRuntimeNode *modelNodeRuntime)

{
  ModelWorldPoint transformedPoint;

  FixedTransform_ApplyPoint
            (&g_ModelTransformOutput,&localPointRecord->localPosition,
             &modelNodeRuntime->worldTransform);
  transformedPoint.yQ12 = g_ModelTransformOutput.y;
  transformedPoint.xQ12 = g_ModelTransformOutput.x;
  transformedPoint.zQ12 = g_ModelTransformOutput.z;
  return transformedPoint;
}


/* Converts a world direction (elevation, azimuth) into the frame of a model node for aiming turrets and weapons
   (ArmyRuntimeClass_UpdateSingleBarrelTurret/B, ArmyRuntimeWeapon_UpdateTargetAimAndFireAttachments):
   rotates a unit vector by the inverse of the node's world rotation and returns its angles, the yaw made
   relative by adding the node's local rotation angle 2 (modelPayload.localRotationAngle2).
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


/* Builds the child models of a new model hierarchy from its MDL definition node: every linked definition list
   yields the variant the faction's technology selects, which is created in the matching child slot and then
   built the same way. Returns true when a child cannot be created.
*/
Bool8 ModelNodeRuntime_InstantiateLinkedChildrenRecursive
          (FactionRuntimeIndex factionIndex,GraphicsPaletteAsset *paletteAsset,
          GraphicsTextureSet *textureSet,ModelRuntimeSlot *modelRuntimeSlot,
          ModelDefinitionHierarchyNodeAddress32 definitionNode,WorldRuntimeContext *worldRuntime)

{
  ModelLinkedDefinitionListAddress32 linkedDefinitionList;
  PckModelDefinitionIdCatalog childDefinitionId;
  int linksRemaining;
  ModelRuntimeAttachmentIndex childSlotIndex;
  Bool8 childFailed;
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


/* Gives a model node and all its descendants a new palette and texture set; used when two factions merge
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
    if (modelNode->childNodes[0] != nullptr) {
      ModelRuntimeHierarchy_SetPaletteAndTextureSetNonNullRecursive
                (paletteAsset,textureSet,modelNode->childNodes[0]);
    }
    /* steps the cursor by one dword, i.e. to the next childNodes[] entry */
    modelNode = (ModelRuntimeNode *)((uint32_t *)modelNode + 1);
  }
}


/* Walks a model runtime hierarchy and clears the target (classLinkState.armyLinkOrState.classState)
   of every model of class 13 (ModelDefinition.runtimeClassId) that points at targetRuntimeId, so no model keeps
   aiming at a destroyed object.
*/
void ModelRuntimeHierarchy_ClearMatchingTargetRecursive(const void *targetRuntimeId,int *modelRuntime)

{
  ModelRuntimeSlot *modelRuntimeSlot;
  ModelRuntimeAttachmentDescriptor *attachment;
  int childrenRemaining;

  if (modelRuntime == nullptr) {
    return;
  }
  modelRuntimeSlot = (ModelRuntimeSlot *)modelRuntime;
  childrenRemaining = modelRuntimeSlot->attachmentCount;
  if (modelRuntimeSlot->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_13 &&
      targetRuntimeId == modelRuntimeSlot->classLinkState.armyLinkOrState.armyRuntime) {
    /* class 13 links its army here (the classState view of the same pointer in the original) */
    modelRuntimeSlot->classLinkState.armyLinkOrState.armyRuntime = nullptr;
  }
  /* empty attachment slots are passed on too; the callee returns at once for NULL */
  attachment = modelRuntimeSlot->attachments;
  for (; childrenRemaining != 0; childrenRemaining--) {
    ModelRuntimeHierarchy_ClearMatchingTargetRecursive
              (targetRuntimeId,(int *)attachment->childModelRuntimeOrSavedOffset);
    attachment++;
  }
}


/* Collects the attachment points of a runtime model from its serialized MDL definition node (nodes whose
   nodeFlags low nibble is not 0 return false and are not walked). Per child slot the first transform record of
   kind 0 or 1 naming that slot is searched in the definition's sprite asset; when the child definition is not
   walked (false from the recursion), the record goes into the next of the six attachments[] entries.
   Returns true when the node was walked.
*/
Bool8 ModelRuntimeHierarchy_CollectAttachmentDescriptorsRecursive
          (ModelRuntimeSlot *modelRuntime,MdlSerializedNodeHeader *definitionNode)

{
  uint32_t attachmentSlot;
  uint32_t attachmentKind;
  int transformRecordsRemaining;
  MdlChildCount childCountRemaining;
  uint32_t childIndex;
  ModelAttachmentTransformRecord *attachmentTransformCursor;
  Bool8 childWalked;
  ModelResource *definitionResource;

  if ((definitionNode->nodeFlags & 0xf) != 0) {
    return false;
  }
  childCountRemaining = definitionNode->childCount;
  definitionResource = definitionNode->spriteAssetReference.modelResource;
  childIndex = 0;
  for (; childCountRemaining != 0; childCountRemaining--) {
    attachmentTransformCursor =
         (ModelAttachmentTransformRecord *)
         ((uint8_t *)definitionResource + definitionResource->packedLookupTableRelativeOffset);
    for (transformRecordsRemaining = definitionResource->packedLookupTableEntryCount;
        transformRecordsRemaining != 0;
        transformRecordsRemaining--) {
      /* packedKindAndSelector: kind in bits 0..3, child slot index above */
      attachmentKind = attachmentTransformCursor->packedKindAndSelector & 0xf;
      if (((attachmentKind == 0) || (attachmentKind == 1)) &&
         (childIndex == attachmentTransformCursor->packedKindAndSelector >> 4)) {
        /* 5f-format: MdlSerializedNodeHeader.childSerializedOffsets (relocated to 32-bit addresses) */
        childWalked =
             ModelRuntimeHierarchy_CollectAttachmentDescriptorsRecursive
                       (modelRuntime,
                        Thandor_U32ToPointer<MdlSerializedNodeHeader>(
                        definitionNode->childSerializedOffsets[childIndex]));
        if (!childWalked) {
          attachmentSlot = modelRuntime->attachmentCount;
          if (attachmentSlot < 6) {
            modelRuntime->attachmentCount++;
            modelRuntime->attachments[attachmentSlot].sourceTransform = attachmentTransformCursor;
          }
        }
        break;
      }
      attachmentTransformCursor++;
    }
    /* The original advances the child index only when an attachment transform matched (it undoes the shared
       increment when none did). */
    if (transformRecordsRemaining != 0) {
      childIndex++;
    }
  }
  return true;
}


/* The first attachment transform record of kind 0 or 1 in the resource's packed lookup table whose selector
   (bits 4..31) is the given child slot, or NULL when there is none. */
static ModelAttachmentTransformRecord *ModelResource_FindChildAttachmentTransform
          (ModelResource *resourceView,uint32_t childIndex)

{
  ModelAttachmentTransformRecord *attachmentTransform;
  ModelPackedLookupTableEntryCount transformRecordsRemaining;
  uint32_t attachmentKind;

  attachmentTransform = (ModelAttachmentTransformRecord *)
            ((uint8_t *)resourceView + resourceView->packedLookupTableRelativeOffset);
  for (transformRecordsRemaining = resourceView->packedLookupTableEntryCount; transformRecordsRemaining != 0;
      transformRecordsRemaining--) {
    attachmentKind = attachmentTransform->packedKindAndSelector & 0xf;
    if ((attachmentKind == 0 || attachmentKind == 1) &&
        childIndex == attachmentTransform->packedKindAndSelector >> 4) {
      return attachmentTransform;
    }
    attachmentTransform++;
  }
  return nullptr;
}


/* Builds the runtime node tree of a model from its serialized MDL node tree: allocates a world node per
   definition node, copies the local rotation and the mesh resource, and places each child at the translation
   of the attachment transform record (kind 0 or 1) that names its slot. Definition nodes whose low nibble of
   nodeFlags is set are not instantiated (NULL); for such a child an attachment point is recorded in the model
   runtime so that another model can be attached there later. Returns true with the new node (NULL for a
   non-instantiated definition node) in *outNode, or false when a world node could not be allocated (the
   original returned FATAL_ERROR_GENERAL_FAILURE as its failure value; *outNode is then left unchanged).
*/
Bool8 ModelNodeRuntime_CreateHierarchyRecursive
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,
          ModelRuntimeSlot *modelRuntime,MdlSerializedNodeHeader *definitionNode,
          WorldRuntimeContext *worldRuntime,ModelRuntimeNode **outNode)

{
  AngleTurn32 rotationAngleA;
  AngleTurn32 rotationAngleB;
  ArmyRuntimeSlot *ownerArmy;
  ModelDefinition *modelDefinition;
  ModelResource *resourceView;
  Q12 boundingRadiusQ12;
  Q12 translationYQ12;
  Q12 translationZQ12;
  MdlSerializedNodeHeader *childDefinition;
  ModelRuntimeNode *newNode;
  uint32_t attachmentSlot;
  uint32_t childrenRemaining;
  uint32_t childIndex;
  ModelAttachmentTransformRecord *attachmentTransform;
  ModelRuntimeNode *childNode;

  if ((definitionNode->nodeFlags & 0xf) != 0) {
    *outNode = nullptr;
    return true;
  }
  newNode = (ModelRuntimeNode *)WorldObjectArray_AllocateFreeRecord(worldRuntime);
  if (newNode == nullptr) {
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
  modelDefinition = modelRuntime->definitionOrSavedId.runtimeDefinition;
  newNode->tintArgb = 0xffffffff;
  /* ModelDefinition.modelFlags 0x10, 0x20 and not 0x40 become node flags 0x10, 0x200 and 0x100 */
  if ((modelDefinition->modelFlags & MODEL_DEFINITION_FLAG_NOT_REMEMBERED) != 0) {
    newNode->runtimeFlags = newNode->runtimeFlags | TERRAIN_OCCUPANCY_FLAG_NOT_REMEMBERED;
  }
  if ((modelDefinition->modelFlags & MODEL_DEFINITION_FLAG_DRAW_BEFORE_TERRAIN) != 0) {
    newNode->runtimeFlags = newNode->runtimeFlags | MODEL_NODE_FLAG_DRAW_BEFORE_TERRAIN;
  }
  if ((modelDefinition->modelFlags & MODEL_DEFINITION_FLAG_NO_SHADING_PASS) == 0) {
    newNode->runtimeFlags = newNode->runtimeFlags | MODEL_NODE_FLAG_SHADING_PASS;
  }
  resourceView = definitionNode->spriteAssetReference.modelResource;
  newNode->modelPayload.paletteAsset = paletteAsset;
  boundingRadiusQ12 = resourceView->boundingRadiusQ12;
  newNode->modelPayload.textureSet = textureSet;
  newNode->subtreeBoundingRadiusQ12 = boundingRadiusQ12;
  newNode->modelPayload.modelResource = resourceView;
  newNode->shadingRecord = nullptr;
  newNode->modelRuntimeLinkOrSavedOffset = nullptr;
  newNode->renderDepthBiasOrState = 0;
  childrenRemaining = definitionNode->childCount;
  if (childrenRemaining > sizeof(definitionNode->childSerializedOffsets) /
                          sizeof(definitionNode->childSerializedOffsets[0])) {
    /* The original takes childCount unchecked and writes childNodes[] past the 13 slots of the world record
       (and reads the child offsets past the serialized header's six); bounded here because a node with a sprite
       and more than six children cannot be valid: registration (ModelDefinition_ResolveNodeSprites) would have
       relocated its sprite reference as a seventh child offset. Extra children are dropped. */
    static Bool8 s_loggedChildCountOutOfRange = false;
    if (!s_loggedChildCountOutOfRange) {
      s_loggedChildCountOutOfRange = true;
      Thandor_Log("model: MDL node with %u children, only the first 6 are created",childrenRemaining);
    }
    childrenRemaining = sizeof(definitionNode->childSerializedOffsets) /
                        sizeof(definitionNode->childSerializedOffsets[0]);
  }
  childIndex = 0;
  newNode->childCount = childrenRemaining;
  newNode->parentNode = nullptr;
  for (; childrenRemaining != 0; childrenRemaining--) {
    attachmentTransform = ModelResource_FindChildAttachmentTransform(resourceView,childIndex);
    if (attachmentTransform == nullptr) {
      /* no attachment transform for this child */
      newNode->childNodes[childIndex] = nullptr;
    }
    else {
      /* 5f-format: MdlSerializedNodeHeader.childSerializedOffsets (relocated to 32-bit addresses) */
      if (!ModelNodeRuntime_CreateHierarchyRecursive
              (paletteAsset,textureSet,modelRuntime,
               Thandor_U32ToPointer<MdlSerializedNodeHeader>(definitionNode->childSerializedOffsets[childIndex]),worldRuntime,
               &childNode)) {
        return false;
      }
      newNode->childNodes[childIndex] = childNode;
      if (childNode == nullptr) {
        /* an attachment point: record where the child model will hang */
        attachmentSlot = modelRuntime->attachmentCount;
        if (attachmentSlot < MODEL_RUNTIME_ATTACHMENT_CAPACITY) {
          modelRuntime->attachmentCount++;
          modelRuntime->attachments[attachmentSlot].sourceTransform = attachmentTransform;
          modelRuntime->attachments[attachmentSlot].childNodeIndex = childIndex;
          modelRuntime->attachments[attachmentSlot].parentModelNodeOrSavedOffset = newNode;
          /* 5f-format: MdlSerializedNodeHeader.childSerializedOffsets */
          childDefinition = Thandor_U32ToPointer<MdlSerializedNodeHeader>(definitionNode->childSerializedOffsets[childIndex]);
          modelRuntime->attachments[attachmentSlot].childModelRuntimeOrSavedOffset = nullptr;
          rotationAngleA = childDefinition->localRotationAngle0;
          rotationAngleB = childDefinition->localRotationAngle1;
          modelRuntime->attachments[attachmentSlot].childLocalRotationAngle2 = childDefinition->localRotationAngle2;
          modelRuntime->attachments[attachmentSlot].childLocalRotationAngle1 = rotationAngleB;
          modelRuntime->attachments[attachmentSlot].childLocalRotationAngle0 = rotationAngleA;
        }
      }
      else {
        childNode->parentNode = newNode;
        translationYQ12 = attachmentTransform->localTranslationYQ12;
        translationZQ12 = attachmentTransform->localTranslationZQ12;
        childNode->modelPayload.localTranslationXQ12 = attachmentTransform->localTranslationXQ12;
        childNode->modelPayload.localTranslationYQ12 = translationYQ12;
        childNode->modelPayload.localTranslationZQ12 = translationZQ12;
      }
    }
    childIndex++;
  }
  *outNode = newNode;
  return true;
}


/* Frees a model node and its whole subtree: releases the children first, clears the parent's childNodes[]
   entries that point at this node and finally unlinks the node from its world owner list.
*/
void ModelRuntimeNode_ReleaseRecursiveAndDetachParent(ModelRuntimeNode *node)

{
  uint32_t childrenRemaining;
  uint32_t parentSlotsRemaining;
  ModelRuntimeNode *childSlotCursor;
  
  childSlotCursor = node;
  for (childrenRemaining = node->childCount; childrenRemaining != 0; childrenRemaining--) {
    if (childSlotCursor->childNodes[0] != nullptr) {
      ModelRuntimeNode_ReleaseRecursiveAndDetachParent(childSlotCursor->childNodes[0]);
    }
    /* steps the cursor by one dword, i.e. to the next childNodes[] entry */
    childSlotCursor = (ModelRuntimeNode *)((uint32_t *)childSlotCursor + 1);
  }
  childSlotCursor = node->parentNode;
  if (childSlotCursor != nullptr) {
    for (parentSlotsRemaining = childSlotCursor->childCount; parentSlotsRemaining != 0; parentSlotsRemaining--) {
      if (childSlotCursor->childNodes[0] == node) {
        childSlotCursor->childNodes[0] = nullptr;
      }
      childSlotCursor = (ModelRuntimeNode *)((uint32_t *)childSlotCursor + 1);
    }
  }
  WorldRuntime_UnlinkOwnerListNode((WorldOwnerListNode *)node);
}


/* Sets the packed ARGB tint of a model node and of all its descendants (the state tint of a whole model,
   see ModelNodeRuntime_UpdateStateTintRecursive).
*/
void ModelNodeRuntime_ApplyTintRecursive(PackedArgb32 tintArgb,ModelRuntimeNode *modelNode)

{
  uint32_t childrenRemaining;
  
  childrenRemaining = modelNode->childCount;
  modelNode->tintArgb = tintArgb;
  for (; childrenRemaining != 0; childrenRemaining--) {
    if (modelNode->childNodes[0] != nullptr) {
      ModelNodeRuntime_ApplyTintRecursive(tintArgb,modelNode->childNodes[0]);
    }
    /* steps the cursor by one dword, i.e. to the next childNodes[] entry */
    modelNode = (ModelRuntimeNode *)((uint32_t *)modelNode + 1);
  }
}


/* Computes the world transforms of a model hierarchy: a root node first gets its rotation basis from its world
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
  if (modelNodeRuntime->parentNode == nullptr) {
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
      if (currentChild != nullptr) {
        /* the scratch matrix (rotation basis plus translation) forms the child's local transform */
        FixedTransform_BuildRotationBasis
                  ((GraphicsFixedMatrix3x4 *)&g_ModelTransformScratchMatrix,
                   currentChild->modelPayload.localRotationAngle2,
                   currentChild->modelPayload.localRotationAngle1,
                   currentChild->modelPayload.localRotationAngle0);
        g_ModelTransformScratchMatrix.translation.x = currentChild->modelPayload.localTranslationXQ12;
        g_ModelTransformScratchMatrix.translation.y = currentChild->modelPayload.localTranslationYQ12;
        g_ModelTransformScratchMatrix.translation.z = currentChild->modelPayload.localTranslationZQ12;
        FixedTransform_Compose
                  (&currentChild->worldTransform,
                   (GraphicsFixedMatrix3x4 *)&g_ModelTransformScratchMatrix,
                   &modelNodeRuntime->worldTransform);
        childEulerAngles = FixedTransform_ExtractEulerAngles(&currentChild->worldTransform);
        currentChild->modelPayload.worldRotationAngle2 = childEulerAngles.rollAngle;
        currentChild->modelPayload.worldRotationAngle0 = childEulerAngles.azimuthAngle;
        currentChild->modelPayload.worldRotationAngle1 = childEulerAngles.elevationAngle;
        g_ModelTransformScratchMatrix.translation.x = 0;
        g_ModelTransformScratchMatrix.translation.y = 0;
        g_ModelTransformScratchMatrix.translation.z = 0;
        inheritedTintArgb = modelNodeRuntime->tintArgb;
        currentChild->runtimeFlags = currentChild->runtimeFlags | 1;
        currentChild->tintArgb = inheritedTintArgb;
        ModelNodeRuntime_ComposeChildTransformsRecursive(currentChild);
      }
    } while (childIndex < modelNodeRuntime->childCount);
  }
}


