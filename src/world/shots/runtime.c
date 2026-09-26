/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/shots/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/shots/runtime.h>
#include <thandor/thandor.h>

/* Implementation ownership: world/shots/runtime. */

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
   GameFactionRuntime_TestCapabilityBitClearCf [gameplay/faction/runtime],
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
  dword capabilityBitIndex;
  dword factionIndex;
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
        capabilityBitClear = GameFactionRuntime_TestCapabilityBitClearCf(capabilityBitIndex,factionIndex);
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
   Ownership: world/shots/runtime.
   Purpose: CF set propagates a load or allocation failure.
   Cross-module calls: WidePath_SetExtensionCode [core/text/path], MoviePlayback_AdvanceScheduledFrameAndTick
   [movie/runtime/playback].
*/
StatusValueEaxCf5 ShotRuntime_InitGraphicsResources(word *mutableBasePath)

{
  ShotRuntimeSlot *runtimeSlotCursor;
  int runtimeSlotsRemaining;
  ArenaAllocEaxCf5 loadResult;
  StatusValueEaxCf5 initResult;
  
  WidePath_SetExtensionCode(0x786667,mutableBasePath);
  MoviePlayback_AdvanceScheduledFrameAndTick();
  loadResult = THANDOR_BITCAST(GraphicsTextureSetEaxCf5, ArenaAllocEaxCf5, (*g_GraphicsTextureSetLoadPackageCf)(mutableBasePath));
  if (!loadResult.carry) {
    MoviePlayback_AdvanceScheduledFrameAndTick();
    g_ShotTextureSet = (GraphicsTextureSet *)loadResult.eax;
    WidePath_SetExtensionCode(0x6c6170,mutableBasePath);
    loadResult = THANDOR_BITCAST(GraphicsPaletteAssetEaxCf5, ArenaAllocEaxCf5, (*g_GraphicsPaletteAssetLoadPackage)(mutableBasePath));
    if (!loadResult.carry) {
      MoviePlayback_AdvanceScheduledFrameAndTick();
      g_ShotPalette = (GraphicsPaletteAsset *)loadResult.eax;
      loadResult = (*g_MemoryApi.alloc)(0x40000);
      runtimeSlotCursor = (ShotRuntimeSlot *)loadResult.eax;
      if (!loadResult.carry) {
        g_ShotRuntimeRebaseBaseMinusOne =
             (byte *)((int)&runtimeSlotCursor[-1].ownerAndTrajectory.secondaryEffectCountdownTicks +
                     3);
        g_ShotRuntimeSlots = runtimeSlotCursor;
        for (runtimeSlotsRemaining = 0x10000; runtimeSlotsRemaining != 0;
            runtimeSlotsRemaining = runtimeSlotsRemaining + -1) {
          (runtimeSlotCursor->definitionOrSavedId).definition = (ShotDefinition *)0x0;
          runtimeSlotCursor = (ShotRuntimeSlot *)&runtimeSlotCursor->launchSpeedQ12;
        }
        loadResult.eax = 0;
        loadResult.carry = false;
      }
    }
  }
  initResult.valueOrError = loadResult.eax;
  initResult.carry = loadResult.carry;
  return initResult;
}


/* Address: 0x0052B5C0.
   Ownership: world/shots/runtime.
   Purpose: Releases the shot runtime pool and graphics resources, releases owned definition resources, clears the
   registry, and has no semantic normal return.
   Cross-module calls: Resource_Release [assets/resource/runtime].
*/
void __thandor_void_preserve_eax_ecx ShotRuntime_ShutdownGraphicsResources(void)

{
  int registrySlotsRemaining;
  ShotDefinition **registryCursor;
  ShotDefinition *currentDefinition;
  
  (*g_MemoryApi.free)(g_ShotRuntimeSlots);
  g_ShotRuntimeSlots = (ShotRuntimeSlot *)0x0;
  if (g_ShotTextureSet != (GraphicsTextureSet *)0x0) {
    (*g_GraphicsTextureSetReleasePackageCf)(g_ShotTextureSet);
    g_ShotTextureSet = (GraphicsTextureSet *)0x0;
  }
  if (g_ShotPalette != (GraphicsPaletteAsset *)0x0) {
    (*g_GraphicsPaletteAssetLifecycleCallbacks3.releasePackage)(g_ShotPalette);
    g_ShotPalette = (GraphicsPaletteAsset *)0x0;
  }
  registryCursor = g_ShotDefinitionRegistry;
  registrySlotsRemaining = 0x100;
  do {
    currentDefinition = *registryCursor;
    if ((currentDefinition != (ShotDefinition *)0x0) &&
       (currentDefinition->ownedNestedResourcePresent != 0)) {
      Resource_Release(currentDefinition->ownedNestedResource);
    }
    *registryCursor = (ShotDefinition *)0x0;
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
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
ShotDefinitionLookupEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ShotRuntime_FindDefinitionByIdCf(PckShotDefinitionIdCatalog definitionId)

{
  ShotDefinition *registryDefinition;
  int registrySlotsRemaining;
  ShotDefinition **registryCursor;
  ShotDefinitionLookupEaxCf5 failureResult;
  ShotDefinitionLookupEaxCf5 successResult;
  ShotDefinition *candidateDefinition;
  
  registryCursor = g_ShotDefinitionRegistry;
  registrySlotsRemaining = 0x100;
  while ((registryDefinition = *registryCursor, registryDefinition == (ShotDefinition *)0x0 ||
         (registryDefinition->definitionId != definitionId))) {
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
    if (registrySlotsRemaining == 0) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(sdword)registryDefinition,g_PackageLastErrorPath);
      failureResult.carry = true;
      failureResult.definitionOrError = (ShotDefinition *)0x44;
      return failureResult;
    }
  }
  successResult.carry = false;
  successResult.definitionOrError = registryDefinition;
  return successResult;
}


/* Address: 0x0052B750.
   Ownership: world/shots/runtime.
   Purpose: Rebases all 4096 live shot slots after load and replaces each saved definitionId with its registered
   ShotDefinition pointer. The 170 physical records and 136 unique ids include bank aliases; no fabricated per-
   alias gameplay meaning is assigned.
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
  shotSlotsRemaining = 0x1000;
  firstSlotCountdown = &(g_ShotRuntimeSlots->ownerAndTrajectory).secondaryEffectCountdownTicks;
  *firstSlotCountdown = ~*firstSlotCountdown;
  do {
    rebasedRuntimeState = (shotSlot->runtimeStateOrSavedOffset).runtimeStatePointer;
    savedOwnerArmy = (shotSlot->ownerAndTrajectory).ownerArmyRuntime;
    if ((shotSlot->modelNodeOrSavedOffset).modelNode != (ModelRuntimeNode *)0x0) {
      if (rebasedRuntimeState != (void *)0x0) {
        rebasedRuntimeState = (void *)((int)rebasedRuntimeState + g_ModelRuntimeRebaseDelta);
      }
      rebasedOwnerArmy = (ArmyRuntimeSlot *)0x0;
      if (savedOwnerArmy != (ArmyRuntimeSlot *)0x0) {
        rebasedOwnerArmy = (ArmyRuntimeSlot *)
                    ((int)&savedOwnerArmy->modelRuntimeOrSavedOffset +
                    (int)g_ArmyRuntimeRebaseBaseMinusOne);
      }
      (shotSlot->modelNodeOrSavedOffset).modelNode =
           (ModelRuntimeNode *)
           (g_RuntimeObjectRebaseBaseMinusOne +
           (int)(&((shotSlot->modelNodeOrSavedOffset).modelNode)->modelPayload + -1) + 0x30);
      (shotSlot->runtimeStateOrSavedOffset).runtimeStatePointer = rebasedRuntimeState;
      (shotSlot->ownerAndTrajectory).ownerArmyRuntime = rebasedOwnerArmy;
      registryCursor = g_ShotDefinitionRegistry;
      registrySlotsRemaining = 0x100;
      do {
        registryDefinition = *registryCursor;
        if ((registryDefinition != (ShotDefinition *)0x0) &&
           ((shotSlot->definitionOrSavedId).definition ==
            (ShotDefinition *)registryDefinition->definitionId))
        goto ShotRuntime_RebaseSlotsAfterLoad_CommitResolvedDefinitionAndAdvance;
        registryCursor = registryCursor + 1;
        registrySlotsRemaining = registrySlotsRemaining + -1;
      } while (registrySlotsRemaining != 0);
      (shotSlot->modelNodeOrSavedOffset).modelNode = (ModelRuntimeNode *)0x0;
ShotRuntime_RebaseSlotsAfterLoad_CommitResolvedDefinitionAndAdvance:
      (shotSlot->definitionOrSavedId).definition = registryDefinition;
    }
    shotSlot = shotSlot + 1;
    shotSlotsRemaining = shotSlotsRemaining + -1;
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
   Cross-module calls: WorldObjectArray_AllocateFreeRecordCf [world/runtime/core],
   WorldRuntime_LinkNodeIntoOwnerListD8 [world/runtime/core], ShotDefinition_ComputeLaunchAnglesRegs
   [assets/shot/catalog], FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed],
   ModelLookupTable_ContainsPackedKeyCf [assets/model/definitions], ModelNodeRuntime_TransformLocalPointRegs
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
  short tintChannel0;
  short tintChannel1;
  short tintChannel2;
  short tintChannel3;
  ushort nodeAlphaPair;
  ushort definitionAlphaPair;
  ShotRuntimeSlot *slotsRemainingOrPool;
  GraphicsPaletteAsset *shotPalette;
  ShotModelRuntimeNodeClassView100 *shotModelNode;
  PackedArgb32 nodeTintArgb;
  ShotRuntimeSlot *slotsRemaining;
  Q12 runtimeLaunchSpeedQ12;
  Q12 worldXQ12;
  dword directionZOrNeighborhoodMask;
  ShotRuntimeSlot *shotRuntimeCursor;
  undefined1 nodeTintByte3Or1;
  undefined1 nodeTintByte2;
  undefined8 tintProduct;
  undefined1 definitionTintByte3Or1;
  undefined1 definitionTintByte2;
  ShotLaunchAnglesEaxEdx8 launchAngles;
  WorldObjectRecordEaxCf5 allocatedRecord;
  ModelLookupEntryEaxCf5 lookupEntry;
  GraphicsShadingRuntimeRecordEaxCf5 shadingAllocation;
  FixedDirectionXyzRegs12 launchDirection;
  ModelLocalPointRegs12 localPoint;
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
  allocatedRecord = WorldObjectArray_AllocateFreeRecordCf(worldRuntime);
  shotModelNode = (ShotModelRuntimeNodeClassView100 *)allocatedRecord.recordOrError;
  if (allocatedRecord.carry) {
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
  directionZOrNeighborhoodMask = launchDirection.edx;
  (shotRuntimeCursor->ownerAndTrajectory).directionComponent0Q12 = launchDirection.eax;
  if (shotDefinition->trajectoryMode == SHOT_TRAJECTORY_BALLISTIC) {
    directionZOrNeighborhoodMask = directionZOrNeighborhoodMask + (shotDefinition->ballisticDivisorQ12 >> 1);
  }
  shotModelNode->renderDepthBiasOrState = 0;
  (shotRuntimeCursor->ownerAndTrajectory).directionComponent1Q12 = launchDirection.ecx;
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
  lookupEntry = ModelLookupTable_ContainsPackedKeyCf(0,4,shotDefinition->ownedNestedResource);
  if (lookupEntry.carry) {
    shotModelNode->shadingRecord = (GraphicsShadingRuntimeRecord *)0x0;
  }
  else {
    localPoint = ModelNodeRuntime_TransformLocalPointRegs
                       (lookupEntry.entry,(ModelRuntimeNode *)shotModelNode);
    shadingAllocation = GraphicsShadingRuntime_AllocateRecordRegs
                       (shotDefinition->shadingTransitionDurationTicks,
                        (shotDefinition->shadingColorArgb >> 0x18) << 8,
                        shotDefinition->shadingColorArgb,localPoint.edx,localPoint.ecx,localPoint.eax);
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
  nodeTintByte3Or1 = (undefined1)(nodeTintArgb >> 0x18);
  nodeAlphaPair = CONCAT11(nodeTintByte3Or1,nodeTintByte3Or1);
  nodeTintByte2 = (undefined1)(nodeTintArgb >> 0x10);
  nodeTintByte3Or1 = (undefined1)(nodeTintArgb >> 8);
  definitionTintByte3Or1 = (undefined1)(definitionTintArgb >> 0x18);
  definitionAlphaPair = CONCAT11(definitionTintByte3Or1,definitionTintByte3Or1);
  definitionTintByte2 = (undefined1)(definitionTintArgb >> 0x10);
  definitionTintByte3Or1 = (undefined1)(definitionTintArgb >> 8);
  tintProduct = pmulhw(CONCAT26(nodeAlphaPair >> 4,
                           CONCAT24((ushort)(CONCAT35(CONCAT21(nodeAlphaPair,nodeTintByte2),
                                                      CONCAT14(nodeTintByte2,nodeTintArgb)) >> 0x20) >> 4,
                                    CONCAT22(CONCAT11(nodeTintByte3Or1,nodeTintByte3Or1) >> 4,
                                             CONCAT11((char)nodeTintArgb,(char)nodeTintArgb) >> 4))),
                  CONCAT26(definitionAlphaPair >> 4,
                           CONCAT24((ushort)(CONCAT35(CONCAT21(definitionAlphaPair,definitionTintByte2),CONCAT14(definitionTintByte2,definitionTintArgb)
                                                     ) >> 0x20) >> 4,
                                    CONCAT22(CONCAT11(definitionTintByte3Or1,definitionTintByte3Or1) >> 4,
                                             CONCAT11((char)definitionTintArgb,(char)definitionTintArgb) >> 4))));
  tintChannel0 = (short)tintProduct;
  tintChannel1 = (short)((ulonglong)tintProduct >> 0x10);
  tintChannel2 = (short)((ulonglong)tintProduct >> 0x20);
  tintChannel3 = (short)((ulonglong)tintProduct >> 0x30);
  shotModelNode->tintArgb =
       CONCAT13((0 < tintChannel3) * (tintChannel3 < 0x100) * (char)((ulonglong)tintProduct >> 0x30) -
                (0xff < tintChannel3),
                CONCAT12((0 < tintChannel2) * (tintChannel2 < 0x100) * (char)((ulonglong)tintProduct >> 0x20) -
                         (0xff < tintChannel2),
                         CONCAT11((0 < tintChannel1) * (tintChannel1 < 0x100) * (char)((ulonglong)tintProduct >> 0x10)
                                  - (0xff < tintChannel1),
                                  (0 < tintChannel0) * (tintChannel0 < 0x100) * (char)tintProduct - (0xff < tintChannel0))));
  ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)shotModelNode);
  ModelNodeRuntime_UpdateDepthBinMasks(0,(ModelRuntimeNode *)shotModelNode);
  lookupEntry = ModelLookupTable_ContainsPackedKeyCf(0,3,shotDefinition->ownedNestedResource);
  if (!lookupEntry.carry) {
    localPoint = ModelNodeRuntime_TransformLocalPointRegs
                       (lookupEntry.entry,(ModelRuntimeNode *)shotModelNode);
    worldXQ12 = localPoint.ecx;
    EffectRuntimePool_CreateInstanceFromDefinitionCf
              (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference4, 0x0),
               (shotModelNode->modelPayload).worldRotationAngle2,
               (shotModelNode->modelPayload).worldRotationAngle1,
               (shotModelNode->modelPayload).worldRotationAngle0,localPoint.edx,worldXQ12,localPoint.eax,
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

