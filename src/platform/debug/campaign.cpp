/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/debug/campaign.cpp
 * Project code (not in the original game)
 */

#include <thandor/platform/debug/campaign.h>

#include <stdlib.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/test_aids.h>

/* Port-only test aid for campaign carry-over tests: OPEN_THANDOR_AUTOWIN=<seconds> ends a campaign level as won
   after that many seconds of the session (with OPEN_THANDOR_AUTOWIN_LEVELS=<k> only in the first k sessions). It
   picks the active end trigger of the level script whose end selection moves the campaign forward (a later
   level, else successor 0 = the campaign end; see the scoring below), moves the owner-list position of every
   mobile unit of the local faction that stands outside the local faction's exit zone of the campaign level
   record to the zone centre, and then sets the end movie state exactly as
   InGameConditionRuntime_UpdateScheduledRecords does for that trigger. From there the original code runs:
   the end movie, OldUnitRuntime_RebuildScenarioReplayTables (collects the units in the exit zone), the next
   level and OldUnitRuntime_MergeMasksAndReplayRecords. Called once per frame by InGameRuntime_RunSessionUntilExit. */
void DebugCampaign_AutoWinTick(void)
{
  static unsigned sessionSeen;
  static unsigned sessionStart;
  static int done;
  const char *value = getenv("OPEN_THANDOR_AUTOWIN");
  CampaignAsset *campaign = (CampaignAsset *)g_FrontendLoadedCampaignAsset;
  CampaignLevelRecord *level = NULL;
  InGameEndConditionTriggerRecord8 *chosen = NULL;
  int chosenScore = 0;
  InGameEndConditionTriggerRecord8 *trigger;
  WorldOwnerListNode *ownerNode;
  uint32_t localFaction;
  int index;
  int moved = 0;
  if (value == NULL || g_InGameRuntimeRoot == NULL) {
    return;
  }
  /* OPEN_THANDOR_AUTOWIN_LEVELS=<k>: only the first k sessions of the process end automatically */
  if (getenv("OPEN_THANDOR_AUTOWIN_LEVELS") != NULL &&
      g_TestAidSessionCount > (unsigned)atoi(getenv("OPEN_THANDOR_AUTOWIN_LEVELS"))) {
    return;
  }
  if (sessionSeen != g_TestAidSessionCount) {
    sessionSeen = g_TestAidSessionCount;
    sessionStart = Thandor_TickCount();
    done = 0;
  }
  if (done || Thandor_TickCount() - sessionStart < (unsigned)atoi(value) * 1000u) {
    return;
  }
  done = 1;
  if (campaign == NULL) {
    Thandor_Log("test aid: auto-win: not a campaign level");
    return;
  }
  for (index = 0; index < campaign->levelRecordCount; index++) {
    if (campaign->levels[index].levelId == campaign->currentLevelId) {
      level = &campaign->levels[index];
    }
  }
  if (level == NULL) {
    Thandor_Log("test aid: auto-win: level %d has no campaign record", campaign->currentLevelId);
    return;
  }
  localFaction = (g_InGameRuntimeRoot->worldRuntime).activeFactionRuntimeIndex;
  /* the trigger that moves the campaign forward: a successor with a higher level id (score 3), else successor 0,
     the campaign end (2), else another known level (1); a trigger ending another faction than the local one wins
     a tie. A successor equal to the current level (replay) or unknown is never chosen, except as the last resort
     in a level without any other trigger. */
  trigger = (InGameEndConditionTriggerRecord8 *)(g_InGameLevelRuntimeGlobalBlock.conditionStorage)->schedule.triggers;
  for (index = 0; index < INGAME_END_CONDITION_TRIGGER_COUNT; index++, trigger++) {
    uint32_t selection = trigger->endMovieSelectionIndex;
    int successor;
    int score = 0;
    int other;
    if (trigger->stateFlags != INGAME_END_CONDITION_TRIGGER_ACTIVE || selection >= 8) {
      continue;
    }
    successor = level->successorLevelIds[selection];
    Thandor_Log("test aid: auto-win: level %d trigger %d: ends faction %u, end selection %u -> level %d", level->levelId,
                index, (unsigned)trigger->factionRuntimeIndex, (unsigned)selection, successor);
    for (other = 0; other < campaign->levelRecordCount; other++) {
      if (campaign->levels[other].levelId == successor && successor != level->levelId) {
        score = successor > level->levelId ? 3 : 1;
      }
    }
    if (successor == 0) {
      score = 2;
    }
    score = score * 2 + (trigger->factionRuntimeIndex != localFaction);
    if (chosen == NULL || score > chosenScore) {
      chosen = trigger;
      chosenScore = score;
    }
  }
  if (chosen == NULL) {
    Thandor_Log("test aid: auto-win: level %d has no active end trigger", level->levelId);
    return;
  }
  if (level->successorLevelIds[chosen->endMovieSelectionIndex] == 0 || chosenScore < 2) {
    Thandor_Log("test aid: auto-win: level %d has no successor level, firing the campaign end", level->levelId);
  }
  for (ownerNode = (g_InGameRuntimeRoot->worldRuntime).ownerListHead; ownerNode != NULL;
       ownerNode = ownerNode->nextNode) {
    ModelRuntimeSlot *model;
    if (ownerNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
      continue;
    }
    model = (ModelRuntimeSlot *)ownerNode->runtimePayload;
    /* units already inside the zone stay where they are (many levels use centre 0,0 with a radius covering the
       whole map, i.e. everything is carried over at its position); radius 0 means no carry-over */
    if (level->exitZoneRadius[localFaction] <= 0 ||
        model->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex != localFaction ||
        model->definitionOrSavedId.runtimeDefinition->accelerationPerTick == 0 ||
        (int)FixedMath_Length2(level->exitZoneCenterY[localFaction] - ownerNode->worldYQ12,
                               level->exitZoneCenterX[localFaction] - ownerNode->worldXQ12) <=
            level->exitZoneRadius[localFaction]) {
      continue;
    }
    ownerNode->worldXQ12 = level->exitZoneCenterX[localFaction];
    ownerNode->worldYQ12 = level->exitZoneCenterY[localFaction];
    moved++;
  }
  /* end movie state as the level script sets it for this trigger (the ended faction is another one than the
     local one when a win trigger was found) */
  g_EndMovieVariantIndex = chosen->movieVariantSelector;
  if (chosen->factionRuntimeIndex != localFaction) {
    g_EndMovieVariantIndex = chosen->movieVariantSelector ^ 1;
  }
  g_EndMovieSelectionIndex = chosen->endMovieSelectionIndex;
  g_EndMoviePath = (uint16_t *)g_SessionEndMoviePathUtf16;
  if (g_EndMovieVariantIndex == 0) {
    g_EndMoviePath = (uint16_t *)g_FlmEnde0001FlmPathUtf16;
  }
  g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING;
  Thandor_Log("test aid: auto-win: level %d, end selection %u -> level %d, exit zone radius %d, "
              "%d mobile units of faction %u outside it moved into it", level->levelId, (unsigned)chosen->endMovieSelectionIndex,
              level->successorLevelIds[chosen->endMovieSelectionIndex], level->exitZoneRadius[localFaction],
              moved, localFaction);
  Thandor_Log("test aid: auto-win: exit zone centre (%d,%d), destination (%d,%d)",
              level->exitZoneCenterX[localFaction], level->exitZoneCenterY[localFaction],
              level->exitZoneDestinationX[localFaction], level->exitZoneDestinationY[localFaction]);
}

/* Port-only test aid: logs the campaign carry-over at level start, called by InGameRuntime_InitializeNewSession
   around OldUnitRuntime_MergeMasksAndReplayRecords. Before the merge (afterMerge 0) it logs the number of carried
   units; after it (afterMerge 1) the per-faction counts and where the carried units of the local faction landed,
   against the level's start view. */
void DebugCampaign_LogCarryOver(int afterMerge)
{
  if (!afterMerge) {
    Thandor_Log("level start: campaign carries over %u units from the previous level",
                (unsigned)g_OldUnitRecordCount);
    return;
  }
  if (g_InGameRuntimeRoot != NULL && g_OldUnitRecordCount != 0) {
    /* where the carried units of the local faction landed, against the level's start view */
    WorldRuntimeContext *world = &g_InGameRuntimeRoot->worldRuntime;
    unsigned perFaction[8] = {0}, record, shown = 0;
    unsigned localFaction = world->activeFactionRuntimeIndex;
    for (record = 0; record < (unsigned)g_OldUnitRecordCount && record < OLD_UNIT_PRIMARY_RECORD_CAPACITY; record++) {
      perFaction[g_OldUnitPrimaryTable[record * 8 + 1] & 7]++;
    }
    Thandor_Log("level start: view target (%d,%d), local faction %u, carried per faction "
                "%u %u %u %u %u %u %u %u", world->motion.targetPositionXQ12,
                world->motion.targetPositionYQ12, localFaction, perFaction[0], perFaction[1],
                perFaction[2], perFaction[3], perFaction[4], perFaction[5], perFaction[6],
                perFaction[7]);
    for (record = 0; record < (unsigned)g_OldUnitRecordCount && record < OLD_UNIT_PRIMARY_RECORD_CAPACITY && shown < 4;
         record++) {
      if (g_OldUnitPrimaryTable[record * 8 + 1] == localFaction) {
        shown++;
        Thandor_Log("level start: local carried unit, army asset %u at (%d,%d)",
                    (unsigned)g_OldUnitPrimaryTable[record * 8],
                    (int)g_OldUnitPrimaryTable[record * 8 + 2],
                    (int)g_OldUnitPrimaryTable[record * 8 + 3]);
      }
    }
  }
}

/* Port-only test aid: UTF-16 list row text as ANSI for the log (non-ASCII becomes '?'). */
static const char *DebugCampaign_RowName(const uint16_t *text, char *out, unsigned capacity)
{
  unsigned length = 0;
  while (text[length] != 0 && length + 1 < capacity) {
    out[length] = (text[length] < 128) ? (char)text[length] : '?';
    length++;
  }
  if (length != 0 && out[length - 1] == '.') { /* the list rows keep the extension dot of the file name */
    length--;
  }
  out[length] = 0;
  return out;
}

/* Port-only test aid for unattended mission runs, called when the "Choose game" page opens in a local game:
     OPEN_THANDOR_LIST_SCENARIOS=1 logs the names of all single games and campaigns ("scenario: ...") and
                                   ends the process;
     OPEN_THANDOR_CAMPAIGN=<name>  starts that campaign (name as in the campaigns list, or its 0-based row)
                                   like its Start button, once per process; OPEN_THANDOR_CAMPAIGN_LEVEL picks
                                   the level (see DebugCampaign_SelectCampaignLevel).
   Returns nonzero when a campaign was started. */
int DebugCampaign_ApplyScenarioOptions(void)
{
  static int used;
  const char *wanted;
  UiListControl *list;
  char name[64];
  uint32_t row;
  if (used) {
    return 0;
  }
  used = 1;
  if (getenv("OPEN_THANDOR_LIST_SCENARIOS") != NULL) {
    ScenarioCatalog_RebuildLevelRecordListPage(g_LocalPlayerRuntimeId,0,0,0);
    list = (UiListControl *)FRONTEND_UI(g_FrontendRootNode,missionsList);
    for (row = 0; row < list->rowCount; row++) {
      Thandor_Log("scenario: single \"%s\"",
                  DebugCampaign_RowName((const uint16_t *)list->rowSlots[row],name,sizeof name));
    }
    ScenarioCatalog_RebuildCampaignRecordListPage(g_LocalPlayerRuntimeId,0,0,0);
    list = (UiListControl *)FRONTEND_UI(g_FrontendRootNode,campaignsList);
    for (row = 0; row < list->rowCount; row++) {
      Thandor_Log("scenario: campaign %u \"%s\"",row,
                  DebugCampaign_RowName((const uint16_t *)list->rowSlots[row],name,sizeof name));
    }
    ExitProcess(0);
  }
  wanted = getenv("OPEN_THANDOR_CAMPAIGN");
  if (wanted == NULL) {
    return 0;
  }
  ScenarioCatalog_RebuildCampaignRecordListPage(g_LocalPlayerRuntimeId,0,0,0);
  list = (UiListControl *)FRONTEND_UI(g_FrontendRootNode,campaignsList);
  for (row = 0; row < list->rowCount; row++) {
    DebugCampaign_RowName((const uint16_t *)list->rowSlots[row],name,sizeof name);
    if (_stricmp(name,wanted) == 0 ||
        (wanted[0] >= '0' && wanted[0] <= '9' && (uint32_t)atoi(wanted) == row)) {
      Thandor_Log("test aid: starting campaign %u \"%s\"",row,name);
      ScenarioCatalog_SelectCampaignAndShowDescription(g_LocalPlayerRuntimeId,0,0,row);
      FrontendScenarioSession_LoadOrRequestCampaignBundle(g_LocalPlayerRuntimeId,0,0,row);
      return 1;
    }
  }
  Thandor_Log("test aid: campaign \"%s\" not found",wanted);
  return 0;
}

/* Port-only test aid: logs the levels of a just loaded campaign (CampaignAsset) and, with
   OPEN_THANDOR_CAMPAIGN_LEVEL=<n>, makes its n-th level (1-based, in record order) the first one. */
void DebugCampaign_SelectCampaignLevel(uint8_t *campaignBytes)
{
  CampaignAsset *campaign = (CampaignAsset *)campaignBytes;
  const char *wanted = getenv("OPEN_THANDOR_CAMPAIGN_LEVEL");
  int count = campaign->levelRecordCount;
  int index;
  char name[64];
  for (index = 0; index < count; index++) {
    const CampaignLevelRecord *record = &campaign->levels[index];
    Thandor_Log("campaign level %d: id %d \"%s\"%s",index + 1,record->levelId,
                DebugCampaign_RowName(record->levelFileName,name,sizeof name),
                (record->levelId == campaign->firstLevelId) ? " (first)" : "");
  }
  if (wanted != NULL && atoi(wanted) >= 1 && atoi(wanted) <= count) {
    campaign->firstLevelId = campaign->levels[atoi(wanted) - 1].levelId;
    Thandor_Log("test aid: campaign starts at level %d",atoi(wanted));
  }
}
