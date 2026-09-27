/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/effects/maintenance.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/effects/maintenance.h>
#include <thandor/thandor.h>

/* Implementation ownership: world/effects/maintenance. */

/* PUNPCKLBW mm,mm then PSRLW mm,shift: the four bytes b of value as the words ((b << 8) | b) >> shift. */
static __inline uint64_t EffectTint_UnpackBytesShiftRight(uint32_t value,int shift)

{
  ThandorMmx lanes;
  int lane;

  for (lane = 0; lane < 4; lane = lane + 1) {
    lanes.uw[lane] = (uint16_t)(((value >> (lane * 8) & 0xff) * 0x101) >> shift);
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
  for (lane = 0; lane < 4; lane = lane + 1) {
    packed = packed |
             (uint32_t)(lanes.sw[lane] < 0 ? 0 : (0xff < lanes.sw[lane] ? 0xff : lanes.sw[lane])) << (lane * 8);
  }
  return packed;
}

/* Address: 0x0051E790.
   Ownership: world/effects/maintenance.
   Purpose: Binary entry is anchored by g_ArmyRuntimeCallbackTable12[5]@00562DEC. Maintenance table phase
   terrainStateRefresh, object kind effect. The 4x3 table bytes, target body, calling convention, and RET 0x08
   contract remain unchanged.
   Cross-module calls: TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint [world/terrain/occupancy],
   TerrainOccupancyMask_ResolveRuntimeClassFlags [world/terrain/occupancy], UiModelControl_RefreshStateTint
   [ui/controls/misc].
*/
void __thandor_void_preserve_eax_ecx_edx
EffectRuntimeMaintenance_RefreshOccupancyFlagsAndTint
          (WorldRuntimeContext *worldRuntime,EffectModelRuntimeNodeClassView100 *modelNode)

{
  PackedArgb32 effectTintArgb;
  PackedArgb32 definitionTintArgb;
  uint32_t primaryOccupancyMask;
  uint64_t mm0PackedValue0;
  TerrainOccupancyResolvedMasksRegs12 resolvedMasks;
  EffectRuntimeSlot *effectRuntime;
  
  primaryOccupancyMask =
       TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                 (0x1000,(modelNode->worldTransform).translation.y,
                  (modelNode->worldTransform).translation.x,worldRuntime->fieldGrid);
  modelNode->runtimeFlags = modelNode->runtimeFlags & 0xfffffff3;
  effectRuntime = modelNode->effectRuntime;
  resolvedMasks = TerrainOccupancyMask_ResolveRuntimeClassFlags
                     (modelNode->runtimeFlags,0,primaryOccupancyMask,
                      (char)worldRuntime->activeFactionRuntimeIndex);
  modelNode->runtimeFlags = modelNode->runtimeFlags | resolvedMasks.runtimeFlags;
  effectRuntime->terrainRuntimeClassState = resolvedMasks.primaryOccupancyMask;
  UiModelControl_RefreshStateTint((ModelRuntimeNode *)modelNode);
  if ((modelNode->tintArgb & 0xff000000) != 0) {
    effectTintArgb = effectRuntime->stateTintArgb;
    definitionTintArgb = ((effectRuntime->definitionOrSavedId).definition)->stateTintArgb;
    /* PUNPCKLBW/PSRLW 4 both tints, PMULHW, PACKUSWB */
    mm0PackedValue0 =
         pmulhw(EffectTint_UnpackBytesShiftRight(effectTintArgb,4),
                EffectTint_UnpackBytesShiftRight(definitionTintArgb,4));
    modelNode->tintArgb = EffectTint_PackWordsUnsignedSaturate(mm0PackedValue0);
  }
  return;
}


/* Address: 0x0051E830.
   Ownership: world/effects/maintenance.
   Purpose: Exact two-argument no-op installed in the effect column of the unified model, shot, and effect method
   table. It returns with ret 0x08. Maintenance table phase occupancyRebuild, object kind effect. The 4x3 table
   bytes, target body, calling convention, and RET 0x08 contract remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
EffectRuntimeMaintenance_OccupancyRebuildNoOp(WorldRuntimeContext *worldRuntime,void *runtimeObject)

{
  return;
}


/* Address: 0x0051E840.
   Ownership: world/effects/maintenance.
   Purpose: Second exact two-argument no-op installed in the effect column of the unified model, shot, and effect
   method table. It returns with ret 0x08. Maintenance table phase audioRefresh, object kind effect. The 4x3 table
   bytes, target body, calling convention, and RET 0x08 contract remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
EffectRuntimeMaintenance_AudioRefreshNoOp(WorldRuntimeContext *worldRuntime,void *runtimeObject)

{
  return;
}


/* Address: 0x0051E850.
   Ownership: world/effects/maintenance.
   Purpose: Maintenance slot 2 receives WorldRuntimeContext and ModelRuntimeNode, advances the effect runtime
   payload lifecycle, animation/tint state, and effect transitions. The exact seven-range body, three RET 0x08
   terminals, and five-entry switch table are sealed against the immutable original binary. Conservative slot
   identity is retained until subtype fields are closed. Advances effect lifecycle timers and animation, updates
   shading ownership, tint and scale interpolation, and dispatches the sealed effect transition modes. Maintenance
   table phase primaryUpdate, object kind effect.
   Cross-module calls: InterpolationState_SetNegatedTargetAndRescaleProgress [core/math/interpolation],
   WorldRuntime_UnlinkNodeFromOwnerListD8 [world/runtime/core], ModelLookupTable_ContainsPackedKeyCf
   [assets/model/definitions], ModelNodeRuntime_TransformLocalPointRegs [world/model/hierarchy],
   GraphicsShadingRuntime_AllocateRecordRegs [graphics/render/shading],
   EffectRuntimePool_CreateInstanceFromDefinitionCf [world/effects/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
EffectModelRuntimeMaintenance_UpdateLifecycleTintScaleAndTransitions
          (WorldRuntimeContext *worldRuntime,EffectModelRuntimeNodeClassView100 *modelNode)

{
  EffectAnimationFrameCount *framesRemainingPtr;
  EffectShadingCountdownTicks *shadingCountdownPtr;
  EffectPeriodicIntervalTicks *periodicCountdownPtr;
  DefinitionReferencePresentFlag *linkedCountdownPtr;
  uint32_t *completionCountdownPtr;
  AngleTurn32 *rotationAngle1Ptr;
  GraphicsFixedVec3 *translationPtr;
  GraphicsWorldCoordinateQ12 *translationAxisPtr;
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
  Q12 worldXQ12;
  int frameAdvancedOrScratch;
  Q12 worldZQ12;
  GameEntityRuntime *spawnArmyCompletionEntity;
  EffectCompletionLinkedHandlerOwnerColumns104 *linkedHandlerCompletionOwner;
  void *completionOwnerCarrier;
  uint64_t modulatedLanes;
  ModelLookupEntryEaxCf5 lookupResult;
  GraphicsShadingRuntimeRecordEaxCf5 shadingAllocation;
  ArmyRuntimeCreateEaxCf5 armyCreateResult;
  FieldGridHeightNormalEaxEdxCf9 terrainSample;
  ModelLocalPointRegs12 localPoint;
  FixedDirectionXyzRegs12 scaledDirection;
  GraphicsFixedVec3 terrainNormalDirection;
  GraphicsFixedVec3 motionDirection;
  EffectDefinition *periodicDefinition;
  WorldRuntimeContext *spawnWorldRuntime;
  InGameSimulationStepBatchTicks remainingStepTicks;
  EffectDefinition *effectDefinition;
  ModelRuntimeNode *ownerModelNode;
  GraphicsShadingRuntimeRecord *activeShadingRecord;
  
  remainingStepTicks = g_InGameSimulationStepTicks;
  do {
    effectSlot = modelNode->effectRuntime;
    effectDefinition = (effectSlot->definitionOrSavedId).definition;
    frameAgeOrTintValue = (effectSlot->lifecycleOwnerAndDefinition).animationFrameAccumulatorQ4 + 0x10;
    frameAdvancedOrScratch = 0;
    (effectSlot->lifecycleOwnerAndDefinition).animationFrameAccumulatorQ4 = frameAgeOrTintValue;
    if (effectDefinition->frameAdvanceThresholdQ4 <= frameAgeOrTintValue) {
      (effectSlot->lifecycleOwnerAndDefinition).animationFrameAccumulatorQ4 =
           frameAgeOrTintValue - effectDefinition->frameAdvanceThresholdQ4;
      frameAdvancedOrScratch = 1;
      framesRemainingPtr = &effectSlot->animationFramesRemaining;
      *framesRemainingPtr = *framesRemainingPtr - 1;
      if (*framesRemainingPtr == 0) {
        InterpolationState_SetNegatedTargetAndRescaleProgress
                  (effectDefinition->shadingReleaseTransitionDurationTicks,modelNode->shadingRecord
                  );
        WorldRuntime_UnlinkNodeFromOwnerListD8((WorldOwnerListNode100 *)modelNode);
        (effectSlot->modelNodeOrSavedOffset).modelNode = (ModelRuntimeNode *)0x0;
        return;
      }
    }
    if (frameAdvancedOrScratch != 0) {
      modelNode->textureSubresourceBaseIndex = modelNode->textureSubresourceBaseIndex + 1;
      shadingCountdownPtr = &effectSlot->shadingStartCountdownTicksRemaining;
      *shadingCountdownPtr = *shadingCountdownPtr - 1;
      if ((*shadingCountdownPtr == 0) && (modelNode->shadingRecord == (GraphicsShadingRuntimeRecord *)0x0)) {
        lookupResult = ModelLookupTable_ContainsPackedKeyCf(0,4,effectDefinition->ownedNestedResource);
        if (!lookupResult.carry) {
          localPoint = ModelNodeRuntime_TransformLocalPointRegs
                             (lookupResult.entry,(ModelRuntimeNode *)modelNode);
          shadingAllocation = GraphicsShadingRuntime_AllocateRecordRegs
                             (effectDefinition->shadingTransitionDurationTicks,
                              (effectDefinition->shadingColorArgb >> 0x18) << 8,
                              effectDefinition->shadingColorArgb,localPoint.edx,localPoint.ecx,localPoint.eax);
          modelNode->shadingRecord = shadingAllocation.record;
        }
      }
      shadingCountdownPtr = &effectSlot->shadingStopCountdownTicksRemaining;
      *shadingCountdownPtr = *shadingCountdownPtr - 1;
      if ((*shadingCountdownPtr == 0) && (modelNode->shadingRecord != (GraphicsShadingRuntimeRecord *)0x0)) {
        InterpolationState_SetNegatedTargetAndRescaleProgress
                  (effectDefinition->shadingReleaseTransitionDurationTicks,modelNode->shadingRecord
                  );
        modelNode->shadingRecord = (GraphicsShadingRuntimeRecord *)0x0;
      }
    }
    if ((effectDefinition->transitionPrefix).transitionKind !=
        EFFECT_TRANSITION_ADVANCE_TERRAIN_RELATIVE_MOTION_AND_TERMINATE_ON_CONTACT) {
      frameAgeOrTintValue = effectSlot->effectAgeTicks;
      fadeOutStartTicks = (effectDefinition->animationFrameCount * effectDefinition->frameAdvanceThresholdQ4
               >> 4) - effectDefinition->alphaFadeOutTicks;
      if (frameAgeOrTintValue < effectDefinition->alphaFadeInTicks) {
        fadeDurationTicks = effectDefinition->alphaFadeInTicks;
        effectSlot->stateTintArgb = effectSlot->stateTintArgb & 0xffffff;
        effectSlot->stateTintArgb =
             effectSlot->stateTintArgb |
             (int)(((int64_t)(int)frameAgeOrTintValue * 0xff) / (int64_t)(int)fadeDurationTicks) << 0x18;
      }
      else if ((fadeOutStartTicks < frameAgeOrTintValue) && (0 < (int)effectDefinition->alphaFadeOutTicks)) {
        fadeDurationTicks = effectDefinition->alphaFadeOutTicks;
        fadeOutDivisorTicks = effectDefinition->alphaFadeOutTicks;
        effectSlot->stateTintArgb = effectSlot->stateTintArgb & 0xffffff;
        effectSlot->stateTintArgb =
             effectSlot->stateTintArgb |
             (int)(((int64_t)(int)((frameAgeOrTintValue - fadeOutStartTicks) - fadeDurationTicks) * -0xff) / (int64_t)(int)fadeOutDivisorTicks) <<
             0x18;
      }
      else {
        effectSlot->stateTintArgb = effectSlot->stateTintArgb | 0xff000000;
      }
      if ((modelNode->runtimeFlags & 4) == 0) {
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
    if ((modelNode->runtimeFlags & 0x800) != 0) {
      modelNode->modelScaleQ12 =
           (int)(((int64_t)
                  (effectDefinition->modelScaleEndQ12 - effectDefinition->modelScaleStartQ12) *
                 (int64_t)(int)effectSlot->effectAgeTicks) /
                (int64_t)
                (int)(effectDefinition->animationFrameCount *
                      effectDefinition->frameAdvanceThresholdQ4 >> 4)) +
           effectDefinition->modelScaleStartQ12;
    }
    effectSlot->effectAgeTicks = effectSlot->effectAgeTicks + 1;
    periodicCountdownPtr = &effectSlot->periodicEffectCountdownTicks;
    *periodicCountdownPtr = *periodicCountdownPtr - 1;
    if (*periodicCountdownPtr == 0) {
      effectSlot->periodicEffectCountdownTicks = effectDefinition->periodicEffectIntervalTicks;
      lookupResult = ModelLookupTable_ContainsPackedKeyCf(1,3,effectDefinition->ownedNestedResource);
      if (!lookupResult.carry) {
        periodicDefinition = effectDefinition->periodicEffectDefinition;
        spawnWorldRuntime = worldRuntime;
        localPoint = ModelNodeRuntime_TransformLocalPointRegs
                           (lookupResult.entry,(ModelRuntimeNode *)modelNode);
        worldZQ12 = localPoint.edx;
        worldXQ12 = localPoint.ecx;
        EffectRuntimePool_CreateInstanceFromDefinitionCf
                  (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference4, 0x0),0,0x4000,0,
                   worldZQ12,worldXQ12,localPoint.eax,periodicDefinition,spawnWorldRuntime);
      }
    }
                    // WARNING: Switch is manually overridden
    switch((effectDefinition->transitionPrefix).transitionKind) {
    case EFFECT_TRANSITION_SPAWN_LINKED_EFFECT_AFTER_COUNTDOWN:
      if (frameAdvancedOrScratch != 0) {
        linkedCountdownPtr = &effectSlot->linkedEffectPresent;
        *linkedCountdownPtr = *linkedCountdownPtr - 1;
        if (*linkedCountdownPtr == 0) {
          lookupResult = ModelLookupTable_ContainsPackedKeyCf(0,3,effectDefinition->ownedNestedResource);
          if (!lookupResult.carry) {
            localPoint = ModelNodeRuntime_TransformLocalPointRegs
                               (lookupResult.entry,(ModelRuntimeNode *)modelNode);
            EffectRuntimePool_CreateInstanceFromDefinitionCf
                      (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference4, 0x0),
                       (modelNode->modelPayload).worldRotationAngle2,
                       (modelNode->modelPayload).worldRotationAngle1,
                       (modelNode->modelPayload).worldRotationAngle0,localPoint.edx,localPoint.ecx,
                       localPoint.eax,effectDefinition->linkedEffectDefinition,worldRuntime);
          }
        }
      }
      break;
    case EFFECT_TRANSITION_ADVANCE_PERIODIC_EMISSION_AND_COMPLETION_ACTION:
      if (frameAdvancedOrScratch != 0) {
        linkedCountdownPtr = &effectSlot->linkedShotPresent;
        *linkedCountdownPtr = *linkedCountdownPtr - 1;
        if (*linkedCountdownPtr == 0) {
          keyIndex = (effectSlot->lifecycleOwnerAndDefinition).runtimeState14;
          effectSlot->linkedShotPresent = effectDefinition->linkedShotPresent;
          (effectSlot->lifecycleOwnerAndDefinition).runtimeState14 =
               (effectSlot->lifecycleOwnerAndDefinition).runtimeState14 + 1;
          lookupResult = ModelLookupTable_ContainsPackedKeyCf
                             (keyIndex,2,effectDefinition->ownedNestedResource);
          if (!lookupResult.carry) {
            localPoint = ModelNodeRuntime_TransformLocalPointRegs
                               (lookupResult.entry,(ModelRuntimeNode *)modelNode);
            ShotRuntimePool_CreateProjectileFromDefinition
                      (0,(ArmyRuntimeSlot *)0x0,
                       (localPoint.edx - (modelNode->worldTransform).translation.z) * 2 +
                       (modelNode->worldTransform).translation.z,
                       (localPoint.ecx - (modelNode->worldTransform).translation.y) * 2 +
                       (modelNode->worldTransform).translation.y,
                       (localPoint.eax - (modelNode->worldTransform).translation.x) * 2 +
                       (modelNode->worldTransform).translation.x,localPoint.edx,localPoint.ecx,localPoint.eax,
                       effectDefinition->linkedShotDefinition,worldRuntime);
          }
        }
        linkedCountdownPtr = &effectSlot->linkedEffectPresent;
        *linkedCountdownPtr = *linkedCountdownPtr - 1;
        if (*linkedCountdownPtr == 0) {
          lookupResult = ModelLookupTable_ContainsPackedKeyCf(0,3,effectDefinition->ownedNestedResource);
          if (!lookupResult.carry) {
            localPoint = ModelNodeRuntime_TransformLocalPointRegs
                               (lookupResult.entry,(ModelRuntimeNode *)modelNode);
            EffectRuntimePool_CreateInstanceFromDefinitionCf
                      (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference4, 0x0),
                       (modelNode->modelPayload).worldRotationAngle2,
                       (modelNode->modelPayload).worldRotationAngle1,
                       (modelNode->modelPayload).worldRotationAngle0,localPoint.edx,localPoint.ecx,
                       localPoint.eax,effectDefinition->linkedEffectDefinition,worldRuntime);
          }
        }
        completionCountdownPtr = &(effectSlot->lifecycleOwnerAndDefinition).ownerAndDefinition.runtimeValue24;
        *completionCountdownPtr = *completionCountdownPtr - 1;
        if (*completionCountdownPtr == 0) {
          pendingCompletionAction = effectSlot->completionAction;
          completionOwnerCarrier =
               (effectSlot->lifecycleOwnerAndDefinition).ownerAndDefinition.owner.modelNode;
          if (pendingCompletionAction == EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY) {
EffectModelRuntimeMaintenance_TransitionType1DestroyModel:
            if (completionOwnerCarrier != (void *)0x0) {
              ModelRuntimePool_DestroyHierarchyAndDetach(worldRuntime,completionOwnerCarrier);
            }
          }
          else {
            spawnArmyCompletionEntity = completionOwnerCarrier;
            if (pendingCompletionAction == EFFECT_RUNTIME_COMPLETION_SPAWN_ARMY_FROM_MODEL) {
EffectModelRuntimeMaintenance_TransitionType3SpawnArmy:
              if (spawnArmyCompletionEntity != (GameEntityRuntime *)0x0) {
                ownerClassRecord = (spawnArmyCompletionEntity->common).ownership.definitionOrClassRecord;
                ownerModelNode = (spawnArmyCompletionEntity->common).ownership.modelNode;
                frameAdvancedOrScratch = *ownerClassRecord;
                if (*(int *)(frameAdvancedOrScratch + 0x4c) == 0x12) {
                  armyCreateResult = ArmyRuntime_CreateInstanceFromAssetCf
                                     (6,(ownerModelNode->modelPayload).worldRotationAngle2,
                                      (ownerModelNode->worldTransform).translation.y,
                                      (ownerModelNode->worldTransform).translation.x,
                                      (spawnArmyCompletionEntity->common).ownership.ownerIndex,
                                      *(PckArmyAssetIdCatalog *)(frameAdvancedOrScratch + 0xc0),worldRuntime);
                  if (!armyCreateResult.carry) {
                    (*(int **)armyCreateResult.eax)[0xf] =
                         (int)(((int64_t)ownerClassRecord[0xf] *
                               (int64_t)*(int *)(**(int **)armyCreateResult.eax + 0x60)) /
                              (int64_t)*(int *)(frameAdvancedOrScratch + 0x60));
                    ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,spawnArmyCompletionEntity);
                  }
                }
              }
            }
            else {
              linkedHandlerCompletionOwner = completionOwnerCarrier;
              if (pendingCompletionAction == EFFECT_RUNTIME_COMPLETION_INVOKE_LINKED_HANDLER)
              goto EffectModelRuntimeMaintenance_TransitionType2InvokeLinkedHandler;
            }
          }
        }
      }
      break;
    case EFFECT_TRANSITION_INTEGRATE_LINEAR_MOTION_AND_SHADING_POSITION:
      scaledDirection = FixedMath_DirectionFromAnglesScaledRegs
                         ((modelNode->modelPayload).worldRotationAngle1,
                          (modelNode->modelPayload).worldRotationAngle0,
                          effectDefinition->movementSpeedQ12);
      previousRotationAngle1 = (modelNode->modelPayload).worldRotationAngle1;
      activeShadingRecord = modelNode->shadingRecord;
      translationPtr = &(modelNode->worldTransform).translation;
      translationPtr->x = translationPtr->x + scaledDirection.eax;
      translationAxisPtr = &(modelNode->worldTransform).translation.y;
      *translationAxisPtr = *translationAxisPtr + scaledDirection.ecx;
      translationAxisPtr = &(modelNode->worldTransform).translation.z;
      *translationAxisPtr = *translationAxisPtr + scaledDirection.edx;
      ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)modelNode);
      (modelNode->modelPayload).worldRotationAngle1 = (int)(previousRotationAngle1 * 0x3f + 0x4000) >> 6;
      if (activeShadingRecord != (GraphicsShadingRuntimeRecord *)0x0) {
        activeShadingRecord->worldXQ12 = activeShadingRecord->worldXQ12 + scaledDirection.eax;
        activeShadingRecord->worldYQ12 = activeShadingRecord->worldYQ12 + scaledDirection.ecx;
        activeShadingRecord->worldZQ12 = activeShadingRecord->worldZQ12 + scaledDirection.edx;
      }
      break;
    case EFFECT_TRANSITION_ADVANCE_TERRAIN_RELATIVE_MOTION_AND_TERMINATE_ON_CONTACT:
      terrainSample = FieldGrid_InterpolateTerrainHeightAndNormal
                         ((modelNode->worldTransform).translation.y,
                          (modelNode->worldTransform).translation.x,worldRuntime->fieldGrid);
      if (!terrainSample.carry) {
        /* Both register-returned directions are spilled to the stack in the binary; Ghidra showed
           them as &stack0xffffffd4 / &stack0xffffffc8. The dot product is symmetric. */
        scaledDirection = FixedMath_DirectionFromAnglesScaledRegs
                  ((int)terrainSample.packedNormalAngles >> 0x10,terrainSample.packedNormalAngles & 0xffff,
                   0x10000000);
        terrainNormalDirection.x = scaledDirection.eax;
        terrainNormalDirection.y = scaledDirection.ecx;
        terrainNormalDirection.z = scaledDirection.edx;
        scaledDirection = FixedMath_DirectionFromAnglesScaledRegs
                  ((modelNode->modelPayload).worldRotationAngle1 -
                   effectSlot->effectAgeTicks * effectSlot->effectAgeTicks * effectDefinition->unknown58,
                   (modelNode->modelPayload).worldRotationAngle0,0x10000000);
        motionDirection.x = scaledDirection.eax;
        motionDirection.y = scaledDirection.ecx;
        motionDirection.z = scaledDirection.edx;
        normalDotMotion = FixedVec3_DotQ28(&terrainNormalDirection,&motionDirection);
        if (normalDotMotion < 0) {
          if (effectSlot->stateTintArgb < 0x1000000) {
            InterpolationState_SetNegatedTargetAndRescaleProgress
                      (effectDefinition->shadingReleaseTransitionDurationTicks,
                       modelNode->shadingRecord);
            WorldRuntime_UnlinkNodeFromOwnerListD8((WorldOwnerListNode100 *)modelNode);
            (effectSlot->modelNodeOrSavedOffset).modelNode = (ModelRuntimeNode *)0x0;
            return;
          }
          frameAgeOrTintValue = effectSlot->stateTintArgb >> 0x18;
          frameAdvancedOrScratch = frameAgeOrTintValue - effectDefinition->unknown5C;
          if (frameAgeOrTintValue < effectDefinition->unknown5C) {
            frameAdvancedOrScratch = 0;
          }
          frameAgeOrTintValue = effectSlot->stateTintArgb & 0xffffff | frameAdvancedOrScratch << 0x18;
        }
        else {
          frameAgeOrTintValue = 0xffffffff;
          rotationAngle1Ptr = &(modelNode->modelPayload).worldRotationAngle1;
          *rotationAngle1Ptr = *rotationAngle1Ptr - effectSlot->effectAgeTicks * effectSlot->effectAgeTicks *
                              effectDefinition->unknown58;
        }
        effectSlot->stateTintArgb = frameAgeOrTintValue;
        if ((modelNode->runtimeFlags & 4) != 0) {
          effectOrDefinitionTintArgb = effectDefinition->stateTintArgb;
          /* PUNPCKLBW/PSRLW 4 both tints, PMULHW, PACKUSWB */
          modulatedLanes = pmulhw(EffectTint_UnpackBytesShiftRight(frameAgeOrTintValue,4),
                                  EffectTint_UnpackBytesShiftRight(effectOrDefinitionTintArgb,4));
          modelNode->tintArgb = EffectTint_PackWordsUnsignedSaturate(modulatedLanes);
        }
        ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)modelNode);
        completionCountdownPtr = &(effectSlot->lifecycleOwnerAndDefinition).ownerAndDefinition.runtimeValue24;
        *completionCountdownPtr = *completionCountdownPtr - 1;
        if (*completionCountdownPtr == 0) {
          pendingCompletionAction = effectSlot->completionAction;
          completionOwnerCarrier =
               (effectSlot->lifecycleOwnerAndDefinition).ownerAndDefinition.owner.modelNode;
          if (pendingCompletionAction == EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY)
          goto EffectModelRuntimeMaintenance_TransitionType1DestroyModel;
          linkedHandlerCompletionOwner = completionOwnerCarrier;
          if (pendingCompletionAction == EFFECT_RUNTIME_COMPLETION_INVOKE_LINKED_HANDLER) {
EffectModelRuntimeMaintenance_TransitionType2InvokeLinkedHandler:
            if ((linkedHandlerCompletionOwner != (EffectCompletionLinkedHandlerOwnerColumns104 *)0x0
                ) && (0 < (int)linkedHandlerCompletionOwner->auxiliaryValue80)) {
              FieldGrid_ApplyRadialTerrainHeightDeltaAndRefreshSurfaceCf
                        (linkedHandlerCompletionOwner->terrainMaterialIndex100,
                         linkedHandlerCompletionOwner->auxiliaryValue80,
                         linkedHandlerCompletionOwner->ownerSlot0,
                         (modelNode->worldTransform).translation.y,
                         (modelNode->worldTransform).translation.x,worldRuntime->fieldGrid);
            }
          }
          else {
            spawnArmyCompletionEntity = completionOwnerCarrier;
            if (pendingCompletionAction == EFFECT_RUNTIME_COMPLETION_SPAWN_ARMY_FROM_MODEL)
            goto EffectModelRuntimeMaintenance_TransitionType3SpawnArmy;
          }
        }
      }
    }
    remainingStepTicks = remainingStepTicks - 1;
    if (remainingStepTicks == 0) {
      return;
    }
  } while( true );
}

