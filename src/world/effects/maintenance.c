#include <thandor/world/effects/maintenance.h>

/* Implementation ownership: world/effects/maintenance. */

/* Address: 0x0051E790.
   Ownership: world/effects/maintenance.
   Purpose: Binary entry is anchored by g_ArmyRuntimeCallbackTable12[5]@00562DEC. Maintenance table phase
   terrainStateRefresh, object kind effect. The 4x3 table bytes, target body, calling convention, and RET 0x08
   contract remain unchanged.
   Cross-module calls: TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint [world/terrain/occupancy],
   TerrainOccupancyMask_ResolveRuntimeClassFlags [world/terrain/occupancy], UiModelControl_RefreshStateTint
   [ui/controls/misc].
*/
void EffectRuntimeMaintenance_RefreshOccupancyFlagsAndTint
               (WorldRuntimeContext *worldRuntime,ModelRuntimeNode *modelNodeRuntime)

{
  Q12 QVar1;
  undefined4 uVar2;
  short sVar3;
  short sVar4;
  short sVar5;
  short sVar6;
  ushort uVar7;
  ushort uVar8;
  uint uVar9;
  Q12 extraout_ECX;
  undefined1 mm0PackedValue0ByteLane1;
  undefined1 mm0PackedValue0ByteLane2;
  undefined8 mm0PackedValue0;
  undefined1 uVar10;
  undefined1 uVar11;
  ulonglong uVar12;
  ArmyRuntimeSlot *armySlot1;
  
  uVar12 = TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                     (0x1000,(modelNodeRuntime->worldTransform).translation.y,
                      (modelNodeRuntime->worldTransform).translation.x,worldRuntime->fieldGrid);
  modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags & 0xfffffff3;
  armySlot1 = (modelNodeRuntime->runtimePayload).armyRuntime;
  uVar9 = TerrainOccupancyMask_ResolveRuntimeClassFlags
                    (modelNodeRuntime->runtimeFlags,0,(FieldGridRegionMask)(uVar12 >> 0x20),
                     (char)worldRuntime->activeFactionRuntimeIndex);
  modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | uVar9;
  armySlot1->actionVector0Q12 = extraout_ECX;
  UiModelControl_RefreshStateTint(modelNodeRuntime);
  if ((modelNodeRuntime->tintArgb & 0xff000000) != 0) {
    QVar1 = armySlot1->actionVector1Q12;
    uVar2 = *(undefined4 *)((int)armySlot1->definitionOrAsset + 0x60);
    mm0PackedValue0ByteLane1 = (undefined1)((uint)QVar1 >> 0x18);
    uVar7 = CONCAT11(mm0PackedValue0ByteLane1,mm0PackedValue0ByteLane1);
    mm0PackedValue0ByteLane2 = (undefined1)((uint)QVar1 >> 0x10);
    mm0PackedValue0ByteLane1 = (undefined1)((uint)QVar1 >> 8);
    uVar10 = (undefined1)((uint)uVar2 >> 0x18);
    uVar8 = CONCAT11(uVar10,uVar10);
    uVar11 = (undefined1)((uint)uVar2 >> 0x10);
    uVar10 = (undefined1)((uint)uVar2 >> 8);
    mm0PackedValue0 =
         pmulhw(CONCAT26(uVar7 >> 4,
                         CONCAT24((ushort)(CONCAT35(CONCAT21(uVar7,mm0PackedValue0ByteLane2),
                                                    CONCAT14(mm0PackedValue0ByteLane2,QVar1)) >>
                                          0x20) >> 4,
                                  CONCAT22(CONCAT11(mm0PackedValue0ByteLane1,
                                                    mm0PackedValue0ByteLane1) >> 4,
                                           CONCAT11((char)QVar1,(char)QVar1) >> 4))),
                CONCAT26(uVar8 >> 4,
                         CONCAT24((ushort)(CONCAT35(CONCAT21(uVar8,uVar11),CONCAT14(uVar11,uVar2))
                                          >> 0x20) >> 4,
                                  CONCAT22(CONCAT11(uVar10,uVar10) >> 4,
                                           CONCAT11((char)uVar2,(char)uVar2) >> 4))));
    sVar3 = (short)mm0PackedValue0;
    sVar4 = (short)((ulonglong)mm0PackedValue0 >> 0x10);
    sVar5 = (short)((ulonglong)mm0PackedValue0 >> 0x20);
    sVar6 = (short)((ulonglong)mm0PackedValue0 >> 0x30);
    modelNodeRuntime->tintArgb =
         CONCAT13((0 < sVar6) * (sVar6 < 0x100) * (char)((ulonglong)mm0PackedValue0 >> 0x30) -
                  (0xff < sVar6),
                  CONCAT12((0 < sVar5) * (sVar5 < 0x100) *
                           (char)((ulonglong)mm0PackedValue0 >> 0x20) - (0xff < sVar5),
                           CONCAT11((0 < sVar4) * (sVar4 < 0x100) *
                                    (char)((ulonglong)mm0PackedValue0 >> 0x10) - (0xff < sVar4),
                                    (0 < sVar3) * (sVar3 < 0x100) * (char)mm0PackedValue0 -
                                    (0xff < sVar3))));
  }
  return;
}

/* Address: 0x0051E830.
   Ownership: world/effects/maintenance.
   Purpose: Exact two-argument no-op installed in the effect column of the unified model, shot, and effect method
   table. It returns with ret 0x08. Maintenance table phase occupancyRebuild, object kind effect. The 4x3 table
   bytes, target body, calling convention, and RET 0x08 contract remain unchanged.
*/
void EffectRuntimeMaintenance_OccupancyRebuildNoOp
               (WorldRuntimeContext *worldRuntime,void *runtimeObject)

{
  return;
}

/* Address: 0x0051E840.
   Ownership: world/effects/maintenance.
   Purpose: Second exact two-argument no-op installed in the effect column of the unified model, shot, and effect
   method table. It returns with ret 0x08. Maintenance table phase audioRefresh, object kind effect. The 4x3 table
   bytes, target body, calling convention, and RET 0x08 contract remain unchanged.
*/
void EffectRuntimeMaintenance_AudioRefreshNoOp
               (WorldRuntimeContext *worldRuntime,void *runtimeObject)

{
  return;
}

/* Address: 0x0051E850.
   Ownership: world/effects/maintenance.
   Purpose: Maintenance slot 2 receives WorldRuntimeContext and ModelRuntimeNode, advances the effect runtime
   payload lifecycle, animation/tint state, and effect transitions. The exact seven-range body, three RET 0x08
   terminals, and five-entry switch table are sealed against the immutable original binary. Conservative slot
   identity is retained until subtype fields are closed. Advances effect lifecycle timers and animation, updates
   shading ownership, tint and scale interpolation, and dispatches the sealed effect transition modes. Maintenance
   table phase primaryUpdate, object kind effect.
   Cross-module calls: InterpolationState_SetNegatedTargetAndRescaleProgress [core/math/interpolation],
   WorldRuntime_UnlinkNodeFromOwnerListD8 [world/runtime/core], ModelLookupTable_ContainsPackedKeyCf
   [assets/model/definitions], ModelNodeRuntime_TransformLocalPointRegs [world/model/hierarchy],
   GraphicsShadingRuntime_AllocateRecordRegs [graphics/render/shading],
   EffectRuntimePool_CreateInstanceFromDefinitionCf [world/effects/runtime].
*/
void EffectModelRuntimeMaintenance_UpdateLifecycleTintScaleAndTransitions
               (WorldRuntimeContext *worldRuntime,EffectModelRuntimeNodeClassView100 *modelNode)

{
  EffectAnimationFrameCount *pEVar1;
  EffectShadingCountdownTicks *pEVar2;
  EffectPeriodicIntervalTicks *pEVar3;
  DefinitionReferencePresentFlag *pDVar4;
  dword *pdVar5;
  AngleTurn32 *pAVar6;
  GraphicsWorldCoordinateQ12 *pGVar7;
  GraphicsFixedVec3 *pGVar8;
  EffectRuntimeSlot *pEVar9;
  EffectAlphaFadeTicks EVar10;
  EffectAlphaFadeTicks EVar11;
  PackedArgb32 PVar12;
  PackedArgb32 PVar13;
  EffectRuntimeCompletionAction EVar14;
  int *piVar15;
  dword keyIndex;
  AngleTurn32 AVar16;
  short sVar17;
  short sVar18;
  short sVar19;
  short sVar20;
  ushort uVar21;
  ushort uVar22;
  uint uVar23;
  ModelPackedPointRecord *pMVar24;
  sdword sVar25;
  uint uVar26;
  ArmyRuntimeSlot *armySlot1;
  undefined4 extraout_ECX;
  GraphicsWorldCoordinateQ12 worldYQ12;
  uint uVar27;
  undefined4 extraout_ECX_00;
  Q12 worldXQ12;
  undefined4 extraout_ECX_01;
  undefined4 worldXQ12_00;
  uint extraout_ECX_02;
  undefined4 extraout_ECX_03;
  int launchWorldYQ12;
  undefined4 extraout_ECX_04;
  undefined4 worldXQ12_01;
  int extraout_ECX_05;
  int extraout_ECX_06;
  int iVar28;
  int extraout_EDX;
  Q12 worldZQ12;
  undefined4 uVar29;
  GameEntityRuntime *spawnArmyCompletionEntity;
  GameEntityRuntime *entityRuntime;
  EffectCompletionLinkedHandlerOwnerColumns104 *linkedHandlerCompletionOwner;
  void *completionOwnerCarrier;
  bool bVar30;
  undefined1 uVar31;
  undefined1 uVar32;
  undefined1 uVar33;
  undefined1 uVar34;
  undefined8 uVar35;
  longlong lVar36;
  qword qVar37;
  FixedDirectionXZEdxEax8 FVar38;
  EffectDefinition *effectDefinition;
  WorldRuntimeContext *worldRuntime_00;
  InGameSimulationStepBatchTicks IStack_20;
  EffectDefinition *effectDefinition1;
  ModelRuntimeNode *modelNode1;
  GraphicsShadingRuntimeRecord *shadingRecord1;
  
  IStack_20 = g_InGameSimulationStepTicks;
  do {
    pEVar9 = modelNode->effectRuntime;
    effectDefinition1 = (pEVar9->definitionOrSavedId).definition;
    uVar23 = (pEVar9->lifecycleOwnerAndDefinition).animationFrameAccumulatorQ4 + 0x10;
    iVar28 = 0;
    (pEVar9->lifecycleOwnerAndDefinition).animationFrameAccumulatorQ4 = uVar23;
    if (effectDefinition1->frameAdvanceThresholdQ4 <= uVar23) {
      (pEVar9->lifecycleOwnerAndDefinition).animationFrameAccumulatorQ4 =
           uVar23 - effectDefinition1->frameAdvanceThresholdQ4;
      iVar28 = 1;
      pEVar1 = &pEVar9->animationFramesRemaining;
      *pEVar1 = *pEVar1 - 1;
      if (*pEVar1 == 0) {
        InterpolationState_SetNegatedTargetAndRescaleProgress
                  (effectDefinition1->shadingReleaseTransitionDurationTicks,modelNode->shadingRecord
                  );
        WorldRuntime_UnlinkNodeFromOwnerListD8((WorldRuntimeNode *)modelNode);
        (pEVar9->modelNodeOrSavedOffset).modelNode = (ModelRuntimeNode *)0x0;
        return;
      }
    }
    if (iVar28 != 0) {
      modelNode->textureSubresourceBaseIndex = modelNode->textureSubresourceBaseIndex + 1;
      pEVar2 = &pEVar9->shadingStartCountdownTicksRemaining;
      *pEVar2 = *pEVar2 - 1;
      if (((*pEVar2 == 0) &&
          (bVar30 = false, modelNode->shadingRecord == (GraphicsShadingRuntimeRecord *)0x0)) &&
         (pMVar24 = (ModelPackedPointRecord *)
                    ModelLookupTable_ContainsPackedKeyCf(0,4,effectDefinition1->ownedNestedResource)
         , !bVar30)) {
        uVar35 = ModelNodeRuntime_TransformLocalPointRegs
                           (extraout_ECX,pMVar24,(ModelRuntimeNode *)modelNode);
        lVar36 = GraphicsShadingRuntime_AllocateRecordRegs
                           (effectDefinition1->shadingTransitionDurationTicks,
                            (effectDefinition1->shadingColorArgb >> 0x18) << 8,
                            effectDefinition1->shadingColorArgb,
                            (GraphicsWorldCoordinateQ12)((ulonglong)uVar35 >> 0x20),worldYQ12,
                            (GraphicsWorldCoordinateQ12)uVar35);
        modelNode->shadingRecord = (GraphicsShadingRuntimeRecord *)lVar36;
      }
      pEVar2 = &pEVar9->shadingStopCountdownTicksRemaining;
      *pEVar2 = *pEVar2 - 1;
      if ((*pEVar2 == 0) && (modelNode->shadingRecord != (GraphicsShadingRuntimeRecord *)0x0)) {
        InterpolationState_SetNegatedTargetAndRescaleProgress
                  (effectDefinition1->shadingReleaseTransitionDurationTicks,modelNode->shadingRecord
                  );
        modelNode->shadingRecord = (GraphicsShadingRuntimeRecord *)0x0;
        iVar28 = extraout_EDX;
      }
    }
    if ((effectDefinition1->transitionPrefix).transitionKind !=
        EFFECT_TRANSITION_ADVANCE_TERRAIN_RELATIVE_MOTION_AND_TERMINATE_ON_CONTACT) {
      uVar23 = pEVar9->effectAgeTicks;
      uVar27 = (effectDefinition1->animationFrameCount * effectDefinition1->frameAdvanceThresholdQ4
               >> 4) - effectDefinition1->alphaFadeOutTicks;
      if (uVar23 < effectDefinition1->alphaFadeInTicks) {
        EVar10 = effectDefinition1->alphaFadeInTicks;
        pEVar9->stateTintArgb = pEVar9->stateTintArgb & 0xffffff;
        pEVar9->stateTintArgb =
             pEVar9->stateTintArgb |
             (int)(((longlong)(int)uVar23 * 0xff) / (longlong)(int)EVar10) << 0x18;
      }
      else if ((uVar27 < uVar23) && (0 < (int)effectDefinition1->alphaFadeOutTicks)) {
        EVar10 = effectDefinition1->alphaFadeOutTicks;
        EVar11 = effectDefinition1->alphaFadeOutTicks;
        pEVar9->stateTintArgb = pEVar9->stateTintArgb & 0xffffff;
        pEVar9->stateTintArgb =
             pEVar9->stateTintArgb |
             (int)(((longlong)(int)((uVar23 - uVar27) - EVar10) * -0xff) / (longlong)(int)EVar11) <<
             0x18;
      }
      else {
        pEVar9->stateTintArgb = pEVar9->stateTintArgb | 0xff000000;
      }
      if ((modelNode->runtimeFlags & 4) == 0) {
        modelNode->tintArgb = 0xffffff;
      }
      else {
        PVar12 = pEVar9->stateTintArgb;
        PVar13 = effectDefinition1->stateTintArgb;
        uVar31 = (undefined1)(PVar12 >> 0x18);
        uVar21 = CONCAT11(uVar31,uVar31);
        uVar32 = (undefined1)(PVar12 >> 0x10);
        uVar31 = (undefined1)(PVar12 >> 8);
        uVar33 = (undefined1)(PVar13 >> 0x18);
        uVar22 = CONCAT11(uVar33,uVar33);
        uVar34 = (undefined1)(PVar13 >> 0x10);
        uVar33 = (undefined1)(PVar13 >> 8);
        uVar35 = pmulhw(CONCAT26(uVar21 >> 4,
                                 CONCAT24((ushort)(CONCAT35(CONCAT21(uVar21,uVar32),
                                                            CONCAT14(uVar32,PVar12)) >> 0x20) >> 4,
                                          CONCAT22(CONCAT11(uVar31,uVar31) >> 4,
                                                   CONCAT11((char)PVar12,(char)PVar12) >> 4))),
                        CONCAT26(uVar22 >> 4,
                                 CONCAT24((ushort)(CONCAT35(CONCAT21(uVar22,uVar34),
                                                            CONCAT14(uVar34,PVar13)) >> 0x20) >> 4,
                                          CONCAT22(CONCAT11(uVar33,uVar33) >> 4,
                                                   CONCAT11((char)PVar13,(char)PVar13) >> 4))));
        sVar17 = (short)uVar35;
        sVar18 = (short)((ulonglong)uVar35 >> 0x10);
        sVar19 = (short)((ulonglong)uVar35 >> 0x20);
        sVar20 = (short)((ulonglong)uVar35 >> 0x30);
        modelNode->tintArgb =
             CONCAT13((0 < sVar20) * (sVar20 < 0x100) * (char)((ulonglong)uVar35 >> 0x30) -
                      (0xff < sVar20),
                      CONCAT12((0 < sVar19) * (sVar19 < 0x100) * (char)((ulonglong)uVar35 >> 0x20) -
                               (0xff < sVar19),
                               CONCAT11((0 < sVar18) * (sVar18 < 0x100) *
                                        (char)((ulonglong)uVar35 >> 0x10) - (0xff < sVar18),
                                        (0 < sVar17) * (sVar17 < 0x100) * (char)uVar35 -
                                        (0xff < sVar17))));
      }
    }
    uVar31 = false;
    if ((modelNode->runtimeFlags & 0x800) != 0) {
      uVar23 = (uint)(((longlong)
                       (effectDefinition1->modelScaleEndQ12 - effectDefinition1->modelScaleStartQ12)
                      * (longlong)(int)pEVar9->effectAgeTicks) /
                     (longlong)
                     (int)(effectDefinition1->animationFrameCount *
                           effectDefinition1->frameAdvanceThresholdQ4 >> 4));
      uVar31 = CARRY4(uVar23,effectDefinition1->modelScaleStartQ12);
      modelNode->modelScaleQ12 = uVar23 + effectDefinition1->modelScaleStartQ12;
    }
    pEVar9->effectAgeTicks = pEVar9->effectAgeTicks + 1;
    pEVar3 = &pEVar9->periodicEffectCountdownTicks;
    *pEVar3 = *pEVar3 - 1;
    if (*pEVar3 == 0) {
      pEVar9->periodicEffectCountdownTicks = effectDefinition1->periodicEffectIntervalTicks;
      pMVar24 = (ModelPackedPointRecord *)
                ModelLookupTable_ContainsPackedKeyCf(1,3,effectDefinition1->ownedNestedResource);
      if (!(bool)uVar31) {
        effectDefinition = effectDefinition1->periodicEffectDefinition;
        worldRuntime_00 = worldRuntime;
        uVar35 = ModelNodeRuntime_TransformLocalPointRegs
                           (extraout_ECX_00,pMVar24,(ModelRuntimeNode *)modelNode);
        worldZQ12 = (Q12)((ulonglong)uVar35 >> 0x20);
        EffectRuntimePool_CreateInstanceFromDefinitionCf
                  (worldXQ12,worldZQ12,EFFECT_RUNTIME_COMPLETION_NONE,0,0,0x4000,0,worldZQ12,
                   worldXQ12,(Q12)uVar35,effectDefinition,worldRuntime_00);
      }
    }
                    
    switch((effectDefinition1->transitionPrefix).transitionKind) {
    case EFFECT_TRANSITION_SPAWN_LINKED_EFFECT_AFTER_COUNTDOWN:
      bVar30 = false;
      if (iVar28 != 0) {
        pDVar4 = &pEVar9->linkedEffectPresent;
        *pDVar4 = *pDVar4 - 1;
        if ((*pDVar4 == 0) &&
           (pMVar24 = (ModelPackedPointRecord *)
                      ModelLookupTable_ContainsPackedKeyCf
                                (0,3,effectDefinition1->ownedNestedResource), !bVar30)) {
          uVar35 = ModelNodeRuntime_TransformLocalPointRegs
                             (extraout_ECX_01,pMVar24,(ModelRuntimeNode *)modelNode);
          uVar29 = (undefined4)((ulonglong)uVar35 >> 0x20);
          EffectRuntimePool_CreateInstanceFromDefinitionCf
                    (worldXQ12_00,uVar29,EFFECT_RUNTIME_COMPLETION_NONE,0,
                     (modelNode->modelPayload).worldRotationAngle2,
                     (modelNode->modelPayload).worldRotationAngle1,
                     (modelNode->modelPayload).worldRotationAngle0,uVar29,worldXQ12_00,(Q12)uVar35,
                     effectDefinition1->linkedEffectDefinition,worldRuntime);
        }
      }
      break;
    case EFFECT_TRANSITION_ADVANCE_PERIODIC_EMISSION_AND_COMPLETION_ACTION:
      uVar31 = false;
      if (iVar28 != 0) {
        pDVar4 = &pEVar9->linkedShotPresent;
        *pDVar4 = *pDVar4 - 1;
        if (*pDVar4 == 0) {
          keyIndex = (pEVar9->lifecycleOwnerAndDefinition).runtimeState14;
          pEVar9->linkedShotPresent = effectDefinition1->linkedShotPresent;
          (pEVar9->lifecycleOwnerAndDefinition).runtimeState14 =
               (pEVar9->lifecycleOwnerAndDefinition).runtimeState14 + 1;
          pMVar24 = (ModelPackedPointRecord *)
                    ModelLookupTable_ContainsPackedKeyCf
                              (keyIndex,2,effectDefinition1->ownedNestedResource);
          if (!(bool)uVar31) {
            uVar35 = ModelNodeRuntime_TransformLocalPointRegs
                               (extraout_ECX_03,pMVar24,(ModelRuntimeNode *)modelNode);
            iVar28 = (int)((ulonglong)uVar35 >> 0x20);
            uVar23 = (iVar28 - (modelNode->worldTransform).translation.z) * 2;
            pGVar7 = &(modelNode->worldTransform).translation.z;
            uVar31 = CARRY4(uVar23,*pGVar7);
            ShotRuntimePool_CreateProjectileFromDefinition
                      (0,(ArmyRuntimeSlot *)0x0,uVar23 + *pGVar7,
                       (launchWorldYQ12 - (modelNode->worldTransform).translation.y) * 2 +
                       (modelNode->worldTransform).translation.y,
                       ((int)uVar35 - (modelNode->worldTransform).translation.x) * 2 +
                       (modelNode->worldTransform).translation.x,iVar28,launchWorldYQ12,(int)uVar35,
                       effectDefinition1->linkedShotDefinition,worldRuntime);
          }
        }
        pDVar4 = &pEVar9->linkedEffectPresent;
        *pDVar4 = *pDVar4 - 1;
        if ((*pDVar4 == 0) &&
           (pMVar24 = (ModelPackedPointRecord *)
                      ModelLookupTable_ContainsPackedKeyCf
                                (0,3,effectDefinition1->ownedNestedResource), !(bool)uVar31)) {
          uVar35 = ModelNodeRuntime_TransformLocalPointRegs
                             (extraout_ECX_04,pMVar24,(ModelRuntimeNode *)modelNode);
          uVar29 = (undefined4)((ulonglong)uVar35 >> 0x20);
          EffectRuntimePool_CreateInstanceFromDefinitionCf
                    (worldXQ12_01,uVar29,EFFECT_RUNTIME_COMPLETION_NONE,0,
                     (modelNode->modelPayload).worldRotationAngle2,
                     (modelNode->modelPayload).worldRotationAngle1,
                     (modelNode->modelPayload).worldRotationAngle0,uVar29,worldXQ12_01,(Q12)uVar35,
                     effectDefinition1->linkedEffectDefinition,worldRuntime);
        }
        pdVar5 = &(pEVar9->lifecycleOwnerAndDefinition).ownerAndDefinition.runtimeValue24;
        *pdVar5 = *pdVar5 - 1;
        if (*pdVar5 == 0) {
          EVar14 = pEVar9->completionAction;
          completionOwnerCarrier =
               (pEVar9->lifecycleOwnerAndDefinition).ownerAndDefinition.owner.modelNode;
          if (EVar14 == EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY) {
EffectModelRuntimeMaintenance_TransitionType1DestroyModel:
            if (completionOwnerCarrier != (void *)0x0) {
              ModelRuntimePool_DestroyHierarchyAndDetach(worldRuntime,completionOwnerCarrier);
            }
          }
          else {
            spawnArmyCompletionEntity = completionOwnerCarrier;
            if (EVar14 == EFFECT_RUNTIME_COMPLETION_SPAWN_ARMY_FROM_MODEL) {
EffectModelRuntimeMaintenance_TransitionType3SpawnArmy:
              if (spawnArmyCompletionEntity != (GameEntityRuntime *)0x0) {
                piVar15 = (spawnArmyCompletionEntity->common).ownership.definitionOrClassRecord;
                modelNode1 = (spawnArmyCompletionEntity->common).ownership.modelNode;
                iVar28 = *piVar15;
                bVar30 = *(uint *)(iVar28 + 0x4c) < 0x12;
                if ((*(uint *)(iVar28 + 0x4c) == 0x12) &&
                   (armySlot1 = ArmyRuntime_CreateInstanceFromAssetCf
                                          (6,(modelNode1->modelPayload).worldRotationAngle2,
                                           (modelNode1->worldTransform).translation.y,
                                           (modelNode1->worldTransform).translation.x,
                                           (spawnArmyCompletionEntity->common).ownership.ownerIndex,
                                           *(PckArmyAssetIdCatalog *)(iVar28 + 0xc0),worldRuntime),
                   !bVar30)) {
                  *(int *)((int)armySlot1->definitionOrAsset + 0x3c) =
                       (int)(((longlong)piVar15[0xf] *
                             (longlong)*(int *)(*(int *)armySlot1->definitionOrAsset + 0x60)) /
                            (longlong)*(int *)(iVar28 + 0x60));
                  ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,entityRuntime);
                }
              }
            }
            else {
              linkedHandlerCompletionOwner = completionOwnerCarrier;
              if (EVar14 == EFFECT_RUNTIME_COMPLETION_INVOKE_LINKED_HANDLER)
              goto EffectModelRuntimeMaintenance_TransitionType2InvokeLinkedHandler;
            }
          }
        }
      }
      break;
    case EFFECT_TRANSITION_INTEGRATE_LINEAR_MOTION_AND_SHADING_POSITION:
      FVar38 = FixedMath_DirectionFromAnglesScaledRegs
                         ((modelNode->modelPayload).worldRotationAngle1,
                          (modelNode->modelPayload).worldRotationAngle0,
                          effectDefinition1->movementSpeedQ12);
      iVar28 = (int)(FVar38 >> 0x20);
      AVar16 = (modelNode->modelPayload).worldRotationAngle1;
      shadingRecord1 = modelNode->shadingRecord;
      pGVar8 = &(modelNode->worldTransform).translation;
      pGVar8->x = pGVar8->x + (int)FVar38;
      pGVar7 = &(modelNode->worldTransform).translation.y;
      *pGVar7 = *pGVar7 + extraout_ECX_05;
      pGVar7 = &(modelNode->worldTransform).translation.z;
      *pGVar7 = *pGVar7 + iVar28;
      uVar35 = ModelNodeRuntime_RebuildTransformsFromRoot
                         (extraout_ECX_05,iVar28,(ModelRuntimeNode *)modelNode);
      (modelNode->modelPayload).worldRotationAngle1 = (int)(AVar16 * 0x3f + 0x4000) >> 6;
      if (shadingRecord1 != (GraphicsShadingRuntimeRecord *)0x0) {
        shadingRecord1->worldXQ12 = shadingRecord1->worldXQ12 + (int)uVar35;
        shadingRecord1->worldYQ12 = shadingRecord1->worldYQ12 + extraout_ECX_06;
        shadingRecord1->worldZQ12 = shadingRecord1->worldZQ12 + (int)((ulonglong)uVar35 >> 0x20);
      }
      break;
    case EFFECT_TRANSITION_ADVANCE_TERRAIN_RELATIVE_MOTION_AND_TERMINATE_ON_CONTACT:
      qVar37 = FieldGrid_InterpolateTerrainHeightAndNormal
                         ((modelNode->worldTransform).translation.y,
                          (modelNode->worldTransform).translation.x,worldRuntime->fieldGrid);
      uVar23 = (uint)(qVar37 >> 0x20);
      if (!(bool)uVar31) {
        FixedMath_DirectionFromAnglesScaledRegs((int)uVar23 >> 0x10,uVar23 & 0xffff,0x10000000);
        FixedMath_DirectionFromAnglesScaledRegs
                  ((modelNode->modelPayload).worldRotationAngle1 -
                   pEVar9->effectAgeTicks * pEVar9->effectAgeTicks * effectDefinition1->unknown58,
                   (modelNode->modelPayload).worldRotationAngle0,0x10000000);
        sVar25 = FixedVec3_DotQ28((GraphicsFixedVec3 *)&stack0xffffffd4,
                                  (GraphicsFixedVec3 *)&stack0xffffffc8);
        if (sVar25 < 0) {
          if (pEVar9->stateTintArgb < 0x1000000) {
            InterpolationState_SetNegatedTargetAndRescaleProgress
                      (effectDefinition1->shadingReleaseTransitionDurationTicks,
                       modelNode->shadingRecord);
            WorldRuntime_UnlinkNodeFromOwnerListD8((WorldRuntimeNode *)modelNode);
            (pEVar9->modelNodeOrSavedOffset).modelNode = (ModelRuntimeNode *)0x0;
            return;
          }
          uVar23 = effectDefinition1->unknown5C;
          uVar27 = pEVar9->stateTintArgb >> 0x18;
          iVar28 = uVar27 - uVar23;
          if (uVar27 < uVar23) {
            iVar28 = 0;
          }
          uVar27 = iVar28 << 0x18;
          uVar26 = pEVar9->stateTintArgb & 0xffffff | uVar27;
        }
        else {
          uVar26 = 0xffffffff;
          uVar23 = pEVar9->effectAgeTicks * pEVar9->effectAgeTicks * effectDefinition1->unknown58;
          pAVar6 = &(modelNode->modelPayload).worldRotationAngle1;
          *pAVar6 = *pAVar6 - uVar23;
          uVar27 = extraout_ECX_02;
        }
        pEVar9->stateTintArgb = uVar26;
        if ((modelNode->runtimeFlags & 4) != 0) {
          PVar12 = effectDefinition1->stateTintArgb;
          uVar31 = (undefined1)(uVar26 >> 0x18);
          uVar21 = CONCAT11(uVar31,uVar31);
          uVar32 = (undefined1)(uVar26 >> 0x10);
          uVar31 = (undefined1)(uVar26 >> 8);
          uVar33 = (undefined1)(PVar12 >> 0x18);
          uVar22 = CONCAT11(uVar33,uVar33);
          uVar34 = (undefined1)(PVar12 >> 0x10);
          uVar33 = (undefined1)(PVar12 >> 8);
          uVar35 = pmulhw(CONCAT26(uVar21 >> 4,
                                   CONCAT24((ushort)(CONCAT35(CONCAT21(uVar21,uVar32),
                                                              CONCAT14(uVar32,uVar26)) >> 0x20) >> 4
                                            ,CONCAT22(CONCAT11(uVar31,uVar31) >> 4,
                                                      CONCAT11((char)uVar26,(char)uVar26) >> 4))),
                          CONCAT26(uVar22 >> 4,
                                   CONCAT24((ushort)(CONCAT35(CONCAT21(uVar22,uVar34),
                                                              CONCAT14(uVar34,PVar12)) >> 0x20) >> 4
                                            ,CONCAT22(CONCAT11(uVar33,uVar33) >> 4,
                                                      CONCAT11((char)PVar12,(char)PVar12) >> 4))));
          sVar17 = (short)uVar35;
          sVar18 = (short)((ulonglong)uVar35 >> 0x10);
          sVar19 = (short)((ulonglong)uVar35 >> 0x20);
          sVar20 = (short)((ulonglong)uVar35 >> 0x30);
          modelNode->tintArgb =
               CONCAT13((0 < sVar20) * (sVar20 < 0x100) * (char)((ulonglong)uVar35 >> 0x30) -
                        (0xff < sVar20),
                        CONCAT12((0 < sVar19) * (sVar19 < 0x100) * (char)((ulonglong)uVar35 >> 0x20)
                                 - (0xff < sVar19),
                                 CONCAT11((0 < sVar18) * (sVar18 < 0x100) *
                                          (char)((ulonglong)uVar35 >> 0x10) - (0xff < sVar18),
                                          (0 < sVar17) * (sVar17 < 0x100) * (char)uVar35 -
                                          (0xff < sVar17))));
        }
        ModelNodeRuntime_RebuildTransformsFromRoot(uVar27,uVar23,(ModelRuntimeNode *)modelNode);
        pdVar5 = &(pEVar9->lifecycleOwnerAndDefinition).ownerAndDefinition.runtimeValue24;
        *pdVar5 = *pdVar5 - 1;
        if (*pdVar5 == 0) {
          EVar14 = pEVar9->completionAction;
          completionOwnerCarrier =
               (pEVar9->lifecycleOwnerAndDefinition).ownerAndDefinition.owner.modelNode;
          if (EVar14 == EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY)
          goto EffectModelRuntimeMaintenance_TransitionType1DestroyModel;
          linkedHandlerCompletionOwner = completionOwnerCarrier;
          if (EVar14 == EFFECT_RUNTIME_COMPLETION_INVOKE_LINKED_HANDLER) {
EffectModelRuntimeMaintenance_TransitionType2InvokeLinkedHandler:
            if ((linkedHandlerCompletionOwner != (EffectCompletionLinkedHandlerOwnerColumns104 *)0x0
                ) && (0 < (int)linkedHandlerCompletionOwner->auxiliaryValue80)) {
              FieldGrid_ApplyRadialTerrainHeightDeltaAndRefreshSurfaceCf
                        (linkedHandlerCompletionOwner->terrainMaterialIndex100,
                         linkedHandlerCompletionOwner->auxiliaryValue80,
                         linkedHandlerCompletionOwner->ownerSlot0,
                         (modelNode->worldTransform).translation.y,
                         (modelNode->worldTransform).translation.x,worldRuntime->fieldGrid);
            }
          }
          else {
            spawnArmyCompletionEntity = completionOwnerCarrier;
            if (EVar14 == EFFECT_RUNTIME_COMPLETION_SPAWN_ARMY_FROM_MODEL)
            goto EffectModelRuntimeMaintenance_TransitionType3SpawnArmy;
          }
        }
      }
    }
    IStack_20 = IStack_20 - 1;
    if (IStack_20 == 0) {
      return;
    }
  } while( true );
}
