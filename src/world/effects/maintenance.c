/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/effects/maintenance.c
 * Reverse engineering by idkFoxes 2026
 */

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
void __thandor_void_preserve_eax_ecx_edx
EffectRuntimeMaintenance_RefreshOccupancyFlagsAndTint
          (WorldRuntimeContext *worldRuntime,EffectModelRuntimeNodeClassView100 *modelNode)

{
  PackedArgb32 PVar1;
  PackedArgb32 PVar2;
  short sVar3;
  short sVar4;
  short sVar5;
  short sVar6;
  ushort uVar7;
  ushort uVar8;
  dword primaryOccupancyMask;
  undefined1 mm0PackedValue0ByteLane1;
  undefined1 mm0PackedValue0ByteLane2;
  undefined8 mm0PackedValue0;
  undefined1 uVar9;
  undefined1 uVar10;
  TerrainOccupancyResolvedMasksRegs12 TVar11;
  EffectRuntimeSlot *effectRuntime;
  
  primaryOccupancyMask =
       TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                 (0x1000,(modelNode->worldTransform).translation.y,
                  (modelNode->worldTransform).translation.x,worldRuntime->fieldGrid);
  modelNode->runtimeFlags = modelNode->runtimeFlags & 0xfffffff3;
  effectRuntime = modelNode->effectRuntime;
  TVar11 = TerrainOccupancyMask_ResolveRuntimeClassFlags
                     (modelNode->runtimeFlags,0,primaryOccupancyMask,
                      (char)worldRuntime->activeFactionRuntimeIndex);
  modelNode->runtimeFlags = modelNode->runtimeFlags | TVar11.runtimeFlags;
  effectRuntime->terrainRuntimeClassState = TVar11.primaryOccupancyMask;
  UiModelControl_RefreshStateTint((ModelRuntimeNode *)modelNode);
  if ((modelNode->tintArgb & 0xff000000) != 0) {
    PVar1 = effectRuntime->stateTintArgb;
    PVar2 = ((effectRuntime->definitionOrSavedId).definition)->stateTintArgb;
    mm0PackedValue0ByteLane1 = (undefined1)(PVar1 >> 0x18);
    uVar7 = CONCAT11(mm0PackedValue0ByteLane1,mm0PackedValue0ByteLane1);
    mm0PackedValue0ByteLane2 = (undefined1)(PVar1 >> 0x10);
    mm0PackedValue0ByteLane1 = (undefined1)(PVar1 >> 8);
    uVar9 = (undefined1)(PVar2 >> 0x18);
    uVar8 = CONCAT11(uVar9,uVar9);
    uVar10 = (undefined1)(PVar2 >> 0x10);
    uVar9 = (undefined1)(PVar2 >> 8);
    mm0PackedValue0 =
         pmulhw(CONCAT26(uVar7 >> 4,
                         CONCAT24((ushort)(CONCAT35(CONCAT21(uVar7,mm0PackedValue0ByteLane2),
                                                    CONCAT14(mm0PackedValue0ByteLane2,PVar1)) >>
                                          0x20) >> 4,
                                  CONCAT22(CONCAT11(mm0PackedValue0ByteLane1,
                                                    mm0PackedValue0ByteLane1) >> 4,
                                           CONCAT11((char)PVar1,(char)PVar1) >> 4))),
                CONCAT26(uVar8 >> 4,
                         CONCAT24((ushort)(CONCAT35(CONCAT21(uVar8,uVar10),CONCAT14(uVar10,PVar2))
                                          >> 0x20) >> 4,
                                  CONCAT22(CONCAT11(uVar9,uVar9) >> 4,
                                           CONCAT11((char)PVar2,(char)PVar2) >> 4))));
    sVar3 = (short)mm0PackedValue0;
    sVar4 = (short)((ulonglong)mm0PackedValue0 >> 0x10);
    sVar5 = (short)((ulonglong)mm0PackedValue0 >> 0x20);
    sVar6 = (short)((ulonglong)mm0PackedValue0 >> 0x30);
    modelNode->tintArgb =
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
void __thandor_void_preserve_eax_ecx_edx
EffectRuntimeMaintenance_OccupancyRebuildNoOp(WorldRuntimeContext *worldRuntime,void *runtimeObject)

{
  return;
}


/* Address: 0x0051E840.
   Ownership: world/effects/maintenance.
   Purpose: Second exact two-argument no-op installed in the effect column of the unified model, shot, and effect
   method table. It returns with ret 0x08. Maintenance table phase audioRefresh, object kind effect. The 4x3 table
   bytes, target body, calling convention, and RET 0x08 contract remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
EffectRuntimeMaintenance_AudioRefreshNoOp(WorldRuntimeContext *worldRuntime,void *runtimeObject)

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
void __thandor_void_preserve_eax_ecx_edx
EffectModelRuntimeMaintenance_UpdateLifecycleTintScaleAndTransitions
          (WorldRuntimeContext *worldRuntime,EffectModelRuntimeNodeClassView100 *modelNode)

{
  EffectAnimationFrameCount *pEVar1;
  EffectShadingCountdownTicks *pEVar2;
  EffectPeriodicIntervalTicks *pEVar3;
  DefinitionReferencePresentFlag *pDVar4;
  dword *pdVar5;
  AngleTurn32 *pAVar6;
  GraphicsFixedVec3 *pGVar7;
  GraphicsWorldCoordinateQ12 *pGVar8;
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
  sdword sVar24;
  uint uVar25;
  Q12 worldXQ12;
  int iVar26;
  Q12 worldZQ12;
  GameEntityRuntime *spawnArmyCompletionEntity;
  EffectCompletionLinkedHandlerOwnerColumns104 *linkedHandlerCompletionOwner;
  void *completionOwnerCarrier;
  undefined1 uVar28;
  undefined1 uVar29;
  undefined8 uVar27;
  undefined1 uVar30;
  undefined1 uVar31;
  ModelLookupEntryEaxCf5 MVar32;
  GraphicsShadingRuntimeRecordEaxCf5 GVar33;
  ArmyRuntimeCreateEaxCf5 AVar34;
  FieldGridHeightNormalEaxEdxCf9 FVar35;
  ModelLocalPointRegs12 MVar36;
  FixedDirectionXyzRegs12 FVar37;
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
    iVar26 = 0;
    (pEVar9->lifecycleOwnerAndDefinition).animationFrameAccumulatorQ4 = uVar23;
    if (effectDefinition1->frameAdvanceThresholdQ4 <= uVar23) {
      (pEVar9->lifecycleOwnerAndDefinition).animationFrameAccumulatorQ4 =
           uVar23 - effectDefinition1->frameAdvanceThresholdQ4;
      iVar26 = 1;
      pEVar1 = &pEVar9->animationFramesRemaining;
      *pEVar1 = *pEVar1 - 1;
      if (*pEVar1 == 0) {
        InterpolationState_SetNegatedTargetAndRescaleProgress
                  (effectDefinition1->shadingReleaseTransitionDurationTicks,modelNode->shadingRecord
                  );
        WorldRuntime_UnlinkNodeFromOwnerListD8((WorldOwnerListNode100 *)modelNode);
        (pEVar9->modelNodeOrSavedOffset).modelNode = (ModelRuntimeNode *)0x0;
        return;
      }
    }
    if (iVar26 != 0) {
      modelNode->textureSubresourceBaseIndex = modelNode->textureSubresourceBaseIndex + 1;
      pEVar2 = &pEVar9->shadingStartCountdownTicksRemaining;
      *pEVar2 = *pEVar2 - 1;
      if ((*pEVar2 == 0) && (modelNode->shadingRecord == (GraphicsShadingRuntimeRecord *)0x0)) {
        MVar32 = ModelLookupTable_ContainsPackedKeyCf(0,4,effectDefinition1->ownedNestedResource);
        if (!MVar32.carry) {
          MVar36 = ModelNodeRuntime_TransformLocalPointRegs
                             (MVar32.entry,(ModelRuntimeNode *)modelNode);
          GVar33 = GraphicsShadingRuntime_AllocateRecordRegs
                             (effectDefinition1->shadingTransitionDurationTicks,
                              (effectDefinition1->shadingColorArgb >> 0x18) << 8,
                              effectDefinition1->shadingColorArgb,MVar36.edx,MVar36.ecx,MVar36.eax);
          modelNode->shadingRecord = GVar33.record;
        }
      }
      pEVar2 = &pEVar9->shadingStopCountdownTicksRemaining;
      *pEVar2 = *pEVar2 - 1;
      if ((*pEVar2 == 0) && (modelNode->shadingRecord != (GraphicsShadingRuntimeRecord *)0x0)) {
        InterpolationState_SetNegatedTargetAndRescaleProgress
                  (effectDefinition1->shadingReleaseTransitionDurationTicks,modelNode->shadingRecord
                  );
        modelNode->shadingRecord = (GraphicsShadingRuntimeRecord *)0x0;
      }
    }
    if ((effectDefinition1->transitionPrefix).transitionKind !=
        EFFECT_TRANSITION_ADVANCE_TERRAIN_RELATIVE_MOTION_AND_TERMINATE_ON_CONTACT) {
      uVar23 = pEVar9->effectAgeTicks;
      uVar25 = (effectDefinition1->animationFrameCount * effectDefinition1->frameAdvanceThresholdQ4
               >> 4) - effectDefinition1->alphaFadeOutTicks;
      if (uVar23 < effectDefinition1->alphaFadeInTicks) {
        EVar10 = effectDefinition1->alphaFadeInTicks;
        pEVar9->stateTintArgb = pEVar9->stateTintArgb & 0xffffff;
        pEVar9->stateTintArgb =
             pEVar9->stateTintArgb |
             (int)(((longlong)(int)uVar23 * 0xff) / (longlong)(int)EVar10) << 0x18;
      }
      else if ((uVar25 < uVar23) && (0 < (int)effectDefinition1->alphaFadeOutTicks)) {
        EVar10 = effectDefinition1->alphaFadeOutTicks;
        EVar11 = effectDefinition1->alphaFadeOutTicks;
        pEVar9->stateTintArgb = pEVar9->stateTintArgb & 0xffffff;
        pEVar9->stateTintArgb =
             pEVar9->stateTintArgb |
             (int)(((longlong)(int)((uVar23 - uVar25) - EVar10) * -0xff) / (longlong)(int)EVar11) <<
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
        uVar28 = (undefined1)(PVar12 >> 0x18);
        uVar21 = CONCAT11(uVar28,uVar28);
        uVar29 = (undefined1)(PVar12 >> 0x10);
        uVar28 = (undefined1)(PVar12 >> 8);
        uVar30 = (undefined1)(PVar13 >> 0x18);
        uVar22 = CONCAT11(uVar30,uVar30);
        uVar31 = (undefined1)(PVar13 >> 0x10);
        uVar30 = (undefined1)(PVar13 >> 8);
        uVar27 = pmulhw(CONCAT26(uVar21 >> 4,
                                 CONCAT24((ushort)(CONCAT35(CONCAT21(uVar21,uVar29),
                                                            CONCAT14(uVar29,PVar12)) >> 0x20) >> 4,
                                          CONCAT22(CONCAT11(uVar28,uVar28) >> 4,
                                                   CONCAT11((char)PVar12,(char)PVar12) >> 4))),
                        CONCAT26(uVar22 >> 4,
                                 CONCAT24((ushort)(CONCAT35(CONCAT21(uVar22,uVar31),
                                                            CONCAT14(uVar31,PVar13)) >> 0x20) >> 4,
                                          CONCAT22(CONCAT11(uVar30,uVar30) >> 4,
                                                   CONCAT11((char)PVar13,(char)PVar13) >> 4))));
        sVar17 = (short)uVar27;
        sVar18 = (short)((ulonglong)uVar27 >> 0x10);
        sVar19 = (short)((ulonglong)uVar27 >> 0x20);
        sVar20 = (short)((ulonglong)uVar27 >> 0x30);
        modelNode->tintArgb =
             CONCAT13((0 < sVar20) * (sVar20 < 0x100) * (char)((ulonglong)uVar27 >> 0x30) -
                      (0xff < sVar20),
                      CONCAT12((0 < sVar19) * (sVar19 < 0x100) * (char)((ulonglong)uVar27 >> 0x20) -
                               (0xff < sVar19),
                               CONCAT11((0 < sVar18) * (sVar18 < 0x100) *
                                        (char)((ulonglong)uVar27 >> 0x10) - (0xff < sVar18),
                                        (0 < sVar17) * (sVar17 < 0x100) * (char)uVar27 -
                                        (0xff < sVar17))));
      }
    }
    if ((modelNode->runtimeFlags & 0x800) != 0) {
      modelNode->modelScaleQ12 =
           (int)(((longlong)
                  (effectDefinition1->modelScaleEndQ12 - effectDefinition1->modelScaleStartQ12) *
                 (longlong)(int)pEVar9->effectAgeTicks) /
                (longlong)
                (int)(effectDefinition1->animationFrameCount *
                      effectDefinition1->frameAdvanceThresholdQ4 >> 4)) +
           effectDefinition1->modelScaleStartQ12;
    }
    pEVar9->effectAgeTicks = pEVar9->effectAgeTicks + 1;
    pEVar3 = &pEVar9->periodicEffectCountdownTicks;
    *pEVar3 = *pEVar3 - 1;
    if (*pEVar3 == 0) {
      pEVar9->periodicEffectCountdownTicks = effectDefinition1->periodicEffectIntervalTicks;
      MVar32 = ModelLookupTable_ContainsPackedKeyCf(1,3,effectDefinition1->ownedNestedResource);
      if (!MVar32.carry) {
        effectDefinition = effectDefinition1->periodicEffectDefinition;
        worldRuntime_00 = worldRuntime;
        MVar36 = ModelNodeRuntime_TransformLocalPointRegs
                           (MVar32.entry,(ModelRuntimeNode *)modelNode);
        worldZQ12 = MVar36.edx;
        worldXQ12 = MVar36.ecx;
        EffectRuntimePool_CreateInstanceFromDefinitionCf
                  (EFFECT_RUNTIME_COMPLETION_NONE,(EffectRuntimeOwnerReference4)0x0,0,0x4000,0,
                   worldZQ12,worldXQ12,MVar36.eax,effectDefinition,worldRuntime_00);
      }
    }
                    // WARNING: Switch is manually overridden
    switch((effectDefinition1->transitionPrefix).transitionKind) {
    case EFFECT_TRANSITION_SPAWN_LINKED_EFFECT_AFTER_COUNTDOWN:
      if (iVar26 != 0) {
        pDVar4 = &pEVar9->linkedEffectPresent;
        *pDVar4 = *pDVar4 - 1;
        if (*pDVar4 == 0) {
          MVar32 = ModelLookupTable_ContainsPackedKeyCf(0,3,effectDefinition1->ownedNestedResource);
          if (!MVar32.carry) {
            MVar36 = ModelNodeRuntime_TransformLocalPointRegs
                               (MVar32.entry,(ModelRuntimeNode *)modelNode);
            EffectRuntimePool_CreateInstanceFromDefinitionCf
                      (EFFECT_RUNTIME_COMPLETION_NONE,(EffectRuntimeOwnerReference4)0x0,
                       (modelNode->modelPayload).worldRotationAngle2,
                       (modelNode->modelPayload).worldRotationAngle1,
                       (modelNode->modelPayload).worldRotationAngle0,MVar36.edx,MVar36.ecx,
                       MVar36.eax,effectDefinition1->linkedEffectDefinition,worldRuntime);
          }
        }
      }
      break;
    case EFFECT_TRANSITION_ADVANCE_PERIODIC_EMISSION_AND_COMPLETION_ACTION:
      if (iVar26 != 0) {
        pDVar4 = &pEVar9->linkedShotPresent;
        *pDVar4 = *pDVar4 - 1;
        if (*pDVar4 == 0) {
          keyIndex = (pEVar9->lifecycleOwnerAndDefinition).runtimeState14;
          pEVar9->linkedShotPresent = effectDefinition1->linkedShotPresent;
          (pEVar9->lifecycleOwnerAndDefinition).runtimeState14 =
               (pEVar9->lifecycleOwnerAndDefinition).runtimeState14 + 1;
          MVar32 = ModelLookupTable_ContainsPackedKeyCf
                             (keyIndex,2,effectDefinition1->ownedNestedResource);
          if (!MVar32.carry) {
            MVar36 = ModelNodeRuntime_TransformLocalPointRegs
                               (MVar32.entry,(ModelRuntimeNode *)modelNode);
            ShotRuntimePool_CreateProjectileFromDefinition
                      (0,(ArmyRuntimeSlot *)0x0,
                       (MVar36.edx - (modelNode->worldTransform).translation.z) * 2 +
                       (modelNode->worldTransform).translation.z,
                       (MVar36.ecx - (modelNode->worldTransform).translation.y) * 2 +
                       (modelNode->worldTransform).translation.y,
                       (MVar36.eax - (modelNode->worldTransform).translation.x) * 2 +
                       (modelNode->worldTransform).translation.x,MVar36.edx,MVar36.ecx,MVar36.eax,
                       effectDefinition1->linkedShotDefinition,worldRuntime);
          }
        }
        pDVar4 = &pEVar9->linkedEffectPresent;
        *pDVar4 = *pDVar4 - 1;
        if (*pDVar4 == 0) {
          MVar32 = ModelLookupTable_ContainsPackedKeyCf(0,3,effectDefinition1->ownedNestedResource);
          if (!MVar32.carry) {
            MVar36 = ModelNodeRuntime_TransformLocalPointRegs
                               (MVar32.entry,(ModelRuntimeNode *)modelNode);
            EffectRuntimePool_CreateInstanceFromDefinitionCf
                      (EFFECT_RUNTIME_COMPLETION_NONE,(EffectRuntimeOwnerReference4)0x0,
                       (modelNode->modelPayload).worldRotationAngle2,
                       (modelNode->modelPayload).worldRotationAngle1,
                       (modelNode->modelPayload).worldRotationAngle0,MVar36.edx,MVar36.ecx,
                       MVar36.eax,effectDefinition1->linkedEffectDefinition,worldRuntime);
          }
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
                iVar26 = *piVar15;
                if (*(int *)(iVar26 + 0x4c) == 0x12) {
                  AVar34 = ArmyRuntime_CreateInstanceFromAssetCf
                                     (6,(modelNode1->modelPayload).worldRotationAngle2,
                                      (modelNode1->worldTransform).translation.y,
                                      (modelNode1->worldTransform).translation.x,
                                      (spawnArmyCompletionEntity->common).ownership.ownerIndex,
                                      *(PckArmyAssetIdCatalog *)(iVar26 + 0xc0),worldRuntime);
                  if (!AVar34.carry) {
                    (*(int **)AVar34.eax)[0xf] =
                         (int)(((longlong)piVar15[0xf] *
                               (longlong)*(int *)(**(int **)AVar34.eax + 0x60)) /
                              (longlong)*(int *)(iVar26 + 0x60));
                    ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,spawnArmyCompletionEntity);
                  }
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
      FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                         ((modelNode->modelPayload).worldRotationAngle1,
                          (modelNode->modelPayload).worldRotationAngle0,
                          effectDefinition1->movementSpeedQ12);
      AVar16 = (modelNode->modelPayload).worldRotationAngle1;
      shadingRecord1 = modelNode->shadingRecord;
      pGVar7 = &(modelNode->worldTransform).translation;
      pGVar7->x = pGVar7->x + FVar37.eax;
      pGVar8 = &(modelNode->worldTransform).translation.y;
      *pGVar8 = *pGVar8 + FVar37.ecx;
      pGVar8 = &(modelNode->worldTransform).translation.z;
      *pGVar8 = *pGVar8 + FVar37.edx;
      ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)modelNode);
      (modelNode->modelPayload).worldRotationAngle1 = (int)(AVar16 * 0x3f + 0x4000) >> 6;
      if (shadingRecord1 != (GraphicsShadingRuntimeRecord *)0x0) {
        shadingRecord1->worldXQ12 = shadingRecord1->worldXQ12 + FVar37.eax;
        shadingRecord1->worldYQ12 = shadingRecord1->worldYQ12 + FVar37.ecx;
        shadingRecord1->worldZQ12 = shadingRecord1->worldZQ12 + FVar37.edx;
      }
      break;
    case EFFECT_TRANSITION_ADVANCE_TERRAIN_RELATIVE_MOTION_AND_TERMINATE_ON_CONTACT:
      FVar35 = FieldGrid_InterpolateTerrainHeightAndNormal
                         ((modelNode->worldTransform).translation.y,
                          (modelNode->worldTransform).translation.x,worldRuntime->fieldGrid);
      if (!FVar35.carry) {
        FixedMath_DirectionFromAnglesScaledRegs
                  ((int)FVar35.packedNormalAngles >> 0x10,FVar35.packedNormalAngles & 0xffff,
                   0x10000000);
        FixedMath_DirectionFromAnglesScaledRegs
                  ((modelNode->modelPayload).worldRotationAngle1 -
                   pEVar9->effectAgeTicks * pEVar9->effectAgeTicks * effectDefinition1->unknown58,
                   (modelNode->modelPayload).worldRotationAngle0,0x10000000);
        sVar24 = FixedVec3_DotQ28((GraphicsFixedVec3 *)&stack0xffffffd4,
                                  (GraphicsFixedVec3 *)&stack0xffffffc8);
        if (sVar24 < 0) {
          if (pEVar9->stateTintArgb < 0x1000000) {
            InterpolationState_SetNegatedTargetAndRescaleProgress
                      (effectDefinition1->shadingReleaseTransitionDurationTicks,
                       modelNode->shadingRecord);
            WorldRuntime_UnlinkNodeFromOwnerListD8((WorldOwnerListNode100 *)modelNode);
            (pEVar9->modelNodeOrSavedOffset).modelNode = (ModelRuntimeNode *)0x0;
            return;
          }
          uVar23 = pEVar9->stateTintArgb >> 0x18;
          iVar26 = uVar23 - effectDefinition1->unknown5C;
          if (uVar23 < effectDefinition1->unknown5C) {
            iVar26 = 0;
          }
          uVar23 = pEVar9->stateTintArgb & 0xffffff | iVar26 << 0x18;
        }
        else {
          uVar23 = 0xffffffff;
          pAVar6 = &(modelNode->modelPayload).worldRotationAngle1;
          *pAVar6 = *pAVar6 - pEVar9->effectAgeTicks * pEVar9->effectAgeTicks *
                              effectDefinition1->unknown58;
        }
        pEVar9->stateTintArgb = uVar23;
        if ((modelNode->runtimeFlags & 4) != 0) {
          PVar12 = effectDefinition1->stateTintArgb;
          uVar28 = (undefined1)(uVar23 >> 0x18);
          uVar21 = CONCAT11(uVar28,uVar28);
          uVar29 = (undefined1)(uVar23 >> 0x10);
          uVar28 = (undefined1)(uVar23 >> 8);
          uVar30 = (undefined1)(PVar12 >> 0x18);
          uVar22 = CONCAT11(uVar30,uVar30);
          uVar31 = (undefined1)(PVar12 >> 0x10);
          uVar30 = (undefined1)(PVar12 >> 8);
          uVar27 = pmulhw(CONCAT26(uVar21 >> 4,
                                   CONCAT24((ushort)(CONCAT35(CONCAT21(uVar21,uVar29),
                                                              CONCAT14(uVar29,uVar23)) >> 0x20) >> 4
                                            ,CONCAT22(CONCAT11(uVar28,uVar28) >> 4,
                                                      CONCAT11((char)uVar23,(char)uVar23) >> 4))),
                          CONCAT26(uVar22 >> 4,
                                   CONCAT24((ushort)(CONCAT35(CONCAT21(uVar22,uVar31),
                                                              CONCAT14(uVar31,PVar12)) >> 0x20) >> 4
                                            ,CONCAT22(CONCAT11(uVar30,uVar30) >> 4,
                                                      CONCAT11((char)PVar12,(char)PVar12) >> 4))));
          sVar17 = (short)uVar27;
          sVar18 = (short)((ulonglong)uVar27 >> 0x10);
          sVar19 = (short)((ulonglong)uVar27 >> 0x20);
          sVar20 = (short)((ulonglong)uVar27 >> 0x30);
          modelNode->tintArgb =
               CONCAT13((0 < sVar20) * (sVar20 < 0x100) * (char)((ulonglong)uVar27 >> 0x30) -
                        (0xff < sVar20),
                        CONCAT12((0 < sVar19) * (sVar19 < 0x100) * (char)((ulonglong)uVar27 >> 0x20)
                                 - (0xff < sVar19),
                                 CONCAT11((0 < sVar18) * (sVar18 < 0x100) *
                                          (char)((ulonglong)uVar27 >> 0x10) - (0xff < sVar18),
                                          (0 < sVar17) * (sVar17 < 0x100) * (char)uVar27 -
                                          (0xff < sVar17))));
        }
        ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)modelNode);
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

