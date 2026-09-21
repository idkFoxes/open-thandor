/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

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

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClassUpdateSlot21_DispatchByClassId
          (WorldRuntimeContext *worldRuntime,ModelRuntimeClass21UpdateView200 *modelRuntime)

{
  dword *pdVar1;
  GraphicsFixedVec3 *pGVar2;
  GraphicsWorldCoordinateQ12 *pGVar3;
  ModelRuntimeArmyLinkOrState4 *pMVar4;
  ArmyRuntimeClassUpdate21DefinitionView27C *pAVar5;
  ModelRuntimeSlot *armyRuntime;
  dword dVar6;
  void *pvVar7;
  ModelRuntimeNode *pMVar8;
  ModelRuntimeSlot *pMVar9;
  ModelDefinitionRuntimeSemanticView280 *pMVar10;
  longlong lVar11;
  dword dVar12;
  int iVar13;
  ArmyRuntimeSlot *armySlot2;
  uint uVar14;
  int iVar15;
  int iVar16;
  Q12 QVar17;
  ArmyRuntimeClassUpdate21DefinitionView27C *classUpdate21Definition;
  ModelRuntimeNode *modelNode1;
  bool bVar18;
  FixedSinCosEdxEax8 FVar19;
  FieldGridHeightEaxCf5 FVar20;
  AngleTurn32 AVar21;
  InGameSimulationStepBatchTicks IVar22;
  AngleTurn32 AVar23;
  ArmyRuntimeClassUpdate21DefinitionView27C *savedClassUpdate21Definition;
  ModelRuntimeClass21UpdateView200 *savedModelRuntime;
  ModelRuntimeNode *modelNode2;
  
  pAVar5 = modelRuntime->modelDefinition;
  armyRuntime = (modelRuntime->classLinkState).modelLinkOrState60.modelRuntime;
  modelNode1 = modelRuntime->rootModelNode;
                    // WARNING: Switch is manually overridden
  switch((modelRuntime->class21State84).classStateB8) {
  case 1:
    if ((armyRuntime == (ModelRuntimeSlot *)0x0) || ((armyRuntime->classState).classStateB0 == 3)) {
      dVar12 = pAVar5->phaseInitialC0;
      dVar6 = pAVar5->phaseDurationC4;
      (modelRuntime->class21State84).classStateB8 = 3;
      (modelRuntime->classLinkState).classState7C = 0;
      (modelRuntime->classLinkState).classState64 = dVar12;
      (modelRuntime->classLinkState).classState68 = dVar6;
    }
    else if (((armyRuntime->classState).classStateB0 == 0) ||
            ((armyRuntime->classState).classStateB0 == 6)) {
      EffectRuntimePool_CreateInstanceFromDefinitionCf
                (EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY,
                 (EffectRuntimeOwnerReference4)modelRuntime,
                 (modelNode1->modelPayload).worldRotationAngle2,
                 (modelNode1->modelPayload).worldRotationAngle1,
                 (modelNode1->modelPayload).worldRotationAngle0,
                 (modelNode1->worldTransform).translation.z,
                 (modelNode1->worldTransform).translation.y,
                 (modelNode1->worldTransform).translation.x,pAVar5->completionEffect190,worldRuntime
                );
      pdVar1 = &(modelRuntime->class21State84).classStateEC;
      *pdVar1 = *pdVar1 | 0x20;
    }
    else {
      dVar12 = (((armyRuntime->rootModelNodeOrSavedOffset).modelNode)->childNodes[0]->modelPayload).
               localTranslationZQ12;
      (modelNode1->childNodes[0]->modelPayload).localTranslationZQ12 = dVar12;
      (modelRuntime->classLinkState).classState80 = dVar12;
    }
    break;
  case 2:
    dVar12 = (modelRuntime->classLinkState).classState7C;
    lVar11 = (longlong)(int)dVar12 * (longlong)(int)dVar12;
    modelNode1 = modelNode1->childNodes[0];
    lVar11 = (longlong)(int)(pAVar5->arcCoefficient14 * g_InGameSimulationStepTicks) *
             (longlong)(int)((int)((ulonglong)lVar11 >> 0x20) << 0x14 | (uint)lVar11 >> 0xc);
    (modelNode1->modelPayload).localRotationAngle0 = 0x8000;
    dVar12 = (modelRuntime->classLinkState).classState7C;
    (modelNode1->modelPayload).localTranslationZQ12 =
         ((int)((ulonglong)lVar11 >> 0x20) << 0x14 | (uint)lVar11 >> 0xc) +
         (modelRuntime->classLinkState).classState80;
    lVar11 = (longlong)(int)(dVar12 * g_InGameSimulationStepTicks) *
             (longlong)pAVar5->arcCoefficient14;
    dVar12 = FixedMath_Atan2Angle16
                       ((int)((ulonglong)lVar11 >> 0x20) << 0x15 | (uint)lVar11 >> 0xb,0x1000);
    iVar13 = pAVar5->movementStepQ12_0C * g_InGameSimulationStepTicks;
    pdVar1 = &(modelRuntime->classLinkState).classState7C;
    *pdVar1 = *pdVar1 + iVar13;
    (modelNode1->modelPayload).localRotationAngle1 = 0x4000 - dVar12 & 0xffff;
    modelNode1 = modelRuntime->rootModelNode;
    FVar19 = FixedMath_SinCosScaled((modelNode1->modelPayload).worldRotationAngle2,iVar13);
    pGVar2 = &(modelNode1->worldTransform).translation;
    pGVar2->x = pGVar2->x + (int)FVar19;
    pGVar3 = &(modelNode1->worldTransform).translation.y;
    *pGVar3 = *pGVar3 + (int)(FVar19 >> 0x20);
    pdVar1 = &(modelRuntime->classLinkState).classState68;
    *pdVar1 = *pdVar1 - 1;
    if (((int)*pdVar1 < 0) &&
       ((modelRuntime->class21State84).classStateB8 = 1, armyRuntime != (ModelRuntimeSlot *)0x0)) {
      pAVar5 = modelRuntime->modelDefinition;
      pMVar9 = (ModelRuntimeSlot *)(armyRuntime->definitionOrSavedId).savedIdOrOffset;
      (armyRuntime->classState).classStateB0 = 4;
      ArmyRuntimeSpawner_PlayCreationSound((ArmyRuntimeSlot *)armyRuntime,worldRuntime);
      iVar13 = (int)(((longlong)(int)armyRuntime->definitionValue60_3C *
                     (longlong)pAVar5->healthOrScaleQ12_60) /
                    (longlong)(pMVar9->classLinkState).modelLinkOrState60.signedScalarState);
      uVar14 = iVar13 - modelRuntime->definitionValue60_3C;
      if (uVar14 != 0 && (int)modelRuntime->definitionValue60_3C <= iVar13) {
        ArmyRuntime_ApplyDamageAndPropagateToParent(uVar14 >> 1,(ArmyRuntimeSlot *)armyRuntime);
      }
    }
    break;
  case 3:
    dVar12 = (modelRuntime->classLinkState).classState7C;
    lVar11 = (longlong)(int)dVar12 * (longlong)(int)dVar12;
    modelNode1 = modelNode1->childNodes[0];
    lVar11 = (longlong)(int)(pAVar5->arcCoefficient14 * g_InGameSimulationStepTicks) *
             (longlong)(int)((int)((ulonglong)lVar11 >> 0x20) << 0x14 | (uint)lVar11 >> 0xc);
    (modelNode1->modelPayload).localRotationAngle0 = 0x8000;
    dVar12 = (modelRuntime->classLinkState).classState7C;
    (modelNode1->modelPayload).localTranslationZQ12 =
         ((int)((ulonglong)lVar11 >> 0x20) << 0x14 | (uint)lVar11 >> 0xc) +
         (modelRuntime->classLinkState).classState80;
    lVar11 = (longlong)(int)(dVar12 * g_InGameSimulationStepTicks) *
             (longlong)pAVar5->arcCoefficient14;
    dVar12 = FixedMath_Atan2Angle16
                       ((int)((ulonglong)lVar11 >> 0x20) << 0x15 | (uint)lVar11 >> 0xb,0x1000);
    iVar13 = pAVar5->movementStepQ12_0C * g_InGameSimulationStepTicks;
    pdVar1 = &(modelRuntime->classLinkState).classState7C;
    *pdVar1 = *pdVar1 + iVar13;
    (modelNode1->modelPayload).localRotationAngle1 = 0x4000 - dVar12 & 0xffff;
    modelNode1 = modelRuntime->rootModelNode;
    FVar19 = FixedMath_SinCosScaled((modelNode1->modelPayload).worldRotationAngle2,iVar13);
    pGVar2 = &(modelNode1->worldTransform).translation;
    pGVar2->x = pGVar2->x + (int)FVar19;
    pGVar3 = &(modelNode1->worldTransform).translation.y;
    *pGVar3 = *pGVar3 + (int)(FVar19 >> 0x20);
    pdVar1 = &(modelRuntime->classLinkState).classState64;
    *pdVar1 = *pdVar1 - 1;
    if ((*pdVar1 == 0) && (armyRuntime != (ModelRuntimeSlot *)0x0)) {
      (armyRuntime->classState).classStateB0 = 4;
      ArmyRuntimeSpawner_PlayCreationSound((ArmyRuntimeSlot *)armyRuntime,worldRuntime);
    }
    pdVar1 = &(modelRuntime->classLinkState).classState68;
    *pdVar1 = *pdVar1 - 1;
    if ((int)*pdVar1 < 0) {
      (modelNode1->worldTransform).translation.x = -0x100000;
      (modelNode1->worldTransform).translation.y = 0x100000;
      (modelRuntime->class21State84).classStateB8 = 4;
    }
    break;
  case 4:
    bVar18 = ArmyRuntime_TestWorldPointAllowedDefaultCf
                       (pAVar5->worldPointAllowedContext48,
                        (modelRuntime->classLinkState).classState74,
                        (modelRuntime->classLinkState).classState70);
    if (!bVar18) {
      uVar14 = (modelRuntime->classLinkState).classState78;
      FVar19 = FixedMath_SinCosScaled
                         (uVar14 ^ 0x8000,pAVar5->movementStepQ12_0C * pAVar5->travelStepCountC8);
      dVar12 = (modelRuntime->classLinkState).classState74;
      (modelNode1->worldTransform).translation.x =
           (int)FVar19 + (modelRuntime->classLinkState).classState70;
      (modelNode1->worldTransform).translation.y = (int)(FVar19 >> 0x20) + dVar12;
      (modelNode1->modelPayload).worldRotationAngle2 = uVar14;
      dVar6 = pAVar5->travelStepCountC8;
      (modelRuntime->classLinkState).armyLinkOrState6C.classState = dVar6;
      dVar12 = dVar6 * 2;
      iVar13 = pAVar5->movementStepQ12_0C;
      (modelRuntime->classLinkState).classState68 = dVar12;
      (modelRuntime->classLinkState).classState7C = -dVar6 * iVar13;
      (modelRuntime->class21State84).classStateB8 = 5;
      FVar19 = FixedMath_SinCosScaled(uVar14,pAVar5->movementStepQ12_0C * 10);
      iVar13 = (modelNode1->worldTransform).translation.x;
      iVar16 = (modelNode1->worldTransform).translation.y;
      iVar15 = 0;
      dVar6 = (modelRuntime->classLinkState).classState80;
      do {
        FVar20 = FieldGrid_InterpolateTopSurfaceHeight(iVar16,iVar13,worldRuntime->fieldGrid);
        if (iVar15 < FVar20.heightQ12) {
          iVar15 = FVar20.heightQ12;
        }
        iVar13 = iVar13 + (int)FVar19;
        iVar16 = iVar16 + (int)(FVar19 >> 0x20);
        dVar12 = dVar12 - 10;
      } while (-1 < (int)dVar12);
      (modelRuntime->class21State84).trajectoryTerrainReferenceHeightQ12_84 =
           iVar15 + 0x800 + dVar6 * 2;
    }
    break;
  case 5:
    FVar19 = FixedMath_SinCosScaled
                       ((modelNode1->modelPayload).worldRotationAngle2,
                        g_InGameSimulationStepTicks * pAVar5->movementStepQ12_0C);
    pGVar2 = &(modelNode1->worldTransform).translation;
    pGVar2->x = pGVar2->x + (int)FVar19;
    pGVar3 = &(modelNode1->worldTransform).translation.y;
    *pGVar3 = *pGVar3 + (int)(FVar19 >> 0x20);
    FVar20 = FieldGrid_InterpolateTopSurfaceHeight
                       ((modelNode1->worldTransform).translation.y,
                        (modelNode1->worldTransform).translation.x,worldRuntime->fieldGrid);
    classUpdate21Definition = modelRuntime->modelDefinition;
    dVar12 = (modelRuntime->classLinkState).classState7C;
    lVar11 = (longlong)(int)dVar12 * (longlong)(int)dVar12;
    modelNode1 = modelNode1->childNodes[0];
    lVar11 = (longlong)
             (int)(classUpdate21Definition->verticalArcCoefficientCC * g_InGameSimulationStepTicks)
             * (longlong)(int)((int)((ulonglong)lVar11 >> 0x20) << 0x14 | (uint)lVar11 >> 0xc);
    (modelNode1->modelPayload).localRotationAngle0 = 0x8000;
    dVar12 = (modelRuntime->classLinkState).classState7C;
    (modelNode1->modelPayload).localTranslationZQ12 =
         (((int)((ulonglong)lVar11 >> 0x20) << 0x14 | (uint)lVar11 >> 0xc) +
         (modelRuntime->class21State84).trajectoryTerrainReferenceHeightQ12_84) - FVar20.heightQ12;
    lVar11 = (longlong)(int)(dVar12 * g_InGameSimulationStepTicks) *
             (longlong)classUpdate21Definition->verticalArcCoefficientCC;
    dVar12 = FixedMath_Atan2Angle16
                       ((int)((ulonglong)lVar11 >> 0x20) << 0x15 | (uint)lVar11 >> 0xb,0x1000);
    pdVar1 = &(modelRuntime->classLinkState).classState7C;
    *pdVar1 = *pdVar1 + classUpdate21Definition->movementStepQ12_0C * g_InGameSimulationStepTicks;
    (modelNode1->modelPayload).localRotationAngle1 = 0x4000 - dVar12 & 0xffff;
    modelNode1 = modelRuntime->rootModelNode;
    IVar22 = g_InGameSimulationStepTicks;
    do {
      pMVar4 = &(modelRuntime->classLinkState).armyLinkOrState6C;
      pMVar4->armyRuntime = (ArmyRuntimeSlot *)(pMVar4->classState - 1);
      pdVar1 = &(modelRuntime->classLinkState).classState68;
      *pdVar1 = *pdVar1 - 1;
      if ((int)*pdVar1 < 0) {
        (modelNode1->worldTransform).translation.x = -0x100000;
        (modelNode1->worldTransform).translation.y = 0x100000;
        ((ModelRuntimeSlotClassState7C *)&modelRuntime->class21State84)->classStateB8 = 6;
      }
      pvVar7 = classUpdate21Definition->modelPointSource64;
      savedClassUpdate21Definition = classUpdate21Definition;
      modelNode2 = modelNode1;
      savedModelRuntime = modelRuntime;
      ArmyRuntime_TryPlayMappedTerrainSoundAtWorldPoint
                (modelRuntime->ownerArmyRuntime->factionIndex,
                 (modelNode1->worldTransform).translation.y,
                 (modelNode1->worldTransform).translation.x,
                 classUpdate21Definition->terrainSoundAssetIndex26C,worldRuntime);
      pMVar8 = modelNode1->childNodes[0];
      iVar13 = classUpdate21Definition->modelPointStep30 * -3;
      iVar16 = (pMVar8->worldTransform).translation.z + -0x2000;
      modelNode1 = modelNode2;
      if (classUpdate21Definition->modelPointStep30 != 0) {
        pvVar7 = *(void **)((int)pvVar7 + 0x18);
        if (iVar13 - (modelRuntime->classLinkState).armyLinkOrState6C.classState == 0) {
          ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
                    (0,iVar16,(pMVar8->worldTransform).translation.y,
                     (pMVar8->worldTransform).translation.x,7,
                     classUpdate21Definition->modelPointEffectId2C,pMVar8,pvVar7,worldRuntime);
          modelNode1 = modelNode2;
        }
        else {
          armySlot2 = (ArmyRuntimeSlot *)(iVar13 + classUpdate21Definition->modelPointStep30);
          if (armySlot2 == (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime) {
            ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
                      (0,iVar16,(pMVar8->worldTransform).translation.y,
                       (pMVar8->worldTransform).translation.x,6,
                       classUpdate21Definition->modelPointEffectId2C,pMVar8,pvVar7,worldRuntime);
            modelNode1 = modelNode2;
          }
          else {
            armySlot2 = (ArmyRuntimeSlot *)
                        ((int)&armySlot2->modelRuntimeOrSavedOffset +
                        classUpdate21Definition->modelPointStep30);
            if (armySlot2 == (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime) {
              ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
                        (0,iVar16,(pMVar8->worldTransform).translation.y,
                         (pMVar8->worldTransform).translation.x,5,
                         classUpdate21Definition->modelPointEffectId2C,pMVar8,pvVar7,worldRuntime);
              modelNode1 = modelNode2;
            }
            else {
              armySlot2 = (ArmyRuntimeSlot *)
                          ((int)&armySlot2->modelRuntimeOrSavedOffset +
                          classUpdate21Definition->modelPointStep30);
              if (armySlot2 == (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime) {
                ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
                          (0,iVar16,(pMVar8->worldTransform).translation.y,
                           (pMVar8->worldTransform).translation.x,4,
                           classUpdate21Definition->modelPointEffectId2C,pMVar8,pvVar7,worldRuntime)
                ;
                modelNode1 = modelNode2;
              }
              else {
                armySlot2 = (ArmyRuntimeSlot *)
                            ((int)&armySlot2->modelRuntimeOrSavedOffset +
                            classUpdate21Definition->modelPointStep30);
                if (armySlot2 == (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime) {
                  ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
                            (0,iVar16,(pMVar8->worldTransform).translation.y,
                             (pMVar8->worldTransform).translation.x,3,
                             classUpdate21Definition->modelPointEffectId2C,pMVar8,pvVar7,
                             worldRuntime);
                  modelNode1 = modelNode2;
                }
                else {
                  armySlot2 = (ArmyRuntimeSlot *)
                              ((int)&armySlot2->modelRuntimeOrSavedOffset +
                              classUpdate21Definition->modelPointStep30);
                  if (armySlot2 == (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime) {
                    ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
                              (0,iVar16,(pMVar8->worldTransform).translation.y,
                               (pMVar8->worldTransform).translation.x,2,
                               classUpdate21Definition->modelPointEffectId2C,pMVar8,pvVar7,
                               worldRuntime);
                    modelNode1 = modelNode2;
                  }
                  else if ((ArmyRuntimeSlot *)
                           ((int)&armySlot2->modelRuntimeOrSavedOffset +
                           classUpdate21Definition->modelPointStep30) ==
                           (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime) {
                    ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
                              (0,iVar16,(pMVar8->worldTransform).translation.y,
                               (pMVar8->worldTransform).translation.x,1,
                               classUpdate21Definition->modelPointEffectId2C,pMVar8,pvVar7,
                               worldRuntime);
                    modelNode1 = modelNode2;
                  }
                }
              }
            }
          }
        }
      }
      modelRuntime = savedModelRuntime;
      classUpdate21Definition = savedClassUpdate21Definition;
      IVar22 = IVar22 - 1;
    } while (IVar22 != 0);
    break;
  case 6:
    if (armyRuntime != (ModelRuntimeSlot *)0x0) {
      modelNode1 = (armyRuntime->rootModelNodeOrSavedOffset).modelNode;
      if (((armyRuntime->classState).classStateB0 == 0) &&
         (bVar18 = ArmyRuntime_TestWorldPointAllowedDefaultCf
                             (pAVar5->worldPointAllowedContext48,
                              (modelNode1->worldTransform).translation.y,
                              (modelNode1->worldTransform).translation.x), !bVar18)) {
        (armyRuntime->classState).classStateB0 = 1;
        ArmyRuntime_TrySpawnDefinitionEffectAtWorldPoint
                  ((ArmyRuntimeSlot *)armyRuntime,worldRuntime);
        uVar14 = (modelNode1->modelPayload).worldRotationAngle2;
        FVar19 = FixedMath_SinCosScaled
                           (uVar14 ^ 0x8000,pAVar5->movementStepQ12_0C * pAVar5->phaseDurationC4);
        iVar13 = (modelNode1->worldTransform).translation.y;
        pMVar8 = modelRuntime->rootModelNode;
        (pMVar8->worldTransform).translation.x =
             (int)FVar19 + (modelNode1->worldTransform).translation.x;
        (pMVar8->worldTransform).translation.y = (int)(FVar19 >> 0x20) + iVar13;
        (pMVar8->modelPayload).worldRotationAngle2 = uVar14;
        dVar12 = pAVar5->phaseDurationC4;
        iVar13 = pAVar5->movementStepQ12_0C;
        (modelRuntime->classLinkState).classState68 = dVar12;
        (modelRuntime->classLinkState).classState7C = -dVar12 * iVar13;
        (modelRuntime->class21State84).classStateB8 = 2;
      }
      break;
    }
    ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive
              (worldRuntime,(int *)modelRuntime->ownerArmyRuntime);
  case 0:
    if (armyRuntime == (ModelRuntimeSlot *)0x0) {
      QVar17 = 0x1000;
    }
    else {
      QVar17 = (((armyRuntime->rootModelNodeOrSavedOffset).modelNode)->childNodes[0]->modelPayload).
               localTranslationZQ12;
    }
    (modelNode1->childNodes[0]->modelPayload).localTranslationZQ12 = QVar17;
  }
  modelNode1 = modelRuntime->rootModelNode;
  AVar23 = (modelNode1->modelPayload).worldRotationAngle0;
  AVar21 = (modelNode1->modelPayload).worldRotationAngle1;
  (*g_ArmyPlacementContactKindDispatchTable.callbacks
    [((ModelDefinitionRuntimeSemanticView280 *)modelRuntime->modelDefinition)->
     placementContactKindIndex278])
            (((ModelDefinitionRuntimeSemanticView280 *)modelRuntime->modelDefinition)->
             placementHeightOffsetQ12,(modelNode1->worldTransform).translation.y,
             (modelNode1->worldTransform).translation.x,modelNode1,worldRuntime);
  dVar12 = ((ModelRuntimeSlotClassState7C *)&modelRuntime->class21State84)->classStateB8;
  (modelNode1->modelPayload).worldRotationAngle1 = AVar21;
  (modelNode1->modelPayload).worldRotationAngle0 = AVar23;
  if ((dVar12 != 0) && (dVar12 != 1)) {
    ArmyRuntime_UpdateTimedShotAndEffectEmitters
              (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
  }
  pMVar10 = (ModelDefinitionRuntimeSemanticView280 *)modelRuntime->modelDefinition;
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNode1);
  ModelNodeRuntime_RecomputeSubtreeBoundingRadius(modelNode1);
  ModelNodeRuntime_UpdateDepthBinMasks(pMVar10->placementRadiusOrClearanceDC,modelNode1);
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

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_UpdateLinkedModelFlagsAndDispatchTerrainContactMode
          (WorldRuntimeContext *worldRuntime,
          ModelRuntimeLinkedChildSpawnAndBuildView200 *modelRuntime)

{
  FactionArmyAssetCount *pFVar1;
  FactionRelationCounter *pFVar2;
  byte *pbVar3;
  Q12 *pQVar4;
  int *piVar5;
  dword dVar6;
  dword dVar7;
  int iVar8;
  uint uVar9;
  PckArmyAssetIdCatalog PVar10;
  ModelRuntimeNode *modelNodeRuntime;
  DirectSoundVoiceSet **ppDVar11;
  ModelRuntimeNode *pMVar12;
  ModelDefinitionRecordPrefix *modelDefinition1;
  FactionArmyAssetCount linkedChildUsedSlotCount;
  ArmyBuildXeniteCostQ4 secondaryArmyBuildXeniteCostQ4;
  FactionArmyAssetCount FVar13;
  int linkedChildReverseSlotIndex;
  int iVar14;
  uint uVar15;
  WorldRuntimeNode *ownerNodeCursor;
  dword *pdVar16;
  bool bVar17;
  ArmyRegistryEaxCf5_51b6d0 AVar18;
  ModelDefinitionLookupEaxCf5 MVar19;
  ModelLookupPayloadEaxEcxEdxCf13 MVar20;
  Q12 QVar21;
  PckModelDefinitionIdCatalog notificationMovieId;
  ModelDefinitionLinkedChildStateView280 *pMVar22;
  int *modelRuntimePayloadWords;
  ArmyRuntimeSlot *entityRuntime1;
  
  if ((modelRuntime->linkedChildRuntimeFlagsEC & 0x10) != 0) {
    ownerNodeCursor = (WorldRuntimeNode *)worldRuntime->ownerListHead;
    do {
      if ((((ownerNodeCursor[2].common.nextNode == (WorldRuntimeNode *)0x0) &&
           (piVar5 = ownerNodeCursor->runtimePayload, *(int *)(*piVar5 + 0x4c) == 0x15)) &&
          (modelRuntime == (ModelRuntimeLinkedChildSpawnAndBuildView200 *)piVar5[0x18])) &&
         ((piVar5[0x3b] & 0x10U) == 0)) {
        ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive(worldRuntime,(int *)piVar5[2]);
      }
      ownerNodeCursor = (ownerNodeCursor->common).nextNode;
    } while (ownerNodeCursor != (WorldRuntimeNode *)0x0);
  }
                    // WARNING: Switch is manually overridden
  switch(modelRuntime->secondaryArmyAssetBuildStateAC) {
  case 0:
    if ((modelRuntime->linkedChildRuntimeFlagsEC & 0x40) == 0) {
                    // [V427CI_ARMY_C4_SCALAR_ARTIFACT] Immutable 00526708: CMP ECX,[EAX+0xC4]; ECX
                    // and linkedChildSlotCountC4 are FactionArmyAssetCount scalars. Any
                    // GraphicsFixedVec3* cast here is a symbol-less decompiler presentation
                    // artifact.
      if (((modelRuntime->linkedChildRuntimeFlagsEC & 0xc9) == 0) &&
         (iVar8 = ((modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime)->factionIndex,
         (modelRuntime->linkedChildBuildState60).completedSecondaryArmyAssetCount6C <
         modelRuntime->modelDefinition->linkedChildSlotCapacityC4)) {
        FVar13 = g_GameFactionRuntimeImage.records[iVar8].secondaryArmyAssetCount;
        pdVar16 = g_GameFactionRuntimeImage.records[iVar8].secondaryArmyAssetPointersOrIds;
        if (FVar13 != 0) {
ArmyRuntimeClass_SelectAffordableSecondaryArmyAssetLoop:
          dVar6 = *pdVar16;
                    // [V427CI_ARMY_XENITE_SCALAR_ARTIFACT] Immutable 00526745: CMP
                    // ECX,[EDX+0x50F340]; ECX is the +0x28 secondary Army Xenite cost and memory is
                    // faction xeniteCurrentQ4. Both are scalar resource values; pointer casts are
                    // presentation-only.
          if (((*(uint *)(dVar6 + 0x14) & 8) == 0) ||
             (g_GameFactionRuntimeImage.records[iVar8].xeniteCurrentQ4 < *(uint *)(dVar6 + 0x28)))
          goto ArmyRuntimeClass_AdvanceSecondaryArmyAssetCandidate;
          g_GameFactionRuntimeImage.records[iVar8].xeniteCurrentQ4 =
               g_GameFactionRuntimeImage.records[iVar8].xeniteCurrentQ4 - *(uint *)(dVar6 + 0x28);
          uVar15 = *(uint *)(dVar6 + 0x24);
          dVar7 = *(dword *)(dVar6 + 0x2c);
          if ((g_UiCommandRuntimeFlags & 0x100000) != 0) {
            uVar15 = (uVar15 >> 4) + 1;
          }
          PVar10 = *(PckArmyAssetIdCatalog *)(dVar6 + 8);
          (modelRuntime->linkedChildBuildState60).secondaryArmyAssetBuildRequiredTicks68 = uVar15;
          (modelRuntime->linkedChildBuildState60).selectedSecondaryArmyAssetValue74 = dVar7;
          (modelRuntime->linkedChildBuildState60).selectedSecondaryArmyAssetId60 = PVar10;
          modelRuntime->definitionDerivedValueF4 = modelRuntime->definitionDerivedValueF4 + dVar7;
          (modelRuntime->linkedChildBuildState60).secondaryArmyAssetBuildElapsedTicks64 = 0;
          pFVar1 = &g_GameFactionRuntimeImage.records[iVar8].secondaryArmyAssetCount;
          *pFVar1 = *pFVar1 - 1;
          do {
            *pdVar16 = pdVar16[1];
            pdVar16 = pdVar16 + 1;
            FVar13 = FVar13 - 1;
          } while (FVar13 != 0);
          modelRuntime->secondaryArmyAssetBuildStateAC = 1;
          modelRuntime->linkedChildRuntimeFlagsEC = modelRuntime->linkedChildRuntimeFlagsEC | 0x100;
          break;
        }
      }
    }
    else if ((modelRuntime->linkedChildRuntimeFlagsEC & 9) == 0) {
      ArmyRuntime_UpdateTimedShotAndEffectEmitters
                (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
      ArmyRuntime_UpdateAnimatedModelSubnodes
                (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
    }
    break;
  case 1:
    if ((modelRuntime->linkedChildRuntimeFlagsEC & 9) == 0) {
      pdVar16 = &(modelRuntime->linkedChildBuildState60).secondaryArmyAssetBuildElapsedTicks64;
      *pdVar16 = *pdVar16 + g_InGameSimulationStepTicks;
      entityRuntime1 = (modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime;
      ArmyRuntime_UpdateTimedShotAndEffectEmitters
                (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
      uVar15 = (modelRuntime->linkedChildBuildState60).secondaryArmyAssetBuildElapsedTicks64;
      ArmyRuntime_UpdateAnimatedModelSubnodes
                (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
      if ((modelRuntime->linkedChildBuildState60).secondaryArmyAssetBuildRequiredTicks68 <= uVar15)
      {
        iVar8 = entityRuntime1->factionIndex;
        uVar9 = (modelRuntime->linkedChildBuildState60).selectedSecondaryArmyAssetValue74;
        (modelRuntime->linkedChildBuildState60).selectedSecondaryArmyAssetValue74 = 0;
        modelRuntime->secondaryArmyAssetBuildStateAC = 0;
        modelRuntime->linkedChildRuntimeFlagsEC =
             modelRuntime->linkedChildRuntimeFlagsEC & 0xfffffeff;
        pdVar16 = &modelRuntime->definitionDerivedValueF4;
        uVar15 = *pdVar16;
        *pdVar16 = *pdVar16 - uVar9;
        PVar10 = (modelRuntime->linkedChildBuildState60).selectedSecondaryArmyAssetId60;
        (modelRuntime->linkedChildBuildState60).selectedSecondaryArmyAssetId60 = 0;
        if (uVar9 <= uVar15) {
          iVar14 = modelRuntime->modelDefinition->linkedChildSlotCapacityC4 - 1;
          do {
            if (modelRuntime->completedSecondaryArmyAssetIds78[iVar14] == 0)
            goto ArmyRuntimeClass_StoreCompletedSecondaryArmyAssetId;
                    // [V427CI_ARMY_REVERSE_INDEX_ARTIFACT] Immutable 005268DA: DEC ECX; 005268DB:
                    // JNS 005268D0. ECX is a signed reverse linked-child slot index. The persistent
                    // DB phase local linkedChildReverseSlotIndex is int; pointer-shaped
                    // HighVariable recovery at this site is non-authoritative unless it maps to a
                    // pointer-typed persistent Function local.
            iVar14 = iVar14 + -1;
          } while (-1 < iVar14);
          iVar14 = 0;
ArmyRuntimeClass_StoreCompletedSecondaryArmyAssetId:
          modelRuntime->completedSecondaryArmyAssetIds78[iVar14] = PVar10;
          pFVar2 = &g_GameFactionRuntimeImage.records[iVar8].relationCounterA;
          *pFVar2 = *pFVar2 + 1;
          pFVar1 = &(modelRuntime->linkedChildBuildState60).completedSecondaryArmyAssetCount6C;
          *pFVar1 = *pFVar1 + 1;
          pdVar16 = &(modelRuntime->linkedChildBuildState60).classState70;
          *pdVar16 = *pdVar16 + 1;
          if (iVar8 == worldRuntime->activeFactionRuntimeIndex) {
            AVar18 = ArmyAssetRegistry_FindByIdCf(PVar10);
            MVar19 = ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                               (entityRuntime1->factionIndex,(AVar18.eax)->rootNodeOffsetOrPointer);
            modelDefinition1 = MVar19.modelDefinition;
            modelDefinition1[0x24].byteSize = modelDefinition1[0x24].byteSize + 1;
            notificationMovieId = modelDefinition1[0x1d].flags;
            if (modelDefinition1[0x24].byteSize != 1) {
              notificationMovieId = modelDefinition1[0x1d].definitionId;
            }
            InGameNotificationQueue_InsertPriorityRecord
                      (NONE,0,(worldRuntime->motion).pitchAngle,(worldRuntime->motion).headingAngle,
                       (((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->worldTransform).
                       translation.y,
                       (((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->worldTransform).
                       translation.x,3,notificationMovieId);
          }
        }
      }
    }
  }
ArmyRuntimeClass_DispatchLinkedChildSpawnAndDamageEffectState:
  pMVar22 = modelRuntime->modelDefinition;
  modelNodeRuntime = (modelRuntime->rootModelNodeOrSavedOffset).modelNode;
                    // WARNING: Switch is manually overridden
  switch(modelRuntime->linkedChildTransitionStateB0) {
  case 0:
    goto ArmyRuntimeClass_ProcessPendingLinkedChildSpawnsAndDamageEffect;
  case 1:
    modelNodeRuntime->primaryTextureOffsetV =
         modelNodeRuntime->primaryTextureOffsetV +
         pMVar22->linkedChildTextureVStepPerTick0C * g_InGameSimulationStepTicks;
    if (0x7ffff < modelNodeRuntime->primaryTextureOffsetV) {
      modelNodeRuntime->primaryTextureOffsetV = 0x80000;
      modelRuntime->linkedChildTransitionStateB0 = 2;
      uVar15 = pMVar22->linkedChildTransitionSoundAssetIndex270;
      if (((uVar15 != 0) && (uVar15 < worldRuntime->dwordArrayCount)) &&
         (worldRuntime->dwordArray != (dword *)0x0)) {
        ppDVar11 = (DirectSoundVoiceSet **)worldRuntime->dwordArray[uVar15];
        if ((ppDVar11 != (DirectSoundVoiceSet **)0x0) &&
           (bVar17 = TerrainGrid_TestProjectedCellMaskBits01Cf
                               ((modelNodeRuntime->worldTransform).translation.y,
                                (modelNodeRuntime->worldTransform).translation.x,worldRuntime),
           !bVar17)) {
          SpatialSound_PlayPositionedOneShot
                    (pMVar22->positionedSoundMaximumDistanceQ12,pMVar22->positionedSoundGainQ15,
                     &(modelNodeRuntime->worldTransform).translation,ppDVar11);
        }
      }
    }
    break;
  case 2:
    pMVar12 = modelNodeRuntime->childNodes[0];
    iVar8 = pMVar22->linkedChildTranslationLimitQ12CC;
    pQVar4 = &(pMVar12->modelPayload).localTranslationZQ12;
    *pQVar4 = *pQVar4 + pMVar22->linkedChildTranslationStepQ12PerTickC8 *
                        g_InGameSimulationStepTicks;
    if (iVar8 < (pMVar12->modelPayload).localTranslationZQ12) {
      modelRuntime->linkedChildTransitionStateB0 = 3;
      (pMVar12->modelPayload).localTranslationZQ12 = iVar8;
    }
    ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
    break;
  case 4:
    QVar21 = pMVar22->linkedChildTranslationStepQ12PerTickC8;
    MVar20 = ModelLookupTable_FindPackedKeyEntryRegsCf
                       (0,1,(modelNodeRuntime->modelPayload).modelResource);
    pMVar12 = modelNodeRuntime->childNodes[0];
    pQVar4 = &(pMVar12->modelPayload).localTranslationZQ12;
    *pQVar4 = *pQVar4 - QVar21 * g_InGameSimulationStepTicks;
    if ((pMVar12->modelPayload).localTranslationZQ12 < (int)MVar20.payloadEdx) {
      modelRuntime->linkedChildTransitionStateB0 = 5;
      (pMVar12->modelPayload).localTranslationZQ12 = MVar20.payloadEdx;
    }
    ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
    if (modelRuntime->linkedChildTransitionStateB0 == 5) {
      uVar15 = pMVar22->linkedChildTransitionSoundAssetIndex26C;
      if (((uVar15 != 0) && (uVar15 < worldRuntime->dwordArrayCount)) &&
         (worldRuntime->dwordArray != (dword *)0x0)) {
        ppDVar11 = (DirectSoundVoiceSet **)worldRuntime->dwordArray[uVar15];
        if ((ppDVar11 != (DirectSoundVoiceSet **)0x0) &&
           (bVar17 = TerrainGrid_TestProjectedCellMaskBits01Cf
                               ((modelNodeRuntime->worldTransform).translation.y,
                                (modelNodeRuntime->worldTransform).translation.x,worldRuntime),
           !bVar17)) {
          SpatialSound_PlayPositionedOneShot
                    (pMVar22->positionedSoundMaximumDistanceQ12,pMVar22->positionedSoundGainQ15,
                     &(modelNodeRuntime->worldTransform).translation,ppDVar11);
        }
      }
    }
    break;
  case 5:
    modelNodeRuntime->primaryTextureOffsetV =
         modelNodeRuntime->primaryTextureOffsetV -
         pMVar22->linkedChildTextureVStepPerTick0C * g_InGameSimulationStepTicks;
    if (modelNodeRuntime->primaryTextureOffsetV < 1) {
      modelNodeRuntime->primaryTextureOffsetV = 0;
      modelRuntime->linkedChildTransitionStateB0 = 6;
    }
    break;
  case 6:
    modelRuntime->linkedChildTransitionStateB0 = 0;
ArmyRuntimeClass_ProcessPendingLinkedChildSpawnsAndDamageEffect:
    if (((modelRuntime->linkedChildRuntimeFlagsEC & 9) == 0) &&
       (bVar17 = ArmyRuntime_TestWorldPointAllowedDefaultCf
                           (pMVar22->runtimeValue48,(modelNodeRuntime->worldTransform).translation.y
                            ,(modelNodeRuntime->worldTransform).translation.x), !bVar17)) {
      if ((modelRuntime->linkedChildPendingSpawnCountsDC).slot0 != 0) {
        (modelRuntime->linkedChildPendingSpawnCountsDC).slot0 =
             (modelRuntime->linkedChildPendingSpawnCountsDC).slot0 - 1;
        bVar17 = ArmyRuntimeSpawner_CreateLinkedChildInstanceCf
                           (modelRuntime->linkedChildSpawnInheritedStateB8[0].inheritedValue78,
                            modelRuntime->linkedChildSpawnInheritedStateB8[0].inheritedValue74,
                            modelRuntime->linkedChildSpawnInheritedStateB8[0].inheritedValue70,
                            g_ArmyLinkedChildAssetIdSlot0,worldRuntime,
                            (ArmyRuntimeLinkedChildMaskSlotView *)modelRuntime);
        if (!bVar17) {
          modelRuntime->linkedChildTransitionStateB0 = 1;
          uVar15 = pMVar22->linkedChildTransitionSoundAssetIndex26C;
          if (((uVar15 != 0) && (uVar15 < worldRuntime->dwordArrayCount)) &&
             (worldRuntime->dwordArray != (dword *)0x0)) {
            ppDVar11 = (DirectSoundVoiceSet **)worldRuntime->dwordArray[uVar15];
            if ((ppDVar11 != (DirectSoundVoiceSet **)0x0) &&
               (bVar17 = TerrainGrid_TestProjectedCellMaskBits01Cf
                                   ((modelNodeRuntime->worldTransform).translation.y,
                                    (modelNodeRuntime->worldTransform).translation.x,worldRuntime),
               !bVar17)) {
              SpatialSound_PlayPositionedOneShot
                        (pMVar22->positionedSoundMaximumDistanceQ12,pMVar22->positionedSoundGainQ15,
                         &(modelNodeRuntime->worldTransform).translation,ppDVar11);
            }
          }
          break;
        }
      }
      if ((modelRuntime->linkedChildPendingSpawnCountsDC).slot1 != 0) {
        pbVar3 = &(modelRuntime->linkedChildPendingSpawnCountsDC).slot1;
        *pbVar3 = *pbVar3 - 1;
        bVar17 = ArmyRuntimeSpawner_CreateLinkedChildInstanceCf
                           (modelRuntime->linkedChildSpawnInheritedStateB8[1].inheritedValue78,
                            modelRuntime->linkedChildSpawnInheritedStateB8[1].inheritedValue74,
                            modelRuntime->linkedChildSpawnInheritedStateB8[1].inheritedValue70,
                            g_ArmyLinkedChildAssetIdSlot1,worldRuntime,
                            (ArmyRuntimeLinkedChildMaskSlotView *)modelRuntime);
        if (!bVar17) {
          modelRuntime->linkedChildTransitionStateB0 = 1;
          uVar15 = pMVar22->linkedChildTransitionSoundAssetIndex26C;
          if (((uVar15 != 0) && (uVar15 < worldRuntime->dwordArrayCount)) &&
             (worldRuntime->dwordArray != (dword *)0x0)) {
            ppDVar11 = (DirectSoundVoiceSet **)worldRuntime->dwordArray[uVar15];
            if ((ppDVar11 != (DirectSoundVoiceSet **)0x0) &&
               (bVar17 = TerrainGrid_TestProjectedCellMaskBits01Cf
                                   ((modelNodeRuntime->worldTransform).translation.y,
                                    (modelNodeRuntime->worldTransform).translation.x,worldRuntime),
               !bVar17)) {
              SpatialSound_PlayPositionedOneShot
                        (pMVar22->positionedSoundMaximumDistanceQ12,pMVar22->positionedSoundGainQ15,
                         &(modelNodeRuntime->worldTransform).translation,ppDVar11);
            }
          }
          break;
        }
      }
      if ((modelRuntime->linkedChildPendingSpawnCountsDC).slot2 != 0) {
        pbVar3 = &(modelRuntime->linkedChildPendingSpawnCountsDC).slot2;
        *pbVar3 = *pbVar3 - 1;
        bVar17 = ArmyRuntimeSpawner_CreateLinkedChildInstanceCf
                           (modelRuntime->linkedChildSpawnInheritedStateB8[2].inheritedValue78,
                            modelRuntime->linkedChildSpawnInheritedStateB8[2].inheritedValue74,
                            modelRuntime->linkedChildSpawnInheritedStateB8[2].inheritedValue70,
                            g_ArmyLinkedChildAssetIdSlot2,worldRuntime,
                            (ArmyRuntimeLinkedChildMaskSlotView *)modelRuntime);
        if (!bVar17) {
          modelRuntime->linkedChildTransitionStateB0 = 1;
          uVar15 = pMVar22->linkedChildTransitionSoundAssetIndex26C;
          if (((uVar15 != 0) && (uVar15 < worldRuntime->dwordArrayCount)) &&
             (worldRuntime->dwordArray != (dword *)0x0)) {
            ppDVar11 = (DirectSoundVoiceSet **)worldRuntime->dwordArray[uVar15];
            if ((ppDVar11 != (DirectSoundVoiceSet **)0x0) &&
               (bVar17 = TerrainGrid_TestProjectedCellMaskBits01Cf
                                   ((modelNodeRuntime->worldTransform).translation.y,
                                    (modelNodeRuntime->worldTransform).translation.x,worldRuntime),
               !bVar17)) {
              SpatialSound_PlayPositionedOneShot
                        (pMVar22->positionedSoundMaximumDistanceQ12,pMVar22->positionedSoundGainQ15,
                         &(modelNodeRuntime->worldTransform).translation,ppDVar11);
            }
          }
        }
      }
    }
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  return;
ArmyRuntimeClass_AdvanceSecondaryArmyAssetCandidate:
  pdVar16 = pdVar16 + 1;
  FVar13 = FVar13 - 1;
  if (FVar13 == 0) goto ArmyRuntimeClass_DispatchLinkedChildSpawnAndDamageEffectState;
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
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClassUpdateSlot13_PrepareModelAndDispatchByClassId
          (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView200 *modelRuntime)

{
  byte *pbVar1;
  FactionRelationCounter *pFVar2;
  dword *pdVar3;
  FactionArmyAssetCount *pFVar4;
  ModelRuntimeNode *pMVar5;
  ModelDefinitionRuntimeSemanticView280 *pMVar6;
  ModelRuntimeSlot *pMVar7;
  int iVar8;
  int *piVar9;
  int iVar10;
  int iVar11;
  DirectSoundVoiceSet **ppDVar12;
  ModelRuntimeSlotLinkOrState4 MVar13;
  dword dVar14;
  ArmyRuntimeSlot *armySlot1;
  Q12 targetWorldXQ12;
  dword dVar15;
  Q12 worldXQ12;
  GraphicsFixedVec3 *pGVar16;
  FactionArmyAssetCount FVar17;
  Q12 worldYQ12;
  uint uVar18;
  dword *pdVar19;
  bool bVar20;
  ModelLookupEntryEaxCf5 MVar21;
  ArmyRuntimeCreateEaxCf5 AVar22;
  ModelLocalPointRegs12 MVar23;
  InGameNotificationMovieId notificationMovieId;
  ArmyRuntimeSlot *entityRuntime1;
  
  pMVar5 = modelRuntime->rootModelNode;
  if ((*(uint *)(modelRuntime->classState).reservedBC_BF & 1) != 0) {
    MVar21 = ModelLookupTable_ContainsPackedKeyCf(1,5,(pMVar5->modelPayload).modelResource);
    if (!MVar21.carry) {
      MVar23 = ModelNodeRuntime_TransformLocalPointRegs(MVar21.entry,pMVar5);
      (modelRuntime->classLinkState).classState78 = MVar23.eax;
      (modelRuntime->classLinkState).classState7C = MVar23.ecx;
      pbVar1 = (modelRuntime->classState).reservedBC_BF;
      *(uint *)pbVar1 = *(uint *)pbVar1 & 0xfffffffe;
    }
  }
  dVar14 = (modelRuntime->classState).classStateB8;
  pMVar6 = modelRuntime->modelDefinition;
  if ((3 < pMVar5->childCount) && (pMVar5->childNodes[3] != (ModelRuntimeNode *)0x0)) {
    WorldRuntime_UnlinkNodeFromOwnerListD8((WorldOwnerListNode100 *)pMVar5->childNodes[3]);
    pMVar5->childNodes[3] = (ModelRuntimeNode *)0x0;
  }
                    // WARNING: Switch is manually overridden
  switch(dVar14) {
  case 0:
    if (((modelRuntime->classState).classStateEC & 0x40) == 0) {
      if (((modelRuntime->classState).classStateEC & 0xc9) == 0) {
        iVar8 = modelRuntime->ownerArmyRuntime->factionIndex;
        FVar17 = g_GameFactionRuntimeImage.records[iVar8].secondaryArmyAssetCount;
        pdVar19 = g_GameFactionRuntimeImage.records[iVar8].secondaryArmyAssetPointersOrIds;
        if (FVar17 != 0) {
          do {
            if (((*(uint *)(*pdVar19 + 0x14) &
                 *(uint *)(modelRuntime->modelDefinition->reserved0C0_0DB + 4)) != 0) &&
               (uVar18 = *(uint *)(*pdVar19 + 0x28),
               uVar18 <= g_GameFactionRuntimeImage.records[iVar8].xeniteCurrentQ4)) {
              dVar14 = *pdVar19;
              g_GameFactionRuntimeImage.records[iVar8].xeniteCurrentQ4 =
                   g_GameFactionRuntimeImage.records[iVar8].xeniteCurrentQ4 - uVar18;
              uVar18 = *(uint *)(dVar14 + 0x24);
              dVar15 = *(dword *)(dVar14 + 0x2c);
              if ((g_UiCommandRuntimeFlags & 0x100000) != 0) {
                uVar18 = (uVar18 >> 4) + 1;
              }
              MVar13 = *(ModelRuntimeSlotLinkOrState4 *)(dVar14 + 8);
              (modelRuntime->classLinkState).classState68 = uVar18;
              (modelRuntime->classLinkState).classState74 = dVar15;
              (modelRuntime->classLinkState).modelLinkOrState60 = MVar13;
              pdVar3 = &(modelRuntime->classState).definitionDerivedValueF4;
              *pdVar3 = *pdVar3 + dVar15;
              (modelRuntime->classLinkState).classState64 = 0;
              pFVar4 = &g_GameFactionRuntimeImage.records[iVar8].secondaryArmyAssetCount;
              *pFVar4 = *pFVar4 - 1;
              do {
                *pdVar19 = pdVar19[1];
                pdVar19 = pdVar19 + 1;
                FVar17 = FVar17 - 1;
              } while (FVar17 != 0);
              (modelRuntime->classState).classStateB8 = 1;
              pdVar19 = &(modelRuntime->classState).classStateEC;
              *pdVar19 = *pdVar19 | 0x100;
              break;
            }
            pdVar19 = pdVar19 + 1;
            FVar17 = FVar17 - 1;
          } while (FVar17 != 0);
        }
      }
    }
    else if (((modelRuntime->classState).classStateEC & 9) == 0) {
      ArmyRuntime_UpdateTimedShotAndEffectEmitters(worldRuntime,modelRuntime);
      ArmyRuntime_UpdateAnimatedModelSubnodes(worldRuntime,modelRuntime);
    }
    break;
  case 1:
    if (((modelRuntime->classState).classStateEC & 9) == 0) {
      pdVar19 = &(modelRuntime->classLinkState).classState64;
      *pdVar19 = *pdVar19 + g_InGameSimulationStepTicks;
      ArmyRuntime_UpdateTimedShotAndEffectEmitters(worldRuntime,modelRuntime);
      uVar18 = (modelRuntime->classLinkState).classState64;
      ArmyRuntime_UpdateAnimatedModelSubnodes(worldRuntime,modelRuntime);
      if ((modelRuntime->classLinkState).classState68 <= uVar18) {
        MVar21 = ModelLookupTable_ContainsPackedKeyCf(1,5,(pMVar5->modelPayload).modelResource);
        if (!MVar21.carry) {
          MVar23 = ModelNodeRuntime_TransformLocalPointRegs(MVar21.entry,pMVar5);
          dVar15 = MVar23.ecx;
          dVar14 = MVar23.eax;
          MVar21 = ModelLookupTable_ContainsPackedKeyCf(0,5,(pMVar5->modelPayload).modelResource);
          if (!MVar21.carry) {
            MVar23 = ModelNodeRuntime_TransformLocalPointRegs(MVar21.entry,pMVar5);
            dVar14 = FixedMath_Atan2Angle16(dVar15 - MVar23.ecx,dVar14 - MVar23.eax);
            entityRuntime1 = modelRuntime->ownerArmyRuntime;
            AVar22 = ArmyRuntime_CreateInstanceFromAssetCf
                               (4,dVar14,MVar23.ecx,MVar23.eax,entityRuntime1->factionIndex,
                                (modelRuntime->classLinkState).modelLinkOrState60.classState,
                                worldRuntime);
            armySlot1 = (ArmyRuntimeSlot *)AVar22.eax;
            if (!AVar22.carry) {
              pFVar2 = &g_GameFactionRuntimeImage.records[entityRuntime1->factionIndex].
                        relationCounterA;
              *pFVar2 = *pFVar2 + 1;
              pMVar6 = modelRuntime->modelDefinition;
              armySlot1->movementStateFlags = armySlot1->movementStateFlags | 0x402;
              uVar18 = *(uint *)pMVar6->reserved26C_277;
              if (((uVar18 != 0) && (uVar18 < worldRuntime->dwordArrayCount)) &&
                 (worldRuntime->dwordArray != (dword *)0x0)) {
                pMVar5 = modelRuntime->rootModelNode;
                ppDVar12 = *(DirectSoundVoiceSet ***)
                            ((pMVar5->modelPayload).reserved2C_33 + uVar18 * 4 + -0x38);
                pGVar16 = &(pMVar5->worldTransform).translation;
                if ((ppDVar12 != (DirectSoundVoiceSet **)0x0) &&
                   (bVar20 = TerrainGrid_TestProjectedCellMaskBits01Cf
                                       ((pMVar5->worldTransform).translation.y,pGVar16->x,
                                        worldRuntime), !bVar20)) {
                  SpatialSound_PlayPositionedOneShot
                            (pMVar6->positionedSoundMaximumDistanceQ12,
                             pMVar6->positionedSoundGainQ15,pGVar16,ppDVar12);
                }
              }
              dVar14 = (modelRuntime->classLinkState).classState74;
              (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime = armySlot1;
              iVar8 = armySlot1->factionIndex;
              (modelRuntime->classState).classStateB8 = 2;
              (modelRuntime->classLinkState).classState74 = 0;
              pdVar19 = &(modelRuntime->classState).definitionDerivedValueF4;
              *pdVar19 = *pdVar19 - dVar14;
              piVar9 = (int *)(armySlot1->modelRuntimeOrSavedOffset).savedIdOrOffset;
              armySlot1->movementStateFlags = armySlot1->movementStateFlags | 2;
              piVar9[0x3c] = (int)modelRuntime;
              if (iVar8 == worldRuntime->activeFactionRuntimeIndex) {
                ArmyAssetRegistry_FindByIdCf
                          ((modelRuntime->classLinkState).modelLinkOrState60.classState);
                dVar14 = (worldRuntime->motion).pitchAngle;
                iVar8 = piVar9[1];
                iVar10 = *piVar9;
                iVar11 = *(int *)(iVar8 + 0x14);
                *(int *)(iVar10 + 0x1b0) = *(int *)(iVar10 + 0x1b0) + 1;
                notificationMovieId = *(InGameNotificationMovieId *)(iVar10 + 0x160);
                if (*(int *)(iVar10 + 0x1b0) != 1) {
                  notificationMovieId = *(InGameNotificationMovieId *)(iVar10 + 0x164);
                }
                InGameNotificationQueue_InsertPriorityRecord
                          (ARMY_CREATED,0,dVar14,iVar11 + 0x8800U & 0xffff,*(Q12 *)(iVar8 + 0x98),
                           *(Q12 *)(iVar8 + 0x94),2,notificationMovieId);
              }
            }
          }
        }
      }
    }
    break;
  case 2:
    pMVar5->primaryTextureOffsetV =
         pMVar5->primaryTextureOffsetV +
         *(int *)pMVar6->reserved00C_023 * g_InGameSimulationStepTicks;
    if (0x7ffff < pMVar5->primaryTextureOffsetV) {
      pMVar5->primaryTextureOffsetV = 0x80000;
      (modelRuntime->classState).classStateB8 = 3;
      MVar21 = ModelLookupTable_ContainsPackedKeyCf(1,5,(pMVar5->modelPayload).modelResource);
      if (!MVar21.carry) {
        entityRuntime1 = (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime;
        MVar23 = ModelNodeRuntime_TransformLocalPointRegs(MVar21.entry,pMVar5);
        targetWorldXQ12 = MVar23.ecx;
        pMVar7 = (entityRuntime1->modelRuntimeOrSavedOffset).modelRuntime;
        ArmyRuntime_StartMoveCommandWithAuxiliaryValues
                  ((modelRuntime->classLinkState).classState7C,
                   (modelRuntime->classLinkState).classState78,targetWorldXQ12,MVar23.eax,
                   (ArmyMovementRuntime *)entityRuntime1);
        (pMVar7->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime =
             (ArmyRuntimeSlot *)modelRuntime;
      }
    }
    break;
  case 3:
    entityRuntime1 = (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime;
    if ((entityRuntime1 == (ArmyRuntimeSlot *)0x0) ||
       ((ModelRuntimeUpdateView200 *)
        (((entityRuntime1->modelRuntimeOrSavedOffset).modelRuntime)->classState).
        linkedArmyRuntimeOrSavedOffset.armyRuntime != modelRuntime)) {
      pMVar6 = modelRuntime->modelDefinition;
      (modelRuntime->classState).classStateB8 = 4;
      (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime = (ArmyRuntimeSlot *)0x0;
      uVar18 = *(uint *)pMVar6->reserved26C_277;
      if ((uVar18 != 0) &&
         ((uVar18 < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != (dword *)0x0)))) {
        pMVar5 = modelRuntime->rootModelNode;
        ppDVar12 = *(DirectSoundVoiceSet ***)
                    ((pMVar5->modelPayload).reserved2C_33 + uVar18 * 4 + -0x38);
        pGVar16 = &(pMVar5->worldTransform).translation;
        if ((ppDVar12 != (DirectSoundVoiceSet **)0x0) &&
           (bVar20 = TerrainGrid_TestProjectedCellMaskBits01Cf
                               ((pMVar5->worldTransform).translation.y,pGVar16->x,worldRuntime),
           !bVar20)) {
          SpatialSound_PlayPositionedOneShot
                    (pMVar6->positionedSoundMaximumDistanceQ12,pMVar6->positionedSoundGainQ15,
                     pGVar16,ppDVar12);
        }
      }
    }
    break;
  case 4:
    pMVar5->primaryTextureOffsetV =
         pMVar5->primaryTextureOffsetV -
         *(int *)pMVar6->reserved00C_023 * g_InGameSimulationStepTicks;
    if (pMVar5->primaryTextureOffsetV < 1) {
      pMVar5->primaryTextureOffsetV = 0;
      (modelRuntime->classState).classStateB8 = 0;
      pdVar19 = &(modelRuntime->classState).classStateEC;
      *pdVar19 = *pdVar19 & 0xfffffeff;
    }
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
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
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClassUpdateSlot11_DispatchByClassId
          (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView200 *modelRuntime)

{
  dword *pdVar1;
  FactionArmyAssetCount *pFVar2;
  dword dVar3;
  ModelRuntimeSlotLinkOrState4 MVar4;
  int iVar5;
  dword dVar6;
  int iVar7;
  AngleTurn32 AVar8;
  ModelDefinitionRecordPrefix *modelDefinition1;
  FactionArmyAssetCount FVar9;
  uint uVar10;
  dword *pdVar11;
  ArmyRegistryEaxCf5_51b6d0 AVar12;
  ModelDefinitionLookupEaxCf5 MVar13;
  PckModelDefinitionIdCatalog notificationMovieId;
  ArmyRuntimeSlot *entityRuntime1;
  ModelRuntimeNode *modelNode1;
  
                    // WARNING: Switch is manually overridden
  switch((modelRuntime->classState).classStateB8) {
  case 0:
    if (((modelRuntime->classState).classStateEC & 0x40) == 0) {
      if (((modelRuntime->classState).classStateEC & 0xc9) == 0) {
        iVar5 = modelRuntime->ownerArmyRuntime->factionIndex;
        pdVar11 = g_GameFactionRuntimeImage.records[iVar5].secondaryArmyAssetPointersOrIds;
        for (FVar9 = g_GameFactionRuntimeImage.records[iVar5].secondaryArmyAssetCount; FVar9 != 0;
            FVar9 = FVar9 - 1) {
          dVar6 = *pdVar11;
          if (((*(uint *)(dVar6 + 0x14) & 0x10) != 0) &&
             (*(uint *)(dVar6 + 0x28) <= g_GameFactionRuntimeImage.records[iVar5].xeniteCurrentQ4))
          {
            g_GameFactionRuntimeImage.records[iVar5].xeniteCurrentQ4 =
                 g_GameFactionRuntimeImage.records[iVar5].xeniteCurrentQ4 - *(uint *)(dVar6 + 0x28);
            uVar10 = *(uint *)(dVar6 + 0x24);
            dVar3 = *(dword *)(dVar6 + 0x2c);
            if ((g_UiCommandRuntimeFlags & 0x100000) != 0) {
              uVar10 = (uVar10 >> 4) + 1;
            }
            MVar4 = *(ModelRuntimeSlotLinkOrState4 *)(dVar6 + 8);
            (modelRuntime->classLinkState).classState68 = uVar10;
            (modelRuntime->classLinkState).classState74 = dVar3;
            (modelRuntime->classLinkState).modelLinkOrState60 = MVar4;
            pdVar1 = &(modelRuntime->classState).definitionDerivedValueF4;
            *pdVar1 = *pdVar1 + dVar3;
            (modelRuntime->classLinkState).classState64 = 0;
            pFVar2 = &g_GameFactionRuntimeImage.records[iVar5].secondaryArmyAssetCount;
            *pFVar2 = *pFVar2 - 1;
            do {
              *pdVar11 = pdVar11[1];
              pdVar11 = pdVar11 + 1;
              FVar9 = FVar9 - 1;
            } while (FVar9 != 0);
            (modelRuntime->classState).classStateB8 = 1;
            pdVar11 = &(modelRuntime->classState).classStateEC;
            *pdVar11 = *pdVar11 | 0x100;
            break;
          }
          pdVar11 = pdVar11 + 1;
        }
      }
    }
    else if (((modelRuntime->classState).classStateEC & 9) == 0) {
      ArmyRuntime_UpdateTimedShotAndEffectEmitters(worldRuntime,modelRuntime);
      ArmyRuntime_UpdateAnimatedModelSubnodes(worldRuntime,modelRuntime);
    }
    break;
  case 1:
    if (((modelRuntime->classState).classStateEC & 9) == 0) {
      entityRuntime1 = modelRuntime->ownerArmyRuntime;
      pdVar11 = &(modelRuntime->classLinkState).classState64;
      *pdVar11 = *pdVar11 + g_InGameSimulationStepTicks;
      ArmyRuntime_UpdateTimedShotAndEffectEmitters(worldRuntime,modelRuntime);
      uVar10 = (modelRuntime->classLinkState).classState64;
      ArmyRuntime_UpdateAnimatedModelSubnodes(worldRuntime,modelRuntime);
      if ((modelRuntime->classLinkState).classState68 <= uVar10) {
        iVar5 = entityRuntime1->factionIndex;
        dVar6 = (modelRuntime->classLinkState).classState74;
        (modelRuntime->classLinkState).classState74 = 0;
        (modelRuntime->classState).classStateB8 = 0;
        pdVar11 = &(modelRuntime->classState).classStateEC;
        *pdVar11 = *pdVar11 & 0xfffffeff;
        pdVar11 = &(modelRuntime->classState).definitionDerivedValueF4;
        *pdVar11 = *pdVar11 - dVar6;
        AVar12 = ArmyAssetRegistry_FindByIdCf
                           ((modelRuntime->classLinkState).modelLinkOrState60.classState);
        (modelRuntime->classLinkState).modelLinkOrState60.modelRuntime = (ModelRuntimeSlot *)0x0;
        if (!AVar12.carry) {
          uVar10 = g_GameFactionRuntimeImage.records[iVar5].primaryArmyAssetCount;
          if (uVar10 < 0x40) {
            iVar7 = worldRuntime->activeFactionRuntimeIndex;
            *(ArmyAssetRecordPrefix **)(iVar5 * 0x740 + 0x50f520 + uVar10 * 4) = AVar12.eax;
            pFVar2 = &g_GameFactionRuntimeImage.records[iVar5].primaryArmyAssetCount;
            *pFVar2 = *pFVar2 + 1;
            if (iVar7 == entityRuntime1->factionIndex) {
              dVar6 = (AVar12.eax)->rootNodeOffsetOrPointer;
              UiCommandSpriteVariantA_RebuildGrid((UiNodeBase *)worldRuntime);
              MVar13 = ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                                 (entityRuntime1->factionIndex,dVar6);
              modelDefinition1 = MVar13.modelDefinition;
              modelNode1 = modelRuntime->rootModelNode;
              dVar6 = (worldRuntime->motion).pitchAngle;
              AVar8 = (modelNode1->modelPayload).worldRotationAngle2;
              modelDefinition1[0x24].byteSize = modelDefinition1[0x24].byteSize + 1;
              notificationMovieId = modelDefinition1[0x1d].flags;
              if (modelDefinition1[0x24].byteSize != 1) {
                notificationMovieId = modelDefinition1[0x1d].definitionId;
              }
              InGameNotificationQueue_InsertPriorityRecord
                        (ARMY_CREATED,0,dVar6,AVar8 + 0x1800 & 0xffff,
                         (modelNode1->worldTransform).translation.y,
                         (modelNode1->worldTransform).translation.x,3,notificationMovieId);
            }
          }
        }
      }
    }
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
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
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_UpdateGridBoundEffectsAndModels
          (WorldRuntimeContext *worldRuntime,ModelRuntimeClass14UpdateView200 *modelRuntime)

{
  int iVar1;
  dword in_EDX;
  int FVar3;
  FieldCellPackedFlagsAndMaterial FVar2;
  dword extraout_EDX;
  FieldGridCoordinatesEaxEdx8 FVar4;
  FieldGridAsset *fieldGrid1;
  
  if ((1 < (int)modelRuntime->definitionValue60_3C) &&
     (((modelRuntime->classState).classStateEC & 9) == 0)) {
    FVar4 = FieldGrid_WorldToGridQ12
                      ((modelRuntime->rootModelNode->worldTransform).translation.y,
                       (modelRuntime->rootModelNode->worldTransform).translation.x);
    iVar1 = (FVar4.columnQ12 >> 0xb) + 1 >> 1;
    FVar3 = (FVar4.rowQ12 >> 0xb) + 1 >> 1;
    fieldGrid1 = worldRuntime->fieldGrid;
    if ((0 < iVar1) && (0 < FVar3)) {
      if ((iVar1 + 1 < (int)fieldGrid1->gridWidth) && (FVar3 + 1 < (int)fieldGrid1->gridHeight)) {
        iVar1 = FVar3 * fieldGrid1->gridWidth + iVar1;
        FVar2 = 0x800 << ((byte)modelRuntime->modelDefinition->resourceFieldSupportSelectorC0 & 0x1f
                         );
        fieldGrid1->cells[iVar1].runtime7C =
             modelRuntime->ownerArmyRuntime->factionIndex << 0xd | FVar2 |
             *(int *)&modelRuntime->modelDefinition->field_0xc8 << 0x18;
        if ((fieldGrid1->cells[iVar1].flagsAndMaterial & FVar2) != 0) {
          fieldGrid1->cells[iVar1].armyRuntimeSavedOffset6C =
               (int)modelRuntime - g_ModelRuntimeRebaseDelta;
          ArmyRuntime_UpdateTimedShotAndEffectEmitters
                    (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
          ArmyRuntime_UpdateAnimatedModelSubnodes
                    (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
        }
      }
    }
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
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
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_ClassCommandHandlerGroupACf
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ModelRuntimeNode *pMVar1;
  ArmyRuntimeSlot *pAVar2;
  longlong lVar3;
  longlong lVar4;
  dword dVar5;
  byte *pbVar6;
  int iVar7;
  ModelRuntimeNode *modelNode1;
  bool bVar8;
  EffectDefinitionLookupEaxCf5 EVar9;
  DamageAmount32 damageAmount;
  int iVar10;
  int iVar11;
  ArmyRuntimeSlot *armySlot1;
  
  modelNode1 = (ModelRuntimeNode *)worldRuntime->ownerListHead;
  pMVar1 = armyRuntime->modelNodeRuntime;
  if ((modelNode1 != (ModelRuntimeNode *)0x0) && ((g_UiCommandRuntimeFlags & 4) == 0)) {
    do {
      if (modelNode1->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
        armySlot1 = (modelNode1->runtimePayload).armyRuntime;
        if ((pMVar1 != modelNode1) &&
           ((((armySlot1->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C == 0 ||
            (((armySlot1->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C == 0xc))))
        {
          bVar8 = ArmyRuntime_TestClass13ProximityCandidateCf(armySlot1,armyRuntime);
          if (!bVar8) {
            bVar8 = ArmyRuntime_TestModelAttachmentProximityCf(armySlot1,armyRuntime);
            if (bVar8)
            goto 
            ArmyRuntime_ClassCommandHandlerGroupACf_AdvanceOwnerScanAfterProximityOrImpactDecision;
          }
          damageAmount = 0x100000;
          dVar5 = FixedMath_Atan2Angle16
                            ((modelNode1->worldTransform).translation.y -
                             (pMVar1->worldTransform).translation.y,
                             (modelNode1->worldTransform).translation.x -
                             (pMVar1->worldTransform).translation.x);
          ArmyRuntime_ApplyImpactDamageAndFinalizeState(dVar5,damageAmount,armySlot1);
        }
      }
ArmyRuntime_ClassCommandHandlerGroupACf_AdvanceOwnerScanAfterProximityOrImpactDecision:
      modelNode1 = (ModelRuntimeNode *)(modelNode1->common).nextNode;
    } while (modelNode1 != (ModelRuntimeNode *)0x0);
  }
  modelNode1 = armyRuntime->modelNodeRuntime;
  if (((((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->classLinkState).classState68 & 0x20
      ) == 0) {
    FieldGrid_ApplyHeightAtWorldPointAndRefreshNeighborsCf
              ((((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->classState).classStateDC,
               (modelNode1->worldTransform).translation.z,(modelNode1->worldTransform).translation.y
               ,(modelNode1->worldTransform).translation.x,worldRuntime->fieldGrid);
  }
  modelNode1 = (ModelRuntimeNode *)worldRuntime->ownerListHead;
  pMVar1 = (armyRuntime->linkedEntityRuntime->common).ownership.modelNode;
  if (modelNode1 != (ModelRuntimeNode *)0x0) {
    armySlot1 = (ArmyRuntimeSlot *)0x0;
    iVar10 = 0;
    do {
      iVar11 = iVar10;
      if ((((modelNode1->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) && (modelNode1 != pMVar1)) &&
          (dVar5 = ((((modelNode1->runtimePayload).armyRuntime)->modelRuntimeOrSavedOffset).
                   modelRuntime)->attachments140[2].reserved1C,
          (armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex ==
          (((modelNode1->runtimePayload).armyRuntime)->linkedEntityRuntime->common).ownership.
          ownerIndex)) && (dVar5 != 0)) {
        pbVar6 = ((((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->attachments140[3].
                  parentModelNodeOrSavedOffset08)->modelPayload).reserved2C_33 + (dVar5 - 0x38);
        iVar7 = (pMVar1->worldTransform).translation.x - (modelNode1->worldTransform).translation.x;
        lVar4 = (longlong)(int)pbVar6 * (longlong)(int)pbVar6 - (longlong)iVar7 * (longlong)iVar7;
        if ((-1 < lVar4) &&
           (iVar7 = (pMVar1->worldTransform).translation.y -
                    (modelNode1->worldTransform).translation.y,
           lVar3 = (longlong)iVar7 * (longlong)iVar7,
           -1 < (int)(((int)((ulonglong)lVar4 >> 0x20) - (int)((ulonglong)lVar3 >> 0x20)) -
                     (uint)((uint)lVar4 < (uint)lVar3)))) {
          pAVar2 = (modelNode1->runtimePayload).armyRuntime;
          iVar11 = iVar10 + 1;
          if ((((pAVar2->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C == 0x12) &&
             ((pAVar2->runtimeFlags & 0x18) == 0)) {
            iVar11 = iVar10;
            armySlot1 = pAVar2;
          }
        }
      }
      modelNode1 = (ModelRuntimeNode *)(modelNode1->common).nextNode;
      iVar10 = iVar11;
    } while (modelNode1 != (ModelRuntimeNode *)0x0);
    if ((iVar11 == 0) && (armySlot1 != (ArmyRuntimeSlot *)0x0)) {
      modelNode1 = armySlot1->modelNodeRuntime;
      armySlot1->runtimeFlags = armySlot1->runtimeFlags | 8;
      EVar9 = EffectDefinitionRegistry_FindByIdWithErrorCf
                        (*(PckEffectDefinitionIdCatalog *)
                          (((armySlot1->modelRuntimeOrSavedOffset).modelRuntime)->classState).
                          reservedC4_C7);
      if (!EVar9.carry) {
        EffectRuntimePool_CreateInstanceFromDefinitionCf
                  (EFFECT_RUNTIME_COMPLETION_SPAWN_ARMY_FROM_MODEL,
                   (EffectRuntimeOwnerReference4)armySlot1->linkedEntityRuntime,
                   (modelNode1->modelPayload).worldRotationAngle2,
                   (modelNode1->modelPayload).worldRotationAngle1,
                   (modelNode1->modelPayload).worldRotationAngle0,
                   (modelNode1->worldTransform).translation.z,
                   (modelNode1->worldTransform).translation.y,
                   (modelNode1->worldTransform).translation.x,EVar9.definitionOrError,worldRuntime);
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
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeMaintenance_InitializeOccupancyAndStateTint
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
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeMaintenance_DispatchClassMethodDRecursive
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
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeMaintenance_UpdateHierarchyAiAndTimers
          (WorldRuntimeContext *worldRuntime,WorldOwnerListNode100 *ownerNode)

{
  ArmyCommandGeneration *pAVar1;
  ModelRuntimeSlot *modelRuntime;
  ArmyRuntimeSlot *armyRuntime;
  int iVar2;
  
  modelRuntime = ownerNode->runtimePayload;
  armyRuntime = (modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime;
  iVar2 = armyRuntime->factionIndex;
  ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive(worldRuntime,modelRuntime);
  if (iVar2 != 0) {
    AiCombatDecision_UpdateTargetAssignment(worldRuntime,armyRuntime);
  }
  pAVar1 = &armyRuntime->commandGeneration;
  *pAVar1 = *pAVar1 - g_InGameSimulationStepTicks;
  if ((int)*pAVar1 < 0) {
    armyRuntime->commandModeFlags = armyRuntime->commandModeFlags & 0xfffffff3;
  }
  if (((armyRuntime->movementStateFlags & 2) != 0) &&
     ((modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime ==
      (ArmyRuntimeSlot *)0x0)) {
    armyRuntime->movementStateFlags = armyRuntime->movementStateFlags & 0xfffffffd;
  }
  if (armyRuntime->runtimeStateA4 != 0) {
    armyRuntime->runtimeStateA4 = armyRuntime->runtimeStateA4 - 1;
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
ArmyRuntimeInitEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ArmyRuntime_InitializePoolAndGraphicsCf(void *ownerContext,word *graphicsBasePath)

{
  word wVar1;
  uint uVar2;
  ArmyAssetRecordPrefix *pAVar3;
  ArmyRuntimeSlot *armySlot1;
  GraphicsPaletteAsset *pGVar4;
  GraphicsTextureSourceAsset *arg0;
  GraphicsTextureSet *pGVar5;
  GraphicsPixelDimension previewHeight;
  int iVar6;
  int iVar7;
  int frontendPlayerRuntimeId;
  ArmyAssetRecordPrefix **ppAVar8;
  word *pwVar9;
  word *pwVar10;
  ArenaAllocEaxCf5 AVar11;
  PackageLoadEntryEaxCf5 PVar12;
  GraphicsTextureSetEaxCf5 GVar13;
  ArmyRuntimeInitEaxCf5 AVar14;
  ArmyPreviewTextureEaxCf5 AVar15;
  ArmyRuntimeInitEaxCf5 AVar16;
  
  AVar11 = (*g_MemoryApi.alloc)(0x48000);
  armySlot1 = (ArmyRuntimeSlot *)AVar11.eax;
  if (!AVar11.carry) {
    g_ArmyRuntimeRebaseBaseMinusOne = (void *)((int)&armySlot1[-1].selectionMetric5 + 3);
    g_ArmyRuntimeSlots = armySlot1;
    for (iVar6 = 0x12000; iVar6 != 0; iVar6 = iVar6 + -1) {
      (armySlot1->modelRuntimeOrSavedOffset).modelRuntime = (ModelRuntimeSlot *)0x0;
      armySlot1 = (ArmyRuntimeSlot *)&armySlot1->modelNodeRuntime;
    }
    frontendPlayerRuntimeId = 0;
    iVar6 = 0x20;
    pwVar9 = graphicsBasePath;
    do {
      pwVar10 = pwVar9;
      if (iVar6 == 0) break;
      iVar6 = iVar6 + -1;
      pwVar10 = pwVar9 + 1;
      wVar1 = *pwVar9;
      pwVar9 = pwVar10;
    } while (wVar1 != 0);
    iVar6 = 8;
    pwVar10 = pwVar10 + -1;
    g_MoviePlaybackScheduleSpan = 0x1a;
    do {
      MoviePlayback_AdvanceScheduledFrameAndTick();
      iVar7 = 0x30;
      if (frontendPlayerRuntimeId == 0) {
ArmyRuntime_InitializePoolAndGraphicsCf_LoadCurrentFactionGraphicsPackageAndBindings:
        *(int *)pwVar10 = iVar7;
        WidePath_SetExtensionCode(0x786667,graphicsBasePath);
        PVar12 = Package_LoadEntry(graphicsBasePath);
        arg0 = PVar12.bufferOrError;
        if (PVar12.carry) {
          return (ArmyRuntimeInitEaxCf5)PVar12;
        }
        ArmyGraphics_CopyFrontendPlayerPaletteAndTexture
                  (frontendPlayerRuntimeId,(ArmyGraphicsAssetAddress32)arg0);
        GVar13 = (*g_GraphicsCreateTextureSet)(arg0);
        pGVar5 = GVar13.textureSet;
        if (GVar13.carry) {
          LOCK();
          UNLOCK();
          Resource_Release(arg0);
          AVar14.carry = true;
          AVar14.errorOrValue = (dword)pGVar5;
          return AVar14;
        }
        MoviePlayback_AdvanceScheduledFrameAndTick();
        g_ArmyGraphicsBindings[frontendPlayerRuntimeId].textureSet = pGVar5;
        WidePath_SetExtensionCode(0x6c6170,graphicsBasePath);
        AVar14 = (ArmyRuntimeInitEaxCf5)(*g_GraphicsPaletteAssetLoadPackage)(graphicsBasePath);
        pGVar4 = (GraphicsPaletteAsset *)AVar14.errorOrValue;
        if (AVar14.carry) {
          return AVar14;
        }
        g_ArmyGraphicsBindings[frontendPlayerRuntimeId].paletteAsset = pGVar4;
      }
      else {
        pGVar4 = (GraphicsPaletteAsset *)(frontendPlayerRuntimeId * 0x740);
        MoviePlayback_AdvanceScheduledFrameAndTick();
        if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[frontendPlayerRuntimeId] != 0) {
          uVar2 = *(uint *)(pGVar4[0x27da].reserved08_AF + 0xa0);
          g_MoviePlaybackScheduleCounter = g_MoviePlaybackScheduleCounter + -1;
          if (uVar2 < 10) {
            iVar7 = uVar2 + 0x30;
          }
          else {
            iVar7 = uVar2 + 0x37;
          }
          goto ArmyRuntime_InitializePoolAndGraphicsCf_LoadCurrentFactionGraphicsPackageAndBindings;
        }
      }
      frontendPlayerRuntimeId = frontendPlayerRuntimeId + 1;
      MoviePlayback_AdvanceScheduledFrameAndTick();
      iVar6 = iVar6 + -1;
    } while (iVar6 != 0);
    pwVar10[0] = 0;
    pwVar10[1] = 0;
    ppAVar8 = g_ArmyAssetRecordRegistry;
    iVar6 = 0x300;
    do {
      pAVar3 = *ppAVar8;
      if ((pAVar3 != (ArmyAssetRecordPrefix *)0x0) &&
         ((pAVar3[1].selectionDetailTemplateVariantIndex & 0xfe) != 0)) {
        AVar15 = ArmyRuntime_RenderPreviewTextureCf
                           (g_InGamePanelTextureSubresource34Height,
                            g_InGamePanelTextureSubresource34Width,
                            *(FactionRuntimeIndex *)((int)ownerContext + 0x50),pAVar3->registryId,
                            ownerContext);
        pGVar4 = (GraphicsPaletteAsset *)AVar15.previewTexture;
        if (!AVar15.carry) {
          pAVar3[1].rootNodeOffsetOrPointer = (dword)pGVar4;
          previewHeight =
               (GraphicsPixelDimension)
               ((ulonglong)(longlong)g_InGamePanelTextureSubresource02Width / 3);
          AVar15 = ArmyRuntime_RenderPreviewTextureCf
                             (previewHeight,previewHeight,
                              *(FactionRuntimeIndex *)((int)ownerContext + 0x50),pAVar3->registryId,
                              ownerContext);
          pGVar4 = (GraphicsPaletteAsset *)AVar15.previewTexture;
          if (!AVar15.carry) {
            pAVar3[1].registryId = (PckArmyAssetIdCatalog)pGVar4;
          }
        }
      }
      ppAVar8 = ppAVar8 + 1;
      iVar6 = iVar6 + -1;
    } while (iVar6 != 0);
    AVar11.carry = false;
    AVar11.eax = (dword)pGVar4;
  }
  AVar16.errorOrValue = AVar11.eax;
  AVar16.carry = AVar11.carry;
  return AVar16;
}


/* Address: 0x00528330.
   Ownership: gameplay/army/runtime.
   Purpose: Binary entry is anchored by g_CodePointerTable_0051FC98[12]@0051FC98. Runtime-update partition slots
   0-23 receive (worldRuntime, armyRuntime).
   Local calls: ArmyRuntime_UpdateTimedShotAndEffectEmitters.
   Cross-module calls: ModelRuntimePool_DestroyHierarchyAndDetach [world/model/runtime],
   ModelNodeRuntime_RebuildTransformsFromRoot [world/model/hierarchy].
*/

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_UpdateEffectsAndDestroyModelHierarchy
          (WorldRuntimeContext *worldRuntime,ModelRuntimeDestroyEffectsView200 *modelRuntime)

{
  ModelRuntimeNode *modelNodeRuntime;
  ModelResourceHitTestAndRenderView210 *pMVar1;
  int verticalStepQ12;
  int modelHeightQ12;
  GraphicsWorldCoordinateQ12 *worldTranslationZQ12Field;
  ModelResourceHitTestAndRenderView210 *modelResource;
  int localBoundsZ1Q12;
  int remainingClassDistanceQ12;
  
  ArmyRuntime_UpdateTimedShotAndEffectEmitters
            (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
  modelNodeRuntime = modelRuntime->rootModelNode;
  pMVar1 = (modelNodeRuntime->modelPayload).modelResource;
  verticalStepQ12 =
       modelRuntime->modelDefinition->verticalTranslationStepQ12PerTick0C *
       g_InGameSimulationStepTicks;
  worldTranslationZQ12Field = &(modelNodeRuntime->worldTransform).translation.z;
  *worldTranslationZQ12Field = *worldTranslationZQ12Field + verticalStepQ12;
  localBoundsZ1Q12 = pMVar1->localBoundsZ1Q12;
  (modelRuntime->classLinkState).modelLinkOrState60.modelRuntime =
       (ModelRuntimeSlot *)
       ((int)(modelRuntime->classLinkState).modelLinkOrState60.modelRuntime - verticalStepQ12);
  modelHeightQ12 = localBoundsZ1Q12 - pMVar1->localBoundsZ0Q12;
  remainingClassDistanceQ12 = (modelRuntime->classLinkState).modelLinkOrState60.signedScalarState;
  modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
  if (modelHeightQ12 < remainingClassDistanceQ12) {
    ModelRuntimePool_DestroyHierarchyAndDetach(worldRuntime,(ModelRuntimeSlot *)modelRuntime);
  }
  else {
    ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
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
void __thandor_preserve_eax_edx
ArmyRuntimeNode_RebuildTerrainOccupancyAndVisualStateCallback
          (WorldRuntimeContext *armyContext,WorldOwnerListNode100 *node)

{
  if (node->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
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
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_SetNonzeroActionVector
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
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_ResolveCommandTarget(ArmyRuntimeSlot *targetArmyRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ArmyCommandGeneration AVar1;
  ArmyCommandGeneration resolvedCommandGeneration;
  
  AVar1 = g_ArmyCommandGenerationStandard;
  if ((armyRuntime->movementStateFlags & 0x20) != 0) {
    if ((armyRuntime->commandModeFlags & 8) == 0) {
      ArmyRuntime_ResetMovementStatePreserveQueuedTarget((ArmyMovementRuntime *)armyRuntime);
    }
    else {
      ArmyRuntime_ResetMovementStateFromCurrentPosition((ArmyMovementRuntime *)armyRuntime);
    }
  }
  if (targetArmyRuntime == (ArmyRuntimeSlot *)0x0) {
    armyRuntime->commandModeFlags = 0;
    armyRuntime->commandGeneration = 0;
  }
  else {
    armyRuntime->commandModeFlags = 1;
    armyRuntime->commandGeneration = AVar1;
  }
  armyRuntime->commandTargetArmyRuntime = targetArmyRuntime;
  return;
}


/* Address: 0x0051C620.
   Ownership: gameplay/army/runtime.
   Purpose: The three Q12 command coordinates and command-generation state map to
   GameEntityRuntimeCommon.commandTarget.
   Cross-module calls: ArmyRuntime_ResetMovementStatePreserveQueuedTarget [gameplay/army/movement],
   ArmyRuntime_ResetMovementStateFromCurrentPosition [gameplay/army/movement].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_ApplyTargetPositionCommand
          (Q12 coordinateA,Q12 coordinateB,Q12 coordinateC,ArmyRuntimeSlot *armyRuntime)

{
  ArmyCommandGeneration commandGeneration;
  
  if ((armyRuntime->movementStateFlags & 0x20) != 0) {
    if ((armyRuntime->commandModeFlags & 8) == 0) {
      ArmyRuntime_ResetMovementStatePreserveQueuedTarget((ArmyMovementRuntime *)armyRuntime);
    }
    else {
      ArmyRuntime_ResetMovementStateFromCurrentPosition((ArmyMovementRuntime *)armyRuntime);
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
WorldPositionEaxEcxEdxCf13
ArmyRuntime_ResolveShotAimPointCf
          (Q12 sourceWorldZQ12,Q12 sourceWorldYQ12,Q12 sourceWorldXQ12,
          ShotDefinition *shotDefinition,GameEntityRuntime *targetState)

{
  int *piVar1;
  int iVar2;
  longlong lVar3;
  longlong lVar4;
  uint in_EAX;
  dword dVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int in_ECX;
  dword dVar9;
  int iVar10;
  int *in_EDX;
  int iVar11;
  ModelRuntimeNode *modelNode1;
  ShotModeRangeLimitEbxCf5 SVar12;
  FixedDirectionXyzRegs12 FVar13;
  WorldPositionEaxEcxEdxCf13 WVar14;
  WorldPositionEaxEcxEdxCf13 WVar15;
  GameEntityRuntime *entityRuntime1;
  
  if (((targetState->common).commandTarget.targetFlags & 1) == 0) {
    if (((targetState->common).commandTarget.targetFlags & 2) != 0) {
      iVar8 = (targetState->common).commandTarget.targetWorldXQ12;
      iVar10 = (targetState->common).commandTarget.targetWorldYQ12;
      iVar11 = (targetState->common).commandTarget.targetWorldZQ12;
      goto ArmyRuntime_ResolveShotAimPointCf_ReturnResolvedWorldXZWithCarryClear;
    }
  }
  else {
    entityRuntime1 = (targetState->common).commandTarget.targetEntity;
    in_EAX = 2;
    in_ECX = (targetState->common).ownership.ownerIndex * 2;
    if (entityRuntime1 != (GameEntityRuntime *)0x0) {
      in_EAX = 2 << ((byte)in_ECX & 0x1f);
      in_EDX = (entityRuntime1->common).ownership.definitionOrClassRecord;
      modelNode1 = (entityRuntime1->common).ownership.modelNode;
      in_ECX = *in_EDX;
      if ((*(uint *)((entityRuntime1->common).damageState.reserved0C_23 + 0x10) & in_EAX) != 0) {
        if (*(int *)(in_ECX + 0x4c) == 0x15) {
          modelNode1 = modelNode1->childNodes[0];
        }
        iVar8 = (modelNode1->worldTransform).translation.x;
        piVar1 = (entityRuntime1->common).ownership.definitionOrClassRecord;
        iVar7 = *piVar1;
        iVar10 = (modelNode1->worldTransform).translation.y;
        iVar11 = (modelNode1->worldTransform).translation.z + *(int *)(iVar7 + 0x50);
        iVar2 = piVar1[2];
        if ((*(int *)(iVar7 + 0x18) != 0) && ((*(uint *)(iVar2 + 0x18) & 4) == 0)) {
          dVar5 = FixedMath_Length3(iVar11 - sourceWorldZQ12,iVar10 - sourceWorldYQ12,
                                    iVar8 - sourceWorldXQ12);
          SVar12 = ShotDefinition_GetModeRangeLimitEbx(shotDefinition);
          iVar6 = (int)(((longlong)(int)dVar5 * (longlong)*(int *)(iVar7 + 0xc)) /
                       (longlong)SVar12.rangeLimitQ12);
          dVar5 = ShotDefinition_ComputeMode3LeadAdjustment(shotDefinition);
          iVar6 = iVar6 + dVar5 * *(int *)(iVar7 + 0xc);
          dVar5 = FixedMath_Atan2Angle16
                            (*(int *)(iVar2 + 0x5c) - *(int *)(*(int *)(iVar2 + 4) + 0x98),
                             *(int *)(iVar2 + 0x58) - *(int *)(*(int *)(iVar2 + 4) + 0x94));
          FVar13 = FixedMath_DirectionFromAnglesScaledRegs(0,dVar5,iVar6);
          dVar9 = FVar13.ecx;
          dVar5 = FVar13.eax;
          entityRuntime1 = (targetState->common).commandTarget.targetEntity;
          iVar7 = (entityRuntime1->common).damageState.trackedCoordinate0Q12;
          if (iVar7 == (entityRuntime1->common).pathCoordinate0Q12) {
            iVar7 = iVar7 - (modelNode1->worldTransform).translation.x;
            lVar4 = ((longlong)(int)dVar9 * (longlong)(int)dVar9 +
                    (longlong)(int)dVar5 * (longlong)(int)dVar5) - (longlong)iVar7 * (longlong)iVar7
            ;
            if (((-1 < lVar4) &&
                (iVar7 = (entityRuntime1->common).damageState.trackedCoordinate1Q12,
                iVar7 == (entityRuntime1->common).pathCoordinate1Q12)) &&
               (iVar7 = iVar7 - (modelNode1->worldTransform).translation.y,
               lVar3 = (longlong)iVar7 * (longlong)iVar7,
               -1 < (int)(((int)((ulonglong)lVar4 >> 0x20) - (int)((ulonglong)lVar3 >> 0x20)) -
                         (uint)((uint)lVar4 < (uint)lVar3)))) {
              iVar8 = (entityRuntime1->common).pathCoordinate0Q12;
              iVar10 = (entityRuntime1->common).pathCoordinate1Q12;
              iVar11 = (((entityRuntime1->common).ownership.modelNode)->worldTransform).translation.
                       z + *(int *)(*(int *)(entityRuntime1->common).ownership.
                                            definitionOrClassRecord + 0x50);
              goto ArmyRuntime_ResolveShotAimPointCf_ReturnResolvedWorldXZWithCarryClear;
            }
          }
          iVar11 = FVar13.edx + iVar11;
          iVar10 = dVar9 + iVar10;
          iVar8 = dVar5 + iVar8;
        }
ArmyRuntime_ResolveShotAimPointCf_ReturnResolvedWorldXZWithCarryClear:
        WVar14.worldYQ12 = iVar10;
        WVar14.worldXQ12 = iVar8;
        WVar14.worldZQ12 = iVar11;
        WVar14.carry = false;
        return WVar14;
      }
      (targetState->common).commandTarget.targetEntity = (GameEntityRuntime *)0x0;
      (targetState->common).commandTarget.targetFlags = 0;
    }
  }
  WVar15.worldYQ12 = in_ECX;
  WVar15.worldXQ12 = in_EAX;
  WVar15.carry = true;
  WVar15.worldZQ12 = (Q12)in_EDX;
  return WVar15;
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
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeNode_AccumulateTerrainOcclusionAndOccupancyCallback
          (WorldRuntimeContext *worldRuntime,WorldOwnerListNode100 *node)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  ulonglong uVar4;
  ulonglong uVar5;
  Q12 worldXQ12;
  Q12 worldYQ12;
  int occupancyByteOffset;
  FieldGridAsset *fieldGrid;
  
  if (node->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
    worldYQ12 = node->worldXQ12;
    iVar1 = *(int *)((int)node->runtimePayload + 8);
    worldXQ12 = node->worldYQ12;
    if (*(int *)(iVar1 + 0xc) != 0) {
      occupancyByteOffset = *(int *)(iVar1 + 0xc);
      fieldGrid = worldRuntime->fieldGrid;
      uVar2 = g_GameFactionRuntimeImage.records[occupancyByteOffset].packedRelationStates;
      uVar5 = (ulonglong)*(uint *)(iVar1 + 0x9c);
      uVar4 = 0;
      if ((uVar2 & 0x80000000) != 0) {
        uVar4 = uVar5;
      }
      uVar4 = uVar4 << 8;
      if ((uVar2 & 0x8000000) != 0) {
        uVar4 = uVar4 | uVar5;
      }
      uVar4 = uVar4 << 8;
      if ((uVar2 & 0x800000) != 0) {
        uVar4 = uVar4 | uVar5;
      }
      uVar4 = uVar4 << 8;
      if ((uVar2 & 0x80000) != 0) {
        uVar4 = uVar4 | uVar5;
      }
      uVar4 = uVar4 << 8;
      if ((uVar2 & 0x8000) != 0) {
        uVar4 = uVar4 | uVar5;
      }
      uVar4 = uVar4 << 8;
      if ((uVar2 & 0x800) != 0) {
        uVar4 = uVar4 | uVar5;
      }
      iVar3 = worldRuntime->activeFactionRuntimeIndex;
      uVar4 = uVar4 << 8;
      if ((uVar2 & 0x80) != 0) {
        uVar4 = uVar4 | uVar5;
      }
      TerrainProjectedOcclusion_AccumulateMaskAroundWorldPoint
                (uVar4 << 8,*(FieldGridRadiusUnits *)(iVar1 + 0x44),
                 node->worldZQ12 + *(int *)(iVar1 + 0x48),worldXQ12,worldYQ12,
                 worldRuntime->fieldGrid);
      if ((uVar2 >> ((byte)(iVar3 << 2) & 0x1f) & 8) != 0) {
        TerrainOccupancyBit2_MarkAroundWorldPoint
                  (*(FieldGridRadiusUnits *)(iVar1 + 0x90),worldXQ12,worldYQ12,occupancyByteOffset,
                   fieldGrid);
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
bool __thandor_cf_preserve_eax_ecx_edx
ArmyRuntime_TestStateField100ZeroCf(ArmyRuntimeSlot *runtimeState)

{
  return runtimeState->stateOrTechnologyId == 0;
}


/* Address: 0x0051D330.
   Ownership: gameplay/army/runtime.
   Purpose: Tests whether the signed state field at army-runtime offset 0x100 is nonnegative; CF carries the
   result.
*/
bool __thandor_cf_preserve_eax_ecx_edx
ArmyRuntime_TestStateField100NonnegativeCf(ArmyRuntimeSlot *armyRuntime)

{
  return -1 < armyRuntime->stateOrTechnologyId;
}


/* Address: 0x0051D350.
   Ownership: gameplay/army/runtime.
   Purpose: Loads the pointed runtime node, selects one callback through the exact 0x0051FED8 type-index table, and
   forwards the node plus caller argument. Placement-validation partition slots 24-47 receive (worldRuntime,
   armyRuntime), with CF carrying acceptance.
*/
bool __thandor_cf_preserve_eax_edx
ArmyRuntimeNode_DispatchTypedCallback
          (ArmyRuntimeSlot **modelRuntimeHolder,WorldRuntimeContext *worldRuntime)

{
  bool bVar1;
  
  bVar1 = (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.placementValidationCf
            [((ModelRuntimePlacementValidationView200 *)*modelRuntimeHolder)->modelDefinition->
             runtimeClassId4C])
                    (worldRuntime,(ModelRuntimePlacementValidationView200 *)*modelRuntimeHolder);
  return bVar1;
}


/* Address: 0x0051D4D0.
   Ownership: gameplay/army/runtime.
   Purpose: Dispatches a two-argument army runtime command through the verified runtime-class handler table indexed
   by the root model definition class. Dispatch wrapper for the typed twenty-four-entry class-command table.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_DispatchClassCommand
          (ArmyRuntimeSlot **armyRuntimeHolder,WorldRuntimeContext *worldRuntime)

{
  (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand
    [(((*armyRuntimeHolder)->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C])
            (worldRuntime,*armyRuntimeHolder);
  return;
}


/* Address: 0x0051D8C0.
   Ownership: gameplay/army/runtime.
   Purpose: Frees the army runtime pool, releases all eight texture/palette binding pairs, frees the two verified
   owned allocations for every populated army registry entry, and clears the registry pointers.
*/
void __thandor_void_preserve_eax_ecx_edx ArmyRuntime_ShutdownPoolAndGraphics(void)

{
  ArmyAssetRecordPrefix *pAVar1;
  int iVar2;
  ArmyGraphicsBinding *graphicsBindingCursor;
  ArmyAssetRecordPrefix **assetRegistryCursor;
  
  (*g_MemoryApi.free)(g_ArmyRuntimeSlots);
  g_ArmyRuntimeSlots = (ArmyRuntimeSlot *)0x0;
  graphicsBindingCursor = g_ArmyGraphicsBindings;
  iVar2 = 8;
  do {
    if (graphicsBindingCursor->textureSet != (GraphicsTextureSet *)0x0) {
      (*g_GraphicsTextureSetReleasePackageCf)(graphicsBindingCursor->textureSet);
      graphicsBindingCursor->textureSet = (GraphicsTextureSet *)0x0;
    }
    if (graphicsBindingCursor->paletteAsset != (GraphicsPaletteAsset *)0x0) {
      (*g_GraphicsPaletteAssetLifecycleCallbacks3.releasePackage)
                (graphicsBindingCursor->paletteAsset);
      graphicsBindingCursor->paletteAsset = (GraphicsPaletteAsset *)0x0;
    }
    graphicsBindingCursor = graphicsBindingCursor + 1;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  assetRegistryCursor = g_ArmyAssetRecordRegistry;
  iVar2 = 0x300;
  do {
    pAVar1 = *assetRegistryCursor;
    if (pAVar1 != (ArmyAssetRecordPrefix *)0x0) {
      (*g_MemoryApi.free)((void *)pAVar1[1].rootNodeOffsetOrPointer);
      (*g_MemoryApi.free)((void *)pAVar1[1].registryId);
      *assetRegistryCursor = (ArmyAssetRecordPrefix *)0x0;
    }
    assetRegistryCursor = assetRegistryCursor + 1;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  return;
}


/* Address: 0x0051D960.
   Ownership: gameplay/army/runtime.
   Purpose: Traverses 1024 exact 0x120-byte slots. Populated slots convert four verified runtime pointers to
   serialized offsets; empty slots are zeroed. Returns the pool base in EAX and exact byte size 0x48000 in EDX.
*/
RuntimeImagePointerByteSizeEdxEax8 __cdecl ArmyRuntimePool_ConvertPointersToOffsetsForSaveRegs(void)

{
  dword dVar1;
  ModelRuntimeSlot *pMVar2;
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
        (armySlot2->modelRuntimeOrSavedOffset).modelRuntime = (ModelRuntimeSlot *)0x0;
        armySlot2 = (ArmyRuntimeSlot *)&armySlot2->modelNodeRuntime;
      }
      iVar4 = iVar4 + -1;
      if (iVar4 == 0) {
        return CONCAT44(0x48000,g_ArmyRuntimeSlots);
      }
    }
    pMVar2 = (ModelRuntimeSlot *)
             ((int)(armySlot2->modelRuntimeOrSavedOffset).modelRuntime - g_ModelRuntimeRebaseDelta);
    if (armySlot1 != (ArmyRuntimeSlot *)0x0) {
      armySlot1 = (ArmyRuntimeSlot *)((int)armySlot1 - (int)g_ArmyRuntimeRebaseBaseMinusOne);
    }
    armySlot2->modelNodeRuntime =
         (ModelRuntimeNode *)
         ((int)armySlot2->modelNodeRuntime - (int)g_RuntimeObjectRebaseBaseMinusOne);
    dVar1 = armySlot2->runtimeState98;
    (armySlot2->modelRuntimeOrSavedOffset).modelRuntime = pMVar2;
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
void __thandor_void_preserve_eax_ecx_edx ArmyRuntimePool_RebaseAfterLoad(void)

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
           ((runtimeSlotCursor->modelRuntimeOrSavedOffset).modelRuntime)->reserved10_37 +
           g_ModelRuntimeRebaseDelta + -0x10;
      rebasedCommandTarget = (ArmyRuntimeSlot *)0x0;
      if (runtimeSlotCursor->commandTargetArmyRuntime != (ArmyRuntimeSlot *)0x0) {
        rebasedCommandTarget =
             (ArmyRuntimeSlot *)
             ((int)&runtimeSlotCursor->commandTargetArmyRuntime->modelRuntimeOrSavedOffset +
             (int)g_ArmyRuntimeRebaseBaseMinusOne);
      }
      runtimeSlotCursor->modelNodeRuntime =
           (ModelRuntimeNode *)
           (g_RuntimeObjectRebaseBaseMinusOne +
           (int)(&runtimeSlotCursor->modelNodeRuntime->modelPayload + -1) + 0x30);
      savedRuntimeState98Offset = runtimeSlotCursor->runtimeState98;
      (runtimeSlotCursor->modelRuntimeOrSavedOffset).modelRuntime = rebasedDefinition;
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

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_UpdatePositionedSoundsVariantA
          (WorldRuntimeContext *worldRuntime,
          ArmyRuntimeGroundMovementPositionedSoundView120 *armyRuntime)

{
  uint uVar1;
  SpatialSoundSlot *pSVar2;
  GraphicsFixedVec3 *pGVar3;
  ModelRuntimeGroundMovementPositionedSoundView200 *pMVar4;
  bool bVar5;
  
  pMVar4 = armyRuntime->modelRuntime;
  if ((armyRuntime->movementControl).turnVelocityAngle16 == 0) {
    pMVar4 = armyRuntime->modelRuntime;
    if ((armyRuntime->movementControl).movementAdvancePerTickQ12 == 0) {
      return;
    }
  }
  else {
    uVar1 = (pMVar4->positionedSoundClassState84).positionedSoundSlotIndex1D8;
    if (((uVar1 != 0) && (uVar1 < worldRuntime->dwordArrayCount)) &&
       (worldRuntime->dwordArray != (dword *)0x0)) {
      pSVar2 = (SpatialSoundSlot *)worldRuntime->dwordArray[uVar1];
      if (pSVar2 != (SpatialSoundSlot *)0x0) {
        pGVar3 = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
        bVar5 = TerrainGrid_TestProjectedCellMaskBits01Cf
                          ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,pGVar3->x,
                           worldRuntime);
        if (bVar5)
        goto 
        ArmyRuntimeClass_UpdatePositionedSoundsVariantA_ContinueWithSecondaryConfiguredPositionedSound
        ;
        SpatialSound_UpdateDesiredPositionedGains
                  ((pMVar4->positionedSoundLinkState60).positionedSoundMaximumDistanceQ12_7C,
                   (pMVar4->positionedSoundLinkState60).positionedSoundGainQ15_78,pGVar3,pSVar2);
      }
    }
    pMVar4 = armyRuntime->modelRuntime;
  }
ArmyRuntimeClass_UpdatePositionedSoundsVariantA_ContinueWithSecondaryConfiguredPositionedSound:
  uVar1 = (pMVar4->positionedSoundClassState84).positionedSoundSlotIndex0D0;
  if (((uVar1 != 0) && (uVar1 < worldRuntime->dwordArrayCount)) &&
     (worldRuntime->dwordArray != (dword *)0x0)) {
    pSVar2 = (SpatialSoundSlot *)worldRuntime->dwordArray[uVar1];
    if (pSVar2 != (SpatialSoundSlot *)0x0) {
      pGVar3 = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
      bVar5 = TerrainGrid_TestProjectedCellMaskBits01Cf
                        ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,pGVar3->x,
                         worldRuntime);
      if (!bVar5) {
        SpatialSound_UpdateDesiredPositionedGains
                  ((pMVar4->positionedSoundLinkState60).positionedSoundMaximumDistanceQ12_7C,
                   (pMVar4->positionedSoundLinkState60).positionedSoundGainQ15_78,pGVar3,pSVar2);
      }
    }
  }
  return;
}


/* Address: 0x00522B70.
   Ownership: gameplay/army/runtime.
   Purpose: Binary entry is anchored by g_CodePointerTable_0051FC98[27]@0051FC98. Class method-D partition slots
   24-47 receive (worldRuntime, armyRuntime).
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_NoOpUpdate(WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  return;
}


/* Address: 0x00523E70.
   Ownership: gameplay/army/runtime.
   Purpose: Iterator callback that filters candidate world nodes and stores an accepted projectile target into the
   active army runtime slot.
*/

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_SelectProjectileTargetNode
          (ModelRuntimeTimedTargetProjectileView200 *modelRuntime,
          WorldOwnerListNode100 *candidateNode)

{
  longlong lVar1;
  longlong lVar2;
  int iVar3;
  int iVar4;
  ModelRuntimeSlot *pMVar5;
  int unaff_EBP;
  int unaff_ESI;
  ModelRuntimeSlot *pMVar6;
  
  if (candidateNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
    iVar4 = ((modelRuntime->modelDefinition->shotDefinitionReference2C).definition)->
            mode2SelectionRangeQ12;
    iVar3 = candidateNode->worldXQ12 - (modelRuntime->rootModelNode->worldTransform).translation.x;
    lVar2 = (longlong)iVar4 * (longlong)iVar4 - (longlong)iVar3 * (longlong)iVar3;
    if ((((-1 < lVar2) &&
         (iVar4 = candidateNode->worldYQ12 -
                  (modelRuntime->rootModelNode->worldTransform).translation.y,
         lVar1 = (longlong)iVar4 * (longlong)iVar4,
         -1 < (int)(((int)((ulonglong)lVar2 >> 0x20) - (int)((ulonglong)lVar1 >> 0x20)) -
                   (uint)((uint)lVar2 < (uint)lVar1)))) &&
        (iVar4 = *(int *)(*(int *)((int)candidateNode->runtimePayload + 8) + 0xc),
        iVar4 != modelRuntime->ownerArmyRuntime->factionIndex)) && (iVar4 != 0)) {
      iVar4 = 0;
      pMVar6 = candidateNode->runtimePayload;
      pMVar5 = (ModelRuntimeSlot *)0x0;
      do {
        if (*(int *)((pMVar6->definitionOrSavedId).savedIdOrOffset + 0x4c) == 10) {
          pMVar5 = pMVar6;
        }
        iVar4 = iVar4 + 1;
        iVar3 = unaff_ESI;
        do {
          while (unaff_EBP == 0) {
            iVar4 = iVar4 + -1;
            if (iVar4 == 0) {
              if (pMVar5 == (ModelRuntimeSlot *)0x0) {
                return;
              }
              if (((pMVar5->classState).classStateEC & 8) != 0) {
                return;
              }
              (modelRuntime->timedTargetLinkState).selectedTargetModelRuntime60 = pMVar5;
              return;
            }
          }
          unaff_EBP = unaff_EBP + -1;
          unaff_ESI = iVar3 + 0x20;
          pMVar6 = *(ModelRuntimeSlot **)(iVar3 + 0x140);
          iVar3 = unaff_ESI;
        } while (pMVar6 == (ModelRuntimeSlot *)0x0);
      } while( true );
    }
  }
  else if ((candidateNode->ownerClassId == WORLD_OWNER_RUNTIME_SHOT) &&
          ((modelRuntime->modelDefinition->shotDefinitionReference2C).definition ==
           (candidateNode->runtimePayload->definitionOrSavedId).definition)) {
    (modelRuntime->timedTargetLinkState).matchingActiveShotRuntime64 = candidateNode->runtimePayload
    ;
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

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_UpdateTimedTargetProjectilesAndEffects
          (WorldRuntimeContext *worldRuntime,ModelRuntimeTimedTargetProjectileView200 *modelRuntime)

{
  ModelMeshGroupMask *pMVar1;
  int *piVar2;
  ModelDefinitionTimedTargetProjectileView280 *pMVar3;
  ModelRuntimeNode *pMVar4;
  ModelRuntimeSlot *pMVar5;
  Q12 targetWorldXQ12;
  Q12 targetWorldYQ12;
  Q12 targetWorldZQ12;
  int iVar6;
  
  if (((modelRuntime->classState).classStateEC & 9) == 0) {
    pMVar3 = modelRuntime->modelDefinition;
    iVar6 = (modelRuntime->timedTargetState).targetProjectileReloadCountdownTicks24 -
            g_InGameSimulationStepTicks;
    pMVar4 = modelRuntime->rootModelNode;
    (modelRuntime->timedTargetState).targetProjectileReloadCountdownTicks24 = iVar6;
    if (iVar6 < 1) {
      (modelRuntime->timedTargetState).targetProjectileReloadCountdownTicks24 = 0;
      pMVar1 = &(pMVar4->modelPayload).meshGroupMask;
      *pMVar1 = *pMVar1 | 1;
      (modelRuntime->timedTargetLinkState).selectedTargetModelRuntime60 = (ModelRuntimeSlot *)0x0;
      (modelRuntime->timedTargetLinkState).matchingActiveShotRuntime64 = (ShotRuntimeSlot *)0x0;
      WorldRuntime_ForEachNodeInOwnerListD8
                (modelRuntime,ArmyRuntimeClass_SelectProjectileTargetNode,worldRuntime);
      pMVar5 = (modelRuntime->timedTargetLinkState).selectedTargetModelRuntime60;
      if (((modelRuntime->timedTargetLinkState).matchingActiveShotRuntime64 ==
           (ShotRuntimeSlot *)0x0) && (pMVar5 != (ModelRuntimeSlot *)0x0)) {
        pMVar4 = (pMVar5->rootModelNodeOrSavedOffset).modelNode;
        targetWorldXQ12 = (pMVar4->worldTransform).translation.x;
        targetWorldYQ12 = (pMVar4->worldTransform).translation.y;
        targetWorldZQ12 = (pMVar4->worldTransform).translation.z;
        pMVar4 = modelRuntime->rootModelNode;
        piVar2 = &(modelRuntime->timedTargetState).targetProjectileReloadCountdownTicks24;
        *piVar2 = *piVar2 + (pMVar3->timedTargetParameters30).reloadTicks30;
        pMVar4->runtimeFlags = pMVar4->runtimeFlags | 1;
        pMVar1 = &(pMVar4->modelPayload).meshGroupMask;
        *pMVar1 = *pMVar1 & 0xfffffffe;
        ModelRuntime_EmitProjectilesFromAttachmentPoints
                  ((ShotRuntimeState14)
                   (modelRuntime->timedTargetLinkState).selectedTargetModelRuntime60,targetWorldZQ12
                   ,targetWorldYQ12,targetWorldXQ12,(pMVar3->shotDefinitionReference2C).definition,
                   pMVar4,(MdlSerializedNodeHeader38 *)pMVar3->serializedNodeOffsetOrPointer64,
                   worldRuntime);
      }
    }
  }
  ModelNodeRuntime_RebuildTransformsFromRoot(modelRuntime->rootModelNode);
  (modelRuntime->timedTargetLinkState).selectedTargetModelRuntime60 = (ModelRuntimeSlot *)0x0;
  (modelRuntime->timedTargetLinkState).matchingActiveShotRuntime64 = (ShotRuntimeSlot *)0x0;
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
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

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_UpdatePositionedSoundsVariantB
          (WorldRuntimeContext *worldRuntime,
          ArmyRuntimeGroundMovementPositionedSoundView120 *armyRuntime)

{
  uint uVar1;
  SpatialSoundSlot *pSVar2;
  GraphicsFixedVec3 *pGVar3;
  ModelRuntimeGroundMovementPositionedSoundView200 *pMVar4;
  bool bVar5;
  
  pMVar4 = armyRuntime->modelRuntime;
  if ((armyRuntime->movementControl).turnVelocityAngle16 == 0) {
    pMVar4 = armyRuntime->modelRuntime;
    if ((armyRuntime->movementControl).movementAdvancePerTickQ12 == 0) {
      return;
    }
  }
  else {
    uVar1 = (pMVar4->positionedSoundClassState84).positionedSoundSlotIndex1D8;
    if (((uVar1 != 0) && (uVar1 < worldRuntime->dwordArrayCount)) &&
       (worldRuntime->dwordArray != (dword *)0x0)) {
      pSVar2 = (SpatialSoundSlot *)worldRuntime->dwordArray[uVar1];
      if (pSVar2 != (SpatialSoundSlot *)0x0) {
        pGVar3 = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
        bVar5 = TerrainGrid_TestProjectedCellMaskBits01Cf
                          ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,pGVar3->x,
                           worldRuntime);
        if (!bVar5) {
          SpatialSound_UpdateDesiredPositionedGains
                    ((pMVar4->positionedSoundLinkState60).positionedSoundMaximumDistanceQ12_7C,
                     (pMVar4->positionedSoundLinkState60).positionedSoundGainQ15_78,pGVar3,pSVar2);
        }
      }
    }
  }
  uVar1 = (pMVar4->positionedSoundClassState84).positionedSoundSlotIndex0D0;
  if (((uVar1 != 0) && (uVar1 < worldRuntime->dwordArrayCount)) &&
     (worldRuntime->dwordArray != (dword *)0x0)) {
    pSVar2 = (SpatialSoundSlot *)worldRuntime->dwordArray[uVar1];
    if (pSVar2 != (SpatialSoundSlot *)0x0) {
      pGVar3 = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
      bVar5 = TerrainGrid_TestProjectedCellMaskBits01Cf
                        ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,pGVar3->x,
                         worldRuntime);
      if (!bVar5) {
        SpatialSound_UpdateDesiredPositionedGains
                  ((pMVar4->positionedSoundLinkState60).positionedSoundMaximumDistanceQ12_7C,
                   (pMVar4->positionedSoundLinkState60).positionedSoundGainQ15_78,pGVar3,pSVar2);
      }
    }
  }
  return;
}


/* Address: 0x00526FE0.
   Ownership: gameplay/army/runtime.
   Purpose: Loads EBX from army-runtime offset 0x6C and ECX from the linked model-definition offset 0xC4. The
   selection-panel renderer consumes both register results when composing metric bars.
*/
ArmyMetric6CDefinitionC4Regs8 __thandor_regs_ebx_ecx_preserve_eax_edx
ArmyRuntime_QueryMetric6CAndDefinitionC4Regs(ArmyRuntimeSlot *armyRuntime)

{
  ArmyMetric6CDefinitionC4Regs8 AVar1;
  
  AVar1.ecx = *(dword *)(((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->classState).
                        reservedC4_C7;
  AVar1.ebx = (dword)armyRuntime->linkedArmyRuntimeOrSavedOffset;
  return AVar1;
}


/* Address: 0x00527150.
   Ownership: gameplay/army/runtime.
   Purpose: Scans thirteen attachment slots and ORs effect-variant bits 1, 2, and 4 into EBX.
*/
int __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_AccumulateAttachmentEffectVariantMaskRegs(ArmyRuntimeSlot *armyRuntime)

{
  int iVar1;
  int attachmentEffectSlotsRemaining;
  uint uVar2;
  
  uVar2 = 0;
  attachmentEffectSlotsRemaining = 0xd;
  do {
    iVar1 = armyRuntime->movementTarget0Q12;
    if (iVar1 == g_ArmyLinkedChildAssetIdSlot0) {
      uVar2 = uVar2 | 1;
    }
    if (iVar1 == g_ArmyLinkedChildAssetIdSlot1) {
      uVar2 = uVar2 | 2;
    }
    if (iVar1 == g_ArmyLinkedChildAssetIdSlot2) {
      uVar2 = uVar2 | 4;
    }
    armyRuntime = (ArmyRuntimeSlot *)&armyRuntime->modelNodeRuntime;
    attachmentEffectSlotsRemaining = attachmentEffectSlotsRemaining + -1;
  } while (attachmentEffectSlotsRemaining != 0);
  return uVar2;
}


/* Address: 0x00527FE0.
   Ownership: gameplay/army/runtime.
   Purpose: Refreshes the configured looping positioned sound while the runtime remains active and its projected
   terrain cell is available. Class method-D partition slots 24-47 receive (worldRuntime, armyRuntime).
   Cross-module calls: TerrainGrid_TestProjectedCellMaskBits01Cf [world/terrain/grid],
   SpatialSound_UpdateDesiredPositionedGains [audio/spatial/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_UpdateLoopingPositionedSound
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  GraphicsFixedVec3 *worldPosition;
  ModelRuntimeSlot *pMVar1;
  uint uVar2;
  SpatialSoundSlot *slot;
  bool bVar3;
  
  pMVar1 = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  uVar2 = pMVar1->attachments140[3].childNodeIndex0C;
  if (((((armyRuntime->runtimeFlags & 1) == 0) && (uVar2 != 0)) &&
      (uVar2 < worldRuntime->dwordArrayCount)) &&
     ((worldRuntime->dwordArray != (dword *)0x0 &&
      (slot = (SpatialSoundSlot *)worldRuntime->dwordArray[uVar2], slot != (SpatialSoundSlot *)0x0))
     )) {
    worldPosition = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
    bVar3 = TerrainGrid_TestProjectedCellMaskBits01Cf
                      ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,
                       worldPosition->x,worldRuntime);
    if (!bVar3) {
      SpatialSound_UpdateDesiredPositionedGains
                ((pMVar1->classLinkState).classState7C,(pMVar1->classLinkState).classState78,
                 worldPosition,slot);
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

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_UpdateVerticalDeploymentAndCollisionState
          (WorldRuntimeContext *worldRuntime,ModelRuntimeVerticalDeploymentView200 *modelRuntime)

{
  dword *pdVar1;
  Q12 *pQVar2;
  ModelDefinitionVerticalDeploymentView280 *pMVar3;
  ArmyRuntimeSlot *armyRuntime;
  uint uVar4;
  DirectSoundVoiceSet **ppDVar5;
  int iVar6;
  bool bVar7;
  ModelRuntimeNode *modelNode1;
  
  pMVar3 = modelRuntime->modelDefinition;
  modelNode1 = modelRuntime->rootModelNode;
  armyRuntime = (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime;
  iVar6 = pMVar3->deploymentTravelLimitQ12;
  if (((modelRuntime->classState).classStateB8 & 1) == 0) {
    if ((modelRuntime->deploymentState60).deploymentTravelQ12_60 != 0) {
      if ((modelRuntime->deploymentState60).deploymentTravelQ12_60 <= iVar6) {
        uVar4 = pMVar3->deploymentSoundAssetIndex26C;
        if (((uVar4 != 0) && (uVar4 < worldRuntime->dwordArrayCount)) &&
           (worldRuntime->dwordArray != (dword *)0x0)) {
          ppDVar5 = (DirectSoundVoiceSet **)worldRuntime->dwordArray[uVar4];
          if (ppDVar5 != (DirectSoundVoiceSet **)0x0) {
            bVar7 = TerrainGrid_TestProjectedCellMaskBits01Cf
                              ((modelNode1->worldTransform).translation.y,
                               (modelNode1->worldTransform).translation.x,worldRuntime);
            if (!bVar7) {
              SpatialSound_PlayPositionedOneShot
                        (pMVar3->positionedSoundMaximumDistanceQ12,pMVar3->positionedSoundGainQ15,
                         &(modelNode1->worldTransform).translation,ppDVar5);
            }
          }
        }
      }
      modelNode1 = modelNode1->childNodes[0];
      iVar6 = pMVar3->verticalDeploymentStepQ12PerTick;
      pdVar1 = &(modelRuntime->classState).classStateB8;
      *pdVar1 = *pdVar1 & 0xfffffffd;
      iVar6 = iVar6 * g_InGameSimulationStepTicks;
      if (modelNode1 != (ModelRuntimeNode *)0x0) {
        pQVar2 = &(modelNode1->modelPayload).localTranslationZQ12;
        *pQVar2 = *pQVar2 + iVar6;
        (modelRuntime->deploymentState60).deploymentTravelQ12_60 =
             (modelRuntime->deploymentState60).deploymentTravelQ12_60 + iVar6;
        ModelNodeRuntime_RebuildTransformsFromRoot(modelRuntime->rootModelNode);
      }
    }
  }
  else {
    if ((modelRuntime->deploymentState60).collisionRetryCountdown64 != 0) {
      pdVar1 = &(modelRuntime->deploymentState60).collisionRetryCountdown64;
      *pdVar1 = *pdVar1 - 1;
    }
    pdVar1 = &(modelRuntime->classState).classStateB8;
    *pdVar1 = *pdVar1 | 2;
    if (((modelRuntime->deploymentState60).collisionRetryCountdown64 == 0) &&
       (pdVar1 = &(modelRuntime->classState).classStateB8, *pdVar1 = *pdVar1 & 0xfffffffe,
       armyRuntime != (ArmyRuntimeSlot *)0x0)) {
      (modelRuntime->deploymentState60).collisionRetryCountdown64 = 8;
      (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime = (ArmyRuntimeSlot *)0x0
      ;
      bVar7 = ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf
                        (pMVar3->placementRadiusOrClearanceDC,
                         (modelNode1->worldTransform).translation.y,
                         (modelNode1->worldTransform).translation.x,armyRuntime);
      if (bVar7) {
        (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime = armyRuntime;
        pdVar1 = &(modelRuntime->classState).classStateB8;
        *pdVar1 = *pdVar1 | 1;
      }
    }
    if (iVar6 < (modelRuntime->deploymentState60).deploymentTravelQ12_60) {
      pdVar1 = &(modelRuntime->classState).classStateB8;
      *pdVar1 = *pdVar1 & 0xfffffffd;
      if ((modelRuntime->deploymentState60).deploymentTravelQ12_60 == 0) {
        uVar4 = pMVar3->deploymentSoundAssetIndex26C;
        if (((uVar4 != 0) && (uVar4 < worldRuntime->dwordArrayCount)) &&
           (worldRuntime->dwordArray != (dword *)0x0)) {
          ppDVar5 = (DirectSoundVoiceSet **)worldRuntime->dwordArray[uVar4];
          if (ppDVar5 != (DirectSoundVoiceSet **)0x0) {
            bVar7 = TerrainGrid_TestProjectedCellMaskBits01Cf
                              ((modelNode1->worldTransform).translation.y,
                               (modelNode1->worldTransform).translation.x,worldRuntime);
            if (!bVar7) {
              SpatialSound_PlayPositionedOneShot
                        (pMVar3->positionedSoundMaximumDistanceQ12,pMVar3->positionedSoundGainQ15,
                         &(modelNode1->worldTransform).translation,ppDVar5);
            }
          }
        }
      }
      if (modelNode1->childNodes[0] != (ModelRuntimeNode *)0x0) {
        iVar6 = pMVar3->verticalDeploymentStepQ12PerTick * g_InGameSimulationStepTicks;
        pQVar2 = &(modelNode1->childNodes[0]->modelPayload).localTranslationZQ12;
        *pQVar2 = *pQVar2 - iVar6;
        (modelRuntime->deploymentState60).deploymentTravelQ12_60 =
             (modelRuntime->deploymentState60).deploymentTravelQ12_60 - iVar6;
        ModelNodeRuntime_RebuildTransformsFromRoot(modelRuntime->rootModelNode);
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
bool __thandor_cf_preserve_eax_ecx_edx
ArmyRuntime_ResolveShotLaunchFromModelAttachmentCf
          (ShotRuntimeState14 shotRuntimeState14,Q12 targetWorldXQ12,Q12 targetWorldYQ12,
          Q12 targetWorldZQ12,SprAttachmentSelectorOrdinal attachmentSelectorOrdinal,
          ShotDefinition *shotDefinition,ModelRuntimeNode *modelNode,
          MdlSerializedNodeHeader38 *definitionNode,WorldRuntimeContext *worldRuntime)

{
  ModelResourceHitTestAndRenderView210 *pMVar1;
  Q12 launchWorldYQ12;
  Q12 launchWorldZQ12;
  ModelPackedLookupTableEntryCount MVar2;
  ModelPackedPointRecord *localPointRecord;
  ModelLocalPointRegs12 MVar3;
  
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNode);
  pMVar1 = (definitionNode->spriteAssetReference).modelResource;
  MVar2 = pMVar1->packedLookupTableEntryCount;
  localPointRecord =
       (ModelPackedPointRecord *)(pMVar1->reserved00_AF + pMVar1->packedLookupTableRelativeOffset);
  while( true ) {
    if (MVar2 == 0) {
      return true;
    }
    if (localPointRecord->packedLookupKey == (attachmentSelectorOrdinal << 4 | 2)) break;
    localPointRecord = localPointRecord + 1;
    MVar2 = MVar2 - 1;
  }
  MVar3 = ModelNodeRuntime_TransformLocalPointRegs(localPointRecord,modelNode);
  launchWorldZQ12 = MVar3.edx;
  launchWorldYQ12 = MVar3.ecx;
  ShotRuntimePool_CreateProjectileFromDefinition
            (shotRuntimeState14,
             (ArmyRuntimeSlot *)((modelNode->runtimePayload).armyRuntime)->linkedEntityRuntime,
             targetWorldXQ12,targetWorldYQ12,targetWorldZQ12,launchWorldZQ12,launchWorldYQ12,
             MVar3.eax,shotDefinition,worldRuntime);
  return false;
}


/* Address: 0x00529B50.
   Ownership: gameplay/army/runtime.
   Purpose: Computes and clamps the activation metric from definition and runtime state, stores it atomically, and
   plays the configured positioned start sound on a zero-to-nonzero transition when the projected terrain cell is
   available.
   Cross-module calls: TerrainGrid_TestProjectedCellMaskBits01Cf [world/terrain/grid],
   SpatialSound_PlayPositionedOneShot [audio/spatial/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_UpdateActivationMetricAndPlayStartSound
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  GraphicsFixedVec3 *worldPosition;
  int iVar1;
  ModelRuntimeSlot *pMVar2;
  AngleTurn32 AVar3;
  DirectSoundVoiceSet **voiceSetRef;
  uint uVar4;
  uint uVar5;
  bool bVar6;
  ModelRuntimeNode *modelNode1;
  
  modelNode1 = armyRuntime->modelNodeRuntime;
  pMVar2 = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  AVar3 = (modelNode1->modelPayload).worldRotationAngle1;
  uVar5 = pMVar2->attachmentCount0C;
  if ((int)AVar3 < (int)pMVar2[1].classLinkState.classState68) {
    uVar4 = (modelNode1->modelPayload).worldRotationAngle2 -
            (modelNode1->modelPayload).worldRotationAngle0 & 0xffff;
    uVar5 = uVar5 * 5 >> 4;
    if (((*(int *)(pMVar2->reserved10_37 + 0x14) <= (int)AVar3) &&
        (uVar5 = pMVar2->attachmentCount0C, 0x4000 < uVar4)) && (uVar4 < 0xc000)) {
      uVar5 = uVar5 * 5 >> 3;
    }
  }
  uVar4 = (armyRuntime->movementControl).movementAdvancePerTickQ12;
  if ((uVar4 < uVar5) && (uVar4 = uVar4 + *(int *)(pMVar2->reserved10_37 + 8), uVar4 < uVar5)) {
    uVar5 = uVar4;
  }
  LOCK();
  iVar1 = (armyRuntime->movementControl).movementAdvancePerTickQ12;
  (armyRuntime->movementControl).movementAdvancePerTickQ12 = uVar5;
  UNLOCK();
  if ((iVar1 == 0) && (uVar5 != 0)) {
    uVar5 = *(uint *)(pMVar2->classState).reservedD4_DB;
    if ((uVar5 != 0) &&
       ((uVar5 < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != (dword *)0x0)))) {
      voiceSetRef = (DirectSoundVoiceSet **)worldRuntime->dwordArray[uVar5];
      worldPosition = &(modelNode1->worldTransform).translation;
      if (voiceSetRef != (DirectSoundVoiceSet **)0x0) {
        bVar6 = TerrainGrid_TestProjectedCellMaskBits01Cf
                          ((modelNode1->worldTransform).translation.y,worldPosition->x,worldRuntime)
        ;
        if (!bVar6) {
          SpatialSound_PlayPositionedOneShot
                    ((pMVar2->classLinkState).classState7C,(pMVar2->classLinkState).classState78,
                     worldPosition,voiceSetRef);
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
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_HandleCollisionPartner
          (ArmyRuntimeSlot *currentArmyRuntime,Q12 currentWorldXQ12,Q12 currentWorldYQ12,
          ArmyRuntimeSlot *collisionPartnerArmyRuntime,WorldRuntimeContext *worldRuntime)

{
  Q12 *pQVar1;
  dword impactAngle;
  
  if (collisionPartnerArmyRuntime != (ArmyRuntimeSlot *)0x0) {
    if (((collisionPartnerArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
        definitionValue9C_4C == 0x17) {
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
    else if (((collisionPartnerArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
             definitionValue9C_4C == 0) {
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
ArmyPreviewTextureEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ArmyRuntime_RenderPreviewTextureCf
          (GraphicsPixelDimension previewHeight,GraphicsPixelDimension previewWidth,
          FactionRuntimeIndex factionIndex,PckArmyAssetIdCatalog armyAssetId,
          WorldRuntimeContext *worldRuntime)

{
  undefined8 uVar1;
  ModelRuntimeNode *pMVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  GraphicsPixelDimension GVar7;
  GameEntityRuntime *entityRuntime1;
  int iVar8;
  GameEntityRuntime *memory;
  void *pvVar9;
  int iVar10;
  int iVar11;
  Q12 *pQVar12;
  Q12 *pQVar13;
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
  ArmyRuntimeCreateEaxCf5 AVar30;
  GraphicsOffscreenAllocationEaxCf5 GVar31;
  ArmyPreviewTextureEaxCf5 AVar32;
  ArmyPreviewTextureEaxCf5 AVar33;
  
  GVar7 = previewWidth;
  AVar30 = ArmyRuntime_CreateInstanceFromAssetCf
                     (1,0,0x6000000,0x6000000,factionIndex,armyAssetId,worldRuntime);
  entityRuntime1 = (GameEntityRuntime *)AVar30.eax;
  if (!AVar30.carry) {
    pMVar2 = (entityRuntime1->common).ownership.modelNode;
    if (((*(int *)(*(int *)(entityRuntime1->common).ownership.definitionOrClassRecord + 0x4c) == 0xd
         ) && (3 < pMVar2->childCount)) && (pMVar2->childNodes[3] != (ModelRuntimeNode *)0x0)) {
      WorldRuntime_UnlinkNodeFromOwnerListD8((WorldOwnerListNode100 *)pMVar2->childNodes[3]);
      pMVar2->childNodes[3] = (ModelRuntimeNode *)0x0;
    }
    (pMVar2->modelPayload).worldRotationAngle2 = 0x2000;
    (pMVar2->modelPayload).worldRotationAngle1 = 0x3000;
    pMVar2->runtimeFlags = pMVar2->runtimeFlags | 1;
    pMVar2->tintArgb = 0xffffffff;
    ModelNodeRuntime_RebuildTransformsFromRoot(pMVar2);
    g_ModelBoundsMinimumX = (pMVar2->worldTransform).translation.x;
    g_ModelBoundsMinimumY = (pMVar2->worldTransform).translation.y;
    g_ModelBoundsMinimumZ = (pMVar2->worldTransform).translation.z;
    g_ModelBoundsMaximumX = g_ModelBoundsMinimumX;
    g_ModelBoundsMaximumY = g_ModelBoundsMinimumY;
    g_ModelBoundsMaximumZ = g_ModelBoundsMinimumZ;
    ModelNodeRuntime_AccumulateTransformedBoundsRecursive(pMVar2);
    iVar8 = g_ModelBoundsMaximumY - g_ModelBoundsMinimumY;
    iVar10 = g_ModelBoundsMaximumZ - g_ModelBoundsMinimumZ;
    iVar11 = iVar10;
    if (iVar10 < iVar8) {
      iVar11 = iVar8;
    }
    g_ArmyPreviewViewOriginYQ12 = iVar8 + g_ModelBoundsMinimumY * 2 >> 1;
    g_ArmyPreviewViewOriginZQ12 = iVar10 + g_ModelBoundsMinimumZ * 2 >> 1;
    g_ArmyPreviewAuxiliaryOrientation0 = 0x6000;
    g_ArmyPreviewAuxiliaryOrientation1 = 0xffffe667;
    g_ArmyPreviewViewOriginXQ12 = g_ModelBoundsMaximumX + iVar11 * 4;
    g_ArmyPreviewPrimaryColorArgb = 0xffc0c0c0;
    g_ArmyPreviewSecondaryColorArgb = 0xff606060;
    g_ArmyPreviewProjectionScaleQ12 = 0x800;
    g_ArmyPreviewViewAngle0 = 0xffff8000;
    g_ArmyPreviewViewAngle1 = 0;
    g_ArmyPreviewProjectionShift = 4;
    g_ArmyPreviewModelNodePointer = (dword)pMVar2;
    GVar31 = (*g_GraphicsOffscreenRenderModelListToTextureSourceCf)
                       ((GraphicsOffscreenSceneExtents *)&g_ArmyPreviewPrimaryColorArgb,
                        &g_ArmyPreviewAuxiliaryOrientation0,
                        (GraphicsOffscreenViewParameters *)&g_ArmyPreviewViewOriginXQ12,
                        previewHeight * 2,previewWidth * 2,1,
                        (ModelRuntimeNode **)&g_ArmyPreviewModelNodePointer);
    memory = GVar31.allocation;
    if (!GVar31.carry) {
      pQVar12 = &memory[1].common.commandTarget.targetWorldXQ12;
      ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,entityRuntime1);
      pQVar13 = pQVar12;
      previewWidth = GVar7;
      do {
        do {
          uVar3 = *pQVar12;
          uVar4 = pQVar12[1];
          uVar5 = pQVar12[GVar7 * 2];
          uVar6 = pQVar12[GVar7 * 2 + 1];
          uVar16 = (undefined1)(uVar3 >> 0x18);
          uVar14 = CONCAT11(uVar16,uVar16);
          uVar17 = (undefined1)(uVar3 >> 0x10);
          uVar16 = (undefined1)(uVar3 >> 8);
          uVar24 = (undefined1)(uVar4 >> 0x18);
          uVar15 = CONCAT11(uVar24,uVar24);
          uVar25 = (undefined1)(uVar4 >> 0x10);
          uVar24 = (undefined1)(uVar4 >> 8);
          uVar26 = (undefined1)(uVar5 >> 0x18);
          uVar18 = CONCAT11(uVar26,uVar26);
          uVar27 = (undefined1)(uVar5 >> 0x10);
          uVar26 = (undefined1)(uVar5 >> 8);
          uVar28 = (undefined1)(uVar6 >> 0x18);
          uVar19 = CONCAT11(uVar28,uVar28);
          uVar29 = (undefined1)(uVar6 >> 0x10);
          uVar28 = (undefined1)(uVar6 >> 8);
          mm0PackedValue0 =
               pmulhw(CONCAT26(uVar14 >> 4,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(uVar14,uVar17),
                                                          CONCAT14(uVar17,uVar3)) >> 0x20) >> 4,
                                        CONCAT22(CONCAT11(uVar16,uVar16) >> 4,
                                                 CONCAT11((char)uVar3,(char)uVar3) >> 4))),
                      *(undefined8 *)(&g_ArmyPreviewAlphaPremultiplyMmxLut256 + (uVar3 >> 0x18) * 8)
                     );
          mm1PackedValue0 =
               pmulhw(CONCAT26(uVar15 >> 4,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(uVar15,uVar25),
                                                          CONCAT14(uVar25,uVar4)) >> 0x20) >> 4,
                                        CONCAT22(CONCAT11(uVar24,uVar24) >> 4,
                                                 CONCAT11((char)uVar4,(char)uVar4) >> 4))),
                      *(undefined8 *)(&g_ArmyPreviewAlphaPremultiplyMmxLut256 + (uVar4 >> 0x18) * 8)
                     );
          mm2PackedValue0 =
               pmulhw(CONCAT26(uVar18 >> 4,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(uVar18,uVar27),
                                                          CONCAT14(uVar27,uVar5)) >> 0x20) >> 4,
                                        CONCAT22(CONCAT11(uVar26,uVar26) >> 4,
                                                 CONCAT11((char)uVar5,(char)uVar5) >> 4))),
                      *(undefined8 *)(&g_ArmyPreviewAlphaPremultiplyMmxLut256 + (uVar5 >> 0x18) * 8)
                     );
          mm3PackedValue0 =
               pmulhw(CONCAT26(uVar19 >> 4,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(uVar19,uVar29),
                                                          CONCAT14(uVar29,uVar6)) >> 0x20) >> 4,
                                        CONCAT22(CONCAT11(uVar28,uVar28) >> 4,
                                                 CONCAT11((char)uVar6,(char)uVar6) >> 4))),
                      *(undefined8 *)(&g_ArmyPreviewAlphaPremultiplyMmxLut256 + (uVar6 >> 0x18) * 8)
                     );
          uVar1 = *(undefined8 *)
                   (&g_ArmyPreviewAverageAlphaReciprocalMmxLut256 +
                   ((uVar3 >> 0x18) + (uVar4 >> 0x18) + (uVar5 >> 0x18) + (uVar6 >> 0x18) >> 2) * 8)
          ;
          uVar14 = ((short)mm0PackedValue0 + (short)mm1PackedValue0 +
                    (short)mm2PackedValue0 + (short)mm3PackedValue0 +
                   (short)g_ArmyPreviewDownsampleAlphaRoundingBiasMmx) * (short)uVar1;
          uVar18 = ((short)((ulonglong)mm0PackedValue0 >> 0x10) +
                    (short)((ulonglong)mm1PackedValue0 >> 0x10) +
                    (short)((ulonglong)mm2PackedValue0 >> 0x10) +
                    (short)((ulonglong)mm3PackedValue0 >> 0x10) +
                   (short)((ulonglong)g_ArmyPreviewDownsampleAlphaRoundingBiasMmx >> 0x10)) *
                   (short)((ulonglong)uVar1 >> 0x10);
          uVar20 = ((short)((ulonglong)mm0PackedValue0 >> 0x20) +
                    (short)((ulonglong)mm1PackedValue0 >> 0x20) +
                    (short)((ulonglong)mm2PackedValue0 >> 0x20) +
                    (short)((ulonglong)mm3PackedValue0 >> 0x20) +
                   (short)((ulonglong)g_ArmyPreviewDownsampleAlphaRoundingBiasMmx >> 0x20)) *
                   (short)((ulonglong)uVar1 >> 0x20);
          uVar22 = ((short)((ulonglong)mm0PackedValue0 >> 0x30) +
                    (short)((ulonglong)mm1PackedValue0 >> 0x30) +
                    (short)((ulonglong)mm2PackedValue0 >> 0x30) +
                    (short)((ulonglong)mm3PackedValue0 >> 0x30) +
                   (short)((ulonglong)g_ArmyPreviewDownsampleAlphaRoundingBiasMmx >> 0x30)) *
                   (short)((ulonglong)uVar1 >> 0x30);
          uVar15 = uVar14 >> 8;
          uVar19 = uVar18 >> 8;
          uVar21 = uVar20 >> 8;
          uVar23 = uVar22 >> 8;
          *pQVar13 = CONCAT13((uVar23 != 0) * (uVar23 < 0x100) * (char)(uVar22 >> 8) -
                              (0xff < uVar23),
                              CONCAT12((uVar21 != 0) * (uVar21 < 0x100) * (char)(uVar20 >> 8) -
                                       (0xff < uVar21),
                                       CONCAT11((uVar19 != 0) * (uVar19 < 0x100) *
                                                (char)(uVar18 >> 8) - (0xff < uVar19),
                                                (uVar15 != 0) * (uVar15 < 0x100) *
                                                (char)(uVar14 >> 8) - (0xff < uVar15))));
          pQVar12 = pQVar12 + 2;
          pQVar13 = pQVar13 + 1;
          previewWidth = previewWidth - 1;
        } while (previewWidth != 0);
        pQVar12 = pQVar12 + GVar7 * 2;
        previewHeight = previewHeight - 1;
        previewWidth = GVar7;
      } while (previewHeight != 0);
      pvVar9 = (void *)((int)memory[1].common.commandFlags >> 1);
      entityRuntime1 = (GameEntityRuntime *)((int)memory[1].common.commandTarget.targetEntity >> 1);
      memory[1].common.commandFlags = (GameEntityCommandFlags)pvVar9;
      memory[1].common.commandTarget.targetEntity = entityRuntime1;
      memory[1].common.ownership.definitionOrClassRecord = pvVar9;
      memory[1].common.ownership.modelNode = (ModelRuntimeNode *)entityRuntime1;
      pMVar2 = (ModelRuntimeNode *)((int)pvVar9 * (int)entityRuntime1 * 4 + 0x220);
      (memory->common).ownership.modelNode = pMVar2;
      (*g_MemoryApi.shrinkInPlace)((dword)pMVar2,memory);
      AVar32.carry = false;
      AVar32.previewTexture = (GraphicsTextureResource *)memory;
      return AVar32;
    }
    ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,entityRuntime1);
    entityRuntime1 = memory;
  }
  AVar33.carry = true;
  AVar33.previewTexture = (GraphicsTextureResource *)entityRuntime1;
  return AVar33;
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
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive
          (WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  dword *pdVar1;
  dword dVar2;
  ModelRuntimeNode *pMVar3;
  int iVar4;
  ModelRuntimeSlot *pMVar5;
  uint uVar6;
  ModelRuntimeSlot *pMVar7;
  int iVar8;
  dword dVar9;
  uint uVar10;
  
  dVar9 = (modelRuntime->definitionOrSavedId).savedIdOrOffset;
  (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[*(int *)(dVar9 + 0x4c)])
            (worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  if (((modelRuntime->classState).classStateEC & 0x10) == 0) {
    pdVar1 = &(modelRuntime->classState).classStateF8;
    *pdVar1 = *pdVar1 - g_InGameSimulationStepTicks;
    if ((int)*pdVar1 < 0) {
      (modelRuntime->classState).classStateF8 = 4;
      dVar2 = modelRuntime->definitionValue60_3C;
      uVar10 = *(int *)(dVar9 + 0x60) * 3;
      if (dVar2 != 0) {
        if (((modelRuntime->classState).classStateEC & 1) == 0) {
          if (((modelRuntime->classState).classStateEC & 0x400) == 0) {
            uVar6 = dVar2 + *(int *)(dVar9 + 0x1b4);
            uVar10 = uVar10 >> 2;
            if ((int)uVar10 < (int)uVar6) {
              uVar6 = uVar10;
            }
            if ((int)modelRuntime->definitionValue60_3C < (int)uVar6) {
              modelRuntime->definitionValue60_3C = uVar6;
            }
          }
        }
        else {
          uVar6 = dVar2 - *(int *)(dVar9 + 0x1b4);
          uVar10 = uVar10 >> 2;
          if ((int)uVar6 < (int)uVar10) {
            uVar6 = uVar10;
          }
          if ((int)uVar6 < (int)modelRuntime->definitionValue60_3C) {
            modelRuntime->definitionValue60_3C = uVar6;
          }
        }
      }
    }
  }
  pdVar1 = &(modelRuntime->classState).classStateFC;
  *pdVar1 = *pdVar1 - g_InGameSimulationStepTicks;
  if (((((int)*pdVar1 < 0) &&
       (pdVar1 = &(modelRuntime->classState).classStateFC, *pdVar1 = *pdVar1 + 0xc,
       ((modelRuntime->classState).classStateEC & 0x10) != 0)) && (*(int *)(dVar9 + 0x184) != 0)) &&
     ((*(int *)(dVar9 + 0x60) != 0 && (0 < (int)modelRuntime->definitionValue60_3C)))) {
    iVar8 = ((modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime)->factionIndex;
    g_GameFactionRuntimeImage.records[iVar8].xeniteCurrentQ4 =
         g_GameFactionRuntimeImage.records[iVar8].xeniteCurrentQ4 + (*(uint *)(dVar9 + 0x184) >> 5);
    uVar10 = *(uint *)(dVar9 + 0x60);
    if (*(int *)(dVar9 + 0x4c) == 0x16) {
      uVar6 = ArmyRuntimeSpawner_ComputeRemainingLinkedAssetMetric
                        ((ArmyRuntimeLinkedChildMaskSlotView *)modelRuntime);
      g_GameFactionRuntimeImage.records[iVar8].xeniteCurrentQ4 =
           g_GameFactionRuntimeImage.records[iVar8].xeniteCurrentQ4 + uVar6;
    }
    uVar10 = uVar10 >> 4;
    pdVar1 = &modelRuntime->definitionValue60_3C;
    dVar2 = *pdVar1;
    *pdVar1 = *pdVar1 - uVar10;
    if (*pdVar1 == 0 || SBORROW4(dVar2,uVar10) != (int)*pdVar1 < 0) {
      modelRuntime->definitionValue60_3C = 0;
      pdVar1 = &(modelRuntime->classState).classStateEC;
      *pdVar1 = *pdVar1 ^ 0x210;
      pMVar3 = (modelRuntime->rootModelNodeOrSavedOffset).modelNode;
      EffectRuntimePool_CreateInstanceFromDefinitionCf
                (EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY,
                 (EffectRuntimeOwnerReference4)modelRuntime,
                 (pMVar3->modelPayload).worldRotationAngle2,
                 (pMVar3->modelPayload).worldRotationAngle1,
                 (pMVar3->modelPayload).worldRotationAngle0,(pMVar3->worldTransform).translation.z,
                 (pMVar3->worldTransform).translation.y,(pMVar3->worldTransform).translation.x,
                 *(EffectDefinition **)(dVar9 + 400),worldRuntime);
      pdVar1 = &(modelRuntime->classState).classStateEC;
      *pdVar1 = *pdVar1 | 0x20;
      goto 
      ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive_ContinueWithTechnologyPaymentAndChildRecursion
      ;
    }
  }
  if ((((modelRuntime->classState).classStateEC & 0x20) == 0) &&
     ((int)modelRuntime->definitionValue60_3C < 1)) {
    pMVar7 = (ModelRuntimeSlot *)g_InGameSimulationStepTicks;
    pMVar5 = (modelRuntime->linkedModelRuntimeOrSavedOffset).modelRuntime;
    while (pMVar5 != (ModelRuntimeSlot *)0x0) {
      ArmyRuntime_ProcessReadyAttachmentChannels(worldRuntime,modelRuntime);
      modelRuntime->definitionValue84_40 = modelRuntime->definitionValue84_40 - 1;
      modelRuntime->definitionValue88_44 = modelRuntime->definitionValue88_44 - 1;
      modelRuntime->definitionValue94_48 = modelRuntime->definitionValue94_48 - 1;
      modelRuntime->definitionValue9C_4C = modelRuntime->definitionValue9C_4C - 1;
      modelRuntime->definitionValueA4_50 = modelRuntime->definitionValueA4_50 - 1;
      modelRuntime->definitionValueAC_54 = modelRuntime->definitionValueAC_54 - 1;
      modelRuntime->definitionValueB4_58 = modelRuntime->definitionValueB4_58 - 1;
      modelRuntime->definitionValueBC_5C = modelRuntime->definitionValueBC_5C - 1;
      pMVar7 = (ModelRuntimeSlot *)((int)&pMVar7[-1].attachments140[5].reserved1C + 3);
      pMVar5 = pMVar7;
    }
  }

  ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive_ContinueWithTechnologyPaymentAndChildRecursion
  :
  if ((((modelRuntime->classState).classStateEC & 0x40) != 0) &&
     (iVar8 = *(int *)(modelRuntime->reserved100_117 + 8) + g_InGameSimulationStepTicks,
     ((modelRuntime->classState).classStateEC & 1) == 0)) {
    *(int *)(modelRuntime->reserved100_117 + 8) = iVar8;
    if (*(int *)(modelRuntime->reserved100_117 + 4) <= iVar8) {
      Technology_UnlockForFaction
                ((((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->worldTransform).
                 translation.y,
                 (((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->worldTransform).
                 translation.x,*(TechnologyId *)modelRuntime->reserved100_117,
                 ((modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime)->factionIndex);
      iVar8 = *(int *)(modelRuntime->reserved100_117 + 0xc);
      pdVar1 = &(modelRuntime->classState).classStateEC;
      *pdVar1 = *pdVar1 & 0xffffffbf;
      pdVar1 = &(modelRuntime->classState).definitionDerivedValueF4;
      *pdVar1 = *pdVar1 - iVar8;
    }
  }
  if ((((modelRuntime->classState).classStateEC & 0x80) != 0) &&
     (((modelRuntime->classState).classStateEC & 0x140) == 0)) {
    iVar8 = ((modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime)->factionIndex;
    iVar4 = *(int *)(modelRuntime->reserved100_117 + 0xc);
    if (*(int *)(modelRuntime->reserved100_117 + 0x10) <=
        (int)g_GameFactionRuntimeImage.records[iVar8].xeniteCurrentQ4) {
      g_GameFactionRuntimeImage.records[iVar8].xeniteCurrentQ4 =
           g_GameFactionRuntimeImage.records[iVar8].xeniteCurrentQ4 -
           *(int *)(modelRuntime->reserved100_117 + 0x10);
      modelRuntime->reserved100_117[0x10] = 0;
      modelRuntime->reserved100_117[0x11] = 0;
      modelRuntime->reserved100_117[0x12] = 0;
      modelRuntime->reserved100_117[0x13] = 0;
      pdVar1 = &(modelRuntime->classState).classStateEC;
      *pdVar1 = *pdVar1 ^ 0xc0;
      pdVar1 = &(modelRuntime->classState).definitionDerivedValueF4;
      *pdVar1 = *pdVar1 + iVar4;
    }
  }
  dVar9 = modelRuntime->attachmentCount0C;
  uVar10 = (modelRuntime->classState).classStateEC;
  if (dVar9 != 0) {
    do {
      pMVar7 = modelRuntime->attachments140[0].childModelRuntimeOrSavedOffset00;
      if (pMVar7 != (ModelRuntimeSlot *)0x0) {
        pdVar1 = &(pMVar7->classState).classStateEC;
        *pdVar1 = *pdVar1 | uVar10 & 8;
        ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive(worldRuntime,pMVar7);
      }
      modelRuntime = (ModelRuntimeSlot *)(modelRuntime->reserved10_37 + 0x10);
      dVar9 = dVar9 - 1;
    } while (dVar9 != 0);
  }
  return;
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
bool __thandor_cf_preserve_eax_ecx_edx
ArmyRuntimeSpawner_CreateLinkedChildInstanceCf
          (WorldMotionValue78 inheritedValue78,WorldMotionValue74 inheritedValue74,
          WorldMotionValue70 inheritedValue70,PckArmyAssetIdCatalog linkedArmyAssetId,
          WorldRuntimeContext *worldRuntime,ArmyRuntimeLinkedChildMaskSlotView *armyRuntime)

{
  ArmyRuntimeLinkedChildSlotMaskState4 *pAVar1;
  int *piVar2;
  ModelRuntimeNode *pMVar3;
  int iVar4;
  uint uVar5;
  ArmyRuntimeLinkedChildMaskSlotView *pAVar6;
  ArmyRuntimeCreateEaxCf5 AVar7;
  ModelRuntimeNode *modelNode1;
  
  uVar5 = 1;
  iVar4 = *(int *)((int)armyRuntime->definitionOrAsset + 0xc4);
  pAVar6 = armyRuntime;
  while ((linkedArmyAssetId != pAVar6->movementTarget0Q12 ||
         (((armyRuntime->articulatedContact).linkedChildSlotMaskState.linkedChildSlotMask & uVar5)
          != 0))) {
    pAVar6 = (ArmyRuntimeLinkedChildMaskSlotView *)&pAVar6->modelNodeRuntime;
    uVar5 = uVar5 * 2;
    iVar4 = iVar4 + -1;
    if (iVar4 == 0) {
      return true;
    }
  }
  modelNode1 = armyRuntime->modelNodeRuntime;
  AVar7 = ArmyRuntime_CreateInstanceFromAssetCf
                    (0,(modelNode1->modelPayload).worldRotationAngle2,
                     (modelNode1->worldTransform).translation.y,
                     (modelNode1->worldTransform).translation.x,
                     (armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex,
                     linkedArmyAssetId,worldRuntime);
  if (AVar7.carry) {
    return true;
  }
  piVar2 = *(int **)AVar7.eax;
  pAVar1 = &(armyRuntime->articulatedContact).linkedChildSlotMaskState;
  pAVar1->linkedChildSlotMask = pAVar1->linkedChildSlotMask | uVar5;
  armyRuntime->fallbackWorldYQ12 = armyRuntime->fallbackWorldYQ12 + -1;
  piVar2[0x18] = (int)armyRuntime;
  piVar2[0x2e] = 1;
  modelNode1 = (ModelRuntimeNode *)piVar2[1];
  piVar2[0x1c] = inheritedValue70;
  piVar2[0x1d] = inheritedValue74;
  pMVar3 = armyRuntime->modelNodeRuntime;
  piVar2[0x1e] = inheritedValue78;
  (modelNode1->childNodes[0]->modelPayload).localTranslationZQ12 =
       (pMVar3->childNodes[0]->modelPayload).localTranslationZQ12;
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNode1);
  piVar2[0xf] = (int)(((longlong)armyRuntime->actionVector2Q12 * (longlong)*(int *)(*piVar2 + 0x60))
                     / (longlong)*(int *)((int)armyRuntime->definitionOrAsset + 0x60));
  return false;
}


/* Address: 0x00527430.
   Ownership: gameplay/army/runtime.
   Purpose: Tests proximity across the base model and its attachment descriptors for two army runtimes; status is
   returned through CF.
   Local calls: ArmyRuntime_TestPositionDistanceWithinCombinedRadiusCf.
*/
bool __thandor_cf_preserve_eax_ecx_edx
ArmyRuntime_TestModelAttachmentProximityCf
          (ArmyRuntimeSlot *candidateArmyRuntime,ArmyRuntimeSlot *sourceArmyRuntime)

{
  ModelRuntimeSlot *pMVar1;
  int *piVar2;
  int iVar3;
  bool bVar4;
  bool bVar5;
  
  pMVar1 = (candidateArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  iVar3 = sourceArmyRuntime->factionIndex;
  bVar4 = ArmyRuntime_TestPositionDistanceWithinCombinedRadiusCf
                    ((UQ12)pMVar1->attachments140[3].childModelRuntimeOrSavedOffset00,
                     (UQ12)((sourceArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
                           attachments140[3].childModelRuntimeOrSavedOffset00,
                     candidateArmyRuntime->modelNodeRuntime,sourceArmyRuntime->modelNodeRuntime);
  bVar5 = false;
  if (bVar4) {
    for (; iVar3 != 0; iVar3 = iVar3 + -1) {
      piVar2 = (int *)sourceArmyRuntime[1].commandCoordinate0Q12;
      if ((piVar2 != (int *)0x0) &&
         (bVar5 = ArmyRuntime_TestPositionDistanceWithinCombinedRadiusCf
                            ((UQ12)pMVar1->attachments140[3].childModelRuntimeOrSavedOffset00,
                             *(UQ12 *)(*piVar2 + 0x1a0),candidateArmyRuntime->modelNodeRuntime,
                             (void *)piVar2[1]), !bVar5)) {
        return bVar5;
      }
      sourceArmyRuntime = (ArmyRuntimeSlot *)&sourceArmyRuntime->commandCoordinate0Q12;
    }
    bVar5 = true;
  }
  return bVar5;
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
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_DestroyInstanceAndRefreshUi
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
bool __thandor_cf_preserve_eax_ecx_edx
ArmyRuntime_TestClass13ProximityCandidateCf
          (ArmyRuntimeSlot *candidateArmyRuntime,ArmyRuntimeSlot *sourceArmyRuntime)

{
  ModelRuntimeSlot *pMVar1;
  ModelRuntimeNode *modelNodeRuntime;
  dword dVar2;
  ModelLookupEntryEaxCf5 MVar3;
  ModelLocalPointRegs12 MVar4;
  ModelRuntimeNode *modelNode1;
  
  if (((sourceArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C == 0xd) {
    pMVar1 = ((candidateArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->attachments140[3].
             childModelRuntimeOrSavedOffset00;
    modelNodeRuntime = sourceArmyRuntime->modelNodeRuntime;
    modelNode1 = candidateArmyRuntime->modelNodeRuntime;
    MVar3 = ModelLookupTable_ContainsPackedKeyCf(1,5,(modelNodeRuntime->modelPayload).modelResource)
    ;
    if (!MVar3.carry) {
      MVar4 = ModelNodeRuntime_TransformLocalPointRegs(MVar3.entry,modelNodeRuntime);
      dVar2 = FixedMath_Length2(MVar4.ecx - (modelNode1->worldTransform).translation.y,
                                MVar4.eax - (modelNode1->worldTransform).translation.x);
      if ((int)dVar2 <= (int)(pMVar1 + 6)) {
        return true;
      }
    }
  }
  return false;
}


/* Address: 0x00526510.
   Ownership: gameplay/army/runtime.
   Purpose: Typed parameters: p3 soundAssetIndex→SoundAssetIndex_V343. Calling convention, complete VariableStorage
   serialization, function bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: GameFactionRuntime_TestCapabilityBitClearCf [gameplay/faction/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_TryPlayMappedTerrainSoundAtWorldPoint
          (FactionRuntimeIndex factionIndex,Q12 worldYQ12,Q12 worldXQ12,
          SoundAssetIndex soundAssetIndex,WorldRuntimeContext *worldContext)

{
  undefined4 *puVar1;
  FieldGridDimension FVar2;
  dword capabilityBitIndex;
  int iVar3;
  uint uVar4;
  int iVar5;
  bool bVar6;
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
      capabilityBitIndex = worldContext->activeFactionRuntimeIndex;
      if (factionIndex != capabilityBitIndex) {
        bVar6 = GameFactionRuntime_TestCapabilityBitClearCf(capabilityBitIndex,factionIndex);
        if (((bVar6) &&
            ((fieldGrid1->cells[FVar2 * iVar5 + iVar3].runtime60_6B[capabilityBitIndex + 0x10] &
             0x10) != 0)) &&
           (0x10 < g_GameFactionRuntimeImage.records[capabilityBitIndex].relationTransitionTick)) {
          g_GameFactionRuntimeImage.records[capabilityBitIndex].relationTransitionTick = 0;
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
uint __thandor_void_preserve_eax_ecx
ArmyRuntimeSpawner_ComputeRemainingLinkedAssetMetric
          (ArmyRuntimeLinkedChildMaskSlotView *armyRuntime)

{
  FactionRuntimeIndex factionIndex;
  int iVar1;
  uint uVar2;
  uint uVar3;
  ArmyRuntimeLinkedChildMaskSlotView *pAVar4;
  ArmyRegistryEaxCf5_51b6d0 AVar5;
  ModelDefinitionLookupEaxCf5 MVar6;
  
  uVar2 = 0;
  factionIndex = (armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex;
  uVar3 = 1;
  iVar1 = *(int *)((int)armyRuntime->definitionOrAsset + 0xc4);
  pAVar4 = armyRuntime;
  do {
    if (((armyRuntime->articulatedContact).linkedChildSlotMaskState.linkedChildSlotMask & uVar3) ==
        0) {
      AVar5 = ArmyAssetRegistry_FindByIdCf(pAVar4->movementTarget0Q12);
      if (!AVar5.carry) {
        MVar6 = ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                          (factionIndex,(AVar5.eax)->rootNodeOffsetOrPointer);
        uVar2 = uVar2 + MVar6.modelDefinition[0x20].flags;
      }
    }
    pAVar4 = (ArmyRuntimeLinkedChildMaskSlotView *)&pAVar4->modelNodeRuntime;
    uVar3 = uVar3 * 2;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  return uVar2 >> 5;
}


/* Address: 0x00527230.
   Ownership: gameplay/army/runtime.
   Purpose: Projects the spawned runtime position through the field mask and plays its configured one-shot creation
   sound when the selected sound resource is available.
   Cross-module calls: TerrainGrid_TestProjectedCellMaskBits01Cf [world/terrain/grid],
   SpatialSound_PlayPositionedOneShot [audio/spatial/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeSpawner_PlayCreationSound(ArmyRuntimeSlot *armyRuntime,WorldRuntimeContext *worldRuntime)

{
  ModelRuntimeSlot *pMVar1;
  ModelRuntimeNode *pMVar2;
  uint uVar3;
  DirectSoundVoiceSet **voiceSetRef;
  bool bVar4;
  
  pMVar1 = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  pMVar2 = armyRuntime->modelNodeRuntime;
  uVar3 = pMVar1[1].classLinkState.classState70;
  if (((uVar3 != 0) && (uVar3 < worldRuntime->dwordArrayCount)) &&
     (worldRuntime->dwordArray != (dword *)0x0)) {
    voiceSetRef = (DirectSoundVoiceSet **)worldRuntime->dwordArray[uVar3];
    if (voiceSetRef != (DirectSoundVoiceSet **)0x0) {
      bVar4 = TerrainGrid_TestProjectedCellMaskBits01Cf
                        ((pMVar2->worldTransform).translation.y,
                         (pMVar2->worldTransform).translation.x,worldRuntime);
      if (!bVar4) {
        SpatialSound_PlayPositionedOneShot
                  ((pMVar1->classLinkState).classState7C,(pMVar1->classLinkState).classState78,
                   &(pMVar2->worldTransform).translation,voiceSetRef);
      }
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
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_TrySpawnDefinitionEffectAtWorldPoint
          (ArmyRuntimeSlot *armyRuntime,WorldRuntimeContext *worldContext)

{
  ModelRuntimeSlot *pMVar1;
  ModelRuntimeNode *pMVar2;
  uint uVar3;
  DirectSoundVoiceSet **voiceSetRef;
  bool bVar4;
  
  pMVar1 = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  pMVar2 = armyRuntime->modelNodeRuntime;
  uVar3 = pMVar1[1].classLinkState.armyLinkOrState6C.classState;
  if (((uVar3 != 0) && (uVar3 < worldContext->dwordArrayCount)) &&
     (worldContext->dwordArray != (dword *)0x0)) {
    voiceSetRef = (DirectSoundVoiceSet **)worldContext->dwordArray[uVar3];
    if (voiceSetRef != (DirectSoundVoiceSet **)0x0) {
      bVar4 = TerrainGrid_TestProjectedCellMaskBits01Cf
                        ((pMVar2->worldTransform).translation.y,
                         (pMVar2->worldTransform).translation.x,worldContext);
      if (!bVar4) {
        SpatialSound_PlayPositionedOneShot
                  ((pMVar1->classLinkState).classState7C,(pMVar1->classLinkState).classState78,
                   &(pMVar2->worldTransform).translation,voiceSetRef);
      }
    }
  }
  return;
}


/* Address: 0x005273D0.
   Ownership: gameplay/army/runtime.
   Purpose: Compares squared XY distance between two position runtimes with the square of their combined Q12 radii
   and returns the outside/inside status through CF.
*/
bool __thandor_cf_preserve_eax_ecx_edx
ArmyRuntime_TestPositionDistanceWithinCombinedRadiusCf
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
    return false;
  }
  return true;
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
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
          (EffectCreationFlagBits effectFlags,Q12 worldZQ12,Q12 worldYQ12,Q12 worldXQ12,
          ModelAttachmentOrdinal modelPointOrdinal,PckEffectDefinitionIdCatalog effectDefinitionId,
          void *sourceRuntime,void *modelPointTable,WorldRuntimeContext *worldContext)

{
  ArmyRuntimeSlot *sourceArmyRuntime;
  ArmyRuntimeSlot *candidateArmyRuntime;
  Q12 QVar1;
  uint uVar2;
  dword dVar3;
  int x;
  uint currentBestScore;
  int y;
  int iVar4;
  WorldOwnerListNode100 *worldNode1;
  ModelPackedPointRecord *localPointRecord;
  ModelLocalPointRegs12 MVar5;
  WorldOwnerListNode100 *worldNode2;
  uint uStack_20;
  
  worldNode2 = (WorldOwnerListNode100 *)0x0;
  worldNode1 = worldContext->ownerListHead;
  uStack_20 = 0xffffffff;
  currentBestScore = 0;
  sourceArmyRuntime = *(ArmyRuntimeSlot **)(*(int *)((int)sourceRuntime + 0x48) + 8);
  do {
    if ((((worldNode1->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) && (worldNode1 != sourceRuntime))
        && (candidateArmyRuntime = *(ArmyRuntimeSlot **)((int)worldNode1->runtimePayload + 8),
           (*(uint *)((int)worldNode1->runtimePayload + 0xec) & 8) == 0)) &&
       ((candidateArmyRuntime->factionIndex != 0 &&
        (candidateArmyRuntime->factionIndex != sourceArmyRuntime->factionIndex)))) {
      sourceArmyRuntime->runtimeState4C =
           *(dword *)((((candidateArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
                      definitionOrSavedId).savedIdOrOffset + 0xdc);
      uVar2 = AiCombatTarget_EvaluateCandidateScore
                        (currentBestScore,1,0xffffffff,0xffffffff,candidateArmyRuntime,
                         sourceArmyRuntime);
      if (currentBestScore < uVar2) {
        currentBestScore = uVar2;
        worldNode2 = worldNode1;
      }
    }
    worldNode1 = worldNode1->nextNode;
  } while (worldNode1 != (WorldOwnerListNode100 *)0x0);
  if (worldNode2 != (WorldOwnerListNode100 *)0x0) {
    QVar1 = worldNode2->worldZQ12;
    y = worldNode2->worldXQ12 - worldXQ12;
    x = worldNode2->worldYQ12 - worldYQ12;
    iVar4 = *(int *)(*(int *)worldNode2->runtimePayload + 0xdc);
    dVar3 = FixedMath_Length2(x,y);
    if ((int)(dVar3 - iVar4) < 0x1001) {
      uStack_20 = 0;
      worldXQ12 = y + worldXQ12;
      worldYQ12 = x + worldYQ12;
      worldZQ12 = QVar1;
    }
  }
  ModelNodeRuntime_RebuildTransformsFromRoot(sourceRuntime);
  iVar4 = *(int *)((int)modelPointTable + 0x30);
  localPointRecord = (ModelPackedPointRecord *)(iVar4 + *(int *)(iVar4 + 0xe4));
  for (iVar4 = *(int *)(iVar4 + 0xe8); iVar4 != 0; iVar4 = iVar4 + -1) {
    if (localPointRecord->packedLookupKey == (modelPointOrdinal << 4 | 2)) {
      MVar5 = ModelNodeRuntime_TransformLocalPointRegs(localPointRecord,sourceRuntime);
      ShotRuntimePool_CreateProjectileFromDefinition
                (effectFlags,*(ArmyRuntimeSlot **)(*(int *)((int)sourceRuntime + 0x48) + 8),
                 worldZQ12,(MVar5.ecx - *(int *)((int)sourceRuntime + 0x98) & uStack_20) + worldYQ12
                 ,(MVar5.eax - *(int *)((int)sourceRuntime + 0x94) & uStack_20) + worldXQ12,
                 MVar5.edx,MVar5.ecx,MVar5.eax,(ShotDefinition *)effectDefinitionId,worldContext);
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
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_ProcessReadyAttachmentChannels
          (WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  ModelRuntimeSlot *pMVar1;
  uint uVar2;
  Q12 worldXQ12;
  Q12 worldXQ12_00;
  dword dVar3;
  int iVar4;
  int iVar5;
  ModelPackedPointRecord *pMVar6;
  ModelLocalPointRegs12 MVar7;
  ModelRuntimeNode *modelNode1;
  
  uVar2 = 0;
  iVar5 = *(int *)(*(int *)((modelRuntime->definitionOrSavedId).savedIdOrOffset + 100) + 0x30);
  modelRuntime->definitionValue60_3C = 0;
  do {
    if (*(int *)(modelRuntime->reserved10_37 + uVar2 * 4 + 0x30) == 0) {
      iVar4 = *(int *)(iVar5 + 0xe8);
      pMVar6 = (ModelPackedPointRecord *)(iVar5 + *(int *)(iVar5 + 0xe4));
      if (iVar4 != 0) {
        do {
          if (uVar2 * 0x10 + 3 == pMVar6->packedLookupKey) {
            MVar7 = ModelNodeRuntime_TransformLocalPointRegs
                              (pMVar6,(modelRuntime->rootModelNodeOrSavedOffset).modelNode);
            worldXQ12 = MVar7.ecx;
            modelNode1 = (modelRuntime->rootModelNodeOrSavedOffset).modelNode;
            EffectRuntimePool_CreateInstanceFromDefinitionCf
                      (EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY,
                       *(EffectRuntimeOwnerReference4 *)
                        &modelRuntime->linkedModelRuntimeOrSavedOffset,
                       (modelNode1->modelPayload).worldRotationAngle2,
                       (modelNode1->modelPayload).worldRotationAngle1,
                       (modelNode1->modelPayload).worldRotationAngle0,MVar7.edx,worldXQ12,MVar7.eax,
                       *(EffectDefinition **)
                        ((modelRuntime->definitionOrSavedId).savedIdOrOffset + 0x80 + uVar2 * 8),
                       worldRuntime);
          }
          pMVar6 = pMVar6 + 1;
          iVar4 = iVar4 + -1;
        } while (iVar4 != 0);
      }
    }
    uVar2 = uVar2 + 1;
  } while (uVar2 < 8);
  iVar5 = *(int *)((modelRuntime->definitionOrSavedId).savedIdOrOffset + 100);
  uVar2 = 0;
  if ((*(int *)(iVar5 + 0x14) != 0) &&
     (iVar5 = *(int *)(iVar5 + 0x18), iVar4 = *(int *)(iVar5 + 0x30),
     (*(uint *)(iVar5 + 4) & 0xf) == 0)) {
    do {
      if (*(int *)(modelRuntime->reserved10_37 + uVar2 * 4 + 0x30) == 0) {
        iVar5 = *(int *)(iVar4 + 0xe8);
        pMVar6 = (ModelPackedPointRecord *)(iVar4 + *(int *)(iVar4 + 0xe4));
        if (iVar5 != 0) {
          do {
            if (((uVar2 * 0x10 + 3 == pMVar6->packedLookupKey) &&
                (((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->childCount != 0)) &&
               (modelNode1 = ((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->childNodes[0],
               modelNode1 != (ModelRuntimeNode *)0x0)) {
              MVar7 = ModelNodeRuntime_TransformLocalPointRegs(pMVar6,modelNode1);
              worldXQ12_00 = MVar7.ecx;
              EffectRuntimePool_CreateInstanceFromDefinitionCf
                        (EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY,
                         *(EffectRuntimeOwnerReference4 *)
                          &modelRuntime->linkedModelRuntimeOrSavedOffset,0,0x4000,0,MVar7.edx,
                         worldXQ12_00,MVar7.eax,
                         *(EffectDefinition **)
                          ((modelRuntime->definitionOrSavedId).savedIdOrOffset + 0x80 + uVar2 * 8),
                         worldRuntime);
            }
            pMVar6 = pMVar6 + 1;
            iVar5 = iVar5 + -1;
          } while (iVar5 != 0);
        }
      }
      uVar2 = uVar2 + 1;
    } while (uVar2 < 8);
  }
  for (dVar3 = modelRuntime->attachmentCount0C; dVar3 != 0; dVar3 = dVar3 - 1) {
    pMVar1 = modelRuntime->attachments140[0].childModelRuntimeOrSavedOffset00;
    if (pMVar1 != (ModelRuntimeSlot *)0x0) {
      pMVar1->definitionValue60_3C = 0;
      (pMVar1->linkedModelRuntimeOrSavedOffset).modelRuntime = (ModelRuntimeSlot *)0x0;
    }
    modelRuntime = (ModelRuntimeSlot *)(modelRuntime->reserved10_37 + 0x10);
  }
  return;
}


/* Address: 0x0052A760.
   Ownership: gameplay/army/runtime.
   Purpose: Invokes the class-indexed method-D callback for the current runtime and recursively dispatches the same
   callback through its child hierarchy. Class method-D partition slots 24-47 receive (worldRuntime, armyRuntime).
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeHierarchy_DispatchClassMethodDRecursive
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  int iVar1;
  
  iVar1 = armyRuntime->factionIndex;
  (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classMethodD
    [((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C])
            (worldRuntime,armyRuntime);
  for (; iVar1 != 0; iVar1 = iVar1 + -1) {
    if ((ArmyRuntimeSlot *)armyRuntime[1].commandCoordinate0Q12 != (ArmyRuntimeSlot *)0x0) {
      ArmyRuntimeHierarchy_DispatchClassMethodDRecursive
                (worldRuntime,(ArmyRuntimeSlot *)armyRuntime[1].commandCoordinate0Q12);
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
void __thandor_void_preserve_eax_ecx
ArmyRuntime_RebuildDerivedSelectionMetrics(ArmyRuntimeSlot *armyRuntime)

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
  if ((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime != (ModelRuntimeSlot *)0x0) {
    ModelRuntimeHierarchy_AccumulateDerivedSelectionMetrics
              ((int *)(armyRuntime->modelRuntimeOrSavedOffset).modelRuntime);
  }
  return;
}


/* Address: 0x0051DBA0.
   Ownership: gameplay/army/runtime.
   Purpose: The current implementation preserves the input state, clears carry, and therefore permits every tested
   point.
*/
bool __thandor_cf_preserve_eax_ecx_edx
ArmyRuntime_TestWorldPointAllowedDefaultCf(dword arg0,dword arg1,dword arg2)

{
  return false;
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
ArmyRuntimeCreateEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ArmyRuntime_CreateInstanceFromAssetCf
          (WorldObjectAllocationFlags creationFlags,AngleTurn32 orientationAngle,Q12 worldXQ12,
          Q12 worldYQ12,FactionRuntimeIndex factionIndex,PckArmyAssetIdCatalog armyAssetId,
          WorldRuntimeContext *worldRuntime)

{
  ArmyAssetRecordPrefix *definitionNode_00;
  GraphicsTextureSet *textureSet;
  GraphicsPaletteAsset *paletteAsset;
  dword dVar1;
  PckArmyAssetIdCatalog PVar2;
  GameEntityRuntime *pGVar3;
  ModelRuntimeNode *modelNodeRuntime;
  PckModelDefinitionIdCatalog modelDefinitionId;
  ModelRuntimeNode *pMVar4;
  uint armySlotsRemaining;
  int iVar5;
  ArmyAssetRecordPrefix **ppAVar6;
  ArmyRuntimeSlot *armyRuntime;
  bool bVar7;
  ArmyRuntimeCreateEaxCf5 AVar8;
  ModelDefinitionLookupEaxCf5 MVar9;
  ModelNodeCreateEaxCf5 MVar10;
  ArmyRuntimeCreateEaxCf5 AVar11;
  dword slotScanContinueValue;
  ArmyAssetRuntimeSemanticView80 *definitionNode;
  
  armySlotsRemaining = 0x400;
  armyRuntime = g_ArmyRuntimeSlots;
  slotScanContinueValue = (dword)g_ArmyRuntimeSlots;
  while (pMVar4 = (ModelRuntimeNode *)0x14, slotScanContinueValue != 0) {
    if (armyRuntime->modelNodeRuntime == (ModelRuntimeNode *)0x0) {
      ppAVar6 = g_ArmyAssetRecordRegistry;
      iVar5 = 0x300;
      goto ArmyRuntime_CreateInstanceFromAsset_ScanAssetDefinitionRegistry;
    }
    armyRuntime = armyRuntime + 1;
    armySlotsRemaining = armySlotsRemaining - 1;
    slotScanContinueValue = armySlotsRemaining;
  }
  goto ArmyRuntime_CreateInstanceFromAsset_ReturnCreationFailure;
  while( true ) {
    ppAVar6 = ppAVar6 + 1;
    iVar5 = iVar5 + -1;
    if (iVar5 == 0) break;
ArmyRuntime_CreateInstanceFromAsset_ScanAssetDefinitionRegistry:
    definitionNode_00 = *ppAVar6;
    if ((definitionNode_00 != (ArmyAssetRecordPrefix *)0x0) &&
       (definitionNode_00->registryId == armyAssetId)) {
      armyRuntime->armyAssetId = armyAssetId;
      if (((creationFlags & 2) != 0) && (factionIndex == worldRuntime->activeFactionRuntimeIndex)) {
        MVar9 = ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                          (factionIndex,definitionNode_00->rootNodeOffsetOrPointer);
        MVar9.modelDefinition[0x24].byteSize = MVar9.modelDefinition[0x24].byteSize + 1;
      }
      if (7 < (uint)factionIndex) {
        factionIndex = 7;
      }
      armyRuntime->factionIndex = factionIndex;
      if ((creationFlags & 4) != 0) {
        ModelDefinitionHierarchy_UnlockSelectedLinkedTechnology
                  (factionIndex,(ModelDefinitionHierarchyNodeAddress32)definitionNode_00);
      }
      textureSet = g_ArmyGraphicsBindings[factionIndex].textureSet;
      paletteAsset = g_ArmyGraphicsBindings[factionIndex].paletteAsset;
      dVar1 = definitionNode_00[7].selectionDetailTemplateVariantIndex;
      PVar2 = definitionNode_00[7].registryId;
      armyRuntime->definitionClassValue80 = definitionNode_00[7].byteSize;
      armyRuntime->definitionClassValue84 = dVar1;
      armyRuntime->definitionClassValue88 = PVar2;
      pGVar3 = (GameEntityRuntime *)definitionNode_00[1].byteSize;
      armyRuntime->runtimeState90 = 0;
      armyRuntime->runtimeState44 = 0;
      armyRuntime->runtimeState48 = 0;
      armyRuntime->runtimeState94 = 0;
      dVar1 = definitionNode_00->rootNodeOffsetOrPointer;
      (armyRuntime->articulatedContact).fallbackPosition0Q12 = worldYQ12;
      (armyRuntime->articulatedContact).fallbackPosition1Q12 = worldXQ12;
      armyRuntime->linkedEntityRuntime = pGVar3;
      (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime = (ModelRuntimeSlot *)0x0;
      armyRuntime->runtimeState8C = 0;
      modelDefinitionId = ModelDefinition_SelectFactionUnlockedLinkedIdCf(factionIndex,dVar1);
      MVar10 = ModelRuntimePool_CreateInstanceByDefinitionIdCf
                         (paletteAsset,textureSet,armyRuntime,modelDefinitionId,worldRuntime);
      pMVar4 = MVar10.modelNode;
      if (!MVar10.carry) {
        modelNodeRuntime = (ModelRuntimeNode *)(pMVar4->common).nextNode;
        (armyRuntime->modelRuntimeOrSavedOffset).savedIdOrOffset = (dword)pMVar4;
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
        (armyRuntime->linkedChildOverloadedState).primaryCoordinateCommandOrHistory.
        coordinateOrTargetQ12 = worldYQ12;
        (armyRuntime->linkedChildOverloadedState).secondaryCoordinateCommandOrHistory.
        coordinateOrTargetQ12 = worldXQ12;
        armyRuntime->movementPosition0Q12 = worldYQ12;
        armyRuntime->movementPosition1Q12 = worldXQ12;
        armyRuntime->movementStateFlags = 0;
        armyRuntime->actionVector1Q12 = 0;
        armyRuntime->terrainOccupancyMask0 = 0;
        armyRuntime->terrainOccupancyMask1 = 0;
        armyRuntime->runtimeState40 = 0;
        bVar7 = ModelNodeRuntime_InstantiateLinkedChildrenRecursiveCf
                          (factionIndex,paletteAsset,textureSet,
                           (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime,dVar1,worldRuntime)
        ;
        pMVar4 = (ModelRuntimeNode *)worldYQ12;
        if (!bVar7) {
          WorldRuntime_LinkNodeIntoOwnerListD8((WorldOwnerListNode100 *)modelNodeRuntime);
          ModelNodeRuntime_RecomputeSubtreeBoundingRadius(modelNodeRuntime);
          iVar5 = *(int *)armyRuntime->modelRuntimeOrSavedOffset;
          (*g_ArmyPlacementContactKindDispatchTable.callbacks[*(int *)(iVar5 + 0x278)])
                    (*(Q12 *)(iVar5 + 0x54),(modelNodeRuntime->worldTransform).translation.y,
                     (modelNodeRuntime->worldTransform).translation.x,modelNodeRuntime,worldRuntime)
          ;
          ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
          armyRuntime->depthBinClass =
               *(ModelRuntimeClassId *)
                (&g_ArmyRuntimeDepthBinClassByModelClass + *(int *)(iVar5 + 0x4c) * 4);
          ModelNodeRuntime_UpdateDepthBinMasks
                    (*(DepthIntervalRadius32 *)(iVar5 + 0xdc),modelNodeRuntime);
          ArmyRuntime_InitializeTerrainOccupancyFlags(worldRuntime,armyRuntime);
          UiModelControl_RefreshStateTint(modelNodeRuntime);
          ArmyRuntime_RebuildDerivedSelectionMetrics(armyRuntime);
          AVar11.carry = false;
          AVar11.eax = (dword)armyRuntime;
          return AVar11;
        }
      }
      goto ArmyRuntime_CreateInstanceFromAsset_ReturnCreationFailure;
    }
  }
  (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,armyAssetId,g_PackageLastErrorPath)
  ;
  pMVar4 = (ModelRuntimeNode *)0x41;
ArmyRuntime_CreateInstanceFromAsset_ReturnCreationFailure:
  AVar8.carry = true;
  AVar8.eax = (dword)pMVar4;
  return AVar8;
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
  ulonglong neighborhoodClassificationPair;
  TerrainOccupancyResolvedMasksRegs12 TVar1;
  ModelRuntimeNode *modelNode;
  void *definition;
  
  neighborhoodClassificationPair._4_4_ =
       TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                 (*(Q12 *)((((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
                           definitionOrSavedId).savedIdOrOffset + 0xdc),
                  (armyRuntime->modelNodeRuntime->worldTransform).translation.y,
                  (armyRuntime->modelNodeRuntime->worldTransform).translation.x,
                  worldRuntime->fieldGrid);
  armyRuntime->terrainOccupancyMask0 = neighborhoodClassificationPair._4_4_;
  modelNode = armyRuntime->modelNodeRuntime;
  TVar1 = TerrainOccupancyMask_ResolveRuntimeClassFlags
                    (modelNode->runtimeFlags,armyRuntime->terrainOccupancyMask1,
                     armyRuntime->terrainOccupancyMask0,
                     (char)worldRuntime->activeFactionRuntimeIndex);
  occupancyRuntimeFlags = TVar1.runtimeFlags;
  modelNode->runtimeFlags = modelNode->runtimeFlags & 0xffffeff3;
  armyRuntime->terrainOccupancyMask0 = TVar1.primaryOccupancyMask;
  armyRuntime->terrainOccupancyMask1 = TVar1.secondaryOccupancyMask;
  definition = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
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
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_UpdateAnimatedModelSubnodes
          (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView200 *modelRuntime)

{
  dword *pdVar1;
  ModelRuntimeNode *modelNodeRuntime;
  ModelDefinitionRuntimeSemanticView280 *pMVar2;
  Q12 updatedChildTranslationZQ12;
  int iVar3;
  ModelRuntimeNode *animatedChildNode;
  int childTranslationZQ12;
  ModelRuntimeNode *modelNode1;
  
  if (((modelRuntime->classState).classStateEC & 1) == 0) {
    if (((modelRuntime->classState).classStateEC & 4) != 0) {
      pdVar1 = &(modelRuntime->classState).classStateEC;
      *pdVar1 = *pdVar1 ^ 4;
      ArmyRuntime_RebuildDerivedSelectionMetrics(modelRuntime->ownerArmyRuntime);
    }
    modelNodeRuntime = modelRuntime->rootModelNode;
    pMVar2 = modelRuntime->modelDefinition;
    animatedChildNode = modelNodeRuntime->childNodes[0];
    if (modelNodeRuntime->childCount != 0) {
      if (animatedChildNode != (ModelRuntimeNode *)0x0) {
        (animatedChildNode->modelPayload).localRotationAngle2 =
             *(int *)(pMVar2->reserved00C_023 + 4) * g_InGameSimulationStepTicks +
             (animatedChildNode->modelPayload).localRotationAngle2 & 0xffff;
        animatedChildNode->runtimeFlags = animatedChildNode->runtimeFlags | 1;
      }
      modelNode1 = modelNodeRuntime->childNodes[1];
      if (1 < modelNodeRuntime->childCount) {
        if (modelNode1 != (ModelRuntimeNode *)0x0) {
          (modelNode1->modelPayload).localRotationAngle2 =
               *(int *)(pMVar2->reserved00C_023 + 0x10) * g_InGameSimulationStepTicks +
               (modelNode1->modelPayload).localRotationAngle2 & 0xffff;
          modelNode1->runtimeFlags = modelNode1->runtimeFlags | 1;
        }
        modelNode1 = modelNodeRuntime->childNodes[2];
        if ((2 < modelNodeRuntime->childCount) && (modelNode1 != (ModelRuntimeNode *)0x0)) {
          childTranslationZQ12 = (modelNode1->modelPayload).localTranslationZQ12;
          iVar3 = *(int *)(pMVar2->reserved00C_023 + 8) * g_InGameSimulationStepTicks;
          if (((modelRuntime->classState).classStateEC & 2) == 0) {
            updatedChildTranslationZQ12 = childTranslationZQ12 - iVar3;
            if (updatedChildTranslationZQ12 < (int)pMVar2->runtimeValue24) {
              updatedChildTranslationZQ12 = pMVar2->runtimeValue24;
              pdVar1 = &(modelRuntime->classState).classStateEC;
              *pdVar1 = *pdVar1 ^ 2;
            }
          }
          else {
            updatedChildTranslationZQ12 = childTranslationZQ12 + iVar3;
            if ((int)pMVar2->runtimeValue28 < updatedChildTranslationZQ12) {
              updatedChildTranslationZQ12 = pMVar2->runtimeValue28;
              pdVar1 = &(modelRuntime->classState).classStateEC;
              *pdVar1 = *pdVar1 ^ 2;
            }
          }
          (modelNode1->modelPayload).localTranslationZQ12 = updatedChildTranslationZQ12;
          modelNode1->runtimeFlags = modelNode1->runtimeFlags | 1;
        }
      }
    }
    ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
  }
  else if (((modelRuntime->classState).classStateEC & 4) == 0) {
    pdVar1 = &(modelRuntime->classState).classStateEC;
    *pdVar1 = *pdVar1 ^ 4;
    ArmyRuntime_RebuildDerivedSelectionMetrics(modelRuntime->ownerArmyRuntime);
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
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
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_UpdateTimedShotAndEffectEmitters
          (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView200 *modelRuntime)

{
  dword *pdVar1;
  ModelDefinitionRuntimeSemanticView280 *pMVar2;
  int iVar3;
  dword dVar4;
  uint uVar5;
  sdword sVar6;
  dword dVar7;
  int iVar8;
  dword worldY;
  AngleTurn32 orientationAngle0;
  uint uVar9;
  uint *puVar10;
  dword worldZQ12;
  AngleTurn32 orientationAngle1;
  ModelRuntimeNode *modelNode1;
  EffectDefinition *effectDefinition1;
  bool bVar11;
  ModelLookupEntryEaxCf5 MVar12;
  FixedDirectionXyzRegs12 FVar13;
  ModelLocalPointRegs12 MVar14;
  AngleTurn32 orientationAngle2;
  GraphicsWorldCoordinateQ12 launchWorldZQ12;
  GraphicsWorldCoordinateQ12 launchWorldYQ12;
  GraphicsWorldCoordinateQ12 launchWorldXQ12;
  ShotDefinition *shotDefinition;
  EffectDefinition *effectDefinition;
  WorldRuntimeContext *worldContext1;
  
  pMVar2 = modelRuntime->modelDefinition;
  pdVar1 = &(modelRuntime->classState).enabledStateE4;
  bVar11 = SBORROW4(*pdVar1,g_InGameSimulationStepTicks);
  *pdVar1 = *pdVar1 - g_InGameSimulationStepTicks;
  if ((*pdVar1 == 0 || bVar11 != (int)*pdVar1 < 0) &&
     ((pMVar2->shotDefinitionReference168).definition != (ShotDefinition *)0xffffffff)) {
    uVar9 = 0;
    if (*(int *)(pMVar2->reserved16C_173 + 4) != 0) {
      dVar4 = (*g_RandomGeneratorState.next)();
      uVar9 = dVar4 % *(uint *)(pMVar2->reserved16C_173 + 4);
    }
    shotDefinition = (pMVar2->shotDefinitionReference168).definition;
    modelNode1 = modelRuntime->rootModelNode;
    (modelRuntime->classState).enabledStateE4 = uVar9 + *(int *)pMVar2->reserved16C_173;
    launchWorldXQ12 = (modelNode1->worldTransform).translation.x;
    launchWorldYQ12 = (modelNode1->worldTransform).translation.y;
    launchWorldZQ12 = (modelNode1->worldTransform).translation.z;
    worldContext1 = worldRuntime;
    FVar13 = FixedMath_DirectionFromAnglesScaledRegs
                       ((modelNode1->modelPayload).worldRotationAngle1,
                        (modelNode1->modelPayload).worldRotationAngle0,0x1000);
    ShotRuntimePool_CreateProjectileFromDefinition
              (0,modelRuntime->ownerArmyRuntime,FVar13.edx + launchWorldZQ12,
               FVar13.ecx + launchWorldYQ12,FVar13.eax + launchWorldXQ12,launchWorldZQ12,
               launchWorldYQ12,launchWorldXQ12,shotDefinition,worldContext1);
  }
  pMVar2 = modelRuntime->modelDefinition;
  pdVar1 = &(modelRuntime->classState).enabledStateE8;
  bVar11 = SBORROW4(*pdVar1,g_InGameSimulationStepTicks);
  *pdVar1 = *pdVar1 - g_InGameSimulationStepTicks;
  if ((*pdVar1 != 0 && bVar11 == (int)*pdVar1 < 0) ||
     (((pMVar2->effectDefinitionReference174).definition == (EffectDefinition *)0x0 &&
      ((pMVar2->effectDefinitionReference58).definition == (EffectDefinition *)0x0))))
  goto ArmyRuntime_UpdateTimedShotAndEffectEmitters_UpdateDamageThresholdEffectAndReturn;
  uVar9 = 0;
  if (*(int *)(pMVar2->reserved178_187 + 4) != 0) {
    dVar4 = (*g_RandomGeneratorState.next)();
    uVar9 = dVar4 % *(uint *)(pMVar2->reserved178_187 + 4);
  }
  (modelRuntime->classState).enabledStateE8 = uVar9 + *(int *)pMVar2->reserved178_187;
  dVar4 = pMVar2->serializedNodeOffsetOrPointer64;
  if (pMVar2->runtimeClassId4C == MODEL_RUNTIME_CLASS_21_AIRCRAFT) {
    dVar4 = *(dword *)(dVar4 + 0x18);
  }
  iVar3 = *(int *)(dVar4 + 0x30);
  iVar8 = *(int *)(iVar3 + 0xe8);
  worldContext1 = worldRuntime;
  if (iVar8 == 0) {
ArmyRuntime_UpdateTimedShotAndEffectEmitters_UseModelWorldPositionForEffectEmitter:
    modelNode1 = modelRuntime->rootModelNode;
    dVar4 = (modelNode1->worldTransform).translation.x;
    worldY = (modelNode1->worldTransform).translation.y;
    worldZQ12 = (modelNode1->worldTransform).translation.z;
  }
  else {
    uVar9 = 0;
    puVar10 = (uint *)(iVar3 + *(int *)(iVar3 + 0xe4));
    do {
      if (((*puVar10 & 0xf) == 6) && (uVar9 <= *puVar10 >> 4)) {
        uVar9 = (*puVar10 >> 4) + 1;
      }
      iVar8 = iVar8 + -1;
      puVar10 = puVar10 + 4;
    } while (iVar8 != 0);
    if (uVar9 == 0)
    goto ArmyRuntime_UpdateTimedShotAndEffectEmitters_UseModelWorldPositionForEffectEmitter;
    uVar5 = (modelRuntime->classState).classStateE0;
    if ((pMVar2->runtimeValue68 & 1) == 0) {
      uVar5 = (*g_RandomGeneratorState.next)();
    }
    dVar4 = pMVar2->serializedNodeOffsetOrPointer64;
    if (pMVar2->runtimeClassId4C == MODEL_RUNTIME_CLASS_21_AIRCRAFT) {
      dVar4 = *(dword *)(dVar4 + 0x18);
    }
    MVar12 = ModelLookupTable_ContainsPackedKeyCf
                       (uVar5 % uVar9,6,*(ModelResourceHitTestAndRenderView210 **)(dVar4 + 0x30));
    if (MVar12.carry)
    goto ArmyRuntime_UpdateTimedShotAndEffectEmitters_UseModelWorldPositionForEffectEmitter;
    modelNode1 = modelRuntime->rootModelNode;
    if (pMVar2->runtimeClassId4C == MODEL_RUNTIME_CLASS_21_AIRCRAFT) {
      modelNode1 = modelNode1->childNodes[0];
    }
    MVar14 = ModelNodeRuntime_TransformLocalPointRegs(MVar12.entry,modelNode1);
    worldZQ12 = MVar14.edx;
    worldY = MVar14.ecx;
    dVar4 = MVar14.eax;
  }
  effectDefinition1 = (pMVar2->effectDefinitionReference174).definition;
  effectDefinition = effectDefinition1;
  sVar6 = FieldGrid_GetNearestWaterDelta(worldY,dVar4,worldRuntime->fieldGrid);
  if (0 < sVar6) {
    effectDefinition1 = (pMVar2->effectDefinitionReference58).definition;
    effectDefinition = effectDefinition1;
  }
  if ((effectDefinition1 == (EffectDefinition *)0x0) ||
     ((effectDefinition1->transitionPrefix).transitionKind !=
      EFFECT_TRANSITION_INTEGRATE_LINEAR_MOTION_AND_SHADING_POSITION)) {
    modelNode1 = modelRuntime->rootModelNode;
    orientationAngle2 = (modelNode1->modelPayload).worldRotationAngle0;
    orientationAngle1 = (modelNode1->modelPayload).worldRotationAngle1;
    orientationAngle0 = (modelNode1->modelPayload).worldRotationAngle2;
  }
  else {
    dVar7 = (*g_RandomGeneratorState.next)();
    orientationAngle0 = dVar7 & 0xffff;
    orientationAngle1 = 0x4000 - (dVar7 >> 0x14);
    orientationAngle2 = orientationAngle0;
  }
  EffectRuntimePool_CreateInstanceFromDefinitionCf
            (EFFECT_RUNTIME_COMPLETION_NONE,(EffectRuntimeOwnerReference4)0x0,orientationAngle0,
             orientationAngle1,orientationAngle2,worldZQ12,worldY,dVar4,effectDefinition,
             worldContext1);
  pdVar1 = &(modelRuntime->classState).classStateE0;
  *pdVar1 = *pdVar1 + 1;
ArmyRuntime_UpdateTimedShotAndEffectEmitters_UpdateDamageThresholdEffectAndReturn:
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  return;
}

