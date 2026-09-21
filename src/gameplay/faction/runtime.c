/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/faction/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/faction/runtime.h>

/* Implementation ownership: gameplay/faction/runtime. */

/* Address: 0x0055F790.
   Ownership: gameplay/faction/runtime.
   Purpose: Advances the selected faction pair through the verified relation-state transition table, using recent-
   timed-state checks for transitional states. It is distinct from FrontendPlayerIndex_V306, PlayerRuntimeId,
   active-faction masks or codes, and PCK-backed ArmyAssetId, ModelDefinitionId, and TechnologyId domains.
   Local calls: GameFactionRuntime_IsRecentTimedRelationStateCf,
   GameFactionRuntime_ApplyPairwiseRelationTransition.
*/
void __thandor_void_preserve_eax_ecx
GameFactionRuntime_AdvancePairwiseRelationState
          (dword unusedRelationArgument0,dword unusedRelationArgument1,
          FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex)

{
  bool bVar1;
  
  switch(g_GameFactionRuntimeImage.records[targetFactionIndex].packedRelationStates >>
         ((byte)(sourceFactionIndex << 2) & 0x1f) & 0xf) {
  case 2:
    bVar1 = GameFactionRuntime_IsRecentTimedRelationStateCf(sourceFactionIndex,targetFactionIndex);
    if (bVar1) {
      return;
    }
  case 0:
  case 1:
    GameFactionRuntime_ApplyPairwiseRelationTransition
              (1,0,3,2,sourceFactionIndex,targetFactionIndex);
    break;
  case 3:
    GameFactionRuntime_ApplyPairwiseRelationTransition
              (2,2,4,4,sourceFactionIndex,targetFactionIndex);
    break;
  case 4:
    bVar1 = GameFactionRuntime_IsRecentTimedRelationStateCf(sourceFactionIndex,targetFactionIndex);
    if (bVar1) {
      return;
    }
  case 5:
    GameFactionRuntime_ApplyPairwiseRelationTransition
              (4,3,6,5,sourceFactionIndex,targetFactionIndex);
    break;
  case 6:
    GameFactionRuntime_ApplyPairwiseRelationTransition
              (5,5,8,8,sourceFactionIndex,targetFactionIndex);
    break;
  case 8:
    bVar1 = GameFactionRuntime_IsRecentTimedRelationStateCf(sourceFactionIndex,targetFactionIndex);
    if (bVar1) {
      return;
    }
  case 9:
    GameFactionRuntime_ApplyPairwiseRelationTransition
              (7,6,10,9,sourceFactionIndex,targetFactionIndex);
    break;
  case 10:
    GameFactionRuntime_ApplyPairwiseRelationTransition
              (8,8,0xb,0xb,sourceFactionIndex,targetFactionIndex);
  }
  return;
}


/* Address: 0x0055F910.
   Ownership: gameplay/faction/runtime.
   Purpose: Resets or normalizes the selected faction pair through the verified relation-state transition table. It
   is distinct from FrontendPlayerIndex_V306, PlayerRuntimeId, active-faction masks or codes, and PCK-backed
   ArmyAssetId, ModelDefinitionId, and TechnologyId domains.
   Local calls: GameFactionRuntime_ApplyPairwiseRelationTransition.
*/
void __thandor_void_preserve_eax_ecx_edx
GameFactionRuntime_ResetPairwiseRelationState
          (dword unusedRelationArgument0,dword unusedRelationArgument1,
          FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex)

{
  switch(g_GameFactionRuntimeImage.records[targetFactionIndex].packedRelationStates >>
         ((byte)(sourceFactionIndex << 2) & 0x1f) & 0xf) {
  case 1:
  case 2:
  case 3:
    GameFactionRuntime_ApplyPairwiseRelationTransition
              (0x13,0x13,0,0,sourceFactionIndex,targetFactionIndex);
    break;
  case 4:
  case 6:
    GameFactionRuntime_ApplyPairwiseRelationTransition
              (9,9,0,0,sourceFactionIndex,targetFactionIndex);
    break;
  case 5:
    GameFactionRuntime_ApplyPairwiseRelationTransition
              (0x13,0x13,4,4,sourceFactionIndex,targetFactionIndex);
    break;
  case 8:
  case 10:
    GameFactionRuntime_ApplyPairwiseRelationTransition
              (10,10,4,4,sourceFactionIndex,targetFactionIndex);
    break;
  case 9:
    GameFactionRuntime_ApplyPairwiseRelationTransition
              (0x13,0x13,8,8,sourceFactionIndex,targetFactionIndex);
  }
  return;
}


/* Address: 0x00565320.
   Ownership: gameplay/faction/runtime.
   Purpose: Rebuilds the fixed old-unit secondary masks and spatial replay records from the selected campaign
   scenario and active world entities, or clears the tables when no matching scenario data is available.
   Local calls: OldUnitRuntime_ResetPendingTables.
   Cross-module calls: FixedMath_Length2 [core/math/fixed].
*/

void __fastcall OldUnitRuntime_RebuildScenarioReplayTables(void)

{
  WorldOwnerListNode100 *pWVar1;
  dword dVar2;
  uint uVar3;
  dword dVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int unaff_ESI;
  dword *pdVar10;
  dword *pdVar11;
  
  if ((g_InGameRuntimeRoot != (InGameRuntimeRootImageC3E4 *)0x0) &&
     (pdVar10 = (dword *)((int)g_GameFactionRuntimeImage.records[0].technologyMasks256Bits +
                         unaff_ESI *
                         (g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex),
     g_FrontendLoadedCampaignAsset != 0)) {
    iVar5 = *(int *)(g_FrontendLoadedCampaignAsset + 0xb8);
    iVar8 = g_FrontendLoadedCampaignAsset;
    do {
      if (*(int *)(g_FrontendLoadedCampaignAsset + 0xc4) == *(int *)(iVar8 + 0x300)) {
        iVar9 = iVar8 + 0x200;
        uVar3 = *(uint *)(iVar8 + 0x304) >> ((byte)g_EndMovieSelectionIndex & 0x1f);
        uVar6 = *(uint *)(iVar8 + 0x308) >> ((byte)g_EndMovieSelectionIndex & 0x1f);
        iVar5 = 8;
        pdVar11 = g_OldUnitSecondaryTable;
        goto 
        OldUnitRuntime_RebuildScenarioReplayTables_ProcessNextReplayGroupMaskAndCopyOrClearBlock;
      }
      iVar8 = iVar8 + 0x180;
      iVar5 = iVar5 + -1;
    } while (iVar5 != 0);
  }
  OldUnitRuntime_ResetPendingTables();
  return;
OldUnitRuntime_RebuildScenarioReplayTables_ProcessNextReplayGroupMaskAndCopyOrClearBlock:
  if ((uVar6 & 1) == 0) {
    iVar7 = 8;
    if (((uVar3 & 1) == 0) ||
       ((((*(int *)(iVar9 + 0x60) == 0 && (*(int *)(iVar9 + 0x80) == 0)) &&
         (*(int *)(iVar9 + 0xc0) == 0)) &&
        ((*(int *)(iVar9 + 0xe0) == 0 && (*(int *)(iVar9 + 0xa0) == 0)))))) {
      for (iVar7 = 8; iVar7 != 0; iVar7 = iVar7 + -1) {
        *pdVar11 = 0;
        pdVar11 = pdVar11 + 1;
      }
      goto OldUnitRuntime_RebuildScenarioReplayTables_AdvanceSourceBlockAfterSkippedOrClearedGroup;
    }
    do {
      *pdVar11 = *pdVar10;
      pdVar10 = pdVar10 + 1;
      pdVar11 = pdVar11 + 1;
      iVar7 = iVar7 + -1;
    } while (iVar7 != 0);
  }
  else {
OldUnitRuntime_RebuildScenarioReplayTables_AdvanceSourceBlockAfterSkippedOrClearedGroup:
    pdVar10 = pdVar10 + 8;
  }
  pdVar10 = pdVar10 + 0x1c8;
  iVar9 = iVar9 + 4;
  iVar5 = iVar5 + -1;
  if (iVar5 == 0) {
    if (((g_InGameRuntimeRoot != (InGameRuntimeRootImageC3E4 *)0x0) &&
        (pWVar1 = (g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead, (uVar6 & 1) == 0)) &&
       (g_OldUnitRecordCount = 0, pdVar10 = g_OldUnitPrimaryTable, (uVar3 & 1) != 0)) {
      for (; pWVar1 != (WorldOwnerListNode100 *)0x0; pWVar1 = pWVar1->nextNode) {
        if (((pWVar1->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
            (dVar2 = *(dword *)(*(int *)((int)pWVar1->runtimePayload + 8) + 0xc),
            0 < *(int *)(iVar8 + 0x2a0 + dVar2 * 4))) &&
           (dVar4 = FixedMath_Length2(*(int *)(iVar8 + 0x280 + dVar2 * 4) - pWVar1->worldYQ12,
                                      *(int *)(iVar8 + 0x260 + dVar2 * 4) - pWVar1->worldXQ12),
           (int)dVar4 <= *(int *)(iVar8 + 0x2a0 + dVar2 * 4))) {
          iVar5 = pWVar1->worldYQ12;
          iVar9 = *(int *)(iVar8 + 0x2e0 + dVar2 * 4);
          iVar7 = *(int *)(iVar8 + 0x280 + dVar2 * 4);
          pdVar10[2] = (pWVar1->worldXQ12 + *(int *)(iVar8 + 0x2c0 + dVar2 * 4)) -
                       *(int *)(iVar8 + 0x260 + dVar2 * 4);
          pdVar10[3] = (iVar5 + iVar9) - iVar7;
          pdVar10[1] = dVar2;
          if (*(int *)((int)pWVar1->runtimePayload + 0xf0) == 0) {
            dVar2 = *(dword *)(*(int *)((int)pWVar1->runtimePayload + 8) + 0xa0);
            pdVar10[4] = pWVar1->modelLocalRotationAngle2;
            *pdVar10 = dVar2;
            g_OldUnitRecordCount = g_OldUnitRecordCount + 1;
            pdVar10 = pdVar10 + 8;
            if (0x1ff < g_OldUnitRecordCount) {
              return;
            }
          }
        }
      }
    }
    return;
  }
  goto OldUnitRuntime_RebuildScenarioReplayTables_ProcessNextReplayGroupMaskAndCopyOrClearBlock;
}


/* Address: 0x005130B0.
   Ownership: gameplay/faction/runtime.
   Purpose: Resolves the two serialized army-asset identifier arrays in each faction record through the army
   registry, clears an array count on lookup failure, and rebases the 256 stored runtime pointers with the verified
   army-runtime rebase delta.
   Cross-module calls: ArmyAssetRegistry_FindByIdCf [assets/army/catalog].
*/
void __fastcall GameFactionRuntime_RebaseLoadedArmyReferences(void)

{
  ArmyRuntimeSlot *armySlot1;
  int iVar1;
  FactionArmyAssetCount FVar2;
  int iVar3;
  GameFactionRuntimeImage *pGVar4;
  ArmyAssetRecordPrefix **pdVar6;
  dword *pdVar7;
  ArmyAssetRecordPrefix **pdVar5;
  ArmyRuntimeSlot **ppAVar8;
  ArmyRegistryEaxCf5_51b6d0 AVar9;
  
  pGVar4 = &g_GameFactionRuntimeImage;
  iVar1 = 8;
  do {
    pdVar7 = pGVar4->records[0].secondaryArmyAssetPointersOrIds;
    for (FVar2 = pGVar4->records[0].secondaryArmyAssetCount; FVar2 != 0; FVar2 = FVar2 - 1) {
      AVar9 = ArmyAssetRegistry_FindByIdCf(*pdVar7);
      if (AVar9.carry) {
        pGVar4->records[0].secondaryArmyAssetCount = 0;
        break;
      }
      *pdVar7 = (dword)AVar9.eax;
      pdVar7 = pdVar7 + 1;
    }
    pdVar7 = pGVar4->records[0].primaryArmyAssetPointersOrIds;
    for (FVar2 = pGVar4->records[0].primaryArmyAssetCount; FVar2 != 0; FVar2 = FVar2 - 1) {
      AVar9 = ArmyAssetRegistry_FindByIdCf(*pdVar7);
      if (AVar9.carry) {
        pGVar4->records[0].primaryArmyAssetCount = 0;
        break;
      }
      *pdVar7 = (dword)AVar9.eax;
      pdVar7 = pdVar7 + 1;
    }
    ppAVar8 = pGVar4->records[0].runtimeGroupMembers8x32;
    iVar3 = 0x100;
    do {
      armySlot1 = (ArmyRuntimeSlot *)0x0;
      if (*ppAVar8 != (ArmyRuntimeSlot *)0x0) {
        armySlot1 = (ArmyRuntimeSlot *)
                    ((int)&(*ppAVar8)->modelRuntimeOrSavedOffset +
                    (int)g_ArmyRuntimeRebaseBaseMinusOne);
      }
      *ppAVar8 = armySlot1;
      ppAVar8 = ppAVar8 + 1;
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
    pGVar4 = (GameFactionRuntimeImage *)(pGVar4->records + 1);
    iVar1 = iVar1 + -1;
    if (iVar1 == 0) {
      return;
    }
  } while( true );
}


/* Address: 0x00513960.
   Ownership: gameplay/faction/runtime.
   Purpose: Scans all eight faction records and clears every matching dword in each 256-entry technology-associated
   table. Clears one tech bit across all eight faction unlock words for every faction.
*/
void __thandor_void_preserve_eax_ecx_edx
GameFactionRuntime_ClearRuntimeGroupMemberPointerFromAllFactionTables(void *runtimeGroupMember)

{
  int factionsRemaining;
  int groupSlotsRemaining;
  int groupSlotIndex;
  GameFactionRuntimeImage *factionRecordCursor;
  
  factionRecordCursor = &g_GameFactionRuntimeImage;
  factionsRemaining = 8;
  groupSlotsRemaining = 0x100;
  groupSlotIndex = 0;
  do {
    do {
      if (runtimeGroupMember ==
          factionRecordCursor->records[0].runtimeGroupMembers8x32[groupSlotIndex]) {
        factionRecordCursor->records[0].runtimeGroupMembers8x32[groupSlotIndex] =
             (ArmyRuntimeSlot *)0x0;
      }
      groupSlotIndex = groupSlotIndex + 1;
      groupSlotsRemaining = groupSlotsRemaining + -1;
    } while (groupSlotsRemaining != 0);
    factionRecordCursor = (GameFactionRuntimeImage *)(factionRecordCursor->records + 1);
    groupSlotsRemaining = 0x100;
    groupSlotIndex = 0;
    factionsRemaining = factionsRemaining + -1;
  } while (factionsRemaining != 0);
  return;
}


/* Address: 0x00513CA0.
   Ownership: gameplay/faction/runtime.
   Purpose: Tests one bit in the selected faction runtime capability mask at offset 0x3C. Carry is set when the
   capability bit is clear and clear when the bit is present.
*/
bool __thandor_cf_preserve_eax_ecx_edx
GameFactionRuntime_TestCapabilityBitClearCf
          (dword capabilityBitIndex,FactionRuntimeIndex factionIndex)

{
  return (g_GameFactionRuntimeImage.records[factionIndex].capabilityFlags &
         1 << ((byte)capabilityBitIndex & 0x1f)) == 0;
}


/* Address: 0x00513CD0.
   Ownership: gameplay/faction/runtime.
   Purpose: Returns one four-bit value from the packed dword at GameFactionRuntimeRecord+0x40. stateIndex selects
   one of eight nibbles and factionIndex selects the 0x740-byte record.
*/
FactionRelationState __thandor_eax_preserve_ecx_edx
GameFactionRuntime_GetPackedStateNibble
          (FactionRuntimeIndex otherFactionIndex,FactionRuntimeIndex factionIndex)

{
  return g_GameFactionRuntimeImage.records[factionIndex].packedRelationStates >>
         ((byte)(otherFactionIndex << 2) & 0x1f) & 0xf;
}


/* Address: 0x00513D70.
   Ownership: gameplay/faction/runtime.
   Purpose: Traverses unordered faction pairs 1 through 7. For pairwise relation states 8, 9, or 10, finds the
   first asymmetric technology bit and unlocks the corresponding missing technologies in both factions, then
   rebuilds the other-player UI slots.
   Cross-module calls: Technology_UnlockForFaction [gameplay/technology/runtime],
   InGameOtherPlayerCommand_RebuildTargetEntries [ui/ingame/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
GameFactionRuntime_SynchronizeTechnologiesForRelationStates8To10(void)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  TechnologyId technologyIndex;
  uint factionIndex;
  uint factionIndex_00;
  TechnologyId technologyIndex_00;
  int iVar5;
  
  factionIndex_00 = 1;
  iVar5 = 0x50fa80;
GameFactionRuntime_SynchronizeTechnologiesForRelationStates8To10_BeginNextSourceFactionScan:
  factionIndex = factionIndex_00 + 1;
  iVar1 = iVar5;
GameFactionRuntime_SynchronizeTechnologiesForRelationStates8To10_EvaluateNextFactionPair:
  switch(*(uint *)(iVar1 + 0x780) >> ((char)factionIndex_00 * '\x04' & 0x1fU) & 0xf) {
  case 8:
  case 9:
  case 10:
    uVar3 = 1;
    uVar4 = 0;
    technologyIndex_00 = 0;
    do {
      if (((*(uint *)(iVar5 + 0x6e0 + uVar4 * 4) & uVar3) != 0) &&
         ((*(uint *)(iVar1 + 0xe20 + uVar4 * 4) & uVar3) == 0)) {
        uVar4 = 0;
        uVar3 = 1;
        technologyIndex = 0;
        goto 
        GameFactionRuntime_SynchronizeTechnologiesForRelationStates8To10_ScanReciprocalTechnologyBitsAndUnlockMatch
        ;
      }
      iVar2 = (int)uVar3 >> 0x1f;
      uVar3 = uVar3 << 1 | -iVar2;
      uVar4 = uVar4 + (-iVar2 != 0);
      technologyIndex_00 = technologyIndex_00 + 1;
    } while (uVar4 < 8);
  }
  goto switchD_00513d9f_caseD_0;
  while( true ) {
    iVar2 = (int)uVar3 >> 0x1f;
    uVar3 = uVar3 << 1 | -iVar2;
    uVar4 = uVar4 + (-iVar2 != 0);
    technologyIndex = technologyIndex + 1;
    if (7 < uVar4) break;

    GameFactionRuntime_SynchronizeTechnologiesForRelationStates8To10_ScanReciprocalTechnologyBitsAndUnlockMatch
    :
    if (((*(uint *)(iVar1 + 0xe20 + uVar4 * 4) & uVar3) != 0) &&
       ((*(uint *)(iVar5 + 0x6e0 + uVar4 * 4) & uVar3) == 0)) {
      Technology_UnlockForFaction(0,0,technologyIndex,factionIndex_00);
      Technology_UnlockForFaction(0,0,technologyIndex_00,factionIndex);
      break;
    }
  }
switchD_00513d9f_caseD_0:
  factionIndex = factionIndex + 1;
  iVar1 = iVar1 + 0x740;
  if (7 < factionIndex) goto code_r0x00513eb0;
  goto GameFactionRuntime_SynchronizeTechnologiesForRelationStates8To10_EvaluateNextFactionPair;
code_r0x00513eb0:
  factionIndex_00 = factionIndex_00 + 1;
  iVar5 = iVar5 + 0x740;
  if (6 < factionIndex_00) {
    InGameOtherPlayerCommand_RebuildTargetEntries((UiNodeBase *)g_InGameRuntimeRoot);
    return;
  }
  goto GameFactionRuntime_SynchronizeTechnologiesForRelationStates8To10_BeginNextSourceFactionScan;
}


/* Address: 0x00514510.
   Ownership: gameplay/faction/runtime.
   Purpose: Updates the target faction's primary or secondary impact-alert anchor from the linked model position,
   refreshes the exact 0x96 cooldown, and emits notification 0x12C or 0x12D when the world runtime identity matches
   the impacted entity.
   Cross-module calls: InGameNotificationQueue_InsertPriorityRecord [ui/ingame/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
GameFactionRuntime_UpdateImpactAlertAnchorAndNotify
          (ArmyRuntimeSlot *targetArmyRuntime,WorldRuntimeContext *worldRuntime)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  FactionAnchorCooldownTicks FVar4;
  GraphicsWorldCoordinateQ12 GVar5;
  int iVar6;
  int iVar7;
  
  piVar1 = (targetArmyRuntime->linkedEntityRuntime->common).ownership.definitionOrClassRecord;
  iVar2 = (targetArmyRuntime->linkedEntityRuntime->common).ownership.ownerIndex;
  iVar3 = piVar1[1];
  if (*(int *)(*piVar1 + 0x18) == 0) {
    FVar4 = g_GameFactionRuntimeImage.records[iVar2].primaryAnchorCooldown;
    iVar6 = g_GameFactionRuntimeImage.records[iVar2].primaryAnchorYQ12 - *(int *)(iVar3 + 0x94);
    if (iVar6 < 0) {
      iVar6 = -iVar6;
    }
    iVar7 = g_GameFactionRuntimeImage.records[iVar2].primaryAnchorXQ12 - *(int *)(iVar3 + 0x98);
    if (iVar7 < 0) {
      iVar7 = -iVar7;
    }
    g_GameFactionRuntimeImage.records[iVar2].primaryAnchorCooldown = 0x96;
    if (((int)FVar4 < 0x32) && ((0xc000 < iVar6 + iVar7 || (FVar4 == 0)))) {
      GVar5 = *(GraphicsWorldCoordinateQ12 *)(iVar3 + 0x98);
      iVar6 = piVar1[2];
      g_GameFactionRuntimeImage.records[iVar2].primaryAnchorYQ12 =
           *(GraphicsWorldCoordinateQ12 *)(iVar3 + 0x94);
      g_GameFactionRuntimeImage.records[iVar2].primaryAnchorXQ12 = GVar5;
      if (worldRuntime->activeFactionRuntimeIndex == *(int *)(iVar6 + 0xc)) {
        InGameNotificationQueue_InsertPriorityRecord
                  (FACTION_IMPACT_ANCHOR,0,(worldRuntime->motion).pitchAngle,
                   (worldRuntime->motion).headingAngle,*(Q12 *)(piVar1[1] + 0x98),
                   *(Q12 *)(piVar1[1] + 0x94),8,300);
      }
    }
    else {
      GVar5 = *(GraphicsWorldCoordinateQ12 *)(iVar3 + 0x98);
      g_GameFactionRuntimeImage.records[iVar2].primaryAnchorYQ12 =
           *(GraphicsWorldCoordinateQ12 *)(iVar3 + 0x94);
      g_GameFactionRuntimeImage.records[iVar2].primaryAnchorXQ12 = GVar5;
    }
  }
  else {
    FVar4 = g_GameFactionRuntimeImage.records[iVar2].anchorCooldown0;
    iVar6 = g_GameFactionRuntimeImage.records[iVar2].secondaryAnchorYQ12 - *(int *)(iVar3 + 0x94);
    if (iVar6 < 0) {
      iVar6 = -iVar6;
    }
    iVar7 = g_GameFactionRuntimeImage.records[iVar2].secondaryAnchorXQ12 - *(int *)(iVar3 + 0x98);
    if (iVar7 < 0) {
      iVar7 = -iVar7;
    }
    g_GameFactionRuntimeImage.records[iVar2].anchorCooldown0 = 0x96;
    if (((int)FVar4 < 0x32) && ((0xc000 < iVar6 + iVar7 || (FVar4 == 0)))) {
      GVar5 = *(GraphicsWorldCoordinateQ12 *)(iVar3 + 0x98);
      iVar6 = piVar1[2];
      g_GameFactionRuntimeImage.records[iVar2].secondaryAnchorYQ12 =
           *(GraphicsWorldCoordinateQ12 *)(iVar3 + 0x94);
      g_GameFactionRuntimeImage.records[iVar2].secondaryAnchorXQ12 = GVar5;
      if (worldRuntime->activeFactionRuntimeIndex == *(int *)(iVar6 + 0xc)) {
        InGameNotificationQueue_InsertPriorityRecord
                  (FACTION_IMPACT_ANCHOR,0,(worldRuntime->motion).pitchAngle,
                   (worldRuntime->motion).headingAngle,*(Q12 *)(piVar1[1] + 0x98),
                   *(Q12 *)(piVar1[1] + 0x94),7,0x12d);
      }
    }
    else {
      GVar5 = *(GraphicsWorldCoordinateQ12 *)(iVar3 + 0x98);
      g_GameFactionRuntimeImage.records[iVar2].secondaryAnchorYQ12 =
           *(GraphicsWorldCoordinateQ12 *)(iVar3 + 0x94);
      g_GameFactionRuntimeImage.records[iVar2].secondaryAnchorXQ12 = GVar5;
    }
  }
  return;
}


/* Address: 0x00514730.
   Ownership: gameplay/faction/runtime.
   Purpose: Recomputes explored-terrain percentage, unlocked-technology count, resource and progress components,
   active army contribution, and the derived score fields for one faction. It is distinct from
   FrontendPlayerIndex_V306, PlayerRuntimeId, active-faction masks or codes, and PCK-backed ArmyAssetId,
   ModelDefinitionId, and TechnologyId domains.
   Cross-module calls: ArmyAssetRegistry_FindByIdCf [assets/army/catalog].
*/
void __thandor_void_preserve_eax_ecx_edx
GameFactionRuntime_RecomputeProgressAndScoreMetrics
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  TerrainExploredPercent TVar1;
  ResourceExtractionRateQ4PerTick RVar2;
  int iVar3;
  uint uVar4;
  FactionTechnologyCount FVar5;
  uint uVar6;
  int iVar7;
  byte *pbVar8;
  uint uVar9;
  ArmyRegistryEaxCf5_51b6d0 AVar10;
  FieldGridAsset *fieldGrid1;
  WorldOwnerListNode100 *worldNode1;
  
  fieldGrid1 = worldRuntime->fieldGrid;
  uVar4 = fieldGrid1->gridWidth * fieldGrid1->gridHeight;
  iVar3 = 0;
  pbVar8 = fieldGrid1->cells[0].runtime60_6B + factionIndex + 0x10;
  uVar6 = uVar4;
  do {
    if ((*pbVar8 & 0xf8) != 0) {
      iVar3 = iVar3 + 1;
    }
    pbVar8 = pbVar8 + 0x80;
    uVar6 = uVar6 - 1;
  } while (uVar6 != 0);
  g_GameFactionRuntimeImage.records[factionIndex].exploredTerrainPercent =
       (uint)(iVar3 * 100) / uVar4;
  uVar6 = 1;
  uVar4 = 0;
  uVar9 = 0;
  do {
    do {
      if ((*(uint *)(factionIndex * 0x740 + 0x50fa20 + uVar9 * 4) & uVar6) != 0) {
        uVar4 = uVar4 + 1;
      }
      uVar6 = uVar6 * 2;
    } while (uVar6 != 0);
    uVar9 = uVar9 + 1;
    uVar6 = 1;
  } while (uVar9 < 8);
  FVar5 = uVar4 - 5;
  if (uVar4 < 5) {
    FVar5 = 0;
  }
  TVar1 = g_GameFactionRuntimeImage.records[factionIndex].exploredTerrainPercent;
  iVar3 = (int)g_GameFactionRuntimeImage.records[factionIndex].xeniteExtractedTotalQ4 >> 4;
  iVar7 = (int)g_GameFactionRuntimeImage.records[factionIndex].tritiumExtractedTotalQ4 >> 4;
  g_GameFactionRuntimeImage.records[factionIndex].unlockedTechnologyCountBeyondBaseline = FVar5;
  g_GameFactionRuntimeImage.records[factionIndex].primaryResourceComponent = iVar3;
  g_GameFactionRuntimeImage.records[factionIndex].secondaryResourceComponent = iVar7;
  g_GameFactionRuntimeImage.records[factionIndex].economyProgressScore =
       (int)(iVar3 * 0x10 + iVar7 * 8 + FVar5 * 0xa000 + TVar1 * 0x1000) >> 0xc;
  g_GameFactionRuntimeImage.records[factionIndex].relationScore =
       g_GameFactionRuntimeImage.records[factionIndex].relationCounterB * 0x28000 +
       g_GameFactionRuntimeImage.records[factionIndex].relationCounterD * -0x14000 +
       g_GameFactionRuntimeImage.records[factionIndex].relationCounterA * 0x14000 +
       g_GameFactionRuntimeImage.records[factionIndex].relationCounterC * -0xa000 +
       g_GameFactionRuntimeImage.records[factionIndex].relationCounterE * 0x14000 +
       g_GameFactionRuntimeImage.records[factionIndex].relationCounterF * 0x28000 >> 0xc;
  iVar3 = 0;
  for (worldNode1 = worldRuntime->ownerListHead; worldNode1 != (WorldOwnerListNode100 *)0x0;
      worldNode1 = worldNode1->nextNode) {
    if ((worldNode1->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
       (iVar7 = *(int *)((int)worldNode1->runtimePayload + 8), factionIndex == *(int *)(iVar7 + 0xc)
       )) {
      AVar10 = ArmyAssetRegistry_FindByIdCf(*(PckArmyAssetIdCatalog *)(iVar7 + 0xa0));
      if (!AVar10.carry) {
        iVar3 = iVar3 + AVar10.eax[2].registryId;
      }
    }
  }
  uVar6 = g_GameFactionRuntimeImage.records[factionIndex].tritiumExtractionRateQ4PerTick;
  RVar2 = g_GameFactionRuntimeImage.records[factionIndex].xeniteExtractionRateQ4PerTick;
  TVar1 = g_GameFactionRuntimeImage.records[factionIndex].exploredTerrainPercent;
  FVar5 = g_GameFactionRuntimeImage.records[factionIndex].unlockedTechnologyCountBeyondBaseline;
  g_GameFactionRuntimeImage.records[factionIndex].activeArmyContribution = iVar3;
  g_GameFactionRuntimeImage.records[factionIndex].combinedProgressScore =
       ((uVar6 >> 1) + RVar2) * TVar1 * FVar5 >> 3;
  return;
}


/* Address: 0x00514900.
   Ownership: gameplay/faction/runtime.
   Purpose: Scans the selected faction's eight fixed runtime groups, each containing up to 32 pointers, and returns
   the one-based group index with carry clear or the original runtime pointer with carry set.
*/
FactionRuntimeGroupIndexEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GameFactionRuntime_FindRuntimeGroupIndexCf(RuntimeModelFactionPrefix10 *runtimeEntry)

{
  int iVar1;
  uint uVar2;
  ArmyRuntimeSlot **ppAVar3;
  ArmyRuntimeSlot **ppAVar4;
  bool bVar5;
  FactionRuntimeGroupIndexEaxCf5 FVar6;
  FactionRuntimeGroupIndexEaxCf5 FVar7;
  
  uVar2 = 0;
  ppAVar4 = g_GameFactionRuntimeImage.records[runtimeEntry->factionIndex].runtimeGroupMembers8x32;
  do {
    iVar1 = 0x20;
    uVar2 = uVar2 + 1;
    bVar5 = uVar2 == 0;
    ppAVar3 = ppAVar4;
    do {
      ppAVar4 = ppAVar3;
      if (iVar1 == 0) break;
      iVar1 = iVar1 + -1;
      ppAVar4 = ppAVar3 + 1;
      bVar5 = (ArmyRuntimeSlot *)runtimeEntry == *ppAVar3;
      ppAVar3 = ppAVar4;
    } while (!bVar5);
    if (bVar5) {
      FVar7.carry = false;
      FVar7.runtimeGroupIndex = uVar2;
      return FVar7;
    }
    if (7 < uVar2) {
      FVar6.carry = true;
      FVar6.runtimeGroupIndex = (dword)runtimeEntry;
      return FVar6;
    }
  } while( true );
}


/* Address: 0x0051B800.
   Ownership: gameplay/faction/runtime.
   Purpose: Tests the faction army-asset identifier list and then active class-0x0B or class-0x0D structures for a
   matching identifier, returning the result through carry.
*/
bool __thandor_cf_preserve_eax_ecx_edx
FactionRuntime_HasArmyAssetOrActiveStructureCf
          (FactionRuntimeIndex factionIndex,ArmyAssetRecordPrefix *armyAssetRecord)

{
  ArmyAssetRecordPrefix *pAVar1;
  WorldOwnerListNode100 *pWVar2;
  int *piVar3;
  FactionArmyAssetCount FVar4;
  dword *pdVar5;
  bool bVar6;
  
  FVar4 = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount;
  pdVar5 = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetPointersOrIds;
  bVar6 = FVar4 == 0;
  if (!bVar6) {
    do {
      if (FVar4 == 0) break;
      FVar4 = FVar4 - 1;
      bVar6 = armyAssetRecord == (ArmyAssetRecordPrefix *)*pdVar5;
      pdVar5 = pdVar5 + 1;
    } while (!bVar6);
    if (bVar6) {
      return false;
    }
  }
  pWVar2 = (g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
  do {
    if (pWVar2 == (WorldOwnerListNode100 *)0x0) {
      return true;
    }
    if ((pWVar2->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
       (piVar3 = pWVar2->runtimePayload, factionIndex == *(int *)(piVar3[2] + 0xc))) {
      if (*(int *)(*piVar3 + 0x4c) == 0xb) {
        if (piVar3[0x2e] == 1) {
          pAVar1 = (ArmyAssetRecordPrefix *)piVar3[0x18];
          goto joined_r0x0051b8c2;
        }
      }
      else if ((*(int *)(*piVar3 + 0x4c) == 0xd) && (piVar3[0x2e] == 1)) {
        pAVar1 = (ArmyAssetRecordPrefix *)piVar3[0x18];
joined_r0x0051b8c2:
        if (armyAssetRecord == pAVar1) {
          return false;
        }
      }
    }
    pWVar2 = pWVar2->nextNode;
  } while( true );
}


/* Address: 0x0051C4C0.
   Ownership: gameplay/faction/runtime.
   Purpose: Handles game entity runtime reset movement flags and anchor coordinates from model.
*/
void __thandor_void_preserve_eax_ecx
GameEntityRuntime_ResetMovementFlagsAndAnchorCoordinatesFromModel(GameEntityRuntime *entityRuntime)

{
  GameEntityCommandFlags *pGVar1;
  ModelRuntimeNode *pMVar2;
  GraphicsWorldCoordinateQ12 GVar3;
  GraphicsWorldCoordinateQ12 GVar4;
  
  pMVar2 = (entityRuntime->common).ownership.modelNode;
  pGVar1 = &(entityRuntime->common).commandFlags;
  *pGVar1 = *pGVar1 & 0xffffffc6;
  GVar3 = (pMVar2->worldTransform).translation.x;
  GVar4 = (pMVar2->worldTransform).translation.y;
  (entityRuntime->common).pathCoordinate0Q12 = GVar3;
  (entityRuntime->common).pathCoordinate1Q12 = GVar4;
  (entityRuntime->common).trackedCoordinate0Q12 = GVar3;
  (entityRuntime->common).trackedCoordinate1Q12 = GVar4;
  (entityRuntime->common).damageState.trackedCoordinate0Q12 = GVar3;
  (entityRuntime->common).damageState.trackedCoordinate1Q12 = GVar4;
  return;
}


/* Address: 0x0051C680.
   Ownership: gameplay/faction/runtime.
   Purpose: Resolves the active command target to EAX/ECX/EDX Q12 coordinates and reports failure through CF. The
   ordinary return remains void because the three-register result is not a C scalar return.
*/
WorldPositionEaxEcxEdxCf13
GameEntityRuntime_ResolveCommandTargetPositionCf(GameEntityRuntime *targetState)

{
  GameEntityRuntime *pGVar1;
  uint in_EAX;
  Q12 QVar2;
  int in_ECX;
  Q12 QVar3;
  int *in_EDX;
  int iVar4;
  ModelRuntimeNode *pMVar5;
  WorldPositionEaxEcxEdxCf13 WVar6;
  WorldPositionEaxEcxEdxCf13 WVar7;
  GameEntityRuntime *targetEntityRuntime;
  
  if (((targetState->common).commandTarget.targetFlags & 1) == 0) {
    if (((targetState->common).commandTarget.targetFlags & 2) != 0) {
      QVar2 = (targetState->common).commandTarget.targetWorldXQ12;
      QVar3 = (targetState->common).commandTarget.targetWorldYQ12;
      iVar4 = (targetState->common).commandTarget.targetWorldZQ12;
LAB_0051c6a4:
      WVar6.worldYQ12 = QVar3;
      WVar6.worldXQ12 = QVar2;
      WVar6.worldZQ12 = iVar4;
      WVar6.carry = false;
      return WVar6;
    }
  }
  else {
    pGVar1 = (targetState->common).commandTarget.targetEntity;
    in_EAX = 2;
    in_ECX = (targetState->common).ownership.ownerIndex * 2;
    if (pGVar1 != (GameEntityRuntime *)0x0) {
      in_EAX = 2 << ((byte)in_ECX & 0x1f);
      in_EDX = (pGVar1->common).ownership.definitionOrClassRecord;
      if ((*(uint *)((pGVar1->common).damageState.reserved0C_23 + 0x10) & in_EAX) != 0) {
        pMVar5 = (pGVar1->common).ownership.modelNode;
        if (*(int *)(*in_EDX + 0x4c) == 0x15) {
          pMVar5 = pMVar5->childNodes[0];
        }
        QVar2 = (pMVar5->worldTransform).translation.x;
        QVar3 = (pMVar5->worldTransform).translation.y;
        iVar4 = (pMVar5->worldTransform).translation.z + *(int *)(*in_EDX + 0x50);
        goto LAB_0051c6a4;
      }
      (targetState->common).commandTarget.targetEntity = (GameEntityRuntime *)0x0;
      (targetState->common).commandTarget.targetFlags = 0;
    }
  }
  WVar7.worldYQ12 = in_ECX;
  WVar7.worldXQ12 = in_EAX;
  WVar7.carry = true;
  WVar7.worldZQ12 = (Q12)in_EDX;
  return WVar7;
}


/* Address: 0x0052A4D0.
   Ownership: gameplay/faction/runtime.
   Purpose: Applies impact damage and state to one GameEntityRuntime, updates attached model/runtime state, and
   records the verified source/target faction relation counters. The function preserves the normal return registers
   and returns with RET 0x10. Typed parameters: p2 impactValue→ImpactDamageValue32_V342. Calling convention, exact
   VariableStorage serialization, function body bytes, control flow, globals, locals, and executable data remain
   unchanged.
   Cross-module calls: ArmyRuntime_ApplyDamageAndPropagateToParent [gameplay/army/combat].
*/
void __thandor_void_preserve_eax_ecx_edx
GameEntityRuntime_ApplyImpactDamageAndFactionRelationState
          (AngleTurn32 impactAngle,FactionRuntimeIndex sourceFactionIndex,
          ImpactDamageValue32 impactValue,GameEntityRuntime *targetEntityRuntime)

{
  int *piVar1;
  GameEntityRuntimeFlags *pGVar2;
  FactionRelationCounter *pFVar3;
  ArmyRuntimeSlot *armyRuntime;
  void *pvVar4;
  int iVar5;
  int iVar6;
  ModelRuntimeNode *modelNode1;
  
  if (impactValue < 0) {
    targetEntityRuntime = *(GameEntityRuntime **)(targetEntityRuntime->common).ownership.runtimeLink
    ;
  }
  (targetEntityRuntime->common).pathingAndImpactState.impactReaction.state08 = 0;
  (targetEntityRuntime->common).pathingAndImpactState.impactReaction.reactionCode09 = 2;
  (targetEntityRuntime->common).pathingAndImpactState.impactReaction.state0A = 0;
  (targetEntityRuntime->common).pathingAndImpactState.impactReaction.state0B = 0;
  if (0 < (targetEntityRuntime->common).damageState.remainingIntegrity) {
    iVar5 = *(int *)((int)(targetEntityRuntime->common).ownership.definitionOrClassRecord + 0x60);
    piVar1 = &(targetEntityRuntime->common).damageState.remainingIntegrity;
    iVar6 = *piVar1;
    *piVar1 = *piVar1 - impactValue;
    if (*piVar1 == 0 || SBORROW4(iVar6,impactValue) != *piVar1 < 0) {
      modelNode1 = (targetEntityRuntime->common).ownership.modelNode;
      iVar6 = (targetEntityRuntime->common).damageState.remainingIntegrity;
      pGVar2 = &(targetEntityRuntime->common).runtimeFlags;
      *pGVar2 = *pGVar2 | 8;
      modelNode1 = modelNode1->parentNode;
      (targetEntityRuntime->common).damageState.counterOrTerminalReference.terminalEntity =
           targetEntityRuntime;
      (targetEntityRuntime->common).damageState.remainingIntegrity = 0;
      if (modelNode1 == (ModelRuntimeNode *)0x0) {
        pvVar4 = (targetEntityRuntime->common).ownership.definitionOrClassRecord;
        if ((*(int *)((int)pvVar4 + 0x4c) == 0) && (*(int *)((int)pvVar4 + 0x278) == 0)) {
          (((targetEntityRuntime->common).ownership.modelNode)->modelPayload).worldRotationAngle0 =
               impactAngle;
        }
        if (impactValue != 0) {
          iVar6 = *(int *)((int)(targetEntityRuntime->common).ownership.runtimeLink + 0xc);
          iVar5 = *(int *)((int)(targetEntityRuntime->common).ownership.definitionOrClassRecord +
                          0x4c);
          pFVar3 = &g_GameFactionRuntimeImage.records[iVar6].relationCounterC;
          *pFVar3 = *pFVar3 + 1;
          pFVar3 = &g_GameFactionRuntimeImage.records[sourceFactionIndex].relationCounterE;
          *pFVar3 = *pFVar3 + 1;
          if (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand[iVar5] ==
              ArmyRuntime_ClassCommandHandlerGroupACf) {
            pFVar3 = &g_GameFactionRuntimeImage.records[iVar6].relationCounterD;
            *pFVar3 = *pFVar3 + 1;
            pFVar3 = &g_GameFactionRuntimeImage.records[sourceFactionIndex].relationCounterF;
            *pFVar3 = *pFVar3 + 1;
            pFVar3 = &g_GameFactionRuntimeImage.records[iVar6].relationCounterC;
            *pFVar3 = *pFVar3 + -1;
            pFVar3 = &g_GameFactionRuntimeImage.records[sourceFactionIndex].relationCounterE;
            *pFVar3 = *pFVar3 + -1;
          }
        }
      }
      else {
        ArmyRuntime_ApplyDamageAndPropagateToParent(-iVar6,(modelNode1->runtimePayload).armyRuntime)
        ;
      }
    }
    else {
      piVar1 = &(targetEntityRuntime->common).damageState.remainingIntegrity;
      iVar6 = iVar5 - *piVar1;
      if (iVar6 == 0 || iVar5 < *piVar1) {
        piVar1 = &(targetEntityRuntime->common).damageState.remainingIntegrity;
        *piVar1 = *piVar1 + iVar6;
        armyRuntime = (targetEntityRuntime->classPayload).impactOwnerLinks.primaryImpactArmyRuntime;
        iVar5 = (targetEntityRuntime->common).ownership.ownerIndex;
        for (; iVar5 != 0; iVar5 = iVar5 + -1) {
          if ((armyRuntime != (ArmyRuntimeSlot *)0x0) &&
             ((ModelRuntimeSlot *)armyRuntime->actionVector2Q12 !=
              (((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->classLinkState).
              modelLinkOrState60.modelRuntime)) {
            ArmyRuntime_ApplyDamageAndPropagateToParent(iVar6,armyRuntime);
            return;
          }
          armyRuntime = (targetEntityRuntime->classPayload).impactOwnerLinks.
                        secondaryImpactArmyRuntime;
          targetEntityRuntime =
               (GameEntityRuntime *)&(targetEntityRuntime->common).commandTarget.targetWorldXQ12;
        }
      }
    }
  }
  return;
}


/* Address: 0x00560110.
   Ownership: gameplay/faction/runtime.
   Purpose: Resolves an army asset registry ID and appends the resulting pointer to the selected faction runtime
   record for the requested repetition count, bounded by the verified 64-entry array. It is distinct from
   FrontendPlayerIndex_V306, PlayerRuntimeId, active-faction masks or codes, and PCK-backed ArmyAssetId,
   ModelDefinitionId, and TechnologyId domains. Typed parameters: p3 repetitionCount→FactionArmyAssetCount_V304.
   Nearby but non-identical semantic domains were explicitly deferred. Calling convention, parameter storage, body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: ArmyAssetRegistry_FindByIdCf [assets/army/catalog].
*/
void __thandor_void_preserve_eax_ecx_edx
GameFactionRuntime_RegisterArmyAssetPointers
          (dword reservedDword0,FactionArmyAssetCount repetitionCount,
          PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex)

{
  FactionArmyAssetCount *pFVar1;
  FactionArmyAssetCount FVar2;
  ArmyRegistryEaxCf5_51b6d0 AVar3;
  
  AVar3 = ArmyAssetRegistry_FindByIdCf(armyAssetId);
  if (!AVar3.carry) {
    FVar2 = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount;
    do {
      if (0x3f < FVar2) {
        return;
      }
      *(ArmyAssetRecordPrefix **)(factionIndex * 0x740 + 0x50f420 + FVar2 * 4) = AVar3.eax;
      pFVar1 = &g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount;
      *pFVar1 = *pFVar1 + 1;
      FVar2 = FVar2 + 1;
      repetitionCount = repetitionCount - 1;
    } while (repetitionCount != 0);
  }
  return;
}


/* Address: 0x00560160.
   Ownership: gameplay/faction/runtime.
   Purpose: Removes requested matching army-asset pointers from a faction queue, cancels remaining matching runtime
   entries, and refunds their full stored asset value. ArmyAssetId is preserved through candidate, queue, refund,
   transfer, and sale paths; cost and eligibility semantics remain those proved by the live consumers and PCK
   records. It is distinct from FrontendPlayerIndex_V306, PlayerRuntimeId, active-faction masks or codes, and PCK-
   backed ArmyAssetId, ModelDefinitionId, and TechnologyId domains. Typed parameters: p3
   requestedCount→FactionArmyAssetCount_V304. Nearby but non-identical semantic domains were explicitly deferred.
   Cross-module calls: ArmyAssetRegistry_FindByIdCf [assets/army/catalog].
*/
void __thandor_void_preserve_eax_ecx_edx
GameFactionRuntime_CancelQueuedArmyAssetsAndRefund
          (dword reservedDword0,FactionArmyAssetCount requestedCount,
          PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex)

{
  FactionArmyAssetCount *pFVar1;
  int *piVar2;
  PckArmyAssetIdCatalog PVar3;
  ArmyAssetRecordPrefix *armyDefinition1;
  FactionArmyAssetCount FVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  WorldOwnerListNode100 *worldNode1;
  ArmyRegistryEaxCf5_51b6d0 AVar8;
  
  AVar8 = ArmyAssetRegistry_FindByIdCf(armyAssetId);
  armyDefinition1 = AVar8.eax;
  if (!AVar8.carry) {
    iVar5 = factionIndex * 0x740;
    iVar7 = 0;
    iVar6 = 0;
    for (FVar4 = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount; FVar4 != 0
        ; FVar4 = FVar4 - 1) {
      while ((armyDefinition1 == *(ArmyAssetRecordPrefix **)(iVar5 + 0x50f420 + iVar7 * 4) &&
             (0 < (int)requestedCount))) {
        iVar7 = iVar7 + 1;
        pFVar1 = &g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount;
        *pFVar1 = *pFVar1 - 1;
        requestedCount = requestedCount - 1;
        FVar4 = FVar4 - 1;
        if (FVar4 == 0)
        goto 
        GameFactionRuntime_CancelQueuedArmyAssetsAndRefund_ContinueWithActiveRuntimeCancellation;
      }
      *(undefined4 *)(iVar5 + 0x50f420 + iVar6 * 4) = *(undefined4 *)(iVar5 + 0x50f420 + iVar7 * 4);
      iVar7 = iVar7 + 1;
      iVar6 = iVar6 + 1;
    }
GameFactionRuntime_CancelQueuedArmyAssetsAndRefund_ContinueWithActiveRuntimeCancellation:
    if (requestedCount != 0) {
      worldNode1 = (g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
      if (worldNode1 != (WorldOwnerListNode100 *)0x0) {
        iVar7 = 0xb;
        if (((armyDefinition1[1].selectionDetailTemplateVariantIndex & 0x10) == 0) &&
           (iVar7 = 0x16, (armyDefinition1[1].selectionDetailTemplateVariantIndex & 8) == 0)) {
          iVar7 = 0xd;
        }
        do {
          if (worldNode1->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
            piVar2 = worldNode1->runtimePayload;
            if ((factionIndex == *(int *)(piVar2[2] + 0xc)) && (iVar7 == *(int *)(*piVar2 + 0x4c)))
            {
              if (iVar7 == 0xd) {
                if ((((*(uint *)(*piVar2 + 0xc4) &
                      armyDefinition1[1].selectionDetailTemplateVariantIndex) != 0) &&
                    (armyDefinition1->registryId == piVar2[0x18])) && (piVar2[0x2e] == 1)) {
                  piVar2[0x3d] = piVar2[0x3d] - piVar2[0x1d];
                  PVar3 = armyDefinition1[2].registryId;
                  piVar2[0x1d] = 0;
                  piVar2[0x2e] = 0;
                  piVar2[0x3b] = piVar2[0x3b] & 0xfffffeff;
                  piVar2[0x18] = 0;
                  g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 =
                       g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 + PVar3;
                  goto joined_r0x005603c3;
                }
              }
              else if (iVar7 == 0xb) {
                if (armyDefinition1->registryId == piVar2[0x18]) {
                  piVar2[0x3d] = piVar2[0x3d] - piVar2[0x1d];
                  PVar3 = armyDefinition1[2].registryId;
                  piVar2[0x1d] = 0;
                  piVar2[0x2e] = 0;
                  piVar2[0x3b] = piVar2[0x3b] & 0xfffffeff;
                  piVar2[0x18] = 0;
                  g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 =
                       g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 + PVar3;
                  goto joined_r0x005603c3;
                }
              }
              else if (armyDefinition1->registryId == piVar2[0x18]) {
                piVar2[0x3d] = piVar2[0x3d] - piVar2[0x1d];
                PVar3 = armyDefinition1[2].registryId;
                piVar2[0x1d] = 0;
                piVar2[0x2b] = 0;
                piVar2[0x3b] = piVar2[0x3b] & 0xfffffeff;
                piVar2[0x18] = 0;
                g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 =
                     g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 + PVar3;
joined_r0x005603c3:
                requestedCount = requestedCount - 1;
                if (requestedCount == 0) {
                  return;
                }
              }
            }
          }
          worldNode1 = worldNode1->nextNode;
        } while (worldNode1 != (WorldOwnerListNode100 *)0x0);
      }
    }
  }
  return;
}


/* Address: 0x00560400.
   Ownership: gameplay/faction/runtime.
   Purpose: Removes one army-asset pointer from the faction array, stores it in the player pending-transfer slot,
   compacts the array, and refreshes the local grid state. ArmyAssetId is preserved through candidate, queue,
   refund, transfer, and sale paths; cost and eligibility semantics remain those proved by the live consumers and
   PCK records. ArmyAssetId remains the PCK-backed asset identity; transfer, queue, and refund operations do not
   collapse these domains.
   Cross-module calls: ArmyAssetRegistry_FindByIdCf [assets/army/catalog], UiCommandSpriteVariantA_RebuildGrid
   [ui/ingame/commands].
*/
void __thandor_void_preserve_eax_ecx_edx
GameFactionRuntime_RemoveArmyAssetAndStagePlayerTransfer
          (PlayerRuntimeId playerRuntimeId,dword reservedDword04,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex)

{
  FactionArmyAssetCount *pFVar1;
  SelectionPlayerRuntimeBlock *pSVar2;
  InGameRuntimeRootImageC3E4 *node;
  ArmyAssetRecordPrefix *armyDefinition1;
  int iVar3;
  FactionArmyAssetCount FVar4;
  ArmyRegistryEaxCf5_51b6d0 AVar5;
  
  AVar5 = ArmyAssetRegistry_FindByIdCf(armyAssetId);
  armyDefinition1 = AVar5.eax;
  if (!AVar5.carry) {
    pSVar2 = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId];
    iVar3 = factionIndex * 0x740;
    FVar4 = g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
    pSVar2->pendingSelectionEntityOffset8098 = (dword)armyDefinition1;
    for (; FVar4 != 0; FVar4 = FVar4 - 1) {
      if (armyDefinition1 ==
          *(ArmyAssetRecordPrefix **)
           ((int)g_GameFactionRuntimeImage.records[0].primaryArmyAssetPointersOrIds + iVar3)) {
        do {
          *(undefined4 *)
           ((int)g_GameFactionRuntimeImage.records[0].primaryArmyAssetPointersOrIds + iVar3) =
               *(undefined4 *)
                ((int)g_GameFactionRuntimeImage.records[0].primaryArmyAssetPointersOrIds + iVar3 + 4
                );
          node = g_InGameRuntimeRoot;
          iVar3 = iVar3 + 4;
          FVar4 = FVar4 - 1;
        } while (FVar4 != 0);
        iVar3 = (g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex;
        pFVar1 = &g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
        *pFVar1 = *pFVar1 - 1;
        if (factionIndex != iVar3) {
          return;
        }
        UiCommandSpriteVariantA_RebuildGrid((UiNodeBase *)node);
        if (playerRuntimeId != g_LocalPlayerRuntimeId) {
          return;
        }
        g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | 0x20;
        g_InGamePendingPlacementArmyAsset = (sdword)armyDefinition1;
        g_InGamePlacementSurfaceHeightQ12OrSentinel = 0x7fffffff;
        return;
      }
      iVar3 = iVar3 + 4;
    }
    pSVar2->pendingSelectionEntityOffset8098 = 0;
  }
  return;
}


/* Address: 0x00560620.
   Ownership: gameplay/faction/runtime.
   Purpose: Atomically consumes one pending per-player army asset pointer, appends it to the selected faction
   runtime record, rebuilds the local command grid when applicable, and clears the pending UI flag. ArmyAssetId
   remains the PCK-backed asset identity; transfer, queue, and refund operations do not collapse these domains.
   Cross-module calls: UiCommandSpriteVariantA_RebuildGrid [ui/ingame/commands].
*/
void __thandor_void_preserve_eax_ecx_edx
GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid
          (PlayerRuntimeId playerRuntimeId,dword unusedConsumeArgument0,dword unusedConsumeArgument1
          ,FactionRuntimeIndex factionIndex)

{
  FactionArmyAssetCount *pFVar1;
  dword dVar2;
  uint uVar3;
  InGameRuntimeRootImageC3E4 *node;
  
  LOCK();
  dVar2 = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->pendingSelectionEntityOffset8098;
  g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->pendingSelectionEntityOffset8098 = 0;
  node = g_InGameRuntimeRoot;
  UNLOCK();
  if (dVar2 != 0) {
    uVar3 = g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
    if (uVar3 < 0x40) {
      *(dword *)(factionIndex * 0x740 + 0x50f520 + uVar3 * 4) = dVar2;
      pFVar1 = &g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
      *pFVar1 = *pFVar1 + 1;
    }
    if (factionIndex == (node->worldRuntime0A30).activeFactionRuntimeIndex) {
      UiCommandSpriteVariantA_RebuildGrid((UiNodeBase *)node);
      if (playerRuntimeId == g_LocalPlayerRuntimeId) {
        g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & 0xffffffdf;
        g_InGamePendingPlacementArmyAsset = 0;
      }
    }
  }
  return;
}


/* Address: 0x005606A0.
   Ownership: gameplay/faction/runtime.
   Purpose: Removes one army-asset pointer from the faction array, refunds seven eighths of its stored value,
   compacts the array, and refreshes the local grid state. ArmyAssetId is preserved through candidate, queue,
   refund, transfer, and sale paths; cost and eligibility semantics remain those proved by the live consumers and
   PCK records. It is distinct from FrontendPlayerIndex_V306, PlayerRuntimeId, active-faction masks or codes, and
   PCK-backed ArmyAssetId, ModelDefinitionId, and TechnologyId domains.
   Cross-module calls: ArmyAssetRegistry_FindByIdCf [assets/army/catalog], UiCommandSpriteVariantA_RebuildGrid
   [ui/ingame/commands].
*/
void __thandor_void_preserve_eax_ecx_edx
GameFactionRuntime_SellArmyAssetAndRefundSevenEighths
          (dword unusedSaleArgument0,dword unusedSaleArgument1,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex)

{
  FactionArmyAssetCount *pFVar1;
  PckArmyAssetIdCatalog PVar2;
  InGameRuntimeRootImageC3E4 *node;
  int iVar3;
  FactionArmyAssetCount FVar4;
  ArmyRegistryEaxCf5_51b6d0 AVar5;
  
  AVar5 = ArmyAssetRegistry_FindByIdCf(armyAssetId);
  if (!AVar5.carry) {
    iVar3 = factionIndex * 0x740;
    for (FVar4 = g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount; FVar4 != 0;
        FVar4 = FVar4 - 1) {
      if (AVar5.eax ==
          *(ArmyAssetRecordPrefix **)
           ((int)g_GameFactionRuntimeImage.records[0].primaryArmyAssetPointersOrIds + iVar3)) {
        do {
          *(undefined4 *)
           ((int)g_GameFactionRuntimeImage.records[0].primaryArmyAssetPointersOrIds + iVar3) =
               *(undefined4 *)
                ((int)g_GameFactionRuntimeImage.records[0].primaryArmyAssetPointersOrIds + iVar3 + 4
                );
          node = g_InGameRuntimeRoot;
          iVar3 = iVar3 + 4;
          FVar4 = FVar4 - 1;
        } while (FVar4 != 0);
        PVar2 = AVar5.eax[2].registryId;
        iVar3 = (g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex;
        pFVar1 = &g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
        *pFVar1 = *pFVar1 - 1;
        g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 =
             g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 +
             ((int)(PVar2 * 7) >> 3);
        if (iVar3 != factionIndex) {
          return;
        }
        UiCommandSpriteVariantA_RebuildGrid((UiNodeBase *)node);
        return;
      }
      iVar3 = iVar3 + 4;
    }
  }
  return;
}


/* Address: 0x00561F80.
   Ownership: gameplay/faction/runtime.
   Purpose: Resolves a runtime value through the existing lookup helper and stores the resulting relative token at
   player-runtime offset 0x8094, or zero on failure. Kept distinct from frontend slot indices, faction runtime
   indices, network endpoint identity, and PCK asset identifiers. Typed parameters: p2
   playerRuntimeId→PlayerRuntimeId. Calling convention, storage, body bytes, control flow, and executable data
   remain unchanged. Typed parameters: p5 lookupToken→RuntimeToken.
   Cross-module calls: ArmyRuntime_CreateInstanceFromAssetCf [gameplay/army/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
PlayerRuntime_ResolveAndStoreState8094
          (PlayerRuntimeId playerRuntimeId,PlayerStateLookupValue0 lookupValue0,
          PlayerStateLookupValue1 lookupValue1,RuntimeToken lookupToken)

{
  SelectionPlayerRuntimeBlock *pSVar1;
  ArmyRuntimeCreateEaxCf5 AVar2;
  
  pSVar1 = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId];
  AVar2 = ArmyRuntime_CreateInstanceFromAssetCf
                    (4,0,lookupValue0,lookupValue1,pSVar1->constructionLookupState8090,lookupToken,
                     &g_InGameRuntimeRoot->worldRuntime0A30);
  if (!AVar2.carry) {
    pSVar1->primarySelectionEntityOffset8094 = AVar2.eax - (int)g_ArmyRuntimeRebaseBaseMinusOne;
    return;
  }
  pSVar1->primarySelectionEntityOffset8094 = 0;
  return;
}


/* Address: 0x00561FF0.
   Ownership: gameplay/faction/runtime.
   Purpose: Stores the caller-provided value at per-player runtime offset 0x8090. Kept distinct from frontend slot
   indices, faction runtime indices, network endpoint identity, and PCK asset identifiers. Typed parameters: p2
   playerRuntimeId→PlayerRuntimeId. Calling convention, storage, body bytes, control flow, and executable data
   remain unchanged. Typed parameters: p5 stateValue→PlayerState8090Value_V344.
*/
void __thandor_void_preserve_eax_ecx_edx
PlayerRuntime_SetState8090
          (PlayerRuntimeId playerRuntimeId,dword reservedZero0,dword reservedZero1,
          PlayerState8090Value stateValue)

{
  g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->constructionLookupState8090 = stateValue;
  return;
}


/* Address: 0x00562020.
   Ownership: gameplay/faction/runtime.
   Purpose: Stores the caller-provided value at per-player runtime offset 0x8094. Kept distinct from frontend slot
   indices, faction runtime indices, network endpoint identity, and PCK asset identifiers. Typed parameters: p2
   playerRuntimeId→PlayerRuntimeId. Calling convention, storage, body bytes, control flow, and executable data
   remain unchanged. Typed parameters: p5 stateValue→PlayerState8094Value_V344.
*/
void __thandor_void_preserve_eax_ecx_edx
PlayerRuntime_SetState8094
          (PlayerRuntimeId playerRuntimeId,dword reservedZero0,dword reservedZero1,
          PlayerState8094Value stateValue)

{
  g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->primarySelectionEntityOffset8094 =
       stateValue;
  return;
}


/* Address: 0x005622C0.
   Ownership: gameplay/faction/runtime.
   Purpose: Clears per-player runtime offset 0x8094.
*/
void __thandor_void_preserve_eax_ecx_edx
PlayerRuntime_ClearState8094
          (PlayerRuntimeId playerRuntimeId,dword reservedZero0,dword reservedZero1,
          dword reservedZero2)

{
  g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->primarySelectionEntityOffset8094 = 0;
  return;
}


/* Address: 0x00565590.
   Ownership: gameplay/faction/runtime.
   Purpose: ORs the exact 64-dword secondary old-unit mask table into eight faction records at offset +0x6E0, then,
   when runtime state and imported records are available, replays fixed 0x20-byte primary records, runs two
   verified traversal callbacks, and refreshes the paired runtime values. Existing EAX and EDX results are
   preserved.
   Cross-module calls: ArmyRuntime_CreateInstanceFromAssetCf [gameplay/army/runtime],
   WorldRuntime_ForEachNodeInOwnerListD8 [world/runtime/core], FieldGrid_ClassifyCellFlagsToRuntimeByte
   [world/terrain/grid].
*/
void __thandor_void_preserve_eax_ecx_edx OldUnitRuntime_MergeMasksAndReplayRecords(void)

{
  InGameRuntimeRootImageC3E4 *pIVar1;
  dword *pdVar2;
  int iVar3;
  OldUnitRecordCount OVar4;
  int iVar5;
  WorldRuntimeContext *worldRuntime;
  dword *pdVar6;
  dword *pPVar7;
  dword *pdVar7;
  
  iVar3 = 8;
  iVar5 = 8;
  pdVar6 = g_OldUnitSecondaryTable;
  pdVar2 = g_GameFactionRuntimeImage.records[0].technologyMasks256Bits;
  do {
    do {
      pdVar7 = pdVar2;
      *pdVar7 = *pdVar7 | *pdVar6;
      pIVar1 = g_InGameRuntimeRoot;
      pdVar6 = pdVar6 + 1;
      iVar5 = iVar5 + -1;
      pdVar2 = pdVar7 + 1;
    } while (iVar5 != 0);
    iVar5 = 8;
    iVar3 = iVar3 + -1;
    pdVar2 = pdVar7 + 0x1c9;
  } while (iVar3 != 0);
  if ((g_InGameRuntimeRoot != (InGameRuntimeRootImageC3E4 *)0x0) && (g_OldUnitRecordCount != 0)) {
    worldRuntime = &g_InGameRuntimeRoot->worldRuntime0A30;
    OVar4 = g_OldUnitRecordCount;
    pPVar7 = g_OldUnitPrimaryTable;
    do {
      ArmyRuntime_CreateInstanceFromAssetCf
                (6,pPVar7[4],pPVar7[3],pPVar7[2],pPVar7[1],*pPVar7,worldRuntime);
      pPVar7 = pPVar7 + 8;
      OVar4 = OVar4 - 1;
    } while (OVar4 != 0);
    WorldRuntime_ForEachNodeInOwnerListD8
              (worldRuntime,ArmyRuntimeNode_AccumulateTerrainOcclusionAndOccupancyCallback,
               worldRuntime);
    WorldRuntime_ForEachNodeInOwnerListD8
              (worldRuntime,ArmyRuntimeNode_RebuildTerrainOccupancyAndVisualStateCallback,
               worldRuntime);
    FieldGrid_ClassifyCellFlagsToRuntimeByte
              ((pIVar1->worldRuntime0A30).activeFactionRuntimeIndex,
               (pIVar1->worldRuntime0A30).fieldGrid);
  }
  return;
}


/* Address: 0x00513D00.
   Ownership: gameplay/faction/runtime.
   Purpose: Checks the packed relation nibble for factionIndex toward otherFactionIndex. CF is set only for states
   2, 5, or 9 whose bidirectional timestamp age is at most 0x258 ticks; EAX is preserved.
*/
bool __thandor_cf_preserve_eax_ecx_edx
GameFactionRuntime_IsRecentTimedRelationStateCf
          (FactionRuntimeIndex otherFactionIndex,FactionRuntimeIndex factionIndex)

{
  uint relationStateNibble;
  
  relationStateNibble =
       g_GameFactionRuntimeImage.records[factionIndex].packedRelationStates >>
       ((char)otherFactionIndex * '\x04' & 0x1fU) & 0xf;
  if ((((relationStateNibble == 2) || (relationStateNibble == 5)) || (relationStateNibble == 9)) &&
     ((int)(g_GameFactionRuntimeImage.tail.simulationTick -
           *(int *)(factionIndex * 0x740 + 0x50fa40 + otherFactionIndex * 4)) < 0x259)) {
    return true;
  }
  return false;
}


/* Address: 0x00565650.
   Ownership: gameplay/faction/runtime.
   Purpose: Clears all 64 dwords in the secondary old-unit mask table and resets the imported primary-record count
   to zero. EAX is preserved.
*/
void __thandor_void_preserve_eax_ecx OldUnitRuntime_ResetPendingTables(void)

{
  int tableEntriesRemaining;
  dword *tableCursor;
  
  tableCursor = g_OldUnitSecondaryTable;
  for (tableEntriesRemaining = 0x40; tableEntriesRemaining != 0;
      tableEntriesRemaining = tableEntriesRemaining + -1) {
    *tableCursor = 0;
    tableCursor = tableCursor + 1;
  }
  g_OldUnitRecordCount = 0;
  return;
}


/* Address: 0x00513EE0.
   Ownership: gameplay/faction/runtime.
   Purpose: Writes the two directed four-bit relation states, updates reciprocal state masks and timestamps, emits
   active-faction notifications, and rebuilds the other-player UI. When stateSecondTowardFirst is 0x0B, the routine
   additionally merges the selected faction's units, ownership, resources, technology masks, and runtime references
   into the surviving faction. It is distinct from FrontendPlayerIndex_V306, PlayerRuntimeId, active-faction masks
   or codes, and PCK-backed ArmyAssetId, ModelDefinitionId, and TechnologyId domains. Typed parameters: p0
   activeFactionCodeForFirst→FactionNotificationCodeBase_V344, p1
   activeFactionCodeForSecond→FactionNotificationCodeBase_V344, p2
   stateFirstTowardSecond→FactionRelationStateNibble_V344, p3
   stateSecondTowardFirst→FactionRelationStateNibble_V344. Calling convention, complete VariableStorage
   serialization, function bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: InGameNotificationQueue_InsertPriorityRecord [ui/ingame/runtime],
   ModelRuntimeHierarchy_SetCommandTargetRecursive [world/model/hierarchy], UiCommandSpriteVariantA_RebuildGrid
   [ui/ingame/commands], UiCatalogGroup42_RebuildGrid [ui/ingame/technology], UiCatalogGroup48_RebuildGrid
   [ui/ingame/technology], InGameOtherPlayerCommand_RebuildTargetEntries [ui/ingame/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
GameFactionRuntime_ApplyPairwiseRelationTransition
          (FactionNotificationCodeBase activeFactionCodeForFirst,
          FactionNotificationCodeBase activeFactionCodeForSecond,
          FactionRelationStateNibble stateFirstTowardSecond,
          FactionRelationStateNibble stateSecondTowardFirst,FactionRuntimeIndex firstFactionIndex,
          FactionRuntimeIndex secondFactionIndex)

{
  FactionCapabilityFlags *pFVar1;
  TritiumAmountQ4 *pTVar2;
  XeniteAmountQ4 *pXVar3;
  EnergyAmountQ4 *pEVar4;
  dword *pdVar5;
  FactionRelationCounter *pFVar6;
  FactionArmyAssetCount *pFVar7;
  TritiumAmountQ4 TVar8;
  XeniteAmountQ4 XVar9;
  TritiumAmountQ4 TVar10;
  EnergyAmountQ4 EVar11;
  uint uVar12;
  int iVar13;
  int iVar14;
  InGameSimulationTick IVar15;
  FactionRuntimeIndex FVar16;
  uint uVar17;
  dword dVar18;
  byte bVar19;
  byte bVar20;
  FactionArmyAssetCount FVar21;
  uint uVar22;
  FrontendPlayerRuntimeRecord *pFVar23;
  int iVar24;
  FrontendPlayerRuntimeBlockCount FVar25;
  FieldGridCell *gridCell1;
  dword primaryArmyAssetReferenceDword;
  dword secondaryArmyAssetReferenceDword;
  GraphicsTextureSet *survivingFactionTextureSet;
  GraphicsPaletteAsset *survivingFactionPaletteAsset;
  FieldGridAsset *fieldGrid1;
  InGameRuntimeRootImageC3E4 *gameRuntime1;
  WorldOwnerListNode100 *worldNode1;
  
  FVar16 = firstFactionIndex;
  if ((g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex == secondFactionIndex) {
    InGameNotificationQueue_InsertPriorityRecord(NONE,0,0,0,0,0,2,activeFactionCodeForSecond + 500);
  }
  else if ((g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex == firstFactionIndex) {
    InGameNotificationQueue_InsertPriorityRecord(NONE,0,0,0,0,0,2,activeFactionCodeForFirst + 500);
  }
  bVar19 = (byte)secondFactionIndex * '\x04';
  bVar20 = (byte)firstFactionIndex * '\x04';
  g_GameFactionRuntimeImage.records[firstFactionIndex].packedRelationStates =
       stateFirstTowardSecond << (bVar19 & 0x1f) |
       ~(0xf << (bVar19 & 0x1f)) &
       g_GameFactionRuntimeImage.records[firstFactionIndex].packedRelationStates;
  g_GameFactionRuntimeImage.records[secondFactionIndex].packedRelationStates =
       ~(0xf << (bVar20 & 0x1f)) &
       g_GameFactionRuntimeImage.records[secondFactionIndex].packedRelationStates |
       stateSecondTowardFirst << (bVar20 & 0x1f);
  uVar17 = 1 << ((byte)secondFactionIndex & 0x1f);
  if (stateSecondTowardFirst < 4) {
    pFVar1 = &g_GameFactionRuntimeImage.records[firstFactionIndex].capabilityFlags;
    *pFVar1 = *pFVar1 & ~uVar17;
  }
  else {
    pFVar1 = &g_GameFactionRuntimeImage.records[firstFactionIndex].capabilityFlags;
    *pFVar1 = *pFVar1 | uVar17;
  }
  uVar17 = 1 << ((byte)firstFactionIndex & 0x1f);
  if (stateSecondTowardFirst < 4) {
    pFVar1 = &g_GameFactionRuntimeImage.records[secondFactionIndex].capabilityFlags;
    *pFVar1 = *pFVar1 & ~uVar17;
  }
  else {
    pFVar1 = &g_GameFactionRuntimeImage.records[secondFactionIndex].capabilityFlags;
    *pFVar1 = *pFVar1 | uVar17;
  }
  IVar15 = g_GameFactionRuntimeImage.tail.simulationTick;
  *(InGameSimulationTick *)(firstFactionIndex * 0x740 + 0x50fa40 + secondFactionIndex * 4) =
       g_GameFactionRuntimeImage.tail.simulationTick;
  *(InGameSimulationTick *)(secondFactionIndex * 0x740 + 0x50fa40 + firstFactionIndex * 4) = IVar15;
  if (stateSecondTowardFirst != 0xb)
  goto GameFactionRuntime_ApplyPairwiseRelationTransition_RebuildTargetEntriesAndReturn;
  uVar17 = 0;
  uVar22 = 0;
  FVar25 = g_FrontendPlayerRuntimeBlockCount;
  pFVar23 = g_FrontendPlayerRuntimeBlocks;
  do {
    if (secondFactionIndex == (pFVar23->factionAssignment).factionAssignmentIndex) {
      uVar17 = uVar17 + 1;
    }
    if (firstFactionIndex == (pFVar23->factionAssignment).factionAssignmentIndex) {
      uVar22 = uVar22 + 1;
    }
    pFVar23 = pFVar23 + 1;
    FVar25 = FVar25 - 1;
  } while (FVar25 != 0);
  if ((uVar17 & uVar22) == 0) {
    if (uVar17 != 0)
    goto GameFactionRuntime_ApplyPairwiseRelationTransition_CommitSelectedFactionMergeDirection;
    if (uVar22 == 0)
    goto GameFactionRuntime_ApplyPairwiseRelationTransition_ResolveState11FactionMergeDirection;
  }
  else {
GameFactionRuntime_ApplyPairwiseRelationTransition_ResolveState11FactionMergeDirection:
    dVar18 = (*g_RandomGeneratorState.next)();
    if ((dVar18 & 0x2000) == 0) {
GameFactionRuntime_ApplyPairwiseRelationTransition_CommitSelectedFactionMergeDirection:
      firstFactionIndex = secondFactionIndex;
      secondFactionIndex = FVar16;
    }
  }
  gameRuntime1 = g_InGameRuntimeRoot;
  if (secondFactionIndex == (g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex) {
    (g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex = firstFactionIndex;
  }
  survivingFactionTextureSet = g_ArmyGraphicsBindings[firstFactionIndex].textureSet;
  survivingFactionPaletteAsset = g_ArmyGraphicsBindings[firstFactionIndex].paletteAsset;
  pFVar23 = g_FrontendPlayerRuntimeBlocks;
  FVar25 = g_FrontendPlayerRuntimeBlockCount;
  for (worldNode1 = (gameRuntime1->worldRuntime0A30).ownerListHead;
      g_FrontendPlayerRuntimeBlocks = pFVar23, g_FrontendPlayerRuntimeBlockCount = FVar25,
      worldNode1 != (WorldOwnerListNode100 *)0x0; worldNode1 = worldNode1->nextNode) {
    if ((worldNode1->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
       (iVar24 = *(int *)((int)worldNode1->runtimePayload + 8),
       *(int *)(iVar24 + 0xc) == secondFactionIndex)) {
      *(FactionRuntimeIndex *)(iVar24 + 0xc) = firstFactionIndex;
      ModelRuntimeHierarchy_SetCommandTargetRecursive
                (survivingFactionPaletteAsset,survivingFactionTextureSet,
                 *(ModelRuntimeNode **)(iVar24 + 4));
    }
    pFVar23 = g_FrontendPlayerRuntimeBlocks;
    FVar25 = g_FrontendPlayerRuntimeBlockCount;
  }
  do {
    if (secondFactionIndex == (pFVar23->factionAssignment).factionAssignmentIndex) {
      iVar24 = pFVar23->playerRuntimeId;
      (pFVar23->factionAssignment).factionAssignmentIndex = firstFactionIndex;
      g_SelectionPlayerRuntimeBlockPointers[iVar24]->primaryEntityOrFactionToken8080 =
           firstFactionIndex;
    }
    pFVar23 = pFVar23 + 1;
    FVar25 = FVar25 - 1;
  } while (FVar25 != 0);
  fieldGrid1 = (gameRuntime1->worldRuntime0A30).fieldGrid;
  iVar24 = fieldGrid1->gridWidth * fieldGrid1->gridHeight;
  gridCell1 = fieldGrid1->cells;
  do {
    gridCell1->runtime60_6B[firstFactionIndex + 0x10] =
         gridCell1->runtime60_6B[firstFactionIndex + 0x10] |
         gridCell1->runtime60_6B[secondFactionIndex + 0x10];
    gridCell1 = gridCell1 + 1;
    iVar24 = iVar24 + -1;
  } while (iVar24 != 0);
  g_GameFactionRuntimeImage.tail.factionLifecycleStates[secondFactionIndex] = 0;
  TVar8 = g_GameFactionRuntimeImage.records[secondFactionIndex].tritiumCurrentQ4;
  XVar9 = g_GameFactionRuntimeImage.records[secondFactionIndex].xeniteStorageLimitQ4;
  TVar10 = g_GameFactionRuntimeImage.records[secondFactionIndex].tritiumStorageLimitQ4;
  g_GameFactionRuntimeImage.records[firstFactionIndex].xeniteCurrentQ4 =
       g_GameFactionRuntimeImage.records[firstFactionIndex].xeniteCurrentQ4 +
       g_GameFactionRuntimeImage.records[secondFactionIndex].xeniteCurrentQ4;
  pTVar2 = &g_GameFactionRuntimeImage.records[firstFactionIndex].tritiumCurrentQ4;
  *pTVar2 = *pTVar2 + TVar8;
  pXVar3 = &g_GameFactionRuntimeImage.records[firstFactionIndex].xeniteStorageLimitQ4;
  *pXVar3 = *pXVar3 + XVar9;
  pTVar2 = &g_GameFactionRuntimeImage.records[firstFactionIndex].tritiumStorageLimitQ4;
  *pTVar2 = *pTVar2 + TVar10;
  EVar11 = g_GameFactionRuntimeImage.records[secondFactionIndex].energyGenerationCapacityQ4;
  pEVar4 = &g_GameFactionRuntimeImage.records[firstFactionIndex].baselineEnergySupplyQ4;
  *pEVar4 = *pEVar4 + g_GameFactionRuntimeImage.records[secondFactionIndex].baselineEnergySupplyQ4;
  pEVar4 = &g_GameFactionRuntimeImage.records[firstFactionIndex].energyGenerationCapacityQ4;
  *pEVar4 = *pEVar4 + EVar11;
  uVar17 = g_GameFactionRuntimeImage.records[secondFactionIndex].technologyMasks256Bits[1];
  uVar22 = g_GameFactionRuntimeImage.records[secondFactionIndex].technologyMasks256Bits[2];
  uVar12 = g_GameFactionRuntimeImage.records[secondFactionIndex].technologyMasks256Bits[3];
  pdVar5 = g_GameFactionRuntimeImage.records[firstFactionIndex].technologyMasks256Bits;
  *pdVar5 = *pdVar5 | g_GameFactionRuntimeImage.records[secondFactionIndex].technologyMasks256Bits
                      [0];
  pdVar5 = g_GameFactionRuntimeImage.records[firstFactionIndex].technologyMasks256Bits + 1;
  *pdVar5 = *pdVar5 | uVar17;
  pdVar5 = g_GameFactionRuntimeImage.records[firstFactionIndex].technologyMasks256Bits + 2;
  *pdVar5 = *pdVar5 | uVar22;
  pdVar5 = g_GameFactionRuntimeImage.records[firstFactionIndex].technologyMasks256Bits + 3;
  *pdVar5 = *pdVar5 | uVar12;
  uVar17 = g_GameFactionRuntimeImage.records[secondFactionIndex].technologyMasks256Bits[5];
  uVar22 = g_GameFactionRuntimeImage.records[secondFactionIndex].technologyMasks256Bits[6];
  uVar12 = g_GameFactionRuntimeImage.records[secondFactionIndex].technologyMasks256Bits[7];
  pdVar5 = g_GameFactionRuntimeImage.records[firstFactionIndex].technologyMasks256Bits + 4;
  *pdVar5 = *pdVar5 | g_GameFactionRuntimeImage.records[secondFactionIndex].technologyMasks256Bits
                      [4];
  pdVar5 = g_GameFactionRuntimeImage.records[firstFactionIndex].technologyMasks256Bits + 5;
  *pdVar5 = *pdVar5 | uVar17;
  pdVar5 = g_GameFactionRuntimeImage.records[firstFactionIndex].technologyMasks256Bits + 6;
  *pdVar5 = *pdVar5 | uVar22;
  pdVar5 = g_GameFactionRuntimeImage.records[firstFactionIndex].technologyMasks256Bits + 7;
  *pdVar5 = *pdVar5 | uVar12;
  TVar8 = g_GameFactionRuntimeImage.records[secondFactionIndex].tritiumExtractedTotalQ4;
  iVar24 = g_GameFactionRuntimeImage.records[secondFactionIndex].relationCounterA;
  iVar13 = g_GameFactionRuntimeImage.records[secondFactionIndex].relationCounterB;
  pXVar3 = &g_GameFactionRuntimeImage.records[firstFactionIndex].xeniteExtractedTotalQ4;
  *pXVar3 = *pXVar3 + g_GameFactionRuntimeImage.records[secondFactionIndex].xeniteExtractedTotalQ4;
  pTVar2 = &g_GameFactionRuntimeImage.records[firstFactionIndex].tritiumExtractedTotalQ4;
  *pTVar2 = *pTVar2 + TVar8;
  pFVar6 = &g_GameFactionRuntimeImage.records[firstFactionIndex].relationCounterA;
  *pFVar6 = *pFVar6 + iVar24;
  pFVar6 = &g_GameFactionRuntimeImage.records[firstFactionIndex].relationCounterB;
  *pFVar6 = *pFVar6 + iVar13;
  iVar24 = g_GameFactionRuntimeImage.records[secondFactionIndex].relationCounterD;
  iVar13 = g_GameFactionRuntimeImage.records[secondFactionIndex].relationCounterE;
  iVar14 = g_GameFactionRuntimeImage.records[secondFactionIndex].relationCounterF;
  pFVar6 = &g_GameFactionRuntimeImage.records[firstFactionIndex].relationCounterC;
  *pFVar6 = *pFVar6 + g_GameFactionRuntimeImage.records[secondFactionIndex].relationCounterC;
  pFVar6 = &g_GameFactionRuntimeImage.records[firstFactionIndex].relationCounterD;
  *pFVar6 = *pFVar6 + iVar24;
  pFVar6 = &g_GameFactionRuntimeImage.records[firstFactionIndex].relationCounterE;
  *pFVar6 = *pFVar6 + iVar13;
  pFVar6 = &g_GameFactionRuntimeImage.records[firstFactionIndex].relationCounterF;
  *pFVar6 = *pFVar6 + iVar14;
  FVar21 = g_GameFactionRuntimeImage.records[secondFactionIndex].primaryArmyAssetCount;
  iVar24 = 0;
  for (uVar17 = g_GameFactionRuntimeImage.records[firstFactionIndex].primaryArmyAssetCount;
      (FVar21 != 0 && (uVar17 < 0x40)); uVar17 = uVar17 + 1) {
    primaryArmyAssetReferenceDword = *(dword *)(secondFactionIndex * 0x740 + 0x50f520 + iVar24 * 4);
    pFVar7 = &g_GameFactionRuntimeImage.records[firstFactionIndex].primaryArmyAssetCount;
    *pFVar7 = *pFVar7 + 1;
    *(dword *)(firstFactionIndex * 0x740 + 0x50f520 + uVar17 * 4) = primaryArmyAssetReferenceDword;
    iVar24 = iVar24 + 1;
    FVar21 = FVar21 - 1;
  }
  FVar21 = g_GameFactionRuntimeImage.records[secondFactionIndex].secondaryArmyAssetCount;
  iVar24 = 0;
  for (uVar17 = g_GameFactionRuntimeImage.records[firstFactionIndex].secondaryArmyAssetCount;
      (FVar21 != 0 && (uVar17 < 0x40)); uVar17 = uVar17 + 1) {
    secondaryArmyAssetReferenceDword =
         *(dword *)(secondFactionIndex * 0x740 + 0x50f420 + iVar24 * 4);
    pFVar7 = &g_GameFactionRuntimeImage.records[firstFactionIndex].secondaryArmyAssetCount;
    *pFVar7 = *pFVar7 + 1;
    *(dword *)(firstFactionIndex * 0x740 + 0x50f420 + uVar17 * 4) = secondaryArmyAssetReferenceDword
    ;
    iVar24 = iVar24 + 1;
    FVar21 = FVar21 - 1;
  }
  UiCommandSpriteVariantA_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
  UiCatalogGroup42_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
  UiCatalogGroup48_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
GameFactionRuntime_ApplyPairwiseRelationTransition_RebuildTargetEntriesAndReturn:
  InGameOtherPlayerCommand_RebuildTargetEntries((UiNodeBase *)g_InGameRuntimeRoot);
  return;
}

