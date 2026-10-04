/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/walker.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/army/walker.h>
#include <thandor/thandor.h>

/* Module data. */

static GraphicsFixedVec3 g_ArmySuspensionBlendVectorA = {0, 0, 0};

static GraphicsFixedVec3 g_ArmySuspensionBlendVectorB = {0, 0, 0};

static GraphicsFixedMatrix3x4 g_ArmySuspensionRotationMatrixScratchA = {0};

static GraphicsFixedMatrix3x4 g_ArmySuspensionRotationMatrixScratchB = {0};

static GraphicsFixedMatrix3x4 g_ArmySuspensionRotationMatrixComposedScratch = {0};

/* What ArticulatedWalker_TryStartRouteStep decided for a standing walker. */
enum {
  ARTICULATED_ROUTE_STEP_NONE,             /* nothing started: check whether the feet need closing */
  ARTICULATED_ROUTE_STEP_STARTED,          /* a walking step or a turn on the spot has been started */
  ARTICULATED_ROUTE_STEP_CLOSE_FEET,       /* walking ended off the walk-on angle: close the feet */
  ARTICULATED_ROUTE_STEP_ADVANCE_WAYPOINT  /* the route point is reached */
};
using ArticulatedRouteStep = int;

/* Implementation ownership: gameplay/army/walker. */

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
           Thandor_U32ToPointer<ArmyRuntimeSlot>(modelRuntime->fallbackWorldXQ12); /* 5f-format: ArmyRuntimeSlot.linkedArmyRuntimeOrSavedOffset (Q12 overlay) */
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
static Bool8 ArticulatedWalker_StepEndReachedRoutePoint(WorldRuntimeContext *worldRuntime,
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
      targetAngleLength = THANDOR_COMPOUND(FixedLengthAngle){ .length = 0, .angle = (rootNode->modelPayload).worldRotationAngle2 };
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
static Bool8 ArticulatedWalker_FeetNeedClosing(ModelRuntimeArticulatedMovementDefinitionView *modelRuntime,
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
static Bool8 ArticulatedWalker_ChooseNextStep(WorldRuntimeContext *worldRuntime,
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
  if ((linkedModelRuntime != nullptr) &&
     ((linkedModelRuntime->definitionOrSavedId.runtimeDefinition->footprintRadius == 0) ||
      (modelRuntime->modelDefinition->footprintRadius == 0) ||
      !ArmyCollision_TestPointWithinExpandedRuntimeRadius
             (modelRuntime->modelDefinition->footprintRadius,(rootNode->worldTransform).translation.y,
              (rootNode->worldTransform).translation.x,linkedModelRuntime))) {
    modelRuntime->linkedModelRuntime = nullptr;
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

/* Puts the two feet of an articulated walker at rest beside its root: both feet (current position and step
   target) lateralOffsetQ12 to the left and right of the root at the terrain height under the root, all
   headings equal to the root heading, and both ground normals pointing straight up. Called by
   ArmyPlacementContact_InitializeArticulatedSuspension (gameplay/army/placement_contact.cpp, placement contact kind 3)
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
  if (worldRuntime->fieldGrid != nullptr) {
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
  if (activeFieldGrid == nullptr) {
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
    if (blockingModelRuntime == nullptr) {
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
  if (activeFieldGrid == nullptr) {
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
    if (blockingModelRuntime == nullptr) {
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
  Bool8 cellMasked;
  ModelRuntimeNode *footNode;

  definition = modelRuntime->definitionOrSavedId.runtimeDefinition;
  soundIndex = definition->classParameterCC;
  voiceSetRef = nullptr;
  if ((soundIndex != 0) && (soundIndex < worldRuntime->dwordArrayCount) && (worldRuntime->dwordArray != nullptr)) {
    voiceSetRef = (DirectSoundVoiceSet **)worldRuntime->dwordArray[soundIndex];
  }
  if (voiceSetRef != nullptr) {
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
  if (effectDefinition != nullptr) {
    EffectRuntimePool_CreateInstanceFromDefinition
              (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_COMPOUND(EffectRuntimeOwnerReference){ .modelNode = nullptr },
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
                       (articulatedRuntime->fallbackWorldXQ12,(int)articulatedRuntime->linkedArmyRuntimeOrSavedOffset, /* 5f-format: ArmyRuntimeSlot.linkedArmyRuntimeOrSavedOffset (Q12 overlay) */
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
static Bool8 ArmyArticulatedRuntime_SetStepRate(ArmyArticulatedRuntimeSlotView *armyRuntime,uint32_t footTravel)

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
  if (worldRuntime->fieldGrid != nullptr) {
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
  if (worldRuntime->fieldGrid != nullptr) {
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
  if (fieldGrid != nullptr &&
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
  if (fieldGrid != nullptr &&
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
