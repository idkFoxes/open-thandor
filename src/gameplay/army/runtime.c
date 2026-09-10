#include <thandor/gameplay/army/runtime.h>

/* Implementation ownership: gameplay/army/runtime. */

/* Address: 0x00525A60.
   Ownership: gameplay/army/runtime.
   Purpose: This function object claims Listing ownership for a previously unowned multi-entry/shared-
   tail/computed-dispatch region; it does not assert that every member entry is an independent ABI-level function.
   Runtime-update partition slots 0-23 receive (worldRuntime, armyRuntime). Exact disjoint ranges, terminal
   instructions, inherited register state, shared exits, and caller/table references were revalidated. No function
   splitting or boundary change is permitted.
   Local calls: ArmyRuntimeSpawner_PlayCreationSound, ArmyRuntime_TestWorldPointAllowedDefaultCf,
   ArmyRuntime_TryPlayMappedTerrainSoundAtWorldPoint, ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate,
   ArmyRuntime_TrySpawnDefinitionEffectAtWorldPoint, ArmyRuntime_UpdateTimedShotAndEffectEmitters.
   Cross-module calls: EffectRuntimePool_CreateInstanceFromDefinitionCf [world/effects/runtime],
   FixedMath_Atan2Angle16 [core/math/fixed], FixedMath_SinCosScaled [core/math/fixed],
   ArmyRuntime_ApplyDamageAndPropagateToParent [gameplay/army/combat], FieldGrid_InterpolateTopSurfaceHeight
   [world/terrain/grid], ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive [world/model/hierarchy].
*/
void ArmyRuntimeClassUpdateSlot21_DispatchByClassId
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  GraphicsFixedVec3 *pGVar1;
  GraphicsWorldCoordinateQ12 *pGVar2;
  dword *pdVar3;
  dword dVar4;
  ModelRuntimeNode *sourceRuntime;
  void *modelPointTable;
  longlong lVar5;
  dword dVar6;
  int iVar7;
  ArmyRuntimeSlot *armySlot2;
  uint uVar8;
  ArmyRuntimeSlot *armyRuntime_00;
  uint extraout_ECX;
  int extraout_ECX_00;
  int iVar9;
  int extraout_ECX_01;
  ArmyRuntimeSlot *armyRuntime_01;
  uint extraout_ECX_02;
  ArmyRuntimeSlot *armyRuntime_02;
  ArmyRuntimeSlot *armyRuntime_03;
  dword extraout_ECX_03;
  undefined4 extraout_ECX_04;
  undefined4 extraout_ECX_05;
  int iVar10;
  int extraout_EDX;
  Q12 QVar11;
  undefined4 extraout_EDX_00;
  void *pvVar12;
  ModelRuntimeNode *modelNode1;
  undefined1 in_CF;
  bool bVar13;
  FixedSinCosEdxEax8 FVar14;
  undefined8 uVar15;
  AngleTurn32 heightOffsetQ12;
  AngleTurn32 worldXQ12;
  void *pvVar16;
  InGameSimulationStepBatchTicks IVar17;
  ArmyRuntimeSlot *armySlot1;
  ModelRuntimeNode *modelNode2;
  
  pvVar12 = armyRuntime->definitionOrAsset;
  dVar6 = armyRuntime->classState60;
  modelNode1 = armyRuntime->modelNodeRuntime;
                    
  switch((armyRuntime->articulatedContact).fallbackPosition0Q12) {
  case 1:
    if ((dVar6 == 0) || (*(int *)(dVar6 + 0xb0) == 3)) {
      dVar6 = *(dword *)((int)pvVar12 + 0xc0);
      dVar4 = *(dword *)((int)pvVar12 + 0xc4);
      (armyRuntime->articulatedContact).fallbackPosition0Q12 = 3;
      armyRuntime->movementTarget1Q12 = 0;
      armyRuntime->ownerValue64 = dVar6;
      armyRuntime->ownerValue68 = dVar4;
    }
    else if ((*(int *)(dVar6 + 0xb0) == 0) || (*(int *)(dVar6 + 0xb0) == 6)) {
      EffectRuntimePool_CreateInstanceFromDefinitionCf
                (dVar6,pvVar12,EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY,armyRuntime,
                 (modelNode1->modelPayload).worldRotationAngle2,
                 (modelNode1->modelPayload).worldRotationAngle1,
                 (modelNode1->modelPayload).worldRotationAngle0,
                 (modelNode1->worldTransform).translation.z,
                 (modelNode1->worldTransform).translation.y,
                 (modelNode1->worldTransform).translation.x,
                 *(EffectDefinition **)((int)pvVar12 + 400),worldRuntime);
      armyRuntime->runtimeFlags = armyRuntime->runtimeFlags | 0x20;
    }
    else {
      dVar6 = *(dword *)(*(int *)(*(int *)(dVar6 + 4) + 0xcc) + 0x20);
      (modelNode1->childNodes[0]->modelPayload).localTranslationZQ12 = dVar6;
      armyRuntime->definitionClassValue80 = dVar6;
    }
    break;
  case 2:
    lVar5 = (longlong)armyRuntime->movementTarget1Q12 * (longlong)armyRuntime->movementTarget1Q12;
    modelNode1 = modelNode1->childNodes[0];
    lVar5 = (longlong)(int)(*(int *)((int)pvVar12 + 0x14) * g_InGameSimulationStepTicks) *
            (longlong)(int)((int)((ulonglong)lVar5 >> 0x20) << 0x14 | (uint)lVar5 >> 0xc);
    (modelNode1->modelPayload).localRotationAngle0 = 0x8000;
    iVar7 = armyRuntime->movementTarget1Q12;
    (modelNode1->modelPayload).localTranslationZQ12 =
         ((int)((ulonglong)lVar5 >> 0x20) << 0x14 | (uint)lVar5 >> 0xc) +
         armyRuntime->definitionClassValue80;
    lVar5 = (longlong)(int)(iVar7 * g_InGameSimulationStepTicks) *
            (longlong)*(int *)((int)pvVar12 + 0x14);
    dVar6 = FixedMath_Atan2Angle16
                      ((int)((ulonglong)lVar5 >> 0x20) << 0x15 | (uint)lVar5 >> 0xb,0x1000);
    iVar7 = *(int *)((int)pvVar12 + 0xc) * g_InGameSimulationStepTicks;
    armyRuntime->movementTarget1Q12 = armyRuntime->movementTarget1Q12 + iVar7;
    (modelNode1->modelPayload).localRotationAngle1 = 0x4000 - dVar6 & 0xffff;
    modelNode1 = armyRuntime->modelNodeRuntime;
    FVar14 = FixedMath_SinCosScaled((modelNode1->modelPayload).worldRotationAngle2,iVar7);
    pGVar1 = &(modelNode1->worldTransform).translation;
    pGVar1->x = pGVar1->x + (int)FVar14;
    pGVar2 = &(modelNode1->worldTransform).translation.y;
    *pGVar2 = *pGVar2 + (int)(FVar14 >> 0x20);
    pdVar3 = &armyRuntime->ownerValue68;
    *pdVar3 = *pdVar3 - 1;
    if (((int)*pdVar3 < 0) &&
       ((armyRuntime->articulatedContact).fallbackPosition0Q12 = 1,
       armyRuntime_02 != (ArmyRuntimeSlot *)0x0)) {
      pvVar12 = armyRuntime->definitionOrAsset;
      pvVar16 = armyRuntime_02->definitionOrAsset;
      (armyRuntime_02->articulatedContact).lateralOffsetQ12 = 4;
      ArmyRuntimeSpawner_PlayCreationSound(armyRuntime_02,worldRuntime);
      iVar7 = (int)(((longlong)armyRuntime_03->actionVector2Q12 *
                    (longlong)*(int *)((int)pvVar12 + 0x60)) /
                   (longlong)*(int *)((int)pvVar16 + 0x60));
      uVar8 = iVar7 - armyRuntime->actionVector2Q12;
      if (uVar8 != 0 && armyRuntime->actionVector2Q12 <= iVar7) {
        ArmyRuntime_ApplyDamageAndPropagateToParent(uVar8 >> 1,armyRuntime_03);
      }
    }
    break;
  case 3:
    lVar5 = (longlong)armyRuntime->movementTarget1Q12 * (longlong)armyRuntime->movementTarget1Q12;
    modelNode1 = modelNode1->childNodes[0];
    lVar5 = (longlong)(int)(*(int *)((int)pvVar12 + 0x14) * g_InGameSimulationStepTicks) *
            (longlong)(int)((int)((ulonglong)lVar5 >> 0x20) << 0x14 | (uint)lVar5 >> 0xc);
    (modelNode1->modelPayload).localRotationAngle0 = 0x8000;
    iVar7 = armyRuntime->movementTarget1Q12;
    (modelNode1->modelPayload).localTranslationZQ12 =
         ((int)((ulonglong)lVar5 >> 0x20) << 0x14 | (uint)lVar5 >> 0xc) +
         armyRuntime->definitionClassValue80;
    lVar5 = (longlong)(int)(iVar7 * g_InGameSimulationStepTicks) *
            (longlong)*(int *)((int)pvVar12 + 0x14);
    dVar6 = FixedMath_Atan2Angle16
                      ((int)((ulonglong)lVar5 >> 0x20) << 0x15 | (uint)lVar5 >> 0xb,0x1000);
    iVar7 = *(int *)((int)pvVar12 + 0xc) * g_InGameSimulationStepTicks;
    armyRuntime->movementTarget1Q12 = armyRuntime->movementTarget1Q12 + iVar7;
    (modelNode1->modelPayload).localRotationAngle1 = 0x4000 - dVar6 & 0xffff;
    modelNode1 = armyRuntime->modelNodeRuntime;
    FVar14 = FixedMath_SinCosScaled((modelNode1->modelPayload).worldRotationAngle2,iVar7);
    pGVar1 = &(modelNode1->worldTransform).translation;
    pGVar1->x = pGVar1->x + (int)FVar14;
    pGVar2 = &(modelNode1->worldTransform).translation.y;
    *pGVar2 = *pGVar2 + (int)(FVar14 >> 0x20);
    pdVar3 = &armyRuntime->ownerValue64;
    *pdVar3 = *pdVar3 - 1;
    if ((*pdVar3 == 0) && (armyRuntime_00 != (ArmyRuntimeSlot *)0x0)) {
      (armyRuntime_00->articulatedContact).lateralOffsetQ12 = 4;
      ArmyRuntimeSpawner_PlayCreationSound(armyRuntime_00,worldRuntime);
    }
    pdVar3 = &armyRuntime->ownerValue68;
    *pdVar3 = *pdVar3 - 1;
    if ((int)*pdVar3 < 0) {
      (modelNode1->worldTransform).translation.x = -0x100000;
      (modelNode1->worldTransform).translation.y = 0x100000;
      (armyRuntime->articulatedContact).fallbackPosition0Q12 = 4;
    }
    break;
  case 4:
    uVar15 = ArmyRuntime_TestWorldPointAllowedDefaultCf(dVar6,pvVar12);
    iVar7 = (int)((ulonglong)uVar15 >> 0x20);
    if (!(bool)in_CF) {
      FVar14 = FixedMath_SinCosScaled
                         (armyRuntime->movementTarget0Q12 ^ 0x8000,
                          *(int *)(iVar7 + 0xc) * *(int *)(iVar7 + 200));
      iVar10 = armyRuntime->fallbackWorldXQ12;
      (modelNode1->worldTransform).translation.x = (int)FVar14 + armyRuntime->fallbackWorldYQ12;
      (modelNode1->worldTransform).translation.y = (int)(FVar14 >> 0x20) + iVar10;
      (modelNode1->modelPayload).worldRotationAngle2 = extraout_ECX ^ 0x8000;
      armySlot1 = *(ArmyRuntimeSlot **)(iVar7 + 200);
      armyRuntime->linkedArmyRuntimeOrSavedOffset = armySlot1;
      dVar6 = (int)armySlot1 * 2;
      iVar10 = *(int *)(iVar7 + 0xc);
      armyRuntime->ownerValue68 = dVar6;
      armyRuntime->movementTarget1Q12 = -(int)armySlot1 * iVar10;
      (armyRuntime->articulatedContact).fallbackPosition0Q12 = 5;
      FVar14 = FixedMath_SinCosScaled(extraout_ECX ^ 0x8000,*(int *)(iVar7 + 0xc) * 10);
      iVar7 = (modelNode1->worldTransform).translation.x;
      iVar10 = (modelNode1->worldTransform).translation.y;
      dVar4 = armyRuntime->definitionClassValue80;
      do {
        QVar11 = FieldGrid_InterpolateTopSurfaceHeight(iVar10,iVar7,worldRuntime->fieldGrid);
        iVar9 = extraout_ECX_00;
        if (extraout_ECX_00 < QVar11) {
          iVar9 = QVar11;
        }
        iVar7 = iVar7 + (int)FVar14;
        iVar10 = iVar10 + (int)(FVar14 >> 0x20);
        dVar6 = dVar6 - 10;
      } while (-1 < (int)dVar6);
      armyRuntime->definitionClassValue84 = iVar9 + 0x800 + dVar4 * 2;
    }
    break;
  case 5:
    FVar14 = FixedMath_SinCosScaled
                       ((modelNode1->modelPayload).worldRotationAngle2,
                        g_InGameSimulationStepTicks * *(int *)((int)pvVar12 + 0xc));
    pGVar1 = &(modelNode1->worldTransform).translation;
    pGVar1->x = pGVar1->x + (int)FVar14;
    pGVar2 = &(modelNode1->worldTransform).translation.y;
    *pGVar2 = *pGVar2 + (int)(FVar14 >> 0x20);
    QVar11 = FieldGrid_InterpolateTopSurfaceHeight
                       ((modelNode1->worldTransform).translation.y,
                        (modelNode1->worldTransform).translation.x,worldRuntime->fieldGrid);
    pvVar12 = armyRuntime->definitionOrAsset;
    lVar5 = (longlong)armyRuntime->movementTarget1Q12 * (longlong)armyRuntime->movementTarget1Q12;
    modelNode1 = modelNode1->childNodes[0];
    lVar5 = (longlong)(int)(*(int *)((int)pvVar12 + 0xcc) * g_InGameSimulationStepTicks) *
            (longlong)(int)((int)((ulonglong)lVar5 >> 0x20) << 0x14 | (uint)lVar5 >> 0xc);
    (modelNode1->modelPayload).localRotationAngle0 = 0x8000;
    iVar7 = armyRuntime->movementTarget1Q12;
    (modelNode1->modelPayload).localTranslationZQ12 =
         (((int)((ulonglong)lVar5 >> 0x20) << 0x14 | (uint)lVar5 >> 0xc) +
         armyRuntime->definitionClassValue84) - QVar11;
    lVar5 = (longlong)(int)(iVar7 * g_InGameSimulationStepTicks) *
            (longlong)*(int *)((int)pvVar12 + 0xcc);
    dVar6 = FixedMath_Atan2Angle16
                      ((int)((ulonglong)lVar5 >> 0x20) << 0x15 | (uint)lVar5 >> 0xb,0x1000);
    armyRuntime->movementTarget1Q12 =
         armyRuntime->movementTarget1Q12 +
         *(int *)((int)pvVar12 + 0xc) * g_InGameSimulationStepTicks;
    (modelNode1->modelPayload).localRotationAngle1 = 0x4000 - dVar6 & 0xffff;
    modelNode1 = armyRuntime->modelNodeRuntime;
    IVar17 = g_InGameSimulationStepTicks;
    do {
      armyRuntime->linkedArmyRuntimeOrSavedOffset =
           (ArmyRuntimeSlot *)
           ((int)&armyRuntime->linkedArmyRuntimeOrSavedOffset[-1].selectionMetric5 + 3);
      pdVar3 = &armyRuntime->ownerValue68;
      *pdVar3 = *pdVar3 - 1;
      if ((int)*pdVar3 < 0) {
        (modelNode1->worldTransform).translation.x = -0x100000;
        (modelNode1->worldTransform).translation.y = 0x100000;
        (armyRuntime->articulatedContact).fallbackPosition0Q12 = 6;
      }
      pvVar16 = pvVar12;
      modelNode2 = modelNode1;
      armySlot1 = armyRuntime;
      ArmyRuntime_TryPlayMappedTerrainSoundAtWorldPoint
                ((armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex,
                 (modelNode1->worldTransform).translation.y,
                 (modelNode1->worldTransform).translation.x,
                 *(SoundAssetIndex *)((int)pvVar12 + 0x26c),worldRuntime);
      sourceRuntime = modelNode1->childNodes[0];
      iVar7 = *(int *)((int)pvVar12 + 0x30) * -3;
      iVar10 = (sourceRuntime->worldTransform).translation.z + -0x2000;
      modelNode1 = modelNode2;
      if (*(int *)((int)pvVar12 + 0x30) != 0) {
        modelPointTable = *(void **)(extraout_ECX_01 + 0x18);
        if (iVar7 - (int)armyRuntime->linkedArmyRuntimeOrSavedOffset == 0) {
          ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
                    (0,iVar10,(sourceRuntime->worldTransform).translation.y,
                     (sourceRuntime->worldTransform).translation.x,7,
                     *(PckEffectDefinitionIdCatalog *)((int)pvVar12 + 0x2c),sourceRuntime,
                     modelPointTable,worldRuntime);
          modelNode1 = modelNode2;
        }
        else {
          armySlot2 = (ArmyRuntimeSlot *)(iVar7 + *(int *)((int)pvVar12 + 0x30));
          if (armySlot2 == armyRuntime->linkedArmyRuntimeOrSavedOffset) {
            ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
                      (0,iVar10,(sourceRuntime->worldTransform).translation.y,
                       (sourceRuntime->worldTransform).translation.x,6,
                       *(PckEffectDefinitionIdCatalog *)((int)pvVar12 + 0x2c),sourceRuntime,
                       modelPointTable,worldRuntime);
            modelNode1 = modelNode2;
          }
          else {
            armySlot2 = (ArmyRuntimeSlot *)
                        ((int)&armySlot2->definitionOrAsset + *(int *)((int)pvVar12 + 0x30));
            if (armySlot2 == armyRuntime->linkedArmyRuntimeOrSavedOffset) {
              ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
                        (0,iVar10,(sourceRuntime->worldTransform).translation.y,
                         (sourceRuntime->worldTransform).translation.x,5,
                         *(PckEffectDefinitionIdCatalog *)((int)pvVar12 + 0x2c),sourceRuntime,
                         modelPointTable,worldRuntime);
              modelNode1 = modelNode2;
            }
            else {
              armySlot2 = (ArmyRuntimeSlot *)
                          ((int)&armySlot2->definitionOrAsset + *(int *)((int)pvVar12 + 0x30));
              if (armySlot2 == armyRuntime->linkedArmyRuntimeOrSavedOffset) {
                ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
                          (0,iVar10,(sourceRuntime->worldTransform).translation.y,
                           (sourceRuntime->worldTransform).translation.x,4,
                           *(PckEffectDefinitionIdCatalog *)((int)pvVar12 + 0x2c),sourceRuntime,
                           modelPointTable,worldRuntime);
                modelNode1 = modelNode2;
              }
              else {
                armySlot2 = (ArmyRuntimeSlot *)
                            ((int)&armySlot2->definitionOrAsset + *(int *)((int)pvVar12 + 0x30));
                if (armySlot2 == armyRuntime->linkedArmyRuntimeOrSavedOffset) {
                  ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
                            (0,iVar10,(sourceRuntime->worldTransform).translation.y,
                             (sourceRuntime->worldTransform).translation.x,3,
                             *(PckEffectDefinitionIdCatalog *)((int)pvVar12 + 0x2c),sourceRuntime,
                             modelPointTable,worldRuntime);
                  modelNode1 = modelNode2;
                }
                else {
                  armySlot2 = (ArmyRuntimeSlot *)
                              ((int)&armySlot2->definitionOrAsset + *(int *)((int)pvVar12 + 0x30));
                  if (armySlot2 == armyRuntime->linkedArmyRuntimeOrSavedOffset) {
                    ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
                              (0,iVar10,(sourceRuntime->worldTransform).translation.y,
                               (sourceRuntime->worldTransform).translation.x,2,
                               *(PckEffectDefinitionIdCatalog *)((int)pvVar12 + 0x2c),sourceRuntime,
                               modelPointTable,worldRuntime);
                    modelNode1 = modelNode2;
                  }
                  else if ((ArmyRuntimeSlot *)
                           ((int)&armySlot2->definitionOrAsset + *(int *)((int)pvVar12 + 0x30)) ==
                           armyRuntime->linkedArmyRuntimeOrSavedOffset) {
                    ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
                              (0,iVar10,(sourceRuntime->worldTransform).translation.y,
                               (sourceRuntime->worldTransform).translation.x,1,
                               *(PckEffectDefinitionIdCatalog *)((int)pvVar12 + 0x2c),sourceRuntime,
                               modelPointTable,worldRuntime);
                    modelNode1 = modelNode2;
                  }
                }
              }
            }
          }
        }
      }
      armyRuntime = armySlot1;
      pvVar12 = pvVar16;
      IVar17 = IVar17 - 1;
    } while (IVar17 != 0);
    break;
  case 6:
    if (dVar6 != 0) {
      iVar7 = *(int *)(dVar6 + 4);
      bVar13 = false;
      if ((*(int *)(dVar6 + 0xb0) == 0) &&
         (ArmyRuntime_TestWorldPointAllowedDefaultCf(dVar6,pvVar12), !bVar13)) {
        (armyRuntime_01->articulatedContact).lateralOffsetQ12 = 1;
        ArmyRuntime_TrySpawnDefinitionEffectAtWorldPoint(armyRuntime_01,worldRuntime);
        FVar14 = FixedMath_SinCosScaled
                           (*(uint *)(iVar7 + 0x14) ^ 0x8000,
                            *(int *)(extraout_EDX + 0xc) * *(int *)(extraout_EDX + 0xc4));
        iVar10 = *(int *)(iVar7 + 0x98);
        modelNode1 = armyRuntime->modelNodeRuntime;
        (modelNode1->worldTransform).translation.x = (int)FVar14 + *(int *)(iVar7 + 0x94);
        (modelNode1->worldTransform).translation.y = (int)(FVar14 >> 0x20) + iVar10;
        (modelNode1->modelPayload).worldRotationAngle2 = extraout_ECX_02 ^ 0x8000;
        dVar6 = *(dword *)(extraout_EDX + 0xc4);
        iVar7 = *(int *)(extraout_EDX + 0xc);
        armyRuntime->ownerValue68 = dVar6;
        armyRuntime->movementTarget1Q12 = -dVar6 * iVar7;
        (armyRuntime->articulatedContact).fallbackPosition0Q12 = 2;
      }
      break;
    }
    ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive
              (worldRuntime,(int *)armyRuntime->linkedEntityRuntime);
    dVar6 = extraout_ECX_03;
  case 0:
    if (dVar6 == 0) {
      QVar11 = 0x1000;
    }
    else {
      QVar11 = *(Q12 *)(*(int *)(*(int *)(dVar6 + 4) + 0xcc) + 0x20);
    }
    (modelNode1->childNodes[0]->modelPayload).localTranslationZQ12 = QVar11;
  }
  modelNode1 = armyRuntime->modelNodeRuntime;
  worldXQ12 = (modelNode1->worldTransform).translation.y;
  heightOffsetQ12 = *(AngleTurn32 *)((int)armyRuntime->definitionOrAsset + 0x54);
  (*g_ArmyPlacementContactKindDispatchTable.callbacks
    [*(int *)((int)armyRuntime->definitionOrAsset + 0x278)])
            (heightOffsetQ12,worldXQ12,(modelNode1->worldTransform).translation.x,modelNode1,
             worldRuntime);
  iVar7 = (armyRuntime->articulatedContact).fallbackPosition0Q12;
  (modelNode1->modelPayload).worldRotationAngle1 = heightOffsetQ12;
  (modelNode1->modelPayload).worldRotationAngle0 = worldXQ12;
  if ((iVar7 != 0) && (iVar7 != 1)) {
    ArmyRuntime_UpdateTimedShotAndEffectEmitters(worldRuntime,armyRuntime);
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,armyRuntime);
  ModelNodeRuntime_RebuildTransformsFromRoot(extraout_ECX_04,extraout_EDX_00,modelNode1);
  uVar15 = ModelNodeRuntime_RecomputeSubtreeBoundingRadius(modelNode1);
  iVar7 = (int)((ulonglong)uVar15 >> 0x20);
  ModelNodeRuntime_UpdateDepthBinMasks
            (extraout_ECX_05,iVar7,*(DepthIntervalRadius32 *)(iVar7 + 0xdc),modelNode1);
  return;
}

/* Address: 0x00526620.
   Ownership: gameplay/army/runtime.
   Purpose: Runtime-update slot 22 owns only the exact non-overlapping dispatcher prefix. Its second switch tail-
   dispatches into the established ArmyRuntimeCallbackTable00526990Ownership function at 005269C0 and interior
   entries of that sealed ownership container. The dispatcher retains the common void __stdcall two-argument table
   ABI and purge 8 contract; no false RET is assigned to the prefix. Updates linked model hierarchy flags when the
   runtime transition bit is active, then dispatches the sealed terrain-contact mode through the existing ownership
   container. Runtime-update partition slots 0-23 receive (worldRuntime, armyRuntime).
   Local calls: ArmyRuntime_UpdateTimedShotAndEffectEmitters, ArmyRuntime_UpdateAnimatedModelSubnodes,
   ArmyRuntime_TestWorldPointAllowedDefaultCf, ArmyRuntimeSpawner_CreateLinkedChildInstanceCf.
   Cross-module calls: ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive [world/model/hierarchy],
   ArmyAssetRegistry_FindByIdCf [assets/army/catalog], ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
   [assets/model/definitions], InGameNotificationQueue_InsertPriorityRecord [ui/ingame/runtime],
   TerrainGrid_TestProjectedCellMaskBits01Cf [world/terrain/grid], SpatialSound_PlayPositionedOneShot
   [audio/spatial/runtime].
*/
void ArmyRuntimeClass_UpdateLinkedModelFlagsAndDispatchTerrainContactMode
               (WorldRuntimeContext *worldRuntime,
               ArmyRuntimeTerrainContactLinkedChildSlotView120 *armyRuntime)

{
  FactionArmyAssetCount *pFVar1;
  ArmyRuntimeTimer *pAVar2;
  FactionRelationCounter *pFVar3;
  byte *pbVar4;
  Q12 *pQVar5;
  uint *puVar6;
  dword dVar7;
  uint uVar8;
  PckArmyAssetIdCatalog registryId;
  ModelRuntimeNode *modelNodeRuntime;
  ArmyAssetRecordPrefix *pAVar9;
  ModelDefinitionRecordPrefix *modelDefinition1;
  uint in_ECX;
  uint extraout_ECX;
  FactionArmyAssetCount linkedChildUsedSlotCount;
  ArmyBuildXeniteCostQ4 secondaryArmyBuildXeniteCostQ4;
  FactionArmyAssetCount FVar10;
  uint extraout_ECX_00;
  uint extraout_ECX_01;
  int linkedChildReverseSlotIndex;
  uint extraout_ECX_02;
  GraphicsFixedVec3 *worldPosition;
  GraphicsFixedVec3 *worldPosition_00;
  GraphicsFixedVec3 *worldPosition_01;
  GraphicsFixedVec3 *worldPosition_02;
  GraphicsFixedVec3 *worldPosition_03;
  uint extraout_EDX;
  int extraout_EDX_00;
  int extraout_EDX_01;
  int extraout_EDX_02;
  int iVar11;
  uint uVar12;
  WorldRuntimeNode *ownerNodeCursor;
  dword *pdVar13;
  bool bVar14;
  undefined8 uVar15;
  int iVar16;
  PckModelDefinitionIdCatalog notificationMovieId;
  ModelRuntimeLinkedChildClassView200 *pMVar17;
  int *modelRuntimePayloadWords;
  GameEntityRuntime *entityRuntime1;
  ModelRuntimeNode *modelNode1;
  
  if ((armyRuntime->runtimeFlags & 0x10) != 0) {
    ownerNodeCursor = worldRuntime->ownerListHead;
    do {
      if (ownerNodeCursor[2].common.nextNode == (WorldRuntimeNode *)0x0) {
        puVar6 = ownerNodeCursor->runtimePayload;
        in_ECX = *puVar6;
        if (((*(int *)(in_ECX + 0x4c) == 0x15) &&
            (armyRuntime == (ArmyRuntimeTerrainContactLinkedChildSlotView120 *)puVar6[0x18])) &&
           ((puVar6[0x3b] & 0x10) == 0)) {
          ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive(worldRuntime,(int *)puVar6[2]);
          in_ECX = extraout_ECX;
        }
      }
      ownerNodeCursor = (ownerNodeCursor->common).nextNode;
    } while (ownerNodeCursor != (WorldRuntimeNode *)0x0);
  }
                    
  switch((armyRuntime->terrainContactLinkedChildState).terrainContactMode) {
  case ARMY_TERRAIN_CONTACT_ACQUIRE_OR_INITIALIZE_CONTACT_SLOT:
    if ((armyRuntime->runtimeFlags & 0x40) == 0) {
      if ((armyRuntime->runtimeFlags & 0xc9) == 0) {
        in_ECX = armyRuntime->linkedChildUsedSlotCount6C;
        iVar16 = (armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex;

        if (in_ECX < (armyRuntime->modelRuntime->linkedChildClassState).linkedChildSlotCountC4) {
          FVar10 = g_GameFactionRuntimeImage.records[iVar16].secondaryArmyAssetCount;
          pdVar13 = g_GameFactionRuntimeImage.records[iVar16].secondaryArmyAssetPointersOrIds;
          in_ECX = 0;
          if (FVar10 != 0) {
ArmyRuntimeClass_SelectAffordableSecondaryArmyAssetLoop:
            dVar7 = *pdVar13;
            in_ECX = *(uint *)(dVar7 + 0x28);

            if (((*(uint *)(dVar7 + 0x14) & 8) == 0) ||
               (g_GameFactionRuntimeImage.records[iVar16].xeniteCurrentQ4 < in_ECX))
            goto ArmyRuntimeClass_AdvanceSecondaryArmyAssetCandidate;
            g_GameFactionRuntimeImage.records[iVar16].xeniteCurrentQ4 =
                 g_GameFactionRuntimeImage.records[iVar16].xeniteCurrentQ4 - in_ECX;
            uVar12 = *(uint *)(dVar7 + 0x24);
            iVar11 = *(int *)(dVar7 + 0x2c);
            if ((g_UiCommandRuntimeFlags & 0x100000) != 0) {
              uVar12 = (uVar12 >> 4) + 1;
            }
            dVar7 = *(dword *)(dVar7 + 8);
            armyRuntime->ownerValue68 = uVar12;
            armyRuntime->fallbackWorldXQ12 = iVar11;
            armyRuntime->classState60 = dVar7;
            armyRuntime->runtimeTimer = armyRuntime->runtimeTimer + iVar11;
            armyRuntime->ownerValue64 = 0;
            pFVar1 = &g_GameFactionRuntimeImage.records[iVar16].secondaryArmyAssetCount;
            *pFVar1 = *pFVar1 - 1;
            do {
              *pdVar13 = pdVar13[1];
              pdVar13 = pdVar13 + 1;
              FVar10 = FVar10 - 1;
            } while (FVar10 != 0);
            (armyRuntime->terrainContactLinkedChildState).terrainContactMode =
                 ARMY_TERRAIN_CONTACT_ADVANCE_ACTIVE_CONTACT_AND_RELEASE;
            armyRuntime->runtimeFlags = armyRuntime->runtimeFlags | 0x100;
            in_ECX = 0;
            break;
          }
        }
      }
    }
    else if ((armyRuntime->runtimeFlags & 9) == 0) {
      ArmyRuntime_UpdateTimedShotAndEffectEmitters(worldRuntime,(ArmyRuntimeSlot *)armyRuntime);
      ArmyRuntime_UpdateAnimatedModelSubnodes(worldRuntime,(ArmyRuntimeSlot *)armyRuntime);
      in_ECX = extraout_ECX_00;
    }
    break;
  case ARMY_TERRAIN_CONTACT_ADVANCE_ACTIVE_CONTACT_AND_RELEASE:
    if ((armyRuntime->runtimeFlags & 9) == 0) {
      armyRuntime->ownerValue64 = armyRuntime->ownerValue64 + g_InGameSimulationStepTicks;
      entityRuntime1 = armyRuntime->linkedEntityRuntime;
      ArmyRuntime_UpdateTimedShotAndEffectEmitters(worldRuntime,(ArmyRuntimeSlot *)armyRuntime);
      ArmyRuntime_UpdateAnimatedModelSubnodes(worldRuntime,(ArmyRuntimeSlot *)armyRuntime);
      in_ECX = extraout_ECX_01;
      if (armyRuntime->ownerValue68 <= extraout_EDX) {
        iVar16 = (entityRuntime1->common).ownership.ownerIndex;
        uVar8 = armyRuntime->fallbackWorldXQ12;
        armyRuntime->fallbackWorldXQ12 = 0;
        (armyRuntime->terrainContactLinkedChildState).terrainContactMode =
             ARMY_TERRAIN_CONTACT_ACQUIRE_OR_INITIALIZE_CONTACT_SLOT;
        armyRuntime->runtimeFlags = armyRuntime->runtimeFlags & 0xfffffeff;
        pAVar2 = &armyRuntime->runtimeTimer;
        uVar12 = *pAVar2;
        *pAVar2 = *pAVar2 - uVar8;
        registryId = armyRuntime->classState60;
        armyRuntime->classState60 = 0;
        if (uVar8 <= uVar12) {
          in_ECX = (armyRuntime->modelRuntime->linkedChildClassState).linkedChildSlotCountC4 - 1;
          do {
            if (armyRuntime->linkedChildAssetIdSlots13[in_ECX] == 0)
            goto ArmyRuntimeClass_StoreCompletedSecondaryArmyAssetId;

            in_ECX = in_ECX - 1;
          } while (-1 < (int)in_ECX);
          in_ECX = 0;
ArmyRuntimeClass_StoreCompletedSecondaryArmyAssetId:
          armyRuntime->linkedChildAssetIdSlots13[in_ECX] = registryId;
          pFVar3 = &g_GameFactionRuntimeImage.records[iVar16].relationCounterA;
          *pFVar3 = *pFVar3 + 1;
          armyRuntime->linkedChildUsedSlotCount6C = armyRuntime->linkedChildUsedSlotCount6C + 1;
          armyRuntime->fallbackWorldYQ12 = armyRuntime->fallbackWorldYQ12 + 1;
          if (iVar16 == worldRuntime->activeFactionRuntimeIndex) {
            pAVar9 = ArmyAssetRegistry_FindByIdCf(registryId);
            modelDefinition1 =
                 ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                           ((entityRuntime1->common).ownership.ownerIndex,
                            pAVar9->rootNodeOffsetOrPointer);
            modelDefinition1[0x24].byteSize = modelDefinition1[0x24].byteSize + 1;
            notificationMovieId = modelDefinition1[0x1d].flags;
            if (modelDefinition1[0x24].byteSize != 1) {
              notificationMovieId = modelDefinition1[0x1d].definitionId;
            }
            InGameNotificationQueue_InsertPriorityRecord
                      (NONE,0,(worldRuntime->motion).pitchAngle,(worldRuntime->motion).headingAngle,
                       (armyRuntime->modelNodeRuntime->worldTransform).translation.y,
                       (armyRuntime->modelNodeRuntime->worldTransform).translation.x,3,
                       notificationMovieId);
            in_ECX = extraout_ECX_02;
          }
        }
      }
    }
  }
ArmyRuntimeClass_DispatchLinkedChildSpawnAndDamageEffectState:
  pMVar17 = armyRuntime->modelRuntime;
  modelNodeRuntime = armyRuntime->modelNodeRuntime;
                    
  switch((armyRuntime->terrainContactLinkedChildState).linkedChildDispatchState) {
  case 0:
    goto ArmyRuntimeClass_ProcessPendingLinkedChildSpawnsAndDamageEffect;
  case 1:
    modelNodeRuntime->primaryTextureOffsetV =
         modelNodeRuntime->primaryTextureOffsetV +
         pMVar17->attachmentCount0C * g_InGameSimulationStepTicks;
    if (0x7ffff < modelNodeRuntime->primaryTextureOffsetV) {
      modelNodeRuntime->primaryTextureOffsetV = 0x80000;
      (armyRuntime->terrainContactLinkedChildState).linkedChildDispatchState = 2;
      uVar12 = pMVar17[1].classLinkState.classState70;
      if ((((uVar12 != 0) && (uVar12 < worldRuntime->dwordArrayCount)) &&
          (worldRuntime->dwordArray != (dword *)0x0)) &&
         (bVar14 = false, worldRuntime->dwordArray[uVar12] != 0)) {
        uVar15 = TerrainGrid_TestProjectedCellMaskBits01Cf
                           ((modelNodeRuntime->worldTransform).translation.y,
                            (modelNodeRuntime->worldTransform).translation.x,worldRuntime);
        iVar16 = (int)((ulonglong)uVar15 >> 0x20);
        if (!bVar14) {
          SpatialSound_PlayPositionedOneShot
                    (*(SpatialSoundMaximumDistanceQ12 *)(iVar16 + 0x7c),
                     *(SpatialSoundGainQ15 *)(iVar16 + 0x78),worldPosition_02,
                     (DirectSoundVoiceSet **)uVar15);
        }
      }
    }
    break;
  case 2:
    modelNode1 = modelNodeRuntime->childNodes[0];
    iVar16 = *(int *)((pMVar17->linkedChildClassState).reservedC8_CF + 4);
    pQVar5 = &(modelNode1->modelPayload).localTranslationZQ12;
    *pQVar5 = *pQVar5 + *(int *)(pMVar17->linkedChildClassState).reservedC8_CF *
                        g_InGameSimulationStepTicks;
    if (iVar16 < (modelNode1->modelPayload).localTranslationZQ12) {
      (armyRuntime->terrainContactLinkedChildState).linkedChildDispatchState = 3;
      (modelNode1->modelPayload).localTranslationZQ12 = iVar16;
    }
    ModelNodeRuntime_RebuildTransformsFromRoot(modelNode1,iVar16,modelNodeRuntime);
    break;
  case 4:
    iVar16 = *(int *)(pMVar17->linkedChildClassState).reservedC8_CF;
    uVar15 = ModelLookupTable_FindPackedKeyEntryRegsCf
                       (0,1,(modelNodeRuntime->modelPayload).modelResource);
    iVar11 = (int)((ulonglong)uVar15 >> 0x20);
    modelNode1 = modelNodeRuntime->childNodes[0];
    pQVar5 = &(modelNode1->modelPayload).localTranslationZQ12;
    *pQVar5 = *pQVar5 - iVar16 * g_InGameSimulationStepTicks;
    if ((modelNode1->modelPayload).localTranslationZQ12 < iVar11) {
      (armyRuntime->terrainContactLinkedChildState).linkedChildDispatchState = 5;
      (modelNode1->modelPayload).localTranslationZQ12 = iVar11;
    }
    ModelNodeRuntime_RebuildTransformsFromRoot(modelNode1,iVar11,modelNodeRuntime);
    if ((armyRuntime->terrainContactLinkedChildState).linkedChildDispatchState == 5) {
      uVar12 = pMVar17[1].classLinkState.armyLinkOrState6C.classState;
      if (((uVar12 != 0) && (uVar12 < worldRuntime->dwordArrayCount)) &&
         ((worldRuntime->dwordArray != (dword *)0x0 &&
          (bVar14 = false, worldRuntime->dwordArray[uVar12] != 0)))) {
        uVar15 = TerrainGrid_TestProjectedCellMaskBits01Cf
                           ((modelNodeRuntime->worldTransform).translation.y,
                            (modelNodeRuntime->worldTransform).translation.x,worldRuntime);
        iVar16 = (int)((ulonglong)uVar15 >> 0x20);
        if (!bVar14) {
          SpatialSound_PlayPositionedOneShot
                    (*(SpatialSoundMaximumDistanceQ12 *)(iVar16 + 0x7c),
                     *(SpatialSoundGainQ15 *)(iVar16 + 0x78),worldPosition_03,
                     (DirectSoundVoiceSet **)uVar15);
        }
      }
    }
    break;
  case 5:
    modelNodeRuntime->primaryTextureOffsetV =
         modelNodeRuntime->primaryTextureOffsetV -
         pMVar17->attachmentCount0C * g_InGameSimulationStepTicks;
    if (modelNodeRuntime->primaryTextureOffsetV < 1) {
      modelNodeRuntime->primaryTextureOffsetV = 0;
      (armyRuntime->terrainContactLinkedChildState).linkedChildDispatchState = 6;
    }
    break;
  case 6:
    (armyRuntime->terrainContactLinkedChildState).linkedChildDispatchState = 0;
ArmyRuntimeClass_ProcessPendingLinkedChildSpawnsAndDamageEffect:
    bVar14 = false;
    if (((armyRuntime->runtimeFlags & 9) == 0) &&
       (ArmyRuntime_TestWorldPointAllowedDefaultCf(in_ECX,pMVar17), !bVar14)) {
      bVar14 = false;
      if ((armyRuntime->linkedChildPendingCounts).slot0 != 0) {
        (armyRuntime->linkedChildPendingCounts).slot0 =
             (armyRuntime->linkedChildPendingCounts).slot0 - 1;
        ArmyRuntimeSpawner_CreateLinkedChildInstanceCf
                  ((armyRuntime->linkedChildOverloadedState).primaryCoordinateCommandOrHistory.
                   headingOrTurnValue,
                   (armyRuntime->terrainContactLinkedChildState).fallbackPosition1Q12,
                   (armyRuntime->terrainContactLinkedChildState).fallbackPosition0Q12,
                   g_ArmyLinkedChildAssetIdSlot0,worldRuntime,
                   (ArmyRuntimeLinkedChildMaskSlotView *)armyRuntime);
        if (!bVar14) {
          (armyRuntime->terrainContactLinkedChildState).linkedChildDispatchState = 1;
          uVar12 = *(uint *)(extraout_EDX_00 + 0x26c);
          if (((uVar12 != 0) && (uVar12 < worldRuntime->dwordArrayCount)) &&
             ((worldRuntime->dwordArray != (dword *)0x0 &&
              (bVar14 = false, worldRuntime->dwordArray[uVar12] != 0)))) {
            uVar15 = TerrainGrid_TestProjectedCellMaskBits01Cf
                               ((modelNodeRuntime->worldTransform).translation.y,
                                (modelNodeRuntime->worldTransform).translation.x,worldRuntime);
            iVar16 = (int)((ulonglong)uVar15 >> 0x20);
            if (!bVar14) {
              SpatialSound_PlayPositionedOneShot
                        (*(SpatialSoundMaximumDistanceQ12 *)(iVar16 + 0x7c),
                         *(SpatialSoundGainQ15 *)(iVar16 + 0x78),worldPosition,
                         (DirectSoundVoiceSet **)uVar15);
            }
          }
          break;
        }
      }
      bVar14 = false;
      if ((armyRuntime->linkedChildPendingCounts).slot1 != 0) {
        pbVar4 = &(armyRuntime->linkedChildPendingCounts).slot1;
        *pbVar4 = *pbVar4 - 1;
        ArmyRuntimeSpawner_CreateLinkedChildInstanceCf
                  ((armyRuntime->linkedChildOverloadedState).rightHeadingCommandOrSpawnValue.
                   headingOrTurnValue,
                   (armyRuntime->linkedChildOverloadedState).leftHeadingCommandOrSpawnValue.
                   headingOrTurnValue,
                   (armyRuntime->linkedChildOverloadedState).secondaryCoordinateCommandOrHistory.
                   headingOrTurnValue,g_ArmyLinkedChildAssetIdSlot1,worldRuntime,
                   (ArmyRuntimeLinkedChildMaskSlotView *)armyRuntime);
        if (!bVar14) {
          (armyRuntime->terrainContactLinkedChildState).linkedChildDispatchState = 1;
          uVar12 = *(uint *)(extraout_EDX_01 + 0x26c);
          if ((((uVar12 != 0) && (uVar12 < worldRuntime->dwordArrayCount)) &&
              (worldRuntime->dwordArray != (dword *)0x0)) &&
             (bVar14 = false, worldRuntime->dwordArray[uVar12] != 0)) {
            uVar15 = TerrainGrid_TestProjectedCellMaskBits01Cf
                               ((modelNodeRuntime->worldTransform).translation.y,
                                (modelNodeRuntime->worldTransform).translation.x,worldRuntime);
            iVar16 = (int)((ulonglong)uVar15 >> 0x20);
            if (!bVar14) {
              SpatialSound_PlayPositionedOneShot
                        (*(SpatialSoundMaximumDistanceQ12 *)(iVar16 + 0x7c),
                         *(SpatialSoundGainQ15 *)(iVar16 + 0x78),worldPosition_00,
                         (DirectSoundVoiceSet **)uVar15);
            }
          }
          break;
        }
      }
      bVar14 = false;
      if ((armyRuntime->linkedChildPendingCounts).slot2 != 0) {
        pbVar4 = &(armyRuntime->linkedChildPendingCounts).slot2;
        *pbVar4 = *pbVar4 - 1;
        ArmyRuntimeSpawner_CreateLinkedChildInstanceCf
                  ((armyRuntime->linkedChildSpawnParameters).parameter2,
                   (armyRuntime->linkedChildSpawnParameters).parameter1,
                   (armyRuntime->linkedChildSpawnParameters).parameter0,
                   g_ArmyLinkedChildAssetIdSlot2,worldRuntime,
                   (ArmyRuntimeLinkedChildMaskSlotView *)armyRuntime);
        if (!bVar14) {
          (armyRuntime->terrainContactLinkedChildState).linkedChildDispatchState = 1;
          uVar12 = *(uint *)(extraout_EDX_02 + 0x26c);
          if (((uVar12 != 0) && (uVar12 < worldRuntime->dwordArrayCount)) &&
             ((worldRuntime->dwordArray != (dword *)0x0 &&
              (bVar14 = false, worldRuntime->dwordArray[uVar12] != 0)))) {
            uVar15 = TerrainGrid_TestProjectedCellMaskBits01Cf
                               ((modelNodeRuntime->worldTransform).translation.y,
                                (modelNodeRuntime->worldTransform).translation.x,worldRuntime);
            iVar16 = (int)((ulonglong)uVar15 >> 0x20);
            if (!bVar14) {
              SpatialSound_PlayPositionedOneShot
                        (*(SpatialSoundMaximumDistanceQ12 *)(iVar16 + 0x7c),
                         *(SpatialSoundGainQ15 *)(iVar16 + 0x78),worldPosition_01,
                         (DirectSoundVoiceSet **)uVar15);
            }
          }
        }
      }
    }
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)armyRuntime);
  return;
ArmyRuntimeClass_AdvanceSecondaryArmyAssetCandidate:
  pdVar13 = pdVar13 + 1;
  FVar10 = FVar10 - 1;
  if (FVar10 == 0) goto ArmyRuntimeClass_DispatchLinkedChildSpawnAndDamageEffectState;
  goto ArmyRuntimeClass_SelectAffordableSecondaryArmyAssetLoop;
}

/* Address: 0x00524740.
   Ownership: gameplay/army/runtime.
   Purpose: This function object claims Listing ownership for a previously unowned multi-entry/shared-
   tail/computed-dispatch region; it does not assert that every member entry is an independent ABI-level function.
   Runtime-update partition slots 0-23 receive (worldRuntime, armyRuntime). Exact disjoint ranges, terminal
   instructions, inherited register state, shared exits, and caller/table references were revalidated. No function
   splitting or boundary change is permitted.
   Local calls: ArmyRuntime_UpdateTimedShotAndEffectEmitters, ArmyRuntime_UpdateAnimatedModelSubnodes,
   ArmyRuntime_CreateInstanceFromAssetCf.
   Cross-module calls: ModelLookupTable_ContainsPackedKeyCf [assets/model/definitions],
   ModelNodeRuntime_TransformLocalPointRegs [world/model/hierarchy], WorldRuntime_UnlinkNodeFromOwnerListD8
   [world/runtime/core], FixedMath_Atan2Angle16 [core/math/fixed], TerrainGrid_TestProjectedCellMaskBits01Cf
   [world/terrain/grid], SpatialSound_PlayPositionedOneShot [audio/spatial/runtime].
*/
void ArmyRuntimeClassUpdateSlot13_PrepareModelAndDispatchByClassId
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  Q12 *pQVar1;
  FactionRelationCounter *pFVar2;
  FactionArmyAssetCount *pFVar3;
  Q12 QVar4;
  int iVar5;
  int iVar6;
  void *pvVar7;
  ModelPackedPointRecord *pMVar8;
  int iVar9;
  dword dVar10;
  ArmyRuntimeSlot *armySlot1;
  undefined4 extraout_ECX;
  Q12 extraout_ECX_00;
  undefined4 extraout_ECX_01;
  Q12 targetWorldXQ12;
  undefined4 extraout_ECX_02;
  uint extraout_ECX_03;
  undefined4 extraout_ECX_04;
  uint extraout_ECX_05;
  Q12 worldXQ12;
  GraphicsFixedVec3 *worldPosition;
  GraphicsFixedVec3 *worldPosition_00;
  FactionArmyAssetCount FVar11;
  uint extraout_EDX;
  Q12 worldYQ12;
  int *extraout_EDX_00;
  DirectSoundVoiceSet **voiceSetRef;
  uint uVar12;
  dword *pdVar13;
  bool bVar14;
  undefined1 uVar15;
  undefined8 uVar16;
  InGameNotificationMovieId notificationMovieId;
  GameEntityRuntime *entityRuntime1;
  ModelRuntimeNode *modelNode1;
  
  modelNode1 = armyRuntime->modelNodeRuntime;
  bVar14 = false;
  if ((((armyRuntime->articulatedContact).fallbackPosition1Q12 & 1U) != 0) &&
     (pMVar8 = (ModelPackedPointRecord *)
               ModelLookupTable_ContainsPackedKeyCf(1,5,(modelNode1->modelPayload).modelResource),
     !bVar14)) {
    uVar16 = ModelNodeRuntime_TransformLocalPointRegs(extraout_ECX,pMVar8,modelNode1);
    armyRuntime->movementTarget0Q12 = (Q12)uVar16;
    armyRuntime->movementTarget1Q12 = extraout_ECX_00;
    pQVar1 = &(armyRuntime->articulatedContact).fallbackPosition1Q12;
    *pQVar1 = *pQVar1 & 0xfffffffe;
  }
  QVar4 = (armyRuntime->articulatedContact).fallbackPosition0Q12;
  uVar16 = CONCAT44(armyRuntime->definitionOrAsset,QVar4);
  if ((3 < modelNode1->childCount) &&
     (uVar16 = CONCAT44(armyRuntime->definitionOrAsset,QVar4),
     modelNode1->childNodes[3] != (ModelRuntimeNode *)0x0)) {
    uVar16 = WorldRuntime_UnlinkNodeFromOwnerListD8((WorldRuntimeNode *)modelNode1->childNodes[3]);
    modelNode1->childNodes[3] = (ModelRuntimeNode *)0x0;
  }
  iVar9 = (int)((ulonglong)uVar16 >> 0x20);
                    
  switch((int)uVar16) {
  case 0:
    if ((armyRuntime->runtimeFlags & 0x40) == 0) {
      if ((armyRuntime->runtimeFlags & 0xc9) == 0) {
        iVar9 = (armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex;
        FVar11 = g_GameFactionRuntimeImage.records[iVar9].secondaryArmyAssetCount;
        pdVar13 = g_GameFactionRuntimeImage.records[iVar9].secondaryArmyAssetPointersOrIds;
        if (FVar11 != 0) {
          do {
            if (((*(uint *)(*pdVar13 + 0x14) & *(uint *)((int)armyRuntime->definitionOrAsset + 0xc4)
                 ) != 0) &&
               (uVar12 = *(uint *)(*pdVar13 + 0x28),
               uVar12 <= g_GameFactionRuntimeImage.records[iVar9].xeniteCurrentQ4)) {
              dVar10 = *pdVar13;
              g_GameFactionRuntimeImage.records[iVar9].xeniteCurrentQ4 =
                   g_GameFactionRuntimeImage.records[iVar9].xeniteCurrentQ4 - uVar12;
              uVar12 = *(uint *)(dVar10 + 0x24);
              iVar5 = *(int *)(dVar10 + 0x2c);
              if ((g_UiCommandRuntimeFlags & 0x100000) != 0) {
                uVar12 = (uVar12 >> 4) + 1;
              }
              dVar10 = *(dword *)(dVar10 + 8);
              armyRuntime->ownerValue68 = uVar12;
              armyRuntime->fallbackWorldXQ12 = iVar5;
              armyRuntime->classState60 = dVar10;
              armyRuntime->runtimeTimer = armyRuntime->runtimeTimer + iVar5;
              armyRuntime->ownerValue64 = 0;
              pFVar3 = &g_GameFactionRuntimeImage.records[iVar9].secondaryArmyAssetCount;
              *pFVar3 = *pFVar3 - 1;
              do {
                *pdVar13 = pdVar13[1];
                pdVar13 = pdVar13 + 1;
                FVar11 = FVar11 - 1;
              } while (FVar11 != 0);
              (armyRuntime->articulatedContact).fallbackPosition0Q12 = 1;
              armyRuntime->runtimeFlags = armyRuntime->runtimeFlags | 0x100;
              break;
            }
            pdVar13 = pdVar13 + 1;
            FVar11 = FVar11 - 1;
          } while (FVar11 != 0);
        }
      }
    }
    else if ((armyRuntime->runtimeFlags & 9) == 0) {
      ArmyRuntime_UpdateTimedShotAndEffectEmitters(worldRuntime,armyRuntime);
      ArmyRuntime_UpdateAnimatedModelSubnodes(worldRuntime,armyRuntime);
    }
    break;
  case 1:
    if ((armyRuntime->runtimeFlags & 9) == 0) {
      armyRuntime->ownerValue64 = armyRuntime->ownerValue64 + g_InGameSimulationStepTicks;
      ArmyRuntime_UpdateTimedShotAndEffectEmitters(worldRuntime,armyRuntime);
      ArmyRuntime_UpdateAnimatedModelSubnodes(worldRuntime,armyRuntime);
      uVar15 = extraout_EDX < armyRuntime->ownerValue68;
      if ((!(bool)uVar15) &&
         (pMVar8 = (ModelPackedPointRecord *)
                   ModelLookupTable_ContainsPackedKeyCf
                             (1,5,(modelNode1->modelPayload).modelResource), !(bool)uVar15)) {
        uVar16 = ModelNodeRuntime_TransformLocalPointRegs(extraout_ECX_02,pMVar8,modelNode1);
        iVar9 = (int)uVar16;
        uVar12 = extraout_ECX_03;
        pMVar8 = (ModelPackedPointRecord *)
                 ModelLookupTable_ContainsPackedKeyCf(0,5,(modelNode1->modelPayload).modelResource);
        if (!(bool)uVar15) {
          uVar16 = ModelNodeRuntime_TransformLocalPointRegs(extraout_ECX_04,pMVar8,modelNode1);
          uVar15 = uVar12 < extraout_ECX_05;
          dVar10 = FixedMath_Atan2Angle16(uVar12 - extraout_ECX_05,iVar9 - (int)uVar16);
          entityRuntime1 = armyRuntime->linkedEntityRuntime;
          armySlot1 = ArmyRuntime_CreateInstanceFromAssetCf
                                (4,dVar10,worldXQ12,worldYQ12,
                                 (entityRuntime1->common).ownership.ownerIndex,
                                 armyRuntime->classState60,worldRuntime);
          if (!(bool)uVar15) {
            pFVar2 = &g_GameFactionRuntimeImage.records
                      [(entityRuntime1->common).ownership.ownerIndex].relationCounterA;
            *pFVar2 = *pFVar2 + 1;
            pvVar7 = armyRuntime->definitionOrAsset;
            armySlot1->movementStateFlags = armySlot1->movementStateFlags | 0x402;
            uVar12 = *(uint *)((int)pvVar7 + 0x26c);
            if (((uVar12 != 0) && (uVar12 < worldRuntime->dwordArrayCount)) &&
               (worldRuntime->dwordArray != (dword *)0x0)) {
              modelNode1 = armyRuntime->modelNodeRuntime;
              bVar14 = false;
              if (*(int *)((modelNode1->modelPayload).reserved2C_33 + uVar12 * 4 + -0x38) != 0) {
                uVar16 = TerrainGrid_TestProjectedCellMaskBits01Cf
                                   ((modelNode1->worldTransform).translation.y,
                                    (modelNode1->worldTransform).translation.x,worldRuntime);
                armySlot1 = (ArmyRuntimeSlot *)uVar16;
                if (!bVar14) {
                  uVar16 = SpatialSound_PlayPositionedOneShot
                                     (*(SpatialSoundMaximumDistanceQ12 *)((int)pvVar7 + 0x7c),
                                      *(SpatialSoundGainQ15 *)((int)pvVar7 + 0x78),worldPosition,
                                      (DirectSoundVoiceSet **)((ulonglong)uVar16 >> 0x20));
                  armySlot1 = (ArmyRuntimeSlot *)uVar16;
                }
              }
            }
            iVar9 = armyRuntime->fallbackWorldXQ12;
            armyRuntime->linkedArmyRuntimeOrSavedOffset = armySlot1;
            iVar5 = armySlot1->factionIndex;
            (armyRuntime->articulatedContact).fallbackPosition0Q12 = 2;
            armyRuntime->fallbackWorldXQ12 = 0;
            armyRuntime->runtimeTimer = armyRuntime->runtimeTimer - iVar9;
            armySlot1->movementStateFlags = armySlot1->movementStateFlags | 2;
            *(ArmyRuntimeSlot **)((int)armySlot1->definitionOrAsset + 0xf0) = armyRuntime;
            if (iVar5 == worldRuntime->activeFactionRuntimeIndex) {
              ArmyAssetRegistry_FindByIdCf(armyRuntime->classState60);
              dVar10 = (worldRuntime->motion).pitchAngle;
              iVar9 = extraout_EDX_00[1];
              iVar5 = *extraout_EDX_00;
              iVar6 = *(int *)(iVar9 + 0x14);
              *(int *)(iVar5 + 0x1b0) = *(int *)(iVar5 + 0x1b0) + 1;
              notificationMovieId = *(InGameNotificationMovieId *)(iVar5 + 0x160);
              if (*(int *)(iVar5 + 0x1b0) != 1) {
                notificationMovieId = *(InGameNotificationMovieId *)(iVar5 + 0x164);
              }
              InGameNotificationQueue_InsertPriorityRecord
                        (ARMY_CREATED,0,dVar10,iVar6 + 0x8800U & 0xffff,*(Q12 *)(iVar9 + 0x98),
                         *(Q12 *)(iVar9 + 0x94),2,notificationMovieId);
            }
          }
        }
      }
    }
    break;
  case 2:
    modelNode1->primaryTextureOffsetV =
         modelNode1->primaryTextureOffsetV + *(int *)(iVar9 + 0xc) * g_InGameSimulationStepTicks;
    bVar14 = (uint)modelNode1->primaryTextureOffsetV < 0x80000;
    if (0x7ffff < modelNode1->primaryTextureOffsetV) {
      modelNode1->primaryTextureOffsetV = 0x80000;
      (armyRuntime->articulatedContact).fallbackPosition0Q12 = 3;
      pMVar8 = (ModelPackedPointRecord *)
               ModelLookupTable_ContainsPackedKeyCf(1,5,(modelNode1->modelPayload).modelResource);
      if (!bVar14) {
        armySlot1 = armyRuntime->linkedArmyRuntimeOrSavedOffset;
        uVar16 = ModelNodeRuntime_TransformLocalPointRegs(extraout_ECX_01,pMVar8,modelNode1);
        entityRuntime1 = armySlot1->definitionOrAsset;
        ArmyRuntime_StartMoveCommandWithAuxiliaryValues
                  (targetWorldXQ12,(int)((ulonglong)uVar16 >> 0x20),armyRuntime->movementTarget1Q12,
                   armyRuntime->movementTarget0Q12,targetWorldXQ12,(Q12)uVar16,
                   (ArmyMovementRuntime *)armySlot1);
        (entityRuntime1->common).pathingAndImpactState.pathingReferences.overlappingEntity =
             (GameEntityRuntime *)armyRuntime;
      }
    }
    break;
  case 3:
    if ((armyRuntime->linkedArmyRuntimeOrSavedOffset == (ArmyRuntimeSlot *)0x0) ||
       (*(ArmyRuntimeSlot **)
         ((int)armyRuntime->linkedArmyRuntimeOrSavedOffset->definitionOrAsset + 0xf0) != armyRuntime
       )) {
      pvVar7 = armyRuntime->definitionOrAsset;
      (armyRuntime->articulatedContact).fallbackPosition0Q12 = 4;
      armyRuntime->linkedArmyRuntimeOrSavedOffset = (ArmyRuntimeSlot *)0x0;
      uVar12 = *(uint *)((int)pvVar7 + 0x26c);
      if ((uVar12 != 0) &&
         ((uVar12 < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != (dword *)0x0)))) {
        modelNode1 = armyRuntime->modelNodeRuntime;
        bVar14 = false;
        if ((*(int *)((modelNode1->modelPayload).reserved2C_33 + uVar12 * 4 + -0x38) != 0) &&
           (TerrainGrid_TestProjectedCellMaskBits01Cf
                      ((modelNode1->worldTransform).translation.y,
                       (modelNode1->worldTransform).translation.x,worldRuntime), !bVar14)) {
          SpatialSound_PlayPositionedOneShot
                    (*(SpatialSoundMaximumDistanceQ12 *)((int)pvVar7 + 0x7c),
                     *(SpatialSoundGainQ15 *)((int)pvVar7 + 0x78),worldPosition_00,voiceSetRef);
        }
      }
    }
    break;
  case 4:
    modelNode1->primaryTextureOffsetV =
         modelNode1->primaryTextureOffsetV - *(int *)(iVar9 + 0xc) * g_InGameSimulationStepTicks;
    if (modelNode1->primaryTextureOffsetV < 1) {
      modelNode1->primaryTextureOffsetV = 0;
      (armyRuntime->articulatedContact).fallbackPosition0Q12 = 0;
      armyRuntime->runtimeFlags = armyRuntime->runtimeFlags & 0xfffffeff;
    }
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,armyRuntime);
  return;
}

/* Address: 0x005240F0.
   Ownership: gameplay/army/runtime.
   Purpose: This function object claims Listing ownership for a previously unowned multi-entry/shared-
   tail/computed-dispatch region; it does not assert that every member entry is an independent ABI-level function.
   Runtime-update partition slots 0-23 receive (worldRuntime, armyRuntime). Exact disjoint ranges, terminal
   instructions, inherited register state, shared exits, and caller/table references were revalidated. No function
   splitting or boundary change is permitted. [RESOURCE_FUEL_ENERGY_CAPACITY_SEPARATION_CLOSURE] Production path:
   queue candidate +0x28 is paid once from faction Xenite stock; candidate +0x2C is added to the ArmyRuntime active
   Energy load for the production duration and removed on completion.
   Local calls: ArmyRuntime_UpdateTimedShotAndEffectEmitters, ArmyRuntime_UpdateAnimatedModelSubnodes.
   Cross-module calls: ArmyAssetRegistry_FindByIdCf [assets/army/catalog], UiCommandSpriteVariantA_RebuildGrid
   [ui/ingame/commands], ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf [assets/model/definitions],
   InGameNotificationQueue_InsertPriorityRecord [ui/ingame/runtime], ArmyRuntime_EmitDamageThresholdEffect
   [gameplay/army/combat].
*/
void ArmyRuntimeClassUpdateSlot11_DispatchByClassId
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  FactionArmyAssetCount *pFVar1;
  ArmyRuntimeTimer *pAVar2;
  int iVar3;
  int iVar4;
  dword dVar5;
  AngleTurn32 AVar6;
  ArmyAssetRecordPrefix *armyDefinition1;
  ModelDefinitionRecordPrefix *modelDefinition1;
  FactionArmyAssetCount FVar7;
  uint extraout_EDX;
  int extraout_EDX_00;
  uint uVar8;
  dword *pdVar9;
  bool bVar10;
  PckModelDefinitionIdCatalog notificationMovieId;
  GameEntityRuntime *entityRuntime1;
  ModelRuntimeNode *modelNode1;

  switch((armyRuntime->articulatedContact).fallbackPosition0Q12) {
  case 0:
    if ((armyRuntime->runtimeFlags & 0x40) == 0) {
      if ((armyRuntime->runtimeFlags & 0xc9) == 0) {
        iVar3 = (armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex;
        pdVar9 = g_GameFactionRuntimeImage.records[iVar3].secondaryArmyAssetPointersOrIds;
        for (FVar7 = g_GameFactionRuntimeImage.records[iVar3].secondaryArmyAssetCount; FVar7 != 0;
            FVar7 = FVar7 - 1) {
          dVar5 = *pdVar9;
          if (((*(uint *)(dVar5 + 0x14) & 0x10) != 0) &&
             (*(uint *)(dVar5 + 0x28) <= g_GameFactionRuntimeImage.records[iVar3].xeniteCurrentQ4))
          {
            g_GameFactionRuntimeImage.records[iVar3].xeniteCurrentQ4 =
                 g_GameFactionRuntimeImage.records[iVar3].xeniteCurrentQ4 - *(uint *)(dVar5 + 0x28);
            uVar8 = *(uint *)(dVar5 + 0x24);
            iVar4 = *(int *)(dVar5 + 0x2c);
            if ((g_UiCommandRuntimeFlags & 0x100000) != 0) {
              uVar8 = (uVar8 >> 4) + 1;
            }
            dVar5 = *(dword *)(dVar5 + 8);
            armyRuntime->ownerValue68 = uVar8;
            armyRuntime->fallbackWorldXQ12 = iVar4;
            armyRuntime->classState60 = dVar5;
            armyRuntime->runtimeTimer = armyRuntime->runtimeTimer + iVar4;
            armyRuntime->ownerValue64 = 0;
            pFVar1 = &g_GameFactionRuntimeImage.records[iVar3].secondaryArmyAssetCount;
            *pFVar1 = *pFVar1 - 1;
            do {
              *pdVar9 = pdVar9[1];
              pdVar9 = pdVar9 + 1;
              FVar7 = FVar7 - 1;
            } while (FVar7 != 0);
            (armyRuntime->articulatedContact).fallbackPosition0Q12 = 1;
            armyRuntime->runtimeFlags = armyRuntime->runtimeFlags | 0x100;
            break;
          }
          pdVar9 = pdVar9 + 1;
        }
      }
    }
    else if ((armyRuntime->runtimeFlags & 9) == 0) {
      ArmyRuntime_UpdateTimedShotAndEffectEmitters(worldRuntime,armyRuntime);
      ArmyRuntime_UpdateAnimatedModelSubnodes(worldRuntime,armyRuntime);
    }
    break;
  case 1:
    if ((armyRuntime->runtimeFlags & 9) == 0) {
      entityRuntime1 = armyRuntime->linkedEntityRuntime;
      armyRuntime->ownerValue64 = armyRuntime->ownerValue64 + g_InGameSimulationStepTicks;
      ArmyRuntime_UpdateTimedShotAndEffectEmitters(worldRuntime,armyRuntime);
      ArmyRuntime_UpdateAnimatedModelSubnodes(worldRuntime,armyRuntime);
      if (armyRuntime->ownerValue68 <= extraout_EDX) {
        iVar3 = (entityRuntime1->common).ownership.ownerIndex;
        uVar8 = armyRuntime->fallbackWorldXQ12;
        armyRuntime->fallbackWorldXQ12 = 0;
        (armyRuntime->articulatedContact).fallbackPosition0Q12 = 0;
        armyRuntime->runtimeFlags = armyRuntime->runtimeFlags & 0xfffffeff;
        pAVar2 = &armyRuntime->runtimeTimer;
        bVar10 = *pAVar2 < uVar8;
        *pAVar2 = *pAVar2 - uVar8;
        armyDefinition1 = ArmyAssetRegistry_FindByIdCf(armyRuntime->classState60);
        armyRuntime->classState60 = 0;
        if (!bVar10) {
          uVar8 = g_GameFactionRuntimeImage.records[iVar3].primaryArmyAssetCount;
          if (uVar8 < 0x40) {
            iVar4 = worldRuntime->activeFactionRuntimeIndex;
            *(ArmyAssetRecordPrefix **)(iVar3 * 0x740 + 0x50f520 + uVar8 * 4) = armyDefinition1;
            pFVar1 = &g_GameFactionRuntimeImage.records[iVar3].primaryArmyAssetCount;
            *pFVar1 = *pFVar1 + 1;
            if (iVar4 == (entityRuntime1->common).ownership.ownerIndex) {
              dVar5 = armyDefinition1->rootNodeOffsetOrPointer;
              UiCommandSpriteVariantA_RebuildGrid((UiNodeBase *)worldRuntime);
              modelDefinition1 =
                   ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                             ((entityRuntime1->common).ownership.ownerIndex,dVar5);
              modelNode1 = armyRuntime->modelNodeRuntime;
              dVar5 = *(dword *)(extraout_EDX_00 + 0x74);
              AVar6 = (modelNode1->modelPayload).worldRotationAngle2;
              modelDefinition1[0x24].byteSize = modelDefinition1[0x24].byteSize + 1;
              notificationMovieId = modelDefinition1[0x1d].flags;
              if (modelDefinition1[0x24].byteSize != 1) {
                notificationMovieId = modelDefinition1[0x1d].definitionId;
              }
              InGameNotificationQueue_InsertPriorityRecord
                        (ARMY_CREATED,0,dVar5,AVar6 + 0x1800 & 0xffff,
                         (modelNode1->worldTransform).translation.y,
                         (modelNode1->worldTransform).translation.x,3,notificationMovieId);
            }
          }
        }
      }
    }
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,armyRuntime);
  return;
}

/* Address: 0x00525020.
   Ownership: gameplay/army/runtime.
   Purpose: Binary entry is anchored by g_CodePointerTable_0051FC98[14]@0051FC98. Runtime-update partition slots
   0-23 receive (worldRuntime, armyRuntime).
   Local calls: ArmyRuntime_UpdateTimedShotAndEffectEmitters, ArmyRuntime_UpdateAnimatedModelSubnodes.
   Cross-module calls: FieldGrid_WorldToGridQ12 [world/terrain/grid], ArmyRuntime_EmitDamageThresholdEffect
   [gameplay/army/combat].
*/
void ArmyRuntimeClass_UpdateGridBoundEffectsAndModels
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  int iVar1;
  dword in_EDX;
  int FVar3;
  FieldCellPackedFlagsAndMaterial FVar2;
  dword extraout_EDX;
  qword qVar3;
  FieldGridAsset *fieldGrid1;
  
  if ((1 < armyRuntime->actionVector2Q12) && ((armyRuntime->runtimeFlags & 9) == 0)) {
    qVar3 = FieldGrid_WorldToGridQ12
                      ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,
                       (armyRuntime->modelNodeRuntime->worldTransform).translation.x);
    iVar1 = ((int)qVar3 >> 0xb) + 1 >> 1;
    FVar3 = (int)((longlong)qVar3 >> 0x2b) + 1 >> 1;
    fieldGrid1 = worldRuntime->fieldGrid;
    if ((0 < iVar1) && (0 < FVar3)) {
      if ((iVar1 + 1 < (int)fieldGrid1->gridWidth) && (FVar3 + 1 < (int)fieldGrid1->gridHeight)) {
        iVar1 = FVar3 * fieldGrid1->gridWidth + iVar1;
        FVar2 = 0x800 << ((byte)*(undefined4 *)((int)armyRuntime->definitionOrAsset + 0xc0) & 0x1f);
        fieldGrid1->cells[iVar1].runtime7C =
             (armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex << 0xd | FVar2 |
             *(int *)((int)armyRuntime->definitionOrAsset + 200) << 0x18;
        if ((fieldGrid1->cells[iVar1].flagsAndMaterial & FVar2) != 0) {
          *(int *)(fieldGrid1->cells[iVar1].runtime58_6F + 0x14) =
               (int)armyRuntime - g_ModelRuntimeRebaseDelta;
          ArmyRuntime_UpdateTimedShotAndEffectEmitters(worldRuntime,armyRuntime);
          ArmyRuntime_UpdateAnimatedModelSubnodes(worldRuntime,armyRuntime);
        }
      }
    }
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,armyRuntime);
  return;
}

/* Address: 0x005274D0.
   Ownership: gameplay/army/runtime.
   Purpose: Army class command-table handler shared by the Group A class entries. Class-command handler used by the
   ten non-default class slots.
   Local calls: ArmyRuntime_TestClass13ProximityCandidateCf, ArmyRuntime_TestModelAttachmentProximityCf.
   Cross-module calls: FixedMath_Atan2Angle16 [core/math/fixed], ArmyRuntime_ApplyImpactDamageAndFinalizeState
   [gameplay/army/combat], FieldGrid_ApplyHeightAtWorldPointAndRefreshNeighborsCf [world/terrain/grid],
   EffectDefinitionRegistry_FindByIdWithErrorCf [assets/effect/catalog],
   EffectRuntimePool_CreateInstanceFromDefinitionCf [world/effects/runtime].
*/
void ArmyRuntime_ClassCommandHandlerGroupACf
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  void *pvVar1;
  uint uVar2;
  longlong lVar3;
  longlong lVar4;
  ArmyRuntimeSlot *candidateArmyRuntime;
  ArmyRuntimeSlot *extraout_EAX;
  ArmyRuntimeSlot *armySlot2;
  dword impactAngle;
  int iVar5;
  int iVar6;
  EffectDefinition *effectDefinition;
  ModelRuntimeNode *extraout_ECX;
  ModelRuntimeNode *extraout_ECX_00;
  ModelRuntimeNode *extraout_ECX_01;
  ModelRuntimeNode *modelNode2;
  undefined4 extraout_ECX_02;
  ArmyRuntimeSlot *sourceArmyRuntime;
  ArmyRuntimeSlot *extraout_EDX;
  ArmyRuntimeSlot *extraout_EDX_00;
  undefined4 extraout_EDX_01;
  ModelRuntimeNode *modelNode1;
  bool bVar7;
  DamageAmount32 damageAmount;
  int iVar8;
  int iVar9;
  ArmyRuntimeSlot *armySlot1;
  
  modelNode1 = (ModelRuntimeNode *)worldRuntime->ownerListHead;
  modelNode2 = armyRuntime->modelNodeRuntime;
  if ((modelNode1 != (ModelRuntimeNode *)0x0) &&
     (armySlot1 = armyRuntime, (g_UiCommandRuntimeFlags & 4) == 0)) {
    do {
      if (modelNode1->ownerClassId == MODEL_RUNTIME_CLASS_00) {
        pvVar1 = ((modelNode1->runtimePayload).armyRuntime)->definitionOrAsset;
        if ((modelNode2 != modelNode1) &&
           ((bVar7 = false, *(int *)((int)pvVar1 + 0x4c) == 0 ||
            (uVar2 = *(uint *)((int)pvVar1 + 0x4c), bVar7 = uVar2 < 0xc, uVar2 == 0xc)))) {
          ArmyRuntime_TestClass13ProximityCandidateCf
                    ((modelNode1->runtimePayload).armyRuntime,armySlot1);
          armySlot2 = candidateArmyRuntime;
          modelNode2 = extraout_ECX;
          if (!bVar7) {
            ArmyRuntime_TestModelAttachmentProximityCf(candidateArmyRuntime,sourceArmyRuntime);
            armySlot2 = extraout_EAX;
            modelNode2 = extraout_ECX_00;
            armySlot1 = extraout_EDX;
            if (bVar7)
            goto 
            ArmyRuntime_ClassCommandHandlerGroupACf_AdvanceOwnerScanAfterProximityOrImpactDecision;
          }
          damageAmount = 0x100000;
          impactAngle = FixedMath_Atan2Angle16
                                  ((modelNode1->worldTransform).translation.y -
                                   (modelNode2->worldTransform).translation.y,
                                   (modelNode1->worldTransform).translation.x -
                                   (modelNode2->worldTransform).translation.x);
          ArmyRuntime_ApplyImpactDamageAndFinalizeState(impactAngle,damageAmount,armySlot2);
          modelNode2 = extraout_ECX_01;
          armySlot1 = extraout_EDX_00;
        }
      }
ArmyRuntime_ClassCommandHandlerGroupACf_AdvanceOwnerScanAfterProximityOrImpactDecision:
      modelNode1 = (ModelRuntimeNode *)(modelNode1->common).nextNode;
    } while (modelNode1 != (ModelRuntimeNode *)0x0);
  }
  modelNode1 = armyRuntime->modelNodeRuntime;
  if ((*(uint *)((int)armyRuntime->definitionOrAsset + 0x68) & 0x20) == 0) {
    FieldGrid_ApplyHeightAtWorldPointAndRefreshNeighborsCf
              (*(TerrainHeightBrushDeltaSource *)((int)armyRuntime->definitionOrAsset + 0xdc),
               (modelNode1->worldTransform).translation.z,(modelNode1->worldTransform).translation.y
               ,(modelNode1->worldTransform).translation.x,worldRuntime->fieldGrid);
  }
  modelNode1 = (ModelRuntimeNode *)worldRuntime->ownerListHead;
  modelNode2 = (armyRuntime->linkedEntityRuntime->common).ownership.modelNode;
  if (modelNode1 != (ModelRuntimeNode *)0x0) {
    armySlot1 = (ArmyRuntimeSlot *)0x0;
    iVar8 = 0;
    do {
      iVar9 = iVar8;
      if ((((modelNode1->ownerClassId == MODEL_RUNTIME_CLASS_00) && (modelNode1 != modelNode2)) &&
          (iVar5 = *(int *)((int)((modelNode1->runtimePayload).armyRuntime)->definitionOrAsset +
                           0x19c),
          (armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex ==
          (((modelNode1->runtimePayload).armyRuntime)->linkedEntityRuntime->common).ownership.
          ownerIndex)) && (iVar5 != 0)) {
        iVar5 = iVar5 + *(int *)((int)armyRuntime->definitionOrAsset + 0x1a8);
        iVar6 = (modelNode2->worldTransform).translation.x -
                (modelNode1->worldTransform).translation.x;
        lVar4 = (longlong)iVar5 * (longlong)iVar5 - (longlong)iVar6 * (longlong)iVar6;
        if ((-1 < lVar4) &&
           (iVar5 = (modelNode2->worldTransform).translation.y -
                    (modelNode1->worldTransform).translation.y,
           lVar3 = (longlong)iVar5 * (longlong)iVar5,
           -1 < (int)(((int)((ulonglong)lVar4 >> 0x20) - (int)((ulonglong)lVar3 >> 0x20)) -
                     (uint)((uint)lVar4 < (uint)lVar3)))) {
          armySlot2 = (modelNode1->runtimePayload).armyRuntime;
          iVar9 = iVar8 + 1;
          if ((*(int *)((int)armySlot2->definitionOrAsset + 0x4c) == 0x12) &&
             ((armySlot2->runtimeFlags & 0x18) == 0)) {
            iVar9 = iVar8;
            armySlot1 = armySlot2;
          }
        }
      }
      modelNode1 = (ModelRuntimeNode *)(modelNode1->common).nextNode;
      iVar8 = iVar9;
    } while (modelNode1 != (ModelRuntimeNode *)0x0);
    if ((iVar9 == 0) && (armySlot1 != (ArmyRuntimeSlot *)0x0)) {
      modelNode1 = armySlot1->modelNodeRuntime;
      bVar7 = false;
      armySlot1->runtimeFlags = armySlot1->runtimeFlags | 8;
      effectDefinition =
           EffectDefinitionRegistry_FindByIdWithErrorCf
                     (*(PckEffectDefinitionIdCatalog *)((int)armySlot1->definitionOrAsset + 0xc4));
      if (!bVar7) {
        EffectRuntimePool_CreateInstanceFromDefinitionCf
                  (extraout_ECX_02,extraout_EDX_01,EFFECT_RUNTIME_COMPLETION_SPAWN_ARMY_FROM_MODEL,
                   armySlot1->linkedEntityRuntime,(modelNode1->modelPayload).worldRotationAngle2,
                   (modelNode1->modelPayload).worldRotationAngle1,
                   (modelNode1->modelPayload).worldRotationAngle0,
                   (modelNode1->worldTransform).translation.z,
                   (modelNode1->worldTransform).translation.y,
                   (modelNode1->worldTransform).translation.x,effectDefinition,worldRuntime);
      }
    }
  }
  return;
}

/* Address: 0x0051D140.
   Ownership: gameplay/army/runtime.
   Purpose: Binary entry is anchored by g_ArmyRuntimeCallbackTable12[3]@00562DEC. Maintenance table phase
   terrainStateRefresh, object kind army. The 4x3 table bytes, target body, calling convention, and RET 0x08
   contract remain unchanged.
   Local calls: ArmyRuntime_InitializeTerrainOccupancyFlags.
   Cross-module calls: ModelNodeRuntime_UpdateStateTintRecursive [world/model/hierarchy].
*/
void ArmyRuntimeMaintenance_InitializeOccupancyAndStateTint
               (WorldRuntimeContext *worldRuntime,ModelRuntimeNode *modelNodeRuntime)

{
  ArmyRuntime_InitializeTerrainOccupancyFlags
            (worldRuntime,
             (ArmyRuntimeSlot *)
             ((modelNodeRuntime->runtimePayload).armyRuntime)->linkedEntityRuntime);
  ModelNodeRuntime_UpdateStateTintRecursive(modelNodeRuntime);
  return;
}

/* Address: 0x0051D280.
   Ownership: gameplay/army/runtime.
   Purpose: Binary entry is anchored by g_ArmyRuntimeCallbackTable12[9]@00562DEC. Maintenance table phase
   audioRefresh, object kind army. The 4x3 table bytes, target body, calling convention, and RET 0x08 contract
   remain unchanged.
   Local calls: ArmyRuntimeHierarchy_DispatchClassMethodDRecursive.
*/
void ArmyRuntimeMaintenance_DispatchClassMethodDRecursive
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ArmyRuntimeHierarchy_DispatchClassMethodDRecursive
            (worldRuntime,(ArmyRuntimeSlot *)armyRuntime->runtimeState48);
  return;
}

/* Address: 0x0051D2A0.
   Ownership: gameplay/army/runtime.
   Purpose: Binary entry is anchored by g_ArmyRuntimeCallbackTable12[0]@00562DEC. Maintenance table phase
   primaryUpdate, object kind army. The 4x3 table bytes, target body, calling convention, and RET 0x08 contract
   remain unchanged.
   Local calls: ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive.
   Cross-module calls: AiCombatDecision_UpdateTargetAssignment [gameplay/ai/combat].
*/
void ArmyRuntimeMaintenance_UpdateHierarchyAiAndTimers
               (WorldRuntimeContext *worldRuntime,WorldRuntimeNode *ownerNode)

{
  ArmyCommandGeneration *pAVar1;
  void *armyRuntime;
  ArmyRuntimeSlot *armyRuntime_00;
  undefined4 in_ECX;
  int iVar2;
  int extraout_EDX;
  undefined8 uVar3;
  
  armyRuntime = ownerNode->runtimePayload;
  armyRuntime_00 = *(ArmyRuntimeSlot **)((int)armyRuntime + 8);
  uVar3 = ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive
                    (in_ECX,armyRuntime,worldRuntime,armyRuntime);
  iVar2 = (int)((ulonglong)uVar3 >> 0x20);
  if ((int)uVar3 != 0) {
    AiCombatDecision_UpdateTargetAssignment(worldRuntime,armyRuntime_00);
    iVar2 = extraout_EDX;
  }
  pAVar1 = &armyRuntime_00->commandGeneration;
  *pAVar1 = *pAVar1 - g_InGameSimulationStepTicks;
  if ((int)*pAVar1 < 0) {
    armyRuntime_00->commandModeFlags = armyRuntime_00->commandModeFlags & 0xfffffff3;
  }
  if (((armyRuntime_00->movementStateFlags & 2) != 0) && (*(int *)(iVar2 + 0xf0) == 0)) {
    armyRuntime_00->movementStateFlags = armyRuntime_00->movementStateFlags & 0xfffffffd;
  }
  if (armyRuntime_00->runtimeStateA4 != 0) {
    armyRuntime_00->runtimeStateA4 = armyRuntime_00->runtimeStateA4 - 1;
  }
  return;
}

/* Address: 0x0051D6B0.
   Ownership: gameplay/army/runtime.
   Purpose: Allocates and zeroes the exact 0x48000-byte army runtime pool, records its base and base-minus-one
   relocation value, loads up to eight faction-dependent gfx/palette binding pairs, and creates runtime instances
   for eligible records in the fixed 768-entry army definition registry. CF and EAX errors are preserved.
   Local calls: ArmyRuntime_RenderPreviewTextureCf.
   Cross-module calls: MoviePlayback_AdvanceScheduledFrameAndTick [movie/runtime/playback],
   WidePath_SetExtensionCode [core/text/path], Package_LoadEntry [assets/package/runtime],
   ArmyGraphics_CopyFrontendPlayerPaletteAndTexture [gameplay/army/audio], Resource_Release
   [assets/resource/runtime].
*/
dword ArmyRuntime_InitializePoolAndGraphicsCf(void *ownerContext,word *graphicsBasePath)

{
  word wVar1;
  uint uVar2;
  ArmyRuntimeSlot *armySlot1;
  void *armyGraphicsAsset;
  GraphicsTextureSourceAsset *arg0;
  GraphicsTextureSet *pGVar3;
  GraphicsPaletteAsset *pGVar4;
  GraphicsPixelDimension previewHeight;
  int iVar5;
  dword arg0_00;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  word *path;
  word *path_00;
  dword arg2;
  int iVar6;
  ArmyAssetRecordPrefix **ppAVar7;
  word *pwVar8;
  undefined1 in_CF;
  undefined1 uVar9;
  bool bVar10;
  undefined8 uVar11;
  ArmyAssetRecordPrefix *armyDefinition1;
  
  armySlot1 = (*g_MemoryApi.alloc)(0x48000);
  if (!(bool)in_CF) {
    g_ArmyRuntimeRebaseBaseMinusOne = (void *)((int)&armySlot1[-1].selectionMetric5 + 3);
    g_ArmyRuntimeSlots = armySlot1;
    for (iVar5 = 0x12000; iVar5 != 0; iVar5 = iVar5 + -1) {
      armySlot1->definitionOrAsset = (void *)0x0;
      armySlot1 = (ArmyRuntimeSlot *)&armySlot1->modelNodeRuntime;
    }
    iVar6 = 0;
    iVar5 = 0x20;
    do {
      pwVar8 = graphicsBasePath;
      if (iVar5 == 0) break;
      iVar5 = iVar5 + -1;
      pwVar8 = graphicsBasePath + 1;
      wVar1 = *graphicsBasePath;
      graphicsBasePath = pwVar8;
    } while (wVar1 != 0);
    pwVar8 = pwVar8 + -1;
    g_MoviePlaybackScheduleSpan = 0x1a;
    do {
      uVar11 = MoviePlayback_AdvanceScheduledFrameAndTick();
      path = (word *)((ulonglong)uVar11 >> 0x20);
      iVar5 = 0x30;
      uVar9 = false;
      if (iVar6 == 0) {
ArmyRuntime_InitializePoolAndGraphicsCf_LoadCurrentFactionGraphicsPackageAndBindings:
        *(int *)pwVar8 = iVar5;
        WidePath_SetExtensionCode(0x786667,path);
        armyGraphicsAsset = Package_LoadEntry(path_00);
        if ((bool)uVar9) {
          return (dword)armyGraphicsAsset;
        }
        arg0 = (GraphicsTextureSourceAsset *)
               ArmyGraphics_CopyFrontendPlayerPaletteAndTexture
                         (iVar6,(ArmyGraphicsAssetAddress32)armyGraphicsAsset);
        pGVar3 = (*g_GraphicsCreateTextureSet)(arg0);
        if ((bool)uVar9) {
          LOCK();
          UNLOCK();
          Resource_Release(arg0);
          return (dword)pGVar3;
        }
        uVar9 = (undefined1 *)0xfffffffb < &stack0xffffffe4;
        uVar11 = MoviePlayback_AdvanceScheduledFrameAndTick();
        g_ArmyGraphicsBindings[iVar6].textureSet = (void *)uVar11;
        WidePath_SetExtensionCode(0x6c6170,(word *)((ulonglong)uVar11 >> 0x20));
        pGVar4 = (*g_GraphicsPaletteAssetLoadPackage)(arg0_00,arg2,(word *)arg2);
        if ((bool)uVar9) {
          return (dword)pGVar4;
        }
        g_ArmyGraphicsBindings[iVar6].paletteAsset = pGVar4;
      }
      else {
        uVar11 = MoviePlayback_AdvanceScheduledFrameAndTick();
        path = (word *)((ulonglong)uVar11 >> 0x20);
        if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[iVar6] != 0) {
          uVar2 = *(uint *)(g_GameFactionRuntimeImage.records[0].reserved78_87 + (int)uVar11 + -0x40
                           );
          g_MoviePlaybackScheduleCounter = g_MoviePlaybackScheduleCounter + -1;
          if (uVar2 < 10) {
            uVar9 = 0xffffffcf < uVar2;
            iVar5 = uVar2 + 0x30;
          }
          else {
            uVar9 = 0xffffffc8 < uVar2;
            iVar5 = uVar2 + 0x37;
          }
          goto ArmyRuntime_InitializePoolAndGraphicsCf_LoadCurrentFactionGraphicsPackageAndBindings;
        }
      }
      iVar6 = iVar6 + 1;
      uVar11 = MoviePlayback_AdvanceScheduledFrameAndTick();
      armySlot1 = (ArmyRuntimeSlot *)uVar11;
    } while (extraout_ECX != 1);
    pwVar8[0] = 0;
    pwVar8[1] = 0;
    ppAVar7 = g_ArmyAssetRecordRegistry;
    iVar5 = 0x300;
    do {
      armyDefinition1 = *ppAVar7;
      if ((armyDefinition1 != (ArmyAssetRecordPrefix *)0x0) &&
         (bVar10 = false, (armyDefinition1[1].selectionDetailTemplateVariantIndex & 0xfe) != 0)) {
        uVar11 = ArmyRuntime_RenderPreviewTextureCf
                           (iVar5,armyDefinition1,g_InGamePanelTextureSubresource34Height,
                            g_InGamePanelTextureSubresource34Width,
                            *(FactionRuntimeIndex *)((int)ownerContext + 0x50),
                            armyDefinition1->registryId,ownerContext);
        iVar6 = (int)((ulonglong)uVar11 >> 0x20);
        armySlot1 = (ArmyRuntimeSlot *)uVar11;
        iVar5 = extraout_ECX_00;
        if (!bVar10) {
          *(ArmyRuntimeSlot **)(iVar6 + 0x1c) = armySlot1;
          previewHeight =
               (GraphicsPixelDimension)
               ((ulonglong)(longlong)g_InGamePanelTextureSubresource02Width / 3);
          uVar11 = ArmyRuntime_RenderPreviewTextureCf
                             (extraout_ECX_00,iVar6,previewHeight,previewHeight,
                              *(FactionRuntimeIndex *)((int)ownerContext + 0x50),
                              *(PckArmyAssetIdCatalog *)(iVar6 + 8),ownerContext);
          armySlot1 = (ArmyRuntimeSlot *)uVar11;
          iVar5 = extraout_ECX_01;
          if (!bVar10) {
            *(ArmyRuntimeSlot **)((int)((ulonglong)uVar11 >> 0x20) + 0x18) = armySlot1;
          }
        }
      }
      ppAVar7 = ppAVar7 + 1;
      iVar5 = iVar5 + -1;
    } while (iVar5 != 0);
  }
  return (dword)armySlot1;
}

/* Address: 0x00528330.
   Ownership: gameplay/army/runtime.
   Purpose: Binary entry is anchored by g_CodePointerTable_0051FC98[12]@0051FC98. Runtime-update partition slots
   0-23 receive (worldRuntime, armyRuntime).
   Local calls: ArmyRuntime_UpdateTimedShotAndEffectEmitters.
   Cross-module calls: ModelRuntimePool_DestroyHierarchyAndDetach [world/model/runtime],
   ModelNodeRuntime_RebuildTransformsFromRoot [world/model/hierarchy].
*/
void ArmyRuntimeClass_UpdateEffectsAndDestroyModelHierarchy
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ModelRuntimeNode *modelNodeRuntime;
  int verticalStepQ12;
  int modelHeightQ12;
  GraphicsWorldCoordinateQ12 *worldTranslationZQ12Field;
  ModelResourceHitTestAndRenderView210 *modelResource;
  int localBoundsZ1Q12;
  dword remainingClassDistanceQ12;
  
  ArmyRuntime_UpdateTimedShotAndEffectEmitters(worldRuntime,armyRuntime);
  modelNodeRuntime = armyRuntime->modelNodeRuntime;
  modelResource = (modelNodeRuntime->modelPayload).modelResource;
  verticalStepQ12 =
       *(int *)((int)armyRuntime->definitionOrAsset + 0xc) * g_InGameSimulationStepTicks;
  worldTranslationZQ12Field = &(modelNodeRuntime->worldTransform).translation.z;
  *worldTranslationZQ12Field = *worldTranslationZQ12Field + verticalStepQ12;
  localBoundsZ1Q12 = modelResource->localBoundsZ1Q12;
  armyRuntime->classState60 = armyRuntime->classState60 - verticalStepQ12;
  modelHeightQ12 = localBoundsZ1Q12 - modelResource->localBoundsZ0Q12;
  remainingClassDistanceQ12 = armyRuntime->classState60;
  modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
  if (modelHeightQ12 < (int)remainingClassDistanceQ12) {
    ModelRuntimePool_DestroyHierarchyAndDetach(worldRuntime,(ModelRuntimeSlot *)armyRuntime);
  }
  else {
    ModelNodeRuntime_RebuildTransformsFromRoot(modelHeightQ12,modelResource,modelNodeRuntime);
  }
  return;
}

/* Address: 0x00531130.
   Ownership: gameplay/army/runtime.
   Purpose: Companion army-node callback that clears transient node flags, rebuilds the terrain occupancy
   contribution, and refreshes the node visual/runtime state.
   Local calls: ArmyRuntime_InitializeTerrainOccupancyFlags.
   Cross-module calls: UiModelControl_RefreshStateTint [ui/controls/misc].
*/
void ArmyRuntimeNode_RebuildTerrainOccupancyAndVisualStateCallback
               (WorldRuntimeContext *armyContext,WorldRuntimeNode *node)

{
  if (node[2].common.nextNode == (WorldRuntimeNode *)0x0) {
    node->runtimeFlags = node->runtimeFlags & 0xfffffff3;
    ArmyRuntime_InitializeTerrainOccupancyFlags
              (armyContext,*(ArmyRuntimeSlot **)((int)node->runtimePayload + 8));
    UiModelControl_RefreshStateTint((ModelRuntimeNode *)node);
  }
  return;
}

/* Address: 0x0051C3B0.
   Ownership: gameplay/army/runtime.
   Purpose: Typed parameters: p2 actionVector0→Q12, p3 actionVector2→Q12, p4 actionVector1→Q12. Calling convention,
   complete VariableStorage serialization, function bytes, control flow, globals, locals, and executable data
   remain unchanged.
*/
void ArmyRuntime_SetNonzeroActionVector
               (Q12 actionVector0,Q12 actionVector2,Q12 actionVector1,ArmyRuntimeSlot *armyRuntime)

{
  if ((actionVector1 != 0) && (actionVector2 != 0)) {
    armyRuntime->actionVector1Q12 = actionVector1;
    armyRuntime->actionVector0Q12 = actionVector0;
    armyRuntime->actionVector2Q12 = actionVector2;
  }
  return;
}

/* Address: 0x0051C540.
   Ownership: gameplay/army/runtime.
   Purpose: Resolves the current command target according to the movement-mode flags, stores the target pointer and
   state code, and clears the target fields when resolution fails. Two stack arguments are authoritative from RET
   0x08; EAX/ECX are internal or preserved state, not a semantic return.
   Cross-module calls: ArmyRuntime_ResetMovementStatePreserveQueuedTarget [gameplay/army/movement],
   ArmyRuntime_ResetMovementStateFromCurrentPosition [gameplay/army/movement].
*/
void ArmyRuntime_ResolveCommandTarget
               (GameEntityRuntime *resolvedTarget,ArmyRuntimeSlot *armyRuntime)

{
  ArmyCommandGeneration extraout_ECX;
  ArmyCommandGeneration extraout_ECX_00;
  ArmyCommandGeneration resolvedCommandGeneration;
  
  resolvedCommandGeneration = g_ArmyCommandGenerationStandard;
  if ((armyRuntime->movementStateFlags & 0x20) != 0) {
    if ((armyRuntime->commandModeFlags & 8) == 0) {
      resolvedTarget =
           (GameEntityRuntime *)
           ArmyRuntime_ResetMovementStatePreserveQueuedTarget((ArmyMovementRuntime *)armyRuntime);
      resolvedCommandGeneration = extraout_ECX_00;
    }
    else {
      resolvedTarget =
           (GameEntityRuntime *)
           ArmyRuntime_ResetMovementStateFromCurrentPosition((ArmyMovementRuntime *)armyRuntime);
      resolvedCommandGeneration = extraout_ECX;
    }
  }
  if (resolvedTarget == (GameEntityRuntime *)0x0) {
    armyRuntime->commandModeFlags = 0;
    armyRuntime->commandGeneration = 0;
  }
  else {
    armyRuntime->commandModeFlags = 1;
    armyRuntime->commandGeneration = resolvedCommandGeneration;
  }
  armyRuntime->commandTargetArmyRuntime = (ArmyRuntimeSlot *)resolvedTarget;
  return;
}

/* Address: 0x0051C620.
   Ownership: gameplay/army/runtime.
   Purpose: The three Q12 command coordinates and command-generation state map to
   GameEntityRuntimeCommon.commandTarget.
   Cross-module calls: ArmyRuntime_ResetMovementStatePreserveQueuedTarget [gameplay/army/movement],
   ArmyRuntime_ResetMovementStateFromCurrentPosition [gameplay/army/movement].
*/
void ArmyRuntime_ApplyTargetPositionCommand
               (Q12 coordinateA,Q12 coordinateB,Q12 coordinateC,ArmyRuntimeSlot *armyRuntime)

{
  Q12 extraout_ECX;
  Q12 extraout_ECX_00;
  Q12 extraout_EDX;
  Q12 extraout_EDX_00;
  ArmyCommandGeneration commandGeneration;
  
  if ((armyRuntime->movementStateFlags & 0x20) != 0) {
    if ((armyRuntime->commandModeFlags & 8) == 0) {
      coordinateC = ArmyRuntime_ResetMovementStatePreserveQueuedTarget
                              ((ArmyMovementRuntime *)armyRuntime);
      coordinateB = extraout_ECX_00;
      coordinateA = extraout_EDX_00;
    }
    else {
      coordinateC = ArmyRuntime_ResetMovementStateFromCurrentPosition
                              ((ArmyMovementRuntime *)armyRuntime);
      coordinateB = extraout_ECX;
      coordinateA = extraout_EDX;
    }
  }
  armyRuntime->commandModeFlags = 2;
  armyRuntime->commandCoordinate0Q12 = coordinateC;
  armyRuntime->commandCoordinate1Q12 = coordinateB;
  armyRuntime->commandCoordinate2Q12 = coordinateA;
  commandGeneration = g_ArmyCommandGenerationStandard;
  armyRuntime->commandTargetArmyRuntime = (ArmyRuntimeSlot *)0x0;
  armyRuntime->movementStateFlags = armyRuntime->movementStateFlags & 0xffffffdf;
  armyRuntime->commandGeneration = commandGeneration;
  return;
}

/* Address: 0x0051C720.
   Ownership: gameplay/army/runtime.
   Purpose: Resolves the active shot aim point from the current target state, applies the verified range and lead
   adjustment when required, and reports invalid target state through carry. Five stack arguments are authoritative
   from RET 0x14. EDX:EAX carries the nominal X/Z result pair, ECX carries Y, and CF reports validity. Role:
   Resolves the world aim point from explicit coordinates or a target entity, including lead adjustment. Inputs:
   Shooter position, command target state, ShotDefinition and target model hierarchy.
   Cross-module calls: FixedMath_Length3 [core/math/fixed], ShotDefinition_GetModeRangeLimitEbx
   [assets/shot/catalog], ShotDefinition_ComputeMode3LeadAdjustment [assets/shot/catalog], FixedMath_Atan2Angle16
   [core/math/fixed], FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed].
*/
ShotAimXZRegisterPairQ12
ArmyRuntime_ResolveShotAimPointCf
          (Q12 sourceWorldZQ12,Q12 sourceWorldYQ12,Q12 sourceWorldXQ12,
          ShotDefinition *shotDefinition,GameEntityRuntime *targetState)

{
  int *piVar1;
  longlong lVar2;
  longlong lVar3;
  uint in_EAX;
  int iVar4;
  int iVar5;
  dword dVar6;
  int iVar7;
  int iVar8;
  int extraout_ECX;
  int extraout_ECX_00;
  int *in_EDX;
  int iVar9;
  ModelRuntimeNode *modelNode1;
  FixedDirectionXZEdxEax8 FVar10;
  GameEntityRuntime *entityRuntime1;
  
  if (((targetState->common).commandTarget.targetFlags & 1) == 0) {
    if (((targetState->common).commandTarget.targetFlags & 2) != 0) {
      iVar8 = (targetState->common).commandTarget.targetWorldXQ12;
      iVar9 = (targetState->common).commandTarget.targetWorldZQ12;
      goto ArmyRuntime_ResolveShotAimPointCf_ReturnResolvedWorldXZWithCarryClear;
    }
  }
  else {
    entityRuntime1 = (targetState->common).commandTarget.targetEntity;
    in_EAX = 2;
    if (entityRuntime1 != (GameEntityRuntime *)0x0) {
      in_EAX = 2 << ((char)(targetState->common).ownership.ownerIndex * '\x02' & 0x1fU);
      in_EDX = (entityRuntime1->common).ownership.definitionOrClassRecord;
      modelNode1 = (entityRuntime1->common).ownership.modelNode;
      if ((*(uint *)((entityRuntime1->common).damageState.reserved0C_2B + 0x10) & in_EAX) != 0) {
        if (*(int *)(*in_EDX + 0x4c) == 0x15) {
          modelNode1 = modelNode1->childNodes[0];
        }
        iVar8 = (modelNode1->worldTransform).translation.x;
        piVar1 = (entityRuntime1->common).ownership.definitionOrClassRecord;
        iVar7 = *piVar1;
        iVar9 = (modelNode1->worldTransform).translation.z + *(int *)(iVar7 + 0x50);
        iVar5 = piVar1[2];
        if ((*(int *)(iVar7 + 0x18) != 0) && ((*(uint *)(iVar5 + 0x18) & 4) == 0)) {
          FixedMath_Length3(iVar9 - sourceWorldZQ12,
                            (modelNode1->worldTransform).translation.y - sourceWorldYQ12,
                            iVar8 - sourceWorldXQ12);
          iVar4 = ShotDefinition_GetModeRangeLimitEbx(shotDefinition);
          iVar5 = (int)(((longlong)iVar4 * (longlong)*(int *)(iVar7 + 0xc)) / (longlong)iVar5);
          dVar6 = ShotDefinition_ComputeMode3LeadAdjustment(shotDefinition);
          iVar5 = iVar5 + dVar6 * *(int *)(iVar7 + 0xc);
          dVar6 = FixedMath_Atan2Angle16
                            (*(int *)(extraout_ECX + 0x5c) -
                             *(int *)(*(int *)(extraout_ECX + 4) + 0x98),
                             *(int *)(extraout_ECX + 0x58) -
                             *(int *)(*(int *)(extraout_ECX + 4) + 0x94));
          FVar10 = FixedMath_DirectionFromAnglesScaledRegs(0,dVar6,iVar5);
          iVar5 = (int)FVar10;
          entityRuntime1 = (targetState->common).commandTarget.targetEntity;
          iVar7 = *(int *)((entityRuntime1->common).damageState.reserved0C_2B + 0x18);
          if (iVar7 == (entityRuntime1->common).pathCoordinate0Q12) {
            iVar7 = iVar7 - (modelNode1->worldTransform).translation.x;
            lVar3 = ((longlong)extraout_ECX_00 * (longlong)extraout_ECX_00 +
                    (longlong)iVar5 * (longlong)iVar5) - (longlong)iVar7 * (longlong)iVar7;
            if (((-1 < lVar3) &&
                (iVar7 = *(int *)((entityRuntime1->common).damageState.reserved0C_2B + 0x1c),
                iVar7 == (entityRuntime1->common).pathCoordinate1Q12)) &&
               (iVar7 = iVar7 - (modelNode1->worldTransform).translation.y,
               lVar2 = (longlong)iVar7 * (longlong)iVar7,
               -1 < (int)(((int)((ulonglong)lVar3 >> 0x20) - (int)((ulonglong)lVar2 >> 0x20)) -
                         (uint)((uint)lVar3 < (uint)lVar2)))) {
              iVar8 = (entityRuntime1->common).pathCoordinate0Q12;
              iVar9 = (((entityRuntime1->common).ownership.modelNode)->worldTransform).translation.z
                      + *(int *)(*(int *)(entityRuntime1->common).ownership.definitionOrClassRecord
                                + 0x50);
              goto ArmyRuntime_ResolveShotAimPointCf_ReturnResolvedWorldXZWithCarryClear;
            }
          }
          iVar9 = (int)(FVar10 >> 0x20) + iVar9;
          iVar8 = iVar5 + iVar8;
        }
ArmyRuntime_ResolveShotAimPointCf_ReturnResolvedWorldXZWithCarryClear:
        return CONCAT44(iVar9,iVar8);
      }
      (targetState->common).commandTarget.targetEntity = (GameEntityRuntime *)0x0;
      (targetState->common).commandTarget.targetFlags = 0;
    }
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x0051D170.
   Ownership: gameplay/army/runtime.
   Purpose: Army-node owner-list callback that derives the faction visibility mask, accumulates projected terrain
   occlusion around the node world position, and marks occupancy bit 2 when the definition flags require it.
   Maintenance table phase occupancyRebuild, object kind army. The 4x3 table bytes, target body, calling
   convention, and RET 0x08 contract remain unchanged.
   Cross-module calls: TerrainProjectedOcclusion_AccumulateMaskAroundWorldPoint [world/terrain/projection],
   TerrainOccupancyBit2_MarkAroundWorldPoint [world/terrain/occupancy].
*/
void ArmyRuntimeNode_AccumulateTerrainOcclusionAndOccupancyCallback
               (WorldRuntimeContext *worldRuntime,WorldRuntimeNode *node)

{
  int iVar1;
  ulonglong uVar2;
  void *worldXQ12;
  ModelMeshGroupMask worldYQ12;
  FieldGridOccupancyByteIndex occupancyByteOffset;
  FieldGridAsset *fieldGrid;
  
  if (node[2].common.nextNode == (WorldRuntimeNode *)0x0) {
    worldYQ12 = node[1].classPayload.model.meshGroupMask;
    iVar1 = *(int *)((int)node->runtimePayload + 8);
    worldXQ12 = node[1].runtimePayload;
    if (*(int *)(iVar1 + 0xc) != 0) {
      occupancyByteOffset = *(FieldGridOccupancyByteIndex *)(iVar1 + 0xc);
      fieldGrid = worldRuntime->fieldGrid;
      uVar2 = TerrainProjectedOcclusion_AccumulateMaskAroundWorldPoint
                        (worldRuntime->activeFactionRuntimeIndex << 2,8,
                         *(FieldGridRadiusUnits *)(iVar1 + 0x44),
                         node[1].runtimeFlags + *(int *)(iVar1 + 0x48),(Q12)worldXQ12,worldYQ12,
                         worldRuntime->fieldGrid);
      if ((uVar2 & 8) != 0) {
        TerrainOccupancyBit2_MarkAroundWorldPoint
                  (*(FieldGridRadiusUnits *)(iVar1 + 0x90),(Q12)worldXQ12,worldYQ12,
                   occupancyByteOffset,fieldGrid);
      }
    }
  }
  return;
}

/* Address: 0x0051D310.
   Ownership: gameplay/army/runtime.
   Purpose: Tests the army-runtime dword at offset 0x100 and returns the result through carry. Carry is set exactly
   when the field is zero.
*/
undefined4 ArmyRuntime_TestStateField100ZeroCf(void)

{
  undefined4 in_EAX;
  
  return in_EAX;
}

/* Address: 0x0051D330.
   Ownership: gameplay/army/runtime.
   Purpose: Tests whether the signed state field at army-runtime offset 0x100 is nonnegative; CF carries the
   result.
*/
void ArmyRuntime_TestStateField100NonnegativeCf(ArmyRuntimeSlot *armyRuntime)

{
  return;
}

/* Address: 0x0051D350.
   Ownership: gameplay/army/runtime.
   Purpose: Loads the pointed runtime node, selects one callback through the exact 0x0051FED8 type-index table, and
   forwards the node plus caller argument. Placement-validation partition slots 24-47 receive (worldRuntime,
   armyRuntime), with CF carrying acceptance.
*/
void ArmyRuntimeNode_DispatchTypedCallback
               (ArmyRuntimeSlot **modelRuntimeHolder,WorldRuntimeContext *worldRuntime)

{
  (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.placementValidationCf
    [*(int *)((int)(*modelRuntimeHolder)->definitionOrAsset + 0x4c)])
            (worldRuntime,*modelRuntimeHolder);
  return;
}

/* Address: 0x0051D4D0.
   Ownership: gameplay/army/runtime.
   Purpose: Dispatches a two-argument army runtime command through the verified runtime-class handler table indexed
   by the root model definition class. Dispatch wrapper for the typed twenty-four-entry class-command table.
*/
void ArmyRuntime_DispatchClassCommand
               (ArmyRuntimeSlot **armyRuntimeHolder,WorldRuntimeContext *worldRuntime)

{
  (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand
    [*(int *)((int)(*armyRuntimeHolder)->definitionOrAsset + 0x4c)])
            (worldRuntime,*armyRuntimeHolder);
  return;
}

/* Address: 0x0051D8C0.
   Ownership: gameplay/army/runtime.
   Purpose: Frees the army runtime pool, releases all eight texture/palette binding pairs, frees the two verified
   owned allocations for every populated army registry entry, and clears the registry pointers.
*/
undefined8 __cdecl ArmyRuntime_ShutdownPoolAndGraphics(void)

{
  undefined4 in_ECX;
  int iVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_EDX;
  undefined4 unaff_EDI;
  ArmyGraphicsBinding *graphicsBindingCursor;
  ArmyAssetRecordPrefix **assetRegistryCursor;
  
  (*g_MemoryApi.free)(g_ArmyRuntimeSlots);
  g_ArmyRuntimeSlots = (ArmyRuntimeSlot *)0x0;
  graphicsBindingCursor = g_ArmyGraphicsBindings;
  iVar1 = 8;
  do {
    if (graphicsBindingCursor->textureSet != (void *)0x0) {
      (*(code *)g_GraphicsTextureSetReleasePackageCf)(graphicsBindingCursor->textureSet);
      graphicsBindingCursor->textureSet = (void *)0x0;
      iVar1 = extraout_ECX;
    }
    if (graphicsBindingCursor->paletteAsset != (GraphicsPaletteAsset *)0x0) {
      (*g_GraphicsPaletteAssetLifecycleCallbacks3.releasePackage)
                (graphicsBindingCursor->paletteAsset);
      graphicsBindingCursor->paletteAsset = (GraphicsPaletteAsset *)0x0;
      iVar1 = extraout_ECX_00;
    }
    graphicsBindingCursor = graphicsBindingCursor + 1;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  assetRegistryCursor = g_ArmyAssetRecordRegistry;
  iVar1 = 0x300;
  do {
    if (*assetRegistryCursor != (ArmyAssetRecordPrefix *)0x0) {
      (*g_MemoryApi.free)((void *)(*assetRegistryCursor)[1].rootNodeOffsetOrPointer);
      (*g_MemoryApi.free)(*(void **)(extraout_EDX + 0x18));
      *assetRegistryCursor = (ArmyAssetRecordPrefix *)0x0;
      iVar1 = extraout_ECX_01;
    }
    assetRegistryCursor = assetRegistryCursor + 1;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  return CONCAT44(unaff_EDI,in_ECX);
}

/* Address: 0x0051D960.
   Ownership: gameplay/army/runtime.
   Purpose: Traverses 1024 exact 0x120-byte slots. Populated slots convert four verified runtime pointers to
   serialized offsets; empty slots are zeroed. Returns the pool base in EAX and exact byte size 0x48000 in EDX.
*/
RuntimeImagePointerByteSizeEdxEax8 __cdecl ArmyRuntimePool_ConvertPointersToOffsetsForSaveRegs(void)

{
  dword dVar1;
  void *pvVar2;
  int iVar3;
  int iVar4;
  ArmyRuntimeSlot *armySlot1;
  ArmyRuntimeSlot *armySlot2;
  
  iVar4 = 0x400;
  armySlot2 = g_ArmyRuntimeSlots;
  do {
    while( true ) {
      armySlot1 = armySlot2->commandTargetArmyRuntime;
      if (armySlot2->modelNodeRuntime != (ModelRuntimeNode *)0x0) break;
      for (iVar3 = 0x48; iVar3 != 0; iVar3 = iVar3 + -1) {
        armySlot2->definitionOrAsset = (void *)0x0;
        armySlot2 = (ArmyRuntimeSlot *)&armySlot2->modelNodeRuntime;
      }
      iVar4 = iVar4 + -1;
      if (iVar4 == 0) {
        return CONCAT44(0x48000,g_ArmyRuntimeSlots);
      }
    }
    pvVar2 = (void *)((int)armySlot2->definitionOrAsset - g_ModelRuntimeRebaseDelta);
    if (armySlot1 != (ArmyRuntimeSlot *)0x0) {
      armySlot1 = (ArmyRuntimeSlot *)((int)armySlot1 - (int)g_ArmyRuntimeRebaseBaseMinusOne);
    }
    armySlot2->modelNodeRuntime =
         (ModelRuntimeNode *)
         ((int)armySlot2->modelNodeRuntime - (int)g_RuntimeObjectRebaseBaseMinusOne);
    dVar1 = armySlot2->runtimeState98;
    armySlot2->definitionOrAsset = pvVar2;
    if (dVar1 != 0) {
      dVar1 = dVar1 - (int)g_ArmyRuntimeRebaseBaseMinusOne;
    }
    armySlot2->commandTargetArmyRuntime = armySlot1;
    armySlot2->runtimeState98 = dVar1;
    armySlot2 = armySlot2 + 1;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  return CONCAT44(0x48000,g_ArmyRuntimeSlots);
}

/* Address: 0x0051D9F0.
   Ownership: gameplay/army/runtime.
   Purpose: Traverses 1024 exact 0x120-byte slots and converts the four verified serialized offsets in each
   populated slot back to runtime pointers using the model, world, and army pool relocation bases.
*/
void __cdecl ArmyRuntimePool_RebaseAfterLoad(void)

{
  dword savedRuntimeState98Offset;
  void *rebasedDefinition;
  int runtimeSlotsRemaining;
  ArmyRuntimeSlot *rebasedCommandTarget;
  ArmyRuntimeSlot *runtimeSlotCursor;
  
  runtimeSlotsRemaining = 0x400;
  runtimeSlotCursor = g_ArmyRuntimeSlots;
  do {
    if (runtimeSlotCursor->modelNodeRuntime != (ModelRuntimeNode *)0x0) {
      rebasedDefinition =
           (void *)((int)runtimeSlotCursor->definitionOrAsset + g_ModelRuntimeRebaseDelta);
      rebasedCommandTarget = (ArmyRuntimeSlot *)0x0;
      if (runtimeSlotCursor->commandTargetArmyRuntime != (ArmyRuntimeSlot *)0x0) {
        rebasedCommandTarget =
             (ArmyRuntimeSlot *)
             ((int)&runtimeSlotCursor->commandTargetArmyRuntime->definitionOrAsset +
             (int)g_ArmyRuntimeRebaseBaseMinusOne);
      }
      runtimeSlotCursor->modelNodeRuntime =
           (ModelRuntimeNode *)
           (g_RuntimeObjectRebaseBaseMinusOne +
           (int)(&runtimeSlotCursor->modelNodeRuntime->modelPayload + -1) + 0x30);
      savedRuntimeState98Offset = runtimeSlotCursor->runtimeState98;
      runtimeSlotCursor->definitionOrAsset = rebasedDefinition;
      if (savedRuntimeState98Offset != 0) {
        savedRuntimeState98Offset = savedRuntimeState98Offset + (int)g_ArmyRuntimeRebaseBaseMinusOne
        ;
      }
      runtimeSlotCursor->commandTargetArmyRuntime = rebasedCommandTarget;
      runtimeSlotCursor->runtimeState98 = savedRuntimeState98Offset;
    }
    runtimeSlotCursor = runtimeSlotCursor + 1;
    runtimeSlotsRemaining = runtimeSlotsRemaining + -1;
  } while (runtimeSlotsRemaining != 0);
  return;
}

/* Address: 0x00520CF0.
   Ownership: gameplay/army/runtime.
   Purpose: Refreshes the two verified positioned-sound channels for the first ground-movement runtime class when
   their terrain projection cells remain active. Class method-D partition slots 24-47 receive (worldRuntime,
   armyRuntime).
   Cross-module calls: TerrainGrid_TestProjectedCellMaskBits01Cf [world/terrain/grid],
   SpatialSound_UpdateDesiredPositionedGains [audio/spatial/runtime].
*/
void ArmyRuntimeClass_UpdatePositionedSoundsVariantA
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  uint uVar1;
  void *pvVar2;
  bool bVar3;
  undefined8 uVar4;
  ModelRuntimeNode *modelNode1;
  
  pvVar2 = armyRuntime->definitionOrAsset;
  if ((armyRuntime->movementControl).turnVelocityAngle16 == 0) {
    pvVar2 = armyRuntime->definitionOrAsset;
    if ((armyRuntime->movementControl).movementAdvancePerTickQ12 == 0) {
      return;
    }
  }
  else {
    uVar1 = *(uint *)((int)pvVar2 + 0xd8);
    if ((((uVar1 != 0) && (uVar1 < worldRuntime->dwordArrayCount)) &&
        (worldRuntime->dwordArray != (dword *)0x0)) &&
       (modelNode1 = armyRuntime->modelNodeRuntime, worldRuntime->dwordArray[uVar1] != 0)) {
      bVar3 = (ModelRuntimeNode *)0xffffff6b < modelNode1;
      uVar4 = TerrainGrid_TestProjectedCellMaskBits01Cf
                        ((modelNode1->worldTransform).translation.y,
                         (modelNode1->worldTransform).translation.x,worldRuntime);
      if (bVar3)
      goto 
      ArmyRuntimeClass_UpdatePositionedSoundsVariantA_ContinueWithSecondaryConfiguredPositionedSound
      ;
      SpatialSound_UpdateDesiredPositionedGains
                (*(SpatialSoundMaximumDistanceQ12 *)((int)pvVar2 + 0x7c),
                 *(SpatialSoundGainQ15 *)((int)pvVar2 + 0x78),
                 (GraphicsFixedVec3 *)((ulonglong)uVar4 >> 0x20),(SpatialSoundSlot *)uVar4);
    }
    pvVar2 = armyRuntime->definitionOrAsset;
  }
ArmyRuntimeClass_UpdatePositionedSoundsVariantA_ContinueWithSecondaryConfiguredPositionedSound:
  uVar1 = *(uint *)((int)pvVar2 + 0xd0);
  if (((uVar1 != 0) && (uVar1 < worldRuntime->dwordArrayCount)) &&
     ((worldRuntime->dwordArray != (dword *)0x0 &&
      (modelNode1 = armyRuntime->modelNodeRuntime, worldRuntime->dwordArray[uVar1] != 0)))) {
    bVar3 = (ModelRuntimeNode *)0xffffff6b < modelNode1;
    uVar4 = TerrainGrid_TestProjectedCellMaskBits01Cf
                      ((modelNode1->worldTransform).translation.y,
                       (modelNode1->worldTransform).translation.x,worldRuntime);
    if (!bVar3) {
      SpatialSound_UpdateDesiredPositionedGains
                (*(SpatialSoundMaximumDistanceQ12 *)((int)pvVar2 + 0x7c),
                 *(SpatialSoundGainQ15 *)((int)pvVar2 + 0x78),
                 (GraphicsFixedVec3 *)((ulonglong)uVar4 >> 0x20),(SpatialSoundSlot *)uVar4);
    }
  }
  return;
}

/* Address: 0x00522B70.
   Ownership: gameplay/army/runtime.
   Purpose: Binary entry is anchored by g_CodePointerTable_0051FC98[27]@0051FC98. Class method-D partition slots
   24-47 receive (worldRuntime, armyRuntime).
*/
void ArmyRuntimeClass_NoOpUpdate(WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  return;
}

/* Address: 0x00523E70.
   Ownership: gameplay/army/runtime.
   Purpose: Iterator callback that filters candidate world nodes and stores an accepted projectile target into the
   active army runtime slot.
*/
void ArmyRuntimeClass_SelectProjectileTargetNodeCf
               (ArmyRuntimeSlot *armyRuntime,WorldRuntimeNode *candidateNode)

{
  longlong lVar1;
  longlong lVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  int unaff_EBP;
  int unaff_ESI;
  int *piVar6;
  
  if (candidateNode[2].common.nextNode == (WorldRuntimeNode *)0x0) {
    iVar4 = *(int *)(*(int *)((int)armyRuntime->definitionOrAsset + 0x2c) + 0x27c);
    iVar3 = candidateNode[1].classPayload.model.meshGroupMask -
            (armyRuntime->modelNodeRuntime->worldTransform).translation.x;
    lVar2 = (longlong)iVar4 * (longlong)iVar4 - (longlong)iVar3 * (longlong)iVar3;
    if ((((-1 < lVar2) &&
         (iVar4 = (int)candidateNode[1].runtimePayload -
                  (armyRuntime->modelNodeRuntime->worldTransform).translation.y,
         lVar1 = (longlong)iVar4 * (longlong)iVar4,
         -1 < (int)(((int)((ulonglong)lVar2 >> 0x20) - (int)((ulonglong)lVar1 >> 0x20)) -
                   (uint)((uint)lVar2 < (uint)lVar1)))) &&
        (iVar4 = *(int *)(*(int *)((int)candidateNode->runtimePayload + 8) + 0xc),
        iVar4 != (armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex)) && (iVar4 != 0))
    {
      iVar4 = 0;
      piVar6 = candidateNode->runtimePayload;
      piVar5 = (int *)0x0;
      do {
        if (*(int *)(*piVar6 + 0x4c) == 10) {
          piVar5 = piVar6;
        }
        iVar4 = iVar4 + 1;
        iVar3 = unaff_ESI;
        do {
          while (unaff_EBP == 0) {
            iVar4 = iVar4 + -1;
            if (iVar4 == 0) {
              if (piVar5 == (int *)0x0) {
                return;
              }
              if ((piVar5[0x3b] & 8U) != 0) {
                return;
              }
              armyRuntime->classState60 = (dword)piVar5;
              return;
            }
          }
          unaff_EBP = unaff_EBP + -1;
          unaff_ESI = iVar3 + 0x20;
          piVar6 = *(int **)(iVar3 + 0x140);
          iVar3 = unaff_ESI;
        } while (piVar6 == (int *)0x0);
      } while( true );
    }
  }
  else if ((candidateNode[2].common.nextNode == (WorldRuntimeNode *)0x1) &&
          (*(int *)((int)armyRuntime->definitionOrAsset + 0x2c) ==
           *(int *)candidateNode->runtimePayload)) {
    armyRuntime->ownerValue64 = (dword)candidateNode->runtimePayload;
  }
  return;
}

/* Address: 0x00523FC0.
   Ownership: gameplay/army/runtime.
   Purpose: Binary entry is anchored by g_CodePointerTable_0051FC98[20]@0051FC98. Runtime-update partition slots
   0-23 receive (worldRuntime, armyRuntime). Role: Class callback that advances target/projectile timers and
   associated effects. Inputs: World context, army target state, timers and definitions. Outputs: Updated target
   state and scheduled projectile/effect creation.
   Cross-module calls: WorldRuntime_ForEachNodeInOwnerListD8 [world/runtime/core],
   ModelRuntime_EmitProjectilesFromAttachmentPoints [world/model/runtime],
   ModelNodeRuntime_RebuildTransformsFromRoot [world/model/hierarchy], ArmyRuntime_EmitDamageThresholdEffect
   [gameplay/army/combat].
*/
void ArmyRuntimeClass_UpdateTimedTargetProjectilesAndEffects
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ModelMeshGroupMask *pMVar1;
  void *pvVar2;
  Q12 targetWorldXQ12;
  Q12 targetWorldYQ12;
  Q12 targetWorldZQ12;
  ModelRuntimeNode *modelNodeRuntime;
  int iVar3;
  ModelRuntimeNode *in_ECX;
  ModelRuntimeNode *extraout_ECX;
  ModelRuntimeNode *extraout_ECX_00;
  dword in_EDX;
  dword extraout_EDX;
  
  if ((armyRuntime->runtimeFlags & 9) == 0) {
    pvVar2 = armyRuntime->definitionOrAsset;
    iVar3 = armyRuntime->commandCoordinate1Q12 - g_InGameSimulationStepTicks;
    in_ECX = armyRuntime->modelNodeRuntime;
    armyRuntime->commandCoordinate1Q12 = iVar3;
    if (iVar3 < 1) {
      armyRuntime->commandCoordinate1Q12 = 0;
      pMVar1 = &(in_ECX->modelPayload).meshGroupMask;
      *pMVar1 = *pMVar1 | 1;
      armyRuntime->classState60 = 0;
      armyRuntime->ownerValue64 = 0;
      WorldRuntime_ForEachNodeInOwnerListD8
                (armyRuntime,ArmyRuntimeClass_SelectProjectileTargetNodeCf,worldRuntime);
      in_EDX = armyRuntime->classState60;
      in_ECX = extraout_ECX;
      if ((armyRuntime->ownerValue64 == 0) && (in_EDX != 0)) {
        iVar3 = *(int *)(in_EDX + 4);
        targetWorldXQ12 = *(Q12 *)(iVar3 + 0x94);
        targetWorldYQ12 = *(Q12 *)(iVar3 + 0x98);
        targetWorldZQ12 = *(Q12 *)(iVar3 + 0x9c);
        modelNodeRuntime = armyRuntime->modelNodeRuntime;
        armyRuntime->commandCoordinate1Q12 =
             armyRuntime->commandCoordinate1Q12 + *(int *)((int)pvVar2 + 0x30);
        modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
        pMVar1 = &(modelNodeRuntime->modelPayload).meshGroupMask;
        *pMVar1 = *pMVar1 & 0xfffffffe;
        ModelRuntime_EmitProjectilesFromAttachmentPoints
                  (armyRuntime->classState60,targetWorldZQ12,targetWorldYQ12,targetWorldXQ12,
                   *(ShotDefinition **)((int)pvVar2 + 0x2c),modelNodeRuntime,
                   *(MdlSerializedNodeHeader38 **)((int)pvVar2 + 100),worldRuntime);
        in_ECX = extraout_ECX_00;
        in_EDX = extraout_EDX;
      }
    }
  }
  ModelNodeRuntime_RebuildTransformsFromRoot(in_ECX,in_EDX,armyRuntime->modelNodeRuntime);
  armyRuntime->classState60 = 0;
  armyRuntime->ownerValue64 = 0;
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,armyRuntime);
  return;
}

/* Address: 0x00525960.
   Ownership: gameplay/army/runtime.
   Purpose: Refreshes the two verified positioned-sound channels for the second ground-movement runtime class when
   their terrain projection cells remain active. Class method-D partition slots 24-47 receive (worldRuntime,
   armyRuntime).
   Cross-module calls: TerrainGrid_TestProjectedCellMaskBits01Cf [world/terrain/grid],
   SpatialSound_UpdateDesiredPositionedGains [audio/spatial/runtime].
*/
void ArmyRuntimeClass_UpdatePositionedSoundsVariantB
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  uint uVar1;
  void *pvVar2;
  bool bVar3;
  undefined8 uVar4;
  ModelRuntimeNode *modelNode1;
  
  pvVar2 = armyRuntime->definitionOrAsset;
  if ((armyRuntime->movementControl).turnVelocityAngle16 == 0) {
    pvVar2 = armyRuntime->definitionOrAsset;
    if ((armyRuntime->movementControl).movementAdvancePerTickQ12 == 0) {
      return;
    }
  }
  else {
    uVar1 = *(uint *)((int)pvVar2 + 0xd8);
    if ((((uVar1 != 0) && (uVar1 < worldRuntime->dwordArrayCount)) &&
        (worldRuntime->dwordArray != (dword *)0x0)) &&
       (modelNode1 = armyRuntime->modelNodeRuntime, worldRuntime->dwordArray[uVar1] != 0)) {
      bVar3 = (ModelRuntimeNode *)0xffffff6b < modelNode1;
      uVar4 = TerrainGrid_TestProjectedCellMaskBits01Cf
                        ((modelNode1->worldTransform).translation.y,
                         (modelNode1->worldTransform).translation.x,worldRuntime);
      if (!bVar3) {
        SpatialSound_UpdateDesiredPositionedGains
                  (*(SpatialSoundMaximumDistanceQ12 *)((int)pvVar2 + 0x7c),
                   *(SpatialSoundGainQ15 *)((int)pvVar2 + 0x78),
                   (GraphicsFixedVec3 *)((ulonglong)uVar4 >> 0x20),(SpatialSoundSlot *)uVar4);
      }
    }
  }
  uVar1 = *(uint *)((int)pvVar2 + 0xd0);
  if (((uVar1 != 0) && (uVar1 < worldRuntime->dwordArrayCount)) &&
     ((worldRuntime->dwordArray != (dword *)0x0 &&
      (modelNode1 = armyRuntime->modelNodeRuntime, worldRuntime->dwordArray[uVar1] != 0)))) {
    bVar3 = (ModelRuntimeNode *)0xffffff6b < modelNode1;
    uVar4 = TerrainGrid_TestProjectedCellMaskBits01Cf
                      ((modelNode1->worldTransform).translation.y,
                       (modelNode1->worldTransform).translation.x,worldRuntime);
    if (!bVar3) {
      SpatialSound_UpdateDesiredPositionedGains
                (*(SpatialSoundMaximumDistanceQ12 *)((int)pvVar2 + 0x7c),
                 *(SpatialSoundGainQ15 *)((int)pvVar2 + 0x78),
                 (GraphicsFixedVec3 *)((ulonglong)uVar4 >> 0x20),(SpatialSoundSlot *)uVar4);
    }
  }
  return;
}

/* Address: 0x00526FE0.
   Ownership: gameplay/army/runtime.
   Purpose: Loads EBX from army-runtime offset 0x6C and ECX from the linked model-definition offset 0xC4. The
   selection-panel renderer consumes both register results when composing metric bars.
*/
void ArmyRuntime_QueryMetric6CAndDefinitionC4Regs(void)

{
  return;
}

/* Address: 0x00527150.
   Ownership: gameplay/army/runtime.
   Purpose: Scans thirteen attachment slots and ORs effect-variant bits 1, 2, and 4 into EBX.
*/
void ArmyRuntime_AccumulateAttachmentEffectVariantMaskRegs(ArmyRuntimeSlot *armyRuntime)

{
  int attachmentEffectSlotsRemaining;
  
  attachmentEffectSlotsRemaining = 0xd;
  do {
    attachmentEffectSlotsRemaining = attachmentEffectSlotsRemaining + -1;
  } while (attachmentEffectSlotsRemaining != 0);
  return;
}

/* Address: 0x00527FE0.
   Ownership: gameplay/army/runtime.
   Purpose: Refreshes the configured looping positioned sound while the runtime remains active and its projected
   terrain cell is available. Class method-D partition slots 24-47 receive (worldRuntime, armyRuntime).
   Cross-module calls: TerrainGrid_TestProjectedCellMaskBits01Cf [world/terrain/grid],
   SpatialSound_UpdateDesiredPositionedGains [audio/spatial/runtime].
*/
void ArmyRuntime_UpdateLoopingPositionedSound
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  uint uVar1;
  int extraout_ECX;
  bool bVar2;
  undefined8 uVar3;
  
  uVar1 = *(uint *)((int)armyRuntime->definitionOrAsset + 0x1ac);
  if (((((armyRuntime->runtimeFlags & 1) == 0) && (uVar1 != 0)) &&
      (uVar1 < worldRuntime->dwordArrayCount)) &&
     ((worldRuntime->dwordArray != (dword *)0x0 &&
      (bVar2 = false, worldRuntime->dwordArray[uVar1] != 0)))) {
    uVar3 = TerrainGrid_TestProjectedCellMaskBits01Cf
                      ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,
                       (armyRuntime->modelNodeRuntime->worldTransform).translation.x,worldRuntime);
    if (!bVar2) {
      SpatialSound_UpdateDesiredPositionedGains
                (*(SpatialSoundMaximumDistanceQ12 *)(extraout_ECX + 0x7c),
                 *(SpatialSoundGainQ15 *)(extraout_ECX + 0x78),
                 (GraphicsFixedVec3 *)((ulonglong)uVar3 >> 0x20),(SpatialSoundSlot *)uVar3);
    }
  }
  return;
}

/* Address: 0x005283D0.
   Ownership: gameplay/army/runtime.
   Purpose: Table membership RUNTIME_UPDATE[23]. Raises or lowers the child model local-Z deployment state, toggles
   linked collision/runtime state at the endpoints, and emits the associated projected one-shot sounds. Runtime-
   update partition slots 0-23 receive (worldRuntime, armyRuntime).
   Cross-module calls: TerrainGrid_TestProjectedCellMaskBits01Cf [world/terrain/grid],
   SpatialSound_PlayPositionedOneShot [audio/spatial/runtime], ModelNodeRuntime_RebuildTransformsFromRoot
   [world/model/hierarchy], ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf [gameplay/army/placement].
*/
void ArmyRuntimeClass_UpdateVerticalDeploymentAndCollisionState
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  Q12 *pQVar1;
  uint uVar2;
  void **ppvVar3;
  int iVar4;
  int iVar5;
  ArmyRuntimeSlot *worldPosition;
  ArmyRuntimeSlot *extraout_ECX;
  ArmyRuntimeSlot *extraout_ECX_00;
  ArmyRuntimeSlot *worldPosition_00;
  ArmyRuntimeSlot *extraout_ECX_01;
  ArmyRuntimeSlot *armyRuntime_00;
  void *pvVar6;
  bool bVar7;
  undefined8 uVar8;
  ModelRuntimeNode *modelNode1;
  
  pvVar6 = armyRuntime->definitionOrAsset;
  modelNode1 = armyRuntime->modelNodeRuntime;
  armyRuntime_00 = armyRuntime->linkedArmyRuntime;
  iVar4 = *(int *)((int)pvVar6 + 0x24);
  if (((armyRuntime->articulatedContact).fallbackPosition0Q12 & 1U) == 0) {
    if (armyRuntime->classState60 != 0) {
      if ((int)armyRuntime->classState60 <= iVar4) {
        uVar2 = *(uint *)((int)pvVar6 + 0x26c);
        armyRuntime_00 = (ArmyRuntimeSlot *)worldRuntime->dwordArray;
        if (((uVar2 != 0) && (uVar2 < worldRuntime->dwordArrayCount)) &&
           (armyRuntime_00 != (ArmyRuntimeSlot *)0x0)) {
          ppvVar3 = &armyRuntime_00->definitionOrAsset;
          armyRuntime_00 = (ArmyRuntimeSlot *)&(modelNode1->worldTransform).translation;
          bVar7 = false;
          if (ppvVar3[uVar2] != (void *)0x0) {
            uVar8 = TerrainGrid_TestProjectedCellMaskBits01Cf
                              ((modelNode1->worldTransform).translation.y,
                               (modelNode1->worldTransform).translation.x,worldRuntime);
            pvVar6 = (void *)((ulonglong)uVar8 >> 0x20);
            armyRuntime_00 = worldPosition;
            if (!bVar7) {
              uVar8 = SpatialSound_PlayPositionedOneShot
                                (*(SpatialSoundMaximumDistanceQ12 *)((int)pvVar6 + 0x7c),
                                 *(SpatialSoundGainQ15 *)((int)pvVar6 + 0x78),
                                 (GraphicsFixedVec3 *)worldPosition,(DirectSoundVoiceSet **)uVar8);
              pvVar6 = (void *)((ulonglong)uVar8 >> 0x20);
              armyRuntime_00 = extraout_ECX;
            }
          }
        }
      }
      modelNode1 = modelNode1->childNodes[0];
      iVar4 = *(int *)((int)pvVar6 + 0xc);
      pQVar1 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
      *pQVar1 = *pQVar1 & 0xfffffffd;
      iVar4 = iVar4 * g_InGameSimulationStepTicks;
      if (modelNode1 != (ModelRuntimeNode *)0x0) {
        pQVar1 = &(modelNode1->modelPayload).localTranslationZQ12;
        *pQVar1 = *pQVar1 + iVar4;
        armyRuntime->classState60 = armyRuntime->classState60 + iVar4;
        ModelNodeRuntime_RebuildTransformsFromRoot
                  (armyRuntime_00,pvVar6,armyRuntime->modelNodeRuntime);
      }
    }
  }
  else {
    if (armyRuntime->ownerValue64 != 0) {
      armyRuntime->ownerValue64 = armyRuntime->ownerValue64 - 1;
    }
    pQVar1 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
    *pQVar1 = *pQVar1 | 2;
    uVar8 = CONCAT44(pvVar6,iVar4);
    if (armyRuntime->ownerValue64 == 0) {
      pQVar1 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
      *pQVar1 = *pQVar1 & 0xfffffffe;
      bVar7 = false;
      uVar8 = CONCAT44(pvVar6,iVar4);
      if (armyRuntime_00 != (ArmyRuntimeSlot *)0x0) {
        armyRuntime->ownerValue64 = 8;
        armyRuntime->linkedArmyRuntime = (ArmyRuntimeSlot *)0x0;
        uVar8 = ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf
                          (*(Q12 *)((int)pvVar6 + 0xdc),(modelNode1->worldTransform).translation.y,
                           (modelNode1->worldTransform).translation.x,armyRuntime_00);
        armyRuntime_00 = extraout_ECX_00;
        if (bVar7) {
          armyRuntime->linkedArmyRuntime = extraout_ECX_00;
          pQVar1 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
          *pQVar1 = *pQVar1 | 1;
        }
      }
    }
    iVar4 = (int)((ulonglong)uVar8 >> 0x20);
    if ((int)uVar8 < (int)armyRuntime->classState60) {
      pQVar1 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
      *pQVar1 = *pQVar1 & 0xfffffffd;
      if (armyRuntime->classState60 == 0) {
        uVar2 = *(uint *)(iVar4 + 0x26c);
        armyRuntime_00 = (ArmyRuntimeSlot *)worldRuntime->dwordArray;
        if (((uVar2 != 0) && (uVar2 < worldRuntime->dwordArrayCount)) &&
           (armyRuntime_00 != (ArmyRuntimeSlot *)0x0)) {
          ppvVar3 = &armyRuntime_00->definitionOrAsset;
          armyRuntime_00 = (ArmyRuntimeSlot *)&(modelNode1->worldTransform).translation;
          bVar7 = false;
          if (ppvVar3[uVar2] != (void *)0x0) {
            uVar8 = TerrainGrid_TestProjectedCellMaskBits01Cf
                              ((modelNode1->worldTransform).translation.y,
                               (modelNode1->worldTransform).translation.x,worldRuntime);
            iVar4 = (int)((ulonglong)uVar8 >> 0x20);
            armyRuntime_00 = worldPosition_00;
            if (!bVar7) {
              uVar8 = SpatialSound_PlayPositionedOneShot
                                (*(SpatialSoundMaximumDistanceQ12 *)(iVar4 + 0x7c),
                                 *(SpatialSoundGainQ15 *)(iVar4 + 0x78),
                                 (GraphicsFixedVec3 *)worldPosition_00,(DirectSoundVoiceSet **)uVar8
                                );
              iVar4 = (int)((ulonglong)uVar8 >> 0x20);
              armyRuntime_00 = extraout_ECX_01;
            }
          }
        }
      }
      if (modelNode1->childNodes[0] != (ModelRuntimeNode *)0x0) {
        iVar5 = *(int *)(iVar4 + 0xc) * g_InGameSimulationStepTicks;
        pQVar1 = &(modelNode1->childNodes[0]->modelPayload).localTranslationZQ12;
        *pQVar1 = *pQVar1 - iVar5;
        armyRuntime->classState60 = armyRuntime->classState60 - iVar5;
        ModelNodeRuntime_RebuildTransformsFromRoot
                  (armyRuntime_00,iVar4,armyRuntime->modelNodeRuntime);
      }
    }
  }
  return;
}

/* Address: 0x00529720.
   Ownership: gameplay/army/runtime.
   Purpose: The helper scans model lookup records, resolves a matching model attachment, derives transformed launch
   state, and calls ShotDefinition_ComputeLaunchAnglesRegs. CF clear reports a match; CF set reports no match. The
   normal return is void and all exits use RET 0x24. Role: Finds one exact packed attachment selector and launches
   a projectile from it. Inputs: Attachment selector, model hierarchy, ShotDefinition, target and owner/world
   state.
   Cross-module calls: ModelNodeRuntime_RebuildTransformsFromRoot [world/model/hierarchy],
   ModelNodeRuntime_TransformLocalPointRegs [world/model/hierarchy], ShotRuntimePool_CreateProjectileFromDefinition
   [world/shots/runtime].
*/
void ArmyRuntime_ResolveShotLaunchFromModelAttachmentCf
               (ShotRuntimeState14 shotRuntimeState14,Q12 targetWorldXQ12,Q12 targetWorldYQ12,
               Q12 targetWorldZQ12,SprAttachmentSelectorOrdinal attachmentSelectorOrdinal,
               ShotDefinition *shotDefinition,ModelRuntimeNode *modelNode,
               MdlSerializedNodeHeader38 *definitionNode,WorldRuntimeContext *worldRuntime)

{
  SpriteAssetHeader *pSVar1;
  undefined4 in_ECX;
  undefined4 extraout_ECX;
  Q12 launchWorldYQ12;
  undefined4 in_EDX;
  Q12 launchWorldZQ12;
  int iVar2;
  ModelPackedPointRecord *localPointRecord;
  undefined8 uVar3;
  
  ModelNodeRuntime_RebuildTransformsFromRoot(in_ECX,in_EDX,modelNode);
  pSVar1 = (definitionNode->spriteAssetReference).spriteAsset;
  iVar2 = *(int *)(pSVar1->reservedBC_1FF + 0x2c);
  localPointRecord =
       (ModelPackedPointRecord *)
       ((pSVar1->registryHeader).common.buildMetadata.assetRelativeAddressAnchor28 +
       *(int *)(pSVar1->reservedBC_1FF + 0x28) + -0x28);
  while( true ) {
    if (iVar2 == 0) {
      return;
    }
    if (localPointRecord->packedLookupKey == (attachmentSelectorOrdinal << 4 | 2)) break;
    localPointRecord = localPointRecord + 1;
    iVar2 = iVar2 + -1;
  }
  uVar3 = ModelNodeRuntime_TransformLocalPointRegs(extraout_ECX,localPointRecord,modelNode);
  launchWorldZQ12 = (Q12)((ulonglong)uVar3 >> 0x20);
  ShotRuntimePool_CreateProjectileFromDefinition
            (shotRuntimeState14,
             (ArmyRuntimeSlot *)((modelNode->runtimePayload).armyRuntime)->linkedEntityRuntime,
             targetWorldXQ12,targetWorldYQ12,targetWorldZQ12,launchWorldZQ12,launchWorldYQ12,
             (Q12)uVar3,shotDefinition,worldRuntime);
  return;
}

/* Address: 0x00529B50.
   Ownership: gameplay/army/runtime.
   Purpose: Computes and clamps the activation metric from definition and runtime state, stores it atomically, and
   plays the configured positioned start sound on a zero-to-nonzero transition when the projected terrain cell is
   available.
   Cross-module calls: TerrainGrid_TestProjectedCellMaskBits01Cf [world/terrain/grid],
   SpatialSound_PlayPositionedOneShot [audio/spatial/runtime].
*/
void ArmyRuntime_UpdateActivationMetricAndPlayStartSound
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  int iVar1;
  void *pvVar2;
  AngleTurn32 AVar3;
  uint uVar4;
  uint uVar5;
  bool bVar6;
  undefined8 uVar7;
  ModelRuntimeNode *modelNode1;
  
  modelNode1 = armyRuntime->modelNodeRuntime;
  pvVar2 = armyRuntime->definitionOrAsset;
  AVar3 = (modelNode1->modelPayload).worldRotationAngle1;
  uVar5 = *(uint *)((int)pvVar2 + 0xc);
  if ((int)AVar3 < *(int *)((int)pvVar2 + 0x268)) {
    uVar4 = (modelNode1->modelPayload).worldRotationAngle2 -
            (modelNode1->modelPayload).worldRotationAngle0 & 0xffff;
    uVar5 = uVar5 * 5 >> 4;
    if (((*(int *)((int)pvVar2 + 0x24) <= (int)AVar3) &&
        (uVar5 = *(uint *)((int)pvVar2 + 0xc), 0x4000 < uVar4)) && (uVar4 < 0xc000)) {
      uVar5 = uVar5 * 5 >> 3;
    }
  }
  uVar4 = (armyRuntime->movementControl).movementAdvancePerTickQ12;
  if ((uVar4 < uVar5) && (uVar4 = uVar4 + *(int *)((int)pvVar2 + 0x18), uVar4 < uVar5)) {
    uVar5 = uVar4;
  }
  LOCK();
  iVar1 = (armyRuntime->movementControl).movementAdvancePerTickQ12;
  (armyRuntime->movementControl).movementAdvancePerTickQ12 = uVar5;
  UNLOCK();
  if ((iVar1 == 0) && (uVar5 != 0)) {
    uVar5 = *(uint *)((int)pvVar2 + 0xd4);
    if ((uVar5 != 0) &&
       ((uVar5 < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != (dword *)0x0)))) {
      bVar6 = false;
      if (worldRuntime->dwordArray[uVar5] != 0) {
        uVar7 = TerrainGrid_TestProjectedCellMaskBits01Cf
                          ((modelNode1->worldTransform).translation.y,
                           (modelNode1->worldTransform).translation.x,worldRuntime);
        if (!bVar6) {
          SpatialSound_PlayPositionedOneShot
                    (*(SpatialSoundMaximumDistanceQ12 *)((int)pvVar2 + 0x7c),
                     *(SpatialSoundGainQ15 *)((int)pvVar2 + 0x78),
                     (GraphicsFixedVec3 *)((ulonglong)uVar7 >> 0x20),(DirectSoundVoiceSet **)uVar7);
        }
      }
    }
  }
  return;
}

/* Address: 0x0052A040.
   Ownership: gameplay/army/runtime.
   Purpose: Handles a resolved collision partner. The class-0x17 path links compatible same-faction targets and
   initializes their state, while the class-zero path computes a bearing and invokes the verified collision-damage
   helper. Typed parameters: p3 currentWorldXQ12→Q12, p4 currentWorldYQ12→Q12. Nearby but non-identical semantic
   domains were explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals,
   locals, and executable data remain unchanged.
   Cross-module calls: FixedMath_Atan2Angle16 [core/math/fixed], ArmyRuntime_ApplyImpactDamageAndFinalizeState
   [gameplay/army/combat].
*/
void ArmyRuntime_HandleCollisionPartner
               (ArmyRuntimeSlot *currentArmyRuntime,Q12 currentWorldXQ12,Q12 currentWorldYQ12,
               ArmyRuntimeSlot *collisionPartnerArmyRuntime,WorldRuntimeContext *worldRuntime)

{
  Q12 *pQVar1;
  dword impactAngle;
  
  if (collisionPartnerArmyRuntime != (ArmyRuntimeSlot *)0x0) {
    if (*(int *)((int)collisionPartnerArmyRuntime->definitionOrAsset + 0x4c) == 0x17) {
      if (((collisionPartnerArmyRuntime->linkedEntityRuntime->common).ownership.ownerIndex ==
           (currentArmyRuntime->linkedEntityRuntime->common).ownership.ownerIndex) &&
         (collisionPartnerArmyRuntime->linkedArmyRuntime == (ArmyRuntimeSlot *)0x0)) {
        pQVar1 = &(collisionPartnerArmyRuntime->articulatedContact).fallbackPosition0Q12;
        *pQVar1 = *pQVar1 | 1;
        collisionPartnerArmyRuntime->ownerValue64 = 0x20;
        if (((collisionPartnerArmyRuntime->articulatedContact).fallbackPosition0Q12 & 2U) != 0) {
          collisionPartnerArmyRuntime->linkedArmyRuntime = currentArmyRuntime;
          currentArmyRuntime->linkedArmyRuntime = collisionPartnerArmyRuntime;
        }
      }
    }
    else if (*(int *)((int)collisionPartnerArmyRuntime->definitionOrAsset + 0x4c) == 0) {
      impactAngle = FixedMath_Atan2Angle16
                              ((collisionPartnerArmyRuntime->modelNodeRuntime->worldTransform).
                               translation.y - currentWorldXQ12,
                               (collisionPartnerArmyRuntime->modelNodeRuntime->worldTransform).
                               translation.x - currentWorldYQ12);
      ArmyRuntime_ApplyImpactDamageAndFinalizeState
                (impactAngle,0x100000,collisionPartnerArmyRuntime);
    }
  }
  return;
}

/* Address: 0x0051BC00.
   Ownership: gameplay/army/runtime.
   Purpose: Creates a temporary army runtime instance, prepares its model view and transformed bounds, renders a
   generated preview texture, applies the verified MMX reduction and color postprocess, destroys the temporary
   instance, and returns status through carry. Typed parameters: p2 previewHeight→GraphicsPixelDimension_V302.
   Nearby but non-identical semantic domains were explicitly deferred. Calling convention, parameter storage, body
   bytes, control flow, globals, locals, and executable data remain unchanged. Typed parameters: p3
   previewWidth→GraphicsPixelDimension_V302.
   Local calls: ArmyRuntime_CreateInstanceFromAssetCf, ArmyRuntime_DestroyInstanceAndRefreshUi.
   Cross-module calls: WorldRuntime_UnlinkNodeFromOwnerListD8 [world/runtime/core],
   ModelNodeRuntime_RebuildTransformsFromRoot [world/model/hierarchy],
   ModelNodeRuntime_AccumulateTransformedBoundsRecursive [world/model/hierarchy].
*/
undefined8 __fastcall
ArmyRuntime_RenderPreviewTextureCf
          (undefined4 param_1,undefined4 param_2,GraphicsPixelDimension previewHeight,
          GraphicsPixelDimension previewWidth,FactionRuntimeIndex factionIndex,
          PckArmyAssetIdCatalog armyAssetId,WorldRuntimeContext *worldRuntime)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  GraphicsPixelDimension GVar5;
  GameEntityRuntime *entityRuntime1;
  int iVar6;
  GameEntityRuntime *memory;
  void *pvVar7;
  int extraout_ECX;
  int iVar8;
  int *piVar9;
  int iVar10;
  Q12 *pQVar11;
  Q12 *pQVar12;
  undefined1 in_CF;
  bool bVar13;
  ushort uVar14;
  ushort uVar15;
  undefined1 uVar16;
  undefined1 uVar17;
  ushort uVar18;
  ushort uVar19;
  ushort uVar20;
  ushort uVar21;
  undefined8 mm0PackedValue0;
  ushort uVar22;
  ushort uVar23;
  undefined1 uVar24;
  undefined1 uVar25;
  undefined8 mm1PackedValue0;
  undefined1 uVar26;
  undefined1 uVar27;
  undefined8 mm2PackedValue0;
  undefined1 uVar28;
  undefined1 uVar29;
  undefined8 mm3PackedValue0;
  undefined8 uVar30;
  ModelRuntimeNode *modelNode1;
  
  GVar5 = previewWidth;
  entityRuntime1 =
       (GameEntityRuntime *)
       ArmyRuntime_CreateInstanceFromAssetCf
                 (1,0,0x6000000,0x6000000,factionIndex,armyAssetId,worldRuntime);
  if (!(bool)in_CF) {
    piVar9 = (entityRuntime1->common).ownership.definitionOrClassRecord;
    modelNode1 = (entityRuntime1->common).ownership.modelNode;
    iVar8 = *piVar9;
    if (((*(int *)(iVar8 + 0x4c) == 0xd) && (3 < modelNode1->childCount)) &&
       (modelNode1->childNodes[3] != (ModelRuntimeNode *)0x0)) {
      uVar30 = WorldRuntime_UnlinkNodeFromOwnerListD8((WorldRuntimeNode *)modelNode1->childNodes[3])
      ;
      piVar9 = (int *)((ulonglong)uVar30 >> 0x20);
      modelNode1->childNodes[3] = (ModelRuntimeNode *)0x0;
      iVar8 = extraout_ECX;
    }
    (modelNode1->modelPayload).worldRotationAngle2 = 0x2000;
    (modelNode1->modelPayload).worldRotationAngle1 = 0x3000;
    modelNode1->runtimeFlags = modelNode1->runtimeFlags | 1;
    modelNode1->tintArgb = 0xffffffff;
    ModelNodeRuntime_RebuildTransformsFromRoot(iVar8,piVar9,modelNode1);
    g_ModelBoundsMinimumX = (modelNode1->worldTransform).translation.x;
    g_ModelBoundsMinimumY = (modelNode1->worldTransform).translation.y;
    g_ModelBoundsMinimumZ = (modelNode1->worldTransform).translation.z;
    g_ModelBoundsMaximumX = g_ModelBoundsMinimumX;
    g_ModelBoundsMaximumY = g_ModelBoundsMinimumY;
    g_ModelBoundsMaximumZ = g_ModelBoundsMinimumZ;
    ModelNodeRuntime_AccumulateTransformedBoundsRecursive(modelNode1);
    iVar6 = g_ModelBoundsMaximumY - g_ModelBoundsMinimumY;
    iVar10 = g_ModelBoundsMaximumZ - g_ModelBoundsMinimumZ;
    iVar8 = iVar10;
    if (iVar10 < iVar6) {
      iVar8 = iVar6;
    }
    g_ArmyPreviewViewOriginYQ12 = iVar6 + g_ModelBoundsMinimumY * 2 >> 1;
    g_ArmyPreviewViewOriginZQ12 = iVar10 + g_ModelBoundsMinimumZ * 2 >> 1;
    g_ArmyPreviewAuxiliaryOrientation0 = 0x6000;
    g_ArmyPreviewAuxiliaryOrientation1 = 0xffffe667;
    g_ArmyPreviewViewOriginXQ12 = g_ModelBoundsMaximumX + iVar8 * 4;
    g_ArmyPreviewPrimaryColorArgb = 0xffc0c0c0;
    g_ArmyPreviewSecondaryColorArgb = 0xff606060;
    g_ArmyPreviewProjectionScaleQ12 = 0x800;
    g_ArmyPreviewViewAngle0 = 0xffff8000;
    g_ArmyPreviewViewAngle1 = 0;
    g_ArmyPreviewProjectionShift = 4;
    bVar13 = CARRY4(previewHeight,previewHeight);
    g_ArmyPreviewModelNodePointer = (dword)modelNode1;
    memory = (GameEntityRuntime *)
             (*(code *)g_GraphicsOffscreenRenderModelListToTextureSourceCf)
                       (&g_ArmyPreviewPrimaryColorArgb,&g_ArmyPreviewAuxiliaryOrientation0,
                        &g_ArmyPreviewViewOriginXQ12,previewHeight * 2,previewWidth * 2,1,
                        &g_ArmyPreviewModelNodePointer);
    if (!bVar13) {
      pQVar11 = &memory[1].common.commandTarget.targetWorldXQ12;
      ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,entityRuntime1);
      pQVar12 = pQVar11;
      previewWidth = GVar5;
      do {
        do {
          uVar1 = *pQVar11;
          uVar2 = pQVar11[1];
          uVar3 = pQVar11[GVar5 * 2];
          uVar4 = pQVar11[GVar5 * 2 + 1];
          uVar16 = (undefined1)(uVar1 >> 0x18);
          uVar14 = CONCAT11(uVar16,uVar16);
          uVar17 = (undefined1)(uVar1 >> 0x10);
          uVar16 = (undefined1)(uVar1 >> 8);
          uVar24 = (undefined1)(uVar2 >> 0x18);
          uVar15 = CONCAT11(uVar24,uVar24);
          uVar25 = (undefined1)(uVar2 >> 0x10);
          uVar24 = (undefined1)(uVar2 >> 8);
          uVar26 = (undefined1)(uVar3 >> 0x18);
          uVar18 = CONCAT11(uVar26,uVar26);
          uVar27 = (undefined1)(uVar3 >> 0x10);
          uVar26 = (undefined1)(uVar3 >> 8);
          uVar28 = (undefined1)(uVar4 >> 0x18);
          uVar19 = CONCAT11(uVar28,uVar28);
          uVar29 = (undefined1)(uVar4 >> 0x10);
          uVar28 = (undefined1)(uVar4 >> 8);
          mm0PackedValue0 =
               pmulhw(CONCAT26(uVar14 >> 4,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(uVar14,uVar17),
                                                          CONCAT14(uVar17,uVar1)) >> 0x20) >> 4,
                                        CONCAT22(CONCAT11(uVar16,uVar16) >> 4,
                                                 CONCAT11((char)uVar1,(char)uVar1) >> 4))),
                      *(undefined8 *)(&g_ArmyPreviewAlphaPremultiplyMmxLut256 + (uVar1 >> 0x18) * 8)
                     );
          mm1PackedValue0 =
               pmulhw(CONCAT26(uVar15 >> 4,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(uVar15,uVar25),
                                                          CONCAT14(uVar25,uVar2)) >> 0x20) >> 4,
                                        CONCAT22(CONCAT11(uVar24,uVar24) >> 4,
                                                 CONCAT11((char)uVar2,(char)uVar2) >> 4))),
                      *(undefined8 *)(&g_ArmyPreviewAlphaPremultiplyMmxLut256 + (uVar2 >> 0x18) * 8)
                     );
          mm2PackedValue0 =
               pmulhw(CONCAT26(uVar18 >> 4,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(uVar18,uVar27),
                                                          CONCAT14(uVar27,uVar3)) >> 0x20) >> 4,
                                        CONCAT22(CONCAT11(uVar26,uVar26) >> 4,
                                                 CONCAT11((char)uVar3,(char)uVar3) >> 4))),
                      *(undefined8 *)(&g_ArmyPreviewAlphaPremultiplyMmxLut256 + (uVar3 >> 0x18) * 8)
                     );
          mm3PackedValue0 =
               pmulhw(CONCAT26(uVar19 >> 4,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(uVar19,uVar29),
                                                          CONCAT14(uVar29,uVar4)) >> 0x20) >> 4,
                                        CONCAT22(CONCAT11(uVar28,uVar28) >> 4,
                                                 CONCAT11((char)uVar4,(char)uVar4) >> 4))),
                      *(undefined8 *)(&g_ArmyPreviewAlphaPremultiplyMmxLut256 + (uVar4 >> 0x18) * 8)
                     );
          uVar30 = *(undefined8 *)
                    (&g_ArmyPreviewAverageAlphaReciprocalMmxLut256 +
                    ((uVar1 >> 0x18) + (uVar2 >> 0x18) + (uVar3 >> 0x18) + (uVar4 >> 0x18) >> 2) * 8
                    );
          uVar14 = ((short)mm0PackedValue0 + (short)mm1PackedValue0 +
                    (short)mm2PackedValue0 + (short)mm3PackedValue0 +
                   (short)g_ArmyPreviewDownsampleAlphaRoundingBiasMmx) * (short)uVar30;
          uVar18 = ((short)((ulonglong)mm0PackedValue0 >> 0x10) +
                    (short)((ulonglong)mm1PackedValue0 >> 0x10) +
                    (short)((ulonglong)mm2PackedValue0 >> 0x10) +
                    (short)((ulonglong)mm3PackedValue0 >> 0x10) +
                   (short)((ulonglong)g_ArmyPreviewDownsampleAlphaRoundingBiasMmx >> 0x10)) *
                   (short)((ulonglong)uVar30 >> 0x10);
          uVar20 = ((short)((ulonglong)mm0PackedValue0 >> 0x20) +
                    (short)((ulonglong)mm1PackedValue0 >> 0x20) +
                    (short)((ulonglong)mm2PackedValue0 >> 0x20) +
                    (short)((ulonglong)mm3PackedValue0 >> 0x20) +
                   (short)((ulonglong)g_ArmyPreviewDownsampleAlphaRoundingBiasMmx >> 0x20)) *
                   (short)((ulonglong)uVar30 >> 0x20);
          uVar22 = ((short)((ulonglong)mm0PackedValue0 >> 0x30) +
                    (short)((ulonglong)mm1PackedValue0 >> 0x30) +
                    (short)((ulonglong)mm2PackedValue0 >> 0x30) +
                    (short)((ulonglong)mm3PackedValue0 >> 0x30) +
                   (short)((ulonglong)g_ArmyPreviewDownsampleAlphaRoundingBiasMmx >> 0x30)) *
                   (short)((ulonglong)uVar30 >> 0x30);
          uVar15 = uVar14 >> 8;
          uVar19 = uVar18 >> 8;
          uVar21 = uVar20 >> 8;
          uVar23 = uVar22 >> 8;
          *pQVar12 = CONCAT13((uVar23 != 0) * (uVar23 < 0x100) * (char)(uVar22 >> 8) -
                              (0xff < uVar23),
                              CONCAT12((uVar21 != 0) * (uVar21 < 0x100) * (char)(uVar20 >> 8) -
                                       (0xff < uVar21),
                                       CONCAT11((uVar19 != 0) * (uVar19 < 0x100) *
                                                (char)(uVar18 >> 8) - (0xff < uVar19),
                                                (uVar15 != 0) * (uVar15 < 0x100) *
                                                (char)(uVar14 >> 8) - (0xff < uVar15))));
          pQVar11 = pQVar11 + 2;
          pQVar12 = pQVar12 + 1;
          previewWidth = previewWidth - 1;
        } while (previewWidth != 0);
        pQVar11 = pQVar11 + GVar5 * 2;
        previewHeight = previewHeight - 1;
        previewWidth = GVar5;
      } while (previewHeight != 0);
      pvVar7 = (void *)((int)memory[1].common.commandFlags >> 1);
      entityRuntime1 = (GameEntityRuntime *)((int)memory[1].common.commandTarget.targetEntity >> 1);
      memory[1].common.commandFlags = (GameEntityCommandFlags)pvVar7;
      memory[1].common.commandTarget.targetEntity = entityRuntime1;
      memory[1].common.ownership.definitionOrClassRecord = pvVar7;
      memory[1].common.ownership.modelNode = (ModelRuntimeNode *)entityRuntime1;
      modelNode1 = (ModelRuntimeNode *)((int)pvVar7 * (int)entityRuntime1 * 4 + 0x220);
      (memory->common).ownership.modelNode = modelNode1;
      (*g_MemoryApi.shrinkInPlace)((dword)modelNode1,memory);
      return CONCAT44(param_2,memory);
    }
    ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,entityRuntime1);
    entityRuntime1 = memory;
  }
  return CONCAT44(param_2,entityRuntime1);
}

/* Address: 0x0052A7C0.
   Ownership: gameplay/army/runtime.
   Purpose: Executes the class-indexed timed-emitter callback, advances progress, health, resource, and technology
   state, triggers configured effects, and recursively updates child runtimes.
   [RESOURCE_FUEL_ENERGY_CAPACITY_SEPARATION_CLOSURE] Research state: pending technology pays Xenite once from
   faction xeniteCurrentQ4, then adds technology Energy requirement to ArmyRuntime active load while research is in
   progress; completion unlocks the technology and removes that Energy load.
   Local calls: ArmyRuntimeSpawner_ComputeRemainingLinkedAssetMetric, ArmyRuntime_ProcessReadyAttachmentChannels.
   Cross-module calls: EffectRuntimePool_CreateInstanceFromDefinitionCf [world/effects/runtime],
   Technology_UnlockForFaction [gameplay/technology/runtime].
*/
undefined8 __fastcall
ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive
          (undefined4 incomingEcxValue,undefined4 preservedEdxValue,
          WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  Q12 *pQVar1;
  byte *pbVar2;
  void *pvVar3;
  ArmyRuntimeSlot *armyRuntime_00;
  undefined4 in_EAX;
  uint uVar4;
  int iVar5;
  int extraout_ECX;
  int extraout_ECX_00;
  int iVar6;
  int extraout_ECX_01;
  uint uVar7;
  undefined8 uVar8;
  ModelRuntimeNode *modelNode1;
  
  pvVar3 = armyRuntime->definitionOrAsset;
  (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[*(int *)((int)pvVar3 + 0x4c)])
            (worldRuntime,armyRuntime);
  if ((armyRuntime->runtimeFlags & 0x10) == 0) {
    pbVar2 = armyRuntime->reservedF8_FF;
    *(InGameSimulationStepBatchTicks *)pbVar2 = *(int *)pbVar2 - g_InGameSimulationStepTicks;
    if (*(int *)pbVar2 < 0) {
      armyRuntime->reservedF8_FF[0] = 4;
      armyRuntime->reservedF8_FF[1] = 0;
      armyRuntime->reservedF8_FF[2] = 0;
      armyRuntime->reservedF8_FF[3] = 0;
      iVar6 = armyRuntime->actionVector2Q12;
      uVar7 = *(int *)((int)pvVar3 + 0x60) * 3;
      if (iVar6 != 0) {
        if ((armyRuntime->runtimeFlags & 1) == 0) {
          if ((armyRuntime->runtimeFlags & 0x400) == 0) {
            uVar4 = iVar6 + *(int *)((int)pvVar3 + 0x1b4);
            uVar7 = uVar7 >> 2;
            if ((int)uVar7 < (int)uVar4) {
              uVar4 = uVar7;
            }
            if (armyRuntime->actionVector2Q12 < (int)uVar4) {
              armyRuntime->actionVector2Q12 = uVar4;
            }
          }
        }
        else {
          uVar4 = iVar6 - *(int *)((int)pvVar3 + 0x1b4);
          uVar7 = uVar7 >> 2;
          if ((int)uVar4 < (int)uVar7) {
            uVar4 = uVar7;
          }
          if ((int)uVar4 < armyRuntime->actionVector2Q12) {
            armyRuntime->actionVector2Q12 = uVar4;
          }
        }
      }
    }
  }
  pbVar2 = armyRuntime->reservedF8_FF + 4;
  *(InGameSimulationStepBatchTicks *)pbVar2 = *(int *)pbVar2 - g_InGameSimulationStepTicks;
  if ((((*(int *)pbVar2 < 0) &&
       (*(int *)(armyRuntime->reservedF8_FF + 4) = *(int *)(armyRuntime->reservedF8_FF + 4) + 0xc,
       (armyRuntime->runtimeFlags & 0x10) != 0)) && (*(int *)((int)pvVar3 + 0x184) != 0)) &&
     ((*(int *)((int)pvVar3 + 0x60) != 0 && (0 < armyRuntime->actionVector2Q12)))) {
    iVar6 = (armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex;
    iVar5 = iVar6 * 0x740;
    g_GameFactionRuntimeImage.records[iVar6].xeniteCurrentQ4 =
         g_GameFactionRuntimeImage.records[iVar6].xeniteCurrentQ4 +
         (*(uint *)((int)pvVar3 + 0x184) >> 5);
    uVar7 = *(uint *)((int)pvVar3 + 0x60);
    if (*(int *)((int)pvVar3 + 0x4c) == 0x16) {
      uVar8 = ArmyRuntimeSpawner_ComputeRemainingLinkedAssetMetric
                        ((ArmyRuntimeLinkedChildMaskSlotView *)armyRuntime);
      uVar7 = (uint)uVar8;
      pbVar2 = g_GameFactionRuntimeImage.records[0].reserved78_87 + extraout_ECX + -0x78;
      *(int *)pbVar2 = *(int *)pbVar2 + (int)((ulonglong)uVar8 >> 0x20);
      iVar5 = extraout_ECX;
    }
    pQVar1 = &armyRuntime->actionVector2Q12;
    iVar6 = *pQVar1;
    *pQVar1 = *pQVar1 - (uVar7 >> 4);
    if (*pQVar1 == 0 || SBORROW4(iVar6,uVar7 >> 4) != *pQVar1 < 0) {
      armyRuntime->actionVector2Q12 = 0;
      armyRuntime->runtimeFlags = armyRuntime->runtimeFlags ^ 0x210;
      modelNode1 = armyRuntime->modelNodeRuntime;
      EffectRuntimePool_CreateInstanceFromDefinitionCf
                (iVar5,modelNode1,EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY,armyRuntime,
                 (modelNode1->modelPayload).worldRotationAngle2,
                 (modelNode1->modelPayload).worldRotationAngle1,
                 (modelNode1->modelPayload).worldRotationAngle0,
                 (modelNode1->worldTransform).translation.z,
                 (modelNode1->worldTransform).translation.y,
                 (modelNode1->worldTransform).translation.x,
                 *(EffectDefinition **)((int)pvVar3 + 400),worldRuntime);
      armyRuntime->runtimeFlags = armyRuntime->runtimeFlags | 0x20;
      goto 
      ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive_ContinueWithTechnologyPaymentAndChildRecursion
      ;
    }
  }
  if ((((armyRuntime->runtimeFlags & 0x20) == 0) && (armyRuntime->actionVector2Q12 < 1)) &&
     (armyRuntime->actionVector1Q12 != 0)) {
    do {
      ArmyRuntime_ProcessReadyAttachmentChannels(worldRuntime,armyRuntime);
      armyRuntime->runtimeState40 = armyRuntime->runtimeState40 - 1;
      armyRuntime->runtimeState44 = armyRuntime->runtimeState44 - 1;
      armyRuntime->runtimeState48 = armyRuntime->runtimeState48 - 1;
      armyRuntime->runtimeState4C = armyRuntime->runtimeState4C - 1;
      armyRuntime->terrainOccupancyMask0 = armyRuntime->terrainOccupancyMask0 - 1;
      armyRuntime->terrainOccupancyMask1 = armyRuntime->terrainOccupancyMask1 - 1;
      armyRuntime->movementPosition0Q12 = armyRuntime->movementPosition0Q12 + -1;
      armyRuntime->movementPosition1Q12 = armyRuntime->movementPosition1Q12 + -1;
    } while (extraout_ECX_00 != 1);
  }

  ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive_ContinueWithTechnologyPaymentAndChildRecursion
  :
  if (((armyRuntime->runtimeFlags & 0x40) != 0) &&
     (iVar6 = armyRuntime->selectionMetric0 + g_InGameSimulationStepTicks,
     (armyRuntime->runtimeFlags & 1) == 0)) {
    armyRuntime->selectionMetric0 = iVar6;
    if ((int)armyRuntime->runtimeState104 <= iVar6) {
      Technology_UnlockForFaction
                ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,
                 (armyRuntime->modelNodeRuntime->worldTransform).translation.x,
                 armyRuntime->stateOrTechnologyId,
                 (armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex);
      armyRuntime->runtimeFlags = armyRuntime->runtimeFlags & 0xffffffbf;
      armyRuntime->runtimeTimer = armyRuntime->runtimeTimer - armyRuntime->selectionMetric1;
    }
  }
  if (((armyRuntime->runtimeFlags & 0x80) != 0) && ((armyRuntime->runtimeFlags & 0x140) == 0)) {
    iVar6 = (armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex;
    iVar5 = armyRuntime->selectionMetric1;
    if (armyRuntime->selectionMetric2 <=
        (int)g_GameFactionRuntimeImage.records[iVar6].xeniteCurrentQ4) {
      g_GameFactionRuntimeImage.records[iVar6].xeniteCurrentQ4 =
           g_GameFactionRuntimeImage.records[iVar6].xeniteCurrentQ4 - armyRuntime->selectionMetric2;
      armyRuntime->selectionMetric2 = 0;
      armyRuntime->runtimeFlags = armyRuntime->runtimeFlags ^ 0xc0;
      armyRuntime->runtimeTimer = armyRuntime->runtimeTimer + iVar5;
    }
  }
  iVar6 = armyRuntime->factionIndex;
  if (iVar6 != 0) {
    uVar7 = armyRuntime->runtimeFlags & 8;
    do {
      armyRuntime_00 = (ArmyRuntimeSlot *)armyRuntime[1].commandCoordinate0Q12;
      if (armyRuntime_00 != (ArmyRuntimeSlot *)0x0) {
        armyRuntime_00->runtimeFlags = armyRuntime_00->runtimeFlags | uVar7;
        uVar8 = ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive
                          (iVar6,uVar7,worldRuntime,armyRuntime_00);
        uVar7 = (uint)((ulonglong)uVar8 >> 0x20);
        iVar6 = extraout_ECX_01;
      }
      armyRuntime = (ArmyRuntimeSlot *)&armyRuntime->commandCoordinate0Q12;
      iVar6 = iVar6 + -1;
    } while (iVar6 != 0);
  }
  return CONCAT44(preservedEdxValue,in_EAX);
}

/* Address: 0x00527010.
   Ownership: gameplay/army/runtime.
   Purpose: Finds an unused linked-asset slot, creates the corresponding army runtime instance, attaches it to the
   parent, copies the command payload, and scales its inherited metric. Stock ARM contains 675 records and 326
   unique ids; placement workspace, producer, tier, class, and faction-role semantics are not inferred from numeric
   adjacency. Typed parameters: p2 inheritedValue78→WorldMotionValue78_V344, p3
   inheritedValue74→WorldMotionValue74_V344, p4 inheritedValue70→WorldMotionValue70_V344. Calling convention,
   complete VariableStorage serialization, function bytes, control flow, globals, locals, and executable data
   remain unchanged.
   Local calls: ArmyRuntime_CreateInstanceFromAssetCf.
   Cross-module calls: ModelNodeRuntime_RebuildTransformsFromRoot [world/model/hierarchy].
*/
undefined8
ArmyRuntimeSpawner_CreateLinkedChildInstanceCf
          (WorldMotionValue78 inheritedValue78,WorldMotionValue74 inheritedValue74,
          WorldMotionValue70 inheritedValue70,PckArmyAssetIdCatalog linkedArmyAssetId,
          WorldRuntimeContext *worldRuntime,ArmyRuntimeLinkedChildMaskSlotView *armyRuntime)

{
  ArmyRuntimeLinkedChildSlotMaskState4 *pAVar1;
  void *pvVar2;
  ModelRuntimeNode *pMVar3;
  Q12 QVar4;
  undefined4 in_EAX;
  ArmyRuntimeSlot *pAVar5;
  int iVar6;
  undefined4 in_EDX;
  int *piVar7;
  uint uVar8;
  ArmyRuntimeLinkedChildMaskSlotView *pAVar9;
  bool bVar10;
  undefined8 uVar11;
  ModelRuntimeNode *modelNode1;
  
  uVar8 = 1;
  iVar6 = *(int *)((int)armyRuntime->definitionOrAsset + 0xc4);
  pAVar9 = armyRuntime;
  while ((linkedArmyAssetId != pAVar9->movementTarget0Q12 ||
         (bVar10 = false,
         ((armyRuntime->articulatedContact).linkedChildSlotMaskState.linkedChildSlotMask & uVar8) !=
         0))) {
    pAVar9 = (ArmyRuntimeLinkedChildMaskSlotView *)&pAVar9->modelNodeRuntime;
    uVar8 = uVar8 * 2;
    iVar6 = iVar6 + -1;
    if (iVar6 == 0) {
ArmyRuntimeSpawner_CreateLinkedChildInstance_ReturnWithoutCreatedChild:
      return CONCAT44(in_EDX,in_EAX);
    }
  }
  modelNode1 = armyRuntime->modelNodeRuntime;
  pAVar5 = ArmyRuntime_CreateInstanceFromAssetCf
                     (0,(modelNode1->modelPayload).worldRotationAngle2,
                      (modelNode1->worldTransform).translation.y,
                      (modelNode1->worldTransform).translation.x,
                      (armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex,
                      linkedArmyAssetId,worldRuntime);
  if (!bVar10) {
    pvVar2 = pAVar5->definitionOrAsset;
    pAVar1 = &(armyRuntime->articulatedContact).linkedChildSlotMaskState;
    pAVar1->linkedChildSlotMask = pAVar1->linkedChildSlotMask | uVar8;
    armyRuntime->fallbackWorldYQ12 = armyRuntime->fallbackWorldYQ12 + -1;
    *(ArmyRuntimeLinkedChildMaskSlotView **)((int)pvVar2 + 0x60) = armyRuntime;
    *(undefined4 *)((int)pvVar2 + 0xb8) = 1;
    modelNode1 = *(ModelRuntimeNode **)((int)pvVar2 + 4);
    *(WorldMotionValue70 *)((int)pvVar2 + 0x70) = inheritedValue70;
    *(WorldMotionValue74 *)((int)pvVar2 + 0x74) = inheritedValue74;
    pMVar3 = armyRuntime->modelNodeRuntime;
    *(WorldMotionValue78 *)((int)pvVar2 + 0x78) = inheritedValue78;
    QVar4 = (pMVar3->childNodes[0]->modelPayload).localTranslationZQ12;
    (modelNode1->childNodes[0]->modelPayload).localTranslationZQ12 = QVar4;
    uVar11 = ModelNodeRuntime_RebuildTransformsFromRoot(QVar4,pvVar2,modelNode1);
    piVar7 = (int *)((ulonglong)uVar11 >> 0x20);
    piVar7[0xf] = (int)(((longlong)armyRuntime->actionVector2Q12 *
                        (longlong)*(int *)(*piVar7 + 0x60)) /
                       (longlong)*(int *)((int)armyRuntime->definitionOrAsset + 0x60));
    return CONCAT44(in_EDX,in_EAX);
  }
  goto ArmyRuntimeSpawner_CreateLinkedChildInstance_ReturnWithoutCreatedChild;
}

/* Address: 0x00527430.
   Ownership: gameplay/army/runtime.
   Purpose: Tests proximity across the base model and its attachment descriptors for two army runtimes; status is
   returned through CF.
   Local calls: ArmyRuntime_TestPositionDistanceWithinCombinedRadiusCf.
*/
void ArmyRuntime_TestModelAttachmentProximityCf
               (ArmyRuntimeSlot *candidateArmyRuntime,ArmyRuntimeSlot *sourceArmyRuntime)

{
  int *piVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  int iVar2;
  int extraout_EDX;
  int extraout_EDX_00;
  int iVar3;
  undefined1 in_CF;
  bool bVar4;
  
  ArmyRuntime_TestPositionDistanceWithinCombinedRadiusCf
            (*(UQ12 *)((int)candidateArmyRuntime->definitionOrAsset + 0x1a0),
             *(UQ12 *)((int)sourceArmyRuntime->definitionOrAsset + 0x1a0),
             candidateArmyRuntime->modelNodeRuntime,sourceArmyRuntime->modelNodeRuntime);
  iVar2 = extraout_ECX;
  iVar3 = extraout_EDX;
  if ((bool)in_CF) {
    for (; iVar3 != 0; iVar3 = iVar3 + -1) {
      piVar1 = (int *)sourceArmyRuntime[1].commandCoordinate0Q12;
      bVar4 = false;
      if ((piVar1 != (int *)0x0) &&
         (ArmyRuntime_TestPositionDistanceWithinCombinedRadiusCf
                    (*(UQ12 *)(iVar2 + 0x1a0),*(UQ12 *)(*piVar1 + 0x1a0),
                     candidateArmyRuntime->modelNodeRuntime,(void *)piVar1[1]),
         iVar2 = extraout_ECX_00, iVar3 = extraout_EDX_00, !bVar4)) {
        return;
      }
      sourceArmyRuntime = (ArmyRuntimeSlot *)&sourceArmyRuntime->commandCoordinate0Q12;
    }
  }
  return;
}

/* Address: 0x0051C040.
   Ownership: gameplay/army/runtime.
   Purpose: Releases an army runtime model instance, removes selection and global references, clears the identifier
   from faction technology tables, marks the runtime slot free, and refreshes the two catalog grids and selection-
   detail panel.
   Cross-module calls: ModelRuntimePool_DestroyHierarchyAndDetach [world/model/runtime],
   SelectionPlayerBlocks_RemovePointer [gameplay/selection/runtime], WorldRuntime_ForEachNodeInOwnerListD8
   [world/runtime/core], GameFactionRuntime_ClearRuntimeGroupMemberPointerFromAllFactionTables
   [gameplay/faction/runtime], UiCatalogGroup48_RebuildGrid [ui/ingame/technology], UiCatalogGroup42_RebuildGrid
   [ui/ingame/technology].
*/
void ArmyRuntime_DestroyInstanceAndRefreshUi
               (WorldRuntimeContext *worldRuntime,GameEntityRuntime *entityRuntime)

{
  ModelRuntimeSlot *modelRuntime;
  FrontendPlayerRuntimeBlockCount FVar1;
  FrontendPlayerRuntimeRecord *pFVar2;
  
  modelRuntime = (entityRuntime->common).ownership.definitionOrClassRecord;
  if (modelRuntime != (ModelRuntimeSlot *)0x0) {
    (entityRuntime->common).ownership.definitionOrClassRecord = (void *)0x0;
    ModelRuntimePool_DestroyHierarchyAndDetach(worldRuntime,modelRuntime);
  }
  SelectionPlayerBlocks_RemovePointer(entityRuntime);
  if (entityRuntime == (worldRuntime->selection).selectedEntity) {
    (worldRuntime->selection).selectedEntity = (GameEntityRuntime *)0x0;
  }
  WorldRuntime_ForEachNodeInOwnerListD8
            (entityRuntime,WorldRuntimeNode_ClearOwnedModelReferencesCallback,worldRuntime);
  FVar1 = g_FrontendPlayerRuntimeBlockCount;
  pFVar2 = g_FrontendPlayerRuntimeBlocks;
  do {
    if (entityRuntime ==
        (GameEntityRuntime *)
        g_SelectionPlayerRuntimeBlockPointers[pFVar2->playerRuntimeId]->
        primarySelectionEntityOffset8094) {
      g_SelectionPlayerRuntimeBlockPointers[pFVar2->playerRuntimeId]->
      primarySelectionEntityOffset8094 = 0;
    }
    pFVar2 = pFVar2 + 1;
    FVar1 = FVar1 - 1;
  } while (FVar1 != 0);
  GameFactionRuntime_ClearRuntimeGroupMemberPointerFromAllFactionTables(entityRuntime);
  (entityRuntime->common).ownership.modelNode = (ModelRuntimeNode *)0x0;
  UiCatalogGroup48_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
  UiCatalogGroup42_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
  InGameSelectionDetailPanel_Rebuild();
  return;
}

/* Address: 0x005246B0.
   Ownership: gameplay/army/runtime.
   Purpose: Tests the class-13 candidate relation and distance/proximity conditions used by the Group-A command
   handler; status is returned through CF.
   Cross-module calls: ModelLookupTable_ContainsPackedKeyCf [assets/model/definitions],
   ModelNodeRuntime_TransformLocalPointRegs [world/model/hierarchy], FixedMath_Length2 [core/math/fixed].
*/
void ArmyRuntime_TestClass13ProximityCandidateCf
               (ArmyRuntimeSlot *candidateArmyRuntime,ArmyRuntimeSlot *sourceArmyRuntime)

{
  uint uVar1;
  ModelRuntimeNode *modelNodeRuntime;
  ModelPackedPointRecord *localPointRecord;
  dword dVar2;
  undefined4 extraout_ECX;
  int extraout_ECX_00;
  int extraout_EDX;
  bool bVar3;
  undefined8 uVar4;
  ModelRuntimeNode *modelNode1;
  
  uVar1 = *(uint *)((int)sourceArmyRuntime->definitionOrAsset + 0x4c);
  bVar3 = uVar1 < 0xd;
  if (uVar1 == 0xd) {
    modelNodeRuntime = sourceArmyRuntime->modelNodeRuntime;
    modelNode1 = candidateArmyRuntime->modelNodeRuntime;
    localPointRecord =
         (ModelPackedPointRecord *)
         ModelLookupTable_ContainsPackedKeyCf(1,5,(modelNodeRuntime->modelPayload).modelResource);
    if (!bVar3) {
      uVar4 = ModelNodeRuntime_TransformLocalPointRegs
                        (extraout_ECX,localPointRecord,modelNodeRuntime);
      dVar2 = FixedMath_Length2(extraout_ECX_00 - (modelNode1->worldTransform).translation.y,
                                (int)uVar4 - (modelNode1->worldTransform).translation.x);
      if ((int)dVar2 <= extraout_EDX) {
        return;
      }
    }
  }
  return;
}

/* Address: 0x00526510.
   Ownership: gameplay/army/runtime.
   Purpose: Typed parameters: p3 soundAssetIndex→SoundAssetIndex_V343. Calling convention, complete VariableStorage
   serialization, function bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: GameFactionRuntime_TestCapabilityBitClearCf [gameplay/faction/runtime].
*/
void ArmyRuntime_TryPlayMappedTerrainSoundAtWorldPoint
               (FactionRuntimeIndex factionIndex,Q12 worldYQ12,Q12 worldXQ12,
               SoundAssetIndex soundAssetIndex,WorldRuntimeContext *worldContext)

{
  undefined4 *puVar1;
  FieldGridDimension FVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  bool bVar6;
  GameFactionCapabilityCfVolatileContinuityResult GVar7;
  FieldGridAsset *fieldGrid1;
  
  if ((((soundAssetIndex != 0) && (worldContext->dwordArray != (dword *)0x0)) &&
      (soundAssetIndex < worldContext->dwordArrayCount)) &&
     (puVar1 = (undefined4 *)worldContext->dwordArray[soundAssetIndex], puVar1 != (undefined4 *)0x0)
     ) {
    fieldGrid1 = worldContext->fieldGrid;
    uVar4 = (int)((ulonglong)((longlong)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
            (uint)((longlong)worldYQ12 * -0x20c8cc) >> 0x15;
    FVar2 = fieldGrid1->gridWidth;
    iVar3 = (int)((((int)((ulonglong)((longlong)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                   (uint)((longlong)worldXQ12 * 0x1c6e9c) >> 0x14) - uVar4) + 0x800) >> 0xc;
    if (((-1 < iVar3) && (iVar5 = (int)(uVar4 * 2 + 0x800) >> 0xc, -1 < iVar5)) &&
       ((iVar3 < (int)FVar2 && (iVar5 < (int)fieldGrid1->gridHeight)))) {
      uVar4 = worldContext->activeFactionRuntimeIndex;
      bVar6 = (uint)factionIndex < uVar4;
      if (factionIndex != uVar4) {
        GVar7 = GameFactionRuntime_TestCapabilityBitClearCf(uVar4,factionIndex);
        if (((bVar6) &&
            ((fieldGrid1->cells[FVar2 * iVar5 + iVar3].runtime58_6F[uVar4 + 0x18] & 0x10) != 0)) &&
           (0x10 < g_GameFactionRuntimeImage.records[GVar7.preservedEcxCallerValue].
                   relationTransitionTick)) {
          g_GameFactionRuntimeImage.records[GVar7.preservedEcxCallerValue].relationTransitionTick =
               0;
          (*g_SoundPlayOneShot)
                    (g_SoundEffectsGainQ15,g_SoundEffectsGainQ15,(DirectSoundVoiceSet *)*puVar1);
        }
      }
    }
  }
  return;
}

/* Address: 0x005271A0.
   Ownership: gameplay/army/runtime.
   Purpose: Handles army runtime spawner compute remaining linked asset metric.
   Cross-module calls: ArmyAssetRegistry_FindByIdCf [assets/army/catalog],
   ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf [assets/model/definitions].
*/
undefined8
ArmyRuntimeSpawner_ComputeRemainingLinkedAssetMetric
          (ArmyRuntimeLinkedChildMaskSlotView *armyRuntime)

{
  FactionRuntimeIndex factionIndex;
  undefined4 in_EAX;
  ArmyAssetRecordPrefix *pAVar1;
  ModelDefinitionRecordPrefix *pMVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  int iVar3;
  uint uVar4;
  uint extraout_EDX;
  int extraout_EDX_00;
  uint uVar5;
  ArmyRuntimeLinkedChildMaskSlotView *pAVar6;
  bool bVar7;
  
  uVar4 = 0;
  factionIndex = (armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex;
  uVar5 = 1;
  iVar3 = *(int *)((int)armyRuntime->definitionOrAsset + 0xc4);
  pAVar6 = armyRuntime;
  do {
    bVar7 = false;
    if (((armyRuntime->articulatedContact).linkedChildSlotMaskState.linkedChildSlotMask & uVar5) ==
        0) {
      pAVar1 = ArmyAssetRegistry_FindByIdCf(pAVar6->movementTarget0Q12);
      iVar3 = extraout_ECX;
      uVar4 = extraout_EDX;
      if (!bVar7) {
        pMVar2 = ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                           (factionIndex,pAVar1->rootNodeOffsetOrPointer);
        uVar4 = extraout_EDX_00 + pMVar2[0x20].flags;
        iVar3 = extraout_ECX_00;
      }
    }
    pAVar6 = (ArmyRuntimeLinkedChildMaskSlotView *)&pAVar6->modelNodeRuntime;
    uVar5 = uVar5 * 2;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  return CONCAT44(uVar4 >> 5,in_EAX);
}

/* Address: 0x00527230.
   Ownership: gameplay/army/runtime.
   Purpose: Projects the spawned runtime position through the field mask and plays its configured one-shot creation
   sound when the selected sound resource is available.
   Cross-module calls: TerrainGrid_TestProjectedCellMaskBits01Cf [world/terrain/grid],
   SpatialSound_PlayPositionedOneShot [audio/spatial/runtime].
*/
void ArmyRuntimeSpawner_PlayCreationSound
               (ArmyRuntimeSlot *armyRuntime,WorldRuntimeContext *worldRuntime)

{
  uint uVar1;
  GraphicsFixedVec3 *worldPosition;
  int iVar2;
  bool bVar3;
  undefined8 uVar4;
  
  uVar1 = *(uint *)((int)armyRuntime->definitionOrAsset + 0x270);
  if ((((uVar1 != 0) && (uVar1 < worldRuntime->dwordArrayCount)) &&
      (worldRuntime->dwordArray != (dword *)0x0)) &&
     (bVar3 = false, worldRuntime->dwordArray[uVar1] != 0)) {
    uVar4 = TerrainGrid_TestProjectedCellMaskBits01Cf
                      ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,
                       (armyRuntime->modelNodeRuntime->worldTransform).translation.x,worldRuntime);
    iVar2 = (int)((ulonglong)uVar4 >> 0x20);
    if (!bVar3) {
      SpatialSound_PlayPositionedOneShot
                (*(SpatialSoundMaximumDistanceQ12 *)(iVar2 + 0x7c),
                 *(SpatialSoundGainQ15 *)(iVar2 + 0x78),worldPosition,(DirectSoundVoiceSet **)uVar4)
      ;
    }
  }
  return;
}

/* Address: 0x005272B0.
   Ownership: gameplay/army/runtime.
   Purpose: Handles army runtime try spawn definition effect at world point.
   Cross-module calls: TerrainGrid_TestProjectedCellMaskBits01Cf [world/terrain/grid],
   SpatialSound_PlayPositionedOneShot [audio/spatial/runtime].
*/
void ArmyRuntime_TrySpawnDefinitionEffectAtWorldPoint
               (ArmyRuntimeSlot *armyRuntime,WorldRuntimeContext *worldContext)

{
  uint uVar1;
  GraphicsFixedVec3 *worldPosition;
  int iVar2;
  bool bVar3;
  undefined8 uVar4;
  
  uVar1 = *(uint *)((int)armyRuntime->definitionOrAsset + 0x26c);
  if ((((uVar1 != 0) && (uVar1 < worldContext->dwordArrayCount)) &&
      (worldContext->dwordArray != (dword *)0x0)) &&
     (bVar3 = false, worldContext->dwordArray[uVar1] != 0)) {
    uVar4 = TerrainGrid_TestProjectedCellMaskBits01Cf
                      ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,
                       (armyRuntime->modelNodeRuntime->worldTransform).translation.x,worldContext);
    iVar2 = (int)((ulonglong)uVar4 >> 0x20);
    if (!bVar3) {
      SpatialSound_PlayPositionedOneShot
                (*(SpatialSoundMaximumDistanceQ12 *)(iVar2 + 0x7c),
                 *(SpatialSoundGainQ15 *)(iVar2 + 0x78),worldPosition,(DirectSoundVoiceSet **)uVar4)
      ;
    }
  }
  return;
}

/* Address: 0x005273D0.
   Ownership: gameplay/army/runtime.
   Purpose: Compares squared XY distance between two position runtimes with the square of their combined Q12 radii
   and returns the outside/inside status through CF.
*/
void ArmyRuntime_TestPositionDistanceWithinCombinedRadiusCf
               (UQ12 candidateRadiusQ12,UQ12 sourceRadiusQ12,void *candidatePositionRuntime,
               void *sourcePositionRuntime)

{
  int currentAxisDeltaQ12;
  int iVar1;
  longlong remainingRadiusSquaredAfterXQ24;
  longlong yDistanceSquaredQ24;
  
  currentAxisDeltaQ12 =
       *(int *)((int)sourcePositionRuntime + 0x94) - *(int *)((int)candidatePositionRuntime + 0x94);
  remainingRadiusSquaredAfterXQ24 =
       (longlong)(int)(sourceRadiusQ12 + candidateRadiusQ12) *
       (longlong)(int)(sourceRadiusQ12 + candidateRadiusQ12) -
       (longlong)currentAxisDeltaQ12 * (longlong)currentAxisDeltaQ12;
  if ((-1 < remainingRadiusSquaredAfterXQ24) &&
     (iVar1 = *(int *)((int)sourcePositionRuntime + 0x98) -
              *(int *)((int)candidatePositionRuntime + 0x98),
     yDistanceSquaredQ24 = (longlong)iVar1 * (longlong)iVar1,
     -1 < (int)(((int)((ulonglong)remainingRadiusSquaredAfterXQ24 >> 0x20) -
                (int)((ulonglong)yDistanceSquaredQ24 >> 0x20)) -
               (uint)((uint)remainingRadiusSquaredAfterXQ24 < (uint)yDistanceSquaredQ24)))) {
    return;
  }
  return;
}

/* Address: 0x005297D0.
   Ownership: gameplay/army/runtime.
   Purpose: Finds an eligible nearby candidate, resolves the requested model-point ordinal, transforms that point,
   and creates the indexed effect at the resulting world position. The stock corpus contains 140 unique
   EffectDefinition ids; serialized ids remain distinct from relocated EffectDefinition pointers and consumer-
   specific union facets. It is separate from world-unit radii, angles, grid indices, and serialized PCK
   identities. Typed parameters: p0 effectFlags→EffectCreationFlags_V304. Nearby but non-identical semantic domains
   were explicitly deferred.
   Cross-module calls: AiCombatTarget_EvaluateCandidateScore [gameplay/ai/combat], FixedMath_Length2
   [core/math/fixed], ModelNodeRuntime_RebuildTransformsFromRoot [world/model/hierarchy],
   ModelNodeRuntime_TransformLocalPointRegs [world/model/hierarchy], ShotRuntimePool_CreateProjectileFromDefinition
   [world/shots/runtime].
*/
void ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
               (EffectCreationFlagBits effectFlags,Q12 worldZQ12,Q12 worldYQ12,Q12 worldXQ12,
               ModelAttachmentOrdinal modelPointOrdinal,
               PckEffectDefinitionIdCatalog effectDefinitionId,void *sourceRuntime,
               void *modelPointTable,WorldRuntimeContext *worldContext)

{
  ArmyRuntimeSlot *sourceArmyRuntime;
  uint uVar1;
  dword dVar2;
  ArmyRuntimeSlot *in_ECX;
  ArmyRuntimeSlot *extraout_ECX;
  ArmyRuntimeSlot *extraout_ECX_00;
  undefined4 extraout_ECX_01;
  int launchWorldYQ12;
  undefined4 extraout_ECX_02;
  undefined4 uVar3;
  uint currentBestScore;
  uint extraout_EDX;
  Q12 extraout_EDX_00;
  Q12 QVar4;
  int y;
  int iVar5;
  WorldRuntimeNode *worldNode1;
  ModelPackedPointRecord *localPointRecord;
  undefined8 uVar6;
  WorldRuntimeNode *worldNode2;
  uint uStack_20;
  
  worldNode2 = (WorldRuntimeNode *)0x0;
  worldNode1 = worldContext->ownerListHead;
  uStack_20 = 0xffffffff;
  currentBestScore = 0;
  sourceArmyRuntime = *(ArmyRuntimeSlot **)(*(int *)((int)sourceRuntime + 0x48) + 8);
  do {
    if ((((worldNode1[2].common.nextNode == (WorldRuntimeNode *)0x0) &&
         (worldNode1 != sourceRuntime)) &&
        (in_ECX = *(ArmyRuntimeSlot **)((int)worldNode1->runtimePayload + 8),
        (*(uint *)((int)worldNode1->runtimePayload + 0xec) & 8) == 0)) &&
       ((in_ECX->factionIndex != 0 && (in_ECX->factionIndex != sourceArmyRuntime->factionIndex)))) {
      sourceArmyRuntime->runtimeState4C = *(dword *)(*(int *)in_ECX->definitionOrAsset + 0xdc);
      uVar1 = AiCombatTarget_EvaluateCandidateScore
                        (currentBestScore,1,0xffffffff,0xffffffff,in_ECX,sourceArmyRuntime);
      in_ECX = extraout_ECX;
      currentBestScore = extraout_EDX;
      if (extraout_EDX < uVar1) {
        currentBestScore = uVar1;
        worldNode2 = worldNode1;
      }
    }
    worldNode1 = (worldNode1->common).nextNode;
  } while (worldNode1 != (WorldRuntimeNode *)0x0);
  QVar4 = 0;
  if (worldNode2 != (WorldRuntimeNode *)0x0) {
    y = worldNode2[1].classPayload.model.meshGroupMask - worldXQ12;
    iVar5 = *(int *)(*(int *)worldNode2->runtimePayload + 0xdc);
    dVar2 = FixedMath_Length2((int)worldNode2[1].runtimePayload - worldYQ12,y);
    in_ECX = extraout_ECX_00;
    QVar4 = extraout_EDX_00;
    if ((int)(dVar2 - iVar5) < 0x1001) {
      uStack_20 = 0;
      worldXQ12 = y + worldXQ12;
      in_ECX = (ArmyRuntimeSlot *)((int)&extraout_ECX_00->definitionOrAsset + worldYQ12);
      worldZQ12 = extraout_EDX_00;
      worldYQ12 = (Q12)in_ECX;
    }
  }
  ModelNodeRuntime_RebuildTransformsFromRoot(in_ECX,QVar4,sourceRuntime);
  iVar5 = *(int *)((int)modelPointTable + 0x30);
  localPointRecord = (ModelPackedPointRecord *)(iVar5 + *(int *)(iVar5 + 0xe4));
  uVar3 = extraout_ECX_01;
  for (iVar5 = *(int *)(iVar5 + 0xe8); iVar5 != 0; iVar5 = iVar5 + -1) {
    if (localPointRecord->packedLookupKey == (modelPointOrdinal << 4 | 2)) {
      uVar6 = ModelNodeRuntime_TransformLocalPointRegs(uVar3,localPointRecord,sourceRuntime);
      ShotRuntimePool_CreateProjectileFromDefinition
                (effectFlags,*(ArmyRuntimeSlot **)(*(int *)((int)sourceRuntime + 0x48) + 8),
                 worldZQ12,
                 (launchWorldYQ12 - *(int *)((int)sourceRuntime + 0x98) & uStack_20) + worldYQ12,
                 ((int)uVar6 - *(int *)((int)sourceRuntime + 0x94) & uStack_20) + worldXQ12,
                 (Q12)((ulonglong)uVar6 >> 0x20),launchWorldYQ12,(int)uVar6,
                 (ShotDefinition *)effectDefinitionId,worldContext);
      uVar3 = extraout_ECX_02;
    }
    localPointRecord = localPointRecord + 1;
  }
  return;
}

/* Address: 0x00529980.
   Ownership: gameplay/army/runtime.
   Purpose: Resets the shared attachment progress field, processes up to eight ready attachment channels by
   matching typed model anchors and emitting configured runtime effects, then clears the verified child timer and
   state fields. Two stack arguments are authoritative from RET 0x08; prior register parameters and return were
   synthetic. Role: Processes up to eight ready channels and emits configured effects from packed attachment keys.
   Inputs: Army channel timers, model attachment table and per-channel effect definitions. Outputs: EffectRuntime
   instances plus cleared channel/child state.
   Cross-module calls: ModelNodeRuntime_TransformLocalPointRegs [world/model/hierarchy],
   EffectRuntimePool_CreateInstanceFromDefinitionCf [world/effects/runtime].
*/
void ArmyRuntime_ProcessReadyAttachmentChannels
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  uint uVar1;
  Q12 worldXQ12;
  Q12 worldXQ12_00;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  ModelPackedPointRecord *pMVar5;
  undefined8 uVar6;
  ModelRuntimeNode *modelNode1;
  
  uVar1 = 0;
  iVar4 = *(int *)(*(int *)((int)armyRuntime->definitionOrAsset + 100) + 0x30);
  armyRuntime->actionVector2Q12 = 0;
  do {
    if ((&armyRuntime->runtimeState40)[uVar1] == 0) {
      iVar3 = *(int *)(iVar4 + 0xe8);
      pMVar5 = (ModelPackedPointRecord *)(iVar4 + *(int *)(iVar4 + 0xe4));
      if (iVar3 != 0) {
        do {
          if (uVar1 * 0x10 + 3 == pMVar5->packedLookupKey) {
            uVar6 = ModelNodeRuntime_TransformLocalPointRegs
                              (uVar1,pMVar5,armyRuntime->modelNodeRuntime);
            uVar2 = (undefined4)((ulonglong)uVar6 >> 0x20);
            modelNode1 = armyRuntime->modelNodeRuntime;
            EffectRuntimePool_CreateInstanceFromDefinitionCf
                      (worldXQ12,uVar2,EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY,
                       armyRuntime->actionVector1Q12,(modelNode1->modelPayload).worldRotationAngle2,
                       (modelNode1->modelPayload).worldRotationAngle1,
                       (modelNode1->modelPayload).worldRotationAngle0,uVar2,worldXQ12,(Q12)uVar6,
                       *(EffectDefinition **)
                        ((int)armyRuntime->definitionOrAsset + uVar1 * 8 + 0x80),worldRuntime);
          }
          pMVar5 = pMVar5 + 1;
          iVar3 = iVar3 + -1;
        } while (iVar3 != 0);
      }
    }
    uVar1 = uVar1 + 1;
  } while (uVar1 < 8);
  iVar4 = *(int *)((int)armyRuntime->definitionOrAsset + 100);
  uVar1 = 0;
  if ((*(int *)(iVar4 + 0x14) != 0) &&
     (iVar4 = *(int *)(iVar4 + 0x18), iVar3 = *(int *)(iVar4 + 0x30),
     (*(uint *)(iVar4 + 4) & 0xf) == 0)) {
    do {
      if ((&armyRuntime->runtimeState40)[uVar1] == 0) {
        iVar4 = *(int *)(iVar3 + 0xe8);
        pMVar5 = (ModelPackedPointRecord *)(iVar3 + *(int *)(iVar3 + 0xe4));
        if (iVar4 != 0) {
          do {
            if (((uVar1 * 0x10 + 3 == pMVar5->packedLookupKey) &&
                (armyRuntime->modelNodeRuntime->childCount != 0)) &&
               (modelNode1 = armyRuntime->modelNodeRuntime->childNodes[0],
               modelNode1 != (ModelRuntimeNode *)0x0)) {
              uVar6 = ModelNodeRuntime_TransformLocalPointRegs(uVar1,pMVar5,modelNode1);
              uVar2 = (undefined4)((ulonglong)uVar6 >> 0x20);
              EffectRuntimePool_CreateInstanceFromDefinitionCf
                        (worldXQ12_00,uVar2,EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY,
                         armyRuntime->actionVector1Q12,0,0x4000,0,uVar2,worldXQ12_00,(Q12)uVar6,
                         *(EffectDefinition **)
                          ((int)armyRuntime->definitionOrAsset + uVar1 * 8 + 0x80),worldRuntime);
            }
            pMVar5 = pMVar5 + 1;
            iVar4 = iVar4 + -1;
          } while (iVar4 != 0);
        }
      }
      uVar1 = uVar1 + 1;
    } while (uVar1 < 8);
  }
  for (iVar4 = armyRuntime->factionIndex; iVar4 != 0; iVar4 = iVar4 + -1) {
    iVar3 = armyRuntime[1].commandCoordinate0Q12;
    if (iVar3 != 0) {
      *(undefined4 *)(iVar3 + 0x3c) = 0;
      *(undefined4 *)(iVar3 + 0x38) = 0;
    }
    armyRuntime = (ArmyRuntimeSlot *)&armyRuntime->commandCoordinate0Q12;
  }
  return;
}

/* Address: 0x0052A760.
   Ownership: gameplay/army/runtime.
   Purpose: Invokes the class-indexed method-D callback for the current runtime and recursively dispatches the same
   callback through its child hierarchy. Class method-D partition slots 24-47 receive (worldRuntime, armyRuntime).
*/
void ArmyRuntimeHierarchy_DispatchClassMethodDRecursive
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  int extraout_ECX;
  int extraout_ECX_00;
  int iVar1;
  
  (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classMethodD
    [*(int *)((int)armyRuntime->definitionOrAsset + 0x4c)])(worldRuntime,armyRuntime);
  for (iVar1 = extraout_ECX; iVar1 != 0; iVar1 = iVar1 + -1) {
    if ((ArmyRuntimeSlot *)armyRuntime[1].commandCoordinate0Q12 != (ArmyRuntimeSlot *)0x0) {
      ArmyRuntimeHierarchy_DispatchClassMethodDRecursive
                (worldRuntime,(ArmyRuntimeSlot *)armyRuntime[1].commandCoordinate0Q12);
      iVar1 = extraout_ECX_00;
    }
    armyRuntime = (ArmyRuntimeSlot *)&armyRuntime->commandCoordinate0Q12;
  }
  return;
}

/* Address: 0x0051C350.
   Ownership: gameplay/army/runtime.
   Purpose: Clears the verified derived selection and hierarchy metric fields, then rebuilds them recursively from
   the attached model runtime hierarchy when present.
   Cross-module calls: ModelRuntimeHierarchy_AccumulateDerivedSelectionMetrics [world/model/hierarchy].
*/
void ArmyRuntime_RebuildDerivedSelectionMetrics(ArmyRuntimeSlot *armyRuntime)

{
  byte *pbVar1;
  int iVar2;
  
  armyRuntime->runtimeState90 = 0;
  armyRuntime->runtimeState44 = 0;
  armyRuntime->runtimeState48 = 0;
  iVar2 = 7;
  armyRuntime->runtimeState4C = 0;
  do {
    pbVar1 = armyRuntime->reservedF8_FF + iVar2 * 4 + 8;
    pbVar1[0] = 0;
    pbVar1[1] = 0;
    pbVar1[2] = 0;
    pbVar1[3] = 0;
    iVar2 = iVar2 + -1;
  } while (-1 < iVar2);
  if (armyRuntime->definitionOrAsset != (int *)0x0) {
    ModelRuntimeHierarchy_AccumulateDerivedSelectionMetrics(armyRuntime->definitionOrAsset);
  }
  return;
}

/* Address: 0x0051DBA0.
   Ownership: gameplay/army/runtime.
   Purpose: The current implementation preserves the input state, clears carry, and therefore permits every tested
   point.
*/
undefined8 __fastcall
ArmyRuntime_TestWorldPointAllowedDefaultCf(undefined4 param_1,undefined4 param_2)

{
  undefined4 in_EAX;
  
  return CONCAT44(param_2,in_EAX);
}

/* Address: 0x0051B8F0.
   Ownership: gameplay/army/runtime.
   Purpose: Allocates a free 0x120-byte army runtime slot, resolves the requested asset and faction graphics,
   initializes the runtime state, creates and links the model hierarchy, and rebuilds transforms, bounds, and tint
   before returning status through carry. Role: Creates a world army/placeable instance from an asset record and
   its model graph. Inputs: World context, army definition, model selection/technology/faction state and world
   transform. Outputs: ArmyRuntimeSlot, ModelRuntimeSlot/Node hierarchy, world links, occupancy, tint and derived
   radius/depth. Edges: ModelRuntimePool_CreateInstanceByDefinitionIdCf -> linked-child recursion ->
   transform/occupancy initialization.
   Local calls: ArmyRuntime_InitializeTerrainOccupancyFlags, ArmyRuntime_RebuildDerivedSelectionMetrics.
   Cross-module calls: ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf [assets/model/definitions],
   ModelDefinitionHierarchy_UnlockSelectedLinkedTechnology [assets/model/definitions],
   ModelDefinition_SelectFactionUnlockedLinkedIdCf [assets/model/definitions],
   ModelRuntimePool_CreateInstanceByDefinitionIdCf [world/model/runtime],
   ModelNodeRuntime_InstantiateLinkedChildrenRecursiveCf [world/model/hierarchy],
   WorldRuntime_LinkNodeIntoOwnerListD8 [world/runtime/core].
*/
ArmyRuntimeSlot *
ArmyRuntime_CreateInstanceFromAssetCf
          (WorldObjectAllocationFlags creationFlags,AngleTurn32 orientationAngle,Q12 worldXQ12,
          Q12 worldYQ12,FactionRuntimeIndex factionIndex,PckArmyAssetIdCatalog armyAssetId,
          WorldRuntimeContext *worldRuntime)

{
  GraphicsTextureSet *textureSet;
  GraphicsPaletteAsset *paletteAsset;
  dword dVar1;
  dword dVar2;
  GameEntityRuntime *pGVar3;
  ModelRuntimeNode *modelNodeRuntime;
  ModelDefinitionRecordPrefix *modelDefinition1;
  PckModelDefinitionIdCatalog modelDefinitionId;
  ArmyRuntimeSlot *armySlot2;
  uint uVar4;
  uint extraout_ECX;
  int extraout_ECX_00;
  int iVar5;
  undefined4 extraout_ECX_01;
  int extraout_ECX_02;
  int iVar6;
  int extraout_EDX;
  undefined4 extraout_EDX_00;
  WorldRuntimeContext *worldRuntime_00;
  ArmyAssetRecordPrefix **ppAVar7;
  ArmyRuntimeSlot *armyRuntime;
  undefined1 uVar8;
  undefined8 uVar9;
  ArmyAssetRuntimeSemanticView80 *definitionNode;
  ArmyRuntimeSlot *armySlot1;
  
  armySlot2 = (ArmyRuntimeSlot *)0x400;
  armyRuntime = g_ArmyRuntimeSlots;
  armySlot1 = g_ArmyRuntimeSlots;
  while( true ) {
    if (armySlot1 == (ArmyRuntimeSlot *)0x0) {
      return (ArmyRuntimeSlot *)0x14;
    }
    if (armyRuntime->modelNodeRuntime == (ModelRuntimeNode *)0x0) break;
    armyRuntime = armyRuntime + 1;
    armySlot2 = (ArmyRuntimeSlot *)((int)&armySlot2[-1].selectionMetric5 + 3);
    armySlot1 = armySlot2;
  }
  ppAVar7 = g_ArmyAssetRecordRegistry;
  iVar6 = 0x300;
  while ((definitionNode = (ArmyAssetRuntimeSemanticView80 *)*ppAVar7,
         definitionNode == (ArmyAssetRuntimeSemanticView80 *)0x0 ||
         (definitionNode->registryId != armyAssetId))) {
    ppAVar7 = ppAVar7 + 1;
    iVar6 = iVar6 + -1;
    if (iVar6 == 0) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,armyAssetId,g_PackageLastErrorPath);
      return (ArmyRuntimeSlot *)0x41;
    }
  }
  armyRuntime->armyAssetId = armyAssetId;
  uVar4 = 8;
  if (((creationFlags & 2) != 0) && (factionIndex == worldRuntime->activeFactionRuntimeIndex)) {
    modelDefinition1 =
         ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                   (factionIndex,definitionNode->rootNodeOffsetOrPointer);
    modelDefinition1[0x24].byteSize = modelDefinition1[0x24].byteSize + 1;
    uVar4 = extraout_ECX;
    iVar6 = extraout_EDX;
  }
  if (uVar4 <= (uint)factionIndex) {
    factionIndex = uVar4 - 1;
  }
  armyRuntime->factionIndex = factionIndex;
  uVar8 = 0;
  iVar5 = factionIndex;
  if ((creationFlags & 4) != 0) {
    ModelDefinitionHierarchy_UnlockSelectedLinkedTechnology
              (factionIndex,iVar6,factionIndex,(ModelDefinitionHierarchyNodeAddress32)definitionNode
              );
    iVar5 = extraout_ECX_00;
  }
  textureSet = g_ArmyGraphicsBindings[iVar5].textureSet;
  paletteAsset = g_ArmyGraphicsBindings[iVar5].paletteAsset;
  dVar1 = definitionNode->definitionClassValue74;
  dVar2 = definitionNode->definitionClassValue78;
  armyRuntime->definitionClassValue80 = definitionNode->definitionClassValue70;
  armyRuntime->definitionClassValue84 = dVar1;
  armyRuntime->definitionClassValue88 = dVar2;
  pGVar3 = definitionNode->linkedRuntimeOrRecord10;
  armyRuntime->runtimeState90 = 0;
  armyRuntime->runtimeState44 = 0;
  armyRuntime->runtimeState48 = 0;
  armyRuntime->runtimeState94 = 0;
  dVar1 = definitionNode->rootNodeOffsetOrPointer;
  (armyRuntime->articulatedContact).fallbackPosition0Q12 = worldYQ12;
  (armyRuntime->articulatedContact).fallbackPosition1Q12 = worldXQ12;
  armyRuntime->linkedEntityRuntime = pGVar3;
  armyRuntime->definitionOrAsset = (void *)0x0;
  armyRuntime->runtimeState8C = 0;
  modelDefinitionId = ModelDefinition_SelectFactionUnlockedLinkedIdCf(factionIndex,dVar1);
  uVar9 = ModelRuntimePool_CreateInstanceByDefinitionIdCf
                    (paletteAsset,textureSet,armyRuntime,modelDefinitionId,worldRuntime);
  armySlot2 = (ArmyRuntimeSlot *)uVar9;
  if ((bool)uVar8) {
    return armySlot2;
  }
  modelNodeRuntime = armySlot2->modelNodeRuntime;
  armyRuntime->definitionOrAsset = armySlot2;
  armyRuntime->modelNodeRuntime = modelNodeRuntime;
  (modelNodeRuntime->worldTransform).translation.x = worldYQ12;
  (modelNodeRuntime->worldTransform).translation.y = worldXQ12;
  (modelNodeRuntime->worldTransform).translation.z = 0;
  armyRuntime->runtimeStateA4 = 0;
  armyRuntime->fallbackWorldYQ12 = worldYQ12;
  armyRuntime->fallbackWorldXQ12 = worldXQ12;
  armyRuntime->movementTarget0Q12 = worldYQ12;
  armyRuntime->movementTarget1Q12 = worldXQ12;
  (modelNodeRuntime->modelPayload).worldRotationAngle0 = 0;
  (modelNodeRuntime->modelPayload).worldRotationAngle1 = 0x4000;
  (modelNodeRuntime->modelPayload).worldRotationAngle2 = orientationAngle;
  armyRuntime->commandTargetArmyRuntime = (ArmyRuntimeSlot *)0x0;
  armyRuntime->runtimeState98 = 0;
  armyRuntime->commandCoordinate0Q12 = 0;
  armyRuntime->commandCoordinate1Q12 = 0;
  armyRuntime->commandCoordinate2Q12 = 0;
  armyRuntime->commandModeFlags = 0;
  armyRuntime->commandGeneration = 0;
  (armyRuntime->articulatedContact).fallbackPosition0Q12 = worldYQ12;
  (armyRuntime->articulatedContact).fallbackPosition1Q12 = worldXQ12;
  (armyRuntime->linkedChildOverloadedState).primaryCoordinateCommandOrHistory.coordinateOrTargetQ12
       = worldYQ12;
  (armyRuntime->linkedChildOverloadedState).secondaryCoordinateCommandOrHistory.
  coordinateOrTargetQ12 = worldXQ12;
  armyRuntime->movementPosition0Q12 = worldYQ12;
  armyRuntime->movementPosition1Q12 = worldXQ12;
  armyRuntime->movementStateFlags = 0;
  armyRuntime->actionVector1Q12 = 0;
  armyRuntime->terrainOccupancyMask0 = 0;
  armyRuntime->terrainOccupancyMask1 = 0;
  armyRuntime->runtimeState40 = 0;
  armySlot2 = (ArmyRuntimeSlot *)
              ModelNodeRuntime_InstantiateLinkedChildrenRecursiveCf
                        (factionIndex,paletteAsset,textureSet,armyRuntime->definitionOrAsset,dVar1,
                         worldRuntime);
  if ((bool)uVar8) {
    return armySlot2;
  }
  WorldRuntime_LinkNodeIntoOwnerListD8((WorldRuntimeNode *)modelNodeRuntime);
  uVar9 = ModelNodeRuntime_RecomputeSubtreeBoundingRadius(modelNodeRuntime);
  (*g_ArmyPlacementContactKindDispatchTable.callbacks
    [*(int *)(*(int *)armyRuntime->definitionOrAsset + 0x278)])
            (*(Q12 *)(*(int *)armyRuntime->definitionOrAsset + 0x54),
             (modelNodeRuntime->worldTransform).translation.y,
             (modelNodeRuntime->worldTransform).translation.x,modelNodeRuntime,
             (WorldRuntimeContext *)((ulonglong)uVar9 >> 0x20));
  uVar9 = ModelNodeRuntime_RebuildTransformsFromRoot
                    (extraout_ECX_01,extraout_EDX_00,modelNodeRuntime);
  armyRuntime->depthBinClass =
       *(ModelRuntimeClassId *)
        (&g_ArmyRuntimeDepthBinClassByModelClass + *(int *)(extraout_ECX_02 + 0x4c) * 4);
  ModelNodeRuntime_UpdateDepthBinMasks
            (extraout_ECX_02,(int)((ulonglong)uVar9 >> 0x20),
             *(DepthIntervalRadius32 *)(extraout_ECX_02 + 0xdc),modelNodeRuntime);
  ArmyRuntime_InitializeTerrainOccupancyFlags(worldRuntime_00,armyRuntime);
  UiModelControl_RefreshStateTint(modelNodeRuntime);
  ArmyRuntime_RebuildDerivedSelectionMetrics(armyRuntime);
  return armyRuntime;
}

/* Address: 0x0051D0B0.
   Ownership: gameplay/army/runtime.
   Purpose: Classifies the army runtime position against terrain occupancy, stores the resulting masks, resolves
   class-state flags, and applies the verified model-definition flag override.
   Cross-module calls: TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint [world/terrain/occupancy],
   TerrainOccupancyMask_ResolveRuntimeClassFlags [world/terrain/occupancy].
*/
void ArmyRuntime_InitializeTerrainOccupancyFlags
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  uint occupancyRuntimeFlags;
  int extraout_ECX;
  FieldGridRegionMask extraout_ECX_00;
  FieldGridRegionMask extraout_EDX;
  ulonglong neighborhoodClassificationPair;
  ModelRuntimeNode *modelNode;
  void *definition;
  
  neighborhoodClassificationPair =
       TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                 (*(undefined4 *)(*(int *)armyRuntime->definitionOrAsset + 0xdc),
                  (armyRuntime->modelNodeRuntime->worldTransform).translation.y,
                  (armyRuntime->modelNodeRuntime->worldTransform).translation.x,
                  worldRuntime->fieldGrid);
  *(int *)(extraout_ECX + 0x50) = (int)(neighborhoodClassificationPair >> 0x20);
  modelNode = armyRuntime->modelNodeRuntime;
  occupancyRuntimeFlags =
       TerrainOccupancyMask_ResolveRuntimeClassFlags
                 (modelNode->runtimeFlags,armyRuntime->terrainOccupancyMask1,
                  armyRuntime->terrainOccupancyMask0,(char)worldRuntime->activeFactionRuntimeIndex);
  modelNode->runtimeFlags = modelNode->runtimeFlags & 0xffffeff3;
  armyRuntime->terrainOccupancyMask0 = extraout_ECX_00;
  armyRuntime->terrainOccupancyMask1 = extraout_EDX;
  definition = armyRuntime->definitionOrAsset;
  modelNode->runtimeFlags = modelNode->runtimeFlags | occupancyRuntimeFlags;
  if ((*(uint *)((int)definition + 0xec) & 0x200) != 0) {
    modelNode->runtimeFlags = modelNode->runtimeFlags | 0x1000;
  }
  return;
}

/* Address: 0x00527E70.
   Ownership: gameplay/army/runtime.
   Purpose: Updates the verified animated model subnodes, reverses the bounded oscillating channel at its limits,
   rebuilds transforms, and refreshes damage-threshold effects. Runtime-update partition slots 0-23 receive
   (worldRuntime, armyRuntime). PART anim rate source (viewer-research-backlog section 9): child[0]/[1] spin
   localRotationAngle2 += def[+0x10]/ def[+0x1C] x stepTicks; child[2] bob translation by def[+0x14] clamped
   [def+0x24, def+0x28]; clock = g_InGameSimulationStepTicks (not look phase). Replaces the viewer's VH spin/bob
   constants. Role: Advances two spinning child nodes and one bounded Z-bobbing child, then rebuilds transforms.
   Local calls: ArmyRuntime_RebuildDerivedSelectionMetrics.
   Cross-module calls: ModelNodeRuntime_RebuildTransformsFromRoot [world/model/hierarchy],
   ArmyRuntime_EmitDamageThresholdEffect [gameplay/army/combat].
*/
void ArmyRuntime_UpdateAnimatedModelSubnodes
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ModelRuntimeNode *modelNodeRuntime;
  Q12 updatedChildTranslationZQ12;
  int in_ECX;
  int extraout_ECX;
  void *armyDefinitionOrAsset;
  ModelRuntimeNode *animatedChildNode;
  int childTranslationZQ12;
  ModelRuntimeNode *modelNode1;
  
  if ((armyRuntime->runtimeFlags & 1) == 0) {
    if ((armyRuntime->runtimeFlags & 4) != 0) {
      armyRuntime->runtimeFlags = armyRuntime->runtimeFlags ^ 4;
      ArmyRuntime_RebuildDerivedSelectionMetrics
                ((ArmyRuntimeSlot *)armyRuntime->linkedEntityRuntime);
      in_ECX = extraout_ECX;
    }
    modelNodeRuntime = armyRuntime->modelNodeRuntime;
    armyDefinitionOrAsset = armyRuntime->definitionOrAsset;
    animatedChildNode = modelNodeRuntime->childNodes[0];
    if (modelNodeRuntime->childCount != 0) {
      if (animatedChildNode != (ModelRuntimeNode *)0x0) {
        (animatedChildNode->modelPayload).localRotationAngle2 =
             *(int *)((int)armyDefinitionOrAsset + 0x10) * g_InGameSimulationStepTicks +
             (animatedChildNode->modelPayload).localRotationAngle2 & 0xffff;
        animatedChildNode->runtimeFlags = animatedChildNode->runtimeFlags | 1;
      }
      modelNode1 = modelNodeRuntime->childNodes[1];
      if (1 < modelNodeRuntime->childCount) {
        if (modelNode1 != (ModelRuntimeNode *)0x0) {
          (modelNode1->modelPayload).localRotationAngle2 =
               *(int *)((int)armyDefinitionOrAsset + 0x1c) * g_InGameSimulationStepTicks +
               (modelNode1->modelPayload).localRotationAngle2 & 0xffff;
          modelNode1->runtimeFlags = modelNode1->runtimeFlags | 1;
        }
        modelNode1 = modelNodeRuntime->childNodes[2];
        if ((2 < modelNodeRuntime->childCount) && (modelNode1 != (ModelRuntimeNode *)0x0)) {
          childTranslationZQ12 = (modelNode1->modelPayload).localTranslationZQ12;
          in_ECX = *(int *)((int)armyDefinitionOrAsset + 0x14) * g_InGameSimulationStepTicks;
          if ((armyRuntime->runtimeFlags & 2) == 0) {
            updatedChildTranslationZQ12 = childTranslationZQ12 - in_ECX;
            if (updatedChildTranslationZQ12 < *(int *)((int)armyDefinitionOrAsset + 0x24)) {
              updatedChildTranslationZQ12 = *(Q12 *)((int)armyDefinitionOrAsset + 0x24);
              armyRuntime->runtimeFlags = armyRuntime->runtimeFlags ^ 2;
            }
          }
          else {
            updatedChildTranslationZQ12 = childTranslationZQ12 + in_ECX;
            if (*(int *)((int)armyDefinitionOrAsset + 0x28) < updatedChildTranslationZQ12) {
              updatedChildTranslationZQ12 = *(Q12 *)((int)armyDefinitionOrAsset + 0x28);
              armyRuntime->runtimeFlags = armyRuntime->runtimeFlags ^ 2;
            }
          }
          (modelNode1->modelPayload).localTranslationZQ12 = updatedChildTranslationZQ12;
          modelNode1->runtimeFlags = modelNode1->runtimeFlags | 1;
        }
      }
    }
    ModelNodeRuntime_RebuildTransformsFromRoot(in_ECX,armyDefinitionOrAsset,modelNodeRuntime);
  }
  else if ((armyRuntime->runtimeFlags & 4) == 0) {
    armyRuntime->runtimeFlags = armyRuntime->runtimeFlags ^ 4;
    ArmyRuntime_RebuildDerivedSelectionMetrics((ArmyRuntimeSlot *)armyRuntime->linkedEntityRuntime);
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,armyRuntime);
  return;
}

/* Address: 0x00527C00.
   Ownership: gameplay/army/runtime.
   Purpose: Advances the two verified emitter timers, creates scheduled shots, chooses model attachment points,
   selects land or water effects, and emits the resulting runtime effect instances. Runtime-update partition slots
   0-23 receive (worldRuntime, armyRuntime). Role: Advances serialized timers and emits scheduled shots/effects
   from model attachment points. Inputs: Army definition timed fields, model hierarchy, SPR attachment records,
   water/terrain context. Outputs: ShotRuntime and/or EffectRuntime instances at transformed attachment positions.
   Cross-module calls: FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed],
   ShotRuntimePool_CreateProjectileFromDefinition [world/shots/runtime], ModelLookupTable_ContainsPackedKeyCf
   [assets/model/definitions], ModelNodeRuntime_TransformLocalPointRegs [world/model/hierarchy],
   FieldGrid_GetNearestWaterDelta [world/terrain/grid], EffectRuntimePool_CreateInstanceFromDefinitionCf
   [world/effects/runtime].
*/
void ArmyRuntime_UpdateTimedShotAndEffectEmitters
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  byte *pbVar1;
  void *pvVar2;
  dword dVar3;
  uint uVar4;
  ModelPackedPointRecord *localPointRecord;
  Q12 worldX;
  sdword sVar5;
  int extraout_ECX;
  int iVar6;
  undefined4 extraout_ECX_00;
  Q12 extraout_ECX_01;
  Q12 worldY;
  uint extraout_ECX_02;
  uint extraout_ECX_03;
  int iVar7;
  undefined4 extraout_EDX;
  undefined4 extraout_EDX_00;
  uint *puVar8;
  AngleTurn32 extraout_EDX_01;
  AngleTurn32 AVar9;
  uint uVar10;
  ModelRuntimeNode *modelNode1;
  EffectDefinition *effectDefinition1;
  bool bVar11;
  FixedDirectionXZEdxEax8 FVar12;
  undefined8 uVar13;
  AngleTurn32 orientationAngle0;
  AngleTurn32 orientationAngle1;
  AngleTurn32 orientationAngle2;
  GraphicsWorldCoordinateQ12 launchWorldZQ12;
  Q12 worldZQ12;
  GraphicsWorldCoordinateQ12 launchWorldYQ12;
  GraphicsWorldCoordinateQ12 launchWorldXQ12;
  ShotDefinition *shotDefinition;
  EffectDefinition *effectDefinition;
  WorldRuntimeContext *worldContext1;
  
  pvVar2 = armyRuntime->definitionOrAsset;
  pbVar1 = armyRuntime->reservedE0_EB + 4;
  bVar11 = SBORROW4(*(int *)pbVar1,g_InGameSimulationStepTicks);
  *(InGameSimulationStepBatchTicks *)pbVar1 = *(int *)pbVar1 - g_InGameSimulationStepTicks;
  if ((*(int *)pbVar1 == 0 || bVar11 != *(int *)pbVar1 < 0) && (*(int *)((int)pvVar2 + 0x168) != -1)
     ) {
    iVar7 = 0;
    if (*(int *)((int)pvVar2 + 0x170) != 0) {
      dVar3 = (*g_RandomGeneratorState.next)();
      iVar7 = (int)(CONCAT44(extraout_EDX,dVar3) % (ulonglong)*(uint *)((int)pvVar2 + 0x170));
    }
    shotDefinition = *(ShotDefinition **)((int)pvVar2 + 0x168);
    modelNode1 = armyRuntime->modelNodeRuntime;
    *(int *)(armyRuntime->reservedE0_EB + 4) = iVar7 + *(int *)((int)pvVar2 + 0x16c);
    launchWorldXQ12 = (modelNode1->worldTransform).translation.x;
    launchWorldYQ12 = (modelNode1->worldTransform).translation.y;
    launchWorldZQ12 = (modelNode1->worldTransform).translation.z;
    worldContext1 = worldRuntime;
    FVar12 = FixedMath_DirectionFromAnglesScaledRegs
                       ((modelNode1->modelPayload).worldRotationAngle1,
                        (modelNode1->modelPayload).worldRotationAngle0,0x1000);
    ShotRuntimePool_CreateProjectileFromDefinition
              (0,(ArmyRuntimeSlot *)armyRuntime->linkedEntityRuntime,
               (int)(FVar12 >> 0x20) + launchWorldZQ12,extraout_ECX + launchWorldYQ12,
               (int)FVar12 + launchWorldXQ12,launchWorldZQ12,launchWorldYQ12,launchWorldXQ12,
               shotDefinition,worldContext1);
  }
  pvVar2 = armyRuntime->definitionOrAsset;
  pbVar1 = armyRuntime->reservedE0_EB + 8;
  bVar11 = SBORROW4(*(int *)pbVar1,g_InGameSimulationStepTicks);
  *(InGameSimulationStepBatchTicks *)pbVar1 = *(int *)pbVar1 - g_InGameSimulationStepTicks;
  if ((*(int *)pbVar1 != 0 && bVar11 == *(int *)pbVar1 < 0) ||
     ((*(int *)((int)pvVar2 + 0x174) == 0 && (*(int *)((int)pvVar2 + 0x58) == 0))))
  goto ArmyRuntime_UpdateTimedShotAndEffectEmitters_UpdateDamageThresholdEffectAndReturn;
  iVar7 = 0;
  if (*(int *)((int)pvVar2 + 0x17c) != 0) {
    dVar3 = (*g_RandomGeneratorState.next)();
    iVar7 = (int)(CONCAT44(extraout_EDX_00,dVar3) % (ulonglong)*(uint *)((int)pvVar2 + 0x17c));
  }
  *(int *)(armyRuntime->reservedE0_EB + 8) = iVar7 + *(int *)((int)pvVar2 + 0x178);
  iVar7 = *(int *)((int)pvVar2 + 100);
  if (*(int *)((int)pvVar2 + 0x4c) == 0x15) {
    iVar7 = *(int *)(iVar7 + 0x18);
  }
  iVar7 = *(int *)(iVar7 + 0x30);
  iVar6 = *(int *)(iVar7 + 0xe8);
  worldContext1 = worldRuntime;
  if (iVar6 == 0) {
ArmyRuntime_UpdateTimedShotAndEffectEmitters_UseModelWorldPositionForEffectEmitter:
    modelNode1 = armyRuntime->modelNodeRuntime;
    worldY = (modelNode1->worldTransform).translation.y;
    uVar13 = CONCAT44((modelNode1->worldTransform).translation.z,
                      (modelNode1->worldTransform).translation.x);
  }
  else {
    uVar10 = 0;
    puVar8 = (uint *)(iVar7 + *(int *)(iVar7 + 0xe4));
    do {
      if (((*puVar8 & 0xf) == 6) && (uVar10 <= *puVar8 >> 4)) {
        uVar10 = (*puVar8 >> 4) + 1;
      }
      iVar6 = iVar6 + -1;
      puVar8 = puVar8 + 4;
    } while (iVar6 != 0);
    if (uVar10 == 0)
    goto ArmyRuntime_UpdateTimedShotAndEffectEmitters_UseModelWorldPositionForEffectEmitter;
    uVar4 = *(uint *)armyRuntime->reservedE0_EB;
    if ((*(uint *)((int)pvVar2 + 0x68) & 1) == 0) {
      uVar4 = (*g_RandomGeneratorState.next)();
    }
    iVar7 = *(int *)((int)pvVar2 + 100);
    bVar11 = *(uint *)((int)pvVar2 + 0x4c) < 0x15;
    if (*(uint *)((int)pvVar2 + 0x4c) == 0x15) {
      iVar7 = *(int *)(iVar7 + 0x18);
    }
    localPointRecord =
         (ModelPackedPointRecord *)
         ModelLookupTable_ContainsPackedKeyCf
                   (uVar4 % uVar10,6,*(ModelResourceHitTestAndRenderView210 **)(iVar7 + 0x30));
    if (bVar11)
    goto ArmyRuntime_UpdateTimedShotAndEffectEmitters_UseModelWorldPositionForEffectEmitter;
    modelNode1 = armyRuntime->modelNodeRuntime;
    if (*(int *)((int)pvVar2 + 0x4c) == 0x15) {
      modelNode1 = modelNode1->childNodes[0];
    }
    uVar13 = ModelNodeRuntime_TransformLocalPointRegs(extraout_ECX_00,localPointRecord,modelNode1);
    worldY = extraout_ECX_01;
  }
  worldX = (Q12)uVar13;
  effectDefinition1 = *(EffectDefinition **)((int)pvVar2 + 0x174);
  effectDefinition = effectDefinition1;
  sVar5 = FieldGrid_GetNearestWaterDelta(worldY,worldX,worldRuntime->fieldGrid);
  worldZQ12 = (Q12)((ulonglong)uVar13 >> 0x20);
  if (0 < sVar5) {
    effectDefinition1 = *(EffectDefinition **)((int)pvVar2 + 0x58);
    effectDefinition = effectDefinition1;
  }
  if ((effectDefinition1 == (EffectDefinition *)0x0) ||
     ((effectDefinition1->transitionPrefix).transitionKind !=
      EFFECT_TRANSITION_INTEGRATE_LINEAR_MOTION_AND_SHADING_POSITION)) {
    modelNode1 = armyRuntime->modelNodeRuntime;
    orientationAngle2 = (modelNode1->modelPayload).worldRotationAngle0;
    orientationAngle1 = (modelNode1->modelPayload).worldRotationAngle1;
    orientationAngle0 = (modelNode1->modelPayload).worldRotationAngle2;
    uVar10 = extraout_ECX_02;
    AVar9 = extraout_EDX_01;
  }
  else {
    dVar3 = (*g_RandomGeneratorState.next)();
    uVar10 = extraout_ECX_03 & dVar3;
    AVar9 = 0x4000 - (dVar3 >> 0x14);
    orientationAngle0 = uVar10;
    orientationAngle1 = AVar9;
    orientationAngle2 = uVar10;
  }
  EffectRuntimePool_CreateInstanceFromDefinitionCf
            (uVar10,AVar9,EFFECT_RUNTIME_COMPLETION_NONE,0,orientationAngle0,orientationAngle1,
             orientationAngle2,worldZQ12,worldY,worldX,effectDefinition,worldContext1);
  *(int *)armyRuntime->reservedE0_EB = *(int *)armyRuntime->reservedE0_EB + 1;
ArmyRuntime_UpdateTimedShotAndEffectEmitters_UpdateDamageThresholdEffectAndReturn:
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,armyRuntime);
  return;
}
