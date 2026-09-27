/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/ai/planning.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/ai/planning.h>
#include <thandor/thandor.h>

/* Implementation ownership: gameplay/ai/planning. */

/* Address: 0x00514350.
   Runs the planning phase for every active AI faction (1..7, a faction without a player block) and scales its
   terrain contribution by the game speed, then rebuilds the per-faction AI pressure table: each of the eight
   pressure channels decays to about 3/4, every runtime model adds 0x100 to the channel of its definition
   (+0x5C, stock data ~0) for each other faction flagged in its faction mask, and the channel maximum is stored.
   Called on tick-wheel cases 2 and 6.
*/
void __fastcall AiFactionRuntime_RebuildPlanningCapacityState(void)

{
  GameSpeedQ8 currentGameSpeedQ8;
  int factionIndexOrScratch;
  int remainingFactionsOrPressureValue;
  FrontendPlayerRuntimeBlockCount remainingPlayerBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  uint32_t nextChannelOrFactionBit;
  uint32_t channelOrFactionIndex;
  GameFactionRuntimeRecord *pressureTargetRecord;
  GameFactionRuntimeRecord *planningRecord;
  GameFactionRuntimeRecord *decayRecord;
  GameFactionRuntimeRecord *maximumScanRecord;
  FactionRuntimeLifecycleObservedState *lifecycleState;
  WorldOwnerListNode100 *ownerNode;
  
  planningRecord = g_GameFactionRuntimeImage.records;
  lifecycleState = g_GameFactionRuntimeImage.tail.factionLifecycleStates;
  remainingFactionsOrPressureValue = 7;
  factionIndexOrScratch = 1;
  do {
    remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount;
    playerBlock = g_FrontendPlayerRuntimeBlocks;
    lifecycleState++;
    planningRecord++;
    planningRecord->terrainContributionScaleQ8 = 0x100; /* 1.0 in Q8 */
    currentGameSpeedQ8 = g_GameFactionRuntimeImage.tail.gameSpeedQ8;
    if (*lifecycleState == FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
      /* Only factions without a player block are AI-controlled. */
      do {
        if (factionIndexOrScratch == (playerBlock->factionAssignment).factionAssignmentIndex) break;
        playerBlock++;
        remainingPlayerBlocks--;
      } while (remainingPlayerBlocks != 0);
      if (remainingPlayerBlocks == 0) {
        AiRuntime_DispatchFactionPlanningPhase(factionIndexOrScratch,(WorldRuntimeContext *)g_InGameRuntimeRoot);
        planningRecord->terrainContributionScaleQ8 = currentGameSpeedQ8;
      }
    }
    factionIndexOrScratch++;
    remainingFactionsOrPressureValue--;
    if (remainingFactionsOrPressureValue == 0) {
      /* Decay: value = ((value * 3 + 1) >> 2) + 1, two channels per step. */
      decayRecord = g_GameFactionRuntimeImage.records + 1;
      factionIndexOrScratch = 7;
      channelOrFactionIndex = 0;
      do {
        do {
          nextChannelOrFactionBit = channelOrFactionIndex + 2;
          decayRecord->aiPressureValues[channelOrFactionIndex] =
               ((decayRecord->aiPressureValues[channelOrFactionIndex] * 3 + 1U) >> 2) + 1;
          decayRecord->aiPressureValues[channelOrFactionIndex + 1] =
               ((decayRecord->aiPressureValues[channelOrFactionIndex + 1] * 3 + 1U) >> 2) + 1;
          channelOrFactionIndex = nextChannelOrFactionBit;
        } while (nextChannelOrFactionBit < 8);
        decayRecord->maximumAiPressure = 0;
        channelOrFactionIndex = 0;
        decayRecord++;
        factionIndexOrScratch--;
      } while (factionIndexOrScratch != 0);
      ownerNode = (g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
      if (ownerNode != NULL) {
        do {
          if (ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
            /* runtimePayload is the ModelRuntimeSlot: +0 its definition (+0x5C = pressure channel), +8 a
               state block with the owning faction at +0x0C and a faction mask at +0x50 (bit 3 + 2 * (f - 1)
               for faction f). */
            factionIndexOrScratch = *(int *)((int)ownerNode->runtimePayload + 8);
            if (*(int *)(factionIndexOrScratch + 0xc) != 0) {
              remainingFactionsOrPressureValue = *(int *)(*(int *)ownerNode->runtimePayload + 0x5c);
              nextChannelOrFactionBit = 8;
              channelOrFactionIndex = 1;
              pressureTargetRecord = g_GameFactionRuntimeImage.records;
              do {
                pressureTargetRecord++;
                if (((*(uint32_t *)(factionIndexOrScratch + 0x50) & nextChannelOrFactionBit) != 0) && (channelOrFactionIndex != *(uint32_t *)(factionIndexOrScratch + 0xc))) {
                  pressureTargetRecord->aiPressureValues[remainingFactionsOrPressureValue] =
                       pressureTargetRecord->aiPressureValues[remainingFactionsOrPressureValue] + 0x100;
                }
                nextChannelOrFactionBit = nextChannelOrFactionBit << 2;
                channelOrFactionIndex++;
              } while (channelOrFactionIndex < 8);
            }
          }
          ownerNode = ownerNode->nextNode;
        } while (ownerNode != NULL);
        maximumScanRecord = g_GameFactionRuntimeImage.records + 1;
        factionIndexOrScratch = 7;
        channelOrFactionIndex = 0;
        do {
          do {
            remainingFactionsOrPressureValue = maximumScanRecord->aiPressureValues[channelOrFactionIndex + 1];
            if (maximumScanRecord->maximumAiPressure <
                maximumScanRecord->aiPressureValues[channelOrFactionIndex]) {
              maximumScanRecord->maximumAiPressure =
                   maximumScanRecord->aiPressureValues[channelOrFactionIndex];
            }
            channelOrFactionIndex = channelOrFactionIndex + 2;
            if (maximumScanRecord->maximumAiPressure < remainingFactionsOrPressureValue) {
              maximumScanRecord->maximumAiPressure = remainingFactionsOrPressureValue;
            }
          } while (channelOrFactionIndex < 8);
          channelOrFactionIndex = 0;
          maximumScanRecord++;
          factionIndexOrScratch--;
        } while (factionIndexOrScratch != 0);
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
   ModelDefinition_IsFactionTechnologyUnlocked [assets/model/definitions].
*/
AiCandidateScore32
AiArmyCandidate_ComputeAverageCompatibleAssetScore
          (AiArmyScoreWeights *scoreWeights,FactionRuntimeIndex factionIndex,
          ModelRuntimeClassId runtimeClassId)

{
  ArmyAssetRuntimeSemanticView80 *armyAssetRecord;
  uint32_t assetDefinitionListAddress;
  ModelDefinitionRecordPrefix *modelDefinition;
  AiCandidateScore32 candidateScore;
  int registryEntriesRemaining;
  ArmyAssetRecordPrefix **armyAssetRegistryCursor;
  bool technologyUnlocked;
  int compatibleAssetCount;
  uint32_t compatibleAssetScoreSum;
  AiLinkedDefinitionListView *assetLinkedDefinitions;
  AiLinkedDefinitionListView *nestedLinkedDefinitions;
  PckModelDefinitionIdCatalog candidateModelDefinitionId;
  AiLinkedDefinitionListView *secondNestedLinkedDefinitions;
  
  modelDefinition = ModelDefinitionRegistry_FindByRuntimeClassId(runtimeClassId);
  candidateScore = 0;
  if (modelDefinition != (ModelDefinitionRecordPrefix *)0x0) {
    candidateModelDefinitionId = modelDefinition->definitionId;
    armyAssetRegistryCursor = g_ArmyAssetRecordRegistry;
    registryEntriesRemaining = 0x300;
    compatibleAssetScoreSum = 0;
    compatibleAssetCount = 0;
    for (; registryEntriesRemaining != 0;
        armyAssetRegistryCursor = armyAssetRegistryCursor + 1,
        registryEntriesRemaining = registryEntriesRemaining + -1) {
      armyAssetRecord = (ArmyAssetRuntimeSemanticView80 *)*armyAssetRegistryCursor;
      if ((armyAssetRecord != (ArmyAssetRuntimeSemanticView80 *)0x0) &&
         ((armyAssetRecord->flags14 & 1) != 0)) {
        assetDefinitionListAddress = armyAssetRecord->rootNodeOffsetOrPointer;
        if ((((candidateModelDefinitionId != *(PckModelDefinitionIdCatalog *)(assetDefinitionListAddress + 0x20)) &&
             (((candidateModelDefinitionId != *(PckModelDefinitionIdCatalog *)(assetDefinitionListAddress + 0x24) &&
               (candidateModelDefinitionId != *(PckModelDefinitionIdCatalog *)(assetDefinitionListAddress + 0x28))) &&
              (candidateModelDefinitionId != *(PckModelDefinitionIdCatalog *)(assetDefinitionListAddress + 0x2c))))) &&
            (((candidateModelDefinitionId != *(PckModelDefinitionIdCatalog *)(assetDefinitionListAddress + 0x30) &&
              (candidateModelDefinitionId != *(PckModelDefinitionIdCatalog *)(assetDefinitionListAddress + 0x34))) &&
             (candidateModelDefinitionId != *(PckModelDefinitionIdCatalog *)(assetDefinitionListAddress + 0x38))))) &&
           (candidateModelDefinitionId != *(PckModelDefinitionIdCatalog *)(assetDefinitionListAddress + 0x3c))) {
          technologyUnlocked = ModelDefinition_IsFactionTechnologyUnlocked
                            (g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits,
                             *(PckModelDefinitionIdCatalog *)(assetDefinitionListAddress + 0x20));
          if (((technologyUnlocked) ||
              (nestedLinkedDefinitions = *(AiLinkedDefinitionListView **)(assetDefinitionListAddress + 0xc),
              *(int *)(assetDefinitionListAddress + 8) == 0)) ||
             ((((candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[0] &&
                ((((candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[1] &&
                   (candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[2])) &&
                  (candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[3])) &&
                 ((candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[4] &&
                  (candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[5])))))) &&
               ((candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[6] &&
                (candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[7])))) &&
              ((secondNestedLinkedDefinitions = *(AiLinkedDefinitionListView **)(assetDefinitionListAddress + 0x10),
               *(uint32_t *)(assetDefinitionListAddress + 8) < 2 ||
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
          continue;
        }
        candidateScore = AiArmyCandidate_ComputeFactionWeightedScore
                          (scoreWeights,factionIndex,armyAssetRecord);
        if (0 < candidateScore) {
          compatibleAssetScoreSum = compatibleAssetScoreSum + candidateScore;
          compatibleAssetCount = compatibleAssetCount + 1;
        }
      }
    }
    candidateScore = 0;
    if (compatibleAssetCount != 0) {
      candidateScore = (AiCandidateScore32)
              ((int64_t)(uint64_t)compatibleAssetScoreSum / (int64_t)compatibleAssetCount);
    }
  }
  return candidateScore;
}


/* Address: 0x005379E0.
   Ownership: gameplay/ai/planning.
   Purpose: Scans the secondary AI entity workspace and collects up to four unique combined grid-mask classes
   derived from each active entity definition. When no class is found it installs the verified default mask
   0x90000100.
*/
void __thandor_void_preserve_eax_ecx_edx AiPlanning_CollectActiveGridMaskClasses(void)

{
  ModelRuntimeSlot *entityModelRuntime;
  int lowClassShift;
  uint32_t highClassShift;
  uint32_t maskClass0;
  uint32_t maskClass1;
  uint32_t maskClass2;
  uint32_t combinedMask;
  int remainingEntries;
  AiRuntimeWorkspaceEntry *runtimeWorkspaceEntry;
  
  g_AiActiveGridMaskClass0 = 0xffffffff;
  g_AiActiveGridMaskClass1 = 0xffffffff;
  g_AiActiveGridMaskClass2 = 0xffffffff;
  g_AiActiveGridMaskClass3 = 0xffffffff;
  maskClass0 = g_AiActiveGridMaskClass0;
  maskClass1 = g_AiActiveGridMaskClass1;
  maskClass2 = g_AiActiveGridMaskClass2;
  runtimeWorkspaceEntry = g_AiWorkspaceBuffer01_Size0200;
  for (remainingEntries = g_AiWorkspace01Count; g_AiActiveGridMaskClass0 = maskClass0, remainingEntries != 0;
      remainingEntries = remainingEntries + -1) {
    g_AiActiveGridMaskClass1 = maskClass1;
    g_AiActiveGridMaskClass2 = maskClass2;
    if (runtimeWorkspaceEntry->armyRuntime != (ArmyRuntimeSlot *)0x0) {
      entityModelRuntime = (runtimeWorkspaceEntry->armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
      lowClassShift = entityModelRuntime[1].classLinkState.modelLinkOrState60.signedScalarState;
      if ((*(int *)(entityModelRuntime->reserved10_37 + 8) != 0) && (-1 < lowClassShift)) {
        highClassShift = entityModelRuntime[1].classLinkState.classState64;
        if ((((-1 < (int)highClassShift) &&
             ((((combinedMask = 0x100 << ((uint8_t)lowClassShift & 0x1f) | 0x80000000U |
                         0x1000000 << ((uint8_t)highClassShift & 0x1f), combinedMask != maskClass0 && (combinedMask != maskClass1)) &&
               (combinedMask != maskClass2)) &&
              ((combinedMask != g_AiActiveGridMaskClass3 &&
               (g_AiActiveGridMaskClass0 = combinedMask, maskClass0 != 0xffffffff)))))) &&
            (g_AiActiveGridMaskClass0 = maskClass0, g_AiActiveGridMaskClass1 = combinedMask, maskClass1 != 0xffffffff
            )) && ((g_AiActiveGridMaskClass1 = maskClass1, g_AiActiveGridMaskClass2 = combinedMask,
                   maskClass2 != 0xffffffff &&
                   (g_AiActiveGridMaskClass2 = maskClass2, g_AiActiveGridMaskClass3 == 0xffffffff)))) {
          g_AiActiveGridMaskClass3 = combinedMask;
        }
      }
    }
    runtimeWorkspaceEntry = runtimeWorkspaceEntry + 1;
    maskClass0 = g_AiActiveGridMaskClass0;
    maskClass1 = g_AiActiveGridMaskClass1;
    maskClass2 = g_AiActiveGridMaskClass2;
  }
  g_AiActiveGridMaskClass1 = maskClass1;
  g_AiActiveGridMaskClass2 = maskClass2;
  if (maskClass0 == 0xffffffff) {
    g_AiActiveGridMaskClass0 = 0x90000100;
  }
  return;
}


/* Address: 0x0053C810.
   AI planning job of the simulation step for one faction. Each faction gets its turn every 64 simulation ticks
   (bits 3-5 of the tick select the faction); bit 6 alternates between the unit phase (behaviour update and
   group-to-target assignment) and the economy phase (relations, pending construction requests, then the
   purchase candidates, which are reused from the faction's cache while it is valid and the anchor cooldowns
   are below 50).
*/
void __thandor_void_preserve_eax_ecx_edx
AiRuntime_DispatchFactionPlanningPhase
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *inGameRuntime)

{
  uint32_t planningPhaseDispatchIndex;
  WorldObjectRecordCount *worldRuntime;
  bool phaseResult;
  AiKnowledgeDataImage *knowledgeData;

  if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
       SESSION_NETWORK_ROLE_LOCAL) || ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_AI_PLANNING_OFF) == 0)) {
    planningPhaseDispatchIndex = g_GameFactionRuntimeImage.tail.simulationTick >> 6 & 1;
    /* the world view inside the in-game root: inGameRuntime + 0xA30 (ADD EBX,0xa30 in the original) */
    worldRuntime = &inGameRuntime[7].objectCount;
    if ((g_GameFactionRuntimeImage.tail.simulationTick >> 3 & 7) == factionIndex) {
      AiPlanning_RebuildFactionWorkspaces
                (planningPhaseDispatchIndex,factionIndex,factionIndex,
                 (WorldRuntimeContext *)worldRuntime);
      switch(planningPhaseDispatchIndex) {
      case 0:
        AiUnitBehavior_UpdateWorkspace01Entities(factionIndex,(WorldRuntimeContext *)worldRuntime);
        AiUnitGroup_AssignCollectedEntitiesToBestTarget();
        break;
      case 1:
        AiFactionPlanning_UpdateActiveEntityPressureFlag(factionIndex);
        GameFactionRelations_UpdateAllPairsForFaction
                  (factionIndex,(WorldRuntimeContext *)worldRuntime);
        phaseResult = AiConstructionPlanner_ProcessPendingAssetRequests
                          (factionIndex,(WorldRuntimeContext *)worldRuntime);
        knowledgeData = g_AiKnowledgeData;
        if (!phaseResult) {
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
            phaseResult = AiPurchasePlanner_ExecuteAffordableCandidates(factionIndex);
            if (phaseResult) {
              AiCandidateWorkspace_SaveToFactionImage(factionIndex * 0x740);
              g_GameFactionRuntimeImage.records[factionIndex].candidateCache.cacheReuseState = 0x10;
            }
          }
          else {
            AiCandidateWorkspace_LoadFromFactionImage(factionIndex * 0x740);
            phaseResult = AiPurchasePlanner_ExecuteAffordableCandidates(factionIndex);
            if (!phaseResult) {
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
   Cross-module calls: AiPrimaryWorkspace_HasUnassignedEntryById [gameplay/ai/workspaces],
   AiConstructionPlanner_PlaceSpecialAssetFromWorkspace [gameplay/ai/workspaces], ArmyAssetRegistry_FindById
   [assets/army/catalog], ModelDefinition_SelectFactionUnlockedLinkedDefinition [assets/model/definitions].
*/
bool __thandor_cf_preserve_eax_ecx_edx
AiConstructionPlanner_ProcessPendingAssetRequests
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  PckArmyAssetIdCatalog armyAssetId;
  int remainingRequests;
  AiRuntimeWorkspaceEntry *requestEntry;
  bool hasUnassignedEntry;
  ArmyAssetLookupResult armyAssetLookup;
  ModelDefinitionResult linkedDefinitionLookup;
  
  g_AiConstructionPendingAssetConsumedCount = 0;
  remainingRequests = g_AiWorkspace04Count;
  requestEntry = g_AiWorkspaceBuffer04_Size0040;
  do {
    if (remainingRequests == 0) {
      return false;
    }
    armyAssetId = requestEntry->armyAssetId;
    if (armyAssetId == ARM_0300_BUILDING_MDL0301) {
      hasUnassignedEntry = AiPrimaryWorkspace_HasUnassignedEntryById(ARM_0330_BUILDING_MDL0303);
      if (!hasUnassignedEntry) {
        AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate
                  (armyAssetId,factionIndex,worldRuntime);
      }
    }
    else if ((armyAssetId == ARM_0330_BUILDING_MDL0303) ||
            (armyAssetId == ARM_0332_BUILDING_MDL0302)) {
      AiConstructionPlanner_PlaceSpecialAssetFromWorkspace(armyAssetId,factionIndex,worldRuntime);
    }
    else if (armyAssetId == ARM_0331_BUILDING_MDL0308) {
      AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate
                (armyAssetId,factionIndex,worldRuntime);
    }
    else if (armyAssetId == ARM_0333_BUILDING_MDL0307) {
      AiConstructionPlanner_PlaceDerivedAsset14D
                (ARM_0333_BUILDING_MDL0307,factionIndex,worldRuntime);
    }
    else if (armyAssetId < ARM_0340_BUILDING_MDL0314) {
      armyAssetLookup = ArmyAssetRegistry_FindById(armyAssetId);
      if (!armyAssetLookup.notFound) {
        /* The original then compares the selected definition's +0x278 word with 1 (ignoring the selector's
           CF), but both outcomes call the same placement handler. */
        linkedDefinitionLookup = ModelDefinition_SelectFactionUnlockedLinkedDefinition
                          (factionIndex,(armyAssetLookup.recordOrError)->rootNodeOffsetOrPointer);
        AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate
                  (armyAssetId,factionIndex,worldRuntime);
      }
    }
    else {
      AiConstructionPlanner_PlaceExtendedAssetNearFactionAnchor
                (armyAssetId,factionIndex,worldRuntime);
    }
    if (g_AiConstructionPendingAssetConsumedCount != 0) {
      return true;
    }
    requestEntry = requestEntry + 1;
    remainingRequests = remainingRequests + -1;
  } while( true );
}


/* Address: 0x005378C0.
   Ownership: gameplay/ai/planning.
   Purpose: Sorts the purchase-candidate workspace, subtracts each candidate cost from the faction budget, verifies
   an eligible producer, and applies the first affordable supported candidate. It returns a carry-derived status
   through the original convention. Stock tech.tec has 512 records over canonical ids 0..255; localized titles do
   not prove source-building, tier, direction, or effect mappings.
   Local calls: AiPurchaseCandidate_HasEligibleProducer, AiPurchaseCandidate_ApplyToFaction.
   Cross-module calls: AiCandidateWorkspace_SortDescending [gameplay/ai/workspaces],
   AiCandidateWorkspace_GetEntryEntityValue [gameplay/ai/workspaces].
*/
bool __thandor_cf_preserve_eax_ecx_edx
AiPurchasePlanner_ExecuteAffordableCandidates(FactionRuntimeIndex factionIndex)

{
  bool noCandidateApplied;
  uint32_t candidateCost;
  int remainingCandidates;
  XeniteAmountQ4 remainingXenite;
  AiCandidateWorkspaceEntry *entry;
  bool insufficientOrNoProducer;
  
  remainingCandidates = g_AiCandidateWorkspaceEntryCount;
  entry = g_AiWorkspaceBuffer13_Size0400;
  g_AiPurchaseAppliedArmyClassMask = 0;
  noCandidateApplied = false;
  if (g_AiCandidateWorkspaceEntryCount != 0) {
    noCandidateApplied = true;
    AiCandidateWorkspace_SortDescending();
    remainingXenite = g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4;
    do {
      candidateCost = AiCandidateWorkspace_GetEntryEntityValue(entry);
      insufficientOrNoProducer = remainingXenite < candidateCost;
      remainingXenite = remainingXenite - candidateCost;
      if (insufficientOrNoProducer) {
        return noCandidateApplied;
      }
      insufficientOrNoProducer = AiPurchaseCandidate_HasEligibleProducer(entry,factionIndex);
      if (!insufficientOrNoProducer) {
        AiPurchaseCandidate_ApplyToFaction(entry,factionIndex);
        noCandidateApplied = false;
      }
      entry = entry + 1;
      remainingCandidates = remainingCandidates + -1;
    } while (remainingCandidates != 0);
  }
  return noCandidateApplied;
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
   AiPlacement_FindNearestValidWorkspace09Anchor [gameplay/ai/placement], ArmyRuntime_CreateInstanceFromAsset
   [gameplay/army/runtime], ModelNodeRuntime_RebuildTransformsFromRoot [world/model/hierarchy],
   ArmyRuntime_DispatchClassCommand [gameplay/army/runtime], EffectRuntimePool_CreateInstanceFromDefinition
   [world/effects/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
AiConstructionPlanner_PlaceDerivedAsset14D
          (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime)

{
  FieldGridCell *sourceCell;
  ArmyRuntimeSlot *createdModelNode;
  ArmyRuntimeSlot *createdArmySlot;
  ModelRuntimeSlot *createdModelRuntime;
  ArmyRuntimeSlot **armyRuntime;
  AiTerrainFeatureWorkspaceEntry *site14AEntry;
  AiTerrainFeatureWorkspaceEntry *site14CEntry;
  int remaining14AEntries;
  int remaining14CEntries;
  bool siteRejected;
  ArmyRuntimeCreateResult createdInstance;
  AiAnchorResult anchorResult;
  
  site14AEntry = g_AiWorkspaceBuffer08_Size0200;
  remaining14AEntries = g_AiWorkspace08Count;
  if (g_AiWorkspace08Count != 0) {
    do {
      if (site14AEntry->armyAssetId == ARM_0330_BUILDING_MDL0303) {
        sourceCell = site14AEntry->cell;
        siteRejected = AiPlacement_ReserveAdditionalSpecialSite
                          (site14AEntry->armyAssetId,sourceCell,factionIndex,worldRuntime);
        if (!siteRejected) {
          anchorResult = AiPlacement_FindNearestValidWorkspace09Anchor
                            (sourceCell->worldY,sourceCell->worldX,armyAssetId,factionIndex,worldRuntime);
          if (!anchorResult.notFound) {
            createdInstance = ArmyRuntime_CreateInstanceFromAsset
                              (4,0,anchorResult.worldYQ12,anchorResult.worldXQ12,factionIndex,armyAssetId,
                               worldRuntime);
            armyRuntime = (ArmyRuntimeSlot **)createdInstance.armyRuntimeOrError;
            if (createdInstance.failed) {
              return;
            }
            createdModelNode = armyRuntime[1];
            createdArmySlot = *armyRuntime;
            createdModelNode->movementPosition0Q12 = 0;
            createdModelRuntime = (createdArmySlot->modelRuntimeOrSavedOffset).modelRuntime;
            ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)createdModelNode);
            ArmyRuntime_DispatchClassCommand(armyRuntime,worldRuntime);
            EffectRuntimePool_CreateInstanceFromDefinition
                      (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference4, 0x0),
                       (createdModelNode->movementControl).turnVelocityAngle16,
                       (createdModelNode->movementControl).movementAdvancePerTickQ12,
                       ((WorldRuntimeNodeModelPayload *)&createdModelNode->factionIndex)->worldRotationAngle0,
                       createdModelNode->depthBinClass,createdModelNode->runtimeState98,
                       ((GraphicsFixedVec3 *)&createdModelNode->runtimeState94)->x,
                       (EffectDefinition *)createdModelRuntime->attachments140[2].childLocalRotationAngle0,
                       worldRuntime);
            AiConstructionPlanner_ConsumeFactionPendingArmyAsset(armyAssetId,factionIndex);
            return;
          }
        }
      }
      site14AEntry = site14AEntry + 1;
      remaining14AEntries = remaining14AEntries + -1;
      site14CEntry = g_AiWorkspaceBuffer08_Size0200;
      remaining14CEntries = g_AiWorkspace08Count;
    } while (remaining14AEntries != 0);
    do {
      if (site14CEntry->armyAssetId == ARM_0332_BUILDING_MDL0302) {
        sourceCell = site14CEntry->cell;
        siteRejected = AiPlacement_ReserveAdditionalSpecialSite
                          (site14CEntry->armyAssetId,sourceCell,factionIndex,worldRuntime);
        if (!siteRejected) {
          anchorResult = AiPlacement_FindNearestValidWorkspace09Anchor
                            (sourceCell->worldY,sourceCell->worldX,armyAssetId,factionIndex,worldRuntime);
          if (!anchorResult.notFound) {
            createdInstance = ArmyRuntime_CreateInstanceFromAsset
                              (4,0,anchorResult.worldYQ12,anchorResult.worldXQ12,factionIndex,armyAssetId,
                               worldRuntime);
            armyRuntime = (ArmyRuntimeSlot **)createdInstance.armyRuntimeOrError;
            if (createdInstance.failed) {
              return;
            }
            createdModelNode = armyRuntime[1];
            createdArmySlot = *armyRuntime;
            createdModelNode->movementPosition0Q12 = 0;
            createdModelRuntime = (createdArmySlot->modelRuntimeOrSavedOffset).modelRuntime;
            ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)createdModelNode);
            ArmyRuntime_DispatchClassCommand(armyRuntime,worldRuntime);
            EffectRuntimePool_CreateInstanceFromDefinition
                      (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference4, 0x0),
                       (createdModelNode->movementControl).turnVelocityAngle16,
                       (createdModelNode->movementControl).movementAdvancePerTickQ12,
                       ((WorldRuntimeNodeModelPayload *)&createdModelNode->factionIndex)->worldRotationAngle0,
                       createdModelNode->depthBinClass,createdModelNode->runtimeState98,
                       ((GraphicsFixedVec3 *)&createdModelNode->runtimeState94)->x,
                       (EffectDefinition *)createdModelRuntime->attachments140[2].childLocalRotationAngle0,
                       worldRuntime);
            AiConstructionPlanner_ConsumeFactionPendingArmyAsset(armyAssetId,factionIndex);
            return;
          }
        }
      }
      remaining14CEntries = remaining14CEntries + -1;
      site14CEntry = site14CEntry + 1;
    } while (remaining14CEntries != 0);
  }
  AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate(armyAssetId,factionIndex,worldRuntime);
  return;
}


/* Address: 0x0053A6E0.
   Ownership: gameplay/ai/planning.
   Purpose: Scores eligible workspace11 army candidates with coefficient profile A, selects the highest positive
   result, adjusts its planning weight for existing pending extended assets and faction state, and adds the
   selected registry ID to the candidate workspace.
   Local calls: AiFactionRuntime_TestPlanningCapacityExceeded, AiArmyCandidate_ComputeFactionWeightedScore.
   Cross-module calls: Technology_IsUnlockedForFaction [gameplay/technology/runtime],
   AiCandidateWorkspace_AddOrAccumulateWeightedEntry [gameplay/ai/workspaces].
*/
void __thandor_void_preserve_eax_ecx_edx
AiArmyCandidate_AddBestScoredVariantA
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  AiCandidateScore32 candidateScore;
  uint32_t weightRange;
  int remainingEntries;
  uint32_t pendingCountOrWeight;
  int bestScore;
  ArmyAssetRecordPrefix *bestArmyAsset;
  AiRuntimeWorkspaceEntry *runtimeWorkspaceEntry;
  ArmyAssetRecordPrefix **armyAssetRegistryCursor;
  bool blockingCondition;
  
  remainingEntries = g_AiWorkspace11Count;
  armyAssetRegistryCursor = g_AiWorkspaceBuffer11_Size1000;
  if (g_AiWorkspace11Count != 0) {
    bestScore = 0;
    blockingCondition = AiFactionRuntime_TestPlanningCapacityExceeded(4,factionIndex);
    if ((!blockingCondition) &&
       (blockingCondition = Technology_IsUnlockedForFaction(TEC_001_ARMS_FACTORIES,factionIndex), !blockingCondition)) {
      do {
        if (((((ArmyAssetRuntimeSemanticView80 *)*armyAssetRegistryCursor)->flags14 & 1) != 0) &&
           (candidateScore = AiArmyCandidate_ComputeFactionWeightedScore
                              (&g_AiArmyCandidateScoreWeightsVariantA15,factionIndex,
                               (ArmyAssetRuntimeSemanticView80 *)*armyAssetRegistryCursor),
           bestScore < candidateScore)) {
          bestArmyAsset = *armyAssetRegistryCursor;
          bestScore = candidateScore;
        }
        armyAssetRegistryCursor = armyAssetRegistryCursor + 1;
        remainingEntries = remainingEntries + -1;
      } while (remainingEntries != 0);
      if (0 < bestScore) {
        pendingCountOrWeight = 1;
        runtimeWorkspaceEntry = g_AiWorkspaceBuffer04_Size0040;
        for (remainingEntries = g_AiWorkspace04Count; remainingEntries != 0; remainingEntries = remainingEntries + -1) {
          if (((ARM_0320_BUILDING_MDL0311|ARM_0019_UNIT_MDL0101) <
               runtimeWorkspaceEntry->armyAssetId) &&
             (runtimeWorkspaceEntry->armyAssetId < ARM_0380_BUILDING_MDL0329)) {
            pendingCountOrWeight = pendingCountOrWeight + 1;
          }
          runtimeWorkspaceEntry = runtimeWorkspaceEntry + 1;
        }
        pendingCountOrWeight = (g_AiKnowledgeData->parameters).armyVariantABaseWeight / pendingCountOrWeight;
        if (g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorCooldown == 0) {
          weightRange = pendingCountOrWeight * 3 >> 2;
        }
        else {
          weightRange = pendingCountOrWeight * 2;
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
   available class in the 0x12E through 0x132 family and adds it with a knowledge-derived weight reduced by the
   existing class count.
   Local calls: AiFactionRuntime_TestPlanningCapacityExceeded, AiStrategicClass_SelectBestCandidate12ETo132.
   Cross-module calls: AiPrimaryWorkspace_HasEntryById [gameplay/ai/workspaces],
   Technology_IsUnlockedForFaction [gameplay/technology/runtime],
   AiCandidateWorkspace_AddOrAccumulateWeightedEntry [gameplay/ai/workspaces].
*/
void __thandor_void_preserve_eax_ecx_edx
AiStrategicClass_AddCandidate12DOr12FTo132
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  uint32_t weightRange;
  bool conditionMet;
  AiStrategicClassSelectionRegs8 classSelection;
  AiKnowledgeDataImage *knowledgeData;
  
  knowledgeData = g_AiKnowledgeData;
  conditionMet = AiPrimaryWorkspace_HasEntryById(ARM_0330_BUILDING_MDL0303);
  if (conditionMet) {
    conditionMet = Technology_IsUnlockedForFaction(TEC_001_ARMS_FACTORIES,factionIndex);
    if (conditionMet) {
      conditionMet = AiPrimaryWorkspace_HasEntryById(ARM_0301_BUILDING_MDL0318);
      if (!conditionMet) {
        conditionMet = AiFactionRuntime_TestPlanningCapacityExceeded
                          ((knowledgeData->parameters).
                           strategic12dAnd141To143AdditionalPlanningCapacity,factionIndex);
        if (!conditionMet) {
          AiCandidateWorkspace_AddOrAccumulateWeightedEntry
                    (0x12d,(knowledgeData->parameters).strategicClass12dBaseWeight,1);
        }
      }
    }
    else {
      conditionMet = AiFactionRuntime_TestPlanningCapacityExceeded
                        ((knowledgeData->parameters).strategic12fTo132AdditionalPlanningCapacity,
                         factionIndex);
      if (!conditionMet) {
        classSelection = AiStrategicClass_SelectBestCandidate12ETo132(factionIndex,worldRuntime);
        if (classSelection.selectedRuntimeToken != 0) {
          weightRange = (knowledgeData->parameters).strategicClass12fTo132BaseWeight;
          if (classSelection.existingCountOrPressure != 0) {
            weightRange = weightRange / (classSelection.existingCountOrPressure * 2);
          }
          AiCandidateWorkspace_AddOrAccumulateWeightedEntry
                    (classSelection.selectedRuntimeToken,weightRange,1);
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
   Local calls: AiFactionRuntime_TestPlanningCapacityExceeded, AiStrategicClass_SelectWeightedClass141To143.
   Cross-module calls: AiPrimaryWorkspace_HasEntryById [gameplay/ai/workspaces],
   AiCandidateWorkspace_AddOrAccumulateWeightedEntry [gameplay/ai/workspaces].
*/
void __thandor_void_preserve_eax_ecx_edx
AiStrategicClass_AddWeightedClassCandidate
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  AiKnowledgeDataImage *knowledgeData;
  uint32_t weightRange;
  bool conditionMet;
  AiStrategicClassSelectionRegs8 classSelection;
  
  knowledgeData = g_AiKnowledgeData;
  conditionMet = AiPrimaryWorkspace_HasEntryById(ARM_0330_BUILDING_MDL0303);
  if (conditionMet) {
    conditionMet = AiFactionRuntime_TestPlanningCapacityExceeded
                      ((knowledgeData->parameters).strategic12dAnd141To143AdditionalPlanningCapacity,
                       factionIndex);
    if (!conditionMet) {
      classSelection = AiStrategicClass_SelectWeightedClass141To143(factionIndex,worldRuntime);
      if (classSelection.selectedRuntimeToken != 0) {
        weightRange = (knowledgeData->parameters).strategicClass141To143BaseWeight;
        if (classSelection.existingCountOrPressure != 0) {
          weightRange = weightRange / (classSelection.existingCountOrPressure * 2);
        }
        AiCandidateWorkspace_AddOrAccumulateWeightedEntry(classSelection.selectedRuntimeToken,weightRange,1);
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
   AiWorkspace03_GetMinimumManhattanDistanceToPoint [gameplay/ai/workspaces], ArmyRuntime_CreateInstanceFromAsset
   [gameplay/army/runtime], ModelNodeRuntime_RebuildTransformsFromRoot [world/model/hierarchy].
*/
void __thandor_void_preserve_eax_ecx_edx
AiConstructionPlanner_PlaceExtendedAssetNearFactionAnchor
          (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime)

{
  ArmyRuntimeSlot *modelNodeRuntime;
  ArmyRuntimeSlot *createdArmySlot;
  ModelRuntimeSlot *createdModelRuntime;
  int anchorDistanceY;
  int workspaceDistanceOrScore;
  ArmyRuntimeSlot **createdSlots;
  int remainingCells;
  int anchorDistanceX;
  FieldGridCell **gridCellCursor;
  PlacementDispatchResult placementResult;
  ArmyRuntimeCreateResult createdInstance;
  FieldGridCell *bestCell;
  int bestScore;
  FieldGridCell *candidateCell;
  bool siteDistanceInRange;

  if ((g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorCooldown != 0) &&
     (g_AiWorkspace09Count != 0)) {
    bestScore = 0x7fffffff;
    remainingCells = g_AiWorkspace09Count;
    gridCellCursor = g_AiWorkspaceBuffer09_Size1000;
    do {
      candidateCell = *gridCellCursor;
      anchorDistanceY = g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorYQ12 - candidateCell->worldX;
      if (anchorDistanceY < 0) {
        anchorDistanceY = -anchorDistanceY;
      }
      anchorDistanceX = g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorXQ12 - candidateCell->worldY;
      if (anchorDistanceX < 0) {
        anchorDistanceX = -anchorDistanceX;
      }
      if ((anchorDistanceX + anchorDistanceY < bestScore) &&
         (workspaceDistanceOrScore = AiPrimaryWorkspace_GetMinimumActiveManhattanDistanceToPoint
                            (candidateCell->worldY,candidateCell->worldX), 0x1fff < workspaceDistanceOrScore)) {
        /* Distance to the nearest workspace-02 site (x4, at most 0x5000), or without any such site to the
           nearest workspace-03 site (x2, at most 0x8000). */
        workspaceDistanceOrScore = AiWorkspace02_GetMinimumManhattanDistanceToPoint
                          (candidateCell->worldY,candidateCell->worldX);
        if (workspaceDistanceOrScore < 0x7fffffff) {
          siteDistanceInRange = workspaceDistanceOrScore < 0x5001;
          if (siteDistanceInRange) {
            workspaceDistanceOrScore = workspaceDistanceOrScore * 4;
          }
        }
        else {
          workspaceDistanceOrScore = AiWorkspace03_GetMinimumManhattanDistanceToPoint
                            (candidateCell->worldY,candidateCell->worldX);
          siteDistanceInRange = workspaceDistanceOrScore < 0x8001;
          if (siteDistanceInRange) {
            workspaceDistanceOrScore = workspaceDistanceOrScore * 2;
          }
        }
        if (siteDistanceInRange) {
          workspaceDistanceOrScore = anchorDistanceX + anchorDistanceY + workspaceDistanceOrScore;
          if ((workspaceDistanceOrScore < bestScore) &&
             (placementResult = ArmyPlacement_DispatchAssetAtFieldPoint
                                (1,0,(uint32_t)(uint16_t)candidateCell->triangle0NormalAngles,
                                 candidateCell->worldY,candidateCell->worldX,armyAssetId,factionIndex,
                                 (UiRootNode *)worldRuntime), !placementResult.failed)) {
            bestCell = candidateCell;
            bestScore = workspaceDistanceOrScore;
          }
        }
      }
      gridCellCursor = gridCellCursor + 1;
      remainingCells = remainingCells + -1;
    } while (remainingCells != 0);
    if (bestScore < 0x7fffffff) {
      createdInstance = ArmyRuntime_CreateInstanceFromAsset
                        (4,(uint32_t)(uint16_t)bestCell->triangle0NormalAngles,bestCell->worldY,
                         bestCell->worldX,factionIndex,armyAssetId,worldRuntime);
      createdSlots = (ArmyRuntimeSlot **)createdInstance.armyRuntimeOrError;
      if (!createdInstance.failed) {
        modelNodeRuntime = createdSlots[1];
        createdArmySlot = *createdSlots;
        modelNodeRuntime->movementPosition0Q12 = 0;
        createdModelRuntime = (createdArmySlot->modelRuntimeOrSavedOffset).modelRuntime;
        ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)modelNodeRuntime);
        ArmyRuntime_DispatchClassCommand(createdSlots,worldRuntime);
        EffectRuntimePool_CreateInstanceFromDefinition
                  (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference4, 0x0),
                   (modelNodeRuntime->movementControl).turnVelocityAngle16,
                   (modelNodeRuntime->movementControl).movementAdvancePerTickQ12,
                   ((WorldRuntimeNodeModelPayload *)&modelNodeRuntime->factionIndex)->
                   worldRotationAngle0,modelNodeRuntime->depthBinClass,
                   modelNodeRuntime->runtimeState98,
                   ((GraphicsFixedVec3 *)&modelNodeRuntime->runtimeState94)->x,
                   (EffectDefinition *)createdModelRuntime->attachments140[2].childLocalRotationAngle0,
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
  AiCandidateScore32 candidateScore;
  uint32_t weightRange;
  int remainingEntries;
  int bestScore;
  ArmyAssetRecordPrefix *bestArmyAsset;
  ArmyAssetRecordPrefix **armyAssetRegistryCursor;
  
  if (((g_AiWorkspace05Count != 0) && (g_AiWorkspace11Count != 0)) && (g_AiWorkspace01Count < 0xb))
  {
    bestScore = 0;
    remainingEntries = g_AiWorkspace11Count;
    armyAssetRegistryCursor = g_AiWorkspaceBuffer11_Size1000;
    do {
      if ((((ArmyAssetRuntimeSemanticView80 *)*armyAssetRegistryCursor)->flags14 & 1) != 0) {
        candidateScore = AiArmyCandidate_ComputeFactionWeightedScore
                          (&g_AiArmyCandidateScoreWeightsVariantB15,factionIndex,
                           (ArmyAssetRuntimeSemanticView80 *)*armyAssetRegistryCursor);
        if (bestScore < candidateScore) {
          bestArmyAsset = *armyAssetRegistryCursor;
          bestScore = candidateScore;
        }
      }
      armyAssetRegistryCursor = armyAssetRegistryCursor + 1;
      remainingEntries = remainingEntries + -1;
    } while (remainingEntries != 0);
    if (0 < bestScore) {
      weightRange = (uint32_t)(((int64_t)
                             (int)(100 - g_GameFactionRuntimeImage.records[factionIndex].
                                         exploredTerrainPercent) *
                            (int64_t)
                            (int)(g_AiKnowledgeData->parameters).
                                 armyVariantBUnexploredTerrainWeightCoefficient) /
                           (int64_t)(int)((g_AiWorkspace01Count + 1) * 0x20));
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
  AiCandidateScore32 candidateScore;
  uint32_t weightRange;
  int remainingEntries;
  int bestScore;
  ArmyAssetRecordPrefix *bestArmyAsset;
  ArmyAssetRecordPrefix **armyAssetRegistryCursor;
  
  if (((g_AiWorkspace07Count != 0) && (g_AiWorkspace11Count != 0)) && (g_AiWorkspace01Count < 0xb))
  {
    bestScore = 0;
    remainingEntries = g_AiWorkspace11Count;
    armyAssetRegistryCursor = g_AiWorkspaceBuffer11_Size1000;
    do {
      if ((((ArmyAssetRuntimeSemanticView80 *)*armyAssetRegistryCursor)->flags14 & 1) != 0) {
        candidateScore = AiArmyCandidate_ComputeFactionWeightedScore
                          (&g_AiArmyCandidateScoreWeightsVariantC15,factionIndex,
                           (ArmyAssetRuntimeSemanticView80 *)*armyAssetRegistryCursor);
        if (bestScore < candidateScore) {
          bestArmyAsset = *armyAssetRegistryCursor;
          bestScore = candidateScore;
        }
      }
      armyAssetRegistryCursor = armyAssetRegistryCursor + 1;
      remainingEntries = remainingEntries + -1;
    } while (remainingEntries != 0);
    if (0 < bestScore) {
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
   Cross-module calls: Technology_IsAvailableForFaction [gameplay/technology/runtime],
   ArmyAssetRegistry_FindById [assets/army/catalog].
*/
bool __thandor_cf_preserve_eax_ecx_edx
AiPurchaseCandidate_HasEligibleProducer
          (AiCandidateWorkspaceEntry *candidateEntry,FactionRuntimeIndex factionIndex)

{
  uint32_t countOrClassMask;
  int *entitySlot;
  uint32_t remainingGuard;
  int technologySlotIndex;
  uint32_t remainingEntries;
  RuntimeToken technologyIndex;
  AiWorkspace00EntryView8 *workspaceEntry;
  bool technologyAvailable;
  ArmyAssetLookupResult armyAssetLookup;
  
  technologyIndex = candidateEntry->entityIdAndMultiplicity & 0xffff;
  if ((candidateEntry->weightedScoreAndKind & 0xf) == 2) {
    technologyAvailable = Technology_IsAvailableForFaction(technologyIndex,factionIndex);
    workspaceEntry = g_AiWorkspaceBuffer00_Size0400;
    countOrClassMask = g_AiWorkspace00Count;
    if (technologyAvailable) {
      for (; countOrClassMask != 0; countOrClassMask = countOrClassMask - 1) {
        entitySlot = (int *)workspaceEntry->runtimeSlotAddressOrZero;
        if ((entitySlot != (int *)0x0) && ((entitySlot[0x3b] & 0x89U) == 0)) {
          technologySlotIndex = 0x1c;
          do {
            if (technologyIndex == *(RuntimeToken *)(*entitySlot + 0x1c4 + technologySlotIndex * 4)) {
              return false;
            }
            technologySlotIndex = technologySlotIndex + -1;
          } while (technologySlotIndex != 0);
        }
        workspaceEntry = workspaceEntry + 1;
      }
    }
  }
  else {
    armyAssetLookup = ArmyAssetRegistry_FindById(technologyIndex);
    countOrClassMask = armyAssetLookup.recordOrError[1].selectionDetailTemplateVariantIndex;
    if ((g_AiWorkspace00Count != 0) && ((g_AiPurchaseAppliedArmyClassMask & countOrClassMask) == 0)) {
      remainingEntries = g_AiWorkspace00Count;
      workspaceEntry = g_AiWorkspaceBuffer00_Size0400;
      if ((countOrClassMask & 0x10) == 0) {
        if ((countOrClassMask & 8) == 0) {
          remainingGuard = countOrClassMask & 0xee;
          while (remainingGuard != 0) {
            entitySlot = (int *)workspaceEntry->runtimeSlotAddressOrZero;
            if (((entitySlot != (int *)0x0) && (*(int *)(*entitySlot + 0x4c) == 0xd)) &&
               (((entitySlot[0x3b] & 0xc9U) == 0 &&
                (((*(uint32_t *)(*entitySlot + 0xc4) & countOrClassMask & 0xee) != 0 && (entitySlot[0x2e] == 0)))))) {
              return false;
            }
            workspaceEntry = workspaceEntry + 1;
            remainingEntries = remainingEntries - 1;
            remainingGuard = remainingEntries;
          }
        }
        else {
          do {
            entitySlot = (int *)workspaceEntry->runtimeSlotAddressOrZero;
            if (((entitySlot != (int *)0x0) && (*(int *)(*entitySlot + 0x4c) == 0x16)) &&
               (((entitySlot[0x3b] & 0xc9U) == 0 && (entitySlot[0x2b] == 0)))) {
              return false;
            }
            workspaceEntry = workspaceEntry + 1;
            remainingEntries = remainingEntries - 1;
          } while (remainingEntries != 0);
        }
      }
      else {
        do {
          entitySlot = (int *)workspaceEntry->runtimeSlotAddressOrZero;
          if ((((entitySlot != (int *)0x0) && (*(int *)(*entitySlot + 0x4c) == 0xb)) &&
              ((entitySlot[0x3b] & 0xc9U) == 0)) && (entitySlot[0x2e] == 0)) {
            return false;
          }
          workspaceEntry = workspaceEntry + 1;
          remainingEntries = remainingEntries - 1;
        } while (remainingEntries != 0);
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
   GameFactionRuntime_RegisterArmyAssetPointers [gameplay/faction/runtime], ArmyAssetRegistry_FindById
   [assets/army/catalog].
*/
void __thandor_void_preserve_eax_ecx_edx
AiPurchaseCandidate_ApplyToFaction
          (AiCandidateWorkspaceEntry *candidateEntry,FactionRuntimeIndex factionIndex)

{
  GameEntityRuntime *entity;
  int remainingEntries;
  AiWorkspace00EntryView8 *workspaceEntry;
  int technologySlotIndex;
  RuntimeToken technologyIndex;
  ArmyAssetLookupResult armyAssetLookup;
  
  technologyIndex = candidateEntry->entityIdAndMultiplicity & 0xffff;
  remainingEntries = g_AiWorkspace00Count;
  workspaceEntry = g_AiWorkspaceBuffer00_Size0400;
  if ((candidateEntry->weightedScoreAndKind & 0xf) == 2) {
    for (; remainingEntries != 0; remainingEntries = remainingEntries + -1) {
      entity = (GameEntityRuntime *)workspaceEntry->runtimeSlotAddressOrZero;
      if ((entity != (GameEntityRuntime *)0x0) && (((entity->common).runtimeFlags & 0x89) == 0)) {
        technologySlotIndex = 0x1c;
        do {
          if (technologyIndex ==
              *(RuntimeToken *)
               ((int)(entity->common).ownership.definitionOrClassRecord + technologySlotIndex * 4 + 0x1c4)) {
            Technology_ApplyRecordToEntity(technologyIndex,entity);
            workspaceEntry->runtimeSlotAddressOrZero = 0;
            return;
          }
          technologySlotIndex = technologySlotIndex + -1;
        } while (technologySlotIndex != 0);
      }
      workspaceEntry = workspaceEntry + 1;
    }
  }
  else {
    GameFactionRuntime_RegisterArmyAssetPointers(0xffffffff,1,technologyIndex,factionIndex);
    armyAssetLookup = ArmyAssetRegistry_FindById(technologyIndex);
    if (!armyAssetLookup.notFound) {
      g_AiPurchaseAppliedArmyClassMask =
           g_AiPurchaseAppliedArmyClassMask | armyAssetLookup.recordOrError[1].selectionDetailTemplateVariantIndex;
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
  FactionRuntimeFlags *factionRuntimeFlags;
  int remainingEntries;
  int thresholdOrRemaining;
  WorldRuntimeContext *contextArg;
  AiWorkspace00EntryView8 *primaryEntry;
  AiRuntimeWorkspaceEntry *runtimeWorkspaceEntry;
  ArmyRuntimeSlot *armySlot;
  
  thresholdOrRemaining = 2;
  primaryEntry = g_AiWorkspaceBuffer00_Size0400;
  for (remainingEntries = g_AiWorkspace00Count; remainingEntries != 0; remainingEntries = remainingEntries + -1) {
    if ((primaryEntry->runtimeSlotAddressOrZero != 0) &&
       ((primaryEntry->armyAssetId < ARM_0340_BUILDING_MDL0314 ||
        ((primaryEntry->armyAssetId < ARM_0380_BUILDING_MDL0329 && (thresholdOrRemaining = thresholdOrRemaining + -1, thresholdOrRemaining == 0)))))) {
      factionRuntimeFlags = &g_GameFactionRuntimeImage.records[factionIndex].runtimeFlags;
      *factionRuntimeFlags = *factionRuntimeFlags | 1;
      return;
    }
    primaryEntry = primaryEntry + 1;
  }
  thresholdOrRemaining = thresholdOrRemaining + 1;
  remainingEntries = g_AiWorkspace01Count;
  runtimeWorkspaceEntry = g_AiWorkspaceBuffer01_Size0200;
  while( true ) {
    if (remainingEntries == 0) {
      if ((g_GameFactionRuntimeImage.records[factionIndex].runtimeFlags & 1) != 0) {
        contextArg = &g_InGameRuntimeRoot->worldRuntime0A30;
        primaryEntry = g_AiWorkspaceBuffer00_Size0400;
        for (thresholdOrRemaining = g_AiWorkspace00Count; remainingEntries = g_AiWorkspace01Count,
            runtimeWorkspaceEntry = g_AiWorkspaceBuffer01_Size0200, thresholdOrRemaining != 0; thresholdOrRemaining = thresholdOrRemaining + -1)
        {
          if (primaryEntry->runtimeSlotAddressOrZero != 0) {
            ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive
                      (contextArg,*(int **)(primaryEntry->runtimeSlotAddressOrZero + 8));
          }
          primaryEntry = primaryEntry + 1;
        }
        for (; remainingEntries != 0; remainingEntries = remainingEntries + -1) {
          if (runtimeWorkspaceEntry->armyRuntime != (ArmyRuntimeSlot *)0x0) {
            ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive
                      (contextArg,(int *)runtimeWorkspaceEntry->armyRuntime->linkedEntityRuntime);
          }
          runtimeWorkspaceEntry = runtimeWorkspaceEntry + 1;
        }
      }
      return;
    }
    armySlot = runtimeWorkspaceEntry->armyRuntime;
    if ((((armySlot != (ArmyRuntimeSlot *)0x0) && (armySlot->factionIndex != 0)) &&
        ((armySlot[1].commandCoordinate0Q12 != 0 ||
         ((1 < (uint32_t)armySlot->factionIndex && (armySlot[1].runtimeState40 != 0)))))) &&
       (thresholdOrRemaining = thresholdOrRemaining + -1, thresholdOrRemaining == 0)) break;
    runtimeWorkspaceEntry = runtimeWorkspaceEntry + 1;
    remainingEntries = remainingEntries + -1;
  }
  factionRuntimeFlags = &g_GameFactionRuntimeImage.records[factionIndex].runtimeFlags;
  *factionRuntimeFlags = *factionRuntimeFlags | 1;
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
   Cross-module calls: AiPrimaryWorkspace_HasUnassignedEntryById [gameplay/ai/workspaces],
   AiPrimaryWorkspace_HasEntryById [gameplay/ai/workspaces], AiCandidateWorkspace_AddOrAccumulateWeightedEntry
   [gameplay/ai/workspaces], AiPlacement_ReserveAdditionalSpecialSite [gameplay/ai/placement],
   AiPrimaryWorkspace_CountAssignedEntriesById [gameplay/ai/workspaces].
*/
void __thandor_void_preserve_eax_ecx_edx
AiStructureCandidate_AddWeightedId14BOr14CCandidate
          (AiCandidateScore32 baseWeight,PckArmyAssetIdCatalog candidateArmyAssetId,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  uint32_t storageLimit;
  uint32_t xeniteCurrent;
  AiTerrainFeatureWorkspaceEntry *terrainFeatureEntry;
  int remainingOrAssignedCount;
  uint32_t derivedWeight;
  bool conditionMet;
  AiKnowledgeDataImage *knowledgeData;
  
  conditionMet = AiPrimaryWorkspace_HasUnassignedEntryById(candidateArmyAssetId);
  knowledgeData = g_AiKnowledgeData;
  if (!conditionMet) {
    storageLimit = g_GameFactionRuntimeImage.records[factionIndex].tritiumStorageLimitQ4;
    if (candidateArmyAssetId == ARM_0331_BUILDING_MDL0308) {
      storageLimit = g_GameFactionRuntimeImage.records[factionIndex].xeniteStorageLimitQ4;
      conditionMet = AiPrimaryWorkspace_HasEntryById(ARM_0330_BUILDING_MDL0303);
      if (((conditionMet) && (storageLimit != 0)) &&
         (storageLimit < (knowledgeData->parameters).structure14bPrerequisite14aCountLimit)) {
        xeniteCurrent = g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4;
        if (storageLimit == xeniteCurrent) {
          baseWeight = baseWeight << 2;
        }
        if (storageLimit - xeniteCurrent < (knowledgeData->parameters).structure14bCountGapLimit) {
          AiCandidateWorkspace_AddOrAccumulateWeightedEntry(0x14b,baseWeight,1);
        }
      }
    }
    else {
      conditionMet = AiPrimaryWorkspace_HasEntryById(ARM_0332_BUILDING_MDL0302);
      terrainFeatureEntry = g_AiWorkspaceBuffer08_Size0200;
      remainingOrAssignedCount = g_AiWorkspace08Count;
      if (((conditionMet) && (storageLimit != 0)) &&
         ((storageLimit < (knowledgeData->parameters).structure14dPrerequisite14cCountLimit &&
          (storageLimit - g_GameFactionRuntimeImage.records[factionIndex].tritiumCurrentQ4 <
           (knowledgeData->parameters).structure14dCountGapLimit)))) {
        AiCandidateWorkspace_AddOrAccumulateWeightedEntry(candidateArmyAssetId,baseWeight,1);
        terrainFeatureEntry = g_AiWorkspaceBuffer08_Size0200;
        remainingOrAssignedCount = g_AiWorkspace08Count;
      }
      for (; remainingOrAssignedCount != 0; remainingOrAssignedCount = remainingOrAssignedCount + -1) {
        conditionMet = AiPlacement_ReserveAdditionalSpecialSite
                          (terrainFeatureEntry->armyAssetId,terrainFeatureEntry->cell,factionIndex,worldRuntime);
        if (!conditionMet) {
          derivedWeight = (knowledgeData->parameters).workspace08Id14aDerivedWeight;
          if (terrainFeatureEntry->armyAssetId != ARM_0330_BUILDING_MDL0303) {
            derivedWeight = (knowledgeData->parameters).workspace08OtherDerivedWeight;
          }
          remainingOrAssignedCount = AiPrimaryWorkspace_CountAssignedEntriesById(terrainFeatureEntry->armyAssetId);
          AiCandidateWorkspace_AddOrAccumulateWeightedEntry
                    (candidateArmyAssetId,(derivedWeight * 3) / (remainingOrAssignedCount + 3U),1);
          return;
        }
        terrainFeatureEntry = terrainFeatureEntry + 1;
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
   Cross-module calls: AiPrimaryWorkspace_HasEntryById [gameplay/ai/workspaces],
   AiPrimaryWorkspace_HasUnassignedEntryById [gameplay/ai/workspaces],
   AiCandidateWorkspace_AddOrAccumulateWeightedEntry [gameplay/ai/workspaces].
*/
void __thandor_void_preserve_eax_ecx_edx
AiResourceCandidate_AddWeightedId136(FactionRuntimeIndex factionIndex)

{
  int energySupply;
  int energySurplus;
  int energyDemand;
  int generationCapacity;
  bool conditionMet;
  
  conditionMet = AiPrimaryWorkspace_HasEntryById(ARM_0330_BUILDING_MDL0303);
  if (conditionMet) {
    conditionMet = AiPrimaryWorkspace_HasUnassignedEntryById(ARM_0310_BUILDING_MDL0305);
    if (!conditionMet) {
      generationCapacity = (int)g_GameFactionRuntimeImage.records[factionIndex].energyGenerationCapacityQ4 >> 4;
      energyDemand = (int)(g_GameFactionRuntimeImage.records[factionIndex].suppliedEnergyDemandQ4 +
                   g_GameFactionRuntimeImage.records[factionIndex].unpoweredEnergyDemandQ4) >> 4;
      if (energyDemand == 0) {
        energyDemand = 1;
      }
      energySupply = ((int)g_GameFactionRuntimeImage.records[factionIndex].baselineEnergySupplyQ4 >> 4) +
              g_GameFactionRuntimeImage.records[factionIndex].tritiumExtractionRateQ4PerTick;
      energySurplus = energySupply - generationCapacity;
      if (energySurplus != 0 && generationCapacity <= energySupply) {
        AiCandidateWorkspace_AddOrAccumulateWeightedEntry
                  (0x136,(uint32_t)(((int64_t)
                                  (int)(((int64_t)energySurplus * (int64_t)energyDemand) / (int64_t)generationCapacity) *
                                 (int64_t)
                                 (int)(g_AiKnowledgeData->parameters).
                                      resource136DeficitScoreNumerator) /
                                (int64_t)
                                (int)(g_AiKnowledgeData->parameters).
                                     resource136DeficitScoreDenominator),1);
      }
    }
  }
  return;
}


/* Address: 0x0053A9D0.
   Ownership: gameplay/ai/planning.
   Purpose: Evaluates strategic class IDs 0x12E through 0x132 using faction category totals, class-specific
   coefficient tables, current workspace presence, and randomized tie variation. The best available class and
   comparison state are returned through the engine register convention.
   Cross-module calls: AiPrimaryWorkspace_HasEntryById [gameplay/ai/workspaces],
   ArmyAssetRegistry_FindEnabledById [assets/army/catalog].
*/
AiStrategicClassSelectionRegs8 __thandor_regs_ebx_ecx_preserve_eax_edx
AiStrategicClass_SelectBestCandidate12ETo132
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  GridScratchStateMask cellStateMask;
  uint32_t pendingExistingCount;
  int freeBits25To27PercentOrScore;
  int freeBit24Percent;
  int freeBits28To30Percent;
  uint32_t cellCount;
  uint32_t remainingCellsOrTieBits;
  uint32_t freeBits25To27Cells;
  int candidateScore;
  uint32_t freeBit24Cells;
  GridScratchCell *scratchCell;
  uint32_t freeBits28To30Cells;
  bool conditionMet;
  AiStrategicClassSelectionRegs8 selection;
  uint32_t existingClassCount;
  uint32_t randomizedTieBits;
  RuntimeToken selectedToken;
  int bestCandidateScore;
  
  cellCount = g_GridScratchWidth * g_GridScratchHeight;
  freeBit24Cells = 0;
  freeBits25To27Cells = 0;
  freeBits28To30Cells = 0;
  remainingCellsOrTieBits = cellCount;
  scratchCell = g_GridScratchPrimary;
  do {
    cellStateMask = scratchCell->stateMask;
    if ((cellStateMask & GRID_SCRATCH_TERRAIN_CLASS_BIT24) == 0) {
      freeBit24Cells = freeBit24Cells + 1;
    }
    if ((cellStateMask & (GRID_SCRATCH_TERRAIN_CLASS_BIT27|GRID_SCRATCH_TERRAIN_CLASS_BIT26|
                 GRID_SCRATCH_TERRAIN_CLASS_BIT25)) == 0) {
      freeBits25To27Cells = freeBits25To27Cells + 1;
    }
    if ((cellStateMask & (GRID_SCRATCH_TERRAIN_CLASS_BIT30|GRID_SCRATCH_TERRAIN_CLASS_BIT29|
                 GRID_SCRATCH_TERRAIN_CLASS_BIT28)) == 0) {
      freeBits28To30Cells = freeBits28To30Cells + 1;
    }
    scratchCell = scratchCell + 1;
    remainingCellsOrTieBits = remainingCellsOrTieBits - 1;
  } while (remainingCellsOrTieBits != 0);
  freeBits25To27PercentOrScore = (int)(((uint64_t)freeBits25To27Cells * 100) / (uint64_t)cellCount);
  freeBit24Percent = (int)(((uint64_t)freeBit24Cells * 100) / (uint64_t)cellCount);
  freeBits28To30Percent = (int)(((uint64_t)freeBits28To30Cells * 100) / (uint64_t)cellCount);
  randomizedTieBits = g_RandomGeneratorState.next();
  bestCandidateScore = 0;
  selectedToken = 0;
  existingClassCount = 5;
  conditionMet = AiPrimaryWorkspace_HasEntryById(ARM_0302_BUILDING_MDL0300);
  pendingExistingCount = existingClassCount;
  if (!conditionMet) {
    existingClassCount = 4;
    conditionMet = ArmyAssetRegistry_FindEnabledById(ARM_0302_BUILDING_MDL0300);
    pendingExistingCount = 4;
    if (!conditionMet) {
      selectedToken = 0x12e;
      remainingCellsOrTieBits = randomizedTieBits & 0x3fff;
      randomizedTieBits = randomizedTieBits >> 5;
      bestCandidateScore =
           freeBits28To30Percent * g_AiStrategicClassTerrainWeights[0][0] +
           freeBits25To27PercentOrScore * g_AiStrategicClassTerrainWeights[0][1] +
           freeBit24Percent * g_AiStrategicClassTerrainWeights[0][2] + remainingCellsOrTieBits;
      pendingExistingCount = existingClassCount;
    }
  }
  existingClassCount = pendingExistingCount;
  conditionMet = AiPrimaryWorkspace_HasEntryById(ARM_0303_BUILDING_MDL0316);
  if (!conditionMet) {
    existingClassCount = existingClassCount - 1;
    conditionMet = ArmyAssetRegistry_FindEnabledById(ARM_0303_BUILDING_MDL0316);
    if (!conditionMet) {
      remainingCellsOrTieBits = randomizedTieBits & 0x3fff;
      randomizedTieBits = randomizedTieBits >> 5;
      candidateScore = freeBits28To30Percent * g_AiStrategicClassTerrainWeights[1][0] +
              freeBits25To27PercentOrScore * g_AiStrategicClassTerrainWeights[1][1] +
              freeBit24Percent * g_AiStrategicClassTerrainWeights[1][2] + remainingCellsOrTieBits;
      if (bestCandidateScore < candidateScore) {
        selectedToken = 0x12f;
        bestCandidateScore = candidateScore;
      }
    }
  }
  conditionMet = AiPrimaryWorkspace_HasEntryById(ARM_0304_BUILDING_MDL0324);
  if (!conditionMet) {
    existingClassCount = existingClassCount - 1;
    conditionMet = ArmyAssetRegistry_FindEnabledById(ARM_0304_BUILDING_MDL0324);
    if (!conditionMet) {
      remainingCellsOrTieBits = randomizedTieBits & 0x3fff;
      randomizedTieBits = randomizedTieBits >> 5;
      candidateScore = freeBits28To30Percent * g_AiStrategicClassTerrainWeights[2][0] +
              freeBits25To27PercentOrScore * g_AiStrategicClassTerrainWeights[2][1] +
              freeBit24Percent * g_AiStrategicClassTerrainWeights[2][2] + remainingCellsOrTieBits;
      if (bestCandidateScore < candidateScore) {
        selectedToken = 0x130;
        bestCandidateScore = candidateScore;
      }
    }
  }
  conditionMet = AiPrimaryWorkspace_HasEntryById(ARM_0305_BUILDING_MDL0317);
  if (!conditionMet) {
    existingClassCount = existingClassCount - 1;
    conditionMet = ArmyAssetRegistry_FindEnabledById(ARM_0305_BUILDING_MDL0317);
    if (!conditionMet) {
      freeBits25To27PercentOrScore = freeBits28To30Percent * g_AiStrategicClassTerrainWeights[3][0] +
              freeBits25To27PercentOrScore * g_AiStrategicClassTerrainWeights[3][1] +
              freeBit24Percent * g_AiStrategicClassTerrainWeights[3][2] + (randomizedTieBits & 0x3fff);
      if (bestCandidateScore < freeBits25To27PercentOrScore) {
        selectedToken = 0x131;
        bestCandidateScore = freeBits25To27PercentOrScore;
      }
    }
  }
  conditionMet = AiPrimaryWorkspace_HasEntryById(ARM_0306_BUILDING_MDL0310);
  if (!conditionMet) {
    existingClassCount = existingClassCount - 1;
    conditionMet = ArmyAssetRegistry_FindEnabledById(ARM_0306_BUILDING_MDL0310);
    if (!conditionMet) {
      /* the original also scores class 0x132 with g_AiStrategicClassTerrainWeights[4] but
         discards the result (XOR EDX,EDX): 0x132 wins only over a negative best score */
      if (bestCandidateScore < 0) {
        selectedToken = 0x132;
      }
    }
  }
  selection.selectedRuntimeToken = selectedToken;
  selection.existingCountOrPressure = existingClassCount;
  return selection;
}


/* Address: 0x0053AF00.
   Ownership: gameplay/ai/planning.
   Purpose: Selects among strategic class IDs 0x141 through 0x143 using faction category totals, knowledge
   coefficients, current workspace entries, and randomized variation. The selected class and existing-count state
   are returned through the original register convention. It is distinct from FrontendPlayerIndex_V306,
   PlayerRuntimeId, active-faction masks or codes, and PCK-backed ArmyAssetId, ModelDefinitionId, and TechnologyId
   domains.
   Cross-module calls: AiPrimaryWorkspace_HasEntryById [gameplay/ai/workspaces],
   ArmyAssetRegistry_FindEnabledById [assets/army/catalog].
*/
AiStrategicClassSelectionRegs8 __thandor_regs_ebx_ecx_preserve_eax_edx
AiStrategicClass_SelectWeightedClass141To143
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  int pressure2For141;
  int pressure3For141;
  int pressure4For141;
  uint32_t class141Coefficient1;
  uint32_t class141Coefficient2;
  int pressure2For142;
  int pressure3For142;
  int pressure4For142;
  uint32_t class142Coefficient0;
  uint32_t class142Coefficient1;
  uint32_t class142Coefficient2;
  int pressure2For143;
  int pressure3For143;
  int pressure4For143;
  uint32_t class143Coefficient0;
  uint32_t class143Coefficient1;
  uint32_t class143Coefficient2;
  uint32_t randomBits;
  RuntimeToken selectedToken;
  uint32_t pressureSumPlusOne;
  uint32_t class141Coefficient0OrExistingCount;
  uint32_t class141Score;
  uint32_t bestScore;
  uint32_t class142Score;
  bool conditionMet;
  AiStrategicClassSelectionRegs8 selection;
  
  pressure2For141 = g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[2];
  pressure3For141 = g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[3];
  pressure4For141 = g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[4];
  class141Coefficient0OrExistingCount = (g_AiKnowledgeData->parameters).unknownParameterDwords90_119[0x1a];
  class141Coefficient1 = (g_AiKnowledgeData->parameters).unknownParameterDwords90_119[0x1b];
  class141Coefficient2 = (g_AiKnowledgeData->parameters).unknownParameterDwords90_119[0x1c];
  pressure2For142 = g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[2];
  pressure3For142 = g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[3];
  pressure4For142 = g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[4];
  class142Coefficient0 = (g_AiKnowledgeData->parameters).strategicClass141Weight;
  class142Coefficient1 = (g_AiKnowledgeData->parameters).strategicClass142Weight;
  class142Coefficient2 = (g_AiKnowledgeData->parameters).strategicClass143Weight;
  pressure2For143 = g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[2];
  pressure3For143 = g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[3];
  pressure4For143 = g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[4];
  class143Coefficient0 = (g_AiKnowledgeData->parameters).unknownParameterDwords123_127[1];
  class143Coefficient1 = (g_AiKnowledgeData->parameters).unknownParameterDwords123_127[2];
  class143Coefficient2 = (g_AiKnowledgeData->parameters).unknownParameterDwords123_127[3];
  pressureSumPlusOne = g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[2] +
           g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[3] +
           g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[4] + 1;
  randomBits = g_RandomGeneratorState.next();
  class141Score = ((pressure2For141 + 1) * class141Coefficient0OrExistingCount + (pressure3For141 + 1) * class141Coefficient1 + (pressure4For141 + 1) * class141Coefficient2) / pressureSumPlusOne +
           (randomBits & 0x7f);
  class142Score = ((pressure2For142 + 1) * class142Coefficient0 + (pressure3For142 + 1) * class142Coefficient1 + (pressure4For142 + 1) * class142Coefficient2) / pressureSumPlusOne +
           (randomBits >> 0x13 & 0x7f);
  bestScore = 0;
  class141Coefficient0OrExistingCount = 3;
  selectedToken = 0;
  conditionMet = AiPrimaryWorkspace_HasEntryById(ARM_0321_BUILDING_MDL0326);
  if (!conditionMet) {
    class141Coefficient0OrExistingCount = 2;
    conditionMet = ArmyAssetRegistry_FindEnabledById(ARM_0321_BUILDING_MDL0326);
    if ((!conditionMet) && (class141Score != 0)) {
      selectedToken = 0x141;
      bestScore = class141Score;
    }
  }
  conditionMet = AiPrimaryWorkspace_HasEntryById(ARM_0322_BUILDING_MDL0327);
  if (!conditionMet) {
    class141Coefficient0OrExistingCount = class141Coefficient0OrExistingCount - 1;
    conditionMet = ArmyAssetRegistry_FindEnabledById(ARM_0322_BUILDING_MDL0327);
    if ((!conditionMet) && (bestScore < class142Score)) {
      selectedToken = 0x142;
      bestScore = class142Score;
    }
  }
  conditionMet = AiPrimaryWorkspace_HasEntryById(ARM_0323_BUILDING_MDL0328);
  if (!conditionMet) {
    class141Coefficient0OrExistingCount = class141Coefficient0OrExistingCount - 1;
    conditionMet = ArmyAssetRegistry_FindEnabledById(ARM_0323_BUILDING_MDL0328);
    if ((!conditionMet) &&
       (bestScore < ((pressure2For143 + 1) * class143Coefficient0 + (pressure3For143 + 1) * class143Coefficient1 + (pressure4For143 + 1) * class143Coefficient2) / pressureSumPlusOne +
                 (randomBits >> 7 & 0x7f))) {
      selectedToken = 0x143;
    }
  }
  selection.selectedRuntimeToken = selectedToken;
  selection.existingCountOrPressure = class141Coefficient0OrExistingCount;
  return selection;
}


/* Address: 0x00539600.
   Ownership: gameplay/ai/planning.
   Purpose: Places an army asset by scoring workspace10 points with randomized tie variation and a connected-region
   reachability test. The chosen point is instantiated and initialized before the pending faction asset is
   consumed. Stock ARM contains 675 records and 326 unique ids; placement workspace, producer, tier, class, and
   faction-role semantics are not inferred from numeric adjacency.
   Local calls: AiConstructionPlanner_ConsumeFactionPendingArmyAsset.
   Cross-module calls: ArmyAssetRegistry_FindById [assets/army/catalog],
   ModelDefinitionRegistry_FindByIdWithError [assets/model/definitions], ArmyPlacement_DispatchAssetAtFieldPoint
   [gameplay/army/placement], GridReachability_RebuildConnectedRegionAroundWorldPoint [world/pathing/grid],
   ArmyRuntime_CreateInstanceFromAsset [gameplay/army/runtime], ModelNodeRuntime_RebuildTransformsFromRoot
   [world/model/hierarchy].
*/
void __thandor_void_preserve_eax_ecx_edx
AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate
          (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime)

{
  uint32_t radiusMetric;
  FieldGridCell *candidateCell;
  ArmyRuntimeSlot *modelNodeRuntime;
  ArmyRuntimeSlot *createdArmySlot;
  ModelRuntimeSlot *createdModelRuntime;
  int distanceXOrScore;
  uint32_t randomBits;
  ArmyRuntimeSlot **createdSlots;
  int remainingCells;
  int distanceY;
  FieldGridCell **gridCellCursor;
  bool regionUnreachable;
  ArmyAssetLookupResult armyAssetLookup;
  ModelDefinitionResult modelDefinitionLookup;
  PlacementDispatchResult placementResult;
  ArmyRuntimeCreateResult createdInstance;
  FieldGridCell *bestCell;
  int bestScore;
  
  armyAssetLookup = ArmyAssetRegistry_FindById(armyAssetId);
  if ((!armyAssetLookup.notFound) && (g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorCooldown == 0)
     ) {
    modelDefinitionLookup = ModelDefinitionRegistry_FindByIdWithError
                       (*(PckModelDefinitionIdCatalog *)
                         ((armyAssetLookup.recordOrError)->rootNodeOffsetOrPointer + 0x20));
    if (!modelDefinitionLookup.notFound) {
      radiusMetric = modelDefinitionLookup.modelDefinition[0x12].flags;
      if (g_AiWorkspace10Count != 0) {
        bestScore = 0x7fffffff;
        remainingCells = g_AiWorkspace10Count;
        gridCellCursor = g_AiWorkspaceBuffer10_Size0400;
        do {
          candidateCell = *gridCellCursor;
          distanceXOrScore = candidateCell->worldX;
          distanceY = candidateCell->worldY;
          if (g_AiWorkspaceOwnedAsset300Runtime != (ArmyRuntimeSlot *)0x0) {
            distanceXOrScore = distanceXOrScore - (g_AiWorkspaceOwnedAsset300Runtime->modelNodeRuntime->worldTransform).
                            translation.x;
            if (distanceXOrScore < 0) {
              distanceXOrScore = -distanceXOrScore;
            }
            distanceY = distanceY - (g_AiWorkspaceOwnedAsset300Runtime->modelNodeRuntime->worldTransform).
                            translation.y;
            if (distanceY < 0) {
              distanceY = -distanceY;
            }
          }
          randomBits = g_RandomGeneratorState.next();
          distanceXOrScore = distanceY + distanceXOrScore + (randomBits & 0xffff);
          if (distanceXOrScore < bestScore) {
            placementResult = ArmyPlacement_DispatchAssetAtFieldPoint
                               (1,0,(uint32_t)(uint16_t)candidateCell->triangle0NormalAngles,candidateCell->worldY,
                                candidateCell->worldX,armyAssetId,factionIndex,(UiRootNode *)worldRuntime);
            if (!placementResult.failed) {
              regionUnreachable = GridReachability_RebuildConnectedRegionAroundWorldPoint
                                (radiusMetric,candidateCell->worldY,candidateCell->worldX);
              if (!regionUnreachable) {
                bestCell = candidateCell;
                bestScore = distanceXOrScore;
              }
            }
          }
          gridCellCursor = gridCellCursor + 1;
          remainingCells = remainingCells + -1;
        } while (remainingCells != 0);
        if (bestScore < 0x7fffffff) {
          createdInstance = ArmyRuntime_CreateInstanceFromAsset
                             (4,(uint32_t)(uint16_t)bestCell->triangle0NormalAngles,bestCell->worldY,
                              bestCell->worldX,factionIndex,armyAssetId,worldRuntime);
          createdSlots = (ArmyRuntimeSlot **)createdInstance.armyRuntimeOrError;
          if (!createdInstance.failed) {
            modelNodeRuntime = createdSlots[1];
            createdArmySlot = *createdSlots;
            modelNodeRuntime->movementPosition0Q12 = 0;
            createdModelRuntime = (createdArmySlot->modelRuntimeOrSavedOffset).modelRuntime;
            ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)modelNodeRuntime);
            ArmyRuntime_DispatchClassCommand(createdSlots,worldRuntime);
            EffectRuntimePool_CreateInstanceFromDefinition
                      (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference4, 0x0),
                       (modelNodeRuntime->movementControl).turnVelocityAngle16,
                       (modelNodeRuntime->movementControl).movementAdvancePerTickQ12,
                       ((WorldRuntimeNodeModelPayload *)&modelNodeRuntime->factionIndex)->
                       worldRotationAngle0,modelNodeRuntime->depthBinClass,
                       modelNodeRuntime->runtimeState98,
                       ((GraphicsFixedVec3 *)&modelNodeRuntime->runtimeState94)->x,
                       (EffectDefinition *)createdModelRuntime->attachments140[2].childLocalRotationAngle0,
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
   Cross-module calls: ArmyAssetRegistry_FindById [assets/army/catalog].
*/
void __thandor_void_preserve_eax_ecx
AiConstructionPlanner_ConsumeFactionPendingArmyAsset
          (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex)

{
  FactionRelationCounter *relationCounter;
  FactionArmyAssetCount *pendingAssetCount;
  FactionArmyAssetCount remainingAssets;
  uint32_t *assetPointerCursor;
  ArmyAssetLookupResult armyAssetLookup;
  
  g_AiConstructionPendingAssetConsumedCount = g_AiConstructionPendingAssetConsumedCount + 1;
  remainingAssets = g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
  armyAssetLookup = ArmyAssetRegistry_FindById(armyAssetId);
  assetPointerCursor = g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetPointersOrIds;
  relationCounter = &g_GameFactionRuntimeImage.records[factionIndex].relationCounterB;
  *relationCounter = *relationCounter + 1;
  while( true ) {
    if (remainingAssets == 0) {
      return;
    }
    if (armyAssetLookup.recordOrError == (ArmyAssetRecordPrefix *)*assetPointerCursor) break;
    assetPointerCursor = assetPointerCursor + 1;
    remainingAssets = remainingAssets - 1;
  }
  pendingAssetCount = &g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
  *pendingAssetCount = *pendingAssetCount - 1;
  while (remainingAssets = remainingAssets - 1, remainingAssets != 0) {
    *assetPointerCursor = assetPointerCursor[1];
    assetPointerCursor = assetPointerCursor + 1;
  }
  return;
}


/* Address: 0x0053A980.
   Ownership: gameplay/ai/planning.
   Purpose: Handles ai faction runtime test planning capacity exceeded carry-flag result.
*/
bool __thandor_cf_preserve_eax_ecx_edx
AiFactionRuntime_TestPlanningCapacityExceeded
          (uint32_t additionalPlanningCapacity,FactionRuntimeIndex factionIndex)

{
  int supplyCapacity;
  int effectiveCapacity;
  
  supplyCapacity = ((int)g_GameFactionRuntimeImage.records[factionIndex].baselineEnergySupplyQ4 >> 4) +
          g_GameFactionRuntimeImage.records[factionIndex].tritiumExtractionRateQ4PerTick;
  effectiveCapacity = (int)g_GameFactionRuntimeImage.records[factionIndex].energyGenerationCapacityQ4 >> 4;
  if (supplyCapacity < effectiveCapacity) {
    effectiveCapacity = supplyCapacity;
  }
  return effectiveCapacity < (int)(((int)(g_GameFactionRuntimeImage.records[factionIndex].suppliedEnergyDemandQ4
                             + g_GameFactionRuntimeImage.records[factionIndex].
                               unpoweredEnergyDemandQ4) >> 4) + additionalPlanningCapacity);
}


/* Address: 0x0053A2A0.
   Ownership: gameplay/ai/planning.
   Purpose: Computes a composite AI score for one army candidate. Stock tech.tec has 512 records over canonical ids
   0..255; localized titles do not prove source-building, tier, direction, or effect mappings. Typed parameters: p4
   armyAssetRecord→AiArmyAssetRecordAddress32_V345. Calling convention, complete VariableStorage serialization,
   function bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: ModelDefinition_SelectFactionUnlockedLinkedDefinition [assets/model/definitions].
*/
AiCandidateScore32 __thandor_eax_preserve_ecx_edx
AiArmyCandidate_ComputeFactionWeightedScore
          (AiArmyScoreWeights *scoreWeights,FactionRuntimeIndex factionIndex,
          ArmyAssetRuntimeSemanticView80 *armyAssetRecord)

{
  ModelDefinitionResolvePhaseView280 *selectedModelDefinition;
  ModelDefinitionResolvePhaseView280 *selectedChildModelDefinition0;
  uint32_t pressureWeightedDamage0;
  uint32_t pressureWeightedDamage1;
  uint32_t pressureWeightedDamage2;
  uint32_t pressureWeightedDamage3;
  uint32_t pressureWeightedDamage4;
  uint32_t pressureWeightedDamage5;
  uint32_t pressureWeightedDamage6;
  uint32_t pressureWeightedDamage7;
  ModelDefinitionResolvePhaseView280 *selectedChildModelDefinition1;
  uint32_t childCountOrWeightedDamage;
  int weightedDefinitionScore;
  ModelDefinitionResult definitionLookup;
  AiLinkedDefinitionListView *linkedDefinitionList;
  int secondChildScaleDivisor30;
  ShotDefinition *selectedShotDefinition;
  ShotDefinition *secondChildShotDefinition;
  int definitionScaleDivisor30;
  
  linkedDefinitionList = (AiLinkedDefinitionListView *)armyAssetRecord->rootNodeOffsetOrPointer;
  definitionLookup = ModelDefinition_SelectFactionUnlockedLinkedDefinition
                    (factionIndex,(ModelLinkedDefinitionListAddress32)linkedDefinitionList);
  selectedModelDefinition = (ModelDefinitionResolvePhaseView280 *)definitionLookup.modelDefinition;
  if (definitionLookup.notFound) {
    return 0;
  }
  weightedDefinitionScore = scoreWeights->baseScore;
  if (selectedModelDefinition->runtimeValue18 != 0) {
    weightedDefinitionScore = weightedDefinitionScore + scoreWeights->nonzeroDefinition18Bonus;
  }
  childCountOrWeightedDamage = linkedDefinitionList->childListCount;
  weightedDefinitionScore =
       ((int)(((int64_t)(int)selectedModelDefinition->runtimeValue0C *
              (int64_t)scoreWeights->definitionValue0CWeight) /
             (int64_t)(int)g_AiArmyCandidateFlaggedDefinitionValueMaximum) +
       weightedDefinitionScore +
       (int)(((int64_t)(int)selectedModelDefinition->runtimeValue60 *
             (int64_t)scoreWeights->definitionValue60Weight) /
            (int64_t)
            (&g_TechnologyCategoryMaximum0)[selectedModelDefinition->categoryMaximumIndex5C])) * 8;
  if (childCountOrWeightedDamage != 0) {
    definitionLookup = ModelDefinition_SelectFactionUnlockedLinkedDefinition
                      (factionIndex,linkedDefinitionList->childList0Address);
    selectedChildModelDefinition0 = (ModelDefinitionResolvePhaseView280 *)definitionLookup.modelDefinition;
    if (definitionLookup.notFound) {
      return 0;
    }
    if (selectedChildModelDefinition0->runtimeValue30 != 0) {
      selectedShotDefinition = selectedChildModelDefinition0->shotDefinitionReference2C;
      definitionScaleDivisor30 = selectedChildModelDefinition0->runtimeValue30;
      if (g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure != 0) {
        pressureWeightedDamage0 =
             (uint32_t)(((int64_t)
                     (int)(((int64_t)selectedShotDefinition->targetClassImpactDamageQ12[0] *
                           (int64_t)scoreWeights->pressureCategoryWeights[0]) /
                          (int64_t)definitionScaleDivisor30) *
                    (int64_t)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[0]) /
                   (int64_t)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
        pressureWeightedDamage1 =
             (uint32_t)(((int64_t)
                     (int)(((int64_t)selectedShotDefinition->targetClassImpactDamageQ12[1] *
                           (int64_t)scoreWeights->pressureCategoryWeights[1]) /
                          (int64_t)definitionScaleDivisor30) *
                    (int64_t)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[1]) /
                   (int64_t)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
        pressureWeightedDamage2 =
             (uint32_t)(((int64_t)
                     (int)(((int64_t)selectedShotDefinition->targetClassImpactDamageQ12[2] *
                           (int64_t)scoreWeights->pressureCategoryWeights[2]) /
                          (int64_t)definitionScaleDivisor30) *
                    (int64_t)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[2]) /
                   (int64_t)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
        pressureWeightedDamage3 =
             (uint32_t)(((int64_t)
                     (int)(((int64_t)selectedShotDefinition->targetClassImpactDamageQ12[3] *
                           (int64_t)scoreWeights->pressureCategoryWeights[3]) /
                          (int64_t)definitionScaleDivisor30) *
                    (int64_t)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[3]) /
                   (int64_t)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
        pressureWeightedDamage4 =
             (uint32_t)(((int64_t)
                     (int)(((int64_t)selectedShotDefinition->targetClassImpactDamageQ12[4] *
                           (int64_t)scoreWeights->pressureCategoryWeights[4]) /
                          (int64_t)definitionScaleDivisor30) *
                    (int64_t)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[4]) /
                   (int64_t)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
        pressureWeightedDamage5 =
             (uint32_t)(((int64_t)
                     (int)(((int64_t)selectedShotDefinition->targetClassImpactDamageQ12[5] *
                           (int64_t)scoreWeights->pressureCategoryWeights[5]) /
                          (int64_t)definitionScaleDivisor30) *
                    (int64_t)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[5]) /
                   (int64_t)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
        pressureWeightedDamage6 =
             (uint32_t)(((int64_t)
                     (int)(((int64_t)selectedShotDefinition->targetClassImpactDamageQ12[6] *
                           (int64_t)scoreWeights->pressureCategoryWeights[6]) /
                          (int64_t)definitionScaleDivisor30) *
                    (int64_t)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[6]) /
                   (int64_t)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
        pressureWeightedDamage7 =
             (uint32_t)(((int64_t)
                     (int)(((int64_t)selectedShotDefinition->targetClassImpactDamageQ12[7] *
                           (int64_t)scoreWeights->pressureCategoryWeights[7]) /
                          (int64_t)definitionScaleDivisor30) *
                    (int64_t)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[7]) /
                   (int64_t)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
        weightedDefinitionScore =
             weightedDefinitionScore +
             (int)((int64_t)((uint64_t)pressureWeightedDamage0 << 10) /
                  (int64_t)(int)g_TechnologyCategoryMaximum0) +
             (int)((int64_t)((uint64_t)pressureWeightedDamage1 << 10) /
                  (int64_t)(int)g_TechnologyCategoryMaximum1) +
             (int)((int64_t)((uint64_t)pressureWeightedDamage2 << 10) /
                  (int64_t)(int)g_TechnologyCategoryMaximum2) +
             (int)((int64_t)((uint64_t)pressureWeightedDamage3 << 10) /
                  (int64_t)(int)g_TechnologyCategoryMaximum3) +
             (int)((int64_t)((uint64_t)pressureWeightedDamage4 << 10) /
                  (int64_t)(int)g_TechnologyCategoryMaximum4) +
             (int)((int64_t)((uint64_t)pressureWeightedDamage5 << 10) /
                  (int64_t)(int)g_TechnologyCategoryMaximum5) +
             (int)((int64_t)((uint64_t)pressureWeightedDamage6 << 10) /
                  (int64_t)(int)g_TechnologyCategoryMaximum6) +
             (int)((int64_t)((uint64_t)pressureWeightedDamage7 << 10) /
                  (int64_t)(int)g_TechnologyCategoryMaximum7);
      }
    }
    if (1 < childCountOrWeightedDamage) {
      definitionLookup = ModelDefinition_SelectFactionUnlockedLinkedDefinition
                        (factionIndex,linkedDefinitionList->childList1Address);
      selectedChildModelDefinition1 = (ModelDefinitionResolvePhaseView280 *)definitionLookup.modelDefinition;
      if (definitionLookup.notFound) {
        return 0;
      }
      if (selectedChildModelDefinition1->runtimeValue30 != 0) {
        secondChildShotDefinition = selectedChildModelDefinition1->shotDefinitionReference2C;
        secondChildScaleDivisor30 = selectedChildModelDefinition1->runtimeValue30;
        if (g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure != 0) {
          childCountOrWeightedDamage = (uint32_t)(((int64_t)
                          (int)(((int64_t)secondChildShotDefinition->targetClassImpactDamageQ12[0]
                                * (int64_t)scoreWeights->pressureCategoryWeights[0]) /
                               (int64_t)secondChildScaleDivisor30) *
                         (int64_t)
                         g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[0]) /
                        (int64_t)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure)
          ;
          pressureWeightedDamage0 =
               (uint32_t)(((int64_t)
                       (int)(((int64_t)secondChildShotDefinition->targetClassImpactDamageQ12[1] *
                             (int64_t)scoreWeights->pressureCategoryWeights[1]) /
                            (int64_t)secondChildScaleDivisor30) *
                      (int64_t)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[1])
                     / (int64_t)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
          pressureWeightedDamage1 =
               (uint32_t)(((int64_t)
                       (int)(((int64_t)secondChildShotDefinition->targetClassImpactDamageQ12[2] *
                             (int64_t)scoreWeights->pressureCategoryWeights[2]) /
                            (int64_t)secondChildScaleDivisor30) *
                      (int64_t)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[2])
                     / (int64_t)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
          pressureWeightedDamage2 =
               (uint32_t)(((int64_t)
                       (int)(((int64_t)secondChildShotDefinition->targetClassImpactDamageQ12[3] *
                             (int64_t)scoreWeights->pressureCategoryWeights[3]) /
                            (int64_t)secondChildScaleDivisor30) *
                      (int64_t)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[3])
                     / (int64_t)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
          pressureWeightedDamage3 =
               (uint32_t)(((int64_t)
                       (int)(((int64_t)secondChildShotDefinition->targetClassImpactDamageQ12[4] *
                             (int64_t)scoreWeights->pressureCategoryWeights[4]) /
                            (int64_t)secondChildScaleDivisor30) *
                      (int64_t)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[4])
                     / (int64_t)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
          pressureWeightedDamage4 =
               (uint32_t)(((int64_t)
                       (int)(((int64_t)secondChildShotDefinition->targetClassImpactDamageQ12[5] *
                             (int64_t)scoreWeights->pressureCategoryWeights[5]) /
                            (int64_t)secondChildScaleDivisor30) *
                      (int64_t)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[5])
                     / (int64_t)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
          pressureWeightedDamage5 =
               (uint32_t)(((int64_t)
                       (int)(((int64_t)secondChildShotDefinition->targetClassImpactDamageQ12[6] *
                             (int64_t)scoreWeights->pressureCategoryWeights[6]) /
                            (int64_t)secondChildScaleDivisor30) *
                      (int64_t)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[6])
                     / (int64_t)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
          pressureWeightedDamage6 =
               (uint32_t)(((int64_t)
                       (int)(((int64_t)secondChildShotDefinition->targetClassImpactDamageQ12[7] *
                             (int64_t)scoreWeights->pressureCategoryWeights[7]) /
                            (int64_t)secondChildScaleDivisor30) *
                      (int64_t)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[7])
                     / (int64_t)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
          weightedDefinitionScore =
               weightedDefinitionScore +
               (int)((int64_t)((uint64_t)childCountOrWeightedDamage << 10) / (int64_t)(int)g_TechnologyCategoryMaximum0) +
               (int)((int64_t)((uint64_t)pressureWeightedDamage0 << 10) /
                    (int64_t)(int)g_TechnologyCategoryMaximum1) +
               (int)((int64_t)((uint64_t)pressureWeightedDamage1 << 10) /
                    (int64_t)(int)g_TechnologyCategoryMaximum2) +
               (int)((int64_t)((uint64_t)pressureWeightedDamage2 << 10) /
                    (int64_t)(int)g_TechnologyCategoryMaximum3) +
               (int)((int64_t)((uint64_t)pressureWeightedDamage3 << 10) /
                    (int64_t)(int)g_TechnologyCategoryMaximum4) +
               (int)((int64_t)((uint64_t)pressureWeightedDamage4 << 10) /
                    (int64_t)(int)g_TechnologyCategoryMaximum5) +
               (int)((int64_t)((uint64_t)pressureWeightedDamage5 << 10) /
                    (int64_t)(int)g_TechnologyCategoryMaximum6) +
               (int)((int64_t)((uint64_t)pressureWeightedDamage6 << 10) /
                    (int64_t)(int)g_TechnologyCategoryMaximum7);
        }
      }
    }
  }
  return scoreWeights->armyRecord74Weight * armyAssetRecord->definitionClassValue74 +
         weightedDefinitionScore * 0xc +
         scoreWeights->armyRecord78Weight * armyAssetRecord->definitionClassValue78 +
         scoreWeights->armyRecord70Weight * armyAssetRecord->definitionClassValue70;
}

