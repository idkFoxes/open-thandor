/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/shots/maintenance.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/shots/maintenance.h>
#include <thandor/thandor.h>

/* Implementation ownership: world/shots/maintenance. */

/* Address: 0x0052C080.
   Ownership: world/shots/maintenance.
   Purpose: Table membership MAINTENANCE[4]. Classifies the shot model node against nearby terrain occupancy,
   refreshes runtime class flags and state tint, then composes the packed tint with the model-definition tint.
   Maintenance table phase terrainStateRefresh, object kind shot. The 4x3 table bytes, target body, calling
   convention, and RET 0x08 contract remain unchanged.
   Cross-module calls: TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint [world/terrain/occupancy],
   FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed], TerrainOccupancyMask_ResolveRuntimeClassFlags
   [world/terrain/occupancy], UiModelControl_RefreshStateTint [ui/controls/misc].
*/
void __thandor_void_preserve_eax_ecx_edx
ShotModelRuntimeMaintenance_RefreshTerrainClassAndTint
          (WorldRuntimeContext *worldRuntime,ShotModelRuntimeNodeClassView100 *modelNode)

{
  PackedArgb32 nodeTintArgb;
  PackedArgb32 definitionTintArgb;
  short tintLane0;
  short tintLane1;
  short tintLane2;
  short tintLane3;
  ushort nodeAlphaPair;
  ushort definitionAlphaPair;
  FieldGridRegionMask primaryOccupancyMask;
  dword probeOccupancyMask;
  undefined1 mm0PackedValue0ByteLane1;
  undefined1 mm0PackedValue0ByteLane2;
  undefined8 mm0PackedValue0;
  undefined1 definitionTintByteLane1;
  undefined1 definitionTintByteLane2;
  FixedDirectionXyzRegs12 probeOffset;
  TerrainOccupancyResolvedMasksRegs12 resolvedMasks;
  uint combinedOccupancyMask;
  ShotRuntimeSlot *shotRuntime;
  
  shotRuntime = modelNode->shotRuntime;
  primaryOccupancyMask =
       TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                 (0x1000,(modelNode->worldTransform).translation.y,
                  (modelNode->worldTransform).translation.x,worldRuntime->fieldGrid);
  if ((((shotRuntime->definitionOrSavedId).definition)->trajectoryMode ==
       SHOT_TRAJECTORY_DIRECT_LINE) && (modelNode->renderDepthBiasOrState != 0)) {
    probeOffset = FixedMath_DirectionFromAnglesScaledRegs
                       ((modelNode->modelPayload).worldRotationAngle1,
                        (modelNode->modelPayload).worldRotationAngle0,
                        modelNode->renderDepthBiasOrState >> 1);
    probeOccupancyMask = TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                      (0x1000,probeOffset.ecx + (modelNode->worldTransform).translation.y,
                       probeOffset.eax + (modelNode->worldTransform).translation.x,
                       worldRuntime->fieldGrid);
    combinedOccupancyMask = primaryOccupancyMask | probeOccupancyMask;
    probeOffset = FixedMath_DirectionFromAnglesScaledRegs
                       ((modelNode->modelPayload).worldRotationAngle1,
                        (modelNode->modelPayload).worldRotationAngle0,
                        modelNode->renderDepthBiasOrState);
    probeOccupancyMask = TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                      (0x1000,probeOffset.ecx + (modelNode->worldTransform).translation.y,
                       probeOffset.eax + (modelNode->worldTransform).translation.x,
                       worldRuntime->fieldGrid);
    primaryOccupancyMask = probeOccupancyMask | combinedOccupancyMask;
  }
  modelNode->runtimeFlags = modelNode->runtimeFlags & 0xfffffff3;
  resolvedMasks = TerrainOccupancyMask_ResolveRuntimeClassFlags
                     (modelNode->runtimeFlags,0,primaryOccupancyMask,
                      (char)worldRuntime->activeFactionRuntimeIndex);
  modelNode->runtimeFlags = modelNode->runtimeFlags | resolvedMasks.runtimeFlags;
  shotRuntime->terrainRuntimeClassState = resolvedMasks.primaryOccupancyMask;
  UiModelControl_RefreshStateTint((ModelRuntimeNode *)modelNode);
  nodeTintArgb = modelNode->tintArgb;
  definitionTintArgb = ((shotRuntime->definitionOrSavedId).definition)->stateTintArgb;
  mm0PackedValue0ByteLane1 = (undefined1)(nodeTintArgb >> 0x18);
  nodeAlphaPair = CONCAT11(mm0PackedValue0ByteLane1,mm0PackedValue0ByteLane1);
  mm0PackedValue0ByteLane2 = (undefined1)(nodeTintArgb >> 0x10);
  mm0PackedValue0ByteLane1 = (undefined1)(nodeTintArgb >> 8);
  definitionTintByteLane1 = (undefined1)(definitionTintArgb >> 0x18);
  definitionAlphaPair = CONCAT11(definitionTintByteLane1,definitionTintByteLane1);
  definitionTintByteLane2 = (undefined1)(definitionTintArgb >> 0x10);
  definitionTintByteLane1 = (undefined1)(definitionTintArgb >> 8);
  mm0PackedValue0 =
       pmulhw(CONCAT26(nodeAlphaPair >> 4,
                       CONCAT24((ushort)(CONCAT35(CONCAT21(nodeAlphaPair,mm0PackedValue0ByteLane2),
                                                  CONCAT14(mm0PackedValue0ByteLane2,nodeTintArgb)) >> 0x20)
                                >> 4,CONCAT22(CONCAT11(mm0PackedValue0ByteLane1,
                                                       mm0PackedValue0ByteLane1) >> 4,
                                              CONCAT11((char)nodeTintArgb,(char)nodeTintArgb) >> 4))),
              CONCAT26(definitionAlphaPair >> 4,
                       CONCAT24((ushort)(CONCAT35(CONCAT21(definitionAlphaPair,definitionTintByteLane2),CONCAT14(definitionTintByteLane2,definitionTintArgb)) >>
                                        0x20) >> 4,
                                CONCAT22(CONCAT11(definitionTintByteLane1,definitionTintByteLane1) >> 4,
                                         CONCAT11((char)definitionTintArgb,(char)definitionTintArgb) >> 4))));
  tintLane0 = (short)mm0PackedValue0;
  tintLane1 = (short)((ulonglong)mm0PackedValue0 >> 0x10);
  tintLane2 = (short)((ulonglong)mm0PackedValue0 >> 0x20);
  tintLane3 = (short)((ulonglong)mm0PackedValue0 >> 0x30);
  modelNode->tintArgb =
       CONCAT13((0 < tintLane3) * (tintLane3 < 0x100) * (char)((ulonglong)mm0PackedValue0 >> 0x30) -
                (0xff < tintLane3),
                CONCAT12((0 < tintLane2) * (tintLane2 < 0x100) * (char)((ulonglong)mm0PackedValue0 >> 0x20)
                         - (0xff < tintLane2),
                         CONCAT11((0 < tintLane1) * (tintLane1 < 0x100) *
                                  (char)((ulonglong)mm0PackedValue0 >> 0x10) - (0xff < tintLane1),
                                  (0 < tintLane0) * (tintLane0 < 0x100) * (char)mm0PackedValue0 -
                                  (0xff < tintLane0))));
  return;
}


/* Address: 0x0052C1A0.
   Ownership: world/shots/maintenance.
   Purpose: Two-argument no-op callback selected from the mixed army, shot, and effect runtime dispatch table for
   the shot-runtime class. Maintenance slot 7 passes two stack arguments and the exact body returns with RET 0x08;
   the callback intentionally ignores WorldRuntimeContext and the runtime object. Maintenance table phase
   occupancyRebuild, object kind shot. The 4x3 table bytes, target body, calling convention, and RET 0x08 contract
   remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
ShotRuntimeMaintenance_OccupancyRebuildNoOp(WorldRuntimeContext *worldRuntime,void *runtimeObject)

{
  return;
}


/* Address: 0x0052C1B0.
   Ownership: world/shots/maintenance.
   Purpose: Binary entry is anchored by g_ArmyRuntimeCallbackTable12[10]@00562DEC. Maintenance table phase
   audioRefresh, object kind shot. Maintenance audioRefresh.shot callback. The second argument is the
   ModelRuntimeNode whose runtimePayload and world transform drive projected sound gain updates. The 4x3 table
   bytes, target body, calling convention, and RET 0x08 contract remain unchanged.
   Cross-module calls: TerrainGrid_TestProjectedCellMaskBits01Cf [world/terrain/grid],
   SpatialSound_UpdateDesiredPositionedGains [audio/spatial/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
ShotRuntimeMaintenance_UpdateHierarchyProjectedSound
          (WorldRuntimeContext *worldRuntime,ShotModelRuntimeNodeClassView100 *modelNode)

{
  GraphicsFixedVec3 *worldPosition;
  uint soundSlotIndex;
  SpatialSoundSlot *slot;
  bool cellMasked;
  ShotRuntimeSlot *shotRuntime;
  
  shotRuntime = modelNode->shotRuntime;
  if ((worldRuntime->dwordArray != (dword *)0x0) &&
     (soundSlotIndex = ((shotRuntime->definitionOrSavedId).definition)->terrainGridMaskIndex,
     soundSlotIndex < worldRuntime->dwordArrayCount)) {
    slot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex];
    worldPosition = &(modelNode->worldTransform).translation;
    if (slot != (SpatialSoundSlot *)0x0) {
      cellMasked = TerrainGrid_TestProjectedCellMaskBits01Cf
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
   Ownership: world/shots/maintenance.
   Purpose: Maintenance slot 1 receives WorldRuntimeContext and ModelRuntimeNode and advances the linked shot
   runtime payload through timers, animation/model state, collision and effect transitions. The exact fifteen-range
   body and five RET 0x08 exits are sealed against the immutable original binary. The slot-based name remains
   conservative. Advances projectile animation and motion, resolves terrain, secondary-surface and runtime
   collisions, emits impact effects, applies army-hit relation logic and damage, and unlinks completed projectile
   nodes. Maintenance table phase primaryUpdate, object kind shot.
   Cross-module calls: ModelLookupTable_ContainsPackedKeyCf [assets/model/definitions],
   ModelNodeRuntime_TransformLocalPointRegs [world/model/hierarchy],
   EffectRuntimePool_CreateInstanceFromDefinitionCf [world/effects/runtime],
   ModelRuntime_RaycastCandidateListNearestCf [world/model/runtime], FieldGrid_RaycastTerrainSurfaceDistanceCf
   [world/terrain/grid], FieldGrid_RaycastSecondarySurfaceDistanceCf [world/terrain/grid].
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
  uint frameAccumulatorOrDistance;
  uint frameCountDistanceOrAge;
  dword mixedScalarOrPointerCarrier;
  int workingValue;
  int targetDeltaX;
  ModelRuntimeNode *ownerModelNode;
  uint terrainHitDistance;
  uint secondaryHitDistance;
  Q12 worldXQ12;
  Q12 trajectoryStepYQ12;
  AngleTurn16Stored32 headingTurnDeltaAngle16;
  AngleTurn32 ballisticAzimuthAngle;
  ModelRaycastNearestNodeOrScratch4 nearestArmyHit;
  dword terrainMaterialIndex;
  AngleTurn16Stored32 elevationTurnDeltaAngle16;
  ShotDefinition *shotDefinition;
  FactionRuntimeIndex ownerFactionIndex;
  ShotRuntimeSlot *shotRuntime;
  ModelRuntimeNode *modelNodeRuntime;
  ModelLookupEntryEaxCf5 emitterLookup;
  ModelRaycastNearestHitEaxEdxCf9 armyRaycast;
  FieldGridRaycastEaxEdxCf9 surfaceRaycast;
  FixedMathVectorAnglesRegs8 targetAngles;
  ModelLocalPointRegs12 emitterWorldPoint;
  FixedLengthAnglesEaxEcxEdx12 ballisticAngles;
  FixedDirectionXyzRegs12 directionOffset;
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
    frameAccumulatorOrDistance = shotRuntime->animationFrameAccumulatorQ4 + 0x10;
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
    if (*lifetimeTicksPtr == 0)
    goto 
    ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects_UnlinkExpiredOrOrphanedProjectileAndReturn
    ;
    secondaryCountdownPtr = &(shotRuntime->ownerAndTrajectory).secondaryEffectCountdownTicks;
    *secondaryCountdownPtr = *secondaryCountdownPtr - 1;
    if (*secondaryCountdownPtr == 0) {
      (shotRuntime->ownerAndTrajectory).secondaryEffectCountdownTicks =
           shotDefinition->secondaryEffectIntervalTicks;
      emitterLookup = ModelLookupTable_ContainsPackedKeyCf(1,3,shotDefinition->ownedNestedResource);
      if (!emitterLookup.carry) {
        effectDefinition = shotDefinition->secondaryEffectDefinition;
        effectWorldRuntime = worldRuntime;
        emitterWorldPoint = ModelNodeRuntime_TransformLocalPointRegs
                           (emitterLookup.entry,(ModelRuntimeNode *)modelNode);
        worldXQ12 = emitterWorldPoint.ecx;
        EffectRuntimePool_CreateInstanceFromDefinitionCf
                  (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference4, 0x0),0,0x4000,0,
                   emitterWorldPoint.edx,worldXQ12,emitterWorldPoint.eax,effectDefinition,effectWorldRuntime);
      }
    }
    if (shotDefinition->trajectoryMode == SHOT_TRAJECTORY_DIRECT_LINE) {
      ownerOrHitArmy = (shotRuntime->ownerAndTrajectory).ownerArmyRuntime;
      frameCountDistanceOrAge = shotDefinition->projectileLifetimeTicks * shotDefinition->launchSpeedQ12;
      rotationAngle = (modelNode->modelPayload).worldRotationAngle1;
      modelNode->renderDepthBiasOrState = frameCountDistanceOrAge;
      ownerModelNode = (ModelRuntimeNode *)0x0;
      if (ownerOrHitArmy != (ArmyRuntimeSlot *)0x0) {
        ownerModelNode = ownerOrHitArmy->modelNodeRuntime;
      }
      armyRaycast = ModelRuntime_RaycastCandidateListNearestCf
                         (rotationAngle - shotRuntime->elevationOffsetAngle16,
                          (modelNode->modelPayload).worldRotationAngle0,frameCountDistanceOrAge,
                          (modelNode->worldTransform).translation.z,
                          (modelNode->worldTransform).translation.y,
                          (modelNode->worldTransform).translation.x,WORLD_OWNER_RUNTIME_MODEL,
                          ownerModelNode,worldRuntime);
      nearestArmyHit = armyRaycast.edxCarrier;
      frameAccumulatorOrDistance = armyRaycast.nearestDistanceQ12;
      if ((armyRaycast.carry) &&
         (targetClassIndex = *(int *)(((((nearestArmyHit.nearestModelNode)->runtimePayload).modelRuntime)->
                               definitionOrSavedId).savedIdOrOffset + 0x5c),
         ((shotRuntime->definitionOrSavedId).definition)->targetClassImpactEffectDefinitions8
         [targetClassIndex] == (EffectDefinition *)0x0)) {
        frameAccumulatorOrDistance = 0x7fffffff;
      }
      surfaceRaycast = FieldGrid_RaycastTerrainSurfaceDistanceCf
                         ((modelNode->modelPayload).worldRotationAngle1 -
                          shotRuntime->elevationOffsetAngle16,
                          (modelNode->modelPayload).worldRotationAngle0,frameCountDistanceOrAge,
                          (modelNode->worldTransform).translation.z,
                          (modelNode->worldTransform).translation.y,
                          (modelNode->worldTransform).translation.x,worldRuntime->fieldGrid);
      terrainMaterialIndex = surfaceRaycast.materialOrCellIndex;
      terrainHitDistance = surfaceRaycast.distanceQ12;
      if ((surfaceRaycast.carry) &&
         (((shotRuntime->definitionOrSavedId).definition)->terrainImpactEffectDefinitions31[terrainMaterialIndex]
          == (EffectDefinition *)0x0)) {
        terrainHitDistance = 0x7fffffff;
      }
      surfaceRaycast = FieldGrid_RaycastSecondarySurfaceDistanceCf
                         ((modelNode->modelPayload).worldRotationAngle1 -
                          shotRuntime->elevationOffsetAngle16,
                          (modelNode->modelPayload).worldRotationAngle0,frameCountDistanceOrAge,
                          (modelNode->worldTransform).translation.z,
                          (modelNode->worldTransform).translation.y,
                          (modelNode->worldTransform).translation.x,worldRuntime->fieldGrid);
      secondaryHitDistance = surfaceRaycast.distanceQ12;
      if ((surfaceRaycast.carry) &&
         (((shotRuntime->definitionOrSavedId).definition)->primaryEffectDefinition ==
          (EffectDefinition *)0x0)) {
        secondaryHitDistance = 0x7fffffff;
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
            EffectRuntimePool_CreateInstanceFromDefinitionCf
                      (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference4, 0x0),0,0x4000,0,
                       directionOffset.edx + (modelNode->worldTransform).translation.z,
                       directionOffset.ecx + (modelNode->worldTransform).translation.y,
                       directionOffset.eax + (modelNode->worldTransform).translation.x,effectDefinition,effectWorldRuntime);
          }
        }
        else {

          ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects_HandleNearestArmyHitAndContinueMotion
          :
          shotDefinition = (shotRuntime->definitionOrSavedId).definition;
          if (frameAccumulatorOrDistance <= frameCountDistanceOrAge) {
            modelNode->renderDepthBiasOrState = frameAccumulatorOrDistance;
            ShotRuntime_ApplyArmyHitRelationAndNotifications
                      (((nearestArmyHit.nearestModelNode)->runtimePayload).armyRuntime,shotRuntime);
            ownerOrHitArmy = ((nearestArmyHit.nearestModelNode)->runtimePayload).armyRuntime;
            ownerArmy = (shotRuntime->ownerAndTrajectory).ownerArmyRuntime;
            ownerFactionIndex = 0;
            if (ownerArmy != (ArmyRuntimeSlot *)0x0) {
              ownerFactionIndex = ownerArmy->factionIndex;
            }
            LOCK();
            UNLOCK();
            rotationAngle = (modelNode->modelPayload).worldRotationAngle0;
            workingValue = shotDefinition->targetClassImpactDamageQ12[targetClassIndex] /
                     (int)shotDefinition->projectileLifetimeTicks;
            LOCK();
            UNLOCK();
            effectDefinition = shotDefinition->targetClassImpactEffectDefinitions8[targetClassIndex];
            if (((shotRuntime->impactEffectEmissionFlags & 1) == 0) &&
               (effectDefinition != (EffectDefinition *)0x0)) {
              shotRuntime->impactEffectEmissionFlags = shotRuntime->impactEffectEmissionFlags | 1;
              effectWorldRuntime = worldRuntime;
              directionOffset = FixedMath_DirectionFromAnglesScaledRegs
                                 ((modelNode->modelPayload).worldRotationAngle1,
                                  (modelNode->modelPayload).worldRotationAngle0,frameAccumulatorOrDistance);
              EffectRuntimePool_CreateInstanceFromDefinitionCf
                        (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference4, 0x0),0,
                         -(modelNode->modelPayload).worldRotationAngle1,
                         (modelNode->modelPayload).worldRotationAngle0 + 0x8000 & 0xffff,
                         directionOffset.edx + (modelNode->worldTransform).translation.z,
                         directionOffset.ecx + (modelNode->worldTransform).translation.y,
                         directionOffset.eax + (modelNode->worldTransform).translation.x,effectDefinition,effectWorldRuntime);
            }
            ArmyRuntime_ApplyImpactDamageToRuntimeAndParent(rotationAngle,ownerFactionIndex,workingValue,ownerOrHitArmy);
          }
        }
      }
      else {
        if (frameAccumulatorOrDistance <= terrainHitDistance)
        goto 
        ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects_HandleNearestArmyHitAndContinueMotion
        ;
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
          EffectRuntimePool_CreateInstanceFromDefinitionCf
                    (EFFECT_RUNTIME_COMPLETION_INVOKE_LINKED_HANDLER,
                     THANDOR_BITCAST(ModelRuntimeNode *, EffectRuntimeOwnerReference4, (ModelRuntimeNode *)(shotDefinition->terrainImpactEffectOwnerSlots31 + terrainMaterialIndex)),0,
                     0x4000,0,directionOffset.edx + (modelNode->worldTransform).translation.z,
                     directionOffset.ecx + (modelNode->worldTransform).translation.y,
                     directionOffset.eax + (modelNode->worldTransform).translation.x,effectDefinition,effectWorldRuntime);
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
      ownerModelNode = (ModelRuntimeNode *)0x0;
      if (ownerOrHitArmy != (ArmyRuntimeSlot *)0x0) {
        ownerModelNode = ownerOrHitArmy->modelNodeRuntime;
      }
      armyRaycast = ModelRuntime_RaycastCandidateListNearestCf
                         ((modelNode->modelPayload).worldRotationAngle1 -
                          shotRuntime->elevationOffsetAngle16,
                          (modelNode->modelPayload).worldRotationAngle0,shotRuntime->launchSpeedQ12,
                          (modelNode->worldTransform).translation.z,
                          (modelNode->worldTransform).translation.y,
                          (modelNode->worldTransform).translation.x,WORLD_OWNER_RUNTIME_MODEL,
                          ownerModelNode,worldRuntime);
      nearestArmyHit = armyRaycast.edxCarrier;
      frameCountDistanceOrAge = armyRaycast.nearestDistanceQ12;
      if ((armyRaycast.carry) &&
         (targetClassIndex = *(int *)(((((nearestArmyHit.nearestModelNode)->runtimePayload).modelRuntime)->
                               definitionOrSavedId).savedIdOrOffset + 0x5c),
         ((shotRuntime->definitionOrSavedId).definition)->targetClassImpactEffectDefinitions8
         [targetClassIndex] == (EffectDefinition *)0x0)) {
        frameCountDistanceOrAge = 0x7fffffff;
      }
      surfaceRaycast = FieldGrid_RaycastTerrainSurfaceDistanceCf
                         ((modelNode->modelPayload).worldRotationAngle1 -
                          shotRuntime->elevationOffsetAngle16,
                          (modelNode->modelPayload).worldRotationAngle0,shotRuntime->launchSpeedQ12,
                          (modelNode->worldTransform).translation.z,
                          (modelNode->worldTransform).translation.y,
                          (modelNode->worldTransform).translation.x,worldRuntime->fieldGrid);
      terrainMaterialIndex = surfaceRaycast.materialOrCellIndex;
      mixedScalarOrPointerCarrier = surfaceRaycast.distanceQ12;
      if ((surfaceRaycast.carry) &&
         (((shotRuntime->definitionOrSavedId).definition)->terrainImpactEffectDefinitions31[terrainMaterialIndex]
          == (EffectDefinition *)0x0)) {
        mixedScalarOrPointerCarrier = 0x7fffffff;
      }
      surfaceRaycast = FieldGrid_RaycastSecondarySurfaceDistanceCf
                         ((modelNode->modelPayload).worldRotationAngle1 -
                          shotRuntime->elevationOffsetAngle16,
                          (modelNode->modelPayload).worldRotationAngle0,shotRuntime->launchSpeedQ12,
                          (modelNode->worldTransform).translation.z,
                          (modelNode->worldTransform).translation.y,
                          (modelNode->worldTransform).translation.x,worldRuntime->fieldGrid);
      frameAccumulatorOrDistance = surfaceRaycast.distanceQ12;
      if ((surfaceRaycast.carry) &&
         (((shotRuntime->definitionOrSavedId).definition)->primaryEffectDefinition ==
          (EffectDefinition *)0x0)) {
        frameAccumulatorOrDistance = 0x7fffffff;
      }
      if (frameAccumulatorOrDistance < mixedScalarOrPointerCarrier) {
        if (frameAccumulatorOrDistance < frameCountDistanceOrAge) {
          shotDefinition = (shotRuntime->definitionOrSavedId).definition;
          if (frameAccumulatorOrDistance <= (uint)shotRuntime->launchSpeedQ12) {
            InterpolationState_SetNegatedTargetAndRescaleProgress
                      (shotDefinition->shadingReleaseTransitionDurationTicks,modelNode->shadingRecord);
            effectDefinition = shotDefinition->primaryEffectDefinition;
            directionOffset = FixedMath_DirectionFromAnglesScaledRegs
                               ((modelNode->modelPayload).worldRotationAngle1,
                                (modelNode->modelPayload).worldRotationAngle0,frameAccumulatorOrDistance);
            EffectRuntimePool_CreateInstanceFromDefinitionCf
                      (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference4, 0x0),0,0x4000,0,
                       directionOffset.edx + (modelNode->worldTransform).translation.z,
                       directionOffset.ecx + (modelNode->worldTransform).translation.y,
                       directionOffset.eax + (modelNode->worldTransform).translation.x,effectDefinition,worldRuntime);
            WorldRuntime_UnlinkNodeFromOwnerListD8((WorldOwnerListNode100 *)modelNode);
            (shotRuntime->modelNodeOrSavedOffset).modelNode = (ModelRuntimeNode *)0x0;
            return;
          }
        }
        else {

          ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects_HandleNearestArmyHitAndTerminateProjectile
          :
          shotDefinition = (shotRuntime->definitionOrSavedId).definition;
          if (frameCountDistanceOrAge <= (uint)shotRuntime->launchSpeedQ12) {
            ShotRuntime_ApplyArmyHitRelationAndNotifications
                      (((nearestArmyHit.nearestModelNode)->runtimePayload).armyRuntime,shotRuntime);
            InterpolationState_SetNegatedTargetAndRescaleProgress
                      (shotDefinition->shadingReleaseTransitionDurationTicks,modelNode->shadingRecord);
            ownerOrHitArmy = ((nearestArmyHit.nearestModelNode)->runtimePayload).armyRuntime;
            impactValue = shotDefinition->targetClassImpactDamageQ12[targetClassIndex];
            ownerArmy = (shotRuntime->ownerAndTrajectory).ownerArmyRuntime;
            ownerFactionIndex = 0;
            if (ownerArmy != (ArmyRuntimeSlot *)0x0) {
              ownerFactionIndex = ownerArmy->factionIndex;
            }
            LOCK();
            UNLOCK();
            effectDefinition = shotDefinition->targetClassImpactEffectDefinitions8[targetClassIndex];
            rotationAngle = (modelNode->modelPayload).worldRotationAngle0;
            if (effectDefinition != (EffectDefinition *)0x0) {
              directionOffset = FixedMath_DirectionFromAnglesScaledRegs
                                 ((modelNode->modelPayload).worldRotationAngle1,
                                  (modelNode->modelPayload).worldRotationAngle0,frameCountDistanceOrAge);
              EffectRuntimePool_CreateInstanceFromDefinitionCf
                        (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference4, 0x0),0,
                         -(modelNode->modelPayload).worldRotationAngle1,
                         (modelNode->modelPayload).worldRotationAngle0 + 0x8000 & 0xffff,
                         directionOffset.edx + (modelNode->worldTransform).translation.z,
                         directionOffset.ecx + (modelNode->worldTransform).translation.y,
                         directionOffset.eax + (modelNode->worldTransform).translation.x,effectDefinition,worldRuntime
                        );
            }
            WorldRuntime_UnlinkNodeFromOwnerListD8((WorldOwnerListNode100 *)modelNode);
            (shotRuntime->modelNodeOrSavedOffset).modelNode = (ModelRuntimeNode *)0x0;
            ArmyRuntime_ApplyImpactDamageToRuntimeAndParent(rotationAngle,ownerFactionIndex,impactValue,ownerOrHitArmy);
            return;
          }
        }
      }
      else {
        if (frameCountDistanceOrAge <= mixedScalarOrPointerCarrier)
        goto 
        ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects_HandleNearestArmyHitAndTerminateProjectile
        ;
        shotDefinition = (shotRuntime->definitionOrSavedId).definition;
        if (mixedScalarOrPointerCarrier <= (uint)shotRuntime->launchSpeedQ12) {
          (shotRuntime->modelNodeOrSavedOffset).modelNode = (ModelRuntimeNode *)0x0;
          InterpolationState_SetNegatedTargetAndRescaleProgress
                    (shotDefinition->shadingReleaseTransitionDurationTicks,modelNode->shadingRecord);
          effectDefinition = shotDefinition->terrainImpactEffectDefinitions31[terrainMaterialIndex];
          directionOffset = FixedMath_DirectionFromAnglesScaledRegs
                             ((modelNode->modelPayload).worldRotationAngle1,
                              (modelNode->modelPayload).worldRotationAngle0,
                              mixedScalarOrPointerCarrier);
          EffectRuntimePool_CreateInstanceFromDefinitionCf
                    (EFFECT_RUNTIME_COMPLETION_INVOKE_LINKED_HANDLER,
                     THANDOR_BITCAST(ModelRuntimeNode *, EffectRuntimeOwnerReference4, (ModelRuntimeNode *)(shotDefinition->terrainImpactEffectOwnerSlots31 + terrainMaterialIndex)),0,
                     0x4000,0,directionOffset.edx + (modelNode->worldTransform).translation.z,
                     directionOffset.ecx + (modelNode->worldTransform).translation.y,
                     directionOffset.eax + (modelNode->worldTransform).translation.x,effectDefinition,worldRuntime);
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
        trajectoryStepYQ12 = directionOffset.ecx;
        nodeShadingRecord = modelNode->shadingRecord;
        translationPtr = &(modelNode->worldTransform).translation;
        translationPtr->x = translationPtr->x + directionOffset.eax;
        if (nodeShadingRecord != (GraphicsShadingRuntimeRecord *)0x0) {
          nodeShadingRecord->worldXQ12 = nodeShadingRecord->worldXQ12 + directionOffset.eax;
          nodeShadingRecord->worldYQ12 = nodeShadingRecord->worldYQ12 + trajectoryStepYQ12;
          nodeShadingRecord->worldZQ12 = nodeShadingRecord->worldZQ12 + directionOffset.edx;
        }
        translationAxisPtr = &(modelNode->worldTransform).translation.y;
        *translationAxisPtr = *translationAxisPtr + trajectoryStepYQ12;
        shotDefinition = (shotRuntime->definitionOrSavedId).definition;
        translationAxisPtr = &(modelNode->worldTransform).translation.z;
        *translationAxisPtr = *translationAxisPtr + directionOffset.edx;
        targetStateRecord = (shotRuntime->runtimeStateOrSavedOffset).runtimeStatePointer;
        if ((shotDefinition->guidanceTurnLimitAngle16 != 0) && (targetStateRecord != (int *)0x0)) {
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
               targetAngles.edx - (modelNodeRuntime->modelPayload).worldRotationAngle1;
          workingValue = shotDefinition->guidanceTurnLimitAngle16;
          headingTurnDeltaAngle16 =
               (int)((targetAngles.ecx - (modelNodeRuntime->modelPayload).worldRotationAngle0) * 0x10000)
               >> 0x10;
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
                 (Q12)(((longlong)
                        (int)(((longlong)(int)frameCountDistanceOrAge * (longlong)(int)frameCountDistanceOrAge) /
                             (longlong)(int)shotDefinition->trajectoryRampDurationTicks) *
                       (longlong)shotDefinition->launchSpeedQ12) /
                      (longlong)(int)shotDefinition->trajectoryRampDurationTicks);
            if (shotDefinition->elevationOffsetAngle16 != 0) {
              frameCountDistanceOrAge = shotDefinition->trajectoryRampDurationTicks * 2;
              workingValue = frameCountDistanceOrAge - shotRuntime->projectileAgeTicks;
              if (frameCountDistanceOrAge < shotRuntime->projectileAgeTicks) {
                workingValue = 0;
              }
              workingValue = ((int)(((longlong)workingValue * (longlong)shotDefinition->elevationOffsetAngle16) /
                             (longlong)(int)shotDefinition->trajectoryRampDurationTicks) >> 1) -
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
              if (nodeShadingRecord != (GraphicsShadingRuntimeRecord *)0x0) {
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
                   (Q12)(((longlong)
                          (int)(((longlong)
                                 (int)(((longlong)(int)frameCountDistanceOrAge * (longlong)(int)frameCountDistanceOrAge) /
                                      (longlong)(int)shotDefinition->trajectoryRampDurationTicks) *
                                (longlong)(int)frameCountDistanceOrAge) /
                               (longlong)(int)shotDefinition->trajectoryRampDurationTicks) *
                         (longlong)shotDefinition->launchSpeedQ12) /
                        (longlong)(int)shotDefinition->trajectoryRampDurationTicks);
            }
            if (shotDefinition->fixedRangeTransitionAgeThresholdTicks <= shotRuntime->projectileAgeTicks) {
              shotDefinition = (ShotDefinition *)(shotRuntime->definitionOrSavedId).savedId;
              if ((shotRuntime->runtimeStateOrSavedOffset).runtimeState == 0) {

                ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects_UnlinkExpiredOrOrphanedProjectileAndReturn
                :
                InterpolationState_SetNegatedTargetAndRescaleProgress
                          (shotDefinition->shadingReleaseTransitionDurationTicks,
                           modelNodeRuntime->shadingRecord);
                WorldRuntime_UnlinkNodeFromOwnerListD8((WorldOwnerListNode100 *)modelNodeRuntime);
                (shotRuntime->modelNodeOrSavedOffset).modelNode = (ModelRuntimeNode *)0x0;
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
              if (nodeShadingRecord != (GraphicsShadingRuntimeRecord *)0x0) {
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

