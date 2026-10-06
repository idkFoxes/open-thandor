/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/aircraft.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/army/aircraft.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Poses the aircraft body (the root's first child) on its vertical arc: the arc position (classState7C) gives the
   height above heightBaseQ12 (arcCoefficient * ticks * position^2) and the pitch, then the arc position advances
   by one movement step for this batch of ticks. Returns that step. */
static int ArmyAircraft_PoseOnVerticalArc(ModelRuntimeClass21UpdateView *modelRuntime,
          ArmyRuntimeClassUpdate21DefinitionView *definition,int arcCoefficient,Q12 heightBaseQ12)
{
  ModelRuntimeNode *bodyNode;
  uint32_t arcPosition;
  int64_t product64;
  uint32_t pitchAngle;
  int stepQ12;

  arcPosition = (modelRuntime->classLinkState).classState7C;
  product64 = (int64_t)(int)arcPosition * (int64_t)(int)arcPosition;
  bodyNode = modelRuntime->rootModelNode->childNodes[0];
  product64 = (int64_t)(int)(arcCoefficient * g_InGameSimulationStepTicks) *
           (int64_t)(int)FIXED_PRODUCT_SHR(product64,Q12_SHIFT);
  (bodyNode->modelPayload).localRotationAngle0 = FIXED_ANGLE16_HALF_TURN;
  (bodyNode->modelPayload).localTranslationZQ12 = FIXED_PRODUCT_SHR(product64,Q12_SHIFT) + heightBaseQ12;
  product64 = (int64_t)(int)(arcPosition * g_InGameSimulationStepTicks) * (int64_t)arcCoefficient;
  pitchAngle = FixedMath_Atan2Angle16(FIXED_PRODUCT_SHR(product64,11),Q12_ONE);
  stepQ12 = definition->movementStepQ12 * g_InGameSimulationStepTicks;
  (modelRuntime->classLinkState).classState7C = (modelRuntime->classLinkState).classState7C + stepQ12;
  (bodyNode->modelPayload).localRotationAngle1 = FIXED_ANGLE16_QUARTER_TURN - pitchAngle & FIXED_ANGLE16_MASK;
  return stepQ12;
}

/* One tick batch of the take-off or landing arc over the home pad: the body follows the vertical arc above the
   parked height (classState80) and the aircraft moves one step along its heading. */
static void ArmyAircraft_FlyPadArc(ModelRuntimeClass21UpdateView *modelRuntime,
          ArmyRuntimeClassUpdate21DefinitionView *definition)
{
  int stepQ12;
  ModelRuntimeNode *rootNode;
  FixedSinCos sinCosStep;

  stepQ12 = ArmyAircraft_PoseOnVerticalArc
                      (modelRuntime,definition,definition->arcCoefficient,
                       (Q12)(modelRuntime->classLinkState).classState80);
  rootNode = modelRuntime->rootModelNode;
  sinCosStep = FixedMath_SinCosScaled((rootNode->modelPayload).worldRotationAngle2,stepQ12);
  (rootNode->worldTransform).translation.x = (rootNode->worldTransform).translation.x + sinCosStep.cosValue;
  (rootNode->worldTransform).translation.y = (rootNode->worldTransform).translation.y + sinCosStep.sinValue;
}

/* Parked on the pad: takes off once the hangar is ready (or there is no pad), is removed when the hangar is idle
   or closed, otherwise rides on the pad's lift (copies its height). */
static void ArmyAircraft_UpdateParked(WorldRuntimeContext *worldRuntime,ModelRuntimeClass21UpdateView *modelRuntime,
          ModelRuntimeSlot *homeModelRuntime)
{
  ArmyRuntimeClassUpdate21DefinitionView *definition;
  ModelRuntimeNode *rootNode;
  Q12 padDeckHeightQ12;

  definition = modelRuntime->modelDefinition;
  rootNode = modelRuntime->rootModelNode;
  if ((homeModelRuntime == nullptr) || ((homeModelRuntime->classState).classStateB0 == ARMY_PAD_HANGAR_READY)) {
    (modelRuntime->class21State).behaviorState = ARMY_AIRCRAFT_STATE_TAKING_OFF;
    (modelRuntime->classLinkState).classState7C = 0;
    (modelRuntime->classLinkState).classState64 = definition->phaseInitial;
    (modelRuntime->classLinkState).classState68 = definition->phaseDuration;
  }
  else if (((homeModelRuntime->classState).classStateB0 == ARMY_PAD_HANGAR_IDLE) ||
          ((homeModelRuntime->classState).classStateB0 == ARMY_PAD_HANGAR_CLOSED)) {
    EffectRuntimePool_CreateInstanceFromDefinition
              (EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY,
               THANDOR_COMPOUND(EffectRuntimeOwnerReference){ .modelRuntime = ModelView_Cast<ModelRuntimeSlot>(modelRuntime) },
               (rootNode->modelPayload).worldRotationAngle2,
               (rootNode->modelPayload).worldRotationAngle1,
               (rootNode->modelPayload).worldRotationAngle0,
               (rootNode->worldTransform).translation.z,
               (rootNode->worldTransform).translation.y,
               (rootNode->worldTransform).translation.x,definition->removalEffect,worldRuntime);
    (modelRuntime->class21State).stateFlags =
         (modelRuntime->class21State).stateFlags | ARMY_MODEL_STATE_DESTRUCTION_STARTED;
  }
  else {
    padDeckHeightQ12 = (((homeModelRuntime->rootModelNodeOrSavedOffset).modelNode)->childNodes[0]->modelPayload).
                       localTranslationZQ12;
    (rootNode->childNodes[0]->modelPayload).localTranslationZQ12 = padDeckHeightQ12;
    (modelRuntime->classLinkState).classState80 = padDeckHeightQ12;
  }
}

/* Touch-down on the home pad: the hangar starts lowering (with its sound). The pad's health, scaled to the
   aircraft's maximum health, is compared with the aircraft's health: when the aircraft has less, the pad takes
   half the difference as damage. */
static void ArmyAircraft_TouchDownOnPad(WorldRuntimeContext *worldRuntime,ModelRuntimeClass21UpdateView *modelRuntime,
          ModelRuntimeSlot *homeModelRuntime)
{
  ArmyRuntimeClassUpdate21DefinitionView *definition;
  ModelRuntimeSlot *homeDefinitionSlot;
  int scaledPadHealth;
  uint32_t healthDifference;

  definition = modelRuntime->modelDefinition;
  homeDefinitionSlot = Thandor_U32ToPointer<ModelRuntimeSlot>((homeModelRuntime->definitionOrSavedId).savedIdOrOffset); /* 32-bit format field: ModelRuntimeSlot.definitionOrSavedId */
  (homeModelRuntime->classState).classStateB0 = ARMY_PAD_HANGAR_LOWERING;
  ModelRuntime_PlayDefinitionOneShotSound
            (homeModelRuntime,homeModelRuntime->definitionOrSavedId.runtimeDefinition->secondarySoundIndex,
             worldRuntime);
  scaledPadHealth = (int)(((int64_t)(int)homeModelRuntime->health * (int64_t)definition->maximumHealth) /
                          (int64_t)(homeDefinitionSlot->classLinkState).modelLinkOrState.signedScalarState);
  healthDifference = scaledPadHealth - modelRuntime->health;
  if (healthDifference != 0 && (int)modelRuntime->health <= scaledPadHealth) {
    ArmyRuntime_ApplyDamageAndPropagateToParent(healthDifference >> 1,homeModelRuntime);
  }
}

/* Landing arc onto the pad; when the landing countdown (classState68) runs out the aircraft is parked again. */
static void ArmyAircraft_UpdateLanding(WorldRuntimeContext *worldRuntime,ModelRuntimeClass21UpdateView *modelRuntime,
          ModelRuntimeSlot *homeModelRuntime)
{
  ArmyAircraft_FlyPadArc(modelRuntime,modelRuntime->modelDefinition);
  (modelRuntime->classLinkState).classState68 = (modelRuntime->classLinkState).classState68 - 1;
  if ((int)(modelRuntime->classLinkState).classState68 < 0) {
    (modelRuntime->class21State).behaviorState = ARMY_AIRCRAFT_STATE_PARKED;
    if (homeModelRuntime != nullptr) {
      ArmyAircraft_TouchDownOnPad(worldRuntime,modelRuntime,homeModelRuntime);
    }
  }
}

/* Take-off arc from the pad: the hangar starts lowering when the countdown classState64 reaches 0; when the
   countdown classState68 runs out the aircraft leaves the map and starts its approach. */
static void ArmyAircraft_UpdateTakingOff(WorldRuntimeContext *worldRuntime,ModelRuntimeClass21UpdateView *modelRuntime,
          ModelRuntimeSlot *homeModelRuntime)
{
  ModelRuntimeNode *rootNode;

  ArmyAircraft_FlyPadArc(modelRuntime,modelRuntime->modelDefinition);
  rootNode = modelRuntime->rootModelNode;
  (modelRuntime->classLinkState).classState64 = (modelRuntime->classLinkState).classState64 - 1;
  if (((modelRuntime->classLinkState).classState64 == 0) && (homeModelRuntime != nullptr)) {
    (homeModelRuntime->classState).classStateB0 = ARMY_PAD_HANGAR_LOWERING;
    ModelRuntime_PlayDefinitionOneShotSound
            (homeModelRuntime,homeModelRuntime->definitionOrSavedId.runtimeDefinition->secondarySoundIndex,
             worldRuntime);
  }
  (modelRuntime->classLinkState).classState68 = (modelRuntime->classLinkState).classState68 - 1;
  if ((int)(modelRuntime->classLinkState).classState68 < 0) {
    (rootNode->worldTransform).translation.x = ARMY_AIRCRAFT_OFF_MAP_X_Q12;
    (rootNode->worldTransform).translation.y = ARMY_AIRCRAFT_OFF_MAP_Y_Q12;
    (modelRuntime->class21State).behaviorState = ARMY_AIRCRAFT_STATE_APPROACH;
  }
}

/* Approach: the aircraft is placed travelStepCount movement steps behind the target point (classState70/74) on
   the attack heading (classState78) and starts the attack run (drop countdown armyLinkOrState = travelStepCount,
   run length classState68 = twice that). The run height is the highest terrain sampled every 10 movement steps
   along the run, plus half a unit and twice the parked height (classState80). The original first tested the
   target point with a world point stub that always rejected it, so the approach always starts at once. */
static void ArmyAircraft_UpdateApproach(WorldRuntimeContext *worldRuntime,ModelRuntimeClass21UpdateView *modelRuntime)
{
  ArmyRuntimeClassUpdate21DefinitionView *definition;
  ModelRuntimeNode *rootNode;
  AngleTurn32 attackHeading;
  FixedSinCos sinCosStep;
  uint32_t travelSteps;
  uint32_t sampleCountdown;
  Q12 sampleXQ12;
  Q12 sampleYQ12;
  int maxTerrainHeightQ12;
  Q12 terrainHeightQ12;
  uint32_t parkedHeightQ12;

  definition = modelRuntime->modelDefinition;
  rootNode = modelRuntime->rootModelNode;
  attackHeading = (modelRuntime->classLinkState).classState78;
  sinCosStep = FixedMath_SinCosScaled
                     (attackHeading ^ FIXED_ANGLE16_HALF_TURN,definition->movementStepQ12 * definition->travelStepCount);
  (rootNode->worldTransform).translation.x = sinCosStep.cosValue + (modelRuntime->classLinkState).classState70;
  (rootNode->worldTransform).translation.y = sinCosStep.sinValue + (modelRuntime->classLinkState).classState74;
  (rootNode->modelPayload).worldRotationAngle2 = attackHeading;
  travelSteps = definition->travelStepCount;
  (modelRuntime->classLinkState).armyLinkOrState.classState = travelSteps;
  sampleCountdown = travelSteps * 2;
  (modelRuntime->classLinkState).classState68 = sampleCountdown;
  (modelRuntime->classLinkState).classState7C = (0 - travelSteps) * definition->movementStepQ12;
  (modelRuntime->class21State).behaviorState = ARMY_AIRCRAFT_STATE_ATTACK_RUN;
  sinCosStep = FixedMath_SinCosScaled(attackHeading,definition->movementStepQ12 * 10);
  sampleXQ12 = (rootNode->worldTransform).translation.x;
  sampleYQ12 = (rootNode->worldTransform).translation.y;
  maxTerrainHeightQ12 = 0;
  parkedHeightQ12 = (modelRuntime->classLinkState).classState80;
  do {
    FieldGrid_InterpolateTopSurfaceHeight(sampleYQ12,sampleXQ12,worldRuntime->fieldGrid,&terrainHeightQ12);
    if (maxTerrainHeightQ12 < terrainHeightQ12) {
      maxTerrainHeightQ12 = terrainHeightQ12;
    }
    sampleXQ12 = sampleXQ12 + sinCosStep.cosValue;
    sampleYQ12 = sampleYQ12 + sinCosStep.sinValue;
    sampleCountdown = sampleCountdown - 10;
  } while ((int)sampleCountdown >= 0);
  (modelRuntime->class21State).trajectoryTerrainReferenceHeightQ12 =
       maxTerrainHeightQ12 + Q12_ONE / 2 + parkedHeightQ12 * 2;
}

/* Releases one model-point effect of the attack run when the drop countdown (armyLinkOrState) sits on a mark: the marks
   are modelPointStep apart from three steps before the target point (model point 7) to three steps after it
   (model point 1). Nothing is dropped when modelPointStep is 0. */
static void ArmyAircraft_DropModelPointEffectAtMark(WorldRuntimeContext *worldRuntime,
          ModelRuntimeClass21UpdateView *modelRuntime,ArmyRuntimeClassUpdate21DefinitionView *definition)
{
  ModelRuntimeNode *bodyNode;
  Q12 dropZQ12;
  void *modelPointTable;
  uint32_t countdownMark;
  ModelAttachmentOrdinal modelPointOrdinal;

  bodyNode = modelRuntime->rootModelNode->childNodes[0];
  countdownMark = definition->modelPointStep * -3;
  dropZQ12 = (bodyNode->worldTransform).translation.z - 2 * Q12_ONE;
  if (definition->modelPointStep == 0) {
    return;
  }
  modelPointTable = Thandor_U32ToPointer<void>(static_cast<MdlSerializedNodeHeader *>(definition->rootNode.get())->childSerializedOffsets[0]); /* 32-bit format field: MdlSerializedNodeHeader.childSerializedOffsets */
  for (modelPointOrdinal = 7; modelPointOrdinal != 0; modelPointOrdinal = modelPointOrdinal - 1) {
    if (countdownMark == (modelRuntime->classLinkState).armyLinkOrState.classState) {
      ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
                (0,dropZQ12,(bodyNode->worldTransform).translation.y,(bodyNode->worldTransform).translation.x,
                 modelPointOrdinal,definition->modelPointEffectId,bodyNode,modelPointTable,worldRuntime);
      return;
    }
    countdownMark = countdownMark + definition->modelPointStep;
  }
}

/* Attack run: flies along the heading at the run height (trajectoryTerrainReferenceHeightQ12, measured from the
   terrain below), then per tick counts down the drop countdown (armyLinkOrState) and the run (classState68,
   leaving the map when it runs out), plays the
   mapped terrain sound and drops the model-point effects at their marks. */
static void ArmyAircraft_UpdateAttackRun(WorldRuntimeContext *worldRuntime,ModelRuntimeClass21UpdateView *modelRuntime)
{
  ArmyRuntimeClassUpdate21DefinitionView *definition;
  ModelRuntimeNode *rootNode;
  FixedSinCos sinCosStep;
  Q12 terrainHeightQ12;
  InGameSimulationStepBatchTicks remainingTicks;

  definition = modelRuntime->modelDefinition;
  rootNode = modelRuntime->rootModelNode;
  sinCosStep = FixedMath_SinCosScaled
                     ((rootNode->modelPayload).worldRotationAngle2,
                      g_InGameSimulationStepTicks * definition->movementStepQ12);
  (rootNode->worldTransform).translation.x = (rootNode->worldTransform).translation.x + sinCosStep.cosValue;
  (rootNode->worldTransform).translation.y = (rootNode->worldTransform).translation.y + sinCosStep.sinValue;
  FieldGrid_InterpolateTopSurfaceHeight
            ((rootNode->worldTransform).translation.y,
             (rootNode->worldTransform).translation.x,worldRuntime->fieldGrid,&terrainHeightQ12);
  ArmyAircraft_PoseOnVerticalArc
            (modelRuntime,definition,definition->verticalArcCoefficient,
             (modelRuntime->class21State).trajectoryTerrainReferenceHeightQ12 - terrainHeightQ12);
  remainingTicks = g_InGameSimulationStepTicks;
  do {
    (modelRuntime->classLinkState).armyLinkOrState.classState =
         (modelRuntime->classLinkState).armyLinkOrState.classState - 1; /* the drop countdown */
    (modelRuntime->classLinkState).classState68 = (modelRuntime->classLinkState).classState68 - 1;
    if ((int)(modelRuntime->classLinkState).classState68 < 0) {
      (rootNode->worldTransform).translation.x = ARMY_AIRCRAFT_OFF_MAP_X_Q12;
      (rootNode->worldTransform).translation.y = ARMY_AIRCRAFT_OFF_MAP_Y_Q12;
      (modelRuntime->class21State).behaviorState = ARMY_AIRCRAFT_STATE_RETURNING;
    }
    ArmyRuntime_TryPlayMappedTerrainSoundAtWorldPoint
              (modelRuntime->ownerArmyRuntime->factionIndex,
               (rootNode->worldTransform).translation.y,
               (rootNode->worldTransform).translation.x,
               definition->terrainSoundAssetIndex,worldRuntime);
    ArmyAircraft_DropModelPointEffectAtMark(worldRuntime,modelRuntime,definition);
    remainingTicks = remainingTicks - 1;
  } while (remainingTicks != 0);
}

/* Returning (off the map): once the home pad's hangar is idle, the hangar starts opening (with its sound) and the
   aircraft is placed phaseDuration movement steps behind the pad on the pad's heading to fly its landing arc. The
   original also tested the pad position with a world point stub that always rejected it. */
static void ArmyAircraft_TryStartLanding(WorldRuntimeContext *worldRuntime,ModelRuntimeClass21UpdateView *modelRuntime,
          ModelRuntimeSlot *homeModelRuntime)
{
  ArmyRuntimeClassUpdate21DefinitionView *definition;
  ModelRuntimeNode *padNode;
  ModelRuntimeNode *aircraftNode;
  AngleTurn32 padHeading;
  FixedSinCos sinCosStep;
  uint32_t landingSteps;

  definition = modelRuntime->modelDefinition;
  padNode = (homeModelRuntime->rootModelNodeOrSavedOffset).modelNode;
  if ((homeModelRuntime->classState).classStateB0 != ARMY_PAD_HANGAR_IDLE) {
    return;
  }
  (homeModelRuntime->classState).classStateB0 = ARMY_PAD_HANGAR_OPENING;
  ModelRuntime_PlayDefinitionOneShotSound
            (homeModelRuntime,homeModelRuntime->definitionOrSavedId.runtimeDefinition->primarySoundIndex,
             worldRuntime);
  padHeading = (padNode->modelPayload).worldRotationAngle2;
  sinCosStep = FixedMath_SinCosScaled
                     (padHeading ^ FIXED_ANGLE16_HALF_TURN,definition->movementStepQ12 * definition->phaseDuration);
  aircraftNode = modelRuntime->rootModelNode;
  (aircraftNode->worldTransform).translation.x = sinCosStep.cosValue + (padNode->worldTransform).translation.x;
  (aircraftNode->worldTransform).translation.y = sinCosStep.sinValue + (padNode->worldTransform).translation.y;
  (aircraftNode->modelPayload).worldRotationAngle2 = padHeading;
  landingSteps = definition->phaseDuration;
  (modelRuntime->classLinkState).classState68 = landingSteps;
  (modelRuntime->classLinkState).classState7C = (0 - landingSteps) * definition->movementStepQ12;
  (modelRuntime->class21State).behaviorState = ARMY_AIRCRAFT_STATE_LANDING;
}

/* Runtime update of the aircraft class (MODEL_RUNTIME_CLASS_21_AIRCRAFT), reached only through
   g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[21] (called by
   ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive). State machine in behaviorState around the
   aircraft's home pad (classLinkState.modelLinkOrState): 1 parked on the pad, 3 take-off arc and off the map,
   4/5 re-entry along the attack heading at a height above the highest terrain on the path, dropping its
   model-point effects on a countdown, 6 off the map again, 2 landing arc back onto the pad. Afterwards the model
   is re-seated on the terrain and its transforms, emitters and damage effect are refreshed.
*/

void ArmyRuntimeClass_UpdateAircraft
          (WorldRuntimeContext *worldRuntime,ModelRuntimeClass21UpdateView *modelRuntime)

{
  ModelRuntimeSlot *homeModelRuntime;
  ModelRuntimeNode *modelNode;
  Q12 childHeightZ;
  ModelDefinition *semanticDefinition;
  uint32_t behaviorState;
  AngleTurn32 savedAngle0;
  AngleTurn32 savedAngle1;

  homeModelRuntime = (modelRuntime->classLinkState).modelLinkOrState.modelRuntime;
  modelNode = modelRuntime->rootModelNode;
  switch((modelRuntime->class21State).behaviorState) {
  case ARMY_AIRCRAFT_STATE_PARKED:
    ArmyAircraft_UpdateParked(worldRuntime,modelRuntime,homeModelRuntime);
    break;
  case ARMY_AIRCRAFT_STATE_LANDING:
    ArmyAircraft_UpdateLanding(worldRuntime,modelRuntime,homeModelRuntime);
    break;
  case ARMY_AIRCRAFT_STATE_TAKING_OFF:
    ArmyAircraft_UpdateTakingOff(worldRuntime,modelRuntime,homeModelRuntime);
    break;
  case ARMY_AIRCRAFT_STATE_APPROACH: /* line up behind the target point (classState70/74) on heading classState78 */
    ArmyAircraft_UpdateApproach(worldRuntime,modelRuntime);
    break;
  case ARMY_AIRCRAFT_STATE_ATTACK_RUN: /* releases the model-point effects when the drop countdown hits a mark */
    ArmyAircraft_UpdateAttackRun(worldRuntime,modelRuntime);
    break;
  case ARMY_AIRCRAFT_STATE_RETURNING:
    if (homeModelRuntime != nullptr) {
      ArmyAircraft_TryStartLanding(worldRuntime,modelRuntime,homeModelRuntime);
      break;
    }
    ModelRuntimeHierarchy_MarkDestroyedRecursive(worldRuntime,modelRuntime->ownerArmyRuntime);
    /* no home pad left: falls through */
  case ARMY_AIRCRAFT_STATE_NO_PAD:
    if (homeModelRuntime == nullptr) {
      childHeightZ = Q12_ONE;
    }
    else {
      childHeightZ = (((homeModelRuntime->rootModelNodeOrSavedOffset).modelNode)->childNodes[0]->modelPayload).
               localTranslationZQ12;
    }
    (modelNode->childNodes[0]->modelPayload).localTranslationZQ12 = childHeightZ;
  }
  modelNode = modelRuntime->rootModelNode;
  savedAngle0 = (modelNode->modelPayload).worldRotationAngle0;
  savedAngle1 = (modelNode->modelPayload).worldRotationAngle1;
  /* the full definition behind the class-21 definition view */
  (*g_ArmyPlacementContactKindDispatchTable.callbacks
    [reinterpret_cast<ModelDefinition *>(modelRuntime->modelDefinition.get())->
     placementContactKindIndex])
            (reinterpret_cast<ModelDefinition *>(modelRuntime->modelDefinition.get())->
             placementHeightOffsetQ12,(modelNode->worldTransform).translation.y,
             (modelNode->worldTransform).translation.x,modelNode,worldRuntime);
  behaviorState = (modelRuntime->class21State).behaviorState;
  (modelNode->modelPayload).worldRotationAngle1 = savedAngle1;
  (modelNode->modelPayload).worldRotationAngle0 = savedAngle0;
  if ((behaviorState != ARMY_AIRCRAFT_STATE_NO_PAD) && (behaviorState != ARMY_AIRCRAFT_STATE_PARKED)) {
    ArmyRuntime_UpdateTimedShotAndEffectEmitters
              (worldRuntime,ModelView_Cast<ModelRuntimeUpdateView>(modelRuntime));
  }
  /* the full definition behind the class-21 definition view */
  semanticDefinition = reinterpret_cast<ModelDefinition *>(modelRuntime->modelDefinition.get());
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,ModelView_Cast<ModelRuntimeSlot>(modelRuntime));
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNode);
  ModelNodeRuntime_RecomputeSubtreeBoundingRadius(modelNode);
  ModelNodeRuntime_UpdateDepthBinMasks(semanticDefinition->footprintRadius,modelNode);
}

/* Takes the first queued pad asset (asset flag 8) the faction can pay for out of its secondary asset queue: the
   Xenite is paid, the build interval and the asset's Energy load (held while building) are stored in the pad, and
   the pad starts producing. Builds one asset at a time. */
static void ArmyPad_StartBuildingFirstAffordableAsset(ModelRuntimeLinkedChildSpawnAndBuildView *padRuntime,
          FactionRuntimeIndex factionIndex)
{
  FactionArmyAssetCount remainingAssetCount;
  uint32_t *queueEntry;
  ArmyAssetRecord *candidateAsset;
  uint32_t buildTicks;
  uint32_t selectedAssetValue;
  PckArmyAssetIdCatalog secondaryAssetId;

  remainingAssetCount = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount;
  queueEntry = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetPointersOrIds;
  for (; remainingAssetCount != 0; remainingAssetCount = remainingAssetCount - 1, queueEntry = queueEntry + 1) {
    /* queued asset record: build ticks (buildTicks), Xenite cost (xeniteCostQ4), Energy load
       (energyLoadQ4), the sums of its model definitions' build metrics */
    candidateAsset = Thandor_U32ToPointer<ArmyAssetRecord>(*queueEntry); /* 32-bit format field: GameFactionRuntimeRecord.secondaryArmyAssetPointersOrIds */
    if (((candidateAsset->flags & ARMY_ASSET_FLAG_BUILT_AT_AIRCRAFT_PAD) == 0) ||
       (g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 < candidateAsset->xeniteCostQ4)) {
      continue;
    }
    g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 =
         g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 - candidateAsset->xeniteCostQ4;
    buildTicks = candidateAsset->buildTicks;
    selectedAssetValue = candidateAsset->energyLoadQ4;
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_CHEAT_FAST_BUILD) != 0) {
      buildTicks = (buildTicks >> 4) + 1;
    }
    secondaryAssetId = candidateAsset->registryId;
    (padRuntime->linkedChildBuildState).secondaryArmyAssetBuildRequiredTicks = buildTicks;
    (padRuntime->linkedChildBuildState).selectedSecondaryArmyAssetValue = selectedAssetValue;
    (padRuntime->linkedChildBuildState).selectedSecondaryArmyAssetId = secondaryAssetId;
    padRuntime->energyLoadQ4 = padRuntime->energyLoadQ4 + selectedAssetValue;
    (padRuntime->linkedChildBuildState).secondaryArmyAssetBuildElapsedTicks = 0;
    g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount =
         g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount - 1;
    /* Remove the entry: shift the rest of the queue down by one. Original quirk: it shifts remainingAssetCount
       entries, i.e. it also copies the slot just behind the last queued entry (as in the factory queue). */
    do {
      *queueEntry = queueEntry[1];
      queueEntry = queueEntry + 1;
      remainingAssetCount = remainingAssetCount - 1;
    } while (remainingAssetCount != 0);
    padRuntime->secondaryArmyAssetBuildState = 1;
    padRuntime->linkedChildRuntimeFlags = padRuntime->linkedChildRuntimeFlags | ARMY_MODEL_STATE_PRODUCING;
    return;
  }
}

/* Plays one hangar sound of the aircraft pad (an index into the world's sound asset workspace; 0 = none) at the
   pad, unless the pad's terrain cell has mask bits 0/1 set. */
static void ArmyPadHangar_PlaySound(WorldRuntimeContext *worldRuntime,ModelDefinitionLinkedChildStateView *padDefinition,
          ModelRuntimeNode *padNode,SoundAssetIndex soundAssetIndex)
{
  SoundVoiceSet **soundVoiceSet;

  if ((soundAssetIndex == 0) || (worldRuntime->dwordArrayCount <= soundAssetIndex) ||
     (worldRuntime->dwordArray == nullptr)) {
    return;
  }
  /* the sound slot (a SpatialSoundSlot address kept as an integer, its voiceSet first) as the voice-set reference */
  soundVoiceSet = reinterpret_cast<SoundVoiceSet **>(worldRuntime->dwordArray[soundAssetIndex]);
  if (soundVoiceSet == nullptr) {
    return;
  }
  if (!TerrainGrid_TestProjectedCellMaskBits01
                ((padNode->worldTransform).translation.y,(padNode->worldTransform).translation.x,worldRuntime)) {
    SpatialSound_PlayPositionedOneShot
              (padDefinition->positionedSoundMaximumDistanceQ12,padDefinition->positionedSoundGainQ15,
               &(padNode->worldTransform).translation,soundVoiceSet);
  }
}

/* Consumes one pending launch of a linked aircraft slot and tries to create the aircraft. On success the hangar
   starts opening (with its transition sound) and true is returned. */
static Bool8 ArmyPadHangar_TryLaunchPendingAircraft(WorldRuntimeContext *worldRuntime,
          ModelRuntimeLinkedChildSpawnAndBuildView *padRuntime,uint8_t *pendingSpawnCount,
          ModelRuntimeLinkedChildSpawnInheritedState *inheritedState,PckArmyAssetIdCatalog linkedArmyAssetId,
          ModelDefinitionLinkedChildStateView *padDefinition,ModelRuntimeNode *padNode)
{
  Bool8 spawnFailed;

  *pendingSpawnCount = *pendingSpawnCount - 1;
  spawnFailed = ArmyRuntimeSpawner_CreateLinkedChildInstance
                          (inheritedState->inheritedValue78,inheritedState->inheritedValue74,
                           inheritedState->inheritedValue70,linkedArmyAssetId,worldRuntime,
                           /* the pad's model runtime read through the linked-child mask view */
                           reinterpret_cast<ArmyRuntimeLinkedChildMaskSlotView *>(padRuntime));
  if (spawnFailed) {
    return false;
  }
  padRuntime->linkedChildTransitionState = ARMY_PAD_HANGAR_OPENING;
  ArmyPadHangar_PlaySound(worldRuntime,padDefinition,padNode,padDefinition->linkedChildTransitionSoundAssetIndex);
  return true;
}

/* Runtime update of the aircraft home pad class (22), reached only through
   g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[22]. While the pad is being dismantled it
   passes the dismantling on to every aircraft (class 21) based on it. It builds one queued secondary army asset
   (asset flag 8) at a time: Xenite is paid up front, the asset's Energy load is held while building, and the
   finished id goes into a free slot of completedSecondaryArmyAssetIds (with a notification for the active
   faction). Then it drives the hangar transition in linkedChildTransitionState (1 open, 2 lift, 4 lower,
   5 close, 6/0 idle) and launches pending linked aircraft.
*/

void ArmyRuntimeClass_UpdateLinkedModelFlagsAndDispatchTerrainContactMode(WorldRuntimeContext *worldRuntime,
          ModelRuntimeLinkedChildSpawnAndBuildView *modelRuntime)

{
  ModelRuntimeSlot *ownerPayload;
  FactionRuntimeIndex factionIndex;
  Q12 translationLimitQ12;
  uint32_t completedAssetValue;
  PckArmyAssetIdCatalog secondaryAssetId;
  ModelRuntimeNode *modelNodeRuntime;
  ModelRuntimeNode *childNode;
  ModelDefinition *linkedModelDefinition;
  int reverseSlotIndex;
  uint32_t elapsedTicks;
  uint32_t energyLoadBefore;
  WorldOwnerListNode *ownerNode;
  ArmyAssetRecordPrefix *assetRecord;
  ModelDefinitionRecordPrefix *selectedDefinition;
  GraphicsFixedVec3 loweredChildPosition;
  Q12 translationStep;
  InGameNotificationMovieId notificationMovieId;
  ModelDefinitionLinkedChildStateView *linkedChildDefinition;
  ArmyRuntimeSlot *ownerArmyRuntime;

  if ((modelRuntime->linkedChildRuntimeFlags & ARMY_MODEL_STATE_DISMANTLING) != 0) {
    /* every aircraft (class 21) based on this pad (its classLinkState.modelLinkOrState) that is not dismantling
       already */
    ownerNode = worldRuntime->ownerListHead;
    do {
      if (ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
        ownerPayload = WorldOwnerNode_ModelRuntime(ownerNode);
        if ((ownerPayload->definitionOrSavedId.runtimeDefinition->runtimeClassId ==
             MODEL_RUNTIME_CLASS_21_AIRCRAFT) &&
           (modelRuntime == (ModelRuntimeLinkedChildSpawnAndBuildView *)
                            ownerPayload->classLinkState.modelLinkOrState.modelRuntime) &&
           ((ownerPayload->classState.stateFlags & ARMY_MODEL_STATE_DISMANTLING) == 0)) {
          ModelRuntimeHierarchy_MarkDestroyedRecursive
                    (worldRuntime,ownerPayload->ownerArmyRuntimeOrSavedOffset.armyRuntime);
        }
      }
      ownerNode = ownerNode->nextNode;
    } while (ownerNode != nullptr);
  }
  switch(modelRuntime->secondaryArmyAssetBuildState) {
  case 0:
    if ((modelRuntime->linkedChildRuntimeFlags & ARMY_MODEL_STATE_RESEARCHING) == 0) {
      if ((modelRuntime->linkedChildRuntimeFlags & ARMY_MODEL_STATE_BUILD_BLOCKING_MASK) == 0) {
        factionIndex = ((modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime)->factionIndex;
        if ((modelRuntime->linkedChildBuildState).completedSecondaryArmyAssetCount <
            modelRuntime->modelDefinition->linkedChildSlotCapacity) {
          ArmyPad_StartBuildingFirstAffordableAsset(modelRuntime,factionIndex);
        }
      }
    }
    else if ((modelRuntime->linkedChildRuntimeFlags & ARMY_MODEL_STATE_INACTIVE_MASK) == 0) {
      ArmyRuntime_UpdateTimedShotAndEffectEmitters
                (worldRuntime,ModelView_Cast<ModelRuntimeUpdateView>(modelRuntime));
      ArmyRuntime_UpdateAnimatedModelSubnodes
                (worldRuntime,ModelView_Cast<ModelRuntimeUpdateView>(modelRuntime));
    }
    break;
  case 1:
    if ((modelRuntime->linkedChildRuntimeFlags & ARMY_MODEL_STATE_INACTIVE_MASK) == 0) {
      (modelRuntime->linkedChildBuildState).secondaryArmyAssetBuildElapsedTicks =
           (modelRuntime->linkedChildBuildState).secondaryArmyAssetBuildElapsedTicks + g_InGameSimulationStepTicks;
      ownerArmyRuntime = (modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime;
      ArmyRuntime_UpdateTimedShotAndEffectEmitters
                (worldRuntime,ModelView_Cast<ModelRuntimeUpdateView>(modelRuntime));
      elapsedTicks = (modelRuntime->linkedChildBuildState).secondaryArmyAssetBuildElapsedTicks;
      ArmyRuntime_UpdateAnimatedModelSubnodes
                (worldRuntime,ModelView_Cast<ModelRuntimeUpdateView>(modelRuntime));
      if ((modelRuntime->linkedChildBuildState).secondaryArmyAssetBuildRequiredTicks <= elapsedTicks)
      {
        factionIndex = ownerArmyRuntime->factionIndex;
        completedAssetValue = (modelRuntime->linkedChildBuildState).selectedSecondaryArmyAssetValue;
        (modelRuntime->linkedChildBuildState).selectedSecondaryArmyAssetValue = 0;
        modelRuntime->secondaryArmyAssetBuildState = 0;
        modelRuntime->linkedChildRuntimeFlags =
             modelRuntime->linkedChildRuntimeFlags & ~ARMY_MODEL_STATE_PRODUCING;
        energyLoadBefore = modelRuntime->energyLoadQ4;
        modelRuntime->energyLoadQ4 = energyLoadBefore - completedAssetValue;
        secondaryAssetId = (modelRuntime->linkedChildBuildState).selectedSecondaryArmyAssetId;
        (modelRuntime->linkedChildBuildState).selectedSecondaryArmyAssetId = 0;
        if (completedAssetValue <= energyLoadBefore) {
          /* the last free slot of completedSecondaryArmyAssetIds (scanning down from the capacity) */
          for (reverseSlotIndex = modelRuntime->modelDefinition->linkedChildSlotCapacity - 1; -1 < reverseSlotIndex;
               reverseSlotIndex = reverseSlotIndex - 1) {
            if (modelRuntime->completedSecondaryArmyAssetIds[reverseSlotIndex] == 0) {
              break;
            }
          }
          if (reverseSlotIndex < 0) {
            reverseSlotIndex = 0; /* no free slot: overwrite the first */
          }
          modelRuntime->completedSecondaryArmyAssetIds[reverseSlotIndex] = secondaryAssetId;
          g_GameFactionRuntimeImage.records[factionIndex].relationCounterA =
               g_GameFactionRuntimeImage.records[factionIndex].relationCounterA + 1;
          (modelRuntime->linkedChildBuildState).completedSecondaryArmyAssetCount =
               (modelRuntime->linkedChildBuildState).completedSecondaryArmyAssetCount + 1;
          (modelRuntime->linkedChildBuildState).classState70 =
               (modelRuntime->linkedChildBuildState).classState70 + 1;
          if (factionIndex == worldRuntime->activeFactionRuntimeIndex) {
            /* Original quirk: the lookup status is not checked (an unknown id leaves the error code in
               assetRecord) */
            ArmyAssetRegistry_FindById(secondaryAssetId,&assetRecord);
            selectedDefinition = ModelDefinition_SelectFactionUnlockedLinkedDefinition
                               (ownerArmyRuntime->factionIndex,assetRecord->rootNodeOffsetOrPointer); /* 32-bit format field: ArmyAssetRecord.rootNodeOffsetOrPointer */
            linkedModelDefinition = ModelView_Cast<ModelDefinition>(selectedDefinition);
            linkedModelDefinition->builtCount = linkedModelDefinition->builtCount + 1;
            notificationMovieId = linkedModelDefinition->firstBuiltNotificationMovieId;
            if (linkedModelDefinition->builtCount != 1) {
              notificationMovieId = linkedModelDefinition->nextBuiltNotificationMovieId;
            }
            InGameNotificationQueue_InsertPriorityRecord
                      (NOTIFICATION_PAYLOAD_NONE,0,(worldRuntime->motion).pitchAngle,(worldRuntime->motion).headingAngle,
                       (((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->worldTransform).
                       translation.y,
                       (((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->worldTransform).
                       translation.x,3,notificationMovieId);
          }
        }
      }
    }
  }
  linkedChildDefinition = modelRuntime->modelDefinition;
  modelNodeRuntime = (modelRuntime->rootModelNodeOrSavedOffset).modelNode;
  switch(modelRuntime->linkedChildTransitionState) {
  case ARMY_PAD_HANGAR_OPENING: /* scroll the hatch texture */
    modelNodeRuntime->primaryTextureOffsetV =
         modelNodeRuntime->primaryTextureOffsetV +
         linkedChildDefinition->linkedChildTextureVStepPerTick * g_InGameSimulationStepTicks;
    if (ARMY_DOOR_TEXTURE_OPEN_V - 1 < modelNodeRuntime->primaryTextureOffsetV) {
      modelNodeRuntime->primaryTextureOffsetV = ARMY_DOOR_TEXTURE_OPEN_V;
      modelRuntime->linkedChildTransitionState = ARMY_PAD_HANGAR_LIFTING;
      ArmyPadHangar_PlaySound(worldRuntime,linkedChildDefinition,modelNodeRuntime,
                              linkedChildDefinition->linkedChildTransitionEndSoundAssetIndex);
    }
    break;
  case ARMY_PAD_HANGAR_LIFTING: /* the platform is child 0 */
    childNode = modelNodeRuntime->childNodes[0];
    translationLimitQ12 = linkedChildDefinition->linkedChildTranslationLimitQ12;
    (childNode->modelPayload).localTranslationZQ12 =
         (childNode->modelPayload).localTranslationZQ12 +
         linkedChildDefinition->linkedChildTranslationStepQ12PerTick * g_InGameSimulationStepTicks;
    if (translationLimitQ12 < (childNode->modelPayload).localTranslationZQ12) {
      modelRuntime->linkedChildTransitionState = ARMY_PAD_HANGAR_READY;
      (childNode->modelPayload).localTranslationZQ12 = translationLimitQ12;
    }
    ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
    break;
  case ARMY_PAD_HANGAR_LOWERING:
    translationStep = linkedChildDefinition->linkedChildTranslationStepQ12PerTick;
    /* (0, 0, 0) when the model has no such point; the result is not checked */
    ModelLookupTable_GetPackedPointPosition
              (0,1,(modelNodeRuntime->modelPayload).modelResource,&loweredChildPosition);
    childNode = modelNodeRuntime->childNodes[0];
    (childNode->modelPayload).localTranslationZQ12 =
         (childNode->modelPayload).localTranslationZQ12 - translationStep * g_InGameSimulationStepTicks;
    if ((childNode->modelPayload).localTranslationZQ12 < (int)loweredChildPosition.z) {
      modelRuntime->linkedChildTransitionState = ARMY_PAD_HANGAR_CLOSING;
      (childNode->modelPayload).localTranslationZQ12 = loweredChildPosition.z;
    }
    ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
    if (modelRuntime->linkedChildTransitionState == ARMY_PAD_HANGAR_CLOSING) {
      ArmyPadHangar_PlaySound(worldRuntime,linkedChildDefinition,modelNodeRuntime,
                              linkedChildDefinition->linkedChildTransitionSoundAssetIndex);
    }
    break;
  case ARMY_PAD_HANGAR_CLOSING:
    modelNodeRuntime->primaryTextureOffsetV =
         modelNodeRuntime->primaryTextureOffsetV -
         linkedChildDefinition->linkedChildTextureVStepPerTick * g_InGameSimulationStepTicks;
    if (modelNodeRuntime->primaryTextureOffsetV < 1) {
      modelNodeRuntime->primaryTextureOffsetV = 0;
      modelRuntime->linkedChildTransitionState = ARMY_PAD_HANGAR_CLOSED;
    }
    break;
  case ARMY_PAD_HANGAR_CLOSED:
    modelRuntime->linkedChildTransitionState = ARMY_PAD_HANGAR_IDLE;
    /* fall through: the idle hangar launches the next pending aircraft */
  case ARMY_PAD_HANGAR_IDLE:
    /* the original also required a world point stub (always false) to reject the pad position */
    if ((modelRuntime->linkedChildRuntimeFlags & ARMY_MODEL_STATE_INACTIVE_MASK) == 0) {
      /* the first pending slot whose aircraft can be created opens the hangar */
      if (((modelRuntime->linkedChildPendingSpawnCounts).slot0 != 0) &&
         ArmyPadHangar_TryLaunchPendingAircraft
                   (worldRuntime,modelRuntime,&(modelRuntime->linkedChildPendingSpawnCounts).slot0,
                    &modelRuntime->linkedChildSpawnInheritedState[0],g_InGamePointerModePreviewArmyIds[1],
                    linkedChildDefinition,modelNodeRuntime)) {
        break;
      }
      if (((modelRuntime->linkedChildPendingSpawnCounts).slot1 != 0) &&
         ArmyPadHangar_TryLaunchPendingAircraft
                   (worldRuntime,modelRuntime,&(modelRuntime->linkedChildPendingSpawnCounts).slot1,
                    &modelRuntime->linkedChildSpawnInheritedState[1],g_InGamePointerModePreviewArmyIds[2],
                    linkedChildDefinition,modelNodeRuntime)) {
        break;
      }
      if ((modelRuntime->linkedChildPendingSpawnCounts).slot2 != 0) {
        ArmyPadHangar_TryLaunchPendingAircraft
                  (worldRuntime,modelRuntime,&(modelRuntime->linkedChildPendingSpawnCounts).slot2,
                   &modelRuntime->linkedChildSpawnInheritedState[2],g_InGamePointerModePreviewArmyIds[4],
                   linkedChildDefinition,modelNodeRuntime);
      }
    }
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,ModelView_Cast<ModelRuntimeSlot>(modelRuntime));
}
