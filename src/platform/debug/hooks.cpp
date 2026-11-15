/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/debug/hooks.cpp
 * Project code (not in the original game)
 */

/* The developer-tool hooks the game calls (thandor/platform/debug/hooks.h), compiled only with the CMake option
   THANDOR_DEV_TOOLS. Each hook hands over to the tool behind it; without its environment variable a tool does
   nothing. The level-script log is in level_script.cpp. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>
#include <thandor/platform/debug/autoshot.h>
#include <thandor/platform/debug/campaign.h>
#include <thandor/platform/debug/movie_decoder.h>
#include <thandor/platform/debug/movie_player.h>
#include <thandor/platform/debug/pathtrace.h>
#include <thandor/platform/debug/profile.h>
#include <thandor/platform/debug/script.h>
#include <thandor/platform/debug/statehash.h>
#include <thandor/platform/debug/test_aids.h>
#include <thandor/platform/selftest/selftest.h>
#include <thandor/world/pathing/mode.h>

/* --- process --- */

int DebugHook_RunSelfTest()
{
  return SelfTest_Run(getenv("OPEN_THANDOR_SELFTEST"));
}

void DebugHook_MessagePump()
{
  DebugAutoShot_Tick();
  DebugScript_Tick();
  Thandor_TestAidRestoreWindowWhenDue(g_MainWindow);
}

/* OPEN_THANDOR_MOVIEEXPORT=<name>[,<name>...] exports those movies and exits; OPEN_THANDOR_MOVIE=<name>|all plays
   them in the debug movie player (which exits the process when done). */
void DebugHook_BeforeIntroMovies()
{
  const char *exportMovies = getenv("OPEN_THANDOR_MOVIEEXPORT");
  const char *debugMovie = getenv("OPEN_THANDOR_MOVIE");
  if ((exportMovies != nullptr) && (exportMovies[0] != 0)) {
    char names[256];
    char *name;
    snprintf(names, sizeof names, "%s", exportMovies);
    for (name = strtok(names, ","); name != nullptr; name = strtok(nullptr, ",")) {
      DebugMovie_ExportOne(name);
    }
    ExitProcess(0);
  }
  if ((debugMovie != nullptr) && (debugMovie[0] != 0)) {
    DebugMovie_Run(debugMovie);
  }
}

int DebugHook_AllowSecondInstance()
{
  return Thandor_TestAidAllowSecondInstance();
}

unsigned long DebugHook_ProcessPriorityClass(unsigned long priorityClass)
{
  return DebugHook_Windowed() ? NORMAL_PRIORITY_CLASS : priorityClass;
}

/* --- windowed mode --- */

int DebugHook_Windowed()
{
  return Thandor_TestAidWindowed();
}

int DebugHook_WindowMinimized()
{
  return Thandor_TestAidWindowMinimized();
}

/* --- input --- */

int DebugHook_IgnoreRealMouse()
{
  return Thandor_TestAidScriptActive();
}

int DebugHook_InterceptOpenUrl(const char *url)
{
  const char *mode = getenv("OPEN_THANDOR_OPEN_URL");

  if ((mode == nullptr) || (strcmp(mode,"log") != 0)) {
    return 0;
  }
  Thandor_Log("open url (OPEN_THANDOR_OPEN_URL=log, not opened): %s", url);
  return 1;
}

/* --- network --- */

static int s_udpPortOverridden;

void DebugHook_BeforeUdpBind(WinSockAddress *bindEndpoint, unsigned gamePort)
{
  unsigned port = Thandor_TestAidNetworkBindPort(gamePort);
  s_udpPortOverridden = port != gamePort;
  if (s_udpPortOverridden) {
    bindEndpoint->addressHeader.fields.portNetworkOrder = g_WinSock_htons((uint16_t)port);
  }
}

void DebugHook_AfterUdpBind(WinSockAddress *bindEndpoint, unsigned gamePort)
{
  /* the local/broadcast descriptor and NetworkFallback_ParsePeerEndpoint keep using the game port */
  if (s_udpPortOverridden) {
    bindEndpoint->addressHeader.fields.portNetworkOrder = g_WinSock_htons((uint16_t)gamePort);
    s_udpPortOverridden = 0;
  }
}

void DebugHook_UdpDatagram(const char *direction, const void *sockaddrIn, unsigned byteCount, const void *buffer)
{
  Thandor_TestAidLogDatagram(direction, sockaddrIn, byteCount, buffer);
}

/* --- movie decoder --- */

void DebugHook_MovieFrameDone(MovieRuntime *movie, uint32_t consumedBytes)
{
  DebugMovieDecoder_DumpFrame(movie, consumedBytes);
}

/* --- campaign and scenario selection --- */

int DebugHook_ScenarioPageOpened()
{
  /* local games only */
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) != SESSION_NETWORK_ROLE_LOCAL) {
    return 0;
  }
  return DebugCampaign_ApplyScenarioOptions();
}

void DebugHook_CampaignLoaded(void *campaignAsset)
{
  DebugCampaign_SelectCampaignLevel(static_cast<uint8_t *>(campaignAsset));
  DebugCampaign_DumpProgress("campaign loaded");
}

void DebugHook_CampaignCarryOver(int afterMerge)
{
  DebugCampaign_LogCarryOver(afterMerge);
}

void DebugHook_CampaignSessionEnded()
{
  DebugCampaign_DumpProgress("session ended");
}

/* --- ghost-target counter of the AI group target (OPEN_THANDOR_AI_GHOST_COUNT=1) --- */

namespace {

/* -1: not read yet */
int s_aiGhostCounting = -1;
/* totals of the current session: group target choices, root-node skips, changed target armies */
unsigned s_aiGhostChoices;
unsigned s_aiGhostSkips;
unsigned s_aiGhostChanges;
/* group choices of the last summary line */
unsigned s_aiGhostLoggedChoices;

/* Summary every this many simulation ticks when a group choice was made since the last one (test games are killed, so
   the last line of a session is at most this old). */
constexpr unsigned AI_GHOST_SUMMARY_TICKS = 600;

void AiGhost_LogTotals(const char *when)
{
  Thandor_Log("test aid: ai ghost target %s: session %u tick %u: %u group choices, %u skips, %u decision changes",
              when, g_TestAidSessionCount, g_GameFactionRuntimeImage.tail.simulationTick, s_aiGhostChoices,
              s_aiGhostSkips, s_aiGhostChanges);
  s_aiGhostLoggedChoices = s_aiGhostChoices;
}

void AiGhost_SessionStarted()
{
  if (DebugHook_AiGhostTargetCounting() == 0) {
    return;
  }
  if (s_aiGhostChoices != 0) {
    AiGhost_LogTotals("previous session total");
  }
  s_aiGhostChoices = 0;
  s_aiGhostSkips = 0;
  s_aiGhostChanges = 0;
  s_aiGhostLoggedChoices = 0;
}

void AiGhost_AfterStep()
{
  if (s_aiGhostCounting != 1 ||
      g_GameFactionRuntimeImage.tail.simulationTick % AI_GHOST_SUMMARY_TICKS != 0 ||
      s_aiGhostChoices == s_aiGhostLoggedChoices) {
    return;
  }
  AiGhost_LogTotals("totals");
}

/* after the frame that closes the session (the three exits of InGameRuntime_RunSessionUntilExit) */
void AiGhost_FrameEnd()
{
  if (s_aiGhostCounting != 1 ||
      !Any(g_UiCommandRuntimeFlags & (UI_COMMAND_RUNTIME_FLAG_SESSION_CLOSED | UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING |
                                      UI_COMMAND_RUNTIME_FLAG_LOCAL_PLAYER_LEFT))) {
    return;
  }
  AiGhost_LogTotals("session end total");
}

}  // namespace

int DebugHook_AiGhostTargetCounting()
{
  if (s_aiGhostCounting < 0) {
    const char *value = getenv("OPEN_THANDOR_AI_GHOST_COUNT");
    s_aiGhostCounting = value != nullptr && value[0] == '1';
    if (s_aiGhostCounting != 0) {
      Thandor_Log("test aid: ai ghost target counter on (OPEN_THANDOR_AI_GHOST_COUNT)");
    }
  }
  return s_aiGhostCounting;
}

void DebugHook_AiGhostTargetCounted(unsigned rootlessSkips, int decisionChanged)
{
  s_aiGhostChoices = s_aiGhostChoices + 1;
  s_aiGhostSkips = s_aiGhostSkips + rootlessSkips;
  if (decisionChanged != 0) {
    s_aiGhostChanges = s_aiGhostChanges + 1;
    Thandor_Log("test aid: ai ghost target changed a group target: session %u tick %u (%u skipped candidates)",
                g_TestAidSessionCount, g_GameFactionRuntimeImage.tail.simulationTick, rootlessSkips);
  }
}

/* --- in-game session --- */

void DebugHook_SessionInitializing()
{
  DebugStateHash_SessionInitializing();
}

void DebugHook_SessionStarted()
{
  g_TestAidSessionCount = g_TestAidSessionCount + 1;
  DebugStateHash_SessionStart();
  DebugProfile_SessionStarted();
  AiGhost_SessionStarted();
  if (g_FrontendLoadedCampaignAsset != nullptr) {
    DebugCampaign_LogLevelStart();
    DebugCampaign_DumpProgress("level started");
  }
}

void DebugHook_SessionFrameBegin()
{
  g_TestAidInGameFrames = g_TestAidInGameFrames + 1;
  DebugProfile_FrameBegin();
}

void DebugHook_SessionFrameEnd()
{
  DebugProfile_FrameEnd();
  DebugCampaign_AutoWinTick();
  DebugCampaign_AutoLoseTick();
  AiGhost_FrameEnd();
}

void DebugHook_SimulationStepBegin()
{
  g_TestAidInSimulationStep = 1;
  DebugProfile_StepBegin();
}

void DebugHook_SimulationStepEnd()
{
  g_TestAidInSimulationStep = 0;
  DebugProfile_StepEnd(g_GameFactionRuntimeImage.tail.simulationTick);
  DebugPathTrace_AfterStep();
  DebugStateHash_AfterStep();
  AiGhost_AfterStep();
  /* OPEN_THANDOR_CRASH_AT_TICK=<n>: an access violation at the end of the simulation step of tick n or later, to test
     the crash report and a crash recording of the game recorder (gameplay/replay/runtime.h) */
  static const long s_crashAtTick = getenv("OPEN_THANDOR_CRASH_AT_TICK") != nullptr
                                        ? strtol(getenv("OPEN_THANDOR_CRASH_AT_TICK"), nullptr, 10) : 0;
  if (s_crashAtTick > 0 && g_GameFactionRuntimeImage.tail.simulationTick >= static_cast<uint32_t>(s_crashAtTick)) {
    Thandor_Log("test aid: OPEN_THANDOR_CRASH_AT_TICK %ld, crashing now", s_crashAtTick);
    *static_cast<volatile int *>(nullptr) = 0;
  }
}

void DebugHook_PathingEvent(int kind, const void *army, const void *other)
{
  DebugPathTrace_Event(kind, army, other);
}

int DebugHook_LocalPathingMode(int mode)
{
  const char *value = getenv("OPEN_THANDOR_PATHING");

  if (value == nullptr) {
    return mode;
  }
  if (_stricmp(value, "improved") == 0) {
    return PATHING_MODE_IMPROVED;
  }
  if (_stricmp(value, "original") == 0) {
    return PATHING_MODE_ORIGINAL;
  }
  return mode; /* empty or "setting": the setting's mode */
}

int DebugHook_AutosaveIntervalSeconds(int seconds)
{
  const char *value = getenv("OPEN_THANDOR_AUTOSAVE");
  char *end = nullptr;
  long parsed;

  if (value == nullptr || value[0] == 0) {
    return seconds;
  }
  if (_stricmp(value, "off") == 0) {
    return 0;
  }
  parsed = strtol(value, &end, 10);
  if (end == value || *end != 0 || parsed < 0) {
    return seconds; /* not a number: the setting's interval */
  }
  return parsed > 0x7FFF ? 0x7FFF : static_cast<int>(parsed); /* 0 switches it off */
}

int DebugHook_OriginalMode(int on)
{
  /* -1: not read yet, 2: no override; read once, since every feature asks the switch (some every frame) */
  static int s_override = -1;

  if (s_override < 0) {
    const char *value = getenv("OPEN_THANDOR_ORIGINAL");
    s_override = 2;
    if (value != nullptr && (strcmp(value, "1") == 0 || _stricmp(value, "on") == 0)) {
      s_override = 1;
    }
    else if (value != nullptr && (strcmp(value, "0") == 0 || _stricmp(value, "off") == 0)) {
      s_override = 0;
    }
    if (s_override != 2) {
      Thandor_Log("original mode: OPEN_THANDOR_ORIGINAL=%s overrides [game] original_thandor", value);
    }
  }
  return s_override == 2 ? on : s_override;
}

void DebugHook_ProfileEnter(int section)
{
  DebugProfile_Enter(section);
}

void DebugHook_ProfileLeave(int section)
{
  DebugProfile_Leave(section);
}

int DebugHook_SuppressTransientMarkers()
{
  return Thandor_TestAidStateHashActive();
}

void DebugHook_NoteOutsideStepFrom(const char *what, void *caller)
{
  Thandor_TestAidNoteOutsideStep(what, caller);
}
