/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/session/level.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_SESSION_LEVEL_H
#define THANDOR_GAMEPLAY_SESSION_LEVEL_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/session/level. */

/* g_InGameLoadedResourcePointers: the level loaders allocate room for 512 loaded EFF/SHT/MDL/ARM file pointers and
   fail with FATAL_ERROR_LEVEL_TOO_MANY_RESOURCES beyond that. */
#define INGAME_LOADED_RESOURCE_CAPACITY 0x200
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00531080 */
EndingMoviePathResult LevelAsset_PrepareEndingMoviePath(uint16_t *currentLevelPath,LevelAssetHeader *asset);

/* 0x005311D0 */
LevelDefaultLoadResult InGameLevelRuntime_LoadResourcesAfterDefaultReset
          (LevelAssetRuntimePrefix *levelImage,WorldRuntimeContext *worldRuntime);

/* 0x00532020 */
LevelLoadResult InGameLevelRuntime_LoadResourcesAfterExternalTables
          (FrontendLoadedLevelAsset *levelImage,WorldRuntimeContext *worldRuntime);

/* 0x005329C0 */
void InGameLevelRuntime_ShutdownLoadedAssetResources(WorldRuntimeContext *worldRuntime);

/* 0x00532CA0 */
StatusResult InGameLevelRuntime_SaveLevelAssetImageFromWorldState(InGameLevelSaveWorldView *saveWorldView);

#endif /* THANDOR_GAMEPLAY_SESSION_LEVEL_H */
