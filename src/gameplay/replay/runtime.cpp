#include <algorithm>
#include <atomic>
#include <cctype>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <set>
#include <stdexcept>
#include <thandor/gameplay/replay/compact_stream.h>
#include <thandor/gameplay/replay/runtime.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/thandor.h>
#ifndef OPEN_THANDOR_REPLAY_BUILD_ID
#define OPEN_THANDOR_REPLAY_BUILD_ID "unconfigured-replay-build"
#endif
std::uint64_t GameplayReplay_StateHash();
namespace {
namespace fs = std::filesystem;
using namespace thandor::replay;
enum class Mode { Off, Record, Play, Failed };
std::atomic<Mode> mode{Mode::Off};
std::mutex clockMutex;
bool clockFrozen = false;
bool running = false, injecting = false, completed = false, userCancelled = false;
std::atomic_bool loading{false};
void freezeClock(bool frozen) {
  std::lock_guard<std::mutex> lock(clockMutex);
  clockFrozen = frozen;
}
unsigned commandDepth = 0;
fs::path recording, cacheDir, checkpoint;
bool ownsCache = false;
std::u16string checkpointUtf16;
Writer writer;
Reader reader;
Header header;
Event pendingStep, pendingCommand;
std::uint64_t lastHash = 0, eventNumber = 0;
constexpr std::uint32_t LoadSeed = 12345;
void fail(const std::string &why) {
  Thandor_Log("replay: STOP at event %llu: %s", static_cast<unsigned long long>(eventNumber), why.c_str());
  mode = Mode::Failed;
  if (g_InGameRuntimeRoot)
    g_UiCommandRuntimeFlags |= UI_COMMAND_RUNTIME_FLAG_SESSION_CLOSED;
}
std::vector<std::uint8_t> fileBytes(const fs::path &p) {
  std::ifstream f(p, std::ios::binary | std::ios::ate);
  if (!f)
    throw std::runtime_error("cannot open checkpoint");
  auto size = f.tellg();
  if (size <= 0 || size > 128 * 1024 * 1024)
    throw std::runtime_error("checkpoint size outside 1..128 MiB");
  std::vector<std::uint8_t> b(static_cast<std::size_t>(size));
  f.seekg(0);
  f.read(reinterpret_cast<char *>(b.data()), size);
  if (!f)
    throw std::runtime_error("cannot read checkpoint");
  return b;
}
std::uint64_t assetsHash() {
  std::vector<fs::path> paths;
  unsigned pckCount = 0;
  const std::set<std::string> extensions = {".pck", ".arm", ".mdl", ".spr", ".fld", ".lev", ".tec", ".sht",
                                            ".eff", ".sam", ".gfx", ".pal", ".rom", ".cgn", ".str", ".hex"};
  for (const auto &e : fs::recursive_directory_iterator(fs::current_path())) {
    if (!e.is_regular_file())
      continue;
    auto ext = e.path().extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(),
                   [](unsigned char c) { return char(std::tolower(c)); });
    if (extensions.contains(ext))
      paths.push_back(e.path());
    if (ext == ".pck")
      ++pckCount;
  }
  if (!pckCount)
    throw std::runtime_error("compact replay requires original PCK assets in the game directory");
  std::sort(paths.begin(), paths.end());
  std::uint64_t h = 14695981039346656037ull;
  char buffer[65536];
  for (const auto &p : paths) {
    auto name = fs::relative(p).generic_string();
    h = hashBytes(h, name.data(), name.size());
    std::ifstream f(p, std::ios::binary);
    if (!f)
      throw std::runtime_error("cannot fingerprint PCK asset");
    while (f) {
      f.read(buffer, sizeof buffer);
      h = hashBytes(h, buffer, static_cast<std::size_t>(f.gcount()));
    }
    if (!f.eof())
      throw std::runtime_error("PCK read error");
  }
  return h;
}
void createCache() {
  auto token = std::chrono::steady_clock::now().time_since_epoch().count();
  for (unsigned i = 0; i < 64; ++i) {
    auto candidate =
        recording.parent_path() / (".open-thandor-replay-" + std::to_string(token) + "-" + std::to_string(i));
    if (fs::create_directory(candidate)) {
      cacheDir = candidate;
      ownsCache = true;
      break;
    }
    if (i == 63)
      throw std::runtime_error("cannot create private checkpoint directory");
  }
  checkpoint = cacheDir / "initial.sve";
  checkpointUtf16 = checkpoint.u16string();
  if (checkpointUtf16.size() > 199)
    throw std::runtime_error("replay path is too long for the game's save loader");
}
Context context() {
  Context c{};
  auto &t = g_GameFactionRuntimeImage.tail;
  c[0] = t.simulationTick;
  c[1] = g_SessionNetworkTickCounter;
  c[2] = g_InGameSimulationStepTicks;
  c[3] = InGameTick_PeriodicClockTick().load(std::memory_order_relaxed);
  c[4] = t.presentationTick;
  c[5] = t.gameSpeedQ8;
  c[6] = ToBits(g_UiCommandRuntimeFlags);
  c[7] = g_RandomGeneratorState.primarySeed;
  c[8] = g_RandomGeneratorState.secondarySeed;
  c[9] = g_RandomGeneratorState.next == Random_NextSecondary ? 1u : 0u;
  c[10] = g_LocalPlayerRuntimeId;
  c[11] = g_InGameRuntimeRoot->worldRuntime.activeFactionRuntimeIndex;
  static_assert(sizeof(WorldMotionState) == 14 * sizeof(std::uint32_t));
  static_assert(std::is_trivially_copyable_v<WorldMotionState>);
  // This typed block contains only fourteen Q12/angle/distance scalars, no pointers.
  std::memcpy(c.data() + 12, &g_InGameRuntimeRoot->worldRuntime.motion, sizeof(WorldMotionState));
  return c;
}
bool apply(const Context &c) {
  if (c[0] != g_GameFactionRuntimeImage.tail.simulationTick || c[1] != g_SessionNetworkTickCounter) {
    fail("simulation/network tick diverged before event");
    return false;
  }
  if (c[2] < 1 || c[2] > INGAME_SIMULATION_STEP_TICKS_MAX || c[9] > 1 || c[10] >= 8 || c[11] >= 8) {
    fail("recorded context is invalid");
    return false;
  }
  auto &t = g_GameFactionRuntimeImage.tail;
  g_InGameSimulationStepTicks = c[2];
  InGameTick_PeriodicClockTick().store(c[3], std::memory_order_relaxed);
  t.presentationTick = c[4];
  t.gameSpeedQ8 = c[5];
  g_UiCommandRuntimeFlags = static_cast<UiCommandRuntimeFlagMask>(c[6]);
  g_RandomGeneratorState.primarySeed = c[7];
  g_RandomGeneratorState.secondarySeed = c[8];
  if (c[9])
    Random_SelectSecondaryStream();
  else
    Random_SelectPrimaryStream();
  g_LocalPlayerRuntimeId = c[10];
  g_InGameRuntimeRoot->worldRuntime.activeFactionRuntimeIndex = c[11];
  std::memcpy(&g_InGameRuntimeRoot->worldRuntime.motion, c.data() + 12, sizeof(WorldMotionState));
  return true;
}
bool append(const Event &e) {
  std::string error;
  if (!writer.append(e, error)) {
    fail(error);
    return false;
  }
  lastHash = e.afterHash;
  ++eventNumber;
  return true;
}
bool matches(std::uint64_t expected, const char *where) {
  auto actual = GameplayReplay_StateHash();
  if (actual != expected) {
    fail(std::string("state divergence ") + where + " expected=" + std::to_string(expected) +
         " actual=" + std::to_string(actual));
    return false;
  }
  lastHash = actual;
  return true;
}
} // namespace
bool GameplayReplay_PrepareSession() {
  mode = Mode::Off;
  running = injecting = completed = userCancelled = false;
  loading = false;
  freezeClock(false);
  commandDepth = 0;
  eventNumber = 0;
  header = Header{};
  const char *rec = std::getenv("OPEN_THANDOR_RECORD");
  const char *play = std::getenv("OPEN_THANDOR_REPLAY");
  if ((!rec || !*rec) && (!play || !*play))
    return true;
  try {
    if (rec && *rec && play && *play)
      throw std::runtime_error("set RECORD or REPLAY, not both");
    if (Any(g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK))
      throw std::runtime_error("this compact replay implementation supports local sessions only");
    if (std::getenv("OPEN_THANDOR_STATEHASH") || std::getenv("OPEN_THANDOR_ARENA_ORDERS") ||
        std::getenv("OPEN_THANDOR_AUTOWIN"))
      throw std::runtime_error("disable statehash/arena/autowin mutation aids while recording or replaying");
    mode = (play && *play) ? Mode::Play : Mode::Record;
    recording = fs::absolute(fs::path(mode == Mode::Play ? play : rec));
    if (mode == Mode::Record && fs::exists(recording))
      throw std::runtime_error("recording already exists; choose another filename");
    if (!fs::is_directory(recording.parent_path()))
      throw std::runtime_error("recording directory does not exist");
    createCache();
    if (mode == Mode::Play) {
      std::string error;
      if (!reader.open(recording, header, error))
        throw std::runtime_error(error);
      if (header.build != OPEN_THANDOR_REPLAY_BUILD_ID)
        throw std::runtime_error("replay requires the identical source build");
      if (header.assetsHash != assetsHash())
        throw std::runtime_error("PCK asset fingerprint differs from recording");
      std::ofstream f(checkpoint, std::ios::binary);
      f.write(reinterpret_cast<const char *>(header.checkpoint.data()),
              static_cast<std::streamsize>(header.checkpoint.size()));
      f.close();
      if (!f)
        throw std::runtime_error("cannot materialize replay checkpoint");
    } else {
      header.build = OPEN_THANDOR_REPLAY_BUILD_ID;
      header.assetsHash = assetsHash();
    }
    return true;
  } catch (const std::exception &e) {
    fail(e.what());
    GameplayReplay_Close();
    return false;
  }
}
bool GameplayReplay_Playing() { return mode == Mode::Play; }
bool GameplayReplay_RecordingRequested() { return mode == Mode::Record; }
bool GameplayReplay_LoadingCheckpoint() { return loading; }
bool GameplayReplay_Active() { return mode != Mode::Off; }
const std::uint16_t *GameplayReplay_CheckpointPath() {
  return reinterpret_cast<const std::uint16_t *>(checkpointUtf16.c_str());
}
bool GameplayReplay_CaptureInitialCheckpoint() {
  static_assert(offsetof(WorldRuntimeContext, activeFactionRuntimeIndex) ==
                offsetof(ResourceRegistrationRuntimeImageSavedView, factionAssignmentIndex));
  static_assert(offsetof(WorldRuntimeContext, fieldGrid) ==
                offsetof(InGameFieldImageSaveContext58, fieldGridAsset));
  static_assert(offsetof(WorldRuntimeContext, objectArray) ==
                offsetof(ResourceRegistrationRuntimeImageSavedView, records));
  static_assert(offsetof(WorldRuntimeContext, objectCount) ==
                offsetof(ResourceRegistrationRuntimeImageSavedView, recordCount));
  freezeClock(true);
  auto error = InGameSaveGame_WritePackage(&g_InGameRuntimeRoot->worldRuntime,
                                           const_cast<std::uint16_t *>(GameplayReplay_CheckpointPath()));
  freezeClock(false);
  if (error) {
    fail("cannot create initial save checkpoint");
    return false;
  }
  return true;
}
void GameplayReplay_BeginCheckpointLoad() {
  loading = true;
  freezeClock(true);
  Random_SetBothSeeds(LoadSeed);
  Random_SelectSecondaryStream();
}
void GameplayReplay_EndCheckpointLoad() {
  loading = false;
  freezeClock(false);
}
bool GameplayReplay_SessionStarted() {
  if (mode == Mode::Off)
    return true;
  if (mode == Mode::Failed)
    return false;
  Random_SelectSecondaryStream();
  if (mode == Mode::Play) {
    // Loader intentionally runs no simulation steps in either mode. Its counters
    // are identical; presentation/RNG/listener values are restored explicitly.
    if (!apply(header.initialContext) || !matches(header.initialHash, "at initial checkpoint"))
      return false;
  } else {
    try {
      header.checkpoint = fileBytes(checkpoint);
      header.initialContext = context();
      header.initialHash = GameplayReplay_StateHash();
      std::string error;
      if (!writer.open(recording, header, error)) {
        fail(error);
        return false;
      }
      lastHash = header.initialHash;
    } catch (const std::exception &e) {
      fail(e.what());
      return false;
    }
  }
  header.checkpoint.clear();
  header.checkpoint.shrink_to_fit();
  running = true;
  Thandor_Log(
      "replay: %s %s; one compressed checkpoint + ordered commands/step contexts, no per-tick actor dumps",
      mode == Mode::Play ? "playing" : "recording", recording.string().c_str());
  return true;
}
bool GameplayReplay_BeforeStep() {
  if (loading || commandDepth)
    return false;
  if (!running)
    return mode != Mode::Failed;
  if (mode == Mode::Record) {
    freezeClock(true);
    pendingStep = Event{};
    pendingStep.context = context();
    pendingStep.beforeHash = GameplayReplay_StateHash();
    return true;
  }
  if (mode != Mode::Play)
    return false;
  for (unsigned count = 0; count < 65536; ++count) {
    Event e;
    std::string error;
    if (!reader.next(e, error)) {
      fail(error);
      return false;
    }
    ++eventNumber;
    if (e.kind == Kind::End) {
      // A frame may advance UI RNG after the last simulated event. The footer
      // commits the last completed event, whose post-state was already checked.
      if (e.afterHash != lastHash) {
        fail("footer differs from last completed event");
        return false;
      }
      completed = true;
      Thandor_Log("replay: COMPLETE, all %llu event checks matched",
                  static_cast<unsigned long long>(eventNumber - 1));
      g_UiCommandRuntimeFlags |= UI_COMMAND_RUNTIME_FLAG_SESSION_CLOSED;
      return false;
    }
    if (!apply(e.context) || !matches(e.beforeHash, "before event"))
      return false;
    if (e.kind == Kind::Step) {
      pendingStep = e;
      return true;
    }
    UiCommandQueueRecord command{};
    command.packedCommandAndPlayerId = e.command[0];
    command.payload1 = e.command[1];
    command.payload2 = e.command[2];
    command.payload3 = e.command[3];
    injecting = true;
    CommandDispatch_ExecuteRecord(INGAME_COMMAND_CODE_BASE, INGAME_COMMAND_HANDLER_REGION_END, &command);
    injecting = false;
    if (!matches(e.afterHash, "after command"))
      return false;
  }
  fail("too many commands without a simulation step");
  return false;
}
void GameplayReplay_AfterStep() {
  if (!running)
    return;
  if (mode == Mode::Record) {
    pendingStep.afterHash = GameplayReplay_StateHash();
    append(pendingStep);
    freezeClock(false);
  } else if (mode == Mode::Play)
    matches(pendingStep.afterHash, "after step");
}
GameplayReplayCommandGuard::GameplayReplayCommandGuard(std::uint32_t code, std::uint32_t player,
                                                       std::uint32_t p1, std::uint32_t p2, std::uint32_t p3) {
  if (!running)
    return;
  if (code > 0xffffffu || player > 255u) {
    fail("command is not represented by the native command table");
    execute_ = false;
    return;
  }
  if (mode == Mode::Play && !injecting) {
    if (code == INGAME_COMMAND_PLAYER_DEPARTURE && (p3 & INGAME_PLAYER_DEPARTURE_FLAG_CLOSE_SESSION)) {
      userCancelled = true;
      Thandor_Log("replay: cancelled by user");
      return;
    }
    execute_ = false;
    return;
  }
  if (mode == Mode::Failed) {
    execute_ = false;
    return;
  }
  entered_ = true;
  if (commandDepth++ == 0 && mode == Mode::Record) {
    freezeClock(true);
    pendingCommand = Event{};
    pendingCommand.kind = Kind::Command;
    pendingCommand.command = {(code << 8) | (player & 255u), p1, p2, p3};
    pendingCommand.context = context();
    pendingCommand.beforeHash = GameplayReplay_StateHash();
  }
}
GameplayReplayCommandGuard::~GameplayReplayCommandGuard() {
  if (!entered_)
    return;
  if (--commandDepth == 0 && mode == Mode::Record) {
    pendingCommand.afterHash = GameplayReplay_StateHash();
    append(pendingCommand);
    freezeClock(false);
  }
}
void GameplayReplay_Close() {
  if (writer.active()) {
    std::string error;
    if (mode == Mode::Record && writer.finish(lastHash, error))
      Thandor_Log("replay: recording finalized (%llu events)", static_cast<unsigned long long>(eventNumber));
    else if (!error.empty())
      Thandor_Log("replay: recording footer FAILED: %s", error.c_str());
  }
  if (running && mode == Mode::Play && !completed && !userCancelled)
    Thandor_Log("replay: stopped before clean completion");
  running = injecting = false;
  loading = false;
  freezeClock(false);
  mode = Mode::Off;
  writer = Writer{};
  reader = Reader{};
  if (ownsCache) {
    std::error_code ec;
    fs::remove(checkpoint, ec);
    fs::remove(cacheDir, ec);
    cacheDir.clear();
    checkpoint.clear();
    checkpointUtf16.clear();
    ownsCache = false;
  }
}

void GameplayReplay_PeriodicClockTick() {
  std::lock_guard<std::mutex> lock(clockMutex);
  if (!clockFrozen && mode != Mode::Play && !loading &&
      InGameTick_ResourceRegistrationBusyCount().load(std::memory_order_relaxed) == 0)
    InGameTick_PeriodicClockTick().fetch_add(1, std::memory_order_relaxed);
}

void GameplayReplay_ClosingWorld() {
  if (!running)
    return;
  if (mode == Mode::Record || userCancelled)
    return;
  if (mode == Mode::Play && !completed) {
    Event e;
    std::string error;
    if (reader.next(e, error) && e.kind == Kind::End && e.afterHash == lastHash) {
      completed = true;
      Thandor_Log("replay: COMPLETE at session end, all event checks matched");
    } else if (mode == Mode::Play)
      fail(error.empty() ? "session ended before the recording footer" : error);
  }
}

void GameplayReplay_ConfigureLoadedPlayer(std::uint32_t faction) {
  if (!loading)
    return;
  // AI planning derives human ownership from frontend player records, rather
  // than the saved selection block. Normalize both record and playback loads.
  if (!g_FrontendPlayerRuntimeBlocks || faction < 1 || faction > 7) {
    fail("invalid local checkpoint player");
    return;
  }
  g_FrontendPlayerRuntimeBlockCount = 1;
  auto &player = g_FrontendPlayerRuntimeBlocks[0];
  player.playerRuntimeId = 0;
  player.factionAssignment = {};
  player.factionAssignment.factionAssignmentIndex = faction;
}
