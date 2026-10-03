/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/data.h
 */

#ifndef THANDOR_UI_FRONTEND_DATA_H
#define THANDOR_UI_FRONTEND_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern CommandLineFindOptionProc *g_CommandLineFindOption; /* 00402024 g_CommandLineFindOption */

extern SpinLockAcquireProc *g_SpinLockAcquire; /* 00402784 g_SpinLockAcquire */

extern SpinLockTryAcquireFlagsProc *g_SpinLockTryAcquire; /* 00402788 g_SpinLockTryAcquire */

extern SpinLockReleaseProc *g_SpinLockRelease; /* 0040278C g_SpinLockRelease */

extern SpinLockReleaseAndInvokeProc *g_SpinLockReleaseAndInvoke; /* 00402790 g_SpinLockReleaseAndInvoke */

extern UiPixelCoordinate g_CursorOverrideX; /* 00416828 g_CursorOverrideX */

extern UiPixelCoordinate g_CursorOverrideY; /* 0041682C g_CursorOverrideY */

extern int32_t g_CursorVisibilityToken; /* 00416834 g_CursorVisibilityToken */

extern uint32_t g_CursorButtonState; /* 00416838 g_CursorButtonState */

extern SoundCreateSampleVoiceSetProc *g_SoundCreateSampleVoiceSet; /* 00417338 g_SoundCreateSampleVoiceSet */

extern uint32_t g_NetworkBackendInstanceCount; /* 0041A544 g_NetworkBackendInstanceCount */

extern NetworkBackendSetSessionCallback *g_NetworkBackendSlot0; /* 0041A54C g_NetworkBackendSlot0 */

extern NetworkBackendCleanupCallback *g_NetworkBackendSlot1; /* 0041A550 g_NetworkBackendSlot1 */

extern NetworkBackendOpenBindCallback *g_NetworkBackendSlot2; /* 0041A554 g_NetworkBackendSlot2 */

extern NetworkBackendCloseCallback *g_NetworkBackendSlot3; /* 0041A558 g_NetworkBackendSlot3 */

extern NetworkBackendParseEndpointCallback *g_NetworkBackendSlot6; /* 0041A564 g_NetworkBackendSlot6 */

extern NetworkBackendFormatAddressCallback *g_NetworkBackendSlot7; /* 0041A568 g_NetworkBackendSlot7 */

extern MovieAudioGainQ15 g_MovieDefaultAudioGainQ15; /* 004A6D9C g_MovieDefaultAudioGainQ15 */

extern MovieAudioGainQ15 g_MovieAlternateAudioGainQ15; /* 004A6DA0 g_MovieAlternateAudioGainQ15 */

extern uint32_t g_FramebufferWidth; /* 004A8E88 g_FramebufferWidth */

extern UQ12 g_WorldMotionTargetDistanceConvergenceStepQ12; /* 0050BADE g_WorldMotionTargetDistanceConvergenceStepQ12 */

extern int g_WorldMotionPointerWheelInputScale; /* 0050BAF6 g_WorldMotionPointerWheelInputScale */

extern UiNodeVtable g_FrontendModelPointerContextVtable; /* 0050BB37 g_FrontendModelPointerContextVtable */

extern uint16_t u_flm_ende0000_flm_0050df4a[17]; /* 0050DF4A u_flm_ende0000_flm_0050df4a */

extern int32_t g_FrontendPlayerRuntimeCount; /* 0050F050 g_FrontendPlayerRuntimeCount */

extern uint16_t g_FrontendLocalPlayerNameUtf16[20]; /* 0050F054 g_FrontendLocalPlayerNameUtf16 */

extern FrontendPlayerRuntimeRecord *g_FrontendPlayerRuntimeBlocks; /* 0050F0C0 g_FrontendPlayerRuntimeBlocks */

extern FrontendPlayerRuntimeBlockCount g_FrontendPlayerRuntimeBlockCount; /* 0050F0C4 g_FrontendPlayerRuntimeBlockCount */

extern SessionNetworkRoleFlags g_SessionNetworkRoleFlags; /* 0050F0C8 g_SessionNetworkRoleFlags */

extern uint32_t g_SessionNetworkTickInterval; /* 0050F0D4 g_SessionNetworkTickInterval: uint32_t network lockstep interval in simulation steps (2 * the frontend speed slider value); sent in the join ack */

extern uint16_t g_FrontendPlayerMessageScratchUtf16[48]; /* 00514DA4 g_FrontendPlayerMessageScratchUtf16 */

extern UiCommandPayloadTextBatch48 g_UiSevenSlotCommandPayloadText; /* 00514E04 g_UiSevenSlotCommandPayloadText */

extern uint16_t g_FrontendResultsValueTextUtf16[32]; /* 00516EA0 g_FrontendResultsValueTextUtf16 */

extern uint16_t g_EndGameElapsedTimeScratchUtf16[64]; /* 00516EE0 g_EndGameElapsedTimeScratchUtf16 */

extern UiNodeVtable g_UiNodeVtable_00516F60; /* 00516F60 g_UiNodeVtable_00516F60 */

extern int g_FrontendResultsColumnAdvance00Pixels; /* 00516FA8 g_FrontendResultsColumnAdvance00Pixels */

extern int g_FrontendResultsColumnAdvance01Pixels; /* 00516FAC g_FrontendResultsColumnAdvance01Pixels */

extern int g_FrontendResultsColumnAdvanceColourPixels; /* 00516FB0 g_FrontendResultsColumnAdvanceColourPixels */

extern int g_FrontendResultsColumnAdvanceEconomyPixels; /* 00516FB4 g_FrontendResultsColumnAdvanceEconomyPixels */

extern int g_FrontendResultsColumnAdvanceMilitaryPixels; /* 00516FB8 g_FrontendResultsColumnAdvanceMilitaryPixels */

extern int g_FrontendResultsColumnAdvancePointsPixels; /* 00516FBC g_FrontendResultsColumnAdvancePointsPixels */

extern int g_FrontendResultsColumnAdvancePlayerPixels; /* 00516FC0 g_FrontendResultsColumnAdvancePlayerPixels */

extern int g_FrontendResultsColumnAdvanceFactionPixels; /* 00516FC4 g_FrontendResultsColumnAdvanceFactionPixels */

extern int g_FrontendResultsColumnAdvanceFactionField98Pixels; /* 00516FC8 g_FrontendResultsColumnAdvanceFactionField98Pixels */

extern int g_FrontendResultsColumnAdvanceFactionField9CPixels; /* 00516FCC g_FrontendResultsColumnAdvanceFactionField9CPixels */

extern int g_FrontendResultsColumnAdvanceFactionFieldA0Pixels; /* 00516FD0 g_FrontendResultsColumnAdvanceFactionFieldA0Pixels */

extern int g_FrontendResultsColumnAdvanceFactionFieldA4Pixels; /* 00516FD4 g_FrontendResultsColumnAdvanceFactionFieldA4Pixels */

extern int g_FrontendResultsColumnAdvanceFactionFieldA8Pixels; /* 00516FD8 g_FrontendResultsColumnAdvanceFactionFieldA8Pixels */

extern int g_FrontendResultsColumnAdvanceFactionFieldACPixels; /* 00516FDC g_FrontendResultsColumnAdvanceFactionFieldACPixels */

extern int g_FrontendResultsColumnAdvanceFactionFieldB0Pixels; /* 00516FE0 g_FrontendResultsColumnAdvanceFactionFieldB0Pixels */

extern int g_FrontendResultsColumnAdvanceFactionFieldB4Pixels; /* 00516FE4 g_FrontendResultsColumnAdvanceFactionFieldB4Pixels */

extern int g_FrontendResultsColumnAdvanceFactionFieldB8Pixels; /* 00516FE8 g_FrontendResultsColumnAdvanceFactionFieldB8Pixels */

extern int g_FrontendResultsColumnAdvanceFactionFieldBCPixels; /* 00516FEC g_FrontendResultsColumnAdvanceFactionFieldBCPixels */

extern uint32_t g_FrontendResultsFramebufferBytesPerPixel; /* 00516FF0 g_FrontendResultsFramebufferBytesPerPixel */

extern uint32_t g_FrontendResultsFramebufferScanlineStrideBytes; /* 00516FF4 g_FrontendResultsFramebufferScanlineStrideBytes */

extern uint32_t g_FrontendResultsFactionPackedPixelColors[7]; /* 00516FF8 g_FrontendResultsFactionPackedPixelColors */

extern void *g_ArmyRuntimeRebaseBaseMinusOne; /* 00519734 g_ArmyRuntimeRebaseBaseMinusOne */

extern RuntimeModelClassPriorityTable24 g_RuntimeModelClassPriorityByModelClassId; /* 0051FBD8 g_RuntimeModelClassPriorityByModelClassId */

extern UiRootCallbacks g_UiRootCallbacks_0053DA70; /* 0053DA70 g_UiRootCallbacks_0053DA70 */

extern FrontendSessionDiscoveryRecord **g_FrontendSessionListRows; /* 0053DA84 g_FrontendSessionListRows */

extern FrontendSessionDiscoveryRecord *g_FrontendSessionDiscoveryRecords; /* 0053DA88 g_FrontendSessionDiscoveryRecords */

extern FrontendUiImage g_FrontendRootInitializationTemplate; /* 0053DA8C g_FrontendRootInitializationTemplate */

extern FrontendPlayerRuntimeRecord *g_FrontendPlayerRuntimeRecordPointers32[32]; /* 005433E0 g_FrontendPlayerRuntimeRecordPointers32 */

extern FrontendTaskAssignmentControlOffsetTables g_FrontendTaskAssignmentControlOffsets; /* 00543460 g_FrontendTaskAssignmentControlOffsets */

extern uint16_t *g_FrontendNetworkBackendNameRows[256]; /* 005434EC g_FrontendNetworkBackendNameRows: row pointer table of the frontend network backend list (display names), filled when the list is built */

extern FrontendUiScratch g_FrontendUiDisplayModeAndTaskAssignmentScratch; /* 005438EC g_FrontendUiDisplayModeAndTaskAssignmentScratch: followed by 4 bytes 0x90 fill (dropped) */

extern uint32_t g_FrontendRootNode; /* 005456F0 g_FrontendRootNode */

extern uint32_t g_FrontendPendingPageAction; /* 005456F4 g_FrontendPendingPageAction */

extern uint32_t g_FrontendRuntimeFlags; /* 00545700 g_FrontendRuntimeFlags */

extern uint32_t g_FrontendCentralTextureSet; /* 00545704 g_FrontendCentralTextureSet */

extern uint32_t g_FrontendCentralPaletteAsset; /* 00545708 g_FrontendCentralPaletteAsset */

extern GraphicsTextureSourceAsset *g_FrontendMenuTextureSource; /* 0054570C g_FrontendMenuTextureSource */

extern uint32_t g_FrontendNetworkTickCounter; /* 00545710 g_FrontendNetworkTickCounter */

extern uint32_t g_FrontendNetworkState; /* 00545714 g_FrontendNetworkState */

extern uint32_t g_FrontendFactionAssignmentReadyStateGeneration; /* 00545718 g_FrontendFactionAssignmentReadyStateGeneration */

extern uint32_t g_FrontendStateTickSpinLock; /* 0054572C g_FrontendStateTickSpinLock */

extern uint32_t g_FrontendTimerCountdownTicks; /* 00545730 g_FrontendTimerCountdownTicks */

extern WorldMotionSplineKeyframe g_FrontendRomTransitionKeyframes[2]; /* 00545734 g_FrontendRomTransitionKeyframes: two spline keyframes (0 = current camera, 1 = target record's camera), passed as an array to WorldMotionSpline_BuildSixChannelCurves */

extern uint32_t g_FrontendCentralRomAsset; /* 00545774 g_FrontendCentralRomAsset */

extern WorldObjectRecord *g_FrontendWorldObjectRecords; /* 0054577C g_FrontendWorldObjectRecords */

extern uint32_t g_FrontendLoadedCampaignAsset; /* 00545914 g_FrontendLoadedCampaignAsset */

extern uint32_t g_FrontendScenarioInitializationCount; /* 00545928 g_FrontendScenarioInitializationCount */

extern uint32_t g_FrontendMusicVoiceSet; /* 0054592C g_FrontendMusicVoiceSet */

extern uint32_t g_FrontendMusicActiveBuffer; /* 00545930 g_FrontendMusicActiveBuffer */

extern uint32_t g_FrontendPendingPageActionDepth; /* 00545934 g_FrontendPendingPageActionDepth */

extern FrontendUiActionHandlerPage20Prefix g_FrontendUiActionHandlersPage20; /* 00545938 g_FrontendUiActionHandlersPage20 */

extern uint16_t u_gfx_texturen_zentrale_gfx_00545acc[26]; /* 00545ACC u_gfx_texturen_zentrale_gfx_00545acc */

extern uint16_t u_gfx_texturen_zentrale_pal_00545b00[26]; /* 00545B00 u_gfx_texturen_zentrale_pal_00545b00 */

extern uint16_t u_sound_menue01_sam_00545b54[18]; /* 00545B54 u_sound_menue01_sam_00545b54 */

extern uint16_t u_gfx_panel_menue_gfx_00545b78[20]; /* 00545B78 u_gfx_panel_menue_gfx_00545b78 */

extern uint16_t g_FrontendMissionBriefingMoviePathUtf16[16]; /* 00545C02 g_FrontendMissionBriefingMoviePathUtf16: L"flm\\lev0000.flm", the mission briefing movie path; the four digits at index 7 (00545C10) are overwritten with the level number; opened with Movie_Open (ui/frontend/scenario.c) */

extern uint16_t u_sound_music00_sam_00545c4e[18]; /* 00545C4E u_sound_music00_sam_00545c4e */

extern char s_SPIELER__SPIEL__NETZWERK__HOST_00545e72[31]; /* 00545E72 s_SPIELER__SPIEL__NETZWERK__HOST_00545e72 */

extern char s_NAME__CLIENT__KARTE___00545e91[21]; /* 00545E91 s_NAME__CLIENT__KARTE___00545e91: the option names NAME=" CLIENT=" KARTE=" (used with explicit lengths); Original quirk: its terminating NUL is the first byte of g_LevelPackageFoundEntry */

extern UiCommandDispatchRecord g_FrontendCommandDispatchRecords_00_Code00030071_Modifier30[4]; /* 00548100 g_FrontendCommandDispatchRecords_00_Code00030071_Modifier30: 3 records + the terminator record [3] at 00548124 (command code 0 ends the dispatch scan; its other fields are the original's NOP fill) */

extern FrontendPlayerRemovalPacket10007 g_FrontendPlayerRemovalPacket10007; /* 0054DB00 g_FrontendPlayerRemovalPacket10007 */

extern UiTransferEndpointDescriptor g_FrontendNetworkEndpointScratch; /* 0054DDD0 g_FrontendNetworkEndpointScratch */

extern uint16_t g_FrontendNetworkRuntimeCountTextUtf16[4]; /* 0054DDE0 g_FrontendNetworkRuntimeCountTextUtf16: decimal number of lobby players (payload 0 of the session player count text) */

extern uint16_t g_FrontendNetworkPlayerCountTextUtf16[4]; /* 0054DDE8 g_FrontendNetworkPlayerCountTextUtf16: decimal maximum player count (payload 1 of the session player count text, bound to a template text control) */

extern uint16_t g_FrontendNetworkSpeedLabelUtf16[32]; /* 0054DDF0 g_FrontendNetworkSpeedLabelUtf16: network speed caption, written with a capacity of 64 bytes */

extern uint16_t g_FrontendNetworkEndpointTextUtf16[512]; /* 0054DE30 g_FrontendNetworkEndpointTextUtf16: local address text from g_NetworkBackendSlot7 */

extern uint16_t g_FrontendCurrentFactionPrimaryResourceTextUtf16[16]; /* 005504DC g_FrontendCurrentFactionPrimaryResourceTextUtf16: decimal xenite amount, bound to a template text control */

extern uint32_t g_DebugOverlayCounterRefreshCountdown; /* 00563330 g_DebugOverlayCounterRefreshCountdown: uint32_t: frames until the debug overlay counters refresh (reloaded with 20); ui/ingame and ui/frontend runtime */

extern uint32_t g_EndMovieSelectionIndex; /* 00563BAC g_EndMovieSelectionIndex */

extern uint32_t g_EndMoviePendingTicks; /* 00563BB8 g_EndMoviePendingTicks */

extern FrontendPlayerRemovalPacket10007 g_FrontendClientPlayerRemovalPacket10007; /* 00572040 g_FrontendClientPlayerRemovalPacket10007 */

#endif
