/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/ai/perception.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/ai/perception.h>
#include <thandor/thandor.h>

/* Implementation ownership: gameplay/ai/perception. */

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
    modelRuntime = (ModelRuntimeSlot *)worldNode->runtimePayload;
    entityRuntime = modelRuntime->ownerArmyRuntimeOrSavedOffset.entityRuntime;
    if (factionIndex == entityRuntime->common.ownership.ownerIndex) {
      assetId = entityRuntime->common.runtimeIdentityOrArmyAssetId;
      if (assetId < ARM_0300_BUILDING_MDL0301) {
        AiPlanningRebuild_AddRuntimeEntry(g_AiWorkspace01Units,&g_AiWorkspace01Count,AI_WORKSPACE01_CAPACITY,
                                          modelRuntime,assetId);
      }
      else if (g_AiWorkspace00Count < AI_WORKSPACE00_CAPACITY) {
        g_AiWorkspace00Structures[g_AiWorkspace00Count].runtimeSlotAddressOrZero = modelRuntime;
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
    assetId = *Thandor_U32ToPointer<PckArmyAssetIdCatalog>(*armyAssetPointerCursor + 8); /* 5f-format: GameFactionRuntimeRecord.primary/secondaryArmyAssetPointersOrIds */
    AiPlanningRebuild_AddUnassignedStructureEntry(assetId);
    AiPlanningRebuild_AddRuntimeEntry(g_AiWorkspace04RequestedAssets,&g_AiWorkspace04Count,
                                      AI_WORKSPACE04_CAPACITY,NULL,assetId);
    armyAssetPointerCursor++;
  }
  armyAssetPointerCursor = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetPointersOrIds;
  for (armyAssetsRemaining = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount;
       armyAssetsRemaining != 0; armyAssetsRemaining--) {
    assetId = *Thandor_U32ToPointer<PckArmyAssetIdCatalog>(*armyAssetPointerCursor + 8); /* 5f-format: GameFactionRuntimeRecord.primary/secondaryArmyAssetPointersOrIds */
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
      slotDefinition = ((ModelRuntimeSlot *)slotWords)->definitionOrSavedId.runtimeDefinition;
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
    if (structureEntry->runtimeSlotAddressOrZero != NULL) {
      slotDefinition = structureEntry->runtimeSlotAddressOrZero->definitionOrSavedId.runtimeDefinition;
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
