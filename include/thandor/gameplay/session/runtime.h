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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00564F70 */
InGameRuntimeRunEaxCf5 __thandor_eax_cf_preserve_ecx_edx
InGameRuntime_RunSessionUntilExit
          (LevelAssetRuntimeImagePrefix370 *levelAsset,
          FrontendBooleanState32 loadExistingSessionFlag,word *levelPathUtf16);

/* 0x00566290 */
void __thandor_void_preserve_eax_ecx_edx
EndGameResultsUiRuntime_UpdateAndHandleInputCf(EndGameResultsRuntimeView44C4 *endGameResultsRuntime);

/* 0x0050EA90 */
void __thandor_void_preserve_eax_ecx_edx
InGameConditionRuntime_RebaseLoadedRecords(InGameConditionRuntime *runtime);

/* 0x00565E10 */
void __cdecl InGameRuntime_PeriodicCountdownAndClockTick(void);

/* 0x00567060 */
bool __thandor_cf_preserve_eax_ecx_edx
EndGameResultsUiRuntime_DispatchCommandByFlagsCf
          (UiKeyboardStateMask modifierFlags,UiActionId commandCode,
          EndGameResultsRuntimeView44C4 *endGameResultsRuntime);

/* 0x00569920 */
void __thandor_void_preserve_eax_ecx_edx InGameRuntime_ProcessQueuedSessionNotificationTimer(void);

/* 0x005641D0 */
InGameRuntimeInitEaxCf5 __thandor_eax_cf_preserve_ecx_edx
InGameRuntime_InitializeNewSession(LevelAssetRuntimeImagePrefix370 *levelAsset,word *levelMoviePath);

/* 0x00564920 */
InGameRuntimeLoadedInitEaxCf5 __thandor_eax_cf_preserve_ecx_edx
InGameRuntime_InitializeLoadedSession(word *savePackagePath);

/* 0x005651D0 */
void __thandor_void_preserve_eax_ecx_edx InGameRuntime_ShutdownAndReleaseResources(void);

/* 0x0050E0D0 */
void __thandor_void_preserve_eax_ecx InGameRuntime_ReleaseFactionScratchBuffers(void);

/* 0x0050E120 */
void __thandor_void_preserve_eax_ecx_edx InGameConditionRuntime_UpdateScheduledRecords(void);

/* 0x00513160 */
void __fastcall InGameRuntime_UpdateFactionResourceExtractionAndEnergyAllocationState(void);

/* 0x0053D4F0 */
void __thandor_void_preserve_eax_ecx_edx InGameRuntime_UpdateCursorGridAndViewScaleCache(void);

/* 0x005651A0 */
void __thandor_void_preserve_eax_ecx
InGameRuntime_PublishRootWorldStatePointer(UiRootNode *inGameRoot);

/* 0x00565E30 */
void __thandor_void_preserve_eax_ecx_edx InGameRuntime_UpdateSimulationAndNetworkTick(void);

/* 0x0050E0B0 */
byte __thandor_cf_preserve_eax_ecx_edx
InGameRuntime_InitializeOptionalSubsystemAlwaysSuccessCf(dword unusedArgument);

#endif /* THANDOR_GAMEPLAY_SESSION_RUNTIME_H */
