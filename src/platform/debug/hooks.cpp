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
#include <thandor/platform/debug/script.h>
#include <thandor/platform/debug/statehash.h>
#include <thandor/platform/debug/test_aids.h>
#include <thandor/platform/selftest/selftest.h>

/* --- process --- */

int DebugHook_RunSelfTest()
{
  return SelfTest_Run(getenv("OPEN_THANDOR_SELFTEST"));
}

void DebugHook_MessagePump()
{
  DebugAutoShot_Tick();
  DebugScript_Tick();
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
  DebugCampaign_SelectCampaignLevel((uint8_t *)campaignAsset);
}

void DebugHook_CampaignCarryOver(int afterMerge)
{
  DebugCampaign_LogCarryOver(afterMerge);
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
}

void DebugHook_SessionFrameBegin()
{
  g_TestAidInGameFrames = g_TestAidInGameFrames + 1;
}

void DebugHook_SessionFrameEnd()
{
  DebugCampaign_AutoWinTick();
}

void DebugHook_SimulationStepBegin()
{
  g_TestAidInSimulationStep = 1;
}

void DebugHook_SimulationStepEnd()
{
  g_TestAidInSimulationStep = 0;
  DebugStateHash_AfterStep();
}

int DebugHook_SuppressTransientMarkers()
{
  return Thandor_TestAidStateHashActive();
}

void DebugHook_NoteOutsideStepFrom(const char *what, void *caller)
{
  Thandor_TestAidNoteOutsideStep(what, caller);
}
