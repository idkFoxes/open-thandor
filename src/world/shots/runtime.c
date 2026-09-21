/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/shots/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/shots/runtime.h>

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
  InGameSimulationTick IVar1;
  InGameRuntimeRootImageC3E4 *pIVar2;
  FactionRelationState FVar3;
  bool bVar4;
  ModelRuntimeScaleRatioRegisterPairQ12 MVar5;
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
      MVar5 = ModelRuntime_QueryHierarchyScaleRatioQ12Regs
                        ((RuntimeModelFactionPrefix10 *)runtimeEntry);
      if ((((int)MVar5 == (int)(MVar5 >> 0x20)) && ((armyRuntime->commandModeFlags & 1) != 0)) &&
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
        bVar4 = GameFactionRuntime_TestCapabilityBitClearCf(capabilityBitIndex,factionIndex);
        pIVar2 = g_InGameRuntimeRoot;
        IVar1 = g_GameFactionRuntimeImage.tail.simulationTick;
        if (bVar4) {
          *(InGameSimulationTick *)(factionIndex * 0x740 + 0x50fa40 + capabilityBitIndex * 4) =
               g_GameFactionRuntimeImage.tail.simulationTick;
          *(InGameSimulationTick *)(capabilityBitIndex * 0x740 + 0x50fa40 + factionIndex * 4) =
               IVar1;
          GameFactionRuntime_UpdateImpactAlertAnchorAndNotify
                    (targetArmyRuntime,&pIVar2->worldRuntime0A30);
          ShotRuntime_PostImpactRelationNotificationNoOp(shotRuntime,&pIVar2->worldRuntime0A30);
        }
        else if ((((armyRuntime->commandModeFlags & 1) == 0) ||
                 ((armyRuntime->commandTargetArmyRuntime != (ArmyRuntimeSlot *)0x0 &&
                  (capabilityBitIndex == armyRuntime->commandTargetArmyRuntime->factionIndex)))) &&
                (99 < (int)((g_GameFactionRuntimeImage.tail.simulationTick * 2 -
                            *(int *)(factionIndex * 0x740 + 0x50fa40 + capabilityBitIndex * 4)) -
                           *(int *)(capabilityBitIndex * 0x740 + 0x50fa40 + factionIndex * 4)))) {
          stateSecondTowardFirst = 0;
          stateFirstTowardSecond = 0;
          activeFactionCodeForSecond = 0xb;
          activeFactionCodeForFirst = 0xb;
          FVar3 = GameFactionRuntime_GetPackedStateNibble(capabilityBitIndex,factionIndex);
          if (7 < FVar3) {
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
  ArenaAllocEaxCf5 AVar1;
  StatusValueEaxCf5 SVar2;
  
  WidePath_SetExtensionCode(0x786667,mutableBasePath);
  MoviePlayback_AdvanceScheduledFrameAndTick();
  AVar1 = (ArenaAllocEaxCf5)(*g_GraphicsTextureSetLoadPackageCf)(mutableBasePath);
  if (!AVar1.carry) {
    MoviePlayback_AdvanceScheduledFrameAndTick();
    g_ShotTextureSet = (GraphicsTextureSet *)AVar1.eax;
    WidePath_SetExtensionCode(0x6c6170,mutableBasePath);
    AVar1 = (ArenaAllocEaxCf5)(*g_GraphicsPaletteAssetLoadPackage)(mutableBasePath);
    if (!AVar1.carry) {
      MoviePlayback_AdvanceScheduledFrameAndTick();
      g_ShotPalette = (GraphicsPaletteAsset *)AVar1.eax;
      AVar1 = (*g_MemoryApi.alloc)(0x40000);
      runtimeSlotCursor = (ShotRuntimeSlot *)AVar1.eax;
      if (!AVar1.carry) {
        g_ShotRuntimeRebaseBaseMinusOne =
             (byte *)((int)&runtimeSlotCursor[-1].ownerAndTrajectory.secondaryEffectCountdownTicks +
                     3);
        g_ShotRuntimeSlots = runtimeSlotCursor;
        for (runtimeSlotsRemaining = 0x10000; runtimeSlotsRemaining != 0;
            runtimeSlotsRemaining = runtimeSlotsRemaining + -1) {
          (runtimeSlotCursor->definitionOrSavedId).definition = (ShotDefinition *)0x0;
          runtimeSlotCursor = (ShotRuntimeSlot *)&runtimeSlotCursor->launchSpeedQ12;
        }
        AVar1.eax = 0;
        AVar1.carry = false;
      }
    }
  }
  SVar2.valueOrError = AVar1.eax;
  SVar2.carry = AVar1.carry;
  return SVar2;
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
  ShotDefinition *arg4;
  int registrySlotsRemaining;
  ShotDefinition **registryCursor;
  ShotDefinitionLookupEaxCf5 SVar1;
  ShotDefinitionLookupEaxCf5 SVar2;
  ShotDefinition *candidateDefinition;
  
  registryCursor = g_ShotDefinitionRegistry;
  registrySlotsRemaining = 0x100;
  while ((arg4 = *registryCursor, arg4 == (ShotDefinition *)0x0 ||
         (arg4->definitionId != definitionId))) {
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
    if (registrySlotsRemaining == 0) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(sdword)arg4,g_PackageLastErrorPath);
      SVar1.carry = true;
      SVar1.definitionOrError = (ShotDefinition *)0x44;
      return SVar1;
    }
  }
  SVar2.carry = false;
  SVar2.definitionOrError = arg4;
  return SVar2;
}


/* Address: 0x0052B750.
   Ownership: world/shots/runtime.
   Purpose: Rebases all 4096 live shot slots after load and replaces each saved definitionId with its registered
   ShotDefinition pointer. The 170 physical records and 136 unique ids include bank aliases; no fabricated per-
   alias gameplay meaning is assigned.
*/
void __thandor_void_preserve_eax_ecx_edx ShotRuntime_RebaseSlotsAfterLoad(void)

{
  ShotSecondaryEffectCountdownTicks *pSVar1;
  void *pvVar2;
  int iVar3;
  int iVar4;
  ArmyRuntimeSlot *armySlot2;
  ShotDefinition **ppSVar5;
  ShotRuntimeSlot *pSVar6;
  ArmyRuntimeSlot *armySlot1;
  ShotDefinition *shotDefinition1;
  
  pSVar6 = g_ShotRuntimeSlots;
  iVar4 = 0x1000;
  pSVar1 = &(g_ShotRuntimeSlots->ownerAndTrajectory).secondaryEffectCountdownTicks;
  *pSVar1 = ~*pSVar1;
  do {
    pvVar2 = (pSVar6->runtimeStateOrSavedOffset).runtimeStatePointer;
    armySlot1 = (pSVar6->ownerAndTrajectory).ownerArmyRuntime;
    if ((pSVar6->modelNodeOrSavedOffset).modelNode != (ModelRuntimeNode *)0x0) {
      if (pvVar2 != (void *)0x0) {
        pvVar2 = (void *)((int)pvVar2 + g_ModelRuntimeRebaseDelta);
      }
      armySlot2 = (ArmyRuntimeSlot *)0x0;
      if (armySlot1 != (ArmyRuntimeSlot *)0x0) {
        armySlot2 = (ArmyRuntimeSlot *)
                    ((int)&armySlot1->modelRuntimeOrSavedOffset +
                    (int)g_ArmyRuntimeRebaseBaseMinusOne);
      }
      (pSVar6->modelNodeOrSavedOffset).modelNode =
           (ModelRuntimeNode *)
           (g_RuntimeObjectRebaseBaseMinusOne +
           (int)(&((pSVar6->modelNodeOrSavedOffset).modelNode)->modelPayload + -1) + 0x30);
      (pSVar6->runtimeStateOrSavedOffset).runtimeStatePointer = pvVar2;
      (pSVar6->ownerAndTrajectory).ownerArmyRuntime = armySlot2;
      ppSVar5 = g_ShotDefinitionRegistry;
      iVar3 = 0x100;
      do {
        shotDefinition1 = *ppSVar5;
        if ((shotDefinition1 != (ShotDefinition *)0x0) &&
           ((pSVar6->definitionOrSavedId).definition ==
            (ShotDefinition *)shotDefinition1->definitionId))
        goto ShotRuntime_RebaseSlotsAfterLoad_CommitResolvedDefinitionAndAdvance;
        ppSVar5 = ppSVar5 + 1;
        iVar3 = iVar3 + -1;
      } while (iVar3 != 0);
      (pSVar6->modelNodeOrSavedOffset).modelNode = (ModelRuntimeNode *)0x0;
ShotRuntime_RebaseSlotsAfterLoad_CommitResolvedDefinitionAndAdvance:
      (pSVar6->definitionOrSavedId).definition = shotDefinition1;
    }
    pSVar6 = pSVar6 + 1;
    iVar4 = iVar4 + -1;
    if (iVar4 == 0) {
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
  ModelResourceHitTestAndRenderView210 *pMVar1;
  Q12 QVar2;
  ShotAnimationFrameAccumulatorQ4 SVar3;
  AngleTurn16Stored32 AVar4;
  ShotSecondaryEffectCountdownTicks SVar5;
  PackedArgb32 PVar6;
  short sVar7;
  short sVar8;
  short sVar9;
  short sVar10;
  ushort uVar11;
  ushort uVar12;
  ShotRuntimeSlot *pSVar13;
  GraphicsPaletteAsset *pGVar14;
  ShotModelRuntimeNodeClassView100 *shotModelNode;
  PackedArgb32 PVar15;
  ShotRuntimeSlot *pSVar16;
  Q12 runtimeLaunchSpeedQ12;
  Q12 worldXQ12;
  dword dVar17;
  ShotRuntimeSlot *shotRuntimeCursor;
  undefined1 uVar19;
  undefined1 uVar20;
  undefined8 uVar18;
  undefined1 uVar21;
  undefined1 uVar22;
  ShotLaunchAnglesEaxEdx8 SVar23;
  WorldObjectRecordEaxCf5 WVar24;
  ModelLookupEntryEaxCf5 MVar25;
  GraphicsShadingRuntimeRecordEaxCf5 GVar26;
  FixedDirectionXyzRegs12 FVar27;
  ModelLocalPointRegs12 MVar28;
  TerrainOccupancyResolvedMasksRegs12 TVar29;
  char runtimeClassIndex;
  
  pSVar16 = (ShotRuntimeSlot *)0x1000;
  shotRuntimeCursor = g_ShotRuntimeSlots;
  pSVar13 = g_ShotRuntimeSlots;
  while( true ) {
    if (pSVar13 == (ShotRuntimeSlot *)0x0) {
      return;
    }
    if ((shotRuntimeCursor->modelNodeOrSavedOffset).modelNode == (ModelRuntimeNode *)0x0) break;
    shotRuntimeCursor = shotRuntimeCursor + 1;
    pSVar16 = (ShotRuntimeSlot *)
              ((int)&pSVar16[-1].ownerAndTrajectory.secondaryEffectCountdownTicks + 3);
    pSVar13 = pSVar16;
  }
  WVar24 = WorldObjectArray_AllocateFreeRecordCf(worldRuntime);
  shotModelNode = (ShotModelRuntimeNodeClassView100 *)WVar24.recordOrError;
  if (WVar24.carry) {
    return;
  }
  WorldRuntime_LinkNodeIntoOwnerListD8((WorldOwnerListNode100 *)shotModelNode);
  (shotRuntimeCursor->modelNodeOrSavedOffset).modelNode = (ModelRuntimeNode *)shotModelNode;
  (shotRuntimeCursor->definitionOrSavedId).definition = shotDefinition;
  shotModelNode->ownerClassId = MODEL_RUNTIME_CLASS_01_GROUND;
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
  SVar23 = ShotDefinition_ComputeLaunchAnglesRegs
                     (targetWorldZQ12,targetWorldYQ12,targetWorldXQ12,launchWorldZQ12,
                      launchWorldYQ12,launchWorldXQ12,shotDefinition);
  (shotModelNode->modelPayload).worldRotationAngle0 = SVar23.headingAngle;
  (shotModelNode->modelPayload).worldRotationAngle1 = SVar23.elevationAngle;
  (shotModelNode->modelPayload).worldRotationAngle2 = SVar23.headingAngle;
  FVar27 = FixedMath_DirectionFromAnglesScaledRegs
                     (SVar23.elevationAngle,SVar23.headingAngle,shotDefinition->launchSpeedQ12);
  dVar17 = FVar27.edx;
  (shotRuntimeCursor->ownerAndTrajectory).directionComponent0Q12 = FVar27.eax;
  if (shotDefinition->trajectoryMode == SHOT_TRAJECTORY_BALLISTIC) {
    dVar17 = dVar17 + (shotDefinition->ballisticDivisorQ12 >> 1);
  }
  shotModelNode->renderDepthBiasOrState = 0;
  (shotRuntimeCursor->ownerAndTrajectory).directionComponent1Q12 = FVar27.ecx;
  (shotRuntimeCursor->ownerAndTrajectory).directionComponent2Q12 = dVar17;
  shotRuntimeCursor->projectileAgeTicks = 0;
  pGVar14 = g_ShotPalette;
  pMVar1 = shotDefinition->ownedNestedResource;
  (shotModelNode->modelPayload).textureSet = g_ShotTextureSet;
  QVar2 = pMVar1->boundingRadiusQ12;
  (shotModelNode->modelPayload).paletteAsset = pGVar14;
  shotModelNode->subtreeBoundingRadiusQ12 = QVar2;
  (shotModelNode->modelPayload).modelResource = pMVar1;
  SVar3 = shotDefinition->animationFrameAdvanceThresholdQ4;
  AVar4 = shotDefinition->elevationOffsetAngle16;
  (shotRuntimeCursor->ownerAndTrajectory).animationFrameIndex = 0;
  shotRuntimeCursor->impactEffectEmissionFlags = 0;
  shotRuntimeCursor->animationFrameAccumulatorQ4 = SVar3;
  (shotRuntimeCursor->runtimeStateOrSavedOffset).runtimeState = shotRuntimeState14;
  shotRuntimeCursor->elevationOffsetAngle16 = AVar4;
  (shotModelNode->modelPayload).meshGroupMask = 0xffffffff;
  SVar5 = shotDefinition->secondaryEffectIntervalTicks;
  shotModelNode->runtimeFlags = shotModelNode->runtimeFlags | 1;
  (shotRuntimeCursor->ownerAndTrajectory).secondaryEffectCountdownTicks = SVar5;
  shotModelNode->textureSubresourceBaseIndex = 0;
  shotModelNode->modelRuntimeLinkOrSavedOffset = (void *)0x0;
  MVar25 = ModelLookupTable_ContainsPackedKeyCf(0,4,shotDefinition->ownedNestedResource);
  if (MVar25.carry) {
    shotModelNode->shadingRecord = (GraphicsShadingRuntimeRecord *)0x0;
  }
  else {
    MVar28 = ModelNodeRuntime_TransformLocalPointRegs
                       (MVar25.entry,(ModelRuntimeNode *)shotModelNode);
    GVar26 = GraphicsShadingRuntime_AllocateRecordRegs
                       (shotDefinition->shadingTransitionDurationTicks,
                        (shotDefinition->shadingColorArgb >> 0x18) << 8,
                        shotDefinition->shadingColorArgb,MVar28.edx,MVar28.ecx,MVar28.eax);
    shotModelNode->shadingRecord = GVar26.record;
  }
  shotModelNode->parentNode = (ModelRuntimeNode *)0x0;
  shotModelNode->childCount = 0;
  runtimeClassIndex = (char)worldRuntime->activeFactionRuntimeIndex;
  dVar17 = TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                     (0x1000,(shotModelNode->worldTransform).translation.y,
                      (shotModelNode->worldTransform).translation.x,worldRuntime->fieldGrid);
  TVar29 = TerrainOccupancyMask_ResolveRuntimeClassFlags(0x10,0,dVar17,runtimeClassIndex);
  shotRuntimeCursor->terrainRuntimeClassState = TVar29.primaryOccupancyMask;
  shotModelNode->runtimeFlags = shotModelNode->runtimeFlags | TVar29.runtimeFlags | 0x10;
  PVar15 = UiNode_GetStateTintArgb((UiNodeBase *)shotModelNode);
  PVar6 = shotDefinition->stateTintArgb;
  uVar19 = (undefined1)(PVar15 >> 0x18);
  uVar11 = CONCAT11(uVar19,uVar19);
  uVar20 = (undefined1)(PVar15 >> 0x10);
  uVar19 = (undefined1)(PVar15 >> 8);
  uVar21 = (undefined1)(PVar6 >> 0x18);
  uVar12 = CONCAT11(uVar21,uVar21);
  uVar22 = (undefined1)(PVar6 >> 0x10);
  uVar21 = (undefined1)(PVar6 >> 8);
  uVar18 = pmulhw(CONCAT26(uVar11 >> 4,
                           CONCAT24((ushort)(CONCAT35(CONCAT21(uVar11,uVar20),
                                                      CONCAT14(uVar20,PVar15)) >> 0x20) >> 4,
                                    CONCAT22(CONCAT11(uVar19,uVar19) >> 4,
                                             CONCAT11((char)PVar15,(char)PVar15) >> 4))),
                  CONCAT26(uVar12 >> 4,
                           CONCAT24((ushort)(CONCAT35(CONCAT21(uVar12,uVar22),CONCAT14(uVar22,PVar6)
                                                     ) >> 0x20) >> 4,
                                    CONCAT22(CONCAT11(uVar21,uVar21) >> 4,
                                             CONCAT11((char)PVar6,(char)PVar6) >> 4))));
  sVar7 = (short)uVar18;
  sVar8 = (short)((ulonglong)uVar18 >> 0x10);
  sVar9 = (short)((ulonglong)uVar18 >> 0x20);
  sVar10 = (short)((ulonglong)uVar18 >> 0x30);
  shotModelNode->tintArgb =
       CONCAT13((0 < sVar10) * (sVar10 < 0x100) * (char)((ulonglong)uVar18 >> 0x30) -
                (0xff < sVar10),
                CONCAT12((0 < sVar9) * (sVar9 < 0x100) * (char)((ulonglong)uVar18 >> 0x20) -
                         (0xff < sVar9),
                         CONCAT11((0 < sVar8) * (sVar8 < 0x100) * (char)((ulonglong)uVar18 >> 0x10)
                                  - (0xff < sVar8),
                                  (0 < sVar7) * (sVar7 < 0x100) * (char)uVar18 - (0xff < sVar7))));
  ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)shotModelNode);
  ModelNodeRuntime_UpdateDepthBinMasks(0,(ModelRuntimeNode *)shotModelNode);
  MVar25 = ModelLookupTable_ContainsPackedKeyCf(0,3,shotDefinition->ownedNestedResource);
  if (!MVar25.carry) {
    MVar28 = ModelNodeRuntime_TransformLocalPointRegs
                       (MVar25.entry,(ModelRuntimeNode *)shotModelNode);
    worldXQ12 = MVar28.ecx;
    EffectRuntimePool_CreateInstanceFromDefinitionCf
              (EFFECT_RUNTIME_COMPLETION_NONE,(EffectRuntimeOwnerReference4)0x0,
               (shotModelNode->modelPayload).worldRotationAngle2,
               (shotModelNode->modelPayload).worldRotationAngle1,
               (shotModelNode->modelPayload).worldRotationAngle0,MVar28.edx,worldXQ12,MVar28.eax,
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

