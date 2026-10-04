/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/session/startup.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_SESSION_STARTUP_H
#define THANDOR_GAMEPLAY_SESSION_STARTUP_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/session/startup. */

/* Timer rate of InGameRuntime_PeriodicCountdownAndClockTick (g_TimerRegisterPeriodic takes a frequency). */
#define INGAME_PERIODIC_TIMER_HZ 80
/* Reload value of g_InGameNetworkTickCountdown: the periodic timer counts it down, and a simulation step only
   runs at zero, so the game advances at most 80 / 4 = 20 steps per second. */
#define INGAME_TIMER_TICKS_PER_SIMULATION_STEP 4
/* Command code (InGameCommandQueue_AppendLocalPlayerCommand) with which a player reports its level as loaded;
   single player calls its handler FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus directly. */
#define INGAME_COMMAND_PLAYER_READY 0x550

/* World object pool of a session (InGameRuntime_InitializeNewSession, InGameRuntime_InitializeLoadedSession): records of
   sizeof(WorldObjectRecord) bytes, handed to WorldRuntime_AttachObjectArray */
#define INGAME_WORLD_OBJECT_RECORD_COUNT 0x4000
#define INGAME_WORLD_DWORD_ARRAY_COUNT 256 /* g_InGameWorldRuntimeDwordArray256 */

/* Camera pitch clamp of a session (WorldRuntimeContext.motion, angle16 as unsigned dwords): -0x3C00 and -0x1800 */
#define INGAME_CAMERA_MINIMUM_PITCH_ANGLE16 0xFFFFC400
#define INGAME_CAMERA_MAXIMUM_PITCH_ANGLE16 0xFFFFE800

/* Functions are grouped by semantic ownership. */

Bool8 InGameRuntime_RunSessionUntilExit(LevelAssetRuntimePrefix *levelAsset,
          FrontendBooleanState32 loadExistingSessionFlag,uint16_t *levelPathUtf16,uint32_t *outError);

Bool8 InGameRuntime_InitializeNewSession(LevelAssetRuntimePrefix *levelAsset,uint16_t *levelMoviePath,
                                        uint32_t *outError);

Bool8 InGameRuntime_InitializeLoadedSession(uint16_t *savePackagePath,uint32_t *outError);

void InGameRuntime_ShutdownAndReleaseResources(void);

void InGameRuntime_ReleaseFactionScratchBuffers(void);

uint8_t InGameRuntime_InitializeOptionalSubsystemAlwaysSuccess(uintptr_t unusedArgument);

extern SelectionPlayerRuntimeBlock *g_SelectionPlayerBlocks;

extern uint32_t g_EndMovieVariantIndex;
extern uint16_t *g_EndMoviePath;
extern uint32_t g_HostCommandBatchSyncSentThisInterval;
extern int32_t g_InGamePendingSimulationTicks;
extern uint32_t g_EndGameResultsCurrentMusicTrackId;

#endif /* THANDOR_GAMEPLAY_SESSION_STARTUP_H */
