/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/ai/workspaces.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/ai/workspaces.h>
#include <thandor/thandor.h>

/* Implementation ownership: gameplay/ai/workspaces. */

/* Address: 0x0053A1E0.
   Proposes armyAssetId at the first workspace 08 site of that asset where it can be placed (placement mode 4),
   unless one of it is still unassigned. Weight: 3 * baseWeight / (existing count + 3); for assets other than
   ARM 330 (0x14A) additionally scaled by (2 * unpowered + supplied Energy demand) / (record +0x358 rate << 4)
   when that rate is nonzero.
*/
void __thandor_void_preserve_eax_ecx_edx
AiWorkspaceAssetCandidate_AddWeightedEntry
          (AiCandidateScore32 baseWeight,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  int remainingOrCountOrRate;
  uint32_t weightRange;
  AiTerrainFeatureWorkspaceEntry *terrainFeatureEntry;
  bool testResult;
  
  remainingOrCountOrRate = g_AiWorkspace08Count;
  terrainFeatureEntry = g_AiWorkspaceBuffer08_Size0200;
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

void __thandor_void_preserve_eax_ecx_edx
AiPlanning_RebuildFactionWorkspaces
          (AiPlanningPhaseIndex planningPhaseDispatchIndex,
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
  ModelRuntimeSlot *slotModelRuntime;
  AiWorkspace00EntryView8 *primaryBuffer;
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
  AiWorkspace00EntryView8 *primaryEntryCursor;
  FieldGridCell *fieldCell;
  uint8_t *cellByteCursor;
  ArmyAssetRecordPrefix **workspace11Cursor;
  WorldOwnerListNode100 *worldNode;
  uint32_t *armyAssetPointerCursor;
  GridScratchCell *scratchCellOrigin;
  GridScratchCell *scratchCell;
  ArmyAssetRecordPrefix **armyAssetRegistryCursor;
  AiTargetWorkspaceEntry *targetEntry;
  bool testResult;
  AiTechnologyPlanningLoopRegisterContinuityResult registerContinuity;
  ArmyRuntimeSlot *armySlot;
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
      runtimeEntryCursor = g_AiWorkspaceBuffer01_Size0200;
      countOrMask = g_AiWorkspace00Count;
      primaryEntryCursor = g_AiWorkspaceBuffer00_Size0400;
      if (worldNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
        armySlot = worldNode->runtimePayload;
        entityRuntime = armySlot->linkedEntityRuntime;
        if (factionIndex == entityRuntime->common.ownership.ownerIndex) {
          assetId = entityRuntime->common.runtimeIdentityOrArmyAssetId;
          if (assetId < ARM_0300_BUILDING_MDL0301) {
            if (g_AiWorkspace01Count < 0x40) {
              g_AiWorkspaceBuffer01_Size0200[g_AiWorkspace01Count].armyRuntime = armySlot;
              runtimeEntryCursor[widthOrCount].armyAssetId = assetId;
              g_AiWorkspace01Count++;
            }
          }
          else if (g_AiWorkspace00Count < 0x80) {
            g_AiWorkspaceBuffer00_Size0400[g_AiWorkspace00Count].runtimeSlotAddressOrZero =
                 (AiWorkspaceRuntimeSlotAddress32)armySlot;
            primaryEntryCursor[countOrMask].armyAssetId = assetId;
            g_AiWorkspace00Count++;
            if (assetId == ARM_0300_BUILDING_MDL0301) {
              g_AiWorkspaceOwnedAsset300Runtime = armySlot;
            }
          }
        }
        else {
          /* two bits per faction: how this faction sees the foreign entity */
          countOrMask = *(uint32_t *)((entityRuntime->common).damageState.reserved0C_23 + 0x10) >>
                   ((char)factionIndex * '\x02' & 0x1fU);
          if (((entityRuntime->common).ownership.ownerIndex != 0) &&
             (testResult = GameFactionRuntime_TestCapabilityBitClear
                                 ((entityRuntime->common).ownership.ownerIndex,factionIndex),
             countSnapshotOrRemaining = g_AiWorkspace03Count, workspace03Buffer = g_AiWorkspaceBuffer03_Size1000,
             widthOrCount = g_AiWorkspace02Count, runtimeEntryCursor = g_AiWorkspaceBuffer02_Size0400
             , testResult)) {
            if (((countOrMask & 2) == 0) &&
               (((countOrMask & 1) == 0 ||
                (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand
                 [*(int *)(*(int *)(entityRuntime->common).ownership.definitionOrClassRecord + 0x4c)] !=
                 ArmyRuntime_ClassCommandHandlerGroupA)))) {
              assetId = (entityRuntime->common).runtimeIdentityOrArmyAssetId;
              if (g_AiWorkspace03Count < 0x200) {
                g_AiWorkspaceBuffer03_Size1000[g_AiWorkspace03Count].armyRuntime = armySlot;
                workspace03Buffer[countSnapshotOrRemaining].armyAssetId = assetId;
                g_AiWorkspace03Count++;
              }
            }
            else {
              assetId = (entityRuntime->common).runtimeIdentityOrArmyAssetId;
              if (g_AiWorkspace02Count < 0x80) {
                g_AiWorkspaceBuffer02_Size0400[g_AiWorkspace02Count].armyRuntime = armySlot;
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
    primaryEntryCursor = g_AiWorkspaceBuffer00_Size0400;
    countOrMask = g_AiWorkspace00Count;
    /* pending (not yet built) army assets: unassigned entries, the primary list also as requests */
    for (armyAssetsRemaining = g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount; armyAssetsRemaining != 0
        ; armyAssetsRemaining--) {
      assetId = *(PckArmyAssetIdCatalog *)(*armyAssetPointerCursor + 8);
      g_AiWorkspaceBuffer00_Size0400 = primaryEntryCursor;
      g_AiWorkspace00Count = countOrMask;
      if (countOrMask < 0x80) {
        primaryEntryCursor[countOrMask].runtimeSlotAddressOrZero = 0;
        primaryEntryCursor[countOrMask].armyAssetId = assetId;
        g_AiWorkspace00Count++;
      }
      countOrMask = g_AiWorkspace04Count;
      runtimeEntryCursor = g_AiWorkspaceBuffer04_Size0040;
      if (g_AiWorkspace04Count < 8) {
        g_AiWorkspaceBuffer04_Size0040[g_AiWorkspace04Count].armyRuntime = NULL;
        runtimeEntryCursor[countOrMask].armyAssetId = assetId;
        g_AiWorkspace04Count++;
      }
      armyAssetPointerCursor++;
      primaryEntryCursor = g_AiWorkspaceBuffer00_Size0400;
      countOrMask = g_AiWorkspace00Count;
    }
    armyAssetPointerCursor = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetPointersOrIds;
    runtimeEntryCursor = g_AiWorkspaceBuffer01_Size0200;
    widthOrCount = g_AiWorkspace01Count;
    for (armyAssetsRemaining = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount;
        primaryBuffer = primaryEntryCursor, countSnapshotOrRemaining = countOrMask, armyAssetsRemaining != 0; armyAssetsRemaining--) {
      assetId = *(PckArmyAssetIdCatalog *)(*armyAssetPointerCursor + 8);
      g_AiWorkspaceBuffer00_Size0400 = primaryEntryCursor;
      g_AiWorkspace00Count = countOrMask;
      g_AiWorkspaceBuffer01_Size0200 = runtimeEntryCursor;
      g_AiWorkspace01Count = widthOrCount;
      if (assetId < ARM_0300_BUILDING_MDL0301) {
        if (widthOrCount < 0x40) {
          runtimeEntryCursor[widthOrCount].armyRuntime = NULL;
          runtimeEntryCursor[widthOrCount].armyAssetId = assetId;
          g_AiWorkspace01Count++;
        }
      }
      else if (countOrMask < 0x80) {
        primaryEntryCursor[countOrMask].runtimeSlotAddressOrZero = 0;
        primaryEntryCursor[countOrMask].armyAssetId = assetId;
        g_AiWorkspace00Count++;
      }
      armyAssetPointerCursor++;
      primaryEntryCursor = g_AiWorkspaceBuffer00_Size0400;
      countOrMask = g_AiWorkspace00Count;
      runtimeEntryCursor = g_AiWorkspaceBuffer01_Size0200;
      widthOrCount = g_AiWorkspace01Count;
    }
    for (; g_AiWorkspaceBuffer00_Size0400 = primaryBuffer, g_AiWorkspace00Count = countOrMask,
        g_AiWorkspaceBuffer01_Size0200 = runtimeEntryCursor, g_AiWorkspace01Count = widthOrCount,
        countSnapshotOrRemaining != 0; countSnapshotOrRemaining--) {
      /* own structures of class 0x0B / 0x16 / 0x0D: the asset in production (slot word 0x18) counts as an
         unassigned entry while their state word (0x2E resp. 0x2B) is 1 */
      primarySlotWords = (int *)primaryEntryCursor->runtimeSlotAddressOrZero;
      if (primarySlotWords != NULL) {
        remainingOrClassRecord = *primarySlotWords;
        if (*(int *)(remainingOrClassRecord + 0x4c) == 0xb) {
          assetId = primarySlotWords[0x18];
          if ((primarySlotWords[0x2e] == 1) && (countOrMask < 0x80)) {
            primaryBuffer[countOrMask].runtimeSlotAddressOrZero = 0;
            primaryBuffer[countOrMask].armyAssetId = assetId;
            g_AiWorkspace00Count++;
          }
        }
        else if (*(int *)(remainingOrClassRecord + 0x4c) == 0x16) {
          assetId = primarySlotWords[0x18];
          if ((primarySlotWords[0x2b] == 1) && (widthOrCount < 0x40)) {
            runtimeEntryCursor[widthOrCount].armyRuntime = NULL;
            runtimeEntryCursor[widthOrCount].armyAssetId = assetId;
            g_AiWorkspace01Count++;
          }
        }
        else if (((*(int *)(remainingOrClassRecord + 0x4c) == 0xd) && (assetId = primarySlotWords[0x18], primarySlotWords[0x2e] == 1)) &&
                (widthOrCount < 0x40)) {
          runtimeEntryCursor[widthOrCount].armyRuntime = NULL;
          runtimeEntryCursor[widthOrCount].armyAssetId = assetId;
          g_AiWorkspace01Count++;
        }
      }
      primaryEntryCursor++;
      primaryBuffer = g_AiWorkspaceBuffer00_Size0400;
      countOrMask = g_AiWorkspace00Count;
      runtimeEntryCursor = g_AiWorkspaceBuffer01_Size0200;
      widthOrCount = g_AiWorkspace01Count;
    }
  }
  scratchWidthOrFactionOffset = g_GridScratchWidth;
  terrainGrid = worldRuntime->fieldGrid;
  widthOrCount = terrainGrid->gridWidth;
  remainingOrClassRecord = terrainGrid->gridHeight - 2;
  cellByteCursor = terrainGrid->cells[0].runtime0C_3F + factionIndex + -0xc;
  scratchCell = g_GridScratchPrimary + g_GridScratchWidth * 2 + 2;
  spacingOrPanelIndex = g_GridScratchWidth * 0x18;
  AiPlanning_CollectActiveGridMaskClasses();
  /* Site scan over the field grid (0x80-byte cells; cellByteCursor points at the faction's byte of the cell's
     runtime area, one row behind) in step with the 4-dword-per-cell scratch grid. */
  rowStrideOrClassId = widthOrCount * 0x80;
  countOrMask = widthOrCount;
  do {
    do {
      if ((((scratchCell[scratchWidthOrFactionOffset * 3].stateMask & 0x80000000) == 0) &&
          ((cellByteCursor[rowStrideOrClassId + 0x70] & 0xf9) != 0)) &&
         (((((*(ResourceExtractionDescriptor32 *)(cellByteCursor + 0x70) & 0xf9) == 0 ||
            (((cellByteCursor[0xf0] & 0xf9) == 0 || ((cellByteCursor[rowStrideOrClassId + -0x10] & 0xf9) == 0)))) ||
           (((cellByteCursor[rowStrideOrClassId + 0xf0] & 0xf9) == 0 ||
            (((cellByteCursor[widthOrCount * 0x100 + -0x10] & 0xf9) == 0 ||
             ((cellByteCursor[widthOrCount * 0x100 + 0x70] & 0xf9) == 0)))))) &&
          ((neighborhoodMask = scratchCell->stateMask | scratchCell[3].stateMask | scratchCell[scratchWidthOrFactionOffset * 3 + -3].stateMask
                     | scratchCell[scratchWidthOrFactionOffset * 3 + 3].stateMask | scratchCell[scratchWidthOrFactionOffset * 6 + -3].stateMask |
                     scratchCell[scratchWidthOrFactionOffset * 6].stateMask | scratchCell[scratchWidthOrFactionOffset * 3].stateMask,
           (g_AiActiveGridMaskClass0 & neighborhoodMask) == 0 ||
           ((((g_AiActiveGridMaskClass1 & neighborhoodMask) == 0 || ((g_AiActiveGridMaskClass2 & neighborhoodMask) == 0)
             ) || ((g_AiActiveGridMaskClass3 & neighborhoodMask) == 0)))))))) {
        AiSiteCandidate_AddGeneralCellIfSeparated
                  ((FieldGridCell *)(cellByteCursor + (rowStrideOrClassId - factionIndex)));
        cellByteCursor = ((FieldGridCell *)(cellByteCursor + (rowStrideOrClassId - factionIndex)))[-widthOrCount].runtime0C_3F +
                  factionIndex + -0xc;
      }
      if ((((cellByteCursor[rowStrideOrClassId + 0x70] & 0x10) != 0) &&
          ((scratchCell[scratchWidthOrFactionOffset * 3].stateMask & 0x80000100) == 0)) &&
         ((neighborhoodMask = scratchCell[scratchWidthOrFactionOffset * 3].stateMask | scratchCell->stateMask | scratchCell[4].stateMask |
                    scratchCell[scratchWidthOrFactionOffset * 3 + -4].stateMask | scratchCell[scratchWidthOrFactionOffset * 3 + 4].stateMask |
                    scratchCell[scratchWidthOrFactionOffset * 6 + -4].stateMask | scratchCell[scratchWidthOrFactionOffset * 6].stateMask,
          (neighborhoodMask & 0x81000000) == 0 ||
          (((neighborhoodMask & 0x82000000) == 0 || ((neighborhoodMask & 0x90000000) == 0)))))) {
        fieldCell = (FieldGridCell *)(cellByteCursor + (rowStrideOrClassId - factionIndex));
        if ((fieldCell->flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) == 0) {
          AiSiteCandidate_AddFlaggedCellIfSeparated(fieldCell);
        }
        cellByteCursor = fieldCell[-widthOrCount].runtime0C_3F + factionIndex + -0xc;
      }
      if (((cellByteCursor[rowStrideOrClassId + 0x70] & 0xf9) != 0) &&
         ((scratchCell[scratchWidthOrFactionOffset * 3].stateMask & 0xf0003f00) == 0)) {
        if (((scratchCell[scratchWidthOrFactionOffset * 3].stateMask & 0xf000ff00) == 0) &&
           (((scratchCell->stateMask | scratchCell[4].stateMask | scratchCell[scratchWidthOrFactionOffset * 3 + -4].stateMask |
              scratchCell[scratchWidthOrFactionOffset * 3 + 4].stateMask | scratchCell[scratchWidthOrFactionOffset * 6 + -4].stateMask |
             scratchCell[scratchWidthOrFactionOffset * 6].stateMask) & 0xf0000000) == 0)) {
          scratchCellOrigin = scratchCell + scratchWidthOrFactionOffset * -3;
          scratchCell = scratchCellOrigin + scratchWidthOrFactionOffset * 3;
          if (((scratchCellOrigin->stateMask | scratchCellOrigin[8].stateMask | scratchCellOrigin[scratchWidthOrFactionOffset * 6 + -8].stateMask |
                scratchCellOrigin[scratchWidthOrFactionOffset * 6 + 8].stateMask | scratchCellOrigin[scratchWidthOrFactionOffset * 0xc + -8].stateMask |
               scratchCellOrigin[scratchWidthOrFactionOffset * 0xc].stateMask) & 0xf0000000) == 0) {
            fieldCell = (FieldGridCell *)(cellByteCursor + (rowStrideOrClassId - factionIndex));
            if ((fieldCell->flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) != 0) {
              AiSiteCandidate_AddTerrainFeatureCellIfSeparated(fieldCell,spacingOrPanelIndex);
            }
            cellByteCursor = fieldCell[-widthOrCount].runtime0C_3F + factionIndex + -0xc;
          }
        }
        if (((scratchCell->stateMask | scratchCell[4].stateMask | scratchCell[scratchWidthOrFactionOffset * 3 + -4].stateMask |
              scratchCell[scratchWidthOrFactionOffset * 3 + 4].stateMask | scratchCell[scratchWidthOrFactionOffset * 6 + -4].stateMask |
             scratchCell[scratchWidthOrFactionOffset * 6].stateMask) & 0xf0000000) == 0) {
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
            AiEntityCandidateWorkspace09_AddInsidePrimaryExtents(fieldCell + widthOrCount);
            fieldCell = fieldCell + widthOrCount + -widthOrCount;
          }
          cellByteCursor = fieldCell->runtime0C_3F + factionIndex + -0xc;
        }
        if (((scratchCell[scratchWidthOrFactionOffset * 3].stateMask & 0xf000ff00) == 0) &&
           (((scratchCell->stateMask | scratchCell[4].stateMask | scratchCell[scratchWidthOrFactionOffset * 3 + -4].stateMask |
              scratchCell[scratchWidthOrFactionOffset * 3 + 4].stateMask | scratchCell[scratchWidthOrFactionOffset * 6 + -4].stateMask |
             scratchCell[scratchWidthOrFactionOffset * 6].stateMask) & 0xf0000000) == 0)) {
          scratchCellOrigin = scratchCell + scratchWidthOrFactionOffset * -3;
          scratchCell = scratchCellOrigin + scratchWidthOrFactionOffset * 3;
          if (((scratchCellOrigin->stateMask | scratchCellOrigin[8].stateMask | scratchCellOrigin[scratchWidthOrFactionOffset * 6 + -8].stateMask |
                scratchCellOrigin[scratchWidthOrFactionOffset * 6 + 8].stateMask | scratchCellOrigin[scratchWidthOrFactionOffset * 0xc + -8].stateMask |
               scratchCellOrigin[scratchWidthOrFactionOffset * 0xc].stateMask) &
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
              AiEntityCandidateWorkspace10_AddInsidePrimaryExtents(fieldCell + widthOrCount);
              fieldCell = fieldCell + widthOrCount + -widthOrCount;
            }
            cellByteCursor = fieldCell->runtime0C_3F + factionIndex + -0xc;
          }
        }
      }
      cellByteCursor = cellByteCursor + 0x80;
      scratchCell = scratchCell + 4;
      countOrMask--;
    } while (countOrMask != 0);
    scratchCell = scratchCell + g_GridScratchWidth * 3;
    countOrMask = widthOrCount & 0x1ffffff;
    remainingOrClassRecord--;
  } while (remainingOrClassRecord != 0);
  /* production mask of the own structures: class 0x0D contributes its mask at +0xC4, class 0x16 bit 3,
     class 0x0B bit 4 */
  countOrMask = 0;
  primaryEntryCursor = g_AiWorkspaceBuffer00_Size0400;
  for (widthOrCount = g_AiWorkspace00Count; widthOrCount != 0; widthOrCount--) {
    if ((int *)primaryEntryCursor->runtimeSlotAddressOrZero != NULL) {
      remainingOrClassRecord = *(int *)primaryEntryCursor->runtimeSlotAddressOrZero;
      rowStrideOrClassId = *(int *)(remainingOrClassRecord + 0x4c);
      if (rowStrideOrClassId == 0xd) {
        countOrMask = countOrMask | *(uint32_t *)(remainingOrClassRecord + 0xc4);
      }
      else if (rowStrideOrClassId == 0x16) {
        countOrMask = countOrMask | 8;
      }
      else if (rowStrideOrClassId == 0xb) {
        countOrMask = countOrMask | 0x10;
      }
    }
    primaryEntryCursor++;
  }
  /* workspace 11: registry assets that are enabled (bit 0), fully unlocked and producible (production mask) */
  armyAssetRegistryCursor = g_ArmyAssetRecordRegistry;
  remainingOrClassRecord = ARMY_ASSET_REGISTRY_SLOT_COUNT;
  workspace11Cursor = g_AiWorkspaceBuffer11_Size1000;
  do {
    definitionNode = *armyAssetRegistryCursor;
    if (((definitionNode != NULL) &&
        ((definitionNode[1].selectionDetailTemplateVariantIndex & 1) != 0)) &&
       (((testResult = ModelDefinitionHierarchy_AllTechnologyUnlockedForFaction
                             (factionIndex,(ModelDefinitionHierarchyNodeAddress32)definitionNode),
         !testResult && ((definitionNode[1].selectionDetailTemplateVariantIndex & countOrMask) != 0)) &&
        (((definitionNode->registryId < ARM_0300_BUILDING_MDL0301 ||
          ((ARM_0340_BUILDING_MDL0314 - 1) < definitionNode->registryId)) &&
         (g_AiWorkspace11Count < 0x400)))))) {
      *workspace11Cursor = definitionNode;
      g_AiWorkspace11Count++;
      workspace11Cursor++;
    }
    armyAssetRegistryCursor++;
    remainingOrClassRecord--;
    runtimeEntryCursor = g_AiWorkspaceBuffer02_Size0400;
    widthOrCount = g_AiWorkspace02Count;
    targetEntry = g_AiWorkspaceBuffer07_Size0400;
  } while (remainingOrClassRecord != 0);
  while ((widthOrCount != 0 &&
         (armySlot = runtimeEntryCursor->armyRuntime, g_AiWorkspace07Count < 0x40))) {
    if (armySlot != NULL) {
      targetEntry->armyRuntime = armySlot;
      modelNode = armySlot->modelNodeRuntime;
      translationX = modelNode->worldTransform.translation.x;
      translationY = modelNode->worldTransform.translation.y;
      targetEntry->modelRuntime = modelNode;
      targetEntry->worldXQ12 = translationX;
      targetEntry->worldYQ12 = translationY;
      g_AiWorkspace07Count++;
      targetEntry++;
    }
    runtimeEntryCursor++;
    widthOrCount--;
  }
  /* technology candidates: dwords 28 down to 1 of the technology table that the typed view reaches as
     attachments140[4].sourceTransform04 in each own structure's model */
  scratchWidthOrFactionOffset = factionIndex * 0x740; /* byte offset of the faction's runtime record */
  primaryEntryCursor = g_AiWorkspaceBuffer00_Size0400;
  for (countOrMask = g_AiWorkspace00Count; countOrMask != 0; countOrMask--) {
    armySlot = (ArmyRuntimeSlot *)primaryEntryCursor->runtimeSlotAddressOrZero;
    if (armySlot != NULL) {
      slotModelRuntime = armySlot->modelRuntimeOrSavedOffset.modelRuntime;
      spacingOrPanelIndex = 28;
      do {
        testResult = AiTechnologyCandidate_IsCurrentlyAvailable
                           ((PckTechnologyIdCatalog)
                            (&slotModelRuntime->attachments140[4].sourceTransform04)[spacingOrPanelIndex],scratchWidthOrFactionOffset);
        if (!testResult) {
          registerContinuity = AiTechnologyPlanning_AddCandidateRecord
                             (spacingOrPanelIndex,countOrMask,scratchWidthOrFactionOffset,armySlot,
                              (PckTechnologyIdCatalog)
                              (&slotModelRuntime->attachments140[4].sourceTransform04)[spacingOrPanelIndex]);
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
void __thandor_void_preserve_eax_ecx_edx
AiStrategicCandidate_AddBestWorkspace12Entry
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

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
      candidateCursor = g_AiWorkspaceBuffer12_Size0200;
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
void __thandor_void_preserve_eax_ecx_edx AiCandidateWorkspace_Clear(void)

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
  
  candidateWorkspaceSourceCursor = &g_AiWorkspaceBuffer13_Size0400->weightedScoreAndKind;
  entryCountOrDwordsRemaining = g_AiCandidateWorkspaceEntryCount;
  if (2 < g_AiCandidateWorkspaceEntryCount) {
    entryCountOrDwordsRemaining = 3;
  }
  *(int *)((int)(g_GameFactionRuntimeImage.records[0].candidateCache.savedEntries + 3) +
          factionImageByteOffset) = entryCountOrDwordsRemaining;
  factionImageDestinationCursor =
       (uint32_t *)((int)&g_GameFactionRuntimeImage.records[0].candidateCache.savedEntries[0].
                       weightedScoreAndKind + factionImageByteOffset);
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
void __thandor_void_preserve_eax_ecx_edx
AiCandidateWorkspace_LoadFromFactionImage(FactionImageByteOffset factionImageByteOffset)

{
  int copyDwordsRemaining;
  uint32_t *factionImageSourceCursor;
  uint32_t *candidateWorkspaceDestinationCursor;
  
  g_AiCandidateWorkspaceEntryCount =
       *(int *)((int)(g_GameFactionRuntimeImage.records[0].candidateCache.savedEntries + 3) +
               factionImageByteOffset);
  factionImageSourceCursor =
       (uint32_t *)((int)&g_GameFactionRuntimeImage.records[0].candidateCache.savedEntries[0].
                       weightedScoreAndKind + factionImageByteOffset);
  copyDwordsRemaining = g_AiCandidateWorkspaceEntryCount * 2;
  candidateWorkspaceDestinationCursor = &g_AiWorkspaceBuffer13_Size0400->weightedScoreAndKind;
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
void __thandor_void_preserve_eax_ecx_edx AiCandidateWorkspace_SortDescending(void)

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
    currentRecordScore = g_AiWorkspaceBuffer13_Size0400->weightedScoreAndKind;
    currentRecordPayload = g_AiWorkspaceBuffer13_Size0400->entityIdAndMultiplicity;
    scanRecordCursor = g_AiWorkspaceBuffer13_Size0400 + 1;
    comparisonsRemaining = g_AiCandidateWorkspaceEntryCount - 1;
    recordsInCurrentPass = g_AiCandidateWorkspaceEntryCount;
    currentRecordCursor = g_AiWorkspaceBuffer13_Size0400;
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
int __thandor_eax_preserve_ecx_edx
AiCandidateWorkspace_GetEntryXeniteCost(AiCandidateWorkspaceEntry *entry)

{
  uint32_t xeniteCostQ4;
  RuntimeToken registryId;
  ArmyAssetLookupResult registryLookup;

  registryId = entry->entityIdAndMultiplicity & AI_CANDIDATE_ID_MASK;
  if ((entry->weightedScoreAndKind & AI_CANDIDATE_KIND_MASK) == AI_CANDIDATE_KIND_TECHNOLOGY) {
    xeniteCostQ4 = g_TechnologyAsset->records[registryId].xeniteCostQ4;
  }
  else {
    registryLookup = ArmyAssetRegistry_FindById(registryId);
    xeniteCostQ4 = 0x7fffffff;
    if (!registryLookup.notFound) {
      /* +0x28 of the army asset record, reached through the 16-byte prefix type */
      xeniteCostQ4 = registryLookup.recordOrError[2].registryId;
    }
  }
  return xeniteCostQ4;
}


/* Address: 0x00538C90.
   Ownership: gameplay/ai/workspaces.
   Purpose: Recovered missed predicate symmetric to the primary workspace helper. It scans secondary eight-byte
   entries and sets CF for a matching ID with zero assignment. Typed parameters: p0 entryId→RuntimeToken. Nearby
   but non-identical semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes,
   control flow, globals, locals, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
AiSecondaryWorkspace_HasUnassignedEntryById(PckArmyAssetIdCatalog entryId)

{
  int workspaceEntriesRemaining;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  
  workspaceEntriesRemaining = g_AiWorkspace01Count;
  workspaceEntryCursor = g_AiWorkspaceBuffer01_Size0200;
  while( true ) {
    if (workspaceEntriesRemaining == 0) {
      return false;
    }
    if ((entryId == workspaceEntryCursor->armyAssetId) &&
       (workspaceEntryCursor->armyRuntime == (ArmyRuntimeSlot *)0x0)) break;
    workspaceEntryCursor = workspaceEntryCursor + 1;
    workspaceEntriesRemaining = workspaceEntriesRemaining + -1;
  }
  return true;
}


/* Address: 0x00538CF0.
   Returns true (CF set) when the secondary workspace (workspace 01) holds an entry of this army asset, assigned
   or not.
*/
bool __thandor_cf_preserve_eax_ecx_edx
AiSecondaryWorkspace_HasEntryById(PckArmyAssetIdCatalog entryId)

{
  int workspaceEntriesRemaining;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;

  workspaceEntriesRemaining = g_AiWorkspace01Count;
  workspaceEntryCursor = g_AiWorkspaceBuffer01_Size0200;
  while( true ) {
    if (workspaceEntriesRemaining == 0) {
      return false;
    }
    if (entryId == workspaceEntryCursor->armyAssetId) break;
    workspaceEntryCursor++;
    workspaceEntriesRemaining--;
  }
  return true;
}


/* Address: 0x00538D40.
   Ownership: gameplay/ai/workspaces.
   Purpose: Semantic ABI remains deferred.
*/
int __thandor_eax_preserve_ecx_edx
AiPrimaryWorkspace_CountAssignedEntriesByIdDuplicate(PckArmyAssetIdCatalog entryId)

{
  int matchingAssignedEntryCount;
  int workspaceEntriesRemaining;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  
  matchingAssignedEntryCount = 0;
  workspaceEntryCursor = (AiRuntimeWorkspaceEntry *)g_AiWorkspaceBuffer00_Size0400;
  for (workspaceEntriesRemaining = g_AiWorkspace00Count; workspaceEntriesRemaining != 0;
      workspaceEntriesRemaining = workspaceEntriesRemaining + -1) {
    if ((workspaceEntryCursor->armyRuntime != (ArmyRuntimeSlot *)0x0) &&
       (entryId == workspaceEntryCursor->armyAssetId)) {
      matchingAssignedEntryCount = matchingAssignedEntryCount + 1;
    }
    workspaceEntryCursor = workspaceEntryCursor + 1;
  }
  return matchingAssignedEntryCount;
}


/* Address: 0x00538D90.
   Returns the smallest Manhattan distance from the point to an assigned secondary-workspace (workspace 01)
   entry, measured to the linked entity's path coordinates, or 0x7FFFFFFF when there is none.
   Arguments are Y first, then X, as every caller passes them.
*/
int __thandor_eax_preserve_ecx_edx
AiSecondaryWorkspace_GetMinimumManhattanDistanceToPoint(Q12 worldY,Q12 worldX)

{
  Q12 minimumManhattanDistanceQ12;
  int workspaceEntriesRemaining;
  int deltaYAbsQ12;
  Q12 deltaXAbsQ12;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  GameEntityRuntime *entityRuntime;
  
  minimumManhattanDistanceQ12 = 0x7fffffff;
  workspaceEntryCursor = g_AiWorkspaceBuffer01_Size0200;
  for (workspaceEntriesRemaining = g_AiWorkspace01Count; workspaceEntriesRemaining != 0;
      workspaceEntriesRemaining--) {
    if (workspaceEntryCursor->armyRuntime != NULL) {
      entityRuntime = workspaceEntryCursor->armyRuntime->linkedEntityRuntime;
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
int __thandor_eax_preserve_ecx_edx
AiPrimaryWorkspace_GetMinimumActiveManhattanDistanceToPoint(Q12 worldY,Q12 worldX)

{
  Q12 minimumActiveManhattanDistanceQ12;
  int workspaceEntriesRemaining;
  int deltaYAbsQ12;
  Q12 deltaXAbsQ12;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  ArmyRuntimeSlot *armySlot;
  
  minimumActiveManhattanDistanceQ12 = 0x7fffffff;
  workspaceEntryCursor = (AiRuntimeWorkspaceEntry *)g_AiWorkspaceBuffer00_Size0400;
  for (workspaceEntriesRemaining = g_AiWorkspace00Count; workspaceEntriesRemaining != 0;
      workspaceEntriesRemaining--) {
    armySlot = workspaceEntryCursor->armyRuntime;
    if ((armySlot != NULL) &&
       ((armySlot->linkedEntityRuntime->common).commandState != 0)) {
      deltaXAbsQ12 = worldX - (armySlot->modelNodeRuntime->worldTransform).translation.x;
      if (deltaXAbsQ12 < 0) {
        deltaXAbsQ12 = -deltaXAbsQ12;
      }
      deltaYAbsQ12 = worldY - (armySlot->modelNodeRuntime->worldTransform).translation.y;
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
int __thandor_eax_preserve_ecx_edx
AiWorkspace02_GetMinimumManhattanDistanceToPoint(Q12 worldY,Q12 worldX)

{
  Q12 minimumManhattanDistanceQ12;
  int workspaceEntriesRemaining;
  int deltaYAbsQ12;
  Q12 deltaXAbsQ12;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  ModelRuntimeNode *modelNode;
  
  minimumManhattanDistanceQ12 = 0x7fffffff;
  workspaceEntryCursor = g_AiWorkspaceBuffer02_Size0400;
  for (workspaceEntriesRemaining = g_AiWorkspace02Count; workspaceEntriesRemaining != 0;
      workspaceEntriesRemaining--) {
    if (workspaceEntryCursor->armyRuntime != NULL) {
      modelNode = workspaceEntryCursor->armyRuntime->modelNodeRuntime;
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
int __thandor_eax_preserve_ecx_edx
AiWorkspace03_GetMinimumManhattanDistanceToPoint(Q12 worldY,Q12 worldX)

{
  Q12 minimumManhattanDistanceQ12;
  int workspaceEntriesRemaining;
  int deltaYAbsQ12;
  Q12 deltaXAbsQ12;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  ModelRuntimeNode *modelNode;
  
  minimumManhattanDistanceQ12 = 0x7fffffff;
  workspaceEntryCursor = g_AiWorkspaceBuffer03_Size1000;
  for (workspaceEntriesRemaining = g_AiWorkspace03Count; workspaceEntriesRemaining != 0;
      workspaceEntriesRemaining--) {
    if (workspaceEntryCursor->armyRuntime != NULL) {
      modelNode = workspaceEntryCursor->armyRuntime->modelNodeRuntime;
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
int __thandor_eax_preserve_ecx_edx
AiPrimaryWorkspace_GetMinimumManhattanDistanceToPoint(Q12 worldY,Q12 worldX)

{
  Q12 minimumManhattanDistanceQ12;
  int workspaceEntriesRemaining;
  int deltaYAbsQ12;
  Q12 deltaXAbsQ12;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  ModelRuntimeNode *modelNode;
  
  minimumManhattanDistanceQ12 = 0x7fffffff;
  workspaceEntryCursor = (AiRuntimeWorkspaceEntry *)g_AiWorkspaceBuffer00_Size0400;
  for (workspaceEntriesRemaining = g_AiWorkspace00Count; workspaceEntriesRemaining != 0;
      workspaceEntriesRemaining--) {
    if (workspaceEntryCursor->armyRuntime != NULL) {
      modelNode = workspaceEntryCursor->armyRuntime->modelNodeRuntime;
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
void __thandor_void_preserve_eax_ecx_edx
AiConstructionPlanner_PlaceSpecialAssetFromWorkspace
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
  terrainFeatureEntry = g_AiWorkspaceBuffer08_Size0200;
  do {
    if (recordsRemaining == 0) {
      return;
    }
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
        ArmyRuntime_DispatchClassCommand(createdSlotPair,worldRuntime);
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
        return;
      }
    }
    terrainFeatureEntry++;
    recordsRemaining--;
  } while( true );
}


/* Address: 0x0053BCB0.
   Ownership: gameplay/ai/workspaces.
   Purpose: Common stdcall stack ABI: FactionRuntimeIndex, TechnologyId, WorldRuntimeContext*. Signed score returns
   in EAX. Target group: stack-only callback target. Exact binary and live ownership are preflight locked.
*/
AiTechnologyCandidateScore __thandor_eax_preserve_ecx_edx
AiWorkspace12Score_DefaultZero
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
StatusResult __thandor_eax_cf_preserve_ecx_edx AiRuntime_InitWorkspace(void)

{
  uint8_t *workspaceAllocation;
  AiTechnologyPlanningCandidate *technologyCandidateWorkspaceAllocation;
  AiKnowledgeDataImage *knowledgeDataImage;
  PackageLoadResult loadResult;
  StatusResult initStatus;
  
  loadResult = THANDOR_BITCAST(ArenaAllocResult, PackageLoadResult, g_MemoryApi.alloc(0x400));
  workspaceAllocation = loadResult.bufferOrError;
  if (!loadResult.failed) {
    g_AiWorkspaceBuffer00_Size0400 = (AiWorkspace00EntryView8 *)workspaceAllocation;
    loadResult = THANDOR_BITCAST(ArenaAllocResult, PackageLoadResult, g_MemoryApi.alloc(0x200));
    if (!loadResult.failed) {
      g_AiWorkspaceBuffer01_Size0200 = loadResult.bufferOrError;
      loadResult = THANDOR_BITCAST(ArenaAllocResult, PackageLoadResult, g_MemoryApi.alloc(0x400));
      if (!loadResult.failed) {
        g_AiWorkspaceBuffer02_Size0400 = loadResult.bufferOrError;
        loadResult = THANDOR_BITCAST(ArenaAllocResult, PackageLoadResult, g_MemoryApi.alloc(0x1000));
        if (!loadResult.failed) {
          g_AiWorkspaceBuffer03_Size1000 = loadResult.bufferOrError;
          loadResult = THANDOR_BITCAST(ArenaAllocResult, PackageLoadResult, g_MemoryApi.alloc(0x40));
          if (!loadResult.failed) {
            g_AiWorkspaceBuffer04_Size0040 = loadResult.bufferOrError;
            loadResult = THANDOR_BITCAST(ArenaAllocResult, PackageLoadResult, g_MemoryApi.alloc(0x200));
            if (!loadResult.failed) {
              g_AiWorkspaceBuffer05_Size0200 = loadResult.bufferOrError;
              loadResult = THANDOR_BITCAST(ArenaAllocResult, PackageLoadResult, g_MemoryApi.alloc(0x400));
              if (!loadResult.failed) {
                g_AiWorkspaceBuffer06_Size0400 = loadResult.bufferOrError;
                loadResult = THANDOR_BITCAST(ArenaAllocResult, PackageLoadResult, g_MemoryApi.alloc(0x400));
                if (!loadResult.failed) {
                  g_AiWorkspaceBuffer07_Size0400 = loadResult.bufferOrError;
                  loadResult = THANDOR_BITCAST(ArenaAllocResult, PackageLoadResult, g_MemoryApi.alloc(0x200));
                  if (!loadResult.failed) {
                    g_AiWorkspaceBuffer08_Size0200 = loadResult.bufferOrError;
                    loadResult = THANDOR_BITCAST(ArenaAllocResult, PackageLoadResult, g_MemoryApi.alloc(0x1000));
                    if (!loadResult.failed) {
                      g_AiWorkspaceBuffer09_Size1000 = loadResult.bufferOrError;
                      loadResult = THANDOR_BITCAST(ArenaAllocResult, PackageLoadResult, g_MemoryApi.alloc(0x400));
                      if (!loadResult.failed) {
                        g_AiWorkspaceBuffer10_Size0400 = loadResult.bufferOrError;
                        loadResult = THANDOR_BITCAST(ArenaAllocResult, PackageLoadResult, g_MemoryApi.alloc(0x1000));
                        if (!loadResult.failed) {
                          g_AiWorkspaceBuffer11_Size1000 = loadResult.bufferOrError;
                          loadResult = THANDOR_BITCAST(ArenaAllocResult, PackageLoadResult, g_MemoryApi.alloc(0x200));
                          technologyCandidateWorkspaceAllocation = loadResult.bufferOrError;
                          if (!loadResult.failed) {
                            g_AiWorkspaceBuffer12_Size0200 = technologyCandidateWorkspaceAllocation;
                            loadResult = THANDOR_BITCAST(ArenaAllocResult, PackageLoadResult, g_MemoryApi.alloc(0x400));
                            if (!loadResult.failed) {
                              g_AiWorkspaceBuffer13_Size0400 = loadResult.bufferOrError;
                              loadResult = THANDOR_BITCAST(ArenaAllocResult, PackageLoadResult, g_MemoryApi.alloc(0x100));
                              if (!loadResult.failed) {
                                g_AiWorkspaceBuffer14_Size0100 = loadResult.bufferOrError;
                                loadResult = Package_LoadEntry((uint16_t *)u_engine_ki_dat_0053c5e4);
                                knowledgeDataImage = loadResult.bufferOrError;
                                if (!loadResult.failed) {
                                  /* success: EAX (the image pointer) stays the result value, CF clear */
                                  loadResult = THANDOR_BITCAST(uint64_t, PackageLoadResult, ((THANDOR_BITCAST(PackageLoadResult, uint64_t, loadResult) & 0xFFFFFFFFFFull) & 0xffffffff));
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
void __thandor_void_preserve_ecx_edx
AiEntityCandidateWorkspace09_AddInsidePrimaryExtents(FieldGridCell *currentCell)

{
  FieldGridCell **cellBuffer;
  uint32_t entryIndex;
  bool isOutsideExtents;

  entryIndex = g_AiWorkspace09Count;
  cellBuffer = g_AiWorkspaceBuffer09_Size1000;
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
   Same as the workspace-09 variant for workspace 10 (at most 256 cells).
*/
void __thandor_void_preserve_ecx_edx
AiEntityCandidateWorkspace10_AddInsidePrimaryExtents(FieldGridCell *currentCell)

{
  FieldGridCell **cellBuffer;
  uint32_t entryIndex;
  bool isOutsideExtents;

  entryIndex = g_AiWorkspace10Count;
  cellBuffer = g_AiWorkspaceBuffer10_Size0400;
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
bool __thandor_cf_preserve_eax_ecx_edx
AiPrimaryWorkspace_HasUnassignedEntryById(PckArmyAssetIdCatalog entryId)

{
  int workspaceEntriesRemaining;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  
  workspaceEntriesRemaining = g_AiWorkspace00Count;
  workspaceEntryCursor = (AiRuntimeWorkspaceEntry *)g_AiWorkspaceBuffer00_Size0400;
  while( true ) {
    if (workspaceEntriesRemaining == 0) {
      return false;
    }
    if ((entryId == workspaceEntryCursor->armyAssetId) &&
       (workspaceEntryCursor->armyRuntime == NULL)) break;
    workspaceEntryCursor++;
    workspaceEntriesRemaining--;
  }
  return true;
}


/* Address: 0x00538BF0.
   Returns true (CF set) when the primary workspace (workspace 00) holds an entry of this army asset, with or
   without a runtime object.
*/
bool __thandor_cf_preserve_eax_ecx_edx
AiPrimaryWorkspace_HasEntryById(PckArmyAssetIdCatalog entryId)

{
  int workspaceEntriesRemaining;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;

  workspaceEntriesRemaining = g_AiWorkspace00Count;
  workspaceEntryCursor = (AiRuntimeWorkspaceEntry *)g_AiWorkspaceBuffer00_Size0400;
  while( true ) {
    if (workspaceEntriesRemaining == 0) {
      return false;
    }
    if (entryId == workspaceEntryCursor->armyAssetId) break;
    workspaceEntryCursor++;
    workspaceEntriesRemaining--;
  }
  return true;
}


/* Address: 0x00538C40.
   Counts the primary-workspace (workspace 00) entries of this army asset that have a runtime object.
*/
int __thandor_eax_preserve_ecx_edx
AiPrimaryWorkspace_CountAssignedEntriesById(PckArmyAssetIdCatalog entryId)

{
  int matchingAssignedEntryCount;
  int workspaceEntriesRemaining;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;

  matchingAssignedEntryCount = 0;
  workspaceEntryCursor = (AiRuntimeWorkspaceEntry *)g_AiWorkspaceBuffer00_Size0400;
  for (workspaceEntriesRemaining = g_AiWorkspace00Count; workspaceEntriesRemaining != 0;
      workspaceEntriesRemaining--) {
    if ((workspaceEntryCursor->armyRuntime != NULL) &&
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
void __thandor_void_preserve_eax_ecx_edx
AiCandidateWorkspace_AddOrAccumulateWeightedEntry
          (RuntimeToken entityId,uint32_t weightRange,AiCandidateEntryKind entryKind)

{
  uint32_t newEntryIndex;
  uint32_t randomValue;
  int weightedScore;
  uint32_t entriesRemaining;
  AiCandidateWorkspaceEntry *candidateEntry;
  
  randomValue = g_RandomGeneratorState.next();
  newEntryIndex = g_AiCandidateWorkspaceEntryCount;
  candidateEntry = g_AiWorkspaceBuffer13_Size0400;
  if (1 < weightRange) {
    weightedScore = weightRange * 5 + randomValue % weightRange;
    /* searched from the last entry down */
    for (entriesRemaining = g_AiCandidateWorkspaceEntryCount; entriesRemaining != 0; entriesRemaining--) {
      if (((g_AiWorkspaceBuffer13_Size0400[entriesRemaining - 1].weightedScoreAndKind & AI_CANDIDATE_KIND_MASK) ==
           entryKind) &&
         ((g_AiWorkspaceBuffer13_Size0400[entriesRemaining - 1].entityIdAndMultiplicity & AI_CANDIDATE_ID_MASK) ==
          entityId))
      {
        g_AiWorkspaceBuffer13_Size0400[entriesRemaining - 1].entityIdAndMultiplicity =
             g_AiWorkspaceBuffer13_Size0400[entriesRemaining - 1].entityIdAndMultiplicity +
             AI_CANDIDATE_MULTIPLICITY_ONE;
        candidateEntry = candidateEntry + (entriesRemaining - 1);
        /* * 0x10: the score sits above the 4 kind bits */
        candidateEntry->weightedScoreAndKind = candidateEntry->weightedScoreAndKind + weightedScore * 0x10;
        return;
      }
    }
    if (g_AiCandidateWorkspaceEntryCount < AI_CANDIDATE_WORKSPACE_CAPACITY) {
      g_AiWorkspaceBuffer13_Size0400[g_AiCandidateWorkspaceEntryCount].entityIdAndMultiplicity =
           entityId + AI_CANDIDATE_MULTIPLICITY_ONE;
      g_AiCandidateWorkspaceEntryCount++;
      candidateEntry[newEntryIndex].weightedScoreAndKind = weightedScore * 0x10 | entryKind;
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
bool __thandor_void_preserve_ecx_edx
AiPrimaryWorkspace_IsPointOutsideAllEntryExtents(Q12 worldY,Q12 worldX)

{
  Q12 deltaXAbsQ12;
  int workspaceEntriesRemaining;
  Q12 deltaYAbsQ12;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  int *workspaceEntryEntityRecord;
  
  workspaceEntriesRemaining = g_AiWorkspace00Count;
  workspaceEntryCursor = (AiRuntimeWorkspaceEntry *)g_AiWorkspaceBuffer00_Size0400;
  do {
    if (workspaceEntriesRemaining == 0) {
      return true;
    }
    /* raw view of the ArmyRuntimeSlot: [0] is its definition, [1] its model node (translation.x at +0x94,
       translation.y at +0x98) */
    workspaceEntryEntityRecord = (int *)workspaceEntryCursor->armyRuntime;
    if (workspaceEntryEntityRecord != NULL) {
      deltaXAbsQ12 = worldX - *(int *)(workspaceEntryEntityRecord[1] + 0x94);
      if (deltaXAbsQ12 < 0) {
        deltaXAbsQ12 = -deltaXAbsQ12;
      }
      deltaYAbsQ12 = worldY - *(int *)(workspaceEntryEntityRecord[1] + 0x98);
      if (deltaYAbsQ12 < 0) {
        deltaYAbsQ12 = -deltaYAbsQ12;
      }
      if ((deltaXAbsQ12 < *(int *)(*workspaceEntryEntityRecord + 0x19c)) &&
         (deltaYAbsQ12 < *(int *)(*workspaceEntryEntityRecord + 0x19c))) {
        return false;
      }
    }
    workspaceEntryCursor++;
    workspaceEntriesRemaining--;
  } while( true );
}

