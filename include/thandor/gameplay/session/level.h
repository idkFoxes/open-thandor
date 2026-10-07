/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/session/level.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_SESSION_LEVEL_H
#define THANDOR_GAMEPLAY_SESSION_LEVEL_H

#include <thandor/core/types.h>
#include <thandor/gameplay/session/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>
#include <thandor/core/text/path.h> /* UTF16_CHAR_PAIR */

/* LevelAsset_PrepareEndingMoviePath: UTF-16 characters 4 and 5 of the movie path select the end movie number */
inline constexpr auto LEVEL_ENDING_MOVIE_NAME_W_UUML = UTF16_CHAR_PAIR('w',0xFC); /* "w" + U+00FC, end movie 2 */
inline constexpr auto LEVEL_ENDING_MOVIE_NAME_EI = UTF16_CHAR_PAIR('e','i'); /* end movie 3 */
inline constexpr auto LEVEL_ENDING_MOVIE_NAME_LA = UTF16_CHAR_PAIR('l','a'); /* end movie 4 */

bool LevelAsset_PrepareEndingMoviePath
          (uint16_t *currentLevelPath,LevelAssetHeader *asset,uint16_t **outMoviePath,uint32_t *outError);

void InGameLevelRuntime_ShutdownLoadedAssetResources(WorldRuntimeContext *worldRuntime);

bool InGameLevelRuntime_SaveLevelAssetImageFromWorldState(InGameLevelSaveWorldView *saveWorldView,uint32_t *outError);

extern uint16_t g_FieldHexPathUtf16[10];

extern uint16_t g_LevelHexPathUtf16[10];
extern uint16_t g_LevelEndingMovieSourcePath[256];

bool LevelPackage_ValidateAndMount(uint16_t *levelPathUtf16);

#endif /* THANDOR_GAMEPLAY_SESSION_LEVEL_H */
