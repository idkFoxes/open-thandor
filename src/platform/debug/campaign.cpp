/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/debug/campaign.cpp
 * Project code (not in the original game)
 */

#include <thandor/platform/debug/campaign.h>

#include <stdio.h>
#include <stdlib.h>
#include <optional>
#include <string>
#include <thandor/thandor.h>
#include <thandor/gameplay/session/campaign_progress.h>
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
   level and OldUnitRuntime_MergeMasksAndReplayRecords. Called once per frame by InGameRuntime_RunSessionUntilExit.
   OPEN_THANDOR_AUTOWIN_SELECTION=<n> prefers the active end trigger with end selection n over the scoring (a
   branch test; a level without such a trigger falls back to the scoring, with one log line). */
void DebugCampaign_AutoWinTick()
{
  static unsigned sessionSeen;
  static unsigned sessionStart;
  static int done;
  const char *value = getenv("OPEN_THANDOR_AUTOWIN");
  CampaignAsset *campaign = g_FrontendLoadedCampaignAsset;
  CampaignLevelRecord *level = nullptr;
  InGameEndConditionTriggerRecord8ReferenceView *chosen = nullptr;
  int chosenScore = 0;
  int chosenBaseScore = 0;
  const char *preferredValue = getenv("OPEN_THANDOR_AUTOWIN_SELECTION");
  int preferredSelection = (preferredValue != nullptr) ? atoi(preferredValue) : -1;
  InGameEndConditionTriggerRecord8ReferenceView *trigger;
  WorldOwnerListNode *ownerNode;
  uint32_t localFaction;
  int index;
  int moved = 0;
  if (value == nullptr || g_InGameRuntimeRoot == nullptr) {
    return;
  }
  /* OPEN_THANDOR_AUTOWIN_LEVELS=<k>: only the first k sessions of the process end automatically */
  if (getenv("OPEN_THANDOR_AUTOWIN_LEVELS") != nullptr &&
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
  if (campaign == nullptr) {
    Thandor_Log("test aid: auto-win: not a campaign level");
    return;
  }
  for (index = 0; index < campaign->levelRecordCount; index++) {
    if (campaign->levels[index].levelId == campaign->currentLevelId) {
      level = &campaign->levels[index];
    }
  }
  if (level == nullptr) {
    Thandor_Log("test aid: auto-win: level %d has no campaign record", campaign->currentLevelId);
    return;
  }
  localFaction = (g_InGameRuntimeRoot->worldRuntime).activeFactionRuntimeIndex;
  /* the trigger that moves the campaign forward: a successor with a higher level id (score 3), else successor 0,
     the campaign end (2), else another known level (1); a trigger ending another faction than the local one wins
     a tie. A successor equal to the current level (replay) or unknown is never chosen, except as the last resort
     in a level without any other trigger. */
  trigger = (g_InGameLevelRuntimeGlobalBlock.conditionStorage)->schedule.triggers;
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
    int baseScore = score;
    score = score * 2 + (trigger->factionRuntimeIndex != localFaction);
    /* OPEN_THANDOR_AUTOWIN_SELECTION: the preferred end selection beats every scored one */
    if ((int)selection == preferredSelection) {
      score += 16;
    }
    if (chosen == nullptr || score > chosenScore) {
      chosen = trigger;
      chosenScore = score;
      chosenBaseScore = baseScore;
    }
  }
  if (chosen != nullptr && preferredSelection >= 0 && (int)chosen->endMovieSelectionIndex != preferredSelection) {
    Thandor_Log("test aid: auto-win: level %d has no active end trigger with end selection %d, using the scoring",
                level->levelId, preferredSelection);
  }
  if (chosen == nullptr) {
    Thandor_Log("test aid: auto-win: level %d has no active end trigger", level->levelId);
    return;
  }
  if (level->successorLevelIds[chosen->endMovieSelectionIndex] == 0 || chosenBaseScore == 0) {
    Thandor_Log("test aid: auto-win: level %d has no successor level, firing the campaign end", level->levelId);
  }
  for (ownerNode = (g_InGameRuntimeRoot->worldRuntime).ownerListHead; ownerNode != nullptr;
       ownerNode = ownerNode->nextNode) {
    ModelRuntimeSlot *model;
    if (ownerNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
      continue;
    }
    model = WorldOwnerNode_ModelRuntime(ownerNode);
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
  g_EndMoviePath = g_SessionEndMoviePathUtf16;
  if (g_EndMovieVariantIndex == 0) {
    g_EndMoviePath = g_FlmEnde0001FlmPathUtf16;
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
  if (g_InGameRuntimeRoot != nullptr && g_OldUnitRecordCount != 0) {
    /* where the carried units of the local faction landed, against the level's start view */
    WorldRuntimeContext *world = &g_InGameRuntimeRoot->worldRuntime;
    unsigned perFaction[8] = {}, record, shown = 0;
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
                                   the level (see DebugCampaign_SelectCampaignLevel); OPEN_THANDOR_CAMPAIGN_MISSION=
                                   <levelId> starts it there through the campaign map path (the start level
                                   argument, checked against the progress; a rejected level starts the first).
   Returns nonzero when a campaign was started. */
int DebugCampaign_ApplyScenarioOptions()
{
  static int used;
  const char *wanted;
  UiListControl *list;
  char name[64];
  uint32_t row;
  const char *mission = getenv("OPEN_THANDOR_CAMPAIGN_MISSION");
  uint32_t startLevelId = 0;
  if (used) {
    return 0;
  }
  used = 1;
  if (getenv("OPEN_THANDOR_LIST_SCENARIOS") != nullptr) {
    ScenarioCatalog_RebuildLevelRecordListPage(g_LocalPlayerRuntimeId,0,0,0);
    list = &g_FrontendRootNode->missionsList;
    for (row = 0; row < list->rowCount; row++) {
      Thandor_Log("scenario: single \"%s\"",
                  DebugCampaign_RowName(static_cast<const uint16_t *>(list->rowSlots[row].get()),name,sizeof name));
    }
    ScenarioCatalog_RebuildCampaignRecordListPage(g_LocalPlayerRuntimeId,0,0,0);
    list = &g_FrontendRootNode->campaignsList;
    for (row = 0; row < list->rowCount; row++) {
      Thandor_Log("scenario: campaign %u \"%s\"",row,
                  DebugCampaign_RowName(static_cast<const uint16_t *>(list->rowSlots[row].get()),name,sizeof name));
    }
    ExitProcess(0);
  }
  wanted = getenv("OPEN_THANDOR_CAMPAIGN");
  if (wanted == nullptr) {
    return 0;
  }
  ScenarioCatalog_RebuildCampaignRecordListPage(g_LocalPlayerRuntimeId,0,0,0);
  list = &g_FrontendRootNode->campaignsList;
  for (row = 0; row < list->rowCount; row++) {
    DebugCampaign_RowName(static_cast<const uint16_t *>(list->rowSlots[row].get()),name,sizeof name);
    if (_stricmp(name,wanted) == 0 ||
        (wanted[0] >= '0' && wanted[0] <= '9' && (uint32_t)atoi(wanted) == row)) {
      Thandor_Log("test aid: starting campaign %u \"%s\"",row,name);
      ScenarioCatalog_SelectCampaignAndShowDescription(g_LocalPlayerRuntimeId,0,0,row);
      /* OPEN_THANDOR_CAMPAIGN_MISSION=<levelId>: the start level argument of the campaign map's Start
         (FRONTEND_COMMAND_LOAD_CAMPAIGN, CampaignStart_Prepare checks it against the progress) */
      if (mission != nullptr) {
        startLevelId = (uint32_t)atoi(mission);
        Thandor_Log("test aid: campaign start level %u requested",startLevelId);
      }
      FrontendScenarioSession_LoadOrRequestCampaignBundle(g_LocalPlayerRuntimeId,startLevelId,0,row);
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
  CampaignAsset *campaign = reinterpret_cast<CampaignAsset *>(campaignBytes);
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
  if (wanted != nullptr && atoi(wanted) >= 1 && atoi(wanted) <= count) {
    campaign->firstLevelId = campaign->levels[atoi(wanted) - 1].levelId;
    Thandor_Log("test aid: campaign starts at level %d",atoi(wanted));
  }
}

/* Port-only test aid: logs the campaign level a session starts (id and file name), so a test can tell which
   mission a campaign start, a retry or a network peer loaded. Called at the session start of a campaign level. */
void DebugCampaign_LogLevelStart()
{
  const CampaignAsset *campaign = g_FrontendLoadedCampaignAsset;
  char name[64];
  if (campaign == nullptr) {
    return;
  }
  for (int index = 0; index < campaign->levelRecordCount; index++) {
    const CampaignLevelRecord *record = &campaign->levels[index];
    if (record->levelId == campaign->currentLevelId) {
      Thandor_Log("level start: campaign level id %d \"%s\"",record->levelId,
                  DebugCampaign_RowName(record->levelFileName,name,sizeof name));
      return;
    }
  }
  Thandor_Log("level start: campaign level id %d has no record",campaign->currentLevelId);
}

/* Port-only test aid for loss and retry tests: OPEN_THANDOR_AUTOLOSE=<seconds> ends the level as lost after that
   many seconds of the session, in the first session of the process only (OPEN_THANDOR_AUTOLOSE_LEVELS=<k>: the
   first k sessions), so a retry level started by the loss runs on. It fires the active end trigger that ends the
   local faction - preferring the failure movie (movie variant 0) and then end selection 0, the loss outcome of the
   stock campaigns - and sets the end movie state exactly as InGameConditionRuntime_RequestEndMovie does when the
   ended faction is the local one. From there the original code runs (end movie, carry-over tables, the successor
   of that selection, in the stock data the level itself). Nothing happens when an end is already pending or the
   level has no such trigger (one log line). Called once per frame by InGameRuntime_RunSessionUntilExit. */
void DebugCampaign_AutoLoseTick()
{
  static unsigned sessionSeen;
  static unsigned sessionStart;
  static int done;
  const char *value = getenv("OPEN_THANDOR_AUTOLOSE");
  const char *levelsValue = getenv("OPEN_THANDOR_AUTOLOSE_LEVELS");
  unsigned sessionLimit = (levelsValue != nullptr) ? (unsigned)atoi(levelsValue) : 1u;
  InGameLevelConditionStorage *storage;
  InGameEndConditionTriggerRecord8ReferenceView *chosen = nullptr;
  InGameEndConditionTriggerRecord8ReferenceView *trigger;
  int chosenScore = -1;
  uint32_t localFaction;
  int index;
  if (value == nullptr || g_InGameRuntimeRoot == nullptr || g_TestAidSessionCount > sessionLimit) {
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
  if (Any(g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING)) {
    Thandor_Log("test aid: auto-lose: the level is already ending");
    return;
  }
  storage = g_InGameLevelRuntimeGlobalBlock.conditionStorage.get();
  if (storage == nullptr) {
    Thandor_Log("test aid: auto-lose: the level has no level script");
    return;
  }
  localFaction = (g_InGameRuntimeRoot->worldRuntime).activeFactionRuntimeIndex;
  trigger = storage->schedule.triggers;
  for (index = 0; index < INGAME_END_CONDITION_TRIGGER_COUNT; index++, trigger++) {
    int score;
    if (trigger->stateFlags != INGAME_END_CONDITION_TRIGGER_ACTIVE || trigger->endMovieSelectionIndex >= 8 ||
        trigger->factionRuntimeIndex != localFaction) {
      continue;
    }
    score = (trigger->movieVariantSelector == 0 ? 2 : 0) + (trigger->endMovieSelectionIndex == 0 ? 1 : 0);
    if (score > chosenScore) {
      chosen = trigger;
      chosenScore = score;
    }
  }
  if (chosen == nullptr) {
    Thandor_Log("test aid: auto-lose: no active end trigger ends the local faction %u", (unsigned)localFaction);
    return;
  }
  /* InGameConditionRuntime_RequestEndMovie with the local faction as the ended one */
  g_EndMovieVariantIndex = chosen->movieVariantSelector;
  g_EndMovieSelectionIndex = chosen->endMovieSelectionIndex;
  g_EndMoviePath = g_SessionEndMoviePathUtf16;
  if (g_EndMovieVariantIndex == 0) {
    g_EndMoviePath = g_FlmEnde0001FlmPathUtf16;
  }
  g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING;
  Thandor_Log("test aid: auto-lose: trigger %d ends the local faction %u, end selection %u, movie variant %u",
              (int)(chosen - storage->schedule.triggers), (unsigned)localFaction,
              (unsigned)chosen->endMovieSelectionIndex, (unsigned)chosen->movieVariantSelector);
}

/* FNV-1a over the four bytes of a 32-bit value, for the progress dump's snapshot hash. */
static uint32_t DebugCampaign_HashWord(uint32_t hash, uint32_t word)
{
  for (int byte = 0; byte < 4; byte++) {
    hash = (hash ^ ((word >> (byte * 8)) & 0xffu)) * 16777619u;
  }
  return hash;
}

/* Logs a progress model, one line per item ("progress dump: ..."): per campaign its fingerprint, the won levels
   with their selection mask and last selection, the explicitly unlocked levels and per carry-over snapshot the
   source, selection, unit count, units per faction, nonzero technology words and an FNV-1a hash over units and
   technology (to compare snapshots between runs). Reads the model only. */
static void DebugCampaign_LogProgress(const CampaignProgress &progress)
{
  for (const CampaignProgressSection &section : progress.campaigns) {
    if (section.fingerprint.has_value()) {
      Thandor_Log("progress dump: campaign \"%s\" fingerprint crc32 %08X records %d", section.name.c_str(),
                  (unsigned)section.fingerprint->crc32, (int)section.fingerprint->recordCount);
    }
    else {
      Thandor_Log("progress dump: campaign \"%s\" without fingerprint", section.name.c_str());
    }
    for (const auto &[levelId, wins] : section.wins) {
      Thandor_Log("progress dump:   done %d selections %02X last %u", (int)levelId, (unsigned)wins.selectionMask,
                  (unsigned)wins.lastSelection);
    }
    if (!section.unlockedLevelIds.empty()) {
      std::string list;
      char number[16];
      for (int32_t levelId : section.unlockedLevelIds) {
        snprintf(number, sizeof number, " %d", (int)levelId);
        list += number;
      }
      Thandor_Log("progress dump:   unlocked%s", list.c_str());
    }
    for (const auto &[targetLevelId, carry] : section.carries) {
      unsigned perFaction[CAMPAIGN_PROGRESS_FACTION_COUNT] = {};
      unsigned techWords = 0;
      uint32_t hash = 2166136261u;
      for (const CampaignProgressUnit &unit : carry.units) {
        if (unit.faction >= 0 && unit.faction < CAMPAIGN_PROGRESS_FACTION_COUNT) {
          perFaction[unit.faction]++;
        }
        hash = DebugCampaign_HashWord(hash, (uint32_t)unit.assetId);
        hash = DebugCampaign_HashWord(hash, (uint32_t)unit.faction);
        hash = DebugCampaign_HashWord(hash, (uint32_t)unit.worldXQ12);
        hash = DebugCampaign_HashWord(hash, (uint32_t)unit.worldYQ12);
        hash = DebugCampaign_HashWord(hash, unit.rotation);
      }
      for (uint32_t word : carry.techMasks) {
        techWords += (word != 0) ? 1u : 0u;
        hash = DebugCampaign_HashWord(hash, word);
      }
      Thandor_Log("progress dump:   carry %d from %d sel %d units %u (per faction %u %u %u %u %u %u %u %u), "
                  "tech words %u, hash %08X", (int)targetLevelId, (int)carry.sourceLevelId, (int)carry.selection,
                  (unsigned)carry.units.size(), perFaction[0], perFaction[1], perFaction[2], perFaction[3],
                  perFaction[4], perFaction[5], perFaction[6], perFaction[7], techWords, (unsigned)hash);
    }
  }
}

/* Port-only test aid: OPEN_THANDOR_CAMPAIGN_PROGRESS_DUMP=1 logs the stored campaign progress, read through
   CampaignProgress_LoadCurrent (native memory and files only, no arena; a broken file is handled, and renamed,
   as the game's own load does). Called when a campaign was loaded (before its start reads the progress), when a
   campaign level has started (after its unlock note may have saved) and when a campaign session has ended
   through its end trigger (after a win may have saved); a progress equal to the one dumped last is logged as
   one "unchanged" line. */
void DebugCampaign_DumpProgress(const char *when)
{
  static std::optional<CampaignProgress> lastDumped;
  if (getenv("OPEN_THANDOR_CAMPAIGN_PROGRESS_DUMP") == nullptr) {
    return;
  }
  CampaignProgress progress = CampaignProgress_LoadCurrent();
  if (lastDumped.has_value() && *lastDumped == progress) {
    Thandor_Log("progress dump (%s): unchanged", when);
    return;
  }
  Thandor_Log("progress dump (%s): %u campaigns", when, (unsigned)progress.campaigns.size());
  DebugCampaign_LogProgress(progress);
  lastDumped = std::move(progress);
}
