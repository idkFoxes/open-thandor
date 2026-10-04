/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/session/startup.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_SESSION_STARTUP_H
#define THANDOR_GAMEPLAY_SESSION_STARTUP_H

#include <thandor/core/types.h>
#include <thandor/gameplay/session/types.h>
#include <thandor/ui/frontend/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/session/startup. */

/* Functions are grouped by semantic ownership. */

Bool8 InGameRuntime_RunSessionUntilExit(LevelAssetRuntimePrefix *levelAsset,
          FrontendBooleanState32 loadExistingSessionFlag,uint16_t *levelPathUtf16,uint32_t *outError);

void InGameRuntime_ShutdownAndReleaseResources(void);

void InGameRuntime_ReleaseFactionScratchBuffers(void);

uint8_t InGameRuntime_InitializeOptionalSubsystemAlwaysSuccess(uintptr_t unusedArgument);

void InGameSession_SetWorldRuntimeFlag(WorldRuntimeContext *world,WorldRuntimeFlags flag,Bool8 enabled);

extern int32_t g_InGamePendingSimulationTicks;

#endif /* THANDOR_GAMEPLAY_SESSION_STARTUP_H */
