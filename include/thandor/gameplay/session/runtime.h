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
/* Command codes the hotkeys queue in network games (InGameHotkeys_DispatchCommandByFlags); a local game calls the
   handler directly. */
#define INGAME_COMMAND_TOGGLE_PAUSE 0x370 /* InGameCommand_TogglePauseRequest */
#define INGAME_COMMAND_ADJUST_GAME_SPEED 0x3F0 /* InGameSimulationSpeed_AdjustPlayerAndRecomputeMinimumTicks */
/* Level script (InGameLevelConditionStorage.schedule): 64 condition records of 16 bytes and 16 end
   triggers of 8 bytes, evaluated by InGameConditionRuntime_UpdateScheduledRecords. */
#define INGAME_SCHEDULED_CONDITION_COUNT 64
#define INGAME_END_CONDITION_TRIGGER_COUNT 16
/* Tokens of a BOOLEAN_POSTFIX_EXPRESSION condition (the bytes after its kind byte); any other byte pushes the
   satisfied bit of the condition with that index onto the bit stack. */
#define INGAME_CONDITION_TOKEN_END 0xFC
#define INGAME_CONDITION_TOKEN_NOT 0xFD
#define INGAME_CONDITION_TOKEN_AND 0xFE
#define INGAME_CONDITION_TOKEN_OR 0xFF
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00564F70 */
SessionRunResult InGameRuntime_RunSessionUntilExit(LevelAssetRuntimePrefix *levelAsset,
          FrontendBooleanState32 loadExistingSessionFlag,uint16_t *levelPathUtf16);

/* 0x00566290 */
void InGameUiRoot_UpdateFrame(InGameRuntimeRootFrameView *inGameRoot);

/* 0x0050EA90 */
void ResourceRegistrationRuntime_RebaseLoadedRecords(ResourceRegistrationRuntimeImage *runtimeImage);

/* 0x00565E10 */
void __cdecl InGameRuntime_PeriodicCountdownAndClockTick(void);

/* 0x00567060 */
bool InGameHotkeys_DispatchCommandByFlags(UiKeyboardStateMask modifierFlags,UiActionId commandCode,
          InGameRuntimeRootFrameView *inGameRoot);

/* 0x00569920 */
void InGameRuntime_ProcessQueuedSessionNotificationTimer(void);

/* 0x005641D0 */
NewSessionInitResult InGameRuntime_InitializeNewSession(LevelAssetRuntimePrefix *levelAsset,uint16_t *levelMoviePath);

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
