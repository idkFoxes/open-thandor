/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/ai/planning.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/ai/planning.h>
#include <thandor/thandor.h>

/* Module data. */

uint32_t g_AiActiveGridMaskClasses[4] = {0, 0, 0, 0};

ModelRuntimeSlot *g_AiWorkspaceOwnedAsset300Runtime = 0;

const AiArmyScoreWeights g_AiArmyCandidateScoreWeightsVariantC15 = {
    .pressureCategoryWeights = {0, 384, 384, 384, 384, 448, 384, 512},
    .definitionValue60Weight = 384,
    .definitionValue0CWeight = 192,
    .armyRecord74Weight = 256,
    .armyRecord78Weight = 512,
    .nonzeroDefinition18Bonus = 4096,
    .baseScore = -4096};

static uint32_t g_AiPurchaseAppliedArmyClassMask = 0;

static uint32_t g_AiConstructionPendingAssetConsumedCount = 0;

static const int32_t g_AiStrategicClassTerrainWeights[5][3] = {
    {256, 0, 0},
    {240, 16, 0},
    {0, 176, 80},
    {0, 0, 256},
    {0, 0, 0}};

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

/* Implementation ownership: gameplay/ai/planning. */

/* Runs the planning phase for every active AI faction (1..7, a faction without a player block) and scales its
   terrain contribution by the game speed, then rebuilds the per-faction AI pressure table: each of the eight
   pressure channels decays to about 3/4, every runtime model adds 0x100 to the channel of its definition
   (targetClassIndex, stock data ~0) for each other faction flagged in its faction mask, and the channel maximum
   is stored.
   Called on tick-wheel cases 2 and 6.
*/
void AiFactionRuntime_RebuildPlanningCapacityState(void)

{
  GameSpeedQ8 currentGameSpeedQ8;
  int factionIndex;
  int recordCount;
  int pressureChannel;
  int pressureValue;
  FrontendPlayerRuntimeBlockCount remainingPlayerBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  uint32_t factionBit;
  uint32_t channel;
  uint32_t targetFactionIndex;
  GameFactionRuntimeRecord *pressureTargetRecord;
  GameFactionRuntimeRecord *planningRecord;
  GameFactionRuntimeRecord *decayRecord;
  GameFactionRuntimeRecord *maximumScanRecord;
  FactionRuntimeLifecycleObservedState *lifecycleState;
  WorldOwnerListNode *ownerNode;
  ArmyRuntimeSlot *ownerArmy;

  planningRecord = g_GameFactionRuntimeImage.records;
  lifecycleState = g_GameFactionRuntimeImage.tail.factionLifecycleStates;
  for (factionIndex = 1; factionIndex <= 7; factionIndex++) {
    remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount;
    playerBlock = g_FrontendPlayerRuntimeBlocks;
    lifecycleState++;
    planningRecord++;
    planningRecord->terrainContributionScaleQ8 = Q8_ONE;
    currentGameSpeedQ8 = g_GameFactionRuntimeImage.tail.gameSpeedQ8;
    if (*lifecycleState == FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
      /* Only factions without a player block are AI-controlled. */
      do {
        if (factionIndex == (playerBlock->factionAssignment).factionAssignmentIndex) break;
        playerBlock++;
        remainingPlayerBlocks--;
      } while (remainingPlayerBlocks != 0);
      if (remainingPlayerBlocks == 0) {
        AiRuntime_DispatchFactionPlanningPhase(factionIndex,g_InGameRuntimeRoot);
        planningRecord->terrainContributionScaleQ8 = currentGameSpeedQ8;
      }
    }
  }
  /* Decay: value = ((value * 3 + 1) >> 2) + 1. */
  decayRecord = g_GameFactionRuntimeImage.records + 1;
  for (recordCount = 7; recordCount != 0; recordCount--) {
    for (channel = 0; channel < 8; channel++) {
      decayRecord->aiPressureValues[channel] = ((decayRecord->aiPressureValues[channel] * 3 + 1U) >> 2) + 1;
    }
    decayRecord->maximumAiPressure = 0;
    decayRecord++;
  }
  ownerNode = (g_InGameRuntimeRoot->worldRuntime).ownerListHead;
  if (ownerNode == NULL) {
    return;
  }
  for (; ownerNode != NULL; ownerNode = ownerNode->nextNode) {
    if (ownerNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) continue;
    /* runtimePayload is the ModelRuntimeSlot: its definition's target class is the pressure channel;
       the owner army holds the owning faction and a faction mask terrainOccupancyMask0 (bit 3 + 2 * (f - 1) for
       faction f). */
    ownerArmy = ((ModelRuntimeSlot *)ownerNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime;
    if (ownerArmy->factionIndex == 0) continue;
    pressureChannel =
         ((ModelRuntimeSlot *)ownerNode->runtimePayload)->definitionOrSavedId.runtimeDefinition->targetClassIndex;
    factionBit = 8;
    pressureTargetRecord = g_GameFactionRuntimeImage.records;
    for (targetFactionIndex = 1; targetFactionIndex < 8; targetFactionIndex++) {
      pressureTargetRecord++;
      if (((ownerArmy->terrainOccupancyMask0 & factionBit) != 0) &&
         (targetFactionIndex != (uint32_t)ownerArmy->factionIndex)) {
        pressureTargetRecord->aiPressureValues[pressureChannel] =
             pressureTargetRecord->aiPressureValues[pressureChannel] + 256;
      }
      factionBit = factionBit << 2;
    }
  }
  maximumScanRecord = g_GameFactionRuntimeImage.records + 1;
  for (recordCount = 7; recordCount != 0; recordCount--) {
    for (channel = 0; channel < 8; channel += 2) {
      pressureValue = maximumScanRecord->aiPressureValues[channel + 1];
      if (maximumScanRecord->maximumAiPressure < maximumScanRecord->aiPressureValues[channel]) {
        maximumScanRecord->maximumAiPressure = maximumScanRecord->aiPressureValues[channel];
      }
      if (maximumScanRecord->maximumAiPressure < pressureValue) {
        maximumScanRecord->maximumAiPressure = pressureValue;
      }
    }
    maximumScanRecord++;
  }
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
          /* true while the technology is still locked */
          technologyLocked = ModelDefinition_IsFactionTechnologyLocked
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


/* Collects up to four distinct movement masks of the faction's own units (workspace 01) into
   g_AiActiveGridMaskClasses[0..3] (0xFFFFFFFF = unused): GRID_SCRATCH_BLOCKED | distance-band bit (8 +
   footprintRadiusClass) | terrain bit (24 + terrainTraversalClass), both taken from the definition of the unit's
   model runtime; only mobile units (accelerationPerTick != 0) with non-negative classes count. The site scan accepts
   a general site when its scratch neighbourhood avoids every bit of one of these masks. Without any unit
   class 0 falls back to 0x90000100 (band bit 8, terrain bit 28).
*/
void AiPlanning_CollectActiveGridMaskClasses(void)

{
  ModelDefinition *unitDefinition;
  int footprintRadiusClass;
  uint32_t terrainTraversalClass;
  uint32_t combinedMask;
  int remainingEntries;
  AiRuntimeWorkspaceEntry *runtimeWorkspaceEntry;

  g_AiActiveGridMaskClasses[0] = AI_GRID_MASK_CLASS_FREE;
  g_AiActiveGridMaskClasses[1] = AI_GRID_MASK_CLASS_FREE;
  g_AiActiveGridMaskClasses[2] = AI_GRID_MASK_CLASS_FREE;
  g_AiActiveGridMaskClasses[3] = AI_GRID_MASK_CLASS_FREE;
  runtimeWorkspaceEntry = g_AiWorkspace01Units;
  for (remainingEntries = g_AiWorkspace01Count; remainingEntries != 0; remainingEntries--, runtimeWorkspaceEntry++) {
    if (runtimeWorkspaceEntry->modelRuntime == NULL) {
      continue;
    }
    unitDefinition = runtimeWorkspaceEntry->modelRuntime->definitionOrSavedId.runtimeDefinition;
    footprintRadiusClass = unitDefinition->footprintRadiusClass;
    if ((unitDefinition->accelerationPerTick == 0) || (footprintRadiusClass < 0)) {
      continue;
    }
    terrainTraversalClass = unitDefinition->terrainTraversalClass;
    if ((int)terrainTraversalClass < 0) {
      continue;
    }
    combinedMask = GRID_SCRATCH_LOW_BAND0 << ((uint8_t)footprintRadiusClass & 31) | GRID_SCRATCH_BLOCKED |
                   GRID_SCRATCH_TERRAIN_CLASS_BIT24 << ((uint8_t)terrainTraversalClass & 31);
    if ((combinedMask == g_AiActiveGridMaskClasses[0]) || (combinedMask == g_AiActiveGridMaskClasses[1]) ||
        (combinedMask == g_AiActiveGridMaskClasses[2]) || (combinedMask == g_AiActiveGridMaskClasses[3])) {
      continue;
    }
    /* a new mask goes into the first free slot; with all four slots taken it is dropped */
    if (g_AiActiveGridMaskClasses[0] == AI_GRID_MASK_CLASS_FREE) {
      g_AiActiveGridMaskClasses[0] = combinedMask;
    }
    else if (g_AiActiveGridMaskClasses[1] == AI_GRID_MASK_CLASS_FREE) {
      g_AiActiveGridMaskClasses[1] = combinedMask;
    }
    else if (g_AiActiveGridMaskClasses[2] == AI_GRID_MASK_CLASS_FREE) {
      g_AiActiveGridMaskClasses[2] = combinedMask;
    }
    else if (g_AiActiveGridMaskClasses[3] == AI_GRID_MASK_CLASS_FREE) {
      g_AiActiveGridMaskClasses[3] = combinedMask;
    }
  }
  if (g_AiActiveGridMaskClasses[0] == AI_GRID_MASK_CLASS_FREE) {
    g_AiActiveGridMaskClasses[0] = GRID_SCRATCH_BLOCKED | GRID_SCRATCH_TERRAIN_CLASS_BIT28 | GRID_SCRATCH_LOW_BAND0;
  }
  return;
}


/* AI planning job of the simulation step for one faction. Each faction gets its turn every 64 simulation ticks
   (bits 3-5 of the tick select the faction); bit 6 alternates between the unit phase (behaviour update and
   group-to-target assignment) and the economy phase (relations, pending construction requests, then the
   purchase candidates, which are reused from the faction's cache while it is valid and the anchor cooldowns
   are below 50).
*/
void AiRuntime_DispatchFactionPlanningPhase(FactionRuntimeIndex factionIndex,InGameRuntimeRoot *inGameRoot)

{
  uint32_t planningPhaseDispatchIndex;
  WorldRuntimeContext *worldRuntime;
  bool phaseResult;
  AiKnowledgeDataImage *knowledgeData;

  if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
       SESSION_NETWORK_ROLE_LOCAL) || ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_AI_PLANNING_OFF) == 0)) {
    planningPhaseDispatchIndex = g_GameFactionRuntimeImage.tail.simulationTick >> 6 & 1;
    /* the world runtime of the in-game root (the caller passes g_InGameRuntimeRoot) */
    worldRuntime = &inGameRoot->worldRuntime;
    if ((g_GameFactionRuntimeImage.tail.simulationTick >> 3 & 7) == factionIndex) {
      AiPlanning_RebuildFactionWorkspaces
                (planningPhaseDispatchIndex,factionIndex,factionIndex,
                 worldRuntime);
      switch(planningPhaseDispatchIndex) {
      case 0:
        AiUnitBehavior_UpdateOwnUnits(factionIndex,worldRuntime);
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
            AiResourceCandidate_AddPowerPlant(factionIndex);
            AiWorkspaceAssetCandidate_AddWeightedEntry
                      ((knowledgeData->parameters).specialSite14aBaseWeight,
                       ARM_0330_BUILDING_MDL0303,factionIndex,worldRuntime);
            AiWorkspaceAssetCandidate_AddWeightedEntry
                      ((knowledgeData->parameters).specialSite14cBaseWeight,
                       ARM_0332_BUILDING_MDL0302,factionIndex,worldRuntime);
            AiStructureCandidate_AddResourceStorage
                      ((knowledgeData->parameters).structure14bBaseWeight,ARM_0331_BUILDING_MDL0308,
                       factionIndex,worldRuntime);
            AiStructureCandidate_AddResourceStorage
                      ((knowledgeData->parameters).structure14dBaseWeight,ARM_0333_BUILDING_MDL0307,
                       factionIndex,worldRuntime);
            AiCandidatePlanning_AddSpecialSiteCandidate
                      (factionIndex,worldRuntime);
            AiArmyCandidate_AddBestDefenseAsset(factionIndex,worldRuntime);
            AiArmyCandidate_AddBestExplorationAsset(factionIndex,worldRuntime);
            AiArmyCandidate_AddBestAttackAsset(factionIndex,worldRuntime);
            AiStrategicClass_AddArmsFactoriesStageBuilding
                      (factionIndex,worldRuntime);
            AiStrategicClass_AddWeightedClassCandidate
                      (factionIndex,worldRuntime);
            AiTechnologyCandidate_AddBestResearch
                      (factionIndex,worldRuntime);
            phaseResult = AiPurchasePlanner_ExecuteAffordableCandidates(factionIndex);
            if (phaseResult) {
              AiCandidateWorkspace_SaveToFactionImage(factionIndex * (int)sizeof(GameFactionRuntimeRecord));
              g_GameFactionRuntimeImage.records[factionIndex].candidateCache.cacheReuseState = 16;
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


/* Works through the faction's pending asset requests (workspace 04) in order and hands each to its placement
   handler by ARM id: 300 only while no unassigned 330 exists, 330/332 at a workspace site, 333 derived from a
   330/332 site, other ids below 340 at a reachable candidate, ids from 340 on near the faction anchor.
   Returns true as soon as a handler has placed an asset (g_AiConstructionPendingAssetConsumedCount).
*/
bool AiConstructionPlanner_ProcessPendingAssetRequests
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  PckArmyAssetIdCatalog armyAssetId;
  int remainingRequests;
  AiRuntimeWorkspaceEntry *requestEntry;
  ArmyAssetRecordPrefix *armyAsset;
  
  g_AiConstructionPendingAssetConsumedCount = 0;
  requestEntry = g_AiWorkspace04RequestedAssets;
  for (remainingRequests = g_AiWorkspace04Count; remainingRequests != 0; remainingRequests--, requestEntry++) {
    armyAssetId = requestEntry->armyAssetId;
    if (armyAssetId == ARM_0300_BUILDING_MDL0301) {
      if (!AiPrimaryWorkspace_HasUnassignedEntryById(ARM_0330_BUILDING_MDL0303)) {
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
      AiConstructionPlanner_PlaceTritiumStorageNearResourceSite
                (ARM_0333_BUILDING_MDL0307,factionIndex,worldRuntime);
    }
    else if (armyAssetId < ARM_0340_BUILDING_MDL0314) {
      if (ArmyAssetRegistry_FindById(armyAssetId,&armyAsset) == 0) {
        /* The original then compares the selected definition's placementContactKindIndex with 1 (ignoring the
           selector's status), but both outcomes call the same placement handler. */
        (void)ModelDefinition_SelectFactionUnlockedLinkedDefinition
                          (factionIndex,armyAsset->rootNodeOffsetOrPointer);
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
  }
  return false;
}


/* Buys the AI's candidates in descending weight order: each candidate's cost is taken from a running copy of
   the faction's Xenite, and every candidate with an eligible producer is applied, until one is no longer
   affordable. The cost is deducted even when no producer is found. Returns true when candidates
   existed but none was applied, false otherwise.
*/
bool AiPurchasePlanner_ExecuteAffordableCandidates(FactionRuntimeIndex factionIndex)

{
  bool noCandidateApplied;
  uint32_t candidateCost;
  int remainingCandidates;
  XeniteAmountQ4 remainingXenite;
  AiCandidateWorkspaceEntry *entry;
  bool insufficientXenite;

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


/* Tries to build armyAssetId next to the workspace-08 resource site siteEntry: when
   AiPlacement_ReserveAdditionalSpecialSite accepts the site and a placeable base cell is found near it, the asset
   is created there (technology unlocked), its model node's movement position cleared, its transforms rebuilt, its
   class command dispatched, the effect named by the created model's attachment 2 started at the node and the asset
   removed from the faction's pending list. Returns true when the attempt is over (asset created, or its creation
   failed), false when the site is not usable. */
static bool AiConstructionPlanner_TryPlaceStorageAtResourceSite
          (AiTerrainFeatureWorkspaceEntry *siteEntry,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)
{
  FieldGridCell *sourceCell;
  ArmyRuntimeSlot *createdModelNode;
  ArmyRuntimeSlot *createdArmySlot;
  ModelRuntimeSlot *createdModelRuntime;
  ArmyRuntimeSlot **armyRuntime;
  Q12 anchorXQ12;
  Q12 anchorYQ12;

  sourceCell = siteEntry->cell;
  if (AiPlacement_ReserveAdditionalSpecialSite(siteEntry->armyAssetId,sourceCell,factionIndex,worldRuntime)) {
    return false;
  }
  if (!AiPlacement_FindNearestPlaceableBaseSite
         (sourceCell->worldY,sourceCell->worldX,armyAssetId,factionIndex,worldRuntime,&anchorYQ12,&anchorXQ12)) {
    return false;
  }
  armyRuntime = (ArmyRuntimeSlot **)ArmyRuntime_CreateInstanceFromAsset
                    (ARMY_CREATE_UNLOCK_TECHNOLOGY,0,anchorYQ12,anchorXQ12,factionIndex,armyAssetId,worldRuntime,
                     NULL);
  if (armyRuntime == NULL) {
    return true;
  }
  createdModelNode = armyRuntime[1];
  createdArmySlot = *armyRuntime;
  createdModelNode->movementPosition0Q12 = 0;
  createdModelRuntime = (createdArmySlot->modelRuntimeOrSavedOffset).modelRuntime;
  ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)createdModelNode);
  ArmyRuntime_DispatchClassCommand((ArmyRuntimeSlot *)armyRuntime,worldRuntime); /* the created army */
  EffectRuntimePool_CreateInstanceFromDefinition
            (EFFECT_RUNTIME_COMPLETION_NONE,(EffectRuntimeOwnerReference){NULL},
             ((ModelRuntimeNode *)createdModelNode)->modelPayload.worldRotationAngle2,
             ((ModelRuntimeNode *)createdModelNode)->modelPayload.worldRotationAngle1,
             ((ModelRuntimeNode *)createdModelNode)->modelPayload.worldRotationAngle0,
             ((ModelRuntimeNode *)createdModelNode)->worldTransform.translation.z,
             ((ModelRuntimeNode *)createdModelNode)->worldTransform.translation.y,
             ((ModelRuntimeNode *)createdModelNode)->worldTransform.translation.x,
             (EffectDefinition *)createdModelRuntime->attachments[2].childLocalRotationAngle0,
             worldRuntime);
  AiConstructionPlanner_ConsumeFactionPendingArmyAsset(armyAssetId,factionIndex);
  return true;
}

/* Runs AiConstructionPlanner_TryPlaceStorageAtResourceSite for every workspace-08 site of siteAssetId in order;
   returns true as soon as one attempt is over. */
static bool AiConstructionPlanner_TryPlaceStorageAtResourceSitesOf
          (PckArmyAssetIdCatalog siteAssetId,PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime)
{
  AiTerrainFeatureWorkspaceEntry *siteEntry;
  int remainingEntries;

  siteEntry = g_AiWorkspace08TerrainFeatureSites;
  for (remainingEntries = g_AiWorkspace08Count; remainingEntries != 0; remainingEntries--, siteEntry++) {
    if ((siteEntry->armyAssetId == siteAssetId) &&
        AiConstructionPlanner_TryPlaceStorageAtResourceSite(siteEntry,armyAssetId,factionIndex,worldRuntime)) {
      return true;
    }
  }
  return false;
}

/* Builds a pending ARM_0333 (the caller's only asset here, 0x14D) next to a resource site: first for each
   ARM_0330 site of workspace 08, then for each ARM_0332 site, where AiPlacement_ReserveAdditionalSpecialSite
   accepts the site it creates the structure at the nearest valid workspace-09 cell, initialises it like the other
   planners, starts its effect and removes it from the pending list (a failed creation ends the attempt). Without
   such a site it falls back to AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate.
*/
void AiConstructionPlanner_PlaceTritiumStorageNearResourceSite(PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime)

{
  if (AiConstructionPlanner_TryPlaceStorageAtResourceSitesOf
        (ARM_0330_BUILDING_MDL0303,armyAssetId,factionIndex,worldRuntime)) {
    return;
  }
  if (AiConstructionPlanner_TryPlaceStorageAtResourceSitesOf
        (ARM_0332_BUILDING_MDL0302,armyAssetId,factionIndex,worldRuntime)) {
    return;
  }
  AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate(armyAssetId,factionIndex,worldRuntime);
  return;
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
    if ((((ArmyAssetRecord *)*armyAssetRegistryCursor)->flags & 1) != 0) {
      candidateScore = AiArmyCandidate_ComputeFactionWeightedScore
                         (&g_AiArmyCandidateScoreWeightsVariantA15,factionIndex,
                          (ArmyAssetRecord *)*armyAssetRegistryCursor);
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


/* Once the faction has an ARM 330 (0x14A) structure: while Arms Factories is still locked it proposes building
   ARM 301 (0x12D) if it has none and the capacity allows; after the research it proposes the best class of the
   ARM 302..306 (0x12E..0x132) family, its weight divided by twice the class's existing count (if any).
*/
void AiStrategicClass_AddArmsFactoriesStageBuilding(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  uint32_t weightRange;
  AiStrategicClassSelection classSelection;
  AiKnowledgeDataImage *knowledgeData;

  knowledgeData = g_AiKnowledgeData;
  if (!AiPrimaryWorkspace_HasEntryById(ARM_0330_BUILDING_MDL0303)) {
    return;
  }
  /* Technology_IsUnlockedForFaction returns true while the technology is still locked */
  if (Technology_IsUnlockedForFaction(TEC_001_ARMS_FACTORIES,factionIndex)) {
    if (!AiPrimaryWorkspace_HasEntryById(ARM_0301_BUILDING_MDL0318) &&
        !AiFactionRuntime_TestPlanningCapacityExceeded
           (knowledgeData->parameters.strategic12dAnd141To143AdditionalPlanningCapacity,factionIndex)) {
      AiCandidateWorkspace_AddOrAccumulateWeightedEntry
                (ARM_0301_BUILDING_MDL0318,knowledgeData->parameters.strategicClass12dBaseWeight,1);
    }
    return;
  }
  if (AiFactionRuntime_TestPlanningCapacityExceeded
        (knowledgeData->parameters.strategic12fTo132AdditionalPlanningCapacity,factionIndex)) {
    return;
  }
  classSelection = AiStrategicClass_SelectTerrainSuitedBuilding(factionIndex,worldRuntime);
  if (classSelection.selectedRuntimeToken != 0) {
    weightRange = knowledgeData->parameters.strategicClass12fTo132BaseWeight;
    if (classSelection.existingCountOrPressure != 0) {
      weightRange = weightRange / (classSelection.existingCountOrPressure * 2);
    }
    AiCandidateWorkspace_AddOrAccumulateWeightedEntry(classSelection.selectedRuntimeToken,weightRange,1);
  }
  return;
}


/* Once the faction has an ARM 330 (0x14A) structure and the planning capacity allows it, proposes the class
   chosen from the ARM 321..323 (0x141..0x143) family, its weight divided by twice the class's existing count
   (if any).
*/
void AiStrategicClass_AddWeightedClassCandidate(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  AiKnowledgeDataImage *knowledgeData;
  uint32_t weightRange;
  AiStrategicClassSelection classSelection;

  knowledgeData = g_AiKnowledgeData;
  if (!AiPrimaryWorkspace_HasEntryById(ARM_0330_BUILDING_MDL0303)) {
    return;
  }
  if (AiFactionRuntime_TestPlanningCapacityExceeded
        (knowledgeData->parameters.strategic12dAnd141To143AdditionalPlanningCapacity,factionIndex)) {
    return;
  }
  classSelection = AiStrategicClass_SelectPressureWeightedBuilding(factionIndex,worldRuntime);
  if (classSelection.selectedRuntimeToken != 0) {
    weightRange = knowledgeData->parameters.strategicClass141To143BaseWeight;
    if (classSelection.existingCountOrPressure != 0) {
      weightRange = weightRange / (classSelection.existingCountOrPressure * 2);
    }
    AiCandidateWorkspace_AddOrAccumulateWeightedEntry(classSelection.selectedRuntimeToken,weightRange,1);
  }
  return;
}


/* Creates armyAssetId for the faction at the chosen field cell (technology unlocked), clears the new model
   node's movement position, rebuilds its transforms, dispatches the army's class command, spawns the effect named
   by the created model's attachment 2 at the node and removes the asset from the faction's pending list. Shared tail of the
   placement planners; nothing happens when the creation fails. */
static void AiConstructionPlanner_CreatePlacedAsset
          (FieldGridCell *cell,PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime)
{
  ArmyRuntimeSlot **createdSlots;
  ArmyRuntimeSlot *modelNodeRuntime;
  ArmyRuntimeSlot *createdArmySlot;
  ModelRuntimeSlot *createdModelRuntime;

  createdSlots = (ArmyRuntimeSlot **)ArmyRuntime_CreateInstanceFromAsset
                    (ARMY_CREATE_UNLOCK_TECHNOLOGY,(uint32_t)(uint16_t)cell->triangle0NormalAngles,
                     cell->worldY,cell->worldX,factionIndex,armyAssetId,worldRuntime,NULL);
  if (createdSlots == NULL) {
    return;
  }
  modelNodeRuntime = createdSlots[1];
  createdArmySlot = *createdSlots;
  modelNodeRuntime->movementPosition0Q12 = 0;
  createdModelRuntime = (createdArmySlot->modelRuntimeOrSavedOffset).modelRuntime;
  ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)modelNodeRuntime);
  ArmyRuntime_DispatchClassCommand((ArmyRuntimeSlot *)createdSlots,worldRuntime); /* the created army */
  EffectRuntimePool_CreateInstanceFromDefinition
            (EFFECT_RUNTIME_COMPLETION_NONE,(EffectRuntimeOwnerReference){NULL},
             ((ModelRuntimeNode *)modelNodeRuntime)->modelPayload.worldRotationAngle2,
             ((ModelRuntimeNode *)modelNodeRuntime)->modelPayload.worldRotationAngle1,
             ((ModelRuntimeNode *)modelNodeRuntime)->modelPayload.worldRotationAngle0,
             ((ModelRuntimeNode *)modelNodeRuntime)->worldTransform.translation.z,
             ((ModelRuntimeNode *)modelNodeRuntime)->worldTransform.translation.y,
             ((ModelRuntimeNode *)modelNodeRuntime)->worldTransform.translation.x,
             (EffectDefinition *)createdModelRuntime->attachments[2].childLocalRotationAngle0,
             worldRuntime);
  AiConstructionPlanner_ConsumeFactionPendingArmyAsset(armyAssetId,factionIndex);
}

/* Default placement of a pending building (every asset the request dispatcher does not handle specially), only
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
  int anchorDistanceY;
  int structureDistance;
  int siteDistance;
  int candidateScore;
  int remainingCells;
  int anchorDistanceX;
  FieldGridCell **gridCellCursor;
  FieldGridCell *bestCell;
  int bestScore;
  FieldGridCell *candidateCell;
  bool siteDistanceInRange;

  if ((g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorCooldown == 0) ||
     (g_AiWorkspace09Count == 0)) {
    return;
  }
  bestScore = INT32_MAX;
  gridCellCursor = g_AiWorkspace09Cells;
  for (remainingCells = g_AiWorkspace09Count; remainingCells != 0; remainingCells--, gridCellCursor++) {
    candidateCell = *gridCellCursor;
    anchorDistanceY = g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorYQ12 - candidateCell->worldX;
    if (anchorDistanceY < 0) {
      anchorDistanceY = -anchorDistanceY;
    }
    anchorDistanceX = g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorXQ12 - candidateCell->worldY;
    if (anchorDistanceX < 0) {
      anchorDistanceX = -anchorDistanceX;
    }
    if (anchorDistanceX + anchorDistanceY >= bestScore) continue;
    structureDistance = AiPrimaryWorkspace_GetMinimumActiveManhattanDistanceToPoint
                          (candidateCell->worldY,candidateCell->worldX);
    if (structureDistance <= 2 * Q12_ONE - 1) continue;
    /* Distance to the nearest workspace-02 site (x4, at most 5.0), or without any such site to the
       nearest workspace-03 site (x2, at most 8.0). */
    siteDistance = AiHostileWorkspace_GetNearestVisibleHostileDistance(candidateCell->worldY,candidateCell->worldX);
    if (siteDistance < INT32_MAX) {
      siteDistanceInRange = siteDistance < 5 * Q12_ONE + 1;
      if (siteDistanceInRange) {
        siteDistance = siteDistance * 4;
      }
    }
    else {
      siteDistance = AiHostileWorkspace_GetNearestUnseenHostileDistance(candidateCell->worldY,candidateCell->worldX);
      siteDistanceInRange = siteDistance < 8 * Q12_ONE + 1;
      if (siteDistanceInRange) {
        siteDistance = siteDistance * 2;
      }
    }
    if (!siteDistanceInRange) continue;
    candidateScore = anchorDistanceX + anchorDistanceY + siteDistance;
    if ((candidateScore < bestScore) &&
       (ArmyPlacement_CanPlaceAssetAtFieldPoint
                          (1,0,(uint32_t)(uint16_t)candidateCell->triangle0NormalAngles,
                           candidateCell->worldY,candidateCell->worldX,armyAssetId,factionIndex,
                           (UiRootNode *)worldRuntime,NULL))) {
      bestCell = candidateCell;
      bestScore = candidateScore;
    }
  }
  if (bestScore < INT32_MAX) {
    AiConstructionPlanner_CreatePlacedAsset(bestCell,armyAssetId,factionIndex,worldRuntime);
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


/* True when technologyIndex is one of the research slots researchTechnologyIds[1..28] of the structure
   definition (slot 0 is not a research slot). */
static bool AiPurchaseCandidate_DefinitionListsResearch(ModelDefinition *definition,RuntimeToken technologyIndex)
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
bool AiPurchaseCandidate_HasEligibleProducer(AiCandidateWorkspaceEntry *candidateEntry,FactionRuntimeIndex factionIndex)

{
  uint32_t producerClassMask;
  int *entitySlot;
  uint32_t remainingEntries;
  RuntimeToken candidateId;
  AiStructureWorkspaceEntry *workspaceEntry;
  ArmyAssetRecordPrefix *armyAsset;

  candidateId = candidateEntry->entityIdAndMultiplicity & AI_CANDIDATE_ID_MASK;
  workspaceEntry = g_AiWorkspace00Structures;
  /* entitySlot[0] = definition, [43] = classState.classStateAC, [46] = classState.behaviorState,
     [59] = runtimeFlags (classState.stateFlags) */
  if ((candidateEntry->weightedScoreAndKind & AI_CANDIDATE_KIND_MASK) == AI_CANDIDATE_KIND_TECHNOLOGY) {
    if (!Technology_IsAvailableForFaction(candidateId,factionIndex)) {
      return true;
    }
    for (remainingEntries = g_AiWorkspace00Count; remainingEntries != 0; remainingEntries--, workspaceEntry++) {
      entitySlot = (int *)workspaceEntry->runtimeSlotAddressOrZero;
      if ((entitySlot != NULL) &&
          ((entitySlot[59] & (ARMY_MODEL_STATE_INACTIVE_MASK | ARMY_MODEL_STATE_RESEARCH_UNPAID)) == 0) &&
          AiPurchaseCandidate_DefinitionListsResearch((ModelDefinition *)*entitySlot,candidateId)) {
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
      entitySlot = (int *)workspaceEntry->runtimeSlotAddressOrZero;
      if ((entitySlot != NULL) && (((ModelDefinition *)*entitySlot)->runtimeClassId == MODEL_RUNTIME_CLASS_11) &&
          ((entitySlot[59] & ARMY_MODEL_STATE_BUILD_BLOCKING_MASK) == 0) && (entitySlot[46] == 0)) {
        return false;
      }
    }
  }
  else if ((producerClassMask & ARMY_ASSET_FLAG_BUILT_AT_AIRCRAFT_PAD) != 0) {
    for (remainingEntries = g_AiWorkspace00Count; remainingEntries != 0; remainingEntries--, workspaceEntry++) {
      entitySlot = (int *)workspaceEntry->runtimeSlotAddressOrZero;
      if ((entitySlot != NULL) && (((ModelDefinition *)*entitySlot)->runtimeClassId == MODEL_RUNTIME_CLASS_22) &&
          ((entitySlot[59] & ARMY_MODEL_STATE_BUILD_BLOCKING_MASK) == 0) && (entitySlot[43] == 0)) {
        return false;
      }
    }
  }
  else if ((producerClassMask & ARMY_ASSET_FLAGS_BUILT_BY_FACTORY) != 0) {
    for (remainingEntries = g_AiWorkspace00Count; remainingEntries != 0; remainingEntries--, workspaceEntry++) {
      entitySlot = (int *)workspaceEntry->runtimeSlotAddressOrZero;
      if ((entitySlot != NULL) && (((ModelDefinition *)*entitySlot)->runtimeClassId == MODEL_RUNTIME_CLASS_13) &&
          ((entitySlot[59] & ARMY_MODEL_STATE_BUILD_BLOCKING_MASK) == 0) &&
          ((((ModelDefinition *)*entitySlot)->classParameterC4 & producerClassMask &
            ARMY_ASSET_FLAGS_BUILT_BY_FACTORY) != 0) &&
          (entitySlot[46] == 0)) {
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
      entity = (GameEntityRuntime *)workspaceEntry->runtimeSlotAddressOrZero;
      if ((entity != NULL) &&
          (((entity->common).runtimeFlags & (ARMY_MODEL_STATE_INACTIVE_MASK | ARMY_MODEL_STATE_RESEARCH_UNPAID)) == 0) &&
          AiPurchaseCandidate_DefinitionListsResearch
            ((ModelDefinition *)(entity->common).ownership.definitionOrClassRecord,candidateId)) {
        Technology_ApplyRecordToEntity(candidateId,entity);
        workspaceEntry->runtimeSlotAddressOrZero = 0;
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
  return;
}


/* Sets bit 0 of the faction's runtimeFlags when the faction is active enough: one assigned workspace 00
   structure below ARM 340, two assigned ARM 340..379 structures, or three qualifying entries counting one such
   structure plus the armies of workspace 01 (attachmentCount >= 1 with attachments[0] set, or >= 2 with
   attachments[1] set).
   When nothing triggers but the flag is already set, re-applies the model flags of every workspace 00/01
   entity in the world runtime.
*/
void AiFactionPlanning_UpdateActiveEntityPressureFlag(FactionRuntimeIndex factionIndex)

{
  FactionRuntimeFlags *factionRuntimeFlags;
  int remainingEntries;
  int entriesStillNeeded;
  WorldRuntimeContext *contextArg;
  AiStructureWorkspaceEntry *primaryEntry;
  AiRuntimeWorkspaceEntry *runtimeWorkspaceEntry;
  ModelRuntimeSlot *unitModelRuntime;

  factionRuntimeFlags = &g_GameFactionRuntimeImage.records[factionIndex].runtimeFlags;
  /* Two ARM 340..379 structures are needed; each one found counts down. */
  entriesStillNeeded = 2;
  primaryEntry = g_AiWorkspace00Structures;
  for (remainingEntries = g_AiWorkspace00Count; remainingEntries != 0; remainingEntries--) {
    if (primaryEntry->runtimeSlotAddressOrZero != 0) {
      if (primaryEntry->armyAssetId < ARM_0340_BUILDING_MDL0314) {
        *factionRuntimeFlags = *factionRuntimeFlags | 1;
        return;
      }
      if (primaryEntry->armyAssetId < ARM_0380_BUILDING_MDL0329) {
        entriesStillNeeded--;
        if (entriesStillNeeded == 0) {
          *factionRuntimeFlags = *factionRuntimeFlags | 1;
          return;
        }
      }
    }
    primaryEntry++;
  }
  /* With workspace-01 armies the threshold is one higher (three entries counting one such structure). */
  entriesStillNeeded++;
  runtimeWorkspaceEntry = g_AiWorkspace01Units;
  for (remainingEntries = g_AiWorkspace01Count; remainingEntries != 0; remainingEntries--) {
    unitModelRuntime = runtimeWorkspaceEntry->modelRuntime;
    /* a unit with a child model in attachment 0 or 1 (e.g. a weapon) */
    if (((unitModelRuntime != NULL) && (unitModelRuntime->attachmentCount != 0)) &&
        ((unitModelRuntime->attachments[0].childModelRuntimeOrSavedOffset != NULL) ||
         ((1 < unitModelRuntime->attachmentCount) &&
          (unitModelRuntime->attachments[1].childModelRuntimeOrSavedOffset != NULL)))) {
      entriesStillNeeded--;
      if (entriesStillNeeded == 0) {
        *factionRuntimeFlags = *factionRuntimeFlags | 1;
        return;
      }
    }
    runtimeWorkspaceEntry++;
  }
  if ((*factionRuntimeFlags & 1) == 0) {
    return;
  }
  contextArg = &g_InGameRuntimeRoot->worldRuntime;
  primaryEntry = g_AiWorkspace00Structures;
  for (remainingEntries = g_AiWorkspace00Count; remainingEntries != 0; remainingEntries--) {
    if (primaryEntry->runtimeSlotAddressOrZero != 0) {
      ModelRuntimeHierarchy_MarkDestroyedRecursive
                (contextArg,
                 ((ModelRuntimeSlot *)primaryEntry->runtimeSlotAddressOrZero)->ownerArmyRuntimeOrSavedOffset.
                 armyRuntime);
    }
    primaryEntry++;
  }
  runtimeWorkspaceEntry = g_AiWorkspace01Units;
  for (remainingEntries = g_AiWorkspace01Count; remainingEntries != 0; remainingEntries--) {
    if (runtimeWorkspaceEntry->modelRuntime != NULL) {
      ModelRuntimeHierarchy_MarkDestroyedRecursive
                (contextArg,runtimeWorkspaceEntry->modelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime);
    }
    runtimeWorkspaceEntry++;
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
  bool hasPrerequisite;
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
  return;
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


/* Picks which of the buildings ARM_0302..ARM_0306 (0x12E..0x132) to propose next. It measures the share (%) of
   scratch-grid cells free of terrain bit 24, of bits 25-27 and of bits 28-30; every building the faction lacks
   (not in workspace 00) but may build (enabled) is scored with its row of g_AiStrategicClassTerrainWeights plus
   14 random bits (0..0x3FFF), and the best one is returned as selectedRuntimeToken (0 = none), with the number of
   these five buildings already present in existingCountOrPressure.
*/
AiStrategicClassSelection AiStrategicClass_SelectTerrainSuitedBuilding
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  GridScratchStateMask cellStateMask;
  int freeBits25To27Percent;
  int freeBit24Percent;
  int freeBits28To30Percent;
  uint32_t cellCount;
  uint32_t remainingCells;
  uint32_t tieBreakBits;
  uint32_t freeBits25To27Cells;
  int candidateScore;
  uint32_t freeBit24Cells;
  GridScratchCell *scratchCell;
  uint32_t freeBits28To30Cells;
  AiStrategicClassSelection selection;
  uint32_t existingClassCount;
  uint32_t randomizedTieBits;
  RuntimeToken selectedToken;
  int bestCandidateScore;

  cellCount = g_GridScratchWidth * g_GridScratchHeight;
  freeBit24Cells = 0;
  freeBits25To27Cells = 0;
  freeBits28To30Cells = 0;
  remainingCells = cellCount;
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
    remainingCells--;
  } while (remainingCells != 0);
  freeBits25To27Percent = (int)(((uint64_t)freeBits25To27Cells * 100) / (uint64_t)cellCount);
  freeBit24Percent = (int)(((uint64_t)freeBit24Cells * 100) / (uint64_t)cellCount);
  freeBits28To30Percent = (int)(((uint64_t)freeBits28To30Cells * 100) / (uint64_t)cellCount);
  randomizedTieBits = g_RandomGeneratorState.next();
  bestCandidateScore = 0;
  selectedToken = 0;
  existingClassCount = 5;
  if (!AiPrimaryWorkspace_HasEntryById(ARM_0302_BUILDING_MDL0300)) {
    existingClassCount = 4;
    if (!ArmyAssetRegistry_FindEnabledById(ARM_0302_BUILDING_MDL0300)) {
      selectedToken = ARM_0302_BUILDING_MDL0300;
      /* the original rotates right by 5; a shift gives the same low 14 bits for the at most three steps used */
      tieBreakBits = randomizedTieBits & AI_STRATEGIC_TIE_BREAK_MASK;
      randomizedTieBits = randomizedTieBits >> 5;
      bestCandidateScore =
           freeBits28To30Percent * g_AiStrategicClassTerrainWeights[0][0] +
           freeBits25To27Percent * g_AiStrategicClassTerrainWeights[0][1] +
           freeBit24Percent * g_AiStrategicClassTerrainWeights[0][2] + tieBreakBits;
    }
  }
  if (!AiPrimaryWorkspace_HasEntryById(ARM_0303_BUILDING_MDL0316)) {
    existingClassCount--;
    if (!ArmyAssetRegistry_FindEnabledById(ARM_0303_BUILDING_MDL0316)) {
      tieBreakBits = randomizedTieBits & AI_STRATEGIC_TIE_BREAK_MASK;
      randomizedTieBits = randomizedTieBits >> 5;
      candidateScore = freeBits28To30Percent * g_AiStrategicClassTerrainWeights[1][0] +
              freeBits25To27Percent * g_AiStrategicClassTerrainWeights[1][1] +
              freeBit24Percent * g_AiStrategicClassTerrainWeights[1][2] + tieBreakBits;
      if (bestCandidateScore < candidateScore) {
        selectedToken = ARM_0303_BUILDING_MDL0316;
        bestCandidateScore = candidateScore;
      }
    }
  }
  if (!AiPrimaryWorkspace_HasEntryById(ARM_0304_BUILDING_MDL0324)) {
    existingClassCount--;
    if (!ArmyAssetRegistry_FindEnabledById(ARM_0304_BUILDING_MDL0324)) {
      tieBreakBits = randomizedTieBits & AI_STRATEGIC_TIE_BREAK_MASK;
      randomizedTieBits = randomizedTieBits >> 5;
      candidateScore = freeBits28To30Percent * g_AiStrategicClassTerrainWeights[2][0] +
              freeBits25To27Percent * g_AiStrategicClassTerrainWeights[2][1] +
              freeBit24Percent * g_AiStrategicClassTerrainWeights[2][2] + tieBreakBits;
      if (bestCandidateScore < candidateScore) {
        selectedToken = ARM_0304_BUILDING_MDL0324;
        bestCandidateScore = candidateScore;
      }
    }
  }
  if (!AiPrimaryWorkspace_HasEntryById(ARM_0305_BUILDING_MDL0317)) {
    existingClassCount--;
    if (!ArmyAssetRegistry_FindEnabledById(ARM_0305_BUILDING_MDL0317)) {
      candidateScore = freeBits28To30Percent * g_AiStrategicClassTerrainWeights[3][0] +
              freeBits25To27Percent * g_AiStrategicClassTerrainWeights[3][1] +
              freeBit24Percent * g_AiStrategicClassTerrainWeights[3][2] + (randomizedTieBits & AI_STRATEGIC_TIE_BREAK_MASK);
      if (bestCandidateScore < candidateScore) {
        selectedToken = ARM_0305_BUILDING_MDL0317;
        bestCandidateScore = candidateScore;
      }
    }
  }
  if (!AiPrimaryWorkspace_HasEntryById(ARM_0306_BUILDING_MDL0310)) {
    existingClassCount--;
    if (!ArmyAssetRegistry_FindEnabledById(ARM_0306_BUILDING_MDL0310)) {
      /* the original also scores ARM_0306 with g_AiStrategicClassTerrainWeights[4] but
         discards the result: ARM_0306 wins only over a negative best score */
      if (bestCandidateScore < 0) {
        selectedToken = ARM_0306_BUILDING_MDL0310;
      }
    }
  }
  selection.selectedRuntimeToken = selectedToken;
  selection.existingCountOrPressure = existingClassCount;
  return selection;
}


/* Pressure score of one strategic building class (without the random jitter):
   sum((pressure + 1) * coefficient) / pressureSumPlusOne over the faction's AI pressure values 2..4. */
static uint32_t AiStrategicClass_ScorePressureCoefficients
          (int pressure2,int pressure3,int pressure4,uint32_t pressure2Coefficient,uint32_t pressure3Coefficient,
          uint32_t pressure4Coefficient,uint32_t pressureSumPlusOne)
{
  return ((pressure2 + 1) * pressure2Coefficient + (pressure3 + 1) * pressure3Coefficient +
          (pressure4 + 1) * pressure4Coefficient) / pressureSumPlusOne;
}

/* Picks which of the buildings ARM_0321..ARM_0323 (0x141..0x143) to propose next. Each is scored from the
   faction's AI pressure values 2..4 as sum((pressure + 1) * coefficient) / (pressure2 + pressure3 + pressure4 + 1)
   with its own three ki.dat coefficients, plus 7 random bits; only buildings the faction lacks (not in workspace
   00) but may build (enabled) compete. Returns the winner in selectedRuntimeToken (0 = none) and the number of
   these three buildings already present in existingCountOrPressure.
*/
AiStrategicClassSelection AiStrategicClass_SelectPressureWeightedBuilding
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  int pressure2;
  int pressure3;
  int pressure4;
  uint32_t randomBits;
  RuntimeToken selectedToken;
  uint32_t pressureSumPlusOne;
  uint32_t existingCount;
  uint32_t class141Score;
  uint32_t class142Score;
  uint32_t class143Score;
  uint32_t bestScore;
  AiStrategicClassSelection selection;

  pressure2 = g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[2];
  pressure3 = g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[3];
  pressure4 = g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[4];
  pressureSumPlusOne = g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[2] +
           g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[3] +
           g_GameFactionRuntimeImage.records[factionIndex].aiPressureValues[4] + 1;
  randomBits = g_RandomGeneratorState.next();
  class141Score = AiStrategicClass_ScorePressureCoefficients
                    (pressure2,pressure3,pressure4,
                     (g_AiKnowledgeData->parameters).strategicClass141Pressure2Coefficient,
                     (g_AiKnowledgeData->parameters).strategicClass141Pressure3Coefficient,
                     (g_AiKnowledgeData->parameters).strategicClass141Pressure4Coefficient,pressureSumPlusOne) +
                  (randomBits & AI_STRATEGIC_SCORE_JITTER_MASK);
  class142Score = AiStrategicClass_ScorePressureCoefficients
                    (pressure2,pressure3,pressure4,
                     (g_AiKnowledgeData->parameters).strategicClass142Pressure2Coefficient,
                     (g_AiKnowledgeData->parameters).strategicClass142Pressure3Coefficient,
                     (g_AiKnowledgeData->parameters).strategicClass142Pressure4Coefficient,pressureSumPlusOne) +
                  (randomBits >> 19 & AI_STRATEGIC_SCORE_JITTER_MASK);
  class143Score = AiStrategicClass_ScorePressureCoefficients
                    (pressure2,pressure3,pressure4,
                     (g_AiKnowledgeData->parameters).strategicClass143Pressure2Coefficient,
                     (g_AiKnowledgeData->parameters).strategicClass143Pressure3Coefficient,
                     (g_AiKnowledgeData->parameters).strategicClass143Pressure4Coefficient,pressureSumPlusOne) +
                  (randomBits >> 7 & AI_STRATEGIC_SCORE_JITTER_MASK);
  bestScore = 0;
  /* counts down for every one of the three buildings the faction lacks */
  existingCount = 3;
  selectedToken = 0;
  /* ArmyAssetRegistry_FindEnabledById returns false when the asset is enabled */
  if (!AiPrimaryWorkspace_HasEntryById(ARM_0321_BUILDING_MDL0326)) {
    existingCount--;
    if (!ArmyAssetRegistry_FindEnabledById(ARM_0321_BUILDING_MDL0326) && (class141Score != 0)) {
      selectedToken = ARM_0321_BUILDING_MDL0326;
      bestScore = class141Score;
    }
  }
  if (!AiPrimaryWorkspace_HasEntryById(ARM_0322_BUILDING_MDL0327)) {
    existingCount--;
    if (!ArmyAssetRegistry_FindEnabledById(ARM_0322_BUILDING_MDL0327) && (bestScore < class142Score)) {
      selectedToken = ARM_0322_BUILDING_MDL0327;
      bestScore = class142Score;
    }
  }
  if (!AiPrimaryWorkspace_HasEntryById(ARM_0323_BUILDING_MDL0328)) {
    existingCount--;
    if (!ArmyAssetRegistry_FindEnabledById(ARM_0323_BUILDING_MDL0328) && (bestScore < class143Score)) {
      selectedToken = ARM_0323_BUILDING_MDL0328;
    }
  }
  selection.selectedRuntimeToken = selectedToken;
  selection.existingCountOrPressure = existingCount;
  return selection;
}


/* Places a pending asset at a workspace-10 cell, only while the faction's primary anchor cooldown is 0. Each cell
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
  int distanceX;
  int candidateScore;
  uint32_t randomBits;
  int remainingCells;
  int distanceY;
  FieldGridCell **gridCellCursor;
  bool regionUnreachable;
  ArmyAssetRecordPrefix *armyAsset;
  ModelDefinitionRecordPrefix *modelDefinition;
  FieldGridCell *bestCell;
  int bestScore;

  if ((ArmyAssetRegistry_FindById(armyAssetId,&armyAsset) != 0) ||
     (g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorCooldown != 0)) {
    return;
  }
  modelDefinition = ModelDefinitionRegistry_FindById
                     (((AiLinkedDefinitionListView *)armyAsset->rootNodeOffsetOrPointer)->definitionIds[0]);
  if (modelDefinition == NULL) {
    return;
  }
  radiusMetric = ((ModelDefinition *)modelDefinition)->footprintRadius;
  if (g_AiWorkspace10Count == 0) {
    return;
  }
  bestScore = INT32_MAX;
  gridCellCursor = g_AiWorkspace10Cells;
  for (remainingCells = g_AiWorkspace10Count; remainingCells != 0; remainingCells--, gridCellCursor++) {
    candidateCell = *gridCellCursor;
    distanceX = candidateCell->worldX;
    distanceY = candidateCell->worldY;
    if (g_AiWorkspaceOwnedAsset300Runtime != NULL) {
      distanceX = distanceX -
                  (g_AiWorkspaceOwnedAsset300Runtime->rootModelNodeOrSavedOffset.modelNode->worldTransform).translation.x;
      if (distanceX < 0) {
        distanceX = -distanceX;
      }
      distanceY = distanceY -
                  (g_AiWorkspaceOwnedAsset300Runtime->rootModelNodeOrSavedOffset.modelNode->worldTransform).translation.y;
      if (distanceY < 0) {
        distanceY = -distanceY;
      }
    }
    randomBits = g_RandomGeneratorState.next();
    candidateScore = distanceY + distanceX + (randomBits & 0xffff);
    if ((candidateScore < bestScore) &&
       (ArmyPlacement_CanPlaceAssetAtFieldPoint
                          (1,0,(uint32_t)(uint16_t)candidateCell->triangle0NormalAngles,candidateCell->worldY,
                           candidateCell->worldX,armyAssetId,factionIndex,(UiRootNode *)worldRuntime,
                           NULL))) {
      regionUnreachable = GridReachability_RebuildConnectedRegionAroundWorldPoint
                            (radiusMetric,candidateCell->worldY,candidateCell->worldX);
      if (!regionUnreachable) {
        bestCell = candidateCell;
        bestScore = candidateScore;
      }
    }
  }
  if (bestScore < INT32_MAX) {
    AiConstructionPlanner_CreatePlacedAsset(bestCell,armyAssetId,factionIndex,worldRuntime);
  }
}


/* Called after the AI has placed an army asset: counts the placement (global counter and the faction's
   relationCounterB) and removes the first entry for that asset's registry record from the faction's pending
   primary army asset list, shifting the rest down. The registry lookup's result is not checked; an unknown id simply
   matches no entry.
*/
void AiConstructionPlanner_ConsumeFactionPendingArmyAsset
          (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex)

{
  FactionRelationCounter *relationCounter;
  FactionArmyAssetCount *pendingAssetCount;
  FactionArmyAssetCount remainingAssets;
  uint32_t *assetPointerCursor;
  ArmyAssetRecordPrefix *armyAsset;
  
  g_AiConstructionPendingAssetConsumedCount++;
  remainingAssets = g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
  /* Original quirk: the lookup status is not checked (an unknown id leaves the error code in armyAsset,
     which then matches no list entry) */
  ArmyAssetRegistry_FindById(armyAssetId,&armyAsset);
  assetPointerCursor = g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetPointersOrIds;
  relationCounter = &g_GameFactionRuntimeImage.records[factionIndex].relationCounterB;
  (*relationCounter)++;
  for (; remainingAssets != 0; remainingAssets--) {
    if (armyAsset == (ArmyAssetRecordPrefix *)*assetPointerCursor) break;
    assetPointerCursor++;
  }
  if (remainingAssets == 0) {
    return;
  }
  pendingAssetCount = &g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
  (*pendingAssetCount)--;
  /* Shift the entries after the match down by one. */
  for (remainingAssets--; remainingAssets != 0; remainingAssets--) {
    *assetPointerCursor = assetPointerCursor[1];
    assetPointerCursor++;
  }
}


/* Returns true when the faction's energy would not cover its demand plus additionalEnergyDemand
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

  linkedDefinitionList = (AiLinkedDefinitionListView *)armyAssetRecord->rootNodeOffsetOrPointer;
  /* the original tests the selector's status after each of the three selections below, but it is always
     "found" */
  selectedModelDefinition = (ModelDefinitionResolveView *)ModelDefinition_SelectFactionUnlockedLinkedDefinition
                    (factionIndex,(ModelLinkedDefinitionListAddress32)linkedDefinitionList);
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
    selectedChildModelDefinition0 = (ModelDefinitionResolveView *)ModelDefinition_SelectFactionUnlockedLinkedDefinition
                      (factionIndex,linkedDefinitionList->childList0Address);
    if ((selectedChildModelDefinition0->reloadTicks != 0) &&
        (g_GameFactionRuntimeImage.records[factionIndex].maximumAiPressure != 0)) {
      weightedDefinitionScore = weightedDefinitionScore +
           AiArmyCandidate_ScoreWeaponPressureDamage
             (scoreWeights,factionIndex,selectedChildModelDefinition0->shotDefinitionReference,
              selectedChildModelDefinition0->reloadTicks);
    }
    if (1 < childCount) {
      selectedChildModelDefinition1 = (ModelDefinitionResolveView *)ModelDefinition_SelectFactionUnlockedLinkedDefinition
                        (factionIndex,linkedDefinitionList->childList1Address);
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

