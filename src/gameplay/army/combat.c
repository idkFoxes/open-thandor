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
          (WorldRuntimeContext *worldRuntime,ModelRuntimeWeaponAimStateView200 *modelRuntime)

{
  ModelRuntimeFlags *nodeRuntimeFlags;
  uint32_t *remainingTicks;
  ModelMeshGroupMask *nodeMeshMask;
  ArmyWeaponDefinitionView68 *weaponDefinitionView;
  ModelRuntimeNode *pitchNode;
  MdlSerializedNodeHeader38 *attachmentNodeHeader;
  ArmyRuntimeSlot *commandTargetArmy;
  InGameSimulationStepBatchTicks stepTicks;
  Q12 aimXQ12;
  ShotRuntimeState14 targetRuntimeReference;
  Q12 aimYQ12;
  Q12 aimZQ12;
  AngleTurn32 targetPitchAngle16;
  SprAttachmentSelectorOrdinal attachmentSelectorOrdinal;
  bool callCarry;
  ShotLaunchAnglesEaxEdx8 launchAngles;
  ModelRelativeDirectionAnglesEaxEdx8 relativeAngles;
  AimSmoothResult smoothResult;
  MovementStepResult movementResult;
  WorldPositionResult aimPoint;
  GameEntityRuntime *ownerEntity;
  ModelRuntimeNode *currentNode;
  
  stepTicks = g_InGameSimulationStepTicks;
  currentNode = modelRuntime->rootModelNode->childNodes[0]->childNodes[0];
  if (((modelRuntime->classState).classStateEC & (ARMY_RUNTIME_FLAG_DESTROYED | 1)) == 0) {
    /* reload of attachment slot i done: show its projectile (mesh group bit i of the barrel node) */
    remainingTicks = modelRuntime->attachmentReloadTicks;
    *remainingTicks = *remainingTicks - g_InGameSimulationStepTicks;
    if ((int)*remainingTicks < 0) {
      modelRuntime->attachmentReloadTicks[0] = 0;
      nodeMeshMask = &(currentNode->modelPayload).meshGroupMask;
      *nodeMeshMask = *nodeMeshMask | 1;
    }
    remainingTicks = modelRuntime->attachmentReloadTicks + 1;
    *remainingTicks = *remainingTicks - stepTicks;
    if ((int)*remainingTicks < 0) {
      modelRuntime->attachmentReloadTicks[1] = 0;
      nodeMeshMask = &(currentNode->modelPayload).meshGroupMask;
      *nodeMeshMask = *nodeMeshMask | 2;
    }
    remainingTicks = modelRuntime->attachmentReloadTicks + 2;
    *remainingTicks = *remainingTicks - stepTicks;
    if ((int)*remainingTicks < 0) {
      modelRuntime->attachmentReloadTicks[2] = 0;
      nodeMeshMask = &(currentNode->modelPayload).meshGroupMask;
      *nodeMeshMask = *nodeMeshMask | 4;
    }
    remainingTicks = modelRuntime->attachmentReloadTicks + 3;
    *remainingTicks = *remainingTicks - stepTicks;
    if ((int)*remainingTicks < 0) {
      modelRuntime->attachmentReloadTicks[3] = 0;
      nodeMeshMask = &(currentNode->modelPayload).meshGroupMask;
      *nodeMeshMask = *nodeMeshMask | 8;
    }
    remainingTicks = modelRuntime->attachmentReloadTicks + 4;
    *remainingTicks = *remainingTicks - stepTicks;
    if ((int)*remainingTicks < 0) {
      modelRuntime->attachmentReloadTicks[4] = 0;
      nodeMeshMask = &(currentNode->modelPayload).meshGroupMask;
      *nodeMeshMask = *nodeMeshMask | 0x10;
    }
    remainingTicks = modelRuntime->attachmentReloadTicks + 5;
    *remainingTicks = *remainingTicks - stepTicks;
    if ((int)*remainingTicks < 0) {
      modelRuntime->attachmentReloadTicks[5] = 0;
      nodeMeshMask = &(currentNode->modelPayload).meshGroupMask;
      *nodeMeshMask = *nodeMeshMask | 0x20;
    }
    remainingTicks = modelRuntime->attachmentReloadTicks + 6;
    *remainingTicks = *remainingTicks - stepTicks;
    if ((int)*remainingTicks < 0) {
      modelRuntime->attachmentReloadTicks[6] = 0;
      nodeMeshMask = &(currentNode->modelPayload).meshGroupMask;
      *nodeMeshMask = *nodeMeshMask | 0x40;
    }
    remainingTicks = modelRuntime->attachmentReloadTicks + 7;
    *remainingTicks = *remainingTicks - stepTicks;
    if ((int)*remainingTicks < 0) {
      modelRuntime->attachmentReloadTicks[7] = 0;
      nodeMeshMask = &(currentNode->modelPayload).meshGroupMask;
      *nodeMeshMask = *nodeMeshMask | 0x80;
    }
    remainingTicks = &modelRuntime->sharedInterShotTicks;
    *remainingTicks = *remainingTicks - stepTicks;
    if ((int)*remainingTicks < 0) {
      modelRuntime->sharedInterShotTicks = 0;
    }
    weaponDefinitionView = modelRuntime->modelDefinition;
    ownerEntity = (GameEntityRuntime *)modelRuntime->ownerArmyRuntime;
    currentNode = modelRuntime->rootModelNode;
    aimPoint = ArmyRuntime_ResolveShotAimPoint
                       ((currentNode->worldTransform).translation.z,
                        (currentNode->worldTransform).translation.y,
                        (currentNode->worldTransform).translation.x,weaponDefinitionView->shotDefinition,
                        ownerEntity);
    aimZQ12 = aimPoint.worldZQ12;
    aimYQ12 = aimPoint.worldYQ12;
    aimXQ12 = aimPoint.worldXQ12;
    if (aimPoint.unresolved) {
      movementResult = ArmyRuntime_UpdateMovementAndWaypoints
                         (worldRuntime,(ArmyMovementRuntime *)ownerEntity);
      if (((!movementResult.arrived) || (modelRuntime->pitchTurnVelocityAngle16 != 0)) ||
         (modelRuntime->yawTurnVelocityAngle16 != 0)) {
        currentNode = modelRuntime->rootModelNode;
        ModelNodeRuntime_SmoothYawTowardTarget(currentNode,modelRuntime,0);
        ModelNodeRuntime_SmoothPitchTowardTarget(currentNode->childNodes[0],modelRuntime,0);
      }
    }
    else {
      currentNode = modelRuntime->rootModelNode;
      pitchNode = currentNode->childNodes[0];
      launchAngles = ShotDefinition_ComputeLaunchAnglesRegs
                        (aimZQ12,aimYQ12,aimXQ12,(pitchNode->worldTransform).translation.z,
                         (pitchNode->worldTransform).translation.y,
                         (pitchNode->worldTransform).translation.x,weaponDefinitionView->shotDefinition);
      relativeAngles = ModelNodeRuntime_ComputeRelativeDirectionAngle
                         (currentNode,launchAngles.elevationAngle,launchAngles.headingAngle);
      targetPitchAngle16 = relativeAngles.relativePitchAngle;
      smoothResult = ModelNodeRuntime_SmoothYawTowardTarget
                         (currentNode,modelRuntime,relativeAngles.relativeYawAngle);
      if (smoothResult.outsideTolerance) {
        ModelNodeRuntime_SmoothPitchTowardTarget(pitchNode,modelRuntime,targetPitchAngle16);
      }
      else {
        smoothResult = ModelNodeRuntime_SmoothPitchTowardTarget
                           (pitchNode,modelRuntime,targetPitchAngle16);
        if (((smoothResult.value == targetPitchAngle16) &&
            (weaponDefinitionView = modelRuntime->modelDefinition, modelRuntime->sharedInterShotTicks == 0)) &&
           (callCarry = ArmyRuntimeCommand_UpdateTargetFollowingState
                              (aimZQ12,aimYQ12,aimXQ12,worldRuntime,(ArmyRuntimeSlot *)modelRuntime)
           , !callCarry)) {
          currentNode = pitchNode->childNodes[0];
          attachmentNodeHeader = weaponDefinitionView->modelPointSource64;
          nodeRuntimeFlags = &currentNode->runtimeFlags;
          *nodeRuntimeFlags = *nodeRuntimeFlags | 1;
          attachmentNodeHeader = (MdlSerializedNodeHeader38 *)
                                 ((MdlSerializedNodeHeader38 *)attachmentNodeHeader->childSerializedOffsets[0])->
                                 childSerializedOffsets[0];
          attachmentSelectorOrdinal = 0;
          do {
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
                *nodeMeshMask = *nodeMeshMask & -2 << ((uint8_t)attachmentSelectorOrdinal & 0x1f);
                break;
              }
              /* launch failed: -1 runs out on the next tick, so the slot is ready again right away */
              modelRuntime->attachmentReloadTicks[attachmentSelectorOrdinal] = 0xffffffff;
            }
            attachmentSelectorOrdinal++;
          } while (attachmentSelectorOrdinal < 8);
        }
      }
    }
  }
  ModelNodeRuntime_RebuildTransformsFromRoot(modelRuntime->rootModelNode);
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  return;
}


/* Address: 0x00525130.
   Runtime update of the resource storage class (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[15],
   0x0051FCD4): moves the storage's fill-level child node between the heights at definition +0x24 and +0x28 in
   proportion to the owner faction's current Xenite (or Tritium when definition +0xC0 is 1) over its storage
   limit, then emits the damage-threshold effect.
*/
void ArmyRuntimeClass_UpdateTransformAndDamageEffect
          (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView200 *modelRuntime)

{
  ModelDefinitionRuntimeSemanticView280 *classDefinition;
  ModelRuntimeNode *rootModelNodeRuntime;
  int factionRecordByteOffset;
  ModelRuntimeNode *childNode;

  classDefinition = modelRuntime->modelDefinition;
  /* byte offset of the faction record's xeniteCurrentQ4 / xeniteStorageLimitQ4 pair, or with +0x10 of
     tritiumCurrentQ4 / tritiumStorageLimitQ4 (reserved78_87 - 0x78 is the record start) */
  factionRecordByteOffset = modelRuntime->ownerArmyRuntime->factionIndex * GAME_FACTION_RUNTIME_RECORD_BYTES;
  if (classDefinition->classParameterC0 == 1) {
    factionRecordByteOffset = factionRecordByteOffset + 0x10;
  }
  rootModelNodeRuntime = modelRuntime->rootModelNode;
  childNode = rootModelNodeRuntime->childNodes[0];
  if ((rootModelNodeRuntime->childCount != 0) && (childNode != NULL)) {
    (childNode->modelPayload).localTranslationZQ12 =
         (int)(((int64_t)(int)(classDefinition->runtimeValue28 - classDefinition->runtimeValue24) *
               (int64_t)
               *(int *)(g_GameFactionRuntimeImage.records[0].reserved78_87 + factionRecordByteOffset + -0x78)) /
              (int64_t)*(int *)(g_GameFactionRuntimeImage.records[0].reserved78_87 + factionRecordByteOffset + -0x74)
              ) + classDefinition->runtimeValue24;
    childNode->runtimeFlags = childNode->runtimeFlags | 1;
    ModelNodeRuntime_RebuildTransformsFromRoot(rootModelNodeRuntime);
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  return;
}


/* Address: 0x00527AC0.
   Runtime update of army class 4 (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[4], 0x0051FCA8):
   while the army is intact it runs its timed shot/effect emitters and animated sub-nodes (only during research
   when the definition's gate at +0x1C8 is set), then emits the damage-threshold effect.
*/

void ArmyRuntimeClass_UpdateTimedEffectsModelsAndDamage
          (WorldRuntimeContext *worldRuntime,ModelRuntimeTimedEffectsUpdateView200 *modelRuntime)

{
  if ((((modelRuntime->classState).classStateEC & (ARMY_RUNTIME_FLAG_DESTROYED | 1)) == 0) &&
     ((modelRuntime->modelDefinition->timedEffectsRequireRuntimeState40Gate1C8 == 0 ||
      (((modelRuntime->classState).classStateEC & ARMY_MODEL_STATE_RESEARCHING) != 0)))) {
    ArmyRuntime_UpdateTimedShotAndEffectEmitters
              (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
    ArmyRuntime_UpdateAnimatedModelSubnodes(worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  return;
}


/* Address: 0x0052A2E0.
   Applies an impact's damage to a living army (health in actionVector2Q12, capped at the class maximum at
   model runtime +0x60). When the health reaches zero the army is marked destroyed; a child passes the excess
   damage on to its parent army; a root army of class 0 with a zero +0x278 state only turns to the impact
   angle, any other root army is counted in its owner faction's relation counter C (group-A command classes:
   counter D).
*/
void ArmyRuntime_ApplyImpactDamageAndFinalizeState
          (AngleTurn32 impactAngle,DamageAmount32 damageAmount,ArmyRuntimeSlot *armyRuntime)

{
  Q12 *healthField;
  FactionRelationCounter *relationCounter;
  int maxHealth;
  uint32_t classId;
  int healthOrOwnerIndex;
  bool rotateToImpact;
  ModelRuntimeNode *parentModelNode;
  
  /* one dword 0x200 at +0xF8 (MOV [ESI+0xF8],0x200) */
  armyRuntime->reservedF8_FF[0] = 0;
  armyRuntime->reservedF8_FF[1] = 2;
  armyRuntime->reservedF8_FF[2] = 0;
  armyRuntime->reservedF8_FF[3] = 0;
  if (0 < armyRuntime->actionVector2Q12) {
    maxHealth = (((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->classLinkState).
            modelLinkOrState60.signedScalarState;
    healthField = &armyRuntime->actionVector2Q12;
    healthOrOwnerIndex = *healthField;
    *healthField = *healthField - damageAmount;
    /* SUB / JLE: the new health is <= 0 */
    if (*healthField == 0 || SBORROW4(healthOrOwnerIndex,damageAmount) != *healthField < 0) {
      healthOrOwnerIndex = armyRuntime->actionVector2Q12; /* <= 0; its negation is the excess damage */
      armyRuntime->runtimeFlags = armyRuntime->runtimeFlags | ARMY_RUNTIME_FLAG_DESTROYED;
      parentModelNode = armyRuntime->modelNodeRuntime->parentNode;
      armyRuntime->actionVector1Q12 = (Q12)armyRuntime;
      armyRuntime->actionVector2Q12 = 0;
      if (parentModelNode == NULL) {
        /* The original tests ZF after IMUL EBX,[EDI+0xC],0x740 (0x0052A395 / JZ 0x0052A39C). ZF is undefined
           after IMUL on paper; measured on an AMD Zen 3 it is left unchanged, so it still holds the result of the
           CMP [+0x4C] / CMP [+0x278] tests: the counters are updated unless the army was turned to the impact.
           The C follows that. */
        rotateToImpact = false;
        if ((((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C == 0) &&
           (rotateToImpact = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime[1].classLinkState.
                    classState78 == 0, rotateToImpact)) {
          (armyRuntime->modelNodeRuntime->modelPayload).worldRotationAngle0 = impactAngle;
        }
        healthOrOwnerIndex = (armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex;
        if ((!rotateToImpact) &&
           (classId = ((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C,
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
        ArmyRuntime_ApplyDamageAndPropagateToParent(-healthOrOwnerIndex,(parentModelNode->runtimePayload).armyRuntime)
        ;
      }
    }
    else {
      /* SUB / JG: a negative damage (repair) never raises the health above maxHealth */
      healthOrOwnerIndex = maxHealth - armyRuntime->actionVector2Q12;
      if (healthOrOwnerIndex == 0 || maxHealth < armyRuntime->actionVector2Q12) {
        armyRuntime->actionVector2Q12 = armyRuntime->actionVector2Q12 + healthOrOwnerIndex;
      }
    }
  }
}


/* Address: 0x0052A3E0.
   Same as ArmyRuntime_ApplyDamageAndPropagateToParent: subtracts damageAmount from a living army's health
   (actionVector2Q12, a repair capped at the class maximum at model runtime +0x60); at zero the army is flagged
   destroyed and the excess damage goes to its parent army. The faction relation counters of the owner and of
   sourceFactionIndex that the original would update for a root army are never reached (see below). No caller or
   table slot referencing it was found in src/ or src/generated/image_data.c.
*/
void ArmyRuntime_ApplyDamageAndFactionRelationState(FactionRuntimeIndex sourceFactionIndex,DamageAmount32 damageAmount,
          ArmyRuntimeSlot *armyRuntime)

{
  Q12 *healthField;
  int maxHealth;
  int healthOrDelta;
  ModelRuntimeNode *parentModelNode;

  /* one dword 0x200 at +0xF8 (MOV [ESI+0xF8],0x200) */
  armyRuntime->reservedF8_FF[0] = 0;
  armyRuntime->reservedF8_FF[1] = 2;
  armyRuntime->reservedF8_FF[2] = 0;
  armyRuntime->reservedF8_FF[3] = 0;
  if (0 < armyRuntime->actionVector2Q12) {
    maxHealth = (((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->classLinkState).
            modelLinkOrState60.signedScalarState;
    healthField = &armyRuntime->actionVector2Q12;
    healthOrDelta = *healthField;
    *healthField = *healthField - damageAmount;
    /* SUB / JLE: the new health is <= 0 */
    if (*healthField == 0 || SBORROW4(healthOrDelta,damageAmount) != *healthField < 0) {
      healthOrDelta = armyRuntime->actionVector2Q12; /* <= 0; its negation is the excess damage */
      armyRuntime->runtimeFlags = armyRuntime->runtimeFlags | ARMY_RUNTIME_FLAG_DESTROYED;
      armyRuntime->actionVector1Q12 = (Q12)armyRuntime;
      parentModelNode = armyRuntime->modelNodeRuntime->parentNode;
      armyRuntime->actionVector2Q12 = 0;
      if (parentModelNode != NULL) {
        ArmyRuntime_ApplyDamageAndPropagateToParent(-healthOrDelta,(parentModelNode->runtimePayload).armyRuntime)
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
      healthOrDelta = maxHealth - armyRuntime->actionVector2Q12;
      if (healthOrDelta == 0 || maxHealth < armyRuntime->actionVector2Q12) {
        armyRuntime->actionVector2Q12 = armyRuntime->actionVector2Q12 + healthOrDelta;
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
          ImpactDamageValue32 impactValue,ArmyRuntimeSlot *targetArmyRuntime)

{
  ModelRuntimeNode *targetModelNodeRuntime;

  targetModelNodeRuntime = targetArmyRuntime->modelNodeRuntime;
  GameEntityRuntime_ApplyImpactDamageAndFactionRelationState
            (impactAngle,sourceFactionIndex,impactValue >> 1,(GameEntityRuntime *)targetArmyRuntime)
  ;
  if (targetModelNodeRuntime->parentNode != NULL) {
    targetArmyRuntime =
         (ArmyRuntimeSlot *)(targetModelNodeRuntime->parentNode->runtimePayload).modelRuntime;
  }
  GameEntityRuntime_ApplyImpactDamageAndFactionRelationState
            (impactAngle,sourceFactionIndex,impactValue - (impactValue >> 1),
             (GameEntityRuntime *)targetArmyRuntime);
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
          WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ModelRuntimeSlot *weaponDefinition;
  int *shotDefinitionWords;
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
  FixedLengthAngleEaxEdx8 horizontalVector;
  TerrainRaycastResult terrainHit;
  ModelRaycastResult modelHit;
  FixedLengthAnglesEaxEcxEdx12 targetVector;
  GraphicsWorldCoordinateQ12 originZQ12;
  GraphicsWorldCoordinateQ12 originYQ12;
  GraphicsWorldCoordinateQ12 originXQ12;
  WorldOwnerRuntimeClassId requiredOwnerId;
  ModelRuntimeNode *excludedNode;
  GameEntityRuntime *hitEntity;
  GameEntityRuntime *ownOrTargetEntity;
  ModelRuntimeNode *originNode;
  
  originNode = armyRuntime->modelNodeRuntime;
  ownOrTargetEntity = armyRuntime->linkedEntityRuntime;
  /* the army's weapon definition (the "army" is the weapon's model runtime, whose first dword is the
     ArmyWeaponDefinitionView68). ShotDefinition dwords: [0] trajectoryMode, [3] launchSpeedQ12, [0x34] projectileLifetimeTicks (+0xD0),
     [0x37] ballisticDivisorQ12 (+0xDC), [0x9C] trajectoryRampDurationTicks (+0x270),
     [0xA4] guidanceTurnLimitAngle16 (+0x290) */
  weaponDefinition = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  deltaXOrScaledLength = targetWorldXQ12 - (originNode->worldTransform).translation.x;
  minAngleOwnerOrDistance = ((ArmyWeaponDefinitionView68 *)weaponDefinition)->minimumPitchAngle24;
  maxAngleOrRange = ((ArmyWeaponDefinitionView68 *)weaponDefinition)->maximumPitchAngle28;
  deltaYOrSpeedSquared = targetWorldYQ12 - (originNode->worldTransform).translation.y;
  shotDefinitionWords = (int *)((ArmyWeaponDefinitionView68 *)weaponDefinition)->shotDefinition;
  deltaZ = targetWorldZQ12 - (originNode->worldTransform).translation.z;
  if (*shotDefinitionWords == 1) {
    horizontalVector = FixedMath_Vector2AngleAndLengthRegs(deltaYOrSpeedSquared,deltaXOrScaledLength);
    deltaYOrSpeedSquared = shotDefinitionWords[3] * shotDefinitionWords[3];
    deltaXOrScaledLength = horizontalVector.length * shotDefinitionWords[0x37];
    discriminant = (int64_t)(deltaYOrSpeedSquared + shotDefinitionWords[0x37] * deltaZ * -2) * (int64_t)deltaYOrSpeedSquared -
            (int64_t)deltaXOrScaledLength * (int64_t)deltaXOrScaledLength;
    if (discriminant < 0) {
      return true;
    }
    angleOrDistance = FixedMath_UInt64Sqrt((UInt64Half32)((uint64_t)discriminant >> 0x20),(UInt64Half32)discriminant);
    if ((-0x1000 < deltaZ) && (deltaZ < 0x1000)) {
      angleOrDistance = -angleOrDistance;
    }
    angleOrDistance = FixedMath_Atan2Angle16(deltaYOrSpeedSquared + angleOrDistance,deltaXOrScaledLength);
    if ((int)angleOrDistance < minAngleOwnerOrDistance) {
      return true;
    }
    if (maxAngleOrRange < (int)angleOrDistance) {
      return true;
    }
    originNode = armyRuntime->modelNodeRuntime;
    modelHit = ModelRuntime_RaycastCandidateListNearest
                       (angleOrDistance,horizontalVector.angle & 0xffff,horizontalVector.length,
                        (originNode->worldTransform).translation.z,
                        (originNode->worldTransform).translation.y,
                        (originNode->worldTransform).translation.x,WORLD_OWNER_RUNTIME_MODEL,
                        (armyRuntime->linkedEntityRuntime->common).ownership.modelNode,worldRuntime)
    ;
    if (!modelHit.hit) {
      return false;
    }
    /* As in the original (MOV EAX,[EDI+0x48] at 0x0052BC55, EDI = own model node): this tests the shooter's
       own entity, not the model that was hit (the other path uses the hit node from EDX). */
    ownOrTargetEntity = armyRuntime->linkedEntityRuntime;
    hitEntity = ((originNode->runtimePayload).armyRuntime)->linkedEntityRuntime;
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
  if (*shotDefinitionWords == 2) {
    return false;
  }
  targetVector = FixedMath_VectorToAnglesAndLength3Regs(deltaZ,deltaYOrSpeedSquared,deltaXOrScaledLength);
  elevationAngle = targetVector.elevationAngle;
  azimuthAngle = targetVector.azimuthAngle;
  angleOrDistance = targetVector.lengthQ12;
  if (shotDefinitionWords[0xa4] == 0) {
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
  maxAngleOrRange = shotDefinitionWords[3] * shotDefinitionWords[0x34];
  originYQ12 = (originNode->worldTransform).translation.y;
  originZQ12 = (originNode->worldTransform).translation.z;
  terrainHit = FieldGrid_RaycastTerrainSurfaceDistance
                     (elevationAngle,azimuthAngle,maxAngleOrRange,(originNode->worldTransform).translation.z,
                      (originNode->worldTransform).translation.y,
                      (originNode->worldTransform).translation.x,worldRuntime->fieldGrid);
  minAngleOwnerOrDistance = terrainHit.distanceQ12;
  modelHit = ModelRuntime_RaycastCandidateListNearest
                     (elevationAngle,azimuthAngle,maxAngleOrRange,originZQ12,originYQ12,originXQ12,
                      requiredOwnerId,excludedNode,worldRuntime);
  if ((!modelHit.hit) ? (minAngleOwnerOrDistance <= 0x7ffffffe) :
      (minAngleOwnerOrDistance < modelHit.nearestDistanceQ12)) {
    /* The terrain is hit first: only a ground shot without an entity target landing within 0x400 of the
       aim distance is clear. */
    distanceDifference = minAngleOwnerOrDistance - angleOrDistance;
    if ((int)distanceDifference < 0) {
      distanceDifference = -distanceDifference;
    }
    if (((armyRuntime->linkedEntityRuntime->common).commandTarget.targetEntity ==
         NULL) && (distanceDifference < 0x401)) {
      return false;
    }
    return true;
  }
  if (modelHit.hit) {
    /* A model is hit first: blocked (CF set) when its owner fails the commandState owner test and it is not
       the command target; otherwise fall through to the range check. */
    ownOrTargetEntity = armyRuntime->linkedEntityRuntime;
    hitEntity =
         (((modelHit.nearestNodeOrScratch.nearestModelNode)->runtimePayload).armyRuntime)->linkedEntityRuntime;
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
     speed * (lifetime - 2/3 of the ramp ticks - 1); -0xAAA / 0x1000 = -2/3 in Q12 */
  ownOrTargetEntity = (armyRuntime->linkedEntityRuntime->common).commandTarget.targetEntity;
  if (ownOrTargetEntity != NULL) {
    angleOrDistance = (int)(angleOrDistance * 2 -
                 ((ModelRuntimeSlot *)(ownOrTargetEntity->common).ownership.definitionOrClassRecord)->
                 definitionOrSavedId.runtimeDefinition->placementRadiusOrClearanceDC
                 ) >> 1;
  }
  if (shotDefinitionWords[3] * ((shotDefinitionWords[0x9c] * -0xaaa >> 0xc) + shotDefinitionWords[0x34] - 1) < (int)angleOrDistance) {
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
void ArmyRuntime_ApplyDamageAndPropagateToParent(DamageAmount32 damageAmount,ArmyRuntimeSlot *armyRuntime)

{
  Q12 *healthField;
  int healthValue;
  int maxHealth;
  ModelRuntimeNode *parentModelNode;

  /* one dword 0x200 at +0xF8 (MOV [ESI+0xF8],0x200) */
  armyRuntime->reservedF8_FF[0] = 0;
  armyRuntime->reservedF8_FF[1] = 2;
  armyRuntime->reservedF8_FF[2] = 0;
  armyRuntime->reservedF8_FF[3] = 0;
  if (0 < armyRuntime->actionVector2Q12) {
    maxHealth = (((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->classLinkState).
            modelLinkOrState60.signedScalarState;
    healthField = &armyRuntime->actionVector2Q12;
    healthValue = *healthField;
    *healthField = *healthField - damageAmount;
    /* SUB / JLE: the new health is <= 0 */
    if (*healthField == 0 || SBORROW4(healthValue,damageAmount) != *healthField < 0) {
      healthValue = armyRuntime->actionVector2Q12; /* <= 0; its negation is the excess damage */
      armyRuntime->runtimeFlags = armyRuntime->runtimeFlags | ARMY_RUNTIME_FLAG_DESTROYED;
      armyRuntime->actionVector1Q12 = (Q12)armyRuntime;
      parentModelNode = armyRuntime->modelNodeRuntime->parentNode;
      armyRuntime->actionVector2Q12 = 0;
      if (parentModelNode != NULL) {
        ArmyRuntime_ApplyDamageAndPropagateToParent(-healthValue,(parentModelNode->runtimePayload).armyRuntime)
        ;
      }
      /* Without a parent the original goes on at 0x0052A290 to the owner faction's relationCounterC/D update of
         ArmyRuntime_ApplyImpactDamageAndFinalizeState, but guarded by JZ right after IMUL EBX,[EDI+0xC],0x740
         (0x0052A29C). IMUL leaves ZF unchanged (measured on an AMD Zen 3) and ZF is still set from
         TEST EDX,EDX with EDX = parent = 0, so the jump is always taken and the update never runs: the C
         omits it. */
    }
    else if (maxHealth < armyRuntime->actionVector2Q12) {
      /* a negative damage (repair) never raises the health above maxHealth */
      armyRuntime->actionVector2Q12 =
           armyRuntime->actionVector2Q12 + (maxHealth - armyRuntime->actionVector2Q12);
    }
  }
  return;
}


/* Address: 0x00528200.
   Damage smoke/fire of a damaged army: while its health (actionVector2Q12) is below the definition's threshold
   percentage (+0x250) of the class maximum (+0x60), it emits the definition's effect (+0x254) every
   +0x258 + random(+0x25C) ticks from the model's damage points (packed point key class 3), cycling through
   them, or from the model origin when it has none, with random orientation angles. Called directly
   at the end of nearly every army class runtime update (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes
   .runtimeUpdate slots, e.g. [4], [9], [15] in this file).
*/
void ArmyRuntime_EmitDamageThresholdEffect(WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ModelRuntimeNode *modelNodeRuntime;
  uint32_t randomOrPointX;
  ModelPackedPointRecord *localPointRecord;
  uint32_t definitionOrRandom;
  uint32_t randomValue;
  uint32_t pointYQ12;
  uint32_t randomOffset;
  uint32_t pointZQ12;
  ModelLookupEntryResult lookupResult;
  ModelWorldPoint transformedPoint;
  EffectDefinition *effectDefinition;

  /* selectionMetric3 (+0x114) is the emission cooldown, selectionMetric4 (+0x118) the next damage point */
  if ((armyRuntime->runtimeFlags & (ARMY_MODEL_STATE_DISMANTLING | 0x200)) != 0) {
    return;
  }
  definitionOrRandom = (armyRuntime->modelRuntimeOrSavedOffset).savedIdOrOffset;
  if ((int)((ModelDefinitionRuntimeSemanticView280 *)definitionOrRandom)->runtimeValue60 < 1) {
    return;
  }
  if (((ModelDefinitionRuntimeSemanticView280 *)definitionOrRandom)->damageEffectHealthPercent250 <=
      (armyRuntime->actionVector2Q12 * 100) / (int)((ModelDefinitionRuntimeSemanticView280 *)definitionOrRandom)->runtimeValue60) {
    return;
  }
  if (0 < armyRuntime->selectionMetric3) {
    armyRuntime->selectionMetric3 = armyRuntime->selectionMetric3 - g_InGameSimulationStepTicks;
    return;
  }
  randomOffset = 0;
  if (((ModelDefinitionRuntimeSemanticView280 *)definitionOrRandom)->damageEffectRandomTicks25C != 0) {
    randomOrPointX = g_RandomGeneratorState.next();
    randomOffset = randomOrPointX % ((ModelDefinitionRuntimeSemanticView280 *)definitionOrRandom)->damageEffectRandomTicks25C;
  }
  modelNodeRuntime = armyRuntime->modelNodeRuntime;
  armyRuntime->selectionMetric3 = randomOffset + ((ModelDefinitionRuntimeSemanticView280 *)definitionOrRandom)->damageEffectIntervalTicks258;
  lookupResult = ModelLookupTable_ContainsPackedKey
                    (armyRuntime->selectionMetric4,3,(modelNodeRuntime->modelPayload).modelResource)
  ;
  localPointRecord = lookupResult.entry;
  if (lookupResult.notFound) {
    /* Wrap around to the first emitter point. */
    armyRuntime->selectionMetric4 = -1;
    lookupResult = ModelLookupTable_ContainsPackedKey(0,3,(modelNodeRuntime->modelPayload).modelResource)
    ;
    localPointRecord = lookupResult.entry;
  }
  if (lookupResult.notFound) {
    /* No emitter point at all: use the model origin. */
    randomOrPointX = (modelNodeRuntime->worldTransform).translation.x;
    pointYQ12 = (modelNodeRuntime->worldTransform).translation.y;
    pointZQ12 = (modelNodeRuntime->worldTransform).translation.z;
  }
  else {
    transformedPoint = ModelNodeRuntime_TransformLocalPointRegs(localPointRecord,modelNodeRuntime);
    pointZQ12 = transformedPoint.zQ12;
    pointYQ12 = transformedPoint.yQ12;
    randomOrPointX = transformedPoint.xQ12;
    armyRuntime->selectionMetric4++;
  }
  effectDefinition = ((ModelDefinitionRuntimeSemanticView280 *)definitionOrRandom)->effectDefinitionReference254.definition;
  definitionOrRandom = g_RandomGeneratorState.next();
  randomOffset = definitionOrRandom & 0xffff;
  randomValue = g_RandomGeneratorState.next();
  EffectRuntimePool_CreateInstanceFromDefinition
            (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference4, 0x0),definitionOrRandom >> 0x10,
             (randomValue & 0x1fff) + 0x1fff,randomOffset,pointZQ12,pointYQ12,randomOrPointX,effectDefinition,worldRuntime
            );
  return;
}

