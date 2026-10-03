/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/session/data.h
 */

#ifndef THANDOR_GAMEPLAY_SESSION_DATA_H
#define THANDOR_GAMEPLAY_SESSION_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern TimerRegisterPeriodicProc *g_TimerRegisterPeriodic;

extern TimerUnregisterPeriodicProc *g_TimerUnregisterPeriodic;

extern GraphicsCursorSetFrameProc *g_GraphicsCursorSetFrame;

extern uint8_t g_KeyboardSpecialKeyDown[32];

extern KeyboardFlushEventsProc *g_KeyboardFlushEvents;

extern SoundReleaseSampleVoiceSetProc *g_SoundReleaseSampleVoiceSet;

extern SoundStopAllVoicesProc *g_SoundStopAllVoices;

extern GraphicsTextureSourceLifecycleCallbackTable g_GraphicsTextureSourceLifecycleCallbacks3;

extern uint16_t u_save_0050daa2[5];

extern uint16_t g_ScenarioCatalogPathScratchUtf16[256];

extern uint16_t u_flm_ende0000_flm_0050df06[17];

extern uint16_t u_flm_ende0001_flm_0050df28[17];

extern uint16_t u_sound_level00_sam_0050df6c[18];

extern uint16_t u_sound_music00_sam_0050df90[18];

extern uint16_t u_effect_hex_0050dfc6[11];

extern uint16_t u_shot_hex_0050dfdc[9];

extern uint16_t u_modul_hex_0050dfee[10];

extern uint16_t u_field_hex_0050e002[10];

extern uint16_t u_light_hex_0050e016[10];

extern uint16_t u_widget_hex_0050e02a[11];

extern uint16_t u_level_hex_0050e040[10];

extern uint32_t g_SessionNetworkTickCounter;

extern uint32_t g_HostCommandBatchSyncSentThisInterval;

extern TerrainRegionCollectionCount g_TerrainRegionCollectionStoredCount;

extern TerrainRegionCollectionCount g_TerrainRegionCollectionVisitedCount;

extern uint32_t g_TerrainRegionCollectionEntries;

extern SelectionPlayerRuntimeBlock *g_SelectionPlayerBlocks;

extern void *g_InGameFactionScratchBufferSetA8[8];

extern void *g_InGameFactionScratchBufferSetB8[8];

extern uint32_t g_FactionEnergyAllocationPriorityByModelClass[24]; /* uint32_t[24] energy allocation priority per model runtime class (0 = none, up to 0x12); gameplay/session/runtime.c energy distribution */

extern void **g_InGameLoadedResourcePointers;

extern InGameLoadedResourcePointerCount g_InGameLoadedResourcePointerCount;

extern uint16_t g_InGameLevelSoundLeafOrCombinedPathScratchUtf16[256];

extern uint16_t g_InGameLevelSoundParentDirectoryScratchUtf16[256];

extern uint16_t g_LevelEndingMovieSourcePath[256];

extern uint32_t g_InGameLevelTitleTextResourceIndex;

extern uint32_t g_InGameLevelCampaignAssociationIndex;

extern DirectSoundVoiceSet *g_InGameLevelEffectVoiceSets[4];

extern uint32_t g_InGameActiveEffectVoice;

extern uint32_t g_InGameEffectsEnabled;

extern DirectSoundVoiceSet *g_InGameLevelMusicVoiceSets[4];

extern uint32_t g_InGameActiveMusicVoice;

extern uint32_t g_InGameMusicNextTrackCountdown;

extern InGameLevelRuntimeGlobalBlock20 g_InGameLevelRuntimeGlobalBlock;

extern uint16_t g_InGameCountdownTextUtf16[8];

extern InGameUiImage g_InGameRuntimeDefaultImageTemplate;

extern uint16_t u_flm_movie000_flm_0056314e[17]; /* notification movie path, digits at [9] overwritten */

extern int32_t g_InGamePendingSimulationTicks;

extern uint32_t g_InGameSessionNotificationTimeoutTicks;

extern uint32_t g_EndGameResultsCurrentMusicTrackId;

extern uint32_t g_InGameNetworkTickCountdown;

extern uint32_t g_InGameStateTickSpinLock;

extern uint32_t g_InGameDiagramTextureSource;

extern uint32_t g_InGameTechnologyTextureSource;

extern uint32_t g_InGameWindowTextureSource;

extern uint32_t g_MoviePlaybackBaseFrameGroup;

extern uint32_t g_MoviePlaybackScheduleCounter;

extern uint32_t g_MoviePlaybackScheduleSpan;

extern WorldObjectRecord *g_InGameWorldObjectRecords;

extern uint32_t g_InGameWorldRuntimeDwordArray256[256];

extern uint32_t g_EndMovieVariantIndex;

extern uint16_t *g_EndMoviePath;

extern UiCommandDispatchRecord g_EndGameResultsCommandDispatchRecords_00_Code00030071_Modifier30[16]; /* 15 records + the terminator record [15] at 005671C4 (command code 0 ends the dispatch scan; its other fields are the original's NOP fill) */

extern uint8_t g_InGameSessionStartedNetworked; /* (followed by NOP fill up to 0056A610) */

extern FrontendPacket10022StatePending g_FrontendPacket10022Buffer;

extern FrontendPacket10023StateAck g_FrontendPacket10023Buffer;

extern uint32_t g_SoundPackageHandle;

#endif
