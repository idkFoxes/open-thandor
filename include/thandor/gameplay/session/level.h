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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00531080 */
EndingMoviePathResult LevelAsset_PrepareEndingMoviePath(uint16_t *currentLevelPath,LevelAssetHeader *asset);

/* 0x005311D0 */
LevelDefaultLoadResult InGameLevelRuntime_LoadResourcesAfterDefaultReset
          (LevelAssetRuntimeImagePrefix370 *levelImage,WorldRuntimeContext *worldRuntime);

/* 0x00532020 */
LevelLoadResult InGameLevelRuntime_LoadResourcesAfterExternalTables
          (FrontendLoadedLevelRuntimeImage370 *levelImage,WorldRuntimeContext *worldRuntime);

/* 0x005329C0 */
void InGameLevelRuntime_ShutdownLoadedAssetResources(WorldRuntimeContext *worldRuntime);

/* 0x00532CA0 */
StatusResult InGameLevelRuntime_SaveLevelAssetImageFromWorldState(InGameLevelSaveWorldView *saveWorldView);

#endif /* THANDOR_GAMEPLAY_SESSION_LEVEL_H */
