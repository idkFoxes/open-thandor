/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/faction/economy.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/faction/economy.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

/* uint32_t[24] energy allocation priority per model runtime class (0 = none, up to 0x12); gameplay/session/runtime.c energy distribution */
static const uint32_t g_FactionEnergyAllocationPriorityByModelClass[24] = {
    /*  0 */ 0, 0, 0, 0, 256, 768, 1024, 1280, 1536, 1792, 512, 4608, 0, 2048, 4096, 0,
    /* 16 */ 0, 0, 0, 0, 0, 0, 2048, 256};

/* Energy consumer of the faction economy, collected into the region scratch buffer
   (g_TerrainRegionCollectionEntries) as 16-byte entries, at most 256. */
typedef struct FactionEnergyConsumerEntry {
  uint32_t modelRuntime; /* the consumer's model runtime (pointer value) */
  uint32_t factionIndex;
  EnergyDemandQ4 demandQ4; /* model runtime classState.energyLoadQ4 */
  uint32_t priority; /* g_FactionEnergyAllocationPriorityByModelClass[runtime class] */
} FactionEnergyConsumerEntry;

/* Implementation ownership: gameplay/faction/economy. */

/* Economy step 1, per faction: reset the step's energy demand and extraction rates, decay the faction's row of
   the pair-pressure matrix by 7/8, count the notification/anchor cooldowns down (anchorCooldown1/2 are the
   energy notification cooldowns) and advance the relation transition tick. */
static void InGameFactionEconomy_ResetAndDecayFactionState()
{
  GameFactionRuntimeRecord *factionRecord;
  uint32_t *pairPressureRow;
  int factionIndex;
  int column;

  pairPressureRow = g_GameDataAuxState.pairPressureMatrix8x8;
  for (factionIndex = 0; factionIndex < 8; factionIndex++) {
    factionRecord = &g_GameFactionRuntimeImage.records[factionIndex];
    factionRecord->suppliedEnergyDemandQ4 = 0;
    factionRecord->unpoweredEnergyDemandQ4 = 0;
    factionRecord->xeniteExtractionRateQ4PerTick = 0;
    factionRecord->tritiumExtractionRateQ4PerTick = 0;
    for (column = 0; column < 8; column++) {
      pairPressureRow[column] = pairPressureRow[column] * 7 >> 3;
    }
    if (factionRecord->anchorCooldown1 != 0) {
      factionRecord->anchorCooldown1--;
    }
    if (factionRecord->anchorCooldown2 != 0) {
      factionRecord->anchorCooldown2--;
    }
    if (factionRecord->primaryAnchorCooldown != 0) {
      factionRecord->primaryAnchorCooldown--;
    }
    if (factionRecord->anchorCooldown0 != 0) {
      factionRecord->anchorCooldown0--;
    }
    factionRecord->relationTransitionTick++;
    pairPressureRow = pairPressureRow + 8;
  }
}

/* Pays one collected region (g_TerrainRegionCollectionStoredCount != 0): every entry {extraction descriptor,
   model offset} gives its faction (descriptor bits 13..23) a rate of cells-per-entry * 2 * share (bits 24..31) *
   terrainContributionScaleQ8 >> 15, added to the rate, the stock and the extracted total (Xenite or Tritium
   fields); the extracting model shows its current yield. */
static void InGameFactionEconomy_PayCollectedRegion(Bool8 payTritium)
{
  int cellsPerEntry;
  const uint32_t *entry;
  TerrainRegionCollectionCount remainingEntries;
  uint32_t factionIndex;
  GameFactionRuntimeRecord *factionRecord;
  uint32_t extractionRate;
  uint32_t modelOffset;
  int tickContribution;
  ModelRuntimeSlot *extractingModel;

  cellsPerEntry = (int)g_TerrainRegionCollectionVisitedCount / (int)g_TerrainRegionCollectionStoredCount;
  entry = (const uint32_t *)(uintptr_t)g_TerrainRegionCollectionEntries;
  for (remainingEntries = g_TerrainRegionCollectionStoredCount; remainingEntries != 0; remainingEntries--) {
    factionIndex = entry[0] >> RESOURCE_EXTRACTION_FACTION_SHIFT & RESOURCE_EXTRACTION_FACTION_MASK;
    factionRecord = &g_GameFactionRuntimeImage.records[factionIndex];
    extractionRate = cellsPerEntry * 2 * (entry[0] >> RESOURCE_EXTRACTION_SHARE_SHIFT) *
                     factionRecord->terrainContributionScaleQ8 >> 15;
    modelOffset = entry[1];
    tickContribution = extractionRate * g_InGameSimulationStepTicks;
    if (payTritium) {
      factionRecord->tritiumExtractionRateQ4PerTick = factionRecord->tritiumExtractionRateQ4PerTick + extractionRate;
      factionRecord->tritiumCurrentQ4 = factionRecord->tritiumCurrentQ4 + tickContribution;
      factionRecord->tritiumExtractedTotalQ4 = factionRecord->tritiumExtractedTotalQ4 + tickContribution;
    }
    else {
      factionRecord->xeniteExtractionRateQ4PerTick = factionRecord->xeniteExtractionRateQ4PerTick + extractionRate;
      factionRecord->xeniteCurrentQ4 = factionRecord->xeniteCurrentQ4 + tickContribution;
      factionRecord->xeniteExtractedTotalQ4 = factionRecord->xeniteExtractedTotalQ4 + tickContribution;
    }
    if (modelOffset != 0) {
      extractingModel = (ModelRuntimeSlot *)((int)modelOffset + g_ModelRuntimeRebaseDelta);
      if (extractingModel->rootModelNodeOrSavedOffset.raw != 0) {
        extractingModel->classLinkState.modelLinkOrState.signedScalarState = tickContribution;
      }
    }
    entry = entry + 2;
  }
}

/* One mining pass: clears the connected-region marks of all cells, then collects every not yet visited region
   of cells with requiredCellFlags (Xenite or Tritium support) and pays it out. */
static void InGameFactionEconomy_PayResourceRegions
          (FieldGridAsset *fieldGrid,FieldGridRegionMask requiredCellFlags,Bool8 payTritium)
{
  FieldGridDimension fieldGridWidth;
  int cellCount;
  int cellIndex;
  FieldGridCell *firstCell;
  FieldGridCell *cell;

  fieldGridWidth = fieldGrid->gridWidth;
  cellCount = fieldGridWidth * fieldGrid->gridHeight;
  firstCell = fieldGrid->cells;
  for (cellIndex = 0; cellIndex < cellCount; cellIndex++) {
    firstCell[cellIndex].flagsAndMaterial =
         firstCell[cellIndex].flagsAndMaterial & ~FIELD_CELL_CONNECTED_REGION_VISITED;
  }
  cell = firstCell;
  for (cellIndex = 0; cellIndex < cellCount; cellIndex++) {
    if (((cell->flagsAndMaterial & (FIELD_CELL_GRID_EDGE_MASK | FIELD_CELL_CONNECTED_REGION_VISITED)) == 0) &&
       ((cell->flagsAndMaterial & requiredCellFlags) != 0)) {
      g_TerrainRegionCollectionStoredCount = 0;
      g_TerrainRegionCollectionVisitedCount = 0;
      TerrainRegionCollection_CollectConnectedCellsRecursive(requiredCellFlags,fieldGridWidth << 7,cell);
      if (g_TerrainRegionCollectionStoredCount != 0) {
        InGameFactionEconomy_PayCollectedRegion(payTritium);
      }
    }
    cell++;
  }
}

/* Caps the Xenite and Tritium stocks of all factions at their storage limits. */
static void InGameFactionEconomy_CapStocksAtStorageLimits()
{
  GameFactionRuntimeRecord *factionRecord;
  int factionIndex;

  for (factionIndex = 0; factionIndex < 8; factionIndex++) {
    factionRecord = &g_GameFactionRuntimeImage.records[factionIndex];
    if (factionRecord->xeniteStorageLimitQ4 < factionRecord->xeniteCurrentQ4) {
      factionRecord->xeniteCurrentQ4 = factionRecord->xeniteStorageLimitQ4;
    }
    if (factionRecord->tritiumStorageLimitQ4 < factionRecord->tritiumCurrentQ4) {
      factionRecord->tritiumCurrentQ4 = factionRecord->tritiumStorageLimitQ4;
    }
  }
}

/* Fills one consumer entry from a model runtime (ownerArmyRuntimeOrSavedOffset army slot, classState.energyLoadQ4
   energy demand). */
static void InGameFactionEconomy_FillEnergyConsumer(FactionEnergyConsumerEntry *consumer,int *modelRuntime)
{
  consumer->modelRuntime = (uint32_t)(uintptr_t)modelRuntime;
  consumer->factionIndex = ((ModelRuntimeSlot *)modelRuntime)->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex;
  consumer->demandQ4 = modelRuntime[61];
  consumer->priority =
       g_FactionEnergyAllocationPriorityByModelClass
       [((ModelRuntimeSlot *)modelRuntime)->definitionOrSavedId.runtimeDefinition->runtimeClassId];
}

/* Collects the powered models (energy demand classState.energyLoadQ4 != 0, not dismantling) and, for models
   whose definition has MODEL_DEFINITION_FLAG_COUNT_ATTACHED_ENERGY, their powered attached parts (attachmentCount,
   part runtimes in attachments[], 32-byte slots) into consumers, at most 256. Returns the number collected. */
static uint32_t InGameFactionEconomy_CollectEnergyConsumers(FactionEnergyConsumerEntry *consumers)
{
  uint32_t consumerCount;
  WorldOwnerListNode *worldNode;
  int *modelRuntime;
  int *attachmentSlot;
  int *attachedRuntime;
  int remainingAttachments;

  consumerCount = 0;
  for (worldNode = (g_InGameRuntimeRoot->worldRuntime).ownerListHead;
      worldNode != nullptr; worldNode = worldNode->nextNode) {
    if (worldNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) continue;
    modelRuntime = (int *)worldNode->runtimePayload;
    if ((modelRuntime[59] & ARMY_MODEL_STATE_DISMANTLING) != 0) continue;
    if (modelRuntime[61] != 0) {
      /* buffer full: the attached parts are skipped as well */
      if (255 < consumerCount) continue;
      InGameFactionEconomy_FillEnergyConsumer(&consumers[consumerCount],modelRuntime);
      consumerCount++;
    }
    if ((consumerCount < 256) &&
       ((((ModelRuntimeSlot *)modelRuntime)->definitionOrSavedId.runtimeDefinition->modelFlags &
         MODEL_DEFINITION_FLAG_COUNT_ATTACHED_ENERGY) != 0)) {
      attachmentSlot = modelRuntime;
      for (remainingAttachments = modelRuntime[3]; remainingAttachments != 0; remainingAttachments--) {
        attachedRuntime = Thandor_U32ToPointer<int>(attachmentSlot[80]); /* 5f-format: ModelRuntimeSlot.attachments[].childModelRuntimeOrSavedOffset (dword view) */
        if (((attachedRuntime != nullptr) && (attachedRuntime[61] != 0)) && (consumerCount < 256)) {
          InGameFactionEconomy_FillEnergyConsumer(&consumers[consumerCount],attachedRuntime);
          consumerCount++;
        }
        attachmentSlot = attachmentSlot + 8;
      }
    }
  }
  return consumerCount;
}

/* Selection sort of the consumers by priority, highest first: each position is swapped with every later entry
   of higher priority. */
static void InGameFactionEconomy_SortEnergyConsumersByPriority
          (FactionEnergyConsumerEntry *consumers,uint32_t consumerCount)
{
  uint32_t first;
  uint32_t other;
  FactionEnergyConsumerEntry swapped;

  for (first = 0; first + 1 < consumerCount; first++) {
    for (other = first + 1; other < consumerCount; other++) {
      if (consumers[first].priority < consumers[other].priority) {
        swapped = consumers[first];
        consumers[first] = consumers[other];
        consumers[other] = swapped;
      }
    }
  }
}

/* Energy shortage of the local player's faction: notification 400 (generation capacity too low) or 401 (supply
   too low), each at most every 150 economy runs (anchorCooldown1 / anchorCooldown2). */
static void InGameFactionEconomy_NotifyLocalEnergyShortage
          (GameFactionRuntimeRecord *factionRecord,EnergyDemandQ4 suppliedDemandQ4)
{
  InGameLevelConditionStorage *levelConditionStorage;
  InGameNotificationMovieId notificationMovieId;

  if (factionRecord->energyGenerationCapacityQ4 < suppliedDemandQ4 + factionRecord->unpoweredEnergyDemandQ4) {
    if (factionRecord->anchorCooldown1 != 0) return;
    notificationMovieId = 400;
    factionRecord->anchorCooldown1 = 150;
  }
  else {
    if (factionRecord->anchorCooldown2 != 0) return;
    notificationMovieId = 401;
    factionRecord->anchorCooldown2 = 150;
  }
  /* Level header text starting with UTF-16 "t00_tu": a fixed notification, and both cooldowns never
     expire. */
  levelConditionStorage = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
  if (((*(int *)&(levelConditionStorage->levelImage).header.levelFileNameUtf16[0] == UTF16_CHAR_PAIR('t','0')) &&
      (*(int *)&(levelConditionStorage->levelImage).header.levelFileNameUtf16[2] == UTF16_CHAR_PAIR('0','_'))) &&
     (*(int *)&(levelConditionStorage->levelImage).header.levelFileNameUtf16[4] == UTF16_CHAR_PAIR('t','u'))) {
    notificationMovieId = 402;
    factionRecord->anchorCooldown1 = INT32_MAX;
    factionRecord->anchorCooldown2 = INT32_MAX;
  }
  InGameNotificationQueue_InsertPriorityRecord(NOTIFICATION_PAYLOAD_NONE,0,0,0,0,0,3,notificationMovieId);
}

/* Energy allocation for one faction: the supply (baselineEnergySupplyQ4 + Tritium stock * 16, capped by
   energyGenerationCapacityQ4, signed comparison) first covers the fixed demand of its army assets, then its
   consumers in priority order; consumers left over get model runtime classState.stateFlags bit 0 (unpowered).
   The energy used above the baseline burns Tritium. */
static void InGameFactionEconomy_AllocateFactionEnergy
          (GameFactionRuntimeRecord *factionRecord,uint32_t factionIndex,
          const FactionEnergyConsumerEntry *consumers,uint32_t consumerCount)
{
  EnergyDemandQ4 armyAssetDemand;
  FactionArmyAssetCount assetIndex;
  EnergyAmountQ4 supply;
  EnergyAmountQ4 remainingEnergy;
  uint32_t consumerIndex;
  const FactionEnergyConsumerEntry *consumer;
  ModelRuntimeSlot *consumerRuntime;
  EnergyDemandQ4 suppliedDemand;
  EnergyAmountQ4 tritiumBurnEnergy;

  /* fixed demand: 1 energy (0x10 Q4) per army asset, 5 (0x50) when its definitionClassValue74 is set */
  armyAssetDemand = 0;
  for (assetIndex = 0; assetIndex < factionRecord->primaryArmyAssetCount; assetIndex++) {
    if (Thandor_U32ToPointer<ArmyAssetRecord>(factionRecord->primaryArmyAssetPointersOrIds[assetIndex])->definitionClassValue74 == 0) { /* 5f-format: GameFactionRuntimeRecord.primaryArmyAssetPointersOrIds */
      armyAssetDemand = armyAssetDemand + 16;
    }
    else {
      armyAssetDemand = armyAssetDemand + 80;
    }
  }
  supply = factionRecord->tritiumCurrentQ4 * 16 + factionRecord->baselineEnergySupplyQ4;
  if ((int)factionRecord->energyGenerationCapacityQ4 < (int)supply) {
    supply = factionRecord->energyGenerationCapacityQ4;
  }
  factionRecord->suppliedEnergyDemandQ4 = factionRecord->suppliedEnergyDemandQ4 + armyAssetDemand;
  remainingEnergy = supply - armyAssetDemand;
  if (supply < armyAssetDemand) {
    remainingEnergy = 0;
  }
  for (consumerIndex = 0; consumerIndex < consumerCount; consumerIndex++) {
    consumer = &consumers[consumerIndex];
    if (factionIndex != consumer->factionIndex) continue;
    consumerRuntime = (ModelRuntimeSlot *)(uintptr_t)consumer->modelRuntime;
    if (remainingEnergy < consumer->demandQ4) {
      consumerRuntime->classState.stateFlags = consumerRuntime->classState.stateFlags | 1;
      factionRecord->unpoweredEnergyDemandQ4 = factionRecord->unpoweredEnergyDemandQ4 + consumer->demandQ4;
    }
    else {
      remainingEnergy = remainingEnergy - consumer->demandQ4;
      factionRecord->suppliedEnergyDemandQ4 = factionRecord->suppliedEnergyDemandQ4 + consumer->demandQ4;
      consumerRuntime->classState.stateFlags = consumerRuntime->classState.stateFlags & ~1u;
    }
  }
  /* energy above the baseline supply is Tritium burnt */
  suppliedDemand = factionRecord->suppliedEnergyDemandQ4;
  tritiumBurnEnergy = suppliedDemand - factionRecord->baselineEnergySupplyQ4;
  if (suppliedDemand < factionRecord->baselineEnergySupplyQ4) {
    tritiumBurnEnergy = 0;
  }
  if (factionRecord->unpoweredEnergyDemandQ4 == 0) {
    factionRecord->anchorCooldown1 = 0;
  }
  else if ((g_InGameRuntimeRoot->worldRuntime).activeFactionRuntimeIndex == factionIndex) {
    InGameFactionEconomy_NotifyLocalEnergyShortage(factionRecord,suppliedDemand);
  }
  factionRecord->tritiumCurrentQ4 =
       factionRecord->tritiumCurrentQ4 - (tritiumBurnEnergy >> 4) * g_InGameSimulationStepTicks;
}

/* Stat table row simulationTick / 128 (0x1000 rows of 7 factions x 2 dwords): the metrics
   combinedProgressScore/activeArmyContribution of factions 1..7, clamped at zero. */
static void InGameFactionEconomy_StoreStatTableSample()
{
  GameFactionRuntimeRecord *statFactionRecord;
  WorldRuntimeContext *worldRuntime;
  int *statSample;
  FactionRuntimeIndex factionIndex;
  FactionProgressScore progressScore;
  FactionProgressScore armyContribution;

  statFactionRecord = &g_GameFactionRuntimeImage.records[1];
  worldRuntime = &g_InGameRuntimeRoot->worldRuntime;
  if (g_GameFactionRuntimeImage.tail.simulationTick >> 7 >= 4096) return;
  statSample = (int *)((uint8_t *)g_GameStatTableImage +
                       (g_GameFactionRuntimeImage.tail.simulationTick >> 7) * RESULTS_STAT_SAMPLE_BYTES);
  for (factionIndex = 1; factionIndex < 8; factionIndex++) {
    GameFactionRuntime_RecomputeProgressAndScoreMetrics(factionIndex,worldRuntime);
    progressScore = statFactionRecord->combinedProgressScore;
    armyContribution = statFactionRecord->activeArmyContribution;
    if (progressScore < 0) {
      progressScore = 0;
    }
    if (armyContribution < 0) {
      armyContribution = 0;
    }
    statSample[0] = progressScore;
    statSample[1] = armyContribution;
    statFactionRecord++;
    statSample = statSample + 2;
  }
}

/* The faction economy, run every 8th simulation step (job 0 of InGameRuntime_UpdateSimulationAndNetworkTick):
   1. per faction: reset the step's energy demand and extraction rates, decay the pair-pressure matrix by 7/8,
      count the notification/anchor cooldowns down;
   2. mining: every connected region of Xenite cells (FIELD_CELL_XENITE_SUPPORT), then of Tritium cells
      (FIELD_CELL_TRITIUM_SUPPORT), pays its owning factions (rate, stock and total), then stocks are capped at
      the storage limits;
   3. energy: all powered models (and the attached parts of models whose class has flag 0x80) are sorted by the
      priority of their class (g_FactionEnergyAllocationPriorityByModelClass); per faction 7..1 the supply
      (baselineEnergySupplyQ4 + Tritium stock * 16, capped by energyGenerationCapacityQ4) first covers the fixed
      demand of its army assets, then the consumers in priority order; consumers left over are flagged unpowered
      and the local player gets notification 400 (generation capacity too low) or 401 (supply too low), at
      most every 150 economy runs. The energy used above the baseline burns Tritium;
   4. every 128 steps the progress/score metrics of factions 1..7 are stored in the stat table
      (g_GameStatTableImage, shown by the results screen).
   All amounts are Q4 fixed point.
*/
void InGameRuntime_UpdateFactionResourceExtractionAndEnergyAllocationState()

{
  FieldGridAsset *fieldGrid;
  FactionEnergyConsumerEntry *consumers;
  uint32_t consumerCount;
  uint32_t factionIndex;

  InGameFactionEconomy_ResetAndDecayFactionState();
  /* 2. mining: first pass Xenite cells, second pass Tritium cells */
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  InGameFactionEconomy_PayResourceRegions(fieldGrid,FIELD_CELL_XENITE_SUPPORT,false);
  InGameFactionEconomy_PayResourceRegions(fieldGrid,FIELD_CELL_TRITIUM_SUPPORT,true);
  InGameFactionEconomy_CapStocksAtStorageLimits();
  /* 3. energy */
  consumers = (FactionEnergyConsumerEntry *)(uintptr_t)g_TerrainRegionCollectionEntries;
  consumerCount = InGameFactionEconomy_CollectEnergyConsumers(consumers);
  /* without any consumer the whole allocation is skipped (no army-asset demand, no Tritium burn) */
  if (consumerCount != 0) {
    InGameFactionEconomy_SortEnergyConsumersByPriority(consumers,consumerCount);
    /* allocate per faction 7..1 (faction 0 gets nothing) */
    for (factionIndex = 7; factionIndex != 0; factionIndex--) {
      InGameFactionEconomy_AllocateFactionEnergy
                (&g_GameFactionRuntimeImage.records[factionIndex],factionIndex,consumers,consumerCount);
    }
  }
  /* 4. every 128 steps: stat table sample */
  if ((g_GameFactionRuntimeImage.tail.simulationTick & INGAME_STAT_SAMPLE_TICK_MASK) == 0) {
    InGameFactionEconomy_StoreStatTableSample();
  }
}
