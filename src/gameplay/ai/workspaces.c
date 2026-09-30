/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/ai/workspaces.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/ai/workspaces.h>
#include <thandor/thandor.h>

/* Implementation ownership: gameplay/ai/workspaces. */

/* Byte offsets of the site scan in AiPlanning_RebuildFactionWorkspaces, which walks the field grid through a
   byte cursor at the faction's occupancy byte: one cell, and the cell's occupancy mask */
#define AI_SCAN_CELL_BYTES ((int)sizeof(FieldGridCell))
#define AI_SCAN_OCCUPANCY_OFFSET ((int)offsetof(FieldGridCell, occupancyMask))

/* Candidate cache of the faction runtime record at factionImageByteOffset (faction * 0x740) */
#define AI_FACTION_CANDIDATE_CACHE(factionImageByteOffset) \
  ((AiFactionCandidateCacheState *)((uint8_t *)&g_GameFactionRuntimeImage.records[0].candidateCache + \
                                    (factionImageByteOffset)))

/* Address: 0x0053A1E0.
   Proposes armyAssetId at the first workspace 08 site of that asset where it can be placed (placement mode 4),
   unless one of it is still unassigned. Weight: 3 * baseWeight / (existing count + 3); for assets other than
   ARM 330 (0x14A) additionally scaled by (2 * unpowered + supplied Energy demand) / (record +0x358 rate << 4)
   when that rate is nonzero.
*/
void AiWorkspaceAssetCandidate_AddWeightedEntry(AiCandidateScore32 baseWeight,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  int remainingOrCountOrRate;
  uint32_t weightRange;
  AiTerrainFeatureWorkspaceEntry *terrainFeatureEntry;
  bool testResult;
  
  remainingOrCountOrRate = g_AiWorkspace08Count;
  terrainFeatureEntry = g_AiWorkspace08TerrainFeatureSites;
  if ((g_AiWorkspace08Count != 0) &&
     (testResult = AiPrimaryWorkspace_HasUnassignedEntryById(armyAssetId), !testResult)) {
    do {
      if ((armyAssetId == terrainFeatureEntry->armyAssetId) &&
         (testResult = AiPlacement_TestMode4AtWorkspaceRecord
                            (armyAssetId,terrainFeatureEntry->cell,factionIndex,worldRuntime),
         !testResult)) {
        remainingOrCountOrRate = AiPrimaryWorkspace_CountAssignedEntriesById(armyAssetId);
        weightRange = (uint32_t)(baseWeight * 3) / (remainingOrCountOrRate + 3U);
        if (armyAssetId != ARM_0330_BUILDING_MDL0303) {
          remainingOrCountOrRate = g_GameFactionRuntimeImage.records[factionIndex].tritiumExtractionRateQ4PerTick <<
                  4;
          if (remainingOrCountOrRate != 0) {
            weightRange = (uint32_t)(((int64_t)(int)weightRange *
                                  (int64_t)
                                  (int)(g_GameFactionRuntimeImage.records[factionIndex].
                                        unpoweredEnergyDemandQ4 * 2 +
                                       g_GameFactionRuntimeImage.records[factionIndex].
                                       suppliedEnergyDemandQ4)) / (int64_t)remainingOrCountOrRate);
          }
        }
        AiCandidateWorkspace_AddOrAccumulateWeightedEntry(armyAssetId,weightRange,1);
        return;
      }
      terrainFeatureEntry++;
      remainingOrCountOrRate--;
    } while (remainingOrCountOrRate != 0);
  }
  return;
}


/* Address: 0x00538230.
   Rebuilds the faction's AI workspaces at the start of a planning pass:
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
  GameEntityRuntime *entityRuntime;
  PckArmyAssetIdCatalog assetId;
  int *primarySlotWords;
  FieldGridAsset *terrainGrid;
  ArmyAssetRecordPrefix *definitionNode;
  GraphicsWorldCoordinateQ12 translationX;
  GraphicsWorldCoordinateQ12 translationY;
  ModelDefinition *slotDefinition;
  AiStructureWorkspaceEntry *primaryBuffer;
  uint32_t countSnapshotOrRemaining;
  AiRuntimeWorkspaceEntry *workspace03Buffer;
  GridScratchStateMask neighborhoodMask;
  FactionArmyAssetCount armyAssetsRemaining;
  int remainingOrClassRecord;
  uint32_t widthOrCount;
  uint32_t spacingOrPanelIndex;
  uint32_t countOrMask;
  uint32_t scratchWidthOrFactionOffset;
  int rowStrideOrClassId;
  AiStructureWorkspaceEntry *primaryEntryCursor;
  FieldGridCell *fieldCell;
  uint8_t *cellByteCursor;
  ArmyAssetRecordPrefix **workspace11Cursor;
  WorldOwnerListNode *worldNode;
  uint32_t *armyAssetPointerCursor;
  GridScratchCell *scratchCellOrigin;
  GridScratchCell *scratchCell;
  ArmyAssetRecordPrefix **armyAssetRegistryCursor;
  AiTargetWorkspaceEntry *targetEntry;
  bool testResult;
  AiTechnologyPlanningLoopRegisterContinuityResult registerContinuity;
  ModelRuntimeSlot *slotModelRuntime;
  ModelRuntimeNode *modelNode;
  AiRuntimeWorkspaceEntry *runtimeEntryCursor;
  
  g_AiWorkspace00Count = 0;
  g_AiWorkspace04Count = 0;
  worldNode = worldRuntime->ownerListHead;
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
  if (worldNode != NULL) {
    do {
      widthOrCount = g_AiWorkspace01Count;
      runtimeEntryCursor = g_AiWorkspace01Units;
      countOrMask = g_AiWorkspace00Count;
      primaryEntryCursor = g_AiWorkspace00Structures;
      if (worldNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
        /* the node's model runtime; entityRuntime is its owning army */
        slotModelRuntime = worldNode->runtimePayload;
        entityRuntime = slotModelRuntime->ownerArmyRuntimeOrSavedOffset.entityRuntime;
        if (factionIndex == entityRuntime->common.ownership.ownerIndex) {
          assetId = entityRuntime->common.runtimeIdentityOrArmyAssetId;
          if (assetId < ARM_0300_BUILDING_MDL0301) {
            if (g_AiWorkspace01Count < AI_WORKSPACE01_CAPACITY) {
              g_AiWorkspace01Units[g_AiWorkspace01Count].modelRuntime = slotModelRuntime;
              runtimeEntryCursor[widthOrCount].armyAssetId = assetId;
              g_AiWorkspace01Count++;
            }
          }
          else if (g_AiWorkspace00Count < AI_WORKSPACE00_CAPACITY) {
            g_AiWorkspace00Structures[g_AiWorkspace00Count].runtimeSlotAddressOrZero =
                 (AiWorkspaceRuntimeSlotAddress32)slotModelRuntime;
            primaryEntryCursor[countOrMask].armyAssetId = assetId;
            g_AiWorkspace00Count++;
            if (assetId == ARM_0300_BUILDING_MDL0301) {
              g_AiWorkspaceOwnedAsset300Runtime = slotModelRuntime;
            }
          }
        }
        else {
          /* two bits per faction: how this faction sees the foreign entity */
          countOrMask = (entityRuntime->common).damageState.factionVisibilityBits1C >>
                   ((char)factionIndex * 2 & 31U);
          if (((entityRuntime->common).ownership.ownerIndex != 0) &&
             (testResult = GameFactionRuntime_TestCapabilityBitClear
                                 ((entityRuntime->common).ownership.ownerIndex,factionIndex),
             countSnapshotOrRemaining = g_AiWorkspace03Count, workspace03Buffer = g_AiWorkspace03UnseenHostiles,
             widthOrCount = g_AiWorkspace02Count, runtimeEntryCursor = g_AiWorkspace02VisibleHostiles
             , testResult)) {
            if (((countOrMask & 2) == 0) &&
               (((countOrMask & 1) == 0 ||
                (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand
                 [((ModelRuntimeSlot *)(entityRuntime->common).ownership.definitionOrClassRecord)->definitionOrSavedId.
                  runtimeDefinition->runtimeClassId] !=
                 ArmyRuntime_ClassCommandHandlerGroupA)))) {
              assetId = (entityRuntime->common).runtimeIdentityOrArmyAssetId;
              if (g_AiWorkspace03Count < AI_WORKSPACE03_CAPACITY) {
                g_AiWorkspace03UnseenHostiles[g_AiWorkspace03Count].modelRuntime = slotModelRuntime;
                workspace03Buffer[countSnapshotOrRemaining].armyAssetId = assetId;
                g_AiWorkspace03Count++;
              }
            }
            else {
              assetId = (entityRuntime->common).runtimeIdentityOrArmyAssetId;
              if (g_AiWorkspace02Count < AI_WORKSPACE02_CAPACITY) {
                g_AiWorkspace02VisibleHostiles[g_AiWorkspace02Count].modelRuntime = slotModelRuntime;
                runtimeEntryCursor[widthOrCount].armyAssetId = assetId;
                g_AiWorkspace02Count++;
              }
            }
          }
        }
      }
      worldNode = worldNode->nextNode;
    } while (worldNode != NULL);
    armyAssetPointerCursor = g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetPointersOrIds;
    primaryEntryCursor = g_AiWorkspace00Structures;
    countOrMask = g_AiWorkspace00Count;
    /* pending (not yet built) army assets: unassigned entries, the primary list also as requests */
    for (armyAssetsRemaining =
         g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount; armyAssetsRemaining != 0
        ; armyAssetsRemaining--) {
      assetId = *(PckArmyAssetIdCatalog *)(*armyAssetPointerCursor + 8);
      g_AiWorkspace00Structures = primaryEntryCursor;
      g_AiWorkspace00Count = countOrMask;
      if (countOrMask < AI_WORKSPACE00_CAPACITY) {
        primaryEntryCursor[countOrMask].runtimeSlotAddressOrZero = 0;
        primaryEntryCursor[countOrMask].armyAssetId = assetId;
        g_AiWorkspace00Count++;
      }
      countOrMask = g_AiWorkspace04Count;
      runtimeEntryCursor = g_AiWorkspace04RequestedAssets;
      if (g_AiWorkspace04Count < AI_WORKSPACE04_CAPACITY) {
        g_AiWorkspace04RequestedAssets[g_AiWorkspace04Count].modelRuntime = NULL;
        runtimeEntryCursor[countOrMask].armyAssetId = assetId;
        g_AiWorkspace04Count++;
      }
      armyAssetPointerCursor++;
      primaryEntryCursor = g_AiWorkspace00Structures;
      countOrMask = g_AiWorkspace00Count;
    }
    armyAssetPointerCursor = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetPointersOrIds;
    runtimeEntryCursor = g_AiWorkspace01Units;
    widthOrCount = g_AiWorkspace01Count;
    for (armyAssetsRemaining = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount;
        primaryBuffer = primaryEntryCursor, countSnapshotOrRemaining = countOrMask,
        armyAssetsRemaining != 0; armyAssetsRemaining--) {
      assetId = *(PckArmyAssetIdCatalog *)(*armyAssetPointerCursor + 8);
      g_AiWorkspace00Structures = primaryEntryCursor;
      g_AiWorkspace00Count = countOrMask;
      g_AiWorkspace01Units = runtimeEntryCursor;
      g_AiWorkspace01Count = widthOrCount;
      if (assetId < ARM_0300_BUILDING_MDL0301) {
        if (widthOrCount < AI_WORKSPACE01_CAPACITY) {
          runtimeEntryCursor[widthOrCount].modelRuntime = NULL;
          runtimeEntryCursor[widthOrCount].armyAssetId = assetId;
          g_AiWorkspace01Count++;
        }
      }
      else if (countOrMask < AI_WORKSPACE00_CAPACITY) {
        primaryEntryCursor[countOrMask].runtimeSlotAddressOrZero = 0;
        primaryEntryCursor[countOrMask].armyAssetId = assetId;
        g_AiWorkspace00Count++;
      }
      armyAssetPointerCursor++;
      primaryEntryCursor = g_AiWorkspace00Structures;
      countOrMask = g_AiWorkspace00Count;
      runtimeEntryCursor = g_AiWorkspace01Units;
      widthOrCount = g_AiWorkspace01Count;
    }
    for (; g_AiWorkspace00Structures = primaryBuffer, g_AiWorkspace00Count = countOrMask,
        g_AiWorkspace01Units = runtimeEntryCursor, g_AiWorkspace01Count = widthOrCount,
        countSnapshotOrRemaining != 0; countSnapshotOrRemaining--) {
      /* own structures of class 11 / 22 / 13: the asset in production (slot word 24) counts as an
         unassigned entry while their state word (46 resp. 43) is 1 */
      primarySlotWords = (int *)primaryEntryCursor->runtimeSlotAddressOrZero;
      if (primarySlotWords != NULL) {
        remainingOrClassRecord = *primarySlotWords;
        if (((ModelDefinition *)remainingOrClassRecord)->runtimeClassId == MODEL_RUNTIME_CLASS_11) {
          assetId = primarySlotWords[24];
          if ((primarySlotWords[46] == 1) && (countOrMask < AI_WORKSPACE00_CAPACITY)) {
            primaryBuffer[countOrMask].runtimeSlotAddressOrZero = 0;
            primaryBuffer[countOrMask].armyAssetId = assetId;
            g_AiWorkspace00Count++;
          }
        }
        else if (((ModelDefinition *)remainingOrClassRecord)->runtimeClassId == MODEL_RUNTIME_CLASS_22) {
          assetId = primarySlotWords[24];
          if ((primarySlotWords[43] == 1) && (widthOrCount < AI_WORKSPACE01_CAPACITY)) {
            runtimeEntryCursor[widthOrCount].modelRuntime = NULL;
            runtimeEntryCursor[widthOrCount].armyAssetId = assetId;
            g_AiWorkspace01Count++;
          }
        }
        else if (((((ModelDefinition *)remainingOrClassRecord)->runtimeClassId == MODEL_RUNTIME_CLASS_13) &&
                 (assetId = primarySlotWords[24], primarySlotWords[46] == 1)) &&
                (widthOrCount < AI_WORKSPACE01_CAPACITY)) {
          runtimeEntryCursor[widthOrCount].modelRuntime = NULL;
          runtimeEntryCursor[widthOrCount].armyAssetId = assetId;
          g_AiWorkspace01Count++;
        }
      }
      primaryEntryCursor++;
      primaryBuffer = g_AiWorkspace00Structures;
      countOrMask = g_AiWorkspace00Count;
      runtimeEntryCursor = g_AiWorkspace01Units;
      widthOrCount = g_AiWorkspace01Count;
    }
  }
  scratchWidthOrFactionOffset = g_GridScratchWidth;
  terrainGrid = worldRuntime->fieldGrid;
  widthOrCount = terrainGrid->gridWidth;
  remainingOrClassRecord = terrainGrid->gridHeight - 2;
  cellByteCursor = (uint8_t *)terrainGrid->cells + factionIndex;
  scratchCell = g_GridScratchPrimary + g_GridScratchWidth * 2 + 2;
  spacingOrPanelIndex = g_GridScratchWidth * 24;
  AiPlanning_CollectActiveGridMaskClasses();
  /* Site scan over the field grid (AI_SCAN_CELL_BYTES-byte cells; cellByteCursor points at the faction's byte of the cell's
     runtime area, one row behind) in step with the 4-dword-per-cell scratch grid. */
  rowStrideOrClassId = widthOrCount * sizeof(FieldGridCell);
  countOrMask = widthOrCount;
  do {
    do {
      if ((((scratchCell[scratchWidthOrFactionOffset * 3].stateMask & GRID_SCRATCH_BLOCKED) == 0) &&
          ((cellByteCursor[rowStrideOrClassId + AI_SCAN_OCCUPANCY_OFFSET] & FIELD_CELL_OCCUPANCY_PRESENCE_BITS) != 0)) &&
         (((((*(ResourceExtractionDescriptor32 *)(cellByteCursor + AI_SCAN_OCCUPANCY_OFFSET) & FIELD_CELL_OCCUPANCY_PRESENCE_BITS) == 0 ||
            (((cellByteCursor[AI_SCAN_CELL_BYTES + AI_SCAN_OCCUPANCY_OFFSET] & FIELD_CELL_OCCUPANCY_PRESENCE_BITS) == 0 || ((cellByteCursor[rowStrideOrClassId + (AI_SCAN_OCCUPANCY_OFFSET - AI_SCAN_CELL_BYTES)] & FIELD_CELL_OCCUPANCY_PRESENCE_BITS) == 0)))) ||
           (((cellByteCursor[rowStrideOrClassId + (AI_SCAN_CELL_BYTES + AI_SCAN_OCCUPANCY_OFFSET)] & FIELD_CELL_OCCUPANCY_PRESENCE_BITS) == 0 ||
            (((cellByteCursor[widthOrCount * (2 * sizeof(FieldGridCell)) + (AI_SCAN_OCCUPANCY_OFFSET - AI_SCAN_CELL_BYTES)] & FIELD_CELL_OCCUPANCY_PRESENCE_BITS) == 0 ||
             ((cellByteCursor[widthOrCount * (2 * sizeof(FieldGridCell)) + AI_SCAN_OCCUPANCY_OFFSET] & FIELD_CELL_OCCUPANCY_PRESENCE_BITS) == 0)))))) &&
          ((neighborhoodMask = scratchCell->stateMask | scratchCell[3].stateMask |
            scratchCell[scratchWidthOrFactionOffset * 3 + -3].stateMask
                     | scratchCell[scratchWidthOrFactionOffset * 3 + 3].stateMask |
                          scratchCell[scratchWidthOrFactionOffset * 6 + -3].stateMask |
                     scratchCell[scratchWidthOrFactionOffset * 6].stateMask | scratchCell[scratchWidthOrFactionOffset *
                                                                                          3].stateMask,
           (g_AiActiveGridMaskClass0 & neighborhoodMask) == 0 ||
           ((((g_AiActiveGridMaskClass1 & neighborhoodMask) == 0 || ((g_AiActiveGridMaskClass2 & neighborhoodMask) == 0)
             ) || ((g_AiActiveGridMaskClass3 & neighborhoodMask) == 0)))))))) {
        AiSiteCandidate_AddGeneralCellIfSeparated
                  ((FieldGridCell *)(cellByteCursor + (rowStrideOrClassId - factionIndex)));
        cellByteCursor = (uint8_t *)&((FieldGridCell *)(cellByteCursor + (rowStrideOrClassId -
                                                                          factionIndex)))[-widthOrCount] +
                  factionIndex;
      }
      if ((((cellByteCursor[rowStrideOrClassId + AI_SCAN_OCCUPANCY_OFFSET] & 0x10) != 0) &&
          ((scratchCell[scratchWidthOrFactionOffset * 3].stateMask & (GRID_SCRATCH_BLOCKED | GRID_SCRATCH_LOW_BAND0)) == 0)) &&
         ((neighborhoodMask = scratchCell[scratchWidthOrFactionOffset * 3].stateMask | scratchCell->stateMask |
           scratchCell[4].stateMask |
                    scratchCell[scratchWidthOrFactionOffset * 3 + -4].stateMask |
                         scratchCell[scratchWidthOrFactionOffset * 3 + 4].stateMask |
                    scratchCell[scratchWidthOrFactionOffset * 6 + -4].stateMask |
                         scratchCell[scratchWidthOrFactionOffset * 6].stateMask,
          (neighborhoodMask & (GRID_SCRATCH_BLOCKED | GRID_SCRATCH_TERRAIN_CLASS_BIT24)) == 0 ||
          (((neighborhoodMask & (GRID_SCRATCH_BLOCKED | GRID_SCRATCH_TERRAIN_CLASS_BIT25)) == 0 || ((neighborhoodMask & (GRID_SCRATCH_BLOCKED | GRID_SCRATCH_TERRAIN_CLASS_BIT28)) == 0)))))) {
        fieldCell = (FieldGridCell *)(cellByteCursor + (rowStrideOrClassId - factionIndex));
        if ((fieldCell->flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) == 0) {
          AiSiteCandidate_AddFlaggedCellIfSeparated(fieldCell);
        }
        cellByteCursor = (uint8_t *)&fieldCell[-widthOrCount] + factionIndex;
      }
      if (((cellByteCursor[rowStrideOrClassId + AI_SCAN_OCCUPANCY_OFFSET] & FIELD_CELL_OCCUPANCY_PRESENCE_BITS) != 0) &&
         ((scratchCell[scratchWidthOrFactionOffset * 3].stateMask & (AI_SITE_SCRATCH_OBSTACLE_BITS | AI_SITE_SCRATCH_BANDS_CLASSES_0_TO_5)) == 0)) {
        if (((scratchCell[scratchWidthOrFactionOffset * 3].stateMask & (AI_SITE_SCRATCH_OBSTACLE_BITS | GRID_SCRATCH_LOW_DISTANCE_BANDS)) == 0) &&
           (((scratchCell->stateMask | scratchCell[4].stateMask | scratchCell[scratchWidthOrFactionOffset * 3 +
                                                                              -4].stateMask |
              scratchCell[scratchWidthOrFactionOffset * 3 + 4].stateMask | scratchCell[scratchWidthOrFactionOffset * 6 +
                                                                                       -4].stateMask |
             scratchCell[scratchWidthOrFactionOffset * 6].stateMask) & AI_SITE_SCRATCH_OBSTACLE_BITS) == 0)) {
          scratchCellOrigin = scratchCell + scratchWidthOrFactionOffset * -3;
          scratchCell = scratchCellOrigin + scratchWidthOrFactionOffset * 3;
          if (((scratchCellOrigin->stateMask | scratchCellOrigin[8].stateMask |
                scratchCellOrigin[scratchWidthOrFactionOffset * 6 + -8].stateMask |
                scratchCellOrigin[scratchWidthOrFactionOffset * 6 + 8].stateMask |
                     scratchCellOrigin[scratchWidthOrFactionOffset * 12 + -8].stateMask |
               scratchCellOrigin[scratchWidthOrFactionOffset * 12].stateMask) & AI_SITE_SCRATCH_OBSTACLE_BITS) == 0) {
            fieldCell = (FieldGridCell *)(cellByteCursor + (rowStrideOrClassId - factionIndex));
            if ((fieldCell->flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) != 0) {
              AiSiteCandidate_AddTerrainFeatureCellIfSeparated(fieldCell,spacingOrPanelIndex);
            }
            cellByteCursor = (uint8_t *)&fieldCell[-widthOrCount] + factionIndex;
          }
        }
        if (((scratchCell->stateMask | scratchCell[4].stateMask | scratchCell[scratchWidthOrFactionOffset * 3 +
                                                                              -4].stateMask |
              scratchCell[scratchWidthOrFactionOffset * 3 + 4].stateMask | scratchCell[scratchWidthOrFactionOffset * 6 +
                                                                                       -4].stateMask |
             scratchCell[scratchWidthOrFactionOffset * 6].stateMask) & AI_SITE_SCRATCH_OBSTACLE_BITS) == 0) {
          fieldCell = (FieldGridCell *)(cellByteCursor + -factionIndex);
          if (((((((fieldCell[widthOrCount].flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) ==
                   0) && ((fieldCell->flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) ==
                          0)) &&
                ((fieldCell[1].flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) == 0)) &&
               (((fieldCell[widthOrCount - 1].flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK)
                 == 0 && ((fieldCell[widthOrCount + 1].flagsAndMaterial &
                          FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) == 0)))) &&
              ((fieldCell[widthOrCount * 2 + -1].flagsAndMaterial &
               FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) == 0)) &&
             ((fieldCell[widthOrCount * 2].flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) ==
              0)) {
            AiBaseSiteWorkspace_AddCellInsideBase(fieldCell + widthOrCount);
            fieldCell = fieldCell + widthOrCount + -widthOrCount;
          }
          cellByteCursor = (uint8_t *)fieldCell + factionIndex;
        }
        if (((scratchCell[scratchWidthOrFactionOffset * 3].stateMask & (AI_SITE_SCRATCH_OBSTACLE_BITS | GRID_SCRATCH_LOW_DISTANCE_BANDS)) == 0) &&
           (((scratchCell->stateMask | scratchCell[4].stateMask | scratchCell[scratchWidthOrFactionOffset * 3 +
                                                                              -4].stateMask |
              scratchCell[scratchWidthOrFactionOffset * 3 + 4].stateMask | scratchCell[scratchWidthOrFactionOffset * 6 +
                                                                                       -4].stateMask |
             scratchCell[scratchWidthOrFactionOffset * 6].stateMask) & AI_SITE_SCRATCH_OBSTACLE_BITS) == 0)) {
          scratchCellOrigin = scratchCell + scratchWidthOrFactionOffset * -3;
          scratchCell = scratchCellOrigin + scratchWidthOrFactionOffset * 3;
          if (((scratchCellOrigin->stateMask | scratchCellOrigin[8].stateMask |
                scratchCellOrigin[scratchWidthOrFactionOffset * 6 + -8].stateMask |
                scratchCellOrigin[scratchWidthOrFactionOffset * 6 + 8].stateMask |
                     scratchCellOrigin[scratchWidthOrFactionOffset * 12 + -8].stateMask |
               scratchCellOrigin[scratchWidthOrFactionOffset * 12].stateMask) &
              (GRID_SCRATCH_TERRAIN_CLASS_BIT30|GRID_SCRATCH_TERRAIN_CLASS_BIT29|
              GRID_SCRATCH_TERRAIN_CLASS_BIT28)) == 0) {
            fieldCell = (FieldGridCell *)(cellByteCursor + -factionIndex);
            if (((((fieldCell->flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) == 0) &&
                 ((fieldCell[1].flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) == 0))
                && ((fieldCell[widthOrCount - 1].flagsAndMaterial &
                    FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) == 0)) &&
               ((((fieldCell[widthOrCount + 1].flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK)
                  == 0 && ((fieldCell[widthOrCount * 2 + -1].flagsAndMaterial &
                           FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) == 0)) &&
                (((fieldCell[widthOrCount * 2].flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK)
                  == 0 && ((fieldCell[widthOrCount].flagsAndMaterial &
                           FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) == 0)))))) {
              AiBaseSiteWorkspace_AddLargeCellInsideBase(fieldCell + widthOrCount);
              fieldCell = fieldCell + widthOrCount + -widthOrCount;
            }
            cellByteCursor = (uint8_t *)fieldCell + factionIndex;
          }
        }
      }
      cellByteCursor = cellByteCursor + sizeof(FieldGridCell);
      scratchCell = scratchCell + 4;
      countOrMask--;
    } while (countOrMask != 0);
    scratchCell = scratchCell + g_GridScratchWidth * 3;
    countOrMask = widthOrCount & FIELD_GRID_ROW_STRIDE_WIDTH_MASK;
    remainingOrClassRecord--;
  } while (remainingOrClassRecord != 0);
  /* production mask of the own structures: class 13 contributes its classParameterC4 mask, class 22 bit 3,
     class 11 bit 4 */
  countOrMask = 0;
  primaryEntryCursor = g_AiWorkspace00Structures;
  for (widthOrCount = g_AiWorkspace00Count; widthOrCount != 0; widthOrCount--) {
    if ((int *)primaryEntryCursor->runtimeSlotAddressOrZero != NULL) {
      remainingOrClassRecord = *(int *)primaryEntryCursor->runtimeSlotAddressOrZero;
      rowStrideOrClassId = ((ModelDefinition *)remainingOrClassRecord)->runtimeClassId;
      if (rowStrideOrClassId == MODEL_RUNTIME_CLASS_13) {
        countOrMask = countOrMask | ((ModelDefinition *)remainingOrClassRecord)->classParameterC4;
      }
      else if (rowStrideOrClassId == MODEL_RUNTIME_CLASS_22) {
        countOrMask = countOrMask | ARMY_ASSET_FLAG_BUILT_AT_AIRCRAFT_PAD;
      }
      else if (rowStrideOrClassId == MODEL_RUNTIME_CLASS_11) {
        countOrMask = countOrMask | ARMY_ASSET_FLAG_BUILT_BY_CLASS11;
      }
    }
    primaryEntryCursor++;
  }
  /* workspace 11: registry assets that are enabled (bit 0), fully unlocked and producible (production mask) */
  armyAssetRegistryCursor = g_ArmyAssetRecordRegistry;
  remainingOrClassRecord = ARMY_ASSET_REGISTRY_SLOT_COUNT;
  workspace11Cursor = g_AiWorkspace11ProducibleAssets;
  do {
    definitionNode = *armyAssetRegistryCursor;
    if (((definitionNode != NULL) &&
        ((definitionNode[1].selectionDetailTemplateVariantIndex & 1) != 0)) &&
       (((testResult = ModelDefinitionHierarchy_AllTechnologyUnlockedForFaction
                             (factionIndex,(ModelDefinitionHierarchyNodeAddress32)definitionNode),
         !testResult && ((definitionNode[1].selectionDetailTemplateVariantIndex & countOrMask) != 0)) &&
        (((definitionNode->registryId < ARM_0300_BUILDING_MDL0301 ||
          ((ARM_0340_BUILDING_MDL0314 - 1) < definitionNode->registryId)) &&
         (g_AiWorkspace11Count < AI_WORKSPACE11_CAPACITY)))))) {
      *workspace11Cursor = definitionNode;
      g_AiWorkspace11Count++;
      workspace11Cursor++;
    }
    armyAssetRegistryCursor++;
    remainingOrClassRecord--;
    runtimeEntryCursor = g_AiWorkspace02VisibleHostiles;
    widthOrCount = g_AiWorkspace02Count;
    targetEntry = g_AiWorkspace07Targets;
  } while (remainingOrClassRecord != 0);
  while ((widthOrCount != 0 &&
         (slotModelRuntime = runtimeEntryCursor->modelRuntime, g_AiWorkspace07Count < AI_WORKSPACE07_CAPACITY))) {
    if (slotModelRuntime != NULL) {
      targetEntry->modelRuntime = slotModelRuntime;
      modelNode = slotModelRuntime->rootModelNodeOrSavedOffset.modelNode;
      translationX = modelNode->worldTransform.translation.x;
      translationY = modelNode->worldTransform.translation.y;
      targetEntry->modelNode = modelNode;
      targetEntry->worldXQ12 = translationX;
      targetEntry->worldYQ12 = translationY;
      g_AiWorkspace07Count++;
      targetEntry++;
    }
    runtimeEntryCursor++;
    widthOrCount--;
  }
  /* technology candidates: researchTechnologyIds[28] down to [1] of each own structure's definition */
  scratchWidthOrFactionOffset = factionIndex * (int)sizeof(GameFactionRuntimeRecord); /* byte offset of the faction's runtime record */
  primaryEntryCursor = g_AiWorkspace00Structures;
  for (countOrMask = g_AiWorkspace00Count; countOrMask != 0; countOrMask--) {
    slotModelRuntime = (ModelRuntimeSlot *)primaryEntryCursor->runtimeSlotAddressOrZero;
    if (slotModelRuntime != NULL) {
      slotDefinition = slotModelRuntime->definitionOrSavedId.runtimeDefinition;
      spacingOrPanelIndex = 28;
      do {
        testResult = AiTechnologyCandidate_IsCurrentlyAvailable
                           ((PckTechnologyIdCatalog)slotDefinition->researchTechnologyIds[spacingOrPanelIndex],
                                 scratchWidthOrFactionOffset);
        if (!testResult) {
          registerContinuity = AiTechnologyPlanning_AddCandidateRecord
                             (spacingOrPanelIndex,countOrMask,scratchWidthOrFactionOffset,slotModelRuntime,
                              (PckTechnologyIdCatalog)slotDefinition->researchTechnologyIds[spacingOrPanelIndex]);
          scratchWidthOrFactionOffset = registerContinuity.preservedEdxFactionRecordOffset;
          countOrMask = registerContinuity.preservedEcxSourceArmyEntriesRemaining;
          spacingOrPanelIndex = registerContinuity.preservedEaxTechnologyPanelIndex;
        }
        spacingOrPanelIndex--;
      } while (spacingOrPanelIndex != 0);
    }
    primaryEntryCursor++;
  }
  return;
}


/* Address: 0x0053BF30.
   Research planning: once the faction has an ARM 330 (0x14A) structure, scores every available technology of
   workspace 12 with the score callback of its kind and proposes the best one (entry kind 2) with
   workspace12BestCandidateBaseWeight, halved while the faction's primary anchor cooldown runs.
*/
void AiTechnologyCandidate_AddBestResearch(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  AiTechnologyCandidateScore candidateScore;
  int wordIndexOrBestScore;
  AiTechnologyPlanningCandidateCount candidatesRemaining;
  AiTechnologyPlanningCandidate *candidateCursor;
  bool hasSpecialAsset;
  uint32_t weightRange;
  AiCandidateEntryKind entryKind;
  RuntimeToken entityId;
  
  hasSpecialAsset = AiPrimaryWorkspace_HasEntryById(ARM_0330_BUILDING_MDL0303);
  if (hasSpecialAsset) {
    /* EDX side channel for the category score callback: bit 2 / bit 4 when the faction owns any
       technology of category 2 / 3 (the decompiler kept only the empty loop). */
    g_AiTechnologyScoreCategoryMaskEdx = 0;
    for (wordIndexOrBestScore = 0; wordIndexOrBestScore < 8; wordIndexOrBestScore++) {
      uint32_t owned = g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits[wordIndexOrBestScore];
      if ((g_TechnologyCategoryMasks.category2[wordIndexOrBestScore] & owned) != 0) {
        g_AiTechnologyScoreCategoryMaskEdx = g_AiTechnologyScoreCategoryMaskEdx | 2;
      }
      if ((g_TechnologyCategoryMasks.category3[wordIndexOrBestScore] & owned) != 0) {
        g_AiTechnologyScoreCategoryMaskEdx = g_AiTechnologyScoreCategoryMaskEdx | 4;
      }
    }
    wordIndexOrBestScore = 0;
    if (g_AiWorkspace12Count != 0) {
      entryKind = 2; /* technology */
      weightRange = g_AiKnowledgeData->parameters.workspace12BestCandidateBaseWeight;
      entityId = 0;
      candidatesRemaining = g_AiWorkspace12Count;
      candidateCursor = g_AiWorkspace12TechnologyCandidates;
      do {
        candidateScore = g_AiTechnologyCandidateScoreCallbackTable[candidateCursor->scoreKind08]
                          (factionIndex,candidateCursor->technologyId00,worldRuntime);
        if (wordIndexOrBestScore < candidateScore) {
          entityId = candidateCursor->technologyId00;
          wordIndexOrBestScore = candidateScore;
        }
        candidateCursor++;
        candidatesRemaining--;
      } while (candidatesRemaining != 0);
      if (wordIndexOrBestScore != 0) {
        if (g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorCooldown != 0) {
          weightRange = weightRange >> 1;
        }
        AiCandidateWorkspace_AddOrAccumulateWeightedEntry(entityId,weightRange,entryKind);
      }
    }
  }
  return;
}


/* Address: 0x00537420.
   Empties the AI candidate workspace (workspace 13) by resetting its entry count.
*/
void AiCandidateWorkspace_Clear(void)

{
  g_AiCandidateWorkspaceEntryCount = 0;
  return;
}


/* Address: 0x00537430.
   Keeps the first (at most three) candidates of the AI candidate workspace in the faction's runtime record
   (candidateCache, factionImageByteOffset = faction * 0x740) so that the next planning pass of this faction can
   start from them (AiCandidateWorkspace_LoadFromFactionImage).
*/
void AiCandidateWorkspace_SaveToFactionImage(FactionImageByteOffset factionImageByteOffset)

{
  int entryCountOrDwordsRemaining;
  uint32_t *candidateWorkspaceSourceCursor;
  uint32_t *factionImageDestinationCursor;
  
  candidateWorkspaceSourceCursor = &g_AiWorkspace13Candidates->weightedScoreAndKind;
  entryCountOrDwordsRemaining = g_AiCandidateWorkspaceEntryCount;
  if (2 < g_AiCandidateWorkspaceEntryCount) {
    entryCountOrDwordsRemaining = 3;
  }
  AI_FACTION_CANDIDATE_CACHE(factionImageByteOffset)->savedEntryCount = entryCountOrDwordsRemaining;
  factionImageDestinationCursor =
       &AI_FACTION_CANDIDATE_CACHE(factionImageByteOffset)->savedEntries[0].weightedScoreAndKind;
  entryCountOrDwordsRemaining = entryCountOrDwordsRemaining * 2; /* two dwords per entry */
  if (entryCountOrDwordsRemaining != 0) {
    for (; entryCountOrDwordsRemaining != 0; entryCountOrDwordsRemaining--) {
      *factionImageDestinationCursor = *candidateWorkspaceSourceCursor;
      candidateWorkspaceSourceCursor++;
      factionImageDestinationCursor++;
    }
  }
  return;
}


/* Address: 0x00537470.
   Refills the shared AI candidate workspace with the candidates that AiCandidateWorkspace_SaveToFactionImage
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


/* Address: 0x00537570.
   Sorts the AI candidate workspace (workspace 13) by descending weightedScoreAndKind (signed compare), so the
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
          /* the original swaps with XCHG, whose implicit bus lock Ghidra shows as LOCK/UNLOCK */
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


/* Address: 0x005375D0.
   Returns the xenite cost (Q4) of a candidate, which the purchase planner checks against the faction's xenite:
   the technology's xeniteCostQ4 for a technology candidate, else the army asset's cost dword at +0x28, or
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
      /* +0x28 of the army asset record, reached through the 16-byte prefix type */
      xeniteCostQ4 = armyAsset[2].registryId;
    }
  }
  return xeniteCostQ4;
}


/* Address: 0x00538C90.
   Returns true (CF set) when the secondary workspace (workspace 01) holds an unassigned entry (no runtime
   object yet, i.e. a pending asset) of this army asset; the workspace-01 counterpart of
   AiPrimaryWorkspace_HasUnassignedEntryById. No caller and no function-pointer table entry for it was found
   in src/ or src/generated/image_data.c.
*/
bool AiSecondaryWorkspace_HasUnassignedEntryById(PckArmyAssetIdCatalog entryId)

{
  int workspaceEntriesRemaining;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  
  workspaceEntriesRemaining = g_AiWorkspace01Count;
  workspaceEntryCursor = g_AiWorkspace01Units;
  for (; workspaceEntriesRemaining != 0; workspaceEntriesRemaining--) {
    if ((entryId == workspaceEntryCursor->armyAssetId) &&
       (workspaceEntryCursor->modelRuntime == NULL)) {
      return true;
    }
    workspaceEntryCursor++;
  }
  return false;
}


/* Address: 0x00538CF0.
   Returns true (CF set) when the secondary workspace (workspace 01) holds an entry of this army asset, assigned
   or not.
*/
bool AiSecondaryWorkspace_HasEntryById(PckArmyAssetIdCatalog entryId)

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


/* Address: 0x00538D40.
   Twin of AiPrimaryWorkspace_CountAssignedEntriesById (0x00538C40) with the same body: counts the
   primary-workspace (workspace 00) entries of this army asset that have a runtime object. No caller and no
   function-pointer table entry for this copy was found in src/ or src/generated/image_data.c.
*/
int AiPrimaryWorkspace_CountAssignedEntriesByIdDuplicate(PckArmyAssetIdCatalog entryId)

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


/* Address: 0x00538D90.
   Returns the smallest Manhattan distance from the point to an assigned secondary-workspace (workspace 01)
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


/* Address: 0x00538E00.
   Returns the smallest Manhattan distance from the point to an assigned primary-workspace (workspace 00) unit
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


/* Address: 0x00538E80.
   Returns the smallest Manhattan distance from the point to an assigned workspace-02 unit, or 0x7FFFFFFF when
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


/* Address: 0x00538EF0.
   Returns the smallest Manhattan distance from the point to an assigned workspace-03 unit, or 0x7FFFFFFF when
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


/* Address: 0x00538F60.
   Returns the smallest Manhattan distance from the point to any assigned primary-workspace (workspace 00) unit,
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


/* Address: 0x00539240.
   Builds a pending resource structure (ARM_0330/ARM_0332) of the AI faction: at the first workspace-08 site of
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
  bool placementRejected;
  ArmyRuntimeCreateResult createResult;
  
  recordsRemaining = g_AiWorkspace08Count;
  terrainFeatureEntry = g_AiWorkspace08TerrainFeatureSites;
  while (recordsRemaining != 0) {
    if (armyAssetId == terrainFeatureEntry->armyAssetId) {
      workspaceRecord = terrainFeatureEntry->cell;
      placementRejected = AiPlacement_TestWorkspaceRecordAtPoint
                        (armyAssetId,workspaceRecord,factionIndex,(UiRootNode *)worldRuntime);
      if (!placementRejected) {
        createResult = ArmyRuntime_CreateInstanceFromAsset
                          (ARMY_CREATE_UNLOCK_TECHNOLOGY,(uint32_t)(uint16_t)workspaceRecord->triangle0NormalAngles,
                           workspaceRecord->worldY,workspaceRecord->worldX,factionIndex,armyAssetId,
                           worldRuntime);
        createdSlotPair = (ArmyRuntimeSlot **)createResult.armyRuntimeOrError;
        if (createResult.failed) {
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
                  (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference, 0),
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
    terrainFeatureEntry++;
    recordsRemaining--;
  }
  return;
}


/* Address: 0x0053BCB0.
   Technology score callback for score kind 0 (g_AiTechnologyCandidateScoreCallbackTable[0], image 0x0053B9E0,
   called by AiTechnologyCandidate_AddBestResearch): a technology of this kind always scores 0, so it
   is never chosen for research.
*/
AiTechnologyCandidateScore AiTechnologyScore_AlwaysZero
          (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId,
          WorldRuntimeContext *worldRuntime)

{
  return 0;
}


/* Address: 0x0053C6C0.
   Allocates the fifteen AI workspace buffers 00-14 from the arena (sizes in their names) and loads the AI
   parameters from engine\ki.dat into g_AiKnowledgeData. Stops at the first failure with CF set and that
   failure's error code; buffers allocated before it are not freed.
*/
StatusResult AiRuntime_InitWorkspace(void)

{
  uint8_t *workspaceAllocation;
  AiTechnologyPlanningCandidate *technologyCandidateWorkspaceAllocation;
  AiKnowledgeDataImage *knowledgeDataImage;
  PackageLoadResult loadResult;
  StatusResult initStatus;
  
  loadResult = THANDOR_BITCAST(ArenaAllocResult, PackageLoadResult, g_MemoryApi.alloc(AI_WORKSPACE00_CAPACITY * sizeof(AiStructureWorkspaceEntry)));
  workspaceAllocation = loadResult.bufferOrError;
  if (!loadResult.failed) {
    g_AiWorkspace00Structures = (AiStructureWorkspaceEntry *)workspaceAllocation;
    loadResult = THANDOR_BITCAST(ArenaAllocResult, PackageLoadResult, g_MemoryApi.alloc(AI_WORKSPACE01_CAPACITY * sizeof(AiRuntimeWorkspaceEntry)));
    if (!loadResult.failed) {
      g_AiWorkspace01Units = loadResult.bufferOrError;
      loadResult = THANDOR_BITCAST(ArenaAllocResult, PackageLoadResult, g_MemoryApi.alloc(AI_WORKSPACE02_CAPACITY * sizeof(AiRuntimeWorkspaceEntry)));
      if (!loadResult.failed) {
        g_AiWorkspace02VisibleHostiles = loadResult.bufferOrError;
        loadResult = THANDOR_BITCAST(ArenaAllocResult, PackageLoadResult, g_MemoryApi.alloc(AI_WORKSPACE03_CAPACITY * sizeof(AiRuntimeWorkspaceEntry)));
        if (!loadResult.failed) {
          g_AiWorkspace03UnseenHostiles = loadResult.bufferOrError;
          loadResult = THANDOR_BITCAST(ArenaAllocResult, PackageLoadResult, g_MemoryApi.alloc(AI_WORKSPACE04_CAPACITY * sizeof(AiRuntimeWorkspaceEntry)));
          if (!loadResult.failed) {
            g_AiWorkspace04RequestedAssets = loadResult.bufferOrError;
            loadResult = THANDOR_BITCAST(ArenaAllocResult, PackageLoadResult, g_MemoryApi.alloc(AI_WORKSPACE05_CAPACITY * sizeof(AiScoredSiteWorkspaceEntry)));
            if (!loadResult.failed) {
              g_AiWorkspace05GeneralSites = loadResult.bufferOrError;
              loadResult = THANDOR_BITCAST(ArenaAllocResult, PackageLoadResult, g_MemoryApi.alloc(AI_WORKSPACE06_CAPACITY * sizeof(AiScoredSiteWorkspaceEntry)));
              if (!loadResult.failed) {
                g_AiWorkspace06FlaggedSites = loadResult.bufferOrError;
                loadResult = THANDOR_BITCAST(ArenaAllocResult, PackageLoadResult, g_MemoryApi.alloc(AI_WORKSPACE07_CAPACITY * sizeof(AiTargetWorkspaceEntry)));
                if (!loadResult.failed) {
                  g_AiWorkspace07Targets = loadResult.bufferOrError;
                  loadResult = THANDOR_BITCAST(ArenaAllocResult, PackageLoadResult, g_MemoryApi.alloc(AI_WORKSPACE08_CAPACITY * sizeof(AiTerrainFeatureWorkspaceEntry)));
                  if (!loadResult.failed) {
                    g_AiWorkspace08TerrainFeatureSites = loadResult.bufferOrError;
                    loadResult = THANDOR_BITCAST(ArenaAllocResult, PackageLoadResult, g_MemoryApi.alloc(AI_WORKSPACE09_CAPACITY * sizeof(FieldGridCell *)));
                    if (!loadResult.failed) {
                      g_AiWorkspace09Cells = loadResult.bufferOrError;
                      loadResult = THANDOR_BITCAST(ArenaAllocResult, PackageLoadResult, g_MemoryApi.alloc(AI_WORKSPACE10_CAPACITY * sizeof(FieldGridCell *)));
                      if (!loadResult.failed) {
                        g_AiWorkspace10Cells = loadResult.bufferOrError;
                        loadResult = THANDOR_BITCAST(ArenaAllocResult, PackageLoadResult, g_MemoryApi.alloc(AI_WORKSPACE11_CAPACITY * sizeof(ArmyAssetRecordPrefix *)));
                        if (!loadResult.failed) {
                          g_AiWorkspace11ProducibleAssets = loadResult.bufferOrError;
                          loadResult = THANDOR_BITCAST(ArenaAllocResult, PackageLoadResult, g_MemoryApi.alloc(AI_WORKSPACE12_CAPACITY * sizeof(AiTechnologyPlanningCandidate)));
                          technologyCandidateWorkspaceAllocation = loadResult.bufferOrError;
                          if (!loadResult.failed) {
                            g_AiWorkspace12TechnologyCandidates = technologyCandidateWorkspaceAllocation;
                            loadResult = THANDOR_BITCAST(ArenaAllocResult, PackageLoadResult, g_MemoryApi.alloc(AI_CANDIDATE_WORKSPACE_CAPACITY * sizeof(AiCandidateWorkspaceEntry)));
                            if (!loadResult.failed) {
                              g_AiWorkspace13Candidates = loadResult.bufferOrError;
                              loadResult = THANDOR_BITCAST(ArenaAllocResult, PackageLoadResult,
                                                           g_MemoryApi.alloc(AI_WORKSPACE14_CAPACITY * sizeof(ArmyRuntimeSlot *)));
                              if (!loadResult.failed) {
                                g_AiWorkspace14CollectedArmies = loadResult.bufferOrError;
                                loadResult = Package_LoadEntry((uint16_t *)u_engine_ki_dat_0053c5e4);
                                knowledgeDataImage = loadResult.bufferOrError;
                                if (!loadResult.failed) {
                                  /* success: EAX (the image pointer) stays the result value, CF clear */
                                  loadResult = THANDOR_BITCAST(uint64_t, PackageLoadResult,
                                       (THANDOR_BITCAST(PackageLoadResult, uint64_t,
                                                         loadResult) & UINT32_MAX));
                                  g_AiKnowledgeData = knowledgeDataImage;
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
  }
  initStatus.valueOrError = (uint32_t)loadResult.bufferOrError;
  initStatus.failed = loadResult.failed;
  return initStatus;
}


/* Address: 0x00537F80.
   Adds a field cell to workspace 09 (at most 1024 cells) when it lies inside the extent (+0x19C of the
   definition) of some primary-workspace structure, i.e. when AiPrimaryWorkspace_IsPointOutsideAllEntryExtents
   returns false. These cells are the build sites near the AI's own base.
*/
void AiBaseSiteWorkspace_AddCellInsideBase(FieldGridCell *currentCell)

{
  FieldGridCell **cellBuffer;
  uint32_t entryIndex;
  bool isOutsideExtents;

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


/* Address: 0x00537FC0.
   Adds a field cell to workspace 10 (base sites with the wider clearance, at most 256 cells) when it lies inside
   the extent of some own structure; otherwise the same as AiBaseSiteWorkspace_AddCellInsideBase (workspace 09).
*/
void AiBaseSiteWorkspace_AddLargeCellInsideBase(FieldGridCell *currentCell)

{
  FieldGridCell **cellBuffer;
  uint32_t entryIndex;
  bool isOutsideExtents;

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


/* Address: 0x00538B90.
   Returns true (CF set) when the primary workspace (workspace 00) holds an entry of this army asset whose
   runtime pointer is NULL.
*/
bool AiPrimaryWorkspace_HasUnassignedEntryById(PckArmyAssetIdCatalog entryId)

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


/* Address: 0x00538BF0.
   Returns true (CF set) when the primary workspace (workspace 00) holds an entry of this army asset, with or
   without a runtime object.
*/
bool AiPrimaryWorkspace_HasEntryById(PckArmyAssetIdCatalog entryId)

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


/* Address: 0x00538C40.
   Counts the primary-workspace (workspace 00) entries of this army asset that have a runtime object.
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


/* Address: 0x005374B0.
   Proposes a purchase candidate (id + kind) with the randomised score 5 * weightRange + random % weightRange.
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


/* Address: 0x00538FD0.
   Returns false (CF clear) as soon as the point lies strictly inside the square extent of some assigned
   primary-workspace (workspace 00) unit: both axis distances to the unit's position below the extent at +0x19C
   of its definition. True (CF set) when it is outside all of them. Arguments are Y first, then X, as every
   caller passes them.
*/
bool AiPrimaryWorkspace_IsPointOutsideAllEntryExtents(Q12 worldY,Q12 worldX)

{
  Q12 deltaXAbsQ12;
  int workspaceEntriesRemaining;
  Q12 deltaYAbsQ12;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  ModelRuntimeSlot *entryModelRuntime;
  
  workspaceEntriesRemaining = g_AiWorkspace00Count;
  workspaceEntryCursor = (AiRuntimeWorkspaceEntry *)g_AiWorkspace00Structures;
  while (workspaceEntriesRemaining != 0) {
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
    workspaceEntryCursor++;
    workspaceEntriesRemaining--;
  }
  return true;
}

