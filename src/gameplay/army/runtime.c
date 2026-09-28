/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/army/runtime.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Implementation ownership: gameplay/army/runtime. */

/* Address: 0x00525A60.
   Runtime update of the aircraft class (MODEL_RUNTIME_CLASS_21_AIRCRAFT), reached only through
   g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[21] (called by
   ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive). State machine in classStateB8 around the
   aircraft's home pad (the army at +0x60): 1 parked on the pad, 3 take-off arc and off the map, 4/5 re-entry
   along the attack heading at a height above the highest terrain on the path, dropping its model-point effects
   on a countdown, 6 off the map again, 2 landing arc back onto the pad. Afterwards the model is re-seated on
   the terrain and its transforms, emitters and damage effect are refreshed.
*/

void ArmyRuntimeClassUpdateSlot21_DispatchByClassId
          (WorldRuntimeContext *worldRuntime,ModelRuntimeClass21UpdateView200 *modelRuntime)

{
  uint32_t *stateField;
  GraphicsFixedVec3 *translationVec;
  GraphicsWorldCoordinateQ12 *translationY;
  ModelRuntimeArmyLinkOrState4 *armyLinkState;
  ArmyRuntimeClassUpdate21DefinitionView27C *currentDefinition;
  ModelRuntimeSlot *homeModelRuntime;
  uint32_t durationValue;
  void *modelPointSource;
  ModelRuntimeNode *childNode;
  ModelRuntimeSlot *homeDefinitionSlot;
  ModelDefinitionRuntimeSemanticView280 *semanticDefinition;
  int64_t product64;
  uint32_t stateValue;
  int stepValue;
  ArmyRuntimeSlot *countdownMark;
  uint32_t headingOrDelta;
  int maxTerrainHeight;
  int sampleCoord;
  Q12 childHeightZ;
  ArmyRuntimeClassUpdate21DefinitionView27C *classUpdate21Definition;
  ModelRuntimeNode *modelNode;
  bool pointAllowed;
  FixedSinCosEdxEax8 sinCosStep;
  HeightSampleResult terrainHeight;
  AngleTurn32 savedAngle1;
  InGameSimulationStepBatchTicks remainingTicks;
  AngleTurn32 savedAngle0;
  ArmyRuntimeClassUpdate21DefinitionView27C *savedClassUpdate21Definition;
  ModelRuntimeClass21UpdateView200 *savedModelRuntime;
  ModelRuntimeNode *savedModelNode;
  
  currentDefinition = modelRuntime->modelDefinition;
  homeModelRuntime = (modelRuntime->classLinkState).modelLinkOrState60.modelRuntime;
  modelNode = modelRuntime->rootModelNode;
  switch((modelRuntime->class21State84).classStateB8) {
  case 1: /* parked on the home pad */
    if ((homeModelRuntime == NULL) || ((homeModelRuntime->classState).classStateB0 == 3)) {
      stateValue = currentDefinition->phaseInitialC0;
      durationValue = currentDefinition->phaseDurationC4;
      (modelRuntime->class21State84).classStateB8 = 3;
      (modelRuntime->classLinkState).classState7C = 0;
      (modelRuntime->classLinkState).classState64 = stateValue;
      (modelRuntime->classLinkState).classState68 = durationValue;
    }
    else if (((homeModelRuntime->classState).classStateB0 == 0) ||
            ((homeModelRuntime->classState).classStateB0 == 6)) {
      EffectRuntimePool_CreateInstanceFromDefinition
                (EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY,
                 THANDOR_BITCAST(ModelRuntimeClass21UpdateView200 *, EffectRuntimeOwnerReference4, modelRuntime),
                 (modelNode->modelPayload).worldRotationAngle2,
                 (modelNode->modelPayload).worldRotationAngle1,
                 (modelNode->modelPayload).worldRotationAngle0,
                 (modelNode->worldTransform).translation.z,
                 (modelNode->worldTransform).translation.y,
                 (modelNode->worldTransform).translation.x,currentDefinition->completionEffect190,worldRuntime
                );
      stateField = &(modelRuntime->class21State84).classStateEC;
      *stateField = *stateField | ARMY_MODEL_STATE_DESTRUCTION_STARTED;
    }
    else {
      stateValue = (((homeModelRuntime->rootModelNodeOrSavedOffset).modelNode)->childNodes[0]->modelPayload).
               localTranslationZQ12;
      (modelNode->childNodes[0]->modelPayload).localTranslationZQ12 = stateValue;
      (modelRuntime->classLinkState).classState80 = stateValue;
    }
    break;
  case 2: /* landing arc onto the home pad */
    stateValue = (modelRuntime->classLinkState).classState7C;
    product64 = (int64_t)(int)stateValue * (int64_t)(int)stateValue;
    modelNode = modelNode->childNodes[0];
    product64 = (int64_t)(int)(currentDefinition->arcCoefficient14 * g_InGameSimulationStepTicks) *
             (int64_t)(int)((int)((uint64_t)product64 >> 0x20) << 0x14 | (uint32_t)product64 >> 0xc);
    (modelNode->modelPayload).localRotationAngle0 = FIXED_ANGLE16_HALF_TURN;
    stateValue = (modelRuntime->classLinkState).classState7C;
    (modelNode->modelPayload).localTranslationZQ12 =
         ((int)((uint64_t)product64 >> 0x20) << 0x14 | (uint32_t)product64 >> 0xc) +
         (modelRuntime->classLinkState).classState80;
    product64 = (int64_t)(int)(stateValue * g_InGameSimulationStepTicks) *
             (int64_t)currentDefinition->arcCoefficient14;
    stateValue = FixedMath_Atan2Angle16
                       ((int)((uint64_t)product64 >> 0x20) << 0x15 | (uint32_t)product64 >> 0xb,Q12_ONE);
    stepValue = currentDefinition->movementStepQ12_0C * g_InGameSimulationStepTicks;
    stateField = &(modelRuntime->classLinkState).classState7C;
    *stateField = *stateField + stepValue;
    (modelNode->modelPayload).localRotationAngle1 = FIXED_ANGLE16_QUARTER_TURN - stateValue & 0xffff;
    modelNode = modelRuntime->rootModelNode;
    sinCosStep = FixedMath_SinCosScaled((modelNode->modelPayload).worldRotationAngle2,stepValue);
    translationVec = &(modelNode->worldTransform).translation;
    translationVec->x = translationVec->x + (int)sinCosStep;
    translationY = &(modelNode->worldTransform).translation.y;
    *translationY = *translationY + (int)(sinCosStep >> 0x20);
    stateField = &(modelRuntime->classLinkState).classState68;
    *stateField = *stateField - 1;
    if (((int)*stateField < 0) &&
       ((modelRuntime->class21State84).classStateB8 = 1, homeModelRuntime != NULL)) {
      currentDefinition = modelRuntime->modelDefinition;
      homeDefinitionSlot = (ModelRuntimeSlot *)(homeModelRuntime->definitionOrSavedId).savedIdOrOffset;
      (homeModelRuntime->classState).classStateB0 = 4;
      ArmyRuntimeSpawner_PlayCreationSound((ArmyRuntimeSlot *)homeModelRuntime,worldRuntime);
      stepValue = (int)(((int64_t)(int)homeModelRuntime->definitionValue60_3C *
                     (int64_t)currentDefinition->healthOrScaleQ12_60) /
                    (int64_t)(homeDefinitionSlot->classLinkState).modelLinkOrState60.signedScalarState);
      headingOrDelta = stepValue - modelRuntime->definitionValue60_3C;
      if (headingOrDelta != 0 && (int)modelRuntime->definitionValue60_3C <= stepValue) {
        ArmyRuntime_ApplyDamageAndPropagateToParent(headingOrDelta >> 1,(ArmyRuntimeSlot *)homeModelRuntime);
      }
    }
    break;
  case 3: /* take-off arc, then off the map */
    stateValue = (modelRuntime->classLinkState).classState7C;
    product64 = (int64_t)(int)stateValue * (int64_t)(int)stateValue;
    modelNode = modelNode->childNodes[0];
    product64 = (int64_t)(int)(currentDefinition->arcCoefficient14 * g_InGameSimulationStepTicks) *
             (int64_t)(int)((int)((uint64_t)product64 >> 0x20) << 0x14 | (uint32_t)product64 >> 0xc);
    (modelNode->modelPayload).localRotationAngle0 = FIXED_ANGLE16_HALF_TURN;
    stateValue = (modelRuntime->classLinkState).classState7C;
    (modelNode->modelPayload).localTranslationZQ12 =
         ((int)((uint64_t)product64 >> 0x20) << 0x14 | (uint32_t)product64 >> 0xc) +
         (modelRuntime->classLinkState).classState80;
    product64 = (int64_t)(int)(stateValue * g_InGameSimulationStepTicks) *
             (int64_t)currentDefinition->arcCoefficient14;
    stateValue = FixedMath_Atan2Angle16
                       ((int)((uint64_t)product64 >> 0x20) << 0x15 | (uint32_t)product64 >> 0xb,Q12_ONE);
    stepValue = currentDefinition->movementStepQ12_0C * g_InGameSimulationStepTicks;
    stateField = &(modelRuntime->classLinkState).classState7C;
    *stateField = *stateField + stepValue;
    (modelNode->modelPayload).localRotationAngle1 = FIXED_ANGLE16_QUARTER_TURN - stateValue & 0xffff;
    modelNode = modelRuntime->rootModelNode;
    sinCosStep = FixedMath_SinCosScaled((modelNode->modelPayload).worldRotationAngle2,stepValue);
    translationVec = &(modelNode->worldTransform).translation;
    translationVec->x = translationVec->x + (int)sinCosStep;
    translationY = &(modelNode->worldTransform).translation.y;
    *translationY = *translationY + (int)(sinCosStep >> 0x20);
    stateField = &(modelRuntime->classLinkState).classState64;
    *stateField = *stateField - 1;
    if ((*stateField == 0) && (homeModelRuntime != NULL)) {
      (homeModelRuntime->classState).classStateB0 = 4;
      ArmyRuntimeSpawner_PlayCreationSound((ArmyRuntimeSlot *)homeModelRuntime,worldRuntime);
    }
    stateField = &(modelRuntime->classLinkState).classState68;
    *stateField = *stateField - 1;
    if ((int)*stateField < 0) {
      (modelNode->worldTransform).translation.x = ARMY_AIRCRAFT_OFF_MAP_X_Q12;
      (modelNode->worldTransform).translation.y = ARMY_AIRCRAFT_OFF_MAP_Y_Q12;
      (modelRuntime->class21State84).classStateB8 = 4;
    }
    break;
  case 4: /* off the map: line up behind the target point (+0x70/+0x74) on heading +0x78 */
    pointAllowed = ArmyRuntime_TestWorldPointAllowedDefault
                       (currentDefinition->worldPointAllowedContext48,
                        (modelRuntime->classLinkState).classState74,
                        (modelRuntime->classLinkState).classState70);
    if (!pointAllowed) {
      headingOrDelta = (modelRuntime->classLinkState).classState78;
      sinCosStep = FixedMath_SinCosScaled
                         (headingOrDelta ^ FIXED_ANGLE16_HALF_TURN,currentDefinition->movementStepQ12_0C * currentDefinition->travelStepCountC8);
      stateValue = (modelRuntime->classLinkState).classState74;
      (modelNode->worldTransform).translation.x =
           (int)sinCosStep + (modelRuntime->classLinkState).classState70;
      (modelNode->worldTransform).translation.y = (int)(sinCosStep >> 0x20) + stateValue;
      (modelNode->modelPayload).worldRotationAngle2 = headingOrDelta;
      durationValue = currentDefinition->travelStepCountC8;
      (modelRuntime->classLinkState).armyLinkOrState6C.classState = durationValue;
      stateValue = durationValue * 2;
      stepValue = currentDefinition->movementStepQ12_0C;
      (modelRuntime->classLinkState).classState68 = stateValue;
      (modelRuntime->classLinkState).classState7C = -durationValue * stepValue;
      (modelRuntime->class21State84).classStateB8 = 5;
      sinCosStep = FixedMath_SinCosScaled(headingOrDelta,currentDefinition->movementStepQ12_0C * 10);
      stepValue = (modelNode->worldTransform).translation.x;
      sampleCoord = (modelNode->worldTransform).translation.y;
      maxTerrainHeight = 0;
      durationValue = (modelRuntime->classLinkState).classState80;
      do {
        terrainHeight = FieldGrid_InterpolateTopSurfaceHeight(sampleCoord,stepValue,worldRuntime->fieldGrid);
        if (maxTerrainHeight < terrainHeight.heightQ12) {
          maxTerrainHeight = terrainHeight.heightQ12;
        }
        stepValue = stepValue + (int)sinCosStep;
        sampleCoord = sampleCoord + (int)(sinCosStep >> 0x20);
        stateValue = stateValue - 10;
      } while (-1 < (int)stateValue);
      (modelRuntime->class21State84).trajectoryTerrainReferenceHeightQ12_84 =
           maxTerrainHeight + 0x800 + durationValue * 2;
    }
    break;
  case 5: /* attack run, releasing the model-point effects when the countdown at +0x6C hits a mark */
    sinCosStep = FixedMath_SinCosScaled
                       ((modelNode->modelPayload).worldRotationAngle2,
                        g_InGameSimulationStepTicks * currentDefinition->movementStepQ12_0C);
    translationVec = &(modelNode->worldTransform).translation;
    translationVec->x = translationVec->x + (int)sinCosStep;
    translationY = &(modelNode->worldTransform).translation.y;
    *translationY = *translationY + (int)(sinCosStep >> 0x20);
    terrainHeight = FieldGrid_InterpolateTopSurfaceHeight
                       ((modelNode->worldTransform).translation.y,
                        (modelNode->worldTransform).translation.x,worldRuntime->fieldGrid);
    classUpdate21Definition = modelRuntime->modelDefinition;
    stateValue = (modelRuntime->classLinkState).classState7C;
    product64 = (int64_t)(int)stateValue * (int64_t)(int)stateValue;
    modelNode = modelNode->childNodes[0];
    product64 = (int64_t)
             (int)(classUpdate21Definition->verticalArcCoefficientCC * g_InGameSimulationStepTicks)
             * (int64_t)(int)((int)((uint64_t)product64 >> 0x20) << 0x14 | (uint32_t)product64 >> 0xc);
    (modelNode->modelPayload).localRotationAngle0 = FIXED_ANGLE16_HALF_TURN;
    stateValue = (modelRuntime->classLinkState).classState7C;
    (modelNode->modelPayload).localTranslationZQ12 =
         (((int)((uint64_t)product64 >> 0x20) << 0x14 | (uint32_t)product64 >> 0xc) +
         (modelRuntime->class21State84).trajectoryTerrainReferenceHeightQ12_84) - terrainHeight.heightQ12;
    product64 = (int64_t)(int)(stateValue * g_InGameSimulationStepTicks) *
             (int64_t)classUpdate21Definition->verticalArcCoefficientCC;
    stateValue = FixedMath_Atan2Angle16
                       ((int)((uint64_t)product64 >> 0x20) << 0x15 | (uint32_t)product64 >> 0xb,Q12_ONE);
    stateField = &(modelRuntime->classLinkState).classState7C;
    *stateField = *stateField + classUpdate21Definition->movementStepQ12_0C * g_InGameSimulationStepTicks;
    (modelNode->modelPayload).localRotationAngle1 = FIXED_ANGLE16_QUARTER_TURN - stateValue & 0xffff;
    modelNode = modelRuntime->rootModelNode;
    remainingTicks = g_InGameSimulationStepTicks;
    do {
      armyLinkState = &(modelRuntime->classLinkState).armyLinkOrState6C;
      armyLinkState->armyRuntime = (ArmyRuntimeSlot *)(armyLinkState->classState - 1);
      stateField = &(modelRuntime->classLinkState).classState68;
      *stateField = *stateField - 1;
      if ((int)*stateField < 0) {
        (modelNode->worldTransform).translation.x = ARMY_AIRCRAFT_OFF_MAP_X_Q12;
        (modelNode->worldTransform).translation.y = ARMY_AIRCRAFT_OFF_MAP_Y_Q12;
        ((ModelRuntimeSlotClassState7C *)&modelRuntime->class21State84)->classStateB8 = 6;
      }
      modelPointSource = classUpdate21Definition->modelPointSource64;
      savedClassUpdate21Definition = classUpdate21Definition;
      savedModelNode = modelNode;
      savedModelRuntime = modelRuntime;
      ArmyRuntime_TryPlayMappedTerrainSoundAtWorldPoint
                (modelRuntime->ownerArmyRuntime->factionIndex,
                 (modelNode->worldTransform).translation.y,
                 (modelNode->worldTransform).translation.x,
                 classUpdate21Definition->terrainSoundAssetIndex26C,worldRuntime);
      childNode = modelNode->childNodes[0];
      stepValue = classUpdate21Definition->modelPointStep30 * -3;
      sampleCoord = (childNode->worldTransform).translation.z - 0x2000;
      modelNode = savedModelNode;
      if (classUpdate21Definition->modelPointStep30 != 0) {
        modelPointSource = *(void **)((int)modelPointSource + 0x18);
        if (stepValue - (modelRuntime->classLinkState).armyLinkOrState6C.classState == 0) {
          ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
                    (0,sampleCoord,(childNode->worldTransform).translation.y,
                     (childNode->worldTransform).translation.x,7,
                     classUpdate21Definition->modelPointEffectId2C,childNode,modelPointSource,worldRuntime);
          modelNode = savedModelNode;
        }
        else {
          countdownMark = (ArmyRuntimeSlot *)(stepValue + classUpdate21Definition->modelPointStep30);
          if (countdownMark == (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime) {
            ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
                      (0,sampleCoord,(childNode->worldTransform).translation.y,
                       (childNode->worldTransform).translation.x,6,
                       classUpdate21Definition->modelPointEffectId2C,childNode,modelPointSource,worldRuntime);
            modelNode = savedModelNode;
          }
          else {
            countdownMark = (ArmyRuntimeSlot *)
                        ((int)&countdownMark->modelRuntimeOrSavedOffset +
                        classUpdate21Definition->modelPointStep30);
            if (countdownMark == (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime) {
              ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
                        (0,sampleCoord,(childNode->worldTransform).translation.y,
                         (childNode->worldTransform).translation.x,5,
                         classUpdate21Definition->modelPointEffectId2C,childNode,modelPointSource,worldRuntime);
              modelNode = savedModelNode;
            }
            else {
              countdownMark = (ArmyRuntimeSlot *)
                          ((int)&countdownMark->modelRuntimeOrSavedOffset +
                          classUpdate21Definition->modelPointStep30);
              if (countdownMark == (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime) {
                ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
                          (0,sampleCoord,(childNode->worldTransform).translation.y,
                           (childNode->worldTransform).translation.x,4,
                           classUpdate21Definition->modelPointEffectId2C,childNode,modelPointSource,worldRuntime)
                ;
                modelNode = savedModelNode;
              }
              else {
                countdownMark = (ArmyRuntimeSlot *)
                            ((int)&countdownMark->modelRuntimeOrSavedOffset +
                            classUpdate21Definition->modelPointStep30);
                if (countdownMark == (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime) {
                  ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
                            (0,sampleCoord,(childNode->worldTransform).translation.y,
                             (childNode->worldTransform).translation.x,3,
                             classUpdate21Definition->modelPointEffectId2C,childNode,modelPointSource,
                             worldRuntime);
                  modelNode = savedModelNode;
                }
                else {
                  countdownMark = (ArmyRuntimeSlot *)
                              ((int)&countdownMark->modelRuntimeOrSavedOffset +
                              classUpdate21Definition->modelPointStep30);
                  if (countdownMark == (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime) {
                    ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
                              (0,sampleCoord,(childNode->worldTransform).translation.y,
                               (childNode->worldTransform).translation.x,2,
                               classUpdate21Definition->modelPointEffectId2C,childNode,modelPointSource,
                               worldRuntime);
                    modelNode = savedModelNode;
                  }
                  else if ((ArmyRuntimeSlot *)
                           ((int)&countdownMark->modelRuntimeOrSavedOffset +
                           classUpdate21Definition->modelPointStep30) ==
                           (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime) {
                    ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
                              (0,sampleCoord,(childNode->worldTransform).translation.y,
                               (childNode->worldTransform).translation.x,1,
                               classUpdate21Definition->modelPointEffectId2C,childNode,modelPointSource,
                               worldRuntime);
                    modelNode = savedModelNode;
                  }
                }
              }
            }
          }
        }
      }
      modelRuntime = savedModelRuntime;
      classUpdate21Definition = savedClassUpdate21Definition;
      remainingTicks = remainingTicks - 1;
    } while (remainingTicks != 0);
    break;
  case 6: /* off the map again: line up for the landing arc */
    if (homeModelRuntime != NULL) {
      modelNode = (homeModelRuntime->rootModelNodeOrSavedOffset).modelNode;
      if (((homeModelRuntime->classState).classStateB0 == 0) &&
         (pointAllowed = ArmyRuntime_TestWorldPointAllowedDefault
                             (currentDefinition->worldPointAllowedContext48,
                              (modelNode->worldTransform).translation.y,
                              (modelNode->worldTransform).translation.x), !pointAllowed)) {
        (homeModelRuntime->classState).classStateB0 = 1;
        ArmyRuntime_TrySpawnDefinitionEffectAtWorldPoint
                  ((ArmyRuntimeSlot *)homeModelRuntime,worldRuntime);
        headingOrDelta = (modelNode->modelPayload).worldRotationAngle2;
        sinCosStep = FixedMath_SinCosScaled
                           (headingOrDelta ^ FIXED_ANGLE16_HALF_TURN,currentDefinition->movementStepQ12_0C * currentDefinition->phaseDurationC4);
        stepValue = (modelNode->worldTransform).translation.y;
        childNode = modelRuntime->rootModelNode;
        (childNode->worldTransform).translation.x =
             (int)sinCosStep + (modelNode->worldTransform).translation.x;
        (childNode->worldTransform).translation.y = (int)(sinCosStep >> 0x20) + stepValue;
        (childNode->modelPayload).worldRotationAngle2 = headingOrDelta;
        stateValue = currentDefinition->phaseDurationC4;
        stepValue = currentDefinition->movementStepQ12_0C;
        (modelRuntime->classLinkState).classState68 = stateValue;
        (modelRuntime->classLinkState).classState7C = -stateValue * stepValue;
        (modelRuntime->class21State84).classStateB8 = 2;
      }
      break;
    }
    ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive
              (worldRuntime,(int *)modelRuntime->ownerArmyRuntime);
    /* no home pad left: falls through */
  case 0:
    if (homeModelRuntime == NULL) {
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
  (*g_ArmyPlacementContactKindDispatchTable.callbacks
    [((ModelDefinitionRuntimeSemanticView280 *)modelRuntime->modelDefinition)->
     placementContactKindIndex278])
            (((ModelDefinitionRuntimeSemanticView280 *)modelRuntime->modelDefinition)->
             placementHeightOffsetQ12,(modelNode->worldTransform).translation.y,
             (modelNode->worldTransform).translation.x,modelNode,worldRuntime);
  stateValue = ((ModelRuntimeSlotClassState7C *)&modelRuntime->class21State84)->classStateB8;
  (modelNode->modelPayload).worldRotationAngle1 = savedAngle1;
  (modelNode->modelPayload).worldRotationAngle0 = savedAngle0;
  if ((stateValue != 0) && (stateValue != 1)) {
    ArmyRuntime_UpdateTimedShotAndEffectEmitters
              (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
  }
  semanticDefinition = (ModelDefinitionRuntimeSemanticView280 *)modelRuntime->modelDefinition;
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNode);
  ModelNodeRuntime_RecomputeSubtreeBoundingRadius(modelNode);
  ModelNodeRuntime_UpdateDepthBinMasks(semanticDefinition->placementRadiusOrClearanceDC,modelNode);
  return;
}


/* Address: 0x00526620.
   Runtime update of the aircraft home pad class (22), reached only through
   g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[22]. While the pad is being dismantled it
   passes the dismantling on to every aircraft (class 21) based on it. It builds one queued secondary army asset
   (asset flag 8) at a time: Xenite is paid up front, the asset's Energy load is held while building, and the
   finished id goes into a free slot of +0x78 (with a notification for the active faction). Then it drives the
   hangar transition in +0xB0 (1 open, 2 lift, 4 lower, 5 close, 6/0 idle) and launches pending linked aircraft.
*/

void ArmyRuntimeClass_UpdateLinkedModelFlagsAndDispatchTerrainContactMode(WorldRuntimeContext *worldRuntime,
          ModelRuntimeLinkedChildSpawnAndBuildView200 *modelRuntime)

{
  FactionArmyAssetCount *assetCountField;
  FactionRelationCounter *relationCounter;
  uint8_t *pendingSpawnCount;
  Q12 *childTranslationZ;
  int *ownerPayload;
  uint32_t candidateAsset;
  uint32_t selectedAssetValue;
  int factionIndexOrLimit;
  uint32_t completedAssetValue;
  PckArmyAssetIdCatalog secondaryAssetId;
  ModelRuntimeNode *modelNodeRuntime;
  DirectSoundVoiceSet **soundVoiceSet;
  ModelRuntimeNode *childNode;
  ModelDefinitionRecordPrefix *linkedModelDefinition;
  FactionArmyAssetCount remainingAssetCount;
  int reverseSlotIndex;
  uint32_t tickOrSoundIndex;
  WorldRuntimeNode *ownerNodeCursor;
  uint32_t *dwordCursor;
  bool testResult;
  ArmyAssetLookupResult assetLookup;
  ModelDefinitionResult definitionLookup;
  ModelLookupPayloadResult lookupPayload;
  Q12 translationStep;
  PckModelDefinitionIdCatalog notificationMovieId;
  ModelDefinitionLinkedChildStateView280 *linkedChildDefinition;
  ArmyRuntimeSlot *ownerArmyRuntime;
  
  if ((modelRuntime->linkedChildRuntimeFlagsEC & ARMY_MODEL_STATE_DISMANTLING) != 0) {
    ownerNodeCursor = (WorldRuntimeNode *)worldRuntime->ownerListHead;
    do {
      if ((((ownerNodeCursor[2].common.nextNode == NULL) &&
           (ownerPayload = ownerNodeCursor->runtimePayload, *(int *)(*ownerPayload + 0x4c) == MODEL_RUNTIME_CLASS_21_AIRCRAFT)) &&
          (modelRuntime == (ModelRuntimeLinkedChildSpawnAndBuildView200 *)ownerPayload[0x18])) &&
         ((ownerPayload[0x3b] & 0x10U) == 0)) { /* +0xEC: the aircraft is not dismantling already */
        ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive(worldRuntime,(int *)ownerPayload[2]);
      }
      ownerNodeCursor = (ownerNodeCursor->common).nextNode;
    } while (ownerNodeCursor != NULL);
  }
  switch(modelRuntime->secondaryArmyAssetBuildStateAC) {
  case 0:
    if ((modelRuntime->linkedChildRuntimeFlagsEC & ARMY_MODEL_STATE_RESEARCHING) == 0) {
      if (((modelRuntime->linkedChildRuntimeFlagsEC & 0xc9) == 0) &&
         (factionIndexOrLimit = ((modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime)->factionIndex,
         (modelRuntime->linkedChildBuildState60).completedSecondaryArmyAssetCount6C <
         modelRuntime->modelDefinition->linkedChildSlotCapacityC4)) {
        remainingAssetCount = g_GameFactionRuntimeImage.records[factionIndexOrLimit].secondaryArmyAssetCount;
        dwordCursor = g_GameFactionRuntimeImage.records[factionIndexOrLimit].secondaryArmyAssetPointersOrIds;
        if (remainingAssetCount != 0) {
ArmyRuntimeClass_SelectAffordableSecondaryArmyAssetLoop:
          /* queued asset record: +0x08 id, +0x14 flags, +0x24 build ticks, +0x28 Xenite cost, +0x2C Energy load */
          candidateAsset = *dwordCursor;
          if (((*(uint32_t *)(candidateAsset + 0x14) & 8) == 0) ||
             (g_GameFactionRuntimeImage.records[factionIndexOrLimit].xeniteCurrentQ4 < *(uint32_t *)(candidateAsset + 0x28)))
          goto ArmyRuntimeClass_AdvanceSecondaryArmyAssetCandidate;
          g_GameFactionRuntimeImage.records[factionIndexOrLimit].xeniteCurrentQ4 =
               g_GameFactionRuntimeImage.records[factionIndexOrLimit].xeniteCurrentQ4 - *(uint32_t *)(candidateAsset + 0x28);
          tickOrSoundIndex = *(uint32_t *)(candidateAsset + 0x24);
          selectedAssetValue = *(uint32_t *)(candidateAsset + 0x2c);
          if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_CHEAT_FAST_BUILD) != 0) {
            tickOrSoundIndex = (tickOrSoundIndex >> 4) + 1;
          }
          secondaryAssetId = *(PckArmyAssetIdCatalog *)(candidateAsset + 8);
          (modelRuntime->linkedChildBuildState60).secondaryArmyAssetBuildRequiredTicks68 = tickOrSoundIndex;
          (modelRuntime->linkedChildBuildState60).selectedSecondaryArmyAssetValue74 = selectedAssetValue;
          (modelRuntime->linkedChildBuildState60).selectedSecondaryArmyAssetId60 = secondaryAssetId;
          modelRuntime->definitionDerivedValueF4 = modelRuntime->definitionDerivedValueF4 + selectedAssetValue;
          (modelRuntime->linkedChildBuildState60).secondaryArmyAssetBuildElapsedTicks64 = 0;
          assetCountField = &g_GameFactionRuntimeImage.records[factionIndexOrLimit].secondaryArmyAssetCount;
          *assetCountField = *assetCountField - 1;
          do {
            *dwordCursor = dwordCursor[1];
            dwordCursor = dwordCursor + 1;
            remainingAssetCount = remainingAssetCount - 1;
          } while (remainingAssetCount != 0);
          modelRuntime->secondaryArmyAssetBuildStateAC = 1;
          modelRuntime->linkedChildRuntimeFlagsEC = modelRuntime->linkedChildRuntimeFlagsEC | ARMY_MODEL_STATE_PRODUCING;
          break;
        }
      }
    }
    else if ((modelRuntime->linkedChildRuntimeFlagsEC & 9) == 0) {
      ArmyRuntime_UpdateTimedShotAndEffectEmitters
                (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
      ArmyRuntime_UpdateAnimatedModelSubnodes
                (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
    }
    break;
  case 1:
    if ((modelRuntime->linkedChildRuntimeFlagsEC & 9) == 0) {
      dwordCursor = &(modelRuntime->linkedChildBuildState60).secondaryArmyAssetBuildElapsedTicks64;
      *dwordCursor = *dwordCursor + g_InGameSimulationStepTicks;
      ownerArmyRuntime = (modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime;
      ArmyRuntime_UpdateTimedShotAndEffectEmitters
                (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
      tickOrSoundIndex = (modelRuntime->linkedChildBuildState60).secondaryArmyAssetBuildElapsedTicks64;
      ArmyRuntime_UpdateAnimatedModelSubnodes
                (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
      if ((modelRuntime->linkedChildBuildState60).secondaryArmyAssetBuildRequiredTicks68 <= tickOrSoundIndex)
      {
        factionIndexOrLimit = ownerArmyRuntime->factionIndex;
        completedAssetValue = (modelRuntime->linkedChildBuildState60).selectedSecondaryArmyAssetValue74;
        (modelRuntime->linkedChildBuildState60).selectedSecondaryArmyAssetValue74 = 0;
        modelRuntime->secondaryArmyAssetBuildStateAC = 0;
        modelRuntime->linkedChildRuntimeFlagsEC =
             modelRuntime->linkedChildRuntimeFlagsEC & ~ARMY_MODEL_STATE_PRODUCING;
        dwordCursor = &modelRuntime->definitionDerivedValueF4;
        tickOrSoundIndex = *dwordCursor;
        *dwordCursor = *dwordCursor - completedAssetValue;
        secondaryAssetId = (modelRuntime->linkedChildBuildState60).selectedSecondaryArmyAssetId60;
        (modelRuntime->linkedChildBuildState60).selectedSecondaryArmyAssetId60 = 0;
        if (completedAssetValue <= tickOrSoundIndex) {
          reverseSlotIndex = modelRuntime->modelDefinition->linkedChildSlotCapacityC4 - 1;
          do {
            if (modelRuntime->completedSecondaryArmyAssetIds78[reverseSlotIndex] == 0)
            goto ArmyRuntimeClass_StoreCompletedSecondaryArmyAssetId;
            reverseSlotIndex = reverseSlotIndex - 1;
          } while (-1 < reverseSlotIndex);
          reverseSlotIndex = 0; /* no free slot: overwrite the first */
ArmyRuntimeClass_StoreCompletedSecondaryArmyAssetId:
          modelRuntime->completedSecondaryArmyAssetIds78[reverseSlotIndex] = secondaryAssetId;
          relationCounter = &g_GameFactionRuntimeImage.records[factionIndexOrLimit].relationCounterA;
          *relationCounter = *relationCounter + 1;
          assetCountField = &(modelRuntime->linkedChildBuildState60).completedSecondaryArmyAssetCount6C;
          *assetCountField = *assetCountField + 1;
          dwordCursor = &(modelRuntime->linkedChildBuildState60).classState70;
          *dwordCursor = *dwordCursor + 1;
          if (factionIndexOrLimit == worldRuntime->activeFactionRuntimeIndex) {
            assetLookup = ArmyAssetRegistry_FindById(secondaryAssetId);
            definitionLookup = ModelDefinition_SelectFactionUnlockedLinkedDefinition
                               (ownerArmyRuntime->factionIndex,(assetLookup.recordOrError)->rootNodeOffsetOrPointer);
            linkedModelDefinition = definitionLookup.modelDefinition;
            linkedModelDefinition[0x24].byteSize = linkedModelDefinition[0x24].byteSize + 1;
            notificationMovieId = linkedModelDefinition[0x1d].flags;
            if (linkedModelDefinition[0x24].byteSize != 1) {
              notificationMovieId = linkedModelDefinition[0x1d].definitionId;
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
ArmyRuntimeClass_DispatchLinkedChildSpawnAndDamageEffectState:
  linkedChildDefinition = modelRuntime->modelDefinition;
  modelNodeRuntime = (modelRuntime->rootModelNodeOrSavedOffset).modelNode;
  switch(modelRuntime->linkedChildTransitionStateB0) {
  case 0: /* idle */
    goto ArmyRuntimeClass_ProcessPendingLinkedChildSpawnsAndDamageEffect;
  case 1: /* opening: scroll the hatch texture */
    modelNodeRuntime->primaryTextureOffsetV =
         modelNodeRuntime->primaryTextureOffsetV +
         linkedChildDefinition->linkedChildTextureVStepPerTick0C * g_InGameSimulationStepTicks;
    if (0x7ffff < modelNodeRuntime->primaryTextureOffsetV) {
      modelNodeRuntime->primaryTextureOffsetV = 0x80000;
      modelRuntime->linkedChildTransitionStateB0 = 2;
      tickOrSoundIndex = linkedChildDefinition->linkedChildTransitionSoundAssetIndex270;
      if (((tickOrSoundIndex != 0) && (tickOrSoundIndex < worldRuntime->dwordArrayCount)) &&
         (worldRuntime->dwordArray != NULL)) {
        soundVoiceSet = (DirectSoundVoiceSet **)worldRuntime->dwordArray[tickOrSoundIndex];
        if ((soundVoiceSet != NULL) &&
           (testResult = TerrainGrid_TestProjectedCellMaskBits01
                               ((modelNodeRuntime->worldTransform).translation.y,
                                (modelNodeRuntime->worldTransform).translation.x,worldRuntime),
           !testResult)) {
          SpatialSound_PlayPositionedOneShot
                    (linkedChildDefinition->positionedSoundMaximumDistanceQ12,linkedChildDefinition->positionedSoundGainQ15,
                     &(modelNodeRuntime->worldTransform).translation,soundVoiceSet);
        }
      }
    }
    break;
  case 2: /* lifting the platform (child 0) */
    childNode = modelNodeRuntime->childNodes[0];
    factionIndexOrLimit = linkedChildDefinition->linkedChildTranslationLimitQ12CC;
    childTranslationZ = &(childNode->modelPayload).localTranslationZQ12;
    *childTranslationZ = *childTranslationZ + linkedChildDefinition->linkedChildTranslationStepQ12PerTickC8 *
                        g_InGameSimulationStepTicks;
    if (factionIndexOrLimit < (childNode->modelPayload).localTranslationZQ12) {
      modelRuntime->linkedChildTransitionStateB0 = 3;
      (childNode->modelPayload).localTranslationZQ12 = factionIndexOrLimit;
    }
    ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
    break;
  case 4: /* lowering the platform */
    translationStep = linkedChildDefinition->linkedChildTranslationStepQ12PerTickC8;
    lookupPayload = ModelLookupTable_FindPackedKeyEntryRegs
                       (0,1,(modelNodeRuntime->modelPayload).modelResource);
    childNode = modelNodeRuntime->childNodes[0];
    childTranslationZ = &(childNode->modelPayload).localTranslationZQ12;
    *childTranslationZ = *childTranslationZ - translationStep * g_InGameSimulationStepTicks;
    if ((childNode->modelPayload).localTranslationZQ12 < (int)lookupPayload.payload12) {
      modelRuntime->linkedChildTransitionStateB0 = 5;
      (childNode->modelPayload).localTranslationZQ12 = lookupPayload.payload12;
    }
    ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
    if (modelRuntime->linkedChildTransitionStateB0 == 5) {
      tickOrSoundIndex = linkedChildDefinition->linkedChildTransitionSoundAssetIndex26C;
      if (((tickOrSoundIndex != 0) && (tickOrSoundIndex < worldRuntime->dwordArrayCount)) &&
         (worldRuntime->dwordArray != NULL)) {
        soundVoiceSet = (DirectSoundVoiceSet **)worldRuntime->dwordArray[tickOrSoundIndex];
        if ((soundVoiceSet != NULL) &&
           (testResult = TerrainGrid_TestProjectedCellMaskBits01
                               ((modelNodeRuntime->worldTransform).translation.y,
                                (modelNodeRuntime->worldTransform).translation.x,worldRuntime),
           !testResult)) {
          SpatialSound_PlayPositionedOneShot
                    (linkedChildDefinition->positionedSoundMaximumDistanceQ12,linkedChildDefinition->positionedSoundGainQ15,
                     &(modelNodeRuntime->worldTransform).translation,soundVoiceSet);
        }
      }
    }
    break;
  case 5: /* closing */
    modelNodeRuntime->primaryTextureOffsetV =
         modelNodeRuntime->primaryTextureOffsetV -
         linkedChildDefinition->linkedChildTextureVStepPerTick0C * g_InGameSimulationStepTicks;
    if (modelNodeRuntime->primaryTextureOffsetV < 1) {
      modelNodeRuntime->primaryTextureOffsetV = 0;
      modelRuntime->linkedChildTransitionStateB0 = 6;
    }
    break;
  case 6:
    modelRuntime->linkedChildTransitionStateB0 = 0;
ArmyRuntimeClass_ProcessPendingLinkedChildSpawnsAndDamageEffect:
    if (((modelRuntime->linkedChildRuntimeFlagsEC & 9) == 0) &&
       (testResult = ArmyRuntime_TestWorldPointAllowedDefault
                           (linkedChildDefinition->runtimeValue48,(modelNodeRuntime->worldTransform).translation.y
                            ,(modelNodeRuntime->worldTransform).translation.x), !testResult)) {
      if ((modelRuntime->linkedChildPendingSpawnCountsDC).slot0 != 0) {
        (modelRuntime->linkedChildPendingSpawnCountsDC).slot0 =
             (modelRuntime->linkedChildPendingSpawnCountsDC).slot0 - 1;
        testResult = ArmyRuntimeSpawner_CreateLinkedChildInstance
                           (modelRuntime->linkedChildSpawnInheritedStateB8[0].inheritedValue78,
                            modelRuntime->linkedChildSpawnInheritedStateB8[0].inheritedValue74,
                            modelRuntime->linkedChildSpawnInheritedStateB8[0].inheritedValue70,
                            g_ArmyLinkedChildAssetIdSlot0,worldRuntime,
                            (ArmyRuntimeLinkedChildMaskSlotView *)modelRuntime);
        if (!testResult) {
          modelRuntime->linkedChildTransitionStateB0 = 1;
          tickOrSoundIndex = linkedChildDefinition->linkedChildTransitionSoundAssetIndex26C;
          if (((tickOrSoundIndex != 0) && (tickOrSoundIndex < worldRuntime->dwordArrayCount)) &&
             (worldRuntime->dwordArray != NULL)) {
            soundVoiceSet = (DirectSoundVoiceSet **)worldRuntime->dwordArray[tickOrSoundIndex];
            if ((soundVoiceSet != NULL) &&
               (testResult = TerrainGrid_TestProjectedCellMaskBits01
                                   ((modelNodeRuntime->worldTransform).translation.y,
                                    (modelNodeRuntime->worldTransform).translation.x,worldRuntime),
               !testResult)) {
              SpatialSound_PlayPositionedOneShot
                        (linkedChildDefinition->positionedSoundMaximumDistanceQ12,linkedChildDefinition->positionedSoundGainQ15,
                         &(modelNodeRuntime->worldTransform).translation,soundVoiceSet);
            }
          }
          break;
        }
      }
      if ((modelRuntime->linkedChildPendingSpawnCountsDC).slot1 != 0) {
        pendingSpawnCount = &(modelRuntime->linkedChildPendingSpawnCountsDC).slot1;
        *pendingSpawnCount = *pendingSpawnCount - 1;
        testResult = ArmyRuntimeSpawner_CreateLinkedChildInstance
                           (modelRuntime->linkedChildSpawnInheritedStateB8[1].inheritedValue78,
                            modelRuntime->linkedChildSpawnInheritedStateB8[1].inheritedValue74,
                            modelRuntime->linkedChildSpawnInheritedStateB8[1].inheritedValue70,
                            g_ArmyLinkedChildAssetIdSlot1,worldRuntime,
                            (ArmyRuntimeLinkedChildMaskSlotView *)modelRuntime);
        if (!testResult) {
          modelRuntime->linkedChildTransitionStateB0 = 1;
          tickOrSoundIndex = linkedChildDefinition->linkedChildTransitionSoundAssetIndex26C;
          if (((tickOrSoundIndex != 0) && (tickOrSoundIndex < worldRuntime->dwordArrayCount)) &&
             (worldRuntime->dwordArray != NULL)) {
            soundVoiceSet = (DirectSoundVoiceSet **)worldRuntime->dwordArray[tickOrSoundIndex];
            if ((soundVoiceSet != NULL) &&
               (testResult = TerrainGrid_TestProjectedCellMaskBits01
                                   ((modelNodeRuntime->worldTransform).translation.y,
                                    (modelNodeRuntime->worldTransform).translation.x,worldRuntime),
               !testResult)) {
              SpatialSound_PlayPositionedOneShot
                        (linkedChildDefinition->positionedSoundMaximumDistanceQ12,linkedChildDefinition->positionedSoundGainQ15,
                         &(modelNodeRuntime->worldTransform).translation,soundVoiceSet);
            }
          }
          break;
        }
      }
      if ((modelRuntime->linkedChildPendingSpawnCountsDC).slot2 != 0) {
        pendingSpawnCount = &(modelRuntime->linkedChildPendingSpawnCountsDC).slot2;
        *pendingSpawnCount = *pendingSpawnCount - 1;
        testResult = ArmyRuntimeSpawner_CreateLinkedChildInstance
                           (modelRuntime->linkedChildSpawnInheritedStateB8[2].inheritedValue78,
                            modelRuntime->linkedChildSpawnInheritedStateB8[2].inheritedValue74,
                            modelRuntime->linkedChildSpawnInheritedStateB8[2].inheritedValue70,
                            g_ArmyLinkedChildAssetIdSlot2,worldRuntime,
                            (ArmyRuntimeLinkedChildMaskSlotView *)modelRuntime);
        if (!testResult) {
          modelRuntime->linkedChildTransitionStateB0 = 1;
          tickOrSoundIndex = linkedChildDefinition->linkedChildTransitionSoundAssetIndex26C;
          if (((tickOrSoundIndex != 0) && (tickOrSoundIndex < worldRuntime->dwordArrayCount)) &&
             (worldRuntime->dwordArray != NULL)) {
            soundVoiceSet = (DirectSoundVoiceSet **)worldRuntime->dwordArray[tickOrSoundIndex];
            if ((soundVoiceSet != NULL) &&
               (testResult = TerrainGrid_TestProjectedCellMaskBits01
                                   ((modelNodeRuntime->worldTransform).translation.y,
                                    (modelNodeRuntime->worldTransform).translation.x,worldRuntime),
               !testResult)) {
              SpatialSound_PlayPositionedOneShot
                        (linkedChildDefinition->positionedSoundMaximumDistanceQ12,linkedChildDefinition->positionedSoundGainQ15,
                         &(modelNodeRuntime->worldTransform).translation,soundVoiceSet);
            }
          }
        }
      }
    }
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  return;
ArmyRuntimeClass_AdvanceSecondaryArmyAssetCandidate:
  dwordCursor = dwordCursor + 1;
  remainingAssetCount = remainingAssetCount - 1;
  if (remainingAssetCount == 0) goto ArmyRuntimeClass_DispatchLinkedChildSpawnAndDamageEffectState;
  goto ArmyRuntimeClass_SelectAffordableSecondaryArmyAssetLoop;
}


/* Address: 0x00524740.
   Runtime update of the unit factory class (13), reached only through
   g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[13]. On the first update it stores the
   factory's exit point (model lookup key 1/5). It builds one queued secondary army asset whose flags match the
   definition's mask (Xenite paid up front, Energy load held while building), creates the army at the spawn
   point, opens the door, sends the army out to the exit point, waits until it has left and closes the door.
*/
void ArmyRuntimeClassUpdateSlot13_PrepareModelAndDispatchByClassId
          (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView200 *modelRuntime)

{
  uint8_t *reservedFlags;
  FactionRelationCounter *relationCounter;
  uint32_t *derivedValue;
  FactionArmyAssetCount *assetCountField;
  ModelRuntimeNode *rootNode;
  ModelDefinitionRuntimeSemanticView280 *semanticDefinition;
  ModelRuntimeSlot *linkedModelRuntime;
  int factionOrNodeValue;
  int *linkedPayload;
  int definitionAddress;
  int nodeHeading;
  DirectSoundVoiceSet **soundVoiceSet;
  ModelRuntimeSlotLinkOrState4 selectedAssetLink;
  uint32_t stateValue;
  ArmyRuntimeSlot *createdArmyRuntime;
  Q12 targetWorldXQ12;
  uint32_t secondaryValue;
  Q12 worldXQ12;
  GraphicsFixedVec3 *translationVec;
  FactionArmyAssetCount remainingAssetCount;
  Q12 worldYQ12;
  uint32_t tickOrSoundIndex;
  uint32_t *dwordCursor;
  bool cellMasked;
  ModelLookupEntryResult lookupEntry;
  ArmyRuntimeCreateResult createResult;
  ModelWorldPoint localPoint;
  InGameNotificationMovieId notificationMovieId;
  ArmyRuntimeSlot *linkedArmyRuntime;
  
  rootNode = modelRuntime->rootModelNode;
  if ((*(uint32_t *)(modelRuntime->classState).reservedBC_BF & 1) != 0) {
    lookupEntry = ModelLookupTable_ContainsPackedKey(1,5,(rootNode->modelPayload).modelResource);
    if (!lookupEntry.notFound) {
      localPoint = ModelNodeRuntime_TransformLocalPointRegs(lookupEntry.entry,rootNode);
      (modelRuntime->classLinkState).classState78 = localPoint.xQ12;
      (modelRuntime->classLinkState).classState7C = localPoint.yQ12;
      reservedFlags = (modelRuntime->classState).reservedBC_BF;
      *(uint32_t *)reservedFlags = *(uint32_t *)reservedFlags & 0xfffffffe;
    }
  }
  stateValue = (modelRuntime->classState).classStateB8;
  semanticDefinition = modelRuntime->modelDefinition;
  if ((3 < rootNode->childCount) && (rootNode->childNodes[3] != NULL)) {
    WorldRuntime_UnlinkNodeFromOwnerListD8((WorldOwnerListNode100 *)rootNode->childNodes[3]);
    rootNode->childNodes[3] = NULL;
  }
  switch(stateValue) {
  case 0: /* idle: start the first affordable queued asset this factory can build */
    if (((modelRuntime->classState).classStateEC & ARMY_MODEL_STATE_RESEARCHING) == 0) {
      if (((modelRuntime->classState).classStateEC & 0xc9) == 0) {
        factionOrNodeValue = modelRuntime->ownerArmyRuntime->factionIndex;
        remainingAssetCount = g_GameFactionRuntimeImage.records[factionOrNodeValue].secondaryArmyAssetCount;
        dwordCursor = g_GameFactionRuntimeImage.records[factionOrNodeValue].secondaryArmyAssetPointersOrIds;
        if (remainingAssetCount != 0) {
          do {
            if (((*(uint32_t *)(*dwordCursor + 0x14) &
                 *(uint32_t *)(modelRuntime->modelDefinition->reserved0C0_0DB + 4)) != 0) &&
               (tickOrSoundIndex = *(uint32_t *)(*dwordCursor + 0x28),
               tickOrSoundIndex <= g_GameFactionRuntimeImage.records[factionOrNodeValue].xeniteCurrentQ4)) {
              stateValue = *dwordCursor;
              g_GameFactionRuntimeImage.records[factionOrNodeValue].xeniteCurrentQ4 =
                   g_GameFactionRuntimeImage.records[factionOrNodeValue].xeniteCurrentQ4 - tickOrSoundIndex;
              tickOrSoundIndex = *(uint32_t *)(stateValue + 0x24);
              secondaryValue = *(uint32_t *)(stateValue + 0x2c);
              if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_CHEAT_FAST_BUILD) != 0) {
                tickOrSoundIndex = (tickOrSoundIndex >> 4) + 1;
              }
              selectedAssetLink = *(ModelRuntimeSlotLinkOrState4 *)(stateValue + 8);
              (modelRuntime->classLinkState).classState68 = tickOrSoundIndex;
              (modelRuntime->classLinkState).classState74 = secondaryValue;
              (modelRuntime->classLinkState).modelLinkOrState60 = selectedAssetLink;
              derivedValue = &(modelRuntime->classState).definitionDerivedValueF4;
              *derivedValue = *derivedValue + secondaryValue;
              (modelRuntime->classLinkState).classState64 = 0;
              assetCountField = &g_GameFactionRuntimeImage.records[factionOrNodeValue].secondaryArmyAssetCount;
              *assetCountField = *assetCountField - 1;
              do {
                *dwordCursor = dwordCursor[1];
                dwordCursor = dwordCursor + 1;
                remainingAssetCount = remainingAssetCount - 1;
              } while (remainingAssetCount != 0);
              (modelRuntime->classState).classStateB8 = 1;
              dwordCursor = &(modelRuntime->classState).classStateEC;
              *dwordCursor = *dwordCursor | ARMY_MODEL_STATE_PRODUCING;
              break;
            }
            dwordCursor = dwordCursor + 1;
            remainingAssetCount = remainingAssetCount - 1;
          } while (remainingAssetCount != 0);
        }
      }
    }
    else if (((modelRuntime->classState).classStateEC & 9) == 0) {
      ArmyRuntime_UpdateTimedShotAndEffectEmitters(worldRuntime,modelRuntime);
      ArmyRuntime_UpdateAnimatedModelSubnodes(worldRuntime,modelRuntime);
    }
    break;
  case 1: /* building; when done create the army at the spawn point (lookup keys 1/5 and 0/5 give its heading) */
    if (((modelRuntime->classState).classStateEC & 9) == 0) {
      dwordCursor = &(modelRuntime->classLinkState).classState64;
      *dwordCursor = *dwordCursor + g_InGameSimulationStepTicks;
      ArmyRuntime_UpdateTimedShotAndEffectEmitters(worldRuntime,modelRuntime);
      tickOrSoundIndex = (modelRuntime->classLinkState).classState64;
      ArmyRuntime_UpdateAnimatedModelSubnodes(worldRuntime,modelRuntime);
      if ((modelRuntime->classLinkState).classState68 <= tickOrSoundIndex) {
        lookupEntry = ModelLookupTable_ContainsPackedKey(1,5,(rootNode->modelPayload).modelResource);
        if (!lookupEntry.notFound) {
          localPoint = ModelNodeRuntime_TransformLocalPointRegs(lookupEntry.entry,rootNode);
          secondaryValue = localPoint.yQ12;
          stateValue = localPoint.xQ12;
          lookupEntry = ModelLookupTable_ContainsPackedKey(0,5,(rootNode->modelPayload).modelResource);
          if (!lookupEntry.notFound) {
            localPoint = ModelNodeRuntime_TransformLocalPointRegs(lookupEntry.entry,rootNode);
            stateValue = FixedMath_Atan2Angle16(secondaryValue - localPoint.yQ12,stateValue - localPoint.xQ12);
            linkedArmyRuntime = modelRuntime->ownerArmyRuntime;
            createResult = ArmyRuntime_CreateInstanceFromAsset
                               (4,stateValue,localPoint.yQ12,localPoint.xQ12,linkedArmyRuntime->factionIndex,
                                (modelRuntime->classLinkState).modelLinkOrState60.classState,
                                worldRuntime);
            createdArmyRuntime = (ArmyRuntimeSlot *)createResult.armyRuntimeOrError;
            if (!createResult.failed) {
              relationCounter = &g_GameFactionRuntimeImage.records[linkedArmyRuntime->factionIndex].
                        relationCounterA;
              *relationCounter = *relationCounter + 1;
              semanticDefinition = modelRuntime->modelDefinition;
              createdArmyRuntime->movementStateFlags = createdArmyRuntime->movementStateFlags | 0x402;
              tickOrSoundIndex = *(uint32_t *)semanticDefinition->reserved26C_277;
              if (((tickOrSoundIndex != 0) && (tickOrSoundIndex < worldRuntime->dwordArrayCount)) &&
                 (worldRuntime->dwordArray != NULL)) {
                rootNode = modelRuntime->rootModelNode;
                /* as in the original (0x0052499F): the voice set is read from rootNode + index * 4, not from
                   worldRuntime->dwordArray, which is only tested for NULL */
                soundVoiceSet = *(DirectSoundVoiceSet ***)
                            ((rootNode->modelPayload).reserved2C_33 + tickOrSoundIndex * 4 - 0x38);
                translationVec = &(rootNode->worldTransform).translation;
                if ((soundVoiceSet != NULL) &&
                   (cellMasked = TerrainGrid_TestProjectedCellMaskBits01
                                       ((rootNode->worldTransform).translation.y,translationVec->x,
                                        worldRuntime), !cellMasked)) {
                  SpatialSound_PlayPositionedOneShot
                            (semanticDefinition->positionedSoundMaximumDistanceQ12,
                             semanticDefinition->positionedSoundGainQ15,translationVec,soundVoiceSet);
                }
              }
              stateValue = (modelRuntime->classLinkState).classState74;
              (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime = createdArmyRuntime;
              factionOrNodeValue = createdArmyRuntime->factionIndex;
              (modelRuntime->classState).classStateB8 = 2;
              (modelRuntime->classLinkState).classState74 = 0;
              dwordCursor = &(modelRuntime->classState).definitionDerivedValueF4;
              *dwordCursor = *dwordCursor - stateValue;
              linkedPayload = (int *)(createdArmyRuntime->modelRuntimeOrSavedOffset).savedIdOrOffset;
              createdArmyRuntime->movementStateFlags = createdArmyRuntime->movementStateFlags | 2;
              linkedPayload[0x3c] = (int)modelRuntime;
              if (factionOrNodeValue == worldRuntime->activeFactionRuntimeIndex) {
                ArmyAssetRegistry_FindById
                          ((modelRuntime->classLinkState).modelLinkOrState60.classState);
                stateValue = (worldRuntime->motion).pitchAngle;
                factionOrNodeValue = linkedPayload[1];
                definitionAddress = *linkedPayload;
                nodeHeading = *(int *)(factionOrNodeValue + 0x14);
                *(int *)(definitionAddress + 0x1b0) = *(int *)(definitionAddress + 0x1b0) + 1;
                notificationMovieId = *(InGameNotificationMovieId *)(definitionAddress + 0x160);
                if (*(int *)(definitionAddress + 0x1b0) != 1) {
                  notificationMovieId = *(InGameNotificationMovieId *)(definitionAddress + 0x164);
                }
                InGameNotificationQueue_InsertPriorityRecord
                          (ARMY_CREATED,0,stateValue,nodeHeading + 0x8800U & 0xffff,*(Q12 *)(factionOrNodeValue + 0x98),
                           *(Q12 *)(factionOrNodeValue + 0x94),2,notificationMovieId);
              }
            }
          }
        }
      }
    }
    break;
  case 2: /* opening the door, then send the new army out to the point stored at +0x78/+0x7C */
    rootNode->primaryTextureOffsetV =
         rootNode->primaryTextureOffsetV +
         *(int *)semanticDefinition->reserved00C_023 * g_InGameSimulationStepTicks;
    if (0x7ffff < rootNode->primaryTextureOffsetV) {
      rootNode->primaryTextureOffsetV = 0x80000;
      (modelRuntime->classState).classStateB8 = 3;
      lookupEntry = ModelLookupTable_ContainsPackedKey(1,5,(rootNode->modelPayload).modelResource);
      if (!lookupEntry.notFound) {
        linkedArmyRuntime = (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime;
        localPoint = ModelNodeRuntime_TransformLocalPointRegs(lookupEntry.entry,rootNode);
        targetWorldXQ12 = localPoint.yQ12;
        linkedModelRuntime = (linkedArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
        ArmyRuntime_StartMoveCommandWithAuxiliaryValues
                  ((modelRuntime->classLinkState).classState7C,
                   (modelRuntime->classLinkState).classState78,targetWorldXQ12,localPoint.xQ12,
                   (ArmyMovementRuntime *)linkedArmyRuntime);
        (linkedModelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime =
             (ArmyRuntimeSlot *)modelRuntime;
      }
    }
    break;
  case 3: /* wait until the army has left (it no longer links back to this factory) */
    linkedArmyRuntime = (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime;
    if ((linkedArmyRuntime == NULL) ||
       ((ModelRuntimeUpdateView200 *)
        (((linkedArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->classState).
        linkedArmyRuntimeOrSavedOffset.armyRuntime != modelRuntime)) {
      semanticDefinition = modelRuntime->modelDefinition;
      (modelRuntime->classState).classStateB8 = 4;
      (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime = NULL;
      tickOrSoundIndex = *(uint32_t *)semanticDefinition->reserved26C_277;
      if ((tickOrSoundIndex != 0) &&
         ((tickOrSoundIndex < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != NULL)))) {
        rootNode = modelRuntime->rootModelNode;
        /* same original quirk as in case 1 (0x00524B0B) */
        soundVoiceSet = *(DirectSoundVoiceSet ***)
                    ((rootNode->modelPayload).reserved2C_33 + tickOrSoundIndex * 4 - 0x38);
        translationVec = &(rootNode->worldTransform).translation;
        if ((soundVoiceSet != NULL) &&
           (cellMasked = TerrainGrid_TestProjectedCellMaskBits01
                               ((rootNode->worldTransform).translation.y,translationVec->x,worldRuntime),
           !cellMasked)) {
          SpatialSound_PlayPositionedOneShot
                    (semanticDefinition->positionedSoundMaximumDistanceQ12,semanticDefinition->positionedSoundGainQ15,
                     translationVec,soundVoiceSet);
        }
      }
    }
    break;
  case 4: /* closing the door */
    rootNode->primaryTextureOffsetV =
         rootNode->primaryTextureOffsetV -
         *(int *)semanticDefinition->reserved00C_023 * g_InGameSimulationStepTicks;
    if (rootNode->primaryTextureOffsetV < 1) {
      rootNode->primaryTextureOffsetV = 0;
      (modelRuntime->classState).classStateB8 = 0;
      dwordCursor = &(modelRuntime->classState).classStateEC;
      *dwordCursor = *dwordCursor & ~ARMY_MODEL_STATE_PRODUCING;
    }
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  return;
}


/* Address: 0x005240F0.
   Runtime update of production class 11, reached only through
   g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[11]. Builds one queued secondary army asset
   with flag 0x10 at a time: its Xenite cost (+0x28) is paid once up front, its Energy load (+0x2C) is held on
   the building while it is built. The finished asset is appended to the faction's primary asset list (at most
   64 entries, from where it is placed) and, for the active faction, the command grid is rebuilt and a
   notification is queued.
*/
void ArmyRuntimeClassUpdateSlot11_DispatchByClassId
          (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView200 *modelRuntime)

{
  uint32_t *derivedValue;
  FactionArmyAssetCount *assetCountField;
  uint32_t assetEnergyValue;
  ModelRuntimeSlotLinkOrState4 selectedAssetLink;
  int factionIndex;
  uint32_t candidateValue;
  int activeFactionIndex;
  AngleTurn32 headingAngle;
  ModelDefinitionRecordPrefix *linkedModelDefinition;
  FactionArmyAssetCount remainingAssetCount;
  uint32_t tickOrCount;
  uint32_t *dwordCursor;
  ArmyAssetLookupResult assetLookup;
  ModelDefinitionResult definitionLookup;
  PckModelDefinitionIdCatalog notificationMovieId;
  ArmyRuntimeSlot *ownerArmyRuntime;
  ModelRuntimeNode *rootNode;
  
  switch((modelRuntime->classState).classStateB8) {
  case 0: /* idle: start the first affordable queued asset with flag 0x10 */
    if (((modelRuntime->classState).classStateEC & ARMY_MODEL_STATE_RESEARCHING) == 0) {
      if (((modelRuntime->classState).classStateEC & 0xc9) == 0) {
        factionIndex = modelRuntime->ownerArmyRuntime->factionIndex;
        dwordCursor = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetPointersOrIds;
        for (remainingAssetCount = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount; remainingAssetCount != 0;
            remainingAssetCount = remainingAssetCount - 1) {
          candidateValue = *dwordCursor;
          if (((*(uint32_t *)(candidateValue + 0x14) & 0x10) != 0) &&
             (*(uint32_t *)(candidateValue + 0x28) <= g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4))
          {
            g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 =
                 g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 - *(uint32_t *)(candidateValue + 0x28);
            tickOrCount = *(uint32_t *)(candidateValue + 0x24);
            assetEnergyValue = *(uint32_t *)(candidateValue + 0x2c);
            if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_CHEAT_FAST_BUILD) != 0) {
              tickOrCount = (tickOrCount >> 4) + 1;
            }
            selectedAssetLink = *(ModelRuntimeSlotLinkOrState4 *)(candidateValue + 8);
            (modelRuntime->classLinkState).classState68 = tickOrCount;
            (modelRuntime->classLinkState).classState74 = assetEnergyValue;
            (modelRuntime->classLinkState).modelLinkOrState60 = selectedAssetLink;
            derivedValue = &(modelRuntime->classState).definitionDerivedValueF4;
            *derivedValue = *derivedValue + assetEnergyValue;
            (modelRuntime->classLinkState).classState64 = 0;
            assetCountField = &g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount;
            *assetCountField = *assetCountField - 1;
            do {
              *dwordCursor = dwordCursor[1];
              dwordCursor = dwordCursor + 1;
              remainingAssetCount = remainingAssetCount - 1;
            } while (remainingAssetCount != 0);
            (modelRuntime->classState).classStateB8 = 1;
            dwordCursor = &(modelRuntime->classState).classStateEC;
            *dwordCursor = *dwordCursor | ARMY_MODEL_STATE_PRODUCING;
            break;
          }
          dwordCursor = dwordCursor + 1;
        }
      }
    }
    else if (((modelRuntime->classState).classStateEC & 9) == 0) {
      ArmyRuntime_UpdateTimedShotAndEffectEmitters(worldRuntime,modelRuntime);
      ArmyRuntime_UpdateAnimatedModelSubnodes(worldRuntime,modelRuntime);
    }
    break;
  case 1: /* building */
    if (((modelRuntime->classState).classStateEC & 9) == 0) {
      ownerArmyRuntime = modelRuntime->ownerArmyRuntime;
      dwordCursor = &(modelRuntime->classLinkState).classState64;
      *dwordCursor = *dwordCursor + g_InGameSimulationStepTicks;
      ArmyRuntime_UpdateTimedShotAndEffectEmitters(worldRuntime,modelRuntime);
      tickOrCount = (modelRuntime->classLinkState).classState64;
      ArmyRuntime_UpdateAnimatedModelSubnodes(worldRuntime,modelRuntime);
      if ((modelRuntime->classLinkState).classState68 <= tickOrCount) {
        factionIndex = ownerArmyRuntime->factionIndex;
        candidateValue = (modelRuntime->classLinkState).classState74;
        (modelRuntime->classLinkState).classState74 = 0;
        (modelRuntime->classState).classStateB8 = 0;
        dwordCursor = &(modelRuntime->classState).classStateEC;
        *dwordCursor = *dwordCursor & ~ARMY_MODEL_STATE_PRODUCING;
        dwordCursor = &(modelRuntime->classState).definitionDerivedValueF4;
        *dwordCursor = *dwordCursor - candidateValue;
        assetLookup = ArmyAssetRegistry_FindById
                           ((modelRuntime->classLinkState).modelLinkOrState60.classState);
        (modelRuntime->classLinkState).modelLinkOrState60.modelRuntime = NULL;
        if (!assetLookup.notFound) {
          tickOrCount = g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
          if (tickOrCount < 64) {
            activeFactionIndex = worldRuntime->activeFactionRuntimeIndex;
            /* appended to the faction's primary asset list: records[factionIndex] + 0x1E0 */
            *(ArmyAssetRecordPrefix **)(factionIndex * 0x740 + THANDOR_ADDR(g_GameFactionRuntimeImage,0x1e0) + tickOrCount * 4) = assetLookup.recordOrError;
            assetCountField = &g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
            *assetCountField = *assetCountField + 1;
            if (activeFactionIndex == ownerArmyRuntime->factionIndex) {
              candidateValue = (assetLookup.recordOrError)->rootNodeOffsetOrPointer;
              UiCommandSpriteVariantA_RebuildGrid((UiNodeBase *)worldRuntime);
              definitionLookup = ModelDefinition_SelectFactionUnlockedLinkedDefinition
                                 (ownerArmyRuntime->factionIndex,candidateValue);
              linkedModelDefinition = definitionLookup.modelDefinition;
              rootNode = modelRuntime->rootModelNode;
              candidateValue = (worldRuntime->motion).pitchAngle;
              headingAngle = (rootNode->modelPayload).worldRotationAngle2;
              linkedModelDefinition[0x24].byteSize = linkedModelDefinition[0x24].byteSize + 1;
              notificationMovieId = linkedModelDefinition[0x1d].flags;
              if (linkedModelDefinition[0x24].byteSize != 1) {
                notificationMovieId = linkedModelDefinition[0x1d].definitionId;
              }
              InGameNotificationQueue_InsertPriorityRecord
                        (ARMY_CREATED,0,candidateValue,headingAngle + 0x1800 & 0xffff,
                         (rootNode->worldTransform).translation.y,
                         (rootNode->worldTransform).translation.x,3,notificationMovieId);
            }
          }
        }
      }
    }
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  return;
}


/* Address: 0x00525020.
   Runtime update of the resource extractor class (14), reached only through
   g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[14]. While it has health left and is active,
   it stamps its faction and resource-field selector into the grid cell under it (inner cells only) and, when
   the cell carries the matching resource-field support bit, registers itself there and runs its emitters and
   animation.
*/
void ArmyRuntimeClass_UpdateGridBoundEffectsAndModels
          (WorldRuntimeContext *worldRuntime,ModelRuntimeClass14UpdateView200 *modelRuntime)

{
  int cellColumnOrIndex;
  int cellRow;
  FieldCellPackedFlagsAndMaterial supportFlagMask;
  FieldGridCoordinatesEaxEdx8 gridCoordinates;
  FieldGridAsset *fieldGrid;
  
  if ((1 < (int)modelRuntime->definitionValue60_3C) &&
     (((modelRuntime->classState).classStateEC & 9) == 0)) {
    gridCoordinates = FieldGrid_WorldToGridQ12
                      ((modelRuntime->rootModelNode->worldTransform).translation.y,
                       (modelRuntime->rootModelNode->worldTransform).translation.x);
    /* grid coordinates rounded to the nearest cell */
    cellColumnOrIndex = ((gridCoordinates.columnQ12 >> 0xb) + 1) >> 1;
    cellRow = ((gridCoordinates.rowQ12 >> 0xb) + 1) >> 1;
    fieldGrid = worldRuntime->fieldGrid;
    if ((0 < cellColumnOrIndex) && (0 < cellRow)) {
      if ((cellColumnOrIndex + 1 < (int)fieldGrid->gridWidth) && (cellRow + 1 < (int)fieldGrid->gridHeight)) {
        cellColumnOrIndex = cellRow * fieldGrid->gridWidth + cellColumnOrIndex;
        supportFlagMask = 0x800 << ((uint8_t)modelRuntime->modelDefinition->resourceFieldSupportSelectorC0 & 0x1f
                         );
        /* claim the cell: faction << 13, the support bit, definition +0xC8 << 24 */
        fieldGrid->cells[cellColumnOrIndex].resourceExtractionDescriptor7C =
             modelRuntime->ownerArmyRuntime->factionIndex << 0xd | supportFlagMask |
             *(int *)((uint8_t *)modelRuntime->modelDefinition + 0xc8) << 0x18;
        if ((fieldGrid->cells[cellColumnOrIndex].flagsAndMaterial & supportFlagMask) != 0) {
          /* the cell supports this extractor: register it (as a saved offset) and run its emitters */
          fieldGrid->cells[cellColumnOrIndex].armyRuntimeSavedOffset6C =
               (int)modelRuntime - g_ModelRuntimeRebaseDelta;
          ArmyRuntime_UpdateTimedShotAndEffectEmitters
                    (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
          ArmyRuntime_UpdateAnimatedModelSubnodes
                    (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
        }
      }
    }
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  return;
}


/* Address: 0x005274D0.
   Class command of the structure classes (the ten non-default class slots that share it; session conditions
   use it to tell structures from units). It hits every class-0/class-12 model standing inside the structure's
   footprint with 0x100000 impact damage, stamps the structure's ground height into the field grid (unless
   class-state bit 0x20 is set), and when every model of the same owner within reach is an idle class-18
   model, marks the last of them (flag 8) and spawns its army-from-model completion effect.
*/
void ArmyRuntime_ClassCommandHandlerGroupA(WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ModelRuntimeNode *ownNode;
  ArmyRuntimeSlot *candidateArmy;
  int64_t deltaYSquared;
  int64_t remainingRadiusSquared;
  uint32_t impactAngleOrReach;
  uint8_t *proximityRadius;
  int axisDelta;
  ModelRuntimeNode *scanNode;
  bool proximityHit;
  EffectDefinitionResult effectLookup;
  DamageAmount32 damageAmount;
  int blockingCount;
  int nextBlockingCount;
  ArmyRuntimeSlot *scanArmy;
  
  scanNode = (ModelRuntimeNode *)worldRuntime->ownerListHead;
  ownNode = armyRuntime->modelNodeRuntime;
  if ((scanNode != NULL) && ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE) == 0)) {
    do {
      if (scanNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
        scanArmy = (scanNode->runtimePayload).armyRuntime;
        if ((ownNode != scanNode) &&
           ((((scanArmy->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C == 0 ||
            (((scanArmy->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C == 0xc))))
        {
          /* Damage the candidate unless the class-13 test misses and the attachment proximity test hits. */
          proximityHit = ArmyRuntime_TestClass13ProximityCandidate(scanArmy,armyRuntime);
          if ((proximityHit) || (proximityHit = ArmyRuntime_TestModelAttachmentProximity(scanArmy,armyRuntime),
                                 !proximityHit)) {
            damageAmount = 0x100000;
            impactAngleOrReach = FixedMath_Atan2Angle16
                              ((scanNode->worldTransform).translation.y -
                               (ownNode->worldTransform).translation.y,
                               (scanNode->worldTransform).translation.x -
                               (ownNode->worldTransform).translation.x);
            ArmyRuntime_ApplyImpactDamageAndFinalizeState(impactAngleOrReach,damageAmount,scanArmy);
          }
        }
      }
      scanNode = (ModelRuntimeNode *)(scanNode->common).nextNode;
    } while (scanNode != NULL);
  }
  scanNode = armyRuntime->modelNodeRuntime;
  if (((((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->classLinkState).classState68 & 0x20
      ) == 0) {
    FieldGrid_ApplyHeightAtWorldPointAndRefreshNeighbors
              ((((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->classState).classStateDC,
               (scanNode->worldTransform).translation.z,(scanNode->worldTransform).translation.y
               ,(scanNode->worldTransform).translation.x,worldRuntime->fieldGrid);
  }
  scanNode = (ModelRuntimeNode *)worldRuntime->ownerListHead;
  ownNode = (armyRuntime->linkedEntityRuntime->common).ownership.modelNode;
  if (scanNode != NULL) {
    scanArmy = NULL;
    blockingCount = 0;
    do {
      nextBlockingCount = blockingCount;
      if ((((scanNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) && (scanNode != ownNode)) &&
          (impactAngleOrReach = ((((scanNode->runtimePayload).armyRuntime)->modelRuntimeOrSavedOffset).
                   modelRuntime)->attachments140[2].reserved1C,
          (armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex ==
          (((scanNode->runtimePayload).armyRuntime)->linkedEntityRuntime->common).ownership.
          ownerIndex)) && (impactAngleOrReach != 0)) {
        /* reach = own model-runtime dword +0x1A8 + candidate dword +0x19C (the pointer arithmetic below is
           only Ghidra's spelling of that integer sum); inside when reach^2 - dx^2 - dy^2 >= 0 in 64 bits */
        proximityRadius = ((((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->attachments140[3].
                  parentModelNodeOrSavedOffset08)->modelPayload).reserved2C_33 + (impactAngleOrReach - 0x38);
        axisDelta = (ownNode->worldTransform).translation.x - (scanNode->worldTransform).translation.x;
        remainingRadiusSquared = (int64_t)(int)proximityRadius * (int64_t)(int)proximityRadius - (int64_t)axisDelta * (int64_t)axisDelta;
        if ((-1 < remainingRadiusSquared) &&
           (axisDelta = (ownNode->worldTransform).translation.y -
                    (scanNode->worldTransform).translation.y,
           deltaYSquared = (int64_t)axisDelta * (int64_t)axisDelta,
           -1 < (int)(((int)((uint64_t)remainingRadiusSquared >> 0x20) - (int)((uint64_t)deltaYSquared >> 0x20)) -
                     (uint32_t)((uint32_t)remainingRadiusSquared < (uint32_t)deltaYSquared)))) {
          candidateArmy = (scanNode->runtimePayload).armyRuntime;
          nextBlockingCount = blockingCount + 1;
          if ((((candidateArmy->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C == 0x12) &&
             ((candidateArmy->runtimeFlags & 0x18) == 0)) {
            /* an idle class-18 model does not block; remember it as the one to complete */
            nextBlockingCount = blockingCount;
            scanArmy = candidateArmy;
          }
        }
      }
      scanNode = (ModelRuntimeNode *)(scanNode->common).nextNode;
      blockingCount = nextBlockingCount;
    } while (scanNode != NULL);
    if ((nextBlockingCount == 0) && (scanArmy != NULL)) {
      scanNode = scanArmy->modelNodeRuntime;
      scanArmy->runtimeFlags = scanArmy->runtimeFlags | 8;
      effectLookup = EffectDefinitionRegistry_FindByIdWithError
                        (*(PckEffectDefinitionIdCatalog *)
                          (((scanArmy->modelRuntimeOrSavedOffset).modelRuntime)->classState).
                          reservedC4_C7);
      if (!effectLookup.notFound) {
        EffectRuntimePool_CreateInstanceFromDefinition
                  (EFFECT_RUNTIME_COMPLETION_SPAWN_ARMY_FROM_MODEL,
                   THANDOR_BITCAST(GameEntityRuntime *, EffectRuntimeOwnerReference4, scanArmy->linkedEntityRuntime),
                   (scanNode->modelPayload).worldRotationAngle2,
                   (scanNode->modelPayload).worldRotationAngle1,
                   (scanNode->modelPayload).worldRotationAngle0,
                   (scanNode->worldTransform).translation.z,
                   (scanNode->worldTransform).translation.y,
                   (scanNode->worldTransform).translation.x,effectLookup.definitionOrError,worldRuntime);
      }
    }
  }
  return;
}


/* Address: 0x0051D140.
   Army entry of the terrainStateRefresh phase of g_RuntimeMaintenanceCallbackPhases (only reached through that
   table): re-registers the owning army's terrain occupancy flags and refreshes the state tint of the model.
*/
void ArmyRuntimeMaintenance_InitializeOccupancyAndStateTint
          (WorldRuntimeContext *worldRuntime,ModelRuntimeNode *modelNodeRuntime)

{
  ArmyRuntime_InitializeTerrainOccupancyFlags
            (worldRuntime,
             (ArmyRuntimeSlot *)
             ((modelNodeRuntime->runtimePayload).armyRuntime)->linkedEntityRuntime);
  ModelNodeRuntime_UpdateStateTintRecursive(modelNodeRuntime);
  return;
}


/* Address: 0x0051D280.
   Army entry of the audioRefresh phase of g_RuntimeMaintenanceCallbackPhases (only reached through that table):
   runs the class sound callbacks (classMethodD) over the army's model hierarchy, starting at the runtime
   stored at +0x48 of the passed slot.
*/
void ArmyRuntimeMaintenance_DispatchClassMethodDRecursive
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ArmyRuntimeHierarchy_DispatchClassMethodDRecursive
            (worldRuntime,(ArmyRuntimeSlot *)armyRuntime->runtimeState48);
  return;
}


/* Address: 0x0051D2A0.
   Army entry of the primaryUpdate phase of g_RuntimeMaintenanceCallbackPhases (only reached through that table,
   once per simulation step and owner-list node): updates the army's model hierarchy, lets the AI pick targets
   for non-neutral factions, drops a timed-out target command, clears the LOCKED movement flag once nothing
   links to the model any more and counts down the timer at +0xA4.
*/
void ArmyRuntimeMaintenance_UpdateHierarchyAiAndTimers
          (WorldRuntimeContext *worldRuntime,WorldOwnerListNode100 *ownerNode)

{
  ArmyCommandGeneration *commandGenerationField;
  ModelRuntimeSlot *modelRuntime;
  ArmyRuntimeSlot *armyRuntime;
  int ownerFactionIndex;
  
  modelRuntime = ownerNode->runtimePayload;
  armyRuntime = (modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime;
  ownerFactionIndex = armyRuntime->factionIndex;
  ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive(worldRuntime,modelRuntime);
  if (ownerFactionIndex != 0) {
    AiCombatDecision_UpdateTargetAssignment(worldRuntime,armyRuntime);
  }
  commandGenerationField = &armyRuntime->commandGeneration;
  *commandGenerationField = *commandGenerationField - g_InGameSimulationStepTicks;
  if ((int)*commandGenerationField < 0) {
    armyRuntime->commandModeFlags = armyRuntime->commandModeFlags & 0xfffffff3;
  }
  if (((armyRuntime->movementStateFlags & ARMY_MOVEMENT_LOCKED) != 0) &&
     ((modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime == NULL)) {
    armyRuntime->movementStateFlags = armyRuntime->movementStateFlags & ~ARMY_MOVEMENT_LOCKED;
  }
  if (armyRuntime->runtimeStateA4 != 0) {
    armyRuntime->runtimeStateA4 = armyRuntime->runtimeStateA4 - 1;
  }
  return;
}


/* Address: 0x0051D6B0.
   Level start: allocates and zeroes the 0x48000-byte army runtime pool, loads the army graphics (texture set and
   palette, "<graphicsBasePath><suffix>.gfx/.pal") of slot 0 and of every existing faction, and renders the two
   panel preview textures of every army asset that has a selection panel entry. The movie schedule is ticked in between, since this
   runs behind the level-loading movie. Any load error is returned with CF set.
*/
ArmyRuntimeInitResult ArmyRuntime_InitializePoolAndGraphics(void *ownerContext,uint16_t *graphicsBasePath)

{
  uint16_t pathChar;
  uint32_t factionGraphicsVariant;
  ArmyAssetRecordPrefix *armyAsset;
  ArmyRuntimeSlot *armySlot1;
  GraphicsPaletteAsset *paletteOrResult;
  GraphicsTextureSourceAsset *textureSourceAsset;
  GraphicsTextureSet *loadedTextureSet;
  GraphicsPixelDimension previewHeight;
  int remainingCount;
  int factionSuffixChar;
  bool loadFactionGraphics;
  int frontendPlayerRuntimeId;
  ArmyAssetRecordPrefix **registryCursor;
  uint16_t *pathCursor;
  uint16_t *pathEnd;
  ArenaAllocResult allocResult;
  PackageLoadResult packageResult;
  TextureSetResult textureSetResult;
  ArmyRuntimeInitResult initResult;
  ArmyPreviewTextureResult previewResult;
  ArmyRuntimeInitResult finalResult;
  
  allocResult = g_MemoryApi.alloc(0x48000);
  armySlot1 = (ArmyRuntimeSlot *)allocResult.payloadOrError;
  if (!allocResult.failed) {
    /* base - 1 (MOV then DEC): the rebase value for saved offsets, see ArmyRuntimePool_RebaseAfterLoad */
    g_ArmyRuntimeRebaseBaseMinusOne = (void *)((int)&armySlot1[-1].selectionMetric5 + 3);
    g_ArmyRuntimeSlots = armySlot1;
    for (remainingCount = 0x12000; remainingCount != 0; remainingCount--) {
      (armySlot1->modelRuntimeOrSavedOffset).modelRuntime = (ModelRuntimeSlot *)0x0;
      armySlot1 = (ArmyRuntimeSlot *)&armySlot1->modelNodeRuntime;
    }
    /* find the end of graphicsBasePath (at most 32 code units); the suffix digit is written there */
    frontendPlayerRuntimeId = 0;
    remainingCount = 0x20;
    pathCursor = graphicsBasePath;
    do {
      pathEnd = pathCursor;
      if (remainingCount == 0) break;
      remainingCount--;
      pathEnd = pathCursor + 1;
      pathChar = *pathCursor;
      pathCursor = pathEnd;
    } while (pathChar != 0);
    remainingCount = ARMY_GRAPHICS_BINDING_COUNT;
    pathEnd = pathEnd - 1;
    g_MoviePlaybackScheduleSpan = 0x1a;
    do {
      MoviePlayback_AdvanceScheduledFrameAndTick();
      factionSuffixChar = '0';
      /* Slot 0 always loads the "0" graphics; the other slots load theirs (suffix 0-9/A-Z from the faction's
         graphics variant) only while the faction exists. */
      loadFactionGraphics = frontendPlayerRuntimeId == 0;
      if (!loadFactionGraphics) {
        paletteOrResult = (GraphicsPaletteAsset *)(frontendPlayerRuntimeId * 0x740);
        MoviePlayback_AdvanceScheduledFrameAndTick();
        if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[frontendPlayerRuntimeId] != 0) {
          /* records[frontendPlayerRuntimeId].factionClassOrMode (+0x38) */
          factionGraphicsVariant = *(uint32_t *)THANDOR_BYTE_AT(g_GameFactionRuntimeImage, frontendPlayerRuntimeId * 0x740 + 0x38);
          g_MoviePlaybackScheduleCounter--;
          if (factionGraphicsVariant < 10) {
            factionSuffixChar = factionGraphicsVariant + '0';
          }
          else {
            factionSuffixChar = factionGraphicsVariant + ('A' - 10);
          }
          loadFactionGraphics = true;
        }
      }
      if (loadFactionGraphics) {
        *(int *)pathEnd = factionSuffixChar; /* the suffix and a terminator in one dword */
        WidePath_SetExtensionCode(ASSET_MAGIC_GFX,graphicsBasePath);
        packageResult = Package_LoadEntry(graphicsBasePath);
        textureSourceAsset = packageResult.bufferOrError;
        if (packageResult.failed) {
          return THANDOR_BITCAST(PackageLoadResult, ArmyRuntimeInitResult, packageResult);
        }
        ArmyGraphics_CopyFrontendPlayerPaletteAndTexture
                  (frontendPlayerRuntimeId,(ArmyGraphicsAssetAddress32)textureSourceAsset);
        textureSetResult = g_GraphicsCreateTextureSet(textureSourceAsset);
        loadedTextureSet = textureSetResult.textureSet;
        if (textureSetResult.failed) {
          LOCK();
          UNLOCK();
          Resource_Release(textureSourceAsset);
          initResult.failed = true;
          initResult.errorOrValue = (uint32_t)loadedTextureSet;
          return initResult;
        }
        MoviePlayback_AdvanceScheduledFrameAndTick();
        g_ArmyGraphicsBindings[frontendPlayerRuntimeId].textureSet = loadedTextureSet;
        WidePath_SetExtensionCode(ASSET_MAGIC_PAL,graphicsBasePath);
        initResult = THANDOR_BITCAST(PaletteAssetResult, ArmyRuntimeInitResult, g_GraphicsPaletteAssetLoadPackage(graphicsBasePath));
        paletteOrResult = (GraphicsPaletteAsset *)initResult.errorOrValue;
        if (initResult.failed) {
          return initResult;
        }
        g_ArmyGraphicsBindings[frontendPlayerRuntimeId].paletteAsset = paletteOrResult;
      }
      frontendPlayerRuntimeId++;
      MoviePlayback_AdvanceScheduledFrameAndTick();
      remainingCount--;
    } while (remainingCount != 0);
    pathEnd[0] = 0;
    pathEnd[1] = 0;
    /* Preview textures for army assets with a non-zero selection detail variant (bits 1-7 of +0x14): the panel
       size one (subresource 34) into +0x1C, a third of the subresource-2 width into +0x18. */
    registryCursor = g_ArmyAssetRecordRegistry;
    remainingCount = ARMY_ASSET_REGISTRY_SLOT_COUNT;
    do {
      armyAsset = *registryCursor;
      if ((armyAsset != NULL) &&
         ((armyAsset[1].selectionDetailTemplateVariantIndex & 0xfe) != 0)) {
        previewResult = ArmyRuntime_RenderPreviewTexture
                           (g_InGamePanelTextureSubresource34Height,
                            g_InGamePanelTextureSubresource34Width,
                            *(FactionRuntimeIndex *)((int)ownerContext + 0x50),armyAsset->registryId,
                            ownerContext);
        paletteOrResult = (GraphicsPaletteAsset *)previewResult.previewTexture;
        if (!previewResult.failed) {
          armyAsset[1].rootNodeOffsetOrPointer = (uint32_t)paletteOrResult;
          previewHeight =
               (GraphicsPixelDimension)
               ((uint64_t)(int64_t)g_InGamePanelTextureSubresource02Width / 3);
          previewResult = ArmyRuntime_RenderPreviewTexture
                             (previewHeight,previewHeight,
                              *(FactionRuntimeIndex *)((int)ownerContext + 0x50),armyAsset->registryId,
                              ownerContext);
          paletteOrResult = (GraphicsPaletteAsset *)previewResult.previewTexture;
          if (!previewResult.failed) {
            armyAsset[1].registryId = (PckArmyAssetIdCatalog)paletteOrResult;
          }
        }
      }
      registryCursor++;
      remainingCount--;
    } while (remainingCount != 0);
    allocResult.failed = false;
    allocResult.payloadOrError = (uint32_t)paletteOrResult;
  }
  finalResult.errorOrValue = allocResult.payloadOrError;
  finalResult.failed = allocResult.failed;
  return finalResult;
}


/* Address: 0x00528330.
   Runtime update of class 12, reached only through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[12]:
   runs the emitters and moves the model vertically by the definition's step per tick, subtracting the step
   from the distance counter at +0x60; once that counter exceeds the model's own height (bounds Z1 - Z0) the
   whole model hierarchy is destroyed, e.g. a wreck that has sunk out of sight.
*/

void ArmyRuntimeClass_UpdateEffectsAndDestroyModelHierarchy
          (WorldRuntimeContext *worldRuntime,ModelRuntimeDestroyEffectsView200 *modelRuntime)

{
  ModelRuntimeNode *modelNodeRuntime;
  ModelResourceHitTestAndRenderView210 *rootModelResource;
  int verticalStepQ12;
  int modelHeightQ12;
  GraphicsWorldCoordinateQ12 *worldTranslationZQ12Field;
  int localBoundsZ1Q12;
  int remainingClassDistanceQ12;
  
  ArmyRuntime_UpdateTimedShotAndEffectEmitters
            (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
  modelNodeRuntime = modelRuntime->rootModelNode;
  rootModelResource = (modelNodeRuntime->modelPayload).modelResource;
  verticalStepQ12 =
       modelRuntime->modelDefinition->verticalTranslationStepQ12PerTick0C *
       g_InGameSimulationStepTicks;
  worldTranslationZQ12Field = &(modelNodeRuntime->worldTransform).translation.z;
  *worldTranslationZQ12Field = *worldTranslationZQ12Field + verticalStepQ12;
  localBoundsZ1Q12 = rootModelResource->localBoundsZ1Q12;
  (modelRuntime->classLinkState).modelLinkOrState60.modelRuntime =
       (ModelRuntimeSlot *)
       ((int)(modelRuntime->classLinkState).modelLinkOrState60.modelRuntime - verticalStepQ12);
  modelHeightQ12 = localBoundsZ1Q12 - rootModelResource->localBoundsZ0Q12;
  remainingClassDistanceQ12 = (modelRuntime->classLinkState).modelLinkOrState60.signedScalarState;
  modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
  if (modelHeightQ12 < remainingClassDistanceQ12) {
    ModelRuntimePool_DestroyHierarchyAndDetach(worldRuntime,(ModelRuntimeSlot *)modelRuntime);
  }
  else {
    ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
  }
  return;
}


/* Address: 0x00531130.
   World owner-list callback: for a model node, clears its runtime flags 0x4 and 0x8, re-registers the owning
   army's terrain occupancy and refreshes the node's state tint. Second pass after
   ArmyRuntimeNode_AccumulateTerrainOcclusionAndOccupancyCallback when a level's armies are set up.
*/
void ArmyRuntimeNode_RebuildTerrainOccupancyAndVisualStateCallback
          (WorldRuntimeContext *armyContext,WorldOwnerListNode100 *node)

{
  if (node->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
    node->runtimeFlags = node->runtimeFlags & 0xfffffff3;
    /* the army runtime at +8 of the node's payload */
    ArmyRuntime_InitializeTerrainOccupancyFlags
              (armyContext,*(ArmyRuntimeSlot **)((int)node->runtimePayload + 8));
    ModelNodeRuntime_RefreshStateTint((ModelRuntimeNode *)node);
  }
}


/* Address: 0x0051C3B0.
   Called by the weapon code (combat/movement) after a shot has been fired: stores the launch heading and the
   weapon definition's two post-launch values as the army's action vector, but only when both of those values
   are nonzero; otherwise the previous vector is kept.
*/
void ArmyRuntime_SetNonzeroActionVector
          (Q12 actionVector0,Q12 actionVector2,Q12 actionVector1,ArmyRuntimeSlot *armyRuntime)

{
  if ((actionVector1 != 0) && (actionVector2 != 0)) {
    armyRuntime->actionVector1Q12 = actionVector1;
    armyRuntime->actionVector0Q12 = actionVector0;
    armyRuntime->actionVector2Q12 = actionVector2;
  }
  return;
}


/* Address: 0x0051C540.
   Gives the army a new target army (NULL clears the command). A move started by target following is
   ended first; the command is stamped with the standard generation, or generation 0 when cleared.
*/
void ArmyRuntime_ResolveCommandTarget(ArmyRuntimeSlot *targetArmyRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ArmyCommandGeneration standardGeneration;

  standardGeneration = g_ArmyCommandGenerationStandard;
  if ((armyRuntime->movementStateFlags & ARMY_MOVEMENT_TARGET_FOLLOWING) != 0) {
    if ((armyRuntime->commandModeFlags & 8) == 0) {
      ArmyRuntime_ResetMovementStatePreserveQueuedTarget((ArmyMovementRuntime *)armyRuntime);
    }
    else {
      ArmyRuntime_ResetMovementStateFromCurrentPosition((ArmyMovementRuntime *)armyRuntime);
    }
  }
  if (targetArmyRuntime == NULL) {
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


/* Address: 0x0051C620.
   Gives the army a target position command (commandCoordinate0-2Q12): ends a move started by target
   following, drops any target army and stamps the standard command generation.
*/
void ArmyRuntime_ApplyTargetPositionCommand
          (Q12 coordinate2Q12,Q12 coordinate1Q12,Q12 coordinate0Q12,ArmyRuntimeSlot *armyRuntime)

{
  ArmyCommandGeneration commandGeneration;

  if ((armyRuntime->movementStateFlags & ARMY_MOVEMENT_TARGET_FOLLOWING) != 0) {
    if ((armyRuntime->commandModeFlags & 8) == 0) {
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
  armyRuntime->commandTargetArmyRuntime = NULL;
  armyRuntime->movementStateFlags = armyRuntime->movementStateFlags & ~ARMY_MOVEMENT_TARGET_FOLLOWING;
  armyRuntime->commandGeneration = commandGeneration;
  return;
}


/* Address: 0x0051C720.
   Resolves the world point a shooter at sourceWorld*Q12 aims its shot at: the explicit target position of the
   command, or the target entity's model (the flying body of an aircraft) raised by its definition's aim height.
   A moving target is led along its heading by the distance it covers during the shot's flight time, unless it
   stands still within that lead range. A target entity the shooter's faction can no longer see is dropped from
   the command. unresolved (CF) is set when there is nothing to aim at.
*/
WorldPositionResult
ArmyRuntime_ResolveShotAimPoint
          (Q12 sourceWorldZQ12,Q12 sourceWorldYQ12,Q12 sourceWorldXQ12,
          ShotDefinition *shotDefinition,GameEntityRuntime *targetState)

{
  int *targetDefinitionRecord;
  int targetClassRecord;
  int64_t deltaYSquared;
  int64_t remainingRangeSquared;
  uint32_t visibilityMask;
  uint32_t distanceOrAngle;
  int leadDistance;
  int definitionOrDelta;
  int aimWorldX;
  uint32_t directionY;
  int aimWorldY;
  int aimWorldZ;
  ModelRuntimeNode *targetNode;
  ShotRangeLimitResult rangeLimit;
  FixedDirection leadDirection;
  WorldPositionResult position;
  GameEntityRuntime *targetEntity;

  /* On failure (CF set) the original leaves whatever is in EAX/ECX/EDX at that point (the caller's values or the
     partial visibility mask / definition pointers). All three callers ignore the coordinates when CF is set, so
     the failure result carries zeros. */
  position.worldXQ12 = 0;
  position.worldYQ12 = 0;
  position.worldZQ12 = 0;
  position.unresolved = true;
  if (((targetState->common).commandTarget.targetFlags & 1) == 0) {
    if (((targetState->common).commandTarget.targetFlags & 2) != 0) {
      position.worldXQ12 = (targetState->common).commandTarget.targetWorldXQ12;
      position.worldYQ12 = (targetState->common).commandTarget.targetWorldYQ12;
      position.worldZQ12 = (targetState->common).commandTarget.targetWorldZQ12;
      position.unresolved = false;
    }
  }
  else {
    targetEntity = (targetState->common).commandTarget.targetEntity;
    if (targetEntity != NULL) {
      /* bit 1 of the shooter faction's 2-bit field: the target is visible to that faction */
      visibilityMask = 2u << ((uint8_t)((targetState->common).ownership.ownerIndex * 2) & 0x1f);
      targetNode = (targetEntity->common).ownership.modelNode;
      if ((*(uint32_t *)((targetEntity->common).damageState.reserved0C_23 + 0x10) & visibilityMask) != 0) {
        if (*(int *)(*(int *)(targetEntity->common).ownership.definitionOrClassRecord + 0x4c) == MODEL_RUNTIME_CLASS_21_AIRCRAFT) {
          targetNode = targetNode->childNodes[0];
        }
        aimWorldX = (targetNode->worldTransform).translation.x;
        targetDefinitionRecord = (targetEntity->common).ownership.definitionOrClassRecord;
        definitionOrDelta = *targetDefinitionRecord;
        aimWorldY = (targetNode->worldTransform).translation.y;
        aimWorldZ = (targetNode->worldTransform).translation.z + *(int *)(definitionOrDelta + 0x50);
        targetClassRecord = targetDefinitionRecord[2];
        if ((*(int *)(definitionOrDelta + 0x18) != 0) && ((*(uint32_t *)(targetClassRecord + 0x18) & 4) == 0)) {
          distanceOrAngle = FixedMath_Length3(aimWorldZ - sourceWorldZQ12,aimWorldY - sourceWorldYQ12,
                                    aimWorldX - sourceWorldXQ12);
          rangeLimit = ShotDefinition_GetModeRangeLimitEbx(shotDefinition);
          leadDistance = (int)(((int64_t)(int)distanceOrAngle * (int64_t)*(int *)(definitionOrDelta + 0xc)) /
                       (int64_t)rangeLimit.rangeLimitQ12);
          distanceOrAngle = ShotDefinition_ComputeMode3LeadAdjustment(shotDefinition);
          leadDistance = leadDistance + distanceOrAngle * *(int *)(definitionOrDelta + 0xc);
          distanceOrAngle = FixedMath_Atan2Angle16
                            (*(int *)(targetClassRecord + 0x5c) - *(int *)(*(int *)(targetClassRecord + 4) + 0x98),
                             *(int *)(targetClassRecord + 0x58) - *(int *)(*(int *)(targetClassRecord + 4) + 0x94));
          leadDirection = FixedMath_DirectionFromAnglesScaledRegs(0,distanceOrAngle,leadDistance);
          directionY = leadDirection.y;
          distanceOrAngle = leadDirection.x;
          targetEntity = (targetState->common).commandTarget.targetEntity;
          definitionOrDelta = (targetEntity->common).damageState.trackedCoordinate0Q12;
          if (definitionOrDelta == (targetEntity->common).pathCoordinate0Q12) {
            definitionOrDelta = definitionOrDelta - (targetNode->worldTransform).translation.x;
            remainingRangeSquared = ((int64_t)(int)directionY * (int64_t)(int)directionY +
                    (int64_t)(int)distanceOrAngle * (int64_t)(int)distanceOrAngle) - (int64_t)definitionOrDelta * (int64_t)definitionOrDelta
            ;
            if (((-1 < remainingRangeSquared) &&
                (definitionOrDelta = (targetEntity->common).damageState.trackedCoordinate1Q12,
                definitionOrDelta == (targetEntity->common).pathCoordinate1Q12)) &&
               (definitionOrDelta = definitionOrDelta - (targetNode->worldTransform).translation.y,
               deltaYSquared = (int64_t)definitionOrDelta * (int64_t)definitionOrDelta,
               -1 < remainingRangeSquared - deltaYSquared)) {
              /* The target is standing still within lead range: aim at its path position directly. */
              position.worldXQ12 = (targetEntity->common).pathCoordinate0Q12;
              position.worldYQ12 = (targetEntity->common).pathCoordinate1Q12;
              position.worldZQ12 =
                   (((targetEntity->common).ownership.modelNode)->worldTransform).translation.z +
                   *(int *)(*(int *)(targetEntity->common).ownership.definitionOrClassRecord + 0x50);
              position.unresolved = false;
              return position;
            }
          }
          aimWorldZ = leadDirection.z + aimWorldZ;
          aimWorldY = directionY + aimWorldY;
          aimWorldX = distanceOrAngle + aimWorldX;
        }
        position.worldXQ12 = aimWorldX;
        position.worldYQ12 = aimWorldY;
        position.worldZQ12 = aimWorldZ;
        position.unresolved = false;
        return position;
      }
      (targetState->common).commandTarget.targetEntity = NULL;
      (targetState->common).commandTarget.targetFlags = 0;
    }
  }
  return position;
}


/* Address: 0x0051D170.
   World owner-list callback: for a model node of an owned army, adds the army's projected terrain occlusion
   (its +0x9C mask in the byte of every faction whose nibble in the owner's packed relation states has bit 3
   set) around the node, and marks occupancy bit 2 around it when the active faction's nibble has bit 3 set. First pass of the occupancy rebuild; see
   ArmyRuntimeNode_RebuildTerrainOccupancyAndVisualStateCallback.
*/
void ArmyRuntimeNode_AccumulateTerrainOcclusionAndOccupancyCallback
          (WorldRuntimeContext *worldRuntime,WorldOwnerListNode100 *node)

{
  int armyRecord;
  uint32_t relationStates;
  int activeFactionIndex;
  uint64_t visibilityMask;
  uint64_t factionMaskByte;
  Q12 worldXQ12;
  Q12 worldYQ12;
  int ownerFactionIndex;
  FieldGridAsset *fieldGrid;
  
  if (node->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
    /* node->worldXQ12 (+0x94) goes to the callees' worldYQ12 and node->worldYQ12 (+0x98) to their worldXQ12,
       as in the original; one of the two namings is swapped. */
    worldYQ12 = node->worldXQ12;
    armyRecord = *(int *)((int)node->runtimePayload + 8);
    worldXQ12 = node->worldYQ12;
    /* armyRecord: the army runtime at +8 of the node payload; +0xC is its owner faction (0 = none) */
    if (*(int *)(armyRecord + 0xc) != 0) {
      ownerFactionIndex = *(int *)(armyRecord + 0xc);
      fieldGrid = worldRuntime->fieldGrid;
      relationStates = g_GameFactionRuntimeImage.records[ownerFactionIndex].packedRelationStates;
      factionMaskByte = (uint64_t)*(uint32_t *)(armyRecord + 0x9c);
      /* one nibble per faction in relationStates, one byte per faction in the 64-bit mask; faction 0 (the
         lowest byte) is not tested and stays zero */
      visibilityMask = 0;
      if ((relationStates & 0x80000000) != 0) {
        visibilityMask = factionMaskByte;
      }
      visibilityMask = visibilityMask << 8;
      if ((relationStates & 0x8000000) != 0) {
        visibilityMask = visibilityMask | factionMaskByte;
      }
      visibilityMask = visibilityMask << 8;
      if ((relationStates & 0x800000) != 0) {
        visibilityMask = visibilityMask | factionMaskByte;
      }
      visibilityMask = visibilityMask << 8;
      if ((relationStates & 0x80000) != 0) {
        visibilityMask = visibilityMask | factionMaskByte;
      }
      visibilityMask = visibilityMask << 8;
      if ((relationStates & 0x8000) != 0) {
        visibilityMask = visibilityMask | factionMaskByte;
      }
      visibilityMask = visibilityMask << 8;
      if ((relationStates & 0x800) != 0) {
        visibilityMask = visibilityMask | factionMaskByte;
      }
      activeFactionIndex = worldRuntime->activeFactionRuntimeIndex;
      visibilityMask = visibilityMask << 8;
      if ((relationStates & 0x80) != 0) {
        visibilityMask = visibilityMask | factionMaskByte;
      }
      TerrainProjectedOcclusion_AccumulateMaskAroundWorldPoint
                (visibilityMask << 8,*(FieldGridRadiusUnits *)(armyRecord + 0x44),
                 node->worldZQ12 + *(int *)(armyRecord + 0x48),worldXQ12,worldYQ12,
                 worldRuntime->fieldGrid);
      if ((relationStates >> ((uint8_t)(activeFactionIndex << 2) & 0x1f) & 8) != 0) {
        TerrainOccupancyBit2_MarkAroundWorldPoint
                  (*(FieldGridRadiusUnits *)(armyRecord + 0x90),worldXQ12,worldYQ12,ownerFactionIndex,
                   fieldGrid);
      }
    }
  }
}


/* Address: 0x0051D310.
   Tests the army's state/technology id at +0x100 for zero (CF set when it is zero, SETZ / RCR); used by
   ArmyRuntime_ResetMovementStateFromModel to decide whether a targeted command is dropped.
*/
bool ArmyRuntime_TestStateField100Zero(ArmyRuntimeSlot *armyRuntime)

{
  return armyRuntime->stateOrTechnologyId == 0;
}


/* Address: 0x0051D330.
   Tests the army's signed state/technology id at +0x100 for being non-negative (CF set when it is >= 0,
   SETGE / RCR). No C code calls it directly.
*/
bool ArmyRuntime_TestStateField100Nonnegative(ArmyRuntimeSlot *armyRuntime)

{
  return -1 < armyRuntime->stateOrTechnologyId;
}


/* Address: 0x0051D350.
   Runs the placement-validation handler of the army's runtime class (table at 0x0051FED8, indexed by the class id
   at model runtime +0x4C) for the army in *armyRuntimeHolder and returns its acceptance in CF.
*/
bool ArmyRuntimeNode_DispatchTypedCallback(ArmyRuntimeSlot **armyRuntimeHolder,WorldRuntimeContext *worldRuntime)

{
  bool accepted;

  /* the view's first field (modelDefinition) is the army's model runtime pointer */
  accepted = (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.placementValidation
            [((ModelRuntimePlacementValidationView200 *)*armyRuntimeHolder)->modelDefinition->
             runtimeClassId4C])
                    (worldRuntime,(ModelRuntimePlacementValidationView200 *)*armyRuntimeHolder);
  return accepted;
}


/* Address: 0x0051D4D0.
   Runs the class-command handler of the army's runtime class (indexed by the class id at model runtime +0x4C)
   for the army in *armyRuntimeHolder; the AI planners call it to start the class-specific behaviour of the
   armies they create or re-task.
*/
void ArmyRuntime_DispatchClassCommand(ArmyRuntimeSlot **armyRuntimeHolder,WorldRuntimeContext *worldRuntime)

{
  (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand
    [(((*armyRuntimeHolder)->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C])
            (worldRuntime,*armyRuntimeHolder);
  return;
}


/* Address: 0x0051D8C0.
   Counterpart of ArmyRuntime_InitializePoolAndGraphics: frees the army runtime pool, releases every faction's
   army texture set and palette, frees the two preview textures (+0x18/+0x1C) of every registered army asset
   and clears the asset registry.
*/
void ArmyRuntime_ShutdownPoolAndGraphics(void)

{
  ArmyAssetRecordPrefix *armyAsset;
  int remainingCount;
  ArmyGraphicsBinding *graphicsBindingCursor;
  ArmyAssetRecordPrefix **assetRegistryCursor;
  
  g_MemoryApi.free(g_ArmyRuntimeSlots);
  g_ArmyRuntimeSlots = NULL;
  graphicsBindingCursor = g_ArmyGraphicsBindings;
  remainingCount = ARMY_GRAPHICS_BINDING_COUNT;
  do {
    if (graphicsBindingCursor->textureSet != NULL) {
      g_GraphicsTextureSetReleasePackage(graphicsBindingCursor->textureSet);
      graphicsBindingCursor->textureSet = NULL;
    }
    if (graphicsBindingCursor->paletteAsset != NULL) {
      g_GraphicsPaletteAssetLifecycleCallbacks3.releasePackage
                (graphicsBindingCursor->paletteAsset);
      graphicsBindingCursor->paletteAsset = NULL;
    }
    graphicsBindingCursor++;
    remainingCount--;
  } while (remainingCount != 0);
  assetRegistryCursor = g_ArmyAssetRecordRegistry;
  remainingCount = ARMY_ASSET_REGISTRY_SLOT_COUNT;
  do {
    armyAsset = *assetRegistryCursor;
    if (armyAsset != NULL) {
      g_MemoryApi.free((void *)armyAsset[1].rootNodeOffsetOrPointer);
      g_MemoryApi.free((void *)armyAsset[1].registryId);
      *assetRegistryCursor = NULL;
    }
    assetRegistryCursor++;
    remainingCount--;
  } while (remainingCount != 0);
}


/* Address: 0x0051D960.
   Savegame writing (called by the in-game save in ui/ingame/runtime): turns the four pointers of every used
   army slot (model runtime, model node, command target, +0x98) into offsets and zeroes the unused slots, so the
   pool can be written as it is. Returns the pool base in EAX and its byte size 0x48000 in EDX;
   ArmyRuntimePool_RebaseAfterLoad is the counterpart.
*/
RuntimeImagePointerByteSizeEdxEax8 __cdecl ArmyRuntimePool_ConvertPointersToOffsetsForSaveRegs(void)

{
  uint32_t runtimeState98Offset;
  ModelRuntimeSlot *savedModelRuntimeOffset;
  int clearWordsRemaining;
  int slotsRemaining;
  ArmyRuntimeSlot *savedTargetOffset;
  ArmyRuntimeSlot *slotCursor;
  
  slotsRemaining = ARMY_RUNTIME_SLOT_COUNT;
  slotCursor = g_ArmyRuntimeSlots;
  do {
    while( true ) {
      savedTargetOffset = slotCursor->commandTargetArmyRuntime;
      if (slotCursor->modelNodeRuntime != NULL) break;
      /* an unused slot is zeroed dword by dword (0x48 dwords = 0x120 bytes), which also advances the cursor */
      for (clearWordsRemaining = 0x48; clearWordsRemaining != 0; clearWordsRemaining = clearWordsRemaining - 1) {
        (slotCursor->modelRuntimeOrSavedOffset).modelRuntime = NULL;
        slotCursor = (ArmyRuntimeSlot *)&slotCursor->modelNodeRuntime;
      }
      slotsRemaining = slotsRemaining - 1;
      if (slotsRemaining == 0) {
        /* EDX = pool byte size, EAX = pool base. */
        return (uint64_t)0x48000 << 32 | (uint32_t)g_ArmyRuntimeSlots;
      }
    }
    savedModelRuntimeOffset = (ModelRuntimeSlot *)
             ((int)(slotCursor->modelRuntimeOrSavedOffset).modelRuntime - g_ModelRuntimeRebaseDelta);
    if (savedTargetOffset != NULL) {
      savedTargetOffset = (ArmyRuntimeSlot *)((int)savedTargetOffset - (int)g_ArmyRuntimeRebaseBaseMinusOne);
    }
    slotCursor->modelNodeRuntime =
         (ModelRuntimeNode *)
         ((int)slotCursor->modelNodeRuntime - (int)g_RuntimeObjectRebaseBaseMinusOne);
    runtimeState98Offset = slotCursor->runtimeState98;
    (slotCursor->modelRuntimeOrSavedOffset).modelRuntime = savedModelRuntimeOffset;
    if (runtimeState98Offset != 0) {
      runtimeState98Offset = runtimeState98Offset - (int)g_ArmyRuntimeRebaseBaseMinusOne;
    }
    slotCursor->commandTargetArmyRuntime = savedTargetOffset;
    slotCursor->runtimeState98 = runtimeState98Offset;
    slotCursor = slotCursor + 1;
    slotsRemaining = slotsRemaining - 1;
  } while (slotsRemaining != 0);
  return (uint64_t)0x48000 << 32 | (uint32_t)g_ArmyRuntimeSlots;
}


/* Address: 0x0051D9F0.
   After a savegame load: turns the saved offsets in every used army slot (model node != 0) back into
   pointers, the counterpart of ArmyRuntimePool_ConvertPointersToOffsetsForSaveRegs. Model runtime (+0x00)
   and model node (+0x04) are rebased by their pools' deltas; the army references (+0x1C, +0x98) are saved
   as pointer - (pool base - 1), so 0 stays NULL.
*/
void ArmyRuntimePool_RebaseAfterLoad(void)

{
  uint32_t savedRuntimeState98Offset;
  void *rebasedModelRuntime;
  int runtimeSlotsRemaining;
  ArmyRuntimeSlot *rebasedCommandTarget;
  ArmyRuntimeSlot *runtimeSlotCursor;
  
  runtimeSlotsRemaining = ARMY_RUNTIME_SLOT_COUNT;
  runtimeSlotCursor = g_ArmyRuntimeSlots;
  do {
    if (runtimeSlotCursor->modelNodeRuntime != NULL) {
      /* Ghidra's field arithmetic: this is modelRuntime + g_ModelRuntimeRebaseDelta (ADD ECX,[0x005200BC]) */
      rebasedModelRuntime =
           ((runtimeSlotCursor->modelRuntimeOrSavedOffset).modelRuntime)->reserved10_37 +
           g_ModelRuntimeRebaseDelta + -0x10;
      rebasedCommandTarget = NULL;
      if (runtimeSlotCursor->commandTargetArmyRuntime != NULL) {
        rebasedCommandTarget =
             (ArmyRuntimeSlot *)
             ((int)&runtimeSlotCursor->commandTargetArmyRuntime->modelRuntimeOrSavedOffset +
             (int)g_ArmyRuntimeRebaseBaseMinusOne);
      }
      /* modelNodeRuntime + g_RuntimeObjectRebaseBaseMinusOne (ADD EAX,[0x00563700]) */
      runtimeSlotCursor->modelNodeRuntime =
           (ModelRuntimeNode *)
           (g_RuntimeObjectRebaseBaseMinusOne +
           (int)(&runtimeSlotCursor->modelNodeRuntime->modelPayload + -1) + 0x30);
      savedRuntimeState98Offset = runtimeSlotCursor->runtimeState98;
      (runtimeSlotCursor->modelRuntimeOrSavedOffset).modelRuntime = rebasedModelRuntime;
      if (savedRuntimeState98Offset != 0) {
        savedRuntimeState98Offset = savedRuntimeState98Offset + (int)g_ArmyRuntimeRebaseBaseMinusOne
        ;
      }
      runtimeSlotCursor->commandTargetArmyRuntime = rebasedCommandTarget;
      runtimeSlotCursor->runtimeState98 = savedRuntimeState98Offset;
    }
    runtimeSlotCursor++;
    runtimeSlotsRemaining--;
  } while (runtimeSlotsRemaining != 0);
}


/* Address: 0x00520CF0.
   Sound update of a moving ground army, reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes
   .classMethodD[1] (0x0051FCF8) and through ArmyRuntimeAudio_DispatchPositionedSoundVariant (classMethodD[18],
   every placement kind but water). While the army turns it keeps the turning sound (slot index +0x1D8 of the
   model) at the army's position; while it turns or drives it keeps the movement sound (+0x0D0) there; both only
   where the active faction's cell bits 0/1 are set (TerrainGrid_TestProjectedCellMaskBits01). Identical to
   variant B except for the reload below.
*/

void ArmyRuntimeClass_UpdatePositionedSoundsVariantA(WorldRuntimeContext *worldRuntime,
          ArmyRuntimeGroundMovementPositionedSoundView120 *armyRuntime)

{
  uint32_t soundSlotIndex;
  SpatialSoundSlot *soundSlot;
  GraphicsFixedVec3 *worldPosition;
  ModelRuntimeGroundMovementPositionedSoundView200 *soundModelRuntime;
  bool cellMasked;
  
  soundModelRuntime = armyRuntime->modelRuntime;
  if ((armyRuntime->movementControl).turnVelocityAngle16 == 0) {
    soundModelRuntime = armyRuntime->modelRuntime;
    if ((armyRuntime->movementControl).movementAdvancePerTickQ12 == 0) {
      return;
    }
  }
  else {
    soundSlotIndex = (soundModelRuntime->positionedSoundClassState84).positionedSoundSlotIndex1D8;
    if (((soundSlotIndex != 0) && (soundSlotIndex < worldRuntime->dwordArrayCount)) &&
       (worldRuntime->dwordArray != NULL)) {
      soundSlot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex];
      if (soundSlot != NULL) {
        worldPosition = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
        cellMasked = TerrainGrid_TestProjectedCellMaskBits01
                          ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,worldPosition->x,
                           worldRuntime);
        if (!cellMasked) {
          SpatialSound_UpdateDesiredPositionedGains
                    ((soundModelRuntime->positionedSoundLinkState60).positionedSoundMaximumDistanceQ12_7C,
                     (soundModelRuntime->positionedSoundLinkState60).positionedSoundGainQ15_78,worldPosition,
                     soundSlot);
        }
      }
    }
    /* Reload (the original skips this after a masked cell; nothing in between writes it). */
    soundModelRuntime = armyRuntime->modelRuntime;
  }
  soundSlotIndex = (soundModelRuntime->positionedSoundClassState84).positionedSoundSlotIndex0D0;
  if (((soundSlotIndex != 0) && (soundSlotIndex < worldRuntime->dwordArrayCount)) &&
     (worldRuntime->dwordArray != NULL)) {
    soundSlot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex];
    if (soundSlot != NULL) {
      worldPosition = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
      cellMasked = TerrainGrid_TestProjectedCellMaskBits01
                        ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,worldPosition->x,
                         worldRuntime);
      if (!cellMasked) {
        SpatialSound_UpdateDesiredPositionedGains
                  ((soundModelRuntime->positionedSoundLinkState60).positionedSoundMaximumDistanceQ12_7C,
                   (soundModelRuntime->positionedSoundLinkState60).positionedSoundGainQ15_78,worldPosition,soundSlot);
      }
    }
  }
  return;
}


/* Address: 0x00522B70.
   Empty sound update of class 3, reached only through
   g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classMethodD[3] (0x0051FCF8).
*/
void ArmyRuntimeClass_NoOpUpdate(WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  return;
}


/* Depth-first over a model runtime tree (count +0x0C, children +0x140 + 32*i, null slots skipped):
   the last node whose definition (+0x00) has class MODEL_RUNTIME_CLASS_10_CONTINUOUS_RADAR at +0x4C, or null.
   Part of ArmyRuntimeClass_SelectProjectileTargetNode (the original walks the tree inline). */
static uint8_t *ArmyRuntimeClass_FindLastClass10Node(uint8_t *node)
{
  uint8_t *found = NULL;
  int i;
  if (*(int *)(*(uint8_t **)node + 0x4c) == MODEL_RUNTIME_CLASS_10_CONTINUOUS_RADAR) {
    found = node;
  }
  for (i = 0; i < *(int *)(node + 0xc); i++) {
    uint8_t *child = *(uint8_t **)(node + 0x140 + i * 0x20);
    if (child != NULL) {
      uint8_t *match = ArmyRuntimeClass_FindLastClass10Node(child);
      if (match != NULL) {
        found = match;
      }
    }
  }
  return found;
}

/* Address: 0x00523E70.
   Owner-list callback of ArmyRuntimeClass_UpdateTimedTargetProjectilesAndEffects (passed to
   WorldRuntime_ForEachNodeInOwnerListD8). For a model of another, non-neutral faction within the shot's
   selection range it stores the last radar node (class 10) of that model as the target at +0x60, unless that
   node is destroyed; for a shot of the same shot definition it records that one is still in flight (+0x64).
*/

void ArmyRuntimeClass_SelectProjectileTargetNode(ModelRuntimeTimedTargetProjectileView200 *modelRuntime,
          WorldOwnerListNode100 *candidateNode)

{
  int64_t deltaYSquared;
  int64_t remainingRangeSquared;
  int deltaX;
  int candidateValue;
  ModelRuntimeSlot *targetModelRuntime;
  
  if (candidateNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
    candidateValue = ((modelRuntime->modelDefinition->shotDefinitionReference2C).definition)->
            mode2SelectionRangeQ12;
    deltaX = candidateNode->worldXQ12 - (modelRuntime->rootModelNode->worldTransform).translation.x;
    remainingRangeSquared = (int64_t)candidateValue * (int64_t)candidateValue - (int64_t)deltaX * (int64_t)deltaX;
    if ((((-1 < remainingRangeSquared) &&
         (candidateValue = candidateNode->worldYQ12 -
                  (modelRuntime->rootModelNode->worldTransform).translation.y,
         deltaYSquared = (int64_t)candidateValue * (int64_t)candidateValue,
         -1 < (int)(((int)((uint64_t)remainingRangeSquared >> 0x20) - (int)((uint64_t)deltaYSquared >> 0x20)) -
                   (uint32_t)((uint32_t)remainingRangeSquared < (uint32_t)deltaYSquared)))) &&
        (candidateValue = *(int *)(*(int *)((int)candidateNode->runtimePayload + 8) + 0xc),
        candidateValue != modelRuntime->ownerArmyRuntime->factionIndex)) && (candidateValue != 0)) {
      /* Rewritten from the assembly (0x00523F09-0x00523F7C): pick the last node, depth-first, whose
         definition has class 10 (+0x4C); the walk kept its frames on the machine stack. */
      targetModelRuntime = (ModelRuntimeSlot *)ArmyRuntimeClass_FindLastClass10Node((uint8_t *)candidateNode->runtimePayload);
      if ((targetModelRuntime != NULL) && (((targetModelRuntime->classState).classStateEC & 8) == 0)) {
        (modelRuntime->timedTargetLinkState).selectedTargetModelRuntime60 = targetModelRuntime;
      }
      return;
    }
  }
  else if ((candidateNode->ownerClassId == WORLD_OWNER_RUNTIME_SHOT) &&
          ((modelRuntime->modelDefinition->shotDefinitionReference2C).definition ==
           (((ModelRuntimeSlot *)candidateNode->runtimePayload)->definitionOrSavedId).definition)) {
    (modelRuntime->timedTargetLinkState).matchingActiveShotRuntime64 = candidateNode->runtimePayload
    ;
  }
  return;
}


/* Address: 0x00523FC0.
   Runtime update of class 20 (a launcher that fires at enemy radar), reached only through
   g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[20]. When its reload countdown has run out it
   shows the loaded missile (mesh group bit 0) and scans the world with ArmyRuntimeClass_SelectProjectileTargetNode;
   if a radar target is in range and none of its own shots is still in flight, it hides the missile, restarts the
   reload and fires from its attachment points at the target's position.
*/

void ArmyRuntimeClass_UpdateTimedTargetProjectilesAndEffects
          (WorldRuntimeContext *worldRuntime,ModelRuntimeTimedTargetProjectileView200 *modelRuntime)

{
  ModelMeshGroupMask *meshMaskField;
  int *reloadCountdown;
  ModelDefinitionTimedTargetProjectileView280 *timedTargetDefinition;
  ModelRuntimeNode *rootNode;
  ModelRuntimeSlot *selectedTarget;
  Q12 targetWorldXQ12;
  Q12 targetWorldYQ12;
  Q12 targetWorldZQ12;
  int reloadCountdownTicks;
  
  if (((modelRuntime->classState).classStateEC & 9) == 0) {
    timedTargetDefinition = modelRuntime->modelDefinition;
    reloadCountdownTicks = (modelRuntime->timedTargetState).targetProjectileReloadCountdownTicks24 -
            g_InGameSimulationStepTicks;
    rootNode = modelRuntime->rootModelNode;
    (modelRuntime->timedTargetState).targetProjectileReloadCountdownTicks24 = reloadCountdownTicks;
    if (reloadCountdownTicks < 1) {
      (modelRuntime->timedTargetState).targetProjectileReloadCountdownTicks24 = 0;
      meshMaskField = &(rootNode->modelPayload).meshGroupMask;
      *meshMaskField = *meshMaskField | 1;
      (modelRuntime->timedTargetLinkState).selectedTargetModelRuntime60 = NULL;
      (modelRuntime->timedTargetLinkState).matchingActiveShotRuntime64 = NULL;
      WorldRuntime_ForEachNodeInOwnerListD8
                (modelRuntime,ArmyRuntimeClass_SelectProjectileTargetNode,worldRuntime);
      selectedTarget = (modelRuntime->timedTargetLinkState).selectedTargetModelRuntime60;
      if (((modelRuntime->timedTargetLinkState).matchingActiveShotRuntime64 ==
           NULL) && (selectedTarget != NULL)) {
        rootNode = (selectedTarget->rootModelNodeOrSavedOffset).modelNode;
        targetWorldXQ12 = (rootNode->worldTransform).translation.x;
        targetWorldYQ12 = (rootNode->worldTransform).translation.y;
        targetWorldZQ12 = (rootNode->worldTransform).translation.z;
        rootNode = modelRuntime->rootModelNode;
        reloadCountdown = &(modelRuntime->timedTargetState).targetProjectileReloadCountdownTicks24;
        *reloadCountdown = *reloadCountdown + (timedTargetDefinition->timedTargetParameters30).reloadTicks30;
        rootNode->runtimeFlags = rootNode->runtimeFlags | 1;
        meshMaskField = &(rootNode->modelPayload).meshGroupMask;
        *meshMaskField = *meshMaskField & 0xfffffffe;
        ModelRuntime_EmitProjectilesFromAttachmentPoints
                  ((ShotRuntimeState14)
                   (modelRuntime->timedTargetLinkState).selectedTargetModelRuntime60,targetWorldZQ12
                   ,targetWorldYQ12,targetWorldXQ12,(timedTargetDefinition->shotDefinitionReference2C).definition,
                   rootNode,(MdlSerializedNodeHeader38 *)timedTargetDefinition->serializedNodeOffsetOrPointer64,
                   worldRuntime);
      }
    }
  }
  ModelNodeRuntime_RebuildTransformsFromRoot(modelRuntime->rootModelNode);
  (modelRuntime->timedTargetLinkState).selectedTargetModelRuntime60 = NULL;
  (modelRuntime->timedTargetLinkState).matchingActiveShotRuntime64 = NULL;
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  return;
}


/* Address: 0x00525960.
   Sound update of a moving army on water, reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes
   .classMethodD[19] (0x0051FCF8) and through ArmyRuntimeAudio_DispatchPositionedSoundVariant (classMethodD[18],
   placement kind 1 = water surface). Same as ArmyRuntimeClass_UpdatePositionedSoundsVariantA: the turning sound
   (+0x1D8) while turning, the movement sound (+0x0D0) while turning or driving, only where the active faction's
   cell bits 0/1 are set.
*/

void ArmyRuntimeClass_UpdatePositionedSoundsVariantB(WorldRuntimeContext *worldRuntime,
          ArmyRuntimeGroundMovementPositionedSoundView120 *armyRuntime)

{
  uint32_t soundSlotIndex;
  SpatialSoundSlot *soundSlot;
  GraphicsFixedVec3 *worldPosition;
  ModelRuntimeGroundMovementPositionedSoundView200 *soundModelRuntime;
  bool cellMasked;
  
  soundModelRuntime = armyRuntime->modelRuntime;
  if ((armyRuntime->movementControl).turnVelocityAngle16 == 0) {
    soundModelRuntime = armyRuntime->modelRuntime;
    if ((armyRuntime->movementControl).movementAdvancePerTickQ12 == 0) {
      return;
    }
  }
  else {
    soundSlotIndex = (soundModelRuntime->positionedSoundClassState84).positionedSoundSlotIndex1D8;
    if (((soundSlotIndex != 0) && (soundSlotIndex < worldRuntime->dwordArrayCount)) &&
       (worldRuntime->dwordArray != NULL)) {
      soundSlot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex];
      if (soundSlot != NULL) {
        worldPosition = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
        cellMasked = TerrainGrid_TestProjectedCellMaskBits01
                          ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,worldPosition->x,
                           worldRuntime);
        if (!cellMasked) {
          SpatialSound_UpdateDesiredPositionedGains
                    ((soundModelRuntime->positionedSoundLinkState60).positionedSoundMaximumDistanceQ12_7C,
                     (soundModelRuntime->positionedSoundLinkState60).positionedSoundGainQ15_78,worldPosition,soundSlot);
        }
      }
    }
  }
  soundSlotIndex = (soundModelRuntime->positionedSoundClassState84).positionedSoundSlotIndex0D0;
  if (((soundSlotIndex != 0) && (soundSlotIndex < worldRuntime->dwordArrayCount)) &&
     (worldRuntime->dwordArray != NULL)) {
    soundSlot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex];
    if (soundSlot != NULL) {
      worldPosition = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
      cellMasked = TerrainGrid_TestProjectedCellMaskBits01
                        ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,worldPosition->x,
                         worldRuntime);
      if (!cellMasked) {
        SpatialSound_UpdateDesiredPositionedGains
                  ((soundModelRuntime->positionedSoundLinkState60).positionedSoundMaximumDistanceQ12_7C,
                   (soundModelRuntime->positionedSoundLinkState60).positionedSoundGainQ15_78,worldPosition,soundSlot);
      }
    }
  }
  return;
}


/* Address: 0x00526FE0.
   Segment meter of the selection panel (called directly by gameplay/selection/runtime with a model runtime):
   filled segments from +0x6C of the passed runtime (the completed linked assets of a class-22 pad), total
   segments from +0xC4 of its definition (the linked-child slot capacity). Returned in EBX/ECX.
   Original register convention: result in EBX and ECX; EAX and EDX preserved.
*/
ArmySegmentMeter ArmyRuntime_QueryMetric6CAndDefinitionC4Regs(ArmyRuntimeSlot *armyRuntime)

{
  ArmySegmentMeter metricRegs;
  
  metricRegs.totalSegments = *(uint32_t *)(((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->classState).
                        reservedC4_C7;
  metricRegs.filledSegments = (uint32_t)armyRuntime->linkedArmyRuntimeOrSavedOffset;
  return metricRegs;
}


/* Address: 0x00527150.
   Returns in EBX which of the three linked-child asset ids (g_ArmyLinkedChildAssetIdSlot0/1/2 as bits 1/2/4)
   occur among the army's 13 attachment asset-id slots (dwords from +0x78); the selection panel ORs these
   masks over all selected armies.
*/
int ArmyRuntime_AccumulateAttachmentEffectVariantMaskRegs(ArmyRuntimeSlot *armyRuntime)

{
  int attachmentAssetId;
  int attachmentEffectSlotsRemaining;
  uint32_t variantMask;

  variantMask = 0;
  attachmentEffectSlotsRemaining = 13;
  do {
    /* +0x78 of the current window; armyRuntime is moved one dword per slot below */
    attachmentAssetId = armyRuntime->movementTarget0Q12;
    if (attachmentAssetId == g_ArmyLinkedChildAssetIdSlot0) {
      variantMask = variantMask | 1;
    }
    if (attachmentAssetId == g_ArmyLinkedChildAssetIdSlot1) {
      variantMask = variantMask | 2;
    }
    if (attachmentAssetId == g_ArmyLinkedChildAssetIdSlot2) {
      variantMask = variantMask | 4;
    }
    armyRuntime = (ArmyRuntimeSlot *)&armyRuntime->modelNodeRuntime; /* + 4 bytes (ADD ESI,4) */
    attachmentEffectSlotsRemaining--;
  } while (attachmentEffectSlotsRemaining != 0);
  return variantMask;
}


/* Address: 0x00527FE0.
   Keeps the army's looping sound (slot index at +0x1AC of its model runtime) at the army's position while
   runtime flag 1 is clear and the active faction's cell bits 0/1 are set there. Reached through
   g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classMethodD[10], [14] and [16] (0x0051FCF8) and through
   ArmyRuntimeAudio_UpdateLoopingSoundWhenEnabled (classMethodD[4]).
*/
void ArmyRuntime_UpdateLoopingPositionedSound(WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  GraphicsFixedVec3 *worldPosition;
  ModelRuntimeSlot *linkedModelRuntime;
  uint32_t soundSlotIndex;
  SpatialSoundSlot *slot;
  bool cellMasked;
  
  linkedModelRuntime = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  soundSlotIndex = linkedModelRuntime->attachments140[3].childNodeIndex0C;
  if (((((armyRuntime->runtimeFlags & 1) == 0) && (soundSlotIndex != 0)) &&
      (soundSlotIndex < worldRuntime->dwordArrayCount)) &&
     ((worldRuntime->dwordArray != NULL &&
      (slot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex], slot != NULL))
     )) {
    worldPosition = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
    cellMasked = TerrainGrid_TestProjectedCellMaskBits01
                      ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,
                       worldPosition->x,worldRuntime);
    if (!cellMasked) {
      SpatialSound_UpdateDesiredPositionedGains
                ((linkedModelRuntime->classLinkState).classState7C,(linkedModelRuntime->classLinkState).classState78,
                 worldPosition,slot);
    }
  }
  return;
}


/* Address: 0x005283D0.
   Runtime update of class 23 (a platform that armies of its faction can dock on, see
   ArmyRuntime_HandleCollisionPartner), reached only through
   g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[23]. While an army is linked (bit 0 of +0xB8,
   re-checked against the army's expanded radius every 8 ticks) the child platform node moves by the step per
   tick until the travel at +0x60 reaches the definition's limit; without a linked army it moves back until the
   travel is 0. A positioned one-shot sound marks the start of either movement.
*/

void ArmyRuntimeClass_UpdateVerticalDeploymentAndCollisionState
          (WorldRuntimeContext *worldRuntime,ModelRuntimeVerticalDeploymentView200 *modelRuntime)

{
  uint32_t *classStateField;
  Q12 *childTranslationZ;
  ModelDefinitionVerticalDeploymentView280 *deploymentDefinition;
  ArmyRuntimeSlot *linkedArmyRuntime;
  uint32_t soundAssetIndex;
  DirectSoundVoiceSet **soundVoiceSet;
  int travelLimitOrStep;
  bool testResult;
  ModelRuntimeNode *modelNode;
  
  deploymentDefinition = modelRuntime->modelDefinition;
  modelNode = modelRuntime->rootModelNode;
  linkedArmyRuntime = (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime;
  travelLimitOrStep = deploymentDefinition->deploymentTravelLimitQ12;
  if (((modelRuntime->classState).classStateB8 & 1) == 0) {
    if ((modelRuntime->deploymentState60).deploymentTravelQ12_60 != 0) {
      if ((modelRuntime->deploymentState60).deploymentTravelQ12_60 <= travelLimitOrStep) {
        soundAssetIndex = deploymentDefinition->deploymentSoundAssetIndex26C;
        if (((soundAssetIndex != 0) && (soundAssetIndex < worldRuntime->dwordArrayCount)) &&
           (worldRuntime->dwordArray != NULL)) {
          soundVoiceSet = (DirectSoundVoiceSet **)worldRuntime->dwordArray[soundAssetIndex];
          if (soundVoiceSet != NULL) {
            testResult = TerrainGrid_TestProjectedCellMaskBits01
                              ((modelNode->worldTransform).translation.y,
                               (modelNode->worldTransform).translation.x,worldRuntime);
            if (!testResult) {
              SpatialSound_PlayPositionedOneShot
                        (deploymentDefinition->positionedSoundMaximumDistanceQ12,deploymentDefinition->positionedSoundGainQ15,
                         &(modelNode->worldTransform).translation,soundVoiceSet);
            }
          }
        }
      }
      modelNode = modelNode->childNodes[0];
      travelLimitOrStep = deploymentDefinition->verticalDeploymentStepQ12PerTick;
      classStateField = &(modelRuntime->classState).classStateB8;
      *classStateField = *classStateField & 0xfffffffd;
      travelLimitOrStep = travelLimitOrStep * g_InGameSimulationStepTicks;
      if (modelNode != NULL) {
        childTranslationZ = &(modelNode->modelPayload).localTranslationZQ12;
        *childTranslationZ = *childTranslationZ + travelLimitOrStep;
        (modelRuntime->deploymentState60).deploymentTravelQ12_60 =
             (modelRuntime->deploymentState60).deploymentTravelQ12_60 + travelLimitOrStep;
        ModelNodeRuntime_RebuildTransformsFromRoot(modelRuntime->rootModelNode);
      }
    }
  }
  else {
    if ((modelRuntime->deploymentState60).collisionRetryCountdown64 != 0) {
      classStateField = &(modelRuntime->deploymentState60).collisionRetryCountdown64;
      *classStateField = *classStateField - 1;
    }
    classStateField = &(modelRuntime->classState).classStateB8;
    *classStateField = *classStateField | 2;
    if (((modelRuntime->deploymentState60).collisionRetryCountdown64 == 0) &&
       (classStateField = &(modelRuntime->classState).classStateB8, *classStateField = *classStateField & 0xfffffffe,
       linkedArmyRuntime != NULL)) {
      (modelRuntime->deploymentState60).collisionRetryCountdown64 = 8;
      (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime = NULL;
      testResult = ArmyCollision_TestPointWithinExpandedRuntimeRadius
                        (deploymentDefinition->placementRadiusOrClearanceDC,
                         (modelNode->worldTransform).translation.y,
                         (modelNode->worldTransform).translation.x,linkedArmyRuntime);
      if (testResult) {
        (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime = linkedArmyRuntime;
        classStateField = &(modelRuntime->classState).classStateB8;
        *classStateField = *classStateField | 1;
      }
    }
    if (travelLimitOrStep < (modelRuntime->deploymentState60).deploymentTravelQ12_60) {
      classStateField = &(modelRuntime->classState).classStateB8;
      *classStateField = *classStateField & 0xfffffffd;
      if ((modelRuntime->deploymentState60).deploymentTravelQ12_60 == 0) {
        soundAssetIndex = deploymentDefinition->deploymentSoundAssetIndex26C;
        if (((soundAssetIndex != 0) && (soundAssetIndex < worldRuntime->dwordArrayCount)) &&
           (worldRuntime->dwordArray != NULL)) {
          soundVoiceSet = (DirectSoundVoiceSet **)worldRuntime->dwordArray[soundAssetIndex];
          if (soundVoiceSet != NULL) {
            testResult = TerrainGrid_TestProjectedCellMaskBits01
                              ((modelNode->worldTransform).translation.y,
                               (modelNode->worldTransform).translation.x,worldRuntime);
            if (!testResult) {
              SpatialSound_PlayPositionedOneShot
                        (deploymentDefinition->positionedSoundMaximumDistanceQ12,deploymentDefinition->positionedSoundGainQ15,
                         &(modelNode->worldTransform).translation,soundVoiceSet);
            }
          }
        }
      }
      if (modelNode->childNodes[0] != NULL) {
        travelLimitOrStep = deploymentDefinition->verticalDeploymentStepQ12PerTick * g_InGameSimulationStepTicks;
        childTranslationZ = &(modelNode->childNodes[0]->modelPayload).localTranslationZQ12;
        *childTranslationZ = *childTranslationZ - travelLimitOrStep;
        (modelRuntime->deploymentState60).deploymentTravelQ12_60 =
             (modelRuntime->deploymentState60).deploymentTravelQ12_60 - travelLimitOrStep;
        ModelNodeRuntime_RebuildTransformsFromRoot(modelRuntime->rootModelNode);
      }
    }
  }
  return;
}


/* Address: 0x00529720.
   Fires a shot of the weapon code (called directly by gameplay/army/combat): looks up the launch point
   (packed key attachmentSelectorOrdinal << 4 | 2) in the sprite model of definitionNode, transforms it by the
   freshly rebuilt modelNode and creates the projectile from there towards the target point. Returns true (CF
   set) when the model has no such launch point.
*/
bool ArmyRuntime_ResolveShotLaunchFromModelAttachment
          (ShotRuntimeState14 shotRuntimeState14,Q12 targetWorldXQ12,Q12 targetWorldYQ12,
          Q12 targetWorldZQ12,SprAttachmentSelectorOrdinal attachmentSelectorOrdinal,
          ShotDefinition *shotDefinition,ModelRuntimeNode *modelNode,
          MdlSerializedNodeHeader38 *definitionNode,WorldRuntimeContext *worldRuntime)

{
  ModelResourceHitTestAndRenderView210 *spriteModelResource;
  Q12 launchWorldYQ12;
  Q12 launchWorldZQ12;
  ModelPackedLookupTableEntryCount remainingEntries;
  ModelPackedPointRecord *localPointRecord;
  ModelWorldPoint launchPoint;
  
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNode);
  spriteModelResource = (definitionNode->spriteAssetReference).modelResource;
  remainingEntries = spriteModelResource->packedLookupTableEntryCount;
  localPointRecord =
       (ModelPackedPointRecord *)(spriteModelResource->reserved00_AF + spriteModelResource->packedLookupTableRelativeOffset);
  while( true ) {
    if (remainingEntries == 0) {
      return true;
    }
    if (localPointRecord->packedLookupKey == (attachmentSelectorOrdinal << 4 | 2)) break;
    localPointRecord = localPointRecord + 1;
    remainingEntries = remainingEntries - 1;
  }
  launchPoint = ModelNodeRuntime_TransformLocalPointRegs(localPointRecord,modelNode);
  launchWorldZQ12 = launchPoint.zQ12;
  launchWorldYQ12 = launchPoint.yQ12;
  ShotRuntimePool_CreateProjectileFromDefinition
            (shotRuntimeState14,
             (ArmyRuntimeSlot *)((modelNode->runtimePayload).armyRuntime)->linkedEntityRuntime,
             targetWorldXQ12,targetWorldYQ12,targetWorldZQ12,launchWorldZQ12,launchWorldYQ12,
             launchPoint.xQ12,shotDefinition,worldRuntime);
  return false;
}


/* Address: 0x00529B50.
   Accelerates a ground army (called directly by gameplay/army/movement): the speed limit is +0x0C of the model
   runtime. While the pitch (worldRotationAngle1) is below the first class threshold it is cut to 5/16, unless
   the pitch is at least the second threshold: then it stays full, or 5/8 when angle2 - angle0 lies between a
   quarter and three quarters of a turn. The advance per tick grows by +0x08 of the class data up to that limit (and drops to it at once). When
   the army starts from standstill its start sound plays where the active faction's cell bits 0/1 are set.
*/
void ArmyRuntime_UpdateActivationMetricAndPlayStartSound(WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  GraphicsFixedVec3 *worldPosition;
  int previousAdvance;
  ModelRuntimeSlot *linkedModelRuntime;
  AngleTurn32 rotationAngle1;
  DirectSoundVoiceSet **voiceSetRef;
  uint32_t angleOrAdvance;
  uint32_t advanceOrSoundIndex;
  bool cellMasked;
  ModelRuntimeNode *rootNode;
  
  rootNode = armyRuntime->modelNodeRuntime;
  linkedModelRuntime = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  rotationAngle1 = (rootNode->modelPayload).worldRotationAngle1;
  advanceOrSoundIndex = linkedModelRuntime->attachmentCount0C;
  if ((int)rotationAngle1 < (int)linkedModelRuntime[1].classLinkState.classState68) {
    angleOrAdvance = (rootNode->modelPayload).worldRotationAngle2 -
            (rootNode->modelPayload).worldRotationAngle0 & 0xffff;
    advanceOrSoundIndex = advanceOrSoundIndex * 5 >> 4;
    if (((*(int *)(linkedModelRuntime->reserved10_37 + 0x14) <= (int)rotationAngle1) &&
        (advanceOrSoundIndex = linkedModelRuntime->attachmentCount0C, FIXED_ANGLE16_QUARTER_TURN < angleOrAdvance)) &&
        (angleOrAdvance < 3 * FIXED_ANGLE16_QUARTER_TURN)) {
      advanceOrSoundIndex = advanceOrSoundIndex * 5 >> 3;
    }
  }
  angleOrAdvance = (armyRuntime->movementControl).movementAdvancePerTickQ12;
  if ((angleOrAdvance < advanceOrSoundIndex) && (angleOrAdvance = angleOrAdvance + *(int *)(linkedModelRuntime->reserved10_37 + 8), angleOrAdvance < advanceOrSoundIndex)) {
    advanceOrSoundIndex = angleOrAdvance;
  }
  /* XCHG in the original */
  LOCK();
  previousAdvance = (armyRuntime->movementControl).movementAdvancePerTickQ12;
  (armyRuntime->movementControl).movementAdvancePerTickQ12 = advanceOrSoundIndex;
  UNLOCK();
  if ((previousAdvance == 0) && (advanceOrSoundIndex != 0)) {
    advanceOrSoundIndex = *(uint32_t *)(linkedModelRuntime->classState).reservedD4_DB;
    if ((advanceOrSoundIndex != 0) &&
       ((advanceOrSoundIndex < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != NULL)))) {
      voiceSetRef = (DirectSoundVoiceSet **)worldRuntime->dwordArray[advanceOrSoundIndex];
      worldPosition = &(rootNode->worldTransform).translation;
      if (voiceSetRef != NULL) {
        cellMasked = TerrainGrid_TestProjectedCellMaskBits01
                          ((rootNode->worldTransform).translation.y,worldPosition->x,worldRuntime)
        ;
        if (!cellMasked) {
          SpatialSound_PlayPositionedOneShot
                    ((linkedModelRuntime->classLinkState).classState7C,(linkedModelRuntime->classLinkState).classState78,
                     worldPosition,voiceSetRef);
        }
      }
    }
  }
  return;
}


/* Address: 0x0052A040.
   Reacts to the army a moving army has run into (called directly by the movement code in gameplay/army/movement
   with the mover's position, Y before X). A free class-23 platform of the same faction is told to dock
   (contact flag 1, timer 0x20) and, once it is ready (flag 2), both armies are linked to each other; an army of
   class 0 is run over and takes impact damage 0x100000 from the direction of the collision.
*/
void ArmyRuntime_HandleCollisionPartner(ArmyRuntimeSlot *currentArmyRuntime,Q12 currentWorldYQ12,Q12 currentWorldXQ12,
          ArmyRuntimeSlot *collisionPartnerArmyRuntime,WorldRuntimeContext *worldRuntime)

{
  Q12 *fallbackPositionField;
  uint32_t impactAngle;
  
  if (collisionPartnerArmyRuntime != NULL) {
    if (((collisionPartnerArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
        definitionValue9C_4C == MODEL_RUNTIME_CLASS_23) {
      if (((collisionPartnerArmyRuntime->linkedEntityRuntime->common).ownership.ownerIndex ==
           (currentArmyRuntime->linkedEntityRuntime->common).ownership.ownerIndex) &&
         (collisionPartnerArmyRuntime->linkedArmyRuntime == NULL)) {
        fallbackPositionField = &(collisionPartnerArmyRuntime->articulatedContact).fallbackPosition0Q12;
        *fallbackPositionField = *fallbackPositionField | 1;
        collisionPartnerArmyRuntime->ownerValue64 = 0x20;
        if (((collisionPartnerArmyRuntime->articulatedContact).fallbackPosition0Q12 & 2U) != 0) {
          collisionPartnerArmyRuntime->linkedArmyRuntime = currentArmyRuntime;
          currentArmyRuntime->linkedArmyRuntime = collisionPartnerArmyRuntime;
        }
      }
    }
    else if (((collisionPartnerArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
             definitionValue9C_4C == MODEL_RUNTIME_CLASS_00) {
      impactAngle = FixedMath_Atan2Angle16
                              ((collisionPartnerArmyRuntime->modelNodeRuntime->worldTransform).
                               translation.y - currentWorldYQ12,
                               (collisionPartnerArmyRuntime->modelNodeRuntime->worldTransform).
                               translation.x - currentWorldXQ12);
      ArmyRuntime_ApplyImpactDamageAndFinalizeState
                (impactAngle,0x100000,collisionPartnerArmyRuntime);
    }
  }
  return;
}


/* Address: 0x0051BC00.
   Renders the picture of an army type for the in-game panels: spawns a temporary army of armyAssetId for
   factionIndex, turns it to a fixed three-quarter view, frames its bounds and renders it off screen at twice the
   requested size, then destroys the army and downsamples the image 2x2 -> 1 with alpha weighting (MMX) into a
   previewWidth x previewHeight texture. Returns the texture, or CF set with the creation/render error.
*/
/* One 16-bit MMX lane per pixel byte: PUNPCKLBW mm,mm duplicates each byte into a word, PSRLW 4 scales it. */
#define ARMY_PREVIEW_UNPACK_BYTE_LANE(pixel, byteIndex) \
  ((uint64_t)((((pixel) >> ((byteIndex) * 8)) & 0xffu) * 0x101u >> 4) << ((byteIndex) * 16))
#define ARMY_PREVIEW_UNPACK_PIXEL_LANES(pixel) \
  (ARMY_PREVIEW_UNPACK_BYTE_LANE(pixel, 3) | ARMY_PREVIEW_UNPACK_BYTE_LANE(pixel, 2) | \
   ARMY_PREVIEW_UNPACK_BYTE_LANE(pixel, 1) | ARMY_PREVIEW_UNPACK_BYTE_LANE(pixel, 0))

ArmyPreviewTextureResult ArmyRuntime_RenderPreviewTexture
          (GraphicsPixelDimension previewHeight,GraphicsPixelDimension previewWidth,
          FactionRuntimeIndex factionIndex,PckArmyAssetIdCatalog armyAssetId,
          WorldRuntimeContext *worldRuntime)

{
  uint64_t alphaReciprocal;
  ModelRuntimeNode *rootNodeOrSize;
  uint32_t pixelTopLeft;
  uint32_t pixelTopRight;
  uint32_t pixelBottomLeft;
  uint32_t pixelBottomRight;
  GraphicsPixelDimension savedPreviewWidth;
  GameEntityRuntime *previewArmyOrValue;
  int boundsSpanY;
  GameEntityRuntime *previewTexture;
  void *halvedWidth;
  int boundsSpanZ;
  int maxBoundsSpan;
  Q12 *sourcePixels;
  Q12 *destinationPixels;
  uint16_t topLeftAlphaOrSum0;
  uint16_t topRightAlphaOrClamp0;
  uint16_t bottomLeftAlphaOrSum1;
  uint16_t bottomRightAlphaOrClamp1;
  uint16_t channelSum2;
  uint16_t channelClamp2;
  uint64_t mm0PackedValue0;
  uint16_t channelSum3;
  uint16_t channelClamp3;
  uint64_t mm1PackedValue0;
  uint64_t mm2PackedValue0;
  uint64_t mm3PackedValue0;
  ArmyRuntimeCreateResult createResult;
  OffscreenRenderResult offscreenResult;
  ArmyPreviewTextureResult successResult;
  ArmyPreviewTextureResult failureResult;
  
  savedPreviewWidth = previewWidth;
  /* a temporary army at world position (0x6000000, 0x6000000) Q12 */
  createResult = ArmyRuntime_CreateInstanceFromAsset
                     (1,0,0x6000000,0x6000000,factionIndex,armyAssetId,worldRuntime);
  previewArmyOrValue = (GameEntityRuntime *)createResult.armyRuntimeOrError;
  if (!createResult.failed) {
    rootNodeOrSize = (previewArmyOrValue->common).ownership.modelNode;
    /* armies of runtime class 13 lose their fourth child node (model runtime +0x4C = class id) */
    if (((*(int *)(*(int *)(previewArmyOrValue->common).ownership.definitionOrClassRecord + 0x4c) ==
          MODEL_RUNTIME_CLASS_13) && (3 < rootNodeOrSize->childCount)) &&
       (rootNodeOrSize->childNodes[3] != NULL)) {
      WorldRuntime_UnlinkNodeFromOwnerListD8((WorldOwnerListNode100 *)rootNodeOrSize->childNodes[3]);
      rootNodeOrSize->childNodes[3] = NULL;
    }
    /* angles are 16-bit turns: 0x2000 = 45 degrees, 0x3000 = 67.5 degrees */
    (rootNodeOrSize->modelPayload).worldRotationAngle2 = 0x2000;
    (rootNodeOrSize->modelPayload).worldRotationAngle1 = 0x3000;
    rootNodeOrSize->runtimeFlags = rootNodeOrSize->runtimeFlags | 1;
    rootNodeOrSize->tintArgb = 0xffffffff;
    ModelNodeRuntime_RebuildTransformsFromRoot(rootNodeOrSize);
    /* start the bounds at the root position; the recursion widens them over all nodes */
    g_ModelBoundsMinimumX = (rootNodeOrSize->worldTransform).translation.x;
    g_ModelBoundsMinimumY = (rootNodeOrSize->worldTransform).translation.y;
    g_ModelBoundsMinimumZ = (rootNodeOrSize->worldTransform).translation.z;
    g_ModelBoundsMaximumX = g_ModelBoundsMinimumX;
    g_ModelBoundsMaximumY = g_ModelBoundsMinimumY;
    g_ModelBoundsMaximumZ = g_ModelBoundsMinimumZ;
    ModelNodeRuntime_AccumulateTransformedBoundsRecursive(rootNodeOrSize);
    boundsSpanY = g_ModelBoundsMaximumY - g_ModelBoundsMinimumY;
    boundsSpanZ = g_ModelBoundsMaximumZ - g_ModelBoundsMinimumZ;
    maxBoundsSpan = boundsSpanZ;
    if (boundsSpanZ < boundsSpanY) {
      maxBoundsSpan = boundsSpanY;
    }
    /* camera centred on the bounds in Y and Z, backed off by four times the larger span in X */
    g_ArmyPreviewViewOriginYQ12 = boundsSpanY + g_ModelBoundsMinimumY * 2 >> 1;
    g_ArmyPreviewViewOriginZQ12 = boundsSpanZ + g_ModelBoundsMinimumZ * 2 >> 1;
    g_ArmyPreviewAuxiliaryOrientation0 = 0x6000;
    g_ArmyPreviewAuxiliaryOrientation1 = 0xffffe667;
    g_ArmyPreviewViewOriginXQ12 = g_ModelBoundsMaximumX + maxBoundsSpan * 4;
    g_ArmyPreviewPrimaryColorArgb = 0xffc0c0c0;
    g_ArmyPreviewSecondaryColorArgb = 0xff606060;
    g_ArmyPreviewProjectionScaleQ12 = 0x800;
    g_ArmyPreviewViewAngle0 = 0xffff8000;
    g_ArmyPreviewViewAngle1 = 0;
    g_ArmyPreviewProjectionShift = 4;
    g_ArmyPreviewModelNodePointer = (uint32_t)rootNodeOrSize;
    offscreenResult = g_GraphicsOffscreenRenderModelListToTextureSource
                       ((GraphicsOffscreenSceneExtents *)&g_ArmyPreviewPrimaryColorArgb,
                        &g_ArmyPreviewAuxiliaryOrientation0,
                        (GraphicsOffscreenViewParameters *)&g_ArmyPreviewViewOriginXQ12,
                        previewHeight * 2,previewWidth * 2,1,
                        (ModelRuntimeNode **)&g_ArmyPreviewModelNodePointer);
    previewTexture = offscreenResult.allocation;
    if (!offscreenResult.failed) {
      /* the texture is typed as GameEntityRuntime here: its pixels start at +0x220 and the header fields
         rewritten below (+0x200/+0x204 and +0x218/+0x21C) hold its width and height; +0x04 is the
         allocation size */
      sourcePixels = &previewTexture[1].common.commandTarget.targetWorldXQ12;
      ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,previewArmyOrValue);
      /* downsample in place: each output pixel averages a 2x2 block of the double-size image */
      destinationPixels = sourcePixels;
      previewWidth = savedPreviewWidth;
      do {
        do {
          pixelTopLeft = *sourcePixels;
          pixelTopRight = sourcePixels[1];
          pixelBottomLeft = sourcePixels[savedPreviewWidth * 2];
          pixelBottomRight = sourcePixels[savedPreviewWidth * 2 + 1];
          /* PUNPCKLBW mm,mm; PSRLW mm,4: each pixel byte b becomes the 16-bit lane (b * 0x101) >> 4. */
          mm0PackedValue0 =
               pmulhw(ARMY_PREVIEW_UNPACK_PIXEL_LANES(pixelTopLeft),
                      ((uint64_t *)&g_ArmyPreviewAlphaPremultiplyMmxLut256)[pixelTopLeft >> 0x18]);
          mm1PackedValue0 =
               pmulhw(ARMY_PREVIEW_UNPACK_PIXEL_LANES(pixelTopRight),
                      ((uint64_t *)&g_ArmyPreviewAlphaPremultiplyMmxLut256)[pixelTopRight >> 0x18]);
          mm2PackedValue0 =
               pmulhw(ARMY_PREVIEW_UNPACK_PIXEL_LANES(pixelBottomLeft),
                      ((uint64_t *)&g_ArmyPreviewAlphaPremultiplyMmxLut256)[pixelBottomLeft >> 0x18]);
          mm3PackedValue0 =
               pmulhw(ARMY_PREVIEW_UNPACK_PIXEL_LANES(pixelBottomRight),
                      ((uint64_t *)&g_ArmyPreviewAlphaPremultiplyMmxLut256)[pixelBottomRight >> 0x18]);
          alphaReciprocal =
               ((uint64_t *)&g_ArmyPreviewAverageAlphaReciprocalMmxLut256)
               [(pixelTopLeft >> 0x18) + (pixelTopRight >> 0x18) + (pixelBottomLeft >> 0x18) +
                (pixelBottomRight >> 0x18) >> 2];
          topLeftAlphaOrSum0 = ((short)mm0PackedValue0 + (short)mm1PackedValue0 +
                    (short)mm2PackedValue0 + (short)mm3PackedValue0 +
                   (short)g_ArmyPreviewDownsampleAlphaRoundingBiasMmx) * (short)alphaReciprocal;
          bottomLeftAlphaOrSum1 = ((short)((uint64_t)mm0PackedValue0 >> 0x10) +
                    (short)((uint64_t)mm1PackedValue0 >> 0x10) +
                    (short)((uint64_t)mm2PackedValue0 >> 0x10) +
                    (short)((uint64_t)mm3PackedValue0 >> 0x10) +
                   (short)((uint64_t)g_ArmyPreviewDownsampleAlphaRoundingBiasMmx >> 0x10)) *
                   (short)((uint64_t)alphaReciprocal >> 0x10);
          channelSum2 = ((short)((uint64_t)mm0PackedValue0 >> 0x20) +
                    (short)((uint64_t)mm1PackedValue0 >> 0x20) +
                    (short)((uint64_t)mm2PackedValue0 >> 0x20) +
                    (short)((uint64_t)mm3PackedValue0 >> 0x20) +
                   (short)((uint64_t)g_ArmyPreviewDownsampleAlphaRoundingBiasMmx >> 0x20)) *
                   (short)((uint64_t)alphaReciprocal >> 0x20);
          channelSum3 = ((short)((uint64_t)mm0PackedValue0 >> 0x30) +
                    (short)((uint64_t)mm1PackedValue0 >> 0x30) +
                    (short)((uint64_t)mm2PackedValue0 >> 0x30) +
                    (short)((uint64_t)mm3PackedValue0 >> 0x30) +
                   (short)((uint64_t)g_ArmyPreviewDownsampleAlphaRoundingBiasMmx >> 0x30)) *
                   (short)((uint64_t)alphaReciprocal >> 0x30);
          topRightAlphaOrClamp0 = topLeftAlphaOrSum0 >> 8;
          bottomRightAlphaOrClamp1 = bottomLeftAlphaOrSum1 >> 8;
          channelClamp2 = channelSum2 >> 8;
          channelClamp3 = channelSum3 >> 8;
          /* PSRLW 8 leaves every lane <= 0xFF, so PACKUSWB never saturates: it just packs the low bytes. */
          *destinationPixels = (uint32_t)(uint8_t)channelClamp3 << 24 | (uint32_t)(uint8_t)channelClamp2 << 16 |
                               (uint32_t)(uint8_t)bottomRightAlphaOrClamp1 << 8 | (uint32_t)(uint8_t)topRightAlphaOrClamp0;
          sourcePixels = sourcePixels + 2;
          destinationPixels = destinationPixels + 1;
          previewWidth = previewWidth - 1;
        } while (previewWidth != 0);
        sourcePixels = sourcePixels + savedPreviewWidth * 2; /* skip the second source row of the pair */
        previewHeight = previewHeight - 1;
        previewWidth = savedPreviewWidth;
      } while (previewHeight != 0);
      /* halve the stored sizes (previewArmyOrValue now holds the halved height) and shrink the allocation
         to the 0x220-byte header plus 32-bit pixels */
      halvedWidth = (void *)((int)previewTexture[1].common.commandFlags >> 1);
      previewArmyOrValue = (GameEntityRuntime *)((int)previewTexture[1].common.commandTarget.targetEntity >> 1);
      previewTexture[1].common.commandFlags = (GameEntityCommandFlags)halvedWidth;
      previewTexture[1].common.commandTarget.targetEntity = previewArmyOrValue;
      previewTexture[1].common.ownership.definitionOrClassRecord = halvedWidth;
      previewTexture[1].common.ownership.modelNode = (ModelRuntimeNode *)previewArmyOrValue;
      rootNodeOrSize = (ModelRuntimeNode *)((int)halvedWidth * (int)previewArmyOrValue * 4 + 0x220);
      (previewTexture->common).ownership.modelNode = rootNodeOrSize;
      g_MemoryApi.shrinkInPlace((uint32_t)rootNodeOrSize,previewTexture);
      successResult.failed = false;
      successResult.previewTexture = (GraphicsTextureResource *)previewTexture;
      return successResult;
    }
    ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,previewArmyOrValue);
    previewArmyOrValue = previewTexture; /* the render error */
  }
  failureResult.failed = true;
  failureResult.previewTexture = (GraphicsTextureResource *)previewArmyOrValue;
  return failureResult;
}


/* Address: 0x0052A7C0.
   Per-step update of one model runtime and, recursively, its attached children (called by
   ArmyRuntimeMaintenance_UpdateHierarchyAiAndTimers, the primaryUpdate entry of g_RuntimeMaintenanceCallbackPhases).
   Runs the class callback runtimeUpdate[class] of g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes, then the
   shared bookkeeping: health regeneration, dismantling (Xenite refund while health drains, destruction effect
   at zero), the attachment channel ticks of a model without health, and technology research (Xenite paid once
   when affordable, the Energy load held until the technology is unlocked).
*/
void ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive
          (WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  uint32_t *classStateField;
  uint32_t previousHealth;
  ModelRuntimeNode *rootNode;
  int energyRequirement;
  ModelRuntimeSlot *tickCursor;
  uint32_t clampedHealth;
  ModelRuntimeSlot *tickCountOrChild;
  int factionOrProgress;
  uint32_t definitionOrCount;
  uint32_t limitOrFlags;
  
  definitionOrCount = (modelRuntime->definitionOrSavedId).savedIdOrOffset;
  g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[*(int *)(definitionOrCount + 0x4c)]
            (worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  /* every 4 ticks health (+0x3C) regenerates by definition +0x1B4 up to 3/4 of the definition's health (+0x60),
     or decays down to it while bit 0 is set */
  if (((modelRuntime->classState).classStateEC & ARMY_MODEL_STATE_DISMANTLING) == 0) {
    classStateField = &(modelRuntime->classState).classStateF8;
    *classStateField = *classStateField - g_InGameSimulationStepTicks;
    if ((int)*classStateField < 0) {
      (modelRuntime->classState).classStateF8 = 4;
      previousHealth = modelRuntime->definitionValue60_3C;
      limitOrFlags = *(int *)(definitionOrCount + 0x60) * 3;
      if (previousHealth != 0) {
        if (((modelRuntime->classState).classStateEC & 1) == 0) {
          if (((modelRuntime->classState).classStateEC & 0x400) == 0) {
            clampedHealth = previousHealth + *(int *)(definitionOrCount + 0x1b4);
            limitOrFlags = limitOrFlags >> 2;
            if ((int)limitOrFlags < (int)clampedHealth) {
              clampedHealth = limitOrFlags;
            }
            if ((int)modelRuntime->definitionValue60_3C < (int)clampedHealth) {
              modelRuntime->definitionValue60_3C = clampedHealth;
            }
          }
        }
        else {
          clampedHealth = previousHealth - *(int *)(definitionOrCount + 0x1b4);
          limitOrFlags = limitOrFlags >> 2;
          if ((int)clampedHealth < (int)limitOrFlags) {
            clampedHealth = limitOrFlags;
          }
          if ((int)clampedHealth < (int)modelRuntime->definitionValue60_3C) {
            modelRuntime->definitionValue60_3C = clampedHealth;
          }
        }
      }
    }
  }
  classStateField = &(modelRuntime->classState).classStateFC;
  *classStateField = *classStateField - g_InGameSimulationStepTicks;
  if (((((int)*classStateField < 0) &&
       (classStateField = &(modelRuntime->classState).classStateFC, *classStateField = *classStateField + 12,
       ((modelRuntime->classState).classStateEC & ARMY_MODEL_STATE_DISMANTLING) != 0)) &&
      (*(int *)(definitionOrCount + 0x184) != 0)) &&
     ((*(int *)(definitionOrCount + 0x60) != 0 && (0 < (int)modelRuntime->definitionValue60_3C)))) {
    /* dismantling, every 12 ticks: refund 1/32 of the Xenite value (+0x184) and drain 1/16 of the health */
    factionOrProgress = ((modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime)->factionIndex;
    g_GameFactionRuntimeImage.records[factionOrProgress].xeniteCurrentQ4 =
         g_GameFactionRuntimeImage.records[factionOrProgress].xeniteCurrentQ4 + (*(uint32_t *)(definitionOrCount + 0x184) >> 5);
    limitOrFlags = *(uint32_t *)(definitionOrCount + 0x60);
    if (*(int *)(definitionOrCount + 0x4c) == MODEL_RUNTIME_CLASS_22) {
      /* a pad also refunds its unlaunched linked assets */
      clampedHealth = ArmyRuntimeSpawner_ComputeRemainingLinkedAssetMetric
                        ((ArmyRuntimeLinkedChildMaskSlotView *)modelRuntime);
      g_GameFactionRuntimeImage.records[factionOrProgress].xeniteCurrentQ4 =
           g_GameFactionRuntimeImage.records[factionOrProgress].xeniteCurrentQ4 + clampedHealth;
    }
    limitOrFlags = limitOrFlags >> 4;
    classStateField = &modelRuntime->definitionValue60_3C;
    previousHealth = *classStateField;
    *classStateField = *classStateField - limitOrFlags;
    if (*classStateField == 0 || SBORROW4(previousHealth,limitOrFlags) != (int)*classStateField < 0) {
      modelRuntime->definitionValue60_3C = 0;
      classStateField = &(modelRuntime->classState).classStateEC;
      *classStateField = *classStateField ^ (ARMY_MODEL_STATE_DISMANTLING | 0x200);
      rootNode = (modelRuntime->rootModelNodeOrSavedOffset).modelNode;
      EffectRuntimePool_CreateInstanceFromDefinition
                (EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY,
                 THANDOR_BITCAST(ModelRuntimeSlot *, EffectRuntimeOwnerReference4, modelRuntime),
                 (rootNode->modelPayload).worldRotationAngle2,
                 (rootNode->modelPayload).worldRotationAngle1,
                 (rootNode->modelPayload).worldRotationAngle0,(rootNode->worldTransform).translation.z,
                 (rootNode->worldTransform).translation.y,(rootNode->worldTransform).translation.x,
                 *(EffectDefinition **)(definitionOrCount + 0x190),worldRuntime);
      /* Setting it also skips the attachment tick loop below (the original jumps past it). */
      classStateField = &(modelRuntime->classState).classStateEC;
      *classStateField = *classStateField | ARMY_MODEL_STATE_DESTRUCTION_STARTED;
    }
  }
  /* health gone (and not already exploding): tick the attachment channels once per simulation tick */
  if ((((modelRuntime->classState).classStateEC & ARMY_MODEL_STATE_DESTRUCTION_STARTED) == 0) &&
     ((int)modelRuntime->definitionValue60_3C < 1)) {
    tickCountOrChild = (ModelRuntimeSlot *)g_InGameSimulationStepTicks;
    tickCursor = (modelRuntime->linkedModelRuntimeOrSavedOffset).modelRuntime;
    while (tickCursor != NULL) {
      ArmyRuntime_ProcessReadyAttachmentChannels(worldRuntime,modelRuntime);
      modelRuntime->definitionValue84_40 = modelRuntime->definitionValue84_40 - 1;
      modelRuntime->definitionValue88_44 = modelRuntime->definitionValue88_44 - 1;
      modelRuntime->definitionValue94_48 = modelRuntime->definitionValue94_48 - 1;
      modelRuntime->definitionValue9C_4C = modelRuntime->definitionValue9C_4C - 1;
      modelRuntime->definitionValueA4_50 = modelRuntime->definitionValueA4_50 - 1;
      modelRuntime->definitionValueAC_54 = modelRuntime->definitionValueAC_54 - 1;
      modelRuntime->definitionValueB4_58 = modelRuntime->definitionValueB4_58 - 1;
      modelRuntime->definitionValueBC_5C = modelRuntime->definitionValueBC_5C - 1;
      /* tickCountOrChild - 1 byte: the tick counter (DEC ECX in the original) */
      tickCountOrChild = (ModelRuntimeSlot *)((int)&tickCountOrChild[-1].attachments140[5].reserved1C + 3);
      tickCursor = tickCountOrChild;
    }
  }
  /* research progress (+0x100 record: +0 technology id, +4 duration, +8 elapsed, +0xC Energy load) */
  if ((((modelRuntime->classState).classStateEC & ARMY_MODEL_STATE_RESEARCHING) != 0) &&
     (factionOrProgress = *(int *)(modelRuntime->reserved100_117 + 8) + g_InGameSimulationStepTicks,
     ((modelRuntime->classState).classStateEC & 1) == 0)) {
    *(int *)(modelRuntime->reserved100_117 + 8) = factionOrProgress;
    if (*(int *)(modelRuntime->reserved100_117 + 4) <= factionOrProgress) {
      Technology_UnlockForFaction
                ((((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->worldTransform).
                 translation.y,
                 (((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->worldTransform).
                 translation.x,*(TechnologyId *)modelRuntime->reserved100_117,
                 ((modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime)->factionIndex);
      factionOrProgress = *(int *)(modelRuntime->reserved100_117 + 0xc);
      classStateField = &(modelRuntime->classState).classStateEC;
      *classStateField = *classStateField & ~ARMY_MODEL_STATE_RESEARCHING;
      classStateField = &(modelRuntime->classState).definitionDerivedValueF4;
      *classStateField = *classStateField - factionOrProgress;
    }
  }
  /* queued research starts once its Xenite cost (+0x10 of the record) can be paid */
  if ((((modelRuntime->classState).classStateEC & ARMY_MODEL_STATE_RESEARCH_UNPAID) != 0) &&
     (((modelRuntime->classState).classStateEC & (ARMY_MODEL_STATE_RESEARCHING | ARMY_MODEL_STATE_PRODUCING)) == 0)) {
    factionOrProgress = ((modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime)->factionIndex;
    energyRequirement = *(int *)(modelRuntime->reserved100_117 + 0xc);
    if (*(int *)(modelRuntime->reserved100_117 + 0x10) <=
        (int)g_GameFactionRuntimeImage.records[factionOrProgress].xeniteCurrentQ4) {
      g_GameFactionRuntimeImage.records[factionOrProgress].xeniteCurrentQ4 =
           g_GameFactionRuntimeImage.records[factionOrProgress].xeniteCurrentQ4 -
           *(int *)(modelRuntime->reserved100_117 + 0x10);
      modelRuntime->reserved100_117[0x10] = 0;
      modelRuntime->reserved100_117[0x11] = 0;
      modelRuntime->reserved100_117[0x12] = 0;
      modelRuntime->reserved100_117[0x13] = 0;
      classStateField = &(modelRuntime->classState).classStateEC;
      *classStateField = *classStateField ^ (ARMY_MODEL_STATE_RESEARCHING | ARMY_MODEL_STATE_RESEARCH_UNPAID);
      classStateField = &(modelRuntime->classState).definitionDerivedValueF4;
      *classStateField = *classStateField + energyRequirement;
    }
  }
  /* recurse into the attached child models, passing bit 8 on to them */
  definitionOrCount = modelRuntime->attachmentCount0C;
  limitOrFlags = (modelRuntime->classState).classStateEC;
  if (definitionOrCount != 0) {
    do {
      tickCountOrChild = modelRuntime->attachments140[0].childModelRuntimeOrSavedOffset00;
      if (tickCountOrChild != NULL) {
        classStateField = &(tickCountOrChild->classState).classStateEC;
        *classStateField = *classStateField | limitOrFlags & 8;
        ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive(worldRuntime,tickCountOrChild);
      }
      modelRuntime = (ModelRuntimeSlot *)(modelRuntime->reserved10_37 + 0x10); /* next 0x20-byte attachment */
      definitionOrCount = definitionOrCount - 1;
    } while (definitionOrCount != 0);
  }
  return;
}


/* Address: 0x00527010.
   Launches one linked asset of a class-22 pad (called directly by
   ArmyRuntimeClass_UpdateLinkedModelFlagsAndDispatchTerrainContactMode): finds a not yet launched slot of +0x78
   holding linkedArmyAssetId, creates that army on the pad, marks the slot as used and links the new aircraft to
   the pad (+0x60, state 1 = parked) with the attack point and heading (+0x70..+0x78) and the pad's platform
   height; its health is scaled by the pad's health. Returns true (CF set) when no slot matches or the creation
   fails.
*/
bool ArmyRuntimeSpawner_CreateLinkedChildInstance
          (WorldMotionValue78 inheritedValue78,WorldMotionValue74 inheritedValue74,
          WorldMotionValue70 inheritedValue70,PckArmyAssetIdCatalog linkedArmyAssetId,
          WorldRuntimeContext *worldRuntime,ArmyRuntimeLinkedChildMaskSlotView *armyRuntime)

{
  ArmyRuntimeLinkedChildSlotMaskState4 *slotMaskState;
  int *childModelRuntime;
  ModelRuntimeNode *parentRootNode;
  int remainingSlots;
  uint32_t slotBit;
  ArmyRuntimeLinkedChildMaskSlotView *slotCursor;
  ArmyRuntimeCreateResult createResult;
  ModelRuntimeNode *modelNode;
  
  slotBit = 1;
  remainingSlots = *(int *)((int)armyRuntime->definitionOrAsset + 0xc4);
  slotCursor = armyRuntime;
  while ((linkedArmyAssetId != slotCursor->movementTarget0Q12 ||
         (((armyRuntime->articulatedContact).linkedChildSlotMaskState.linkedChildSlotMask & slotBit)
          != 0))) {
    slotCursor = (ArmyRuntimeLinkedChildMaskSlotView *)&slotCursor->modelNodeRuntime;
    slotBit = slotBit * 2;
    remainingSlots = remainingSlots - 1;
    if (remainingSlots == 0) {
      return true;
    }
  }
  modelNode = armyRuntime->modelNodeRuntime;
  createResult = ArmyRuntime_CreateInstanceFromAsset
                    (0,(modelNode->modelPayload).worldRotationAngle2,
                     (modelNode->worldTransform).translation.y,
                     (modelNode->worldTransform).translation.x,
                     (armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex,
                     linkedArmyAssetId,worldRuntime);
  if (createResult.failed) {
    return true;
  }
  childModelRuntime = *(int **)createResult.armyRuntimeOrError;
  slotMaskState = &(armyRuntime->articulatedContact).linkedChildSlotMaskState;
  slotMaskState->linkedChildSlotMask = slotMaskState->linkedChildSlotMask | slotBit;
  armyRuntime->fallbackWorldYQ12 = armyRuntime->fallbackWorldYQ12 - 1;
  childModelRuntime[0x18] = (int)armyRuntime;
  childModelRuntime[0x2e] = 1;
  modelNode = (ModelRuntimeNode *)childModelRuntime[1];
  childModelRuntime[0x1c] = inheritedValue70;
  childModelRuntime[0x1d] = inheritedValue74;
  parentRootNode = armyRuntime->modelNodeRuntime;
  childModelRuntime[0x1e] = inheritedValue78;
  (modelNode->childNodes[0]->modelPayload).localTranslationZQ12 =
       (parentRootNode->childNodes[0]->modelPayload).localTranslationZQ12;
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNode);
  childModelRuntime[0xf] = (int)(((int64_t)armyRuntime->actionVector2Q12 * (int64_t)*(int *)(*childModelRuntime + 0x60))
                     / (int64_t)*(int *)((int)armyRuntime->definitionOrAsset + 0x60));
  return false;
}


/* Address: 0x00527430.
   Returns true (CF set) when the candidate army is within the combined radius of the source army's model and
   of every attached model listed in the source's attachment records (+0x140, 0x20 bytes each, count at +0xC);
   false as soon as one of them is out of reach.
*/
bool ArmyRuntime_TestModelAttachmentProximity(ArmyRuntimeSlot *candidateArmyRuntime,ArmyRuntimeSlot *sourceArmyRuntime)

{
  ModelRuntimeSlot *candidateModelRuntime;
  int *attachmentRecord;
  int remainingAttachments;
  bool baseWithinRadius;
  bool result;
  
  candidateModelRuntime = (candidateArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  remainingAttachments = sourceArmyRuntime->factionIndex; /* +0xC, used as the attachment count here */
  /* attachments140[3].childModelRuntimeOrSavedOffset00 is the model runtime's radius at +0x1A0 */
  baseWithinRadius = ArmyRuntime_TestPositionDistanceWithinCombinedRadius
                    ((UQ12)candidateModelRuntime->attachments140[3].childModelRuntimeOrSavedOffset00,
                     (UQ12)((sourceArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
                           attachments140[3].childModelRuntimeOrSavedOffset00,
                     candidateArmyRuntime->modelNodeRuntime,sourceArmyRuntime->modelNodeRuntime);
  result = false;
  if (baseWithinRadius) {
    for (; remainingAttachments != 0; remainingAttachments--) {
      /* the attachment record pointer at +0x140; the cursor advances 0x20 bytes per record. The record holds
         the attached model runtime (radius at +0x1A0) and its model node. */
      attachmentRecord = (int *)sourceArmyRuntime[1].commandCoordinate0Q12;
      if ((attachmentRecord != NULL) &&
         (result = ArmyRuntime_TestPositionDistanceWithinCombinedRadius
                            ((UQ12)candidateModelRuntime->attachments140[3].childModelRuntimeOrSavedOffset00,
                             *(UQ12 *)(*attachmentRecord + 0x1a0),candidateArmyRuntime->modelNodeRuntime,
                             (void *)attachmentRecord[1]), !result)) {
        return result;
      }
      sourceArmyRuntime = (ArmyRuntimeSlot *)&sourceArmyRuntime->commandCoordinate0Q12;
    }
    result = true;
  }
  return result;
}


/* Address: 0x0051C040.
   Removes an army for good: destroys its model hierarchy, drops every reference to it (player selections, the
   world selection, owned-model links of other nodes, each player's primary selection, the faction group
   tables), frees its runtime slot (model node = NULL) and rebuilds the in-game catalog grids and the
   selection detail panel.
*/
void ArmyRuntime_DestroyInstanceAndRefreshUi(WorldRuntimeContext *worldRuntime,GameEntityRuntime *entityRuntime)

{
  ModelRuntimeSlot *modelRuntime;
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  FrontendPlayerRuntimeRecord *playerBlockCursor;
  
  modelRuntime = (entityRuntime->common).ownership.definitionOrClassRecord;
  if (modelRuntime != NULL) {
    (entityRuntime->common).ownership.definitionOrClassRecord = NULL;
    ModelRuntimePool_DestroyHierarchyAndDetach(worldRuntime,modelRuntime);
  }
  SelectionPlayerBlocks_RemovePointer(entityRuntime);
  if (entityRuntime == (worldRuntime->selection).selectedEntity) {
    (worldRuntime->selection).selectedEntity = NULL;
  }
  WorldRuntime_ForEachNodeInOwnerListD8
            (entityRuntime,WorldRuntimeNode_ClearOwnedModelReferencesCallback,worldRuntime);
  remainingBlocks = g_FrontendPlayerRuntimeBlockCount;
  playerBlockCursor = g_FrontendPlayerRuntimeBlocks;
  do {
    if (entityRuntime ==
        (GameEntityRuntime *)
        g_SelectionPlayerRuntimeBlockPointers[playerBlockCursor->playerRuntimeId]->
        primarySelectionEntityOffset8094) {
      g_SelectionPlayerRuntimeBlockPointers[playerBlockCursor->playerRuntimeId]->
      primarySelectionEntityOffset8094 = 0;
    }
    playerBlockCursor++;
    remainingBlocks--;
  } while (remainingBlocks != 0);
  GameFactionRuntime_ClearRuntimeGroupMemberPointerFromAllFactionTables(entityRuntime);
  (entityRuntime->common).ownership.modelNode = NULL; /* marks the army slot free */
  UiCatalogGroup48_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
  UiCatalogGroup42_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
  InGameSelectionDetailPanel_Rebuild();
}


/* Address: 0x005246B0.
   Group-A command check: returns true (CF set) when the source army is of class 13 and the candidate army is
   within its radius + 0xC00 (0.75 in Q12) of the source model's anchor point (model lookup entry (1,5),
   transformed to world space), measured in x/y.
*/
bool ArmyRuntime_TestClass13ProximityCandidate(ArmyRuntimeSlot *candidateArmyRuntime,ArmyRuntimeSlot *sourceArmyRuntime)

{
  ModelRuntimeSlot *attachmentChildRuntime;
  ModelRuntimeNode *modelNodeRuntime;
  uint32_t anchorDistance;
  ModelLookupEntryResult lookupEntry;
  ModelWorldPoint anchorPoint;
  ModelRuntimeNode *modelNode1;
  
  if (((sourceArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C == 13) {
    /* not a pointer: the candidate model runtime's radius at +0x1A0 */
    attachmentChildRuntime = ((candidateArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->attachments140[3].
             childModelRuntimeOrSavedOffset00;
    modelNodeRuntime = sourceArmyRuntime->modelNodeRuntime;
    modelNode1 = candidateArmyRuntime->modelNodeRuntime;
    lookupEntry = ModelLookupTable_ContainsPackedKey(1,5,(modelNodeRuntime->modelPayload).modelResource)
    ;
    if (!lookupEntry.notFound) {
      anchorPoint = ModelNodeRuntime_TransformLocalPointRegs(lookupEntry.entry,modelNodeRuntime);
      anchorDistance = FixedMath_Length2(anchorPoint.yQ12 - (modelNode1->worldTransform).translation.y,
                                anchorPoint.xQ12 - (modelNode1->worldTransform).translation.x);
      /* attachmentChildRuntime + 6 is radius + 0xC00 (ADD EDX,0xC00): Ghidra scaled the constant by
         sizeof(ModelRuntimeSlot) = 0x200 */
      if ((int)anchorDistance <= (int)(attachmentChildRuntime + 6)) {
        return true;
      }
    }
  }
  return false;
}


/* Address: 0x00526510.
   Air-raid alert (called directly by ArmyRuntimeClassUpdateSlot21_DispatchByClassId on every tick of an attack
   run): when the aircraft of factionIndex is hostile to the active faction and flies over a grid cell with bit
   0x10 in the active faction's byte (+0x70 + faction of the cell), the sound soundAssetIndex is played
   unpositioned at the effects gain, at most once per 16 ticks of the faction's relationTransitionTick.
*/
void ArmyRuntime_TryPlayMappedTerrainSoundAtWorldPoint(FactionRuntimeIndex factionIndex,Q12 worldYQ12,Q12 worldXQ12,
          SoundAssetIndex soundAssetIndex,WorldRuntimeContext *worldContext)

{
  DirectSoundVoiceSet **voiceSetRef;
  FieldGridDimension gridWidthCells;
  uint32_t capabilityBitIndex;
  int cellColumn;
  uint32_t projectedRow;
  int cellRow;
  bool capabilityClear;
  FieldGridAsset *fieldGrid;
  
  if ((((soundAssetIndex != 0) && (worldContext->dwordArray != NULL)) &&
      (soundAssetIndex < worldContext->dwordArrayCount)) &&
     (voiceSetRef = (DirectSoundVoiceSet **)worldContext->dwordArray[soundAssetIndex],
     voiceSetRef != NULL)) {
    fieldGrid = worldContext->fieldGrid;
    /* world point -> grid cell (fixed-point projection, rounded) */
    projectedRow = (int)((uint64_t)((int64_t)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
            (uint32_t)((int64_t)worldYQ12 * -0x20c8cc) >> 0x15;
    gridWidthCells = fieldGrid->gridWidth;
    cellColumn = (int)((((int)((uint64_t)((int64_t)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                   (uint32_t)((int64_t)worldXQ12 * 0x1c6e9c) >> 0x14) - projectedRow) + 0x800) >> 0xc;
    if (((-1 < cellColumn) && (cellRow = (int)(projectedRow * 2 + 0x800) >> 0xc, -1 < cellRow)) &&
       ((cellColumn < (int)gridWidthCells && (cellRow < (int)fieldGrid->gridHeight)))) {
      capabilityBitIndex = worldContext->activeFactionRuntimeIndex;
      if (factionIndex != capabilityBitIndex) {
        capabilityClear = GameFactionRuntime_TestCapabilityBitClear(capabilityBitIndex,factionIndex);
        if (((capabilityClear) &&
            ((fieldGrid->cells[gridWidthCells * cellRow + cellColumn].runtime60_6B[capabilityBitIndex + 0x10] &
             0x10) != 0)) &&
           (0x10 < g_GameFactionRuntimeImage.records[capabilityBitIndex].relationTransitionTick)) {
          g_GameFactionRuntimeImage.records[capabilityBitIndex].relationTransitionTick = 0;
          g_SoundPlayOneShot
                    (g_SoundEffectsGainQ15,g_SoundEffectsGainQ15,*voiceSetRef);
        }
      }
    }
  }
  return;
}


/* Address: 0x005271A0.
   Xenite refund for the not yet launched linked assets of a class-22 pad that is being dismantled (called
   directly by ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive): sums the value at +0x184 of the
   faction's model definition of every linked asset whose slot bit is still clear, divided by 32 (the same
   rate as the pad's own refund).
*/
uint32_t ArmyRuntimeSpawner_ComputeRemainingLinkedAssetMetric(ArmyRuntimeLinkedChildMaskSlotView *armyRuntime)

{
  FactionRuntimeIndex factionIndex;
  int remainingSlots;
  uint32_t metricSum;
  uint32_t slotBit;
  ArmyRuntimeLinkedChildMaskSlotView *slotCursor;
  ArmyAssetLookupResult assetLookup;
  ModelDefinitionResult definitionLookup;
  
  metricSum = 0;
  factionIndex = (armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex;
  slotBit = 1;
  remainingSlots = *(int *)((int)armyRuntime->definitionOrAsset + 0xc4);
  slotCursor = armyRuntime;
  do {
    if (((armyRuntime->articulatedContact).linkedChildSlotMaskState.linkedChildSlotMask & slotBit) ==
        0) {
      assetLookup = ArmyAssetRegistry_FindById(slotCursor->movementTarget0Q12);
      if (!assetLookup.notFound) {
        definitionLookup = ModelDefinition_SelectFactionUnlockedLinkedDefinition
                          (factionIndex,(assetLookup.recordOrError)->rootNodeOffsetOrPointer);
        metricSum = metricSum + definitionLookup.modelDefinition[0x20].flags;
      }
    }
    slotCursor = (ArmyRuntimeLinkedChildMaskSlotView *)&slotCursor->modelNodeRuntime;
    slotBit = slotBit * 2;
    remainingSlots = remainingSlots - 1;
  } while (remainingSlots != 0);
  return metricSum >> 5;
}


/* Address: 0x00527230.
   Plays the one-shot sound at +0x270 of the army's definition (+0x00) at the army's position, with the
   definition's range and gain (+0x7C/+0x78), where the active faction's cell bits 0/1 are set. Called directly
   by ArmyRuntimeClassUpdateSlot21_DispatchByClassId for the home pad when an aircraft lands or takes off (the
   pad's platform sound; despite the name nothing is created).
*/
void ArmyRuntimeSpawner_PlayCreationSound(ArmyRuntimeSlot *armyRuntime,WorldRuntimeContext *worldRuntime)

{
  ModelRuntimeSlot *linkedModelRuntime;
  ModelRuntimeNode *rootNode;
  uint32_t soundAssetIndex;
  DirectSoundVoiceSet **voiceSetRef;
  bool cellMasked;
  
  linkedModelRuntime = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  rootNode = armyRuntime->modelNodeRuntime;
  soundAssetIndex = linkedModelRuntime[1].classLinkState.classState70;
  if (((soundAssetIndex != 0) && (soundAssetIndex < worldRuntime->dwordArrayCount)) &&
     (worldRuntime->dwordArray != NULL)) {
    voiceSetRef = (DirectSoundVoiceSet **)worldRuntime->dwordArray[soundAssetIndex];
    if (voiceSetRef != NULL) {
      cellMasked = TerrainGrid_TestProjectedCellMaskBits01
                        ((rootNode->worldTransform).translation.y,
                         (rootNode->worldTransform).translation.x,worldRuntime);
      if (!cellMasked) {
        SpatialSound_PlayPositionedOneShot
                  ((linkedModelRuntime->classLinkState).classState7C,(linkedModelRuntime->classLinkState).classState78,
                   &(rootNode->worldTransform).translation,voiceSetRef);
      }
    }
  }
  return;
}


/* Address: 0x005272B0.
   Plays the one-shot sound at +0x26C of the army's definition (+0x00) at the army's position, with the
   definition's range and gain (+0x7C/+0x78), where the active faction's cell bits 0/1 are set. Called directly
   by ArmyRuntimeClassUpdateSlot21_DispatchByClassId when a returning aircraft opens its home pad. Despite the
   name no effect is spawned; it is the hatch sound class 22 plays itself in
   ArmyRuntimeClass_UpdateLinkedModelFlagsAndDispatchTerrainContactMode.
*/
void ArmyRuntime_TrySpawnDefinitionEffectAtWorldPoint(ArmyRuntimeSlot *armyRuntime,WorldRuntimeContext *worldContext)

{
  ModelRuntimeSlot *linkedModelRuntime;
  ModelRuntimeNode *rootNode;
  uint32_t soundAssetIndex;
  DirectSoundVoiceSet **voiceSetRef;
  bool cellMasked;
  
  linkedModelRuntime = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  rootNode = armyRuntime->modelNodeRuntime;
  soundAssetIndex = linkedModelRuntime[1].classLinkState.armyLinkOrState6C.classState;
  if (((soundAssetIndex != 0) && (soundAssetIndex < worldContext->dwordArrayCount)) &&
     (worldContext->dwordArray != NULL)) {
    voiceSetRef = (DirectSoundVoiceSet **)worldContext->dwordArray[soundAssetIndex];
    if (voiceSetRef != NULL) {
      cellMasked = TerrainGrid_TestProjectedCellMaskBits01
                        ((rootNode->worldTransform).translation.y,
                         (rootNode->worldTransform).translation.x,worldContext);
      if (!cellMasked) {
        SpatialSound_PlayPositionedOneShot
                  ((linkedModelRuntime->classLinkState).classState7C,(linkedModelRuntime->classLinkState).classState78,
                   &(rootNode->worldTransform).translation,voiceSetRef);
      }
    }
  }
  return;
}


/* Address: 0x005273D0.
   Tests whether two model nodes are closer in the XY plane than the sum of their radii (collision/contact test
   of two armies or attachments): CF clear when dx^2 + dy^2 <= (candidateRadius + sourceRadius)^2, in 64-bit
   Q24 arithmetic.
*/
bool ArmyRuntime_TestPositionDistanceWithinCombinedRadius
          (UQ12 candidateRadiusQ12,UQ12 sourceRadiusQ12,void *candidateModelNode,
          void *sourceModelNode)

{
  int currentAxisDeltaQ12;
  int axisDeltaYQ12;
  int64_t remainingRadiusSquaredAfterXQ24;
  int64_t yDistanceSquaredQ24;

  /* +0x94 / +0x98: ModelRuntimeNode worldTransform.translation.x / .y */
  currentAxisDeltaQ12 =
       *(int *)((int)sourceModelNode + 0x94) - *(int *)((int)candidateModelNode + 0x94);
  remainingRadiusSquaredAfterXQ24 =
       (int64_t)(int)(sourceRadiusQ12 + candidateRadiusQ12) *
       (int64_t)(int)(sourceRadiusQ12 + candidateRadiusQ12) -
       (int64_t)currentAxisDeltaQ12 * (int64_t)currentAxisDeltaQ12;
  /* the second test is the 64-bit remainder - dy^2 >= 0, spelled out as high dwords minus the borrow */
  if ((-1 < remainingRadiusSquaredAfterXQ24) &&
     (axisDeltaYQ12 = *(int *)((int)sourceModelNode + 0x98) -
              *(int *)((int)candidateModelNode + 0x98),
     yDistanceSquaredQ24 = (int64_t)axisDeltaYQ12 * (int64_t)axisDeltaYQ12,
     -1 < (int)(((int)((uint64_t)remainingRadiusSquaredAfterXQ24 >> 0x20) -
                (int)((uint64_t)yDistanceSquaredQ24 >> 0x20)) -
               (uint32_t)((uint32_t)remainingRadiusSquaredAfterXQ24 < (uint32_t)yDistanceSquaredQ24)))) {
    return false;
  }
  return true;
}


/* Address: 0x005297D0.
   Bomb release of an aircraft on its attack run (called directly by ArmyRuntimeClassUpdateSlot21_DispatchByClassId):
   scores every intact model (classStateEC bit 8 clear) of another, non-neutral faction with AiCombatTarget_EvaluateCandidateScore. If the
   best one is within 1.0 (Q12) of its radius from the given point, every shot aims at it; otherwise each shot
   aims at the given point shifted by its launch point's offset from the aircraft. One shot of
   effectDefinitionId (despite the name a shot definition) is created from every launch point with packed key
   modelPointOrdinal << 4 | 2.
*/
void ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
          (EffectCreationFlagBits effectFlags,Q12 worldZQ12,Q12 worldYQ12,Q12 worldXQ12,
          ModelAttachmentOrdinal modelPointOrdinal,PckEffectDefinitionIdCatalog effectDefinitionId,
          void *sourceRuntime,void *modelPointTable,WorldRuntimeContext *worldContext)

{
  ArmyRuntimeSlot *sourceArmyRuntime;
  ArmyRuntimeSlot *candidateArmyRuntime;
  Q12 candidateWorldZ;
  uint32_t candidateScore;
  uint32_t candidateDistance;
  int deltaY;
  uint32_t currentBestScore;
  int deltaX;
  int radiusOrCount;
  WorldOwnerListNode100 *nodeCursor;
  ModelPackedPointRecord *localPointRecord;
  ModelWorldPoint localPoint;
  WorldOwnerListNode100 *bestCandidateNode;
  uint32_t pointOffsetMask;
  
  bestCandidateNode = NULL;
  nodeCursor = worldContext->ownerListHead;
  pointOffsetMask = 0xffffffff;
  currentBestScore = 0;
  sourceArmyRuntime = *(ArmyRuntimeSlot **)(*(int *)((int)sourceRuntime + 0x48) + 8);
  do {
    if ((((nodeCursor->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) && (nodeCursor != sourceRuntime))
        && (candidateArmyRuntime = *(ArmyRuntimeSlot **)((int)nodeCursor->runtimePayload + 8),
           (*(uint32_t *)((int)nodeCursor->runtimePayload + 0xec) & 8) == 0)) &&
       ((candidateArmyRuntime->factionIndex != 0 &&
        (candidateArmyRuntime->factionIndex != sourceArmyRuntime->factionIndex)))) {
      sourceArmyRuntime->runtimeState4C =
           *(uint32_t *)((((candidateArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
                      definitionOrSavedId).savedIdOrOffset + 0xdc);
      candidateScore = AiCombatTarget_EvaluateCandidateScore
                        (currentBestScore,1,0xffffffff,0xffffffff,candidateArmyRuntime,
                         sourceArmyRuntime);
      if (currentBestScore < candidateScore) {
        currentBestScore = candidateScore;
        bestCandidateNode = nodeCursor;
      }
    }
    nodeCursor = nodeCursor->nextNode;
  } while (nodeCursor != NULL);
  if (bestCandidateNode != NULL) {
    candidateWorldZ = bestCandidateNode->worldZQ12;
    deltaX = bestCandidateNode->worldXQ12 - worldXQ12;
    deltaY = bestCandidateNode->worldYQ12 - worldYQ12;
    radiusOrCount = *(int *)(*(int *)bestCandidateNode->runtimePayload + 0xdc);
    candidateDistance = FixedMath_Length2(deltaY,deltaX);
    if ((int)(candidateDistance - radiusOrCount) < Q12_ONE + 1) {
      pointOffsetMask = 0;
      worldXQ12 = deltaX + worldXQ12;
      worldYQ12 = deltaY + worldYQ12;
      worldZQ12 = candidateWorldZ;
    }
  }
  ModelNodeRuntime_RebuildTransformsFromRoot(sourceRuntime);
  radiusOrCount = *(int *)((int)modelPointTable + 0x30);
  localPointRecord = (ModelPackedPointRecord *)(radiusOrCount + *(int *)(radiusOrCount + 0xe4));
  for (radiusOrCount = *(int *)(radiusOrCount + 0xe8); radiusOrCount != 0; radiusOrCount = radiusOrCount - 1) {
    if (localPointRecord->packedLookupKey == (modelPointOrdinal << 4 | 2)) {
      localPoint = ModelNodeRuntime_TransformLocalPointRegs(localPointRecord,sourceRuntime);
      ShotRuntimePool_CreateProjectileFromDefinition
                (effectFlags,*(ArmyRuntimeSlot **)(*(int *)((int)sourceRuntime + 0x48) + 8),
                 worldZQ12,(localPoint.yQ12 - *(int *)((int)sourceRuntime + 0x98) & pointOffsetMask) + worldYQ12
                 ,(localPoint.xQ12 - *(int *)((int)sourceRuntime + 0x94) & pointOffsetMask) + worldXQ12,
                 localPoint.zQ12,localPoint.yQ12,localPoint.xQ12,(ShotDefinition *)effectDefinitionId,worldContext);
    }
    localPointRecord = localPointRecord + 1;
  }
  return;
}


/* Address: 0x00529980.
   One tick of a model whose health is gone (called directly by
   ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive, which then counts the eight channel timers at
   +0x40..+0x5C down). For every channel i whose timer is 0 it spawns the channel's
   effect (definition +0x80 + 8 * i) at every model point with packed key i << 4 | 3 of the root model, using the
   root's orientation, and, when the definition has a child model, at those of child node 0 with a fixed
   orientation. Also sets its own health to 0 and clears health and link (+0x38) of every attached child model.
*/
void ArmyRuntime_ProcessReadyAttachmentChannels(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  ModelRuntimeSlot *childModelRuntime;
  uint32_t channelIndex;
  Q12 worldXQ12;
  Q12 childWorldXQ12;
  uint32_t remainingAttachments;
  int recordCountOrTable;
  int tableOrRecordCount;
  ModelPackedPointRecord *pointRecord;
  ModelWorldPoint localPoint;
  ModelRuntimeNode *modelNode;
  
  channelIndex = 0;
  tableOrRecordCount = *(int *)(*(int *)((modelRuntime->definitionOrSavedId).savedIdOrOffset + 0x64) + 0x30);
  modelRuntime->definitionValue60_3C = 0;
  do {
    /* channel timer at +0x40 + 4 * channelIndex */
    if (*(int *)(modelRuntime->reserved10_37 + channelIndex * 4 + 0x30) == 0) {
      recordCountOrTable = *(int *)(tableOrRecordCount + 0xe8);
      pointRecord = (ModelPackedPointRecord *)(tableOrRecordCount + *(int *)(tableOrRecordCount + 0xe4));
      if (recordCountOrTable != 0) {
        do {
          if (channelIndex * 0x10 + 3 == pointRecord->packedLookupKey) {
            localPoint = ModelNodeRuntime_TransformLocalPointRegs
                              (pointRecord,(modelRuntime->rootModelNodeOrSavedOffset).modelNode);
            worldXQ12 = localPoint.yQ12;
            modelNode = (modelRuntime->rootModelNodeOrSavedOffset).modelNode;
            EffectRuntimePool_CreateInstanceFromDefinition
                      (EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY,
                       *(EffectRuntimeOwnerReference4 *)
                        &modelRuntime->linkedModelRuntimeOrSavedOffset,
                       (modelNode->modelPayload).worldRotationAngle2,
                       (modelNode->modelPayload).worldRotationAngle1,
                       (modelNode->modelPayload).worldRotationAngle0,localPoint.zQ12,worldXQ12,localPoint.xQ12,
                       *(EffectDefinition **)
                        ((modelRuntime->definitionOrSavedId).savedIdOrOffset + 0x80 + channelIndex * 8),
                       worldRuntime);
          }
          pointRecord = pointRecord + 1;
          recordCountOrTable = recordCountOrTable - 1;
        } while (recordCountOrTable != 0);
      }
    }
    channelIndex = channelIndex + 1;
  } while (channelIndex < 8);
  tableOrRecordCount = *(int *)((modelRuntime->definitionOrSavedId).savedIdOrOffset + 0x64);
  channelIndex = 0;
  if ((*(int *)(tableOrRecordCount + 0x14) != 0) &&
     (tableOrRecordCount = *(int *)(tableOrRecordCount + 0x18), recordCountOrTable = *(int *)(tableOrRecordCount + 0x30),
     (*(uint32_t *)(tableOrRecordCount + 4) & 0xf) == 0)) {
    do {
      if (*(int *)(modelRuntime->reserved10_37 + channelIndex * 4 + 0x30) == 0) {
        tableOrRecordCount = *(int *)(recordCountOrTable + 0xe8);
        pointRecord = (ModelPackedPointRecord *)(recordCountOrTable + *(int *)(recordCountOrTable + 0xe4));
        if (tableOrRecordCount != 0) {
          do {
            if (((channelIndex * 0x10 + 3 == pointRecord->packedLookupKey) &&
                (((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->childCount != 0)) &&
               (modelNode = ((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->childNodes[0],
               modelNode != NULL)) {
              localPoint = ModelNodeRuntime_TransformLocalPointRegs(pointRecord,modelNode);
              childWorldXQ12 = localPoint.yQ12;
              EffectRuntimePool_CreateInstanceFromDefinition
                        (EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY,
                         *(EffectRuntimeOwnerReference4 *)
                          &modelRuntime->linkedModelRuntimeOrSavedOffset,0,FIXED_ANGLE16_QUARTER_TURN,0,localPoint.zQ12,
                         childWorldXQ12,localPoint.xQ12,
                         *(EffectDefinition **)
                          ((modelRuntime->definitionOrSavedId).savedIdOrOffset + 0x80 + channelIndex * 8),
                         worldRuntime);
            }
            pointRecord = pointRecord + 1;
            tableOrRecordCount = tableOrRecordCount - 1;
          } while (tableOrRecordCount != 0);
        }
      }
      channelIndex = channelIndex + 1;
    } while (channelIndex < 8);
  }
  /* modelRuntime advances by one 0x20-byte attachment record per iteration */
  for (remainingAttachments = modelRuntime->attachmentCount0C; remainingAttachments != 0; remainingAttachments = remainingAttachments - 1) {
    childModelRuntime = modelRuntime->attachments140[0].childModelRuntimeOrSavedOffset00;
    if (childModelRuntime != NULL) {
      childModelRuntime->definitionValue60_3C = 0;
      (childModelRuntime->linkedModelRuntimeOrSavedOffset).modelRuntime = NULL;
    }
    modelRuntime = (ModelRuntimeSlot *)(modelRuntime->reserved10_37 + 0x10);
  }
  return;
}


/* Address: 0x0052A760.
   Runs the sound callback classMethodD[class] of g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes for a model
   runtime and recursively for its attached child models (called by
   ArmyRuntimeMaintenance_DispatchClassMethodDRecursive, the audioRefresh entry of
   g_RuntimeMaintenanceCallbackPhases). The ArmyRuntimeSlot view is really a model runtime: factionIndex is the
   attachment count at +0x0C, commandCoordinate0Q12 of the next slot view the child at +0x140 + 0x20 * i.
*/
void ArmyRuntimeHierarchy_DispatchClassMethodDRecursive(WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  int remainingChildren;
  
  remainingChildren = armyRuntime->factionIndex;
  (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classMethodD
    [((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C])
            (worldRuntime,armyRuntime);
  for (; remainingChildren != 0; remainingChildren = remainingChildren - 1) {
    if ((ArmyRuntimeSlot *)armyRuntime[1].commandCoordinate0Q12 != NULL) {
      ArmyRuntimeHierarchy_DispatchClassMethodDRecursive
                (worldRuntime,(ArmyRuntimeSlot *)armyRuntime[1].commandCoordinate0Q12);
    }
    armyRuntime = (ArmyRuntimeSlot *)&armyRuntime->commandCoordinate0Q12; /* + 0x20: next attachment */
  }
  return;
}


/* Address: 0x0051C350.
   Recomputes the army's derived combat figures shown on selection from its model hierarchy (after creation or
   a change of attachments): the maxima at +0x90, +0x44, +0x48 and the shot selection range at +0x4C are
   cleared, as are the eight per-target-class damage sums at +0x100, then
   ModelRuntimeHierarchy_AccumulateDerivedSelectionMetrics adds up every node of the model.
*/
void ArmyRuntime_RebuildDerivedSelectionMetrics(ArmyRuntimeSlot *armyRuntime)

{
  uint8_t *metricBytes;
  int metricIndex;

  armyRuntime->runtimeState90 = 0;
  armyRuntime->runtimeState44 = 0;
  armyRuntime->runtimeState48 = 0;
  metricIndex = 7;
  armyRuntime->runtimeState4C = 0;
  do {
    /* dword +0x100 + metricIndex * 4 (MOV [EAX+ECX*4+0x100],0) */
    metricBytes = armyRuntime->reservedF8_FF + metricIndex * 4 + 8;
    metricBytes[0] = 0;
    metricBytes[1] = 0;
    metricBytes[2] = 0;
    metricBytes[3] = 0;
    metricIndex--;
  } while (-1 < metricIndex);
  if ((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime != NULL) {
    ModelRuntimeHierarchy_AccumulateDerivedSelectionMetrics
              ((int *)(armyRuntime->modelRuntimeOrSavedOffset).modelRuntime);
  }
  return;
}


/* Address: 0x0051DBA0.
   Stub of a world point test (called directly by the aircraft and pad updates, slots 21 and 22): always
   returns false (CF clear), so the callers' `!result` branches are always taken.
*/
bool ArmyRuntime_TestWorldPointAllowedDefault(uint32_t allowedContext,uint32_t worldYQ12,uint32_t worldXQ12)

{
  return false;
}


/* Address: 0x0051B8F0.
   Creates an army (unit or building) of an army asset for a faction at a world point: takes the first free
   army slot, creates the faction's model (and its linked child models) with the faction's army graphics, links
   it into the world, places it on the terrain and initialises occupancy, tint and selection metrics. Returns
   the army slot, or with CF set FATAL_ERROR_GENERAL_FAILURE (no free slot or model creation failed) or
   FATAL_ERROR_ARMY_ID_NOT_FOUND (the id is left in g_PackageLastErrorPath).
*/
ArmyRuntimeCreateResult ArmyRuntime_CreateInstanceFromAsset
          (WorldObjectAllocationFlags creationFlags,AngleTurn32 orientationAngle,Q12 worldXQ12,
          Q12 worldYQ12,FactionRuntimeIndex factionIndex,PckArmyAssetIdCatalog armyAssetId,
          WorldRuntimeContext *worldRuntime)

{
  ArmyAssetRecordPrefix *armyAssetRecord;
  GraphicsTextureSet *textureSet;
  GraphicsPaletteAsset *paletteAsset;
  uint32_t rootNodeOrClassValue;
  PckArmyAssetIdCatalog classValue88;
  GameEntityRuntime *linkedEntity;
  ModelRuntimeNode *modelNodeRuntime;
  PckModelDefinitionIdCatalog modelDefinitionId;
  ModelRuntimeNode *resultOrModelNode;
  uint32_t armySlotsRemaining;
  int remainingOrDefinition;
  ArmyAssetRecordPrefix **registryCursor;
  ArmyRuntimeSlot *armyRuntime;
  bool childCreateFailed;
  ArmyRuntimeCreateResult failureResult;
  ModelDefinitionResult definitionLookup;
  ModelNodeCreateResult modelCreateResult;
  ArmyRuntimeCreateResult successResult;
  uint32_t slotScanContinueValue;
  ArmyAssetRuntimeSemanticView80 *definitionNode;
  
  /* find a free army slot (model node NULL); slotScanContinueValue is first the pool pointer (no pool: fail) */
  armySlotsRemaining = ARMY_RUNTIME_SLOT_COUNT;
  armyRuntime = g_ArmyRuntimeSlots;
  slotScanContinueValue = (uint32_t)g_ArmyRuntimeSlots;
  while (resultOrModelNode = (ModelRuntimeNode *)FATAL_ERROR_GENERAL_FAILURE, slotScanContinueValue != 0) {
    if (armyRuntime->modelNodeRuntime == NULL) {
      registryCursor = g_ArmyAssetRecordRegistry;
      remainingOrDefinition = ARMY_ASSET_REGISTRY_SLOT_COUNT;
      goto ArmyRuntime_CreateInstanceFromAsset_ScanAssetDefinitionRegistry;
    }
    armyRuntime++;
    armySlotsRemaining--;
    slotScanContinueValue = armySlotsRemaining;
  }
  goto ArmyRuntime_CreateInstanceFromAsset_ReturnCreationFailure;
  while( true ) {
    registryCursor++;
    remainingOrDefinition--;
    if (remainingOrDefinition == 0) break;
ArmyRuntime_CreateInstanceFromAsset_ScanAssetDefinitionRegistry:
    armyAssetRecord = *registryCursor;
    if ((armyAssetRecord != NULL) &&
       (armyAssetRecord->registryId == armyAssetId)) {
      armyRuntime->armyAssetId = armyAssetId;
      if (((creationFlags & ARMY_CREATE_COUNT_FOR_ACTIVE_FACTION) != 0) &&
          (factionIndex == worldRuntime->activeFactionRuntimeIndex)) {
        definitionLookup = ModelDefinition_SelectFactionUnlockedLinkedDefinition
                          (factionIndex,armyAssetRecord->rootNodeOffsetOrPointer);
        /* INC [EAX+0x1B0] */
        definitionLookup.modelDefinition[0x24].byteSize = definitionLookup.modelDefinition[0x24].byteSize + 1;
      }
      /* the graphics bindings exist for faction slots 0-7 only */
      if (7 < (uint32_t)factionIndex) {
        factionIndex = 7;
      }
      armyRuntime->factionIndex = factionIndex;
      if ((creationFlags & ARMY_CREATE_UNLOCK_TECHNOLOGY) != 0) {
        ModelDefinitionHierarchy_UnlockSelectedLinkedTechnology
                  (factionIndex,(ModelDefinitionHierarchyNodeAddress32)armyAssetRecord);
      }
      textureSet = g_ArmyGraphicsBindings[factionIndex].textureSet;
      paletteAsset = g_ArmyGraphicsBindings[factionIndex].paletteAsset;
      rootNodeOrClassValue = armyAssetRecord[7].selectionDetailTemplateVariantIndex;
      classValue88 = armyAssetRecord[7].registryId;
      armyRuntime->definitionClassValue80 = armyAssetRecord[7].byteSize;
      armyRuntime->definitionClassValue84 = rootNodeOrClassValue;
      armyRuntime->definitionClassValue88 = classValue88;
      linkedEntity = (GameEntityRuntime *)armyAssetRecord[1].byteSize;
      armyRuntime->runtimeState90 = 0;
      armyRuntime->runtimeState44 = 0;
      armyRuntime->runtimeState48 = 0;
      armyRuntime->runtimeState94 = 0;
      rootNodeOrClassValue = armyAssetRecord->rootNodeOffsetOrPointer;
      (armyRuntime->articulatedContact).fallbackPosition0Q12 = worldYQ12;
      (armyRuntime->articulatedContact).fallbackPosition1Q12 = worldXQ12;
      armyRuntime->linkedEntityRuntime = linkedEntity;
      (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime = NULL;
      armyRuntime->runtimeState8C = 0;
      modelDefinitionId = ModelDefinition_SelectFactionUnlockedLinkedId(factionIndex,rootNodeOrClassValue);
      modelCreateResult = ModelRuntimePool_CreateInstanceByDefinitionId
                         (paletteAsset,textureSet,armyRuntime,modelDefinitionId,worldRuntime);
      resultOrModelNode = modelCreateResult.modelNode;
      if (!modelCreateResult.failed) {
        /* the create result is the model runtime; +4 is its root model node */
        modelNodeRuntime = (ModelRuntimeNode *)(resultOrModelNode->common).nextNode;
        (armyRuntime->modelRuntimeOrSavedOffset).savedIdOrOffset = (uint32_t)resultOrModelNode;
        armyRuntime->modelNodeRuntime = modelNodeRuntime;
        /* worldYQ12 goes to translation.x and worldXQ12 to translation.y throughout, as in the original; the
           parameter names are swapped relative to the node fields */
        (modelNodeRuntime->worldTransform).translation.x = worldYQ12;
        (modelNodeRuntime->worldTransform).translation.y = worldXQ12;
        (modelNodeRuntime->worldTransform).translation.z = 0;
        armyRuntime->runtimeStateA4 = 0;
        armyRuntime->fallbackWorldYQ12 = worldYQ12;
        armyRuntime->fallbackWorldXQ12 = worldXQ12;
        armyRuntime->movementTarget0Q12 = worldYQ12;
        armyRuntime->movementTarget1Q12 = worldXQ12;
        (modelNodeRuntime->modelPayload).worldRotationAngle0 = 0;
        (modelNodeRuntime->modelPayload).worldRotationAngle1 = FIXED_ANGLE16_QUARTER_TURN;
        (modelNodeRuntime->modelPayload).worldRotationAngle2 = orientationAngle;
        armyRuntime->commandTargetArmyRuntime = NULL;
        armyRuntime->runtimeState98 = 0;
        armyRuntime->commandCoordinate0Q12 = 0;
        armyRuntime->commandCoordinate1Q12 = 0;
        armyRuntime->commandCoordinate2Q12 = 0;
        armyRuntime->commandModeFlags = 0;
        armyRuntime->commandGeneration = 0;
        (armyRuntime->articulatedContact).fallbackPosition0Q12 = worldYQ12;
        (armyRuntime->articulatedContact).fallbackPosition1Q12 = worldXQ12;
        (armyRuntime->linkedChildOverloadedState).primaryCoordinateCommandOrHistory.
        coordinateOrTargetQ12 = worldYQ12;
        (armyRuntime->linkedChildOverloadedState).secondaryCoordinateCommandOrHistory.
        coordinateOrTargetQ12 = worldXQ12;
        armyRuntime->movementPosition0Q12 = worldYQ12;
        armyRuntime->movementPosition1Q12 = worldXQ12;
        armyRuntime->movementStateFlags = 0;
        armyRuntime->actionVector1Q12 = 0;
        armyRuntime->terrainOccupancyMask0 = 0;
        armyRuntime->terrainOccupancyMask1 = 0;
        armyRuntime->runtimeState40 = 0;
        childCreateFailed = ModelNodeRuntime_InstantiateLinkedChildrenRecursive
                          (factionIndex,paletteAsset,textureSet,
                           (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime,rootNodeOrClassValue,worldRuntime)
        ;
        resultOrModelNode = (ModelRuntimeNode *)worldYQ12;
        if (!childCreateFailed) {
          WorldRuntime_LinkNodeIntoOwnerListD8((WorldOwnerListNode100 *)modelNodeRuntime);
          ModelNodeRuntime_RecomputeSubtreeBoundingRadius(modelNodeRuntime);
          /* remainingOrDefinition: the model runtime's definition (its first dword). Terrain contact by the
             definition's contact kind (+0x278); depth class by its model class (+0x4C); depth radius +0xDC. */
          remainingOrDefinition = *THANDOR_BITCAST(ModelRuntimeSlotReferenceOrSavedOffset4, int *, armyRuntime->modelRuntimeOrSavedOffset);
          g_ArmyPlacementContactKindDispatchTable.callbacks[*(int *)(remainingOrDefinition + 0x278)]
                    (*(Q12 *)(remainingOrDefinition + 0x54),(modelNodeRuntime->worldTransform).translation.y,
                     (modelNodeRuntime->worldTransform).translation.x,modelNodeRuntime,worldRuntime)
          ;
          ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
          armyRuntime->depthBinClass =
               *(ModelRuntimeClassId *)
                (&g_ArmyRuntimeDepthBinClassByModelClass + *(int *)(remainingOrDefinition + 0x4c) * 4);
          ModelNodeRuntime_UpdateDepthBinMasks
                    (*(DepthIntervalRadius32 *)(remainingOrDefinition + 0xdc),modelNodeRuntime);
          ArmyRuntime_InitializeTerrainOccupancyFlags(worldRuntime,armyRuntime);
          ModelNodeRuntime_RefreshStateTint(modelNodeRuntime);
          ArmyRuntime_RebuildDerivedSelectionMetrics(armyRuntime);
          successResult.failed = false;
          successResult.armyRuntimeOrError = (uint32_t)armyRuntime;
          return successResult;
        }
      }
      goto ArmyRuntime_CreateInstanceFromAsset_ReturnCreationFailure;
    }
  }
  /* the asset id as decimal text (base 10, at least one digit) for the error message */
  g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,armyAssetId,g_PackageLastErrorPath);
  resultOrModelNode = (ModelRuntimeNode *)FATAL_ERROR_ARMY_ID_NOT_FOUND;
ArmyRuntime_CreateInstanceFromAsset_ReturnCreationFailure:
  failureResult.failed = true;
  failureResult.armyRuntimeOrError = (uint32_t)resultOrModelNode;
  return failureResult;
}


/* Address: 0x0051D0B0.
   Sets up the terrain occupancy of a newly placed army: classifies the field-grid neighbourhood of its model
   node (within the model definition's radius at +0xDC), lets TerrainOccupancyMask_ResolveRuntimeClassFlags
   derive the two occupancy masks and the node's occupancy flags (0x4, 0x8, 0x1000) from it, and forces node flag 0x1000
   when the model runtime has flag 0x200 set at +0xEC.
*/
void ArmyRuntime_InitializeTerrainOccupancyFlags
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  uint32_t occupancyRuntimeFlags;
  uint64_t neighborhoodClassificationPair;
  TerrainOccupancyResolvedMasksRegs12 resolvedMasks;
  ModelRuntimeNode *modelNode;
  void *modelRuntime;

  /* the original stores EDX of the classification result (MOV [ECX+0x50],EDX) */
  THANDOR_PART(uint32_t, neighborhoodClassificationPair, 4) =
       TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                 (*(Q12 *)((((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
                           definitionOrSavedId).savedIdOrOffset + 0xdc),
                  (armyRuntime->modelNodeRuntime->worldTransform).translation.y,
                  (armyRuntime->modelNodeRuntime->worldTransform).translation.x,
                  worldRuntime->fieldGrid);
  armyRuntime->terrainOccupancyMask0 = THANDOR_PART(uint32_t, neighborhoodClassificationPair, 4);
  modelNode = armyRuntime->modelNodeRuntime;
  resolvedMasks = TerrainOccupancyMask_ResolveRuntimeClassFlags
                    (modelNode->runtimeFlags,armyRuntime->terrainOccupancyMask1,
                     armyRuntime->terrainOccupancyMask0,
                     (char)worldRuntime->activeFactionRuntimeIndex);
  occupancyRuntimeFlags = resolvedMasks.runtimeFlags;
  /* replace node flags 0x4, 0x8 and 0x1000 by the resolved ones */
  modelNode->runtimeFlags = modelNode->runtimeFlags & 0xffffeff3;
  armyRuntime->terrainOccupancyMask0 = resolvedMasks.primaryOccupancyMask;
  armyRuntime->terrainOccupancyMask1 = resolvedMasks.secondaryOccupancyMask;
  modelRuntime = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  modelNode->runtimeFlags = modelNode->runtimeFlags | occupancyRuntimeFlags;
  if ((*(uint32_t *)((int)modelRuntime + 0xec) & 0x200) != 0) {
    modelNode->runtimeFlags = modelNode->runtimeFlags | 0x1000;
  }
  return;
}


/* Address: 0x00527E70.
   Idle animation of a building model: child nodes 0 and 1 spin (localRotationAngle2 += definition +0x10 / +0x1C
   per tick), child node 2 bobs in Z by definition +0x14 per tick between +0x24 and +0x28, reversing at the
   limits (classStateEC bit 2 = direction). While bit 0 is set the animation stands still; bit 4 follows bit 0,
   and each change of it rebuilds the owner's selection metrics. Reached through
   g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[10] and directly from the class updates 11,
   13, 14, 22 and gameplay/army/combat.
*/
void ArmyRuntime_UpdateAnimatedModelSubnodes(WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView200 *modelRuntime)

{
  uint32_t *classStateField;
  ModelRuntimeNode *modelNodeRuntime;
  ModelDefinitionRuntimeSemanticView280 *animationDefinition;
  Q12 updatedChildTranslationZQ12;
  int bobStep;
  ModelRuntimeNode *animatedChildNode;
  int childTranslationZQ12;
  ModelRuntimeNode *animatedNode;
  
  if (((modelRuntime->classState).classStateEC & 1) == 0) {
    if (((modelRuntime->classState).classStateEC & 4) != 0) {
      classStateField = &(modelRuntime->classState).classStateEC;
      *classStateField = *classStateField ^ 4;
      ArmyRuntime_RebuildDerivedSelectionMetrics(modelRuntime->ownerArmyRuntime);
    }
    modelNodeRuntime = modelRuntime->rootModelNode;
    animationDefinition = modelRuntime->modelDefinition;
    animatedChildNode = modelNodeRuntime->childNodes[0];
    if (modelNodeRuntime->childCount != 0) {
      if (animatedChildNode != NULL) {
        (animatedChildNode->modelPayload).localRotationAngle2 =
             *(int *)(animationDefinition->reserved00C_023 + 4) * g_InGameSimulationStepTicks +
             (animatedChildNode->modelPayload).localRotationAngle2 & 0xffff;
        animatedChildNode->runtimeFlags = animatedChildNode->runtimeFlags | 1;
      }
      animatedNode = modelNodeRuntime->childNodes[1];
      if (1 < modelNodeRuntime->childCount) {
        if (animatedNode != NULL) {
          (animatedNode->modelPayload).localRotationAngle2 =
               *(int *)(animationDefinition->reserved00C_023 + 0x10) * g_InGameSimulationStepTicks +
               (animatedNode->modelPayload).localRotationAngle2 & 0xffff;
          animatedNode->runtimeFlags = animatedNode->runtimeFlags | 1;
        }
        animatedNode = modelNodeRuntime->childNodes[2];
        if ((2 < modelNodeRuntime->childCount) && (animatedNode != NULL)) {
          childTranslationZQ12 = (animatedNode->modelPayload).localTranslationZQ12;
          bobStep = *(int *)(animationDefinition->reserved00C_023 + 8) * g_InGameSimulationStepTicks;
          if (((modelRuntime->classState).classStateEC & 2) == 0) {
            updatedChildTranslationZQ12 = childTranslationZQ12 - bobStep;
            if (updatedChildTranslationZQ12 < (int)animationDefinition->runtimeValue24) {
              updatedChildTranslationZQ12 = animationDefinition->runtimeValue24;
              classStateField = &(modelRuntime->classState).classStateEC;
              *classStateField = *classStateField ^ 2;
            }
          }
          else {
            updatedChildTranslationZQ12 = childTranslationZQ12 + bobStep;
            if ((int)animationDefinition->runtimeValue28 < updatedChildTranslationZQ12) {
              updatedChildTranslationZQ12 = animationDefinition->runtimeValue28;
              classStateField = &(modelRuntime->classState).classStateEC;
              *classStateField = *classStateField ^ 2;
            }
          }
          (animatedNode->modelPayload).localTranslationZQ12 = updatedChildTranslationZQ12;
          animatedNode->runtimeFlags = animatedNode->runtimeFlags | 1;
        }
      }
    }
    ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
  }
  else if (((modelRuntime->classState).classStateEC & 4) == 0) {
    classStateField = &(modelRuntime->classState).classStateEC;
    *classStateField = *classStateField ^ 4;
    ArmyRuntime_RebuildDerivedSelectionMetrics(modelRuntime->ownerArmyRuntime);
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  return;
}


/* Address: 0x00527C00.
   Timed emitters of an army model, reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes
   .runtimeUpdate[0] and [16] and directly from most class updates (here, gameplay/army/combat and movement).
   Timer +0xE4: fires the definition's shot (+0x168) straight ahead (1.0 along the root's orientation) and
   restarts at +0x16C plus a random part below +0x170. Timer +0xE8: spawns the land (+0x174) or, where
   FieldGrid_GetNearestWaterDelta is positive, the water (+0x58) effect at a model point with key n << 4 | 6 (random, or in turn when definition +0x68
   bit 0 is set; the root position when there is none) and restarts at +0x178 plus a random part below +0x17C.
*/
void ArmyRuntime_UpdateTimedShotAndEffectEmitters
          (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView200 *modelRuntime)

{
  uint32_t *emitterTimer;
  ModelDefinitionRuntimeSemanticView280 *emitterDefinition;
  int modelResourceAddress;
  uint32_t nodeOrWorldX;
  uint32_t pointSelector;
  int32_t waterDelta;
  uint32_t randomValue;
  int remainingRecords;
  uint32_t worldY;
  AngleTurn32 orientationAngle0;
  uint32_t randomOrPointCount;
  uint32_t *pointRecordCursor;
  uint32_t worldZQ12;
  AngleTurn32 orientationAngle1;
  ModelRuntimeNode *modelNode;
  EffectDefinition *selectedEffectDefinition;
  bool timerOverflow;
  ModelLookupEntryResult lookupEntry;
  FixedDirection launchDirection;
  ModelWorldPoint localPoint;
  AngleTurn32 orientationAngle2;
  GraphicsWorldCoordinateQ12 launchWorldZQ12;
  GraphicsWorldCoordinateQ12 launchWorldYQ12;
  GraphicsWorldCoordinateQ12 launchWorldXQ12;
  ShotDefinition *shotDefinition;
  EffectDefinition *effectDefinition;
  WorldRuntimeContext *worldContext;
  
  emitterDefinition = modelRuntime->modelDefinition;
  emitterTimer = &(modelRuntime->classState).enabledStateE4;
  timerOverflow = SBORROW4(*emitterTimer,g_InGameSimulationStepTicks);
  *emitterTimer = *emitterTimer - g_InGameSimulationStepTicks;
  /* SUB + JG: the shot timer has reached 0 or below (signed) */
  if ((*emitterTimer == 0 || timerOverflow != (int)*emitterTimer < 0) &&
     ((emitterDefinition->shotDefinitionReference168).definition != (ShotDefinition *)0xffffffff)) {
    randomOrPointCount = 0;
    if (*(int *)(emitterDefinition->reserved16C_173 + 4) != 0) {
      nodeOrWorldX = g_RandomGeneratorState.next();
      randomOrPointCount = nodeOrWorldX % *(uint32_t *)(emitterDefinition->reserved16C_173 + 4);
    }
    shotDefinition = (emitterDefinition->shotDefinitionReference168).definition;
    modelNode = modelRuntime->rootModelNode;
    (modelRuntime->classState).enabledStateE4 = randomOrPointCount + *(int *)emitterDefinition->reserved16C_173;
    launchWorldXQ12 = (modelNode->worldTransform).translation.x;
    launchWorldYQ12 = (modelNode->worldTransform).translation.y;
    launchWorldZQ12 = (modelNode->worldTransform).translation.z;
    worldContext = worldRuntime;
    launchDirection = FixedMath_DirectionFromAnglesScaledRegs
                       ((modelNode->modelPayload).worldRotationAngle1,
                        (modelNode->modelPayload).worldRotationAngle0,Q12_ONE);
    ShotRuntimePool_CreateProjectileFromDefinition
              (0,modelRuntime->ownerArmyRuntime,launchDirection.z + launchWorldZQ12,
               launchDirection.y + launchWorldYQ12,launchDirection.x + launchWorldXQ12,launchWorldZQ12,
               launchWorldYQ12,launchWorldXQ12,shotDefinition,worldContext);
  }
  emitterDefinition = modelRuntime->modelDefinition;
  emitterTimer = &(modelRuntime->classState).enabledStateE8;
  timerOverflow = SBORROW4(*emitterTimer,g_InGameSimulationStepTicks);
  *emitterTimer = *emitterTimer - g_InGameSimulationStepTicks;
  if ((*emitterTimer != 0 && timerOverflow == (int)*emitterTimer < 0) ||
     (((emitterDefinition->effectDefinitionReference174).definition == NULL &&
      ((emitterDefinition->effectDefinitionReference58).definition == NULL))))
  goto ArmyRuntime_UpdateTimedShotAndEffectEmitters_UpdateDamageThresholdEffectAndReturn;
  randomOrPointCount = 0;
  if (*(int *)(emitterDefinition->reserved178_187 + 4) != 0) {
    nodeOrWorldX = g_RandomGeneratorState.next();
    randomOrPointCount = nodeOrWorldX % *(uint32_t *)(emitterDefinition->reserved178_187 + 4);
  }
  (modelRuntime->classState).enabledStateE8 = randomOrPointCount + *(int *)emitterDefinition->reserved178_187;
  nodeOrWorldX = emitterDefinition->serializedNodeOffsetOrPointer64;
  if (emitterDefinition->runtimeClassId4C == MODEL_RUNTIME_CLASS_21_AIRCRAFT) {
    nodeOrWorldX = *(uint32_t *)(nodeOrWorldX + 0x18);
  }
  modelResourceAddress = *(int *)(nodeOrWorldX + 0x30);
  remainingRecords = *(int *)(modelResourceAddress + 0xe8);
  worldContext = worldRuntime;
  if (remainingRecords == 0) {
ArmyRuntime_UpdateTimedShotAndEffectEmitters_UseModelWorldPositionForEffectEmitter:
    modelNode = modelRuntime->rootModelNode;
    nodeOrWorldX = (modelNode->worldTransform).translation.x;
    worldY = (modelNode->worldTransform).translation.y;
    worldZQ12 = (modelNode->worldTransform).translation.z;
  }
  else {
    randomOrPointCount = 0;
    pointRecordCursor = (uint32_t *)(modelResourceAddress + *(int *)(modelResourceAddress + 0xe4));
    /* count the effect points: highest n + 1 of the packed keys n << 4 | 6 */
    do {
      if (((*pointRecordCursor & 0xf) == 6) && (randomOrPointCount <= *pointRecordCursor >> 4)) {
        randomOrPointCount = (*pointRecordCursor >> 4) + 1;
      }
      remainingRecords = remainingRecords - 1;
      pointRecordCursor = pointRecordCursor + 4;
    } while (remainingRecords != 0);
    if (randomOrPointCount == 0)
    goto ArmyRuntime_UpdateTimedShotAndEffectEmitters_UseModelWorldPositionForEffectEmitter;
    pointSelector = (modelRuntime->classState).classStateE0;
    if ((emitterDefinition->runtimeValue68 & 1) == 0) {
      pointSelector = g_RandomGeneratorState.next();
    }
    nodeOrWorldX = emitterDefinition->serializedNodeOffsetOrPointer64;
    if (emitterDefinition->runtimeClassId4C == MODEL_RUNTIME_CLASS_21_AIRCRAFT) {
      nodeOrWorldX = *(uint32_t *)(nodeOrWorldX + 0x18);
    }
    lookupEntry = ModelLookupTable_ContainsPackedKey
                       (pointSelector % randomOrPointCount,6,*(ModelResourceHitTestAndRenderView210 **)(nodeOrWorldX + 0x30));
    if (lookupEntry.notFound)
    goto ArmyRuntime_UpdateTimedShotAndEffectEmitters_UseModelWorldPositionForEffectEmitter;
    modelNode = modelRuntime->rootModelNode;
    if (emitterDefinition->runtimeClassId4C == MODEL_RUNTIME_CLASS_21_AIRCRAFT) {
      modelNode = modelNode->childNodes[0];
    }
    localPoint = ModelNodeRuntime_TransformLocalPointRegs(lookupEntry.entry,modelNode);
    worldZQ12 = localPoint.zQ12;
    worldY = localPoint.yQ12;
    nodeOrWorldX = localPoint.xQ12;
  }
  selectedEffectDefinition = (emitterDefinition->effectDefinitionReference174).definition;
  effectDefinition = selectedEffectDefinition;
  waterDelta = FieldGrid_GetNearestWaterDelta(worldY,nodeOrWorldX,worldRuntime->fieldGrid);
  if (0 < waterDelta) {
    selectedEffectDefinition = (emitterDefinition->effectDefinitionReference58).definition;
    effectDefinition = selectedEffectDefinition;
  }
  if ((selectedEffectDefinition == NULL) ||
     ((selectedEffectDefinition->transitionPrefix).transitionKind !=
      EFFECT_TRANSITION_INTEGRATE_LINEAR_MOTION_AND_SHADING_POSITION)) {
    modelNode = modelRuntime->rootModelNode;
    orientationAngle2 = (modelNode->modelPayload).worldRotationAngle0;
    orientationAngle1 = (modelNode->modelPayload).worldRotationAngle1;
    orientationAngle0 = (modelNode->modelPayload).worldRotationAngle2;
  }
  else {
    randomValue = g_RandomGeneratorState.next();
    orientationAngle0 = randomValue & 0xffff;
    orientationAngle1 = FIXED_ANGLE16_QUARTER_TURN - (randomValue >> 0x14);
    orientationAngle2 = orientationAngle0;
  }
  EffectRuntimePool_CreateInstanceFromDefinition
            (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference4, 0x0),orientationAngle0,
             orientationAngle1,orientationAngle2,worldZQ12,worldY,nodeOrWorldX,effectDefinition,
             worldContext);
  emitterTimer = &(modelRuntime->classState).classStateE0;
  *emitterTimer = *emitterTimer + 1;
ArmyRuntime_UpdateTimedShotAndEffectEmitters_UpdateDamageThresholdEffectAndReturn:
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  return;
}

