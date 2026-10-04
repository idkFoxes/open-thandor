/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/debug/hooks.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_PLATFORM_DEBUG_HOOKS_H
#define THANDOR_PLATFORM_DEBUG_HOOKS_H

/* The one interface between the game and the developer tools (src/platform/debug, src/platform/selftest).

   The tools are compiled in only with the CMake option THANDOR_DEV_TOOLS (default OFF; preset "test",
   build-test). Then the hooks below are functions in src/platform/debug/hooks.c, and the tools are switched on
   at run time by OPEN_THANDOR_* environment variables (docs/BUILDING.md): self-tests, input scripts, automatic
   screenshots, the determinism state hash, campaign starts and AUTOWIN, the level-script log, movie player,
   dump and compare, windowed mode, a second instance, UDP port and datagram log. Without a variable a hook
   does nothing and the game runs as the original.

   With the option OFF every hook below is a macro that expands to nothing (or to the original value it
   passes through), so the release build contains none of the tools and runs the original code paths.
   Game code calls only these hooks; it has no #ifdef THANDOR_DEV_TOOLS of its own. */

#include <stdint.h>
#include <thandor/movie/runtime/types.h>
#include <thandor/network/backend/types.h>
#include <thandor/platform/debug/types.h>

#ifdef THANDOR_DEV_TOOLS

#ifdef _MSC_VER
#include <intrin.h> /* _ReturnAddress */
#endif

/* --- process --- */
/* WinMain, before ProcessEntry: runs the self-test OPEN_THANDOR_SELFTEST names; nonzero when one ran (the
   process then exits). */
int DebugHook_RunSelfTest();
/* SdlPlatform_PumpEvents, before the queue is read: automatic screenshots and the input script. */
void DebugHook_MessagePump();
/* Game_PlayIntroMovies, before the intro: the debug movie player and export (both exit the process). */
void DebugHook_BeforeIntroMovies();
/* ProcessEntry: nonzero lets a second instance start although a game window exists. */
int DebugHook_AllowSecondInstance();
/* SetPriorityClass: the priority class to use instead of the original's real-time class (windowed test runs
   keep normal priority, many instances at real-time priority starve each other). */
unsigned long DebugHook_ProcessPriorityClass(unsigned long priorityClass);

/* --- windowed mode (OPEN_THANDOR_WINDOWED) --- */
/* Nonzero when the game runs in a normal window (at OPEN_THANDOR_WINDOW_X/Y, the desktop's colour depth) instead
   of full screen (SdlPlatform_CreateMainWindow, SdlVideo_ApplyDisplayMode, normal process priority). */
int DebugHook_Windowed();

/* --- input --- */
/* Nonzero while an input script drives the game: the real mouse is then ignored. */
int DebugHook_IgnoreRealMouse();

/* --- network (OPEN_THANDOR_NET_PORT, OPEN_THANDOR_NETLOG) --- */
/* Around the bind of the game's UDP socket: binds to another port, then restores the game port in the
   endpoint, so everything that reads it afterwards sees the game port as in the original. */
void DebugHook_BeforeUdpBind(WinSockAddress *bindEndpoint, unsigned gamePort);
void DebugHook_AfterUdpBind(WinSockAddress *bindEndpoint, unsigned gamePort);
/* Every datagram sent or received ("send" / "recv"). */
void DebugHook_UdpDatagram(const char *direction, const void *sockaddrIn, unsigned byteCount, const void *buffer);

/* --- movie decoder (OPEN_THANDOR_MOVIEDUMP) --- */
void DebugHook_MovieFrameDone(MovieRuntime *movie, uint32_t consumedBytes);

/* --- campaign and scenario selection --- */
/* "Choose game" page of a local game built: OPEN_THANDOR_LIST_SCENARIOS / OPEN_THANDOR_CAMPAIGN; nonzero when a
   campaign was started (the page handler then returns). */
int DebugHook_ScenarioPageOpened();
/* A campaign asset was just loaded (OPEN_THANDOR_CAMPAIGN_LEVEL). */
void DebugHook_CampaignLoaded(void *campaignAsset);
/* Level start of a campaign, before (afterMerge 0) and after (1) the carried-over units are merged. */
void DebugHook_CampaignCarryOver(int afterMerge);

/* --- in-game session --- */
/* InGameRuntime_RunSessionUntilExit, before the session is initialised (the first simulation steps already run
   during the initialisation): state hash random seed. */
void DebugHook_SessionInitializing();
/* After the session is initialised, before its first frame (session counter, state hash start). */
void DebugHook_SessionStarted();
/* Start and end of every in-game frame (frame counter for the input script; OPEN_THANDOR_AUTOWIN). */
void DebugHook_SessionFrameBegin();
void DebugHook_SessionFrameEnd();
/* Around every simulation step (OPEN_THANDOR_STATEHASH). */
void DebugHook_SimulationStepBegin();
void DebugHook_SimulationStepEnd();
/* Level-script log of InGameConditionRuntime_UpdateScheduledRecords: before and after the conditions are
   evaluated, and when an end trigger fires. */
void DebugHook_LevelScriptBeforeEvaluation();
void DebugHook_LevelScriptAfterEvaluation(const InGameLevelConditionStorage *storage);
void DebugHook_LevelScriptEndTrigger(const InGameLevelConditionStorage *storage, int triggerIndex,
                                     const InGameEndConditionTriggerRecord8 *trigger);
/* Nonzero while the state hash runs: the world overlay then builds no transient markers (they take effect
   slots and world objects shared with the simulation from the render path). */
int DebugHook_SuppressTransientMarkers();
/* Effect creation and session random draws: with the state hash on, logs callers outside a simulation step. */
void DebugHook_NoteOutsideStepFrom(const char *what, void *caller);
#ifdef _MSC_VER
#define DebugHook_NoteOutsideStep(what) DebugHook_NoteOutsideStepFrom((what), _ReturnAddress())
#else
#define DebugHook_NoteOutsideStep(what) DebugHook_NoteOutsideStepFrom((what), __builtin_return_address(0))
#endif

#else /* !THANDOR_DEV_TOOLS: no developer tools, the original behaviour */

#define DebugHook_RunSelfTest() 0
#define DebugHook_MessagePump() ((void)0)
#define DebugHook_BeforeIntroMovies() ((void)0)
#define DebugHook_AllowSecondInstance() 0
#define DebugHook_ProcessPriorityClass(priorityClass) (priorityClass)

#define DebugHook_Windowed() 0

#define DebugHook_IgnoreRealMouse() 0

#define DebugHook_BeforeUdpBind(bindEndpoint, gamePort) ((void)0)
#define DebugHook_AfterUdpBind(bindEndpoint, gamePort) ((void)0)
#define DebugHook_UdpDatagram(direction, sockaddrIn, byteCount, buffer) ((void)0)

#define DebugHook_MovieFrameDone(movie, consumedBytes) ((void)0)

#define DebugHook_ScenarioPageOpened() 0
#define DebugHook_CampaignLoaded(campaignAsset) ((void)0)
#define DebugHook_CampaignCarryOver(afterMerge) ((void)0)

#define DebugHook_SessionInitializing() ((void)0)
#define DebugHook_SessionStarted() ((void)0)
#define DebugHook_SessionFrameBegin() ((void)0)
#define DebugHook_SessionFrameEnd() ((void)0)
#define DebugHook_SimulationStepBegin() ((void)0)
#define DebugHook_SimulationStepEnd() ((void)0)
#define DebugHook_LevelScriptBeforeEvaluation() ((void)0)
#define DebugHook_LevelScriptAfterEvaluation(storage) ((void)0)
#define DebugHook_LevelScriptEndTrigger(storage, triggerIndex, trigger) ((void)0)
#define DebugHook_SuppressTransientMarkers() 0
#define DebugHook_NoteOutsideStep(what) ((void)0)

#endif /* THANDOR_DEV_TOOLS */

#endif /* THANDOR_PLATFORM_DEBUG_HOOKS_H */
