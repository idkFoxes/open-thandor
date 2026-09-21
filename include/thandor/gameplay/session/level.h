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
EndingMoviePathEaxCf5 __thandor_eax_cf_preserve_ecx_edx
LevelAsset_PrepareEndingMoviePathCf(word *currentLevelPath,LevelAssetHeader *asset);

/* 0x005311D0 */
InGameLevelDefaultLoadEaxCf5 __thandor_eax_cf_preserve_ecx_edx
InGameLevelRuntime_LoadResourcesAfterDefaultResetCf
          (LevelAssetRuntimeImagePrefix370 *levelImage,WorldRuntimeContext *worldRuntime);

/* 0x00532020 */
InGameLevelLoadEaxCf5 __thandor_eax_cf_preserve_ecx_edx
InGameLevelRuntime_LoadResourcesAfterExternalTablesCf
          (FrontendLoadedLevelRuntimeImage370 *levelImage,WorldRuntimeContext *worldRuntime);

/* 0x005329C0 */
void __thandor_void_preserve_eax_ecx
InGameLevelRuntime_ShutdownLoadedAssetResources(WorldRuntimeContext *worldRuntime);

/* 0x00532CA0 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
InGameLevelRuntime_SaveLevelAssetImageFromWorldStateCf(InGameLevelSaveWorldView *saveWorldView);

#endif /* THANDOR_GAMEPLAY_SESSION_LEVEL_H */
