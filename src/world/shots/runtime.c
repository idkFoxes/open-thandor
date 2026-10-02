/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/shots/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/shots/runtime.h>
#include <thandor/thandor.h>

/* Implementation ownership: world/shots/runtime. */

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

/* Address: 0x0052CC60.
   Diplomatic side effects of a shot hitting an army (called by the projectile maintenance in
   world/shots/maintenance.c). A repair shot (negative impact damage) that finds its target fully repaired
   ends the shooter's command on it. Any other hit adds to the pair pressure of target and shooter faction;
   if the target's faction already treats the shooter as hostile, the pair's relation tick is renewed and
   the "under attack" alert runs, otherwise friendly fire declares hostility (relation state 0 both ways),
   unless the shooter's active command targets another faction or the pair's last relation change is too
   recent (the ticks elapsed in both directions add up to less than 100).
*/
void ShotRuntime_ApplyArmyHitRelationAndNotifications(ModelRuntimeSlot *targetModelRuntime,ShotRuntimeSlot *shotRuntime)

{
  ArmyRuntimeSlot *shooterArmy;
  InGameSimulationTick currentTick;
  InGameRuntimeRoot *inGameRoot;
  FactionRelationState relationState;
  bool alreadyHostile;
  Q12 conditionRatio;
  FactionNotificationCodeBase activeFactionCodeForFirst;
  FactionNotificationCodeBase activeFactionCodeForSecond;
  FactionRelationStateNibble stateFirstTowardSecond;
  FactionRelationStateNibble stateSecondTowardFirst;
  uint32_t targetFactionIndex;
  uint32_t shooterFactionIndex;
  GameEntityRuntime *targetEntity;
  
  shooterArmy = shotRuntime->ownerAndTrajectory.ownerArmyRuntime;
  targetEntity = targetModelRuntime->ownerArmyRuntimeOrSavedOffset.entityRuntime; /* the hit model's army */
  if (shooterArmy != NULL) {
    if (shotRuntime->definitionOrSavedId.definition->targetClassImpactDamageQ12[0] < 0) {
      /* a condition ratio of 1.0 means the target is fully repaired */
      conditionRatio = ModelRuntime_QueryHierarchyConditionRatioQ12
                        ((RuntimeModelFactionPrefix *)targetEntity);
      if (conditionRatio == Q12_ONE &&
          (shooterArmy->commandModeFlags & ARMY_COMMAND_MODE_TARGET_ARMY) != 0 &&
          targetEntity == (GameEntityRuntime *)shooterArmy->commandTargetArmyRuntime) {
        ArmyRuntimeCommand_InterruptActiveTargetAndStampGeneration(shooterArmy);
        shooterArmy->commandGeneration = 1;
      }
    }
    else {
      shooterFactionIndex = shooterArmy->factionIndex;
      /* the target's owning faction */
      targetFactionIndex = targetEntity->common.ownership.ownerIndex;
      g_GameDataAuxState.pairPressureMatrix8x8[targetEntity->common.ownership.ownerIndex * 8 + shooterFactionIndex] =
           g_GameDataAuxState.pairPressureMatrix8x8
           [targetEntity->common.ownership.ownerIndex * 8 + shooterFactionIndex] + 256;
      if (shooterFactionIndex != 0 && targetFactionIndex != 0 && shooterFactionIndex != targetFactionIndex) {
        alreadyHostile = GameFactionRuntime_TestCapabilityBitClear(targetFactionIndex,shooterFactionIndex);
        inGameRoot = g_InGameRuntimeRoot;
        currentTick = g_GameFactionRuntimeImage.tail.simulationTick;
        /* relationStateTicks: tick of the pair's last relation change */
        if (alreadyHostile) {
          g_GameFactionRuntimeImage.records[shooterFactionIndex].relationStateTicks[targetFactionIndex] =
               g_GameFactionRuntimeImage.tail.simulationTick;
          g_GameFactionRuntimeImage.records[targetFactionIndex].relationStateTicks[shooterFactionIndex] =
               currentTick;
          GameFactionRuntime_UpdateImpactAlertAnchorAndNotify
                    (targetModelRuntime,&inGameRoot->worldRuntime);
          ShotRuntime_PostImpactRelationNotificationNoOp(shotRuntime,&inGameRoot->worldRuntime);
        }
        else if (((shooterArmy->commandModeFlags & ARMY_COMMAND_MODE_TARGET_ARMY) == 0 ||
                  (shooterArmy->commandTargetArmyRuntime != NULL &&
                   targetFactionIndex == shooterArmy->commandTargetArmyRuntime->factionIndex)) &&
                 99 < (int)((g_GameFactionRuntimeImage.tail.simulationTick * 2 -
                             g_GameFactionRuntimeImage.records[shooterFactionIndex].relationStateTicks
                             [targetFactionIndex]) -
                            g_GameFactionRuntimeImage.records[targetFactionIndex].relationStateTicks
                            [shooterFactionIndex])) {
          stateSecondTowardFirst = 0;
          stateFirstTowardSecond = 0;
          /* notification text code 11, or 12 when the relation was at state 8 or above */
          activeFactionCodeForSecond = 11;
          activeFactionCodeForFirst = 11;
          relationState = GameFactionRuntime_GetPackedStateNibble(targetFactionIndex,shooterFactionIndex);
          if (7 < relationState) {
            activeFactionCodeForFirst = 12;
            activeFactionCodeForSecond = 12;
          }
          GameFactionRuntime_ApplyPairwiseRelationTransition
                    (activeFactionCodeForFirst,activeFactionCodeForSecond,stateFirstTowardSecond,
                     stateSecondTowardFirst,targetFactionIndex,shooterFactionIndex);
        }
      }
    }
  }
  return;
}


/* Address: 0x0052B540.
   Loads the shot graphics of a level (mutableBasePath with its extension replaced by .gfx and .pal) and
   allocates the zeroed shot runtime pool; g_ShotRuntimeRebaseBaseMinusOne is set for the 1-based saved slot
   offsets. The movie playback is advanced between the steps so that a running movie keeps going. Returns true
   with *outError = 0 on success, or false with the error code of the first failing load or allocation in
   *outError (always written).
*/
bool ShotRuntime_InitGraphicsResources(uint16_t *mutableBasePath,uint32_t *outError)

{
  ShotRuntimeSlot *runtimeSlotCursor;
  int runtimeSlotsRemaining;
  uint32_t loadError;
  bool loadFailed;
  GraphicsTextureSet *loadedTextureSet;
  GraphicsPaletteAsset *loadedPalette;

  WidePath_SetExtensionCode(ASSET_MAGIC_GFX,mutableBasePath);
  MoviePlayback_AdvanceScheduledFrameAndTick();
  loadedTextureSet = g_GraphicsTextureSetLoadPackage(mutableBasePath,&loadError);
  loadFailed = loadedTextureSet == NULL;
  if (!loadFailed) {
    MoviePlayback_AdvanceScheduledFrameAndTick();
    g_ShotTextureSet = loadedTextureSet;
    WidePath_SetExtensionCode(ASSET_MAGIC_PAL,mutableBasePath);
    loadedPalette = g_GraphicsPaletteAssetLoadPackage(mutableBasePath,&loadError);
    loadFailed = loadedPalette == NULL;
    if (!loadFailed) {
      MoviePlayback_AdvanceScheduledFrameAndTick();
      g_ShotPalette = loadedPalette;
      loadError = g_MemoryApi.alloc(SHOT_RUNTIME_POOL_BYTES,(void **)&runtimeSlotCursor);
      loadFailed = loadError != 0;
      if (!loadFailed) {
        /* pool address - 1 */
        g_ShotRuntimeRebaseBaseMinusOne = (uint8_t *)runtimeSlotCursor - 1;
        g_ShotRuntimeSlots = runtimeSlotCursor;
        /* clears the pool dword by dword */
        for (runtimeSlotsRemaining = SHOT_RUNTIME_POOL_BYTES / 4; runtimeSlotsRemaining != 0;
            runtimeSlotsRemaining--) {
          runtimeSlotCursor->definitionOrSavedId.definition = NULL;
          runtimeSlotCursor = (ShotRuntimeSlot *)((uint32_t *)runtimeSlotCursor + 1);
        }
      }
    }
  }
  *outError = loadError;
  return !loadFailed;
}


/* Address: 0x0052B5C0.
   Counterpart of ShotRuntime_InitGraphicsResources at level end: frees the shot runtime pool, releases the
   shot texture set and palette, releases the nested resource each shot definition owns and empties the shot
   definition registry.
*/
void ShotRuntime_ShutdownGraphicsResources(void)

{
  int registrySlotsRemaining;
  ShotDefinition **registryCursor;
  ShotDefinition *currentDefinition;
  
  g_MemoryApi.free(g_ShotRuntimeSlots);
  g_ShotRuntimeSlots = NULL;
  if (g_ShotTextureSet != NULL) {
    g_GraphicsTextureSetReleasePackage(g_ShotTextureSet);
    g_ShotTextureSet = NULL;
  }
  if (g_ShotPalette != NULL) {
    g_GraphicsPaletteAssetLifecycleCallbacks3.releasePackage(g_ShotPalette);
    g_ShotPalette = NULL;
  }
  registryCursor = g_ShotDefinitionRegistry;
  for (registrySlotsRemaining = SHOT_DEFINITION_REGISTRY_SLOT_COUNT; registrySlotsRemaining != 0;
       registrySlotsRemaining--) {
    currentDefinition = *registryCursor;
    if (currentDefinition != NULL && currentDefinition->ownedNestedResourcePresent != 0) {
      Resource_Release(currentDefinition->ownedNestedResource);
    }
    *registryCursor = NULL;
    registryCursor++;
  }
  return;
}


/* Address: 0x0052B660.
   Looks a shot definition up by id in the 256-slot registry (a second copy of
   ShotDefinitionRegistry_FindByIdWithError, used by ShotDefinition registration to reject duplicates).
   Returns the registered definition (never NULL), or NULL on a miss; then a number is also formatted into
   g_PackageLastErrorPath (the original returned FATAL_ERROR_SHOT_ID_NOT_FOUND as its error value).
*/
ShotDefinition *ShotRuntime_FindDefinitionById(PckShotDefinitionIdCatalog definitionId)

{
  ShotDefinition *registryDefinition;
  int registrySlotsRemaining;
  ShotDefinition **registryCursor;

  registryCursor = g_ShotDefinitionRegistry;
  registrySlotsRemaining = SHOT_DEFINITION_REGISTRY_SLOT_COUNT;
  while (registryDefinition = *registryCursor, registryDefinition == NULL || registryDefinition->definitionId != definitionId) {
    registryCursor++;
    registrySlotsRemaining--;
    if (registrySlotsRemaining == 0) {
      /* the original formats EAX, i.e. the last registry slot, not the missing id (PUSH EAX at 0x0052B699) */
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(int32_t)registryDefinition,g_PackageLastErrorPath);
      return NULL;
    }
  }
  return registryDefinition;
}


/* Address: 0x0052B750.
   Turns the saved form of the shot slots back into pointers after a savegame load (and after writing one):
   for every live shot (non-zero model node) the 1-based model node, runtime state and owner army offsets are
   rebased and the saved definition id is replaced by the registered ShotDefinition. A shot whose id is no
   longer registered is dropped (model node cleared).
*/
void ShotRuntime_RebaseSlotsAfterLoad(void)

{
  ShotSecondaryEffectCountdownTicks *firstSlotCountdown;
  void *rebasedRuntimeState;
  int registrySlotsRemaining;
  int shotSlotsRemaining;
  ArmyRuntimeSlot *rebasedOwnerArmy;
  ShotDefinition **registryCursor;
  ShotRuntimeSlot *shotSlot;
  ArmyRuntimeSlot *savedOwnerArmy;
  ShotDefinition *registryDefinition;
  
  shotSlot = g_ShotRuntimeSlots;
  shotSlotsRemaining = SHOT_RUNTIME_SLOT_COUNT;
  /* NOT dword ptr [slot0 + 0x3C]: inverted on every call, purpose unknown */
  firstSlotCountdown = &g_ShotRuntimeSlots->ownerAndTrajectory.secondaryEffectCountdownTicks;
  *firstSlotCountdown = ~*firstSlotCountdown;
  do {
    rebasedRuntimeState = shotSlot->runtimeStateOrSavedOffset.runtimeStatePointer;
    savedOwnerArmy = shotSlot->ownerAndTrajectory.ownerArmyRuntime;
    if (shotSlot->modelNodeOrSavedOffset.modelNode != NULL) {
      if (rebasedRuntimeState != NULL) {
        rebasedRuntimeState = (void *)((int)rebasedRuntimeState + g_ModelRuntimeRebaseDelta);
      }
      rebasedOwnerArmy = NULL;
      if (savedOwnerArmy != NULL) {
        rebasedOwnerArmy = (ArmyRuntimeSlot *)((int)savedOwnerArmy + (int)g_ArmyRuntimeRebaseBaseMinusOne);
      }
      /* saved model node offset + g_RuntimeObjectRebaseBaseMinusOne */
      shotSlot->modelNodeOrSavedOffset.modelNode =
           (ModelRuntimeNode *)(g_RuntimeObjectRebaseBaseMinusOne + (int)shotSlot->modelNodeOrSavedOffset.modelNode);
      shotSlot->runtimeStateOrSavedOffset.runtimeStatePointer = rebasedRuntimeState;
      shotSlot->ownerAndTrajectory.ownerArmyRuntime = rebasedOwnerArmy;
      registryCursor = g_ShotDefinitionRegistry;
      registrySlotsRemaining = SHOT_DEFINITION_REGISTRY_SLOT_COUNT;
      while (registryDefinition = *registryCursor,
             (registryDefinition == NULL) ||
             (shotSlot->definitionOrSavedId.definition !=
              (ShotDefinition *)registryDefinition->definitionId)) {
        registryCursor++;
        registrySlotsRemaining--;
        if (registrySlotsRemaining == 0) {
          /* saved definition no longer registered: drop the shot (definition = last registry entry) */
          shotSlot->modelNodeOrSavedOffset.modelNode = NULL;
          break;
        }
      }
      shotSlot->definitionOrSavedId.definition = registryDefinition;
    }
    shotSlot++;
    shotSlotsRemaining--;
  } while (shotSlotsRemaining != 0);
}


/* Address: 0x0052BDB0.
   Fires one projectile of shotDefinition from the launch point towards the target point: takes the first
   free slot of the shot pool and a world object record for its model node, links the node into the world's
   owner list and seeds position, launch angles, velocity (plus half the ballistic divisor upwards for
   ballistic shots), lifetime, animation, optional shading light, occupancy class flags and tint; a launch
   effect is spawned when the model has an effect point (MODEL_POINT_CLASS_EFFECT). Called by the army and model weapon
   code (ArmyRuntime_ResolveShotLaunchFromModelAttachment, ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate,
   ArmyRuntime_UpdateTimedShotAndEffectEmitters, ModelRuntime_EmitProjectilesFromAttachmentPoints) and by
   EffectModelRuntimeMaintenance_UpdateLifecycleTintScaleAndTransitions. The original sets CF when no slot
   or record is free; this version just returns.
*/
void ShotRuntimePool_CreateProjectileFromDefinition
          (ShotTargetModelReference targetModelReference,ArmyRuntimeSlot *ownerArmyRuntime,
          Q12 targetWorldZQ12,Q12 targetWorldYQ12,Q12 targetWorldXQ12,Q12 launchWorldZQ12,
          Q12 launchWorldYQ12,Q12 launchWorldXQ12,ShotDefinition *shotDefinition,
          WorldRuntimeContext *worldRuntime)

{
  ModelResource *nestedModelResource;
  Q12 modelBoundingRadiusQ12;
  ShotAnimationFrameAccumulatorQ4 frameThresholdQ4;
  AngleTurn16Stored32 elevationOffsetAngle;
  ShotSecondaryEffectCountdownTicks secondaryEffectInterval;
  PackedArgb32 definitionTintArgb;
  ShotRuntimeSlot *slotsRemainingOrPool;
  GraphicsPaletteAsset *shotPalette;
  ShotModelRuntimeNode *shotModelNode;
  PackedArgb32 nodeTintArgb;
  ShotRuntimeSlot *slotsRemaining;
  Q12 runtimeLaunchSpeedQ12;
  uint32_t directionZOrNeighborhoodMask;
  ShotRuntimeSlot *shotRuntimeCursor;
  uint64_t tintProduct;
  ShotLaunchAngles launchAngles;
  ModelPackedPointRecord *packedPoint;
  FixedDirection launchDirection;
  ModelWorldPoint localPoint;
  TerrainOccupancyResolvedMasks resolvedMasks;
  char runtimeClassIndex;
  
  slotsRemaining = (ShotRuntimeSlot *)SHOT_RUNTIME_SLOT_COUNT; /* a counter kept in a pointer-typed variable */
  shotRuntimeCursor = g_ShotRuntimeSlots;
  slotsRemainingOrPool = g_ShotRuntimeSlots;
  while ((slotsRemainingOrPool != NULL) &&
         (shotRuntimeCursor->modelNodeOrSavedOffset.modelNode != NULL)) {
    shotRuntimeCursor = shotRuntimeCursor + 1;
    slotsRemaining = (ShotRuntimeSlot *)((int)slotsRemaining - 1);
    slotsRemainingOrPool = slotsRemaining;
  }
  if (slotsRemainingOrPool == NULL) {
    return; /* no free slot */
  }
  shotModelNode = (ShotModelRuntimeNode *)WorldObjectArray_AllocateFreeRecord(worldRuntime);
  if (shotModelNode == NULL) {
    return;
  }
  WorldRuntime_LinkOwnerListNode((WorldOwnerListNode *)shotModelNode);
  shotRuntimeCursor->modelNodeOrSavedOffset.modelNode = (ModelRuntimeNode *)shotModelNode;
  shotRuntimeCursor->definitionOrSavedId.definition = shotDefinition;
  shotModelNode->ownerClassId = WORLD_OWNER_RUNTIME_SHOT;
  shotModelNode->shotRuntime = shotRuntimeCursor;
  shotModelNode->worldTransform.translation.x = launchWorldXQ12;
  shotModelNode->worldTransform.translation.y = launchWorldYQ12;
  shotModelNode->worldTransform.translation.z = launchWorldZQ12;
  runtimeLaunchSpeedQ12 = shotDefinition->launchSpeedQ12;
  if (shotDefinition->trajectoryRampDurationTicks != 0) {
    runtimeLaunchSpeedQ12 = 0;
  }
  shotRuntimeCursor->lifetimeTicksRemaining = shotDefinition->projectileLifetimeTicks;
  shotRuntimeCursor->launchSpeedQ12 = runtimeLaunchSpeedQ12;
  shotRuntimeCursor->ownerAndTrajectory.ownerArmyRuntime = ownerArmyRuntime;
  launchAngles = ShotDefinition_ComputeLaunchAngles
                     (targetWorldZQ12,targetWorldYQ12,targetWorldXQ12,launchWorldZQ12,
                      launchWorldYQ12,launchWorldXQ12,shotDefinition);
  shotModelNode->modelPayload.worldRotationAngle0 = launchAngles.headingAngle;
  shotModelNode->modelPayload.worldRotationAngle1 = launchAngles.elevationAngle;
  shotModelNode->modelPayload.worldRotationAngle2 = launchAngles.headingAngle;
  launchDirection = FixedMath_DirectionFromAnglesScaled
                     (launchAngles.elevationAngle,launchAngles.headingAngle,shotDefinition->launchSpeedQ12);
  directionZOrNeighborhoodMask = launchDirection.z;
  shotRuntimeCursor->ownerAndTrajectory.directionComponent0Q12 = launchDirection.x;
  if (shotDefinition->trajectoryMode == SHOT_TRAJECTORY_BALLISTIC) {
    directionZOrNeighborhoodMask = directionZOrNeighborhoodMask + (shotDefinition->ballisticDivisorQ12 >> 1);
  }
  shotModelNode->renderDepthBiasOrState = 0;
  shotRuntimeCursor->ownerAndTrajectory.directionComponent1Q12 = launchDirection.y;
  shotRuntimeCursor->ownerAndTrajectory.directionComponent2Q12 = directionZOrNeighborhoodMask;
  shotRuntimeCursor->projectileAgeTicks = 0;
  shotPalette = g_ShotPalette;
  nestedModelResource = shotDefinition->ownedNestedResource;
  shotModelNode->modelPayload.textureSet = g_ShotTextureSet;
  modelBoundingRadiusQ12 = nestedModelResource->boundingRadiusQ12;
  shotModelNode->modelPayload.paletteAsset = shotPalette;
  shotModelNode->subtreeBoundingRadiusQ12 = modelBoundingRadiusQ12;
  shotModelNode->modelPayload.modelResource = nestedModelResource;
  frameThresholdQ4 = shotDefinition->animationFrameAdvanceThresholdQ4;
  elevationOffsetAngle = shotDefinition->elevationOffsetAngle16;
  shotRuntimeCursor->ownerAndTrajectory.animationFrameIndex = 0;
  shotRuntimeCursor->impactEffectEmissionFlags = 0;
  shotRuntimeCursor->animationFrameAccumulatorQ4 = frameThresholdQ4;
  shotRuntimeCursor->runtimeStateOrSavedOffset.runtimeState = targetModelReference;
  shotRuntimeCursor->elevationOffsetAngle16 = elevationOffsetAngle;
  shotModelNode->modelPayload.meshGroupMask = UINT32_MAX;
  secondaryEffectInterval = shotDefinition->secondaryEffectIntervalTicks;
  shotModelNode->runtimeFlags = shotModelNode->runtimeFlags | 1;
  shotRuntimeCursor->ownerAndTrajectory.secondaryEffectCountdownTicks = secondaryEffectInterval;
  shotModelNode->textureSubresourceBaseIndex = 0;
  shotModelNode->modelRuntimeLinkOrSavedOffset = NULL;
  /* optional light point of the model: allocates a shading record there */
  if (!ModelLookupTable_FindPackedPoint
         (0,MODEL_POINT_CLASS_LIGHT,shotDefinition->ownedNestedResource,&packedPoint)) {
    shotModelNode->shadingRecord = NULL;
  }
  else {
    localPoint = ModelNodeRuntime_TransformLocalPoint
                       (packedPoint,(ModelRuntimeNode *)shotModelNode);
    shotModelNode->shadingRecord = GraphicsShadingRuntime_AllocateRecord
                       (shotDefinition->shadingTransitionDurationTicks,
                        (shotDefinition->shadingColorArgb >> 24) << 8, /* alpha byte = radius / 16 */
                        shotDefinition->shadingColorArgb,localPoint.zQ12,localPoint.yQ12,localPoint.xQ12);
  }
  shotModelNode->parentNode = NULL;
  shotModelNode->childCount = 0;
  runtimeClassIndex = (char)worldRuntime->activeFactionRuntimeIndex;
  directionZOrNeighborhoodMask = TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                     (Q12_ONE,shotModelNode->worldTransform.translation.y,
                      shotModelNode->worldTransform.translation.x,worldRuntime->fieldGrid);
  resolvedMasks =
       TerrainOccupancyMask_ResolveRuntimeClassFlags(TERRAIN_OCCUPANCY_FLAG_NOT_REMEMBERED,0,directionZOrNeighborhoodMask,runtimeClassIndex);
  shotRuntimeCursor->terrainRuntimeClassState = resolvedMasks.primaryOccupancyMask;
  shotModelNode->runtimeFlags = shotModelNode->runtimeFlags | resolvedMasks.runtimeFlags | TERRAIN_OCCUPANCY_FLAG_NOT_REMEMBERED;
  nodeTintArgb = ModelRuntimeNode_GetStateTintArgb(shotModelNode);
  definitionTintArgb = shotDefinition->stateTintArgb;
  /* PUNPCKLBW/PSRLW 4 both tints, PMULHW, PACKUSWB */
  tintProduct = pmulhw(ShotTint_UnpackBytesShiftRight(nodeTintArgb,4),
                       ShotTint_UnpackBytesShiftRight(definitionTintArgb,4));
  shotModelNode->tintArgb = ShotTint_PackWordsUnsignedSaturate(tintProduct);
  ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)shotModelNode);
  ModelNodeRuntime_UpdateDepthBinMasks(0,(ModelRuntimeNode *)shotModelNode);
  if (ModelLookupTable_FindPackedPoint
        (0,MODEL_POINT_CLASS_EFFECT,shotDefinition->ownedNestedResource,&packedPoint)) {
    localPoint = ModelNodeRuntime_TransformLocalPoint
                       (packedPoint,(ModelRuntimeNode *)shotModelNode);
    EffectRuntimePool_CreateInstanceFromDefinition
              (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference, 0),
               shotModelNode->modelPayload.worldRotationAngle2,
               shotModelNode->modelPayload.worldRotationAngle1,
               shotModelNode->modelPayload.worldRotationAngle0,localPoint.zQ12,localPoint.yQ12,localPoint.xQ12,
               shotDefinition->launchEffectDefinition,worldRuntime);
  }
  return;
}


/* Address: 0x00514710.
   Hook called by ShotRuntime_ApplyArmyHitRelationAndNotifications after the "under attack" alert of a hit on
   a hostile army; it does nothing.
*/
void ShotRuntime_PostImpactRelationNotificationNoOp(ShotRuntimeSlot *shotRuntime,WorldRuntimeContext *worldRuntime)

{
  return;
}

