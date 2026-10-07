/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/session/level_new.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_SESSION_LEVEL_NEW_H
#define THANDOR_GAMEPLAY_SESSION_LEVEL_NEW_H

#include <thandor/assets/package/types.h>
#include <thandor/core/text/types.h>
#include <thandor/core/types.h>
#include <thandor/gameplay/session/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

/* g_InGameLoadedResourcePointers: the level loaders allocate room for 512 loaded EFF/SHT/MDL/ARM file pointers and
   fail with FATAL_ERROR_LEVEL_TOO_MANY_RESOURCES beyond that. The pointers are kept as Ptr32 (4 bytes each, like the
   original's 32-bit pointers), so the original's 512 * 4-byte arena block holds all of them on x64 too. */
inline constexpr int INGAME_LOADED_RESOURCE_CAPACITY = 0x200;
/* WidePath records of the EFF/SHT/MDL/ARM path tables: 32 UTF-16 code units each */
inline constexpr int LEVEL_ASSET_PATH_RECORD_UNITS = 32;
/* g_MoviePlaybackScheduleSpan of a loading stage without a known step count: so many ticks per 8 frames that
   the loading movie stays in the stage's frame group (MoviePlayback_AdvanceScheduledFrameAndTick) */
inline constexpr int LEVEL_LOAD_MOVIE_SPAN_HOLD = 0x10000;

/* Prepares one loaded file of a LEV file list (assetByteCount: the loaded byte count); false with the step's
   error code in *outError. */
using NewLevelPrepareAssetFn = bool (*)(void *asset,uint32_t assetByteCount,uint32_t *outError);

Bool8 InGameLevelRuntime_LoadResourcesAfterDefaultReset
          (LevelAssetRuntimePrefix *levelImage,WorldRuntimeContext *worldRuntime,uint32_t *outError);

Bool8 NewLevel_Fail(uint32_t *outError,uint32_t error);

Bool8 NewLevel_CopyRuntimePrefix(LevelAssetRuntimePrefix *levelImage,uint32_t *outError);

Bool8 NewLevel_LoadTechnology(LevelAssetRuntimePrefix *levelImage,uint32_t *outError);

bool NewLevel_PrepareEffectAsset(void *asset,uint32_t assetByteCount,uint32_t *outError);

bool NewLevel_PrepareShotAsset(void *asset,uint32_t assetByteCount,uint32_t *outError);

bool NewLevel_PrepareModelAsset(void *asset,uint32_t assetByteCount,uint32_t *outError);

bool NewLevel_PrepareArmyAsset(void *asset,uint32_t assetByteCount,uint32_t *outError);

Bool8 NewLevel_LoadAssetList
          (LevelAssetRuntimePrefix *levelImage,LevelAssetRelativeByteOffset pathTableOffset,
           LevelAssetRecordCount remainingRecordCount,PackedFileExtensionCode32 extensionCode,
           NewLevelPrepareAssetFn prepareAsset,Ptr32<void> **loadedResourceCursor,uint32_t *outError);

void NewLevel_LoadLevelSamples();

extern uint32_t g_InGameLevelTitleTextResourceIndex;
extern uint32_t g_InGameLevelCampaignAssociationIndex;
extern SoundVoiceSet *g_InGameLevelEffectVoiceSets[4];
extern SoundVoiceSet *g_InGameLevelMusicVoiceSets[4];
extern InGameLevelRuntimeGlobalBlock20 g_InGameLevelRuntimeGlobalBlock;
extern uint32_t g_MoviePlaybackBaseFrameGroup;
extern uint32_t g_MoviePlaybackScheduleCounter;
extern uint32_t g_MoviePlaybackScheduleSpan;
extern EngineFileHandle g_SoundPackageHandle;
extern Ptr32<void> *g_InGameLoadedResourcePointers;
extern InGameLoadedResourcePointerCount g_InGameLoadedResourcePointerCount;
extern uint16_t g_InGameLevelSoundLeafOrCombinedPathScratchUtf16[256];
extern uint16_t g_InGameLevelSoundParentDirectoryScratchUtf16[256];

#endif /* THANDOR_GAMEPLAY_SESSION_LEVEL_NEW_H */
