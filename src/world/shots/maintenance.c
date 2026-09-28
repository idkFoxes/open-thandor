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
    lanes.uw[lane] = (uint16_t)(((value >> (lane * 8) & 0xff) * 0x101) >> shift);
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
void __thandor_void_preserve_eax_ecx_edx
ShotModelRuntimeMaintenance_RefreshTerrainClassAndTint
          (WorldRuntimeContext *worldRuntime,ShotModelRuntimeNodeClassView100 *modelNode)

{
  PackedArgb32 nodeTintArgb;
  PackedArgb32 definitionTintArgb;
  FieldGridRegionMask primaryOccupancyMask;
  uint32_t probeOccupancyMask;
  uint64_t tintProductWords;
  FixedDirection probeOffset;
  TerrainOccupancyResolvedMasksRegs12 resolvedMasks;
  uint32_t combinedOccupancyMask;
  ShotRuntimeSlot *shotRuntime;
  
  shotRuntime = modelNode->shotRuntime;
  primaryOccupancyMask =
       TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                 (0x1000 /* 1.0 in Q12 */,(modelNode->worldTransform).translation.y,
                  (modelNode->worldTransform).translation.x,worldRuntime->fieldGrid);
  if ((((shotRuntime->definitionOrSavedId).definition)->trajectoryMode ==
       SHOT_TRAJECTORY_DIRECT_LINE) && (modelNode->renderDepthBiasOrState != 0)) {
    /* renderDepthBiasOrState holds the beam length (see the primaryUpdate callback) */
    probeOffset = FixedMath_DirectionFromAnglesScaledRegs
                       ((modelNode->modelPayload).worldRotationAngle1,
                        (modelNode->modelPayload).worldRotationAngle0,
                        modelNode->renderDepthBiasOrState >> 1);
    probeOccupancyMask = TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                      (0x1000,probeOffset.y + (modelNode->worldTransform).translation.y,
                       probeOffset.x + (modelNode->worldTransform).translation.x,
                       worldRuntime->fieldGrid);
    combinedOccupancyMask = primaryOccupancyMask | probeOccupancyMask;
    probeOffset = FixedMath_DirectionFromAnglesScaledRegs
                       ((modelNode->modelPayload).worldRotationAngle1,
                        (modelNode->modelPayload).worldRotationAngle0,
                        modelNode->renderDepthBiasOrState);
    probeOccupancyMask = TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                      (0x1000,probeOffset.y + (modelNode->worldTransform).translation.y,
                       probeOffset.x + (modelNode->worldTransform).translation.x,
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
  definitionTintArgb = ((shotRuntime->definitionOrSavedId).definition)->stateTintArgb;
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
void __thandor_void_preserve_eax_ecx_edx
ShotRuntimeMaintenance_OccupancyRebuildNoOp(WorldRuntimeContext *worldRuntime,void *runtimeObject)

{
  return;
}


/* Address: 0x0052C1B0.
   g_RuntimeMaintenanceCallbackPhases.audioRefresh.shot: feeds the shot's position to the positioned sound slot
   its definition selects (terrainGridMaskIndex indexes the world's sound slot table, not a terrain mask),
   unless the shot is over a cell hidden by TerrainGrid_TestProjectedCellMaskBits01.
*/
void __thandor_void_preserve_eax_ecx_edx
ShotRuntimeMaintenance_UpdateHierarchyProjectedSound
          (WorldRuntimeContext *worldRuntime,ShotModelRuntimeNodeClassView100 *modelNode)

{
  GraphicsFixedVec3 *worldPosition;
  uint32_t soundSlotIndex;
  SpatialSoundSlot *slot;
  bool cellMasked;
  ShotRuntimeSlot *shotRuntime;
  
  shotRuntime = modelNode->shotRuntime;
  if ((worldRuntime->dwordArray != NULL) &&
     (soundSlotIndex = ((shotRuntime->definitionOrSavedId).definition)->terrainGridMaskIndex,
     soundSlotIndex < worldRuntime->dwordArrayCount)) {
    slot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex];
    worldPosition = &(modelNode->worldTransform).translation;
    if (slot != NULL) {
      cellMasked = TerrainGrid_TestProjectedCellMaskBits01
                        ((modelNode->worldTransform).translation.y,worldPosition->x,worldRuntime);
      if (!cellMasked) {
        SpatialSound_UpdateDesiredPositionedGains
                  (((shotRuntime->definitionOrSavedId).definition)->
                   positionedSoundMaximumDistanceQ12,
                   ((shotRuntime->definitionOrSavedId).definition)->positionedSoundGainQ15,
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
void __thandor_void_preserve_eax_ecx_edx
ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects
          (WorldRuntimeContext *worldRuntime,ShotModelRuntimeNodeClassView100 *modelNode)

{
  ShotAnimationFrameIndex *animationFrameIndexPtr;
  ShotLifetimeRemainingTicks *lifetimeTicksPtr;
  ShotSecondaryEffectCountdownTicks *secondaryCountdownPtr;
  Q12 *directionComponent2Ptr;
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
  FixedLengthAnglesEaxEcxEdx12 ballisticAngles;
  FixedDirection directionOffset;
  EffectDefinition *effectDefinition;
  WorldRuntimeContext *effectWorldRuntime;
  AngleTurn32 rotationAngle;
  Q12 impactValue;
  InGameSimulationStepBatchTicks remainingStepTicks;
  int targetClassIndex;
  ArmyRuntimeSlot *ownerArmy;
  ArmyRuntimeSlot *ownerOrHitArmy;
  GraphicsShadingRuntimeRecord *nodeShadingRecord;
  
  remainingStepTicks = g_InGameSimulationStepTicks;
  do {
    shotRuntime = modelNode->shotRuntime;
    shotDefinition = (shotRuntime->definitionOrSavedId).definition;
    shotRuntime->projectileAgeTicks = shotRuntime->projectileAgeTicks + 1;
    frameAccumulatorOrDistance = shotRuntime->animationFrameAccumulatorQ4 + 0x10; /* 1.0 in Q4 */
    frameCountDistanceOrAge = shotDefinition->animationFrameCount;
    shotRuntime->animationFrameAccumulatorQ4 = frameAccumulatorOrDistance;
    frameAdvanceThreshold = shotDefinition->animationFrameAdvanceThresholdQ4;
    if (shotDefinition->animationFrameAdvanceThresholdQ4 <= frameAccumulatorOrDistance) {
      animationFrameIndexPtr = &(shotRuntime->ownerAndTrajectory).animationFrameIndex;
      *animationFrameIndexPtr = *animationFrameIndexPtr + 1;
      modelNode->textureSubresourceBaseIndex = modelNode->textureSubresourceBaseIndex + 1;
      shotRuntime->animationFrameAccumulatorQ4 = frameAccumulatorOrDistance - frameAdvanceThreshold;
      if (frameCountDistanceOrAge <= (shotRuntime->ownerAndTrajectory).animationFrameIndex) {
        animationFrameIndexPtr = &(shotRuntime->ownerAndTrajectory).animationFrameIndex;
        *animationFrameIndexPtr = *animationFrameIndexPtr - frameCountDistanceOrAge;
        modelNode->textureSubresourceBaseIndex = modelNode->textureSubresourceBaseIndex - frameCountDistanceOrAge;
      }
    }
    lifetimeTicksPtr = &shotRuntime->lifetimeTicksRemaining;
    *lifetimeTicksPtr = *lifetimeTicksPtr - 1;
    modelNodeRuntime = (ModelRuntimeNode *)modelNode;
    if (*lifetimeTicksPtr == 0) goto UnlinkExpiredOrOrphanedProjectileAndReturn;
    secondaryCountdownPtr = &(shotRuntime->ownerAndTrajectory).secondaryEffectCountdownTicks;
    *secondaryCountdownPtr = *secondaryCountdownPtr - 1;
    if (*secondaryCountdownPtr == 0) {
      (shotRuntime->ownerAndTrajectory).secondaryEffectCountdownTicks =
           shotDefinition->secondaryEffectIntervalTicks;
      emitterLookup = ModelLookupTable_ContainsPackedKey(1,3,shotDefinition->ownedNestedResource);
      if (!emitterLookup.notFound) {
        effectDefinition = shotDefinition->secondaryEffectDefinition;
        effectWorldRuntime = worldRuntime;
        emitterWorldPoint = ModelNodeRuntime_TransformLocalPointRegs
                           (emitterLookup.entry,(ModelRuntimeNode *)modelNode);
        emitterWorldYQ12 = emitterWorldPoint.yQ12;
        EffectRuntimePool_CreateInstanceFromDefinition
                  (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference4, 0x0),0,0x4000,0,
                   emitterWorldPoint.zQ12,emitterWorldYQ12,emitterWorldPoint.xQ12,effectDefinition,effectWorldRuntime);
      }
    }
    if (shotDefinition->trajectoryMode == SHOT_TRAJECTORY_DIRECT_LINE) {
      ownerOrHitArmy = (shotRuntime->ownerAndTrajectory).ownerArmyRuntime;
      /* the beam reaches as far as the shot would fly in its lifetime */
      frameCountDistanceOrAge = shotDefinition->projectileLifetimeTicks * shotDefinition->launchSpeedQ12;
      rotationAngle = (modelNode->modelPayload).worldRotationAngle1;
      modelNode->renderDepthBiasOrState = frameCountDistanceOrAge;
      ownerModelNode = NULL;
      if (ownerOrHitArmy != NULL) {
        ownerModelNode = ownerOrHitArmy->modelNodeRuntime;
      }
      armyRaycast = ModelRuntime_RaycastCandidateListNearest
                         (rotationAngle - shotRuntime->elevationOffsetAngle16,
                          (modelNode->modelPayload).worldRotationAngle0,frameCountDistanceOrAge,
                          (modelNode->worldTransform).translation.z,
                          (modelNode->worldTransform).translation.y,
                          (modelNode->worldTransform).translation.x,WORLD_OWNER_RUNTIME_MODEL,
                          ownerModelNode,worldRuntime);
      nearestArmyHit = armyRaycast.nearestNodeOrScratch;
      frameAccumulatorOrDistance = armyRaycast.nearestDistanceQ12;
      /* hits on target classes and materials without an impact effect do not stop the shot */
      if ((armyRaycast.hit) &&
         (targetClassIndex = *(int *)(((((nearestArmyHit.nearestModelNode)->runtimePayload).modelRuntime)->
                               definitionOrSavedId).savedIdOrOffset + 0x5c),
         ((shotRuntime->definitionOrSavedId).definition)->targetClassImpactEffectDefinitions8
         [targetClassIndex] == NULL)) {
        frameAccumulatorOrDistance = MODEL_RAYCAST_NO_HIT_DISTANCE;
      }
      surfaceRaycast = FieldGrid_RaycastTerrainSurfaceDistance
                         ((modelNode->modelPayload).worldRotationAngle1 -
                          shotRuntime->elevationOffsetAngle16,
                          (modelNode->modelPayload).worldRotationAngle0,frameCountDistanceOrAge,
                          (modelNode->worldTransform).translation.z,
                          (modelNode->worldTransform).translation.y,
                          (modelNode->worldTransform).translation.x,worldRuntime->fieldGrid);
      terrainMaterialIndex = surfaceRaycast.materialOrCellIndex;
      terrainHitDistance = surfaceRaycast.distanceQ12;
      if ((surfaceRaycast.hit) &&
         (((shotRuntime->definitionOrSavedId).definition)->terrainImpactEffectDefinitions31[terrainMaterialIndex]
          == NULL)) {
        terrainHitDistance = FIELD_GRID_RAYCAST_MISS_DISTANCE;
      }
      surfaceRaycast = FieldGrid_RaycastSecondarySurfaceDistance
                         ((modelNode->modelPayload).worldRotationAngle1 -
                          shotRuntime->elevationOffsetAngle16,
                          (modelNode->modelPayload).worldRotationAngle0,frameCountDistanceOrAge,
                          (modelNode->worldTransform).translation.z,
                          (modelNode->worldTransform).translation.y,
                          (modelNode->worldTransform).translation.x,worldRuntime->fieldGrid);
      secondaryHitDistance = surfaceRaycast.distanceQ12;
      if ((surfaceRaycast.hit) &&
         (((shotRuntime->definitionOrSavedId).definition)->primaryEffectDefinition == NULL)) {
        secondaryHitDistance = FIELD_GRID_RAYCAST_MISS_DISTANCE;
      }
      if (secondaryHitDistance < terrainHitDistance) {
        if (secondaryHitDistance < frameAccumulatorOrDistance) {
          shotDefinition = (shotRuntime->definitionOrSavedId).definition;
          if ((secondaryHitDistance <= frameCountDistanceOrAge) &&
             (modelNode->renderDepthBiasOrState = secondaryHitDistance,
             (shotRuntime->impactEffectEmissionFlags & 1) == 0)) {
            shotRuntime->impactEffectEmissionFlags = shotRuntime->impactEffectEmissionFlags | 1;
            effectDefinition = shotDefinition->primaryEffectDefinition;
            effectWorldRuntime = worldRuntime;
            directionOffset = FixedMath_DirectionFromAnglesScaledRegs
                               ((modelNode->modelPayload).worldRotationAngle1,
                                (modelNode->modelPayload).worldRotationAngle0,secondaryHitDistance);
            EffectRuntimePool_CreateInstanceFromDefinition
                      (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference4, 0x0),0,0x4000,0,
                       directionOffset.z + (modelNode->worldTransform).translation.z,
                       directionOffset.y + (modelNode->worldTransform).translation.y,
                       directionOffset.x + (modelNode->worldTransform).translation.x,effectDefinition,effectWorldRuntime);
          }
        }
        else {
HandleNearestArmyHitAndContinueMotion:
          shotDefinition = (shotRuntime->definitionOrSavedId).definition;
          if (frameAccumulatorOrDistance <= frameCountDistanceOrAge) {
            modelNode->renderDepthBiasOrState = frameAccumulatorOrDistance;
            ShotRuntime_ApplyArmyHitRelationAndNotifications
                      (((nearestArmyHit.nearestModelNode)->runtimePayload).armyRuntime,shotRuntime);
            ownerOrHitArmy = ((nearestArmyHit.nearestModelNode)->runtimePayload).armyRuntime;
            ownerArmy = (shotRuntime->ownerAndTrajectory).ownerArmyRuntime;
            ownerFactionIndex = 0;
            if (ownerArmy != NULL) {
              ownerFactionIndex = ownerArmy->factionIndex;
            }
            rotationAngle = (modelNode->modelPayload).worldRotationAngle0;
            /* a beam deals its damage spread over its lifetime */
            workingValue = shotDefinition->targetClassImpactDamageQ12[targetClassIndex] /
                     (int)shotDefinition->projectileLifetimeTicks;
            effectDefinition = shotDefinition->targetClassImpactEffectDefinitions8[targetClassIndex];
            if (((shotRuntime->impactEffectEmissionFlags & 1) == 0) &&
               (effectDefinition != NULL)) {
              shotRuntime->impactEffectEmissionFlags = shotRuntime->impactEffectEmissionFlags | 1;
              effectWorldRuntime = worldRuntime;
              directionOffset = FixedMath_DirectionFromAnglesScaledRegs
                                 ((modelNode->modelPayload).worldRotationAngle1,
                                  (modelNode->modelPayload).worldRotationAngle0,frameAccumulatorOrDistance);
              EffectRuntimePool_CreateInstanceFromDefinition
                        (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference4, 0x0),0,
                         -(modelNode->modelPayload).worldRotationAngle1,
                         (modelNode->modelPayload).worldRotationAngle0 + 0x8000 & 0xffff,
                         directionOffset.z + (modelNode->worldTransform).translation.z,
                         directionOffset.y + (modelNode->worldTransform).translation.y,
                         directionOffset.x + (modelNode->worldTransform).translation.x,effectDefinition,effectWorldRuntime);
            }
            ArmyRuntime_ApplyImpactDamageToRuntimeAndParent(rotationAngle,ownerFactionIndex,workingValue,ownerOrHitArmy);
          }
        }
      }
      else {
        if (frameAccumulatorOrDistance <= terrainHitDistance) goto HandleNearestArmyHitAndContinueMotion;
        shotDefinition = (shotRuntime->definitionOrSavedId).definition;
        if ((terrainHitDistance <= frameCountDistanceOrAge) &&
           (modelNode->renderDepthBiasOrState = terrainHitDistance,
           (shotRuntime->impactEffectEmissionFlags & 1) == 0)) {
          shotRuntime->impactEffectEmissionFlags = shotRuntime->impactEffectEmissionFlags | 1;
          effectDefinition = shotDefinition->terrainImpactEffectDefinitions31[terrainMaterialIndex];
          effectWorldRuntime = worldRuntime;
          directionOffset = FixedMath_DirectionFromAnglesScaledRegs
                             ((modelNode->modelPayload).worldRotationAngle1,
                              (modelNode->modelPayload).worldRotationAngle0,terrainHitDistance);
          EffectRuntimePool_CreateInstanceFromDefinition
                    (EFFECT_RUNTIME_COMPLETION_INVOKE_LINKED_HANDLER,
                     THANDOR_BITCAST(ModelRuntimeNode *, EffectRuntimeOwnerReference4, (ModelRuntimeNode *)(shotDefinition->terrainImpactEffectOwnerSlots31 + terrainMaterialIndex)),0,
                     0x4000,0,directionOffset.z + (modelNode->worldTransform).translation.z,
                     directionOffset.y + (modelNode->worldTransform).translation.y,
                     directionOffset.x + (modelNode->worldTransform).translation.x,effectDefinition,effectWorldRuntime);
          shotRuntime = modelNode->shotRuntime;
        }
      }
      workingValue = ((shotRuntime->definitionOrSavedId).definition)->modelSpinStepTurn16;
      modelNode->runtimeFlags = modelNode->runtimeFlags | 1;
      rotationAnglePtr = &(modelNode->modelPayload).worldRotationAngle2;
      *rotationAnglePtr = *rotationAnglePtr + workingValue;
      rotationAnglePtr = &(modelNode->modelPayload).worldRotationAngle2;
      *rotationAnglePtr = *rotationAnglePtr & 0xffff;
    }
    else {
      ownerOrHitArmy = (shotRuntime->ownerAndTrajectory).ownerArmyRuntime;
      ownerModelNode = NULL;
      if (ownerOrHitArmy != NULL) {
        ownerModelNode = ownerOrHitArmy->modelNodeRuntime;
      }
      armyRaycast = ModelRuntime_RaycastCandidateListNearest
                         ((modelNode->modelPayload).worldRotationAngle1 -
                          shotRuntime->elevationOffsetAngle16,
                          (modelNode->modelPayload).worldRotationAngle0,shotRuntime->launchSpeedQ12,
                          (modelNode->worldTransform).translation.z,
                          (modelNode->worldTransform).translation.y,
                          (modelNode->worldTransform).translation.x,WORLD_OWNER_RUNTIME_MODEL,
                          ownerModelNode,worldRuntime);
      nearestArmyHit = armyRaycast.nearestNodeOrScratch;
      frameCountDistanceOrAge = armyRaycast.nearestDistanceQ12;
      /* hits on target classes and materials without an impact effect do not stop the shot */
      if ((armyRaycast.hit) &&
         (targetClassIndex = *(int *)(((((nearestArmyHit.nearestModelNode)->runtimePayload).modelRuntime)->
                               definitionOrSavedId).savedIdOrOffset + 0x5c),
         ((shotRuntime->definitionOrSavedId).definition)->targetClassImpactEffectDefinitions8
         [targetClassIndex] == NULL)) {
        frameCountDistanceOrAge = MODEL_RAYCAST_NO_HIT_DISTANCE;
      }
      surfaceRaycast = FieldGrid_RaycastTerrainSurfaceDistance
                         ((modelNode->modelPayload).worldRotationAngle1 -
                          shotRuntime->elevationOffsetAngle16,
                          (modelNode->modelPayload).worldRotationAngle0,shotRuntime->launchSpeedQ12,
                          (modelNode->worldTransform).translation.z,
                          (modelNode->worldTransform).translation.y,
                          (modelNode->worldTransform).translation.x,worldRuntime->fieldGrid);
      terrainMaterialIndex = surfaceRaycast.materialOrCellIndex;
      movingTerrainHitDistance = surfaceRaycast.distanceQ12;
      if ((surfaceRaycast.hit) &&
         (((shotRuntime->definitionOrSavedId).definition)->terrainImpactEffectDefinitions31[terrainMaterialIndex]
          == NULL)) {
        movingTerrainHitDistance = FIELD_GRID_RAYCAST_MISS_DISTANCE;
      }
      surfaceRaycast = FieldGrid_RaycastSecondarySurfaceDistance
                         ((modelNode->modelPayload).worldRotationAngle1 -
                          shotRuntime->elevationOffsetAngle16,
                          (modelNode->modelPayload).worldRotationAngle0,shotRuntime->launchSpeedQ12,
                          (modelNode->worldTransform).translation.z,
                          (modelNode->worldTransform).translation.y,
                          (modelNode->worldTransform).translation.x,worldRuntime->fieldGrid);
      frameAccumulatorOrDistance = surfaceRaycast.distanceQ12;
      if ((surfaceRaycast.hit) &&
         (((shotRuntime->definitionOrSavedId).definition)->primaryEffectDefinition == NULL)) {
        frameAccumulatorOrDistance = FIELD_GRID_RAYCAST_MISS_DISTANCE;
      }
      if (frameAccumulatorOrDistance < movingTerrainHitDistance) {
        if (frameAccumulatorOrDistance < frameCountDistanceOrAge) {
          shotDefinition = (shotRuntime->definitionOrSavedId).definition;
          if (frameAccumulatorOrDistance <= (uint32_t)shotRuntime->launchSpeedQ12) {
            InterpolationState_SetNegatedTargetAndRescaleProgress
                      (shotDefinition->shadingReleaseTransitionDurationTicks,modelNode->shadingRecord);
            effectDefinition = shotDefinition->primaryEffectDefinition;
            directionOffset = FixedMath_DirectionFromAnglesScaledRegs
                               ((modelNode->modelPayload).worldRotationAngle1,
                                (modelNode->modelPayload).worldRotationAngle0,frameAccumulatorOrDistance);
            EffectRuntimePool_CreateInstanceFromDefinition
                      (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference4, 0x0),0,0x4000,0,
                       directionOffset.z + (modelNode->worldTransform).translation.z,
                       directionOffset.y + (modelNode->worldTransform).translation.y,
                       directionOffset.x + (modelNode->worldTransform).translation.x,effectDefinition,worldRuntime);
            WorldRuntime_UnlinkNodeFromOwnerListD8((WorldOwnerListNode100 *)modelNode);
            (shotRuntime->modelNodeOrSavedOffset).modelNode = NULL;
            return;
          }
        }
        else {
HandleNearestArmyHitAndTerminateProjectile:
          shotDefinition = (shotRuntime->definitionOrSavedId).definition;
          if (frameCountDistanceOrAge <= (uint32_t)shotRuntime->launchSpeedQ12) {
            ShotRuntime_ApplyArmyHitRelationAndNotifications
                      (((nearestArmyHit.nearestModelNode)->runtimePayload).armyRuntime,shotRuntime);
            InterpolationState_SetNegatedTargetAndRescaleProgress
                      (shotDefinition->shadingReleaseTransitionDurationTicks,modelNode->shadingRecord);
            ownerOrHitArmy = ((nearestArmyHit.nearestModelNode)->runtimePayload).armyRuntime;
            impactValue = shotDefinition->targetClassImpactDamageQ12[targetClassIndex];
            ownerArmy = (shotRuntime->ownerAndTrajectory).ownerArmyRuntime;
            ownerFactionIndex = 0;
            if (ownerArmy != NULL) {
              ownerFactionIndex = ownerArmy->factionIndex;
            }
            effectDefinition = shotDefinition->targetClassImpactEffectDefinitions8[targetClassIndex];
            rotationAngle = (modelNode->modelPayload).worldRotationAngle0;
            if (effectDefinition != NULL) {
              directionOffset = FixedMath_DirectionFromAnglesScaledRegs
                                 ((modelNode->modelPayload).worldRotationAngle1,
                                  (modelNode->modelPayload).worldRotationAngle0,frameCountDistanceOrAge);
              EffectRuntimePool_CreateInstanceFromDefinition
                        (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference4, 0x0),0,
                         -(modelNode->modelPayload).worldRotationAngle1,
                         (modelNode->modelPayload).worldRotationAngle0 + 0x8000 & 0xffff,
                         directionOffset.z + (modelNode->worldTransform).translation.z,
                         directionOffset.y + (modelNode->worldTransform).translation.y,
                         directionOffset.x + (modelNode->worldTransform).translation.x,effectDefinition,worldRuntime
                        );
            }
            WorldRuntime_UnlinkNodeFromOwnerListD8((WorldOwnerListNode100 *)modelNode);
            (shotRuntime->modelNodeOrSavedOffset).modelNode = NULL;
            ArmyRuntime_ApplyImpactDamageToRuntimeAndParent(rotationAngle,ownerFactionIndex,impactValue,ownerOrHitArmy);
            return;
          }
        }
      }
      else {
        if (frameCountDistanceOrAge <= movingTerrainHitDistance) goto HandleNearestArmyHitAndTerminateProjectile;
        shotDefinition = (shotRuntime->definitionOrSavedId).definition;
        if (movingTerrainHitDistance <= (uint32_t)shotRuntime->launchSpeedQ12) {
          (shotRuntime->modelNodeOrSavedOffset).modelNode = NULL;
          InterpolationState_SetNegatedTargetAndRescaleProgress
                    (shotDefinition->shadingReleaseTransitionDurationTicks,modelNode->shadingRecord);
          effectDefinition = shotDefinition->terrainImpactEffectDefinitions31[terrainMaterialIndex];
          directionOffset = FixedMath_DirectionFromAnglesScaledRegs
                             ((modelNode->modelPayload).worldRotationAngle1,
                              (modelNode->modelPayload).worldRotationAngle0,
                              movingTerrainHitDistance);
          EffectRuntimePool_CreateInstanceFromDefinition
                    (EFFECT_RUNTIME_COMPLETION_INVOKE_LINKED_HANDLER,
                     THANDOR_BITCAST(ModelRuntimeNode *, EffectRuntimeOwnerReference4, (ModelRuntimeNode *)(shotDefinition->terrainImpactEffectOwnerSlots31 + terrainMaterialIndex)),0,
                     0x4000,0,directionOffset.z + (modelNode->worldTransform).translation.z,
                     directionOffset.y + (modelNode->worldTransform).translation.y,
                     directionOffset.x + (modelNode->worldTransform).translation.x,effectDefinition,worldRuntime);
          WorldRuntime_UnlinkNodeFromOwnerListD8((WorldOwnerListNode100 *)modelNode);
          return;
        }
      }
      shotDefinition = (shotRuntime->definitionOrSavedId).definition;
      if (shotDefinition->trajectoryMode != SHOT_TRAJECTORY_DIRECT_LINE) {
        directionOffset = FixedMath_DirectionFromAnglesScaledRegs
                           ((modelNode->modelPayload).worldRotationAngle1 -
                            shotRuntime->elevationOffsetAngle16,
                            (modelNode->modelPayload).worldRotationAngle0,
                            shotRuntime->launchSpeedQ12);
        trajectoryStepYQ12 = directionOffset.y;
        nodeShadingRecord = modelNode->shadingRecord;
        translationPtr = &(modelNode->worldTransform).translation;
        translationPtr->x = translationPtr->x + directionOffset.x;
        if (nodeShadingRecord != NULL) {
          nodeShadingRecord->worldXQ12 = nodeShadingRecord->worldXQ12 + directionOffset.x;
          nodeShadingRecord->worldYQ12 = nodeShadingRecord->worldYQ12 + trajectoryStepYQ12;
          nodeShadingRecord->worldZQ12 = nodeShadingRecord->worldZQ12 + directionOffset.z;
        }
        translationAxisPtr = &(modelNode->worldTransform).translation.y;
        *translationAxisPtr = *translationAxisPtr + trajectoryStepYQ12;
        shotDefinition = (shotRuntime->definitionOrSavedId).definition;
        translationAxisPtr = &(modelNode->worldTransform).translation.z;
        *translationAxisPtr = *translationAxisPtr + directionOffset.z;
        targetStateRecord = (shotRuntime->runtimeStateOrSavedOffset).runtimeStatePointer;
        /* guided shot: turn towards the target, at most guidanceTurnLimitAngle16 per tick and axis */
        if ((shotDefinition->guidanceTurnLimitAngle16 != 0) && (targetStateRecord != NULL)) {
          workingValue = targetStateRecord[1];
          if (*(int *)(*targetStateRecord + 0x4c) == 0x15) {
            workingValue = *(int *)(workingValue + 0xcc);
          }
          modelNodeRuntime = (shotRuntime->modelNodeOrSavedOffset).modelNode;
          targetAngles = FixedMath_VectorToAngles3Regs
                             ((*(int *)(*targetStateRecord + 0x50) + *(int *)(workingValue + 0x9c)) -
                              (modelNodeRuntime->worldTransform).translation.z,
                              *(int *)(workingValue + 0x98) -
                              (modelNodeRuntime->worldTransform).translation.y,
                              *(int *)(workingValue + 0x94) -
                              (modelNodeRuntime->worldTransform).translation.x);
          elevationTurnDeltaAngle16 =
               targetAngles.elevationAngle - (modelNodeRuntime->modelPayload).worldRotationAngle1;
          workingValue = shotDefinition->guidanceTurnLimitAngle16;
          headingTurnDeltaAngle16 =
               (int)((targetAngles.azimuthAngle - (modelNodeRuntime->modelPayload).worldRotationAngle0) * 0x10000)
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
          rotationAnglePtr = &(modelNodeRuntime->modelPayload).worldRotationAngle1;
          *rotationAnglePtr = *rotationAnglePtr + elevationTurnDeltaAngle16;
          (modelNodeRuntime->modelPayload).worldRotationAngle0 =
               headingTurnDeltaAngle16 + (modelNodeRuntime->modelPayload).worldRotationAngle0 &
               0xffff;
        }
      }
      workingValue = shotDefinition->modelSpinStepTurn16;
      modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
      rotationAnglePtr = &(modelNodeRuntime->modelPayload).worldRotationAngle2;
      *rotationAnglePtr = *rotationAnglePtr + workingValue;
      rotationAnglePtr = &(modelNodeRuntime->modelPayload).worldRotationAngle2;
      *rotationAnglePtr = *rotationAnglePtr & 0xffff;
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
              rotationAnglePtr = &(modelNodeRuntime->modelPayload).worldRotationAngle1;
              *rotationAnglePtr = *rotationAnglePtr + workingValue;
            }
          }
        }
        else if (shotDefinition->trajectoryMode == SHOT_TRAJECTORY_BALLISTIC) {
          directionComponent2Ptr = &(shotRuntime->ownerAndTrajectory).directionComponent2Q12;
          *directionComponent2Ptr = *directionComponent2Ptr - ((shotRuntime->definitionOrSavedId).definition)->ballisticDivisorQ12;
          ballisticAngles = FixedMath_VectorToAnglesAndLengthVec3Regs
                             ((GraphicsFixedVec3 *)
                              &(shotRuntime->ownerAndTrajectory).directionComponent0Q12);
          ballisticAzimuthAngle = ballisticAngles.azimuthAngle;
          shotRuntime->launchSpeedQ12 = ballisticAngles.lengthQ12;
          (modelNodeRuntime->modelPayload).worldRotationAngle0 = ballisticAzimuthAngle;
          (modelNodeRuntime->modelPayload).worldRotationAngle1 = ballisticAngles.elevationAngle;
        }
        else if (shotDefinition->trajectoryMode == SHOT_TRAJECTORY_FIXED_RANGE) {
          if ((int)(modelNodeRuntime->modelPayload).worldRotationAngle1 < 0) {
            nodeShadingRecord = modelNodeRuntime->shadingRecord;
            if ((shotRuntime->runtimeStateOrSavedOffset).runtimeState != 0) {
              workingValue = *(int *)((shotRuntime->runtimeStateOrSavedOffset).runtimeState + 4);
              targetDeltaX = *(int *)(workingValue + 0x94) - (modelNodeRuntime->worldTransform).translation.x;
              workingValue = *(int *)(workingValue + 0x98) - (modelNodeRuntime->worldTransform).translation.y;
              translationPtr = &(modelNodeRuntime->worldTransform).translation;
              translationPtr->x = translationPtr->x + targetDeltaX;
              translationAxisPtr = &(modelNodeRuntime->worldTransform).translation.y;
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
              if ((shotRuntime->runtimeStateOrSavedOffset).runtimeState == 0) {
UnlinkExpiredOrOrphanedProjectileAndReturn:
                InterpolationState_SetNegatedTargetAndRescaleProgress
                          (shotDefinition->shadingReleaseTransitionDurationTicks,
                           modelNodeRuntime->shadingRecord);
                WorldRuntime_UnlinkNodeFromOwnerListD8((WorldOwnerListNode100 *)modelNodeRuntime);
                (shotRuntime->modelNodeOrSavedOffset).modelNode = NULL;
                return;
              }
              workingValue = *(int *)((shotRuntime->runtimeStateOrSavedOffset).runtimeState + 4);
              nodeShadingRecord = modelNodeRuntime->shadingRecord;
              targetDeltaX = *(int *)(workingValue + 0x94) - (modelNodeRuntime->worldTransform).translation.x;
              workingValue = *(int *)(workingValue + 0x98) - (modelNodeRuntime->worldTransform).translation.y;
              translationPtr = &(modelNodeRuntime->worldTransform).translation;
              translationPtr->x = translationPtr->x + targetDeltaX;
              translationAxisPtr = &(modelNodeRuntime->worldTransform).translation.y;
              *translationAxisPtr = *translationAxisPtr + workingValue;
              rotationAnglePtr = &(modelNodeRuntime->modelPayload).worldRotationAngle1;
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
    remainingStepTicks = remainingStepTicks - 1;
    if (remainingStepTicks == 0) {
      return;
    }
  } while( true );
}

