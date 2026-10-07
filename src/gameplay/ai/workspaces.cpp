/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/ai/workspaces.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/ai/workspaces.h>
#include <thandor/thandor.h>

/* Module data. */

AiCandidateWorkspaceEntry *g_AiWorkspace13Candidates = nullptr;

uint32_t g_AiCandidateWorkspaceEntryCount = 0;

AiStructureWorkspaceEntry *g_AiWorkspace00Structures = nullptr;

uint32_t g_AiWorkspace00Count = 0;

AiRuntimeWorkspaceEntry *g_AiWorkspace01Units = nullptr;

uint32_t g_AiWorkspace01Count = 0;

AiRuntimeWorkspaceEntry *g_AiWorkspace03UnseenHostiles = nullptr;

uint32_t g_AiWorkspace03Count = 0;

AiRuntimeWorkspaceEntry *g_AiWorkspace04RequestedAssets = nullptr;

uint32_t g_AiWorkspace04Count = 0;

AiScoredSiteWorkspaceEntry *g_AiWorkspace05GeneralSites = nullptr;

uint32_t g_AiWorkspace05Count = 0;

AiScoredSiteWorkspaceEntry *g_AiWorkspace06FlaggedSites = nullptr;

uint32_t g_AiWorkspace06Count = 0;

AiTargetWorkspaceEntry *g_AiWorkspace07Targets = nullptr;

uint32_t g_AiWorkspace07Count = 0;

AiTerrainFeatureWorkspaceEntry *g_AiWorkspace08TerrainFeatureSites = nullptr;

uint32_t g_AiWorkspace08Count = 0;

FieldGridCell **g_AiWorkspace09Cells = nullptr;

uint32_t g_AiWorkspace09Count = 0;

FieldGridCell **g_AiWorkspace10Cells = nullptr;

uint32_t g_AiWorkspace10Count = 0;

ArmyAssetRecordPrefix **g_AiWorkspace11ProducibleAssets = nullptr;

uint32_t g_AiWorkspace11Count = 0;

AiTechnologyPlanningCandidate *g_AiWorkspace12TechnologyCandidates = nullptr;

AiTechnologyPlanningCandidateCount g_AiWorkspace12Count = 0;

ArmyRuntimeSlot **g_AiWorkspace14CollectedArmies = nullptr;

AiRuntimeWorkspaceEntry *g_AiWorkspace02VisibleHostiles = nullptr;

uint32_t g_AiWorkspace02Count = 0;

/* L"engine\\ki.dat" */
static uint16_t g_EngineKiDatPathUtf16[14] = {'e', 'n', 'g', 'i', 'n', 'e', '\\', 'k', 'i', '.', 'd', 'a', 't', 0};

/* Candidate cache of the faction runtime record at factionImageByteOffset (faction * 0x740; a byte offset into
   the record array, applied as the original does) */
static inline AiFactionCandidateCacheState *AI_FACTION_CANDIDATE_CACHE(FactionImageByteOffset factionImageByteOffset)
{
  return reinterpret_cast<AiFactionCandidateCacheState *>(
       reinterpret_cast<uint8_t *>(&g_GameFactionRuntimeImage.records[0].candidateCache) + factionImageByteOffset);
}

/* Proposes armyAssetId at the first workspace 08 site of that asset where it can be placed (placement mode 4),
   unless one of it is still unassigned. Weight: 3 * baseWeight / (existing count + 3); for assets other than
   ARM 330 (0x14A) additionally scaled by (2 * unpowered + supplied Energy demand) /
   (record tritiumExtractionRateQ4PerTick << 4) when that rate is nonzero.
*/
void AiWorkspaceAssetCandidate_AddWeightedEntry(AiCandidateScore32 baseWeight,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  int remaining;
  int assignedCount;
  int extractionRate;
  uint32_t weightRange;
  AiTerrainFeatureWorkspaceEntry *terrainFeatureEntry;

  remaining = g_AiWorkspace08Count;
  terrainFeatureEntry = g_AiWorkspace08TerrainFeatureSites;
  if (g_AiWorkspace08Count == 0) {
    return;
  }
  if (AiPrimaryWorkspace_HasUnassignedEntryById(armyAssetId)) {
    return;
  }
  for (; remaining != 0; remaining--, terrainFeatureEntry++) {
    if (armyAssetId != terrainFeatureEntry->armyAssetId) {
      continue;
    }
    if (AiPlacement_TestMode4AtWorkspaceRecord(armyAssetId,terrainFeatureEntry->cell,factionIndex,worldRuntime)) {
      continue;
    }
    assignedCount = AiPrimaryWorkspace_CountAssignedEntriesById(armyAssetId);
    weightRange = (uint32_t)(baseWeight * 3) / (assignedCount + 3U);
    if (armyAssetId != ARM_0330_BUILDING_MDL0303) {
      extractionRate = g_GameFactionRuntimeImage.records[factionIndex].tritiumExtractionRateQ4PerTick << 4;
      if (extractionRate != 0) {
        weightRange = (uint32_t)(((int64_t)(int)weightRange *
                                  (int64_t)(int)(g_GameFactionRuntimeImage.records[factionIndex].
                                                 unpoweredEnergyDemandQ4 * 2 +
                                                 g_GameFactionRuntimeImage.records[factionIndex].
                                                 suppliedEnergyDemandQ4)) / (int64_t)extractionRate);
      }
    }
    AiCandidateWorkspace_AddOrAccumulateWeightedEntry(armyAssetId,weightRange,1);
    return;
  }
}

/* Empties the AI candidate workspace (workspace 13) by resetting its entry count.
*/
void AiCandidateWorkspace_Clear()

{
  g_AiCandidateWorkspaceEntryCount = 0;
}

/* Keeps the first (at most three) candidates of the AI candidate workspace in the faction's runtime record
   (candidateCache, factionImageByteOffset = faction * 0x740) so that the next planning pass of this faction can
   start from them (AiCandidateWorkspace_LoadFromFactionImage).
*/
void AiCandidateWorkspace_SaveToFactionImage(FactionImageByteOffset factionImageByteOffset)

{
  int entryCount;
  int dwordsRemaining;
  uint32_t *candidateWorkspaceSourceCursor;
  uint32_t *factionImageDestinationCursor;

  candidateWorkspaceSourceCursor = &g_AiWorkspace13Candidates->weightedScoreAndKind;
  entryCount = g_AiCandidateWorkspaceEntryCount;
  if (2 < g_AiCandidateWorkspaceEntryCount) {
    entryCount = 3;
  }
  AI_FACTION_CANDIDATE_CACHE(factionImageByteOffset)->savedEntryCount = entryCount;
  factionImageDestinationCursor =
       &AI_FACTION_CANDIDATE_CACHE(factionImageByteOffset)->savedEntries[0].weightedScoreAndKind;
  /* two dwords per entry */
  for (dwordsRemaining = entryCount * 2; dwordsRemaining != 0; dwordsRemaining--) {
    *factionImageDestinationCursor = *candidateWorkspaceSourceCursor;
    candidateWorkspaceSourceCursor++;
    factionImageDestinationCursor++;
  }
}

/* Refills the shared AI candidate workspace with the candidates that AiCandidateWorkspace_SaveToFactionImage
   kept in the faction's runtime record (factionImageByteOffset = faction * 0x740).
*/
void AiCandidateWorkspace_LoadFromFactionImage(FactionImageByteOffset factionImageByteOffset)

{
  int copyDwordsRemaining;
  uint32_t *factionImageSourceCursor;
  uint32_t *candidateWorkspaceDestinationCursor;
  
  g_AiCandidateWorkspaceEntryCount = AI_FACTION_CANDIDATE_CACHE(factionImageByteOffset)->savedEntryCount;
  factionImageSourceCursor =
       &AI_FACTION_CANDIDATE_CACHE(factionImageByteOffset)->savedEntries[0].weightedScoreAndKind;
  copyDwordsRemaining = g_AiCandidateWorkspaceEntryCount * 2;
  candidateWorkspaceDestinationCursor = &g_AiWorkspace13Candidates->weightedScoreAndKind;
  if (copyDwordsRemaining != 0) {
    for (; copyDwordsRemaining != 0; copyDwordsRemaining--) {
      *candidateWorkspaceDestinationCursor = *factionImageSourceCursor;
      factionImageSourceCursor++;
      candidateWorkspaceDestinationCursor++;
    }
  }
}

/* Sorts the AI candidate workspace (workspace 13) by descending weightedScoreAndKind (signed compare), so the
   purchase planner tries the best candidates first. Selection sort: each pass swaps every higher entry into the
   pass's first slot, carrying the id/multiplicity dword along.
*/
void AiCandidateWorkspace_SortDescending()

{
  int currentRecordScore;
  uint32_t recordsInCurrentPass;
  int comparisonsRemaining;
  int currentRecordPayload;
  AiCandidateWorkspaceEntry *scanRecordCursor;
  AiCandidateWorkspaceEntry *currentRecordCursor;
  uint32_t promotedScore;
  uint32_t promotedPayload;
  
  if (1 < g_AiCandidateWorkspaceEntryCount) {
    currentRecordScore = g_AiWorkspace13Candidates->weightedScoreAndKind;
    currentRecordPayload = g_AiWorkspace13Candidates->entityIdAndMultiplicity;
    scanRecordCursor = g_AiWorkspace13Candidates + 1;
    comparisonsRemaining = g_AiCandidateWorkspaceEntryCount - 1;
    recordsInCurrentPass = g_AiCandidateWorkspaceEntryCount;
    currentRecordCursor = g_AiWorkspace13Candidates;
    do {
      for (; comparisonsRemaining != 0; comparisonsRemaining--) {
        if (currentRecordScore < (int)scanRecordCursor->weightedScoreAndKind) {
          promotedScore = scanRecordCursor->weightedScoreAndKind;
          scanRecordCursor->weightedScoreAndKind = currentRecordScore;
          promotedPayload = scanRecordCursor->entityIdAndMultiplicity;
          scanRecordCursor->entityIdAndMultiplicity = currentRecordPayload;
          currentRecordCursor->weightedScoreAndKind = promotedScore;
          currentRecordCursor->entityIdAndMultiplicity = promotedPayload;
          currentRecordScore = promotedScore;
          currentRecordPayload = promotedPayload;
        }
        scanRecordCursor++;
      }
      currentRecordScore = currentRecordCursor[1].weightedScoreAndKind;
      currentRecordPayload = currentRecordCursor[1].entityIdAndMultiplicity;
      comparisonsRemaining = recordsInCurrentPass - 2;
      scanRecordCursor = currentRecordCursor + 2;
      recordsInCurrentPass--;
      currentRecordCursor++;
    } while (comparisonsRemaining != 0);
  }
}

/* Returns the xenite cost (Q4) of a candidate, which the purchase planner checks against the faction's xenite:
   the technology's xeniteCostQ4 for a technology candidate, else the army asset's xeniteCostQ4, or
   0x7FFFFFFF (never affordable) when the asset is unknown.
*/
int AiCandidateWorkspace_GetEntryXeniteCost(AiCandidateWorkspaceEntry *entry)

{
  uint32_t xeniteCostQ4;
  RuntimeToken registryId;
  uint32_t lookupError;
  ArmyAssetRecordPrefix *armyAsset;

  registryId = entry->entityIdAndMultiplicity & AI_CANDIDATE_ID_MASK;
  if ((entry->weightedScoreAndKind & AI_CANDIDATE_KIND_MASK) == AI_CANDIDATE_KIND_TECHNOLOGY) {
    xeniteCostQ4 = g_TechnologyAsset->records[registryId].xeniteCostQ4;
  }
  else {
    lookupError = ArmyAssetRegistry_FindById(registryId,&armyAsset);
    xeniteCostQ4 = INT32_MAX;
    if (lookupError == 0) {
      /* ArmyAssetRecord.xeniteCostQ4, reached through the 16-byte prefix type */
      xeniteCostQ4 = armyAsset[2].registryId;
    }
  }
  return xeniteCostQ4;
}

/* Returns true when the secondary workspace (workspace 01) holds an entry of this army asset, assigned
   or not.
*/
Bool8 AiSecondaryWorkspace_HasEntryById(PckArmyAssetIdCatalog entryId)

{
  int workspaceEntriesRemaining;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;

  workspaceEntriesRemaining = g_AiWorkspace01Count;
  workspaceEntryCursor = g_AiWorkspace01Units;
  for (; workspaceEntriesRemaining != 0; workspaceEntriesRemaining--) {
    if (entryId == workspaceEntryCursor->armyAssetId) {
      return true;
    }
    workspaceEntryCursor++;
  }
  return false;
}

/* Returns the smallest Manhattan distance from the point to an assigned secondary-workspace (workspace 01)
   entry, measured to the linked entity's path coordinates, or 0x7FFFFFFF when there is none.
   Arguments are Y first, then X, as every caller passes them.
*/
int AiSecondaryWorkspace_GetMinimumManhattanDistanceToPoint(Q12 worldY,Q12 worldX)

{
  Q12 minimumManhattanDistanceQ12;
  int workspaceEntriesRemaining;
  int deltaYAbsQ12;
  Q12 deltaXAbsQ12;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  GameEntityRuntime *entityRuntime;
  
  minimumManhattanDistanceQ12 = INT32_MAX;
  workspaceEntryCursor = g_AiWorkspace01Units;
  for (workspaceEntriesRemaining = g_AiWorkspace01Count; workspaceEntriesRemaining != 0;
      workspaceEntriesRemaining--) {
    if (workspaceEntryCursor->modelRuntime != nullptr) {
      entityRuntime = workspaceEntryCursor->modelRuntime->ownerArmyRuntimeOrSavedOffset.entityRuntime;
      deltaXAbsQ12 = worldX - (entityRuntime->common).pathCoordinate0Q12;
      if (deltaXAbsQ12 < 0) {
        deltaXAbsQ12 = -deltaXAbsQ12;
      }
      deltaYAbsQ12 = worldY - (entityRuntime->common).pathCoordinate1Q12;
      if (deltaYAbsQ12 < 0) {
        deltaYAbsQ12 = -deltaYAbsQ12;
      }
      if (deltaXAbsQ12 + deltaYAbsQ12 < minimumManhattanDistanceQ12) {
        minimumManhattanDistanceQ12 = deltaXAbsQ12 + deltaYAbsQ12;
      }
    }
    workspaceEntryCursor++;
  }
  return minimumManhattanDistanceQ12;
}

/* Returns the smallest Manhattan distance from the point to an assigned primary-workspace (workspace 00) unit
   whose linked entity has a nonzero commandState (i.e. is active), or 0x7FFFFFFF when there is none.
   Arguments are Y first, then X, as every caller passes them.
*/
int AiPrimaryWorkspace_GetMinimumActiveManhattanDistanceToPoint(Q12 worldY,Q12 worldX)

{
  Q12 minimumActiveManhattanDistanceQ12;
  int workspaceEntriesRemaining;
  int deltaYAbsQ12;
  Q12 deltaXAbsQ12;
  AiStructureWorkspaceEntry *workspaceEntryCursor;
  ModelRuntimeSlot *slotModelRuntime;
  
  minimumActiveManhattanDistanceQ12 = INT32_MAX;
  workspaceEntryCursor = g_AiWorkspace00Structures;
  for (workspaceEntriesRemaining = g_AiWorkspace00Count; workspaceEntriesRemaining != 0;
      workspaceEntriesRemaining--) {
    slotModelRuntime = workspaceEntryCursor->runtimeSlotAddressOrZero;
    if ((slotModelRuntime != nullptr) &&
       ((slotModelRuntime->ownerArmyRuntimeOrSavedOffset.entityRuntime->common).commandState != 0)) {
      deltaXAbsQ12 = worldX - (slotModelRuntime->rootModelNodeOrSavedOffset.modelNode->worldTransform).translation.x;
      if (deltaXAbsQ12 < 0) {
        deltaXAbsQ12 = -deltaXAbsQ12;
      }
      deltaYAbsQ12 = worldY - (slotModelRuntime->rootModelNodeOrSavedOffset.modelNode->worldTransform).translation.y;
      if (deltaYAbsQ12 < 0) {
        deltaYAbsQ12 = -deltaYAbsQ12;
      }
      if (deltaXAbsQ12 + deltaYAbsQ12 < minimumActiveManhattanDistanceQ12) {
        minimumActiveManhattanDistanceQ12 = deltaXAbsQ12 + deltaYAbsQ12;
      }
    }
    workspaceEntryCursor++;
  }
  return minimumActiveManhattanDistanceQ12;
}

/* Returns the smallest Manhattan distance from the point to an assigned workspace-02 unit, or 0x7FFFFFFF when
   there is none. Arguments are Y first, then X, as every caller passes them.
*/
int AiHostileWorkspace_GetNearestVisibleHostileDistance(Q12 worldY,Q12 worldX)

{
  Q12 minimumManhattanDistanceQ12;
  int workspaceEntriesRemaining;
  int deltaYAbsQ12;
  Q12 deltaXAbsQ12;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  ModelRuntimeNode *modelNode;
  
  minimumManhattanDistanceQ12 = INT32_MAX;
  workspaceEntryCursor = g_AiWorkspace02VisibleHostiles;
  for (workspaceEntriesRemaining = g_AiWorkspace02Count; workspaceEntriesRemaining != 0;
      workspaceEntriesRemaining--) {
    if (workspaceEntryCursor->modelRuntime != nullptr) {
      modelNode = workspaceEntryCursor->modelRuntime->rootModelNodeOrSavedOffset.modelNode;
      deltaXAbsQ12 = worldX - (modelNode->worldTransform).translation.x;
      if (deltaXAbsQ12 < 0) {
        deltaXAbsQ12 = -deltaXAbsQ12;
      }
      deltaYAbsQ12 = worldY - (modelNode->worldTransform).translation.y;
      if (deltaYAbsQ12 < 0) {
        deltaYAbsQ12 = -deltaYAbsQ12;
      }
      if (deltaXAbsQ12 + deltaYAbsQ12 < minimumManhattanDistanceQ12) {
        minimumManhattanDistanceQ12 = deltaXAbsQ12 + deltaYAbsQ12;
      }
    }
    workspaceEntryCursor++;
  }
  return minimumManhattanDistanceQ12;
}

/* Returns the smallest Manhattan distance from the point to an assigned workspace-03 unit, or 0x7FFFFFFF when
   there is none. Arguments are Y first, then X, as every caller passes them.
*/
int AiHostileWorkspace_GetNearestUnseenHostileDistance(Q12 worldY,Q12 worldX)

{
  Q12 minimumManhattanDistanceQ12;
  int workspaceEntriesRemaining;
  int deltaYAbsQ12;
  Q12 deltaXAbsQ12;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  ModelRuntimeNode *modelNode;
  
  minimumManhattanDistanceQ12 = INT32_MAX;
  workspaceEntryCursor = g_AiWorkspace03UnseenHostiles;
  for (workspaceEntriesRemaining = g_AiWorkspace03Count; workspaceEntriesRemaining != 0;
      workspaceEntriesRemaining--) {
    if (workspaceEntryCursor->modelRuntime != nullptr) {
      modelNode = workspaceEntryCursor->modelRuntime->rootModelNodeOrSavedOffset.modelNode;
      deltaXAbsQ12 = worldX - (modelNode->worldTransform).translation.x;
      if (deltaXAbsQ12 < 0) {
        deltaXAbsQ12 = -deltaXAbsQ12;
      }
      deltaYAbsQ12 = worldY - (modelNode->worldTransform).translation.y;
      if (deltaYAbsQ12 < 0) {
        deltaYAbsQ12 = -deltaYAbsQ12;
      }
      if (deltaXAbsQ12 + deltaYAbsQ12 < minimumManhattanDistanceQ12) {
        minimumManhattanDistanceQ12 = deltaXAbsQ12 + deltaYAbsQ12;
      }
    }
    workspaceEntryCursor++;
  }
  return minimumManhattanDistanceQ12;
}

/* Returns the smallest Manhattan distance from the point to any assigned primary-workspace (workspace 00) unit,
   or 0x7FFFFFFF when there is none. Arguments are Y first, then X, as every caller passes them.
*/
int AiPrimaryWorkspace_GetMinimumManhattanDistanceToPoint(Q12 worldY,Q12 worldX)

{
  Q12 minimumManhattanDistanceQ12;
  int workspaceEntriesRemaining;
  int deltaYAbsQ12;
  Q12 deltaXAbsQ12;
  AiStructureWorkspaceEntry *workspaceEntryCursor;
  ModelRuntimeNode *modelNode;
  
  minimumManhattanDistanceQ12 = INT32_MAX;
  workspaceEntryCursor = g_AiWorkspace00Structures;
  for (workspaceEntriesRemaining = g_AiWorkspace00Count; workspaceEntriesRemaining != 0;
      workspaceEntriesRemaining--) {
    if (workspaceEntryCursor->runtimeSlotAddressOrZero != nullptr) {
      modelNode = workspaceEntryCursor->runtimeSlotAddressOrZero->rootModelNodeOrSavedOffset.modelNode;
      deltaXAbsQ12 = worldX - (modelNode->worldTransform).translation.x;
      if (deltaXAbsQ12 < 0) {
        deltaXAbsQ12 = -deltaXAbsQ12;
      }
      deltaYAbsQ12 = worldY - (modelNode->worldTransform).translation.y;
      if (deltaYAbsQ12 < 0) {
        deltaYAbsQ12 = -deltaYAbsQ12;
      }
      if (deltaXAbsQ12 + deltaYAbsQ12 < minimumManhattanDistanceQ12) {
        minimumManhattanDistanceQ12 = deltaXAbsQ12 + deltaYAbsQ12;
      }
    }
    workspaceEntryCursor++;
  }
  return minimumManhattanDistanceQ12;
}

/* Allocates the fifteen AI workspace buffers 00-14 from the arena (sizes in their names) and loads the AI
   parameters from engine\ki.dat into g_AiKnowledgeData. Returns true on success; stops at the first failure,
   returning false with that failure's error code in *outErrorCode (buffers allocated before it are not freed).
*/
Bool8 AiRuntime_InitWorkspace(uint32_t *outErrorCode)

{
  AiKnowledgeDataImage *knowledgeDataImage;
  uint32_t allocError;
  uint32_t loadErrorCode;

  /* the arena returns the block through a void ** out parameter; each typed workspace pointer is passed as one
     (same object representation) */
  allocError = g_MemoryApi.alloc(AI_WORKSPACE00_CAPACITY * sizeof(AiStructureWorkspaceEntry),
                                 reinterpret_cast<void **>(&g_AiWorkspace00Structures));
  if (allocError == 0) {
    allocError = g_MemoryApi.alloc(AI_WORKSPACE01_CAPACITY * sizeof(AiRuntimeWorkspaceEntry),
                                   reinterpret_cast<void **>(&g_AiWorkspace01Units));
    if (allocError == 0) {
      allocError = g_MemoryApi.alloc(AI_WORKSPACE02_CAPACITY * sizeof(AiRuntimeWorkspaceEntry),
                                     reinterpret_cast<void **>(&g_AiWorkspace02VisibleHostiles));
      if (allocError == 0) {
        allocError = g_MemoryApi.alloc(AI_WORKSPACE03_CAPACITY * sizeof(AiRuntimeWorkspaceEntry),
                                       reinterpret_cast<void **>(&g_AiWorkspace03UnseenHostiles));
        if (allocError == 0) {
          allocError = g_MemoryApi.alloc(AI_WORKSPACE04_CAPACITY * sizeof(AiRuntimeWorkspaceEntry),
                                         reinterpret_cast<void **>(&g_AiWorkspace04RequestedAssets));
          if (allocError == 0) {
            allocError = g_MemoryApi.alloc(AI_WORKSPACE05_CAPACITY * sizeof(AiScoredSiteWorkspaceEntry),
                                           reinterpret_cast<void **>(&g_AiWorkspace05GeneralSites));
            if (allocError == 0) {
              allocError = g_MemoryApi.alloc(AI_WORKSPACE06_CAPACITY * sizeof(AiScoredSiteWorkspaceEntry),
                                             reinterpret_cast<void **>(&g_AiWorkspace06FlaggedSites));
              if (allocError == 0) {
                allocError = g_MemoryApi.alloc(AI_WORKSPACE07_CAPACITY * sizeof(AiTargetWorkspaceEntry),
                                               reinterpret_cast<void **>(&g_AiWorkspace07Targets));
                if (allocError == 0) {
                  allocError = g_MemoryApi.alloc(AI_WORKSPACE08_CAPACITY * sizeof(AiTerrainFeatureWorkspaceEntry),
                                                 reinterpret_cast<void **>(&g_AiWorkspace08TerrainFeatureSites));
                  if (allocError == 0) {
                    allocError = g_MemoryApi.alloc(AI_WORKSPACE09_CAPACITY * sizeof(FieldGridCell *),
                                                   reinterpret_cast<void **>(&g_AiWorkspace09Cells));
                    if (allocError == 0) {
                      allocError = g_MemoryApi.alloc(AI_WORKSPACE10_CAPACITY * sizeof(FieldGridCell *),
                                                     reinterpret_cast<void **>(&g_AiWorkspace10Cells));
                      if (allocError == 0) {
                        allocError = g_MemoryApi.alloc(AI_WORKSPACE11_CAPACITY * sizeof(ArmyAssetRecordPrefix *),
                                                       reinterpret_cast<void **>(&g_AiWorkspace11ProducibleAssets));
                        if (allocError == 0) {
                          allocError = g_MemoryApi.alloc(AI_WORKSPACE12_CAPACITY * sizeof(AiTechnologyPlanningCandidate),
                                                         reinterpret_cast<void **>(&g_AiWorkspace12TechnologyCandidates));
                          if (allocError == 0) {
                            allocError = g_MemoryApi.alloc(AI_CANDIDATE_WORKSPACE_CAPACITY * sizeof(AiCandidateWorkspaceEntry),
                                                           reinterpret_cast<void **>(&g_AiWorkspace13Candidates));
                            if (allocError == 0) {
                              allocError = g_MemoryApi.alloc(AI_WORKSPACE14_CAPACITY * sizeof(ArmyRuntimeSlot *),
                                                             reinterpret_cast<void **>(&g_AiWorkspace14CollectedArmies));
                              if (allocError == 0) {
                                knowledgeDataImage = static_cast<AiKnowledgeDataImage *>(Package_LoadEntry(g_EngineKiDatPathUtf16,&loadErrorCode));
                                if (knowledgeDataImage != nullptr) {
                                  g_AiKnowledgeData = knowledgeDataImage;
                                  return true;
                                }
                                /* the failure exit reports the error of the last failed step */
                                allocError = loadErrorCode;
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  *outErrorCode = allocError;
  return false;
}

/* Adds a field cell to workspace 09 (at most 1024 cells) when it lies inside the extent (supportRadius of the
   definition) of some primary-workspace structure, i.e. when AiPrimaryWorkspace_IsPointOutsideAllEntryExtents
   returns false. These cells are the build sites near the AI's own base.
*/
void AiBaseSiteWorkspace_AddCellInsideBase(FieldGridCell *currentCell)

{
  FieldGridCell **cellBuffer;
  uint32_t entryIndex;
  Bool8 isOutsideExtents;

  entryIndex = g_AiWorkspace09Count;
  cellBuffer = g_AiWorkspace09Cells;
  if (g_AiWorkspace09Count < AI_WORKSPACE09_CAPACITY) {
    isOutsideExtents = AiPrimaryWorkspace_IsPointOutsideAllEntryExtents
                      (currentCell->worldY,currentCell->worldX);
    if (!isOutsideExtents) {
      cellBuffer[entryIndex] = currentCell;
      g_AiWorkspace09Count++;
    }
  }
}

/* Adds a field cell to workspace 10 (base sites with the wider clearance, at most 256 cells) when it lies inside
   the extent of some own structure; otherwise the same as AiBaseSiteWorkspace_AddCellInsideBase (workspace 09).
*/
void AiBaseSiteWorkspace_AddLargeCellInsideBase(FieldGridCell *currentCell)

{
  FieldGridCell **cellBuffer;
  uint32_t entryIndex;
  Bool8 isOutsideExtents;

  entryIndex = g_AiWorkspace10Count;
  cellBuffer = g_AiWorkspace10Cells;
  if (g_AiWorkspace10Count < AI_WORKSPACE10_CAPACITY) {
    isOutsideExtents = AiPrimaryWorkspace_IsPointOutsideAllEntryExtents
                      (currentCell->worldY,currentCell->worldX);
    if (!isOutsideExtents) {
      cellBuffer[entryIndex] = currentCell;
      g_AiWorkspace10Count++;
    }
  }
}

/* Returns true when the primary workspace (workspace 00) holds an entry of this army asset whose
   runtime pointer is NULL.
*/
Bool8 AiPrimaryWorkspace_HasUnassignedEntryById(PckArmyAssetIdCatalog entryId)

{
  int workspaceEntriesRemaining;
  AiStructureWorkspaceEntry *workspaceEntryCursor;
  
  workspaceEntriesRemaining = g_AiWorkspace00Count;
  workspaceEntryCursor = g_AiWorkspace00Structures;
  for (; workspaceEntriesRemaining != 0; workspaceEntriesRemaining--) {
    if ((entryId == workspaceEntryCursor->armyAssetId) &&
       (workspaceEntryCursor->runtimeSlotAddressOrZero == nullptr)) {
      return true;
    }
    workspaceEntryCursor++;
  }
  return false;
}

/* Returns true when the primary workspace (workspace 00) holds an entry of this army asset, with or
   without a runtime object.
*/
Bool8 AiPrimaryWorkspace_HasEntryById(PckArmyAssetIdCatalog entryId)

{
  int workspaceEntriesRemaining;
  AiStructureWorkspaceEntry *workspaceEntryCursor;

  workspaceEntriesRemaining = g_AiWorkspace00Count;
  workspaceEntryCursor = g_AiWorkspace00Structures;
  for (; workspaceEntriesRemaining != 0; workspaceEntriesRemaining--) {
    if (entryId == workspaceEntryCursor->armyAssetId) {
      return true;
    }
    workspaceEntryCursor++;
  }
  return false;
}

/* Counts the primary-workspace (workspace 00) entries of this army asset that have a runtime object.
*/
int AiPrimaryWorkspace_CountAssignedEntriesById(PckArmyAssetIdCatalog entryId)

{
  int matchingAssignedEntryCount;
  int workspaceEntriesRemaining;
  AiStructureWorkspaceEntry *workspaceEntryCursor;

  matchingAssignedEntryCount = 0;
  workspaceEntryCursor = g_AiWorkspace00Structures;
  for (workspaceEntriesRemaining = g_AiWorkspace00Count; workspaceEntriesRemaining != 0;
      workspaceEntriesRemaining--) {
    if ((workspaceEntryCursor->runtimeSlotAddressOrZero != nullptr) &&
       (entryId == workspaceEntryCursor->armyAssetId)) {
      matchingAssignedEntryCount++;
    }
    workspaceEntryCursor++;
  }
  return matchingAssignedEntryCount;
}

/* Proposes a purchase candidate (id + kind) with the randomised score 5 * weightRange + random % weightRange.
   An existing entry of the same kind and id gets the score added and its multiplicity raised by one; otherwise
   a new entry is appended while the workspace has fewer than 128. A weightRange of 0 or 1 proposes nothing.
*/
void AiCandidateWorkspace_AddOrAccumulateWeightedEntry
          (RuntimeToken entityId,uint32_t weightRange,AiCandidateEntryKind entryKind)

{
  uint32_t newEntryIndex;
  uint32_t randomValue;
  int weightedScore;
  uint32_t entriesRemaining;
  AiCandidateWorkspaceEntry *candidateEntry;
  
  randomValue = g_RandomGeneratorState.next();
  newEntryIndex = g_AiCandidateWorkspaceEntryCount;
  candidateEntry = g_AiWorkspace13Candidates;
  if (1 < weightRange) {
    weightedScore = weightRange * 5 + randomValue % weightRange;
    /* searched from the last entry down */
    for (entriesRemaining = g_AiCandidateWorkspaceEntryCount; entriesRemaining != 0; entriesRemaining--) {
      if (((g_AiWorkspace13Candidates[entriesRemaining - 1].weightedScoreAndKind & AI_CANDIDATE_KIND_MASK) ==
           entryKind) &&
         ((g_AiWorkspace13Candidates[entriesRemaining - 1].entityIdAndMultiplicity & AI_CANDIDATE_ID_MASK) ==
          entityId))
      {
        g_AiWorkspace13Candidates[entriesRemaining - 1].entityIdAndMultiplicity =
             g_AiWorkspace13Candidates[entriesRemaining - 1].entityIdAndMultiplicity +
             AI_CANDIDATE_MULTIPLICITY_ONE;
        candidateEntry = candidateEntry + (entriesRemaining - 1);
        /* the score sits above the 4 kind bits */
        candidateEntry->weightedScoreAndKind = candidateEntry->weightedScoreAndKind + weightedScore * AI_CANDIDATE_SCORE_ONE;
        return;
      }
    }
    if (g_AiCandidateWorkspaceEntryCount < AI_CANDIDATE_WORKSPACE_CAPACITY) {
      g_AiWorkspace13Candidates[g_AiCandidateWorkspaceEntryCount].entityIdAndMultiplicity =
           entityId + AI_CANDIDATE_MULTIPLICITY_ONE;
      g_AiCandidateWorkspaceEntryCount++;
      candidateEntry[newEntryIndex].weightedScoreAndKind = weightedScore * AI_CANDIDATE_SCORE_ONE | entryKind;
    }
  }
}

/* Returns false as soon as the point lies strictly inside the square extent of some assigned
   primary-workspace (workspace 00) unit: both axis distances to the unit's position below the extent
   supportRadius of its definition. True when it is outside all of them. Arguments are Y first, then X, as every
   caller passes them.
*/
Bool8 AiPrimaryWorkspace_IsPointOutsideAllEntryExtents(Q12 worldY,Q12 worldX)

{
  Q12 deltaXAbsQ12;
  int workspaceEntriesRemaining;
  Q12 deltaYAbsQ12;
  AiStructureWorkspaceEntry *workspaceEntryCursor;
  ModelRuntimeSlot *entryModelRuntime;
  
  workspaceEntryCursor = g_AiWorkspace00Structures;
  for (workspaceEntriesRemaining = g_AiWorkspace00Count; workspaceEntriesRemaining != 0;
      workspaceEntriesRemaining--, workspaceEntryCursor++) {
    entryModelRuntime = workspaceEntryCursor->runtimeSlotAddressOrZero;
    if (entryModelRuntime != nullptr) {
      deltaXAbsQ12 = worldX - entryModelRuntime->rootModelNodeOrSavedOffset.modelNode->worldTransform.translation.x;
      if (deltaXAbsQ12 < 0) {
        deltaXAbsQ12 = -deltaXAbsQ12;
      }
      deltaYAbsQ12 = worldY - entryModelRuntime->rootModelNodeOrSavedOffset.modelNode->worldTransform.translation.y;
      if (deltaYAbsQ12 < 0) {
        deltaYAbsQ12 = -deltaYAbsQ12;
      }
      if ((deltaXAbsQ12 < entryModelRuntime->definitionOrSavedId.runtimeDefinition->supportRadius) &&
         (deltaYAbsQ12 < entryModelRuntime->definitionOrSavedId.runtimeDefinition->supportRadius)) {
        return false;
      }
    }
  }
  return true;
}
