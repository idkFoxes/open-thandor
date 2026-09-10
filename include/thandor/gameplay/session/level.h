#ifndef THANDOR_GAMEPLAY_SESSION_LEVEL_H
#define THANDOR_GAMEPLAY_SESSION_LEVEL_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/session/level. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00531080 */
undefined8 LevelAsset_PrepareEndingMoviePathCf(word *currentLevelPath,LevelAssetHeader *asset);

/* 0x005311D0 */
undefined8 __fastcall InGameLevelRuntime_LoadResourcesAfterDefaultResetCf (undefined4 param_1,undefined4 param_2,LevelAssetRuntimeImagePrefix370 *levelImage, WorldRuntimeContext *worldRuntime);

/* 0x00532020 */
undefined8 __fastcall InGameLevelRuntime_LoadResourcesAfterExternalTablesCf (undefined4 param_1,undefined4 param_2,LevelAssetRuntimeImagePrefix370 *levelImage, WorldRuntimeContext *worldRuntime);

/* 0x005329C0 */
void __thiscall InGameLevelRuntime_ShutdownLoadedAssetResources(void *this,LevelAssetHeader *levelAsset);

/* 0x00532CA0 */
void InGameLevelRuntime_SaveLevelAssetImageFromWorldStateCf(int param_1);

#endif /* THANDOR_GAMEPLAY_SESSION_LEVEL_H */
