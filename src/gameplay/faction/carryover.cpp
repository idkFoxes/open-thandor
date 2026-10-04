/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/faction/carryover.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/faction/carryover.h>
#include <thandor/thandor.h>

/* Module data. */

uint32_t *g_OldUnitSecondaryTable = 0;

uint32_t *g_OldUnitPrimaryTable = 0;

OldUnitRecordCount g_OldUnitRecordCount = 0;

/* Implementation ownership: gameplay/faction/carryover. */

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
