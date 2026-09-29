/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/shots/maintenance.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/shots/maintenance.h>
#include <thandor/thandor.h>

/* Implementation ownership: world/shots/maintenance. */

/* PUNPCKLBW mm,mm then PSRLW mm,shift: the four bytes b of value as the words ((b << 8) | b) >> shift. */
static __inline uint64_t ShotTint_UnpackBytesShiftRight(uint32_t value,int shift)

{
  ThandorMmx lanes;
  int lane;

  for (lane = 0; lane < 4; lane = lane + 1) {
    lanes.uw[lane] = (uint16_t)(((value >> (lane * 8) & 0xff) * COLOR_CHANNEL_TO_WORD_LANE) >> shift);
  }
  return lanes.q;
}

/* PACKUSWB mm,mm (low dword): the four signed words saturated to unsigned bytes. */
static __inline uint32_t ShotTint_PackWordsUnsignedSaturate(uint64_t words)

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

/* Address: 0x0052C080.
   g_RuntimeMaintenanceCallbackPhases.terrainStateRefresh.shot: works out which factions are around the shot
   (for a direct-line shot also at the middle and the end of its beam), turns that into the
   TERRAIN_OCCUPANCY_FLAG_* visibility flags for the active faction and refreshes the state tint, then
   modulates the node tint with the shot definition's tint.
*/
void ShotModelRuntimeMaintenance_RefreshTerrainClassAndTint
          (WorldRuntimeContext *worldRuntime,ShotModelRuntimeNode *modelNode)

{
  PackedArgb32 nodeTintArgb;
  PackedArgb32 definitionTintArgb;
  FieldGridRegionMask primaryOccupancyMask;
  uint32_t probeOccupancyMask;
  uint64_t tintProductWords;
  FixedDirection probeOffset;
  TerrainOccupancyResolvedMasks resolvedMasks;
  uint32_t combinedOccupancyMask;
  ShotRuntimeSlot *shotRuntime;
  
  shotRuntime = modelNode->shotRuntime;
  primaryOccupancyMask =
       TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                 (Q12_ONE,modelNode->worldTransform.translation.y,
                  modelNode->worldTransform.translation.x,worldRuntime->fieldGrid);
  if ((shotRuntime->definitionOrSavedId.definition->trajectoryMode ==
       SHOT_TRAJECTORY_DIRECT_LINE) && (modelNode->renderDepthBiasOrState != 0)) {
    /* renderDepthBiasOrState holds the beam length (see the primaryUpdate callback) */
    probeOffset = FixedMath_DirectionFromAnglesScaledRegs
                       (modelNode->modelPayload.worldRotationAngle1,
                        modelNode->modelPayload.worldRotationAngle0,
                        modelNode->renderDepthBiasOrState >> 1);
    probeOccupancyMask = TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                      (Q12_ONE,probeOffset.y + modelNode->worldTransform.translation.y,
                       probeOffset.x + modelNode->worldTransform.translation.x,
                       worldRuntime->fieldGrid);
    combinedOccupancyMask = primaryOccupancyMask | probeOccupancyMask;
    probeOffset = FixedMath_DirectionFromAnglesScaledRegs
                       (modelNode->modelPayload.worldRotationAngle1,
                        modelNode->modelPayload.worldRotationAngle0,
                        modelNode->renderDepthBiasOrState);
    probeOccupancyMask = TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                      (Q12_ONE,probeOffset.y + modelNode->worldTransform.translation.y,
                       probeOffset.x + modelNode->worldTransform.translation.x,
                       worldRuntime->fieldGrid);
    primaryOccupancyMask = probeOccupancyMask | combinedOccupancyMask;
  }
  modelNode->runtimeFlags =
       modelNode->runtimeFlags & ~(TERRAIN_OCCUPANCY_FLAG_PRESENT | TERRAIN_OCCUPANCY_FLAG_SEEN_BEFORE);
  resolvedMasks = TerrainOccupancyMask_ResolveRuntimeClassFlags
                     (modelNode->runtimeFlags,0,primaryOccupancyMask,
                      (char)worldRuntime->activeFactionRuntimeIndex);
  modelNode->runtimeFlags = modelNode->runtimeFlags | resolvedMasks.runtimeFlags;
  shotRuntime->terrainRuntimeClassState = resolvedMasks.primaryOccupancyMask;
  ModelNodeRuntime_RefreshStateTint((ModelRuntimeNode *)modelNode);
  nodeTintArgb = modelNode->tintArgb;
  definitionTintArgb = shotRuntime->definitionOrSavedId.definition->stateTintArgb;
  /* per channel (a * 0x101 >> 4) * (b * 0x101 >> 4) >> 16, about a * b / 256 (MMX in the original) */
  tintProductWords =
       pmulhw(ShotTint_UnpackBytesShiftRight(nodeTintArgb,4),
              ShotTint_UnpackBytesShiftRight(definitionTintArgb,4));
  modelNode->tintArgb = ShotTint_PackWordsUnsignedSaturate(tintProductWords);
  return;
}


/* Address: 0x0052C1A0.
   g_RuntimeMaintenanceCallbackPhases.occupancyRebuild.shot: shots do not contribute to the terrain occupancy,
   so this callback does nothing (it still pops its two stack arguments).
*/
void ShotRuntimeMaintenance_OccupancyRebuildNoOp(WorldRuntimeContext *worldRuntime,void *runtimeObject)

{
  return;
}


/* Address: 0x0052C1B0.
   g_RuntimeMaintenanceCallbackPhases.audioRefresh.shot: feeds the shot's position to the positioned sound slot
   its definition selects (soundSlotIndex indexes the world's sound slot table, not a terrain mask),
   unless the shot is over a cell hidden by TerrainGrid_TestProjectedCellMaskBits01.
*/
void ShotRuntimeMaintenance_UpdateHierarchyProjectedSound
          (WorldRuntimeContext *worldRuntime,ShotModelRuntimeNode *modelNode)

{
  GraphicsFixedVec3 *worldPosition;
  uint32_t soundSlotIndex;
  SpatialSoundSlot *slot;
  bool cellMasked;
  ShotRuntimeSlot *shotRuntime;
  
  shotRuntime = modelNode->shotRuntime;
  if ((worldRuntime->dwordArray != NULL) &&
     (soundSlotIndex = shotRuntime->definitionOrSavedId.definition->soundSlotIndex,
     soundSlotIndex < worldRuntime->dwordArrayCount)) {
    slot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex];
    worldPosition = &modelNode->worldTransform.translation;
    if (slot != NULL) {
      cellMasked = TerrainGrid_TestProjectedCellMaskBits01
                        (modelNode->worldTransform.translation.y,worldPosition->x,worldRuntime);
      if (!cellMasked) {
        SpatialSound_UpdateDesiredPositionedGains
                  (shotRuntime->definitionOrSavedId.definition->
                   positionedSoundMaximumDistanceQ12,
                   shotRuntime->definitionOrSavedId.definition->positionedSoundGainQ15,
                   worldPosition,slot);
      }
    }
  }
  return;
}


/* Address: 0x0052C230.
   g_RuntimeMaintenanceCallbackPhases.primaryUpdate.shot: advances a shot by g_InGameSimulationStepTicks ticks.
   Each tick steps the animation, ages the shot (it expires when its lifetime runs out), emits the secondary
   trail effect, then casts a ray against armies, terrain and the secondary surface. A direct-line shot (beam)
   stays in place: its length is cut at the nearest hit, whose impact effect is emitted once, and an army hit
   takes the impact damage spread over the beam's lifetime. Every other shot moves by its speed (with guidance
   towards its target) and ends at the first hit, emitting the impact effect and dealing the full damage to
   an army. Ballistic, lead-adjusted and fixed-range shots then update speed and angles for the next tick.
*/
void ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects
          (WorldRuntimeContext *worldRuntime,ShotModelRuntimeNode *modelNode)

{
  ShotLifetimeRemainingTicks *lifetimeTicksPtr;
  GraphicsFixedVec3 *translationPtr;
  GraphicsWorldCoordinateQ12 *translationAxisPtr;
  AngleTurn32 *rotationAnglePtr;
  ShotFrameAdvanceThresholdQ4 frameAdvanceThreshold;
  int *targetStateRecord;
  uint32_t frameAccumulatorOrDistance;
  uint32_t frameCountDistanceOrAge;
  uint32_t movingTerrainHitDistance;
  int workingValue;
  int targetDeltaX;
  ModelRuntimeNode *ownerModelNode;
  uint32_t terrainHitDistance;
  uint32_t secondaryHitDistance;
  Q12 emitterWorldYQ12;
  Q12 trajectoryStepYQ12;
  AngleTurn16Stored32 headingTurnDeltaAngle16;
  AngleTurn32 ballisticAzimuthAngle;
  ModelRaycastNearestNodeOrScratch4 nearestArmyHit;
  uint32_t terrainMaterialIndex;
  AngleTurn16Stored32 elevationTurnDeltaAngle16;
  ShotDefinition *shotDefinition;
  FactionRuntimeIndex ownerFactionIndex;
  ShotRuntimeSlot *shotRuntime;
  ModelRuntimeNode *modelNodeRuntime;
  ModelLookupEntryResult emitterLookup;
  ModelRaycastResult armyRaycast;
  TerrainRaycastResult surfaceRaycast;
  FixedVectorAngles targetAngles;
  ModelWorldPoint emitterWorldPoint;
  FixedLengthAzimuthElevation ballisticAngles;
  FixedDirection directionOffset;
  EffectDefinition *effectDefinition;
  WorldRuntimeContext *effectWorldRuntime;
  AngleTurn32 rotationAngle;
  Q12 impactValue;
  InGameSimulationStepBatchTicks remainingStepTicks;
  int targetClassIndex;
  ArmyRuntimeSlot *ownerArmy;
  ArmyRuntimeSlot *shotOwnerArmy;
  ModelRuntimeSlot *hitModelRuntime;
  GraphicsShadingRuntimeRecord *nodeShadingRecord;
  
  remainingStepTicks = g_InGameSimulationStepTicks;
  do {
    shotRuntime = modelNode->shotRuntime;
    shotDefinition = shotRuntime->definitionOrSavedId.definition;
    shotRuntime->projectileAgeTicks = shotRuntime->projectileAgeTicks + 1;
    frameAccumulatorOrDistance = shotRuntime->animationFrameAccumulatorQ4 + (1 << Q4_SHIFT);
    frameCountDistanceOrAge = shotDefinition->animationFrameCount;
    shotRuntime->animationFrameAccumulatorQ4 = frameAccumulatorOrDistance;
    frameAdvanceThreshold = shotDefinition->animationFrameAdvanceThresholdQ4;
    if (shotDefinition->animationFrameAdvanceThresholdQ4 <= frameAccumulatorOrDistance) {
      shotRuntime->ownerAndTrajectory.animationFrameIndex += 1;
      modelNode->textureSubresourceBaseIndex = modelNode->textureSubresourceBaseIndex + 1;
      shotRuntime->animationFrameAccumulatorQ4 = frameAccumulatorOrDistance - frameAdvanceThreshold;
      if (frameCountDistanceOrAge <= shotRuntime->ownerAndTrajectory.animationFrameIndex) {
        shotRuntime->ownerAndTrajectory.animationFrameIndex -= frameCountDistanceOrAge;
        modelNode->textureSubresourceBaseIndex = modelNode->textureSubresourceBaseIndex - frameCountDistanceOrAge;
      }
    }
    lifetimeTicksPtr = &shotRuntime->lifetimeTicksRemaining;
    *lifetimeTicksPtr = *lifetimeTicksPtr - 1;
    modelNodeRuntime = (ModelRuntimeNode *)modelNode;
    if (*lifetimeTicksPtr == 0) goto UnlinkExpiredOrOrphanedProjectileAndReturn;
    shotRuntime->ownerAndTrajectory.secondaryEffectCountdownTicks--;
    if (shotRuntime->ownerAndTrajectory.secondaryEffectCountdownTicks == 0) {
      shotRuntime->ownerAndTrajectory.secondaryEffectCountdownTicks =
           shotDefinition->secondaryEffectIntervalTicks;
      emitterLookup = ModelLookupTable_ContainsPackedKey
                         (1,MODEL_POINT_CLASS_EFFECT,shotDefinition->ownedNestedResource);
      if (!emitterLookup.notFound) {
        effectDefinition = shotDefinition->secondaryEffectDefinition;
        effectWorldRuntime = worldRuntime;
        emitterWorldPoint = ModelNodeRuntime_TransformLocalPointRegs
                           (emitterLookup.entry,(ModelRuntimeNode *)modelNode);
        emitterWorldYQ12 = emitterWorldPoint.yQ12;
        EffectRuntimePool_CreateInstanceFromDefinition
                  (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference, 0),0,
                   FIXED_ANGLE16_QUARTER_TURN,0,
                   emitterWorldPoint.zQ12,emitterWorldYQ12,emitterWorldPoint.xQ12,effectDefinition,effectWorldRuntime);
      }
    }
    if (shotDefinition->trajectoryMode == SHOT_TRAJECTORY_DIRECT_LINE) {
      shotOwnerArmy = shotRuntime->ownerAndTrajectory.ownerArmyRuntime;
      /* the beam reaches as far as the shot would fly in its lifetime */
      frameCountDistanceOrAge = shotDefinition->projectileLifetimeTicks * shotDefinition->launchSpeedQ12;
      rotationAngle = modelNode->modelPayload.worldRotationAngle1;
      modelNode->renderDepthBiasOrState = frameCountDistanceOrAge;
      ownerModelNode = NULL;
      if (shotOwnerArmy != NULL) {
        ownerModelNode = shotOwnerArmy->modelNodeRuntime;
      }
      armyRaycast = ModelRuntime_RaycastCandidateListNearest
                         (rotationAngle - shotRuntime->elevationOffsetAngle16,
                          modelNode->modelPayload.worldRotationAngle0,frameCountDistanceOrAge,
                          modelNode->worldTransform.translation.z,
                          modelNode->worldTransform.translation.y,
                          modelNode->worldTransform.translation.x,WORLD_OWNER_RUNTIME_MODEL,
                          ownerModelNode,worldRuntime);
      nearestArmyHit = armyRaycast.nearestNodeOrScratch;
      frameAccumulatorOrDistance = armyRaycast.nearestDistanceQ12;
      /* hits on target classes and materials without an impact effect do not stop the shot */
      if ((armyRaycast.hit) &&
         (targetClassIndex = (nearestArmyHit.nearestModelNode->runtimePayload.modelRuntime->
                               definitionOrSavedId).runtimeDefinition->targetClassIndex,
         shotRuntime->definitionOrSavedId.definition->targetClassImpactEffectDefinitions8
         [targetClassIndex] == NULL)) {
        frameAccumulatorOrDistance = MODEL_RAYCAST_NO_HIT_DISTANCE;
      }
      surfaceRaycast = FieldGrid_RaycastTerrainSurfaceDistance
                         (modelNode->modelPayload.worldRotationAngle1 -
                          shotRuntime->elevationOffsetAngle16,
                          modelNode->modelPayload.worldRotationAngle0,frameCountDistanceOrAge,
                          modelNode->worldTransform.translation.z,
                          modelNode->worldTransform.translation.y,
                          modelNode->worldTransform.translation.x,worldRuntime->fieldGrid);
      terrainMaterialIndex = surfaceRaycast.materialOrCellIndex;
      terrainHitDistance = surfaceRaycast.distanceQ12;
      if ((surfaceRaycast.hit) &&
         (shotRuntime->definitionOrSavedId.definition->terrainImpactEffectDefinitions31[terrainMaterialIndex]
          == NULL)) {
        terrainHitDistance = FIELD_GRID_RAYCAST_MISS_DISTANCE;
      }
      surfaceRaycast = FieldGrid_RaycastSecondarySurfaceDistance
                         (modelNode->modelPayload.worldRotationAngle1 -
                          shotRuntime->elevationOffsetAngle16,
                          modelNode->modelPayload.worldRotationAngle0,frameCountDistanceOrAge,
                          modelNode->worldTransform.translation.z,
                          modelNode->worldTransform.translation.y,
                          modelNode->worldTransform.translation.x,worldRuntime->fieldGrid);
      secondaryHitDistance = surfaceRaycast.distanceQ12;
      if ((surfaceRaycast.hit) &&
         (shotRuntime->definitionOrSavedId.definition->primaryEffectDefinition == NULL)) {
        secondaryHitDistance = FIELD_GRID_RAYCAST_MISS_DISTANCE;
      }
      if (secondaryHitDistance < terrainHitDistance) {
        if (secondaryHitDistance < frameAccumulatorOrDistance) {
          shotDefinition = shotRuntime->definitionOrSavedId.definition;
          if ((secondaryHitDistance <= frameCountDistanceOrAge) &&
             (modelNode->renderDepthBiasOrState = secondaryHitDistance,
             (shotRuntime->impactEffectEmissionFlags & SHOT_IMPACT_EFFECT_EMITTED) == 0)) {
            shotRuntime->impactEffectEmissionFlags |= SHOT_IMPACT_EFFECT_EMITTED;
            effectDefinition = shotDefinition->primaryEffectDefinition;
            effectWorldRuntime = worldRuntime;
            directionOffset = FixedMath_DirectionFromAnglesScaledRegs
                               (modelNode->modelPayload.worldRotationAngle1,
                                modelNode->modelPayload.worldRotationAngle0,secondaryHitDistance);
            EffectRuntimePool_CreateInstanceFromDefinition
                      (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference, 0),0,
                   FIXED_ANGLE16_QUARTER_TURN,0,
                       directionOffset.z + modelNode->worldTransform.translation.z,
                       directionOffset.y + modelNode->worldTransform.translation.y,
                       directionOffset.x + modelNode->worldTransform.translation.x,effectDefinition,effectWorldRuntime);
          }
        }
        else {
HandleNearestArmyHitAndContinueMotion:
          shotDefinition = shotRuntime->definitionOrSavedId.definition;
          if (frameAccumulatorOrDistance <= frameCountDistanceOrAge) {
            modelNode->renderDepthBiasOrState = frameAccumulatorOrDistance;
            ShotRuntime_ApplyArmyHitRelationAndNotifications
                      (nearestArmyHit.nearestModelNode->runtimePayload.modelRuntime,shotRuntime);
            hitModelRuntime = nearestArmyHit.nearestModelNode->runtimePayload.modelRuntime;
            ownerArmy = shotRuntime->ownerAndTrajectory.ownerArmyRuntime;
            ownerFactionIndex = 0;
            if (ownerArmy != NULL) {
              ownerFactionIndex = ownerArmy->factionIndex;
            }
            rotationAngle = modelNode->modelPayload.worldRotationAngle0;
            /* a beam deals its damage spread over its lifetime */
            workingValue = shotDefinition->targetClassImpactDamageQ12[targetClassIndex] /
                     (int)shotDefinition->projectileLifetimeTicks;
            effectDefinition = shotDefinition->targetClassImpactEffectDefinitions8[targetClassIndex];
            if (((shotRuntime->impactEffectEmissionFlags & SHOT_IMPACT_EFFECT_EMITTED) == 0) &&
               (effectDefinition != NULL)) {
              shotRuntime->impactEffectEmissionFlags |= SHOT_IMPACT_EFFECT_EMITTED;
              effectWorldRuntime = worldRuntime;
              directionOffset = FixedMath_DirectionFromAnglesScaledRegs
                                 (modelNode->modelPayload.worldRotationAngle1,
                                  modelNode->modelPayload.worldRotationAngle0,frameAccumulatorOrDistance);
              EffectRuntimePool_CreateInstanceFromDefinition
                        (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference, 0),0,
                         -modelNode->modelPayload.worldRotationAngle1,
                         modelNode->modelPayload.worldRotationAngle0 + FIXED_ANGLE16_HALF_TURN & FIXED_ANGLE16_MASK,
                         directionOffset.z + modelNode->worldTransform.translation.z,
                         directionOffset.y + modelNode->worldTransform.translation.y,
                         directionOffset.x + modelNode->worldTransform.translation.x,effectDefinition,
                         effectWorldRuntime);
            }
            ArmyRuntime_ApplyImpactDamageToRuntimeAndParent
                      (rotationAngle,ownerFactionIndex,workingValue,hitModelRuntime);
          }
        }
      }
      else {
        if (frameAccumulatorOrDistance <= terrainHitDistance) goto HandleNearestArmyHitAndContinueMotion;
        shotDefinition = shotRuntime->definitionOrSavedId.definition;
        if ((terrainHitDistance <= frameCountDistanceOrAge) &&
           (modelNode->renderDepthBiasOrState = terrainHitDistance,
           (shotRuntime->impactEffectEmissionFlags & SHOT_IMPACT_EFFECT_EMITTED) == 0)) {
          shotRuntime->impactEffectEmissionFlags |= SHOT_IMPACT_EFFECT_EMITTED;
          effectDefinition = shotDefinition->terrainImpactEffectDefinitions31[terrainMaterialIndex];
          effectWorldRuntime = worldRuntime;
          directionOffset = FixedMath_DirectionFromAnglesScaledRegs
                             (modelNode->modelPayload.worldRotationAngle1,
                              modelNode->modelPayload.worldRotationAngle0,terrainHitDistance);
          EffectRuntimePool_CreateInstanceFromDefinition
                    (EFFECT_RUNTIME_COMPLETION_INVOKE_LINKED_HANDLER,
                     THANDOR_BITCAST(ModelRuntimeNode *, EffectRuntimeOwnerReference,
                                     (ModelRuntimeNode *)(shotDefinition->terrainImpactHeightDeltasQ12 +
                                                          terrainMaterialIndex)),
                     0,FIXED_ANGLE16_QUARTER_TURN,0,directionOffset.z + modelNode->worldTransform.translation.z,
                     directionOffset.y + modelNode->worldTransform.translation.y,
                     directionOffset.x + modelNode->worldTransform.translation.x,effectDefinition,effectWorldRuntime);
          shotRuntime = modelNode->shotRuntime;
        }
      }
      workingValue = shotRuntime->definitionOrSavedId.definition->modelSpinStepTurn16;
      modelNode->runtimeFlags = modelNode->runtimeFlags | 1;
      modelNode->modelPayload.worldRotationAngle2 += workingValue;
      modelNode->modelPayload.worldRotationAngle2 &= FIXED_ANGLE16_MASK;
    }
    else {
      shotOwnerArmy = shotRuntime->ownerAndTrajectory.ownerArmyRuntime;
      ownerModelNode = NULL;
      if (shotOwnerArmy != NULL) {
        ownerModelNode = shotOwnerArmy->modelNodeRuntime;
      }
      armyRaycast = ModelRuntime_RaycastCandidateListNearest
                         (modelNode->modelPayload.worldRotationAngle1 -
                          shotRuntime->elevationOffsetAngle16,
                          modelNode->modelPayload.worldRotationAngle0,shotRuntime->launchSpeedQ12,
                          modelNode->worldTransform.translation.z,
                          modelNode->worldTransform.translation.y,
                          modelNode->worldTransform.translation.x,WORLD_OWNER_RUNTIME_MODEL,
                          ownerModelNode,worldRuntime);
      nearestArmyHit = armyRaycast.nearestNodeOrScratch;
      frameCountDistanceOrAge = armyRaycast.nearestDistanceQ12;
      /* hits on target classes and materials without an impact effect do not stop the shot */
      if ((armyRaycast.hit) &&
         (targetClassIndex = (nearestArmyHit.nearestModelNode->runtimePayload.modelRuntime->
                               definitionOrSavedId).runtimeDefinition->targetClassIndex,
         shotRuntime->definitionOrSavedId.definition->targetClassImpactEffectDefinitions8
         [targetClassIndex] == NULL)) {
        frameCountDistanceOrAge = MODEL_RAYCAST_NO_HIT_DISTANCE;
      }
      surfaceRaycast = FieldGrid_RaycastTerrainSurfaceDistance
                         (modelNode->modelPayload.worldRotationAngle1 -
                          shotRuntime->elevationOffsetAngle16,
                          modelNode->modelPayload.worldRotationAngle0,shotRuntime->launchSpeedQ12,
                          modelNode->worldTransform.translation.z,
                          modelNode->worldTransform.translation.y,
                          modelNode->worldTransform.translation.x,worldRuntime->fieldGrid);
      terrainMaterialIndex = surfaceRaycast.materialOrCellIndex;
      movingTerrainHitDistance = surfaceRaycast.distanceQ12;
      if ((surfaceRaycast.hit) &&
         (shotRuntime->definitionOrSavedId.definition->terrainImpactEffectDefinitions31[terrainMaterialIndex]
          == NULL)) {
        movingTerrainHitDistance = FIELD_GRID_RAYCAST_MISS_DISTANCE;
      }
      surfaceRaycast = FieldGrid_RaycastSecondarySurfaceDistance
                         (modelNode->modelPayload.worldRotationAngle1 -
                          shotRuntime->elevationOffsetAngle16,
                          modelNode->modelPayload.worldRotationAngle0,shotRuntime->launchSpeedQ12,
                          modelNode->worldTransform.translation.z,
                          modelNode->worldTransform.translation.y,
                          modelNode->worldTransform.translation.x,worldRuntime->fieldGrid);
      frameAccumulatorOrDistance = surfaceRaycast.distanceQ12;
      if ((surfaceRaycast.hit) &&
         (shotRuntime->definitionOrSavedId.definition->primaryEffectDefinition == NULL)) {
        frameAccumulatorOrDistance = FIELD_GRID_RAYCAST_MISS_DISTANCE;
      }
      if (frameAccumulatorOrDistance < movingTerrainHitDistance) {
        if (frameAccumulatorOrDistance < frameCountDistanceOrAge) {
          shotDefinition = shotRuntime->definitionOrSavedId.definition;
          if (frameAccumulatorOrDistance <= (uint32_t)shotRuntime->launchSpeedQ12) {
            InterpolationState_SetNegatedTargetAndRescaleProgress
                      (shotDefinition->shadingReleaseTransitionDurationTicks,modelNode->shadingRecord);
            effectDefinition = shotDefinition->primaryEffectDefinition;
            directionOffset = FixedMath_DirectionFromAnglesScaledRegs
                               (modelNode->modelPayload.worldRotationAngle1,
                                modelNode->modelPayload.worldRotationAngle0,frameAccumulatorOrDistance);
            EffectRuntimePool_CreateInstanceFromDefinition
                      (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference, 0),0,
                   FIXED_ANGLE16_QUARTER_TURN,0,
                       directionOffset.z + modelNode->worldTransform.translation.z,
                       directionOffset.y + modelNode->worldTransform.translation.y,
                       directionOffset.x + modelNode->worldTransform.translation.x,effectDefinition,worldRuntime);
            WorldRuntime_UnlinkOwnerListNode((WorldOwnerListNode *)modelNode);
            shotRuntime->modelNodeOrSavedOffset.modelNode = NULL;
            return;
          }
        }
        else {
HandleNearestArmyHitAndTerminateProjectile:
          shotDefinition = shotRuntime->definitionOrSavedId.definition;
          if (frameCountDistanceOrAge <= (uint32_t)shotRuntime->launchSpeedQ12) {
            ShotRuntime_ApplyArmyHitRelationAndNotifications
                      (nearestArmyHit.nearestModelNode->runtimePayload.modelRuntime,shotRuntime);
            InterpolationState_SetNegatedTargetAndRescaleProgress
                      (shotDefinition->shadingReleaseTransitionDurationTicks,modelNode->shadingRecord);
            hitModelRuntime = nearestArmyHit.nearestModelNode->runtimePayload.modelRuntime;
            impactValue = shotDefinition->targetClassImpactDamageQ12[targetClassIndex];
            ownerArmy = shotRuntime->ownerAndTrajectory.ownerArmyRuntime;
            ownerFactionIndex = 0;
            if (ownerArmy != NULL) {
              ownerFactionIndex = ownerArmy->factionIndex;
            }
            effectDefinition = shotDefinition->targetClassImpactEffectDefinitions8[targetClassIndex];
            rotationAngle = modelNode->modelPayload.worldRotationAngle0;
            if (effectDefinition != NULL) {
              directionOffset = FixedMath_DirectionFromAnglesScaledRegs
                                 (modelNode->modelPayload.worldRotationAngle1,
                                  modelNode->modelPayload.worldRotationAngle0,frameCountDistanceOrAge);
              EffectRuntimePool_CreateInstanceFromDefinition
                        (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference, 0),0,
                         -modelNode->modelPayload.worldRotationAngle1,
                         modelNode->modelPayload.worldRotationAngle0 + FIXED_ANGLE16_HALF_TURN & FIXED_ANGLE16_MASK,
                         directionOffset.z + modelNode->worldTransform.translation.z,
                         directionOffset.y + modelNode->worldTransform.translation.y,
                         directionOffset.x + modelNode->worldTransform.translation.x,effectDefinition,worldRuntime
                        );
            }
            WorldRuntime_UnlinkOwnerListNode((WorldOwnerListNode *)modelNode);
            shotRuntime->modelNodeOrSavedOffset.modelNode = NULL;
            ArmyRuntime_ApplyImpactDamageToRuntimeAndParent(rotationAngle,ownerFactionIndex,impactValue,hitModelRuntime);
            return;
          }
        }
      }
      else {
        if (frameCountDistanceOrAge <= movingTerrainHitDistance) goto HandleNearestArmyHitAndTerminateProjectile;
        shotDefinition = shotRuntime->definitionOrSavedId.definition;
        if (movingTerrainHitDistance <= (uint32_t)shotRuntime->launchSpeedQ12) {
          shotRuntime->modelNodeOrSavedOffset.modelNode = NULL;
          InterpolationState_SetNegatedTargetAndRescaleProgress
                    (shotDefinition->shadingReleaseTransitionDurationTicks,modelNode->shadingRecord);
          effectDefinition = shotDefinition->terrainImpactEffectDefinitions31[terrainMaterialIndex];
          directionOffset = FixedMath_DirectionFromAnglesScaledRegs
                             (modelNode->modelPayload.worldRotationAngle1,
                              modelNode->modelPayload.worldRotationAngle0,
                              movingTerrainHitDistance);
          EffectRuntimePool_CreateInstanceFromDefinition
                    (EFFECT_RUNTIME_COMPLETION_INVOKE_LINKED_HANDLER,
                     THANDOR_BITCAST(ModelRuntimeNode *, EffectRuntimeOwnerReference,
                                     (ModelRuntimeNode *)(shotDefinition->terrainImpactHeightDeltasQ12 +
                                                          terrainMaterialIndex)),
                     0,FIXED_ANGLE16_QUARTER_TURN,0,directionOffset.z + modelNode->worldTransform.translation.z,
                     directionOffset.y + modelNode->worldTransform.translation.y,
                     directionOffset.x + modelNode->worldTransform.translation.x,effectDefinition,worldRuntime);
          WorldRuntime_UnlinkOwnerListNode((WorldOwnerListNode *)modelNode);
          return;
        }
      }
      shotDefinition = shotRuntime->definitionOrSavedId.definition;
      if (shotDefinition->trajectoryMode != SHOT_TRAJECTORY_DIRECT_LINE) {
        directionOffset = FixedMath_DirectionFromAnglesScaledRegs
                           (modelNode->modelPayload.worldRotationAngle1 -
                            shotRuntime->elevationOffsetAngle16,
                            modelNode->modelPayload.worldRotationAngle0,
                            shotRuntime->launchSpeedQ12);
        trajectoryStepYQ12 = directionOffset.y;
        nodeShadingRecord = modelNode->shadingRecord;
        translationPtr = &modelNode->worldTransform.translation;
        translationPtr->x = translationPtr->x + directionOffset.x;
        if (nodeShadingRecord != NULL) {
          nodeShadingRecord->worldXQ12 = nodeShadingRecord->worldXQ12 + directionOffset.x;
          nodeShadingRecord->worldYQ12 = nodeShadingRecord->worldYQ12 + trajectoryStepYQ12;
          nodeShadingRecord->worldZQ12 = nodeShadingRecord->worldZQ12 + directionOffset.z;
        }
        translationAxisPtr = &modelNode->worldTransform.translation.y;
        *translationAxisPtr = *translationAxisPtr + trajectoryStepYQ12;
        shotDefinition = shotRuntime->definitionOrSavedId.definition;
        translationAxisPtr = &modelNode->worldTransform.translation.z;
        *translationAxisPtr = *translationAxisPtr + directionOffset.z;
        targetStateRecord = shotRuntime->runtimeStateOrSavedOffset.runtimeStatePointer;
        /* guided shot: turn towards the target, at most guidanceTurnLimitAngle16 per tick and axis */
        if ((shotDefinition->guidanceTurnLimitAngle16 != 0) && (targetStateRecord != NULL)) {
          /* the target's root model node; for an aircraft its first child */
          workingValue = (int)((ModelRuntimeSlot *)targetStateRecord)->rootModelNodeOrSavedOffset.modelNode;
          if (((ModelRuntimeSlot *)targetStateRecord)->definitionOrSavedId.runtimeDefinition->runtimeClassId ==
              MODEL_RUNTIME_CLASS_21_AIRCRAFT) {
            workingValue = (int)((ModelRuntimeNode *)workingValue)->childNodes[0];
          }
          modelNodeRuntime = shotRuntime->modelNodeOrSavedOffset.modelNode;
          targetAngles = FixedMath_VectorToAngles3Regs
                             ((((ModelRuntimeSlot *)targetStateRecord)->definitionOrSavedId.runtimeDefinition->
                               aimHeightOffsetQ12 +
                              ((ModelRuntimeNode *)workingValue)->worldTransform.translation.z) -
                              modelNodeRuntime->worldTransform.translation.z,
                              ((ModelRuntimeNode *)workingValue)->worldTransform.translation.y -
                              modelNodeRuntime->worldTransform.translation.y,
                              ((ModelRuntimeNode *)workingValue)->worldTransform.translation.x -
                              modelNodeRuntime->worldTransform.translation.x);
          elevationTurnDeltaAngle16 =
               targetAngles.elevationAngle - modelNodeRuntime->modelPayload.worldRotationAngle1;
          workingValue = shotDefinition->guidanceTurnLimitAngle16;
          headingTurnDeltaAngle16 =
               (int)((targetAngles.azimuthAngle - modelNodeRuntime->modelPayload.worldRotationAngle0) * FIXED_ANGLE16_FULL_TURN)
               >> 16; /* wrapped to a signed 16-bit turn */
          if (workingValue < elevationTurnDeltaAngle16) {
            elevationTurnDeltaAngle16 = workingValue;
          }
          if (workingValue < headingTurnDeltaAngle16) {
            headingTurnDeltaAngle16 = workingValue;
          }
          workingValue = -workingValue;
          if (headingTurnDeltaAngle16 < workingValue) {
            headingTurnDeltaAngle16 = workingValue;
          }
          if (elevationTurnDeltaAngle16 < workingValue) {
            elevationTurnDeltaAngle16 = workingValue;
          }
          modelNodeRuntime->modelPayload.worldRotationAngle1 += elevationTurnDeltaAngle16;
          modelNodeRuntime->modelPayload.worldRotationAngle0 =
               headingTurnDeltaAngle16 + modelNodeRuntime->modelPayload.worldRotationAngle0 &
               FIXED_ANGLE16_MASK;
        }
      }
      workingValue = shotDefinition->modelSpinStepTurn16;
      modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
      modelNodeRuntime->modelPayload.worldRotationAngle2 += workingValue;
      modelNodeRuntime->modelPayload.worldRotationAngle2 &= FIXED_ANGLE16_MASK;
      if (shotDefinition->trajectoryMode != SHOT_TRAJECTORY_DIRECT_LINE) {
        if (shotDefinition->trajectoryMode == SHOT_TRAJECTORY_LEAD_ADJUSTED) {
          if (shotDefinition->trajectoryRampDurationTicks != 0) {
            frameCountDistanceOrAge = shotRuntime->projectileAgeTicks;
            if (shotDefinition->trajectoryRampDurationTicks < frameCountDistanceOrAge) {
              frameCountDistanceOrAge = shotDefinition->trajectoryRampDurationTicks;
            }
            shotRuntime->launchSpeedQ12 =
                 (Q12)(((int64_t)
                        (int)(((int64_t)(int)frameCountDistanceOrAge * (int64_t)(int)frameCountDistanceOrAge) /
                             (int64_t)(int)shotDefinition->trajectoryRampDurationTicks) *
                       (int64_t)shotDefinition->launchSpeedQ12) /
                      (int64_t)(int)shotDefinition->trajectoryRampDurationTicks);
            if (shotDefinition->elevationOffsetAngle16 != 0) {
              frameCountDistanceOrAge = shotDefinition->trajectoryRampDurationTicks * 2;
              workingValue = frameCountDistanceOrAge - shotRuntime->projectileAgeTicks;
              if (frameCountDistanceOrAge < shotRuntime->projectileAgeTicks) {
                workingValue = 0;
              }
              workingValue = ((int)(((int64_t)workingValue * (int64_t)shotDefinition->elevationOffsetAngle16) /
                             (int64_t)(int)shotDefinition->trajectoryRampDurationTicks) >> 1) -
                       shotRuntime->elevationOffsetAngle16;
              shotRuntime->elevationOffsetAngle16 = shotRuntime->elevationOffsetAngle16 + workingValue;
              modelNodeRuntime->modelPayload.worldRotationAngle1 += workingValue;
            }
          }
        }
        else if (shotDefinition->trajectoryMode == SHOT_TRAJECTORY_BALLISTIC) {
          shotRuntime->ownerAndTrajectory.directionComponent2Q12 -=
               shotRuntime->definitionOrSavedId.definition->ballisticDivisorQ12;
          ballisticAngles = FixedMath_VectorToAnglesAndLengthVec3Regs
                             ((GraphicsFixedVec3 *)
                              &shotRuntime->ownerAndTrajectory.directionComponent0Q12);
          ballisticAzimuthAngle = ballisticAngles.azimuthAngle;
          shotRuntime->launchSpeedQ12 = ballisticAngles.lengthQ12;
          modelNodeRuntime->modelPayload.worldRotationAngle0 = ballisticAzimuthAngle;
          modelNodeRuntime->modelPayload.worldRotationAngle1 = ballisticAngles.elevationAngle;
        }
        else if (shotDefinition->trajectoryMode == SHOT_TRAJECTORY_FIXED_RANGE) {
          if ((int)(modelNodeRuntime->modelPayload).worldRotationAngle1 < 0) {
            nodeShadingRecord = modelNodeRuntime->shadingRecord;
            if (shotRuntime->runtimeStateOrSavedOffset.runtimeState != 0) {
              workingValue = (int)((ModelRuntimeSlot *)(shotRuntime->runtimeStateOrSavedOffset).runtimeStatePointer)->
                             rootModelNodeOrSavedOffset.modelNode;
              targetDeltaX = ((ModelRuntimeNode *)workingValue)->worldTransform.translation.x -
                             modelNodeRuntime->worldTransform.translation.x;
              workingValue = ((ModelRuntimeNode *)workingValue)->worldTransform.translation.y -
                             modelNodeRuntime->worldTransform.translation.y;
              translationPtr = &modelNodeRuntime->worldTransform.translation;
              translationPtr->x = translationPtr->x + targetDeltaX;
              translationAxisPtr = &modelNodeRuntime->worldTransform.translation.y;
              *translationAxisPtr = *translationAxisPtr + workingValue;
              if (nodeShadingRecord != NULL) {
                nodeShadingRecord->worldXQ12 = nodeShadingRecord->worldXQ12 + targetDeltaX;
                nodeShadingRecord->worldYQ12 = nodeShadingRecord->worldYQ12 + workingValue;
              }
            }
          }
          else {
            if (shotDefinition->trajectoryRampDurationTicks != 0) {
              frameCountDistanceOrAge = shotRuntime->projectileAgeTicks;
              if (shotDefinition->trajectoryRampDurationTicks < frameCountDistanceOrAge) {
                frameCountDistanceOrAge = shotDefinition->trajectoryRampDurationTicks;
              }
              shotRuntime->launchSpeedQ12 =
                   (Q12)(((int64_t)
                          (int)(((int64_t)
                                 (int)(((int64_t)(int)frameCountDistanceOrAge * (int64_t)(int)frameCountDistanceOrAge) /
                                      (int64_t)(int)shotDefinition->trajectoryRampDurationTicks) *
                                (int64_t)(int)frameCountDistanceOrAge) /
                               (int64_t)(int)shotDefinition->trajectoryRampDurationTicks) *
                         (int64_t)shotDefinition->launchSpeedQ12) /
                        (int64_t)(int)shotDefinition->trajectoryRampDurationTicks);
            }
            if (shotDefinition->fixedRangeTransitionAgeThresholdTicks <= shotRuntime->projectileAgeTicks) {
              shotDefinition = (ShotDefinition *)(shotRuntime->definitionOrSavedId).savedId;
              if (shotRuntime->runtimeStateOrSavedOffset.runtimeState == 0) {
UnlinkExpiredOrOrphanedProjectileAndReturn:
                InterpolationState_SetNegatedTargetAndRescaleProgress
                          (shotDefinition->shadingReleaseTransitionDurationTicks,
                           modelNodeRuntime->shadingRecord);
                WorldRuntime_UnlinkOwnerListNode((WorldOwnerListNode *)modelNodeRuntime);
                shotRuntime->modelNodeOrSavedOffset.modelNode = NULL;
                return;
              }
              workingValue = (int)((ModelRuntimeSlot *)(shotRuntime->runtimeStateOrSavedOffset).runtimeStatePointer)->
                             rootModelNodeOrSavedOffset.modelNode;
              nodeShadingRecord = modelNodeRuntime->shadingRecord;
              targetDeltaX = ((ModelRuntimeNode *)workingValue)->worldTransform.translation.x -
                             modelNodeRuntime->worldTransform.translation.x;
              workingValue = ((ModelRuntimeNode *)workingValue)->worldTransform.translation.y -
                             modelNodeRuntime->worldTransform.translation.y;
              translationPtr = &modelNodeRuntime->worldTransform.translation;
              translationPtr->x = translationPtr->x + targetDeltaX;
              modelNodeRuntime->worldTransform.translation.y += workingValue;
              rotationAnglePtr = &modelNodeRuntime->modelPayload.worldRotationAngle1;
              *rotationAnglePtr = -*rotationAnglePtr;
              if (nodeShadingRecord != NULL) {
                nodeShadingRecord->worldXQ12 = nodeShadingRecord->worldXQ12 + targetDeltaX;
                nodeShadingRecord->worldYQ12 = nodeShadingRecord->worldYQ12 + workingValue;
              }
            }
          }
        }
      }
    }
    ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
    ModelNodeRuntime_UpdateDepthBinMasks(0,modelNodeRuntime);
    remainingStepTicks--;
  } while (remainingStepTicks != 0);
}

