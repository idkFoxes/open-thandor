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
   Ownership: gameplay/ai/workspaces.
   Purpose: Finds a matching workspace08 record, verifies mode-four placement, and adds a weighted asset candidate.
   The weight is reduced by assigned-count pressure and scaled by faction resource state for non-0x14A assets.
   Stock ARM contains 675 records and 326 unique ids; placement workspace, producer, tier, class, and faction-role
   semantics are not inferred from numeric adjacency. Typed parameters: p2 baseWeight→AiCandidateScore32_V342.
   Calling convention, complete VariableStorage serialization, function bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Local calls: AiPrimaryWorkspace_HasUnassignedEntryByIdCf, AiPrimaryWorkspace_CountAssignedEntriesById,
   AiCandidateWorkspace_AddOrAccumulateWeightedEntry.
   Cross-module calls: AiPlacement_TestMode4AtWorkspaceRecord [gameplay/ai/placement].
*/
void __thandor_void_preserve_eax_ecx_edx
AiWorkspaceAssetCandidate_AddWeightedEntry
          (AiCandidateScore32 baseWeight,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  int remainingOrCountOrRate;
  dword weightRange;
  AiTerrainFeatureWorkspaceEntry *terrainFeatureEntry;
  bool testResult;
  
  remainingOrCountOrRate = g_AiWorkspace08Count;
  terrainFeatureEntry = g_AiWorkspaceBuffer08_Size0200;
  if ((g_AiWorkspace08Count != 0) &&
     (testResult = AiPrimaryWorkspace_HasUnassignedEntryByIdCf(armyAssetId), !testResult)) {
    do {
      if ((armyAssetId == terrainFeatureEntry->armyAssetId) &&
         (testResult = AiPlacement_TestMode4AtWorkspaceRecord
                            (armyAssetId,terrainFeatureEntry->cell,factionIndex,worldRuntime),
         !testResult)) {
        remainingOrCountOrRate = AiPrimaryWorkspace_CountAssignedEntriesById(armyAssetId);
        weightRange = (uint)(baseWeight * 3) / (remainingOrCountOrRate + 3U);
        if (armyAssetId != ARM_0330_BUILDING_MDL0303) {
          remainingOrCountOrRate = g_GameFactionRuntimeImage.records[factionIndex].tritiumExtractionRateQ4PerTick <<
                  4;
          if (remainingOrCountOrRate != 0) {
            weightRange = (dword)(((longlong)(int)weightRange *
                                  (longlong)
                                  (int)(g_GameFactionRuntimeImage.records[factionIndex].
                                        unpoweredEnergyDemandQ4 * 2 +
                                       g_GameFactionRuntimeImage.records[factionIndex].
                                       suppliedEnergyDemandQ4)) / (longlong)remainingOrCountOrRate);
          }
        }
        AiCandidateWorkspace_AddOrAccumulateWeightedEntry(armyAssetId,weightRange,1);
        return;
      }
      terrainFeatureEntry = terrainFeatureEntry + 1;
      remainingOrCountOrRate = remainingOrCountOrRate + -1;
    } while (remainingOrCountOrRate != 0);
  }
  return;
}


/* Address: 0x00538230.
   Ownership: gameplay/ai/workspaces.
   Purpose: Clears and rebuilds the faction AI workspaces from live entities, faction lists, field-grid conditions,
   army definitions, placement candidates, and currently available technologies. It populates all verified
   workspace counts and candidate buffers used by subsequent AI decision passes. Stock tech.tec has 512 records
   over canonical ids 0..255; localized titles do not prove source-building, tier, direction, or effect mappings.
   Local calls: AiEntityCandidateWorkspace09_AddOutsidePrimaryExtents,
   AiEntityCandidateWorkspace10_AddOutsidePrimaryExtents.
   Cross-module calls: GameFactionRuntime_TestCapabilityBitClearCf [gameplay/faction/runtime],
   AiPlanning_CollectActiveGridMaskClasses [gameplay/ai/planning], AiSiteCandidate_AddGeneralCellIfSeparated
   [gameplay/ai/placement], AiSiteCandidate_AddFlaggedCellIfSeparated [gameplay/ai/placement],
   AiSiteCandidate_AddTerrainFeatureCellIfSeparated [gameplay/ai/placement],
   ModelDefinitionHierarchy_AllTechnologyUnlockedForFactionCf [assets/model/definitions].
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
  uint countSnapshotOrRemaining;
  AiRuntimeWorkspaceEntry *workspace03Buffer;
  GridScratchStateMask neighborhoodMask;
  FactionArmyAssetCount armyAssetsRemaining;
  int remainingOrClassRecord;
  uint widthOrCount;
  dword spacingOrPanelIndex;
  uint countOrMask;
  dword scratchWidthOrFactionOffset;
  int rowStrideOrClassId;
  AiWorkspace00EntryView8 *primaryEntryCursor;
  FieldGridCell *fieldCell;
  byte *cellByteCursor;
  ArmyAssetRecordPrefix **workspace11Cursor;
  WorldOwnerListNode100 *worldNode;
  dword *armyAssetPointerCursor;
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
  g_AiWorkspaceOwnedAsset300Runtime = (ArmyRuntimeSlot *)0x0;
  if (worldNode != (WorldOwnerListNode100 *)0x0) {
    do {
      widthOrCount = g_AiWorkspace01Count;
      runtimeEntryCursor = g_AiWorkspaceBuffer01_Size0200;
      countOrMask = g_AiWorkspace00Count;
      primaryEntryCursor = g_AiWorkspaceBuffer00_Size0400;
      if (worldNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
        armySlot = worldNode->runtimePayload;
        entityRuntime = armySlot->linkedEntityRuntime;
        if (factionIndex == (entityRuntime->common).ownership.ownerIndex) {
          assetId = (entityRuntime->common).runtimeIdentityOrArmyAssetId;
          if (assetId < ARM_0300_BUILDING_MDL0301) {
            if (g_AiWorkspace01Count < 0x40) {
              g_AiWorkspaceBuffer01_Size0200[g_AiWorkspace01Count].armyRuntime = armySlot;
              runtimeEntryCursor[widthOrCount].armyAssetId = assetId;
              g_AiWorkspace01Count = g_AiWorkspace01Count + 1;
            }
          }
          else if (g_AiWorkspace00Count < 0x80) {
            g_AiWorkspaceBuffer00_Size0400[g_AiWorkspace00Count].runtimeSlotAddressOrZero =
                 (AiWorkspaceRuntimeSlotAddress32)armySlot;
            primaryEntryCursor[countOrMask].armyAssetId = assetId;
            g_AiWorkspace00Count = g_AiWorkspace00Count + 1;
            if (assetId == ARM_0300_BUILDING_MDL0301) {
              g_AiWorkspaceOwnedAsset300Runtime = armySlot;
            }
          }
        }
        else {
          countOrMask = *(uint *)((entityRuntime->common).damageState.reserved0C_23 + 0x10) >>
                   ((char)factionIndex * '\x02' & 0x1fU);
          if (((entityRuntime->common).ownership.ownerIndex != 0) &&
             (testResult = GameFactionRuntime_TestCapabilityBitClearCf
                                 ((entityRuntime->common).ownership.ownerIndex,factionIndex),
             countSnapshotOrRemaining = g_AiWorkspace03Count, workspace03Buffer = g_AiWorkspaceBuffer03_Size1000,
             widthOrCount = g_AiWorkspace02Count, runtimeEntryCursor = g_AiWorkspaceBuffer02_Size0400
             , testResult)) {
            if (((countOrMask & 2) == 0) &&
               (((countOrMask & 1) == 0 ||
                (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand
                 [*(int *)(*(int *)(entityRuntime->common).ownership.definitionOrClassRecord + 0x4c)] !=
                 ArmyRuntime_ClassCommandHandlerGroupACf)))) {
              assetId = (entityRuntime->common).runtimeIdentityOrArmyAssetId;
              if (g_AiWorkspace03Count < 0x200) {
                g_AiWorkspaceBuffer03_Size1000[g_AiWorkspace03Count].armyRuntime = armySlot;
                workspace03Buffer[countSnapshotOrRemaining].armyAssetId = assetId;
                g_AiWorkspace03Count = g_AiWorkspace03Count + 1;
              }
            }
            else {
              assetId = (entityRuntime->common).runtimeIdentityOrArmyAssetId;
              if (g_AiWorkspace02Count < 0x80) {
                g_AiWorkspaceBuffer02_Size0400[g_AiWorkspace02Count].armyRuntime = armySlot;
                runtimeEntryCursor[widthOrCount].armyAssetId = assetId;
                g_AiWorkspace02Count = g_AiWorkspace02Count + 1;
              }
            }
          }
        }
      }
      worldNode = worldNode->nextNode;
    } while (worldNode != (WorldOwnerListNode100 *)0x0);
    armyAssetPointerCursor = g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetPointersOrIds;
    primaryEntryCursor = g_AiWorkspaceBuffer00_Size0400;
    countOrMask = g_AiWorkspace00Count;
    for (armyAssetsRemaining = g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount; armyAssetsRemaining != 0
        ; armyAssetsRemaining = armyAssetsRemaining - 1) {
      assetId = *(PckArmyAssetIdCatalog *)(*armyAssetPointerCursor + 8);
      g_AiWorkspaceBuffer00_Size0400 = primaryEntryCursor;
      g_AiWorkspace00Count = countOrMask;
      if (countOrMask < 0x80) {
        primaryEntryCursor[countOrMask].runtimeSlotAddressOrZero = 0;
        primaryEntryCursor[countOrMask].armyAssetId = assetId;
        g_AiWorkspace00Count = g_AiWorkspace00Count + 1;
      }
      countOrMask = g_AiWorkspace04Count;
      runtimeEntryCursor = g_AiWorkspaceBuffer04_Size0040;
      if (g_AiWorkspace04Count < 8) {
        g_AiWorkspaceBuffer04_Size0040[g_AiWorkspace04Count].armyRuntime = (ArmyRuntimeSlot *)0x0;
        runtimeEntryCursor[countOrMask].armyAssetId = assetId;
        g_AiWorkspace04Count = g_AiWorkspace04Count + 1;
      }
      armyAssetPointerCursor = armyAssetPointerCursor + 1;
      primaryEntryCursor = g_AiWorkspaceBuffer00_Size0400;
      countOrMask = g_AiWorkspace00Count;
    }
    armyAssetPointerCursor = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetPointersOrIds;
    runtimeEntryCursor = g_AiWorkspaceBuffer01_Size0200;
    widthOrCount = g_AiWorkspace01Count;
    for (armyAssetsRemaining = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount;
        primaryBuffer = primaryEntryCursor, countSnapshotOrRemaining = countOrMask, armyAssetsRemaining != 0; armyAssetsRemaining = armyAssetsRemaining - 1) {
      assetId = *(PckArmyAssetIdCatalog *)(*armyAssetPointerCursor + 8);
      g_AiWorkspaceBuffer00_Size0400 = primaryEntryCursor;
      g_AiWorkspace00Count = countOrMask;
      g_AiWorkspaceBuffer01_Size0200 = runtimeEntryCursor;
      g_AiWorkspace01Count = widthOrCount;
      if (assetId < ARM_0300_BUILDING_MDL0301) {
        if (widthOrCount < 0x40) {
          runtimeEntryCursor[widthOrCount].armyRuntime = (ArmyRuntimeSlot *)0x0;
          runtimeEntryCursor[widthOrCount].armyAssetId = assetId;
          g_AiWorkspace01Count = g_AiWorkspace01Count + 1;
        }
      }
      else if (countOrMask < 0x80) {
        primaryEntryCursor[countOrMask].runtimeSlotAddressOrZero = 0;
        primaryEntryCursor[countOrMask].armyAssetId = assetId;
        g_AiWorkspace00Count = g_AiWorkspace00Count + 1;
      }
      armyAssetPointerCursor = armyAssetPointerCursor + 1;
      primaryEntryCursor = g_AiWorkspaceBuffer00_Size0400;
      countOrMask = g_AiWorkspace00Count;
      runtimeEntryCursor = g_AiWorkspaceBuffer01_Size0200;
      widthOrCount = g_AiWorkspace01Count;
    }
    for (; g_AiWorkspaceBuffer00_Size0400 = primaryBuffer, g_AiWorkspace00Count = countOrMask,
        g_AiWorkspaceBuffer01_Size0200 = runtimeEntryCursor, g_AiWorkspace01Count = widthOrCount,
        countSnapshotOrRemaining != 0; countSnapshotOrRemaining = countSnapshotOrRemaining - 1) {
      primarySlotWords = (int *)primaryEntryCursor->runtimeSlotAddressOrZero;
      if (primarySlotWords != (int *)0x0) {
        remainingOrClassRecord = *primarySlotWords;
        if (*(int *)(remainingOrClassRecord + 0x4c) == 0xb) {
          assetId = primarySlotWords[0x18];
          if ((primarySlotWords[0x2e] == 1) && (countOrMask < 0x80)) {
            primaryBuffer[countOrMask].runtimeSlotAddressOrZero = 0;
            primaryBuffer[countOrMask].armyAssetId = assetId;
            g_AiWorkspace00Count = g_AiWorkspace00Count + 1;
          }
        }
        else if (*(int *)(remainingOrClassRecord + 0x4c) == 0x16) {
          assetId = primarySlotWords[0x18];
          if ((primarySlotWords[0x2b] == 1) && (widthOrCount < 0x40)) {
            runtimeEntryCursor[widthOrCount].armyRuntime = (ArmyRuntimeSlot *)0x0;
            runtimeEntryCursor[widthOrCount].armyAssetId = assetId;
            g_AiWorkspace01Count = g_AiWorkspace01Count + 1;
          }
        }
        else if (((*(int *)(remainingOrClassRecord + 0x4c) == 0xd) && (assetId = primarySlotWords[0x18], primarySlotWords[0x2e] == 1)) &&
                (widthOrCount < 0x40)) {
          runtimeEntryCursor[widthOrCount].armyRuntime = (ArmyRuntimeSlot *)0x0;
          runtimeEntryCursor[widthOrCount].armyAssetId = assetId;
          g_AiWorkspace01Count = g_AiWorkspace01Count + 1;
        }
      }
      primaryEntryCursor = primaryEntryCursor + 1;
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
            AiEntityCandidateWorkspace09_AddOutsidePrimaryExtents(fieldCell + widthOrCount);
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
              AiEntityCandidateWorkspace10_AddOutsidePrimaryExtents(fieldCell + widthOrCount);
              fieldCell = fieldCell + widthOrCount + -widthOrCount;
            }
            cellByteCursor = fieldCell->runtime0C_3F + factionIndex + -0xc;
          }
        }
      }
      cellByteCursor = cellByteCursor + 0x80;
      scratchCell = scratchCell + 4;
      countOrMask = countOrMask - 1;
    } while (countOrMask != 0);
    scratchCell = scratchCell + g_GridScratchWidth * 3;
    countOrMask = widthOrCount & 0x1ffffff;
    remainingOrClassRecord = remainingOrClassRecord + -1;
  } while (remainingOrClassRecord != 0);
  countOrMask = 0;
  primaryEntryCursor = g_AiWorkspaceBuffer00_Size0400;
  for (widthOrCount = g_AiWorkspace00Count; widthOrCount != 0; widthOrCount = widthOrCount - 1) {
    if ((int *)primaryEntryCursor->runtimeSlotAddressOrZero != (int *)0x0) {
      remainingOrClassRecord = *(int *)primaryEntryCursor->runtimeSlotAddressOrZero;
      rowStrideOrClassId = *(int *)(remainingOrClassRecord + 0x4c);
      if (rowStrideOrClassId == 0xd) {
        countOrMask = countOrMask | *(uint *)(remainingOrClassRecord + 0xc4);
      }
      else if (rowStrideOrClassId == 0x16) {
        countOrMask = countOrMask | 8;
      }
      else if (rowStrideOrClassId == 0xb) {
        countOrMask = countOrMask | 0x10;
      }
    }
    primaryEntryCursor = primaryEntryCursor + 1;
  }
  armyAssetRegistryCursor = g_ArmyAssetRecordRegistry;
  remainingOrClassRecord = 0x300;
  workspace11Cursor = g_AiWorkspaceBuffer11_Size1000;
  do {
    definitionNode = *armyAssetRegistryCursor;
    if (((definitionNode != (ArmyAssetRecordPrefix *)0x0) &&
        ((definitionNode[1].selectionDetailTemplateVariantIndex & 1) != 0)) &&
       (((testResult = ModelDefinitionHierarchy_AllTechnologyUnlockedForFactionCf
                             (factionIndex,(ModelDefinitionHierarchyNodeAddress32)definitionNode),
         !testResult && ((definitionNode[1].selectionDetailTemplateVariantIndex & countOrMask) != 0)) &&
        (((definitionNode->registryId < ARM_0300_BUILDING_MDL0301 ||
          ((ARM_0320_BUILDING_MDL0311|ARM_0019_UNIT_MDL0101) < definitionNode->registryId)) &&
         (g_AiWorkspace11Count < 0x400)))))) {
      *workspace11Cursor = definitionNode;
      g_AiWorkspace11Count = g_AiWorkspace11Count + 1;
      workspace11Cursor = workspace11Cursor + 1;
    }
    armyAssetRegistryCursor = armyAssetRegistryCursor + 1;
    remainingOrClassRecord = remainingOrClassRecord + -1;
    runtimeEntryCursor = g_AiWorkspaceBuffer02_Size0400;
    widthOrCount = g_AiWorkspace02Count;
    targetEntry = g_AiWorkspaceBuffer07_Size0400;
  } while (remainingOrClassRecord != 0);
  while ((widthOrCount != 0 &&
         (armySlot = runtimeEntryCursor->armyRuntime, g_AiWorkspace07Count < 0x40))) {
    if (armySlot != (ArmyRuntimeSlot *)0x0) {
      targetEntry->armyRuntime = armySlot;
      modelNode = armySlot->modelNodeRuntime;
      translationX = (modelNode->worldTransform).translation.x;
      translationY = (modelNode->worldTransform).translation.y;
      targetEntry->modelRuntime = modelNode;
      targetEntry->worldXQ12 = translationX;
      targetEntry->worldYQ12 = translationY;
      g_AiWorkspace07Count = g_AiWorkspace07Count + 1;
      targetEntry = targetEntry + 1;
    }
    runtimeEntryCursor = runtimeEntryCursor + 1;
    widthOrCount = widthOrCount - 1;
  }
  scratchWidthOrFactionOffset = factionIndex * 0x740;
  primaryEntryCursor = g_AiWorkspaceBuffer00_Size0400;
  for (countOrMask = g_AiWorkspace00Count; countOrMask != 0; countOrMask = countOrMask - 1) {
    armySlot = (ArmyRuntimeSlot *)primaryEntryCursor->runtimeSlotAddressOrZero;
    if (armySlot != (ArmyRuntimeSlot *)0x0) {
      slotModelRuntime = (armySlot->modelRuntimeOrSavedOffset).modelRuntime;
      spacingOrPanelIndex = 0x1c;
      do {
        testResult = AiTechnologyCandidate_IsCurrentlyAvailableCf
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
        spacingOrPanelIndex = spacingOrPanelIndex - 1;
      } while (spacingOrPanelIndex != 0);
    }
    primaryEntryCursor = primaryEntryCursor + 1;
  }
  return;
}


/* Address: 0x0053BF30.
   Ownership: gameplay/ai/workspaces.
   Purpose: When candidate 0x14A is present, evaluates typed workspace12 entries through their callback table,
   selects the highest positive score, applies faction-anchor pressure to the knowledge weight, and adds the chosen
   candidate. It is distinct from FrontendPlayerIndex_V306, PlayerRuntimeId, active-faction masks or codes, and
   PCK-backed ArmyAssetId, ModelDefinitionId, and TechnologyId domains.
   Local calls: AiPrimaryWorkspace_HasEntryByIdCf, AiCandidateWorkspace_AddOrAccumulateWeightedEntry.
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
  uint weightRange;
  AiCandidateEntryKind entryKind;
  RuntimeToken entityId;
  
  hasSpecialAsset = AiPrimaryWorkspace_HasEntryByIdCf(ARM_0330_BUILDING_MDL0303);
  if (hasSpecialAsset) {
    /* EDX side channel for the category score callback: bit 2 / bit 4 when the faction owns any
       technology of category 2 / 3 (the decompiler kept only the empty loop). */
    g_AiTechnologyScoreCategoryMaskEdx = 0;
    for (wordIndexOrBestScore = 0; wordIndexOrBestScore < 8; wordIndexOrBestScore++) {
      dword owned = g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits[wordIndexOrBestScore];
      if ((g_TechnologyCategoryMasks.category2[wordIndexOrBestScore] & owned) != 0) {
        g_AiTechnologyScoreCategoryMaskEdx = g_AiTechnologyScoreCategoryMaskEdx | 2;
      }
      if ((g_TechnologyCategoryMasks.category3[wordIndexOrBestScore] & owned) != 0) {
        g_AiTechnologyScoreCategoryMaskEdx = g_AiTechnologyScoreCategoryMaskEdx | 4;
      }
    }
    wordIndexOrBestScore = 0;
    if (g_AiWorkspace12Count != 0) {
      entryKind = 2;
      weightRange = (g_AiKnowledgeData->parameters).workspace12BestCandidateBaseWeight;
      entityId = 0;
      candidatesRemaining = g_AiWorkspace12Count;
      candidateCursor = g_AiWorkspaceBuffer12_Size0200;
      do {
        candidateScore = (*g_AiTechnologyCandidateScoreCallbackTable[candidateCursor->scoreKind08])
                          (factionIndex,candidateCursor->technologyId00,worldRuntime);
        if (wordIndexOrBestScore < candidateScore) {
          entityId = candidateCursor->technologyId00;
          wordIndexOrBestScore = candidateScore;
        }
        candidateCursor = candidateCursor + 1;
        candidatesRemaining = candidatesRemaining - 1;
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
   Ownership: gameplay/ai/workspaces.
   Purpose: Clears the global AI candidate-workspace entry count.
*/
void __thandor_void_preserve_eax_ecx_edx AiCandidateWorkspace_Clear(void)

{
  g_AiCandidateWorkspaceEntryCount = 0;
  return;
}


/* Address: 0x00537430.
   Ownership: gameplay/ai/workspaces.
   Purpose: Clamps the workspace count to three, stores it in the faction runtime image at byte offset +0x50F418,
   and copies two dwords per retained entry to +0x50F400. Typed parameters: p0
   factionImageByteOffset→FactionImageByteOffset_V343. Calling convention, complete VariableStorage serialization,
   function bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void AiCandidateWorkspace_SaveToFactionImage(FactionImageByteOffset factionImageByteOffset)

{
  int entryCountOrDwordsRemaining;
  dword *candidateWorkspaceSourceCursor;
  dword *factionImageDestinationCursor;
  
  candidateWorkspaceSourceCursor = &g_AiWorkspaceBuffer13_Size0400->weightedScoreAndKind;
  entryCountOrDwordsRemaining = g_AiCandidateWorkspaceEntryCount;
  if (2 < g_AiCandidateWorkspaceEntryCount) {
    entryCountOrDwordsRemaining = 3;
  }
  *(int *)((int)(g_GameFactionRuntimeImage.records[0].candidateCache.savedEntries + 3) +
          factionImageByteOffset) = entryCountOrDwordsRemaining;
  factionImageDestinationCursor =
       (dword *)((int)&g_GameFactionRuntimeImage.records[0].candidateCache.savedEntries[0].
                       weightedScoreAndKind + factionImageByteOffset);
  entryCountOrDwordsRemaining = entryCountOrDwordsRemaining * 2;
  if (entryCountOrDwordsRemaining != 0) {
    for (; entryCountOrDwordsRemaining != 0; entryCountOrDwordsRemaining = entryCountOrDwordsRemaining + -1) {
      *factionImageDestinationCursor = *candidateWorkspaceSourceCursor;
      candidateWorkspaceSourceCursor = candidateWorkspaceSourceCursor + 1;
      factionImageDestinationCursor = factionImageDestinationCursor + 1;
    }
  }
  return;
}


/* Address: 0x00537470.
   Ownership: gameplay/ai/workspaces.
   Purpose: Loads the saved AI candidate count and two-dword entries from the faction runtime image into the shared
   workspace. Typed parameters: p0 factionImageByteOffset→FactionImageByteOffset_V343. Calling convention, complete
   VariableStorage serialization, function bytes, control flow, globals, locals, and executable data remain
   unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
AiCandidateWorkspace_LoadFromFactionImage(FactionImageByteOffset factionImageByteOffset)

{
  int copyDwordsRemaining;
  dword *factionImageSourceCursor;
  dword *candidateWorkspaceDestinationCursor;
  
  g_AiCandidateWorkspaceEntryCount =
       *(int *)((int)(g_GameFactionRuntimeImage.records[0].candidateCache.savedEntries + 3) +
               factionImageByteOffset);
  factionImageSourceCursor =
       (dword *)((int)&g_GameFactionRuntimeImage.records[0].candidateCache.savedEntries[0].
                       weightedScoreAndKind + factionImageByteOffset);
  copyDwordsRemaining = g_AiCandidateWorkspaceEntryCount * 2;
  candidateWorkspaceDestinationCursor = &g_AiWorkspaceBuffer13_Size0400->weightedScoreAndKind;
  if (copyDwordsRemaining != 0) {
    for (; copyDwordsRemaining != 0; copyDwordsRemaining = copyDwordsRemaining + -1) {
      *candidateWorkspaceDestinationCursor = *factionImageSourceCursor;
      factionImageSourceCursor = factionImageSourceCursor + 1;
      candidateWorkspaceDestinationCursor = candidateWorkspaceDestinationCursor + 1;
    }
  }
  return;
}


/* Address: 0x00537570.
   Ownership: gameplay/ai/workspaces.
   Purpose: Sorts the global two-dword candidate entries in descending order by the packed first dword while moving
   the paired second dword with each entry.
*/
void __thandor_void_preserve_eax_ecx_edx AiCandidateWorkspace_SortDescending(void)

{
  int currentRecordScore;
  uint recordsInCurrentPass;
  int comparisonsRemaining;
  int currentRecordPayload;
  AiCandidateWorkspaceEntry *scanRecordCursor;
  AiCandidateWorkspaceEntry *currentRecordCursor;
  dword promotedScore;
  dword promotedPayload;
  
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
        scanRecordCursor = scanRecordCursor + 1;
        comparisonsRemaining = comparisonsRemaining + -1;
      } while (comparisonsRemaining != 0);
      currentRecordScore = currentRecordCursor[1].weightedScoreAndKind;
      currentRecordPayload = currentRecordCursor[1].entityIdAndMultiplicity;
      comparisonsRemaining = recordsInCurrentPass - 2;
      scanRecordCursor = currentRecordCursor + 2;
      recordsInCurrentPass = recordsInCurrentPass - 1;
      currentRecordCursor = currentRecordCursor + 1;
    } while (comparisonsRemaining != 0);
  }
  return;
}


/* Address: 0x005375D0.
   Ownership: gameplay/ai/workspaces.
   Purpose: Decodes the low-nibble entry kind and low-word ID. Kind 2 reads TechnologyRecord.entityValue20; other
   nontrivial kinds resolve an ArmyAsset record and read +0x28, with 0x7FFFFFFF fallback.
   Cross-module calls: ArmyAssetRegistry_FindByIdCf [assets/army/catalog].
*/
int __thandor_eax_preserve_ecx_edx
AiCandidateWorkspace_GetEntryEntityValue(AiCandidateWorkspaceEntry *entry)

{
  dword resolvedEntityValue;
  RuntimeToken registryId;
  ArmyRegistryEaxCf5_51b6d0 registryLookup;
  
  registryId = entry->entityIdAndMultiplicity & 0xffff;
  if ((entry->weightedScoreAndKind & 0xf) == 2) {
    resolvedEntityValue = g_TechnologyAsset->records[registryId].xeniteCostQ4;
  }
  else {
    registryLookup = ArmyAssetRegistry_FindByIdCf(registryId);
    resolvedEntityValue = 0x7fffffff;
    if (!registryLookup.carry) {
      resolvedEntityValue = registryLookup.eax[2].registryId;
    }
  }
  return resolvedEntityValue;
}


/* Address: 0x00538C90.
   Ownership: gameplay/ai/workspaces.
   Purpose: Recovered missed predicate symmetric to the primary workspace helper. It scans secondary eight-byte
   entries and sets CF for a matching ID with zero assignment. Typed parameters: p0 entryId→RuntimeToken. Nearby
   but non-identical semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes,
   control flow, globals, locals, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
AiSecondaryWorkspace_HasUnassignedEntryByIdCf(PckArmyAssetIdCatalog entryId)

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
   Ownership: gameplay/ai/workspaces.
   Purpose: Scans the secondary eight-byte AI workspace entries and returns CF set on any matching ID at +0x04,
   clear on absence, while preserving EAX. Typed parameters: p0 entryId→RuntimeToken. Nearby but non-identical
   semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes, control flow,
   globals, locals, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
AiSecondaryWorkspace_HasEntryByIdCf(PckArmyAssetIdCatalog entryId)

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
    workspaceEntryCursor = workspaceEntryCursor + 1;
    workspaceEntriesRemaining = workspaceEntriesRemaining + -1;
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
   Ownership: gameplay/ai/workspaces.
   Purpose: Returns the signed minimum Manhattan distance in EAX for secondary-workspace entries; empty input
   returns INT_MAX. Secondary-workspace variant. Typed parameters: p0 worldX→Q12, p1 worldY→Q12. Nearby but non-
   identical semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes, control
   flow, globals, locals, and executable data remain unchanged.
*/
int __thandor_eax_preserve_ecx_edx
AiSecondaryWorkspace_GetMinimumManhattanDistanceToPoint(Q12 worldX,Q12 worldY)

{
  Q12 minimumManhattanDistanceQ12;
  int workspaceEntriesRemaining;
  int deltaXAbsQ12;
  Q12 deltaYAbsQ12;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  GameEntityRuntime *entityRuntime;
  
  minimumManhattanDistanceQ12 = 0x7fffffff;
  workspaceEntryCursor = g_AiWorkspaceBuffer01_Size0200;
  for (workspaceEntriesRemaining = g_AiWorkspace01Count; workspaceEntriesRemaining != 0;
      workspaceEntriesRemaining = workspaceEntriesRemaining + -1) {
    if (workspaceEntryCursor->armyRuntime != (ArmyRuntimeSlot *)0x0) {
      entityRuntime = workspaceEntryCursor->armyRuntime->linkedEntityRuntime;
      deltaYAbsQ12 = worldY - (entityRuntime->common).pathCoordinate0Q12;
      if (deltaYAbsQ12 < 0) {
        deltaYAbsQ12 = -deltaYAbsQ12;
      }
      deltaXAbsQ12 = worldX - (entityRuntime->common).pathCoordinate1Q12;
      if (deltaXAbsQ12 < 0) {
        deltaXAbsQ12 = -deltaXAbsQ12;
      }
      if (deltaYAbsQ12 + deltaXAbsQ12 < minimumManhattanDistanceQ12) {
        minimumManhattanDistanceQ12 = deltaYAbsQ12 + deltaXAbsQ12;
      }
    }
    workspaceEntryCursor = workspaceEntryCursor + 1;
  }
  return minimumManhattanDistanceQ12;
}


/* Address: 0x00538E00.
   Ownership: gameplay/ai/workspaces.
   Purpose: Returns the signed minimum Manhattan distance in EAX for active primary-workspace entries; empty input
   returns INT_MAX. Typed parameters: p0 worldX→Q12, p1 worldY→Q12. Nearby but non-identical semantic domains were
   explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals, locals, and
   executable data remain unchanged.
*/
int __thandor_eax_preserve_ecx_edx
AiPrimaryWorkspace_GetMinimumActiveManhattanDistanceToPoint(Q12 worldX,Q12 worldY)

{
  Q12 minimumActiveManhattanDistanceQ12;
  int workspaceEntriesRemaining;
  int deltaXAbsQ12;
  Q12 deltaYAbsQ12;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  ArmyRuntimeSlot *armySlot;
  
  minimumActiveManhattanDistanceQ12 = 0x7fffffff;
  workspaceEntryCursor = (AiRuntimeWorkspaceEntry *)g_AiWorkspaceBuffer00_Size0400;
  for (workspaceEntriesRemaining = g_AiWorkspace00Count; workspaceEntriesRemaining != 0;
      workspaceEntriesRemaining = workspaceEntriesRemaining + -1) {
    armySlot = workspaceEntryCursor->armyRuntime;
    if ((armySlot != (ArmyRuntimeSlot *)0x0) &&
       ((armySlot->linkedEntityRuntime->common).commandState != 0)) {
      deltaYAbsQ12 = worldY - (armySlot->modelNodeRuntime->worldTransform).translation.x;
      if (deltaYAbsQ12 < 0) {
        deltaYAbsQ12 = -deltaYAbsQ12;
      }
      deltaXAbsQ12 = worldX - (armySlot->modelNodeRuntime->worldTransform).translation.y;
      if (deltaXAbsQ12 < 0) {
        deltaXAbsQ12 = -deltaXAbsQ12;
      }
      if (deltaYAbsQ12 + deltaXAbsQ12 < minimumActiveManhattanDistanceQ12) {
        minimumActiveManhattanDistanceQ12 = deltaYAbsQ12 + deltaXAbsQ12;
      }
    }
    workspaceEntryCursor = workspaceEntryCursor + 1;
  }
  return minimumActiveManhattanDistanceQ12;
}


/* Address: 0x00538E80.
   Ownership: gameplay/ai/workspaces.
   Purpose: Returns the signed minimum Manhattan distance in EAX for workspace02 entries; empty input returns
   INT_MAX. Workspace-02 variant. Typed parameters: p0 worldX→Q12, p1 worldY→Q12. Nearby but non-identical semantic
   domains were explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals,
   locals, and executable data remain unchanged.
*/
int __thandor_eax_preserve_ecx_edx
AiWorkspace02_GetMinimumManhattanDistanceToPoint(Q12 worldX,Q12 worldY)

{
  Q12 minimumManhattanDistanceQ12;
  int workspaceEntriesRemaining;
  int deltaXAbsQ12;
  Q12 deltaYAbsQ12;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  ModelRuntimeNode *modelNode;
  
  minimumManhattanDistanceQ12 = 0x7fffffff;
  workspaceEntryCursor = g_AiWorkspaceBuffer02_Size0400;
  for (workspaceEntriesRemaining = g_AiWorkspace02Count; workspaceEntriesRemaining != 0;
      workspaceEntriesRemaining = workspaceEntriesRemaining + -1) {
    if (workspaceEntryCursor->armyRuntime != (ArmyRuntimeSlot *)0x0) {
      modelNode = workspaceEntryCursor->armyRuntime->modelNodeRuntime;
      deltaYAbsQ12 = worldY - (modelNode->worldTransform).translation.x;
      if (deltaYAbsQ12 < 0) {
        deltaYAbsQ12 = -deltaYAbsQ12;
      }
      deltaXAbsQ12 = worldX - (modelNode->worldTransform).translation.y;
      if (deltaXAbsQ12 < 0) {
        deltaXAbsQ12 = -deltaXAbsQ12;
      }
      if (deltaYAbsQ12 + deltaXAbsQ12 < minimumManhattanDistanceQ12) {
        minimumManhattanDistanceQ12 = deltaYAbsQ12 + deltaXAbsQ12;
      }
    }
    workspaceEntryCursor = workspaceEntryCursor + 1;
  }
  return minimumManhattanDistanceQ12;
}


/* Address: 0x00538EF0.
   Ownership: gameplay/ai/workspaces.
   Purpose: Returns the signed minimum Manhattan distance in EAX for workspace03 entries; empty input returns
   INT_MAX. Typed parameters: p0 worldX→Q12, p1 worldY→Q12. Nearby but non-identical semantic domains were
   explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals, locals, and
   executable data remain unchanged.
*/
int __thandor_eax_preserve_ecx_edx
AiWorkspace03_GetMinimumManhattanDistanceToPoint(Q12 worldX,Q12 worldY)

{
  Q12 minimumManhattanDistanceQ12;
  int workspaceEntriesRemaining;
  int deltaXAbsQ12;
  Q12 deltaYAbsQ12;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  ModelRuntimeNode *modelNode;
  
  minimumManhattanDistanceQ12 = 0x7fffffff;
  workspaceEntryCursor = g_AiWorkspaceBuffer03_Size1000;
  for (workspaceEntriesRemaining = g_AiWorkspace03Count; workspaceEntriesRemaining != 0;
      workspaceEntriesRemaining = workspaceEntriesRemaining + -1) {
    if (workspaceEntryCursor->armyRuntime != (ArmyRuntimeSlot *)0x0) {
      modelNode = workspaceEntryCursor->armyRuntime->modelNodeRuntime;
      deltaYAbsQ12 = worldY - (modelNode->worldTransform).translation.x;
      if (deltaYAbsQ12 < 0) {
        deltaYAbsQ12 = -deltaYAbsQ12;
      }
      deltaXAbsQ12 = worldX - (modelNode->worldTransform).translation.y;
      if (deltaXAbsQ12 < 0) {
        deltaXAbsQ12 = -deltaXAbsQ12;
      }
      if (deltaYAbsQ12 + deltaXAbsQ12 < minimumManhattanDistanceQ12) {
        minimumManhattanDistanceQ12 = deltaYAbsQ12 + deltaXAbsQ12;
      }
    }
    workspaceEntryCursor = workspaceEntryCursor + 1;
  }
  return minimumManhattanDistanceQ12;
}


/* Address: 0x00538F60.
   Ownership: gameplay/ai/workspaces.
   Purpose: Returns the signed minimum Manhattan distance in EAX for all primary-workspace entries; empty input
   returns INT_MAX. Typed parameters: p0 worldX→Q12, p1 worldY→Q12. Nearby but non-identical semantic domains were
   explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals, locals, and
   executable data remain unchanged.
*/
int __thandor_eax_preserve_ecx_edx
AiPrimaryWorkspace_GetMinimumManhattanDistanceToPoint(Q12 worldX,Q12 worldY)

{
  Q12 minimumManhattanDistanceQ12;
  int workspaceEntriesRemaining;
  int deltaXAbsQ12;
  Q12 deltaYAbsQ12;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  ModelRuntimeNode *modelNode;
  
  minimumManhattanDistanceQ12 = 0x7fffffff;
  workspaceEntryCursor = (AiRuntimeWorkspaceEntry *)g_AiWorkspaceBuffer00_Size0400;
  for (workspaceEntriesRemaining = g_AiWorkspace00Count; workspaceEntriesRemaining != 0;
      workspaceEntriesRemaining = workspaceEntriesRemaining + -1) {
    if (workspaceEntryCursor->armyRuntime != (ArmyRuntimeSlot *)0x0) {
      modelNode = workspaceEntryCursor->armyRuntime->modelNodeRuntime;
      deltaYAbsQ12 = worldY - (modelNode->worldTransform).translation.x;
      if (deltaYAbsQ12 < 0) {
        deltaYAbsQ12 = -deltaYAbsQ12;
      }
      deltaXAbsQ12 = worldX - (modelNode->worldTransform).translation.y;
      if (deltaXAbsQ12 < 0) {
        deltaXAbsQ12 = -deltaXAbsQ12;
      }
      if (deltaYAbsQ12 + deltaXAbsQ12 < minimumManhattanDistanceQ12) {
        minimumManhattanDistanceQ12 = deltaYAbsQ12 + deltaXAbsQ12;
      }
    }
    workspaceEntryCursor = workspaceEntryCursor + 1;
  }
  return minimumManhattanDistanceQ12;
}


/* Address: 0x00539240.
   Ownership: gameplay/ai/workspaces.
   Purpose: Finds a workspace08 record with the requested special asset ID, validates the point, creates and
   initializes the runtime entity, rebuilds its model transforms and auxiliary state, then consumes the faction
   pending asset. Stock ARM contains 675 records and 326 unique ids; placement workspace, producer, tier, class,
   and faction-role semantics are not inferred from numeric adjacency.
   Cross-module calls: AiPlacement_TestWorkspaceRecordAtPoint [gameplay/ai/placement],
   ArmyRuntime_CreateInstanceFromAssetCf [gameplay/army/runtime], ModelNodeRuntime_RebuildTransformsFromRoot
   [world/model/hierarchy], ArmyRuntime_DispatchClassCommand [gameplay/army/runtime],
   EffectRuntimePool_CreateInstanceFromDefinitionCf [world/effects/runtime],
   AiConstructionPlanner_ConsumeFactionPendingArmyAsset [gameplay/ai/planning].
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
  ArmyRuntimeCreateEaxCf5 createResult;
  
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
        createResult = ArmyRuntime_CreateInstanceFromAssetCf
                          (4,(uint)(ushort)workspaceRecord->triangle0NormalAngles,
                           workspaceRecord->worldY,workspaceRecord->worldX,factionIndex,armyAssetId,
                           worldRuntime);
        createdSlotPair = (ArmyRuntimeSlot **)createResult.eax;
        if (createResult.carry) {
          return;
        }
        modelNodeRuntime = createdSlotPair[1];
        primarySlot = *createdSlotPair;
        modelNodeRuntime->movementPosition0Q12 = 0;
        createdModelRuntime = (primarySlot->modelRuntimeOrSavedOffset).modelRuntime;
        ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)modelNodeRuntime);
        ArmyRuntime_DispatchClassCommand(createdSlotPair,worldRuntime);
        EffectRuntimePool_CreateInstanceFromDefinitionCf
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
    terrainFeatureEntry = terrainFeatureEntry + 1;
    recordsRemaining = recordsRemaining + -1;
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
   Ownership: gameplay/ai/workspaces.
   Purpose: Allocates fifteen fixed-size opaque AI workspace buffers and loads the exact 0x200-byte engine\ki.dat
   image. Allocation or package-load failure exits through CF without inventing field semantics for the workspaces.
   Allocates the 15 AI workspace buffers (0x400,0x200,0x400,0x1000,0x40,0x200,0x400,0x400,
   0x200,0x1000,0x400,0x1000,0x200,0x400,0x100) then loads engine\ki.dat (0x200 bytes) -> g_AiKnowledgeData. CF
   exit on any failure.
   Cross-module calls: Package_LoadEntry [assets/package/runtime].
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx AiRuntime_InitWorkspace(void)

{
  byte *workspaceAllocation;
  AiTechnologyPlanningCandidate *technologyCandidateWorkspaceAllocation;
  AiKnowledgeDataImage *knowledgeDataImage;
  PackageLoadEntryEaxCf5 loadResult;
  StatusValueEaxCf5 initStatus;
  
  loadResult = THANDOR_BITCAST(ArenaAllocEaxCf5, PackageLoadEntryEaxCf5, (*g_MemoryApi.alloc)(0x400));
  workspaceAllocation = loadResult.bufferOrError;
  if (!loadResult.carry) {
    g_AiWorkspaceBuffer00_Size0400 = (AiWorkspace00EntryView8 *)workspaceAllocation;
    loadResult = THANDOR_BITCAST(ArenaAllocEaxCf5, PackageLoadEntryEaxCf5, (*g_MemoryApi.alloc)(0x200));
    if (!loadResult.carry) {
      g_AiWorkspaceBuffer01_Size0200 = loadResult.bufferOrError;
      loadResult = THANDOR_BITCAST(ArenaAllocEaxCf5, PackageLoadEntryEaxCf5, (*g_MemoryApi.alloc)(0x400));
      if (!loadResult.carry) {
        g_AiWorkspaceBuffer02_Size0400 = loadResult.bufferOrError;
        loadResult = THANDOR_BITCAST(ArenaAllocEaxCf5, PackageLoadEntryEaxCf5, (*g_MemoryApi.alloc)(0x1000));
        if (!loadResult.carry) {
          g_AiWorkspaceBuffer03_Size1000 = loadResult.bufferOrError;
          loadResult = THANDOR_BITCAST(ArenaAllocEaxCf5, PackageLoadEntryEaxCf5, (*g_MemoryApi.alloc)(0x40));
          if (!loadResult.carry) {
            g_AiWorkspaceBuffer04_Size0040 = loadResult.bufferOrError;
            loadResult = THANDOR_BITCAST(ArenaAllocEaxCf5, PackageLoadEntryEaxCf5, (*g_MemoryApi.alloc)(0x200));
            if (!loadResult.carry) {
              g_AiWorkspaceBuffer05_Size0200 = loadResult.bufferOrError;
              loadResult = THANDOR_BITCAST(ArenaAllocEaxCf5, PackageLoadEntryEaxCf5, (*g_MemoryApi.alloc)(0x400));
              if (!loadResult.carry) {
                g_AiWorkspaceBuffer06_Size0400 = loadResult.bufferOrError;
                loadResult = THANDOR_BITCAST(ArenaAllocEaxCf5, PackageLoadEntryEaxCf5, (*g_MemoryApi.alloc)(0x400));
                if (!loadResult.carry) {
                  g_AiWorkspaceBuffer07_Size0400 = loadResult.bufferOrError;
                  loadResult = THANDOR_BITCAST(ArenaAllocEaxCf5, PackageLoadEntryEaxCf5, (*g_MemoryApi.alloc)(0x200));
                  if (!loadResult.carry) {
                    g_AiWorkspaceBuffer08_Size0200 = loadResult.bufferOrError;
                    loadResult = THANDOR_BITCAST(ArenaAllocEaxCf5, PackageLoadEntryEaxCf5, (*g_MemoryApi.alloc)(0x1000));
                    if (!loadResult.carry) {
                      g_AiWorkspaceBuffer09_Size1000 = loadResult.bufferOrError;
                      loadResult = THANDOR_BITCAST(ArenaAllocEaxCf5, PackageLoadEntryEaxCf5, (*g_MemoryApi.alloc)(0x400));
                      if (!loadResult.carry) {
                        g_AiWorkspaceBuffer10_Size0400 = loadResult.bufferOrError;
                        loadResult = THANDOR_BITCAST(ArenaAllocEaxCf5, PackageLoadEntryEaxCf5, (*g_MemoryApi.alloc)(0x1000));
                        if (!loadResult.carry) {
                          g_AiWorkspaceBuffer11_Size1000 = loadResult.bufferOrError;
                          loadResult = THANDOR_BITCAST(ArenaAllocEaxCf5, PackageLoadEntryEaxCf5, (*g_MemoryApi.alloc)(0x200));
                          technologyCandidateWorkspaceAllocation = loadResult.bufferOrError;
                          if (!loadResult.carry) {
                            g_AiWorkspaceBuffer12_Size0200 = technologyCandidateWorkspaceAllocation;
                            loadResult = THANDOR_BITCAST(ArenaAllocEaxCf5, PackageLoadEntryEaxCf5, (*g_MemoryApi.alloc)(0x400));
                            if (!loadResult.carry) {
                              g_AiWorkspaceBuffer13_Size0400 = loadResult.bufferOrError;
                              loadResult = THANDOR_BITCAST(ArenaAllocEaxCf5, PackageLoadEntryEaxCf5, (*g_MemoryApi.alloc)(0x100));
                              if (!loadResult.carry) {
                                g_AiWorkspaceBuffer14_Size0100 = loadResult.bufferOrError;
                                loadResult = Package_LoadEntry((word *)u_engine_ki_dat_0053c5e4);
                                knowledgeDataImage = loadResult.bufferOrError;
                                if (!loadResult.carry) {
                                  loadResult = THANDOR_BITCAST(qword, PackageLoadEntryEaxCf5, ((THANDOR_BITCAST(PackageLoadEntryEaxCf5, qword, loadResult) & 0xFFFFFFFFFFull) & 0xffffffff));
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
  initStatus.valueOrError = (dword)loadResult.bufferOrError;
  initStatus.carry = loadResult.carry;
  return initStatus;
}


/* Address: 0x00537F80.
   Ownership: gameplay/ai/workspaces.
   Purpose: Adds the current entity pointer to the 0x400-entry workspace09 list when capacity remains and the
   entity position lies outside every primary-workspace entry extent.
   Local calls: AiPrimaryWorkspace_IsPointOutsideAllEntryExtentsCf.
*/
void __thandor_void_preserve_ecx_edx
AiEntityCandidateWorkspace09_AddOutsidePrimaryExtents(FieldGridCell *currentCell)

{
  FieldGridCell **cellBuffer;
  uint entryIndex;
  bool isOutsideExtents;
  
  entryIndex = g_AiWorkspace09Count;
  cellBuffer = g_AiWorkspaceBuffer09_Size1000;
  if (g_AiWorkspace09Count < 0x400) {
    isOutsideExtents = AiPrimaryWorkspace_IsPointOutsideAllEntryExtentsCf
                      (currentCell->worldY,currentCell->worldX);
    if (!isOutsideExtents) {
      cellBuffer[entryIndex] = currentCell;
      g_AiWorkspace09Count = g_AiWorkspace09Count + 1;
    }
  }
  return;
}


/* Address: 0x00537FC0.
   Ownership: gameplay/ai/workspaces.
   Purpose: Adds the current entity pointer to the 0x100-entry workspace10 list under the same outside-primary-
   extents test.
   Local calls: AiPrimaryWorkspace_IsPointOutsideAllEntryExtentsCf.
*/
void __thandor_void_preserve_ecx_edx
AiEntityCandidateWorkspace10_AddOutsidePrimaryExtents(FieldGridCell *currentCell)

{
  FieldGridCell **cellBuffer;
  uint entryIndex;
  bool isOutsideExtents;
  
  entryIndex = g_AiWorkspace10Count;
  cellBuffer = g_AiWorkspaceBuffer10_Size0400;
  if (g_AiWorkspace10Count < 0x100) {
    isOutsideExtents = AiPrimaryWorkspace_IsPointOutsideAllEntryExtentsCf
                      (currentCell->worldY,currentCell->worldX);
    if (!isOutsideExtents) {
      cellBuffer[entryIndex] = currentCell;
      g_AiWorkspace10Count = g_AiWorkspace10Count + 1;
    }
  }
  return;
}


/* Address: 0x00538B90.
   Ownership: gameplay/ai/workspaces.
   Purpose: Scans the primary eight-byte AI workspace entries. CF is set when an entry has the requested ID at
   +0x04 and zero assignment at +0x00; CF is clear otherwise. EAX is preserved. Typed parameters: p0
   entryId→RuntimeToken. Nearby but non-identical semantic domains were explicitly deferred.
*/
bool __thandor_cf_preserve_eax_ecx_edx
AiPrimaryWorkspace_HasUnassignedEntryByIdCf(PckArmyAssetIdCatalog entryId)

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
       (workspaceEntryCursor->armyRuntime == (ArmyRuntimeSlot *)0x0)) break;
    workspaceEntryCursor = workspaceEntryCursor + 1;
    workspaceEntriesRemaining = workspaceEntriesRemaining + -1;
  }
  return true;
}


/* Address: 0x00538BF0.
   Ownership: gameplay/ai/workspaces.
   Purpose: Scans the primary eight-byte AI workspace entries and returns CF set on any matching ID at +0x04, clear
   on absence, while preserving EAX. Typed parameters: p0 entryId→RuntimeToken. Nearby but non-identical semantic
   domains were explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals,
   locals, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
AiPrimaryWorkspace_HasEntryByIdCf(PckArmyAssetIdCatalog entryId)

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
    workspaceEntryCursor = workspaceEntryCursor + 1;
    workspaceEntriesRemaining = workspaceEntriesRemaining + -1;
  }
  return true;
}


/* Address: 0x00538C40.
   Ownership: gameplay/ai/workspaces.
   Purpose: Counts primary workspace entries whose assignment dword at +0x00 is nonzero and whose ID at +0x04
   matches the requested value. The count is returned in EAX. Typed parameters: p0 entryId→RuntimeToken. Nearby but
   non-identical semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes,
   control flow, globals, locals, and executable data remain unchanged.
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
      workspaceEntriesRemaining = workspaceEntriesRemaining + -1) {
    if ((workspaceEntryCursor->armyRuntime != (ArmyRuntimeSlot *)0x0) &&
       (entryId == workspaceEntryCursor->armyAssetId)) {
      matchingAssignedEntryCount = matchingAssignedEntryCount + 1;
    }
    workspaceEntryCursor = workspaceEntryCursor + 1;
  }
  return matchingAssignedEntryCount;
}


/* Address: 0x005374B0.
   Ownership: gameplay/ai/workspaces.
   Purpose: Matching kind and ID entries accumulate count and weight; new entries are capped at 128. Typed
   parameters: p0 entityId→RuntimeToken. Nearby but non-identical semantic domains were explicitly deferred.
   Calling convention, parameter storage, body bytes, control flow, globals, locals, and executable data remain
   unchanged. Typed parameters: p2 entryKind→AiCandidateEntryKind_V344.
*/
void __thandor_void_preserve_eax_ecx_edx
AiCandidateWorkspace_AddOrAccumulateWeightedEntry
          (RuntimeToken entityId,dword weightRange,AiCandidateEntryKind entryKind)

{
  uint newEntryIndex;
  dword randomValue;
  int weightedScore;
  uint entriesRemaining;
  AiCandidateWorkspaceEntry *candidateEntry;
  
  randomValue = (*g_RandomGeneratorState.next)();
  newEntryIndex = g_AiCandidateWorkspaceEntryCount;
  candidateEntry = g_AiWorkspaceBuffer13_Size0400;
  if (1 < weightRange) {
    weightedScore = weightRange * 5 + randomValue % weightRange;
    for (entriesRemaining = g_AiCandidateWorkspaceEntryCount; entriesRemaining != 0; entriesRemaining = entriesRemaining - 1) {
      if (((g_AiWorkspaceBuffer13_Size0400[entriesRemaining - 1].weightedScoreAndKind & 0xf) == entryKind) &&
         ((g_AiWorkspaceBuffer13_Size0400[entriesRemaining - 1].entityIdAndMultiplicity & 0xffff) == entityId))
      {
        g_AiWorkspaceBuffer13_Size0400[entriesRemaining - 1].entityIdAndMultiplicity =
             g_AiWorkspaceBuffer13_Size0400[entriesRemaining - 1].entityIdAndMultiplicity + 0x10000;
        candidateEntry = candidateEntry + (entriesRemaining - 1);
        candidateEntry->weightedScoreAndKind = candidateEntry->weightedScoreAndKind + weightedScore * 0x10;
        return;
      }
    }
    if (g_AiCandidateWorkspaceEntryCount < 0x80) {
      g_AiWorkspaceBuffer13_Size0400[g_AiCandidateWorkspaceEntryCount].entityIdAndMultiplicity =
           entityId + 0x10000;
      g_AiCandidateWorkspaceEntryCount = g_AiCandidateWorkspaceEntryCount + 1;
      candidateEntry[newEntryIndex].weightedScoreAndKind = weightedScore * 0x10 | entryKind;
    }
  }
  return;
}


/* Address: 0x00538FD0.
   Ownership: gameplay/ai/workspaces.
   Purpose: Scans populated primary-workspace entries and returns CF clear when the point lies strictly inside both
   axis extents defined by record field +0x19C. CF is set when the point lies outside every entry extent. Typed
   parameters: p0 worldX→Q12, p1 worldY→Q12. Nearby but non-identical semantic domains were explicitly deferred.
   Calling convention, parameter storage, body bytes, control flow, globals, locals, and executable data remain
   unchanged.
*/
bool __thandor_void_preserve_ecx_edx
AiPrimaryWorkspace_IsPointOutsideAllEntryExtentsCf(Q12 worldX,Q12 worldY)

{
  Q12 deltaYAbsQ12;
  int workspaceEntriesRemaining;
  Q12 deltaXAbsQ12;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  int *workspaceEntryEntityRecord;
  
  workspaceEntriesRemaining = g_AiWorkspace00Count;
  workspaceEntryCursor = (AiRuntimeWorkspaceEntry *)g_AiWorkspaceBuffer00_Size0400;
  do {
    if (workspaceEntriesRemaining == 0) {
      return true;
    }
    workspaceEntryEntityRecord = (int *)workspaceEntryCursor->armyRuntime;
    if (workspaceEntryEntityRecord != (int *)0x0) {
      deltaYAbsQ12 = worldY - *(int *)(workspaceEntryEntityRecord[1] + 0x94);
      if (deltaYAbsQ12 < 0) {
        deltaYAbsQ12 = -deltaYAbsQ12;
      }
      deltaXAbsQ12 = worldX - *(int *)(workspaceEntryEntityRecord[1] + 0x98);
      if (deltaXAbsQ12 < 0) {
        deltaXAbsQ12 = -deltaXAbsQ12;
      }
      if ((deltaYAbsQ12 < *(int *)(*workspaceEntryEntityRecord + 0x19c)) &&
         (deltaXAbsQ12 < *(int *)(*workspaceEntryEntityRecord + 0x19c))) {
        return false;
      }
    }
    workspaceEntryCursor = workspaceEntryCursor + 1;
    workspaceEntriesRemaining = workspaceEntriesRemaining + -1;
  } while( true );
}

