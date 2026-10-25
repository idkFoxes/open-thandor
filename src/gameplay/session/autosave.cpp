/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/session/autosave.cpp
 * Project code (not in the original game)
 */

/* The autosave (include/thandor/gameplay/session/autosave.h).

   Why between frames and not at the end of a simulation step: the simulation runs as the UI runtime's
   synchronization hook in the middle of a frame (between the 3D passes); the session loop between two frames is the
   one place where neither a step nor a UI action is running, as for the in-game save button's action, whose path
   (InGameSaveGame_WritePackage) is used unchanged. The tick lock is taken for the save, so no step can slip in.

   What the save writer touches: it converts the runtime pools' pointers to offsets and back with the load path's
   functions, which are not neutral for a running game - they zero free slots and records, run each model class's
   load repair, rebuild the attachment descriptors and drop what no longer resolves (measured: with an autosave every
   2 s the determinism scenario "production" drifted by one Q12 unit in an effect position) - stores the camera as
   the local player's start camera in the level image and hands each player's pending placement asset back to its
   faction (the autosave waits while one is pending). The in-game save button does the same in the original game;
   the autosave must not change the game, so it copies every block the writer modifies before the save and puts
   the copies back afterwards (AutosaveLiveState, about 9 MB of native memory, a few ms): the running game continues
   with exactly the bytes it had. No random number is drawn; the package directory of the mounted save is allocated
   from the arena and freed again, as for any save. */

#include <thandor/gameplay/session/autosave.h>
#include <algorithm>
#include <array>
#include <chrono>
#include <cstring>
#include <new>
#include <vector>
#include <thandor/thandor.h>
#include <thandor/core/settings/original_mode.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>
#include <thandor/ui/ingame/objective_overlay.h>

namespace {

using AutosaveClock = std::chrono::steady_clock;

/* L"autosave": save\autosave.sve */
uint16_t g_AutosaveLeafUtf16[9] = {'a', 'u', 't', 'o', 's', 'a', 'v', 'e', 0};

/* The list text: "Autosave", the separator, and the record's room for it (the header's 0x38-unit save name field). */
constexpr uint16_t AUTOSAVE_LIST_LABEL[] = {u'A', u'u', u't', u'o', u's', u'a', u'v', u'e', 0};
constexpr uint16_t EN_DASH_UNICODE = 0x2013;
constexpr uint16_t EN_DASH_CP1252 = 0x96;
constexpr uint16_t ELLIPSIS_UNICODE = 0x2026;
constexpr uint16_t ELLIPSIS_CP1252 = 0x85;
/* The name column of both save lists is 256 pixels wide (the date column follows); the list text is cut to fit. */
constexpr int32_t AUTOSAVE_LIST_NAME_MAX_WIDTH = 248;
constexpr size_t AUTOSAVE_LIST_TEXT_UNITS =
    offsetof(ScenarioCatalogSaveRecord, levelTitleTextId) / sizeof(uint16_t); /* 0x38, terminator included */

/* The play time of the running session. */
struct AutosaveState {
    bool timing = false;                  /* lastFrame is valid */
    AutosaveClock::time_point lastFrame{};
    /* play time since the session start or the last autosave, in clock ticks: whole milliseconds per frame would
       drop the rest of every frame (all of it above 1000 frames per second, vsync off) */
    AutosaveClock::duration played{0};
    bool loggedWaiting = false;           /* a due save that waits was logged once */
};
AutosaveState s_Autosave;

/* The interval in seconds; 0 when the autosave is off (setting, "Original Thandor" or test aid). The master switch
   overrides the setting without changing it (the original has no autosave); the developer tools' interval
   (OPEN_THANDOR_AUTOSAVE) still wins, as for the setting. */
int Autosave_IntervalSeconds()
{
  const bool enabled = Autosave_Enabled() && OtFeature_Active(OT_FEATURE_AUTOSAVE);
  return DebugHook_AutosaveIntervalSeconds(enabled ? AUTOSAVE_INTERVAL_SECONDS : 0);
}

/* A local game whose simulation runs normally: not a network game (the original offers no save there), not the map
   editor, not paused or waiting, and not over (the local faction ended, an end trigger chose the end movie, the
   session is closing). */
bool Autosave_GameRunning()
{
  constexpr UiCommandRuntimeFlagMask stopped =
      UI_COMMAND_RUNTIME_FLAG_PAUSED | UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE |
      UI_COMMAND_RUNTIME_FLAG_LOCAL_FACTION_ENDED | UI_COMMAND_RUNTIME_FLAG_WAITING_FOR_PLAYERS |
      UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED | UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING |
      UI_COMMAND_RUNTIME_FLAG_RESULTS_CLOSED | UI_COMMAND_RUNTIME_FLAG_WINDOW_PAUSE |
      UI_COMMAND_RUNTIME_FLAG_SESSION_CLOSED | UI_COMMAND_RUNTIME_FLAG_LOCAL_PLAYER_LEFT;

  if (g_InGameRuntimeRoot == nullptr ||
      (g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) != SESSION_NETWORK_ROLE_LOCAL) {
    return false;
  }
  if (Any(g_UiCommandRuntimeFlags & stopped)) {
    return false;
  }
  return !Any((g_InGameRuntimeRoot->worldRuntime).runtimeFlags & INGAME_WORLD_FLAG_EDITOR);
}

/* The running game is at a point where the save can be written and the player loses nothing: no window open (game
   menu, save page, settings, mission help), no movie playing, no army waiting for its placement (the save writer
   would hand it back to the stock) and the tick lock free. */
bool Autosave_AtSafePoint(const char **outReason)
{
  InGameUiImage *image = InGameUi_Image(g_InGameRuntimeRoot);
  uint32_t windowPage;
  FrontendPlayerRuntimeBlockCount playerIndex;

  windowPage = UiPageStack_ActivePageIndex(UiLayoutContainerControl_AsPageStack(&image->gameWindowPageStack));
  if (windowPage >= INGAME_WINDOW_PAGE_GAME_MENU && windowPage <= INGAME_WINDOW_PAGE_MISSION_HELP) {
    *outReason = "a window is open";
    return false;
  }
  if (g_InGameRuntimeRoot->activeEndMovieRuntime != nullptr || g_InGameRuntimeRoot->levelMovieRuntime != nullptr) {
    *outReason = "a movie plays";
    return false;
  }
  if (Any(g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PLACEMENT_PENDING)) {
    *outReason = "a placement is pending";
    return false;
  }
  for (playerIndex = 0; playerIndex < g_FrontendPlayerRuntimeBlockCount; playerIndex++) {
    SelectionPlayerRuntimeBlock *block =
        g_SelectionPlayerRuntimeBlockPointers[g_FrontendPlayerRuntimeBlocks[playerIndex].playerRuntimeId];
    if (block != nullptr && block->pendingPlacementArmyAsset != 0) {
      *outReason = "a placement is pending";
      return false;
    }
  }
  return true;
}

/* The blocks InGameSaveGame_WritePackage modifies (pools, world object records, faction image, level image, light
   records), copied before an autosave and put back after it. */
class AutosaveLiveState {
public:
  /* false when the memory for the copies is missing (the save is then skipped) */
  bool Take()
  {
    InGameLevelConditionStorage *levelStorage = g_InGameLevelRuntimeGlobalBlock.conditionStorage;

    count_ = 0;
    if (!Add(g_ArmyRuntimeSlots,ARMY_RUNTIME_SLOT_COUNT * sizeof(ArmyRuntimeSlot)) ||
        !Add(g_ModelRuntimeSlots,MODEL_RUNTIME_SLOT_COUNT * sizeof(ModelRuntimeSlot)) ||
        !Add(g_ShotRuntimeSlots,SHOT_RUNTIME_POOL_BYTES) ||
        !Add(g_EffectRuntimeSlots,EFFECT_RUNTIME_SLOT_COUNT * sizeof(EffectRuntimeSlot)) ||
        !Add(g_InGameWorldObjectRecords,INGAME_WORLD_OBJECT_RECORD_COUNT * sizeof(WorldObjectRecord)) ||
        !Add(&g_GameFactionRuntimeImage,sizeof(GameFactionRuntimeImage)) ||
        !Add(g_GraphicsShadingRuntimeRecords,sizeof(g_GraphicsShadingRuntimeRecords)) ||
        (levelStorage != nullptr &&
         !Add(levelStorage,
              (levelStorage->levelImage).header.resourceTables.runtimePrefixByteSizeAndInitialArmyPlacementOffset))) {
      count_ = 0;
      return false;
    }
    return true;
  }

  void Restore()
  {
    for (size_t index = 0; index < count_; index++) {
      memcpy(blocks_[index].live,blocks_[index].copy.data(),blocks_[index].copy.size());
    }
    count_ = 0;
  }

private:
  struct Block {
    void *live = nullptr;
    std::vector<uint8_t> copy;
  };

  bool Add(void *live,size_t bytes)
  {
    if (live == nullptr || bytes == 0) {
      return true;
    }
    if (count_ >= blocks_.size()) {
      return false;
    }
    Block &block = blocks_[count_];
    try {
      block.copy.resize(bytes);
    }
    catch (const std::bad_alloc &) {
      return false;
    }
    block.live = live;
    memcpy(block.copy.data(),live,bytes);
    count_++;
    return true;
  }

  std::array<Block, 8> blocks_{};
  size_t count_ = 0;
};
AutosaveLiveState s_LiveState; /* the copies' memory is kept between autosaves */

/* Writes save\autosave.sve with the tick lock held; logs the result and its duration and shows the corner notice. */
void Autosave_Write()
{
  uint16_t savePath[THANDOR_PATH_CAPACITY];
  uint32_t saveError;

  if (g_SpinLockTryAcquire(&g_InGameStateTickSpinLock)) {
    Thandor_Log("autosave: the tick lock is held, the save waits for the next frame");
    return;
  }
  WidePath_CombineDirectoryAndLeaf(savePath,g_SaveDirectoryUtf16,g_ExecutableDirectoryUtf16);
  WidePath_CombineDirectoryAndLeaf(savePath,g_AutosaveLeafUtf16,savePath);
  WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_SVE,savePath,THANDOR_PATH_CAPACITY);
  const AutosaveClock::time_point start = AutosaveClock::now();
  if (!s_LiveState.Take()) {
    g_SpinLockRelease(&g_InGameStateTickSpinLock);
    s_Autosave.played = AutosaveClock::duration(0);
    Thandor_Log("autosave: no memory for the copy of the game state, skipped");
    return;
  }
  saveError = InGameSaveGame_WritePackage(&InGameUi_Image(g_InGameRuntimeRoot)->worldView.base,savePath,true);
  s_LiveState.Restore();
  const long long elapsedMs =
      std::chrono::duration_cast<std::chrono::milliseconds>(AutosaveClock::now() - start).count();
  g_SpinLockRelease(&g_InGameStateTickSpinLock);
  s_Autosave.played = AutosaveClock::duration(0);
  s_Autosave.loggedWaiting = false;
  if (saveError != 0) {
    /* InGameSaveGame_WritePackage logged the failed step; the old autosave is kept */
    Thandor_Log("autosave: failed (error 0x%X) after %lld ms, the game goes on",saveError,elapsedMs);
    return;
  }
  Thandor_Log("autosave: save\\autosave.sve written in %lld ms (step %u)",elapsedMs,
              g_GameFactionRuntimeImage.tail.simulationTick);
  /* a short notice at the bottom left, not a message line: it would push the player's messages out of the history */
  ObjectiveOverlay_ShowNotice(TEXT_ID_AUTOSAVE_DONE);
}

/* Appends text (up to its terminator; a leading command unit, blanks that would follow a blank and everything from a
   later command on are left out, so no command payload is cut) to out at *length, keeping room for the terminator. */
void Autosave_AppendPlainText(uint16_t *out,size_t capacity,size_t *length,const uint16_t *text)
{
  size_t index = 0;

  if (text == nullptr) {
    return;
  }
  if ((text[0] & RICHTEXT_COMMAND_FLAG) != 0) {
    index = 1; /* the title's colour command slot */
  }
  while (*length > 0 && out[*length - 1] == u' ' && text[index] == u' ') {
    index++; /* one blank between the parts (a title may start with one) */
  }
  for (; text[index] != 0 && (text[index] & RICHTEXT_COMMAND_FLAG) == 0 && *length + 1 < capacity; index++) {
    out[(*length)++] = text[index];
  }
  out[*length] = 0;
}

/* " - " with an en dash when the list font has one (U+2013, else Windows-1252 0x96), else a hyphen. */
void Autosave_AppendSeparator(uint16_t *out,size_t capacity,size_t *length)
{
  uint16_t separator[4] = {u' ', u'-', u' ', 0};

  if (FontGlyph_GetLogicalSizeForStyle(g_UiTextStyleNormal,EN_DASH_UNICODE,nullptr) != 0) {
    separator[1] = EN_DASH_UNICODE;
  }
  else if (FontGlyph_GetLogicalSizeForStyle(g_UiTextStyleNormal,EN_DASH_CP1252,nullptr) != 0) {
    separator[1] = EN_DASH_CP1252;
  }
  Autosave_AppendPlainText(out,capacity,length,separator);
}

/* Cuts text (length units) at its end until it is at most AUTOSAVE_LIST_NAME_MAX_WIDTH pixels wide in the list font,
   with an ellipsis (U+2026, else Windows-1252 0x85, else "...") behind the cut text; keepUnits stay in any case. */
void Autosave_FitListText(uint16_t *text,size_t *length,size_t keepUnits)
{
  uint16_t ellipsis[4] = {u'.', u'.', u'.', 0};
  size_t ellipsisLength = 3;

  if (static_cast<int32_t>(RichTextCommandStream_MeasureLine(g_UiTextStyleNormal,text).widthPixels) <=
      AUTOSAVE_LIST_NAME_MAX_WIDTH) {
    return;
  }
  if (FontGlyph_GetLogicalSizeForStyle(g_UiTextStyleNormal,ELLIPSIS_UNICODE,nullptr) != 0) {
    ellipsis[0] = ELLIPSIS_UNICODE;
    ellipsis[1] = 0;
    ellipsisLength = 1;
  }
  else if (FontGlyph_GetLogicalSizeForStyle(g_UiTextStyleNormal,ELLIPSIS_CP1252,nullptr) != 0) {
    ellipsis[0] = ELLIPSIS_CP1252;
    ellipsis[1] = 0;
    ellipsisLength = 1;
  }
  while (*length > keepUnits) {
    (*length)--;
    while (*length > keepUnits && text[*length - 1] == u' ') {
      (*length)--; /* no blank before the ellipsis */
    }
    if (*length + ellipsisLength >= AUTOSAVE_LIST_TEXT_UNITS) {
      continue; /* no room for the ellipsis yet */
    }
    memcpy(text + *length,ellipsis,(ellipsisLength + 1) * sizeof(uint16_t));
    if (static_cast<int32_t>(RichTextCommandStream_MeasureLine(g_UiTextStyleNormal,text).widthPixels) <=
        AUTOSAVE_LIST_NAME_MAX_WIDTH) {
      *length += ellipsisLength;
      return;
    }
    text[*length] = 0;
  }
  text[*length] = 0; /* only the kept units fit */
}

} // namespace

bool Autosave_Enabled()
{
  return PersistentSettings_Read(PERSISTENT_AUTOSAVE_ON,PERSISTENT_SETTING_AUTOSAVE) != PERSISTENT_AUTOSAVE_OFF;
}

void Autosave_SessionStarted()
{
  s_Autosave = AutosaveState{};
}

void Autosave_AfterFrame()
{
  const AutosaveClock::time_point now = AutosaveClock::now();
  const int intervalSeconds = Autosave_IntervalSeconds();
  const char *reason = nullptr;
  bool running;

  if (intervalSeconds <= 0 || g_InGameRuntimeRoot == nullptr) {
    s_Autosave.timing = false;
    return;
  }
  running = Autosave_GameRunning();
  if (s_Autosave.timing && running) {
    s_Autosave.played += std::min<AutosaveClock::duration>(now - s_Autosave.lastFrame,
                                                           std::chrono::milliseconds(AUTOSAVE_MAX_FRAME_GAP_MS));
  }
  s_Autosave.timing = true;
  s_Autosave.lastFrame = now;
  if (!running || s_Autosave.played < std::chrono::seconds(intervalSeconds)) {
    return;
  }
  if (!Autosave_AtSafePoint(&reason)) {
    if (!s_Autosave.loggedWaiting) {
      s_Autosave.loggedWaiting = true;
      Thandor_Log("autosave: due, waits (%s)",reason);
    }
    return;
  }
  Autosave_Write();
}

void Autosave_PrepareCatalogRecord(ScenarioCatalogSaveRecord *record)
{
  /* identifier runs on into metadata40_6F: the header's 0x38-unit save name field */
  uint16_t *listText = reinterpret_cast<uint16_t *>(record);
  uint16_t text[AUTOSAVE_LIST_TEXT_UNITS];
  uint16_t *titleText = nullptr;
  size_t leafLength = 0;
  size_t length = 0;
  size_t labelLength;

  if (record->autosaveMarker != INGAME_SAVE_AUTOSAVE_MARKER) {
    return;
  }
  record->autosaveFileLeaf[0] = 0;
  while (leafLength < AUTOSAVE_LIST_TEXT_UNITS && listText[leafLength] != 0) {
    leafLength++;
  }
  if (leafLength == 0 || leafLength >= sizeof(record->autosaveFileLeaf) / sizeof(uint16_t)) {
    return; /* no room for the file name: listed under its name as any save */
  }
  memcpy(record->autosaveFileLeaf,listText,(leafLength + 1) * sizeof(uint16_t));
  Autosave_AppendPlainText(text,AUTOSAVE_LIST_TEXT_UNITS,&length,AUTOSAVE_LIST_LABEL);
  labelLength = length;
  /* a compact id (page << 8 | index) below 0x10000, as the save page checks it */
  if (static_cast<uint32_t>(record->levelTitleTextId) < 0x10000 &&
      TextResource_TryResolve(static_cast<TextResourceId>(record->levelTitleTextId),&titleText) &&
      titleText != nullptr) {
    Autosave_AppendSeparator(text,AUTOSAVE_LIST_TEXT_UNITS,&length);
    Autosave_AppendPlainText(text,AUTOSAVE_LIST_TEXT_UNITS,&length,titleText);
  }
  /* the date and time of the save stand in the list's date column (the record's timestampText) */
  Autosave_FitListText(text,&length,labelLength);
  memcpy(listText,text,(length + 1) * sizeof(uint16_t));
}

uint16_t *Autosave_SaveRecordFileLeaf(void *record)
{
  ScenarioCatalogSaveRecord *saveRecord = static_cast<ScenarioCatalogSaveRecord *>(record);

  if (saveRecord->autosaveMarker == INGAME_SAVE_AUTOSAVE_MARKER && saveRecord->autosaveFileLeaf[0] != 0) {
    return saveRecord->autosaveFileLeaf;
  }
  return saveRecord->identifier;
}
