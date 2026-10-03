/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/combat.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/army/combat.h>
#include <thandor/thandor.h>

/* Implementation ownership: gameplay/army/combat. */

/* Counts down the reload timers of the eight launch attachments; a slot whose reload is done shows its
   projectile again (mesh group bit i of the barrel node). Then counts down the shared inter-shot timer. */
static void ArmyWeaponRuntime_CountDownReloadTimers
          (ModelRuntimeWeaponAimStateView *modelRuntime,ModelRuntimeNode *barrelNode,
           InGameSimulationStepBatchTicks stepTicks)

{
  int attachmentSlot;

  for (attachmentSlot = 0; attachmentSlot < ARMY_WEAPON_ATTACHMENT_COUNT; attachmentSlot++) {
    modelRuntime->attachmentReloadTicks[attachmentSlot] =
         modelRuntime->attachmentReloadTicks[attachmentSlot] - stepTicks;
    if ((int)modelRuntime->attachmentReloadTicks[attachmentSlot] < 0) {
      modelRuntime->attachmentReloadTicks[attachmentSlot] = 0;
      (barrelNode->modelPayload).meshGroupMask =
           (barrelNode->modelPayload).meshGroupMask | ARMY_WEAPON_ATTACHMENT_MESH_BIT(attachmentSlot);
    }
  }
  modelRuntime->sharedInterShotTicks = modelRuntime->sharedInterShotTicks - stepTicks;
  if ((int)modelRuntime->sharedInterShotTicks < 0) {
    modelRuntime->sharedInterShotTicks = 0;
  }
}


/* Fires from the first loaded launch attachment whose launch succeeds: arms its reload timer and the shared
   inter-shot timer, applies the post-launch vector to the owner army and hides the fired projectile. An
   attachment whose launch fails is marked ready again for the next tick and the next one is tried. */
static void ArmyWeaponRuntime_FireFromFirstLoadedAttachment
          (WorldRuntimeContext *worldRuntime,ModelRuntimeWeaponAimStateView *modelRuntime,
           ArmyWeaponDefinitionView *weaponDefinitionView,ModelRuntimeNode *pitchNode,Q12 aimZQ12,Q12 aimYQ12,
           Q12 aimXQ12,AngleTurn32 launchHeadingAngle)

{
  ModelRuntimeNode *launchNode;
  MdlSerializedNodeHeader *attachmentNodeHeader;
  ArmyRuntimeSlot *commandTargetArmy;
  ShotTargetModelReference targetRuntimeReference;
  SprAttachmentSelectorOrdinal attachmentSelectorOrdinal;
  Bool8 launchFailed;
  ModelMeshGroupMask *barrelMeshMask;

  launchNode = pitchNode->childNodes[0];
  launchNode->runtimeFlags = launchNode->runtimeFlags | 1;
  attachmentNodeHeader = (MdlSerializedNodeHeader *)
                         ((MdlSerializedNodeHeader *)weaponDefinitionView->rootNode->childSerializedOffsets[0])->
                         childSerializedOffsets[0];
  for (attachmentSelectorOrdinal = 0; attachmentSelectorOrdinal < ARMY_WEAPON_ATTACHMENT_COUNT;
      attachmentSelectorOrdinal++) {
    if (modelRuntime->attachmentReloadTicks[attachmentSelectorOrdinal] != 0) {
      continue;
    }
    modelRuntime->attachmentReloadTicks[attachmentSelectorOrdinal] = weaponDefinitionView->attachmentReloadTicks;
    commandTargetArmy = modelRuntime->ownerArmyRuntime->commandTargetArmyRuntime;
    targetRuntimeReference = 0;
    if (commandTargetArmy != NULL) {
      targetRuntimeReference = (commandTargetArmy->modelRuntimeOrSavedOffset).savedIdOrOffset;
    }
    launchFailed = ArmyRuntime_ResolveShotLaunchFromModelAttachment
                             (targetRuntimeReference,aimZQ12,aimYQ12,aimXQ12,attachmentSelectorOrdinal,
                              weaponDefinitionView->shotDefinition,launchNode,attachmentNodeHeader,worldRuntime);
    if (!launchFailed) {
      modelRuntime->sharedInterShotTicks = weaponDefinitionView->sharedInterShotTicks;
      ArmyRuntime_SetNonzeroActionVector
                (launchHeadingAngle,weaponDefinitionView->postLaunchVector1Q12,
                 weaponDefinitionView->postLaunchVector0Q12,modelRuntime->ownerArmyRuntime);
      barrelMeshMask = &(modelRuntime->rootModelNode->childNodes[0]->childNodes[0]->modelPayload).meshGroupMask;
      /* hides the fired projectile; a plain shift (not a rotate) as in the original, so bits below it go too */
      *barrelMeshMask = *barrelMeshMask & -2 << ((uint8_t)attachmentSelectorOrdinal & 31);
      return;
    }
    /* launch failed: -1 runs out on the next tick, so the slot is ready again right away */
    modelRuntime->attachmentReloadTicks[attachmentSelectorOrdinal] = UINT32_MAX;
  }
}


/* Runtime update of the turret-weapon class (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[9]).
   Counts down the reload timers of the eight launch attachments (showing a slot's projectile mesh
   bit again when it is loaded) and the shared inter-shot timer, resolves the aim point of the current target and
   turns the turret (yaw on the root node, pitch on its first child) toward the launch angles; without a target it
   moves and returns the turret to rest. Once yaw and pitch are on target, the inter-shot timer has run out and
   the target-following check passes, it fires from the first loaded attachment. Ends with the damage-threshold
   effect.
*/

void ArmyRuntimeWeapon_UpdateTargetAimAndFireAttachments
          (WorldRuntimeContext *worldRuntime,ModelRuntimeWeaponAimStateView *modelRuntime)

{
  ArmyWeaponDefinitionView *weaponDefinitionView;
  ModelRuntimeNode *rootNode;
  ModelRuntimeNode *pitchNode;
  ModelRuntimeNode *barrelNode;
  Q12 aimXQ12;
  Q12 aimYQ12;
  Q12 aimZQ12;
  AngleTurn32 targetPitchAngle16;
  Bool8 targetFollowingFailed;
  ShotLaunchAngles launchAngles;
  ModelRelativeDirectionAngles relativeAngles;
  uint32_t pitchAimValue;
  Bool8 movementArrived;
  Q12 steerWorldXQ12; /* unused here */
  Q12 steerWorldYQ12; /* unused here */
  GraphicsFixedVec3 aimPoint;
  Bool8 aimPointFound;
  GameEntityRuntime *ownerEntity;

  barrelNode = modelRuntime->rootModelNode->childNodes[0]->childNodes[0];
  if (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_INACTIVE_MASK) == 0) {
    ArmyWeaponRuntime_CountDownReloadTimers(modelRuntime,barrelNode,g_InGameSimulationStepTicks);
    weaponDefinitionView = modelRuntime->modelDefinition;
    ownerEntity = (GameEntityRuntime *)modelRuntime->ownerArmyRuntime;
    rootNode = modelRuntime->rootModelNode;
    aimPointFound = ArmyRuntime_ResolveShotAimPoint
                       ((rootNode->worldTransform).translation.z,
                        (rootNode->worldTransform).translation.y,
                        (rootNode->worldTransform).translation.x,weaponDefinitionView->shotDefinition,
                        ownerEntity,&aimPoint);
    aimZQ12 = aimPoint.z;
    aimYQ12 = aimPoint.y;
    aimXQ12 = aimPoint.x;
    if (!aimPointFound) {
      /* no target: move, and turn the turret back to rest while moving or still turning */
      movementArrived = ArmyRuntime_UpdateMovementAndWaypoints
                         (worldRuntime,(ArmyMovementRuntime *)ownerEntity,&steerWorldXQ12,&steerWorldYQ12);
      if (((!movementArrived) || (modelRuntime->pitchTurnVelocityAngle16 != 0)) ||
         (modelRuntime->yawTurnVelocityAngle16 != 0)) {
        rootNode = modelRuntime->rootModelNode;
        ModelNodeRuntime_SmoothYawTowardTarget(rootNode,modelRuntime,0);
        ModelNodeRuntime_SmoothPitchTowardTarget(rootNode->childNodes[0],modelRuntime,0);
      }
    }
    else {
      rootNode = modelRuntime->rootModelNode;
      pitchNode = rootNode->childNodes[0];
      launchAngles = ShotDefinition_ComputeLaunchAngles
                        (aimZQ12,aimYQ12,aimXQ12,(pitchNode->worldTransform).translation.z,
                         (pitchNode->worldTransform).translation.y,
                         (pitchNode->worldTransform).translation.x,weaponDefinitionView->shotDefinition);
      relativeAngles = ModelNodeRuntime_ComputeRelativeDirectionAngle
                         (rootNode,launchAngles.elevationAngle,launchAngles.headingAngle);
      targetPitchAngle16 = relativeAngles.relativePitchAngle;
      if (ModelNodeRuntime_SmoothYawTowardTarget
                         (rootNode,modelRuntime,relativeAngles.relativeYawAngle)) {
        /* yaw still turning */
        ModelNodeRuntime_SmoothPitchTowardTarget(pitchNode,modelRuntime,targetPitchAngle16);
      }
      else {
        pitchAimValue = ModelNodeRuntime_SmoothPitchTowardTarget
                           (pitchNode,modelRuntime,targetPitchAngle16);
        if (pitchAimValue == targetPitchAngle16) {
          weaponDefinitionView = modelRuntime->modelDefinition;
          if (modelRuntime->sharedInterShotTicks == 0) {
            targetFollowingFailed = ArmyRuntimeCommand_UpdateTargetFollowingState
                                      (aimZQ12,aimYQ12,aimXQ12,worldRuntime,(ModelRuntimeSlot *)modelRuntime);
            if (!targetFollowingFailed) {
              ArmyWeaponRuntime_FireFromFirstLoadedAttachment
                        (worldRuntime,modelRuntime,weaponDefinitionView,pitchNode,aimZQ12,aimYQ12,aimXQ12,
                         launchAngles.headingAngle);
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


/* Runtime update of the resource storage class (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[15]):
   moves the storage's fill-level child node between the heights definition runtimeValue24 and runtimeValue28 in
   proportion to the owner faction's current Xenite (or Tritium when definition classParameterC0 is 1) over its
   storage limit, then emits the damage-threshold effect.
*/
void ArmyRuntimeClass_UpdateTransformAndDamageEffect
          (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView *modelRuntime)

{
  ModelDefinition *classDefinition;
  ModelRuntimeNode *rootModelNodeRuntime;
  int factionRecordByteOffset;
  ModelRuntimeNode *childNode;

  classDefinition = modelRuntime->modelDefinition;
  /* byte offset of the faction record's xeniteCurrentQ4 / xeniteStorageLimitQ4 pair, or 16 bytes further that
     of tritiumCurrentQ4 / tritiumStorageLimitQ4 */
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


/* Runtime update of army class 4 (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[4]):
   while the army is intact it runs its timed shot/effect emitters and animated sub-nodes (only during research
   when the definition's gate timedEffectsRequireStateBit40 is set), then emits the damage-threshold effect.
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


/* Applies an impact's damage to a living army (health, capped at the definition's maximumHealth). When the
   health reaches zero the army is marked destroyed; a child passes the excess damage on to its parent army; a
   root army of class 0 with a zero placementContactKindIndex only turns to the impact
   angle, any other root army is counted in its owner faction's relation counter C (group-A command classes:
   counter D).
*/
void ArmyRuntime_ApplyImpactDamageAndFinalizeState
          (AngleTurn32 impactAngle,DamageAmount32 damageAmount,ModelRuntimeSlot *modelRuntime)

{
  Q12 *healthField;
  GameFactionRuntimeRecord *ownerFaction;
  int maxHealth;
  uint32_t classId;
  int previousHealth;
  int remainingHealth;
  int healthToMaximum;
  int ownerFactionIndex;
  Bool8 rotateToImpact;
  ModelRuntimeNode *parentModelNode;

  (modelRuntime->classState).healthRegenerationDelayTicks = ARMY_DAMAGE_REGENERATION_DELAY_TICKS;
  if ((int)modelRuntime->health <= 0) {
    return;
  }
  maxHealth = modelRuntime->definitionOrSavedId.runtimeDefinition->maximumHealth;
  healthField = (Q12 *)&modelRuntime->health;
  previousHealth = *healthField;
  *healthField = *healthField - damageAmount;
  /* the new health is still > 0; a negative damage (repair) never raises it above maxHealth */
  if (previousHealth > damageAmount) {
    healthToMaximum = maxHealth - modelRuntime->health;
    if (healthToMaximum == 0 || maxHealth < (int)modelRuntime->health) {
      modelRuntime->health = modelRuntime->health + healthToMaximum;
    }
    return;
  }
  /* the new health is <= 0 */
  remainingHealth = modelRuntime->health; /* <= 0; its negation is the excess damage */
  (modelRuntime->classState).stateFlags = (modelRuntime->classState).stateFlags | ARMY_RUNTIME_FLAG_DESTROYED;
  parentModelNode = modelRuntime->rootModelNodeOrSavedOffset.modelNode->parentNode;
  /* a destroyed model links to itself */
  modelRuntime->linkedModelRuntimeOrSavedOffset.modelRuntime = modelRuntime;
  modelRuntime->health = 0;
  if (parentModelNode != NULL) {
    ArmyRuntime_ApplyDamageAndPropagateToParent(-remainingHealth,(parentModelNode->runtimePayload).modelRuntime);
    return;
  }
  /* The original's branch here tests a CPU flag that is formally undefined at that point (it is left over
     from a multiplication); measured on an AMD Zen 3 it still holds the result of the runtimeClassId /
     placementContactKindIndex tests above, so the counters are updated unless the army was turned to the
     impact. The C follows that. */
  rotateToImpact =
       (modelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_00) &&
       (modelRuntime->definitionOrSavedId.runtimeDefinition->placementContactKindIndex == 0);
  if (rotateToImpact) {
    (modelRuntime->rootModelNodeOrSavedOffset.modelNode->modelPayload).worldRotationAngle0 = impactAngle;
    return;
  }
  ownerFactionIndex = modelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex;
  ownerFaction = &g_GameFactionRuntimeImage.records[ownerFactionIndex];
  classId = modelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId;
  ownerFaction->relationCounterC = ownerFaction->relationCounterC + 1;
  if (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand[classId] ==
      ArmyRuntime_ClassCommandHandlerGroupA) {
    /* armies of the group-A command class move from counter C to counter D */
    ownerFaction->relationCounterD = ownerFaction->relationCounterD + 1;
    ownerFaction->relationCounterC = ownerFaction->relationCounterC - 1;
  }
}


/* Splits a shot impact between the hit army and the army it is mounted on: half (rounded down) goes to the
   hit army, the rest to the parent model node's army, or to the hit army again when it has no parent. Called
   directly by the shot impact handling in world/shots/maintenance.c.
*/
void ArmyRuntime_ApplyImpactDamageToRuntimeAndParent(AngleTurn32 impactAngle,FactionRuntimeIndex sourceFactionIndex,
          ImpactDamageValue32 impactValue,ModelRuntimeSlot *targetModelRuntime)

{
  ModelRuntimeNode *targetModelNodeRuntime;
  ModelRuntimeSlot *secondHalfRecipient;

  /* GameEntityRuntime_ApplyImpactDamageAndFactionRelationState gets the model runtime under its
     GameEntityRuntime parameter type */
  targetModelNodeRuntime = targetModelRuntime->rootModelNodeOrSavedOffset.modelNode;
  GameEntityRuntime_ApplyImpactDamageAndFactionRelationState
            (impactAngle,sourceFactionIndex,impactValue >> 1,(GameEntityRuntime *)targetModelRuntime);
  secondHalfRecipient = targetModelRuntime;
  if (targetModelNodeRuntime->parentNode != NULL) {
    secondHalfRecipient = (targetModelNodeRuntime->parentNode->runtimePayload).modelRuntime;
  }
  GameEntityRuntime_ApplyImpactDamageAndFactionRelationState
            (impactAngle,sourceFactionIndex,impactValue - (impactValue >> 1),
             (GameEntityRuntime *)secondHalfRecipient);
}


/* Owner test for a model in the line of fire: it blocks unless it is the shooter's command target or passes
   the commandState owner test (commandState < 1: models of the own owner pass, otherwise those of other
   owners). */
static Bool8 ArmyWeaponRuntime_IsBlockedByHitEntity(GameEntityRuntime *ownEntity,GameEntityRuntime *hitEntity)

{
  int ownOwnerIndex;
  Bool8 ownerTestFails;

  ownOwnerIndex = (ownEntity->common).ownership.ownerIndex;
  if ((ownEntity->common).commandState < 1) {
    ownerTestFails = ownOwnerIndex != (hitEntity->common).ownership.ownerIndex;
  }
  else {
    ownerTestFails = ownOwnerIndex == (hitEntity->common).ownership.ownerIndex;
  }
  return ownerTestFails && (hitEntity != (ownEntity->common).commandTarget.targetEntity);
}


/* Line-of-fire test for ballistic shots (true = blocked): the arc must be solvable, its elevation within
   [minPitchAngle, maxPitchAngle] and no model in the way along the horizontal distance. */
static Bool8 ArmyWeaponRuntime_TestBallisticLineOfFire
          (int deltaZ,int deltaY,int deltaX,int minPitchAngle,int maxPitchAngle,ShotDefinition *shotDefinition,
           ModelRuntimeNode *originNode,WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  FixedLengthAngle horizontalVector;
  int speedSquared;
  int scaledLength;
  int64_t discriminant;
  uint32_t discriminantRoot;
  uint32_t elevationAngle;
  Bool8 modelHit;
  Q12 modelHitDistanceQ12;
  ModelRuntimeNode *hitModelNode;
  GameEntityRuntime *ownEntity;
  GameEntityRuntime *hitEntity;

  horizontalVector = FixedMath_Vector2AngleAndLength(deltaY,deltaX);
  speedSquared = shotDefinition->launchSpeedQ12 * shotDefinition->launchSpeedQ12;
  scaledLength = horizontalVector.length * shotDefinition->ballisticDivisorQ12;
  discriminant = (int64_t)(speedSquared + shotDefinition->ballisticDivisorQ12 * deltaZ * -2) *
                 (int64_t)speedSquared -
                 (int64_t)scaledLength * (int64_t)scaledLength;
  if (discriminant < 0) {
    return true;
  }
  discriminantRoot = FIXED_UINT64_SQRT(discriminant);
  if ((-Q12_ONE < deltaZ) && (deltaZ < Q12_ONE)) {
    discriminantRoot = -discriminantRoot;
  }
  elevationAngle = FixedMath_Atan2Angle16(speedSquared + discriminantRoot,scaledLength);
  if ((int)elevationAngle < minPitchAngle) {
    return true;
  }
  if (maxPitchAngle < (int)elevationAngle) {
    return true;
  }
  modelHit = ModelRuntime_RaycastCandidateListNearest
                     (elevationAngle,horizontalVector.angle & FIXED_ANGLE16_MASK,horizontalVector.length,
                      (originNode->worldTransform).translation.z,
                      (originNode->worldTransform).translation.y,
                      (originNode->worldTransform).translation.x,WORLD_OWNER_RUNTIME_MODEL,
                      (modelRuntime->ownerArmyRuntimeOrSavedOffset.entityRuntime->common).ownership.modelNode,
                      worldRuntime,&modelHitDistanceQ12,&hitModelNode);
  if (!modelHit) {
    return false;
  }
  /* As in the original: this tests the shooter's own entity (through originNode), not the model that was hit
     (the other path uses hitModelNode). */
  ownEntity = modelRuntime->ownerArmyRuntimeOrSavedOffset.entityRuntime;
  hitEntity = ((originNode->runtimePayload).modelRuntime)->ownerArmyRuntimeOrSavedOffset.entityRuntime;
  return ArmyWeaponRuntime_IsBlockedByHitEntity(ownEntity,hitEntity);
}


/* Checks whether the army's weapon can hit the target position; true = blocked. Ballistic shots need a
   solvable arc whose elevation lies within the weapon's limits (definition minimumPitchAngle /
   maximumPitchAngle) and no model in the way
   along the horizontal distance; fixed-range shots always pass; other shots need an elevation within the limits
   (unless guided), no terrain in front of the target (a ground shot without an entity target may land within
   0x400 of the aim point) and no model in the way, and must reach the target (its distance minus half its radius)
   within speed * (lifetime - 2/3 ramp - 1). A model in the way does not block when it is the command target or
   passes the owner test (commandState < 1: models of the own owner, otherwise those of other owners). Called
   directly by the AI combat target selection (gameplay/ai/combat.c) and gameplay/army/movement.c.
*/
Bool8 ArmyWeaponRuntime_TestTargetLineOfFire(Q12 targetWorldZQ12,Q12 targetWorldYQ12,Q12 targetWorldXQ12,
          WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  ArmyWeaponDefinitionView *weaponDefinition;
  ShotDefinition *shotDefinition;
  int deltaX;
  int deltaY;
  int deltaZ;
  int minPitchAngle;
  int maxPitchAngle;
  AngleTurn32 azimuthAngle;
  AngleTurn32 elevationAngle;
  uint32_t targetDistance;
  int maxRayLength;
  int terrainHitDistance;
  uint32_t distanceDifference;
  Bool8 modelHit;
  Bool8 terrainHitFirst;
  Q12 modelHitDistanceQ12;
  ModelRuntimeNode *hitModelNode;
  FixedLengthAzimuthElevation targetVector;
  GraphicsWorldCoordinateQ12 originZQ12;
  GraphicsWorldCoordinateQ12 originYQ12;
  GraphicsWorldCoordinateQ12 originXQ12;
  WorldOwnerRuntimeClassId requiredOwnerId;
  ModelRuntimeNode *excludedNode;
  GameEntityRuntime *hitEntity;
  GameEntityRuntime *ownEntity;
  GameEntityRuntime *targetEntity;
  ModelRuntimeNode *originNode;

  /* modelRuntime is the weapon's model runtime; ownEntity is its owning army */
  originNode = modelRuntime->rootModelNodeOrSavedOffset.modelNode;
  ownEntity = modelRuntime->ownerArmyRuntimeOrSavedOffset.entityRuntime;
  weaponDefinition = (ArmyWeaponDefinitionView *)modelRuntime->definitionOrSavedId.runtimeDefinition;
  deltaX = targetWorldXQ12 - (originNode->worldTransform).translation.x;
  minPitchAngle = weaponDefinition->minimumPitchAngle;
  maxPitchAngle = weaponDefinition->maximumPitchAngle;
  deltaY = targetWorldYQ12 - (originNode->worldTransform).translation.y;
  shotDefinition = weaponDefinition->shotDefinition;
  deltaZ = targetWorldZQ12 - (originNode->worldTransform).translation.z;
  if (shotDefinition->trajectoryMode == SHOT_TRAJECTORY_BALLISTIC) {
    return ArmyWeaponRuntime_TestBallisticLineOfFire
                     (deltaZ,deltaY,deltaX,minPitchAngle,maxPitchAngle,shotDefinition,originNode,worldRuntime,
                      modelRuntime);
  }
  if (shotDefinition->trajectoryMode == SHOT_TRAJECTORY_FIXED_RANGE) {
    return false;
  }
  targetVector = FixedMath_VectorToAnglesAndLength(deltaZ,deltaY,deltaX);
  elevationAngle = targetVector.elevationAngle;
  azimuthAngle = targetVector.azimuthAngle;
  targetDistance = targetVector.lengthQ12;
  if (shotDefinition->guidanceTurnLimitAngle16 == 0) {
    if ((int)elevationAngle < minPitchAngle) {
      return true;
    }
    if (maxPitchAngle < (int)elevationAngle) {
      return true;
    }
  }
  excludedNode = (ownEntity->common).ownership.modelNode;
  requiredOwnerId = WORLD_OWNER_RUNTIME_MODEL;
  originXQ12 = (originNode->worldTransform).translation.x;
  maxRayLength = shotDefinition->launchSpeedQ12 * (int)shotDefinition->projectileLifetimeTicks;
  originYQ12 = (originNode->worldTransform).translation.y;
  originZQ12 = (originNode->worldTransform).translation.z;
  /* only the distance matters: a miss reports FIELD_GRID_RAYCAST_MISS_DISTANCE */
  (void)FieldGrid_RaycastTerrainSurfaceDistance
                     (elevationAngle,azimuthAngle,maxRayLength,(originNode->worldTransform).translation.z,
                      (originNode->worldTransform).translation.y,
                      (originNode->worldTransform).translation.x,worldRuntime->fieldGrid,
                      &terrainHitDistance,NULL);
  modelHit = ModelRuntime_RaycastCandidateListNearest
                     (elevationAngle,azimuthAngle,maxRayLength,originZQ12,originYQ12,originXQ12,
                      requiredOwnerId,excludedNode,worldRuntime,&modelHitDistanceQ12,&hitModelNode);
  if (modelHit) {
    terrainHitFirst = terrainHitDistance < modelHitDistanceQ12;
  }
  else {
    terrainHitFirst = terrainHitDistance <= INT32_MAX - 1;
  }
  if (terrainHitFirst) {
    /* The terrain is hit first: only a ground shot without an entity target landing within 0x400 of the
       aim distance is clear. */
    distanceDifference = terrainHitDistance - targetDistance;
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
    /* A model is hit first: blocked when its owner fails the commandState owner test and it is not
       the command target; otherwise fall through to the range check. */
    ownEntity = modelRuntime->ownerArmyRuntimeOrSavedOffset.entityRuntime;
    hitEntity = ((hitModelNode->runtimePayload).modelRuntime)->ownerArmyRuntimeOrSavedOffset.entityRuntime;
    if (ArmyWeaponRuntime_IsBlockedByHitEntity(ownEntity,hitEntity)) {
      return true;
    }
  }
  /* range check: distance to the target minus half its radius (footprintRadius of its definition) against
     speed * (lifetime - 2/3 of the ramp ticks - 1); ARMY_SHOT_RAMP_RANGE_FACTOR_Q12 is -2/3 in Q12 */
  targetEntity = (modelRuntime->ownerArmyRuntimeOrSavedOffset.entityRuntime->common).commandTarget.targetEntity;
  if (targetEntity != NULL) {
    targetDistance = (int)(targetDistance * 2 -
                 ((ModelRuntimeSlot *)(targetEntity->common).ownership.definitionOrClassRecord)->
                 definitionOrSavedId.runtimeDefinition->footprintRadius
                 ) >> 1;
  }
  if (shotDefinition->launchSpeedQ12 *
      (((int)shotDefinition->trajectoryRampDurationTicks * ARMY_SHOT_RAMP_RANGE_FACTOR_Q12 >> Q12_SHIFT) +
       (int)shotDefinition->projectileLifetimeTicks - 1) < (int)targetDistance) {
    return true;
  }
  return false;
}


/* Subtracts damageAmount from the health of a living army. When the health drops to zero or below the army is
   flagged destroyed and the excess damage is passed on to the army it is attached to (the parent model node),
   so destroying a mounted part also damages its carrier; a negative damage (repair) is capped at the maximum
   health.
*/
void ArmyRuntime_ApplyDamageAndPropagateToParent(DamageAmount32 damageAmount,ModelRuntimeSlot *modelRuntime)

{
  Q12 *healthField;
  int previousHealth;
  int remainingHealth;
  int maxHealth;
  ModelRuntimeNode *parentModelNode;

  (modelRuntime->classState).healthRegenerationDelayTicks = ARMY_DAMAGE_REGENERATION_DELAY_TICKS;
  if (0 < (int)modelRuntime->health) {
    maxHealth = modelRuntime->definitionOrSavedId.runtimeDefinition->maximumHealth;
    healthField = (Q12 *)&modelRuntime->health;
    previousHealth = *healthField;
    *healthField = *healthField - damageAmount;
    /* the new health is <= 0 */
    if (previousHealth <= damageAmount) {
      remainingHealth = modelRuntime->health; /* <= 0; its negation is the excess damage */
      (modelRuntime->classState).stateFlags =
           (modelRuntime->classState).stateFlags | ARMY_RUNTIME_FLAG_DESTROYED;
      /* a destroyed model links to itself */
      modelRuntime->linkedModelRuntimeOrSavedOffset.modelRuntime = modelRuntime;
      parentModelNode = modelRuntime->rootModelNodeOrSavedOffset.modelNode->parentNode;
      modelRuntime->health = 0;
      if (parentModelNode != NULL) {
        ArmyRuntime_ApplyDamageAndPropagateToParent(-remainingHealth,(parentModelNode->runtimePayload).modelRuntime);
      }
      /* Without a parent the original goes on to the owner faction's relationCounterC/D update of
         ArmyRuntime_ApplyImpactDamageAndFinalizeState, but guarded by a branch on a CPU flag that is left over
         from a multiplication (measured on an AMD Zen 3: unchanged) and still reflects the parent == NULL test,
         so the branch is always taken and the update never runs: the C omits it. */
    }
    else if (maxHealth < (int)modelRuntime->health) {
      /* a negative damage (repair) never raises the health above maxHealth */
      modelRuntime->health =
           modelRuntime->health + (maxHealth - modelRuntime->health);
    }
  }
  return;
}


/* Damage smoke/fire of a damaged army: while its health is below the definition's threshold percentage
   (damageEffectHealthPercent) of the class maximum (maximumHealth), it emits the definition's effect
   (damageEffectDefinitionReference) every damageEffectIntervalTicks + random(damageEffectRandomTicks) ticks
   from the model's damage points (packed point key class 3), cycling through them, or from the model origin
   when it has none, with random orientation angles. Called directly at the end of nearly every army class
   runtime update (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate slots, e.g. [4], [9], [15] in
   this file).
*/
void ArmyRuntime_EmitDamageThresholdEffect(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  ModelRuntimeNode *modelNodeRuntime;
  uint32_t cooldownRandom;
  uint32_t pointXQ12;
  ModelPackedPointRecord *localPointRecord;
  ModelDefinition *definition;
  uint32_t randomBits;
  uint32_t randomValue;
  uint32_t pointYQ12;
  uint32_t cooldownRandomTicks;
  uint32_t angleRandom;
  uint32_t pointZQ12;
  Bool8 emitterPointFound;
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
  cooldownRandomTicks = 0;
  if (definition->damageEffectRandomTicks != 0) {
    cooldownRandom = g_RandomGeneratorState.next();
    cooldownRandomTicks = cooldownRandom % definition->damageEffectRandomTicks;
  }
  modelNodeRuntime = modelRuntime->rootModelNodeOrSavedOffset.modelNode;
  modelRuntime->damageEffectCooldownTicks = cooldownRandomTicks + definition->damageEffectIntervalTicks;
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
    pointXQ12 = (modelNodeRuntime->worldTransform).translation.x;
    pointYQ12 = (modelNodeRuntime->worldTransform).translation.y;
    pointZQ12 = (modelNodeRuntime->worldTransform).translation.z;
  }
  else {
    transformedPoint = ModelNodeRuntime_TransformLocalPoint(localPointRecord,modelNodeRuntime);
    pointZQ12 = transformedPoint.zQ12;
    pointYQ12 = transformedPoint.yQ12;
    pointXQ12 = transformedPoint.xQ12;
    modelRuntime->damageEffectPointIndex++;
  }
  effectDefinition = definition->damageEffectDefinitionReference.definition;
  randomBits = g_RandomGeneratorState.next();
  angleRandom = randomBits & FIXED_ANGLE16_MASK;
  randomValue = g_RandomGeneratorState.next();
  EffectRuntimePool_CreateInstanceFromDefinition
            (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_COMPOUND(EffectRuntimeOwnerReference){ .modelNode = NULL },randomBits >> 16,
             (randomValue & (FIXED_ANGLE16_EIGHTH_TURN - 1)) + (FIXED_ANGLE16_EIGHTH_TURN - 1),angleRandom,
             pointZQ12,pointYQ12,pointXQ12,effectDefinition,worldRuntime);
  return;
}

