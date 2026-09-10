#ifndef THANDOR_GAMEPLAY_SESSION_RUNTIME_H
#define THANDOR_GAMEPLAY_SESSION_RUNTIME_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/session/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00564F70 */
undefined8 __fastcall InGameRuntime_RunSessionUntilExit (undefined4 param_1,undefined4 param_2,LevelAssetRuntimeImagePrefix370 *levelAsset, FrontendBooleanState32 loadExistingSessionFlag,word *levelPathUtf16);

/* 0x00566290 */
void EndGameResultsUiRuntime_UpdateAndHandleInputCf(void *endGameResultsRuntime);

/* 0x0050EA90 */
void InGameConditionRuntime_RebaseLoadedRecords(InGameConditionRuntime *runtime);

/* 0x00565E10 */
void __cdecl InGameRuntime_PeriodicCountdownAndClockTick(void);

/* 0x00567060 */
void EndGameResultsUiRuntime_DispatchCommandByFlagsCf (UiKeyboardStateMask modifierFlags,UiActionId commandCode,void *endGameResultsRuntime );

/* 0x00569920 */
void __cdecl InGameRuntime_ProcessQueuedSessionNotificationTimer(void);

/* 0x005641D0 */
undefined8 __fastcall InGameRuntime_InitializeNewSession (undefined4 param_1,undefined4 param_2,LevelAssetRuntimeImagePrefix370 *levelAsset, word *levelMoviePath);

/* 0x00564920 */
undefined8 __fastcall InGameRuntime_InitializeLoadedSession(undefined4 param_1,undefined4 param_2,word *savePackagePath);

/* 0x005651D0 */
void InGameRuntime_ShutdownAndReleaseResources(void);

/* 0x0050E0D0 */
void InGameRuntime_ReleaseFactionScratchBuffers(void);

/* 0x0050E120 */
void __fastcall InGameConditionRuntime_UpdateScheduledRecords(void);

/* 0x00513160 */
void __fastcall InGameRuntime_UpdateFactionResourceExtractionAndEnergyAllocationState(void);

/* 0x0053D4F0 */
void InGameRuntime_UpdateCursorGridAndViewScaleCache(void);

/* 0x005651A0 */
void InGameRuntime_PublishRootWorldStatePointer(UiRootNode *inGameRoot);

/* 0x00565E30 */
void __cdecl InGameRuntime_UpdateSimulationAndNetworkTick(void);

/* 0x0050E0B0 */
void InGameRuntime_InitializeOptionalSubsystemAlwaysSuccessCf(void);

#endif /* THANDOR_GAMEPLAY_SESSION_RUNTIME_H */
