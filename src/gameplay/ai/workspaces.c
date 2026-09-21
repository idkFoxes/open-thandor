/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/ai/workspaces.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/ai/workspaces.h>

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
  int iVar1;
  dword weightRange;
  AiTerrainFeatureWorkspaceEntry *terrainFeatureEntry;
  bool bVar2;
  
  iVar1 = g_AiWorkspace08Count;
  terrainFeatureEntry = g_AiWorkspaceBuffer08_Size0200;
  if ((g_AiWorkspace08Count != 0) &&
     (bVar2 = AiPrimaryWorkspace_HasUnassignedEntryByIdCf(armyAssetId), !bVar2)) {
    do {
      if ((armyAssetId == terrainFeatureEntry->armyAssetId) &&
         (bVar2 = AiPlacement_TestMode4AtWorkspaceRecord
                            (armyAssetId,terrainFeatureEntry->cell,factionIndex,worldRuntime),
         !bVar2)) {
        iVar1 = AiPrimaryWorkspace_CountAssignedEntriesById(armyAssetId);
        weightRange = (uint)(baseWeight * 3) / (iVar1 + 3U);
        if (armyAssetId != ARM_0330_BUILDING_MDL0303) {
          iVar1 = g_GameFactionRuntimeImage.records[factionIndex].tritiumExtractionRateQ4PerTick <<
                  4;
          if (iVar1 != 0) {
            weightRange = (dword)(((longlong)(int)weightRange *
                                  (longlong)
                                  (int)(g_GameFactionRuntimeImage.records[factionIndex].
                                        unpoweredEnergyDemandQ4 * 2 +
                                       g_GameFactionRuntimeImage.records[factionIndex].
                                       suppliedEnergyDemandQ4)) / (longlong)iVar1);
          }
        }
        AiCandidateWorkspace_AddOrAccumulateWeightedEntry(armyAssetId,weightRange,1);
        return;
      }
      terrainFeatureEntry = terrainFeatureEntry + 1;
      iVar1 = iVar1 + -1;
    } while (iVar1 != 0);
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
  GameEntityRuntime *pGVar1;
  PckArmyAssetIdCatalog PVar2;
  int *piVar3;
  FieldGridAsset *pFVar4;
  ArmyAssetRecordPrefix *definitionNode;
  GraphicsWorldCoordinateQ12 GVar5;
  GraphicsWorldCoordinateQ12 GVar6;
  ModelRuntimeSlot *pMVar7;
  AiWorkspace00EntryView8 *pAVar8;
  uint uVar9;
  AiRuntimeWorkspaceEntry *pAVar10;
  GridScratchStateMask GVar11;
  FactionArmyAssetCount FVar12;
  int iVar13;
  uint uVar14;
  dword dVar15;
  uint uVar16;
  dword dVar17;
  int iVar18;
  AiWorkspace00EntryView8 *pAVar19;
  FieldGridCell *pFVar20;
  byte *pbVar21;
  ArmyAssetRecordPrefix **armyAssetRegistryCursor26;
  WorldOwnerListNode100 *worldNode1;
  dword *pdVar22;
  GridScratchCell *pGVar23;
  GridScratchCell *pGVar24;
  ArmyAssetRecordPrefix **armyAssetRegistryCursor31;
  AiTargetWorkspaceEntry *targetEntry;
  bool bVar25;
  AiTechnologyPlanningLoopRegisterContinuityResult AVar26;
  ArmyRuntimeSlot *armySlot1;
  ModelRuntimeNode *modelNode;
  AiRuntimeWorkspaceEntry *runtimeWorkspaceEntry13;
  
  g_AiWorkspace00Count = 0;
  g_AiWorkspace04Count = 0;
  worldNode1 = worldRuntime->ownerListHead;
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
  if (worldNode1 != (WorldOwnerListNode100 *)0x0) {
    do {
      uVar14 = g_AiWorkspace01Count;
      runtimeWorkspaceEntry13 = g_AiWorkspaceBuffer01_Size0200;
      uVar16 = g_AiWorkspace00Count;
      pAVar19 = g_AiWorkspaceBuffer00_Size0400;
      if (worldNode1->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
        armySlot1 = worldNode1->runtimePayload;
        pGVar1 = armySlot1->linkedEntityRuntime;
        if (factionIndex == (pGVar1->common).ownership.ownerIndex) {
          PVar2 = (pGVar1->common).runtimeIdentityOrArmyAssetId;
          if (PVar2 < ARM_0300_BUILDING_MDL0301) {
            if (g_AiWorkspace01Count < 0x40) {
              g_AiWorkspaceBuffer01_Size0200[g_AiWorkspace01Count].armyRuntime = armySlot1;
              runtimeWorkspaceEntry13[uVar14].armyAssetId = PVar2;
              g_AiWorkspace01Count = g_AiWorkspace01Count + 1;
            }
          }
          else if (g_AiWorkspace00Count < 0x80) {
            g_AiWorkspaceBuffer00_Size0400[g_AiWorkspace00Count].runtimeSlotAddressOrZero =
                 (AiWorkspaceRuntimeSlotAddress32)armySlot1;
            pAVar19[uVar16].armyAssetId = PVar2;
            g_AiWorkspace00Count = g_AiWorkspace00Count + 1;
            if (PVar2 == ARM_0300_BUILDING_MDL0301) {
              g_AiWorkspaceOwnedAsset300Runtime = armySlot1;
            }
          }
        }
        else {
          uVar16 = *(uint *)((pGVar1->common).damageState.reserved0C_23 + 0x10) >>
                   ((char)factionIndex * '\x02' & 0x1fU);
          if (((pGVar1->common).ownership.ownerIndex != 0) &&
             (bVar25 = GameFactionRuntime_TestCapabilityBitClearCf
                                 ((pGVar1->common).ownership.ownerIndex,factionIndex),
             uVar9 = g_AiWorkspace03Count, pAVar10 = g_AiWorkspaceBuffer03_Size1000,
             uVar14 = g_AiWorkspace02Count, runtimeWorkspaceEntry13 = g_AiWorkspaceBuffer02_Size0400
             , bVar25)) {
            if (((uVar16 & 2) == 0) &&
               (((uVar16 & 1) == 0 ||
                (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand
                 [*(int *)(*(int *)(pGVar1->common).ownership.definitionOrClassRecord + 0x4c)] !=
                 ArmyRuntime_ClassCommandHandlerGroupACf)))) {
              PVar2 = (pGVar1->common).runtimeIdentityOrArmyAssetId;
              if (g_AiWorkspace03Count < 0x200) {
                g_AiWorkspaceBuffer03_Size1000[g_AiWorkspace03Count].armyRuntime = armySlot1;
                pAVar10[uVar9].armyAssetId = PVar2;
                g_AiWorkspace03Count = g_AiWorkspace03Count + 1;
              }
            }
            else {
              PVar2 = (pGVar1->common).runtimeIdentityOrArmyAssetId;
              if (g_AiWorkspace02Count < 0x80) {
                g_AiWorkspaceBuffer02_Size0400[g_AiWorkspace02Count].armyRuntime = armySlot1;
                runtimeWorkspaceEntry13[uVar14].armyAssetId = PVar2;
                g_AiWorkspace02Count = g_AiWorkspace02Count + 1;
              }
            }
          }
        }
      }
      worldNode1 = worldNode1->nextNode;
    } while (worldNode1 != (WorldOwnerListNode100 *)0x0);
    pdVar22 = g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetPointersOrIds;
    pAVar19 = g_AiWorkspaceBuffer00_Size0400;
    uVar16 = g_AiWorkspace00Count;
    for (FVar12 = g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount; FVar12 != 0
        ; FVar12 = FVar12 - 1) {
      PVar2 = *(PckArmyAssetIdCatalog *)(*pdVar22 + 8);
      g_AiWorkspaceBuffer00_Size0400 = pAVar19;
      g_AiWorkspace00Count = uVar16;
      if (uVar16 < 0x80) {
        pAVar19[uVar16].runtimeSlotAddressOrZero = 0;
        pAVar19[uVar16].armyAssetId = PVar2;
        g_AiWorkspace00Count = g_AiWorkspace00Count + 1;
      }
      uVar16 = g_AiWorkspace04Count;
      runtimeWorkspaceEntry13 = g_AiWorkspaceBuffer04_Size0040;
      if (g_AiWorkspace04Count < 8) {
        g_AiWorkspaceBuffer04_Size0040[g_AiWorkspace04Count].armyRuntime = (ArmyRuntimeSlot *)0x0;
        runtimeWorkspaceEntry13[uVar16].armyAssetId = PVar2;
        g_AiWorkspace04Count = g_AiWorkspace04Count + 1;
      }
      pdVar22 = pdVar22 + 1;
      pAVar19 = g_AiWorkspaceBuffer00_Size0400;
      uVar16 = g_AiWorkspace00Count;
    }
    pdVar22 = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetPointersOrIds;
    runtimeWorkspaceEntry13 = g_AiWorkspaceBuffer01_Size0200;
    uVar14 = g_AiWorkspace01Count;
    for (FVar12 = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount;
        pAVar8 = pAVar19, uVar9 = uVar16, FVar12 != 0; FVar12 = FVar12 - 1) {
      PVar2 = *(PckArmyAssetIdCatalog *)(*pdVar22 + 8);
      g_AiWorkspaceBuffer00_Size0400 = pAVar19;
      g_AiWorkspace00Count = uVar16;
      g_AiWorkspaceBuffer01_Size0200 = runtimeWorkspaceEntry13;
      g_AiWorkspace01Count = uVar14;
      if (PVar2 < ARM_0300_BUILDING_MDL0301) {
        if (uVar14 < 0x40) {
          runtimeWorkspaceEntry13[uVar14].armyRuntime = (ArmyRuntimeSlot *)0x0;
          runtimeWorkspaceEntry13[uVar14].armyAssetId = PVar2;
          g_AiWorkspace01Count = g_AiWorkspace01Count + 1;
        }
      }
      else if (uVar16 < 0x80) {
        pAVar19[uVar16].runtimeSlotAddressOrZero = 0;
        pAVar19[uVar16].armyAssetId = PVar2;
        g_AiWorkspace00Count = g_AiWorkspace00Count + 1;
      }
      pdVar22 = pdVar22 + 1;
      pAVar19 = g_AiWorkspaceBuffer00_Size0400;
      uVar16 = g_AiWorkspace00Count;
      runtimeWorkspaceEntry13 = g_AiWorkspaceBuffer01_Size0200;
      uVar14 = g_AiWorkspace01Count;
    }
    for (; g_AiWorkspaceBuffer00_Size0400 = pAVar8, g_AiWorkspace00Count = uVar16,
        g_AiWorkspaceBuffer01_Size0200 = runtimeWorkspaceEntry13, g_AiWorkspace01Count = uVar14,
        uVar9 != 0; uVar9 = uVar9 - 1) {
      piVar3 = (int *)pAVar19->runtimeSlotAddressOrZero;
      if (piVar3 != (int *)0x0) {
        iVar13 = *piVar3;
        if (*(int *)(iVar13 + 0x4c) == 0xb) {
          PVar2 = piVar3[0x18];
          if ((piVar3[0x2e] == 1) && (uVar16 < 0x80)) {
            pAVar8[uVar16].runtimeSlotAddressOrZero = 0;
            pAVar8[uVar16].armyAssetId = PVar2;
            g_AiWorkspace00Count = g_AiWorkspace00Count + 1;
          }
        }
        else if (*(int *)(iVar13 + 0x4c) == 0x16) {
          PVar2 = piVar3[0x18];
          if ((piVar3[0x2b] == 1) && (uVar14 < 0x40)) {
            runtimeWorkspaceEntry13[uVar14].armyRuntime = (ArmyRuntimeSlot *)0x0;
            runtimeWorkspaceEntry13[uVar14].armyAssetId = PVar2;
            g_AiWorkspace01Count = g_AiWorkspace01Count + 1;
          }
        }
        else if (((*(int *)(iVar13 + 0x4c) == 0xd) && (PVar2 = piVar3[0x18], piVar3[0x2e] == 1)) &&
                (uVar14 < 0x40)) {
          runtimeWorkspaceEntry13[uVar14].armyRuntime = (ArmyRuntimeSlot *)0x0;
          runtimeWorkspaceEntry13[uVar14].armyAssetId = PVar2;
          g_AiWorkspace01Count = g_AiWorkspace01Count + 1;
        }
      }
      pAVar19 = pAVar19 + 1;
      pAVar8 = g_AiWorkspaceBuffer00_Size0400;
      uVar16 = g_AiWorkspace00Count;
      runtimeWorkspaceEntry13 = g_AiWorkspaceBuffer01_Size0200;
      uVar14 = g_AiWorkspace01Count;
    }
  }
  dVar17 = g_GridScratchWidth;
  pFVar4 = worldRuntime->fieldGrid;
  uVar14 = pFVar4->gridWidth;
  iVar13 = pFVar4->gridHeight - 2;
  pbVar21 = pFVar4->cells[0].runtime0C_3F + factionIndex + -0xc;
  pGVar24 = g_GridScratchPrimary + g_GridScratchWidth * 2 + 2;
  dVar15 = g_GridScratchWidth * 0x18;
  AiPlanning_CollectActiveGridMaskClasses();
  iVar18 = uVar14 * 0x80;
  uVar16 = uVar14;
  do {
    do {
      if ((((pGVar24[dVar17 * 3].stateMask & 0x80000000) == 0) &&
          ((pbVar21[iVar18 + 0x70] & 0xf9) != 0)) &&
         (((((*(ResourceExtractionDescriptor32 *)(pbVar21 + 0x70) & 0xf9) == 0 ||
            (((pbVar21[0xf0] & 0xf9) == 0 || ((pbVar21[iVar18 + -0x10] & 0xf9) == 0)))) ||
           (((pbVar21[iVar18 + 0xf0] & 0xf9) == 0 ||
            (((pbVar21[uVar14 * 0x100 + -0x10] & 0xf9) == 0 ||
             ((pbVar21[uVar14 * 0x100 + 0x70] & 0xf9) == 0)))))) &&
          ((GVar11 = pGVar24->stateMask | pGVar24[3].stateMask | pGVar24[dVar17 * 3 + -3].stateMask
                     | pGVar24[dVar17 * 3 + 3].stateMask | pGVar24[dVar17 * 6 + -3].stateMask |
                     pGVar24[dVar17 * 6].stateMask | pGVar24[dVar17 * 3].stateMask,
           (g_AiActiveGridMaskClass0 & GVar11) == 0 ||
           ((((g_AiActiveGridMaskClass1 & GVar11) == 0 || ((g_AiActiveGridMaskClass2 & GVar11) == 0)
             ) || ((g_AiActiveGridMaskClass3 & GVar11) == 0)))))))) {
        AiSiteCandidate_AddGeneralCellIfSeparated
                  ((FieldGridCell *)(pbVar21 + (iVar18 - factionIndex)));
        pbVar21 = ((FieldGridCell *)(pbVar21 + (iVar18 - factionIndex)))[-uVar14].runtime0C_3F +
                  factionIndex + -0xc;
      }
      if ((((pbVar21[iVar18 + 0x70] & 0x10) != 0) &&
          ((pGVar24[dVar17 * 3].stateMask & 0x80000100) == 0)) &&
         ((GVar11 = pGVar24[dVar17 * 3].stateMask | pGVar24->stateMask | pGVar24[4].stateMask |
                    pGVar24[dVar17 * 3 + -4].stateMask | pGVar24[dVar17 * 3 + 4].stateMask |
                    pGVar24[dVar17 * 6 + -4].stateMask | pGVar24[dVar17 * 6].stateMask,
          (GVar11 & 0x81000000) == 0 ||
          (((GVar11 & 0x82000000) == 0 || ((GVar11 & 0x90000000) == 0)))))) {
        pFVar20 = (FieldGridCell *)(pbVar21 + (iVar18 - factionIndex));
        if ((pFVar20->flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) == 0) {
          AiSiteCandidate_AddFlaggedCellIfSeparated(pFVar20);
        }
        pbVar21 = pFVar20[-uVar14].runtime0C_3F + factionIndex + -0xc;
      }
      if (((pbVar21[iVar18 + 0x70] & 0xf9) != 0) &&
         ((pGVar24[dVar17 * 3].stateMask & 0xf0003f00) == 0)) {
        if (((pGVar24[dVar17 * 3].stateMask & 0xf000ff00) == 0) &&
           (((pGVar24->stateMask | pGVar24[4].stateMask | pGVar24[dVar17 * 3 + -4].stateMask |
              pGVar24[dVar17 * 3 + 4].stateMask | pGVar24[dVar17 * 6 + -4].stateMask |
             pGVar24[dVar17 * 6].stateMask) & 0xf0000000) == 0)) {
          pGVar23 = pGVar24 + dVar17 * -3;
          pGVar24 = pGVar23 + dVar17 * 3;
          if (((pGVar23->stateMask | pGVar23[8].stateMask | pGVar23[dVar17 * 6 + -8].stateMask |
                pGVar23[dVar17 * 6 + 8].stateMask | pGVar23[dVar17 * 0xc + -8].stateMask |
               pGVar23[dVar17 * 0xc].stateMask) & 0xf0000000) == 0) {
            pFVar20 = (FieldGridCell *)(pbVar21 + (iVar18 - factionIndex));
            if ((pFVar20->flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) != 0) {
              AiSiteCandidate_AddTerrainFeatureCellIfSeparated(pFVar20,dVar15);
            }
            pbVar21 = pFVar20[-uVar14].runtime0C_3F + factionIndex + -0xc;
          }
        }
        if (((pGVar24->stateMask | pGVar24[4].stateMask | pGVar24[dVar17 * 3 + -4].stateMask |
              pGVar24[dVar17 * 3 + 4].stateMask | pGVar24[dVar17 * 6 + -4].stateMask |
             pGVar24[dVar17 * 6].stateMask) & 0xf0000000) == 0) {
          pFVar20 = (FieldGridCell *)(pbVar21 + -factionIndex);
          if (((((((pFVar20[uVar14].flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) ==
                   0) && ((pFVar20->flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) ==
                          0)) &&
                ((pFVar20[1].flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) == 0)) &&
               (((pFVar20[uVar14 - 1].flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK)
                 == 0 && ((pFVar20[uVar14 + 1].flagsAndMaterial &
                          FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) == 0)))) &&
              ((pFVar20[uVar14 * 2 + -1].flagsAndMaterial &
               FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) == 0)) &&
             ((pFVar20[uVar14 * 2].flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) ==
              0)) {
            AiEntityCandidateWorkspace09_AddOutsidePrimaryExtents(pFVar20 + uVar14);
            pFVar20 = pFVar20 + uVar14 + -uVar14;
          }
          pbVar21 = pFVar20->runtime0C_3F + factionIndex + -0xc;
        }
        if (((pGVar24[dVar17 * 3].stateMask & 0xf000ff00) == 0) &&
           (((pGVar24->stateMask | pGVar24[4].stateMask | pGVar24[dVar17 * 3 + -4].stateMask |
              pGVar24[dVar17 * 3 + 4].stateMask | pGVar24[dVar17 * 6 + -4].stateMask |
             pGVar24[dVar17 * 6].stateMask) & 0xf0000000) == 0)) {
          pGVar23 = pGVar24 + dVar17 * -3;
          pGVar24 = pGVar23 + dVar17 * 3;
          if (((pGVar23->stateMask | pGVar23[8].stateMask | pGVar23[dVar17 * 6 + -8].stateMask |
                pGVar23[dVar17 * 6 + 8].stateMask | pGVar23[dVar17 * 0xc + -8].stateMask |
               pGVar23[dVar17 * 0xc].stateMask) &
              (GRID_SCRATCH_TERRAIN_CLASS_BIT30|GRID_SCRATCH_TERRAIN_CLASS_BIT29|
              GRID_SCRATCH_TERRAIN_CLASS_BIT28)) == 0) {
            pFVar20 = (FieldGridCell *)(pbVar21 + -factionIndex);
            if (((((pFVar20->flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) == 0) &&
                 ((pFVar20[1].flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) == 0))
                && ((pFVar20[uVar14 - 1].flagsAndMaterial &
                    FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) == 0)) &&
               ((((pFVar20[uVar14 + 1].flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK)
                  == 0 && ((pFVar20[uVar14 * 2 + -1].flagsAndMaterial &
                           FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) == 0)) &&
                (((pFVar20[uVar14 * 2].flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK)
                  == 0 && ((pFVar20[uVar14].flagsAndMaterial &
                           FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) == 0)))))) {
              AiEntityCandidateWorkspace10_AddOutsidePrimaryExtents(pFVar20 + uVar14);
              pFVar20 = pFVar20 + uVar14 + -uVar14;
            }
            pbVar21 = pFVar20->runtime0C_3F + factionIndex + -0xc;
          }
        }
      }
      pbVar21 = pbVar21 + 0x80;
      pGVar24 = pGVar24 + 4;
      uVar16 = uVar16 - 1;
    } while (uVar16 != 0);
    pGVar24 = pGVar24 + g_GridScratchWidth * 3;
    uVar16 = uVar14 & 0x1ffffff;
    iVar13 = iVar13 + -1;
  } while (iVar13 != 0);
  uVar16 = 0;
  pAVar19 = g_AiWorkspaceBuffer00_Size0400;
  for (uVar14 = g_AiWorkspace00Count; uVar14 != 0; uVar14 = uVar14 - 1) {
    if ((int *)pAVar19->runtimeSlotAddressOrZero != (int *)0x0) {
      iVar13 = *(int *)pAVar19->runtimeSlotAddressOrZero;
      iVar18 = *(int *)(iVar13 + 0x4c);
      if (iVar18 == 0xd) {
        uVar16 = uVar16 | *(uint *)(iVar13 + 0xc4);
      }
      else if (iVar18 == 0x16) {
        uVar16 = uVar16 | 8;
      }
      else if (iVar18 == 0xb) {
        uVar16 = uVar16 | 0x10;
      }
    }
    pAVar19 = pAVar19 + 1;
  }
  armyAssetRegistryCursor31 = g_ArmyAssetRecordRegistry;
  iVar13 = 0x300;
  armyAssetRegistryCursor26 = g_AiWorkspaceBuffer11_Size1000;
  do {
    definitionNode = *armyAssetRegistryCursor31;
    if (((definitionNode != (ArmyAssetRecordPrefix *)0x0) &&
        ((definitionNode[1].selectionDetailTemplateVariantIndex & 1) != 0)) &&
       (((bVar25 = ModelDefinitionHierarchy_AllTechnologyUnlockedForFactionCf
                             (factionIndex,(ModelDefinitionHierarchyNodeAddress32)definitionNode),
         !bVar25 && ((definitionNode[1].selectionDetailTemplateVariantIndex & uVar16) != 0)) &&
        (((definitionNode->registryId < ARM_0300_BUILDING_MDL0301 ||
          ((ARM_0320_BUILDING_MDL0311|ARM_0019_UNIT_MDL0101) < definitionNode->registryId)) &&
         (g_AiWorkspace11Count < 0x400)))))) {
      *armyAssetRegistryCursor26 = definitionNode;
      g_AiWorkspace11Count = g_AiWorkspace11Count + 1;
      armyAssetRegistryCursor26 = armyAssetRegistryCursor26 + 1;
    }
    armyAssetRegistryCursor31 = armyAssetRegistryCursor31 + 1;
    iVar13 = iVar13 + -1;
    runtimeWorkspaceEntry13 = g_AiWorkspaceBuffer02_Size0400;
    uVar14 = g_AiWorkspace02Count;
    targetEntry = g_AiWorkspaceBuffer07_Size0400;
  } while (iVar13 != 0);
  while ((uVar14 != 0 &&
         (armySlot1 = runtimeWorkspaceEntry13->armyRuntime, g_AiWorkspace07Count < 0x40))) {
    if (armySlot1 != (ArmyRuntimeSlot *)0x0) {
      targetEntry->armyRuntime = armySlot1;
      modelNode = armySlot1->modelNodeRuntime;
      GVar5 = (modelNode->worldTransform).translation.x;
      GVar6 = (modelNode->worldTransform).translation.y;
      targetEntry->modelRuntime = modelNode;
      targetEntry->worldXQ12 = GVar5;
      targetEntry->worldYQ12 = GVar6;
      g_AiWorkspace07Count = g_AiWorkspace07Count + 1;
      targetEntry = targetEntry + 1;
    }
    runtimeWorkspaceEntry13 = runtimeWorkspaceEntry13 + 1;
    uVar14 = uVar14 - 1;
  }
  dVar17 = factionIndex * 0x740;
  pAVar19 = g_AiWorkspaceBuffer00_Size0400;
  for (uVar16 = g_AiWorkspace00Count; uVar16 != 0; uVar16 = uVar16 - 1) {
    armySlot1 = (ArmyRuntimeSlot *)pAVar19->runtimeSlotAddressOrZero;
    if (armySlot1 != (ArmyRuntimeSlot *)0x0) {
      pMVar7 = (armySlot1->modelRuntimeOrSavedOffset).modelRuntime;
      dVar15 = 0x1c;
      do {
        bVar25 = AiTechnologyCandidate_IsCurrentlyAvailableCf
                           ((PckTechnologyIdCatalog)
                            (&pMVar7->attachments140[4].sourceTransform04)[dVar15],dVar17);
        if (!bVar25) {
          AVar26 = AiTechnologyPlanning_AddCandidateRecord
                             (dVar15,uVar16,dVar17,armySlot1,
                              (PckTechnologyIdCatalog)
                              (&pMVar7->attachments140[4].sourceTransform04)[dVar15]);
          dVar17 = AVar26.preservedEdxFactionRecordOffset;
          uVar16 = AVar26.preservedEcxSourceArmyEntriesRemaining;
          dVar15 = AVar26.preservedEaxTechnologyPanelIndex;
        }
        dVar15 = dVar15 - 1;
      } while (dVar15 != 0);
    }
    pAVar19 = pAVar19 + 1;
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
  AiTechnologyCandidateScore AVar1;
  int iVar2;
  AiTechnologyPlanningCandidateCount AVar3;
  AiTechnologyPlanningCandidate *pAVar4;
  bool bVar5;
  uint weightRange;
  AiCandidateEntryKind entryKind;
  RuntimeToken entityId;
  
  bVar5 = AiPrimaryWorkspace_HasEntryByIdCf(ARM_0330_BUILDING_MDL0303);
  if (bVar5) {
    iVar2 = 8;
    do {
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
    iVar2 = 0;
    if (g_AiWorkspace12Count != 0) {
      entryKind = 2;
      weightRange = (g_AiKnowledgeData->parameters).workspace12BestCandidateBaseWeight;
      entityId = 0;
      AVar3 = g_AiWorkspace12Count;
      pAVar4 = g_AiWorkspaceBuffer12_Size0200;
      do {
        AVar1 = (*g_AiTechnologyCandidateScoreCallbackTable[pAVar4->scoreKind08])
                          (factionIndex,pAVar4->technologyId00,worldRuntime);
        if (iVar2 < AVar1) {
          entityId = pAVar4->technologyId00;
          iVar2 = AVar1;
        }
        pAVar4 = pAVar4 + 1;
        AVar3 = AVar3 - 1;
      } while (AVar3 != 0);
      if (iVar2 != 0) {
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
  int iVar1;
  dword *candidateWorkspaceSourceCursor;
  dword *factionImageDestinationCursor;
  
  candidateWorkspaceSourceCursor = &g_AiWorkspaceBuffer13_Size0400->weightedScoreAndKind;
  iVar1 = g_AiCandidateWorkspaceEntryCount;
  if (2 < g_AiCandidateWorkspaceEntryCount) {
    iVar1 = 3;
  }
  *(int *)((int)(g_GameFactionRuntimeImage.records[0].candidateCache.savedEntries + 3) +
          factionImageByteOffset) = iVar1;
  factionImageDestinationCursor =
       (dword *)((int)&g_GameFactionRuntimeImage.records[0].candidateCache.savedEntries[0].
                       weightedScoreAndKind + factionImageByteOffset);
  iVar1 = iVar1 * 2;
  if (iVar1 != 0) {
    for (; iVar1 != 0; iVar1 = iVar1 + -1) {
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
  ArmyRegistryEaxCf5_51b6d0 AVar1;
  
  registryId = entry->entityIdAndMultiplicity & 0xffff;
  if ((entry->weightedScoreAndKind & 0xf) == 2) {
    resolvedEntityValue = g_TechnologyAsset->records[registryId].xeniteCostQ4;
  }
  else {
    AVar1 = ArmyAssetRegistry_FindByIdCf(registryId);
    resolvedEntityValue = 0x7fffffff;
    if (!AVar1.carry) {
      resolvedEntityValue = AVar1.eax[2].registryId;
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
  int iVar1;
  Q12 deltaYAbsQ12;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  GameEntityRuntime *entityRuntime1;
  
  minimumManhattanDistanceQ12 = 0x7fffffff;
  workspaceEntryCursor = g_AiWorkspaceBuffer01_Size0200;
  for (workspaceEntriesRemaining = g_AiWorkspace01Count; workspaceEntriesRemaining != 0;
      workspaceEntriesRemaining = workspaceEntriesRemaining + -1) {
    if (workspaceEntryCursor->armyRuntime != (ArmyRuntimeSlot *)0x0) {
      entityRuntime1 = workspaceEntryCursor->armyRuntime->linkedEntityRuntime;
      deltaYAbsQ12 = worldY - (entityRuntime1->common).pathCoordinate0Q12;
      if (deltaYAbsQ12 < 0) {
        deltaYAbsQ12 = -deltaYAbsQ12;
      }
      iVar1 = worldX - (entityRuntime1->common).pathCoordinate1Q12;
      if (iVar1 < 0) {
        iVar1 = -iVar1;
      }
      if (deltaYAbsQ12 + iVar1 < minimumManhattanDistanceQ12) {
        minimumManhattanDistanceQ12 = deltaYAbsQ12 + iVar1;
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
  int iVar1;
  Q12 deltaYAbsQ12;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  ArmyRuntimeSlot *armySlot1;
  
  minimumActiveManhattanDistanceQ12 = 0x7fffffff;
  workspaceEntryCursor = (AiRuntimeWorkspaceEntry *)g_AiWorkspaceBuffer00_Size0400;
  for (workspaceEntriesRemaining = g_AiWorkspace00Count; workspaceEntriesRemaining != 0;
      workspaceEntriesRemaining = workspaceEntriesRemaining + -1) {
    armySlot1 = workspaceEntryCursor->armyRuntime;
    if ((armySlot1 != (ArmyRuntimeSlot *)0x0) &&
       ((armySlot1->linkedEntityRuntime->common).commandState != 0)) {
      deltaYAbsQ12 = worldY - (armySlot1->modelNodeRuntime->worldTransform).translation.x;
      if (deltaYAbsQ12 < 0) {
        deltaYAbsQ12 = -deltaYAbsQ12;
      }
      iVar1 = worldX - (armySlot1->modelNodeRuntime->worldTransform).translation.y;
      if (iVar1 < 0) {
        iVar1 = -iVar1;
      }
      if (deltaYAbsQ12 + iVar1 < minimumActiveManhattanDistanceQ12) {
        minimumActiveManhattanDistanceQ12 = deltaYAbsQ12 + iVar1;
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
  int iVar1;
  Q12 deltaYAbsQ12;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  ModelRuntimeNode *modelNode1;
  
  minimumManhattanDistanceQ12 = 0x7fffffff;
  workspaceEntryCursor = g_AiWorkspaceBuffer02_Size0400;
  for (workspaceEntriesRemaining = g_AiWorkspace02Count; workspaceEntriesRemaining != 0;
      workspaceEntriesRemaining = workspaceEntriesRemaining + -1) {
    if (workspaceEntryCursor->armyRuntime != (ArmyRuntimeSlot *)0x0) {
      modelNode1 = workspaceEntryCursor->armyRuntime->modelNodeRuntime;
      deltaYAbsQ12 = worldY - (modelNode1->worldTransform).translation.x;
      if (deltaYAbsQ12 < 0) {
        deltaYAbsQ12 = -deltaYAbsQ12;
      }
      iVar1 = worldX - (modelNode1->worldTransform).translation.y;
      if (iVar1 < 0) {
        iVar1 = -iVar1;
      }
      if (deltaYAbsQ12 + iVar1 < minimumManhattanDistanceQ12) {
        minimumManhattanDistanceQ12 = deltaYAbsQ12 + iVar1;
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
  int iVar1;
  Q12 deltaYAbsQ12;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  ModelRuntimeNode *modelNode1;
  
  minimumManhattanDistanceQ12 = 0x7fffffff;
  workspaceEntryCursor = g_AiWorkspaceBuffer03_Size1000;
  for (workspaceEntriesRemaining = g_AiWorkspace03Count; workspaceEntriesRemaining != 0;
      workspaceEntriesRemaining = workspaceEntriesRemaining + -1) {
    if (workspaceEntryCursor->armyRuntime != (ArmyRuntimeSlot *)0x0) {
      modelNode1 = workspaceEntryCursor->armyRuntime->modelNodeRuntime;
      deltaYAbsQ12 = worldY - (modelNode1->worldTransform).translation.x;
      if (deltaYAbsQ12 < 0) {
        deltaYAbsQ12 = -deltaYAbsQ12;
      }
      iVar1 = worldX - (modelNode1->worldTransform).translation.y;
      if (iVar1 < 0) {
        iVar1 = -iVar1;
      }
      if (deltaYAbsQ12 + iVar1 < minimumManhattanDistanceQ12) {
        minimumManhattanDistanceQ12 = deltaYAbsQ12 + iVar1;
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
  int iVar1;
  Q12 deltaYAbsQ12;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  ModelRuntimeNode *modelNode1;
  
  minimumManhattanDistanceQ12 = 0x7fffffff;
  workspaceEntryCursor = (AiRuntimeWorkspaceEntry *)g_AiWorkspaceBuffer00_Size0400;
  for (workspaceEntriesRemaining = g_AiWorkspace00Count; workspaceEntriesRemaining != 0;
      workspaceEntriesRemaining = workspaceEntriesRemaining + -1) {
    if (workspaceEntryCursor->armyRuntime != (ArmyRuntimeSlot *)0x0) {
      modelNode1 = workspaceEntryCursor->armyRuntime->modelNodeRuntime;
      deltaYAbsQ12 = worldY - (modelNode1->worldTransform).translation.x;
      if (deltaYAbsQ12 < 0) {
        deltaYAbsQ12 = -deltaYAbsQ12;
      }
      iVar1 = worldX - (modelNode1->worldTransform).translation.y;
      if (iVar1 < 0) {
        iVar1 = -iVar1;
      }
      if (deltaYAbsQ12 + iVar1 < minimumManhattanDistanceQ12) {
        minimumManhattanDistanceQ12 = deltaYAbsQ12 + iVar1;
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
  ArmyRuntimeSlot *pAVar1;
  ModelRuntimeSlot *pMVar2;
  ArmyRuntimeSlot **armySlot1;
  int iVar3;
  AiTerrainFeatureWorkspaceEntry *terrainFeatureEntry;
  bool bVar4;
  ArmyRuntimeCreateEaxCf5 AVar5;
  
  iVar3 = g_AiWorkspace08Count;
  terrainFeatureEntry = g_AiWorkspaceBuffer08_Size0200;
  do {
    if (iVar3 == 0) {
      return;
    }
    if (armyAssetId == terrainFeatureEntry->armyAssetId) {
      workspaceRecord = terrainFeatureEntry->cell;
      bVar4 = AiPlacement_TestWorkspaceRecordAtPoint
                        (armyAssetId,workspaceRecord,factionIndex,(UiRootNode *)worldRuntime);
      if (!bVar4) {
        AVar5 = ArmyRuntime_CreateInstanceFromAssetCf
                          (4,(uint)(ushort)workspaceRecord->triangle0NormalAngles,
                           workspaceRecord->worldY,workspaceRecord->worldX,factionIndex,armyAssetId,
                           worldRuntime);
        armySlot1 = (ArmyRuntimeSlot **)AVar5.eax;
        if (AVar5.carry) {
          return;
        }
        modelNodeRuntime = armySlot1[1];
        pAVar1 = *armySlot1;
        modelNodeRuntime->movementPosition0Q12 = 0;
        pMVar2 = (pAVar1->modelRuntimeOrSavedOffset).modelRuntime;
        ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)modelNodeRuntime);
        ArmyRuntime_DispatchClassCommand(armySlot1,worldRuntime);
        EffectRuntimePool_CreateInstanceFromDefinitionCf
                  (EFFECT_RUNTIME_COMPLETION_NONE,(EffectRuntimeOwnerReference4)0x0,
                   (modelNodeRuntime->movementControl).turnVelocityAngle16,
                   (modelNodeRuntime->movementControl).movementAdvancePerTickQ12,
                   ((WorldRuntimeNodeModelPayload *)&modelNodeRuntime->factionIndex)->
                   worldRotationAngle0,modelNodeRuntime->depthBinClass,
                   modelNodeRuntime->runtimeState98,
                   ((GraphicsFixedVec3 *)&modelNodeRuntime->runtimeState94)->x,
                   (EffectDefinition *)pMVar2->attachments140[2].childLocalRotationAngle0,
                   worldRuntime);
        AiConstructionPlanner_ConsumeFactionPendingArmyAsset(armyAssetId,factionIndex);
        return;
      }
    }
    terrainFeatureEntry = terrainFeatureEntry + 1;
    iVar3 = iVar3 + -1;
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
  PackageLoadEntryEaxCf5 PVar1;
  StatusValueEaxCf5 SVar2;
  
  PVar1 = (PackageLoadEntryEaxCf5)(*g_MemoryApi.alloc)(0x400);
  workspaceAllocation = PVar1.bufferOrError;
  if (!PVar1.carry) {
    g_AiWorkspaceBuffer00_Size0400 = (AiWorkspace00EntryView8 *)workspaceAllocation;
    PVar1 = (PackageLoadEntryEaxCf5)(*g_MemoryApi.alloc)(0x200);
    if (!PVar1.carry) {
      g_AiWorkspaceBuffer01_Size0200 = PVar1.bufferOrError;
      PVar1 = (PackageLoadEntryEaxCf5)(*g_MemoryApi.alloc)(0x400);
      if (!PVar1.carry) {
        g_AiWorkspaceBuffer02_Size0400 = PVar1.bufferOrError;
        PVar1 = (PackageLoadEntryEaxCf5)(*g_MemoryApi.alloc)(0x1000);
        if (!PVar1.carry) {
          g_AiWorkspaceBuffer03_Size1000 = PVar1.bufferOrError;
          PVar1 = (PackageLoadEntryEaxCf5)(*g_MemoryApi.alloc)(0x40);
          if (!PVar1.carry) {
            g_AiWorkspaceBuffer04_Size0040 = PVar1.bufferOrError;
            PVar1 = (PackageLoadEntryEaxCf5)(*g_MemoryApi.alloc)(0x200);
            if (!PVar1.carry) {
              g_AiWorkspaceBuffer05_Size0200 = PVar1.bufferOrError;
              PVar1 = (PackageLoadEntryEaxCf5)(*g_MemoryApi.alloc)(0x400);
              if (!PVar1.carry) {
                g_AiWorkspaceBuffer06_Size0400 = PVar1.bufferOrError;
                PVar1 = (PackageLoadEntryEaxCf5)(*g_MemoryApi.alloc)(0x400);
                if (!PVar1.carry) {
                  g_AiWorkspaceBuffer07_Size0400 = PVar1.bufferOrError;
                  PVar1 = (PackageLoadEntryEaxCf5)(*g_MemoryApi.alloc)(0x200);
                  if (!PVar1.carry) {
                    g_AiWorkspaceBuffer08_Size0200 = PVar1.bufferOrError;
                    PVar1 = (PackageLoadEntryEaxCf5)(*g_MemoryApi.alloc)(0x1000);
                    if (!PVar1.carry) {
                      g_AiWorkspaceBuffer09_Size1000 = PVar1.bufferOrError;
                      PVar1 = (PackageLoadEntryEaxCf5)(*g_MemoryApi.alloc)(0x400);
                      if (!PVar1.carry) {
                        g_AiWorkspaceBuffer10_Size0400 = PVar1.bufferOrError;
                        PVar1 = (PackageLoadEntryEaxCf5)(*g_MemoryApi.alloc)(0x1000);
                        if (!PVar1.carry) {
                          g_AiWorkspaceBuffer11_Size1000 = PVar1.bufferOrError;
                          PVar1 = (PackageLoadEntryEaxCf5)(*g_MemoryApi.alloc)(0x200);
                          technologyCandidateWorkspaceAllocation = PVar1.bufferOrError;
                          if (!PVar1.carry) {
                            g_AiWorkspaceBuffer12_Size0200 = technologyCandidateWorkspaceAllocation;
                            PVar1 = (PackageLoadEntryEaxCf5)(*g_MemoryApi.alloc)(0x400);
                            if (!PVar1.carry) {
                              g_AiWorkspaceBuffer13_Size0400 = PVar1.bufferOrError;
                              PVar1 = (PackageLoadEntryEaxCf5)(*g_MemoryApi.alloc)(0x100);
                              if (!PVar1.carry) {
                                g_AiWorkspaceBuffer14_Size0100 = PVar1.bufferOrError;
                                PVar1 = Package_LoadEntry((word *)u_engine_ki_dat_0053c5e4);
                                knowledgeDataImage = PVar1.bufferOrError;
                                if (!PVar1.carry) {
                                  PVar1 = (PackageLoadEntryEaxCf5)((uint5)PVar1 & 0xffffffff);
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
  SVar2.valueOrError = (dword)PVar1.bufferOrError;
  SVar2.carry = PVar1.carry;
  return SVar2;
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
  FieldGridCell **ppFVar1;
  uint uVar2;
  bool bVar3;
  
  uVar2 = g_AiWorkspace09Count;
  ppFVar1 = g_AiWorkspaceBuffer09_Size1000;
  if (g_AiWorkspace09Count < 0x400) {
    bVar3 = AiPrimaryWorkspace_IsPointOutsideAllEntryExtentsCf
                      (currentCell->worldY,currentCell->worldX);
    if (!bVar3) {
      ppFVar1[uVar2] = currentCell;
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
  FieldGridCell **ppFVar1;
  uint uVar2;
  bool bVar3;
  
  uVar2 = g_AiWorkspace10Count;
  ppFVar1 = g_AiWorkspaceBuffer10_Size0400;
  if (g_AiWorkspace10Count < 0x100) {
    bVar3 = AiPrimaryWorkspace_IsPointOutsideAllEntryExtentsCf
                      (currentCell->worldY,currentCell->worldX);
    if (!bVar3) {
      ppFVar1[uVar2] = currentCell;
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
  uint uVar1;
  dword dVar2;
  int iVar3;
  uint uVar4;
  AiCandidateWorkspaceEntry *candidateEntry;
  
  dVar2 = (*g_RandomGeneratorState.next)();
  uVar1 = g_AiCandidateWorkspaceEntryCount;
  candidateEntry = g_AiWorkspaceBuffer13_Size0400;
  if (1 < weightRange) {
    iVar3 = weightRange * 5 + dVar2 % weightRange;
    for (uVar4 = g_AiCandidateWorkspaceEntryCount; uVar4 != 0; uVar4 = uVar4 - 1) {
      if (((g_AiWorkspaceBuffer13_Size0400[uVar4 - 1].weightedScoreAndKind & 0xf) == entryKind) &&
         ((g_AiWorkspaceBuffer13_Size0400[uVar4 - 1].entityIdAndMultiplicity & 0xffff) == entityId))
      {
        g_AiWorkspaceBuffer13_Size0400[uVar4 - 1].entityIdAndMultiplicity =
             g_AiWorkspaceBuffer13_Size0400[uVar4 - 1].entityIdAndMultiplicity + 0x10000;
        candidateEntry = candidateEntry + (uVar4 - 1);
        candidateEntry->weightedScoreAndKind = candidateEntry->weightedScoreAndKind + iVar3 * 0x10;
        return;
      }
    }
    if (g_AiCandidateWorkspaceEntryCount < 0x80) {
      g_AiWorkspaceBuffer13_Size0400[g_AiCandidateWorkspaceEntryCount].entityIdAndMultiplicity =
           entityId + 0x10000;
      g_AiCandidateWorkspaceEntryCount = g_AiCandidateWorkspaceEntryCount + 1;
      candidateEntry[uVar1].weightedScoreAndKind = iVar3 * 0x10 | entryKind;
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

