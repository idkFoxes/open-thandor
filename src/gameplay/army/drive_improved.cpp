/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/drive_improved.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/army/drive_improved.h>
#include <thandor/thandor.h>
#include <thandor/platform/debug/hooks.h>

/* open-thandor: movement of the improved pathing mode (docs/plans/pathfinding.md 5, P1 and P6). Only reached
   behind Pathing_Improved(); see drive_improved.h. */

/* Absolute heading difference of two 16-bit angles, 0..FIXED_ANGLE16_HALF_TURN. */
static uint32_t ArmyDriveImproved_AbsoluteAngleDifference(uint32_t angleA,uint32_t angleB)

{
  uint32_t difference;

  difference = angleA - angleB & FIXED_ANGLE16_MASK;
  if (FIXED_ANGLE16_HALF_TURN < difference) {
    difference = FIXED_ANGLE16_FULL_TURN - difference;
  }
  return difference;
}

/* True when the route point is the end of the current move: the plan's fallback position (within the route end
   radius ArmyRuntime_UpdateMovementAndWaypoints uses) with no queued waypoint after it, or no move at all. */
static bool ArmyDriveImproved_IsFinalRoutePoint(ArmyRuntimeSlot *ownerArmy,Q12 waypointWorldXQ12,Q12 waypointWorldYQ12)

{
  ArmyMovementRuntime *movementRuntime;
  int offsetX;
  int offsetY;

  movementRuntime = ModelView_Cast<ArmyMovementRuntime>(ownerArmy);
  if (!Any(movementRuntime->movementStateFlags & ARMY_MOVEMENT_ACTIVE)) {
    return true;
  }
  if (Any(movementRuntime->movementStateFlags & ARMY_MOVEMENT_WAYPOINTS_QUEUED)) {
    return false;
  }
  offsetX = (movementRuntime->fallbackPosition).worldXQ12 - waypointWorldXQ12;
  offsetY = (movementRuntime->fallbackPosition).worldYQ12 - waypointWorldYQ12;
  return (offsetX < ARMY_MOVEMENT_ROUTE_END_RADIUS_Q12) && (offsetY < ARMY_MOVEMENT_ROUTE_END_RADIUS_Q12) &&
         (-ARMY_MOVEMENT_ROUTE_END_RADIUS_Q12 < offsetX) && (-ARMY_MOVEMENT_ROUTE_END_RADIUS_Q12 < offsetY);
}

bool ArmyDriveImproved_UpdateGroundAdvance
          (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView *modelRuntime,
           Q12 waypointWorldXQ12,Q12 waypointWorldYQ12,uint32_t facingAngle,FixedLengthAngle routeAngleAndLength,
           bool *coasting)

{
  DebugHook_ProfileScope(DEBUG_PROFILE_DRIVE_IMPROVED);
  ModelDefinitionGroundMovementSteeringView *movementDefinition;
  ModelDefinition *definition;
  uint32_t headingError;
  uint32_t headingErrorLimit;
  uint32_t farLimit;
  int previousAdvance;
  int newAdvance;
  int cappedAdvance;
  int decelerationStep;
  int slowedAdvance;
  int sineQ16;
  int64_t turnRateLimit;
  int64_t reachableTravel;
  int64_t reachableAdvance;

  *coasting = false;
  movementDefinition = modelRuntime->modelDefinition;
  definition = ModelView_Cast<ModelRuntimeSlot>(modelRuntime)->definitionOrSavedId.runtimeDefinition;
  headingError = ArmyDriveImproved_AbsoluteAngleDifference(facingAngle,routeAngleAndLength.angle);
  /* allowed heading error: a quarter turn; the final route point narrows it towards the near limit as the
     original does for every route point */
  headingErrorLimit = FIXED_ANGLE16_QUARTER_TURN;
  if (ArmyDriveImproved_IsFinalRoutePoint(modelRuntime->ownerArmyRuntime,waypointWorldXQ12,waypointWorldYQ12) &&
      ((int)routeAngleAndLength.length < movementDefinition->headingErrorInterpolationDistanceQ12)) {
    farLimit = FIXED_ANGLE16_QUARTER_TURN;
    if (movementDefinition->nearHeadingErrorLimitAngle < farLimit) {
      headingErrorLimit = movementDefinition->nearHeadingErrorLimitAngle +
               (uint32_t)(((int64_t)(farLimit - movementDefinition->nearHeadingErrorLimitAngle) *
                           (int64_t)(int)routeAngleAndLength.length) /
                          (int64_t)movementDefinition->headingErrorInterpolationDistanceQ12);
    }
  }
  previousAdvance = (modelRuntime->movementControl).movementAdvancePerTickQ12;
  if (headingErrorLimit < headingError) {
    /* coast: roll on along the facing while slowing down, then turn on the spot */
    newAdvance = previousAdvance >> 1;
    if (newAdvance <= definition->accelerationPerTick) {
      newAdvance = 0;
    }
    (modelRuntime->movementControl).movementAdvancePerTickQ12 = newAdvance;
    *coasting = true;
    return newAdvance != 0;
  }
  ArmyRuntime_UpdateActivationMetricAndPlayStartSound(worldRuntime,ModelView_Cast<ModelRuntimeSlot>(modelRuntime));
  /* drive while turning: the advance is capped at movementSpeed * cos(heading error) (0..1 for 0..90 degrees,
     Q28 sine table) and reached by deceleration, at least accelerationPerTick or a quarter of the advance per step */
  cappedAdvance = FixedMath_SinCosScaled(headingError,definition->movementSpeed).cosValue;
  /* and at the advance whose turning circle still reaches the route point, else the unit circles around it:
     a point at distance d and heading error e lies on the circle of radius d / (2 sin e); at the turn rate limit
     w (angle16 per step) the circle of a step travel s has radius s * 65536 / (2 pi w), so
     s <= d * w * pi / (65536 * sin e); with sin e in Q16 and pi ~ 3217 / 1024: s <= d * w * 3217 / (1024 * sinQ16).
     Half of it as margin (the turn velocity first has to accelerate). */
  sineQ16 = FixedMath_SinCosScaled(headingError,0x10000).sinValue;
  turnRateLimit = (int64_t)movementDefinition->turnRateLimitAnglePerTick * g_InGameSimulationStepTicks;
  if ((0 < sineQ16) && (0 < turnRateLimit) && (0 < g_InGameSimulationStepTicks)) {
    reachableTravel = ((int64_t)(int)routeAngleAndLength.length * turnRateLimit * 3217) / ((int64_t)sineQ16 * 2048);
    reachableAdvance = reachableTravel / g_InGameSimulationStepTicks;
    if (reachableAdvance < cappedAdvance) {
      cappedAdvance = (int)reachableAdvance;
    }
  }
  newAdvance = (modelRuntime->movementControl).movementAdvancePerTickQ12;
  if (cappedAdvance < newAdvance) {
    decelerationStep = previousAdvance >> 2;
    if (decelerationStep < definition->accelerationPerTick) {
      decelerationStep = definition->accelerationPerTick;
    }
    slowedAdvance = previousAdvance - decelerationStep;
    if (slowedAdvance < cappedAdvance) {
      slowedAdvance = cappedAdvance;
    }
    if (slowedAdvance < newAdvance) {
      newAdvance = slowedAdvance;
    }
    if (newAdvance < 0) {
      newAdvance = 0;
    }
    (modelRuntime->movementControl).movementAdvancePerTickQ12 = newAdvance;
  }
  return newAdvance != 0;
}

void ArmyDriveImproved_KeepBankingSpeedOnNewRoutePoint
          (ModelRuntimeGroundMovementSteeringView *modelRuntime,ModelRuntimeNode *rootNode,
           Q12 oldWaypointWorldXQ12,Q12 oldWaypointWorldYQ12,Q12 waypointWorldXQ12,Q12 waypointWorldYQ12)

{
  int advance;
  int oldDeltaX;
  int oldDeltaY;
  int newDeltaX;
  int newDeltaY;
  uint32_t oldTravelAngle;
  uint32_t newTravelAngle;
  uint32_t directionChange;

  (modelRuntime->movementControl).turnVelocityAngle16 = 0;
  advance = (modelRuntime->movementControl).movementAdvancePerTickQ12;
  if (advance <= 0) {
    (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
    return;
  }
  newDeltaX = waypointWorldXQ12 - (rootNode->worldTransform).translation.x;
  newDeltaY = waypointWorldYQ12 - (rootNode->worldTransform).translation.y;
  if ((newDeltaX == 0) && (newDeltaY == 0)) {
    (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
    return;
  }
  newTravelAngle = FixedMath_Vector2AngleAndLength(newDeltaY,newDeltaX).angle;
  oldDeltaX = oldWaypointWorldXQ12 - (rootNode->worldTransform).translation.x;
  oldDeltaY = oldWaypointWorldYQ12 - (rootNode->worldTransform).translation.y;
  if ((oldDeltaX == 0) && (oldDeltaY == 0)) {
    oldTravelAngle = (modelRuntime->classLinkState).classState64;
  }
  else {
    oldTravelAngle = FixedMath_Vector2AngleAndLength(oldDeltaY,oldDeltaX).angle;
  }
  directionChange = ArmyDriveImproved_AbsoluteAngleDifference(newTravelAngle,oldTravelAngle);
  if (FIXED_ANGLE16_QUARTER_TURN <= directionChange) {
    (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
    return;
  }
  (modelRuntime->movementControl).movementAdvancePerTickQ12 =
       FixedMath_SinCosScaled(directionChange,advance).cosValue;
}
