/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/movement.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/army/movement.h>
#include <thandor/thandor.h>

/* Implementation ownership: gameplay/army/movement. */

/* What ArticulatedWalker_TryStartRouteStep decided for a standing walker. */
typedef enum ArticulatedRouteStep {
  ARTICULATED_ROUTE_STEP_NONE,             /* nothing started: check whether the feet need closing */
  ARTICULATED_ROUTE_STEP_STARTED,          /* a walking step or a turn on the spot has been started */
  ARTICULATED_ROUTE_STEP_CLOSE_FEET,       /* walking ended off the walk-on angle: close the feet */
  ARTICULATED_ROUTE_STEP_ADVANCE_WAYPOINT  /* the route point is reached */
} ArticulatedRouteStep;

/* Advances the running step of the articulated walker: walking speed rises in the first half of the step
   and falls in the second; the step progress grows by previous speed * step rate (fallbackPosition1Q12) *
   ticks. When the progress reaches the end, the foot target becomes the foot position. */
static void ArticulatedWalker_AdvanceRunningStep(WorldRuntimeContext *worldRuntime,
          ModelRuntimeArticulatedMovementDefinitionView *modelRuntime,
          ModelDefinitionArticulatedMovementView *movementDefinition)
{
  Q12 *stepState = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
  Q12 previousSpeed;
  Q12 newSpeed;
  uint32_t progressIncrement;
  ArmyRuntimeCoordinateCommandOrHistoryValue footHeading;
  uint32_t restartSpeed;

  previousSpeed = (modelRuntime->movementControl).movementAdvancePerTickQ12;
  newSpeed = movementDefinition->movementAdvanceDeltaQ12PerTick;
  if (((int)modelRuntime->leftStepProgressQ12 < ARMY_ARTICULATED_STEP_PROGRESS_END_Q12 / 2 + 1) &&
     ((int)(modelRuntime->articulatedContact).terrainContactMode < ARMY_ARTICULATED_STEP_PROGRESS_END_Q12 / 2 + 1)) {
    newSpeed = newSpeed + previousSpeed;
  }
  else {
    newSpeed = -(newSpeed - previousSpeed);
  }
  (modelRuntime->movementControl).movementAdvancePerTickQ12 = newSpeed;
  progressIncrement =
       (uint32_t)((int)(previousSpeed * (modelRuntime->articulatedContact).fallbackPosition1Q12 *
                        g_InGameSimulationStepTicks) >> 12);
  if ((*stepState & ARMY_ARTICULATED_STEP_RIGHT) == 0) {
    modelRuntime->leftStepProgressQ12 = progressIncrement + modelRuntime->leftStepProgressQ12;
    if (ARMY_ARTICULATED_STEP_PROGRESS_END_Q12 - 1 < modelRuntime->leftStepProgressQ12) {
      /* left step done: the left foot target becomes the left foot position */
      *stepState = *stepState & ~ARMY_ARTICULATED_STEP_LEFT;
      modelRuntime->leftStepProgressQ12 = 0;
      modelRuntime->movementTarget0Q12 = modelRuntime->leftStepTargetXQ12;
      modelRuntime->leftFootYQ12 = modelRuntime->leftStepTargetYQ12;
      modelRuntime->leftFootZQ12 = modelRuntime->leftStepTargetZQ12;
      footHeading = (modelRuntime->linkedChildOverloadedState).leftHeadingCommandOrSpawnValue;
      restartSpeed = (modelRuntime->linkedChildSpawnParameters).parameter0;
      modelRuntime->stepStartHeading = modelRuntime->stepEndHeading;
      modelRuntime->leftFootGroundNormal = modelRuntime->fallbackWorldYQ12;
      (modelRuntime->linkedChildOverloadedState).primaryCoordinateCommandOrHistory = footHeading;
      (modelRuntime->movementControl).movementAdvancePerTickQ12 = restartSpeed;
      ArmyArticulatedRuntime_UpdateContactChildAndEffects
                (modelRuntime->rootModelNode->childNodes[0],worldRuntime,(ModelRuntimeSlot *)modelRuntime);
    }
  }
  else {
    /* terrainContactMode is the right step progress here */
    (modelRuntime->articulatedContact).terrainContactMode =
         (ArmyTerrainContactDispatchMode)(progressIncrement + (modelRuntime->articulatedContact).terrainContactMode);
    if (ARMY_ARTICULATED_STEP_PROGRESS_END_Q12 - 1 < (modelRuntime->articulatedContact).terrainContactMode) {
      /* right step done: the right foot target becomes the right foot position */
      *stepState = *stepState & ~ARMY_ARTICULATED_STEP_RIGHT;
      (modelRuntime->articulatedContact).terrainContactMode =
           ARMY_TERRAIN_CONTACT_ACQUIRE_OR_INITIALIZE_CONTACT_SLOT;
      modelRuntime->movementTarget1Q12 = modelRuntime->rightStepTargetXQ12;
      modelRuntime->rightFootYQ12 = modelRuntime->rightStepTargetYQ12;
      modelRuntime->rightFootZQ12 = modelRuntime->rightStepTargetZQ12;
      footHeading = (modelRuntime->linkedChildOverloadedState).rightHeadingCommandOrSpawnValue;
      restartSpeed = (modelRuntime->linkedChildSpawnParameters).parameter0;
      modelRuntime->stepStartHeading = modelRuntime->stepEndHeading;
      modelRuntime->linkedArmyRuntimeOrSavedOffset =
           (ArmyRuntimeSlot *)modelRuntime->fallbackWorldXQ12;
      (modelRuntime->linkedChildOverloadedState).secondaryCoordinateCommandOrHistory = footHeading;
      (modelRuntime->movementControl).movementAdvancePerTickQ12 = restartSpeed;
      ArmyArticulatedRuntime_UpdateContactChildAndEffects
                (modelRuntime->rootModelNode->childNodes[1],worldRuntime,(ModelRuntimeSlot *)modelRuntime);
    }
  }
}

/* After a step has ended: the route point counts as reached when there is no route and the walker faces the
   command target (within ARMY_ARTICULATED_TURN_ANGLE16) or has none, or when it stands within
   ARMY_MOVEMENT_ROUTE_END_RADIUS_Q12 of the route point. */
static bool ArticulatedWalker_StepEndReachedRoutePoint(WorldRuntimeContext *worldRuntime,
          ModelRuntimeArticulatedMovementDefinitionView *modelRuntime)
{
  ModelRuntimeNode *rootNode = modelRuntime->rootModelNode;
  Q12 waypointWorldXQ12;
  Q12 waypointWorldYQ12;
  FixedVectorQ12 commandTargetPosition;
  uint32_t targetHeading;
  uint32_t steerAngle;
  uint32_t waypointDistance;

  if (ArmyRuntime_UpdateMovementAndWaypoints
           (worldRuntime,(ArmyMovementRuntime *)modelRuntime->ownerArmyRuntime,&waypointWorldXQ12,
            &waypointWorldYQ12)) {
    if (GameEntityRuntime_ResolveCommandTargetPosition
             ((GameEntityRuntime *)modelRuntime->ownerArmyRuntime,&commandTargetPosition)) {
      targetHeading = FixedMath_Atan2Angle16
                        (commandTargetPosition.yQ12 - (rootNode->worldTransform).translation.y,
                         commandTargetPosition.xQ12 - (rootNode->worldTransform).translation.x);
      steerAngle = targetHeading - (rootNode->modelPayload).worldRotationAngle2 & FIXED_ANGLE16_MASK;
      if ((ARMY_ARTICULATED_TURN_ANGLE16 < steerAngle) &&
         (steerAngle < FIXED_ANGLE16_FULL_TURN - ARMY_ARTICULATED_TURN_ANGLE16)) {
        return false;
      }
    }
    return true;
  }
  waypointDistance = FixedMath_Length2(waypointWorldYQ12 - (rootNode->worldTransform).translation.y,
                                       waypointWorldXQ12 - (rootNode->worldTransform).translation.x);
  return waypointDistance <= ARMY_MOVEMENT_ROUTE_END_RADIUS_Q12;
}

/* Route point reached: ends walking/turning and moves the owner on to the next route point
   (ARMY_MOVEMENT_ROUTE_POINT_REACHED while the route goes on). */
static void ArticulatedWalker_AdvanceWaypoint(WorldRuntimeContext *worldRuntime,
          ModelRuntimeArticulatedMovementDefinitionView *modelRuntime)
{
  ArmyRuntimeSlot *ownerArmy = modelRuntime->ownerArmyRuntime;
  Q12 *stepState = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
  Q12 waypointWorldXQ12;
  Q12 waypointWorldYQ12;

  *stepState = *stepState & ~(ARMY_ARTICULATED_STEP_WALK | ARMY_ARTICULATED_STEP_TURN);
  if (!ArmyRuntime_UpdateMovementAndWaypoints
            (worldRuntime,(ArmyMovementRuntime *)ownerArmy,&waypointWorldXQ12,&waypointWorldYQ12)) {
    ownerArmy->movementStateFlags = ownerArmy->movementStateFlags | ARMY_MOVEMENT_ROUTE_POINT_REACHED;
  }
}

/* Second half of a turn, or a closing step: the other foot is set beside the first. */
static void ArticulatedWalker_StartClosingStep(WorldRuntimeContext *worldRuntime,
          ModelRuntimeArticulatedMovementDefinitionView *modelRuntime)
{
  Q12 *stepState = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
  uint32_t closingHeading;

  /* the stored turn angle in the upper 16 bits is added to the step end heading */
  closingHeading = (*stepState >> 16) + modelRuntime->stepEndHeading & FIXED_ANGLE16_MASK;
  if ((*stepState & ARMY_ARTICULATED_STEP_LEFT_LAST) == 0) {
    *stepState = *stepState & ~(ARMY_ARTICULATED_STEP_WALK | ARMY_ARTICULATED_STEP_TURN | ARMY_ARTICULATED_STEP_RIGHT_LAST | ARMY_ARTICULATED_STEP_LEFT_LAST | ARMY_ARTICULATED_STEP_RIGHT | ARMY_ARTICULATED_STEP_LEFT);
    *stepState = *stepState |
         (ARMY_ARTICULATED_STEP_CLOSE | ARMY_ARTICULATED_STEP_LEFT_LAST | ARMY_ARTICULATED_STEP_LEFT);
    ArmyArticulatedRuntime_InitializeLeftTerrainContact
              ((AngleTurn16Stored32)closingHeading,(ArmyArticulatedRuntimeSlotView *)modelRuntime,worldRuntime);
  }
  else {
    *stepState = *stepState & ~(ARMY_ARTICULATED_STEP_WALK | ARMY_ARTICULATED_STEP_TURN | ARMY_ARTICULATED_STEP_RIGHT_LAST | ARMY_ARTICULATED_STEP_LEFT_LAST | ARMY_ARTICULATED_STEP_RIGHT | ARMY_ARTICULATED_STEP_LEFT);
    *stepState = *stepState |
         (ARMY_ARTICULATED_STEP_CLOSE | ARMY_ARTICULATED_STEP_RIGHT_LAST | ARMY_ARTICULATED_STEP_RIGHT);
    ArmyArticulatedRuntime_InitializeRightTerrainContact
              ((AngleTurn16Stored32)closingHeading,(ArmyArticulatedRuntimeSlotView *)modelRuntime,worldRuntime);
  }
}

/* Standing walker with a living owner: starts a walking step when the route point lies ahead, or a turn on
   the spot (towards the route point, or towards the command target when there is no route; length
   UINT32_MAX = do not walk). */
static ArticulatedRouteStep ArticulatedWalker_TryStartRouteStep(WorldRuntimeContext *worldRuntime,
          ModelRuntimeArticulatedMovementDefinitionView *modelRuntime,ModelRuntimeNode *rootNode)
{
  Q12 *stepState = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
  Q12 waypointWorldXQ12;
  Q12 waypointWorldYQ12;
  FixedVectorQ12 commandTargetPosition;
  FixedLengthAngle targetAngleLength;
  int deltaX;
  int deltaY;
  uint32_t steerAngle;
  uint32_t walkOnAngle;

  if (ArmyRuntime_UpdateMovementAndWaypoints
           (worldRuntime,(ArmyMovementRuntime *)modelRuntime->ownerArmyRuntime,&waypointWorldXQ12,
            &waypointWorldYQ12)) {
    /* no route: only turn towards the command target */
    if (!GameEntityRuntime_ResolveCommandTargetPosition
              ((GameEntityRuntime *)modelRuntime->ownerArmyRuntime,&commandTargetPosition)) {
      return ARTICULATED_ROUTE_STEP_NONE;
    }
    targetAngleLength.angle =
         FixedMath_Atan2Angle16
                   (commandTargetPosition.yQ12 - (rootNode->worldTransform).translation.y,
                    commandTargetPosition.xQ12 - (rootNode->worldTransform).translation.x);
    targetAngleLength.length = UINT32_MAX;
  }
  else {
    deltaX = waypointWorldXQ12 - (rootNode->worldTransform).translation.x;
    deltaY = waypointWorldYQ12 - (rootNode->worldTransform).translation.y;
    if ((deltaX == 0) && (deltaY == 0)) {
      targetAngleLength = (FixedLengthAngle){ .length = 0, .angle = (rootNode->modelPayload).worldRotationAngle2 };
    }
    else {
      targetAngleLength = FixedMath_Vector2AngleAndLength(deltaY,deltaX);
    }
  }
  steerAngle = targetAngleLength.angle - (rootNode->modelPayload).worldRotationAngle2 & FIXED_ANGLE16_MASK;
  if (0 < (int)targetAngleLength.length) {
    /* walk on when the route point lies within the eighth turn ahead (ARMY_ARTICULATED_WALK_ON_ANGLE16 while
       already walking) */
    walkOnAngle = ((*stepState & ARMY_ARTICULATED_STEP_WALK) == 0) ? FIXED_ANGLE16_EIGHTH_TURN
                                                                    : ARMY_ARTICULATED_WALK_ON_ANGLE16;
    if ((steerAngle <= walkOnAngle) || (FIXED_ANGLE16_FULL_TURN - walkOnAngle <= steerAngle)) {
      /* the keep mask also clears the stored turn angle in the upper 16 bits */
      if ((*stepState & ARMY_ARTICULATED_STEP_LEFT_LAST) == 0) {
        *stepState = *stepState & ARMY_ARTICULATED_STEP_NEW_WALK_KEEP_MASK;
        *stepState = *stepState |
             (ARMY_ARTICULATED_STEP_WALK | ARMY_ARTICULATED_STEP_LEFT_LAST | ARMY_ARTICULATED_STEP_LEFT);
        ArmyArticulatedRuntime_UpdateLeftTerrainContact
                  (targetAngleLength.angle,targetAngleLength.length,
                   (ArmyArticulatedRuntimeSlotView *)modelRuntime,worldRuntime);
      }
      else {
        *stepState = *stepState & ARMY_ARTICULATED_STEP_NEW_WALK_KEEP_MASK;
        *stepState = *stepState |
             (ARMY_ARTICULATED_STEP_WALK | ARMY_ARTICULATED_STEP_RIGHT_LAST | ARMY_ARTICULATED_STEP_RIGHT);
        ArmyArticulatedRuntime_UpdateRightTerrainContact
                  (targetAngleLength.angle,targetAngleLength.length,
                   (ArmyArticulatedRuntimeSlotView *)modelRuntime,worldRuntime);
      }
      return ARTICULATED_ROUTE_STEP_STARTED;
    }
  }
  if ((*stepState & ARMY_ARTICULATED_STEP_WALK) != 0) {
    return ARTICULATED_ROUTE_STEP_CLOSE_FEET;
  }
  if (targetAngleLength.length <= ARMY_MOVEMENT_ROUTE_END_RADIUS_Q12) {
    return ARTICULATED_ROUTE_STEP_ADVANCE_WAYPOINT;
  }
  if ((-1 < (int)targetAngleLength.length) ||
     ((ARMY_ARTICULATED_TURN_ANGLE16 < steerAngle) &&
      (steerAngle < FIXED_ANGLE16_FULL_TURN - ARMY_ARTICULATED_TURN_ANGLE16))) {
    /* turn on the spot, with the foot on the side of the turn */
    ArmyArticulatedRuntime_UpdateSelectedTerrainContact
              ((AngleTurn32)steerAngle,(ArmyArticulatedRuntimeSlotView *)modelRuntime,worldRuntime);
    *stepState = *stepState &
         ~(ARMY_ARTICULATED_STEP_CLOSE | ARMY_ARTICULATED_STEP_TURN | ARMY_ARTICULATED_STEP_RIGHT_LAST |
           ARMY_ARTICULATED_STEP_LEFT_LAST | ARMY_ARTICULATED_STEP_RIGHT | ARMY_ARTICULATED_STEP_LEFT);
    if (steerAngle < FIXED_ANGLE16_HALF_TURN) {
      *stepState = *stepState |
           (ARMY_ARTICULATED_STEP_TURN | ARMY_ARTICULATED_STEP_LEFT_LAST | ARMY_ARTICULATED_STEP_LEFT);
    }
    else {
      *stepState = *stepState |
           (ARMY_ARTICULATED_STEP_TURN | ARMY_ARTICULATED_STEP_RIGHT_LAST | ARMY_ARTICULATED_STEP_RIGHT);
    }
    return ARTICULATED_ROUTE_STEP_STARTED;
  }
  return ARTICULATED_ROUTE_STEP_NONE;
}

/* True when the line between the feet is not roughly square to the body heading (and no closing step has
   been made yet). */
static bool ArticulatedWalker_FeetNeedClosing(ModelRuntimeArticulatedMovementDefinitionView *modelRuntime,
          ModelRuntimeNode *rootNode)
{
  uint32_t feetLineAngle;
  uint32_t feetLineHeading;

  if (((modelRuntime->articulatedContact).fallbackPosition0Q12 & ARMY_ARTICULATED_STEP_CLOSE) != 0) {
    return false;
  }
  feetLineAngle = FixedMath_Atan2Angle16
                    (modelRuntime->leftFootYQ12 - modelRuntime->rightFootYQ12,
                     modelRuntime->movementTarget0Q12 - modelRuntime->movementTarget1Q12);
  feetLineHeading = (feetLineAngle - (rootNode->modelPayload).worldRotationAngle2) - FIXED_ANGLE16_QUARTER_TURN & FIXED_ANGLE16_MASK;
  return (ARMY_ARTICULATED_FEET_SQUARE_ANGLE16 <= feetLineHeading) &&
         ((feetLineHeading < FIXED_ANGLE16_HALF_TURN - ARMY_ARTICULATED_FEET_SQUARE_ANGLE16) ||
          ((FIXED_ANGLE16_HALF_TURN + ARMY_ARTICULATED_FEET_SQUARE_ANGLE16 <= feetLineHeading) &&
           (feetLineHeading < FIXED_ANGLE16_FULL_TURN - ARMY_ARTICULATED_FEET_SQUARE_ANGLE16)));
}

/* No step running and no turn to finish: chooses the next step. Returns false when the walker stays as it
   is and the pose is not refreshed (field grid state flag 1 clear). */
static bool ArticulatedWalker_ChooseNextStep(WorldRuntimeContext *worldRuntime,
          ModelRuntimeArticulatedMovementDefinitionView *modelRuntime,ModelRuntimeNode *rootNode)
{
  ArticulatedRouteStep routeStep = ARTICULATED_ROUTE_STEP_NONE;

  if ((modelRuntime->runtimeFlags & ARMY_RUNTIME_FLAG_DESTROYED) == 0) {
    routeStep = ArticulatedWalker_TryStartRouteStep(worldRuntime,modelRuntime,rootNode);
  }
  if (routeStep == ARTICULATED_ROUTE_STEP_STARTED) {
    return true;
  }
  if (routeStep == ARTICULATED_ROUTE_STEP_ADVANCE_WAYPOINT) {
    ArticulatedWalker_AdvanceWaypoint(worldRuntime,modelRuntime);
    return true;
  }
  /* standing: close the feet when the line between them is not roughly square to the heading */
  if ((routeStep == ARTICULATED_ROUTE_STEP_CLOSE_FEET) || ArticulatedWalker_FeetNeedClosing(modelRuntime,rootNode)) {
    ArticulatedWalker_StartClosingStep(worldRuntime,modelRuntime);
    return true;
  }
  return (worldRuntime->fieldGrid->runtimeStateFlags & 1) != 0;
}

/* Runtime update of the two-legged articulated walker (runtimeUpdate slot 3 of
   g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes, called by model class from
   ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive). While a foot is moving it advances that
   step (walking speed rises in the first half of the step and falls in the second); when no step is running
   it picks the next one from the route: a walking step, a turn on the spot, or a closing step that puts the
   feet side by side again. Then it places the body between the feet (ArmyArticulatedRuntime_UpdateSuspensionHierarchy)
   and applies water damage, linked-army and moved/turned bookkeeping like the other ground classes.
   The step state bits are the ARMY_ARTICULATED_STEP_* flags; the foot positions are described at
   ArmyArticulatedRuntime_InitializeTerrainContactGeometry.
*/

void ArmyRuntimeClass_UpdateArticulatedMovement(WorldRuntimeContext *worldRuntime,
          ModelRuntimeArticulatedMovementDefinitionView *modelRuntime)

{
  ArmyMovementStateFlags *ownerMovementFlags;
  AngleTurn32 previousRotationAngle;
  int previousWorldX;
  int previousWorldY;
  ModelDefinitionArticulatedMovementView *movementDefinition;
  int32_t waterDelta;
  int waterDamage;
  ArmyRuntimeSlot *ownerArmy;
  ModelRuntimeSlot *linkedModelRuntime;
  ModelRuntimeNode *rootNode;

  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ModelRuntimeSlot *)modelRuntime);
  linkedModelRuntime = modelRuntime->linkedModelRuntime;
  rootNode = modelRuntime->rootModelNode;
  ownerMovementFlags = &modelRuntime->ownerArmyRuntime->movementStateFlags;
  *ownerMovementFlags = *ownerMovementFlags | ARMY_MOVEMENT_STATIONARY;
  /* Drop the linked model (classState.linkedArmyRuntimeOrSavedOffset) unless both definitions have a footprint
     radius and this unit is still within it. */
  if ((linkedModelRuntime != NULL) &&
     ((linkedModelRuntime->definitionOrSavedId.runtimeDefinition->footprintRadius == 0) ||
      (modelRuntime->modelDefinition->footprintRadius == 0) ||
      !ArmyCollision_TestPointWithinExpandedRuntimeRadius
             (modelRuntime->modelDefinition->footprintRadius,(rootNode->worldTransform).translation.y,
              (rootNode->worldTransform).translation.x,linkedModelRuntime))) {
    modelRuntime->linkedModelRuntime = NULL;
  }
  previousRotationAngle = (rootNode->modelPayload).worldRotationAngle2;
  previousWorldX = (rootNode->worldTransform).translation.x;
  previousWorldY = (rootNode->worldTransform).translation.y;
  movementDefinition = modelRuntime->modelDefinition;
  waterDelta = FieldGrid_InterpolateWaterDelta
                    ((rootNode->worldTransform).translation.y,
                     (rootNode->worldTransform).translation.x,worldRuntime->fieldGrid);
  if (movementDefinition->waterDamageThreshold < waterDelta) {
    waterDamage = waterDelta * movementDefinition->waterDamageMultiplier >> 7;
    if (-1 < waterDamage) {
      ArmyRuntime_ApplyDamageAndPropagateToParent(waterDamage,(ModelRuntimeSlot *)modelRuntime);
    }
  }
  if (((modelRuntime->articulatedContact).fallbackPosition0Q12 &
       (ARMY_ARTICULATED_STEP_LEFT | ARMY_ARTICULATED_STEP_RIGHT)) != 0) {
    ArticulatedWalker_AdvanceRunningStep(worldRuntime,modelRuntime,movementDefinition);
    if ((((modelRuntime->articulatedContact).fallbackPosition0Q12 &
          (ARMY_ARTICULATED_STEP_LEFT | ARMY_ARTICULATED_STEP_RIGHT)) == 0) &&
       ArticulatedWalker_StepEndReachedRoutePoint(worldRuntime,modelRuntime)) {
      ArticulatedWalker_AdvanceWaypoint(worldRuntime,modelRuntime);
    }
  }
  else if (((modelRuntime->articulatedContact).fallbackPosition0Q12 & ARMY_ARTICULATED_STEP_TURN) == 0) {
    if (!ArticulatedWalker_ChooseNextStep(worldRuntime,modelRuntime,rootNode)) {
      return;
    }
  }
  else {
    ArticulatedWalker_StartClosingStep(worldRuntime,modelRuntime);
  }
  /* The original also stores left-over intermediate values into the root X/Y here (the real position only on
     the standing path); ArmyArticulatedRuntime_UpdateSuspensionHierarchy overwrites both before anything
     reads them, so that store is left out. */
  rootNode = modelRuntime->rootModelNode;
  rootNode->runtimeFlags = rootNode->runtimeFlags | 1;
  ArmyArticulatedRuntime_UpdateSuspensionHierarchy(rootNode,worldRuntime);
  if (((previousWorldX != (rootNode->worldTransform).translation.x) ||
      (previousWorldY != (rootNode->worldTransform).translation.y)) ||
     (previousRotationAngle != (rootNode->modelPayload).worldRotationAngle2)) {
    ownerArmy = modelRuntime->ownerArmyRuntime;
    ArmyRuntime_UpdateTimedShotAndEffectEmitters
              (worldRuntime,(ModelRuntimeUpdateView *)modelRuntime);
    ownerMovementFlags = &ownerArmy->movementStateFlags;
    *ownerMovementFlags = *ownerMovementFlags & ~ARMY_MOVEMENT_STATIONARY;
  }
  movementDefinition = modelRuntime->modelDefinition;
  ModelNodeRuntime_RebuildTransformsFromRoot(rootNode);
  ModelNodeRuntime_UpdateDepthBinMasks(movementDefinition->footprintRadius,rootNode);
}


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
  if (targetRuntime == NULL) {
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


/* Runtime update of model class 18 (runtimeUpdate slot 18 of g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes,
   called by model class from ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive): runs the AI special
   behaviour while the owner has movement flag 0x100 (set by that behaviour), then moves the unit with ground
   ArmyRuntimeClass_UpdateWaterSurfaceMovement when it sits on the water surface, otherwise with
   ArmyRuntimeClass_UpdateGroundMovement.
*/

void ArmyRuntimeClass_UpdateSpecialBehaviorAndGroundMovement
          (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView *modelRuntime
          )

{
  ArmyRuntimeSlot *armyRuntime;
  ArmyPlacementContactKindIndex32 placementContactKind;

  armyRuntime = modelRuntime->ownerArmyRuntime;
  placementContactKind = modelRuntime->modelDefinition->placementContactKindIndex;
  if ((armyRuntime->movementStateFlags & ARMY_MOVEMENT_SPECIAL_BEHAVIOR) != 0) {
    AiUnitBehavior_UpdatePioneerVehicle
              ((MdlDefinitionSemanticPrefix *)modelRuntime->modelDefinition,armyRuntime,
               armyRuntime->factionIndex,worldRuntime);
  }
  if (placementContactKind == ARMY_PLACEMENT_CONTACT_KIND_WATER_SURFACE) {
    ArmyRuntimeClass_UpdateWaterSurfaceMovement(worldRuntime,modelRuntime);
  }
  else {
    ArmyRuntimeClass_UpdateGroundMovement(worldRuntime,modelRuntime);
  }
  return;
}


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
  bool waypointArrived;
  Q12 waypointWorldXQ12;
  Q12 waypointWorldYQ12;
  GraphicsFixedVec3 aimPoint;
  bool aimPointFound;
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
                       ((MdlSerializedNodeHeader *)weaponDefinition->rootNode->childSerializedOffsets[0])->
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
  bool waypointArrived;
  Q12 waypointWorldXQ12;
  Q12 waypointWorldYQ12;
  GraphicsFixedVec3 aimPoint;
  bool aimPointFound;
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
                       ((MdlSerializedNodeHeader *)weaponDefinition->rootNode->childSerializedOffsets[0])->
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


/* Helper for ArmyRuntimeClass_UpdateGroundMovementCollisionAndTrackAnimation: recoil after a shot, tilts the
   model away from the shot direction (actionVector0Q12 + 180 degrees) by recoilTilt. */
static void TrackedMovement_ApplyRecoilTilt
          (ModelRuntimeGroundMovementTrackView *modelRuntime,ModelRuntimeNode *rootNode,int recoilTilt)

{
  FixedAzimuthElevationRoll composedAngles;

  composedAngles = FixedTransform_ComposeEulerAngles
                     (0,FIXED_ANGLE16_QUARTER_TURN - recoilTilt,
                      (modelRuntime->ownerArmyRuntime->actionVector0Q12 + FIXED_ANGLE16_HALF_TURN) -
                      (rootNode->modelPayload).worldRotationAngle2 & FIXED_ANGLE16_MASK,
                      (rootNode->modelPayload).worldRotationAngle2,
                      (rootNode->modelPayload).worldRotationAngle1,
                      (rootNode->modelPayload).worldRotationAngle0);
  (rootNode->modelPayload).worldRotationAngle0 = composedAngles.azimuthAngle;
  (rootNode->modelPayload).worldRotationAngle1 = composedAngles.elevationAngle;
  (rootNode->modelPayload).worldRotationAngle2 = composedAngles.rollAngle;
}

/* Helper for ArmyRuntimeClass_UpdateGroundMovementCollisionAndTrackAnimation: not driving this tick.
   Re-places the model at its current position (while a recoil runs, or always when the field grid flags it)
   and applies the recoil tilt. actionVector1Q12 counts the remaining recoil ticks, actionVector2Q12 is the
   tilt per tick. Returns the root node used for the rest of the tick. */
static ModelRuntimeNode *TrackedMovement_PlaceStationary
          (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementTrackView *modelRuntime)

{
  ArmyRuntimeSlot *ownerArmy;
  ModelRuntimeNode *rootNode;
  int remainingRecoilTicks;
  int recoilTilt;

  ownerArmy = modelRuntime->ownerArmyRuntime;
  rootNode = modelRuntime->rootModelNode;
  remainingRecoilTicks = ownerArmy->actionVector1Q12 - 1;
  if (remainingRecoilTicks < 0) {
    if ((worldRuntime->fieldGrid->runtimeStateFlags & 1) != 0) {
      (*g_ArmyPlacementContactKindDispatchTable.callbacks
        [modelRuntime->modelDefinition->placementContactKindIndex])
                (modelRuntime->modelDefinition->placementHeightOffsetQ12,
                 (rootNode->worldTransform).translation.y,
                 (rootNode->worldTransform).translation.x,rootNode,worldRuntime);
    }
    return rootNode;
  }
  recoilTilt = remainingRecoilTicks * ownerArmy->actionVector2Q12;
  ownerArmy->actionVector1Q12 = ownerArmy->actionVector1Q12 - 1;
  (*g_ArmyPlacementContactKindDispatchTable.callbacks
    [modelRuntime->modelDefinition->placementContactKindIndex])
            (modelRuntime->modelDefinition->placementHeightOffsetQ12,
             (rootNode->worldTransform).translation.y,(rootNode->worldTransform).translation.x
             ,rootNode,worldRuntime);
  TrackedMovement_ApplyRecoilTilt(modelRuntime,rootNode,recoilTilt);
  return rootNode;
}

/* Helper for ArmyRuntimeClass_UpdateGroundMovementCollisionAndTrackAnimation: accelerated turning as in
   ArmyRuntimeClass_UpdateGroundMovement. Turns by the current turn velocity, then accelerates it (clamped to
   the turn rate limit); a turn in the other direction first resets the velocity, a small remaining error
   snaps onto the desired heading. Returns the new (unmasked) facing angle. */
static uint32_t TrackedMovement_TurnTowardsHeading
          (ModelRuntimeGroundMovementTrackView *modelRuntime,
          ModelDefinitionGroundMovementTrackView *movementDefinition,uint32_t facingAngle,uint32_t desiredHeading)

{
  uint32_t turnVelocity;
  uint32_t headingDifference;
  int turnVelocityLimit;
  int acceleratedTurnVelocity;

  turnVelocity = (modelRuntime->movementControl).turnVelocityAngle16;
  headingDifference = desiredHeading - facingAngle & FIXED_ANGLE16_MASK;
  if (headingDifference < FIXED_ANGLE16_HALF_TURN) {
    if ((int)turnVelocity < 0) {
      (modelRuntime->movementControl).turnVelocityAngle16 = 0;
      return facingAngle;
    }
    if (headingDifference <= turnVelocity) {
      (modelRuntime->movementControl).turnVelocityAngle16 = 0;
      return desiredHeading;
    }
    turnVelocityLimit = movementDefinition->turnRateLimitAnglePerTick * g_InGameSimulationStepTicks;
    acceleratedTurnVelocity =
         turnVelocity + movementDefinition->turnRateAccelerationAnglePerTick * g_InGameSimulationStepTicks;
    (modelRuntime->movementControl).turnVelocityAngle16 = turnVelocityLimit;
    if (acceleratedTurnVelocity < turnVelocityLimit) {
      (modelRuntime->movementControl).turnVelocityAngle16 = acceleratedTurnVelocity;
    }
    return facingAngle + turnVelocity;
  }
  if (0 < (int)turnVelocity) {
    (modelRuntime->movementControl).turnVelocityAngle16 = 0;
    return facingAngle;
  }
  if (turnVelocity + FIXED_ANGLE16_FULL_TURN <= headingDifference) {
    (modelRuntime->movementControl).turnVelocityAngle16 = 0;
    return desiredHeading;
  }
  turnVelocityLimit = -movementDefinition->turnRateLimitAnglePerTick * g_InGameSimulationStepTicks;
  acceleratedTurnVelocity =
       turnVelocity - movementDefinition->turnRateAccelerationAnglePerTick * g_InGameSimulationStepTicks;
  (modelRuntime->movementControl).turnVelocityAngle16 = turnVelocityLimit;
  if (turnVelocityLimit < acceleratedTurnVelocity) {
    (modelRuntime->movementControl).turnVelocityAngle16 = acceleratedTurnVelocity;
  }
  return facingAngle + turnVelocity;
}

/* Helper for ArmyRuntimeClass_UpdateGroundMovementCollisionAndTrackAnimation: turns the model towards the
   waypoint and, if the heading error is small enough, drives towards it (or stops at a blocking army), places
   it with the placement callback and applies the recoil tilt; otherwise it stops and is placed stationary.
   Returns the root node used for the rest of the tick. */
static ModelRuntimeNode *TrackedMovement_SteerAndDrive
          (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementTrackView *modelRuntime,
          ModelRuntimeNode *rootNode,Q12 waypointWorldXQ12,Q12 waypointWorldYQ12)

{
  ArmyMovementStateFlags *ownerMovementFlags;
  ArmyRuntimeSlot *ownerArmy;
  ModelDefinitionGroundMovementTrackView *movementDefinition;
  ArmyPlacementContactKindIndex32 placementContactKind;
  int routeDeltaX;
  int routeDeltaY;
  int travelDistance;
  int remainingRecoilTicks;
  int recoilTilt;
  uint32_t facingAngle;
  uint32_t desiredHeading;
  uint32_t headingErrorLimit;
  uint32_t headingError;
  uint32_t targetDistance;
  FixedLengthAngle angleAndLength;
  FixedPlanarPointQ12 nextPosition;
  ModelRuntimeSlot *blockingModelRuntime;
  ModelRuntimeNode *blockedRootNode;
  ModelRuntimeNode *placedRootNode;
  Q12 heightOffsetQ12;

  routeDeltaY = waypointWorldYQ12 - (rootNode->worldTransform).translation.y;
  routeDeltaX = waypointWorldXQ12 - (rootNode->worldTransform).translation.x;
  if ((routeDeltaX == 0) && (routeDeltaY == 0)) {
    angleAndLength = (FixedLengthAngle){ .length = 0, .angle = (rootNode->modelPayload).worldRotationAngle2 };
  }
  else {
    angleAndLength = FixedMath_Vector2AngleAndLength(routeDeltaY,routeDeltaX);
  }
  desiredHeading = angleAndLength.angle;
  movementDefinition = modelRuntime->modelDefinition;
  facingAngle = TrackedMovement_TurnTowardsHeading
                  (modelRuntime,movementDefinition,(rootNode->modelPayload).worldRotationAngle2,desiredHeading);
  rootNode = modelRuntime->rootModelNode;
  facingAngle = facingAngle & FIXED_ANGLE16_MASK;
  if (facingAngle != (rootNode->modelPayload).worldRotationAngle2) {
    (rootNode->modelPayload).worldRotationAngle2 = facingAngle;
    rootNode->runtimeFlags = rootNode->runtimeFlags | 1;
  }
  /* allowed heading error: the far limit, blended towards the near limit inside the interpolation distance */
  targetDistance = angleAndLength.length;
  headingErrorLimit = movementDefinition->farHeadingErrorLimitAngle;
  headingError = facingAngle - desiredHeading & FIXED_ANGLE16_MASK;
  if ((int)targetDistance < movementDefinition->headingErrorInterpolationDistanceQ12) {
    headingErrorLimit = movementDefinition->nearHeadingErrorLimitAngle +
             (int)(((int64_t)(int)(headingErrorLimit - movementDefinition->nearHeadingErrorLimitAngle) *
                   (int64_t)(int)targetDistance) /
                  (int64_t)movementDefinition->headingErrorInterpolationDistanceQ12);
  }
  if ((headingErrorLimit < headingError) && (headingError < FIXED_ANGLE16_FULL_TURN - headingErrorLimit)) {
    (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
    return TrackedMovement_PlaceStationary(worldRuntime,modelRuntime);
  }
  ArmyRuntime_UpdateActivationMetricAndPlayStartSound
            (worldRuntime,(ModelRuntimeSlot *)modelRuntime);
  /* drive forward, or jump onto the route point once it is closer than twice this tick's travel */
  travelDistance = (modelRuntime->movementControl).movementAdvancePerTickQ12 * g_InGameSimulationStepTicks;
  if (travelDistance < (int)targetDistance >> 1) {
    nextPosition = FixedTrig_ProjectPlanarPoint
                       ((rootNode->worldTransform).translation.x,(rootNode->worldTransform).translation.y,
                        travelDistance,(rootNode->modelPayload).worldRotationAngle2);
  }
  else {
    ownerArmy = modelRuntime->ownerArmyRuntime;
    ArmyRuntime_UpdateMovementAndWaypoints
              (worldRuntime,(ArmyMovementRuntime *)ownerArmy,&waypointWorldXQ12,&waypointWorldYQ12);
    nextPosition.xQ12 = waypointWorldXQ12;
    nextPosition.yQ12 = waypointWorldYQ12;
    ownerMovementFlags = &ownerArmy->movementStateFlags;
    *ownerMovementFlags = *ownerMovementFlags | ARMY_MOVEMENT_ROUTE_POINT_REACHED;
  }
  placementContactKind = movementDefinition->placementContactKindIndex;
  rootNode = modelRuntime->rootModelNode;
  heightOffsetQ12 = movementDefinition->placementHeightOffsetQ12;
  blockingModelRuntime = ArmyCollision_FindBlockingRuntimeForCurrentUnit
                     (nextPosition.yQ12,nextPosition.xQ12,
                      (RuntimeCollisionQueryView *)modelRuntime,worldRuntime);
  if (blockingModelRuntime != NULL) {
    /* stay where we are and let the collision partner react */
    blockedRootNode = modelRuntime->rootModelNode;
    ArmyRuntime_HandleCollisionPartner
              ((ModelRuntimeSlot *)modelRuntime,(blockedRootNode->worldTransform).translation.y,
               (blockedRootNode->worldTransform).translation.x,blockingModelRuntime,
               worldRuntime);
    (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
    nextPosition.xQ12 = (blockedRootNode->worldTransform).translation.x;
    nextPosition.yQ12 = (blockedRootNode->worldTransform).translation.y;
    ownerMovementFlags = &modelRuntime->ownerArmyRuntime->movementStateFlags;
    *ownerMovementFlags = *ownerMovementFlags | ARMY_MOVEMENT_ROUTE_POINT_REACHED;
  }
  ownerArmy = modelRuntime->ownerArmyRuntime;
  placedRootNode = modelRuntime->rootModelNode;
  g_ArmyPlacementContactKindDispatchTable.callbacks[placementContactKind]
            (heightOffsetQ12,nextPosition.yQ12,nextPosition.xQ12,rootNode,worldRuntime);
  /* recoil after a shot: actionVector1Q12 counts the remaining ticks, actionVector2Q12 is the tilt per tick */
  remainingRecoilTicks = ownerArmy->actionVector1Q12 - 1;
  if (remainingRecoilTicks >= 0) {
    recoilTilt = remainingRecoilTicks * ownerArmy->actionVector2Q12;
    ownerArmy->actionVector1Q12 = ownerArmy->actionVector1Q12 - 1;
    TrackedMovement_ApplyRecoilTilt(modelRuntime,placedRootNode,recoilTilt);
  }
  return placedRootNode;
}

/* Helper for ArmyRuntimeClass_UpdateGroundMovementCollisionAndTrackAnimation: the signed distance one track
   side travelled this tick. The side point lies localBoundsY0Q12 to the side of the centre in the direction
   currentSideHeading now and previousSideHeading at the start of the tick (heading -+ 90 degrees); the
   distance is negative when that side moved backwards. */
static uint32_t TrackedMovement_MeasureTrackSideTravel
          (ModelRuntimeNode *rootNode,ModelResource *nodeModelResource,uint32_t currentSideHeading,
          uint32_t previousSideHeading,int previousWorldX,int previousWorldY)

{
  FixedSinCos sinCosOffset;
  FixedLengthAngle sideTravel;
  int sidePointX;
  int sidePointY;
  uint32_t trackDistance;
  uint32_t travelHeadingOffset;

  sinCosOffset = FixedMath_SinCosScaled(currentSideHeading,nodeModelResource->localBoundsY0Q12);
  sidePointX = sinCosOffset.cosValue + (rootNode->worldTransform).translation.x;
  sidePointY = sinCosOffset.sinValue + (rootNode->worldTransform).translation.y;
  sinCosOffset = FixedMath_SinCosScaled(previousSideHeading,nodeModelResource->localBoundsY0Q12);
  sideTravel = FixedMath_Vector2AngleAndLength
                 (sidePointY - (sinCosOffset.sinValue + previousWorldY),
                  sidePointX - (sinCosOffset.cosValue + previousWorldX));
  trackDistance = sideTravel.length;
  travelHeadingOffset = sideTravel.angle - (rootNode->modelPayload).worldRotationAngle2 & FIXED_ANGLE16_MASK;
  if ((FIXED_ANGLE16_QUARTER_TURN < travelHeadingOffset) &&
      (travelHeadingOffset < FIXED_ANGLE16_THREE_QUARTER_TURN)) {
    trackDistance = -trackDistance;
  }
  return trackDistance;
}

/* Wraps a track texture offset back into +-ARMY_TRACK_TEXTURE_U_WRAP. */
static int TrackedMovement_WrapTrackTextureOffset(int textureOffsetU)

{
  if (ARMY_TRACK_TEXTURE_U_WRAP + 1 <= textureOffsetU) {
    return textureOffsetU - ARMY_TRACK_TEXTURE_U_WRAP;
  }
  if (textureOffsetU < -ARMY_TRACK_TEXTURE_U_WRAP) {
    return textureOffsetU + ARMY_TRACK_TEXTURE_U_WRAP;
  }
  return textureOffsetU;
}

/* Helper for ArmyRuntimeClass_UpdateGroundMovementCollisionAndTrackAnimation: scrolls the texture of each
   track side by the signed distance that side travelled since the start of the tick. */
static void TrackedMovement_ScrollTrackTextures
          (ModelRuntimeGroundMovementTrackView *modelRuntime,ModelRuntimeNode *rootNode,
          AngleTurn32 previousRotationAngle,int previousWorldX,int previousWorldY)

{
  ModelResource *nodeModelResource;
  uint32_t previousSideHeading;
  uint32_t currentSideHeading;
  uint32_t trackDistance;
  int primaryOffsetU;
  int secondaryOffsetU;

  nodeModelResource = (rootNode->modelPayload).modelResource;
  previousSideHeading = previousRotationAngle - FIXED_ANGLE16_QUARTER_TURN & FIXED_ANGLE16_MASK;
  currentSideHeading = (rootNode->modelPayload).worldRotationAngle2 - FIXED_ANGLE16_QUARTER_TURN & FIXED_ANGLE16_MASK;
  /* side at heading - 90 degrees */
  trackDistance = TrackedMovement_MeasureTrackSideTravel
                    (rootNode,nodeModelResource,currentSideHeading,previousSideHeading,previousWorldX,
                     previousWorldY);
  rootNode->primaryTextureOffsetU =
       rootNode->primaryTextureOffsetU + trackDistance * modelRuntime->modelDefinition->trackTextureUScalePerDistance;
  /* the other side (heading + 90 degrees) */
  trackDistance = TrackedMovement_MeasureTrackSideTravel
                    (rootNode,nodeModelResource,currentSideHeading ^ FIXED_ANGLE16_HALF_TURN,
                     previousSideHeading ^ FIXED_ANGLE16_HALF_TURN,previousWorldX,previousWorldY);
  primaryOffsetU = rootNode->primaryTextureOffsetU;
  secondaryOffsetU = trackDistance * modelRuntime->modelDefinition->trackTextureUScalePerDistance +
                     rootNode->secondaryTextureOffsetU;
  rootNode->secondaryTextureOffsetU = TrackedMovement_WrapTrackTextureOffset(secondaryOffsetU);
  rootNode->primaryTextureOffsetU = TrackedMovement_WrapTrackTextureOffset(primaryOffsetU);
}


/* Ground movement of tracked vehicles (runtimeUpdate slot 2 of g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes,
   called by model class from ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive). Moves exactly like
   ArmyRuntimeClass_UpdateGroundMovement and then scrolls the texture of the left and right track by the
   signed distance each track side travelled this tick (so the tracks also run while turning on the spot),
   wrapping the offsets at +-0x100000.
*/

void ArmyRuntimeClass_UpdateGroundMovementCollisionAndTrackAnimation
          (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementTrackView *modelRuntime)

{
  ArmyMovementStateFlags *ownerMovementFlags;
  ArmyRuntimeSlot *ownerArmy;
  ModelRuntimeSlot *linkedModelRuntime;
  AngleTurn32 previousRotationAngle;
  int previousWorldX;
  int previousWorldY;
  ModelDefinitionGroundMovementTrackView *movementDefinition;
  int32_t waterDelta;
  int waterDamage;
  ModelRuntimeNode *placedRootNode;
  Q12 waypointWorldXQ12;
  Q12 waypointWorldYQ12;
  ModelRuntimeNode *rootNode;

  linkedModelRuntime = (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.modelRuntime;
  rootNode = modelRuntime->rootModelNode;
  ownerMovementFlags = &modelRuntime->ownerArmyRuntime->movementStateFlags;
  *ownerMovementFlags = *ownerMovementFlags | ARMY_MOVEMENT_STATIONARY;
  /* Drop the linked model (classState.linkedArmyRuntimeOrSavedOffset) unless both definitions have a footprint
     radius and this unit is still within it. */
  if ((linkedModelRuntime != NULL) &&
      ((linkedModelRuntime->definitionOrSavedId.runtimeDefinition->footprintRadius == 0) ||
       (modelRuntime->modelDefinition->footprintRadius == 0) ||
       !ArmyCollision_TestPointWithinExpandedRuntimeRadius
                  (modelRuntime->modelDefinition->footprintRadius,(rootNode->worldTransform).translation.y,
                   (rootNode->worldTransform).translation.x,linkedModelRuntime))) {
    (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.modelRuntime = NULL;
  }
  previousRotationAngle = (rootNode->modelPayload).worldRotationAngle2;
  previousWorldX = (rootNode->worldTransform).translation.x;
  previousWorldY = (rootNode->worldTransform).translation.y;
  movementDefinition = modelRuntime->modelDefinition;
  waterDelta = FieldGrid_InterpolateWaterDelta(previousWorldY,previousWorldX,worldRuntime->fieldGrid);
  if (movementDefinition->waterDamageThreshold < waterDelta) {
    waterDamage = waterDelta * movementDefinition->waterDamageMultiplier >> 7;
    if (-1 < waterDamage) {
      ArmyRuntime_ApplyDamageAndPropagateToParent(waterDamage,(ModelRuntimeSlot *)modelRuntime);
    }
  }
  /* alive and still on its way to a route point */
  if ((((modelRuntime->classState).stateFlags & ARMY_RUNTIME_FLAG_DESTROYED) == 0) &&
      !ArmyRuntime_UpdateMovementAndWaypoints
         (worldRuntime,(ArmyMovementRuntime *)modelRuntime->ownerArmyRuntime,&waypointWorldXQ12,
          &waypointWorldYQ12)) {
    placedRootNode = TrackedMovement_SteerAndDrive
                       (worldRuntime,modelRuntime,rootNode,waypointWorldXQ12,waypointWorldYQ12);
  }
  else {
    (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
    (modelRuntime->movementControl).turnVelocityAngle16 = 0;
    placedRootNode = TrackedMovement_PlaceStationary(worldRuntime,modelRuntime);
  }
  /* Track animation */
  TrackedMovement_ScrollTrackTextures
            (modelRuntime,placedRootNode,previousRotationAngle,previousWorldX,previousWorldY);
  if (((previousWorldX != (placedRootNode->worldTransform).translation.x) ||
      (previousWorldY != (placedRootNode->worldTransform).translation.y)) ||
     (previousRotationAngle != (placedRootNode->modelPayload).worldRotationAngle2)) {
    ownerArmy = modelRuntime->ownerArmyRuntime;
    ArmyRuntime_UpdateTimedShotAndEffectEmitters
              (worldRuntime,(ModelRuntimeUpdateView *)modelRuntime);
    ownerMovementFlags = &ownerArmy->movementStateFlags;
    *ownerMovementFlags = *ownerMovementFlags & ~ARMY_MOVEMENT_STATIONARY;
  }
  movementDefinition = modelRuntime->modelDefinition;
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ModelRuntimeSlot *)modelRuntime);
  ModelNodeRuntime_RebuildTransformsFromRoot(placedRootNode);
  ModelNodeRuntime_UpdateDepthBinMasks(movementDefinition->footprintRadius,placedRootNode);
  return;
}


/* Helper for ArmyRuntimeClass_UpdateMovementBankingAndChildAnimation: drops the linked model
   (classState.linkedArmyRuntimeOrSavedOffset) unless both definitions have a footprint radius and this unit is
   still within it. When the link is dropped while class state bit 4 is set and the root node has at least three
   children, the child-part animation starts and the definition's sound positionedSoundSlotIndex is played. */
static void ArmyRuntimeClass_ReleaseLinkedModelOutsideFootprint
          (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView *modelRuntime)
{
  ModelRuntimeSlot *linkedModelRuntime;
  ModelRuntimeNode *rootNode;
  ModelDefinitionGroundMovementSteeringView *movementDefinition;
  uint32_t childCount;
  uint32_t soundIndex;
  DirectSoundVoiceSet **voiceSetRef;
  GraphicsFixedVec3 *worldPosition;
  uint32_t *classStateWord;

  linkedModelRuntime = (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.modelRuntime;
  rootNode = modelRuntime->rootModelNode;
  if (linkedModelRuntime == NULL) {
    return;
  }
  movementDefinition = modelRuntime->modelDefinition;
  if ((linkedModelRuntime->definitionOrSavedId.runtimeDefinition->footprintRadius != 0) &&
      (movementDefinition->footprintRadius != 0) &&
      ArmyCollision_TestPointWithinExpandedRuntimeRadius
                (movementDefinition->footprintRadius,(rootNode->worldTransform).translation.y,
                 (rootNode->worldTransform).translation.x,linkedModelRuntime)) {
    return;
  }
  childCount = rootNode->childCount;
  (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.modelRuntime = NULL;
  if ((childCount < 3) || (((modelRuntime->classState).behaviorState & 4) == 0)) {
    return;
  }
  classStateWord = &(modelRuntime->classState).behaviorState;
  *classStateWord = *classStateWord | 1;
  soundIndex = ((ModelDefinition *)movementDefinition)->positionedSoundSlotIndex;
  if ((soundIndex == 0) || (soundIndex >= worldRuntime->dwordArrayCount) || (worldRuntime->dwordArray == NULL)) {
    return;
  }
  voiceSetRef = (DirectSoundVoiceSet **)worldRuntime->dwordArray[soundIndex];
  if (voiceSetRef == NULL) {
    return;
  }
  worldPosition = &(modelRuntime->rootModelNode->worldTransform).translation;
  if (!TerrainGrid_TestProjectedCellMaskBits01
                ((modelRuntime->rootModelNode->worldTransform).translation.y,worldPosition->x,worldRuntime)) {
    SpatialSound_PlayPositionedOneShot
              (movementDefinition->positionedSoundMaximumDistanceQ12,movementDefinition->positionedSoundGainQ15,
               worldPosition,voiceSetRef);
  }
}

/* Helper for ArmyRuntimeClass_UpdateMovementBankingAndChildAnimation: counts down classState70 (clearing
   behaviour bits 0-2 when it runs out) and spins the three child parts by one step. */
static void ArmyRuntimeClass_SpinBankingChildParts
          (ModelRuntimeGroundMovementSteeringView *modelRuntime,ModelRuntimeNode *rootNode)
{
  uint32_t *classStateWord;
  ModelRuntimeNode *secondChildNode;
  ModelRuntimeNode *thirdChildNode;
  AngleTurn32 *childRotationAngle;

  classStateWord = &(modelRuntime->classLinkState).classState70;
  *classStateWord = *classStateWord - 1;
  if (*classStateWord == 0) {
    classStateWord = &(modelRuntime->classState).behaviorState;
    *classStateWord = *classStateWord & ~7u;
  }
  secondChildNode = rootNode->childNodes[1];
  thirdChildNode = rootNode->childNodes[2];
  childRotationAngle = &(rootNode->childNodes[0]->modelPayload).localRotationAngle1;
  *childRotationAngle = *childRotationAngle + ARMY_SPIN_CHILD_STEP_ANGLE16;
  childRotationAngle = &(secondChildNode->modelPayload).localRotationAngle1;
  *childRotationAngle = *childRotationAngle + ARMY_SPIN_CHILD_STEP_ANGLE16;
  childRotationAngle = &(thirdChildNode->modelPayload).localRotationAngle1;
  *childRotationAngle = *childRotationAngle + ARMY_SPIN_CHILD_STEP_ANGLE16;
}

/* Helper for ArmyRuntimeClass_SteerBankingHeading: stops turning and keeps the current model heading. */
static uint32_t ArmyRuntimeClass_HoldBankingHeading(ModelRuntimeGroundMovementSteeringView *modelRuntime)
{
  (modelRuntime->movementControl).turnVelocityAngle16 = 0;
  return (modelRuntime->rootModelNode->modelPayload).worldRotationAngle2;
}

/* Helper for ArmyRuntimeClass_UpdateMovementBankingAndChildAnimation: updates the turn velocity and returns
   the new (unmasked) heading. Close to the route point and not turning, the heading is kept; a turn against
   the current turn direction also stops and keeps the heading; once the remaining error is within the turn
   velocity it snaps to the desired heading; otherwise it turns on and accelerates up to the turn rate limit. */
static uint32_t ArmyRuntimeClass_SteerBankingHeading
          (ModelRuntimeGroundMovementSteeringView *modelRuntime,ModelRuntimeNode *rootNode,
           ModelDefinitionGroundMovementSteeringView *movementDefinition,FixedLengthAngle targetAngleLength)
{
  uint32_t turnVelocity;
  AngleTurn32 currentHeading;
  uint32_t headingError;
  int turnLimit;
  int acceleratedVelocity;
  uint32_t newHeading;

  turnVelocity = (modelRuntime->movementControl).turnVelocityAngle16;
  if ((turnVelocity == 0) &&
      (targetAngleLength.length < (uint32_t)movementDefinition->headingErrorInterpolationDistanceQ12)) {
    return ArmyRuntimeClass_HoldBankingHeading(modelRuntime);
  }
  currentHeading = (rootNode->modelPayload).worldRotationAngle2;
  headingError = targetAngleLength.angle - currentHeading & FIXED_ANGLE16_MASK;
  if (headingError < FIXED_ANGLE16_HALF_TURN) {
    /* turn left (positive) */
    if ((int)turnVelocity < 0) {
      return ArmyRuntimeClass_HoldBankingHeading(modelRuntime);
    }
    if (headingError <= turnVelocity) {
      (modelRuntime->movementControl).turnVelocityAngle16 = 0;
      return targetAngleLength.angle;
    }
    turnLimit = movementDefinition->turnRateLimitAnglePerTick * g_InGameSimulationStepTicks;
    acceleratedVelocity = turnVelocity + movementDefinition->turnRateAccelerationAnglePerTick * g_InGameSimulationStepTicks;
    (modelRuntime->movementControl).turnVelocityAngle16 = turnLimit;
    newHeading = currentHeading + turnVelocity;
    if (acceleratedVelocity < turnLimit) {
      (modelRuntime->movementControl).turnVelocityAngle16 = acceleratedVelocity;
    }
    return newHeading;
  }
  /* turn right (negative) */
  if (0 < (int)turnVelocity) {
    return ArmyRuntimeClass_HoldBankingHeading(modelRuntime);
  }
  if (turnVelocity + FIXED_ANGLE16_FULL_TURN <= headingError) {
    (modelRuntime->movementControl).turnVelocityAngle16 = 0;
    return targetAngleLength.angle;
  }
  newHeading = currentHeading + turnVelocity;
  turnLimit = -movementDefinition->turnRateLimitAnglePerTick * g_InGameSimulationStepTicks;
  acceleratedVelocity = turnVelocity - movementDefinition->turnRateAccelerationAnglePerTick * g_InGameSimulationStepTicks;
  (modelRuntime->movementControl).turnVelocityAngle16 = turnLimit;
  if (turnLimit < acceleratedVelocity) {
    (modelRuntime->movementControl).turnVelocityAngle16 = acceleratedVelocity;
  }
  return newHeading;
}

/* Helper for ArmyRuntimeClass_UpdateMovementBankingAndChildAnimation: places the root node with the
   definition's placement callback at its current position. */
static void ArmyRuntimeClass_PlaceBankingUnitInPlace
          (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView *modelRuntime,
           ModelRuntimeNode *rootNode)
{
  (*g_ArmyPlacementContactKindDispatchTable.callbacks
    [modelRuntime->modelDefinition->placementContactKindIndex])
            (modelRuntime->modelDefinition->placementHeightOffsetQ12,
             (rootNode->worldTransform).translation.y,
             (rootNode->worldTransform).translation.x,rootNode,worldRuntime);
}

/* Helper for ArmyRuntimeClass_UpdateMovementBankingAndChildAnimation: tilts the root node back by the recoil
   after a shot, as in ArmyRuntimeClass_UpdateGroundMovement. */
static void ArmyRuntimeClass_ApplyBankingRecoilTilt
          (ModelRuntimeGroundMovementSteeringView *modelRuntime,ModelRuntimeNode *rootNode,int recoilTilt)
{
  FixedAzimuthElevationRoll composedAngles;

  composedAngles = FixedTransform_ComposeEulerAngles
                     (0,FIXED_ANGLE16_QUARTER_TURN - recoilTilt,
                      (modelRuntime->ownerArmyRuntime->actionVector0Q12 + FIXED_ANGLE16_HALF_TURN) -
                      (rootNode->modelPayload).worldRotationAngle2 & FIXED_ANGLE16_MASK,
                      (rootNode->modelPayload).worldRotationAngle2,
                      (rootNode->modelPayload).worldRotationAngle1,
                      (rootNode->modelPayload).worldRotationAngle0);
  (rootNode->modelPayload).worldRotationAngle0 = composedAngles.azimuthAngle;
  (rootNode->modelPayload).worldRotationAngle1 = composedAngles.elevationAngle;
  (rootNode->modelPayload).worldRotationAngle2 = composedAngles.rollAngle;
}

/* Helper for ArmyRuntimeClass_UpdateMovementBankingAndChildAnimation: the unit is destroyed or has arrived.
   Stops it, places it where it stands and applies a pending recoil. Returns the root node. */
static ModelRuntimeNode *ArmyRuntimeClass_PlaceBankingUnitStationary
          (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView *modelRuntime)
{
  ArmyRuntimeSlot *ownerArmy;
  ModelRuntimeNode *rootNode;
  int recoilTilt;

  (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
  (modelRuntime->movementControl).turnVelocityAngle16 = 0;
  ownerArmy = modelRuntime->ownerArmyRuntime;
  rootNode = modelRuntime->rootModelNode;
  recoilTilt = ownerArmy->actionVector1Q12 - 1;
  if (recoilTilt < 0) {
    ArmyRuntimeClass_PlaceBankingUnitInPlace(worldRuntime,modelRuntime,rootNode);
    return rootNode;
  }
  recoilTilt = recoilTilt * ownerArmy->actionVector2Q12;
  ownerArmy->actionVector1Q12 = ownerArmy->actionVector1Q12 - 1;
  ArmyRuntimeClass_PlaceBankingUnitInPlace(worldRuntime,modelRuntime,rootNode);
  ArmyRuntimeClass_ApplyBankingRecoilTilt(modelRuntime,rootNode,recoilTilt);
  return rootNode;
}

/* Helper for ArmyRuntimeClass_UpdateMovementBankingAndChildAnimation: turns towards the route point
   (waypointWorldX, waypointWorldY), slides straight towards it, stops at blocking armies, places the unit and
   applies a pending recoil. Stores the direction and distance to the route point in *targetAngleLength and
   returns the root node. */
static ModelRuntimeNode *ArmyRuntimeClass_MoveBankingUnitTowardsRoutePoint
          (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView *modelRuntime,
           ModelRuntimeNode *rootNode,uint32_t waypointWorldX,Q12 waypointWorldY,
           FixedLengthAngle *targetAngleLength)
{
  ModelDefinitionGroundMovementSteeringView *movementDefinition;
  ArmyRuntimeSlot *ownerArmy;
  ArmyMovementStateFlags *ownerMovementFlags;
  ModelRuntimeNode *collisionRootNode;
  ModelRuntimeSlot *blockingModelRuntime;
  ArmyPlacementContactKindIndex32 placementContactKind;
  Q12 heightOffsetQ12;
  FixedPlanarPointQ12 nextPosition;
  Q12 reachedWorldXQ12;
  Q12 reachedWorldYQ12;
  AngleTurn32 desiredHeading;
  uint32_t newHeading;
  int deltaX;
  int deltaY;
  int travelDistance;
  int recoilTilt;

  /* a new route point (classState68/armyLinkOrState) restarts from standstill */
  if ((waypointWorldX != (modelRuntime->classLinkState).classState68) &&
     ((uint32_t)waypointWorldY != (modelRuntime->classLinkState).armyLinkOrState.classState)) {
    (modelRuntime->classLinkState).classState68 = waypointWorldX;
    (modelRuntime->classLinkState).armyLinkOrState.classState = (uint32_t)waypointWorldY;
    (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
    (modelRuntime->movementControl).turnVelocityAngle16 = 0;
  }
  deltaX = waypointWorldX - (rootNode->worldTransform).translation.x;
  deltaY = waypointWorldY - (rootNode->worldTransform).translation.y;
  if ((deltaX == 0) && (deltaY == 0)) {
    *targetAngleLength = (FixedLengthAngle){ .length = 0, .angle = (rootNode->modelPayload).worldRotationAngle2 };
  }
  else {
    *targetAngleLength = FixedMath_Vector2AngleAndLength(deltaY,deltaX);
  }
  desiredHeading = targetAngleLength->angle;
  movementDefinition = modelRuntime->modelDefinition;
  newHeading = ArmyRuntimeClass_SteerBankingHeading(modelRuntime,rootNode,movementDefinition,*targetAngleLength);
  rootNode = modelRuntime->rootModelNode;
  if ((newHeading & FIXED_ANGLE16_MASK) != (rootNode->modelPayload).worldRotationAngle2) {
    (rootNode->modelPayload).worldRotationAngle2 = newHeading & FIXED_ANGLE16_MASK;
    rootNode->runtimeFlags = rootNode->runtimeFlags | 1;
  }
  ArmyRuntime_UpdateActivationMetricAndPlayStartSound
            (worldRuntime,(ModelRuntimeSlot *)modelRuntime);
  /* slide straight towards the route point (desiredHeading, not the model heading) */
  travelDistance = (modelRuntime->movementControl).movementAdvancePerTickQ12 * g_InGameSimulationStepTicks;
  if (travelDistance < (int)targetAngleLength->length >> 1) {
    nextPosition = FixedTrig_ProjectPlanarPoint
                       ((rootNode->worldTransform).translation.x,(rootNode->worldTransform).translation.y,
                        travelDistance,desiredHeading);
  }
  else {
    ownerArmy = modelRuntime->ownerArmyRuntime;
    ArmyRuntime_UpdateMovementAndWaypoints
              (worldRuntime,(ArmyMovementRuntime *)ownerArmy,&reachedWorldXQ12,&reachedWorldYQ12);
    nextPosition.xQ12 = reachedWorldXQ12;
    nextPosition.yQ12 = reachedWorldYQ12;
    ownerMovementFlags = &ownerArmy->movementStateFlags;
    *ownerMovementFlags = *ownerMovementFlags | ARMY_MOVEMENT_ROUTE_POINT_REACHED;
  }
  placementContactKind = movementDefinition->placementContactKindIndex;
  rootNode = modelRuntime->rootModelNode;
  heightOffsetQ12 = movementDefinition->placementHeightOffsetQ12;
  blockingModelRuntime = ArmyCollision_FindBlockingRuntimeForCurrentUnit
                     (nextPosition.yQ12,nextPosition.xQ12,
                      (RuntimeCollisionQueryView *)modelRuntime,worldRuntime);
  if (blockingModelRuntime != NULL) {
    collisionRootNode = modelRuntime->rootModelNode;
    ArmyRuntime_HandleCollisionPartner
              ((ModelRuntimeSlot *)modelRuntime,(collisionRootNode->worldTransform).translation.y,
               (collisionRootNode->worldTransform).translation.x,blockingModelRuntime,
               worldRuntime);
    (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
    nextPosition.xQ12 = (collisionRootNode->worldTransform).translation.x;
    nextPosition.yQ12 = (collisionRootNode->worldTransform).translation.y;
    ownerMovementFlags = &modelRuntime->ownerArmyRuntime->movementStateFlags;
    *ownerMovementFlags = *ownerMovementFlags | ARMY_MOVEMENT_ROUTE_POINT_REACHED;
  }
  ownerArmy = modelRuntime->ownerArmyRuntime;
  g_ArmyPlacementContactKindDispatchTable.callbacks[placementContactKind]
            (heightOffsetQ12,nextPosition.yQ12,nextPosition.xQ12,rootNode,worldRuntime);
  rootNode = modelRuntime->rootModelNode;
  /* recoil after a shot, as in ArmyRuntimeClass_UpdateGroundMovement */
  recoilTilt = ownerArmy->actionVector1Q12 - 1;
  if (recoilTilt >= 0) {
    recoilTilt = recoilTilt * ownerArmy->actionVector2Q12;
    ownerArmy->actionVector1Q12 = ownerArmy->actionVector1Q12 - 1;
    ArmyRuntimeClass_ApplyBankingRecoilTilt(modelRuntime,rootNode,recoilTilt);
  }
  return rootNode;
}

/* Helper for ArmyRuntimeClass_UpdateMovementBankingAndChildAnimation. Banking: classLinkState.modelLinkOrState
   is the bank value, ARMY_GLIDER_BANK_LEVEL_ANGLE16 = level, classState64 the bank heading. Applies the
   current bank to the root node, then updates bank value and bank heading for the next tick.
   targetAngleLength is only read while the unit moves (it is not set when the unit stands). */
static void ArmyRuntimeClass_UpdateBankAngle
          (ModelRuntimeGroundMovementSteeringView *modelRuntime,ModelRuntimeNode *rootNode,
           const FixedLengthAngle *targetAngleLength)
{
  ModelDefinitionGroundMovementSteeringView *movementDefinition;
  FixedAzimuthElevationRoll composedAngles;
  uint32_t bankHeadingStep;

  movementDefinition = modelRuntime->modelDefinition;
  if ((modelRuntime->classLinkState).modelLinkOrState.classState != ARMY_GLIDER_BANK_LEVEL_ANGLE16) {
    composedAngles = FixedTransform_ComposeEulerAngles
                       ((rootNode->modelPayload).worldRotationAngle2,
                        (rootNode->modelPayload).worldRotationAngle1,
                        (rootNode->modelPayload).worldRotationAngle0,0,
                        (modelRuntime->classLinkState).modelLinkOrState.classState,
                        (modelRuntime->classLinkState).classState64);
    (rootNode->modelPayload).worldRotationAngle0 = composedAngles.azimuthAngle;
    (rootNode->modelPayload).worldRotationAngle1 = composedAngles.elevationAngle;
    (rootNode->modelPayload).worldRotationAngle2 = composedAngles.rollAngle;
  }
  if (((modelRuntime->movementControl).movementAdvancePerTickQ12 == 0) ||
     ((int)targetAngleLength->length <= movementDefinition->movementStepQ12PerTick * 32)) {
    /* stopped or within 32 steps of the route point: bank value rises, up to level */
    (modelRuntime->classLinkState).modelLinkOrState.classState =
         (modelRuntime->classLinkState).modelLinkOrState.classState + ARMY_GLIDER_BANK_RECOVER_ANGLE16;
    if (ARMY_GLIDER_BANK_LEVEL_ANGLE16 < (modelRuntime->classLinkState).modelLinkOrState.signedScalarState) {
      (modelRuntime->classLinkState).modelLinkOrState.classState = ARMY_GLIDER_BANK_LEVEL_ANGLE16;
    }
    return;
  }
  /* moving: turn the bank heading towards the travel direction by at most
     ARMY_GLIDER_BANK_TURN_LIMIT_ANGLE16, the bank value drops down to ARMY_GLIDER_BANK_MAX_ANGLE16 */
  bankHeadingStep = targetAngleLength->angle - (modelRuntime->classLinkState).classState64 & FIXED_ANGLE16_MASK;
  if (bankHeadingStep < FIXED_ANGLE16_HALF_TURN + 1) {
    if (ARMY_GLIDER_BANK_TURN_LIMIT_ANGLE16 < bankHeadingStep) {
      bankHeadingStep = ARMY_GLIDER_BANK_TURN_LIMIT_ANGLE16;
    }
  }
  else if (bankHeadingStep < FIXED_ANGLE16_FULL_TURN - ARMY_GLIDER_BANK_TURN_LIMIT_ANGLE16) {
    bankHeadingStep = (uint32_t)-ARMY_GLIDER_BANK_TURN_LIMIT_ANGLE16;
  }
  (modelRuntime->classLinkState).modelLinkOrState.classState =
       (modelRuntime->classLinkState).modelLinkOrState.classState - ARMY_GLIDER_BANK_STEP_ANGLE16;
  (modelRuntime->classLinkState).classState64 =
       bankHeadingStep + (modelRuntime->classLinkState).classState64 & FIXED_ANGLE16_MASK;
  if ((modelRuntime->classLinkState).modelLinkOrState.signedScalarState < ARMY_GLIDER_BANK_MAX_ANGLE16) {
    (modelRuntime->classLinkState).modelLinkOrState.classState = ARMY_GLIDER_BANK_MAX_ANGLE16;
  }
}

/* Movement of banking units with three animated child parts (runtimeUpdate slot 17 of
   g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes, called by model class from
   ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive). The unit slides straight towards the route
   point while it turns (turning like ArmyRuntimeClass_UpdateGroundMovement, but it keeps its heading
   when it is close and not turning), stops at blocking armies and is placed by its placement callback. It then
   banks: while moving the bank value (modelLinkOrState) sinks from 0x4000 (level) to at least 0x3800 and the
   bank heading (classState64) follows the travel direction by at most 0x400 per tick; when stopped it returns
   to level. When the unit leaves its linked army while class state bit 4 is set, it plays the definition's
   positioned sound and spins its three child parts for the countdown classState70.
*/

void ArmyRuntimeClass_UpdateMovementBankingAndChildAnimation
          (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView *modelRuntime
          )

{
  ArmyMovementStateFlags *ownerMovementFlags;
  ModelDefinitionGroundMovementSteeringView *movementDefinition;
  AngleTurn32 previousRotationAngle;
  int previousWorldX;
  int previousWorldY;
  ModelRuntimeNode *rootNode;
  bool waypointArrived;
  Q12 waypointWorldXQ12;
  Q12 waypointWorldYQ12;
  /* set only while the unit moves; read by the bank update only then */
  FixedLengthAngle targetAngleLength;

  ownerMovementFlags = &modelRuntime->ownerArmyRuntime->movementStateFlags;
  *ownerMovementFlags = *ownerMovementFlags | ARMY_MOVEMENT_STATIONARY;
  ArmyRuntimeClass_ReleaseLinkedModelOutsideFootprint(worldRuntime,modelRuntime);
  rootNode = modelRuntime->rootModelNode;
  previousRotationAngle = (rootNode->modelPayload).worldRotationAngle2;
  previousWorldX = (rootNode->worldTransform).translation.x;
  previousWorldY = (rootNode->worldTransform).translation.y;
  if (((modelRuntime->classState).stateFlags & ARMY_RUNTIME_FLAG_DESTROYED) != 0) {
    rootNode = ArmyRuntimeClass_PlaceBankingUnitStationary(worldRuntime,modelRuntime);
  }
  else {
    if (((modelRuntime->classState).behaviorState & 1) != 0) {
      ArmyRuntimeClass_SpinBankingChildParts(modelRuntime,rootNode);
    }
    waypointArrived = ArmyRuntime_UpdateMovementAndWaypoints
                       (worldRuntime,(ArmyMovementRuntime *)modelRuntime->ownerArmyRuntime,&waypointWorldXQ12,
                        &waypointWorldYQ12);
    if (waypointArrived) {
      rootNode = ArmyRuntimeClass_PlaceBankingUnitStationary(worldRuntime,modelRuntime);
    }
    else {
      rootNode = ArmyRuntimeClass_MoveBankingUnitTowardsRoutePoint
                   (worldRuntime,modelRuntime,rootNode,waypointWorldXQ12,waypointWorldYQ12,&targetAngleLength);
    }
  }
  ArmyRuntimeClass_UpdateBankAngle(modelRuntime,rootNode,&targetAngleLength);
  if (((modelRuntime->classState).behaviorState & 2) == 0) {
    ArmyRuntime_UpdateTimedShotAndEffectEmitters
              (worldRuntime,(ModelRuntimeUpdateView *)modelRuntime);
  }
  if (((previousWorldX != (rootNode->worldTransform).translation.x) ||
      (previousWorldY != (rootNode->worldTransform).translation.y)) ||
     (previousRotationAngle != (rootNode->modelPayload).worldRotationAngle2)) {
    ownerMovementFlags = &modelRuntime->ownerArmyRuntime->movementStateFlags;
    *ownerMovementFlags = *ownerMovementFlags & ~ARMY_MOVEMENT_STATIONARY;
  }
  movementDefinition = modelRuntime->modelDefinition;
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ModelRuntimeSlot *)modelRuntime);
  ModelNodeRuntime_RebuildTransformsFromRoot(rootNode);
  ModelNodeRuntime_UpdateDepthBinMasks(movementDefinition->footprintRadius,rootNode);
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
    if (child != NULL) {
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
  bool hasNoWeaponDamage;
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
    armyRuntime->commandTargetArmyRuntime = NULL;
  }
  return;
}


/* Starts a locked, routed move to the target with exactly one queued waypoint, whose position is given
   separately by the two auxiliary values (x = auxiliaryValue0, y = auxiliaryValue1). The target becomes both
   the route end and the final movement target. Called directly from gameplay/army/runtime.c.
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


/* Puts the two feet of an articulated walker at rest beside its root: both feet (current position and step
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
void ArmyArticulatedRuntime_InitializeTerrainContactGeometry
          (ModelRuntimeNode *modelNodeRuntime,WorldRuntimeContext *worldRuntime)

{
  AngleTurn32 rootHeading;
  Q12 terrainHeight;
  uint32_t footX;
  uint32_t footY;
  int lateralOffsetY;
  FixedSinCos lateralOffset;
  Q12 worldXQ12;
  Q12 worldYQ12;
  ArmyArticulatedRuntimeSlotView *articulatedRuntime;

  articulatedRuntime = (ArmyArticulatedRuntimeSlotView *)(modelNodeRuntime->runtimePayload).modelRuntime;
  worldXQ12 = (modelNodeRuntime->worldTransform).translation.x;
  worldYQ12 = (modelNodeRuntime->worldTransform).translation.y;
  terrainHeight = 0;
  if (worldRuntime->fieldGrid != NULL) {
    FieldGrid_InterpolateTerrainHeight(worldYQ12,worldXQ12,worldRuntime->fieldGrid,&terrainHeight);
  }
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
  lateralOffset = FixedMath_SinCosScaled
                    ((modelNodeRuntime->modelPayload).worldRotationAngle2 + FIXED_ANGLE16_QUARTER_TURN & FIXED_ANGLE16_MASK,
                     (articulatedRuntime->articulatedContact).lateralOffsetQ12);
  lateralOffsetY = lateralOffset.sinValue;
  /* left foot: heading + 90 degrees */
  footX = worldXQ12 + lateralOffset.cosValue;
  footY = worldYQ12 + lateralOffsetY;
  articulatedRuntime->movementTarget0Q12 = footX;
  articulatedRuntime->definitionClassValue80 = footY;
  articulatedRuntime->definitionClassValue88 = terrainHeight;
  articulatedRuntime->runtimeState90 = footX;
  articulatedRuntime->runtimeState98 = footY;
  articulatedRuntime->articulatedHeightOrStateA0 = terrainHeight;
  /* right foot */
  footX = worldXQ12 - lateralOffset.cosValue;
  footY = worldYQ12 - lateralOffsetY;
  articulatedRuntime->movementTarget1Q12 = footX;
  articulatedRuntime->definitionClassValue84 = footY;
  articulatedRuntime->runtimeState8C = terrainHeight;
  articulatedRuntime->runtimeState94 = footX;
  articulatedRuntime->articulatedCoordinateOrState9C = footY;
  articulatedRuntime->runtimeStateA4 = terrainHeight;
  rootHeading = (modelNodeRuntime->modelPayload).worldRotationAngle2;
  articulatedRuntime->classState60 = rootHeading;
  articulatedRuntime->ownerValue64 = rootHeading;
  (articulatedRuntime->linkedChildOverloadedState).primaryCoordinateCommandOrHistory.headingOrTurnValue = rootHeading;
  (articulatedRuntime->linkedChildOverloadedState).leftHeadingCommandOrSpawnValue.headingOrTurnValue = rootHeading;
  (articulatedRuntime->linkedChildOverloadedState).secondaryCoordinateCommandOrHistory.headingOrTurnValue = rootHeading;
  (articulatedRuntime->linkedChildOverloadedState).rightHeadingCommandOrSpawnValue.headingOrTurnValue = rootHeading;
  /* ground normals: elevation a quarter turn (straight up), azimuth 0 */
  articulatedRuntime->ownerValue68 = ARMY_ARTICULATED_NORMAL_UP;
  articulatedRuntime->fallbackWorldYQ12 = ARMY_ARTICULATED_NORMAL_UP;
  articulatedRuntime->linkedArmyRuntimeOrSavedOffset = (ArmyRuntimeSlot *)ARMY_ARTICULATED_NORMAL_UP;
  articulatedRuntime->fallbackWorldXQ12 = ARMY_ARTICULATED_NORMAL_UP;
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
   Entities whose definition record has zero at +0x18 ignore the order.
*/
void ArmyRuntime_StartRoutedMoveCommand(Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime)

{
  GraphicsWorldCoordinateQ12 currentWorldX;
  GraphicsWorldCoordinateQ12 currentWorldY;
  WorldRuntimeContext *worldRuntime;
  PathingDestination resolvedDestination;

  if (*(int *)((int)(movementRuntime->entityRuntime->common).ownership.definitionOrClassRecord +
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

  if (*(int *)((int)(movementRuntime->entityRuntime->common).ownership.definitionOrClassRecord +
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


/* Recoil after a shot: tilts the model away from the shot direction (actionVector0Q12 + 180 degrees) by
   recoilTilt. */
static void ArmyGroundMovement_ApplyRecoilTilt
          (ModelRuntimeGroundMovementSteeringView *modelRuntime,ModelRuntimeNode *rootNode,int recoilTilt)

{
  FixedAzimuthElevationRoll composedAngles;

  composedAngles = FixedTransform_ComposeEulerAngles
                     (0,FIXED_ANGLE16_QUARTER_TURN - recoilTilt,
                      (modelRuntime->ownerArmyRuntime->actionVector0Q12 + FIXED_ANGLE16_HALF_TURN) -
                      (rootNode->modelPayload).worldRotationAngle2 & FIXED_ANGLE16_MASK,
                      (rootNode->modelPayload).worldRotationAngle2,
                      (rootNode->modelPayload).worldRotationAngle1,
                      (rootNode->modelPayload).worldRotationAngle0);
  (rootNode->modelPayload).worldRotationAngle0 = composedAngles.azimuthAngle;
  (rootNode->modelPayload).worldRotationAngle1 = composedAngles.elevationAngle;
  (rootNode->modelPayload).worldRotationAngle2 = composedAngles.rollAngle;
}

/* Not driving this tick: re-places the model at its current position (while a recoil runs, or always when
   the field grid flags it) and applies the recoil tilt. actionVector1Q12 counts the remaining recoil ticks,
   actionVector2Q12 is the tilt per tick. Returns the root node used for the rest of the tick. */
static ModelRuntimeNode *ArmyGroundMovement_PlaceStationary
          (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView *modelRuntime)

{
  ArmyRuntimeSlot *ownerArmy;
  ModelRuntimeNode *rootNode;
  int remainingRecoilTicks;
  int recoilTilt;

  ownerArmy = modelRuntime->ownerArmyRuntime;
  rootNode = modelRuntime->rootModelNode;
  remainingRecoilTicks = ownerArmy->actionVector1Q12 - 1;
  if (remainingRecoilTicks < 0) {
    if ((worldRuntime->fieldGrid->runtimeStateFlags & 1) != 0) {
      (*g_ArmyPlacementContactKindDispatchTable.callbacks
        [modelRuntime->modelDefinition->placementContactKindIndex])
                (modelRuntime->modelDefinition->placementHeightOffsetQ12,
                 (rootNode->worldTransform).translation.y,
                 (rootNode->worldTransform).translation.x,rootNode,worldRuntime);
    }
    return rootNode;
  }
  recoilTilt = remainingRecoilTicks * ownerArmy->actionVector2Q12;
  ownerArmy->actionVector1Q12 = ownerArmy->actionVector1Q12 - 1;
  (*g_ArmyPlacementContactKindDispatchTable.callbacks
    [modelRuntime->modelDefinition->placementContactKindIndex])
            (modelRuntime->modelDefinition->placementHeightOffsetQ12,
             (rootNode->worldTransform).translation.y,(rootNode->worldTransform).translation.x
             ,rootNode,worldRuntime);
  ArmyGroundMovement_ApplyRecoilTilt(modelRuntime,rootNode,recoilTilt);
  return rootNode;
}

/* Turns the model towards the waypoint and, if the heading error is small enough, drives towards it (or
   stops at a blocking army), places it with the placement callback and applies the recoil tilt; otherwise
   it stops and is placed stationary. Returns the root node used for the rest of the tick. */
static ModelRuntimeNode *ArmyGroundMovement_SteerAndDrive
          (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView *modelRuntime,
          ModelRuntimeNode *rootNode,Q12 waypointWorldXQ12,Q12 waypointWorldYQ12)

{
  ArmyMovementStateFlags *ownerMovementFlags;
  ArmyRuntimeSlot *ownerArmy;
  ModelDefinitionGroundMovementSteeringView *movementDefinition;
  ArmyPlacementContactKindIndex32 placementContactKind;
  int routeDeltaX;
  int routeDeltaY;
  int turnVelocityLimit;
  int acceleratedTurnVelocity;
  int travelDistance;
  int remainingRecoilTicks;
  int recoilTilt;
  uint32_t facingAngle;
  uint32_t turnVelocity;
  uint32_t headingErrorLimit;
  uint32_t headingError;
  uint32_t desiredHeading;
  uint32_t headingDifference;
  FixedLengthAngle angleAndLength;
  FixedPlanarPointQ12 nextPosition;
  ModelRuntimeSlot *blockingModelRuntime;
  Q12 heightOffsetQ12;
  uint32_t targetDistance;
  ModelRuntimeNode *blockedRootNode;

  routeDeltaY = waypointWorldYQ12 - (rootNode->worldTransform).translation.y;
  routeDeltaX = waypointWorldXQ12 - (rootNode->worldTransform).translation.x;
  if ((routeDeltaX == 0) && (routeDeltaY == 0)) {
    angleAndLength = (FixedLengthAngle){ .length = 0, .angle = (rootNode->modelPayload).worldRotationAngle2 };
  }
  else {
    angleAndLength = FixedMath_Vector2AngleAndLength(routeDeltaY,routeDeltaX);
  }
  /* turn by the current turn velocity, then accelerate it (clamped to the turn rate limit); a turn in the
     other direction first resets the velocity, a small remaining error snaps onto the desired heading */
  desiredHeading = angleAndLength.angle;
  movementDefinition = modelRuntime->modelDefinition;
  facingAngle = (rootNode->modelPayload).worldRotationAngle2;
  turnVelocity = (modelRuntime->movementControl).turnVelocityAngle16;
  headingDifference = desiredHeading - facingAngle & FIXED_ANGLE16_MASK;
  if (headingDifference < FIXED_ANGLE16_HALF_TURN) {
    if ((int)turnVelocity < 0) {
      (modelRuntime->movementControl).turnVelocityAngle16 = 0;
    }
    else if (turnVelocity < headingDifference) {
      facingAngle = facingAngle + turnVelocity;
      turnVelocityLimit = movementDefinition->turnRateLimitAnglePerTick * g_InGameSimulationStepTicks;
      acceleratedTurnVelocity =
           turnVelocity + movementDefinition->turnRateAccelerationAnglePerTick * g_InGameSimulationStepTicks;
      (modelRuntime->movementControl).turnVelocityAngle16 = turnVelocityLimit;
      if (acceleratedTurnVelocity < turnVelocityLimit) {
        (modelRuntime->movementControl).turnVelocityAngle16 = acceleratedTurnVelocity;
      }
    }
    else {
      (modelRuntime->movementControl).turnVelocityAngle16 = 0;
      facingAngle = desiredHeading;
    }
  }
  else if (0 < (int)turnVelocity) {
    (modelRuntime->movementControl).turnVelocityAngle16 = 0;
  }
  else if (turnVelocity + FIXED_ANGLE16_FULL_TURN <= headingDifference) {
    (modelRuntime->movementControl).turnVelocityAngle16 = 0;
    facingAngle = desiredHeading;
  }
  else {
    facingAngle = facingAngle + turnVelocity;
    turnVelocityLimit = -movementDefinition->turnRateLimitAnglePerTick * g_InGameSimulationStepTicks;
    acceleratedTurnVelocity =
         turnVelocity - movementDefinition->turnRateAccelerationAnglePerTick * g_InGameSimulationStepTicks;
    (modelRuntime->movementControl).turnVelocityAngle16 = turnVelocityLimit;
    if (turnVelocityLimit < acceleratedTurnVelocity) {
      (modelRuntime->movementControl).turnVelocityAngle16 = acceleratedTurnVelocity;
    }
  }
  rootNode = modelRuntime->rootModelNode;
  facingAngle = facingAngle & FIXED_ANGLE16_MASK;
  if (facingAngle != (rootNode->modelPayload).worldRotationAngle2) {
    (rootNode->modelPayload).worldRotationAngle2 = facingAngle;
    rootNode->runtimeFlags = rootNode->runtimeFlags | 1;
  }
  /* allowed heading error: the far limit, blended towards the near limit inside the interpolation distance */
  targetDistance = angleAndLength.length;
  headingErrorLimit = movementDefinition->farHeadingErrorLimitAngle;
  headingError = facingAngle - desiredHeading & FIXED_ANGLE16_MASK;
  if ((int)targetDistance < movementDefinition->headingErrorInterpolationDistanceQ12) {
    headingErrorLimit = movementDefinition->nearHeadingErrorLimitAngle +
            (int)(((int64_t)(int)(headingErrorLimit - movementDefinition->nearHeadingErrorLimitAngle) *
                  (int64_t)(int)targetDistance) /
                 (int64_t)movementDefinition->headingErrorInterpolationDistanceQ12);
  }
  if ((headingErrorLimit < headingError) && (headingError < FIXED_ANGLE16_FULL_TURN - headingErrorLimit)) {
    (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
    return ArmyGroundMovement_PlaceStationary(worldRuntime,modelRuntime);
  }
  ArmyRuntime_UpdateActivationMetricAndPlayStartSound
            (worldRuntime,(ModelRuntimeSlot *)modelRuntime);
  /* drive forward, or jump onto the route point once it is closer than twice this tick's travel */
  travelDistance = (modelRuntime->movementControl).movementAdvancePerTickQ12 * g_InGameSimulationStepTicks;
  if (travelDistance < (int)targetDistance >> 1) {
    nextPosition = FixedTrig_ProjectPlanarPoint
                       ((rootNode->worldTransform).translation.x,(rootNode->worldTransform).translation.y,
                        travelDistance,(rootNode->modelPayload).worldRotationAngle2);
  }
  else {
    ownerArmy = modelRuntime->ownerArmyRuntime;
    ArmyRuntime_UpdateMovementAndWaypoints
              (worldRuntime,(ArmyMovementRuntime *)ownerArmy,&waypointWorldXQ12,&waypointWorldYQ12);
    nextPosition.xQ12 = waypointWorldXQ12;
    nextPosition.yQ12 = waypointWorldYQ12;
    ownerMovementFlags = &ownerArmy->movementStateFlags;
    *ownerMovementFlags = *ownerMovementFlags | ARMY_MOVEMENT_ROUTE_POINT_REACHED;
  }
  placementContactKind = movementDefinition->placementContactKindIndex;
  rootNode = modelRuntime->rootModelNode;
  heightOffsetQ12 = movementDefinition->placementHeightOffsetQ12;
  blockingModelRuntime = ArmyCollision_FindBlockingRuntimeForCurrentUnit
                     (nextPosition.yQ12,nextPosition.xQ12,
                      (RuntimeCollisionQueryView *)modelRuntime,worldRuntime);
  if (blockingModelRuntime != NULL) {
    /* stay where we are and let the collision partner react */
    blockedRootNode = modelRuntime->rootModelNode;
    ArmyRuntime_HandleCollisionPartner
              ((ModelRuntimeSlot *)modelRuntime,(blockedRootNode->worldTransform).translation.y,
               (blockedRootNode->worldTransform).translation.x,blockingModelRuntime,
               worldRuntime);
    (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
    nextPosition.xQ12 = (blockedRootNode->worldTransform).translation.x;
    nextPosition.yQ12 = (blockedRootNode->worldTransform).translation.y;
    ownerMovementFlags = &modelRuntime->ownerArmyRuntime->movementStateFlags;
    *ownerMovementFlags = *ownerMovementFlags | ARMY_MOVEMENT_ROUTE_POINT_REACHED;
  }
  ownerArmy = modelRuntime->ownerArmyRuntime;
  g_ArmyPlacementContactKindDispatchTable.callbacks[placementContactKind]
            (heightOffsetQ12,nextPosition.yQ12,nextPosition.xQ12,rootNode,worldRuntime);
  rootNode = modelRuntime->rootModelNode;
  /* recoil after a shot: actionVector1Q12 counts the remaining ticks, actionVector2Q12 is the tilt per tick */
  remainingRecoilTicks = ownerArmy->actionVector1Q12 - 1;
  if (remainingRecoilTicks >= 0) {
    recoilTilt = remainingRecoilTicks * ownerArmy->actionVector2Q12;
    ownerArmy->actionVector1Q12 = ownerArmy->actionVector1Q12 - 1;
    ArmyGroundMovement_ApplyRecoilTilt(modelRuntime,rootNode,recoilTilt);
  }
  return rootNode;
}

/* Standard ground movement (runtimeUpdate slot 1 of g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes, and
   the land case of slot 18 via ArmyRuntimeClass_UpdateSpecialBehaviorAndGroundMovement). Turns the model
   towards the current route point with accelerated turning, drives forward only while the heading error is
   within the definition's limit (the limit narrows near the target), stops at blocking armies, puts the
   model back onto the ground with its placement callback and rocks it back after a shot (action vector).
   Also applies water damage, drops a linked army that is out of reach and does the moved/turned bookkeeping.
*/

void ArmyRuntimeClass_UpdateGroundMovement
          (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView *modelRuntime
          )

{
  ArmyMovementStateFlags *ownerMovementFlags;
  ArmyRuntimeSlot *ownerArmy;
  ModelRuntimeSlot *linkedModelRuntime;
  AngleTurn32 previousRotationAngle;
  int previousWorldX;
  int previousWorldY;
  ModelDefinitionGroundMovementSteeringView *movementDefinition;
  int32_t waterDelta;
  int waterDamage;
  ModelRuntimeNode *rootNode;
  Q12 waypointWorldXQ12;
  Q12 waypointWorldYQ12;

  linkedModelRuntime = (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.modelRuntime;
  rootNode = modelRuntime->rootModelNode;
  ownerMovementFlags = &modelRuntime->ownerArmyRuntime->movementStateFlags;
  *ownerMovementFlags = *ownerMovementFlags | ARMY_MOVEMENT_STATIONARY;
  /* Drop the linked model (classState.linkedArmyRuntimeOrSavedOffset) unless both definitions have a footprint
     radius and this unit is still within it. */
  if ((linkedModelRuntime != NULL) &&
      ((linkedModelRuntime->definitionOrSavedId.runtimeDefinition->footprintRadius == 0) ||
       (modelRuntime->modelDefinition->footprintRadius == 0) ||
       !ArmyCollision_TestPointWithinExpandedRuntimeRadius
          (modelRuntime->modelDefinition->footprintRadius,(rootNode->worldTransform).translation.y,
           (rootNode->worldTransform).translation.x,linkedModelRuntime))) {
    (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.modelRuntime = NULL;
  }
  previousRotationAngle = (rootNode->modelPayload).worldRotationAngle2;
  previousWorldX = (rootNode->worldTransform).translation.x;
  previousWorldY = (rootNode->worldTransform).translation.y;
  movementDefinition = modelRuntime->modelDefinition;
  waterDelta = FieldGrid_InterpolateWaterDelta(previousWorldY,previousWorldX,worldRuntime->fieldGrid);
  if (movementDefinition->waterDamageThreshold < waterDelta) {
    waterDamage = waterDelta * movementDefinition->waterDamageMultiplier >> 7;
    if (-1 < waterDamage) {
      ArmyRuntime_ApplyDamageAndPropagateToParent(waterDamage,(ModelRuntimeSlot *)modelRuntime);
    }
  }
  /* alive and still on its way to a route point */
  if ((((modelRuntime->classState).stateFlags & ARMY_RUNTIME_FLAG_DESTROYED) == 0) &&
      !ArmyRuntime_UpdateMovementAndWaypoints
         (worldRuntime,(ArmyMovementRuntime *)modelRuntime->ownerArmyRuntime,&waypointWorldXQ12,
          &waypointWorldYQ12)) {
    rootNode = ArmyGroundMovement_SteerAndDrive
                         (worldRuntime,modelRuntime,rootNode,waypointWorldXQ12,waypointWorldYQ12);
  }
  else {
    (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
    (modelRuntime->movementControl).turnVelocityAngle16 = 0;
    rootNode = ArmyGroundMovement_PlaceStationary(worldRuntime,modelRuntime);
  }
  if (((previousWorldX != (rootNode->worldTransform).translation.x) ||
      (previousWorldY != (rootNode->worldTransform).translation.y)) ||
     (previousRotationAngle != (rootNode->modelPayload).worldRotationAngle2)) {
    ownerArmy = modelRuntime->ownerArmyRuntime;
    ArmyRuntime_UpdateTimedShotAndEffectEmitters
              (worldRuntime,(ModelRuntimeUpdateView *)modelRuntime);
    ownerMovementFlags = &ownerArmy->movementStateFlags;
    *ownerMovementFlags = *ownerMovementFlags & ~ARMY_MOVEMENT_STATIONARY;
  }
  movementDefinition = modelRuntime->modelDefinition;
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ModelRuntimeSlot *)modelRuntime);
  ModelNodeRuntime_RebuildTransformsFromRoot(rootNode);
  ModelNodeRuntime_UpdateDepthBinMasks(movementDefinition->footprintRadius,rootNode);
  return;
}


/* Step rate of a terrain-contact step: (stride (definition classParameterC0) << 13) / (3D foot travel + 2 * lift
   height (definition classParameterC4)), or 0x2000 when that sum is zero. */
static void ArticulatedContact_SetStepRateFromTravel(ArmyArticulatedRuntimeSlotView *armyRuntime,
          uint32_t footTravelLength)

{
  ModelDefinition *movementDefinition;
  int travelPlusLift;
  int strideLength;

  movementDefinition = (ModelDefinition *)armyRuntime->definitionOrAsset;
  travelPlusLift = footTravelLength + movementDefinition->classParameterC4 * 2;
  strideLength = movementDefinition->classParameterC0;
  (armyRuntime->articulatedContact).fallbackPosition1Q12 = 2 * Q12_ONE;
  if (travelPlusLift != 0) {
    (armyRuntime->articulatedContact).fallbackPosition1Q12 =
         (Q12)((int64_t)(uint64_t)(uint32_t)(strideLength << 13) / (int64_t)travelPlusLift);
  }
  return;
}


/* The foot was set down beside the other foot instead of at its target: the owner's route point counts as
   reached and the step is marked obstructed (a second obstruction in a row clears both step flags). */
static void ArticulatedContact_MarkStepObstructed(ArmyArticulatedRuntimeSlotView *armyRuntime)

{
  GameEntityCommandFlags *entityCommandFlags;
  Q12 *contactStateFlags;

  /* the owner's common.commandFlags is its movementStateFlags */
  entityCommandFlags = &(armyRuntime->linkedEntityRuntime->common).commandFlags;
  *entityCommandFlags = *entityCommandFlags | ARMY_MOVEMENT_ROUTE_POINT_REACHED;
  contactStateFlags = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
  if ((*contactStateFlags & ARMY_ARTICULATED_STEP_OBSTRUCTED) != 0) {
    *contactStateFlags = *contactStateFlags & ~(ARMY_ARTICULATED_STEP_RIGHT | ARMY_ARTICULATED_STEP_LEFT);
  }
  *contactStateFlags = *contactStateFlags | ARMY_ARTICULATED_STEP_OBSTRUCTED;
  return;
}


/* Left foot step rate from its 3D travel between the previous contact (movementTarget0Q12,
   definitionClassValue80, definitionClassValue88) and the new one (X runtimeState90, sampled Y and height). */
static void ArticulatedContact_SetLeftStepRate(ArmyArticulatedRuntimeSlotView *armyRuntime,int footZ,
          uint32_t footY)

{
  ArticulatedContact_SetStepRateFromTravel
            (armyRuntime,
             FixedMath_Length3(footZ - armyRuntime->definitionClassValue88,
                               footY - armyRuntime->definitionClassValue80,
                               armyRuntime->runtimeState90 - armyRuntime->movementTarget0Q12));
  return;
}


/* Obstructed left step: sets the left foot down right beside the right foot (twice the lateral offset to the
   left of its previous contact), marks the step obstructed and derives the step rate. Stops after storing the
   foot position when that spot has no ground sample. */
static void ArticulatedContact_PlaceLeftFootBesideRightFoot(AngleTurn32 headingAngle16,
          ArmyArticulatedRuntimeSlotView *armyRuntime,WorldRuntimeContext *worldRuntime)

{
  FieldGridAsset *activeFieldGrid;
  uint32_t sideAngle;
  uint32_t footX;
  uint32_t footY;
  uint32_t sampledFootY;
  int sampledFootZ;
  FixedSinCos lateralSinCos;
  Q12 terrainHeightQ12;
  uint32_t terrainNormalAngles;
  UQ12 footRadius;

  sideAngle = headingAngle16 + FIXED_ANGLE16_QUARTER_TURN & FIXED_ANGLE16_MASK;
  lateralSinCos = FixedMath_SinCosScaled(sideAngle,(armyRuntime->articulatedContact).lateralOffsetQ12);
  footRadius = (armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.contactRadiusQ12;
  footX = armyRuntime->movementTarget1Q12 + lateralSinCos.cosValue * 2;
  footY = armyRuntime->definitionClassValue84 + lateralSinCos.sinValue * 2;
  activeFieldGrid = worldRuntime->fieldGrid;
  armyRuntime->runtimeState90 = footX;
  armyRuntime->runtimeState98 = footY;
  lateralSinCos = FixedMath_SinCosScaled(sideAngle,footRadius);
  if (!FieldGrid_InterpolateTerrainHeightAndNormal
                     (lateralSinCos.sinValue + footY,lateralSinCos.cosValue + footX,activeFieldGrid,
                      &terrainHeightQ12,&terrainNormalAngles)) {
    return;
  }
  armyRuntime->articulatedHeightOrStateA0 = terrainHeightQ12;
  armyRuntime->fallbackWorldYQ12 = terrainNormalAngles;
  sampledFootY = armyRuntime->runtimeState98;
  sampledFootZ = armyRuntime->articulatedHeightOrStateA0;
  ArticulatedContact_MarkStepObstructed(armyRuntime);
  ArticulatedContact_SetLeftStepRate(armyRuntime,sampledFootZ,sampledFootY);
  return;
}


/* Plans a walking step of the left foot towards heading headingAngle16 (routeDistanceQ12 = distance to the
   route point). The foot target is one stride (definition classParameterC0) past the spot beside the right foot, or on
   the route point itself when that is closer, offset to the left side. When the target is blocked by an army
   (whose owner is notified) or has no ground sample, the foot is set down right beside the right foot instead,
   the route point is advanced, and the step is marked obstructed (a second obstruction in a row cancels it).
   Finally the step rate is derived from the foot's 3D travel. Called by ArmyRuntimeClass_UpdateArticulatedMovement;
   the walker state layout is described at ArmyArticulatedRuntime_InitializeTerrainContactGeometry.
*/
void ArmyArticulatedRuntime_UpdateLeftTerrainContact(AngleTurn32 headingAngle16,Q12 routeDistanceQ12,
          ArmyArticulatedRuntimeSlotView *armyRuntime,WorldRuntimeContext *worldRuntime)

{
  Q12 *contactStateFlags;
  void *movementDefinition;
  FieldGridAsset *activeFieldGrid;
  uint32_t reachFromRoot;
  uint32_t strideReach;
  int aheadX;
  int aheadY;
  uint32_t footX;
  uint32_t footY;
  uint32_t sampledFootY;
  int sampledFootZ;
  uint32_t sideAngle;
  FixedSinCos lateralSinCos;
  FixedSinCos headingSinCos;
  ModelRuntimeSlot *blockingModelRuntime;
  Q12 waypointWorldXQ12;
  Q12 waypointWorldYQ12;
  Q12 terrainHeightQ12;
  uint32_t terrainNormalAngles;
  UQ12 footRadius;
  ModelRuntimeNode *rootNode;

  rootNode = armyRuntime->modelNodeRuntime;
  movementDefinition = armyRuntime->definitionOrAsset;
  armyRuntime->ownerValue64 = headingAngle16;
  (armyRuntime->linkedChildOverloadedState).leftHeadingCommandOrSpawnValue.headingOrTurnValue =
       headingAngle16;
  sideAngle = headingAngle16 + FIXED_ANGLE16_QUARTER_TURN & FIXED_ANGLE16_MASK;
  lateralSinCos = FixedMath_SinCosScaled(sideAngle,(armyRuntime->articulatedContact).lateralOffsetQ12);
  footRadius = (armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.contactRadiusQ12;
  reachFromRoot = FixedMath_Length2((lateralSinCos.sinValue + armyRuntime->articulatedCoordinateOrState9C) -
                            (rootNode->worldTransform).translation.y,
                            (lateralSinCos.cosValue + armyRuntime->runtimeState94) -
                            (rootNode->worldTransform).translation.x);
  /* classParameterC0: stride length */
  strideReach = reachFromRoot + ((ModelDefinition *)movementDefinition)->classParameterC0;
  if (strideReach < (uint32_t)routeDistanceQ12) {
    headingSinCos = FixedMath_SinCosScaled(headingAngle16,strideReach);
    aheadX = headingSinCos.cosValue + (rootNode->worldTransform).translation.x;
    aheadY = headingSinCos.sinValue + (rootNode->worldTransform).translation.y;
  }
  else {
    ArmyRuntime_UpdateMovementAndWaypoints
              (worldRuntime,(ArmyMovementRuntime *)armyRuntime->linkedEntityRuntime,&waypointWorldXQ12,
               &waypointWorldYQ12);
    aheadY = waypointWorldYQ12;
    aheadX = waypointWorldXQ12;
  }
  footX = aheadX + lateralSinCos.cosValue;
  footY = aheadY + lateralSinCos.sinValue;
  activeFieldGrid = worldRuntime->fieldGrid;
  armyRuntime->runtimeState90 = footX;
  armyRuntime->runtimeState98 = footY;
  /* the ground is sampled footRadius further out to the side */
  lateralSinCos = FixedMath_SinCosScaled(sideAngle,footRadius);
  if (activeFieldGrid == NULL) {
    return;
  }
  if (FieldGrid_InterpolateTerrainHeightAndNormal
                     (lateralSinCos.sinValue + footY,lateralSinCos.cosValue + footX,activeFieldGrid,
                      &terrainHeightQ12,&terrainNormalAngles)) {
    armyRuntime->fallbackWorldYQ12 = terrainNormalAngles;
    armyRuntime->articulatedHeightOrStateA0 = terrainHeightQ12;
    sampledFootY = armyRuntime->runtimeState98;
    sampledFootZ = armyRuntime->articulatedHeightOrStateA0;
    blockingModelRuntime = ArmyCollision_FindBlockingRuntimeForCurrentUnit
                       (sampledFootY,armyRuntime->runtimeState90,(RuntimeCollisionQueryView *)armyRuntime
                        ,worldRuntime);
    if (blockingModelRuntime == NULL) {
      contactStateFlags = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
      *contactStateFlags = *contactStateFlags & ~ARMY_ARTICULATED_STEP_OBSTRUCTED;
      ArticulatedContact_SetLeftStepRate(armyRuntime,sampledFootZ,sampledFootY);
      return;
    }
    ArmyRuntime_HandleCollisionPartner
              ((ModelRuntimeSlot *)armyRuntime,
               (armyRuntime->modelNodeRuntime->worldTransform).translation.y,
               (armyRuntime->modelNodeRuntime->worldTransform).translation.x,
               blockingModelRuntime,worldRuntime);
  }
  /* blocked or no ground at the target */
  ArticulatedContact_PlaceLeftFootBesideRightFoot(headingAngle16,armyRuntime,worldRuntime);
  return;
}


/* Right foot step rate from its 3D travel between the previous contact (movementTarget1Q12,
   definitionClassValue84, runtimeState8C) and the new one (X runtimeState94, sampled Y and height). */
static void ArticulatedContact_SetRightStepRate(ArmyArticulatedRuntimeSlotView *armyRuntime,uint32_t footZ,
          int footY)

{
  ArticulatedContact_SetStepRateFromTravel
            (armyRuntime,
             FixedMath_Length3(footZ - armyRuntime->runtimeState8C,
                               footY - armyRuntime->definitionClassValue84,
                               armyRuntime->runtimeState94 - armyRuntime->movementTarget1Q12));
  return;
}


/* Obstructed right step: sets the right foot down right beside the left foot (twice the lateral offset to the
   right of its previous contact), marks the step obstructed and derives the step rate. Stops after storing the
   foot position when that spot has no ground sample. */
static void ArticulatedContact_PlaceRightFootBesideLeftFoot(AngleTurn32 headingAngle16,
          ArmyArticulatedRuntimeSlotView *armyRuntime,WorldRuntimeContext *worldRuntime)

{
  FieldGridAsset *activeFieldGrid;
  uint32_t sideAngle;
  uint32_t footX;
  int footY;
  int sampledFootY;
  uint32_t sampledFootZ;
  FixedSinCos lateralSinCos;
  Q12 terrainHeightQ12;
  uint32_t terrainNormalAngles;
  UQ12 footRadius;

  sideAngle = headingAngle16 - FIXED_ANGLE16_QUARTER_TURN & FIXED_ANGLE16_MASK;
  lateralSinCos = FixedMath_SinCosScaled(sideAngle,(armyRuntime->articulatedContact).lateralOffsetQ12);
  footRadius = (armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.contactRadiusQ12;
  footX = armyRuntime->movementTarget0Q12 + lateralSinCos.cosValue * 2;
  footY = armyRuntime->definitionClassValue80 + lateralSinCos.sinValue * 2;
  activeFieldGrid = worldRuntime->fieldGrid;
  armyRuntime->runtimeState94 = footX;
  armyRuntime->articulatedCoordinateOrState9C = footY;
  lateralSinCos = FixedMath_SinCosScaled(sideAngle,footRadius);
  if (!FieldGrid_InterpolateTerrainHeightAndNormal
                     (lateralSinCos.sinValue + footY,lateralSinCos.cosValue + footX,activeFieldGrid,
                      &terrainHeightQ12,&terrainNormalAngles)) {
    return;
  }
  armyRuntime->runtimeStateA4 = terrainHeightQ12;
  armyRuntime->fallbackWorldXQ12 = terrainNormalAngles;
  sampledFootY = armyRuntime->articulatedCoordinateOrState9C;
  sampledFootZ = armyRuntime->runtimeStateA4;
  ArticulatedContact_MarkStepObstructed(armyRuntime);
  ArticulatedContact_SetRightStepRate(armyRuntime,sampledFootZ,sampledFootY);
  return;
}


/* Mirror of ArmyArticulatedRuntime_UpdateLeftTerrainContact for the right foot (side angle heading - 90
   degrees, placed relative to the left foot). Called by ArmyRuntimeClass_UpdateArticulatedMovement.
*/
void ArmyArticulatedRuntime_UpdateRightTerrainContact(AngleTurn32 headingAngle16,Q12 routeDistanceQ12,
          ArmyArticulatedRuntimeSlotView *armyRuntime,WorldRuntimeContext *worldRuntime)

{
  Q12 *contactStateFlags;
  void *movementDefinition;
  FieldGridAsset *activeFieldGrid;
  uint32_t reachFromRoot;
  uint32_t strideReach;
  int aheadX;
  int aheadY;
  uint32_t footX;
  int footY;
  int sampledFootY;
  uint32_t sampledFootZ;
  uint32_t sideAngle;
  FixedSinCos lateralSinCos;
  FixedSinCos headingSinCos;
  ModelRuntimeSlot *blockingModelRuntime;
  Q12 waypointWorldXQ12;
  Q12 waypointWorldYQ12;
  Q12 terrainHeightQ12;
  uint32_t terrainNormalAngles;
  UQ12 footRadius;
  ModelRuntimeNode *rootNode;

  rootNode = armyRuntime->modelNodeRuntime;
  movementDefinition = armyRuntime->definitionOrAsset;
  armyRuntime->ownerValue64 = headingAngle16;
  (armyRuntime->linkedChildOverloadedState).rightHeadingCommandOrSpawnValue.headingOrTurnValue =
       headingAngle16;
  sideAngle = headingAngle16 - FIXED_ANGLE16_QUARTER_TURN & FIXED_ANGLE16_MASK;
  lateralSinCos = FixedMath_SinCosScaled(sideAngle,(armyRuntime->articulatedContact).lateralOffsetQ12);
  footRadius = (armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.contactRadiusQ12;
  reachFromRoot = FixedMath_Length2((lateralSinCos.sinValue + armyRuntime->runtimeState98) -
                            (rootNode->worldTransform).translation.y,
                            (lateralSinCos.cosValue + armyRuntime->runtimeState90) -
                            (rootNode->worldTransform).translation.x);
  /* classParameterC0: stride length */
  strideReach = reachFromRoot + ((ModelDefinition *)movementDefinition)->classParameterC0;
  if (strideReach < (uint32_t)routeDistanceQ12) {
    headingSinCos = FixedMath_SinCosScaled(headingAngle16,strideReach);
    aheadX = headingSinCos.cosValue + (rootNode->worldTransform).translation.x;
    aheadY = headingSinCos.sinValue + (rootNode->worldTransform).translation.y;
  }
  else {
    ArmyRuntime_UpdateMovementAndWaypoints
              (worldRuntime,(ArmyMovementRuntime *)armyRuntime->linkedEntityRuntime,&waypointWorldXQ12,
               &waypointWorldYQ12);
    aheadY = waypointWorldYQ12;
    aheadX = waypointWorldXQ12;
  }
  footX = aheadX + lateralSinCos.cosValue;
  footY = aheadY + lateralSinCos.sinValue;
  activeFieldGrid = worldRuntime->fieldGrid;
  armyRuntime->runtimeState94 = footX;
  armyRuntime->articulatedCoordinateOrState9C = footY;
  lateralSinCos = FixedMath_SinCosScaled(sideAngle,footRadius);
  if (activeFieldGrid == NULL) {
    return;
  }
  if (FieldGrid_InterpolateTerrainHeightAndNormal
                     (lateralSinCos.sinValue + footY,lateralSinCos.cosValue + footX,activeFieldGrid,
                      &terrainHeightQ12,&terrainNormalAngles)) {
    armyRuntime->fallbackWorldXQ12 = terrainNormalAngles;
    armyRuntime->runtimeStateA4 = terrainHeightQ12;
    sampledFootY = armyRuntime->articulatedCoordinateOrState9C;
    sampledFootZ = armyRuntime->runtimeStateA4;
    blockingModelRuntime = ArmyCollision_FindBlockingRuntimeForCurrentUnit
                       (sampledFootY,armyRuntime->runtimeState94,(RuntimeCollisionQueryView *)armyRuntime
                        ,worldRuntime);
    if (blockingModelRuntime == NULL) {
      contactStateFlags = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
      *contactStateFlags = *contactStateFlags & ~ARMY_ARTICULATED_STEP_OBSTRUCTED;
      ArticulatedContact_SetRightStepRate(armyRuntime,sampledFootZ,sampledFootY);
      return;
    }
    ArmyRuntime_HandleCollisionPartner
              ((ModelRuntimeSlot *)armyRuntime,
               (armyRuntime->modelNodeRuntime->worldTransform).translation.y,
               (armyRuntime->modelNodeRuntime->worldTransform).translation.x,
               blockingModelRuntime,worldRuntime);
  }
  /* blocked or no ground at the target */
  ArticulatedContact_PlaceRightFootBesideLeftFoot(headingAngle16,armyRuntime,worldRuntime);
  return;
}


/* Water-surface variant of ArmyGroundMovement_SteerAndDrive: turns the model towards the waypoint and, if the
   heading error is small enough, drives towards it, places it with the placement callback and applies the
   recoil tilt; otherwise it stops and is placed stationary. Unlike the ground version a blocking army is not
   notified, the unit just stops. Returns the root node used for the rest of the tick. */
static ModelRuntimeNode *ArmyWaterSurfaceMovement_SteerAndDrive
          (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView *modelRuntime,
          ModelRuntimeNode *rootNode,Q12 waypointWorldXQ12,Q12 waypointWorldYQ12)

{
  ArmyMovementStateFlags *ownerMovementFlags;
  ArmyRuntimeSlot *ownerArmy;
  ModelDefinitionGroundMovementSteeringView *movementDefinition;
  ArmyPlacementContactKindIndex32 placementContactKind;
  int routeDeltaX;
  int routeDeltaY;
  int turnVelocityLimit;
  int acceleratedTurnVelocity;
  int travelDistance;
  int remainingRecoilTicks;
  int recoilTilt;
  uint32_t facingAngle;
  uint32_t turnVelocity;
  uint32_t headingErrorLimit;
  uint32_t headingError;
  uint32_t desiredHeading;
  uint32_t headingDifference;
  FixedLengthAngle angleAndLength;
  FixedPlanarPointQ12 nextPosition;
  ModelRuntimeSlot *blockingModelRuntime;
  Q12 heightOffsetQ12;
  uint32_t targetDistance;
  ModelRuntimeNode *blockedRootNode;

  routeDeltaY = waypointWorldYQ12 - (rootNode->worldTransform).translation.y;
  routeDeltaX = waypointWorldXQ12 - (rootNode->worldTransform).translation.x;
  if ((routeDeltaX == 0) && (routeDeltaY == 0)) {
    angleAndLength = (FixedLengthAngle){ .length = 0, .angle = (rootNode->modelPayload).worldRotationAngle2 };
  }
  else {
    angleAndLength = FixedMath_Vector2AngleAndLength(routeDeltaY,routeDeltaX);
  }
  /* accelerated turning as in ArmyRuntimeClass_UpdateGroundMovement: turn by the current turn velocity,
     then accelerate it (clamped to the turn rate limit); a turn in the other direction first resets the
     velocity, a small remaining error snaps onto the desired heading */
  desiredHeading = angleAndLength.angle;
  movementDefinition = modelRuntime->modelDefinition;
  facingAngle = (rootNode->modelPayload).worldRotationAngle2;
  turnVelocity = (modelRuntime->movementControl).turnVelocityAngle16;
  headingDifference = desiredHeading - facingAngle & FIXED_ANGLE16_MASK;
  if (headingDifference < FIXED_ANGLE16_HALF_TURN) {
    if ((int)turnVelocity < 0) {
      (modelRuntime->movementControl).turnVelocityAngle16 = 0;
    }
    else if (turnVelocity < headingDifference) {
      facingAngle = facingAngle + turnVelocity;
      turnVelocityLimit = movementDefinition->turnRateLimitAnglePerTick * g_InGameSimulationStepTicks;
      acceleratedTurnVelocity =
           turnVelocity + movementDefinition->turnRateAccelerationAnglePerTick * g_InGameSimulationStepTicks;
      (modelRuntime->movementControl).turnVelocityAngle16 = turnVelocityLimit;
      if (acceleratedTurnVelocity < turnVelocityLimit) {
        (modelRuntime->movementControl).turnVelocityAngle16 = acceleratedTurnVelocity;
      }
    }
    else {
      (modelRuntime->movementControl).turnVelocityAngle16 = 0;
      facingAngle = desiredHeading;
    }
  }
  else if (0 < (int)turnVelocity) {
    (modelRuntime->movementControl).turnVelocityAngle16 = 0;
  }
  else if (turnVelocity + FIXED_ANGLE16_FULL_TURN <= headingDifference) {
    (modelRuntime->movementControl).turnVelocityAngle16 = 0;
    facingAngle = desiredHeading;
  }
  else {
    facingAngle = facingAngle + turnVelocity;
    turnVelocityLimit = -movementDefinition->turnRateLimitAnglePerTick * g_InGameSimulationStepTicks;
    acceleratedTurnVelocity =
         turnVelocity - movementDefinition->turnRateAccelerationAnglePerTick * g_InGameSimulationStepTicks;
    (modelRuntime->movementControl).turnVelocityAngle16 = turnVelocityLimit;
    if (turnVelocityLimit < acceleratedTurnVelocity) {
      (modelRuntime->movementControl).turnVelocityAngle16 = acceleratedTurnVelocity;
    }
  }
  rootNode = modelRuntime->rootModelNode;
  facingAngle = facingAngle & FIXED_ANGLE16_MASK;
  if (facingAngle != (rootNode->modelPayload).worldRotationAngle2) {
    (rootNode->modelPayload).worldRotationAngle2 = facingAngle;
    rootNode->runtimeFlags = rootNode->runtimeFlags | 1;
  }
  /* allowed heading error: the far limit, blended towards the near limit inside the interpolation distance */
  targetDistance = angleAndLength.length;
  headingErrorLimit = movementDefinition->farHeadingErrorLimitAngle;
  headingError = facingAngle - desiredHeading & FIXED_ANGLE16_MASK;
  if ((int)targetDistance < movementDefinition->headingErrorInterpolationDistanceQ12) {
    headingErrorLimit = movementDefinition->nearHeadingErrorLimitAngle +
            (int)(((int64_t)(int)(headingErrorLimit - movementDefinition->nearHeadingErrorLimitAngle) *
                  (int64_t)(int)targetDistance) /
                 (int64_t)movementDefinition->headingErrorInterpolationDistanceQ12);
  }
  if ((headingErrorLimit < headingError) && (headingError < FIXED_ANGLE16_FULL_TURN - headingErrorLimit)) {
    (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
    return ArmyGroundMovement_PlaceStationary(worldRuntime,modelRuntime);
  }
  ArmyRuntime_UpdateActivationMetricAndPlayStartSound
            (worldRuntime,(ModelRuntimeSlot *)modelRuntime);
  /* drive forward, or jump onto the route point once it is closer than twice this tick's travel */
  travelDistance = (modelRuntime->movementControl).movementAdvancePerTickQ12 * g_InGameSimulationStepTicks;
  if (travelDistance < (int)targetDistance >> 1) {
    nextPosition = FixedTrig_ProjectPlanarPoint
                       ((rootNode->worldTransform).translation.x,(rootNode->worldTransform).translation.y,
                        travelDistance,(rootNode->modelPayload).worldRotationAngle2);
  }
  else {
    ownerArmy = modelRuntime->ownerArmyRuntime;
    ArmyRuntime_UpdateMovementAndWaypoints
              (worldRuntime,(ArmyMovementRuntime *)ownerArmy,&waypointWorldXQ12,&waypointWorldYQ12);
    nextPosition.xQ12 = waypointWorldXQ12;
    nextPosition.yQ12 = waypointWorldYQ12;
    ownerMovementFlags = &ownerArmy->movementStateFlags;
    *ownerMovementFlags = *ownerMovementFlags | ARMY_MOVEMENT_ROUTE_POINT_REACHED;
  }
  placementContactKind = movementDefinition->placementContactKindIndex;
  rootNode = modelRuntime->rootModelNode;
  heightOffsetQ12 = movementDefinition->placementHeightOffsetQ12;
  blockingModelRuntime = ArmyCollision_FindBlockingRuntimeForCurrentUnit
                     (nextPosition.yQ12,nextPosition.xQ12,
                      (RuntimeCollisionQueryView *)modelRuntime,worldRuntime);
  if (blockingModelRuntime != NULL) {
    /* stay where we are; the blocking army is not notified */
    blockedRootNode = modelRuntime->rootModelNode;
    (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
    nextPosition.xQ12 = (blockedRootNode->worldTransform).translation.x;
    nextPosition.yQ12 = (blockedRootNode->worldTransform).translation.y;
    ownerMovementFlags = &modelRuntime->ownerArmyRuntime->movementStateFlags;
    *ownerMovementFlags = *ownerMovementFlags | ARMY_MOVEMENT_ROUTE_POINT_REACHED;
  }
  ownerArmy = modelRuntime->ownerArmyRuntime;
  g_ArmyPlacementContactKindDispatchTable.callbacks[placementContactKind]
            (heightOffsetQ12,nextPosition.yQ12,nextPosition.xQ12,rootNode,worldRuntime);
  rootNode = modelRuntime->rootModelNode;
  /* recoil after a shot: actionVector1Q12 counts the remaining ticks, actionVector2Q12 is the tilt per tick */
  remainingRecoilTicks = ownerArmy->actionVector1Q12 - 1;
  if (remainingRecoilTicks >= 0) {
    recoilTilt = remainingRecoilTicks * ownerArmy->actionVector2Q12;
    ownerArmy->actionVector1Q12 = ownerArmy->actionVector1Q12 - 1;
    ArmyGroundMovement_ApplyRecoilTilt(modelRuntime,rootNode,recoilTilt);
  }
  return rootNode;
}

/* Ground movement without water damage and without notifying the blocking army on a collision (the unit just
   stops); otherwise identical to ArmyRuntimeClass_UpdateGroundMovement. Used by runtimeUpdate slot 19
   of g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes and, for units on the water surface, by slot 18
   (ArmyRuntimeClass_UpdateSpecialBehaviorAndGroundMovement).
*/

void ArmyRuntimeClass_UpdateWaterSurfaceMovement
          (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView *modelRuntime
          )

{
  ArmyMovementStateFlags *ownerMovementFlags;
  ArmyRuntimeSlot *ownerArmy;
  ModelRuntimeSlot *linkedModelRuntime;
  AngleTurn32 previousRotationAngle;
  int previousWorldX;
  int previousWorldY;
  ModelDefinitionGroundMovementSteeringView *movementDefinition;
  ModelRuntimeNode *rootNode;
  Q12 waypointWorldXQ12;
  Q12 waypointWorldYQ12;

  linkedModelRuntime = (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.modelRuntime;
  rootNode = modelRuntime->rootModelNode;
  ownerMovementFlags = &modelRuntime->ownerArmyRuntime->movementStateFlags;
  *ownerMovementFlags = *ownerMovementFlags | ARMY_MOVEMENT_STATIONARY;
  /* Drop the linked model (classState.linkedArmyRuntimeOrSavedOffset) unless both definitions have a footprint
     radius and this unit is still within it. */
  if ((linkedModelRuntime != NULL) &&
      ((linkedModelRuntime->definitionOrSavedId.runtimeDefinition->footprintRadius == 0) ||
       (modelRuntime->modelDefinition->footprintRadius == 0) ||
       !ArmyCollision_TestPointWithinExpandedRuntimeRadius
          (modelRuntime->modelDefinition->footprintRadius,(rootNode->worldTransform).translation.y,
           (rootNode->worldTransform).translation.x,linkedModelRuntime))) {
    (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.modelRuntime = NULL;
  }
  previousRotationAngle = (rootNode->modelPayload).worldRotationAngle2;
  previousWorldX = (rootNode->worldTransform).translation.x;
  previousWorldY = (rootNode->worldTransform).translation.y;
  /* alive and still on its way to a route point */
  if ((((modelRuntime->classState).stateFlags & ARMY_RUNTIME_FLAG_DESTROYED) == 0) &&
      !ArmyRuntime_UpdateMovementAndWaypoints
         (worldRuntime,(ArmyMovementRuntime *)modelRuntime->ownerArmyRuntime,&waypointWorldXQ12,
          &waypointWorldYQ12)) {
    rootNode = ArmyWaterSurfaceMovement_SteerAndDrive
                         (worldRuntime,modelRuntime,rootNode,waypointWorldXQ12,waypointWorldYQ12);
  }
  else {
    (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
    (modelRuntime->movementControl).turnVelocityAngle16 = 0;
    rootNode = ArmyGroundMovement_PlaceStationary(worldRuntime,modelRuntime);
  }
  if (((previousWorldX != (rootNode->worldTransform).translation.x) ||
      (previousWorldY != (rootNode->worldTransform).translation.y)) ||
     (previousRotationAngle != (rootNode->modelPayload).worldRotationAngle2)) {
    ownerArmy = modelRuntime->ownerArmyRuntime;
    ArmyRuntime_UpdateTimedShotAndEffectEmitters
              (worldRuntime,(ModelRuntimeUpdateView *)modelRuntime);
    ownerMovementFlags = &ownerArmy->movementStateFlags;
    *ownerMovementFlags = *ownerMovementFlags & ~ARMY_MOVEMENT_STATIONARY;
  }
  movementDefinition = modelRuntime->modelDefinition;
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ModelRuntimeSlot *)modelRuntime);
  ModelNodeRuntime_RebuildTransformsFromRoot(rootNode);
  ModelNodeRuntime_UpdateDepthBinMasks(movementDefinition->footprintRadius,rootNode);
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


/* Footfall of an articulated walker, called by ArmyRuntimeClass_UpdateArticulatedMovement when a foot has
   finished its step (legNode = root child 0 for the left leg, 1 for the right). Plays the definition's
   footstep sound at the walker unless its cell is masked, and spawns the footprint effect at the foot node
   (three levels below the leg): one effect on dry ground, another where the nearest water is above ground.
*/
void ArmyArticulatedRuntime_UpdateContactChildAndEffects(ModelRuntimeNode *legNode,WorldRuntimeContext *worldRuntime,
          ModelRuntimeSlot *modelRuntime)

{
  GraphicsFixedVec3 *worldPosition;
  /* the footstep sound index is the class parameter classParameterCC of the walker's definition */
  ModelDefinition *definition;
  uint32_t soundIndex;
  DirectSoundVoiceSet **voiceSetRef;
  int32_t waterDelta;
  EffectDefinition *effectDefinition;
  bool cellMasked;
  ModelRuntimeNode *footNode;

  definition = modelRuntime->definitionOrSavedId.runtimeDefinition;
  soundIndex = definition->classParameterCC;
  voiceSetRef = NULL;
  if ((soundIndex != 0) && (soundIndex < worldRuntime->dwordArrayCount) && (worldRuntime->dwordArray != NULL)) {
    voiceSetRef = (DirectSoundVoiceSet **)worldRuntime->dwordArray[soundIndex];
  }
  if (voiceSetRef != NULL) {
    worldPosition = &(modelRuntime->rootModelNodeOrSavedOffset.modelNode->worldTransform).translation;
    cellMasked = TerrainGrid_TestProjectedCellMaskBits01
                      ((modelRuntime->rootModelNodeOrSavedOffset.modelNode->worldTransform).translation.y,
                       worldPosition->x,worldRuntime);
    if (!cellMasked) {
      SpatialSound_PlayPositionedOneShot
                (definition->positionedSoundMaximumDistanceQ12,definition->positionedSoundGainQ15,
                 worldPosition,voiceSetRef);
    }
  }
  footNode = legNode->childNodes[0]->childNodes[0]->childNodes[0];
  waterDelta = FieldGrid_GetNearestWaterDelta
                    ((footNode->worldTransform).translation.y,
                     (footNode->worldTransform).translation.x,worldRuntime->fieldGrid);
  if (waterDelta < 1) {
    effectDefinition = definition->emitterEffectDefinitionReference.definition; /* dry ground */
  }
  else {
    effectDefinition = definition->waterEmitterEffectDefinitionReference.definition; /* water */
  }
  if (effectDefinition != NULL) {
    EffectRuntimePool_CreateInstanceFromDefinition
              (EFFECT_RUNTIME_COMPLETION_NONE,(EffectRuntimeOwnerReference){ .modelNode = NULL },
               (footNode->modelPayload).worldRotationAngle2,
               (footNode->modelPayload).worldRotationAngle1,
               (footNode->modelPayload).worldRotationAngle0,
               (footNode->worldTransform).translation.z,(footNode->worldTransform).translation.y
               ,(footNode->worldTransform).translation.x,effectDefinition,worldRuntime);
  }
  return;
}


/* Places one foot of the articulated walker along its step: position = current + (target - current) *
   progress (Q12); the target height is raised by (1.0 - progress) * lift * 4, which gives the arc. All
   differences are taken in 32-bit unsigned arithmetic and then read as signed, as in the original. */
static void ArticulatedWalker_PlaceFootAlongStep
          (ModelRuntimeNode *footNode,uint32_t blendQ12,int liftHeight,uint32_t currentX,uint32_t targetX,
          uint32_t currentY,uint32_t targetY,uint32_t currentZ,uint32_t targetGroundZ)

{
  int64_t blendProduct;
  uint32_t targetZ;

  blendProduct = (int64_t)(int)(targetX - currentX) * (int64_t)(int)blendQ12;
  (footNode->worldTransform).translation.x = FIXED_PRODUCT_SHR(blendProduct,Q12_SHIFT) + currentX;
  blendProduct = (int64_t)(int)(targetY - currentY) * (int64_t)(int)blendQ12;
  (footNode->worldTransform).translation.y = FIXED_PRODUCT_SHR(blendProduct,Q12_SHIFT) + currentY;
  targetZ = ((int)((Q12_ONE - blendQ12) * (uint32_t)liftHeight) >> 10) + targetGroundZ;
  blendProduct = (int64_t)(int)(targetZ - currentZ) * (int64_t)(int)blendQ12;
  (footNode->worldTransform).translation.z = FIXED_PRODUCT_SHR(blendProduct,Q12_SHIFT) + currentZ;
}

/* Foot tilt: blends the target ground normal (packed elevation << 16 | azimuth) by progress with the current
   ground normal by (1.0 - progress) in the g_ArmySuspensionBlendVectorA/B scratch vectors and returns the
   angles of the blended normal. */
static FixedElevationAzimuth ArticulatedWalker_BlendGroundNormal
          (int targetNormalAngles,int currentNormalAngles,int blendQ12,int inverseBlendQ12)

{
  FixedMath_WriteDirectionQ28
            (&g_ArmySuspensionBlendVectorA,
             targetNormalAngles >> 16,targetNormalAngles & FIXED_ANGLE16_MASK);
  FixedMath_WriteDirectionQ28
            (&g_ArmySuspensionBlendVectorB,
             currentNormalAngles >> 16,currentNormalAngles & FIXED_ANGLE16_MASK);
  g_ArmySuspensionBlendVectorA.x =
       FIXED_MUL_SHR(g_ArmySuspensionBlendVectorA.x,blendQ12,Q12_SHIFT) +
       FIXED_MUL_SHR(g_ArmySuspensionBlendVectorB.x,inverseBlendQ12,Q12_SHIFT);
  g_ArmySuspensionBlendVectorA.y =
       FIXED_MUL_SHR(g_ArmySuspensionBlendVectorA.y,blendQ12,Q12_SHIFT) +
       FIXED_MUL_SHR(g_ArmySuspensionBlendVectorB.y,inverseBlendQ12,Q12_SHIFT);
  g_ArmySuspensionBlendVectorA.z =
       FIXED_MUL_SHR(g_ArmySuspensionBlendVectorA.z,blendQ12,Q12_SHIFT) +
       FIXED_MUL_SHR(g_ArmySuspensionBlendVectorB.z,inverseBlendQ12,Q12_SHIFT);
  return FixedMath_VectorToAnglesVec3(&g_ArmySuspensionBlendVectorA);
}

/* Foot orientation: world rotation (footHeading, blended normal angles) times the inverse of the foot's
   current world rotation, stored as the foot's local angles (g_ArmySuspensionRotationMatrix* scratch). */
static void ArticulatedWalker_SetFootLocalOrientation
          (ModelRuntimeNode *footNode,AngleTurn32 footHeading,FixedElevationAzimuth groundNormalAngles)

{
  FixedRollAzimuthElevation extractedAngles;

  FixedTransform_BuildRotationBasis
            (&g_ArmySuspensionRotationMatrixScratchB,
             (footNode->modelPayload).worldRotationAngle2,
             (footNode->modelPayload).worldRotationAngle1,
             (footNode->modelPayload).worldRotationAngle0);
  FixedTransform_InvertRigidQ28
            (&g_ArmySuspensionRotationMatrixScratchA,&g_ArmySuspensionRotationMatrixScratchB);
  FixedTransform_BuildRotationBasis
            (&g_ArmySuspensionRotationMatrixScratchB,footHeading,
             groundNormalAngles.elevationAngle,groundNormalAngles.azimuthAngle);
  FixedTransform_Compose
            (&g_ArmySuspensionRotationMatrixComposedScratch,&g_ArmySuspensionRotationMatrixScratchB,
             &g_ArmySuspensionRotationMatrixScratchA);
  extractedAngles = FixedTransform_ExtractEulerAngles(&g_ArmySuspensionRotationMatrixComposedScratch);
  (footNode->modelPayload).localRotationAngle0 = extractedAngles.azimuthAngle;
  (footNode->modelPayload).localRotationAngle1 = extractedAngles.elevationAngle;
  (footNode->modelPayload).localRotationAngle2 = extractedAngles.rollAngle;
}

/* Poses the two-legged articulated walker from its foot state (layout at
   ArmyArticulatedRuntime_InitializeTerrainContactGeometry). Each foot node (four levels below root child 0 =
   left leg, child 1 = right leg) is interpolated from its position to its step target by the step progress,
   lifted in an arc by the lift height (definition classParameterC4), and tilted by the blended ground normals.
   The body heading follows the step progress, the root sits midway between the feet at hip height (definition
   placementHeightOffsetQ12),
   the hip joints yaw towards the feet, and thigh and shin are bent by a two-bone triangle solution; finally
   the foot orientation is converted into the shin's local frame. Called by
   ArmyRuntimeClass_UpdateArticulatedMovement and ArmyPlacementContact_InitializeArticulatedSuspension
   (placement contact kind 3). g_ArmySuspension* are scratch vectors and matrices.
*/

void ArmyArticulatedRuntime_UpdateSuspensionHierarchy
          (ModelRuntimeNode *modelNodeRuntime,WorldRuntimeContext *worldRuntime)

{
  GraphicsWorldCoordinateQ12 *nodeTranslationZ;
  uint32_t leftBlendQ12;
  int leftHeading;
  int leftStartHeading;
  int rightBlendQ12;
  int rightHeading;
  int rightStartHeading;
  void *movementDefinition;
  int liftHeight;
  int footHeightOffset;
  int leftFootX;
  int leftFootY;
  int leftFootZ;
  int rightFootX;
  int rightFootY;
  int rightFootZ;
  AngleTurn32 rootHeading;
  uint32_t leftHipDistance;
  uint32_t rightHipDistance;
  uint32_t leftRelativeRaw;
  uint32_t leftRelativeMasked;
  uint32_t rightRelativeRaw;
  uint32_t rightRelativeMasked;
  short leftRelativeAngle;
  short rightRelativeAngle;
  uint32_t leftHipYawOffset;
  int rightHipYawOffset;
  uint32_t rightHipYaw;
  uint32_t rightAnkleAngle;
  int leftAnkleX;
  int leftAnkleY;
  int rightAnkleX;
  int rightAnkleY;
  uint32_t leftLegRelativeAzimuth;
  uint32_t rightLegRelativeAzimuth;
  int leftLegElevation;
  int rightLegElevation;
  uint32_t thighLengthQ12;
  uint32_t shinLengthQ12;
  int leftThighBend;
  int rightThighBend;
  FixedLengthAngle leftPlanarVector;
  FixedLengthAngle rightPlanarVector;
  FixedSinCos contactOffset;
  FixedTriangleJointAngles jointAngles;
  FixedLengthAzimuthElevation leftLegVector;
  FixedLengthAzimuthElevation rightLegVector;
  FixedElevationAzimuth leftBlendAngles;
  FixedElevationAzimuth rightBlendAngles;
  UQ12 scale;
  GraphicsWorldCoordinateQ12 leftContactX;
  GraphicsWorldCoordinateQ12 leftContactY;
  GraphicsWorldCoordinateQ12 leftContactZ;
  GraphicsWorldCoordinateQ12 rightContactX;
  GraphicsWorldCoordinateQ12 rightContactY;
  GraphicsWorldCoordinateQ12 rightContactZ;
  ArmyArticulatedRuntimeSlotView *articulatedRuntime;
  ArmyRuntimeSlot *walkerRuntime;
  ModelRuntimeNode *rightNode;
  ModelRuntimeNode *leftNode;

  articulatedRuntime = (ArmyArticulatedRuntimeSlotView *)(modelNodeRuntime->runtimePayload).armyRuntime;
  /* left foot position along its step (progress in runtimeStateA8) */
  leftNode = modelNodeRuntime->childNodes[0]->childNodes[0]->childNodes[0]->childNodes[0];
  leftBlendQ12 = articulatedRuntime->runtimeStateA8;
  liftHeight = ((ModelDefinition *)articulatedRuntime->definitionOrAsset)->classParameterC4;
  ArticulatedWalker_PlaceFootAlongStep
            (leftNode,leftBlendQ12,liftHeight,
             articulatedRuntime->movementTarget0Q12,articulatedRuntime->runtimeState90,
             articulatedRuntime->definitionClassValue80,articulatedRuntime->runtimeState98,
             articulatedRuntime->definitionClassValue88,articulatedRuntime->articulatedHeightOrStateA0);
  /* left foot tilt: target normal * progress + current normal * (1.0 - progress) */
  leftHeading = (articulatedRuntime->linkedChildOverloadedState).leftHeadingCommandOrSpawnValue.signedValue;
  leftStartHeading = (articulatedRuntime->linkedChildOverloadedState).primaryCoordinateCommandOrHistory.signedValue;
  leftBlendAngles = ArticulatedWalker_BlendGroundNormal
                      (articulatedRuntime->fallbackWorldYQ12,(int)articulatedRuntime->ownerValue68,
                       (int)leftBlendQ12,Q12_ONE - articulatedRuntime->runtimeStateA8);
  /* the same for the right foot (progress in terrainContactMode) */
  rightNode = modelNodeRuntime->childNodes[1]->childNodes[0]->childNodes[0]->childNodes[0];
  rightBlendQ12 = (articulatedRuntime->articulatedContact).terrainContactMode;
  liftHeight = ((ModelDefinition *)articulatedRuntime->definitionOrAsset)->classParameterC4;
  ArticulatedWalker_PlaceFootAlongStep
            (rightNode,rightBlendQ12,liftHeight,
             articulatedRuntime->movementTarget1Q12,articulatedRuntime->runtimeState94,
             articulatedRuntime->definitionClassValue84,articulatedRuntime->articulatedCoordinateOrState9C,
             articulatedRuntime->runtimeState8C,articulatedRuntime->runtimeStateA4);
  rightHeading = (articulatedRuntime->linkedChildOverloadedState).rightHeadingCommandOrSpawnValue.signedValue;
  rightStartHeading = (articulatedRuntime->linkedChildOverloadedState).secondaryCoordinateCommandOrHistory.signedValue;
  rightBlendAngles = ArticulatedWalker_BlendGroundNormal
                       (articulatedRuntime->fallbackWorldXQ12,(int)articulatedRuntime->linkedArmyRuntimeOrSavedOffset,
                        rightBlendQ12,Q12_ONE - (articulatedRuntime->articulatedContact).terrainContactMode);
  movementDefinition = articulatedRuntime->definitionOrAsset;
  /* body heading = start heading + (end - start) * (left progress + right progress) */
  walkerRuntime = (modelNodeRuntime->runtimePayload).armyRuntime;
  (modelNodeRuntime->modelPayload).worldRotationAngle2 =
       (((int)((walkerRuntime->classState64 - walkerRuntime->classState60) * ARMY_ANGLE16_SIGN_EXTEND_SCALE) >> 16) *
        (walkerRuntime->runtimeStateA8 + (walkerRuntime->articulatedContact).terrainContactMode) >> Q12_SHIFT) +
       walkerRuntime->classState60 & FIXED_ANGLE16_MASK;
  /* raise both feet by the foot model's height offset; the root goes midway between them at hip height */
  leftFootX = (leftNode->worldTransform).translation.x;
  leftFootY = (leftNode->worldTransform).translation.y;
  footHeightOffset = ((rightNode->modelPayload).modelResource)->placementHeightOffsetQ12;
  rightFootX = (rightNode->worldTransform).translation.x;
  rightFootY = (rightNode->worldTransform).translation.y;
  nodeTranslationZ = &(leftNode->worldTransform).translation.z;
  *nodeTranslationZ = *nodeTranslationZ + footHeightOffset;
  nodeTranslationZ = &(rightNode->worldTransform).translation.z;
  *nodeTranslationZ = *nodeTranslationZ + footHeightOffset;
  leftFootZ = (leftNode->worldTransform).translation.z;
  rightFootZ = (rightNode->worldTransform).translation.z;
  (modelNodeRuntime->worldTransform).translation.x = (leftFootX + rightFootX) >> 1;
  (modelNodeRuntime->worldTransform).translation.y = (leftFootY + rightFootY) >> 1;
  (modelNodeRuntime->worldTransform).translation.z =
       ((leftFootZ + rightFootZ) >> 1) + ((ModelDefinition *)movementDefinition)->placementHeightOffsetQ12;
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
  leftPlanarVector = FixedMath_Vector2AngleAndLength
                     (leftContactY - (leftNode->worldTransform).translation.y,
                      leftContactX - (leftNode->worldTransform).translation.x);
  leftHipDistance = leftPlanarVector.length;
  rightPlanarVector = FixedMath_Vector2AngleAndLength
                     (rightContactY - (rightNode->worldTransform).translation.y,
                      rightContactX - (rightNode->worldTransform).translation.x);
  rightHipDistance = rightPlanarVector.length;
  rootHeading = (modelNodeRuntime->modelPayload).worldRotationAngle2;
  leftRelativeRaw = leftPlanarVector.angle - rootHeading;
  rightRelativeRaw = rightPlanarVector.angle - rootHeading;
  leftRelativeMasked = leftRelativeRaw & FIXED_ANGLE16_MASK;
  leftRelativeAngle = (short)leftRelativeRaw;
  rightRelativeMasked = rightRelativeRaw & FIXED_ANGLE16_MASK;
  rightRelativeAngle = (short)rightRelativeRaw;
  if ((FIXED_ANGLE16_QUARTER_TURN - 1 < leftRelativeMasked) && (leftRelativeMasked < FIXED_ANGLE16_THREE_QUARTER_TURN + 1)) {
    leftRelativeAngle = leftRelativeAngle + -FIXED_ANGLE16_HALF_TURN;
  }
  if ((FIXED_ANGLE16_QUARTER_TURN - 1 < rightRelativeMasked) && (rightRelativeMasked < FIXED_ANGLE16_THREE_QUARTER_TURN + 1)) {
    rightRelativeAngle = rightRelativeAngle + -FIXED_ANGLE16_HALF_TURN;
  }
  leftHipYawOffset = (uint32_t)leftRelativeAngle;
  rightHipYawOffset = (int)rightRelativeAngle;
  if (rightHipDistance < 320) {
    if (rightHipDistance < 64) {
      rightHipYawOffset = 0;
    }
    else {
      rightHipYawOffset = (int)(rightHipYawOffset * (rightHipDistance - 64)) >> 8;
    }
  }
  if (leftHipDistance < 320) {
    if (leftHipDistance < 64) {
      leftHipYawOffset = 0;
    }
    else {
      leftHipYawOffset = (int)(leftHipYawOffset * (leftHipDistance - 64)) >> 8;
    }
  }
  rootHeading = (modelNodeRuntime->modelPayload).worldRotationAngle2;
  rightHipYaw = rightHipYawOffset + FIXED_ANGLE16_HALF_TURN & FIXED_ANGLE16_MASK;
  (leftNode->modelPayload).localRotationAngle2 = leftHipYawOffset & FIXED_ANGLE16_MASK;
  (rightNode->modelPayload).localRotationAngle2 = rightHipYaw;
  rightAnkleAngle = (rootHeading - FIXED_ANGLE16_QUARTER_TURN) + rightHipYaw & FIXED_ANGLE16_MASK;
  scale = (((modelNodeRuntime->runtimePayload).armyRuntime)->articulatedContact).
          contactRadiusOrLinkedSlotMask.contactRadiusQ12;
  contactOffset = FixedMath_SinCosScaled
                     (rootHeading + FIXED_ANGLE16_QUARTER_TURN + (leftHipYawOffset & FIXED_ANGLE16_MASK) & FIXED_ANGLE16_MASK,
                      (((modelNodeRuntime->runtimePayload).armyRuntime)->articulatedContact).
                      contactRadiusOrLinkedSlotMask.contactRadiusQ12);
  leftAnkleX = leftContactX + contactOffset.cosValue;
  leftAnkleY = leftContactY + contactOffset.sinValue;
  contactOffset = FixedMath_SinCosScaled(rightAnkleAngle,scale);
  rightAnkleX = rightContactX - contactOffset.cosValue;
  rightAnkleY = rightContactY - contactOffset.sinValue;
  /* two-bone leg: aim thigh and shin at the ankle points (contactRadius beside each foot) */
  ModelNodeRuntime_RebuildTransformsFromRoot(leftNode);
  ModelNodeRuntime_RebuildTransformsFromRoot(rightNode);
  leftNode = leftNode->childNodes[0];
  rightNode = rightNode->childNodes[0];
  leftLegVector = FixedMath_VectorToAnglesAndLength
                     (leftContactZ - (leftNode->worldTransform).translation.z,
                      leftAnkleY - (leftNode->worldTransform).translation.y,
                      leftAnkleX - (leftNode->worldTransform).translation.x);
  leftLegRelativeAzimuth = leftLegVector.azimuthAngle - (modelNodeRuntime->modelPayload).worldRotationAngle2 & FIXED_ANGLE16_MASK;
  leftLegElevation = leftLegVector.elevationAngle + FIXED_ANGLE16_QUARTER_TURN;
  if ((FIXED_ANGLE16_QUARTER_TURN - 1 < leftLegRelativeAzimuth) && (leftLegRelativeAzimuth < FIXED_ANGLE16_THREE_QUARTER_TURN + 1)) {
    leftLegElevation = -leftLegElevation;
  }
  rightLegVector = FixedMath_VectorToAnglesAndLength
                     (rightContactZ - (rightNode->worldTransform).translation.z,
                      rightAnkleY - (rightNode->worldTransform).translation.y,
                      rightAnkleX - (rightNode->worldTransform).translation.x);
  rightLegRelativeAzimuth = rightLegVector.azimuthAngle - (modelNodeRuntime->modelPayload).worldRotationAngle2 & FIXED_ANGLE16_MASK;
  rightLegElevation = rightLegVector.elevationAngle + FIXED_ANGLE16_QUARTER_TURN;
  if ((FIXED_ANGLE16_QUARTER_TURN - 1 < rightLegRelativeAzimuth) && (rightLegRelativeAzimuth < FIXED_ANGLE16_THREE_QUARTER_TURN + 1)) {
    rightLegElevation = -rightLegElevation;
  }
  /* bone lengths from the left leg (knee and ankle joint offsets), used for both legs */
  leftNode = leftNode->childNodes[0];
  thighLengthQ12 = FixedMath_LengthVec3
                     ((GraphicsFixedVec3 *)&(leftNode->modelPayload).localTranslationXQ12);
  shinLengthQ12 =
       FixedMath_LengthVec3
                 ((GraphicsFixedVec3 *)
                  &(leftNode->childNodes[0]->modelPayload).localTranslationXQ12);
  leftNode = modelNodeRuntime->childNodes[0]->childNodes[0];
  rightNode = modelNodeRuntime->childNodes[1]->childNodes[0];
  jointAngles = FixedGeometry_SolveTriangleJointAngles(shinLengthQ12,thighLengthQ12,leftLegVector.lengthQ12);
  leftThighBend = jointAngles.jointAngle0 - leftLegElevation;
  if (leftThighBend < 0) {
    (leftNode->modelPayload).localRotationAngle0 = FIXED_ANGLE16_HALF_TURN;
    (leftNode->modelPayload).localRotationAngle1 = leftThighBend + FIXED_ANGLE16_QUARTER_TURN;
  }
  else {
    (leftNode->modelPayload).localRotationAngle0 = 0;
    (leftNode->modelPayload).localRotationAngle1 = FIXED_ANGLE16_QUARTER_TURN - leftThighBend;
  }
  leftNode = leftNode->childNodes[0];
  (leftNode->modelPayload).localRotationAngle1 = FIXED_ANGLE16_QUARTER_TURN - jointAngles.jointAngle1;
  jointAngles = FixedGeometry_SolveTriangleJointAngles(shinLengthQ12,thighLengthQ12,rightLegVector.lengthQ12);
  rightThighBend = jointAngles.jointAngle0 - rightLegElevation;
  if (rightThighBend < 0) {
    (rightNode->modelPayload).localRotationAngle0 = 0;
    (rightNode->modelPayload).localRotationAngle1 = rightThighBend + FIXED_ANGLE16_QUARTER_TURN;
  }
  else {
    (rightNode->modelPayload).localRotationAngle0 = FIXED_ANGLE16_HALF_TURN;
    (rightNode->modelPayload).localRotationAngle1 = FIXED_ANGLE16_QUARTER_TURN - rightThighBend;
  }
  rightNode = rightNode->childNodes[0];
  (rightNode->modelPayload).localRotationAngle1 = FIXED_ANGLE16_QUARTER_TURN - jointAngles.jointAngle1;
  leftNode = leftNode->childNodes[0];
  rightNode = rightNode->childNodes[0];
  (leftNode->modelPayload).localRotationAngle0 = 0;
  (leftNode->modelPayload).localRotationAngle1 = FIXED_ANGLE16_QUARTER_TURN;
  (leftNode->modelPayload).localRotationAngle2 = 0;
  (rightNode->modelPayload).localRotationAngle0 = 0;
  (rightNode->modelPayload).localRotationAngle1 = FIXED_ANGLE16_QUARTER_TURN;
  (rightNode->modelPayload).localRotationAngle2 = 0;
  modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
  /* foot orientation: foot heading interpolated by progress (unsigned scaling for the left foot, signed for
     the right, as in the original) with the blended normal angles */
  ArticulatedWalker_SetFootLocalOrientation
            (leftNode,
             (((leftHeading - leftStartHeading) * ARMY_ANGLE16_SIGN_EXTEND_SCALE >> 16) * leftBlendQ12 >> Q12_SHIFT) +
             leftStartHeading & FIXED_ANGLE16_MASK,leftBlendAngles);
  ArticulatedWalker_SetFootLocalOrientation
            (rightNode,
             (((rightHeading - rightStartHeading) * ARMY_ANGLE16_SIGN_EXTEND_SCALE >> 16) * rightBlendQ12 >> Q12_SHIFT) +
             rightStartHeading & FIXED_ANGLE16_MASK,rightBlendAngles);
  modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
  return;
}


/* Sets the step rate of a planned foot step (articulatedContact.fallbackPosition1Q12): stride length
   (definition classParameterC0) << 13 divided by footTravel plus 4 * lift height (definition
   classParameterC4), or 2.0 when that
   sum is 0. Returns false in that case. */
static bool ArmyArticulatedRuntime_SetStepRate(ArmyArticulatedRuntimeSlotView *armyRuntime,uint32_t footTravel)

{
  ModelDefinition *movementDefinition;
  int travelPlusLift;
  int strideLength;

  movementDefinition = (ModelDefinition *)armyRuntime->definitionOrAsset;
  travelPlusLift = footTravel + movementDefinition->classParameterC4 * 4;
  strideLength = movementDefinition->classParameterC0;
  (armyRuntime->articulatedContact).fallbackPosition1Q12 = 2 * Q12_ONE;
  if (travelPlusLift == 0) {
    return false;
  }
  (armyRuntime->articulatedContact).fallbackPosition1Q12 =
       (Q12)((int64_t)(uint64_t)(uint32_t)(strideLength << 13) / (int64_t)travelPlusLift);
  return true;
}

/* Plans a closing step of the left foot: the body heading target and the left foot heading become
   headingAngle16, and the left foot target is set right beside the right foot's target (2 * lateralOffsetQ12
   to the left) at the ground height there. The step rate uses the foot travel plus 4 * lift height. Called by
   ArmyRuntimeClass_UpdateArticulatedMovement for the second half of a turn and for closing steps; the walker
   state layout is described at ArmyArticulatedRuntime_InitializeTerrainContactGeometry.
*/
void ArmyArticulatedRuntime_InitializeLeftTerrainContact
          (AngleTurn16Stored32 headingAngle16,ArmyArticulatedRuntimeSlotView *armyRuntime,
          WorldRuntimeContext *worldRuntime)

{
  uint32_t footTravel;
  uint32_t sideAngle;
  FixedSinCos offsetSinCos;
  Q12 terrainHeightQ12;
  uint32_t terrainNormalAngles;

  armyRuntime->ownerValue64 = headingAngle16;
  (armyRuntime->linkedChildOverloadedState).leftHeadingCommandOrSpawnValue.signedValue =
       headingAngle16;
  sideAngle = headingAngle16 + FIXED_ANGLE16_QUARTER_TURN & FIXED_ANGLE16_MASK;
  offsetSinCos = FixedMath_SinCosScaled(sideAngle,(armyRuntime->articulatedContact).lateralOffsetQ12);
  armyRuntime->runtimeState90 = offsetSinCos.cosValue * 2 + armyRuntime->runtimeState94;
  armyRuntime->runtimeState98 =
       offsetSinCos.sinValue * 2 + armyRuntime->articulatedCoordinateOrState9C;
  offsetSinCos = FixedMath_SinCosScaled
                    (sideAngle,(armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.
                           contactRadiusQ12);
  if (worldRuntime->fieldGrid != NULL) {
    if (FieldGrid_InterpolateTerrainHeightAndNormal
                      (offsetSinCos.sinValue + armyRuntime->runtimeState98,
                       offsetSinCos.cosValue + armyRuntime->runtimeState90,worldRuntime->fieldGrid,
                       &terrainHeightQ12,&terrainNormalAngles)) {
      armyRuntime->articulatedHeightOrStateA0 = terrainHeightQ12;
      armyRuntime->fallbackWorldYQ12 = terrainNormalAngles;
      footTravel = FixedMath_Length3(armyRuntime->articulatedHeightOrStateA0 -
                                armyRuntime->definitionClassValue88,
                                armyRuntime->runtimeState98 - armyRuntime->definitionClassValue80,
                                armyRuntime->runtimeState90 - armyRuntime->movementTarget0Q12);
      if (ArmyArticulatedRuntime_SetStepRate(armyRuntime,footTravel)) {
        (armyRuntime->articulatedContact).fallbackPosition0Q12 &= ~ARMY_ARTICULATED_STEP_OBSTRUCTED;
      }
    }
  }
  return;
}


/* Mirror of ArmyArticulatedRuntime_InitializeLeftTerrainContact for the right foot (set beside the left
   foot's target, side angle heading - 90 degrees). Called by ArmyRuntimeClass_UpdateArticulatedMovement.
*/
void ArmyArticulatedRuntime_InitializeRightTerrainContact
          (AngleTurn16Stored32 headingAngle16,ArmyArticulatedRuntimeSlotView *armyRuntime,
          WorldRuntimeContext *worldRuntime)

{
  uint32_t footTravel;
  uint32_t sideAngle;
  FixedSinCos offsetSinCos;
  Q12 terrainHeightQ12;
  uint32_t terrainNormalAngles;

  armyRuntime->ownerValue64 = headingAngle16;
  (armyRuntime->linkedChildOverloadedState).rightHeadingCommandOrSpawnValue.signedValue =
       headingAngle16;
  sideAngle = headingAngle16 - FIXED_ANGLE16_QUARTER_TURN & FIXED_ANGLE16_MASK;
  offsetSinCos = FixedMath_SinCosScaled(sideAngle,(armyRuntime->articulatedContact).lateralOffsetQ12);
  armyRuntime->runtimeState94 = offsetSinCos.cosValue * 2 + armyRuntime->runtimeState90;
  armyRuntime->articulatedCoordinateOrState9C =
       offsetSinCos.sinValue * 2 + armyRuntime->runtimeState98;
  offsetSinCos = FixedMath_SinCosScaled
                    (sideAngle,(armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.
                           contactRadiusQ12);
  if (worldRuntime->fieldGrid != NULL) {
    if (FieldGrid_InterpolateTerrainHeightAndNormal
                      (offsetSinCos.sinValue + armyRuntime->articulatedCoordinateOrState9C,
                       offsetSinCos.cosValue + armyRuntime->runtimeState94,worldRuntime->fieldGrid,
                       &terrainHeightQ12,&terrainNormalAngles)) {
      armyRuntime->runtimeStateA4 = terrainHeightQ12;
      armyRuntime->fallbackWorldXQ12 = terrainNormalAngles;
      footTravel = FixedMath_Length3(armyRuntime->runtimeStateA4 - armyRuntime->runtimeState8C,
                                armyRuntime->articulatedCoordinateOrState9C -
                                armyRuntime->definitionClassValue84,
                                armyRuntime->runtimeState94 - armyRuntime->movementTarget1Q12);
      if (ArmyArticulatedRuntime_SetStepRate(armyRuntime,footTravel)) {
        (armyRuntime->articulatedContact).fallbackPosition0Q12 &= ~ARMY_ARTICULATED_STEP_OBSTRUCTED;
      }
    }
  }
  return;
}


/* Left turn of ArmyArticulatedRuntime_UpdateSelectedTerrainContact (steeringAngle16 up to 0x8000): clamps
   the steering angle to the maximum turn per step, plans the left foot target and heading, the body heading
   target, the stored half turn and the step rate. */
static void ArmyArticulatedRuntime_PlanLeftTurnStep
          (AngleTurn32 steeringAngle16,ArmyArticulatedRuntimeSlotView *armyRuntime,
          WorldRuntimeContext *worldRuntime)

{
  ModelDefinition *movementDefinition;
  ModelRuntimeNode *rootNode;
  FieldGridAsset *fieldGrid;
  int64_t scaledOffsetProduct;
  uint32_t pivotDistance;
  uint32_t footHeading;
  int pivotX;
  int pivotY;
  int stepCenterX;
  int stepCenterY;
  uint32_t footTargetX;
  uint32_t footTargetY;
  int footTargetHeight;
  uint32_t footTravel;
  FixedSinCos offsetSinCos;
  Q12 terrainHeightQ12;
  uint32_t terrainNormalAngles;

  /* definition classParameterC8: maximum turn per step */
  movementDefinition = (ModelDefinition *)armyRuntime->definitionOrAsset;
  if ((uint32_t)movementDefinition->classParameterC8 < steeringAngle16) {
    steeringAngle16 = (AngleTurn32)movementDefinition->classParameterC8;
  }
  /* pivot 1.5 * lateral offset behind the root */
  rootNode = armyRuntime->modelNodeRuntime;
  footHeading = armyRuntime->classState60 + FIXED_ANGLE16_HALF_TURN & FIXED_ANGLE16_MASK;
  scaledOffsetProduct = (int64_t)(armyRuntime->articulatedContact).lateralOffsetQ12 * (3 * Q12_ONE / 2);
  pivotDistance = FIXED_PRODUCT_SHR(scaledOffsetProduct,Q12_SHIFT);
  offsetSinCos = FixedMath_SinCosScaled(footHeading,pivotDistance);
  pivotX = offsetSinCos.cosValue + (rootNode->worldTransform).translation.x;
  pivotY = offsetSinCos.sinValue + (rootNode->worldTransform).translation.y;
  /* rotate forward from the pivot by the steering angle, then step out to the left */
  footHeading = (footHeading + steeringAngle16) - FIXED_ANGLE16_HALF_TURN & FIXED_ANGLE16_MASK;
  (armyRuntime->linkedChildOverloadedState).leftHeadingCommandOrSpawnValue.headingOrTurnValue =
       footHeading;
  offsetSinCos = FixedMath_SinCosScaled(footHeading,pivotDistance);
  stepCenterX = pivotX + offsetSinCos.cosValue;
  stepCenterY = pivotY + offsetSinCos.sinValue;
  footHeading = footHeading + FIXED_ANGLE16_QUARTER_TURN & FIXED_ANGLE16_MASK;
  offsetSinCos = FixedMath_SinCosScaled(footHeading,(armyRuntime->articulatedContact).lateralOffsetQ12);
  armyRuntime->runtimeState90 = offsetSinCos.cosValue + stepCenterX;
  armyRuntime->runtimeState98 = offsetSinCos.sinValue + stepCenterY;
  offsetSinCos = FixedMath_SinCosScaled
                     (footHeading,(armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.
                            contactRadiusQ12);
  fieldGrid = worldRuntime->fieldGrid;
  armyRuntime->ownerValue64 = ((int)steeringAngle16 >> 1) + armyRuntime->classState60 & FIXED_ANGLE16_MASK;
  if (fieldGrid != NULL &&
      FieldGrid_InterpolateTerrainHeightAndNormal
                (offsetSinCos.sinValue + armyRuntime->runtimeState98,
                 offsetSinCos.cosValue + armyRuntime->runtimeState90,fieldGrid,
                 &terrainHeightQ12,&terrainNormalAngles)) {
    armyRuntime->articulatedHeightOrStateA0 = terrainHeightQ12;
    armyRuntime->fallbackWorldYQ12 = terrainNormalAngles;
  }
  /* upper 16 bits of the step state = steeringAngle16 / 2 (bit 15 gets its lowest bit) */
  (armyRuntime->articulatedContact).fallbackPosition0Q12 =
       (armyRuntime->articulatedContact).fallbackPosition0Q12 & 0xffff;
  footTargetX = armyRuntime->runtimeState90;
  footTargetY = armyRuntime->runtimeState98;
  footTargetHeight = armyRuntime->articulatedHeightOrStateA0;
  (armyRuntime->articulatedContact).fallbackPosition0Q12 =
       (armyRuntime->articulatedContact).fallbackPosition0Q12 | steeringAngle16 << 15;
  footTravel = FixedMath_Length3(footTargetHeight - armyRuntime->definitionClassValue88,
                                 footTargetY - armyRuntime->definitionClassValue80,
                                 footTargetX - armyRuntime->movementTarget0Q12);
  ArmyArticulatedRuntime_SetStepRate(armyRuntime,footTravel);
}

/* Right turn of ArmyArticulatedRuntime_UpdateSelectedTerrainContact (steeringAngle16 above 0x8000): the same
   with the right foot and the negative steering angle steeringAngle16 - 0x10000. */
static void ArmyArticulatedRuntime_PlanRightTurnStep
          (AngleTurn32 steeringAngle16,ArmyArticulatedRuntimeSlotView *armyRuntime,
          WorldRuntimeContext *worldRuntime)

{
  ModelDefinition *movementDefinition;
  ModelRuntimeNode *rootNode;
  FieldGridAsset *fieldGrid;
  uint32_t minimumSteeringAngle;
  int signedSteeringAngle;
  int64_t scaledOffsetProduct;
  uint32_t pivotDistance;
  uint32_t footHeading;
  int pivotX;
  int pivotY;
  int stepCenterX;
  int stepCenterY;
  uint32_t footTargetX;
  int footTargetY;
  uint32_t footTargetHeight;
  uint32_t footTravel;
  FixedSinCos offsetSinCos;
  Q12 terrainHeightQ12;
  uint32_t terrainNormalAngles;

  /* definition classParameterC8: maximum turn per step */
  movementDefinition = (ModelDefinition *)armyRuntime->definitionOrAsset;
  minimumSteeringAngle = FIXED_ANGLE16_FULL_TURN - movementDefinition->classParameterC8;
  if (steeringAngle16 < minimumSteeringAngle) {
    steeringAngle16 = minimumSteeringAngle;
  }
  rootNode = armyRuntime->modelNodeRuntime;
  signedSteeringAngle = steeringAngle16 - FIXED_ANGLE16_FULL_TURN;
  footHeading = armyRuntime->classState60 + FIXED_ANGLE16_HALF_TURN & FIXED_ANGLE16_MASK;
  scaledOffsetProduct = (int64_t)(armyRuntime->articulatedContact).lateralOffsetQ12 * (3 * Q12_ONE / 2);
  pivotDistance = FIXED_PRODUCT_SHR(scaledOffsetProduct,Q12_SHIFT);
  offsetSinCos = FixedMath_SinCosScaled(footHeading,pivotDistance);
  pivotX = offsetSinCos.cosValue + (rootNode->worldTransform).translation.x;
  pivotY = offsetSinCos.sinValue + (rootNode->worldTransform).translation.y;
  footHeading = (footHeading + signedSteeringAngle) - FIXED_ANGLE16_HALF_TURN & FIXED_ANGLE16_MASK;
  (armyRuntime->linkedChildOverloadedState).rightHeadingCommandOrSpawnValue.headingOrTurnValue =
       footHeading;
  offsetSinCos = FixedMath_SinCosScaled(footHeading,pivotDistance);
  stepCenterX = pivotX + offsetSinCos.cosValue;
  stepCenterY = pivotY + offsetSinCos.sinValue;
  footHeading = footHeading - FIXED_ANGLE16_QUARTER_TURN & FIXED_ANGLE16_MASK;
  offsetSinCos = FixedMath_SinCosScaled(footHeading,(armyRuntime->articulatedContact).lateralOffsetQ12);
  armyRuntime->runtimeState94 = offsetSinCos.cosValue + stepCenterX;
  armyRuntime->articulatedCoordinateOrState9C = offsetSinCos.sinValue + stepCenterY;
  offsetSinCos = FixedMath_SinCosScaled
                     (footHeading,(armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.
                            contactRadiusQ12);
  fieldGrid = worldRuntime->fieldGrid;
  armyRuntime->ownerValue64 = (signedSteeringAngle >> 1) + armyRuntime->classState60 & FIXED_ANGLE16_MASK;
  if (fieldGrid != NULL &&
      FieldGrid_InterpolateTerrainHeightAndNormal
                (offsetSinCos.sinValue + armyRuntime->articulatedCoordinateOrState9C,
                 offsetSinCos.cosValue + armyRuntime->runtimeState94,fieldGrid,
                 &terrainHeightQ12,&terrainNormalAngles)) {
    armyRuntime->runtimeStateA4 = terrainHeightQ12;
    armyRuntime->fallbackWorldXQ12 = terrainNormalAngles;
  }
  (armyRuntime->articulatedContact).fallbackPosition0Q12 =
       (armyRuntime->articulatedContact).fallbackPosition0Q12 & 0xffff;
  footTargetX = armyRuntime->runtimeState94;
  footTargetY = armyRuntime->articulatedCoordinateOrState9C;
  footTargetHeight = armyRuntime->runtimeStateA4;
  (armyRuntime->articulatedContact).fallbackPosition0Q12 =
       (armyRuntime->articulatedContact).fallbackPosition0Q12 |
       signedSteeringAngle * ARMY_ARTICULATED_STEP_HALF_TURN_SCALE;
  footTravel = FixedMath_Length3(footTargetHeight - armyRuntime->runtimeState8C,
                                 footTargetY - armyRuntime->definitionClassValue84,
                                 footTargetX - armyRuntime->movementTarget1Q12);
  ArmyArticulatedRuntime_SetStepRate(armyRuntime,footTravel);
}

/* Plans the first step of a turn on the spot. steeringAngle16 (0..0xFFFF, clamped to +-the maximum turn per
   step, definition classParameterC8) selects the foot: up to 0x8000 the left foot (turning left), otherwise the right
   foot. The foot target is found by turning the root position about a pivot 1.5 * lateralOffsetQ12 behind it
   by the full steering angle and stepping out to that side; the body heading target becomes the start heading
   plus half the steering angle, and that half is also kept in the upper 16 bits of the step state for the
   closing step. The step rate uses the foot travel plus 4 * lift height. Called by
   ArmyRuntimeClass_UpdateArticulatedMovement; the walker state layout is described at
   ArmyArticulatedRuntime_InitializeTerrainContactGeometry.
*/
void ArmyArticulatedRuntime_UpdateSelectedTerrainContact
          (AngleTurn32 steeringAngle16,ArmyArticulatedRuntimeSlotView *armyRuntime,
          WorldRuntimeContext *worldRuntime)

{
  if (steeringAngle16 < FIXED_ANGLE16_HALF_TURN + 1) {
    ArmyArticulatedRuntime_PlanLeftTurnStep(steeringAngle16,armyRuntime,worldRuntime);
  }
  else {
    ArmyArticulatedRuntime_PlanRightTurnStep(steeringAngle16,armyRuntime,worldRuntime);
  }
  return;
}


/* Called by a weapon that is aimed and ready to fire. When the shot to the target is blocked, the owning army
   (if this weapon is its primary weapon or it has none, and it is not already following) starts a
   target-following move towards the target, clamped for AI combat targets, and true is returned: the weapon
   must not fire. With a clear line of fire a running target-following move is stopped and false is returned. Called
   by the aim-and-fire class updates (runtime-update slots 7 and 8 here, and gameplay/army/combat.c).
*/
bool ArmyRuntimeCommand_UpdateTargetFollowingState(Q12 targetWorldZQ12,Q12 targetWorldYQ12,Q12 targetWorldXQ12,
          WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  GameEntityRuntime *ownerEntity;
  ModelRuntimeSlot *ownerRootModelRuntime;
  bool lineOfFireBlocked;

  /* modelRuntime is the weapon's model runtime. ownerEntity is the owning army (GameEntityRuntime view: its
     common.commandFlags is ArmyRuntimeSlot.movementStateFlags and commandTarget.targetFlags is commandModeFlags);
     the army's root model runtime carries the primary weapon as its first attachment */
  ownerEntity = modelRuntime->ownerArmyRuntimeOrSavedOffset.entityRuntime;
  lineOfFireBlocked = ArmyWeaponRuntime_TestTargetLineOfFire
                    (targetWorldZQ12,targetWorldYQ12,targetWorldXQ12,worldRuntime,modelRuntime);
  ownerRootModelRuntime = (ownerEntity->common).ownership.definitionOrClassRecord;
  if (lineOfFireBlocked) {
    if (((modelRuntime == ownerRootModelRuntime->attachments[0].childModelRuntimeOrSavedOffset) ||
        (ownerRootModelRuntime->attachments[0].childModelRuntimeOrSavedOffset == NULL)
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
   fallback position is first saved into the waypoint queue (apparently so the army resumes it afterwards).
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
bool ArmyRuntime_UpdateMovementAndWaypoints
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
  bool belowThreshold;
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

