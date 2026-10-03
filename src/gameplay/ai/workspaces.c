/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/ai/workspaces.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/ai/workspaces.h>
#include <thandor/thandor.h>

/* Module data. */

AiCandidateWorkspaceEntry *g_AiWorkspace13Candidates = 0;

uint32_t g_AiCandidateWorkspaceEntryCount = 0;

AiStructureWorkspaceEntry *g_AiWorkspace00Structures = 0;

uint32_t g_AiWorkspace00Count = 0;

AiRuntimeWorkspaceEntry *g_AiWorkspace01Units = 0;

uint32_t g_AiWorkspace01Count = 0;

AiRuntimeWorkspaceEntry *g_AiWorkspace03UnseenHostiles = 0;

uint32_t g_AiWorkspace03Count = 0;

AiRuntimeWorkspaceEntry *g_AiWorkspace04RequestedAssets = 0;

uint32_t g_AiWorkspace04Count = 0;

AiScoredSiteWorkspaceEntry *g_AiWorkspace05GeneralSites = 0;

uint32_t g_AiWorkspace05Count = 0;

uint8_t *g_AiWorkspace06FlaggedSites = 0;

uint32_t g_AiWorkspace06Count = 0;

AiTargetWorkspaceEntry *g_AiWorkspace07Targets = 0;

uint32_t g_AiWorkspace07Count = 0;

AiTerrainFeatureWorkspaceEntry *g_AiWorkspace08TerrainFeatureSites = 0;

uint32_t g_AiWorkspace08Count = 0;

FieldGridCell **g_AiWorkspace09Cells = 0;

uint32_t g_AiWorkspace09Count = 0;

FieldGridCell **g_AiWorkspace10Cells = 0;

uint32_t g_AiWorkspace10Count = 0;

ArmyAssetRecordPrefix **g_AiWorkspace11ProducibleAssets = 0;

uint32_t g_AiWorkspace11Count = 0;

AiTechnologyPlanningCandidate *g_AiWorkspace12TechnologyCandidates = 0;

AiTechnologyPlanningCandidateCount g_AiWorkspace12Count = 0;

ArmyRuntimeSlot **g_AiWorkspace14CollectedArmies = 0;

static AiRuntimeWorkspaceEntry *g_AiWorkspace02VisibleHostiles = 0;

static uint32_t g_AiWorkspace02Count = 0;

static uint16_t u_engine_ki_dat_0053c5e4[14] = L"engine\\ki.dat";

/* Implementation ownership: gameplay/ai/workspaces. */

/* Candidate cache of the faction runtime record at factionImageByteOffset (faction * 0x740) */
#define AI_FACTION_CANDIDATE_CACHE(factionImageByteOffset) \
  ((AiFactionCandidateCacheState *)((uint8_t *)&g_GameFactionRuntimeImage.records[0].candidateCache + \
                                    (factionImageByteOffset)))

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


/* Appends (modelRuntime, assetId) to a runtime-entry workspace (01 to 04) unless it is full. */
static void AiPlanningRebuild_AddRuntimeEntry(AiRuntimeWorkspaceEntry *entries,uint32_t *entryCount,
          uint32_t capacity,ModelRuntimeSlot *modelRuntime,PckArmyAssetIdCatalog assetId)

{
  if (*entryCount < capacity) {
    entries[*entryCount].modelRuntime = modelRuntime;
    entries[*entryCount].armyAssetId = assetId;
    (*entryCount)++;
  }
}


/* Appends assetId as an unassigned (not yet built) entry to workspace 00 unless it is full. */
static void AiPlanningRebuild_AddUnassignedStructureEntry(PckArmyAssetIdCatalog assetId)

{
  if (g_AiWorkspace00Count < AI_WORKSPACE00_CAPACITY) {
    g_AiWorkspace00Structures[g_AiWorkspace00Count].runtimeSlotAddressOrZero = 0;
    g_AiWorkspace00Structures[g_AiWorkspace00Count].armyAssetId = assetId;
    g_AiWorkspace00Count++;
  }
}


/* World entities: own units (ARM < 300) into 01, own structures into 00 (the ARM 300 one also remembered);
   entities of other factions (owner != 0) whose capability bit for us is clear into 03 or 02, depending on how
   this faction sees them. */
static void AiPlanningRebuild_CollectWorldEntities(FactionRuntimeIndex factionIndex,WorldOwnerListNode *worldNode)

{
  ModelRuntimeSlot *modelRuntime;
  GameEntityRuntime *entityRuntime;
  PckArmyAssetIdCatalog assetId;
  uint32_t visibilityBits;

  for (; worldNode != NULL; worldNode = worldNode->nextNode) {
    if (worldNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
      continue;
    }
    /* the node's model runtime; entityRuntime is its owning army */
    modelRuntime = worldNode->runtimePayload;
    entityRuntime = modelRuntime->ownerArmyRuntimeOrSavedOffset.entityRuntime;
    if (factionIndex == entityRuntime->common.ownership.ownerIndex) {
      assetId = entityRuntime->common.runtimeIdentityOrArmyAssetId;
      if (assetId < ARM_0300_BUILDING_MDL0301) {
        AiPlanningRebuild_AddRuntimeEntry(g_AiWorkspace01Units,&g_AiWorkspace01Count,AI_WORKSPACE01_CAPACITY,
                                          modelRuntime,assetId);
      }
      else if (g_AiWorkspace00Count < AI_WORKSPACE00_CAPACITY) {
        g_AiWorkspace00Structures[g_AiWorkspace00Count].runtimeSlotAddressOrZero =
             (AiWorkspaceRuntimeSlotAddress32)modelRuntime;
        g_AiWorkspace00Structures[g_AiWorkspace00Count].armyAssetId = assetId;
        g_AiWorkspace00Count++;
        if (assetId == ARM_0300_BUILDING_MDL0301) {
          g_AiWorkspaceOwnedAsset300Runtime = modelRuntime;
        }
      }
      continue;
    }
    /* two bits per faction: how this faction sees the foreign entity */
    visibilityBits = entityRuntime->common.damageState.factionVisibilityBits1C >> ((char)factionIndex * 2 & 31U);
    if ((entityRuntime->common.ownership.ownerIndex != 0) &&
        GameFactionRuntime_TestCapabilityBitClear(entityRuntime->common.ownership.ownerIndex,factionIndex)) {
      if (((visibilityBits & 2) == 0) &&
          (((visibilityBits & 1) == 0 ||
            (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand
             [((ModelRuntimeSlot *)entityRuntime->common.ownership.definitionOrClassRecord)->definitionOrSavedId.
              runtimeDefinition->runtimeClassId] != ArmyRuntime_ClassCommandHandlerGroupA)))) {
        AiPlanningRebuild_AddRuntimeEntry(g_AiWorkspace03UnseenHostiles,&g_AiWorkspace03Count,
                                          AI_WORKSPACE03_CAPACITY,modelRuntime,
                                          entityRuntime->common.runtimeIdentityOrArmyAssetId);
      }
      else {
        AiPlanningRebuild_AddRuntimeEntry(g_AiWorkspace02VisibleHostiles,&g_AiWorkspace02Count,
                                          AI_WORKSPACE02_CAPACITY,modelRuntime,
                                          entityRuntime->common.runtimeIdentityOrArmyAssetId);
      }
    }
  }
}


/* The faction's pending (not yet built) army assets as unassigned entries: the primary list into 00 and as
   requests into 04, the secondary list into 01 (ARM < 300) or 00. */
static void AiPlanningRebuild_AddPendingArmyAssets(FactionRuntimeIndex factionIndex)

{
  uint32_t *armyAssetPointerCursor;
  FactionArmyAssetCount armyAssetsRemaining;
  PckArmyAssetIdCatalog assetId;

  armyAssetPointerCursor = g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetPointersOrIds;
  for (armyAssetsRemaining = g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
       armyAssetsRemaining != 0; armyAssetsRemaining--) {
    assetId = *(PckArmyAssetIdCatalog *)(*armyAssetPointerCursor + 8);
    AiPlanningRebuild_AddUnassignedStructureEntry(assetId);
    AiPlanningRebuild_AddRuntimeEntry(g_AiWorkspace04RequestedAssets,&g_AiWorkspace04Count,
                                      AI_WORKSPACE04_CAPACITY,NULL,assetId);
    armyAssetPointerCursor++;
  }
  armyAssetPointerCursor = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetPointersOrIds;
  for (armyAssetsRemaining = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount;
       armyAssetsRemaining != 0; armyAssetsRemaining--) {
    assetId = *(PckArmyAssetIdCatalog *)(*armyAssetPointerCursor + 8);
    if (assetId < ARM_0300_BUILDING_MDL0301) {
      AiPlanningRebuild_AddRuntimeEntry(g_AiWorkspace01Units,&g_AiWorkspace01Count,AI_WORKSPACE01_CAPACITY,NULL,
                                        assetId);
    }
    else {
      AiPlanningRebuild_AddUnassignedStructureEntry(assetId);
    }
    armyAssetPointerCursor++;
  }
}


/* Own structures of class 11 / 22 / 13 (among the 00 entries present before this step): the asset in production
   (slot word 24) counts as an unassigned entry (00 for class 11, 01 otherwise) while their state word (46, 43 for
   class 22) is 1. */
static void AiPlanningRebuild_AddAssetsInProduction(void)

{
  AiStructureWorkspaceEntry *structureEntry;
  uint32_t entriesRemaining;
  int *slotWords;
  ModelDefinition *slotDefinition;

  structureEntry = g_AiWorkspace00Structures;
  for (entriesRemaining = g_AiWorkspace00Count; entriesRemaining != 0; entriesRemaining--) {
    slotWords = (int *)structureEntry->runtimeSlotAddressOrZero;
    if (slotWords != NULL) {
      slotDefinition = (ModelDefinition *)*slotWords;
      if (slotDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_11) {
        if (slotWords[46] == 1) {
          AiPlanningRebuild_AddUnassignedStructureEntry(slotWords[24]);
        }
      }
      else if (slotDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_22) {
        if (slotWords[43] == 1) {
          AiPlanningRebuild_AddRuntimeEntry(g_AiWorkspace01Units,&g_AiWorkspace01Count,AI_WORKSPACE01_CAPACITY,
                                            NULL,slotWords[24]);
        }
      }
      else if ((slotDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_13) && (slotWords[46] == 1)) {
        AiPlanningRebuild_AddRuntimeEntry(g_AiWorkspace01Units,&g_AiWorkspace01Count,AI_WORKSPACE01_CAPACITY,NULL,
                                          slotWords[24]);
      }
    }
    structureEntry++;
  }
}


/* OR of the scratch-grid state of six scratch cells: topCell, topCell + step, the cells step to the left and right
   one scratch row (rowStride cells) further, and the cells step to the left and straight below two rows further. */
static uint32_t AiPlanningRebuild_ScratchFootprintMask(const GridScratchCell *topCell,uint32_t rowStride,
          uint32_t step)

{
  return topCell[0].stateMask | topCell[step].stateMask | topCell[rowStride - step].stateMask |
         topCell[rowStride + step].stateMask | topCell[rowStride * 2 - step].stateMask |
         topCell[rowStride * 2].stateMask;
}


/* True when one of the active grid mask classes 0..3 is absent from the neighbourhood mask. */
static Bool8 AiPlanningRebuild_LacksActiveMaskClass(uint32_t neighborhoodMask)

{
  return (g_AiActiveGridMaskClasses[0] & neighborhoodMask) == 0 ||
         (g_AiActiveGridMaskClasses[1] & neighborhoodMask) == 0 ||
         (g_AiActiveGridMaskClasses[2] & neighborhoodMask) == 0 ||
         (g_AiActiveGridMaskClasses[3] & neighborhoodMask) == 0;
}


/* True when one of the six field-grid neighbours of the cell below cellAbove (the two above, left, right, the two
   below) has none of the faction's presence bits. */
static Bool8 AiPlanningRebuild_HasUnoccupiedNeighbour(FieldGridCell *cellAbove,uint32_t gridWidth,
          FactionRuntimeIndex factionIndex)

{
  return (FIELD_CELL_OCCUPANCY_BYTE(&cellAbove[0],factionIndex) & FIELD_CELL_OCCUPANCY_PRESENCE_BITS) == 0 ||
         (FIELD_CELL_OCCUPANCY_BYTE(&cellAbove[1],factionIndex) & FIELD_CELL_OCCUPANCY_PRESENCE_BITS) == 0 ||
         (FIELD_CELL_OCCUPANCY_BYTE(&cellAbove[gridWidth - 1],factionIndex) & FIELD_CELL_OCCUPANCY_PRESENCE_BITS) ==
          0 ||
         (FIELD_CELL_OCCUPANCY_BYTE(&cellAbove[gridWidth + 1],factionIndex) & FIELD_CELL_OCCUPANCY_PRESENCE_BITS) ==
          0 ||
         (FIELD_CELL_OCCUPANCY_BYTE(&cellAbove[gridWidth * 2 - 1],factionIndex) &
          FIELD_CELL_OCCUPANCY_PRESENCE_BITS) == 0 ||
         (FIELD_CELL_OCCUPANCY_BYTE(&cellAbove[gridWidth * 2],factionIndex) & FIELD_CELL_OCCUPANCY_PRESENCE_BITS) ==
          0;
}


/* True when the cell below cellAbove or one of its six neighbours carries xenite/tritium support flags. */
static Bool8 AiPlanningRebuild_FootprintHasResourceSupport(const FieldGridCell *cellAbove,uint32_t gridWidth)

{
  return (cellAbove[gridWidth].flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) != 0 ||
         (cellAbove[0].flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) != 0 ||
         (cellAbove[1].flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) != 0 ||
         (cellAbove[gridWidth - 1].flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) != 0 ||
         (cellAbove[gridWidth + 1].flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) != 0 ||
         (cellAbove[gridWidth * 2 - 1].flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) != 0 ||
         (cellAbove[gridWidth * 2].flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) != 0;
}


/* Site tests of one field cell (the cell below cellAbove) against the scratch grid: scratchCell is one scratch row
   (scratchRowStride cells) above the cell's scratch state. Every test re-reads the grids, since the add calls may
   change them. */
static void AiPlanningRebuild_ScanSiteCell(FactionRuntimeIndex factionIndex,FieldGridCell *cellAbove,
          uint32_t gridWidth,const GridScratchCell *scratchCell,uint32_t scratchRowStride,
          uint32_t terrainFeatureSpacing)

{
  FieldGridCell *currentCell;
  uint32_t neighborhoodMask;

  currentCell = cellAbove + gridWidth;
  /* general site: an occupied, unblocked cell at the edge of the faction's presence where also one of the active
     mask classes is missing around it (both conditions, not either) */
  if (((scratchCell[scratchRowStride].stateMask & GRID_SCRATCH_BLOCKED) == 0) &&
      ((FIELD_CELL_OCCUPANCY_BYTE(currentCell,factionIndex) & FIELD_CELL_OCCUPANCY_PRESENCE_BITS) != 0) &&
      AiPlanningRebuild_HasUnoccupiedNeighbour(cellAbove,gridWidth,factionIndex) &&
      (AiPlanningRebuild_LacksActiveMaskClass
                 (scratchCell[scratchRowStride].stateMask |
                  AiPlanningRebuild_ScratchFootprintMask(scratchCell,scratchRowStride,3)))) {
    AiSiteCandidate_AddGeneralCellIfSeparated(currentCell);
  }
  /* flagged site: occupancy bit 4 set and one of the terrain classes 24/25/28 absent around it */
  if (((FIELD_CELL_OCCUPANCY_BYTE(currentCell,factionIndex) & 0x10) != 0) &&
      ((scratchCell[scratchRowStride].stateMask & (GRID_SCRATCH_BLOCKED | GRID_SCRATCH_LOW_BAND0)) == 0)) {
    neighborhoodMask = scratchCell[scratchRowStride].stateMask |
                       AiPlanningRebuild_ScratchFootprintMask(scratchCell,scratchRowStride,4);
    if (((neighborhoodMask & (GRID_SCRATCH_BLOCKED | GRID_SCRATCH_TERRAIN_CLASS_BIT24)) == 0 ||
         (neighborhoodMask & (GRID_SCRATCH_BLOCKED | GRID_SCRATCH_TERRAIN_CLASS_BIT25)) == 0 ||
         (neighborhoodMask & (GRID_SCRATCH_BLOCKED | GRID_SCRATCH_TERRAIN_CLASS_BIT28)) == 0) &&
        ((currentCell->flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) == 0)) {
      AiSiteCandidate_AddFlaggedCellIfSeparated(currentCell);
    }
  }
  if (((FIELD_CELL_OCCUPANCY_BYTE(currentCell,factionIndex) & FIELD_CELL_OCCUPANCY_PRESENCE_BITS) == 0) ||
      ((scratchCell[scratchRowStride].stateMask &
        (AI_SITE_SCRATCH_OBSTACLE_BITS | AI_SITE_SCRATCH_BANDS_CLASSES_0_TO_5)) != 0)) {
    return;
  }
  /* terrain-feature site: a resource cell with no obstacles in the small and the wide footprint */
  if (((scratchCell[scratchRowStride].stateMask & (AI_SITE_SCRATCH_OBSTACLE_BITS | GRID_SCRATCH_LOW_DISTANCE_BANDS))
       == 0) &&
      ((AiPlanningRebuild_ScratchFootprintMask(scratchCell,scratchRowStride,4) & AI_SITE_SCRATCH_OBSTACLE_BITS) ==
       0) &&
      ((AiPlanningRebuild_ScratchFootprintMask(scratchCell - scratchRowStride,scratchRowStride * 2,8) &
        AI_SITE_SCRATCH_OBSTACLE_BITS) == 0) &&
      ((currentCell->flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) != 0)) {
    AiSiteCandidate_AddTerrainFeatureCellIfSeparated(currentCell,terrainFeatureSpacing);
  }
  /* base site: no obstacles around it and no resource cell in its footprint */
  if (((AiPlanningRebuild_ScratchFootprintMask(scratchCell,scratchRowStride,4) & AI_SITE_SCRATCH_OBSTACLE_BITS) ==
       0) &&
      !AiPlanningRebuild_FootprintHasResourceSupport(cellAbove,gridWidth)) {
    AiBaseSiteWorkspace_AddCellInsideBase(currentCell);
  }
  /* large base site: additionally none of the terrain classes 28..30 in the wide footprint */
  if (((scratchCell[scratchRowStride].stateMask & (AI_SITE_SCRATCH_OBSTACLE_BITS | GRID_SCRATCH_LOW_DISTANCE_BANDS))
       == 0) &&
      ((AiPlanningRebuild_ScratchFootprintMask(scratchCell,scratchRowStride,4) & AI_SITE_SCRATCH_OBSTACLE_BITS) ==
       0) &&
      ((AiPlanningRebuild_ScratchFootprintMask(scratchCell - scratchRowStride,scratchRowStride * 2,8) &
        (GRID_SCRATCH_TERRAIN_CLASS_BIT30 | GRID_SCRATCH_TERRAIN_CLASS_BIT29 | GRID_SCRATCH_TERRAIN_CLASS_BIT28)) ==
       0) &&
      !AiPlanningRebuild_FootprintHasResourceSupport(cellAbove,gridWidth)) {
    AiBaseSiteWorkspace_AddLargeCellInsideBase(currentCell);
  }
}


/* Site scan over the field grid rows 1 .. height-2 (all columns), in step with the scratch grid (4 scratch cells
   per field cell, plus 3 * scratch width per row). */
static void AiPlanningRebuild_ScanFieldGridSites(FactionRuntimeIndex factionIndex,FieldGridAsset *terrainGrid)

{
  uint32_t scratchWidth;
  uint32_t gridWidth;
  int rowsRemaining;
  uint32_t columnsRemaining;
  FieldGridCell *cellAbove;
  GridScratchCell *scratchCell;
  uint32_t terrainFeatureSpacing;

  scratchWidth = g_GridScratchWidth;
  gridWidth = terrainGrid->gridWidth;
  rowsRemaining = terrainGrid->gridHeight - 2;
  cellAbove = terrainGrid->cells;
  scratchCell = g_GridScratchPrimary + g_GridScratchWidth * 2 + 2;
  terrainFeatureSpacing = g_GridScratchWidth * 24;
  AiPlanning_CollectActiveGridMaskClasses();
  /* Original quirk: the first row scans gridWidth columns, the later ones gridWidth & 0x1FFFFFF. */
  columnsRemaining = gridWidth;
  do {
    do {
      AiPlanningRebuild_ScanSiteCell(factionIndex,cellAbove,gridWidth,scratchCell,scratchWidth * 3,
                                     terrainFeatureSpacing);
      cellAbove++;
      scratchCell = scratchCell + 4;
      columnsRemaining--;
    } while (columnsRemaining != 0);
    scratchCell = scratchCell + g_GridScratchWidth * 3;
    columnsRemaining = gridWidth & FIELD_GRID_ROW_STRIDE_WIDTH_MASK;
    rowsRemaining--;
  } while (rowsRemaining != 0);
}


/* Production mask of the own structures: class 13 contributes its classParameterC4 mask, class 22 bit 3,
   class 11 bit 4. */
static uint32_t AiPlanningRebuild_CollectProductionMask(void)

{
  uint32_t productionMask;
  uint32_t entriesRemaining;
  AiStructureWorkspaceEntry *structureEntry;
  ModelDefinition *slotDefinition;

  productionMask = 0;
  structureEntry = g_AiWorkspace00Structures;
  for (entriesRemaining = g_AiWorkspace00Count; entriesRemaining != 0; entriesRemaining--) {
    if ((int *)structureEntry->runtimeSlotAddressOrZero != NULL) {
      slotDefinition = (ModelDefinition *)*(int *)structureEntry->runtimeSlotAddressOrZero;
      if (slotDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_13) {
        productionMask = productionMask | slotDefinition->classParameterC4;
      }
      else if (slotDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_22) {
        productionMask = productionMask | ARMY_ASSET_FLAG_BUILT_AT_AIRCRAFT_PAD;
      }
      else if (slotDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_11) {
        productionMask = productionMask | ARMY_ASSET_FLAG_BUILT_BY_CLASS11;
      }
    }
    structureEntry++;
  }
  return productionMask;
}


/* Workspace 11: registry assets that are enabled (bit 0), not technology-locked for the faction, producible
   (production mask) and not a building of ARM 300..339. */
static void AiPlanningRebuild_CollectProducibleAssets(FactionRuntimeIndex factionIndex,uint32_t productionMask)

{
  ArmyAssetRecordPrefix **armyAssetRegistryCursor;
  ArmyAssetRecordPrefix **workspace11Cursor;
  ArmyAssetRecordPrefix *definitionNode;
  int slotsRemaining;
  Bool8 technologyLocked;

  armyAssetRegistryCursor = g_ArmyAssetRecordRegistry;
  workspace11Cursor = g_AiWorkspace11ProducibleAssets;
  for (slotsRemaining = ARMY_ASSET_REGISTRY_SLOT_COUNT; slotsRemaining != 0; slotsRemaining--) {
    definitionNode = *armyAssetRegistryCursor;
    if ((definitionNode != NULL) && ((definitionNode[1].selectionDetailTemplateVariantIndex & 1) != 0)) {
      /* returns true while some technology of the hierarchy is still locked */
      technologyLocked = ModelDefinitionHierarchy_AllTechnologyUnlockedForFaction
                               (factionIndex,(ModelDefinitionHierarchyNodeAddress32)definitionNode);
      if (!technologyLocked && ((definitionNode[1].selectionDetailTemplateVariantIndex & productionMask) != 0) &&
          ((definitionNode->registryId < ARM_0300_BUILDING_MDL0301) ||
           ((ARM_0340_BUILDING_MDL0314 - 1) < definitionNode->registryId)) &&
          (g_AiWorkspace11Count < AI_WORKSPACE11_CAPACITY)) {
        *workspace11Cursor = definitionNode;
        g_AiWorkspace11Count++;
        workspace11Cursor++;
      }
    }
    armyAssetRegistryCursor++;
  }
}


/* Workspace 07: the targets of workspace 02 with their model node and world position. */
static void AiPlanningRebuild_CollectTargets(void)

{
  AiRuntimeWorkspaceEntry *hostileEntry;
  AiTargetWorkspaceEntry *targetEntry;
  uint32_t hostilesRemaining;
  ModelRuntimeSlot *modelRuntime;
  ModelRuntimeNode *modelNode;
  GraphicsWorldCoordinateQ12 translationX;
  GraphicsWorldCoordinateQ12 translationY;

  hostileEntry = g_AiWorkspace02VisibleHostiles;
  targetEntry = g_AiWorkspace07Targets;
  for (hostilesRemaining = g_AiWorkspace02Count;
       (hostilesRemaining != 0) && (g_AiWorkspace07Count < AI_WORKSPACE07_CAPACITY); hostilesRemaining--) {
    modelRuntime = hostileEntry->modelRuntime;
    if (modelRuntime != NULL) {
      targetEntry->modelRuntime = modelRuntime;
      modelNode = modelRuntime->rootModelNodeOrSavedOffset.modelNode;
      translationX = modelNode->worldTransform.translation.x;
      translationY = modelNode->worldTransform.translation.y;
      targetEntry->modelNode = modelNode;
      targetEntry->worldXQ12 = translationX;
      targetEntry->worldYQ12 = translationY;
      g_AiWorkspace07Count++;
      targetEntry++;
    }
    hostileEntry++;
  }
}


/* Technology candidates: researchTechnologyIds[28] down to [1] of each own structure's definition. */
static void AiPlanningRebuild_CollectResearchCandidates(FactionRuntimeIndex factionIndex)

{
  FactionRuntimeRecordByteOffset factionRecordOffset;
  AiStructureWorkspaceEntry *structureEntry;
  uint32_t entriesRemaining;
  ModelRuntimeSlot *modelRuntime;
  ModelDefinition *slotDefinition;
  uint32_t technologySlot;
  Bool8 notAvailable;

  factionRecordOffset = factionIndex * (int)sizeof(GameFactionRuntimeRecord);
  structureEntry = g_AiWorkspace00Structures;
  for (entriesRemaining = g_AiWorkspace00Count; entriesRemaining != 0; entriesRemaining--) {
    modelRuntime = (ModelRuntimeSlot *)structureEntry->runtimeSlotAddressOrZero;
    if (modelRuntime != NULL) {
      slotDefinition = modelRuntime->definitionOrSavedId.runtimeDefinition;
      for (technologySlot = 28; technologySlot != 0; technologySlot--) {
        /* returns false when the AI may plan this technology */
        notAvailable = AiTechnologyCandidate_IsCurrentlyAvailable
                             ((PckTechnologyIdCatalog)slotDefinition->researchTechnologyIds[technologySlot],
                              factionRecordOffset);
        if (!notAvailable) {
          AiTechnologyPlanning_AddCandidateRecord
                    (modelRuntime,(PckTechnologyIdCatalog)slotDefinition->researchTechnologyIds[technologySlot]);
        }
      }
    }
    structureEntry++;
  }
}


/* Rebuilds the faction's AI workspaces at the start of a planning pass:
   - world entities: own units (ARM < 300) into 01, own structures into 00 (ARM 300 also remembered); entities
     of factions whose capability bit for us is clear into 03 or 02, depending on their visibility bits;
   - the faction's pending army assets as unassigned 00/01 entries (the primary list also as requests in 04),
     plus the assets that own class 0x0B/0x16/0x0D structures are producing;
   - a scan of the field grid for general, flagged and terrain-feature sites and the 09/10 entity candidates;
   - 11: every registry asset the faction's structures can produce with all technology unlocked (ARM < 300 or
     >= 340); 07: the targets from 02 with their positions; 12: the technologies of the own structures'
     research slots that are currently available.
   Capacities: 00 128, 01 64, 02 128, 03 512, 04 8, 07 64, 11 1024 entries.
*/
void AiPlanning_RebuildFactionWorkspaces(AiPlanningPhaseIndex planningPhaseDispatchIndex,
          FactionRuntimeIndex factionRuntimeIndexRegisterCopy,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime)

{
  g_AiWorkspace00Count = 0;
  g_AiWorkspace04Count = 0;
  g_AiWorkspace01Count = 0;
  g_AiWorkspace02Count = 0;
  g_AiWorkspace03Count = 0;
  g_AiWorkspace05Count = 0;
  g_AiWorkspace06Count = 0;
  g_AiWorkspace07Count = 0;
  g_AiWorkspace08Count = 0;
  g_AiWorkspace09Count = 0;
  g_AiWorkspace10Count = 0;
  g_AiWorkspace11Count = 0;
  g_AiWorkspace12Count = 0;
  g_AiWorkspaceOwnedAsset300Runtime = NULL;
  /* Original quirk: the pending and in-production assets are only added when the world owner list is not
     empty. */
  if (worldRuntime->ownerListHead != NULL) {
    AiPlanningRebuild_CollectWorldEntities(factionIndex,worldRuntime->ownerListHead);
    AiPlanningRebuild_AddPendingArmyAssets(factionIndex);
    AiPlanningRebuild_AddAssetsInProduction();
  }
  AiPlanningRebuild_ScanFieldGridSites(factionIndex,worldRuntime->fieldGrid);
  AiPlanningRebuild_CollectProducibleAssets(factionIndex,AiPlanningRebuild_CollectProductionMask());
  AiPlanningRebuild_CollectTargets();
  AiPlanningRebuild_CollectResearchCandidates(factionIndex);
  return;
}


/* Research planning: once the faction has an ARM 330 (0x14A) structure, scores every available technology of
   workspace 12 with the score callback of its kind and proposes the best one (entry kind 2) with
   workspace12BestCandidateBaseWeight, halved while the faction's primary anchor cooldown runs.
*/
void AiTechnologyCandidate_AddBestResearch(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  AiTechnologyCandidateScore candidateScore;
  int wordIndex;
  AiTechnologyCandidateScore bestScore;
  AiTechnologyPlanningCandidateCount candidatesRemaining;
  AiTechnologyPlanningCandidate *candidateCursor;
  uint32_t weightRange;
  RuntimeToken entityId;

  if (!AiPrimaryWorkspace_HasEntryById(ARM_0330_BUILDING_MDL0303)) {
    return;
  }
  /* Category mask for the category score callback: bit 2 / bit 4 when the faction owns any
     technology of category 2 / 3. */
  g_AiTechnologyScoreCategoryMask = 0;
  for (wordIndex = 0; wordIndex < 8; wordIndex++) {
    uint32_t owned = g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits[wordIndex];
    if ((g_TechnologyCategoryMasks.category2[wordIndex] & owned) != 0) {
      g_AiTechnologyScoreCategoryMask = g_AiTechnologyScoreCategoryMask | 2;
    }
    if ((g_TechnologyCategoryMasks.category3[wordIndex] & owned) != 0) {
      g_AiTechnologyScoreCategoryMask = g_AiTechnologyScoreCategoryMask | 4;
    }
  }
  if (g_AiWorkspace12Count == 0) {
    return;
  }
  bestScore = 0;
  weightRange = g_AiKnowledgeData->parameters.workspace12BestCandidateBaseWeight;
  entityId = 0;
  candidateCursor = g_AiWorkspace12TechnologyCandidates;
  for (candidatesRemaining = g_AiWorkspace12Count; candidatesRemaining != 0; candidatesRemaining--) {
    candidateScore = g_AiTechnologyCandidateScoreCallbackTable[candidateCursor->scoreKind08]
                      (factionIndex,candidateCursor->technologyId00,worldRuntime);
    if (bestScore < candidateScore) {
      entityId = candidateCursor->technologyId00;
      bestScore = candidateScore;
    }
    candidateCursor++;
  }
  if (bestScore != 0) {
    if (g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorCooldown != 0) {
      weightRange = weightRange >> 1;
    }
    AiCandidateWorkspace_AddOrAccumulateWeightedEntry(entityId,weightRange,2 /* technology */);
  }
}


/* Empties the AI candidate workspace (workspace 13) by resetting its entry count.
*/
void AiCandidateWorkspace_Clear(void)

{
  g_AiCandidateWorkspaceEntryCount = 0;
  return;
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
  return;
}


/* Sorts the AI candidate workspace (workspace 13) by descending weightedScoreAndKind (signed compare), so the
   purchase planner tries the best candidates first. Selection sort: each pass swaps every higher entry into the
   pass's first slot, carrying the id/multiplicity dword along.
*/
void AiCandidateWorkspace_SortDescending(void)

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
      do {
        if (currentRecordScore < (int)scanRecordCursor->weightedScoreAndKind) {
          /* the original swaps with an atomic (bus-locked) exchange, shown as LOCK/UNLOCK */
          LOCK();
          promotedScore = scanRecordCursor->weightedScoreAndKind;
          scanRecordCursor->weightedScoreAndKind = currentRecordScore;
          UNLOCK();
          LOCK();
          promotedPayload = scanRecordCursor->entityIdAndMultiplicity;
          scanRecordCursor->entityIdAndMultiplicity = currentRecordPayload;
          UNLOCK();
          currentRecordCursor->weightedScoreAndKind = promotedScore;
          currentRecordCursor->entityIdAndMultiplicity = promotedPayload;
          currentRecordScore = promotedScore;
          currentRecordPayload = promotedPayload;
        }
        scanRecordCursor++;
        comparisonsRemaining--;
      } while (comparisonsRemaining != 0);
      currentRecordScore = currentRecordCursor[1].weightedScoreAndKind;
      currentRecordPayload = currentRecordCursor[1].entityIdAndMultiplicity;
      comparisonsRemaining = recordsInCurrentPass - 2;
      scanRecordCursor = currentRecordCursor + 2;
      recordsInCurrentPass--;
      currentRecordCursor++;
    } while (comparisonsRemaining != 0);
  }
  return;
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
    if (workspaceEntryCursor->modelRuntime != NULL) {
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
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  ModelRuntimeSlot *slotModelRuntime;
  
  minimumActiveManhattanDistanceQ12 = INT32_MAX;
  workspaceEntryCursor = (AiRuntimeWorkspaceEntry *)g_AiWorkspace00Structures;
  for (workspaceEntriesRemaining = g_AiWorkspace00Count; workspaceEntriesRemaining != 0;
      workspaceEntriesRemaining--) {
    slotModelRuntime = workspaceEntryCursor->modelRuntime;
    if ((slotModelRuntime != NULL) &&
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
    if (workspaceEntryCursor->modelRuntime != NULL) {
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
    if (workspaceEntryCursor->modelRuntime != NULL) {
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
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  ModelRuntimeNode *modelNode;
  
  minimumManhattanDistanceQ12 = INT32_MAX;
  workspaceEntryCursor = (AiRuntimeWorkspaceEntry *)g_AiWorkspace00Structures;
  for (workspaceEntriesRemaining = g_AiWorkspace00Count; workspaceEntriesRemaining != 0;
      workspaceEntriesRemaining--) {
    if (workspaceEntryCursor->modelRuntime != NULL) {
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


/* Builds a pending resource structure (ARM_0330/ARM_0332) of the AI faction: at the first workspace-08 site of
   this asset where the mode-0 placement test passes it creates the structure with the site's heading, rebuilds
   its model transforms, dispatches its class command, starts the effect referenced by its model runtime and
   removes the asset from the faction's pending list. Nothing happens when no site passes.
*/
void AiConstructionPlanner_PlaceSpecialAssetFromWorkspace
          (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime)

{
  FieldGridCell *workspaceRecord;
  ArmyRuntimeSlot *modelNodeRuntime;
  ArmyRuntimeSlot *primarySlot;
  ModelRuntimeSlot *createdModelRuntime;
  ArmyRuntimeSlot **createdSlotPair;
  int recordsRemaining;
  AiTerrainFeatureWorkspaceEntry *terrainFeatureEntry;

  terrainFeatureEntry = g_AiWorkspace08TerrainFeatureSites;
  for (recordsRemaining = g_AiWorkspace08Count; recordsRemaining != 0; recordsRemaining--, terrainFeatureEntry++) {
    if (armyAssetId != terrainFeatureEntry->armyAssetId) {
      continue;
    }
    workspaceRecord = terrainFeatureEntry->cell;
    if (AiPlacement_TestWorkspaceRecordAtPoint(armyAssetId,workspaceRecord,factionIndex,(UiRootNode *)worldRuntime)) {
      continue; /* placement rejected */
    }
    createdSlotPair = (ArmyRuntimeSlot **)ArmyRuntime_CreateInstanceFromAsset
                      (ARMY_CREATE_UNLOCK_TECHNOLOGY,(uint32_t)(uint16_t)workspaceRecord->triangle0NormalAngles,
                       workspaceRecord->worldY,workspaceRecord->worldX,factionIndex,armyAssetId,
                       worldRuntime,NULL);
    if (createdSlotPair == NULL) {
      return;
    }
    /* the create result points at the pair {army slot, model node}; the node is typed as a slot here, so
       the effect arguments below are its fields under ArmyRuntimeSlot names */
    modelNodeRuntime = createdSlotPair[1];
    primarySlot = *createdSlotPair;
    modelNodeRuntime->movementPosition0Q12 = 0;
    createdModelRuntime = (primarySlot->modelRuntimeOrSavedOffset).modelRuntime;
    ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)modelNodeRuntime);
    ArmyRuntime_DispatchClassCommand((ArmyRuntimeSlot *)createdSlotPair,worldRuntime); /* the created army */
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
    return;
  }
}


/* Technology score callback for score kind 0 (g_AiTechnologyCandidateScoreCallbackTable[0],
   called by AiTechnologyCandidate_AddBestResearch): a technology of this kind always scores 0, so it
   is never chosen for research.
*/
AiTechnologyCandidateScore AiTechnologyScore_AlwaysZero
          (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId,
          WorldRuntimeContext *worldRuntime)

{
  return 0;
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

  allocError = g_MemoryApi.alloc(AI_WORKSPACE00_CAPACITY * sizeof(AiStructureWorkspaceEntry),
                                 (void **)&g_AiWorkspace00Structures);
  if (allocError == 0) {
    allocError = g_MemoryApi.alloc(AI_WORKSPACE01_CAPACITY * sizeof(AiRuntimeWorkspaceEntry),
                                   (void **)&g_AiWorkspace01Units);
    if (allocError == 0) {
      allocError = g_MemoryApi.alloc(AI_WORKSPACE02_CAPACITY * sizeof(AiRuntimeWorkspaceEntry),
                                     (void **)&g_AiWorkspace02VisibleHostiles);
      if (allocError == 0) {
        allocError = g_MemoryApi.alloc(AI_WORKSPACE03_CAPACITY * sizeof(AiRuntimeWorkspaceEntry),
                                       (void **)&g_AiWorkspace03UnseenHostiles);
        if (allocError == 0) {
          allocError = g_MemoryApi.alloc(AI_WORKSPACE04_CAPACITY * sizeof(AiRuntimeWorkspaceEntry),
                                         (void **)&g_AiWorkspace04RequestedAssets);
          if (allocError == 0) {
            allocError = g_MemoryApi.alloc(AI_WORKSPACE05_CAPACITY * sizeof(AiScoredSiteWorkspaceEntry),
                                           (void **)&g_AiWorkspace05GeneralSites);
            if (allocError == 0) {
              allocError = g_MemoryApi.alloc(AI_WORKSPACE06_CAPACITY * sizeof(AiScoredSiteWorkspaceEntry),
                                             (void **)&g_AiWorkspace06FlaggedSites);
              if (allocError == 0) {
                allocError = g_MemoryApi.alloc(AI_WORKSPACE07_CAPACITY * sizeof(AiTargetWorkspaceEntry),
                                               (void **)&g_AiWorkspace07Targets);
                if (allocError == 0) {
                  allocError = g_MemoryApi.alloc(AI_WORKSPACE08_CAPACITY * sizeof(AiTerrainFeatureWorkspaceEntry),
                                                 (void **)&g_AiWorkspace08TerrainFeatureSites);
                  if (allocError == 0) {
                    allocError = g_MemoryApi.alloc(AI_WORKSPACE09_CAPACITY * sizeof(FieldGridCell *),
                                                   (void **)&g_AiWorkspace09Cells);
                    if (allocError == 0) {
                      allocError = g_MemoryApi.alloc(AI_WORKSPACE10_CAPACITY * sizeof(FieldGridCell *),
                                                     (void **)&g_AiWorkspace10Cells);
                      if (allocError == 0) {
                        allocError = g_MemoryApi.alloc(AI_WORKSPACE11_CAPACITY * sizeof(ArmyAssetRecordPrefix *),
                                                       (void **)&g_AiWorkspace11ProducibleAssets);
                        if (allocError == 0) {
                          allocError = g_MemoryApi.alloc(AI_WORKSPACE12_CAPACITY * sizeof(AiTechnologyPlanningCandidate),
                                                         (void **)&g_AiWorkspace12TechnologyCandidates);
                          if (allocError == 0) {
                            allocError = g_MemoryApi.alloc(AI_CANDIDATE_WORKSPACE_CAPACITY * sizeof(AiCandidateWorkspaceEntry),
                                                           (void **)&g_AiWorkspace13Candidates);
                            if (allocError == 0) {
                              allocError = g_MemoryApi.alloc(AI_WORKSPACE14_CAPACITY * sizeof(ArmyRuntimeSlot *),
                                                             (void **)&g_AiWorkspace14CollectedArmies);
                              if (allocError == 0) {
                                knowledgeDataImage = Package_LoadEntry((uint16_t *)u_engine_ki_dat_0053c5e4,&loadErrorCode);
                                if (knowledgeDataImage != NULL) {
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
  return;
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
  return;
}


/* Returns true when the primary workspace (workspace 00) holds an entry of this army asset whose
   runtime pointer is NULL.
*/
Bool8 AiPrimaryWorkspace_HasUnassignedEntryById(PckArmyAssetIdCatalog entryId)

{
  int workspaceEntriesRemaining;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  
  workspaceEntriesRemaining = g_AiWorkspace00Count;
  workspaceEntryCursor = (AiRuntimeWorkspaceEntry *)g_AiWorkspace00Structures;
  for (; workspaceEntriesRemaining != 0; workspaceEntriesRemaining--) {
    if ((entryId == workspaceEntryCursor->armyAssetId) &&
       (workspaceEntryCursor->modelRuntime == NULL)) {
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
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;

  workspaceEntriesRemaining = g_AiWorkspace00Count;
  workspaceEntryCursor = (AiRuntimeWorkspaceEntry *)g_AiWorkspace00Structures;
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
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;

  matchingAssignedEntryCount = 0;
  workspaceEntryCursor = (AiRuntimeWorkspaceEntry *)g_AiWorkspace00Structures;
  for (workspaceEntriesRemaining = g_AiWorkspace00Count; workspaceEntriesRemaining != 0;
      workspaceEntriesRemaining--) {
    if ((workspaceEntryCursor->modelRuntime != NULL) &&
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
  return;
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
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  ModelRuntimeSlot *entryModelRuntime;
  
  workspaceEntryCursor = (AiRuntimeWorkspaceEntry *)g_AiWorkspace00Structures;
  for (workspaceEntriesRemaining = g_AiWorkspace00Count; workspaceEntriesRemaining != 0;
      workspaceEntriesRemaining--, workspaceEntryCursor++) {
    entryModelRuntime = workspaceEntryCursor->modelRuntime;
    if (entryModelRuntime != NULL) {
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

