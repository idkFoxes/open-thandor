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

/* Modulates two ARGB tints channel by channel: PUNPCKLBW/PSRLW 4 both tints, PMULHW, PACKUSWB. */
static uint32_t EffectTint_Modulate(uint32_t effectTintArgb,uint32_t definitionTintArgb)

{
  uint64_t modulatedLanes;

  modulatedLanes = pmulhw(EffectTint_UnpackBytesShiftRight(effectTintArgb,4),
                          EffectTint_UnpackBytesShiftRight(definitionTintArgb,4));
  return EffectTint_PackWordsUnsignedSaturate(modulatedLanes);
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
  uint32_t primaryOccupancyMask;
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
    modelNode->tintArgb =
         EffectTint_Modulate(effectRuntime->stateTintArgb,effectRuntime->definitionOrSavedId.definition->stateTintArgb);
  }
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


/* Ends the effect: releases its shading record over the definition's release time and unlinks the model node. */
static void EffectLifecycle_ReleaseShadingAndUnlink
          (EffectModelRuntimeNode *modelNode,EffectRuntimeSlot *effectSlot,EffectDefinition *effectDefinition)

{
  InterpolationState_SetNegatedTargetAndRescaleProgress
            (effectDefinition->shadingReleaseTransitionDurationTicks,modelNode->shadingRecord);
  WorldRuntime_UnlinkOwnerListNode((WorldOwnerListNode *)modelNode);
  effectSlot->modelNodeOrSavedOffset.modelNode = NULL;
}

/* One step adds 1.0 (Q4) to the frame accumulator; a frame advances (returns 1) when it reaches the definition's
   threshold. */
static int EffectLifecycle_AccumulateAnimationFrame(EffectRuntimeSlot *effectSlot,EffectDefinition *effectDefinition)

{
  uint32_t frameAccumulatorQ4;

  frameAccumulatorQ4 = effectSlot->lifecycleOwnerAndDefinition.animationFrameAccumulatorQ4 + (1 << Q4_SHIFT);
  effectSlot->lifecycleOwnerAndDefinition.animationFrameAccumulatorQ4 = frameAccumulatorQ4;
  if (frameAccumulatorQ4 < effectDefinition->frameAdvanceThresholdQ4) {
    return 0;
  }
  effectSlot->lifecycleOwnerAndDefinition.animationFrameAccumulatorQ4 =
       frameAccumulatorQ4 - effectDefinition->frameAdvanceThresholdQ4;
  return 1;
}

/* Per advanced frame: next texture frame, then the shading start and stop countdowns. */
static void EffectLifecycle_UpdateShadingOnFrameAdvance
          (EffectModelRuntimeNode *modelNode,EffectRuntimeSlot *effectSlot,EffectDefinition *effectDefinition)

{
  ModelPackedPointRecord *packedPoint;
  ModelWorldPoint localPoint;

  modelNode->textureSubresourceBaseIndex++;
  effectSlot->shadingStartCountdownTicksRemaining--;
  if ((effectSlot->shadingStartCountdownTicksRemaining == 0) && (modelNode->shadingRecord == NULL)) {
    if (ModelLookupTable_FindPackedPoint
          (0,MODEL_POINT_CLASS_LIGHT,effectDefinition->ownedNestedResource,&packedPoint)) {
      localPoint = ModelNodeRuntime_TransformLocalPoint(packedPoint,(ModelRuntimeNode *)modelNode);
      /* the alpha byte of the shading colour is the radius in 1/16 world units */
      modelNode->shadingRecord = GraphicsShadingRuntime_AllocateRecord
                         (effectDefinition->shadingTransitionDurationTicks,
                          (effectDefinition->shadingColorArgb >> 24) << 8,
                          effectDefinition->shadingColorArgb,localPoint.zQ12,localPoint.yQ12,localPoint.xQ12);
    }
  }
  effectSlot->shadingStopCountdownTicksRemaining--;
  if ((effectSlot->shadingStopCountdownTicksRemaining == 0) && (modelNode->shadingRecord != NULL)) {
    InterpolationState_SetNegatedTargetAndRescaleProgress
              (effectDefinition->shadingReleaseTransitionDurationTicks,modelNode->shadingRecord);
    modelNode->shadingRecord = NULL;
  }
}

/* Alpha ramps 0 -> 255 over the fade-in and 255 -> 0 over the fade-out at the end of the animation; the node tint is
   the state tint modulated by the definition tint, or fully transparent when the local faction cannot see it. */
static void EffectLifecycle_UpdateAlphaFadeAndTint
          (EffectModelRuntimeNode *modelNode,EffectRuntimeSlot *effectSlot,EffectDefinition *effectDefinition)

{
  EffectAlphaFadeTicks fadeDurationTicks;
  EffectAlphaFadeTicks fadeOutDivisorTicks;
  uint32_t effectAgeTicks;
  uint32_t fadeOutStartTicks;

  effectAgeTicks = effectSlot->effectAgeTicks;
  fadeOutStartTicks = (effectDefinition->animationFrameCount * effectDefinition->frameAdvanceThresholdQ4
           >> 4) - effectDefinition->alphaFadeOutTicks;
  if (effectAgeTicks < effectDefinition->alphaFadeInTicks) {
    fadeDurationTicks = effectDefinition->alphaFadeInTicks;
    effectSlot->stateTintArgb = effectSlot->stateTintArgb & 0xffffff;
    effectSlot->stateTintArgb =
         effectSlot->stateTintArgb |
         (int)(((int64_t)(int)effectAgeTicks * 255) / (int64_t)(int)fadeDurationTicks) << 24;
  }
  else if ((fadeOutStartTicks < effectAgeTicks) && (0 < (int)effectDefinition->alphaFadeOutTicks)) {
    fadeDurationTicks = effectDefinition->alphaFadeOutTicks;
    fadeOutDivisorTicks = effectDefinition->alphaFadeOutTicks;
    effectSlot->stateTintArgb = effectSlot->stateTintArgb & 0xffffff;
    effectSlot->stateTintArgb =
         effectSlot->stateTintArgb |
         (int)(((int64_t)(int)((effectAgeTicks - fadeOutStartTicks) - fadeDurationTicks) * -255) /
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
    modelNode->tintArgb = EffectTint_Modulate(effectSlot->stateTintArgb,effectDefinition->stateTintArgb);
  }
}

/* Spawns the definition's periodic child effect at effect point 1 whenever the periodic countdown runs out. */
static void EffectLifecycle_CountDownPeriodicEffect
          (WorldRuntimeContext *worldRuntime,EffectModelRuntimeNode *modelNode,EffectRuntimeSlot *effectSlot,
          EffectDefinition *effectDefinition)

{
  ModelPackedPointRecord *packedPoint;
  ModelWorldPoint localPoint;
  EffectDefinition *periodicDefinition;

  effectSlot->periodicEffectCountdownTicks--;
  if (effectSlot->periodicEffectCountdownTicks != 0) {
    return;
  }
  effectSlot->periodicEffectCountdownTicks = effectDefinition->periodicEffectIntervalTicks;
  if (ModelLookupTable_FindPackedPoint
        (1,MODEL_POINT_CLASS_EFFECT,effectDefinition->ownedNestedResource,&packedPoint)) {
    periodicDefinition = effectDefinition->periodicEffectDefinition;
    localPoint = ModelNodeRuntime_TransformLocalPoint(packedPoint,(ModelRuntimeNode *)modelNode);
    /* the periodic child effect always starts with rotation angle 1 at a quarter turn */
    EffectRuntimePool_CreateInstanceFromDefinition
              (EFFECT_RUNTIME_COMPLETION_NONE,(EffectRuntimeOwnerReference){ .modelNode = NULL },0,
               FIXED_ANGLE16_QUARTER_TURN,0,
               localPoint.zQ12,localPoint.yQ12,localPoint.xQ12,periodicDefinition,worldRuntime);
  }
}

/* Spawns the definition's linked effect at effect point 0, with the node's rotation, when the linked effect
   countdown runs out. */
static void EffectLifecycle_CountDownLinkedEffect
          (WorldRuntimeContext *worldRuntime,EffectModelRuntimeNode *modelNode,EffectRuntimeSlot *effectSlot,
          EffectDefinition *effectDefinition)

{
  ModelPackedPointRecord *packedPoint;
  ModelWorldPoint localPoint;

  effectSlot->linkedEffectPresent--;
  if (effectSlot->linkedEffectPresent != 0) {
    return;
  }
  if (ModelLookupTable_FindPackedPoint
        (0,MODEL_POINT_CLASS_EFFECT,effectDefinition->ownedNestedResource,&packedPoint)) {
    localPoint = ModelNodeRuntime_TransformLocalPoint(packedPoint,(ModelRuntimeNode *)modelNode);
    EffectRuntimePool_CreateInstanceFromDefinition
              (EFFECT_RUNTIME_COMPLETION_NONE,(EffectRuntimeOwnerReference){ .modelNode = NULL },
               modelNode->modelPayload.worldRotationAngle2,
               modelNode->modelPayload.worldRotationAngle1,
               modelNode->modelPayload.worldRotationAngle0,localPoint.zQ12,localPoint.yQ12,
               localPoint.xQ12,effectDefinition->linkedEffectDefinition,worldRuntime);
  }
}

/* When the shot countdown runs out it restarts and the linked shot is fired from the next shot point, away from the
   node origin (the target is the shot point mirrored through it, i.e. twice its offset). */
static void EffectLifecycle_CountDownLinkedShot
          (WorldRuntimeContext *worldRuntime,EffectModelRuntimeNode *modelNode,EffectRuntimeSlot *effectSlot,
          EffectDefinition *effectDefinition)

{
  ModelPackedPointRecord *packedPoint;
  ModelWorldPoint localPoint;
  uint32_t shotPointIndex;

  effectSlot->linkedShotPresent--;
  if (effectSlot->linkedShotPresent != 0) {
    return;
  }
  shotPointIndex = effectSlot->lifecycleOwnerAndDefinition.nextShotPointIndex;
  effectSlot->linkedShotPresent = effectDefinition->linkedShotPresent;
  effectSlot->lifecycleOwnerAndDefinition.nextShotPointIndex =
       effectSlot->lifecycleOwnerAndDefinition.nextShotPointIndex + 1;
  if (ModelLookupTable_FindPackedPoint
        (shotPointIndex,MODEL_POINT_CLASS_SHOT,effectDefinition->ownedNestedResource,&packedPoint)) {
    localPoint = ModelNodeRuntime_TransformLocalPoint(packedPoint,(ModelRuntimeNode *)modelNode);
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

/* EFFECT_RUNTIME_COMPLETION_SPAWN_ARMY_FROM_MODEL: only an owner whose model definition has class 18 turns into the
   army asset named by classParameterC0. The new model keeps the owner's armour points (+0x3C) in proportion,
   rescaled by the two definitions' maximumHealth (presumably the full armour), and the owner is destroyed. */
static void EffectLifecycle_SpawnArmyFromOwner(WorldRuntimeContext *worldRuntime,GameEntityRuntime *ownerEntity)

{
  ModelRuntimeSlot *ownerModelSlot;
  ModelRuntimeNode *ownerModelNode;
  ModelDefinition *ownerDefinition;
  ArmyRuntimeSlot *createdArmy;
  ModelRuntimeSlot *createdModelSlot;

  if (ownerEntity == NULL) {
    return;
  }
  ownerModelSlot = (ModelRuntimeSlot *)ownerEntity->common.ownership.definitionOrClassRecord;
  ownerModelNode = ownerEntity->common.ownership.modelNode;
  ownerDefinition = ownerModelSlot->definitionOrSavedId.runtimeDefinition;
  if (ownerDefinition->runtimeClassId != MODEL_RUNTIME_CLASS_18) {
    return;
  }
  createdArmy = ArmyRuntime_CreateInstanceFromAsset
                     (ARMY_CREATE_COUNT_FOR_ACTIVE_FACTION | ARMY_CREATE_UNLOCK_TECHNOLOGY,
                      ownerModelNode->modelPayload.worldRotationAngle2,
                      ownerModelNode->worldTransform.translation.y,
                      ownerModelNode->worldTransform.translation.x,
                      ownerEntity->common.ownership.ownerIndex,
                      (PckArmyAssetIdCatalog)ownerDefinition->classParameterC0,
                      worldRuntime,NULL);
  if (createdArmy != NULL) {
    createdModelSlot = createdArmy->modelRuntimeOrSavedOffset.modelRuntime;
    createdModelSlot->health =
         (int)(((int64_t)(int)ownerModelSlot->health *
                (int64_t)(int)createdModelSlot->definitionOrSavedId.runtimeDefinition->maximumHealth) /
               (int64_t)(int)ownerDefinition->maximumHealth);
    ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,ownerEntity);
  }
}

/* Counts the completion countdown down and, when it runs out, performs the slot's completion action on its owner. */
static void EffectLifecycle_CountDownCompletionAction
          (WorldRuntimeContext *worldRuntime,EffectModelRuntimeNode *modelNode,EffectRuntimeSlot *effectSlot)

{
  EffectRuntimeCompletionAction pendingCompletionAction;
  EffectRuntimeOwnerReference owner;
  ShotTerrainImpactDeformationColumns *impactColumns;

  effectSlot->lifecycleOwnerAndDefinition.ownerAndDefinition.completionCountdownTicks--;
  if (effectSlot->lifecycleOwnerAndDefinition.ownerAndDefinition.completionCountdownTicks != 0) {
    return;
  }
  pendingCompletionAction = effectSlot->completionAction;
  owner = effectSlot->lifecycleOwnerAndDefinition.ownerAndDefinition.owner;
  if (pendingCompletionAction == EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY) {
    if (owner.modelRuntime != NULL) {
      ModelRuntimePool_DestroyHierarchyAndDetach(worldRuntime,owner.modelRuntime);
    }
  }
  else if (pendingCompletionAction == EFFECT_RUNTIME_COMPLETION_SPAWN_ARMY_FROM_MODEL) {
    EffectLifecycle_SpawnArmyFromOwner(worldRuntime,(GameEntityRuntime *)owner.modelNode);
  }
  else if (pendingCompletionAction == EFFECT_RUNTIME_COMPLETION_INVOKE_LINKED_HANDLER) {
    impactColumns = owner.terrainImpactColumns;
    if ((impactColumns != NULL) && (0 < (int)impactColumns->radiusWorldUnits)) {
      FieldGrid_ApplyRadialTerrainHeightDeltaAndRefreshSurface
                (impactColumns->terrainMaterialIndex,impactColumns->radiusWorldUnits,
                 impactColumns->heightDeltaQ12,
                 modelNode->worldTransform.translation.y,
                 modelNode->worldTransform.translation.x,worldRuntime->fieldGrid);
    }
  }
}

/* Moves the node (and its shading record) along its direction at the definition speed. */
static void EffectLifecycle_IntegrateLinearMotion
          (EffectModelRuntimeNode *modelNode,EffectDefinition *effectDefinition)

{
  FixedDirection scaledDirection;
  AngleTurn32 previousRotationAngle1;
  GraphicsShadingRuntimeRecord *activeShadingRecord;

  scaledDirection = FixedMath_DirectionFromAnglesScaled
                     (modelNode->modelPayload.worldRotationAngle1,
                      modelNode->modelPayload.worldRotationAngle0,
                      effectDefinition->movementSpeedQ12);
  previousRotationAngle1 = modelNode->modelPayload.worldRotationAngle1;
  activeShadingRecord = modelNode->shadingRecord;
  modelNode->worldTransform.translation.x += scaledDirection.x;
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
}

/* Terrain-relative motion: while the motion direction points away from the terrain normal the pitch drops with the
   square of the effect's age; moving into the ground fades the alpha by groundContactAlphaFadeStep per step and ends
   the effect (returns 1) once the alpha is already 0. Afterwards the completion countdown runs. Nothing happens
   outside the terrain. */
static int EffectLifecycle_AdvanceTerrainRelativeMotion
          (WorldRuntimeContext *worldRuntime,EffectModelRuntimeNode *modelNode,EffectRuntimeSlot *effectSlot,
          EffectDefinition *effectDefinition)

{
  Q12 terrainHeightQ12; /* sampled with the normal, unused here */
  uint32_t terrainNormalAngles;
  FixedDirection scaledDirection;
  GraphicsFixedVec3 terrainNormalDirection;
  GraphicsFixedVec3 motionDirection;
  int32_t normalDotMotion;
  uint32_t currentAlpha;
  int fadedAlpha;
  uint32_t newStateTintArgb;

  if (!FieldGrid_InterpolateTerrainHeightAndNormal
                     (modelNode->worldTransform.translation.y,
                      modelNode->worldTransform.translation.x,worldRuntime->fieldGrid,
                      &terrainHeightQ12,&terrainNormalAngles)) {
    return 0;
  }
  /* Both are unit vectors (0x10000000 = 1.0 in Q28); the motion elevation drops with the square of the effect's
     age. The dot product is symmetric. */
  scaledDirection = FixedMath_DirectionFromAnglesScaled
            ((int)terrainNormalAngles >> 16,terrainNormalAngles & FIXED_ANGLE16_MASK,
             Q28_ONE);
  terrainNormalDirection.x = scaledDirection.x;
  terrainNormalDirection.y = scaledDirection.y;
  terrainNormalDirection.z = scaledDirection.z;
  scaledDirection = FixedMath_DirectionFromAnglesScaled
            (modelNode->modelPayload.worldRotationAngle1 -
             effectSlot->effectAgeTicks * effectSlot->effectAgeTicks * effectDefinition->pitchDropPerAgeSquared,
             modelNode->modelPayload.worldRotationAngle0,Q28_ONE);
  motionDirection.x = scaledDirection.x;
  motionDirection.y = scaledDirection.y;
  motionDirection.z = scaledDirection.z;
  normalDotMotion = FixedVec3_DotQ28(&terrainNormalDirection,&motionDirection);
  if (normalDotMotion < 0) {
    if (effectSlot->stateTintArgb < ARGB8888_ALPHA_ONE) {
      EffectLifecycle_ReleaseShadingAndUnlink(modelNode,effectSlot,effectDefinition);
      return 1;
    }
    currentAlpha = effectSlot->stateTintArgb >> 24;
    fadedAlpha = currentAlpha - effectDefinition->groundContactAlphaFadeStep;
    if (currentAlpha < effectDefinition->groundContactAlphaFadeStep) {
      fadedAlpha = 0;
    }
    newStateTintArgb = effectSlot->stateTintArgb & 0xffffff | fadedAlpha << 24;
  }
  else {
    newStateTintArgb = 0xffffffff;
    modelNode->modelPayload.worldRotationAngle1 -= effectSlot->effectAgeTicks * effectSlot->effectAgeTicks *
                        effectDefinition->pitchDropPerAgeSquared;
  }
  effectSlot->stateTintArgb = newStateTintArgb;
  if ((modelNode->runtimeFlags & TERRAIN_OCCUPANCY_FLAG_PRESENT) != 0) {
    modelNode->tintArgb = EffectTint_Modulate(newStateTintArgb,effectDefinition->stateTintArgb);
  }
  ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)modelNode);
  EffectLifecycle_CountDownCompletionAction(worldRuntime,modelNode,effectSlot);
  return 0;
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
  EffectRuntimeSlot *effectSlot;
  EffectDefinition *effectDefinition;
  int frameAdvanced;
  InGameSimulationStepBatchTicks remainingStepTicks;

  remainingStepTicks = g_InGameSimulationStepTicks;
  do {
    effectSlot = modelNode->effectRuntime;
    effectDefinition = effectSlot->definitionOrSavedId.definition;
    frameAdvanced = EffectLifecycle_AccumulateAnimationFrame(effectSlot,effectDefinition);
    if (frameAdvanced) {
      effectSlot->animationFramesRemaining--;
      if (effectSlot->animationFramesRemaining == 0) {
        EffectLifecycle_ReleaseShadingAndUnlink(modelNode,effectSlot,effectDefinition);
        return;
      }
      EffectLifecycle_UpdateShadingOnFrameAdvance(modelNode,effectSlot,effectDefinition);
    }
    if (effectDefinition->transitionPrefix.transitionKind !=
        EFFECT_TRANSITION_ADVANCE_TERRAIN_RELATIVE_MOTION_AND_TERMINATE_ON_CONTACT) {
      EffectLifecycle_UpdateAlphaFadeAndTint(modelNode,effectSlot,effectDefinition);
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
    EffectLifecycle_CountDownPeriodicEffect(worldRuntime,modelNode,effectSlot,effectDefinition);
    switch(effectDefinition->transitionPrefix.transitionKind) {
    case EFFECT_TRANSITION_SPAWN_LINKED_EFFECT_AFTER_COUNTDOWN:
      if (frameAdvanced) {
        EffectLifecycle_CountDownLinkedEffect(worldRuntime,modelNode,effectSlot,effectDefinition);
      }
      break;
    case EFFECT_TRANSITION_ADVANCE_PERIODIC_EMISSION_AND_COMPLETION_ACTION:
      if (frameAdvanced) {
        EffectLifecycle_CountDownLinkedShot(worldRuntime,modelNode,effectSlot,effectDefinition);
        EffectLifecycle_CountDownLinkedEffect(worldRuntime,modelNode,effectSlot,effectDefinition);
        EffectLifecycle_CountDownCompletionAction(worldRuntime,modelNode,effectSlot);
      }
      break;
    case EFFECT_TRANSITION_INTEGRATE_LINEAR_MOTION_AND_SHADING_POSITION:
      EffectLifecycle_IntegrateLinearMotion(modelNode,effectDefinition);
      break;
    case EFFECT_TRANSITION_ADVANCE_TERRAIN_RELATIVE_MOTION_AND_TERMINATE_ON_CONTACT:
      if (EffectLifecycle_AdvanceTerrainRelativeMotion(worldRuntime,modelNode,effectSlot,effectDefinition)) {
        return;
      }
      break;
    }
    remainingStepTicks--;
  } while (remainingStepTicks != 0);
}

