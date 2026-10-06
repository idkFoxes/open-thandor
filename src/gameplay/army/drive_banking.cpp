/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/drive_banking.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/army/drive_banking.h>
#include <thandor/thandor.h>

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
  SoundVoiceSet **voiceSetRef;
  GraphicsFixedVec3 *worldPosition;
  uint32_t *classStateWord;

  linkedModelRuntime = (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.modelRuntime;
  rootNode = modelRuntime->rootModelNode;
  if (linkedModelRuntime == nullptr) {
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
  (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.modelRuntime = nullptr;
  if ((childCount < 3) || (((modelRuntime->classState).behaviorState & 4) == 0)) {
    return;
  }
  classStateWord = &(modelRuntime->classState).behaviorState;
  *classStateWord = *classStateWord | 1;
  /* the full definition behind its ground-steering view */
  soundIndex = reinterpret_cast<ModelDefinition *>(movementDefinition)->positionedSoundSlotIndex;
  if ((soundIndex == 0) || (soundIndex >= worldRuntime->dwordArrayCount) || (worldRuntime->dwordArray == nullptr)) {
    return;
  }
  voiceSetRef = ArmySound_VoiceSetRef(worldRuntime,soundIndex);
  if (voiceSetRef == nullptr) {
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
  ArmyGroundMovement_ApplyRecoilTilt(modelRuntime,rootNode,recoilTilt);
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

  /* a new route point (classState68/armyLinkOrState) restarts from standstill. Original quirk: `&&`, so a route
     point that differs in only one coordinate keeps the current speed and turn velocity. */
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
    *targetAngleLength = THANDOR_COMPOUND(FixedLengthAngle){ .length = 0, .angle = (rootNode->modelPayload).worldRotationAngle2 };
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
            (worldRuntime,ModelView_Cast<ModelRuntimeSlot>(modelRuntime));
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
              (worldRuntime,ModelView_Cast<ArmyMovementRuntime>(ownerArmy),&reachedWorldXQ12,&reachedWorldYQ12);
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
                      ModelView_Cast<RuntimeCollisionQueryView>(modelRuntime),worldRuntime);
  if (blockingModelRuntime != nullptr) {
    collisionRootNode = modelRuntime->rootModelNode;
    ArmyRuntime_HandleCollisionPartner
              (ModelView_Cast<ModelRuntimeSlot>(modelRuntime),(collisionRootNode->worldTransform).translation.y,
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
    ArmyGroundMovement_ApplyRecoilTilt(modelRuntime,rootNode,recoilTilt);
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
  Bool8 waypointArrived;
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
                       (worldRuntime,ModelView_Cast<ArmyMovementRuntime>(modelRuntime->ownerArmyRuntime),&waypointWorldXQ12,
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
              (worldRuntime,ModelView_Cast<ModelRuntimeUpdateView>(modelRuntime));
  }
  if (((previousWorldX != (rootNode->worldTransform).translation.x) ||
      (previousWorldY != (rootNode->worldTransform).translation.y)) ||
     (previousRotationAngle != (rootNode->modelPayload).worldRotationAngle2)) {
    ownerMovementFlags = &modelRuntime->ownerArmyRuntime->movementStateFlags;
    *ownerMovementFlags = *ownerMovementFlags & ~ARMY_MOVEMENT_STATIONARY;
  }
  movementDefinition = modelRuntime->modelDefinition;
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,ModelView_Cast<ModelRuntimeSlot>(modelRuntime));
  ModelNodeRuntime_RebuildTransformsFromRoot(rootNode);
  ModelNodeRuntime_UpdateDepthBinMasks(movementDefinition->footprintRadius,rootNode);
}
