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
#include <thandor/gameplay/selection/types.h>
#include <thandor/ui/frontend/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

Bool8 InGameRuntime_RunSessionUntilExit(LevelAssetRuntimePrefix *levelAsset,
          FrontendBooleanState32 loadExistingSessionFlag,uint16_t *levelPathUtf16,uint32_t *outError);

void InGameRuntime_ShutdownAndReleaseResources();

void InGameRuntime_ReleaseFactionScratchBuffers();

void InGameSession_SetWorldRuntimeFlag(WorldRuntimeContext *world,WorldRuntimeFlags flag,Bool8 enabled);

/* Shared steps of InGameRuntime_InitializeNewSession and InGameRuntime_InitializeLoadedSession. */

void InGameSession_ResetTickState();

void InGameSession_InstallStepTimerAndHooks();

Bool8 InGameSession_CreateRoot(SelectionInfoEntitySlots *localPlayerInfoSlots,InGameRuntimeRoot **outRoot,
          uint32_t *outError);

Bool8 InGameSession_OpenLoadingMovieAndAttachObjects(uint16_t *levelMoviePath,LevelAssetHeader *levelHeader,
          InGameRuntimeRoot *inGameRoot,uint32_t *outError);

Bool8 InGameSession_ClearNotificationsAndCreateTerrainTexture(InGameRuntimeRoot *inGameRoot,uint32_t *outError);

void InGameSession_InitShadingAndMirrorViewOptions(WorldRuntimeContext *world);

Bool8 InGameSession_AllocateGridScratchAndRebuildDerived(WorldRuntimeContext *world,uint32_t *outError);

void InGameSession_RebuildUiGrids(InGameRuntimeRoot *inGameRoot);

void InGameSession_ReportReadyAndWaitForPlayers(InGameRuntimeRoot *inGameRoot);

extern int32_t g_InGamePendingSimulationTicks;

#endif /* THANDOR_GAMEPLAY_SESSION_STARTUP_H */
