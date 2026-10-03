/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/session/data.h
 */

#ifndef THANDOR_GAMEPLAY_SESSION_DATA_H
#define THANDOR_GAMEPLAY_SESSION_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern TimerRegisterPeriodicProc *g_TimerRegisterPeriodic; /* 00402018 g_TimerRegisterPeriodic */

extern TimerUnregisterPeriodicProc *g_TimerUnregisterPeriodic; /* 0040201C g_TimerUnregisterPeriodic */

extern GraphicsCursorSetFrameProc *g_GraphicsCursorSetFrame; /* 00416848 g_GraphicsCursorSetFrame */

extern uint8_t g_KeyboardSpecialKeyDown[32]; /* 004171F0 g_KeyboardSpecialKeyDown */

extern KeyboardFlushEventsProc *g_KeyboardFlushEvents; /* 00417210 g_KeyboardFlushEvents */

extern SoundReleaseSampleVoiceSetProc *g_SoundReleaseSampleVoiceSet; /* 0041733C g_SoundReleaseSampleVoiceSet */

extern SoundStopAllVoicesProc *g_SoundStopAllVoices; /* 00417354 g_SoundStopAllVoices */

extern GraphicsTextureSourceLifecycleCallbackTable g_GraphicsTextureSourceLifecycleCallbacks3; /* 004A8F40 g_GraphicsTextureSourceLifecycleCallbacks3 */

extern uint16_t u_save_0050daa2[5]; /* 0050DAA2 u_save_0050daa2 */

extern uint16_t g_ScenarioCatalogPathScratchUtf16[256]; /* 0050DAC4 g_ScenarioCatalogPathScratchUtf16 */

extern uint16_t u_flm_ende0000_flm_0050df06[17]; /* 0050DF06 u_flm_ende0000_flm_0050df06 */

extern uint16_t u_flm_ende0001_flm_0050df28[17]; /* 0050DF28 u_flm_ende0001_flm_0050df28 */

extern uint16_t u_sound_level00_sam_0050df6c[18]; /* 0050DF6C u_sound_level00_sam_0050df6c */

extern uint16_t u_sound_music00_sam_0050df90[18]; /* 0050DF90 u_sound_music00_sam_0050df90 */

extern uint16_t u_effect_hex_0050dfc6[11]; /* 0050DFC6 u_effect_hex_0050dfc6 */

extern uint16_t u_shot_hex_0050dfdc[9]; /* 0050DFDC u_shot_hex_0050dfdc */

extern uint16_t u_modul_hex_0050dfee[10]; /* 0050DFEE u_modul_hex_0050dfee */

extern uint16_t u_field_hex_0050e002[10]; /* 0050E002 u_field_hex_0050e002 */

extern uint16_t u_light_hex_0050e016[10]; /* 0050E016 u_light_hex_0050e016 */

extern uint16_t u_widget_hex_0050e02a[11]; /* 0050E02A u_widget_hex_0050e02a */

extern uint16_t u_level_hex_0050e040[10]; /* 0050E040 u_level_hex_0050e040 */

extern uint32_t g_SessionNetworkTickCounter; /* 0050F0D8 g_SessionNetworkTickCounter */

extern uint32_t g_HostCommandBatchSyncSentThisInterval; /* 0050F0DC g_HostCommandBatchSyncSentThisInterval */

extern TerrainRegionCollectionCount g_TerrainRegionCollectionStoredCount; /* 00512D60 g_TerrainRegionCollectionStoredCount */

extern TerrainRegionCollectionCount g_TerrainRegionCollectionVisitedCount; /* 00512D64 g_TerrainRegionCollectionVisitedCount */

extern uint32_t g_TerrainRegionCollectionEntries; /* 00512D68 g_TerrainRegionCollectionEntries */

extern SelectionPlayerRuntimeBlock *g_SelectionPlayerBlocks; /* 00514D60 g_SelectionPlayerBlocks */

extern void *g_InGameFactionScratchBufferSetA8[8]; /* 00514D64 g_InGameFactionScratchBufferSetA8 */

extern void *g_InGameFactionScratchBufferSetB8[8]; /* 00514D84 g_InGameFactionScratchBufferSetB8 */

extern uint32_t g_FactionEnergyAllocationPriorityByModelClass[24]; /* 0051FB18 g_FactionEnergyAllocationPriorityByModelClass: uint32_t[24] energy allocation priority per model runtime class (0 = none, up to 0x12); gameplay/session/runtime.c energy distribution */

extern void **g_InGameLoadedResourcePointers; /* 00530820 g_InGameLoadedResourcePointers */

extern InGameLoadedResourcePointerCount g_InGameLoadedResourcePointerCount; /* 00530824 g_InGameLoadedResourcePointerCount */

extern uint16_t g_InGameLevelSoundLeafOrCombinedPathScratchUtf16[256]; /* 00530828 g_InGameLevelSoundLeafOrCombinedPathScratchUtf16 */

extern uint16_t g_InGameLevelSoundParentDirectoryScratchUtf16[256]; /* 00530A28 g_InGameLevelSoundParentDirectoryScratchUtf16 */

extern uint16_t g_LevelEndingMovieSourcePath[256]; /* 00530E28 g_LevelEndingMovieSourcePath */

extern uint32_t g_InGameLevelTitleTextResourceIndex; /* 00531028 g_InGameLevelTitleTextResourceIndex */

extern uint32_t g_InGameLevelCampaignAssociationIndex; /* 0053102C g_InGameLevelCampaignAssociationIndex */

extern DirectSoundVoiceSet *g_InGameLevelEffectVoiceSets[4]; /* 00531030 g_InGameLevelEffectVoiceSets */

extern uint32_t g_InGameActiveEffectVoice; /* 00531040 g_InGameActiveEffectVoice */

extern uint32_t g_InGameEffectsEnabled; /* 00531044 g_InGameEffectsEnabled */

extern DirectSoundVoiceSet *g_InGameLevelMusicVoiceSets[4]; /* 00531048 g_InGameLevelMusicVoiceSets */

extern uint32_t g_InGameActiveMusicVoice; /* 00531058 g_InGameActiveMusicVoice */

extern uint32_t g_InGameMusicNextTrackCountdown; /* 0053105C g_InGameMusicNextTrackCountdown */

extern InGameLevelRuntimeGlobalBlock20 g_InGameLevelRuntimeGlobalBlock; /* 00531060 g_InGameLevelRuntimeGlobalBlock */

extern uint16_t g_InGameCountdownTextUtf16[8]; /* 00550590 g_InGameCountdownTextUtf16 */

extern InGameUiImage g_InGameRuntimeDefaultImageTemplate; /* 005505A0 g_InGameRuntimeDefaultImageTemplate */

extern uint16_t u_flm_movie000_flm_0056314e[17]; /* 0056314E u_flm_movie000_flm_0056314e: notification movie path, digits at [9] overwritten */

extern int32_t g_InGamePendingSimulationTicks; /* 00563268 g_InGamePendingSimulationTicks */

extern uint32_t g_InGameSessionNotificationTimeoutTicks; /* 0056326C g_InGameSessionNotificationTimeoutTicks */

extern uint32_t g_EndGameResultsCurrentMusicTrackId; /* 00563270 g_EndGameResultsCurrentMusicTrackId */

extern uint32_t g_InGameNetworkTickCountdown; /* 00563274 g_InGameNetworkTickCountdown */

extern uint32_t g_InGameStateTickSpinLock; /* 00563278 g_InGameStateTickSpinLock */

extern uint32_t g_InGameDiagramTextureSource; /* 00563284 g_InGameDiagramTextureSource */

extern uint32_t g_InGameTechnologyTextureSource; /* 00563288 g_InGameTechnologyTextureSource */

extern uint32_t g_InGameWindowTextureSource; /* 0056328C g_InGameWindowTextureSource */

extern uint32_t g_MoviePlaybackBaseFrameGroup; /* 00563320 g_MoviePlaybackBaseFrameGroup */

extern uint32_t g_MoviePlaybackScheduleCounter; /* 00563324 g_MoviePlaybackScheduleCounter */

extern uint32_t g_MoviePlaybackScheduleSpan; /* 00563328 g_MoviePlaybackScheduleSpan */

extern WorldObjectRecord *g_InGameWorldObjectRecords; /* 005636FC g_InGameWorldObjectRecords */

extern uint32_t g_InGameWorldRuntimeDwordArray256[256]; /* 005637AC g_InGameWorldRuntimeDwordArray256 */

extern uint32_t g_EndMovieVariantIndex; /* 00563BB0 g_EndMovieVariantIndex */

extern uint16_t *g_EndMoviePath; /* 00563BB4 g_EndMoviePath */

extern UiCommandDispatchRecord g_EndGameResultsCommandDispatchRecords_00_Code00030071_Modifier30[16]; /* 00567110 g_EndGameResultsCommandDispatchRecords_00_Code00030071_Modifier30: 15 records + the terminator record [15] at 005671C4 (command code 0 ends the dispatch scan; its other fields are the original's NOP fill) */

extern uint8_t g_InGameSessionStartedNetworked; /* 0056A604 g_InGameSessionStartedNetworked (followed by NOP fill up to 0056A610) */

extern FrontendPacket10022StatePending g_FrontendPacket10022Buffer; /* 00572280 g_FrontendPacket10022Buffer */

extern FrontendPacket10023StateAck g_FrontendPacket10023Buffer; /* 005722A0 g_FrontendPacket10023Buffer */

extern uint32_t g_SoundPackageHandle; /* 00572ACC g_SoundPackageHandle */

#endif
