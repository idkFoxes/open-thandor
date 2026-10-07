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

void ResourceRegistrationRuntime_RebaseLoadedRecords(ResourceRegistrationRuntimeImage *runtimeImage);

bool SavedLevel_LoadRuntimePools(WorldRuntimeContext *worldRuntime,uint32_t *outError);

void ArmyRuntimePool_RebaseAfterLoad();

void GameFactionRuntime_RebaseLoadedArmyReferences();

#endif /* THANDOR_GAMEPLAY_SESSION_SAVEGAME_LOAD_H */
