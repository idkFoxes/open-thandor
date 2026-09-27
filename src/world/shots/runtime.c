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

/* Address: 0x0052CC60.
   Ownership: world/shots/runtime.
   Purpose: Processes a shot hit against an ArmyRuntimeSlot, updates pairwise faction pressure and relation
   timestamps, applies verified relation-state transitions, and emits the associated runtime notifications. The
   function preserves normal return registers and returns with RET 0x08. Nested raw-callee closure: command
   interruption, faction impact-anchor notification, and the exact post-impact no-op hook are now separately owned
   and typed. Role: Applies faction-relation/target interruption and notification side effects for an army hit.
   Inputs: Hit ArmyRuntimeSlot and ShotRuntimeSlot including owner/faction.
   Local calls: ShotRuntime_PostImpactRelationNotificationNoOp.
   Cross-module calls: ModelRuntime_QueryHierarchyScaleRatioQ12Regs [world/model/runtime],
   ArmyRuntimeCommand_InterruptActiveTargetAndStampGeneration [gameplay/army/movement],
   GameFactionRuntime_TestCapabilityBitClear [gameplay/faction/runtime],
   GameFactionRuntime_UpdateImpactAlertAnchorAndNotify [gameplay/faction/runtime],
   GameFactionRuntime_GetPackedStateNibble [gameplay/faction/runtime],
   GameFactionRuntime_ApplyPairwiseRelationTransition [gameplay/faction/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
ShotRuntime_ApplyArmyHitRelationAndNotifications
          (ArmyRuntimeSlot *targetArmyRuntime,ShotRuntimeSlot *shotRuntime)

{
  ArmyRuntimeSlot *armyRuntime;
  InGameSimulationTick currentTick;
  InGameRuntimeRootImageC3E4 *inGameRoot;
  FactionRelationState relationState;
  bool capabilityBitClear;
  ModelRuntimeScaleRatioRegisterPairQ12 scaleRatio;
  FactionNotificationCodeBase activeFactionCodeForFirst;
  FactionNotificationCodeBase activeFactionCodeForSecond;
  FactionRelationStateNibble stateFirstTowardSecond;
  FactionRelationStateNibble stateSecondTowardFirst;
  uint32_t capabilityBitIndex;
  uint32_t factionIndex;
  GameEntityRuntime *runtimeEntry;
  
  armyRuntime = (shotRuntime->ownerAndTrajectory).ownerArmyRuntime;
  runtimeEntry = targetArmyRuntime->linkedEntityRuntime;
  if (armyRuntime != (ArmyRuntimeSlot *)0x0) {
    if (((shotRuntime->definitionOrSavedId).definition)->targetClassImpactDamageQ12[0] < 0) {
      scaleRatio = ModelRuntime_QueryHierarchyScaleRatioQ12Regs
                        ((RuntimeModelFactionPrefix10 *)runtimeEntry);
      if ((((int)scaleRatio == (int)(scaleRatio >> 0x20)) && ((armyRuntime->commandModeFlags & 1) != 0)) &&
         (runtimeEntry == (GameEntityRuntime *)armyRuntime->commandTargetArmyRuntime)) {
        ArmyRuntimeCommand_InterruptActiveTargetAndStampGeneration(armyRuntime);
        armyRuntime->commandGeneration = 1;
      }
    }
    else {
      factionIndex = armyRuntime->factionIndex;
      capabilityBitIndex =
           ((ModelRuntimeSlotReferenceOrSavedOffset4 *)&runtimeEntry->common)[3].savedIdOrOffset;
      g_GameDataAuxState.pairPressureMatrix8x8
      [((ModelRuntimeSlotReferenceOrSavedOffset4 *)&runtimeEntry->common)[3].savedIdOrOffset * 8 +
       factionIndex] =
           g_GameDataAuxState.pairPressureMatrix8x8
           [((ModelRuntimeSlotReferenceOrSavedOffset4 *)&runtimeEntry->common)[3].savedIdOrOffset *
            8 + factionIndex] + 0x100;
      if (((factionIndex != 0) && (capabilityBitIndex != 0)) && (factionIndex != capabilityBitIndex)
         ) {
        capabilityBitClear = GameFactionRuntime_TestCapabilityBitClear(capabilityBitIndex,factionIndex);
        inGameRoot = g_InGameRuntimeRoot;
        currentTick = g_GameFactionRuntimeImage.tail.simulationTick;
        if (capabilityBitClear) {
          *(InGameSimulationTick *)(factionIndex * 0x740 + THANDOR_ADDR(g_GameFactionRuntimeImage,0x700) + capabilityBitIndex * 4) =
               g_GameFactionRuntimeImage.tail.simulationTick;
          *(InGameSimulationTick *)(capabilityBitIndex * 0x740 + THANDOR_ADDR(g_GameFactionRuntimeImage,0x700) + factionIndex * 4) =
               currentTick;
          GameFactionRuntime_UpdateImpactAlertAnchorAndNotify
                    (targetArmyRuntime,&inGameRoot->worldRuntime0A30);
          ShotRuntime_PostImpactRelationNotificationNoOp(shotRuntime,&inGameRoot->worldRuntime0A30);
        }
        else if ((((armyRuntime->commandModeFlags & 1) == 0) ||
                 ((armyRuntime->commandTargetArmyRuntime != (ArmyRuntimeSlot *)0x0 &&
                  (capabilityBitIndex == armyRuntime->commandTargetArmyRuntime->factionIndex)))) &&
                (99 < (int)((g_GameFactionRuntimeImage.tail.simulationTick * 2 -
                            *(int *)(factionIndex * 0x740 + THANDOR_ADDR(g_GameFactionRuntimeImage,0x700) + capabilityBitIndex * 4)) -
                           *(int *)(capabilityBitIndex * 0x740 + THANDOR_ADDR(g_GameFactionRuntimeImage,0x700) + factionIndex * 4)))) {
          stateSecondTowardFirst = 0;
          stateFirstTowardSecond = 0;
          activeFactionCodeForSecond = 0xb;
          activeFactionCodeForFirst = 0xb;
          relationState = GameFactionRuntime_GetPackedStateNibble(capabilityBitIndex,factionIndex);
          if (7 < relationState) {
            activeFactionCodeForFirst = 0xc;
            activeFactionCodeForSecond = 0xc;
          }
          GameFactionRuntime_ApplyPairwiseRelationTransition
                    (activeFactionCodeForFirst,activeFactionCodeForSecond,stateFirstTowardSecond,
                     stateSecondTowardFirst,capabilityBitIndex,factionIndex);
        }
      }
    }
  }
  return;
}


/* Address: 0x0052B540.
   Loads the shot graphics of a level (mutableBasePath with its extension replaced by .gfx and .pal) and
   allocates the zeroed shot runtime pool; g_ShotRuntimeRebaseBaseMinusOne is set for the 1-based saved slot
   offsets. The movie playback is advanced between the steps so that a running movie keeps going. CF set with
   the error code of the first failing load or allocation.
*/
StatusResult ShotRuntime_InitGraphicsResources(uint16_t *mutableBasePath)

{
  ShotRuntimeSlot *runtimeSlotCursor;
  int runtimeSlotsRemaining;
  ArenaAllocResult loadResult;
  StatusResult initResult;
  
  WidePath_SetExtensionCode(0x786667,mutableBasePath); /* "gfx" */
  MoviePlayback_AdvanceScheduledFrameAndTick();
  loadResult = THANDOR_BITCAST(TextureSetResult, ArenaAllocResult, g_GraphicsTextureSetLoadPackage(mutableBasePath));
  if (!loadResult.failed) {
    MoviePlayback_AdvanceScheduledFrameAndTick();
    g_ShotTextureSet = (GraphicsTextureSet *)loadResult.payloadOrError;
    WidePath_SetExtensionCode(0x6c6170,mutableBasePath); /* "pal" */
    loadResult = THANDOR_BITCAST(PaletteAssetResult, ArenaAllocResult, g_GraphicsPaletteAssetLoadPackage(mutableBasePath));
    if (!loadResult.failed) {
      MoviePlayback_AdvanceScheduledFrameAndTick();
      g_ShotPalette = (GraphicsPaletteAsset *)loadResult.payloadOrError;
      loadResult = g_MemoryApi.alloc(SHOT_RUNTIME_POOL_BYTES);
      runtimeSlotCursor = (ShotRuntimeSlot *)loadResult.payloadOrError;
      if (!loadResult.failed) {
        /* pool address - 1 */
        g_ShotRuntimeRebaseBaseMinusOne =
             (uint8_t *)((int)&runtimeSlotCursor[-1].ownerAndTrajectory.secondaryEffectCountdownTicks +
                     3);
        g_ShotRuntimeSlots = runtimeSlotCursor;
        /* clears the pool dword by dword */
        for (runtimeSlotsRemaining = SHOT_RUNTIME_POOL_BYTES / 4; runtimeSlotsRemaining != 0;
            runtimeSlotsRemaining--) {
          (runtimeSlotCursor->definitionOrSavedId).definition = NULL;
          runtimeSlotCursor = (ShotRuntimeSlot *)&runtimeSlotCursor->launchSpeedQ12;
        }
        loadResult.payloadOrError = 0;
        loadResult.failed = false;
      }
    }
  }
  initResult.valueOrError = loadResult.payloadOrError;
  initResult.failed = loadResult.failed;
  return initResult;
}


/* Address: 0x0052B5C0.
   Counterpart of ShotRuntime_InitGraphicsResources at level end: frees the shot runtime pool, releases the
   shot texture set and palette, releases the nested resource each shot definition owns and empties the shot
   definition registry.
*/
void __thandor_void_preserve_eax_ecx ShotRuntime_ShutdownGraphicsResources(void)

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
  registrySlotsRemaining = SHOT_DEFINITION_REGISTRY_SLOTS;
  do {
    currentDefinition = *registryCursor;
    if ((currentDefinition != NULL) &&
       (currentDefinition->ownedNestedResourcePresent != 0)) {
      Resource_Release(currentDefinition->ownedNestedResource);
    }
    *registryCursor = NULL;
    registryCursor++;
    registrySlotsRemaining--;
  } while (registrySlotsRemaining != 0);
  return;
}


/* Address: 0x0052B660.
   Ownership: world/shots/runtime.
   Purpose: Scans the fixed 256-pointer shot-definition registry for definitionId. A match returns ShotDefinition *
   with CF clear. Failure writes the requested identifier to the package error buffer and returns error 0x44 with
   CF set. The stock corpus contains 170 records and 136 unique ids across five shot banks; duplicate ids are
   aliases/variants, not permission to invent distinct gameplay meanings.
*/
ShotDefinitionResult __thandor_eax_cf_preserve_ecx_edx
ShotRuntime_FindDefinitionById(PckShotDefinitionIdCatalog definitionId)

{
  ShotDefinition *registryDefinition;
  int registrySlotsRemaining;
  ShotDefinition **registryCursor;
  ShotDefinitionResult failureResult;
  ShotDefinitionResult successResult;
  ShotDefinition *candidateDefinition;
  
  registryCursor = g_ShotDefinitionRegistry;
  registrySlotsRemaining = 0x100;
  while ((registryDefinition = *registryCursor, registryDefinition == (ShotDefinition *)0x0 ||
         (registryDefinition->definitionId != definitionId))) {
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
    if (registrySlotsRemaining == 0) {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(int32_t)registryDefinition,g_PackageLastErrorPath);
      failureResult.notFound = true;
      failureResult.definitionOrError = (ShotDefinition *)0x44;
      return failureResult;
    }
  }
  successResult.notFound = false;
  successResult.definitionOrError = registryDefinition;
  return successResult;
}


/* Address: 0x0052B750.
   Turns the saved form of the shot slots back into pointers after a savegame load (and after writing one):
   for every live shot (non-zero model node) the 1-based model node, runtime state and owner army offsets are
   rebased and the saved definition id is replaced by the registered ShotDefinition. A shot whose id is no
   longer registered is dropped (model node cleared).
*/
void __thandor_void_preserve_eax_ecx_edx ShotRuntime_RebaseSlotsAfterLoad(void)

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
  firstSlotCountdown = &(g_ShotRuntimeSlots->ownerAndTrajectory).secondaryEffectCountdownTicks;
  *firstSlotCountdown = ~*firstSlotCountdown;
  do {
    rebasedRuntimeState = (shotSlot->runtimeStateOrSavedOffset).runtimeStatePointer;
    savedOwnerArmy = (shotSlot->ownerAndTrajectory).ownerArmyRuntime;
    if ((shotSlot->modelNodeOrSavedOffset).modelNode != NULL) {
      if (rebasedRuntimeState != NULL) {
        rebasedRuntimeState = (void *)((int)rebasedRuntimeState + g_ModelRuntimeRebaseDelta);
      }
      rebasedOwnerArmy = NULL;
      if (savedOwnerArmy != NULL) {
        rebasedOwnerArmy = (ArmyRuntimeSlot *)
                    ((int)&savedOwnerArmy->modelRuntimeOrSavedOffset +
                    (int)g_ArmyRuntimeRebaseBaseMinusOne);
      }
      /* saved model node offset + g_RuntimeObjectRebaseBaseMinusOne */
      (shotSlot->modelNodeOrSavedOffset).modelNode =
           (ModelRuntimeNode *)
           (g_RuntimeObjectRebaseBaseMinusOne +
           (int)(&((shotSlot->modelNodeOrSavedOffset).modelNode)->modelPayload + -1) + 0x30);
      (shotSlot->runtimeStateOrSavedOffset).runtimeStatePointer = rebasedRuntimeState;
      (shotSlot->ownerAndTrajectory).ownerArmyRuntime = rebasedOwnerArmy;
      registryCursor = g_ShotDefinitionRegistry;
      registrySlotsRemaining = SHOT_DEFINITION_REGISTRY_SLOTS;
      for (;;) {
        registryDefinition = *registryCursor;
        if ((registryDefinition != NULL) &&
           ((shotSlot->definitionOrSavedId).definition ==
            (ShotDefinition *)registryDefinition->definitionId)) break;
        registryCursor++;
        registrySlotsRemaining--;
        if (registrySlotsRemaining == 0) {
          /* saved definition no longer registered: drop the shot (definition = last registry entry) */
          (shotSlot->modelNodeOrSavedOffset).modelNode = NULL;
          break;
        }
      }
      (shotSlot->definitionOrSavedId).definition = registryDefinition;
    }
    shotSlot++;
    shotSlotsRemaining--;
    if (shotSlotsRemaining == 0) {
      return;
    }
  } while( true );
}


/* Address: 0x0052BDB0.
   Ownership: world/shots/runtime.
   Purpose: Allocates one ShotRuntimeSlot and projectile model from a typed ShotDefinition; launch coordinates are
   fixed-point scalars and ownerArmyRuntime is an ArmyRuntimeSlot pointer. EAX/CF remain the nonstandard result
   channels. Fire-chain terminus: allocates a shot runtime record from the pool, seeds trajectory from the
   transformed launch point, and links it for the projectile motion maintenance pass. CF-style allocation failure.
   Role: Allocates and initializes a live projectile from a ShotDefinition.
   Cross-module calls: WorldObjectArray_AllocateFreeRecord [world/runtime/core],
   WorldRuntime_LinkNodeIntoOwnerListD8 [world/runtime/core], ShotDefinition_ComputeLaunchAnglesRegs
   [assets/shot/catalog], FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed],
   ModelLookupTable_ContainsPackedKey [assets/model/definitions], ModelNodeRuntime_TransformLocalPointRegs
   [world/model/hierarchy].
*/
void __thandor_void_preserve_eax_ecx_edx
ShotRuntimePool_CreateProjectileFromDefinition
          (ShotRuntimeState14 shotRuntimeState14,ArmyRuntimeSlot *ownerArmyRuntime,
          Q12 targetWorldZQ12,Q12 targetWorldYQ12,Q12 targetWorldXQ12,Q12 launchWorldZQ12,
          Q12 launchWorldYQ12,Q12 launchWorldXQ12,ShotDefinition *shotDefinition,
          WorldRuntimeContext *worldRuntime)

{
  ModelResourceHitTestAndRenderView210 *nestedModelResource;
  Q12 modelBoundingRadiusQ12;
  ShotAnimationFrameAccumulatorQ4 frameThresholdQ4;
  AngleTurn16Stored32 elevationOffsetAngle;
  ShotSecondaryEffectCountdownTicks secondaryEffectInterval;
  PackedArgb32 definitionTintArgb;
  ShotRuntimeSlot *slotsRemainingOrPool;
  GraphicsPaletteAsset *shotPalette;
  ShotModelRuntimeNodeClassView100 *shotModelNode;
  PackedArgb32 nodeTintArgb;
  ShotRuntimeSlot *slotsRemaining;
  Q12 runtimeLaunchSpeedQ12;
  Q12 worldXQ12;
  uint32_t directionZOrNeighborhoodMask;
  ShotRuntimeSlot *shotRuntimeCursor;
  uint64_t tintProduct;
  ShotLaunchAnglesEaxEdx8 launchAngles;
  WorldObjectAllocResult allocatedRecord;
  ModelLookupEntryResult lookupEntry;
  ShadingRecordResult shadingAllocation;
  FixedDirection launchDirection;
  ModelWorldPoint localPoint;
  TerrainOccupancyResolvedMasksRegs12 resolvedMasks;
  char runtimeClassIndex;
  
  slotsRemaining = (ShotRuntimeSlot *)0x1000;
  shotRuntimeCursor = g_ShotRuntimeSlots;
  slotsRemainingOrPool = g_ShotRuntimeSlots;
  while( true ) {
    if (slotsRemainingOrPool == (ShotRuntimeSlot *)0x0) {
      return;
    }
    if ((shotRuntimeCursor->modelNodeOrSavedOffset).modelNode == (ModelRuntimeNode *)0x0) break;
    shotRuntimeCursor = shotRuntimeCursor + 1;
    slotsRemaining = (ShotRuntimeSlot *)
              ((int)&slotsRemaining[-1].ownerAndTrajectory.secondaryEffectCountdownTicks + 3);
    slotsRemainingOrPool = slotsRemaining;
  }
  allocatedRecord = WorldObjectArray_AllocateFreeRecord(worldRuntime);
  shotModelNode = (ShotModelRuntimeNodeClassView100 *)allocatedRecord.recordOrError;
  if (allocatedRecord.failed) {
    return;
  }
  WorldRuntime_LinkNodeIntoOwnerListD8((WorldOwnerListNode100 *)shotModelNode);
  (shotRuntimeCursor->modelNodeOrSavedOffset).modelNode = (ModelRuntimeNode *)shotModelNode;
  (shotRuntimeCursor->definitionOrSavedId).definition = shotDefinition;
  shotModelNode->ownerClassId = WORLD_OWNER_RUNTIME_SHOT;
  shotModelNode->shotRuntime = shotRuntimeCursor;
  (shotModelNode->worldTransform).translation.x = launchWorldXQ12;
  (shotModelNode->worldTransform).translation.y = launchWorldYQ12;
  (shotModelNode->worldTransform).translation.z = launchWorldZQ12;
  runtimeLaunchSpeedQ12 = shotDefinition->launchSpeedQ12;
  if (shotDefinition->trajectoryRampDurationTicks != 0) {
    runtimeLaunchSpeedQ12 = 0;
  }
  shotRuntimeCursor->lifetimeTicksRemaining = shotDefinition->projectileLifetimeTicks;
  shotRuntimeCursor->launchSpeedQ12 = runtimeLaunchSpeedQ12;
  (shotRuntimeCursor->ownerAndTrajectory).ownerArmyRuntime = ownerArmyRuntime;
  launchAngles = ShotDefinition_ComputeLaunchAnglesRegs
                     (targetWorldZQ12,targetWorldYQ12,targetWorldXQ12,launchWorldZQ12,
                      launchWorldYQ12,launchWorldXQ12,shotDefinition);
  (shotModelNode->modelPayload).worldRotationAngle0 = launchAngles.headingAngle;
  (shotModelNode->modelPayload).worldRotationAngle1 = launchAngles.elevationAngle;
  (shotModelNode->modelPayload).worldRotationAngle2 = launchAngles.headingAngle;
  launchDirection = FixedMath_DirectionFromAnglesScaledRegs
                     (launchAngles.elevationAngle,launchAngles.headingAngle,shotDefinition->launchSpeedQ12);
  directionZOrNeighborhoodMask = launchDirection.z;
  (shotRuntimeCursor->ownerAndTrajectory).directionComponent0Q12 = launchDirection.x;
  if (shotDefinition->trajectoryMode == SHOT_TRAJECTORY_BALLISTIC) {
    directionZOrNeighborhoodMask = directionZOrNeighborhoodMask + (shotDefinition->ballisticDivisorQ12 >> 1);
  }
  shotModelNode->renderDepthBiasOrState = 0;
  (shotRuntimeCursor->ownerAndTrajectory).directionComponent1Q12 = launchDirection.y;
  (shotRuntimeCursor->ownerAndTrajectory).directionComponent2Q12 = directionZOrNeighborhoodMask;
  shotRuntimeCursor->projectileAgeTicks = 0;
  shotPalette = g_ShotPalette;
  nestedModelResource = shotDefinition->ownedNestedResource;
  (shotModelNode->modelPayload).textureSet = g_ShotTextureSet;
  modelBoundingRadiusQ12 = nestedModelResource->boundingRadiusQ12;
  (shotModelNode->modelPayload).paletteAsset = shotPalette;
  shotModelNode->subtreeBoundingRadiusQ12 = modelBoundingRadiusQ12;
  (shotModelNode->modelPayload).modelResource = nestedModelResource;
  frameThresholdQ4 = shotDefinition->animationFrameAdvanceThresholdQ4;
  elevationOffsetAngle = shotDefinition->elevationOffsetAngle16;
  (shotRuntimeCursor->ownerAndTrajectory).animationFrameIndex = 0;
  shotRuntimeCursor->impactEffectEmissionFlags = 0;
  shotRuntimeCursor->animationFrameAccumulatorQ4 = frameThresholdQ4;
  (shotRuntimeCursor->runtimeStateOrSavedOffset).runtimeState = shotRuntimeState14;
  shotRuntimeCursor->elevationOffsetAngle16 = elevationOffsetAngle;
  (shotModelNode->modelPayload).meshGroupMask = 0xffffffff;
  secondaryEffectInterval = shotDefinition->secondaryEffectIntervalTicks;
  shotModelNode->runtimeFlags = shotModelNode->runtimeFlags | 1;
  (shotRuntimeCursor->ownerAndTrajectory).secondaryEffectCountdownTicks = secondaryEffectInterval;
  shotModelNode->textureSubresourceBaseIndex = 0;
  shotModelNode->modelRuntimeLinkOrSavedOffset = (void *)0x0;
  lookupEntry = ModelLookupTable_ContainsPackedKey(0,4,shotDefinition->ownedNestedResource);
  if (lookupEntry.notFound) {
    shotModelNode->shadingRecord = (GraphicsShadingRuntimeRecord *)0x0;
  }
  else {
    localPoint = ModelNodeRuntime_TransformLocalPointRegs
                       (lookupEntry.entry,(ModelRuntimeNode *)shotModelNode);
    shadingAllocation = GraphicsShadingRuntime_AllocateRecordRegs
                       (shotDefinition->shadingTransitionDurationTicks,
                        (shotDefinition->shadingColorArgb >> 0x18) << 8,
                        shotDefinition->shadingColorArgb,localPoint.zQ12,localPoint.yQ12,localPoint.xQ12);
    shotModelNode->shadingRecord = shadingAllocation.record;
  }
  shotModelNode->parentNode = (ModelRuntimeNode *)0x0;
  shotModelNode->childCount = 0;
  runtimeClassIndex = (char)worldRuntime->activeFactionRuntimeIndex;
  directionZOrNeighborhoodMask = TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                     (0x1000,(shotModelNode->worldTransform).translation.y,
                      (shotModelNode->worldTransform).translation.x,worldRuntime->fieldGrid);
  resolvedMasks = TerrainOccupancyMask_ResolveRuntimeClassFlags(0x10,0,directionZOrNeighborhoodMask,runtimeClassIndex);
  shotRuntimeCursor->terrainRuntimeClassState = resolvedMasks.primaryOccupancyMask;
  shotModelNode->runtimeFlags = shotModelNode->runtimeFlags | resolvedMasks.runtimeFlags | 0x10;
  nodeTintArgb = UiNode_GetStateTintArgb((UiNodeBase *)shotModelNode);
  definitionTintArgb = shotDefinition->stateTintArgb;
  /* PUNPCKLBW/PSRLW 4 both tints, PMULHW, PACKUSWB */
  tintProduct = pmulhw(ShotTint_UnpackBytesShiftRight(nodeTintArgb,4),
                       ShotTint_UnpackBytesShiftRight(definitionTintArgb,4));
  shotModelNode->tintArgb = ShotTint_PackWordsUnsignedSaturate(tintProduct);
  ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)shotModelNode);
  ModelNodeRuntime_UpdateDepthBinMasks(0,(ModelRuntimeNode *)shotModelNode);
  lookupEntry = ModelLookupTable_ContainsPackedKey(0,3,shotDefinition->ownedNestedResource);
  if (!lookupEntry.notFound) {
    localPoint = ModelNodeRuntime_TransformLocalPointRegs
                       (lookupEntry.entry,(ModelRuntimeNode *)shotModelNode);
    worldXQ12 = localPoint.yQ12;
    EffectRuntimePool_CreateInstanceFromDefinition
              (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference4, 0x0),
               (shotModelNode->modelPayload).worldRotationAngle2,
               (shotModelNode->modelPayload).worldRotationAngle1,
               (shotModelNode->modelPayload).worldRotationAngle0,localPoint.zQ12,worldXQ12,localPoint.xQ12,
               shotDefinition->launchEffectDefinition,worldRuntime);
  }
  return;
}


/* Address: 0x00514710.
   Ownership: world/shots/runtime.
   Purpose: Exact two-argument post-impact relation hook. The archived implementation preserves the normal
   registers, performs no state change, and returns with RET 0x08.
*/
void __thandor_void_preserve_eax_ecx_edx
ShotRuntime_PostImpactRelationNotificationNoOp
          (ShotRuntimeSlot *shotRuntime,WorldRuntimeContext *worldRuntime)

{
  return;
}

