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
   Ownership: gameplay/army/runtime.
   Purpose: This function object claims Listing ownership for a previously unowned multi-entry/shared-
   tail/computed-dispatch region; it does not assert that every member entry is an independent ABI-level function.
   Runtime-update partition slots 0-23 receive (worldRuntime, armyRuntime). Exact disjoint ranges, terminal
   instructions, inherited register state, shared exits, and caller/table references were revalidated. No function
   splitting or boundary change is permitted.
   Local calls: ArmyRuntimeSpawner_PlayCreationSound, ArmyRuntime_TestWorldPointAllowedDefaultCf,
   ArmyRuntime_TryPlayMappedTerrainSoundAtWorldPoint, ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate,
   ArmyRuntime_TrySpawnDefinitionEffectAtWorldPoint, ArmyRuntime_UpdateTimedShotAndEffectEmitters.
   Cross-module calls: EffectRuntimePool_CreateInstanceFromDefinitionCf [world/effects/runtime],
   FixedMath_Atan2Angle16 [core/math/fixed], FixedMath_SinCosScaled [core/math/fixed],
   ArmyRuntime_ApplyDamageAndPropagateToParent [gameplay/army/combat], FieldGrid_InterpolateTopSurfaceHeight
   [world/terrain/grid], ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive [world/model/hierarchy].
*/

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClassUpdateSlot21_DispatchByClassId
          (WorldRuntimeContext *worldRuntime,ModelRuntimeClass21UpdateView200 *modelRuntime)

{
  dword *stateField;
  GraphicsFixedVec3 *translationVec;
  GraphicsWorldCoordinateQ12 *translationY;
  ModelRuntimeArmyLinkOrState4 *armyLinkState;
  ArmyRuntimeClassUpdate21DefinitionView27C *currentDefinition;
  ModelRuntimeSlot *armyRuntime;
  dword durationValue;
  void *modelPointSource;
  ModelRuntimeNode *childNode;
  ModelRuntimeSlot *linkedDefinitionSlot;
  ModelDefinitionRuntimeSemanticView280 *semanticDefinition;
  longlong product64;
  dword stateValue;
  int stepValue;
  ArmyRuntimeSlot *armySlot2;
  uint headingOrDelta;
  int maxTerrainHeight;
  int sampleCoord;
  Q12 childHeightZ;
  ArmyRuntimeClassUpdate21DefinitionView27C *classUpdate21Definition;
  ModelRuntimeNode *modelNode1;
  bool pointAllowed;
  FixedSinCosEdxEax8 sinCosStep;
  FieldGridHeightEaxCf5 terrainHeight;
  AngleTurn32 savedAngle1;
  InGameSimulationStepBatchTicks remainingTicks;
  AngleTurn32 savedAngle0;
  ArmyRuntimeClassUpdate21DefinitionView27C *savedClassUpdate21Definition;
  ModelRuntimeClass21UpdateView200 *savedModelRuntime;
  ModelRuntimeNode *modelNode2;
  
  currentDefinition = modelRuntime->modelDefinition;
  armyRuntime = (modelRuntime->classLinkState).modelLinkOrState60.modelRuntime;
  modelNode1 = modelRuntime->rootModelNode;
                    // WARNING: Switch is manually overridden
  switch((modelRuntime->class21State84).classStateB8) {
  case 1:
    if ((armyRuntime == (ModelRuntimeSlot *)0x0) || ((armyRuntime->classState).classStateB0 == 3)) {
      stateValue = currentDefinition->phaseInitialC0;
      durationValue = currentDefinition->phaseDurationC4;
      (modelRuntime->class21State84).classStateB8 = 3;
      (modelRuntime->classLinkState).classState7C = 0;
      (modelRuntime->classLinkState).classState64 = stateValue;
      (modelRuntime->classLinkState).classState68 = durationValue;
    }
    else if (((armyRuntime->classState).classStateB0 == 0) ||
            ((armyRuntime->classState).classStateB0 == 6)) {
      EffectRuntimePool_CreateInstanceFromDefinitionCf
                (EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY,
                 THANDOR_BITCAST(ModelRuntimeClass21UpdateView200 *, EffectRuntimeOwnerReference4, modelRuntime),
                 (modelNode1->modelPayload).worldRotationAngle2,
                 (modelNode1->modelPayload).worldRotationAngle1,
                 (modelNode1->modelPayload).worldRotationAngle0,
                 (modelNode1->worldTransform).translation.z,
                 (modelNode1->worldTransform).translation.y,
                 (modelNode1->worldTransform).translation.x,currentDefinition->completionEffect190,worldRuntime
                );
      stateField = &(modelRuntime->class21State84).classStateEC;
      *stateField = *stateField | 0x20;
    }
    else {
      stateValue = (((armyRuntime->rootModelNodeOrSavedOffset).modelNode)->childNodes[0]->modelPayload).
               localTranslationZQ12;
      (modelNode1->childNodes[0]->modelPayload).localTranslationZQ12 = stateValue;
      (modelRuntime->classLinkState).classState80 = stateValue;
    }
    break;
  case 2:
    stateValue = (modelRuntime->classLinkState).classState7C;
    product64 = (longlong)(int)stateValue * (longlong)(int)stateValue;
    modelNode1 = modelNode1->childNodes[0];
    product64 = (longlong)(int)(currentDefinition->arcCoefficient14 * g_InGameSimulationStepTicks) *
             (longlong)(int)((int)((ulonglong)product64 >> 0x20) << 0x14 | (uint)product64 >> 0xc);
    (modelNode1->modelPayload).localRotationAngle0 = 0x8000;
    stateValue = (modelRuntime->classLinkState).classState7C;
    (modelNode1->modelPayload).localTranslationZQ12 =
         ((int)((ulonglong)product64 >> 0x20) << 0x14 | (uint)product64 >> 0xc) +
         (modelRuntime->classLinkState).classState80;
    product64 = (longlong)(int)(stateValue * g_InGameSimulationStepTicks) *
             (longlong)currentDefinition->arcCoefficient14;
    stateValue = FixedMath_Atan2Angle16
                       ((int)((ulonglong)product64 >> 0x20) << 0x15 | (uint)product64 >> 0xb,0x1000);
    stepValue = currentDefinition->movementStepQ12_0C * g_InGameSimulationStepTicks;
    stateField = &(modelRuntime->classLinkState).classState7C;
    *stateField = *stateField + stepValue;
    (modelNode1->modelPayload).localRotationAngle1 = 0x4000 - stateValue & 0xffff;
    modelNode1 = modelRuntime->rootModelNode;
    sinCosStep = FixedMath_SinCosScaled((modelNode1->modelPayload).worldRotationAngle2,stepValue);
    translationVec = &(modelNode1->worldTransform).translation;
    translationVec->x = translationVec->x + (int)sinCosStep;
    translationY = &(modelNode1->worldTransform).translation.y;
    *translationY = *translationY + (int)(sinCosStep >> 0x20);
    stateField = &(modelRuntime->classLinkState).classState68;
    *stateField = *stateField - 1;
    if (((int)*stateField < 0) &&
       ((modelRuntime->class21State84).classStateB8 = 1, armyRuntime != (ModelRuntimeSlot *)0x0)) {
      currentDefinition = modelRuntime->modelDefinition;
      linkedDefinitionSlot = (ModelRuntimeSlot *)(armyRuntime->definitionOrSavedId).savedIdOrOffset;
      (armyRuntime->classState).classStateB0 = 4;
      ArmyRuntimeSpawner_PlayCreationSound((ArmyRuntimeSlot *)armyRuntime,worldRuntime);
      stepValue = (int)(((longlong)(int)armyRuntime->definitionValue60_3C *
                     (longlong)currentDefinition->healthOrScaleQ12_60) /
                    (longlong)(linkedDefinitionSlot->classLinkState).modelLinkOrState60.signedScalarState);
      headingOrDelta = stepValue - modelRuntime->definitionValue60_3C;
      if (headingOrDelta != 0 && (int)modelRuntime->definitionValue60_3C <= stepValue) {
        ArmyRuntime_ApplyDamageAndPropagateToParent(headingOrDelta >> 1,(ArmyRuntimeSlot *)armyRuntime);
      }
    }
    break;
  case 3:
    stateValue = (modelRuntime->classLinkState).classState7C;
    product64 = (longlong)(int)stateValue * (longlong)(int)stateValue;
    modelNode1 = modelNode1->childNodes[0];
    product64 = (longlong)(int)(currentDefinition->arcCoefficient14 * g_InGameSimulationStepTicks) *
             (longlong)(int)((int)((ulonglong)product64 >> 0x20) << 0x14 | (uint)product64 >> 0xc);
    (modelNode1->modelPayload).localRotationAngle0 = 0x8000;
    stateValue = (modelRuntime->classLinkState).classState7C;
    (modelNode1->modelPayload).localTranslationZQ12 =
         ((int)((ulonglong)product64 >> 0x20) << 0x14 | (uint)product64 >> 0xc) +
         (modelRuntime->classLinkState).classState80;
    product64 = (longlong)(int)(stateValue * g_InGameSimulationStepTicks) *
             (longlong)currentDefinition->arcCoefficient14;
    stateValue = FixedMath_Atan2Angle16
                       ((int)((ulonglong)product64 >> 0x20) << 0x15 | (uint)product64 >> 0xb,0x1000);
    stepValue = currentDefinition->movementStepQ12_0C * g_InGameSimulationStepTicks;
    stateField = &(modelRuntime->classLinkState).classState7C;
    *stateField = *stateField + stepValue;
    (modelNode1->modelPayload).localRotationAngle1 = 0x4000 - stateValue & 0xffff;
    modelNode1 = modelRuntime->rootModelNode;
    sinCosStep = FixedMath_SinCosScaled((modelNode1->modelPayload).worldRotationAngle2,stepValue);
    translationVec = &(modelNode1->worldTransform).translation;
    translationVec->x = translationVec->x + (int)sinCosStep;
    translationY = &(modelNode1->worldTransform).translation.y;
    *translationY = *translationY + (int)(sinCosStep >> 0x20);
    stateField = &(modelRuntime->classLinkState).classState64;
    *stateField = *stateField - 1;
    if ((*stateField == 0) && (armyRuntime != (ModelRuntimeSlot *)0x0)) {
      (armyRuntime->classState).classStateB0 = 4;
      ArmyRuntimeSpawner_PlayCreationSound((ArmyRuntimeSlot *)armyRuntime,worldRuntime);
    }
    stateField = &(modelRuntime->classLinkState).classState68;
    *stateField = *stateField - 1;
    if ((int)*stateField < 0) {
      (modelNode1->worldTransform).translation.x = -0x100000;
      (modelNode1->worldTransform).translation.y = 0x100000;
      (modelRuntime->class21State84).classStateB8 = 4;
    }
    break;
  case 4:
    pointAllowed = ArmyRuntime_TestWorldPointAllowedDefaultCf
                       (currentDefinition->worldPointAllowedContext48,
                        (modelRuntime->classLinkState).classState74,
                        (modelRuntime->classLinkState).classState70);
    if (!pointAllowed) {
      headingOrDelta = (modelRuntime->classLinkState).classState78;
      sinCosStep = FixedMath_SinCosScaled
                         (headingOrDelta ^ 0x8000,currentDefinition->movementStepQ12_0C * currentDefinition->travelStepCountC8);
      stateValue = (modelRuntime->classLinkState).classState74;
      (modelNode1->worldTransform).translation.x =
           (int)sinCosStep + (modelRuntime->classLinkState).classState70;
      (modelNode1->worldTransform).translation.y = (int)(sinCosStep >> 0x20) + stateValue;
      (modelNode1->modelPayload).worldRotationAngle2 = headingOrDelta;
      durationValue = currentDefinition->travelStepCountC8;
      (modelRuntime->classLinkState).armyLinkOrState6C.classState = durationValue;
      stateValue = durationValue * 2;
      stepValue = currentDefinition->movementStepQ12_0C;
      (modelRuntime->classLinkState).classState68 = stateValue;
      (modelRuntime->classLinkState).classState7C = -durationValue * stepValue;
      (modelRuntime->class21State84).classStateB8 = 5;
      sinCosStep = FixedMath_SinCosScaled(headingOrDelta,currentDefinition->movementStepQ12_0C * 10);
      stepValue = (modelNode1->worldTransform).translation.x;
      sampleCoord = (modelNode1->worldTransform).translation.y;
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
  case 5:
    sinCosStep = FixedMath_SinCosScaled
                       ((modelNode1->modelPayload).worldRotationAngle2,
                        g_InGameSimulationStepTicks * currentDefinition->movementStepQ12_0C);
    translationVec = &(modelNode1->worldTransform).translation;
    translationVec->x = translationVec->x + (int)sinCosStep;
    translationY = &(modelNode1->worldTransform).translation.y;
    *translationY = *translationY + (int)(sinCosStep >> 0x20);
    terrainHeight = FieldGrid_InterpolateTopSurfaceHeight
                       ((modelNode1->worldTransform).translation.y,
                        (modelNode1->worldTransform).translation.x,worldRuntime->fieldGrid);
    classUpdate21Definition = modelRuntime->modelDefinition;
    stateValue = (modelRuntime->classLinkState).classState7C;
    product64 = (longlong)(int)stateValue * (longlong)(int)stateValue;
    modelNode1 = modelNode1->childNodes[0];
    product64 = (longlong)
             (int)(classUpdate21Definition->verticalArcCoefficientCC * g_InGameSimulationStepTicks)
             * (longlong)(int)((int)((ulonglong)product64 >> 0x20) << 0x14 | (uint)product64 >> 0xc);
    (modelNode1->modelPayload).localRotationAngle0 = 0x8000;
    stateValue = (modelRuntime->classLinkState).classState7C;
    (modelNode1->modelPayload).localTranslationZQ12 =
         (((int)((ulonglong)product64 >> 0x20) << 0x14 | (uint)product64 >> 0xc) +
         (modelRuntime->class21State84).trajectoryTerrainReferenceHeightQ12_84) - terrainHeight.heightQ12;
    product64 = (longlong)(int)(stateValue * g_InGameSimulationStepTicks) *
             (longlong)classUpdate21Definition->verticalArcCoefficientCC;
    stateValue = FixedMath_Atan2Angle16
                       ((int)((ulonglong)product64 >> 0x20) << 0x15 | (uint)product64 >> 0xb,0x1000);
    stateField = &(modelRuntime->classLinkState).classState7C;
    *stateField = *stateField + classUpdate21Definition->movementStepQ12_0C * g_InGameSimulationStepTicks;
    (modelNode1->modelPayload).localRotationAngle1 = 0x4000 - stateValue & 0xffff;
    modelNode1 = modelRuntime->rootModelNode;
    remainingTicks = g_InGameSimulationStepTicks;
    do {
      armyLinkState = &(modelRuntime->classLinkState).armyLinkOrState6C;
      armyLinkState->armyRuntime = (ArmyRuntimeSlot *)(armyLinkState->classState - 1);
      stateField = &(modelRuntime->classLinkState).classState68;
      *stateField = *stateField - 1;
      if ((int)*stateField < 0) {
        (modelNode1->worldTransform).translation.x = -0x100000;
        (modelNode1->worldTransform).translation.y = 0x100000;
        ((ModelRuntimeSlotClassState7C *)&modelRuntime->class21State84)->classStateB8 = 6;
      }
      modelPointSource = classUpdate21Definition->modelPointSource64;
      savedClassUpdate21Definition = classUpdate21Definition;
      modelNode2 = modelNode1;
      savedModelRuntime = modelRuntime;
      ArmyRuntime_TryPlayMappedTerrainSoundAtWorldPoint
                (modelRuntime->ownerArmyRuntime->factionIndex,
                 (modelNode1->worldTransform).translation.y,
                 (modelNode1->worldTransform).translation.x,
                 classUpdate21Definition->terrainSoundAssetIndex26C,worldRuntime);
      childNode = modelNode1->childNodes[0];
      stepValue = classUpdate21Definition->modelPointStep30 * -3;
      sampleCoord = (childNode->worldTransform).translation.z + -0x2000;
      modelNode1 = modelNode2;
      if (classUpdate21Definition->modelPointStep30 != 0) {
        modelPointSource = *(void **)((int)modelPointSource + 0x18);
        if (stepValue - (modelRuntime->classLinkState).armyLinkOrState6C.classState == 0) {
          ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
                    (0,sampleCoord,(childNode->worldTransform).translation.y,
                     (childNode->worldTransform).translation.x,7,
                     classUpdate21Definition->modelPointEffectId2C,childNode,modelPointSource,worldRuntime);
          modelNode1 = modelNode2;
        }
        else {
          armySlot2 = (ArmyRuntimeSlot *)(stepValue + classUpdate21Definition->modelPointStep30);
          if (armySlot2 == (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime) {
            ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
                      (0,sampleCoord,(childNode->worldTransform).translation.y,
                       (childNode->worldTransform).translation.x,6,
                       classUpdate21Definition->modelPointEffectId2C,childNode,modelPointSource,worldRuntime);
            modelNode1 = modelNode2;
          }
          else {
            armySlot2 = (ArmyRuntimeSlot *)
                        ((int)&armySlot2->modelRuntimeOrSavedOffset +
                        classUpdate21Definition->modelPointStep30);
            if (armySlot2 == (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime) {
              ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
                        (0,sampleCoord,(childNode->worldTransform).translation.y,
                         (childNode->worldTransform).translation.x,5,
                         classUpdate21Definition->modelPointEffectId2C,childNode,modelPointSource,worldRuntime);
              modelNode1 = modelNode2;
            }
            else {
              armySlot2 = (ArmyRuntimeSlot *)
                          ((int)&armySlot2->modelRuntimeOrSavedOffset +
                          classUpdate21Definition->modelPointStep30);
              if (armySlot2 == (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime) {
                ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
                          (0,sampleCoord,(childNode->worldTransform).translation.y,
                           (childNode->worldTransform).translation.x,4,
                           classUpdate21Definition->modelPointEffectId2C,childNode,modelPointSource,worldRuntime)
                ;
                modelNode1 = modelNode2;
              }
              else {
                armySlot2 = (ArmyRuntimeSlot *)
                            ((int)&armySlot2->modelRuntimeOrSavedOffset +
                            classUpdate21Definition->modelPointStep30);
                if (armySlot2 == (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime) {
                  ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
                            (0,sampleCoord,(childNode->worldTransform).translation.y,
                             (childNode->worldTransform).translation.x,3,
                             classUpdate21Definition->modelPointEffectId2C,childNode,modelPointSource,
                             worldRuntime);
                  modelNode1 = modelNode2;
                }
                else {
                  armySlot2 = (ArmyRuntimeSlot *)
                              ((int)&armySlot2->modelRuntimeOrSavedOffset +
                              classUpdate21Definition->modelPointStep30);
                  if (armySlot2 == (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime) {
                    ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
                              (0,sampleCoord,(childNode->worldTransform).translation.y,
                               (childNode->worldTransform).translation.x,2,
                               classUpdate21Definition->modelPointEffectId2C,childNode,modelPointSource,
                               worldRuntime);
                    modelNode1 = modelNode2;
                  }
                  else if ((ArmyRuntimeSlot *)
                           ((int)&armySlot2->modelRuntimeOrSavedOffset +
                           classUpdate21Definition->modelPointStep30) ==
                           (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime) {
                    ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
                              (0,sampleCoord,(childNode->worldTransform).translation.y,
                               (childNode->worldTransform).translation.x,1,
                               classUpdate21Definition->modelPointEffectId2C,childNode,modelPointSource,
                               worldRuntime);
                    modelNode1 = modelNode2;
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
  case 6:
    if (armyRuntime != (ModelRuntimeSlot *)0x0) {
      modelNode1 = (armyRuntime->rootModelNodeOrSavedOffset).modelNode;
      if (((armyRuntime->classState).classStateB0 == 0) &&
         (pointAllowed = ArmyRuntime_TestWorldPointAllowedDefaultCf
                             (currentDefinition->worldPointAllowedContext48,
                              (modelNode1->worldTransform).translation.y,
                              (modelNode1->worldTransform).translation.x), !pointAllowed)) {
        (armyRuntime->classState).classStateB0 = 1;
        ArmyRuntime_TrySpawnDefinitionEffectAtWorldPoint
                  ((ArmyRuntimeSlot *)armyRuntime,worldRuntime);
        headingOrDelta = (modelNode1->modelPayload).worldRotationAngle2;
        sinCosStep = FixedMath_SinCosScaled
                           (headingOrDelta ^ 0x8000,currentDefinition->movementStepQ12_0C * currentDefinition->phaseDurationC4);
        stepValue = (modelNode1->worldTransform).translation.y;
        childNode = modelRuntime->rootModelNode;
        (childNode->worldTransform).translation.x =
             (int)sinCosStep + (modelNode1->worldTransform).translation.x;
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
  case 0:
    if (armyRuntime == (ModelRuntimeSlot *)0x0) {
      childHeightZ = 0x1000;
    }
    else {
      childHeightZ = (((armyRuntime->rootModelNodeOrSavedOffset).modelNode)->childNodes[0]->modelPayload).
               localTranslationZQ12;
    }
    (modelNode1->childNodes[0]->modelPayload).localTranslationZQ12 = childHeightZ;
  }
  modelNode1 = modelRuntime->rootModelNode;
  savedAngle0 = (modelNode1->modelPayload).worldRotationAngle0;
  savedAngle1 = (modelNode1->modelPayload).worldRotationAngle1;
  (*g_ArmyPlacementContactKindDispatchTable.callbacks
    [((ModelDefinitionRuntimeSemanticView280 *)modelRuntime->modelDefinition)->
     placementContactKindIndex278])
            (((ModelDefinitionRuntimeSemanticView280 *)modelRuntime->modelDefinition)->
             placementHeightOffsetQ12,(modelNode1->worldTransform).translation.y,
             (modelNode1->worldTransform).translation.x,modelNode1,worldRuntime);
  stateValue = ((ModelRuntimeSlotClassState7C *)&modelRuntime->class21State84)->classStateB8;
  (modelNode1->modelPayload).worldRotationAngle1 = savedAngle1;
  (modelNode1->modelPayload).worldRotationAngle0 = savedAngle0;
  if ((stateValue != 0) && (stateValue != 1)) {
    ArmyRuntime_UpdateTimedShotAndEffectEmitters
              (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
  }
  semanticDefinition = (ModelDefinitionRuntimeSemanticView280 *)modelRuntime->modelDefinition;
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNode1);
  ModelNodeRuntime_RecomputeSubtreeBoundingRadius(modelNode1);
  ModelNodeRuntime_UpdateDepthBinMasks(semanticDefinition->placementRadiusOrClearanceDC,modelNode1);
  return;
}


/* Address: 0x00526620.
   Ownership: gameplay/army/runtime.
   Purpose: Runtime-update slot 22 owns only the exact non-overlapping dispatcher prefix. Its second switch tail-
   dispatches into the established ArmyRuntimeCallbackTable00526990Ownership function at 005269C0 and interior
   entries of that sealed ownership container. The dispatcher retains the common void __stdcall two-argument table
   ABI and purge 8 contract; no false RET is assigned to the prefix. Updates linked model hierarchy flags when the
   runtime transition bit is active, then dispatches the sealed terrain-contact mode through the existing ownership
   container. Runtime-update partition slots 0-23 receive (worldRuntime, armyRuntime).
   Local calls: ArmyRuntime_UpdateTimedShotAndEffectEmitters, ArmyRuntime_UpdateAnimatedModelSubnodes,
   ArmyRuntime_TestWorldPointAllowedDefaultCf, ArmyRuntimeSpawner_CreateLinkedChildInstanceCf.
   Cross-module calls: ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive [world/model/hierarchy],
   ArmyAssetRegistry_FindByIdCf [assets/army/catalog], ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
   [assets/model/definitions], InGameNotificationQueue_InsertPriorityRecord [ui/ingame/runtime],
   TerrainGrid_TestProjectedCellMaskBits01Cf [world/terrain/grid], SpatialSound_PlayPositionedOneShot
   [audio/spatial/runtime].
*/

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_UpdateLinkedModelFlagsAndDispatchTerrainContactMode
          (WorldRuntimeContext *worldRuntime,
          ModelRuntimeLinkedChildSpawnAndBuildView200 *modelRuntime)

{
  FactionArmyAssetCount *assetCountField;
  FactionRelationCounter *relationCounter;
  byte *pendingSpawnCount;
  Q12 *childTranslationZ;
  int *ownerPayload;
  dword candidateAsset;
  dword selectedAssetValue;
  int factionIndexOrLimit;
  uint completedAssetValue;
  PckArmyAssetIdCatalog secondaryAssetId;
  ModelRuntimeNode *modelNodeRuntime;
  DirectSoundVoiceSet **soundVoiceSet;
  ModelRuntimeNode *childNode;
  ModelDefinitionRecordPrefix *modelDefinition1;
  FactionArmyAssetCount linkedChildUsedSlotCount;
  ArmyBuildXeniteCostQ4 secondaryArmyBuildXeniteCostQ4;
  FactionArmyAssetCount remainingAssetCount;
  int linkedChildReverseSlotIndex;
  int reverseSlotIndex;
  uint tickOrSoundIndex;
  WorldRuntimeNode *ownerNodeCursor;
  dword *dwordCursor;
  bool testResult;
  ArmyRegistryEaxCf5_51b6d0 assetLookup;
  ModelDefinitionLookupEaxCf5 definitionLookup;
  ModelLookupPayloadEaxEcxEdxCf13 lookupPayload;
  Q12 translationStep;
  PckModelDefinitionIdCatalog notificationMovieId;
  ModelDefinitionLinkedChildStateView280 *linkedChildDefinition;
  int *modelRuntimePayloadWords;
  ArmyRuntimeSlot *entityRuntime1;
  
  if ((modelRuntime->linkedChildRuntimeFlagsEC & 0x10) != 0) {
    ownerNodeCursor = (WorldRuntimeNode *)worldRuntime->ownerListHead;
    do {
      if ((((ownerNodeCursor[2].common.nextNode == (WorldRuntimeNode *)0x0) &&
           (ownerPayload = ownerNodeCursor->runtimePayload, *(int *)(*ownerPayload + 0x4c) == 0x15)) &&
          (modelRuntime == (ModelRuntimeLinkedChildSpawnAndBuildView200 *)ownerPayload[0x18])) &&
         ((ownerPayload[0x3b] & 0x10U) == 0)) {
        ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive(worldRuntime,(int *)ownerPayload[2]);
      }
      ownerNodeCursor = (ownerNodeCursor->common).nextNode;
    } while (ownerNodeCursor != (WorldRuntimeNode *)0x0);
  }
                    // WARNING: Switch is manually overridden
  switch(modelRuntime->secondaryArmyAssetBuildStateAC) {
  case 0:
    if ((modelRuntime->linkedChildRuntimeFlagsEC & 0x40) == 0) {
                    // [V427CI_ARMY_C4_SCALAR_ARTIFACT] Immutable 00526708: CMP ECX,[EAX+0xC4]; ECX
                    // and linkedChildSlotCountC4 are FactionArmyAssetCount scalars. Any
                    // GraphicsFixedVec3* cast here is a symbol-less decompiler presentation
                    // artifact.
      if (((modelRuntime->linkedChildRuntimeFlagsEC & 0xc9) == 0) &&
         (factionIndexOrLimit = ((modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime)->factionIndex,
         (modelRuntime->linkedChildBuildState60).completedSecondaryArmyAssetCount6C <
         modelRuntime->modelDefinition->linkedChildSlotCapacityC4)) {
        remainingAssetCount = g_GameFactionRuntimeImage.records[factionIndexOrLimit].secondaryArmyAssetCount;
        dwordCursor = g_GameFactionRuntimeImage.records[factionIndexOrLimit].secondaryArmyAssetPointersOrIds;
        if (remainingAssetCount != 0) {
ArmyRuntimeClass_SelectAffordableSecondaryArmyAssetLoop:
          candidateAsset = *dwordCursor;
                    // [V427CI_ARMY_XENITE_SCALAR_ARTIFACT] Immutable 00526745: CMP
                    // ECX,[EDX+0x50F340]; ECX is the +0x28 secondary Army Xenite cost and memory is
                    // faction xeniteCurrentQ4. Both are scalar resource values; pointer casts are
                    // presentation-only.
          if (((*(uint *)(candidateAsset + 0x14) & 8) == 0) ||
             (g_GameFactionRuntimeImage.records[factionIndexOrLimit].xeniteCurrentQ4 < *(uint *)(candidateAsset + 0x28)))
          goto ArmyRuntimeClass_AdvanceSecondaryArmyAssetCandidate;
          g_GameFactionRuntimeImage.records[factionIndexOrLimit].xeniteCurrentQ4 =
               g_GameFactionRuntimeImage.records[factionIndexOrLimit].xeniteCurrentQ4 - *(uint *)(candidateAsset + 0x28);
          tickOrSoundIndex = *(uint *)(candidateAsset + 0x24);
          selectedAssetValue = *(dword *)(candidateAsset + 0x2c);
          if ((g_UiCommandRuntimeFlags & 0x100000) != 0) {
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
          modelRuntime->linkedChildRuntimeFlagsEC = modelRuntime->linkedChildRuntimeFlagsEC | 0x100;
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
      entityRuntime1 = (modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime;
      ArmyRuntime_UpdateTimedShotAndEffectEmitters
                (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
      tickOrSoundIndex = (modelRuntime->linkedChildBuildState60).secondaryArmyAssetBuildElapsedTicks64;
      ArmyRuntime_UpdateAnimatedModelSubnodes
                (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
      if ((modelRuntime->linkedChildBuildState60).secondaryArmyAssetBuildRequiredTicks68 <= tickOrSoundIndex)
      {
        factionIndexOrLimit = entityRuntime1->factionIndex;
        completedAssetValue = (modelRuntime->linkedChildBuildState60).selectedSecondaryArmyAssetValue74;
        (modelRuntime->linkedChildBuildState60).selectedSecondaryArmyAssetValue74 = 0;
        modelRuntime->secondaryArmyAssetBuildStateAC = 0;
        modelRuntime->linkedChildRuntimeFlagsEC =
             modelRuntime->linkedChildRuntimeFlagsEC & 0xfffffeff;
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
                    // [V427CI_ARMY_REVERSE_INDEX_ARTIFACT] Immutable 005268DA: DEC ECX; 005268DB:
                    // JNS 005268D0. ECX is a signed reverse linked-child slot index. The persistent
                    // DB phase local linkedChildReverseSlotIndex is int; pointer-shaped
                    // HighVariable recovery at this site is non-authoritative unless it maps to a
                    // pointer-typed persistent Function local.
            reverseSlotIndex = reverseSlotIndex + -1;
          } while (-1 < reverseSlotIndex);
          reverseSlotIndex = 0;
ArmyRuntimeClass_StoreCompletedSecondaryArmyAssetId:
          modelRuntime->completedSecondaryArmyAssetIds78[reverseSlotIndex] = secondaryAssetId;
          relationCounter = &g_GameFactionRuntimeImage.records[factionIndexOrLimit].relationCounterA;
          *relationCounter = *relationCounter + 1;
          assetCountField = &(modelRuntime->linkedChildBuildState60).completedSecondaryArmyAssetCount6C;
          *assetCountField = *assetCountField + 1;
          dwordCursor = &(modelRuntime->linkedChildBuildState60).classState70;
          *dwordCursor = *dwordCursor + 1;
          if (factionIndexOrLimit == worldRuntime->activeFactionRuntimeIndex) {
            assetLookup = ArmyAssetRegistry_FindByIdCf(secondaryAssetId);
            definitionLookup = ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                               (entityRuntime1->factionIndex,(assetLookup.eax)->rootNodeOffsetOrPointer);
            modelDefinition1 = definitionLookup.modelDefinition;
            modelDefinition1[0x24].byteSize = modelDefinition1[0x24].byteSize + 1;
            notificationMovieId = modelDefinition1[0x1d].flags;
            if (modelDefinition1[0x24].byteSize != 1) {
              notificationMovieId = modelDefinition1[0x1d].definitionId;
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
                    // WARNING: Switch is manually overridden
  switch(modelRuntime->linkedChildTransitionStateB0) {
  case 0:
    goto ArmyRuntimeClass_ProcessPendingLinkedChildSpawnsAndDamageEffect;
  case 1:
    modelNodeRuntime->primaryTextureOffsetV =
         modelNodeRuntime->primaryTextureOffsetV +
         linkedChildDefinition->linkedChildTextureVStepPerTick0C * g_InGameSimulationStepTicks;
    if (0x7ffff < modelNodeRuntime->primaryTextureOffsetV) {
      modelNodeRuntime->primaryTextureOffsetV = 0x80000;
      modelRuntime->linkedChildTransitionStateB0 = 2;
      tickOrSoundIndex = linkedChildDefinition->linkedChildTransitionSoundAssetIndex270;
      if (((tickOrSoundIndex != 0) && (tickOrSoundIndex < worldRuntime->dwordArrayCount)) &&
         (worldRuntime->dwordArray != (dword *)0x0)) {
        soundVoiceSet = (DirectSoundVoiceSet **)worldRuntime->dwordArray[tickOrSoundIndex];
        if ((soundVoiceSet != (DirectSoundVoiceSet **)0x0) &&
           (testResult = TerrainGrid_TestProjectedCellMaskBits01Cf
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
  case 2:
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
  case 4:
    translationStep = linkedChildDefinition->linkedChildTranslationStepQ12PerTickC8;
    lookupPayload = ModelLookupTable_FindPackedKeyEntryRegsCf
                       (0,1,(modelNodeRuntime->modelPayload).modelResource);
    childNode = modelNodeRuntime->childNodes[0];
    childTranslationZ = &(childNode->modelPayload).localTranslationZQ12;
    *childTranslationZ = *childTranslationZ - translationStep * g_InGameSimulationStepTicks;
    if ((childNode->modelPayload).localTranslationZQ12 < (int)lookupPayload.payloadEdx) {
      modelRuntime->linkedChildTransitionStateB0 = 5;
      (childNode->modelPayload).localTranslationZQ12 = lookupPayload.payloadEdx;
    }
    ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
    if (modelRuntime->linkedChildTransitionStateB0 == 5) {
      tickOrSoundIndex = linkedChildDefinition->linkedChildTransitionSoundAssetIndex26C;
      if (((tickOrSoundIndex != 0) && (tickOrSoundIndex < worldRuntime->dwordArrayCount)) &&
         (worldRuntime->dwordArray != (dword *)0x0)) {
        soundVoiceSet = (DirectSoundVoiceSet **)worldRuntime->dwordArray[tickOrSoundIndex];
        if ((soundVoiceSet != (DirectSoundVoiceSet **)0x0) &&
           (testResult = TerrainGrid_TestProjectedCellMaskBits01Cf
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
  case 5:
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
       (testResult = ArmyRuntime_TestWorldPointAllowedDefaultCf
                           (linkedChildDefinition->runtimeValue48,(modelNodeRuntime->worldTransform).translation.y
                            ,(modelNodeRuntime->worldTransform).translation.x), !testResult)) {
      if ((modelRuntime->linkedChildPendingSpawnCountsDC).slot0 != 0) {
        (modelRuntime->linkedChildPendingSpawnCountsDC).slot0 =
             (modelRuntime->linkedChildPendingSpawnCountsDC).slot0 - 1;
        testResult = ArmyRuntimeSpawner_CreateLinkedChildInstanceCf
                           (modelRuntime->linkedChildSpawnInheritedStateB8[0].inheritedValue78,
                            modelRuntime->linkedChildSpawnInheritedStateB8[0].inheritedValue74,
                            modelRuntime->linkedChildSpawnInheritedStateB8[0].inheritedValue70,
                            g_ArmyLinkedChildAssetIdSlot0,worldRuntime,
                            (ArmyRuntimeLinkedChildMaskSlotView *)modelRuntime);
        if (!testResult) {
          modelRuntime->linkedChildTransitionStateB0 = 1;
          tickOrSoundIndex = linkedChildDefinition->linkedChildTransitionSoundAssetIndex26C;
          if (((tickOrSoundIndex != 0) && (tickOrSoundIndex < worldRuntime->dwordArrayCount)) &&
             (worldRuntime->dwordArray != (dword *)0x0)) {
            soundVoiceSet = (DirectSoundVoiceSet **)worldRuntime->dwordArray[tickOrSoundIndex];
            if ((soundVoiceSet != (DirectSoundVoiceSet **)0x0) &&
               (testResult = TerrainGrid_TestProjectedCellMaskBits01Cf
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
        testResult = ArmyRuntimeSpawner_CreateLinkedChildInstanceCf
                           (modelRuntime->linkedChildSpawnInheritedStateB8[1].inheritedValue78,
                            modelRuntime->linkedChildSpawnInheritedStateB8[1].inheritedValue74,
                            modelRuntime->linkedChildSpawnInheritedStateB8[1].inheritedValue70,
                            g_ArmyLinkedChildAssetIdSlot1,worldRuntime,
                            (ArmyRuntimeLinkedChildMaskSlotView *)modelRuntime);
        if (!testResult) {
          modelRuntime->linkedChildTransitionStateB0 = 1;
          tickOrSoundIndex = linkedChildDefinition->linkedChildTransitionSoundAssetIndex26C;
          if (((tickOrSoundIndex != 0) && (tickOrSoundIndex < worldRuntime->dwordArrayCount)) &&
             (worldRuntime->dwordArray != (dword *)0x0)) {
            soundVoiceSet = (DirectSoundVoiceSet **)worldRuntime->dwordArray[tickOrSoundIndex];
            if ((soundVoiceSet != (DirectSoundVoiceSet **)0x0) &&
               (testResult = TerrainGrid_TestProjectedCellMaskBits01Cf
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
        testResult = ArmyRuntimeSpawner_CreateLinkedChildInstanceCf
                           (modelRuntime->linkedChildSpawnInheritedStateB8[2].inheritedValue78,
                            modelRuntime->linkedChildSpawnInheritedStateB8[2].inheritedValue74,
                            modelRuntime->linkedChildSpawnInheritedStateB8[2].inheritedValue70,
                            g_ArmyLinkedChildAssetIdSlot2,worldRuntime,
                            (ArmyRuntimeLinkedChildMaskSlotView *)modelRuntime);
        if (!testResult) {
          modelRuntime->linkedChildTransitionStateB0 = 1;
          tickOrSoundIndex = linkedChildDefinition->linkedChildTransitionSoundAssetIndex26C;
          if (((tickOrSoundIndex != 0) && (tickOrSoundIndex < worldRuntime->dwordArrayCount)) &&
             (worldRuntime->dwordArray != (dword *)0x0)) {
            soundVoiceSet = (DirectSoundVoiceSet **)worldRuntime->dwordArray[tickOrSoundIndex];
            if ((soundVoiceSet != (DirectSoundVoiceSet **)0x0) &&
               (testResult = TerrainGrid_TestProjectedCellMaskBits01Cf
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
   Ownership: gameplay/army/runtime.
   Purpose: This function object claims Listing ownership for a previously unowned multi-entry/shared-
   tail/computed-dispatch region; it does not assert that every member entry is an independent ABI-level function.
   Runtime-update partition slots 0-23 receive (worldRuntime, armyRuntime). Exact disjoint ranges, terminal
   instructions, inherited register state, shared exits, and caller/table references were revalidated. No function
   splitting or boundary change is permitted.
   Local calls: ArmyRuntime_UpdateTimedShotAndEffectEmitters, ArmyRuntime_UpdateAnimatedModelSubnodes,
   ArmyRuntime_CreateInstanceFromAssetCf.
   Cross-module calls: ModelLookupTable_ContainsPackedKeyCf [assets/model/definitions],
   ModelNodeRuntime_TransformLocalPointRegs [world/model/hierarchy], WorldRuntime_UnlinkNodeFromOwnerListD8
   [world/runtime/core], FixedMath_Atan2Angle16 [core/math/fixed], TerrainGrid_TestProjectedCellMaskBits01Cf
   [world/terrain/grid], SpatialSound_PlayPositionedOneShot [audio/spatial/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClassUpdateSlot13_PrepareModelAndDispatchByClassId
          (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView200 *modelRuntime)

{
  byte *reservedFlags;
  FactionRelationCounter *relationCounter;
  dword *derivedValue;
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
  dword stateValue;
  ArmyRuntimeSlot *armySlot1;
  Q12 targetWorldXQ12;
  dword secondaryValue;
  Q12 worldXQ12;
  GraphicsFixedVec3 *translationVec;
  FactionArmyAssetCount remainingAssetCount;
  Q12 worldYQ12;
  uint tickOrSoundIndex;
  dword *dwordCursor;
  bool cellMasked;
  ModelLookupEntryEaxCf5 lookupEntry;
  ArmyRuntimeCreateEaxCf5 createResult;
  ModelLocalPointRegs12 localPoint;
  InGameNotificationMovieId notificationMovieId;
  ArmyRuntimeSlot *entityRuntime1;
  
  rootNode = modelRuntime->rootModelNode;
  if ((*(uint *)(modelRuntime->classState).reservedBC_BF & 1) != 0) {
    lookupEntry = ModelLookupTable_ContainsPackedKeyCf(1,5,(rootNode->modelPayload).modelResource);
    if (!lookupEntry.carry) {
      localPoint = ModelNodeRuntime_TransformLocalPointRegs(lookupEntry.entry,rootNode);
      (modelRuntime->classLinkState).classState78 = localPoint.eax;
      (modelRuntime->classLinkState).classState7C = localPoint.ecx;
      reservedFlags = (modelRuntime->classState).reservedBC_BF;
      *(uint *)reservedFlags = *(uint *)reservedFlags & 0xfffffffe;
    }
  }
  stateValue = (modelRuntime->classState).classStateB8;
  semanticDefinition = modelRuntime->modelDefinition;
  if ((3 < rootNode->childCount) && (rootNode->childNodes[3] != (ModelRuntimeNode *)0x0)) {
    WorldRuntime_UnlinkNodeFromOwnerListD8((WorldOwnerListNode100 *)rootNode->childNodes[3]);
    rootNode->childNodes[3] = (ModelRuntimeNode *)0x0;
  }
                    // WARNING: Switch is manually overridden
  switch(stateValue) {
  case 0:
    if (((modelRuntime->classState).classStateEC & 0x40) == 0) {
      if (((modelRuntime->classState).classStateEC & 0xc9) == 0) {
        factionOrNodeValue = modelRuntime->ownerArmyRuntime->factionIndex;
        remainingAssetCount = g_GameFactionRuntimeImage.records[factionOrNodeValue].secondaryArmyAssetCount;
        dwordCursor = g_GameFactionRuntimeImage.records[factionOrNodeValue].secondaryArmyAssetPointersOrIds;
        if (remainingAssetCount != 0) {
          do {
            if (((*(uint *)(*dwordCursor + 0x14) &
                 *(uint *)(modelRuntime->modelDefinition->reserved0C0_0DB + 4)) != 0) &&
               (tickOrSoundIndex = *(uint *)(*dwordCursor + 0x28),
               tickOrSoundIndex <= g_GameFactionRuntimeImage.records[factionOrNodeValue].xeniteCurrentQ4)) {
              stateValue = *dwordCursor;
              g_GameFactionRuntimeImage.records[factionOrNodeValue].xeniteCurrentQ4 =
                   g_GameFactionRuntimeImage.records[factionOrNodeValue].xeniteCurrentQ4 - tickOrSoundIndex;
              tickOrSoundIndex = *(uint *)(stateValue + 0x24);
              secondaryValue = *(dword *)(stateValue + 0x2c);
              if ((g_UiCommandRuntimeFlags & 0x100000) != 0) {
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
              *dwordCursor = *dwordCursor | 0x100;
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
  case 1:
    if (((modelRuntime->classState).classStateEC & 9) == 0) {
      dwordCursor = &(modelRuntime->classLinkState).classState64;
      *dwordCursor = *dwordCursor + g_InGameSimulationStepTicks;
      ArmyRuntime_UpdateTimedShotAndEffectEmitters(worldRuntime,modelRuntime);
      tickOrSoundIndex = (modelRuntime->classLinkState).classState64;
      ArmyRuntime_UpdateAnimatedModelSubnodes(worldRuntime,modelRuntime);
      if ((modelRuntime->classLinkState).classState68 <= tickOrSoundIndex) {
        lookupEntry = ModelLookupTable_ContainsPackedKeyCf(1,5,(rootNode->modelPayload).modelResource);
        if (!lookupEntry.carry) {
          localPoint = ModelNodeRuntime_TransformLocalPointRegs(lookupEntry.entry,rootNode);
          secondaryValue = localPoint.ecx;
          stateValue = localPoint.eax;
          lookupEntry = ModelLookupTable_ContainsPackedKeyCf(0,5,(rootNode->modelPayload).modelResource);
          if (!lookupEntry.carry) {
            localPoint = ModelNodeRuntime_TransformLocalPointRegs(lookupEntry.entry,rootNode);
            stateValue = FixedMath_Atan2Angle16(secondaryValue - localPoint.ecx,stateValue - localPoint.eax);
            entityRuntime1 = modelRuntime->ownerArmyRuntime;
            createResult = ArmyRuntime_CreateInstanceFromAssetCf
                               (4,stateValue,localPoint.ecx,localPoint.eax,entityRuntime1->factionIndex,
                                (modelRuntime->classLinkState).modelLinkOrState60.classState,
                                worldRuntime);
            armySlot1 = (ArmyRuntimeSlot *)createResult.eax;
            if (!createResult.carry) {
              relationCounter = &g_GameFactionRuntimeImage.records[entityRuntime1->factionIndex].
                        relationCounterA;
              *relationCounter = *relationCounter + 1;
              semanticDefinition = modelRuntime->modelDefinition;
              armySlot1->movementStateFlags = armySlot1->movementStateFlags | 0x402;
              tickOrSoundIndex = *(uint *)semanticDefinition->reserved26C_277;
              if (((tickOrSoundIndex != 0) && (tickOrSoundIndex < worldRuntime->dwordArrayCount)) &&
                 (worldRuntime->dwordArray != (dword *)0x0)) {
                rootNode = modelRuntime->rootModelNode;
                soundVoiceSet = *(DirectSoundVoiceSet ***)
                            ((rootNode->modelPayload).reserved2C_33 + tickOrSoundIndex * 4 + -0x38);
                translationVec = &(rootNode->worldTransform).translation;
                if ((soundVoiceSet != (DirectSoundVoiceSet **)0x0) &&
                   (cellMasked = TerrainGrid_TestProjectedCellMaskBits01Cf
                                       ((rootNode->worldTransform).translation.y,translationVec->x,
                                        worldRuntime), !cellMasked)) {
                  SpatialSound_PlayPositionedOneShot
                            (semanticDefinition->positionedSoundMaximumDistanceQ12,
                             semanticDefinition->positionedSoundGainQ15,translationVec,soundVoiceSet);
                }
              }
              stateValue = (modelRuntime->classLinkState).classState74;
              (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime = armySlot1;
              factionOrNodeValue = armySlot1->factionIndex;
              (modelRuntime->classState).classStateB8 = 2;
              (modelRuntime->classLinkState).classState74 = 0;
              dwordCursor = &(modelRuntime->classState).definitionDerivedValueF4;
              *dwordCursor = *dwordCursor - stateValue;
              linkedPayload = (int *)(armySlot1->modelRuntimeOrSavedOffset).savedIdOrOffset;
              armySlot1->movementStateFlags = armySlot1->movementStateFlags | 2;
              linkedPayload[0x3c] = (int)modelRuntime;
              if (factionOrNodeValue == worldRuntime->activeFactionRuntimeIndex) {
                ArmyAssetRegistry_FindByIdCf
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
  case 2:
    rootNode->primaryTextureOffsetV =
         rootNode->primaryTextureOffsetV +
         *(int *)semanticDefinition->reserved00C_023 * g_InGameSimulationStepTicks;
    if (0x7ffff < rootNode->primaryTextureOffsetV) {
      rootNode->primaryTextureOffsetV = 0x80000;
      (modelRuntime->classState).classStateB8 = 3;
      lookupEntry = ModelLookupTable_ContainsPackedKeyCf(1,5,(rootNode->modelPayload).modelResource);
      if (!lookupEntry.carry) {
        entityRuntime1 = (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime;
        localPoint = ModelNodeRuntime_TransformLocalPointRegs(lookupEntry.entry,rootNode);
        targetWorldXQ12 = localPoint.ecx;
        linkedModelRuntime = (entityRuntime1->modelRuntimeOrSavedOffset).modelRuntime;
        ArmyRuntime_StartMoveCommandWithAuxiliaryValues
                  ((modelRuntime->classLinkState).classState7C,
                   (modelRuntime->classLinkState).classState78,targetWorldXQ12,localPoint.eax,
                   (ArmyMovementRuntime *)entityRuntime1);
        (linkedModelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime =
             (ArmyRuntimeSlot *)modelRuntime;
      }
    }
    break;
  case 3:
    entityRuntime1 = (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime;
    if ((entityRuntime1 == (ArmyRuntimeSlot *)0x0) ||
       ((ModelRuntimeUpdateView200 *)
        (((entityRuntime1->modelRuntimeOrSavedOffset).modelRuntime)->classState).
        linkedArmyRuntimeOrSavedOffset.armyRuntime != modelRuntime)) {
      semanticDefinition = modelRuntime->modelDefinition;
      (modelRuntime->classState).classStateB8 = 4;
      (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime = (ArmyRuntimeSlot *)0x0;
      tickOrSoundIndex = *(uint *)semanticDefinition->reserved26C_277;
      if ((tickOrSoundIndex != 0) &&
         ((tickOrSoundIndex < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != (dword *)0x0)))) {
        rootNode = modelRuntime->rootModelNode;
        soundVoiceSet = *(DirectSoundVoiceSet ***)
                    ((rootNode->modelPayload).reserved2C_33 + tickOrSoundIndex * 4 + -0x38);
        translationVec = &(rootNode->worldTransform).translation;
        if ((soundVoiceSet != (DirectSoundVoiceSet **)0x0) &&
           (cellMasked = TerrainGrid_TestProjectedCellMaskBits01Cf
                               ((rootNode->worldTransform).translation.y,translationVec->x,worldRuntime),
           !cellMasked)) {
          SpatialSound_PlayPositionedOneShot
                    (semanticDefinition->positionedSoundMaximumDistanceQ12,semanticDefinition->positionedSoundGainQ15,
                     translationVec,soundVoiceSet);
        }
      }
    }
    break;
  case 4:
    rootNode->primaryTextureOffsetV =
         rootNode->primaryTextureOffsetV -
         *(int *)semanticDefinition->reserved00C_023 * g_InGameSimulationStepTicks;
    if (rootNode->primaryTextureOffsetV < 1) {
      rootNode->primaryTextureOffsetV = 0;
      (modelRuntime->classState).classStateB8 = 0;
      dwordCursor = &(modelRuntime->classState).classStateEC;
      *dwordCursor = *dwordCursor & 0xfffffeff;
    }
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  return;
}


/* Address: 0x005240F0.
   Ownership: gameplay/army/runtime.
   Purpose: This function object claims Listing ownership for a previously unowned multi-entry/shared-
   tail/computed-dispatch region; it does not assert that every member entry is an independent ABI-level function.
   Runtime-update partition slots 0-23 receive (worldRuntime, armyRuntime). Exact disjoint ranges, terminal
   instructions, inherited register state, shared exits, and caller/table references were revalidated. No function
   splitting or boundary change is permitted. [RESOURCE_FUEL_ENERGY_CAPACITY_SEPARATION_CLOSURE] Production path:
   queue candidate +0x28 is paid once from faction Xenite stock; candidate +0x2C is added to the ArmyRuntime active
   Energy load for the production duration and removed on completion.
   Local calls: ArmyRuntime_UpdateTimedShotAndEffectEmitters, ArmyRuntime_UpdateAnimatedModelSubnodes.
   Cross-module calls: ArmyAssetRegistry_FindByIdCf [assets/army/catalog], UiCommandSpriteVariantA_RebuildGrid
   [ui/ingame/commands], ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf [assets/model/definitions],
   InGameNotificationQueue_InsertPriorityRecord [ui/ingame/runtime], ArmyRuntime_EmitDamageThresholdEffect
   [gameplay/army/combat].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClassUpdateSlot11_DispatchByClassId
          (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView200 *modelRuntime)

{
  dword *derivedValue;
  FactionArmyAssetCount *assetCountField;
  dword assetEnergyValue;
  ModelRuntimeSlotLinkOrState4 selectedAssetLink;
  int factionIndex;
  dword candidateValue;
  int activeFactionIndex;
  AngleTurn32 headingAngle;
  ModelDefinitionRecordPrefix *modelDefinition1;
  FactionArmyAssetCount remainingAssetCount;
  uint tickOrCount;
  dword *dwordCursor;
  ArmyRegistryEaxCf5_51b6d0 assetLookup;
  ModelDefinitionLookupEaxCf5 definitionLookup;
  PckModelDefinitionIdCatalog notificationMovieId;
  ArmyRuntimeSlot *entityRuntime1;
  ModelRuntimeNode *modelNode1;
  
                    // WARNING: Switch is manually overridden
  switch((modelRuntime->classState).classStateB8) {
  case 0:
    if (((modelRuntime->classState).classStateEC & 0x40) == 0) {
      if (((modelRuntime->classState).classStateEC & 0xc9) == 0) {
        factionIndex = modelRuntime->ownerArmyRuntime->factionIndex;
        dwordCursor = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetPointersOrIds;
        for (remainingAssetCount = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount; remainingAssetCount != 0;
            remainingAssetCount = remainingAssetCount - 1) {
          candidateValue = *dwordCursor;
          if (((*(uint *)(candidateValue + 0x14) & 0x10) != 0) &&
             (*(uint *)(candidateValue + 0x28) <= g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4))
          {
            g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 =
                 g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 - *(uint *)(candidateValue + 0x28);
            tickOrCount = *(uint *)(candidateValue + 0x24);
            assetEnergyValue = *(dword *)(candidateValue + 0x2c);
            if ((g_UiCommandRuntimeFlags & 0x100000) != 0) {
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
            *dwordCursor = *dwordCursor | 0x100;
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
  case 1:
    if (((modelRuntime->classState).classStateEC & 9) == 0) {
      entityRuntime1 = modelRuntime->ownerArmyRuntime;
      dwordCursor = &(modelRuntime->classLinkState).classState64;
      *dwordCursor = *dwordCursor + g_InGameSimulationStepTicks;
      ArmyRuntime_UpdateTimedShotAndEffectEmitters(worldRuntime,modelRuntime);
      tickOrCount = (modelRuntime->classLinkState).classState64;
      ArmyRuntime_UpdateAnimatedModelSubnodes(worldRuntime,modelRuntime);
      if ((modelRuntime->classLinkState).classState68 <= tickOrCount) {
        factionIndex = entityRuntime1->factionIndex;
        candidateValue = (modelRuntime->classLinkState).classState74;
        (modelRuntime->classLinkState).classState74 = 0;
        (modelRuntime->classState).classStateB8 = 0;
        dwordCursor = &(modelRuntime->classState).classStateEC;
        *dwordCursor = *dwordCursor & 0xfffffeff;
        dwordCursor = &(modelRuntime->classState).definitionDerivedValueF4;
        *dwordCursor = *dwordCursor - candidateValue;
        assetLookup = ArmyAssetRegistry_FindByIdCf
                           ((modelRuntime->classLinkState).modelLinkOrState60.classState);
        (modelRuntime->classLinkState).modelLinkOrState60.modelRuntime = (ModelRuntimeSlot *)0x0;
        if (!assetLookup.carry) {
          tickOrCount = g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
          if (tickOrCount < 0x40) {
            activeFactionIndex = worldRuntime->activeFactionRuntimeIndex;
            *(ArmyAssetRecordPrefix **)(factionIndex * 0x740 + THANDOR_ADDR(g_GameFactionRuntimeImage,0x1e0) + tickOrCount * 4) = assetLookup.eax;
            assetCountField = &g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
            *assetCountField = *assetCountField + 1;
            if (activeFactionIndex == entityRuntime1->factionIndex) {
              candidateValue = (assetLookup.eax)->rootNodeOffsetOrPointer;
              UiCommandSpriteVariantA_RebuildGrid((UiNodeBase *)worldRuntime);
              definitionLookup = ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                                 (entityRuntime1->factionIndex,candidateValue);
              modelDefinition1 = definitionLookup.modelDefinition;
              modelNode1 = modelRuntime->rootModelNode;
              candidateValue = (worldRuntime->motion).pitchAngle;
              headingAngle = (modelNode1->modelPayload).worldRotationAngle2;
              modelDefinition1[0x24].byteSize = modelDefinition1[0x24].byteSize + 1;
              notificationMovieId = modelDefinition1[0x1d].flags;
              if (modelDefinition1[0x24].byteSize != 1) {
                notificationMovieId = modelDefinition1[0x1d].definitionId;
              }
              InGameNotificationQueue_InsertPriorityRecord
                        (ARMY_CREATED,0,candidateValue,headingAngle + 0x1800 & 0xffff,
                         (modelNode1->worldTransform).translation.y,
                         (modelNode1->worldTransform).translation.x,3,notificationMovieId);
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
   Ownership: gameplay/army/runtime.
   Purpose: Binary entry is anchored by g_CodePointerTable_0051FC98[14]@0051FC98. Runtime-update partition slots
   0-23 receive (worldRuntime, armyRuntime).
   Local calls: ArmyRuntime_UpdateTimedShotAndEffectEmitters, ArmyRuntime_UpdateAnimatedModelSubnodes.
   Cross-module calls: FieldGrid_WorldToGridQ12 [world/terrain/grid], ArmyRuntime_EmitDamageThresholdEffect
   [gameplay/army/combat].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_UpdateGridBoundEffectsAndModels
          (WorldRuntimeContext *worldRuntime,ModelRuntimeClass14UpdateView200 *modelRuntime)

{
  int cellColumnOrIndex;
  int cellRow;
  FieldCellPackedFlagsAndMaterial supportFlagMask;
  FieldGridCoordinatesEaxEdx8 gridCoordinates;
  FieldGridAsset *fieldGrid1;
  
  if ((1 < (int)modelRuntime->definitionValue60_3C) &&
     (((modelRuntime->classState).classStateEC & 9) == 0)) {
    gridCoordinates = FieldGrid_WorldToGridQ12
                      ((modelRuntime->rootModelNode->worldTransform).translation.y,
                       (modelRuntime->rootModelNode->worldTransform).translation.x);
    cellColumnOrIndex = (gridCoordinates.columnQ12 >> 0xb) + 1 >> 1;
    cellRow = (gridCoordinates.rowQ12 >> 0xb) + 1 >> 1;
    fieldGrid1 = worldRuntime->fieldGrid;
    if ((0 < cellColumnOrIndex) && (0 < cellRow)) {
      if ((cellColumnOrIndex + 1 < (int)fieldGrid1->gridWidth) && (cellRow + 1 < (int)fieldGrid1->gridHeight)) {
        cellColumnOrIndex = cellRow * fieldGrid1->gridWidth + cellColumnOrIndex;
        supportFlagMask = 0x800 << ((byte)modelRuntime->modelDefinition->resourceFieldSupportSelectorC0 & 0x1f
                         );
        fieldGrid1->cells[cellColumnOrIndex].resourceExtractionDescriptor7C =
             modelRuntime->ownerArmyRuntime->factionIndex << 0xd | supportFlagMask |
             *(int *)((byte *)modelRuntime->modelDefinition + 0xc8) << 0x18;
        if ((fieldGrid1->cells[cellColumnOrIndex].flagsAndMaterial & supportFlagMask) != 0) {
          fieldGrid1->cells[cellColumnOrIndex].armyRuntimeSavedOffset6C =
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
   Ownership: gameplay/army/runtime.
   Purpose: Army class command-table handler shared by the Group A class entries. Class-command handler used by the
   ten non-default class slots.
   Local calls: ArmyRuntime_TestClass13ProximityCandidateCf, ArmyRuntime_TestModelAttachmentProximityCf.
   Cross-module calls: FixedMath_Atan2Angle16 [core/math/fixed], ArmyRuntime_ApplyImpactDamageAndFinalizeState
   [gameplay/army/combat], FieldGrid_ApplyHeightAtWorldPointAndRefreshNeighborsCf [world/terrain/grid],
   EffectDefinitionRegistry_FindByIdWithErrorCf [assets/effect/catalog],
   EffectRuntimePool_CreateInstanceFromDefinitionCf [world/effects/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_ClassCommandHandlerGroupACf
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ModelRuntimeNode *referenceNode;
  ArmyRuntimeSlot *candidateArmy;
  longlong deltaYSquared;
  longlong remainingRadiusSquared;
  dword angleOrRadius;
  byte *proximityRadius;
  int axisDelta;
  ModelRuntimeNode *modelNode1;
  bool proximityHit;
  EffectDefinitionLookupEaxCf5 effectLookup;
  DamageAmount32 damageAmount;
  int nearbyCount;
  int nextNearbyCount;
  ArmyRuntimeSlot *armySlot1;
  
  modelNode1 = (ModelRuntimeNode *)worldRuntime->ownerListHead;
  referenceNode = armyRuntime->modelNodeRuntime;
  if ((modelNode1 != (ModelRuntimeNode *)0x0) && ((g_UiCommandRuntimeFlags & 4) == 0)) {
    do {
      if (modelNode1->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
        armySlot1 = (modelNode1->runtimePayload).armyRuntime;
        if ((referenceNode != modelNode1) &&
           ((((armySlot1->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C == 0 ||
            (((armySlot1->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C == 0xc))))
        {
          /* Damage the candidate unless the class-13 test misses and the attachment proximity test hits. */
          proximityHit = ArmyRuntime_TestClass13ProximityCandidateCf(armySlot1,armyRuntime);
          if ((proximityHit) || (proximityHit = ArmyRuntime_TestModelAttachmentProximityCf(armySlot1,armyRuntime),
                                 !proximityHit)) {
            damageAmount = 0x100000;
            angleOrRadius = FixedMath_Atan2Angle16
                              ((modelNode1->worldTransform).translation.y -
                               (referenceNode->worldTransform).translation.y,
                               (modelNode1->worldTransform).translation.x -
                               (referenceNode->worldTransform).translation.x);
            ArmyRuntime_ApplyImpactDamageAndFinalizeState(angleOrRadius,damageAmount,armySlot1);
          }
        }
      }
      modelNode1 = (ModelRuntimeNode *)(modelNode1->common).nextNode;
    } while (modelNode1 != (ModelRuntimeNode *)0x0);
  }
  modelNode1 = armyRuntime->modelNodeRuntime;
  if (((((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->classLinkState).classState68 & 0x20
      ) == 0) {
    FieldGrid_ApplyHeightAtWorldPointAndRefreshNeighborsCf
              ((((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->classState).classStateDC,
               (modelNode1->worldTransform).translation.z,(modelNode1->worldTransform).translation.y
               ,(modelNode1->worldTransform).translation.x,worldRuntime->fieldGrid);
  }
  modelNode1 = (ModelRuntimeNode *)worldRuntime->ownerListHead;
  referenceNode = (armyRuntime->linkedEntityRuntime->common).ownership.modelNode;
  if (modelNode1 != (ModelRuntimeNode *)0x0) {
    armySlot1 = (ArmyRuntimeSlot *)0x0;
    nearbyCount = 0;
    do {
      nextNearbyCount = nearbyCount;
      if ((((modelNode1->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) && (modelNode1 != referenceNode)) &&
          (angleOrRadius = ((((modelNode1->runtimePayload).armyRuntime)->modelRuntimeOrSavedOffset).
                   modelRuntime)->attachments140[2].reserved1C,
          (armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex ==
          (((modelNode1->runtimePayload).armyRuntime)->linkedEntityRuntime->common).ownership.
          ownerIndex)) && (angleOrRadius != 0)) {
        proximityRadius = ((((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->attachments140[3].
                  parentModelNodeOrSavedOffset08)->modelPayload).reserved2C_33 + (angleOrRadius - 0x38);
        axisDelta = (referenceNode->worldTransform).translation.x - (modelNode1->worldTransform).translation.x;
        remainingRadiusSquared = (longlong)(int)proximityRadius * (longlong)(int)proximityRadius - (longlong)axisDelta * (longlong)axisDelta;
        if ((-1 < remainingRadiusSquared) &&
           (axisDelta = (referenceNode->worldTransform).translation.y -
                    (modelNode1->worldTransform).translation.y,
           deltaYSquared = (longlong)axisDelta * (longlong)axisDelta,
           -1 < (int)(((int)((ulonglong)remainingRadiusSquared >> 0x20) - (int)((ulonglong)deltaYSquared >> 0x20)) -
                     (uint)((uint)remainingRadiusSquared < (uint)deltaYSquared)))) {
          candidateArmy = (modelNode1->runtimePayload).armyRuntime;
          nextNearbyCount = nearbyCount + 1;
          if ((((candidateArmy->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C == 0x12) &&
             ((candidateArmy->runtimeFlags & 0x18) == 0)) {
            nextNearbyCount = nearbyCount;
            armySlot1 = candidateArmy;
          }
        }
      }
      modelNode1 = (ModelRuntimeNode *)(modelNode1->common).nextNode;
      nearbyCount = nextNearbyCount;
    } while (modelNode1 != (ModelRuntimeNode *)0x0);
    if ((nextNearbyCount == 0) && (armySlot1 != (ArmyRuntimeSlot *)0x0)) {
      modelNode1 = armySlot1->modelNodeRuntime;
      armySlot1->runtimeFlags = armySlot1->runtimeFlags | 8;
      effectLookup = EffectDefinitionRegistry_FindByIdWithErrorCf
                        (*(PckEffectDefinitionIdCatalog *)
                          (((armySlot1->modelRuntimeOrSavedOffset).modelRuntime)->classState).
                          reservedC4_C7);
      if (!effectLookup.carry) {
        EffectRuntimePool_CreateInstanceFromDefinitionCf
                  (EFFECT_RUNTIME_COMPLETION_SPAWN_ARMY_FROM_MODEL,
                   THANDOR_BITCAST(GameEntityRuntime *, EffectRuntimeOwnerReference4, armySlot1->linkedEntityRuntime),
                   (modelNode1->modelPayload).worldRotationAngle2,
                   (modelNode1->modelPayload).worldRotationAngle1,
                   (modelNode1->modelPayload).worldRotationAngle0,
                   (modelNode1->worldTransform).translation.z,
                   (modelNode1->worldTransform).translation.y,
                   (modelNode1->worldTransform).translation.x,effectLookup.definitionOrError,worldRuntime);
      }
    }
  }
  return;
}


/* Address: 0x0051D140.
   Ownership: gameplay/army/runtime.
   Purpose: Binary entry is anchored by g_ArmyRuntimeCallbackTable12[3]@00562DEC. Maintenance table phase
   terrainStateRefresh, object kind army. The 4x3 table bytes, target body, calling convention, and RET 0x08
   contract remain unchanged.
   Local calls: ArmyRuntime_InitializeTerrainOccupancyFlags.
   Cross-module calls: ModelNodeRuntime_UpdateStateTintRecursive [world/model/hierarchy].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeMaintenance_InitializeOccupancyAndStateTint
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
   Ownership: gameplay/army/runtime.
   Purpose: Binary entry is anchored by g_ArmyRuntimeCallbackTable12[9]@00562DEC. Maintenance table phase
   audioRefresh, object kind army. The 4x3 table bytes, target body, calling convention, and RET 0x08 contract
   remain unchanged.
   Local calls: ArmyRuntimeHierarchy_DispatchClassMethodDRecursive.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeMaintenance_DispatchClassMethodDRecursive
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ArmyRuntimeHierarchy_DispatchClassMethodDRecursive
            (worldRuntime,(ArmyRuntimeSlot *)armyRuntime->runtimeState48);
  return;
}


/* Address: 0x0051D2A0.
   Ownership: gameplay/army/runtime.
   Purpose: Binary entry is anchored by g_ArmyRuntimeCallbackTable12[0]@00562DEC. Maintenance table phase
   primaryUpdate, object kind army. The 4x3 table bytes, target body, calling convention, and RET 0x08 contract
   remain unchanged.
   Local calls: ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive.
   Cross-module calls: AiCombatDecision_UpdateTargetAssignment [gameplay/ai/combat].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeMaintenance_UpdateHierarchyAiAndTimers
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
  if (((armyRuntime->movementStateFlags & 2) != 0) &&
     ((modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime ==
      (ArmyRuntimeSlot *)0x0)) {
    armyRuntime->movementStateFlags = armyRuntime->movementStateFlags & 0xfffffffd;
  }
  if (armyRuntime->runtimeStateA4 != 0) {
    armyRuntime->runtimeStateA4 = armyRuntime->runtimeStateA4 - 1;
  }
  return;
}


/* Address: 0x0051D6B0.
   Ownership: gameplay/army/runtime.
   Purpose: Allocates and zeroes the exact 0x48000-byte army runtime pool, records its base and base-minus-one
   relocation value, loads up to eight faction-dependent gfx/palette binding pairs, and creates runtime instances
   for eligible records in the fixed 768-entry army definition registry. CF and EAX errors are preserved.
   Local calls: ArmyRuntime_RenderPreviewTextureCf.
   Cross-module calls: MoviePlayback_AdvanceScheduledFrameAndTick [movie/runtime/playback],
   WidePath_SetExtensionCode [core/text/path], Package_LoadEntry [assets/package/runtime],
   ArmyGraphics_CopyFrontendPlayerPaletteAndTexture [gameplay/army/audio], Resource_Release
   [assets/resource/runtime].
*/
ArmyRuntimeInitEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ArmyRuntime_InitializePoolAndGraphicsCf(void *ownerContext,word *graphicsBasePath)

{
  word pathChar;
  uint factionGraphicsVariant;
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
  word *pathCursor;
  word *pathEnd;
  ArenaAllocEaxCf5 allocResult;
  PackageLoadEntryEaxCf5 packageResult;
  GraphicsTextureSetEaxCf5 textureSetResult;
  ArmyRuntimeInitEaxCf5 initResult;
  ArmyPreviewTextureEaxCf5 previewResult;
  ArmyRuntimeInitEaxCf5 finalResult;
  
  allocResult = (*g_MemoryApi.alloc)(0x48000);
  armySlot1 = (ArmyRuntimeSlot *)allocResult.eax;
  if (!allocResult.carry) {
    g_ArmyRuntimeRebaseBaseMinusOne = (void *)((int)&armySlot1[-1].selectionMetric5 + 3);
    g_ArmyRuntimeSlots = armySlot1;
    for (remainingCount = 0x12000; remainingCount != 0; remainingCount = remainingCount + -1) {
      (armySlot1->modelRuntimeOrSavedOffset).modelRuntime = (ModelRuntimeSlot *)0x0;
      armySlot1 = (ArmyRuntimeSlot *)&armySlot1->modelNodeRuntime;
    }
    frontendPlayerRuntimeId = 0;
    remainingCount = 0x20;
    pathCursor = graphicsBasePath;
    do {
      pathEnd = pathCursor;
      if (remainingCount == 0) break;
      remainingCount = remainingCount + -1;
      pathEnd = pathCursor + 1;
      pathChar = *pathCursor;
      pathCursor = pathEnd;
    } while (pathChar != 0);
    remainingCount = 8;
    pathEnd = pathEnd + -1;
    g_MoviePlaybackScheduleSpan = 0x1a;
    do {
      MoviePlayback_AdvanceScheduledFrameAndTick();
      factionSuffixChar = 0x30;
      /* Slot 0 always loads the "0" graphics; the other slots load theirs (suffix 0-9/A-Z from the faction's
         graphics variant) only while the faction exists. */
      loadFactionGraphics = frontendPlayerRuntimeId == 0;
      if (!loadFactionGraphics) {
        paletteOrResult = (GraphicsPaletteAsset *)(frontendPlayerRuntimeId * 0x740);
        MoviePlayback_AdvanceScheduledFrameAndTick();
        if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[frontendPlayerRuntimeId] != 0) {
          factionGraphicsVariant = *(uint *)THANDOR_BYTE_AT(g_GameFactionRuntimeImage, frontendPlayerRuntimeId * 0x740 + 0x38);
          g_MoviePlaybackScheduleCounter = g_MoviePlaybackScheduleCounter + -1;
          if (factionGraphicsVariant < 10) {
            factionSuffixChar = factionGraphicsVariant + 0x30;
          }
          else {
            factionSuffixChar = factionGraphicsVariant + 0x37;
          }
          loadFactionGraphics = true;
        }
      }
      if (loadFactionGraphics) {
        *(int *)pathEnd = factionSuffixChar;
        WidePath_SetExtensionCode(0x786667,graphicsBasePath);
        packageResult = Package_LoadEntry(graphicsBasePath);
        textureSourceAsset = packageResult.bufferOrError;
        if (packageResult.carry) {
          return THANDOR_BITCAST(PackageLoadEntryEaxCf5, ArmyRuntimeInitEaxCf5, packageResult);
        }
        ArmyGraphics_CopyFrontendPlayerPaletteAndTexture
                  (frontendPlayerRuntimeId,(ArmyGraphicsAssetAddress32)textureSourceAsset);
        textureSetResult = (*g_GraphicsCreateTextureSet)(textureSourceAsset);
        loadedTextureSet = textureSetResult.textureSet;
        if (textureSetResult.carry) {
          LOCK();
          UNLOCK();
          Resource_Release(textureSourceAsset);
          initResult.carry = true;
          initResult.errorOrValue = (dword)loadedTextureSet;
          return initResult;
        }
        MoviePlayback_AdvanceScheduledFrameAndTick();
        g_ArmyGraphicsBindings[frontendPlayerRuntimeId].textureSet = loadedTextureSet;
        WidePath_SetExtensionCode(0x6c6170,graphicsBasePath);
        initResult = THANDOR_BITCAST(GraphicsPaletteAssetEaxCf5, ArmyRuntimeInitEaxCf5, (*g_GraphicsPaletteAssetLoadPackage)(graphicsBasePath));
        paletteOrResult = (GraphicsPaletteAsset *)initResult.errorOrValue;
        if (initResult.carry) {
          return initResult;
        }
        g_ArmyGraphicsBindings[frontendPlayerRuntimeId].paletteAsset = paletteOrResult;
      }
      frontendPlayerRuntimeId = frontendPlayerRuntimeId + 1;
      MoviePlayback_AdvanceScheduledFrameAndTick();
      remainingCount = remainingCount + -1;
    } while (remainingCount != 0);
    pathEnd[0] = 0;
    pathEnd[1] = 0;
    registryCursor = g_ArmyAssetRecordRegistry;
    remainingCount = 0x300;
    do {
      armyAsset = *registryCursor;
      if ((armyAsset != (ArmyAssetRecordPrefix *)0x0) &&
         ((armyAsset[1].selectionDetailTemplateVariantIndex & 0xfe) != 0)) {
        previewResult = ArmyRuntime_RenderPreviewTextureCf
                           (g_InGamePanelTextureSubresource34Height,
                            g_InGamePanelTextureSubresource34Width,
                            *(FactionRuntimeIndex *)((int)ownerContext + 0x50),armyAsset->registryId,
                            ownerContext);
        paletteOrResult = (GraphicsPaletteAsset *)previewResult.previewTexture;
        if (!previewResult.carry) {
          armyAsset[1].rootNodeOffsetOrPointer = (dword)paletteOrResult;
          previewHeight =
               (GraphicsPixelDimension)
               ((ulonglong)(longlong)g_InGamePanelTextureSubresource02Width / 3);
          previewResult = ArmyRuntime_RenderPreviewTextureCf
                             (previewHeight,previewHeight,
                              *(FactionRuntimeIndex *)((int)ownerContext + 0x50),armyAsset->registryId,
                              ownerContext);
          paletteOrResult = (GraphicsPaletteAsset *)previewResult.previewTexture;
          if (!previewResult.carry) {
            armyAsset[1].registryId = (PckArmyAssetIdCatalog)paletteOrResult;
          }
        }
      }
      registryCursor = registryCursor + 1;
      remainingCount = remainingCount + -1;
    } while (remainingCount != 0);
    allocResult.carry = false;
    allocResult.eax = (dword)paletteOrResult;
  }
  finalResult.errorOrValue = allocResult.eax;
  finalResult.carry = allocResult.carry;
  return finalResult;
}


/* Address: 0x00528330.
   Ownership: gameplay/army/runtime.
   Purpose: Binary entry is anchored by g_CodePointerTable_0051FC98[12]@0051FC98. Runtime-update partition slots
   0-23 receive (worldRuntime, armyRuntime).
   Local calls: ArmyRuntime_UpdateTimedShotAndEffectEmitters.
   Cross-module calls: ModelRuntimePool_DestroyHierarchyAndDetach [world/model/runtime],
   ModelNodeRuntime_RebuildTransformsFromRoot [world/model/hierarchy].
*/

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_UpdateEffectsAndDestroyModelHierarchy
          (WorldRuntimeContext *worldRuntime,ModelRuntimeDestroyEffectsView200 *modelRuntime)

{
  ModelRuntimeNode *modelNodeRuntime;
  ModelResourceHitTestAndRenderView210 *rootModelResource;
  int verticalStepQ12;
  int modelHeightQ12;
  GraphicsWorldCoordinateQ12 *worldTranslationZQ12Field;
  ModelResourceHitTestAndRenderView210 *modelResource;
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
   Ownership: gameplay/army/runtime.
   Purpose: Companion army-node callback that clears transient node flags, rebuilds the terrain occupancy
   contribution, and refreshes the node visual/runtime state.
   Local calls: ArmyRuntime_InitializeTerrainOccupancyFlags.
   Cross-module calls: UiModelControl_RefreshStateTint [ui/controls/misc].
*/
void __thandor_preserve_eax_edx
ArmyRuntimeNode_RebuildTerrainOccupancyAndVisualStateCallback
          (WorldRuntimeContext *armyContext,WorldOwnerListNode100 *node)

{
  if (node->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
    node->runtimeFlags = node->runtimeFlags & 0xfffffff3;
    ArmyRuntime_InitializeTerrainOccupancyFlags
              (armyContext,*(ArmyRuntimeSlot **)((int)node->runtimePayload + 8));
    UiModelControl_RefreshStateTint((ModelRuntimeNode *)node);
  }
  return;
}


/* Address: 0x0051C3B0.
   Ownership: gameplay/army/runtime.
   Purpose: Typed parameters: p2 actionVector0→Q12, p3 actionVector2→Q12, p4 actionVector1→Q12. Calling convention,
   complete VariableStorage serialization, function bytes, control flow, globals, locals, and executable data
   remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_SetNonzeroActionVector
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
   Ownership: gameplay/army/runtime.
   Purpose: Resolves the current command target according to the movement-mode flags, stores the target pointer and
   state code, and clears the target fields when resolution fails. Two stack arguments are authoritative from RET
   0x08; EAX/ECX are internal or preserved state, not a semantic return.
   Cross-module calls: ArmyRuntime_ResetMovementStatePreserveQueuedTarget [gameplay/army/movement],
   ArmyRuntime_ResetMovementStateFromCurrentPosition [gameplay/army/movement].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_ResolveCommandTarget(ArmyRuntimeSlot *targetArmyRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ArmyCommandGeneration standardGeneration;
  ArmyCommandGeneration resolvedCommandGeneration;
  
  standardGeneration = g_ArmyCommandGenerationStandard;
  if ((armyRuntime->movementStateFlags & 0x20) != 0) {
    if ((armyRuntime->commandModeFlags & 8) == 0) {
      ArmyRuntime_ResetMovementStatePreserveQueuedTarget((ArmyMovementRuntime *)armyRuntime);
    }
    else {
      ArmyRuntime_ResetMovementStateFromCurrentPosition((ArmyMovementRuntime *)armyRuntime);
    }
  }
  if (targetArmyRuntime == (ArmyRuntimeSlot *)0x0) {
    armyRuntime->commandModeFlags = 0;
    armyRuntime->commandGeneration = 0;
  }
  else {
    armyRuntime->commandModeFlags = 1;
    armyRuntime->commandGeneration = standardGeneration;
  }
  armyRuntime->commandTargetArmyRuntime = targetArmyRuntime;
  return;
}


/* Address: 0x0051C620.
   Ownership: gameplay/army/runtime.
   Purpose: The three Q12 command coordinates and command-generation state map to
   GameEntityRuntimeCommon.commandTarget.
   Cross-module calls: ArmyRuntime_ResetMovementStatePreserveQueuedTarget [gameplay/army/movement],
   ArmyRuntime_ResetMovementStateFromCurrentPosition [gameplay/army/movement].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_ApplyTargetPositionCommand
          (Q12 coordinateA,Q12 coordinateB,Q12 coordinateC,ArmyRuntimeSlot *armyRuntime)

{
  ArmyCommandGeneration commandGeneration;
  
  if ((armyRuntime->movementStateFlags & 0x20) != 0) {
    if ((armyRuntime->commandModeFlags & 8) == 0) {
      ArmyRuntime_ResetMovementStatePreserveQueuedTarget((ArmyMovementRuntime *)armyRuntime);
    }
    else {
      ArmyRuntime_ResetMovementStateFromCurrentPosition((ArmyMovementRuntime *)armyRuntime);
    }
  }
  armyRuntime->commandModeFlags = 2;
  armyRuntime->commandCoordinate0Q12 = coordinateC;
  armyRuntime->commandCoordinate1Q12 = coordinateB;
  armyRuntime->commandCoordinate2Q12 = coordinateA;
  commandGeneration = g_ArmyCommandGenerationStandard;
  armyRuntime->commandTargetArmyRuntime = (ArmyRuntimeSlot *)0x0;
  armyRuntime->movementStateFlags = armyRuntime->movementStateFlags & 0xffffffdf;
  armyRuntime->commandGeneration = commandGeneration;
  return;
}


/* Address: 0x0051C720.
   Ownership: gameplay/army/runtime.
   Purpose: Resolves the active shot aim point from the current target state, applies the verified range and lead
   adjustment when required, and reports invalid target state through carry. Five stack arguments are authoritative
   from RET 0x14. EDX:EAX carries the nominal X/Z result pair, ECX carries Y, and CF reports validity. Role:
   Resolves the world aim point from explicit coordinates or a target entity, including lead adjustment. Inputs:
   Shooter position, command target state, ShotDefinition and target model hierarchy.
   Cross-module calls: FixedMath_Length3 [core/math/fixed], ShotDefinition_GetModeRangeLimitEbx
   [assets/shot/catalog], ShotDefinition_ComputeMode3LeadAdjustment [assets/shot/catalog], FixedMath_Atan2Angle16
   [core/math/fixed], FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed].
*/
WorldPositionEaxEcxEdxCf13
ArmyRuntime_ResolveShotAimPointCf
          (Q12 sourceWorldZQ12,Q12 sourceWorldYQ12,Q12 sourceWorldXQ12,
          ShotDefinition *shotDefinition,GameEntityRuntime *targetState)

{
  int *targetDefinitionRecord;
  int targetClassRecord;
  longlong deltaYSquared;
  longlong remainingRangeSquared;
  uint visibilityMask;
  dword distanceOrAngle;
  int leadDistance;
  int definitionOrDelta;
  int aimWorldX;
  dword directionY;
  int aimWorldY;
  int aimWorldZ;
  ModelRuntimeNode *modelNode1;
  ShotModeRangeLimitEbxCf5 rangeLimit;
  FixedDirectionXyzRegs12 leadDirection;
  WorldPositionEaxEcxEdxCf13 position;
  GameEntityRuntime *entityRuntime1;

  /* On failure (CF set) the original leaves whatever is in EAX/ECX/EDX at that point (the caller's values or the
     partial visibility mask / definition pointers). All three callers ignore the coordinates when CF is set, so
     the failure result carries zeros. */
  position.worldXQ12 = 0;
  position.worldYQ12 = 0;
  position.worldZQ12 = 0;
  position.carry = true;
  if (((targetState->common).commandTarget.targetFlags & 1) == 0) {
    if (((targetState->common).commandTarget.targetFlags & 2) != 0) {
      position.worldXQ12 = (targetState->common).commandTarget.targetWorldXQ12;
      position.worldYQ12 = (targetState->common).commandTarget.targetWorldYQ12;
      position.worldZQ12 = (targetState->common).commandTarget.targetWorldZQ12;
      position.carry = false;
    }
  }
  else {
    entityRuntime1 = (targetState->common).commandTarget.targetEntity;
    if (entityRuntime1 != (GameEntityRuntime *)0x0) {
      visibilityMask = 2u << ((byte)((targetState->common).ownership.ownerIndex * 2) & 0x1f);
      modelNode1 = (entityRuntime1->common).ownership.modelNode;
      if ((*(uint *)((entityRuntime1->common).damageState.reserved0C_23 + 0x10) & visibilityMask) != 0) {
        if (*(int *)(*(int *)(entityRuntime1->common).ownership.definitionOrClassRecord + 0x4c) == 0x15) {
          modelNode1 = modelNode1->childNodes[0];
        }
        aimWorldX = (modelNode1->worldTransform).translation.x;
        targetDefinitionRecord = (entityRuntime1->common).ownership.definitionOrClassRecord;
        definitionOrDelta = *targetDefinitionRecord;
        aimWorldY = (modelNode1->worldTransform).translation.y;
        aimWorldZ = (modelNode1->worldTransform).translation.z + *(int *)(definitionOrDelta + 0x50);
        targetClassRecord = targetDefinitionRecord[2];
        if ((*(int *)(definitionOrDelta + 0x18) != 0) && ((*(uint *)(targetClassRecord + 0x18) & 4) == 0)) {
          distanceOrAngle = FixedMath_Length3(aimWorldZ - sourceWorldZQ12,aimWorldY - sourceWorldYQ12,
                                    aimWorldX - sourceWorldXQ12);
          rangeLimit = ShotDefinition_GetModeRangeLimitEbx(shotDefinition);
          leadDistance = (int)(((longlong)(int)distanceOrAngle * (longlong)*(int *)(definitionOrDelta + 0xc)) /
                       (longlong)rangeLimit.rangeLimitQ12);
          distanceOrAngle = ShotDefinition_ComputeMode3LeadAdjustment(shotDefinition);
          leadDistance = leadDistance + distanceOrAngle * *(int *)(definitionOrDelta + 0xc);
          distanceOrAngle = FixedMath_Atan2Angle16
                            (*(int *)(targetClassRecord + 0x5c) - *(int *)(*(int *)(targetClassRecord + 4) + 0x98),
                             *(int *)(targetClassRecord + 0x58) - *(int *)(*(int *)(targetClassRecord + 4) + 0x94));
          leadDirection = FixedMath_DirectionFromAnglesScaledRegs(0,distanceOrAngle,leadDistance);
          directionY = leadDirection.ecx;
          distanceOrAngle = leadDirection.eax;
          entityRuntime1 = (targetState->common).commandTarget.targetEntity;
          definitionOrDelta = (entityRuntime1->common).damageState.trackedCoordinate0Q12;
          if (definitionOrDelta == (entityRuntime1->common).pathCoordinate0Q12) {
            definitionOrDelta = definitionOrDelta - (modelNode1->worldTransform).translation.x;
            remainingRangeSquared = ((longlong)(int)directionY * (longlong)(int)directionY +
                    (longlong)(int)distanceOrAngle * (longlong)(int)distanceOrAngle) - (longlong)definitionOrDelta * (longlong)definitionOrDelta
            ;
            if (((-1 < remainingRangeSquared) &&
                (definitionOrDelta = (entityRuntime1->common).damageState.trackedCoordinate1Q12,
                definitionOrDelta == (entityRuntime1->common).pathCoordinate1Q12)) &&
               (definitionOrDelta = definitionOrDelta - (modelNode1->worldTransform).translation.y,
               deltaYSquared = (longlong)definitionOrDelta * (longlong)definitionOrDelta,
               -1 < remainingRangeSquared - deltaYSquared)) {
              /* The target is standing still within lead range: aim at its path position directly. */
              position.worldXQ12 = (entityRuntime1->common).pathCoordinate0Q12;
              position.worldYQ12 = (entityRuntime1->common).pathCoordinate1Q12;
              position.worldZQ12 =
                   (((entityRuntime1->common).ownership.modelNode)->worldTransform).translation.z +
                   *(int *)(*(int *)(entityRuntime1->common).ownership.definitionOrClassRecord + 0x50);
              position.carry = false;
              return position;
            }
          }
          aimWorldZ = leadDirection.edx + aimWorldZ;
          aimWorldY = directionY + aimWorldY;
          aimWorldX = distanceOrAngle + aimWorldX;
        }
        position.worldXQ12 = aimWorldX;
        position.worldYQ12 = aimWorldY;
        position.worldZQ12 = aimWorldZ;
        position.carry = false;
        return position;
      }
      (targetState->common).commandTarget.targetEntity = (GameEntityRuntime *)0x0;
      (targetState->common).commandTarget.targetFlags = 0;
    }
  }
  return position;
}


/* Address: 0x0051D170.
   Ownership: gameplay/army/runtime.
   Purpose: Army-node owner-list callback that derives the faction visibility mask, accumulates projected terrain
   occlusion around the node world position, and marks occupancy bit 2 when the definition flags require it.
   Maintenance table phase occupancyRebuild, object kind army. The 4x3 table bytes, target body, calling
   convention, and RET 0x08 contract remain unchanged.
   Cross-module calls: TerrainProjectedOcclusion_AccumulateMaskAroundWorldPoint [world/terrain/projection],
   TerrainOccupancyBit2_MarkAroundWorldPoint [world/terrain/occupancy].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeNode_AccumulateTerrainOcclusionAndOccupancyCallback
          (WorldRuntimeContext *worldRuntime,WorldOwnerListNode100 *node)

{
  int armyRecord;
  uint relationStates;
  int activeFactionIndex;
  ulonglong visibilityMask;
  ulonglong factionMaskByte;
  Q12 worldXQ12;
  Q12 worldYQ12;
  int occupancyByteOffset;
  FieldGridAsset *fieldGrid;
  
  if (node->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
    worldYQ12 = node->worldXQ12;
    armyRecord = *(int *)((int)node->runtimePayload + 8);
    worldXQ12 = node->worldYQ12;
    if (*(int *)(armyRecord + 0xc) != 0) {
      occupancyByteOffset = *(int *)(armyRecord + 0xc);
      fieldGrid = worldRuntime->fieldGrid;
      relationStates = g_GameFactionRuntimeImage.records[occupancyByteOffset].packedRelationStates;
      factionMaskByte = (ulonglong)*(uint *)(armyRecord + 0x9c);
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
      if ((relationStates >> ((byte)(activeFactionIndex << 2) & 0x1f) & 8) != 0) {
        TerrainOccupancyBit2_MarkAroundWorldPoint
                  (*(FieldGridRadiusUnits *)(armyRecord + 0x90),worldXQ12,worldYQ12,occupancyByteOffset,
                   fieldGrid);
      }
    }
  }
  return;
}


/* Address: 0x0051D310.
   Ownership: gameplay/army/runtime.
   Purpose: Tests the army-runtime dword at offset 0x100 and returns the result through carry. Carry is set exactly
   when the field is zero.
*/
bool __thandor_cf_preserve_eax_ecx_edx
ArmyRuntime_TestStateField100ZeroCf(ArmyRuntimeSlot *runtimeState)

{
  return runtimeState->stateOrTechnologyId == 0;
}


/* Address: 0x0051D330.
   Ownership: gameplay/army/runtime.
   Purpose: Tests whether the signed state field at army-runtime offset 0x100 is nonnegative; CF carries the
   result.
*/
bool __thandor_cf_preserve_eax_ecx_edx
ArmyRuntime_TestStateField100NonnegativeCf(ArmyRuntimeSlot *armyRuntime)

{
  return -1 < armyRuntime->stateOrTechnologyId;
}


/* Address: 0x0051D350.
   Ownership: gameplay/army/runtime.
   Purpose: Loads the pointed runtime node, selects one callback through the exact 0x0051FED8 type-index table, and
   forwards the node plus caller argument. Placement-validation partition slots 24-47 receive (worldRuntime,
   armyRuntime), with CF carrying acceptance.
*/
bool __thandor_cf_preserve_eax_edx
ArmyRuntimeNode_DispatchTypedCallback
          (ArmyRuntimeSlot **modelRuntimeHolder,WorldRuntimeContext *worldRuntime)

{
  bool accepted;
  
  accepted = (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.placementValidationCf
            [((ModelRuntimePlacementValidationView200 *)*modelRuntimeHolder)->modelDefinition->
             runtimeClassId4C])
                    (worldRuntime,(ModelRuntimePlacementValidationView200 *)*modelRuntimeHolder);
  return accepted;
}


/* Address: 0x0051D4D0.
   Ownership: gameplay/army/runtime.
   Purpose: Dispatches a two-argument army runtime command through the verified runtime-class handler table indexed
   by the root model definition class. Dispatch wrapper for the typed twenty-four-entry class-command table.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_DispatchClassCommand
          (ArmyRuntimeSlot **armyRuntimeHolder,WorldRuntimeContext *worldRuntime)

{
  (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand
    [(((*armyRuntimeHolder)->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C])
            (worldRuntime,*armyRuntimeHolder);
  return;
}


/* Address: 0x0051D8C0.
   Ownership: gameplay/army/runtime.
   Purpose: Frees the army runtime pool, releases all eight texture/palette binding pairs, frees the two verified
   owned allocations for every populated army registry entry, and clears the registry pointers.
*/
void __thandor_void_preserve_eax_ecx_edx ArmyRuntime_ShutdownPoolAndGraphics(void)

{
  ArmyAssetRecordPrefix *armyAsset;
  int remainingCount;
  ArmyGraphicsBinding *graphicsBindingCursor;
  ArmyAssetRecordPrefix **assetRegistryCursor;
  
  (*g_MemoryApi.free)(g_ArmyRuntimeSlots);
  g_ArmyRuntimeSlots = (ArmyRuntimeSlot *)0x0;
  graphicsBindingCursor = g_ArmyGraphicsBindings;
  remainingCount = 8;
  do {
    if (graphicsBindingCursor->textureSet != (GraphicsTextureSet *)0x0) {
      (*g_GraphicsTextureSetReleasePackageCf)(graphicsBindingCursor->textureSet);
      graphicsBindingCursor->textureSet = (GraphicsTextureSet *)0x0;
    }
    if (graphicsBindingCursor->paletteAsset != (GraphicsPaletteAsset *)0x0) {
      (*g_GraphicsPaletteAssetLifecycleCallbacks3.releasePackage)
                (graphicsBindingCursor->paletteAsset);
      graphicsBindingCursor->paletteAsset = (GraphicsPaletteAsset *)0x0;
    }
    graphicsBindingCursor = graphicsBindingCursor + 1;
    remainingCount = remainingCount + -1;
  } while (remainingCount != 0);
  assetRegistryCursor = g_ArmyAssetRecordRegistry;
  remainingCount = 0x300;
  do {
    armyAsset = *assetRegistryCursor;
    if (armyAsset != (ArmyAssetRecordPrefix *)0x0) {
      (*g_MemoryApi.free)((void *)armyAsset[1].rootNodeOffsetOrPointer);
      (*g_MemoryApi.free)((void *)armyAsset[1].registryId);
      *assetRegistryCursor = (ArmyAssetRecordPrefix *)0x0;
    }
    assetRegistryCursor = assetRegistryCursor + 1;
    remainingCount = remainingCount + -1;
  } while (remainingCount != 0);
  return;
}


/* Address: 0x0051D960.
   Ownership: gameplay/army/runtime.
   Purpose: Traverses 1024 exact 0x120-byte slots. Populated slots convert four verified runtime pointers to
   serialized offsets; empty slots are zeroed. Returns the pool base in EAX and exact byte size 0x48000 in EDX.
*/
RuntimeImagePointerByteSizeEdxEax8 __cdecl ArmyRuntimePool_ConvertPointersToOffsetsForSaveRegs(void)

{
  dword runtimeState98Offset;
  ModelRuntimeSlot *savedModelRuntimeOffset;
  int clearWordsRemaining;
  int slotsRemaining;
  ArmyRuntimeSlot *armySlot1;
  ArmyRuntimeSlot *armySlot2;
  
  slotsRemaining = 0x400;
  armySlot2 = g_ArmyRuntimeSlots;
  do {
    while( true ) {
      armySlot1 = armySlot2->commandTargetArmyRuntime;
      if (armySlot2->modelNodeRuntime != (ModelRuntimeNode *)0x0) break;
      for (clearWordsRemaining = 0x48; clearWordsRemaining != 0; clearWordsRemaining = clearWordsRemaining + -1) {
        (armySlot2->modelRuntimeOrSavedOffset).modelRuntime = (ModelRuntimeSlot *)0x0;
        armySlot2 = (ArmyRuntimeSlot *)&armySlot2->modelNodeRuntime;
      }
      slotsRemaining = slotsRemaining + -1;
      if (slotsRemaining == 0) {
        /* EDX = pool byte size, EAX = pool base. */
        return (qword)0x48000 << 32 | (dword)g_ArmyRuntimeSlots;
      }
    }
    savedModelRuntimeOffset = (ModelRuntimeSlot *)
             ((int)(armySlot2->modelRuntimeOrSavedOffset).modelRuntime - g_ModelRuntimeRebaseDelta);
    if (armySlot1 != (ArmyRuntimeSlot *)0x0) {
      armySlot1 = (ArmyRuntimeSlot *)((int)armySlot1 - (int)g_ArmyRuntimeRebaseBaseMinusOne);
    }
    armySlot2->modelNodeRuntime =
         (ModelRuntimeNode *)
         ((int)armySlot2->modelNodeRuntime - (int)g_RuntimeObjectRebaseBaseMinusOne);
    runtimeState98Offset = armySlot2->runtimeState98;
    (armySlot2->modelRuntimeOrSavedOffset).modelRuntime = savedModelRuntimeOffset;
    if (runtimeState98Offset != 0) {
      runtimeState98Offset = runtimeState98Offset - (int)g_ArmyRuntimeRebaseBaseMinusOne;
    }
    armySlot2->commandTargetArmyRuntime = armySlot1;
    armySlot2->runtimeState98 = runtimeState98Offset;
    armySlot2 = armySlot2 + 1;
    slotsRemaining = slotsRemaining + -1;
  } while (slotsRemaining != 0);
  return (qword)0x48000 << 32 | (dword)g_ArmyRuntimeSlots;
}


/* Address: 0x0051D9F0.
   Ownership: gameplay/army/runtime.
   Purpose: Traverses 1024 exact 0x120-byte slots and converts the four verified serialized offsets in each
   populated slot back to runtime pointers using the model, world, and army pool relocation bases.
*/
void __thandor_void_preserve_eax_ecx_edx ArmyRuntimePool_RebaseAfterLoad(void)

{
  dword savedRuntimeState98Offset;
  void *rebasedDefinition;
  int runtimeSlotsRemaining;
  ArmyRuntimeSlot *rebasedCommandTarget;
  ArmyRuntimeSlot *runtimeSlotCursor;
  
  runtimeSlotsRemaining = 0x400;
  runtimeSlotCursor = g_ArmyRuntimeSlots;
  do {
    if (runtimeSlotCursor->modelNodeRuntime != (ModelRuntimeNode *)0x0) {
      rebasedDefinition =
           ((runtimeSlotCursor->modelRuntimeOrSavedOffset).modelRuntime)->reserved10_37 +
           g_ModelRuntimeRebaseDelta + -0x10;
      rebasedCommandTarget = (ArmyRuntimeSlot *)0x0;
      if (runtimeSlotCursor->commandTargetArmyRuntime != (ArmyRuntimeSlot *)0x0) {
        rebasedCommandTarget =
             (ArmyRuntimeSlot *)
             ((int)&runtimeSlotCursor->commandTargetArmyRuntime->modelRuntimeOrSavedOffset +
             (int)g_ArmyRuntimeRebaseBaseMinusOne);
      }
      runtimeSlotCursor->modelNodeRuntime =
           (ModelRuntimeNode *)
           (g_RuntimeObjectRebaseBaseMinusOne +
           (int)(&runtimeSlotCursor->modelNodeRuntime->modelPayload + -1) + 0x30);
      savedRuntimeState98Offset = runtimeSlotCursor->runtimeState98;
      (runtimeSlotCursor->modelRuntimeOrSavedOffset).modelRuntime = rebasedDefinition;
      if (savedRuntimeState98Offset != 0) {
        savedRuntimeState98Offset = savedRuntimeState98Offset + (int)g_ArmyRuntimeRebaseBaseMinusOne
        ;
      }
      runtimeSlotCursor->commandTargetArmyRuntime = rebasedCommandTarget;
      runtimeSlotCursor->runtimeState98 = savedRuntimeState98Offset;
    }
    runtimeSlotCursor = runtimeSlotCursor + 1;
    runtimeSlotsRemaining = runtimeSlotsRemaining + -1;
  } while (runtimeSlotsRemaining != 0);
  return;
}


/* Address: 0x00520CF0.
   Ownership: gameplay/army/runtime.
   Purpose: Refreshes the two verified positioned-sound channels for the first ground-movement runtime class when
   their terrain projection cells remain active. Class method-D partition slots 24-47 receive (worldRuntime,
   armyRuntime).
   Cross-module calls: TerrainGrid_TestProjectedCellMaskBits01Cf [world/terrain/grid],
   SpatialSound_UpdateDesiredPositionedGains [audio/spatial/runtime].
*/

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_UpdatePositionedSoundsVariantA
          (WorldRuntimeContext *worldRuntime,
          ArmyRuntimeGroundMovementPositionedSoundView120 *armyRuntime)

{
  uint soundSlotIndex;
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
       (worldRuntime->dwordArray != (dword *)0x0)) {
      soundSlot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex];
      if (soundSlot != (SpatialSoundSlot *)0x0) {
        worldPosition = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
        cellMasked = TerrainGrid_TestProjectedCellMaskBits01Cf
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
     (worldRuntime->dwordArray != (dword *)0x0)) {
    soundSlot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex];
    if (soundSlot != (SpatialSoundSlot *)0x0) {
      worldPosition = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
      cellMasked = TerrainGrid_TestProjectedCellMaskBits01Cf
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
   Ownership: gameplay/army/runtime.
   Purpose: Binary entry is anchored by g_CodePointerTable_0051FC98[27]@0051FC98. Class method-D partition slots
   24-47 receive (worldRuntime, armyRuntime).
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_NoOpUpdate(WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  return;
}


/* Depth-first over a model runtime tree (count +0x0C, children +0x140 + 32*i, null slots skipped):
   the last node whose definition (+0x00) has class value 10 at +0x4C, or null. */
static byte *ArmyRuntimeClass_FindLastClass10Node(byte *node)
{
  byte *found = (byte *)0x0;
  int i;
  if (*(int *)(*(byte **)node + 0x4c) == 10) {
    found = node;
  }
  for (i = 0; i < *(int *)(node + 0xc); i++) {
    byte *child = *(byte **)(node + 0x140 + i * 0x20);
    if (child != (byte *)0x0) {
      byte *match = ArmyRuntimeClass_FindLastClass10Node(child);
      if (match != (byte *)0x0) {
        found = match;
      }
    }
  }
  return found;
}

/* Address: 0x00523E70.
   Ownership: gameplay/army/runtime.
   Purpose: Iterator callback that filters candidate world nodes and stores an accepted projectile target into the
   active army runtime slot.
*/

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_SelectProjectileTargetNode
          (ModelRuntimeTimedTargetProjectileView200 *modelRuntime,
          WorldOwnerListNode100 *candidateNode)

{
  longlong deltaYSquared;
  longlong remainingRangeSquared;
  int deltaX;
  int candidateValue;
  ModelRuntimeSlot *targetModelRuntime;
  
  if (candidateNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
    candidateValue = ((modelRuntime->modelDefinition->shotDefinitionReference2C).definition)->
            mode2SelectionRangeQ12;
    deltaX = candidateNode->worldXQ12 - (modelRuntime->rootModelNode->worldTransform).translation.x;
    remainingRangeSquared = (longlong)candidateValue * (longlong)candidateValue - (longlong)deltaX * (longlong)deltaX;
    if ((((-1 < remainingRangeSquared) &&
         (candidateValue = candidateNode->worldYQ12 -
                  (modelRuntime->rootModelNode->worldTransform).translation.y,
         deltaYSquared = (longlong)candidateValue * (longlong)candidateValue,
         -1 < (int)(((int)((ulonglong)remainingRangeSquared >> 0x20) - (int)((ulonglong)deltaYSquared >> 0x20)) -
                   (uint)((uint)remainingRangeSquared < (uint)deltaYSquared)))) &&
        (candidateValue = *(int *)(*(int *)((int)candidateNode->runtimePayload + 8) + 0xc),
        candidateValue != modelRuntime->ownerArmyRuntime->factionIndex)) && (candidateValue != 0)) {
      /* Rewritten from the assembly (0x00523F09-0x00523F7C): pick the last node, depth-first, whose
         definition has class 10 (+0x4C); the walk kept its frames on the machine stack. */
      targetModelRuntime = (ModelRuntimeSlot *)ArmyRuntimeClass_FindLastClass10Node((byte *)candidateNode->runtimePayload);
      if ((targetModelRuntime != (ModelRuntimeSlot *)0x0) && (((targetModelRuntime->classState).classStateEC & 8) == 0)) {
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
   Ownership: gameplay/army/runtime.
   Purpose: Binary entry is anchored by g_CodePointerTable_0051FC98[20]@0051FC98. Runtime-update partition slots
   0-23 receive (worldRuntime, armyRuntime). Role: Class callback that advances target/projectile timers and
   associated effects. Inputs: World context, army target state, timers and definitions. Outputs: Updated target
   state and scheduled projectile/effect creation.
   Cross-module calls: WorldRuntime_ForEachNodeInOwnerListD8 [world/runtime/core],
   ModelRuntime_EmitProjectilesFromAttachmentPoints [world/model/runtime],
   ModelNodeRuntime_RebuildTransformsFromRoot [world/model/hierarchy], ArmyRuntime_EmitDamageThresholdEffect
   [gameplay/army/combat].
*/

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_UpdateTimedTargetProjectilesAndEffects
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
      (modelRuntime->timedTargetLinkState).selectedTargetModelRuntime60 = (ModelRuntimeSlot *)0x0;
      (modelRuntime->timedTargetLinkState).matchingActiveShotRuntime64 = (ShotRuntimeSlot *)0x0;
      WorldRuntime_ForEachNodeInOwnerListD8
                (modelRuntime,ArmyRuntimeClass_SelectProjectileTargetNode,worldRuntime);
      selectedTarget = (modelRuntime->timedTargetLinkState).selectedTargetModelRuntime60;
      if (((modelRuntime->timedTargetLinkState).matchingActiveShotRuntime64 ==
           (ShotRuntimeSlot *)0x0) && (selectedTarget != (ModelRuntimeSlot *)0x0)) {
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
  (modelRuntime->timedTargetLinkState).selectedTargetModelRuntime60 = (ModelRuntimeSlot *)0x0;
  (modelRuntime->timedTargetLinkState).matchingActiveShotRuntime64 = (ShotRuntimeSlot *)0x0;
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  return;
}


/* Address: 0x00525960.
   Ownership: gameplay/army/runtime.
   Purpose: Refreshes the two verified positioned-sound channels for the second ground-movement runtime class when
   their terrain projection cells remain active. Class method-D partition slots 24-47 receive (worldRuntime,
   armyRuntime).
   Cross-module calls: TerrainGrid_TestProjectedCellMaskBits01Cf [world/terrain/grid],
   SpatialSound_UpdateDesiredPositionedGains [audio/spatial/runtime].
*/

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_UpdatePositionedSoundsVariantB
          (WorldRuntimeContext *worldRuntime,
          ArmyRuntimeGroundMovementPositionedSoundView120 *armyRuntime)

{
  uint soundSlotIndex;
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
       (worldRuntime->dwordArray != (dword *)0x0)) {
      soundSlot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex];
      if (soundSlot != (SpatialSoundSlot *)0x0) {
        worldPosition = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
        cellMasked = TerrainGrid_TestProjectedCellMaskBits01Cf
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
     (worldRuntime->dwordArray != (dword *)0x0)) {
    soundSlot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex];
    if (soundSlot != (SpatialSoundSlot *)0x0) {
      worldPosition = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
      cellMasked = TerrainGrid_TestProjectedCellMaskBits01Cf
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
   Ownership: gameplay/army/runtime.
   Purpose: Loads EBX from army-runtime offset 0x6C and ECX from the linked model-definition offset 0xC4. The
   selection-panel renderer consumes both register results when composing metric bars.
*/
ArmyMetric6CDefinitionC4Regs8 __thandor_regs_ebx_ecx_preserve_eax_edx
ArmyRuntime_QueryMetric6CAndDefinitionC4Regs(ArmyRuntimeSlot *armyRuntime)

{
  ArmyMetric6CDefinitionC4Regs8 metricRegs;
  
  metricRegs.ecx = *(dword *)(((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->classState).
                        reservedC4_C7;
  metricRegs.ebx = (dword)armyRuntime->linkedArmyRuntimeOrSavedOffset;
  return metricRegs;
}


/* Address: 0x00527150.
   Ownership: gameplay/army/runtime.
   Purpose: Scans thirteen attachment slots and ORs effect-variant bits 1, 2, and 4 into EBX.
*/
int __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_AccumulateAttachmentEffectVariantMaskRegs(ArmyRuntimeSlot *armyRuntime)

{
  int attachmentAssetId;
  int attachmentEffectSlotsRemaining;
  uint variantMask;
  
  variantMask = 0;
  attachmentEffectSlotsRemaining = 0xd;
  do {
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
    armyRuntime = (ArmyRuntimeSlot *)&armyRuntime->modelNodeRuntime;
    attachmentEffectSlotsRemaining = attachmentEffectSlotsRemaining + -1;
  } while (attachmentEffectSlotsRemaining != 0);
  return variantMask;
}


/* Address: 0x00527FE0.
   Ownership: gameplay/army/runtime.
   Purpose: Refreshes the configured looping positioned sound while the runtime remains active and its projected
   terrain cell is available. Class method-D partition slots 24-47 receive (worldRuntime, armyRuntime).
   Cross-module calls: TerrainGrid_TestProjectedCellMaskBits01Cf [world/terrain/grid],
   SpatialSound_UpdateDesiredPositionedGains [audio/spatial/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_UpdateLoopingPositionedSound
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  GraphicsFixedVec3 *worldPosition;
  ModelRuntimeSlot *linkedModelRuntime;
  uint soundSlotIndex;
  SpatialSoundSlot *slot;
  bool cellMasked;
  
  linkedModelRuntime = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  soundSlotIndex = linkedModelRuntime->attachments140[3].childNodeIndex0C;
  if (((((armyRuntime->runtimeFlags & 1) == 0) && (soundSlotIndex != 0)) &&
      (soundSlotIndex < worldRuntime->dwordArrayCount)) &&
     ((worldRuntime->dwordArray != (dword *)0x0 &&
      (slot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex], slot != (SpatialSoundSlot *)0x0))
     )) {
    worldPosition = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
    cellMasked = TerrainGrid_TestProjectedCellMaskBits01Cf
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
   Ownership: gameplay/army/runtime.
   Purpose: Table membership RUNTIME_UPDATE[23]. Raises or lowers the child model local-Z deployment state, toggles
   linked collision/runtime state at the endpoints, and emits the associated projected one-shot sounds. Runtime-
   update partition slots 0-23 receive (worldRuntime, armyRuntime).
   Cross-module calls: TerrainGrid_TestProjectedCellMaskBits01Cf [world/terrain/grid],
   SpatialSound_PlayPositionedOneShot [audio/spatial/runtime], ModelNodeRuntime_RebuildTransformsFromRoot
   [world/model/hierarchy], ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf [gameplay/army/placement].
*/

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_UpdateVerticalDeploymentAndCollisionState
          (WorldRuntimeContext *worldRuntime,ModelRuntimeVerticalDeploymentView200 *modelRuntime)

{
  dword *classStateField;
  Q12 *childTranslationZ;
  ModelDefinitionVerticalDeploymentView280 *deploymentDefinition;
  ArmyRuntimeSlot *armyRuntime;
  uint soundAssetIndex;
  DirectSoundVoiceSet **soundVoiceSet;
  int travelLimitOrStep;
  bool testResult;
  ModelRuntimeNode *modelNode1;
  
  deploymentDefinition = modelRuntime->modelDefinition;
  modelNode1 = modelRuntime->rootModelNode;
  armyRuntime = (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime;
  travelLimitOrStep = deploymentDefinition->deploymentTravelLimitQ12;
  if (((modelRuntime->classState).classStateB8 & 1) == 0) {
    if ((modelRuntime->deploymentState60).deploymentTravelQ12_60 != 0) {
      if ((modelRuntime->deploymentState60).deploymentTravelQ12_60 <= travelLimitOrStep) {
        soundAssetIndex = deploymentDefinition->deploymentSoundAssetIndex26C;
        if (((soundAssetIndex != 0) && (soundAssetIndex < worldRuntime->dwordArrayCount)) &&
           (worldRuntime->dwordArray != (dword *)0x0)) {
          soundVoiceSet = (DirectSoundVoiceSet **)worldRuntime->dwordArray[soundAssetIndex];
          if (soundVoiceSet != (DirectSoundVoiceSet **)0x0) {
            testResult = TerrainGrid_TestProjectedCellMaskBits01Cf
                              ((modelNode1->worldTransform).translation.y,
                               (modelNode1->worldTransform).translation.x,worldRuntime);
            if (!testResult) {
              SpatialSound_PlayPositionedOneShot
                        (deploymentDefinition->positionedSoundMaximumDistanceQ12,deploymentDefinition->positionedSoundGainQ15,
                         &(modelNode1->worldTransform).translation,soundVoiceSet);
            }
          }
        }
      }
      modelNode1 = modelNode1->childNodes[0];
      travelLimitOrStep = deploymentDefinition->verticalDeploymentStepQ12PerTick;
      classStateField = &(modelRuntime->classState).classStateB8;
      *classStateField = *classStateField & 0xfffffffd;
      travelLimitOrStep = travelLimitOrStep * g_InGameSimulationStepTicks;
      if (modelNode1 != (ModelRuntimeNode *)0x0) {
        childTranslationZ = &(modelNode1->modelPayload).localTranslationZQ12;
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
       armyRuntime != (ArmyRuntimeSlot *)0x0)) {
      (modelRuntime->deploymentState60).collisionRetryCountdown64 = 8;
      (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime = (ArmyRuntimeSlot *)0x0
      ;
      testResult = ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf
                        (deploymentDefinition->placementRadiusOrClearanceDC,
                         (modelNode1->worldTransform).translation.y,
                         (modelNode1->worldTransform).translation.x,armyRuntime);
      if (testResult) {
        (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime = armyRuntime;
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
           (worldRuntime->dwordArray != (dword *)0x0)) {
          soundVoiceSet = (DirectSoundVoiceSet **)worldRuntime->dwordArray[soundAssetIndex];
          if (soundVoiceSet != (DirectSoundVoiceSet **)0x0) {
            testResult = TerrainGrid_TestProjectedCellMaskBits01Cf
                              ((modelNode1->worldTransform).translation.y,
                               (modelNode1->worldTransform).translation.x,worldRuntime);
            if (!testResult) {
              SpatialSound_PlayPositionedOneShot
                        (deploymentDefinition->positionedSoundMaximumDistanceQ12,deploymentDefinition->positionedSoundGainQ15,
                         &(modelNode1->worldTransform).translation,soundVoiceSet);
            }
          }
        }
      }
      if (modelNode1->childNodes[0] != (ModelRuntimeNode *)0x0) {
        travelLimitOrStep = deploymentDefinition->verticalDeploymentStepQ12PerTick * g_InGameSimulationStepTicks;
        childTranslationZ = &(modelNode1->childNodes[0]->modelPayload).localTranslationZQ12;
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
   Ownership: gameplay/army/runtime.
   Purpose: The helper scans model lookup records, resolves a matching model attachment, derives transformed launch
   state, and calls ShotDefinition_ComputeLaunchAnglesRegs. CF clear reports a match; CF set reports no match. The
   normal return is void and all exits use RET 0x24. Role: Finds one exact packed attachment selector and launches
   a projectile from it. Inputs: Attachment selector, model hierarchy, ShotDefinition, target and owner/world
   state.
   Cross-module calls: ModelNodeRuntime_RebuildTransformsFromRoot [world/model/hierarchy],
   ModelNodeRuntime_TransformLocalPointRegs [world/model/hierarchy], ShotRuntimePool_CreateProjectileFromDefinition
   [world/shots/runtime].
*/
bool __thandor_cf_preserve_eax_ecx_edx
ArmyRuntime_ResolveShotLaunchFromModelAttachmentCf
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
  ModelLocalPointRegs12 launchPoint;
  
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
  launchWorldZQ12 = launchPoint.edx;
  launchWorldYQ12 = launchPoint.ecx;
  ShotRuntimePool_CreateProjectileFromDefinition
            (shotRuntimeState14,
             (ArmyRuntimeSlot *)((modelNode->runtimePayload).armyRuntime)->linkedEntityRuntime,
             targetWorldXQ12,targetWorldYQ12,targetWorldZQ12,launchWorldZQ12,launchWorldYQ12,
             launchPoint.eax,shotDefinition,worldRuntime);
  return false;
}


/* Address: 0x00529B50.
   Ownership: gameplay/army/runtime.
   Purpose: Computes and clamps the activation metric from definition and runtime state, stores it atomically, and
   plays the configured positioned start sound on a zero-to-nonzero transition when the projected terrain cell is
   available.
   Cross-module calls: TerrainGrid_TestProjectedCellMaskBits01Cf [world/terrain/grid],
   SpatialSound_PlayPositionedOneShot [audio/spatial/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_UpdateActivationMetricAndPlayStartSound
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  GraphicsFixedVec3 *worldPosition;
  int previousAdvance;
  ModelRuntimeSlot *linkedModelRuntime;
  AngleTurn32 rotationAngle1;
  DirectSoundVoiceSet **voiceSetRef;
  uint angleOrAdvance;
  uint advanceOrSoundIndex;
  bool cellMasked;
  ModelRuntimeNode *modelNode1;
  
  modelNode1 = armyRuntime->modelNodeRuntime;
  linkedModelRuntime = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  rotationAngle1 = (modelNode1->modelPayload).worldRotationAngle1;
  advanceOrSoundIndex = linkedModelRuntime->attachmentCount0C;
  if ((int)rotationAngle1 < (int)linkedModelRuntime[1].classLinkState.classState68) {
    angleOrAdvance = (modelNode1->modelPayload).worldRotationAngle2 -
            (modelNode1->modelPayload).worldRotationAngle0 & 0xffff;
    advanceOrSoundIndex = advanceOrSoundIndex * 5 >> 4;
    if (((*(int *)(linkedModelRuntime->reserved10_37 + 0x14) <= (int)rotationAngle1) &&
        (advanceOrSoundIndex = linkedModelRuntime->attachmentCount0C, 0x4000 < angleOrAdvance)) && (angleOrAdvance < 0xc000)) {
      advanceOrSoundIndex = advanceOrSoundIndex * 5 >> 3;
    }
  }
  angleOrAdvance = (armyRuntime->movementControl).movementAdvancePerTickQ12;
  if ((angleOrAdvance < advanceOrSoundIndex) && (angleOrAdvance = angleOrAdvance + *(int *)(linkedModelRuntime->reserved10_37 + 8), angleOrAdvance < advanceOrSoundIndex)) {
    advanceOrSoundIndex = angleOrAdvance;
  }
  LOCK();
  previousAdvance = (armyRuntime->movementControl).movementAdvancePerTickQ12;
  (armyRuntime->movementControl).movementAdvancePerTickQ12 = advanceOrSoundIndex;
  UNLOCK();
  if ((previousAdvance == 0) && (advanceOrSoundIndex != 0)) {
    advanceOrSoundIndex = *(uint *)(linkedModelRuntime->classState).reservedD4_DB;
    if ((advanceOrSoundIndex != 0) &&
       ((advanceOrSoundIndex < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != (dword *)0x0)))) {
      voiceSetRef = (DirectSoundVoiceSet **)worldRuntime->dwordArray[advanceOrSoundIndex];
      worldPosition = &(modelNode1->worldTransform).translation;
      if (voiceSetRef != (DirectSoundVoiceSet **)0x0) {
        cellMasked = TerrainGrid_TestProjectedCellMaskBits01Cf
                          ((modelNode1->worldTransform).translation.y,worldPosition->x,worldRuntime)
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
   Ownership: gameplay/army/runtime.
   Purpose: Handles a resolved collision partner. The class-0x17 path links compatible same-faction targets and
   initializes their state, while the class-zero path computes a bearing and invokes the verified collision-damage
   helper. Typed parameters: p3 currentWorldXQ12→Q12, p4 currentWorldYQ12→Q12. Nearby but non-identical semantic
   domains were explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals,
   locals, and executable data remain unchanged.
   Cross-module calls: FixedMath_Atan2Angle16 [core/math/fixed], ArmyRuntime_ApplyImpactDamageAndFinalizeState
   [gameplay/army/combat].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_HandleCollisionPartner
          (ArmyRuntimeSlot *currentArmyRuntime,Q12 currentWorldXQ12,Q12 currentWorldYQ12,
          ArmyRuntimeSlot *collisionPartnerArmyRuntime,WorldRuntimeContext *worldRuntime)

{
  Q12 *fallbackPositionField;
  dword impactAngle;
  
  if (collisionPartnerArmyRuntime != (ArmyRuntimeSlot *)0x0) {
    if (((collisionPartnerArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
        definitionValue9C_4C == 0x17) {
      if (((collisionPartnerArmyRuntime->linkedEntityRuntime->common).ownership.ownerIndex ==
           (currentArmyRuntime->linkedEntityRuntime->common).ownership.ownerIndex) &&
         (collisionPartnerArmyRuntime->linkedArmyRuntime == (ArmyRuntimeSlot *)0x0)) {
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
             definitionValue9C_4C == 0) {
      impactAngle = FixedMath_Atan2Angle16
                              ((collisionPartnerArmyRuntime->modelNodeRuntime->worldTransform).
                               translation.y - currentWorldXQ12,
                               (collisionPartnerArmyRuntime->modelNodeRuntime->worldTransform).
                               translation.x - currentWorldYQ12);
      ArmyRuntime_ApplyImpactDamageAndFinalizeState
                (impactAngle,0x100000,collisionPartnerArmyRuntime);
    }
  }
  return;
}


/* Address: 0x0051BC00.
   Ownership: gameplay/army/runtime.
   Purpose: Creates a temporary army runtime instance, prepares its model view and transformed bounds, renders a
   generated preview texture, applies the verified MMX reduction and color postprocess, destroys the temporary
   instance, and returns status through carry. Typed parameters: p2 previewHeight→GraphicsPixelDimension_V302.
   Nearby but non-identical semantic domains were explicitly deferred. Calling convention, parameter storage, body
   bytes, control flow, globals, locals, and executable data remain unchanged. Typed parameters: p3
   previewWidth→GraphicsPixelDimension_V302.
   Local calls: ArmyRuntime_CreateInstanceFromAssetCf, ArmyRuntime_DestroyInstanceAndRefreshUi.
   Cross-module calls: WorldRuntime_UnlinkNodeFromOwnerListD8 [world/runtime/core],
   ModelNodeRuntime_RebuildTransformsFromRoot [world/model/hierarchy],
   ModelNodeRuntime_AccumulateTransformedBoundsRecursive [world/model/hierarchy].
*/
/* One 16-bit MMX lane per pixel byte: PUNPCKLBW mm,mm duplicates each byte into a word, PSRLW 4 scales it. */
#define ARMY_PREVIEW_UNPACK_BYTE_LANE(pixel, byteIndex) \
  ((ulonglong)((((pixel) >> ((byteIndex) * 8)) & 0xffu) * 0x101u >> 4) << ((byteIndex) * 16))
#define ARMY_PREVIEW_UNPACK_PIXEL_LANES(pixel) \
  (ARMY_PREVIEW_UNPACK_BYTE_LANE(pixel, 3) | ARMY_PREVIEW_UNPACK_BYTE_LANE(pixel, 2) | \
   ARMY_PREVIEW_UNPACK_BYTE_LANE(pixel, 1) | ARMY_PREVIEW_UNPACK_BYTE_LANE(pixel, 0))

ArmyPreviewTextureEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ArmyRuntime_RenderPreviewTextureCf
          (GraphicsPixelDimension previewHeight,GraphicsPixelDimension previewWidth,
          FactionRuntimeIndex factionIndex,PckArmyAssetIdCatalog armyAssetId,
          WorldRuntimeContext *worldRuntime)

{
  ulonglong alphaReciprocal;
  ModelRuntimeNode *rootNodeOrSize;
  uint pixelTopLeft;
  uint pixelTopRight;
  uint pixelBottomLeft;
  uint pixelBottomRight;
  GraphicsPixelDimension savedPreviewWidth;
  GameEntityRuntime *entityRuntime1;
  int boundsSpanY;
  GameEntityRuntime *memory;
  void *halvedWidth;
  int boundsSpanZ;
  int maxBoundsSpan;
  Q12 *sourcePixels;
  Q12 *destinationPixels;
  ushort topLeftAlphaOrSum0;
  ushort topRightAlphaOrClamp0;
  ushort bottomLeftAlphaOrSum1;
  ushort bottomRightAlphaOrClamp1;
  ushort channelSum2;
  ushort channelClamp2;
  ulonglong mm0PackedValue0;
  ushort channelSum3;
  ushort channelClamp3;
  ulonglong mm1PackedValue0;
  ulonglong mm2PackedValue0;
  ulonglong mm3PackedValue0;
  ArmyRuntimeCreateEaxCf5 createResult;
  GraphicsOffscreenAllocationEaxCf5 offscreenResult;
  ArmyPreviewTextureEaxCf5 successResult;
  ArmyPreviewTextureEaxCf5 failureResult;
  
  savedPreviewWidth = previewWidth;
  createResult = ArmyRuntime_CreateInstanceFromAssetCf
                     (1,0,0x6000000,0x6000000,factionIndex,armyAssetId,worldRuntime);
  entityRuntime1 = (GameEntityRuntime *)createResult.eax;
  if (!createResult.carry) {
    rootNodeOrSize = (entityRuntime1->common).ownership.modelNode;
    if (((*(int *)(*(int *)(entityRuntime1->common).ownership.definitionOrClassRecord + 0x4c) == 0xd
         ) && (3 < rootNodeOrSize->childCount)) && (rootNodeOrSize->childNodes[3] != (ModelRuntimeNode *)0x0)) {
      WorldRuntime_UnlinkNodeFromOwnerListD8((WorldOwnerListNode100 *)rootNodeOrSize->childNodes[3]);
      rootNodeOrSize->childNodes[3] = (ModelRuntimeNode *)0x0;
    }
    (rootNodeOrSize->modelPayload).worldRotationAngle2 = 0x2000;
    (rootNodeOrSize->modelPayload).worldRotationAngle1 = 0x3000;
    rootNodeOrSize->runtimeFlags = rootNodeOrSize->runtimeFlags | 1;
    rootNodeOrSize->tintArgb = 0xffffffff;
    ModelNodeRuntime_RebuildTransformsFromRoot(rootNodeOrSize);
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
    g_ArmyPreviewModelNodePointer = (dword)rootNodeOrSize;
    offscreenResult = (*g_GraphicsOffscreenRenderModelListToTextureSourceCf)
                       ((GraphicsOffscreenSceneExtents *)&g_ArmyPreviewPrimaryColorArgb,
                        &g_ArmyPreviewAuxiliaryOrientation0,
                        (GraphicsOffscreenViewParameters *)&g_ArmyPreviewViewOriginXQ12,
                        previewHeight * 2,previewWidth * 2,1,
                        (ModelRuntimeNode **)&g_ArmyPreviewModelNodePointer);
    memory = offscreenResult.allocation;
    if (!offscreenResult.carry) {
      sourcePixels = &memory[1].common.commandTarget.targetWorldXQ12;
      ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,entityRuntime1);
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
                      ((ulonglong *)&g_ArmyPreviewAlphaPremultiplyMmxLut256)[pixelTopLeft >> 0x18]);
          mm1PackedValue0 =
               pmulhw(ARMY_PREVIEW_UNPACK_PIXEL_LANES(pixelTopRight),
                      ((ulonglong *)&g_ArmyPreviewAlphaPremultiplyMmxLut256)[pixelTopRight >> 0x18]);
          mm2PackedValue0 =
               pmulhw(ARMY_PREVIEW_UNPACK_PIXEL_LANES(pixelBottomLeft),
                      ((ulonglong *)&g_ArmyPreviewAlphaPremultiplyMmxLut256)[pixelBottomLeft >> 0x18]);
          mm3PackedValue0 =
               pmulhw(ARMY_PREVIEW_UNPACK_PIXEL_LANES(pixelBottomRight),
                      ((ulonglong *)&g_ArmyPreviewAlphaPremultiplyMmxLut256)[pixelBottomRight >> 0x18]);
          alphaReciprocal =
               ((ulonglong *)&g_ArmyPreviewAverageAlphaReciprocalMmxLut256)
               [(pixelTopLeft >> 0x18) + (pixelTopRight >> 0x18) + (pixelBottomLeft >> 0x18) +
                (pixelBottomRight >> 0x18) >> 2];
          topLeftAlphaOrSum0 = ((short)mm0PackedValue0 + (short)mm1PackedValue0 +
                    (short)mm2PackedValue0 + (short)mm3PackedValue0 +
                   (short)g_ArmyPreviewDownsampleAlphaRoundingBiasMmx) * (short)alphaReciprocal;
          bottomLeftAlphaOrSum1 = ((short)((ulonglong)mm0PackedValue0 >> 0x10) +
                    (short)((ulonglong)mm1PackedValue0 >> 0x10) +
                    (short)((ulonglong)mm2PackedValue0 >> 0x10) +
                    (short)((ulonglong)mm3PackedValue0 >> 0x10) +
                   (short)((ulonglong)g_ArmyPreviewDownsampleAlphaRoundingBiasMmx >> 0x10)) *
                   (short)((ulonglong)alphaReciprocal >> 0x10);
          channelSum2 = ((short)((ulonglong)mm0PackedValue0 >> 0x20) +
                    (short)((ulonglong)mm1PackedValue0 >> 0x20) +
                    (short)((ulonglong)mm2PackedValue0 >> 0x20) +
                    (short)((ulonglong)mm3PackedValue0 >> 0x20) +
                   (short)((ulonglong)g_ArmyPreviewDownsampleAlphaRoundingBiasMmx >> 0x20)) *
                   (short)((ulonglong)alphaReciprocal >> 0x20);
          channelSum3 = ((short)((ulonglong)mm0PackedValue0 >> 0x30) +
                    (short)((ulonglong)mm1PackedValue0 >> 0x30) +
                    (short)((ulonglong)mm2PackedValue0 >> 0x30) +
                    (short)((ulonglong)mm3PackedValue0 >> 0x30) +
                   (short)((ulonglong)g_ArmyPreviewDownsampleAlphaRoundingBiasMmx >> 0x30)) *
                   (short)((ulonglong)alphaReciprocal >> 0x30);
          topRightAlphaOrClamp0 = topLeftAlphaOrSum0 >> 8;
          bottomRightAlphaOrClamp1 = bottomLeftAlphaOrSum1 >> 8;
          channelClamp2 = channelSum2 >> 8;
          channelClamp3 = channelSum3 >> 8;
          /* PSRLW 8 leaves every lane <= 0xFF, so PACKUSWB never saturates: it just packs the low bytes. */
          *destinationPixels = (dword)(byte)channelClamp3 << 24 | (dword)(byte)channelClamp2 << 16 |
                               (dword)(byte)bottomRightAlphaOrClamp1 << 8 | (dword)(byte)topRightAlphaOrClamp0;
          sourcePixels = sourcePixels + 2;
          destinationPixels = destinationPixels + 1;
          previewWidth = previewWidth - 1;
        } while (previewWidth != 0);
        sourcePixels = sourcePixels + savedPreviewWidth * 2;
        previewHeight = previewHeight - 1;
        previewWidth = savedPreviewWidth;
      } while (previewHeight != 0);
      halvedWidth = (void *)((int)memory[1].common.commandFlags >> 1);
      entityRuntime1 = (GameEntityRuntime *)((int)memory[1].common.commandTarget.targetEntity >> 1);
      memory[1].common.commandFlags = (GameEntityCommandFlags)halvedWidth;
      memory[1].common.commandTarget.targetEntity = entityRuntime1;
      memory[1].common.ownership.definitionOrClassRecord = halvedWidth;
      memory[1].common.ownership.modelNode = (ModelRuntimeNode *)entityRuntime1;
      rootNodeOrSize = (ModelRuntimeNode *)((int)halvedWidth * (int)entityRuntime1 * 4 + 0x220);
      (memory->common).ownership.modelNode = rootNodeOrSize;
      (*g_MemoryApi.shrinkInPlace)((dword)rootNodeOrSize,memory);
      successResult.carry = false;
      successResult.previewTexture = (GraphicsTextureResource *)memory;
      return successResult;
    }
    ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,entityRuntime1);
    entityRuntime1 = memory;
  }
  failureResult.carry = true;
  failureResult.previewTexture = (GraphicsTextureResource *)entityRuntime1;
  return failureResult;
}


/* Address: 0x0052A7C0.
   Ownership: gameplay/army/runtime.
   Purpose: Executes the class-indexed timed-emitter callback, advances progress, health, resource, and technology
   state, triggers configured effects, and recursively updates child runtimes.
   [RESOURCE_FUEL_ENERGY_CAPACITY_SEPARATION_CLOSURE] Research state: pending technology pays Xenite once from
   faction xeniteCurrentQ4, then adds technology Energy requirement to ArmyRuntime active load while research is in
   progress; completion unlocks the technology and removes that Energy load.
   Local calls: ArmyRuntimeSpawner_ComputeRemainingLinkedAssetMetric, ArmyRuntime_ProcessReadyAttachmentChannels.
   Cross-module calls: EffectRuntimePool_CreateInstanceFromDefinitionCf [world/effects/runtime],
   Technology_UnlockForFaction [gameplay/technology/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive
          (WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  dword *classStateField;
  dword previousHealth;
  ModelRuntimeNode *rootNode;
  int energyRequirement;
  ModelRuntimeSlot *tickCursor;
  uint clampedHealth;
  ModelRuntimeSlot *tickCountOrChild;
  int factionOrProgress;
  dword definitionOrCount;
  uint limitOrFlags;
  
  definitionOrCount = (modelRuntime->definitionOrSavedId).savedIdOrOffset;
  (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[*(int *)(definitionOrCount + 0x4c)])
            (worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  if (((modelRuntime->classState).classStateEC & 0x10) == 0) {
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
       (classStateField = &(modelRuntime->classState).classStateFC, *classStateField = *classStateField + 0xc,
       ((modelRuntime->classState).classStateEC & 0x10) != 0)) && (*(int *)(definitionOrCount + 0x184) != 0)) &&
     ((*(int *)(definitionOrCount + 0x60) != 0 && (0 < (int)modelRuntime->definitionValue60_3C)))) {
    factionOrProgress = ((modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime)->factionIndex;
    g_GameFactionRuntimeImage.records[factionOrProgress].xeniteCurrentQ4 =
         g_GameFactionRuntimeImage.records[factionOrProgress].xeniteCurrentQ4 + (*(uint *)(definitionOrCount + 0x184) >> 5);
    limitOrFlags = *(uint *)(definitionOrCount + 0x60);
    if (*(int *)(definitionOrCount + 0x4c) == 0x16) {
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
      *classStateField = *classStateField ^ 0x210;
      rootNode = (modelRuntime->rootModelNodeOrSavedOffset).modelNode;
      EffectRuntimePool_CreateInstanceFromDefinitionCf
                (EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY,
                 THANDOR_BITCAST(ModelRuntimeSlot *, EffectRuntimeOwnerReference4, modelRuntime),
                 (rootNode->modelPayload).worldRotationAngle2,
                 (rootNode->modelPayload).worldRotationAngle1,
                 (rootNode->modelPayload).worldRotationAngle0,(rootNode->worldTransform).translation.z,
                 (rootNode->worldTransform).translation.y,(rootNode->worldTransform).translation.x,
                 *(EffectDefinition **)(definitionOrCount + 400),worldRuntime);
      /* Setting 0x20 also skips the attachment tick loop below (the original jumps past it). */
      classStateField = &(modelRuntime->classState).classStateEC;
      *classStateField = *classStateField | 0x20;
    }
  }
  if ((((modelRuntime->classState).classStateEC & 0x20) == 0) &&
     ((int)modelRuntime->definitionValue60_3C < 1)) {
    tickCountOrChild = (ModelRuntimeSlot *)g_InGameSimulationStepTicks;
    tickCursor = (modelRuntime->linkedModelRuntimeOrSavedOffset).modelRuntime;
    while (tickCursor != (ModelRuntimeSlot *)0x0) {
      ArmyRuntime_ProcessReadyAttachmentChannels(worldRuntime,modelRuntime);
      modelRuntime->definitionValue84_40 = modelRuntime->definitionValue84_40 - 1;
      modelRuntime->definitionValue88_44 = modelRuntime->definitionValue88_44 - 1;
      modelRuntime->definitionValue94_48 = modelRuntime->definitionValue94_48 - 1;
      modelRuntime->definitionValue9C_4C = modelRuntime->definitionValue9C_4C - 1;
      modelRuntime->definitionValueA4_50 = modelRuntime->definitionValueA4_50 - 1;
      modelRuntime->definitionValueAC_54 = modelRuntime->definitionValueAC_54 - 1;
      modelRuntime->definitionValueB4_58 = modelRuntime->definitionValueB4_58 - 1;
      modelRuntime->definitionValueBC_5C = modelRuntime->definitionValueBC_5C - 1;
      tickCountOrChild = (ModelRuntimeSlot *)((int)&tickCountOrChild[-1].attachments140[5].reserved1C + 3);
      tickCursor = tickCountOrChild;
    }
  }
  if ((((modelRuntime->classState).classStateEC & 0x40) != 0) &&
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
      *classStateField = *classStateField & 0xffffffbf;
      classStateField = &(modelRuntime->classState).definitionDerivedValueF4;
      *classStateField = *classStateField - factionOrProgress;
    }
  }
  if ((((modelRuntime->classState).classStateEC & 0x80) != 0) &&
     (((modelRuntime->classState).classStateEC & 0x140) == 0)) {
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
      *classStateField = *classStateField ^ 0xc0;
      classStateField = &(modelRuntime->classState).definitionDerivedValueF4;
      *classStateField = *classStateField + energyRequirement;
    }
  }
  definitionOrCount = modelRuntime->attachmentCount0C;
  limitOrFlags = (modelRuntime->classState).classStateEC;
  if (definitionOrCount != 0) {
    do {
      tickCountOrChild = modelRuntime->attachments140[0].childModelRuntimeOrSavedOffset00;
      if (tickCountOrChild != (ModelRuntimeSlot *)0x0) {
        classStateField = &(tickCountOrChild->classState).classStateEC;
        *classStateField = *classStateField | limitOrFlags & 8;
        ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive(worldRuntime,tickCountOrChild);
      }
      modelRuntime = (ModelRuntimeSlot *)(modelRuntime->reserved10_37 + 0x10);
      definitionOrCount = definitionOrCount - 1;
    } while (definitionOrCount != 0);
  }
  return;
}


/* Address: 0x00527010.
   Ownership: gameplay/army/runtime.
   Purpose: Finds an unused linked-asset slot, creates the corresponding army runtime instance, attaches it to the
   parent, copies the command payload, and scales its inherited metric. Stock ARM contains 675 records and 326
   unique ids; placement workspace, producer, tier, class, and faction-role semantics are not inferred from numeric
   adjacency. Typed parameters: p2 inheritedValue78→WorldMotionValue78_V344, p3
   inheritedValue74→WorldMotionValue74_V344, p4 inheritedValue70→WorldMotionValue70_V344. Calling convention,
   complete VariableStorage serialization, function bytes, control flow, globals, locals, and executable data
   remain unchanged.
   Local calls: ArmyRuntime_CreateInstanceFromAssetCf.
   Cross-module calls: ModelNodeRuntime_RebuildTransformsFromRoot [world/model/hierarchy].
*/
bool __thandor_cf_preserve_eax_ecx_edx
ArmyRuntimeSpawner_CreateLinkedChildInstanceCf
          (WorldMotionValue78 inheritedValue78,WorldMotionValue74 inheritedValue74,
          WorldMotionValue70 inheritedValue70,PckArmyAssetIdCatalog linkedArmyAssetId,
          WorldRuntimeContext *worldRuntime,ArmyRuntimeLinkedChildMaskSlotView *armyRuntime)

{
  ArmyRuntimeLinkedChildSlotMaskState4 *slotMaskState;
  int *childModelRuntime;
  ModelRuntimeNode *parentRootNode;
  int remainingSlots;
  uint slotBit;
  ArmyRuntimeLinkedChildMaskSlotView *slotCursor;
  ArmyRuntimeCreateEaxCf5 createResult;
  ModelRuntimeNode *modelNode1;
  
  slotBit = 1;
  remainingSlots = *(int *)((int)armyRuntime->definitionOrAsset + 0xc4);
  slotCursor = armyRuntime;
  while ((linkedArmyAssetId != slotCursor->movementTarget0Q12 ||
         (((armyRuntime->articulatedContact).linkedChildSlotMaskState.linkedChildSlotMask & slotBit)
          != 0))) {
    slotCursor = (ArmyRuntimeLinkedChildMaskSlotView *)&slotCursor->modelNodeRuntime;
    slotBit = slotBit * 2;
    remainingSlots = remainingSlots + -1;
    if (remainingSlots == 0) {
      return true;
    }
  }
  modelNode1 = armyRuntime->modelNodeRuntime;
  createResult = ArmyRuntime_CreateInstanceFromAssetCf
                    (0,(modelNode1->modelPayload).worldRotationAngle2,
                     (modelNode1->worldTransform).translation.y,
                     (modelNode1->worldTransform).translation.x,
                     (armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex,
                     linkedArmyAssetId,worldRuntime);
  if (createResult.carry) {
    return true;
  }
  childModelRuntime = *(int **)createResult.eax;
  slotMaskState = &(armyRuntime->articulatedContact).linkedChildSlotMaskState;
  slotMaskState->linkedChildSlotMask = slotMaskState->linkedChildSlotMask | slotBit;
  armyRuntime->fallbackWorldYQ12 = armyRuntime->fallbackWorldYQ12 + -1;
  childModelRuntime[0x18] = (int)armyRuntime;
  childModelRuntime[0x2e] = 1;
  modelNode1 = (ModelRuntimeNode *)childModelRuntime[1];
  childModelRuntime[0x1c] = inheritedValue70;
  childModelRuntime[0x1d] = inheritedValue74;
  parentRootNode = armyRuntime->modelNodeRuntime;
  childModelRuntime[0x1e] = inheritedValue78;
  (modelNode1->childNodes[0]->modelPayload).localTranslationZQ12 =
       (parentRootNode->childNodes[0]->modelPayload).localTranslationZQ12;
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNode1);
  childModelRuntime[0xf] = (int)(((longlong)armyRuntime->actionVector2Q12 * (longlong)*(int *)(*childModelRuntime + 0x60))
                     / (longlong)*(int *)((int)armyRuntime->definitionOrAsset + 0x60));
  return false;
}


/* Address: 0x00527430.
   Ownership: gameplay/army/runtime.
   Purpose: Tests proximity across the base model and its attachment descriptors for two army runtimes; status is
   returned through CF.
   Local calls: ArmyRuntime_TestPositionDistanceWithinCombinedRadiusCf.
*/
bool __thandor_cf_preserve_eax_ecx_edx
ArmyRuntime_TestModelAttachmentProximityCf
          (ArmyRuntimeSlot *candidateArmyRuntime,ArmyRuntimeSlot *sourceArmyRuntime)

{
  ModelRuntimeSlot *candidateModelRuntime;
  int *attachmentRecord;
  int remainingAttachments;
  bool baseWithinRadius;
  bool result;
  
  candidateModelRuntime = (candidateArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  remainingAttachments = sourceArmyRuntime->factionIndex;
  baseWithinRadius = ArmyRuntime_TestPositionDistanceWithinCombinedRadiusCf
                    ((UQ12)candidateModelRuntime->attachments140[3].childModelRuntimeOrSavedOffset00,
                     (UQ12)((sourceArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
                           attachments140[3].childModelRuntimeOrSavedOffset00,
                     candidateArmyRuntime->modelNodeRuntime,sourceArmyRuntime->modelNodeRuntime);
  result = false;
  if (baseWithinRadius) {
    for (; remainingAttachments != 0; remainingAttachments = remainingAttachments + -1) {
      attachmentRecord = (int *)sourceArmyRuntime[1].commandCoordinate0Q12;
      if ((attachmentRecord != (int *)0x0) &&
         (result = ArmyRuntime_TestPositionDistanceWithinCombinedRadiusCf
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
   Ownership: gameplay/army/runtime.
   Purpose: Releases an army runtime model instance, removes selection and global references, clears the identifier
   from faction technology tables, marks the runtime slot free, and refreshes the two catalog grids and selection-
   detail panel.
   Cross-module calls: ModelRuntimePool_DestroyHierarchyAndDetach [world/model/runtime],
   SelectionPlayerBlocks_RemovePointer [gameplay/selection/runtime], WorldRuntime_ForEachNodeInOwnerListD8
   [world/runtime/core], GameFactionRuntime_ClearRuntimeGroupMemberPointerFromAllFactionTables
   [gameplay/faction/runtime], UiCatalogGroup48_RebuildGrid [ui/ingame/technology], UiCatalogGroup42_RebuildGrid
   [ui/ingame/technology].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_DestroyInstanceAndRefreshUi
          (WorldRuntimeContext *worldRuntime,GameEntityRuntime *entityRuntime)

{
  ModelRuntimeSlot *modelRuntime;
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  FrontendPlayerRuntimeRecord *playerBlockCursor;
  
  modelRuntime = (entityRuntime->common).ownership.definitionOrClassRecord;
  if (modelRuntime != (ModelRuntimeSlot *)0x0) {
    (entityRuntime->common).ownership.definitionOrClassRecord = (void *)0x0;
    ModelRuntimePool_DestroyHierarchyAndDetach(worldRuntime,modelRuntime);
  }
  SelectionPlayerBlocks_RemovePointer(entityRuntime);
  if (entityRuntime == (worldRuntime->selection).selectedEntity) {
    (worldRuntime->selection).selectedEntity = (GameEntityRuntime *)0x0;
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
    playerBlockCursor = playerBlockCursor + 1;
    remainingBlocks = remainingBlocks - 1;
  } while (remainingBlocks != 0);
  GameFactionRuntime_ClearRuntimeGroupMemberPointerFromAllFactionTables(entityRuntime);
  (entityRuntime->common).ownership.modelNode = (ModelRuntimeNode *)0x0;
  UiCatalogGroup48_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
  UiCatalogGroup42_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
  InGameSelectionDetailPanel_Rebuild();
  return;
}


/* Address: 0x005246B0.
   Ownership: gameplay/army/runtime.
   Purpose: Tests the class-13 candidate relation and distance/proximity conditions used by the Group-A command
   handler; status is returned through CF.
   Cross-module calls: ModelLookupTable_ContainsPackedKeyCf [assets/model/definitions],
   ModelNodeRuntime_TransformLocalPointRegs [world/model/hierarchy], FixedMath_Length2 [core/math/fixed].
*/
bool __thandor_cf_preserve_eax_ecx_edx
ArmyRuntime_TestClass13ProximityCandidateCf
          (ArmyRuntimeSlot *candidateArmyRuntime,ArmyRuntimeSlot *sourceArmyRuntime)

{
  ModelRuntimeSlot *attachmentChildRuntime;
  ModelRuntimeNode *modelNodeRuntime;
  dword anchorDistance;
  ModelLookupEntryEaxCf5 lookupEntry;
  ModelLocalPointRegs12 anchorPoint;
  ModelRuntimeNode *modelNode1;
  
  if (((sourceArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C == 0xd) {
    attachmentChildRuntime = ((candidateArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->attachments140[3].
             childModelRuntimeOrSavedOffset00;
    modelNodeRuntime = sourceArmyRuntime->modelNodeRuntime;
    modelNode1 = candidateArmyRuntime->modelNodeRuntime;
    lookupEntry = ModelLookupTable_ContainsPackedKeyCf(1,5,(modelNodeRuntime->modelPayload).modelResource)
    ;
    if (!lookupEntry.carry) {
      anchorPoint = ModelNodeRuntime_TransformLocalPointRegs(lookupEntry.entry,modelNodeRuntime);
      anchorDistance = FixedMath_Length2(anchorPoint.ecx - (modelNode1->worldTransform).translation.y,
                                anchorPoint.eax - (modelNode1->worldTransform).translation.x);
      if ((int)anchorDistance <= (int)(attachmentChildRuntime + 6)) {
        return true;
      }
    }
  }
  return false;
}


/* Address: 0x00526510.
   Ownership: gameplay/army/runtime.
   Purpose: Typed parameters: p3 soundAssetIndex→SoundAssetIndex_V343. Calling convention, complete VariableStorage
   serialization, function bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: GameFactionRuntime_TestCapabilityBitClearCf [gameplay/faction/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_TryPlayMappedTerrainSoundAtWorldPoint
          (FactionRuntimeIndex factionIndex,Q12 worldYQ12,Q12 worldXQ12,
          SoundAssetIndex soundAssetIndex,WorldRuntimeContext *worldContext)

{
  DirectSoundVoiceSet **voiceSetRef;
  FieldGridDimension gridWidthCells;
  dword capabilityBitIndex;
  int cellColumn;
  uint projectedRow;
  int cellRow;
  bool capabilityClear;
  FieldGridAsset *fieldGrid1;
  
  if ((((soundAssetIndex != 0) && (worldContext->dwordArray != (dword *)0x0)) &&
      (soundAssetIndex < worldContext->dwordArrayCount)) &&
     (voiceSetRef = (DirectSoundVoiceSet **)worldContext->dwordArray[soundAssetIndex],
     voiceSetRef != (DirectSoundVoiceSet **)0x0)) {
    fieldGrid1 = worldContext->fieldGrid;
    projectedRow = (int)((ulonglong)((longlong)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
            (uint)((longlong)worldYQ12 * -0x20c8cc) >> 0x15;
    gridWidthCells = fieldGrid1->gridWidth;
    cellColumn = (int)((((int)((ulonglong)((longlong)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                   (uint)((longlong)worldXQ12 * 0x1c6e9c) >> 0x14) - projectedRow) + 0x800) >> 0xc;
    if (((-1 < cellColumn) && (cellRow = (int)(projectedRow * 2 + 0x800) >> 0xc, -1 < cellRow)) &&
       ((cellColumn < (int)gridWidthCells && (cellRow < (int)fieldGrid1->gridHeight)))) {
      capabilityBitIndex = worldContext->activeFactionRuntimeIndex;
      if (factionIndex != capabilityBitIndex) {
        capabilityClear = GameFactionRuntime_TestCapabilityBitClearCf(capabilityBitIndex,factionIndex);
        if (((capabilityClear) &&
            ((fieldGrid1->cells[gridWidthCells * cellRow + cellColumn].runtime60_6B[capabilityBitIndex + 0x10] &
             0x10) != 0)) &&
           (0x10 < g_GameFactionRuntimeImage.records[capabilityBitIndex].relationTransitionTick)) {
          g_GameFactionRuntimeImage.records[capabilityBitIndex].relationTransitionTick = 0;
          (*g_SoundPlayOneShot)
                    (g_SoundEffectsGainQ15,g_SoundEffectsGainQ15,*voiceSetRef);
        }
      }
    }
  }
  return;
}


/* Address: 0x005271A0.
   Ownership: gameplay/army/runtime.
   Purpose: Handles army runtime spawner compute remaining linked asset metric.
   Cross-module calls: ArmyAssetRegistry_FindByIdCf [assets/army/catalog],
   ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf [assets/model/definitions].
*/
uint __thandor_void_preserve_eax_ecx
ArmyRuntimeSpawner_ComputeRemainingLinkedAssetMetric
          (ArmyRuntimeLinkedChildMaskSlotView *armyRuntime)

{
  FactionRuntimeIndex factionIndex;
  int remainingSlots;
  uint metricSum;
  uint slotBit;
  ArmyRuntimeLinkedChildMaskSlotView *slotCursor;
  ArmyRegistryEaxCf5_51b6d0 assetLookup;
  ModelDefinitionLookupEaxCf5 definitionLookup;
  
  metricSum = 0;
  factionIndex = (armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex;
  slotBit = 1;
  remainingSlots = *(int *)((int)armyRuntime->definitionOrAsset + 0xc4);
  slotCursor = armyRuntime;
  do {
    if (((armyRuntime->articulatedContact).linkedChildSlotMaskState.linkedChildSlotMask & slotBit) ==
        0) {
      assetLookup = ArmyAssetRegistry_FindByIdCf(slotCursor->movementTarget0Q12);
      if (!assetLookup.carry) {
        definitionLookup = ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                          (factionIndex,(assetLookup.eax)->rootNodeOffsetOrPointer);
        metricSum = metricSum + definitionLookup.modelDefinition[0x20].flags;
      }
    }
    slotCursor = (ArmyRuntimeLinkedChildMaskSlotView *)&slotCursor->modelNodeRuntime;
    slotBit = slotBit * 2;
    remainingSlots = remainingSlots + -1;
  } while (remainingSlots != 0);
  return metricSum >> 5;
}


/* Address: 0x00527230.
   Ownership: gameplay/army/runtime.
   Purpose: Projects the spawned runtime position through the field mask and plays its configured one-shot creation
   sound when the selected sound resource is available.
   Cross-module calls: TerrainGrid_TestProjectedCellMaskBits01Cf [world/terrain/grid],
   SpatialSound_PlayPositionedOneShot [audio/spatial/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeSpawner_PlayCreationSound(ArmyRuntimeSlot *armyRuntime,WorldRuntimeContext *worldRuntime)

{
  ModelRuntimeSlot *linkedModelRuntime;
  ModelRuntimeNode *rootNode;
  uint soundAssetIndex;
  DirectSoundVoiceSet **voiceSetRef;
  bool cellMasked;
  
  linkedModelRuntime = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  rootNode = armyRuntime->modelNodeRuntime;
  soundAssetIndex = linkedModelRuntime[1].classLinkState.classState70;
  if (((soundAssetIndex != 0) && (soundAssetIndex < worldRuntime->dwordArrayCount)) &&
     (worldRuntime->dwordArray != (dword *)0x0)) {
    voiceSetRef = (DirectSoundVoiceSet **)worldRuntime->dwordArray[soundAssetIndex];
    if (voiceSetRef != (DirectSoundVoiceSet **)0x0) {
      cellMasked = TerrainGrid_TestProjectedCellMaskBits01Cf
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
   Ownership: gameplay/army/runtime.
   Purpose: Handles army runtime try spawn definition effect at world point.
   Cross-module calls: TerrainGrid_TestProjectedCellMaskBits01Cf [world/terrain/grid],
   SpatialSound_PlayPositionedOneShot [audio/spatial/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_TrySpawnDefinitionEffectAtWorldPoint
          (ArmyRuntimeSlot *armyRuntime,WorldRuntimeContext *worldContext)

{
  ModelRuntimeSlot *linkedModelRuntime;
  ModelRuntimeNode *rootNode;
  uint soundAssetIndex;
  DirectSoundVoiceSet **voiceSetRef;
  bool cellMasked;
  
  linkedModelRuntime = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  rootNode = armyRuntime->modelNodeRuntime;
  soundAssetIndex = linkedModelRuntime[1].classLinkState.armyLinkOrState6C.classState;
  if (((soundAssetIndex != 0) && (soundAssetIndex < worldContext->dwordArrayCount)) &&
     (worldContext->dwordArray != (dword *)0x0)) {
    voiceSetRef = (DirectSoundVoiceSet **)worldContext->dwordArray[soundAssetIndex];
    if (voiceSetRef != (DirectSoundVoiceSet **)0x0) {
      cellMasked = TerrainGrid_TestProjectedCellMaskBits01Cf
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
   Ownership: gameplay/army/runtime.
   Purpose: Compares squared XY distance between two position runtimes with the square of their combined Q12 radii
   and returns the outside/inside status through CF.
*/
bool __thandor_cf_preserve_eax_ecx_edx
ArmyRuntime_TestPositionDistanceWithinCombinedRadiusCf
          (UQ12 candidateRadiusQ12,UQ12 sourceRadiusQ12,void *candidatePositionRuntime,
          void *sourcePositionRuntime)

{
  int currentAxisDeltaQ12;
  int axisDeltaYQ12;
  longlong remainingRadiusSquaredAfterXQ24;
  longlong yDistanceSquaredQ24;
  
  currentAxisDeltaQ12 =
       *(int *)((int)sourcePositionRuntime + 0x94) - *(int *)((int)candidatePositionRuntime + 0x94);
  remainingRadiusSquaredAfterXQ24 =
       (longlong)(int)(sourceRadiusQ12 + candidateRadiusQ12) *
       (longlong)(int)(sourceRadiusQ12 + candidateRadiusQ12) -
       (longlong)currentAxisDeltaQ12 * (longlong)currentAxisDeltaQ12;
  if ((-1 < remainingRadiusSquaredAfterXQ24) &&
     (axisDeltaYQ12 = *(int *)((int)sourcePositionRuntime + 0x98) -
              *(int *)((int)candidatePositionRuntime + 0x98),
     yDistanceSquaredQ24 = (longlong)axisDeltaYQ12 * (longlong)axisDeltaYQ12,
     -1 < (int)(((int)((ulonglong)remainingRadiusSquaredAfterXQ24 >> 0x20) -
                (int)((ulonglong)yDistanceSquaredQ24 >> 0x20)) -
               (uint)((uint)remainingRadiusSquaredAfterXQ24 < (uint)yDistanceSquaredQ24)))) {
    return false;
  }
  return true;
}


/* Address: 0x005297D0.
   Ownership: gameplay/army/runtime.
   Purpose: Finds an eligible nearby candidate, resolves the requested model-point ordinal, transforms that point,
   and creates the indexed effect at the resulting world position. The stock corpus contains 140 unique
   EffectDefinition ids; serialized ids remain distinct from relocated EffectDefinition pointers and consumer-
   specific union facets. It is separate from world-unit radii, angles, grid indices, and serialized PCK
   identities. Typed parameters: p0 effectFlags→EffectCreationFlags_V304. Nearby but non-identical semantic domains
   were explicitly deferred.
   Cross-module calls: AiCombatTarget_EvaluateCandidateScore [gameplay/ai/combat], FixedMath_Length2
   [core/math/fixed], ModelNodeRuntime_RebuildTransformsFromRoot [world/model/hierarchy],
   ModelNodeRuntime_TransformLocalPointRegs [world/model/hierarchy], ShotRuntimePool_CreateProjectileFromDefinition
   [world/shots/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
          (EffectCreationFlagBits effectFlags,Q12 worldZQ12,Q12 worldYQ12,Q12 worldXQ12,
          ModelAttachmentOrdinal modelPointOrdinal,PckEffectDefinitionIdCatalog effectDefinitionId,
          void *sourceRuntime,void *modelPointTable,WorldRuntimeContext *worldContext)

{
  ArmyRuntimeSlot *sourceArmyRuntime;
  ArmyRuntimeSlot *candidateArmyRuntime;
  Q12 candidateWorldZ;
  uint candidateScore;
  dword candidateDistance;
  int x;
  uint currentBestScore;
  int y;
  int radiusOrCount;
  WorldOwnerListNode100 *worldNode1;
  ModelPackedPointRecord *localPointRecord;
  ModelLocalPointRegs12 localPoint;
  WorldOwnerListNode100 *worldNode2;
  uint pointOffsetMask;
  
  worldNode2 = (WorldOwnerListNode100 *)0x0;
  worldNode1 = worldContext->ownerListHead;
  pointOffsetMask = 0xffffffff;
  currentBestScore = 0;
  sourceArmyRuntime = *(ArmyRuntimeSlot **)(*(int *)((int)sourceRuntime + 0x48) + 8);
  do {
    if ((((worldNode1->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) && (worldNode1 != sourceRuntime))
        && (candidateArmyRuntime = *(ArmyRuntimeSlot **)((int)worldNode1->runtimePayload + 8),
           (*(uint *)((int)worldNode1->runtimePayload + 0xec) & 8) == 0)) &&
       ((candidateArmyRuntime->factionIndex != 0 &&
        (candidateArmyRuntime->factionIndex != sourceArmyRuntime->factionIndex)))) {
      sourceArmyRuntime->runtimeState4C =
           *(dword *)((((candidateArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
                      definitionOrSavedId).savedIdOrOffset + 0xdc);
      candidateScore = AiCombatTarget_EvaluateCandidateScore
                        (currentBestScore,1,0xffffffff,0xffffffff,candidateArmyRuntime,
                         sourceArmyRuntime);
      if (currentBestScore < candidateScore) {
        currentBestScore = candidateScore;
        worldNode2 = worldNode1;
      }
    }
    worldNode1 = worldNode1->nextNode;
  } while (worldNode1 != (WorldOwnerListNode100 *)0x0);
  if (worldNode2 != (WorldOwnerListNode100 *)0x0) {
    candidateWorldZ = worldNode2->worldZQ12;
    y = worldNode2->worldXQ12 - worldXQ12;
    x = worldNode2->worldYQ12 - worldYQ12;
    radiusOrCount = *(int *)(*(int *)worldNode2->runtimePayload + 0xdc);
    candidateDistance = FixedMath_Length2(x,y);
    if ((int)(candidateDistance - radiusOrCount) < 0x1001) {
      pointOffsetMask = 0;
      worldXQ12 = y + worldXQ12;
      worldYQ12 = x + worldYQ12;
      worldZQ12 = candidateWorldZ;
    }
  }
  ModelNodeRuntime_RebuildTransformsFromRoot(sourceRuntime);
  radiusOrCount = *(int *)((int)modelPointTable + 0x30);
  localPointRecord = (ModelPackedPointRecord *)(radiusOrCount + *(int *)(radiusOrCount + 0xe4));
  for (radiusOrCount = *(int *)(radiusOrCount + 0xe8); radiusOrCount != 0; radiusOrCount = radiusOrCount + -1) {
    if (localPointRecord->packedLookupKey == (modelPointOrdinal << 4 | 2)) {
      localPoint = ModelNodeRuntime_TransformLocalPointRegs(localPointRecord,sourceRuntime);
      ShotRuntimePool_CreateProjectileFromDefinition
                (effectFlags,*(ArmyRuntimeSlot **)(*(int *)((int)sourceRuntime + 0x48) + 8),
                 worldZQ12,(localPoint.ecx - *(int *)((int)sourceRuntime + 0x98) & pointOffsetMask) + worldYQ12
                 ,(localPoint.eax - *(int *)((int)sourceRuntime + 0x94) & pointOffsetMask) + worldXQ12,
                 localPoint.edx,localPoint.ecx,localPoint.eax,(ShotDefinition *)effectDefinitionId,worldContext);
    }
    localPointRecord = localPointRecord + 1;
  }
  return;
}


/* Address: 0x00529980.
   Ownership: gameplay/army/runtime.
   Purpose: Resets the shared attachment progress field, processes up to eight ready attachment channels by
   matching typed model anchors and emitting configured runtime effects, then clears the verified child timer and
   state fields. Two stack arguments are authoritative from RET 0x08; prior register parameters and return were
   synthetic. Role: Processes up to eight ready channels and emits configured effects from packed attachment keys.
   Inputs: Army channel timers, model attachment table and per-channel effect definitions. Outputs: EffectRuntime
   instances plus cleared channel/child state.
   Cross-module calls: ModelNodeRuntime_TransformLocalPointRegs [world/model/hierarchy],
   EffectRuntimePool_CreateInstanceFromDefinitionCf [world/effects/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_ProcessReadyAttachmentChannels
          (WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  ModelRuntimeSlot *childModelRuntime;
  uint channelIndex;
  Q12 worldXQ12;
  Q12 childWorldXQ12;
  dword remainingAttachments;
  int recordCountOrTable;
  int tableOrRecordCount;
  ModelPackedPointRecord *pointRecord;
  ModelLocalPointRegs12 localPoint;
  ModelRuntimeNode *modelNode1;
  
  channelIndex = 0;
  tableOrRecordCount = *(int *)(*(int *)((modelRuntime->definitionOrSavedId).savedIdOrOffset + 100) + 0x30);
  modelRuntime->definitionValue60_3C = 0;
  do {
    if (*(int *)(modelRuntime->reserved10_37 + channelIndex * 4 + 0x30) == 0) {
      recordCountOrTable = *(int *)(tableOrRecordCount + 0xe8);
      pointRecord = (ModelPackedPointRecord *)(tableOrRecordCount + *(int *)(tableOrRecordCount + 0xe4));
      if (recordCountOrTable != 0) {
        do {
          if (channelIndex * 0x10 + 3 == pointRecord->packedLookupKey) {
            localPoint = ModelNodeRuntime_TransformLocalPointRegs
                              (pointRecord,(modelRuntime->rootModelNodeOrSavedOffset).modelNode);
            worldXQ12 = localPoint.ecx;
            modelNode1 = (modelRuntime->rootModelNodeOrSavedOffset).modelNode;
            EffectRuntimePool_CreateInstanceFromDefinitionCf
                      (EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY,
                       *(EffectRuntimeOwnerReference4 *)
                        &modelRuntime->linkedModelRuntimeOrSavedOffset,
                       (modelNode1->modelPayload).worldRotationAngle2,
                       (modelNode1->modelPayload).worldRotationAngle1,
                       (modelNode1->modelPayload).worldRotationAngle0,localPoint.edx,worldXQ12,localPoint.eax,
                       *(EffectDefinition **)
                        ((modelRuntime->definitionOrSavedId).savedIdOrOffset + 0x80 + channelIndex * 8),
                       worldRuntime);
          }
          pointRecord = pointRecord + 1;
          recordCountOrTable = recordCountOrTable + -1;
        } while (recordCountOrTable != 0);
      }
    }
    channelIndex = channelIndex + 1;
  } while (channelIndex < 8);
  tableOrRecordCount = *(int *)((modelRuntime->definitionOrSavedId).savedIdOrOffset + 100);
  channelIndex = 0;
  if ((*(int *)(tableOrRecordCount + 0x14) != 0) &&
     (tableOrRecordCount = *(int *)(tableOrRecordCount + 0x18), recordCountOrTable = *(int *)(tableOrRecordCount + 0x30),
     (*(uint *)(tableOrRecordCount + 4) & 0xf) == 0)) {
    do {
      if (*(int *)(modelRuntime->reserved10_37 + channelIndex * 4 + 0x30) == 0) {
        tableOrRecordCount = *(int *)(recordCountOrTable + 0xe8);
        pointRecord = (ModelPackedPointRecord *)(recordCountOrTable + *(int *)(recordCountOrTable + 0xe4));
        if (tableOrRecordCount != 0) {
          do {
            if (((channelIndex * 0x10 + 3 == pointRecord->packedLookupKey) &&
                (((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->childCount != 0)) &&
               (modelNode1 = ((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->childNodes[0],
               modelNode1 != (ModelRuntimeNode *)0x0)) {
              localPoint = ModelNodeRuntime_TransformLocalPointRegs(pointRecord,modelNode1);
              childWorldXQ12 = localPoint.ecx;
              EffectRuntimePool_CreateInstanceFromDefinitionCf
                        (EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY,
                         *(EffectRuntimeOwnerReference4 *)
                          &modelRuntime->linkedModelRuntimeOrSavedOffset,0,0x4000,0,localPoint.edx,
                         childWorldXQ12,localPoint.eax,
                         *(EffectDefinition **)
                          ((modelRuntime->definitionOrSavedId).savedIdOrOffset + 0x80 + channelIndex * 8),
                         worldRuntime);
            }
            pointRecord = pointRecord + 1;
            tableOrRecordCount = tableOrRecordCount + -1;
          } while (tableOrRecordCount != 0);
        }
      }
      channelIndex = channelIndex + 1;
    } while (channelIndex < 8);
  }
  for (remainingAttachments = modelRuntime->attachmentCount0C; remainingAttachments != 0; remainingAttachments = remainingAttachments - 1) {
    childModelRuntime = modelRuntime->attachments140[0].childModelRuntimeOrSavedOffset00;
    if (childModelRuntime != (ModelRuntimeSlot *)0x0) {
      childModelRuntime->definitionValue60_3C = 0;
      (childModelRuntime->linkedModelRuntimeOrSavedOffset).modelRuntime = (ModelRuntimeSlot *)0x0;
    }
    modelRuntime = (ModelRuntimeSlot *)(modelRuntime->reserved10_37 + 0x10);
  }
  return;
}


/* Address: 0x0052A760.
   Ownership: gameplay/army/runtime.
   Purpose: Invokes the class-indexed method-D callback for the current runtime and recursively dispatches the same
   callback through its child hierarchy. Class method-D partition slots 24-47 receive (worldRuntime, armyRuntime).
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeHierarchy_DispatchClassMethodDRecursive
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  int remainingChildren;
  
  remainingChildren = armyRuntime->factionIndex;
  (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classMethodD
    [((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C])
            (worldRuntime,armyRuntime);
  for (; remainingChildren != 0; remainingChildren = remainingChildren + -1) {
    if ((ArmyRuntimeSlot *)armyRuntime[1].commandCoordinate0Q12 != (ArmyRuntimeSlot *)0x0) {
      ArmyRuntimeHierarchy_DispatchClassMethodDRecursive
                (worldRuntime,(ArmyRuntimeSlot *)armyRuntime[1].commandCoordinate0Q12);
    }
    armyRuntime = (ArmyRuntimeSlot *)&armyRuntime->commandCoordinate0Q12;
  }
  return;
}


/* Address: 0x0051C350.
   Ownership: gameplay/army/runtime.
   Purpose: Clears the verified derived selection and hierarchy metric fields, then rebuilds them recursively from
   the attached model runtime hierarchy when present.
   Cross-module calls: ModelRuntimeHierarchy_AccumulateDerivedSelectionMetrics [world/model/hierarchy].
*/
void __thandor_void_preserve_eax_ecx
ArmyRuntime_RebuildDerivedSelectionMetrics(ArmyRuntimeSlot *armyRuntime)

{
  byte *metricBytes;
  int metricIndex;
  
  armyRuntime->runtimeState90 = 0;
  armyRuntime->runtimeState44 = 0;
  armyRuntime->runtimeState48 = 0;
  metricIndex = 7;
  armyRuntime->runtimeState4C = 0;
  do {
    metricBytes = armyRuntime->reservedF8_FF + metricIndex * 4 + 8;
    metricBytes[0] = 0;
    metricBytes[1] = 0;
    metricBytes[2] = 0;
    metricBytes[3] = 0;
    metricIndex = metricIndex + -1;
  } while (-1 < metricIndex);
  if ((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime != (ModelRuntimeSlot *)0x0) {
    ModelRuntimeHierarchy_AccumulateDerivedSelectionMetrics
              ((int *)(armyRuntime->modelRuntimeOrSavedOffset).modelRuntime);
  }
  return;
}


/* Address: 0x0051DBA0.
   Ownership: gameplay/army/runtime.
   Purpose: The current implementation preserves the input state, clears carry, and therefore permits every tested
   point.
*/
bool __thandor_cf_preserve_eax_ecx_edx
ArmyRuntime_TestWorldPointAllowedDefaultCf(dword allowedContext,dword worldYQ12,dword worldXQ12)

{
  return false;
}


/* Address: 0x0051B8F0.
   Ownership: gameplay/army/runtime.
   Purpose: Allocates a free 0x120-byte army runtime slot, resolves the requested asset and faction graphics,
   initializes the runtime state, creates and links the model hierarchy, and rebuilds transforms, bounds, and tint
   before returning status through carry. Role: Creates a world army/placeable instance from an asset record and
   its model graph. Inputs: World context, army definition, model selection/technology/faction state and world
   transform. Outputs: ArmyRuntimeSlot, ModelRuntimeSlot/Node hierarchy, world links, occupancy, tint and derived
   radius/depth. Edges: ModelRuntimePool_CreateInstanceByDefinitionIdCf -> linked-child recursion ->
   transform/occupancy initialization.
   Local calls: ArmyRuntime_InitializeTerrainOccupancyFlags, ArmyRuntime_RebuildDerivedSelectionMetrics.
   Cross-module calls: ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf [assets/model/definitions],
   ModelDefinitionHierarchy_UnlockSelectedLinkedTechnology [assets/model/definitions],
   ModelDefinition_SelectFactionUnlockedLinkedIdCf [assets/model/definitions],
   ModelRuntimePool_CreateInstanceByDefinitionIdCf [world/model/runtime],
   ModelNodeRuntime_InstantiateLinkedChildrenRecursiveCf [world/model/hierarchy],
   WorldRuntime_LinkNodeIntoOwnerListD8 [world/runtime/core].
*/
ArmyRuntimeCreateEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ArmyRuntime_CreateInstanceFromAssetCf
          (WorldObjectAllocationFlags creationFlags,AngleTurn32 orientationAngle,Q12 worldXQ12,
          Q12 worldYQ12,FactionRuntimeIndex factionIndex,PckArmyAssetIdCatalog armyAssetId,
          WorldRuntimeContext *worldRuntime)

{
  ArmyAssetRecordPrefix *armyAssetRecord;
  GraphicsTextureSet *textureSet;
  GraphicsPaletteAsset *paletteAsset;
  dword rootNodeOrClassValue;
  PckArmyAssetIdCatalog classValue88;
  GameEntityRuntime *linkedEntity;
  ModelRuntimeNode *modelNodeRuntime;
  PckModelDefinitionIdCatalog modelDefinitionId;
  ModelRuntimeNode *resultOrModelNode;
  uint armySlotsRemaining;
  int remainingOrDefinition;
  ArmyAssetRecordPrefix **registryCursor;
  ArmyRuntimeSlot *armyRuntime;
  bool childCreateFailed;
  ArmyRuntimeCreateEaxCf5 failureResult;
  ModelDefinitionLookupEaxCf5 definitionLookup;
  ModelNodeCreateEaxCf5 modelCreateResult;
  ArmyRuntimeCreateEaxCf5 successResult;
  dword slotScanContinueValue;
  ArmyAssetRuntimeSemanticView80 *definitionNode;
  
  armySlotsRemaining = 0x400;
  armyRuntime = g_ArmyRuntimeSlots;
  slotScanContinueValue = (dword)g_ArmyRuntimeSlots;
  while (resultOrModelNode = (ModelRuntimeNode *)0x14, slotScanContinueValue != 0) {
    if (armyRuntime->modelNodeRuntime == (ModelRuntimeNode *)0x0) {
      registryCursor = g_ArmyAssetRecordRegistry;
      remainingOrDefinition = 0x300;
      goto ArmyRuntime_CreateInstanceFromAsset_ScanAssetDefinitionRegistry;
    }
    armyRuntime = armyRuntime + 1;
    armySlotsRemaining = armySlotsRemaining - 1;
    slotScanContinueValue = armySlotsRemaining;
  }
  goto ArmyRuntime_CreateInstanceFromAsset_ReturnCreationFailure;
  while( true ) {
    registryCursor = registryCursor + 1;
    remainingOrDefinition = remainingOrDefinition + -1;
    if (remainingOrDefinition == 0) break;
ArmyRuntime_CreateInstanceFromAsset_ScanAssetDefinitionRegistry:
    armyAssetRecord = *registryCursor;
    if ((armyAssetRecord != (ArmyAssetRecordPrefix *)0x0) &&
       (armyAssetRecord->registryId == armyAssetId)) {
      armyRuntime->armyAssetId = armyAssetId;
      if (((creationFlags & 2) != 0) && (factionIndex == worldRuntime->activeFactionRuntimeIndex)) {
        definitionLookup = ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                          (factionIndex,armyAssetRecord->rootNodeOffsetOrPointer);
        definitionLookup.modelDefinition[0x24].byteSize = definitionLookup.modelDefinition[0x24].byteSize + 1;
      }
      if (7 < (uint)factionIndex) {
        factionIndex = 7;
      }
      armyRuntime->factionIndex = factionIndex;
      if ((creationFlags & 4) != 0) {
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
      (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime = (ModelRuntimeSlot *)0x0;
      armyRuntime->runtimeState8C = 0;
      modelDefinitionId = ModelDefinition_SelectFactionUnlockedLinkedIdCf(factionIndex,rootNodeOrClassValue);
      modelCreateResult = ModelRuntimePool_CreateInstanceByDefinitionIdCf
                         (paletteAsset,textureSet,armyRuntime,modelDefinitionId,worldRuntime);
      resultOrModelNode = modelCreateResult.modelNode;
      if (!modelCreateResult.carry) {
        modelNodeRuntime = (ModelRuntimeNode *)(resultOrModelNode->common).nextNode;
        (armyRuntime->modelRuntimeOrSavedOffset).savedIdOrOffset = (dword)resultOrModelNode;
        armyRuntime->modelNodeRuntime = modelNodeRuntime;
        (modelNodeRuntime->worldTransform).translation.x = worldYQ12;
        (modelNodeRuntime->worldTransform).translation.y = worldXQ12;
        (modelNodeRuntime->worldTransform).translation.z = 0;
        armyRuntime->runtimeStateA4 = 0;
        armyRuntime->fallbackWorldYQ12 = worldYQ12;
        armyRuntime->fallbackWorldXQ12 = worldXQ12;
        armyRuntime->movementTarget0Q12 = worldYQ12;
        armyRuntime->movementTarget1Q12 = worldXQ12;
        (modelNodeRuntime->modelPayload).worldRotationAngle0 = 0;
        (modelNodeRuntime->modelPayload).worldRotationAngle1 = 0x4000;
        (modelNodeRuntime->modelPayload).worldRotationAngle2 = orientationAngle;
        armyRuntime->commandTargetArmyRuntime = (ArmyRuntimeSlot *)0x0;
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
        childCreateFailed = ModelNodeRuntime_InstantiateLinkedChildrenRecursiveCf
                          (factionIndex,paletteAsset,textureSet,
                           (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime,rootNodeOrClassValue,worldRuntime)
        ;
        resultOrModelNode = (ModelRuntimeNode *)worldYQ12;
        if (!childCreateFailed) {
          WorldRuntime_LinkNodeIntoOwnerListD8((WorldOwnerListNode100 *)modelNodeRuntime);
          ModelNodeRuntime_RecomputeSubtreeBoundingRadius(modelNodeRuntime);
          remainingOrDefinition = *THANDOR_BITCAST(ModelRuntimeSlotReferenceOrSavedOffset4, int *, armyRuntime->modelRuntimeOrSavedOffset);
          (*g_ArmyPlacementContactKindDispatchTable.callbacks[*(int *)(remainingOrDefinition + 0x278)])
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
          UiModelControl_RefreshStateTint(modelNodeRuntime);
          ArmyRuntime_RebuildDerivedSelectionMetrics(armyRuntime);
          successResult.carry = false;
          successResult.eax = (dword)armyRuntime;
          return successResult;
        }
      }
      goto ArmyRuntime_CreateInstanceFromAsset_ReturnCreationFailure;
    }
  }
  (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,armyAssetId,g_PackageLastErrorPath)
  ;
  resultOrModelNode = (ModelRuntimeNode *)0x41;
ArmyRuntime_CreateInstanceFromAsset_ReturnCreationFailure:
  failureResult.carry = true;
  failureResult.eax = (dword)resultOrModelNode;
  return failureResult;
}


/* Address: 0x0051D0B0.
   Ownership: gameplay/army/runtime.
   Purpose: Classifies the army runtime position against terrain occupancy, stores the resulting masks, resolves
   class-state flags, and applies the verified model-definition flag override.
   Cross-module calls: TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint [world/terrain/occupancy],
   TerrainOccupancyMask_ResolveRuntimeClassFlags [world/terrain/occupancy].
*/
void ArmyRuntime_InitializeTerrainOccupancyFlags
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  uint occupancyRuntimeFlags;
  ulonglong neighborhoodClassificationPair;
  TerrainOccupancyResolvedMasksRegs12 resolvedMasks;
  ModelRuntimeNode *modelNode;
  void *definition;
  
  THANDOR_PART(dword, neighborhoodClassificationPair, 4) =
       TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                 (*(Q12 *)((((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
                           definitionOrSavedId).savedIdOrOffset + 0xdc),
                  (armyRuntime->modelNodeRuntime->worldTransform).translation.y,
                  (armyRuntime->modelNodeRuntime->worldTransform).translation.x,
                  worldRuntime->fieldGrid);
  armyRuntime->terrainOccupancyMask0 = THANDOR_PART(dword, neighborhoodClassificationPair, 4);
  modelNode = armyRuntime->modelNodeRuntime;
  resolvedMasks = TerrainOccupancyMask_ResolveRuntimeClassFlags
                    (modelNode->runtimeFlags,armyRuntime->terrainOccupancyMask1,
                     armyRuntime->terrainOccupancyMask0,
                     (char)worldRuntime->activeFactionRuntimeIndex);
  occupancyRuntimeFlags = resolvedMasks.runtimeFlags;
  modelNode->runtimeFlags = modelNode->runtimeFlags & 0xffffeff3;
  armyRuntime->terrainOccupancyMask0 = resolvedMasks.primaryOccupancyMask;
  armyRuntime->terrainOccupancyMask1 = resolvedMasks.secondaryOccupancyMask;
  definition = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  modelNode->runtimeFlags = modelNode->runtimeFlags | occupancyRuntimeFlags;
  if ((*(uint *)((int)definition + 0xec) & 0x200) != 0) {
    modelNode->runtimeFlags = modelNode->runtimeFlags | 0x1000;
  }
  return;
}


/* Address: 0x00527E70.
   Ownership: gameplay/army/runtime.
   Purpose: Updates the verified animated model subnodes, reverses the bounded oscillating channel at its limits,
   rebuilds transforms, and refreshes damage-threshold effects. Runtime-update partition slots 0-23 receive
   (worldRuntime, armyRuntime). PART anim rate source (viewer-research-backlog section 9): child[0]/[1] spin
   localRotationAngle2 += def[+0x10]/ def[+0x1C] x stepTicks; child[2] bob translation by def[+0x14] clamped
   [def+0x24, def+0x28]; clock = g_InGameSimulationStepTicks (not look phase). Replaces the viewer's VH spin/bob
   constants. Role: Advances two spinning child nodes and one bounded Z-bobbing child, then rebuilds transforms.
   Local calls: ArmyRuntime_RebuildDerivedSelectionMetrics.
   Cross-module calls: ModelNodeRuntime_RebuildTransformsFromRoot [world/model/hierarchy],
   ArmyRuntime_EmitDamageThresholdEffect [gameplay/army/combat].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_UpdateAnimatedModelSubnodes
          (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView200 *modelRuntime)

{
  dword *classStateField;
  ModelRuntimeNode *modelNodeRuntime;
  ModelDefinitionRuntimeSemanticView280 *animationDefinition;
  Q12 updatedChildTranslationZQ12;
  int bobStep;
  ModelRuntimeNode *animatedChildNode;
  int childTranslationZQ12;
  ModelRuntimeNode *modelNode1;
  
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
      if (animatedChildNode != (ModelRuntimeNode *)0x0) {
        (animatedChildNode->modelPayload).localRotationAngle2 =
             *(int *)(animationDefinition->reserved00C_023 + 4) * g_InGameSimulationStepTicks +
             (animatedChildNode->modelPayload).localRotationAngle2 & 0xffff;
        animatedChildNode->runtimeFlags = animatedChildNode->runtimeFlags | 1;
      }
      modelNode1 = modelNodeRuntime->childNodes[1];
      if (1 < modelNodeRuntime->childCount) {
        if (modelNode1 != (ModelRuntimeNode *)0x0) {
          (modelNode1->modelPayload).localRotationAngle2 =
               *(int *)(animationDefinition->reserved00C_023 + 0x10) * g_InGameSimulationStepTicks +
               (modelNode1->modelPayload).localRotationAngle2 & 0xffff;
          modelNode1->runtimeFlags = modelNode1->runtimeFlags | 1;
        }
        modelNode1 = modelNodeRuntime->childNodes[2];
        if ((2 < modelNodeRuntime->childCount) && (modelNode1 != (ModelRuntimeNode *)0x0)) {
          childTranslationZQ12 = (modelNode1->modelPayload).localTranslationZQ12;
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
          (modelNode1->modelPayload).localTranslationZQ12 = updatedChildTranslationZQ12;
          modelNode1->runtimeFlags = modelNode1->runtimeFlags | 1;
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
   Ownership: gameplay/army/runtime.
   Purpose: Advances the two verified emitter timers, creates scheduled shots, chooses model attachment points,
   selects land or water effects, and emits the resulting runtime effect instances. Runtime-update partition slots
   0-23 receive (worldRuntime, armyRuntime). Role: Advances serialized timers and emits scheduled shots/effects
   from model attachment points. Inputs: Army definition timed fields, model hierarchy, SPR attachment records,
   water/terrain context. Outputs: ShotRuntime and/or EffectRuntime instances at transformed attachment positions.
   Cross-module calls: FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed],
   ShotRuntimePool_CreateProjectileFromDefinition [world/shots/runtime], ModelLookupTable_ContainsPackedKeyCf
   [assets/model/definitions], ModelNodeRuntime_TransformLocalPointRegs [world/model/hierarchy],
   FieldGrid_GetNearestWaterDelta [world/terrain/grid], EffectRuntimePool_CreateInstanceFromDefinitionCf
   [world/effects/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_UpdateTimedShotAndEffectEmitters
          (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView200 *modelRuntime)

{
  dword *emitterTimer;
  ModelDefinitionRuntimeSemanticView280 *emitterDefinition;
  int modelResourceAddress;
  dword nodeOrWorldX;
  uint pointSelector;
  sdword waterDelta;
  dword randomValue;
  int remainingRecords;
  dword worldY;
  AngleTurn32 orientationAngle0;
  uint randomOrPointCount;
  uint *pointRecordCursor;
  dword worldZQ12;
  AngleTurn32 orientationAngle1;
  ModelRuntimeNode *modelNode1;
  EffectDefinition *effectDefinition1;
  bool timerOverflow;
  ModelLookupEntryEaxCf5 lookupEntry;
  FixedDirectionXyzRegs12 launchDirection;
  ModelLocalPointRegs12 localPoint;
  AngleTurn32 orientationAngle2;
  GraphicsWorldCoordinateQ12 launchWorldZQ12;
  GraphicsWorldCoordinateQ12 launchWorldYQ12;
  GraphicsWorldCoordinateQ12 launchWorldXQ12;
  ShotDefinition *shotDefinition;
  EffectDefinition *effectDefinition;
  WorldRuntimeContext *worldContext1;
  
  emitterDefinition = modelRuntime->modelDefinition;
  emitterTimer = &(modelRuntime->classState).enabledStateE4;
  timerOverflow = SBORROW4(*emitterTimer,g_InGameSimulationStepTicks);
  *emitterTimer = *emitterTimer - g_InGameSimulationStepTicks;
  if ((*emitterTimer == 0 || timerOverflow != (int)*emitterTimer < 0) &&
     ((emitterDefinition->shotDefinitionReference168).definition != (ShotDefinition *)0xffffffff)) {
    randomOrPointCount = 0;
    if (*(int *)(emitterDefinition->reserved16C_173 + 4) != 0) {
      nodeOrWorldX = (*g_RandomGeneratorState.next)();
      randomOrPointCount = nodeOrWorldX % *(uint *)(emitterDefinition->reserved16C_173 + 4);
    }
    shotDefinition = (emitterDefinition->shotDefinitionReference168).definition;
    modelNode1 = modelRuntime->rootModelNode;
    (modelRuntime->classState).enabledStateE4 = randomOrPointCount + *(int *)emitterDefinition->reserved16C_173;
    launchWorldXQ12 = (modelNode1->worldTransform).translation.x;
    launchWorldYQ12 = (modelNode1->worldTransform).translation.y;
    launchWorldZQ12 = (modelNode1->worldTransform).translation.z;
    worldContext1 = worldRuntime;
    launchDirection = FixedMath_DirectionFromAnglesScaledRegs
                       ((modelNode1->modelPayload).worldRotationAngle1,
                        (modelNode1->modelPayload).worldRotationAngle0,0x1000);
    ShotRuntimePool_CreateProjectileFromDefinition
              (0,modelRuntime->ownerArmyRuntime,launchDirection.edx + launchWorldZQ12,
               launchDirection.ecx + launchWorldYQ12,launchDirection.eax + launchWorldXQ12,launchWorldZQ12,
               launchWorldYQ12,launchWorldXQ12,shotDefinition,worldContext1);
  }
  emitterDefinition = modelRuntime->modelDefinition;
  emitterTimer = &(modelRuntime->classState).enabledStateE8;
  timerOverflow = SBORROW4(*emitterTimer,g_InGameSimulationStepTicks);
  *emitterTimer = *emitterTimer - g_InGameSimulationStepTicks;
  if ((*emitterTimer != 0 && timerOverflow == (int)*emitterTimer < 0) ||
     (((emitterDefinition->effectDefinitionReference174).definition == (EffectDefinition *)0x0 &&
      ((emitterDefinition->effectDefinitionReference58).definition == (EffectDefinition *)0x0))))
  goto ArmyRuntime_UpdateTimedShotAndEffectEmitters_UpdateDamageThresholdEffectAndReturn;
  randomOrPointCount = 0;
  if (*(int *)(emitterDefinition->reserved178_187 + 4) != 0) {
    nodeOrWorldX = (*g_RandomGeneratorState.next)();
    randomOrPointCount = nodeOrWorldX % *(uint *)(emitterDefinition->reserved178_187 + 4);
  }
  (modelRuntime->classState).enabledStateE8 = randomOrPointCount + *(int *)emitterDefinition->reserved178_187;
  nodeOrWorldX = emitterDefinition->serializedNodeOffsetOrPointer64;
  if (emitterDefinition->runtimeClassId4C == MODEL_RUNTIME_CLASS_21_AIRCRAFT) {
    nodeOrWorldX = *(dword *)(nodeOrWorldX + 0x18);
  }
  modelResourceAddress = *(int *)(nodeOrWorldX + 0x30);
  remainingRecords = *(int *)(modelResourceAddress + 0xe8);
  worldContext1 = worldRuntime;
  if (remainingRecords == 0) {
ArmyRuntime_UpdateTimedShotAndEffectEmitters_UseModelWorldPositionForEffectEmitter:
    modelNode1 = modelRuntime->rootModelNode;
    nodeOrWorldX = (modelNode1->worldTransform).translation.x;
    worldY = (modelNode1->worldTransform).translation.y;
    worldZQ12 = (modelNode1->worldTransform).translation.z;
  }
  else {
    randomOrPointCount = 0;
    pointRecordCursor = (uint *)(modelResourceAddress + *(int *)(modelResourceAddress + 0xe4));
    do {
      if (((*pointRecordCursor & 0xf) == 6) && (randomOrPointCount <= *pointRecordCursor >> 4)) {
        randomOrPointCount = (*pointRecordCursor >> 4) + 1;
      }
      remainingRecords = remainingRecords + -1;
      pointRecordCursor = pointRecordCursor + 4;
    } while (remainingRecords != 0);
    if (randomOrPointCount == 0)
    goto ArmyRuntime_UpdateTimedShotAndEffectEmitters_UseModelWorldPositionForEffectEmitter;
    pointSelector = (modelRuntime->classState).classStateE0;
    if ((emitterDefinition->runtimeValue68 & 1) == 0) {
      pointSelector = (*g_RandomGeneratorState.next)();
    }
    nodeOrWorldX = emitterDefinition->serializedNodeOffsetOrPointer64;
    if (emitterDefinition->runtimeClassId4C == MODEL_RUNTIME_CLASS_21_AIRCRAFT) {
      nodeOrWorldX = *(dword *)(nodeOrWorldX + 0x18);
    }
    lookupEntry = ModelLookupTable_ContainsPackedKeyCf
                       (pointSelector % randomOrPointCount,6,*(ModelResourceHitTestAndRenderView210 **)(nodeOrWorldX + 0x30));
    if (lookupEntry.carry)
    goto ArmyRuntime_UpdateTimedShotAndEffectEmitters_UseModelWorldPositionForEffectEmitter;
    modelNode1 = modelRuntime->rootModelNode;
    if (emitterDefinition->runtimeClassId4C == MODEL_RUNTIME_CLASS_21_AIRCRAFT) {
      modelNode1 = modelNode1->childNodes[0];
    }
    localPoint = ModelNodeRuntime_TransformLocalPointRegs(lookupEntry.entry,modelNode1);
    worldZQ12 = localPoint.edx;
    worldY = localPoint.ecx;
    nodeOrWorldX = localPoint.eax;
  }
  effectDefinition1 = (emitterDefinition->effectDefinitionReference174).definition;
  effectDefinition = effectDefinition1;
  waterDelta = FieldGrid_GetNearestWaterDelta(worldY,nodeOrWorldX,worldRuntime->fieldGrid);
  if (0 < waterDelta) {
    effectDefinition1 = (emitterDefinition->effectDefinitionReference58).definition;
    effectDefinition = effectDefinition1;
  }
  if ((effectDefinition1 == (EffectDefinition *)0x0) ||
     ((effectDefinition1->transitionPrefix).transitionKind !=
      EFFECT_TRANSITION_INTEGRATE_LINEAR_MOTION_AND_SHADING_POSITION)) {
    modelNode1 = modelRuntime->rootModelNode;
    orientationAngle2 = (modelNode1->modelPayload).worldRotationAngle0;
    orientationAngle1 = (modelNode1->modelPayload).worldRotationAngle1;
    orientationAngle0 = (modelNode1->modelPayload).worldRotationAngle2;
  }
  else {
    randomValue = (*g_RandomGeneratorState.next)();
    orientationAngle0 = randomValue & 0xffff;
    orientationAngle1 = 0x4000 - (randomValue >> 0x14);
    orientationAngle2 = orientationAngle0;
  }
  EffectRuntimePool_CreateInstanceFromDefinitionCf
            (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference4, 0x0),orientationAngle0,
             orientationAngle1,orientationAngle2,worldZQ12,worldY,nodeOrWorldX,effectDefinition,
             worldContext1);
  emitterTimer = &(modelRuntime->classState).classStateE0;
  *emitterTimer = *emitterTimer + 1;
ArmyRuntime_UpdateTimedShotAndEffectEmitters_UpdateDamageThresholdEffectAndReturn:
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  return;
}

