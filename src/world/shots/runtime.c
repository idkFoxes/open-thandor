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
void ShotRuntime_ApplyArmyHitRelationAndNotifications
               (ArmyRuntimeSlot *targetArmyRuntime,ShotRuntimeSlot *shotRuntime)

{
  ArmyRuntimeSlot *armyRuntime;
  InGameSimulationTick IVar1;
  InGameRuntimeRootImageC3E4 *pIVar2;
  FactionRelationState FVar3;
  WorldRuntimeContext *worldRuntime;
  bool bVar4;
  ModelRuntimeScaleRatioRegisterPairQ12 MVar5;
  FactionNotificationCodeBase activeFactionCodeForFirst;
  FactionNotificationCodeBase activeFactionCodeForSecond;
  FactionRelationStateNibble stateFirstTowardSecond;
  FactionRelationStateNibble stateSecondTowardFirst;
  uint capabilityBitIndex;
  uint factionIndex;
  GameEntityRuntime *modelRuntimeHolder;
  
  armyRuntime = (shotRuntime->ownerAndTrajectory).ownerArmyRuntime;
  modelRuntimeHolder = targetArmyRuntime->linkedEntityRuntime;
  if (armyRuntime != (ArmyRuntimeSlot *)0x0) {
    if (((shotRuntime->definitionOrSavedId).definition)->targetClassImpactDamageQ12[0] < 0) {
      MVar5 = ModelRuntime_QueryHierarchyScaleRatioQ12Regs((ArmyRuntimeSlot *)modelRuntimeHolder);
      if ((((int)MVar5 == (int)(MVar5 >> 0x20)) && ((armyRuntime->commandModeFlags & 1) != 0)) &&
         (modelRuntimeHolder == (GameEntityRuntime *)armyRuntime->commandTargetArmyRuntime)) {
        ArmyRuntimeCommand_InterruptActiveTargetAndStampGeneration(armyRuntime);
        armyRuntime->commandGeneration = 1;
      }
    }
    else {
      factionIndex = armyRuntime->factionIndex;
      capabilityBitIndex = (modelRuntimeHolder->common).ownership.ownerIndex;
      g_GameDataAuxState.pairPressureMatrix8x8
      [(modelRuntimeHolder->common).ownership.ownerIndex * 8 + factionIndex] =
           g_GameDataAuxState.pairPressureMatrix8x8
           [(modelRuntimeHolder->common).ownership.ownerIndex * 8 + factionIndex] + 0x100;
      if (((factionIndex != 0) && (capabilityBitIndex != 0)) &&
         (bVar4 = factionIndex < capabilityBitIndex, factionIndex != capabilityBitIndex)) {
        GameFactionRuntime_TestCapabilityBitClearCf(capabilityBitIndex,factionIndex);
        pIVar2 = g_InGameRuntimeRoot;
        IVar1 = g_GameFactionRuntimeImage.tail.simulationTick;
        if (bVar4) {
          *(InGameSimulationTick *)(factionIndex * 0x740 + 0x50fa40 + capabilityBitIndex * 4) =
               g_GameFactionRuntimeImage.tail.simulationTick;
          *(InGameSimulationTick *)(capabilityBitIndex * 0x740 + 0x50fa40 + factionIndex * 4) =
               IVar1;
          GameFactionRuntime_UpdateImpactAlertAnchorAndNotify
                    (targetArmyRuntime,&pIVar2->worldRuntime0A30);
          ShotRuntime_PostImpactRelationNotificationNoOp(shotRuntime,worldRuntime);
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
void ShotRuntime_InitGraphicsResources(word *mutableBasePath)

{
  ShotRuntimeSlot *runtimeSlotCursor;
  dword arg0;
  int runtimeSlotsRemaining;
  dword arg1;
  undefined1 in_CF;
  undefined8 uVar1;
  
  WidePath_SetExtensionCode(0x786667,mutableBasePath);
  MoviePlayback_AdvanceScheduledFrameAndTick();
  (*(code *)g_GraphicsTextureSetLoadPackageCf)(mutableBasePath);
  if (!(bool)in_CF) {
    uVar1 = MoviePlayback_AdvanceScheduledFrameAndTick();
    g_ShotTextureSet = (GraphicsTextureSet *)uVar1;
    WidePath_SetExtensionCode(0x6c6170,mutableBasePath);
    (*g_GraphicsPaletteAssetLoadPackage)(arg0,arg1,mutableBasePath);
    if (!(bool)in_CF) {
      uVar1 = MoviePlayback_AdvanceScheduledFrameAndTick();
      g_ShotPalette = (GraphicsPaletteAsset *)uVar1;
      runtimeSlotCursor = (*g_MemoryApi.alloc)(0x40000);
      if (!(bool)in_CF) {
        g_ShotRuntimeRebaseBaseMinusOne =
             (byte *)((int)&runtimeSlotCursor[-1].ownerAndTrajectory.secondaryEffectCountdownTicks +
                     3);
        g_ShotRuntimeSlots = runtimeSlotCursor;
        for (runtimeSlotsRemaining = 0x10000; runtimeSlotsRemaining != 0;
            runtimeSlotsRemaining = runtimeSlotsRemaining + -1) {
          (runtimeSlotCursor->definitionOrSavedId).definition = (ShotDefinition *)0x0;
          runtimeSlotCursor = (ShotRuntimeSlot *)&runtimeSlotCursor->launchSpeedQ12;
        }
      }
    }
  }
  return;
}

/* Address: 0x0052B5C0.
   Ownership: world/shots/runtime.
   Purpose: Releases the shot runtime pool and graphics resources, releases owned definition resources, clears the
   registry, and has no semantic normal return.
   Cross-module calls: Resource_Release [assets/resource/runtime].
*/
void ShotRuntime_ShutdownGraphicsResources(void)

{
  int registrySlotsRemaining;
  int extraout_ECX;
  ShotDefinition **registryCursor;
  ShotDefinition *currentDefinition;
  
  (*g_MemoryApi.free)(g_ShotRuntimeSlots);
  g_ShotRuntimeSlots = (ShotRuntimeSlot *)0x0;
  if (g_ShotTextureSet != (GraphicsTextureSet *)0x0) {
    (*(code *)g_GraphicsTextureSetReleasePackageCf)(g_ShotTextureSet);
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
      registrySlotsRemaining = extraout_ECX;
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
ShotDefinition * ShotRuntime_FindDefinitionByIdCf(PckShotDefinitionIdCatalog definitionId)

{
  ShotDefinition *arg4;
  int registrySlotsRemaining;
  ShotDefinition **registryCursor;
  ShotDefinition *candidateDefinition;
  
  registryCursor = g_ShotDefinitionRegistry;
  registrySlotsRemaining = 0x100;
  while( true ) {
    arg4 = *registryCursor;
    if ((arg4 != (ShotDefinition *)0x0) && (arg4->definitionId == definitionId)) break;
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
    if (registrySlotsRemaining == 0) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(sdword)arg4,g_PackageLastErrorPath);
      return (ShotDefinition *)0x44;
    }
  }
  return arg4;
}

/* Address: 0x0052B750.
   Ownership: world/shots/runtime.
   Purpose: Rebases all 4096 live shot slots after load and replaces each saved definitionId with its registered
   ShotDefinition pointer. The 170 physical records and 136 unique ids include bank aliases; no fabricated per-
   alias gameplay meaning is assigned.
*/
void __cdecl ShotRuntime_RebaseSlotsAfterLoad(void)

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
                    ((int)&armySlot1->definitionOrAsset + (int)g_ArmyRuntimeRebaseBaseMinusOne);
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
undefined8
ShotRuntimePool_CreateProjectileFromDefinition
          (ShotRuntimeState14 shotRuntimeState14,ArmyRuntimeSlot *ownerArmyRuntime,
          Q12 targetWorldZQ12,Q12 targetWorldYQ12,Q12 targetWorldXQ12,Q12 launchWorldZQ12,
          Q12 launchWorldYQ12,Q12 launchWorldXQ12,ShotDefinition *shotDefinition,
          WorldRuntimeContext *worldRuntime)

{
  ModelResourceHitTestAndRenderView210 *pMVar1;
  ShotAnimationFrameAccumulatorQ4 SVar2;
  ArmyMovementStateFlags AVar3;
  ShotSecondaryEffectCountdownTicks SVar4;
  PackedArgb32 PVar5;
  short sVar6;
  short sVar7;
  short sVar8;
  short sVar9;
  ushort uVar10;
  ushort uVar11;
  ShotRuntimeSlot *pSVar12;
  GraphicsPaletteAsset *pGVar13;
  undefined4 in_EAX;
  WorldRuntimeNode *node;
  ModelRuntimeNode *modelNodeRuntime;
  ModelPackedPointRecord *pMVar14;
  uint uVar15;
  undefined4 uVar16;
  ShotRuntimeSlot *pSVar17;
  ModelRuntimeNode *modelNode1;
  ArmyCommandGeneration extraout_ECX;
  undefined4 extraout_ECX_00;
  GraphicsWorldCoordinateQ12 worldYQ12;
  GameEntityRuntime *extraout_ECX_01;
  undefined4 extraout_ECX_02;
  undefined4 extraout_ECX_03;
  undefined4 extraout_ECX_04;
  Q12 worldXQ12;
  undefined4 in_EDX;
  Q12 QVar18;
  undefined4 extraout_EDX;
  ArmyRuntimeSlot *armySlot1;
  bool bVar19;
  undefined1 uVar20;
  undefined1 uVar21;
  undefined1 uVar22;
  undefined1 uVar23;
  undefined1 uVar24;
  undefined8 uVar25;
  FixedDirectionXZEdxEax8 FVar26;
  longlong lVar27;
  ulonglong uVar28;
  char runtimeClassIndex;
  
  pSVar17 = (ShotRuntimeSlot *)0x1000;
  armySlot1 = (ArmyRuntimeSlot *)g_ShotRuntimeSlots;
  pSVar12 = g_ShotRuntimeSlots;
  do {
    if (pSVar12 == (ShotRuntimeSlot *)0x0) {
ShotRuntimePool_CreateProjectile_ReturnAllocationFailure:
      return CONCAT44(in_EDX,in_EAX);
    }
    bVar19 = false;
    if (((ShotModelNodeReferenceOrSavedOffset4 *)&armySlot1->movementControl)->modelNode ==
        (ModelRuntimeNode *)0x0) {
      node = (WorldRuntimeNode *)WorldObjectArray_AllocateFreeRecordCf(worldRuntime);
      if (!bVar19) {
        modelNodeRuntime = (ModelRuntimeNode *)WorldRuntime_LinkNodeIntoOwnerListD8(node);
        ((ShotModelNodeReferenceOrSavedOffset4 *)&armySlot1->movementControl)->modelNode =
             modelNodeRuntime;
        ((ShotDefinitionReferenceOrSavedId4 *)&armySlot1->definitionOrAsset)->definition =
             shotDefinition;
        modelNodeRuntime->ownerClassId = MODEL_RUNTIME_CLASS_01_GROUND;
        (modelNodeRuntime->runtimePayload).armyRuntime = armySlot1;
        (modelNodeRuntime->worldTransform).translation.x = launchWorldXQ12;
        (modelNodeRuntime->worldTransform).translation.y = launchWorldYQ12;
        (modelNodeRuntime->worldTransform).translation.z = launchWorldZQ12;
        modelNode1 = (ModelRuntimeNode *)shotDefinition->launchSpeedQ12;
        if (shotDefinition->trajectoryRampDurationTicks != 0) {
          modelNode1 = (ModelRuntimeNode *)0x0;
        }
        armySlot1->commandCoordinate1Q12 = shotDefinition->projectileLifetimeTicks;
        armySlot1->modelNodeRuntime = modelNode1;
        ((ShotRuntimeOwnerAndTrajectoryLiveState18 *)&armySlot1->commandCoordinate2Q12)->
        ownerArmyRuntime = ownerArmyRuntime;
        uVar25 = ShotDefinition_ComputeLaunchAnglesRegs
                           (targetWorldZQ12,targetWorldYQ12,targetWorldXQ12,launchWorldZQ12,
                            launchWorldYQ12,launchWorldXQ12,shotDefinition);
        (modelNodeRuntime->modelPayload).worldRotationAngle0 = (int)uVar25;
        (modelNodeRuntime->modelPayload).worldRotationAngle1 = (int)((ulonglong)uVar25 >> 0x20);
        (modelNodeRuntime->modelPayload).worldRotationAngle2 = (AngleTurn32)uVar25;
        FVar26 = FixedMath_DirectionFromAnglesScaledRegs
                           ((AngleTurn32)((ulonglong)uVar25 >> 0x20),(AngleTurn32)uVar25,
                            shotDefinition->launchSpeedQ12);
        QVar18 = (Q12)(FVar26 >> 0x20);
        armySlot1->commandModeFlags = (ArmyCommandModeFlags)FVar26;
        if (shotDefinition->trajectoryMode == SHOT_TRAJECTORY_BALLISTIC) {
          QVar18 = QVar18 + (shotDefinition->ballisticDivisorQ12 >> 1);
        }
        modelNodeRuntime->renderDepthBiasOrState = 0;
        armySlot1->commandGeneration = extraout_ECX;
        armySlot1->actionVector0Q12 = QVar18;
        armySlot1->commandCoordinate0Q12 = 0;
        pGVar13 = g_ShotPalette;
        pMVar1 = shotDefinition->ownedNestedResource;
        (modelNodeRuntime->modelPayload).textureSet = g_ShotTextureSet;
        QVar18 = pMVar1->boundingRadiusQ12;
        (modelNodeRuntime->modelPayload).paletteAsset = pGVar13;
        modelNodeRuntime->subtreeBoundingRadiusQ12 = QVar18;
        (modelNodeRuntime->modelPayload).modelResource = pMVar1;
        SVar2 = shotDefinition->animationFrameAdvanceThresholdQ4;
        AVar3 = shotDefinition->elevationOffsetAngle16;
        armySlot1->actionVector1Q12 = 0;
        armySlot1->commandTargetArmyRuntime = (ArmyRuntimeSlot *)0x0;
        armySlot1->factionIndex = SVar2;
        ((ShotModelRuntimeStateOrSavedOffset4 *)
        ((ShotModelNodeReferenceOrSavedOffset4 *)&armySlot1->movementControl + 1))->runtimeState =
             shotRuntimeState14;
        armySlot1->movementStateFlags = AVar3;
        (modelNodeRuntime->modelPayload).meshGroupMask = 0xffffffff;
        SVar4 = shotDefinition->secondaryEffectIntervalTicks;
        bVar19 = false;
        modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
        armySlot1->actionVector2Q12 = SVar4;
        modelNodeRuntime->textureSubresourceBaseIndex = 0;
        modelNodeRuntime->modelRuntimeLinkOrSavedOffset = (void *)0x0;
        pMVar14 = (ModelPackedPointRecord *)
                  ModelLookupTable_ContainsPackedKeyCf(0,4,shotDefinition->ownedNestedResource);
        if (bVar19) {
          modelNodeRuntime->shadingRecord = (GraphicsShadingRuntimeRecord *)0x0;
        }
        else {
          uVar25 = ModelNodeRuntime_TransformLocalPointRegs
                             (extraout_ECX_00,pMVar14,modelNodeRuntime);
          lVar27 = GraphicsShadingRuntime_AllocateRecordRegs
                             (shotDefinition->shadingTransitionDurationTicks,
                              (shotDefinition->shadingColorArgb >> 0x18) << 8,
                              shotDefinition->shadingColorArgb,
                              (GraphicsWorldCoordinateQ12)((ulonglong)uVar25 >> 0x20),worldYQ12,
                              (GraphicsWorldCoordinateQ12)uVar25);
          modelNodeRuntime->shadingRecord = (GraphicsShadingRuntimeRecord *)lVar27;
        }
        modelNodeRuntime->parentNode = (ModelRuntimeNode *)0x0;
        modelNodeRuntime->childCount = 0;
        runtimeClassIndex = (char)worldRuntime->activeFactionRuntimeIndex;
        uVar28 = TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                           (0x1000,(modelNodeRuntime->worldTransform).translation.y,
                            (modelNodeRuntime->worldTransform).translation.x,worldRuntime->fieldGrid
                           );
        uVar15 = TerrainOccupancyMask_ResolveRuntimeClassFlags
                           (0x10,0,(FieldGridRegionMask)(uVar28 >> 0x20),runtimeClassIndex);
        armySlot1->linkedEntityRuntime = extraout_ECX_01;
        uVar20 = 0;
        modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | uVar15 | 0x10;
        uVar16 = UiNode_GetStateTintArgb((UiNodeBase *)modelNodeRuntime);
        PVar5 = shotDefinition->stateTintArgb;
        uVar21 = (undefined1)((uint)uVar16 >> 0x18);
        uVar10 = CONCAT11(uVar21,uVar21);
        uVar22 = (undefined1)((uint)uVar16 >> 0x10);
        uVar21 = (undefined1)((uint)uVar16 >> 8);
        uVar23 = (undefined1)(PVar5 >> 0x18);
        uVar11 = CONCAT11(uVar23,uVar23);
        uVar24 = (undefined1)(PVar5 >> 0x10);
        uVar23 = (undefined1)(PVar5 >> 8);
        uVar25 = pmulhw(CONCAT26(uVar10 >> 4,
                                 CONCAT24((ushort)(CONCAT35(CONCAT21(uVar10,uVar22),
                                                            CONCAT14(uVar22,uVar16)) >> 0x20) >> 4,
                                          CONCAT22(CONCAT11(uVar21,uVar21) >> 4,
                                                   CONCAT11((char)uVar16,(char)uVar16) >> 4))),
                        CONCAT26(uVar11 >> 4,
                                 CONCAT24((ushort)(CONCAT35(CONCAT21(uVar11,uVar24),
                                                            CONCAT14(uVar24,PVar5)) >> 0x20) >> 4,
                                          CONCAT22(CONCAT11(uVar23,uVar23) >> 4,
                                                   CONCAT11((char)PVar5,(char)PVar5) >> 4))));
        sVar6 = (short)uVar25;
        sVar7 = (short)((ulonglong)uVar25 >> 0x10);
        sVar8 = (short)((ulonglong)uVar25 >> 0x20);
        sVar9 = (short)((ulonglong)uVar25 >> 0x30);
        modelNodeRuntime->tintArgb =
             CONCAT13((0 < sVar9) * (sVar9 < 0x100) * (char)((ulonglong)uVar25 >> 0x30) -
                      (0xff < sVar9),
                      CONCAT12((0 < sVar8) * (sVar8 < 0x100) * (char)((ulonglong)uVar25 >> 0x20) -
                               (0xff < sVar8),
                               CONCAT11((0 < sVar7) * (sVar7 < 0x100) *
                                        (char)((ulonglong)uVar25 >> 0x10) - (0xff < sVar7),
                                        (0 < sVar6) * (sVar6 < 0x100) * (char)uVar25 -
                                        (0xff < sVar6))));
        uVar25 = ModelNodeRuntime_RebuildTransformsFromRoot
                           (extraout_ECX_02,extraout_EDX,modelNodeRuntime);
        ModelNodeRuntime_UpdateDepthBinMasks
                  (extraout_ECX_03,(int)((ulonglong)uVar25 >> 0x20),0,modelNodeRuntime);
        pMVar14 = (ModelPackedPointRecord *)
                  ModelLookupTable_ContainsPackedKeyCf(0,3,shotDefinition->ownedNestedResource);
        if (!(bool)uVar20) {
          uVar25 = ModelNodeRuntime_TransformLocalPointRegs
                             (extraout_ECX_04,pMVar14,modelNodeRuntime);
          uVar16 = (undefined4)((ulonglong)uVar25 >> 0x20);
          EffectRuntimePool_CreateInstanceFromDefinitionCf
                    (worldXQ12,uVar16,EFFECT_RUNTIME_COMPLETION_NONE,0,
                     (modelNodeRuntime->modelPayload).worldRotationAngle2,
                     (modelNodeRuntime->modelPayload).worldRotationAngle1,
                     (modelNodeRuntime->modelPayload).worldRotationAngle0,uVar16,worldXQ12,
                     (Q12)uVar25,shotDefinition->launchEffectDefinition,worldRuntime);
        }
        return CONCAT44(in_EDX,in_EAX);
      }
      goto ShotRuntimePool_CreateProjectile_ReturnAllocationFailure;
    }
    armySlot1 = (ArmyRuntimeSlot *)&armySlot1->runtimeState40;
    pSVar17 = (ShotRuntimeSlot *)
              ((int)&pSVar17[-1].ownerAndTrajectory.secondaryEffectCountdownTicks + 3);
    pSVar12 = pSVar17;
  } while( true );
}

/* Address: 0x00514710.
   Ownership: world/shots/runtime.
   Purpose: Exact two-argument post-impact relation hook. The archived implementation preserves the normal
   registers, performs no state change, and returns with RET 0x08.
*/
void ShotRuntime_PostImpactRelationNotificationNoOp
               (ShotRuntimeSlot *shotRuntime,WorldRuntimeContext *worldRuntime)

{
  return;
}
