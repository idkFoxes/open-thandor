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
  WorldOwnerListNode *ownerNode;
  
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
  } while (remainingFactionsOrPressureValue != 0);
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
        /* runtimePayload is the ModelRuntimeSlot: its definition's target class is the pressure channel;
           the owner army holds the owning faction and a faction mask at +0x50 (bit 3 + 2 * (f - 1) for
           faction f). */
        factionIndexOrScratch =
             (int)((ModelRuntimeSlot *)ownerNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime;
        if (((ArmyRuntimeSlot *)factionIndexOrScratch)->factionIndex != 0) {
          remainingFactionsOrPressureValue =
               ((ModelRuntimeSlot *)ownerNode->runtimePayload)->definitionOrSavedId.runtimeDefinition->
               targetClassIndex5C;
          nextChannelOrFactionBit = 8;
          channelOrFactionIndex = 1;
          pressureTargetRecord = g_GameFactionRuntimeImage.records;
          do {
            pressureTargetRecord++;
            if (((((ArmyRuntimeSlot *)factionIndexOrScratch)->terrainOccupancyMask0 & nextChannelOrFactionBit) != 0) &&
               (channelOrFactionIndex != (uint32_t)((ArmyRuntimeSlot *)factionIndexOrScratch)->factionIndex)) {
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


/* Address: 0x0053BA50.
   Average faction-weighted score (AiArmyCandidate_ComputeFactionWeightedScore) of the enabled army assets
   that can carry the model definition of runtimeClassId: those whose own linked-definition list names it,
   or, once the technology of the asset's first linked definition is unlocked, one of its (up to two) child
   lists. Only positive scores count; 0 when the class has no definition or no asset scores. Only caller:
   AiTechnologyScore_ComputeCategoryCompatibleCandidateValue, which passes a technology id as runtimeClassId.
*/
AiCandidateScore32
AiArmyCandidate_ComputeAverageCompatibleAssetScore
          (AiArmyScoreWeights *scoreWeights,FactionRuntimeIndex factionIndex,
          ModelRuntimeClassId runtimeClassId)

{
  ArmyAssetRecord *armyAssetRecord;
  uint32_t assetDefinitionListAddress;
  ModelDefinitionRecordPrefix *modelDefinition;
  AiCandidateScore32 candidateScore;
  int registryEntriesRemaining;
  ArmyAssetRecordPrefix **armyAssetRegistryCursor;
  bool technologyLocked;
  int compatibleAssetCount;
  uint32_t compatibleAssetScoreSum;
  AiLinkedDefinitionListView *nestedLinkedDefinitions;
  PckModelDefinitionIdCatalog candidateModelDefinitionId;
  AiLinkedDefinitionListView *secondNestedLinkedDefinitions;
  
  modelDefinition = ModelDefinitionRegistry_FindByRuntimeClassId(runtimeClassId);
  candidateScore = 0;
  if (modelDefinition != NULL) {
    candidateModelDefinitionId = modelDefinition->definitionId;
    armyAssetRegistryCursor = g_ArmyAssetRecordRegistry;
    registryEntriesRemaining = ARMY_ASSET_REGISTRY_SLOT_COUNT;
    compatibleAssetScoreSum = 0;
    compatibleAssetCount = 0;
    for (; registryEntriesRemaining != 0;
        armyAssetRegistryCursor++,
        registryEntriesRemaining--) {
      armyAssetRecord = (ArmyAssetRecord *)*armyAssetRegistryCursor;
      if ((armyAssetRecord != NULL) &&
         ((armyAssetRecord->flags & 1) != 0)) { /* bit 0: asset enabled */
        /* the asset's root node is an AiLinkedDefinitionListView */
        assetDefinitionListAddress = armyAssetRecord->rootNodeOffsetOrPointer;
        if ((((candidateModelDefinitionId !=
               ((AiLinkedDefinitionListView *)assetDefinitionListAddress)->definitionIds[0]) &&
             (((candidateModelDefinitionId !=
                ((AiLinkedDefinitionListView *)assetDefinitionListAddress)->definitionIds[1] &&
               (candidateModelDefinitionId !=
                ((AiLinkedDefinitionListView *)assetDefinitionListAddress)->definitionIds[2])) &&
              (candidateModelDefinitionId !=
               ((AiLinkedDefinitionListView *)assetDefinitionListAddress)->definitionIds[3])))) &&
            (((candidateModelDefinitionId !=
               ((AiLinkedDefinitionListView *)assetDefinitionListAddress)->definitionIds[4] &&
              (candidateModelDefinitionId !=
               ((AiLinkedDefinitionListView *)assetDefinitionListAddress)->definitionIds[5])) &&
             (candidateModelDefinitionId !=
              ((AiLinkedDefinitionListView *)assetDefinitionListAddress)->definitionIds[6])))) &&
           (candidateModelDefinitionId !=
            ((AiLinkedDefinitionListView *)assetDefinitionListAddress)->definitionIds[7])) {
          /* despite its name the check returns true (CF set) while the technology is still locked */
          technologyLocked = ModelDefinition_IsFactionTechnologyUnlocked
                            (g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits,
                             ((AiLinkedDefinitionListView *)assetDefinitionListAddress)->definitionIds[0]);
          if (((technologyLocked) ||
              (nestedLinkedDefinitions =
               (AiLinkedDefinitionListView *)((AiLinkedDefinitionListView *)assetDefinitionListAddress)->childList0Address,
              ((AiLinkedDefinitionListView *)assetDefinitionListAddress)->childListCount == 0)) ||
             ((((candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[0] &&
                ((((candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[1] &&
                   (candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[2])) &&
                  (candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[3])) &&
                 ((candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[4] &&
                  (candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[5])))))) &&
               ((candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[6] &&
                (candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[7])))) &&
              ((secondNestedLinkedDefinitions =
                (AiLinkedDefinitionListView *)((AiLinkedDefinitionListView *)assetDefinitionListAddress)->childList1Address,
               ((AiLinkedDefinitionListView *)assetDefinitionListAddress)->childListCount < 2 ||
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
          compatibleAssetCount++;
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
   Collects up to four distinct movement masks of the faction's own units (workspace 01) into
   g_AiActiveGridMaskClass0..3 (0xFFFFFFFF = unused): GRID_SCRATCH_BLOCKED | distance-band bit (8 + low class)
   | terrain bit (24 + high class), with both classes taken from the unit's model runtime. The site scan accepts
   a general site when its scratch neighbourhood avoids every bit of one of these masks. Without any unit
   class 0 falls back to 0x90000100 (band bit 8, terrain bit 28).
*/
void AiPlanning_CollectActiveGridMaskClasses(void)

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
  runtimeWorkspaceEntry = g_AiWorkspace01Units;
  /* The condition chain below inserts combinedMask into the first free slot unless one of the slots already
     holds it; the slots are written back through the locals as in the original. */
  for (remainingEntries = g_AiWorkspace01Count; g_AiActiveGridMaskClass0 = maskClass0, remainingEntries != 0;
      remainingEntries = remainingEntries - 1) {
    g_AiActiveGridMaskClass1 = maskClass1;
    g_AiActiveGridMaskClass2 = maskClass2;
    if (runtimeWorkspaceEntry->armyRuntime != NULL) {
      entityModelRuntime = (runtimeWorkspaceEntry->armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
      lowClassShift = entityModelRuntime[1].classLinkState.modelLinkOrState60.signedScalarState;
      if ((*(int *)(entityModelRuntime->reserved10_37 + 8) != 0) && (-1 < lowClassShift)) {
        highClassShift = entityModelRuntime[1].classLinkState.classState64;
        if ((((-1 < (int)highClassShift) &&
             ((((combinedMask = 0x100 << ((uint8_t)lowClassShift & 0x1f) | 0x80000000U |
                         0x1000000 << ((uint8_t)highClassShift & 0x1f),
                combinedMask != maskClass0 && (combinedMask != maskClass1)) &&
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
    runtimeWorkspaceEntry++;
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
void AiRuntime_DispatchFactionPlanningPhase(FactionRuntimeIndex factionIndex,WorldRuntimeContext *inGameRuntime)

{
  uint32_t planningPhaseDispatchIndex;
  WorldRuntimeContext *worldRuntime;
  bool phaseResult;
  AiKnowledgeDataImage *knowledgeData;

  if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
       SESSION_NETWORK_ROLE_LOCAL) || ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_AI_PLANNING_OFF) == 0)) {
    planningPhaseDispatchIndex = g_GameFactionRuntimeImage.tail.simulationTick >> 6 & 1;
    /* inGameRuntime is really the in-game root (the caller passes g_InGameRuntimeRoot); its world view */
    worldRuntime = &((InGameRuntimeRootImageC3E4 *)inGameRuntime)->worldRuntime0A30;
    if ((g_GameFactionRuntimeImage.tail.simulationTick >> 3 & 7) == factionIndex) {
      AiPlanning_RebuildFactionWorkspaces
                (planningPhaseDispatchIndex,factionIndex,factionIndex,
                 worldRuntime);
      switch(planningPhaseDispatchIndex) {
      case 0:
        AiUnitBehavior_UpdateWorkspace01Entities(factionIndex,worldRuntime);
        AiUnitGroup_AssignCollectedEntitiesToBestTarget();
        break;
      case 1:
        AiFactionPlanning_UpdateActiveEntityPressureFlag(factionIndex);
        GameFactionRelations_UpdateAllPairsForFaction
                  (factionIndex,worldRuntime);
        phaseResult = AiConstructionPlanner_ProcessPendingAssetRequests
                          (factionIndex,worldRuntime);
        knowledgeData = g_AiKnowledgeData;
        if (!phaseResult) {
          if (((g_GameFactionRuntimeImage.records[factionIndex].candidateCache.cacheReuseState == 0)
              || (49 < (int)g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorCooldown)
              ) || (49 < (int)g_GameFactionRuntimeImage.records[factionIndex].anchorCooldown0)) {
            AiCandidateWorkspace_Clear();
            AiResourceCandidate_AddWeightedId136(factionIndex);
            AiWorkspaceAssetCandidate_AddWeightedEntry
                      ((knowledgeData->parameters).specialSite14aBaseWeight,
                       ARM_0330_BUILDING_MDL0303,factionIndex,worldRuntime);
            AiWorkspaceAssetCandidate_AddWeightedEntry
                      ((knowledgeData->parameters).specialSite14cBaseWeight,
                       ARM_0332_BUILDING_MDL0302,factionIndex,worldRuntime);
            AiStructureCandidate_AddWeightedId14BOr14CCandidate
                      ((knowledgeData->parameters).structure14bBaseWeight,ARM_0331_BUILDING_MDL0308,
                       factionIndex,worldRuntime);
            AiStructureCandidate_AddWeightedId14BOr14CCandidate
                      ((knowledgeData->parameters).structure14dBaseWeight,ARM_0333_BUILDING_MDL0307,
                       factionIndex,worldRuntime);
            AiCandidatePlanning_AddSpecialSiteCandidate
                      (factionIndex,worldRuntime);
            AiArmyCandidate_AddBestScoredVariantA(factionIndex,worldRuntime);
            AiArmyCandidate_AddBestScoredVariantB(factionIndex,worldRuntime);
            AiArmyCandidate_AddBestScoredVariantC(factionIndex,worldRuntime);
            AiStrategicClass_AddCandidate12DOr12FTo132
                      (factionIndex,worldRuntime);
            AiStrategicClass_AddWeightedClassCandidate
                      (factionIndex,worldRuntime);
            AiStrategicCandidate_AddBestWorkspace12Entry
                      (factionIndex,worldRuntime);
            phaseResult = AiPurchasePlanner_ExecuteAffordableCandidates(factionIndex);
            if (phaseResult) {
              AiCandidateWorkspace_SaveToFactionImage(factionIndex * (int)sizeof(GameFactionRuntimeRecord));
              g_GameFactionRuntimeImage.records[factionIndex].candidateCache.cacheReuseState = 0x10;
            }
          }
          else {
            AiCandidateWorkspace_LoadFromFactionImage(factionIndex * (int)sizeof(GameFactionRuntimeRecord));
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
   Works through the faction's pending asset requests (workspace 04) in order and hands each to its placement
   handler by ARM id: 300 only while no unassigned 330 exists, 330/332 at a workspace site, 333 derived from a
   330/332 site, other ids below 340 at a reachable candidate, ids from 340 on near the faction anchor.
   Returns true (CF set) as soon as a handler has placed an asset (g_AiConstructionPendingAssetConsumedCount).
*/
bool AiConstructionPlanner_ProcessPendingAssetRequests
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
  requestEntry = g_AiWorkspace04RequestedAssets;
  while (remainingRequests != 0) {
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
                          (factionIndex,armyAssetLookup.recordOrError->rootNodeOffsetOrPointer);
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
    requestEntry++;
    remainingRequests--;
  }
  return false;
}


/* Address: 0x005378C0.
   Buys the AI's candidates in descending weight order: each candidate's cost is taken from a running copy of
   the faction's Xenite, and every candidate with an eligible producer is applied, until one is no longer
   affordable. The cost is deducted even when no producer is found. Returns true (CF set) when candidates
   existed but none was applied, false otherwise.
*/
bool AiPurchasePlanner_ExecuteAffordableCandidates(FactionRuntimeIndex factionIndex)

{
  bool noCandidateApplied;
  uint32_t candidateCost;
  int remainingCandidates;
  XeniteAmountQ4 remainingXenite;
  AiCandidateWorkspaceEntry *entry;
  bool insufficientOrNoProducer;
  
  remainingCandidates = g_AiCandidateWorkspaceEntryCount;
  entry = g_AiWorkspace13Candidates;
  g_AiPurchaseAppliedArmyClassMask = 0;
  noCandidateApplied = false;
  if (g_AiCandidateWorkspaceEntryCount != 0) {
    noCandidateApplied = true;
    AiCandidateWorkspace_SortDescending();
    remainingXenite = g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4;
    do {
      candidateCost = AiCandidateWorkspace_GetEntryXeniteCost(entry);
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
      entry++;
      remainingCandidates--;
    } while (remainingCandidates != 0);
  }
  return noCandidateApplied;
}


/* Address: 0x005393F0.
   Builds a pending ARM_0333 (the caller's only asset here, 0x14D) next to a resource site: first for each
   ARM_0330 site of workspace 08, then for each ARM_0332 site, where AiPlacement_ReserveAdditionalSpecialSite
   accepts the site it creates the structure at the nearest valid workspace-09 cell, initialises it like the other
   planners, starts its effect and removes it from the pending list (a failed creation ends the attempt). Without
   such a site it falls back to AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate.
*/
void AiConstructionPlanner_PlaceDerivedAsset14D(PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime)

{
  FieldGridCell *sourceCell;
  ArmyRuntimeSlot *createdModelNode;
  ArmyRuntimeSlot *createdArmySlot;
  ModelRuntimeSlot *createdModelRuntime;
  ArmyRuntimeSlot **armyRuntime;
  AiTerrainFeatureWorkspaceEntry *arm0330SiteEntry;
  AiTerrainFeatureWorkspaceEntry *arm0332SiteEntry;
  int remainingArm0330Entries;
  int remainingArm0332Entries;
  bool siteRejected;
  ArmyRuntimeCreateResult createdInstance;
  AiAnchorResult anchorResult;
  
  arm0330SiteEntry = g_AiWorkspace08TerrainFeatureSites;
  remainingArm0330Entries = g_AiWorkspace08Count;
  if (g_AiWorkspace08Count != 0) {
    do {
      if (arm0330SiteEntry->armyAssetId == ARM_0330_BUILDING_MDL0303) {
        sourceCell = arm0330SiteEntry->cell;
        siteRejected = AiPlacement_ReserveAdditionalSpecialSite
                          (arm0330SiteEntry->armyAssetId,sourceCell,factionIndex,worldRuntime);
        if (!siteRejected) {
          anchorResult = AiPlacement_FindNearestValidWorkspace09Anchor
                            (sourceCell->worldY,sourceCell->worldX,armyAssetId,factionIndex,worldRuntime);
          if (!anchorResult.notFound) {
            createdInstance = ArmyRuntime_CreateInstanceFromAsset
                              (ARMY_CREATE_UNLOCK_TECHNOLOGY,0,anchorResult.worldYQ12,anchorResult.worldXQ12,
                               factionIndex,armyAssetId,
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
                       (EffectDefinition *)createdModelRuntime->attachments[2].childLocalRotationAngle0,
                       worldRuntime);
            AiConstructionPlanner_ConsumeFactionPendingArmyAsset(armyAssetId,factionIndex);
            return;
          }
        }
      }
      arm0330SiteEntry++;
      remainingArm0330Entries--;
      arm0332SiteEntry = g_AiWorkspace08TerrainFeatureSites;
      remainingArm0332Entries = g_AiWorkspace08Count;
    } while (remainingArm0330Entries != 0);
    do {
      if (arm0332SiteEntry->armyAssetId == ARM_0332_BUILDING_MDL0302) {
        sourceCell = arm0332SiteEntry->cell;
        siteRejected = AiPlacement_ReserveAdditionalSpecialSite
                          (arm0332SiteEntry->armyAssetId,sourceCell,factionIndex,worldRuntime);
        if (!siteRejected) {
          anchorResult = AiPlacement_FindNearestValidWorkspace09Anchor
                            (sourceCell->worldY,sourceCell->worldX,armyAssetId,factionIndex,worldRuntime);
          if (!anchorResult.notFound) {
            createdInstance = ArmyRuntime_CreateInstanceFromAsset
                              (ARMY_CREATE_UNLOCK_TECHNOLOGY,0,anchorResult.worldYQ12,anchorResult.worldXQ12,
                               factionIndex,armyAssetId,
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
                       (EffectDefinition *)createdModelRuntime->attachments[2].childLocalRotationAngle0,
                       worldRuntime);
            AiConstructionPlanner_ConsumeFactionPendingArmyAsset(armyAssetId,factionIndex);
            return;
          }
        }
      }
      remainingArm0332Entries--;
      arm0332SiteEntry++;
    } while (remainingArm0332Entries != 0);
  }
  AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate(armyAssetId,factionIndex,worldRuntime);
  return;
}


/* Address: 0x0053A6E0.
   Once Arms Factories is researched and the planning capacity allows it, scores the eligible army assets of
   workspace 11 with weight profile A and proposes the best one. Its weight (armyVariantABaseWeight) is divided
   by 1 + the number of pending requests with ARM ids 340..379, then taken x3/4 while the faction's primary
   anchor cooldown is 0, else x2. Assets below ARM 300 are only proposed while workspace 01 has at most 10
   entries.
*/
void AiArmyCandidate_AddBestScoredVariantA(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

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
  armyAssetRegistryCursor = g_AiWorkspace11ProducibleAssets;
  if (g_AiWorkspace11Count != 0) {
    bestScore = 0;
    blockingCondition = AiFactionRuntime_TestPlanningCapacityExceeded(4,factionIndex);
    /* Technology_IsUnlockedForFaction returns true (CF set) while the technology is still locked */
    if ((!blockingCondition) &&
       (blockingCondition = Technology_IsUnlockedForFaction(TEC_001_ARMS_FACTORIES,factionIndex), !blockingCondition)) {
      do {
        if (((((ArmyAssetRecord *)*armyAssetRegistryCursor)->flags & 1) != 0) &&
           (candidateScore = AiArmyCandidate_ComputeFactionWeightedScore
                              (&g_AiArmyCandidateScoreWeightsVariantA15,factionIndex,
                               (ArmyAssetRecord *)*armyAssetRegistryCursor),
           bestScore < candidateScore)) {
          bestArmyAsset = *armyAssetRegistryCursor;
          bestScore = candidateScore;
        }
        armyAssetRegistryCursor++;
        remainingEntries--;
      } while (remainingEntries != 0);
      if (0 < bestScore) {
        pendingCountOrWeight = 1;
        runtimeWorkspaceEntry = g_AiWorkspace04RequestedAssets;
        for (remainingEntries = g_AiWorkspace04Count; remainingEntries != 0; remainingEntries--) {
          if (((ARM_0340_BUILDING_MDL0314 - 1) < runtimeWorkspaceEntry->armyAssetId) &&
             (runtimeWorkspaceEntry->armyAssetId < ARM_0380_BUILDING_MDL0329)) {
            pendingCountOrWeight++;
          }
          runtimeWorkspaceEntry++;
        }
        pendingCountOrWeight = g_AiKnowledgeData->parameters.armyVariantABaseWeight / pendingCountOrWeight;
        if (g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorCooldown == 0) {
          weightRange = pendingCountOrWeight * 3 >> 2;
        }
        else {
          weightRange = pendingCountOrWeight * 2;
        }
        if ((299 < bestArmyAsset->registryId) || (g_AiWorkspace01Count < 11)) {
          AiCandidateWorkspace_AddOrAccumulateWeightedEntry(bestArmyAsset->registryId,weightRange,1);
        }
      }
    }
  }
  return;
}


/* Address: 0x0053AC20.
   Once the faction has an ARM 330 (0x14A) structure: while Arms Factories is still locked it proposes building
   ARM 301 (0x12D) if it has none and the capacity allows; after the research it proposes the best class of the
   ARM 302..306 (0x12E..0x132) family, its weight divided by twice the class's existing count (if any).
*/
void AiStrategicClass_AddCandidate12DOr12FTo132(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  uint32_t weightRange;
  bool conditionMet;
  AiStrategicClassSelectionRegs8 classSelection;
  AiKnowledgeDataImage *knowledgeData;
  
  knowledgeData = g_AiKnowledgeData;
  conditionMet = AiPrimaryWorkspace_HasEntryById(ARM_0330_BUILDING_MDL0303);
  if (conditionMet) {
    /* true (CF set) while the technology is still locked */
    conditionMet = Technology_IsUnlockedForFaction(TEC_001_ARMS_FACTORIES,factionIndex);
    if (conditionMet) {
      conditionMet = AiPrimaryWorkspace_HasEntryById(ARM_0301_BUILDING_MDL0318);
      if (!conditionMet) {
        conditionMet = AiFactionRuntime_TestPlanningCapacityExceeded
                          (knowledgeData->parameters.strategic12dAnd141To143AdditionalPlanningCapacity,
                           factionIndex);
        if (!conditionMet) {
          AiCandidateWorkspace_AddOrAccumulateWeightedEntry
                    (ARM_0301_BUILDING_MDL0318,knowledgeData->parameters.strategicClass12dBaseWeight,1);
        }
      }
    }
    else {
      conditionMet = AiFactionRuntime_TestPlanningCapacityExceeded
                        (knowledgeData->parameters.strategic12fTo132AdditionalPlanningCapacity,factionIndex);
      if (!conditionMet) {
        classSelection = AiStrategicClass_SelectBestCandidate12ETo132(factionIndex,worldRuntime);
        if (classSelection.selectedRuntimeToken != 0) {
          weightRange = knowledgeData->parameters.strategicClass12fTo132BaseWeight;
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
   Once the faction has an ARM 330 (0x14A) structure and the planning capacity allows it, proposes the class
   chosen from the ARM 321..323 (0x141..0x143) family, its weight divided by twice the class's existing count
   (if any).
*/
void AiStrategicClass_AddWeightedClassCandidate(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  AiKnowledgeDataImage *knowledgeData;
  uint32_t weightRange;
  bool conditionMet;
  AiStrategicClassSelectionRegs8 classSelection;
  
  knowledgeData = g_AiKnowledgeData;
  conditionMet = AiPrimaryWorkspace_HasEntryById(ARM_0330_BUILDING_MDL0303);
  if (conditionMet) {
    conditionMet = AiFactionRuntime_TestPlanningCapacityExceeded
                      (knowledgeData->parameters.strategic12dAnd141To143AdditionalPlanningCapacity,factionIndex);
    if (!conditionMet) {
      classSelection = AiStrategicClass_SelectWeightedClass141To143(factionIndex,worldRuntime);
      if (classSelection.selectedRuntimeToken != 0) {
        weightRange = knowledgeData->parameters.strategicClass141To143BaseWeight;
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
   Default placement of a pending building (every asset the request dispatcher does not handle specially), only
   while the faction's primary anchor cooldown is nonzero: among the workspace-09 cells at least 0x2000 from every
   active primary-workspace structure it takes the one with the lowest Manhattan distance to the primary anchor
   plus 4x the distance to the nearest workspace-02 site (at most 0x5000) or, without such a site, 2x the
   distance to the nearest workspace-03 site (at most 0x8000), where the mode-1 placement test passes. The
   building is created there, initialised like the other planners and removed from the pending list.
*/
void AiConstructionPlanner_PlaceExtendedAssetNearFactionAnchor
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
    gridCellCursor = g_AiWorkspace09Cells;
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
      gridCellCursor++;
      remainingCells--;
    } while (remainingCells != 0);
    if (bestScore < 0x7fffffff) {
      createdInstance = ArmyRuntime_CreateInstanceFromAsset
                        (ARMY_CREATE_UNLOCK_TECHNOLOGY,(uint32_t)(uint16_t)bestCell->triangle0NormalAngles,
                         bestCell->worldY,
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
                   (EffectDefinition *)createdModelRuntime->attachments[2].childLocalRotationAngle0,
                   worldRuntime);
        AiConstructionPlanner_ConsumeFactionPendingArmyAsset(armyAssetId,factionIndex);
      }
    }
  }
  return;
}


/* Address: 0x0053A800.
   Exploration: while there are general sites (workspace 05) and workspace 01 has at most 10 entries, scores
   the eligible army assets of workspace 11 with weight profile B and proposes the best one. Its weight grows
   with the unexplored share of the terrain, (100 - explored %) * coefficient / (32 * (workspace 01 count + 1)),
   halved while there are targets (workspace 07).
*/
void AiArmyCandidate_AddBestScoredVariantB(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  AiCandidateScore32 candidateScore;
  uint32_t weightRange;
  int remainingEntries;
  int bestScore;
  ArmyAssetRecordPrefix *bestArmyAsset;
  ArmyAssetRecordPrefix **armyAssetRegistryCursor;
  
  if (((g_AiWorkspace05Count != 0) && (g_AiWorkspace11Count != 0)) && (g_AiWorkspace01Count < 11))
  {
    bestScore = 0;
    remainingEntries = g_AiWorkspace11Count;
    armyAssetRegistryCursor = g_AiWorkspace11ProducibleAssets;
    do {
      if ((((ArmyAssetRecord *)*armyAssetRegistryCursor)->flags & 1) != 0) {
        candidateScore = AiArmyCandidate_ComputeFactionWeightedScore
                          (&g_AiArmyCandidateScoreWeightsVariantB15,factionIndex,
                           (ArmyAssetRecord *)*armyAssetRegistryCursor);
        if (bestScore < candidateScore) {
          bestArmyAsset = *armyAssetRegistryCursor;
          bestScore = candidateScore;
        }
      }
      armyAssetRegistryCursor++;
      remainingEntries--;
    } while (remainingEntries != 0);
    if (0 < bestScore) {
      weightRange = (uint32_t)(((int64_t)
                             (int)(100 - g_GameFactionRuntimeImage.records[factionIndex].exploredTerrainPercent) *
                            (int64_t)
                            (int)g_AiKnowledgeData->parameters.armyVariantBUnexploredTerrainWeightCoefficient) /
                           (int64_t)(int)((g_AiWorkspace01Count + 1) * 32));
      if (g_AiWorkspace07Count != 0) {
        weightRange = weightRange >> 1;
      }
      AiCandidateWorkspace_AddOrAccumulateWeightedEntry(bestArmyAsset->registryId,weightRange,1);
    }
  }
  return;
}


/* Address: 0x0053A8D0.
   Attack: while there are targets (workspace 07) and workspace 01 has at most 10 entries, scores the eligible
   army assets of workspace 11 with weight profile C and proposes the best one with armyVariantCBaseWeight.
   The halving for an empty workspace 07 can never apply (the entry check requires targets); the original
   (0x0053A95D) has the same dead test.
*/
void AiArmyCandidate_AddBestScoredVariantC(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  AiCandidateScore32 candidateScore;
  uint32_t weightRange;
  int remainingEntries;
  int bestScore;
  ArmyAssetRecordPrefix *bestArmyAsset;
  ArmyAssetRecordPrefix **armyAssetRegistryCursor;
  
  if (((g_AiWorkspace07Count != 0) && (g_AiWorkspace11Count != 0)) && (g_AiWorkspace01Count < 11))
  {
    bestScore = 0;
    remainingEntries = g_AiWorkspace11Count;
    armyAssetRegistryCursor = g_AiWorkspace11ProducibleAssets;
    do {
      if ((((ArmyAssetRecord *)*armyAssetRegistryCursor)->flags & 1) != 0) {
        candidateScore = AiArmyCandidate_ComputeFactionWeightedScore
                          (&g_AiArmyCandidateScoreWeightsVariantC15,factionIndex,
                           (ArmyAssetRecord *)*armyAssetRegistryCursor);
        if (bestScore < candidateScore) {
          bestArmyAsset = *armyAssetRegistryCursor;
          bestScore = candidateScore;
        }
      }
      armyAssetRegistryCursor++;
      remainingEntries--;
    } while (remainingEntries != 0);
    if (0 < bestScore) {
      weightRange = g_AiKnowledgeData->parameters.armyVariantCBaseWeight;
      if (g_AiWorkspace07Count == 0) {
        weightRange = weightRange >> 1;
      }
      AiCandidateWorkspace_AddOrAccumulateWeightedEntry(bestArmyAsset->registryId,weightRange,1);
    }
  }
  return;
}


/* Address: 0x00537630.
   Returns false (CF clear) when an idle structure of workspace 00 can carry out a purchase candidate. A
   technology must be available to the faction and listed in the 28 research slots (+0x1C8..+0x234) of the
   definition of a structure whose runtimeFlags have none of 0x89. An army asset's producer class mask (asset +0x14) must not
   overlap g_AiPurchaseAppliedArmyClassMask (one purchase per producer class and round); bit 0x10 needs a
   class-11 structure with +0xB8 clear, bit 0x08 a class-22 structure with +0xAC clear, the bits 0xEE a class-13
   structure whose definition mask at +0xC4 shares them and with +0xB8 clear (runtimeFlags without 0xC9 each).
*/
bool AiPurchaseCandidate_HasEligibleProducer(AiCandidateWorkspaceEntry *candidateEntry,FactionRuntimeIndex factionIndex)

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
  
  technologyIndex = candidateEntry->entityIdAndMultiplicity & AI_CANDIDATE_ID_MASK;
  if ((candidateEntry->weightedScoreAndKind & AI_CANDIDATE_KIND_MASK) == AI_CANDIDATE_KIND_TECHNOLOGY) {
    technologyAvailable = Technology_IsAvailableForFaction(technologyIndex,factionIndex);
    workspaceEntry = g_AiWorkspace00Structures;
    countOrClassMask = g_AiWorkspace00Count;
    if (technologyAvailable) {
      for (; countOrClassMask != 0; countOrClassMask = countOrClassMask - 1) {
        /* entitySlot[0] = definition, [0x3B] = runtimeFlags (+0xEC) */
        entitySlot = (int *)workspaceEntry->runtimeSlotAddressOrZero;
        if ((entitySlot != NULL) && ((entitySlot[0x3b] & 0x89U) == 0)) {
          technologySlotIndex = 0x1c;
          do {
            if (technologyIndex == ((ModelDefinition *)*entitySlot)->researchTechnologyIds1C4[technologySlotIndex]) {
              return false;
            }
            technologySlotIndex--;
          } while (technologySlotIndex != 0);
        }
        workspaceEntry++;
      }
    }
  }
  else {
    /* the original ignores the lookup's CF and reads the class mask at +0x14 of whatever EAX holds */
    armyAssetLookup = ArmyAssetRegistry_FindById(technologyIndex);
    countOrClassMask = armyAssetLookup.recordOrError[1].selectionDetailTemplateVariantIndex;
    if ((g_AiWorkspace00Count != 0) && ((g_AiPurchaseAppliedArmyClassMask & countOrClassMask) == 0)) {
      remainingEntries = g_AiWorkspace00Count;
      workspaceEntry = g_AiWorkspace00Structures;
      if ((countOrClassMask & 0x10) == 0) {
        if ((countOrClassMask & 8) == 0) {
          remainingGuard = countOrClassMask & 0xee;
          while (remainingGuard != 0) {
            entitySlot = (int *)workspaceEntry->runtimeSlotAddressOrZero;
            if (((entitySlot != NULL) && (((ModelDefinition *)*entitySlot)->runtimeClassId4C ==
                                          MODEL_RUNTIME_CLASS_13)) &&
               (((entitySlot[0x3b] & 0xc9U) == 0 &&
                (((((ModelDefinition *)*entitySlot)->classParameterC4 & countOrClassMask & 0xee) != 0 &&
                  (entitySlot[0x2e] == 0)))))) {
              return false;
            }
            workspaceEntry++;
            remainingEntries = remainingEntries - 1;
            remainingGuard = remainingEntries;
          }
        }
        else {
          do {
            entitySlot = (int *)workspaceEntry->runtimeSlotAddressOrZero;
            if (((entitySlot != NULL) && (((ModelDefinition *)*entitySlot)->runtimeClassId4C ==
                                          MODEL_RUNTIME_CLASS_22)) &&
               (((entitySlot[0x3b] & 0xc9U) == 0 && (entitySlot[0x2b] == 0)))) {
              return false;
            }
            workspaceEntry++;
            remainingEntries--;
          } while (remainingEntries != 0);
        }
      }
      else {
        do {
          entitySlot = (int *)workspaceEntry->runtimeSlotAddressOrZero;
          if ((((entitySlot != NULL) && (((ModelDefinition *)*entitySlot)->runtimeClassId4C ==
                                         MODEL_RUNTIME_CLASS_11)) &&
              ((entitySlot[0x3b] & 0xc9U) == 0)) && (entitySlot[0x2e] == 0)) {
            return false;
          }
          workspaceEntry++;
          remainingEntries--;
        } while (remainingEntries != 0);
      }
    }
  }
  return true;
}


/* Address: 0x00537800.
   Carries out an affordable purchase candidate. A technology starts researching at the first idle workspace-00
   structure that lists it (the checks of AiPurchaseCandidate_HasEligibleProducer), whose workspace slot is then
   cleared so it takes no second job this round. An army asset is appended once to the faction's pending asset
   list, and its producer class mask is added to g_AiPurchaseAppliedArmyClassMask.
*/
void AiPurchaseCandidate_ApplyToFaction(AiCandidateWorkspaceEntry *candidateEntry,FactionRuntimeIndex factionIndex)

{
  GameEntityRuntime *entity;
  int remainingEntries;
  AiWorkspace00EntryView8 *workspaceEntry;
  int technologySlotIndex;
  RuntimeToken technologyIndex;
  ArmyAssetLookupResult armyAssetLookup;
  
  technologyIndex = candidateEntry->entityIdAndMultiplicity & AI_CANDIDATE_ID_MASK;
  remainingEntries = g_AiWorkspace00Count;
  workspaceEntry = g_AiWorkspace00Structures;
  if ((candidateEntry->weightedScoreAndKind & AI_CANDIDATE_KIND_MASK) == AI_CANDIDATE_KIND_TECHNOLOGY) {
    for (; remainingEntries != 0; remainingEntries--) {
      entity = (GameEntityRuntime *)workspaceEntry->runtimeSlotAddressOrZero;
      if ((entity != NULL) && (((entity->common).runtimeFlags & 0x89) == 0)) {
        technologySlotIndex = 0x1c;
        do {
          if (technologyIndex ==
              ((ModelDefinition *)(entity->common).ownership.definitionOrClassRecord)->
              researchTechnologyIds1C4[technologySlotIndex]) {
            Technology_ApplyRecordToEntity(technologyIndex,entity);
            workspaceEntry->runtimeSlotAddressOrZero = 0;
            return;
          }
          technologySlotIndex--;
        } while (technologySlotIndex != 0);
      }
      workspaceEntry++;
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
   Sets bit 0 of the faction's runtimeFlags when the faction is active enough: one assigned workspace 00
   structure below ARM 340, two assigned ARM 340..379 structures, or three qualifying entries counting one such
   structure plus the armies of workspace 01 (field +0x0C >= 1 with +0x140 set, or >= 2 with +0x160 set).
   When nothing triggers but the flag is already set, re-applies the model flags of every workspace 00/01
   entity in the world runtime.
*/
void AiFactionPlanning_UpdateActiveEntityPressureFlag(FactionRuntimeIndex factionIndex)

{
  FactionRuntimeFlags *factionRuntimeFlags;
  int remainingEntries;
  int thresholdOrRemaining;
  WorldRuntimeContext *contextArg;
  AiWorkspace00EntryView8 *primaryEntry;
  AiRuntimeWorkspaceEntry *runtimeWorkspaceEntry;
  ArmyRuntimeSlot *armySlot;
  
  thresholdOrRemaining = 2;
  primaryEntry = g_AiWorkspace00Structures;
  for (remainingEntries = g_AiWorkspace00Count; remainingEntries != 0; remainingEntries--) {
    if ((primaryEntry->runtimeSlotAddressOrZero != 0) &&
       ((primaryEntry->armyAssetId < ARM_0340_BUILDING_MDL0314 ||
        ((primaryEntry->armyAssetId < ARM_0380_BUILDING_MDL0329 && (thresholdOrRemaining = thresholdOrRemaining + -1,
                                                                    thresholdOrRemaining == 0)))))) {
      factionRuntimeFlags = &g_GameFactionRuntimeImage.records[factionIndex].runtimeFlags;
      *factionRuntimeFlags = *factionRuntimeFlags | 1;
      return;
    }
    primaryEntry++;
  }
  thresholdOrRemaining++;
  remainingEntries = g_AiWorkspace01Count;
  runtimeWorkspaceEntry = g_AiWorkspace01Units;
  while( true ) {
    if (remainingEntries == 0) {
      if ((g_GameFactionRuntimeImage.records[factionIndex].runtimeFlags & 1) != 0) {
        contextArg = &g_InGameRuntimeRoot->worldRuntime0A30;
        primaryEntry = g_AiWorkspace00Structures;
        for (thresholdOrRemaining = g_AiWorkspace00Count; remainingEntries = g_AiWorkspace01Count,
            runtimeWorkspaceEntry = g_AiWorkspace01Units, thresholdOrRemaining != 0; thresholdOrRemaining--)
        {
          if (primaryEntry->runtimeSlotAddressOrZero != 0) {
            ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive
                      (contextArg,*(int **)(primaryEntry->runtimeSlotAddressOrZero + 8));
          }
          primaryEntry++;
        }
        for (; remainingEntries != 0; remainingEntries--) {
          if (runtimeWorkspaceEntry->armyRuntime != NULL) {
            ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive
                      (contextArg,(int *)runtimeWorkspaceEntry->armyRuntime->linkedEntityRuntime);
          }
          runtimeWorkspaceEntry++;
        }
      }
      return;
    }
    armySlot = runtimeWorkspaceEntry->armyRuntime;
    /* "factionIndex" (+0x0C), "[1].commandCoordinate0Q12" (+0x140) and "[1].runtimeState40" (+0x160) as
       typed; the original reads exactly these offsets */
    if ((((armySlot != NULL) && (armySlot->factionIndex != 0)) &&
        ((armySlot[1].commandCoordinate0Q12 != 0 ||
         ((1 < (uint32_t)armySlot->factionIndex && (armySlot[1].runtimeState40 != 0)))))) &&
       (thresholdOrRemaining = thresholdOrRemaining + -1, thresholdOrRemaining == 0)) break;
    runtimeWorkspaceEntry++;
    remainingEntries--;
  }
  factionRuntimeFlags = &g_GameFactionRuntimeImage.records[factionIndex].runtimeFlags;
  *factionRuntimeFlags = *factionRuntimeFlags | 1;
  return;
}


/* Address: 0x00539D20.
   Proposes the resource structure candidateArmyAssetId unless one of it is still unassigned. ARM 331 (0x14B):
   with an ARM 330 present and the Xenite storage limit below the knowledge limit, when the free storage
   (limit - current Xenite) is below the gap limit; a full storage quadruples the weight. Any other candidate
   (Tritium): with an ARM 332 present, the same test on the Tritium storage; independently of that the
   first workspace 08 site that still allows an extra special site adds the candidate again with a weight
   3 * derived / (existing count of that site's structure + 3).
*/
void AiStructureCandidate_AddWeightedId14BOr14CCandidate
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
         (storageLimit < knowledgeData->parameters.structure14bPrerequisite14aCountLimit)) {
        xeniteCurrent = g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4;
        if (storageLimit == xeniteCurrent) {
          baseWeight = baseWeight << 2;
        }
        if (storageLimit - xeniteCurrent < knowledgeData->parameters.structure14bCountGapLimit) {
          AiCandidateWorkspace_AddOrAccumulateWeightedEntry(ARM_0331_BUILDING_MDL0308,baseWeight,1);
        }
      }
    }
    else {
      conditionMet = AiPrimaryWorkspace_HasEntryById(ARM_0332_BUILDING_MDL0302);
      terrainFeatureEntry = g_AiWorkspace08TerrainFeatureSites;
      remainingOrAssignedCount = g_AiWorkspace08Count;
      if (((conditionMet) && (storageLimit != 0)) &&
         ((storageLimit < knowledgeData->parameters.structure14dPrerequisite14cCountLimit &&
          (storageLimit - g_GameFactionRuntimeImage.records[factionIndex].tritiumCurrentQ4 <
           knowledgeData->parameters.structure14dCountGapLimit)))) {
        AiCandidateWorkspace_AddOrAccumulateWeightedEntry(candidateArmyAssetId,baseWeight,1);
        terrainFeatureEntry = g_AiWorkspace08TerrainFeatureSites;
        remainingOrAssignedCount = g_AiWorkspace08Count;
      }
      for (; remainingOrAssignedCount != 0; remainingOrAssignedCount--) {
        conditionMet = AiPlacement_ReserveAdditionalSpecialSite
                          (terrainFeatureEntry->armyAssetId,terrainFeatureEntry->cell,factionIndex,worldRuntime);
        if (!conditionMet) {
          derivedWeight = knowledgeData->parameters.workspace08Id14aDerivedWeight;
          if (terrainFeatureEntry->armyAssetId != ARM_0330_BUILDING_MDL0303) {
            derivedWeight = knowledgeData->parameters.workspace08OtherDerivedWeight;
          }
          remainingOrAssignedCount = AiPrimaryWorkspace_CountAssignedEntriesById(terrainFeatureEntry->armyAssetId);
          AiCandidateWorkspace_AddOrAccumulateWeightedEntry
                    (candidateArmyAssetId,(derivedWeight * 3) / (remainingOrAssignedCount + 3U),1);
          return;
        }
        terrainFeatureEntry++;
      }
    }
  }
  return;
}


/* Address: 0x00539E60.
   Proposes ARM 310 (0x136) once an ARM 330 exists and no ARM 310 is unassigned, when baseline Energy supply
   plus the record's +0x358 rate (typed tritiumExtractionRateQ4PerTick) exceeds the Energy generation capacity.
   Weight: surplus * demand / capacity * resource136DeficitScoreNumerator / resource136DeficitScoreDenominator
   (Q4 values taken as integers, demand at least 1).
*/
void AiResourceCandidate_AddWeightedId136(FactionRuntimeIndex factionIndex)

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
                  (ARM_0310_BUILDING_MDL0305,
                   (uint32_t)(((int64_t)
                              (int)(((int64_t)energySurplus * (int64_t)energyDemand) / (int64_t)generationCapacity) *
                              (int64_t)(int)g_AiKnowledgeData->parameters.resource136DeficitScoreNumerator) /
                             (int64_t)(int)g_AiKnowledgeData->parameters.resource136DeficitScoreDenominator),1);
      }
    }
  }
  return;
}


/* Address: 0x0053A9D0.
   Picks which of the buildings ARM_0302..ARM_0306 (0x12E..0x132) to propose next. It measures the share (%) of
   scratch-grid cells free of terrain bit 24, of bits 25-27 and of bits 28-30; every building the faction lacks
   (not in workspace 00) but may build (enabled) is scored with its row of g_AiStrategicClassTerrainWeights plus
   14 random bits (0..0x3FFF), and the best one is returned in EBX (0 = none), with the number of these five
   buildings already present in ECX.
   Original register convention: result in EBX and ECX; EAX and EDX preserved.
*/
AiStrategicClassSelectionRegs8 AiStrategicClass_SelectBestCandidate12ETo132
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
      freeBit24Cells++;
    }
    if ((cellStateMask & (GRID_SCRATCH_TERRAIN_CLASS_BIT27|GRID_SCRATCH_TERRAIN_CLASS_BIT26|
                 GRID_SCRATCH_TERRAIN_CLASS_BIT25)) == 0) {
      freeBits25To27Cells++;
    }
    if ((cellStateMask & (GRID_SCRATCH_TERRAIN_CLASS_BIT30|GRID_SCRATCH_TERRAIN_CLASS_BIT29|
                 GRID_SCRATCH_TERRAIN_CLASS_BIT28)) == 0) {
      freeBits28To30Cells++;
    }
    scratchCell++;
    remainingCellsOrTieBits--;
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
      selectedToken = ARM_0302_BUILDING_MDL0300;
      /* the original rotates (ROR 5); a shift gives the same low 14 bits for the at most three steps used */
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
    existingClassCount--;
    conditionMet = ArmyAssetRegistry_FindEnabledById(ARM_0303_BUILDING_MDL0316);
    if (!conditionMet) {
      remainingCellsOrTieBits = randomizedTieBits & 0x3fff;
      randomizedTieBits = randomizedTieBits >> 5;
      candidateScore = freeBits28To30Percent * g_AiStrategicClassTerrainWeights[1][0] +
              freeBits25To27PercentOrScore * g_AiStrategicClassTerrainWeights[1][1] +
              freeBit24Percent * g_AiStrategicClassTerrainWeights[1][2] + remainingCellsOrTieBits;
      if (bestCandidateScore < candidateScore) {
        selectedToken = ARM_0303_BUILDING_MDL0316;
        bestCandidateScore = candidateScore;
      }
    }
  }
  conditionMet = AiPrimaryWorkspace_HasEntryById(ARM_0304_BUILDING_MDL0324);
  if (!conditionMet) {
    existingClassCount--;
    conditionMet = ArmyAssetRegistry_FindEnabledById(ARM_0304_BUILDING_MDL0324);
    if (!conditionMet) {
      remainingCellsOrTieBits = randomizedTieBits & 0x3fff;
      randomizedTieBits = randomizedTieBits >> 5;
      candidateScore = freeBits28To30Percent * g_AiStrategicClassTerrainWeights[2][0] +
              freeBits25To27PercentOrScore * g_AiStrategicClassTerrainWeights[2][1] +
              freeBit24Percent * g_AiStrategicClassTerrainWeights[2][2] + remainingCellsOrTieBits;
      if (bestCandidateScore < candidateScore) {
        selectedToken = ARM_0304_BUILDING_MDL0324;
        bestCandidateScore = candidateScore;
      }
    }
  }
  conditionMet = AiPrimaryWorkspace_HasEntryById(ARM_0305_BUILDING_MDL0317);
  if (!conditionMet) {
    existingClassCount--;
    conditionMet = ArmyAssetRegistry_FindEnabledById(ARM_0305_BUILDING_MDL0317);
    if (!conditionMet) {
      freeBits25To27PercentOrScore = freeBits28To30Percent * g_AiStrategicClassTerrainWeights[3][0] +
              freeBits25To27PercentOrScore * g_AiStrategicClassTerrainWeights[3][1] +
              freeBit24Percent * g_AiStrategicClassTerrainWeights[3][2] + (randomizedTieBits & 0x3fff);
      if (bestCandidateScore < freeBits25To27PercentOrScore) {
        selectedToken = ARM_0305_BUILDING_MDL0317;
        bestCandidateScore = freeBits25To27PercentOrScore;
      }
    }
  }
  conditionMet = AiPrimaryWorkspace_HasEntryById(ARM_0306_BUILDING_MDL0310);
  if (!conditionMet) {
    existingClassCount--;
    conditionMet = ArmyAssetRegistry_FindEnabledById(ARM_0306_BUILDING_MDL0310);
    if (!conditionMet) {
      /* the original also scores ARM_0306 with g_AiStrategicClassTerrainWeights[4] but
         discards the result (XOR EDX,EDX): ARM_0306 wins only over a negative best score */
      if (bestCandidateScore < 0) {
        selectedToken = ARM_0306_BUILDING_MDL0310;
      }
    }
  }
  selection.selectedRuntimeToken = selectedToken;
  selection.existingCountOrPressure = existingClassCount;
  return selection;
}


/* Address: 0x0053AF00.
   Picks which of the buildings ARM_0321..ARM_0323 (0x141..0x143) to propose next. Each is scored from the
   faction's AI pressure values 2..4 as sum((pressure + 1) * coefficient) / (pressure2 + pressure3 + pressure4 + 1)
   with its own three ki.dat coefficients, plus 7 random bits; only buildings the faction lacks (not in workspace
   00) but may build (enabled) compete. Returns the winner in EBX (0 = none) and the number of these three
   buildings already present in ECX.
   Original register convention: result in EBX and ECX; EAX and EDX preserved.
*/
AiStrategicClassSelectionRegs8 AiStrategicClass_SelectWeightedClass141To143
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
  class141Score = ((pressure2For141 + 1) * class141Coefficient0OrExistingCount + (pressure3For141 + 1) *
                   class141Coefficient1 + (pressure4For141 + 1) * class141Coefficient2) / pressureSumPlusOne +
           (randomBits & 0x7f);
  class142Score = ((pressure2For142 + 1) * class142Coefficient0 + (pressure3For142 + 1) * class142Coefficient1 +
                   (pressure4For142 + 1) * class142Coefficient2) / pressureSumPlusOne +
           (randomBits >> 0x13 & 0x7f);
  bestScore = 0;
  class141Coefficient0OrExistingCount = 3;
  selectedToken = 0;
  conditionMet = AiPrimaryWorkspace_HasEntryById(ARM_0321_BUILDING_MDL0326);
  if (!conditionMet) {
    class141Coefficient0OrExistingCount = 2;
    conditionMet = ArmyAssetRegistry_FindEnabledById(ARM_0321_BUILDING_MDL0326);
    if ((!conditionMet) && (class141Score != 0)) {
      selectedToken = ARM_0321_BUILDING_MDL0326;
      bestScore = class141Score;
    }
  }
  conditionMet = AiPrimaryWorkspace_HasEntryById(ARM_0322_BUILDING_MDL0327);
  if (!conditionMet) {
    class141Coefficient0OrExistingCount--;
    conditionMet = ArmyAssetRegistry_FindEnabledById(ARM_0322_BUILDING_MDL0327);
    if ((!conditionMet) && (bestScore < class142Score)) {
      selectedToken = ARM_0322_BUILDING_MDL0327;
      bestScore = class142Score;
    }
  }
  conditionMet = AiPrimaryWorkspace_HasEntryById(ARM_0323_BUILDING_MDL0328);
  if (!conditionMet) {
    class141Coefficient0OrExistingCount--;
    conditionMet = ArmyAssetRegistry_FindEnabledById(ARM_0323_BUILDING_MDL0328);
    if ((!conditionMet) &&
       (bestScore < ((pressure2For143 + 1) * class143Coefficient0 + (pressure3For143 + 1) * class143Coefficient1 +
                     (pressure4For143 + 1) * class143Coefficient2) / pressureSumPlusOne +
                 (randomBits >> 7 & 0x7f))) {
      selectedToken = ARM_0323_BUILDING_MDL0328;
    }
  }
  selection.selectedRuntimeToken = selectedToken;
  selection.existingCountOrPressure = class141Coefficient0OrExistingCount;
  return selection;
}


/* Address: 0x00539600.
   Places a pending asset at a workspace-10 cell, only while the faction's primary anchor cooldown is 0. Each cell
   is scored by its Manhattan distance to the faction's ARM_0300 structure (g_AiWorkspaceOwnedAsset300Runtime;
   the raw cell coordinates without one) plus 16 random bits; the lowest-scoring cell where the mode-1
   placement test passes and GridReachability_RebuildConnectedRegionAroundWorldPoint (with the radius of the
   asset's model definition) finds a connected region wins. The asset is created there, initialised like the
   other planners and removed from the pending list.
*/
void AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate
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
                       (((AiLinkedDefinitionListView *)(armyAssetLookup.recordOrError)->rootNodeOffsetOrPointer)->
                        definitionIds[0]);
    if (!modelDefinitionLookup.notFound) {
      radiusMetric =
           ((ModelDefinition *)modelDefinitionLookup.modelDefinition)->placementRadiusOrClearanceDC;
      if (g_AiWorkspace10Count != 0) {
        bestScore = 0x7fffffff;
        remainingCells = g_AiWorkspace10Count;
        gridCellCursor = g_AiWorkspace10Cells;
        do {
          candidateCell = *gridCellCursor;
          distanceXOrScore = candidateCell->worldX;
          distanceY = candidateCell->worldY;
          if (g_AiWorkspaceOwnedAsset300Runtime != NULL) {
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
          gridCellCursor++;
          remainingCells--;
        } while (remainingCells != 0);
        if (bestScore < 0x7fffffff) {
          createdInstance = ArmyRuntime_CreateInstanceFromAsset
                             (ARMY_CREATE_UNLOCK_TECHNOLOGY,(uint32_t)(uint16_t)bestCell->triangle0NormalAngles,
                              bestCell->worldY,
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
                       (EffectDefinition *)createdModelRuntime->attachments[2].childLocalRotationAngle0,
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
   Called after the AI has placed an army asset: counts the placement (global counter and the faction's
   relationCounterB) and removes the first entry for that asset's registry record from the faction's pending
   primary army asset list, shifting the rest down. The registry lookup's CF is not checked; an unknown id simply
   matches no entry.
*/
void AiConstructionPlanner_ConsumeFactionPendingArmyAsset
          (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex)

{
  FactionRelationCounter *relationCounter;
  FactionArmyAssetCount *pendingAssetCount;
  FactionArmyAssetCount remainingAssets;
  uint32_t *assetPointerCursor;
  ArmyAssetLookupResult armyAssetLookup;
  
  g_AiConstructionPendingAssetConsumedCount++;
  remainingAssets = g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
  armyAssetLookup = ArmyAssetRegistry_FindById(armyAssetId);
  assetPointerCursor = g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetPointersOrIds;
  relationCounter = &g_GameFactionRuntimeImage.records[factionIndex].relationCounterB;
  (*relationCounter)++;
  while( true ) {
    if (remainingAssets == 0) {
      return;
    }
    if (armyAssetLookup.recordOrError == (ArmyAssetRecordPrefix *)*assetPointerCursor) break;
    assetPointerCursor++;
    remainingAssets--;
  }
  pendingAssetCount = &g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
  (*pendingAssetCount)--;
  while (remainingAssets = remainingAssets - 1, remainingAssets != 0) {
    *assetPointerCursor = assetPointerCursor[1];
    assetPointerCursor++;
  }
  return;
}


/* Address: 0x0053A980.
   Returns true (CF set) when the faction's energy would not cover its demand plus additionalEnergyDemand
   (whole units): the usable supply is the smaller of the generation capacity and baseline supply + tritium
   extraction rate, the demand is supplied + unpowered demand (Q4 values shifted down by 4).
*/
bool AiFactionRuntime_TestPlanningCapacityExceeded(uint32_t additionalEnergyDemand,FactionRuntimeIndex factionIndex)

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
                               unpoweredEnergyDemandQ4) >> 4) + additionalEnergyDemand);
}


/* Address: 0x0053A2A0.
   Scores an army asset for the purchase planner with one weight profile (scoreWeights). The faction's unlocked
   variant of the asset's model definition gives baseScore (+ a bonus when +0x18 is nonzero) plus its +0x0C and
   +0x60 values scaled to their maxima, x8; for each of up to two linked child definitions (the weapons) with a
   nonzero divisor at +0x30 (and while the faction has AI pressure), the shot's impact damage against each of the 8 target classes is weighted
   by the profile, divided by that divisor, scaled by the faction's AI pressure on that class / maximum pressure
   and added as << 10 / the class maximum. Result: 12 * that + the weighted asset values at +0x70/+0x74/+0x78;
   0 when a definition is not available to the faction.
*/
AiCandidateScore32 AiArmyCandidate_ComputeFactionWeightedScore
          (AiArmyScoreWeights *scoreWeights,FactionRuntimeIndex factionIndex,
          ArmyAssetRecord *armyAssetRecord)

{
  ModelDefinitionResolveView *selectedModelDefinition;
  ModelDefinitionResolveView *selectedChildModelDefinition0;
  uint32_t pressureWeightedDamage0;
  uint32_t pressureWeightedDamage1;
  uint32_t pressureWeightedDamage2;
  uint32_t pressureWeightedDamage3;
  uint32_t pressureWeightedDamage4;
  uint32_t pressureWeightedDamage5;
  uint32_t pressureWeightedDamage6;
  uint32_t pressureWeightedDamage7;
  ModelDefinitionResolveView *selectedChildModelDefinition1;
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
  selectedModelDefinition = (ModelDefinitionResolveView *)definitionLookup.modelDefinition;
  if (definitionLookup.notFound) {
    return 0;
  }
  weightedDefinitionScore = scoreWeights->baseScore;
  if (selectedModelDefinition->accelerationPerTick18 != 0) {
    weightedDefinitionScore = weightedDefinitionScore + scoreWeights->nonzeroDefinition18Bonus;
  }
  childCountOrWeightedDamage = linkedDefinitionList->childListCount;
  weightedDefinitionScore =
       ((int)(((int64_t)(int)selectedModelDefinition->movementSpeed0C *
              (int64_t)scoreWeights->definitionValue0CWeight) /
             (int64_t)(int)g_AiArmyCandidateFlaggedDefinitionValueMaximum) +
       weightedDefinitionScore +
       (int)(((int64_t)(int)selectedModelDefinition->maximumHealth60 *
             (int64_t)scoreWeights->definitionValue60Weight) /
            (int64_t)
            (&g_TechnologyCategoryMaximum0)[selectedModelDefinition->categoryMaximumIndex5C])) * 8;
  if (childCountOrWeightedDamage != 0) {
    definitionLookup = ModelDefinition_SelectFactionUnlockedLinkedDefinition
                      (factionIndex,linkedDefinitionList->childList0Address);
    selectedChildModelDefinition0 = (ModelDefinitionResolveView *)definitionLookup.modelDefinition;
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
      selectedChildModelDefinition1 = (ModelDefinitionResolveView *)definitionLookup.modelDefinition;
      if (definitionLookup.notFound) {
        return 0;
      }
      if (selectedChildModelDefinition1->runtimeValue30 != 0) {
        secondChildShotDefinition = selectedChildModelDefinition1->shotDefinitionReference2C;
        secondChildScaleDivisor30 = selectedChildModelDefinition1->runtimeValue30;
        if (g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure != 0) {
          /* the locals are reused shifted by one class: childCountOrWeightedDamage holds class 0,
             pressureWeightedDamage0..6 hold classes 1..7 */
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

