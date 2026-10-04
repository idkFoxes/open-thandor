/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/drive_ground.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/army/drive_ground.h>
#include <thandor/thandor.h>

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
}

/* Recoil after a shot: tilts the model away from the shot direction (actionVector0Q12 + 180 degrees) by
   recoilTilt. Also used by the banking movement (drive_banking). */
void ArmyGroundMovement_ApplyRecoilTilt
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

/* Accelerated turning of the ground movement callbacks. Turns by the current turn velocity, then accelerates it (clamped to
   the turn rate limit); a turn in the other direction first resets the velocity, a small remaining error
   snaps onto the desired heading. Returns the new (unmasked) facing angle. */
static uint32_t ArmyGroundMovement_TurnTowardsHeading
          (ModelRuntimeGroundMovementSteeringView *modelRuntime,
          ModelDefinitionGroundMovementSteeringView *movementDefinition,uint32_t facingAngle,uint32_t desiredHeading)

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

/* Turns the model towards the waypoint and, if the heading error is small enough, drives towards it (or
   stops at a blocking army), places it with the placement callback and applies the recoil tilt; otherwise
   it stops and is placed stationary. Returns the root node used for the rest of the tick. A blocking army
   is told about the collision only when notifyBlockingArmy is set (ground and tracked movement); the
   water-surface movement just stops. */
static ModelRuntimeNode *ArmyGroundMovement_SteerAndDrive
          (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView *modelRuntime,
          ModelRuntimeNode *rootNode,Q12 waypointWorldXQ12,Q12 waypointWorldYQ12,bool notifyBlockingArmy)

{
  ArmyMovementStateFlags *ownerMovementFlags;
  ArmyRuntimeSlot *ownerArmy;
  ModelDefinitionGroundMovementSteeringView *movementDefinition;
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
    angleAndLength = THANDOR_COMPOUND(FixedLengthAngle){ .length = 0, .angle = (rootNode->modelPayload).worldRotationAngle2 };
  }
  else {
    angleAndLength = FixedMath_Vector2AngleAndLength(routeDeltaY,routeDeltaX);
  }
  desiredHeading = angleAndLength.angle;
  movementDefinition = modelRuntime->modelDefinition;
  facingAngle = ArmyGroundMovement_TurnTowardsHeading
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
  if (blockingModelRuntime != nullptr) {
    /* stay where we are and let the collision partner react (water surface: it is not notified) */
    blockedRootNode = modelRuntime->rootModelNode;
    if (notifyBlockingArmy) {
      ArmyRuntime_HandleCollisionPartner
                ((ModelRuntimeSlot *)modelRuntime,(blockedRootNode->worldTransform).translation.y,
                 (blockedRootNode->worldTransform).translation.x,blockingModelRuntime,
                 worldRuntime);
    }
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
    ArmyGroundMovement_ApplyRecoilTilt(modelRuntime,placedRootNode,recoilTilt);
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
  if ((linkedModelRuntime != nullptr) &&
      ((linkedModelRuntime->definitionOrSavedId.runtimeDefinition->footprintRadius == 0) ||
       (modelRuntime->modelDefinition->footprintRadius == 0) ||
       !ArmyCollision_TestPointWithinExpandedRuntimeRadius
                  (modelRuntime->modelDefinition->footprintRadius,(rootNode->worldTransform).translation.y,
                   (rootNode->worldTransform).translation.x,linkedModelRuntime))) {
    (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.modelRuntime = nullptr;
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
    placedRootNode = ArmyGroundMovement_SteerAndDrive
                       (worldRuntime,(ModelRuntimeGroundMovementSteeringView *)modelRuntime,rootNode,
                        waypointWorldXQ12,waypointWorldYQ12,true);
  }
  else {
    (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
    (modelRuntime->movementControl).turnVelocityAngle16 = 0;
    placedRootNode = ArmyGroundMovement_PlaceStationary
                       (worldRuntime,(ModelRuntimeGroundMovementSteeringView *)modelRuntime);
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
  if ((linkedModelRuntime != nullptr) &&
      ((linkedModelRuntime->definitionOrSavedId.runtimeDefinition->footprintRadius == 0) ||
       (modelRuntime->modelDefinition->footprintRadius == 0) ||
       !ArmyCollision_TestPointWithinExpandedRuntimeRadius
          (modelRuntime->modelDefinition->footprintRadius,(rootNode->worldTransform).translation.y,
           (rootNode->worldTransform).translation.x,linkedModelRuntime))) {
    (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.modelRuntime = nullptr;
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
                         (worldRuntime,modelRuntime,rootNode,waypointWorldXQ12,waypointWorldYQ12,true);
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
  if ((linkedModelRuntime != nullptr) &&
      ((linkedModelRuntime->definitionOrSavedId.runtimeDefinition->footprintRadius == 0) ||
       (modelRuntime->modelDefinition->footprintRadius == 0) ||
       !ArmyCollision_TestPointWithinExpandedRuntimeRadius
          (modelRuntime->modelDefinition->footprintRadius,(rootNode->worldTransform).translation.y,
           (rootNode->worldTransform).translation.x,linkedModelRuntime))) {
    (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.modelRuntime = nullptr;
  }
  previousRotationAngle = (rootNode->modelPayload).worldRotationAngle2;
  previousWorldX = (rootNode->worldTransform).translation.x;
  previousWorldY = (rootNode->worldTransform).translation.y;
  /* alive and still on its way to a route point */
  if ((((modelRuntime->classState).stateFlags & ARMY_RUNTIME_FLAG_DESTROYED) == 0) &&
      !ArmyRuntime_UpdateMovementAndWaypoints
         (worldRuntime,(ArmyMovementRuntime *)modelRuntime->ownerArmyRuntime,&waypointWorldXQ12,
          &waypointWorldYQ12)) {
    rootNode = ArmyGroundMovement_SteerAndDrive
                         (worldRuntime,modelRuntime,rootNode,waypointWorldXQ12,waypointWorldYQ12,false);
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
}
