/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/move_orders.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/army/move_orders.h>
#include <thandor/thandor.h>

/* Module data. */

const ArmyCommandGeneration g_ArmyCommandGenerationStandard = 1024;

/* Implementation ownership: gameplay/army/move_orders. */

/* Makes targetRuntime the army's command target: a running movement (movement flag 0x20) is reset first, then
   the army starts a route to the target's current world position under the standard command generation.
   A null target clears the command instead.
*/
void ArmyRuntime_ResolveCommandTargetAndRoute(GameEntityRuntime *targetRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ArmyCommandGeneration standardGeneration;
  ModelRuntimeNode *targetModelNode;

  standardGeneration = g_ArmyCommandGenerationStandard;
  if ((armyRuntime->movementStateFlags & ARMY_MOVEMENT_TARGET_FOLLOWING) != 0) {
    if ((armyRuntime->commandModeFlags & ARMY_COMMAND_MODE_AI_COMBAT_TARGET) == 0) {
      ArmyRuntime_ResetMovementStatePreserveQueuedTarget((ArmyMovementRuntime *)armyRuntime);
    }
    else {
      ArmyRuntime_ResetMovementStateFromCurrentPosition((ArmyMovementRuntime *)armyRuntime);
    }
  }
  if (targetRuntime == nullptr) {
    armyRuntime->commandModeFlags = 0;
    armyRuntime->commandGeneration = 0;
  }
  else {
    armyRuntime->commandGeneration = standardGeneration;
    armyRuntime->commandModeFlags = ARMY_COMMAND_MODE_TARGET_ARMY;
    targetModelNode = (targetRuntime->common).ownership.modelNode;
    ArmyRuntime_StartMoveCommandWithFallbackWaypoints
              ((targetModelNode->worldTransform).translation.y,(targetModelNode->worldTransform).translation.x
               ,(ArmyMovementRuntime *)armyRuntime);
  }
  armyRuntime->commandTargetArmyRuntime = (ArmyRuntimeSlot *)targetRuntime;
  return;
}

/* Helper for ArmyRuntime_ResetMovementStateFromModel (no original address: the original walks the tree
   iteratively with an explicit stack). Clears the dismantling/destroyed state bits (0x218) of a model runtime
   and of all its children (the attached child model runtimes, null slots skipped). */
static void ArmyRuntime_ClearModelTreeFlags218(ModelRuntimeSlot *node)
{
  int childIndex;
  node->classState.stateFlags = node->classState.stateFlags &
       ~(ARMY_MODEL_STATE_DISMANTLED | ARMY_MODEL_STATE_DISMANTLING | ARMY_RUNTIME_FLAG_DESTROYED);
  for (childIndex = 0; childIndex < (int)node->attachmentCount; childIndex++) {
    ModelRuntimeSlot *child = node->attachments[childIndex].childModelRuntimeOrSavedOffset;
    if (child != nullptr) {
      ArmyRuntime_ClearModelTreeFlags218(child);
    }
  }
}

/* Stops the army where its model currently stands: clears the move flags, cancels an active target
   command (unless state field 0x100 is zero), and sets every move target to the current model position.
   If the attached model has class-state bit 0x10 and a non-zero health, state bits 0x218
   are cleared on its whole model tree.
*/
void ArmyRuntime_ResetMovementStateFromModel(ArmyRuntimeSlot *armyRuntime)

{
  GraphicsWorldCoordinateQ12 currentWorldX;
  GraphicsWorldCoordinateQ12 currentWorldY;
  ArmyCommandGeneration standardGeneration;
  ModelRuntimeSlot *attachedModelRuntime;
  Bool8 hasNoWeaponDamage;
  ModelRuntimeNode *modelNode;

  standardGeneration = g_ArmyCommandGenerationStandard;
  modelNode = armyRuntime->modelNodeRuntime;
  armyRuntime->movementStateFlags =
       armyRuntime->movementStateFlags &
       ~(ARMY_MOVEMENT_ACTIVE | ARMY_MOVEMENT_WAYPOINTS_QUEUED | ARMY_MOVEMENT_ROUTE_POINT_REACHED |
         ARMY_MOVEMENT_TARGET_FOLLOWING);
  hasNoWeaponDamage = ArmyRuntime_TestHasNoWeaponDamage(armyRuntime);
  if ((!hasNoWeaponDamage) &&
      ((armyRuntime->commandModeFlags & (ARMY_COMMAND_MODE_TARGET_ARMY | ARMY_COMMAND_MODE_TARGET_POSITION)) != 0)) {
    armyRuntime->commandModeFlags =
         armyRuntime->commandModeFlags & ~(ARMY_COMMAND_MODE_TARGET_ARMY | ARMY_COMMAND_MODE_TARGET_POSITION);
    armyRuntime->commandModeFlags = armyRuntime->commandModeFlags | ARMY_COMMAND_MODE_INTERRUPTED;
    armyRuntime->commandGeneration = standardGeneration;
    armyRuntime->commandTargetArmyRuntime = nullptr;
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
  if ((((attachedModelRuntime->classState).stateFlags & ARMY_MODEL_STATE_DISMANTLING) != 0) &&
      (attachedModelRuntime->health != 0)) {
    /* clear 0x218 on the whole model tree */
    ArmyRuntime_ClearModelTreeFlags218(attachedModelRuntime);
  }
  return;

}

/* Cancels an active target command (army or position): marks the command as interrupted, stamps the
   standard command generation and drops the target army. Also used by the 32-slot command reset traversal.
*/
void ArmyRuntimeCommand_InterruptActiveTargetAndStampGeneration(ArmyRuntimeSlot *armyRuntime)

{
  ArmyCommandGeneration commandGeneration;

  commandGeneration = g_ArmyCommandGenerationStandard;
  if ((armyRuntime->commandModeFlags & (ARMY_COMMAND_MODE_TARGET_ARMY | ARMY_COMMAND_MODE_TARGET_POSITION)) != 0) {
    armyRuntime->commandModeFlags =
         armyRuntime->commandModeFlags & ~(ARMY_COMMAND_MODE_TARGET_ARMY | ARMY_COMMAND_MODE_TARGET_POSITION);
    armyRuntime->commandModeFlags = armyRuntime->commandModeFlags | ARMY_COMMAND_MODE_INTERRUPTED;
    armyRuntime->commandGeneration = commandGeneration;
    armyRuntime->commandTargetArmyRuntime = nullptr;
  }
  return;
}

/* Starts a locked, routed move to the target with exactly one queued waypoint, whose position is given
   separately by the two auxiliary values (x = auxiliaryValue0, y = auxiliaryValue1). The target becomes both
   the route end and the final movement target. Called directly from gameplay/army/factory.cpp.
*/
void ArmyRuntime_StartMoveCommandWithAuxiliaryValues
          (ArmyMoveAuxiliaryValue1 auxiliaryValue1,ArmyMoveAuxiliaryValue0 auxiliaryValue0,
          Q12 targetWorldYQ12,Q12 targetWorldXQ12,ArmyMovementRuntime *movementRuntime)

{
  GraphicsWorldCoordinateQ12 currentWorldX;
  GraphicsWorldCoordinateQ12 currentWorldY;
  WorldRuntimeContext *worldRuntime;
  PathingDestination resolvedDestination;
  
  worldRuntime = &g_InGameRuntimeRoot->worldRuntime;
  movementRuntime->movementStateFlags =
       movementRuntime->movementStateFlags |
       (ARMY_MOVEMENT_ROUTED | ARMY_MOVEMENT_ORDERED | ARMY_MOVEMENT_WAYPOINTS_QUEUED | ARMY_MOVEMENT_LOCKED | ARMY_MOVEMENT_ACTIVE);
  movementRuntime->movementStateFlags =
       movementRuntime->movementStateFlags &
       ~(ARMY_MOVEMENT_DIRECT | ARMY_MOVEMENT_TARGET_FOLLOWING | ARMY_MOVEMENT_ROUTE_POINT_REACHED);
  movementRuntime->commandModeFlags = movementRuntime->commandModeFlags & ~(uint32_t)ARMY_COMMAND_MODE_SELECTION_ORDER;
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

/* Sets a new immediate move position without path finding (ignored while the movement is locked). The
   fallback position only follows when no move was active, the final target only when mirroring is enabled;
   the current model position is recorded for the route-retry check.
*/
void ArmyRuntime_SetPendingMoveTarget(Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime)

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
         movementRuntime->movementStateFlags & ~(ARMY_MOVEMENT_ROUTE_POINT_REACHED | ARMY_MOVEMENT_TARGET_FOLLOWING);
    currentWorldX = (modelNode->worldTransform).translation.x;
    currentWorldY = (modelNode->worldTransform).translation.y;
    movementRuntime->retryCountdown = ARMY_MOVEMENT_RETRY_TICKS;
    movementRuntime->lastCheckedWorldXQ12 = currentWorldX;
    movementRuntime->lastCheckedWorldYQ12 = currentWorldY;
  }
  return;
}

/* Class command that does nothing: model classes without their own command handling. It fills the
   classCommand slots 0-3, 5-9, 12, 17-19 and 21 of g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes, which
   ArmyRuntime_DispatchClassCommand calls by model class.
*/
void ArmyRuntimeClassCommand_NoOp(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  return;
}

/* Starts a new routed move order to the target (path finding via EntityPathing), dropping any waypoint
   queue and target mirroring. While the movement is locked the target replaces the waypoint queue instead.
   Entities whose definition has no accelerationPerTick (0: not mobile) ignore the order.
*/
void ArmyRuntime_StartRoutedMoveCommand(Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime)

{
  GraphicsWorldCoordinateQ12 currentWorldX;
  GraphicsWorldCoordinateQ12 currentWorldY;
  WorldRuntimeContext *worldRuntime;
  PathingDestination resolvedDestination;

  if (*(int *)((uintptr_t)(movementRuntime->entityRuntime->common).ownership.definitionOrClassRecord +
              24) != 0) {
    if ((movementRuntime->movementStateFlags & ARMY_MOVEMENT_LOCKED) == 0) {
      worldRuntime = &g_InGameRuntimeRoot->worldRuntime;
      movementRuntime->movementStateFlags =
           movementRuntime->movementStateFlags | (ARMY_MOVEMENT_ROUTED | ARMY_MOVEMENT_ORDERED | ARMY_MOVEMENT_ACTIVE);
      movementRuntime->movementStateFlags =
           movementRuntime->movementStateFlags &
           ~(ARMY_MOVEMENT_MIRROR_TARGET | ARMY_MOVEMENT_DIRECT | ARMY_MOVEMENT_TARGET_FOLLOWING | ARMY_MOVEMENT_ROUTE_POINT_REACHED |
             ARMY_MOVEMENT_WAYPOINTS_QUEUED);
      movementRuntime->commandModeFlags = movementRuntime->commandModeFlags & ~(uint32_t)ARMY_COMMAND_MODE_SELECTION_ORDER;
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
      ArmyRuntime_AppendWaypointOrStartMove(targetWorldY,targetWorldX,movementRuntime);
    }
  }
  return;
}

/* Same as ArmyRuntime_StartRoutedMoveCommand, but keeps the waypoint queue and target mirroring:
   used by ArmyRuntime_UpdateMovementAndWaypoints to start the next queued waypoint.
*/
void ArmyRuntime_StartNextQueuedWaypointMove(Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime)

{
  GraphicsWorldCoordinateQ12 currentWorldX;
  GraphicsWorldCoordinateQ12 currentWorldY;
  WorldRuntimeContext *worldRuntime;
  PathingDestination resolvedDestination;

  if (*(int *)((uintptr_t)(movementRuntime->entityRuntime->common).ownership.definitionOrClassRecord +
              24) != 0) {
    if ((movementRuntime->movementStateFlags & ARMY_MOVEMENT_LOCKED) == 0) {
      worldRuntime = &g_InGameRuntimeRoot->worldRuntime;
      movementRuntime->movementStateFlags =
           movementRuntime->movementStateFlags | (ARMY_MOVEMENT_ROUTED | ARMY_MOVEMENT_ORDERED | ARMY_MOVEMENT_ACTIVE);
      movementRuntime->movementStateFlags =
           movementRuntime->movementStateFlags & ~(ARMY_MOVEMENT_DIRECT | ARMY_MOVEMENT_TARGET_FOLLOWING | ARMY_MOVEMENT_ROUTE_POINT_REACHED);
      movementRuntime->commandModeFlags = movementRuntime->commandModeFlags & ~(uint32_t)ARMY_COMMAND_MODE_SELECTION_ORDER;
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
      ArmyRuntime_AppendWaypointOrStartMove(targetWorldY,targetWorldX,movementRuntime);
    }
  }
  return;
}

/* Starts a target-following move (only when no move is active, the movement is not locked and the route-retry
   countdown has run out). The step away from the current final target is clamped to 2.0 world units, so a
   following army re-routes in short hops; the final target itself stays unchanged. Called by
   ArmyRuntimeCommand_UpdateTargetFollowingState.
*/
void ArmyRuntime_StartClampedMoveCommand(Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime)

{
  GraphicsWorldCoordinateQ12 currentWorldX;
  GraphicsWorldCoordinateQ12 currentWorldY;
  InGameRuntimeRoot *inGameRoot;
  FixedLengthAngle offsetAngleLength;
  FixedSinCos clampedOffset;
  PathingDestination resolvedDestination;

  inGameRoot = g_InGameRuntimeRoot;
  if (((movementRuntime->movementStateFlags & (ARMY_MOVEMENT_ACTIVE | ARMY_MOVEMENT_LOCKED)) == 0) &&
      (movementRuntime->retryCountdown == 0)) {
    offsetAngleLength = FixedMath_Vector2AngleAndLength
                      (targetWorldY - movementRuntime->movementTargetWorldYQ12,
                       targetWorldX - movementRuntime->movementTargetWorldXQ12);
    if (ARMY_MOVEMENT_FOLLOW_MAX_STEP_Q12 < (int)offsetAngleLength.length) {
      clampedOffset = FixedMath_SinCosScaled(offsetAngleLength.angle,ARMY_MOVEMENT_FOLLOW_MAX_STEP_Q12);
      targetWorldX = clampedOffset.cosValue + movementRuntime->movementTargetWorldXQ12;
      targetWorldY = clampedOffset.sinValue + movementRuntime->movementTargetWorldYQ12;
    }
    movementRuntime->movementStateFlags =
         movementRuntime->movementStateFlags &
         ~(ARMY_MOVEMENT_MIRROR_TARGET | ARMY_MOVEMENT_ORDERED | ARMY_MOVEMENT_ROUTE_POINT_REACHED | ARMY_MOVEMENT_WAYPOINTS_QUEUED);
    movementRuntime->movementStateFlags =
         movementRuntime->movementStateFlags |
         (ARMY_MOVEMENT_DIRECT | ARMY_MOVEMENT_TARGET_FOLLOWING | ARMY_MOVEMENT_ACTIVE);
    resolvedDestination = EntityPathing_ResolveDestinationAndRebuildRoutes
                      (targetWorldY,targetWorldX,movementRuntime->entityRuntime,
                       &inGameRoot->worldRuntime);
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

/* Starts a direct move to the target (unless the movement is locked): drops the waypoint queue and
   routes to the target, but leaves the final movement target unchanged. Used by
   ArmyRuntime_UpdateMovementAndWaypoints to re-approach the final target.
*/
void ArmyRuntime_StartDirectMoveCommand(Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime)

{
  GraphicsWorldCoordinateQ12 currentWorldX;
  GraphicsWorldCoordinateQ12 currentWorldY;
  WorldRuntimeContext *worldRuntime;
  PathingDestination resolvedDestination;

  if ((movementRuntime->movementStateFlags & ARMY_MOVEMENT_LOCKED) == 0) {
    worldRuntime = &g_InGameRuntimeRoot->worldRuntime;
    movementRuntime->movementStateFlags =
         movementRuntime->movementStateFlags | (ARMY_MOVEMENT_DIRECT | ARMY_MOVEMENT_ACTIVE);
    movementRuntime->movementStateFlags =
         movementRuntime->movementStateFlags &
         ~(ARMY_MOVEMENT_ORDERED | ARMY_MOVEMENT_TARGET_FOLLOWING | ARMY_MOVEMENT_ROUTE_POINT_REACHED |ARMY_MOVEMENT_WAYPOINTS_QUEUED);
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

/* Called by a weapon that is aimed and ready to fire. When the shot to the target is blocked, the owning army
   (if this weapon is its primary weapon or it has none, and it is not already following) starts a
   target-following move towards the target, clamped for AI combat targets, and true is returned: the weapon
   must not fire. With a clear line of fire a running target-following move is stopped and false is returned. Called
   by the aim-and-fire class updates (gameplay/army/turrets.cpp and combat.cpp).
*/
Bool8 ArmyRuntimeCommand_UpdateTargetFollowingState(Q12 targetWorldZQ12,Q12 targetWorldYQ12,Q12 targetWorldXQ12,
          WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  GameEntityRuntime *ownerEntity;
  ModelRuntimeSlot *ownerRootModelRuntime;
  Bool8 lineOfFireBlocked;

  /* modelRuntime is the weapon's model runtime. ownerEntity is the owning army (GameEntityRuntime view: its
     common.commandFlags is ArmyRuntimeSlot.movementStateFlags and commandTarget.targetFlags is commandModeFlags);
     the army's root model runtime carries the primary weapon as its first attachment */
  ownerEntity = modelRuntime->ownerArmyRuntimeOrSavedOffset.entityRuntime;
  lineOfFireBlocked = ArmyWeaponRuntime_TestTargetLineOfFire
                    (targetWorldZQ12,targetWorldYQ12,targetWorldXQ12,worldRuntime,modelRuntime);
  ownerRootModelRuntime = (ModelRuntimeSlot *)(ownerEntity->common).ownership.definitionOrClassRecord;
  if (lineOfFireBlocked) {
    if (((modelRuntime == ownerRootModelRuntime->attachments[0].childModelRuntimeOrSavedOffset) ||
        (ownerRootModelRuntime->attachments[0].childModelRuntimeOrSavedOffset == nullptr)
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

/* While a non-direct move is active or the movement is locked, appends the target to the waypoint queue
   (starting a new queue if none is in use; when full the last entry is overwritten); otherwise starts it
   at once with ArmyRuntime_StartRoutedMoveCommand.
*/
void ArmyRuntime_AppendWaypointOrStartMove
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
    movementRuntime->movementStateFlags = movementRuntime->movementStateFlags | ARMY_MOVEMENT_ORDERED;
    if (waypointCount < ARMY_MOVEMENT_WAYPOINT_CAPACITY) {
      waypointCount = waypointCount + 1;
    }
    movementRuntime->commandModeFlags = movementRuntime->commandModeFlags & ~(uint32_t)(ARMY_COMMAND_MODE_UNUSED_400 | ARMY_COMMAND_MODE_SELECTION_ORDER);
    movementRuntime->queuedWaypoints[waypointCount - 1].worldXQ12 = targetWorldX;
    movementRuntime->queuedWaypoints[waypointCount - 1].worldYQ12 = targetWorldY;
    movementRuntime->queuedWaypointCount = waypointCount;
  }
  else {
    ArmyRuntime_StartRoutedMoveCommand(targetWorldY,targetWorldX,movementRuntime);
  }
  return;
}

/* Target-following move (ArmyRuntimeCommand_UpdateTargetFollowingState): unless the movement is locked,
   routed or still waiting for its retry countdown, routes to the target. If a move was active, its
   fallback position is first saved into the waypoint queue (so the army can resume it afterwards).
*/
void ArmyRuntime_StartMoveCommandWithFallbackWaypoints
          (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime)

{
  GraphicsWorldCoordinateQ12 currentWorldX;
  GraphicsWorldCoordinateQ12 currentWorldY;
  int remainingCount;
  WorldRuntimeContext *worldRuntime;
  Q12 *fallbackCoordinateRead;
  Q12 *waypointCoordinateWrite;
  PathingDestination resolvedDestination;

  if (((movementRuntime->movementStateFlags & (ARMY_MOVEMENT_ROUTED | ARMY_MOVEMENT_LOCKED)) == 0) &&
      (movementRuntime->retryCountdown == 0)) {
    worldRuntime = &g_InGameRuntimeRoot->worldRuntime;
    movementRuntime->movementStateFlags =
         movementRuntime->movementStateFlags &
         ~(ARMY_MOVEMENT_MIRROR_TARGET | ARMY_MOVEMENT_DIRECT | ARMY_MOVEMENT_ROUTE_POINT_REACHED |ARMY_MOVEMENT_WAYPOINTS_QUEUED);
    if ((movementRuntime->movementStateFlags & ARMY_MOVEMENT_ACTIVE) != 0) {
      /* Forward dword-by-dword copy (not memmove) of 16 dwords from fallbackPosition to the queuedWaypoints
         that directly follow it, exactly as in the original. The ranges overlap by 8 bytes, so this does not
         shift the queue: it fills all 8 waypoints with fallbackPosition. */
      remainingCount = 16;
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
         movementRuntime->movementStateFlags | (ARMY_MOVEMENT_ORDERED | ARMY_MOVEMENT_TARGET_FOLLOWING | ARMY_MOVEMENT_ACTIVE);
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

/* Ends a target-following move: the move stays active only if waypoints are queued, and unless a routed
   move is in progress all targets are set to the current model position so the army stops where it is.
   Bits 0x10-0x80 are cleared.
*/
void ArmyRuntime_ResetMovementStatePreserveQueuedTarget(ArmyMovementRuntime *movementRuntime)

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
       ~(ARMY_MOVEMENT_DIRECT | ARMY_MOVEMENT_ORDERED | ARMY_MOVEMENT_TARGET_FOLLOWING | ARMY_MOVEMENT_ROUTE_POINT_REACHED);
  return;
}

/* Ends a clamped target-following move: without queued waypoints the move just stops being active;
   with queued waypoints the current and fallback positions are set to the model position (the final
   target is kept). Bits 0x10-0x80 are cleared.
*/
void ArmyRuntime_ResetMovementStateFromCurrentPosition(ArmyMovementRuntime *movementRuntime)

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
       ~(ARMY_MOVEMENT_DIRECT | ARMY_MOVEMENT_ORDERED | ARMY_MOVEMENT_TARGET_FOLLOWING | ARMY_MOVEMENT_ROUTE_POINT_REACHED);
  return;
}

/* Per-tick movement step; always stores the position to steer to in *outWorldXQ12 / *outWorldYQ12 and
   returns true when the unit has arrived. While a move is active the stored movement position is returned.
   When bit 0x10 is set, the route end is checked: once reached, the next queued waypoint is started (and the
   step repeated); otherwise the route is rebuilt when the model moved or the retry countdown ran out. Near
   the final target the move ends (arrived, current position); farther away a direct move to it is started.
*/
Bool8 ArmyRuntime_UpdateMovementAndWaypoints
          (WorldRuntimeContext *worldRuntime,ArmyMovementRuntime *movementRuntime,Q12 *outWorldXQ12,
          Q12 *outWorldYQ12)

{
  uint32_t exceededDistance;
  Q12 movementWorldX;
  Q12 movementWorldY;
  Q12 currentWorldX;
  Q12 currentWorldY;
  int offsetX;
  int offsetY;
  int modelWorldX;
  int modelWorldY;
  int waypointIndex;
  uint32_t distanceX;
  uint32_t distanceY;
  Bool8 belowThreshold;
  PathingDestination resolvedDestination;
  Q12 queuedWorldYQ12;
  Q12 queuedWorldXQ12;
  ModelRuntimeNode *modelNode;

  modelNode = movementRuntime->modelNodeRuntime;
  if ((movementRuntime->movementStateFlags & ARMY_MOVEMENT_ROUTE_POINT_REACHED) == 0) {
    if ((movementRuntime->movementStateFlags & ARMY_MOVEMENT_ACTIVE) != 0) {
      movementWorldX = movementRuntime->movementWorldXQ12;
      movementWorldY = movementRuntime->movementWorldYQ12;
      *outWorldYQ12 = movementWorldY;
      *outWorldXQ12 = movementWorldX;
      return false;
    }
  }
  else {
    movementRuntime->movementStateFlags = movementRuntime->movementStateFlags & ~ARMY_MOVEMENT_ROUTE_POINT_REACHED;
    offsetX = (movementRuntime->fallbackPosition).worldXQ12 -
            (modelNode->worldTransform).translation.x;
    offsetY = (movementRuntime->fallbackPosition).worldYQ12 -
            (modelNode->worldTransform).translation.y;
    if ((offsetX < ARMY_MOVEMENT_ROUTE_END_RADIUS_Q12) && (offsetY < ARMY_MOVEMENT_ROUTE_END_RADIUS_Q12) &&
        (-ARMY_MOVEMENT_ROUTE_END_RADIUS_Q12 < offsetX) && (-ARMY_MOVEMENT_ROUTE_END_RADIUS_Q12 < offsetY)) {
      if ((movementRuntime->movementStateFlags & ARMY_MOVEMENT_WAYPOINTS_QUEUED) != 0) {
        /* pop queuedWaypoints[0] and move the other seven entries down */
        queuedWorldXQ12 = movementRuntime->queuedWaypoints[0].worldXQ12;
        queuedWorldYQ12 = movementRuntime->queuedWaypoints[0].worldYQ12;
        movementRuntime->queuedWaypointCount = movementRuntime->queuedWaypointCount - 1;
        if (movementRuntime->queuedWaypointCount == 0) {
          movementRuntime->movementStateFlags = movementRuntime->movementStateFlags & ~ARMY_MOVEMENT_WAYPOINTS_QUEUED;
        }
        else {
          for (waypointIndex = 0; waypointIndex < 7; waypointIndex++) {
            movementRuntime->queuedWaypoints[waypointIndex] = movementRuntime->queuedWaypoints[waypointIndex + 1];
          }
        }
        ArmyRuntime_StartNextQueuedWaypointMove(queuedWorldYQ12,queuedWorldXQ12,movementRuntime);
        return ArmyRuntime_UpdateMovementAndWaypoints(worldRuntime,movementRuntime,outWorldXQ12,outWorldYQ12);
      }
    }
    else {
      modelWorldX = (modelNode->worldTransform).translation.x;
      modelWorldY = (modelNode->worldTransform).translation.y;
      if ((movementRuntime->retryCountdown != 0) && (modelWorldX == movementRuntime->lastCheckedWorldXQ12) &&
          (modelWorldY == movementRuntime->lastCheckedWorldYQ12)) {
        /* Still waiting at the same spot: keep the stored movement position. */
        *outWorldYQ12 = movementRuntime->movementWorldYQ12;
        *outWorldXQ12 = movementRuntime->movementWorldXQ12;
        return false;
      }
      movementRuntime->retryCountdown = ARMY_MOVEMENT_RETRY_TICKS;
      movementRuntime->lastCheckedWorldXQ12 = modelWorldX;
      movementRuntime->lastCheckedWorldYQ12 = modelWorldY;
      resolvedDestination = EntityPathing_ResolveDestinationAndRebuildRoutes
                         ((movementRuntime->fallbackPosition).worldYQ12,
                          (movementRuntime->fallbackPosition).worldXQ12,
                          movementRuntime->entityRuntime,worldRuntime);
      movementRuntime->movementWorldXQ12 = resolvedDestination.primaryWorldXQ12;
      movementRuntime->movementWorldYQ12 = resolvedDestination.primaryWorldYQ12;
      (movementRuntime->fallbackPosition).worldXQ12 = resolvedDestination.fallbackWorldXQ12;
      (movementRuntime->fallbackPosition).worldYQ12 = resolvedDestination.fallbackWorldYQ12;
      *outWorldXQ12 = resolvedDestination.primaryWorldXQ12;
      *outWorldYQ12 = resolvedDestination.primaryWorldYQ12;
      return false;
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
  if ((distanceX < ARMY_MOVEMENT_TARGET_RADIUS_Q12 + 1) && (distanceY < ARMY_MOVEMENT_TARGET_RADIUS_Q12 + 1)) {
    movementRuntime->movementStateFlags =
         movementRuntime->movementStateFlags &
         ~(ARMY_MOVEMENT_DIRECT | ARMY_MOVEMENT_ORDERED | ARMY_MOVEMENT_TARGET_FOLLOWING | ARMY_MOVEMENT_ROUTE_POINT_REACHED |ARMY_MOVEMENT_WAYPOINTS_QUEUED |
           ARMY_MOVEMENT_ACTIVE);
    currentWorldX = (modelNode->worldTransform).translation.x;
    currentWorldY = (modelNode->worldTransform).translation.y;
    *outWorldYQ12 = currentWorldY;
    *outWorldXQ12 = currentWorldX;
    return true;
  }
  /* Original quirk: the "position" returned here is the pair of absolute distances to the target, not a
     world position. In the original the arrival flag is the status ArmyRuntime_StartDirectMoveCommand leaves
     behind (not arrived when locked, else the pathing call's status); belowThreshold is always false here since
     exceededDistance > ARMY_MOVEMENT_TARGET_RADIUS_Q12. That matches: EntityPathing_ResolveDestinationAndRebuildRoutes
     always reports "not arrived", so the status left by the direct move is always "not arrived". */
  /* the first distance that is outside the target radius */
  if (distanceX < ARMY_MOVEMENT_TARGET_RADIUS_Q12 + 1) {
    exceededDistance = distanceY;
  }
  else {
    exceededDistance = distanceX;
  }
  belowThreshold = exceededDistance < ARMY_MOVEMENT_TARGET_RADIUS_Q12;
  ArmyRuntime_StartDirectMoveCommand
            (movementRuntime->movementTargetWorldYQ12,movementRuntime->movementTargetWorldXQ12,
             movementRuntime);
  *outWorldYQ12 = (Q12)distanceY;
  *outWorldXQ12 = (Q12)distanceX;
  return belowThreshold;
}

/* Gives the army a new target army (NULL clears the command). A move started by target following is
   ended first; the command is stamped with the standard generation, or generation 0 when cleared.
*/
void ArmyRuntime_ResolveCommandTarget(ArmyRuntimeSlot *targetArmyRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ArmyCommandGeneration standardGeneration;

  standardGeneration = g_ArmyCommandGenerationStandard;
  if ((armyRuntime->movementStateFlags & ARMY_MOVEMENT_TARGET_FOLLOWING) != 0) {
    if ((armyRuntime->commandModeFlags & ARMY_COMMAND_MODE_AI_COMBAT_TARGET) == 0) {
      ArmyRuntime_ResetMovementStatePreserveQueuedTarget((ArmyMovementRuntime *)armyRuntime);
    }
    else {
      ArmyRuntime_ResetMovementStateFromCurrentPosition((ArmyMovementRuntime *)armyRuntime);
    }
  }
  if (targetArmyRuntime == nullptr) {
    armyRuntime->commandModeFlags = 0;
    armyRuntime->commandGeneration = 0;
  }
  else {
    armyRuntime->commandModeFlags = ARMY_COMMAND_MODE_TARGET_ARMY;
    armyRuntime->commandGeneration = standardGeneration;
  }
  armyRuntime->commandTargetArmyRuntime = targetArmyRuntime;
  return;
}

/* Gives the army a target position command (commandCoordinate0-2Q12): ends a move started by target
   following, drops any target army and stamps the standard command generation.
*/
void ArmyRuntime_ApplyTargetPositionCommand
          (Q12 coordinate2Q12,Q12 coordinate1Q12,Q12 coordinate0Q12,ArmyRuntimeSlot *armyRuntime)

{
  ArmyCommandGeneration commandGeneration;

  if ((armyRuntime->movementStateFlags & ARMY_MOVEMENT_TARGET_FOLLOWING) != 0) {
    if ((armyRuntime->commandModeFlags & ARMY_COMMAND_MODE_AI_COMBAT_TARGET) == 0) {
      ArmyRuntime_ResetMovementStatePreserveQueuedTarget((ArmyMovementRuntime *)armyRuntime);
    }
    else {
      ArmyRuntime_ResetMovementStateFromCurrentPosition((ArmyMovementRuntime *)armyRuntime);
    }
  }
  armyRuntime->commandModeFlags = ARMY_COMMAND_MODE_TARGET_POSITION;
  armyRuntime->commandCoordinate0Q12 = coordinate0Q12;
  armyRuntime->commandCoordinate1Q12 = coordinate1Q12;
  armyRuntime->commandCoordinate2Q12 = coordinate2Q12;
  commandGeneration = g_ArmyCommandGenerationStandard;
  armyRuntime->commandTargetArmyRuntime = nullptr;
  armyRuntime->movementStateFlags = armyRuntime->movementStateFlags & ~ARMY_MOVEMENT_TARGET_FOLLOWING;
  armyRuntime->commandGeneration = commandGeneration;
  return;
}

/* Stops an entity where it stands (used by the stop command on the selection): clears the command flags
   0x01, 0x08, 0x10 and 0x20 and sets the path target and both tracked coordinate pairs to the current
   x/y position of its model.
*/
void GameEntityRuntime_ResetMovementFlagsAndAnchorCoordinatesFromModel(GameEntityRuntime *entityRuntime)

{
  GameEntityCommandFlags *commandFlagsField;
  ModelRuntimeNode *ownerModelNode;
  GraphicsWorldCoordinateQ12 modelX;
  GraphicsWorldCoordinateQ12 modelY;

  ownerModelNode = (entityRuntime->common).ownership.modelNode;
  commandFlagsField = &(entityRuntime->common).commandFlags;
  *commandFlagsField = *commandFlagsField &
                      ~(uint32_t)(ARMY_MOVEMENT_ACTIVE | ARMY_MOVEMENT_WAYPOINTS_QUEUED |
                                  ARMY_MOVEMENT_ROUTE_POINT_REACHED | ARMY_MOVEMENT_TARGET_FOLLOWING);
  modelX = (ownerModelNode->worldTransform).translation.x;
  modelY = (ownerModelNode->worldTransform).translation.y;
  (entityRuntime->common).pathCoordinate0Q12 = modelX;
  (entityRuntime->common).pathCoordinate1Q12 = modelY;
  (entityRuntime->common).trackedCoordinate0Q12 = modelX;
  (entityRuntime->common).trackedCoordinate1Q12 = modelY;
  (entityRuntime->common).damageState.trackedCoordinate0Q12 = modelX;
  (entityRuntime->common).damageState.trackedCoordinate1Q12 = modelY;
  return;
}

/* Where an entity's current command should take it, for the walker movement (walker.cpp): target flag
   1 aims at a target entity (its model position, raised by the definition's aimHeightOffsetQ12; class 0x15
   aims at its first child node), flag 2 at a fixed world position. A target entity that the owner's faction can
   no longer see is dropped (entity and flags cleared). Writes the position to *outPosition and returns true, or
   returns false when there is none.
*/
Bool8 GameEntityRuntime_ResolveCommandTargetPosition(GameEntityRuntime *targetState,FixedVectorQ12 *outPosition)

{
  GameEntityRuntime *commandTargetEntity;
  uint32_t visibilityMask;
  ModelRuntimeSlot *targetModelRuntime;
  ModelRuntimeNode *targetModelNode;

  /* Original quirk: on failure the original leaves the position undefined (left-over intermediate values), and
     one caller still copies the Z value into a local. The port writes zeros instead, so *outPosition is always
     written. */
  outPosition->xQ12 = 0;
  outPosition->yQ12 = 0;
  outPosition->zQ12 = 0;
  if (((targetState->common).commandTarget.targetFlags & 1) == 0) {
    if (((targetState->common).commandTarget.targetFlags & 2) == 0) {
      return false;
    }
    outPosition->xQ12 = (targetState->common).commandTarget.targetWorldXQ12;
    outPosition->yQ12 = (targetState->common).commandTarget.targetWorldYQ12;
    outPosition->zQ12 = (targetState->common).commandTarget.targetWorldZQ12;
    return true;
  }
  commandTargetEntity = (targetState->common).commandTarget.targetEntity;
  if (commandTargetEntity == nullptr) {
    return false;
  }
  /* two bits per faction; the upper one = the target is visible to that faction */
  visibilityMask = 2u << ((uint8_t)((targetState->common).ownership.ownerIndex * 2) & 31);
  targetModelRuntime = (ModelRuntimeSlot *)(commandTargetEntity->common).ownership.definitionOrClassRecord;
  if (((commandTargetEntity->common).damageState.factionVisibilityBits1C & visibilityMask) == 0) {
    /* the owner's faction lost sight of the target: drop it */
    (targetState->common).commandTarget.targetEntity = nullptr;
    (targetState->common).commandTarget.targetFlags = 0;
    return false;
  }
  targetModelNode = (commandTargetEntity->common).ownership.modelNode;
  if (targetModelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_21_AIRCRAFT) {
    targetModelNode = targetModelNode->childNodes[0];
  }
  outPosition->xQ12 = (targetModelNode->worldTransform).translation.x;
  outPosition->yQ12 = (targetModelNode->worldTransform).translation.y;
  outPosition->zQ12 =
       (targetModelNode->worldTransform).translation.z +
       targetModelRuntime->definitionOrSavedId.runtimeDefinition->aimHeightOffsetQ12;
  return true;
}
