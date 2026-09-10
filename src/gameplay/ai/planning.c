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
  int *piVar1;
  int iVar2;
  AiFactionPlanningCallerLoopRegisterContinuityResult AVar3;
  GameSpeedQ8 GVar4;
  int iVar5;
  dword dVar6;
  uint uVar7;
  FrontendPlayerRuntimeBlockCount FVar8;
  FrontendPlayerRuntimeRecord *pFVar9;
  uint uVar10;
  int iVar11;
  int iVar12;
  FactionRuntimeLifecycleObservedState *pFVar13;
  WorldRuntimeNode *worldNode1;
  AiFactionPlanningCallerLoopRegisterContinuityResult AVar14;
  
  iVar12 = 0x50fa80;
  pFVar13 = g_GameFactionRuntimeImage.tail.factionLifecycleStates;
  dVar6 = 7;
  iVar5 = 1;
  do {
    FVar8 = g_FrontendPlayerRuntimeBlockCount;
    pFVar9 = g_FrontendPlayerRuntimeBlocks;
    pFVar13 = pFVar13 + 1;
    AVar3.preservedEcxRemainingFactionCount = dVar6;
    AVar3.preservedEaxFactionIndex = iVar5;
    AVar14.preservedEcxRemainingFactionCount = dVar6;
    AVar14.preservedEaxFactionIndex = iVar5;
    *(undefined4 *)(iVar12 + 0x48) = 0x100;
    GVar4 = g_GameFactionRuntimeImage.tail.gameSpeedQ8;
    if (*pFVar13 == FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
      do {
        AVar14 = AVar3;
        if (iVar5 == (pFVar9->factionAssignment).factionAssignmentIndex)
        goto AiFactionRuntime_RebuildPlanningCapacityState_AdvanceAfterPlayerOrAiPlanningDispatch;
        pFVar9 = pFVar9 + 1;
        FVar8 = FVar8 - 1;
      } while (FVar8 != 0);
      AVar14 = AiRuntime_DispatchFactionPlanningPhase
                         (iVar5,(WorldRuntimeContext *)g_InGameRuntimeRoot);
      *(GameSpeedQ8 *)(iVar12 + 0x48) = GVar4;
    }
AiFactionRuntime_RebuildPlanningCapacityState_AdvanceAfterPlayerOrAiPlanningDispatch:
    iVar12 = iVar12 + 0x740;
    iVar5 = AVar14.preservedEaxFactionIndex + 1;
    dVar6 = AVar14.preservedEcxRemainingFactionCount - 1;
    if (dVar6 == 0) {
      iVar12 = 0x50fa80;
      iVar5 = 7;
      uVar10 = 0;
      do {
        do {
          iVar11 = uVar10 * 4;
          iVar2 = *(int *)(iVar12 + 0x724 + uVar10 * 4);
          uVar10 = uVar10 + 2;
          *(uint *)(iVar12 + 0x718 + uVar10 * 4) =
               (*(int *)(iVar12 + 0x720 + iVar11) * 3 + 1U >> 2) + 1;
          *(uint *)(iVar12 + 0x71c + uVar10 * 4) = (iVar2 * 3 + 1U >> 2) + 1;
        } while (uVar10 < 8);
        *(undefined4 *)(iVar12 + 0x74) = 0;
        uVar10 = 0;
        iVar12 = iVar12 + 0x740;
        iVar5 = iVar5 + -1;
      } while (iVar5 != 0);
      worldNode1 = (g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
      if (worldNode1 != (WorldRuntimeNode *)0x0) {
        do {
          if (worldNode1[2].common.nextNode == (WorldRuntimeNode *)0x0) {
            iVar5 = *(int *)((int)worldNode1->runtimePayload + 8);
            if (*(int *)(iVar5 + 0xc) != 0) {
              iVar12 = *(int *)(*(int *)worldNode1->runtimePayload + 0x5c);
              uVar7 = 8;
              uVar10 = 1;
              iVar11 = 0x50fa80;
              do {
                if (((*(uint *)(iVar5 + 0x50) & uVar7) != 0) && (uVar10 != *(uint *)(iVar5 + 0xc)))
                {
                  piVar1 = (int *)(iVar11 + 0x720 + iVar12 * 4);
                  *piVar1 = *piVar1 + 0x100;
                }
                uVar7 = uVar7 << 2;
                uVar10 = uVar10 + 1;
                iVar11 = iVar11 + 0x740;
              } while (uVar10 < 8);
            }
          }
          worldNode1 = (worldNode1->common).nextNode;
        } while (worldNode1 != (WorldRuntimeNode *)0x0);
        iVar12 = 0x50fa80;
        iVar5 = 7;
        uVar10 = 0;
        do {
          do {
            iVar11 = *(int *)(iVar12 + 0x720 + uVar10 * 4);
            iVar2 = *(int *)(iVar12 + 0x724 + uVar10 * 4);
            if (*(int *)(iVar12 + 0x74) < iVar11) {
              *(int *)(iVar12 + 0x74) = iVar11;
            }
            uVar10 = uVar10 + 2;
            if (*(int *)(iVar12 + 0x74) < iVar2) {
              *(int *)(iVar12 + 0x74) = iVar2;
            }
          } while (uVar10 < 8);
          uVar10 = 0;
          iVar12 = iVar12 + 0x740;
          iVar5 = iVar5 + -1;
        } while (iVar5 != 0);
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
  ModelDefinitionRecordPrefix *modelDefinition1;
  AiCandidateScore32 AVar1;
  int extraout_ECX;
  int registryEntriesRemaining;
  int extraout_EDX;
  int extraout_EDX_00;
  ArmyAssetRecordPrefix **armyAssetRegistryCursor;
  bool bVar2;
  int compatibleAssetCount;
  uint compatibleAssetScoreSum;
  AiLinkedDefinitionListView *assetLinkedDefinitions;
  AiLinkedDefinitionListView *nestedLinkedDefinitions;
  AiLinkedDefinitionListView *secondNestedLinkedDefinitions;
  PckModelDefinitionIdCatalog candidateModelDefinitionId;
  
  modelDefinition1 = ModelDefinitionRegistry_FindByRuntimeClassId(runtimeClassId);
  AVar1 = 0;
  if (modelDefinition1 != (ModelDefinitionRecordPrefix *)0x0) {
    candidateModelDefinitionId = modelDefinition1->definitionId;
    armyAssetRegistryCursor = g_ArmyAssetRecordRegistry;
    registryEntriesRemaining = 0x300;
    compatibleAssetScoreSum = 0;
    compatibleAssetCount = 0;
    do {
      armyAssetRecord = (ArmyAssetRuntimeSemanticView80 *)*armyAssetRegistryCursor;
      if ((armyAssetRecord != (ArmyAssetRuntimeSemanticView80 *)0x0) &&
         ((armyAssetRecord->flags14 & 1) != 0)) {
        assetLinkedDefinitions =
             (AiLinkedDefinitionListView *)armyAssetRecord->rootNodeOffsetOrPointer;
        if ((((candidateModelDefinitionId != assetLinkedDefinitions->definitionIds[0]) &&
             (((candidateModelDefinitionId != assetLinkedDefinitions->definitionIds[1] &&
               (candidateModelDefinitionId != assetLinkedDefinitions->definitionIds[2])) &&
              (candidateModelDefinitionId != assetLinkedDefinitions->definitionIds[3])))) &&
            (((candidateModelDefinitionId != assetLinkedDefinitions->definitionIds[4] &&
              (candidateModelDefinitionId != assetLinkedDefinitions->definitionIds[5])) &&
             (candidateModelDefinitionId != assetLinkedDefinitions->definitionIds[6])))) &&
           (candidateModelDefinitionId != assetLinkedDefinitions->definitionIds[7])) {
          bVar2 = 0xffaf05df < (uint)(factionIndex * 0x740);
          ModelDefinition_IsFactionTechnologyUnlockedCf
                    (g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits,
                     assetLinkedDefinitions->definitionIds[0]);
          registryEntriesRemaining = extraout_EDX;
          if (((bVar2) ||
              (nestedLinkedDefinitions = *(AiLinkedDefinitionListView **)(extraout_ECX + 0xc),
              *(int *)(extraout_ECX + 8) == 0)) ||
             ((((candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[0] &&
                ((((candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[1] &&
                   (candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[2])) &&
                  (candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[3])) &&
                 ((candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[4] &&
                  (candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[5])))))) &&
               ((candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[6] &&
                (candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[7])))) &&
              ((secondNestedLinkedDefinitions =
                     *(AiLinkedDefinitionListView **)(extraout_ECX + 0x10),
               *(uint *)(extraout_ECX + 8) < 2 ||
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
        AVar1 = AiArmyCandidate_ComputeFactionWeightedScore
                          (scoreWeights,factionIndex,armyAssetRecord);
        registryEntriesRemaining = extraout_EDX_00;
        if (0 < AVar1) {
          compatibleAssetScoreSum = compatibleAssetScoreSum + AVar1;
          compatibleAssetCount = compatibleAssetCount + 1;
        }
      }
AiArmyCandidate_ComputeAverageCompatibleAssetScore_AdvanceRegistryScan:
      armyAssetRegistryCursor = armyAssetRegistryCursor + 1;
      registryEntriesRemaining = registryEntriesRemaining + -1;
    } while (registryEntriesRemaining != 0);
    AVar1 = 0;
    if (compatibleAssetCount != 0) {
      AVar1 = (AiCandidateScore32)
              ((longlong)(ulonglong)compatibleAssetScoreSum / (longlong)compatibleAssetCount);
    }
  }
  return AVar1;
}

/* Address: 0x005379E0.
   Ownership: gameplay/ai/planning.
   Purpose: Scans the secondary AI entity workspace and collects up to four unique combined grid-mask classes
   derived from each active entity definition. When no class is found it installs the verified default mask
   0x90000100.
*/
AiPlanningGridScanLoopContinuityResult __fastcall AiPlanning_CollectActiveGridMaskClasses(void)

{
  void *pvVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  dword in_ECX;
  dword in_EDX;
  int iVar6;
  AiRuntimeWorkspaceEntry *runtimeWorkspaceEntry;
  AiPlanningGridScanLoopContinuityResult AVar7;
  
  g_AiActiveGridMaskClass0 = 0xffffffff;
  g_AiActiveGridMaskClass1 = 0xffffffff;
  g_AiActiveGridMaskClass2 = 0xffffffff;
  g_AiActiveGridMaskClass3 = 0xffffffff;
  uVar2 = g_AiActiveGridMaskClass0;
  uVar3 = g_AiActiveGridMaskClass1;
  uVar4 = g_AiActiveGridMaskClass2;
  runtimeWorkspaceEntry = g_AiWorkspaceBuffer01_Size0200;
  for (iVar6 = g_AiWorkspace01Count; g_AiActiveGridMaskClass0 = uVar2, iVar6 != 0;
      iVar6 = iVar6 + -1) {
    g_AiActiveGridMaskClass1 = uVar3;
    g_AiActiveGridMaskClass2 = uVar4;
    if (runtimeWorkspaceEntry->armyRuntime != (ArmyRuntimeSlot *)0x0) {
      pvVar1 = runtimeWorkspaceEntry->armyRuntime->definitionOrAsset;
      if ((*(int *)((int)pvVar1 + 0x18) != 0) && (-1 < *(int *)((int)pvVar1 + 0x260))) {
        if ((((-1 < *(int *)((int)pvVar1 + 0x264)) &&
             ((((uVar5 = 0x100 << ((byte)*(int *)((int)pvVar1 + 0x260) & 0x1f) | 0x80000000U |
                         0x1000000 << ((byte)*(int *)((int)pvVar1 + 0x264) & 0x1f), uVar5 != uVar2
                && (uVar5 != uVar3)) && (uVar5 != uVar4)) &&
              ((uVar5 != g_AiActiveGridMaskClass3 &&
               (g_AiActiveGridMaskClass0 = uVar5, uVar2 != 0xffffffff)))))) &&
            (g_AiActiveGridMaskClass0 = uVar2, g_AiActiveGridMaskClass1 = uVar5, uVar3 != 0xffffffff
            )) && ((g_AiActiveGridMaskClass1 = uVar3, g_AiActiveGridMaskClass2 = uVar5,
                   uVar4 != 0xffffffff &&
                   (g_AiActiveGridMaskClass2 = uVar4, g_AiActiveGridMaskClass3 == 0xffffffff)))) {
          g_AiActiveGridMaskClass3 = uVar5;
        }
      }
    }
    runtimeWorkspaceEntry = runtimeWorkspaceEntry + 1;
    uVar2 = g_AiActiveGridMaskClass0;
    uVar3 = g_AiActiveGridMaskClass1;
    uVar4 = g_AiActiveGridMaskClass2;
  }
  g_AiActiveGridMaskClass1 = uVar3;
  g_AiActiveGridMaskClass2 = uVar4;
  if (uVar2 == 0xffffffff) {
    g_AiActiveGridMaskClass0 = 0x90000100;
  }
  AVar7.preservedEdxGridScratchRowStrideBytes = in_EDX;
  AVar7.preservedEcxInteriorGridRowCount = in_ECX;
  return AVar7;
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
AiFactionPlanningCallerLoopRegisterContinuityResult
AiRuntime_DispatchFactionPlanningPhase
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *inGameRuntime)

{
  uint uVar1;
  FactionRuntimeIndex in_EAX;
  dword in_ECX;
  AiPreservedFactionIndexEdxResult AVar2;
  FactionRuntimeIndex purchaseFactionIndexAfterCacheLoad;
  FactionRuntimeIndex factionIndex_00;
  FactionRuntimeIndex factionIndex_01;
  WorldObjectRecordCount *worldRuntime;
  FactionImageByteOffset factionImageByteOffset;
  undefined1 uVar3;
  AiFactionPlanningCallerLoopRegisterContinuityResult AVar4;
  AiPlanningDispatchRegisterContinuityResult AVar5;
  AiKnowledgeDataImage *knowledgeData;
  
  if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
       SESSION_NETWORK_ROLE_LOCAL) || ((g_UiCommandRuntimeFlags & 2) == 0)) {
    worldRuntime = &inGameRuntime[7].objectCount;
    if ((g_GameFactionRuntimeImage.tail.simulationTick >> 3 & 7) == factionIndex) {
      factionImageByteOffset = (FactionImageByteOffset)((longlong)factionIndex * 0x740);
      uVar3 = (longlong)(int)factionImageByteOffset != (longlong)factionIndex * 0x740;
      AVar5 = AiPlanning_RebuildFactionWorkspaces(factionIndex,(WorldRuntimeContext *)worldRuntime);
                    
      switch(AVar5.preservedEcxPlanningPhase) {
      case 0:
        AiUnitBehavior_UpdateWorkspace01Entities
                  (AVar5.preservedEdxFactionIndex,(WorldRuntimeContext *)worldRuntime);
        AiUnitGroup_AssignCollectedEntitiesToBestTarget();
        break;
      case 1:
        AVar2 = AiFactionPlanning_UpdateActiveEntityPressureFlag(AVar5.preservedEdxFactionIndex);
        AVar2 = GameFactionRelations_UpdateAllPairsForFaction
                          (AVar2.preservedEdxFactionIndex,(WorldRuntimeContext *)worldRuntime);
        AiConstructionPlanner_ProcessPendingAssetRequests
                  (AVar2.preservedEdxFactionIndex,(WorldRuntimeContext *)worldRuntime);
        knowledgeData = g_AiKnowledgeData;
        if (!(bool)uVar3) {
          uVar3 = false;
          if (((*(int *)((int)(g_GameFactionRuntimeImage.records[0].candidateCache.savedEntries + 3)
                        + factionImageByteOffset + 4) == 0) ||
              (uVar1 = *(uint *)(g_GameFactionRuntimeImage.records[0].reserved78_87 +
                                (factionImageByteOffset - 0x20)), uVar3 = uVar1 < 0x32,
              0x31 < (int)uVar1)) ||
             (uVar1 = *(uint *)(g_GameFactionRuntimeImage.records[0].reserved78_87 +
                               (factionImageByteOffset - 0x14)), uVar3 = uVar1 < 0x32,
             0x31 < (int)uVar1)) {
            factionIndex_00 = (FactionRuntimeIndex)AiCandidateWorkspace_Clear();
            AVar2 = AiResourceCandidate_AddWeightedId136(factionIndex_00);
            AVar2 = AiWorkspaceAssetCandidate_AddWeightedEntry
                              ((knowledgeData->parameters).specialSite14aBaseWeight,
                               ARM_0330_BUILDING_MDL0303,AVar2.preservedEdxFactionIndex,
                               (WorldRuntimeContext *)worldRuntime);
            AVar2 = AiWorkspaceAssetCandidate_AddWeightedEntry
                              ((knowledgeData->parameters).specialSite14cBaseWeight,
                               ARM_0332_BUILDING_MDL0302,AVar2.preservedEdxFactionIndex,
                               (WorldRuntimeContext *)worldRuntime);
            AVar2 = AiStructureCandidate_AddWeightedId14BOr14CCandidate
                              ((knowledgeData->parameters).structure14bBaseWeight,
                               ARM_0331_BUILDING_MDL0308,AVar2.preservedEdxFactionIndex,
                               (WorldRuntimeContext *)worldRuntime);
            AVar2 = AiStructureCandidate_AddWeightedId14BOr14CCandidate
                              ((knowledgeData->parameters).structure14dBaseWeight,
                               ARM_0333_BUILDING_MDL0307,AVar2.preservedEdxFactionIndex,
                               (WorldRuntimeContext *)worldRuntime);
            factionIndex_01 =
                 (FactionRuntimeIndex)
                 AiCandidatePlanning_AddSpecialSiteCandidate
                           (AVar2.preservedEdxFactionIndex,(WorldRuntimeContext *)worldRuntime);
            AVar2 = AiArmyCandidate_AddBestScoredVariantA
                              (factionIndex_01,(WorldRuntimeContext *)worldRuntime);
            AVar2 = AiArmyCandidate_AddBestScoredVariantB
                              (AVar2.preservedEdxFactionIndex,(WorldRuntimeContext *)worldRuntime);
            AVar2 = AiArmyCandidate_AddBestScoredVariantC
                              (AVar2.preservedEdxFactionIndex,(WorldRuntimeContext *)worldRuntime);
            AVar2 = AiStrategicClass_AddCandidate12DOr12FTo132
                              (AVar2.preservedEdxFactionIndex,(WorldRuntimeContext *)worldRuntime);
            AVar2 = AiStrategicClass_AddWeightedClassCandidate
                              (AVar2.preservedEdxFactionIndex,(WorldRuntimeContext *)worldRuntime);
            AVar2 = AiStrategicCandidate_AddBestWorkspace12Entry
                              (AVar2.preservedEdxFactionIndex,(WorldRuntimeContext *)worldRuntime);
            AiPurchasePlanner_ExecuteAffordableCandidates(AVar2.preservedEdxFactionIndex);
            if ((bool)uVar3) {
              AiCandidateWorkspace_SaveToFactionImage(factionImageByteOffset);
              *(undefined4 *)
               ((int)(g_GameFactionRuntimeImage.records[0].candidateCache.savedEntries + 3) +
               factionImageByteOffset + 4) = 0x10;
            }
          }
          else {
            purchaseFactionIndexAfterCacheLoad =
                 (FactionRuntimeIndex)
                 AiCandidateWorkspace_LoadFromFactionImage(factionImageByteOffset);
            AiPurchasePlanner_ExecuteAffordableCandidates(purchaseFactionIndexAfterCacheLoad);
            if (!(bool)uVar3) {
              *(undefined4 *)
               ((int)(g_GameFactionRuntimeImage.records[0].candidateCache.savedEntries + 3) +
               factionImageByteOffset + 4) = 0;
            }
          }
        }
      }
    }
  }
  AVar4.preservedEcxRemainingFactionCount = in_ECX;
  AVar4.preservedEaxFactionIndex = in_EAX;
  return AVar4;
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
AiPreservedFactionIndexEdxResult
AiConstructionPlanner_ProcessPendingAssetRequests
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  ArmyAssetRecordPrefix *pAVar1;
  ModelDefinitionRecordPrefix *pMVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  int extraout_ECX_05;
  int iVar3;
  FactionRuntimeIndex in_EDX;
  PckArmyAssetIdCatalog armyAssetId;
  PckArmyAssetIdCatalog armyAssetId_00;
  PckArmyAssetIdCatalog extraout_EDX;
  AiRuntimeWorkspaceEntry *runtimeWorkspaceEntry;
  bool bVar4;
  
  g_AiConstructionPendingAssetConsumedCount = 0;
  runtimeWorkspaceEntry = g_AiWorkspaceBuffer04_Size0040;
  if (g_AiWorkspace04Count != 0) {
    do {
      armyAssetId_00 = runtimeWorkspaceEntry->armyAssetId;
      bVar4 = armyAssetId_00 < ARM_0300_BUILDING_MDL0301;
      if (armyAssetId_00 == ARM_0300_BUILDING_MDL0301) {
        AiPrimaryWorkspace_HasUnassignedEntryByIdCf(ARM_0330_BUILDING_MDL0303);
        iVar3 = extraout_ECX_05;
        armyAssetId_00 = extraout_EDX;
        if (!bVar4) {
AiConstructionPlanner_ProcessPendingAssetRequests_DispatchReachableCandidatePlacement:
          AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate
                    (armyAssetId_00,factionIndex,worldRuntime);
          iVar3 = extraout_ECX_00;
        }
      }
      else if ((armyAssetId_00 == ARM_0330_BUILDING_MDL0303) ||
              (armyAssetId_00 == ARM_0332_BUILDING_MDL0302)) {
        AiConstructionPlanner_PlaceSpecialAssetFromWorkspace
                  (armyAssetId_00,factionIndex,worldRuntime);
        iVar3 = extraout_ECX_02;
      }
      else {
        if (armyAssetId_00 == ARM_0331_BUILDING_MDL0308)
        goto AiConstructionPlanner_ProcessPendingAssetRequests_DispatchReachableCandidatePlacement;
        if (armyAssetId_00 == ARM_0333_BUILDING_MDL0307) {
          AiConstructionPlanner_PlaceDerivedAsset14D
                    (ARM_0333_BUILDING_MDL0307,factionIndex,worldRuntime);
          iVar3 = extraout_ECX_03;
        }
        else {
          bVar4 = armyAssetId_00 < ARM_0340_BUILDING_MDL0314;
          if (bVar4) {
            pAVar1 = ArmyAssetRegistry_FindByIdCf(armyAssetId_00);
            iVar3 = extraout_ECX;
            if (!bVar4) {
              pMVar2 = ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                                 (factionIndex,pAVar1->rootNodeOffsetOrPointer);
              armyAssetId_00 = armyAssetId;
              if (pMVar2[0x34].definitionId != 1)
              goto 
              AiConstructionPlanner_ProcessPendingAssetRequests_DispatchReachableCandidatePlacement;
              AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate
                        (armyAssetId,factionIndex,worldRuntime);
              iVar3 = extraout_ECX_04;
            }
          }
          else {
            AiConstructionPlanner_PlaceExtendedAssetNearFactionAnchor
                      (armyAssetId_00,factionIndex,worldRuntime);
            iVar3 = extraout_ECX_01;
          }
        }
      }
      if (g_AiConstructionPendingAssetConsumedCount != 0) {
        return (AiPreservedFactionIndexEdxResult)in_EDX;
      }
      runtimeWorkspaceEntry = runtimeWorkspaceEntry + 1;
    } while (iVar3 != 1);
  }
  return (AiPreservedFactionIndexEdxResult)in_EDX;
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
void AiPurchasePlanner_ExecuteAffordableCandidates(FactionRuntimeIndex factionIndex)

{
  int extraout_ECX;
  int extraout_ECX_00;
  int iVar1;
  FactionRuntimeIndex factionIndex_00;
  XeniteAmountQ4 XVar2;
  AiCandidateWorkspaceEntry *entry;
  bool bVar3;
  AiWorkspaceEntryValueEaxPreservedEdxCarrier64 AVar4;
  
  entry = g_AiWorkspaceBuffer13_Size0400;
  g_AiPurchaseAppliedArmyClassMask = 0;
  if (g_AiCandidateWorkspaceEntryCount != 0) {
    AiCandidateWorkspace_SortDescending();
    XVar2 = g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4;
    do {
      AVar4 = AiCandidateWorkspace_GetEntryEntityValue(entry);
      bVar3 = XVar2 < (uint)AVar4;
      XVar2 = XVar2 - (uint)AVar4;
      if (bVar3) {
        return;
      }
      AiPurchaseCandidate_HasEligibleProducerCf(entry,(FactionRuntimeIndex)(AVar4 >> 0x20));
      iVar1 = extraout_ECX;
      if (!bVar3) {
        AiPurchaseCandidate_ApplyToFaction(entry,factionIndex_00);
        iVar1 = extraout_ECX_00;
      }
      entry = entry + 1;
    } while (iVar1 != 1);
  }
  return;
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
void AiConstructionPlanner_PlaceDerivedAsset14D
               (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
               WorldRuntimeContext *worldRuntime)

{
  undefined4 *puVar1;
  ArmyRuntimeSlot *armyRuntime;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_EDX;
  int extraout_EDX_00;
  int extraout_EDX_01;
  int extraout_EDX_02;
  AiTerrainFeatureWorkspaceEntry *terrainFeatureEntry4;
  AiTerrainFeatureWorkspaceEntry *terrainFeatureEntry5;
  int iVar2;
  int iVar3;
  bool bVar4;
  undefined8 uVar5;
  AiWorkspace09AnchorCfRegisterResult AVar6;
  ModelRuntimeNode *modelNode;
  
  terrainFeatureEntry4 = g_AiWorkspaceBuffer08_Size0200;
  iVar2 = g_AiWorkspace08Count;
  if (g_AiWorkspace08Count != 0) {
    do {
      bVar4 = terrainFeatureEntry4->armyAssetId < ARM_0330_BUILDING_MDL0303;
      if ((terrainFeatureEntry4->armyAssetId == ARM_0330_BUILDING_MDL0303) &&
         (AiPlacement_ReserveAdditionalSpecialSite
                    (terrainFeatureEntry4->armyAssetId,terrainFeatureEntry4->cell,factionIndex,
                     worldRuntime), !bVar4)) {
        AVar6 = AiPlacement_FindNearestValidWorkspace09Anchor
                          (*(Q12 *)(extraout_EDX + 0x44),*(Q12 *)(extraout_EDX + 0x40),armyAssetId,
                           factionIndex,worldRuntime);
        if (!bVar4) {
          armyRuntime = ArmyRuntime_CreateInstanceFromAssetCf
                                  (4,0,AVar6.worldXQ12,AVar6.worldYQ12,factionIndex,armyAssetId,
                                   worldRuntime);
          if (bVar4) {
            return;
          }
          modelNode = armyRuntime->modelNodeRuntime;
          puVar1 = armyRuntime->definitionOrAsset;
          modelNode->tintArgb = 0;
          uVar5 = ModelNodeRuntime_RebuildTransformsFromRoot(modelNode,*puVar1,modelNode);
          ArmyRuntime_DispatchClassCommand((ArmyRuntimeSlot **)uVar5,worldRuntime);
          EffectRuntimePool_CreateInstanceFromDefinitionCf
                    (extraout_ECX,extraout_EDX_00,EFFECT_RUNTIME_COMPLETION_NONE,0,
                     *(AngleTurn32 *)(extraout_ECX + 0x14),*(AngleTurn32 *)(extraout_ECX + 0x10),
                     *(AngleTurn32 *)(extraout_ECX + 0xc),*(Q12 *)(extraout_ECX + 0x9c),
                     *(Q12 *)(extraout_ECX + 0x98),*(Q12 *)(extraout_ECX + 0x94),
                     *(EffectDefinition **)(extraout_EDX_00 + 400),worldRuntime);
          AiConstructionPlanner_ConsumeFactionPendingArmyAsset(armyAssetId,factionIndex);
          return;
        }
      }
      terrainFeatureEntry4 = terrainFeatureEntry4 + 1;
      iVar2 = iVar2 + -1;
      terrainFeatureEntry5 = g_AiWorkspaceBuffer08_Size0200;
      iVar3 = g_AiWorkspace08Count;
    } while (iVar2 != 0);
    do {
      bVar4 = terrainFeatureEntry5->armyAssetId < ARM_0332_BUILDING_MDL0302;
      if ((terrainFeatureEntry5->armyAssetId == ARM_0332_BUILDING_MDL0302) &&
         (AiPlacement_ReserveAdditionalSpecialSite
                    (terrainFeatureEntry5->armyAssetId,terrainFeatureEntry5->cell,factionIndex,
                     worldRuntime), !bVar4)) {
        AVar6 = AiPlacement_FindNearestValidWorkspace09Anchor
                          (*(Q12 *)(extraout_EDX_01 + 0x44),*(Q12 *)(extraout_EDX_01 + 0x40),
                           armyAssetId,factionIndex,worldRuntime);
        if (!bVar4) {
          armyRuntime = ArmyRuntime_CreateInstanceFromAssetCf
                                  (4,0,AVar6.worldXQ12,AVar6.worldYQ12,factionIndex,armyAssetId,
                                   worldRuntime);
          if (bVar4) {
            return;
          }
          modelNode = armyRuntime->modelNodeRuntime;
          puVar1 = armyRuntime->definitionOrAsset;
          modelNode->tintArgb = 0;
          uVar5 = ModelNodeRuntime_RebuildTransformsFromRoot(modelNode,*puVar1,modelNode);
          ArmyRuntime_DispatchClassCommand((ArmyRuntimeSlot **)uVar5,worldRuntime);
          EffectRuntimePool_CreateInstanceFromDefinitionCf
                    (extraout_ECX_00,extraout_EDX_02,EFFECT_RUNTIME_COMPLETION_NONE,0,
                     *(AngleTurn32 *)(extraout_ECX_00 + 0x14),
                     *(AngleTurn32 *)(extraout_ECX_00 + 0x10),
                     *(AngleTurn32 *)(extraout_ECX_00 + 0xc),*(Q12 *)(extraout_ECX_00 + 0x9c),
                     *(Q12 *)(extraout_ECX_00 + 0x98),*(Q12 *)(extraout_ECX_00 + 0x94),
                     *(EffectDefinition **)(extraout_EDX_02 + 400),worldRuntime);
          AiConstructionPlanner_ConsumeFactionPendingArmyAsset(armyAssetId,factionIndex);
          return;
        }
      }
      iVar3 = iVar3 + -1;
      terrainFeatureEntry5 = terrainFeatureEntry5 + 1;
    } while (iVar3 != 0);
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
AiPreservedFactionIndexEdxResult
AiArmyCandidate_AddBestScoredVariantA
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  AiCandidateScore32 AVar1;
  dword weightRange;
  int extraout_ECX;
  int extraout_ECX_00;
  int iVar2;
  uint uVar3;
  FactionRuntimeIndex in_EDX;
  int extraout_EDX;
  int extraout_EDX_00;
  int iVar4;
  ArmyAssetRecordPrefix *unaff_EBX;
  AiRuntimeWorkspaceEntry *runtimeWorkspaceEntry;
  ArmyAssetRecordPrefix **armyAssetRegistryCursor;
  bool bVar5;
  
  armyAssetRegistryCursor = g_AiWorkspaceBuffer11_Size1000;
  if (g_AiWorkspace11Count != 0) {
    bVar5 = false;
    AiFactionRuntime_TestPlanningCapacityExceededCf(4,factionIndex);
    if ((!bVar5) &&
       (Technology_IsUnlockedForFactionCf(TEC_001_ARMS_FACTORIES,factionIndex), iVar2 = extraout_ECX
       , iVar4 = extraout_EDX, !bVar5)) {
      do {
        if (((((ArmyAssetRuntimeSemanticView80 *)*armyAssetRegistryCursor)->flags14 & 1) != 0) &&
           (AVar1 = AiArmyCandidate_ComputeFactionWeightedScore
                              (&g_AiArmyCandidateScoreWeightsVariantA15,factionIndex,
                               (ArmyAssetRuntimeSemanticView80 *)*armyAssetRegistryCursor),
           iVar2 = extraout_ECX_00, iVar4 = extraout_EDX_00, extraout_EDX_00 < AVar1)) {
          unaff_EBX = *armyAssetRegistryCursor;
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
        if ((299 < unaff_EBX->registryId) || (g_AiWorkspace01Count < 0xb)) {
          AiCandidateWorkspace_AddOrAccumulateWeightedEntry(unaff_EBX->registryId,weightRange,1);
        }
      }
    }
  }
  return (AiPreservedFactionIndexEdxResult)in_EDX;
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
AiPreservedFactionIndexEdxResult
AiStrategicClass_AddCandidate12DOr12FTo132
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  uint weightRange;
  int extraout_ECX;
  FactionRuntimeIndex in_EDX;
  RuntimeToken unaff_EBX;
  undefined1 in_CF;
  AiKnowledgeDataImage *knowledgeData;
  
  knowledgeData = g_AiKnowledgeData;
  AiPrimaryWorkspace_HasEntryByIdCf(ARM_0330_BUILDING_MDL0303);
  if ((bool)in_CF) {
    Technology_IsUnlockedForFactionCf(TEC_001_ARMS_FACTORIES,factionIndex);
    if ((bool)in_CF) {
      AiPrimaryWorkspace_HasEntryByIdCf(ARM_0301_BUILDING_MDL0318);
      if (!(bool)in_CF) {
        AiFactionRuntime_TestPlanningCapacityExceededCf
                  ((knowledgeData->parameters).strategic12dAnd141To143AdditionalPlanningCapacity,
                   factionIndex);
        if (!(bool)in_CF) {
          AiCandidateWorkspace_AddOrAccumulateWeightedEntry
                    (0x12d,(knowledgeData->parameters).strategicClass12dBaseWeight,1);
        }
      }
    }
    else {
      AiFactionRuntime_TestPlanningCapacityExceededCf
                ((knowledgeData->parameters).strategic12fTo132AdditionalPlanningCapacity,
                 factionIndex);
      if (!(bool)in_CF) {
        AiStrategicClass_SelectBestCandidate12FTo132(factionIndex,worldRuntime);
        if (unaff_EBX != 0) {
          weightRange = (knowledgeData->parameters).strategicClass12fTo132BaseWeight;
          if (extraout_ECX != 0) {
            weightRange = weightRange / (uint)(extraout_ECX * 2);
          }
          AiCandidateWorkspace_AddOrAccumulateWeightedEntry(unaff_EBX,weightRange,1);
        }
      }
    }
  }
  return (AiPreservedFactionIndexEdxResult)in_EDX;
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
AiPreservedFactionIndexEdxResult
AiStrategicClass_AddWeightedClassCandidate
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  AiKnowledgeDataImage *pAVar1;
  uint weightRange;
  int extraout_ECX;
  FactionRuntimeIndex in_EDX;
  RuntimeToken unaff_EBX;
  undefined1 in_CF;
  
  pAVar1 = g_AiKnowledgeData;
  AiPrimaryWorkspace_HasEntryByIdCf(ARM_0330_BUILDING_MDL0303);
  if ((bool)in_CF) {
    AiFactionRuntime_TestPlanningCapacityExceededCf
              ((pAVar1->parameters).strategic12dAnd141To143AdditionalPlanningCapacity,factionIndex);
    if (!(bool)in_CF) {
      AiStrategicClass_SelectWeightedClass141To143(factionIndex,worldRuntime);
      if (unaff_EBX != 0) {
        weightRange = (pAVar1->parameters).strategicClass141To143BaseWeight;
        if (extraout_ECX != 0) {
          weightRange = weightRange / (uint)(extraout_ECX * 2);
        }
        AiCandidateWorkspace_AddOrAccumulateWeightedEntry(unaff_EBX,weightRange,1);
      }
    }
  }
  return (AiPreservedFactionIndexEdxResult)in_EDX;
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
void AiConstructionPlanner_PlaceExtendedAssetNearFactionAnchor
               (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
               WorldRuntimeContext *worldRuntime)

{
  uint uVar1;
  ModelRuntimeNode *modelNodeRuntime;
  undefined4 *puVar2;
  int iVar3;
  ArmyRuntimeSlot *armySlot1;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int iVar4;
  int extraout_ECX_03;
  int iVar5;
  int extraout_EDX;
  int extraout_EDX_00;
  uint extraout_EDX_01;
  int extraout_EDX_02;
  FieldGridCell **gridCellCursor;
  bool bVar6;
  undefined8 uVar7;
  FieldGridCell *local_24;
  uint local_20;
  FieldGridCell *gridCell2;
  
  if ((g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorCooldown != 0) &&
     (g_AiWorkspace09Count != 0)) {
    local_20 = 0x7fffffff;
    iVar4 = g_AiWorkspace09Count;
    gridCellCursor = g_AiWorkspaceBuffer09_Size1000;
    do {
      gridCell2 = *gridCellCursor;
      iVar3 = g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorYQ12 - gridCell2->worldX;
      if (iVar3 < 0) {
        iVar3 = -iVar3;
      }
      iVar5 = g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorXQ12 - gridCell2->worldY;
      if (iVar5 < 0) {
        iVar5 = -iVar5;
      }
      if ((iVar5 + iVar3 < (int)local_20) &&
         (iVar3 = AiPrimaryWorkspace_GetMinimumActiveManhattanDistanceToPoint
                            (gridCell2->worldY,gridCell2->worldX), iVar4 = extraout_ECX,
         0x1fff < iVar3)) {
        iVar3 = AiWorkspace02_GetMinimumManhattanDistanceToPoint
                          (gridCell2->worldY,gridCell2->worldX);
        if (iVar3 < 0x7fffffff) {
          iVar4 = extraout_ECX_00;
          if (iVar3 < 0x5001) {
            uVar1 = extraout_EDX + iVar3 * 4;
joined_r0x00539894:
            if ((int)uVar1 < (int)local_20) {
              bVar6 = uVar1 < local_20;
              ArmyPlacement_DispatchAssetAtFieldPoint
                        (1,0,(uint)(ushort)gridCell2->triangle0NormalAngles,gridCell2->worldY,
                         gridCell2->worldX,armyAssetId,factionIndex,(UiRootNode *)worldRuntime);
              iVar4 = extraout_ECX_02;
              if (!bVar6) {
                local_24 = gridCell2;
                local_20 = extraout_EDX_01;
              }
            }
          }
        }
        else {
          iVar3 = AiWorkspace03_GetMinimumManhattanDistanceToPoint
                            (gridCell2->worldY,gridCell2->worldX);
          iVar4 = extraout_ECX_01;
          if (iVar3 < 0x8001) {
            uVar1 = extraout_EDX_00 + iVar3 * 2;
            goto joined_r0x00539894;
          }
        }
      }
      gridCellCursor = gridCellCursor + 1;
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
    bVar6 = local_20 < 0x7fffffff;
    if (((int)local_20 < 0x7fffffff) &&
       (armySlot1 = ArmyRuntime_CreateInstanceFromAssetCf
                              (4,(uint)(ushort)local_24->triangle0NormalAngles,local_24->worldY,
                               local_24->worldX,factionIndex,armyAssetId,worldRuntime), !bVar6)) {
      modelNodeRuntime = armySlot1->modelNodeRuntime;
      puVar2 = armySlot1->definitionOrAsset;
      modelNodeRuntime->tintArgb = 0;
      uVar7 = ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime,*puVar2,modelNodeRuntime);
      ArmyRuntime_DispatchClassCommand((ArmyRuntimeSlot **)uVar7,worldRuntime);
      EffectRuntimePool_CreateInstanceFromDefinitionCf
                (extraout_ECX_03,extraout_EDX_02,EFFECT_RUNTIME_COMPLETION_NONE,0,
                 *(AngleTurn32 *)(extraout_ECX_03 + 0x14),*(AngleTurn32 *)(extraout_ECX_03 + 0x10),
                 *(AngleTurn32 *)(extraout_ECX_03 + 0xc),*(Q12 *)(extraout_ECX_03 + 0x9c),
                 *(Q12 *)(extraout_ECX_03 + 0x98),*(Q12 *)(extraout_ECX_03 + 0x94),
                 *(EffectDefinition **)(extraout_EDX_02 + 400),worldRuntime);
      AiConstructionPlanner_ConsumeFactionPendingArmyAsset(armyAssetId,factionIndex);
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
AiPreservedFactionIndexEdxResult
AiArmyCandidate_AddBestScoredVariantB
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  AiCandidateScore32 AVar1;
  dword weightRange;
  int extraout_ECX;
  int iVar2;
  FactionRuntimeIndex in_EDX;
  int iVar3;
  int extraout_EDX;
  ArmyAssetRecordPrefix *unaff_EBX;
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
        iVar2 = extraout_ECX;
        iVar3 = extraout_EDX;
        if (extraout_EDX < AVar1) {
          unaff_EBX = *armyAssetRegistryCursor;
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
      AiCandidateWorkspace_AddOrAccumulateWeightedEntry(unaff_EBX->registryId,weightRange,1);
    }
  }
  return (AiPreservedFactionIndexEdxResult)in_EDX;
}

/* Address: 0x0053A8D0.
   Ownership: gameplay/ai/planning.
   Purpose: Scores eligible workspace11 army candidates with coefficient profile C, selects the highest positive
   result, applies the profile-specific knowledge weight, and adds the selected registry ID to the candidate
   workspace.
   Local calls: AiArmyCandidate_ComputeFactionWeightedScore.
   Cross-module calls: AiCandidateWorkspace_AddOrAccumulateWeightedEntry [gameplay/ai/workspaces].
*/
AiPreservedFactionIndexEdxResult
AiArmyCandidate_AddBestScoredVariantC
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  AiCandidateScore32 AVar1;
  uint weightRange;
  int extraout_ECX;
  int iVar2;
  FactionRuntimeIndex in_EDX;
  int iVar3;
  int extraout_EDX;
  ArmyAssetRecordPrefix *unaff_EBX;
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
        iVar2 = extraout_ECX;
        iVar3 = extraout_EDX;
        if (extraout_EDX < AVar1) {
          unaff_EBX = *armyAssetRegistryCursor;
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
      AiCandidateWorkspace_AddOrAccumulateWeightedEntry(unaff_EBX->registryId,weightRange,1);
    }
  }
  return (AiPreservedFactionIndexEdxResult)in_EDX;
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
void AiPurchaseCandidate_HasEligibleProducerCf
               (AiCandidateWorkspaceEntry *candidateEntry,FactionRuntimeIndex factionIndex)

{
  int *piVar1;
  uint uVar2;
  uint uVar3;
  ArmyAssetRecordPrefix *pAVar4;
  int iVar5;
  uint uVar6;
  RuntimeToken technologyIndex;
  int extraout_EDX;
  byte *pbVar7;
  bool bVar8;
  
  uVar3 = candidateEntry->weightedScoreAndKind & 0xf;
  technologyIndex = candidateEntry->entityIdAndMultiplicity & 0xffff;
  bVar8 = uVar3 < 2;
  if (uVar3 == 2) {
    Technology_IsAvailableForFactionCf(technologyIndex,factionIndex);
    pbVar7 = g_AiWorkspaceBuffer00_Size0400;
    uVar3 = g_AiWorkspace00Count;
    if (bVar8) {
      for (; uVar3 != 0; uVar3 = uVar3 - 1) {
        piVar1 = *(int **)pbVar7;
        if ((piVar1 != (int *)0x0) && ((piVar1[0x3b] & 0x89U) == 0)) {
          iVar5 = 0x1c;
          do {
            if (extraout_EDX == *(int *)(*piVar1 + 0x1c4 + iVar5 * 4)) {
              return;
            }
            iVar5 = iVar5 + -1;
          } while (iVar5 != 0);
        }
        pbVar7 = pbVar7 + 8;
      }
    }
  }
  else {
    pAVar4 = ArmyAssetRegistry_FindByIdCf(technologyIndex);
    uVar3 = pAVar4[1].selectionDetailTemplateVariantIndex;
    if ((g_AiWorkspace00Count != 0) && ((g_AiPurchaseAppliedArmyClassMask & uVar3) == 0)) {
      uVar6 = g_AiWorkspace00Count;
      pbVar7 = g_AiWorkspaceBuffer00_Size0400;
      if ((uVar3 & 0x10) == 0) {
        if ((uVar3 & 8) == 0) {
          uVar2 = uVar3 & 0xee;
          while (uVar2 != 0) {
            piVar1 = *(int **)pbVar7;
            if (((piVar1 != (int *)0x0) && (*(int *)(*piVar1 + 0x4c) == 0xd)) &&
               (((piVar1[0x3b] & 0xc9U) == 0 &&
                (((*(uint *)(*piVar1 + 0xc4) & uVar3 & 0xee) != 0 && (piVar1[0x2e] == 0)))))) {
              return;
            }
            pbVar7 = pbVar7 + 8;
            uVar6 = uVar6 - 1;
            uVar2 = uVar6;
          }
        }
        else {
          do {
            piVar1 = *(int **)pbVar7;
            if (((piVar1 != (int *)0x0) && (*(int *)(*piVar1 + 0x4c) == 0x16)) &&
               (((piVar1[0x3b] & 0xc9U) == 0 && (piVar1[0x2b] == 0)))) {
              return;
            }
            pbVar7 = pbVar7 + 8;
            uVar6 = uVar6 - 1;
          } while (uVar6 != 0);
        }
      }
      else {
        do {
          piVar1 = *(int **)pbVar7;
          if ((((piVar1 != (int *)0x0) && (*(int *)(*piVar1 + 0x4c) == 0xb)) &&
              ((piVar1[0x3b] & 0xc9U) == 0)) && (piVar1[0x2e] == 0)) {
            return;
          }
          pbVar7 = pbVar7 + 8;
          uVar6 = uVar6 - 1;
        } while (uVar6 != 0);
      }
    }
  }
  return;
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
void AiPurchaseCandidate_ApplyToFaction
               (AiCandidateWorkspaceEntry *candidateEntry,FactionRuntimeIndex factionIndex)

{
  GameEntityRuntime *entity;
  int iVar1;
  byte *pbVar2;
  uint uVar3;
  ArmyAssetRecordPrefix *pAVar4;
  int iVar5;
  RuntimeToken technologyIndex;
  PckArmyAssetIdCatalog registryId;
  undefined1 uVar6;
  
  uVar3 = candidateEntry->weightedScoreAndKind & 0xf;
  technologyIndex = candidateEntry->entityIdAndMultiplicity & 0xffff;
  uVar6 = uVar3 < 2;
  iVar1 = g_AiWorkspace00Count;
  pbVar2 = g_AiWorkspaceBuffer00_Size0400;
  if (uVar3 == 2) {
    for (; iVar1 != 0; iVar1 = iVar1 + -1) {
      entity = *(GameEntityRuntime **)pbVar2;
      if ((entity != (GameEntityRuntime *)0x0) && (((entity->common).runtimeFlags & 0x89) == 0)) {
        iVar5 = 0x1c;
        do {
          if (technologyIndex ==
              *(RuntimeToken *)
               ((int)(entity->common).ownership.definitionOrClassRecord + iVar5 * 4 + 0x1c4)) {
            Technology_ApplyRecordToEntity(technologyIndex,entity);
            pbVar2[0] = 0;
            pbVar2[1] = 0;
            pbVar2[2] = 0;
            pbVar2[3] = 0;
            return;
          }
          iVar5 = iVar5 + -1;
        } while (iVar5 != 0);
      }
      pbVar2 = pbVar2 + 8;
    }
  }
  else {
    GameFactionRuntime_RegisterArmyAssetPointers(0xffffffff,1,technologyIndex,factionIndex);
    pAVar4 = ArmyAssetRegistry_FindByIdCf(registryId);
    if (!(bool)uVar6) {
      g_AiPurchaseAppliedArmyClassMask =
           g_AiPurchaseAppliedArmyClassMask | pAVar4[1].selectionDetailTemplateVariantIndex;
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
AiPreservedFactionIndexEdxResult
AiFactionPlanning_UpdateActiveEntityPressureFlag(FactionRuntimeIndex factionIndex)

{
  FactionRuntimeFlags *pFVar1;
  int iVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  FactionRuntimeIndex in_EDX;
  int iVar3;
  WorldRuntimeContext *contextArg;
  byte *pbVar4;
  AiRuntimeWorkspaceEntry *runtimeWorkspaceEntry;
  ArmyRuntimeSlot *armySlot1;
  
  iVar3 = 2;
  pbVar4 = g_AiWorkspaceBuffer00_Size0400;
  for (iVar2 = g_AiWorkspace00Count; iVar2 != 0; iVar2 = iVar2 + -1) {
    if ((*(int *)pbVar4 != 0) &&
       ((*(uint *)(pbVar4 + 4) < 0x154 ||
        ((*(uint *)(pbVar4 + 4) < 0x17c && (iVar3 = iVar3 + -1, iVar3 == 0))))))
    goto AiFactionPlanning_UpdateActiveEntityPressureFlag_SetPressureFlag;
    pbVar4 = pbVar4 + 8;
  }
  iVar3 = iVar3 + 1;
  iVar2 = g_AiWorkspace01Count;
  runtimeWorkspaceEntry = g_AiWorkspaceBuffer01_Size0200;
  while( true ) {
    if (iVar2 == 0) {
      if ((g_GameFactionRuntimeImage.records[factionIndex].runtimeFlags & 1) != 0) {
        contextArg = &g_InGameRuntimeRoot->worldRuntime0A30;
        pbVar4 = g_AiWorkspaceBuffer00_Size0400;
        for (iVar3 = g_AiWorkspace00Count; iVar2 = g_AiWorkspace01Count,
            runtimeWorkspaceEntry = g_AiWorkspaceBuffer01_Size0200, iVar3 != 0; iVar3 = iVar3 + -1)
        {
          if (*(int *)pbVar4 != 0) {
            ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive
                      (contextArg,*(int **)(*(int *)pbVar4 + 8));
            iVar3 = extraout_ECX;
          }
          pbVar4 = pbVar4 + 8;
        }
        for (; iVar2 != 0; iVar2 = iVar2 + -1) {
          if (runtimeWorkspaceEntry->armyRuntime != (ArmyRuntimeSlot *)0x0) {
            ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive
                      (contextArg,(int *)runtimeWorkspaceEntry->armyRuntime->linkedEntityRuntime);
            iVar2 = extraout_ECX_00;
          }
          runtimeWorkspaceEntry = runtimeWorkspaceEntry + 1;
        }
      }
      return (AiPreservedFactionIndexEdxResult)in_EDX;
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
  return (AiPreservedFactionIndexEdxResult)in_EDX;
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
AiPreservedFactionIndexEdxResult
AiStructureCandidate_AddWeightedId14BOr14CCandidate
          (AiCandidateScore32 baseWeight,PckArmyAssetIdCatalog candidateArmyAssetId,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  uint uVar1;
  uint extraout_ECX;
  int extraout_ECX_00;
  uint extraout_ECX_01;
  FactionRuntimeIndex in_EDX;
  dword dVar2;
  int iVar3;
  AiTerrainFeatureWorkspaceEntry *terrainFeatureEntry;
  bool bVar4;
  AiWorkspaceCountEaxPreservedEdxCarrier64 AVar5;
  AiKnowledgeDataImage *knowledgeData;
  
  iVar3 = (int)((longlong)factionIndex * 0x740);
  bVar4 = (longlong)iVar3 != (longlong)factionIndex * 0x740;
  AiPrimaryWorkspace_HasUnassignedEntryByIdCf(candidateArmyAssetId);
  knowledgeData = g_AiKnowledgeData;
  if (!bVar4) {
    bVar4 = candidateArmyAssetId < ARM_0331_BUILDING_MDL0308;
    if (candidateArmyAssetId == ARM_0331_BUILDING_MDL0308) {
      AiPrimaryWorkspace_HasEntryByIdCf(ARM_0330_BUILDING_MDL0303);
      if (((bVar4) && (extraout_ECX_01 != 0)) &&
         (extraout_ECX_01 < (knowledgeData->parameters).structure14bPrerequisite14aCountLimit)) {
        uVar1 = *(uint *)(g_GameFactionRuntimeImage.records[0].reserved78_87 + iVar3 + -0x78);
        if (extraout_ECX_01 == uVar1) {
          baseWeight = baseWeight << 2;
        }
        if (extraout_ECX_01 - uVar1 < (knowledgeData->parameters).structure14bCountGapLimit) {
          AiCandidateWorkspace_AddOrAccumulateWeightedEntry(0x14b,baseWeight,1);
        }
      }
    }
    else {
      AiPrimaryWorkspace_HasEntryByIdCf(ARM_0332_BUILDING_MDL0302);
      if (((bVar4) && (extraout_ECX != 0)) &&
         ((extraout_ECX < (knowledgeData->parameters).structure14dPrerequisite14cCountLimit &&
          (extraout_ECX -
           *(int *)(g_GameFactionRuntimeImage.records[0].reserved78_87 + iVar3 + -0x68) <
           (knowledgeData->parameters).structure14dCountGapLimit)))) {
        AiCandidateWorkspace_AddOrAccumulateWeightedEntry(candidateArmyAssetId,baseWeight,1);
      }
      bVar4 = false;
      terrainFeatureEntry = g_AiWorkspaceBuffer08_Size0200;
      if (g_AiWorkspace08Count != 0) {
        do {
          AiPlacement_ReserveAdditionalSpecialSite
                    (terrainFeatureEntry->armyAssetId,terrainFeatureEntry->cell,factionIndex,
                     worldRuntime);
          if (!bVar4) {
            dVar2 = (knowledgeData->parameters).workspace08Id14aDerivedWeight;
            if (terrainFeatureEntry->armyAssetId != ARM_0330_BUILDING_MDL0303) {
              dVar2 = (knowledgeData->parameters).workspace08OtherDerivedWeight;
            }
            AVar5 = AiPrimaryWorkspace_CountAssignedEntriesById(terrainFeatureEntry->armyAssetId);
            AiCandidateWorkspace_AddOrAccumulateWeightedEntry
                      (candidateArmyAssetId,(dVar2 * 3) / ((int)AVar5 + 3U),1);
            return (AiPreservedFactionIndexEdxResult)in_EDX;
          }
          bVar4 = (AiTerrainFeatureWorkspaceEntry *)0xffffffef < terrainFeatureEntry;
          terrainFeatureEntry = terrainFeatureEntry + 1;
        } while (extraout_ECX_00 != 1);
      }
    }
  }
  return (AiPreservedFactionIndexEdxResult)in_EDX;
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
AiPreservedFactionIndexEdxResult
AiResourceCandidate_AddWeightedId136(FactionRuntimeIndex factionIndex)

{
  int iVar1;
  FactionRuntimeIndex in_EDX;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined1 in_CF;
  bool bVar5;
  
  AiPrimaryWorkspace_HasEntryByIdCf(ARM_0330_BUILDING_MDL0303);
  if ((bool)in_CF) {
    iVar4 = (int)((longlong)factionIndex * 0x740);
    bVar5 = (longlong)iVar4 != (longlong)factionIndex * 0x740;
    AiPrimaryWorkspace_HasUnassignedEntryByIdCf(ARM_0310_BUILDING_MDL0305);
    if (!bVar5) {
      iVar3 = *(int *)(g_GameFactionRuntimeImage.records[0].reserved78_87 + iVar4 + -0x54) >> 4;
      iVar2 = *(int *)(g_GameFactionRuntimeImage.records[0].reserved78_87 + iVar4 + -0x50) +
              *(int *)(g_GameFactionRuntimeImage.records[0].reserved78_87 + iVar4 + -0x4c) >> 4;
      if (iVar2 == 0) {
        iVar2 = 1;
      }
      iVar4 = (*(int *)(g_GameFactionRuntimeImage.records[0].reserved78_87 + iVar4 + -0x58) >> 4) +
              *(int *)(g_GameFactionRuntimeImage.records[0].reserved78_87 + iVar4 + -0x60);
      iVar1 = iVar4 - iVar3;
      if (iVar1 != 0 && iVar3 <= iVar4) {
        AiCandidateWorkspace_AddOrAccumulateWeightedEntry
                  (0x136,(dword)(((longlong)
                                  (int)(((longlong)iVar1 * (longlong)iVar2) / (longlong)iVar3) *
                                 (longlong)
                                 (int)(g_AiKnowledgeData->parameters).
                                      resource136DeficitScoreNumerator) /
                                (longlong)
                                (int)(g_AiKnowledgeData->parameters).
                                     resource136DeficitScoreDenominator),1);
      }
    }
  }
  return (AiPreservedFactionIndexEdxResult)in_EDX;
}

/* Address: 0x0053A9D0.
   Ownership: gameplay/ai/planning.
   Purpose: Evaluates strategic class IDs 0x12F through 0x132 using faction category totals, class-specific
   coefficient tables, current workspace presence, and randomized tie variation. The best available class and
   comparison state are returned through the engine register convention.
   Cross-module calls: AiPrimaryWorkspace_HasEntryByIdCf [gameplay/ai/workspaces],
   ArmyAssetRegistry_FindEnabledByIdCf [assets/army/catalog].
*/
void AiStrategicClass_SelectBestCandidate12FTo132
               (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  GridScratchStateMask GVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  GridScratchCell *pGVar9;
  uint uVar10;
  undefined1 uVar11;
  uint randomizedTieBits;
  uint bestCandidateScore;
  
  uVar5 = g_GridScratchWidth * g_GridScratchHeight;
  uVar8 = 0;
  uVar7 = 0;
  uVar10 = 0;
  uVar6 = uVar5;
  pGVar9 = g_GridScratchPrimary;
  do {
    GVar1 = pGVar9->stateMask;
    if ((GVar1 & GRID_SCRATCH_TERRAIN_CLASS_BIT24) == 0) {
      uVar8 = uVar8 + 1;
    }
    if ((GVar1 & (GRID_SCRATCH_TERRAIN_CLASS_BIT27|GRID_SCRATCH_TERRAIN_CLASS_BIT26|
                 GRID_SCRATCH_TERRAIN_CLASS_BIT25)) == 0) {
      uVar7 = uVar7 + 1;
    }
    if ((GVar1 & (GRID_SCRATCH_TERRAIN_CLASS_BIT30|GRID_SCRATCH_TERRAIN_CLASS_BIT29|
                 GRID_SCRATCH_TERRAIN_CLASS_BIT28)) == 0) {
      uVar10 = uVar10 + 1;
    }
    pGVar9 = pGVar9 + 1;
    uVar6 = uVar6 - 1;
  } while (uVar6 != 0);
  iVar2 = (int)(((ulonglong)uVar7 * 100) / (ulonglong)uVar5);
  iVar3 = (int)(((ulonglong)uVar8 * 100) / (ulonglong)uVar5);
  uVar6 = (uint)((ulonglong)uVar10 * 100 >> 0x20);
  uVar11 = uVar6 != 0;
  iVar4 = (int)(((ulonglong)uVar6 << 0x20 | (ulonglong)uVar10 * 100 & 0xffffffff) / (ulonglong)uVar5
               );
  randomizedTieBits = (*g_RandomGeneratorState.next)();
  bestCandidateScore = 0;
  AiPrimaryWorkspace_HasEntryByIdCf(ARM_0302_BUILDING_MDL0300);
  if ((!(bool)uVar11) &&
     (ArmyAssetRegistry_FindEnabledByIdCf(ARM_0302_BUILDING_MDL0300), !(bool)uVar11)) {
    bestCandidateScore =
         iVar4 * g_AiStrategicClass12FWeightComponent0 +
         iVar2 * g_AiStrategicClass12FWeightComponent1 +
         iVar3 * g_AiStrategicClass12FWeightComponent2;
    uVar6 = randomizedTieBits & 0x3fff;
    randomizedTieBits = randomizedTieBits >> 5;
    uVar11 = CARRY4(bestCandidateScore,uVar6);
    bestCandidateScore = bestCandidateScore + uVar6;
  }
  AiPrimaryWorkspace_HasEntryByIdCf(ARM_0303_BUILDING_MDL0316);
  if ((!(bool)uVar11) &&
     (ArmyAssetRegistry_FindEnabledByIdCf(ARM_0303_BUILDING_MDL0316), !(bool)uVar11)) {
    uVar6 = randomizedTieBits & 0x3fff;
    randomizedTieBits = randomizedTieBits >> 5;
    uVar6 = iVar4 * g_AiStrategicClass130WeightComponent0 +
            iVar2 * g_AiStrategicClass130WeightComponent1 +
            iVar3 * g_AiStrategicClass130WeightComponent2 + uVar6;
    uVar11 = uVar6 < bestCandidateScore;
    if ((int)bestCandidateScore < (int)uVar6) {
      bestCandidateScore = uVar6;
    }
  }
  AiPrimaryWorkspace_HasEntryByIdCf(ARM_0304_BUILDING_MDL0324);
  if ((!(bool)uVar11) &&
     (ArmyAssetRegistry_FindEnabledByIdCf(ARM_0304_BUILDING_MDL0324), !(bool)uVar11)) {
    uVar6 = randomizedTieBits & 0x3fff;
    randomizedTieBits = randomizedTieBits >> 5;
    uVar6 = iVar4 * g_AiStrategicClass131WeightComponent0 +
            iVar2 * g_AiStrategicClass131WeightComponent1 +
            iVar3 * g_AiStrategicClass131WeightComponent2 + uVar6;
    uVar11 = uVar6 < bestCandidateScore;
    if ((int)bestCandidateScore < (int)uVar6) {
      bestCandidateScore = uVar6;
    }
  }
  AiPrimaryWorkspace_HasEntryByIdCf(ARM_0305_BUILDING_MDL0317);
  if ((!(bool)uVar11) &&
     (ArmyAssetRegistry_FindEnabledByIdCf(ARM_0305_BUILDING_MDL0317), !(bool)uVar11)) {
    uVar11 = iVar4 * g_AiStrategicClass132WeightComponent0 +
             iVar2 * g_AiStrategicClass132WeightComponent1 +
             iVar3 * g_AiStrategicClass132WeightComponent2 + (randomizedTieBits & 0x3fff) <
             bestCandidateScore;
  }
  AiPrimaryWorkspace_HasEntryByIdCf(ARM_0306_BUILDING_MDL0310);
  if (!(bool)uVar11) {
    ArmyAssetRegistry_FindEnabledByIdCf(ARM_0306_BUILDING_MDL0310);
  }
  return;
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
void AiStrategicClass_SelectWeightedClass141To143
               (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  int iVar1;
  int iVar2;
  int iVar3;
  dword dVar4;
  dword dVar5;
  dword dVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  dword dVar10;
  uint extraout_EDX;
  uint uVar11;
  undefined1 uVar12;
  
  iVar1 = g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[2];
  iVar2 = g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[3];
  iVar3 = g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[4];
  dVar4 = (g_AiKnowledgeData->parameters).strategicClass141Weight;
  dVar5 = (g_AiKnowledgeData->parameters).strategicClass142Weight;
  dVar6 = (g_AiKnowledgeData->parameters).strategicClass143Weight;
  iVar7 = g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[2];
  iVar8 = g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[3];
  iVar9 = g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[4];
  dVar10 = (*g_RandomGeneratorState.next)();
  uVar11 = 0;
  uVar12 = false;
  AiPrimaryWorkspace_HasEntryByIdCf(ARM_0321_BUILDING_MDL0326);
  if (((!(bool)uVar12) &&
      (ArmyAssetRegistry_FindEnabledByIdCf(ARM_0321_BUILDING_MDL0326), !(bool)uVar12)) &&
     (uVar12 = 0, extraout_EDX != 0)) {
    uVar11 = extraout_EDX;
  }
  AiPrimaryWorkspace_HasEntryByIdCf(ARM_0322_BUILDING_MDL0327);
  if ((!(bool)uVar12) &&
     (ArmyAssetRegistry_FindEnabledByIdCf(ARM_0322_BUILDING_MDL0327), !(bool)uVar12)) {
    uVar12 = ((iVar1 + 1) * dVar4 + (iVar2 + 1) * dVar5 + (iVar3 + 1) * dVar6) /
             (iVar7 + iVar8 + iVar9 + 1U) + (dVar10 >> 0x13 & 0x7f) < uVar11;
  }
  AiPrimaryWorkspace_HasEntryByIdCf(ARM_0323_BUILDING_MDL0328);
  if (!(bool)uVar12) {
    ArmyAssetRegistry_FindEnabledByIdCf(ARM_0323_BUILDING_MDL0328);
  }
  return;
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
void AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate
               (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
               WorldRuntimeContext *worldRuntime)

{
  dword radiusMetric;
  ModelRuntimeNode *modelNodeRuntime;
  undefined4 *puVar1;
  ArmyAssetRecordPrefix *pAVar2;
  ModelDefinitionRecordPrefix *pMVar3;
  dword dVar4;
  ArmyRuntimeSlot *armySlot1;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_EDX;
  uint uVar5;
  int extraout_EDX_00;
  int iVar6;
  FieldGridCell **gridCellCursor;
  bool bVar7;
  undefined8 uVar8;
  FieldGridCell *local_2c;
  uint local_24;
  FieldGridCell *gridCell1;
  
  iVar6 = (int)((longlong)factionIndex * 0x740);
  bVar7 = (longlong)iVar6 != (longlong)factionIndex * 0x740;
  pAVar2 = ArmyAssetRegistry_FindByIdCf(armyAssetId);
  if ((((!bVar7) &&
       (bVar7 = false,
       *(int *)(g_GameFactionRuntimeImage.records[0].reserved78_87 + iVar6 + -0x20) == 0)) &&
      (pMVar3 = ModelDefinitionRegistry_FindByIdWithErrorCf
                          (*(PckModelDefinitionIdCatalog *)(pAVar2->rootNodeOffsetOrPointer + 0x20))
      , !bVar7)) && (radiusMetric = pMVar3[0x12].flags, g_AiWorkspace10Count != 0)) {
    local_24 = 0x7fffffff;
    gridCellCursor = g_AiWorkspaceBuffer10_Size0400;
    do {
      gridCell1 = *gridCellCursor;
      dVar4 = (*g_RandomGeneratorState.next)();
      uVar5 = extraout_EDX + (dVar4 & 0xffff);
      bVar7 = uVar5 < local_24;
      iVar6 = extraout_ECX;
      if (((int)uVar5 < (int)local_24) &&
         (ArmyPlacement_DispatchAssetAtFieldPoint
                    (1,0,(uint)(ushort)gridCell1->triangle0NormalAngles,gridCell1->worldY,
                     gridCell1->worldX,armyAssetId,factionIndex,(UiRootNode *)worldRuntime),
         iVar6 = extraout_ECX_00, !bVar7)) {
        uVar8 = GridReachability_RebuildConnectedRegionAroundWorldPoint
                          (radiusMetric,gridCell1->worldY,gridCell1->worldX);
        iVar6 = extraout_ECX_01;
        if (!bVar7) {
          local_2c = gridCell1;
          local_24 = (uint)((ulonglong)uVar8 >> 0x20);
        }
      }
      gridCellCursor = gridCellCursor + 1;
    } while (iVar6 != 1);
    bVar7 = local_24 < 0x7fffffff;
    if (((int)local_24 < 0x7fffffff) &&
       (armySlot1 = ArmyRuntime_CreateInstanceFromAssetCf
                              (4,(uint)(ushort)local_2c->triangle0NormalAngles,local_2c->worldY,
                               local_2c->worldX,factionIndex,armyAssetId,worldRuntime), !bVar7)) {
      modelNodeRuntime = armySlot1->modelNodeRuntime;
      puVar1 = armySlot1->definitionOrAsset;
      modelNodeRuntime->tintArgb = 0;
      uVar8 = ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime,*puVar1,modelNodeRuntime);
      ArmyRuntime_DispatchClassCommand((ArmyRuntimeSlot **)uVar8,worldRuntime);
      EffectRuntimePool_CreateInstanceFromDefinitionCf
                (extraout_ECX_02,extraout_EDX_00,EFFECT_RUNTIME_COMPLETION_NONE,0,
                 *(AngleTurn32 *)(extraout_ECX_02 + 0x14),*(AngleTurn32 *)(extraout_ECX_02 + 0x10),
                 *(AngleTurn32 *)(extraout_ECX_02 + 0xc),*(Q12 *)(extraout_ECX_02 + 0x9c),
                 *(Q12 *)(extraout_ECX_02 + 0x98),*(Q12 *)(extraout_ECX_02 + 0x94),
                 *(EffectDefinition **)(extraout_EDX_00 + 400),worldRuntime);
      AiConstructionPlanner_ConsumeFactionPendingArmyAsset(armyAssetId,factionIndex);
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
void AiConstructionPlanner_ConsumeFactionPendingArmyAsset
               (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex)

{
  FactionRelationCounter *pFVar1;
  FactionArmyAssetCount *pFVar2;
  ArmyAssetRecordPrefix *pAVar3;
  int extraout_ECX;
  int iVar4;
  dword *pdVar5;
  
  g_AiConstructionPendingAssetConsumedCount = g_AiConstructionPendingAssetConsumedCount + 1;
  pAVar3 = ArmyAssetRegistry_FindByIdCf(armyAssetId);
  pdVar5 = g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetPointersOrIds;
  pFVar1 = &g_GameFactionRuntimeImage.records[factionIndex].relationCounterB;
  *pFVar1 = *pFVar1 + 1;
  iVar4 = extraout_ECX;
  while( true ) {
    if (iVar4 == 0) {
      return;
    }
    if (pAVar3 == (ArmyAssetRecordPrefix *)*pdVar5) break;
    pdVar5 = pdVar5 + 1;
    iVar4 = iVar4 + -1;
  }
  pFVar2 = &g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
  *pFVar2 = *pFVar2 - 1;
  while (iVar4 = iVar4 + -1, iVar4 != 0) {
    *pdVar5 = pdVar5[1];
    pdVar5 = pdVar5 + 1;
  }
  return;
}

/* Address: 0x0053A980.
   Ownership: gameplay/ai/planning.
   Purpose: Handles ai faction runtime test planning capacity exceeded carry-flag result.
*/
void AiFactionRuntime_TestPlanningCapacityExceededCf
               (dword additionalPlanningCapacity,FactionRuntimeIndex factionIndex)

{
  return;
}

/* Address: 0x0053A2A0.
   Ownership: gameplay/ai/planning.
   Purpose: Computes a composite AI score for one army candidate. Stock tech.tec has 512 records over canonical ids
   0..255; localized titles do not prove source-building, tier, direction, or effect mappings. Typed parameters: p4
   armyAssetRecord→AiArmyAssetRecordAddress32_V345. Calling convention, complete VariableStorage serialization,
   function bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf [assets/model/definitions].
*/
AiCandidateScore32
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
  uint extraout_ECX;
  int extraout_EDX;
  int extraout_EDX_00;
  int weightedDefinitionScore;
  undefined1 in_CF;
  bool bVar1;
  AiLinkedDefinitionListView *linkedDefinitionList;
  int secondChildScaleDivisor30;
  ShotDefinition *selectedShotDefinition;
  ShotDefinition *secondChildShotDefinition;
  int definitionScaleDivisor30;
  
  linkedDefinitionList = (AiLinkedDefinitionListView *)armyAssetRecord->rootNodeOffsetOrPointer;
  selectedModelDefinition =
       (ModelDefinitionResolvePhaseView280 *)
       ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                 (factionIndex,(ModelLinkedDefinitionListAddress32)linkedDefinitionList);
  if ((bool)in_CF) {
    return 0;
  }
  weightedDefinitionScore = scoreWeights->baseScore;
  if (selectedModelDefinition->runtimeValue18 != 0) {
    weightedDefinitionScore = weightedDefinitionScore + scoreWeights->nonzeroDefinition18Bonus;
  }
  weightedDefinitionScore =
       ((int)(((longlong)(int)selectedModelDefinition->runtimeValue0C *
              (longlong)scoreWeights->definitionValue0CWeight) /
             (longlong)g_AiArmyCandidateFlaggedDefinitionValueMaximum) +
       weightedDefinitionScore +
       (int)(((longlong)(int)selectedModelDefinition->runtimeValue60 *
             (longlong)scoreWeights->definitionValue60Weight) /
            (longlong)
            (&g_TechnologyCategoryMaximum0)[selectedModelDefinition->categoryMaximumIndex5C])) * 8;
  bVar1 = false;
  if (linkedDefinitionList->childListCount != 0) {
    selectedChildModelDefinition0 =
         (ModelDefinitionResolvePhaseView280 *)
         ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                   (factionIndex,linkedDefinitionList->childList0Address);
    if (bVar1) {
      return 0;
    }
    weightedDefinitionScore = extraout_EDX;
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
             extraout_EDX +
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
    bVar1 = extraout_ECX == 0;
    if (1 < extraout_ECX) {
      selectedChildModelDefinition1 =
           (ModelDefinitionResolvePhaseView280 *)
           ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                     (factionIndex,linkedDefinitionList->childList1Address);
      if (bVar1) {
        return 0;
      }
      weightedDefinitionScore = extraout_EDX_00;
      if (selectedChildModelDefinition1->runtimeValue30 != 0) {
        secondChildShotDefinition = selectedChildModelDefinition1->shotDefinitionReference2C;
        secondChildScaleDivisor30 = selectedChildModelDefinition1->runtimeValue30;
        if (g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure != 0) {
          pressureWeightedDamage0 =
               (uint)(((longlong)
                       (int)(((longlong)secondChildShotDefinition->targetClassImpactDamageQ12[0] *
                             (longlong)scoreWeights->pressureCategoryWeights[0]) /
                            (longlong)secondChildScaleDivisor30) *
                      (longlong)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[0])
                     / (longlong)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
          pressureWeightedDamage1 =
               (uint)(((longlong)
                       (int)(((longlong)secondChildShotDefinition->targetClassImpactDamageQ12[1] *
                             (longlong)scoreWeights->pressureCategoryWeights[1]) /
                            (longlong)secondChildScaleDivisor30) *
                      (longlong)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[1])
                     / (longlong)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
          pressureWeightedDamage2 =
               (uint)(((longlong)
                       (int)(((longlong)secondChildShotDefinition->targetClassImpactDamageQ12[2] *
                             (longlong)scoreWeights->pressureCategoryWeights[2]) /
                            (longlong)secondChildScaleDivisor30) *
                      (longlong)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[2])
                     / (longlong)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
          pressureWeightedDamage3 =
               (uint)(((longlong)
                       (int)(((longlong)secondChildShotDefinition->targetClassImpactDamageQ12[3] *
                             (longlong)scoreWeights->pressureCategoryWeights[3]) /
                            (longlong)secondChildScaleDivisor30) *
                      (longlong)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[3])
                     / (longlong)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
          pressureWeightedDamage4 =
               (uint)(((longlong)
                       (int)(((longlong)secondChildShotDefinition->targetClassImpactDamageQ12[4] *
                             (longlong)scoreWeights->pressureCategoryWeights[4]) /
                            (longlong)secondChildScaleDivisor30) *
                      (longlong)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[4])
                     / (longlong)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
          pressureWeightedDamage5 =
               (uint)(((longlong)
                       (int)(((longlong)secondChildShotDefinition->targetClassImpactDamageQ12[5] *
                             (longlong)scoreWeights->pressureCategoryWeights[5]) /
                            (longlong)secondChildScaleDivisor30) *
                      (longlong)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[5])
                     / (longlong)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
          pressureWeightedDamage6 =
               (uint)(((longlong)
                       (int)(((longlong)secondChildShotDefinition->targetClassImpactDamageQ12[6] *
                             (longlong)scoreWeights->pressureCategoryWeights[6]) /
                            (longlong)secondChildScaleDivisor30) *
                      (longlong)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[6])
                     / (longlong)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
          pressureWeightedDamage7 =
               (uint)(((longlong)
                       (int)(((longlong)secondChildShotDefinition->targetClassImpactDamageQ12[7] *
                             (longlong)scoreWeights->pressureCategoryWeights[7]) /
                            (longlong)secondChildScaleDivisor30) *
                      (longlong)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[7])
                     / (longlong)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
          weightedDefinitionScore =
               extraout_EDX_00 +
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
    }
  }
  return scoreWeights->armyRecord74Weight * armyAssetRecord->definitionClassValue74 +
         weightedDefinitionScore * 0xc +
         scoreWeights->armyRecord78Weight * armyAssetRecord->definitionClassValue78 +
         scoreWeights->armyRecord70Weight * armyAssetRecord->definitionClassValue70;
}
