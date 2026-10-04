/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/faction/runtime.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/faction/runtime.h>
#include <thandor/thandor.h>

/* Module data. */

uint32_t *g_OldUnitSecondaryTable = 0;

uint32_t *g_OldUnitPrimaryTable = 0;

OldUnitRecordCount g_OldUnitRecordCount = 0;

GameFactionRuntimeImage g_GameFactionRuntimeImage = {.tail = {.factionLifecycleStates = {0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1}}};

/* Implementation ownership: gameplay/faction/runtime. */

/* Moves the diplomatic relation of a faction pair one step closer, chosen by the state of targetFactionIndex
   towards sourceFactionIndex: 0..2 -> 3/2, 3 -> 4, 4..5 -> 6/5, 6 -> 8, 8..9 -> 10/9, 10 -> 11 (merge),
   first value for the source's state towards the target. State 2 does nothing while the last change is at
   most 600 ticks old; the same check in states 4 and 8 never holds (it only accepts 2, 5 and 9). The first
   two arguments are not used.
*/
void GameFactionRuntime_AdvancePairwiseRelationState(uint32_t unusedRelationArgument0,uint32_t unusedRelationArgument1,
          FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex)

{
  Bool8 isRecentTimedState;
  
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


/* Moves the diplomatic relation of a faction pair back, chosen by the state of targetFactionIndex towards
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


/* Mission carry-over after a session ends: finds the current scenario's record in the loaded campaign and,
   for the outcome selected by g_EndMovieSelectionIndex, stores each faction's technology masks (8 dwords) in
   the old-unit secondary table and every unit standing inside its faction's exit zone as a primary record,
   moved by the scenario's per-faction offset. OldUnitRuntime_MergeMasksAndReplayRecords applies both in the
   next mission. Without a matching scenario both tables are cleared.
*/
void OldUnitRuntime_RebuildScenarioReplayTables(void)

{
  WorldOwnerListNode *ownerNode;
  ModelRuntimeSlot *modelRuntime;
  CampaignLevelRecord *scenarioLevel;
  CampaignLevelRecord *factionExitZoneCursor;
  uint32_t unitFactionIndex;
  uint32_t carryOverMaskBits;
  uint32_t skipMaskBits;
  uint32_t distanceToAnchor;
  int levelRecordsRemaining;
  int factionsRemaining;
  int wordIndex;
  uintptr_t scenarioRecord;
  uint32_t *technologyMasks;
  uint32_t *secondaryTableCursor;
  uint32_t *primaryRecord;
  Bool8 scenarioFound;

  scenarioFound = false;
  /* Campaign asset (CampaignAsset): the cursor starts at the asset base and advances by one 0x180-byte level
     record, so ((CampaignAsset *)cursor)->levels[0] is the current record. */
  if ((g_InGameRuntimeRoot != NULL) && (g_FrontendLoadedCampaignAsset != 0)) {
    levelRecordsRemaining = ((CampaignAsset *)g_FrontendLoadedCampaignAsset)->levelRecordCount;
    scenarioRecord = g_FrontendLoadedCampaignAsset;
    do {
      if (((CampaignAsset *)g_FrontendLoadedCampaignAsset)->currentLevelId ==
          ((CampaignAsset *)scenarioRecord)->levels[0].levelId) {
        scenarioFound = true;
        break;
      }
      scenarioRecord = scenarioRecord + sizeof(CampaignLevelRecord);
      levelRecordsRemaining--;
    } while (levelRecordsRemaining != 0);
  }
  if (!scenarioFound) {
    OldUnitRuntime_ResetPendingTables();
    return;
  }
  scenarioLevel = &((CampaignAsset *)scenarioRecord)->levels[0];
  /* Original quirk: the original offsets the faction-record cursor by an uninitialized value times the
     active faction index; the loop then walks all eight 0x740-byte faction records, which only stays
     inside the table from records[0], so the cursor always starts there. */
  technologyMasks = (uint32_t *)g_GameFactionRuntimeImage.records[0].technologyMasks256Bits;
  /* Per-faction exit zones of the level record; carry-over and skip hold one bit per outcome.
     factionExitZoneCursor walks the level record 4 bytes (one faction) per group, so element 0 of each
     per-faction array is the current faction's. */
  factionExitZoneCursor = scenarioLevel;
  carryOverMaskBits = scenarioLevel->carryOverMask >> ((uint8_t)g_EndMovieSelectionIndex & 31);
  skipMaskBits = scenarioLevel->skipMask >> ((uint8_t)g_EndMovieSelectionIndex & 31);
  secondaryTableCursor = g_OldUnitSecondaryTable;
  /* Eight groups, one per faction record. The masks are not shifted per group (as in the original). */
  for (factionsRemaining = 8; factionsRemaining != 0; factionsRemaining--) {
    if (((skipMaskBits & 1) == 0) && ((carryOverMaskBits & 1) != 0) &&
        ((factionExitZoneCursor->exitZoneCenterX[0] != 0) || (factionExitZoneCursor->exitZoneCenterY[0] != 0) ||
         (factionExitZoneCursor->exitZoneDestinationX[0] != 0) ||
         (factionExitZoneCursor->exitZoneDestinationY[0] != 0) || (factionExitZoneCursor->exitZoneRadius[0] != 0))) {
      for (wordIndex = 0; wordIndex < 8; wordIndex++) {
        secondaryTableCursor[wordIndex] = technologyMasks[wordIndex];
      }
      secondaryTableCursor = secondaryTableCursor + 8;
    }
    else if ((skipMaskBits & 1) == 0) {
      for (wordIndex = 0; wordIndex < 8; wordIndex++) {
        secondaryTableCursor[wordIndex] = 0;
      }
      secondaryTableCursor = secondaryTableCursor + 8;
    }
    /* on to the next faction record's technology masks */
    technologyMasks = technologyMasks + GAME_FACTION_RUNTIME_RECORD_BYTES / 4;
    factionExitZoneCursor =
         (CampaignLevelRecord *)((uint8_t *)factionExitZoneCursor + sizeof(factionExitZoneCursor->exitZoneCenterX[0]));
  }
  /* Primary records (8 dwords each, at most 0x200): [0] army asset id, [1] faction, [2] X, [3] Y,
     [4] rotation angle. */
  if ((g_InGameRuntimeRoot == NULL) || ((skipMaskBits & 1) != 0)) {
    return;
  }
  ownerNode = (g_InGameRuntimeRoot->worldRuntime).ownerListHead;
  g_OldUnitRecordCount = 0;
  if ((carryOverMaskBits & 1) == 0) {
    return;
  }
  primaryRecord = g_OldUnitPrimaryTable;
  for (; ownerNode != NULL; ownerNode = ownerNode->nextNode) {
    if (ownerNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
      continue;
    }
    /* the owner army holds the faction and the army asset id */
    modelRuntime = (ModelRuntimeSlot *)ownerNode->runtimePayload;
    unitFactionIndex = modelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex;
    if (scenarioLevel->exitZoneRadius[unitFactionIndex] <= 0) {
      continue;
    }
    distanceToAnchor = FixedMath_Length2(scenarioLevel->exitZoneCenterY[unitFactionIndex] - ownerNode->worldYQ12,
                                         scenarioLevel->exitZoneCenterX[unitFactionIndex] - ownerNode->worldXQ12);
    if ((int)distanceToAnchor > scenarioLevel->exitZoneRadius[unitFactionIndex]) {
      continue;
    }
    primaryRecord[2] = (ownerNode->worldXQ12 + scenarioLevel->exitZoneDestinationX[unitFactionIndex]) -
                       scenarioLevel->exitZoneCenterX[unitFactionIndex];
    primaryRecord[3] = (ownerNode->worldYQ12 + scenarioLevel->exitZoneDestinationY[unitFactionIndex]) -
                       scenarioLevel->exitZoneCenterY[unitFactionIndex];
    primaryRecord[1] = unitFactionIndex;
    /* only a model whose classState linkedArmyRuntimeOrSavedOffset is zero is committed as a record */
    if (modelRuntime->classState.linkedArmyRuntimeOrSavedOffset.modelRuntime == NULL) {
      primaryRecord[4] = ownerNode->modelLocalRotationAngle2;
      primaryRecord[0] = modelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime->armyAssetId;
      g_OldUnitRecordCount++;
      primaryRecord = primaryRecord + 8;
      if (OLD_UNIT_PRIMARY_RECORD_CAPACITY - 1 < g_OldUnitRecordCount) {
        return;
      }
    }
  }
}


/* Turns the *assetCount saved army-asset ids of one list back into registry pointers, in place. The first
   unknown id empties the list (*assetCount = 0); the entries already converted stay pointers. */
static void GameFactionRuntime_ResolveLoadedArmyAssetIds(uint32_t *assetIds,FactionArmyAssetCount *assetCount)
{
  FactionArmyAssetCount assetsRemaining;
  uint32_t *assetIdCursor;
  ArmyAssetRecordPrefix *resolvedAsset;

  assetIdCursor = assetIds;
  for (assetsRemaining = *assetCount; assetsRemaining != 0; assetsRemaining--) {
    if (ArmyAssetRegistry_FindById(*assetIdCursor,&resolvedAsset) != 0) {
      *assetCount = 0;
      return;
    }
    *assetIdCursor = (uint32_t)resolvedAsset; /* 5f-format: GameFactionRuntimeRecord.primary/secondaryArmyAssetPointersOrIds (daten.hex) */
    assetIdCursor++;
  }
}


/* After loading a save: turns the army-asset ids of both army-asset lists of all eight faction records back into
   registry pointers (an unknown id empties that list) and converts the 256 saved runtime-group member offsets
   into pointers again (offset + g_ArmyRuntimeRebaseBaseMinusOne; 0 stays NULL).
*/
void GameFactionRuntime_RebaseLoadedArmyReferences(void)

{
  int factionIndex;
  int groupSlotIndex;
  GameFactionRuntimeRecord *factionRecord;
  ArmyRuntimeSlot *savedSlotOffset;
  ArmyRuntimeSlot *rebasedSlot;

  for (factionIndex = 0; factionIndex < 8; factionIndex++) {
    factionRecord = &g_GameFactionRuntimeImage.records[factionIndex];
    GameFactionRuntime_ResolveLoadedArmyAssetIds(factionRecord->secondaryArmyAssetPointersOrIds,
                                                 &factionRecord->secondaryArmyAssetCount);
    GameFactionRuntime_ResolveLoadedArmyAssetIds(factionRecord->primaryArmyAssetPointersOrIds,
                                                 &factionRecord->primaryArmyAssetCount);
    for (groupSlotIndex = 0; groupSlotIndex < 256; groupSlotIndex++) {
      /* the slot holds the saved offset */
      savedSlotOffset = factionRecord->runtimeGroupMembers8x32[groupSlotIndex];
      rebasedSlot = NULL;
      if (savedSlotOffset != NULL) {
        rebasedSlot = (ArmyRuntimeSlot *)((int)savedSlotOffset + (int)g_ArmyRuntimeRebaseBaseMinusOne); /* 5f-format: GameFactionRuntimeRecord.runtimeGroupMembers8x32 (daten.hex) */
      }
      factionRecord->runtimeGroupMembers8x32[groupSlotIndex] = rebasedSlot;
    }
  }
}


/* Called when an army is destroyed: clears every slot of the eight factions' 256-entry runtime group member
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


/* Returns true when bit otherFactionIndex is clear in factionIndex's record capabilityFlags.
   The mask holds one bit per faction: GameData_ResetDefaults sets the faction's own bit and bit 0, and
   GameFactionRuntime_ApplyPairwiseRelationTransition sets or clears the others, so a clear bit marks a faction
   this one is not friendly with (the AI treats its entities as foreign/hostile).
*/
Bool8 GameFactionRuntime_TestCapabilityBitClear(uint32_t otherFactionIndex,FactionRuntimeIndex factionIndex)

{
  return (g_GameFactionRuntimeImage.records[factionIndex].capabilityFlags &
         1 << ((uint8_t)otherFactionIndex & 31)) == 0;
}


/* Returns the diplomatic relation state (0..11) of factionIndex towards otherFactionIndex: nibble
   otherFactionIndex of the faction record's packedRelationStates. States from 4 on count as friendly.
*/
FactionRelationState GameFactionRuntime_GetPackedStateNibble
          (FactionRuntimeIndex otherFactionIndex,FactionRuntimeIndex factionIndex)

{
  return g_GameFactionRuntimeImage.records[factionIndex].packedRelationStates >>
         ((uint8_t)(otherFactionIndex << 2) & 31) & 0xf;
}


#define FACTION_TECHNOLOGY_MASK_BITS 256u

/* Lowest technology whose bit is set in haveMasks and clear in lackMasks (both 256-bit technology masks), or
   FACTION_TECHNOLOGY_MASK_BITS when there is none. */
static TechnologyId GameFactionRuntime_FindFirstTechnologyOnlyIn(const uint32_t *haveMasks,
          const uint32_t *lackMasks)
{
  TechnologyId technologyIndex;
  uint32_t bitMask;

  for (technologyIndex = 0; technologyIndex < FACTION_TECHNOLOGY_MASK_BITS; technologyIndex++) {
    bitMask = 1u << (technologyIndex & 31);
    if (((haveMasks[technologyIndex >> 5] & bitMask) != 0) && ((lackMasks[technologyIndex >> 5] & bitMask) == 0)) {
      return technologyIndex;
    }
  }
  return FACTION_TECHNOLOGY_MASK_BITS;
}


/* Technology exchange between related factions: for every unordered pair of factions 1..7 whose relation
   state is 8, 9 or 10, the lowest technology only the source faction has and the lowest technology only the
   other faction has are swapped (each side unlocks the other's). A pair where either side has nothing the
   other lacks exchanges nothing. Afterwards the other-player command entries are rebuilt.
*/
void GameFactionRuntime_SynchronizeTechnologiesForRelationStates8To10(void)

{
  uint32_t sourceFactionIndex;
  uint32_t otherFactionIndex;
  uint32_t relationState;
  TechnologyId sourceTechnologyIndex;
  TechnologyId otherTechnologyIndex;
  GameFactionRuntimeRecord *sourceRecord;
  GameFactionRuntimeRecord *otherRecord;

  for (sourceFactionIndex = 1; sourceFactionIndex < 7; sourceFactionIndex++) {
    sourceRecord = &g_GameFactionRuntimeImage.records[sourceFactionIndex];
    for (otherFactionIndex = sourceFactionIndex + 1; otherFactionIndex < 8; otherFactionIndex++) {
      otherRecord = &g_GameFactionRuntimeImage.records[otherFactionIndex];
      /* the other faction's relation-state nibble towards the source faction */
      relationState = otherRecord->packedRelationStates >> ((char)sourceFactionIndex * 4 & 31U) & 0xf;
      if ((relationState < 8) || (10 < relationState)) {
        continue;
      }
      sourceTechnologyIndex = GameFactionRuntime_FindFirstTechnologyOnlyIn(sourceRecord->technologyMasks256Bits,
                                                                          otherRecord->technologyMasks256Bits);
      if (sourceTechnologyIndex == FACTION_TECHNOLOGY_MASK_BITS) {
        continue;
      }
      otherTechnologyIndex = GameFactionRuntime_FindFirstTechnologyOnlyIn(otherRecord->technologyMasks256Bits,
                                                                         sourceRecord->technologyMasks256Bits);
      if (otherTechnologyIndex == FACTION_TECHNOLOGY_MASK_BITS) {
        continue;
      }
      /* swap the pair */
      Technology_UnlockForFaction(0,0,otherTechnologyIndex,sourceFactionIndex);
      Technology_UnlockForFaction(0,0,sourceTechnologyIndex,otherFactionIndex);
    }
  }
  InGameOtherPlayerCommand_RebuildTargetEntries((UiNodeBase *)g_InGameRuntimeRoot);
}


/* One alert anchor of GameFactionRuntime_UpdateImpactAlertAnchorAndNotify: restarts *cooldown at 150, moves the
   anchor (*anchorY = hit model x, *anchorX = hit model y) and queues alert movie notificationMovieId when the old
   cooldown was below 50 and the hit is far from the old anchor or the cooldown had run out, and the shown faction
   owns the hit army (ownershipRecord[1] = root model node, [2] = owning army). */
static void GameFactionRuntime_MoveImpactAlertAnchor(FactionAnchorCooldownTicks *cooldown,
          GraphicsWorldCoordinateQ12 *anchorY,GraphicsWorldCoordinateQ12 *anchorX,const ModelRuntimeSlot *ownershipRecord,
          WorldRuntimeContext *worldRuntime,InGameNotificationPriority priority,
          InGameNotificationMovieId notificationMovieId)
{
  ModelRuntimeNode *hitModelNode;
  ArmyRuntimeSlot *owningArmy;
  FactionAnchorCooldownTicks previousCooldown;
  GraphicsWorldCoordinateQ12 hitX;
  GraphicsWorldCoordinateQ12 hitY;
  int distanceY;
  int distanceX;
  Bool8 notify;

  hitModelNode = ownershipRecord->rootModelNodeOrSavedOffset.modelNode;
  previousCooldown = *cooldown;
  distanceY = *anchorY - hitModelNode->worldTransform.translation.x;
  if (distanceY < 0) {
    distanceY = -distanceY;
  }
  distanceX = *anchorX - hitModelNode->worldTransform.translation.y;
  if (distanceX < 0) {
    distanceX = -distanceX;
  }
  *cooldown = 150;
  /* 0xC000 = 12.0 world units (Q12), measured as |dy| + |dx| */
  notify = ((int)previousCooldown < 50) && ((12 * Q12_ONE < distanceY + distanceX) || (previousCooldown == 0));
  hitY = hitModelNode->worldTransform.translation.y;
  hitX = hitModelNode->worldTransform.translation.x;
  *anchorY = hitX;
  *anchorX = hitY;
  if (notify) {
    owningArmy = ownershipRecord->ownerArmyRuntimeOrSavedOffset.armyRuntime;
    if (worldRuntime->activeFactionRuntimeIndex == owningArmy->factionIndex) {
      InGameNotificationQueue_InsertPriorityRecord
                (FACTION_IMPACT_ANCHOR,0,(worldRuntime->motion).pitchAngle,(worldRuntime->motion).headingAngle,
                 hitModelNode->worldTransform.translation.y,hitModelNode->worldTransform.translation.x,priority,
                 notificationMovieId);
    }
  }
}


/* "Under attack" alert for the faction owning a hit army (called by ShotRuntime_ApplyArmyHitRelationAndNotifications
   when a shot opens hostilities): moves the faction's primary or secondary alert anchor (chosen by the definition's
   accelerationPerTick: zero = primary) to the hit model and restarts its 150-tick cooldown. The alert movie
   300 / 301 is only queued when the cooldown had fallen below 50 and the hit is more than 12 world units from
   the old anchor or the cooldown had run out, and only when the shown faction owns the hit model, so a sustained
   attack does not repeat the alert.
*/
void GameFactionRuntime_UpdateImpactAlertAnchorAndNotify
          (ModelRuntimeSlot *hitModelRuntime,WorldRuntimeContext *worldRuntime)

{
  /* the root model runtime of the hit model's army: [0] definition, [1] root model node, [2] owning army
     (with its factionIndex) */
  ModelRuntimeSlot *ownershipRecord;
  int ownerFactionIndex;
  GameFactionRuntimeRecord *ownerRecord;

  ownershipRecord =
       hitModelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime->modelRuntimeOrSavedOffset.modelRuntime;
  ownerFactionIndex = hitModelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex;
  ownerRecord = &g_GameFactionRuntimeImage.records[ownerFactionIndex];
  if (ownershipRecord->definitionOrSavedId.runtimeDefinition->accelerationPerTick == 0) {
    GameFactionRuntime_MoveImpactAlertAnchor(&ownerRecord->primaryAnchorCooldown,&ownerRecord->primaryAnchorYQ12,
                                             &ownerRecord->primaryAnchorXQ12,ownershipRecord,worldRuntime,8,300);
  }
  else {
    GameFactionRuntime_MoveImpactAlertAnchor(&ownerRecord->anchorCooldown0,&ownerRecord->secondaryAnchorYQ12,
                                             &ownerRecord->secondaryAnchorXQ12,ownershipRecord,worldRuntime,7,301);
  }
}


/* Recomputes one faction's statistics for the score / results screens: explored terrain percent, unlocked
   technologies beyond the five starting ones, extracted resource components, the economy and relation scores,
   the summed value of its army assets on the map, and the combined progress score.
*/
void GameFactionRuntime_RecomputeProgressAndScoreMetrics
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  TerrainExploredPercent exploredPercent;
  ResourceExtractionRateQ4PerTick xeniteRate;
  ResourceExtractionRateQ4PerTick tritiumRate;
  int exploredCellCount;
  uint32_t cellCount;
  uint32_t cellsRemaining;
  uint32_t technologyCount;
  uint32_t technologyBit;
  FactionTechnologyCount technologyCountBeyondBaseline;
  int xeniteComponent;
  int tritiumComponent;
  int armyAssetValueSum;
  ArmyRuntimeSlot *ownerArmy;
  uint8_t *cellVisibilityCursor;
  uint32_t maskWordIndex;
  ArmyAssetRecordPrefix *resolvedAsset;
  FieldGridAsset *terrainGrid;
  WorldOwnerListNode *ownerNode;

  terrainGrid = worldRuntime->fieldGrid;
  cellCount = terrainGrid->gridWidth * terrainGrid->gridHeight;
  exploredCellCount = 0;
  /* one byte per faction in the cell's occupancyMask; bits 3-7 set = the faction has explored the cell (0x80-byte cells).
     Original quirk: a grid with zero cells would count down from 0 (never happens). */
  cellVisibilityCursor = (uint8_t *)&terrainGrid->cells[0].occupancyMask + factionIndex;
  cellsRemaining = cellCount;
  do {
    if ((*cellVisibilityCursor & FIELD_CELL_OCCUPANCY_EXPLORED_BITS) != 0) {
      exploredCellCount++;
    }
    cellVisibilityCursor = cellVisibilityCursor + sizeof(FieldGridCell);
    cellsRemaining--;
  } while (cellsRemaining != 0);
  g_GameFactionRuntimeImage.records[factionIndex].exploredTerrainPercent =
       (uint32_t)(exploredCellCount * 100) / cellCount;
  /* count the set bits of the faction record's 256-bit technology mask (technologyMasks256Bits) */
  technologyCount = 0;
  for (maskWordIndex = 0; maskWordIndex < 8; maskWordIndex++) {
    for (technologyBit = 1; technologyBit != 0; technologyBit = technologyBit * 2) {
      if ((g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits[maskWordIndex] & technologyBit) != 0) {
        technologyCount++;
      }
    }
  }
  technologyCountBeyondBaseline = technologyCount - 5;
  if (technologyCount < 5) {
    technologyCountBeyondBaseline = 0;
  }
  exploredPercent = g_GameFactionRuntimeImage.records[factionIndex].exploredTerrainPercent;
  xeniteComponent = (int)g_GameFactionRuntimeImage.records[factionIndex].xeniteExtractedTotalQ4 >> 4;
  tritiumComponent = (int)g_GameFactionRuntimeImage.records[factionIndex].tritiumExtractedTotalQ4 >> 4;
  g_GameFactionRuntimeImage.records[factionIndex].unlockedTechnologyCountBeyondBaseline = technologyCountBeyondBaseline;
  g_GameFactionRuntimeImage.records[factionIndex].primaryResourceComponent = xeniteComponent;
  g_GameFactionRuntimeImage.records[factionIndex].secondaryResourceComponent = tritiumComponent;
  g_GameFactionRuntimeImage.records[factionIndex].economyProgressScore =
       (int)(xeniteComponent * 16 + tritiumComponent * 8 + technologyCountBeyondBaseline * (10 * Q12_ONE) +
             exploredPercent * Q12_ONE) >> Q12_SHIFT;
  g_GameFactionRuntimeImage.records[factionIndex].relationScore =
       (g_GameFactionRuntimeImage.records[factionIndex].relationCounterB * (40 * Q12_ONE) +
       g_GameFactionRuntimeImage.records[factionIndex].relationCounterD * (-20 * Q12_ONE) +
       g_GameFactionRuntimeImage.records[factionIndex].relationCounterA * (20 * Q12_ONE) +
       g_GameFactionRuntimeImage.records[factionIndex].relationCounterC * (-10 * Q12_ONE) +
       g_GameFactionRuntimeImage.records[factionIndex].relationCounterE * (20 * Q12_ONE) +
       g_GameFactionRuntimeImage.records[factionIndex].relationCounterF * (40 * Q12_ONE)) >> Q12_SHIFT;
  /* sum the army asset's xeniteCostQ4 (read as resolvedAsset[2].registryId) over every model of this faction
     on the map */
  armyAssetValueSum = 0;
  for (ownerNode = worldRuntime->ownerListHead; ownerNode != NULL;
      ownerNode = ownerNode->nextNode) {
    if (ownerNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
      continue;
    }
    ownerArmy = ((ModelRuntimeSlot *)ownerNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime;
    if ((factionIndex == ownerArmy->factionIndex) &&
        (ArmyAssetRegistry_FindById(ownerArmy->armyAssetId,&resolvedAsset) == 0)) {
      armyAssetValueSum = armyAssetValueSum + resolvedAsset[2].registryId;
    }
  }
  tritiumRate = g_GameFactionRuntimeImage.records[factionIndex].tritiumExtractionRateQ4PerTick;
  xeniteRate = g_GameFactionRuntimeImage.records[factionIndex].xeniteExtractionRateQ4PerTick;
  exploredPercent = g_GameFactionRuntimeImage.records[factionIndex].exploredTerrainPercent;
  technologyCountBeyondBaseline = g_GameFactionRuntimeImage.records[factionIndex].unlockedTechnologyCountBeyondBaseline;
  g_GameFactionRuntimeImage.records[factionIndex].activeArmyContribution = armyAssetValueSum;
  g_GameFactionRuntimeImage.records[factionIndex].combinedProgressScore =
       ((tritiumRate >> 1) + xeniteRate) * exploredPercent * technologyCountBeyondBaseline >> 3;
}


/* Finds which of its faction's eight runtime groups (32 member slots each) holds runtimeEntry, for the group
   selection commands in gameplay/selection/runtime. Returns the one-based group number (1..8), or 0 when it is
   in no group.
*/
uint32_t GameFactionRuntime_FindRuntimeGroupNumber(RuntimeModelFactionPrefix *runtimeEntry)

{
  int slotsRemaining;
  uint32_t groupNumber;
  ArmyRuntimeSlot **slotCursor;
  ArmyRuntimeSlot **nextSlotCursor;
  Bool8 found;

  groupNumber = 0;
  nextSlotCursor = g_GameFactionRuntimeImage.records[runtimeEntry->factionIndex].runtimeGroupMembers8x32;
  do {
    slotsRemaining = 32;
    groupNumber++;
    found = groupNumber == 0; /* the original's zero test of the increment; never true */
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


/* Checks whether the faction already has armyAssetRecord pending: in its secondary army-asset list, or in
   production in one of its class 0x0B/0x0D structures (state word 0x2E == 1). The result is
   false when found, true when not.
*/
Bool8 FactionRuntime_IsArmyAssetNotPending
          (FactionRuntimeIndex factionIndex,ArmyAssetRecordPrefix *armyAssetRecord)

{
  ArmyAssetRecordPrefix *activeAssetRecord;
  WorldOwnerListNode *ownerNode;
  int *modelPayload;
  FactionArmyAssetCount assetIndex;

  for (assetIndex = 0; assetIndex < g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount;
       assetIndex++) {
    if (armyAssetRecord ==
        (ArmyAssetRecordPrefix *)g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetPointersOrIds[assetIndex]) { /* 5f-format: GameFactionRuntimeRecord.secondaryArmyAssetPointersOrIds */
      return false;
    }
  }
  /* Structures of class 0xB or 0xD in state 1 that are currently producing armyAssetRecord. */
  for (ownerNode = g_InGameRuntimeRoot->worldRuntime.ownerListHead;
      ownerNode != NULL; ownerNode = ownerNode->nextNode) {
    if (ownerNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
      continue;
    }
    modelPayload = (int *)ownerNode->runtimePayload;
    if ((factionIndex == ((ModelRuntimeSlot *)modelPayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex) &&
        ((((ModelRuntimeSlot *)modelPayload)->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_11) ||
         (((ModelRuntimeSlot *)modelPayload)->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_13)) &&
        (modelPayload[46] == 1)) {
      activeAssetRecord = (ArmyAssetRecordPrefix *)modelPayload[24]; /* 5f-format: ModelRuntimeSlot class state word 24 (asset in production) */
      if (armyAssetRecord == activeAssetRecord) {
        return false;
      }
    }
  }
  return true;
}


/* Stops an entity where it stands (used by the stop command on the selection): clears the command flags
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


/* Where an entity's current command should take it, for the movement code in gameplay/army/movement: target flag
   1 aims at a target entity (its model position, raised by the definition's aimHeightOffsetQ12; class 0x15
   aims at its first child node), flag 2 at a fixed world position. A target entity that the owner's faction can
   no longer see is dropped (entity and flags cleared). Writes the position to *outPosition and returns true, or
   returns false when there is none.
*/
Bool8 GameEntityRuntime_ResolveCommandTargetPosition(GameEntityRuntime *targetState,FixedVectorQ12 *outPosition)

{
  GameEntityRuntime *commandTargetEntity;
  uint32_t visibilityMask;
  ModelRuntimeSlot *targetModelRuntime;
  ModelRuntimeNode *targetModelNode;

  /* Original quirk: on failure the original leaves the position undefined (left-over intermediate values), and
     one caller still copies the Z value into a local. The port writes zeros instead, so *outPosition is always
     written. */
  outPosition->xQ12 = 0;
  outPosition->yQ12 = 0;
  outPosition->zQ12 = 0;
  if (((targetState->common).commandTarget.targetFlags & 1) == 0) {
    if (((targetState->common).commandTarget.targetFlags & 2) == 0) {
      return false;
    }
    outPosition->xQ12 = (targetState->common).commandTarget.targetWorldXQ12;
    outPosition->yQ12 = (targetState->common).commandTarget.targetWorldYQ12;
    outPosition->zQ12 = (targetState->common).commandTarget.targetWorldZQ12;
    return true;
  }
  commandTargetEntity = (targetState->common).commandTarget.targetEntity;
  if (commandTargetEntity == NULL) {
    return false;
  }
  /* two bits per faction; the upper one = the target is visible to that faction */
  visibilityMask = 2u << ((uint8_t)((targetState->common).ownership.ownerIndex * 2) & 31);
  targetModelRuntime = (ModelRuntimeSlot *)(commandTargetEntity->common).ownership.definitionOrClassRecord;
  if (((commandTargetEntity->common).damageState.factionVisibilityBits1C & visibilityMask) == 0) {
    /* the owner's faction lost sight of the target: drop it */
    (targetState->common).commandTarget.targetEntity = NULL;
    (targetState->common).commandTarget.targetFlags = 0;
    return false;
  }
  targetModelNode = (commandTargetEntity->common).ownership.modelNode;
  if (targetModelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_21_AIRCRAFT) {
    targetModelNode = targetModelNode->childNodes[0];
  }
  outPosition->xQ12 = (targetModelNode->worldTransform).translation.x;
  outPosition->yQ12 = (targetModelNode->worldTransform).translation.y;
  outPosition->zQ12 =
       (targetModelNode->worldTransform).translation.z +
       targetModelRuntime->definitionOrSavedId.runtimeDefinition->aimHeightOffsetQ12;
  return true;
}


/* Applies impactValue to an entity's integrity (called twice per hit by ArmyRuntime_ApplyImpactDamageToRuntimeAndParent;
   a negative value repairs and goes to the entity its runtime link points at). A destroyed entity passes the
   overkill on to its parent model's army, or, without a parent, is turned to the impact angle (definition class 0
   with placementContactKindIndex 0) and counted in the score counters: a loss for its faction, a kill for
   sourceFactionIndex (the heavier counters D/F instead of C/E for classes handled by
   ArmyRuntime_ClassCommandHandlerGroupA). Repair beyond the definition maximum (maximumHealth) is clamped and the
   excess handed to the first linked army that is not at full integrity.
*/
void GameEntityRuntime_ApplyImpactDamageAndFactionRelationState
          (AngleTurn32 impactAngle,FactionRuntimeIndex sourceFactionIndex,
          ImpactDamageValue32 impactValue,GameEntityRuntime *targetEntityRuntime)

{
  int *integrityField;
  GameEntityRuntimeFlags *runtimeFlagsField;
  FactionRelationCounter *relationCounter;
  ModelRuntimeSlot *attachedModelRuntime;
  GameEntityRuntime *attachmentCursor;
  void *definitionRecord;
  int maximumIntegrity;
  int previousIntegrity;
  int overkillIntegrity;
  int repairExcess;
  int victimFactionIndex;
  int victimClassId;
  int attachmentsRemaining;
  ModelRuntimeNode *parentNode;

  if (impactValue < 0) {
    targetEntityRuntime = *(GameEntityRuntime **)(targetEntityRuntime->common).ownership.runtimeLink;
  }
  (targetEntityRuntime->common).pathingAndImpactState.impactReaction.state08 = 0;
  (targetEntityRuntime->common).pathingAndImpactState.impactReaction.reactionCode09 = 2;
  (targetEntityRuntime->common).pathingAndImpactState.impactReaction.state0A = 0;
  (targetEntityRuntime->common).pathingAndImpactState.impactReaction.state0B = 0;
  if (0 < (targetEntityRuntime->common).damageState.remainingIntegrity) {
    maximumIntegrity =
         ((ModelDefinition *)(targetEntityRuntime->common).ownership.definitionOrClassRecord)->
         maximumHealth;
    integrityField = &(targetEntityRuntime->common).damageState.remainingIntegrity;
    previousIntegrity = *integrityField;
    *integrityField = *integrityField - impactValue;
    /* the impact used up the remaining integrity: the entity is destroyed */
    if (previousIntegrity <= impactValue) {
      overkillIntegrity = (targetEntityRuntime->common).damageState.remainingIntegrity;
      runtimeFlagsField = &(targetEntityRuntime->common).runtimeFlags;
      *runtimeFlagsField = *runtimeFlagsField | 8;
      parentNode = ((targetEntityRuntime->common).ownership.modelNode)->parentNode;
      (targetEntityRuntime->common).damageState.counterOrTerminalReference.terminalEntity =
           targetEntityRuntime;
      (targetEntityRuntime->common).damageState.remainingIntegrity = 0;
      if (parentNode == NULL) {
        definitionRecord = (targetEntityRuntime->common).ownership.definitionOrClassRecord;
        if ((((ModelDefinition *)definitionRecord)->runtimeClassId == 0) &&
           (((ModelDefinition *)definitionRecord)->placementContactKindIndex == 0)) {
          (((targetEntityRuntime->common).ownership.modelNode)->modelPayload).worldRotationAngle0 =
               impactAngle;
        }
        if (impactValue != 0) {
          /* The original's branch here tests a CPU flag that is left over from a multiplication (measured on
             an AMD Zen 3: unchanged), so it still holds the impactValue == 0 test, whose own branch already left
             for zero; the branch is never taken and the counters are always updated. The C follows that. */
          victimFactionIndex =
               ((ArmyRuntimeSlot *)(targetEntityRuntime->common).ownership.runtimeLink)->factionIndex;
          victimClassId =
               ((ModelDefinition *)(targetEntityRuntime->common).ownership.definitionOrClassRecord)->
               runtimeClassId;
          relationCounter = &g_GameFactionRuntimeImage.records[victimFactionIndex].relationCounterC;
          *relationCounter = *relationCounter + 1;
          relationCounter = &g_GameFactionRuntimeImage.records[sourceFactionIndex].relationCounterE;
          *relationCounter = *relationCounter + 1;
          if (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand[victimClassId] ==
              ArmyRuntime_ClassCommandHandlerGroupA) {
            relationCounter = &g_GameFactionRuntimeImage.records[victimFactionIndex].relationCounterD;
            *relationCounter = *relationCounter + 1;
            relationCounter = &g_GameFactionRuntimeImage.records[sourceFactionIndex].relationCounterF;
            *relationCounter = *relationCounter + 1;
            relationCounter = &g_GameFactionRuntimeImage.records[victimFactionIndex].relationCounterC;
            *relationCounter = *relationCounter - 1;
            relationCounter = &g_GameFactionRuntimeImage.records[sourceFactionIndex].relationCounterE;
            *relationCounter = *relationCounter - 1;
          }
        }
      }
      else {
        /* the overkill goes on to the parent's army */
        ArmyRuntime_ApplyDamageAndPropagateToParent(-overkillIntegrity,
                                                    (parentNode->runtimePayload).modelRuntime);
      }
    }
    else {
      integrityField = &(targetEntityRuntime->common).damageState.remainingIntegrity;
      repairExcess = maximumIntegrity - *integrityField;
      if (repairExcess == 0 || maximumIntegrity < *integrityField) {
        /* repaired to or beyond the maximum: clamp, the (negative) excess repairs a linked army */
        integrityField = &(targetEntityRuntime->common).damageState.remainingIntegrity;
        *integrityField = *integrityField + repairExcess;
        /* common.ownership.ownerIndex is the model runtime's attachmentCount here, the attached child model
           runtimes are its attachments[] with a stride of 0x20 (attachmentCursor advances by those 0x20 bytes) */
        attachedModelRuntime = (targetEntityRuntime->classPayload).impactOwnerLinks.attachment0ChildModelRuntime;
        attachmentCursor = targetEntityRuntime;
        for (attachmentsRemaining = (targetEntityRuntime->common).ownership.ownerIndex; attachmentsRemaining != 0;
             attachmentsRemaining--) {
          /* the child's health against its definition's maximumHealth: not at full health */
          if ((attachedModelRuntime != NULL) &&
             (attachedModelRuntime->health !=
              attachedModelRuntime->definitionOrSavedId.runtimeDefinition->maximumHealth)) {
            ArmyRuntime_ApplyDamageAndPropagateToParent(repairExcess,attachedModelRuntime);
            return;
          }
          attachedModelRuntime = (attachmentCursor->classPayload).impactOwnerLinks.attachment1ChildModelRuntime;
          attachmentCursor =
               (GameEntityRuntime *)&(attachmentCursor->common).commandTarget.targetWorldXQ12;
        }
      }
    }
  }
}


/* Queues repetitionCount units of an army record for a faction (the build buttons of the in-game catalog and
   the AI): appends the registry pointer of armyAssetId that many times to the faction's secondary army-asset
   list, stopping when its 64 entries are full. An unknown id queues nothing.
*/
void GameFactionRuntime_RegisterArmyAssetPointers(uint32_t unusedPlayerRuntimeId,FactionArmyAssetCount repetitionCount,
          PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex)

{
  GameFactionRuntimeRecord *factionRecord;
  ArmyAssetRecordPrefix *resolvedAsset;

  if (ArmyAssetRegistry_FindById(armyAssetId,&resolvedAsset) != 0) {
    return;
  }
  factionRecord = &g_GameFactionRuntimeImage.records[factionIndex];
  /* Original quirk: the count is tested after the first append, so repetitionCount 0 wraps around and fills
     the list up to its capacity. */
  do {
    if (factionRecord->secondaryArmyAssetCount >= FACTION_ARMY_ASSET_LIST_CAPACITY) {
      return;
    }
    factionRecord->secondaryArmyAssetPointersOrIds[factionRecord->secondaryArmyAssetCount] = (uint32_t)resolvedAsset; /* 5f-format: GameFactionRuntimeRecord.secondaryArmyAssetPointersOrIds */
    factionRecord->secondaryArmyAssetCount++;
    repetitionCount--;
  } while (repetitionCount != 0);
}


/* Whether a producing structure of class producerClassId is building armyDefinition right now: payload [0x18]
   holds the army in production; a class 0x0D structure must also be able to build the army (its definition's
   classParameterC4 against the army's flags) and have production state [0x2E] == 1. */
static Bool8 GameFactionRuntime_StructureProducesArmy(const int *modelPayload,int producerClassId,
          const ArmyAssetRecordPrefix *armyDefinition)
{
  if (producerClassId == MODEL_RUNTIME_CLASS_13) {
    return (((uint32_t)((ModelRuntimeSlot *)modelPayload)->definitionOrSavedId.runtimeDefinition->classParameterC4 &
             armyDefinition[1].selectionDetailTemplateVariantIndex) != 0) &&
           (armyDefinition->registryId == modelPayload[24]) && (modelPayload[46] == 1);
  }
  return armyDefinition->registryId == modelPayload[24];
}


/* Stops a structure's production: clears the army in production [0x18], its state word (stateWordIndex: [0x2E],
   or [0x2B] for the aircraft pad) and flag 0x100 of [0x3B], takes its share [0x1D] back out of [0x3D] and refunds
   the full price to the faction's xenite. */
static void GameFactionRuntime_StopProductionAndRefund(int *modelPayload,int stateWordIndex,
          const ArmyAssetRecordPrefix *armyDefinition,FactionRuntimeIndex factionIndex)
{
  PckArmyAssetIdCatalog refundAmount;

  modelPayload[61] = modelPayload[61] - modelPayload[29];
  refundAmount = armyDefinition[2].registryId;
  modelPayload[29] = 0;
  modelPayload[stateWordIndex] = 0;
  modelPayload[59] = modelPayload[59] & ~(uint32_t)ARMY_MODEL_STATE_PRODUCING;
  modelPayload[24] = 0;
  g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 =
       g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 + refundAmount;
}


/* In-game command INGAME_COMMAND_CANCEL_QUEUED_ARMY (the cancel click on a build button of the in-game catalog,
   the reverse of GameFactionRuntime_RegisterArmyAssetPointers): cancels up to requestedCount orders of an army
   record. Waiting orders are taken out of the faction's production queue first (without a refund); what is left is cancelled in the faction's producing structures (class 0x0B, 0x16 or 0x0D, chosen by the
   army's flags 0x10 / 0x08), which stop production and refund the full price to the faction's xenite.
*/
void GameFactionRuntime_CancelQueuedArmyAssetsAndRefund
          (uint32_t unusedPlayerRuntimeId,FactionArmyAssetCount requestedCount,
          PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex)

{
  FactionArmyAssetCount *secondaryCount;
  uint32_t *queuedAssets;
  int *modelPayload;
  ArmyAssetRecordPrefix *armyDefinition;
  FactionArmyAssetCount assetsRemaining;
  int writeIndex;
  int readIndex;
  int producerClassId;
  WorldOwnerListNode *ownerNode;

  if (ArmyAssetRegistry_FindById(armyAssetId,&armyDefinition) != 0) {
    return;
  }
  /* compact the queue (secondaryArmyAssetPointersOrIds), dropping the first matching entries */
  queuedAssets = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetPointersOrIds;
  readIndex = 0;
  writeIndex = 0;
  assetsRemaining = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount;
  while (assetsRemaining != 0) {
    if (((uint32_t)armyDefinition == queuedAssets[readIndex]) && (0 < (int)requestedCount)) { /* 5f-format: GameFactionRuntimeRecord.secondaryArmyAssetPointersOrIds */
      readIndex++;
      secondaryCount = &g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount;
      *secondaryCount = *secondaryCount - 1;
      requestedCount--;
    }
    else {
      queuedAssets[writeIndex] = queuedAssets[readIndex];
      readIndex++;
      writeIndex++;
    }
    assetsRemaining--;
  }
  if (requestedCount == 0) {
    return;
  }
  /* the structure class that produces this army */
  ownerNode = (g_InGameRuntimeRoot->worldRuntime).ownerListHead;
  if ((armyDefinition[1].selectionDetailTemplateVariantIndex & ARMY_ASSET_FLAG_BUILT_BY_CLASS11) != 0) {
    producerClassId = MODEL_RUNTIME_CLASS_11;
  }
  else if ((armyDefinition[1].selectionDetailTemplateVariantIndex & ARMY_ASSET_FLAG_BUILT_AT_AIRCRAFT_PAD) != 0) {
    producerClassId = MODEL_RUNTIME_CLASS_22;
  }
  else {
    producerClassId = MODEL_RUNTIME_CLASS_13;
  }
  for (; ownerNode != NULL; ownerNode = ownerNode->nextNode) {
    if (ownerNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
      continue;
    }
    modelPayload = (int *)ownerNode->runtimePayload;
    if ((factionIndex != ((ModelRuntimeSlot *)modelPayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex) ||
        (producerClassId != ((ModelRuntimeSlot *)modelPayload)->definitionOrSavedId.runtimeDefinition->runtimeClassId)) {
      continue;
    }
    if (GameFactionRuntime_StructureProducesArmy(modelPayload,producerClassId,armyDefinition)) {
      GameFactionRuntime_StopProductionAndRefund
                (modelPayload,producerClassId == MODEL_RUNTIME_CLASS_22 ? 43 : 46,armyDefinition,factionIndex);
      requestedCount--;
      if (requestedCount == 0) {
        return;
      }
    }
  }
}


/* Takes the first entry equal to armyDefinition out of the faction's primary army-asset list
   (primaryArmyAssetPointersOrIds),
   moving the later entries down one slot, and returns true; returns false when there is none.
   Original quirk: the last move reads the entry one past the count (with a full list of 64 that is the first
   runtime group member pointer that follows the list). */
static Bool8 GameFactionRuntime_RemoveFirstPrimaryArmyAsset(GameFactionRuntimeRecord *factionRecord,
          const ArmyAssetRecordPrefix *armyDefinition)
{
  uint32_t *primaryAssets;
  FactionArmyAssetCount assetIndex;

  primaryAssets = factionRecord->primaryArmyAssetPointersOrIds;
  for (assetIndex = 0; assetIndex < factionRecord->primaryArmyAssetCount; assetIndex++) {
    if (armyDefinition == (const ArmyAssetRecordPrefix *)primaryAssets[assetIndex]) { /* 5f-format: GameFactionRuntimeRecord.primaryArmyAssetPointersOrIds */
      /* close the gap */
      for (; assetIndex < factionRecord->primaryArmyAssetCount; assetIndex++) {
        primaryAssets[assetIndex] = primaryAssets[assetIndex + 1];
      }
      factionRecord->primaryArmyAssetCount--;
      return true;
    }
  }
  return false;
}


/* In-game command INGAME_COMMAND_TAKE_ARMY_FOR_PLACEMENT (clicking a finished army in the in-game catalog):
   takes the first entry of the army record out of the faction's primary army-asset list
   (primaryArmyAssetPointersOrIds) and stages it in the player's pending slot (pendingPlacementArmyAsset) for
   placement on the map. When the faction is the one shown
   the command sprite grid is rebuilt, and for the local player the placement cursor is armed. If the faction
   has no such entry the pending slot is cleared again.
*/
void GameFactionRuntime_RemoveArmyAssetAndStagePlayerTransfer
          (PlayerRuntimeId playerRuntimeId,uint32_t unusedZero,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex)

{
  SelectionPlayerRuntimeBlock *playerBlock;
  ArmyAssetRecordPrefix *armyDefinition;

  if (ArmyAssetRegistry_FindById(armyAssetId,&armyDefinition) != 0) {
    return;
  }
  playerBlock = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId];
  playerBlock->pendingPlacementArmyAsset = (uintptr_t)armyDefinition;
  if (!GameFactionRuntime_RemoveFirstPrimaryArmyAsset(&g_GameFactionRuntimeImage.records[factionIndex],
                                                      armyDefinition)) {
    playerBlock->pendingPlacementArmyAsset = 0;
    return;
  }
  if (factionIndex != (g_InGameRuntimeRoot->worldRuntime).activeFactionRuntimeIndex) {
    return;
  }
  InGameArmyStock_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
  if (playerRuntimeId != g_LocalPlayerRuntimeId) {
    return;
  }
  g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | UI_COMMAND_RUNTIME_FLAG_PLACEMENT_PENDING;
  g_InGamePendingPlacementArmyAsset = (intptr_t)armyDefinition;
  g_InGamePlacementSurfaceHeightQ12OrSentinel = INT32_MAX; /* no surface picked yet */
}


/* In-game command handler, the reverse of GameFactionRuntime_RemoveArmyAssetAndStagePlayerTransfer: takes the
   army asset staged for the player's placement (exchanged with 0) back into the faction's primary army-asset list
   (at most 64 entries). When the faction is the one shown it rebuilds the command sprite grid, and for the local
   player it ends the pending placement.
*/
void GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid
          (PlayerRuntimeId playerRuntimeId,uint32_t unusedConsumeArgument0,uint32_t unusedConsumeArgument1
          ,FactionRuntimeIndex factionIndex)

{
  uintptr_t pendingAsset;
  uint32_t assetCount;
  InGameRuntimeRoot *runtimeRoot;

  LOCK();
  pendingAsset = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->pendingPlacementArmyAsset;
  g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->pendingPlacementArmyAsset = 0;
  runtimeRoot = g_InGameRuntimeRoot;
  UNLOCK();
  if (pendingAsset == 0) {
    return;
  }
  assetCount = g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
  if (assetCount < FACTION_ARMY_ASSET_LIST_CAPACITY) {
    /* primaryArmyAssetPointersOrIds[assetCount] */
    g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetPointersOrIds[assetCount] =
         (uint32_t)pendingAsset; /* 5f-format: GameFactionRuntimeRecord.primaryArmyAssetPointersOrIds */
    g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount++;
  }
  if (factionIndex == runtimeRoot->worldRuntime.activeFactionRuntimeIndex) {
    InGameArmyStock_RebuildGrid((UiNodeBase *)runtimeRoot);
    if (playerRuntimeId == g_LocalPlayerRuntimeId) {
      g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & ~UI_COMMAND_RUNTIME_FLAG_PLACEMENT_PENDING;
      g_InGamePendingPlacementArmyAsset = 0;
    }
  }
}


/* In-game command INGAME_COMMAND_SELL_ARMY (the sell click on a finished army in the in-game catalog): takes the
   first entry of the army record out of the faction's primary army-asset list (primaryArmyAssetPointersOrIds)
   and credits 7/8 of its price to the faction's xenite. The command sprite grid is rebuilt when the faction is
   the one shown.
*/
void GameFactionRuntime_SellArmyAssetAndRefundSevenEighths
          (uint32_t unusedPlayerRuntimeId,uint32_t unusedZero,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex)

{
  PckArmyAssetIdCatalog price;
  ArmyAssetRecordPrefix *resolvedAsset;

  if (ArmyAssetRegistry_FindById(armyAssetId,&resolvedAsset) != 0) {
    return;
  }
  if (!GameFactionRuntime_RemoveFirstPrimaryArmyAsset(&g_GameFactionRuntimeImage.records[factionIndex],
                                                      resolvedAsset)) {
    return;
  }
  price = resolvedAsset[2].registryId;
  g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 =
       g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 + ((int)(price * 7) >> 3);
  if ((g_InGameRuntimeRoot->worldRuntime).activeFactionRuntimeIndex == factionIndex) {
    InGameArmyStock_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
  }
}


/* In-game command INGAME_COMMAND_PLACEMENT_CREATE_ARMY (map click while placing an army in command mode 3/4, from
   InGameUiCommand_BeginInteractionByMode): creates army armyAssetId at the clicked position for the faction set
   by PlayerRuntime_SetPlacementFaction and keeps it as the player's placed army (placedArmyToken, as an offset from
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
    playerBlock->placedArmyToken =
         (uint32_t)((uintptr_t)createdRuntime - (uintptr_t)g_ArmyRuntimeRebaseBaseMinusOne);
    return;
  }
  playerBlock->placedArmyToken = 0;
}


/* In-game command INGAME_COMMAND_PLACEMENT_SET_FACTION (from InGameUiCommand_BeginInteractionByMode, before
   INGAME_COMMAND_PLACEMENT_CREATE_ARMY): sets the faction (placementFactionIndex) that the player's next placed
   army belongs to.
*/
void PlayerRuntime_SetPlacementFaction(PlayerRuntimeId playerRuntimeId,uint32_t unusedZero0,uint32_t unusedZero1,
          PlacementFactionIndex placementFactionIndex)

{
  g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->placementFactionIndex = placementFactionIndex;
}


/* In-game command INGAME_COMMAND_PLACEMENT_SET_ARMY (clicking an existing army in placement sub-mode 2, from
   InGameUiCommand_BeginInteractionByMode): makes it the player's placed army (placedArmyToken); armyToken is its
   offset from g_ArmyRuntimeRebaseBaseMinusOne.
*/
void PlayerRuntime_SetPlacementArmy(PlayerRuntimeId playerRuntimeId,uint32_t unusedZero0,uint32_t unusedZero1,
          PlacedArmyToken armyToken)

{
  g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->placedArmyToken = armyToken;
}


/* In-game command INGAME_COMMAND_PLACEMENT_CLEAR_ARMY (end of a placement interaction, from
   InGameUiCommand_EndInteractionByMode): forgets the player's placed army (placedArmyToken).
*/
void PlayerRuntime_ClearPlacementArmy(PlayerRuntimeId playerRuntimeId,uint32_t unusedZero0,uint32_t unusedZero1,
          uint32_t unusedZero2)

{
  g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->placedArmyToken = 0;
}


/* Applies the mission carry-over stored by OldUnitRuntime_RebuildScenarioReplayTables at the start of the next
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
      runtimeRoot = g_InGameRuntimeRoot; /* the original reads it once after the loop */
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
      /* record [2] and [3] go to the parameters named worldYQ12 / worldXQ12 (passed as in the original),
         although the rebuild stores the X coordinate in [2] */
      ArmyRuntime_CreateInstanceFromAsset
                (ARMY_CREATE_COUNT_FOR_ACTIVE_FACTION | ARMY_CREATE_UNLOCK_TECHNOLOGY,primaryRecordCursor[4],
                 primaryRecordCursor[3],primaryRecordCursor[2],primaryRecordCursor[1],*primaryRecordCursor,
                      worldRuntime,NULL);
      primaryRecordCursor = primaryRecordCursor + 8; /* 0x20-byte records */
      recordsRemaining--;
    } while (recordsRemaining != 0);
    /* signature differs: the callbacks' context is WorldRuntimeContext *, the slot's void * */
    WorldRuntime_ForEachOwnerListNode
              (worldRuntime,
               (WorldRuntimeNodeTraversalCallback *)ArmyRuntimeNode_AccumulateTerrainOcclusionAndOccupancyCallback,
               worldRuntime);
    WorldRuntime_ForEachOwnerListNode
              (worldRuntime,
               (WorldRuntimeNodeTraversalCallback *)ArmyRuntimeNode_RebuildTerrainOccupancyAndVisualStateCallback,
               worldRuntime);
    FieldGrid_ClassifyCellFlagsToRuntimeByte
              ((runtimeRoot->worldRuntime).activeFactionRuntimeIndex,
               (runtimeRoot->worldRuntime).fieldGrid);
  }
  return;
}


/* Returns true when the relation of factionIndex towards otherFactionIndex is in one of the pending states
   2, 5 or 9 and the pair's last relation change is at most 600 ticks old, so the relation does not advance
   again too soon.
*/
Bool8 GameFactionRuntime_IsRecentTimedRelationState
          (FactionRuntimeIndex otherFactionIndex,FactionRuntimeIndex factionIndex)

{
  uint32_t relationStateNibble;

  relationStateNibble =
       g_GameFactionRuntimeImage.records[factionIndex].packedRelationStates >>
       ((char)otherFactionIndex * 4 & 31U) & 0xf;
  /* relationStateTicks[other faction]: tick of the pair's last relation change */
  if ((((relationStateNibble == 2) || (relationStateNibble == 5)) || (relationStateNibble == 9)) &&
     ((int)(g_GameFactionRuntimeImage.tail.simulationTick -
           (int)g_GameFactionRuntimeImage.records[factionIndex].relationStateTicks[otherFactionIndex]) <
      FACTION_RELATION_CHANGE_COOLDOWN_TICKS + 1)) {
    return true;
  }
  return false;
}


/* Drops any pending mission carry-over: clears the 64-dword technology-mask table and the unit-record count, so
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


/* Appends an absorbed faction's army-asset list to the survivor's list of the same kind, stopping when the
   survivor's 64 entries are full. */
static void GameFactionRuntime_AppendArmyAssetList(FactionArmyAssetCount *survivorCount,uint32_t *survivorList,
          FactionArmyAssetCount absorbedCount,const uint32_t *absorbedList)
{
  FactionArmyAssetCount assetsRemaining;
  uint32_t armyAssetReferenceDword;
  uint32_t slotIndex;
  int sourceIndex;

  assetsRemaining = absorbedCount;
  sourceIndex = 0;
  for (slotIndex = *survivorCount;
      (assetsRemaining != 0) && (slotIndex < FACTION_ARMY_ASSET_LIST_CAPACITY); slotIndex++) {
    armyAssetReferenceDword = absorbedList[sourceIndex];
    *survivorCount = *survivorCount + 1;
    survivorList[slotIndex] = armyAssetReferenceDword;
    sourceIndex++;
    assetsRemaining--;
  }
}


/* Merge (relation state 11) of GameFactionRuntime_ApplyPairwiseRelationTransition: survivingFactionIndex takes
   over absorbedFactionIndex's models (re-owned and repainted), players, per-faction cell bytes, resources,
   technology, statistics and army-asset lists; the absorbed faction becomes inactive and the in-game catalogs
   are rebuilt. */
static void GameFactionRuntime_MergeAbsorbedFaction(FactionRuntimeIndex survivingFactionIndex,
          FactionRuntimeIndex absorbedFactionIndex)
{
  TritiumAmountQ4 *tritiumField;
  XeniteAmountQ4 *xeniteField;
  EnergyAmountQ4 *energyField;
  FactionRelationCounter *relationCounter;
  TritiumAmountQ4 tritiumAmount;
  XeniteAmountQ4 xeniteLimit;
  TritiumAmountQ4 tritiumLimit;
  EnergyAmountQ4 energyCapacity;
  TritiumAmountQ4 tritiumExtracted;
  FactionRelationCounter counterA;
  FactionRelationCounter counterB;
  FactionRelationCounter counterD;
  FactionRelationCounter counterE;
  FactionRelationCounter counterF;
  int maskWordIndex;
  int playerRuntimeId;
  int cellsRemaining;
  FrontendPlayerRuntimeRecord *playerBlockCursor;
  FrontendPlayerRuntimeBlockCount playerBlocksRemaining;
  FieldGridCell *gridCell;
  GraphicsTextureSet *survivingFactionTextureSet;
  GraphicsPaletteAsset *survivingFactionPaletteAsset;
  FieldGridAsset *terrainGrid;
  InGameRuntimeRoot *runtimeRoot;
  WorldOwnerListNode *ownerNode;
  ArmyRuntimeSlot *armyRuntime;
  GameFactionRuntimeRecord *survivor;
  GameFactionRuntimeRecord *absorbed;

  runtimeRoot = g_InGameRuntimeRoot;
  if (absorbedFactionIndex == g_InGameRuntimeRoot->worldRuntime.activeFactionRuntimeIndex) {
    g_InGameRuntimeRoot->worldRuntime.activeFactionRuntimeIndex = survivingFactionIndex;
  }
  survivingFactionTextureSet = g_ArmyGraphicsBindings[survivingFactionIndex].textureSet;
  survivingFactionPaletteAsset = g_ArmyGraphicsBindings[survivingFactionIndex].paletteAsset;
  for (ownerNode = (runtimeRoot->worldRuntime).ownerListHead; ownerNode != NULL; ownerNode = ownerNode->nextNode) {
    /* re-own the absorbed faction's models (their army's factionIndex) and repaint them in the survivor's colours */
    if (ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
      armyRuntime = ((ModelRuntimeSlot *)ownerNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime;
      if (armyRuntime->factionIndex == absorbedFactionIndex) {
        armyRuntime->factionIndex = survivingFactionIndex;
        ModelRuntimeHierarchy_SetPaletteAndTextureSetNonNullRecursive
                  (survivingFactionPaletteAsset,survivingFactionTextureSet,armyRuntime->modelNodeRuntime);
      }
    }
  }
  /* the player blocks are read after the repaint walk */
  playerBlockCursor = g_FrontendPlayerRuntimeBlocks;
  playerBlocksRemaining = g_FrontendPlayerRuntimeBlockCount;
  do {
    if (absorbedFactionIndex == (playerBlockCursor->factionAssignment).factionAssignmentIndex) {
      playerRuntimeId = playerBlockCursor->playerRuntimeId;
      (playerBlockCursor->factionAssignment).factionAssignmentIndex = survivingFactionIndex;
      g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->factionIndex = survivingFactionIndex;
    }
    playerBlockCursor++;
    playerBlocksRemaining--;
  } while (playerBlocksRemaining != 0);
  /* the survivor also gets the absorbed faction's per-faction cell byte (its byte of the cell's occupancyMask) */
  terrainGrid = runtimeRoot->worldRuntime.fieldGrid;
  cellsRemaining = terrainGrid->gridWidth * terrainGrid->gridHeight;
  gridCell = terrainGrid->cells;
  do {
    ((uint8_t *)&gridCell->occupancyMask)[survivingFactionIndex] =
         ((uint8_t *)&gridCell->occupancyMask)[survivingFactionIndex] |
         ((uint8_t *)&gridCell->occupancyMask)[absorbedFactionIndex];
    gridCell++;
    cellsRemaining--;
  } while (cellsRemaining != 0);
  g_GameFactionRuntimeImage.tail.factionLifecycleStates[absorbedFactionIndex] = FACTION_RUNTIME_LIFECYCLE_INACTIVE;
  survivor = &g_GameFactionRuntimeImage.records[survivingFactionIndex];
  absorbed = &g_GameFactionRuntimeImage.records[absorbedFactionIndex];
  tritiumAmount = absorbed->tritiumCurrentQ4;
  xeniteLimit = absorbed->xeniteStorageLimitQ4;
  tritiumLimit = absorbed->tritiumStorageLimitQ4;
  survivor->xeniteCurrentQ4 = survivor->xeniteCurrentQ4 + absorbed->xeniteCurrentQ4;
  tritiumField = &survivor->tritiumCurrentQ4;
  *tritiumField = *tritiumField + tritiumAmount;
  xeniteField = &survivor->xeniteStorageLimitQ4;
  *xeniteField = *xeniteField + xeniteLimit;
  tritiumField = &survivor->tritiumStorageLimitQ4;
  *tritiumField = *tritiumField + tritiumLimit;
  energyCapacity = absorbed->energyGenerationCapacityQ4;
  energyField = &survivor->baselineEnergySupplyQ4;
  *energyField = *energyField + absorbed->baselineEnergySupplyQ4;
  energyField = &survivor->energyGenerationCapacityQ4;
  *energyField = *energyField + energyCapacity;
  for (maskWordIndex = 0; maskWordIndex < 8; maskWordIndex++) {
    survivor->technologyMasks256Bits[maskWordIndex] =
         survivor->technologyMasks256Bits[maskWordIndex] | absorbed->technologyMasks256Bits[maskWordIndex];
  }
  tritiumExtracted = absorbed->tritiumExtractedTotalQ4;
  counterA = absorbed->relationCounterA;
  counterB = absorbed->relationCounterB;
  xeniteField = &survivor->xeniteExtractedTotalQ4;
  *xeniteField = *xeniteField + absorbed->xeniteExtractedTotalQ4;
  tritiumField = &survivor->tritiumExtractedTotalQ4;
  *tritiumField = *tritiumField + tritiumExtracted;
  relationCounter = &survivor->relationCounterA;
  *relationCounter = *relationCounter + counterA;
  relationCounter = &survivor->relationCounterB;
  *relationCounter = *relationCounter + counterB;
  counterD = absorbed->relationCounterD;
  counterE = absorbed->relationCounterE;
  counterF = absorbed->relationCounterF;
  relationCounter = &survivor->relationCounterC;
  *relationCounter = *relationCounter + absorbed->relationCounterC;
  relationCounter = &survivor->relationCounterD;
  *relationCounter = *relationCounter + counterD;
  relationCounter = &survivor->relationCounterE;
  *relationCounter = *relationCounter + counterE;
  relationCounter = &survivor->relationCounterF;
  *relationCounter = *relationCounter + counterF;
  /* append both army-asset lists (primaryArmyAssetPointersOrIds, secondaryArmyAssetPointersOrIds; 64 entries
     each) */
  GameFactionRuntime_AppendArmyAssetList(&survivor->primaryArmyAssetCount,survivor->primaryArmyAssetPointersOrIds,
                                         absorbed->primaryArmyAssetCount,absorbed->primaryArmyAssetPointersOrIds);
  GameFactionRuntime_AppendArmyAssetList(&survivor->secondaryArmyAssetCount,
                                         survivor->secondaryArmyAssetPointersOrIds,
                                         absorbed->secondaryArmyAssetCount,absorbed->secondaryArmyAssetPointersOrIds);
  InGameArmyStock_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
  InGameSpecialBuildCatalog_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
  InGameBuildCatalog_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
}


/* Changes the diplomatic relation between two factions: notifies the shown faction (text code + 500), stores
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
  InGameSimulationTick currentTick;
  FactionRuntimeIndex originalFirstFactionIndex;
  uint32_t secondFactionBit;
  uint32_t firstFactionBit;
  uint32_t secondFactionPlayerCount;
  uint32_t firstFactionPlayerCount;
  uint32_t randomValue;
  Bool8 swapMergeDirection;
  uint8_t secondShift;
  uint8_t firstShift;
  FrontendPlayerRuntimeRecord *playerBlockCursor;
  FrontendPlayerRuntimeBlockCount playerBlocksRemaining;

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
  secondFactionBit = 1 << ((uint8_t)secondFactionIndex & 31);
  if (stateSecondTowardFirst < FACTION_RELATION_STATE_FRIENDLY) {
    capabilityFlagsField = &g_GameFactionRuntimeImage.records[firstFactionIndex].capabilityFlags;
    *capabilityFlagsField = *capabilityFlagsField & ~secondFactionBit;
  }
  else {
    capabilityFlagsField = &g_GameFactionRuntimeImage.records[firstFactionIndex].capabilityFlags;
    *capabilityFlagsField = *capabilityFlagsField | secondFactionBit;
  }
  firstFactionBit = 1 << ((uint8_t)firstFactionIndex & 31);
  if (stateSecondTowardFirst < FACTION_RELATION_STATE_FRIENDLY) {
    capabilityFlagsField = &g_GameFactionRuntimeImage.records[secondFactionIndex].capabilityFlags;
    *capabilityFlagsField = *capabilityFlagsField & ~firstFactionBit;
  }
  else {
    capabilityFlagsField = &g_GameFactionRuntimeImage.records[secondFactionIndex].capabilityFlags;
    *capabilityFlagsField = *capabilityFlagsField | firstFactionBit;
  }
  /* tick of the last relation change per pair (relationStateTicks[other faction]) */
  currentTick = g_GameFactionRuntimeImage.tail.simulationTick;
  g_GameFactionRuntimeImage.records[firstFactionIndex].relationStateTicks[secondFactionIndex] =
       g_GameFactionRuntimeImage.tail.simulationTick;
  g_GameFactionRuntimeImage.records[secondFactionIndex].relationStateTicks[firstFactionIndex] = currentTick;
  if (stateSecondTowardFirst == FACTION_RELATION_MERGE) {
    secondFactionPlayerCount = 0;
    firstFactionPlayerCount = 0;
    playerBlocksRemaining = g_FrontendPlayerRuntimeBlockCount;
    playerBlockCursor = g_FrontendPlayerRuntimeBlocks;
    do {
      if (secondFactionIndex == (playerBlockCursor->factionAssignment).factionAssignmentIndex) {
        secondFactionPlayerCount++;
      }
      if (firstFactionIndex == (playerBlockCursor->factionAssignment).factionAssignmentIndex) {
        firstFactionPlayerCount++;
      }
      playerBlockCursor++;
      playerBlocksRemaining--;
    } while (playerBlocksRemaining != 0);
    /* State 11 merges the two factions. The second faction survives when it has players and the (bitwise) counts
       do not overlap; with no players on either side, or overlapping counts, a random bit decides. */
    if (((secondFactionPlayerCount & firstFactionPlayerCount) == 0) &&
        ((secondFactionPlayerCount != 0) || (firstFactionPlayerCount != 0))) {
      swapMergeDirection = secondFactionPlayerCount != 0;
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
    GameFactionRuntime_MergeAbsorbedFaction(firstFactionIndex,secondFactionIndex);
  }
  InGameOtherPlayerCommand_RebuildTargetEntries((UiNodeBase *)g_InGameRuntimeRoot);
}

