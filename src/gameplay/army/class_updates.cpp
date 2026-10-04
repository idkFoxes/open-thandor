/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/class_updates.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/army/class_updates.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Implementation ownership: gameplay/army/class_updates. */

/* Runtime update of the resource extractor class (14), reached only through
   g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[14]. While it has health left and is active,
   it stamps its faction and resource-field selector into the grid cell under it (inner cells only) and, when
   the cell carries the matching resource-field support bit, registers itself there and runs its emitters and
   animation.
*/
void ArmyRuntimeClass_UpdateGridBoundEffectsAndModels
          (WorldRuntimeContext *worldRuntime,ModelRuntimeClass14UpdateView *modelRuntime)

{
  int cellColumn;
  int cellRow;
  int cellIndex;
  FieldCellPackedFlagsAndMaterial supportFlagMask;
  FieldGridCoordinates gridCoordinates;
  FieldGridAsset *fieldGrid;

  if ((1 < (int)modelRuntime->health) &&
     (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_INACTIVE_MASK) == 0)) {
    gridCoordinates = FieldGrid_WorldToGridQ12
                      ((modelRuntime->rootModelNode->worldTransform).translation.y,
                       (modelRuntime->rootModelNode->worldTransform).translation.x);
    /* grid coordinates rounded to the nearest cell */
    cellColumn = ((gridCoordinates.columnQ12 >> (Q12_SHIFT - 1)) + 1) >> 1;
    cellRow = ((gridCoordinates.rowQ12 >> (Q12_SHIFT - 1)) + 1) >> 1;
    fieldGrid = worldRuntime->fieldGrid;
    if ((0 < cellColumn) && (0 < cellRow)) {
      if ((cellColumn + 1 < (int)fieldGrid->gridWidth) && (cellRow + 1 < (int)fieldGrid->gridHeight)) {
        cellIndex = cellRow * fieldGrid->gridWidth + cellColumn;
        supportFlagMask = FIELD_CELL_XENITE_SUPPORT << ((uint8_t)modelRuntime->modelDefinition->resourceFieldSupportSelector & 31
                         );
        /* claim the cell: faction << 13, the support bit, claimedCellTag << 24 */
        fieldGrid->cells[cellIndex].resourceExtractionDescriptor =
             modelRuntime->ownerArmyRuntime->factionIndex << 13 | supportFlagMask |
             modelRuntime->modelDefinition->claimedCellTag << 24;
        if ((fieldGrid->cells[cellIndex].flagsAndMaterial & supportFlagMask) != 0) {
          /* the cell supports this extractor: register it (as a saved offset) and run its emitters */
          fieldGrid->cells[cellIndex].armyRuntimeSavedOffset =
               Thandor_PointerToI32(modelRuntime) - g_ModelRuntimeRebaseDelta; /* 32-bit format field: FieldGridCell.armyRuntimeSavedOffset */
          ArmyRuntime_UpdateTimedShotAndEffectEmitters
                    (worldRuntime,(ModelRuntimeUpdateView *)modelRuntime);
          ArmyRuntime_UpdateAnimatedModelSubnodes
                    (worldRuntime,(ModelRuntimeUpdateView *)modelRuntime);
        }
      }
    }
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ModelRuntimeSlot *)modelRuntime);
  return;
}

/* Class command of the structure classes (the ten non-default class slots that share it; session conditions
   use it to tell structures from units). It hits every class-0/class-12 model standing inside the structure's
   footprint with 0x100000 impact damage, stamps the structure's ground height into the field grid (unless
   class-state bit 0x20 is set), and when every model of the same owner within reach is an idle class-18
   model, marks the last of them (flag 8) and spawns its army-from-model completion effect.
*/
void ArmyRuntime_ClassCommandHandlerGroupA(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  ModelRuntimeNode *ownNode;
  ModelRuntimeNode *armyModelNode;
  ModelRuntimeNode *completionNode;
  ModelRuntimeSlot *candidateModelRuntime;
  ModelRuntimeSlot *completionModelRuntime;
  int64_t deltaYSquared;
  int64_t remainingRadiusSquared;
  uint32_t impactAngle;
  uint32_t candidateSupportRadius;
  int proximityRadius;
  int deltaX;
  int deltaY;
  ModelRuntimeNode *scanNode;
  EffectDefinition *completionEffect;
  int blockingCount;

  /* crush every class-0/class-12 model standing inside the structure */
  ownNode = modelRuntime->rootModelNodeOrSavedOffset.modelNode;
  if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE) == 0) {
    for (scanNode = (ModelRuntimeNode *)worldRuntime->ownerListHead; scanNode != nullptr;
        scanNode = (ModelRuntimeNode *)(scanNode->common).nextNode) {
      if (scanNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
        continue;
      }
      candidateModelRuntime = (scanNode->runtimePayload).modelRuntime;
      if ((ownNode != scanNode) &&
         ((candidateModelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_00 ||
          (candidateModelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_12))))
      {
        /* Damage the candidate unless the class-13 test misses and the attachment proximity test hits. */
        if (ArmyRuntime_TestArmyNearFactoryExit(candidateModelRuntime,modelRuntime) ||
            !ArmyRuntime_TestModelAttachmentProximity(candidateModelRuntime,modelRuntime)) {
          impactAngle = FixedMath_Atan2Angle16
                            ((scanNode->worldTransform).translation.y -
                             (ownNode->worldTransform).translation.y,
                             (scanNode->worldTransform).translation.x -
                             (ownNode->worldTransform).translation.x);
          ArmyRuntime_ApplyImpactDamageAndFinalizeState(impactAngle,ARMY_CRUSH_IMPACT_DAMAGE,candidateModelRuntime);
        }
      }
    }
  }
  /* stamp the structure's ground height (root node read again after the damage calls, as in the original) */
  ownNode = modelRuntime->rootModelNodeOrSavedOffset.modelNode;
  if ((modelRuntime->definitionOrSavedId.runtimeDefinition->modelFlags & MODEL_DEFINITION_FLAG_DRAW_BEFORE_TERRAIN) == 0) {
    FieldGrid_ApplyHeightAtWorldPointAndRefreshNeighbors
              (modelRuntime->definitionOrSavedId.runtimeDefinition->footprintRadius,
               (ownNode->worldTransform).translation.z,(ownNode->worldTransform).translation.y
               ,(ownNode->worldTransform).translation.x,worldRuntime->fieldGrid);
  }
  /* count the models of the same faction within reach of the owning army's model node; idle class-18 models
     do not block, the last of them is the one to complete */
  armyModelNode = modelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime->modelNodeRuntime;
  completionModelRuntime = nullptr;
  blockingCount = 0;
  for (scanNode = (ModelRuntimeNode *)worldRuntime->ownerListHead; scanNode != nullptr;
      scanNode = (ModelRuntimeNode *)(scanNode->common).nextNode) {
    if ((scanNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) || (scanNode == armyModelNode)) {
      continue;
    }
    candidateModelRuntime = (scanNode->runtimePayload).modelRuntime;
    candidateSupportRadius = candidateModelRuntime->definitionOrSavedId.runtimeDefinition->supportRadius;
    if ((modelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex !=
         candidateModelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex) ||
        (candidateSupportRadius == 0)) {
      continue;
    }
    /* reach = own definition placementFlags + candidate supportRadius; inside when reach^2 - dx^2 - dy^2 >= 0 in
       64 bits */
    proximityRadius =
         modelRuntime->definitionOrSavedId.runtimeDefinition->placementFlags + candidateSupportRadius;
    deltaX = (armyModelNode->worldTransform).translation.x - (scanNode->worldTransform).translation.x;
    remainingRadiusSquared =
         (int64_t)proximityRadius * (int64_t)proximityRadius - (int64_t)deltaX * (int64_t)deltaX;
    if (remainingRadiusSquared < 0) {
      continue;
    }
    deltaY = (armyModelNode->worldTransform).translation.y - (scanNode->worldTransform).translation.y;
    deltaYSquared = (int64_t)deltaY * (int64_t)deltaY;
    if (remainingRadiusSquared - deltaYSquared < 0) {
      continue;
    }
    if ((candidateModelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_18) &&
       (((candidateModelRuntime->classState).stateFlags &
         (ARMY_RUNTIME_FLAG_DESTROYED | ARMY_MODEL_STATE_DISMANTLING)) == 0)) {
      completionModelRuntime = candidateModelRuntime;
    }
    else {
      blockingCount = blockingCount + 1;
    }
  }
  if ((blockingCount == 0) && (completionModelRuntime != nullptr)) {
    completionNode = completionModelRuntime->rootModelNodeOrSavedOffset.modelNode;
    (completionModelRuntime->classState).stateFlags =
         (completionModelRuntime->classState).stateFlags | ARMY_RUNTIME_FLAG_DESTROYED;
    if (EffectDefinitionRegistry_FindById
                      ((PckEffectDefinitionIdCatalog)
                       completionModelRuntime->definitionOrSavedId.runtimeDefinition->classParameterC4,
                       &completionEffect) == 0) {
      EffectRuntimePool_CreateInstanceFromDefinition
                (EFFECT_RUNTIME_COMPLETION_SPAWN_ARMY_FROM_MODEL,
                 THANDOR_COMPOUND(EffectRuntimeOwnerReference){
                   .armyRuntime = completionModelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime },
                 (completionNode->modelPayload).worldRotationAngle2,
                 (completionNode->modelPayload).worldRotationAngle1,
                 (completionNode->modelPayload).worldRotationAngle0,
                 (completionNode->worldTransform).translation.z,
                 (completionNode->worldTransform).translation.y,
                 (completionNode->worldTransform).translation.x,completionEffect,worldRuntime);
    }
  }
}

/* Runtime update of class 12, reached only through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[12]:
   runs the emitters and moves the model vertically by the definition's step per tick, subtracting the step
   from the distance counter in classLinkState.modelLinkOrState; once that counter exceeds the model's own
   height (bounds Z1 - Z0) the whole model hierarchy is destroyed, e.g. a wreck that has sunk out of sight.
*/

void ArmyRuntimeClass_UpdateEffectsAndDestroyModelHierarchy
          (WorldRuntimeContext *worldRuntime,ModelRuntimeDestroyEffectsView *modelRuntime)

{
  ModelRuntimeNode *modelNodeRuntime;
  ModelResource *rootModelResource;
  int verticalStepQ12;
  int modelHeightQ12;
  int remainingClassDistanceQ12;

  ArmyRuntime_UpdateTimedShotAndEffectEmitters
            (worldRuntime,(ModelRuntimeUpdateView *)modelRuntime);
  modelNodeRuntime = modelRuntime->rootModelNode;
  rootModelResource = (modelNodeRuntime->modelPayload).modelResource;
  verticalStepQ12 =
       modelRuntime->modelDefinition->verticalTranslationStepQ12PerTick *
       g_InGameSimulationStepTicks;
  (modelNodeRuntime->worldTransform).translation.z += verticalStepQ12;
  /* the class link field (modelLinkOrState) holds the distance counter here */
  (modelRuntime->classLinkState).modelLinkOrState.signedScalarState -= verticalStepQ12;
  modelHeightQ12 = rootModelResource->localBoundsZ1Q12 - rootModelResource->localBoundsZ0Q12;
  remainingClassDistanceQ12 = (modelRuntime->classLinkState).modelLinkOrState.signedScalarState;
  modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
  if (modelHeightQ12 < remainingClassDistanceQ12) {
    ModelRuntimePool_DestroyHierarchyAndDetach(worldRuntime,(ModelRuntimeSlot *)modelRuntime);
  }
  else {
    ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
  }
  return;
}

/* Segment meter of the selection panel (called directly by gameplay/selection/runtime with a model runtime):
   filled segments from linkedChildBuildState.completedSecondaryArmyAssetCount of the passed runtime (the
   completed linked assets of a class-22 pad), total segments from its definition's linkedChildSlotCapacity.
*/
ArmySegmentMeter ArmyRuntime_GetLinkedChildSlotMeter(ModelRuntimeLinkedChildSpawnAndBuildView *linkedChildRuntime)

{
  ArmySegmentMeter slotMeter;

  slotMeter.totalSegments = linkedChildRuntime->modelDefinition->linkedChildSlotCapacity;
  slotMeter.filledSegments = (linkedChildRuntime->linkedChildBuildState).completedSecondaryArmyAssetCount;
  return slotMeter;
}

/* Returns which of the three linked-child asset ids (g_InGamePointerModePreviewArmyIds[1]/[2]/[4] as bits 1/2/4)
   occur among the army's 13 attachment asset-id slots (completedSecondaryArmyAssetIds); the selection panel ORs these
   masks over all selected armies.
*/
int ArmyRuntime_GetAttachmentEffectVariantMask(ModelRuntimeLinkedChildSpawnAndBuildView *linkedChildRuntime)

{
  int attachmentAssetId;
  int attachmentEffectSlotsRemaining;
  uint32_t variantMask;

  variantMask = 0;
  attachmentEffectSlotsRemaining = 13;
  do {
    /* completedSecondaryArmyAssetIds[0] of the current window; the runtime pointer itself moves one dword per
       slot below */
    attachmentAssetId = linkedChildRuntime->completedSecondaryArmyAssetIds[0];
    if (attachmentAssetId == g_InGamePointerModePreviewArmyIds[1]) {
      variantMask = variantMask | 1;
    }
    if (attachmentAssetId == g_InGamePointerModePreviewArmyIds[2]) {
      variantMask = variantMask | 2;
    }
    if (attachmentAssetId == g_InGamePointerModePreviewArmyIds[4]) {
      variantMask = variantMask | 4;
    }
    linkedChildRuntime = (ModelRuntimeLinkedChildSpawnAndBuildView *)((uint8_t *)linkedChildRuntime + 4);
    attachmentEffectSlotsRemaining--;
  } while (attachmentEffectSlotsRemaining != 0);
  return variantMask;
}

/* The deployment one-shot sound of ArmyRuntimeClass_UpdateVerticalDeploymentAndCollisionState at the root
   node's position, only where the active faction's cell bits 0/1 are set. */
static void ArmyRuntimeClass_PlayVerticalDeploymentSound(WorldRuntimeContext *worldRuntime,
          ModelDefinitionVerticalDeploymentView *deploymentDefinition,ModelRuntimeNode *rootNode)
{
  uint32_t soundAssetIndex;
  DirectSoundVoiceSet **soundVoiceSet;
  Bool8 cellMasked;

  soundAssetIndex = deploymentDefinition->deploymentSoundAssetIndex;
  if ((soundAssetIndex == 0) || (soundAssetIndex >= worldRuntime->dwordArrayCount) ||
      (worldRuntime->dwordArray == nullptr)) {
    return;
  }
  soundVoiceSet = (DirectSoundVoiceSet **)worldRuntime->dwordArray[soundAssetIndex];
  if (soundVoiceSet == nullptr) {
    return;
  }
  cellMasked = TerrainGrid_TestProjectedCellMaskBits01
                    ((rootNode->worldTransform).translation.y,(rootNode->worldTransform).translation.x,worldRuntime);
  if (!cellMasked) {
    SpatialSound_PlayPositionedOneShot
              (deploymentDefinition->positionedSoundMaximumDistanceQ12,deploymentDefinition->positionedSoundGainQ15,
               &(rootNode->worldTransform).translation,soundVoiceSet);
  }
}

/* Runtime update of class 23 (a platform that armies of its faction can dock on, see
   ArmyRuntime_HandleCollisionPartner), reached only through
   g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[23]. While an army is linked (bit 0 of
   classState.behaviorState, re-checked against the army's expanded radius every 8 ticks) the child platform node
   moves by the step per tick until the travel (deploymentState.deploymentTravelQ12) reaches the definition's
   limit; without a linked army it moves back until the travel is 0. A positioned one-shot sound marks the start
   of either movement.
*/

void ArmyRuntimeClass_UpdateVerticalDeploymentAndCollisionState
          (WorldRuntimeContext *worldRuntime,ModelRuntimeVerticalDeploymentView *modelRuntime)

{
  ModelDefinitionVerticalDeploymentView *deploymentDefinition;
  ModelRuntimeSlot *linkedModelRuntime;
  int travelLimit;
  int travelStep;
  Bool8 linkedStillInRange;
  ModelRuntimeNode *rootNode;
  ModelRuntimeNode *platformNode;

  deploymentDefinition = modelRuntime->modelDefinition;
  rootNode = modelRuntime->rootModelNode;
  linkedModelRuntime = (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.modelRuntime;
  travelLimit = deploymentDefinition->deploymentTravelLimitQ12;
  if (((modelRuntime->classState).behaviorState & 1) == 0) {
    /* no army linked (bit 0 clear): move back until the travel is 0 */
    if ((modelRuntime->deploymentState).deploymentTravelQ12 == 0) {
      return;
    }
    if ((modelRuntime->deploymentState).deploymentTravelQ12 <= travelLimit) {
      ArmyRuntimeClass_PlayVerticalDeploymentSound(worldRuntime,deploymentDefinition,rootNode);
    }
    platformNode = rootNode->childNodes[0];
    travelStep = deploymentDefinition->verticalDeploymentStepQ12PerTick;
    (modelRuntime->classState).behaviorState &= ~2u;
    travelStep = travelStep * g_InGameSimulationStepTicks;
    if (platformNode != nullptr) {
      (platformNode->modelPayload).localTranslationZQ12 += travelStep;
      (modelRuntime->deploymentState).deploymentTravelQ12 += travelStep;
      ModelNodeRuntime_RebuildTransformsFromRoot(modelRuntime->rootModelNode);
    }
    return;
  }
  /* docking requested or army linked (bit 0): when the retry countdown has run out, bit 0 is dropped and only
     restored while the linked army is still within range */
  if ((modelRuntime->deploymentState).collisionRetryCountdown != 0) {
    (modelRuntime->deploymentState).collisionRetryCountdown -= 1;
  }
  (modelRuntime->classState).behaviorState |= 2;
  if ((modelRuntime->deploymentState).collisionRetryCountdown == 0) {
    (modelRuntime->classState).behaviorState &= ~1u;
    if (linkedModelRuntime != nullptr) {
      (modelRuntime->deploymentState).collisionRetryCountdown = 8;
      (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.modelRuntime = nullptr;
      linkedStillInRange = ArmyCollision_TestPointWithinExpandedRuntimeRadius
                             (deploymentDefinition->footprintRadius,
                              (rootNode->worldTransform).translation.y,
                              (rootNode->worldTransform).translation.x,linkedModelRuntime);
      if (linkedStillInRange) {
        (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.modelRuntime = linkedModelRuntime;
        (modelRuntime->classState).behaviorState |= 1;
      }
    }
  }
  /* move towards the travel limit */
  if (travelLimit < (modelRuntime->deploymentState).deploymentTravelQ12) {
    (modelRuntime->classState).behaviorState &= ~2u;
    if ((modelRuntime->deploymentState).deploymentTravelQ12 == 0) {
      ArmyRuntimeClass_PlayVerticalDeploymentSound(worldRuntime,deploymentDefinition,rootNode);
    }
    if (rootNode->childNodes[0] != nullptr) {
      travelStep = deploymentDefinition->verticalDeploymentStepQ12PerTick * g_InGameSimulationStepTicks;
      (rootNode->childNodes[0]->modelPayload).localTranslationZQ12 -= travelStep;
      (modelRuntime->deploymentState).deploymentTravelQ12 -= travelStep;
      ModelNodeRuntime_RebuildTransformsFromRoot(modelRuntime->rootModelNode);
    }
  }
}

/* Returns true when the candidate model is within the combined radius (definition footprintRadiusCopy) of the
   source model and of every attached child model of the source (attachments, attachmentCount); false as soon as
   one of them is out of reach.
*/
Bool8 ArmyRuntime_TestModelAttachmentProximity(ModelRuntimeSlot *candidateModelRuntime,ModelRuntimeSlot *sourceModelRuntime)

{
  ModelDefinition *candidateDefinition;
  ModelRuntimeSlot *childModelRuntime;
  int remainingAttachments;
  Bool8 baseWithinRadius;
  Bool8 childWithinRadius;

  candidateDefinition = candidateModelRuntime->definitionOrSavedId.runtimeDefinition;
  remainingAttachments = sourceModelRuntime->attachmentCount;
  /* The candidate radius is candidateDefinition->footprintRadiusCopy (the original read it through a
     pointer-typed field at the same offset, attachments[3].childModelRuntimeOrSavedOffset of a ModelRuntimeSlot
     view). */
  baseWithinRadius = ArmyRuntime_TestPositionDistanceWithinCombinedRadius
                    (candidateDefinition->footprintRadiusCopy,
                     sourceModelRuntime->definitionOrSavedId.runtimeDefinition->footprintRadiusCopy,
                     candidateModelRuntime->rootModelNodeOrSavedOffset.modelNode,
                     sourceModelRuntime->rootModelNodeOrSavedOffset.modelNode);
  if (!baseWithinRadius) {
    return false;
  }
  for (; remainingAttachments != 0; remainingAttachments--) {
    childModelRuntime = sourceModelRuntime->attachments[0].childModelRuntimeOrSavedOffset;
    if (childModelRuntime != nullptr) {
      childWithinRadius = ArmyRuntime_TestPositionDistanceWithinCombinedRadius
                            (candidateDefinition->footprintRadiusCopy,
                             childModelRuntime->definitionOrSavedId.runtimeDefinition->footprintRadiusCopy,
                             candidateModelRuntime->rootModelNodeOrSavedOffset.modelNode,
                             childModelRuntime->rootModelNodeOrSavedOffset.modelNode);
      if (!childWithinRadius) {
        return false;
      }
    }
    /* the original advances the source pointer itself by one attachment descriptor (0x20), so
       attachments[0] walks the attachments */
    sourceModelRuntime =
         (ModelRuntimeSlot *)((uint8_t *)sourceModelRuntime + sizeof(ModelRuntimeAttachmentDescriptor));
  }
  return true;
}

/* Tests whether two model nodes are closer in the XY plane than the sum of their radii (collision/contact test
   of two armies or attachments): returns false when dx^2 + dy^2 <= (candidateRadius + sourceRadius)^2, true
   otherwise, in 64-bit Q24 arithmetic.
*/
Bool8 ArmyRuntime_TestPositionDistanceWithinCombinedRadius
          (UQ12 candidateRadiusQ12,UQ12 sourceRadiusQ12,void *candidateModelNode,
          void *sourceModelNode)

{
  int axisDeltaXQ12;
  int axisDeltaYQ12;
  int64_t remainingRadiusSquaredAfterXQ24;
  int64_t yDistanceSquaredQ24;

  axisDeltaXQ12 =
       ((ModelRuntimeNode *)sourceModelNode)->worldTransform.translation.x -
       ((ModelRuntimeNode *)candidateModelNode)->worldTransform.translation.x;
  remainingRadiusSquaredAfterXQ24 =
       (int64_t)(int)(sourceRadiusQ12 + candidateRadiusQ12) *
       (int64_t)(int)(sourceRadiusQ12 + candidateRadiusQ12) -
       (int64_t)axisDeltaXQ12 * (int64_t)axisDeltaXQ12;
  if (remainingRadiusSquaredAfterXQ24 < 0) {
    return true;
  }
  axisDeltaYQ12 = ((ModelRuntimeNode *)sourceModelNode)->worldTransform.translation.y -
                  ((ModelRuntimeNode *)candidateModelNode)->worldTransform.translation.y;
  yDistanceSquaredQ24 = (int64_t)axisDeltaYQ12 * (int64_t)axisDeltaYQ12;
  /* the original tests the sign of the high dword of remainder - dy^2 (high dwords minus the borrow); both
     values are below 2^62 in magnitude, so this is a plain 64-bit comparison */
  if (remainingRadiusSquaredAfterXQ24 >= yDistanceSquaredQ24) {
    return false;
  }
  return true;
}

/* Recomputes the army's derived combat figures shown on selection from its model hierarchy (after creation or
   a change of attachments): the maxima occupancyMarkRadius, visibilityRadius, visibilityHeightOffset and the
   shot selection range weaponRangeQ12 are cleared, as are the eight per-target-class damage sums
   targetClassShotDamage, then
   ModelRuntimeHierarchy_AccumulateDerivedSelectionMetrics adds up every node of the model.
*/
void ArmyRuntime_RebuildDerivedSelectionMetrics(ArmyRuntimeSlot *armyRuntime)

{
  int targetClassIndex;

  armyRuntime->occupancyMarkRadius = 0;
  armyRuntime->visibilityRadius = 0;
  armyRuntime->visibilityHeightOffset = 0;
  armyRuntime->weaponRangeQ12 = 0;
  for (targetClassIndex = 7; targetClassIndex >= 0; targetClassIndex--) {
    armyRuntime->targetClassShotDamage[targetClassIndex] = 0;
  }
  if ((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime != nullptr) {
    ModelRuntimeHierarchy_AccumulateDerivedSelectionMetrics
              ((int *)(armyRuntime->modelRuntimeOrSavedOffset).modelRuntime);
  }
}

/* Idle animation of a building model: child nodes 0 and 1 spin (localRotationAngle2 += definition
   animatedChild0RotationStep / animatedChild1RotationStep per tick), child node 2 bobs in Z by
   animatedChild2BobStep per tick between runtimeValue24 and runtimeValue28, reversing at the
   limits (stateFlags bit 2 = direction). While bit 0 is set the animation stands still; bit 4 follows bit 0,
   and each change of it rebuilds the owner's selection metrics. Reached through
   g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[10] and directly from the class updates 11,
   13, 14, 22 and gameplay/army/combat.
*/
void ArmyRuntime_UpdateAnimatedModelSubnodes(WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView *modelRuntime)

{
  uint32_t *classStateField;
  ModelRuntimeNode *modelNodeRuntime;
  ModelDefinition *animationDefinition;
  Q12 updatedChildTranslationZQ12;
  int bobStep;
  ModelRuntimeNode *animatedChildNode;
  int childTranslationZQ12;
  ModelRuntimeNode *animatedNode;
  
  if (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_SWITCHED_OFF) == 0) {
    if (((modelRuntime->classState).stateFlags & 4) != 0) {
      classStateField = &(modelRuntime->classState).stateFlags;
      *classStateField = *classStateField ^ 4;
      ArmyRuntime_RebuildDerivedSelectionMetrics(modelRuntime->ownerArmyRuntime);
    }
    modelNodeRuntime = modelRuntime->rootModelNode;
    animationDefinition = modelRuntime->modelDefinition;
    animatedChildNode = modelNodeRuntime->childNodes[0];
    if (modelNodeRuntime->childCount != 0) {
      if (animatedChildNode != nullptr) {
        (animatedChildNode->modelPayload).localRotationAngle2 =
             animationDefinition->animatedChild0RotationStep * g_InGameSimulationStepTicks +
             (animatedChildNode->modelPayload).localRotationAngle2 & FIXED_ANGLE16_MASK;
        animatedChildNode->runtimeFlags = animatedChildNode->runtimeFlags | 1;
      }
      animatedNode = modelNodeRuntime->childNodes[1];
      if (1 < modelNodeRuntime->childCount) {
        if (animatedNode != nullptr) {
          (animatedNode->modelPayload).localRotationAngle2 =
               animationDefinition->animatedChild1RotationStep * g_InGameSimulationStepTicks +
               (animatedNode->modelPayload).localRotationAngle2 & FIXED_ANGLE16_MASK;
          animatedNode->runtimeFlags = animatedNode->runtimeFlags | 1;
        }
        animatedNode = modelNodeRuntime->childNodes[2];
        if ((2 < modelNodeRuntime->childCount) && (animatedNode != nullptr)) {
          childTranslationZQ12 = (animatedNode->modelPayload).localTranslationZQ12;
          bobStep = animationDefinition->animatedChild2BobStep * g_InGameSimulationStepTicks;
          if (((modelRuntime->classState).stateFlags & 2) == 0) {
            updatedChildTranslationZQ12 = childTranslationZQ12 - bobStep;
            if (updatedChildTranslationZQ12 < (int)animationDefinition->runtimeValue24) {
              updatedChildTranslationZQ12 = animationDefinition->runtimeValue24;
              classStateField = &(modelRuntime->classState).stateFlags;
              *classStateField = *classStateField ^ 2;
            }
          }
          else {
            updatedChildTranslationZQ12 = childTranslationZQ12 + bobStep;
            if ((int)animationDefinition->runtimeValue28 < updatedChildTranslationZQ12) {
              updatedChildTranslationZQ12 = animationDefinition->runtimeValue28;
              classStateField = &(modelRuntime->classState).stateFlags;
              *classStateField = *classStateField ^ 2;
            }
          }
          (animatedNode->modelPayload).localTranslationZQ12 = updatedChildTranslationZQ12;
          animatedNode->runtimeFlags = animatedNode->runtimeFlags | 1;
        }
      }
    }
    ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
  }
  else if (((modelRuntime->classState).stateFlags & 4) == 0) {
    classStateField = &(modelRuntime->classState).stateFlags;
    *classStateField = *classStateField ^ 4;
    ArmyRuntime_RebuildDerivedSelectionMetrics(modelRuntime->ownerArmyRuntime);
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ModelRuntimeSlot *)modelRuntime);
  return;
}
