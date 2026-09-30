/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/effects/maintenance.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/effects/maintenance.h>
#include <thandor/thandor.h>

/* Implementation ownership: world/effects/maintenance. */

/* PUNPCKLBW mm,mm then PSRLW mm,shift: the four bytes b of value as the words ((b << 8) | b) >> shift.
   With shift 4 each colour channel becomes a Q12 factor (0xFF -> 0x0FFF) for the PMULHW tint modulation. */
static __inline uint64_t EffectTint_UnpackBytesShiftRight(uint32_t value,int shift)

{
  ThandorMmx lanes;
  int lane;

  for (lane = 0; lane < 4; lane++) {
    lanes.uw[lane] = (uint16_t)(((value >> (lane * 8) & 0xff) * COLOR_CHANNEL_TO_WORD_LANE) >> shift);
  }
  return lanes.q;
}

/* PACKUSWB mm,mm (low dword): the four signed words saturated to unsigned bytes. */
static __inline uint32_t EffectTint_PackWordsUnsignedSaturate(uint64_t words)

{
  ThandorMmx lanes;
  uint32_t packed;
  int lane;

  lanes.q = words;
  packed = 0;
  for (lane = 0; lane < 4; lane++) {
    packed = packed |
             (uint32_t)(lanes.sw[lane] < 0 ? 0 : (0xff < lanes.sw[lane] ? 0xff : lanes.sw[lane])) << (lane * 8);
  }
  return packed;
}

/* Address: 0x0051E790.
   Effect entry of the terrainStateRefresh phase of g_RuntimeMaintenanceCallbackPhases (only reached through that
   table, from InGameRuntime_UpdateSimulationAndNetworkTick). Classifies the terrain occupancy around the effect,
   replaces the node's PRESENT/SEEN_BEFORE visibility flags with the resolved ones and refreshes the state tint;
   while the resulting tint is not fully transparent it becomes the effect tint modulated by the definition tint.
*/
void EffectRuntimeMaintenance_RefreshOccupancyFlagsAndTint
          (WorldRuntimeContext *worldRuntime,EffectModelRuntimeNode *modelNode)

{
  PackedArgb32 effectTintArgb;
  PackedArgb32 definitionTintArgb;
  uint32_t primaryOccupancyMask;
  uint64_t modulatedLanes;
  TerrainOccupancyResolvedMasks resolvedMasks;
  EffectRuntimeSlot *effectRuntime;

  primaryOccupancyMask =
       TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                 (Q12_ONE,modelNode->worldTransform.translation.y,
                  modelNode->worldTransform.translation.x,worldRuntime->fieldGrid);
  modelNode->runtimeFlags =
       modelNode->runtimeFlags & ~(TERRAIN_OCCUPANCY_FLAG_PRESENT | TERRAIN_OCCUPANCY_FLAG_SEEN_BEFORE);
  effectRuntime = modelNode->effectRuntime;
  resolvedMasks = TerrainOccupancyMask_ResolveRuntimeClassFlags
                     (modelNode->runtimeFlags,0,primaryOccupancyMask,
                      (char)worldRuntime->activeFactionRuntimeIndex);
  modelNode->runtimeFlags = modelNode->runtimeFlags | resolvedMasks.runtimeFlags;
  effectRuntime->terrainRuntimeClassState = resolvedMasks.primaryOccupancyMask;
  ModelNodeRuntime_RefreshStateTint((ModelRuntimeNode *)modelNode);
  if ((modelNode->tintArgb & 0xff000000) != 0) {
    effectTintArgb = effectRuntime->stateTintArgb;
    definitionTintArgb = effectRuntime->definitionOrSavedId.definition->stateTintArgb;
    /* PUNPCKLBW/PSRLW 4 both tints, PMULHW, PACKUSWB */
    modulatedLanes =
         pmulhw(EffectTint_UnpackBytesShiftRight(effectTintArgb,4),
                EffectTint_UnpackBytesShiftRight(definitionTintArgb,4));
    modelNode->tintArgb = EffectTint_PackWordsUnsignedSaturate(modulatedLanes);
  }
  return;
}


/* Address: 0x0051E830.
   Effect entry of the occupancyRebuild phase of g_RuntimeMaintenanceCallbackPhases (only reached through that
   table, from InGameRuntime_UpdateSimulationAndNetworkTick): effects take no part in the occupancy rebuild, so this
   does nothing (RET 8).
*/
void EffectRuntimeMaintenance_OccupancyRebuildNoOp(WorldRuntimeContext *worldRuntime,void *runtimeObject)

{
  return;
}


/* Address: 0x0051E840.
   Effect entry of the audioRefresh phase of g_RuntimeMaintenanceCallbackPhases (only reached through that table,
   from the every-8th-frame spatial sound pass in InGameUiRoot_UpdateFrame): effects add no
   spatial sound, so this does nothing (RET 8).
*/
void EffectRuntimeMaintenance_AudioRefreshNoOp(WorldRuntimeContext *worldRuntime,void *runtimeObject)

{
  return;
}


/* Address: 0x0051E850.
   Effect entry of the primaryUpdate phase of g_RuntimeMaintenanceCallbackPhases (only reached through that table,
   from InGameRuntime_UpdateSimulationAndNetworkTick). Runs once per simulation step of the batch: advances the
   animation frames (the effect ends and unlinks itself when they run out), starts and stops its shading record,
   fades the alpha in and out, interpolates the model scale, spawns periodic child effects and then performs the
   definition's transition kind (linked effect, shots and completion action, linear motion, or terrain-relative
   motion that ends on ground contact).
*/
void EffectModelRuntimeMaintenance_UpdateLifecycleTintScaleAndTransitions
          (WorldRuntimeContext *worldRuntime,EffectModelRuntimeNode *modelNode)

{
  GraphicsFixedVec3 *translationPtr;
  EffectRuntimeSlot *effectSlot;
  EffectAlphaFadeTicks fadeDurationTicks;
  EffectAlphaFadeTicks fadeOutDivisorTicks;
  PackedArgb32 effectOrDefinitionTintArgb;
  PackedArgb32 definitionTintArgb;
  EffectRuntimeCompletionAction pendingCompletionAction;
  int *ownerClassRecord;
  uint32_t keyIndex;
  AngleTurn32 previousRotationAngle1;
  uint32_t frameAgeOrTintValue;
  int32_t normalDotMotion;
  uint32_t fadeOutStartTicks;
  int frameAdvancedOrScratch;
  GameEntityRuntime *spawnArmyCompletionEntity;
  ShotTerrainImpactDeformationColumns *linkedHandlerCompletionOwner;
  void *completionOwnerCarrier;
  uint64_t modulatedLanes;
  ModelPackedPointRecord *packedPoint;
  ShadingRecordResult shadingAllocation;
  ArmyRuntimeSlot *createdArmy;
  HeightNormalSampleResult terrainSample;
  ModelWorldPoint localPoint;
  FixedDirection scaledDirection;
  GraphicsFixedVec3 terrainNormalDirection;
  GraphicsFixedVec3 motionDirection;
  EffectDefinition *periodicDefinition;
  InGameSimulationStepBatchTicks remainingStepTicks;
  EffectDefinition *effectDefinition;
  ModelRuntimeNode *ownerModelNode;
  GraphicsShadingRuntimeRecord *activeShadingRecord;
  
  remainingStepTicks = g_InGameSimulationStepTicks;
  do {
    effectSlot = modelNode->effectRuntime;
    effectDefinition = effectSlot->definitionOrSavedId.definition;
    /* one step adds 1.0 (Q4) to the frame accumulator; a frame advances when it reaches the definition's
       threshold */
    frameAgeOrTintValue = effectSlot->lifecycleOwnerAndDefinition.animationFrameAccumulatorQ4 + (1 << Q4_SHIFT);
    frameAdvancedOrScratch = 0;
    effectSlot->lifecycleOwnerAndDefinition.animationFrameAccumulatorQ4 = frameAgeOrTintValue;
    if (effectDefinition->frameAdvanceThresholdQ4 <= frameAgeOrTintValue) {
      effectSlot->lifecycleOwnerAndDefinition.animationFrameAccumulatorQ4 =
           frameAgeOrTintValue - effectDefinition->frameAdvanceThresholdQ4;
      frameAdvancedOrScratch = 1;
      effectSlot->animationFramesRemaining--;
      if (effectSlot->animationFramesRemaining == 0) {
        InterpolationState_SetNegatedTargetAndRescaleProgress
                  (effectDefinition->shadingReleaseTransitionDurationTicks,modelNode->shadingRecord
                  );
        WorldRuntime_UnlinkOwnerListNode((WorldOwnerListNode *)modelNode);
        effectSlot->modelNodeOrSavedOffset.modelNode = NULL;
        return;
      }
    }
    if (frameAdvancedOrScratch != 0) {
      modelNode->textureSubresourceBaseIndex++;
      effectSlot->shadingStartCountdownTicksRemaining--;
      if ((effectSlot->shadingStartCountdownTicksRemaining == 0) && (modelNode->shadingRecord == NULL)) {
        if (ModelLookupTable_FindPackedPoint
              (0,MODEL_POINT_CLASS_LIGHT,effectDefinition->ownedNestedResource,&packedPoint)) {
          localPoint = ModelNodeRuntime_TransformLocalPointRegs
                             (packedPoint,(ModelRuntimeNode *)modelNode);
          /* the alpha byte of the shading colour is the radius in 1/16 world units */
          shadingAllocation = GraphicsShadingRuntime_AllocateRecordRegs
                             (effectDefinition->shadingTransitionDurationTicks,
                              (effectDefinition->shadingColorArgb >> 24) << 8,
                              effectDefinition->shadingColorArgb,localPoint.zQ12,localPoint.yQ12,localPoint.xQ12);
          modelNode->shadingRecord = shadingAllocation.record;
        }
      }
      effectSlot->shadingStopCountdownTicksRemaining--;
      if ((effectSlot->shadingStopCountdownTicksRemaining == 0) && (modelNode->shadingRecord != NULL)) {
        InterpolationState_SetNegatedTargetAndRescaleProgress
                  (effectDefinition->shadingReleaseTransitionDurationTicks,modelNode->shadingRecord
                  );
        modelNode->shadingRecord = NULL;
      }
    }
    if (effectDefinition->transitionPrefix.transitionKind !=
        EFFECT_TRANSITION_ADVANCE_TERRAIN_RELATIVE_MOTION_AND_TERMINATE_ON_CONTACT) {
      frameAgeOrTintValue = effectSlot->effectAgeTicks;
      fadeOutStartTicks = (effectDefinition->animationFrameCount * effectDefinition->frameAdvanceThresholdQ4
               >> 4) - effectDefinition->alphaFadeOutTicks;
      /* alpha ramps 0 -> 255 over the fade-in and 255 -> 0 over the fade-out at the end of the animation */
      if (frameAgeOrTintValue < effectDefinition->alphaFadeInTicks) {
        fadeDurationTicks = effectDefinition->alphaFadeInTicks;
        effectSlot->stateTintArgb = effectSlot->stateTintArgb & 0xffffff;
        effectSlot->stateTintArgb =
             effectSlot->stateTintArgb |
             (int)(((int64_t)(int)frameAgeOrTintValue * 255) / (int64_t)(int)fadeDurationTicks) << 24;
      }
      else if ((fadeOutStartTicks < frameAgeOrTintValue) && (0 < (int)effectDefinition->alphaFadeOutTicks)) {
        fadeDurationTicks = effectDefinition->alphaFadeOutTicks;
        fadeOutDivisorTicks = effectDefinition->alphaFadeOutTicks;
        effectSlot->stateTintArgb = effectSlot->stateTintArgb & 0xffffff;
        effectSlot->stateTintArgb =
             effectSlot->stateTintArgb |
             (int)(((int64_t)(int)((frameAgeOrTintValue - fadeOutStartTicks) - fadeDurationTicks) * -255) /
                   (int64_t)(int)fadeOutDivisorTicks) << 24;
      }
      else {
        effectSlot->stateTintArgb = effectSlot->stateTintArgb | 0xff000000;
      }
      /* not visible to the local faction: fully transparent */
      if ((modelNode->runtimeFlags & TERRAIN_OCCUPANCY_FLAG_PRESENT) == 0) {
        modelNode->tintArgb = 0xffffff;
      }
      else {
        effectOrDefinitionTintArgb = effectSlot->stateTintArgb;
        definitionTintArgb = effectDefinition->stateTintArgb;
        /* PUNPCKLBW/PSRLW 4 both tints, PMULHW, PACKUSWB */
        modulatedLanes = pmulhw(EffectTint_UnpackBytesShiftRight(effectOrDefinitionTintArgb,4),
                                EffectTint_UnpackBytesShiftRight(definitionTintArgb,4));
        modelNode->tintArgb = EffectTint_PackWordsUnsignedSaturate(modulatedLanes);
      }
    }
    /* the scale runs linearly from start to end over the whole animation */
    if ((modelNode->runtimeFlags & MODEL_RUNTIME_FLAG_APPLY_SCALE) != 0) {
      modelNode->modelScaleQ12 =
           (int)(((int64_t)
                  (effectDefinition->modelScaleEndQ12 - effectDefinition->modelScaleStartQ12) *
                 (int64_t)(int)effectSlot->effectAgeTicks) /
                (int64_t)
                (int)(effectDefinition->animationFrameCount *
                      effectDefinition->frameAdvanceThresholdQ4 >> 4)) +
           effectDefinition->modelScaleStartQ12;
    }
    effectSlot->effectAgeTicks++;
    effectSlot->periodicEffectCountdownTicks--;
    if (effectSlot->periodicEffectCountdownTicks == 0) {
      effectSlot->periodicEffectCountdownTicks = effectDefinition->periodicEffectIntervalTicks;
      if (ModelLookupTable_FindPackedPoint
            (1,MODEL_POINT_CLASS_EFFECT,effectDefinition->ownedNestedResource,&packedPoint)) {
        periodicDefinition = effectDefinition->periodicEffectDefinition;
        localPoint = ModelNodeRuntime_TransformLocalPointRegs
                           (packedPoint,(ModelRuntimeNode *)modelNode);
        /* the periodic child effect always starts with rotation angle 1 at a quarter turn */
        EffectRuntimePool_CreateInstanceFromDefinition
                  (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference, 0),0,
                   FIXED_ANGLE16_QUARTER_TURN,0,
                   localPoint.zQ12,localPoint.yQ12,localPoint.xQ12,periodicDefinition,worldRuntime);
      }
    }
    switch(effectDefinition->transitionPrefix.transitionKind) {
    case EFFECT_TRANSITION_SPAWN_LINKED_EFFECT_AFTER_COUNTDOWN:
      if (frameAdvancedOrScratch != 0) {
        effectSlot->linkedEffectPresent--;
        if (effectSlot->linkedEffectPresent == 0) {
          if (ModelLookupTable_FindPackedPoint
                (0,MODEL_POINT_CLASS_EFFECT,effectDefinition->ownedNestedResource,&packedPoint)) {
            localPoint = ModelNodeRuntime_TransformLocalPointRegs
                               (packedPoint,(ModelRuntimeNode *)modelNode);
            EffectRuntimePool_CreateInstanceFromDefinition
                      (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference, 0),
                       modelNode->modelPayload.worldRotationAngle2,
                       modelNode->modelPayload.worldRotationAngle1,
                       modelNode->modelPayload.worldRotationAngle0,localPoint.zQ12,localPoint.yQ12,
                       localPoint.xQ12,effectDefinition->linkedEffectDefinition,worldRuntime);
          }
        }
      }
      break;
    case EFFECT_TRANSITION_ADVANCE_PERIODIC_EMISSION_AND_COMPLETION_ACTION:
      if (frameAdvancedOrScratch != 0) {
        effectSlot->linkedShotPresent--;
        if (effectSlot->linkedShotPresent == 0) {
          keyIndex = effectSlot->lifecycleOwnerAndDefinition.nextShotPointIndex;
          effectSlot->linkedShotPresent = effectDefinition->linkedShotPresent;
          effectSlot->lifecycleOwnerAndDefinition.nextShotPointIndex =
               effectSlot->lifecycleOwnerAndDefinition.nextShotPointIndex + 1;
          if (ModelLookupTable_FindPackedPoint
                (keyIndex,MODEL_POINT_CLASS_SHOT,effectDefinition->ownedNestedResource,&packedPoint)) {
            localPoint = ModelNodeRuntime_TransformLocalPointRegs
                               (packedPoint,(ModelRuntimeNode *)modelNode);
            ShotRuntimePool_CreateProjectileFromDefinition
                      (0,NULL,
                       (localPoint.zQ12 - modelNode->worldTransform.translation.z) * 2 +
                       modelNode->worldTransform.translation.z,
                       (localPoint.yQ12 - modelNode->worldTransform.translation.y) * 2 +
                       modelNode->worldTransform.translation.y,
                       (localPoint.xQ12 - modelNode->worldTransform.translation.x) * 2 +
                       modelNode->worldTransform.translation.x,localPoint.zQ12,localPoint.yQ12,localPoint.xQ12,
                       effectDefinition->linkedShotDefinition,worldRuntime);
          }
        }
        effectSlot->linkedEffectPresent--;
        if (effectSlot->linkedEffectPresent == 0) {
          if (ModelLookupTable_FindPackedPoint
                (0,MODEL_POINT_CLASS_EFFECT,effectDefinition->ownedNestedResource,&packedPoint)) {
            localPoint = ModelNodeRuntime_TransformLocalPointRegs
                               (packedPoint,(ModelRuntimeNode *)modelNode);
            EffectRuntimePool_CreateInstanceFromDefinition
                      (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference, 0),
                       modelNode->modelPayload.worldRotationAngle2,
                       modelNode->modelPayload.worldRotationAngle1,
                       modelNode->modelPayload.worldRotationAngle0,localPoint.zQ12,localPoint.yQ12,
                       localPoint.xQ12,effectDefinition->linkedEffectDefinition,worldRuntime);
          }
        }
        effectSlot->lifecycleOwnerAndDefinition.ownerAndDefinition.completionCountdownTicks--;
        if (effectSlot->lifecycleOwnerAndDefinition.ownerAndDefinition.completionCountdownTicks == 0) {
          pendingCompletionAction = effectSlot->completionAction;
          completionOwnerCarrier =
               effectSlot->lifecycleOwnerAndDefinition.ownerAndDefinition.owner.modelNode;
          if (pendingCompletionAction == EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY) {
DestroyOwnerModel:
            if (completionOwnerCarrier != NULL) {
              ModelRuntimePool_DestroyHierarchyAndDetach(worldRuntime,completionOwnerCarrier);
            }
          }
          else {
            spawnArmyCompletionEntity = completionOwnerCarrier;
            if (pendingCompletionAction == EFFECT_RUNTIME_COMPLETION_SPAWN_ARMY_FROM_MODEL) {
SpawnArmyFromOwner:
              if (spawnArmyCompletionEntity != NULL) {
                ownerClassRecord = spawnArmyCompletionEntity->common.ownership.definitionOrClassRecord;
                ownerModelNode = spawnArmyCompletionEntity->common.ownership.modelNode;
                /* frameAdvancedOrScratch now holds the owner's model definition: only class 18 turns into the
                   army asset named by classParameterC0. The new model keeps the owner's armour points (+0x3C) in
                   proportion, rescaled by the two definitions' maximumHealth (presumably the full armour). */
                frameAdvancedOrScratch = *ownerClassRecord;
                if (((ModelDefinition *)frameAdvancedOrScratch)->runtimeClassId == MODEL_RUNTIME_CLASS_18) {
                  createdArmy = ArmyRuntime_CreateInstanceFromAsset
                                     (ARMY_CREATE_COUNT_FOR_ACTIVE_FACTION | ARMY_CREATE_UNLOCK_TECHNOLOGY,
                                      ownerModelNode->modelPayload.worldRotationAngle2,
                                      ownerModelNode->worldTransform.translation.y,
                                      ownerModelNode->worldTransform.translation.x,
                                      spawnArmyCompletionEntity->common.ownership.ownerIndex,
                                      (PckArmyAssetIdCatalog)
                                      ((ModelDefinition *)frameAdvancedOrScratch)->classParameterC0,
                                      worldRuntime,NULL);
                  if (createdArmy != NULL) {
                    /* ownerClassRecord is the owner's ModelRuntimeSlot */
                    (*(ModelRuntimeSlot **)createdArmy)->health =
                         (int)(((int64_t)(int)((ModelRuntimeSlot *)ownerClassRecord)->health *
                               (int64_t)(int)(*(ModelRuntimeSlot **)createdArmy)->
                                              definitionOrSavedId.runtimeDefinition->maximumHealth) /
                              (int64_t)(int)((ModelDefinition *)frameAdvancedOrScratch)->
                                            maximumHealth);
                    ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,spawnArmyCompletionEntity);
                  }
                }
              }
            }
            else {
              linkedHandlerCompletionOwner = completionOwnerCarrier;
              if (pendingCompletionAction == EFFECT_RUNTIME_COMPLETION_INVOKE_LINKED_HANDLER) goto InvokeLinkedHandler;
            }
          }
        }
      }
      break;
    case EFFECT_TRANSITION_INTEGRATE_LINEAR_MOTION_AND_SHADING_POSITION:
      scaledDirection = FixedMath_DirectionFromAnglesScaledRegs
                         (modelNode->modelPayload.worldRotationAngle1,
                          modelNode->modelPayload.worldRotationAngle0,
                          effectDefinition->movementSpeedQ12);
      previousRotationAngle1 = modelNode->modelPayload.worldRotationAngle1;
      activeShadingRecord = modelNode->shadingRecord;
      translationPtr = &modelNode->worldTransform.translation;
      translationPtr->x = translationPtr->x + scaledDirection.x;
      modelNode->worldTransform.translation.y += scaledDirection.y;
      modelNode->worldTransform.translation.z += scaledDirection.z;
      ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)modelNode);
      /* each step moves rotation angle 1 a 64th of the way towards a quarter turn */
      modelNode->modelPayload.worldRotationAngle1 =
           (int)(previousRotationAngle1 * 63 + FIXED_ANGLE16_QUARTER_TURN) >> 6;
      if (activeShadingRecord != NULL) {
        activeShadingRecord->worldXQ12 = activeShadingRecord->worldXQ12 + scaledDirection.x;
        activeShadingRecord->worldYQ12 = activeShadingRecord->worldYQ12 + scaledDirection.y;
        activeShadingRecord->worldZQ12 = activeShadingRecord->worldZQ12 + scaledDirection.z;
      }
      break;
    case EFFECT_TRANSITION_ADVANCE_TERRAIN_RELATIVE_MOTION_AND_TERMINATE_ON_CONTACT:
      terrainSample = FieldGrid_InterpolateTerrainHeightAndNormal
                         (modelNode->worldTransform.translation.y,
                          modelNode->worldTransform.translation.x,worldRuntime->fieldGrid);
      if (!terrainSample.failed) {
        /* Both register-returned directions are spilled to the stack in the binary; Ghidra showed
           them as &stack0xffffffd4 / &stack0xffffffc8. The dot product is symmetric. Both are unit vectors
           (0x10000000 = 1.0 in Q28); the motion elevation drops with the square of the effect's age. */
        scaledDirection = FixedMath_DirectionFromAnglesScaledRegs
                  ((int)terrainSample.packedNormalAngles >> 16,terrainSample.packedNormalAngles & FIXED_ANGLE16_MASK,
                   Q28_ONE);
        terrainNormalDirection.x = scaledDirection.x;
        terrainNormalDirection.y = scaledDirection.y;
        terrainNormalDirection.z = scaledDirection.z;
        scaledDirection = FixedMath_DirectionFromAnglesScaledRegs
                  (modelNode->modelPayload.worldRotationAngle1 -
                   effectSlot->effectAgeTicks * effectSlot->effectAgeTicks * effectDefinition->pitchDropPerAgeSquared,
                   modelNode->modelPayload.worldRotationAngle0,Q28_ONE);
        motionDirection.x = scaledDirection.x;
        motionDirection.y = scaledDirection.y;
        motionDirection.z = scaledDirection.z;
        normalDotMotion = FixedVec3_DotQ28(&terrainNormalDirection,&motionDirection);
        if (normalDotMotion < 0) {
          /* moving into the ground: fade the alpha by groundContactAlphaFadeStep per step, end once it is already 0 */
          if (effectSlot->stateTintArgb < ARGB8888_ALPHA_ONE) {
            InterpolationState_SetNegatedTargetAndRescaleProgress
                      (effectDefinition->shadingReleaseTransitionDurationTicks,
                       modelNode->shadingRecord);
            WorldRuntime_UnlinkOwnerListNode((WorldOwnerListNode *)modelNode);
            effectSlot->modelNodeOrSavedOffset.modelNode = NULL;
            return;
          }
          frameAgeOrTintValue = effectSlot->stateTintArgb >> 24;
          frameAdvancedOrScratch = frameAgeOrTintValue - effectDefinition->groundContactAlphaFadeStep;
          if (frameAgeOrTintValue < effectDefinition->groundContactAlphaFadeStep) {
            frameAdvancedOrScratch = 0;
          }
          frameAgeOrTintValue = effectSlot->stateTintArgb & 0xffffff | frameAdvancedOrScratch << 24;
        }
        else {
          frameAgeOrTintValue = 0xffffffff;
          modelNode->modelPayload.worldRotationAngle1 -= effectSlot->effectAgeTicks * effectSlot->effectAgeTicks *
                              effectDefinition->pitchDropPerAgeSquared;
        }
        effectSlot->stateTintArgb = frameAgeOrTintValue;
        if ((modelNode->runtimeFlags & TERRAIN_OCCUPANCY_FLAG_PRESENT) != 0) {
          effectOrDefinitionTintArgb = effectDefinition->stateTintArgb;
          /* PUNPCKLBW/PSRLW 4 both tints, PMULHW, PACKUSWB */
          modulatedLanes = pmulhw(EffectTint_UnpackBytesShiftRight(frameAgeOrTintValue,4),
                                  EffectTint_UnpackBytesShiftRight(effectOrDefinitionTintArgb,4));
          modelNode->tintArgb = EffectTint_PackWordsUnsignedSaturate(modulatedLanes);
        }
        ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)modelNode);
        effectSlot->lifecycleOwnerAndDefinition.ownerAndDefinition.completionCountdownTicks--;
        if (effectSlot->lifecycleOwnerAndDefinition.ownerAndDefinition.completionCountdownTicks == 0) {
          pendingCompletionAction = effectSlot->completionAction;
          completionOwnerCarrier =
               effectSlot->lifecycleOwnerAndDefinition.ownerAndDefinition.owner.modelNode;
          if (pendingCompletionAction == EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY) goto DestroyOwnerModel;
          linkedHandlerCompletionOwner = completionOwnerCarrier;
          if (pendingCompletionAction == EFFECT_RUNTIME_COMPLETION_INVOKE_LINKED_HANDLER) {
InvokeLinkedHandler:
            if ((linkedHandlerCompletionOwner != NULL) &&
                (0 < (int)linkedHandlerCompletionOwner->radiusWorldUnits)) {
              FieldGrid_ApplyRadialTerrainHeightDeltaAndRefreshSurface
                        (linkedHandlerCompletionOwner->terrainMaterialIndex,
                         linkedHandlerCompletionOwner->radiusWorldUnits,
                         linkedHandlerCompletionOwner->heightDeltaQ12,
                         modelNode->worldTransform.translation.y,
                         modelNode->worldTransform.translation.x,worldRuntime->fieldGrid);
            }
          }
          else {
            spawnArmyCompletionEntity = completionOwnerCarrier;
            if (pendingCompletionAction == EFFECT_RUNTIME_COMPLETION_SPAWN_ARMY_FROM_MODEL) goto SpawnArmyFromOwner;
          }
        }
      }
    }
    remainingStepTicks--;
  } while (remainingStepTicks != 0);
}

