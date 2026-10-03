/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/data.h
 */

#ifndef THANDOR_UI_FRONTEND_DATA_H
#define THANDOR_UI_FRONTEND_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern CommandLineFindOptionProc *g_CommandLineFindOption;

extern SpinLockAcquireProc *g_SpinLockAcquire;

extern SpinLockTryAcquireFlagsProc *g_SpinLockTryAcquire;

extern SpinLockReleaseProc *g_SpinLockRelease;

extern SpinLockReleaseAndInvokeProc *g_SpinLockReleaseAndInvoke;

extern UiPixelCoordinate g_CursorOverrideX;

extern UiPixelCoordinate g_CursorOverrideY;

extern int32_t g_CursorVisibilityToken;

extern uint32_t g_CursorButtonState;

extern SoundCreateSampleVoiceSetProc *g_SoundCreateSampleVoiceSet;

extern uint32_t g_NetworkBackendInstanceCount;

extern NetworkBackendSetSessionCallback *g_NetworkBackendSlot0;

extern NetworkBackendCleanupCallback *g_NetworkBackendSlot1;

extern NetworkBackendOpenBindCallback *g_NetworkBackendSlot2;

extern NetworkBackendCloseCallback *g_NetworkBackendSlot3;

extern NetworkBackendParseEndpointCallback *g_NetworkBackendSlot6;

extern NetworkBackendFormatAddressCallback *g_NetworkBackendSlot7;

extern MovieAudioGainQ15 g_MovieDefaultAudioGainQ15;

extern MovieAudioGainQ15 g_MovieAlternateAudioGainQ15;

extern uint32_t g_FramebufferWidth;

extern UQ12 g_WorldMotionTargetDistanceConvergenceStepQ12;

extern int g_WorldMotionPointerWheelInputScale;

extern UiNodeVtable g_FrontendModelPointerContextVtable;

extern uint16_t u_flm_ende0000_flm_0050df4a[17];

extern int32_t g_FrontendPlayerRuntimeCount;

extern uint16_t g_FrontendLocalPlayerNameUtf16[20];

extern FrontendPlayerRuntimeRecord *g_FrontendPlayerRuntimeBlocks;

extern FrontendPlayerRuntimeBlockCount g_FrontendPlayerRuntimeBlockCount;

extern SessionNetworkRoleFlags g_SessionNetworkRoleFlags;

extern uint32_t g_SessionNetworkTickInterval; /* uint32_t network lockstep interval in simulation steps (2 * the frontend speed slider value); sent in the join ack */

extern uint16_t g_FrontendPlayerMessageScratchUtf16[48];

extern UiCommandPayloadTextBatch48 g_UiSevenSlotCommandPayloadText;

extern uint16_t g_FrontendResultsValueTextUtf16[32];

extern uint16_t g_EndGameElapsedTimeScratchUtf16[64];

extern UiNodeVtable g_UiNodeVtable_00516F60;

extern int g_FrontendResultsColumnAdvance00Pixels;

extern int g_FrontendResultsColumnAdvance01Pixels;

extern int g_FrontendResultsColumnAdvanceColourPixels;

extern int g_FrontendResultsColumnAdvanceEconomyPixels;

extern int g_FrontendResultsColumnAdvanceMilitaryPixels;

extern int g_FrontendResultsColumnAdvancePointsPixels;

extern int g_FrontendResultsColumnAdvancePlayerPixels;

extern int g_FrontendResultsColumnAdvanceFactionPixels;

extern int g_FrontendResultsColumnAdvanceFactionField98Pixels;

extern int g_FrontendResultsColumnAdvanceFactionField9CPixels;

extern int g_FrontendResultsColumnAdvanceFactionFieldA0Pixels;

extern int g_FrontendResultsColumnAdvanceFactionFieldA4Pixels;

extern int g_FrontendResultsColumnAdvanceFactionFieldA8Pixels;

extern int g_FrontendResultsColumnAdvanceFactionFieldACPixels;

extern int g_FrontendResultsColumnAdvanceFactionFieldB0Pixels;

extern int g_FrontendResultsColumnAdvanceFactionFieldB4Pixels;

extern int g_FrontendResultsColumnAdvanceFactionFieldB8Pixels;

extern int g_FrontendResultsColumnAdvanceFactionFieldBCPixels;

extern uint32_t g_FrontendResultsFramebufferBytesPerPixel;

extern uint32_t g_FrontendResultsFramebufferScanlineStrideBytes;

extern uint32_t g_FrontendResultsFactionPackedPixelColors[7];

extern void *g_ArmyRuntimeRebaseBaseMinusOne;

extern RuntimeModelClassPriorityTable24 g_RuntimeModelClassPriorityByModelClassId;

extern UiRootCallbacks g_UiRootCallbacks_0053DA70;

extern FrontendSessionDiscoveryRecord **g_FrontendSessionListRows;

extern FrontendSessionDiscoveryRecord *g_FrontendSessionDiscoveryRecords;

extern FrontendUiImage g_FrontendRootInitializationTemplate;

extern FrontendPlayerRuntimeRecord *g_FrontendPlayerRuntimeRecordPointers32[32];

extern FrontendTaskAssignmentControlOffsetTables g_FrontendTaskAssignmentControlOffsets;

extern uint16_t *g_FrontendNetworkBackendNameRows[256]; /* row pointer table of the frontend network backend list (display names), filled when the list is built */

extern FrontendUiScratch g_FrontendUiDisplayModeAndTaskAssignmentScratch; /* followed by 4 bytes 0x90 fill (dropped) */

extern uint32_t g_FrontendRootNode;

extern uint32_t g_FrontendPendingPageAction;

extern uint32_t g_FrontendRuntimeFlags;

extern uint32_t g_FrontendCentralTextureSet;

extern uint32_t g_FrontendCentralPaletteAsset;

extern GraphicsTextureSourceAsset *g_FrontendMenuTextureSource;

extern uint32_t g_FrontendNetworkTickCounter;

extern uint32_t g_FrontendNetworkState;

extern uint32_t g_FrontendFactionAssignmentReadyStateGeneration;

extern uint32_t g_FrontendStateTickSpinLock;

extern uint32_t g_FrontendTimerCountdownTicks;

extern WorldMotionSplineKeyframe g_FrontendRomTransitionKeyframes[2]; /* two spline keyframes (0 = current camera, 1 = target record's camera), passed as an array to WorldMotionSpline_BuildSixChannelCurves */

extern uint32_t g_FrontendCentralRomAsset;

extern WorldObjectRecord *g_FrontendWorldObjectRecords;

extern uint32_t g_FrontendLoadedCampaignAsset;

extern uint32_t g_FrontendScenarioInitializationCount;

extern uint32_t g_FrontendMusicVoiceSet;

extern uint32_t g_FrontendMusicActiveBuffer;

extern uint32_t g_FrontendPendingPageActionDepth;

extern FrontendUiActionHandlerPage20Prefix g_FrontendUiActionHandlersPage20;

extern uint16_t u_gfx_texturen_zentrale_gfx_00545acc[26];

extern uint16_t u_gfx_texturen_zentrale_pal_00545b00[26];

extern uint16_t u_sound_menue01_sam_00545b54[18];

extern uint16_t u_gfx_panel_menue_gfx_00545b78[20];

extern uint16_t g_FrontendMissionBriefingMoviePathUtf16[16]; /* L"flm\\lev0000.flm", the mission briefing movie path; the four digits at index 7 are overwritten with the level number; opened with Movie_Open (ui/frontend/scenario.c) */

extern uint16_t u_sound_music00_sam_00545c4e[18];

extern char s_SPIELER__SPIEL__NETZWERK__HOST_00545e72[31];

extern char s_NAME__CLIENT__KARTE___00545e91[21]; /* the option names NAME=" CLIENT=" KARTE=" (used with explicit lengths); Original quirk: its terminating NUL is the first byte of g_LevelPackageFoundEntry */

extern UiCommandDispatchRecord g_FrontendCommandDispatchRecords_00_Code00030071_Modifier30[4]; /* 3 records + the terminator record [3] (command code 0 ends the dispatch scan; its other fields are the original's NOP fill) */

extern FrontendPlayerRemovalPacket10007 g_FrontendPlayerRemovalPacket10007;

extern UiTransferEndpointDescriptor g_FrontendNetworkEndpointScratch;

extern uint16_t g_FrontendNetworkRuntimeCountTextUtf16[4]; /* decimal number of lobby players (payload 0 of the session player count text) */

extern uint16_t g_FrontendNetworkPlayerCountTextUtf16[4]; /* decimal maximum player count (payload 1 of the session player count text, bound to a template text control) */

extern uint16_t g_FrontendNetworkSpeedLabelUtf16[32]; /* network speed caption, written with a capacity of 64 bytes */

extern uint16_t g_FrontendNetworkEndpointTextUtf16[512]; /* local address text from g_NetworkBackendSlot7 */

extern uint16_t g_FrontendCurrentFactionPrimaryResourceTextUtf16[16]; /* decimal xenite amount, bound to a template text control */

extern uint32_t g_DebugOverlayCounterRefreshCountdown; /* uint32_t: frames until the debug overlay counters refresh (reloaded with 20); ui/ingame and ui/frontend runtime */

extern uint32_t g_EndMovieSelectionIndex;

extern uint32_t g_EndMoviePendingTicks;

extern FrontendPlayerRemovalPacket10007 g_FrontendClientPlayerRemovalPacket10007;

#endif
