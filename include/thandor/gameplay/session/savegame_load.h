/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/session/savegame_load.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_SESSION_SAVEGAME_LOAD_H
#define THANDOR_GAMEPLAY_SESSION_SAVEGAME_LOAD_H

#include <thandor/core/types.h>
#include <thandor/gameplay/session/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/session/savegame_load. */

/* Functions are grouped by semantic ownership. */

void ResourceRegistrationRuntime_RebaseLoadedRecords(ResourceRegistrationRuntimeImage *runtimeImage);

Bool8 SavedLevel_LoadRuntimePools(WorldRuntimeContext *worldRuntime,uint32_t *outError);

void ArmyRuntimePool_RebaseAfterLoad(void);

void GameFactionRuntime_RebaseLoadedArmyReferences(void);

#endif /* THANDOR_GAMEPLAY_SESSION_SAVEGAME_LOAD_H */
