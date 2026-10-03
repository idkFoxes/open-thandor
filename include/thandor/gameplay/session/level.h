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
/* WidePath records of the EFF/SHT/MDL/ARM path tables: 32 UTF-16 code units each */
#define LEVEL_ASSET_PATH_RECORD_UNITS 32
/* g_MoviePlaybackScheduleSpan of a loading stage without a known step count: so many ticks per 8 frames that
   the loading movie stays in the stage's frame group (MoviePlayback_AdvanceScheduledFrameAndTick) */
#define LEVEL_LOAD_MOVIE_SPAN_HOLD 0x10000
/* LevelAsset_PrepareEndingMoviePath: UTF-16 characters 4 and 5 of the movie path select the end movie number */
#define LEVEL_ENDING_MOVIE_NAME_W_UUML UTF16_CHAR_PAIR('w',0xFC) /* "w" + U+00FC, end movie 2 */
#define LEVEL_ENDING_MOVIE_NAME_EI UTF16_CHAR_PAIR('e','i')      /* end movie 3 */
#define LEVEL_ENDING_MOVIE_NAME_LA UTF16_CHAR_PAIR('l','a')      /* end movie 4 */
/* Functions are grouped by semantic ownership. */

bool LevelAsset_PrepareEndingMoviePath
          (uint16_t *currentLevelPath,LevelAssetHeader *asset,uint16_t **outMoviePath,uint32_t *outError);

bool InGameLevelRuntime_LoadResourcesAfterDefaultReset
          (LevelAssetRuntimePrefix *levelImage,WorldRuntimeContext *worldRuntime,uint32_t *outError);

bool InGameLevelRuntime_LoadResourcesAfterExternalTables
          (FrontendLoadedLevelAsset *levelImage,WorldRuntimeContext *worldRuntime,uint32_t *outError);

void InGameLevelRuntime_ShutdownLoadedAssetResources(WorldRuntimeContext *worldRuntime);

bool InGameLevelRuntime_SaveLevelAssetImageFromWorldState(InGameLevelSaveWorldView *saveWorldView,uint32_t *outError);

extern uint16_t u_effect_hex_0050dfc6[11];
extern uint16_t u_shot_hex_0050dfdc[9];
extern uint16_t u_modul_hex_0050dfee[10];
extern uint16_t u_field_hex_0050e002[10];
extern uint16_t u_light_hex_0050e016[10];
extern uint16_t u_widget_hex_0050e02a[11];
extern uint16_t u_level_hex_0050e040[10];
extern uint16_t g_LevelEndingMovieSourcePath[256];
extern uint32_t g_InGameLevelTitleTextResourceIndex;
extern uint32_t g_InGameLevelCampaignAssociationIndex;
extern DirectSoundVoiceSet *g_InGameLevelEffectVoiceSets[4];
extern DirectSoundVoiceSet *g_InGameLevelMusicVoiceSets[4];
extern InGameLevelRuntimeGlobalBlock20 g_InGameLevelRuntimeGlobalBlock;
extern uint32_t g_MoviePlaybackBaseFrameGroup;
extern uint32_t g_MoviePlaybackScheduleCounter;
extern uint32_t g_MoviePlaybackScheduleSpan;
extern uint32_t g_SoundPackageHandle;

extern uint16_t u_army_hex_0050dfb4[9];

#endif /* THANDOR_GAMEPLAY_SESSION_LEVEL_H */
