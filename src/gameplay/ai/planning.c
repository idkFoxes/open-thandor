/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/ai/planning.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/ai/planning.h>

/* Implementation ownership: gameplay/ai/planning. */

/* Address: 0x00514350.
   Ownership: gameplay/ai/planning.
   Purpose: Runs the faction-planning dispatcher for active AI factions, derives decayed per-class capacity values,
   accumulates cross-faction model costs from active runtime objects, and rebuilds the verified per-faction
   capacity maxima. Tick-wheel cases 2/6. Contains THE STOCK-INCOME READ: channel = instance+0x5C with a SINGLE
   deref (def copy definitionValueBC_5C, ~0 on disk), crediting +0x100 to faction base+0x720+ch*4.
   Local calls: AiRuntime_DispatchFactionPlanningPhase.
*/
void __fastcall AiFactionRuntime_RebuildPlanningCapacityState(void)

{
  GameSpeedQ8 GVar1;
  int iVar2;
  int iVar3;
  FrontendPlayerRuntimeBlockCount FVar4;
  FrontendPlayerRuntimeRecord *pFVar5;
  uint uVar6;
  uint uVar7;
  GameFactionRuntimeRecord *factionRecordPressureTarget;
  GameFactionRuntimeRecord *factionRecordPlanning;
  GameFactionRuntimeRecord *factionRecordDecay;
  GameFactionRuntimeRecord *factionRecordMaxScan;
  FactionRuntimeLifecycleObservedState *pFVar8;
  WorldOwnerListNode100 *worldNode1;
  
  factionRecordPlanning = g_GameFactionRuntimeImage.records;
  pFVar8 = g_GameFactionRuntimeImage.tail.factionLifecycleStates;
  iVar3 = 7;
  iVar2 = 1;
  do {
    FVar4 = g_FrontendPlayerRuntimeBlockCount;
    pFVar5 = g_FrontendPlayerRuntimeBlocks;
    pFVar8 = pFVar8 + 1;
    factionRecordPlanning = factionRecordPlanning + 1;
    factionRecordPlanning->terrainContributionScaleQ8 = 0x100;
    GVar1 = g_GameFactionRuntimeImage.tail.gameSpeedQ8;
    if (*pFVar8 == FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
      do {
        if (iVar2 == (pFVar5->factionAssignment).factionAssignmentIndex)
        goto AiFactionRuntime_RebuildPlanningCapacityState_AdvanceAfterPlayerOrAiPlanningDispatch;
        pFVar5 = pFVar5 + 1;
        FVar4 = FVar4 - 1;
      } while (FVar4 != 0);
      AiRuntime_DispatchFactionPlanningPhase(iVar2,(WorldRuntimeContext *)g_InGameRuntimeRoot);
      factionRecordPlanning->terrainContributionScaleQ8 = GVar1;
    }
AiFactionRuntime_RebuildPlanningCapacityState_AdvanceAfterPlayerOrAiPlanningDispatch:
    iVar2 = iVar2 + 1;
    iVar3 = iVar3 + -1;
    if (iVar3 == 0) {
      factionRecordDecay = g_GameFactionRuntimeImage.records + 1;
      iVar2 = 7;
      uVar7 = 0;
      do {
        do {
          uVar6 = uVar7 + 2;
          factionRecordDecay->aiPressureValues[uVar7] =
               (factionRecordDecay->aiPressureValues[uVar7] * 3 + 1U >> 2) + 1;
          factionRecordDecay->aiPressureValues[uVar7 + 1] =
               (factionRecordDecay->aiPressureValues[uVar7 + 1] * 3 + 1U >> 2) + 1;
          uVar7 = uVar6;
        } while (uVar6 < 8);
        factionRecordDecay->maximumAiPressure = 0;
        uVar7 = 0;
        factionRecordDecay = factionRecordDecay + 1;
        iVar2 = iVar2 + -1;
      } while (iVar2 != 0);
      worldNode1 = (g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
      if (worldNode1 != (WorldOwnerListNode100 *)0x0) {
        do {
          if (worldNode1->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
            iVar2 = *(int *)((int)worldNode1->runtimePayload + 8);
            if (*(int *)(iVar2 + 0xc) != 0) {
              iVar3 = *(int *)(*(int *)worldNode1->runtimePayload + 0x5c);
              uVar6 = 8;
              uVar7 = 1;
              factionRecordPressureTarget = g_GameFactionRuntimeImage.records;
              do {
                factionRecordPressureTarget = factionRecordPressureTarget + 1;
                if (((*(uint *)(iVar2 + 0x50) & uVar6) != 0) && (uVar7 != *(uint *)(iVar2 + 0xc))) {
                  factionRecordPressureTarget->aiPressureValues[iVar3] =
                       factionRecordPressureTarget->aiPressureValues[iVar3] + 0x100;
                }
                uVar6 = uVar6 << 2;
                uVar7 = uVar7 + 1;
              } while (uVar7 < 8);
            }
          }
          worldNode1 = worldNode1->nextNode;
        } while (worldNode1 != (WorldOwnerListNode100 *)0x0);
        factionRecordMaxScan = g_GameFactionRuntimeImage.records + 1;
        iVar2 = 7;
        uVar7 = 0;
        do {
          do {
            iVar3 = factionRecordMaxScan->aiPressureValues[uVar7 + 1];
            if (factionRecordMaxScan->maximumAiPressure <
                factionRecordMaxScan->aiPressureValues[uVar7]) {
              factionRecordMaxScan->maximumAiPressure =
                   factionRecordMaxScan->aiPressureValues[uVar7];
            }
            uVar7 = uVar7 + 2;
            if (factionRecordMaxScan->maximumAiPressure < iVar3) {
              factionRecordMaxScan->maximumAiPressure = iVar3;
            }
          } while (uVar7 < 8);
          uVar7 = 0;
          factionRecordMaxScan = factionRecordMaxScan + 1;
          iVar2 = iVar2 + -1;
        } while (iVar2 != 0);
      }
      return;
    }
  } while( true );
}


/* Address: 0x0053BA50.
   Ownership: gameplay/ai/planning.
   Purpose: Finds the model definition for a runtime class, scans eligible army assets for matching linked
   definition IDs and faction technology availability, and returns the average positive faction-weighted army
   score. Stock tech.tec has 512 records over canonical ids 0..255; localized titles do not prove source-building,
   tier, direction, or effect mappings.
   Local calls: AiArmyCandidate_ComputeFactionWeightedScore.
   Cross-module calls: ModelDefinitionRegistry_FindByRuntimeClassId [assets/model/definitions],
   ModelDefinition_IsFactionTechnologyUnlockedCf [assets/model/definitions].
*/
AiCandidateScore32
AiArmyCandidate_ComputeAverageCompatibleAssetScore
          (AiArmyScoreWeights *scoreWeights,FactionRuntimeIndex factionIndex,
          ModelRuntimeClassId runtimeClassId)

{
  ArmyAssetRuntimeSemanticView80 *armyAssetRecord;
  dword dVar1;
  ModelDefinitionRecordPrefix *modelDefinition;
  AiCandidateScore32 AVar2;
  int registryEntriesRemaining;
  ArmyAssetRecordPrefix **armyAssetRegistryCursor;
  bool bVar3;
  int compatibleAssetCount;
  uint compatibleAssetScoreSum;
  AiLinkedDefinitionListView *assetLinkedDefinitions;
  AiLinkedDefinitionListView *nestedLinkedDefinitions;
  PckModelDefinitionIdCatalog candidateModelDefinitionId;
  AiLinkedDefinitionListView *secondNestedLinkedDefinitions;
  
  modelDefinition = ModelDefinitionRegistry_FindByRuntimeClassId(runtimeClassId);
  AVar2 = 0;
  if (modelDefinition != (ModelDefinitionRecordPrefix *)0x0) {
    candidateModelDefinitionId = modelDefinition->definitionId;
    armyAssetRegistryCursor = g_ArmyAssetRecordRegistry;
    registryEntriesRemaining = 0x300;
    compatibleAssetScoreSum = 0;
    compatibleAssetCount = 0;
    do {
      armyAssetRecord = (ArmyAssetRuntimeSemanticView80 *)*armyAssetRegistryCursor;
      if ((armyAssetRecord != (ArmyAssetRuntimeSemanticView80 *)0x0) &&
         ((armyAssetRecord->flags14 & 1) != 0)) {
        dVar1 = armyAssetRecord->rootNodeOffsetOrPointer;
        if ((((candidateModelDefinitionId != *(PckModelDefinitionIdCatalog *)(dVar1 + 0x20)) &&
             (((candidateModelDefinitionId != *(PckModelDefinitionIdCatalog *)(dVar1 + 0x24) &&
               (candidateModelDefinitionId != *(PckModelDefinitionIdCatalog *)(dVar1 + 0x28))) &&
              (candidateModelDefinitionId != *(PckModelDefinitionIdCatalog *)(dVar1 + 0x2c))))) &&
            (((candidateModelDefinitionId != *(PckModelDefinitionIdCatalog *)(dVar1 + 0x30) &&
              (candidateModelDefinitionId != *(PckModelDefinitionIdCatalog *)(dVar1 + 0x34))) &&
             (candidateModelDefinitionId != *(PckModelDefinitionIdCatalog *)(dVar1 + 0x38))))) &&
           (candidateModelDefinitionId != *(PckModelDefinitionIdCatalog *)(dVar1 + 0x3c))) {
          bVar3 = ModelDefinition_IsFactionTechnologyUnlockedCf
                            (g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits,
                             *(PckModelDefinitionIdCatalog *)(dVar1 + 0x20));
          if (((bVar3) ||
              (nestedLinkedDefinitions = *(AiLinkedDefinitionListView **)(dVar1 + 0xc),
              *(int *)(dVar1 + 8) == 0)) ||
             ((((candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[0] &&
                ((((candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[1] &&
                   (candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[2])) &&
                  (candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[3])) &&
                 ((candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[4] &&
                  (candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[5])))))) &&
               ((candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[6] &&
                (candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[7])))) &&
              ((secondNestedLinkedDefinitions = *(AiLinkedDefinitionListView **)(dVar1 + 0x10),
               *(uint *)(dVar1 + 8) < 2 ||
               (((candidateModelDefinitionId != secondNestedLinkedDefinitions->definitionIds[0] &&
                 (candidateModelDefinitionId != secondNestedLinkedDefinitions->definitionIds[1])) &&
                (((candidateModelDefinitionId != secondNestedLinkedDefinitions->definitionIds[2] &&
                  (((candidateModelDefinitionId != secondNestedLinkedDefinitions->definitionIds[3]
                    && (candidateModelDefinitionId !=
                        secondNestedLinkedDefinitions->definitionIds[4])) &&
                   (candidateModelDefinitionId != secondNestedLinkedDefinitions->definitionIds[5])))
                  ) && ((candidateModelDefinitionId !=
                         secondNestedLinkedDefinitions->definitionIds[6] &&
                        (candidateModelDefinitionId !=
                         secondNestedLinkedDefinitions->definitionIds[7]))))))))))))
          goto AiArmyCandidate_ComputeAverageCompatibleAssetScore_AdvanceRegistryScan;
        }
        AVar2 = AiArmyCandidate_ComputeFactionWeightedScore
                          (scoreWeights,factionIndex,armyAssetRecord);
        if (0 < AVar2) {
          compatibleAssetScoreSum = compatibleAssetScoreSum + AVar2;
          compatibleAssetCount = compatibleAssetCount + 1;
        }
      }
AiArmyCandidate_ComputeAverageCompatibleAssetScore_AdvanceRegistryScan:
      armyAssetRegistryCursor = armyAssetRegistryCursor + 1;
      registryEntriesRemaining = registryEntriesRemaining + -1;
    } while (registryEntriesRemaining != 0);
    AVar2 = 0;
    if (compatibleAssetCount != 0) {
      AVar2 = (AiCandidateScore32)
              ((longlong)(ulonglong)compatibleAssetScoreSum / (longlong)compatibleAssetCount);
    }
  }
  return AVar2;
}


/* Address: 0x005379E0.
   Ownership: gameplay/ai/planning.
   Purpose: Scans the secondary AI entity workspace and collects up to four unique combined grid-mask classes
   derived from each active entity definition. When no class is found it installs the verified default mask
   0x90000100.
*/
void __thandor_void_preserve_eax_ecx_edx AiPlanning_CollectActiveGridMaskClasses(void)

{
  ModelRuntimeSlot *pMVar1;
  int iVar2;
  dword dVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  int iVar8;
  AiRuntimeWorkspaceEntry *runtimeWorkspaceEntry;
  
  g_AiActiveGridMaskClass0 = 0xffffffff;
  g_AiActiveGridMaskClass1 = 0xffffffff;
  g_AiActiveGridMaskClass2 = 0xffffffff;
  g_AiActiveGridMaskClass3 = 0xffffffff;
  uVar4 = g_AiActiveGridMaskClass0;
  uVar5 = g_AiActiveGridMaskClass1;
  uVar6 = g_AiActiveGridMaskClass2;
  runtimeWorkspaceEntry = g_AiWorkspaceBuffer01_Size0200;
  for (iVar8 = g_AiWorkspace01Count; g_AiActiveGridMaskClass0 = uVar4, iVar8 != 0;
      iVar8 = iVar8 + -1) {
    g_AiActiveGridMaskClass1 = uVar5;
    g_AiActiveGridMaskClass2 = uVar6;
    if (runtimeWorkspaceEntry->armyRuntime != (ArmyRuntimeSlot *)0x0) {
      pMVar1 = (runtimeWorkspaceEntry->armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
      iVar2 = pMVar1[1].classLinkState.modelLinkOrState60.signedScalarState;
      if ((*(int *)(pMVar1->reserved10_37 + 8) != 0) && (-1 < iVar2)) {
        dVar3 = pMVar1[1].classLinkState.classState64;
        if ((((-1 < (int)dVar3) &&
             ((((uVar7 = 0x100 << ((byte)iVar2 & 0x1f) | 0x80000000U |
                         0x1000000 << ((byte)dVar3 & 0x1f), uVar7 != uVar4 && (uVar7 != uVar5)) &&
               (uVar7 != uVar6)) &&
              ((uVar7 != g_AiActiveGridMaskClass3 &&
               (g_AiActiveGridMaskClass0 = uVar7, uVar4 != 0xffffffff)))))) &&
            (g_AiActiveGridMaskClass0 = uVar4, g_AiActiveGridMaskClass1 = uVar7, uVar5 != 0xffffffff
            )) && ((g_AiActiveGridMaskClass1 = uVar5, g_AiActiveGridMaskClass2 = uVar7,
                   uVar6 != 0xffffffff &&
                   (g_AiActiveGridMaskClass2 = uVar6, g_AiActiveGridMaskClass3 == 0xffffffff)))) {
          g_AiActiveGridMaskClass3 = uVar7;
        }
      }
    }
    runtimeWorkspaceEntry = runtimeWorkspaceEntry + 1;
    uVar4 = g_AiActiveGridMaskClass0;
    uVar5 = g_AiActiveGridMaskClass1;
    uVar6 = g_AiActiveGridMaskClass2;
  }
  g_AiActiveGridMaskClass1 = uVar5;
  g_AiActiveGridMaskClass2 = uVar6;
  if (uVar4 == 0xffffffff) {
    g_AiActiveGridMaskClass0 = 0x90000100;
  }
  return;
}


/* Address: 0x0053C810.
   Ownership: gameplay/ai/planning.
   Purpose: Runs the active faction planning phase selected by the runtime phase bit, rebuilding workspaces before
   dispatching either candidate generation and construction processing or unit-behavior and group-assignment
   updates. Table 0053C870: 0053C980, 0053C880. Rally knobs read live from ki.dat (+0xCC/+0xDC, A4). Typed
   parameters: p2 planningPhase→AiPlanningPhaseIndex_V343, p3 inGameRuntime→WorldRuntimeContext *. Calling
   convention, complete VariableStorage serialization, function bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Local calls: AiFactionPlanning_UpdateActiveEntityPressureFlag,
   AiConstructionPlanner_ProcessPendingAssetRequests, AiResourceCandidate_AddWeightedId136,
   AiStructureCandidate_AddWeightedId14BOr14CCandidate, AiArmyCandidate_AddBestScoredVariantA,
   AiArmyCandidate_AddBestScoredVariantB, AiArmyCandidate_AddBestScoredVariantC,
   AiStrategicClass_AddCandidate12DOr12FTo132.
   Cross-module calls: AiPlanning_RebuildFactionWorkspaces [gameplay/ai/workspaces],
   AiUnitBehavior_UpdateWorkspace01Entities [gameplay/ai/units], AiUnitGroup_AssignCollectedEntitiesToBestTarget
   [gameplay/ai/combat], GameFactionRelations_UpdateAllPairsForFaction [gameplay/faction/relations],
   AiCandidateWorkspace_Clear [gameplay/ai/workspaces], AiWorkspaceAssetCandidate_AddWeightedEntry
   [gameplay/ai/workspaces].
*/
void __thandor_void_preserve_eax_ecx_edx
AiRuntime_DispatchFactionPlanningPhase
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *inGameRuntime)

{
  uint planningPhaseDispatchIndex;
  FactionRuntimeIndex purchaseFactionIndexAfterCacheLoad;
  FactionRuntimeIndex factionIndex_00;
  FactionRuntimeIndex factionIndex_01;
  WorldObjectRecordCount *worldRuntime;
  bool bVar1;
  AiKnowledgeDataImage *knowledgeData;
  
  if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
       SESSION_NETWORK_ROLE_LOCAL) || ((g_UiCommandRuntimeFlags & 2) == 0)) {
    planningPhaseDispatchIndex = g_GameFactionRuntimeImage.tail.simulationTick >> 6 & 1;
    worldRuntime = &inGameRuntime[7].objectCount;
    if ((g_GameFactionRuntimeImage.tail.simulationTick >> 3 & 7) == factionIndex) {
      AiPlanning_RebuildFactionWorkspaces
                (planningPhaseDispatchIndex,factionIndex,factionIndex,
                 (WorldRuntimeContext *)worldRuntime);
                    // WARNING: Switch is manually overridden
      switch(planningPhaseDispatchIndex) {
      case 0:
        AiUnitBehavior_UpdateWorkspace01Entities(factionIndex,(WorldRuntimeContext *)worldRuntime);
        AiUnitGroup_AssignCollectedEntitiesToBestTarget();
        break;
      case 1:
        AiFactionPlanning_UpdateActiveEntityPressureFlag(factionIndex);
        GameFactionRelations_UpdateAllPairsForFaction
                  (factionIndex,(WorldRuntimeContext *)worldRuntime);
        bVar1 = AiConstructionPlanner_ProcessPendingAssetRequests
                          (factionIndex,(WorldRuntimeContext *)worldRuntime);
        knowledgeData = g_AiKnowledgeData;
        if (!bVar1) {
          if (((g_GameFactionRuntimeImage.records[factionIndex].candidateCache.cacheReuseState == 0)
              || (0x31 < (int)g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorCooldown)
              ) || (0x31 < (int)g_GameFactionRuntimeImage.records[factionIndex].anchorCooldown0)) {
            AiCandidateWorkspace_Clear();
            AiResourceCandidate_AddWeightedId136(factionIndex);
            AiWorkspaceAssetCandidate_AddWeightedEntry
                      ((knowledgeData->parameters).specialSite14aBaseWeight,
                       ARM_0330_BUILDING_MDL0303,factionIndex,(WorldRuntimeContext *)worldRuntime);
            AiWorkspaceAssetCandidate_AddWeightedEntry
                      ((knowledgeData->parameters).specialSite14cBaseWeight,
                       ARM_0332_BUILDING_MDL0302,factionIndex,(WorldRuntimeContext *)worldRuntime);
            AiStructureCandidate_AddWeightedId14BOr14CCandidate
                      ((knowledgeData->parameters).structure14bBaseWeight,ARM_0331_BUILDING_MDL0308,
                       factionIndex,(WorldRuntimeContext *)worldRuntime);
            AiStructureCandidate_AddWeightedId14BOr14CCandidate
                      ((knowledgeData->parameters).structure14dBaseWeight,ARM_0333_BUILDING_MDL0307,
                       factionIndex,(WorldRuntimeContext *)worldRuntime);
            AiCandidatePlanning_AddSpecialSiteCandidate
                      (factionIndex,(WorldRuntimeContext *)worldRuntime);
            AiArmyCandidate_AddBestScoredVariantA(factionIndex,(WorldRuntimeContext *)worldRuntime);
            AiArmyCandidate_AddBestScoredVariantB(factionIndex,(WorldRuntimeContext *)worldRuntime);
            AiArmyCandidate_AddBestScoredVariantC(factionIndex,(WorldRuntimeContext *)worldRuntime);
            AiStrategicClass_AddCandidate12DOr12FTo132
                      (factionIndex,(WorldRuntimeContext *)worldRuntime);
            AiStrategicClass_AddWeightedClassCandidate
                      (factionIndex,(WorldRuntimeContext *)worldRuntime);
            AiStrategicCandidate_AddBestWorkspace12Entry
                      (factionIndex,(WorldRuntimeContext *)worldRuntime);
            bVar1 = AiPurchasePlanner_ExecuteAffordableCandidates(factionIndex);
            if (bVar1) {
              AiCandidateWorkspace_SaveToFactionImage(factionIndex * 0x740);
              g_GameFactionRuntimeImage.records[factionIndex].candidateCache.cacheReuseState = 0x10;
            }
          }
          else {
            AiCandidateWorkspace_LoadFromFactionImage(factionIndex * 0x740);
            bVar1 = AiPurchasePlanner_ExecuteAffordableCandidates(factionIndex);
            if (!bVar1) {
              g_GameFactionRuntimeImage.records[factionIndex].candidateCache.cacheReuseState = 0;
            }
          }
        }
      }
    }
  }
  return;
}


/* Address: 0x00539070.
   Ownership: gameplay/ai/planning.
   Purpose: Processes the pending AI asset-request workspace in order, dispatching special IDs and army assets to
   their placement handlers. Processing stops after one handler commits an asset and increments the shared
   completion counter. Stock ARM contains 675 records and 326 unique ids; placement workspace, producer, tier,
   class, and faction-role semantics are not inferred from numeric adjacency.
   Local calls: AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate,
   AiConstructionPlanner_PlaceDerivedAsset14D, AiConstructionPlanner_PlaceExtendedAssetNearFactionAnchor.
   Cross-module calls: AiPrimaryWorkspace_HasUnassignedEntryByIdCf [gameplay/ai/workspaces],
   AiConstructionPlanner_PlaceSpecialAssetFromWorkspace [gameplay/ai/workspaces], ArmyAssetRegistry_FindByIdCf
   [assets/army/catalog], ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf [assets/model/definitions].
*/
bool __thandor_cf_preserve_eax_ecx_edx
AiConstructionPlanner_ProcessPendingAssetRequests
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  PckArmyAssetIdCatalog armyAssetId;
  int iVar1;
  AiRuntimeWorkspaceEntry *pAVar2;
  bool bVar3;
  ArmyRegistryEaxCf5_51b6d0 AVar4;
  ModelDefinitionLookupEaxCf5 MVar5;
  
  g_AiConstructionPendingAssetConsumedCount = 0;
  iVar1 = g_AiWorkspace04Count;
  pAVar2 = g_AiWorkspaceBuffer04_Size0040;
  do {
    if (iVar1 == 0) {
      return false;
    }
    armyAssetId = pAVar2->armyAssetId;
    if (armyAssetId == ARM_0300_BUILDING_MDL0301) {
      bVar3 = AiPrimaryWorkspace_HasUnassignedEntryByIdCf(ARM_0330_BUILDING_MDL0303);
      if (!bVar3) {
AiConstructionPlanner_ProcessPendingAssetRequests_DispatchReachableCandidatePlacement:
        AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate
                  (armyAssetId,factionIndex,worldRuntime);
      }
    }
    else if ((armyAssetId == ARM_0330_BUILDING_MDL0303) ||
            (armyAssetId == ARM_0332_BUILDING_MDL0302)) {
      AiConstructionPlanner_PlaceSpecialAssetFromWorkspace(armyAssetId,factionIndex,worldRuntime);
    }
    else {
      if (armyAssetId == ARM_0331_BUILDING_MDL0308)
      goto AiConstructionPlanner_ProcessPendingAssetRequests_DispatchReachableCandidatePlacement;
      if (armyAssetId == ARM_0333_BUILDING_MDL0307) {
        AiConstructionPlanner_PlaceDerivedAsset14D
                  (ARM_0333_BUILDING_MDL0307,factionIndex,worldRuntime);
      }
      else if (armyAssetId < ARM_0340_BUILDING_MDL0314) {
        AVar4 = ArmyAssetRegistry_FindByIdCf(armyAssetId);
        if (!AVar4.carry) {
          MVar5 = ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                            (factionIndex,(AVar4.eax)->rootNodeOffsetOrPointer);
          if (MVar5.modelDefinition[0x34].definitionId != 1)
          goto AiConstructionPlanner_ProcessPendingAssetRequests_DispatchReachableCandidatePlacement
          ;
          AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate
                    (armyAssetId,factionIndex,worldRuntime);
        }
      }
      else {
        AiConstructionPlanner_PlaceExtendedAssetNearFactionAnchor
                  (armyAssetId,factionIndex,worldRuntime);
      }
    }
    if (g_AiConstructionPendingAssetConsumedCount != 0) {
      return true;
    }
    pAVar2 = pAVar2 + 1;
    iVar1 = iVar1 + -1;
  } while( true );
}


/* Address: 0x005378C0.
   Ownership: gameplay/ai/planning.
   Purpose: Sorts the purchase-candidate workspace, subtracts each candidate cost from the faction budget, verifies
   an eligible producer, and applies the first affordable supported candidate. It returns a carry-derived status
   through the original convention. Stock tech.tec has 512 records over canonical ids 0..255; localized titles do
   not prove source-building, tier, direction, or effect mappings.
   Local calls: AiPurchaseCandidate_HasEligibleProducerCf, AiPurchaseCandidate_ApplyToFaction.
   Cross-module calls: AiCandidateWorkspace_SortDescending [gameplay/ai/workspaces],
   AiCandidateWorkspace_GetEntryEntityValue [gameplay/ai/workspaces].
*/
bool __thandor_cf_preserve_eax_ecx_edx
AiPurchasePlanner_ExecuteAffordableCandidates(FactionRuntimeIndex factionIndex)

{
  bool bVar1;
  uint uVar2;
  int iVar3;
  XeniteAmountQ4 XVar4;
  AiCandidateWorkspaceEntry *entry;
  bool bVar5;
  
  iVar3 = g_AiCandidateWorkspaceEntryCount;
  entry = g_AiWorkspaceBuffer13_Size0400;
  g_AiPurchaseAppliedArmyClassMask = 0;
  bVar1 = false;
  if (g_AiCandidateWorkspaceEntryCount != 0) {
    bVar1 = true;
    AiCandidateWorkspace_SortDescending();
    XVar4 = g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4;
    do {
      uVar2 = AiCandidateWorkspace_GetEntryEntityValue(entry);
      bVar5 = XVar4 < uVar2;
      XVar4 = XVar4 - uVar2;
      if (bVar5) {
        return bVar1;
      }
      bVar5 = AiPurchaseCandidate_HasEligibleProducerCf(entry,factionIndex);
      if (!bVar5) {
        AiPurchaseCandidate_ApplyToFaction(entry,factionIndex);
        bVar1 = false;
      }
      entry = entry + 1;
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
  }
  return bVar1;
}


/* Address: 0x005393F0.
   Ownership: gameplay/ai/planning.
   Purpose: Attempts to derive and place asset ID 0x14D from existing 0x14A or 0x14C workspace records. It searches
   a valid anchor, creates the runtime object, initializes model and auxiliary state, and falls back to generic
   army placement when neither source class succeeds. Stock ARM contains 675 records and 326 unique ids; placement
   workspace, producer, tier, class, and faction-role semantics are not inferred from numeric adjacency.
   Local calls: AiConstructionPlanner_ConsumeFactionPendingArmyAsset,
   AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate.
   Cross-module calls: AiPlacement_ReserveAdditionalSpecialSite [gameplay/ai/placement],
   AiPlacement_FindNearestValidWorkspace09Anchor [gameplay/ai/placement], ArmyRuntime_CreateInstanceFromAssetCf
   [gameplay/army/runtime], ModelNodeRuntime_RebuildTransformsFromRoot [world/model/hierarchy],
   ArmyRuntime_DispatchClassCommand [gameplay/army/runtime], EffectRuntimePool_CreateInstanceFromDefinitionCf
   [world/effects/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
AiConstructionPlanner_PlaceDerivedAsset14D
          (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime)

{
  FieldGridCell *pFVar1;
  ArmyRuntimeSlot *pAVar2;
  ArmyRuntimeSlot *pAVar3;
  ModelRuntimeSlot *pMVar4;
  ArmyRuntimeSlot **armyRuntime;
  AiTerrainFeatureWorkspaceEntry *terrainFeatureEntry4;
  AiTerrainFeatureWorkspaceEntry *terrainFeatureEntry5;
  int iVar5;
  int iVar6;
  bool bVar7;
  ArmyRuntimeCreateEaxCf5 AVar8;
  AiWorkspace09AnchorEcxEdxCf9 AVar9;
  
  terrainFeatureEntry4 = g_AiWorkspaceBuffer08_Size0200;
  iVar5 = g_AiWorkspace08Count;
  if (g_AiWorkspace08Count != 0) {
    do {
      if (terrainFeatureEntry4->armyAssetId == ARM_0330_BUILDING_MDL0303) {
        pFVar1 = terrainFeatureEntry4->cell;
        bVar7 = AiPlacement_ReserveAdditionalSpecialSite
                          (terrainFeatureEntry4->armyAssetId,pFVar1,factionIndex,worldRuntime);
        if (!bVar7) {
          AVar9 = AiPlacement_FindNearestValidWorkspace09Anchor
                            (pFVar1->worldY,pFVar1->worldX,armyAssetId,factionIndex,worldRuntime);
          if (!AVar9.carry) {
            AVar8 = ArmyRuntime_CreateInstanceFromAssetCf
                              (4,0,AVar9.worldYQ12,AVar9.worldXQ12,factionIndex,armyAssetId,
                               worldRuntime);
            armyRuntime = (ArmyRuntimeSlot **)AVar8.eax;
            if (AVar8.carry) {
              return;
            }
            pAVar2 = armyRuntime[1];
            pAVar3 = *armyRuntime;
            pAVar2->movementPosition0Q12 = 0;
            pMVar4 = (pAVar3->modelRuntimeOrSavedOffset).modelRuntime;
            ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)pAVar2);
            ArmyRuntime_DispatchClassCommand(armyRuntime,worldRuntime);
            EffectRuntimePool_CreateInstanceFromDefinitionCf
                      (EFFECT_RUNTIME_COMPLETION_NONE,(EffectRuntimeOwnerReference4)0x0,
                       (pAVar2->movementControl).turnVelocityAngle16,
                       (pAVar2->movementControl).movementAdvancePerTickQ12,
                       ((WorldRuntimeNodeModelPayload *)&pAVar2->factionIndex)->worldRotationAngle0,
                       pAVar2->depthBinClass,pAVar2->runtimeState98,
                       ((GraphicsFixedVec3 *)&pAVar2->runtimeState94)->x,
                       (EffectDefinition *)pMVar4->attachments140[2].childLocalRotationAngle0,
                       worldRuntime);
            AiConstructionPlanner_ConsumeFactionPendingArmyAsset(armyAssetId,factionIndex);
            return;
          }
        }
      }
      terrainFeatureEntry4 = terrainFeatureEntry4 + 1;
      iVar5 = iVar5 + -1;
      terrainFeatureEntry5 = g_AiWorkspaceBuffer08_Size0200;
      iVar6 = g_AiWorkspace08Count;
    } while (iVar5 != 0);
    do {
      if (terrainFeatureEntry5->armyAssetId == ARM_0332_BUILDING_MDL0302) {
        pFVar1 = terrainFeatureEntry5->cell;
        bVar7 = AiPlacement_ReserveAdditionalSpecialSite
                          (terrainFeatureEntry5->armyAssetId,pFVar1,factionIndex,worldRuntime);
        if (!bVar7) {
          AVar9 = AiPlacement_FindNearestValidWorkspace09Anchor
                            (pFVar1->worldY,pFVar1->worldX,armyAssetId,factionIndex,worldRuntime);
          if (!AVar9.carry) {
            AVar8 = ArmyRuntime_CreateInstanceFromAssetCf
                              (4,0,AVar9.worldYQ12,AVar9.worldXQ12,factionIndex,armyAssetId,
                               worldRuntime);
            armyRuntime = (ArmyRuntimeSlot **)AVar8.eax;
            if (AVar8.carry) {
              return;
            }
            pAVar2 = armyRuntime[1];
            pAVar3 = *armyRuntime;
            pAVar2->movementPosition0Q12 = 0;
            pMVar4 = (pAVar3->modelRuntimeOrSavedOffset).modelRuntime;
            ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)pAVar2);
            ArmyRuntime_DispatchClassCommand(armyRuntime,worldRuntime);
            EffectRuntimePool_CreateInstanceFromDefinitionCf
                      (EFFECT_RUNTIME_COMPLETION_NONE,(EffectRuntimeOwnerReference4)0x0,
                       (pAVar2->movementControl).turnVelocityAngle16,
                       (pAVar2->movementControl).movementAdvancePerTickQ12,
                       ((WorldRuntimeNodeModelPayload *)&pAVar2->factionIndex)->worldRotationAngle0,
                       pAVar2->depthBinClass,pAVar2->runtimeState98,
                       ((GraphicsFixedVec3 *)&pAVar2->runtimeState94)->x,
                       (EffectDefinition *)pMVar4->attachments140[2].childLocalRotationAngle0,
                       worldRuntime);
            AiConstructionPlanner_ConsumeFactionPendingArmyAsset(armyAssetId,factionIndex);
            return;
          }
        }
      }
      iVar6 = iVar6 + -1;
      terrainFeatureEntry5 = terrainFeatureEntry5 + 1;
    } while (iVar6 != 0);
  }
  AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate(armyAssetId,factionIndex,worldRuntime);
  return;
}


/* Address: 0x0053A6E0.
   Ownership: gameplay/ai/planning.
   Purpose: Scores eligible workspace11 army candidates with coefficient profile A, selects the highest positive
   result, adjusts its planning weight for existing pending extended assets and faction state, and adds the
   selected registry ID to the candidate workspace.
   Local calls: AiFactionRuntime_TestPlanningCapacityExceededCf, AiArmyCandidate_ComputeFactionWeightedScore.
   Cross-module calls: Technology_IsUnlockedForFactionCf [gameplay/technology/runtime],
   AiCandidateWorkspace_AddOrAccumulateWeightedEntry [gameplay/ai/workspaces].
*/
void __thandor_void_preserve_eax_ecx_edx
AiArmyCandidate_AddBestScoredVariantA
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  AiCandidateScore32 AVar1;
  dword weightRange;
  int iVar2;
  uint uVar3;
  int iVar4;
  ArmyAssetRecordPrefix *bestArmyAsset;
  AiRuntimeWorkspaceEntry *runtimeWorkspaceEntry;
  ArmyAssetRecordPrefix **armyAssetRegistryCursor;
  bool bVar5;
  
  iVar2 = g_AiWorkspace11Count;
  armyAssetRegistryCursor = g_AiWorkspaceBuffer11_Size1000;
  if (g_AiWorkspace11Count != 0) {
    iVar4 = 0;
    bVar5 = AiFactionRuntime_TestPlanningCapacityExceededCf(4,factionIndex);
    if ((!bVar5) &&
       (bVar5 = Technology_IsUnlockedForFactionCf(TEC_001_ARMS_FACTORIES,factionIndex), !bVar5)) {
      do {
        if (((((ArmyAssetRuntimeSemanticView80 *)*armyAssetRegistryCursor)->flags14 & 1) != 0) &&
           (AVar1 = AiArmyCandidate_ComputeFactionWeightedScore
                              (&g_AiArmyCandidateScoreWeightsVariantA15,factionIndex,
                               (ArmyAssetRuntimeSemanticView80 *)*armyAssetRegistryCursor),
           iVar4 < AVar1)) {
          bestArmyAsset = *armyAssetRegistryCursor;
          iVar4 = AVar1;
        }
        armyAssetRegistryCursor = armyAssetRegistryCursor + 1;
        iVar2 = iVar2 + -1;
      } while (iVar2 != 0);
      if (0 < iVar4) {
        uVar3 = 1;
        runtimeWorkspaceEntry = g_AiWorkspaceBuffer04_Size0040;
        for (iVar2 = g_AiWorkspace04Count; iVar2 != 0; iVar2 = iVar2 + -1) {
          if (((ARM_0320_BUILDING_MDL0311|ARM_0019_UNIT_MDL0101) <
               runtimeWorkspaceEntry->armyAssetId) &&
             (runtimeWorkspaceEntry->armyAssetId < ARM_0380_BUILDING_MDL0329)) {
            uVar3 = uVar3 + 1;
          }
          runtimeWorkspaceEntry = runtimeWorkspaceEntry + 1;
        }
        uVar3 = (g_AiKnowledgeData->parameters).armyVariantABaseWeight / uVar3;
        if (g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorCooldown == 0) {
          weightRange = uVar3 * 3 >> 2;
        }
        else {
          weightRange = uVar3 * 2;
        }
        if ((299 < bestArmyAsset->registryId) || (g_AiWorkspace01Count < 0xb)) {
          AiCandidateWorkspace_AddOrAccumulateWeightedEntry(bestArmyAsset->registryId,weightRange,1)
          ;
        }
      }
    }
  }
  return;
}


/* Address: 0x0053AC20.
   Ownership: gameplay/ai/planning.
   Purpose: Adds strategic candidate 0x12D when its prerequisite state is appropriate, otherwise selects an
   available class in the 0x12F through 0x132 family and adds it with a knowledge-derived weight reduced by the
   existing class count.
   Local calls: AiFactionRuntime_TestPlanningCapacityExceededCf, AiStrategicClass_SelectBestCandidate12FTo132.
   Cross-module calls: AiPrimaryWorkspace_HasEntryByIdCf [gameplay/ai/workspaces],
   Technology_IsUnlockedForFactionCf [gameplay/technology/runtime],
   AiCandidateWorkspace_AddOrAccumulateWeightedEntry [gameplay/ai/workspaces].
*/
void __thandor_void_preserve_eax_ecx_edx
AiStrategicClass_AddCandidate12DOr12FTo132
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  uint weightRange;
  bool bVar1;
  AiStrategicClassSelectionRegs8 AVar2;
  AiKnowledgeDataImage *knowledgeData;
  
  knowledgeData = g_AiKnowledgeData;
  bVar1 = AiPrimaryWorkspace_HasEntryByIdCf(ARM_0330_BUILDING_MDL0303);
  if (bVar1) {
    bVar1 = Technology_IsUnlockedForFactionCf(TEC_001_ARMS_FACTORIES,factionIndex);
    if (bVar1) {
      bVar1 = AiPrimaryWorkspace_HasEntryByIdCf(ARM_0301_BUILDING_MDL0318);
      if (!bVar1) {
        bVar1 = AiFactionRuntime_TestPlanningCapacityExceededCf
                          ((knowledgeData->parameters).
                           strategic12dAnd141To143AdditionalPlanningCapacity,factionIndex);
        if (!bVar1) {
          AiCandidateWorkspace_AddOrAccumulateWeightedEntry
                    (0x12d,(knowledgeData->parameters).strategicClass12dBaseWeight,1);
        }
      }
    }
    else {
      bVar1 = AiFactionRuntime_TestPlanningCapacityExceededCf
                        ((knowledgeData->parameters).strategic12fTo132AdditionalPlanningCapacity,
                         factionIndex);
      if (!bVar1) {
        AVar2 = AiStrategicClass_SelectBestCandidate12FTo132(factionIndex,worldRuntime);
        if (AVar2.selectedRuntimeToken != 0) {
          weightRange = (knowledgeData->parameters).strategicClass12fTo132BaseWeight;
          if (AVar2.existingCountOrPressure != 0) {
            weightRange = weightRange / (AVar2.existingCountOrPressure * 2);
          }
          AiCandidateWorkspace_AddOrAccumulateWeightedEntry
                    (AVar2.selectedRuntimeToken,weightRange,1);
        }
      }
    }
  }
  return;
}


/* Address: 0x0053B070.
   Ownership: gameplay/ai/planning.
   Purpose: When the primary prerequisite workspace is absent, selects a class in the 0x141 through 0x143 family
   and adds it to the candidate workspace with a knowledge weight divided by the existing class pressure. It is
   distinct from FrontendPlayerIndex_V306, PlayerRuntimeId, active-faction masks or codes, and PCK-backed
   ArmyAssetId, ModelDefinitionId, and TechnologyId domains.
   Local calls: AiFactionRuntime_TestPlanningCapacityExceededCf, AiStrategicClass_SelectWeightedClass141To143.
   Cross-module calls: AiPrimaryWorkspace_HasEntryByIdCf [gameplay/ai/workspaces],
   AiCandidateWorkspace_AddOrAccumulateWeightedEntry [gameplay/ai/workspaces].
*/
void __thandor_void_preserve_eax_ecx_edx
AiStrategicClass_AddWeightedClassCandidate
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  AiKnowledgeDataImage *pAVar1;
  uint weightRange;
  bool bVar2;
  AiStrategicClassSelectionRegs8 AVar3;
  
  pAVar1 = g_AiKnowledgeData;
  bVar2 = AiPrimaryWorkspace_HasEntryByIdCf(ARM_0330_BUILDING_MDL0303);
  if (bVar2) {
    bVar2 = AiFactionRuntime_TestPlanningCapacityExceededCf
                      ((pAVar1->parameters).strategic12dAnd141To143AdditionalPlanningCapacity,
                       factionIndex);
    if (!bVar2) {
      AVar3 = AiStrategicClass_SelectWeightedClass141To143(factionIndex,worldRuntime);
      if (AVar3.selectedRuntimeToken != 0) {
        weightRange = (pAVar1->parameters).strategicClass141To143BaseWeight;
        if (AVar3.existingCountOrPressure != 0) {
          weightRange = weightRange / (AVar3.existingCountOrPressure * 2);
        }
        AiCandidateWorkspace_AddOrAccumulateWeightedEntry(AVar3.selectedRuntimeToken,weightRange,1);
      }
    }
  }
  return;
}


/* Address: 0x005397C0.
   Ownership: gameplay/ai/planning.
   Purpose: Places an extended asset at a workspace09 point selected by distance from the faction anchor and
   separation from primary and secondary AI workspaces. The chosen runtime object is created, initialized, and
   committed to the faction. Stock ARM contains 675 records and 326 unique ids; placement workspace, producer,
   tier, class, and faction-role semantics are not inferred from numeric adjacency.
   Local calls: AiConstructionPlanner_ConsumeFactionPendingArmyAsset.
   Cross-module calls: AiPrimaryWorkspace_GetMinimumActiveManhattanDistanceToPoint [gameplay/ai/workspaces],
   AiWorkspace02_GetMinimumManhattanDistanceToPoint [gameplay/ai/workspaces],
   ArmyPlacement_DispatchAssetAtFieldPoint [gameplay/army/placement],
   AiWorkspace03_GetMinimumManhattanDistanceToPoint [gameplay/ai/workspaces], ArmyRuntime_CreateInstanceFromAssetCf
   [gameplay/army/runtime], ModelNodeRuntime_RebuildTransformsFromRoot [world/model/hierarchy].
*/
void __thandor_void_preserve_eax_ecx_edx
AiConstructionPlanner_PlaceExtendedAssetNearFactionAnchor
          (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime)

{
  ArmyRuntimeSlot *modelNodeRuntime;
  ArmyRuntimeSlot *pAVar1;
  ModelRuntimeSlot *pMVar2;
  int iVar3;
  int iVar4;
  ArmyRuntimeSlot **armySlot1;
  int iVar5;
  int iVar6;
  FieldGridCell **gridCellCursor;
  ArmyPlacementDispatchEaxCf5 AVar7;
  ArmyRuntimeCreateEaxCf5 AVar8;
  FieldGridCell *local_24;
  int local_20;
  FieldGridCell *gridCell2;
  
  if ((g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorCooldown != 0) &&
     (g_AiWorkspace09Count != 0)) {
    local_20 = 0x7fffffff;
    iVar5 = g_AiWorkspace09Count;
    gridCellCursor = g_AiWorkspaceBuffer09_Size1000;
    do {
      gridCell2 = *gridCellCursor;
      iVar3 = g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorYQ12 - gridCell2->worldX;
      if (iVar3 < 0) {
        iVar3 = -iVar3;
      }
      iVar6 = g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorXQ12 - gridCell2->worldY;
      if (iVar6 < 0) {
        iVar6 = -iVar6;
      }
      if ((iVar6 + iVar3 < local_20) &&
         (iVar4 = AiPrimaryWorkspace_GetMinimumActiveManhattanDistanceToPoint
                            (gridCell2->worldY,gridCell2->worldX), 0x1fff < iVar4)) {
        iVar4 = AiWorkspace02_GetMinimumManhattanDistanceToPoint
                          (gridCell2->worldY,gridCell2->worldX);
        if (iVar4 < 0x7fffffff) {
          if (iVar4 < 0x5001) {
            iVar4 = iVar4 * 4;
joined_r0x00539894:
            iVar4 = iVar6 + iVar3 + iVar4;
            if ((iVar4 < local_20) &&
               (AVar7 = ArmyPlacement_DispatchAssetAtFieldPoint
                                  (1,0,(uint)(ushort)gridCell2->triangle0NormalAngles,
                                   gridCell2->worldY,gridCell2->worldX,armyAssetId,factionIndex,
                                   (UiRootNode *)worldRuntime), !AVar7.carry)) {
              local_24 = gridCell2;
              local_20 = iVar4;
            }
          }
        }
        else {
          iVar4 = AiWorkspace03_GetMinimumManhattanDistanceToPoint
                            (gridCell2->worldY,gridCell2->worldX);
          if (iVar4 < 0x8001) {
            iVar4 = iVar4 * 2;
            goto joined_r0x00539894;
          }
        }
      }
      gridCellCursor = gridCellCursor + 1;
      iVar5 = iVar5 + -1;
    } while (iVar5 != 0);
    if (local_20 < 0x7fffffff) {
      AVar8 = ArmyRuntime_CreateInstanceFromAssetCf
                        (4,(uint)(ushort)local_24->triangle0NormalAngles,local_24->worldY,
                         local_24->worldX,factionIndex,armyAssetId,worldRuntime);
      armySlot1 = (ArmyRuntimeSlot **)AVar8.eax;
      if (!AVar8.carry) {
        modelNodeRuntime = armySlot1[1];
        pAVar1 = *armySlot1;
        modelNodeRuntime->movementPosition0Q12 = 0;
        pMVar2 = (pAVar1->modelRuntimeOrSavedOffset).modelRuntime;
        ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)modelNodeRuntime);
        ArmyRuntime_DispatchClassCommand(armySlot1,worldRuntime);
        EffectRuntimePool_CreateInstanceFromDefinitionCf
                  (EFFECT_RUNTIME_COMPLETION_NONE,(EffectRuntimeOwnerReference4)0x0,
                   (modelNodeRuntime->movementControl).turnVelocityAngle16,
                   (modelNodeRuntime->movementControl).movementAdvancePerTickQ12,
                   ((WorldRuntimeNodeModelPayload *)&modelNodeRuntime->factionIndex)->
                   worldRotationAngle0,modelNodeRuntime->depthBinClass,
                   modelNodeRuntime->runtimeState98,
                   ((GraphicsFixedVec3 *)&modelNodeRuntime->runtimeState94)->x,
                   (EffectDefinition *)pMVar2->attachments140[2].childLocalRotationAngle0,
                   worldRuntime);
        AiConstructionPlanner_ConsumeFactionPendingArmyAsset(armyAssetId,factionIndex);
      }
    }
  }
  return;
}


/* Address: 0x0053A800.
   Ownership: gameplay/ai/planning.
   Purpose: Scores eligible workspace11 army candidates with coefficient profile B, selects the highest positive
   result, derives its faction-scaled weight, and adds the chosen registry ID when workspace capacity permits.
   Local calls: AiArmyCandidate_ComputeFactionWeightedScore.
   Cross-module calls: AiCandidateWorkspace_AddOrAccumulateWeightedEntry [gameplay/ai/workspaces].
*/
void __thandor_void_preserve_eax_ecx_edx
AiArmyCandidate_AddBestScoredVariantB
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  AiCandidateScore32 AVar1;
  dword weightRange;
  int iVar2;
  int iVar3;
  ArmyAssetRecordPrefix *bestArmyAsset;
  ArmyAssetRecordPrefix **armyAssetRegistryCursor;
  
  if (((g_AiWorkspace05Count != 0) && (g_AiWorkspace11Count != 0)) && (g_AiWorkspace01Count < 0xb))
  {
    iVar3 = 0;
    iVar2 = g_AiWorkspace11Count;
    armyAssetRegistryCursor = g_AiWorkspaceBuffer11_Size1000;
    do {
      if ((((ArmyAssetRuntimeSemanticView80 *)*armyAssetRegistryCursor)->flags14 & 1) != 0) {
        AVar1 = AiArmyCandidate_ComputeFactionWeightedScore
                          (&g_AiArmyCandidateScoreWeightsVariantB15,factionIndex,
                           (ArmyAssetRuntimeSemanticView80 *)*armyAssetRegistryCursor);
        if (iVar3 < AVar1) {
          bestArmyAsset = *armyAssetRegistryCursor;
          iVar3 = AVar1;
        }
      }
      armyAssetRegistryCursor = armyAssetRegistryCursor + 1;
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
    if (0 < iVar3) {
      weightRange = (dword)(((longlong)
                             (int)(100 - g_GameFactionRuntimeImage.records[factionIndex].
                                         exploredTerrainPercent) *
                            (longlong)
                            (int)(g_AiKnowledgeData->parameters).
                                 armyVariantBUnexploredTerrainWeightCoefficient) /
                           (longlong)(int)((g_AiWorkspace01Count + 1) * 0x20));
      if (g_AiWorkspace07Count != 0) {
        weightRange = weightRange >> 1;
      }
      AiCandidateWorkspace_AddOrAccumulateWeightedEntry(bestArmyAsset->registryId,weightRange,1);
    }
  }
  return;
}


/* Address: 0x0053A8D0.
   Ownership: gameplay/ai/planning.
   Purpose: Scores eligible workspace11 army candidates with coefficient profile C, selects the highest positive
   result, applies the profile-specific knowledge weight, and adds the selected registry ID to the candidate
   workspace.
   Local calls: AiArmyCandidate_ComputeFactionWeightedScore.
   Cross-module calls: AiCandidateWorkspace_AddOrAccumulateWeightedEntry [gameplay/ai/workspaces].
*/
void __thandor_void_preserve_eax_ecx_edx
AiArmyCandidate_AddBestScoredVariantC
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  AiCandidateScore32 AVar1;
  uint weightRange;
  int iVar2;
  int iVar3;
  ArmyAssetRecordPrefix *bestArmyAsset;
  ArmyAssetRecordPrefix **armyAssetRegistryCursor;
  
  if (((g_AiWorkspace07Count != 0) && (g_AiWorkspace11Count != 0)) && (g_AiWorkspace01Count < 0xb))
  {
    iVar3 = 0;
    iVar2 = g_AiWorkspace11Count;
    armyAssetRegistryCursor = g_AiWorkspaceBuffer11_Size1000;
    do {
      if ((((ArmyAssetRuntimeSemanticView80 *)*armyAssetRegistryCursor)->flags14 & 1) != 0) {
        AVar1 = AiArmyCandidate_ComputeFactionWeightedScore
                          (&g_AiArmyCandidateScoreWeightsVariantC15,factionIndex,
                           (ArmyAssetRuntimeSemanticView80 *)*armyAssetRegistryCursor);
        if (iVar3 < AVar1) {
          bestArmyAsset = *armyAssetRegistryCursor;
          iVar3 = AVar1;
        }
      }
      armyAssetRegistryCursor = armyAssetRegistryCursor + 1;
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
    if (0 < iVar3) {
      weightRange = (g_AiKnowledgeData->parameters).armyVariantCBaseWeight;
      if (g_AiWorkspace07Count == 0) {
        weightRange = weightRange >> 1;
      }
      AiCandidateWorkspace_AddOrAccumulateWeightedEntry(bestArmyAsset->registryId,weightRange,1);
    }
  }
  return;
}


/* Address: 0x00537630.
   Ownership: gameplay/ai/planning.
   Purpose: Checks whether a technology or army candidate has a currently eligible producer or supporting runtime
   entity. The scan covers technology lists, army class masks, production state, and relevant entity categories; CF
   is clear when support is found and set otherwise. Stock tech.tec has 512 records over canonical ids 0..255;
   localized titles do not prove source-building, tier, direction, or effect mappings.
   Cross-module calls: Technology_IsAvailableForFactionCf [gameplay/technology/runtime],
   ArmyAssetRegistry_FindByIdCf [assets/army/catalog].
*/
bool __thandor_cf_preserve_eax_ecx_edx
AiPurchaseCandidate_HasEligibleProducerCf
          (AiCandidateWorkspaceEntry *candidateEntry,FactionRuntimeIndex factionIndex)

{
  uint uVar1;
  int *piVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  RuntimeToken technologyIndex;
  AiWorkspace00EntryView8 *pAVar6;
  bool bVar7;
  ArmyRegistryEaxCf5_51b6d0 AVar8;
  
  technologyIndex = candidateEntry->entityIdAndMultiplicity & 0xffff;
  if ((candidateEntry->weightedScoreAndKind & 0xf) == 2) {
    bVar7 = Technology_IsAvailableForFactionCf(technologyIndex,factionIndex);
    pAVar6 = g_AiWorkspaceBuffer00_Size0400;
    uVar1 = g_AiWorkspace00Count;
    if (bVar7) {
      for (; uVar1 != 0; uVar1 = uVar1 - 1) {
        piVar2 = (int *)pAVar6->runtimeSlotAddressOrZero;
        if ((piVar2 != (int *)0x0) && ((piVar2[0x3b] & 0x89U) == 0)) {
          iVar4 = 0x1c;
          do {
            if (technologyIndex == *(RuntimeToken *)(*piVar2 + 0x1c4 + iVar4 * 4)) {
              return false;
            }
            iVar4 = iVar4 + -1;
          } while (iVar4 != 0);
        }
        pAVar6 = pAVar6 + 1;
      }
    }
  }
  else {
    AVar8 = ArmyAssetRegistry_FindByIdCf(technologyIndex);
    uVar1 = AVar8.eax[1].selectionDetailTemplateVariantIndex;
    if ((g_AiWorkspace00Count != 0) && ((g_AiPurchaseAppliedArmyClassMask & uVar1) == 0)) {
      uVar5 = g_AiWorkspace00Count;
      pAVar6 = g_AiWorkspaceBuffer00_Size0400;
      if ((uVar1 & 0x10) == 0) {
        if ((uVar1 & 8) == 0) {
          uVar3 = uVar1 & 0xee;
          while (uVar3 != 0) {
            piVar2 = (int *)pAVar6->runtimeSlotAddressOrZero;
            if (((piVar2 != (int *)0x0) && (*(int *)(*piVar2 + 0x4c) == 0xd)) &&
               (((piVar2[0x3b] & 0xc9U) == 0 &&
                (((*(uint *)(*piVar2 + 0xc4) & uVar1 & 0xee) != 0 && (piVar2[0x2e] == 0)))))) {
              return false;
            }
            pAVar6 = pAVar6 + 1;
            uVar5 = uVar5 - 1;
            uVar3 = uVar5;
          }
        }
        else {
          do {
            piVar2 = (int *)pAVar6->runtimeSlotAddressOrZero;
            if (((piVar2 != (int *)0x0) && (*(int *)(*piVar2 + 0x4c) == 0x16)) &&
               (((piVar2[0x3b] & 0xc9U) == 0 && (piVar2[0x2b] == 0)))) {
              return false;
            }
            pAVar6 = pAVar6 + 1;
            uVar5 = uVar5 - 1;
          } while (uVar5 != 0);
        }
      }
      else {
        do {
          piVar2 = (int *)pAVar6->runtimeSlotAddressOrZero;
          if ((((piVar2 != (int *)0x0) && (*(int *)(*piVar2 + 0x4c) == 0xb)) &&
              ((piVar2[0x3b] & 0xc9U) == 0)) && (piVar2[0x2e] == 0)) {
            return false;
          }
          pAVar6 = pAVar6 + 1;
          uVar5 = uVar5 - 1;
        } while (uVar5 != 0);
      }
    }
  }
  return true;
}


/* Address: 0x00537800.
   Ownership: gameplay/ai/planning.
   Purpose: Applies one accepted purchase candidate. Technology candidates are applied to the first matching
   eligible entity and removed from its workspace slot; army candidates are registered for the faction and their
   class mask is accumulated.
   Cross-module calls: Technology_ApplyRecordToEntity [gameplay/technology/runtime],
   GameFactionRuntime_RegisterArmyAssetPointers [gameplay/faction/runtime], ArmyAssetRegistry_FindByIdCf
   [assets/army/catalog].
*/
void __thandor_void_preserve_eax_ecx_edx
AiPurchaseCandidate_ApplyToFaction
          (AiCandidateWorkspaceEntry *candidateEntry,FactionRuntimeIndex factionIndex)

{
  GameEntityRuntime *entity;
  int iVar1;
  AiWorkspace00EntryView8 *pAVar2;
  int iVar3;
  RuntimeToken technologyIndex;
  ArmyRegistryEaxCf5_51b6d0 AVar4;
  
  technologyIndex = candidateEntry->entityIdAndMultiplicity & 0xffff;
  iVar1 = g_AiWorkspace00Count;
  pAVar2 = g_AiWorkspaceBuffer00_Size0400;
  if ((candidateEntry->weightedScoreAndKind & 0xf) == 2) {
    for (; iVar1 != 0; iVar1 = iVar1 + -1) {
      entity = (GameEntityRuntime *)pAVar2->runtimeSlotAddressOrZero;
      if ((entity != (GameEntityRuntime *)0x0) && (((entity->common).runtimeFlags & 0x89) == 0)) {
        iVar3 = 0x1c;
        do {
          if (technologyIndex ==
              *(RuntimeToken *)
               ((int)(entity->common).ownership.definitionOrClassRecord + iVar3 * 4 + 0x1c4)) {
            Technology_ApplyRecordToEntity(technologyIndex,entity);
            pAVar2->runtimeSlotAddressOrZero = 0;
            return;
          }
          iVar3 = iVar3 + -1;
        } while (iVar3 != 0);
      }
      pAVar2 = pAVar2 + 1;
    }
  }
  else {
    GameFactionRuntime_RegisterArmyAssetPointers(0xffffffff,1,technologyIndex,factionIndex);
    AVar4 = ArmyAssetRegistry_FindByIdCf(technologyIndex);
    if (!AVar4.carry) {
      g_AiPurchaseAppliedArmyClassMask =
           g_AiPurchaseAppliedArmyClassMask | AVar4.eax[1].selectionDetailTemplateVariantIndex;
    }
  }
  return;
}


/* Address: 0x00539A40.
   Ownership: gameplay/ai/planning.
   Purpose: Examines active primary and secondary AI workspaces to decide whether the faction planning pressure
   flag must be set. When the faction runtime flag is already active it also republishes model occupancy data into
   the shared spatial structure. Stock tech.tec has 512 records over canonical ids 0..255; localized titles do not
   prove source-building, tier, direction, or effect mappings.
   Cross-module calls: ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive [world/model/hierarchy].
*/
void __thandor_void_preserve_eax_ecx_edx
AiFactionPlanning_UpdateActiveEntityPressureFlag(FactionRuntimeIndex factionIndex)

{
  FactionRuntimeFlags *pFVar1;
  int iVar2;
  int iVar3;
  WorldRuntimeContext *contextArg;
  AiWorkspace00EntryView8 *pAVar4;
  AiRuntimeWorkspaceEntry *runtimeWorkspaceEntry;
  ArmyRuntimeSlot *armySlot1;
  
  iVar3 = 2;
  pAVar4 = g_AiWorkspaceBuffer00_Size0400;
  for (iVar2 = g_AiWorkspace00Count; iVar2 != 0; iVar2 = iVar2 + -1) {
    if ((pAVar4->runtimeSlotAddressOrZero != 0) &&
       ((pAVar4->armyAssetId < ARM_0340_BUILDING_MDL0314 ||
        ((pAVar4->armyAssetId < ARM_0380_BUILDING_MDL0329 && (iVar3 = iVar3 + -1, iVar3 == 0))))))
    goto AiFactionPlanning_UpdateActiveEntityPressureFlag_SetPressureFlag;
    pAVar4 = pAVar4 + 1;
  }
  iVar3 = iVar3 + 1;
  iVar2 = g_AiWorkspace01Count;
  runtimeWorkspaceEntry = g_AiWorkspaceBuffer01_Size0200;
  while( true ) {
    if (iVar2 == 0) {
      if ((g_GameFactionRuntimeImage.records[factionIndex].runtimeFlags & 1) != 0) {
        contextArg = &g_InGameRuntimeRoot->worldRuntime0A30;
        pAVar4 = g_AiWorkspaceBuffer00_Size0400;
        for (iVar3 = g_AiWorkspace00Count; iVar2 = g_AiWorkspace01Count,
            runtimeWorkspaceEntry = g_AiWorkspaceBuffer01_Size0200, iVar3 != 0; iVar3 = iVar3 + -1)
        {
          if (pAVar4->runtimeSlotAddressOrZero != 0) {
            ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive
                      (contextArg,*(int **)(pAVar4->runtimeSlotAddressOrZero + 8));
          }
          pAVar4 = pAVar4 + 1;
        }
        for (; iVar2 != 0; iVar2 = iVar2 + -1) {
          if (runtimeWorkspaceEntry->armyRuntime != (ArmyRuntimeSlot *)0x0) {
            ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive
                      (contextArg,(int *)runtimeWorkspaceEntry->armyRuntime->linkedEntityRuntime);
          }
          runtimeWorkspaceEntry = runtimeWorkspaceEntry + 1;
        }
      }
      return;
    }
    armySlot1 = runtimeWorkspaceEntry->armyRuntime;
    if ((((armySlot1 != (ArmyRuntimeSlot *)0x0) && (armySlot1->factionIndex != 0)) &&
        ((armySlot1[1].commandCoordinate0Q12 != 0 ||
         ((1 < (uint)armySlot1->factionIndex && (armySlot1[1].runtimeState40 != 0)))))) &&
       (iVar3 = iVar3 + -1, iVar3 == 0)) break;
    runtimeWorkspaceEntry = runtimeWorkspaceEntry + 1;
    iVar2 = iVar2 + -1;
  }
AiFactionPlanning_UpdateActiveEntityPressureFlag_SetPressureFlag:
  pFVar1 = &g_GameFactionRuntimeImage.records[factionIndex].runtimeFlags;
  *pFVar1 = *pFVar1 | 1;
  return;
}


/* Address: 0x00539D20.
   Ownership: gameplay/ai/planning.
   Purpose: Adds weighted construction candidates for special structure IDs 0x14B or 0x14C according to assigned
   and unassigned workspace counts, faction limits, and knowledge parameters, then derives an additional weight
   from a matching workspace08 record. Stock ARM contains 675 records and 326 unique ids; placement workspace,
   producer, tier, class, and faction-role semantics are not inferred from numeric adjacency. Typed parameters: p2
   baseWeight→AiCandidateScore32_V342. Calling convention, complete VariableStorage serialization, function bytes,
   control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: AiPrimaryWorkspace_HasUnassignedEntryByIdCf [gameplay/ai/workspaces],
   AiPrimaryWorkspace_HasEntryByIdCf [gameplay/ai/workspaces], AiCandidateWorkspace_AddOrAccumulateWeightedEntry
   [gameplay/ai/workspaces], AiPlacement_ReserveAdditionalSpecialSite [gameplay/ai/placement],
   AiPrimaryWorkspace_CountAssignedEntriesById [gameplay/ai/workspaces].
*/
void __thandor_void_preserve_eax_ecx_edx
AiStructureCandidate_AddWeightedId14BOr14CCandidate
          (AiCandidateScore32 baseWeight,PckArmyAssetIdCatalog candidateArmyAssetId,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  uint uVar1;
  uint uVar2;
  AiTerrainFeatureWorkspaceEntry *pAVar3;
  int iVar4;
  dword dVar5;
  bool bVar6;
  AiKnowledgeDataImage *knowledgeData;
  
  bVar6 = AiPrimaryWorkspace_HasUnassignedEntryByIdCf(candidateArmyAssetId);
  knowledgeData = g_AiKnowledgeData;
  if (!bVar6) {
    uVar1 = g_GameFactionRuntimeImage.records[factionIndex].tritiumStorageLimitQ4;
    if (candidateArmyAssetId == ARM_0331_BUILDING_MDL0308) {
      uVar1 = g_GameFactionRuntimeImage.records[factionIndex].xeniteStorageLimitQ4;
      bVar6 = AiPrimaryWorkspace_HasEntryByIdCf(ARM_0330_BUILDING_MDL0303);
      if (((bVar6) && (uVar1 != 0)) &&
         (uVar1 < (knowledgeData->parameters).structure14bPrerequisite14aCountLimit)) {
        uVar2 = g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4;
        if (uVar1 == uVar2) {
          baseWeight = baseWeight << 2;
        }
        if (uVar1 - uVar2 < (knowledgeData->parameters).structure14bCountGapLimit) {
          AiCandidateWorkspace_AddOrAccumulateWeightedEntry(0x14b,baseWeight,1);
        }
      }
    }
    else {
      bVar6 = AiPrimaryWorkspace_HasEntryByIdCf(ARM_0332_BUILDING_MDL0302);
      pAVar3 = g_AiWorkspaceBuffer08_Size0200;
      iVar4 = g_AiWorkspace08Count;
      if (((bVar6) && (uVar1 != 0)) &&
         ((uVar1 < (knowledgeData->parameters).structure14dPrerequisite14cCountLimit &&
          (uVar1 - g_GameFactionRuntimeImage.records[factionIndex].tritiumCurrentQ4 <
           (knowledgeData->parameters).structure14dCountGapLimit)))) {
        AiCandidateWorkspace_AddOrAccumulateWeightedEntry(candidateArmyAssetId,baseWeight,1);
        pAVar3 = g_AiWorkspaceBuffer08_Size0200;
        iVar4 = g_AiWorkspace08Count;
      }
      for (; iVar4 != 0; iVar4 = iVar4 + -1) {
        bVar6 = AiPlacement_ReserveAdditionalSpecialSite
                          (pAVar3->armyAssetId,pAVar3->cell,factionIndex,worldRuntime);
        if (!bVar6) {
          dVar5 = (knowledgeData->parameters).workspace08Id14aDerivedWeight;
          if (pAVar3->armyAssetId != ARM_0330_BUILDING_MDL0303) {
            dVar5 = (knowledgeData->parameters).workspace08OtherDerivedWeight;
          }
          iVar4 = AiPrimaryWorkspace_CountAssignedEntriesById(pAVar3->armyAssetId);
          AiCandidateWorkspace_AddOrAccumulateWeightedEntry
                    (candidateArmyAssetId,(dVar5 * 3) / (iVar4 + 3U),1);
          return;
        }
        pAVar3 = pAVar3 + 1;
      }
    }
  }
  return;
}


/* Address: 0x00539E60.
   Ownership: gameplay/ai/planning.
   Purpose: Adds candidate ID 0x136 when the faction resource and capacity relationship indicates a deficit. The
   weight is scaled by the current production, storage, and AI knowledge parameters. Stock tech.tec has 512 records
   over canonical ids 0..255; localized titles do not prove source-building, tier, direction, or effect mappings.
   Cross-module calls: AiPrimaryWorkspace_HasEntryByIdCf [gameplay/ai/workspaces],
   AiPrimaryWorkspace_HasUnassignedEntryByIdCf [gameplay/ai/workspaces],
   AiCandidateWorkspace_AddOrAccumulateWeightedEntry [gameplay/ai/workspaces].
*/
void __thandor_void_preserve_eax_ecx_edx
AiResourceCandidate_AddWeightedId136(FactionRuntimeIndex factionIndex)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  bool bVar5;
  
  bVar5 = AiPrimaryWorkspace_HasEntryByIdCf(ARM_0330_BUILDING_MDL0303);
  if (bVar5) {
    bVar5 = AiPrimaryWorkspace_HasUnassignedEntryByIdCf(ARM_0310_BUILDING_MDL0305);
    if (!bVar5) {
      iVar4 = (int)g_GameFactionRuntimeImage.records[factionIndex].energyGenerationCapacityQ4 >> 4;
      iVar3 = (int)(g_GameFactionRuntimeImage.records[factionIndex].suppliedEnergyDemandQ4 +
                   g_GameFactionRuntimeImage.records[factionIndex].unpoweredEnergyDemandQ4) >> 4;
      if (iVar3 == 0) {
        iVar3 = 1;
      }
      iVar1 = ((int)g_GameFactionRuntimeImage.records[factionIndex].baselineEnergySupplyQ4 >> 4) +
              g_GameFactionRuntimeImage.records[factionIndex].tritiumExtractionRateQ4PerTick;
      iVar2 = iVar1 - iVar4;
      if (iVar2 != 0 && iVar4 <= iVar1) {
        AiCandidateWorkspace_AddOrAccumulateWeightedEntry
                  (0x136,(dword)(((longlong)
                                  (int)(((longlong)iVar2 * (longlong)iVar3) / (longlong)iVar4) *
                                 (longlong)
                                 (int)(g_AiKnowledgeData->parameters).
                                      resource136DeficitScoreNumerator) /
                                (longlong)
                                (int)(g_AiKnowledgeData->parameters).
                                     resource136DeficitScoreDenominator),1);
      }
    }
  }
  return;
}


/* Address: 0x0053A9D0.
   Ownership: gameplay/ai/planning.
   Purpose: Evaluates strategic class IDs 0x12F through 0x132 using faction category totals, class-specific
   coefficient tables, current workspace presence, and randomized tie variation. The best available class and
   comparison state are returned through the engine register convention.
   Cross-module calls: AiPrimaryWorkspace_HasEntryByIdCf [gameplay/ai/workspaces],
   ArmyAssetRegistry_FindEnabledByIdCf [assets/army/catalog].
*/
AiStrategicClassSelectionRegs8 __thandor_regs_ebx_ecx_preserve_eax_edx
AiStrategicClass_SelectBestCandidate12FTo132
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  GridScratchStateMask GVar1;
  dword dVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  int iVar9;
  uint uVar10;
  GridScratchCell *pGVar11;
  uint uVar12;
  bool bVar13;
  AiStrategicClassSelectionRegs8 AVar14;
  dword local_24;
  uint randomizedTieBits;
  RuntimeToken local_1c;
  int bestCandidateScore;
  
  uVar6 = g_GridScratchWidth * g_GridScratchHeight;
  uVar10 = 0;
  uVar8 = 0;
  uVar12 = 0;
  uVar7 = uVar6;
  pGVar11 = g_GridScratchPrimary;
  do {
    GVar1 = pGVar11->stateMask;
    if ((GVar1 & GRID_SCRATCH_TERRAIN_CLASS_BIT24) == 0) {
      uVar10 = uVar10 + 1;
    }
    if ((GVar1 & (GRID_SCRATCH_TERRAIN_CLASS_BIT27|GRID_SCRATCH_TERRAIN_CLASS_BIT26|
                 GRID_SCRATCH_TERRAIN_CLASS_BIT25)) == 0) {
      uVar8 = uVar8 + 1;
    }
    if ((GVar1 & (GRID_SCRATCH_TERRAIN_CLASS_BIT30|GRID_SCRATCH_TERRAIN_CLASS_BIT29|
                 GRID_SCRATCH_TERRAIN_CLASS_BIT28)) == 0) {
      uVar12 = uVar12 + 1;
    }
    pGVar11 = pGVar11 + 1;
    uVar7 = uVar7 - 1;
  } while (uVar7 != 0);
  iVar3 = (int)(((ulonglong)uVar8 * 100) / (ulonglong)uVar6);
  iVar4 = (int)(((ulonglong)uVar10 * 100) / (ulonglong)uVar6);
  iVar5 = (int)(((ulonglong)uVar12 * 100) / (ulonglong)uVar6);
  randomizedTieBits = (*g_RandomGeneratorState.next)();
  bestCandidateScore = 0;
  local_1c = 0;
  local_24 = 5;
  bVar13 = AiPrimaryWorkspace_HasEntryByIdCf(ARM_0302_BUILDING_MDL0300);
  dVar2 = local_24;
  if (!bVar13) {
    local_24 = 4;
    bVar13 = ArmyAssetRegistry_FindEnabledByIdCf(ARM_0302_BUILDING_MDL0300);
    dVar2 = 4;
    if (!bVar13) {
      local_1c = 0x12e;
      uVar7 = randomizedTieBits & 0x3fff;
      randomizedTieBits = randomizedTieBits >> 5;
      bestCandidateScore =
           iVar5 * g_AiStrategicClass12FWeightComponent0 +
           iVar3 * g_AiStrategicClass12FWeightComponent1 +
           iVar4 * g_AiStrategicClass12FWeightComponent2 + uVar7;
      dVar2 = local_24;
    }
  }
  local_24 = dVar2;
  bVar13 = AiPrimaryWorkspace_HasEntryByIdCf(ARM_0303_BUILDING_MDL0316);
  if (!bVar13) {
    local_24 = local_24 - 1;
    bVar13 = ArmyAssetRegistry_FindEnabledByIdCf(ARM_0303_BUILDING_MDL0316);
    if (!bVar13) {
      uVar7 = randomizedTieBits & 0x3fff;
      randomizedTieBits = randomizedTieBits >> 5;
      iVar9 = iVar5 * g_AiStrategicClass130WeightComponent0 +
              iVar3 * g_AiStrategicClass130WeightComponent1 +
              iVar4 * g_AiStrategicClass130WeightComponent2 + uVar7;
      if (bestCandidateScore < iVar9) {
        local_1c = 0x12f;
        bestCandidateScore = iVar9;
      }
    }
  }
  bVar13 = AiPrimaryWorkspace_HasEntryByIdCf(ARM_0304_BUILDING_MDL0324);
  if (!bVar13) {
    local_24 = local_24 - 1;
    bVar13 = ArmyAssetRegistry_FindEnabledByIdCf(ARM_0304_BUILDING_MDL0324);
    if (!bVar13) {
      uVar7 = randomizedTieBits & 0x3fff;
      randomizedTieBits = randomizedTieBits >> 5;
      iVar9 = iVar5 * g_AiStrategicClass131WeightComponent0 +
              iVar3 * g_AiStrategicClass131WeightComponent1 +
              iVar4 * g_AiStrategicClass131WeightComponent2 + uVar7;
      if (bestCandidateScore < iVar9) {
        local_1c = 0x130;
        bestCandidateScore = iVar9;
      }
    }
  }
  bVar13 = AiPrimaryWorkspace_HasEntryByIdCf(ARM_0305_BUILDING_MDL0317);
  if (!bVar13) {
    local_24 = local_24 - 1;
    bVar13 = ArmyAssetRegistry_FindEnabledByIdCf(ARM_0305_BUILDING_MDL0317);
    if (!bVar13) {
      iVar3 = iVar5 * g_AiStrategicClass132WeightComponent0 +
              iVar3 * g_AiStrategicClass132WeightComponent1 +
              iVar4 * g_AiStrategicClass132WeightComponent2 + (randomizedTieBits & 0x3fff);
      if (bestCandidateScore < iVar3) {
        local_1c = 0x131;
        bestCandidateScore = iVar3;
      }
    }
  }
  bVar13 = AiPrimaryWorkspace_HasEntryByIdCf(ARM_0306_BUILDING_MDL0310);
  if (!bVar13) {
    local_24 = local_24 - 1;
    bVar13 = ArmyAssetRegistry_FindEnabledByIdCf(ARM_0306_BUILDING_MDL0310);
    if (!bVar13) {
      if (bestCandidateScore < 0) {
        local_1c = 0x132;
      }
    }
  }
  AVar14.selectedRuntimeToken = local_1c;
  AVar14.existingCountOrPressure = local_24;
  return AVar14;
}


/* Address: 0x0053AF00.
   Ownership: gameplay/ai/planning.
   Purpose: Selects among strategic class IDs 0x141 through 0x143 using faction category totals, knowledge
   coefficients, current workspace entries, and randomized variation. The selected class and existing-count state
   are returned through the original register convention. It is distinct from FrontendPlayerIndex_V306,
   PlayerRuntimeId, active-faction masks or codes, and PCK-backed ArmyAssetId, ModelDefinitionId, and TechnologyId
   domains.
   Cross-module calls: AiPrimaryWorkspace_HasEntryByIdCf [gameplay/ai/workspaces],
   ArmyAssetRegistry_FindEnabledByIdCf [assets/army/catalog].
*/
AiStrategicClassSelectionRegs8 __thandor_regs_ebx_ecx_preserve_eax_edx
AiStrategicClass_SelectWeightedClass141To143
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  int iVar1;
  int iVar2;
  int iVar3;
  dword dVar4;
  dword dVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  dword dVar9;
  dword dVar10;
  dword dVar11;
  int iVar12;
  int iVar13;
  int iVar14;
  dword dVar15;
  dword dVar16;
  dword dVar17;
  dword dVar18;
  RuntimeToken RVar19;
  uint uVar20;
  dword dVar21;
  uint uVar22;
  uint uVar23;
  uint uVar24;
  bool bVar25;
  AiStrategicClassSelectionRegs8 AVar26;
  
  iVar1 = g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[2];
  iVar2 = g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[3];
  iVar3 = g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[4];
  dVar21 = (g_AiKnowledgeData->parameters).unknownParameterDwords90_119[0x1a];
  dVar4 = (g_AiKnowledgeData->parameters).unknownParameterDwords90_119[0x1b];
  dVar5 = (g_AiKnowledgeData->parameters).unknownParameterDwords90_119[0x1c];
  iVar6 = g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[2];
  iVar7 = g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[3];
  iVar8 = g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[4];
  dVar9 = (g_AiKnowledgeData->parameters).strategicClass141Weight;
  dVar10 = (g_AiKnowledgeData->parameters).strategicClass142Weight;
  dVar11 = (g_AiKnowledgeData->parameters).strategicClass143Weight;
  iVar12 = g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[2];
  iVar13 = g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[3];
  iVar14 = g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[4];
  dVar15 = (g_AiKnowledgeData->parameters).unknownParameterDwords123_127[1];
  dVar16 = (g_AiKnowledgeData->parameters).unknownParameterDwords123_127[2];
  dVar17 = (g_AiKnowledgeData->parameters).unknownParameterDwords123_127[3];
  uVar20 = g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[2] +
           g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[3] +
           g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[4] + 1;
  dVar18 = (*g_RandomGeneratorState.next)();
  uVar22 = ((iVar1 + 1) * dVar21 + (iVar2 + 1) * dVar4 + (iVar3 + 1) * dVar5) / uVar20 +
           (dVar18 & 0x7f);
  uVar24 = ((iVar6 + 1) * dVar9 + (iVar7 + 1) * dVar10 + (iVar8 + 1) * dVar11) / uVar20 +
           (dVar18 >> 0x13 & 0x7f);
  uVar23 = 0;
  dVar21 = 3;
  RVar19 = 0;
  bVar25 = AiPrimaryWorkspace_HasEntryByIdCf(ARM_0321_BUILDING_MDL0326);
  if (!bVar25) {
    dVar21 = 2;
    bVar25 = ArmyAssetRegistry_FindEnabledByIdCf(ARM_0321_BUILDING_MDL0326);
    if ((!bVar25) && (uVar22 != 0)) {
      RVar19 = 0x141;
      uVar23 = uVar22;
    }
  }
  bVar25 = AiPrimaryWorkspace_HasEntryByIdCf(ARM_0322_BUILDING_MDL0327);
  if (!bVar25) {
    dVar21 = dVar21 - 1;
    bVar25 = ArmyAssetRegistry_FindEnabledByIdCf(ARM_0322_BUILDING_MDL0327);
    if ((!bVar25) && (uVar23 < uVar24)) {
      RVar19 = 0x142;
      uVar23 = uVar24;
    }
  }
  bVar25 = AiPrimaryWorkspace_HasEntryByIdCf(ARM_0323_BUILDING_MDL0328);
  if (!bVar25) {
    dVar21 = dVar21 - 1;
    bVar25 = ArmyAssetRegistry_FindEnabledByIdCf(ARM_0323_BUILDING_MDL0328);
    if ((!bVar25) &&
       (uVar23 < ((iVar12 + 1) * dVar15 + (iVar13 + 1) * dVar16 + (iVar14 + 1) * dVar17) / uVar20 +
                 (dVar18 >> 7 & 0x7f))) {
      RVar19 = 0x143;
    }
  }
  AVar26.selectedRuntimeToken = RVar19;
  AVar26.existingCountOrPressure = dVar21;
  return AVar26;
}


/* Address: 0x00539600.
   Ownership: gameplay/ai/planning.
   Purpose: Places an army asset by scoring workspace10 points with randomized tie variation and a connected-region
   reachability test. The chosen point is instantiated and initialized before the pending faction asset is
   consumed. Stock ARM contains 675 records and 326 unique ids; placement workspace, producer, tier, class, and
   faction-role semantics are not inferred from numeric adjacency.
   Local calls: AiConstructionPlanner_ConsumeFactionPendingArmyAsset.
   Cross-module calls: ArmyAssetRegistry_FindByIdCf [assets/army/catalog],
   ModelDefinitionRegistry_FindByIdWithErrorCf [assets/model/definitions], ArmyPlacement_DispatchAssetAtFieldPoint
   [gameplay/army/placement], GridReachability_RebuildConnectedRegionAroundWorldPoint [world/pathing/grid],
   ArmyRuntime_CreateInstanceFromAssetCf [gameplay/army/runtime], ModelNodeRuntime_RebuildTransformsFromRoot
   [world/model/hierarchy].
*/
void __thandor_void_preserve_eax_ecx_edx
AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate
          (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime)

{
  dword radiusMetric;
  FieldGridCell *pFVar1;
  ArmyRuntimeSlot *modelNodeRuntime;
  ArmyRuntimeSlot *pAVar2;
  ModelRuntimeSlot *pMVar3;
  int iVar4;
  dword dVar5;
  ArmyRuntimeSlot **armySlot1;
  int iVar6;
  int iVar7;
  FieldGridCell **gridCellCursor;
  bool bVar8;
  ArmyRegistryEaxCf5_51b6d0 AVar9;
  ModelDefinitionLookupEaxCf5 MVar10;
  ArmyPlacementDispatchEaxCf5 AVar11;
  ArmyRuntimeCreateEaxCf5 AVar12;
  FieldGridCell *local_2c;
  int local_24;
  
  AVar9 = ArmyAssetRegistry_FindByIdCf(armyAssetId);
  if ((!AVar9.carry) && (g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorCooldown == 0)
     ) {
    MVar10 = ModelDefinitionRegistry_FindByIdWithErrorCf
                       (*(PckModelDefinitionIdCatalog *)
                         ((AVar9.eax)->rootNodeOffsetOrPointer + 0x20));
    if (!MVar10.carry) {
      radiusMetric = MVar10.modelDefinition[0x12].flags;
      if (g_AiWorkspace10Count != 0) {
        local_24 = 0x7fffffff;
        iVar6 = g_AiWorkspace10Count;
        gridCellCursor = g_AiWorkspaceBuffer10_Size0400;
        do {
          pFVar1 = *gridCellCursor;
          iVar4 = pFVar1->worldX;
          iVar7 = pFVar1->worldY;
          if (g_AiWorkspaceOwnedAsset300Runtime != (ArmyRuntimeSlot *)0x0) {
            iVar4 = iVar4 - (g_AiWorkspaceOwnedAsset300Runtime->modelNodeRuntime->worldTransform).
                            translation.x;
            if (iVar4 < 0) {
              iVar4 = -iVar4;
            }
            iVar7 = iVar7 - (g_AiWorkspaceOwnedAsset300Runtime->modelNodeRuntime->worldTransform).
                            translation.y;
            if (iVar7 < 0) {
              iVar7 = -iVar7;
            }
          }
          dVar5 = (*g_RandomGeneratorState.next)();
          iVar4 = iVar7 + iVar4 + (dVar5 & 0xffff);
          if (iVar4 < local_24) {
            AVar11 = ArmyPlacement_DispatchAssetAtFieldPoint
                               (1,0,(uint)(ushort)pFVar1->triangle0NormalAngles,pFVar1->worldY,
                                pFVar1->worldX,armyAssetId,factionIndex,(UiRootNode *)worldRuntime);
            if (!AVar11.carry) {
              bVar8 = GridReachability_RebuildConnectedRegionAroundWorldPoint
                                (radiusMetric,pFVar1->worldY,pFVar1->worldX);
              if (!bVar8) {
                local_2c = pFVar1;
                local_24 = iVar4;
              }
            }
          }
          gridCellCursor = gridCellCursor + 1;
          iVar6 = iVar6 + -1;
        } while (iVar6 != 0);
        if (local_24 < 0x7fffffff) {
          AVar12 = ArmyRuntime_CreateInstanceFromAssetCf
                             (4,(uint)(ushort)local_2c->triangle0NormalAngles,local_2c->worldY,
                              local_2c->worldX,factionIndex,armyAssetId,worldRuntime);
          armySlot1 = (ArmyRuntimeSlot **)AVar12.eax;
          if (!AVar12.carry) {
            modelNodeRuntime = armySlot1[1];
            pAVar2 = *armySlot1;
            modelNodeRuntime->movementPosition0Q12 = 0;
            pMVar3 = (pAVar2->modelRuntimeOrSavedOffset).modelRuntime;
            ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)modelNodeRuntime);
            ArmyRuntime_DispatchClassCommand(armySlot1,worldRuntime);
            EffectRuntimePool_CreateInstanceFromDefinitionCf
                      (EFFECT_RUNTIME_COMPLETION_NONE,(EffectRuntimeOwnerReference4)0x0,
                       (modelNodeRuntime->movementControl).turnVelocityAngle16,
                       (modelNodeRuntime->movementControl).movementAdvancePerTickQ12,
                       ((WorldRuntimeNodeModelPayload *)&modelNodeRuntime->factionIndex)->
                       worldRotationAngle0,modelNodeRuntime->depthBinClass,
                       modelNodeRuntime->runtimeState98,
                       ((GraphicsFixedVec3 *)&modelNodeRuntime->runtimeState94)->x,
                       (EffectDefinition *)pMVar3->attachments140[2].childLocalRotationAngle0,
                       worldRuntime);
            AiConstructionPlanner_ConsumeFactionPendingArmyAsset(armyAssetId,factionIndex);
          }
        }
      }
    }
  }
  return;
}


/* Address: 0x00539190.
   Ownership: gameplay/ai/planning.
   Purpose: Marks one AI construction request as committed, increments the faction completion counter, removes the
   matching army asset pointer from the faction pending list, and decrements that list count. ArmyAssetId is
   preserved through candidate, queue, refund, transfer, and sale paths; cost and eligibility semantics remain
   those proved by the live consumers and PCK records. Stock ARM contains 675 records and 326 unique ids; placement
   workspace, producer, tier, class, and faction-role semantics are not inferred from numeric adjacency.
   Cross-module calls: ArmyAssetRegistry_FindByIdCf [assets/army/catalog].
*/
void __thandor_void_preserve_eax_ecx
AiConstructionPlanner_ConsumeFactionPendingArmyAsset
          (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex)

{
  FactionRelationCounter *pFVar1;
  FactionArmyAssetCount *pFVar2;
  FactionArmyAssetCount FVar3;
  dword *pdVar4;
  ArmyRegistryEaxCf5_51b6d0 AVar5;
  
  g_AiConstructionPendingAssetConsumedCount = g_AiConstructionPendingAssetConsumedCount + 1;
  FVar3 = g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
  AVar5 = ArmyAssetRegistry_FindByIdCf(armyAssetId);
  pdVar4 = g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetPointersOrIds;
  pFVar1 = &g_GameFactionRuntimeImage.records[factionIndex].relationCounterB;
  *pFVar1 = *pFVar1 + 1;
  while( true ) {
    if (FVar3 == 0) {
      return;
    }
    if (AVar5.eax == (ArmyAssetRecordPrefix *)*pdVar4) break;
    pdVar4 = pdVar4 + 1;
    FVar3 = FVar3 - 1;
  }
  pFVar2 = &g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
  *pFVar2 = *pFVar2 - 1;
  while (FVar3 = FVar3 - 1, FVar3 != 0) {
    *pdVar4 = pdVar4[1];
    pdVar4 = pdVar4 + 1;
  }
  return;
}


/* Address: 0x0053A980.
   Ownership: gameplay/ai/planning.
   Purpose: Handles ai faction runtime test planning capacity exceeded carry-flag result.
*/
bool __thandor_cf_preserve_eax_ecx_edx
AiFactionRuntime_TestPlanningCapacityExceededCf
          (dword additionalPlanningCapacity,FactionRuntimeIndex factionIndex)

{
  int iVar1;
  int iVar2;
  
  iVar1 = ((int)g_GameFactionRuntimeImage.records[factionIndex].baselineEnergySupplyQ4 >> 4) +
          g_GameFactionRuntimeImage.records[factionIndex].tritiumExtractionRateQ4PerTick;
  iVar2 = (int)g_GameFactionRuntimeImage.records[factionIndex].energyGenerationCapacityQ4 >> 4;
  if (iVar1 < iVar2) {
    iVar2 = iVar1;
  }
  return iVar2 < (int)(((int)(g_GameFactionRuntimeImage.records[factionIndex].suppliedEnergyDemandQ4
                             + g_GameFactionRuntimeImage.records[factionIndex].
                               unpoweredEnergyDemandQ4) >> 4) + additionalPlanningCapacity);
}


/* Address: 0x0053A2A0.
   Ownership: gameplay/ai/planning.
   Purpose: Computes a composite AI score for one army candidate. Stock tech.tec has 512 records over canonical ids
   0..255; localized titles do not prove source-building, tier, direction, or effect mappings. Typed parameters: p4
   armyAssetRecord→AiArmyAssetRecordAddress32_V345. Calling convention, complete VariableStorage serialization,
   function bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf [assets/model/definitions].
*/
AiCandidateScore32 __thandor_eax_preserve_ecx_edx
AiArmyCandidate_ComputeFactionWeightedScore
          (AiArmyScoreWeights *scoreWeights,FactionRuntimeIndex factionIndex,
          ArmyAssetRuntimeSemanticView80 *armyAssetRecord)

{
  ModelDefinitionResolvePhaseView280 *selectedModelDefinition;
  ModelDefinitionResolvePhaseView280 *selectedChildModelDefinition0;
  uint pressureWeightedDamage0;
  uint pressureWeightedDamage1;
  uint pressureWeightedDamage2;
  uint pressureWeightedDamage3;
  uint pressureWeightedDamage4;
  uint pressureWeightedDamage5;
  uint pressureWeightedDamage6;
  uint pressureWeightedDamage7;
  ModelDefinitionResolvePhaseView280 *selectedChildModelDefinition1;
  uint uVar1;
  int weightedDefinitionScore;
  ModelDefinitionLookupEaxCf5 MVar2;
  AiLinkedDefinitionListView *linkedDefinitionList;
  int secondChildScaleDivisor30;
  ShotDefinition *selectedShotDefinition;
  ShotDefinition *secondChildShotDefinition;
  int definitionScaleDivisor30;
  
  linkedDefinitionList = (AiLinkedDefinitionListView *)armyAssetRecord->rootNodeOffsetOrPointer;
  MVar2 = ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                    (factionIndex,(ModelLinkedDefinitionListAddress32)linkedDefinitionList);
  selectedModelDefinition = (ModelDefinitionResolvePhaseView280 *)MVar2.modelDefinition;
  if (MVar2.carry) {
    return 0;
  }
  weightedDefinitionScore = scoreWeights->baseScore;
  if (selectedModelDefinition->runtimeValue18 != 0) {
    weightedDefinitionScore = weightedDefinitionScore + scoreWeights->nonzeroDefinition18Bonus;
  }
  uVar1 = linkedDefinitionList->childListCount;
  weightedDefinitionScore =
       ((int)(((longlong)(int)selectedModelDefinition->runtimeValue0C *
              (longlong)scoreWeights->definitionValue0CWeight) /
             (longlong)g_AiArmyCandidateFlaggedDefinitionValueMaximum) +
       weightedDefinitionScore +
       (int)(((longlong)(int)selectedModelDefinition->runtimeValue60 *
             (longlong)scoreWeights->definitionValue60Weight) /
            (longlong)
            (&g_TechnologyCategoryMaximum0)[selectedModelDefinition->categoryMaximumIndex5C])) * 8;
  if (uVar1 != 0) {
    MVar2 = ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                      (factionIndex,linkedDefinitionList->childList0Address);
    selectedChildModelDefinition0 = (ModelDefinitionResolvePhaseView280 *)MVar2.modelDefinition;
    if (MVar2.carry) {
      return 0;
    }
    if (selectedChildModelDefinition0->runtimeValue30 != 0) {
      selectedShotDefinition = selectedChildModelDefinition0->shotDefinitionReference2C;
      definitionScaleDivisor30 = selectedChildModelDefinition0->runtimeValue30;
      if (g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure != 0) {
        pressureWeightedDamage0 =
             (uint)(((longlong)
                     (int)(((longlong)selectedShotDefinition->targetClassImpactDamageQ12[0] *
                           (longlong)scoreWeights->pressureCategoryWeights[0]) /
                          (longlong)definitionScaleDivisor30) *
                    (longlong)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[0]) /
                   (longlong)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
        pressureWeightedDamage1 =
             (uint)(((longlong)
                     (int)(((longlong)selectedShotDefinition->targetClassImpactDamageQ12[1] *
                           (longlong)scoreWeights->pressureCategoryWeights[1]) /
                          (longlong)definitionScaleDivisor30) *
                    (longlong)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[1]) /
                   (longlong)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
        pressureWeightedDamage2 =
             (uint)(((longlong)
                     (int)(((longlong)selectedShotDefinition->targetClassImpactDamageQ12[2] *
                           (longlong)scoreWeights->pressureCategoryWeights[2]) /
                          (longlong)definitionScaleDivisor30) *
                    (longlong)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[2]) /
                   (longlong)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
        pressureWeightedDamage3 =
             (uint)(((longlong)
                     (int)(((longlong)selectedShotDefinition->targetClassImpactDamageQ12[3] *
                           (longlong)scoreWeights->pressureCategoryWeights[3]) /
                          (longlong)definitionScaleDivisor30) *
                    (longlong)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[3]) /
                   (longlong)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
        pressureWeightedDamage4 =
             (uint)(((longlong)
                     (int)(((longlong)selectedShotDefinition->targetClassImpactDamageQ12[4] *
                           (longlong)scoreWeights->pressureCategoryWeights[4]) /
                          (longlong)definitionScaleDivisor30) *
                    (longlong)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[4]) /
                   (longlong)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
        pressureWeightedDamage5 =
             (uint)(((longlong)
                     (int)(((longlong)selectedShotDefinition->targetClassImpactDamageQ12[5] *
                           (longlong)scoreWeights->pressureCategoryWeights[5]) /
                          (longlong)definitionScaleDivisor30) *
                    (longlong)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[5]) /
                   (longlong)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
        pressureWeightedDamage6 =
             (uint)(((longlong)
                     (int)(((longlong)selectedShotDefinition->targetClassImpactDamageQ12[6] *
                           (longlong)scoreWeights->pressureCategoryWeights[6]) /
                          (longlong)definitionScaleDivisor30) *
                    (longlong)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[6]) /
                   (longlong)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
        pressureWeightedDamage7 =
             (uint)(((longlong)
                     (int)(((longlong)selectedShotDefinition->targetClassImpactDamageQ12[7] *
                           (longlong)scoreWeights->pressureCategoryWeights[7]) /
                          (longlong)definitionScaleDivisor30) *
                    (longlong)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[7]) /
                   (longlong)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
        weightedDefinitionScore =
             weightedDefinitionScore +
             (int)(CONCAT44(pressureWeightedDamage0 >> 0x16,pressureWeightedDamage0 << 10) /
                  (longlong)g_TechnologyCategoryMaximum0) +
             (int)(CONCAT44(pressureWeightedDamage1 >> 0x16,pressureWeightedDamage1 << 10) /
                  (longlong)g_TechnologyCategoryMaximum1) +
             (int)(CONCAT44(pressureWeightedDamage2 >> 0x16,pressureWeightedDamage2 << 10) /
                  (longlong)g_TechnologyCategoryMaximum2) +
             (int)(CONCAT44(pressureWeightedDamage3 >> 0x16,pressureWeightedDamage3 << 10) /
                  (longlong)g_TechnologyCategoryMaximum3) +
             (int)(CONCAT44(pressureWeightedDamage4 >> 0x16,pressureWeightedDamage4 << 10) /
                  (longlong)g_TechnologyCategoryMaximum4) +
             (int)(CONCAT44(pressureWeightedDamage5 >> 0x16,pressureWeightedDamage5 << 10) /
                  (longlong)g_TechnologyCategoryMaximum5) +
             (int)(CONCAT44(pressureWeightedDamage6 >> 0x16,pressureWeightedDamage6 << 10) /
                  (longlong)g_TechnologyCategoryMaximum6) +
             (int)(CONCAT44(pressureWeightedDamage7 >> 0x16,pressureWeightedDamage7 << 10) /
                  (longlong)g_TechnologyCategoryMaximum7);
      }
    }
    if (1 < uVar1) {
      MVar2 = ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                        (factionIndex,linkedDefinitionList->childList1Address);
      selectedChildModelDefinition1 = (ModelDefinitionResolvePhaseView280 *)MVar2.modelDefinition;
      if (MVar2.carry) {
        return 0;
      }
      if (selectedChildModelDefinition1->runtimeValue30 != 0) {
        secondChildShotDefinition = selectedChildModelDefinition1->shotDefinitionReference2C;
        secondChildScaleDivisor30 = selectedChildModelDefinition1->runtimeValue30;
        if (g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure != 0) {
          uVar1 = (uint)(((longlong)
                          (int)(((longlong)secondChildShotDefinition->targetClassImpactDamageQ12[0]
                                * (longlong)scoreWeights->pressureCategoryWeights[0]) /
                               (longlong)secondChildScaleDivisor30) *
                         (longlong)
                         g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[0]) /
                        (longlong)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure)
          ;
          pressureWeightedDamage0 =
               (uint)(((longlong)
                       (int)(((longlong)secondChildShotDefinition->targetClassImpactDamageQ12[1] *
                             (longlong)scoreWeights->pressureCategoryWeights[1]) /
                            (longlong)secondChildScaleDivisor30) *
                      (longlong)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[1])
                     / (longlong)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
          pressureWeightedDamage1 =
               (uint)(((longlong)
                       (int)(((longlong)secondChildShotDefinition->targetClassImpactDamageQ12[2] *
                             (longlong)scoreWeights->pressureCategoryWeights[2]) /
                            (longlong)secondChildScaleDivisor30) *
                      (longlong)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[2])
                     / (longlong)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
          pressureWeightedDamage2 =
               (uint)(((longlong)
                       (int)(((longlong)secondChildShotDefinition->targetClassImpactDamageQ12[3] *
                             (longlong)scoreWeights->pressureCategoryWeights[3]) /
                            (longlong)secondChildScaleDivisor30) *
                      (longlong)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[3])
                     / (longlong)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
          pressureWeightedDamage3 =
               (uint)(((longlong)
                       (int)(((longlong)secondChildShotDefinition->targetClassImpactDamageQ12[4] *
                             (longlong)scoreWeights->pressureCategoryWeights[4]) /
                            (longlong)secondChildScaleDivisor30) *
                      (longlong)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[4])
                     / (longlong)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
          pressureWeightedDamage4 =
               (uint)(((longlong)
                       (int)(((longlong)secondChildShotDefinition->targetClassImpactDamageQ12[5] *
                             (longlong)scoreWeights->pressureCategoryWeights[5]) /
                            (longlong)secondChildScaleDivisor30) *
                      (longlong)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[5])
                     / (longlong)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
          pressureWeightedDamage5 =
               (uint)(((longlong)
                       (int)(((longlong)secondChildShotDefinition->targetClassImpactDamageQ12[6] *
                             (longlong)scoreWeights->pressureCategoryWeights[6]) /
                            (longlong)secondChildScaleDivisor30) *
                      (longlong)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[6])
                     / (longlong)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
          pressureWeightedDamage6 =
               (uint)(((longlong)
                       (int)(((longlong)secondChildShotDefinition->targetClassImpactDamageQ12[7] *
                             (longlong)scoreWeights->pressureCategoryWeights[7]) /
                            (longlong)secondChildScaleDivisor30) *
                      (longlong)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[7])
                     / (longlong)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
          weightedDefinitionScore =
               weightedDefinitionScore +
               (int)(CONCAT44(uVar1 >> 0x16,uVar1 << 10) / (longlong)g_TechnologyCategoryMaximum0) +
               (int)(CONCAT44(pressureWeightedDamage0 >> 0x16,pressureWeightedDamage0 << 10) /
                    (longlong)g_TechnologyCategoryMaximum1) +
               (int)(CONCAT44(pressureWeightedDamage1 >> 0x16,pressureWeightedDamage1 << 10) /
                    (longlong)g_TechnologyCategoryMaximum2) +
               (int)(CONCAT44(pressureWeightedDamage2 >> 0x16,pressureWeightedDamage2 << 10) /
                    (longlong)g_TechnologyCategoryMaximum3) +
               (int)(CONCAT44(pressureWeightedDamage3 >> 0x16,pressureWeightedDamage3 << 10) /
                    (longlong)g_TechnologyCategoryMaximum4) +
               (int)(CONCAT44(pressureWeightedDamage4 >> 0x16,pressureWeightedDamage4 << 10) /
                    (longlong)g_TechnologyCategoryMaximum5) +
               (int)(CONCAT44(pressureWeightedDamage5 >> 0x16,pressureWeightedDamage5 << 10) /
                    (longlong)g_TechnologyCategoryMaximum6) +
               (int)(CONCAT44(pressureWeightedDamage6 >> 0x16,pressureWeightedDamage6 << 10) /
                    (longlong)g_TechnologyCategoryMaximum7);
        }
      }
    }
  }
  return scoreWeights->armyRecord74Weight * armyAssetRecord->definitionClassValue74 +
         weightedDefinitionScore * 0xc +
         scoreWeights->armyRecord78Weight * armyAssetRecord->definitionClassValue78 +
         scoreWeights->armyRecord70Weight * armyAssetRecord->definitionClassValue70;
}

