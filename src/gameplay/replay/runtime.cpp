/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/replay/runtime.cpp
 * Project code (not in the original game)
 */

#include <thandor/gameplay/replay/runtime.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <chrono>
#include <vector>

#include <thandor/thandor.h>
#include <thandor/core/settings/original_mode.h>
#include <thandor/core/settings/persistent.h>
#include <thandor/gameplay/replay/files.h>
#include <thandor/gameplay/replay/state_hash.h>
#include <thandor/gameplay/replay/stream.h>
#include <thandor/gameplay/session/new_session.h>
#include <thandor/gameplay/session/savegame.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/world/pathing/mode.h>

/* Not in the original: the game recorder (gameplay/replay/runtime.h). How it works:

   Start. A recorded session is started as the player chose (a new level, a loaded save, a campaign level), then
   saved through the ordinary save writer into a temporary checkpoint, torn down and loaded again from it with both
   random streams seeded with one recorded seed and the simulation on the secondary stream (as a network game does;
   the primary stream is also drawn by the UI per frame). Playback loads the same checkpoint the same way, so both
   start from one loaded state, whatever ran during the level's own initialisation. No simulation step runs while
   the checkpoint loads (g_GameRecorderLoading).

   Events. Every local in-game command (InGameCommand_Issue: in a single player game the command handlers are called
   at once from the UI, between two steps), every manual save and every simulation step is one event, stored with
   the context it ran in (stream.h: ticks, game speed, UI flags, both random seeds, local player, active faction,
   camera) and the state hash (state_hash.h) before and after it. Commands issued from inside a step or from inside
   another command belong to that event and are not recorded on their own. The playback blocks the live commands
   (leaving the game still works), restores the recorded context before every event, executes the recorded commands
   through the command table (CommandDispatch_ExecuteRecord, as received network commands) and stops at the first
   hash that differs ("replay: STOP at event N"); it then pauses the game and hands it to the player.

   Files. The recording is written as it goes (stream.h); a crash or a killed game leaves a file that plays up to its
   last complete step. While a recording is open its path is named in crash.log (Thandor_SetCrashLogNote).

   Limits. Single player only (a network session is never recorded); one recording per session: loading another save
   or the next campaign level ends it cleanly and starts the next one. While recording or playing, the transient
   selection and target preview markers of the world view are not drawn (preview_markers.cpp): they take effect pool
   slots from the render path, which the simulation shares, so they made the result depend on the frames drawn
   (the determinism test hides them for the same reason). The hashes cover the main simulation state, not all of
   it: a change that a hash does not see is found one or more events later, or not at all.

   The idea of the checkpoint start, the command guard and the per-step context follows the compact replay of an
   open-thandor fork based on 1.0.7 (docs/COMPACT_REPLAY.md there); the file format, the shared state hash, the
   build id, the save events, the crash recordings and the player option are this version's. */

extern const char g_ThandorBuildId[];     /* generated/build_id.cpp (cmake/build_id.cmake) */
extern const char g_ThandorBuildConfig[];

bool g_GameRecorderActive = false;
bool g_GameRecorderLoading = false;

namespace {

using namespace thandor::replay;

constexpr unsigned KEEP_RECORDINGS = 10;                     /* the option keeps the newest 10 recordings */
constexpr unsigned long long KEEP_RECORDING_BYTES = 200ull << 20; /* and at most 200 MB of them */
constexpr unsigned MAX_EVENTS_BEFORE_STEP = 65536;
constexpr std::uint32_t SESSION_FLAG_NEW_LEVEL = 1; /* the session started a new level: its intro notifications */

/* exit codes of OPEN_THANDOR_REPLAY_QUIT=1 */
constexpr unsigned REPLAY_EXIT_COMPLETE = 0;
constexpr unsigned REPLAY_EXIT_NO_CLEAN_END = 2;
constexpr unsigned REPLAY_EXIT_FAILED = 3;

/* the UI flags the playback keeps from the live game when it restores a recorded context: a session that is closing
   stays closing */
constexpr UiCommandRuntimeFlagMask LIVE_SESSION_END_FLAGS = UI_COMMAND_RUNTIME_FLAG_SESSION_CLOSED |
                                                            UI_COMMAND_RUNTIME_FLAG_LOCAL_PLAYER_LEFT |
                                                            UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING;

enum class Mode { Off, Record, Play };

struct RecorderState {
  Mode mode = Mode::Off;
  bool running = false;       /* the session started: events are recorded / played */
  bool inStep = false;        /* between BeforeStep and AfterStep */
  unsigned commandDepth = 0;  /* nesting of the command scopes that passed */
  bool sessionWasNew = false; /* recording: the session started a new level */
  std::wstring outputPath;    /* recording: the file; empty = the option's recordings\ name, chosen at the start */
  bool optionRecording = false; /* recording by the option (rotation in recordings\) */
  std::wstring playPath;
  std::wstring tempDirectory;
  std::wstring checkpointPath; /* NUL-terminated copy for the loaders in checkpointUtf16 */
  std::vector<uint16_t> checkpointUtf16;
  std::uint32_t loadSeed = 0;
  Ptr32<uint32_t()> previousRandom{};
  bool randomSaved = false;
  Header header;
  std::vector<std::uint8_t> checkpoint;
  Writer writer;
  Reader reader;
  Event pendingStep;
  Event pendingCommand;
  std::uint64_t lastHash = 0;
  std::uint64_t eventNumber = 0; /* events recorded / played so far */
  bool sessionClosedByReplay = false;
  std::uint64_t kindCounts[5] = {}; /* events by RecordingEventKind */
  std::uint64_t recorderNanoseconds = 0; /* recording: time spent in the recorder (hashes, writing) */
};

RecorderState s_State;

/* Recording: adds the time of its scope to recorderNanoseconds (the overhead line at the end of a recording). */
class RecorderTimer {
public:
  RecorderTimer() : start_(std::chrono::steady_clock::now()) {}
  ~RecorderTimer()
  {
    s_State.recorderNanoseconds += static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - start_).count());
  }
  RecorderTimer(const RecorderTimer &) = delete;
  RecorderTimer &operator=(const RecorderTimer &) = delete;

private:
  std::chrono::steady_clock::time_point start_;
};

void CountEvent(RecordingEventKind kind)
{
  s_State.eventNumber++;
  s_State.kindCounts[static_cast<unsigned>(kind) % 5]++;
}

/* "1234 steps, 56 commands, 1 saves" */
std::string EventCountsText()
{
  char text[128];
  snprintf(text, sizeof text, "%llu steps, %llu commands, %llu saves",
           static_cast<unsigned long long>(s_State.kindCounts[static_cast<unsigned>(RecordingEventKind::Step)]),
           static_cast<unsigned long long>(s_State.kindCounts[static_cast<unsigned>(RecordingEventKind::Command)]),
           static_cast<unsigned long long>(s_State.kindCounts[static_cast<unsigned>(RecordingEventKind::Save)]));
  return text;
}

bool s_ReplayConsumed = false;   /* the playback replaces one session per process */
unsigned s_RecordSessionCount = 0; /* OPEN_THANDOR_RECORD: sessions recorded so far (later ones get -2, -3, ...) */

/* ---- context and state ---- */

static_assert(sizeof(WorldMotionState) == 14 * sizeof(std::uint32_t), "the camera is 14 context words");

bool SecondaryStreamSelected()
{
  return g_RandomGeneratorState.next == THANDOR_SLOT(Random_NextSecondary);
}

Context CaptureContext()
{
  Context context{};
  const GameFactionRuntimeImageTail &tail = g_GameFactionRuntimeImage.tail;
  context[0] = tail.simulationTick;
  context[1] = g_SessionNetworkTickCounter;
  context[2] = static_cast<std::uint32_t>(g_InGameSimulationStepTicks);
  context[3] = tail.presentationTick;
  context[4] = tail.gameSpeedQ8;
  context[5] = ToBits(g_UiCommandRuntimeFlags);
  context[6] = g_RandomGeneratorState.primarySeed;
  context[7] = g_RandomGeneratorState.secondarySeed;
  context[8] = SecondaryStreamSelected() ? 1u : 0u;
  context[9] = g_LocalPlayerRuntimeId;
  context[10] = static_cast<std::uint32_t>(g_InGameRuntimeRoot->worldRuntime.activeFactionRuntimeIndex);
  /* fourteen Q12 / angle / distance scalars, no pointers */
  std::memcpy(context.data() + 11, &g_InGameRuntimeRoot->worldRuntime.motion, sizeof(WorldMotionState));
  return context;
}

std::uint64_t StateHash()
{
  std::uint64_t hash = GameStateHash_Compute(nullptr, nullptr);
  hash = GameStateHash_AddWord(hash, g_SessionNetworkTickCounter);
  hash = GameStateHash_AddWord(hash, static_cast<std::uint32_t>(g_InGameSimulationStepTicks));
  return hash;
}

std::string HashText(std::uint64_t hash)
{
  char text[24];
  snprintf(text, sizeof text, "%08x%08x", static_cast<unsigned>(hash >> 32), static_cast<unsigned>(hash));
  return text;
}

void SetCrashNote()
{
  if (s_State.mode == Mode::Record && s_State.writer.IsOpen()) {
    const std::string note = "recording: " + Narrow(s_State.outputPath) +
                             " (a recording of this game up to the crash: please attach it to the bug report)";
    Thandor_SetCrashLogNote(note.c_str());
  }
  else if (s_State.mode == Mode::Play) {
    const std::string note = "replay: playing " + Narrow(s_State.playPath);
    Thandor_SetCrashLogNote(note.c_str());
  }
  else {
    Thandor_SetCrashLogNote(nullptr);
  }
}

void RemoveTemporaryFiles()
{
  RemoveTemporaryDirectory(s_State.tempDirectory);
  s_State.tempDirectory.clear();
}

bool CreateCheckpointDirectory()
{
  s_State.tempDirectory = CreateTemporaryDirectory();
  if (s_State.tempDirectory.empty()) {
    return false;
  }
  s_State.checkpointPath = s_State.tempDirectory + L"checkpoint.sve";
  s_State.checkpointUtf16.assign(s_State.checkpointPath.begin(), s_State.checkpointPath.end());
  s_State.checkpointUtf16.push_back(0);
  return s_State.checkpointUtf16.size() < THANDOR_PATH_CAPACITY;
}

/* Recording: the state hash before an event. It equals the previous event's after hash unless something outside
   the recorded events changed the state in between, which the playback cannot repeat: logged (the first few), so a
   recording that will diverge says where already while it is made. */
std::uint64_t BeforeHash()
{
  static unsigned s_logged = 0;
  const std::uint64_t hash = StateHash();
  if (hash != s_State.lastHash && s_logged < 5) {
    s_logged++;
    Thandor_Log("recorder: the state changed outside the recorded events before event %llu (%s -> %s)",
                static_cast<unsigned long long>(s_State.eventNumber + 1), HashText(s_State.lastHash).c_str(),
                HashText(hash).c_str());
  }
  return hash;
}

/* The level's file name for the recording's name: its last path component without extension, ASCII letters, digits,
   '-' and '_' (others become '_'); "level" when there is none. */
std::string LevelName()
{
  std::string name;
  const InGameLevelConditionStorage *storage = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
  if (storage == nullptr) {
    return "level";
  }
  const uint16_t *units = storage->levelImage.header.levelFileNameUtf16;
  for (std::size_t index = 0; index < std::size(storage->levelImage.header.levelFileNameUtf16) && units[index] != 0;
       index++) {
    const uint16_t unit = units[index];
    if (unit == '\\' || unit == '/') {
      name.clear(); /* a custom map's level is maps\<name>\<name> */
      continue;
    }
    if (unit == '.') {
      break;
    }
    name.push_back((unit < 0x80 && (isalnum(unit) || unit == '-' || unit == '_')) ? static_cast<char>(unit) : '_');
  }
  return name.empty() ? "level" : name;
}

/* ---- ending ---- */

void ResetSessionFlags()
{
  s_State.running = false;
  s_State.inStep = false;
  s_State.commandDepth = 0;
  g_GameRecorderActive = false;
  g_GameRecorderLoading = false;
}

/* Recording: closes the file with its End event. */
void FinishRecording(RecordingEndReason reason, const char *why)
{
  if (s_State.mode != Mode::Record) {
    return;
  }
  std::string error;
  if (s_State.writer.IsOpen()) {
    if (s_State.writer.Finish(s_State.lastHash, reason, error)) {
      const std::uint64_t steps = s_State.kindCounts[static_cast<unsigned>(RecordingEventKind::Step)];
      Thandor_Log("recorder: %s: %s, %llu events (%s), %llu bytes; recorder time %.1f ms, %.1f us per step",
                  why, Narrow(s_State.outputPath).c_str(), static_cast<unsigned long long>(s_State.writer.EventCount()),
                  EventCountsText().c_str(), static_cast<unsigned long long>(s_State.writer.BytesWritten()),
                  s_State.recorderNanoseconds / 1e6, steps != 0 ? s_State.recorderNanoseconds / 1e3 / steps : 0.0);
    }
    else {
      Thandor_Log("recorder: closing %s failed: %s", Narrow(s_State.outputPath).c_str(), error.c_str());
    }
  }
  s_State.mode = Mode::Off;
  ResetSessionFlags();
  SetCrashNote();
}

/* Playback: ends the playback; the game goes on live and paused (or the process ends, OPEN_THANDOR_REPLAY_QUIT). */
void FinishPlayback(unsigned exitCode, const std::string &message)
{
  if (s_State.mode != Mode::Play) {
    return;
  }
  Thandor_Log("replay: %s", message.c_str());
  const bool wasRunning = s_State.running;
  s_State.mode = Mode::Off;
  ResetSessionFlags();
  SetCrashNote();
  if (EnvironmentFlag(L"OPEN_THANDOR_REPLAY_QUIT")) {
    Thandor_Log("replay: OPEN_THANDOR_REPLAY_QUIT, exit code %u", exitCode);
    ExitProcessWith(exitCode);
  }
  if (wasRunning && g_InGameRuntimeRoot != nullptr && !Any(g_UiCommandRuntimeFlags & LIVE_SESSION_END_FLAGS)) {
    g_UiCommandRuntimeFlags |= UI_COMMAND_RUNTIME_FLAG_PAUSED;
    Thandor_Log("replay: the game is paused and yours from here");
  }
}

void PlaybackDiverged(const std::string &what)
{
  char prefix[64];
  snprintf(prefix, sizeof prefix, "STOP at event %llu: ", static_cast<unsigned long long>(s_State.eventNumber));
  FinishPlayback(REPLAY_EXIT_FAILED, prefix + what);
}

void RecorderFailed(const std::string &what)
{
  Thandor_Log("recorder: stopped at event %llu: %s", static_cast<unsigned long long>(s_State.eventNumber),
              what.c_str());
  FinishRecording(RecordingEndReason::RecorderError, "recording ended early");
}

/* ---- playback ---- */

bool ApplyContext(const Context &context)
{
  GameFactionRuntimeImageTail &tail = g_GameFactionRuntimeImage.tail;
  if (context[0] != tail.simulationTick || context[1] != g_SessionNetworkTickCounter) {
    char text[160];
    snprintf(text, sizeof text, "the simulation is at tick %u / step %u, the recording at %u / %u",
             tail.simulationTick, g_SessionNetworkTickCounter, context[0], context[1]);
    PlaybackDiverged(text);
    return false;
  }
  if (context[2] < 1 || context[2] > INGAME_SIMULATION_STEP_TICKS_MAX || context[8] > 1 || context[9] >= 8 ||
      context[10] >= 8) {
    PlaybackDiverged("the recorded context is invalid");
    return false;
  }
  g_InGameSimulationStepTicks = static_cast<InGameSimulationStepBatchTicks>(context[2]);
  tail.presentationTick = context[3];
  tail.gameSpeedQ8 = context[4];
  g_UiCommandRuntimeFlags = (FromBits<UiCommandRuntimeFlagMask>(context[5]) & ~LIVE_SESSION_END_FLAGS) |
                            (g_UiCommandRuntimeFlags & LIVE_SESSION_END_FLAGS);
  g_RandomGeneratorState.primarySeed = context[6];
  g_RandomGeneratorState.secondarySeed = context[7];
  if (context[8] != 0) {
    Random_SelectSecondaryStream();
  }
  else {
    Random_SelectPrimaryStream();
  }
  g_LocalPlayerRuntimeId = context[9];
  g_InGameRuntimeRoot->worldRuntime.activeFactionRuntimeIndex = static_cast<FactionRuntimeIndex>(context[10]);
  std::memcpy(&g_InGameRuntimeRoot->worldRuntime.motion, context.data() + 11, sizeof(WorldMotionState));
  return true;
}

bool HashMatches(std::uint64_t expected, const char *where)
{
  const std::uint64_t actual = StateHash();
  if (actual != expected) {
    PlaybackDiverged(std::string("the state differs ") + where + " (recorded " + HashText(expected) + ", now " +
                     HashText(actual) + ")");
    return false;
  }
  s_State.lastHash = actual;
  return true;
}

const char *EventKindName(RecordingEventKind kind)
{
  switch (kind) {
  case RecordingEventKind::Command: return "command";
  case RecordingEventKind::Step: return "step";
  case RecordingEventKind::Save: return "save";
  default: return "end";
  }
}

void PlaybackReachedEnd(const Event &end)
{
  if (end.afterHash != s_State.lastHash) {
    PlaybackDiverged("the final state hash differs (recorded " + HashText(end.afterHash) + ", now " +
                     HashText(s_State.lastHash) + ")");
    return;
  }
  char text[200];
  snprintf(text, sizeof text, "COMPLETE: all %llu events matched (%s), final state hash %s%s",
           static_cast<unsigned long long>(s_State.eventNumber), EventCountsText().c_str(), HashText(end.afterHash).c_str(),
           end.endReason == RecordingEndReason::RecorderError ? " (the recorder had stopped early)" : "");
  FinishPlayback(REPLAY_EXIT_COMPLETE, text);
}

/* Executes recorded events up to the next step; true with s_State.pendingStep set when a step is due. */
bool PlayEventsUntilStep()
{
  for (unsigned count = 0; count < MAX_EVENTS_BEFORE_STEP; count++) {
    Event event;
    std::string error;
    switch (s_State.reader.Next(event, error)) {
    case Reader::NextResult::Event:
      break;
    case Reader::NextResult::End:
      PlaybackReachedEnd(event);
      return false;
    case Reader::NextResult::Truncated:
      FinishPlayback(REPLAY_EXIT_NO_CLEAN_END, error + ": every event up to there matched (" + EventCountsText() + "), final state hash " +
                                                   HashText(s_State.lastHash));
      return false;
    case Reader::NextResult::Damaged:
      PlaybackDiverged(error);
      return false;
    }
    CountEvent(event.kind);
    if (!ApplyContext(event.context) || !HashMatches(event.beforeHash, std::string(std::string("before ") +
                                                                                   EventKindName(event.kind)).c_str())) {
      return false;
    }
    if (event.kind == RecordingEventKind::Step) {
      s_State.pendingStep = event;
      return true;
    }
    s_State.commandDepth++;
    if (event.kind == RecordingEventKind::Command) {
      UiCommandQueueRecord record{};
      record.packedCommandAndPlayerId = event.command[0];
      record.payload1 = event.command[1];
      record.payload2 = event.command[2];
      record.payload3 = event.command[3];
      CommandDispatch_ExecuteRecord(INGAME_COMMAND_CODE_BASE, INGAME_COMMAND_HANDLER_REGION_END, &record);
    }
    else {
      /* a manual save: the same save into a temporary file, for the state it changes */
      std::wstring path = s_State.tempDirectory + L"replayed_save.sve";
      std::vector<uint16_t> pathUtf16(path.begin(), path.end());
      pathUtf16.push_back(0);
      const uint32_t saveError =
          InGameSaveGame_WritePackage(&InGameUi_Image(g_InGameRuntimeRoot)->worldView.base, pathUtf16.data());
      if (saveError != 0) {
        Thandor_Log("replay: the replayed save failed (error 0x%X)", saveError);
      }
    }
    s_State.commandDepth--;
    if (!HashMatches(event.afterHash, std::string(std::string("after ") + EventKindName(event.kind)).c_str())) {
      return false;
    }
    if (Any(g_UiCommandRuntimeFlags & LIVE_SESSION_END_FLAGS)) {
      s_State.sessionClosedByReplay = true;
    }
  }
  PlaybackDiverged("too many events without a simulation step");
  return false;
}

/* ---- starting ---- */

bool ConflictingTestAid(const char **name)
{
  static const wchar_t *const aids[] = {L"OPEN_THANDOR_ARENA_ORDERS", L"OPEN_THANDOR_AUTOWIN", L"OPEN_THANDOR_AUTOLOSE",
                                        L"OPEN_THANDOR_STATEHASH_SPEED", L"OPEN_THANDOR_STATEHASH_PAUSE_AT"};
  static const char *const names[] = {"OPEN_THANDOR_ARENA_ORDERS", "OPEN_THANDOR_AUTOWIN", "OPEN_THANDOR_AUTOLOSE",
                                      "OPEN_THANDOR_STATEHASH_SPEED", "OPEN_THANDOR_STATEHASH_PAUSE_AT"};
  for (std::size_t index = 0; index < std::size(aids); index++) {
    if (!EnvironmentValue(aids[index]).empty()) {
      *name = names[index];
      return true;
    }
  }
  return false;
}

bool PreparePlayback(const std::wstring &path)
{
  std::string error;
  s_State.playPath = AbsolutePath(path);
  if (!s_State.reader.Open(s_State.playPath, s_State.header, s_State.checkpoint, error)) {
    Thandor_Log("replay: cannot play %s: %s", Narrow(s_State.playPath).c_str(), error.c_str());
    return false;
  }
  const Header &header = s_State.header;
  Thandor_Log("replay: %s: level %s, recorded %s by build \"%s\" (%s)", Narrow(s_State.playPath).c_str(),
              header.levelName.c_str(), header.startedAt.c_str(), header.buildId.c_str(), header.buildConfig.c_str());
  const bool force = EnvironmentFlag(L"OPEN_THANDOR_REPLAY_FORCE");
  if (header.buildId != g_ThandorBuildId) {
    Thandor_Log("replay: the recording was made by build \"%s\", this is \"%s\"%s", header.buildId.c_str(),
                g_ThandorBuildId, force ? "; played anyway (OPEN_THANDOR_REPLAY_FORCE)" :
                                          ": not played (build the same commit, or OPEN_THANDOR_REPLAY_FORCE=1)");
    if (!force) {
      return false;
    }
  }
  if (header.buildConfig != g_ThandorBuildConfig) {
    Thandor_Log("replay: note: recorded with \"%s\", this build is \"%s\"", header.buildConfig.c_str(),
                g_ThandorBuildConfig);
  }
  const AssetFingerprint assets = ComputeAssetFingerprint({});
  if (header.assetsHash != assets.hash) {
    Thandor_Log("replay: the game files differ from the recording's (%u files, fingerprint %s; here %u files, %s)%s",
                header.assetFileCount, HashText(header.assetsHash).c_str(), assets.fileCount,
                HashText(assets.hash).c_str(),
                force ? "; played anyway (OPEN_THANDOR_REPLAY_FORCE)" : ": not played");
    if (!force) {
      return false;
    }
  }
  if (!CreateCheckpointDirectory() || !WriteFileBytes(s_State.checkpointPath, s_State.checkpoint)) {
    Thandor_Log("replay: cannot write the checkpoint to %s", Narrow(s_State.checkpointPath).c_str());
    return false;
  }
  s_State.checkpoint.clear();
  s_State.checkpoint.shrink_to_fit();
  s_State.loadSeed = header.loadSeed;
  return true;
}

bool PrepareRecording(const std::wstring &environmentPath)
{
  if (!environmentPath.empty()) {
    /* OPEN_THANDOR_RECORD=<path>: the first session there, later ones (or an existing file) as <stem>-2.otr, ... */
    std::wstring path = AbsolutePath(environmentPath);
    const std::size_t dot = path.find_last_of(L'.');
    const std::size_t slash = path.find_last_of(L"\\/");
    const std::wstring stem = dot != std::wstring::npos && (slash == std::wstring::npos || dot > slash) ?
                                  path.substr(0, dot) : path;
    const std::wstring extension = stem.size() < path.size() ? path.substr(stem.size()) : std::wstring(L".otr");
    unsigned number = s_RecordSessionCount + 1;
    while (number > 1 || FileExists(path)) {
      if (number < 2) {
        number = 2;
      }
      path = stem + L"-" + std::to_wstring(number) + extension;
      if (!FileExists(path)) {
        break;
      }
      number++;
    }
    s_State.outputPath = path;
    s_State.optionRecording = false;
  }
  else {
    s_State.outputPath.clear(); /* chosen at the start, when the level name is known */
    s_State.optionRecording = true;
  }
  if (!CreateCheckpointDirectory()) {
    Thandor_Log("recorder: cannot create a temporary directory, this game is not recorded");
    return false;
  }
  /* the seed both streams get for the checkpoint load: the session's current one (any seed works) */
  s_State.loadSeed = g_RandomGeneratorState.primarySeed ^ 0x5EC0FD00u;
  return true;
}

} // namespace

/* ---- session lifecycle ---- */

void GameRecorder_PrepareSession(bool loadExistingSession,const uint16_t *levelPathUtf16)
{
  (void)levelPathUtf16;
  s_State.mode = Mode::Off;
  ResetSessionFlags();
  s_State.sessionWasNew = !loadExistingSession;
  s_State.sessionClosedByReplay = false;
  std::fill(std::begin(s_State.kindCounts), std::end(s_State.kindCounts), 0);
  s_State.recorderNanoseconds = 0;
  s_State.eventNumber = 0;
  s_State.lastHash = 0;
  s_State.header = Header{};
  s_State.checkpoint.clear();

  std::wstring replayPath;
  if (!s_ReplayConsumed) {
    replayPath = EnvironmentValue(L"OPEN_THANDOR_REPLAY");
    if (replayPath.empty()) {
      replayPath = CommandLineReplayPath();
    }
  }
  const std::wstring recordValue = EnvironmentValue(L"OPEN_THANDOR_RECORD");
  const bool recordOff = recordValue == L"0" || recordValue == L"off" || recordValue == L"OFF";
  const bool recordOn = recordValue == L"1" || recordValue == L"on" || recordValue == L"ON";
  const bool recordPath = !recordValue.empty() && !recordOff && !recordOn;
  const bool option = PersistentSettings_Read(PERSISTENT_RECORD_GAMES_OFF, PERSISTENT_SETTING_RECORD_GAMES) ==
                          PERSISTENT_RECORD_GAMES_ON && OtFeature_Active(OT_FEATURE_GAME_RECORDING);
  const bool wantRecord = !recordOff && (recordPath || recordOn || option);

  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) != SESSION_NETWORK_ROLE_LOCAL) {
    if (wantRecord || !replayPath.empty()) {
      Thandor_Log("recorder: network games are not recorded or played back");
    }
    return;
  }
  const char *aid = nullptr;
  if ((wantRecord || !replayPath.empty()) && ConflictingTestAid(&aid)) {
    Thandor_Log("recorder: %s changes the game outside its commands, this game is not recorded or played back", aid);
    return;
  }
  if (!replayPath.empty()) {
    s_ReplayConsumed = true;
    if (PreparePlayback(replayPath)) {
      s_State.mode = Mode::Play;
      return;
    }
    RemoveTemporaryFiles();
    if (EnvironmentFlag(L"OPEN_THANDOR_REPLAY_QUIT")) {
      Thandor_Log("replay: OPEN_THANDOR_REPLAY_QUIT, exit code %u", REPLAY_EXIT_FAILED);
      ExitProcessWith(REPLAY_EXIT_FAILED);
    }
    Thandor_Log("replay: the session starts as chosen instead");
    return;
  }
  if (!wantRecord) {
    return;
  }
  if (PrepareRecording(recordPath ? recordValue : std::wstring())) {
    s_State.mode = Mode::Record;
  }
}

bool GameRecorder_LoadsCheckpointInstead()
{
  return s_State.mode == Mode::Play;
}

bool GameRecorder_ReloadsFromCheckpoint()
{
  return s_State.mode == Mode::Record;
}

uint16_t *GameRecorder_CheckpointPath()
{
  return s_State.checkpointUtf16.data();
}

bool GameRecorder_CaptureCheckpoint()
{
  if (s_State.mode != Mode::Record) {
    return false;
  }
  const uint32_t error =
      InGameSaveGame_WritePackage(&InGameUi_Image(g_InGameRuntimeRoot)->worldView.base, GameRecorder_CheckpointPath());
  if (error != 0 || !ReadFileBytes(s_State.checkpointPath, s_State.checkpoint)) {
    Thandor_Log("recorder: the checkpoint save failed (error 0x%X), this game is not recorded", error);
    s_State.mode = Mode::Off;
    RemoveTemporaryFiles();
    return false;
  }
  return true;
}

void GameRecorder_BeginCheckpointLoad()
{
  if (!s_State.randomSaved) {
    s_State.previousRandom = g_RandomGeneratorState.next;
    s_State.randomSaved = true;
  }
  g_GameRecorderLoading = true;
  Random_SetBothSeeds(static_cast<RandomSeed>(s_State.loadSeed));
  Random_SelectSecondaryStream();
  if (s_State.mode == Mode::Play) {
    /* the frontend player records as in the recorded game (the AI planning tells human factions by them) */
    const std::size_t count = std::min<std::size_t>(s_State.header.frontendPlayers.size(), MAX_FRONTEND_PLAYERS);
    if (count != 0 && g_FrontendPlayerRuntimeBlocks != nullptr) {
      g_FrontendPlayerRuntimeBlockCount = static_cast<FrontendPlayerRuntimeBlockCount>(count);
      for (std::size_t index = 0; index < count; index++) {
        g_FrontendPlayerRuntimeBlocks[index].playerRuntimeId =
            static_cast<FrontendPlayerRuntimeId>(s_State.header.frontendPlayers[index].playerRuntimeId);
        g_FrontendPlayerRuntimeBlocks[index].factionAssignment.factionAssignmentIndex =
            static_cast<FrontendFactionAssignmentIndex>(s_State.header.frontendPlayers[index].factionAssignmentIndex);
      }
    }
  }
}

void GameRecorder_EndCheckpointLoad()
{
  g_GameRecorderLoading = false;
  /* the loader may have changed the selection; the simulation stays on the seeded secondary stream */
  Random_SelectSecondaryStream();
  if ((s_State.mode == Mode::Record ? s_State.sessionWasNew
                                    : (s_State.header.sessionFlags & SESSION_FLAG_NEW_LEVEL) != 0) &&
      g_InGameRuntimeRoot != nullptr) {
    /* a loaded save queues no intro notifications; a new level started them */
    InGameNewSession_QueueIntroNotificationRecords();
  }
}

void GameRecorder_SessionStarted()
{
  if (s_State.mode == Mode::Off || g_InGameRuntimeRoot == nullptr) {
    return;
  }
  if (s_State.mode == Mode::Play) {
    const Context context = s_State.header.initialContext;
    g_GameRecorderActive = true;
    s_State.running = true;
    SetCrashNote();
    if (!ApplyContext(context) || !HashMatches(s_State.header.initialHash, "after loading the checkpoint")) {
      return;
    }
    Thandor_Log("replay: playing %s (%s)%s", Narrow(s_State.playPath).c_str(), s_State.header.levelName.c_str(),
                GameRecorder_StepsUnpaced() ? ", steps unpaced (OPEN_THANDOR_REPLAY_FAST)" : "");
    return;
  }
  /* recording */
  Header &header = s_State.header;
  header.buildId = g_ThandorBuildId;
  header.buildConfig = g_ThandorBuildConfig;
  header.levelName = LevelName();
  header.startedAt = LocalTimeText(false);
  /* the packages only: the level itself (also a custom map) is part of the checkpoint save */
  const AssetFingerprint assets = ComputeAssetFingerprint({});
  header.assetsHash = assets.hash;
  header.assetFileCount = assets.fileCount;
  header.loadSeed = s_State.loadSeed;
  header.pathingMode = static_cast<std::uint32_t>(g_PathingMode);
  header.sessionFlags = s_State.sessionWasNew ? SESSION_FLAG_NEW_LEVEL : 0;
  for (std::uint32_t index = 0; index < g_FrontendPlayerRuntimeBlockCount && index < MAX_FRONTEND_PLAYERS; index++) {
    header.frontendPlayers.push_back(
        {static_cast<std::uint32_t>(g_FrontendPlayerRuntimeBlocks[index].playerRuntimeId),
         static_cast<std::uint32_t>(g_FrontendPlayerRuntimeBlocks[index].factionAssignment.factionAssignmentIndex)});
  }
  header.initialContext = CaptureContext();
  header.initialHash = StateHash();
  if (s_State.optionRecording) {
    const std::wstring directory = ExecutableDirectory() + L"recordings\\";
    CreateDirectoryIfMissing(directory);
    RotateRecordings(directory, KEEP_RECORDINGS, 1, KEEP_RECORDING_BYTES);
    std::wstring name = Widen(LocalTimeText(true) + "_" + header.levelName);
    s_State.outputPath = directory + name + L".otr";
    for (unsigned number = 2; FileExists(s_State.outputPath); number++) {
      s_State.outputPath = directory + name + L"-" + std::to_wstring(number) + L".otr";
    }
  }
  std::string error;
  if (!s_State.writer.Open(s_State.outputPath, header, s_State.checkpoint, error)) {
    Thandor_Log("recorder: cannot write %s: %s; this game is not recorded", Narrow(s_State.outputPath).c_str(),
                error.c_str());
    s_State.mode = Mode::Off;
    s_State.checkpoint.clear();
    return;
  }
  s_RecordSessionCount++;
  s_State.lastHash = header.initialHash;
  Thandor_Log("recorder: recording %s (level %s, checkpoint %zu bytes, %u asset files, build \"%s\")",
              Narrow(s_State.outputPath).c_str(), header.levelName.c_str(), s_State.checkpoint.size(),
              assets.fileCount, g_ThandorBuildId);
  s_State.checkpoint.clear();
  s_State.checkpoint.shrink_to_fit();
  s_State.running = true;
  g_GameRecorderActive = true;
  SetCrashNote();
}

void GameRecorder_SessionEnding()
{
  if (!s_State.running) {
    return;
  }
  if (s_State.mode == Mode::Record) {
    FinishRecording(RecordingEndReason::SessionEnded, "recording finished");
    if (s_State.optionRecording) {
      RotateRecordings(ExecutableDirectory() + L"recordings\\", KEEP_RECORDINGS, 0, KEEP_RECORDING_BYTES);
    }
    return;
  }
  if (s_State.mode != Mode::Play) {
    return;
  }
  /* the session ends: the recording should end here as well (steps the recorded game ran after a command closed
     the session are passed over) */
  unsigned trailingSteps = 0;
  for (;;) {
    Event event;
    std::string error;
    const Reader::NextResult result = s_State.reader.Next(event, error);
    if (result == Reader::NextResult::Event && event.kind == RecordingEventKind::Step &&
        s_State.sessionClosedByReplay) {
      trailingSteps++;
      CountEvent(RecordingEventKind::Step);
      s_State.lastHash = event.afterHash;
      continue;
    }
    if (result == Reader::NextResult::End) {
      if (trailingSteps != 0) {
        Thandor_Log("replay: %u steps the recorded game ran after it was left are not played", trailingSteps);
        s_State.lastHash = event.afterHash; /* the steps are not played, their hashes cannot be checked */
      }
      PlaybackReachedEnd(event);
    }
    else if (result == Reader::NextResult::Truncated) {
      FinishPlayback(REPLAY_EXIT_NO_CLEAN_END, error + "; the session ended there");
    }
    else if (result == Reader::NextResult::Damaged) {
      PlaybackDiverged(error);
    }
    else {
      char text[160];
      snprintf(text, sizeof text, "the session ended at event %llu, before the end of the recording (left by the "
               "player?)", static_cast<unsigned long long>(s_State.eventNumber));
      FinishPlayback(REPLAY_EXIT_FAILED, text);
    }
    return;
  }
}

void GameRecorder_SessionClosed()
{
  if (s_State.mode == Mode::Record) {
    if (s_State.running) {
      FinishRecording(RecordingEndReason::SessionEnded, "recording finished");
    }
    s_State.writer.Abandon();
  }
  else if (s_State.mode == Mode::Play && s_State.running) {
    FinishPlayback(REPLAY_EXIT_FAILED, "the session ended before the end of the recording");
  }
  s_State.mode = Mode::Off;
  ResetSessionFlags();
  s_State.reader = Reader{};
  s_State.checkpoint.clear();
  RemoveTemporaryFiles();
  if (s_State.randomSaved) {
    g_RandomGeneratorState.next = s_State.previousRandom;
    s_State.randomSaved = false;
  }
  SetCrashNote();
}

/* ---- steps ---- */

bool GameRecorder_BeforeStep()
{
  if (!s_State.running) {
    return true;
  }
  if (s_State.commandDepth != 0 || s_State.inStep) {
    return false; /* a command handler draws frames: the step waits until it returned */
  }
  if (s_State.mode == Mode::Record) {
    RecorderTimer timer;
    s_State.pendingStep = Event{};
    s_State.pendingStep.kind = RecordingEventKind::Step;
    s_State.pendingStep.context = CaptureContext();
    s_State.pendingStep.beforeHash = BeforeHash();
    s_State.inStep = true;
    return true;
  }
  if (!PlayEventsUntilStep()) {
    return false; /* the playback ended (or diverged): this step is skipped, the next ones run live */
  }
  s_State.inStep = true;
  return true;
}

void GameRecorder_AfterStep()
{
  if (!s_State.running || !s_State.inStep) {
    return;
  }
  s_State.inStep = false;
  if (s_State.mode == Mode::Record) {
    RecorderTimer timer;
    std::string error;
    s_State.pendingStep.afterHash = StateHash();
    if (!s_State.writer.Append(s_State.pendingStep, error)) {
      RecorderFailed(error);
      return;
    }
    s_State.lastHash = s_State.pendingStep.afterHash;
    CountEvent(RecordingEventKind::Step);
  }
  else if (s_State.mode == Mode::Play) {
    HashMatches(s_State.pendingStep.afterHash, "after step");
  }
}

bool GameRecorder_StepsUnpaced()
{
  static const bool s_fast = EnvironmentFlag(L"OPEN_THANDOR_REPLAY_FAST");
  return s_State.mode == Mode::Play && s_State.running && s_fast;
}

/* ---- commands ---- */

GameRecorderCommandScope::GameRecorderCommandScope(uint32_t code,uint32_t player,uint32_t payload1,uint32_t payload2,
                                                   uint32_t payload3)
{
  if (!s_State.running || s_State.inStep) {
    return; /* not recorded, or part of the step that issues it */
  }
  if (s_State.commandDepth != 0) {
    s_State.commandDepth++; /* part of the command that issues it */
    recorded_ = true;
    return;
  }
  if (s_State.mode == Mode::Play) {
    if (code == INGAME_COMMAND_PLAYER_DEPARTURE && (payload3 & INGAME_PLAYER_DEPARTURE_FLAG_CLOSE_SESSION) != 0) {
      Thandor_Log("replay: left by the player at event %llu", static_cast<unsigned long long>(s_State.eventNumber));
      FinishPlayback(REPLAY_EXIT_FAILED, "cancelled by the player");
      return; /* leaving the game is executed */
    }
    execute_ = false; /* live input does not reach a played back game */
    return;
  }
  if (code > 0xFFFFFFu || player > 0xFFu) {
    char text[96];
    snprintf(text, sizeof text, "a command without a command table entry (code 0x%X) cannot be recorded", code);
    RecorderFailed(text);
    return;
  }
  RecorderTimer timer;
  s_State.pendingCommand = Event{};
  s_State.pendingCommand.kind = RecordingEventKind::Command;
  s_State.pendingCommand.command = {(code << 8) | (player & 0xFFu), payload1, payload2, payload3};
  s_State.pendingCommand.context = CaptureContext();
  s_State.pendingCommand.beforeHash = BeforeHash();
  s_State.commandDepth = 1;
  recorded_ = true;
}

GameRecorderCommandScope::~GameRecorderCommandScope()
{
  if (!recorded_ || s_State.commandDepth == 0) {
    return;
  }
  if (--s_State.commandDepth != 0 || s_State.mode != Mode::Record || !s_State.running) {
    return;
  }
  RecorderTimer timer;
  std::string error;
  s_State.pendingCommand.afterHash = StateHash();
  if (!s_State.writer.Append(s_State.pendingCommand, error)) {
    RecorderFailed(error);
    return;
  }
  s_State.lastHash = s_State.pendingCommand.afterHash;
  CountEvent(RecordingEventKind::Command);
}

/* ---- saves ---- */

GameRecorderSaveScope::GameRecorderSaveScope(bool autosave)
{
  if (autosave || s_State.mode != Mode::Record || !s_State.running || s_State.inStep || s_State.commandDepth != 0) {
    return;
  }
  s_State.pendingCommand = Event{};
  s_State.pendingCommand.kind = RecordingEventKind::Save;
  s_State.pendingCommand.context = CaptureContext();
  s_State.pendingCommand.beforeHash = BeforeHash();
  s_State.commandDepth = 1;
  recorded_ = true;
}

GameRecorderSaveScope::~GameRecorderSaveScope()
{
  if (!recorded_) {
    return;
  }
  s_State.commandDepth = 0;
  if (s_State.mode != Mode::Record || !s_State.running) {
    return;
  }
  std::string error;
  s_State.pendingCommand.afterHash = StateHash();
  if (!s_State.writer.Append(s_State.pendingCommand, error)) {
    RecorderFailed(error);
    return;
  }
  s_State.lastHash = s_State.pendingCommand.afterHash;
  CountEvent(RecordingEventKind::Save);
}

bool GameRecorder_AutosaveAllowed()
{
  return s_State.mode != Mode::Play;
}
