/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/ai/planning.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/ai/planning.h>
#include <thandor/thandor.h>

/* Module data. */

uint32_t g_AiActiveGridMaskClasses[4] = {0, 0, 0, 0};

static const int32_t g_AiStrategicClassTerrainWeights[5][3] = {
    {256, 0, 0},
    {240, 16, 0},
    {0, 176, 80},
    {0, 0, 256},
    {0, 0, 0}};

/* Runs the planning phase for every active AI faction (1..7, a faction without a player block) and scales its
   terrain contribution by the game speed, then rebuilds the per-faction AI pressure table: each of the eight
   pressure channels decays to about 3/4, every runtime model adds 0x100 to the channel of its definition
   (targetClassIndex, stock data ~0) for each other faction flagged in its faction mask, and the channel maximum
   is stored.
   Called on tick-wheel cases 2 and 6.
*/
void AiFactionRuntime_RebuildPlanningCapacityState()

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
      /* Original quirk: a do-while, a count of 0 runs it 2^32 times (D8: kept for step 11) */
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
  if (ownerNode == nullptr) {
    return;
  }
  for (; ownerNode != nullptr; ownerNode = ownerNode->nextNode) {
    if (ownerNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) continue;
    /* runtimePayload is the ModelRuntimeSlot: its definition's target class is the pressure channel;
       the owner army holds the owning faction and a faction mask terrainOccupancyMask0 (bit 3 + 2 * (f - 1) for
       faction f). */
    ownerArmy = WorldOwnerNode_ModelRuntime(ownerNode)->ownerArmyRuntimeOrSavedOffset.armyRuntime;
    if (ownerArmy->factionIndex == 0) continue;
    pressureChannel =
         WorldOwnerNode_ModelRuntime(ownerNode)->definitionOrSavedId.runtimeDefinition->targetClassIndex;
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

/* Collects up to four distinct movement masks of the faction's own units (workspace 01) into
   g_AiActiveGridMaskClasses[0..3] (0xFFFFFFFF = unused): GRID_SCRATCH_BLOCKED | distance-band bit (8 +
   footprintRadiusClass) | terrain bit (24 + terrainTraversalClass), both taken from the definition of the unit's
   model runtime; only mobile units (accelerationPerTick != 0) with non-negative classes count. The site scan accepts
   a general site when its scratch neighbourhood avoids every bit of one of these masks. Without any unit
   class 0 falls back to 0x90000100 (band bit 8, terrain bit 28).
*/
void AiPlanning_CollectActiveGridMaskClasses()

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
    if (runtimeWorkspaceEntry->modelRuntime == nullptr) {
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
  Bool8 phaseResult;
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
    if (primaryEntry->runtimeSlotAddressOrZero != nullptr) {
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
    if (((unitModelRuntime != nullptr) && (unitModelRuntime->attachmentCount != 0)) &&
        ((unitModelRuntime->attachments[ARMY_WEAPON_SLOT_PRIMARY].childModelRuntimeOrSavedOffset != nullptr) ||
         ((1 < unitModelRuntime->attachmentCount) &&
          (unitModelRuntime->attachments[ARMY_WEAPON_SLOT_SECONDARY].childModelRuntimeOrSavedOffset != nullptr)))) {
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
    if (primaryEntry->runtimeSlotAddressOrZero != nullptr) {
      ModelRuntimeHierarchy_MarkDestroyedRecursive
                (contextArg,
                 primaryEntry->runtimeSlotAddressOrZero->ownerArmyRuntimeOrSavedOffset.
                 armyRuntime);
    }
    primaryEntry++;
  }
  runtimeWorkspaceEntry = g_AiWorkspace01Units;
  for (remainingEntries = g_AiWorkspace01Count; remainingEntries != 0; remainingEntries--) {
    if (runtimeWorkspaceEntry->modelRuntime != nullptr) {
      ModelRuntimeHierarchy_MarkDestroyedRecursive
                (contextArg,runtimeWorkspaceEntry->modelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime);
    }
    runtimeWorkspaceEntry++;
  }
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
  /* Original quirk: a do-while, a count of 0 runs it 2^32 times (D8: kept for step 11) */
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

/* Returns true when the faction's energy would not cover its demand plus additionalEnergyDemand
   (whole units): the usable supply is the smaller of the generation capacity and baseline supply + tritium
   extraction rate, the demand is supplied + unpowered demand (Q4 values shifted down by 4).
*/
Bool8 AiFactionRuntime_TestPlanningCapacityExceeded(uint32_t additionalEnergyDemand,FactionRuntimeIndex factionIndex)

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
