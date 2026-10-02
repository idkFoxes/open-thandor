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
   Moves the diplomatic relation of a faction pair one step closer, chosen by the state of targetFactionIndex
   towards sourceFactionIndex: 0..2 -> 3/2, 3 -> 4, 4..5 -> 6/5, 6 -> 8, 8..9 -> 10/9, 10 -> 11 (merge),
   first value for the source's state towards the target. State 2 does nothing while the last change is at
   most 600 ticks old; the same check in states 4 and 8 never holds (it only accepts 2, 5 and 9). The first
   two arguments are not used.
*/
void GameFactionRuntime_AdvancePairwiseRelationState(uint32_t unusedRelationArgument0,uint32_t unusedRelationArgument1,
          FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex)

{
  bool isRecentTimedState;
  
  switch(g_GameFactionRuntimeImage.records[targetFactionIndex].packedRelationStates >>
         ((uint8_t)(sourceFactionIndex << 2) & 31) & 0xf) {
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
              (8,8,FACTION_RELATION_MERGE,FACTION_RELATION_MERGE,sourceFactionIndex,targetFactionIndex);
  }
  return;
}


/* Address: 0x0055F910.
   Moves the diplomatic relation of a faction pair back, chosen by the state of targetFactionIndex towards
   sourceFactionIndex: 1..3 -> 0, 4 and 6 -> 0, 5 -> 4, 8 and 10 -> 4, 9 -> 8 (both directions get the same
   state), with the matching notification text. Other states stay. The first two arguments are not used.
*/
void GameFactionRuntime_ResetPairwiseRelationState(uint32_t unusedRelationArgument0,uint32_t unusedRelationArgument1,
          FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex)

{
  switch(g_GameFactionRuntimeImage.records[targetFactionIndex].packedRelationStates >>
         ((uint8_t)(sourceFactionIndex << 2) & 31) & 0xf) {
  case 1:
  case 2:
  case 3:
    GameFactionRuntime_ApplyPairwiseRelationTransition
              (19,19,0,0,sourceFactionIndex,targetFactionIndex);
    break;
  case 4:
  case 6:
    GameFactionRuntime_ApplyPairwiseRelationTransition
              (9,9,0,0,sourceFactionIndex,targetFactionIndex);
    break;
  case 5:
    GameFactionRuntime_ApplyPairwiseRelationTransition
              (19,19,4,4,sourceFactionIndex,targetFactionIndex);
    break;
  case 8:
  case 10:
    GameFactionRuntime_ApplyPairwiseRelationTransition
              (10,10,4,4,sourceFactionIndex,targetFactionIndex);
    break;
  case 9:
    GameFactionRuntime_ApplyPairwiseRelationTransition
              (19,19,8,8,sourceFactionIndex,targetFactionIndex);
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
void OldUnitRuntime_RebuildScenarioReplayTables(void)

{
  WorldOwnerListNode *ownerNode;
  uint32_t unitFactionOrAssetId;
  uint32_t carryOverMaskBits;
  uint32_t distanceToAnchor;
  int remainingOrWorldY;
  uint32_t skipMaskBits;
  int copyCountOrAnchorY;
  int scenarioRecord;
  int destinationY;
  CampaignLevelRecord *factionExitZoneCursor;
  uint32_t *sourceOrRecordCursor;
  uint32_t *secondaryTableCursor;
  bool scenarioFound;

  scenarioFound = false;
  /* Campaign asset (CampaignAsset): the cursor starts at the asset base and advances by one 0x180-byte level
     record, so ((CampaignAsset *)cursor)->levels[0] is the current record.
     Original quirk: the original offsets the faction-record cursor by an uninitialized value times the
     active faction index; the loop then walks all eight 0x740-byte faction records, which only stays
     inside the table from records[0], so the cursor always starts there. */
  if ((g_InGameRuntimeRoot != NULL) &&
     (sourceOrRecordCursor = (uint32_t *)g_GameFactionRuntimeImage.records[0].technologyMasks256Bits,
     g_FrontendLoadedCampaignAsset != 0)) {
    remainingOrWorldY = ((CampaignAsset *)g_FrontendLoadedCampaignAsset)->levelRecordCount;
    scenarioRecord = g_FrontendLoadedCampaignAsset;
    do {
      if (((CampaignAsset *)g_FrontendLoadedCampaignAsset)->currentLevelId ==
          ((CampaignAsset *)scenarioRecord)->levels[0].levelId) {
        scenarioFound = true;
        break;
      }
      scenarioRecord = scenarioRecord + sizeof(CampaignLevelRecord);
      remainingOrWorldY--;
    } while (remainingOrWorldY != 0);
  }
  if (!scenarioFound) {
    OldUnitRuntime_ResetPendingTables();
    return;
  }
  /* Per-faction exit zones of the level record; carry-over and skip hold one bit per outcome.
     factionExitZoneCursor walks the level record 4 bytes (one faction) per group, so element 0 of each
     per-faction array is the current faction's. */
  factionExitZoneCursor = &((CampaignAsset *)scenarioRecord)->levels[0];
  carryOverMaskBits =
       ((CampaignAsset *)scenarioRecord)->levels[0].carryOverMask >> ((uint8_t)g_EndMovieSelectionIndex & 31);
  skipMaskBits = ((CampaignAsset *)scenarioRecord)->levels[0].skipMask >> ((uint8_t)g_EndMovieSelectionIndex & 31);
  secondaryTableCursor = g_OldUnitSecondaryTable;
  /* Eight groups, one per faction record. The masks are not shifted per group (as in the original). */
  for (remainingOrWorldY = 8; remainingOrWorldY != 0; remainingOrWorldY--) {
    if (((skipMaskBits & 1) == 0) && ((carryOverMaskBits & 1) != 0) &&
        (((factionExitZoneCursor->exitZoneCenterX[0] != 0 ||
           (factionExitZoneCursor->exitZoneCenterY[0] != 0)) ||
          (factionExitZoneCursor->exitZoneDestinationX[0] != 0)) ||
         ((factionExitZoneCursor->exitZoneDestinationY[0] != 0 ||
           (factionExitZoneCursor->exitZoneRadius[0] != 0))))) {
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
    sourceOrRecordCursor = sourceOrRecordCursor + GAME_FACTION_RUNTIME_RECORD_BYTES / 4 - 8;
    factionExitZoneCursor =
         (CampaignLevelRecord *)((uint8_t *)factionExitZoneCursor + sizeof(factionExitZoneCursor->exitZoneCenterX[0]));
  }
  /* Primary records (8 dwords each, at most 0x200): [0] army asset id, [1] faction, [2] X, [3] Y,
     [4] rotation angle. */
  if (((g_InGameRuntimeRoot != NULL) &&
      (ownerNode = (g_InGameRuntimeRoot->worldRuntime).ownerListHead, (skipMaskBits & 1) == 0)) &&
     (g_OldUnitRecordCount = 0, sourceOrRecordCursor = g_OldUnitPrimaryTable, (carryOverMaskBits & 1) != 0)) {
    for (; ownerNode != NULL; ownerNode = ownerNode->nextNode) {
      /* the owner army holds the faction and the army asset id. Only a model whose classState
         linkedArmyRuntimeOrSavedOffset (+0xF0) is zero is committed as a record. */
      if (((ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
          (unitFactionOrAssetId =
           ((ModelRuntimeSlot *)ownerNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex,
          0 < ((CampaignAsset *)scenarioRecord)->levels[0].exitZoneRadius[unitFactionOrAssetId])) &&
         (distanceToAnchor =
          FixedMath_Length2(((CampaignAsset *)scenarioRecord)->levels[0].exitZoneCenterY[unitFactionOrAssetId] -
                            ownerNode->worldYQ12,
                                    ((CampaignAsset *)scenarioRecord)->levels[0].exitZoneCenterX[unitFactionOrAssetId] -
                                         ownerNode->worldXQ12),
         (int)distanceToAnchor <= ((CampaignAsset *)scenarioRecord)->levels[0].exitZoneRadius[unitFactionOrAssetId])) {
        remainingOrWorldY = ownerNode->worldYQ12;
        destinationY =
             ((CampaignAsset *)scenarioRecord)->levels[0].exitZoneDestinationY[unitFactionOrAssetId];
        copyCountOrAnchorY = ((CampaignAsset *)scenarioRecord)->levels[0].exitZoneCenterY[unitFactionOrAssetId];
        sourceOrRecordCursor[2] = (ownerNode->worldXQ12 +
             ((CampaignAsset *)scenarioRecord)->levels[0].exitZoneDestinationX[unitFactionOrAssetId]) -
                     ((CampaignAsset *)scenarioRecord)->levels[0].exitZoneCenterX[unitFactionOrAssetId];
        sourceOrRecordCursor[3] = (remainingOrWorldY + destinationY) - copyCountOrAnchorY;
        sourceOrRecordCursor[1] = unitFactionOrAssetId;
        if (((ModelRuntimeSlot *)ownerNode->runtimePayload)->classState.linkedArmyRuntimeOrSavedOffset.modelRuntime ==
            NULL) {
          unitFactionOrAssetId =
               ((ModelRuntimeSlot *)ownerNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime->armyAssetId;
          sourceOrRecordCursor[4] = ownerNode->modelLocalRotationAngle2;
          *sourceOrRecordCursor = unitFactionOrAssetId;
          g_OldUnitRecordCount++;
          sourceOrRecordCursor = sourceOrRecordCursor + 8;
          if (OLD_UNIT_PRIMARY_RECORD_CAPACITY - 1 < g_OldUnitRecordCount) {
            return;
          }
        }
      }
    }
  }
  return;
}


/* Address: 0x005130B0.
   After loading a save: turns the army-asset ids of both army-asset lists of all eight faction records back into
   registry pointers (an unknown id empties that list) and converts the 256 saved runtime-group member offsets
   into pointers again (offset + g_ArmyRuntimeRebaseBaseMinusOne; 0 stays NULL).
*/
void GameFactionRuntime_RebaseLoadedArmyReferences(void)

{
  ArmyRuntimeSlot *rebasedSlot;
  int factionsRemaining;
  FactionArmyAssetCount assetsRemaining;
  int groupSlotsRemaining;
  GameFactionRuntimeImage *factionRecordCursor; /* steps one faction record at a time (records[0] = current) */
  uint32_t *assetIdCursor;
  ArmyRuntimeSlot **groupSlotCursor;
  ArmyAssetRecordPrefix *resolvedAsset;

  factionRecordCursor = &g_GameFactionRuntimeImage;
  factionsRemaining = 8;
  do {
    assetIdCursor = factionRecordCursor->records[0].secondaryArmyAssetPointersOrIds;
    for (assetsRemaining = factionRecordCursor->records[0].secondaryArmyAssetCount; assetsRemaining !=
         0; assetsRemaining--) {
      if (ArmyAssetRegistry_FindById(*assetIdCursor,&resolvedAsset) != 0) {
        factionRecordCursor->records[0].secondaryArmyAssetCount = 0;
        break;
      }
      *assetIdCursor = (uint32_t)resolvedAsset;
      assetIdCursor++;
    }
    assetIdCursor = factionRecordCursor->records[0].primaryArmyAssetPointersOrIds;
    for (assetsRemaining = factionRecordCursor->records[0].primaryArmyAssetCount; assetsRemaining !=
         0; assetsRemaining--) {
      if (ArmyAssetRegistry_FindById(*assetIdCursor,&resolvedAsset) != 0) {
        factionRecordCursor->records[0].primaryArmyAssetCount = 0;
        break;
      }
      *assetIdCursor = (uint32_t)resolvedAsset;
      assetIdCursor++;
    }
    groupSlotCursor = factionRecordCursor->records[0].runtimeGroupMembers8x32;
    groupSlotsRemaining = 256;
    do {
      rebasedSlot = NULL;
      if (*groupSlotCursor != NULL) {
        /* the slot holds the saved offset */
        rebasedSlot = (ArmyRuntimeSlot *)((int)*groupSlotCursor + (int)g_ArmyRuntimeRebaseBaseMinusOne);
      }
      *groupSlotCursor = rebasedSlot;
      groupSlotCursor++;
      groupSlotsRemaining--;
    } while (groupSlotsRemaining != 0);
    factionRecordCursor = (GameFactionRuntimeImage *)(factionRecordCursor->records + 1);
    factionsRemaining--;
  } while (factionsRemaining != 0);
  return;
}


/* Address: 0x00513960.
   Called when an army is destroyed: clears every slot of the eight factions' 256-entry runtime group member
   tables that still points to it, so no group keeps a dangling pointer to the freed army slot.
*/
void GameFactionRuntime_ClearRuntimeGroupMemberPointerFromAllFactionTables(void *runtimeGroupMember)

{
  int factionsRemaining;
  int groupSlotsRemaining;
  int groupSlotIndex;
  GameFactionRuntimeImage *factionRecordCursor;
  
  factionRecordCursor = &g_GameFactionRuntimeImage;
  factionsRemaining = 8;
  groupSlotsRemaining = 256;
  groupSlotIndex = 0;
  do {
    do {
      if (runtimeGroupMember ==
          factionRecordCursor->records[0].runtimeGroupMembers8x32[groupSlotIndex]) {
        factionRecordCursor->records[0].runtimeGroupMembers8x32[groupSlotIndex] = NULL;
      }
      groupSlotIndex++;
      groupSlotsRemaining--;
    } while (groupSlotsRemaining != 0);
    /* the cursor steps one faction record (0x740 bytes) at a time */
    factionRecordCursor = (GameFactionRuntimeImage *)(factionRecordCursor->records + 1);
    groupSlotsRemaining = 256;
    groupSlotIndex = 0;
    factionsRemaining--;
  } while (factionsRemaining != 0);
  return;
}


/* Address: 0x00513CA0.
   Returns true (CF set) when bit otherFactionIndex is clear in factionIndex's capabilityFlags (record +0x3C).
   The mask holds one bit per faction: GameData_ResetDefaults sets the faction's own bit and bit 0, and
   GameFactionRuntime_ApplyPairwiseRelationTransition sets or clears the others, so a clear bit marks a faction
   this one is not friendly with (the AI treats its entities as foreign/hostile).
*/
bool GameFactionRuntime_TestCapabilityBitClear(uint32_t otherFactionIndex,FactionRuntimeIndex factionIndex)

{
  return (g_GameFactionRuntimeImage.records[factionIndex].capabilityFlags &
         1 << ((uint8_t)otherFactionIndex & 31)) == 0;
}


/* Address: 0x00513CD0.
   Returns the diplomatic relation state (0..11) of factionIndex towards otherFactionIndex: nibble
   otherFactionIndex of the faction record's packedRelationStates. States from 4 on count as friendly.
*/
FactionRelationState GameFactionRuntime_GetPackedStateNibble
          (FactionRuntimeIndex otherFactionIndex,FactionRuntimeIndex factionIndex)

{
  return g_GameFactionRuntimeImage.records[factionIndex].packedRelationStates >>
         ((uint8_t)(otherFactionIndex << 2) & 31) & 0xf;
}


/* Address: 0x00513D70.
   Technology exchange between related factions: for every unordered pair of factions 1..7 whose relation
   state is 8, 9 or 10, the lowest technology only the source faction has and the lowest technology only the
   other faction has are swapped (each side unlocks the other's). A pair where either side has nothing the
   other lacks exchanges nothing. Afterwards the other-player command entries are rebuilt.
*/
void GameFactionRuntime_SynchronizeTechnologiesForRelationStates8To10(void)

{
  int otherRecordBase;
  int carryBit;
  uint32_t bitMask;
  uint32_t maskWordIndex;
  TechnologyId otherTechnologyIndex;
  uint32_t otherFactionIndex;
  uint32_t sourceFactionIndex;
  TechnologyId sourceTechnologyIndex;
  GameFactionRuntimeRecord *sourceRecordBase;
  
  sourceRecordBase = &g_GameFactionRuntimeImage.records[1];
  for (sourceFactionIndex = 1; sourceFactionIndex < 7; sourceFactionIndex++) {
    /* otherRecordBase trails the other faction's record by one record, so the other record is
       ((GameFactionRuntimeRecord *)otherRecordBase)[1]. Its technology masks are read with explicit
       int arithmetic; the pointer form changes the register allocation. */
    otherRecordBase = (int)sourceRecordBase;
    for (otherFactionIndex = sourceFactionIndex + 1; otherFactionIndex < 8;
        otherFactionIndex++) {
      /* the other faction's relation-state nibble towards the source faction */
      switch(((GameFactionRuntimeRecord *)otherRecordBase)[1].packedRelationStates >> ((char)sourceFactionIndex *
                                                                                       4 & 31U) & 0xf) {
      case 8:
      case 9:
      case 10:
        /* First technology the source faction has and the other one lacks (bitMask rotates left, the carry
           advances maskWordIndex). */
        bitMask = 1;
        maskWordIndex = 0;
        sourceTechnologyIndex = 0;
        do {
          if (((sourceRecordBase->technologyMasks256Bits[maskWordIndex] & bitMask) !=
               0) &&
             ((*(uint32_t *)(otherRecordBase +
                             (int)(sizeof(GameFactionRuntimeRecord) +
                                  offsetof(GameFactionRuntimeRecord, technologyMasks256Bits)) +
                             maskWordIndex * 4) & bitMask) == 0)) break;
          carryBit = (int)bitMask >> 31;
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
            if (((*(uint32_t *)(otherRecordBase +
                               (int)(sizeof(GameFactionRuntimeRecord) +
                                    offsetof(GameFactionRuntimeRecord, technologyMasks256Bits)) +
                               maskWordIndex * 4) & bitMask) != 0) &&
               ((sourceRecordBase->technologyMasks256Bits[maskWordIndex] & bitMask) ==
                0)) {
              Technology_UnlockForFaction(0,0,otherTechnologyIndex,sourceFactionIndex);
              Technology_UnlockForFaction(0,0,sourceTechnologyIndex,otherFactionIndex);
              break;
            }
            carryBit = (int)bitMask >> 31;
            bitMask = bitMask << 1 | -carryBit;
            maskWordIndex = maskWordIndex + (-carryBit != 0);
            otherTechnologyIndex++;
          } while (maskWordIndex < 8);
        }
      }
      otherRecordBase = otherRecordBase + (int)sizeof(GameFactionRuntimeRecord);
    }
    sourceRecordBase++;
  }
  InGameOtherPlayerCommand_RebuildTargetEntries((UiNodeBase *)g_InGameRuntimeRoot);
  return;
}


/* Address: 0x00514510.
   "Under attack" alert for the faction owning a hit army (called by ShotRuntime_ApplyArmyHitRelationAndNotifications
   when a shot opens hostilities): moves the faction's primary or secondary alert anchor (chosen by definition
   dword +0x18) to the hit model and restarts its 150-tick cooldown. The alert movie 300 / 301 is only queued when
   the cooldown had fallen below 50 and the hit is more than 12 world units from the old anchor or the cooldown had
   run out, and only when the shown faction owns the hit model, so a sustained attack does not repeat the alert.
*/
void GameFactionRuntime_UpdateImpactAlertAnchorAndNotify
          (ModelRuntimeSlot *hitModelRuntime,WorldRuntimeContext *worldRuntime)

{
  /* the root model runtime of the hit model's army: [0] definition, [1] root model node, [2] owning army
     (+0xC faction) */
  int *ownershipRecord;
  int ownerFactionIndex;
  int hitModelAddress;
  FactionAnchorCooldownTicks previousCooldown;
  GraphicsWorldCoordinateQ12 anchorX;
  int distanceYOrLinkRecord;
  int distanceX;

  ownershipRecord =
       (int *)hitModelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime->modelRuntimeOrSavedOffset.modelRuntime;
  ownerFactionIndex = hitModelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex;
  hitModelAddress = ownershipRecord[1];
  if (((ModelRuntimeSlot *)ownershipRecord)->definitionOrSavedId.runtimeDefinition->accelerationPerTick == 0) {
    previousCooldown = g_GameFactionRuntimeImage.records[ownerFactionIndex].primaryAnchorCooldown;
    distanceYOrLinkRecord = g_GameFactionRuntimeImage.records[ownerFactionIndex].primaryAnchorYQ12 -
         ((ModelRuntimeNode *)hitModelAddress)->worldTransform.translation.x;
    if (distanceYOrLinkRecord < 0) {
      distanceYOrLinkRecord = -distanceYOrLinkRecord;
    }
    distanceX = g_GameFactionRuntimeImage.records[ownerFactionIndex].primaryAnchorXQ12 -
         ((ModelRuntimeNode *)hitModelAddress)->worldTransform.translation.y;
    if (distanceX < 0) {
      distanceX = -distanceX;
    }
    g_GameFactionRuntimeImage.records[ownerFactionIndex].primaryAnchorCooldown = 150;
    /* 0xC000 = 12.0 world units (Q12), measured as |dy| + |dx| */
    if (((int)previousCooldown < 50) && ((12 * Q12_ONE < distanceYOrLinkRecord + distanceX || (previousCooldown == 0)))) {
      anchorX = ((ModelRuntimeNode *)hitModelAddress)->worldTransform.translation.y;
      distanceYOrLinkRecord = ownershipRecord[2];
      g_GameFactionRuntimeImage.records[ownerFactionIndex].primaryAnchorYQ12 =
           ((ModelRuntimeNode *)hitModelAddress)->worldTransform.translation.x;
      g_GameFactionRuntimeImage.records[ownerFactionIndex].primaryAnchorXQ12 = anchorX;
      if (worldRuntime->activeFactionRuntimeIndex == ((ArmyRuntimeSlot *)distanceYOrLinkRecord)->factionIndex) {
        InGameNotificationQueue_InsertPriorityRecord
                  (FACTION_IMPACT_ANCHOR,0,(worldRuntime->motion).pitchAngle,
                   (worldRuntime->motion).headingAngle,
                   ((ModelRuntimeNode *)ownershipRecord[1])->worldTransform.translation.y,
                   ((ModelRuntimeNode *)ownershipRecord[1])->worldTransform.translation.x,8,300);
      }
    }
    else {
      anchorX = ((ModelRuntimeNode *)hitModelAddress)->worldTransform.translation.y;
      g_GameFactionRuntimeImage.records[ownerFactionIndex].primaryAnchorYQ12 =
           ((ModelRuntimeNode *)hitModelAddress)->worldTransform.translation.x;
      g_GameFactionRuntimeImage.records[ownerFactionIndex].primaryAnchorXQ12 = anchorX;
    }
  }
  else {
    previousCooldown = g_GameFactionRuntimeImage.records[ownerFactionIndex].anchorCooldown0;
    distanceYOrLinkRecord = g_GameFactionRuntimeImage.records[ownerFactionIndex].secondaryAnchorYQ12 -
         ((ModelRuntimeNode *)hitModelAddress)->worldTransform.translation.x;
    if (distanceYOrLinkRecord < 0) {
      distanceYOrLinkRecord = -distanceYOrLinkRecord;
    }
    distanceX = g_GameFactionRuntimeImage.records[ownerFactionIndex].secondaryAnchorXQ12 -
         ((ModelRuntimeNode *)hitModelAddress)->worldTransform.translation.y;
    if (distanceX < 0) {
      distanceX = -distanceX;
    }
    g_GameFactionRuntimeImage.records[ownerFactionIndex].anchorCooldown0 = 150;
    if (((int)previousCooldown < 50) && ((12 * Q12_ONE < distanceYOrLinkRecord + distanceX || (previousCooldown == 0)))) {
      anchorX = ((ModelRuntimeNode *)hitModelAddress)->worldTransform.translation.y;
      distanceYOrLinkRecord = ownershipRecord[2];
      g_GameFactionRuntimeImage.records[ownerFactionIndex].secondaryAnchorYQ12 =
           ((ModelRuntimeNode *)hitModelAddress)->worldTransform.translation.x;
      g_GameFactionRuntimeImage.records[ownerFactionIndex].secondaryAnchorXQ12 = anchorX;
      if (worldRuntime->activeFactionRuntimeIndex == ((ArmyRuntimeSlot *)distanceYOrLinkRecord)->factionIndex) {
        InGameNotificationQueue_InsertPriorityRecord
                  (FACTION_IMPACT_ANCHOR,0,(worldRuntime->motion).pitchAngle,
                   (worldRuntime->motion).headingAngle,
                   ((ModelRuntimeNode *)ownershipRecord[1])->worldTransform.translation.y,
                   ((ModelRuntimeNode *)ownershipRecord[1])->worldTransform.translation.x,7,301);
      }
    }
    else {
      anchorX = ((ModelRuntimeNode *)hitModelAddress)->worldTransform.translation.y;
      g_GameFactionRuntimeImage.records[ownerFactionIndex].secondaryAnchorYQ12 =
           ((ModelRuntimeNode *)hitModelAddress)->worldTransform.translation.x;
      g_GameFactionRuntimeImage.records[ownerFactionIndex].secondaryAnchorXQ12 = anchorX;
    }
  }
}


/* Address: 0x00514730.
   Recomputes one faction's statistics for the score / results screens: explored terrain percent, unlocked
   technologies beyond the five starting ones, extracted resource components, the economy and relation scores,
   the summed value of its army assets on the map, and the combined progress score.
*/
void GameFactionRuntime_RecomputeProgressAndScoreMetrics
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
  ArmyAssetRecordPrefix *resolvedAsset;
  FieldGridAsset *terrainGrid;
  WorldOwnerListNode *ownerNode;
  
  terrainGrid = worldRuntime->fieldGrid;
  cellOrTechnologyCount = terrainGrid->gridWidth * terrainGrid->gridHeight;
  tallyOrComponent = 0;
  /* one byte per faction at cell +0x70; bits 3-7 set = the faction has explored the cell (0x80-byte cells) */
  cellVisibilityCursor = (uint8_t *)&terrainGrid->cells[0].occupancyMask + factionIndex;
  cellsLeftOrBitOrRate = cellOrTechnologyCount;
  do {
    if ((*cellVisibilityCursor & FIELD_CELL_OCCUPANCY_EXPLORED_BITS) != 0) {
      tallyOrComponent++;
    }
    cellVisibilityCursor = cellVisibilityCursor + sizeof(FieldGridCell);
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
      if ((g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits[maskWordIndex] & cellsLeftOrBitOrRate) != 0) {
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
       (int)(tallyOrComponent * 16 + tritiumComponentOrModelRecord * 8 + technologyCountBeyondBaseline * (10 * Q12_ONE) +
             exploredPercent * Q12_ONE) >> Q12_SHIFT;
  g_GameFactionRuntimeImage.records[factionIndex].relationScore =
       (g_GameFactionRuntimeImage.records[factionIndex].relationCounterB * (40 * Q12_ONE) +
       g_GameFactionRuntimeImage.records[factionIndex].relationCounterD * (-20 * Q12_ONE) +
       g_GameFactionRuntimeImage.records[factionIndex].relationCounterA * (20 * Q12_ONE) +
       g_GameFactionRuntimeImage.records[factionIndex].relationCounterC * (-10 * Q12_ONE) +
       g_GameFactionRuntimeImage.records[factionIndex].relationCounterE * (20 * Q12_ONE) +
       g_GameFactionRuntimeImage.records[factionIndex].relationCounterF * (40 * Q12_ONE)) >> Q12_SHIFT;
  /* sum the army-asset dword +0x28 over every model of this faction on the map */
  tallyOrComponent = 0;
  for (ownerNode = worldRuntime->ownerListHead; ownerNode != NULL;
      ownerNode = ownerNode->nextNode) {
    if ((ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
       (tritiumComponentOrModelRecord =
        (int)((ModelRuntimeSlot *)ownerNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime,
       factionIndex == ((ArmyRuntimeSlot *)tritiumComponentOrModelRecord)->factionIndex)) {
      if (ArmyAssetRegistry_FindById(((ArmyRuntimeSlot *)tritiumComponentOrModelRecord)->armyAssetId,&resolvedAsset) == 0) {
        tallyOrComponent = tallyOrComponent + resolvedAsset[2].registryId;
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
   Finds which of its faction's eight runtime groups (32 member slots each) holds runtimeEntry, for the group
   selection commands in gameplay/selection/runtime. Returns the one-based group number (1..8), or 0 when it is
   in no group.
*/
uint32_t GameFactionRuntime_FindRuntimeGroupNumber(RuntimeModelFactionPrefix *runtimeEntry)

{
  int slotsRemaining;
  uint32_t groupNumber;
  ArmyRuntimeSlot **slotCursor;
  ArmyRuntimeSlot **nextSlotCursor;
  bool found;

  groupNumber = 0;
  nextSlotCursor = g_GameFactionRuntimeImage.records[runtimeEntry->factionIndex].runtimeGroupMembers8x32;
  do {
    slotsRemaining = 32;
    groupNumber++;
    found = groupNumber == 0; /* the ZF of the original INC; never true */
    slotCursor = nextSlotCursor;
    do {
      nextSlotCursor = slotCursor;
      if (slotsRemaining == 0) break;
      slotsRemaining--;
      nextSlotCursor = slotCursor + 1;
      found = (ArmyRuntimeSlot *)runtimeEntry == *slotCursor;
      slotCursor = nextSlotCursor;
    } while (!found);
    if (found) {
      return groupNumber;
    }
  } while (groupNumber <= 7);
  return 0;
}


/* Address: 0x0051B800.
   Checks whether the faction already has armyAssetRecord pending: in its secondary army-asset list, or in
   production in one of its class 0x0B/0x0D structures (state word 0x2E == 1). The result is
   false (CF clear) when found, true (CF set) when not.
*/
bool FactionRuntime_IsArmyAssetNotPending
          (FactionRuntimeIndex factionIndex,ArmyAssetRecordPrefix *armyAssetRecord)

{
  ArmyAssetRecordPrefix *activeAssetRecord;
  WorldOwnerListNode *ownerNode;
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
      assetsRemaining--;
      matched = armyAssetRecord == (ArmyAssetRecordPrefix *)*assetIdCursor;
      assetIdCursor++;
    } while (!matched);
    if (matched) {
      return false;
    }
  }
  /* Structures of class 0xB or 0xD in state 1 that are currently producing armyAssetRecord. */
  for (ownerNode = g_InGameRuntimeRoot->worldRuntime.ownerListHead;
      ownerNode != NULL; ownerNode = ownerNode->nextNode) {
    if ((ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
       (modelPayload = ownerNode->runtimePayload,
       factionIndex == ((ModelRuntimeSlot *)modelPayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex) &&
       ((((ModelRuntimeSlot *)modelPayload)->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_11) ||
        (((ModelRuntimeSlot *)modelPayload)->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_13)) &&
       (modelPayload[46] == 1)) {
      activeAssetRecord = (ArmyAssetRecordPrefix *)modelPayload[24];
      if (armyAssetRecord == activeAssetRecord) {
        return false;
      }
    }
  }
  return true;
}


/* Address: 0x0051C4C0.
   Stops an entity where it stands (used by the stop command on the selection): clears the command flags
   0x01, 0x08, 0x10 and 0x20 and sets the path target and both tracked coordinate pairs to the current
   x/y position of its model.
*/
void GameEntityRuntime_ResetMovementFlagsAndAnchorCoordinatesFromModel(GameEntityRuntime *entityRuntime)

{
  GameEntityCommandFlags *commandFlagsField;
  ModelRuntimeNode *ownerModelNode;
  GraphicsWorldCoordinateQ12 modelX;
  GraphicsWorldCoordinateQ12 modelY;

  ownerModelNode = (entityRuntime->common).ownership.modelNode;
  commandFlagsField = &(entityRuntime->common).commandFlags;
  *commandFlagsField = *commandFlagsField &
                      ~(uint32_t)(ARMY_MOVEMENT_ACTIVE | ARMY_MOVEMENT_WAYPOINTS_QUEUED |
                                  ARMY_MOVEMENT_ROUTE_POINT_REACHED | ARMY_MOVEMENT_TARGET_FOLLOWING);
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
   Where an entity's current command should take it, for the movement code in gameplay/army/movement: target flag
   1 aims at a target entity (its model position, raised by definition dword +0x50; class 0x15 aims at its first
   child node), flag 2 at a fixed world position. A target entity that the owner's faction can no longer see is
   dropped (entity and flags cleared). Writes the position to *outPosition and returns true, or returns false
   when there is none.
*/
bool GameEntityRuntime_ResolveCommandTargetPosition(GameEntityRuntime *targetState,FixedVectorQ12 *outPosition)

{
  GameEntityRuntime *commandTargetEntity;
  uint32_t visibilityMask;
  int *targetDefinitionRecord;
  ModelRuntimeNode *targetModelNode;

  /* Original quirk: on failure the original leaves whatever is in EAX/ECX/EDX at that point (the caller's values
     or the partial visibility mask / owner shift / definition pointer), and one caller still copies the Z
     register into a local. The port writes zeros instead, so *outPosition is always written. */
  outPosition->xQ12 = 0;
  outPosition->yQ12 = 0;
  outPosition->zQ12 = 0;
  if (((targetState->common).commandTarget.targetFlags & 1) == 0) {
    if (((targetState->common).commandTarget.targetFlags & 2) != 0) {
      outPosition->xQ12 = (targetState->common).commandTarget.targetWorldXQ12;
      outPosition->yQ12 = (targetState->common).commandTarget.targetWorldYQ12;
      outPosition->zQ12 = (targetState->common).commandTarget.targetWorldZQ12;
      return true;
    }
  }
  else {
    commandTargetEntity = (targetState->common).commandTarget.targetEntity;
    if (commandTargetEntity != NULL) {
      /* two bits per faction; the upper one = the target is visible to that faction */
      visibilityMask = 2u << ((uint8_t)((targetState->common).ownership.ownerIndex * 2) & 31);
      targetDefinitionRecord = (commandTargetEntity->common).ownership.definitionOrClassRecord;
      if (((commandTargetEntity->common).damageState.factionVisibilityBits1C & visibilityMask) != 0) {
        targetModelNode = (commandTargetEntity->common).ownership.modelNode;
        if (((ModelRuntimeSlot *)targetDefinitionRecord)->definitionOrSavedId.runtimeDefinition->runtimeClassId ==
            MODEL_RUNTIME_CLASS_21_AIRCRAFT) {
          targetModelNode = targetModelNode->childNodes[0];
        }
        outPosition->xQ12 = (targetModelNode->worldTransform).translation.x;
        outPosition->yQ12 = (targetModelNode->worldTransform).translation.y;
        outPosition->zQ12 =
             (targetModelNode->worldTransform).translation.z +
             ((ModelRuntimeSlot *)targetDefinitionRecord)->definitionOrSavedId.runtimeDefinition->aimHeightOffsetQ12;
        return true;
      }
      (targetState->common).commandTarget.targetEntity = NULL;
      (targetState->common).commandTarget.targetFlags = 0;
    }
  }
  return false;
}


/* Address: 0x0052A4D0.
   Applies impactValue to an entity's integrity (called twice per hit by ArmyRuntime_ApplyImpactDamageToRuntimeAndParent;
   a negative value repairs and goes to the entity its runtime link points at). A destroyed entity passes the
   overkill on to its parent model's army, or, without a parent, is turned to the impact angle (definition class 0
   without +0x278) and counted in the score counters: a loss for its faction, a kill for sourceFactionIndex (the
   heavier counters D/F instead of C/E for classes handled by ArmyRuntime_ClassCommandHandlerGroupA). Repair beyond
   the definition maximum (+0x60) is clamped and the excess handed to the first linked army that is not at full
   integrity.
*/
void GameEntityRuntime_ApplyImpactDamageAndFactionRelationState
          (AngleTurn32 impactAngle,FactionRuntimeIndex sourceFactionIndex,
          ImpactDamageValue32 impactValue,GameEntityRuntime *targetEntityRuntime)

{
  int *integrityField;
  GameEntityRuntimeFlags *runtimeFlagsField;
  FactionRelationCounter *relationCounter;
  ModelRuntimeSlot *attachedModelRuntime;
  void *definitionRecord;
  int maxIntegrityOrClassOrCount;
  int integrityDeltaOrFaction;
  ModelRuntimeNode *modelOrParentNode;

  if (impactValue < 0) {
    targetEntityRuntime = *(GameEntityRuntime **)(targetEntityRuntime->common).ownership.runtimeLink;
  }
  (targetEntityRuntime->common).pathingAndImpactState.impactReaction.state08 = 0;
  (targetEntityRuntime->common).pathingAndImpactState.impactReaction.reactionCode09 = 2;
  (targetEntityRuntime->common).pathingAndImpactState.impactReaction.state0A = 0;
  (targetEntityRuntime->common).pathingAndImpactState.impactReaction.state0B = 0;
  if (0 < (targetEntityRuntime->common).damageState.remainingIntegrity) {
    maxIntegrityOrClassOrCount =
         ((ModelDefinition *)(targetEntityRuntime->common).ownership.definitionOrClassRecord)->
         maximumHealth;
    integrityField = &(targetEntityRuntime->common).damageState.remainingIntegrity;
    integrityDeltaOrFaction = *integrityField;
    *integrityField = *integrityField - impactValue;
    /* JLE after the SUB: the entity is destroyed */
    if (integrityDeltaOrFaction <= impactValue) {
      modelOrParentNode = (targetEntityRuntime->common).ownership.modelNode;
      integrityDeltaOrFaction = (targetEntityRuntime->common).damageState.remainingIntegrity;
      runtimeFlagsField = &(targetEntityRuntime->common).runtimeFlags;
      *runtimeFlagsField = *runtimeFlagsField | 8;
      modelOrParentNode = modelOrParentNode->parentNode;
      (targetEntityRuntime->common).damageState.counterOrTerminalReference.terminalEntity =
           targetEntityRuntime;
      (targetEntityRuntime->common).damageState.remainingIntegrity = 0;
      if (modelOrParentNode == NULL) {
        definitionRecord = (targetEntityRuntime->common).ownership.definitionOrClassRecord;
        if ((((ModelDefinition *)definitionRecord)->runtimeClassId == 0) &&
           (((ModelDefinition *)definitionRecord)->placementContactKindIndex == 0)) {
          (((targetEntityRuntime->common).ownership.modelNode)->modelPayload).worldRotationAngle0 =
               impactAngle;
        }
        if (impactValue != 0) {
          /* The original tests ZF after IMUL EBX,[EDI+0xC],0x740 (0x0052A5DB / JZ 0x0052A630 at 0x0052A5E2).
             IMUL leaves ZF unchanged (measured on an AMD Zen 3) and ZF still holds CMP [EBP+0x28],0 at
             0x0052A5D5, whose own JZ already left for impactValue == 0; so ZF is clear here, the jump is never
             taken and the counters are always updated. The C follows that. */
          integrityDeltaOrFaction =
               ((ArmyRuntimeSlot *)(targetEntityRuntime->common).ownership.runtimeLink)->factionIndex;
          maxIntegrityOrClassOrCount =
               ((ModelDefinition *)(targetEntityRuntime->common).ownership.definitionOrClassRecord)->
               runtimeClassId;
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
            *relationCounter = *relationCounter - 1;
            relationCounter = &g_GameFactionRuntimeImage.records[sourceFactionIndex].relationCounterE;
            *relationCounter = *relationCounter - 1;
          }
        }
      }
      else {
        /* the overkill goes on to the parent's army */
        ArmyRuntime_ApplyDamageAndPropagateToParent(-integrityDeltaOrFaction,
                                                    (modelOrParentNode->runtimePayload).modelRuntime);
      }
    }
    else {
      integrityField = &(targetEntityRuntime->common).damageState.remainingIntegrity;
      integrityDeltaOrFaction = maxIntegrityOrClassOrCount - *integrityField;
      if (integrityDeltaOrFaction == 0 || maxIntegrityOrClassOrCount < *integrityField) {
        /* repaired to or beyond the maximum: clamp, the (negative) excess repairs a linked army */
        integrityField = &(targetEntityRuntime->common).damageState.remainingIntegrity;
        *integrityField = *integrityField + integrityDeltaOrFaction;
        /* dword +0x0C is the model runtime's attachment count here, the attached child model runtimes are at
           +0x140 with a stride of 0x20 */
        attachedModelRuntime = (targetEntityRuntime->classPayload).impactOwnerLinks.attachment0ChildModelRuntime;
        maxIntegrityOrClassOrCount = (targetEntityRuntime->common).ownership.ownerIndex;
        for (; maxIntegrityOrClassOrCount != 0; maxIntegrityOrClassOrCount--) {
          /* the child's health (+0x3C) against its definition's maximumHealth (+0x60): not at full health */
          if ((attachedModelRuntime != NULL) &&
             (attachedModelRuntime->health !=
              attachedModelRuntime->definitionOrSavedId.runtimeDefinition->maximumHealth)) {
            ArmyRuntime_ApplyDamageAndPropagateToParent(integrityDeltaOrFaction,attachedModelRuntime);
            return;
          }
          attachedModelRuntime = (targetEntityRuntime->classPayload).impactOwnerLinks.attachment1ChildModelRuntime;
          targetEntityRuntime =
               (GameEntityRuntime *)&(targetEntityRuntime->common).commandTarget.targetWorldXQ12;
        }
      }
    }
  }
}


/* Address: 0x00560110.
   Queues repetitionCount units of an army record for a faction (the build buttons of the in-game catalog and
   the AI): appends the registry pointer of armyAssetId that many times to the faction's secondary army-asset
   list, stopping when its 64 entries are full. An unknown id queues nothing.
*/
void GameFactionRuntime_RegisterArmyAssetPointers(uint32_t unusedPlayerRuntimeId,FactionArmyAssetCount repetitionCount,
          PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex)

{
  FactionArmyAssetCount *secondaryCount;
  FactionArmyAssetCount slotIndex;
  ArmyAssetRecordPrefix *resolvedAsset;

  if (ArmyAssetRegistry_FindById(armyAssetId,&resolvedAsset) == 0) {
    slotIndex = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount;
    do {
      if (FACTION_ARMY_ASSET_LIST_CAPACITY - 1 < slotIndex) {
        return;
      }
      g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetPointersOrIds[slotIndex] =
           (uint32_t)resolvedAsset;
      secondaryCount = &g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount;
      *secondaryCount = *secondaryCount + 1;
      slotIndex++;
      repetitionCount--;
    } while (repetitionCount != 0);
  }
  return;
}


/* Address: 0x00560160.
   In-game command INGAME_COMMAND_CANCEL_QUEUED_ARMY (the cancel click on a build button of the in-game catalog,
   the reverse of GameFactionRuntime_RegisterArmyAssetPointers): cancels up to requestedCount orders of an army
   record. Waiting orders are taken out of the faction's production queue first (without a refund); what is left is cancelled in the faction's producing structures (class 0x0B, 0x16 or 0x0D, chosen by the
   army's flags 0x10 / 0x08), which stop production and refund the full price to the faction's xenite.
*/
void GameFactionRuntime_CancelQueuedArmyAssetsAndRefund
          (uint32_t unusedPlayerRuntimeId,FactionArmyAssetCount requestedCount,
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
  WorldOwnerListNode *ownerNode;

  if (ArmyAssetRegistry_FindById(armyAssetId,&armyDefinition) == 0) {
    recordOffset = factionIndex * (int)sizeof(GameFactionRuntimeRecord);
    readIndexOrClassId = 0;
    writeIndex = 0;
    /* compact the queue (secondaryArmyAssetPointersOrIds, record +0xE0), dropping the first matching entries */
    for (assetsRemaining = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount; assetsRemaining !=
         0;
        assetsRemaining--) {
      while ((armyDefinition ==
              *(ArmyAssetRecordPrefix **)
               (recordOffset + (uintptr_t)g_GameFactionRuntimeImage.records[0].secondaryArmyAssetPointersOrIds +
               readIndexOrClassId * 4) &&
             (0 < (int)requestedCount))) {
        readIndexOrClassId++;
        secondaryCount = &g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount;
        *secondaryCount = *secondaryCount - 1;
        requestedCount--;
        assetsRemaining--;
        if (assetsRemaining == 0) goto cancelInStructures;
      }
      *(uint32_t *)(recordOffset + (uintptr_t)g_GameFactionRuntimeImage.records[0].secondaryArmyAssetPointersOrIds +
                    writeIndex * 4) =
           *(uint32_t *)(recordOffset +
                         (uintptr_t)g_GameFactionRuntimeImage.records[0].secondaryArmyAssetPointersOrIds +
                        readIndexOrClassId * 4);
      readIndexOrClassId++;
      writeIndex++;
    }
cancelInStructures:
    if (requestedCount != 0) {
      ownerNode = (g_InGameRuntimeRoot->worldRuntime).ownerListHead;
      if (ownerNode != NULL) {
        /* the structure class that produces this army */
        readIndexOrClassId = MODEL_RUNTIME_CLASS_11;
        if (((armyDefinition[1].selectionDetailTemplateVariantIndex & ARMY_ASSET_FLAG_BUILT_BY_CLASS11) == 0) &&
           (readIndexOrClassId = MODEL_RUNTIME_CLASS_22, (armyDefinition[1].selectionDetailTemplateVariantIndex & ARMY_ASSET_FLAG_BUILT_AT_AIRCRAFT_PAD) == 0)) {
          readIndexOrClassId = MODEL_RUNTIME_CLASS_13;
        }
        do {
          if (ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
            modelPayload = ownerNode->runtimePayload;
            if ((factionIndex ==
                 ((ModelRuntimeSlot *)modelPayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex) &&
               (readIndexOrClassId ==
                ((ModelRuntimeSlot *)modelPayload)->definitionOrSavedId.runtimeDefinition->runtimeClassId))
            {
              /* payload [0x18] = army in production; stopping clears it with its state word and flag 0x100 of
                 [0x3B] and takes its share [0x1D] back out of [0x3D] */
              if (readIndexOrClassId == MODEL_RUNTIME_CLASS_13) {
                if (((((uint32_t)((ModelRuntimeSlot *)modelPayload)->definitionOrSavedId.runtimeDefinition->classParameterC4 &
                      armyDefinition[1].selectionDetailTemplateVariantIndex) != 0) &&
                    (armyDefinition->registryId == modelPayload[24])) && (modelPayload[46] == 1)) {
                  modelPayload[61] = modelPayload[61] - modelPayload[29];
                  refundAmount = armyDefinition[2].registryId;
                  modelPayload[29] = 0;
                  modelPayload[46] = 0;
                  modelPayload[59] = modelPayload[59] & ~(uint32_t)ARMY_MODEL_STATE_PRODUCING;
                  modelPayload[24] = 0;
                  g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 =
                       g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 + refundAmount;
                  requestedCount--;
                  if (requestedCount == 0) {
                    return;
                  }
                }
              }
              else if (readIndexOrClassId == MODEL_RUNTIME_CLASS_11) {
                if (armyDefinition->registryId == modelPayload[24]) {
                  modelPayload[61] = modelPayload[61] - modelPayload[29];
                  refundAmount = armyDefinition[2].registryId;
                  modelPayload[29] = 0;
                  modelPayload[46] = 0;
                  modelPayload[59] = modelPayload[59] & ~(uint32_t)ARMY_MODEL_STATE_PRODUCING;
                  modelPayload[24] = 0;
                  g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 =
                       g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 + refundAmount;
                  requestedCount--;
                  if (requestedCount == 0) {
                    return;
                  }
                }
              }
              else if (armyDefinition->registryId == modelPayload[24]) {
                modelPayload[61] = modelPayload[61] - modelPayload[29];
                refundAmount = armyDefinition[2].registryId;
                modelPayload[29] = 0;
                modelPayload[43] = 0;
                modelPayload[59] = modelPayload[59] & ~(uint32_t)ARMY_MODEL_STATE_PRODUCING;
                modelPayload[24] = 0;
                g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 =
                     g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 + refundAmount;
                requestedCount--;
                if (requestedCount == 0) {
                  return;
                }
              }
            }
          }
          ownerNode = ownerNode->nextNode;
        } while (ownerNode != NULL);
      }
    }
  }
}


/* Address: 0x00560400.
   In-game command INGAME_COMMAND_TAKE_ARMY_FOR_PLACEMENT (clicking a finished army in the in-game catalog):
   takes the first entry of the army record out of the faction's primary army-asset list (record +0x1E0) and
   stages it in the player's pending slot (+0x8098) for placement on the map. When the faction is the one shown
   the command sprite grid is rebuilt, and for the local player the placement cursor is armed. If the faction
   has no such entry the pending slot is cleared again.
*/
void GameFactionRuntime_RemoveArmyAssetAndStagePlayerTransfer
          (PlayerRuntimeId playerRuntimeId,uint32_t unusedZero,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex)

{
  FactionArmyAssetCount *primaryCount;
  SelectionPlayerRuntimeBlock *playerBlock;
  InGameRuntimeRoot *runtimeRoot;
  ArmyAssetRecordPrefix *armyDefinition;
  int byteOffsetOrActiveFaction;
  FactionArmyAssetCount assetsRemaining;

  if (ArmyAssetRegistry_FindById(armyAssetId,&armyDefinition) == 0) {
    playerBlock = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId];
    byteOffsetOrActiveFaction = factionIndex * (int)sizeof(GameFactionRuntimeRecord);
    assetsRemaining = g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
    playerBlock->pendingPlacementArmyAsset = (uint32_t)armyDefinition;
    for (; assetsRemaining != 0; assetsRemaining--) {
      if (armyDefinition ==
          *(ArmyAssetRecordPrefix **)
           ((int)g_GameFactionRuntimeImage.records[0].primaryArmyAssetPointersOrIds + byteOffsetOrActiveFaction)) {
        /* close the gap */
        do {
          *(uint32_t *)
           ((int)g_GameFactionRuntimeImage.records[0].primaryArmyAssetPointersOrIds + byteOffsetOrActiveFaction) =
               *(uint32_t *)
                ((int)g_GameFactionRuntimeImage.records[0].primaryArmyAssetPointersOrIds + byteOffsetOrActiveFaction + 4
                );
          runtimeRoot = g_InGameRuntimeRoot;
          byteOffsetOrActiveFaction += 4;
          assetsRemaining--;
        } while (assetsRemaining != 0);
        byteOffsetOrActiveFaction = (g_InGameRuntimeRoot->worldRuntime).activeFactionRuntimeIndex;
        primaryCount = &g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
        *primaryCount = *primaryCount - 1;
        if (factionIndex != byteOffsetOrActiveFaction) {
          return;
        }
        InGameArmyStock_RebuildGrid((UiNodeBase *)runtimeRoot);
        if (playerRuntimeId != g_LocalPlayerRuntimeId) {
          return;
        }
        g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | UI_COMMAND_RUNTIME_FLAG_PLACEMENT_PENDING;
        g_InGamePendingPlacementArmyAsset = (int32_t)armyDefinition;
        g_InGamePlacementSurfaceHeightQ12OrSentinel = INT32_MAX; /* no surface picked yet */
        return;
      }
      byteOffsetOrActiveFaction += 4;
    }
    playerBlock->pendingPlacementArmyAsset = 0;
  }
}


/* Address: 0x00560620.
   In-game command handler, the reverse of GameFactionRuntime_RemoveArmyAssetAndStagePlayerTransfer: takes the
   army asset staged for the player's placement (an XCHG with 0) back into the faction's primary army-asset list
   (at most 64 entries). When the faction is the one shown it rebuilds the command sprite grid, and for the local
   player it ends the pending placement.
*/
void GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid
          (PlayerRuntimeId playerRuntimeId,uint32_t unusedConsumeArgument0,uint32_t unusedConsumeArgument1
          ,FactionRuntimeIndex factionIndex)

{
  FactionArmyAssetCount *primaryCount;
  uint32_t pendingAsset;
  uint32_t assetCount;
  InGameRuntimeRoot *runtimeRoot;

  LOCK();
  pendingAsset = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->pendingPlacementArmyAsset;
  g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->pendingPlacementArmyAsset = 0;
  runtimeRoot = g_InGameRuntimeRoot;
  UNLOCK();
  if (pendingAsset != 0) {
    assetCount = g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
    if (assetCount < FACTION_ARMY_ASSET_LIST_CAPACITY) {
      /* primaryArmyAssetPointersOrIds[assetCount] (record +0x1E0) */
      g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetPointersOrIds[assetCount] = pendingAsset;
      primaryCount = &g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
      *primaryCount = *primaryCount + 1;
    }
    if (factionIndex == runtimeRoot->worldRuntime.activeFactionRuntimeIndex) {
      InGameArmyStock_RebuildGrid((UiNodeBase *)runtimeRoot);
      if (playerRuntimeId == g_LocalPlayerRuntimeId) {
        g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & ~UI_COMMAND_RUNTIME_FLAG_PLACEMENT_PENDING;
        g_InGamePendingPlacementArmyAsset = 0;
      }
    }
  }
  return;
}


/* Address: 0x005606A0.
   In-game command INGAME_COMMAND_SELL_ARMY (the sell click on a finished army in the in-game catalog): takes the
   first entry of the army record out of the faction's primary army-asset list (record +0x1E0) and credits 7/8 of
   its price to the faction's xenite. The command sprite grid is rebuilt when the faction is the one shown.
*/
void GameFactionRuntime_SellArmyAssetAndRefundSevenEighths
          (uint32_t unusedPlayerRuntimeId,uint32_t unusedZero,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex)

{
  FactionArmyAssetCount *primaryCount;
  PckArmyAssetIdCatalog price;
  InGameRuntimeRoot *runtimeRoot;
  int byteOffsetOrActiveFaction;
  FactionArmyAssetCount assetsRemaining;
  ArmyAssetRecordPrefix *resolvedAsset;

  if (ArmyAssetRegistry_FindById(armyAssetId,&resolvedAsset) == 0) {
    byteOffsetOrActiveFaction = factionIndex * (int)sizeof(GameFactionRuntimeRecord);
    for (assetsRemaining = g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount; assetsRemaining != 0;
        assetsRemaining--) {
      if (resolvedAsset ==
          *(ArmyAssetRecordPrefix **)
           ((int)g_GameFactionRuntimeImage.records[0].primaryArmyAssetPointersOrIds + byteOffsetOrActiveFaction)) {
        /* close the gap */
        do {
          *(uint32_t *)
           ((int)g_GameFactionRuntimeImage.records[0].primaryArmyAssetPointersOrIds + byteOffsetOrActiveFaction) =
               *(uint32_t *)
                ((int)g_GameFactionRuntimeImage.records[0].primaryArmyAssetPointersOrIds + byteOffsetOrActiveFaction + 4
                );
          runtimeRoot = g_InGameRuntimeRoot;
          byteOffsetOrActiveFaction += 4;
          assetsRemaining--;
        } while (assetsRemaining != 0);
        price = resolvedAsset[2].registryId;
        byteOffsetOrActiveFaction = (g_InGameRuntimeRoot->worldRuntime).activeFactionRuntimeIndex;
        primaryCount = &g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
        *primaryCount = *primaryCount - 1;
        g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 =
             g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 +
             ((int)(price * 7) >> 3);
        if (byteOffsetOrActiveFaction != factionIndex) {
          return;
        }
        InGameArmyStock_RebuildGrid((UiNodeBase *)runtimeRoot);
        return;
      }
      byteOffsetOrActiveFaction += 4;
    }
  }
}


/* Address: 0x00561F80.
   In-game command INGAME_COMMAND_PLACEMENT_CREATE_ARMY (map click while placing an army in command mode 3/4, from
   InGameUiCommand_BeginInteractionByMode): creates army armyAssetId at the clicked position for the faction set
   by PlayerRuntime_SetPlacementFaction and keeps it as the player's placed army (+0x8094, as an offset from
   g_ArmyRuntimeRebaseBaseMinusOne), or 0 when it could not be created.
*/
void PlayerRuntime_CreatePlacementArmy(PlayerRuntimeId playerRuntimeId,PlayerStateLookupValue0 worldXQ12,
          PlayerStateLookupValue1 worldYQ12,RuntimeToken armyAssetId)

{
  SelectionPlayerRuntimeBlock *playerBlock;
  ArmyRuntimeSlot *createdRuntime;

  playerBlock = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId];
  createdRuntime = ArmyRuntime_CreateInstanceFromAsset
                    (ARMY_CREATE_UNLOCK_TECHNOLOGY,0,worldXQ12,worldYQ12,playerBlock->placementFactionIndex,
                     armyAssetId,
                     &g_InGameRuntimeRoot->worldRuntime,NULL);
  if (createdRuntime != NULL) {
    playerBlock->placedArmyToken = (uint32_t)createdRuntime -
         (int)g_ArmyRuntimeRebaseBaseMinusOne;
    return;
  }
  playerBlock->placedArmyToken = 0;
}


/* Address: 0x00561FF0.
   In-game command INGAME_COMMAND_PLACEMENT_SET_FACTION (from InGameUiCommand_BeginInteractionByMode, before
   INGAME_COMMAND_PLACEMENT_CREATE_ARMY): sets the faction (+0x8090) that the player's next placed army belongs to.
*/
void PlayerRuntime_SetPlacementFaction(PlayerRuntimeId playerRuntimeId,uint32_t unusedZero0,uint32_t unusedZero1,
          PlacementFactionIndex placementFactionIndex)

{
  g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->placementFactionIndex = placementFactionIndex;
}


/* Address: 0x00562020.
   In-game command INGAME_COMMAND_PLACEMENT_SET_ARMY (clicking an existing army in placement sub-mode 2, from
   InGameUiCommand_BeginInteractionByMode): makes it the player's placed army (+0x8094); armyToken is its offset from
   g_ArmyRuntimeRebaseBaseMinusOne.
*/
void PlayerRuntime_SetPlacementArmy(PlayerRuntimeId playerRuntimeId,uint32_t unusedZero0,uint32_t unusedZero1,
          PlacedArmyToken armyToken)

{
  g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->placedArmyToken = armyToken;
}


/* Address: 0x005622C0.
   In-game command INGAME_COMMAND_PLACEMENT_CLEAR_ARMY (end of a placement interaction, from
   InGameUiCommand_EndInteractionByMode): forgets the player's placed army (+0x8094).
*/
void PlayerRuntime_ClearPlacementArmy(PlayerRuntimeId playerRuntimeId,uint32_t unusedZero0,uint32_t unusedZero1,
          uint32_t unusedZero2)

{
  g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->placedArmyToken = 0;
}


/* Address: 0x00565590.
   Applies the mission carry-over stored by OldUnitRuntime_RebuildScenarioReplayTables at the start of the next
   mission: ORs each faction's saved technology masks into its record, recreates every carried-over unit
   (0x20-byte primary records: asset id, faction, position, rotation) in the world, then rebuilds terrain
   occupancy and the cell classification for the active faction.
*/
void OldUnitRuntime_MergeMasksAndReplayRecords(void)

{
  InGameRuntimeRoot *runtimeRoot;
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
  /* 8 factions x 8 dwords; the faction records are 0x740 bytes (0x1D0 dwords) apart */
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
    nextMaskCursor = maskCursor + GAME_FACTION_RUNTIME_RECORD_BYTES / 4 - 7;
  } while (factionsRemaining != 0);
  if ((g_InGameRuntimeRoot != NULL) && (g_OldUnitRecordCount != 0)) {
    worldRuntime = &g_InGameRuntimeRoot->worldRuntime;
    recordsRemaining = g_OldUnitRecordCount;
    primaryRecordCursor = g_OldUnitPrimaryTable;
    do {
      /* record [2] and [3] go to the parameters named worldYQ12 / worldXQ12 (pushed as in the original,
         0x005655FC/0x005655FF), although the rebuild stores the X coordinate in [2] */
      ArmyRuntime_CreateInstanceFromAsset
                (ARMY_CREATE_COUNT_FOR_ACTIVE_FACTION | ARMY_CREATE_UNLOCK_TECHNOLOGY,primaryRecordCursor[4],
                 primaryRecordCursor[3],primaryRecordCursor[2],primaryRecordCursor[1],*primaryRecordCursor,
                      worldRuntime,NULL);
      primaryRecordCursor = primaryRecordCursor + 8; /* 0x20-byte records */
      recordsRemaining--;
    } while (recordsRemaining != 0);
    WorldRuntime_ForEachOwnerListNode
              (worldRuntime,ArmyRuntimeNode_AccumulateTerrainOcclusionAndOccupancyCallback,
               worldRuntime);
    WorldRuntime_ForEachOwnerListNode
              (worldRuntime,ArmyRuntimeNode_RebuildTerrainOccupancyAndVisualStateCallback,
               worldRuntime);
    FieldGrid_ClassifyCellFlagsToRuntimeByte
              ((runtimeRoot->worldRuntime).activeFactionRuntimeIndex,
               (runtimeRoot->worldRuntime).fieldGrid);
  }
  return;
}


/* Address: 0x00513D00.
   Returns true (CF) when the relation of factionIndex towards otherFactionIndex is in one of the pending states
   2, 5 or 9 and the pair's last relation change is at most 600 ticks old, so the relation does not advance
   again too soon.
*/
bool GameFactionRuntime_IsRecentTimedRelationState
          (FactionRuntimeIndex otherFactionIndex,FactionRuntimeIndex factionIndex)

{
  uint32_t relationStateNibble;

  relationStateNibble =
       g_GameFactionRuntimeImage.records[factionIndex].packedRelationStates >>
       ((char)otherFactionIndex * 4 & 31U) & 0xf;
  /* record +0x700 + 4 * other faction: tick of the pair's last relation change */
  if ((((relationStateNibble == 2) || (relationStateNibble == 5)) || (relationStateNibble == 9)) &&
     ((int)(g_GameFactionRuntimeImage.tail.simulationTick -
           (int)g_GameFactionRuntimeImage.records[factionIndex].relationStateTicks[otherFactionIndex]) <
      FACTION_RELATION_CHANGE_COOLDOWN_TICKS + 1)) {
    return true;
  }
  return false;
}


/* Address: 0x00565650.
   Drops any pending mission carry-over: clears the 64-dword technology-mask table and the unit-record count, so
   OldUnitRuntime_MergeMasksAndReplayRecords has nothing to apply.
*/
void OldUnitRuntime_ResetPendingTables(void)

{
  int tableEntriesRemaining;
  uint32_t *tableCursor;
  
  tableCursor = g_OldUnitSecondaryTable;
  for (tableEntriesRemaining = 64; tableEntriesRemaining != 0; tableEntriesRemaining--) {
    *tableCursor = 0;
    tableCursor++;
  }
  g_OldUnitRecordCount = 0;
  return;
}


/* Address: 0x00513EE0.
   Changes the diplomatic relation between two factions: notifies the shown faction (text code + 500), stores
   both directed 4-bit relation states, sets or clears the factions' friendly bits in each other's
   capabilityFlags (both by stateSecondTowardFirst: friendly from state 4 on) and stamps the change tick.
   State 11 merges the factions: one of them (see below) gives its units, cells, players, resources,
   technology, statistics and army-asset lists to the other and becomes inactive. Finally the other-player
   command entries are rebuilt.
*/
void GameFactionRuntime_ApplyPairwiseRelationTransition(FactionNotificationCodeBase activeFactionCodeForFirst,
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
  InGameRuntimeRoot *runtimeRoot;
  WorldOwnerListNode *ownerNode;
  
  originalFirstFactionIndex = firstFactionIndex;
  if (g_InGameRuntimeRoot->worldRuntime.activeFactionRuntimeIndex == secondFactionIndex) {
    InGameNotificationQueue_InsertPriorityRecord(NOTIFICATION_PAYLOAD_NONE,0,0,0,0,0,2,activeFactionCodeForSecond +
                                                 500);
  }
  else if (g_InGameRuntimeRoot->worldRuntime.activeFactionRuntimeIndex == firstFactionIndex) {
    InGameNotificationQueue_InsertPriorityRecord(NOTIFICATION_PAYLOAD_NONE,0,0,0,0,0,2,activeFactionCodeForFirst + 500);
  }
  /* each record's packedRelationStates holds a nibble per other faction */
  secondShift = (uint8_t)secondFactionIndex * 4;
  firstShift = (uint8_t)firstFactionIndex * 4;
  g_GameFactionRuntimeImage.records[firstFactionIndex].packedRelationStates =
       stateFirstTowardSecond << (secondShift & 31) |
       ~(0xf << (secondShift & 31)) &
       g_GameFactionRuntimeImage.records[firstFactionIndex].packedRelationStates;
  g_GameFactionRuntimeImage.records[secondFactionIndex].packedRelationStates =
       ~(0xf << (firstShift & 31)) &
       g_GameFactionRuntimeImage.records[secondFactionIndex].packedRelationStates |
       stateSecondTowardFirst << (firstShift & 31);
  bitOrPlayerCountOrSlot = 1 << ((uint8_t)secondFactionIndex & 31);
  if (stateSecondTowardFirst < FACTION_RELATION_STATE_FRIENDLY) {
    capabilityFlagsField = &g_GameFactionRuntimeImage.records[firstFactionIndex].capabilityFlags;
    *capabilityFlagsField = *capabilityFlagsField & ~bitOrPlayerCountOrSlot;
  }
  else {
    capabilityFlagsField = &g_GameFactionRuntimeImage.records[firstFactionIndex].capabilityFlags;
    *capabilityFlagsField = *capabilityFlagsField | bitOrPlayerCountOrSlot;
  }
  bitOrPlayerCountOrSlot = 1 << ((uint8_t)firstFactionIndex & 31);
  if (stateSecondTowardFirst < FACTION_RELATION_STATE_FRIENDLY) {
    capabilityFlagsField = &g_GameFactionRuntimeImage.records[secondFactionIndex].capabilityFlags;
    *capabilityFlagsField = *capabilityFlagsField & ~bitOrPlayerCountOrSlot;
  }
  else {
    capabilityFlagsField = &g_GameFactionRuntimeImage.records[secondFactionIndex].capabilityFlags;
    *capabilityFlagsField = *capabilityFlagsField | bitOrPlayerCountOrSlot;
  }
  /* tick of the last relation change per pair (record +0x700 + 4 * other faction) */
  currentTick = g_GameFactionRuntimeImage.tail.simulationTick;
  g_GameFactionRuntimeImage.records[firstFactionIndex].relationStateTicks[secondFactionIndex] =
       g_GameFactionRuntimeImage.tail.simulationTick;
  g_GameFactionRuntimeImage.records[secondFactionIndex].relationStateTicks[firstFactionIndex] = currentTick;
  if (stateSecondTowardFirst == FACTION_RELATION_MERGE) {
    bitOrPlayerCountOrSlot = 0;
    playerCountOrMaskWord = 0;
    playerBlocksRemaining = g_FrontendPlayerRuntimeBlockCount;
    playerBlockCursor = g_FrontendPlayerRuntimeBlocks;
    do {
      if (secondFactionIndex == (playerBlockCursor->factionAssignment).factionAssignmentIndex) {
        bitOrPlayerCountOrSlot++;
      }
      if (firstFactionIndex == (playerBlockCursor->factionAssignment).factionAssignmentIndex) {
        playerCountOrMaskWord++;
      }
      playerBlockCursor++;
      playerBlocksRemaining--;
    } while (playerBlocksRemaining != 0);
    /* State 11 merges the two factions. The second faction survives when it has players and the (bitwise) counts
       do not overlap; with no players on either side, or overlapping counts, a random bit decides. */
    if (((bitOrPlayerCountOrSlot & playerCountOrMaskWord) == 0) &&
        ((bitOrPlayerCountOrSlot != 0) || (playerCountOrMaskWord != 0))) {
      swapMergeDirection = bitOrPlayerCountOrSlot != 0;
    }
    else {
      randomValue = g_RandomGeneratorState.next();
      swapMergeDirection = (randomValue & FACTION_MERGE_RANDOM_DIRECTION_BIT) == 0;
    }
    /* from here on firstFactionIndex survives and secondFactionIndex is absorbed */
    if (swapMergeDirection) {
      firstFactionIndex = secondFactionIndex;
      secondFactionIndex = originalFirstFactionIndex;
    }
    runtimeRoot = g_InGameRuntimeRoot;
    if (secondFactionIndex == g_InGameRuntimeRoot->worldRuntime.activeFactionRuntimeIndex) {
      g_InGameRuntimeRoot->worldRuntime.activeFactionRuntimeIndex = firstFactionIndex;
    }
    survivingFactionTextureSet = g_ArmyGraphicsBindings[firstFactionIndex].textureSet;
    survivingFactionPaletteAsset = g_ArmyGraphicsBindings[firstFactionIndex].paletteAsset;
    playerBlockCursor = g_FrontendPlayerRuntimeBlocks;
    playerBlocksRemaining = g_FrontendPlayerRuntimeBlockCount;
    for (ownerNode = (runtimeRoot->worldRuntime).ownerListHead;
        g_FrontendPlayerRuntimeBlocks = playerBlockCursor, g_FrontendPlayerRuntimeBlockCount = playerBlocksRemaining,
        ownerNode != NULL; ownerNode = ownerNode->nextNode) {
      /* re-own the absorbed faction's models (entity +0x0C) and repaint them in the survivor's colours */
      if ((ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
         (recordOrCountOrIndex =
          (int)((ModelRuntimeSlot *)ownerNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime,
         ((ArmyRuntimeSlot *)recordOrCountOrIndex)->factionIndex == secondFactionIndex)) {
        ((ArmyRuntimeSlot *)recordOrCountOrIndex)->factionIndex = firstFactionIndex;
        ModelRuntimeHierarchy_SetPaletteAndTextureSetNonNullRecursive
                  (survivingFactionPaletteAsset,survivingFactionTextureSet,
                   ((ArmyRuntimeSlot *)recordOrCountOrIndex)->modelNodeRuntime);
      }
      playerBlockCursor = g_FrontendPlayerRuntimeBlocks;
      playerBlocksRemaining = g_FrontendPlayerRuntimeBlockCount;
    }
    do {
      if (secondFactionIndex == (playerBlockCursor->factionAssignment).factionAssignmentIndex) {
        recordOrCountOrIndex = playerBlockCursor->playerRuntimeId;
        (playerBlockCursor->factionAssignment).factionAssignmentIndex = firstFactionIndex;
        g_SelectionPlayerRuntimeBlockPointers[recordOrCountOrIndex]->factionIndex =
             firstFactionIndex;
      }
      playerBlockCursor++;
      playerBlocksRemaining--;
    } while (playerBlocksRemaining != 0);
    /* the survivor also gets the absorbed faction's per-faction cell byte (cell +0x70 + faction) */
    terrainGrid = runtimeRoot->worldRuntime.fieldGrid;
    recordOrCountOrIndex = terrainGrid->gridWidth * terrainGrid->gridHeight;
    gridCell = terrainGrid->cells;
    do {
      ((uint8_t *)&gridCell->occupancyMask)[firstFactionIndex] =
           ((uint8_t *)&gridCell->occupancyMask)[firstFactionIndex] |
           ((uint8_t *)&gridCell->occupancyMask)[secondFactionIndex];
      gridCell++;
      recordOrCountOrIndex--;
    } while (recordOrCountOrIndex != 0);
    g_GameFactionRuntimeImage.tail.factionLifecycleStates[secondFactionIndex] = FACTION_RUNTIME_LIFECYCLE_INACTIVE;
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
    *technologyMaskWord = *technologyMaskWord |
         g_GameFactionRuntimeImage.records[secondFactionIndex].technologyMasks256Bits
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
    *technologyMaskWord = *technologyMaskWord |
         g_GameFactionRuntimeImage.records[secondFactionIndex].technologyMasks256Bits
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
    /* append both army-asset lists (primary at record +0x1E0, secondary at +0xE0; 64 entries each) */
    for (bitOrPlayerCountOrSlot = g_GameFactionRuntimeImage.records[firstFactionIndex].primaryArmyAssetCount;
        (assetsRemaining != 0 && (bitOrPlayerCountOrSlot < FACTION_ARMY_ASSET_LIST_CAPACITY)); bitOrPlayerCountOrSlot++) {
      primaryArmyAssetReferenceDword =
           g_GameFactionRuntimeImage.records[secondFactionIndex].primaryArmyAssetPointersOrIds[recordOrCountOrIndex];
      armyAssetCount = &g_GameFactionRuntimeImage.records[firstFactionIndex].primaryArmyAssetCount;
      *armyAssetCount = *armyAssetCount + 1;
      g_GameFactionRuntimeImage.records[firstFactionIndex].primaryArmyAssetPointersOrIds[bitOrPlayerCountOrSlot] =
           primaryArmyAssetReferenceDword;
      recordOrCountOrIndex++;
      assetsRemaining--;
    }
    assetsRemaining = g_GameFactionRuntimeImage.records[secondFactionIndex].secondaryArmyAssetCount;
    recordOrCountOrIndex = 0;
    for (bitOrPlayerCountOrSlot = g_GameFactionRuntimeImage.records[firstFactionIndex].secondaryArmyAssetCount;
        (assetsRemaining != 0 && (bitOrPlayerCountOrSlot < FACTION_ARMY_ASSET_LIST_CAPACITY)); bitOrPlayerCountOrSlot++) {
      secondaryArmyAssetReferenceDword =
           g_GameFactionRuntimeImage.records[secondFactionIndex].secondaryArmyAssetPointersOrIds[recordOrCountOrIndex];
      armyAssetCount = &g_GameFactionRuntimeImage.records[firstFactionIndex].secondaryArmyAssetCount;
      *armyAssetCount = *armyAssetCount + 1;
      g_GameFactionRuntimeImage.records[firstFactionIndex].secondaryArmyAssetPointersOrIds[bitOrPlayerCountOrSlot] =
           secondaryArmyAssetReferenceDword;
      recordOrCountOrIndex++;
      assetsRemaining--;
    }
    InGameArmyStock_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
    InGameSpecialBuildCatalog_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
    InGameBuildCatalog_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
  }
  InGameOtherPlayerCommand_RebuildTargetEntries((UiNodeBase *)g_InGameRuntimeRoot);
}

