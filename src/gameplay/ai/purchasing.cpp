/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/ai/purchasing.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/ai/purchasing.h>
#include <thandor/thandor.h>

/* Module data. */

const AiArmyScoreWeights g_AiArmyCandidateScoreWeightsVariantC15 = {
    .pressureCategoryWeights = {0, 384, 384, 384, 384, 448, 384, 512},
    .definitionValue60Weight = 384,
    .definitionValue0CWeight = 192,
    .armyRecord74Weight = 256,
    .armyRecord78Weight = 512,
    .nonzeroDefinition18Bonus = 4096,
    .baseScore = -4096};

static uint32_t g_AiPurchaseAppliedArmyClassMask = 0;

static const AiArmyScoreWeights g_AiArmyCandidateScoreWeightsVariantB15 = {
    .pressureCategoryWeights = {0, 128, 128, 128, 128, 128, 128, 128},
    .definitionValue0CWeight = 512,
    .armyRecord74Weight = 128,
    .armyRecord70Weight = 512,
    .nonzeroDefinition18Bonus = 4096,
    .baseScore = -4096};

static const AiArmyScoreWeights g_AiArmyCandidateScoreWeightsVariantA15 = {
    .pressureCategoryWeights = {0, 512, 512, 512, 512, 640, 512},
    .definitionValue60Weight = 512,
    .definitionValue0CWeight = -64,
    .armyRecord74Weight = 512,
    .armyRecord78Weight = 256,
    .nonzeroDefinition18Bonus = -256};

/* The army asset registry and workspace 11 keep ArmyAssetRecordPrefix pointers; every registered record is a whole
   ArmyAssetRecord (the prefix is its first 0x10 bytes), which the scoring reads. */
static ArmyAssetRecord *AiPurchase_FullAssetRecord(ArmyAssetRecordPrefix *assetPrefix)
{
  return reinterpret_cast<ArmyAssetRecord *>(assetPrefix);
}

/* Average faction-weighted score (AiArmyCandidate_ComputeFactionWeightedScore) of the enabled army assets
   that can carry the model definition of runtimeClassId: those whose own linked-definition list names it,
   or, once the technology of the asset's first linked definition is unlocked, one of its (up to two) child
   lists. Only positive scores count; 0 when the class has no definition or no asset scores. Only caller:
   AiTechnologyScore_ComputeCategoryCompatibleCandidateValue, which passes a technology id as runtimeClassId.
*/
AiCandidateScore32
AiArmyCandidate_ComputeAverageCompatibleAssetScore
          (const AiArmyScoreWeights *scoreWeights,FactionRuntimeIndex factionIndex,
          ModelRuntimeClassId runtimeClassId)

{
  ArmyAssetRecord *armyAssetRecord;
  AiLinkedDefinitionListView *assetDefinitionList;
  ModelDefinitionRecordPrefix *modelDefinition;
  AiCandidateScore32 candidateScore;
  int registryEntriesRemaining;
  ArmyAssetRecordPrefix **armyAssetRegistryCursor;
  Bool8 technologyLocked;
  int compatibleAssetCount;
  uint32_t compatibleAssetScoreSum;
  AiLinkedDefinitionListView *nestedLinkedDefinitions;
  PckModelDefinitionIdCatalog candidateModelDefinitionId;
  AiLinkedDefinitionListView *secondNestedLinkedDefinitions;
  
  modelDefinition = ModelDefinitionRegistry_FindByRuntimeClassId(runtimeClassId);
  candidateScore = 0;
  if (modelDefinition != nullptr) {
    candidateModelDefinitionId = modelDefinition->definitionId;
    armyAssetRegistryCursor = g_ArmyAssetRecordRegistry;
    registryEntriesRemaining = ARMY_ASSET_REGISTRY_SLOT_COUNT;
    compatibleAssetScoreSum = 0;
    compatibleAssetCount = 0;
    for (; registryEntriesRemaining != 0;
        armyAssetRegistryCursor++,
        registryEntriesRemaining--) {
      armyAssetRecord = AiPurchase_FullAssetRecord(*armyAssetRegistryCursor);
      if ((armyAssetRecord != nullptr) &&
         ((armyAssetRecord->flags & 1) != 0)) { /* bit 0: asset enabled */
        /* the asset's root node is an AiLinkedDefinitionListView */
        assetDefinitionList = Thandor_U32ToPointer<AiLinkedDefinitionListView>(armyAssetRecord->rootNodeOffsetOrPointer); /* 32-bit format field: ArmyAssetRecord.rootNodeOffsetOrPointer */
        if ((((candidateModelDefinitionId !=
               assetDefinitionList->definitionIds[0]) &&
             (((candidateModelDefinitionId !=
                assetDefinitionList->definitionIds[1] &&
               (candidateModelDefinitionId !=
                assetDefinitionList->definitionIds[2])) &&
              (candidateModelDefinitionId !=
               assetDefinitionList->definitionIds[3])))) &&
            (((candidateModelDefinitionId !=
               assetDefinitionList->definitionIds[4] &&
              (candidateModelDefinitionId !=
               assetDefinitionList->definitionIds[5])) &&
             (candidateModelDefinitionId !=
              assetDefinitionList->definitionIds[6])))) &&
           (candidateModelDefinitionId !=
            assetDefinitionList->definitionIds[7])) {
          /* true while the technology is still locked */
          technologyLocked = ModelDefinition_IsFactionTechnologyLocked
                            (g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits,
                             assetDefinitionList->definitionIds[0]);
          if (((technologyLocked) ||
              (nestedLinkedDefinitions =
               Thandor_U32ToPointer<AiLinkedDefinitionListView>(assetDefinitionList->childList0Address), /* 32-bit format field: ArmyModelTreeNode.childList0Address */
              assetDefinitionList->childListCount == 0)) ||
             ((((candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[0] &&
                ((((candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[1] &&
                   (candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[2])) &&
                  (candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[3])) &&
                 ((candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[4] &&
                  (candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[5])))))) &&
               ((candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[6] &&
                (candidateModelDefinitionId != nestedLinkedDefinitions->definitionIds[7])))) &&
              ((secondNestedLinkedDefinitions =
                Thandor_U32ToPointer<AiLinkedDefinitionListView>(assetDefinitionList->childList1Address), /* 32-bit format field: ArmyModelTreeNode.childList1Address */
               assetDefinitionList->childListCount < 2 ||
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

/* Buys the AI's candidates in descending weight order: each candidate's cost is taken from a running copy of
   the faction's Xenite, and every candidate with an eligible producer is applied, until one is no longer
   affordable. The cost is deducted even when no producer is found. Returns true when candidates
   existed but none was applied, false otherwise.
*/
Bool8 AiPurchasePlanner_ExecuteAffordableCandidates(FactionRuntimeIndex factionIndex)

{
  Bool8 noCandidateApplied;
  uint32_t candidateCost;
  int remainingCandidates;
  XeniteAmountQ4 remainingXenite;
  AiCandidateWorkspaceEntry *entry;
  Bool8 insufficientXenite;

  remainingCandidates = g_AiCandidateWorkspaceEntryCount;
  entry = g_AiWorkspace13Candidates;
  g_AiPurchaseAppliedArmyClassMask = 0;
  if (g_AiCandidateWorkspaceEntryCount == 0) {
    return false;
  }
  noCandidateApplied = true;
  AiCandidateWorkspace_SortDescending();
  remainingXenite = g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4;
  for (; remainingCandidates != 0; remainingCandidates--, entry++) {
    candidateCost = AiCandidateWorkspace_GetEntryXeniteCost(entry);
    insufficientXenite = remainingXenite < candidateCost;
    remainingXenite = remainingXenite - candidateCost;
    if (insufficientXenite) {
      return noCandidateApplied;
    }
    if (!AiPurchaseCandidate_HasEligibleProducer(entry,factionIndex)) {
      AiPurchaseCandidate_ApplyToFaction(entry,factionIndex);
      noCandidateApplied = false;
    }
  }
  return noCandidateApplied;
}

/* Proposes the best producible army asset for base defence. Once Arms Factories is researched and the planning
   capacity allows it, scores the eligible army assets of workspace 11 with weight profile A and proposes the best
   one. Its weight (armyVariantABaseWeight) is divided
   by 1 + the number of pending requests with ARM ids 340..379, then taken x3/4 while the faction's primary
   anchor cooldown is 0, else x2 (the cooldown runs after one of the faction's buildings was attacked). Assets below ARM 300 are only proposed while workspace 01 has at most 10
   entries.
*/
void AiArmyCandidate_AddBestDefenseAsset(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  AiCandidateScore32 candidateScore;
  uint32_t weightRange;
  int remainingEntries;
  uint32_t pendingRequestCount;
  uint32_t baseWeight;
  int bestScore;
  ArmyAssetRecordPrefix *bestArmyAsset;
  AiRuntimeWorkspaceEntry *runtimeWorkspaceEntry;
  ArmyAssetRecordPrefix **armyAssetRegistryCursor;

  if (g_AiWorkspace11Count == 0) {
    return;
  }
  bestScore = 0;
  if (AiFactionRuntime_TestPlanningCapacityExceeded(4,factionIndex)) {
    return;
  }
  /* Technology_IsUnlockedForFaction returns true while the technology is still locked */
  if (Technology_IsUnlockedForFaction(TEC_001_ARMS_FACTORIES,factionIndex)) {
    return;
  }
  armyAssetRegistryCursor = g_AiWorkspace11ProducibleAssets;
  for (remainingEntries = g_AiWorkspace11Count; remainingEntries != 0; remainingEntries--) {
    if ((AiPurchase_FullAssetRecord(*armyAssetRegistryCursor)->flags & 1) != 0) {
      candidateScore = AiArmyCandidate_ComputeFactionWeightedScore
                         (&g_AiArmyCandidateScoreWeightsVariantA15,factionIndex,
                          AiPurchase_FullAssetRecord(*armyAssetRegistryCursor));
      if (bestScore < candidateScore) {
        bestArmyAsset = *armyAssetRegistryCursor;
        bestScore = candidateScore;
      }
    }
    armyAssetRegistryCursor++;
  }
  if (bestScore <= 0) {
    return;
  }
  pendingRequestCount = 1;
  runtimeWorkspaceEntry = g_AiWorkspace04RequestedAssets;
  for (remainingEntries = g_AiWorkspace04Count; remainingEntries != 0; remainingEntries--) {
    if (((ARM_0340_BUILDING_MDL0314 - 1) < runtimeWorkspaceEntry->armyAssetId) &&
       (runtimeWorkspaceEntry->armyAssetId < ARM_0380_BUILDING_MDL0329)) {
      pendingRequestCount++;
    }
    runtimeWorkspaceEntry++;
  }
  baseWeight = g_AiKnowledgeData->parameters.armyVariantABaseWeight / pendingRequestCount;
  if (g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorCooldown == 0) {
    weightRange = baseWeight * 3 >> 2;
  }
  else {
    weightRange = baseWeight * 2;
  }
  if ((299 < bestArmyAsset->registryId) || (g_AiWorkspace01Count < 11)) {
    AiCandidateWorkspace_AddOrAccumulateWeightedEntry(bestArmyAsset->registryId,weightRange,1);
  }
}

/* Exploration: while there are general sites (workspace 05) and workspace 01 has at most 10 entries, scores
   the eligible army assets of workspace 11 with weight profile B and proposes the best one. Its weight grows
   with the unexplored share of the terrain, (100 - explored %) * coefficient / (32 * (workspace 01 count + 1)),
   halved while there are targets (workspace 07).
*/
void AiArmyCandidate_AddBestExplorationAsset(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

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
    for (; remainingEntries != 0; remainingEntries--) {
      if ((AiPurchase_FullAssetRecord(*armyAssetRegistryCursor)->flags & 1) != 0) {
        candidateScore = AiArmyCandidate_ComputeFactionWeightedScore
                          (&g_AiArmyCandidateScoreWeightsVariantB15,factionIndex,
                           AiPurchase_FullAssetRecord(*armyAssetRegistryCursor));
        if (bestScore < candidateScore) {
          bestArmyAsset = *armyAssetRegistryCursor;
          bestScore = candidateScore;
        }
      }
      armyAssetRegistryCursor++;
    }
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
}

/* Attack: while there are targets (workspace 07) and workspace 01 has at most 10 entries, scores the eligible
   army assets of workspace 11 with weight profile C and proposes the best one with armyVariantCBaseWeight.
   The halving for an empty workspace 07 can never apply (the entry check requires targets); the original
   has the same dead test.
*/
void AiArmyCandidate_AddBestAttackAsset(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

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
    for (; remainingEntries != 0; remainingEntries--) {
      if ((AiPurchase_FullAssetRecord(*armyAssetRegistryCursor)->flags & 1) != 0) {
        candidateScore = AiArmyCandidate_ComputeFactionWeightedScore
                          (&g_AiArmyCandidateScoreWeightsVariantC15,factionIndex,
                           AiPurchase_FullAssetRecord(*armyAssetRegistryCursor));
        if (bestScore < candidateScore) {
          bestArmyAsset = *armyAssetRegistryCursor;
          bestScore = candidateScore;
        }
      }
      armyAssetRegistryCursor++;
    }
    if (0 < bestScore) {
      weightRange = g_AiKnowledgeData->parameters.armyVariantCBaseWeight;
      if (g_AiWorkspace07Count == 0) {
        weightRange = weightRange >> 1;
      }
      AiCandidateWorkspace_AddOrAccumulateWeightedEntry(bestArmyAsset->registryId,weightRange,1);
    }
  }
}

/* True when technologyIndex is one of the research slots researchTechnologyIds[1..28] of the structure
   definition (slot 0 is not a research slot). */
static Bool8 AiPurchaseCandidate_DefinitionListsResearch(ModelDefinition *definition,RuntimeToken technologyIndex)
{
  int technologySlotIndex;

  for (technologySlotIndex = 28; technologySlotIndex != 0; technologySlotIndex--) {
    if (technologyIndex == definition->researchTechnologyIds[technologySlotIndex]) {
      return true;
    }
  }
  return false;
}

/* Returns false when an idle structure of workspace 00 can carry out a purchase candidate. A
   technology must be available to the faction and listed in the 28 research slots (researchTechnologyIds[1..28])
   of the definition of a structure whose runtimeFlags have none of 0x89. An army asset's producer class mask (the
   asset's flags) must not overlap g_AiPurchaseAppliedArmyClassMask (one purchase per producer class and round);
   bit 0x10 needs a class-11 structure with classState.behaviorState clear, bit 0x08 a class-22 structure with
   classState.classStateAC clear, the bits 0xEE a class-13 structure whose definition mask classParameterC4
   shares them and with classState.behaviorState clear (runtimeFlags without 0xC9 each).
*/
Bool8 AiPurchaseCandidate_HasEligibleProducer(AiCandidateWorkspaceEntry *candidateEntry,FactionRuntimeIndex factionIndex)

{
  uint32_t producerClassMask;
  ModelRuntimeSlot *entitySlot;
  uint32_t remainingEntries;
  RuntimeToken candidateId;
  AiStructureWorkspaceEntry *workspaceEntry;
  ArmyAssetRecordPrefix *armyAsset;

  candidateId = candidateEntry->entityIdAndMultiplicity & AI_CANDIDATE_ID_MASK;
  workspaceEntry = g_AiWorkspace00Structures;
  if ((candidateEntry->weightedScoreAndKind & AI_CANDIDATE_KIND_MASK) == AI_CANDIDATE_KIND_TECHNOLOGY) {
    if (!Technology_IsAvailableForFaction(candidateId,factionIndex)) {
      return true;
    }
    for (remainingEntries = g_AiWorkspace00Count; remainingEntries != 0; remainingEntries--, workspaceEntry++) {
      entitySlot = workspaceEntry->runtimeSlotAddressOrZero;
      if ((entitySlot != nullptr) &&
          (!Any(entitySlot->classState.stateFlags & (ARMY_MODEL_STATE_INACTIVE_MASK | ARMY_MODEL_STATE_RESEARCH_UNPAID))) &&
          AiPurchaseCandidate_DefinitionListsResearch
            (entitySlot->definitionOrSavedId.runtimeDefinition,candidateId)) {
        return false;
      }
    }
    return true;
  }
  /* Original quirk: the lookup status is not checked; the class mask (the asset's flags) is read through
     whatever armyAsset holds (the error code for an unknown id) */
  ArmyAssetRegistry_FindById(candidateId,&armyAsset);
  producerClassMask = armyAsset[1].selectionDetailTemplateVariantIndex;
  if ((g_AiWorkspace00Count == 0) || ((g_AiPurchaseAppliedArmyClassMask & producerClassMask) != 0)) {
    return true;
  }
  if ((producerClassMask & ARMY_ASSET_FLAG_BUILT_BY_CLASS11) != 0) {
    for (remainingEntries = g_AiWorkspace00Count; remainingEntries != 0; remainingEntries--, workspaceEntry++) {
      entitySlot = workspaceEntry->runtimeSlotAddressOrZero;
      if ((entitySlot != nullptr) &&
          (entitySlot->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_11) &&
          (!Any(entitySlot->classState.stateFlags & ARMY_MODEL_STATE_BUILD_BLOCKING_MASK)) && (entitySlot->classState.behaviorState == 0)) {
        return false;
      }
    }
  }
  else if ((producerClassMask & ARMY_ASSET_FLAG_BUILT_AT_AIRCRAFT_PAD) != 0) {
    for (remainingEntries = g_AiWorkspace00Count; remainingEntries != 0; remainingEntries--, workspaceEntry++) {
      entitySlot = workspaceEntry->runtimeSlotAddressOrZero;
      if ((entitySlot != nullptr) &&
          (entitySlot->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_22) &&
          (!Any(entitySlot->classState.stateFlags & ARMY_MODEL_STATE_BUILD_BLOCKING_MASK)) && (entitySlot->classState.classStateAC == 0)) {
        return false;
      }
    }
  }
  else if ((producerClassMask & ARMY_ASSET_FLAGS_BUILT_BY_FACTORY) != 0) {
    for (remainingEntries = g_AiWorkspace00Count; remainingEntries != 0; remainingEntries--, workspaceEntry++) {
      entitySlot = workspaceEntry->runtimeSlotAddressOrZero;
      if ((entitySlot != nullptr) &&
          (entitySlot->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_13) &&
          (!Any(entitySlot->classState.stateFlags & ARMY_MODEL_STATE_BUILD_BLOCKING_MASK)) &&
          ((entitySlot->definitionOrSavedId.runtimeDefinition->classParameterC4 &
            producerClassMask & ARMY_ASSET_FLAGS_BUILT_BY_FACTORY) != 0) &&
          (entitySlot->classState.behaviorState == 0)) {
        return false;
      }
    }
  }
  return true;
}

/* Carries out an affordable purchase candidate. A technology starts researching at the first idle workspace-00
   structure that lists it (the checks of AiPurchaseCandidate_HasEligibleProducer), whose workspace slot is then
   cleared so it takes no second job this round. An army asset is appended once to the faction's pending asset
   list, and its producer class mask is added to g_AiPurchaseAppliedArmyClassMask.
*/
void AiPurchaseCandidate_ApplyToFaction(AiCandidateWorkspaceEntry *candidateEntry,FactionRuntimeIndex factionIndex)

{
  GameEntityRuntime *entity;
  int remainingEntries;
  AiStructureWorkspaceEntry *workspaceEntry;
  RuntimeToken candidateId;
  ArmyAssetRecordPrefix *armyAsset;

  candidateId = candidateEntry->entityIdAndMultiplicity & AI_CANDIDATE_ID_MASK;
  workspaceEntry = g_AiWorkspace00Structures;
  if ((candidateEntry->weightedScoreAndKind & AI_CANDIDATE_KIND_MASK) == AI_CANDIDATE_KIND_TECHNOLOGY) {
    for (remainingEntries = g_AiWorkspace00Count; remainingEntries != 0; remainingEntries--, workspaceEntry++) {
      /* the entity view of the model runtime (runtimeFlags = classState.stateFlags), as Technology_ApplyRecordToEntity
         takes it */
      entity = reinterpret_cast<GameEntityRuntime *>(workspaceEntry->runtimeSlotAddressOrZero.get());
      if ((entity != nullptr) &&
          (!Any((entity->common).runtimeFlags & (ARMY_MODEL_STATE_INACTIVE_MASK | ARMY_MODEL_STATE_RESEARCH_UNPAID))) &&
          AiPurchaseCandidate_DefinitionListsResearch
            (entity->common.ownership.modelDefinition(),candidateId)) {
        Technology_ApplyRecordToEntity(candidateId,entity);
        workspaceEntry->runtimeSlotAddressOrZero = nullptr;
        return;
      }
    }
  }
  else {
    GameFactionRuntime_RegisterArmyAssetPointers(UINT32_MAX,1,candidateId,factionIndex);
    if (ArmyAssetRegistry_FindById(candidateId,&armyAsset) == 0) {
      g_AiPurchaseAppliedArmyClassMask =
           g_AiPurchaseAppliedArmyClassMask | armyAsset[1].selectionDetailTemplateVariantIndex;
    }
  }
}

/* Proposes a resource storage building when the faction's free storage runs low (ARM 331 for Xenite, the
   caller's candidate, ARM 333, for Tritium), plus extra weight per usable resource site.
   Proposes the resource structure candidateArmyAssetId unless one of it is still unassigned. ARM 331 (0x14B):
   with an ARM 330 present and the Xenite storage limit below the knowledge limit, when the free storage
   (limit - current Xenite) is below the gap limit; a full storage quadruples the weight. Any other candidate
   (Tritium): with an ARM 332 present, the same test on the Tritium storage; independently of that the
   first workspace 08 site that still allows an extra special site adds the candidate again with a weight
   3 * derived / (existing count of that site's structure + 3).
*/
void AiStructureCandidate_AddResourceStorage
          (AiCandidateScore32 baseWeight,PckArmyAssetIdCatalog candidateArmyAssetId,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  uint32_t storageLimit;
  uint32_t xeniteCurrent;
  AiTerrainFeatureWorkspaceEntry *terrainFeatureEntry;
  int remainingEntries;
  int assignedSiteStructureCount;
  uint32_t derivedWeight;
  Bool8 hasPrerequisite;
  AiKnowledgeDataImage *knowledgeData;

  if (AiPrimaryWorkspace_HasUnassignedEntryById(candidateArmyAssetId)) {
    return;
  }
  knowledgeData = g_AiKnowledgeData;
  if (candidateArmyAssetId == ARM_0331_BUILDING_MDL0308) {
    storageLimit = g_GameFactionRuntimeImage.records[factionIndex].xeniteStorageLimitQ4;
    hasPrerequisite = AiPrimaryWorkspace_HasEntryById(ARM_0330_BUILDING_MDL0303);
    if (hasPrerequisite && (storageLimit != 0) &&
        (storageLimit < knowledgeData->parameters.structure14bPrerequisite14aCountLimit)) {
      xeniteCurrent = g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4;
      if (storageLimit == xeniteCurrent) {
        baseWeight = baseWeight << 2;
      }
      if (storageLimit - xeniteCurrent < knowledgeData->parameters.structure14bCountGapLimit) {
        AiCandidateWorkspace_AddOrAccumulateWeightedEntry(ARM_0331_BUILDING_MDL0308,baseWeight,1);
      }
    }
    return;
  }
  storageLimit = g_GameFactionRuntimeImage.records[factionIndex].tritiumStorageLimitQ4;
  hasPrerequisite = AiPrimaryWorkspace_HasEntryById(ARM_0332_BUILDING_MDL0302);
  if (hasPrerequisite && (storageLimit != 0) &&
      (storageLimit < knowledgeData->parameters.structure14dPrerequisite14cCountLimit) &&
      (storageLimit - g_GameFactionRuntimeImage.records[factionIndex].tritiumCurrentQ4 <
       knowledgeData->parameters.structure14dCountGapLimit)) {
    AiCandidateWorkspace_AddOrAccumulateWeightedEntry(candidateArmyAssetId,baseWeight,1);
  }
  terrainFeatureEntry = g_AiWorkspace08TerrainFeatureSites;
  for (remainingEntries = g_AiWorkspace08Count; remainingEntries != 0; remainingEntries--, terrainFeatureEntry++) {
    if (!AiPlacement_ReserveAdditionalSpecialSite
           (terrainFeatureEntry->armyAssetId,terrainFeatureEntry->cell,factionIndex,worldRuntime)) {
      derivedWeight = knowledgeData->parameters.workspace08Id14aDerivedWeight;
      if (terrainFeatureEntry->armyAssetId != ARM_0330_BUILDING_MDL0303) {
        derivedWeight = knowledgeData->parameters.workspace08OtherDerivedWeight;
      }
      assignedSiteStructureCount = AiPrimaryWorkspace_CountAssignedEntriesById(terrainFeatureEntry->armyAssetId);
      AiCandidateWorkspace_AddOrAccumulateWeightedEntry
                (candidateArmyAssetId,(derivedWeight * 3) / (assignedSiteStructureCount + 3U),1);
      return;
    }
  }
}

/* Proposes ARM 310 (0x136) once an ARM 330 exists and no ARM 310 is unassigned, when baseline Energy supply
   plus the record's rate tritiumExtractionRateQ4PerTick exceeds the Energy generation capacity.
   Weight: surplus * demand / capacity * resource136DeficitScoreNumerator / resource136DeficitScoreDenominator
   (Q4 values taken as integers, demand at least 1).
*/
void AiResourceCandidate_AddPowerPlant(FactionRuntimeIndex factionIndex)

{
  int energySupply;
  int energySurplus;
  int energyDemand;
  int generationCapacity;
  Bool8 conditionMet;
  
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
}

/* Pressure-weighted damage score of one weapon (a linked child definition) for
   AiArmyCandidate_ComputeFactionWeightedScore: for each of the 8 target classes the shot's impact damage times the
   profile weight / reloadTicks, scaled by the faction's AI pressure on that class / its maximum AI pressure, added
   as << 10 / the class maximum (g_TechnologyCategoryMaximums[0..7]). reloadTicks and the maximum pressure must be
   nonzero. */
static int AiArmyCandidate_ScoreWeaponPressureDamage
          (const AiArmyScoreWeights *scoreWeights,FactionRuntimeIndex factionIndex,ShotDefinition *shotDefinition,
          int reloadTicks)
{
  int targetClass;
  uint32_t pressureWeightedDamage;
  int damageScore;

  damageScore = 0;
  for (targetClass = 0; targetClass < 8; targetClass++) {
    pressureWeightedDamage =
         (uint32_t)(((int64_t)
                 (int)(((int64_t)shotDefinition->targetClassImpactDamageQ12[targetClass] *
                       (int64_t)scoreWeights->pressureCategoryWeights[targetClass]) /
                      (int64_t)reloadTicks) *
                (int64_t)g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[targetClass]) /
               (int64_t)g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure);
    damageScore = damageScore +
         (int)((int64_t)((uint64_t)pressureWeightedDamage << 10) /
              (int64_t)(int)g_TechnologyCategoryMaximums[targetClass]);
  }
  return damageScore;
}

/* Scores an army asset for the purchase planner with one weight profile (scoreWeights). The faction's unlocked
   variant of the asset's model definition gives baseScore (+ a bonus when accelerationPerTick is nonzero) plus
   its movementSpeed and maximumHealth values scaled to their maxima, x8; for each of up to two linked child
   definitions (the weapons) with a nonzero divisor reloadTicks (and while the faction has AI pressure), the
   shot's impact damage against each of the 8 target classes is weighted
   by the profile, divided by that divisor, scaled by the faction's AI pressure on that class / maximum pressure
   and added as << 10 / the class maximum. Result: 12 * that + the weighted asset values definitionClassValue70/74/78;
   0 when a definition is not available to the faction.
*/
AiCandidateScore32 AiArmyCandidate_ComputeFactionWeightedScore
          (const AiArmyScoreWeights *scoreWeights,FactionRuntimeIndex factionIndex,
          ArmyAssetRecord *armyAssetRecord)

{
  ModelDefinitionResolveView *selectedModelDefinition;
  ModelDefinitionResolveView *selectedChildModelDefinition0;
  ModelDefinitionResolveView *selectedChildModelDefinition1;
  uint32_t childCount;
  int weightedDefinitionScore;
  AiLinkedDefinitionListView *linkedDefinitionList;

  linkedDefinitionList = Thandor_U32ToPointer<AiLinkedDefinitionListView>(armyAssetRecord->rootNodeOffsetOrPointer); /* 32-bit format field: ArmyAssetRecord.rootNodeOffsetOrPointer */
  /* the original tests the selector's status after each of the three selections below, but it is always
     "found"; the selected record (a ModelDefinition) is read through the ModelDefinitionResolveView field view */
  selectedModelDefinition = reinterpret_cast<ModelDefinitionResolveView *>(ModelDefinition_SelectFactionUnlockedLinkedDefinition
                    (factionIndex,reinterpret_cast<uintptr_t>(linkedDefinitionList)));
  weightedDefinitionScore = scoreWeights->baseScore;
  if (selectedModelDefinition->accelerationPerTick != 0) {
    weightedDefinitionScore = weightedDefinitionScore + scoreWeights->nonzeroDefinition18Bonus;
  }
  childCount = linkedDefinitionList->childListCount;
  weightedDefinitionScore =
       ((int)(((int64_t)(int)selectedModelDefinition->movementSpeed *
              (int64_t)scoreWeights->definitionValue0CWeight) /
             (int64_t)(int)g_AiArmyCandidateFlaggedDefinitionValueMaximum) +
       weightedDefinitionScore +
       (int)(((int64_t)(int)selectedModelDefinition->maximumHealth *
             (int64_t)scoreWeights->definitionValue60Weight) /
            (int64_t)
            g_TechnologyCategoryMaximums[selectedModelDefinition->targetClassIndex])) * 8;
  if (childCount != 0) {
    selectedChildModelDefinition0 = reinterpret_cast<ModelDefinitionResolveView *>(ModelDefinition_SelectFactionUnlockedLinkedDefinition
                      (factionIndex,linkedDefinitionList->childList0Address)); /* 32-bit format field: ArmyModelTreeNode.childList*Address */
    if ((selectedChildModelDefinition0->reloadTicks != 0) &&
        (g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure != 0)) {
      weightedDefinitionScore = weightedDefinitionScore +
           AiArmyCandidate_ScoreWeaponPressureDamage
             (scoreWeights,factionIndex,selectedChildModelDefinition0->shotDefinitionReference,
              selectedChildModelDefinition0->reloadTicks);
    }
    if (1 < childCount) {
      selectedChildModelDefinition1 = reinterpret_cast<ModelDefinitionResolveView *>(ModelDefinition_SelectFactionUnlockedLinkedDefinition
                        (factionIndex,linkedDefinitionList->childList1Address)); /* 32-bit format field: ArmyModelTreeNode.childList*Address */
      if ((selectedChildModelDefinition1->reloadTicks != 0) &&
          (g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure != 0)) {
        weightedDefinitionScore = weightedDefinitionScore +
             AiArmyCandidate_ScoreWeaponPressureDamage
               (scoreWeights,factionIndex,selectedChildModelDefinition1->shotDefinitionReference,
                selectedChildModelDefinition1->reloadTicks);
      }
    }
  }
  return scoreWeights->armyRecord74Weight * armyAssetRecord->definitionClassValue74 +
         weightedDefinitionScore * 12 +
         scoreWeights->armyRecord78Weight * armyAssetRecord->definitionClassValue78 +
         scoreWeights->armyRecord70Weight * armyAssetRecord->definitionClassValue70;
}
