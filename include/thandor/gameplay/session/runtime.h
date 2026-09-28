/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/session/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_SESSION_RUNTIME_H
#define THANDOR_GAMEPLAY_SESSION_RUNTIME_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/session/runtime. */

/* Timer rate of InGameRuntime_PeriodicCountdownAndClockTick (TimerSystem_RegisterPeriodic takes a frequency). */
#define INGAME_PERIODIC_TIMER_HZ 80
/* Reload value of g_InGameNetworkTickCountdown: the periodic timer counts it down, and a simulation step only
   runs at zero, so the game advances at most 80 / 4 = 20 steps per second. */
#define INGAME_TIMER_TICKS_PER_SIMULATION_STEP 4
/* Command code (InGameCommandQueue_AppendLocalPlayerCommand) with which a player reports its level as loaded;
   single player calls its handler FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus directly. */
#define INGAME_COMMAND_PLAYER_READY 0x550
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00564F70 */
SessionRunResult InGameRuntime_RunSessionUntilExit(LevelAssetRuntimeImagePrefix370 *levelAsset,
          FrontendBooleanState32 loadExistingSessionFlag,uint16_t *levelPathUtf16);

/* 0x00566290 */
void EndGameResultsUiRuntime_UpdateAndHandleInput(EndGameResultsRuntimeView44C4 *endGameResultsRuntime);

/* 0x0050EA90 */
void ResourceRegistrationRuntime_RebaseLoadedRecords(ResourceRegistrationRuntimeImage *runtimeImage);

/* 0x00565E10 */
void __cdecl InGameRuntime_PeriodicCountdownAndClockTick(void);

/* 0x00567060 */
bool InGameHotkeys_DispatchCommandByFlags(UiKeyboardStateMask modifierFlags,UiActionId commandCode,
          EndGameResultsRuntimeView44C4 *endGameResultsRuntime);

/* 0x00569920 */
void InGameRuntime_ProcessQueuedSessionNotificationTimer(void);

/* 0x005641D0 */
NewSessionInitResult InGameRuntime_InitializeNewSession(LevelAssetRuntimeImagePrefix370 *levelAsset,uint16_t *levelMoviePath);

/* 0x00564920 */
LoadedSessionInitResult InGameRuntime_InitializeLoadedSession(uint16_t *savePackagePath);

/* 0x005651D0 */
void InGameRuntime_ShutdownAndReleaseResources(void);

/* 0x0050E0D0 */
void InGameRuntime_ReleaseFactionScratchBuffers(void);

/* 0x0050E120 */
void InGameConditionRuntime_UpdateScheduledRecords(void);

/* 0x00513160 */
void __fastcall InGameRuntime_UpdateFactionResourceExtractionAndEnergyAllocationState(void);

/* 0x0053D4F0 */
void InGameRuntime_UpdateCursorGridAndViewScaleCache(void);

/* 0x005651A0 */
void InGameRuntime_SaveWorldViewInfoTextChoice(UiRootNode *inGameRoot);

/* 0x00565E30 */
void InGameRuntime_UpdateSimulationAndNetworkTick(void);

/* 0x0050E0B0 */
uint8_t InGameRuntime_InitializeOptionalSubsystemAlwaysSuccess(uint32_t unusedArgument);

#endif /* THANDOR_GAMEPLAY_SESSION_RUNTIME_H */
