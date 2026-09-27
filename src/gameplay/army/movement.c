/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/movement.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/army/movement.h>
#include <thandor/thandor.h>

/* Implementation ownership: gameplay/army/movement. */

/* Address: 0x00520F60.
   Ownership: gameplay/army/movement.
   Purpose: Runtime-update slot 3 prefix. Owns exactly 00520F60-005210DD and transfers into the independently
   preserved shared-tail owner at 005210DE.
   Local calls: ArmyArticulatedRuntime_UpdateContactChildAndEffects, ArmyRuntime_UpdateMovementAndWaypoints,
   ArmyArticulatedRuntime_UpdateLeftTerrainContact, ArmyArticulatedRuntime_UpdateRightTerrainContact,
   ArmyArticulatedRuntime_UpdateSelectedTerrainContact, ArmyArticulatedRuntime_InitializeLeftTerrainContact,
   ArmyArticulatedRuntime_InitializeRightTerrainContact, ArmyArticulatedRuntime_UpdateSuspensionHierarchy.
   Cross-module calls: ArmyRuntime_EmitDamageThresholdEffect [gameplay/army/combat],
   ArmyCollision_TestPointWithinExpandedRuntimeRadius [gameplay/army/placement], FieldGrid_InterpolateWaterDelta
   [world/terrain/grid], ArmyRuntime_ApplyDamageAndPropagateToParent [gameplay/army/combat],
   GameEntityRuntime_ResolveCommandTargetPosition [gameplay/faction/runtime], FixedMath_Atan2Angle16
   [core/math/fixed].
*/

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_UpdateArticulatedMovement
          (WorldRuntimeContext *worldRuntime,
          ModelRuntimeArticulatedMovementDefinitionView200 *modelRuntime)

{
  ArmyMovementStateFlags *ownerMovementFlags;
  Q12 *contactStateFlags;
  AngleTurn32 previousRotationAngle;
  int previousWorldX;
  int previousWorldY;
  ModelDefinitionArticulatedMovementView280 *movementDefinition;
  int32_t waterDelta;
  int advanceOrDelta;
  uint32_t advanceOrHeading;
  uint32_t relativeHeading;
  ModelRuntimeNode *steeringAngle16;
  int component0;
  ArmyRuntimeCoordinateCommandOrHistoryValue4 x;
  bool withinLinkRadius;
  MovementStepResult waypointResult;
  WorldPositionResult commandTargetPosition;
  FixedLengthAngleEaxEdx8 targetAngleLength;
  ArmyRuntimeSlot *entityRuntime1;
  ModelRuntimeNode *modelNode1;
  
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  entityRuntime1 = modelRuntime->linkedArmyRuntime;
  modelNode1 = modelRuntime->rootModelNode;
  ownerMovementFlags = &modelRuntime->ownerArmyRuntime->movementStateFlags;
  *ownerMovementFlags = *ownerMovementFlags | 4;
  if ((entityRuntime1 != (ArmyRuntimeSlot *)0x0) &&
     ((((((entityRuntime1->modelRuntimeOrSavedOffset).modelRuntime)->classState).classStateDC == 0
       || (modelRuntime->modelDefinition->placementRadiusOrClearanceDC == 0)) ||
      (withinLinkRadius = ArmyCollision_TestPointWithinExpandedRuntimeRadius
                          (modelRuntime->modelDefinition->placementRadiusOrClearanceDC,
                           (modelNode1->worldTransform).translation.y,
                           (modelNode1->worldTransform).translation.x,entityRuntime1), !withinLinkRadius)))) {
    modelRuntime->linkedArmyRuntime = (ArmyRuntimeSlot *)0x0;
  }
  previousRotationAngle = (modelNode1->modelPayload).worldRotationAngle2;
  previousWorldX = (modelNode1->worldTransform).translation.x;
  previousWorldY = (modelNode1->worldTransform).translation.y;
  movementDefinition = modelRuntime->modelDefinition;
  waterDelta = FieldGrid_InterpolateWaterDelta
                    ((modelNode1->worldTransform).translation.y,
                     (modelNode1->worldTransform).translation.x,worldRuntime->fieldGrid);
  if ((movementDefinition->waterDamageThreshold198 < waterDelta) &&
     (advanceOrDelta = waterDelta * movementDefinition->waterDamageMultiplier194 >> 7, -1 < advanceOrDelta)) {
    ArmyRuntime_ApplyDamageAndPropagateToParent(advanceOrDelta,(ArmyRuntimeSlot *)modelRuntime);
  }
  advanceOrDelta = (modelRuntime->movementControl).movementAdvancePerTickQ12;
  x = THANDOR_BITCAST(Q12, ArmyRuntimeCoordinateCommandOrHistoryValue4, movementDefinition->movementAdvanceDeltaQ12PerTick18);
  if (((modelRuntime->articulatedContact).fallbackPosition0Q12 & 3U) != 0) {
    if (((int)modelRuntime->runtimeStateA8 < 0x801) &&
       ((int)(modelRuntime->articulatedContact).terrainContactMode < 0x801)) {
      x.signedValue = x.signedValue + advanceOrDelta;
    }
    else {
      x.signedValue = -(x.signedValue - advanceOrDelta);
    }
    (modelRuntime->movementControl).movementAdvancePerTickQ12 = THANDOR_BITCAST(ArmyRuntimeCoordinateCommandOrHistoryValue4, Q12, x);
    steeringAngle16 =
         (ModelRuntimeNode *)
         ((int)(advanceOrDelta * (modelRuntime->articulatedContact).fallbackPosition1Q12 *
               g_InGameSimulationStepTicks) >> 0xc);
    if (((modelRuntime->articulatedContact).fallbackPosition0Q12 & 2U) == 0) {
      modelRuntime->runtimeStateA8 =
           (uint32_t)((steeringAngle16->modelPayload).reserved2C_33 +
                  (modelRuntime->runtimeStateA8 - 0x38));
      if (0xfff < modelRuntime->runtimeStateA8) {
        contactStateFlags = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
        *contactStateFlags = *contactStateFlags & 0xfffffffe;
        modelRuntime->runtimeStateA8 = 0;
        modelRuntime->movementTarget0Q12 = modelRuntime->runtimeState90;
        modelRuntime->definitionClassValue80 = modelRuntime->runtimeState98;
        modelRuntime->definitionClassValue88 = modelRuntime->articulatedHeightOrStateA0;
        x = (modelRuntime->linkedChildOverloadedState).leftHeadingCommandOrSpawnValue;
        advanceOrHeading = (modelRuntime->linkedChildSpawnParameters).parameter0;
        modelRuntime->classState60 = modelRuntime->ownerValue64;
        modelRuntime->ownerValue68 = modelRuntime->fallbackWorldYQ12;
        (modelRuntime->linkedChildOverloadedState).primaryCoordinateCommandOrHistory = x;
        (modelRuntime->movementControl).movementAdvancePerTickQ12 = advanceOrHeading;
        steeringAngle16 = modelRuntime->rootModelNode;
        ArmyArticulatedRuntime_UpdateContactChildAndEffects
                  (steeringAngle16->childNodes[0],worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
      }
    }
    else {
      (modelRuntime->articulatedContact).terrainContactMode =
           (ArmyTerrainContactDispatchMode)
           ((steeringAngle16->modelPayload).reserved2C_33 +
           ((modelRuntime->articulatedContact).terrainContactMode - 0x38));
      if (0xfff < (modelRuntime->articulatedContact).terrainContactMode) {
        contactStateFlags = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
        *contactStateFlags = *contactStateFlags & 0xfffffffd;
        (modelRuntime->articulatedContact).terrainContactMode =
             ARMY_TERRAIN_CONTACT_ACQUIRE_OR_INITIALIZE_CONTACT_SLOT;
        modelRuntime->movementTarget1Q12 = modelRuntime->runtimeState94;
        modelRuntime->definitionClassValue84 = modelRuntime->articulatedCoordinateOrState9C;
        modelRuntime->runtimeState8C = modelRuntime->runtimeStateA4;
        x = (modelRuntime->linkedChildOverloadedState).rightHeadingCommandOrSpawnValue;
        advanceOrHeading = (modelRuntime->linkedChildSpawnParameters).parameter0;
        modelRuntime->classState60 = modelRuntime->ownerValue64;
        modelRuntime->linkedArmyRuntimeOrSavedOffset =
             (ArmyRuntimeSlot *)modelRuntime->fallbackWorldXQ12;
        (modelRuntime->linkedChildOverloadedState).secondaryCoordinateCommandOrHistory = x;
        (modelRuntime->movementControl).movementAdvancePerTickQ12 = advanceOrHeading;
        steeringAngle16 = modelRuntime->rootModelNode;
        ArmyArticulatedRuntime_UpdateContactChildAndEffects
                  (steeringAngle16->childNodes[1],worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
      }
    }
    if (((modelRuntime->articulatedContact).fallbackPosition0Q12 & 3U) != 0)
    goto ArmyArticulatedMovement_CommitPositionSuspensionAndTransforms;
    modelNode1 = modelRuntime->rootModelNode;
    waypointResult = ArmyRuntime_UpdateMovementAndWaypoints
                       (worldRuntime,(ArmyMovementRuntime *)modelRuntime->ownerArmyRuntime);
    if (waypointResult.arrived) {
      commandTargetPosition = GameEntityRuntime_ResolveCommandTargetPosition
                         ((GameEntityRuntime *)modelRuntime->ownerArmyRuntime);
      x.signedValue = commandTargetPosition.worldZQ12;
      if (!commandTargetPosition.unresolved) {
        advanceOrHeading = FixedMath_Atan2Angle16
                          (commandTargetPosition.worldYQ12 - (modelNode1->worldTransform).translation.y,
                           commandTargetPosition.worldXQ12 - (modelNode1->worldTransform).translation.x);
        steeringAngle16 =
             (ModelRuntimeNode *)(advanceOrHeading - (modelNode1->modelPayload).worldRotationAngle2 & 0xffff);
        if (((ModelRuntimeNode *)0x800 < steeringAngle16) &&
           (steeringAngle16 < (ModelRuntimeNode *)0xf800))
        goto ArmyArticulatedMovement_CommitPositionSuspensionAndTransforms;
      }
    }
    else {
      x.signedValue = waypointResult.worldYQ12 - (modelNode1->worldTransform).translation.y;
      steeringAngle16 =
           (ModelRuntimeNode *)
           FixedMath_Length2(x.signedValue,
                             waypointResult.worldXQ12 - (modelNode1->worldTransform).translation.x);
      if ((ModelRuntimeNode *)0x40 < steeringAngle16)
      goto ArmyArticulatedMovement_CommitPositionSuspensionAndTransforms;
    }
ArmyArticulatedMovement_ClearContactTransitionAndAdvanceWaypoint:
    entityRuntime1 = modelRuntime->ownerArmyRuntime;
    contactStateFlags = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
    *contactStateFlags = *contactStateFlags & 0xffffffcf;
    waypointResult = ArmyRuntime_UpdateMovementAndWaypoints
                       (worldRuntime,(ArmyMovementRuntime *)entityRuntime1);
    x.signedValue = waypointResult.worldYQ12;
    steeringAngle16 = (ModelRuntimeNode *)waypointResult.worldXQ12;
    if (!waypointResult.arrived) {
      ownerMovementFlags = &entityRuntime1->movementStateFlags;
      *ownerMovementFlags = *ownerMovementFlags | 0x10;
    }
    goto ArmyArticulatedMovement_CommitPositionSuspensionAndTransforms;
  }
  if (((modelRuntime->articulatedContact).fallbackPosition0Q12 & 0x10U) == 0) {
    if ((modelRuntime->runtimeFlags & 8) == 0) {
      waypointResult = ArmyRuntime_UpdateMovementAndWaypoints
                         (worldRuntime,(ArmyMovementRuntime *)modelRuntime->ownerArmyRuntime);
      if (waypointResult.arrived) {
        commandTargetPosition = GameEntityRuntime_ResolveCommandTargetPosition
                           ((GameEntityRuntime *)modelRuntime->ownerArmyRuntime);
        x.signedValue = commandTargetPosition.worldZQ12;
        if (commandTargetPosition.unresolved) goto ArmyArticulatedMovement_SharedContinuation;
        targetAngleLength.angle =
             FixedMath_Atan2Angle16
                       (commandTargetPosition.worldYQ12 - (modelNode1->worldTransform).translation.y,
                        commandTargetPosition.worldXQ12 - (modelNode1->worldTransform).translation.x);
        targetAngleLength.length = 0xffffffff;
      }
      else {
        advanceOrDelta = waypointResult.worldXQ12 - (modelNode1->worldTransform).translation.x;
        component0 = waypointResult.worldYQ12 - (modelNode1->worldTransform).translation.y;
        if ((advanceOrDelta == 0) && (component0 == 0)) {
          targetAngleLength = THANDOR_BITCAST(uint64_t, FixedLengthAngleEaxEdx8, ((uint64_t)(modelNode1->modelPayload).worldRotationAngle2 << 0x20));
        }
        else {
          targetAngleLength = FixedMath_Vector2AngleAndLengthRegs(component0,advanceOrDelta);
        }
        x.signedValue = targetAngleLength.angle;
      }
      steeringAngle16 =
           (ModelRuntimeNode *)
           (targetAngleLength.angle - (modelNode1->modelPayload).worldRotationAngle2 & 0xffff);
      if (0 < (int)targetAngleLength.length) {
        if (((modelRuntime->articulatedContact).fallbackPosition0Q12 & 0x20U) == 0) {
          if ((steeringAngle16 < (ModelRuntimeNode *)0x2001) ||
             ((ModelRuntimeNode *)0xdfff < steeringAngle16)) {
ArmyArticulatedMovement_UpdateSelectedTerrainContact:
            if (((modelRuntime->articulatedContact).fallbackPosition0Q12 & 4U) == 0) {
              contactStateFlags = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
              *contactStateFlags = *contactStateFlags & 0xffa0;
              contactStateFlags = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
              *contactStateFlags = *contactStateFlags | 0x25;
              ArmyArticulatedRuntime_UpdateLeftTerrainContact
                        (targetAngleLength.angle,targetAngleLength.length,
                         (ArmyArticulatedRuntimeSlotView *)modelRuntime,worldRuntime);
            }
            else {
              contactStateFlags = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
              *contactStateFlags = *contactStateFlags & 0xffa0;
              contactStateFlags = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
              *contactStateFlags = *contactStateFlags | 0x2a;
              ArmyArticulatedRuntime_UpdateRightTerrainContact
                        (targetAngleLength.angle,targetAngleLength.length,
                         (ArmyArticulatedRuntimeSlotView *)modelRuntime,worldRuntime);
            }
            goto ArmyArticulatedMovement_CommitPositionSuspensionAndTransforms;
          }
        }
        else if ((steeringAngle16 < (ModelRuntimeNode *)0x201) ||
                ((ModelRuntimeNode *)0xfdff < steeringAngle16))
        goto ArmyArticulatedMovement_UpdateSelectedTerrainContact;
      }
      if (((modelRuntime->articulatedContact).fallbackPosition0Q12 & 0x20U) != 0)
      goto ArmyArticulatedMovement_InitializeSelectedTerrainContact;
      if (targetAngleLength.length < 0x41)
      goto ArmyArticulatedMovement_ClearContactTransitionAndAdvanceWaypoint;
      if ((-1 < (int)targetAngleLength.length) ||
         (((ModelRuntimeNode *)0x800 < steeringAngle16 &&
          (steeringAngle16 < (ModelRuntimeNode *)0xf800)))) {
        ArmyArticulatedRuntime_UpdateSelectedTerrainContact
                  ((AngleTurn32)steeringAngle16,(ArmyArticulatedRuntimeSlotView *)modelRuntime,
                   worldRuntime);
        contactStateFlags = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
        *contactStateFlags = *contactStateFlags & 0xffffffa0;
        if (steeringAngle16 < (ModelRuntimeNode *)0x8000) {
          contactStateFlags = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
          *contactStateFlags = *contactStateFlags | 0x15;
        }
        else {
          contactStateFlags = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
          *contactStateFlags = *contactStateFlags | 0x1a;
        }
        goto ArmyArticulatedMovement_CommitPositionSuspensionAndTransforms;
      }
    }
ArmyArticulatedMovement_SharedContinuation:
    if (((modelRuntime->articulatedContact).fallbackPosition0Q12 & 0x40U) == 0) {
      x.signedValue = modelRuntime->definitionClassValue80 - modelRuntime->definitionClassValue84;
      advanceOrHeading = FixedMath_Atan2Angle16
                        (x.signedValue,
                         modelRuntime->movementTarget0Q12 - modelRuntime->movementTarget1Q12);
      relativeHeading = (advanceOrHeading - (modelNode1->modelPayload).worldRotationAngle2) - 0x4000 & 0xffff;
      if ((0xfff < relativeHeading) && ((relativeHeading < 0x7000 || ((0x8fff < relativeHeading && (relativeHeading < 0xf000))))))
      goto ArmyArticulatedMovement_InitializeSelectedTerrainContact;
    }
    steeringAngle16 =
         (ModelRuntimeNode *)(modelRuntime->rootModelNode->worldTransform).translation.x;
    x = THANDOR_BITCAST(GraphicsWorldCoordinateQ12, ArmyRuntimeCoordinateCommandOrHistoryValue4, (modelRuntime->rootModelNode->worldTransform).translation.y);
    if ((worldRuntime->fieldGrid->runtimeStateFlags & 1) == 0) {
      return;
    }
  }
  else {
ArmyArticulatedMovement_InitializeSelectedTerrainContact:
    steeringAngle16 =
         (ModelRuntimeNode *)
         (((modelRuntime->articulatedContact).fallbackPosition0Q12 >> 0x10) +
          modelRuntime->ownerValue64 & 0xffff);
    if (((modelRuntime->articulatedContact).fallbackPosition0Q12 & 4U) == 0) {
      contactStateFlags = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
      *contactStateFlags = *contactStateFlags & 0xffffffc0;
      contactStateFlags = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
      *contactStateFlags = *contactStateFlags | 0x45;
      ArmyArticulatedRuntime_InitializeLeftTerrainContact
                ((AngleTurn16Stored32)steeringAngle16,(ArmyArticulatedRuntimeSlotView *)modelRuntime
                 ,worldRuntime);
    }
    else {
      contactStateFlags = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
      *contactStateFlags = *contactStateFlags & 0xffffffc0;
      contactStateFlags = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
      *contactStateFlags = *contactStateFlags | 0x4a;
      ArmyArticulatedRuntime_InitializeRightTerrainContact
                ((AngleTurn16Stored32)steeringAngle16,(ArmyArticulatedRuntimeSlotView *)modelRuntime
                 ,worldRuntime);
    }
  }
ArmyArticulatedMovement_CommitPositionSuspensionAndTransforms:
  modelNode1 = modelRuntime->rootModelNode;
  (modelNode1->worldTransform).translation.x = (GraphicsWorldCoordinateQ12)steeringAngle16;
  (modelNode1->worldTransform).translation.y = THANDOR_BITCAST(ArmyRuntimeCoordinateCommandOrHistoryValue4, GraphicsWorldCoordinateQ12, x);
  modelNode1->runtimeFlags = modelNode1->runtimeFlags | 1;
  ArmyArticulatedRuntime_UpdateSuspensionHierarchy(modelNode1,worldRuntime);
  if (((previousWorldX != (modelNode1->worldTransform).translation.x) ||
      (previousWorldY != (modelNode1->worldTransform).translation.y)) ||
     (previousRotationAngle != (modelNode1->modelPayload).worldRotationAngle2)) {
    entityRuntime1 = modelRuntime->ownerArmyRuntime;
    ArmyRuntime_UpdateTimedShotAndEffectEmitters
              (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
    ownerMovementFlags = &entityRuntime1->movementStateFlags;
    *ownerMovementFlags = *ownerMovementFlags & 0xfffffffb;
  }
  movementDefinition = modelRuntime->modelDefinition;
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNode1);
  ModelNodeRuntime_UpdateDepthBinMasks(movementDefinition->placementRadiusOrClearanceDC,modelNode1);
  return;
}


/* Address: 0x0051C5A0.
   Makes targetRuntime the army's command target: a running movement (movement flag 0x20) is reset first, then
   the army starts a route to the target's current world position under the standard command generation.
   A null target clears the command instead.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_ResolveCommandTargetAndRoute
          (GameEntityRuntime *targetRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ArmyCommandGeneration standardGeneration;
  ModelRuntimeNode *targetModelNode;

  standardGeneration = g_ArmyCommandGenerationStandard;
  if ((armyRuntime->movementStateFlags & 0x20) != 0) {
    /* command mode 8 is set by the AI combat target selection */
    if ((armyRuntime->commandModeFlags & 8) == 0) {
      ArmyRuntime_ResetMovementStatePreserveQueuedTarget((ArmyMovementRuntime *)armyRuntime);
    }
    else {
      ArmyRuntime_ResetMovementStateFromCurrentPosition((ArmyMovementRuntime *)armyRuntime);
    }
  }
  if (targetRuntime == NULL) {
    armyRuntime->commandModeFlags = 0;
    armyRuntime->commandGeneration = 0;
  }
  else {
    armyRuntime->commandGeneration = standardGeneration;
    armyRuntime->commandModeFlags = 1;
    targetModelNode = (targetRuntime->common).ownership.modelNode;
    ArmyRuntime_StartMoveCommandWithFallbackWaypoints
              ((targetModelNode->worldTransform).translation.y,(targetModelNode->worldTransform).translation.x
               ,(ArmyMovementRuntime *)armyRuntime);
  }
  armyRuntime->commandTargetArmyRuntime = (ArmyRuntimeSlot *)targetRuntime;
  return;
}


/* Address: 0x00520DF0.
   Ownership: gameplay/army/movement.
   Purpose: Binary entry is anchored by g_CodePointerTable_0051FC98[18]@0051FC98. Runtime-update partition slots
   0-23 receive (worldRuntime, armyRuntime).
   Local calls: ArmyRuntimeClass_UpdateGroundMovementVariantB, ArmyRuntimeClass_UpdateGroundMovementVariantA.
   Cross-module calls: AiUnitBehavior_UpdateSpecialClass12Entity [gameplay/ai/units].
*/

void __thandor_preserve_eax_edx
ArmyRuntimeClass_UpdateSpecialBehaviorAndGroundMovement
          (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView200 *modelRuntime
          )

{
  ArmyRuntimeSlot *armyRuntime;
  ArmyPlacementContactKindIndex32 movementVariant;
  
  armyRuntime = modelRuntime->ownerArmyRuntime;
  movementVariant = *(ArmyPlacementContactKindIndex32 *)
           (modelRuntime->modelDefinition->reserved1C0_253 + 0xb8);
  if ((armyRuntime->movementStateFlags & 0x100) != 0) {
    AiUnitBehavior_UpdateSpecialClass12Entity
              ((MdlDefinitionSemanticPrefix80 *)modelRuntime->modelDefinition,armyRuntime,
               armyRuntime->factionIndex,worldRuntime);
  }
  if (movementVariant == 1) {
    ArmyRuntimeClass_UpdateGroundMovementVariantB(worldRuntime,modelRuntime);
  }
  else {
    ArmyRuntimeClass_UpdateGroundMovementVariantA(worldRuntime,modelRuntime);
  }
  return;
}


/* Address: 0x00523410.
   Ownership: gameplay/army/movement.
   Purpose: Binary entry is anchored by g_CodePointerTable_0051FC98[7]@0051FC98. Runtime-update partition slots
   0-23 receive (worldRuntime, armyRuntime). Role: Class-specific update combining movement, aim and
   projectile/effect processing variant A. Inputs: World context and army runtime state. Outputs: Updated
   movement/aim/model state and possible emitted projectiles/effects.
   Local calls: ArmyRuntime_UpdateMovementAndWaypoints, ArmyRuntimeCommand_UpdateTargetFollowingState.
   Cross-module calls: FixedVector_StepBackwardAlongOwnDirection [core/math/fixed],
   ArmyRuntime_ResolveShotAimPoint [gameplay/army/runtime], ModelNodeRuntime_SmoothYawTowardTarget
   [world/model/hierarchy], ModelNodeRuntime_SmoothPitchTowardTarget [world/model/hierarchy],
   ShotDefinition_ComputeLaunchAnglesRegs [assets/shot/catalog], ModelNodeRuntime_ComputeRelativeDirectionAngle
   [world/model/hierarchy].
*/

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_UpdateMovementAimAndProjectilesVariantA
          (WorldRuntimeContext *worldRuntime,ModelRuntimeWeaponAimStateView200 *modelRuntime)

{
  ModelRuntimeFlags *nodeFlags;
  WeaponAimCountdownTicks *countdownTicks;
  AngleTurn32 *rotationAngle;
  ArmyWeaponDefinitionView68 *weaponDefinition;
  int backwardStepCountdown;
  FixedMathScale32 backwardStepScale;
  ModelRuntimeNode *modelNodeRuntime;
  uint32_t stepMultiplier;
  ArmyRuntimeSlot *commandTargetArmy;
  InGameSimulationStepBatchTicks elapsedTicks;
  Q12 point0Z;
  Q12 point0Y;
  Q12 point0X;
  AngleTurn32 targetPitchAngle16;
  ShotRuntimeState14 shotRuntimeState14;
  bool followingHandled;
  ShotLaunchAnglesEaxEdx8 launchAngles;
  ModelRelativeDirectionAnglesEaxEdx8 relativeAngles;
  AimSmoothResult smoothResult;
  MovementStepResult waypointResult;
  WorldPositionResult aimPoint;
  GameEntityRuntime *entityRuntime1;
  ModelRuntimeNode *modelNode2;
  
  elapsedTicks = g_InGameSimulationStepTicks;
  if (((modelRuntime->classState).classStateEC & 9) == 0) {
    weaponDefinition = modelRuntime->modelDefinition;
    entityRuntime1 = (GameEntityRuntime *)modelRuntime->ownerArmyRuntime;
    backwardStepCountdown = modelRuntime->attachment0BackwardStepCountdownTicks28;
    if (modelRuntime->attachmentReloadCountdownTicks24 != 0) {
      modelNode2 = modelRuntime->rootModelNode->childNodes[0];
      countdownTicks = &modelRuntime->attachmentReloadCountdownTicks24;
      *countdownTicks = *countdownTicks - g_InGameSimulationStepTicks;
      if (*countdownTicks < 0) {
        elapsedTicks = elapsedTicks + modelRuntime->attachmentReloadCountdownTicks24;
        modelRuntime->attachmentReloadCountdownTicks24 = 0;
      }
      modelNode2 = modelNode2->childNodes[0];
      rotationAngle = &(modelNode2->modelPayload).localRotationAngle2;
      *rotationAngle = *rotationAngle + elapsedTicks * weaponDefinition->localRotationAngle2StepPerTick34;
      modelNode2->runtimeFlags = modelNode2->runtimeFlags | 1;
      rotationAngle = &(modelNode2->modelPayload).localRotationAngle2;
      *rotationAngle = *rotationAngle & 0xffff;
    }
    elapsedTicks = g_InGameSimulationStepTicks;
    if (backwardStepCountdown != 0) {
      backwardStepScale = weaponDefinition->backwardStepScale3C;
      modelNode2 = modelRuntime->rootModelNode->childNodes[0];
      countdownTicks = &modelRuntime->attachment0BackwardStepCountdownTicks28;
      *countdownTicks = *countdownTicks - g_InGameSimulationStepTicks;
      if (*countdownTicks < 0) {
        elapsedTicks = elapsedTicks + modelRuntime->attachment0BackwardStepCountdownTicks28;
        modelRuntime->attachment0BackwardStepCountdownTicks28 = 0;
      }
      modelNode2 = modelNode2->childNodes[0];
      FixedVector_StepBackwardAlongOwnDirection(-elapsedTicks,backwardStepScale,(FixedVectorStateAddress32)modelNode2);
      nodeFlags = &modelNode2->runtimeFlags;
      *nodeFlags = *nodeFlags | 1;
    }
    modelNode2 = modelRuntime->rootModelNode;
    aimPoint = ArmyRuntime_ResolveShotAimPoint
                       ((modelNode2->worldTransform).translation.z,
                        (modelNode2->worldTransform).translation.y,
                        (modelNode2->worldTransform).translation.x,weaponDefinition->shotDefinition,
                        entityRuntime1);
    point0X = aimPoint.worldZQ12;
    point0Y = aimPoint.worldYQ12;
    point0Z = aimPoint.worldXQ12;
    if (aimPoint.unresolved) {
      waypointResult = ArmyRuntime_UpdateMovementAndWaypoints
                         (worldRuntime,(ArmyMovementRuntime *)entityRuntime1);
      if (((!waypointResult.arrived) || (modelRuntime->pitchTurnVelocityAngle16 != 0)) ||
         (modelRuntime->yawTurnVelocityAngle16 != 0)) {
        modelNode2 = modelRuntime->rootModelNode;
        ModelNodeRuntime_SmoothYawTowardTarget(modelNode2,modelRuntime,0);
        ModelNodeRuntime_SmoothPitchTowardTarget(modelNode2->childNodes[0],modelRuntime,0);
      }
    }
    else {
      modelNode2 = modelRuntime->rootModelNode;
      modelNodeRuntime = modelNode2->childNodes[0];
      launchAngles = ShotDefinition_ComputeLaunchAnglesRegs
                         (point0X,point0Y,point0Z,(modelNodeRuntime->worldTransform).translation.z,
                          (modelNodeRuntime->worldTransform).translation.y,
                          (modelNodeRuntime->worldTransform).translation.x,weaponDefinition->shotDefinition);
      relativeAngles = ModelNodeRuntime_ComputeRelativeDirectionAngle
                         (modelNode2,launchAngles.elevationAngle,launchAngles.headingAngle);
      targetPitchAngle16 = relativeAngles.relativePitchAngle;
      smoothResult = ModelNodeRuntime_SmoothYawTowardTarget
                         (modelNode2,modelRuntime,relativeAngles.relativeYawAngle);
      if (smoothResult.outsideTolerance) {
        ModelNodeRuntime_SmoothPitchTowardTarget(modelNodeRuntime,modelRuntime,targetPitchAngle16);
      }
      else {
        smoothResult = ModelNodeRuntime_SmoothPitchTowardTarget
                           (modelNodeRuntime,modelRuntime,targetPitchAngle16);
        if ((smoothResult.value == targetPitchAngle16) &&
           (weaponDefinition = modelRuntime->modelDefinition,
           modelRuntime->attachmentReloadCountdownTicks24 == 0)) {
          followingHandled = ArmyRuntimeCommand_UpdateTargetFollowingState
                            (point0X,point0Y,point0Z,worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
          if (!followingHandled) {
            stepMultiplier = weaponDefinition->sharedInterShotTicks;
            backwardStepScale = weaponDefinition->backwardStepScale3C;
            modelRuntime->attachmentReloadCountdownTicks24 =
                 modelRuntime->attachmentReloadCountdownTicks24 + weaponDefinition->attachmentReloadTicks;
            modelRuntime->attachment0BackwardStepCountdownTicks28 =
                 modelRuntime->attachment0BackwardStepCountdownTicks28 + stepMultiplier;
            modelNode2 = modelNodeRuntime->childNodes[0];
            FixedVector_StepBackwardAlongOwnDirection
                      (stepMultiplier,backwardStepScale,(FixedVectorStateAddress32)modelNode2);
            modelNode2->runtimeFlags = modelNode2->runtimeFlags | 1;
            ArmyRuntime_SetNonzeroActionVector
                      (launchAngles.headingAngle,weaponDefinition->postLaunchVector1Q12,weaponDefinition->postLaunchVector0Q12
                       ,modelRuntime->ownerArmyRuntime);
            commandTargetArmy = modelRuntime->ownerArmyRuntime->commandTargetArmyRuntime;
            shotRuntimeState14 = 0;
            if (commandTargetArmy != (ArmyRuntimeSlot *)0x0) {
              shotRuntimeState14 = (commandTargetArmy->modelRuntimeOrSavedOffset).savedIdOrOffset;
            }
            ModelRuntime_EmitProjectilesFromAttachmentPoints
                      (shotRuntimeState14,point0X,point0Y,point0Z,weaponDefinition->shotDefinition,modelNode2,
                       *(MdlSerializedNodeHeader38 **)
                        (weaponDefinition->modelPointSource64->childSerializedOffsets[0] + 0x18),worldRuntime)
            ;
          }
        }
      }
    }
  }
  ModelNodeRuntime_RebuildTransformsFromRoot(modelRuntime->rootModelNode);
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  return;
}


/* Address: 0x00523690.
   Ownership: gameplay/army/movement.
   Purpose: Binary entry is anchored by g_CodePointerTable_0051FC98[8]@0051FC98. Runtime-update partition slots
   0-23 receive (worldRuntime, armyRuntime). Role: Class-specific update combining movement, aim and
   projectile/effect processing variant B. Inputs: World context and army runtime state. Outputs: Updated
   movement/aim/model state and possible emitted projectiles/effects.
   Local calls: ArmyRuntime_UpdateMovementAndWaypoints, ArmyRuntimeCommand_UpdateTargetFollowingState.
   Cross-module calls: FixedVector_StepBackwardAlongOwnDirection [core/math/fixed],
   ArmyRuntime_ResolveShotAimPoint [gameplay/army/runtime], ModelNodeRuntime_SmoothYawTowardTarget
   [world/model/hierarchy], ModelNodeRuntime_SmoothPitchTowardTarget [world/model/hierarchy],
   ShotDefinition_ComputeLaunchAnglesRegs [assets/shot/catalog], ModelNodeRuntime_ComputeRelativeDirectionAngle
   [world/model/hierarchy].
*/

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_UpdateMovementAimAndProjectilesVariantB
          (WorldRuntimeContext *worldRuntime,ModelRuntimeWeaponAimStateView200 *modelRuntime)

{
  ModelRuntimeFlags *nodeFlags;
  WeaponAimCountdownTicks *countdownTicks;
  AngleTurn32 *rotationAngle;
  ArmyWeaponDefinitionView68 *weaponDefinition;
  FixedMathScale32 backwardStepScale;
  ModelRuntimeNode *modelNodeRuntime;
  uint32_t stepMultiplier;
  ArmyRuntimeSlot *commandTargetArmy;
  InGameSimulationStepBatchTicks elapsedTicks;
  Q12 point0Z;
  int countdownOrPointOffset;
  Q12 point0Y;
  Q12 point0X;
  AngleTurn32 targetPitchAngle16;
  ShotRuntimeState14 shotRuntimeState14;
  bool followingHandled;
  ShotLaunchAnglesEaxEdx8 launchAngles;
  ModelRelativeDirectionAnglesEaxEdx8 relativeAngles;
  AimSmoothResult smoothResult;
  MovementStepResult waypointResult;
  WorldPositionResult aimPoint;
  GameEntityRuntime *entityRuntime1;
  ModelRuntimeNode *modelNode2;
  
  elapsedTicks = g_InGameSimulationStepTicks;
  if (((modelRuntime->classState).classStateEC & 9) == 0) {
    weaponDefinition = modelRuntime->modelDefinition;
    entityRuntime1 = (GameEntityRuntime *)modelRuntime->ownerArmyRuntime;
    countdownOrPointOffset = modelRuntime->attachment0BackwardStepCountdownTicks28;
    if (modelRuntime->attachmentReloadCountdownTicks24 != 0) {
      modelNode2 = modelRuntime->rootModelNode->childNodes[0];
      countdownTicks = &modelRuntime->attachmentReloadCountdownTicks24;
      *countdownTicks = *countdownTicks - g_InGameSimulationStepTicks;
      if (*countdownTicks < 0) {
        elapsedTicks = elapsedTicks + modelRuntime->attachmentReloadCountdownTicks24;
        modelRuntime->attachmentReloadCountdownTicks24 = 0;
      }
      modelNode2 = modelNode2->childNodes[0];
      rotationAngle = &(modelNode2->modelPayload).localRotationAngle2;
      *rotationAngle = *rotationAngle + elapsedTicks * weaponDefinition->localRotationAngle2StepPerTick34;
      modelNode2->runtimeFlags = modelNode2->runtimeFlags | 1;
      rotationAngle = &(modelNode2->modelPayload).localRotationAngle2;
      *rotationAngle = *rotationAngle & 0xffff;
    }
    elapsedTicks = g_InGameSimulationStepTicks;
    if (countdownOrPointOffset != 0) {
      backwardStepScale = weaponDefinition->backwardStepScale3C;
      modelNode2 = modelRuntime->rootModelNode->childNodes[0];
      countdownTicks = &modelRuntime->attachment0BackwardStepCountdownTicks28;
      *countdownTicks = *countdownTicks - g_InGameSimulationStepTicks;
      if (*countdownTicks < 0) {
        elapsedTicks = elapsedTicks + modelRuntime->attachment0BackwardStepCountdownTicks28;
        modelRuntime->attachment0BackwardStepCountdownTicks28 = 0;
      }
      modelNode2 = modelNode2->childNodes[0];
      FixedVector_StepBackwardAlongOwnDirection(-elapsedTicks,backwardStepScale,(FixedVectorStateAddress32)modelNode2);
      nodeFlags = &modelNode2->runtimeFlags;
      *nodeFlags = *nodeFlags | 1;
    }
    elapsedTicks = g_InGameSimulationStepTicks;
    if (modelRuntime->attachment1BackwardStepCountdownTicks2C != 0) {
      backwardStepScale = weaponDefinition->backwardStepScale3C;
      modelNode2 = modelRuntime->rootModelNode->childNodes[0];
      countdownTicks = &modelRuntime->attachment1BackwardStepCountdownTicks2C;
      *countdownTicks = *countdownTicks - g_InGameSimulationStepTicks;
      if (*countdownTicks < 0) {
        elapsedTicks = elapsedTicks + modelRuntime->attachment1BackwardStepCountdownTicks2C;
        modelRuntime->attachment1BackwardStepCountdownTicks2C = 0;
      }
      modelNode2 = modelNode2->childNodes[1];
      FixedVector_StepBackwardAlongOwnDirection(-elapsedTicks,backwardStepScale,(FixedVectorStateAddress32)modelNode2);
      nodeFlags = &modelNode2->runtimeFlags;
      *nodeFlags = *nodeFlags | 1;
    }
    modelNode2 = modelRuntime->rootModelNode;
    aimPoint = ArmyRuntime_ResolveShotAimPoint
                       ((modelNode2->worldTransform).translation.z,
                        (modelNode2->worldTransform).translation.y,
                        (modelNode2->worldTransform).translation.x,weaponDefinition->shotDefinition,
                        entityRuntime1);
    point0X = aimPoint.worldZQ12;
    point0Y = aimPoint.worldYQ12;
    point0Z = aimPoint.worldXQ12;
    if (aimPoint.unresolved) {
      waypointResult = ArmyRuntime_UpdateMovementAndWaypoints
                         (worldRuntime,(ArmyMovementRuntime *)entityRuntime1);
      if (((!waypointResult.arrived) || (modelRuntime->pitchTurnVelocityAngle16 != 0)) ||
         (modelRuntime->yawTurnVelocityAngle16 != 0)) {
        modelNode2 = modelRuntime->rootModelNode;
        ModelNodeRuntime_SmoothYawTowardTarget(modelNode2,modelRuntime,0);
        ModelNodeRuntime_SmoothPitchTowardTarget(modelNode2->childNodes[0],modelRuntime,0);
      }
    }
    else {
      modelNode2 = modelRuntime->rootModelNode;
      modelNodeRuntime = modelNode2->childNodes[0];
      launchAngles = ShotDefinition_ComputeLaunchAnglesRegs
                         (point0X,point0Y,point0Z,(modelNodeRuntime->worldTransform).translation.z,
                          (modelNodeRuntime->worldTransform).translation.y,
                          (modelNodeRuntime->worldTransform).translation.x,weaponDefinition->shotDefinition);
      relativeAngles = ModelNodeRuntime_ComputeRelativeDirectionAngle
                         (modelNode2,launchAngles.elevationAngle,launchAngles.headingAngle);
      targetPitchAngle16 = relativeAngles.relativePitchAngle;
      smoothResult = ModelNodeRuntime_SmoothYawTowardTarget
                         (modelNode2,modelRuntime,relativeAngles.relativeYawAngle);
      if (smoothResult.outsideTolerance) {
        ModelNodeRuntime_SmoothPitchTowardTarget(modelNodeRuntime,modelRuntime,targetPitchAngle16);
      }
      else {
        smoothResult = ModelNodeRuntime_SmoothPitchTowardTarget
                           (modelNodeRuntime,modelRuntime,targetPitchAngle16);
        if ((smoothResult.value == targetPitchAngle16) &&
           (weaponDefinition = modelRuntime->modelDefinition,
           modelRuntime->attachmentReloadCountdownTicks24 == 0)) {
          followingHandled = ArmyRuntimeCommand_UpdateTargetFollowingState
                            (point0X,point0Y,point0Z,worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
          if (!followingHandled) {
            stepMultiplier = weaponDefinition->sharedInterShotTicks;
            backwardStepScale = weaponDefinition->backwardStepScale3C;
            modelRuntime->attachmentReloadCountdownTicks24 =
                 modelRuntime->attachmentReloadCountdownTicks24 + weaponDefinition->attachmentReloadTicks;
            if ((modelRuntime->alternatingAttachmentSequence20 & 1) == 0) {
              modelRuntime->attachment1BackwardStepCountdownTicks2C =
                   modelRuntime->attachment1BackwardStepCountdownTicks2C + stepMultiplier;
              modelNode2 = modelNodeRuntime->childNodes[1];
              countdownOrPointOffset = 4;
            }
            else {
              modelRuntime->attachment0BackwardStepCountdownTicks28 =
                   modelRuntime->attachment0BackwardStepCountdownTicks28 + stepMultiplier;
              modelNode2 = modelNodeRuntime->childNodes[0];
              countdownOrPointOffset = 0;
            }
            modelRuntime->alternatingAttachmentSequence20 =
                 modelRuntime->alternatingAttachmentSequence20 + 1;
            FixedVector_StepBackwardAlongOwnDirection
                      (stepMultiplier,backwardStepScale,(FixedVectorStateAddress32)modelNode2);
            modelNode2->runtimeFlags = modelNode2->runtimeFlags | 1;
            ArmyRuntime_SetNonzeroActionVector
                      (launchAngles.headingAngle,weaponDefinition->postLaunchVector1Q12,weaponDefinition->postLaunchVector0Q12
                       ,modelRuntime->ownerArmyRuntime);
            commandTargetArmy = modelRuntime->ownerArmyRuntime->commandTargetArmyRuntime;
            shotRuntimeState14 = 0;
            if (commandTargetArmy != (ArmyRuntimeSlot *)0x0) {
              shotRuntimeState14 = (commandTargetArmy->modelRuntimeOrSavedOffset).savedIdOrOffset;
            }
            ModelRuntime_EmitProjectilesFromAttachmentPoints
                      (shotRuntimeState14,point0X,point0Y,point0Z,weaponDefinition->shotDefinition,modelNode2,
                       *(MdlSerializedNodeHeader38 **)
                        (countdownOrPointOffset + weaponDefinition->modelPointSource64->childSerializedOffsets[0] + 0x18),
                       worldRuntime);
          }
        }
      }
    }
  }
  ModelNodeRuntime_RebuildTransformsFromRoot(modelRuntime->rootModelNode);
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  return;
}


/* Address: 0x00520140.
   Ownership: gameplay/army/movement.
   Purpose: Table membership RUNTIME_UPDATE[2]. Updates waypoint-driven ground movement, turn acceleration, terrain
   and runtime collision response, model orientation, track-texture animation, and timed shot/effect emitters.
   Runtime-update partition slots 0-23 receive (worldRuntime, armyRuntime).
   Local calls: ArmyRuntime_UpdateMovementAndWaypoints.
   Cross-module calls: ArmyCollision_TestPointWithinExpandedRuntimeRadius [gameplay/army/placement],
   FieldGrid_InterpolateWaterDelta [world/terrain/grid], ArmyRuntime_ApplyDamageAndPropagateToParent
   [gameplay/army/combat], FixedMath_Vector2AngleAndLengthRegs [core/math/fixed],
   ArmyRuntime_UpdateActivationMetricAndPlayStartSound [gameplay/army/runtime], FixedTrig_ProjectPlanarPointRegs
   [core/math/fixed].
*/

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_UpdateGroundMovementCollisionAndTrackAnimation
          (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementTrackView200 *modelRuntime)

{
  ArmyMovementStateFlags *ownerMovementFlags;
  ArmyRuntimeSlot *linkedOrOwnerArmy;
  AngleTurn32 previousRotationAngle;
  int worldX;
  int worldY;
  ModelDefinitionGroundMovementTrackView280 *movementDefinition;
  ArmyPlacementContactKindIndex32 placementContactKind;
  ModelResourceHitTestAndRenderView210 *nodeModelResource;
  int32_t waterDelta;
  int primaryDelta;
  uint32_t facingAngle;
  uint32_t trackDistance;
  uint32_t turnVelocityOrLimit;
  int secondaryDelta;
  uint32_t headingDifference;
  uint32_t desiredHeading;
  ModelRuntimeNode *modelNode1;
  bool withinLinkRadius;
  FixedLengthAngleEaxEdx8 angleAndLength;
  FixedPlanarPointEdxEax8 nextPosition;
  FixedSinCosEdxEax8 sinCosOffset;
  ArmyCollisionResult blockingCollision;
  MovementStepResult waypointResult;
  FixedEulerAnglesEaxEbxEdx12 composedAngles;
  Q12 heightOffsetQ12;
  WorldRuntimeContext *dispatchWorldRuntime;
  uint32_t targetDistance;
  ModelRuntimeNode *modelNode2;
  
  linkedOrOwnerArmy = (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime;
  modelNode2 = modelRuntime->rootModelNode;
  ownerMovementFlags = &modelRuntime->ownerArmyRuntime->movementStateFlags;
  *ownerMovementFlags = *ownerMovementFlags | 4;
  /* Drop the linked runtime unless both have a clearance radius and this unit is still within it. */
  if ((linkedOrOwnerArmy != (ArmyRuntimeSlot *)0x0) &&
      ((((((linkedOrOwnerArmy->modelRuntimeOrSavedOffset).modelRuntime)->classState).classStateDC == 0) ||
        (modelRuntime->modelDefinition->placementRadiusOrClearanceDC == 0)) ||
       (withinLinkRadius = ArmyCollision_TestPointWithinExpandedRuntimeRadius
                           (modelRuntime->modelDefinition->placementRadiusOrClearanceDC,
                            (modelNode2->worldTransform).translation.y,
                            (modelNode2->worldTransform).translation.x,linkedOrOwnerArmy), !withinLinkRadius))) {
    (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime = (ArmyRuntimeSlot *)0x0;
  }
  previousRotationAngle = (modelNode2->modelPayload).worldRotationAngle2;
  worldX = (modelNode2->worldTransform).translation.x;
  worldY = (modelNode2->worldTransform).translation.y;
  movementDefinition = modelRuntime->modelDefinition;
  waterDelta = FieldGrid_InterpolateWaterDelta(worldY,worldX,worldRuntime->fieldGrid);
  if ((movementDefinition->waterDamageThreshold198 < waterDelta) &&
     (primaryDelta = waterDelta * movementDefinition->waterDamageMultiplier194 >> 7, -1 < primaryDelta)) {
    ArmyRuntime_ApplyDamageAndPropagateToParent(primaryDelta,(ArmyRuntimeSlot *)modelRuntime);
  }
  if (((modelRuntime->classState).classStateEC & 8) == 0) {
    waypointResult = ArmyRuntime_UpdateMovementAndWaypoints
                       (worldRuntime,(ArmyMovementRuntime *)modelRuntime->ownerArmyRuntime);
    if (waypointResult.arrived) goto ArmyGroundMovementCollision_StopMovementAndTurn;
    secondaryDelta = waypointResult.worldYQ12 - (modelNode2->worldTransform).translation.y;
    primaryDelta = waypointResult.worldXQ12 - (modelNode2->worldTransform).translation.x;
    if ((primaryDelta == 0) && (secondaryDelta == 0)) {
      angleAndLength = THANDOR_BITCAST(uint64_t, FixedLengthAngleEaxEdx8, ((uint64_t)(modelNode2->modelPayload).worldRotationAngle2 << 0x20));
    }
    else {
      angleAndLength = FixedMath_Vector2AngleAndLengthRegs(secondaryDelta,primaryDelta);
    }
    desiredHeading = angleAndLength.angle;
    movementDefinition = modelRuntime->modelDefinition;
    facingAngle = (modelNode2->modelPayload).worldRotationAngle2;
    turnVelocityOrLimit = (modelRuntime->movementControl).turnVelocityAngle16;
    headingDifference = desiredHeading - facingAngle & 0xffff;
    if (headingDifference < 0x8000) {
      if ((int)turnVelocityOrLimit < 0) {
ArmyGroundMovementCollision_ResetTurnVelocityForDirectionReversal:
        (modelRuntime->movementControl).turnVelocityAngle16 = 0;
      }
      else if (turnVelocityOrLimit < headingDifference) {
        facingAngle = facingAngle + turnVelocityOrLimit;
        secondaryDelta = movementDefinition->turnRateLimitAnglePerTick10 * g_InGameSimulationStepTicks;
        primaryDelta = turnVelocityOrLimit + movementDefinition->turnRateAccelerationAnglePerTick1C * g_InGameSimulationStepTicks;
        (modelRuntime->movementControl).turnVelocityAngle16 = secondaryDelta;
        if (primaryDelta < secondaryDelta) {
          (modelRuntime->movementControl).turnVelocityAngle16 = primaryDelta;
        }
      }
      else {
ArmyGroundMovementCollision_SnapFacingToDesiredHeading:
        (modelRuntime->movementControl).turnVelocityAngle16 = 0;
        facingAngle = desiredHeading;
      }
    }
    else {
      if (0 < (int)turnVelocityOrLimit) goto ArmyGroundMovementCollision_ResetTurnVelocityForDirectionReversal;
      if (turnVelocityOrLimit + 0x10000 <= headingDifference) goto ArmyGroundMovementCollision_SnapFacingToDesiredHeading;
      facingAngle = facingAngle + turnVelocityOrLimit;
      secondaryDelta = -movementDefinition->turnRateLimitAnglePerTick10 * g_InGameSimulationStepTicks;
      primaryDelta = turnVelocityOrLimit - movementDefinition->turnRateAccelerationAnglePerTick1C * g_InGameSimulationStepTicks;
      (modelRuntime->movementControl).turnVelocityAngle16 = secondaryDelta;
      if (secondaryDelta < primaryDelta) {
        (modelRuntime->movementControl).turnVelocityAngle16 = primaryDelta;
      }
    }
    modelNode2 = modelRuntime->rootModelNode;
    facingAngle = facingAngle & 0xffff;
    if (facingAngle != (modelNode2->modelPayload).worldRotationAngle2) {
      (modelNode2->modelPayload).worldRotationAngle2 = facingAngle;
      modelNode2->runtimeFlags = modelNode2->runtimeFlags | 1;
    }
    targetDistance = angleAndLength.length;
    turnVelocityOrLimit = movementDefinition->farHeadingErrorLimitAngleC4;
    facingAngle = facingAngle - desiredHeading & 0xffff;
    if ((int)targetDistance < movementDefinition->headingErrorInterpolationDistanceQ12C0) {
      turnVelocityOrLimit = movementDefinition->nearHeadingErrorLimitAngleC8 +
               (int)(((int64_t)(int)(turnVelocityOrLimit - movementDefinition->nearHeadingErrorLimitAngleC8) *
                     (int64_t)(int)targetDistance) /
                    (int64_t)movementDefinition->headingErrorInterpolationDistanceQ12C0);
    }
    if ((turnVelocityOrLimit < facingAngle) && (facingAngle < 0x10000 - turnVelocityOrLimit)) {
      (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
      goto ArmyGroundMovementCollision_ProcessStationaryPlacementAndDamageState;
    }
    ArmyRuntime_UpdateActivationMetricAndPlayStartSound
              (worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
    primaryDelta = (modelRuntime->movementControl).movementAdvancePerTickQ12 * g_InGameSimulationStepTicks;
    if (primaryDelta < (int)targetDistance >> 1) {
      nextPosition = FixedTrig_ProjectPlanarPointRegs
                         (primaryDelta,(modelNode2->modelPayload).worldRotationAngle2,
                          (modelNode2->worldTransform).translation.y,
                          (modelNode2->worldTransform).translation.x);
    }
    else {
      linkedOrOwnerArmy = modelRuntime->ownerArmyRuntime;
      waypointResult = ArmyRuntime_UpdateMovementAndWaypoints(worldRuntime,(ArmyMovementRuntime *)linkedOrOwnerArmy);
      nextPosition = THANDOR_PART(uint64_t, waypointResult, 0);
      ownerMovementFlags = &linkedOrOwnerArmy->movementStateFlags;
      *ownerMovementFlags = *ownerMovementFlags | 0x10;
    }
    placementContactKind = movementDefinition->placementContactKindIndex278;
    modelNode2 = modelRuntime->rootModelNode;
    heightOffsetQ12 = movementDefinition->placementHeightOffsetQ12;
    dispatchWorldRuntime = worldRuntime;
    blockingCollision = ArmyCollision_FindBlockingRuntimeForCurrentUnit
                       ((Q12)(nextPosition >> 0x20),(Q12)nextPosition,
                        (RuntimeCollisionQueryViewF4 *)modelRuntime,worldRuntime);
    if (blockingCollision.blocked) {
      modelNode1 = modelRuntime->rootModelNode;
      ArmyRuntime_HandleCollisionPartner
                ((ArmyRuntimeSlot *)modelRuntime,(modelNode1->worldTransform).translation.y,
                 (modelNode1->worldTransform).translation.x,(ArmyRuntimeSlot *)blockingCollision.blockingArmy,
                 worldRuntime);
      (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
      THANDOR_PART(uint32_t, nextPosition, 0) = (modelNode1->worldTransform).translation.x;
      THANDOR_PART(uint32_t, nextPosition, 4) = (modelNode1->worldTransform).translation.y;
      ownerMovementFlags = &modelRuntime->ownerArmyRuntime->movementStateFlags;
      *ownerMovementFlags = *ownerMovementFlags | 0x10;
    }
    linkedOrOwnerArmy = modelRuntime->ownerArmyRuntime;
    modelNode1 = modelRuntime->rootModelNode;
    g_ArmyPlacementContactKindDispatchTable.callbacks[placementContactKind]
              (heightOffsetQ12,(Q12)(nextPosition >> 0x20),(Q12)nextPosition,modelNode2,dispatchWorldRuntime);
    primaryDelta = linkedOrOwnerArmy->actionVector1Q12 + -1;
    if (primaryDelta < 0) goto ArmyGroundMovementCollision_FinalizeEffectsAnimationAndTransforms;
    primaryDelta = primaryDelta * linkedOrOwnerArmy->actionVector2Q12;
    linkedOrOwnerArmy->actionVector1Q12 = linkedOrOwnerArmy->actionVector1Q12 + -1;
  }
  else {
ArmyGroundMovementCollision_StopMovementAndTurn:
    (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
    (modelRuntime->movementControl).turnVelocityAngle16 = 0;
ArmyGroundMovementCollision_ProcessStationaryPlacementAndDamageState:
    linkedOrOwnerArmy = modelRuntime->ownerArmyRuntime;
    modelNode1 = modelRuntime->rootModelNode;
    primaryDelta = linkedOrOwnerArmy->actionVector1Q12 + -1;
    if (primaryDelta < 0) {
      if ((worldRuntime->fieldGrid->runtimeStateFlags & 1) != 0) {
        (*g_ArmyPlacementContactKindDispatchTable.callbacks
          [modelRuntime->modelDefinition->placementContactKindIndex278])
                  (modelRuntime->modelDefinition->placementHeightOffsetQ12,
                   (modelNode1->worldTransform).translation.y,
                   (modelNode1->worldTransform).translation.x,modelNode1,worldRuntime);
      }
      goto ArmyGroundMovementCollision_FinalizeEffectsAnimationAndTransforms;
    }
    primaryDelta = primaryDelta * linkedOrOwnerArmy->actionVector2Q12;
    linkedOrOwnerArmy->actionVector1Q12 = linkedOrOwnerArmy->actionVector1Q12 + -1;
    (*g_ArmyPlacementContactKindDispatchTable.callbacks
      [modelRuntime->modelDefinition->placementContactKindIndex278])
              (modelRuntime->modelDefinition->placementHeightOffsetQ12,
               (modelNode1->worldTransform).translation.y,(modelNode1->worldTransform).translation.x
               ,modelNode1,worldRuntime);
  }
  composedAngles = FixedTransform_ComposeEulerAnglesRegs
                     (0,0x4000 - primaryDelta,
                      (modelRuntime->ownerArmyRuntime->actionVector0Q12 + 0x8000) -
                      (modelNode1->modelPayload).worldRotationAngle2 & 0xffff,
                      (modelNode1->modelPayload).worldRotationAngle2,
                      (modelNode1->modelPayload).worldRotationAngle1,
                      (modelNode1->modelPayload).worldRotationAngle0);
  (modelNode1->modelPayload).worldRotationAngle0 = composedAngles.angle0;
  (modelNode1->modelPayload).worldRotationAngle1 = composedAngles.angle1;
  (modelNode1->modelPayload).worldRotationAngle2 = composedAngles.angle2;
ArmyGroundMovementCollision_FinalizeEffectsAnimationAndTransforms:
  nodeModelResource = (modelNode1->modelPayload).modelResource;
  desiredHeading = previousRotationAngle - 0x4000 & 0xffff;
  facingAngle = (modelNode1->modelPayload).worldRotationAngle2 - 0x4000 & 0xffff;
  sinCosOffset = FixedMath_SinCosScaled(facingAngle,nodeModelResource->localBoundsY0Q12);
  primaryDelta = (int)sinCosOffset + (modelNode1->worldTransform).translation.x;
  secondaryDelta = (int)(sinCosOffset >> 0x20) + (modelNode1->worldTransform).translation.y;
  sinCosOffset = FixedMath_SinCosScaled(desiredHeading,nodeModelResource->localBoundsY0Q12);
  angleAndLength = FixedMath_Vector2AngleAndLengthRegs
                     (secondaryDelta - ((int)(sinCosOffset >> 0x20) + worldY),primaryDelta - ((int)sinCosOffset + worldX));
  trackDistance = angleAndLength.length;
  turnVelocityOrLimit = angleAndLength.angle - (modelNode1->modelPayload).worldRotationAngle2 & 0xffff;
  if ((0x4000 < turnVelocityOrLimit) && (turnVelocityOrLimit < 0xc000)) {
    trackDistance = -trackDistance;
  }
  modelNode1->primaryTextureOffsetU =
       modelNode1->primaryTextureOffsetU +
       trackDistance * modelRuntime->modelDefinition->trackTextureUScalePerDistance14;
  sinCosOffset = FixedMath_SinCosScaled(facingAngle ^ 0x8000,nodeModelResource->localBoundsY0Q12);
  primaryDelta = (int)sinCosOffset + (modelNode1->worldTransform).translation.x;
  secondaryDelta = (int)(sinCosOffset >> 0x20) + (modelNode1->worldTransform).translation.y;
  sinCosOffset = FixedMath_SinCosScaled(desiredHeading ^ 0x8000,nodeModelResource->localBoundsY0Q12);
  angleAndLength = FixedMath_Vector2AngleAndLengthRegs
                     (secondaryDelta - ((int)(sinCosOffset >> 0x20) + worldY),primaryDelta - ((int)sinCosOffset + worldX));
  trackDistance = angleAndLength.length;
  facingAngle = angleAndLength.angle - (modelNode1->modelPayload).worldRotationAngle2 & 0xffff;
  if ((0x4000 < facingAngle) && (facingAngle < 0xc000)) {
    trackDistance = -trackDistance;
  }
  primaryDelta = modelNode1->primaryTextureOffsetU;
  secondaryDelta = trackDistance * modelRuntime->modelDefinition->trackTextureUScalePerDistance14 +
           modelNode1->secondaryTextureOffsetU;
  if (secondaryDelta < 0x100001) {
    if (secondaryDelta < -0x100000) {
      secondaryDelta = secondaryDelta + 0x100000;
    }
  }
  else {
    secondaryDelta = secondaryDelta + -0x100000;
  }
  if (primaryDelta < 0x100001) {
    if (primaryDelta < -0x100000) {
      primaryDelta = primaryDelta + 0x100000;
    }
  }
  else {
    primaryDelta = primaryDelta + -0x100000;
  }
  modelNode1->secondaryTextureOffsetU = secondaryDelta;
  modelNode1->primaryTextureOffsetU = primaryDelta;
  if (((worldX != (modelNode1->worldTransform).translation.x) ||
      (worldY != (modelNode1->worldTransform).translation.y)) ||
     (previousRotationAngle != (modelNode1->modelPayload).worldRotationAngle2)) {
    linkedOrOwnerArmy = modelRuntime->ownerArmyRuntime;
    ArmyRuntime_UpdateTimedShotAndEffectEmitters
              (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
    ownerMovementFlags = &linkedOrOwnerArmy->movementStateFlags;
    *ownerMovementFlags = *ownerMovementFlags & 0xfffffffb;
  }
  movementDefinition = modelRuntime->modelDefinition;
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNode1);
  ModelNodeRuntime_UpdateDepthBinMasks(movementDefinition->placementRadiusOrClearanceDC,modelNode1);
  return;
}


/* Address: 0x00522C00.
   Ownership: gameplay/army/movement.
   Purpose: Table membership RUNTIME_UPDATE[17]. Updates route movement, yaw and banking state, child-model
   animation, terrain contact, collision response, and timed effects for the class-table implementation. Runtime-
   update partition slots 0-23 receive (worldRuntime, armyRuntime).
   Local calls: ArmyRuntime_UpdateMovementAndWaypoints.
   Cross-module calls: ArmyCollision_TestPointWithinExpandedRuntimeRadius [gameplay/army/placement],
   TerrainGrid_TestProjectedCellMaskBits01 [world/terrain/grid], SpatialSound_PlayPositionedOneShot
   [audio/spatial/runtime], FixedMath_Vector2AngleAndLengthRegs [core/math/fixed],
   ArmyRuntime_UpdateActivationMetricAndPlayStartSound [gameplay/army/runtime], FixedTrig_ProjectPlanarPointRegs
   [core/math/fixed].
*/

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_UpdateMovementBankingAndChildAnimation
          (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView200 *modelRuntime
          )

{
  ArmyMovementStateFlags *ownerMovementFlags;
  AngleTurn32 *childRotationAngle;
  uint32_t *classStateWord;
  ModelDefinitionGroundMovementSteeringView280 *movementDefinition;
  DirectSoundVoiceSet **voiceSetRef;
  AngleTurn32 previousRotationAngle;
  int previousWorldX;
  int previousWorldY;
  ModelRuntimeNode *thirdChildNode;
  AngleTurn32 currentHeading;
  ArmyPlacementContactKindIndex32 placementContactKind;
  uint32_t waypointWorldX;
  int primaryDelta;
  GraphicsFixedVec3 *worldPosition;
  ArmyRuntimeSlot *armyOrWaypointY;
  int secondaryDelta;
  AngleTurn32 angle16;
  uint32_t newHeading;
  uint32_t turnVelocityOrIndex;
  ModelRuntimeNode *modelNode2;
  bool testResult;
  FixedPlanarPointEdxEax8 nextPosition;
  ArmyCollisionResult blockingCollision;
  MovementStepResult waypointResult;
  FixedEulerAnglesEaxEbxEdx12 composedAngles;
  Q12 heightOffsetQ12;
  WorldRuntimeContext *dispatchWorldRuntime;
  FixedLengthAngleEaxEdx8 targetAngleLength;
  ModelRuntimeNode *modelNode1;
  
  armyOrWaypointY = (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime;
  modelNode2 = modelRuntime->rootModelNode;
  ownerMovementFlags = &modelRuntime->ownerArmyRuntime->movementStateFlags;
  *ownerMovementFlags = *ownerMovementFlags | 4;
  /* Drop the linked runtime unless both have a clearance radius and this unit is still within it. */
  if ((armyOrWaypointY != (ArmyRuntimeSlot *)0x0) &&
      (movementDefinition = modelRuntime->modelDefinition,
      (((((armyOrWaypointY->modelRuntimeOrSavedOffset).modelRuntime)->classState).classStateDC == 0) ||
        (movementDefinition->placementRadiusOrClearanceDC == 0)) ||
       (testResult = ArmyCollision_TestPointWithinExpandedRuntimeRadius
                     (movementDefinition->placementRadiusOrClearanceDC,
                      (modelNode2->worldTransform).translation.y,
                      (modelNode2->worldTransform).translation.x,armyOrWaypointY), !testResult))) {
    turnVelocityOrIndex = modelNode2->childCount;
    (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime = (ArmyRuntimeSlot *)0x0;
    if ((2 < turnVelocityOrIndex) && (((modelRuntime->classState).classStateB8 & 4) != 0)) {
      classStateWord = &(modelRuntime->classState).classStateB8;
      *classStateWord = *classStateWord | 1;
      turnVelocityOrIndex = *(uint32_t *)(movementDefinition->reserved26C_277 + 8);
      if ((turnVelocityOrIndex != 0) &&
         ((turnVelocityOrIndex < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != (uint32_t *)0x0)))) {
        voiceSetRef = (DirectSoundVoiceSet **)worldRuntime->dwordArray[turnVelocityOrIndex];
        if (voiceSetRef != (DirectSoundVoiceSet **)0x0) {
          worldPosition = &(modelRuntime->rootModelNode->worldTransform).translation;
          testResult = TerrainGrid_TestProjectedCellMaskBits01
                             ((modelRuntime->rootModelNode->worldTransform).translation.y,
                              worldPosition->x,worldRuntime);
          if (!testResult) {
            SpatialSound_PlayPositionedOneShot
                      (movementDefinition->positionedSoundMaximumDistanceQ12,movementDefinition->positionedSoundGainQ15,
                       worldPosition,voiceSetRef);
          }
        }
      }
    }
  }
  modelNode2 = modelRuntime->rootModelNode;
  previousRotationAngle = (modelNode2->modelPayload).worldRotationAngle2;
  previousWorldX = (modelNode2->worldTransform).translation.x;
  previousWorldY = (modelNode2->worldTransform).translation.y;
  if (((modelRuntime->classState).classStateEC & 8) == 0) {
    if (((modelRuntime->classState).classStateB8 & 1) != 0) {
      classStateWord = &(modelRuntime->classLinkState).classState70;
      *classStateWord = *classStateWord - 1;
      if (*classStateWord == 0) {
        classStateWord = &(modelRuntime->classState).classStateB8;
        *classStateWord = *classStateWord & 0xfffffff8;
      }
      modelNode1 = modelNode2->childNodes[1];
      thirdChildNode = modelNode2->childNodes[2];
      childRotationAngle = &(modelNode2->childNodes[0]->modelPayload).localRotationAngle1;
      *childRotationAngle = *childRotationAngle + 0x2aa;
      childRotationAngle = &(modelNode1->modelPayload).localRotationAngle1;
      *childRotationAngle = *childRotationAngle + 0x2aa;
      childRotationAngle = &(thirdChildNode->modelPayload).localRotationAngle1;
      *childRotationAngle = *childRotationAngle + 0x2aa;
    }
    waypointResult = ArmyRuntime_UpdateMovementAndWaypoints
                       (worldRuntime,(ArmyMovementRuntime *)modelRuntime->ownerArmyRuntime);
    armyOrWaypointY = (ArmyRuntimeSlot *)waypointResult.worldYQ12;
    waypointWorldX = waypointResult.worldXQ12;
    if (waypointResult.arrived) goto ArmyMovementBanking_ProcessStoppedMovementPlacementAndDamageState;
    if ((waypointWorldX != (modelRuntime->classLinkState).classState68) &&
       (armyOrWaypointY != (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime)) {
      (modelRuntime->classLinkState).classState68 = waypointWorldX;
      (modelRuntime->classLinkState).armyLinkOrState6C.classState = (uint32_t)armyOrWaypointY;
      (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
      (modelRuntime->movementControl).turnVelocityAngle16 = 0;
    }
    primaryDelta = waypointWorldX - (modelNode2->worldTransform).translation.x;
    secondaryDelta = (int)armyOrWaypointY - (modelNode2->worldTransform).translation.y;
    if ((primaryDelta == 0) && (secondaryDelta == 0)) {
      targetAngleLength = THANDOR_BITCAST(uint64_t, FixedLengthAngleEaxEdx8, ((uint64_t)(modelNode2->modelPayload).worldRotationAngle2 << 0x20));
    }
    else {
      targetAngleLength = FixedMath_Vector2AngleAndLengthRegs(secondaryDelta,primaryDelta);
    }
    angle16 = targetAngleLength.angle;
    movementDefinition = modelRuntime->modelDefinition;
    turnVelocityOrIndex = (modelRuntime->movementControl).turnVelocityAngle16;
    if ((turnVelocityOrIndex == 0) && (targetAngleLength.length < (uint32_t)movementDefinition->headingErrorInterpolationDistanceQ12C0))
    {
ArmyMovementBanking_HoldCurrentHeadingAndResetTurnVelocity:
      (modelRuntime->movementControl).turnVelocityAngle16 = 0;
      newHeading = (modelRuntime->rootModelNode->modelPayload).worldRotationAngle2;
    }
    else {
      currentHeading = (modelNode2->modelPayload).worldRotationAngle2;
      newHeading = angle16 - currentHeading & 0xffff;
      if (newHeading < 0x8000) {
        if ((int)turnVelocityOrIndex < 0) goto ArmyMovementBanking_HoldCurrentHeadingAndResetTurnVelocity;
        if (turnVelocityOrIndex < newHeading) {
          secondaryDelta = movementDefinition->turnRateLimitAnglePerTick10 * g_InGameSimulationStepTicks;
          primaryDelta = turnVelocityOrIndex + movementDefinition->turnRateAccelerationAnglePerTick1C * g_InGameSimulationStepTicks
          ;
          (modelRuntime->movementControl).turnVelocityAngle16 = secondaryDelta;
          newHeading = currentHeading + turnVelocityOrIndex;
          if (primaryDelta < secondaryDelta) {
            (modelRuntime->movementControl).turnVelocityAngle16 = primaryDelta;
          }
        }
        else {
ArmyMovementBanking_SnapFacingToDesiredHeading:
          (modelRuntime->movementControl).turnVelocityAngle16 = 0;
          newHeading = angle16;
        }
      }
      else {
        if (0 < (int)turnVelocityOrIndex) goto ArmyMovementBanking_HoldCurrentHeadingAndResetTurnVelocity;
        if (turnVelocityOrIndex + 0x10000 <= newHeading) goto ArmyMovementBanking_SnapFacingToDesiredHeading;
        newHeading = currentHeading + turnVelocityOrIndex;
        secondaryDelta = -movementDefinition->turnRateLimitAnglePerTick10 * g_InGameSimulationStepTicks;
        primaryDelta = turnVelocityOrIndex - movementDefinition->turnRateAccelerationAnglePerTick1C * g_InGameSimulationStepTicks;
        (modelRuntime->movementControl).turnVelocityAngle16 = secondaryDelta;
        if (secondaryDelta < primaryDelta) {
          (modelRuntime->movementControl).turnVelocityAngle16 = primaryDelta;
        }
      }
    }
    modelNode2 = modelRuntime->rootModelNode;
    if ((newHeading & 0xffff) != (modelNode2->modelPayload).worldRotationAngle2) {
      (modelNode2->modelPayload).worldRotationAngle2 = newHeading & 0xffff;
      modelNode2->runtimeFlags = modelNode2->runtimeFlags | 1;
    }
    ArmyRuntime_UpdateActivationMetricAndPlayStartSound
              (worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
    primaryDelta = (modelRuntime->movementControl).movementAdvancePerTickQ12 * g_InGameSimulationStepTicks
    ;
    if (primaryDelta < (int)targetAngleLength.length >> 1) {
      nextPosition = FixedTrig_ProjectPlanarPointRegs
                         (primaryDelta,angle16,(modelNode2->worldTransform).translation.y,
                          (modelNode2->worldTransform).translation.x);
    }
    else {
      armyOrWaypointY = modelRuntime->ownerArmyRuntime;
      waypointResult = ArmyRuntime_UpdateMovementAndWaypoints(worldRuntime,(ArmyMovementRuntime *)armyOrWaypointY);
      nextPosition = THANDOR_PART(uint64_t, waypointResult, 0);
      ownerMovementFlags = &armyOrWaypointY->movementStateFlags;
      *ownerMovementFlags = *ownerMovementFlags | 0x10;
    }
    placementContactKind = movementDefinition->placementContactKindIndex278;
    modelNode2 = modelRuntime->rootModelNode;
    heightOffsetQ12 = movementDefinition->placementHeightOffsetQ12;
    dispatchWorldRuntime = worldRuntime;
    blockingCollision = ArmyCollision_FindBlockingRuntimeForCurrentUnit
                       ((Q12)(nextPosition >> 0x20),(Q12)nextPosition,
                        (RuntimeCollisionQueryViewF4 *)modelRuntime,worldRuntime);
    if (blockingCollision.blocked) {
      modelNode1 = modelRuntime->rootModelNode;
      ArmyRuntime_HandleCollisionPartner
                ((ArmyRuntimeSlot *)modelRuntime,(modelNode1->worldTransform).translation.y,
                 (modelNode1->worldTransform).translation.x,(ArmyRuntimeSlot *)blockingCollision.blockingArmy,
                 worldRuntime);
      (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
      THANDOR_PART(uint32_t, nextPosition, 0) = (modelNode1->worldTransform).translation.x;
      THANDOR_PART(uint32_t, nextPosition, 4) = (modelNode1->worldTransform).translation.y;
      ownerMovementFlags = &modelRuntime->ownerArmyRuntime->movementStateFlags;
      *ownerMovementFlags = *ownerMovementFlags | 0x10;
    }
    armyOrWaypointY = modelRuntime->ownerArmyRuntime;
    g_ArmyPlacementContactKindDispatchTable.callbacks[placementContactKind]
              (heightOffsetQ12,(Q12)(nextPosition >> 0x20),(Q12)nextPosition,modelNode2,dispatchWorldRuntime);
    modelNode2 = modelRuntime->rootModelNode;
    primaryDelta = armyOrWaypointY->actionVector1Q12 + -1;
    if (primaryDelta < 0) goto ArmyMovementBanking_FinalizeBankingEffectsAndTransforms;
    primaryDelta = primaryDelta * armyOrWaypointY->actionVector2Q12;
    armyOrWaypointY->actionVector1Q12 = armyOrWaypointY->actionVector1Q12 + -1;
  }
  else {
ArmyMovementBanking_ProcessStoppedMovementPlacementAndDamageState:
    (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
    (modelRuntime->movementControl).turnVelocityAngle16 = 0;
    armyOrWaypointY = modelRuntime->ownerArmyRuntime;
    modelNode2 = modelRuntime->rootModelNode;
    primaryDelta = armyOrWaypointY->actionVector1Q12 + -1;
    if (primaryDelta < 0) {
      (*g_ArmyPlacementContactKindDispatchTable.callbacks
        [modelRuntime->modelDefinition->placementContactKindIndex278])
                (modelRuntime->modelDefinition->placementHeightOffsetQ12,
                 (modelNode2->worldTransform).translation.y,
                 (modelNode2->worldTransform).translation.x,modelNode2,worldRuntime);
      goto ArmyMovementBanking_FinalizeBankingEffectsAndTransforms;
    }
    primaryDelta = primaryDelta * armyOrWaypointY->actionVector2Q12;
    armyOrWaypointY->actionVector1Q12 = armyOrWaypointY->actionVector1Q12 + -1;
    (*g_ArmyPlacementContactKindDispatchTable.callbacks
      [modelRuntime->modelDefinition->placementContactKindIndex278])
              (modelRuntime->modelDefinition->placementHeightOffsetQ12,
               (modelNode2->worldTransform).translation.y,(modelNode2->worldTransform).translation.x
               ,modelNode2,worldRuntime);
  }
  composedAngles = FixedTransform_ComposeEulerAnglesRegs
                     (0,0x4000 - primaryDelta,
                      (modelRuntime->ownerArmyRuntime->actionVector0Q12 + 0x8000) -
                      (modelNode2->modelPayload).worldRotationAngle2 & 0xffff,
                      (modelNode2->modelPayload).worldRotationAngle2,
                      (modelNode2->modelPayload).worldRotationAngle1,
                      (modelNode2->modelPayload).worldRotationAngle0);
  (modelNode2->modelPayload).worldRotationAngle0 = composedAngles.angle0;
  (modelNode2->modelPayload).worldRotationAngle1 = composedAngles.angle1;
  (modelNode2->modelPayload).worldRotationAngle2 = composedAngles.angle2;
ArmyMovementBanking_FinalizeBankingEffectsAndTransforms:
  movementDefinition = modelRuntime->modelDefinition;
  if ((modelRuntime->classLinkState).modelLinkOrState60.modelRuntime != (ModelRuntimeSlot *)0x4000)
  {
    composedAngles = FixedTransform_ComposeEulerAnglesRegs
                       ((modelNode2->modelPayload).worldRotationAngle2,
                        (modelNode2->modelPayload).worldRotationAngle1,
                        (modelNode2->modelPayload).worldRotationAngle0,0,
                        (modelRuntime->classLinkState).modelLinkOrState60.classState,
                        (modelRuntime->classLinkState).classState64);
    (modelNode2->modelPayload).worldRotationAngle0 = composedAngles.angle0;
    (modelNode2->modelPayload).worldRotationAngle1 = composedAngles.angle1;
    (modelNode2->modelPayload).worldRotationAngle2 = composedAngles.angle2;
  }
  turnVelocityOrIndex = targetAngleLength.angle - (modelRuntime->classLinkState).classState64 & 0xffff;
  if (((modelRuntime->movementControl).movementAdvancePerTickQ12 == 0) ||
     ((int)targetAngleLength.length <= movementDefinition->movementStepQ12PerTick0C * 0x20)) {
    (modelRuntime->classLinkState).modelLinkOrState60.modelRuntime =
         (ModelRuntimeSlot *)
         &((modelRuntime->classLinkState).modelLinkOrState60.modelRuntime)->definitionValue84_40;
    if (0x4000 < (modelRuntime->classLinkState).modelLinkOrState60.signedScalarState) {
      (modelRuntime->classLinkState).modelLinkOrState60.classState = 0x4000;
    }
  }
  else {
    if (turnVelocityOrIndex < 0x8001) {
      if (0x400 < turnVelocityOrIndex) {
        turnVelocityOrIndex = 0x400;
      }
    }
    else if (turnVelocityOrIndex < 0xfc00) {
      turnVelocityOrIndex = 0xfffffc00;
    }
    (modelRuntime->classLinkState).modelLinkOrState60.modelRuntime =
         (ModelRuntimeSlot *)((modelRuntime->classLinkState).modelLinkOrState60.classState - 0x100);
    (modelRuntime->classLinkState).classState64 =
         turnVelocityOrIndex + (modelRuntime->classLinkState).classState64 & 0xffff;
    if ((modelRuntime->classLinkState).modelLinkOrState60.signedScalarState < 0x3800) {
      (modelRuntime->classLinkState).modelLinkOrState60.classState = 0x3800;
    }
  }
  if (((modelRuntime->classState).classStateB8 & 2) == 0) {
    ArmyRuntime_UpdateTimedShotAndEffectEmitters
              (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
  }
  if (((previousWorldX != (modelNode2->worldTransform).translation.x) ||
      (previousWorldY != (modelNode2->worldTransform).translation.y)) ||
     (previousRotationAngle != (modelNode2->modelPayload).worldRotationAngle2)) {
    ownerMovementFlags = &modelRuntime->ownerArmyRuntime->movementStateFlags;
    *ownerMovementFlags = *ownerMovementFlags & 0xfffffffb;
  }
  movementDefinition = modelRuntime->modelDefinition;
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNode2);
  ModelNodeRuntime_UpdateDepthBinMasks(movementDefinition->placementRadiusOrClearanceDC,modelNode2);
  return;
}


/* Helper for ArmyRuntime_ResetMovementStateFromModel (no original address: the original walks the tree
   iteratively with an explicit stack). Clears state bits 0x218 of a model runtime and of all its children
   (child count at +0x0C, child pointers at +0x140 + 32*i, null slots skipped). */
static void ArmyRuntime_ClearModelTreeFlags218(uint8_t *node)
{
  int childIndex;
  *(uint32_t *)(node + 0xec) = *(uint32_t *)(node + 0xec) & 0xfffffde7;
  for (childIndex = 0; childIndex < *(int *)(node + 0xc); childIndex++) {
    uint8_t *child = *(uint8_t **)(node + 0x140 + childIndex * 0x20);
    if (child != NULL) {
      ArmyRuntime_ClearModelTreeFlags218(child);
    }
  }
}

/* Address: 0x0051C3E0.
   Stops the army where its model currently stands: clears the move flags, cancels an active target
   command (unless state field 0x100 is zero), and sets every move target to the current model position.
   If the attached model has class-state bit 0x10 and a non-zero definition value +0x3C, state bits 0x218
   are cleared on its whole model tree.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_ResetMovementStateFromModel(ArmyRuntimeSlot *armyRuntime)

{
  GraphicsWorldCoordinateQ12 currentWorldX;
  GraphicsWorldCoordinateQ12 currentWorldY;
  ArmyCommandGeneration standardGeneration;
  ModelRuntimeSlot *attachedModelRuntime;
  bool stateField100Zero;
  ModelRuntimeNode *modelNode;

  standardGeneration = g_ArmyCommandGenerationStandard;
  modelNode = armyRuntime->modelNodeRuntime;
  armyRuntime->movementStateFlags =
       armyRuntime->movementStateFlags &
       ~(ARMY_MOVEMENT_ACTIVE | ARMY_MOVEMENT_WAYPOINTS_QUEUED | 0x10 | ARMY_MOVEMENT_TARGET_FOLLOWING);
  stateField100Zero = ArmyRuntime_TestStateField100Zero(armyRuntime);
  if ((!stateField100Zero) &&
      ((armyRuntime->commandModeFlags & (ARMY_COMMAND_MODE_TARGET_ARMY | ARMY_COMMAND_MODE_TARGET_POSITION)) != 0)) {
    armyRuntime->commandModeFlags =
         armyRuntime->commandModeFlags & ~(ARMY_COMMAND_MODE_TARGET_ARMY | ARMY_COMMAND_MODE_TARGET_POSITION);
    armyRuntime->commandModeFlags = armyRuntime->commandModeFlags | ARMY_COMMAND_MODE_INTERRUPTED;
    armyRuntime->commandGeneration = standardGeneration;
    armyRuntime->commandTargetArmyRuntime = NULL;
  }
  currentWorldX = (modelNode->worldTransform).translation.x;
  currentWorldY = (modelNode->worldTransform).translation.y;
  (armyRuntime->articulatedContact).fallbackPosition0Q12 = currentWorldX;
  (armyRuntime->articulatedContact).fallbackPosition1Q12 = currentWorldY;
  armyRuntime->movementTarget0Q12 = currentWorldX;
  armyRuntime->movementTarget1Q12 = currentWorldY;
  armyRuntime->movementPosition0Q12 = currentWorldX;
  armyRuntime->movementPosition1Q12 = currentWorldY;
  attachedModelRuntime = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  if ((((attachedModelRuntime->classState).classStateEC & 0x10) != 0) && (attachedModelRuntime->definitionValue60_3C != 0)) {
    /* Rewritten from the assembly (0x0051C460-0x0051C4A4): clear 0x218 on the whole model tree. */
    ArmyRuntime_ClearModelTreeFlags218((uint8_t *)attachedModelRuntime);
  }
  return;

}


/* Address: 0x0051C500.
   Cancels an active target command (army or position): marks the command as interrupted, stamps the
   standard command generation and drops the target army. Also used by the 32-slot command reset traversal.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeCommand_InterruptActiveTargetAndStampGeneration(ArmyRuntimeSlot *armyRuntime)

{
  ArmyCommandGeneration commandGeneration;

  commandGeneration = g_ArmyCommandGenerationStandard;
  if ((armyRuntime->commandModeFlags & (ARMY_COMMAND_MODE_TARGET_ARMY | ARMY_COMMAND_MODE_TARGET_POSITION)) != 0) {
    armyRuntime->commandModeFlags =
         armyRuntime->commandModeFlags & ~(ARMY_COMMAND_MODE_TARGET_ARMY | ARMY_COMMAND_MODE_TARGET_POSITION);
    armyRuntime->commandModeFlags = armyRuntime->commandModeFlags | ARMY_COMMAND_MODE_INTERRUPTED;
    armyRuntime->commandGeneration = commandGeneration;
    armyRuntime->commandTargetArmyRuntime = NULL;
  }
  return;
}


/* Address: 0x0051CAF0.
   Ownership: gameplay/army/movement.
   Purpose: Typed parameters: p2 auxiliaryValue1→ArmyMoveAuxiliaryValue1_V344, p3
   auxiliaryValue0→ArmyMoveAuxiliaryValue0_V344. Calling convention, complete VariableStorage serialization,
   function bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: EntityPathing_ResolveDestinationAndRebuildRoutes [world/pathing/grid].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_StartMoveCommandWithAuxiliaryValues
          (ArmyMoveAuxiliaryValue1 auxiliaryValue1,ArmyMoveAuxiliaryValue0 auxiliaryValue0,
          Q12 targetWorldYQ12,Q12 targetWorldXQ12,ArmyMovementRuntime *movementRuntime)

{
  GraphicsWorldCoordinateQ12 currentWorldX;
  GraphicsWorldCoordinateQ12 currentWorldY;
  WorldRuntimeContext *worldRuntime;
  PathingDestinationResult resolvedDestination;
  
  worldRuntime = &g_InGameRuntimeRoot->worldRuntime0A30;
  movementRuntime->movementStateFlags = movementRuntime->movementStateFlags | 0x24b;
  movementRuntime->movementStateFlags = movementRuntime->movementStateFlags & 0xffffff4f;
  movementRuntime->commandModeFlags = movementRuntime->commandModeFlags & 0xffffffef;
  movementRuntime->queuedWaypointCount = 1;
  resolvedDestination = EntityPathing_ResolveDestinationAndRebuildRoutes
                    (targetWorldYQ12,targetWorldXQ12,movementRuntime->entityRuntime,worldRuntime);
  (movementRuntime->fallbackPosition).worldXQ12 = resolvedDestination.fallbackWorldXQ12;
  (movementRuntime->fallbackPosition).worldYQ12 = resolvedDestination.fallbackWorldYQ12;
  movementRuntime->movementTargetWorldXQ12 = resolvedDestination.fallbackWorldXQ12;
  movementRuntime->movementTargetWorldYQ12 = resolvedDestination.fallbackWorldYQ12;
  movementRuntime->movementWorldXQ12 = resolvedDestination.primaryWorldXQ12;
  movementRuntime->movementWorldYQ12 = resolvedDestination.primaryWorldYQ12;
  currentWorldX = (movementRuntime->modelNodeRuntime->worldTransform).translation.x;
  currentWorldY = (movementRuntime->modelNodeRuntime->worldTransform).translation.y;
  movementRuntime->retryCountdown = 0x40;
  movementRuntime->lastCheckedWorldXQ12 = currentWorldX;
  movementRuntime->lastCheckedWorldYQ12 = currentWorldY;
  movementRuntime->queuedWaypoints[0].worldXQ12 = auxiliaryValue0;
  movementRuntime->queuedWaypoints[0].worldYQ12 = auxiliaryValue1;
  return;
}


/* Address: 0x0051CDB0.
   Sets a new immediate move position without path finding (ignored while the movement is locked). The
   fallback position only follows when no move was active, the final target only when mirroring is enabled;
   the current model position is recorded for the route-retry check.
*/
void __thandor_preserve_eax_edx
ArmyRuntime_SetPendingMoveTarget
          (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime)

{
  GraphicsWorldCoordinateQ12 currentWorldX;
  GraphicsWorldCoordinateQ12 currentWorldY;
  ModelRuntimeNode *modelNode;

  if ((movementRuntime->movementStateFlags & ARMY_MOVEMENT_LOCKED) == 0) {
    modelNode = movementRuntime->modelNodeRuntime;
    movementRuntime->movementWorldXQ12 = targetWorldX;
    movementRuntime->movementWorldYQ12 = targetWorldY;
    if ((movementRuntime->movementStateFlags & ARMY_MOVEMENT_ACTIVE) == 0) {
      (movementRuntime->fallbackPosition).worldXQ12 = targetWorldX;
      (movementRuntime->fallbackPosition).worldYQ12 = targetWorldY;
    }
    if ((movementRuntime->movementStateFlags & ARMY_MOVEMENT_MIRROR_TARGET) != 0) {
      movementRuntime->movementTargetWorldXQ12 = targetWorldX;
      movementRuntime->movementTargetWorldYQ12 = targetWorldY;
    }
    movementRuntime->movementStateFlags = movementRuntime->movementStateFlags | ARMY_MOVEMENT_ACTIVE;
    movementRuntime->movementStateFlags =
         movementRuntime->movementStateFlags & ~(0x10 | ARMY_MOVEMENT_TARGET_FOLLOWING);
    currentWorldX = (modelNode->worldTransform).translation.x;
    currentWorldY = (modelNode->worldTransform).translation.y;
    movementRuntime->retryCountdown = ARMY_MOVEMENT_RETRY_TICKS;
    movementRuntime->lastCheckedWorldXQ12 = currentWorldX;
    movementRuntime->lastCheckedWorldYQ12 = currentWorldY;
  }
  return;
}


/* Address: 0x00521680.
   Ownership: gameplay/army/movement.
   Purpose: Projects the root model onto the field surface and initializes the paired lateral contact-point
   coordinates, heading values, and fixed Q30 orientation terms used by the articulated runtime. Two stack
   arguments are authoritative from RET 0x08; prior register parameters and return are synthetic.
   Cross-module calls: FieldGrid_InterpolateTerrainHeight [world/terrain/grid],
   ModelNodeRuntime_RebuildTransformsFromRoot [world/model/hierarchy], FixedMath_SinCosScaled [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyArticulatedRuntime_InitializeTerrainContactGeometry
          (ModelRuntimeNode *modelNodeRuntime,WorldRuntimeContext *worldRuntime)

{
  ArmyRuntimeCoordinateCommandOrHistoryValue4 rootHeading;
  uint32_t terrainHeight;
  uint32_t contactX;
  uint32_t contactY;
  int lateralOffsetY;
  FixedSinCosEdxEax8 lateralOffset;
  HeightSampleResult terrainHeightResult;
  Q12 worldXQ12;
  Q12 worldYQ12;
  ArmyArticulatedRuntimeSlotView *articulatedRuntime;
  
  articulatedRuntime = (ArmyArticulatedRuntimeSlotView *)(modelNodeRuntime->runtimePayload).modelRuntime;
  worldXQ12 = (modelNodeRuntime->worldTransform).translation.x;
  worldYQ12 = (modelNodeRuntime->worldTransform).translation.y;
  terrainHeight = 0;
  if (worldRuntime->fieldGrid != (FieldGridAsset *)0x0) {
    terrainHeightResult = FieldGrid_InterpolateTerrainHeight(worldYQ12,worldXQ12,worldRuntime->fieldGrid);
    terrainHeight = terrainHeightResult.heightQ12;
  }
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
  lateralOffset = FixedMath_SinCosScaled
                    ((modelNodeRuntime->modelPayload).worldRotationAngle2 + 0x4000 & 0xffff,
                     (articulatedRuntime->articulatedContact).lateralOffsetQ12);
  lateralOffsetY = (int)(lateralOffset >> 0x20);
  contactX = worldXQ12 + (int)lateralOffset;
  contactY = worldYQ12 + lateralOffsetY;
  articulatedRuntime->movementTarget0Q12 = contactX;
  articulatedRuntime->definitionClassValue80 = contactY;
  articulatedRuntime->definitionClassValue88 = terrainHeight;
  articulatedRuntime->runtimeState90 = contactX;
  articulatedRuntime->runtimeState98 = contactY;
  articulatedRuntime->articulatedHeightOrStateA0 = terrainHeight;
  contactX = worldXQ12 - (int)lateralOffset;
  contactY = worldYQ12 - lateralOffsetY;
  articulatedRuntime->movementTarget1Q12 = contactX;
  articulatedRuntime->definitionClassValue84 = contactY;
  articulatedRuntime->runtimeState8C = terrainHeight;
  articulatedRuntime->runtimeState94 = contactX;
  articulatedRuntime->articulatedCoordinateOrState9C = contactY;
  articulatedRuntime->runtimeStateA4 = terrainHeight;
  rootHeading = THANDOR_BITCAST(AngleTurn32, ArmyRuntimeCoordinateCommandOrHistoryValue4, (modelNodeRuntime->modelPayload).worldRotationAngle2);
  articulatedRuntime->classState60 = THANDOR_BITCAST(ArmyRuntimeCoordinateCommandOrHistoryValue4, uint32_t, rootHeading);
  articulatedRuntime->ownerValue64 = THANDOR_BITCAST(ArmyRuntimeCoordinateCommandOrHistoryValue4, uint32_t, rootHeading);
  (articulatedRuntime->linkedChildOverloadedState).primaryCoordinateCommandOrHistory = rootHeading;
  (articulatedRuntime->linkedChildOverloadedState).leftHeadingCommandOrSpawnValue = rootHeading;
  (articulatedRuntime->linkedChildOverloadedState).secondaryCoordinateCommandOrHistory = rootHeading;
  (articulatedRuntime->linkedChildOverloadedState).rightHeadingCommandOrSpawnValue = rootHeading;
  articulatedRuntime->ownerValue68 = 0x40000000;
  articulatedRuntime->fallbackWorldYQ12 = 0x40000000;
  articulatedRuntime->linkedArmyRuntimeOrSavedOffset = (ArmyRuntimeSlot *)0x40000000;
  articulatedRuntime->fallbackWorldXQ12 = 0x40000000;
  return;
}


/* Address: 0x00527BC0.
   Ownership: gameplay/army/movement.
   Purpose: Third exact two-argument no-op reused across unified runtime object method tables. It returns with ret
   0x08. Default class-command no-op used by fourteen class slots.
*/
void ArmyRuntimeClassCommand_NoOp(WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  return;
}

/* Address: 0x0051C8E0.
   Starts a new routed move order to the target (path finding via EntityPathing), dropping any waypoint
   queue and target mirroring. While the movement is locked the target replaces the waypoint queue instead.
   Entities whose definition record has zero at +0x18 ignore the order.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_QueueOrStartMoveCommandVariantA
          (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime)

{
  GraphicsWorldCoordinateQ12 currentWorldX;
  GraphicsWorldCoordinateQ12 currentWorldY;
  WorldRuntimeContext *worldRuntime;
  PathingDestinationResult resolvedDestination;

  if (*(int *)((int)(movementRuntime->entityRuntime->common).ownership.definitionOrClassRecord +
              0x18) != 0) {
    if ((movementRuntime->movementStateFlags & ARMY_MOVEMENT_LOCKED) == 0) {
      worldRuntime = &g_InGameRuntimeRoot->worldRuntime0A30;
      movementRuntime->movementStateFlags =
           movementRuntime->movementStateFlags | (ARMY_MOVEMENT_ROUTED | 0x40 | ARMY_MOVEMENT_ACTIVE);
      movementRuntime->movementStateFlags =
           movementRuntime->movementStateFlags &
           ~(ARMY_MOVEMENT_MIRROR_TARGET | ARMY_MOVEMENT_DIRECT | ARMY_MOVEMENT_TARGET_FOLLOWING | 0x10 |
             ARMY_MOVEMENT_WAYPOINTS_QUEUED);
      movementRuntime->commandModeFlags = movementRuntime->commandModeFlags & 0xffffffef;
      resolvedDestination = EntityPathing_ResolveDestinationAndRebuildRoutes
                        (targetWorldY,targetWorldX,movementRuntime->entityRuntime,worldRuntime);
      (movementRuntime->fallbackPosition).worldXQ12 = resolvedDestination.fallbackWorldXQ12;
      (movementRuntime->fallbackPosition).worldYQ12 = resolvedDestination.fallbackWorldYQ12;
      movementRuntime->movementTargetWorldXQ12 = resolvedDestination.fallbackWorldXQ12;
      movementRuntime->movementTargetWorldYQ12 = resolvedDestination.fallbackWorldYQ12;
      movementRuntime->movementWorldXQ12 = resolvedDestination.primaryWorldXQ12;
      movementRuntime->movementWorldYQ12 = resolvedDestination.primaryWorldYQ12;
      currentWorldX = (movementRuntime->modelNodeRuntime->worldTransform).translation.x;
      currentWorldY = (movementRuntime->modelNodeRuntime->worldTransform).translation.y;
      movementRuntime->retryCountdown = ARMY_MOVEMENT_RETRY_TICKS;
      movementRuntime->lastCheckedWorldXQ12 = currentWorldX;
      movementRuntime->lastCheckedWorldYQ12 = currentWorldY;
    }
    else {
      movementRuntime->movementStateFlags = movementRuntime->movementStateFlags & ~ARMY_MOVEMENT_WAYPOINTS_QUEUED;
      ArmyRuntime_QueueWaypointOrStartMoveVariantA(targetWorldY,targetWorldX,movementRuntime);
    }
  }
  return;
}


/* Address: 0x0051C9A0.
   Same as ArmyRuntime_QueueOrStartMoveCommandVariantA, but keeps the waypoint queue and target mirroring:
   used by ArmyRuntime_UpdateMovementAndWaypoints to start the next queued waypoint.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_QueueOrStartMoveCommandVariantB
          (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime)

{
  GraphicsWorldCoordinateQ12 currentWorldX;
  GraphicsWorldCoordinateQ12 currentWorldY;
  WorldRuntimeContext *worldRuntime;
  PathingDestinationResult resolvedDestination;

  if (*(int *)((int)(movementRuntime->entityRuntime->common).ownership.definitionOrClassRecord +
              0x18) != 0) {
    if ((movementRuntime->movementStateFlags & ARMY_MOVEMENT_LOCKED) == 0) {
      worldRuntime = &g_InGameRuntimeRoot->worldRuntime0A30;
      movementRuntime->movementStateFlags =
           movementRuntime->movementStateFlags | (ARMY_MOVEMENT_ROUTED | 0x40 | ARMY_MOVEMENT_ACTIVE);
      movementRuntime->movementStateFlags =
           movementRuntime->movementStateFlags & ~(ARMY_MOVEMENT_DIRECT | ARMY_MOVEMENT_TARGET_FOLLOWING | 0x10);
      movementRuntime->commandModeFlags = movementRuntime->commandModeFlags & 0xffffffef;
      resolvedDestination = EntityPathing_ResolveDestinationAndRebuildRoutes
                        (targetWorldY,targetWorldX,movementRuntime->entityRuntime,worldRuntime);
      (movementRuntime->fallbackPosition).worldXQ12 = resolvedDestination.fallbackWorldXQ12;
      (movementRuntime->fallbackPosition).worldYQ12 = resolvedDestination.fallbackWorldYQ12;
      movementRuntime->movementTargetWorldXQ12 = resolvedDestination.fallbackWorldXQ12;
      movementRuntime->movementTargetWorldYQ12 = resolvedDestination.fallbackWorldYQ12;
      movementRuntime->movementWorldXQ12 = resolvedDestination.primaryWorldXQ12;
      movementRuntime->movementWorldYQ12 = resolvedDestination.primaryWorldYQ12;
      currentWorldX = (movementRuntime->modelNodeRuntime->worldTransform).translation.x;
      currentWorldY = (movementRuntime->modelNodeRuntime->worldTransform).translation.y;
      movementRuntime->retryCountdown = ARMY_MOVEMENT_RETRY_TICKS;
      movementRuntime->lastCheckedWorldXQ12 = currentWorldX;
      movementRuntime->lastCheckedWorldYQ12 = currentWorldY;
    }
    else {
      movementRuntime->movementStateFlags = movementRuntime->movementStateFlags & ~ARMY_MOVEMENT_WAYPOINTS_QUEUED;
      ArmyRuntime_QueueWaypointOrStartMoveVariantA(targetWorldY,targetWorldX,movementRuntime);
    }
  }
  return;
}


/* Address: 0x00520840.
   Ownership: gameplay/army/movement.
   Purpose: Updates route-following, turn acceleration, terrain clearance, collision response, model orientation,
   and depth-bin state for the first verified ground-movement runtime class. Runtime-update partition slots 0-23
   receive (worldRuntime, armyRuntime).
   Local calls: ArmyRuntime_UpdateMovementAndWaypoints.
   Cross-module calls: ArmyCollision_TestPointWithinExpandedRuntimeRadius [gameplay/army/placement],
   FieldGrid_InterpolateWaterDelta [world/terrain/grid], ArmyRuntime_ApplyDamageAndPropagateToParent
   [gameplay/army/combat], FixedMath_Vector2AngleAndLengthRegs [core/math/fixed],
   ArmyRuntime_UpdateActivationMetricAndPlayStartSound [gameplay/army/runtime], FixedTrig_ProjectPlanarPointRegs
   [core/math/fixed].
*/

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_UpdateGroundMovementVariantA
          (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView200 *modelRuntime
          )

{
  ArmyMovementStateFlags *ownerMovementFlags;
  ArmyRuntimeSlot *linkedOrOwnerArmy;
  AngleTurn32 previousRotationAngle;
  int worldX;
  int worldY;
  ModelDefinitionGroundMovementSteeringView280 *movementDefinition;
  ArmyPlacementContactKindIndex32 placementContactKind;
  int32_t waterDelta;
  int primaryDelta;
  uint32_t facingAngle;
  uint32_t turnVelocityOrLimit;
  int secondaryDelta;
  uint32_t desiredHeading;
  uint32_t headingDifference;
  ModelRuntimeNode *modelNode2;
  bool withinLinkRadius;
  FixedLengthAngleEaxEdx8 angleAndLength;
  FixedPlanarPointEdxEax8 nextPosition;
  ArmyCollisionResult blockingCollision;
  MovementStepResult waypointResult;
  FixedEulerAnglesEaxEbxEdx12 composedAngles;
  Q12 heightOffsetQ12;
  WorldRuntimeContext *dispatchWorldRuntime;
  uint32_t targetDistance;
  ModelRuntimeNode *modelNode1;
  
  linkedOrOwnerArmy = (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime;
  modelNode2 = modelRuntime->rootModelNode;
  ownerMovementFlags = &modelRuntime->ownerArmyRuntime->movementStateFlags;
  *ownerMovementFlags = *ownerMovementFlags | 4;
  /* Drop the linked runtime unless both have a clearance radius and this unit is still within it. */
  if ((linkedOrOwnerArmy != (ArmyRuntimeSlot *)0x0) &&
      ((((((linkedOrOwnerArmy->modelRuntimeOrSavedOffset).modelRuntime)->classState).classStateDC == 0) ||
        (modelRuntime->modelDefinition->placementRadiusOrClearanceDC == 0)) ||
       (withinLinkRadius = ArmyCollision_TestPointWithinExpandedRuntimeRadius
                           (modelRuntime->modelDefinition->placementRadiusOrClearanceDC,
                            (modelNode2->worldTransform).translation.y,
                            (modelNode2->worldTransform).translation.x,linkedOrOwnerArmy), !withinLinkRadius))) {
    (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime = (ArmyRuntimeSlot *)0x0;
  }
  previousRotationAngle = (modelNode2->modelPayload).worldRotationAngle2;
  worldX = (modelNode2->worldTransform).translation.x;
  worldY = (modelNode2->worldTransform).translation.y;
  movementDefinition = modelRuntime->modelDefinition;
  waterDelta = FieldGrid_InterpolateWaterDelta(worldY,worldX,worldRuntime->fieldGrid);
  if ((movementDefinition->waterDamageThreshold198 < waterDelta) &&
     (primaryDelta = waterDelta * movementDefinition->waterDamageMultiplier194 >> 7, -1 < primaryDelta)) {
    ArmyRuntime_ApplyDamageAndPropagateToParent(primaryDelta,(ArmyRuntimeSlot *)modelRuntime);
  }
  if (((modelRuntime->classState).classStateEC & 8) == 0) {
    waypointResult = ArmyRuntime_UpdateMovementAndWaypoints
                       (worldRuntime,(ArmyMovementRuntime *)modelRuntime->ownerArmyRuntime);
    if (waypointResult.arrived) goto ArmyGroundMovementVariantA_StopMovementAndTurn;
    secondaryDelta = waypointResult.worldYQ12 - (modelNode2->worldTransform).translation.y;
    primaryDelta = waypointResult.worldXQ12 - (modelNode2->worldTransform).translation.x;
    if ((primaryDelta == 0) && (secondaryDelta == 0)) {
      angleAndLength = THANDOR_BITCAST(uint64_t, FixedLengthAngleEaxEdx8, ((uint64_t)(modelNode2->modelPayload).worldRotationAngle2 << 0x20));
    }
    else {
      angleAndLength = FixedMath_Vector2AngleAndLengthRegs(secondaryDelta,primaryDelta);
    }
    desiredHeading = angleAndLength.angle;
    movementDefinition = modelRuntime->modelDefinition;
    facingAngle = (modelNode2->modelPayload).worldRotationAngle2;
    turnVelocityOrLimit = (modelRuntime->movementControl).turnVelocityAngle16;
    headingDifference = desiredHeading - facingAngle & 0xffff;
    if (headingDifference < 0x8000) {
      if ((int)turnVelocityOrLimit < 0) {
ArmyGroundMovementVariantA_ResetTurnVelocityForDirectionReversal:
        (modelRuntime->movementControl).turnVelocityAngle16 = 0;
      }
      else if (turnVelocityOrLimit < headingDifference) {
        facingAngle = facingAngle + turnVelocityOrLimit;
        secondaryDelta = movementDefinition->turnRateLimitAnglePerTick10 * g_InGameSimulationStepTicks;
        primaryDelta = turnVelocityOrLimit + movementDefinition->turnRateAccelerationAnglePerTick1C * g_InGameSimulationStepTicks;
        (modelRuntime->movementControl).turnVelocityAngle16 = secondaryDelta;
        if (primaryDelta < secondaryDelta) {
          (modelRuntime->movementControl).turnVelocityAngle16 = primaryDelta;
        }
      }
      else {
ArmyGroundMovementVariantA_SnapFacingToDesiredHeading:
        (modelRuntime->movementControl).turnVelocityAngle16 = 0;
        facingAngle = desiredHeading;
      }
    }
    else {
      if (0 < (int)turnVelocityOrLimit) goto ArmyGroundMovementVariantA_ResetTurnVelocityForDirectionReversal;
      if (turnVelocityOrLimit + 0x10000 <= headingDifference) goto ArmyGroundMovementVariantA_SnapFacingToDesiredHeading;
      facingAngle = facingAngle + turnVelocityOrLimit;
      secondaryDelta = -movementDefinition->turnRateLimitAnglePerTick10 * g_InGameSimulationStepTicks;
      primaryDelta = turnVelocityOrLimit - movementDefinition->turnRateAccelerationAnglePerTick1C * g_InGameSimulationStepTicks;
      (modelRuntime->movementControl).turnVelocityAngle16 = secondaryDelta;
      if (secondaryDelta < primaryDelta) {
        (modelRuntime->movementControl).turnVelocityAngle16 = primaryDelta;
      }
    }
    modelNode2 = modelRuntime->rootModelNode;
    facingAngle = facingAngle & 0xffff;
    if (facingAngle != (modelNode2->modelPayload).worldRotationAngle2) {
      (modelNode2->modelPayload).worldRotationAngle2 = facingAngle;
      modelNode2->runtimeFlags = modelNode2->runtimeFlags | 1;
    }
    targetDistance = angleAndLength.length;
    turnVelocityOrLimit = movementDefinition->farHeadingErrorLimitAngleC4;
    facingAngle = facingAngle - desiredHeading & 0xffff;
    if ((int)targetDistance < movementDefinition->headingErrorInterpolationDistanceQ12C0) {
      turnVelocityOrLimit = movementDefinition->nearHeadingErrorLimitAngleC8 +
              (int)(((int64_t)(int)(turnVelocityOrLimit - movementDefinition->nearHeadingErrorLimitAngleC8) *
                    (int64_t)(int)targetDistance) /
                   (int64_t)movementDefinition->headingErrorInterpolationDistanceQ12C0);
    }
    if ((turnVelocityOrLimit < facingAngle) && (facingAngle < 0x10000 - turnVelocityOrLimit)) {
      (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
      goto ArmyGroundMovementVariantA_ProcessStationaryPlacementAndDamageState;
    }
    ArmyRuntime_UpdateActivationMetricAndPlayStartSound
              (worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
    primaryDelta = (modelRuntime->movementControl).movementAdvancePerTickQ12 * g_InGameSimulationStepTicks;
    if (primaryDelta < (int)targetDistance >> 1) {
      nextPosition = FixedTrig_ProjectPlanarPointRegs
                         (primaryDelta,(modelNode2->modelPayload).worldRotationAngle2,
                          (modelNode2->worldTransform).translation.y,
                          (modelNode2->worldTransform).translation.x);
    }
    else {
      linkedOrOwnerArmy = modelRuntime->ownerArmyRuntime;
      waypointResult = ArmyRuntime_UpdateMovementAndWaypoints(worldRuntime,(ArmyMovementRuntime *)linkedOrOwnerArmy);
      nextPosition = THANDOR_PART(uint64_t, waypointResult, 0);
      ownerMovementFlags = &linkedOrOwnerArmy->movementStateFlags;
      *ownerMovementFlags = *ownerMovementFlags | 0x10;
    }
    placementContactKind = movementDefinition->placementContactKindIndex278;
    modelNode2 = modelRuntime->rootModelNode;
    heightOffsetQ12 = movementDefinition->placementHeightOffsetQ12;
    dispatchWorldRuntime = worldRuntime;
    blockingCollision = ArmyCollision_FindBlockingRuntimeForCurrentUnit
                       ((Q12)(nextPosition >> 0x20),(Q12)nextPosition,
                        (RuntimeCollisionQueryViewF4 *)modelRuntime,worldRuntime);
    if (blockingCollision.blocked) {
      modelNode1 = modelRuntime->rootModelNode;
      ArmyRuntime_HandleCollisionPartner
                ((ArmyRuntimeSlot *)modelRuntime,(modelNode1->worldTransform).translation.y,
                 (modelNode1->worldTransform).translation.x,(ArmyRuntimeSlot *)blockingCollision.blockingArmy,
                 worldRuntime);
      (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
      THANDOR_PART(uint32_t, nextPosition, 0) = (modelNode1->worldTransform).translation.x;
      THANDOR_PART(uint32_t, nextPosition, 4) = (modelNode1->worldTransform).translation.y;
      ownerMovementFlags = &modelRuntime->ownerArmyRuntime->movementStateFlags;
      *ownerMovementFlags = *ownerMovementFlags | 0x10;
    }
    linkedOrOwnerArmy = modelRuntime->ownerArmyRuntime;
    g_ArmyPlacementContactKindDispatchTable.callbacks[placementContactKind]
              (heightOffsetQ12,(Q12)(nextPosition >> 0x20),(Q12)nextPosition,modelNode2,dispatchWorldRuntime);
    modelNode2 = modelRuntime->rootModelNode;
    primaryDelta = linkedOrOwnerArmy->actionVector1Q12 + -1;
    if (primaryDelta < 0) goto ArmyGroundMovementVariantA_FinalizeEffectsAnimationAndTransforms;
    primaryDelta = primaryDelta * linkedOrOwnerArmy->actionVector2Q12;
    linkedOrOwnerArmy->actionVector1Q12 = linkedOrOwnerArmy->actionVector1Q12 + -1;
  }
  else {
ArmyGroundMovementVariantA_StopMovementAndTurn:
    (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
    (modelRuntime->movementControl).turnVelocityAngle16 = 0;
ArmyGroundMovementVariantA_ProcessStationaryPlacementAndDamageState:
    linkedOrOwnerArmy = modelRuntime->ownerArmyRuntime;
    modelNode2 = modelRuntime->rootModelNode;
    primaryDelta = linkedOrOwnerArmy->actionVector1Q12 + -1;
    if (primaryDelta < 0) {
      if ((worldRuntime->fieldGrid->runtimeStateFlags & 1) != 0) {
        (*g_ArmyPlacementContactKindDispatchTable.callbacks
          [modelRuntime->modelDefinition->placementContactKindIndex278])
                  (modelRuntime->modelDefinition->placementHeightOffsetQ12,
                   (modelNode2->worldTransform).translation.y,
                   (modelNode2->worldTransform).translation.x,modelNode2,worldRuntime);
      }
      goto ArmyGroundMovementVariantA_FinalizeEffectsAnimationAndTransforms;
    }
    primaryDelta = primaryDelta * linkedOrOwnerArmy->actionVector2Q12;
    linkedOrOwnerArmy->actionVector1Q12 = linkedOrOwnerArmy->actionVector1Q12 + -1;
    (*g_ArmyPlacementContactKindDispatchTable.callbacks
      [modelRuntime->modelDefinition->placementContactKindIndex278])
              (modelRuntime->modelDefinition->placementHeightOffsetQ12,
               (modelNode2->worldTransform).translation.y,(modelNode2->worldTransform).translation.x
               ,modelNode2,worldRuntime);
  }
  composedAngles = FixedTransform_ComposeEulerAnglesRegs
                     (0,0x4000 - primaryDelta,
                      (modelRuntime->ownerArmyRuntime->actionVector0Q12 + 0x8000) -
                      (modelNode2->modelPayload).worldRotationAngle2 & 0xffff,
                      (modelNode2->modelPayload).worldRotationAngle2,
                      (modelNode2->modelPayload).worldRotationAngle1,
                      (modelNode2->modelPayload).worldRotationAngle0);
  (modelNode2->modelPayload).worldRotationAngle0 = composedAngles.angle0;
  (modelNode2->modelPayload).worldRotationAngle1 = composedAngles.angle1;
  (modelNode2->modelPayload).worldRotationAngle2 = composedAngles.angle2;
ArmyGroundMovementVariantA_FinalizeEffectsAnimationAndTransforms:
  if (((worldX != (modelNode2->worldTransform).translation.x) ||
      (worldY != (modelNode2->worldTransform).translation.y)) ||
     (previousRotationAngle != (modelNode2->modelPayload).worldRotationAngle2)) {
    linkedOrOwnerArmy = modelRuntime->ownerArmyRuntime;
    ArmyRuntime_UpdateTimedShotAndEffectEmitters
              (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
    ownerMovementFlags = &linkedOrOwnerArmy->movementStateFlags;
    *ownerMovementFlags = *ownerMovementFlags & 0xfffffffb;
  }
  movementDefinition = modelRuntime->modelDefinition;
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNode2);
  ModelNodeRuntime_UpdateDepthBinMasks(movementDefinition->placementRadiusOrClearanceDC,modelNode2);
  return;
}


/* Address: 0x00522090.
   Ownership: gameplay/army/movement.
   Purpose: Updates the left-side articulated terrain contact, retries from the previous contact on obstruction,
   and derives the Q13 interpolation step from the resolved segment length. Four stack arguments are authoritative
   from RET 0x10; EDX:EAX preserves the resolved world X/Y contact pair.
   Local calls: ArmyRuntime_UpdateMovementAndWaypoints.
   Cross-module calls: FixedMath_SinCosScaled [core/math/fixed], FixedMath_Length2 [core/math/fixed],
   FieldGrid_InterpolateTerrainHeightAndNormal [world/terrain/grid],
   ArmyCollision_FindBlockingRuntimeForCurrentUnit [gameplay/army/placement], ArmyRuntime_HandleCollisionPartner
   [gameplay/army/runtime], FixedMath_Length3 [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyArticulatedRuntime_UpdateLeftTerrainContact
          (AngleTurn32 headingAngle16,Q12 contactDistanceLimitQ12,
          ArmyArticulatedRuntimeSlotView *armyRuntime,WorldRuntimeContext *worldRuntime)

{
  GameEntityCommandFlags *entityCommandFlags;
  Q12 *contactStateFlags;
  void *definitionAsset;
  FieldGridAsset *activeFieldGrid;
  uint32_t contactCoordOrLength;
  uint32_t reachOrAngle;
  int contactXOrHeight;
  uint32_t angle;
  int contactYOrStep;
  uint32_t contactY;
  FixedSinCosEdxEax8 lateralSinCos;
  FixedSinCosEdxEax8 headingSinCos;
  ArmyCollisionResult blockingCollision;
  MovementStepResult waypointResult;
  HeightNormalSampleResult terrainSample;
  UQ12 contactRadius;
  ModelRuntimeNode *modelNode1;
  
  modelNode1 = armyRuntime->modelNodeRuntime;
  definitionAsset = armyRuntime->definitionOrAsset;
  armyRuntime->ownerValue64 = headingAngle16;
  (armyRuntime->linkedChildOverloadedState).leftHeadingCommandOrSpawnValue.headingOrTurnValue =
       headingAngle16;
  angle = headingAngle16 + 0x4000 & 0xffff;
  lateralSinCos = FixedMath_SinCosScaled(angle,(armyRuntime->articulatedContact).lateralOffsetQ12);
  contactRadius = (armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.contactRadiusQ12;
  contactCoordOrLength = FixedMath_Length2(((int)(lateralSinCos >> 0x20) + armyRuntime->articulatedCoordinateOrState9C) -
                            (modelNode1->worldTransform).translation.y,
                            ((int)lateralSinCos + armyRuntime->runtimeState94) -
                            (modelNode1->worldTransform).translation.x);
  reachOrAngle = contactCoordOrLength + *(int *)((int)definitionAsset + 0xc0);
  if (reachOrAngle < (uint32_t)contactDistanceLimitQ12) {
    headingSinCos = FixedMath_SinCosScaled(headingAngle16,reachOrAngle);
    contactXOrHeight = (int)headingSinCos + (modelNode1->worldTransform).translation.x;
    contactYOrStep = (int)(headingSinCos >> 0x20) + (modelNode1->worldTransform).translation.y;
  }
  else {
    waypointResult = ArmyRuntime_UpdateMovementAndWaypoints
                       (worldRuntime,(ArmyMovementRuntime *)armyRuntime->linkedEntityRuntime);
    contactYOrStep = waypointResult.worldYQ12;
    contactXOrHeight = waypointResult.worldXQ12;
  }
  contactCoordOrLength = contactXOrHeight + (int)lateralSinCos;
  contactY = contactYOrStep + (int)(lateralSinCos >> 0x20);
  activeFieldGrid = worldRuntime->fieldGrid;
  armyRuntime->runtimeState90 = contactCoordOrLength;
  armyRuntime->runtimeState98 = contactY;
  lateralSinCos = FixedMath_SinCosScaled(angle,contactRadius);
  if (activeFieldGrid == (FieldGridAsset *)0x0) {
    return;
  }
  terrainSample = FieldGrid_InterpolateTerrainHeightAndNormal
                     ((int)(lateralSinCos >> 0x20) + contactY,(int)lateralSinCos + contactCoordOrLength,activeFieldGrid);
  if (!terrainSample.failed) {
    armyRuntime->fallbackWorldYQ12 = terrainSample.packedNormalAngles;
    armyRuntime->articulatedHeightOrStateA0 = terrainSample.heightQ12;
    contactCoordOrLength = armyRuntime->runtimeState98;
    contactXOrHeight = armyRuntime->articulatedHeightOrStateA0;
    blockingCollision = ArmyCollision_FindBlockingRuntimeForCurrentUnit
                       (contactCoordOrLength,armyRuntime->runtimeState90,(RuntimeCollisionQueryViewF4 *)armyRuntime
                        ,worldRuntime);
    if (!blockingCollision.blocked) {
      contactStateFlags = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
      *contactStateFlags = *contactStateFlags & 0xffffff7f;
      goto ArmyArticulatedRuntime_UpdateLeftTerrainContact_ComputeStepFromContact;
    }
    ArmyRuntime_HandleCollisionPartner
              ((ArmyRuntimeSlot *)armyRuntime,
               (armyRuntime->modelNodeRuntime->worldTransform).translation.y,
               (armyRuntime->modelNodeRuntime->worldTransform).translation.x,
               (ArmyRuntimeSlot *)blockingCollision.blockingArmy,worldRuntime);
  }
  reachOrAngle = headingAngle16 + 0x4000 & 0xffff;
  lateralSinCos = FixedMath_SinCosScaled(reachOrAngle,(armyRuntime->articulatedContact).lateralOffsetQ12);
  contactRadius = (armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.contactRadiusQ12;
  contactCoordOrLength = armyRuntime->movementTarget1Q12 + (int)lateralSinCos * 2;
  contactY = armyRuntime->definitionClassValue84 + (int)(lateralSinCos >> 0x20) * 2;
  activeFieldGrid = worldRuntime->fieldGrid;
  armyRuntime->runtimeState90 = contactCoordOrLength;
  armyRuntime->runtimeState98 = contactY;
  lateralSinCos = FixedMath_SinCosScaled(reachOrAngle,contactRadius);
  terrainSample = FieldGrid_InterpolateTerrainHeightAndNormal
                     ((int)(lateralSinCos >> 0x20) + contactY,(int)lateralSinCos + contactCoordOrLength,activeFieldGrid);
  if (terrainSample.failed) {
    return;
  }
  armyRuntime->articulatedHeightOrStateA0 = terrainSample.heightQ12;
  armyRuntime->fallbackWorldYQ12 = terrainSample.packedNormalAngles;
  contactCoordOrLength = armyRuntime->runtimeState98;
  contactXOrHeight = armyRuntime->articulatedHeightOrStateA0;
  entityCommandFlags = &(armyRuntime->linkedEntityRuntime->common).commandFlags;
  *entityCommandFlags = *entityCommandFlags | 0x10;
  if (((armyRuntime->articulatedContact).fallbackPosition0Q12 & 0x80U) != 0) {
    contactStateFlags = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
    *contactStateFlags = *contactStateFlags & 0xfffffffc;
  }
  contactStateFlags = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
  *contactStateFlags = *contactStateFlags | 0x80;
ArmyArticulatedRuntime_UpdateLeftTerrainContact_ComputeStepFromContact:
  definitionAsset = armyRuntime->definitionOrAsset;
  contactCoordOrLength = FixedMath_Length3(contactXOrHeight - armyRuntime->definitionClassValue88,
                            contactCoordOrLength - armyRuntime->definitionClassValue80,
                            armyRuntime->runtimeState90 - armyRuntime->movementTarget0Q12);
  contactXOrHeight = contactCoordOrLength + *(int *)((int)definitionAsset + 0xc4) * 2;
  contactYOrStep = *(int *)((int)definitionAsset + 0xc0);
  (armyRuntime->articulatedContact).fallbackPosition1Q12 = 0x2000;
  if (contactXOrHeight != 0) {
    (armyRuntime->articulatedContact).fallbackPosition1Q12 =
         (Q12)((int64_t)(uint64_t)(uint32_t)(contactYOrStep << 0xd) / (int64_t)contactXOrHeight);
  }
  return;
}


/* Address: 0x005222F0.
   Ownership: gameplay/army/movement.
   Purpose: Updates the right-side articulated terrain contact, retries from the previous contact on obstruction,
   and derives the Q13 interpolation step from the resolved segment length. Four stack arguments are authoritative
   from RET 0x10; EDX:EAX preserves the resolved world X/Y contact pair.
   Local calls: ArmyRuntime_UpdateMovementAndWaypoints.
   Cross-module calls: FixedMath_SinCosScaled [core/math/fixed], FixedMath_Length2 [core/math/fixed],
   FieldGrid_InterpolateTerrainHeightAndNormal [world/terrain/grid],
   ArmyCollision_FindBlockingRuntimeForCurrentUnit [gameplay/army/placement], ArmyRuntime_HandleCollisionPartner
   [gameplay/army/runtime], FixedMath_Length3 [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyArticulatedRuntime_UpdateRightTerrainContact
          (AngleTurn32 headingAngle16,Q12 contactDistanceLimitQ12,
          ArmyArticulatedRuntimeSlotView *armyRuntime,WorldRuntimeContext *worldRuntime)

{
  GameEntityCommandFlags *entityCommandFlags;
  Q12 *contactStateFlags;
  void *definitionAsset;
  FieldGridAsset *activeFieldGrid;
  uint32_t contactCoordOrLength;
  uint32_t reachOrAngle;
  int contactCoordOrDistance;
  uint32_t angle;
  int contactYOrStep;
  FixedSinCosEdxEax8 lateralSinCos;
  FixedSinCosEdxEax8 headingSinCos;
  ArmyCollisionResult blockingCollision;
  MovementStepResult waypointResult;
  HeightNormalSampleResult terrainSample;
  UQ12 contactRadius;
  ModelRuntimeNode *modelNode1;
  
  modelNode1 = armyRuntime->modelNodeRuntime;
  definitionAsset = armyRuntime->definitionOrAsset;
  armyRuntime->ownerValue64 = headingAngle16;
  (armyRuntime->linkedChildOverloadedState).rightHeadingCommandOrSpawnValue.headingOrTurnValue =
       headingAngle16;
  angle = headingAngle16 - 0x4000 & 0xffff;
  lateralSinCos = FixedMath_SinCosScaled(angle,(armyRuntime->articulatedContact).lateralOffsetQ12);
  contactRadius = (armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.contactRadiusQ12;
  contactCoordOrLength = FixedMath_Length2(((int)(lateralSinCos >> 0x20) + armyRuntime->runtimeState98) -
                            (modelNode1->worldTransform).translation.y,
                            ((int)lateralSinCos + armyRuntime->runtimeState90) -
                            (modelNode1->worldTransform).translation.x);
  reachOrAngle = contactCoordOrLength + *(int *)((int)definitionAsset + 0xc0);
  if (reachOrAngle < (uint32_t)contactDistanceLimitQ12) {
    headingSinCos = FixedMath_SinCosScaled(headingAngle16,reachOrAngle);
    contactCoordOrDistance = (int)headingSinCos + (modelNode1->worldTransform).translation.x;
    contactYOrStep = (int)(headingSinCos >> 0x20) + (modelNode1->worldTransform).translation.y;
  }
  else {
    waypointResult = ArmyRuntime_UpdateMovementAndWaypoints
                       (worldRuntime,(ArmyMovementRuntime *)armyRuntime->linkedEntityRuntime);
    contactYOrStep = waypointResult.worldYQ12;
    contactCoordOrDistance = waypointResult.worldXQ12;
  }
  contactCoordOrLength = contactCoordOrDistance + (int)lateralSinCos;
  contactYOrStep = contactYOrStep + (int)(lateralSinCos >> 0x20);
  activeFieldGrid = worldRuntime->fieldGrid;
  armyRuntime->runtimeState94 = contactCoordOrLength;
  armyRuntime->articulatedCoordinateOrState9C = contactYOrStep;
  lateralSinCos = FixedMath_SinCosScaled(angle,contactRadius);
  if (activeFieldGrid == (FieldGridAsset *)0x0) {
    return;
  }
  terrainSample = FieldGrid_InterpolateTerrainHeightAndNormal
                     ((int)(lateralSinCos >> 0x20) + contactYOrStep,(int)lateralSinCos + contactCoordOrLength,activeFieldGrid);
  if (!terrainSample.failed) {
    armyRuntime->fallbackWorldXQ12 = terrainSample.packedNormalAngles;
    armyRuntime->runtimeStateA4 = terrainSample.heightQ12;
    contactCoordOrDistance = armyRuntime->articulatedCoordinateOrState9C;
    contactCoordOrLength = armyRuntime->runtimeStateA4;
    blockingCollision = ArmyCollision_FindBlockingRuntimeForCurrentUnit
                       (contactCoordOrDistance,armyRuntime->runtimeState94,(RuntimeCollisionQueryViewF4 *)armyRuntime
                        ,worldRuntime);
    if (!blockingCollision.blocked) {
      contactStateFlags = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
      *contactStateFlags = *contactStateFlags & 0xffffff7f;
      goto ArmyArticulatedRuntime_UpdateRightTerrainContact_ComputeStepFromContact;
    }
    ArmyRuntime_HandleCollisionPartner
              ((ArmyRuntimeSlot *)armyRuntime,
               (armyRuntime->modelNodeRuntime->worldTransform).translation.y,
               (armyRuntime->modelNodeRuntime->worldTransform).translation.x,
               (ArmyRuntimeSlot *)blockingCollision.blockingArmy,worldRuntime);
  }
  reachOrAngle = headingAngle16 - 0x4000 & 0xffff;
  lateralSinCos = FixedMath_SinCosScaled(reachOrAngle,(armyRuntime->articulatedContact).lateralOffsetQ12);
  contactRadius = (armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.contactRadiusQ12;
  contactCoordOrLength = armyRuntime->movementTarget0Q12 + (int)lateralSinCos * 2;
  contactCoordOrDistance = armyRuntime->definitionClassValue80 + (int)(lateralSinCos >> 0x20) * 2;
  activeFieldGrid = worldRuntime->fieldGrid;
  armyRuntime->runtimeState94 = contactCoordOrLength;
  armyRuntime->articulatedCoordinateOrState9C = contactCoordOrDistance;
  lateralSinCos = FixedMath_SinCosScaled(reachOrAngle,contactRadius);
  terrainSample = FieldGrid_InterpolateTerrainHeightAndNormal
                     ((int)(lateralSinCos >> 0x20) + contactCoordOrDistance,(int)lateralSinCos + contactCoordOrLength,activeFieldGrid);
  if (terrainSample.failed) {
    return;
  }
  armyRuntime->runtimeStateA4 = terrainSample.heightQ12;
  armyRuntime->fallbackWorldXQ12 = terrainSample.packedNormalAngles;
  contactCoordOrDistance = armyRuntime->articulatedCoordinateOrState9C;
  contactCoordOrLength = armyRuntime->runtimeStateA4;
  entityCommandFlags = &(armyRuntime->linkedEntityRuntime->common).commandFlags;
  *entityCommandFlags = *entityCommandFlags | 0x10;
  if (((armyRuntime->articulatedContact).fallbackPosition0Q12 & 0x80U) != 0) {
    contactStateFlags = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
    *contactStateFlags = *contactStateFlags & 0xfffffffc;
  }
  contactStateFlags = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
  *contactStateFlags = *contactStateFlags | 0x80;
ArmyArticulatedRuntime_UpdateRightTerrainContact_ComputeStepFromContact:
  definitionAsset = armyRuntime->definitionOrAsset;
  contactCoordOrLength = FixedMath_Length3(contactCoordOrLength - armyRuntime->runtimeState8C,
                            contactCoordOrDistance - armyRuntime->definitionClassValue84,
                            armyRuntime->runtimeState94 - armyRuntime->movementTarget1Q12);
  contactCoordOrDistance = contactCoordOrLength + *(int *)((int)definitionAsset + 0xc4) * 2;
  contactYOrStep = *(int *)((int)definitionAsset + 0xc0);
  (armyRuntime->articulatedContact).fallbackPosition1Q12 = 0x2000;
  if (contactCoordOrDistance != 0) {
    (armyRuntime->articulatedContact).fallbackPosition1Q12 =
         (Q12)((int64_t)(uint64_t)(uint32_t)(contactYOrStep << 0xd) / (int64_t)contactCoordOrDistance);
  }
  return;
}


/* Address: 0x005254F0.
   Ownership: gameplay/army/movement.
   Purpose: Updates route-following, turning, collision response, model orientation, and depth-bin state for the
   second verified ground-movement runtime class. Runtime-update partition slots 0-23 receive (worldRuntime,
   armyRuntime).
   Local calls: ArmyRuntime_UpdateMovementAndWaypoints.
   Cross-module calls: ArmyCollision_TestPointWithinExpandedRuntimeRadius [gameplay/army/placement],
   FixedMath_Vector2AngleAndLengthRegs [core/math/fixed], ArmyRuntime_UpdateActivationMetricAndPlayStartSound
   [gameplay/army/runtime], FixedTrig_ProjectPlanarPointRegs [core/math/fixed],
   ArmyCollision_FindBlockingRuntimeForCurrentUnit [gameplay/army/placement],
   FixedTransform_ComposeEulerAnglesRegs [core/math/fixed].
*/

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_UpdateGroundMovementVariantB
          (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView200 *modelRuntime
          )

{
  ArmyMovementStateFlags *ownerMovementFlags;
  ArmyRuntimeSlot *linkedOrOwnerArmy;
  AngleTurn32 previousRotationAngle;
  int previousWorldX;
  int previousWorldY;
  ModelDefinitionGroundMovementSteeringView280 *movementDefinition;
  ArmyPlacementContactKindIndex32 placementContactKind;
  int primaryDelta;
  uint32_t facingAngle;
  uint32_t turnVelocityOrLimit;
  int secondaryDelta;
  uint32_t desiredHeading;
  uint32_t headingDifference;
  ModelRuntimeNode *modelNode2;
  bool withinLinkRadius;
  FixedLengthAngleEaxEdx8 angleAndLength;
  FixedPlanarPointEdxEax8 nextPosition;
  ArmyCollisionResult blockingCollision;
  MovementStepResult waypointResult;
  FixedEulerAnglesEaxEbxEdx12 composedAngles;
  Q12 heightOffsetQ12;
  WorldRuntimeContext *dispatchWorldRuntime;
  uint32_t targetDistance;
  ModelRuntimeNode *modelNode1;
  
  linkedOrOwnerArmy = (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime;
  modelNode2 = modelRuntime->rootModelNode;
  ownerMovementFlags = &modelRuntime->ownerArmyRuntime->movementStateFlags;
  *ownerMovementFlags = *ownerMovementFlags | 4;
  /* Drop the linked runtime unless both have a clearance radius and this unit is still within it. */
  if ((linkedOrOwnerArmy != (ArmyRuntimeSlot *)0x0) &&
      ((((((linkedOrOwnerArmy->modelRuntimeOrSavedOffset).modelRuntime)->classState).classStateDC == 0) ||
        (modelRuntime->modelDefinition->placementRadiusOrClearanceDC == 0)) ||
       (withinLinkRadius = ArmyCollision_TestPointWithinExpandedRuntimeRadius
                           (modelRuntime->modelDefinition->placementRadiusOrClearanceDC,
                            (modelNode2->worldTransform).translation.y,
                            (modelNode2->worldTransform).translation.x,linkedOrOwnerArmy), !withinLinkRadius))) {
    (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime = (ArmyRuntimeSlot *)0x0;
  }
  previousRotationAngle = (modelNode2->modelPayload).worldRotationAngle2;
  previousWorldX = (modelNode2->worldTransform).translation.x;
  previousWorldY = (modelNode2->worldTransform).translation.y;
  if (((modelRuntime->classState).classStateEC & 8) == 0) {
    waypointResult = ArmyRuntime_UpdateMovementAndWaypoints
                       (worldRuntime,(ArmyMovementRuntime *)modelRuntime->ownerArmyRuntime);
    if (waypointResult.arrived) goto ArmyGroundMovementVariantB_StopMovementAndTurn;
    secondaryDelta = waypointResult.worldYQ12 - (modelNode2->worldTransform).translation.y;
    primaryDelta = waypointResult.worldXQ12 - (modelNode2->worldTransform).translation.x;
    if ((primaryDelta == 0) && (secondaryDelta == 0)) {
      angleAndLength = THANDOR_BITCAST(uint64_t, FixedLengthAngleEaxEdx8, ((uint64_t)(modelNode2->modelPayload).worldRotationAngle2 << 0x20));
    }
    else {
      angleAndLength = FixedMath_Vector2AngleAndLengthRegs(secondaryDelta,primaryDelta);
    }
    desiredHeading = angleAndLength.angle;
    movementDefinition = modelRuntime->modelDefinition;
    facingAngle = (modelNode2->modelPayload).worldRotationAngle2;
    turnVelocityOrLimit = (modelRuntime->movementControl).turnVelocityAngle16;
    headingDifference = desiredHeading - facingAngle & 0xffff;
    if (headingDifference < 0x8000) {
      if ((int)turnVelocityOrLimit < 0) {
ArmyGroundMovementVariantB_ResetTurnVelocityForDirectionReversal:
        (modelRuntime->movementControl).turnVelocityAngle16 = 0;
      }
      else if (turnVelocityOrLimit < headingDifference) {
        facingAngle = facingAngle + turnVelocityOrLimit;
        secondaryDelta = movementDefinition->turnRateLimitAnglePerTick10 * g_InGameSimulationStepTicks;
        primaryDelta = turnVelocityOrLimit + movementDefinition->turnRateAccelerationAnglePerTick1C * g_InGameSimulationStepTicks;
        (modelRuntime->movementControl).turnVelocityAngle16 = secondaryDelta;
        if (primaryDelta < secondaryDelta) {
          (modelRuntime->movementControl).turnVelocityAngle16 = primaryDelta;
        }
      }
      else {
ArmyGroundMovementVariantB_SnapFacingToDesiredHeading:
        (modelRuntime->movementControl).turnVelocityAngle16 = 0;
        facingAngle = desiredHeading;
      }
    }
    else {
      if (0 < (int)turnVelocityOrLimit) goto ArmyGroundMovementVariantB_ResetTurnVelocityForDirectionReversal;
      if (turnVelocityOrLimit + 0x10000 <= headingDifference) goto ArmyGroundMovementVariantB_SnapFacingToDesiredHeading;
      facingAngle = facingAngle + turnVelocityOrLimit;
      secondaryDelta = -movementDefinition->turnRateLimitAnglePerTick10 * g_InGameSimulationStepTicks;
      primaryDelta = turnVelocityOrLimit - movementDefinition->turnRateAccelerationAnglePerTick1C * g_InGameSimulationStepTicks;
      (modelRuntime->movementControl).turnVelocityAngle16 = secondaryDelta;
      if (secondaryDelta < primaryDelta) {
        (modelRuntime->movementControl).turnVelocityAngle16 = primaryDelta;
      }
    }
    modelNode2 = modelRuntime->rootModelNode;
    facingAngle = facingAngle & 0xffff;
    if (facingAngle != (modelNode2->modelPayload).worldRotationAngle2) {
      (modelNode2->modelPayload).worldRotationAngle2 = facingAngle;
      modelNode2->runtimeFlags = modelNode2->runtimeFlags | 1;
    }
    targetDistance = angleAndLength.length;
    turnVelocityOrLimit = movementDefinition->farHeadingErrorLimitAngleC4;
    facingAngle = facingAngle - desiredHeading & 0xffff;
    if ((int)targetDistance < movementDefinition->headingErrorInterpolationDistanceQ12C0) {
      turnVelocityOrLimit = movementDefinition->nearHeadingErrorLimitAngleC8 +
               (int)(((int64_t)(int)(turnVelocityOrLimit - movementDefinition->nearHeadingErrorLimitAngleC8) *
                     (int64_t)(int)targetDistance) /
                    (int64_t)movementDefinition->headingErrorInterpolationDistanceQ12C0);
    }
    if ((turnVelocityOrLimit < facingAngle) && (facingAngle < 0x10000 - turnVelocityOrLimit)) {
      (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
      goto ArmyGroundMovementVariantB_ProcessStationaryPlacementAndDamageState;
    }
    ArmyRuntime_UpdateActivationMetricAndPlayStartSound
              (worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
    primaryDelta = (modelRuntime->movementControl).movementAdvancePerTickQ12 * g_InGameSimulationStepTicks;
    if (primaryDelta < (int)targetDistance >> 1) {
      nextPosition = FixedTrig_ProjectPlanarPointRegs
                         (primaryDelta,(modelNode2->modelPayload).worldRotationAngle2,
                          (modelNode2->worldTransform).translation.y,
                          (modelNode2->worldTransform).translation.x);
    }
    else {
      linkedOrOwnerArmy = modelRuntime->ownerArmyRuntime;
      waypointResult = ArmyRuntime_UpdateMovementAndWaypoints(worldRuntime,(ArmyMovementRuntime *)linkedOrOwnerArmy);
      nextPosition = THANDOR_PART(uint64_t, waypointResult, 0);
      ownerMovementFlags = &linkedOrOwnerArmy->movementStateFlags;
      *ownerMovementFlags = *ownerMovementFlags | 0x10;
    }
    placementContactKind = movementDefinition->placementContactKindIndex278;
    modelNode2 = modelRuntime->rootModelNode;
    heightOffsetQ12 = movementDefinition->placementHeightOffsetQ12;
    dispatchWorldRuntime = worldRuntime;
    blockingCollision = ArmyCollision_FindBlockingRuntimeForCurrentUnit
                       ((Q12)(nextPosition >> 0x20),(Q12)nextPosition,
                        (RuntimeCollisionQueryViewF4 *)modelRuntime,worldRuntime);
    if (blockingCollision.blocked) {
      modelNode1 = modelRuntime->rootModelNode;
      (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
      THANDOR_PART(uint32_t, nextPosition, 0) = (modelNode1->worldTransform).translation.x;
      THANDOR_PART(uint32_t, nextPosition, 4) = (modelNode1->worldTransform).translation.y;
      ownerMovementFlags = &modelRuntime->ownerArmyRuntime->movementStateFlags;
      *ownerMovementFlags = *ownerMovementFlags | 0x10;
    }
    linkedOrOwnerArmy = modelRuntime->ownerArmyRuntime;
    g_ArmyPlacementContactKindDispatchTable.callbacks[placementContactKind]
              (heightOffsetQ12,(Q12)(nextPosition >> 0x20),(Q12)nextPosition,modelNode2,dispatchWorldRuntime);
    modelNode2 = modelRuntime->rootModelNode;
    primaryDelta = linkedOrOwnerArmy->actionVector1Q12 + -1;
    if (primaryDelta < 0) goto ArmyGroundMovementVariantB_FinalizeEffectsAnimationAndTransforms;
    primaryDelta = primaryDelta * linkedOrOwnerArmy->actionVector2Q12;
    linkedOrOwnerArmy->actionVector1Q12 = linkedOrOwnerArmy->actionVector1Q12 + -1;
  }
  else {
ArmyGroundMovementVariantB_StopMovementAndTurn:
    (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
    (modelRuntime->movementControl).turnVelocityAngle16 = 0;
ArmyGroundMovementVariantB_ProcessStationaryPlacementAndDamageState:
    linkedOrOwnerArmy = modelRuntime->ownerArmyRuntime;
    modelNode2 = modelRuntime->rootModelNode;
    primaryDelta = linkedOrOwnerArmy->actionVector1Q12 + -1;
    if (primaryDelta < 0) {
      if ((worldRuntime->fieldGrid->runtimeStateFlags & 1) != 0) {
        (*g_ArmyPlacementContactKindDispatchTable.callbacks
          [modelRuntime->modelDefinition->placementContactKindIndex278])
                  (modelRuntime->modelDefinition->placementHeightOffsetQ12,
                   (modelNode2->worldTransform).translation.y,
                   (modelNode2->worldTransform).translation.x,modelNode2,worldRuntime);
      }
      goto ArmyGroundMovementVariantB_FinalizeEffectsAnimationAndTransforms;
    }
    primaryDelta = primaryDelta * linkedOrOwnerArmy->actionVector2Q12;
    linkedOrOwnerArmy->actionVector1Q12 = linkedOrOwnerArmy->actionVector1Q12 + -1;
    (*g_ArmyPlacementContactKindDispatchTable.callbacks
      [modelRuntime->modelDefinition->placementContactKindIndex278])
              (modelRuntime->modelDefinition->placementHeightOffsetQ12,
               (modelNode2->worldTransform).translation.y,(modelNode2->worldTransform).translation.x
               ,modelNode2,worldRuntime);
  }
  composedAngles = FixedTransform_ComposeEulerAnglesRegs
                     (0,0x4000 - primaryDelta,
                      (modelRuntime->ownerArmyRuntime->actionVector0Q12 + 0x8000) -
                      (modelNode2->modelPayload).worldRotationAngle2 & 0xffff,
                      (modelNode2->modelPayload).worldRotationAngle2,
                      (modelNode2->modelPayload).worldRotationAngle1,
                      (modelNode2->modelPayload).worldRotationAngle0);
  (modelNode2->modelPayload).worldRotationAngle0 = composedAngles.angle0;
  (modelNode2->modelPayload).worldRotationAngle1 = composedAngles.angle1;
  (modelNode2->modelPayload).worldRotationAngle2 = composedAngles.angle2;
ArmyGroundMovementVariantB_FinalizeEffectsAnimationAndTransforms:
  if (((previousWorldX != (modelNode2->worldTransform).translation.x) ||
      (previousWorldY != (modelNode2->worldTransform).translation.y)) ||
     (previousRotationAngle != (modelNode2->modelPayload).worldRotationAngle2)) {
    linkedOrOwnerArmy = modelRuntime->ownerArmyRuntime;
    ArmyRuntime_UpdateTimedShotAndEffectEmitters
              (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
    ownerMovementFlags = &linkedOrOwnerArmy->movementStateFlags;
    *ownerMovementFlags = *ownerMovementFlags & 0xfffffffb;
  }
  movementDefinition = modelRuntime->modelDefinition;
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNode2);
  ModelNodeRuntime_UpdateDepthBinMasks(movementDefinition->placementRadiusOrClearanceDC,modelNode2);
  return;
}


/* Address: 0x0051CC60.
   Ownership: gameplay/army/movement.
   Purpose: Clamps an oversized requested displacement to the verified 0x2000 distance and starts a movement route
   toward the resulting point.
   Cross-module calls: FixedMath_Vector2AngleAndLengthRegs [core/math/fixed], FixedMath_SinCosScaled
   [core/math/fixed], EntityPathing_ResolveDestinationAndRebuildRoutes [world/pathing/grid].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_StartClampedMoveCommand
          (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime)

{
  GraphicsWorldCoordinateQ12 currentWorldX;
  GraphicsWorldCoordinateQ12 currentWorldY;
  InGameRuntimeRootImageC3E4 *inGameRoot;
  FixedLengthAngleEaxEdx8 offsetAngleLength;
  FixedSinCosEdxEax8 clampedOffset;
  PathingDestinationResult resolvedDestination;
  
  inGameRoot = g_InGameRuntimeRoot;
  if (((movementRuntime->movementStateFlags & 3) == 0) && (movementRuntime->retryCountdown == 0)) {
    offsetAngleLength = FixedMath_Vector2AngleAndLengthRegs
                      (targetWorldY - movementRuntime->movementTargetWorldYQ12,
                       targetWorldX - movementRuntime->movementTargetWorldXQ12);
    if (0x2000 < (int)offsetAngleLength.length) {
      clampedOffset = FixedMath_SinCosScaled(offsetAngleLength.angle,0x2000);
      targetWorldX = (int)clampedOffset + movementRuntime->movementTargetWorldXQ12;
      targetWorldY = (int)(clampedOffset >> 0x20) + movementRuntime->movementTargetWorldYQ12;
    }
    movementRuntime->movementStateFlags = movementRuntime->movementStateFlags & 0xfffffba7;
    movementRuntime->movementStateFlags = movementRuntime->movementStateFlags | 0xa1;
    resolvedDestination = EntityPathing_ResolveDestinationAndRebuildRoutes
                      (targetWorldY,targetWorldX,movementRuntime->entityRuntime,
                       &inGameRoot->worldRuntime0A30);
    (movementRuntime->fallbackPosition).worldXQ12 = resolvedDestination.fallbackWorldXQ12;
    (movementRuntime->fallbackPosition).worldYQ12 = resolvedDestination.fallbackWorldYQ12;
    movementRuntime->movementWorldXQ12 = resolvedDestination.primaryWorldXQ12;
    movementRuntime->movementWorldYQ12 = resolvedDestination.primaryWorldYQ12;
    currentWorldX = (movementRuntime->modelNodeRuntime->worldTransform).translation.x;
    currentWorldY = (movementRuntime->modelNodeRuntime->worldTransform).translation.y;
    movementRuntime->retryCountdown = 0x40;
    movementRuntime->lastCheckedWorldXQ12 = currentWorldX;
    movementRuntime->lastCheckedWorldYQ12 = currentWorldY;
  }
  return;
}


/* Address: 0x0051CD30.
   Starts a direct move to the target (unless the movement is locked): drops the waypoint queue and
   routes to the target, but leaves the final movement target unchanged. Used by
   ArmyRuntime_UpdateMovementAndWaypoints to re-approach the final target.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_StartDirectMoveCommand
          (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime)

{
  GraphicsWorldCoordinateQ12 currentWorldX;
  GraphicsWorldCoordinateQ12 currentWorldY;
  WorldRuntimeContext *worldRuntime;
  PathingDestinationResult resolvedDestination;

  if ((movementRuntime->movementStateFlags & ARMY_MOVEMENT_LOCKED) == 0) {
    worldRuntime = &g_InGameRuntimeRoot->worldRuntime0A30;
    movementRuntime->movementStateFlags =
         movementRuntime->movementStateFlags | (ARMY_MOVEMENT_DIRECT | ARMY_MOVEMENT_ACTIVE);
    movementRuntime->movementStateFlags =
         movementRuntime->movementStateFlags &
         ~(0x40 | ARMY_MOVEMENT_TARGET_FOLLOWING | 0x10 | ARMY_MOVEMENT_WAYPOINTS_QUEUED);
    resolvedDestination = EntityPathing_ResolveDestinationAndRebuildRoutes
                      (targetWorldY,targetWorldX,movementRuntime->entityRuntime,worldRuntime);
    (movementRuntime->fallbackPosition).worldXQ12 = resolvedDestination.fallbackWorldXQ12;
    (movementRuntime->fallbackPosition).worldYQ12 = resolvedDestination.fallbackWorldYQ12;
    movementRuntime->movementWorldXQ12 = resolvedDestination.primaryWorldXQ12;
    movementRuntime->movementWorldYQ12 = resolvedDestination.primaryWorldYQ12;
    currentWorldX = (movementRuntime->modelNodeRuntime->worldTransform).translation.x;
    currentWorldY = (movementRuntime->modelNodeRuntime->worldTransform).translation.y;
    movementRuntime->retryCountdown = ARMY_MOVEMENT_RETRY_TICKS;
    movementRuntime->lastCheckedWorldXQ12 = currentWorldX;
    movementRuntime->lastCheckedWorldYQ12 = currentWorldY;
  }
  return;
}


/* Address: 0x00521580.
   Ownership: gameplay/army/movement.
   Purpose: Updates one articulated contact child, resolves terrain interaction, and emits the linked effect state.
   Cross-module calls: TerrainGrid_TestProjectedCellMaskBits01 [world/terrain/grid],
   SpatialSound_PlayPositionedOneShot [audio/spatial/runtime], FieldGrid_GetNearestWaterDelta [world/terrain/grid],
   EffectRuntimePool_CreateInstanceFromDefinition [world/effects/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyArticulatedRuntime_UpdateContactChildAndEffects
          (ModelRuntimeNode *contactChildModel,WorldRuntimeContext *worldRuntime,
          ArmyRuntimeSlot *armyRuntime)

{
  GraphicsFixedVec3 *worldPosition;
  ModelRuntimeSlot *armyModelRuntime;
  uint32_t soundIndex;
  DirectSoundVoiceSet **voiceSetRef;
  int32_t waterDelta;
  EffectDefinition *effectDefinition;
  bool cellMasked;
  ModelRuntimeNode *modelNode1;
  
  armyModelRuntime = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  soundIndex = *(uint32_t *)(armyModelRuntime->classState).reservedCC_CF;
  if ((((soundIndex != 0) && (soundIndex < worldRuntime->dwordArrayCount)) &&
      (worldRuntime->dwordArray != (uint32_t *)0x0)) &&
     (voiceSetRef = (DirectSoundVoiceSet **)worldRuntime->dwordArray[soundIndex],
     voiceSetRef != (DirectSoundVoiceSet **)0x0)) {
    worldPosition = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
    cellMasked = TerrainGrid_TestProjectedCellMaskBits01
                      ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,
                       worldPosition->x,worldRuntime);
    if (!cellMasked) {
      SpatialSound_PlayPositionedOneShot
                ((armyModelRuntime->classLinkState).classState7C,(armyModelRuntime->classLinkState).classState78,
                 worldPosition,voiceSetRef);
    }
  }
  modelNode1 = contactChildModel->childNodes[0]->childNodes[0]->childNodes[0];
  waterDelta = FieldGrid_GetNearestWaterDelta
                    ((modelNode1->worldTransform).translation.y,
                     (modelNode1->worldTransform).translation.x,worldRuntime->fieldGrid);
  if (waterDelta < 1) {
    effectDefinition = (EffectDefinition *)armyModelRuntime->attachments140[1].childLocalRotationAngle1;
  }
  else {
    effectDefinition = (EffectDefinition *)armyModelRuntime->definitionValueB4_58;
  }
  if (effectDefinition != (EffectDefinition *)0x0) {
    EffectRuntimePool_CreateInstanceFromDefinition
              (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference4, 0x0),
               (modelNode1->modelPayload).worldRotationAngle2,
               (modelNode1->modelPayload).worldRotationAngle1,
               (modelNode1->modelPayload).worldRotationAngle0,
               (modelNode1->worldTransform).translation.z,(modelNode1->worldTransform).translation.y
               ,(modelNode1->worldTransform).translation.x,effectDefinition,worldRuntime);
  }
  return;
}


/* Address: 0x005217A0.
   Ownership: gameplay/army/movement.
   Purpose: Updates paired articulated subtrees from interpolated contact points, solves their joint angles, and
   rebuilds the root and child transforms for the current suspension state. Two stack arguments are authoritative
   from RET 0x08. The second world-runtime argument remains part of the caller contract even where the
   implementation reuses its stack slot.
   Cross-module calls: FixedMath_WriteDirectionQ28 [core/math/fixed], FixedMath_VectorToAnglesVec3Regs
   [core/math/fixed], ModelNodeRuntime_RebuildTransformsFromRoot [world/model/hierarchy],
   FixedMath_Vector2AngleAndLengthRegs [core/math/fixed], FixedMath_SinCosScaled [core/math/fixed],
   FixedMath_VectorToAnglesAndLength3Regs [core/math/fixed].
*/

void __thandor_void_preserve_eax_ecx_edx
ArmyArticulatedRuntime_UpdateSuspensionHierarchy
          (ModelRuntimeNode *modelNodeRuntime,WorldRuntimeContext *worldRuntime)

{
  GraphicsWorldCoordinateQ12 *nodeTranslationZ;
  uint32_t leftBlendQ12;
  int leftHeading;
  int leftPreviousHeading;
  int leftHeadingBase;
  ArmyTerrainContactDispatchMode rightBlendQ12;
  int rightPreviousHeading;
  int rightHeadingBase;
  void *definitionAsset;
  int rightNodeY;
  int rightNodeZ;
  AngleTurn32 rootHeading;
  int64_t blendProduct;
  uint32_t leftYOrSideLength;
  uint32_t sideLength0Q12;
  short leftRelativeAngle;
  int inverseBlendOrRightHeading;
  int leftXOrAngle;
  uint32_t leftYawOffset;
  uint32_t leftRelativeMasked;
  short rightRelativeAngle;
  uint32_t rightRelativeRaw;
  uint32_t rightLengthOrAngle;
  int leftYOrElevation;
  FixedLengthAngleEaxEdx8 leftPlanarVector;
  FixedLengthAngleEaxEdx8 rightPlanarVector;
  FixedSinCosEdxEax8 contactOffset;
  FixedTriangleJointAnglesEaxEdx8 jointAngles;
  FixedLengthAnglesEaxEcxEdx12 leftLegVector;
  FixedLengthAnglesEaxEcxEdx12 rightLegVector;
  FixedEulerAnglesEaxEcxEdx12 extractedAngles;
  FixedElevationAzimuth leftBlendAngles;
  FixedElevationAzimuth rightBlendAngles;
  UQ12 scale;
  GraphicsWorldCoordinateQ12 leftContactX;
  GraphicsWorldCoordinateQ12 leftContactY;
  GraphicsWorldCoordinateQ12 leftContactZ;
  GraphicsWorldCoordinateQ12 rightContactX;
  int heightOrRightTargetX;
  GraphicsWorldCoordinateQ12 rightContactY;
  int rightXOrTargetY;
  GraphicsWorldCoordinateQ12 rightContactZ;
  AngleTurn32 rightBlendAngleEcx;
  AngleTurn32 rightBlendAngleEdx;
  AngleTurn32 leftBlendAngleEcx;
  AngleTurn32 leftBlendAngleEdx;
  ArmyArticulatedRuntimeSlotView *articulatedRuntime;
  ArmyRuntimeSlot *armySlot1;
  ModelRuntimeNode *modelNode1;
  ModelRuntimeNode *modelNode2;
  
  articulatedRuntime = (ArmyArticulatedRuntimeSlotView *)(modelNodeRuntime->runtimePayload).armyRuntime;
  modelNode2 = modelNodeRuntime->childNodes[0]->childNodes[0]->childNodes[0]->childNodes[0];
  leftBlendQ12 = articulatedRuntime->runtimeStateA8;
  blendProduct = (int64_t)(int)(articulatedRuntime->runtimeState90 - articulatedRuntime->movementTarget0Q12) *
           (int64_t)(int)leftBlendQ12;
  leftYOrSideLength = articulatedRuntime->runtimeState98;
  (modelNode2->worldTransform).translation.x =
       ((int)((uint64_t)blendProduct >> 0x20) << 0x14 | (uint32_t)blendProduct >> 0xc) + articulatedRuntime->movementTarget0Q12
  ;
  blendProduct = (int64_t)(int)(leftYOrSideLength - articulatedRuntime->definitionClassValue80) * (int64_t)(int)leftBlendQ12;
  (modelNode2->worldTransform).translation.y =
       ((int)((uint64_t)blendProduct >> 0x20) << 0x14 | (uint32_t)blendProduct >> 0xc) +
       articulatedRuntime->definitionClassValue80;
  blendProduct = (int64_t)
           (int)((((int)((0x1000 - leftBlendQ12) * *(int *)((int)articulatedRuntime->definitionOrAsset + 0xc4)) >> 10)
                 + articulatedRuntime->articulatedHeightOrStateA0) - articulatedRuntime->definitionClassValue88) *
           (int64_t)(int)leftBlendQ12;
  (modelNode2->worldTransform).translation.z =
       ((int)((uint64_t)blendProduct >> 0x20) << 0x14 | (uint32_t)blendProduct >> 0xc) +
       articulatedRuntime->definitionClassValue88;
  leftHeading = (articulatedRuntime->linkedChildOverloadedState).leftHeadingCommandOrSpawnValue.signedValue;
  leftPreviousHeading = (articulatedRuntime->linkedChildOverloadedState).primaryCoordinateCommandOrHistory.signedValue;
  leftHeadingBase = (articulatedRuntime->linkedChildOverloadedState).primaryCoordinateCommandOrHistory.signedValue;
  FixedMath_WriteDirectionQ28
            ((GraphicsFixedVec3 *)&g_ArmySuspensionBlendVectorAXQ12,
             articulatedRuntime->fallbackWorldYQ12 >> 0x10,articulatedRuntime->fallbackWorldYQ12 & 0xffff);
  FixedMath_WriteDirectionQ28
            ((GraphicsFixedVec3 *)&g_ArmySuspensionBlendVectorBXQ12,
             (int)articulatedRuntime->ownerValue68 >> 0x10,articulatedRuntime->ownerValue68 & 0xffff);
  inverseBlendOrRightHeading = 0x1000 - articulatedRuntime->runtimeStateA8;
  g_ArmySuspensionBlendVectorAXQ12 =
       ((int)((uint64_t)((int64_t)(int)g_ArmySuspensionBlendVectorAXQ12 * (int64_t)(int)leftBlendQ12) >> 0x20
             ) << 0x14 |
       (uint32_t)((int64_t)(int)g_ArmySuspensionBlendVectorAXQ12 * (int64_t)(int)leftBlendQ12) >> 0xc) +
       ((int)((uint64_t)((int64_t)(int)g_ArmySuspensionBlendVectorBXQ12 * (int64_t)inverseBlendOrRightHeading) >> 0x20) <<
        0x14 | (uint32_t)((int64_t)(int)g_ArmySuspensionBlendVectorBXQ12 * (int64_t)inverseBlendOrRightHeading) >> 0xc);
  g_ArmySuspensionBlendVectorAYQ12 =
       ((int)((uint64_t)((int64_t)(int)g_ArmySuspensionBlendVectorAYQ12 * (int64_t)(int)leftBlendQ12) >> 0x20
             ) << 0x14 |
       (uint32_t)((int64_t)(int)g_ArmySuspensionBlendVectorAYQ12 * (int64_t)(int)leftBlendQ12) >> 0xc) +
       ((int)((uint64_t)((int64_t)(int)g_ArmySuspensionBlendVectorBYQ12 * (int64_t)inverseBlendOrRightHeading) >> 0x20) <<
        0x14 | (uint32_t)((int64_t)(int)g_ArmySuspensionBlendVectorBYQ12 * (int64_t)inverseBlendOrRightHeading) >> 0xc);
  g_ArmySuspensionBlendVectorAZQ12 =
       ((int)((uint64_t)((int64_t)(int)g_ArmySuspensionBlendVectorAZQ12 * (int64_t)(int)leftBlendQ12) >> 0x20
             ) << 0x14 |
       (uint32_t)((int64_t)(int)g_ArmySuspensionBlendVectorAZQ12 * (int64_t)(int)leftBlendQ12) >> 0xc) +
       ((int)((uint64_t)((int64_t)(int)g_ArmySuspensionBlendVectorBZQ12 * (int64_t)inverseBlendOrRightHeading) >> 0x20) <<
        0x14 | (uint32_t)((int64_t)(int)g_ArmySuspensionBlendVectorBZQ12 * (int64_t)inverseBlendOrRightHeading) >> 0xc);
  leftBlendAngles = FixedMath_VectorToAnglesVec3Regs((GraphicsFixedVec3 *)&g_ArmySuspensionBlendVectorAXQ12);
  modelNode1 = modelNodeRuntime->childNodes[1]->childNodes[0]->childNodes[0]->childNodes[0];
  rightBlendQ12 = (articulatedRuntime->articulatedContact).terrainContactMode;
  blendProduct = (int64_t)(int)(articulatedRuntime->runtimeState94 - articulatedRuntime->movementTarget1Q12) *
           (int64_t)(int)rightBlendQ12;
  inverseBlendOrRightHeading = articulatedRuntime->articulatedCoordinateOrState9C;
  (modelNode1->worldTransform).translation.x =
       ((int)((uint64_t)blendProduct >> 0x20) << 0x14 | (uint32_t)blendProduct >> 0xc) + articulatedRuntime->movementTarget1Q12
  ;
  blendProduct = (int64_t)(int)(inverseBlendOrRightHeading - articulatedRuntime->definitionClassValue84) * (int64_t)(int)rightBlendQ12;
  (modelNode1->worldTransform).translation.y =
       ((int)((uint64_t)blendProduct >> 0x20) << 0x14 | (uint32_t)blendProduct >> 0xc) +
       articulatedRuntime->definitionClassValue84;
  blendProduct = (int64_t)
           (int)((((int)((0x1000 - rightBlendQ12) * *(int *)((int)articulatedRuntime->definitionOrAsset + 0xc4)) >> 10)
                 + articulatedRuntime->runtimeStateA4) - articulatedRuntime->runtimeState8C) * (int64_t)(int)rightBlendQ12;
  (modelNode1->worldTransform).translation.z =
       ((int)((uint64_t)blendProduct >> 0x20) << 0x14 | (uint32_t)blendProduct >> 0xc) + articulatedRuntime->runtimeState8C;
  inverseBlendOrRightHeading = (articulatedRuntime->linkedChildOverloadedState).rightHeadingCommandOrSpawnValue.signedValue;
  rightPreviousHeading = (articulatedRuntime->linkedChildOverloadedState).secondaryCoordinateCommandOrHistory.signedValue;
  rightHeadingBase = (articulatedRuntime->linkedChildOverloadedState).secondaryCoordinateCommandOrHistory.signedValue;
  FixedMath_WriteDirectionQ28
            ((GraphicsFixedVec3 *)&g_ArmySuspensionBlendVectorAXQ12,
             articulatedRuntime->fallbackWorldXQ12 >> 0x10,articulatedRuntime->fallbackWorldXQ12 & 0xffff);
  FixedMath_WriteDirectionQ28
            ((GraphicsFixedVec3 *)&g_ArmySuspensionBlendVectorBXQ12,
             (int)articulatedRuntime->linkedArmyRuntimeOrSavedOffset >> 0x10,
             (uint32_t)articulatedRuntime->linkedArmyRuntimeOrSavedOffset & 0xffff);
  leftXOrAngle = 0x1000 - (articulatedRuntime->articulatedContact).terrainContactMode;
  g_ArmySuspensionBlendVectorAXQ12 =
       ((int)((uint64_t)((int64_t)(int)g_ArmySuspensionBlendVectorAXQ12 * (int64_t)(int)rightBlendQ12) >> 0x20
             ) << 0x14 |
       (uint32_t)((int64_t)(int)g_ArmySuspensionBlendVectorAXQ12 * (int64_t)(int)rightBlendQ12) >> 0xc) +
       ((int)((uint64_t)((int64_t)(int)g_ArmySuspensionBlendVectorBXQ12 * (int64_t)leftXOrAngle) >> 0x20) <<
        0x14 | (uint32_t)((int64_t)(int)g_ArmySuspensionBlendVectorBXQ12 * (int64_t)leftXOrAngle) >> 0xc);
  g_ArmySuspensionBlendVectorAYQ12 =
       ((int)((uint64_t)((int64_t)(int)g_ArmySuspensionBlendVectorAYQ12 * (int64_t)(int)rightBlendQ12) >> 0x20
             ) << 0x14 |
       (uint32_t)((int64_t)(int)g_ArmySuspensionBlendVectorAYQ12 * (int64_t)(int)rightBlendQ12) >> 0xc) +
       ((int)((uint64_t)((int64_t)(int)g_ArmySuspensionBlendVectorBYQ12 * (int64_t)leftXOrAngle) >> 0x20) <<
        0x14 | (uint32_t)((int64_t)(int)g_ArmySuspensionBlendVectorBYQ12 * (int64_t)leftXOrAngle) >> 0xc);
  g_ArmySuspensionBlendVectorAZQ12 =
       ((int)((uint64_t)((int64_t)(int)g_ArmySuspensionBlendVectorAZQ12 * (int64_t)(int)rightBlendQ12) >> 0x20
             ) << 0x14 |
       (uint32_t)((int64_t)(int)g_ArmySuspensionBlendVectorAZQ12 * (int64_t)(int)rightBlendQ12) >> 0xc) +
       ((int)((uint64_t)((int64_t)(int)g_ArmySuspensionBlendVectorBZQ12 * (int64_t)leftXOrAngle) >> 0x20) <<
        0x14 | (uint32_t)((int64_t)(int)g_ArmySuspensionBlendVectorBZQ12 * (int64_t)leftXOrAngle) >> 0xc);
  rightBlendAngles = FixedMath_VectorToAnglesVec3Regs((GraphicsFixedVec3 *)&g_ArmySuspensionBlendVectorAXQ12);
  definitionAsset = articulatedRuntime->definitionOrAsset;
  LOCK();
  UNLOCK();
  armySlot1 = (modelNodeRuntime->runtimePayload).armyRuntime;
  (modelNodeRuntime->modelPayload).worldRotationAngle2 =
       (((int)((armySlot1->ownerValue64 - armySlot1->classState60) * 0x10000) >> 0x10) *
        (armySlot1->runtimeStateA8 + (armySlot1->articulatedContact).terrainContactMode) >> 0xc) +
       armySlot1->classState60 & 0xffff;
  leftXOrAngle = (modelNode2->worldTransform).translation.x;
  leftYOrElevation = (modelNode2->worldTransform).translation.y;
  heightOrRightTargetX = ((modelNode1->modelPayload).modelResource)->placementHeightOffsetQ12;
  rightXOrTargetY = (modelNode1->worldTransform).translation.x;
  rightNodeY = (modelNode1->worldTransform).translation.y;
  nodeTranslationZ = &(modelNode2->worldTransform).translation.z;
  *nodeTranslationZ = *nodeTranslationZ + heightOrRightTargetX;
  nodeTranslationZ = &(modelNode1->worldTransform).translation.z;
  *nodeTranslationZ = *nodeTranslationZ + heightOrRightTargetX;
  heightOrRightTargetX = (modelNode2->worldTransform).translation.z;
  rightNodeZ = (modelNode1->worldTransform).translation.z;
  (modelNodeRuntime->worldTransform).translation.x = leftXOrAngle + rightXOrTargetY >> 1;
  (modelNodeRuntime->worldTransform).translation.y = leftYOrElevation + rightNodeY >> 1;
  (modelNodeRuntime->worldTransform).translation.z =
       (heightOrRightTargetX + rightNodeZ >> 1) + *(int *)((int)definitionAsset + 0x54);
  rightContactZ = (modelNode1->worldTransform).translation.z;
  rightContactY = (modelNode1->worldTransform).translation.y;
  rightContactX = (modelNode1->worldTransform).translation.x;
  leftContactZ = (modelNode2->worldTransform).translation.z;
  leftContactY = (modelNode2->worldTransform).translation.y;
  leftContactX = (modelNode2->worldTransform).translation.x;
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
  modelNode2 = modelNodeRuntime->childNodes[0];
  modelNode1 = modelNodeRuntime->childNodes[1];
  leftPlanarVector = FixedMath_Vector2AngleAndLengthRegs
                     (leftContactY - (modelNode2->worldTransform).translation.y,
                      leftContactX - (modelNode2->worldTransform).translation.x);
  leftYOrSideLength = leftPlanarVector.length;
  rightPlanarVector = FixedMath_Vector2AngleAndLengthRegs
                     (rightContactY - (modelNode1->worldTransform).translation.y,
                      rightContactX - (modelNode1->worldTransform).translation.x);
  rightLengthOrAngle = rightPlanarVector.length;
  rootHeading = (modelNodeRuntime->modelPayload).worldRotationAngle2;
  leftYawOffset = leftPlanarVector.angle - rootHeading;
  rightRelativeRaw = rightPlanarVector.angle - rootHeading;
  leftRelativeMasked = leftYawOffset & 0xffff;
  leftRelativeAngle = (short)leftYawOffset;
  leftYawOffset = rightRelativeRaw & 0xffff;
  rightRelativeAngle = (short)rightRelativeRaw;
  if ((0x3fff < leftRelativeMasked) && (leftRelativeMasked < 0xc001)) {
    leftRelativeAngle = leftRelativeAngle + -0x8000;
  }
  if ((0x3fff < leftYawOffset) && (leftYawOffset < 0xc001)) {
    rightRelativeAngle = rightRelativeAngle + -0x8000;
  }
  leftYawOffset = (uint32_t)leftRelativeAngle;
  leftXOrAngle = (int)rightRelativeAngle;
  if (rightLengthOrAngle < 0x140) {
    if (rightLengthOrAngle < 0x40) {
      leftXOrAngle = 0;
    }
    else {
      leftXOrAngle = (int)(leftXOrAngle * (rightLengthOrAngle - 0x40)) >> 8;
    }
  }
  if (leftYOrSideLength < 0x140) {
    if (leftYOrSideLength < 0x40) {
      leftYawOffset = 0;
    }
    else {
      leftYawOffset = (int)(leftYawOffset * (leftYOrSideLength - 0x40)) >> 8;
    }
  }
  rootHeading = (modelNodeRuntime->modelPayload).worldRotationAngle2;
  rightLengthOrAngle = leftXOrAngle + 0x8000U & 0xffff;
  (modelNode2->modelPayload).localRotationAngle2 = leftYawOffset & 0xffff;
  (modelNode1->modelPayload).localRotationAngle2 = rightLengthOrAngle;
  rightLengthOrAngle = (rootHeading - 0x4000) + rightLengthOrAngle & 0xffff;
  scale = (((modelNodeRuntime->runtimePayload).armyRuntime)->articulatedContact).
          contactRadiusOrLinkedSlotMask.contactRadiusQ12;
  contactOffset = FixedMath_SinCosScaled
                     (rootHeading + 0x4000 + (leftYawOffset & 0xffff) & 0xffff,
                      (((modelNodeRuntime->runtimePayload).armyRuntime)->articulatedContact).
                      contactRadiusOrLinkedSlotMask.contactRadiusQ12);
  leftXOrAngle = leftContactX + (int)contactOffset;
  leftYOrElevation = leftContactY + (int)(contactOffset >> 0x20);
  contactOffset = FixedMath_SinCosScaled(rightLengthOrAngle,scale);
  heightOrRightTargetX = rightContactX - (int)contactOffset;
  rightXOrTargetY = rightContactY - (int)(contactOffset >> 0x20);
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNode2);
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNode1);
  modelNode2 = modelNode2->childNodes[0];
  modelNode1 = modelNode1->childNodes[0];
  leftLegVector = FixedMath_VectorToAnglesAndLength3Regs
                     (leftContactZ - (modelNode2->worldTransform).translation.z,
                      leftYOrElevation - (modelNode2->worldTransform).translation.y,
                      leftXOrAngle - (modelNode2->worldTransform).translation.x);
  rightLengthOrAngle = leftLegVector.azimuthAngle - (modelNodeRuntime->modelPayload).worldRotationAngle2 & 0xffff;
  leftXOrAngle = leftLegVector.elevationAngle + 0x4000;
  if ((0x3fff < rightLengthOrAngle) && (rightLengthOrAngle < 0xc001)) {
    leftXOrAngle = -leftXOrAngle;
  }
  rightLegVector = FixedMath_VectorToAnglesAndLength3Regs
                     (rightContactZ - (modelNode1->worldTransform).translation.z,
                      rightXOrTargetY - (modelNode1->worldTransform).translation.y,
                      heightOrRightTargetX - (modelNode1->worldTransform).translation.x);
  rightLengthOrAngle = rightLegVector.azimuthAngle - (modelNodeRuntime->modelPayload).worldRotationAngle2 & 0xffff;
  leftYOrElevation = rightLegVector.elevationAngle + 0x4000;
  if ((0x3fff < rightLengthOrAngle) && (rightLengthOrAngle < 0xc001)) {
    leftYOrElevation = -leftYOrElevation;
  }
  modelNode2 = modelNode2->childNodes[0];
  leftYOrSideLength = FixedMath_LengthVec3
                     ((GraphicsFixedVec3 *)&(modelNode2->modelPayload).localTranslationXQ12);
  sideLength0Q12 =
       FixedMath_LengthVec3
                 ((GraphicsFixedVec3 *)
                  &(modelNode2->childNodes[0]->modelPayload).localTranslationXQ12);
  modelNode2 = modelNodeRuntime->childNodes[0]->childNodes[0];
  modelNode1 = modelNodeRuntime->childNodes[1]->childNodes[0];
  jointAngles = FixedGeometry_SolveTriangleJointAnglesRegs(sideLength0Q12,leftYOrSideLength,leftLegVector.lengthQ12);
  leftXOrAngle = jointAngles.jointAngle0 - leftXOrAngle;
  if (leftXOrAngle < 0) {
    (modelNode2->modelPayload).localRotationAngle0 = 0x8000;
    (modelNode2->modelPayload).localRotationAngle1 = leftXOrAngle + 0x4000;
  }
  else {
    (modelNode2->modelPayload).localRotationAngle0 = 0;
    (modelNode2->modelPayload).localRotationAngle1 = 0x4000 - leftXOrAngle;
  }
  modelNode2 = modelNode2->childNodes[0];
  (modelNode2->modelPayload).localRotationAngle1 = 0x4000 - jointAngles.jointAngle1;
  jointAngles = FixedGeometry_SolveTriangleJointAnglesRegs(sideLength0Q12,leftYOrSideLength,rightLegVector.lengthQ12);
  leftYOrElevation = jointAngles.jointAngle0 - leftYOrElevation;
  if (leftYOrElevation < 0) {
    (modelNode1->modelPayload).localRotationAngle0 = 0;
    (modelNode1->modelPayload).localRotationAngle1 = leftYOrElevation + 0x4000;
  }
  else {
    (modelNode1->modelPayload).localRotationAngle0 = 0x8000;
    (modelNode1->modelPayload).localRotationAngle1 = 0x4000 - leftYOrElevation;
  }
  modelNode1 = modelNode1->childNodes[0];
  (modelNode1->modelPayload).localRotationAngle1 = 0x4000 - jointAngles.jointAngle1;
  modelNode2 = modelNode2->childNodes[0];
  modelNode1 = modelNode1->childNodes[0];
  (modelNode2->modelPayload).localRotationAngle0 = 0;
  (modelNode2->modelPayload).localRotationAngle1 = 0x4000;
  (modelNode2->modelPayload).localRotationAngle2 = 0;
  (modelNode1->modelPayload).localRotationAngle0 = 0;
  (modelNode1->modelPayload).localRotationAngle1 = 0x4000;
  (modelNode1->modelPayload).localRotationAngle2 = 0;
  modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
  FixedTransform_BuildRotationBasis
            ((GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchB,
             (modelNode2->modelPayload).worldRotationAngle2,
             (modelNode2->modelPayload).worldRotationAngle1,
             (modelNode2->modelPayload).worldRotationAngle0);
  FixedTransform_InvertRigidQ28
            ((GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchA,
             (GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchB);
  leftBlendAngleEdx = leftBlendAngles.azimuthAngle;
  leftBlendAngleEcx = leftBlendAngles.elevationAngle;
  FixedTransform_BuildRotationBasis
            ((GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchB,
             (((leftHeading - leftPreviousHeading) * 0x10000 >> 0x10) * leftBlendQ12 >> 0xc) + leftHeadingBase & 0xffff,leftBlendAngleEcx,leftBlendAngleEdx
            );
  FixedTransform_Compose
            ((GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixComposedScratch,
             (GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchB,
             (GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchA);
  extractedAngles = FixedTransform_ExtractEulerAnglesRegs
                     ((GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixComposedScratch);
  (modelNode2->modelPayload).localRotationAngle0 = extractedAngles.ecxAngle;
  (modelNode2->modelPayload).localRotationAngle1 = extractedAngles.edxAngle;
  (modelNode2->modelPayload).localRotationAngle2 = extractedAngles.eaxAngle;
  FixedTransform_BuildRotationBasis
            ((GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchB,
             (modelNode1->modelPayload).worldRotationAngle2,
             (modelNode1->modelPayload).worldRotationAngle1,
             (modelNode1->modelPayload).worldRotationAngle0);
  FixedTransform_InvertRigidQ28
            ((GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchA,
             (GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchB);
  rightBlendAngleEdx = rightBlendAngles.azimuthAngle;
  rightBlendAngleEcx = rightBlendAngles.elevationAngle;
  FixedTransform_BuildRotationBasis
            ((GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchB,
             (((inverseBlendOrRightHeading - rightPreviousHeading) * 0x10000 >> 0x10) * rightBlendQ12 >> 0xc) + rightHeadingBase & 0xffff,rightBlendAngleEcx,
             rightBlendAngleEdx);
  FixedTransform_Compose
            ((GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixComposedScratch,
             (GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchB,
             (GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchA);
  extractedAngles = FixedTransform_ExtractEulerAnglesRegs
                     ((GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixComposedScratch);
  (modelNode1->modelPayload).localRotationAngle0 = extractedAngles.ecxAngle;
  (modelNode1->modelPayload).localRotationAngle1 = extractedAngles.edxAngle;
  (modelNode1->modelPayload).localRotationAngle2 = extractedAngles.eaxAngle;
  modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
  return;
}


/* Address: 0x00522550.
   Ownership: gameplay/army/movement.
   Purpose: Initializes the left terrain-contact sample and its interpolation step from the current articulated
   geometry. Typed parameters: p2 headingAngle16→AngleTurn16Stored32_V304. Calling convention, exact
   VariableStorage serialization, function body bytes, control flow, globals, locals, and executable data remain
   unchanged.
   Cross-module calls: FixedMath_SinCosScaled [core/math/fixed], FieldGrid_InterpolateTerrainHeightAndNormal
   [world/terrain/grid], FixedMath_Length3 [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyArticulatedRuntime_InitializeLeftTerrainContact
          (AngleTurn16Stored32 headingAngle16,ArmyArticulatedRuntimeSlotView *armyRuntime,
          WorldRuntimeContext *worldRuntime)

{
  Q12 *contactStateFlags;
  int segmentLength;
  void *definitionAsset;
  int stepBase;
  uint32_t contactLength;
  uint32_t angle;
  FixedSinCosEdxEax8 offsetSinCos;
  HeightNormalSampleResult terrainSample;
  
  armyRuntime->ownerValue64 = headingAngle16;
  (armyRuntime->linkedChildOverloadedState).leftHeadingCommandOrSpawnValue.signedValue =
       headingAngle16;
  angle = headingAngle16 + 0x4000U & 0xffff;
  offsetSinCos = FixedMath_SinCosScaled(angle,(armyRuntime->articulatedContact).lateralOffsetQ12);
  armyRuntime->runtimeState90 = (int)offsetSinCos * 2 + armyRuntime->runtimeState94;
  armyRuntime->runtimeState98 =
       (int)(offsetSinCos >> 0x20) * 2 + armyRuntime->articulatedCoordinateOrState9C;
  offsetSinCos = FixedMath_SinCosScaled
                    (angle,(armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.
                           contactRadiusQ12);
  if (worldRuntime->fieldGrid != (FieldGridAsset *)0x0) {
    terrainSample = FieldGrid_InterpolateTerrainHeightAndNormal
                      ((int)(offsetSinCos >> 0x20) + armyRuntime->runtimeState98,
                       (int)offsetSinCos + armyRuntime->runtimeState90,worldRuntime->fieldGrid);
    if (!terrainSample.failed) {
      armyRuntime->articulatedHeightOrStateA0 = terrainSample.heightQ12;
      armyRuntime->fallbackWorldYQ12 = terrainSample.packedNormalAngles;
      definitionAsset = armyRuntime->definitionOrAsset;
      contactLength = FixedMath_Length3(armyRuntime->articulatedHeightOrStateA0 -
                                armyRuntime->definitionClassValue88,
                                armyRuntime->runtimeState98 - armyRuntime->definitionClassValue80,
                                armyRuntime->runtimeState90 - armyRuntime->movementTarget0Q12);
      segmentLength = contactLength + *(int *)((int)definitionAsset + 0xc4) * 4;
      stepBase = *(int *)((int)definitionAsset + 0xc0);
      (armyRuntime->articulatedContact).fallbackPosition1Q12 = 0x2000;
      if (segmentLength != 0) {
        (armyRuntime->articulatedContact).fallbackPosition1Q12 =
             (Q12)((int64_t)(uint64_t)(uint32_t)(stepBase << 0xd) / (int64_t)segmentLength);
        contactStateFlags = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
        *contactStateFlags = *contactStateFlags & 0xffffff7f;
      }
    }
  }
  return;
}


/* Address: 0x00522660.
   Ownership: gameplay/army/movement.
   Purpose: Initializes the right terrain-contact sample and its interpolation step from the current articulated
   geometry. Typed parameters: p2 headingAngle16→AngleTurn16Stored32_V304. Calling convention, exact
   VariableStorage serialization, function body bytes, control flow, globals, locals, and executable data remain
   unchanged.
   Cross-module calls: FixedMath_SinCosScaled [core/math/fixed], FieldGrid_InterpolateTerrainHeightAndNormal
   [world/terrain/grid], FixedMath_Length3 [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyArticulatedRuntime_InitializeRightTerrainContact
          (AngleTurn16Stored32 headingAngle16,ArmyArticulatedRuntimeSlotView *armyRuntime,
          WorldRuntimeContext *worldRuntime)

{
  Q12 *contactStateFlags;
  int segmentLength;
  void *definitionAsset;
  int stepBase;
  uint32_t contactLength;
  uint32_t angle;
  FixedSinCosEdxEax8 offsetSinCos;
  HeightNormalSampleResult terrainSample;
  
  armyRuntime->ownerValue64 = headingAngle16;
  (armyRuntime->linkedChildOverloadedState).rightHeadingCommandOrSpawnValue.signedValue =
       headingAngle16;
  angle = headingAngle16 - 0x4000U & 0xffff;
  offsetSinCos = FixedMath_SinCosScaled(angle,(armyRuntime->articulatedContact).lateralOffsetQ12);
  armyRuntime->runtimeState94 = (int)offsetSinCos * 2 + armyRuntime->runtimeState90;
  armyRuntime->articulatedCoordinateOrState9C =
       (int)(offsetSinCos >> 0x20) * 2 + armyRuntime->runtimeState98;
  offsetSinCos = FixedMath_SinCosScaled
                    (angle,(armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.
                           contactRadiusQ12);
  if (worldRuntime->fieldGrid != (FieldGridAsset *)0x0) {
    terrainSample = FieldGrid_InterpolateTerrainHeightAndNormal
                      ((int)(offsetSinCos >> 0x20) + armyRuntime->articulatedCoordinateOrState9C,
                       (int)offsetSinCos + armyRuntime->runtimeState94,worldRuntime->fieldGrid);
    if (!terrainSample.failed) {
      armyRuntime->runtimeStateA4 = terrainSample.heightQ12;
      armyRuntime->fallbackWorldXQ12 = terrainSample.packedNormalAngles;
      definitionAsset = armyRuntime->definitionOrAsset;
      contactLength = FixedMath_Length3(armyRuntime->runtimeStateA4 - armyRuntime->runtimeState8C,
                                armyRuntime->articulatedCoordinateOrState9C -
                                armyRuntime->definitionClassValue84,
                                armyRuntime->runtimeState94 - armyRuntime->movementTarget1Q12);
      segmentLength = contactLength + *(int *)((int)definitionAsset + 0xc4) * 4;
      stepBase = *(int *)((int)definitionAsset + 0xc0);
      (armyRuntime->articulatedContact).fallbackPosition1Q12 = 0x2000;
      if (segmentLength != 0) {
        (armyRuntime->articulatedContact).fallbackPosition1Q12 =
             (Q12)((int64_t)(uint64_t)(uint32_t)(stepBase << 0xd) / (int64_t)segmentLength);
        contactStateFlags = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
        *contactStateFlags = *contactStateFlags & 0xffffff7f;
      }
    }
  }
  return;
}


/* Address: 0x00522770.
   Ownership: gameplay/army/movement.
   Purpose: Selects the left or right articulated contact from the signed steering angle, samples its terrain
   point, and rebuilds the corresponding interpolation state. Three stack arguments are authoritative from RET
   0x0C; EDX:EAX preserves the selected world X/Y contact pair.
   Cross-module calls: FixedMath_SinCosScaled [core/math/fixed], FieldGrid_InterpolateTerrainHeightAndNormal
   [world/terrain/grid], FixedMath_Length3 [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyArticulatedRuntime_UpdateSelectedTerrainContact
          (AngleTurn32 steeringAngle16,ArmyArticulatedRuntimeSlotView *armyRuntime,
          WorldRuntimeContext *worldRuntime)

{
  Q12 *contactStateFlags;
  void *definitionAsset;
  uint32_t contactYOrHeight;
  int64_t scaledOffsetProduct;
  int contactXOrSegment;
  uint32_t contactCoordOrLength;
  int signedSteeringAngle;
  uint32_t contactHeading;
  uint32_t scaledLateralOffset;
  int contactYOrStep;
  FixedSinCosEdxEax8 offsetSinCos;
  HeightNormalSampleResult terrainSample;
  FieldGridAsset *fieldGrid1;
  ModelRuntimeNode *modelNode1;
  
  definitionAsset = armyRuntime->definitionOrAsset;
  if (steeringAngle16 < 0x8001) {
    if (*(uint32_t *)((int)definitionAsset + 200) < steeringAngle16) {
      steeringAngle16 = *(AngleTurn32 *)((int)definitionAsset + 200);
    }
    modelNode1 = armyRuntime->modelNodeRuntime;
    contactHeading = armyRuntime->classState60 + 0x8000 & 0xffff;
    scaledOffsetProduct = (int64_t)(armyRuntime->articulatedContact).lateralOffsetQ12 * 0x1800;
    scaledLateralOffset = (int)((uint64_t)scaledOffsetProduct >> 0x20) << 0x14 | (uint32_t)scaledOffsetProduct >> 0xc;
    offsetSinCos = FixedMath_SinCosScaled(contactHeading,scaledLateralOffset);
    contactXOrSegment = (int)offsetSinCos + (modelNode1->worldTransform).translation.x;
    contactYOrStep = (int)(offsetSinCos >> 0x20) + (modelNode1->worldTransform).translation.y;
    contactHeading = (contactHeading + steeringAngle16) - 0x8000 & 0xffff;
    (armyRuntime->linkedChildOverloadedState).leftHeadingCommandOrSpawnValue.headingOrTurnValue =
         contactHeading;
    offsetSinCos = FixedMath_SinCosScaled(contactHeading,scaledLateralOffset);
    contactXOrSegment = contactXOrSegment + (int)offsetSinCos;
    contactYOrStep = contactYOrStep + (int)(offsetSinCos >> 0x20);
    contactHeading = contactHeading + 0x4000 & 0xffff;
    offsetSinCos = FixedMath_SinCosScaled(contactHeading,(armyRuntime->articulatedContact).lateralOffsetQ12);
    armyRuntime->runtimeState90 = (int)offsetSinCos + contactXOrSegment;
    armyRuntime->runtimeState98 = (int)(offsetSinCos >> 0x20) + contactYOrStep;
    offsetSinCos = FixedMath_SinCosScaled
                       (contactHeading,(armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.
                              contactRadiusQ12);
    fieldGrid1 = worldRuntime->fieldGrid;
    armyRuntime->ownerValue64 = ((int)steeringAngle16 >> 1) + armyRuntime->classState60 & 0xffff;
    if (fieldGrid1 != (FieldGridAsset *)0x0) {
      terrainSample = FieldGrid_InterpolateTerrainHeightAndNormal
                         ((int)(offsetSinCos >> 0x20) + armyRuntime->runtimeState98,
                          (int)offsetSinCos + armyRuntime->runtimeState90,fieldGrid1);
      if (!terrainSample.failed) {
        armyRuntime->articulatedHeightOrStateA0 = terrainSample.heightQ12;
        armyRuntime->fallbackWorldYQ12 = terrainSample.packedNormalAngles;
      }
    }
    contactStateFlags = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
    *contactStateFlags = *contactStateFlags & 0xffff;
    contactCoordOrLength = armyRuntime->runtimeState90;
    contactYOrHeight = armyRuntime->runtimeState98;
    contactXOrSegment = armyRuntime->articulatedHeightOrStateA0;
    contactStateFlags = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
    *contactStateFlags = *contactStateFlags | steeringAngle16 << 0xf;
    definitionAsset = armyRuntime->definitionOrAsset;
    contactCoordOrLength = FixedMath_Length3(contactXOrSegment - armyRuntime->definitionClassValue88,
                              contactYOrHeight - armyRuntime->definitionClassValue80,
                              contactCoordOrLength - armyRuntime->movementTarget0Q12);
    contactXOrSegment = contactCoordOrLength + *(int *)((int)definitionAsset + 0xc4) * 4;
    contactYOrStep = *(int *)((int)definitionAsset + 0xc0);
    (armyRuntime->articulatedContact).fallbackPosition1Q12 = 0x2000;
    if (contactXOrSegment != 0) {
      (armyRuntime->articulatedContact).fallbackPosition1Q12 =
           (Q12)((int64_t)(uint64_t)(uint32_t)(contactYOrStep << 0xd) / (int64_t)contactXOrSegment);
      return;
    }
  }
  else {
    contactHeading = 0x10000 - *(int *)((int)definitionAsset + 200);
    if (steeringAngle16 < contactHeading) {
      steeringAngle16 = contactHeading;
    }
    modelNode1 = armyRuntime->modelNodeRuntime;
    signedSteeringAngle = steeringAngle16 - 0x10000;
    contactHeading = armyRuntime->classState60 + 0x8000 & 0xffff;
    scaledOffsetProduct = (int64_t)(armyRuntime->articulatedContact).lateralOffsetQ12 * 0x1800;
    scaledLateralOffset = (int)((uint64_t)scaledOffsetProduct >> 0x20) << 0x14 | (uint32_t)scaledOffsetProduct >> 0xc;
    offsetSinCos = FixedMath_SinCosScaled(contactHeading,scaledLateralOffset);
    contactXOrSegment = (int)offsetSinCos + (modelNode1->worldTransform).translation.x;
    contactYOrStep = (int)(offsetSinCos >> 0x20) + (modelNode1->worldTransform).translation.y;
    contactHeading = (contactHeading + signedSteeringAngle) - 0x8000 & 0xffff;
    (armyRuntime->linkedChildOverloadedState).rightHeadingCommandOrSpawnValue.headingOrTurnValue =
         contactHeading;
    offsetSinCos = FixedMath_SinCosScaled(contactHeading,scaledLateralOffset);
    contactXOrSegment = contactXOrSegment + (int)offsetSinCos;
    contactYOrStep = contactYOrStep + (int)(offsetSinCos >> 0x20);
    contactHeading = contactHeading - 0x4000 & 0xffff;
    offsetSinCos = FixedMath_SinCosScaled(contactHeading,(armyRuntime->articulatedContact).lateralOffsetQ12);
    armyRuntime->runtimeState94 = (int)offsetSinCos + contactXOrSegment;
    armyRuntime->articulatedCoordinateOrState9C = (int)(offsetSinCos >> 0x20) + contactYOrStep;
    offsetSinCos = FixedMath_SinCosScaled
                       (contactHeading,(armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.
                              contactRadiusQ12);
    fieldGrid1 = worldRuntime->fieldGrid;
    armyRuntime->ownerValue64 = (signedSteeringAngle >> 1) + armyRuntime->classState60 & 0xffff;
    if (fieldGrid1 != (FieldGridAsset *)0x0) {
      terrainSample = FieldGrid_InterpolateTerrainHeightAndNormal
                         ((int)(offsetSinCos >> 0x20) + armyRuntime->articulatedCoordinateOrState9C,
                          (int)offsetSinCos + armyRuntime->runtimeState94,fieldGrid1);
      if (!terrainSample.failed) {
        armyRuntime->runtimeStateA4 = terrainSample.heightQ12;
        armyRuntime->fallbackWorldXQ12 = terrainSample.packedNormalAngles;
      }
    }
    contactStateFlags = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
    *contactStateFlags = *contactStateFlags & 0xffff;
    contactCoordOrLength = armyRuntime->runtimeState94;
    contactXOrSegment = armyRuntime->articulatedCoordinateOrState9C;
    contactYOrHeight = armyRuntime->runtimeStateA4;
    contactStateFlags = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
    *contactStateFlags = *contactStateFlags | signedSteeringAngle * 0x8000;
    definitionAsset = armyRuntime->definitionOrAsset;
    contactCoordOrLength = FixedMath_Length3(contactYOrHeight - armyRuntime->runtimeState8C,
                              contactXOrSegment - armyRuntime->definitionClassValue84,
                              contactCoordOrLength - armyRuntime->movementTarget1Q12);
    contactXOrSegment = contactCoordOrLength + *(int *)((int)definitionAsset + 0xc4) * 4;
    contactYOrStep = *(int *)((int)definitionAsset + 0xc0);
    (armyRuntime->articulatedContact).fallbackPosition1Q12 = 0x2000;
    if (contactXOrSegment != 0) {
      (armyRuntime->articulatedContact).fallbackPosition1Q12 =
           (Q12)((int64_t)(uint64_t)(uint32_t)(contactYOrStep << 0xd) / (int64_t)contactXOrSegment);
    }
  }
  return;
}


/* Address: 0x00523340.
   Ownership: gameplay/army/movement.
   Purpose: Evaluates the current target-following condition and starts, clamps, or resets movement according to
   the selected army runtime movement flags. Typed parameters: p2 targetWorldZQ12→Q12, p3 targetWorldYQ12→Q12, p4
   targetWorldXQ12→Q12. Nearby but non-identical semantic domains were explicitly deferred. Calling convention,
   parameter storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: ArmyRuntime_StartMoveCommandWithFallbackWaypoints, ArmyRuntime_StartClampedMoveCommand,
   ArmyRuntime_ResetMovementStatePreserveQueuedTarget, ArmyRuntime_ResetMovementStateFromCurrentPosition.
   Cross-module calls: ArmyWeaponRuntime_TestTargetLineOfFire [gameplay/army/combat].
*/
bool __thandor_cf_preserve_eax_ecx_edx
ArmyRuntimeCommand_UpdateTargetFollowingState
          (Q12 targetWorldZQ12,Q12 targetWorldYQ12,Q12 targetWorldXQ12,
          WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  GameEntityRuntime *movementRuntime;
  GameEntityRuntime *definitionRecord;
  bool lineOfFireClear;
  
  movementRuntime = armyRuntime->linkedEntityRuntime;
  lineOfFireClear = ArmyWeaponRuntime_TestTargetLineOfFire
                    (targetWorldZQ12,targetWorldYQ12,targetWorldXQ12,worldRuntime,armyRuntime);
  definitionRecord = (movementRuntime->common).ownership.definitionOrClassRecord;
  if (lineOfFireClear) {
    if (((armyRuntime == (definitionRecord->classPayload).impactOwnerLinks.primaryImpactArmyRuntime) ||
        ((definitionRecord->classPayload).impactOwnerLinks.primaryImpactArmyRuntime == (ArmyRuntimeSlot *)0x0)
        ) && (((movementRuntime->common).commandFlags & 0x20) == 0)) {
      if (((movementRuntime->common).commandTarget.targetFlags & 8) == 0) {
        ArmyRuntime_StartMoveCommandWithFallbackWaypoints
                  (targetWorldYQ12,targetWorldXQ12,(ArmyMovementRuntime *)movementRuntime);
      }
      else {
        ArmyRuntime_StartClampedMoveCommand
                  (targetWorldYQ12,targetWorldXQ12,(ArmyMovementRuntime *)movementRuntime);
      }
    }
    return true;
  }
  if (((movementRuntime->common).commandFlags & 0x20) != 0) {
    if (((movementRuntime->common).commandTarget.targetFlags & 8) == 0) {
      ArmyRuntime_ResetMovementStatePreserveQueuedTarget((ArmyMovementRuntime *)movementRuntime);
    }
    else {
      ArmyRuntime_ResetMovementStateFromCurrentPosition((ArmyMovementRuntime *)movementRuntime);
    }
  }
  return false;
}


/* Address: 0x0051CA60.
   While a non-direct move is active or the movement is locked, appends the target to the waypoint queue
   (starting a new queue if none is in use; when full the last entry is overwritten); otherwise starts it
   at once with ArmyRuntime_QueueOrStartMoveCommandVariantA.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_QueueWaypointOrStartMoveVariantA
          (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime)

{
  uint32_t waypointCount;

  if (((movementRuntime->movementStateFlags & ARMY_MOVEMENT_DIRECT) == 0) &&
     ((movementRuntime->movementStateFlags & (ARMY_MOVEMENT_LOCKED | ARMY_MOVEMENT_ACTIVE)) != 0)) {
    if ((movementRuntime->movementStateFlags & ARMY_MOVEMENT_WAYPOINTS_QUEUED) == 0) {
      movementRuntime->queuedWaypointCount = 0;
      movementRuntime->movementStateFlags = movementRuntime->movementStateFlags | ARMY_MOVEMENT_WAYPOINTS_QUEUED;
    }
    waypointCount = movementRuntime->queuedWaypointCount;
    movementRuntime->movementStateFlags = movementRuntime->movementStateFlags | 0x40;
    if (waypointCount < ARMY_MOVEMENT_WAYPOINT_CAPACITY) {
      waypointCount = waypointCount + 1;
    }
    movementRuntime->commandModeFlags = movementRuntime->commandModeFlags & 0xfffffbef;
    movementRuntime->queuedWaypoints[waypointCount - 1].worldXQ12 = targetWorldX;
    movementRuntime->queuedWaypoints[waypointCount - 1].worldYQ12 = targetWorldY;
    movementRuntime->queuedWaypointCount = waypointCount;
  }
  else {
    ArmyRuntime_QueueOrStartMoveCommandVariantA(targetWorldY,targetWorldX,movementRuntime);
  }
  return;
}


/* Address: 0x0051CB90.
   Target-following move (ArmyRuntimeCommand_UpdateTargetFollowingState): unless the movement is locked,
   routed or still waiting for its retry countdown, routes to the target. If a move was active, its
   fallback position is first saved into the waypoint queue (apparently so the army resumes it afterwards).
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_StartMoveCommandWithFallbackWaypoints
          (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime)

{
  GraphicsWorldCoordinateQ12 currentWorldX;
  GraphicsWorldCoordinateQ12 currentWorldY;
  int remainingCount;
  WorldRuntimeContext *worldRuntime;
  Q12 *fallbackCoordinateRead;
  Q12 *waypointCoordinateWrite;
  PathingDestinationResult resolvedDestination;

  if (((movementRuntime->movementStateFlags & (ARMY_MOVEMENT_ROUTED | ARMY_MOVEMENT_LOCKED)) == 0) &&
      (movementRuntime->retryCountdown == 0)) {
    worldRuntime = &g_InGameRuntimeRoot->worldRuntime0A30;
    movementRuntime->movementStateFlags =
         movementRuntime->movementStateFlags &
         ~(ARMY_MOVEMENT_MIRROR_TARGET | ARMY_MOVEMENT_DIRECT | 0x10 | ARMY_MOVEMENT_WAYPOINTS_QUEUED);
    if ((movementRuntime->movementStateFlags & ARMY_MOVEMENT_ACTIVE) != 0) {
      /* Forward dword copy of 16 dwords from fallbackPosition (+0xB8) to queuedWaypoints (+0xC0), exactly
         like the original REP MOVSD. The ranges overlap by 8 bytes, so this does not shift the queue: it
         fills all 8 waypoints with fallbackPosition. */
      remainingCount = 0x10;
      fallbackCoordinateRead = &(movementRuntime->fallbackPosition).worldXQ12;
      waypointCoordinateWrite = &movementRuntime->queuedWaypoints[0].worldXQ12;
      for (; remainingCount != 0; remainingCount--) {
        *waypointCoordinateWrite = *fallbackCoordinateRead;
        fallbackCoordinateRead++;
        waypointCoordinateWrite++;
      }
      movementRuntime->movementStateFlags = movementRuntime->movementStateFlags | ARMY_MOVEMENT_WAYPOINTS_QUEUED;
      if (movementRuntime->queuedWaypointCount < ARMY_MOVEMENT_WAYPOINT_CAPACITY) {
        movementRuntime->queuedWaypointCount = movementRuntime->queuedWaypointCount + 1;
      }
    }
    movementRuntime->movementStateFlags =
         movementRuntime->movementStateFlags | (0x40 | ARMY_MOVEMENT_TARGET_FOLLOWING | ARMY_MOVEMENT_ACTIVE);
    resolvedDestination = EntityPathing_ResolveDestinationAndRebuildRoutes
                      (targetWorldY,targetWorldX,movementRuntime->entityRuntime,worldRuntime);
    (movementRuntime->fallbackPosition).worldXQ12 = resolvedDestination.fallbackWorldXQ12;
    (movementRuntime->fallbackPosition).worldYQ12 = resolvedDestination.fallbackWorldYQ12;
    movementRuntime->movementTargetWorldXQ12 = resolvedDestination.fallbackWorldXQ12;
    movementRuntime->movementTargetWorldYQ12 = resolvedDestination.fallbackWorldYQ12;
    movementRuntime->movementWorldXQ12 = resolvedDestination.primaryWorldXQ12;
    movementRuntime->movementWorldYQ12 = resolvedDestination.primaryWorldYQ12;
    currentWorldX = (movementRuntime->modelNodeRuntime->worldTransform).translation.x;
    currentWorldY = (movementRuntime->modelNodeRuntime->worldTransform).translation.y;
    movementRuntime->retryCountdown = ARMY_MOVEMENT_RETRY_TICKS;
    movementRuntime->lastCheckedWorldXQ12 = currentWorldX;
    movementRuntime->lastCheckedWorldYQ12 = currentWorldY;
  }
  return;
}


/* Address: 0x0051CE30.
   Ends a target-following move: the move stays active only if waypoints are queued, and unless a routed
   move is in progress all targets are set to the current model position so the army stops where it is.
   Bits 0x10-0x80 are cleared.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_ResetMovementStatePreserveQueuedTarget(ArmyMovementRuntime *movementRuntime)

{
  GraphicsWorldCoordinateQ12 currentWorldXQ12;
  GraphicsWorldCoordinateQ12 currentWorldYQ12;

  if ((movementRuntime->movementStateFlags & ARMY_MOVEMENT_WAYPOINTS_QUEUED) == 0) {
    movementRuntime->movementStateFlags = movementRuntime->movementStateFlags & ~ARMY_MOVEMENT_ACTIVE;
  }
  if ((movementRuntime->movementStateFlags & ARMY_MOVEMENT_ROUTED) == 0) {
    currentWorldXQ12 = (movementRuntime->modelNodeRuntime->worldTransform).translation.x;
    currentWorldYQ12 = (movementRuntime->modelNodeRuntime->worldTransform).translation.y;
    (movementRuntime->fallbackPosition).worldXQ12 = currentWorldXQ12;
    (movementRuntime->fallbackPosition).worldYQ12 = currentWorldYQ12;
    movementRuntime->movementTargetWorldXQ12 = currentWorldXQ12;
    movementRuntime->movementTargetWorldYQ12 = currentWorldYQ12;
    movementRuntime->movementWorldXQ12 = currentWorldXQ12;
    movementRuntime->movementWorldYQ12 = currentWorldYQ12;
  }
  movementRuntime->movementStateFlags =
       movementRuntime->movementStateFlags &
       ~(ARMY_MOVEMENT_DIRECT | 0x40 | ARMY_MOVEMENT_TARGET_FOLLOWING | 0x10);
  return;
}


/* Address: 0x0051CE90.
   Ends a clamped target-following move: without queued waypoints the move just stops being active;
   with queued waypoints the current and fallback positions are set to the model position (the final
   target is kept). Bits 0x10-0x80 are cleared.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_ResetMovementStateFromCurrentPosition(ArmyMovementRuntime *movementRuntime)

{
  GraphicsWorldCoordinateQ12 currentWorldXQ12;
  GraphicsWorldCoordinateQ12 currentWorldYQ12;

  if ((movementRuntime->movementStateFlags & ARMY_MOVEMENT_WAYPOINTS_QUEUED) == 0) {
    movementRuntime->movementStateFlags = movementRuntime->movementStateFlags & ~ARMY_MOVEMENT_ACTIVE;
  }
  else {
    currentWorldXQ12 = (movementRuntime->modelNodeRuntime->worldTransform).translation.x;
    currentWorldYQ12 = (movementRuntime->modelNodeRuntime->worldTransform).translation.y;
    (movementRuntime->fallbackPosition).worldXQ12 = currentWorldXQ12;
    (movementRuntime->fallbackPosition).worldYQ12 = currentWorldYQ12;
    movementRuntime->movementWorldXQ12 = currentWorldXQ12;
    movementRuntime->movementWorldYQ12 = currentWorldYQ12;
  }
  movementRuntime->movementStateFlags =
       movementRuntime->movementStateFlags &
       ~(ARMY_MOVEMENT_DIRECT | 0x40 | ARMY_MOVEMENT_TARGET_FOLLOWING | 0x10);
  return;
}


/* Address: 0x0051CEE0.
   Per-tick movement step; returns the position to steer to (EDX:EAX) and arrival in CF. While a move is
   active the stored movement position is returned. When bit 0x10 is set, the route end is checked: once
   reached, the next queued waypoint is started (and the step repeated); otherwise the route is rebuilt
   when the model moved or the retry countdown ran out. Near the final target the move ends (arrived);
   farther away a direct move to it is started.
*/
MovementStepResult __thandor_eax_edx_cf_preserve_ecx
ArmyRuntime_UpdateMovementAndWaypoints
          (WorldRuntimeContext *worldRuntime,ArmyMovementRuntime *movementRuntime)

{
  ArmyWaypointCount *waypointCount;
  uint32_t exceededDistance;
  Q12 movementWorldX;
  Q12 movementWorldY;
  Q12 currentWorldX;
  Q12 currentWorldY;
  int offsetXOrCount;
  uint32_t distanceX;
  int offsetY;
  uint32_t distanceY;
  Q12 *queuedCoordinateRead;
  Q12 *queuedCoordinateWrite;
  bool belowThreshold;
  MovementStepResult storedPosition;
  MovementStepResult resolvedPosition;
  MovementStepResult arrivedPosition;
  MovementStepResult directMoveResult;
  PathingDestinationResult resolvedDestination;
  Q12 queuedWorldYQ12;
  Q12 queuedWorldXQ12;
  ModelRuntimeNode *modelNode;

  modelNode = movementRuntime->modelNodeRuntime;
  if ((movementRuntime->movementStateFlags & 0x10) == 0) {
    if ((movementRuntime->movementStateFlags & ARMY_MOVEMENT_ACTIVE) != 0) {
      movementWorldX = movementRuntime->movementWorldXQ12;
      movementWorldY = movementRuntime->movementWorldYQ12;
      storedPosition.worldYQ12 = movementWorldY;
      storedPosition.worldXQ12 = movementWorldX;
      storedPosition.arrived = false;
      return storedPosition;
    }
  }
  else {
    movementRuntime->movementStateFlags = movementRuntime->movementStateFlags & ~0x10;
    offsetXOrCount = (movementRuntime->fallbackPosition).worldXQ12 -
            (modelNode->worldTransform).translation.x;
    offsetY = (movementRuntime->fallbackPosition).worldYQ12 -
            (modelNode->worldTransform).translation.y;
    if ((((offsetXOrCount < ARMY_MOVEMENT_ROUTE_END_RADIUS_Q12) && (offsetY < ARMY_MOVEMENT_ROUTE_END_RADIUS_Q12)) &&
         (-ARMY_MOVEMENT_ROUTE_END_RADIUS_Q12 < offsetXOrCount)) && (-ARMY_MOVEMENT_ROUTE_END_RADIUS_Q12 < offsetY)) {
      if ((movementRuntime->movementStateFlags & ARMY_MOVEMENT_WAYPOINTS_QUEUED) != 0) {
        /* pop queuedWaypoints[0] and move the other seven entries down */
        queuedWorldXQ12 = movementRuntime->queuedWaypoints[0].worldXQ12;
        queuedWorldYQ12 = movementRuntime->queuedWaypoints[0].worldYQ12;
        waypointCount = &movementRuntime->queuedWaypointCount;
        *waypointCount = *waypointCount - 1;
        if (*waypointCount == 0) {
          movementRuntime->movementStateFlags = movementRuntime->movementStateFlags & ~ARMY_MOVEMENT_WAYPOINTS_QUEUED;
        }
        else {
          offsetXOrCount = 0xe;
          queuedCoordinateRead = &movementRuntime->queuedWaypoints[1].worldXQ12;
          queuedCoordinateWrite = &movementRuntime->queuedWaypoints[0].worldXQ12;
          for (; offsetXOrCount != 0; offsetXOrCount--) {
            *queuedCoordinateWrite = *queuedCoordinateRead;
            queuedCoordinateRead++;
            queuedCoordinateWrite++;
          }
        }
        ArmyRuntime_QueueOrStartMoveCommandVariantB(queuedWorldYQ12,queuedWorldXQ12,movementRuntime)
        ;
        resolvedPosition = ArmyRuntime_UpdateMovementAndWaypoints(worldRuntime,movementRuntime);
        return resolvedPosition;
      }
    }
    else {
      offsetXOrCount = (modelNode->worldTransform).translation.x;
      offsetY = (modelNode->worldTransform).translation.y;
      if (((movementRuntime->retryCountdown != 0) &&
          (offsetXOrCount == movementRuntime->lastCheckedWorldXQ12)) &&
         (offsetY == movementRuntime->lastCheckedWorldYQ12)) {
        /* Still waiting at the same spot: keep the stored movement position. */
        storedPosition.worldYQ12 = movementRuntime->movementWorldYQ12;
        storedPosition.worldXQ12 = movementRuntime->movementWorldXQ12;
        storedPosition.arrived = false;
        return storedPosition;
      }
      movementRuntime->retryCountdown = ARMY_MOVEMENT_RETRY_TICKS;
      movementRuntime->lastCheckedWorldXQ12 = offsetXOrCount;
      movementRuntime->lastCheckedWorldYQ12 = offsetY;
      resolvedDestination = EntityPathing_ResolveDestinationAndRebuildRoutes
                         ((movementRuntime->fallbackPosition).worldYQ12,
                          (movementRuntime->fallbackPosition).worldXQ12,
                          movementRuntime->entityRuntime,worldRuntime);
      if (!resolvedDestination.failed) {
        movementRuntime->movementWorldXQ12 = resolvedDestination.primaryWorldXQ12;
        movementRuntime->movementWorldYQ12 = resolvedDestination.primaryWorldYQ12;
        (movementRuntime->fallbackPosition).worldXQ12 = resolvedDestination.fallbackWorldXQ12;
        (movementRuntime->fallbackPosition).worldYQ12 = resolvedDestination.fallbackWorldYQ12;
        resolvedPosition.arrived = false;
        resolvedPosition.worldXQ12 = (int)THANDOR_PART(uint64_t, resolvedDestination, 0);
        resolvedPosition.worldYQ12 = (int)(THANDOR_PART(uint64_t, resolvedDestination, 0) >> 0x20);
        return resolvedPosition;
      }
    }
  }
  distanceX = (modelNode->worldTransform).translation.x - movementRuntime->movementTargetWorldXQ12;
  if ((int)distanceX < 0) {
    distanceX = -distanceX;
  }
  distanceY = (modelNode->worldTransform).translation.y - movementRuntime->movementTargetWorldYQ12;
  if ((int)distanceY < 0) {
    distanceY = -distanceY;
  }
  exceededDistance = distanceX;
  if ((distanceX < ARMY_MOVEMENT_TARGET_RADIUS_Q12 + 1) &&
      (exceededDistance = distanceY, distanceY < ARMY_MOVEMENT_TARGET_RADIUS_Q12 + 1)) {
    movementRuntime->movementStateFlags =
         movementRuntime->movementStateFlags &
         ~(ARMY_MOVEMENT_DIRECT | 0x40 | ARMY_MOVEMENT_TARGET_FOLLOWING | 0x10 | ARMY_MOVEMENT_WAYPOINTS_QUEUED |
           ARMY_MOVEMENT_ACTIVE);
    currentWorldX = (modelNode->worldTransform).translation.x;
    currentWorldY = (modelNode->worldTransform).translation.y;
    arrivedPosition.worldYQ12 = currentWorldY;
    arrivedPosition.worldXQ12 = currentWorldX;
    arrivedPosition.arrived = true;
    return arrivedPosition;
  }
  /* The original returns the distances in EAX/EDX and whatever CF ArmyRuntime_StartDirectMoveCommand
     leaves (clear when locked, else the pathing call's CF); belowThreshold is always false here since
     exceededDistance > ARMY_MOVEMENT_TARGET_RADIUS_Q12. */
  belowThreshold = exceededDistance < ARMY_MOVEMENT_TARGET_RADIUS_Q12;
  ArmyRuntime_StartDirectMoveCommand
            (movementRuntime->movementTargetWorldYQ12,movementRuntime->movementTargetWorldXQ12,
             movementRuntime);
  directMoveResult.worldYQ12 = distanceY;
  directMoveResult.worldXQ12 = distanceX;
  directMoveResult.arrived = belowThreshold;
  return directMoveResult;
}

