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
  attachmentNodeHeader = Thandor_U32ToPointer<MdlSerializedNodeHeader>(
                         Thandor_U32ToPointer<MdlSerializedNodeHeader>(weaponDefinitionView->rootNode->childSerializedOffsets[0])-> /* 32-bit format field: MdlSerializedNodeHeader.childSerializedOffsets */
                         childSerializedOffsets[0]);
  for (attachmentSelectorOrdinal = 0; attachmentSelectorOrdinal < ARMY_WEAPON_ATTACHMENT_COUNT;
      attachmentSelectorOrdinal++) {
    if (modelRuntime->attachmentReloadTicks[attachmentSelectorOrdinal] != 0) {
      continue;
    }
    modelRuntime->attachmentReloadTicks[attachmentSelectorOrdinal] = weaponDefinitionView->attachmentReloadTicks;
    commandTargetArmy = modelRuntime->ownerArmyRuntime->commandTargetArmyRuntime;
    targetRuntimeReference = 0;
    if (commandTargetArmy != nullptr) {
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
   directly by the AI combat target selection (gameplay/ai/combat.cpp) and gameplay/army/move_orders.cpp.
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
                      &terrainHitDistance,nullptr);
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
    if (((modelRuntime->ownerArmyRuntimeOrSavedOffset.entityRuntime->common).commandTarget.targetEntity == nullptr) &&
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
  if (targetEntity != nullptr) {
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
