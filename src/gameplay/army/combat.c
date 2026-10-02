/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/combat.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/army/combat.h>
#include <thandor/thandor.h>

/* Implementation ownership: gameplay/army/combat. */

/* Address: 0x00523980.
   Runtime update of the turret-weapon class (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[9],
   0x0051FCBC). Counts down the reload timers of the eight launch attachments (showing a slot's projectile mesh
   bit again when it is loaded) and the shared inter-shot timer, resolves the aim point of the current target and
   turns the turret (yaw on the root node, pitch on its first child) toward the launch angles; without a target it
   moves and returns the turret to rest. Once yaw and pitch are on target, the inter-shot timer has run out and
   the target-following check passes, it fires from the first loaded attachment. Ends with the damage-threshold
   effect.
*/

void ArmyRuntimeWeapon_UpdateTargetAimAndFireAttachments
          (WorldRuntimeContext *worldRuntime,ModelRuntimeWeaponAimStateView *modelRuntime)

{
  ModelRuntimeFlags *nodeRuntimeFlags;
  uint32_t *remainingTicks;
  ModelMeshGroupMask *nodeMeshMask;
  ArmyWeaponDefinitionView *weaponDefinitionView;
  ModelRuntimeNode *pitchNode;
  MdlSerializedNodeHeader *attachmentNodeHeader;
  ArmyRuntimeSlot *commandTargetArmy;
  InGameSimulationStepBatchTicks stepTicks;
  Q12 aimXQ12;
  ShotTargetModelReference targetRuntimeReference;
  Q12 aimYQ12;
  Q12 aimZQ12;
  AngleTurn32 targetPitchAngle16;
  SprAttachmentSelectorOrdinal attachmentSelectorOrdinal;
  bool callCarry;
  ShotLaunchAngles launchAngles;
  ModelRelativeDirectionAngles relativeAngles;
  uint32_t pitchAimValue;
  bool movementArrived;
  Q12 steerWorldXQ12; /* unused here */
  Q12 steerWorldYQ12; /* unused here */
  GraphicsFixedVec3 aimPoint;
  bool aimPointFound;
  GameEntityRuntime *ownerEntity;
  ModelRuntimeNode *currentNode;
  
  stepTicks = g_InGameSimulationStepTicks;
  currentNode = modelRuntime->rootModelNode->childNodes[0]->childNodes[0];
  if (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_INACTIVE_MASK) == 0) {
    /* reload of attachment slot i done: show its projectile (mesh group bit i of the barrel node) */
    remainingTicks = modelRuntime->attachmentReloadTicks;
    *remainingTicks = *remainingTicks - g_InGameSimulationStepTicks;
    if ((int)*remainingTicks < 0) {
      modelRuntime->attachmentReloadTicks[0] = 0;
      nodeMeshMask = &(currentNode->modelPayload).meshGroupMask;
      *nodeMeshMask = *nodeMeshMask | ARMY_WEAPON_ATTACHMENT_MESH_BIT(0);
    }
    remainingTicks = modelRuntime->attachmentReloadTicks + 1;
    *remainingTicks = *remainingTicks - stepTicks;
    if ((int)*remainingTicks < 0) {
      modelRuntime->attachmentReloadTicks[1] = 0;
      nodeMeshMask = &(currentNode->modelPayload).meshGroupMask;
      *nodeMeshMask = *nodeMeshMask | ARMY_WEAPON_ATTACHMENT_MESH_BIT(1);
    }
    remainingTicks = modelRuntime->attachmentReloadTicks + 2;
    *remainingTicks = *remainingTicks - stepTicks;
    if ((int)*remainingTicks < 0) {
      modelRuntime->attachmentReloadTicks[2] = 0;
      nodeMeshMask = &(currentNode->modelPayload).meshGroupMask;
      *nodeMeshMask = *nodeMeshMask | ARMY_WEAPON_ATTACHMENT_MESH_BIT(2);
    }
    remainingTicks = modelRuntime->attachmentReloadTicks + 3;
    *remainingTicks = *remainingTicks - stepTicks;
    if ((int)*remainingTicks < 0) {
      modelRuntime->attachmentReloadTicks[3] = 0;
      nodeMeshMask = &(currentNode->modelPayload).meshGroupMask;
      *nodeMeshMask = *nodeMeshMask | ARMY_WEAPON_ATTACHMENT_MESH_BIT(3);
    }
    remainingTicks = modelRuntime->attachmentReloadTicks + 4;
    *remainingTicks = *remainingTicks - stepTicks;
    if ((int)*remainingTicks < 0) {
      modelRuntime->attachmentReloadTicks[4] = 0;
      nodeMeshMask = &(currentNode->modelPayload).meshGroupMask;
      *nodeMeshMask = *nodeMeshMask | ARMY_WEAPON_ATTACHMENT_MESH_BIT(4);
    }
    remainingTicks = modelRuntime->attachmentReloadTicks + 5;
    *remainingTicks = *remainingTicks - stepTicks;
    if ((int)*remainingTicks < 0) {
      modelRuntime->attachmentReloadTicks[5] = 0;
      nodeMeshMask = &(currentNode->modelPayload).meshGroupMask;
      *nodeMeshMask = *nodeMeshMask | ARMY_WEAPON_ATTACHMENT_MESH_BIT(5);
    }
    remainingTicks = modelRuntime->attachmentReloadTicks + 6;
    *remainingTicks = *remainingTicks - stepTicks;
    if ((int)*remainingTicks < 0) {
      modelRuntime->attachmentReloadTicks[6] = 0;
      nodeMeshMask = &(currentNode->modelPayload).meshGroupMask;
      *nodeMeshMask = *nodeMeshMask | ARMY_WEAPON_ATTACHMENT_MESH_BIT(6);
    }
    remainingTicks = modelRuntime->attachmentReloadTicks + 7;
    *remainingTicks = *remainingTicks - stepTicks;
    if ((int)*remainingTicks < 0) {
      modelRuntime->attachmentReloadTicks[7] = 0;
      nodeMeshMask = &(currentNode->modelPayload).meshGroupMask;
      *nodeMeshMask = *nodeMeshMask | ARMY_WEAPON_ATTACHMENT_MESH_BIT(7);
    }
    remainingTicks = &modelRuntime->sharedInterShotTicks;
    *remainingTicks = *remainingTicks - stepTicks;
    if ((int)*remainingTicks < 0) {
      modelRuntime->sharedInterShotTicks = 0;
    }
    weaponDefinitionView = modelRuntime->modelDefinition;
    ownerEntity = (GameEntityRuntime *)modelRuntime->ownerArmyRuntime;
    currentNode = modelRuntime->rootModelNode;
    aimPointFound = ArmyRuntime_ResolveShotAimPoint
                       ((currentNode->worldTransform).translation.z,
                        (currentNode->worldTransform).translation.y,
                        (currentNode->worldTransform).translation.x,weaponDefinitionView->shotDefinition,
                        ownerEntity,&aimPoint);
    aimZQ12 = aimPoint.z;
    aimYQ12 = aimPoint.y;
    aimXQ12 = aimPoint.x;
    if (!aimPointFound) {
      movementArrived = ArmyRuntime_UpdateMovementAndWaypoints
                         (worldRuntime,(ArmyMovementRuntime *)ownerEntity,&steerWorldXQ12,&steerWorldYQ12);
      if (((!movementArrived) || (modelRuntime->pitchTurnVelocityAngle16 != 0)) ||
         (modelRuntime->yawTurnVelocityAngle16 != 0)) {
        currentNode = modelRuntime->rootModelNode;
        ModelNodeRuntime_SmoothYawTowardTarget(currentNode,modelRuntime,0);
        ModelNodeRuntime_SmoothPitchTowardTarget(currentNode->childNodes[0],modelRuntime,0);
      }
    }
    else {
      currentNode = modelRuntime->rootModelNode;
      pitchNode = currentNode->childNodes[0];
      launchAngles = ShotDefinition_ComputeLaunchAngles
                        (aimZQ12,aimYQ12,aimXQ12,(pitchNode->worldTransform).translation.z,
                         (pitchNode->worldTransform).translation.y,
                         (pitchNode->worldTransform).translation.x,weaponDefinitionView->shotDefinition);
      relativeAngles = ModelNodeRuntime_ComputeRelativeDirectionAngle
                         (currentNode,launchAngles.elevationAngle,launchAngles.headingAngle);
      targetPitchAngle16 = relativeAngles.relativePitchAngle;
      if (ModelNodeRuntime_SmoothYawTowardTarget
                         (currentNode,modelRuntime,relativeAngles.relativeYawAngle)) {
        ModelNodeRuntime_SmoothPitchTowardTarget(pitchNode,modelRuntime,targetPitchAngle16);
      }
      else {
        pitchAimValue = ModelNodeRuntime_SmoothPitchTowardTarget
                           (pitchNode,modelRuntime,targetPitchAngle16);
        if (((pitchAimValue == targetPitchAngle16) &&
            (weaponDefinitionView = modelRuntime->modelDefinition, modelRuntime->sharedInterShotTicks == 0)) &&
           (callCarry = ArmyRuntimeCommand_UpdateTargetFollowingState
                              (aimZQ12,aimYQ12,aimXQ12,worldRuntime,(ModelRuntimeSlot *)modelRuntime)
           , !callCarry)) {
          currentNode = pitchNode->childNodes[0];
          attachmentNodeHeader = weaponDefinitionView->rootNode;
          nodeRuntimeFlags = &currentNode->runtimeFlags;
          *nodeRuntimeFlags = *nodeRuntimeFlags | 1;
          attachmentNodeHeader = (MdlSerializedNodeHeader *)
                                 ((MdlSerializedNodeHeader *)attachmentNodeHeader->childSerializedOffsets[0])->
                                 childSerializedOffsets[0];
          for (attachmentSelectorOrdinal = 0; attachmentSelectorOrdinal < ARMY_WEAPON_ATTACHMENT_COUNT;
              attachmentSelectorOrdinal++) {
            if (modelRuntime->attachmentReloadTicks[attachmentSelectorOrdinal] == 0) {
              modelRuntime->attachmentReloadTicks[attachmentSelectorOrdinal] =
                   weaponDefinitionView->attachmentReloadTicks;
              commandTargetArmy = modelRuntime->ownerArmyRuntime->commandTargetArmyRuntime;
              targetRuntimeReference = 0;
              if (commandTargetArmy != NULL) {
                targetRuntimeReference = (commandTargetArmy->modelRuntimeOrSavedOffset).savedIdOrOffset;
              }
              callCarry = ArmyRuntime_ResolveShotLaunchFromModelAttachment
                                (targetRuntimeReference,aimZQ12,aimYQ12,aimXQ12,
                                 attachmentSelectorOrdinal,weaponDefinitionView->shotDefinition,currentNode,attachmentNodeHeader,
                                 worldRuntime);
              if (!callCarry) {
                modelRuntime->sharedInterShotTicks = weaponDefinitionView->sharedInterShotTicks;
                ArmyRuntime_SetNonzeroActionVector
                          (launchAngles.headingAngle,weaponDefinitionView->postLaunchVector1Q12,
                           weaponDefinitionView->postLaunchVector0Q12,modelRuntime->ownerArmyRuntime);
                nodeMeshMask = &(modelRuntime->rootModelNode->childNodes[0]->childNodes[0]->modelPayload).
                          meshGroupMask;
                /* hides the fired projectile; SHL (not ROL) as in the original, so bits below it go too */
                *nodeMeshMask = *nodeMeshMask & -2 << ((uint8_t)attachmentSelectorOrdinal & 31);
                break;
              }
              /* launch failed: -1 runs out on the next tick, so the slot is ready again right away */
              modelRuntime->attachmentReloadTicks[attachmentSelectorOrdinal] = UINT32_MAX;
            }
          }
        }
      }
    }
  }
  ModelNodeRuntime_RebuildTransformsFromRoot(modelRuntime->rootModelNode);
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ModelRuntimeSlot *)modelRuntime);
  return;
}


/* Address: 0x00525130.
   Runtime update of the resource storage class (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[15],
   0x0051FCD4): moves the storage's fill-level child node between the heights at definition +0x24 and +0x28 in
   proportion to the owner faction's current Xenite (or Tritium when definition +0xC0 is 1) over its storage
   limit, then emits the damage-threshold effect.
*/
void ArmyRuntimeClass_UpdateTransformAndDamageEffect
          (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView *modelRuntime)

{
  ModelDefinition *classDefinition;
  ModelRuntimeNode *rootModelNodeRuntime;
  int factionRecordByteOffset;
  ModelRuntimeNode *childNode;

  classDefinition = modelRuntime->modelDefinition;
  /* byte offset of the faction record's xeniteCurrentQ4 / xeniteStorageLimitQ4 pair, or with +0x10 of
     tritiumCurrentQ4 / tritiumStorageLimitQ4 */
  factionRecordByteOffset = modelRuntime->ownerArmyRuntime->factionIndex * GAME_FACTION_RUNTIME_RECORD_BYTES;
  if (classDefinition->classParameterC0 == 1) {
    factionRecordByteOffset = factionRecordByteOffset + 16;
  }
  rootModelNodeRuntime = modelRuntime->rootModelNode;
  childNode = rootModelNodeRuntime->childNodes[0];
  if ((rootModelNodeRuntime->childCount != 0) && (childNode != NULL)) {
    (childNode->modelPayload).localTranslationZQ12 =
         (int)(((int64_t)(int)(classDefinition->runtimeValue28 - classDefinition->runtimeValue24) *
               (int64_t)
               *(int *)((uint8_t *)g_GameFactionRuntimeImage.records + factionRecordByteOffset)) /
              (int64_t)*(int *)((uint8_t *)g_GameFactionRuntimeImage.records + factionRecordByteOffset + 4)
              ) + classDefinition->runtimeValue24;
    childNode->runtimeFlags = childNode->runtimeFlags | 1;
    ModelNodeRuntime_RebuildTransformsFromRoot(rootModelNodeRuntime);
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ModelRuntimeSlot *)modelRuntime);
  return;
}


/* Address: 0x00527AC0.
   Runtime update of army class 4 (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[4], 0x0051FCA8):
   while the army is intact it runs its timed shot/effect emitters and animated sub-nodes (only during research
   when the definition's gate at +0x1C8 is set), then emits the damage-threshold effect.
*/

void ArmyRuntimeClass_UpdateTimedEffectsModelsAndDamage
          (WorldRuntimeContext *worldRuntime,ModelRuntimeTimedEffectsUpdateView *modelRuntime)

{
  if ((((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_INACTIVE_MASK) == 0) &&
     ((modelRuntime->modelDefinition->timedEffectsRequireStateBit40 == 0 ||
      (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_RESEARCHING) != 0)))) {
    ArmyRuntime_UpdateTimedShotAndEffectEmitters
              (worldRuntime,(ModelRuntimeUpdateView *)modelRuntime);
    ArmyRuntime_UpdateAnimatedModelSubnodes(worldRuntime,(ModelRuntimeUpdateView *)modelRuntime);
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ModelRuntimeSlot *)modelRuntime);
  return;
}


/* Address: 0x0052A2E0.
   Applies an impact's damage to a living army (health at +0x3C, capped at the definition's maximumHealth). When the health reaches zero the army is marked destroyed; a child passes the excess
   damage on to its parent army; a root army of class 0 with a zero +0x278 state only turns to the impact
   angle, any other root army is counted in its owner faction's relation counter C (group-A command classes:
   counter D).
*/
void ArmyRuntime_ApplyImpactDamageAndFinalizeState
          (AngleTurn32 impactAngle,DamageAmount32 damageAmount,ModelRuntimeSlot *modelRuntime)

{
  Q12 *healthField;
  FactionRelationCounter *relationCounter;
  int maxHealth;
  uint32_t classId;
  int healthOrOwnerIndex;
  bool rotateToImpact;
  ModelRuntimeNode *parentModelNode;
  
  (modelRuntime->classState).healthRegenerationDelayTicks = ARMY_DAMAGE_REGENERATION_DELAY_TICKS;
  if (0 < (int)modelRuntime->health) {
    maxHealth = modelRuntime->definitionOrSavedId.runtimeDefinition->maximumHealth;
    healthField = (Q12 *)&modelRuntime->health;
    healthOrOwnerIndex = *healthField;
    *healthField = *healthField - damageAmount;
    /* SUB / JLE: the new health is <= 0 */
    if (*healthField == 0 || SBORROW4(healthOrOwnerIndex,damageAmount) != *healthField < 0) {
      healthOrOwnerIndex = modelRuntime->health; /* <= 0; its negation is the excess damage */
      (modelRuntime->classState).stateFlags =
           (modelRuntime->classState).stateFlags | ARMY_RUNTIME_FLAG_DESTROYED;
      parentModelNode = modelRuntime->rootModelNodeOrSavedOffset.modelNode->parentNode;
      /* a destroyed model links to itself */
      modelRuntime->linkedModelRuntimeOrSavedOffset.modelRuntime = modelRuntime;
      modelRuntime->health = 0;
      if (parentModelNode == NULL) {
        /* The original tests ZF after IMUL EBX,[EDI+0xC],0x740 (0x0052A395 / JZ 0x0052A39C). ZF is undefined
           after IMUL on paper; measured on an AMD Zen 3 it is left unchanged, so it still holds the result of the
           CMP [+0x4C] / CMP [+0x278] tests: the counters are updated unless the army was turned to the impact.
           The C follows that. */
        rotateToImpact = false;
        if ((modelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_00) &&
           (rotateToImpact =
                 modelRuntime->definitionOrSavedId.runtimeDefinition->placementContactKindIndex == 0,
            rotateToImpact)) {
          (modelRuntime->rootModelNodeOrSavedOffset.modelNode->modelPayload).worldRotationAngle0 = impactAngle;
        }
        healthOrOwnerIndex = modelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex;
        if ((!rotateToImpact) &&
           (classId = modelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId,
           relationCounter = &g_GameFactionRuntimeImage.records[healthOrOwnerIndex].relationCounterC,
           *relationCounter = *relationCounter + 1,
           g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand[classId] ==
           ArmyRuntime_ClassCommandHandlerGroupA)) {
          /* armies of the group-A command class move from counter C to counter D */
          relationCounter = &g_GameFactionRuntimeImage.records[healthOrOwnerIndex].relationCounterD;
          *relationCounter = *relationCounter + 1;
          relationCounter = &g_GameFactionRuntimeImage.records[healthOrOwnerIndex].relationCounterC;
          *relationCounter = *relationCounter - 1;
        }
      }
      else {
        ArmyRuntime_ApplyDamageAndPropagateToParent(-healthOrOwnerIndex,(parentModelNode->runtimePayload).modelRuntime)
        ;
      }
    }
    else {
      /* SUB / JG: a negative damage (repair) never raises the health above maxHealth */
      healthOrOwnerIndex = maxHealth - modelRuntime->health;
      if (healthOrOwnerIndex == 0 || maxHealth < (int)modelRuntime->health) {
        modelRuntime->health = modelRuntime->health + healthOrOwnerIndex;
      }
    }
  }
}


/* Address: 0x0052A3E0.
   Same as ArmyRuntime_ApplyDamageAndPropagateToParent: subtracts damageAmount from a living army's health
   (+0x3C, a repair capped at the definition's maximumHealth); at zero the army is flagged
   destroyed and the excess damage goes to its parent army. The faction relation counters of the owner and of
   sourceFactionIndex that the original would update for a root army are never reached (see below). No caller or
   table slot referencing it was found in src/ or src/generated/image_data.c.
*/
void ArmyRuntime_ApplyDamageAndFactionRelationState(FactionRuntimeIndex sourceFactionIndex,DamageAmount32 damageAmount,
          ModelRuntimeSlot *modelRuntime)

{
  Q12 *healthField;
  int maxHealth;
  int healthOrDelta;
  ModelRuntimeNode *parentModelNode;

  (modelRuntime->classState).healthRegenerationDelayTicks = ARMY_DAMAGE_REGENERATION_DELAY_TICKS;
  if (0 < (int)modelRuntime->health) {
    maxHealth = modelRuntime->definitionOrSavedId.runtimeDefinition->maximumHealth;
    healthField = (Q12 *)&modelRuntime->health;
    healthOrDelta = *healthField;
    *healthField = *healthField - damageAmount;
    /* SUB / JLE: the new health is <= 0 */
    if (*healthField == 0 || SBORROW4(healthOrDelta,damageAmount) != *healthField < 0) {
      healthOrDelta = modelRuntime->health; /* <= 0; its negation is the excess damage */
      (modelRuntime->classState).stateFlags =
           (modelRuntime->classState).stateFlags | ARMY_RUNTIME_FLAG_DESTROYED;
      /* a destroyed model links to itself */
      modelRuntime->linkedModelRuntimeOrSavedOffset.modelRuntime = modelRuntime;
      parentModelNode = modelRuntime->rootModelNodeOrSavedOffset.modelNode->parentNode;
      modelRuntime->health = 0;
      if (parentModelNode != NULL) {
        ArmyRuntime_ApplyDamageAndPropagateToParent(-healthOrDelta,(parentModelNode->runtimePayload).modelRuntime)
        ;
      }
      /* Without a parent the original (0x0052A470) would count the army in the relation counters of its owner
         and of sourceFactionIndex, but behind JZ right after IMUL EBX,[EDI+0xC],0x740 (0x0052A47C). IMUL leaves
         ZF unchanged (measured on an AMD Zen 3) and ZF is still set from TEST EDX,EDX with EDX = parent = 0, so
         the jump is always taken and the update never runs: the C omits it, as in
         ArmyRuntime_ApplyDamageAndPropagateToParent. */
    }
    else {
      /* SUB / JG: a negative damage (repair) never raises the health above maxHealth */
      healthOrDelta = maxHealth - modelRuntime->health;
      if (healthOrDelta == 0 || maxHealth < (int)modelRuntime->health) {
        modelRuntime->health = modelRuntime->health + healthOrDelta;
      }
    }
  }
  return;
}


/* Address: 0x0052A640.
   Splits a shot impact between the hit army and the army it is mounted on: half (rounded down) goes to the
   hit army, the rest to the parent model node's army, or to the hit army again when it has no parent. Called
   directly by the shot impact handling in world/shots/maintenance.c.
*/
void ArmyRuntime_ApplyImpactDamageToRuntimeAndParent(AngleTurn32 impactAngle,FactionRuntimeIndex sourceFactionIndex,
          ImpactDamageValue32 impactValue,ModelRuntimeSlot *targetModelRuntime)

{
  ModelRuntimeNode *targetModelNodeRuntime;

  /* GameEntityRuntime_ApplyImpactDamageAndFactionRelationState gets the model runtime under its
     GameEntityRuntime parameter type */
  targetModelNodeRuntime = targetModelRuntime->rootModelNodeOrSavedOffset.modelNode;
  GameEntityRuntime_ApplyImpactDamageAndFactionRelationState
            (impactAngle,sourceFactionIndex,impactValue >> 1,(GameEntityRuntime *)targetModelRuntime)
  ;
  if (targetModelNodeRuntime->parentNode != NULL) {
    targetModelRuntime = (targetModelNodeRuntime->parentNode->runtimePayload).modelRuntime;
  }
  GameEntityRuntime_ApplyImpactDamageAndFactionRelationState
            (impactAngle,sourceFactionIndex,impactValue - (impactValue >> 1),
             (GameEntityRuntime *)targetModelRuntime);
  return;
}


/* Address: 0x0052B9D0.
   Checks whether the army's weapon can hit the target position; true (CF set) = blocked. Ballistic shots need a
   solvable arc whose elevation lies within the weapon's limits (definition +0x24 / +0x28) and no model in the way
   along the horizontal distance; fixed-range shots always pass; other shots need an elevation within the limits
   (unless guided), no terrain in front of the target (a ground shot without an entity target may land within
   0x400 of the aim point) and no model in the way, and must reach the target (its distance minus half its radius)
   within speed * (lifetime - 2/3 ramp - 1). A model in the way does not block when it is the command target or
   passes the owner test (commandState < 1: models of the own owner, otherwise those of other owners). Called
   directly by the AI combat target selection (gameplay/ai/combat.c) and gameplay/army/movement.c.
*/
bool ArmyWeaponRuntime_TestTargetLineOfFire(Q12 targetWorldZQ12,Q12 targetWorldYQ12,Q12 targetWorldXQ12,
          WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  ArmyWeaponDefinitionView *weaponDefinition;
  ShotDefinition *shotDefinition;
  int64_t discriminant;
  int deltaXOrScaledLength;
  int minAngleOwnerOrDistance;
  int deltaYOrSpeedSquared;
  AngleTurn32 azimuthAngle;
  int deltaZ;
  AngleTurn32 elevationAngle;
  uint32_t angleOrDistance;
  int maxAngleOrRange;
  uint32_t distanceDifference;
  FixedLengthAngle horizontalVector;
  bool modelHit;
  Q12 modelHitDistanceQ12;
  ModelRuntimeNode *hitModelNode;
  FixedLengthAzimuthElevation targetVector;
  GraphicsWorldCoordinateQ12 originZQ12;
  GraphicsWorldCoordinateQ12 originYQ12;
  GraphicsWorldCoordinateQ12 originXQ12;
  WorldOwnerRuntimeClassId requiredOwnerId;
  ModelRuntimeNode *excludedNode;
  GameEntityRuntime *hitEntity;
  GameEntityRuntime *ownOrTargetEntity;
  ModelRuntimeNode *originNode;
  
  /* modelRuntime is the weapon's model runtime; ownOrTargetEntity starts as its owning army */
  originNode = modelRuntime->rootModelNodeOrSavedOffset.modelNode;
  ownOrTargetEntity = modelRuntime->ownerArmyRuntimeOrSavedOffset.entityRuntime;
  weaponDefinition = (ArmyWeaponDefinitionView *)modelRuntime->definitionOrSavedId.runtimeDefinition;
  deltaXOrScaledLength = targetWorldXQ12 - (originNode->worldTransform).translation.x;
  minAngleOwnerOrDistance = weaponDefinition->minimumPitchAngle;
  maxAngleOrRange = weaponDefinition->maximumPitchAngle;
  deltaYOrSpeedSquared = targetWorldYQ12 - (originNode->worldTransform).translation.y;
  shotDefinition = weaponDefinition->shotDefinition;
  deltaZ = targetWorldZQ12 - (originNode->worldTransform).translation.z;
  if (shotDefinition->trajectoryMode == SHOT_TRAJECTORY_BALLISTIC) {
    horizontalVector = FixedMath_Vector2AngleAndLength(deltaYOrSpeedSquared,deltaXOrScaledLength);
    deltaYOrSpeedSquared = shotDefinition->launchSpeedQ12 * shotDefinition->launchSpeedQ12;
    deltaXOrScaledLength = horizontalVector.length * shotDefinition->ballisticDivisorQ12;
    discriminant = (int64_t)(deltaYOrSpeedSquared + shotDefinition->ballisticDivisorQ12 * deltaZ * -2) *
                   (int64_t)deltaYOrSpeedSquared -
                   (int64_t)deltaXOrScaledLength * (int64_t)deltaXOrScaledLength;
    if (discriminant < 0) {
      return true;
    }
    angleOrDistance = FIXED_UINT64_SQRT(discriminant);
    if ((-Q12_ONE < deltaZ) && (deltaZ < Q12_ONE)) {
      angleOrDistance = -angleOrDistance;
    }
    angleOrDistance = FixedMath_Atan2Angle16(deltaYOrSpeedSquared + angleOrDistance,deltaXOrScaledLength);
    if ((int)angleOrDistance < minAngleOwnerOrDistance) {
      return true;
    }
    if (maxAngleOrRange < (int)angleOrDistance) {
      return true;
    }
    originNode = modelRuntime->rootModelNodeOrSavedOffset.modelNode;
    modelHit = ModelRuntime_RaycastCandidateListNearest
                       (angleOrDistance,horizontalVector.angle & FIXED_ANGLE16_MASK,horizontalVector.length,
                        (originNode->worldTransform).translation.z,
                        (originNode->worldTransform).translation.y,
                        (originNode->worldTransform).translation.x,WORLD_OWNER_RUNTIME_MODEL,
                        (modelRuntime->ownerArmyRuntimeOrSavedOffset.entityRuntime->common).ownership.modelNode,
                        worldRuntime,&modelHitDistanceQ12,&hitModelNode)
    ;
    if (!modelHit) {
      return false;
    }
    /* As in the original (MOV EAX,[EDI+0x48] at 0x0052BC55, EDI = own model node): this tests the shooter's
       own entity, not the model that was hit (the other path uses the hit node from EDX). */
    ownOrTargetEntity = modelRuntime->ownerArmyRuntimeOrSavedOffset.entityRuntime;
    hitEntity = ((originNode->runtimePayload).modelRuntime)->ownerArmyRuntimeOrSavedOffset.entityRuntime;
    minAngleOwnerOrDistance = (ownOrTargetEntity->common).ownership.ownerIndex;
    if ((ownOrTargetEntity->common).commandState < 1) {
      if (minAngleOwnerOrDistance == (hitEntity->common).ownership.ownerIndex) {
        return false;
      }
    }
    else if (minAngleOwnerOrDistance != (hitEntity->common).ownership.ownerIndex) {
      return false;
    }
    if (hitEntity == (ownOrTargetEntity->common).commandTarget.targetEntity) {
      return false;
    }
    return true;
  }
  if (shotDefinition->trajectoryMode == SHOT_TRAJECTORY_FIXED_RANGE) {
    return false;
  }
  targetVector = FixedMath_VectorToAnglesAndLength(deltaZ,deltaYOrSpeedSquared,deltaXOrScaledLength);
  elevationAngle = targetVector.elevationAngle;
  azimuthAngle = targetVector.azimuthAngle;
  angleOrDistance = targetVector.lengthQ12;
  if (shotDefinition->guidanceTurnLimitAngle16 == 0) {
    if ((int)elevationAngle < minAngleOwnerOrDistance) {
      return true;
    }
    if (maxAngleOrRange < (int)elevationAngle) {
      return true;
    }
  }
  excludedNode = (ownOrTargetEntity->common).ownership.modelNode;
  requiredOwnerId = WORLD_OWNER_RUNTIME_MODEL;
  originXQ12 = (originNode->worldTransform).translation.x;
  maxAngleOrRange = shotDefinition->launchSpeedQ12 * (int)shotDefinition->projectileLifetimeTicks;
  originYQ12 = (originNode->worldTransform).translation.y;
  originZQ12 = (originNode->worldTransform).translation.z;
  /* only the distance matters: a miss reports FIELD_GRID_RAYCAST_MISS_DISTANCE */
  (void)FieldGrid_RaycastTerrainSurfaceDistance
                     (elevationAngle,azimuthAngle,maxAngleOrRange,(originNode->worldTransform).translation.z,
                      (originNode->worldTransform).translation.y,
                      (originNode->worldTransform).translation.x,worldRuntime->fieldGrid,
                      &minAngleOwnerOrDistance,NULL);
  modelHit = ModelRuntime_RaycastCandidateListNearest
                     (elevationAngle,azimuthAngle,maxAngleOrRange,originZQ12,originYQ12,originXQ12,
                      requiredOwnerId,excludedNode,worldRuntime,&modelHitDistanceQ12,&hitModelNode);
  if ((!modelHit) ? (minAngleOwnerOrDistance <= INT32_MAX - 1) :
      (minAngleOwnerOrDistance < modelHitDistanceQ12)) {
    /* The terrain is hit first: only a ground shot without an entity target landing within 0x400 of the
       aim distance is clear. */
    distanceDifference = minAngleOwnerOrDistance - angleOrDistance;
    if ((int)distanceDifference < 0) {
      distanceDifference = -distanceDifference;
    }
    if (((modelRuntime->ownerArmyRuntimeOrSavedOffset.entityRuntime->common).commandTarget.targetEntity == NULL) &&
        (distanceDifference < ARMY_GROUND_SHOT_LANDING_TOLERANCE_Q12 + 1)) {
      return false;
    }
    return true;
  }
  if (modelHit) {
    /* A model is hit first: blocked (CF set) when its owner fails the commandState owner test and it is not
       the command target; otherwise fall through to the range check. */
    ownOrTargetEntity = modelRuntime->ownerArmyRuntimeOrSavedOffset.entityRuntime;
    hitEntity = ((hitModelNode->runtimePayload).modelRuntime)->
                ownerArmyRuntimeOrSavedOffset.entityRuntime;
    minAngleOwnerOrDistance = (ownOrTargetEntity->common).ownership.ownerIndex;
    if (((ownOrTargetEntity->common).commandState < 1) ?
        (minAngleOwnerOrDistance != (hitEntity->common).ownership.ownerIndex) :
        (minAngleOwnerOrDistance == (hitEntity->common).ownership.ownerIndex)) {
      if (hitEntity != (ownOrTargetEntity->common).commandTarget.targetEntity) {
        return true;
      }
    }
  }
  /* range check: distance to the target minus half its radius (+0xDC of its class record) against
     speed * (lifetime - 2/3 of the ramp ticks - 1); ARMY_SHOT_RAMP_RANGE_FACTOR_Q12 is -2/3 in Q12 */
  ownOrTargetEntity = (modelRuntime->ownerArmyRuntimeOrSavedOffset.entityRuntime->common).commandTarget.targetEntity;
  if (ownOrTargetEntity != NULL) {
    angleOrDistance = (int)(angleOrDistance * 2 -
                 ((ModelRuntimeSlot *)(ownOrTargetEntity->common).ownership.definitionOrClassRecord)->
                 definitionOrSavedId.runtimeDefinition->footprintRadius
                 ) >> 1;
  }
  if (shotDefinition->launchSpeedQ12 *
      (((int)shotDefinition->trajectoryRampDurationTicks * ARMY_SHOT_RAMP_RANGE_FACTOR_Q12 >> Q12_SHIFT) +
       (int)shotDefinition->projectileLifetimeTicks - 1) < (int)angleOrDistance) {
    return true;
  }
  return false;
}


/* Address: 0x0052A200.
   Subtracts damageAmount from the health of a living army. When the health drops to zero or below the army is
   flagged destroyed and the excess damage is passed on to the army it is attached to (the parent model node),
   so destroying a mounted part also damages its carrier; a negative damage (repair) is capped at the maximum
   health.
*/
void ArmyRuntime_ApplyDamageAndPropagateToParent(DamageAmount32 damageAmount,ModelRuntimeSlot *modelRuntime)

{
  Q12 *healthField;
  int healthValue;
  int maxHealth;
  ModelRuntimeNode *parentModelNode;

  (modelRuntime->classState).healthRegenerationDelayTicks = ARMY_DAMAGE_REGENERATION_DELAY_TICKS;
  if (0 < (int)modelRuntime->health) {
    maxHealth = modelRuntime->definitionOrSavedId.runtimeDefinition->maximumHealth;
    healthField = (Q12 *)&modelRuntime->health;
    healthValue = *healthField;
    *healthField = *healthField - damageAmount;
    /* SUB / JLE: the new health is <= 0 */
    if (*healthField == 0 || SBORROW4(healthValue,damageAmount) != *healthField < 0) {
      healthValue = modelRuntime->health; /* <= 0; its negation is the excess damage */
      (modelRuntime->classState).stateFlags =
           (modelRuntime->classState).stateFlags | ARMY_RUNTIME_FLAG_DESTROYED;
      /* a destroyed model links to itself */
      modelRuntime->linkedModelRuntimeOrSavedOffset.modelRuntime = modelRuntime;
      parentModelNode = modelRuntime->rootModelNodeOrSavedOffset.modelNode->parentNode;
      modelRuntime->health = 0;
      if (parentModelNode != NULL) {
        ArmyRuntime_ApplyDamageAndPropagateToParent(-healthValue,(parentModelNode->runtimePayload).modelRuntime)
        ;
      }
      /* Without a parent the original goes on at 0x0052A290 to the owner faction's relationCounterC/D update of
         ArmyRuntime_ApplyImpactDamageAndFinalizeState, but guarded by JZ right after IMUL EBX,[EDI+0xC],0x740
         (0x0052A29C). IMUL leaves ZF unchanged (measured on an AMD Zen 3) and ZF is still set from
         TEST EDX,EDX with EDX = parent = 0, so the jump is always taken and the update never runs: the C
         omits it. */
    }
    else if (maxHealth < (int)modelRuntime->health) {
      /* a negative damage (repair) never raises the health above maxHealth */
      modelRuntime->health =
           modelRuntime->health + (maxHealth - modelRuntime->health);
    }
  }
  return;
}


/* Address: 0x00528200.
   Damage smoke/fire of a damaged army: while its health (+0x3C) is below the definition's threshold
   percentage (+0x250) of the class maximum (+0x60), it emits the definition's effect (+0x254) every
   +0x258 + random(+0x25C) ticks from the model's damage points (packed point key class 3), cycling through
   them, or from the model origin when it has none, with random orientation angles. Called directly
   at the end of nearly every army class runtime update (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes
   .runtimeUpdate slots, e.g. [4], [9], [15] in this file).
*/
void ArmyRuntime_EmitDamageThresholdEffect(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  ModelRuntimeNode *modelNodeRuntime;
  uint32_t randomOrPointX;
  ModelPackedPointRecord *localPointRecord;
  ModelDefinition *definition;
  uint32_t randomBits;
  uint32_t randomValue;
  uint32_t pointYQ12;
  uint32_t randomOffset;
  uint32_t pointZQ12;
  bool emitterPointFound;
  ModelWorldPoint transformedPoint;
  EffectDefinition *effectDefinition;

  if (((modelRuntime->classState).stateFlags & (ARMY_MODEL_STATE_DISMANTLING | ARMY_MODEL_STATE_DISMANTLED)) != 0)
  {
    return;
  }
  definition = modelRuntime->definitionOrSavedId.runtimeDefinition;
  if ((int)definition->maximumHealth < 1) {
    return;
  }
  if (definition->damageEffectHealthPercent <=
      ((int)modelRuntime->health * 100) / (int)definition->maximumHealth) {
    return;
  }
  if (0 < modelRuntime->damageEffectCooldownTicks) {
    modelRuntime->damageEffectCooldownTicks =
         modelRuntime->damageEffectCooldownTicks - g_InGameSimulationStepTicks;
    return;
  }
  randomOffset = 0;
  if (definition->damageEffectRandomTicks != 0) {
    randomOrPointX = g_RandomGeneratorState.next();
    randomOffset = randomOrPointX % definition->damageEffectRandomTicks;
  }
  modelNodeRuntime = modelRuntime->rootModelNodeOrSavedOffset.modelNode;
  modelRuntime->damageEffectCooldownTicks = randomOffset + definition->damageEffectIntervalTicks;
  emitterPointFound = ModelLookupTable_FindPackedPoint
                    (modelRuntime->damageEffectPointIndex,ARMY_MODEL_POINT_CLASS_DAMAGE_EMITTER,
                     (modelNodeRuntime->modelPayload).modelResource,&localPointRecord);
  if (!emitterPointFound) {
    /* Wrap around to the first emitter point. */
    modelRuntime->damageEffectPointIndex = -1;
    emitterPointFound = ModelLookupTable_FindPackedPoint
                      (0,ARMY_MODEL_POINT_CLASS_DAMAGE_EMITTER,(modelNodeRuntime->modelPayload).modelResource,
                       &localPointRecord);
  }
  if (!emitterPointFound) {
    /* No emitter point at all: use the model origin. */
    randomOrPointX = (modelNodeRuntime->worldTransform).translation.x;
    pointYQ12 = (modelNodeRuntime->worldTransform).translation.y;
    pointZQ12 = (modelNodeRuntime->worldTransform).translation.z;
  }
  else {
    transformedPoint = ModelNodeRuntime_TransformLocalPoint(localPointRecord,modelNodeRuntime);
    pointZQ12 = transformedPoint.zQ12;
    pointYQ12 = transformedPoint.yQ12;
    randomOrPointX = transformedPoint.xQ12;
    modelRuntime->damageEffectPointIndex++;
  }
  effectDefinition = definition->damageEffectDefinitionReference.definition;
  randomBits = g_RandomGeneratorState.next();
  randomOffset = randomBits & FIXED_ANGLE16_MASK;
  randomValue = g_RandomGeneratorState.next();
  EffectRuntimePool_CreateInstanceFromDefinition
            (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference, 0),randomBits >> 16,
             (randomValue & (FIXED_ANGLE16_EIGHTH_TURN - 1)) + (FIXED_ANGLE16_EIGHTH_TURN - 1),randomOffset,pointZQ12,pointYQ12,randomOrPointX,effectDefinition,
             worldRuntime);
  return;
}

