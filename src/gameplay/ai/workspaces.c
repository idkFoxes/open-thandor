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
AiPreservedFactionIndexEdxResult
AiWorkspaceAssetCandidate_AddWeightedEntry
          (AiCandidateScore32 baseWeight,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  dword weightRange;
  int extraout_ECX;
  int extraout_ECX_00;
  int iVar1;
  FactionRuntimeIndex in_EDX;
  PckArmyAssetIdCatalog extraout_EDX;
  PckArmyAssetIdCatalog extraout_EDX_00;
  PckArmyAssetIdCatalog armyAssetId_00;
  AiTerrainFeatureWorkspaceEntry *terrainFeatureEntry;
  bool bVar2;
  AiWorkspaceCountEaxPreservedEdxCarrier64 AVar3;
  
  terrainFeatureEntry = g_AiWorkspaceBuffer08_Size0200;
  bVar2 = false;
  if ((g_AiWorkspace08Count != 0) &&
     (AiPrimaryWorkspace_HasUnassignedEntryByIdCf(armyAssetId), iVar1 = extraout_ECX,
     armyAssetId_00 = extraout_EDX, !bVar2)) {
    do {
      bVar2 = armyAssetId_00 < terrainFeatureEntry->armyAssetId;
      if ((armyAssetId_00 == terrainFeatureEntry->armyAssetId) &&
         (AiPlacement_TestMode4AtWorkspaceRecord
                    (armyAssetId_00,terrainFeatureEntry->cell,factionIndex,worldRuntime),
         iVar1 = extraout_ECX_00, armyAssetId_00 = extraout_EDX_00, !bVar2)) {
        AVar3 = AiPrimaryWorkspace_CountAssignedEntriesById(armyAssetId);
        weightRange = (uint)(baseWeight * 3) / ((int)AVar3 + 3U);
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
        return (AiPreservedFactionIndexEdxResult)in_EDX;
      }
      terrainFeatureEntry = terrainFeatureEntry + 1;
      iVar1 = iVar1 + -1;
    } while (iVar1 != 0);
  }
  return (AiPreservedFactionIndexEdxResult)in_EDX;
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
AiPlanningDispatchRegisterContinuityResult
AiPlanning_RebuildFactionWorkspaces
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  byte *pbVar1;
  byte *pbVar2;
  PckArmyAssetIdCatalog PVar3;
  int *piVar4;
  uint uVar5;
  GraphicsWorldCoordinateQ12 GVar6;
  GraphicsWorldCoordinateQ12 GVar7;
  void *pvVar8;
  AiPlanningDispatchRegisterContinuityResult AVar9;
  uint uVar10;
  AiRuntimeWorkspaceEntry *pAVar11;
  GridScratchStateMask GVar12;
  ArmyAssetRecordPrefix *armyDefinition1;
  dword dVar13;
  AiPlanningPhaseIndex in_ECX;
  FactionArmyAssetCount FVar14;
  dword dVar15;
  uint uVar16;
  FactionRuntimeIndex in_EDX;
  undefined4 *puVar17;
  AiPlanningGridScratchEdxContinuityResult AVar18;
  int iVar19;
  byte *pbVar20;
  ArmyAssetRecordPrefix **armyAssetRegistryCursor26;
  WorldRuntimeNode *worldNode1;
  dword *pdVar21;
  uint *puVar22;
  GridScratchCell *pGVar23;
  ArmyAssetRecordPrefix **armyAssetRegistryCursor31;
  AiTargetWorkspaceEntry *targetEntry;
  bool bVar24;
  undefined1 uVar25;
  AiPlanningGridScanLoopContinuityResult AVar26;
  GameFactionCapabilityCfVolatileContinuityResult GVar27;
  ModelTechnologyHierarchyCfVolatileContinuityResult MVar28;
  AiTechnologyPlanningLoopRegisterContinuityResult AVar29;
  ArmyRuntimeSlot *armySlot1;
  GameEntityRuntime *entityRuntime;
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
  dVar15 = factionIndex;
  if (worldNode1 != (WorldRuntimeNode *)0x0) {
    do {
      uVar5 = g_AiWorkspace01Count;
      runtimeWorkspaceEntry13 = g_AiWorkspaceBuffer01_Size0200;
      uVar16 = g_AiWorkspace00Count;
      pbVar20 = g_AiWorkspaceBuffer00_Size0400;
      if (worldNode1[2].common.nextNode == (WorldRuntimeNode *)0x0) {
        armySlot1 = worldNode1->runtimePayload;
        entityRuntime = armySlot1->linkedEntityRuntime;
        if (dVar15 == (entityRuntime->common).ownership.ownerIndex) {
          PVar3 = (entityRuntime->common).runtimeIdentityOrArmyAssetId;
          if (PVar3 < ARM_0300_BUILDING_MDL0301) {
            if (g_AiWorkspace01Count < 0x40) {
              g_AiWorkspaceBuffer01_Size0200[g_AiWorkspace01Count].armyRuntime = armySlot1;
              runtimeWorkspaceEntry13[uVar5].armyAssetId = PVar3;
              g_AiWorkspace01Count = g_AiWorkspace01Count + 1;
            }
          }
          else if (g_AiWorkspace00Count < 0x80) {
            *(ArmyRuntimeSlot **)(g_AiWorkspaceBuffer00_Size0400 + g_AiWorkspace00Count * 8) =
                 armySlot1;
            *(PckArmyAssetIdCatalog *)(pbVar20 + uVar16 * 8 + 4) = PVar3;
            g_AiWorkspace00Count = g_AiWorkspace00Count + 1;
            if (PVar3 == ARM_0300_BUILDING_MDL0301) {
              g_AiWorkspaceOwnedAsset300Runtime = armySlot1;
            }
          }
        }
        else {
          uVar16 = *(uint *)((entityRuntime->common).damageState.reserved0C_2B + 0x10) >>
                   ((char)dVar15 * '\x02' & 0x1fU);
          bVar24 = false;
          if ((entityRuntime->common).ownership.ownerIndex != 0) {
            GVar27 = GameFactionRuntime_TestCapabilityBitClearCf
                               ((entityRuntime->common).ownership.ownerIndex,dVar15);
            uVar10 = g_AiWorkspace03Count;
            pAVar11 = g_AiWorkspaceBuffer03_Size1000;
            uVar5 = g_AiWorkspace02Count;
            runtimeWorkspaceEntry13 = g_AiWorkspaceBuffer02_Size0400;
            puVar17 = (undefined4 *)GVar27.preservedEdxCallerValue;
            dVar15 = GVar27.preservedEaxCallerValue;
            if (bVar24) {
              if (((uVar16 & 2) == 0) &&
                 (((uVar16 & 1) == 0 ||
                  (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand
                   [*(int *)(*(int *)*puVar17 + 0x4c)] != ArmyRuntime_ClassCommandHandlerGroupACf)))
                 ) {
                PVar3 = puVar17[0x28];
                if (g_AiWorkspace03Count < 0x200) {
                  g_AiWorkspaceBuffer03_Size1000[g_AiWorkspace03Count].armyRuntime = armySlot1;
                  pAVar11[uVar10].armyAssetId = PVar3;
                  g_AiWorkspace03Count = g_AiWorkspace03Count + 1;
                }
              }
              else {
                PVar3 = puVar17[0x28];
                if (g_AiWorkspace02Count < 0x80) {
                  g_AiWorkspaceBuffer02_Size0400[g_AiWorkspace02Count].armyRuntime = armySlot1;
                  runtimeWorkspaceEntry13[uVar5].armyAssetId = PVar3;
                  g_AiWorkspace02Count = g_AiWorkspace02Count + 1;
                }
              }
            }
          }
        }
      }
      worldNode1 = (worldNode1->common).nextNode;
    } while (worldNode1 != (WorldRuntimeNode *)0x0);
    pdVar21 = g_GameFactionRuntimeImage.records[dVar15].primaryArmyAssetPointersOrIds;
    pbVar20 = g_AiWorkspaceBuffer00_Size0400;
    uVar16 = g_AiWorkspace00Count;
    for (FVar14 = g_GameFactionRuntimeImage.records[dVar15].primaryArmyAssetCount; FVar14 != 0;
        FVar14 = FVar14 - 1) {
      PVar3 = *(PckArmyAssetIdCatalog *)(*pdVar21 + 8);
      g_AiWorkspaceBuffer00_Size0400 = pbVar20;
      g_AiWorkspace00Count = uVar16;
      if (uVar16 < 0x80) {
        pbVar1 = pbVar20 + uVar16 * 8;
        pbVar1[0] = 0;
        pbVar1[1] = 0;
        pbVar1[2] = 0;
        pbVar1[3] = 0;
        *(PckArmyAssetIdCatalog *)(pbVar20 + uVar16 * 8 + 4) = PVar3;
        g_AiWorkspace00Count = g_AiWorkspace00Count + 1;
      }
      uVar16 = g_AiWorkspace04Count;
      runtimeWorkspaceEntry13 = g_AiWorkspaceBuffer04_Size0040;
      if (g_AiWorkspace04Count < 8) {
        g_AiWorkspaceBuffer04_Size0040[g_AiWorkspace04Count].armyRuntime = (ArmyRuntimeSlot *)0x0;
        runtimeWorkspaceEntry13[uVar16].armyAssetId = PVar3;
        g_AiWorkspace04Count = g_AiWorkspace04Count + 1;
      }
      pdVar21 = pdVar21 + 1;
      pbVar20 = g_AiWorkspaceBuffer00_Size0400;
      uVar16 = g_AiWorkspace00Count;
    }
    pdVar21 = g_GameFactionRuntimeImage.records[dVar15].secondaryArmyAssetPointersOrIds;
    runtimeWorkspaceEntry13 = g_AiWorkspaceBuffer01_Size0200;
    uVar5 = g_AiWorkspace01Count;
    for (FVar14 = g_GameFactionRuntimeImage.records[dVar15].secondaryArmyAssetCount;
        pbVar1 = pbVar20, uVar10 = uVar16, FVar14 != 0; FVar14 = FVar14 - 1) {
      PVar3 = *(PckArmyAssetIdCatalog *)(*pdVar21 + 8);
      g_AiWorkspaceBuffer00_Size0400 = pbVar20;
      g_AiWorkspace00Count = uVar16;
      g_AiWorkspaceBuffer01_Size0200 = runtimeWorkspaceEntry13;
      g_AiWorkspace01Count = uVar5;
      if (PVar3 < ARM_0300_BUILDING_MDL0301) {
        if (uVar5 < 0x40) {
          runtimeWorkspaceEntry13[uVar5].armyRuntime = (ArmyRuntimeSlot *)0x0;
          runtimeWorkspaceEntry13[uVar5].armyAssetId = PVar3;
          g_AiWorkspace01Count = g_AiWorkspace01Count + 1;
        }
      }
      else if (uVar16 < 0x80) {
        pbVar1 = pbVar20 + uVar16 * 8;
        pbVar1[0] = 0;
        pbVar1[1] = 0;
        pbVar1[2] = 0;
        pbVar1[3] = 0;
        *(PckArmyAssetIdCatalog *)(pbVar20 + uVar16 * 8 + 4) = PVar3;
        g_AiWorkspace00Count = g_AiWorkspace00Count + 1;
      }
      pdVar21 = pdVar21 + 1;
      pbVar20 = g_AiWorkspaceBuffer00_Size0400;
      uVar16 = g_AiWorkspace00Count;
      runtimeWorkspaceEntry13 = g_AiWorkspaceBuffer01_Size0200;
      uVar5 = g_AiWorkspace01Count;
    }
    for (; g_AiWorkspaceBuffer00_Size0400 = pbVar1, g_AiWorkspace00Count = uVar16,
        g_AiWorkspaceBuffer01_Size0200 = runtimeWorkspaceEntry13, g_AiWorkspace01Count = uVar5,
        uVar10 != 0; uVar10 = uVar10 - 1) {
      piVar4 = *(int **)pbVar20;
      if (piVar4 != (int *)0x0) {
        iVar19 = *piVar4;
        if (*(int *)(iVar19 + 0x4c) == 0xb) {
          iVar19 = piVar4[0x18];
          if ((piVar4[0x2e] == 1) && (uVar16 < 0x80)) {
            pbVar2 = pbVar1 + uVar16 * 8;
            pbVar2[0] = 0;
            pbVar2[1] = 0;
            pbVar2[2] = 0;
            pbVar2[3] = 0;
            *(int *)(pbVar1 + uVar16 * 8 + 4) = iVar19;
            g_AiWorkspace00Count = g_AiWorkspace00Count + 1;
          }
        }
        else if (*(int *)(iVar19 + 0x4c) == 0x16) {
          PVar3 = piVar4[0x18];
          if ((piVar4[0x2b] == 1) && (uVar5 < 0x40)) {
            runtimeWorkspaceEntry13[uVar5].armyRuntime = (ArmyRuntimeSlot *)0x0;
            runtimeWorkspaceEntry13[uVar5].armyAssetId = PVar3;
            g_AiWorkspace01Count = g_AiWorkspace01Count + 1;
          }
        }
        else if (((*(int *)(iVar19 + 0x4c) == 0xd) && (PVar3 = piVar4[0x18], piVar4[0x2e] == 1)) &&
                (uVar5 < 0x40)) {
          runtimeWorkspaceEntry13[uVar5].armyRuntime = (ArmyRuntimeSlot *)0x0;
          runtimeWorkspaceEntry13[uVar5].armyAssetId = PVar3;
          g_AiWorkspace01Count = g_AiWorkspace01Count + 1;
        }
      }
      pbVar20 = pbVar20 + 8;
      pbVar1 = g_AiWorkspaceBuffer00_Size0400;
      uVar16 = g_AiWorkspace00Count;
      runtimeWorkspaceEntry13 = g_AiWorkspaceBuffer01_Size0200;
      uVar5 = g_AiWorkspace01Count;
    }
  }
  uVar5 = worldRuntime->fieldGrid->gridWidth;
  pbVar20 = worldRuntime->fieldGrid->cells[0].runtime00_07 + factionIndex;
  pGVar23 = g_GridScratchPrimary + g_GridScratchWidth * 2 + 2;
  AVar26 = AiPlanning_CollectActiveGridMaskClasses();
  iVar19 = uVar5 * 0x80;
  uVar16 = uVar5;
  do {
    do {
      dVar15 = AVar26.preservedEdxGridScratchRowStrideBytes;
      dVar13 = AVar26.preservedEcxInteriorGridRowCount;
      if ((((*(uint *)((int)&pGVar23->stateMask + dVar15) & 0x80000000) == 0) &&
          ((pbVar20[iVar19 + 0x70] & 0xf9) != 0)) &&
         (((((*(TerrainOccupancyMask *)(pbVar20 + 0x70) & 0xf9) == 0 ||
            (((pbVar20[0xf0] & 0xf9) == 0 || ((pbVar20[iVar19 + -0x10] & 0xf9) == 0)))) ||
           (((pbVar20[iVar19 + 0xf0] & 0xf9) == 0 ||
            (((pbVar20[uVar5 * 0x100 + -0x10] & 0xf9) == 0 ||
             ((pbVar20[uVar5 * 0x100 + 0x70] & 0xf9) == 0)))))) &&
          ((GVar12 = pGVar23->stateMask | pGVar23[3].stateMask |
                     *(GridScratchStateMask *)((int)&pGVar23[-3].stateMask + dVar15) |
                     *(GridScratchStateMask *)((int)&pGVar23[3].stateMask + dVar15) |
                     *(GridScratchStateMask *)((int)&pGVar23[-3].stateMask + dVar15 * 2) |
                     *(GridScratchStateMask *)((int)&pGVar23->stateMask + dVar15 * 2) |
                     *(GridScratchStateMask *)((int)&pGVar23->stateMask + dVar15),
           (g_AiActiveGridMaskClass0 & GVar12) == 0 ||
           ((((g_AiActiveGridMaskClass1 & GVar12) == 0 || ((g_AiActiveGridMaskClass2 & GVar12) == 0)
             ) || ((g_AiActiveGridMaskClass3 & GVar12) == 0)))))))) {
        AVar18 = AiSiteCandidate_AddGeneralCellIfSeparated();
        AVar26.preservedEdxGridScratchRowStrideBytes = AVar18.preservedEdxGridScratchRowStrideBytes;
        AVar26.preservedEcxInteriorGridRowCount = dVar13;
      }
      dVar15 = AVar26.preservedEdxGridScratchRowStrideBytes;
      dVar13 = AVar26.preservedEcxInteriorGridRowCount;
      GVar12 = *(GridScratchStateMask *)((int)&pGVar23->stateMask + dVar15);
      if ((((pbVar20[iVar19 + 0x70] & 0x10) != 0) && ((GVar12 & 0x80000100) == 0)) &&
         ((GVar12 = GVar12 | pGVar23->stateMask | pGVar23[4].stateMask |
                    *(GridScratchStateMask *)((int)&pGVar23[-4].stateMask + dVar15) |
                    *(GridScratchStateMask *)((int)&pGVar23[4].stateMask + dVar15) |
                    *(GridScratchStateMask *)((int)&pGVar23[-4].stateMask + dVar15 * 2) |
                    *(GridScratchStateMask *)((int)&pGVar23->stateMask + dVar15 * 2),
          (GVar12 & 0x81000000) == 0 ||
          (((GVar12 & 0x82000000) == 0 || ((GVar12 & 0x90000000) == 0)))))) {
        if ((*(uint *)(pbVar20 + (iVar19 - factionIndex) + 0x50) & 0x1800) == 0) {
          AVar18 = AiSiteCandidate_AddFlaggedCellIfSeparated();
          AVar26.preservedEdxGridScratchRowStrideBytes =
               AVar18.preservedEdxGridScratchRowStrideBytes;
          AVar26.preservedEcxInteriorGridRowCount = dVar13;
        }
        pbVar20 = pbVar20 + factionIndex + uVar5 * -0x80 + (iVar19 - factionIndex);
      }
      dVar15 = AVar26.preservedEdxGridScratchRowStrideBytes;
      dVar13 = AVar26.preservedEcxInteriorGridRowCount;
      if (((pbVar20[iVar19 + 0x70] & 0xf9) != 0) &&
         ((*(uint *)((int)&pGVar23->stateMask + dVar15) & 0xf0003f00) == 0)) {
        if (((*(uint *)((int)&pGVar23->stateMask + dVar15) & 0xf000ff00) == 0) &&
           (((pGVar23->stateMask | pGVar23[4].stateMask |
              *(GridScratchStateMask *)((int)&pGVar23[-4].stateMask + dVar15) |
              *(GridScratchStateMask *)((int)&pGVar23[4].stateMask + dVar15) |
              *(GridScratchStateMask *)((int)&pGVar23[-4].stateMask + dVar15 * 2) |
             *(GridScratchStateMask *)((int)&pGVar23->stateMask + dVar15 * 2)) & 0xf0000000) == 0))
        {
          puVar22 = (uint *)((int)pGVar23 - dVar15);
          pGVar23 = (GridScratchCell *)((int)puVar22 + dVar15);
          if (((*puVar22 | puVar22[0x10] | *(uint *)((int)puVar22 + dVar15 * 2 + -0x40) |
                *(uint *)((int)puVar22 + dVar15 * 2 + 0x40) | puVar22[dVar15 - 0x10] |
               puVar22[dVar15]) & 0xf0000000) == 0) {
            if ((*(uint *)(pbVar20 + (iVar19 - factionIndex) + 0x50) & 0x1800) != 0) {
              AVar18 = AiSiteCandidate_AddTerrainFeatureCellIfSeparated();
              AVar26.preservedEdxGridScratchRowStrideBytes =
                   AVar18.preservedEdxGridScratchRowStrideBytes;
              AVar26.preservedEcxInteriorGridRowCount = dVar13;
            }
            pbVar20 = pbVar20 + factionIndex + uVar5 * -0x80 + (iVar19 - factionIndex);
          }
        }
        dVar15 = AVar26.preservedEdxGridScratchRowStrideBytes;
        dVar13 = AVar26.preservedEcxInteriorGridRowCount;
        if ((((((((pGVar23->stateMask | pGVar23[4].stateMask |
                   *(GridScratchStateMask *)((int)&pGVar23[-4].stateMask + dVar15) |
                   *(GridScratchStateMask *)((int)&pGVar23[4].stateMask + dVar15) |
                   *(GridScratchStateMask *)((int)&pGVar23[-4].stateMask + dVar15 * 2) |
                  *(GridScratchStateMask *)((int)&pGVar23->stateMask + dVar15 * 2)) & 0xf0000000) ==
                 0) && ((*(uint *)(pbVar20 + ((iVar19 + 0x50) - factionIndex)) & 0x1800) == 0)) &&
              ((*(uint *)(pbVar20 + (0x50 - factionIndex)) & 0x1800) == 0)) &&
             (((*(uint *)(pbVar20 + (0xd0 - factionIndex)) & 0x1800) == 0 &&
              ((*(uint *)(pbVar20 + ((iVar19 + -0x30) - factionIndex)) & 0x1800) == 0)))) &&
            ((*(uint *)(pbVar20 + ((iVar19 + 0xd0) - factionIndex)) & 0x1800) == 0)) &&
           (((*(uint *)(pbVar20 + uVar5 * 0x100 + (-0x30 - factionIndex)) & 0x1800) == 0 &&
            ((*(uint *)(pbVar20 + uVar5 * 0x100 + (0x50 - factionIndex)) & 0x1800) == 0)))) {
          AVar18 = AiEntityCandidateWorkspace09_AddOutsidePrimaryExtents();
          AVar26.preservedEdxGridScratchRowStrideBytes =
               AVar18.preservedEdxGridScratchRowStrideBytes;
          AVar26.preservedEcxInteriorGridRowCount = dVar13;
        }
        dVar15 = AVar26.preservedEdxGridScratchRowStrideBytes;
        dVar13 = AVar26.preservedEcxInteriorGridRowCount;
        if (((*(uint *)((int)&pGVar23->stateMask + dVar15) & 0xf000ff00) == 0) &&
           (((pGVar23->stateMask | pGVar23[4].stateMask |
              *(GridScratchStateMask *)((int)&pGVar23[-4].stateMask + dVar15) |
              *(GridScratchStateMask *)((int)&pGVar23[4].stateMask + dVar15) |
              *(GridScratchStateMask *)((int)&pGVar23[-4].stateMask + dVar15 * 2) |
             *(GridScratchStateMask *)((int)&pGVar23->stateMask + dVar15 * 2)) & 0xf0000000) == 0))
        {
          puVar22 = (uint *)((int)pGVar23 - dVar15);
          pGVar23 = (GridScratchCell *)((int)puVar22 + dVar15);
          if ((((((*puVar22 | puVar22[0x10] | *(uint *)((int)puVar22 + dVar15 * 2 + -0x40) |
                   *(uint *)((int)puVar22 + dVar15 * 2 + 0x40) | puVar22[dVar15 - 0x10] |
                  puVar22[dVar15]) & 0x70000000) == 0) &&
               ((((*(uint *)(pbVar20 + (0x50 - factionIndex)) & 0x1800) == 0 &&
                 ((*(uint *)(pbVar20 + (0xd0 - factionIndex)) & 0x1800) == 0)) &&
                ((*(uint *)(pbVar20 + ((iVar19 + -0x30) - factionIndex)) & 0x1800) == 0)))) &&
              ((((*(uint *)(pbVar20 + ((iVar19 + 0xd0) - factionIndex)) & 0x1800) == 0 &&
                ((*(uint *)(pbVar20 + uVar5 * 0x100 + (-0x30 - factionIndex)) & 0x1800) == 0)) &&
               ((*(uint *)(pbVar20 + uVar5 * 0x100 + (0x50 - factionIndex)) & 0x1800) == 0)))) &&
             ((*(uint *)(pbVar20 + ((iVar19 + 0x50) - factionIndex)) & 0x1800) == 0)) {
            AVar18 = AiEntityCandidateWorkspace10_AddOutsidePrimaryExtents();
            AVar26.preservedEdxGridScratchRowStrideBytes =
                 AVar18.preservedEdxGridScratchRowStrideBytes;
            AVar26.preservedEcxInteriorGridRowCount = dVar13;
          }
        }
      }
      pbVar20 = pbVar20 + 0x80;
      pGVar23 = pGVar23 + 4;
      uVar16 = uVar16 - 1;
    } while (uVar16 != 0);
    pGVar23 = pGVar23 + g_GridScratchWidth * 3;
    uVar16 = uVar5 & 0x1ffffff;
    dVar15 = AVar26.preservedEcxInteriorGridRowCount - 1;
    AVar26.preservedEdxGridScratchRowStrideBytes = AVar26.preservedEdxGridScratchRowStrideBytes;
    AVar26.preservedEcxInteriorGridRowCount = dVar15;
    uVar10 = g_AiWorkspace00Count;
  } while (dVar15 != 0);
  for (; uVar10 != 0; uVar10 = uVar10 - 1) {
  }
  armyAssetRegistryCursor31 = g_ArmyAssetRecordRegistry;
  dVar15 = 0x300;
  armyAssetRegistryCursor26 = g_AiWorkspaceBuffer11_Size1000;
  do {
    armyDefinition1 = *armyAssetRegistryCursor31;
    if ((armyDefinition1 != (ArmyAssetRecordPrefix *)0x0) &&
       (bVar24 = false, (armyDefinition1[1].selectionDetailTemplateVariantIndex & 1) != 0)) {
      MVar28 = ModelDefinitionHierarchy_AllTechnologyUnlockedForFactionCf
                         (factionIndex,(ModelDefinitionHierarchyNodeAddress32)armyDefinition1);
      dVar15 = MVar28.preservedEcxCallerValue;
      armyDefinition1 = (ArmyAssetRecordPrefix *)MVar28.preservedEaxDefinitionNodeAddress;
      if (((!bVar24) &&
          ((armyDefinition1[1].selectionDetailTemplateVariantIndex & MVar28.preservedEdxCallerValue)
           != 0)) &&
         (((armyDefinition1->registryId < ARM_0300_BUILDING_MDL0301 ||
           ((ARM_0320_BUILDING_MDL0311|ARM_0019_UNIT_MDL0101) < armyDefinition1->registryId)) &&
          (g_AiWorkspace11Count < 0x400)))) {
        *armyAssetRegistryCursor26 = armyDefinition1;
        g_AiWorkspace11Count = g_AiWorkspace11Count + 1;
        armyAssetRegistryCursor26 = armyAssetRegistryCursor26 + 1;
      }
    }
    armyAssetRegistryCursor31 = armyAssetRegistryCursor31 + 1;
    dVar15 = dVar15 - 1;
    runtimeWorkspaceEntry13 = g_AiWorkspaceBuffer02_Size0400;
    uVar16 = g_AiWorkspace02Count;
    targetEntry = g_AiWorkspaceBuffer07_Size0400;
  } while (dVar15 != 0);
  while ((uVar16 != 0 &&
         (armySlot1 = runtimeWorkspaceEntry13->armyRuntime, g_AiWorkspace07Count < 0x40))) {
    if (armySlot1 != (ArmyRuntimeSlot *)0x0) {
      targetEntry->armyRuntime = armySlot1;
      modelNode = armySlot1->modelNodeRuntime;
      GVar6 = (modelNode->worldTransform).translation.x;
      GVar7 = (modelNode->worldTransform).translation.y;
      targetEntry->modelRuntime = modelNode;
      targetEntry->worldXQ12 = GVar6;
      targetEntry->worldYQ12 = GVar7;
      g_AiWorkspace07Count = g_AiWorkspace07Count + 1;
      targetEntry = targetEntry + 1;
    }
    runtimeWorkspaceEntry13 = runtimeWorkspaceEntry13 + 1;
    uVar16 = uVar16 - 1;
  }
  dVar15 = factionIndex * 0x740;
  pbVar20 = g_AiWorkspaceBuffer00_Size0400;
  for (uVar16 = g_AiWorkspace00Count; uVar16 != 0; uVar16 = uVar16 - 1) {
    armySlot1 = *(ArmyRuntimeSlot **)pbVar20;
    uVar25 = 0;
    if (armySlot1 != (ArmyRuntimeSlot *)0x0) {
      pvVar8 = armySlot1->definitionOrAsset;
      iVar19 = 0x1c;
      do {
        AVar29 = AiTechnologyCandidate_IsCurrentlyAvailableCf
                           (*(PckTechnologyIdCatalog *)((int)pvVar8 + iVar19 * 4 + 0x1c4),dVar15);
        dVar15 = AVar29.preservedEdxFactionRecordOffset;
        uVar16 = AVar29.preservedEcxSourceArmyEntriesRemaining;
        dVar13 = AVar29.preservedEaxTechnologyPanelIndex;
        if (!(bool)uVar25) {
          AVar29 = AiTechnologyPlanning_AddCandidateRecord
                             (armySlot1,
                              *(PckTechnologyIdCatalog *)((int)pvVar8 + dVar13 * 4 + 0x1c4));
          dVar15 = AVar29.preservedEdxFactionRecordOffset;
          uVar16 = AVar29.preservedEcxSourceArmyEntriesRemaining;
          dVar13 = AVar29.preservedEaxTechnologyPanelIndex;
        }
        iVar19 = dVar13 - 1;
      } while (iVar19 != 0);
    }
    pbVar20 = pbVar20 + 8;
  }
  AVar9.preservedEcxPlanningPhase = in_ECX;
  AVar9.preservedEdxFactionIndex = in_EDX;
  return AVar9;
}

/* Address: 0x0053BF30.
   Ownership: gameplay/ai/workspaces.
   Purpose: When candidate 0x14A is present, evaluates typed workspace12 entries through their callback table,
   selects the highest positive score, applies faction-anchor pressure to the knowledge weight, and adds the chosen
   candidate. It is distinct from FrontendPlayerIndex_V306, PlayerRuntimeId, active-faction masks or codes, and
   PCK-backed ArmyAssetId, ModelDefinitionId, and TechnologyId domains.
   Local calls: AiPrimaryWorkspace_HasEntryByIdCf, AiCandidateWorkspace_AddOrAccumulateWeightedEntry.
*/
AiPreservedFactionIndexEdxResult
AiStrategicCandidate_AddBestWorkspace12Entry
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  AiTechnologyCandidateScore AVar1;
  int iVar2;
  int extraout_ECX;
  FactionRuntimeIndex in_EDX;
  AiTechnologyPlanningCandidate *pAVar3;
  undefined1 in_CF;
  uint weightRange;
  AiCandidateEntryKind entryKind;
  RuntimeToken entityId;
  
  AiPrimaryWorkspace_HasEntryByIdCf(ARM_0330_BUILDING_MDL0303);
  if ((bool)in_CF) {
    iVar2 = 8;
    do {
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
    iVar2 = 0;
    if (g_AiWorkspace12Count != 0) {
      entryKind = 2;
      weightRange = (g_AiKnowledgeData->parameters).workspace12BestCandidateBaseWeight;
      entityId = 0;
      pAVar3 = g_AiWorkspaceBuffer12_Size0200;
      do {
        AVar1 = (*g_AiTechnologyCandidateScoreCallbackTable[pAVar3->scoreKind08])
                          (factionIndex,pAVar3->technologyId00,worldRuntime);
        if (iVar2 < AVar1) {
          entityId = pAVar3->technologyId00;
          iVar2 = AVar1;
        }
        pAVar3 = pAVar3 + 1;
      } while (extraout_ECX != 1);
      if (iVar2 != 0) {
        if (g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorCooldown != 0) {
          weightRange = weightRange >> 1;
        }
        AiCandidateWorkspace_AddOrAccumulateWeightedEntry(entityId,weightRange,entryKind);
      }
    }
  }
  return (AiPreservedFactionIndexEdxResult)in_EDX;
}

/* Address: 0x00537420.
   Ownership: gameplay/ai/workspaces.
   Purpose: Clears the global AI candidate-workspace entry count.
*/
AiPreservedFactionIndexEdxResult __cdecl AiCandidateWorkspace_Clear(void)

{
  AiPreservedFactionIndexEdxResult in_EDX;
  
  g_AiCandidateWorkspaceEntryCount = 0;
  return (AiPreservedFactionIndexEdxResult)in_EDX.preservedEdxFactionIndex;
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
  byte *candidateWorkspaceSourceCursor;
  byte *factionImageDestinationCursor;
  
  candidateWorkspaceSourceCursor = (byte *)g_AiWorkspaceBuffer13_Size0400;
  iVar1 = g_AiCandidateWorkspaceEntryCount;
  if (2 < g_AiCandidateWorkspaceEntryCount) {
    iVar1 = 3;
  }
  *(int *)((int)(g_GameFactionRuntimeImage.records[0].candidateCache.savedEntries + 3) +
          factionImageByteOffset) = iVar1;
  factionImageDestinationCursor =
       (byte *)((int)&g_GameFactionRuntimeImage.records[0].candidateCache.savedEntries[0].
                      weightedScoreAndKind + factionImageByteOffset);
  iVar1 = iVar1 * 2;
  if (iVar1 != 0) {
    for (; iVar1 != 0; iVar1 = iVar1 + -1) {
      *(undefined4 *)factionImageDestinationCursor = *(undefined4 *)candidateWorkspaceSourceCursor;
      candidateWorkspaceSourceCursor = candidateWorkspaceSourceCursor + 4;
      factionImageDestinationCursor = factionImageDestinationCursor + 4;
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
AiPreservedFactionIndexEdxResult
AiCandidateWorkspace_LoadFromFactionImage(FactionImageByteOffset factionImageByteOffset)

{
  int copyDwordsRemaining;
  AiPreservedFactionIndexEdxResult in_EDX;
  byte *factionImageSourceCursor;
  byte *candidateWorkspaceDestinationCursor;
  
  g_AiCandidateWorkspaceEntryCount =
       *(int *)((int)(g_GameFactionRuntimeImage.records[0].candidateCache.savedEntries + 3) +
               factionImageByteOffset);
  factionImageSourceCursor =
       (byte *)((int)&g_GameFactionRuntimeImage.records[0].candidateCache.savedEntries[0].
                      weightedScoreAndKind + factionImageByteOffset);
  copyDwordsRemaining = g_AiCandidateWorkspaceEntryCount * 2;
  candidateWorkspaceDestinationCursor = (byte *)g_AiWorkspaceBuffer13_Size0400;
  if (copyDwordsRemaining != 0) {
    for (; copyDwordsRemaining != 0; copyDwordsRemaining = copyDwordsRemaining + -1) {
      *(undefined4 *)candidateWorkspaceDestinationCursor = *(undefined4 *)factionImageSourceCursor;
      factionImageSourceCursor = factionImageSourceCursor + 4;
      candidateWorkspaceDestinationCursor = candidateWorkspaceDestinationCursor + 4;
    }
  }
  return (AiPreservedFactionIndexEdxResult)in_EDX.preservedEdxFactionIndex;
}

/* Address: 0x00537570.
   Ownership: gameplay/ai/workspaces.
   Purpose: Sorts the global two-dword candidate entries in descending order by the packed first dword while moving
   the paired second dword with each entry.
*/
void __cdecl AiCandidateWorkspace_SortDescending(void)

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
AiWorkspaceEntryValueEaxPreservedEdxCarrier64
AiCandidateWorkspace_GetEntryEntityValue(AiCandidateWorkspaceEntry *entry)

{
  ArmyAssetRecordPrefix *pAVar1;
  dword resolvedEntityValue;
  undefined4 in_EDX;
  uint uVar2;
  RuntimeToken registryId;
  bool bVar3;
  
  uVar2 = entry->weightedScoreAndKind & 0xf;
  registryId = entry->entityIdAndMultiplicity & 0xffff;
  bVar3 = uVar2 < 2;
  if (uVar2 == 2) {
    resolvedEntityValue = g_TechnologyAsset->records[registryId].xeniteCostQ4;
  }
  else {
    pAVar1 = ArmyAssetRegistry_FindByIdCf(registryId);
    resolvedEntityValue = 0x7fffffff;
    if (!bVar3) {
      resolvedEntityValue = pAVar1[2].registryId;
    }
  }
  return CONCAT44(in_EDX,resolvedEntityValue);
}

/* Address: 0x00538C90.
   Ownership: gameplay/ai/workspaces.
   Purpose: Recovered missed predicate symmetric to the primary workspace helper. It scans secondary eight-byte
   entries and sets CF for a matching ID with zero assignment. Typed parameters: p0 entryId→RuntimeToken. Nearby
   but non-identical semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes,
   control flow, globals, locals, and executable data remain unchanged.
*/
undefined4 AiSecondaryWorkspace_HasUnassignedEntryByIdCf(PckArmyAssetIdCatalog entryId)

{
  undefined4 in_EAX;
  int workspaceEntriesRemaining;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  
  workspaceEntriesRemaining = g_AiWorkspace01Count;
  workspaceEntryCursor = g_AiWorkspaceBuffer01_Size0200;
  while( true ) {
    if (workspaceEntriesRemaining == 0) {
      return in_EAX;
    }
    if ((entryId == workspaceEntryCursor->armyAssetId) &&
       (workspaceEntryCursor->armyRuntime == (ArmyRuntimeSlot *)0x0)) break;
    workspaceEntryCursor = workspaceEntryCursor + 1;
    workspaceEntriesRemaining = workspaceEntriesRemaining + -1;
  }
  return in_EAX;
}

/* Address: 0x00538CF0.
   Ownership: gameplay/ai/workspaces.
   Purpose: Scans the secondary eight-byte AI workspace entries and returns CF set on any matching ID at +0x04,
   clear on absence, while preserving EAX. Typed parameters: p0 entryId→RuntimeToken. Nearby but non-identical
   semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes, control flow,
   globals, locals, and executable data remain unchanged.
*/
undefined4 AiSecondaryWorkspace_HasEntryByIdCf(PckArmyAssetIdCatalog entryId)

{
  undefined4 in_EAX;
  int workspaceEntriesRemaining;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  
  workspaceEntriesRemaining = g_AiWorkspace01Count;
  workspaceEntryCursor = g_AiWorkspaceBuffer01_Size0200;
  while( true ) {
    if (workspaceEntriesRemaining == 0) {
      return in_EAX;
    }
    if (entryId == workspaceEntryCursor->armyAssetId) break;
    workspaceEntryCursor = workspaceEntryCursor + 1;
    workspaceEntriesRemaining = workspaceEntriesRemaining + -1;
  }
  return in_EAX;
}

/* Address: 0x00538D40.
   Ownership: gameplay/ai/workspaces.
   Purpose: Semantic ABI remains deferred.
*/
AiWorkspaceCountEaxPreservedEdxCarrier64
AiPrimaryWorkspace_CountAssignedEntriesByIdDuplicate(PckArmyAssetIdCatalog entryId)

{
  int matchingAssignedEntryCount;
  int workspaceEntriesRemaining;
  undefined4 in_EDX;
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
  return CONCAT44(in_EDX,matchingAssignedEntryCount);
}

/* Address: 0x00538D90.
   Ownership: gameplay/ai/workspaces.
   Purpose: Returns the signed minimum Manhattan distance in EAX for secondary-workspace entries; empty input
   returns INT_MAX. Secondary-workspace variant. Typed parameters: p0 worldX→Q12, p1 worldY→Q12. Nearby but non-
   identical semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes, control
   flow, globals, locals, and executable data remain unchanged.
*/
int AiSecondaryWorkspace_GetMinimumManhattanDistanceToPoint(Q12 worldX,Q12 worldY)

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
int AiPrimaryWorkspace_GetMinimumActiveManhattanDistanceToPoint(Q12 worldX,Q12 worldY)

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
int AiWorkspace02_GetMinimumManhattanDistanceToPoint(Q12 worldX,Q12 worldY)

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
int AiWorkspace03_GetMinimumManhattanDistanceToPoint(Q12 worldX,Q12 worldY)

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
int AiPrimaryWorkspace_GetMinimumManhattanDistanceToPoint(Q12 worldX,Q12 worldY)

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
void AiConstructionPlanner_PlaceSpecialAssetFromWorkspace
               (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
               WorldRuntimeContext *worldRuntime)

{
  ModelRuntimeNode *modelNodeRuntime;
  undefined4 *puVar1;
  ArmyRuntimeSlot *armySlot1;
  int extraout_ECX;
  int extraout_EDX;
  int extraout_EDX_00;
  int iVar2;
  AiTerrainFeatureWorkspaceEntry *terrainFeatureEntry;
  bool bVar3;
  undefined8 uVar4;
  
  iVar2 = g_AiWorkspace08Count;
  terrainFeatureEntry = g_AiWorkspaceBuffer08_Size0200;
  while( true ) {
    if (iVar2 == 0) {
      return;
    }
    bVar3 = armyAssetId < terrainFeatureEntry->armyAssetId;
    if ((armyAssetId == terrainFeatureEntry->armyAssetId) &&
       (AiPlacement_TestWorkspaceRecordAtPoint
                  (armyAssetId,terrainFeatureEntry->cell,factionIndex,(UiRootNode *)worldRuntime),
       !bVar3)) break;
    terrainFeatureEntry = terrainFeatureEntry + 1;
    iVar2 = iVar2 + -1;
  }
  armySlot1 = ArmyRuntime_CreateInstanceFromAssetCf
                        (4,(uint)*(ushort *)(extraout_EDX + 8),*(Q12 *)(extraout_EDX + 0x44),
                         *(Q12 *)(extraout_EDX + 0x40),factionIndex,armyAssetId,worldRuntime);
  if (bVar3) {
    return;
  }
  modelNodeRuntime = armySlot1->modelNodeRuntime;
  puVar1 = armySlot1->definitionOrAsset;
  modelNodeRuntime->tintArgb = 0;
  uVar4 = ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime,*puVar1,modelNodeRuntime);
  ArmyRuntime_DispatchClassCommand((ArmyRuntimeSlot **)uVar4,worldRuntime);
  EffectRuntimePool_CreateInstanceFromDefinitionCf
            (extraout_ECX,extraout_EDX_00,EFFECT_RUNTIME_COMPLETION_NONE,0,
             *(AngleTurn32 *)(extraout_ECX + 0x14),*(AngleTurn32 *)(extraout_ECX + 0x10),
             *(AngleTurn32 *)(extraout_ECX + 0xc),*(Q12 *)(extraout_ECX + 0x9c),
             *(Q12 *)(extraout_ECX + 0x98),*(Q12 *)(extraout_ECX + 0x94),
             *(EffectDefinition **)(extraout_EDX_00 + 400),worldRuntime);
  AiConstructionPlanner_ConsumeFactionPendingArmyAsset(armyAssetId,factionIndex);
  return;
}

/* Address: 0x0053BCB0.
   Ownership: gameplay/ai/workspaces.
   Purpose: Common stdcall stack ABI: FactionRuntimeIndex, TechnologyId, WorldRuntimeContext*. Signed score returns
   in EAX. Target group: stack-only callback target. Exact binary and live ownership are preflight locked.
*/
AiTechnologyCandidateScore
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
void AiRuntime_InitWorkspace(void)

{
  byte *workspaceAllocation;
  AiRuntimeWorkspaceEntry *runtimeWorkspaceEntry;
  AiScoredSiteWorkspaceEntry *pAVar1;
  byte *pbVar2;
  AiTargetWorkspaceEntry *pAVar3;
  AiTerrainFeatureWorkspaceEntry *pAVar4;
  FieldGridCell **gridCellCursor;
  ArmyAssetRecordPrefix **ppAVar5;
  AiTechnologyPlanningCandidate *technologyCandidateWorkspaceAllocation;
  AiCandidateWorkspaceEntry *pAVar6;
  GameEntityRuntime **ppGVar7;
  AiKnowledgeDataImage *knowledgeDataImage;
  undefined1 in_CF;
  
  workspaceAllocation = (*g_MemoryApi.alloc)(0x400);
  if (!(bool)in_CF) {
    g_AiWorkspaceBuffer00_Size0400 = workspaceAllocation;
    runtimeWorkspaceEntry = (*g_MemoryApi.alloc)(0x200);
    if (!(bool)in_CF) {
      g_AiWorkspaceBuffer01_Size0200 = runtimeWorkspaceEntry;
      runtimeWorkspaceEntry = (*g_MemoryApi.alloc)(0x400);
      if (!(bool)in_CF) {
        g_AiWorkspaceBuffer02_Size0400 = runtimeWorkspaceEntry;
        runtimeWorkspaceEntry = (*g_MemoryApi.alloc)(0x1000);
        if (!(bool)in_CF) {
          g_AiWorkspaceBuffer03_Size1000 = runtimeWorkspaceEntry;
          runtimeWorkspaceEntry = (*g_MemoryApi.alloc)(0x40);
          if (!(bool)in_CF) {
            g_AiWorkspaceBuffer04_Size0040 = runtimeWorkspaceEntry;
            pAVar1 = (*g_MemoryApi.alloc)(0x200);
            if (!(bool)in_CF) {
              g_AiWorkspaceBuffer05_Size0200 = pAVar1;
              pbVar2 = (*g_MemoryApi.alloc)(0x400);
              if (!(bool)in_CF) {
                g_AiWorkspaceBuffer06_Size0400 = pbVar2;
                pAVar3 = (*g_MemoryApi.alloc)(0x400);
                if (!(bool)in_CF) {
                  g_AiWorkspaceBuffer07_Size0400 = pAVar3;
                  pAVar4 = (*g_MemoryApi.alloc)(0x200);
                  if (!(bool)in_CF) {
                    g_AiWorkspaceBuffer08_Size0200 = pAVar4;
                    gridCellCursor = (*g_MemoryApi.alloc)(0x1000);
                    if (!(bool)in_CF) {
                      g_AiWorkspaceBuffer09_Size1000 = gridCellCursor;
                      gridCellCursor = (*g_MemoryApi.alloc)(0x400);
                      if (!(bool)in_CF) {
                        g_AiWorkspaceBuffer10_Size0400 = gridCellCursor;
                        ppAVar5 = (*g_MemoryApi.alloc)(0x1000);
                        if (!(bool)in_CF) {
                          g_AiWorkspaceBuffer11_Size1000 = ppAVar5;
                          technologyCandidateWorkspaceAllocation = (*g_MemoryApi.alloc)(0x200);
                          if (!(bool)in_CF) {
                            g_AiWorkspaceBuffer12_Size0200 = technologyCandidateWorkspaceAllocation;
                            pAVar6 = (*g_MemoryApi.alloc)(0x400);
                            if (!(bool)in_CF) {
                              g_AiWorkspaceBuffer13_Size0400 = pAVar6;
                              ppGVar7 = (*g_MemoryApi.alloc)(0x100);
                              if (!(bool)in_CF) {
                                g_AiWorkspaceBuffer14_Size0100 = ppGVar7;
                                knowledgeDataImage =
                                     Package_LoadEntry((word *)u_engine_ki_dat_0053c5e4);
                                if (!(bool)in_CF) {
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
  return;
}

/* Address: 0x00537F80.
   Ownership: gameplay/ai/workspaces.
   Purpose: Adds the current entity pointer to the 0x400-entry workspace09 list when capacity remains and the
   entity position lies outside every primary-workspace entry extent.
   Local calls: AiPrimaryWorkspace_IsPointOutsideAllEntryExtentsCf.
*/
AiPlanningGridScratchEdxContinuityResult AiEntityCandidateWorkspace09_AddOutsidePrimaryExtents(void)

{
  FieldGridCell **ppFVar1;
  int extraout_ECX;
  dword in_EDX;
  FieldGridCell *unaff_ESI;
  bool bVar2;
  
  ppFVar1 = g_AiWorkspaceBuffer09_Size1000;
  bVar2 = g_AiWorkspace09Count < 0x400;
  if (bVar2) {
    AiPrimaryWorkspace_IsPointOutsideAllEntryExtentsCf(unaff_ESI->worldY,unaff_ESI->worldX);
    if (!bVar2) {
      ppFVar1[extraout_ECX] = unaff_ESI;
      g_AiWorkspace09Count = g_AiWorkspace09Count + 1;
    }
  }
  return (AiPlanningGridScratchEdxContinuityResult)in_EDX;
}

/* Address: 0x00537FC0.
   Ownership: gameplay/ai/workspaces.
   Purpose: Adds the current entity pointer to the 0x100-entry workspace10 list under the same outside-primary-
   extents test.
   Local calls: AiPrimaryWorkspace_IsPointOutsideAllEntryExtentsCf.
*/
AiPlanningGridScratchEdxContinuityResult AiEntityCandidateWorkspace10_AddOutsidePrimaryExtents(void)

{
  FieldGridCell **ppFVar1;
  int extraout_ECX;
  dword in_EDX;
  FieldGridCell *unaff_ESI;
  bool bVar2;
  
  ppFVar1 = g_AiWorkspaceBuffer10_Size0400;
  bVar2 = g_AiWorkspace10Count < 0x100;
  if (bVar2) {
    AiPrimaryWorkspace_IsPointOutsideAllEntryExtentsCf(unaff_ESI->worldY,unaff_ESI->worldX);
    if (!bVar2) {
      ppFVar1[extraout_ECX] = unaff_ESI;
      g_AiWorkspace10Count = g_AiWorkspace10Count + 1;
    }
  }
  return (AiPlanningGridScratchEdxContinuityResult)in_EDX;
}

/* Address: 0x00538B90.
   Ownership: gameplay/ai/workspaces.
   Purpose: Scans the primary eight-byte AI workspace entries. CF is set when an entry has the requested ID at
   +0x04 and zero assignment at +0x00; CF is clear otherwise. EAX is preserved. Typed parameters: p0
   entryId→RuntimeToken. Nearby but non-identical semantic domains were explicitly deferred.
*/
undefined4 AiPrimaryWorkspace_HasUnassignedEntryByIdCf(PckArmyAssetIdCatalog entryId)

{
  undefined4 in_EAX;
  int workspaceEntriesRemaining;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  
  workspaceEntriesRemaining = g_AiWorkspace00Count;
  workspaceEntryCursor = (AiRuntimeWorkspaceEntry *)g_AiWorkspaceBuffer00_Size0400;
  while( true ) {
    if (workspaceEntriesRemaining == 0) {
      return in_EAX;
    }
    if ((entryId == workspaceEntryCursor->armyAssetId) &&
       (workspaceEntryCursor->armyRuntime == (ArmyRuntimeSlot *)0x0)) break;
    workspaceEntryCursor = workspaceEntryCursor + 1;
    workspaceEntriesRemaining = workspaceEntriesRemaining + -1;
  }
  return in_EAX;
}

/* Address: 0x00538BF0.
   Ownership: gameplay/ai/workspaces.
   Purpose: Scans the primary eight-byte AI workspace entries and returns CF set on any matching ID at +0x04, clear
   on absence, while preserving EAX. Typed parameters: p0 entryId→RuntimeToken. Nearby but non-identical semantic
   domains were explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals,
   locals, and executable data remain unchanged.
*/
undefined4 AiPrimaryWorkspace_HasEntryByIdCf(PckArmyAssetIdCatalog entryId)

{
  undefined4 in_EAX;
  int workspaceEntriesRemaining;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  
  workspaceEntriesRemaining = g_AiWorkspace00Count;
  workspaceEntryCursor = (AiRuntimeWorkspaceEntry *)g_AiWorkspaceBuffer00_Size0400;
  while( true ) {
    if (workspaceEntriesRemaining == 0) {
      return in_EAX;
    }
    if (entryId == workspaceEntryCursor->armyAssetId) break;
    workspaceEntryCursor = workspaceEntryCursor + 1;
    workspaceEntriesRemaining = workspaceEntriesRemaining + -1;
  }
  return in_EAX;
}

/* Address: 0x00538C40.
   Ownership: gameplay/ai/workspaces.
   Purpose: Counts primary workspace entries whose assignment dword at +0x00 is nonzero and whose ID at +0x04
   matches the requested value. The count is returned in EAX. Typed parameters: p0 entryId→RuntimeToken. Nearby but
   non-identical semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes,
   control flow, globals, locals, and executable data remain unchanged.
*/
AiWorkspaceCountEaxPreservedEdxCarrier64
AiPrimaryWorkspace_CountAssignedEntriesById(PckArmyAssetIdCatalog entryId)

{
  int matchingAssignedEntryCount;
  int workspaceEntriesRemaining;
  undefined4 in_EDX;
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
  return CONCAT44(in_EDX,matchingAssignedEntryCount);
}

/* Address: 0x005374B0.
   Ownership: gameplay/ai/workspaces.
   Purpose: Matching kind and ID entries accumulate count and weight; new entries are capped at 128. Typed
   parameters: p0 entityId→RuntimeToken. Nearby but non-identical semantic domains were explicitly deferred.
   Calling convention, parameter storage, body bytes, control flow, globals, locals, and executable data remain
   unchanged. Typed parameters: p2 entryKind→AiCandidateEntryKind_V344.
*/
void AiCandidateWorkspace_AddOrAccumulateWeightedEntry
               (RuntimeToken entityId,dword weightRange,AiCandidateEntryKind entryKind)

{
  uint uVar1;
  dword dVar2;
  int iVar3;
  uint extraout_ECX;
  uint uVar4;
  AiCandidateWorkspaceEntry *candidateEntry;
  
  dVar2 = (*g_RandomGeneratorState.next)();
  uVar1 = g_AiCandidateWorkspaceEntryCount;
  candidateEntry = g_AiWorkspaceBuffer13_Size0400;
  if (1 < extraout_ECX) {
    iVar3 = extraout_ECX * 5 + dVar2 % extraout_ECX;
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
void AiPrimaryWorkspace_IsPointOutsideAllEntryExtentsCf(Q12 worldX,Q12 worldY)

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
      return;
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
        return;
      }
    }
    workspaceEntryCursor = workspaceEntryCursor + 1;
    workspaceEntriesRemaining = workspaceEntriesRemaining + -1;
  } while( true );
}
