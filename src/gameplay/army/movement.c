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
   Runtime update of the two-legged articulated walker (runtimeUpdate slot 3 of
   g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes, called by model class from
   ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive). While a foot is moving it advances that
   step (walking speed rises in the first half of the step and falls in the second); when no step is running
   it picks the next one from the route: a walking step, a turn on the spot, or a closing step that puts the
   feet side by side again. Then it places the body between the feet (ArmyArticulatedRuntime_UpdateSuspensionHierarchy)
   and applies water damage, linked-army and moved/turned bookkeeping like the other ground classes.
   The step state bits are the ARMY_ARTICULATED_STEP_* flags; the foot positions are described at
   ArmyArticulatedRuntime_InitializeTerrainContactGeometry.
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
  int advanceOrDeltaX;
  uint32_t advanceOrHeading;
  uint32_t feetLineHeading;
  /* holds, in turn, the step progress increment, the steering angle (0..0xFFFF) and the new root X */
  ModelRuntimeNode *stepSteerOrWorldX;
  int deltaY;
  /* holds, in turn, the new walking speed, a foot heading and the new root Y */
  ArmyRuntimeCoordinateCommandOrHistoryValue4 speedHeadingOrWorldY;
  bool withinLinkRadius;
  MovementStepResult waypointResult;
  WorldPositionResult commandTargetPosition;
  FixedLengthAngleEaxEdx8 targetAngleLength;
  ArmyRuntimeSlot *linkedOrOwnerArmy;
  ModelRuntimeNode *rootNode;

  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  linkedOrOwnerArmy = modelRuntime->linkedArmyRuntime;
  rootNode = modelRuntime->rootModelNode;
  ownerMovementFlags = &modelRuntime->ownerArmyRuntime->movementStateFlags;
  *ownerMovementFlags = *ownerMovementFlags | ARMY_MOVEMENT_STATIONARY;
  /* Drop the linked runtime unless both have a clearance radius and this unit is still within it. */
  if ((linkedOrOwnerArmy != NULL) &&
     ((((((linkedOrOwnerArmy->modelRuntimeOrSavedOffset).modelRuntime)->classState).classStateDC == 0
       || (modelRuntime->modelDefinition->placementRadiusOrClearanceDC == 0)) ||
      (withinLinkRadius = ArmyCollision_TestPointWithinExpandedRuntimeRadius
                          (modelRuntime->modelDefinition->placementRadiusOrClearanceDC,
                           (rootNode->worldTransform).translation.y,
                           (rootNode->worldTransform).translation.x,linkedOrOwnerArmy), !withinLinkRadius)))) {
    modelRuntime->linkedArmyRuntime = NULL;
  }
  previousRotationAngle = (rootNode->modelPayload).worldRotationAngle2;
  previousWorldX = (rootNode->worldTransform).translation.x;
  previousWorldY = (rootNode->worldTransform).translation.y;
  movementDefinition = modelRuntime->modelDefinition;
  waterDelta = FieldGrid_InterpolateWaterDelta
                    ((rootNode->worldTransform).translation.y,
                     (rootNode->worldTransform).translation.x,worldRuntime->fieldGrid);
  if ((movementDefinition->waterDamageThreshold198 < waterDelta) &&
     (advanceOrDeltaX = waterDelta * movementDefinition->waterDamageMultiplier194 >> 7, -1 < advanceOrDeltaX)) {
    ArmyRuntime_ApplyDamageAndPropagateToParent(advanceOrDeltaX,(ArmyRuntimeSlot *)modelRuntime);
  }
  advanceOrDeltaX = (modelRuntime->movementControl).movementAdvancePerTickQ12;
  speedHeadingOrWorldY = THANDOR_BITCAST(Q12, ArmyRuntimeCoordinateCommandOrHistoryValue4, movementDefinition->movementAdvanceDeltaQ12PerTick18);
  if (((modelRuntime->articulatedContact).fallbackPosition0Q12 &
       (ARMY_ARTICULATED_STEP_LEFT | ARMY_ARTICULATED_STEP_RIGHT)) != 0) {
    /* a step is running: accelerate in its first half, decelerate in its second */
    if (((int)modelRuntime->runtimeStateA8 < 0x801) &&
       ((int)(modelRuntime->articulatedContact).terrainContactMode < 0x801)) {
      speedHeadingOrWorldY.signedValue = speedHeadingOrWorldY.signedValue + advanceOrDeltaX;
    }
    else {
      speedHeadingOrWorldY.signedValue = -(speedHeadingOrWorldY.signedValue - advanceOrDeltaX);
    }
    (modelRuntime->movementControl).movementAdvancePerTickQ12 = THANDOR_BITCAST(ArmyRuntimeCoordinateCommandOrHistoryValue4, Q12, speedHeadingOrWorldY);
    /* progress increment = previous speed * step rate (fallbackPosition1Q12) * ticks */
    stepSteerOrWorldX =
         (ModelRuntimeNode *)
         ((int)(advanceOrDeltaX * (modelRuntime->articulatedContact).fallbackPosition1Q12 *
               g_InGameSimulationStepTicks) >> 12);
    if (((modelRuntime->articulatedContact).fallbackPosition0Q12 & ARMY_ARTICULATED_STEP_RIGHT) == 0) {
      /* runtimeStateA8 += increment (the pointer arithmetic below is how the decompiler expressed it:
         reserved2C_33 lies at +0x38 of the node) */
      modelRuntime->runtimeStateA8 =
           (uint32_t)((stepSteerOrWorldX->modelPayload).reserved2C_33 +
                  (modelRuntime->runtimeStateA8 - 0x38));
      if (0xfff < modelRuntime->runtimeStateA8) {
        /* left step done: the left foot target becomes the left foot position */
        contactStateFlags = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
        *contactStateFlags = *contactStateFlags & ~ARMY_ARTICULATED_STEP_LEFT;
        modelRuntime->runtimeStateA8 = 0;
        modelRuntime->movementTarget0Q12 = modelRuntime->runtimeState90;
        modelRuntime->definitionClassValue80 = modelRuntime->runtimeState98;
        modelRuntime->definitionClassValue88 = modelRuntime->articulatedHeightOrStateA0;
        speedHeadingOrWorldY = (modelRuntime->linkedChildOverloadedState).leftHeadingCommandOrSpawnValue;
        advanceOrHeading = (modelRuntime->linkedChildSpawnParameters).parameter0;
        modelRuntime->classState60 = modelRuntime->ownerValue64;
        modelRuntime->ownerValue68 = modelRuntime->fallbackWorldYQ12;
        (modelRuntime->linkedChildOverloadedState).primaryCoordinateCommandOrHistory = speedHeadingOrWorldY;
        (modelRuntime->movementControl).movementAdvancePerTickQ12 = advanceOrHeading;
        stepSteerOrWorldX = modelRuntime->rootModelNode;
        ArmyArticulatedRuntime_UpdateContactChildAndEffects
                  (stepSteerOrWorldX->childNodes[0],worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
      }
    }
    else {
      /* terrainContactMode is the right step progress here, same increment as above */
      (modelRuntime->articulatedContact).terrainContactMode =
           (ArmyTerrainContactDispatchMode)
           ((stepSteerOrWorldX->modelPayload).reserved2C_33 +
           ((modelRuntime->articulatedContact).terrainContactMode - 0x38));
      if (0xfff < (modelRuntime->articulatedContact).terrainContactMode) {
        /* right step done: the right foot target becomes the right foot position */
        contactStateFlags = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
        *contactStateFlags = *contactStateFlags & ~ARMY_ARTICULATED_STEP_RIGHT;
        (modelRuntime->articulatedContact).terrainContactMode =
             ARMY_TERRAIN_CONTACT_ACQUIRE_OR_INITIALIZE_CONTACT_SLOT;
        modelRuntime->movementTarget1Q12 = modelRuntime->runtimeState94;
        modelRuntime->definitionClassValue84 = modelRuntime->articulatedCoordinateOrState9C;
        modelRuntime->runtimeState8C = modelRuntime->runtimeStateA4;
        speedHeadingOrWorldY = (modelRuntime->linkedChildOverloadedState).rightHeadingCommandOrSpawnValue;
        advanceOrHeading = (modelRuntime->linkedChildSpawnParameters).parameter0;
        modelRuntime->classState60 = modelRuntime->ownerValue64;
        modelRuntime->linkedArmyRuntimeOrSavedOffset =
             (ArmyRuntimeSlot *)modelRuntime->fallbackWorldXQ12;
        (modelRuntime->linkedChildOverloadedState).secondaryCoordinateCommandOrHistory = speedHeadingOrWorldY;
        (modelRuntime->movementControl).movementAdvancePerTickQ12 = advanceOrHeading;
        stepSteerOrWorldX = modelRuntime->rootModelNode;
        ArmyArticulatedRuntime_UpdateContactChildAndEffects
                  (stepSteerOrWorldX->childNodes[1],worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
      }
    }
    if (((modelRuntime->articulatedContact).fallbackPosition0Q12 &
         (ARMY_ARTICULATED_STEP_LEFT | ARMY_ARTICULATED_STEP_RIGHT)) != 0)
    goto ArmyArticulatedMovement_CommitPositionSuspensionAndTransforms;
    /* the step has just ended: the route point counts as reached when the walker faces the command target
       (within 0x800) or stands within 0x40 of the route point */
    rootNode = modelRuntime->rootModelNode;
    waypointResult = ArmyRuntime_UpdateMovementAndWaypoints
                       (worldRuntime,(ArmyMovementRuntime *)modelRuntime->ownerArmyRuntime);
    if (waypointResult.arrived) {
      commandTargetPosition = GameEntityRuntime_ResolveCommandTargetPosition
                         ((GameEntityRuntime *)modelRuntime->ownerArmyRuntime);
      speedHeadingOrWorldY.signedValue = commandTargetPosition.worldZQ12;
      if (!commandTargetPosition.unresolved) {
        advanceOrHeading = FixedMath_Atan2Angle16
                          (commandTargetPosition.worldYQ12 - (rootNode->worldTransform).translation.y,
                           commandTargetPosition.worldXQ12 - (rootNode->worldTransform).translation.x);
        stepSteerOrWorldX =
             (ModelRuntimeNode *)(advanceOrHeading - (rootNode->modelPayload).worldRotationAngle2 & 0xffff);
        if (((ModelRuntimeNode *)0x800 < stepSteerOrWorldX) &&
           (stepSteerOrWorldX < (ModelRuntimeNode *)0xf800))
        goto ArmyArticulatedMovement_CommitPositionSuspensionAndTransforms;
      }
    }
    else {
      speedHeadingOrWorldY.signedValue = waypointResult.worldYQ12 - (rootNode->worldTransform).translation.y;
      stepSteerOrWorldX =
           (ModelRuntimeNode *)
           FixedMath_Length2(speedHeadingOrWorldY.signedValue,
                             waypointResult.worldXQ12 - (rootNode->worldTransform).translation.x);
      if ((ModelRuntimeNode *)0x40 < stepSteerOrWorldX)
      goto ArmyArticulatedMovement_CommitPositionSuspensionAndTransforms;
    }
ArmyArticulatedMovement_ClearContactTransitionAndAdvanceWaypoint:
    linkedOrOwnerArmy = modelRuntime->ownerArmyRuntime;
    contactStateFlags = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
    *contactStateFlags = *contactStateFlags & ~(ARMY_ARTICULATED_STEP_WALK | ARMY_ARTICULATED_STEP_TURN);
    waypointResult = ArmyRuntime_UpdateMovementAndWaypoints
                       (worldRuntime,(ArmyMovementRuntime *)linkedOrOwnerArmy);
    speedHeadingOrWorldY.signedValue = waypointResult.worldYQ12;
    stepSteerOrWorldX = (ModelRuntimeNode *)waypointResult.worldXQ12;
    if (!waypointResult.arrived) {
      ownerMovementFlags = &linkedOrOwnerArmy->movementStateFlags;
      *ownerMovementFlags = *ownerMovementFlags | ARMY_MOVEMENT_ROUTE_POINT_REACHED;
    }
    goto ArmyArticulatedMovement_CommitPositionSuspensionAndTransforms;
  }
  /* no step running: choose the next one */
  if (((modelRuntime->articulatedContact).fallbackPosition0Q12 & ARMY_ARTICULATED_STEP_TURN) == 0) {
    if ((modelRuntime->runtimeFlags & ARMY_RUNTIME_FLAG_DESTROYED) == 0) {
      waypointResult = ArmyRuntime_UpdateMovementAndWaypoints
                         (worldRuntime,(ArmyMovementRuntime *)modelRuntime->ownerArmyRuntime);
      if (waypointResult.arrived) {
        /* no route: only turn towards the command target (length -1 = do not walk) */
        commandTargetPosition = GameEntityRuntime_ResolveCommandTargetPosition
                           ((GameEntityRuntime *)modelRuntime->ownerArmyRuntime);
        speedHeadingOrWorldY.signedValue = commandTargetPosition.worldZQ12;
        if (commandTargetPosition.unresolved) goto ArmyArticulatedMovement_SharedContinuation;
        targetAngleLength.angle =
             FixedMath_Atan2Angle16
                       (commandTargetPosition.worldYQ12 - (rootNode->worldTransform).translation.y,
                        commandTargetPosition.worldXQ12 - (rootNode->worldTransform).translation.x);
        targetAngleLength.length = 0xffffffff;
      }
      else {
        advanceOrDeltaX = waypointResult.worldXQ12 - (rootNode->worldTransform).translation.x;
        deltaY = waypointResult.worldYQ12 - (rootNode->worldTransform).translation.y;
        if ((advanceOrDeltaX == 0) && (deltaY == 0)) {
          targetAngleLength = THANDOR_BITCAST(uint64_t, FixedLengthAngleEaxEdx8, ((uint64_t)(rootNode->modelPayload).worldRotationAngle2 << 32));
        }
        else {
          targetAngleLength = FixedMath_Vector2AngleAndLengthRegs(deltaY,advanceOrDeltaX);
        }
        speedHeadingOrWorldY.signedValue = targetAngleLength.angle;
      }
      stepSteerOrWorldX =
           (ModelRuntimeNode *)
           (targetAngleLength.angle - (rootNode->modelPayload).worldRotationAngle2 & 0xffff);
      if (0 < (int)targetAngleLength.length) {
        /* walk on when the route point lies within +-0x2000 ahead (+-0x200 while already walking) */
        if (((modelRuntime->articulatedContact).fallbackPosition0Q12 & ARMY_ARTICULATED_STEP_WALK) == 0) {
          if ((stepSteerOrWorldX < (ModelRuntimeNode *)0x2001) ||
             ((ModelRuntimeNode *)0xdfff < stepSteerOrWorldX)) {
ArmyArticulatedMovement_UpdateSelectedTerrainContact:
            /* 0xffa0 also clears the stored turn angle in the upper 16 bits */
            if (((modelRuntime->articulatedContact).fallbackPosition0Q12 & ARMY_ARTICULATED_STEP_LEFT_LAST) == 0) {
              contactStateFlags = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
              *contactStateFlags = *contactStateFlags & 0xffa0;
              contactStateFlags = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
              *contactStateFlags = *contactStateFlags |
                   (ARMY_ARTICULATED_STEP_WALK | ARMY_ARTICULATED_STEP_LEFT_LAST | ARMY_ARTICULATED_STEP_LEFT);
              ArmyArticulatedRuntime_UpdateLeftTerrainContact
                        (targetAngleLength.angle,targetAngleLength.length,
                         (ArmyArticulatedRuntimeSlotView *)modelRuntime,worldRuntime);
            }
            else {
              contactStateFlags = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
              *contactStateFlags = *contactStateFlags & 0xffa0;
              contactStateFlags = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
              *contactStateFlags = *contactStateFlags |
                   (ARMY_ARTICULATED_STEP_WALK | ARMY_ARTICULATED_STEP_RIGHT_LAST | ARMY_ARTICULATED_STEP_RIGHT);
              ArmyArticulatedRuntime_UpdateRightTerrainContact
                        (targetAngleLength.angle,targetAngleLength.length,
                         (ArmyArticulatedRuntimeSlotView *)modelRuntime,worldRuntime);
            }
            goto ArmyArticulatedMovement_CommitPositionSuspensionAndTransforms;
          }
        }
        else if ((stepSteerOrWorldX < (ModelRuntimeNode *)0x201) ||
                ((ModelRuntimeNode *)0xfdff < stepSteerOrWorldX))
        goto ArmyArticulatedMovement_UpdateSelectedTerrainContact;
      }
      if (((modelRuntime->articulatedContact).fallbackPosition0Q12 & ARMY_ARTICULATED_STEP_WALK) != 0)
      goto ArmyArticulatedMovement_InitializeSelectedTerrainContact;
      if (targetAngleLength.length < 0x41)
      goto ArmyArticulatedMovement_ClearContactTransitionAndAdvanceWaypoint;
      if ((-1 < (int)targetAngleLength.length) ||
         (((ModelRuntimeNode *)0x800 < stepSteerOrWorldX &&
          (stepSteerOrWorldX < (ModelRuntimeNode *)0xf800)))) {
        /* turn on the spot, with the foot on the side of the turn */
        ArmyArticulatedRuntime_UpdateSelectedTerrainContact
                  ((AngleTurn32)stepSteerOrWorldX,(ArmyArticulatedRuntimeSlotView *)modelRuntime,
                   worldRuntime);
        contactStateFlags = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
        *contactStateFlags = *contactStateFlags &
             ~(ARMY_ARTICULATED_STEP_CLOSE | ARMY_ARTICULATED_STEP_TURN | ARMY_ARTICULATED_STEP_RIGHT_LAST |
               ARMY_ARTICULATED_STEP_LEFT_LAST | ARMY_ARTICULATED_STEP_RIGHT | ARMY_ARTICULATED_STEP_LEFT);
        if (stepSteerOrWorldX < (ModelRuntimeNode *)0x8000) {
          contactStateFlags = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
          *contactStateFlags = *contactStateFlags |
               (ARMY_ARTICULATED_STEP_TURN | ARMY_ARTICULATED_STEP_LEFT_LAST | ARMY_ARTICULATED_STEP_LEFT);
        }
        else {
          contactStateFlags = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
          *contactStateFlags = *contactStateFlags |
               (ARMY_ARTICULATED_STEP_TURN | ARMY_ARTICULATED_STEP_RIGHT_LAST | ARMY_ARTICULATED_STEP_RIGHT);
        }
        goto ArmyArticulatedMovement_CommitPositionSuspensionAndTransforms;
      }
    }
ArmyArticulatedMovement_SharedContinuation:
    /* standing: close the feet when the line between them is not roughly square to the heading */
    if (((modelRuntime->articulatedContact).fallbackPosition0Q12 & ARMY_ARTICULATED_STEP_CLOSE) == 0) {
      speedHeadingOrWorldY.signedValue = modelRuntime->definitionClassValue80 - modelRuntime->definitionClassValue84;
      advanceOrHeading = FixedMath_Atan2Angle16
                        (speedHeadingOrWorldY.signedValue,
                         modelRuntime->movementTarget0Q12 - modelRuntime->movementTarget1Q12);
      feetLineHeading = (advanceOrHeading - (rootNode->modelPayload).worldRotationAngle2) - 0x4000 & 0xffff;
      if ((0xfff < feetLineHeading) && ((feetLineHeading < 0x7000 || ((0x8fff < feetLineHeading && (feetLineHeading < 0xf000))))))
      goto ArmyArticulatedMovement_InitializeSelectedTerrainContact;
    }
    stepSteerOrWorldX =
         (ModelRuntimeNode *)(modelRuntime->rootModelNode->worldTransform).translation.x;
    speedHeadingOrWorldY = THANDOR_BITCAST(GraphicsWorldCoordinateQ12, ArmyRuntimeCoordinateCommandOrHistoryValue4, (modelRuntime->rootModelNode->worldTransform).translation.y);
    if ((worldRuntime->fieldGrid->runtimeStateFlags & 1) == 0) {
      return;
    }
  }
  else {
ArmyArticulatedMovement_InitializeSelectedTerrainContact:
    /* second half of a turn, or a closing step: the other foot is set beside the first */
    stepSteerOrWorldX =
         (ModelRuntimeNode *)
         (((modelRuntime->articulatedContact).fallbackPosition0Q12 >> 16) +
          modelRuntime->ownerValue64 & 0xffff);
    if (((modelRuntime->articulatedContact).fallbackPosition0Q12 & ARMY_ARTICULATED_STEP_LEFT_LAST) == 0) {
      contactStateFlags = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
      *contactStateFlags = *contactStateFlags & ~0x3f;
      contactStateFlags = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
      *contactStateFlags = *contactStateFlags |
           (ARMY_ARTICULATED_STEP_CLOSE | ARMY_ARTICULATED_STEP_LEFT_LAST | ARMY_ARTICULATED_STEP_LEFT);
      ArmyArticulatedRuntime_InitializeLeftTerrainContact
                ((AngleTurn16Stored32)stepSteerOrWorldX,(ArmyArticulatedRuntimeSlotView *)modelRuntime
                 ,worldRuntime);
    }
    else {
      contactStateFlags = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
      *contactStateFlags = *contactStateFlags & ~0x3f;
      contactStateFlags = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
      *contactStateFlags = *contactStateFlags |
           (ARMY_ARTICULATED_STEP_CLOSE | ARMY_ARTICULATED_STEP_RIGHT_LAST | ARMY_ARTICULATED_STEP_RIGHT);
      ArmyArticulatedRuntime_InitializeRightTerrainContact
                ((AngleTurn16Stored32)stepSteerOrWorldX,(ArmyArticulatedRuntimeSlotView *)modelRuntime
                 ,worldRuntime);
    }
  }
ArmyArticulatedMovement_CommitPositionSuspensionAndTransforms:
  rootNode = modelRuntime->rootModelNode;
  (rootNode->worldTransform).translation.x = (GraphicsWorldCoordinateQ12)stepSteerOrWorldX;
  (rootNode->worldTransform).translation.y = THANDOR_BITCAST(ArmyRuntimeCoordinateCommandOrHistoryValue4, GraphicsWorldCoordinateQ12, speedHeadingOrWorldY);
  rootNode->runtimeFlags = rootNode->runtimeFlags | 1;
  ArmyArticulatedRuntime_UpdateSuspensionHierarchy(rootNode,worldRuntime);
  if (((previousWorldX != (rootNode->worldTransform).translation.x) ||
      (previousWorldY != (rootNode->worldTransform).translation.y)) ||
     (previousRotationAngle != (rootNode->modelPayload).worldRotationAngle2)) {
    linkedOrOwnerArmy = modelRuntime->ownerArmyRuntime;
    ArmyRuntime_UpdateTimedShotAndEffectEmitters
              (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
    ownerMovementFlags = &linkedOrOwnerArmy->movementStateFlags;
    *ownerMovementFlags = *ownerMovementFlags & ~ARMY_MOVEMENT_STATIONARY;
  }
  movementDefinition = modelRuntime->modelDefinition;
  ModelNodeRuntime_RebuildTransformsFromRoot(rootNode);
  ModelNodeRuntime_UpdateDepthBinMasks(movementDefinition->placementRadiusOrClearanceDC,rootNode);
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
   Runtime update of model class 18 (runtimeUpdate slot 18 of g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes,
   called by model class from ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive): runs the AI special
   behaviour while the owner has movement flag 0x100 (set by that behaviour), then moves the unit with ground
   movement variant B when it sits on the water surface, otherwise with variant A.
*/

void __thandor_preserve_eax_edx
ArmyRuntimeClass_UpdateSpecialBehaviorAndGroundMovement
          (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView200 *modelRuntime
          )

{
  ArmyRuntimeSlot *armyRuntime;
  ArmyPlacementContactKindIndex32 placementContactKind;

  armyRuntime = modelRuntime->ownerArmyRuntime;
  placementContactKind = modelRuntime->modelDefinition->placementContactKindIndex278;
  if ((armyRuntime->movementStateFlags & 0x100) != 0) {
    AiUnitBehavior_UpdateSpecialClass12Entity
              ((MdlDefinitionSemanticPrefix80 *)modelRuntime->modelDefinition,armyRuntime,
               armyRuntime->factionIndex,worldRuntime);
  }
  if (placementContactKind == ARMY_PLACEMENT_CONTACT_KIND_WATER_SURFACE) {
    ArmyRuntimeClass_UpdateGroundMovementVariantB(worldRuntime,modelRuntime);
  }
  else {
    ArmyRuntimeClass_UpdateGroundMovementVariantA(worldRuntime,modelRuntime);
  }
  return;
}


/* Address: 0x00523410.
   Turret with one barrel (runtimeUpdate slot 7 of g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes, called by
   model class from ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive). The root node yaws, its first
   child pitches, and that child's first child is the barrel: while reloading it spins by the definition's step
   per tick, after a shot it recoils back and returns over the recoil countdown. With an aim point the turret
   turns towards the launch direction and fires once it is on target and reloaded, unless
   ArmyRuntimeCommand_UpdateTargetFollowingState finds the line of fire blocked; without one it returns to rest
   while the owner moves or it is still turning. Skipped while destroyed.
*/

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_UpdateMovementAimAndProjectilesVariantA
          (WorldRuntimeContext *worldRuntime,ModelRuntimeWeaponAimStateView200 *modelRuntime)

{
  ModelRuntimeFlags *nodeFlags;
  WeaponAimCountdownTicks *countdownTicks;
  AngleTurn32 *rotationAngle;
  ArmyWeaponDefinitionView68 *weaponDefinition;
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
  ShotRuntimeState14 targetReference;
  bool lineOfFireBlocked;
  ShotLaunchAnglesEaxEdx8 launchAngles;
  ModelRelativeDirectionAnglesEaxEdx8 relativeAngles;
  AimSmoothResult smoothResult;
  MovementStepResult waypointResult;
  WorldPositionResult aimPoint;
  GameEntityRuntime *ownerEntity;
  ModelRuntimeNode *partNode;

  elapsedTicks = g_InGameSimulationStepTicks;
  /* bit 0x1 of the runtime flags is not named yet */
  if (((modelRuntime->classState).classStateEC & (ARMY_RUNTIME_FLAG_DESTROYED | 0x1)) == 0) {
    weaponDefinition = modelRuntime->modelDefinition;
    ownerEntity = (GameEntityRuntime *)modelRuntime->ownerArmyRuntime;
    recoilCountdown = modelRuntime->attachment0BackwardStepCountdownTicks28;
    if (modelRuntime->attachmentReloadCountdownTicks24 != 0) {
      /* spin the barrel for the reload ticks that elapsed (at most the remaining countdown) */
      partNode = modelRuntime->rootModelNode->childNodes[0];
      countdownTicks = &modelRuntime->attachmentReloadCountdownTicks24;
      *countdownTicks = *countdownTicks - g_InGameSimulationStepTicks;
      if (*countdownTicks < 0) {
        elapsedTicks = elapsedTicks + modelRuntime->attachmentReloadCountdownTicks24;
        modelRuntime->attachmentReloadCountdownTicks24 = 0;
      }
      partNode = partNode->childNodes[0];
      rotationAngle = &(partNode->modelPayload).localRotationAngle2;
      *rotationAngle = *rotationAngle + elapsedTicks * weaponDefinition->localRotationAngle2StepPerTick34;
      partNode->runtimeFlags = partNode->runtimeFlags | 1;
      rotationAngle = &(partNode->modelPayload).localRotationAngle2;
      *rotationAngle = *rotationAngle & 0xffff;
    }
    elapsedTicks = g_InGameSimulationStepTicks;
    if (recoilCountdown != 0) {
      /* move the barrel forward again by the elapsed recoil ticks */
      recoilScale = weaponDefinition->backwardStepScale3C;
      partNode = modelRuntime->rootModelNode->childNodes[0];
      countdownTicks = &modelRuntime->attachment0BackwardStepCountdownTicks28;
      *countdownTicks = *countdownTicks - g_InGameSimulationStepTicks;
      if (*countdownTicks < 0) {
        elapsedTicks = elapsedTicks + modelRuntime->attachment0BackwardStepCountdownTicks28;
        modelRuntime->attachment0BackwardStepCountdownTicks28 = 0;
      }
      partNode = partNode->childNodes[0];
      FixedVector_StepBackwardAlongOwnDirection(-elapsedTicks,recoilScale,(FixedVectorStateAddress32)partNode);
      nodeFlags = &partNode->runtimeFlags;
      *nodeFlags = *nodeFlags | 1;
    }
    partNode = modelRuntime->rootModelNode;
    aimPoint = ArmyRuntime_ResolveShotAimPoint
                       ((partNode->worldTransform).translation.z,
                        (partNode->worldTransform).translation.y,
                        (partNode->worldTransform).translation.x,weaponDefinition->shotDefinition,
                        ownerEntity);
    aimWorldZ = aimPoint.worldZQ12;
    aimWorldY = aimPoint.worldYQ12;
    aimWorldX = aimPoint.worldXQ12;
    if (aimPoint.unresolved) {
      waypointResult = ArmyRuntime_UpdateMovementAndWaypoints
                         (worldRuntime,(ArmyMovementRuntime *)ownerEntity);
      if (((!waypointResult.arrived) || (modelRuntime->pitchTurnVelocityAngle16 != 0)) ||
         (modelRuntime->yawTurnVelocityAngle16 != 0)) {
        partNode = modelRuntime->rootModelNode;
        ModelNodeRuntime_SmoothYawTowardTarget(partNode,modelRuntime,0);
        ModelNodeRuntime_SmoothPitchTowardTarget(partNode->childNodes[0],modelRuntime,0);
      }
    }
    else {
      partNode = modelRuntime->rootModelNode;
      pitchNode = partNode->childNodes[0];
      launchAngles = ShotDefinition_ComputeLaunchAnglesRegs
                         (aimWorldZ,aimWorldY,aimWorldX,(pitchNode->worldTransform).translation.z,
                          (pitchNode->worldTransform).translation.y,
                          (pitchNode->worldTransform).translation.x,weaponDefinition->shotDefinition);
      relativeAngles = ModelNodeRuntime_ComputeRelativeDirectionAngle
                         (partNode,launchAngles.elevationAngle,launchAngles.headingAngle);
      targetPitchAngle16 = relativeAngles.relativePitchAngle;
      smoothResult = ModelNodeRuntime_SmoothYawTowardTarget
                         (partNode,modelRuntime,relativeAngles.relativeYawAngle);
      if (smoothResult.outsideTolerance) {
        ModelNodeRuntime_SmoothPitchTowardTarget(pitchNode,modelRuntime,targetPitchAngle16);
      }
      else {
        smoothResult = ModelNodeRuntime_SmoothPitchTowardTarget
                           (pitchNode,modelRuntime,targetPitchAngle16);
        if ((smoothResult.value == targetPitchAngle16) &&
           (weaponDefinition = modelRuntime->modelDefinition,
           modelRuntime->attachmentReloadCountdownTicks24 == 0)) {
          lineOfFireBlocked = ArmyRuntimeCommand_UpdateTargetFollowingState
                            (aimWorldZ,aimWorldY,aimWorldX,worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
          if (!lineOfFireBlocked) {
            /* fire: reload, recoil the barrel, rock the owner back and launch the projectiles */
            recoilTicks = weaponDefinition->sharedInterShotTicks;
            recoilScale = weaponDefinition->backwardStepScale3C;
            modelRuntime->attachmentReloadCountdownTicks24 =
                 modelRuntime->attachmentReloadCountdownTicks24 + weaponDefinition->attachmentReloadTicks;
            modelRuntime->attachment0BackwardStepCountdownTicks28 =
                 modelRuntime->attachment0BackwardStepCountdownTicks28 + recoilTicks;
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
   Turret with two alternating barrels (runtimeUpdate slot 8 of g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes,
   called by model class from ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive). Same as
   ArmyRuntimeClass_UpdateMovementAimAndProjectilesVariantA, but the pitch node has two barrels (children 0 and
   1) with their own recoil countdowns; the shots alternate between them, the even sequence numbers firing from
   barrel 1 and its muzzle point.
*/

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_UpdateMovementAimAndProjectilesVariantB
          (WorldRuntimeContext *worldRuntime,ModelRuntimeWeaponAimStateView200 *modelRuntime)

{
  ModelRuntimeFlags *nodeFlags;
  WeaponAimCountdownTicks *countdownTicks;
  AngleTurn32 *rotationAngle;
  ArmyWeaponDefinitionView68 *weaponDefinition;
  FixedMathScale32 recoilScale;
  ModelRuntimeNode *pitchNode;
  uint32_t recoilTicks;
  ArmyRuntimeSlot *commandTargetArmy;
  InGameSimulationStepBatchTicks elapsedTicks;
  Q12 aimWorldX;
  /* barrel 0 recoil countdown, later the byte offset of the firing barrel's muzzle point (0 or 4) */
  int countdownOrPointOffset;
  Q12 aimWorldY;
  Q12 aimWorldZ;
  AngleTurn32 targetPitchAngle16;
  ShotRuntimeState14 targetReference;
  bool lineOfFireBlocked;
  ShotLaunchAnglesEaxEdx8 launchAngles;
  ModelRelativeDirectionAnglesEaxEdx8 relativeAngles;
  AimSmoothResult smoothResult;
  MovementStepResult waypointResult;
  WorldPositionResult aimPoint;
  GameEntityRuntime *ownerEntity;
  ModelRuntimeNode *partNode;

  elapsedTicks = g_InGameSimulationStepTicks;
  /* bit 0x1 of the runtime flags is not named yet */
  if (((modelRuntime->classState).classStateEC & (ARMY_RUNTIME_FLAG_DESTROYED | 0x1)) == 0) {
    weaponDefinition = modelRuntime->modelDefinition;
    ownerEntity = (GameEntityRuntime *)modelRuntime->ownerArmyRuntime;
    countdownOrPointOffset = modelRuntime->attachment0BackwardStepCountdownTicks28;
    if (modelRuntime->attachmentReloadCountdownTicks24 != 0) {
      /* spin the first barrel for the reload ticks that elapsed */
      partNode = modelRuntime->rootModelNode->childNodes[0];
      countdownTicks = &modelRuntime->attachmentReloadCountdownTicks24;
      *countdownTicks = *countdownTicks - g_InGameSimulationStepTicks;
      if (*countdownTicks < 0) {
        elapsedTicks = elapsedTicks + modelRuntime->attachmentReloadCountdownTicks24;
        modelRuntime->attachmentReloadCountdownTicks24 = 0;
      }
      partNode = partNode->childNodes[0];
      rotationAngle = &(partNode->modelPayload).localRotationAngle2;
      *rotationAngle = *rotationAngle + elapsedTicks * weaponDefinition->localRotationAngle2StepPerTick34;
      partNode->runtimeFlags = partNode->runtimeFlags | 1;
      rotationAngle = &(partNode->modelPayload).localRotationAngle2;
      *rotationAngle = *rotationAngle & 0xffff;
    }
    elapsedTicks = g_InGameSimulationStepTicks;
    if (countdownOrPointOffset != 0) {
      /* barrel 0 returns from its recoil */
      recoilScale = weaponDefinition->backwardStepScale3C;
      partNode = modelRuntime->rootModelNode->childNodes[0];
      countdownTicks = &modelRuntime->attachment0BackwardStepCountdownTicks28;
      *countdownTicks = *countdownTicks - g_InGameSimulationStepTicks;
      if (*countdownTicks < 0) {
        elapsedTicks = elapsedTicks + modelRuntime->attachment0BackwardStepCountdownTicks28;
        modelRuntime->attachment0BackwardStepCountdownTicks28 = 0;
      }
      partNode = partNode->childNodes[0];
      FixedVector_StepBackwardAlongOwnDirection(-elapsedTicks,recoilScale,(FixedVectorStateAddress32)partNode);
      nodeFlags = &partNode->runtimeFlags;
      *nodeFlags = *nodeFlags | 1;
    }
    elapsedTicks = g_InGameSimulationStepTicks;
    if (modelRuntime->attachment1BackwardStepCountdownTicks2C != 0) {
      /* barrel 1 returns from its recoil */
      recoilScale = weaponDefinition->backwardStepScale3C;
      partNode = modelRuntime->rootModelNode->childNodes[0];
      countdownTicks = &modelRuntime->attachment1BackwardStepCountdownTicks2C;
      *countdownTicks = *countdownTicks - g_InGameSimulationStepTicks;
      if (*countdownTicks < 0) {
        elapsedTicks = elapsedTicks + modelRuntime->attachment1BackwardStepCountdownTicks2C;
        modelRuntime->attachment1BackwardStepCountdownTicks2C = 0;
      }
      partNode = partNode->childNodes[1];
      FixedVector_StepBackwardAlongOwnDirection(-elapsedTicks,recoilScale,(FixedVectorStateAddress32)partNode);
      nodeFlags = &partNode->runtimeFlags;
      *nodeFlags = *nodeFlags | 1;
    }
    partNode = modelRuntime->rootModelNode;
    aimPoint = ArmyRuntime_ResolveShotAimPoint
                       ((partNode->worldTransform).translation.z,
                        (partNode->worldTransform).translation.y,
                        (partNode->worldTransform).translation.x,weaponDefinition->shotDefinition,
                        ownerEntity);
    aimWorldZ = aimPoint.worldZQ12;
    aimWorldY = aimPoint.worldYQ12;
    aimWorldX = aimPoint.worldXQ12;
    if (aimPoint.unresolved) {
      waypointResult = ArmyRuntime_UpdateMovementAndWaypoints
                         (worldRuntime,(ArmyMovementRuntime *)ownerEntity);
      if (((!waypointResult.arrived) || (modelRuntime->pitchTurnVelocityAngle16 != 0)) ||
         (modelRuntime->yawTurnVelocityAngle16 != 0)) {
        partNode = modelRuntime->rootModelNode;
        ModelNodeRuntime_SmoothYawTowardTarget(partNode,modelRuntime,0);
        ModelNodeRuntime_SmoothPitchTowardTarget(partNode->childNodes[0],modelRuntime,0);
      }
    }
    else {
      partNode = modelRuntime->rootModelNode;
      pitchNode = partNode->childNodes[0];
      launchAngles = ShotDefinition_ComputeLaunchAnglesRegs
                         (aimWorldZ,aimWorldY,aimWorldX,(pitchNode->worldTransform).translation.z,
                          (pitchNode->worldTransform).translation.y,
                          (pitchNode->worldTransform).translation.x,weaponDefinition->shotDefinition);
      relativeAngles = ModelNodeRuntime_ComputeRelativeDirectionAngle
                         (partNode,launchAngles.elevationAngle,launchAngles.headingAngle);
      targetPitchAngle16 = relativeAngles.relativePitchAngle;
      smoothResult = ModelNodeRuntime_SmoothYawTowardTarget
                         (partNode,modelRuntime,relativeAngles.relativeYawAngle);
      if (smoothResult.outsideTolerance) {
        ModelNodeRuntime_SmoothPitchTowardTarget(pitchNode,modelRuntime,targetPitchAngle16);
      }
      else {
        smoothResult = ModelNodeRuntime_SmoothPitchTowardTarget
                           (pitchNode,modelRuntime,targetPitchAngle16);
        if ((smoothResult.value == targetPitchAngle16) &&
           (weaponDefinition = modelRuntime->modelDefinition,
           modelRuntime->attachmentReloadCountdownTicks24 == 0)) {
          lineOfFireBlocked = ArmyRuntimeCommand_UpdateTargetFollowingState
                            (aimWorldZ,aimWorldY,aimWorldX,worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
          if (!lineOfFireBlocked) {
            recoilTicks = weaponDefinition->sharedInterShotTicks;
            recoilScale = weaponDefinition->backwardStepScale3C;
            modelRuntime->attachmentReloadCountdownTicks24 =
                 modelRuntime->attachmentReloadCountdownTicks24 + weaponDefinition->attachmentReloadTicks;
            if ((modelRuntime->alternatingAttachmentSequence20 & 1) == 0) {
              modelRuntime->attachment1BackwardStepCountdownTicks2C =
                   modelRuntime->attachment1BackwardStepCountdownTicks2C + recoilTicks;
              partNode = pitchNode->childNodes[1];
              countdownOrPointOffset = 4;
            }
            else {
              modelRuntime->attachment0BackwardStepCountdownTicks28 =
                   modelRuntime->attachment0BackwardStepCountdownTicks28 + recoilTicks;
              partNode = pitchNode->childNodes[0];
              countdownOrPointOffset = 0;
            }
            modelRuntime->alternatingAttachmentSequence20++;
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
   Ground movement of tracked vehicles (runtimeUpdate slot 2 of g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes,
   called by model class from ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive). Moves exactly like
   ArmyRuntimeClass_UpdateGroundMovementVariantA and then scrolls the texture of the left and right track by the
   signed distance each track side travelled this tick (so the tracks also run while turning on the spot),
   wrapping the offsets at +-0x100000.
*/

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_UpdateGroundMovementCollisionAndTrackAnimation
          (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementTrackView200 *modelRuntime)

{
  ArmyMovementStateFlags *ownerMovementFlags;
  ArmyRuntimeSlot *linkedOrOwnerArmy;
  AngleTurn32 previousRotationAngle;
  int previousWorldX;
  int previousWorldY;
  ModelDefinitionGroundMovementTrackView280 *movementDefinition;
  ArmyPlacementContactKindIndex32 placementContactKind;
  ModelResourceHitTestAndRenderView210 *nodeModelResource;
  int32_t waterDelta;
  /* damage, route delta X, next turn velocity, travel distance, recoil tilt, then track point X / offset */
  int primaryDelta;
  uint32_t facingAngle;
  uint32_t trackDistance;
  uint32_t turnVelocityOrLimit;
  /* route delta Y, turn velocity limit, then track point Y / offset */
  int secondaryDelta;
  uint32_t headingDifference;
  uint32_t desiredHeading;
  ModelRuntimeNode *placedRootNode;
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
  ModelRuntimeNode *rootNode;

  linkedOrOwnerArmy = (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime;
  rootNode = modelRuntime->rootModelNode;
  ownerMovementFlags = &modelRuntime->ownerArmyRuntime->movementStateFlags;
  *ownerMovementFlags = *ownerMovementFlags | ARMY_MOVEMENT_STATIONARY;
  /* Drop the linked runtime unless both have a clearance radius and this unit is still within it. */
  if ((linkedOrOwnerArmy != NULL) &&
      ((((((linkedOrOwnerArmy->modelRuntimeOrSavedOffset).modelRuntime)->classState).classStateDC == 0) ||
        (modelRuntime->modelDefinition->placementRadiusOrClearanceDC == 0)) ||
       (withinLinkRadius = ArmyCollision_TestPointWithinExpandedRuntimeRadius
                           (modelRuntime->modelDefinition->placementRadiusOrClearanceDC,
                            (rootNode->worldTransform).translation.y,
                            (rootNode->worldTransform).translation.x,linkedOrOwnerArmy), !withinLinkRadius))) {
    (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime = NULL;
  }
  previousRotationAngle = (rootNode->modelPayload).worldRotationAngle2;
  previousWorldX = (rootNode->worldTransform).translation.x;
  previousWorldY = (rootNode->worldTransform).translation.y;
  movementDefinition = modelRuntime->modelDefinition;
  waterDelta = FieldGrid_InterpolateWaterDelta(previousWorldY,previousWorldX,worldRuntime->fieldGrid);
  if ((movementDefinition->waterDamageThreshold198 < waterDelta) &&
     (primaryDelta = waterDelta * movementDefinition->waterDamageMultiplier194 >> 7, -1 < primaryDelta)) {
    ArmyRuntime_ApplyDamageAndPropagateToParent(primaryDelta,(ArmyRuntimeSlot *)modelRuntime);
  }
  if (((modelRuntime->classState).classStateEC & ARMY_RUNTIME_FLAG_DESTROYED) == 0) {
    waypointResult = ArmyRuntime_UpdateMovementAndWaypoints
                       (worldRuntime,(ArmyMovementRuntime *)modelRuntime->ownerArmyRuntime);
    if (waypointResult.arrived) goto ArmyGroundMovementCollision_StopMovementAndTurn;
    secondaryDelta = waypointResult.worldYQ12 - (rootNode->worldTransform).translation.y;
    primaryDelta = waypointResult.worldXQ12 - (rootNode->worldTransform).translation.x;
    if ((primaryDelta == 0) && (secondaryDelta == 0)) {
      angleAndLength = THANDOR_BITCAST(uint64_t, FixedLengthAngleEaxEdx8, ((uint64_t)(rootNode->modelPayload).worldRotationAngle2 << 32));
    }
    else {
      angleAndLength = FixedMath_Vector2AngleAndLengthRegs(secondaryDelta,primaryDelta);
    }
    /* accelerated turning as in ArmyRuntimeClass_UpdateGroundMovementVariantA */
    desiredHeading = angleAndLength.angle;
    movementDefinition = modelRuntime->modelDefinition;
    facingAngle = (rootNode->modelPayload).worldRotationAngle2;
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
    rootNode = modelRuntime->rootModelNode;
    facingAngle = facingAngle & 0xffff;
    if (facingAngle != (rootNode->modelPayload).worldRotationAngle2) {
      (rootNode->modelPayload).worldRotationAngle2 = facingAngle;
      rootNode->runtimeFlags = rootNode->runtimeFlags | 1;
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
                         (primaryDelta,(rootNode->modelPayload).worldRotationAngle2,
                          (rootNode->worldTransform).translation.y,
                          (rootNode->worldTransform).translation.x);
    }
    else {
      linkedOrOwnerArmy = modelRuntime->ownerArmyRuntime;
      waypointResult = ArmyRuntime_UpdateMovementAndWaypoints(worldRuntime,(ArmyMovementRuntime *)linkedOrOwnerArmy);
      nextPosition = THANDOR_PART(uint64_t, waypointResult, 0);
      ownerMovementFlags = &linkedOrOwnerArmy->movementStateFlags;
      *ownerMovementFlags = *ownerMovementFlags | ARMY_MOVEMENT_ROUTE_POINT_REACHED;
    }
    placementContactKind = movementDefinition->placementContactKindIndex278;
    rootNode = modelRuntime->rootModelNode;
    heightOffsetQ12 = movementDefinition->placementHeightOffsetQ12;
    dispatchWorldRuntime = worldRuntime;
    blockingCollision = ArmyCollision_FindBlockingRuntimeForCurrentUnit
                       ((Q12)(nextPosition >> 32),(Q12)nextPosition,
                        (RuntimeCollisionQueryViewF4 *)modelRuntime,worldRuntime);
    if (blockingCollision.blocked) {
      placedRootNode = modelRuntime->rootModelNode;
      ArmyRuntime_HandleCollisionPartner
                ((ArmyRuntimeSlot *)modelRuntime,(placedRootNode->worldTransform).translation.y,
                 (placedRootNode->worldTransform).translation.x,(ArmyRuntimeSlot *)blockingCollision.blockingArmy,
                 worldRuntime);
      (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
      THANDOR_PART(uint32_t, nextPosition, 0) = (placedRootNode->worldTransform).translation.x;
      THANDOR_PART(uint32_t, nextPosition, 4) = (placedRootNode->worldTransform).translation.y;
      ownerMovementFlags = &modelRuntime->ownerArmyRuntime->movementStateFlags;
      *ownerMovementFlags = *ownerMovementFlags | ARMY_MOVEMENT_ROUTE_POINT_REACHED;
    }
    linkedOrOwnerArmy = modelRuntime->ownerArmyRuntime;
    placedRootNode = modelRuntime->rootModelNode;
    g_ArmyPlacementContactKindDispatchTable.callbacks[placementContactKind]
              (heightOffsetQ12,(Q12)(nextPosition >> 32),(Q12)nextPosition,rootNode,dispatchWorldRuntime);
    /* recoil after a shot, as in ArmyRuntimeClass_UpdateGroundMovementVariantA */
    primaryDelta = linkedOrOwnerArmy->actionVector1Q12 - 1;
    if (primaryDelta < 0) goto ArmyGroundMovementCollision_FinalizeEffectsAnimationAndTransforms;
    primaryDelta = primaryDelta * linkedOrOwnerArmy->actionVector2Q12;
    linkedOrOwnerArmy->actionVector1Q12 = linkedOrOwnerArmy->actionVector1Q12 - 1;
  }
  else {
ArmyGroundMovementCollision_StopMovementAndTurn:
    (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
    (modelRuntime->movementControl).turnVelocityAngle16 = 0;
ArmyGroundMovementCollision_ProcessStationaryPlacementAndDamageState:
    linkedOrOwnerArmy = modelRuntime->ownerArmyRuntime;
    placedRootNode = modelRuntime->rootModelNode;
    primaryDelta = linkedOrOwnerArmy->actionVector1Q12 - 1;
    if (primaryDelta < 0) {
      if ((worldRuntime->fieldGrid->runtimeStateFlags & 1) != 0) {
        (*g_ArmyPlacementContactKindDispatchTable.callbacks
          [modelRuntime->modelDefinition->placementContactKindIndex278])
                  (modelRuntime->modelDefinition->placementHeightOffsetQ12,
                   (placedRootNode->worldTransform).translation.y,
                   (placedRootNode->worldTransform).translation.x,placedRootNode,worldRuntime);
      }
      goto ArmyGroundMovementCollision_FinalizeEffectsAnimationAndTransforms;
    }
    primaryDelta = primaryDelta * linkedOrOwnerArmy->actionVector2Q12;
    linkedOrOwnerArmy->actionVector1Q12 = linkedOrOwnerArmy->actionVector1Q12 - 1;
    (*g_ArmyPlacementContactKindDispatchTable.callbacks
      [modelRuntime->modelDefinition->placementContactKindIndex278])
              (modelRuntime->modelDefinition->placementHeightOffsetQ12,
               (placedRootNode->worldTransform).translation.y,(placedRootNode->worldTransform).translation.x
               ,placedRootNode,worldRuntime);
  }
  composedAngles = FixedTransform_ComposeEulerAnglesRegs
                     (0,0x4000 - primaryDelta,
                      (modelRuntime->ownerArmyRuntime->actionVector0Q12 + 0x8000) -
                      (placedRootNode->modelPayload).worldRotationAngle2 & 0xffff,
                      (placedRootNode->modelPayload).worldRotationAngle2,
                      (placedRootNode->modelPayload).worldRotationAngle1,
                      (placedRootNode->modelPayload).worldRotationAngle0);
  (placedRootNode->modelPayload).worldRotationAngle0 = composedAngles.angle0;
  (placedRootNode->modelPayload).worldRotationAngle1 = composedAngles.angle1;
  (placedRootNode->modelPayload).worldRotationAngle2 = composedAngles.angle2;
ArmyGroundMovementCollision_FinalizeEffectsAnimationAndTransforms:
  /* Track animation: for each side, the distance between the side point (localBoundsY0Q12 to the side of the
     centre) now and at the start of the tick, negative when that side moved backwards. The reused locals
     hold: desiredHeading = previous heading - 90 degrees, facingAngle = current heading - 90 degrees. */
  nodeModelResource = (placedRootNode->modelPayload).modelResource;
  desiredHeading = previousRotationAngle - 0x4000 & 0xffff;
  facingAngle = (placedRootNode->modelPayload).worldRotationAngle2 - 0x4000 & 0xffff;
  sinCosOffset = FixedMath_SinCosScaled(facingAngle,nodeModelResource->localBoundsY0Q12);
  primaryDelta = (int)sinCosOffset + (placedRootNode->worldTransform).translation.x;
  secondaryDelta = (int)(sinCosOffset >> 32) + (placedRootNode->worldTransform).translation.y;
  sinCosOffset = FixedMath_SinCosScaled(desiredHeading,nodeModelResource->localBoundsY0Q12);
  angleAndLength = FixedMath_Vector2AngleAndLengthRegs
                     (secondaryDelta - ((int)(sinCosOffset >> 32) + previousWorldY),primaryDelta - ((int)sinCosOffset + previousWorldX));
  trackDistance = angleAndLength.length;
  turnVelocityOrLimit = angleAndLength.angle - (placedRootNode->modelPayload).worldRotationAngle2 & 0xffff;
  if ((0x4000 < turnVelocityOrLimit) && (turnVelocityOrLimit < 0xc000)) {
    trackDistance = -trackDistance;
  }
  placedRootNode->primaryTextureOffsetU =
       placedRootNode->primaryTextureOffsetU +
       trackDistance * modelRuntime->modelDefinition->trackTextureUScalePerDistance14;
  /* the other side (heading + 90 degrees) */
  sinCosOffset = FixedMath_SinCosScaled(facingAngle ^ 0x8000,nodeModelResource->localBoundsY0Q12);
  primaryDelta = (int)sinCosOffset + (placedRootNode->worldTransform).translation.x;
  secondaryDelta = (int)(sinCosOffset >> 32) + (placedRootNode->worldTransform).translation.y;
  sinCosOffset = FixedMath_SinCosScaled(desiredHeading ^ 0x8000,nodeModelResource->localBoundsY0Q12);
  angleAndLength = FixedMath_Vector2AngleAndLengthRegs
                     (secondaryDelta - ((int)(sinCosOffset >> 32) + previousWorldY),primaryDelta - ((int)sinCosOffset + previousWorldX));
  trackDistance = angleAndLength.length;
  facingAngle = angleAndLength.angle - (placedRootNode->modelPayload).worldRotationAngle2 & 0xffff;
  if ((0x4000 < facingAngle) && (facingAngle < 0xc000)) {
    trackDistance = -trackDistance;
  }
  /* wrap both texture offsets back into +-0x100000 */
  primaryDelta = placedRootNode->primaryTextureOffsetU;
  secondaryDelta = trackDistance * modelRuntime->modelDefinition->trackTextureUScalePerDistance14 +
           placedRootNode->secondaryTextureOffsetU;
  if (secondaryDelta < 0x100001) {
    if (secondaryDelta < -0x100000) {
      secondaryDelta = secondaryDelta + 0x100000;
    }
  }
  else {
    secondaryDelta = secondaryDelta - 0x100000;
  }
  if (primaryDelta < 0x100001) {
    if (primaryDelta < -0x100000) {
      primaryDelta = primaryDelta + 0x100000;
    }
  }
  else {
    primaryDelta = primaryDelta - 0x100000;
  }
  placedRootNode->secondaryTextureOffsetU = secondaryDelta;
  placedRootNode->primaryTextureOffsetU = primaryDelta;
  if (((previousWorldX != (placedRootNode->worldTransform).translation.x) ||
      (previousWorldY != (placedRootNode->worldTransform).translation.y)) ||
     (previousRotationAngle != (placedRootNode->modelPayload).worldRotationAngle2)) {
    linkedOrOwnerArmy = modelRuntime->ownerArmyRuntime;
    ArmyRuntime_UpdateTimedShotAndEffectEmitters
              (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
    ownerMovementFlags = &linkedOrOwnerArmy->movementStateFlags;
    *ownerMovementFlags = *ownerMovementFlags & ~ARMY_MOVEMENT_STATIONARY;
  }
  movementDefinition = modelRuntime->modelDefinition;
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  ModelNodeRuntime_RebuildTransformsFromRoot(placedRootNode);
  ModelNodeRuntime_UpdateDepthBinMasks(movementDefinition->placementRadiusOrClearanceDC,placedRootNode);
  return;
}


/* Address: 0x00522C00.
   Movement of banking units with three animated child parts (runtimeUpdate slot 17 of
   g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes, called by model class from
   ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive). The unit slides straight towards the route
   point while it turns (turning like ArmyRuntimeClass_UpdateGroundMovementVariantA, but it keeps its heading
   when it is close and not turning), stops at blocking armies and is placed by its placement callback. It then
   banks: while moving the bank value (+0x60) sinks from 0x4000 (level) to at least 0x3800 and the bank heading
   (+0x64) follows the travel direction by at most 0x400 per tick; when stopped it returns to level. When the
   unit leaves its linked army while class state bit 4 is set, it plays the definition's positioned sound and
   spins its three child parts for the countdown at +0x70.
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
  /* next turn velocity, travel distance, then the recoil tilt */
  int primaryDelta;
  GraphicsFixedVec3 *worldPosition;
  /* the linked army, the route point Y, then the owner army */
  ArmyRuntimeSlot *armyOrWaypointY;
  int secondaryDelta;
  AngleTurn32 desiredHeading;
  uint32_t newHeading;
  /* child count, sound index, turn velocity, then the bank heading step */
  uint32_t turnVelocityOrIndex;
  ModelRuntimeNode *rootNode;
  bool testResult;
  FixedPlanarPointEdxEax8 nextPosition;
  ArmyCollisionResult blockingCollision;
  MovementStepResult waypointResult;
  FixedEulerAnglesEaxEbxEdx12 composedAngles;
  Q12 heightOffsetQ12;
  WorldRuntimeContext *dispatchWorldRuntime;
  FixedLengthAngleEaxEdx8 targetAngleLength;
  /* the second child part, later the root node at a collision */
  ModelRuntimeNode *secondChildOrRootNode;

  armyOrWaypointY = (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime;
  rootNode = modelRuntime->rootModelNode;
  ownerMovementFlags = &modelRuntime->ownerArmyRuntime->movementStateFlags;
  *ownerMovementFlags = *ownerMovementFlags | ARMY_MOVEMENT_STATIONARY;
  /* Drop the linked runtime unless both have a clearance radius and this unit is still within it. */
  if ((armyOrWaypointY != NULL) &&
      (movementDefinition = modelRuntime->modelDefinition,
      (((((armyOrWaypointY->modelRuntimeOrSavedOffset).modelRuntime)->classState).classStateDC == 0) ||
        (movementDefinition->placementRadiusOrClearanceDC == 0)) ||
       (testResult = ArmyCollision_TestPointWithinExpandedRuntimeRadius
                     (movementDefinition->placementRadiusOrClearanceDC,
                      (rootNode->worldTransform).translation.y,
                      (rootNode->worldTransform).translation.x,armyOrWaypointY), !testResult))) {
    turnVelocityOrIndex = rootNode->childCount;
    (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime = NULL;
    if ((2 < turnVelocityOrIndex) && (((modelRuntime->classState).classStateB8 & 4) != 0)) {
      /* start the child-part animation and play the sound whose index is at definition +0x274 */
      classStateWord = &(modelRuntime->classState).classStateB8;
      *classStateWord = *classStateWord | 1;
      turnVelocityOrIndex = *(uint32_t *)(movementDefinition->reserved26C_277 + 8);
      if ((turnVelocityOrIndex != 0) &&
         ((turnVelocityOrIndex < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != NULL)))) {
        voiceSetRef = (DirectSoundVoiceSet **)worldRuntime->dwordArray[turnVelocityOrIndex];
        if (voiceSetRef != NULL) {
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
  rootNode = modelRuntime->rootModelNode;
  previousRotationAngle = (rootNode->modelPayload).worldRotationAngle2;
  previousWorldX = (rootNode->worldTransform).translation.x;
  previousWorldY = (rootNode->worldTransform).translation.y;
  if (((modelRuntime->classState).classStateEC & ARMY_RUNTIME_FLAG_DESTROYED) == 0) {
    if (((modelRuntime->classState).classStateB8 & 1) != 0) {
      /* spin the three child parts until the countdown at +0x70 runs out, then clear bits 0-2 */
      classStateWord = &(modelRuntime->classLinkState).classState70;
      *classStateWord = *classStateWord - 1;
      if (*classStateWord == 0) {
        classStateWord = &(modelRuntime->classState).classStateB8;
        *classStateWord = *classStateWord & ~7u;
      }
      secondChildOrRootNode = rootNode->childNodes[1];
      thirdChildNode = rootNode->childNodes[2];
      childRotationAngle = &(rootNode->childNodes[0]->modelPayload).localRotationAngle1;
      *childRotationAngle = *childRotationAngle + 0x2aa;
      childRotationAngle = &(secondChildOrRootNode->modelPayload).localRotationAngle1;
      *childRotationAngle = *childRotationAngle + 0x2aa;
      childRotationAngle = &(thirdChildNode->modelPayload).localRotationAngle1;
      *childRotationAngle = *childRotationAngle + 0x2aa;
    }
    waypointResult = ArmyRuntime_UpdateMovementAndWaypoints
                       (worldRuntime,(ArmyMovementRuntime *)modelRuntime->ownerArmyRuntime);
    armyOrWaypointY = (ArmyRuntimeSlot *)waypointResult.worldYQ12;
    waypointWorldX = waypointResult.worldXQ12;
    if (waypointResult.arrived) goto ArmyMovementBanking_ProcessStoppedMovementPlacementAndDamageState;
    /* a new route point (+0x68/+0x6C) restarts from standstill */
    if ((waypointWorldX != (modelRuntime->classLinkState).classState68) &&
       (armyOrWaypointY != (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime)) {
      (modelRuntime->classLinkState).classState68 = waypointWorldX;
      (modelRuntime->classLinkState).armyLinkOrState6C.classState = (uint32_t)armyOrWaypointY;
      (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
      (modelRuntime->movementControl).turnVelocityAngle16 = 0;
    }
    primaryDelta = waypointWorldX - (rootNode->worldTransform).translation.x;
    secondaryDelta = (int)armyOrWaypointY - (rootNode->worldTransform).translation.y;
    if ((primaryDelta == 0) && (secondaryDelta == 0)) {
      targetAngleLength = THANDOR_BITCAST(uint64_t, FixedLengthAngleEaxEdx8, ((uint64_t)(rootNode->modelPayload).worldRotationAngle2 << 32));
    }
    else {
      targetAngleLength = FixedMath_Vector2AngleAndLengthRegs(secondaryDelta,primaryDelta);
    }
    desiredHeading = targetAngleLength.angle;
    movementDefinition = modelRuntime->modelDefinition;
    turnVelocityOrIndex = (modelRuntime->movementControl).turnVelocityAngle16;
    if ((turnVelocityOrIndex == 0) && (targetAngleLength.length < (uint32_t)movementDefinition->headingErrorInterpolationDistanceQ12C0))
    {
ArmyMovementBanking_HoldCurrentHeadingAndResetTurnVelocity:
      (modelRuntime->movementControl).turnVelocityAngle16 = 0;
      newHeading = (modelRuntime->rootModelNode->modelPayload).worldRotationAngle2;
    }
    else {
      currentHeading = (rootNode->modelPayload).worldRotationAngle2;
      newHeading = desiredHeading - currentHeading & 0xffff;
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
          newHeading = desiredHeading;
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
    rootNode = modelRuntime->rootModelNode;
    if ((newHeading & 0xffff) != (rootNode->modelPayload).worldRotationAngle2) {
      (rootNode->modelPayload).worldRotationAngle2 = newHeading & 0xffff;
      rootNode->runtimeFlags = rootNode->runtimeFlags | 1;
    }
    ArmyRuntime_UpdateActivationMetricAndPlayStartSound
              (worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
    /* slide straight towards the route point (desiredHeading, not the model heading) */
    primaryDelta = (modelRuntime->movementControl).movementAdvancePerTickQ12 * g_InGameSimulationStepTicks
    ;
    if (primaryDelta < (int)targetAngleLength.length >> 1) {
      nextPosition = FixedTrig_ProjectPlanarPointRegs
                         (primaryDelta,desiredHeading,(rootNode->worldTransform).translation.y,
                          (rootNode->worldTransform).translation.x);
    }
    else {
      armyOrWaypointY = modelRuntime->ownerArmyRuntime;
      waypointResult = ArmyRuntime_UpdateMovementAndWaypoints(worldRuntime,(ArmyMovementRuntime *)armyOrWaypointY);
      nextPosition = THANDOR_PART(uint64_t, waypointResult, 0);
      ownerMovementFlags = &armyOrWaypointY->movementStateFlags;
      *ownerMovementFlags = *ownerMovementFlags | ARMY_MOVEMENT_ROUTE_POINT_REACHED;
    }
    placementContactKind = movementDefinition->placementContactKindIndex278;
    rootNode = modelRuntime->rootModelNode;
    heightOffsetQ12 = movementDefinition->placementHeightOffsetQ12;
    dispatchWorldRuntime = worldRuntime;
    blockingCollision = ArmyCollision_FindBlockingRuntimeForCurrentUnit
                       ((Q12)(nextPosition >> 32),(Q12)nextPosition,
                        (RuntimeCollisionQueryViewF4 *)modelRuntime,worldRuntime);
    if (blockingCollision.blocked) {
      secondChildOrRootNode = modelRuntime->rootModelNode;
      ArmyRuntime_HandleCollisionPartner
                ((ArmyRuntimeSlot *)modelRuntime,(secondChildOrRootNode->worldTransform).translation.y,
                 (secondChildOrRootNode->worldTransform).translation.x,(ArmyRuntimeSlot *)blockingCollision.blockingArmy,
                 worldRuntime);
      (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
      THANDOR_PART(uint32_t, nextPosition, 0) = (secondChildOrRootNode->worldTransform).translation.x;
      THANDOR_PART(uint32_t, nextPosition, 4) = (secondChildOrRootNode->worldTransform).translation.y;
      ownerMovementFlags = &modelRuntime->ownerArmyRuntime->movementStateFlags;
      *ownerMovementFlags = *ownerMovementFlags | ARMY_MOVEMENT_ROUTE_POINT_REACHED;
    }
    armyOrWaypointY = modelRuntime->ownerArmyRuntime;
    g_ArmyPlacementContactKindDispatchTable.callbacks[placementContactKind]
              (heightOffsetQ12,(Q12)(nextPosition >> 32),(Q12)nextPosition,rootNode,dispatchWorldRuntime);
    rootNode = modelRuntime->rootModelNode;
    /* recoil after a shot, as in ArmyRuntimeClass_UpdateGroundMovementVariantA */
    primaryDelta = armyOrWaypointY->actionVector1Q12 - 1;
    if (primaryDelta < 0) goto ArmyMovementBanking_FinalizeBankingEffectsAndTransforms;
    primaryDelta = primaryDelta * armyOrWaypointY->actionVector2Q12;
    armyOrWaypointY->actionVector1Q12 = armyOrWaypointY->actionVector1Q12 - 1;
  }
  else {
ArmyMovementBanking_ProcessStoppedMovementPlacementAndDamageState:
    (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
    (modelRuntime->movementControl).turnVelocityAngle16 = 0;
    armyOrWaypointY = modelRuntime->ownerArmyRuntime;
    rootNode = modelRuntime->rootModelNode;
    primaryDelta = armyOrWaypointY->actionVector1Q12 - 1;
    if (primaryDelta < 0) {
      (*g_ArmyPlacementContactKindDispatchTable.callbacks
        [modelRuntime->modelDefinition->placementContactKindIndex278])
                (modelRuntime->modelDefinition->placementHeightOffsetQ12,
                 (rootNode->worldTransform).translation.y,
                 (rootNode->worldTransform).translation.x,rootNode,worldRuntime);
      goto ArmyMovementBanking_FinalizeBankingEffectsAndTransforms;
    }
    primaryDelta = primaryDelta * armyOrWaypointY->actionVector2Q12;
    armyOrWaypointY->actionVector1Q12 = armyOrWaypointY->actionVector1Q12 - 1;
    (*g_ArmyPlacementContactKindDispatchTable.callbacks
      [modelRuntime->modelDefinition->placementContactKindIndex278])
              (modelRuntime->modelDefinition->placementHeightOffsetQ12,
               (rootNode->worldTransform).translation.y,(rootNode->worldTransform).translation.x
               ,rootNode,worldRuntime);
  }
  composedAngles = FixedTransform_ComposeEulerAnglesRegs
                     (0,0x4000 - primaryDelta,
                      (modelRuntime->ownerArmyRuntime->actionVector0Q12 + 0x8000) -
                      (rootNode->modelPayload).worldRotationAngle2 & 0xffff,
                      (rootNode->modelPayload).worldRotationAngle2,
                      (rootNode->modelPayload).worldRotationAngle1,
                      (rootNode->modelPayload).worldRotationAngle0);
  (rootNode->modelPayload).worldRotationAngle0 = composedAngles.angle0;
  (rootNode->modelPayload).worldRotationAngle1 = composedAngles.angle1;
  (rootNode->modelPayload).worldRotationAngle2 = composedAngles.angle2;
ArmyMovementBanking_FinalizeBankingEffectsAndTransforms:
  /* Banking: +0x60 (modelLinkOrState60) is the bank value, 0x4000 = level, +0x64 (classState64) the bank
     heading. The union member written as a ModelRuntimeSlot pointer below is just that integer. */
  movementDefinition = modelRuntime->modelDefinition;
  if ((modelRuntime->classLinkState).modelLinkOrState60.modelRuntime != (ModelRuntimeSlot *)0x4000)
  {
    composedAngles = FixedTransform_ComposeEulerAnglesRegs
                       ((rootNode->modelPayload).worldRotationAngle2,
                        (rootNode->modelPayload).worldRotationAngle1,
                        (rootNode->modelPayload).worldRotationAngle0,0,
                        (modelRuntime->classLinkState).modelLinkOrState60.classState,
                        (modelRuntime->classLinkState).classState64);
    (rootNode->modelPayload).worldRotationAngle0 = composedAngles.angle0;
    (rootNode->modelPayload).worldRotationAngle1 = composedAngles.angle1;
    (rootNode->modelPayload).worldRotationAngle2 = composedAngles.angle2;
  }
  turnVelocityOrIndex = targetAngleLength.angle - (modelRuntime->classLinkState).classState64 & 0xffff;
  if (((modelRuntime->movementControl).movementAdvancePerTickQ12 == 0) ||
     ((int)targetAngleLength.length <= movementDefinition->movementStepQ12PerTick0C * 0x20)) {
    /* stopped or within 32 steps of the route point: bank value += 0x40 (the member at +0x40 of the
       "pointer"), up to level */
    (modelRuntime->classLinkState).modelLinkOrState60.modelRuntime =
         (ModelRuntimeSlot *)
         &((modelRuntime->classLinkState).modelLinkOrState60.modelRuntime)->definitionValue84_40;
    if (0x4000 < (modelRuntime->classLinkState).modelLinkOrState60.signedScalarState) {
      (modelRuntime->classLinkState).modelLinkOrState60.classState = 0x4000;
    }
  }
  else {
    /* moving: turn the bank heading towards the travel direction by at most 0x400, bank value -= 0x100
       down to 0x3800 */
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
  if (((previousWorldX != (rootNode->worldTransform).translation.x) ||
      (previousWorldY != (rootNode->worldTransform).translation.y)) ||
     (previousRotationAngle != (rootNode->modelPayload).worldRotationAngle2)) {
    ownerMovementFlags = &modelRuntime->ownerArmyRuntime->movementStateFlags;
    *ownerMovementFlags = *ownerMovementFlags & ~ARMY_MOVEMENT_STATIONARY;
  }
  movementDefinition = modelRuntime->modelDefinition;
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  ModelNodeRuntime_RebuildTransformsFromRoot(rootNode);
  ModelNodeRuntime_UpdateDepthBinMasks(movementDefinition->placementRadiusOrClearanceDC,rootNode);
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
   Starts a locked, routed move to the target with exactly one queued waypoint, whose position is given
   separately by the two auxiliary values (x = auxiliaryValue0, y = auxiliaryValue1). The target becomes both
   the route end and the final movement target. Called directly from gameplay/army/runtime.c.
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
  movementRuntime->movementStateFlags =
       movementRuntime->movementStateFlags |
       (ARMY_MOVEMENT_ROUTED | 0x40 | ARMY_MOVEMENT_WAYPOINTS_QUEUED | ARMY_MOVEMENT_LOCKED | ARMY_MOVEMENT_ACTIVE);
  movementRuntime->movementStateFlags =
       movementRuntime->movementStateFlags &
       ~(ARMY_MOVEMENT_DIRECT | ARMY_MOVEMENT_TARGET_FOLLOWING | ARMY_MOVEMENT_ROUTE_POINT_REACHED);
  movementRuntime->commandModeFlags = movementRuntime->commandModeFlags & ~0x10u;
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
  movementRuntime->retryCountdown = ARMY_MOVEMENT_RETRY_TICKS;
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
   Puts the two feet of an articulated walker at rest beside its root: both feet (current position and step
   target) lateralOffsetQ12 to the left and right of the root at the terrain height under the root, all
   headings equal to the root heading, and both ground normals pointing straight up. Called by
   ArmyPlacementContact_InitializeArticulatedSuspension (gameplay/army/placement.c, placement contact kind 3)
   before ArmyArticulatedRuntime_UpdateSuspensionHierarchy.

   Walker state in ArmyArticulatedRuntimeSlotView (the generated field names do not fit):
     left foot  X/Y/Z: movementTarget0Q12, definitionClassValue80, definitionClassValue88
     right foot X/Y/Z: movementTarget1Q12, definitionClassValue84, runtimeState8C
     left foot step target X/Y/Z:  runtimeState90, runtimeState98, articulatedHeightOrStateA0
     right foot step target X/Y/Z: runtimeState94, articulatedCoordinateOrState9C, runtimeStateA4
     body heading at step start / end: classState60 / ownerValue64
     left foot ground normal now / target: ownerValue68 / fallbackWorldYQ12 (packed elevation << 16 | azimuth)
     right foot ground normal now / target: linkedArmyRuntimeOrSavedOffset / fallbackWorldXQ12
     left foot heading now / target:  linkedChildOverloadedState.primaryCoordinateCommandOrHistory /
                                      leftHeadingCommandOrSpawnValue
     right foot heading now / target: secondaryCoordinateCommandOrHistory / rightHeadingCommandOrSpawnValue
     step progress (Q12, 0..1.0): runtimeStateA8 (left), articulatedContact.terrainContactMode (right)
     step state: articulatedContact.fallbackPosition0Q12 (ARMY_ARTICULATED_STEP_*), step rate: fallbackPosition1Q12
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyArticulatedRuntime_InitializeTerrainContactGeometry
          (ModelRuntimeNode *modelNodeRuntime,WorldRuntimeContext *worldRuntime)

{
  ArmyRuntimeCoordinateCommandOrHistoryValue4 rootHeading;
  uint32_t terrainHeight;
  uint32_t footX;
  uint32_t footY;
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
  if (worldRuntime->fieldGrid != NULL) {
    terrainHeightResult = FieldGrid_InterpolateTerrainHeight(worldYQ12,worldXQ12,worldRuntime->fieldGrid);
    terrainHeight = terrainHeightResult.heightQ12;
  }
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
  lateralOffset = FixedMath_SinCosScaled
                    ((modelNodeRuntime->modelPayload).worldRotationAngle2 + 0x4000 & 0xffff,
                     (articulatedRuntime->articulatedContact).lateralOffsetQ12);
  lateralOffsetY = (int)(lateralOffset >> 32);
  /* left foot: heading + 90 degrees */
  footX = worldXQ12 + (int)lateralOffset;
  footY = worldYQ12 + lateralOffsetY;
  articulatedRuntime->movementTarget0Q12 = footX;
  articulatedRuntime->definitionClassValue80 = footY;
  articulatedRuntime->definitionClassValue88 = terrainHeight;
  articulatedRuntime->runtimeState90 = footX;
  articulatedRuntime->runtimeState98 = footY;
  articulatedRuntime->articulatedHeightOrStateA0 = terrainHeight;
  /* right foot */
  footX = worldXQ12 - (int)lateralOffset;
  footY = worldYQ12 - lateralOffsetY;
  articulatedRuntime->movementTarget1Q12 = footX;
  articulatedRuntime->definitionClassValue84 = footY;
  articulatedRuntime->runtimeState8C = terrainHeight;
  articulatedRuntime->runtimeState94 = footX;
  articulatedRuntime->articulatedCoordinateOrState9C = footY;
  articulatedRuntime->runtimeStateA4 = terrainHeight;
  rootHeading = THANDOR_BITCAST(AngleTurn32, ArmyRuntimeCoordinateCommandOrHistoryValue4, (modelNodeRuntime->modelPayload).worldRotationAngle2);
  articulatedRuntime->classState60 = THANDOR_BITCAST(ArmyRuntimeCoordinateCommandOrHistoryValue4, uint32_t, rootHeading);
  articulatedRuntime->ownerValue64 = THANDOR_BITCAST(ArmyRuntimeCoordinateCommandOrHistoryValue4, uint32_t, rootHeading);
  (articulatedRuntime->linkedChildOverloadedState).primaryCoordinateCommandOrHistory = rootHeading;
  (articulatedRuntime->linkedChildOverloadedState).leftHeadingCommandOrSpawnValue = rootHeading;
  (articulatedRuntime->linkedChildOverloadedState).secondaryCoordinateCommandOrHistory = rootHeading;
  (articulatedRuntime->linkedChildOverloadedState).rightHeadingCommandOrSpawnValue = rootHeading;
  /* ground normals: elevation 0x4000 (straight up) << 16 | azimuth 0 */
  articulatedRuntime->ownerValue68 = 0x40000000;
  articulatedRuntime->fallbackWorldYQ12 = 0x40000000;
  articulatedRuntime->linkedArmyRuntimeOrSavedOffset = (ArmyRuntimeSlot *)0x40000000;
  articulatedRuntime->fallbackWorldXQ12 = 0x40000000;
  return;
}


/* Address: 0x00527BC0.
   Class command that does nothing (RET 0x08): model classes without their own command handling. It fills the
   classCommand slots 0-3, 5-9, 12, 17-19 and 21 of g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes, which
   ArmyRuntime_DispatchClassCommand calls by model class.
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
   Standard ground movement (runtimeUpdate slot 1 of g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes, and
   the land case of slot 18 via ArmyRuntimeClass_UpdateSpecialBehaviorAndGroundMovement). Turns the model
   towards the current route point with accelerated turning, drives forward only while the heading error is
   within the definition's limit (the limit narrows near the target), stops at blocking armies, puts the
   model back onto the ground with its placement callback and rocks it back after a shot (action vector).
   Also applies water damage, drops a linked army that is out of reach and does the moved/turned bookkeeping.
*/

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_UpdateGroundMovementVariantA
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
  int32_t waterDelta;
  /* damage, route delta X, next turn velocity, travel distance, then the recoil tilt */
  int deltaXOrTilt;
  uint32_t facingAngle;
  uint32_t turnVelocityOrLimit;
  /* route delta Y, then the turn velocity limit */
  int deltaYOrTurnLimit;
  uint32_t desiredHeading;
  uint32_t headingDifference;
  ModelRuntimeNode *rootNode;
  bool withinLinkRadius;
  FixedLengthAngleEaxEdx8 angleAndLength;
  FixedPlanarPointEdxEax8 nextPosition;
  ArmyCollisionResult blockingCollision;
  MovementStepResult waypointResult;
  FixedEulerAnglesEaxEbxEdx12 composedAngles;
  Q12 heightOffsetQ12;
  WorldRuntimeContext *dispatchWorldRuntime;
  uint32_t targetDistance;
  ModelRuntimeNode *blockedRootNode;

  linkedOrOwnerArmy = (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime;
  rootNode = modelRuntime->rootModelNode;
  ownerMovementFlags = &modelRuntime->ownerArmyRuntime->movementStateFlags;
  *ownerMovementFlags = *ownerMovementFlags | ARMY_MOVEMENT_STATIONARY;
  /* Drop the linked runtime unless both have a clearance radius and this unit is still within it. */
  if ((linkedOrOwnerArmy != NULL) &&
      ((((((linkedOrOwnerArmy->modelRuntimeOrSavedOffset).modelRuntime)->classState).classStateDC == 0) ||
        (modelRuntime->modelDefinition->placementRadiusOrClearanceDC == 0)) ||
       (withinLinkRadius = ArmyCollision_TestPointWithinExpandedRuntimeRadius
                           (modelRuntime->modelDefinition->placementRadiusOrClearanceDC,
                            (rootNode->worldTransform).translation.y,
                            (rootNode->worldTransform).translation.x,linkedOrOwnerArmy), !withinLinkRadius))) {
    (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime = NULL;
  }
  previousRotationAngle = (rootNode->modelPayload).worldRotationAngle2;
  previousWorldX = (rootNode->worldTransform).translation.x;
  previousWorldY = (rootNode->worldTransform).translation.y;
  movementDefinition = modelRuntime->modelDefinition;
  waterDelta = FieldGrid_InterpolateWaterDelta(previousWorldY,previousWorldX,worldRuntime->fieldGrid);
  if ((movementDefinition->waterDamageThreshold198 < waterDelta) &&
     (deltaXOrTilt = waterDelta * movementDefinition->waterDamageMultiplier194 >> 7, -1 < deltaXOrTilt)) {
    ArmyRuntime_ApplyDamageAndPropagateToParent(deltaXOrTilt,(ArmyRuntimeSlot *)modelRuntime);
  }
  if (((modelRuntime->classState).classStateEC & ARMY_RUNTIME_FLAG_DESTROYED) == 0) {
    waypointResult = ArmyRuntime_UpdateMovementAndWaypoints
                       (worldRuntime,(ArmyMovementRuntime *)modelRuntime->ownerArmyRuntime);
    if (waypointResult.arrived) goto ArmyGroundMovementVariantA_StopMovementAndTurn;
    deltaYOrTurnLimit = waypointResult.worldYQ12 - (rootNode->worldTransform).translation.y;
    deltaXOrTilt = waypointResult.worldXQ12 - (rootNode->worldTransform).translation.x;
    if ((deltaXOrTilt == 0) && (deltaYOrTurnLimit == 0)) {
      angleAndLength = THANDOR_BITCAST(uint64_t, FixedLengthAngleEaxEdx8, ((uint64_t)(rootNode->modelPayload).worldRotationAngle2 << 32));
    }
    else {
      angleAndLength = FixedMath_Vector2AngleAndLengthRegs(deltaYOrTurnLimit,deltaXOrTilt);
    }
    /* turn by the current turn velocity, then accelerate it (clamped to the turn rate limit); a turn in the
       other direction first resets the velocity, a small remaining error snaps onto the desired heading */
    desiredHeading = angleAndLength.angle;
    movementDefinition = modelRuntime->modelDefinition;
    facingAngle = (rootNode->modelPayload).worldRotationAngle2;
    turnVelocityOrLimit = (modelRuntime->movementControl).turnVelocityAngle16;
    headingDifference = desiredHeading - facingAngle & 0xffff;
    if (headingDifference < 0x8000) {
      if ((int)turnVelocityOrLimit < 0) {
ArmyGroundMovementVariantA_ResetTurnVelocityForDirectionReversal:
        (modelRuntime->movementControl).turnVelocityAngle16 = 0;
      }
      else if (turnVelocityOrLimit < headingDifference) {
        facingAngle = facingAngle + turnVelocityOrLimit;
        deltaYOrTurnLimit = movementDefinition->turnRateLimitAnglePerTick10 * g_InGameSimulationStepTicks;
        deltaXOrTilt = turnVelocityOrLimit + movementDefinition->turnRateAccelerationAnglePerTick1C * g_InGameSimulationStepTicks;
        (modelRuntime->movementControl).turnVelocityAngle16 = deltaYOrTurnLimit;
        if (deltaXOrTilt < deltaYOrTurnLimit) {
          (modelRuntime->movementControl).turnVelocityAngle16 = deltaXOrTilt;
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
      deltaYOrTurnLimit = -movementDefinition->turnRateLimitAnglePerTick10 * g_InGameSimulationStepTicks;
      deltaXOrTilt = turnVelocityOrLimit - movementDefinition->turnRateAccelerationAnglePerTick1C * g_InGameSimulationStepTicks;
      (modelRuntime->movementControl).turnVelocityAngle16 = deltaYOrTurnLimit;
      if (deltaYOrTurnLimit < deltaXOrTilt) {
        (modelRuntime->movementControl).turnVelocityAngle16 = deltaXOrTilt;
      }
    }
    rootNode = modelRuntime->rootModelNode;
    facingAngle = facingAngle & 0xffff;
    if (facingAngle != (rootNode->modelPayload).worldRotationAngle2) {
      (rootNode->modelPayload).worldRotationAngle2 = facingAngle;
      rootNode->runtimeFlags = rootNode->runtimeFlags | 1;
    }
    /* allowed heading error: the far limit, blended towards the near limit inside the interpolation distance */
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
    /* drive forward, or jump onto the route point once it is closer than twice this tick's travel */
    deltaXOrTilt = (modelRuntime->movementControl).movementAdvancePerTickQ12 * g_InGameSimulationStepTicks;
    if (deltaXOrTilt < (int)targetDistance >> 1) {
      nextPosition = FixedTrig_ProjectPlanarPointRegs
                         (deltaXOrTilt,(rootNode->modelPayload).worldRotationAngle2,
                          (rootNode->worldTransform).translation.y,
                          (rootNode->worldTransform).translation.x);
    }
    else {
      linkedOrOwnerArmy = modelRuntime->ownerArmyRuntime;
      waypointResult = ArmyRuntime_UpdateMovementAndWaypoints(worldRuntime,(ArmyMovementRuntime *)linkedOrOwnerArmy);
      nextPosition = THANDOR_PART(uint64_t, waypointResult, 0);
      ownerMovementFlags = &linkedOrOwnerArmy->movementStateFlags;
      *ownerMovementFlags = *ownerMovementFlags | ARMY_MOVEMENT_ROUTE_POINT_REACHED;
    }
    placementContactKind = movementDefinition->placementContactKindIndex278;
    rootNode = modelRuntime->rootModelNode;
    heightOffsetQ12 = movementDefinition->placementHeightOffsetQ12;
    dispatchWorldRuntime = worldRuntime;
    blockingCollision = ArmyCollision_FindBlockingRuntimeForCurrentUnit
                       ((Q12)(nextPosition >> 32),(Q12)nextPosition,
                        (RuntimeCollisionQueryViewF4 *)modelRuntime,worldRuntime);
    if (blockingCollision.blocked) {
      /* stay where we are and let the collision partner react */
      blockedRootNode = modelRuntime->rootModelNode;
      ArmyRuntime_HandleCollisionPartner
                ((ArmyRuntimeSlot *)modelRuntime,(blockedRootNode->worldTransform).translation.y,
                 (blockedRootNode->worldTransform).translation.x,(ArmyRuntimeSlot *)blockingCollision.blockingArmy,
                 worldRuntime);
      (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
      THANDOR_PART(uint32_t, nextPosition, 0) = (blockedRootNode->worldTransform).translation.x;
      THANDOR_PART(uint32_t, nextPosition, 4) = (blockedRootNode->worldTransform).translation.y;
      ownerMovementFlags = &modelRuntime->ownerArmyRuntime->movementStateFlags;
      *ownerMovementFlags = *ownerMovementFlags | ARMY_MOVEMENT_ROUTE_POINT_REACHED;
    }
    linkedOrOwnerArmy = modelRuntime->ownerArmyRuntime;
    g_ArmyPlacementContactKindDispatchTable.callbacks[placementContactKind]
              (heightOffsetQ12,(Q12)(nextPosition >> 32),(Q12)nextPosition,rootNode,dispatchWorldRuntime);
    rootNode = modelRuntime->rootModelNode;
    /* recoil after a shot: actionVector1Q12 counts the remaining ticks, actionVector2Q12 is the tilt per tick */
    deltaXOrTilt = linkedOrOwnerArmy->actionVector1Q12 - 1;
    if (deltaXOrTilt < 0) goto ArmyGroundMovementVariantA_FinalizeEffectsAnimationAndTransforms;
    deltaXOrTilt = deltaXOrTilt * linkedOrOwnerArmy->actionVector2Q12;
    linkedOrOwnerArmy->actionVector1Q12 = linkedOrOwnerArmy->actionVector1Q12 - 1;
  }
  else {
ArmyGroundMovementVariantA_StopMovementAndTurn:
    (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
    (modelRuntime->movementControl).turnVelocityAngle16 = 0;
ArmyGroundMovementVariantA_ProcessStationaryPlacementAndDamageState:
    linkedOrOwnerArmy = modelRuntime->ownerArmyRuntime;
    rootNode = modelRuntime->rootModelNode;
    deltaXOrTilt = linkedOrOwnerArmy->actionVector1Q12 - 1;
    if (deltaXOrTilt < 0) {
      if ((worldRuntime->fieldGrid->runtimeStateFlags & 1) != 0) {
        (*g_ArmyPlacementContactKindDispatchTable.callbacks
          [modelRuntime->modelDefinition->placementContactKindIndex278])
                  (modelRuntime->modelDefinition->placementHeightOffsetQ12,
                   (rootNode->worldTransform).translation.y,
                   (rootNode->worldTransform).translation.x,rootNode,worldRuntime);
      }
      goto ArmyGroundMovementVariantA_FinalizeEffectsAnimationAndTransforms;
    }
    deltaXOrTilt = deltaXOrTilt * linkedOrOwnerArmy->actionVector2Q12;
    linkedOrOwnerArmy->actionVector1Q12 = linkedOrOwnerArmy->actionVector1Q12 - 1;
    (*g_ArmyPlacementContactKindDispatchTable.callbacks
      [modelRuntime->modelDefinition->placementContactKindIndex278])
              (modelRuntime->modelDefinition->placementHeightOffsetQ12,
               (rootNode->worldTransform).translation.y,(rootNode->worldTransform).translation.x
               ,rootNode,worldRuntime);
  }
  /* tilt the model away from the shot direction (actionVector0Q12 + 180 degrees) */
  composedAngles = FixedTransform_ComposeEulerAnglesRegs
                     (0,0x4000 - deltaXOrTilt,
                      (modelRuntime->ownerArmyRuntime->actionVector0Q12 + 0x8000) -
                      (rootNode->modelPayload).worldRotationAngle2 & 0xffff,
                      (rootNode->modelPayload).worldRotationAngle2,
                      (rootNode->modelPayload).worldRotationAngle1,
                      (rootNode->modelPayload).worldRotationAngle0);
  (rootNode->modelPayload).worldRotationAngle0 = composedAngles.angle0;
  (rootNode->modelPayload).worldRotationAngle1 = composedAngles.angle1;
  (rootNode->modelPayload).worldRotationAngle2 = composedAngles.angle2;
ArmyGroundMovementVariantA_FinalizeEffectsAnimationAndTransforms:
  if (((previousWorldX != (rootNode->worldTransform).translation.x) ||
      (previousWorldY != (rootNode->worldTransform).translation.y)) ||
     (previousRotationAngle != (rootNode->modelPayload).worldRotationAngle2)) {
    linkedOrOwnerArmy = modelRuntime->ownerArmyRuntime;
    ArmyRuntime_UpdateTimedShotAndEffectEmitters
              (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
    ownerMovementFlags = &linkedOrOwnerArmy->movementStateFlags;
    *ownerMovementFlags = *ownerMovementFlags & ~ARMY_MOVEMENT_STATIONARY;
  }
  movementDefinition = modelRuntime->modelDefinition;
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  ModelNodeRuntime_RebuildTransformsFromRoot(rootNode);
  ModelNodeRuntime_UpdateDepthBinMasks(movementDefinition->placementRadiusOrClearanceDC,rootNode);
  return;
}


/* Address: 0x00522090.
   Plans a walking step of the left foot towards heading headingAngle16 (routeDistanceQ12 = distance to the
   route point). The foot target is one stride (definition +0xC0) past the spot beside the right foot, or on
   the route point itself when that is closer, offset to the left side. When the target is blocked by an army
   (whose owner is notified) or has no ground sample, the foot is set down right beside the right foot instead,
   the route point is advanced, and the step is marked obstructed (a second obstruction in a row cancels it).
   Finally the step rate is derived from the foot's 3D travel. Called by ArmyRuntimeClass_UpdateArticulatedMovement;
   the walker state layout is described at ArmyArticulatedRuntime_InitializeTerrainContactGeometry.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyArticulatedRuntime_UpdateLeftTerrainContact
          (AngleTurn32 headingAngle16,Q12 routeDistanceQ12,
          ArmyArticulatedRuntimeSlotView *armyRuntime,WorldRuntimeContext *worldRuntime)

{
  GameEntityCommandFlags *entityCommandFlags;
  Q12 *contactStateFlags;
  void *movementDefinition;
  FieldGridAsset *activeFieldGrid;
  /* reach from the root, the target X, the target Y, then the foot travel length */
  uint32_t footXOrLength;
  /* stride reach, then the side angle again */
  uint32_t reachOrSideAngle;
  /* the point ahead X, then the target height, then the travel length plus lift */
  int aheadXOrFootZ;
  uint32_t sideAngle;
  /* the point ahead Y, then the stride length */
  int aheadYOrStride;
  uint32_t footY;
  FixedSinCosEdxEax8 lateralSinCos;
  FixedSinCosEdxEax8 headingSinCos;
  ArmyCollisionResult blockingCollision;
  MovementStepResult waypointResult;
  HeightNormalSampleResult terrainSample;
  UQ12 footRadius;
  ModelRuntimeNode *rootNode;

  rootNode = armyRuntime->modelNodeRuntime;
  movementDefinition = armyRuntime->definitionOrAsset;
  armyRuntime->ownerValue64 = headingAngle16;
  (armyRuntime->linkedChildOverloadedState).leftHeadingCommandOrSpawnValue.headingOrTurnValue =
       headingAngle16;
  sideAngle = headingAngle16 + 0x4000 & 0xffff;
  lateralSinCos = FixedMath_SinCosScaled(sideAngle,(armyRuntime->articulatedContact).lateralOffsetQ12);
  footRadius = (armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.contactRadiusQ12;
  footXOrLength = FixedMath_Length2(((int)(lateralSinCos >> 32) + armyRuntime->articulatedCoordinateOrState9C) -
                            (rootNode->worldTransform).translation.y,
                            ((int)lateralSinCos + armyRuntime->runtimeState94) -
                            (rootNode->worldTransform).translation.x);
  /* definition +0xC0: stride length */
  reachOrSideAngle = footXOrLength + *(int *)((int)movementDefinition + 0xc0);
  if (reachOrSideAngle < (uint32_t)routeDistanceQ12) {
    headingSinCos = FixedMath_SinCosScaled(headingAngle16,reachOrSideAngle);
    aheadXOrFootZ = (int)headingSinCos + (rootNode->worldTransform).translation.x;
    aheadYOrStride = (int)(headingSinCos >> 32) + (rootNode->worldTransform).translation.y;
  }
  else {
    waypointResult = ArmyRuntime_UpdateMovementAndWaypoints
                       (worldRuntime,(ArmyMovementRuntime *)armyRuntime->linkedEntityRuntime);
    aheadYOrStride = waypointResult.worldYQ12;
    aheadXOrFootZ = waypointResult.worldXQ12;
  }
  footXOrLength = aheadXOrFootZ + (int)lateralSinCos;
  footY = aheadYOrStride + (int)(lateralSinCos >> 32);
  activeFieldGrid = worldRuntime->fieldGrid;
  armyRuntime->runtimeState90 = footXOrLength;
  armyRuntime->runtimeState98 = footY;
  /* the ground is sampled footRadius further out to the side */
  lateralSinCos = FixedMath_SinCosScaled(sideAngle,footRadius);
  if (activeFieldGrid == NULL) {
    return;
  }
  terrainSample = FieldGrid_InterpolateTerrainHeightAndNormal
                     ((int)(lateralSinCos >> 32) + footY,(int)lateralSinCos + footXOrLength,activeFieldGrid);
  if (!terrainSample.failed) {
    armyRuntime->fallbackWorldYQ12 = terrainSample.packedNormalAngles;
    armyRuntime->articulatedHeightOrStateA0 = terrainSample.heightQ12;
    footXOrLength = armyRuntime->runtimeState98;
    aheadXOrFootZ = armyRuntime->articulatedHeightOrStateA0;
    blockingCollision = ArmyCollision_FindBlockingRuntimeForCurrentUnit
                       (footXOrLength,armyRuntime->runtimeState90,(RuntimeCollisionQueryViewF4 *)armyRuntime
                        ,worldRuntime);
    if (!blockingCollision.blocked) {
      contactStateFlags = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
      *contactStateFlags = *contactStateFlags & ~ARMY_ARTICULATED_STEP_OBSTRUCTED;
      goto ArmyArticulatedRuntime_UpdateLeftTerrainContact_ComputeStepFromContact;
    }
    ArmyRuntime_HandleCollisionPartner
              ((ArmyRuntimeSlot *)armyRuntime,
               (armyRuntime->modelNodeRuntime->worldTransform).translation.y,
               (armyRuntime->modelNodeRuntime->worldTransform).translation.x,
               (ArmyRuntimeSlot *)blockingCollision.blockingArmy,worldRuntime);
  }
  /* obstructed: put the left foot right beside the right foot */
  reachOrSideAngle = headingAngle16 + 0x4000 & 0xffff;
  lateralSinCos = FixedMath_SinCosScaled(reachOrSideAngle,(armyRuntime->articulatedContact).lateralOffsetQ12);
  footRadius = (armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.contactRadiusQ12;
  footXOrLength = armyRuntime->movementTarget1Q12 + (int)lateralSinCos * 2;
  footY = armyRuntime->definitionClassValue84 + (int)(lateralSinCos >> 32) * 2;
  activeFieldGrid = worldRuntime->fieldGrid;
  armyRuntime->runtimeState90 = footXOrLength;
  armyRuntime->runtimeState98 = footY;
  lateralSinCos = FixedMath_SinCosScaled(reachOrSideAngle,footRadius);
  terrainSample = FieldGrid_InterpolateTerrainHeightAndNormal
                     ((int)(lateralSinCos >> 32) + footY,(int)lateralSinCos + footXOrLength,activeFieldGrid);
  if (terrainSample.failed) {
    return;
  }
  armyRuntime->articulatedHeightOrStateA0 = terrainSample.heightQ12;
  armyRuntime->fallbackWorldYQ12 = terrainSample.packedNormalAngles;
  footXOrLength = armyRuntime->runtimeState98;
  aheadXOrFootZ = armyRuntime->articulatedHeightOrStateA0;
  /* the owner's common.commandFlags is its movementStateFlags */
  entityCommandFlags = &(armyRuntime->linkedEntityRuntime->common).commandFlags;
  *entityCommandFlags = *entityCommandFlags | ARMY_MOVEMENT_ROUTE_POINT_REACHED;
  if (((armyRuntime->articulatedContact).fallbackPosition0Q12 & ARMY_ARTICULATED_STEP_OBSTRUCTED) != 0) {
    contactStateFlags = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
    *contactStateFlags = *contactStateFlags & ~(ARMY_ARTICULATED_STEP_RIGHT | ARMY_ARTICULATED_STEP_LEFT);
  }
  contactStateFlags = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
  *contactStateFlags = *contactStateFlags | ARMY_ARTICULATED_STEP_OBSTRUCTED;
ArmyArticulatedRuntime_UpdateLeftTerrainContact_ComputeStepFromContact:
  /* step rate = (stride << 13) / (3D foot travel + 2 * lift height (definition +0xC4)), 0x2000 if zero */
  movementDefinition = armyRuntime->definitionOrAsset;
  footXOrLength = FixedMath_Length3(aheadXOrFootZ - armyRuntime->definitionClassValue88,
                            footXOrLength - armyRuntime->definitionClassValue80,
                            armyRuntime->runtimeState90 - armyRuntime->movementTarget0Q12);
  aheadXOrFootZ = footXOrLength + *(int *)((int)movementDefinition + 0xc4) * 2;
  aheadYOrStride = *(int *)((int)movementDefinition + 0xc0);
  (armyRuntime->articulatedContact).fallbackPosition1Q12 = 0x2000;
  if (aheadXOrFootZ != 0) {
    (armyRuntime->articulatedContact).fallbackPosition1Q12 =
         (Q12)((int64_t)(uint64_t)(uint32_t)(aheadYOrStride << 13) / (int64_t)aheadXOrFootZ);
  }
  return;
}


/* Address: 0x005222F0.
   Mirror of ArmyArticulatedRuntime_UpdateLeftTerrainContact for the right foot (side angle heading - 90
   degrees, placed relative to the left foot). Called by ArmyRuntimeClass_UpdateArticulatedMovement.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyArticulatedRuntime_UpdateRightTerrainContact
          (AngleTurn32 headingAngle16,Q12 routeDistanceQ12,
          ArmyArticulatedRuntimeSlotView *armyRuntime,WorldRuntimeContext *worldRuntime)

{
  GameEntityCommandFlags *entityCommandFlags;
  Q12 *contactStateFlags;
  void *movementDefinition;
  FieldGridAsset *activeFieldGrid;
  /* reach from the root, the target X, the target height, then the foot travel length */
  uint32_t footXOrLength;
  /* stride reach, then the side angle again */
  uint32_t reachOrSideAngle;
  /* the point ahead X, then the target Y, then the travel length plus lift */
  int aheadXOrFootY;
  uint32_t sideAngle;
  /* the point ahead Y, then the target Y, then the stride length */
  int aheadYOrStride;
  FixedSinCosEdxEax8 lateralSinCos;
  FixedSinCosEdxEax8 headingSinCos;
  ArmyCollisionResult blockingCollision;
  MovementStepResult waypointResult;
  HeightNormalSampleResult terrainSample;
  UQ12 footRadius;
  ModelRuntimeNode *rootNode;

  rootNode = armyRuntime->modelNodeRuntime;
  movementDefinition = armyRuntime->definitionOrAsset;
  armyRuntime->ownerValue64 = headingAngle16;
  (armyRuntime->linkedChildOverloadedState).rightHeadingCommandOrSpawnValue.headingOrTurnValue =
       headingAngle16;
  sideAngle = headingAngle16 - 0x4000 & 0xffff;
  lateralSinCos = FixedMath_SinCosScaled(sideAngle,(armyRuntime->articulatedContact).lateralOffsetQ12);
  footRadius = (armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.contactRadiusQ12;
  footXOrLength = FixedMath_Length2(((int)(lateralSinCos >> 32) + armyRuntime->runtimeState98) -
                            (rootNode->worldTransform).translation.y,
                            ((int)lateralSinCos + armyRuntime->runtimeState90) -
                            (rootNode->worldTransform).translation.x);
  /* definition +0xC0: stride length */
  reachOrSideAngle = footXOrLength + *(int *)((int)movementDefinition + 0xc0);
  if (reachOrSideAngle < (uint32_t)routeDistanceQ12) {
    headingSinCos = FixedMath_SinCosScaled(headingAngle16,reachOrSideAngle);
    aheadXOrFootY = (int)headingSinCos + (rootNode->worldTransform).translation.x;
    aheadYOrStride = (int)(headingSinCos >> 32) + (rootNode->worldTransform).translation.y;
  }
  else {
    waypointResult = ArmyRuntime_UpdateMovementAndWaypoints
                       (worldRuntime,(ArmyMovementRuntime *)armyRuntime->linkedEntityRuntime);
    aheadYOrStride = waypointResult.worldYQ12;
    aheadXOrFootY = waypointResult.worldXQ12;
  }
  footXOrLength = aheadXOrFootY + (int)lateralSinCos;
  aheadYOrStride = aheadYOrStride + (int)(lateralSinCos >> 32);
  activeFieldGrid = worldRuntime->fieldGrid;
  armyRuntime->runtimeState94 = footXOrLength;
  armyRuntime->articulatedCoordinateOrState9C = aheadYOrStride;
  lateralSinCos = FixedMath_SinCosScaled(sideAngle,footRadius);
  if (activeFieldGrid == NULL) {
    return;
  }
  terrainSample = FieldGrid_InterpolateTerrainHeightAndNormal
                     ((int)(lateralSinCos >> 32) + aheadYOrStride,(int)lateralSinCos + footXOrLength,activeFieldGrid);
  if (!terrainSample.failed) {
    armyRuntime->fallbackWorldXQ12 = terrainSample.packedNormalAngles;
    armyRuntime->runtimeStateA4 = terrainSample.heightQ12;
    aheadXOrFootY = armyRuntime->articulatedCoordinateOrState9C;
    footXOrLength = armyRuntime->runtimeStateA4;
    blockingCollision = ArmyCollision_FindBlockingRuntimeForCurrentUnit
                       (aheadXOrFootY,armyRuntime->runtimeState94,(RuntimeCollisionQueryViewF4 *)armyRuntime
                        ,worldRuntime);
    if (!blockingCollision.blocked) {
      contactStateFlags = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
      *contactStateFlags = *contactStateFlags & ~ARMY_ARTICULATED_STEP_OBSTRUCTED;
      goto ArmyArticulatedRuntime_UpdateRightTerrainContact_ComputeStepFromContact;
    }
    ArmyRuntime_HandleCollisionPartner
              ((ArmyRuntimeSlot *)armyRuntime,
               (armyRuntime->modelNodeRuntime->worldTransform).translation.y,
               (armyRuntime->modelNodeRuntime->worldTransform).translation.x,
               (ArmyRuntimeSlot *)blockingCollision.blockingArmy,worldRuntime);
  }
  /* obstructed: put the right foot right beside the left foot */
  reachOrSideAngle = headingAngle16 - 0x4000 & 0xffff;
  lateralSinCos = FixedMath_SinCosScaled(reachOrSideAngle,(armyRuntime->articulatedContact).lateralOffsetQ12);
  footRadius = (armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.contactRadiusQ12;
  footXOrLength = armyRuntime->movementTarget0Q12 + (int)lateralSinCos * 2;
  aheadXOrFootY = armyRuntime->definitionClassValue80 + (int)(lateralSinCos >> 32) * 2;
  activeFieldGrid = worldRuntime->fieldGrid;
  armyRuntime->runtimeState94 = footXOrLength;
  armyRuntime->articulatedCoordinateOrState9C = aheadXOrFootY;
  lateralSinCos = FixedMath_SinCosScaled(reachOrSideAngle,footRadius);
  terrainSample = FieldGrid_InterpolateTerrainHeightAndNormal
                     ((int)(lateralSinCos >> 32) + aheadXOrFootY,(int)lateralSinCos + footXOrLength,activeFieldGrid);
  if (terrainSample.failed) {
    return;
  }
  armyRuntime->runtimeStateA4 = terrainSample.heightQ12;
  armyRuntime->fallbackWorldXQ12 = terrainSample.packedNormalAngles;
  aheadXOrFootY = armyRuntime->articulatedCoordinateOrState9C;
  footXOrLength = armyRuntime->runtimeStateA4;
  /* the owner's common.commandFlags is its movementStateFlags */
  entityCommandFlags = &(armyRuntime->linkedEntityRuntime->common).commandFlags;
  *entityCommandFlags = *entityCommandFlags | ARMY_MOVEMENT_ROUTE_POINT_REACHED;
  if (((armyRuntime->articulatedContact).fallbackPosition0Q12 & ARMY_ARTICULATED_STEP_OBSTRUCTED) != 0) {
    contactStateFlags = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
    *contactStateFlags = *contactStateFlags & ~(ARMY_ARTICULATED_STEP_RIGHT | ARMY_ARTICULATED_STEP_LEFT);
  }
  contactStateFlags = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
  *contactStateFlags = *contactStateFlags | ARMY_ARTICULATED_STEP_OBSTRUCTED;
ArmyArticulatedRuntime_UpdateRightTerrainContact_ComputeStepFromContact:
  /* step rate as in ArmyArticulatedRuntime_UpdateLeftTerrainContact */
  movementDefinition = armyRuntime->definitionOrAsset;
  footXOrLength = FixedMath_Length3(footXOrLength - armyRuntime->runtimeState8C,
                            aheadXOrFootY - armyRuntime->definitionClassValue84,
                            armyRuntime->runtimeState94 - armyRuntime->movementTarget1Q12);
  aheadXOrFootY = footXOrLength + *(int *)((int)movementDefinition + 0xc4) * 2;
  aheadYOrStride = *(int *)((int)movementDefinition + 0xc0);
  (armyRuntime->articulatedContact).fallbackPosition1Q12 = 0x2000;
  if (aheadXOrFootY != 0) {
    (armyRuntime->articulatedContact).fallbackPosition1Q12 =
         (Q12)((int64_t)(uint64_t)(uint32_t)(aheadYOrStride << 13) / (int64_t)aheadXOrFootY);
  }
  return;
}


/* Address: 0x005254F0.
   Ground movement without water damage and without notifying the blocking army on a collision (the unit just
   stops); otherwise identical to ArmyRuntimeClass_UpdateGroundMovementVariantA. Used by runtimeUpdate slot 19
   of g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes and, for units on the water surface, by slot 18
   (ArmyRuntimeClass_UpdateSpecialBehaviorAndGroundMovement).
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
  /* route delta X, next turn velocity, travel distance, then the recoil tilt */
  int deltaXOrTilt;
  uint32_t facingAngle;
  uint32_t turnVelocityOrLimit;
  /* route delta Y, then the turn velocity limit */
  int deltaYOrTurnLimit;
  uint32_t desiredHeading;
  uint32_t headingDifference;
  ModelRuntimeNode *rootNode;
  bool withinLinkRadius;
  FixedLengthAngleEaxEdx8 angleAndLength;
  FixedPlanarPointEdxEax8 nextPosition;
  ArmyCollisionResult blockingCollision;
  MovementStepResult waypointResult;
  FixedEulerAnglesEaxEbxEdx12 composedAngles;
  Q12 heightOffsetQ12;
  WorldRuntimeContext *dispatchWorldRuntime;
  uint32_t targetDistance;
  ModelRuntimeNode *blockedRootNode;

  linkedOrOwnerArmy = (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime;
  rootNode = modelRuntime->rootModelNode;
  ownerMovementFlags = &modelRuntime->ownerArmyRuntime->movementStateFlags;
  *ownerMovementFlags = *ownerMovementFlags | ARMY_MOVEMENT_STATIONARY;
  /* Drop the linked runtime unless both have a clearance radius and this unit is still within it. */
  if ((linkedOrOwnerArmy != NULL) &&
      ((((((linkedOrOwnerArmy->modelRuntimeOrSavedOffset).modelRuntime)->classState).classStateDC == 0) ||
        (modelRuntime->modelDefinition->placementRadiusOrClearanceDC == 0)) ||
       (withinLinkRadius = ArmyCollision_TestPointWithinExpandedRuntimeRadius
                           (modelRuntime->modelDefinition->placementRadiusOrClearanceDC,
                            (rootNode->worldTransform).translation.y,
                            (rootNode->worldTransform).translation.x,linkedOrOwnerArmy), !withinLinkRadius))) {
    (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime = NULL;
  }
  previousRotationAngle = (rootNode->modelPayload).worldRotationAngle2;
  previousWorldX = (rootNode->worldTransform).translation.x;
  previousWorldY = (rootNode->worldTransform).translation.y;
  if (((modelRuntime->classState).classStateEC & ARMY_RUNTIME_FLAG_DESTROYED) == 0) {
    waypointResult = ArmyRuntime_UpdateMovementAndWaypoints
                       (worldRuntime,(ArmyMovementRuntime *)modelRuntime->ownerArmyRuntime);
    if (waypointResult.arrived) goto ArmyGroundMovementVariantB_StopMovementAndTurn;
    deltaYOrTurnLimit = waypointResult.worldYQ12 - (rootNode->worldTransform).translation.y;
    deltaXOrTilt = waypointResult.worldXQ12 - (rootNode->worldTransform).translation.x;
    if ((deltaXOrTilt == 0) && (deltaYOrTurnLimit == 0)) {
      angleAndLength = THANDOR_BITCAST(uint64_t, FixedLengthAngleEaxEdx8, ((uint64_t)(rootNode->modelPayload).worldRotationAngle2 << 32));
    }
    else {
      angleAndLength = FixedMath_Vector2AngleAndLengthRegs(deltaYOrTurnLimit,deltaXOrTilt);
    }
    /* accelerated turning as in ArmyRuntimeClass_UpdateGroundMovementVariantA */
    desiredHeading = angleAndLength.angle;
    movementDefinition = modelRuntime->modelDefinition;
    facingAngle = (rootNode->modelPayload).worldRotationAngle2;
    turnVelocityOrLimit = (modelRuntime->movementControl).turnVelocityAngle16;
    headingDifference = desiredHeading - facingAngle & 0xffff;
    if (headingDifference < 0x8000) {
      if ((int)turnVelocityOrLimit < 0) {
ArmyGroundMovementVariantB_ResetTurnVelocityForDirectionReversal:
        (modelRuntime->movementControl).turnVelocityAngle16 = 0;
      }
      else if (turnVelocityOrLimit < headingDifference) {
        facingAngle = facingAngle + turnVelocityOrLimit;
        deltaYOrTurnLimit = movementDefinition->turnRateLimitAnglePerTick10 * g_InGameSimulationStepTicks;
        deltaXOrTilt = turnVelocityOrLimit + movementDefinition->turnRateAccelerationAnglePerTick1C * g_InGameSimulationStepTicks;
        (modelRuntime->movementControl).turnVelocityAngle16 = deltaYOrTurnLimit;
        if (deltaXOrTilt < deltaYOrTurnLimit) {
          (modelRuntime->movementControl).turnVelocityAngle16 = deltaXOrTilt;
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
      deltaYOrTurnLimit = -movementDefinition->turnRateLimitAnglePerTick10 * g_InGameSimulationStepTicks;
      deltaXOrTilt = turnVelocityOrLimit - movementDefinition->turnRateAccelerationAnglePerTick1C * g_InGameSimulationStepTicks;
      (modelRuntime->movementControl).turnVelocityAngle16 = deltaYOrTurnLimit;
      if (deltaYOrTurnLimit < deltaXOrTilt) {
        (modelRuntime->movementControl).turnVelocityAngle16 = deltaXOrTilt;
      }
    }
    rootNode = modelRuntime->rootModelNode;
    facingAngle = facingAngle & 0xffff;
    if (facingAngle != (rootNode->modelPayload).worldRotationAngle2) {
      (rootNode->modelPayload).worldRotationAngle2 = facingAngle;
      rootNode->runtimeFlags = rootNode->runtimeFlags | 1;
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
    deltaXOrTilt = (modelRuntime->movementControl).movementAdvancePerTickQ12 * g_InGameSimulationStepTicks;
    if (deltaXOrTilt < (int)targetDistance >> 1) {
      nextPosition = FixedTrig_ProjectPlanarPointRegs
                         (deltaXOrTilt,(rootNode->modelPayload).worldRotationAngle2,
                          (rootNode->worldTransform).translation.y,
                          (rootNode->worldTransform).translation.x);
    }
    else {
      linkedOrOwnerArmy = modelRuntime->ownerArmyRuntime;
      waypointResult = ArmyRuntime_UpdateMovementAndWaypoints(worldRuntime,(ArmyMovementRuntime *)linkedOrOwnerArmy);
      nextPosition = THANDOR_PART(uint64_t, waypointResult, 0);
      ownerMovementFlags = &linkedOrOwnerArmy->movementStateFlags;
      *ownerMovementFlags = *ownerMovementFlags | ARMY_MOVEMENT_ROUTE_POINT_REACHED;
    }
    placementContactKind = movementDefinition->placementContactKindIndex278;
    rootNode = modelRuntime->rootModelNode;
    heightOffsetQ12 = movementDefinition->placementHeightOffsetQ12;
    dispatchWorldRuntime = worldRuntime;
    blockingCollision = ArmyCollision_FindBlockingRuntimeForCurrentUnit
                       ((Q12)(nextPosition >> 32),(Q12)nextPosition,
                        (RuntimeCollisionQueryViewF4 *)modelRuntime,worldRuntime);
    if (blockingCollision.blocked) {
      blockedRootNode = modelRuntime->rootModelNode;
      (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
      THANDOR_PART(uint32_t, nextPosition, 0) = (blockedRootNode->worldTransform).translation.x;
      THANDOR_PART(uint32_t, nextPosition, 4) = (blockedRootNode->worldTransform).translation.y;
      ownerMovementFlags = &modelRuntime->ownerArmyRuntime->movementStateFlags;
      *ownerMovementFlags = *ownerMovementFlags | ARMY_MOVEMENT_ROUTE_POINT_REACHED;
    }
    linkedOrOwnerArmy = modelRuntime->ownerArmyRuntime;
    g_ArmyPlacementContactKindDispatchTable.callbacks[placementContactKind]
              (heightOffsetQ12,(Q12)(nextPosition >> 32),(Q12)nextPosition,rootNode,dispatchWorldRuntime);
    rootNode = modelRuntime->rootModelNode;
    /* recoil after a shot, as in variant A */
    deltaXOrTilt = linkedOrOwnerArmy->actionVector1Q12 - 1;
    if (deltaXOrTilt < 0) goto ArmyGroundMovementVariantB_FinalizeEffectsAnimationAndTransforms;
    deltaXOrTilt = deltaXOrTilt * linkedOrOwnerArmy->actionVector2Q12;
    linkedOrOwnerArmy->actionVector1Q12 = linkedOrOwnerArmy->actionVector1Q12 - 1;
  }
  else {
ArmyGroundMovementVariantB_StopMovementAndTurn:
    (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
    (modelRuntime->movementControl).turnVelocityAngle16 = 0;
ArmyGroundMovementVariantB_ProcessStationaryPlacementAndDamageState:
    linkedOrOwnerArmy = modelRuntime->ownerArmyRuntime;
    rootNode = modelRuntime->rootModelNode;
    deltaXOrTilt = linkedOrOwnerArmy->actionVector1Q12 - 1;
    if (deltaXOrTilt < 0) {
      if ((worldRuntime->fieldGrid->runtimeStateFlags & 1) != 0) {
        (*g_ArmyPlacementContactKindDispatchTable.callbacks
          [modelRuntime->modelDefinition->placementContactKindIndex278])
                  (modelRuntime->modelDefinition->placementHeightOffsetQ12,
                   (rootNode->worldTransform).translation.y,
                   (rootNode->worldTransform).translation.x,rootNode,worldRuntime);
      }
      goto ArmyGroundMovementVariantB_FinalizeEffectsAnimationAndTransforms;
    }
    deltaXOrTilt = deltaXOrTilt * linkedOrOwnerArmy->actionVector2Q12;
    linkedOrOwnerArmy->actionVector1Q12 = linkedOrOwnerArmy->actionVector1Q12 - 1;
    (*g_ArmyPlacementContactKindDispatchTable.callbacks
      [modelRuntime->modelDefinition->placementContactKindIndex278])
              (modelRuntime->modelDefinition->placementHeightOffsetQ12,
               (rootNode->worldTransform).translation.y,(rootNode->worldTransform).translation.x
               ,rootNode,worldRuntime);
  }
  composedAngles = FixedTransform_ComposeEulerAnglesRegs
                     (0,0x4000 - deltaXOrTilt,
                      (modelRuntime->ownerArmyRuntime->actionVector0Q12 + 0x8000) -
                      (rootNode->modelPayload).worldRotationAngle2 & 0xffff,
                      (rootNode->modelPayload).worldRotationAngle2,
                      (rootNode->modelPayload).worldRotationAngle1,
                      (rootNode->modelPayload).worldRotationAngle0);
  (rootNode->modelPayload).worldRotationAngle0 = composedAngles.angle0;
  (rootNode->modelPayload).worldRotationAngle1 = composedAngles.angle1;
  (rootNode->modelPayload).worldRotationAngle2 = composedAngles.angle2;
ArmyGroundMovementVariantB_FinalizeEffectsAnimationAndTransforms:
  if (((previousWorldX != (rootNode->worldTransform).translation.x) ||
      (previousWorldY != (rootNode->worldTransform).translation.y)) ||
     (previousRotationAngle != (rootNode->modelPayload).worldRotationAngle2)) {
    linkedOrOwnerArmy = modelRuntime->ownerArmyRuntime;
    ArmyRuntime_UpdateTimedShotAndEffectEmitters
              (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
    ownerMovementFlags = &linkedOrOwnerArmy->movementStateFlags;
    *ownerMovementFlags = *ownerMovementFlags & ~ARMY_MOVEMENT_STATIONARY;
  }
  movementDefinition = modelRuntime->modelDefinition;
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  ModelNodeRuntime_RebuildTransformsFromRoot(rootNode);
  ModelNodeRuntime_UpdateDepthBinMasks(movementDefinition->placementRadiusOrClearanceDC,rootNode);
  return;
}


/* Address: 0x0051CC60.
   Starts a target-following move (only when no move is active, the movement is not locked and the route-retry
   countdown has run out). The step away from the current final target is clamped to 2.0 world units, so a
   following army re-routes in short hops; the final target itself stays unchanged. Called by
   ArmyRuntimeCommand_UpdateTargetFollowingState.
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
  if (((movementRuntime->movementStateFlags & (ARMY_MOVEMENT_ACTIVE | ARMY_MOVEMENT_LOCKED)) == 0) &&
      (movementRuntime->retryCountdown == 0)) {
    offsetAngleLength = FixedMath_Vector2AngleAndLengthRegs
                      (targetWorldY - movementRuntime->movementTargetWorldYQ12,
                       targetWorldX - movementRuntime->movementTargetWorldXQ12);
    if (ARMY_MOVEMENT_FOLLOW_MAX_STEP_Q12 < (int)offsetAngleLength.length) {
      clampedOffset = FixedMath_SinCosScaled(offsetAngleLength.angle,ARMY_MOVEMENT_FOLLOW_MAX_STEP_Q12);
      targetWorldX = (int)clampedOffset + movementRuntime->movementTargetWorldXQ12;
      targetWorldY = (int)(clampedOffset >> 32) + movementRuntime->movementTargetWorldYQ12;
    }
    movementRuntime->movementStateFlags =
         movementRuntime->movementStateFlags &
         ~(ARMY_MOVEMENT_MIRROR_TARGET | 0x40 | ARMY_MOVEMENT_ROUTE_POINT_REACHED | ARMY_MOVEMENT_WAYPOINTS_QUEUED);
    movementRuntime->movementStateFlags =
         movementRuntime->movementStateFlags |
         (ARMY_MOVEMENT_DIRECT | ARMY_MOVEMENT_TARGET_FOLLOWING | ARMY_MOVEMENT_ACTIVE);
    resolvedDestination = EntityPathing_ResolveDestinationAndRebuildRoutes
                      (targetWorldY,targetWorldX,movementRuntime->entityRuntime,
                       &inGameRoot->worldRuntime0A30);
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
   Footfall of an articulated walker, called by ArmyRuntimeClass_UpdateArticulatedMovement when a foot has
   finished its step (legNode = root child 0 for the left leg, 1 for the right). Plays the definition's
   footstep sound at the walker unless its cell is masked, and spawns the footprint effect at the foot node
   (three levels below the leg): one effect on dry ground, another where the nearest water is above ground.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyArticulatedRuntime_UpdateContactChildAndEffects
          (ModelRuntimeNode *legNode,WorldRuntimeContext *worldRuntime,
          ArmyRuntimeSlot *armyRuntime)

{
  GraphicsFixedVec3 *worldPosition;
  /* the army's first field is its model definition; the generated ModelRuntimeSlot view is used for it
     (sound index at +0xCC, sound gain/maximum distance at +0x78/+0x7C) */
  ModelRuntimeSlot *definitionView;
  uint32_t soundIndex;
  DirectSoundVoiceSet **voiceSetRef;
  int32_t waterDelta;
  EffectDefinition *effectDefinition;
  bool cellMasked;
  ModelRuntimeNode *footNode;

  definitionView = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  soundIndex = *(uint32_t *)(definitionView->classState).reservedCC_CF;
  if ((((soundIndex != 0) && (soundIndex < worldRuntime->dwordArrayCount)) &&
      (worldRuntime->dwordArray != NULL)) &&
     (voiceSetRef = (DirectSoundVoiceSet **)worldRuntime->dwordArray[soundIndex],
     voiceSetRef != NULL)) {
    worldPosition = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
    cellMasked = TerrainGrid_TestProjectedCellMaskBits01
                      ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,
                       worldPosition->x,worldRuntime);
    if (!cellMasked) {
      SpatialSound_PlayPositionedOneShot
                ((definitionView->classLinkState).classState7C,(definitionView->classLinkState).classState78,
                 worldPosition,voiceSetRef);
    }
  }
  footNode = legNode->childNodes[0]->childNodes[0]->childNodes[0];
  waterDelta = FieldGrid_GetNearestWaterDelta
                    ((footNode->worldTransform).translation.y,
                     (footNode->worldTransform).translation.x,worldRuntime->fieldGrid);
  if (waterDelta < 1) {
    effectDefinition = (EffectDefinition *)definitionView->attachments140[1].childLocalRotationAngle1;
  }
  else {
    effectDefinition = (EffectDefinition *)definitionView->definitionValueB4_58;
  }
  if (effectDefinition != NULL) {
    EffectRuntimePool_CreateInstanceFromDefinition
              (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference4, 0x0),
               (footNode->modelPayload).worldRotationAngle2,
               (footNode->modelPayload).worldRotationAngle1,
               (footNode->modelPayload).worldRotationAngle0,
               (footNode->worldTransform).translation.z,(footNode->worldTransform).translation.y
               ,(footNode->worldTransform).translation.x,effectDefinition,worldRuntime);
  }
  return;
}


/* Address: 0x005217A0.
   Poses the two-legged articulated walker from its foot state (layout at
   ArmyArticulatedRuntime_InitializeTerrainContactGeometry). Each foot node (four levels below root child 0 =
   left leg, child 1 = right leg) is interpolated from its position to its step target by the step progress,
   lifted in an arc by the lift height (definition +0xC4), and tilted by the blended ground normals. The body
   heading follows the step progress, the root sits midway between the feet at hip height (definition +0x54),
   the hip joints yaw towards the feet, and thigh and shin are bent by a two-bone triangle solution; finally
   the foot orientation is converted into the shin's local frame. Called by
   ArmyRuntimeClass_UpdateArticulatedMovement and ArmyPlacementContact_InitializeArticulatedSuspension
   (placement contact kind 3). g_ArmySuspension* are scratch vectors and matrices.
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
  void *movementDefinition;
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
  ArmyRuntimeSlot *walkerRuntime;
  ModelRuntimeNode *rightNode;
  ModelRuntimeNode *leftNode;
  
  articulatedRuntime = (ArmyArticulatedRuntimeSlotView *)(modelNodeRuntime->runtimePayload).armyRuntime;
  /* left foot: position = current + (target - current) * progress (Q12); the target height is raised by
     (1.0 - progress) * lift * 4, which gives the arc */
  leftNode = modelNodeRuntime->childNodes[0]->childNodes[0]->childNodes[0]->childNodes[0];
  leftBlendQ12 = articulatedRuntime->runtimeStateA8;
  blendProduct = (int64_t)(int)(articulatedRuntime->runtimeState90 - articulatedRuntime->movementTarget0Q12) *
           (int64_t)(int)leftBlendQ12;
  leftYOrSideLength = articulatedRuntime->runtimeState98;
  (leftNode->worldTransform).translation.x =
       ((int)((uint64_t)blendProduct >> 0x20) << 0x14 | (uint32_t)blendProduct >> 0xc) + articulatedRuntime->movementTarget0Q12
  ;
  blendProduct = (int64_t)(int)(leftYOrSideLength - articulatedRuntime->definitionClassValue80) * (int64_t)(int)leftBlendQ12;
  (leftNode->worldTransform).translation.y =
       ((int)((uint64_t)blendProduct >> 0x20) << 0x14 | (uint32_t)blendProduct >> 0xc) +
       articulatedRuntime->definitionClassValue80;
  blendProduct = (int64_t)
           (int)((((int)((0x1000 - leftBlendQ12) * *(int *)((int)articulatedRuntime->definitionOrAsset + 0xc4)) >> 10)
                 + articulatedRuntime->articulatedHeightOrStateA0) - articulatedRuntime->definitionClassValue88) *
           (int64_t)(int)leftBlendQ12;
  (leftNode->worldTransform).translation.z =
       ((int)((uint64_t)blendProduct >> 0x20) << 0x14 | (uint32_t)blendProduct >> 0xc) +
       articulatedRuntime->definitionClassValue88;
  /* left foot tilt: target normal * progress + current normal * (1.0 - progress) */
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
  /* the same for the right foot (progress in terrainContactMode) */
  rightNode = modelNodeRuntime->childNodes[1]->childNodes[0]->childNodes[0]->childNodes[0];
  rightBlendQ12 = (articulatedRuntime->articulatedContact).terrainContactMode;
  blendProduct = (int64_t)(int)(articulatedRuntime->runtimeState94 - articulatedRuntime->movementTarget1Q12) *
           (int64_t)(int)rightBlendQ12;
  inverseBlendOrRightHeading = articulatedRuntime->articulatedCoordinateOrState9C;
  (rightNode->worldTransform).translation.x =
       ((int)((uint64_t)blendProduct >> 0x20) << 0x14 | (uint32_t)blendProduct >> 0xc) + articulatedRuntime->movementTarget1Q12
  ;
  blendProduct = (int64_t)(int)(inverseBlendOrRightHeading - articulatedRuntime->definitionClassValue84) * (int64_t)(int)rightBlendQ12;
  (rightNode->worldTransform).translation.y =
       ((int)((uint64_t)blendProduct >> 0x20) << 0x14 | (uint32_t)blendProduct >> 0xc) +
       articulatedRuntime->definitionClassValue84;
  blendProduct = (int64_t)
           (int)((((int)((0x1000 - rightBlendQ12) * *(int *)((int)articulatedRuntime->definitionOrAsset + 0xc4)) >> 10)
                 + articulatedRuntime->runtimeStateA4) - articulatedRuntime->runtimeState8C) * (int64_t)(int)rightBlendQ12;
  (rightNode->worldTransform).translation.z =
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
  movementDefinition = articulatedRuntime->definitionOrAsset;
  /* the original swaps EBX with a stack slot here (XCHG [ESP],EBX at 0x00521A9D); no C equivalent */
  /* body heading = start heading + (end - start) * (left progress + right progress) */
  walkerRuntime = (modelNodeRuntime->runtimePayload).armyRuntime;
  (modelNodeRuntime->modelPayload).worldRotationAngle2 =
       (((int)((walkerRuntime->ownerValue64 - walkerRuntime->classState60) * 0x10000) >> 0x10) *
        (walkerRuntime->runtimeStateA8 + (walkerRuntime->articulatedContact).terrainContactMode) >> 0xc) +
       walkerRuntime->classState60 & 0xffff;
  /* raise both feet by the foot model's height offset; the root goes midway between them at hip height */
  leftXOrAngle = (leftNode->worldTransform).translation.x;
  leftYOrElevation = (leftNode->worldTransform).translation.y;
  heightOrRightTargetX = ((rightNode->modelPayload).modelResource)->placementHeightOffsetQ12;
  rightXOrTargetY = (rightNode->worldTransform).translation.x;
  rightNodeY = (rightNode->worldTransform).translation.y;
  nodeTranslationZ = &(leftNode->worldTransform).translation.z;
  *nodeTranslationZ = *nodeTranslationZ + heightOrRightTargetX;
  nodeTranslationZ = &(rightNode->worldTransform).translation.z;
  *nodeTranslationZ = *nodeTranslationZ + heightOrRightTargetX;
  heightOrRightTargetX = (leftNode->worldTransform).translation.z;
  rightNodeZ = (rightNode->worldTransform).translation.z;
  (modelNodeRuntime->worldTransform).translation.x = leftXOrAngle + rightXOrTargetY >> 1;
  (modelNodeRuntime->worldTransform).translation.y = leftYOrElevation + rightNodeY >> 1;
  (modelNodeRuntime->worldTransform).translation.z =
       (heightOrRightTargetX + rightNodeZ >> 1) + *(int *)((int)movementDefinition + 0x54);
  rightContactZ = (rightNode->worldTransform).translation.z;
  rightContactY = (rightNode->worldTransform).translation.y;
  rightContactX = (rightNode->worldTransform).translation.x;
  leftContactZ = (leftNode->worldTransform).translation.z;
  leftContactY = (leftNode->worldTransform).translation.y;
  leftContactX = (leftNode->worldTransform).translation.x;
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
  /* hip yaw: each hip turns towards its foot relative to the body (flipped by 180 degrees when the foot is
     behind), faded out when the foot is closer than 0x140 and zero below 0x40 */
  leftNode = modelNodeRuntime->childNodes[0];
  rightNode = modelNodeRuntime->childNodes[1];
  leftPlanarVector = FixedMath_Vector2AngleAndLengthRegs
                     (leftContactY - (leftNode->worldTransform).translation.y,
                      leftContactX - (leftNode->worldTransform).translation.x);
  leftYOrSideLength = leftPlanarVector.length;
  rightPlanarVector = FixedMath_Vector2AngleAndLengthRegs
                     (rightContactY - (rightNode->worldTransform).translation.y,
                      rightContactX - (rightNode->worldTransform).translation.x);
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
  (leftNode->modelPayload).localRotationAngle2 = leftYawOffset & 0xffff;
  (rightNode->modelPayload).localRotationAngle2 = rightLengthOrAngle;
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
  /* two-bone leg: aim thigh and shin at the ankle points (contactRadius beside each foot) */
  ModelNodeRuntime_RebuildTransformsFromRoot(leftNode);
  ModelNodeRuntime_RebuildTransformsFromRoot(rightNode);
  leftNode = leftNode->childNodes[0];
  rightNode = rightNode->childNodes[0];
  leftLegVector = FixedMath_VectorToAnglesAndLength3Regs
                     (leftContactZ - (leftNode->worldTransform).translation.z,
                      leftYOrElevation - (leftNode->worldTransform).translation.y,
                      leftXOrAngle - (leftNode->worldTransform).translation.x);
  rightLengthOrAngle = leftLegVector.azimuthAngle - (modelNodeRuntime->modelPayload).worldRotationAngle2 & 0xffff;
  leftXOrAngle = leftLegVector.elevationAngle + 0x4000;
  if ((0x3fff < rightLengthOrAngle) && (rightLengthOrAngle < 0xc001)) {
    leftXOrAngle = -leftXOrAngle;
  }
  rightLegVector = FixedMath_VectorToAnglesAndLength3Regs
                     (rightContactZ - (rightNode->worldTransform).translation.z,
                      rightXOrTargetY - (rightNode->worldTransform).translation.y,
                      heightOrRightTargetX - (rightNode->worldTransform).translation.x);
  rightLengthOrAngle = rightLegVector.azimuthAngle - (modelNodeRuntime->modelPayload).worldRotationAngle2 & 0xffff;
  leftYOrElevation = rightLegVector.elevationAngle + 0x4000;
  if ((0x3fff < rightLengthOrAngle) && (rightLengthOrAngle < 0xc001)) {
    leftYOrElevation = -leftYOrElevation;
  }
  leftNode = leftNode->childNodes[0];
  leftYOrSideLength = FixedMath_LengthVec3
                     ((GraphicsFixedVec3 *)&(leftNode->modelPayload).localTranslationXQ12);
  sideLength0Q12 =
       FixedMath_LengthVec3
                 ((GraphicsFixedVec3 *)
                  &(leftNode->childNodes[0]->modelPayload).localTranslationXQ12);
  leftNode = modelNodeRuntime->childNodes[0]->childNodes[0];
  rightNode = modelNodeRuntime->childNodes[1]->childNodes[0];
  jointAngles = FixedGeometry_SolveTriangleJointAnglesRegs(sideLength0Q12,leftYOrSideLength,leftLegVector.lengthQ12);
  leftXOrAngle = jointAngles.jointAngle0 - leftXOrAngle;
  if (leftXOrAngle < 0) {
    (leftNode->modelPayload).localRotationAngle0 = 0x8000;
    (leftNode->modelPayload).localRotationAngle1 = leftXOrAngle + 0x4000;
  }
  else {
    (leftNode->modelPayload).localRotationAngle0 = 0;
    (leftNode->modelPayload).localRotationAngle1 = 0x4000 - leftXOrAngle;
  }
  leftNode = leftNode->childNodes[0];
  (leftNode->modelPayload).localRotationAngle1 = 0x4000 - jointAngles.jointAngle1;
  jointAngles = FixedGeometry_SolveTriangleJointAnglesRegs(sideLength0Q12,leftYOrSideLength,rightLegVector.lengthQ12);
  leftYOrElevation = jointAngles.jointAngle0 - leftYOrElevation;
  if (leftYOrElevation < 0) {
    (rightNode->modelPayload).localRotationAngle0 = 0;
    (rightNode->modelPayload).localRotationAngle1 = leftYOrElevation + 0x4000;
  }
  else {
    (rightNode->modelPayload).localRotationAngle0 = 0x8000;
    (rightNode->modelPayload).localRotationAngle1 = 0x4000 - leftYOrElevation;
  }
  rightNode = rightNode->childNodes[0];
  (rightNode->modelPayload).localRotationAngle1 = 0x4000 - jointAngles.jointAngle1;
  leftNode = leftNode->childNodes[0];
  rightNode = rightNode->childNodes[0];
  (leftNode->modelPayload).localRotationAngle0 = 0;
  (leftNode->modelPayload).localRotationAngle1 = 0x4000;
  (leftNode->modelPayload).localRotationAngle2 = 0;
  (rightNode->modelPayload).localRotationAngle0 = 0;
  (rightNode->modelPayload).localRotationAngle1 = 0x4000;
  (rightNode->modelPayload).localRotationAngle2 = 0;
  modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
  /* foot orientation: world rotation (foot heading interpolated by progress, blended normal angles) times
     the inverse of the foot's current world rotation, stored as its local angles */
  FixedTransform_BuildRotationBasis
            ((GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchB,
             (leftNode->modelPayload).worldRotationAngle2,
             (leftNode->modelPayload).worldRotationAngle1,
             (leftNode->modelPayload).worldRotationAngle0);
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
  (leftNode->modelPayload).localRotationAngle0 = extractedAngles.ecxAngle;
  (leftNode->modelPayload).localRotationAngle1 = extractedAngles.edxAngle;
  (leftNode->modelPayload).localRotationAngle2 = extractedAngles.eaxAngle;
  FixedTransform_BuildRotationBasis
            ((GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchB,
             (rightNode->modelPayload).worldRotationAngle2,
             (rightNode->modelPayload).worldRotationAngle1,
             (rightNode->modelPayload).worldRotationAngle0);
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
  (rightNode->modelPayload).localRotationAngle0 = extractedAngles.ecxAngle;
  (rightNode->modelPayload).localRotationAngle1 = extractedAngles.edxAngle;
  (rightNode->modelPayload).localRotationAngle2 = extractedAngles.eaxAngle;
  modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
  return;
}


/* Address: 0x00522550.
   Plans a closing step of the left foot: the body heading target and the left foot heading become
   headingAngle16, and the left foot target is set right beside the right foot's target (2 * lateralOffsetQ12
   to the left) at the ground height there. The step rate uses the foot travel plus 4 * lift height. Called by
   ArmyRuntimeClass_UpdateArticulatedMovement for the second half of a turn and for closing steps; the walker
   state layout is described at ArmyArticulatedRuntime_InitializeTerrainContactGeometry.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyArticulatedRuntime_InitializeLeftTerrainContact
          (AngleTurn16Stored32 headingAngle16,ArmyArticulatedRuntimeSlotView *armyRuntime,
          WorldRuntimeContext *worldRuntime)

{
  int travelPlusLift;
  void *movementDefinition;
  int strideLength;
  uint32_t footTravel;
  uint32_t sideAngle;
  FixedSinCosEdxEax8 offsetSinCos;
  HeightNormalSampleResult terrainSample;

  armyRuntime->ownerValue64 = headingAngle16;
  (armyRuntime->linkedChildOverloadedState).leftHeadingCommandOrSpawnValue.signedValue =
       headingAngle16;
  sideAngle = headingAngle16 + 0x4000U & 0xffff;
  offsetSinCos = FixedMath_SinCosScaled(sideAngle,(armyRuntime->articulatedContact).lateralOffsetQ12);
  armyRuntime->runtimeState90 = (int)offsetSinCos * 2 + armyRuntime->runtimeState94;
  armyRuntime->runtimeState98 =
       (int)(offsetSinCos >> 32) * 2 + armyRuntime->articulatedCoordinateOrState9C;
  offsetSinCos = FixedMath_SinCosScaled
                    (sideAngle,(armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.
                           contactRadiusQ12);
  if (worldRuntime->fieldGrid != NULL) {
    terrainSample = FieldGrid_InterpolateTerrainHeightAndNormal
                      ((int)(offsetSinCos >> 32) + armyRuntime->runtimeState98,
                       (int)offsetSinCos + armyRuntime->runtimeState90,worldRuntime->fieldGrid);
    if (!terrainSample.failed) {
      armyRuntime->articulatedHeightOrStateA0 = terrainSample.heightQ12;
      armyRuntime->fallbackWorldYQ12 = terrainSample.packedNormalAngles;
      movementDefinition = armyRuntime->definitionOrAsset;
      footTravel = FixedMath_Length3(armyRuntime->articulatedHeightOrStateA0 -
                                armyRuntime->definitionClassValue88,
                                armyRuntime->runtimeState98 - armyRuntime->definitionClassValue80,
                                armyRuntime->runtimeState90 - armyRuntime->movementTarget0Q12);
      /* definition +0xC4: lift height, +0xC0: stride length */
      travelPlusLift = footTravel + *(int *)((int)movementDefinition + 0xc4) * 4;
      strideLength = *(int *)((int)movementDefinition + 0xc0);
      (armyRuntime->articulatedContact).fallbackPosition1Q12 = 0x2000;
      if (travelPlusLift != 0) {
        (armyRuntime->articulatedContact).fallbackPosition1Q12 =
             (Q12)((int64_t)(uint64_t)(uint32_t)(strideLength << 13) / (int64_t)travelPlusLift);
        (armyRuntime->articulatedContact).fallbackPosition0Q12 &= ~ARMY_ARTICULATED_STEP_OBSTRUCTED;
      }
    }
  }
  return;
}


/* Address: 0x00522660.
   Mirror of ArmyArticulatedRuntime_InitializeLeftTerrainContact for the right foot (set beside the left
   foot's target, side angle heading - 90 degrees). Called by ArmyRuntimeClass_UpdateArticulatedMovement.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyArticulatedRuntime_InitializeRightTerrainContact
          (AngleTurn16Stored32 headingAngle16,ArmyArticulatedRuntimeSlotView *armyRuntime,
          WorldRuntimeContext *worldRuntime)

{
  int travelPlusLift;
  void *movementDefinition;
  int strideLength;
  uint32_t footTravel;
  uint32_t sideAngle;
  FixedSinCosEdxEax8 offsetSinCos;
  HeightNormalSampleResult terrainSample;

  armyRuntime->ownerValue64 = headingAngle16;
  (armyRuntime->linkedChildOverloadedState).rightHeadingCommandOrSpawnValue.signedValue =
       headingAngle16;
  sideAngle = headingAngle16 - 0x4000U & 0xffff;
  offsetSinCos = FixedMath_SinCosScaled(sideAngle,(armyRuntime->articulatedContact).lateralOffsetQ12);
  armyRuntime->runtimeState94 = (int)offsetSinCos * 2 + armyRuntime->runtimeState90;
  armyRuntime->articulatedCoordinateOrState9C =
       (int)(offsetSinCos >> 32) * 2 + armyRuntime->runtimeState98;
  offsetSinCos = FixedMath_SinCosScaled
                    (sideAngle,(armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.
                           contactRadiusQ12);
  if (worldRuntime->fieldGrid != NULL) {
    terrainSample = FieldGrid_InterpolateTerrainHeightAndNormal
                      ((int)(offsetSinCos >> 32) + armyRuntime->articulatedCoordinateOrState9C,
                       (int)offsetSinCos + armyRuntime->runtimeState94,worldRuntime->fieldGrid);
    if (!terrainSample.failed) {
      armyRuntime->runtimeStateA4 = terrainSample.heightQ12;
      armyRuntime->fallbackWorldXQ12 = terrainSample.packedNormalAngles;
      movementDefinition = armyRuntime->definitionOrAsset;
      footTravel = FixedMath_Length3(armyRuntime->runtimeStateA4 - armyRuntime->runtimeState8C,
                                armyRuntime->articulatedCoordinateOrState9C -
                                armyRuntime->definitionClassValue84,
                                armyRuntime->runtimeState94 - armyRuntime->movementTarget1Q12);
      travelPlusLift = footTravel + *(int *)((int)movementDefinition + 0xc4) * 4;
      strideLength = *(int *)((int)movementDefinition + 0xc0);
      (armyRuntime->articulatedContact).fallbackPosition1Q12 = 0x2000;
      if (travelPlusLift != 0) {
        (armyRuntime->articulatedContact).fallbackPosition1Q12 =
             (Q12)((int64_t)(uint64_t)(uint32_t)(strideLength << 13) / (int64_t)travelPlusLift);
        (armyRuntime->articulatedContact).fallbackPosition0Q12 &= ~ARMY_ARTICULATED_STEP_OBSTRUCTED;
      }
    }
  }
  return;
}


/* Address: 0x00522770.
   Plans the first step of a turn on the spot. steeringAngle16 (0..0xFFFF, clamped to +-the maximum turn per
   step at definition +0xC8) selects the foot: up to 0x8000 the left foot (turning left), otherwise the right
   foot. The foot target is found by turning the root position about a pivot 1.5 * lateralOffsetQ12 behind it
   by the full steering angle and stepping out to that side; the body heading target becomes the start heading
   plus half the steering angle, and that half is also kept in the upper 16 bits of the step state for the
   closing step. The step rate uses the foot travel plus 4 * lift height. Called by
   ArmyRuntimeClass_UpdateArticulatedMovement; the walker state layout is described at
   ArmyArticulatedRuntime_InitializeTerrainContactGeometry.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyArticulatedRuntime_UpdateSelectedTerrainContact
          (AngleTurn32 steeringAngle16,ArmyArticulatedRuntimeSlotView *armyRuntime,
          WorldRuntimeContext *worldRuntime)

{
  Q12 *contactStateFlags;
  void *movementDefinition;
  /* the target Y, or (right foot) the target height */
  uint32_t footYOrZ;
  int64_t scaledOffsetProduct;
  /* pivot/target X, the target height (left) or Y (right), then the travel length plus lift */
  int pointXOrSegment;
  /* the target X, then the foot travel length */
  uint32_t footXOrLength;
  int signedSteeringAngle;
  uint32_t footHeading;
  uint32_t pivotDistance;
  /* pivot/target Y, then the stride length */
  int pointYOrStride;
  FixedSinCosEdxEax8 offsetSinCos;
  HeightNormalSampleResult terrainSample;
  FieldGridAsset *fieldGrid;
  ModelRuntimeNode *rootNode;

  movementDefinition = armyRuntime->definitionOrAsset;
  if (steeringAngle16 < 0x8001) {
    if (*(uint32_t *)((int)movementDefinition + 200) < steeringAngle16) {
      steeringAngle16 = *(AngleTurn32 *)((int)movementDefinition + 200);
    }
    /* pivot 1.5 * lateral offset behind the root */
    rootNode = armyRuntime->modelNodeRuntime;
    footHeading = armyRuntime->classState60 + 0x8000 & 0xffff;
    scaledOffsetProduct = (int64_t)(armyRuntime->articulatedContact).lateralOffsetQ12 * 0x1800;
    pivotDistance = (int)((uint64_t)scaledOffsetProduct >> 32) << 20 | (uint32_t)scaledOffsetProduct >> 12;
    offsetSinCos = FixedMath_SinCosScaled(footHeading,pivotDistance);
    pointXOrSegment = (int)offsetSinCos + (rootNode->worldTransform).translation.x;
    pointYOrStride = (int)(offsetSinCos >> 32) + (rootNode->worldTransform).translation.y;
    /* rotate forward from the pivot by the steering angle, then step out to the left */
    footHeading = (footHeading + steeringAngle16) - 0x8000 & 0xffff;
    (armyRuntime->linkedChildOverloadedState).leftHeadingCommandOrSpawnValue.headingOrTurnValue =
         footHeading;
    offsetSinCos = FixedMath_SinCosScaled(footHeading,pivotDistance);
    pointXOrSegment = pointXOrSegment + (int)offsetSinCos;
    pointYOrStride = pointYOrStride + (int)(offsetSinCos >> 32);
    footHeading = footHeading + 0x4000 & 0xffff;
    offsetSinCos = FixedMath_SinCosScaled(footHeading,(armyRuntime->articulatedContact).lateralOffsetQ12);
    armyRuntime->runtimeState90 = (int)offsetSinCos + pointXOrSegment;
    armyRuntime->runtimeState98 = (int)(offsetSinCos >> 32) + pointYOrStride;
    offsetSinCos = FixedMath_SinCosScaled
                       (footHeading,(armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.
                              contactRadiusQ12);
    fieldGrid = worldRuntime->fieldGrid;
    armyRuntime->ownerValue64 = ((int)steeringAngle16 >> 1) + armyRuntime->classState60 & 0xffff;
    if (fieldGrid != NULL) {
      terrainSample = FieldGrid_InterpolateTerrainHeightAndNormal
                         ((int)(offsetSinCos >> 32) + armyRuntime->runtimeState98,
                          (int)offsetSinCos + armyRuntime->runtimeState90,fieldGrid);
      if (!terrainSample.failed) {
        armyRuntime->articulatedHeightOrStateA0 = terrainSample.heightQ12;
        armyRuntime->fallbackWorldYQ12 = terrainSample.packedNormalAngles;
      }
    }
    /* upper 16 bits of the step state = steeringAngle16 / 2 (bit 15 gets its lowest bit) */
    contactStateFlags = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
    *contactStateFlags = *contactStateFlags & 0xffff;
    footXOrLength = armyRuntime->runtimeState90;
    footYOrZ = armyRuntime->runtimeState98;
    pointXOrSegment = armyRuntime->articulatedHeightOrStateA0;
    contactStateFlags = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
    *contactStateFlags = *contactStateFlags | steeringAngle16 << 15;
    movementDefinition = armyRuntime->definitionOrAsset;
    footXOrLength = FixedMath_Length3(pointXOrSegment - armyRuntime->definitionClassValue88,
                              footYOrZ - armyRuntime->definitionClassValue80,
                              footXOrLength - armyRuntime->movementTarget0Q12);
    pointXOrSegment = footXOrLength + *(int *)((int)movementDefinition + 0xc4) * 4;
    pointYOrStride = *(int *)((int)movementDefinition + 0xc0);
    (armyRuntime->articulatedContact).fallbackPosition1Q12 = 0x2000;
    if (pointXOrSegment != 0) {
      (armyRuntime->articulatedContact).fallbackPosition1Q12 =
           (Q12)((int64_t)(uint64_t)(uint32_t)(pointYOrStride << 13) / (int64_t)pointXOrSegment);
      return;
    }
  }
  else {
    /* right turn: the same with the right foot and a negative steering angle */
    footHeading = 0x10000 - *(int *)((int)movementDefinition + 200);
    if (steeringAngle16 < footHeading) {
      steeringAngle16 = footHeading;
    }
    rootNode = armyRuntime->modelNodeRuntime;
    signedSteeringAngle = steeringAngle16 - 0x10000;
    footHeading = armyRuntime->classState60 + 0x8000 & 0xffff;
    scaledOffsetProduct = (int64_t)(armyRuntime->articulatedContact).lateralOffsetQ12 * 0x1800;
    pivotDistance = (int)((uint64_t)scaledOffsetProduct >> 32) << 20 | (uint32_t)scaledOffsetProduct >> 12;
    offsetSinCos = FixedMath_SinCosScaled(footHeading,pivotDistance);
    pointXOrSegment = (int)offsetSinCos + (rootNode->worldTransform).translation.x;
    pointYOrStride = (int)(offsetSinCos >> 32) + (rootNode->worldTransform).translation.y;
    footHeading = (footHeading + signedSteeringAngle) - 0x8000 & 0xffff;
    (armyRuntime->linkedChildOverloadedState).rightHeadingCommandOrSpawnValue.headingOrTurnValue =
         footHeading;
    offsetSinCos = FixedMath_SinCosScaled(footHeading,pivotDistance);
    pointXOrSegment = pointXOrSegment + (int)offsetSinCos;
    pointYOrStride = pointYOrStride + (int)(offsetSinCos >> 32);
    footHeading = footHeading - 0x4000 & 0xffff;
    offsetSinCos = FixedMath_SinCosScaled(footHeading,(armyRuntime->articulatedContact).lateralOffsetQ12);
    armyRuntime->runtimeState94 = (int)offsetSinCos + pointXOrSegment;
    armyRuntime->articulatedCoordinateOrState9C = (int)(offsetSinCos >> 32) + pointYOrStride;
    offsetSinCos = FixedMath_SinCosScaled
                       (footHeading,(armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.
                              contactRadiusQ12);
    fieldGrid = worldRuntime->fieldGrid;
    armyRuntime->ownerValue64 = (signedSteeringAngle >> 1) + armyRuntime->classState60 & 0xffff;
    if (fieldGrid != NULL) {
      terrainSample = FieldGrid_InterpolateTerrainHeightAndNormal
                         ((int)(offsetSinCos >> 32) + armyRuntime->articulatedCoordinateOrState9C,
                          (int)offsetSinCos + armyRuntime->runtimeState94,fieldGrid);
      if (!terrainSample.failed) {
        armyRuntime->runtimeStateA4 = terrainSample.heightQ12;
        armyRuntime->fallbackWorldXQ12 = terrainSample.packedNormalAngles;
      }
    }
    contactStateFlags = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
    *contactStateFlags = *contactStateFlags & 0xffff;
    footXOrLength = armyRuntime->runtimeState94;
    pointXOrSegment = armyRuntime->articulatedCoordinateOrState9C;
    footYOrZ = armyRuntime->runtimeStateA4;
    contactStateFlags = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
    *contactStateFlags = *contactStateFlags | signedSteeringAngle * 0x8000;
    movementDefinition = armyRuntime->definitionOrAsset;
    footXOrLength = FixedMath_Length3(footYOrZ - armyRuntime->runtimeState8C,
                              pointXOrSegment - armyRuntime->definitionClassValue84,
                              footXOrLength - armyRuntime->movementTarget1Q12);
    pointXOrSegment = footXOrLength + *(int *)((int)movementDefinition + 0xc4) * 4;
    pointYOrStride = *(int *)((int)movementDefinition + 0xc0);
    (armyRuntime->articulatedContact).fallbackPosition1Q12 = 0x2000;
    if (pointXOrSegment != 0) {
      (armyRuntime->articulatedContact).fallbackPosition1Q12 =
           (Q12)((int64_t)(uint64_t)(uint32_t)(pointYOrStride << 13) / (int64_t)pointXOrSegment);
    }
  }
  return;
}


/* Address: 0x00523340.
   Called by a weapon that is aimed and ready to fire. When the shot to the target is blocked, the owning army
   (if this weapon is its primary weapon or it has none, and it is not already following) starts a
   target-following move towards the target, clamped for AI combat targets, and CF (true) tells the weapon not
   to fire. With a clear line of fire a running target-following move is stopped and false is returned. Called
   by the aim-and-fire class updates (runtime-update slots 7 and 8 here, and gameplay/army/combat.c).
*/
bool __thandor_cf_preserve_eax_ecx_edx
ArmyRuntimeCommand_UpdateTargetFollowingState
          (Q12 targetWorldZQ12,Q12 targetWorldYQ12,Q12 targetWorldXQ12,
          WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  GameEntityRuntime *ownerEntity;
  GameEntityRuntime *ownerRecord;
  bool lineOfFireBlocked;

  /* ownerEntity is the owning army; its common.commandFlags is ArmyRuntimeSlot.movementStateFlags and
     commandTarget.targetFlags is commandModeFlags */
  ownerEntity = armyRuntime->linkedEntityRuntime;
  lineOfFireBlocked = ArmyWeaponRuntime_TestTargetLineOfFire
                    (targetWorldZQ12,targetWorldYQ12,targetWorldXQ12,worldRuntime,armyRuntime);
  ownerRecord = (ownerEntity->common).ownership.definitionOrClassRecord;
  if (lineOfFireBlocked) {
    if (((armyRuntime == (ownerRecord->classPayload).impactOwnerLinks.primaryImpactArmyRuntime) ||
        ((ownerRecord->classPayload).impactOwnerLinks.primaryImpactArmyRuntime == NULL)
        ) && (((ownerEntity->common).commandFlags & ARMY_MOVEMENT_TARGET_FOLLOWING) == 0)) {
      if (((ownerEntity->common).commandTarget.targetFlags & ARMY_COMMAND_MODE_AI_COMBAT_TARGET) == 0) {
        ArmyRuntime_StartMoveCommandWithFallbackWaypoints
                  (targetWorldYQ12,targetWorldXQ12,(ArmyMovementRuntime *)ownerEntity);
      }
      else {
        ArmyRuntime_StartClampedMoveCommand
                  (targetWorldYQ12,targetWorldXQ12,(ArmyMovementRuntime *)ownerEntity);
      }
    }
    return true;
  }
  if (((ownerEntity->common).commandFlags & ARMY_MOVEMENT_TARGET_FOLLOWING) != 0) {
    if (((ownerEntity->common).commandTarget.targetFlags & ARMY_COMMAND_MODE_AI_COMBAT_TARGET) == 0) {
      ArmyRuntime_ResetMovementStatePreserveQueuedTarget((ArmyMovementRuntime *)ownerEntity);
    }
    else {
      ArmyRuntime_ResetMovementStateFromCurrentPosition((ArmyMovementRuntime *)ownerEntity);
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
     exceededDistance > ARMY_MOVEMENT_TARGET_RADIUS_Q12. That matches: EntityPathing_ResolveDestinationAndRebuildRoutes
     has a single exit with CLC (0x00534E61), so the CF left by the direct move is always clear (not arrived). */
  belowThreshold = exceededDistance < ARMY_MOVEMENT_TARGET_RADIUS_Q12;
  ArmyRuntime_StartDirectMoveCommand
            (movementRuntime->movementTargetWorldYQ12,movementRuntime->movementTargetWorldXQ12,
             movementRuntime);
  directMoveResult.worldYQ12 = distanceY;
  directMoveResult.worldXQ12 = distanceX;
  directMoveResult.arrived = belowThreshold;
  return directMoveResult;
}

