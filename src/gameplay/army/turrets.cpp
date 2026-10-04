/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/turrets.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/army/turrets.h>
#include <thandor/thandor.h>

/* Implementation ownership: gameplay/army/turrets. */

/* Turret with one barrel (runtimeUpdate slot 7 of g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes, called by
   model class from ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive). The root node yaws, its first
   child pitches, and that child's first child is the barrel: while reloading it spins by the definition's step
   per tick, after a shot it recoils back and returns over the recoil countdown. With an aim point the turret
   turns towards the launch direction and fires once it is on target and reloaded, unless
   ArmyRuntimeCommand_UpdateTargetFollowingState finds the line of fire blocked; without one it returns to rest
   while the owner moves or it is still turning. Skipped while destroyed.
*/

void ArmyRuntimeClass_UpdateSingleBarrelTurret
          (WorldRuntimeContext *worldRuntime,ModelRuntimeWeaponAimStateView *modelRuntime)

{
  ModelRuntimeFlags *nodeFlags;
  WeaponAimCountdownTicks *countdownTicks;
  AngleTurn32 *rotationAngle;
  ArmyWeaponDefinitionView *weaponDefinition;
  int recoilCountdown;
  FixedMathScale32 recoilScale;
  ModelRuntimeNode *pitchNode;
  uint32_t recoilTicks;
  ArmyRuntimeSlot *commandTargetArmy;
  InGameSimulationStepBatchTicks elapsedTicks;
  Q12 aimWorldX;
  Q12 aimWorldY;
  Q12 aimWorldZ;
  AngleTurn32 targetPitchAngle16;
  ShotTargetModelReference targetReference;
  ShotLaunchAngles launchAngles;
  ModelRelativeDirectionAngles relativeAngles;
  uint32_t pitchAimValue;
  Bool8 waypointArrived;
  Q12 waypointWorldXQ12;
  Q12 waypointWorldYQ12;
  GraphicsFixedVec3 aimPoint;
  Bool8 aimPointFound;
  GameEntityRuntime *ownerEntity;
  ModelRuntimeNode *partNode;

  elapsedTicks = g_InGameSimulationStepTicks;
  /* bit 0x1 of the runtime flags is not named yet */
  if (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_INACTIVE_MASK) == 0) {
    weaponDefinition = modelRuntime->modelDefinition;
    ownerEntity = (GameEntityRuntime *)modelRuntime->ownerArmyRuntime;
    recoilCountdown = modelRuntime->attachment0BackwardStepCountdownTicks;
    if (modelRuntime->attachmentReloadCountdownTicks != 0) {
      /* spin the barrel for the reload ticks that elapsed (at most the remaining countdown) */
      partNode = modelRuntime->rootModelNode->childNodes[0];
      countdownTicks = &modelRuntime->attachmentReloadCountdownTicks;
      *countdownTicks = *countdownTicks - g_InGameSimulationStepTicks;
      if (*countdownTicks < 0) {
        elapsedTicks = elapsedTicks + modelRuntime->attachmentReloadCountdownTicks;
        modelRuntime->attachmentReloadCountdownTicks = 0;
      }
      partNode = partNode->childNodes[0];
      rotationAngle = &(partNode->modelPayload).localRotationAngle2;
      *rotationAngle = *rotationAngle + elapsedTicks * weaponDefinition->localRotationAngle2StepPerTick;
      partNode->runtimeFlags = partNode->runtimeFlags | 1;
      rotationAngle = &(partNode->modelPayload).localRotationAngle2;
      *rotationAngle = *rotationAngle & FIXED_ANGLE16_MASK;
    }
    elapsedTicks = g_InGameSimulationStepTicks;
    if (recoilCountdown != 0) {
      /* move the barrel forward again by the elapsed recoil ticks */
      recoilScale = weaponDefinition->backwardStepScale;
      partNode = modelRuntime->rootModelNode->childNodes[0];
      countdownTicks = &modelRuntime->attachment0BackwardStepCountdownTicks;
      *countdownTicks = *countdownTicks - g_InGameSimulationStepTicks;
      if (*countdownTicks < 0) {
        elapsedTicks = elapsedTicks + modelRuntime->attachment0BackwardStepCountdownTicks;
        modelRuntime->attachment0BackwardStepCountdownTicks = 0;
      }
      partNode = partNode->childNodes[0];
      FixedVector_StepBackwardAlongOwnDirection(-elapsedTicks,recoilScale,(FixedVectorStateAddress32)partNode);
      nodeFlags = &partNode->runtimeFlags;
      *nodeFlags = *nodeFlags | 1;
    }
    partNode = modelRuntime->rootModelNode;
    aimPointFound = ArmyRuntime_ResolveShotAimPoint
                       ((partNode->worldTransform).translation.z,
                        (partNode->worldTransform).translation.y,
                        (partNode->worldTransform).translation.x,weaponDefinition->shotDefinition,
                        ownerEntity,&aimPoint);
    aimWorldZ = aimPoint.z;
    aimWorldY = aimPoint.y;
    aimWorldX = aimPoint.x;
    if (!aimPointFound) {
      waypointArrived = ArmyRuntime_UpdateMovementAndWaypoints
                         (worldRuntime,(ArmyMovementRuntime *)ownerEntity,&waypointWorldXQ12,&waypointWorldYQ12);
      if (((!waypointArrived) || (modelRuntime->pitchTurnVelocityAngle16 != 0)) ||
         (modelRuntime->yawTurnVelocityAngle16 != 0)) {
        partNode = modelRuntime->rootModelNode;
        ModelNodeRuntime_SmoothYawTowardTarget(partNode,modelRuntime,0);
        ModelNodeRuntime_SmoothPitchTowardTarget(partNode->childNodes[0],modelRuntime,0);
      }
    }
    else {
      partNode = modelRuntime->rootModelNode;
      pitchNode = partNode->childNodes[0];
      launchAngles = ShotDefinition_ComputeLaunchAngles
                         (aimWorldZ,aimWorldY,aimWorldX,(pitchNode->worldTransform).translation.z,
                          (pitchNode->worldTransform).translation.y,
                          (pitchNode->worldTransform).translation.x,weaponDefinition->shotDefinition);
      relativeAngles = ModelNodeRuntime_ComputeRelativeDirectionAngle
                         (partNode,launchAngles.elevationAngle,launchAngles.headingAngle);
      targetPitchAngle16 = relativeAngles.relativePitchAngle;
      if (ModelNodeRuntime_SmoothYawTowardTarget
                         (partNode,modelRuntime,relativeAngles.relativeYawAngle)) {
        ModelNodeRuntime_SmoothPitchTowardTarget(pitchNode,modelRuntime,targetPitchAngle16);
      }
      else {
        pitchAimValue = ModelNodeRuntime_SmoothPitchTowardTarget
                           (pitchNode,modelRuntime,targetPitchAngle16);
        if (pitchAimValue == targetPitchAngle16) {
          weaponDefinition = modelRuntime->modelDefinition;
          /* reloaded and the line of fire is free */
          if ((modelRuntime->attachmentReloadCountdownTicks == 0) &&
              (!ArmyRuntimeCommand_UpdateTargetFollowingState
                  (aimWorldZ,aimWorldY,aimWorldX,worldRuntime,(ModelRuntimeSlot *)modelRuntime))) {
            /* fire: reload, recoil the barrel, rock the owner back and launch the projectiles */
            recoilTicks = weaponDefinition->sharedInterShotTicks;
            recoilScale = weaponDefinition->backwardStepScale;
            modelRuntime->attachmentReloadCountdownTicks =
                 modelRuntime->attachmentReloadCountdownTicks + weaponDefinition->attachmentReloadTicks;
            modelRuntime->attachment0BackwardStepCountdownTicks =
                 modelRuntime->attachment0BackwardStepCountdownTicks + recoilTicks;
            partNode = pitchNode->childNodes[0];
            FixedVector_StepBackwardAlongOwnDirection
                      (recoilTicks,recoilScale,(FixedVectorStateAddress32)partNode);
            partNode->runtimeFlags = partNode->runtimeFlags | 1;
            ArmyRuntime_SetNonzeroActionVector
                      (launchAngles.headingAngle,weaponDefinition->postLaunchVector1Q12,weaponDefinition->postLaunchVector0Q12
                       ,modelRuntime->ownerArmyRuntime);
            commandTargetArmy = modelRuntime->ownerArmyRuntime->commandTargetArmyRuntime;
            targetReference = 0;
            if (commandTargetArmy != NULL) {
              targetReference = (commandTargetArmy->modelRuntimeOrSavedOffset).savedIdOrOffset;
            }
            /* the muzzle point is the first serialized child of the model point source */
            ModelRuntime_EmitProjectilesFromAttachmentPoints
                      (targetReference,aimWorldZ,aimWorldY,aimWorldX,weaponDefinition->shotDefinition,partNode,
                       (MdlSerializedNodeHeader *)
                       ((MdlSerializedNodeHeader *)weaponDefinition->rootNode->childSerializedOffsets[0])-> /* 5f-format: MdlSerializedNodeHeader.childSerializedOffsets */
                       childSerializedOffsets[0],worldRuntime);
          }
        }
      }
    }
  }
  ModelNodeRuntime_RebuildTransformsFromRoot(modelRuntime->rootModelNode);
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ModelRuntimeSlot *)modelRuntime);
  return;
}

/* Turret with two alternating barrels (runtimeUpdate slot 8 of g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes,
   called by model class from ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive). Same as
   ArmyRuntimeClass_UpdateSingleBarrelTurret, but the pitch node has two barrels (children 0 and
   1) with their own recoil countdowns; the shots alternate between them, the even sequence numbers firing from
   barrel 1 and its muzzle point.
*/

void ArmyRuntimeClass_UpdateTwinBarrelTurret
          (WorldRuntimeContext *worldRuntime,ModelRuntimeWeaponAimStateView *modelRuntime)

{
  ModelRuntimeFlags *nodeFlags;
  WeaponAimCountdownTicks *countdownTicks;
  AngleTurn32 *rotationAngle;
  ArmyWeaponDefinitionView *weaponDefinition;
  FixedMathScale32 recoilScale;
  ModelRuntimeNode *pitchNode;
  uint32_t recoilTicks;
  ArmyRuntimeSlot *commandTargetArmy;
  InGameSimulationStepBatchTicks elapsedTicks;
  Q12 aimWorldX;
  int barrel0RecoilCountdown;
  /* index of the firing barrel's muzzle point among the muzzle node's serialized children (0 or 1) */
  int muzzlePointIndex;
  Q12 aimWorldY;
  Q12 aimWorldZ;
  AngleTurn32 targetPitchAngle16;
  ShotTargetModelReference targetReference;
  ShotLaunchAngles launchAngles;
  ModelRelativeDirectionAngles relativeAngles;
  uint32_t pitchAimValue;
  Bool8 waypointArrived;
  Q12 waypointWorldXQ12;
  Q12 waypointWorldYQ12;
  GraphicsFixedVec3 aimPoint;
  Bool8 aimPointFound;
  GameEntityRuntime *ownerEntity;
  ModelRuntimeNode *partNode;

  elapsedTicks = g_InGameSimulationStepTicks;
  /* bit 0x1 of the runtime flags is not named yet */
  if (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_INACTIVE_MASK) == 0) {
    weaponDefinition = modelRuntime->modelDefinition;
    ownerEntity = (GameEntityRuntime *)modelRuntime->ownerArmyRuntime;
    barrel0RecoilCountdown = modelRuntime->attachment0BackwardStepCountdownTicks;
    if (modelRuntime->attachmentReloadCountdownTicks != 0) {
      /* spin the first barrel for the reload ticks that elapsed */
      partNode = modelRuntime->rootModelNode->childNodes[0];
      countdownTicks = &modelRuntime->attachmentReloadCountdownTicks;
      *countdownTicks = *countdownTicks - g_InGameSimulationStepTicks;
      if (*countdownTicks < 0) {
        elapsedTicks = elapsedTicks + modelRuntime->attachmentReloadCountdownTicks;
        modelRuntime->attachmentReloadCountdownTicks = 0;
      }
      partNode = partNode->childNodes[0];
      rotationAngle = &(partNode->modelPayload).localRotationAngle2;
      *rotationAngle = *rotationAngle + elapsedTicks * weaponDefinition->localRotationAngle2StepPerTick;
      partNode->runtimeFlags = partNode->runtimeFlags | 1;
      rotationAngle = &(partNode->modelPayload).localRotationAngle2;
      *rotationAngle = *rotationAngle & FIXED_ANGLE16_MASK;
    }
    elapsedTicks = g_InGameSimulationStepTicks;
    if (barrel0RecoilCountdown != 0) {
      /* barrel 0 returns from its recoil */
      recoilScale = weaponDefinition->backwardStepScale;
      partNode = modelRuntime->rootModelNode->childNodes[0];
      countdownTicks = &modelRuntime->attachment0BackwardStepCountdownTicks;
      *countdownTicks = *countdownTicks - g_InGameSimulationStepTicks;
      if (*countdownTicks < 0) {
        elapsedTicks = elapsedTicks + modelRuntime->attachment0BackwardStepCountdownTicks;
        modelRuntime->attachment0BackwardStepCountdownTicks = 0;
      }
      partNode = partNode->childNodes[0];
      FixedVector_StepBackwardAlongOwnDirection(-elapsedTicks,recoilScale,(FixedVectorStateAddress32)partNode);
      nodeFlags = &partNode->runtimeFlags;
      *nodeFlags = *nodeFlags | 1;
    }
    elapsedTicks = g_InGameSimulationStepTicks;
    if (modelRuntime->attachment1BackwardStepCountdownTicks != 0) {
      /* barrel 1 returns from its recoil */
      recoilScale = weaponDefinition->backwardStepScale;
      partNode = modelRuntime->rootModelNode->childNodes[0];
      countdownTicks = &modelRuntime->attachment1BackwardStepCountdownTicks;
      *countdownTicks = *countdownTicks - g_InGameSimulationStepTicks;
      if (*countdownTicks < 0) {
        elapsedTicks = elapsedTicks + modelRuntime->attachment1BackwardStepCountdownTicks;
        modelRuntime->attachment1BackwardStepCountdownTicks = 0;
      }
      partNode = partNode->childNodes[1];
      FixedVector_StepBackwardAlongOwnDirection(-elapsedTicks,recoilScale,(FixedVectorStateAddress32)partNode);
      nodeFlags = &partNode->runtimeFlags;
      *nodeFlags = *nodeFlags | 1;
    }
    partNode = modelRuntime->rootModelNode;
    aimPointFound = ArmyRuntime_ResolveShotAimPoint
                       ((partNode->worldTransform).translation.z,
                        (partNode->worldTransform).translation.y,
                        (partNode->worldTransform).translation.x,weaponDefinition->shotDefinition,
                        ownerEntity,&aimPoint);
    aimWorldZ = aimPoint.z;
    aimWorldY = aimPoint.y;
    aimWorldX = aimPoint.x;
    if (!aimPointFound) {
      waypointArrived = ArmyRuntime_UpdateMovementAndWaypoints
                         (worldRuntime,(ArmyMovementRuntime *)ownerEntity,&waypointWorldXQ12,&waypointWorldYQ12);
      if (((!waypointArrived) || (modelRuntime->pitchTurnVelocityAngle16 != 0)) ||
         (modelRuntime->yawTurnVelocityAngle16 != 0)) {
        partNode = modelRuntime->rootModelNode;
        ModelNodeRuntime_SmoothYawTowardTarget(partNode,modelRuntime,0);
        ModelNodeRuntime_SmoothPitchTowardTarget(partNode->childNodes[0],modelRuntime,0);
      }
    }
    else {
      partNode = modelRuntime->rootModelNode;
      pitchNode = partNode->childNodes[0];
      launchAngles = ShotDefinition_ComputeLaunchAngles
                         (aimWorldZ,aimWorldY,aimWorldX,(pitchNode->worldTransform).translation.z,
                          (pitchNode->worldTransform).translation.y,
                          (pitchNode->worldTransform).translation.x,weaponDefinition->shotDefinition);
      relativeAngles = ModelNodeRuntime_ComputeRelativeDirectionAngle
                         (partNode,launchAngles.elevationAngle,launchAngles.headingAngle);
      targetPitchAngle16 = relativeAngles.relativePitchAngle;
      if (ModelNodeRuntime_SmoothYawTowardTarget
                         (partNode,modelRuntime,relativeAngles.relativeYawAngle)) {
        ModelNodeRuntime_SmoothPitchTowardTarget(pitchNode,modelRuntime,targetPitchAngle16);
      }
      else {
        pitchAimValue = ModelNodeRuntime_SmoothPitchTowardTarget
                           (pitchNode,modelRuntime,targetPitchAngle16);
        if (pitchAimValue == targetPitchAngle16) {
          weaponDefinition = modelRuntime->modelDefinition;
          /* reloaded and the line of fire is free */
          if ((modelRuntime->attachmentReloadCountdownTicks == 0) &&
              (!ArmyRuntimeCommand_UpdateTargetFollowingState
                  (aimWorldZ,aimWorldY,aimWorldX,worldRuntime,(ModelRuntimeSlot *)modelRuntime))) {
            /* fire from the next barrel in turn */
            recoilTicks = weaponDefinition->sharedInterShotTicks;
            recoilScale = weaponDefinition->backwardStepScale;
            modelRuntime->attachmentReloadCountdownTicks =
                 modelRuntime->attachmentReloadCountdownTicks + weaponDefinition->attachmentReloadTicks;
            if ((modelRuntime->alternatingAttachmentSequence & 1) == 0) {
              modelRuntime->attachment1BackwardStepCountdownTicks =
                   modelRuntime->attachment1BackwardStepCountdownTicks + recoilTicks;
              partNode = pitchNode->childNodes[1];
              muzzlePointIndex = 1;
            }
            else {
              modelRuntime->attachment0BackwardStepCountdownTicks =
                   modelRuntime->attachment0BackwardStepCountdownTicks + recoilTicks;
              partNode = pitchNode->childNodes[0];
              muzzlePointIndex = 0;
            }
            modelRuntime->alternatingAttachmentSequence++;
            FixedVector_StepBackwardAlongOwnDirection
                      (recoilTicks,recoilScale,(FixedVectorStateAddress32)partNode);
            partNode->runtimeFlags = partNode->runtimeFlags | 1;
            ArmyRuntime_SetNonzeroActionVector
                      (launchAngles.headingAngle,weaponDefinition->postLaunchVector1Q12,weaponDefinition->postLaunchVector0Q12
                       ,modelRuntime->ownerArmyRuntime);
            commandTargetArmy = modelRuntime->ownerArmyRuntime->commandTargetArmyRuntime;
            targetReference = 0;
            if (commandTargetArmy != NULL) {
              targetReference = (commandTargetArmy->modelRuntimeOrSavedOffset).savedIdOrOffset;
            }
            ModelRuntime_EmitProjectilesFromAttachmentPoints
                      (targetReference,aimWorldZ,aimWorldY,aimWorldX,weaponDefinition->shotDefinition,partNode,
                       (MdlSerializedNodeHeader *)
                       ((MdlSerializedNodeHeader *)weaponDefinition->rootNode->childSerializedOffsets[0])-> /* 5f-format: MdlSerializedNodeHeader.childSerializedOffsets */
                       childSerializedOffsets[muzzlePointIndex],worldRuntime);
          }
        }
      }
    }
  }
  ModelNodeRuntime_RebuildTransformsFromRoot(modelRuntime->rootModelNode);
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ModelRuntimeSlot *)modelRuntime);
  return;
}
