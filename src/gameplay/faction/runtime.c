/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/faction/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/faction/runtime.h>
#include <thandor/thandor.h>

/* Implementation ownership: gameplay/faction/runtime. */

/* Address: 0x0055F790.
   Ownership: gameplay/faction/runtime.
   Purpose: Advances the selected faction pair through the verified relation-state transition table, using recent-
   timed-state checks for transitional states. It is distinct from FrontendPlayerIndex_V306, PlayerRuntimeId,
   active-faction masks or codes, and PCK-backed ArmyAssetId, ModelDefinitionId, and TechnologyId domains.
   Local calls: GameFactionRuntime_IsRecentTimedRelationState,
   GameFactionRuntime_ApplyPairwiseRelationTransition.
*/
void __thandor_void_preserve_eax_ecx
GameFactionRuntime_AdvancePairwiseRelationState
          (uint32_t unusedRelationArgument0,uint32_t unusedRelationArgument1,
          FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex)

{
  bool isRecentTimedState;
  
  switch(g_GameFactionRuntimeImage.records[targetFactionIndex].packedRelationStates >>
         ((uint8_t)(sourceFactionIndex << 2) & 0x1f) & 0xf) {
  case 2:
    isRecentTimedState = GameFactionRuntime_IsRecentTimedRelationState(sourceFactionIndex,targetFactionIndex);
    if (isRecentTimedState) {
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
    isRecentTimedState = GameFactionRuntime_IsRecentTimedRelationState(sourceFactionIndex,targetFactionIndex);
    if (isRecentTimedState) {
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
    isRecentTimedState = GameFactionRuntime_IsRecentTimedRelationState(sourceFactionIndex,targetFactionIndex);
    if (isRecentTimedState) {
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
          (uint32_t unusedRelationArgument0,uint32_t unusedRelationArgument1,
          FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex)

{
  switch(g_GameFactionRuntimeImage.records[targetFactionIndex].packedRelationStates >>
         ((uint8_t)(sourceFactionIndex << 2) & 0x1f) & 0xf) {
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
   Mission carry-over after a session ends: finds the current scenario's record in the loaded campaign and,
   for the outcome selected by g_EndMovieSelectionIndex, stores each faction's technology masks (8 dwords) in
   the old-unit secondary table and every unit standing inside its faction's exit zone as a primary record,
   moved by the scenario's per-faction offset. OldUnitRuntime_MergeMasksAndReplayRecords applies both in the
   next mission. Without a matching scenario both tables are cleared.
*/
void __fastcall OldUnitRuntime_RebuildScenarioReplayTables(void)

{
  WorldOwnerListNode100 *ownerNode;
  uint32_t unitFactionOrAssetId;
  uint32_t carryOverMaskBits;
  uint32_t distanceToAnchor;
  int remainingOrWorldY;
  uint32_t skipMaskBits;
  int copyCountOrAnchorY;
  int scenarioRecord;
  int factionFieldCursorOrOffsetY;
  /* The original multiplies a stale caller ESI by the active faction index here; the loop then walks
     all eight 0x740-byte faction records, which only stays inside the table from records[0]. */
  int staleCallerEsi = 0;
  uint32_t *sourceOrRecordCursor;
  uint32_t *secondaryTableCursor;
  bool scenarioFound;

  scenarioFound = false;
  /* Campaign asset: +0xB8 scenario count, +0xC4 current scenario id; scenario records of 0x180 bytes whose
     id is at +0x300 relative to the record pointer. */
  if ((g_InGameRuntimeRoot != NULL) &&
     (sourceOrRecordCursor = (uint32_t *)((int)g_GameFactionRuntimeImage.records[0].technologyMasks256Bits +
                         staleCallerEsi *
                         (g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex),
     g_FrontendLoadedCampaignAsset != 0)) {
    remainingOrWorldY = *(int *)(g_FrontendLoadedCampaignAsset + 0xb8);
    scenarioRecord = g_FrontendLoadedCampaignAsset;
    do {
      if (*(int *)(g_FrontendLoadedCampaignAsset + 0xc4) == *(int *)(scenarioRecord + 0x300)) {
        scenarioFound = true;
        break;
      }
      scenarioRecord = scenarioRecord + 0x180;
      remainingOrWorldY--;
    } while (remainingOrWorldY != 0);
  }
  if (!scenarioFound) {
    OldUnitRuntime_ResetPendingTables();
    return;
  }
  /* Per-faction dword arrays of the scenario record (indexed by faction): +0x260 exit-zone centre X,
     +0x280 centre Y, +0x2A0 radius, +0x2C0 destination X, +0x2E0 destination Y. +0x304 and +0x308 hold
     one carry-over and one skip bit per outcome. */
  factionFieldCursorOrOffsetY = scenarioRecord + 0x200;
  carryOverMaskBits = *(uint32_t *)(scenarioRecord + 0x304) >> ((uint8_t)g_EndMovieSelectionIndex & 0x1f);
  skipMaskBits = *(uint32_t *)(scenarioRecord + 0x308) >> ((uint8_t)g_EndMovieSelectionIndex & 0x1f);
  secondaryTableCursor = g_OldUnitSecondaryTable;
  /* Eight groups, one per faction record. The masks are not shifted per group (as in the original). */
  for (remainingOrWorldY = 8; remainingOrWorldY != 0; remainingOrWorldY--) {
    if (((skipMaskBits & 1) == 0) && ((carryOverMaskBits & 1) != 0) &&
        (((*(int *)(factionFieldCursorOrOffsetY + 0x60) != 0 || (*(int *)(factionFieldCursorOrOffsetY + 0x80) != 0)) ||
          (*(int *)(factionFieldCursorOrOffsetY + 0xc0) != 0)) ||
         ((*(int *)(factionFieldCursorOrOffsetY + 0xe0) != 0 || (*(int *)(factionFieldCursorOrOffsetY + 0xa0) != 0))))) {
      for (copyCountOrAnchorY = 8; copyCountOrAnchorY != 0; copyCountOrAnchorY--) {
        *secondaryTableCursor = *sourceOrRecordCursor;
        sourceOrRecordCursor++;
        secondaryTableCursor++;
      }
    }
    else {
      if ((skipMaskBits & 1) == 0) {
        for (copyCountOrAnchorY = 8; copyCountOrAnchorY != 0; copyCountOrAnchorY--) {
          *secondaryTableCursor = 0;
          secondaryTableCursor++;
        }
      }
      sourceOrRecordCursor = sourceOrRecordCursor + 8;
    }
    /* on to the next faction record's technology masks */
    sourceOrRecordCursor = sourceOrRecordCursor + 0x1c8;
    factionFieldCursorOrOffsetY = factionFieldCursorOrOffsetY + 4;
  }
  /* Primary records (8 dwords each, at most 0x200): [0] army asset id, [1] faction, [2] X, [3] Y,
     [4] rotation angle. */
  if (((g_InGameRuntimeRoot != NULL) &&
      (ownerNode = (g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead, (skipMaskBits & 1) == 0)) &&
     (g_OldUnitRecordCount = 0, sourceOrRecordCursor = g_OldUnitPrimaryTable, (carryOverMaskBits & 1) != 0)) {
    for (; ownerNode != NULL; ownerNode = ownerNode->nextNode) {
      /* runtimePayload + 8: the unit state (+0x0C faction, +0xA0 army asset id). Only a unit with a
         zero dword at runtimePayload + 0xF0 is committed as a record. */
      if (((ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
          (unitFactionOrAssetId = *(uint32_t *)(*(int *)((int)ownerNode->runtimePayload + 8) + 0xc),
          0 < *(int *)(scenarioRecord + 0x2a0 + unitFactionOrAssetId * 4))) &&
         (distanceToAnchor = FixedMath_Length2(*(int *)(scenarioRecord + 0x280 + unitFactionOrAssetId * 4) - ownerNode->worldYQ12,
                                    *(int *)(scenarioRecord + 0x260 + unitFactionOrAssetId * 4) - ownerNode->worldXQ12),
         (int)distanceToAnchor <= *(int *)(scenarioRecord + 0x2a0 + unitFactionOrAssetId * 4))) {
        remainingOrWorldY = ownerNode->worldYQ12;
        factionFieldCursorOrOffsetY = *(int *)(scenarioRecord + 0x2e0 + unitFactionOrAssetId * 4);
        copyCountOrAnchorY = *(int *)(scenarioRecord + 0x280 + unitFactionOrAssetId * 4);
        sourceOrRecordCursor[2] = (ownerNode->worldXQ12 + *(int *)(scenarioRecord + 0x2c0 + unitFactionOrAssetId * 4)) -
                     *(int *)(scenarioRecord + 0x260 + unitFactionOrAssetId * 4);
        sourceOrRecordCursor[3] = (remainingOrWorldY + factionFieldCursorOrOffsetY) - copyCountOrAnchorY;
        sourceOrRecordCursor[1] = unitFactionOrAssetId;
        if (*(int *)((int)ownerNode->runtimePayload + 0xf0) == 0) {
          unitFactionOrAssetId = *(uint32_t *)(*(int *)((int)ownerNode->runtimePayload + 8) + 0xa0);
          sourceOrRecordCursor[4] = ownerNode->modelLocalRotationAngle2;
          *sourceOrRecordCursor = unitFactionOrAssetId;
          g_OldUnitRecordCount++;
          sourceOrRecordCursor = sourceOrRecordCursor + 8;
          if (0x1ff < g_OldUnitRecordCount) {
            return;
          }
        }
      }
    }
  }
  return;
}


/* Address: 0x005130B0.
   Ownership: gameplay/faction/runtime.
   Purpose: Resolves the two serialized army-asset identifier arrays in each faction record through the army
   registry, clears an array count on lookup failure, and rebases the 256 stored runtime pointers with the verified
   army-runtime rebase delta.
   Cross-module calls: ArmyAssetRegistry_FindById [assets/army/catalog].
*/
void __fastcall GameFactionRuntime_RebaseLoadedArmyReferences(void)

{
  ArmyRuntimeSlot *rebasedSlot;
  int factionsRemaining;
  FactionArmyAssetCount assetsRemaining;
  int groupSlotsRemaining;
  GameFactionRuntimeImage *factionRecordCursor;
  ArmyAssetRecordPrefix **unusedAssetCursorA;
  uint32_t *assetIdCursor;
  ArmyAssetRecordPrefix **unusedAssetCursorB;
  ArmyRuntimeSlot **groupSlotCursor;
  ArmyAssetLookupResult resolvedAsset;
  
  factionRecordCursor = &g_GameFactionRuntimeImage;
  factionsRemaining = 8;
  do {
    assetIdCursor = factionRecordCursor->records[0].secondaryArmyAssetPointersOrIds;
    for (assetsRemaining = factionRecordCursor->records[0].secondaryArmyAssetCount; assetsRemaining != 0; assetsRemaining = assetsRemaining - 1) {
      resolvedAsset = ArmyAssetRegistry_FindById(*assetIdCursor);
      if (resolvedAsset.notFound) {
        factionRecordCursor->records[0].secondaryArmyAssetCount = 0;
        break;
      }
      *assetIdCursor = (uint32_t)resolvedAsset.recordOrError;
      assetIdCursor = assetIdCursor + 1;
    }
    assetIdCursor = factionRecordCursor->records[0].primaryArmyAssetPointersOrIds;
    for (assetsRemaining = factionRecordCursor->records[0].primaryArmyAssetCount; assetsRemaining != 0; assetsRemaining = assetsRemaining - 1) {
      resolvedAsset = ArmyAssetRegistry_FindById(*assetIdCursor);
      if (resolvedAsset.notFound) {
        factionRecordCursor->records[0].primaryArmyAssetCount = 0;
        break;
      }
      *assetIdCursor = (uint32_t)resolvedAsset.recordOrError;
      assetIdCursor = assetIdCursor + 1;
    }
    groupSlotCursor = factionRecordCursor->records[0].runtimeGroupMembers8x32;
    groupSlotsRemaining = 0x100;
    do {
      rebasedSlot = (ArmyRuntimeSlot *)0x0;
      if (*groupSlotCursor != (ArmyRuntimeSlot *)0x0) {
        rebasedSlot = (ArmyRuntimeSlot *)
                    ((int)&(*groupSlotCursor)->modelRuntimeOrSavedOffset +
                    (int)g_ArmyRuntimeRebaseBaseMinusOne);
      }
      *groupSlotCursor = rebasedSlot;
      groupSlotCursor = groupSlotCursor + 1;
      groupSlotsRemaining = groupSlotsRemaining + -1;
    } while (groupSlotsRemaining != 0);
    factionRecordCursor = (GameFactionRuntimeImage *)(factionRecordCursor->records + 1);
    factionsRemaining = factionsRemaining + -1;
    if (factionsRemaining == 0) {
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
GameFactionRuntime_TestCapabilityBitClear
          (uint32_t capabilityBitIndex,FactionRuntimeIndex factionIndex)

{
  return (g_GameFactionRuntimeImage.records[factionIndex].capabilityFlags &
         1 << ((uint8_t)capabilityBitIndex & 0x1f)) == 0;
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
         ((uint8_t)(otherFactionIndex << 2) & 0x1f) & 0xf;
}


/* Address: 0x00513D70.
   Technology exchange between related factions: for every unordered pair of factions 1..7 whose relation
   state is 8, 9 or 10, the lowest technology only the source faction has and the lowest technology only the
   other faction has are swapped (each side unlocks the other's). A pair where either side has nothing the
   other lacks exchanges nothing. Afterwards the other-player command entries are rebuilt.
*/
void __thandor_void_preserve_eax_ecx_edx
GameFactionRuntime_SynchronizeTechnologiesForRelationStates8To10(void)

{
  int otherRecordBase;
  int carryBit;
  uint32_t bitMask;
  uint32_t maskWordIndex;
  TechnologyId otherTechnologyIndex;
  uint32_t otherFactionIndex;
  uint32_t sourceFactionIndex;
  TechnologyId sourceTechnologyIndex;
  int sourceRecordBase;
  
  sourceRecordBase = THANDOR_ADDR(g_GameFactionRuntimeImage,0x740);
  for (sourceFactionIndex = 1; sourceFactionIndex < 7; sourceFactionIndex++) {
    /* otherRecordBase trails the other faction's record by one record (0x740 bytes), so +0x780 is the
       other record's packedRelationStates (+0x40) and +0xE20 its technologyMasks256Bits (+0x6E0). */
    otherRecordBase = sourceRecordBase;
    for (otherFactionIndex = sourceFactionIndex + 1; otherFactionIndex < 8;
        otherFactionIndex++) {
      /* the other faction's relation-state nibble towards the source faction */
      switch(*(uint32_t *)(otherRecordBase + 0x780) >> ((char)sourceFactionIndex * 4 & 0x1fU) & 0xf) {
      case 8:
      case 9:
      case 10:
        /* First technology the source faction has and the other one lacks (bitMask rotates left, the carry
           advances maskWordIndex). */
        bitMask = 1;
        maskWordIndex = 0;
        sourceTechnologyIndex = 0;
        do {
          if (((*(uint32_t *)(sourceRecordBase + 0x6e0 + maskWordIndex * 4) & bitMask) != 0) &&
             ((*(uint32_t *)(otherRecordBase + 0xe20 + maskWordIndex * 4) & bitMask) == 0)) break;
          carryBit = (int)bitMask >> 0x1f;
          bitMask = bitMask << 1 | -carryBit;
          maskWordIndex = maskWordIndex + (-carryBit != 0);
          sourceTechnologyIndex++;
        } while (maskWordIndex < 8);
        if (maskWordIndex < 8) {
          /* First technology the other faction has and the source one lacks; swap the pair. */
          maskWordIndex = 0;
          bitMask = 1;
          otherTechnologyIndex = 0;
          do {
            if (((*(uint32_t *)(otherRecordBase + 0xe20 + maskWordIndex * 4) & bitMask) != 0) &&
               ((*(uint32_t *)(sourceRecordBase + 0x6e0 + maskWordIndex * 4) & bitMask) == 0)) {
              Technology_UnlockForFaction(0,0,otherTechnologyIndex,sourceFactionIndex);
              Technology_UnlockForFaction(0,0,sourceTechnologyIndex,otherFactionIndex);
              break;
            }
            carryBit = (int)bitMask >> 0x1f;
            bitMask = bitMask << 1 | -carryBit;
            maskWordIndex = maskWordIndex + (-carryBit != 0);
            otherTechnologyIndex++;
          } while (maskWordIndex < 8);
        }
      }
      otherRecordBase = otherRecordBase + 0x740;
    }
    sourceRecordBase = sourceRecordBase + 0x740;
  }
  InGameOtherPlayerCommand_RebuildTargetEntries((UiNodeBase *)g_InGameRuntimeRoot);
  return;
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
  int *ownerDefinitionRecord;
  int ownerFactionIndex;
  int linkedModelAddress;
  FactionAnchorCooldownTicks previousCooldown;
  GraphicsWorldCoordinateQ12 anchorX;
  int distanceYOrLinkRecord;
  int distanceX;
  
  ownerDefinitionRecord = (targetArmyRuntime->linkedEntityRuntime->common).ownership.definitionOrClassRecord;
  ownerFactionIndex = (targetArmyRuntime->linkedEntityRuntime->common).ownership.ownerIndex;
  linkedModelAddress = ownerDefinitionRecord[1];
  if (*(int *)(*ownerDefinitionRecord + 0x18) == 0) {
    previousCooldown = g_GameFactionRuntimeImage.records[ownerFactionIndex].primaryAnchorCooldown;
    distanceYOrLinkRecord = g_GameFactionRuntimeImage.records[ownerFactionIndex].primaryAnchorYQ12 - *(int *)(linkedModelAddress + 0x94);
    if (distanceYOrLinkRecord < 0) {
      distanceYOrLinkRecord = -distanceYOrLinkRecord;
    }
    distanceX = g_GameFactionRuntimeImage.records[ownerFactionIndex].primaryAnchorXQ12 - *(int *)(linkedModelAddress + 0x98);
    if (distanceX < 0) {
      distanceX = -distanceX;
    }
    g_GameFactionRuntimeImage.records[ownerFactionIndex].primaryAnchorCooldown = 0x96;
    if (((int)previousCooldown < 0x32) && ((0xc000 < distanceYOrLinkRecord + distanceX || (previousCooldown == 0)))) {
      anchorX = *(GraphicsWorldCoordinateQ12 *)(linkedModelAddress + 0x98);
      distanceYOrLinkRecord = ownerDefinitionRecord[2];
      g_GameFactionRuntimeImage.records[ownerFactionIndex].primaryAnchorYQ12 =
           *(GraphicsWorldCoordinateQ12 *)(linkedModelAddress + 0x94);
      g_GameFactionRuntimeImage.records[ownerFactionIndex].primaryAnchorXQ12 = anchorX;
      if (worldRuntime->activeFactionRuntimeIndex == *(int *)(distanceYOrLinkRecord + 0xc)) {
        InGameNotificationQueue_InsertPriorityRecord
                  (FACTION_IMPACT_ANCHOR,0,(worldRuntime->motion).pitchAngle,
                   (worldRuntime->motion).headingAngle,*(Q12 *)(ownerDefinitionRecord[1] + 0x98),
                   *(Q12 *)(ownerDefinitionRecord[1] + 0x94),8,300);
      }
    }
    else {
      anchorX = *(GraphicsWorldCoordinateQ12 *)(linkedModelAddress + 0x98);
      g_GameFactionRuntimeImage.records[ownerFactionIndex].primaryAnchorYQ12 =
           *(GraphicsWorldCoordinateQ12 *)(linkedModelAddress + 0x94);
      g_GameFactionRuntimeImage.records[ownerFactionIndex].primaryAnchorXQ12 = anchorX;
    }
  }
  else {
    previousCooldown = g_GameFactionRuntimeImage.records[ownerFactionIndex].anchorCooldown0;
    distanceYOrLinkRecord = g_GameFactionRuntimeImage.records[ownerFactionIndex].secondaryAnchorYQ12 - *(int *)(linkedModelAddress + 0x94);
    if (distanceYOrLinkRecord < 0) {
      distanceYOrLinkRecord = -distanceYOrLinkRecord;
    }
    distanceX = g_GameFactionRuntimeImage.records[ownerFactionIndex].secondaryAnchorXQ12 - *(int *)(linkedModelAddress + 0x98);
    if (distanceX < 0) {
      distanceX = -distanceX;
    }
    g_GameFactionRuntimeImage.records[ownerFactionIndex].anchorCooldown0 = 0x96;
    if (((int)previousCooldown < 0x32) && ((0xc000 < distanceYOrLinkRecord + distanceX || (previousCooldown == 0)))) {
      anchorX = *(GraphicsWorldCoordinateQ12 *)(linkedModelAddress + 0x98);
      distanceYOrLinkRecord = ownerDefinitionRecord[2];
      g_GameFactionRuntimeImage.records[ownerFactionIndex].secondaryAnchorYQ12 =
           *(GraphicsWorldCoordinateQ12 *)(linkedModelAddress + 0x94);
      g_GameFactionRuntimeImage.records[ownerFactionIndex].secondaryAnchorXQ12 = anchorX;
      if (worldRuntime->activeFactionRuntimeIndex == *(int *)(distanceYOrLinkRecord + 0xc)) {
        InGameNotificationQueue_InsertPriorityRecord
                  (FACTION_IMPACT_ANCHOR,0,(worldRuntime->motion).pitchAngle,
                   (worldRuntime->motion).headingAngle,*(Q12 *)(ownerDefinitionRecord[1] + 0x98),
                   *(Q12 *)(ownerDefinitionRecord[1] + 0x94),7,0x12d);
      }
    }
    else {
      anchorX = *(GraphicsWorldCoordinateQ12 *)(linkedModelAddress + 0x98);
      g_GameFactionRuntimeImage.records[ownerFactionIndex].secondaryAnchorYQ12 =
           *(GraphicsWorldCoordinateQ12 *)(linkedModelAddress + 0x94);
      g_GameFactionRuntimeImage.records[ownerFactionIndex].secondaryAnchorXQ12 = anchorX;
    }
  }
  return;
}


/* Address: 0x00514730.
   Recomputes one faction's statistics for the score / results screens: explored terrain percent, unlocked
   technologies beyond the five starting ones, extracted resource components, the economy and relation scores,
   the summed value of its army assets on the map, and the combined progress score.
*/
void __thandor_void_preserve_eax_ecx_edx
GameFactionRuntime_RecomputeProgressAndScoreMetrics
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  TerrainExploredPercent exploredPercent;
  ResourceExtractionRateQ4PerTick xeniteRate;
  int tallyOrComponent;
  uint32_t cellOrTechnologyCount;
  FactionTechnologyCount technologyCountBeyondBaseline;
  uint32_t cellsLeftOrBitOrRate;
  int tritiumComponentOrModelRecord;
  uint8_t *cellVisibilityCursor;
  uint32_t maskWordIndex;
  ArmyAssetLookupResult resolvedAsset;
  FieldGridAsset *terrainGrid;
  WorldOwnerListNode100 *ownerNode;
  
  terrainGrid = worldRuntime->fieldGrid;
  cellOrTechnologyCount = terrainGrid->gridWidth * terrainGrid->gridHeight;
  tallyOrComponent = 0;
  /* one byte per faction at cell +0x70; bits 3-7 set = the faction has explored the cell (0x80-byte cells) */
  cellVisibilityCursor = terrainGrid->cells[0].runtime60_6B + factionIndex + 0x10;
  cellsLeftOrBitOrRate = cellOrTechnologyCount;
  do {
    if ((*cellVisibilityCursor & 0xf8) != 0) {
      tallyOrComponent++;
    }
    cellVisibilityCursor = cellVisibilityCursor + 0x80;
    cellsLeftOrBitOrRate = cellsLeftOrBitOrRate - 1;
  } while (cellsLeftOrBitOrRate != 0);
  g_GameFactionRuntimeImage.records[factionIndex].exploredTerrainPercent =
       (uint32_t)(tallyOrComponent * 100) / cellOrTechnologyCount;
  /* count the set bits of the 256-bit technology mask at faction record +0x6E0 (technologyMasks256Bits) */
  cellsLeftOrBitOrRate = 1;
  cellOrTechnologyCount = 0;
  maskWordIndex = 0;
  do {
    do {
      if ((*(uint32_t *)(factionIndex * 0x740 + THANDOR_ADDR(g_GameFactionRuntimeImage,0x6e0) + maskWordIndex * 4) & cellsLeftOrBitOrRate) != 0) {
        cellOrTechnologyCount++;
      }
      cellsLeftOrBitOrRate = cellsLeftOrBitOrRate * 2;
    } while (cellsLeftOrBitOrRate != 0);
    maskWordIndex++;
    cellsLeftOrBitOrRate = 1;
  } while (maskWordIndex < 8);
  technologyCountBeyondBaseline = cellOrTechnologyCount - 5;
  if (cellOrTechnologyCount < 5) {
    technologyCountBeyondBaseline = 0;
  }
  exploredPercent = g_GameFactionRuntimeImage.records[factionIndex].exploredTerrainPercent;
  tallyOrComponent = (int)g_GameFactionRuntimeImage.records[factionIndex].xeniteExtractedTotalQ4 >> 4;
  tritiumComponentOrModelRecord = (int)g_GameFactionRuntimeImage.records[factionIndex].tritiumExtractedTotalQ4 >> 4;
  g_GameFactionRuntimeImage.records[factionIndex].unlockedTechnologyCountBeyondBaseline = technologyCountBeyondBaseline;
  g_GameFactionRuntimeImage.records[factionIndex].primaryResourceComponent = tallyOrComponent;
  g_GameFactionRuntimeImage.records[factionIndex].secondaryResourceComponent = tritiumComponentOrModelRecord;
  g_GameFactionRuntimeImage.records[factionIndex].economyProgressScore =
       (int)(tallyOrComponent * 0x10 + tritiumComponentOrModelRecord * 8 + technologyCountBeyondBaseline * 0xa000 + exploredPercent * 0x1000) >> 0xc;
  g_GameFactionRuntimeImage.records[factionIndex].relationScore =
       (g_GameFactionRuntimeImage.records[factionIndex].relationCounterB * 0x28000 +
       g_GameFactionRuntimeImage.records[factionIndex].relationCounterD * -0x14000 +
       g_GameFactionRuntimeImage.records[factionIndex].relationCounterA * 0x14000 +
       g_GameFactionRuntimeImage.records[factionIndex].relationCounterC * -0xa000 +
       g_GameFactionRuntimeImage.records[factionIndex].relationCounterE * 0x14000 +
       g_GameFactionRuntimeImage.records[factionIndex].relationCounterF * 0x28000) >> 0xc;
  /* sum the army-asset dword +0x28 over every model of this faction on the map */
  tallyOrComponent = 0;
  for (ownerNode = worldRuntime->ownerListHead; ownerNode != NULL;
      ownerNode = ownerNode->nextNode) {
    if ((ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
       (tritiumComponentOrModelRecord = *(int *)((int)ownerNode->runtimePayload + 8), factionIndex == *(int *)(tritiumComponentOrModelRecord + 0xc)
       )) {
      resolvedAsset = ArmyAssetRegistry_FindById(*(PckArmyAssetIdCatalog *)(tritiumComponentOrModelRecord + 0xa0));
      if (!resolvedAsset.notFound) {
        tallyOrComponent = tallyOrComponent + resolvedAsset.recordOrError[2].registryId;
      }
    }
  }
  cellsLeftOrBitOrRate = g_GameFactionRuntimeImage.records[factionIndex].tritiumExtractionRateQ4PerTick;
  xeniteRate = g_GameFactionRuntimeImage.records[factionIndex].xeniteExtractionRateQ4PerTick;
  exploredPercent = g_GameFactionRuntimeImage.records[factionIndex].exploredTerrainPercent;
  technologyCountBeyondBaseline = g_GameFactionRuntimeImage.records[factionIndex].unlockedTechnologyCountBeyondBaseline;
  g_GameFactionRuntimeImage.records[factionIndex].activeArmyContribution = tallyOrComponent;
  g_GameFactionRuntimeImage.records[factionIndex].combinedProgressScore =
       ((cellsLeftOrBitOrRate >> 1) + xeniteRate) * exploredPercent * technologyCountBeyondBaseline >> 3;
  return;
}


/* Address: 0x00514900.
   Ownership: gameplay/faction/runtime.
   Purpose: Scans the selected faction's eight fixed runtime groups, each containing up to 32 pointers, and returns
   the one-based group index with carry clear or the original runtime pointer with carry set.
*/
RuntimeGroupIndexResult __thandor_eax_cf_preserve_ecx_edx
GameFactionRuntime_FindRuntimeGroupIndex(RuntimeModelFactionPrefix10 *runtimeEntry)

{
  int slotsRemaining;
  uint32_t groupNumber;
  ArmyRuntimeSlot **slotCursor;
  ArmyRuntimeSlot **nextSlotCursor;
  bool found;
  RuntimeGroupIndexResult notFoundResult;
  RuntimeGroupIndexResult foundResult;
  
  groupNumber = 0;
  nextSlotCursor = g_GameFactionRuntimeImage.records[runtimeEntry->factionIndex].runtimeGroupMembers8x32;
  do {
    slotsRemaining = 0x20;
    groupNumber = groupNumber + 1;
    found = groupNumber == 0;
    slotCursor = nextSlotCursor;
    do {
      nextSlotCursor = slotCursor;
      if (slotsRemaining == 0) break;
      slotsRemaining = slotsRemaining + -1;
      nextSlotCursor = slotCursor + 1;
      found = (ArmyRuntimeSlot *)runtimeEntry == *slotCursor;
      slotCursor = nextSlotCursor;
    } while (!found);
    if (found) {
      foundResult.notFound = false;
      foundResult.runtimeGroupIndex = groupNumber;
      return foundResult;
    }
    if (7 < groupNumber) {
      notFoundResult.notFound = true;
      notFoundResult.runtimeGroupIndex = (uint32_t)runtimeEntry;
      return notFoundResult;
    }
  } while( true );
}


/* Address: 0x0051B800.
   Ownership: gameplay/faction/runtime.
   Purpose: Tests the faction army-asset identifier list and then active class-0x0B or class-0x0D structures for a
   matching identifier, returning the result through carry.
*/
bool __thandor_cf_preserve_eax_ecx_edx
FactionRuntime_HasArmyAssetOrActiveStructure
          (FactionRuntimeIndex factionIndex,ArmyAssetRecordPrefix *armyAssetRecord)

{
  ArmyAssetRecordPrefix *activeAssetRecord;
  WorldOwnerListNode100 *ownerNode;
  int *modelPayload;
  FactionArmyAssetCount assetsRemaining;
  uint32_t *assetIdCursor;
  bool matched;
  
  assetsRemaining = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount;
  assetIdCursor = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetPointersOrIds;
  matched = assetsRemaining == 0;
  if (!matched) {
    do {
      if (assetsRemaining == 0) break;
      assetsRemaining = assetsRemaining - 1;
      matched = armyAssetRecord == (ArmyAssetRecordPrefix *)*assetIdCursor;
      assetIdCursor = assetIdCursor + 1;
    } while (!matched);
    if (matched) {
      return false;
    }
  }
  /* Structures of class 0xB or 0xD in state 1 that are currently producing armyAssetRecord. */
  for (ownerNode = (g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
      ownerNode != (WorldOwnerListNode100 *)0x0; ownerNode = ownerNode->nextNode) {
    if ((ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
       (modelPayload = ownerNode->runtimePayload, factionIndex == *(int *)(modelPayload[2] + 0xc)) &&
       ((*(int *)(*modelPayload + 0x4c) == 0xb) || (*(int *)(*modelPayload + 0x4c) == 0xd)) &&
       (modelPayload[0x2e] == 1)) {
      activeAssetRecord = (ArmyAssetRecordPrefix *)modelPayload[0x18];
      if (armyAssetRecord == activeAssetRecord) {
        return false;
      }
    }
  }
  return true;
}


/* Address: 0x0051C4C0.
   Ownership: gameplay/faction/runtime.
   Purpose: Handles game entity runtime reset movement flags and anchor coordinates from model.
*/
void __thandor_void_preserve_eax_ecx
GameEntityRuntime_ResetMovementFlagsAndAnchorCoordinatesFromModel(GameEntityRuntime *entityRuntime)

{
  GameEntityCommandFlags *commandFlagsField;
  ModelRuntimeNode *ownerModelNode;
  GraphicsWorldCoordinateQ12 modelX;
  GraphicsWorldCoordinateQ12 modelY;
  
  ownerModelNode = (entityRuntime->common).ownership.modelNode;
  commandFlagsField = &(entityRuntime->common).commandFlags;
  *commandFlagsField = *commandFlagsField & 0xffffffc6;
  modelX = (ownerModelNode->worldTransform).translation.x;
  modelY = (ownerModelNode->worldTransform).translation.y;
  (entityRuntime->common).pathCoordinate0Q12 = modelX;
  (entityRuntime->common).pathCoordinate1Q12 = modelY;
  (entityRuntime->common).trackedCoordinate0Q12 = modelX;
  (entityRuntime->common).trackedCoordinate1Q12 = modelY;
  (entityRuntime->common).damageState.trackedCoordinate0Q12 = modelX;
  (entityRuntime->common).damageState.trackedCoordinate1Q12 = modelY;
  return;
}


/* Address: 0x0051C680.
   Ownership: gameplay/faction/runtime.
   Purpose: Resolves the active command target to EAX/ECX/EDX Q12 coordinates and reports failure through CF. The
   ordinary return remains void because the three-register result is not a C scalar return.
*/
WorldPositionResult
GameEntityRuntime_ResolveCommandTargetPosition(GameEntityRuntime *targetState)

{
  GameEntityRuntime *commandTargetEntity;
  uint32_t visibilityMask;
  int *targetDefinitionRecord;
  ModelRuntimeNode *targetModelNode;
  WorldPositionResult position;

  /* On failure (CF set) the original leaves whatever is in EAX/ECX/EDX at that point (the caller's values or the
     partial visibility mask / owner shift / definition pointer). Both callers ignore the coordinates when CF is
     set, so the failure result carries zeros. */
  position.worldXQ12 = 0;
  position.worldYQ12 = 0;
  position.worldZQ12 = 0;
  position.unresolved = true;
  if (((targetState->common).commandTarget.targetFlags & 1) == 0) {
    if (((targetState->common).commandTarget.targetFlags & 2) != 0) {
      position.worldXQ12 = (targetState->common).commandTarget.targetWorldXQ12;
      position.worldYQ12 = (targetState->common).commandTarget.targetWorldYQ12;
      position.worldZQ12 = (targetState->common).commandTarget.targetWorldZQ12;
      position.unresolved = false;
    }
  }
  else {
    commandTargetEntity = (targetState->common).commandTarget.targetEntity;
    if (commandTargetEntity != (GameEntityRuntime *)0x0) {
      visibilityMask = 2u << ((uint8_t)((targetState->common).ownership.ownerIndex * 2) & 0x1f);
      targetDefinitionRecord = (commandTargetEntity->common).ownership.definitionOrClassRecord;
      if ((*(uint32_t *)((commandTargetEntity->common).damageState.reserved0C_23 + 0x10) & visibilityMask) != 0) {
        targetModelNode = (commandTargetEntity->common).ownership.modelNode;
        if (*(int *)(*targetDefinitionRecord + 0x4c) == 0x15) {
          targetModelNode = targetModelNode->childNodes[0];
        }
        position.worldXQ12 = (targetModelNode->worldTransform).translation.x;
        position.worldYQ12 = (targetModelNode->worldTransform).translation.y;
        position.worldZQ12 =
             (targetModelNode->worldTransform).translation.z + *(int *)(*targetDefinitionRecord + 0x50);
        position.unresolved = false;
        return position;
      }
      (targetState->common).commandTarget.targetEntity = (GameEntityRuntime *)0x0;
      (targetState->common).commandTarget.targetFlags = 0;
    }
  }
  return position;
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
  int *integrityField;
  GameEntityRuntimeFlags *runtimeFlagsField;
  FactionRelationCounter *relationCounter;
  ArmyRuntimeSlot *armyRuntime;
  void *definitionRecord;
  int maxIntegrityOrClassOrCount;
  int integrityDeltaOrFaction;
  ModelRuntimeNode *modelOrParentNode;
  
  if (impactValue < 0) {
    targetEntityRuntime = *(GameEntityRuntime **)(targetEntityRuntime->common).ownership.runtimeLink
    ;
  }
  (targetEntityRuntime->common).pathingAndImpactState.impactReaction.state08 = 0;
  (targetEntityRuntime->common).pathingAndImpactState.impactReaction.reactionCode09 = 2;
  (targetEntityRuntime->common).pathingAndImpactState.impactReaction.state0A = 0;
  (targetEntityRuntime->common).pathingAndImpactState.impactReaction.state0B = 0;
  if (0 < (targetEntityRuntime->common).damageState.remainingIntegrity) {
    maxIntegrityOrClassOrCount = *(int *)((int)(targetEntityRuntime->common).ownership.definitionOrClassRecord + 0x60);
    integrityField = &(targetEntityRuntime->common).damageState.remainingIntegrity;
    integrityDeltaOrFaction = *integrityField;
    *integrityField = *integrityField - impactValue;
    if (*integrityField == 0 || SBORROW4(integrityDeltaOrFaction,impactValue) != *integrityField < 0) {
      modelOrParentNode = (targetEntityRuntime->common).ownership.modelNode;
      integrityDeltaOrFaction = (targetEntityRuntime->common).damageState.remainingIntegrity;
      runtimeFlagsField = &(targetEntityRuntime->common).runtimeFlags;
      *runtimeFlagsField = *runtimeFlagsField | 8;
      modelOrParentNode = modelOrParentNode->parentNode;
      (targetEntityRuntime->common).damageState.counterOrTerminalReference.terminalEntity =
           targetEntityRuntime;
      (targetEntityRuntime->common).damageState.remainingIntegrity = 0;
      if (modelOrParentNode == (ModelRuntimeNode *)0x0) {
        definitionRecord = (targetEntityRuntime->common).ownership.definitionOrClassRecord;
        if ((*(int *)((int)definitionRecord + 0x4c) == 0) && (*(int *)((int)definitionRecord + 0x278) == 0)) {
          (((targetEntityRuntime->common).ownership.modelNode)->modelPayload).worldRotationAngle0 =
               impactAngle;
        }
        if (impactValue != 0) {
          integrityDeltaOrFaction = *(int *)((int)(targetEntityRuntime->common).ownership.runtimeLink + 0xc);
          maxIntegrityOrClassOrCount = *(int *)((int)(targetEntityRuntime->common).ownership.definitionOrClassRecord +
                          0x4c);
          relationCounter = &g_GameFactionRuntimeImage.records[integrityDeltaOrFaction].relationCounterC;
          *relationCounter = *relationCounter + 1;
          relationCounter = &g_GameFactionRuntimeImage.records[sourceFactionIndex].relationCounterE;
          *relationCounter = *relationCounter + 1;
          if (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand[maxIntegrityOrClassOrCount] ==
              ArmyRuntime_ClassCommandHandlerGroupA) {
            relationCounter = &g_GameFactionRuntimeImage.records[integrityDeltaOrFaction].relationCounterD;
            *relationCounter = *relationCounter + 1;
            relationCounter = &g_GameFactionRuntimeImage.records[sourceFactionIndex].relationCounterF;
            *relationCounter = *relationCounter + 1;
            relationCounter = &g_GameFactionRuntimeImage.records[integrityDeltaOrFaction].relationCounterC;
            *relationCounter = *relationCounter + -1;
            relationCounter = &g_GameFactionRuntimeImage.records[sourceFactionIndex].relationCounterE;
            *relationCounter = *relationCounter + -1;
          }
        }
      }
      else {
        ArmyRuntime_ApplyDamageAndPropagateToParent(-integrityDeltaOrFaction,(modelOrParentNode->runtimePayload).armyRuntime)
        ;
      }
    }
    else {
      integrityField = &(targetEntityRuntime->common).damageState.remainingIntegrity;
      integrityDeltaOrFaction = maxIntegrityOrClassOrCount - *integrityField;
      if (integrityDeltaOrFaction == 0 || maxIntegrityOrClassOrCount < *integrityField) {
        integrityField = &(targetEntityRuntime->common).damageState.remainingIntegrity;
        *integrityField = *integrityField + integrityDeltaOrFaction;
        armyRuntime = (targetEntityRuntime->classPayload).impactOwnerLinks.primaryImpactArmyRuntime;
        maxIntegrityOrClassOrCount = (targetEntityRuntime->common).ownership.ownerIndex;
        for (; maxIntegrityOrClassOrCount != 0; maxIntegrityOrClassOrCount = maxIntegrityOrClassOrCount + -1) {
          if ((armyRuntime != (ArmyRuntimeSlot *)0x0) &&
             ((ModelRuntimeSlot *)armyRuntime->actionVector2Q12 !=
              (((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->classLinkState).
              modelLinkOrState60.modelRuntime)) {
            ArmyRuntime_ApplyDamageAndPropagateToParent(integrityDeltaOrFaction,armyRuntime);
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
   Cross-module calls: ArmyAssetRegistry_FindById [assets/army/catalog].
*/
void __thandor_void_preserve_eax_ecx_edx
GameFactionRuntime_RegisterArmyAssetPointers
          (uint32_t reservedDword0,FactionArmyAssetCount repetitionCount,
          PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex)

{
  FactionArmyAssetCount *secondaryCount;
  FactionArmyAssetCount slotIndex;
  ArmyAssetLookupResult resolvedAsset;
  
  resolvedAsset = ArmyAssetRegistry_FindById(armyAssetId);
  if (!resolvedAsset.notFound) {
    slotIndex = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount;
    do {
      if (0x3f < slotIndex) {
        return;
      }
      *(ArmyAssetRecordPrefix **)(factionIndex * 0x740 + THANDOR_ADDR(g_GameFactionRuntimeImage,0xe0) + slotIndex * 4) = resolvedAsset.recordOrError;
      secondaryCount = &g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount;
      *secondaryCount = *secondaryCount + 1;
      slotIndex = slotIndex + 1;
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
   Cross-module calls: ArmyAssetRegistry_FindById [assets/army/catalog].
*/
void __thandor_void_preserve_eax_ecx_edx
GameFactionRuntime_CancelQueuedArmyAssetsAndRefund
          (uint32_t reservedDword0,FactionArmyAssetCount requestedCount,
          PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex)

{
  FactionArmyAssetCount *secondaryCount;
  int *modelPayload;
  PckArmyAssetIdCatalog refundAmount;
  ArmyAssetRecordPrefix *armyDefinition;
  FactionArmyAssetCount assetsRemaining;
  int recordOffset;
  int writeIndex;
  int readIndexOrClassId;
  WorldOwnerListNode100 *ownerNode;
  ArmyAssetLookupResult resolvedAsset;
  
  resolvedAsset = ArmyAssetRegistry_FindById(armyAssetId);
  armyDefinition = resolvedAsset.recordOrError;
  if (!resolvedAsset.notFound) {
    recordOffset = factionIndex * 0x740;
    readIndexOrClassId = 0;
    writeIndex = 0;
    for (assetsRemaining = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount; assetsRemaining != 0
        ; assetsRemaining = assetsRemaining - 1) {
      while ((armyDefinition == *(ArmyAssetRecordPrefix **)(recordOffset + THANDOR_ADDR(g_GameFactionRuntimeImage,0xe0) + readIndexOrClassId * 4) &&
             (0 < (int)requestedCount))) {
        readIndexOrClassId = readIndexOrClassId + 1;
        secondaryCount = &g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount;
        *secondaryCount = *secondaryCount - 1;
        requestedCount = requestedCount - 1;
        assetsRemaining = assetsRemaining - 1;
        if (assetsRemaining == 0)
        goto 
        GameFactionRuntime_CancelQueuedArmyAssetsAndRefund_ContinueWithActiveRuntimeCancellation;
      }
      *(uint32_t *)(recordOffset + THANDOR_ADDR(g_GameFactionRuntimeImage,0xe0) + writeIndex * 4) = *(uint32_t *)(recordOffset + THANDOR_ADDR(g_GameFactionRuntimeImage,0xe0) + readIndexOrClassId * 4);
      readIndexOrClassId = readIndexOrClassId + 1;
      writeIndex = writeIndex + 1;
    }
GameFactionRuntime_CancelQueuedArmyAssetsAndRefund_ContinueWithActiveRuntimeCancellation:
    if (requestedCount != 0) {
      ownerNode = (g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
      if (ownerNode != (WorldOwnerListNode100 *)0x0) {
        readIndexOrClassId = 0xb;
        if (((armyDefinition[1].selectionDetailTemplateVariantIndex & 0x10) == 0) &&
           (readIndexOrClassId = 0x16, (armyDefinition[1].selectionDetailTemplateVariantIndex & 8) == 0)) {
          readIndexOrClassId = 0xd;
        }
        do {
          if (ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
            modelPayload = ownerNode->runtimePayload;
            if ((factionIndex == *(int *)(modelPayload[2] + 0xc)) && (readIndexOrClassId == *(int *)(*modelPayload + 0x4c)))
            {
              if (readIndexOrClassId == 0xd) {
                if ((((*(uint32_t *)(*modelPayload + 0xc4) &
                      armyDefinition[1].selectionDetailTemplateVariantIndex) != 0) &&
                    (armyDefinition->registryId == modelPayload[0x18])) && (modelPayload[0x2e] == 1)) {
                  modelPayload[0x3d] = modelPayload[0x3d] - modelPayload[0x1d];
                  refundAmount = armyDefinition[2].registryId;
                  modelPayload[0x1d] = 0;
                  modelPayload[0x2e] = 0;
                  modelPayload[0x3b] = modelPayload[0x3b] & 0xfffffeff;
                  modelPayload[0x18] = 0;
                  g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 =
                       g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 + refundAmount;
                  requestedCount = requestedCount - 1;
                  if (requestedCount == 0) {
                    return;
                  }
                }
              }
              else if (readIndexOrClassId == 0xb) {
                if (armyDefinition->registryId == modelPayload[0x18]) {
                  modelPayload[0x3d] = modelPayload[0x3d] - modelPayload[0x1d];
                  refundAmount = armyDefinition[2].registryId;
                  modelPayload[0x1d] = 0;
                  modelPayload[0x2e] = 0;
                  modelPayload[0x3b] = modelPayload[0x3b] & 0xfffffeff;
                  modelPayload[0x18] = 0;
                  g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 =
                       g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 + refundAmount;
                  requestedCount = requestedCount - 1;
                  if (requestedCount == 0) {
                    return;
                  }
                }
              }
              else if (armyDefinition->registryId == modelPayload[0x18]) {
                modelPayload[0x3d] = modelPayload[0x3d] - modelPayload[0x1d];
                refundAmount = armyDefinition[2].registryId;
                modelPayload[0x1d] = 0;
                modelPayload[0x2b] = 0;
                modelPayload[0x3b] = modelPayload[0x3b] & 0xfffffeff;
                modelPayload[0x18] = 0;
                g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 =
                     g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 + refundAmount;
                requestedCount = requestedCount - 1;
                if (requestedCount == 0) {
                  return;
                }
              }
            }
          }
          ownerNode = ownerNode->nextNode;
        } while (ownerNode != (WorldOwnerListNode100 *)0x0);
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
   Cross-module calls: ArmyAssetRegistry_FindById [assets/army/catalog], UiCommandSpriteVariantA_RebuildGrid
   [ui/ingame/commands].
*/
void __thandor_void_preserve_eax_ecx_edx
GameFactionRuntime_RemoveArmyAssetAndStagePlayerTransfer
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedDword04,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex)

{
  FactionArmyAssetCount *primaryCount;
  SelectionPlayerRuntimeBlock *playerBlock;
  InGameRuntimeRootImageC3E4 *runtimeRoot;
  ArmyAssetRecordPrefix *armyDefinition;
  int byteOffsetOrActiveFaction;
  FactionArmyAssetCount assetsRemaining;
  ArmyAssetLookupResult resolvedAsset;
  
  resolvedAsset = ArmyAssetRegistry_FindById(armyAssetId);
  armyDefinition = resolvedAsset.recordOrError;
  if (!resolvedAsset.notFound) {
    playerBlock = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId];
    byteOffsetOrActiveFaction = factionIndex * 0x740;
    assetsRemaining = g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
    playerBlock->pendingSelectionEntityOffset8098 = (uint32_t)armyDefinition;
    for (; assetsRemaining != 0; assetsRemaining = assetsRemaining - 1) {
      if (armyDefinition ==
          *(ArmyAssetRecordPrefix **)
           ((int)g_GameFactionRuntimeImage.records[0].primaryArmyAssetPointersOrIds + byteOffsetOrActiveFaction)) {
        do {
          *(uint32_t *)
           ((int)g_GameFactionRuntimeImage.records[0].primaryArmyAssetPointersOrIds + byteOffsetOrActiveFaction) =
               *(uint32_t *)
                ((int)g_GameFactionRuntimeImage.records[0].primaryArmyAssetPointersOrIds + byteOffsetOrActiveFaction + 4
                );
          runtimeRoot = g_InGameRuntimeRoot;
          byteOffsetOrActiveFaction = byteOffsetOrActiveFaction + 4;
          assetsRemaining = assetsRemaining - 1;
        } while (assetsRemaining != 0);
        byteOffsetOrActiveFaction = (g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex;
        primaryCount = &g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
        *primaryCount = *primaryCount - 1;
        if (factionIndex != byteOffsetOrActiveFaction) {
          return;
        }
        UiCommandSpriteVariantA_RebuildGrid((UiNodeBase *)runtimeRoot);
        if (playerRuntimeId != g_LocalPlayerRuntimeId) {
          return;
        }
        g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | 0x20;
        g_InGamePendingPlacementArmyAsset = (int32_t)armyDefinition;
        g_InGamePlacementSurfaceHeightQ12OrSentinel = 0x7fffffff;
        return;
      }
      byteOffsetOrActiveFaction = byteOffsetOrActiveFaction + 4;
    }
    playerBlock->pendingSelectionEntityOffset8098 = 0;
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
          (PlayerRuntimeId playerRuntimeId,uint32_t unusedConsumeArgument0,uint32_t unusedConsumeArgument1
          ,FactionRuntimeIndex factionIndex)

{
  FactionArmyAssetCount *primaryCount;
  uint32_t pendingAsset;
  uint32_t assetCount;
  InGameRuntimeRootImageC3E4 *runtimeRoot;
  
  LOCK();
  pendingAsset = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->pendingSelectionEntityOffset8098;
  g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->pendingSelectionEntityOffset8098 = 0;
  runtimeRoot = g_InGameRuntimeRoot;
  UNLOCK();
  if (pendingAsset != 0) {
    assetCount = g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
    if (assetCount < 0x40) {
      *(uint32_t *)(factionIndex * 0x740 + THANDOR_ADDR(g_GameFactionRuntimeImage,0x1e0) + assetCount * 4) = pendingAsset;
      primaryCount = &g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
      *primaryCount = *primaryCount + 1;
    }
    if (factionIndex == (runtimeRoot->worldRuntime0A30).activeFactionRuntimeIndex) {
      UiCommandSpriteVariantA_RebuildGrid((UiNodeBase *)runtimeRoot);
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
   Cross-module calls: ArmyAssetRegistry_FindById [assets/army/catalog], UiCommandSpriteVariantA_RebuildGrid
   [ui/ingame/commands].
*/
void __thandor_void_preserve_eax_ecx_edx
GameFactionRuntime_SellArmyAssetAndRefundSevenEighths
          (uint32_t unusedSaleArgument0,uint32_t unusedSaleArgument1,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex)

{
  FactionArmyAssetCount *primaryCount;
  PckArmyAssetIdCatalog storedValue;
  InGameRuntimeRootImageC3E4 *runtimeRoot;
  int byteOffsetOrActiveFaction;
  FactionArmyAssetCount assetsRemaining;
  ArmyAssetLookupResult resolvedAsset;
  
  resolvedAsset = ArmyAssetRegistry_FindById(armyAssetId);
  if (!resolvedAsset.notFound) {
    byteOffsetOrActiveFaction = factionIndex * 0x740;
    for (assetsRemaining = g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount; assetsRemaining != 0;
        assetsRemaining = assetsRemaining - 1) {
      if (resolvedAsset.recordOrError ==
          *(ArmyAssetRecordPrefix **)
           ((int)g_GameFactionRuntimeImage.records[0].primaryArmyAssetPointersOrIds + byteOffsetOrActiveFaction)) {
        do {
          *(uint32_t *)
           ((int)g_GameFactionRuntimeImage.records[0].primaryArmyAssetPointersOrIds + byteOffsetOrActiveFaction) =
               *(uint32_t *)
                ((int)g_GameFactionRuntimeImage.records[0].primaryArmyAssetPointersOrIds + byteOffsetOrActiveFaction + 4
                );
          runtimeRoot = g_InGameRuntimeRoot;
          byteOffsetOrActiveFaction = byteOffsetOrActiveFaction + 4;
          assetsRemaining = assetsRemaining - 1;
        } while (assetsRemaining != 0);
        storedValue = resolvedAsset.recordOrError[2].registryId;
        byteOffsetOrActiveFaction = (g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex;
        primaryCount = &g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
        *primaryCount = *primaryCount - 1;
        g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 =
             g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 +
             ((int)(storedValue * 7) >> 3);
        if (byteOffsetOrActiveFaction != factionIndex) {
          return;
        }
        UiCommandSpriteVariantA_RebuildGrid((UiNodeBase *)runtimeRoot);
        return;
      }
      byteOffsetOrActiveFaction = byteOffsetOrActiveFaction + 4;
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
   Cross-module calls: ArmyRuntime_CreateInstanceFromAsset [gameplay/army/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
PlayerRuntime_ResolveAndStoreState8094
          (PlayerRuntimeId playerRuntimeId,PlayerStateLookupValue0 lookupValue0,
          PlayerStateLookupValue1 lookupValue1,RuntimeToken lookupToken)

{
  SelectionPlayerRuntimeBlock *playerBlock;
  ArmyRuntimeCreateResult createdRuntime;
  
  playerBlock = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId];
  createdRuntime = ArmyRuntime_CreateInstanceFromAsset
                    (4,0,lookupValue0,lookupValue1,playerBlock->constructionLookupState8090,lookupToken,
                     &g_InGameRuntimeRoot->worldRuntime0A30);
  if (!createdRuntime.failed) {
    playerBlock->primarySelectionEntityOffset8094 = createdRuntime.armyRuntimeOrError - (int)g_ArmyRuntimeRebaseBaseMinusOne;
    return;
  }
  playerBlock->primarySelectionEntityOffset8094 = 0;
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
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedZero0,uint32_t reservedZero1,
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
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedZero0,uint32_t reservedZero1,
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
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedZero0,uint32_t reservedZero1,
          uint32_t reservedZero2)

{
  g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->primarySelectionEntityOffset8094 = 0;
  return;
}


/* Address: 0x00565590.
   Applies the mission carry-over stored by OldUnitRuntime_RebuildScenarioReplayTables at the start of the next
   mission: ORs each faction's saved technology masks into its record, recreates every carried-over unit
   (0x20-byte primary records: asset id, faction, position, rotation) in the world, then rebuilds terrain
   occupancy and the cell classification for the active faction.
*/
void __thandor_void_preserve_eax_ecx_edx OldUnitRuntime_MergeMasksAndReplayRecords(void)

{
  InGameRuntimeRootImageC3E4 *runtimeRoot;
  uint32_t *nextMaskCursor;
  int factionsRemaining;
  OldUnitRecordCount recordsRemaining;
  int wordsRemaining;
  WorldRuntimeContext *worldRuntime;
  uint32_t *secondaryCursor;
  uint32_t *primaryRecordCursor;
  uint32_t *maskCursor;
  
  factionsRemaining = 8;
  wordsRemaining = 8;
  /* 8 factions x 8 dwords; the faction records are 0x740 bytes apart */
  secondaryCursor = g_OldUnitSecondaryTable;
  nextMaskCursor = g_GameFactionRuntimeImage.records[0].technologyMasks256Bits;
  do {
    do {
      maskCursor = nextMaskCursor;
      *maskCursor = *maskCursor | *secondaryCursor;
      runtimeRoot = g_InGameRuntimeRoot; /* Ghidra placement; the original reads it once after the loop */
      secondaryCursor++;
      wordsRemaining--;
      nextMaskCursor = maskCursor + 1;
    } while (wordsRemaining != 0);
    wordsRemaining = 8;
    factionsRemaining--;
    nextMaskCursor = maskCursor + 0x1c9;
  } while (factionsRemaining != 0);
  if ((g_InGameRuntimeRoot != NULL) && (g_OldUnitRecordCount != 0)) {
    worldRuntime = &g_InGameRuntimeRoot->worldRuntime0A30;
    recordsRemaining = g_OldUnitRecordCount;
    primaryRecordCursor = g_OldUnitPrimaryTable;
    do {
      /* record [2] and [3] go to the parameters named worldYQ12 / worldXQ12 (pushed as in the original,
         0x005655FC/0x005655FF), although the rebuild stores the X coordinate in [2] */
      ArmyRuntime_CreateInstanceFromAsset
                (6,primaryRecordCursor[4],primaryRecordCursor[3],primaryRecordCursor[2],primaryRecordCursor[1],*primaryRecordCursor,worldRuntime);
      primaryRecordCursor = primaryRecordCursor + 8; /* 0x20-byte records */
      recordsRemaining--;
    } while (recordsRemaining != 0);
    WorldRuntime_ForEachNodeInOwnerListD8
              (worldRuntime,ArmyRuntimeNode_AccumulateTerrainOcclusionAndOccupancyCallback,
               worldRuntime);
    WorldRuntime_ForEachNodeInOwnerListD8
              (worldRuntime,ArmyRuntimeNode_RebuildTerrainOccupancyAndVisualStateCallback,
               worldRuntime);
    FieldGrid_ClassifyCellFlagsToRuntimeByte
              ((runtimeRoot->worldRuntime0A30).activeFactionRuntimeIndex,
               (runtimeRoot->worldRuntime0A30).fieldGrid);
  }
  return;
}


/* Address: 0x00513D00.
   Ownership: gameplay/faction/runtime.
   Purpose: Checks the packed relation nibble for factionIndex toward otherFactionIndex. CF is set only for states
   2, 5, or 9 whose bidirectional timestamp age is at most 0x258 ticks; EAX is preserved.
*/
bool __thandor_cf_preserve_eax_ecx_edx
GameFactionRuntime_IsRecentTimedRelationState
          (FactionRuntimeIndex otherFactionIndex,FactionRuntimeIndex factionIndex)

{
  uint32_t relationStateNibble;
  
  relationStateNibble =
       g_GameFactionRuntimeImage.records[factionIndex].packedRelationStates >>
       ((char)otherFactionIndex * '\x04' & 0x1fU) & 0xf;
  if ((((relationStateNibble == 2) || (relationStateNibble == 5)) || (relationStateNibble == 9)) &&
     ((int)(g_GameFactionRuntimeImage.tail.simulationTick -
           *(int *)(factionIndex * 0x740 + THANDOR_ADDR(g_GameFactionRuntimeImage,0x700) + otherFactionIndex * 4)) < 0x259)) {
    return true;
  }
  return false;
}


/* Address: 0x00565650.
   Drops any pending mission carry-over: clears the 64-dword technology-mask table and the unit-record count, so
   OldUnitRuntime_MergeMasksAndReplayRecords has nothing to apply.
*/
void __thandor_void_preserve_eax_ecx OldUnitRuntime_ResetPendingTables(void)

{
  int tableEntriesRemaining;
  uint32_t *tableCursor;
  
  tableCursor = g_OldUnitSecondaryTable;
  for (tableEntriesRemaining = 0x40; tableEntriesRemaining != 0;
      tableEntriesRemaining--) {
    *tableCursor = 0;
    tableCursor++;
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
  FactionCapabilityFlags *capabilityFlagsField;
  TritiumAmountQ4 *tritiumField;
  XeniteAmountQ4 *xeniteField;
  EnergyAmountQ4 *energyField;
  uint32_t *technologyMaskWord;
  FactionRelationCounter *relationCounter;
  FactionArmyAssetCount *armyAssetCount;
  TritiumAmountQ4 tritiumAmount;
  XeniteAmountQ4 xeniteLimit;
  TritiumAmountQ4 tritiumLimit;
  EnergyAmountQ4 energyCapacity;
  uint32_t trailingMaskWord;
  int counterValueBOrE;
  int counterValueF;
  InGameSimulationTick currentTick;
  FactionRuntimeIndex originalFirstFactionIndex;
  uint32_t bitOrPlayerCountOrSlot;
  uint32_t randomValue;
  bool swapMergeDirection;
  uint8_t secondShift;
  uint8_t firstShift;
  FactionArmyAssetCount assetsRemaining;
  uint32_t playerCountOrMaskWord;
  FrontendPlayerRuntimeRecord *playerBlockCursor;
  int recordOrCountOrIndex;
  FrontendPlayerRuntimeBlockCount playerBlocksRemaining;
  FieldGridCell *gridCell;
  uint32_t primaryArmyAssetReferenceDword;
  uint32_t secondaryArmyAssetReferenceDword;
  GraphicsTextureSet *survivingFactionTextureSet;
  GraphicsPaletteAsset *survivingFactionPaletteAsset;
  FieldGridAsset *terrainGrid;
  InGameRuntimeRootImageC3E4 *runtimeRoot;
  WorldOwnerListNode100 *ownerNode;
  
  originalFirstFactionIndex = firstFactionIndex;
  if ((g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex == secondFactionIndex) {
    InGameNotificationQueue_InsertPriorityRecord(NOTIFICATION_PAYLOAD_NONE,0,0,0,0,0,2,activeFactionCodeForSecond + 500);
  }
  else if ((g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex == firstFactionIndex) {
    InGameNotificationQueue_InsertPriorityRecord(NOTIFICATION_PAYLOAD_NONE,0,0,0,0,0,2,activeFactionCodeForFirst + 500);
  }
  secondShift = (uint8_t)secondFactionIndex * '\x04';
  firstShift = (uint8_t)firstFactionIndex * '\x04';
  g_GameFactionRuntimeImage.records[firstFactionIndex].packedRelationStates =
       stateFirstTowardSecond << (secondShift & 0x1f) |
       ~(0xf << (secondShift & 0x1f)) &
       g_GameFactionRuntimeImage.records[firstFactionIndex].packedRelationStates;
  g_GameFactionRuntimeImage.records[secondFactionIndex].packedRelationStates =
       ~(0xf << (firstShift & 0x1f)) &
       g_GameFactionRuntimeImage.records[secondFactionIndex].packedRelationStates |
       stateSecondTowardFirst << (firstShift & 0x1f);
  bitOrPlayerCountOrSlot = 1 << ((uint8_t)secondFactionIndex & 0x1f);
  if (stateSecondTowardFirst < 4) {
    capabilityFlagsField = &g_GameFactionRuntimeImage.records[firstFactionIndex].capabilityFlags;
    *capabilityFlagsField = *capabilityFlagsField & ~bitOrPlayerCountOrSlot;
  }
  else {
    capabilityFlagsField = &g_GameFactionRuntimeImage.records[firstFactionIndex].capabilityFlags;
    *capabilityFlagsField = *capabilityFlagsField | bitOrPlayerCountOrSlot;
  }
  bitOrPlayerCountOrSlot = 1 << ((uint8_t)firstFactionIndex & 0x1f);
  if (stateSecondTowardFirst < 4) {
    capabilityFlagsField = &g_GameFactionRuntimeImage.records[secondFactionIndex].capabilityFlags;
    *capabilityFlagsField = *capabilityFlagsField & ~bitOrPlayerCountOrSlot;
  }
  else {
    capabilityFlagsField = &g_GameFactionRuntimeImage.records[secondFactionIndex].capabilityFlags;
    *capabilityFlagsField = *capabilityFlagsField | bitOrPlayerCountOrSlot;
  }
  currentTick = g_GameFactionRuntimeImage.tail.simulationTick;
  *(InGameSimulationTick *)(firstFactionIndex * 0x740 + THANDOR_ADDR(g_GameFactionRuntimeImage,0x700) + secondFactionIndex * 4) =
       g_GameFactionRuntimeImage.tail.simulationTick;
  *(InGameSimulationTick *)(secondFactionIndex * 0x740 + THANDOR_ADDR(g_GameFactionRuntimeImage,0x700) + firstFactionIndex * 4) = currentTick;
  if (stateSecondTowardFirst != 0xb)
  goto GameFactionRuntime_ApplyPairwiseRelationTransition_RebuildTargetEntriesAndReturn;
  bitOrPlayerCountOrSlot = 0;
  playerCountOrMaskWord = 0;
  playerBlocksRemaining = g_FrontendPlayerRuntimeBlockCount;
  playerBlockCursor = g_FrontendPlayerRuntimeBlocks;
  do {
    if (secondFactionIndex == (playerBlockCursor->factionAssignment).factionAssignmentIndex) {
      bitOrPlayerCountOrSlot = bitOrPlayerCountOrSlot + 1;
    }
    if (firstFactionIndex == (playerBlockCursor->factionAssignment).factionAssignmentIndex) {
      playerCountOrMaskWord = playerCountOrMaskWord + 1;
    }
    playerBlockCursor = playerBlockCursor + 1;
    playerBlocksRemaining = playerBlocksRemaining - 1;
  } while (playerBlocksRemaining != 0);
  /* State 11 merges the two factions. The second faction survives when it has players and the (bitwise) counts
     do not overlap; with no players on either side, or overlapping counts, a random bit decides. */
  if (((bitOrPlayerCountOrSlot & playerCountOrMaskWord) == 0) &&
      ((bitOrPlayerCountOrSlot != 0) || (playerCountOrMaskWord != 0))) {
    swapMergeDirection = bitOrPlayerCountOrSlot != 0;
  }
  else {
    randomValue = g_RandomGeneratorState.next();
    swapMergeDirection = (randomValue & 0x2000) == 0;
  }
  if (swapMergeDirection) {
    firstFactionIndex = secondFactionIndex;
    secondFactionIndex = originalFirstFactionIndex;
  }
  runtimeRoot = g_InGameRuntimeRoot;
  if (secondFactionIndex == (g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex) {
    (g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex = firstFactionIndex;
  }
  survivingFactionTextureSet = g_ArmyGraphicsBindings[firstFactionIndex].textureSet;
  survivingFactionPaletteAsset = g_ArmyGraphicsBindings[firstFactionIndex].paletteAsset;
  playerBlockCursor = g_FrontendPlayerRuntimeBlocks;
  playerBlocksRemaining = g_FrontendPlayerRuntimeBlockCount;
  for (ownerNode = (runtimeRoot->worldRuntime0A30).ownerListHead;
      g_FrontendPlayerRuntimeBlocks = playerBlockCursor, g_FrontendPlayerRuntimeBlockCount = playerBlocksRemaining,
      ownerNode != (WorldOwnerListNode100 *)0x0; ownerNode = ownerNode->nextNode) {
    if ((ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
       (recordOrCountOrIndex = *(int *)((int)ownerNode->runtimePayload + 8),
       *(int *)(recordOrCountOrIndex + 0xc) == secondFactionIndex)) {
      *(FactionRuntimeIndex *)(recordOrCountOrIndex + 0xc) = firstFactionIndex;
      ModelRuntimeHierarchy_SetCommandTargetRecursive
                (survivingFactionPaletteAsset,survivingFactionTextureSet,
                 *(ModelRuntimeNode **)(recordOrCountOrIndex + 4));
    }
    playerBlockCursor = g_FrontendPlayerRuntimeBlocks;
    playerBlocksRemaining = g_FrontendPlayerRuntimeBlockCount;
  }
  do {
    if (secondFactionIndex == (playerBlockCursor->factionAssignment).factionAssignmentIndex) {
      recordOrCountOrIndex = playerBlockCursor->playerRuntimeId;
      (playerBlockCursor->factionAssignment).factionAssignmentIndex = firstFactionIndex;
      g_SelectionPlayerRuntimeBlockPointers[recordOrCountOrIndex]->primaryEntityOrFactionToken8080 =
           firstFactionIndex;
    }
    playerBlockCursor = playerBlockCursor + 1;
    playerBlocksRemaining = playerBlocksRemaining - 1;
  } while (playerBlocksRemaining != 0);
  terrainGrid = (runtimeRoot->worldRuntime0A30).fieldGrid;
  recordOrCountOrIndex = terrainGrid->gridWidth * terrainGrid->gridHeight;
  gridCell = terrainGrid->cells;
  do {
    gridCell->runtime60_6B[firstFactionIndex + 0x10] =
         gridCell->runtime60_6B[firstFactionIndex + 0x10] |
         gridCell->runtime60_6B[secondFactionIndex + 0x10];
    gridCell = gridCell + 1;
    recordOrCountOrIndex = recordOrCountOrIndex + -1;
  } while (recordOrCountOrIndex != 0);
  g_GameFactionRuntimeImage.tail.factionLifecycleStates[secondFactionIndex] = 0;
  tritiumAmount = g_GameFactionRuntimeImage.records[secondFactionIndex].tritiumCurrentQ4;
  xeniteLimit = g_GameFactionRuntimeImage.records[secondFactionIndex].xeniteStorageLimitQ4;
  tritiumLimit = g_GameFactionRuntimeImage.records[secondFactionIndex].tritiumStorageLimitQ4;
  g_GameFactionRuntimeImage.records[firstFactionIndex].xeniteCurrentQ4 =
       g_GameFactionRuntimeImage.records[firstFactionIndex].xeniteCurrentQ4 +
       g_GameFactionRuntimeImage.records[secondFactionIndex].xeniteCurrentQ4;
  tritiumField = &g_GameFactionRuntimeImage.records[firstFactionIndex].tritiumCurrentQ4;
  *tritiumField = *tritiumField + tritiumAmount;
  xeniteField = &g_GameFactionRuntimeImage.records[firstFactionIndex].xeniteStorageLimitQ4;
  *xeniteField = *xeniteField + xeniteLimit;
  tritiumField = &g_GameFactionRuntimeImage.records[firstFactionIndex].tritiumStorageLimitQ4;
  *tritiumField = *tritiumField + tritiumLimit;
  energyCapacity = g_GameFactionRuntimeImage.records[secondFactionIndex].energyGenerationCapacityQ4;
  energyField = &g_GameFactionRuntimeImage.records[firstFactionIndex].baselineEnergySupplyQ4;
  *energyField = *energyField + g_GameFactionRuntimeImage.records[secondFactionIndex].baselineEnergySupplyQ4;
  energyField = &g_GameFactionRuntimeImage.records[firstFactionIndex].energyGenerationCapacityQ4;
  *energyField = *energyField + energyCapacity;
  bitOrPlayerCountOrSlot = g_GameFactionRuntimeImage.records[secondFactionIndex].technologyMasks256Bits[1];
  playerCountOrMaskWord = g_GameFactionRuntimeImage.records[secondFactionIndex].technologyMasks256Bits[2];
  trailingMaskWord = g_GameFactionRuntimeImage.records[secondFactionIndex].technologyMasks256Bits[3];
  technologyMaskWord = g_GameFactionRuntimeImage.records[firstFactionIndex].technologyMasks256Bits;
  *technologyMaskWord = *technologyMaskWord | g_GameFactionRuntimeImage.records[secondFactionIndex].technologyMasks256Bits
                      [0];
  technologyMaskWord = g_GameFactionRuntimeImage.records[firstFactionIndex].technologyMasks256Bits + 1;
  *technologyMaskWord = *technologyMaskWord | bitOrPlayerCountOrSlot;
  technologyMaskWord = g_GameFactionRuntimeImage.records[firstFactionIndex].technologyMasks256Bits + 2;
  *technologyMaskWord = *technologyMaskWord | playerCountOrMaskWord;
  technologyMaskWord = g_GameFactionRuntimeImage.records[firstFactionIndex].technologyMasks256Bits + 3;
  *technologyMaskWord = *technologyMaskWord | trailingMaskWord;
  bitOrPlayerCountOrSlot = g_GameFactionRuntimeImage.records[secondFactionIndex].technologyMasks256Bits[5];
  playerCountOrMaskWord = g_GameFactionRuntimeImage.records[secondFactionIndex].technologyMasks256Bits[6];
  trailingMaskWord = g_GameFactionRuntimeImage.records[secondFactionIndex].technologyMasks256Bits[7];
  technologyMaskWord = g_GameFactionRuntimeImage.records[firstFactionIndex].technologyMasks256Bits + 4;
  *technologyMaskWord = *technologyMaskWord | g_GameFactionRuntimeImage.records[secondFactionIndex].technologyMasks256Bits
                      [4];
  technologyMaskWord = g_GameFactionRuntimeImage.records[firstFactionIndex].technologyMasks256Bits + 5;
  *technologyMaskWord = *technologyMaskWord | bitOrPlayerCountOrSlot;
  technologyMaskWord = g_GameFactionRuntimeImage.records[firstFactionIndex].technologyMasks256Bits + 6;
  *technologyMaskWord = *technologyMaskWord | playerCountOrMaskWord;
  technologyMaskWord = g_GameFactionRuntimeImage.records[firstFactionIndex].technologyMasks256Bits + 7;
  *technologyMaskWord = *technologyMaskWord | trailingMaskWord;
  tritiumAmount = g_GameFactionRuntimeImage.records[secondFactionIndex].tritiumExtractedTotalQ4;
  recordOrCountOrIndex = g_GameFactionRuntimeImage.records[secondFactionIndex].relationCounterA;
  counterValueBOrE = g_GameFactionRuntimeImage.records[secondFactionIndex].relationCounterB;
  xeniteField = &g_GameFactionRuntimeImage.records[firstFactionIndex].xeniteExtractedTotalQ4;
  *xeniteField = *xeniteField + g_GameFactionRuntimeImage.records[secondFactionIndex].xeniteExtractedTotalQ4;
  tritiumField = &g_GameFactionRuntimeImage.records[firstFactionIndex].tritiumExtractedTotalQ4;
  *tritiumField = *tritiumField + tritiumAmount;
  relationCounter = &g_GameFactionRuntimeImage.records[firstFactionIndex].relationCounterA;
  *relationCounter = *relationCounter + recordOrCountOrIndex;
  relationCounter = &g_GameFactionRuntimeImage.records[firstFactionIndex].relationCounterB;
  *relationCounter = *relationCounter + counterValueBOrE;
  recordOrCountOrIndex = g_GameFactionRuntimeImage.records[secondFactionIndex].relationCounterD;
  counterValueBOrE = g_GameFactionRuntimeImage.records[secondFactionIndex].relationCounterE;
  counterValueF = g_GameFactionRuntimeImage.records[secondFactionIndex].relationCounterF;
  relationCounter = &g_GameFactionRuntimeImage.records[firstFactionIndex].relationCounterC;
  *relationCounter = *relationCounter + g_GameFactionRuntimeImage.records[secondFactionIndex].relationCounterC;
  relationCounter = &g_GameFactionRuntimeImage.records[firstFactionIndex].relationCounterD;
  *relationCounter = *relationCounter + recordOrCountOrIndex;
  relationCounter = &g_GameFactionRuntimeImage.records[firstFactionIndex].relationCounterE;
  *relationCounter = *relationCounter + counterValueBOrE;
  relationCounter = &g_GameFactionRuntimeImage.records[firstFactionIndex].relationCounterF;
  *relationCounter = *relationCounter + counterValueF;
  assetsRemaining = g_GameFactionRuntimeImage.records[secondFactionIndex].primaryArmyAssetCount;
  recordOrCountOrIndex = 0;
  for (bitOrPlayerCountOrSlot = g_GameFactionRuntimeImage.records[firstFactionIndex].primaryArmyAssetCount;
      (assetsRemaining != 0 && (bitOrPlayerCountOrSlot < 0x40)); bitOrPlayerCountOrSlot = bitOrPlayerCountOrSlot + 1) {
    primaryArmyAssetReferenceDword = *(uint32_t *)(secondFactionIndex * 0x740 + THANDOR_ADDR(g_GameFactionRuntimeImage,0x1e0) + recordOrCountOrIndex * 4);
    armyAssetCount = &g_GameFactionRuntimeImage.records[firstFactionIndex].primaryArmyAssetCount;
    *armyAssetCount = *armyAssetCount + 1;
    *(uint32_t *)(firstFactionIndex * 0x740 + THANDOR_ADDR(g_GameFactionRuntimeImage,0x1e0) + bitOrPlayerCountOrSlot * 4) = primaryArmyAssetReferenceDword;
    recordOrCountOrIndex = recordOrCountOrIndex + 1;
    assetsRemaining = assetsRemaining - 1;
  }
  assetsRemaining = g_GameFactionRuntimeImage.records[secondFactionIndex].secondaryArmyAssetCount;
  recordOrCountOrIndex = 0;
  for (bitOrPlayerCountOrSlot = g_GameFactionRuntimeImage.records[firstFactionIndex].secondaryArmyAssetCount;
      (assetsRemaining != 0 && (bitOrPlayerCountOrSlot < 0x40)); bitOrPlayerCountOrSlot = bitOrPlayerCountOrSlot + 1) {
    secondaryArmyAssetReferenceDword =
         *(uint32_t *)(secondFactionIndex * 0x740 + THANDOR_ADDR(g_GameFactionRuntimeImage,0xe0) + recordOrCountOrIndex * 4);
    armyAssetCount = &g_GameFactionRuntimeImage.records[firstFactionIndex].secondaryArmyAssetCount;
    *armyAssetCount = *armyAssetCount + 1;
    *(uint32_t *)(firstFactionIndex * 0x740 + THANDOR_ADDR(g_GameFactionRuntimeImage,0xe0) + bitOrPlayerCountOrSlot * 4) = secondaryArmyAssetReferenceDword
    ;
    recordOrCountOrIndex = recordOrCountOrIndex + 1;
    assetsRemaining = assetsRemaining - 1;
  }
  UiCommandSpriteVariantA_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
  UiCatalogGroup42_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
  UiCatalogGroup48_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
GameFactionRuntime_ApplyPairwiseRelationTransition_RebuildTargetEntriesAndReturn:
  InGameOtherPlayerCommand_RebuildTargetEntries((UiNodeBase *)g_InGameRuntimeRoot);
  return;
}

