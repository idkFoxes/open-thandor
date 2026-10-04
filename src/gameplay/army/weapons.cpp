/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/weapons.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/army/weapons.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Implementation ownership: gameplay/army/weapons. */

/* Called by the weapon code (combat/movement) after a shot has been fired: stores the launch heading and the
   weapon definition's two post-launch values as the army's action vector, but only when both of those values
   are nonzero; otherwise the previous vector is kept.
*/
void ArmyRuntime_SetNonzeroActionVector
          (Q12 actionVector0,Q12 actionVector2,Q12 actionVector1,ArmyRuntimeSlot *armyRuntime)

{
  if ((actionVector1 != 0) && (actionVector2 != 0)) {
    armyRuntime->actionVector1Q12 = actionVector1;
    armyRuntime->actionVector0Q12 = actionVector0;
    armyRuntime->actionVector2Q12 = actionVector2;
  }
  return;
}

/* Resolves the world point a shooter at sourceWorld*Q12 aims its shot at: the explicit target position of the
   command, or the target entity's model (the flying body of an aircraft) raised by its definition's aim height.
   A moving target is led along its heading by the distance it covers during the shot's flight time, unless it
   stands still within that lead range. A target entity the shooter's faction can no longer see is dropped from
   the command. Returns true with the point in *outAimPoint, or false (and *outAimPoint zeroed) when there is
   nothing to aim at.
*/
Bool8
ArmyRuntime_ResolveShotAimPoint
          (Q12 sourceWorldZQ12,Q12 sourceWorldYQ12,Q12 sourceWorldXQ12,
          ShotDefinition *shotDefinition,GameEntityRuntime *targetState,GraphicsFixedVec3 *outAimPoint)

{
  int *targetDefinitionRecord;
  ModelDefinition *targetDefinition;
  ArmyRuntimeSlot *targetArmy;
  int64_t deltaYSquared;
  int64_t remainingRangeSquared;
  uint32_t visibilityMask;
  uint32_t targetDistance;
  uint32_t rampUpLeadTime;
  uint32_t targetHeading;
  int leadDistance;
  Q12 trackedX;
  Q12 trackedY;
  int deltaX;
  int deltaY;
  int aimWorldX;
  int aimWorldY;
  int aimWorldZ;
  ModelRuntimeNode *targetNode;
  Q12 shotLeadSpeedQ12;
  FixedDirection leadDirection;
  GameEntityRuntime *targetEntity;

  /* On failure the original leaves the aim point undefined (left-over intermediate values). All three callers
     ignore the coordinates on failure, so the point is zeroed. */
  outAimPoint->x = 0;
  outAimPoint->y = 0;
  outAimPoint->z = 0;
  if (((targetState->common).commandTarget.targetFlags & 1) == 0) {
    if (((targetState->common).commandTarget.targetFlags & 2) != 0) {
      outAimPoint->x = (targetState->common).commandTarget.targetWorldXQ12;
      outAimPoint->y = (targetState->common).commandTarget.targetWorldYQ12;
      outAimPoint->z = (targetState->common).commandTarget.targetWorldZQ12;
      return true;
    }
  }
  else {
    targetEntity = (targetState->common).commandTarget.targetEntity;
    if (targetEntity != nullptr) {
      /* bit 1 of the shooter faction's 2-bit field: the target is visible to that faction */
      visibilityMask = 2u << ((uint8_t)((targetState->common).ownership.ownerIndex * 2) & 31);
      targetNode = (targetEntity->common).ownership.modelNode;
      if (((targetEntity->common).damageState.factionVisibilityBits1C & visibilityMask) != 0) {
        if (((ModelRuntimeSlot *)(targetEntity->common).ownership.definitionOrClassRecord)->definitionOrSavedId.
            runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_21_AIRCRAFT) {
          targetNode = targetNode->childNodes[0];
        }
        aimWorldX = (targetNode->worldTransform).translation.x;
        targetDefinitionRecord = (int *)(targetEntity->common).ownership.definitionOrClassRecord;
        targetDefinition = ((ModelRuntimeSlot *)targetDefinitionRecord)->definitionOrSavedId.runtimeDefinition;
        aimWorldY = (targetNode->worldTransform).translation.y;
        aimWorldZ = (targetNode->worldTransform).translation.z + targetDefinition->aimHeightOffsetQ12;
        targetArmy = ((ModelRuntimeSlot *)targetDefinitionRecord)->ownerArmyRuntimeOrSavedOffset.armyRuntime;
        if ((targetDefinition->accelerationPerTick != 0) && ((targetArmy->movementStateFlags & 4) == 0)) {
          targetDistance = FixedMath_Length3(aimWorldZ - sourceWorldZQ12,aimWorldY - sourceWorldYQ12,
                                             aimWorldX - sourceWorldXQ12);
          shotLeadSpeedQ12 = ShotDefinition_GetLeadSpeed(shotDefinition);
          leadDistance = (int)(((int64_t)(int)targetDistance * (int64_t)targetDefinition->movementSpeed) /
                               (int64_t)shotLeadSpeedQ12);
          rampUpLeadTime = ShotDefinition_ComputeRampUpLeadTime(shotDefinition);
          leadDistance = leadDistance + rampUpLeadTime * targetDefinition->movementSpeed;
          targetHeading = FixedMath_Atan2Angle16
                            (targetArmy->movementPosition1Q12 -
                             targetArmy->modelNodeRuntime->worldTransform.translation.y,
                             targetArmy->movementPosition0Q12 -
                             targetArmy->modelNodeRuntime->worldTransform.translation.x);
          leadDirection = FixedMath_DirectionFromAnglesScaled(0,targetHeading,leadDistance);
          targetEntity = (targetState->common).commandTarget.targetEntity;
          trackedX = (targetEntity->common).damageState.trackedCoordinate0Q12;
          if (trackedX == (targetEntity->common).pathCoordinate0Q12) {
            deltaX = trackedX - (targetNode->worldTransform).translation.x;
            remainingRangeSquared = ((int64_t)(int)leadDirection.y * (int64_t)(int)leadDirection.y +
                                     (int64_t)(int)leadDirection.x * (int64_t)(int)leadDirection.x) -
                                    (int64_t)deltaX * (int64_t)deltaX;
            if (-1 < remainingRangeSquared) {
              trackedY = (targetEntity->common).damageState.trackedCoordinate1Q12;
              if (trackedY == (targetEntity->common).pathCoordinate1Q12) {
                deltaY = trackedY - (targetNode->worldTransform).translation.y;
                deltaYSquared = (int64_t)deltaY * (int64_t)deltaY;
                if (-1 < remainingRangeSquared - deltaYSquared) {
                  /* The target is standing still within lead range: aim at its path position directly. */
                  outAimPoint->x = (targetEntity->common).pathCoordinate0Q12;
                  outAimPoint->y = (targetEntity->common).pathCoordinate1Q12;
                  outAimPoint->z =
                       (((targetEntity->common).ownership.modelNode)->worldTransform).translation.z +
                       ((ModelRuntimeSlot *)(targetEntity->common).ownership.definitionOrClassRecord)->
                       definitionOrSavedId.runtimeDefinition->aimHeightOffsetQ12;
                  return true;
                }
              }
            }
          }
          aimWorldZ = leadDirection.z + aimWorldZ;
          aimWorldY = leadDirection.y + aimWorldY;
          aimWorldX = leadDirection.x + aimWorldX;
        }
        outAimPoint->x = aimWorldX;
        outAimPoint->y = aimWorldY;
        outAimPoint->z = aimWorldZ;
        return true;
      }
      (targetState->common).commandTarget.targetEntity = nullptr;
      (targetState->common).commandTarget.targetFlags = 0;
    }
  }
  return false;
}

/* Tests the army's summed weapon damage against target class 0 (targetClassShotDamage[0], stateOrTechnologyId)
   for zero, i.e. an unarmed army (returns true when it is zero); used by
   ArmyRuntime_ResetMovementStateFromModel to decide whether a targeted command is dropped.
*/
Bool8 ArmyRuntime_TestHasNoWeaponDamage(ArmyRuntimeSlot *armyRuntime)

{
  return armyRuntime->stateOrTechnologyId == 0;
}

/* Tests the army's summed weapon damage against target class 0 (targetClassShotDamage[0], stateOrTechnologyId)
   for being non-negative (returns true when it is >= 0). Used by the selection queries (selection/queries.cpp).
*/
Bool8 ArmyRuntime_TestWeaponDamageNonnegative(ArmyRuntimeSlot *armyRuntime)

{
  return -1 < armyRuntime->stateOrTechnologyId;
}

/* Depth-first over a model runtime tree (the attached child model runtimes, null slots skipped):
   the last node whose definition has class MODEL_RUNTIME_CLASS_10_CONTINUOUS_RADAR, or null.
   Part of ArmyRuntimeClass_SelectProjectileTargetNode (the original walks the tree inline). */
static uint8_t *ArmyRuntimeClass_FindLastClass10Node(uint8_t *node)
{
  uint8_t *found = nullptr;
  int i;
  if (((ModelRuntimeSlot *)node)->definitionOrSavedId.runtimeDefinition->runtimeClassId ==
      MODEL_RUNTIME_CLASS_10_CONTINUOUS_RADAR) {
    found = node;
  }
  for (i = 0; i < (int)((ModelRuntimeSlot *)node)->attachmentCount; i++) {
    uint8_t *child = (uint8_t *)((ModelRuntimeSlot *)node)->attachments[i].childModelRuntimeOrSavedOffset;
    if (child != nullptr) {
      uint8_t *match = ArmyRuntimeClass_FindLastClass10Node(child);
      if (match != nullptr) {
        found = match;
      }
    }
  }
  return found;
}

/* Owner-list callback of ArmyRuntimeClass_UpdateTimedTargetProjectilesAndEffects (passed to
   WorldRuntime_ForEachOwnerListNode). For a model of another, non-neutral faction within the shot's
   selection range it stores the last radar node (class 10) of that model as the target
   (timedTargetLinkState.selectedTargetModelRuntime), unless that node is destroyed; for a shot of the same shot
   definition it records that one is still in flight (timedTargetLinkState.matchingActiveShotRuntime).
*/

void ArmyRuntimeClass_SelectProjectileTargetNode(ModelRuntimeTimedTargetProjectileView *modelRuntime,
          WorldOwnerListNode *candidateNode)

{
  int64_t deltaYSquared;
  int64_t remainingRangeSquared;
  int deltaX;
  int deltaY;
  int selectionRange;
  int candidateFactionIndex;
  ModelRuntimeSlot *targetModelRuntime;

  if (candidateNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
    selectionRange = ((modelRuntime->modelDefinition->shotDefinitionReference).definition)->
            mode2SelectionRangeQ12;
    deltaX = candidateNode->worldXQ12 - (modelRuntime->rootModelNode->worldTransform).translation.x;
    remainingRangeSquared = (int64_t)selectionRange * (int64_t)selectionRange - (int64_t)deltaX * (int64_t)deltaX;
    if (remainingRangeSquared < 0) {
      return;
    }
    deltaY = candidateNode->worldYQ12 - (modelRuntime->rootModelNode->worldTransform).translation.y;
    deltaYSquared = (int64_t)deltaY * (int64_t)deltaY;
    /* the original tests the sign of the high dword of remainingRangeSquared - deltaYSquared; both are
       non-negative, so this is a plain comparison */
    if (remainingRangeSquared < deltaYSquared) {
      return;
    }
    candidateFactionIndex =
         ((ModelRuntimeSlot *)candidateNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex;
    if ((candidateFactionIndex == modelRuntime->ownerArmyRuntime->factionIndex) || (candidateFactionIndex == 0)) {
      return;
    }
    /* pick the last node, depth-first, whose definition has class 10 (runtimeClassId) */
    targetModelRuntime = (ModelRuntimeSlot *)ArmyRuntimeClass_FindLastClass10Node((uint8_t *)candidateNode->runtimePayload);
    if ((targetModelRuntime != nullptr) && (((targetModelRuntime->classState).stateFlags & ARMY_RUNTIME_FLAG_DESTROYED) == 0)) {
      (modelRuntime->timedTargetLinkState).selectedTargetModelRuntime = targetModelRuntime;
    }
  }
  else if ((candidateNode->ownerClassId == WORLD_OWNER_RUNTIME_SHOT) &&
          ((modelRuntime->modelDefinition->shotDefinitionReference).definition ==
           (ShotDefinition *)(((ModelRuntimeSlot *)candidateNode->runtimePayload)->definitionOrSavedId).definition)) {
    (modelRuntime->timedTargetLinkState).matchingActiveShotRuntime = (ShotRuntimeSlot *)candidateNode->runtimePayload;
  }
}

/* Runtime update of class 20 (a launcher that fires at enemy radar), reached only through
   g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[20]. When its reload countdown has run out it
   shows the loaded missile (mesh group bit 0) and scans the world with ArmyRuntimeClass_SelectProjectileTargetNode;
   if a radar target is in range and none of its own shots is still in flight, it hides the missile, restarts the
   reload and fires from its attachment points at the target's position.
*/

void ArmyRuntimeClass_UpdateTimedTargetProjectilesAndEffects
          (WorldRuntimeContext *worldRuntime,ModelRuntimeTimedTargetProjectileView *modelRuntime)

{
  ModelDefinitionTimedTargetProjectileView *timedTargetDefinition;
  ModelRuntimeNode *rootNode;
  ModelRuntimeNode *targetRootNode;
  ModelRuntimeSlot *selectedTarget;
  Q12 targetWorldXQ12;
  Q12 targetWorldYQ12;
  Q12 targetWorldZQ12;
  int reloadCountdownTicks;

  if (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_INACTIVE_MASK) == 0) {
    timedTargetDefinition = modelRuntime->modelDefinition;
    reloadCountdownTicks = (modelRuntime->timedTargetState).targetProjectileReloadCountdownTicks -
            g_InGameSimulationStepTicks;
    rootNode = modelRuntime->rootModelNode;
    (modelRuntime->timedTargetState).targetProjectileReloadCountdownTicks = reloadCountdownTicks;
    if (reloadCountdownTicks < 1) {
      (modelRuntime->timedTargetState).targetProjectileReloadCountdownTicks = 0;
      /* show the loaded missile */
      (rootNode->modelPayload).meshGroupMask |= 1;
      (modelRuntime->timedTargetLinkState).selectedTargetModelRuntime = nullptr;
      (modelRuntime->timedTargetLinkState).matchingActiveShotRuntime = nullptr;
      WorldRuntime_ForEachOwnerListNode
                (modelRuntime,THANDOR_SLOT(ArmyRuntimeClass_SelectProjectileTargetNode),
                 worldRuntime);
      selectedTarget = (modelRuntime->timedTargetLinkState).selectedTargetModelRuntime;
      if (((modelRuntime->timedTargetLinkState).matchingActiveShotRuntime ==
           nullptr) && (selectedTarget != nullptr)) {
        targetRootNode = (selectedTarget->rootModelNodeOrSavedOffset).modelNode;
        targetWorldXQ12 = (targetRootNode->worldTransform).translation.x;
        targetWorldYQ12 = (targetRootNode->worldTransform).translation.y;
        targetWorldZQ12 = (targetRootNode->worldTransform).translation.z;
        rootNode = modelRuntime->rootModelNode;
        (modelRuntime->timedTargetState).targetProjectileReloadCountdownTicks +=
             (timedTargetDefinition->timedTargetParameters).reloadTicks;
        /* hide the missile and fire */
        rootNode->runtimeFlags = rootNode->runtimeFlags | 1;
        (rootNode->modelPayload).meshGroupMask &= ~1u;
        ModelRuntime_EmitProjectilesFromAttachmentPoints
                  ((ShotTargetModelReference) /* 32-bit format field: ShotTargetModelReference (ShotRuntimeSlot +0x14) */
                   (modelRuntime->timedTargetLinkState).selectedTargetModelRuntime,targetWorldZQ12
                   ,targetWorldYQ12,targetWorldXQ12,(timedTargetDefinition->shotDefinitionReference).definition,
                   rootNode,Thandor_U32ToPointer<MdlSerializedNodeHeader>(timedTargetDefinition->rootNodeOffsetOrPointer), /* 32-bit format field: ModelDefinition.rootNodeOffsetOrPointer */
                   worldRuntime);
      }
    }
  }
  ModelNodeRuntime_RebuildTransformsFromRoot(modelRuntime->rootModelNode);
  (modelRuntime->timedTargetLinkState).selectedTargetModelRuntime = nullptr;
  (modelRuntime->timedTargetLinkState).matchingActiveShotRuntime = nullptr;
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ModelRuntimeSlot *)modelRuntime);
  return;
}

/* Fires a shot of the weapon code (called directly by gameplay/army/combat): looks up the launch point
   (packed key attachmentSelectorOrdinal << 4 | 2) in the sprite model of definitionNode, transforms it by the
   freshly rebuilt modelNode and creates the projectile from there towards the target point. Returns true
   when the model has no such launch point.
*/
Bool8 ArmyRuntime_ResolveShotLaunchFromModelAttachment
          (ShotTargetModelReference targetModelReference,Q12 targetWorldXQ12,Q12 targetWorldYQ12,
          Q12 targetWorldZQ12,SprAttachmentSelectorOrdinal attachmentSelectorOrdinal,
          ShotDefinition *shotDefinition,ModelRuntimeNode *modelNode,
          MdlSerializedNodeHeader *definitionNode,WorldRuntimeContext *worldRuntime)

{
  ModelResource *spriteModelResource;
  Q12 launchWorldYQ12;
  Q12 launchWorldZQ12;
  ModelPackedLookupTableEntryCount remainingEntries;
  ModelPackedPointRecord *localPointRecord;
  ModelWorldPoint launchPoint;
  
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNode);
  spriteModelResource = (definitionNode->spriteAssetReference).modelResource;
  remainingEntries = spriteModelResource->packedLookupTableEntryCount;
  localPointRecord =
       (ModelPackedPointRecord *)((uint8_t *)spriteModelResource + spriteModelResource->packedLookupTableRelativeOffset);
  /* find the attachment point record (key = selector << 4 | 2); none -> fail */
  while (remainingEntries != 0 && localPointRecord->packedLookupKey != (attachmentSelectorOrdinal << 4 | 2)) {
    localPointRecord = localPointRecord + 1;
    remainingEntries = remainingEntries - 1;
  }
  if (remainingEntries == 0) {
    return true;
  }
  launchPoint = ModelNodeRuntime_TransformLocalPoint(localPointRecord,modelNode);
  launchWorldZQ12 = launchPoint.zQ12;
  launchWorldYQ12 = launchPoint.yQ12;
  ShotRuntimePool_CreateProjectileFromDefinition
            (targetModelReference,
             (ArmyRuntimeSlot *)((modelNode->runtimePayload).armyRuntime)->linkedEntityRuntime,
             targetWorldXQ12,targetWorldYQ12,targetWorldZQ12,launchWorldZQ12,launchWorldYQ12,
             launchPoint.xQ12,shotDefinition,worldRuntime);
  return false;
}

/* Bomb release of an aircraft on its attack run (called directly by ArmyRuntimeClass_UpdateAircraft):
   scores every intact model (stateFlags bit 8 clear) of another, non-neutral faction with AiCombatTarget_EvaluateCandidateScore. If the
   best one is within 1.0 (Q12) of its radius from the given point, every shot aims at it; otherwise each shot
   aims at the given point shifted by its launch point's offset from the aircraft. One shot of
   effectDefinitionId (despite the name a shot definition) is created from every launch point with packed key
   modelPointOrdinal << 4 | 2.
*/
void ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
          (EffectCreationFlagBits effectFlags,Q12 worldZQ12,Q12 worldYQ12,Q12 worldXQ12,
          ModelAttachmentOrdinal modelPointOrdinal,PckEffectDefinitionIdCatalog effectDefinitionId,
          void *sourceRuntime,void *modelPointTable,WorldRuntimeContext *worldContext)

{
  ArmyRuntimeSlot *sourceArmyRuntime;
  ArmyRuntimeSlot *candidateArmyRuntime;
  ModelRuntimeSlot *candidateModelRuntime;
  Q12 candidateWorldZ;
  uint32_t candidateScore;
  uint32_t candidateDistance;
  int deltaY;
  uint32_t currentBestScore;
  int deltaX;
  int candidateRadius;
  ModelResource *spriteModelResource;
  int remainingEntries;
  WorldOwnerListNode *nodeCursor;
  ModelPackedPointRecord *localPointRecord;
  ModelWorldPoint localPoint;
  WorldOwnerListNode *bestCandidateNode;
  uint32_t pointOffsetMask;

  bestCandidateNode = nullptr;
  nodeCursor = worldContext->ownerListHead;
  pointOffsetMask = UINT32_MAX;
  currentBestScore = 0;
  sourceArmyRuntime =
       ((ModelRuntimeNode *)sourceRuntime)->runtimePayload.modelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime;
  do {
    if ((nodeCursor->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) && (nodeCursor != sourceRuntime)) {
      candidateModelRuntime = (ModelRuntimeSlot *)nodeCursor->runtimePayload;
      candidateArmyRuntime = candidateModelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime;
      if (((candidateModelRuntime->classState.stateFlags & ARMY_RUNTIME_FLAG_DESTROYED) == 0) &&
          (candidateArmyRuntime->factionIndex != 0) &&
          (candidateArmyRuntime->factionIndex != sourceArmyRuntime->factionIndex)) {
        /* Original quirk: the shooter's weaponRangeQ12 is overwritten with each candidate's footprint radius
           (read by AiCombatTarget_EvaluateCandidateScore) and never restored. */
        sourceArmyRuntime->weaponRangeQ12 =
             (((candidateArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->definitionOrSavedId).
             runtimeDefinition->footprintRadius;
        candidateScore = AiCombatTarget_EvaluateCandidateScore
                          (currentBestScore,1,UINT32_MAX,UINT32_MAX,candidateArmyRuntime,
                           sourceArmyRuntime);
        if (currentBestScore < candidateScore) {
          currentBestScore = candidateScore;
          bestCandidateNode = nodeCursor;
        }
      }
    }
    nodeCursor = nodeCursor->nextNode;
  } while (nodeCursor != nullptr);
  if (bestCandidateNode != nullptr) {
    candidateWorldZ = bestCandidateNode->worldZQ12;
    deltaX = bestCandidateNode->worldXQ12 - worldXQ12;
    deltaY = bestCandidateNode->worldYQ12 - worldYQ12;
    candidateRadius =
         ((ModelRuntimeSlot *)bestCandidateNode->runtimePayload)->definitionOrSavedId.runtimeDefinition->
         footprintRadius;
    candidateDistance = FixedMath_Length2(deltaY,deltaX);
    if ((int)(candidateDistance - candidateRadius) < Q12_ONE + 1) {
      pointOffsetMask = 0;
      worldXQ12 = deltaX + worldXQ12;
      worldYQ12 = deltaY + worldYQ12;
      worldZQ12 = candidateWorldZ;
    }
  }
  ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)sourceRuntime);
  spriteModelResource = ((MdlSerializedNodeHeader *)modelPointTable)->spriteAssetReference.modelResource;
  localPointRecord =
       (ModelPackedPointRecord *)
       ((uint8_t *)spriteModelResource + spriteModelResource->packedLookupTableRelativeOffset);
  for (remainingEntries = spriteModelResource->packedLookupTableEntryCount;
      remainingEntries != 0; remainingEntries = remainingEntries - 1) {
    if (localPointRecord->packedLookupKey == (modelPointOrdinal << 4 | 2)) {
      localPoint = ModelNodeRuntime_TransformLocalPoint(localPointRecord,(ModelRuntimeNode *)sourceRuntime);
      ShotRuntimePool_CreateProjectileFromDefinition
                (effectFlags,
                 ((ModelRuntimeNode *)sourceRuntime)->runtimePayload.modelRuntime->ownerArmyRuntimeOrSavedOffset.
                 armyRuntime,worldZQ12,
                 (localPoint.yQ12 - ((ModelRuntimeNode *)sourceRuntime)->worldTransform.translation.y &
                 pointOffsetMask) + worldYQ12,
                 (localPoint.xQ12 - ((ModelRuntimeNode *)sourceRuntime)->worldTransform.translation.x &
                 pointOffsetMask) + worldXQ12,
                 localPoint.zQ12,localPoint.yQ12,localPoint.xQ12,Thandor_U32ToPointer<ShotDefinition>(effectDefinitionId),worldContext); /* 32-bit format field: ModelDefinition.modelPointEffectId (relocated shot reference) */
    }
    localPointRecord = localPointRecord + 1;
  }
  return;
}

/* One tick of a model whose health is gone (called directly by
   ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive, which then counts the eight channel timers
   destructionEffectTimers down). For every channel i whose timer is 0 it spawns the channel's
   effect (definition (&destructionEffect0)[2 * i]) at every model point with packed key i << 4 | 3 of the root
   model, using the root's orientation, and, when the definition has a child model, at those of child node 0 with
   a fixed orientation. Also sets its own health to 0 and clears health and link (linkedModelRuntimeOrSavedOffset)
   of every attached child model.
*/
void ArmyRuntime_ProcessReadyAttachmentChannels(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  ModelRuntimeSlot *childModelRuntime;
  uint32_t channelIndex;
  uint32_t remainingAttachments;
  int remainingRecords;
  ModelResource *rootModelResource;
  ModelResource *childModelResource;
  MdlSerializedNodeHeader *rootNodeHeader;
  MdlSerializedNodeHeader *childNodeHeader;
  ModelPackedPointRecord *pointRecord;
  ModelWorldPoint localPoint;
  ModelRuntimeNode *modelNode;

  /* root model: the effects use the root node's orientation */
  rootModelResource =
       (ModelResource *)Thandor_U32ToPointer<MdlSerializedNodeHeader>(
                         (modelRuntime->definitionOrSavedId).runtimeDefinition->rootNodeOffsetOrPointer)-> /* 32-bit format field: ModelDefinition.rootNodeOffsetOrPointer */
       spriteAssetReference.modelResource;
  modelRuntime->health = 0;
  for (channelIndex = 0; channelIndex < 8; channelIndex = channelIndex + 1) {
    if (modelRuntime->destructionEffectTimers[channelIndex] != 0) {
      continue;
    }
    pointRecord =
         (ModelPackedPointRecord *)
         ((uint8_t *)rootModelResource + rootModelResource->packedLookupTableRelativeOffset);
    for (remainingRecords = rootModelResource->packedLookupTableEntryCount; remainingRecords != 0;
        remainingRecords = remainingRecords - 1) {
      if (channelIndex * 16 + 3 == pointRecord->packedLookupKey) {
        localPoint = ModelNodeRuntime_TransformLocalPoint
                          (pointRecord,(modelRuntime->rootModelNodeOrSavedOffset).modelNode);
        modelNode = (modelRuntime->rootModelNodeOrSavedOffset).modelNode;
        EffectRuntimePool_CreateInstanceFromDefinition
                  (EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY,
                   *(EffectRuntimeOwnerReference *)
                    &modelRuntime->linkedModelRuntimeOrSavedOffset,
                   (modelNode->modelPayload).worldRotationAngle2,
                   (modelNode->modelPayload).worldRotationAngle1,
                   (modelNode->modelPayload).worldRotationAngle0,localPoint.zQ12,localPoint.yQ12,localPoint.xQ12,
                   /* the definition's eight {effect, value} pairs from destructionEffect0 */
                   (&(modelRuntime->definitionOrSavedId).runtimeDefinition->destructionEffect0)
                   [channelIndex * 2].definition,worldRuntime);
      }
      pointRecord = pointRecord + 1;
    }
  }
  /* child model 0 (when its node flags' low nibble is 0): the effects use a fixed orientation */
  rootNodeHeader =
       Thandor_U32ToPointer<MdlSerializedNodeHeader>((modelRuntime->definitionOrSavedId).runtimeDefinition->rootNodeOffsetOrPointer); /* 32-bit format field: ModelDefinition.rootNodeOffsetOrPointer */
  if (rootNodeHeader->childCount != 0) {
    childNodeHeader = Thandor_U32ToPointer<MdlSerializedNodeHeader>(rootNodeHeader->childSerializedOffsets[0]); /* 32-bit format field: MdlSerializedNodeHeader.childSerializedOffsets */
    childModelResource = (ModelResource *)childNodeHeader->spriteAssetReference.modelResource;
    if ((childNodeHeader->nodeFlags & 0xf) == 0) {
      for (channelIndex = 0; channelIndex < 8; channelIndex = channelIndex + 1) {
        if (modelRuntime->destructionEffectTimers[channelIndex] != 0) {
          continue;
        }
        pointRecord =
             (ModelPackedPointRecord *)
             ((uint8_t *)childModelResource + childModelResource->packedLookupTableRelativeOffset);
        for (remainingRecords = childModelResource->packedLookupTableEntryCount; remainingRecords != 0;
            remainingRecords = remainingRecords - 1) {
          if ((channelIndex * 16 + 3 == pointRecord->packedLookupKey) &&
              (((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->childCount != 0)) {
            modelNode = ((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->childNodes[0];
            if (modelNode != nullptr) {
              localPoint = ModelNodeRuntime_TransformLocalPoint(pointRecord,modelNode);
              EffectRuntimePool_CreateInstanceFromDefinition
                        (EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY,
                         *(EffectRuntimeOwnerReference *)
                          &modelRuntime->linkedModelRuntimeOrSavedOffset,0,FIXED_ANGLE16_QUARTER_TURN,0,localPoint.zQ12,
                         localPoint.yQ12,localPoint.xQ12,
                         (&(modelRuntime->definitionOrSavedId).runtimeDefinition->destructionEffect0)
                         [channelIndex * 2].definition,worldRuntime);
            }
          }
          pointRecord = pointRecord + 1;
        }
      }
    }
  }
  /* modelRuntime advances by one 0x20-byte attachment record per iteration */
  for (remainingAttachments = modelRuntime->attachmentCount; remainingAttachments != 0; remainingAttachments = remainingAttachments - 1) {
    childModelRuntime = modelRuntime->attachments[0].childModelRuntimeOrSavedOffset;
    if (childModelRuntime != nullptr) {
      childModelRuntime->health = 0;
      (childModelRuntime->linkedModelRuntimeOrSavedOffset).modelRuntime = nullptr;
    }
    modelRuntime = (ModelRuntimeSlot *)((uint8_t *)modelRuntime + sizeof(ModelRuntimeAttachmentDescriptor));
  }
  return;
}

/* Picks the model point of the timed effect emitter: the model (the first child for aircraft) has effect points
   with packed keys n << 4 | 6; one of them is chosen in turn (definition modelFlags bit 0) or at random and
   transformed to world space. Returns false when the model has no such point (the caller then uses the root
   position). */
static Bool8 ArmyEmitter_FindEffectPoint(ModelRuntimeUpdateView *modelRuntime,ModelDefinition *emitterDefinition,
          ModelWorldPoint *outWorldPoint)
{
  MdlSerializedNodeHeader *serializedNode;
  ModelResource *modelResource;
  int remainingRecords;
  uint32_t *pointRecordCursor;
  uint32_t pointCount;
  uint32_t pointSelector;
  ModelPackedPointRecord *emitterPoint;
  ModelRuntimeNode *modelNode;

  serializedNode = Thandor_U32ToPointer<MdlSerializedNodeHeader>(emitterDefinition->rootNodeOffsetOrPointer); /* 32-bit format field: ModelDefinition.rootNodeOffsetOrPointer */
  if (emitterDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_21_AIRCRAFT) {
    serializedNode = Thandor_U32ToPointer<MdlSerializedNodeHeader>(serializedNode->childSerializedOffsets[0]); /* 32-bit format field: MdlSerializedNodeHeader.childSerializedOffsets */
  }
  modelResource = (ModelResource *)serializedNode->spriteAssetReference.modelResource;
  remainingRecords = modelResource->packedLookupTableEntryCount;
  if (remainingRecords == 0) {
    return false;
  }
  pointCount = 0;
  pointRecordCursor = (uint32_t *)((uint8_t *)modelResource + modelResource->packedLookupTableRelativeOffset);
  /* count the effect points: highest n + 1 of the packed keys n << 4 | 6 */
  for (; remainingRecords != 0; remainingRecords = remainingRecords - 1, pointRecordCursor = pointRecordCursor + 4) {
    if (((*pointRecordCursor & 0xf) == 6) && (pointCount <= *pointRecordCursor >> 4)) {
      pointCount = (*pointRecordCursor >> 4) + 1;
    }
  }
  if (pointCount == 0) {
    return false;
  }
  pointSelector = (modelRuntime->classState).effectEmitterPointIndex;
  if ((emitterDefinition->modelFlags & 1) == 0) {
    pointSelector = g_RandomGeneratorState.next();
  }
  if (!ModelLookupTable_FindPackedPoint
            (pointSelector % pointCount,6,serializedNode->spriteAssetReference.modelResource,&emitterPoint)) {
    return false;
  }
  modelNode = modelRuntime->rootModelNode;
  if (emitterDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_21_AIRCRAFT) {
    modelNode = modelNode->childNodes[0];
  }
  *outWorldPoint = ModelNodeRuntime_TransformLocalPoint(emitterPoint,modelNode);
  return true;
}

/* Timed emitters of an army model, reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes
   .runtimeUpdate[0] and [16] and directly from most class updates (here, gameplay/army/combat and movement).
   Timer classState.shotEmitterTimerTicks: fires the definition's shot (emitterShotDefinitionReference) straight
   ahead (1.0 along the root's orientation) and restarts at shotEmitterIntervalTicks plus a random part below
   shotEmitterRandomTicks. Timer classState.effectEmitterTimerTicks: spawns the land
   (emitterEffectDefinitionReference) or, where FieldGrid_GetNearestWaterDelta is positive, the water
   (waterEmitterEffectDefinitionReference) effect at a model point with key n << 4 | 6 (random, or in turn when
   definition modelFlags bit 0 is set; the root position when there is none) and restarts at
   effectEmitterIntervalTicks plus a random part below effectEmitterRandomTicks.
*/
void ArmyRuntime_UpdateTimedShotAndEffectEmitters
          (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView *modelRuntime)

{
  ModelDefinition *emitterDefinition;
  int previousTimerTicks;
  uint32_t randomValue;
  uint32_t randomTicks;
  ModelRuntimeNode *modelNode;
  FixedDirection launchDirection;
  GraphicsWorldCoordinateQ12 launchWorldZQ12;
  GraphicsWorldCoordinateQ12 launchWorldYQ12;
  GraphicsWorldCoordinateQ12 launchWorldXQ12;
  ShotDefinition *shotDefinition;
  ModelWorldPoint emitterPoint;
  uint32_t worldX;
  uint32_t worldY;
  uint32_t worldZQ12;
  int32_t waterDelta;
  EffectDefinition *effectDefinition;
  AngleTurn32 orientationAngle0;
  AngleTurn32 orientationAngle1;
  AngleTurn32 orientationAngle2;

  emitterDefinition = modelRuntime->modelDefinition;
  previousTimerTicks = (int)(modelRuntime->classState).shotEmitterTimerTicks;
  (modelRuntime->classState).shotEmitterTimerTicks =
       (modelRuntime->classState).shotEmitterTimerTicks - g_InGameSimulationStepTicks;
  /* the shot timer has reached 0 or below (signed compare of the old value with the step) */
  if (previousTimerTicks <= (int)g_InGameSimulationStepTicks &&
     ((emitterDefinition->emitterShotDefinitionReference).definition != (ShotDefinition *)(intptr_t)-1)) {
    randomTicks = 0;
    if (emitterDefinition->shotEmitterRandomTicks != 0) {
      randomValue = g_RandomGeneratorState.next();
      randomTicks = randomValue % emitterDefinition->shotEmitterRandomTicks;
    }
    shotDefinition = (emitterDefinition->emitterShotDefinitionReference).definition;
    modelNode = modelRuntime->rootModelNode;
    (modelRuntime->classState).shotEmitterTimerTicks = randomTicks + emitterDefinition->shotEmitterIntervalTicks;
    launchWorldXQ12 = (modelNode->worldTransform).translation.x;
    launchWorldYQ12 = (modelNode->worldTransform).translation.y;
    launchWorldZQ12 = (modelNode->worldTransform).translation.z;
    launchDirection = FixedMath_DirectionFromAnglesScaled
                       ((modelNode->modelPayload).worldRotationAngle1,
                        (modelNode->modelPayload).worldRotationAngle0,Q12_ONE);
    ShotRuntimePool_CreateProjectileFromDefinition
              (0,modelRuntime->ownerArmyRuntime,launchDirection.z + launchWorldZQ12,
               launchDirection.y + launchWorldYQ12,launchDirection.x + launchWorldXQ12,launchWorldZQ12,
               launchWorldYQ12,launchWorldXQ12,shotDefinition,worldRuntime);
  }
  emitterDefinition = modelRuntime->modelDefinition;
  previousTimerTicks = (int)(modelRuntime->classState).effectEmitterTimerTicks;
  (modelRuntime->classState).effectEmitterTimerTicks =
       (modelRuntime->classState).effectEmitterTimerTicks - g_InGameSimulationStepTicks;
  /* the effect timer has reached 0 or below (signed) and there is an effect to emit */
  if (previousTimerTicks <= (int)g_InGameSimulationStepTicks &&
      ((emitterDefinition->emitterEffectDefinitionReference).definition != nullptr ||
       (emitterDefinition->waterEmitterEffectDefinitionReference).definition != nullptr)) {
    randomTicks = 0;
    if (emitterDefinition->effectEmitterRandomTicks != 0) {
      randomValue = g_RandomGeneratorState.next();
      randomTicks = randomValue % emitterDefinition->effectEmitterRandomTicks;
    }
    (modelRuntime->classState).effectEmitterTimerTicks = randomTicks + emitterDefinition->effectEmitterIntervalTicks;
    if (ArmyEmitter_FindEffectPoint(modelRuntime,emitterDefinition,&emitterPoint)) {
      worldZQ12 = emitterPoint.zQ12;
      worldY = emitterPoint.yQ12;
      worldX = emitterPoint.xQ12;
    }
    else {
      modelNode = modelRuntime->rootModelNode;
      worldX = (modelNode->worldTransform).translation.x;
      worldY = (modelNode->worldTransform).translation.y;
      worldZQ12 = (modelNode->worldTransform).translation.z;
    }
    effectDefinition = (emitterDefinition->emitterEffectDefinitionReference).definition;
    waterDelta = FieldGrid_GetNearestWaterDelta(worldY,worldX,worldRuntime->fieldGrid);
    if (0 < waterDelta) {
      effectDefinition = (emitterDefinition->waterEmitterEffectDefinitionReference).definition;
    }
    if ((effectDefinition == nullptr) ||
       ((effectDefinition->transitionPrefix).transitionKind !=
        EFFECT_TRANSITION_INTEGRATE_LINEAR_MOTION_AND_SHADING_POSITION)) {
      modelNode = modelRuntime->rootModelNode;
      orientationAngle2 = (modelNode->modelPayload).worldRotationAngle0;
      orientationAngle1 = (modelNode->modelPayload).worldRotationAngle1;
      orientationAngle0 = (modelNode->modelPayload).worldRotationAngle2;
    }
    else {
      randomValue = g_RandomGeneratorState.next();
      orientationAngle0 = randomValue & FIXED_ANGLE16_MASK;
      orientationAngle1 = FIXED_ANGLE16_QUARTER_TURN - (randomValue >> 20);
      orientationAngle2 = orientationAngle0;
    }
    EffectRuntimePool_CreateInstanceFromDefinition
              (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_COMPOUND(EffectRuntimeOwnerReference){ .modelNode = nullptr },orientationAngle0,
               orientationAngle1,orientationAngle2,worldZQ12,worldY,worldX,effectDefinition,worldRuntime);
    (modelRuntime->classState).effectEmitterPointIndex = (modelRuntime->classState).effectEmitterPointIndex + 1;
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ModelRuntimeSlot *)modelRuntime);
  return;
}

/* Fires a shot from every launch point of a model node: rebuilds the node transforms, then for each point record
   of the node's sprite asset with kind 2 (low nibble of packedLookupKey) creates a projectile from shotDefinition
   at the point's world position, aimed at the target shifted by the point's X/Y offset from the node, so that
   side-by-side launchers fire parallel shots. Called by the army turret code
   (src/gameplay/army/turrets.cpp).
*/
void ModelRuntime_EmitProjectilesFromAttachmentPoints
          (ShotTargetModelReference targetModelReference,Q12 targetWorldZQ12,Q12 targetWorldYQ12,
          Q12 targetWorldXQ12,ShotDefinition *shotDefinition,ModelRuntimeNode *modelNodeRuntime,
          MdlSerializedNodeHeader *definitionNode,WorldRuntimeContext *worldRuntime)

{
  int modelPointRecordsRemaining;
  ModelPackedPointRecord *localPointRecord;
  ModelWorldPoint launchPointWorld;
  AssetRecordByteCount modelPointTableBase;

  ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
  /* sprite asset: +0xE4 offset of the point records, +0xE8 their count */
  /* 32-bit format field: MdlSerializedNodeHeader.spriteAssetReference (ModelResource address in a 32-bit slot) */
  modelPointTableBase = definitionNode->spriteAssetReference.savedId;
  localPointRecord =
       Thandor_U32ToPointer<ModelPackedPointRecord>(
       modelPointTableBase +
       Thandor_U32ToPointer<ModelResource>(modelPointTableBase)->packedLookupTableRelativeOffset);
  for (modelPointRecordsRemaining =
           Thandor_U32ToPointer<ModelResource>(modelPointTableBase)->packedLookupTableEntryCount;
      modelPointRecordsRemaining != 0; modelPointRecordsRemaining--)
  {
    if ((localPointRecord->packedLookupKey & 0xf) == MODEL_POINT_CLASS_SHOT) {
      launchPointWorld = ModelNodeRuntime_TransformLocalPoint(localPointRecord,modelNodeRuntime);
      ShotRuntimePool_CreateProjectileFromDefinition
                (targetModelReference,
                 (ArmyRuntimeSlot *)
                 modelNodeRuntime->runtimePayload.armyRuntime->linkedEntityRuntime,
                 targetWorldZQ12,
                 (launchPointWorld.yQ12 - modelNodeRuntime->worldTransform.translation.y) + targetWorldYQ12,
                 (launchPointWorld.xQ12 - modelNodeRuntime->worldTransform.translation.x) + targetWorldXQ12,
                 launchPointWorld.zQ12,launchPointWorld.yQ12,launchPointWorld.xQ12,shotDefinition,worldRuntime);
    }
    localPointRecord++;
  }
}
