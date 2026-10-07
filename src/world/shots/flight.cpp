/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/shots/flight.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/shots/flight.h>
#include <thandor/thandor.h>
#include <thandor/core/color_lanes.h>
#include <thandor/platform/bootstrap/image.h>

/* g_RuntimeMaintenanceCallbackPhases.terrainStateRefresh.shot: works out which factions are around the shot
   (for a direct-line shot also at the middle and the end of its beam), turns that into the
   TERRAIN_OCCUPANCY_FLAG_* visibility flags for the active faction and refreshes the state tint, then
   modulates the node tint with the shot definition's tint.
*/
void ShotModelRuntimeMaintenance_RefreshTerrainClassAndTint
          (WorldRuntimeContext *worldRuntime,ShotModelRuntimeNode *modelNode)

{
  PackedArgb32 nodeTintArgb;
  PackedArgb32 definitionTintArgb;
  FieldGridRegionMask occupancyMask;
  uint32_t beamMiddleOccupancyMask;
  uint32_t beamEndOccupancyMask;
  uint64_t tintProductWords;
  FixedDirection beamMiddleOffset;
  FixedDirection beamEndOffset;
  TerrainOccupancyResolvedMasks resolvedMasks;
  ShotRuntimeSlot *shotRuntime;

  shotRuntime = modelNode->shotRuntime;
  occupancyMask =
       TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                 (Q12_ONE,modelNode->worldTransform.translation.y,
                  modelNode->worldTransform.translation.x,worldRuntime->fieldGrid);
  if ((shotRuntime->definitionOrSavedId.definition->trajectoryMode ==
       SHOT_TRAJECTORY_DIRECT_LINE) && (modelNode->renderDepthBiasOrState != 0)) {
    /* renderDepthBiasOrState holds the beam length (see the primaryUpdate callback) */
    beamMiddleOffset = FixedMath_DirectionFromAnglesScaled
                            (modelNode->modelPayload.worldRotationAngle1,
                             modelNode->modelPayload.worldRotationAngle0,
                             modelNode->renderDepthBiasOrState >> 1);
    beamMiddleOccupancyMask = TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                      (Q12_ONE,beamMiddleOffset.y + modelNode->worldTransform.translation.y,
                       beamMiddleOffset.x + modelNode->worldTransform.translation.x,
                       worldRuntime->fieldGrid);
    beamEndOffset = FixedMath_DirectionFromAnglesScaled
                         (modelNode->modelPayload.worldRotationAngle1,
                          modelNode->modelPayload.worldRotationAngle0,
                          modelNode->renderDepthBiasOrState);
    beamEndOccupancyMask = TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                      (Q12_ONE,beamEndOffset.y + modelNode->worldTransform.translation.y,
                       beamEndOffset.x + modelNode->worldTransform.translation.x,
                       worldRuntime->fieldGrid);
    occupancyMask = occupancyMask | beamMiddleOccupancyMask | beamEndOccupancyMask;
  }
  modelNode->runtimeFlags =
       modelNode->runtimeFlags & ~(TERRAIN_OCCUPANCY_FLAG_PRESENT | TERRAIN_OCCUPANCY_FLAG_SEEN_BEFORE);
  resolvedMasks = TerrainOccupancyMask_ResolveRuntimeClassFlags
                     (modelNode->runtimeFlags,0,occupancyMask,
                      (char)worldRuntime->activeFactionRuntimeIndex);
  modelNode->runtimeFlags = modelNode->runtimeFlags | resolvedMasks.runtimeFlags;
  shotRuntime->terrainRuntimeClassState = resolvedMasks.primaryOccupancyMask;
  ModelNodeRuntime_RefreshStateTint(WorldNode_View<ModelRuntimeNode>(modelNode));
  nodeTintArgb = modelNode->tintArgb;
  definitionTintArgb = shotRuntime->definitionOrSavedId.definition->stateTintArgb;
  /* per channel (a * 0x101 >> 4) * (b * 0x101 >> 4) >> 16, about a * b / 256 (MMX in the original) */
  tintProductWords =
       pmulhw(ColorLanes_UnpackBytesShiftRight(nodeTintArgb,4),
              ColorLanes_UnpackBytesShiftRight(definitionTintArgb,4));
  modelNode->tintArgb = ColorLanes_PackWordsUnsignedSaturate(tintProductWords);
}


/* g_RuntimeMaintenanceCallbackPhases.occupancyRebuild.shot: shots do not contribute to the terrain occupancy,
   so this callback does nothing (it still pops its two stack arguments).
*/
void ShotRuntimeMaintenance_OccupancyRebuildNoOp(WorldRuntimeContext *worldRuntime,void *runtimeObject)

{
}


/* g_RuntimeMaintenanceCallbackPhases.audioRefresh.shot: feeds the shot's position to the positioned sound slot
   its definition selects (soundSlotIndex indexes the world's sound slot table, not a terrain mask),
   unless the shot is over a cell hidden by TerrainGrid_TestProjectedCellMaskBits01.
*/
void ShotRuntimeMaintenance_UpdateHierarchyProjectedSound
          (WorldRuntimeContext *worldRuntime,ShotModelRuntimeNode *modelNode)

{
  GraphicsFixedVec3 *worldPosition;
  uint32_t soundSlotIndex;
  SpatialSoundSlot *slot;
  ShotRuntimeSlot *shotRuntime;

  shotRuntime = modelNode->shotRuntime;
  if (worldRuntime->dwordArray == nullptr) {
    return;
  }
  soundSlotIndex = shotRuntime->definitionOrSavedId.definition->soundSlotIndex;
  if (soundSlotIndex >= worldRuntime->dwordArrayCount) {
    return;
  }
  slot = reinterpret_cast<SpatialSoundSlot *>(worldRuntime->dwordArray[soundSlotIndex]); /* the workspace keeps pointers as uintptr_t */
  worldPosition = &modelNode->worldTransform.translation;
  if (slot == nullptr) {
    return;
  }
  if (TerrainGrid_TestProjectedCellMaskBits01(worldPosition->y,worldPosition->x,worldRuntime)) {
    return;
  }
  SpatialSound_UpdateDesiredPositionedGains
            (shotRuntime->definitionOrSavedId.definition->positionedSoundMaximumDistanceQ12,
             shotRuntime->definitionOrSavedId.definition->positionedSoundGainQ15,worldPosition,slot);
}


/* Results of the three rays a shot casts each tick (ShotModel_CastHitRays). The caller keeps one instance over
   all ticks: targetClassIndex is only refreshed on an army hit, like the original's function-wide local. */
struct ShotRayHits {
  ModelRaycastNearestNodeOrScratch4 nearestArmyHit;
  int targetClassIndex;
  uint32_t armyHitDistance;      /* MODEL_RAYCAST_NO_HIT_DISTANCE also for a target class without impact effect */
  uint32_t terrainHitDistance;   /* FIELD_GRID_RAYCAST_MISS_DISTANCE also for a material without impact effect */
  uint32_t terrainMaterialIndex;
  uint32_t secondaryHitDistance; /* FIELD_GRID_RAYCAST_MISS_DISTANCE also when there is no primary effect */
};

/* One tick of age and sprite animation: the frame advances whenever the Q4 accumulator reaches the threshold
   and wraps after animationFrameCount frames. */
static void ShotModel_AgeAndAdvanceAnimation
          (ShotModelRuntimeNode *modelNode,ShotRuntimeSlot *shotRuntime,ShotDefinition *shotDefinition)

{
  uint32_t frameAccumulatorQ4;
  uint32_t frameCount;
  ShotFrameAdvanceThresholdQ4 frameAdvanceThreshold;

  shotRuntime->projectileAgeTicks = shotRuntime->projectileAgeTicks + 1;
  frameAccumulatorQ4 = shotRuntime->animationFrameAccumulatorQ4 + (1 << Q4_SHIFT);
  frameCount = shotDefinition->animationFrameCount;
  shotRuntime->animationFrameAccumulatorQ4 = frameAccumulatorQ4;
  frameAdvanceThreshold = shotDefinition->animationFrameAdvanceThresholdQ4;
  if (frameAdvanceThreshold <= frameAccumulatorQ4) {
    shotRuntime->ownerAndTrajectory.animationFrameIndex += 1;
    modelNode->textureSubresourceBaseIndex = modelNode->textureSubresourceBaseIndex + 1;
    shotRuntime->animationFrameAccumulatorQ4 = frameAccumulatorQ4 - frameAdvanceThreshold;
    if (frameCount <= shotRuntime->ownerAndTrajectory.animationFrameIndex) {
      shotRuntime->ownerAndTrajectory.animationFrameIndex -= frameCount;
      modelNode->textureSubresourceBaseIndex = modelNode->textureSubresourceBaseIndex - frameCount;
    }
  }
}

/* Fades the shot's shading out and removes its model node from the world (expired or orphaned shot). */
static void ShotModel_ReleaseAndUnlink
          (ShotRuntimeSlot *shotRuntime,ShotDefinition *shotDefinition,ModelRuntimeNode *modelNodeRuntime)

{
  InterpolationState_SetNegatedTargetAndRescaleProgress
            (shotDefinition->shadingReleaseTransitionDurationTicks,modelNodeRuntime->shadingRecord);
  WorldRuntime_UnlinkOwnerListNode(WorldNode_View<WorldOwnerListNode>(modelNodeRuntime));
  shotRuntime->modelNodeOrSavedOffset.modelNode = nullptr;
}

/* Counts the secondary effect interval down; when it runs out, reloads it and emits the secondary (trail) effect
   at the sprite's effect point, if it has one. */
static void ShotModel_EmitSecondaryTrailEffect
          (WorldRuntimeContext *worldRuntime,ShotModelRuntimeNode *modelNode,ShotRuntimeSlot *shotRuntime,
           ShotDefinition *shotDefinition)

{
  ModelPackedPointRecord *emitterRecord;
  ModelWorldPoint emitterWorldPoint;

  shotRuntime->ownerAndTrajectory.secondaryEffectCountdownTicks--;
  if (shotRuntime->ownerAndTrajectory.secondaryEffectCountdownTicks != 0) {
    return;
  }
  shotRuntime->ownerAndTrajectory.secondaryEffectCountdownTicks = shotDefinition->secondaryEffectIntervalTicks;
  if (ModelLookupTable_FindPackedPoint
        (1,MODEL_POINT_CLASS_EFFECT,static_cast<ModelResource *>(shotDefinition->ownedNestedResource.get()),&emitterRecord)) {
    emitterWorldPoint = ModelNodeRuntime_TransformLocalPoint(emitterRecord,WorldNode_View<ModelRuntimeNode>(modelNode));
    EffectRuntimePool_CreateInstanceFromDefinition
              (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_COMPOUND(EffectRuntimeOwnerReference){ .modelNode = nullptr },0,
               FIXED_ANGLE16_QUARTER_TURN,0,
               emitterWorldPoint.zQ12,emitterWorldPoint.yQ12,emitterWorldPoint.xQ12,
               shotDefinition->secondaryEffectDefinition,worldRuntime);
  }
}

/* Creates an effect at the point hitDistance along the shot's heading (worldRotationAngle1/0, without the
   elevation offset). */
static void ShotModel_EmitEffectAlongHeading
          (EffectRuntimeCompletionAction completionAction,EffectRuntimeOwnerReference ownerRuntime,
           AngleTurn32 orientationAngle1,AngleTurn32 orientationAngle2,uint32_t hitDistance,
           ShotModelRuntimeNode *modelNode,EffectDefinition *effectDefinition,WorldRuntimeContext *worldRuntime)

{
  FixedDirection hitOffset;

  hitOffset = FixedMath_DirectionFromAnglesScaled
                   (modelNode->modelPayload.worldRotationAngle1,modelNode->modelPayload.worldRotationAngle0,
                    hitDistance);
  EffectRuntimePool_CreateInstanceFromDefinition
            (completionAction,ownerRuntime,0,orientationAngle1,orientationAngle2,
             hitOffset.z + modelNode->worldTransform.translation.z,
             hitOffset.y + modelNode->worldTransform.translation.y,
             hitOffset.x + modelNode->worldTransform.translation.x,effectDefinition,worldRuntime);
}

/* Impact effect on an army: oriented against the shot's heading. */
static void ShotModel_EmitArmyImpactEffect
          (WorldRuntimeContext *worldRuntime,ShotModelRuntimeNode *modelNode,uint32_t hitDistance,
           EffectDefinition *effectDefinition)

{
  ShotModel_EmitEffectAlongHeading
            (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_COMPOUND(EffectRuntimeOwnerReference){ .modelNode = nullptr },
             -modelNode->modelPayload.worldRotationAngle1,
             modelNode->modelPayload.worldRotationAngle0 + FIXED_ANGLE16_HALF_TURN & FIXED_ANGLE16_MASK,
             hitDistance,modelNode,effectDefinition,worldRuntime);
}

/* Impact effect on the terrain: when it completes it deforms the terrain with the material's crater columns. */
static void ShotModel_EmitTerrainImpactEffect
          (WorldRuntimeContext *worldRuntime,ShotModelRuntimeNode *modelNode,ShotDefinition *shotDefinition,
           uint32_t terrainMaterialIndex,uint32_t hitDistance)

{
  ShotModel_EmitEffectAlongHeading
            (EFFECT_RUNTIME_COMPLETION_INVOKE_LINKED_HANDLER,
             THANDOR_COMPOUND(EffectRuntimeOwnerReference){
               .terrainImpactColumns = (ShotTerrainImpactDeformationColumns *)
                                       (shotDefinition->terrainImpactHeightDeltasQ12 + terrainMaterialIndex) },
             FIXED_ANGLE16_QUARTER_TURN,0,hitDistance,modelNode,
             shotDefinition->terrainImpactEffectDefinitions31[terrainMaterialIndex],worldRuntime);
}

/* Casts the shot's ray (rayLengthQ12 long) against armies (except the owner's model), the terrain surface and
   the secondary surface. Hits on target classes and materials without an impact effect do not stop the shot. */
static void ShotModel_CastHitRays
          (WorldRuntimeContext *worldRuntime,ShotModelRuntimeNode *modelNode,ShotRuntimeSlot *shotRuntime,
           Q12 rayLengthQ12,ShotRayHits *hits)

{
  ArmyRuntimeSlot *shotOwnerArmy;
  ModelRuntimeNode *ownerModelNode;
  Bool8 armyHit;
  Bool8 surfaceHit;
  Q12 armyHitDistanceQ12;
  Q12 surfaceDistanceQ12;
  ModelRuntimeNode *nearestArmyModelNode;

  shotOwnerArmy = shotRuntime->ownerAndTrajectory.ownerArmyRuntime;
  ownerModelNode = nullptr;
  if (shotOwnerArmy != nullptr) {
    ownerModelNode = shotOwnerArmy->modelNodeRuntime;
  }
  nearestArmyModelNode = hits->nearestArmyHit.nearestModelNode;
  armyHit = ModelRuntime_RaycastCandidateListNearest
                     (modelNode->modelPayload.worldRotationAngle1 - shotRuntime->elevationOffsetAngle16,
                      modelNode->modelPayload.worldRotationAngle0,rayLengthQ12,
                      modelNode->worldTransform.translation.z,
                      modelNode->worldTransform.translation.y,
                      modelNode->worldTransform.translation.x,WORLD_OWNER_RUNTIME_MODEL,
                      ownerModelNode,worldRuntime,&armyHitDistanceQ12,&nearestArmyModelNode);
  hits->nearestArmyHit.nearestModelNode = nearestArmyModelNode;
  hits->armyHitDistance = armyHitDistanceQ12;
  if (armyHit) {
    hits->targetClassIndex = (hits->nearestArmyHit.nearestModelNode->runtimePayload.modelRuntime->
                              definitionOrSavedId).runtimeDefinition->targetClassIndex;
    if (shotRuntime->definitionOrSavedId.definition->targetClassImpactEffectDefinitions8
        [hits->targetClassIndex] == nullptr) {
      hits->armyHitDistance = MODEL_RAYCAST_NO_HIT_DISTANCE;
    }
  }
  surfaceHit = FieldGrid_RaycastTerrainSurfaceDistance
                     (modelNode->modelPayload.worldRotationAngle1 - shotRuntime->elevationOffsetAngle16,
                      modelNode->modelPayload.worldRotationAngle0,rayLengthQ12,
                      modelNode->worldTransform.translation.z,
                      modelNode->worldTransform.translation.y,
                      modelNode->worldTransform.translation.x,worldRuntime->fieldGrid,
                      &surfaceDistanceQ12,&hits->terrainMaterialIndex);
  hits->terrainHitDistance = surfaceDistanceQ12;
  if ((surfaceHit) && (hits->terrainMaterialIndex >= SHOT_TERRAIN_MATERIAL_REFERENCE_COUNT)) {
    /* The original indexes the 31-entry material arrays with the cell's 8-bit material id and reads the
       following ShotDefinition fields for ids 31..255; bounded here because valid maps use ids 0..25 only:
       such a material counts as one without impact effect. */
    static Bool8 s_loggedMaterialOutOfRange = false;
    if (!s_loggedMaterialOutOfRange) {
      s_loggedMaterialOutOfRange = true;
      Thandor_Log("shot: terrain material %u out of range, no impact effect",hits->terrainMaterialIndex);
    }
    hits->terrainMaterialIndex = 0;
    hits->terrainHitDistance = FIELD_GRID_RAYCAST_MISS_DISTANCE;
  }
  else if ((surfaceHit) &&
     (shotRuntime->definitionOrSavedId.definition->terrainImpactEffectDefinitions31[hits->terrainMaterialIndex]
      == nullptr)) {
    hits->terrainHitDistance = FIELD_GRID_RAYCAST_MISS_DISTANCE;
  }
  surfaceHit = FieldGrid_RaycastSecondarySurfaceDistance
                     (modelNode->modelPayload.worldRotationAngle1 - shotRuntime->elevationOffsetAngle16,
                      modelNode->modelPayload.worldRotationAngle0,rayLengthQ12,
                      modelNode->worldTransform.translation.z,
                      modelNode->worldTransform.translation.y,
                      modelNode->worldTransform.translation.x,worldRuntime->fieldGrid,
                      &surfaceDistanceQ12);
  hits->secondaryHitDistance = surfaceDistanceQ12;
  if ((surfaceHit) && (shotRuntime->definitionOrSavedId.definition->primaryEffectDefinition == nullptr)) {
    hits->secondaryHitDistance = FIELD_GRID_RAYCAST_MISS_DISTANCE;
  }
}

/* Which of the three rays stops the shot: the secondary surface wins when it is strictly nearest, otherwise the
   army wins over the terrain on a tie (and whenever the secondary surface is nearer than the terrain). */
enum {
  SHOT_NEAREST_HIT_SECONDARY_SURFACE,
  SHOT_NEAREST_HIT_ARMY,
  SHOT_NEAREST_HIT_TERRAIN
};
using ShotNearestHitKind = int;

static ShotNearestHitKind ShotModel_SelectNearestHit(const ShotRayHits *hits)

{
  if (hits->secondaryHitDistance < hits->terrainHitDistance) {
    if (hits->secondaryHitDistance < hits->armyHitDistance) {
      return SHOT_NEAREST_HIT_SECONDARY_SURFACE;
    }
    return SHOT_NEAREST_HIT_ARMY;
  }
  if (hits->armyHitDistance <= hits->terrainHitDistance) {
    return SHOT_NEAREST_HIT_ARMY;
  }
  return SHOT_NEAREST_HIT_TERRAIN;
}

/* Beam hit on an army within its length: cut the beam there, emit the impact effect once and deal the impact
   damage spread over the beam's lifetime (every tick). */
static void ShotBeam_ApplyArmyHit
          (WorldRuntimeContext *worldRuntime,ShotModelRuntimeNode *modelNode,ShotRuntimeSlot *shotRuntime,
           ShotDefinition *shotDefinition,const ShotRayHits *hits)

{
  ModelRuntimeSlot *hitModelRuntime;
  ArmyRuntimeSlot *ownerArmy;
  FactionRuntimeIndex ownerFactionIndex;
  AngleTurn32 impactAngle;
  ImpactDamageValue32 damagePerTick;
  EffectDefinition *effectDefinition;

  modelNode->renderDepthBiasOrState = hits->armyHitDistance;
  ShotRuntime_ApplyArmyHitRelationAndNotifications
            (hits->nearestArmyHit.nearestModelNode->runtimePayload.modelRuntime,shotRuntime);
  hitModelRuntime = hits->nearestArmyHit.nearestModelNode->runtimePayload.modelRuntime;
  ownerArmy = shotRuntime->ownerAndTrajectory.ownerArmyRuntime;
  ownerFactionIndex = 0;
  if (ownerArmy != nullptr) {
    ownerFactionIndex = ownerArmy->factionIndex;
  }
  impactAngle = modelNode->modelPayload.worldRotationAngle0;
  damagePerTick = shotDefinition->targetClassImpactDamageQ12[hits->targetClassIndex] /
                  (int)shotDefinition->projectileLifetimeTicks;
  effectDefinition = shotDefinition->targetClassImpactEffectDefinitions8[hits->targetClassIndex];
  if (((shotRuntime->impactEffectEmissionFlags & SHOT_IMPACT_EFFECT_EMITTED) == 0) &&
     (effectDefinition != nullptr)) {
    shotRuntime->impactEffectEmissionFlags |= SHOT_IMPACT_EFFECT_EMITTED;
    ShotModel_EmitArmyImpactEffect(worldRuntime,modelNode,hits->armyHitDistance,effectDefinition);
  }
  ArmyRuntime_ApplyImpactDamageToRuntimeAndParent(impactAngle,ownerFactionIndex,damagePerTick,hitModelRuntime);
}

/* One tick of a direct-line shot (beam): it stays in place, reaching as far as the shot would fly in its
   lifetime; the nearest hit within that length cuts it and gets its impact effect once. Then the model spins. */
static void ShotBeam_UpdateTick
          (WorldRuntimeContext *worldRuntime,ShotModelRuntimeNode *modelNode,ShotRuntimeSlot *shotRuntime,
           ShotDefinition *shotDefinition,ShotRayHits *hits)

{
  uint32_t beamLength;
  int spinStep;

  beamLength = shotDefinition->projectileLifetimeTicks * shotDefinition->launchSpeedQ12;
  modelNode->renderDepthBiasOrState = beamLength;
  ShotModel_CastHitRays(worldRuntime,modelNode,shotRuntime,beamLength,hits);
  shotDefinition = shotRuntime->definitionOrSavedId.definition;
  switch (ShotModel_SelectNearestHit(hits)) {
  case SHOT_NEAREST_HIT_SECONDARY_SURFACE:
    if (hits->secondaryHitDistance <= beamLength) {
      modelNode->renderDepthBiasOrState = hits->secondaryHitDistance;
      if ((shotRuntime->impactEffectEmissionFlags & SHOT_IMPACT_EFFECT_EMITTED) == 0) {
        shotRuntime->impactEffectEmissionFlags |= SHOT_IMPACT_EFFECT_EMITTED;
        ShotModel_EmitEffectAlongHeading
                  (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_COMPOUND(EffectRuntimeOwnerReference){ .modelNode = nullptr },
                   FIXED_ANGLE16_QUARTER_TURN,0,hits->secondaryHitDistance,modelNode,
                   shotDefinition->primaryEffectDefinition,worldRuntime);
      }
    }
    break;
  case SHOT_NEAREST_HIT_ARMY:
    if (hits->armyHitDistance <= beamLength) {
      ShotBeam_ApplyArmyHit(worldRuntime,modelNode,shotRuntime,shotDefinition,hits);
    }
    break;
  default:
    if (hits->terrainHitDistance <= beamLength) {
      modelNode->renderDepthBiasOrState = hits->terrainHitDistance;
      if ((shotRuntime->impactEffectEmissionFlags & SHOT_IMPACT_EFFECT_EMITTED) == 0) {
        shotRuntime->impactEffectEmissionFlags |= SHOT_IMPACT_EFFECT_EMITTED;
        ShotModel_EmitTerrainImpactEffect
                  (worldRuntime,modelNode,shotDefinition,hits->terrainMaterialIndex,hits->terrainHitDistance);
        shotRuntime = modelNode->shotRuntime;
      }
    }
    break;
  }
  spinStep = shotRuntime->definitionOrSavedId.definition->modelSpinStepTurn16;
  modelNode->runtimeFlags = modelNode->runtimeFlags | 1;
  modelNode->modelPayload.worldRotationAngle2 += spinStep;
  modelNode->modelPayload.worldRotationAngle2 &= FIXED_ANGLE16_MASK;
}

/* Moving shot hit on an army within this tick's step: impact effect (if any), shot removed, full damage. */
static void ShotProjectile_ImpactArmy
          (WorldRuntimeContext *worldRuntime,ShotModelRuntimeNode *modelNode,ShotRuntimeSlot *shotRuntime,
           ShotDefinition *shotDefinition,const ShotRayHits *hits)

{
  ModelRuntimeSlot *hitModelRuntime;
  ArmyRuntimeSlot *ownerArmy;
  FactionRuntimeIndex ownerFactionIndex;
  AngleTurn32 impactAngle;
  Q12 impactValue;
  EffectDefinition *effectDefinition;

  ShotRuntime_ApplyArmyHitRelationAndNotifications
            (hits->nearestArmyHit.nearestModelNode->runtimePayload.modelRuntime,shotRuntime);
  InterpolationState_SetNegatedTargetAndRescaleProgress
            (shotDefinition->shadingReleaseTransitionDurationTicks,modelNode->shadingRecord);
  hitModelRuntime = hits->nearestArmyHit.nearestModelNode->runtimePayload.modelRuntime;
  impactValue = shotDefinition->targetClassImpactDamageQ12[hits->targetClassIndex];
  ownerArmy = shotRuntime->ownerAndTrajectory.ownerArmyRuntime;
  ownerFactionIndex = 0;
  if (ownerArmy != nullptr) {
    ownerFactionIndex = ownerArmy->factionIndex;
  }
  effectDefinition = shotDefinition->targetClassImpactEffectDefinitions8[hits->targetClassIndex];
  impactAngle = modelNode->modelPayload.worldRotationAngle0;
  if (effectDefinition != nullptr) {
    ShotModel_EmitArmyImpactEffect(worldRuntime,modelNode,hits->armyHitDistance,effectDefinition);
  }
  WorldRuntime_UnlinkOwnerListNode(WorldNode_View<WorldOwnerListNode>(modelNode));
  shotRuntime->modelNodeOrSavedOffset.modelNode = nullptr;
  ArmyRuntime_ApplyImpactDamageToRuntimeAndParent(impactAngle,ownerFactionIndex,impactValue,hitModelRuntime);
}

/* Moving shot: ends at the nearest hit within this tick's step (launchSpeedQ12), emitting the impact effect.
   Returns true when the shot was removed. */
static Bool8 ShotProjectile_ApplyNearestHit
          (WorldRuntimeContext *worldRuntime,ShotModelRuntimeNode *modelNode,ShotRuntimeSlot *shotRuntime,
           const ShotRayHits *hits)

{
  ShotDefinition *shotDefinition;

  shotDefinition = shotRuntime->definitionOrSavedId.definition;
  switch (ShotModel_SelectNearestHit(hits)) {
  case SHOT_NEAREST_HIT_SECONDARY_SURFACE:
    if (hits->secondaryHitDistance > (uint32_t)shotRuntime->launchSpeedQ12) {
      return false;
    }
    InterpolationState_SetNegatedTargetAndRescaleProgress
              (shotDefinition->shadingReleaseTransitionDurationTicks,modelNode->shadingRecord);
    ShotModel_EmitEffectAlongHeading
              (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_COMPOUND(EffectRuntimeOwnerReference){ .modelNode = nullptr },
               FIXED_ANGLE16_QUARTER_TURN,0,hits->secondaryHitDistance,modelNode,
               shotDefinition->primaryEffectDefinition,worldRuntime);
    WorldRuntime_UnlinkOwnerListNode(WorldNode_View<WorldOwnerListNode>(modelNode));
    shotRuntime->modelNodeOrSavedOffset.modelNode = nullptr;
    return true;
  case SHOT_NEAREST_HIT_ARMY:
    if (hits->armyHitDistance > (uint32_t)shotRuntime->launchSpeedQ12) {
      return false;
    }
    ShotProjectile_ImpactArmy(worldRuntime,modelNode,shotRuntime,shotDefinition,hits);
    return true;
  default:
    if (hits->terrainHitDistance > (uint32_t)shotRuntime->launchSpeedQ12) {
      return false;
    }
    shotRuntime->modelNodeOrSavedOffset.modelNode = nullptr;
    InterpolationState_SetNegatedTargetAndRescaleProgress
              (shotDefinition->shadingReleaseTransitionDurationTicks,modelNode->shadingRecord);
    ShotModel_EmitTerrainImpactEffect
              (worldRuntime,modelNode,shotDefinition,hits->terrainMaterialIndex,hits->terrainHitDistance);
    WorldRuntime_UnlinkOwnerListNode(WorldNode_View<WorldOwnerListNode>(modelNode));
    return true;
  }
}

/* Moves the shot (and its shading record) one step of launchSpeedQ12 along its heading incl. elevation offset. */
static void ShotProjectile_MoveOneStep(ShotModelRuntimeNode *modelNode,ShotRuntimeSlot *shotRuntime)

{
  FixedDirection stepOffset;
  GraphicsShadingRuntimeRecord *nodeShadingRecord;

  stepOffset = FixedMath_DirectionFromAnglesScaled
                    (modelNode->modelPayload.worldRotationAngle1 - shotRuntime->elevationOffsetAngle16,
                     modelNode->modelPayload.worldRotationAngle0,shotRuntime->launchSpeedQ12);
  nodeShadingRecord = modelNode->shadingRecord;
  modelNode->worldTransform.translation.x = modelNode->worldTransform.translation.x + stepOffset.x;
  if (nodeShadingRecord != nullptr) {
    nodeShadingRecord->worldXQ12 = nodeShadingRecord->worldXQ12 + stepOffset.x;
    nodeShadingRecord->worldYQ12 = nodeShadingRecord->worldYQ12 + stepOffset.y;
    nodeShadingRecord->worldZQ12 = nodeShadingRecord->worldZQ12 + stepOffset.z;
  }
  modelNode->worldTransform.translation.y = modelNode->worldTransform.translation.y + stepOffset.y;
  modelNode->worldTransform.translation.z = modelNode->worldTransform.translation.z + stepOffset.z;
}

/* Guided shot: turns shotNode towards the target's aim point (the root model node, for an aircraft its first
   child, raised by aimHeightOffsetQ12), at most guidanceTurnLimitAngle16 per tick and axis. */
static void ShotProjectile_SteerTowardsTarget
          (ShotDefinition *shotDefinition,ModelRuntimeSlot *targetModelRuntime,ModelRuntimeNode *shotNode)

{
  ModelRuntimeNode *targetNode;
  FixedVectorAngles targetAngles;
  AngleTurn16Stored32 elevationTurnDeltaAngle16;
  AngleTurn16Stored32 headingTurnDeltaAngle16;
  int maximumTurn;
  int minimumTurn;

  targetNode = targetModelRuntime->rootModelNodeOrSavedOffset.modelNode;
  if (targetModelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId ==
      MODEL_RUNTIME_CLASS_21_AIRCRAFT) {
    targetNode = targetNode->childNodes[0];
  }
  targetAngles = FixedMath_VectorToAngles
                     ((targetModelRuntime->definitionOrSavedId.runtimeDefinition->aimHeightOffsetQ12 +
                       targetNode->worldTransform.translation.z) - shotNode->worldTransform.translation.z,
                      targetNode->worldTransform.translation.y - shotNode->worldTransform.translation.y,
                      targetNode->worldTransform.translation.x - shotNode->worldTransform.translation.x);
  elevationTurnDeltaAngle16 = targetAngles.elevationAngle - shotNode->modelPayload.worldRotationAngle1;
  maximumTurn = shotDefinition->guidanceTurnLimitAngle16;
  headingTurnDeltaAngle16 =
       (int)((targetAngles.azimuthAngle - shotNode->modelPayload.worldRotationAngle0) * FIXED_ANGLE16_FULL_TURN)
       >> 16; /* wrapped to a signed 16-bit turn */
  if (maximumTurn < elevationTurnDeltaAngle16) {
    elevationTurnDeltaAngle16 = maximumTurn;
  }
  if (maximumTurn < headingTurnDeltaAngle16) {
    headingTurnDeltaAngle16 = maximumTurn;
  }
  minimumTurn = -maximumTurn;
  if (headingTurnDeltaAngle16 < minimumTurn) {
    headingTurnDeltaAngle16 = minimumTurn;
  }
  if (elevationTurnDeltaAngle16 < minimumTurn) {
    elevationTurnDeltaAngle16 = minimumTurn;
  }
  shotNode->modelPayload.worldRotationAngle1 += elevationTurnDeltaAngle16;
  shotNode->modelPayload.worldRotationAngle0 =
       headingTurnDeltaAngle16 + shotNode->modelPayload.worldRotationAngle0 & FIXED_ANGLE16_MASK;
}

/* Lead-adjusted shot: the speed ramps up quadratically over trajectoryRampDurationTicks, and the elevation
   offset decays linearly from half the definition's offset to zero over twice that time. */
static void ShotProjectile_UpdateLeadAdjustedTrajectory
          (ShotDefinition *shotDefinition,ShotRuntimeSlot *shotRuntime,ModelRuntimeNode *shotNode)

{
  uint32_t rampAgeTicks;
  uint32_t elevationWindowTicks;
  int elevationRemainingTicks;
  int elevationOffsetStep;

  if (shotDefinition->trajectoryRampDurationTicks == 0) {
    return;
  }
  rampAgeTicks = shotRuntime->projectileAgeTicks;
  if (shotDefinition->trajectoryRampDurationTicks < rampAgeTicks) {
    rampAgeTicks = shotDefinition->trajectoryRampDurationTicks;
  }
  shotRuntime->launchSpeedQ12 =
       (Q12)(((int64_t)
              (int)(((int64_t)(int)rampAgeTicks * (int64_t)(int)rampAgeTicks) /
                   (int64_t)(int)shotDefinition->trajectoryRampDurationTicks) *
             (int64_t)shotDefinition->launchSpeedQ12) /
            (int64_t)(int)shotDefinition->trajectoryRampDurationTicks);
  if (shotDefinition->elevationOffsetAngle16 != 0) {
    elevationWindowTicks = shotDefinition->trajectoryRampDurationTicks * 2;
    elevationRemainingTicks = elevationWindowTicks - shotRuntime->projectileAgeTicks;
    if (elevationWindowTicks < shotRuntime->projectileAgeTicks) {
      elevationRemainingTicks = 0;
    }
    elevationOffsetStep =
         ((int)(((int64_t)elevationRemainingTicks * (int64_t)shotDefinition->elevationOffsetAngle16) /
                (int64_t)(int)shotDefinition->trajectoryRampDurationTicks) >> 1) -
         shotRuntime->elevationOffsetAngle16;
    shotRuntime->elevationOffsetAngle16 = shotRuntime->elevationOffsetAngle16 + elevationOffsetStep;
    shotNode->modelPayload.worldRotationAngle1 += elevationOffsetStep;
  }
}

/* Ballistic shot: gravity (ballisticDivisorQ12) pulls the direction vector down; speed and angles follow it. */
static void ShotProjectile_UpdateBallisticTrajectory(ShotRuntimeSlot *shotRuntime,ModelRuntimeNode *shotNode)

{
  FixedLengthAzimuthElevation ballisticAngles;

  shotRuntime->ownerAndTrajectory.directionComponent2Q12 -=
       shotRuntime->definitionOrSavedId.definition->ballisticDivisorQ12;
  ballisticAngles = FixedMath_VectorToAnglesAndLengthVec3
                         (reinterpret_cast<GraphicsFixedVec3 *>(&shotRuntime->ownerAndTrajectory.directionComponent0Q12)) /* direction components 0..2 are consecutive */;
  shotRuntime->launchSpeedQ12 = ballisticAngles.lengthQ12;
  shotNode->modelPayload.worldRotationAngle0 = ballisticAngles.azimuthAngle;
  shotNode->modelPayload.worldRotationAngle1 = ballisticAngles.elevationAngle;
}

/* Moves shotNode (and nodeShadingRecord) horizontally onto the x/y position of the target's root model node. */
static void ShotProjectile_ShiftOverTarget
          (ModelRuntimeSlot *targetModelRuntime,ModelRuntimeNode *shotNode,
           GraphicsShadingRuntimeRecord *nodeShadingRecord)

{
  ModelRuntimeNode *targetNode;
  int targetDeltaX;
  int targetDeltaY;

  targetNode = targetModelRuntime->rootModelNodeOrSavedOffset.modelNode;
  targetDeltaX = targetNode->worldTransform.translation.x - shotNode->worldTransform.translation.x;
  targetDeltaY = targetNode->worldTransform.translation.y - shotNode->worldTransform.translation.y;
  shotNode->worldTransform.translation.x = shotNode->worldTransform.translation.x + targetDeltaX;
  shotNode->worldTransform.translation.y = shotNode->worldTransform.translation.y + targetDeltaY;
  if (nodeShadingRecord != nullptr) {
    nodeShadingRecord->worldXQ12 = nodeShadingRecord->worldXQ12 + targetDeltaX;
    nodeShadingRecord->worldYQ12 = nodeShadingRecord->worldYQ12 + targetDeltaY;
  }
}

/* Fixed-range shot: climbs with a cubic speed ramp until fixedRangeTransitionAgeThresholdTicks, then jumps over
   its target and turns downwards; while descending it stays over the target. Returns true when the shot was
   removed because it reached the threshold without a target. */
static Bool8 ShotProjectile_UpdateFixedRangeTrajectory
          (ShotDefinition *shotDefinition,ShotRuntimeSlot *shotRuntime,ModelRuntimeNode *shotNode)

{
  uint32_t rampAgeTicks;
  GraphicsShadingRuntimeRecord *nodeShadingRecord;

  if ((int)shotNode->modelPayload.worldRotationAngle1 < 0) {
    nodeShadingRecord = shotNode->shadingRecord;
    if (shotRuntime->runtimeStateOrSavedOffset.runtimeState != 0) {
      ShotProjectile_ShiftOverTarget
                (shotRuntime->runtimeStateOrSavedOffset.targetModelRuntime,shotNode,
                 nodeShadingRecord);
    }
    return false;
  }
  if (shotDefinition->trajectoryRampDurationTicks != 0) {
    rampAgeTicks = shotRuntime->projectileAgeTicks;
    if (shotDefinition->trajectoryRampDurationTicks < rampAgeTicks) {
      rampAgeTicks = shotDefinition->trajectoryRampDurationTicks;
    }
    shotRuntime->launchSpeedQ12 =
         (Q12)(((int64_t)
                (int)(((int64_t)
                       (int)(((int64_t)(int)rampAgeTicks * (int64_t)(int)rampAgeTicks) /
                            (int64_t)(int)shotDefinition->trajectoryRampDurationTicks) *
                      (int64_t)(int)rampAgeTicks) /
                     (int64_t)(int)shotDefinition->trajectoryRampDurationTicks) *
               (int64_t)shotDefinition->launchSpeedQ12) /
              (int64_t)(int)shotDefinition->trajectoryRampDurationTicks);
  }
  if (shotDefinition->fixedRangeTransitionAgeThresholdTicks <= shotRuntime->projectileAgeTicks) {
    shotDefinition = shotRuntime->definitionOrSavedId.definition;
    if (shotRuntime->runtimeStateOrSavedOffset.runtimeState == 0) {
      ShotModel_ReleaseAndUnlink(shotRuntime,shotDefinition,shotNode);
      return true;
    }
    nodeShadingRecord = shotNode->shadingRecord;
    ShotProjectile_ShiftOverTarget
              (shotRuntime->runtimeStateOrSavedOffset.targetModelRuntime,shotNode,
               nodeShadingRecord);
    shotNode->modelPayload.worldRotationAngle1 = -shotNode->modelPayload.worldRotationAngle1;
  }
  return false;
}

/* g_RuntimeMaintenanceCallbackPhases.primaryUpdate.shot: advances a shot by g_InGameSimulationStepTicks ticks.
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
  /* zero-initialised: the original's locals were garbage here; ShotModel_CastHitRays writes every field it
     reads later (targetClassIndex only on an army hit, and it is used only after one) */
  ShotRayHits hits = {};
  ShotDefinition *shotDefinition;
  ShotRuntimeSlot *shotRuntime;
  ModelRuntimeNode *modelNodeRuntime;
  ModelRuntimeSlot *targetModelRuntime;
  int spinStep;
  InGameSimulationStepBatchTicks remainingStepTicks;

  remainingStepTicks = g_InGameSimulationStepTicks;
  do {
    shotRuntime = modelNode->shotRuntime;
    shotDefinition = shotRuntime->definitionOrSavedId.definition;
    ShotModel_AgeAndAdvanceAnimation(modelNode,shotRuntime,shotDefinition);
    shotRuntime->lifetimeTicksRemaining = shotRuntime->lifetimeTicksRemaining - 1;
    modelNodeRuntime = WorldNode_View<ModelRuntimeNode>(modelNode);
    if (shotRuntime->lifetimeTicksRemaining == 0) {
      ShotModel_ReleaseAndUnlink(shotRuntime,shotDefinition,modelNodeRuntime);
      return;
    }
    ShotModel_EmitSecondaryTrailEffect(worldRuntime,modelNode,shotRuntime,shotDefinition);
    if (shotDefinition->trajectoryMode == SHOT_TRAJECTORY_DIRECT_LINE) {
      ShotBeam_UpdateTick(worldRuntime,modelNode,shotRuntime,shotDefinition,&hits);
    }
    else {
      ShotModel_CastHitRays(worldRuntime,modelNode,shotRuntime,shotRuntime->launchSpeedQ12,&hits);
      if (ShotProjectile_ApplyNearestHit(worldRuntime,modelNode,shotRuntime,&hits)) {
        return;
      }
      shotDefinition = shotRuntime->definitionOrSavedId.definition;
      ShotProjectile_MoveOneStep(modelNode,shotRuntime);
      targetModelRuntime = shotRuntime->runtimeStateOrSavedOffset.targetModelRuntime;
      if ((shotDefinition->guidanceTurnLimitAngle16 != 0) && (targetModelRuntime != nullptr)) {
        modelNodeRuntime = shotRuntime->modelNodeOrSavedOffset.modelNode;
        ShotProjectile_SteerTowardsTarget(shotDefinition,targetModelRuntime,modelNodeRuntime);
      }
      spinStep = shotDefinition->modelSpinStepTurn16;
      modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
      modelNodeRuntime->modelPayload.worldRotationAngle2 += spinStep;
      modelNodeRuntime->modelPayload.worldRotationAngle2 &= FIXED_ANGLE16_MASK;
      if (shotDefinition->trajectoryMode == SHOT_TRAJECTORY_LEAD_ADJUSTED) {
        ShotProjectile_UpdateLeadAdjustedTrajectory(shotDefinition,shotRuntime,modelNodeRuntime);
      }
      else if (shotDefinition->trajectoryMode == SHOT_TRAJECTORY_BALLISTIC) {
        ShotProjectile_UpdateBallisticTrajectory(shotRuntime,modelNodeRuntime);
      }
      else if (shotDefinition->trajectoryMode == SHOT_TRAJECTORY_FIXED_RANGE) {
        if (ShotProjectile_UpdateFixedRangeTrajectory(shotDefinition,shotRuntime,modelNodeRuntime)) {
          return;
        }
      }
    }
    ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
    ModelNodeRuntime_UpdateDepthBinMasks(0,modelNodeRuntime);
    remainingStepTicks--;
  } while (remainingStepTicks != 0);
}

