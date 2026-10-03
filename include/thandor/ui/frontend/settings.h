/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/settings.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_FRONTEND_SETTINGS_H
#define THANDOR_UI_FRONTEND_SETTINGS_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* The graphics settings page's shading level box as laid out in the frontend template (shadingLevelGroup and
   the six grid/depth choices that follow it). 0x2C4 bytes. */
typedef struct FrontendShadingLevelGroup {
    UiTitledWindowControl frame;
    UiNumericPairTextButton levels[6]; /* +0x54: grid/depth 32/32, 32/64, 32/128, 64/64, 64/128, 128/128 */
} FrontendShadingLevelGroup;

/* The graphics settings page's texture quality box as laid out in the frontend template (textureQualityGroup
   and its three choices). 0x174 bytes. */
typedef struct FrontendTextureQualityGroup {
    UiTitledWindowControl frame;
    UiTextButtonControl low;    /* +0x54 */
    UiTextButtonControl medium; /* +0xB4 */
    UiTextButtonControl high;   /* +0x114 */
} FrontendTextureQualityGroup;

/* Submodule: ui/frontend/settings. */
/* Functions are grouped by semantic ownership. */

void FrontendTaskAssignmentPage_Initialize(FrontendTaskAssignmentPageInitView *frontendRootPage);

void FrontendDisplaySettingsAction_ApplyPendingResolution(UiNodeBase *optionButton);

void FrontendDisplaySettingsAction_ApplyPendingColorDepth(UiNodeBase *optionButton);

void FrontendDisplaySettings_ApplyMode(void *control);

void FrontendNetworkSettings_SetPlayerName(UiTextEditControl *control);

void FrontendGameplaySettings_SetGameSpeedPercent(UiSettingsValueControl *control);

void FrontendGameplaySettings_SetRightButtonDoesNotScroll(UiSelectableControl *control);

void FrontendGameplaySettings_SetCameraScrollStep(UiSettingsValueControl *control);

void FrontendGameplaySettings_SetAutomaticZoomOff(UiSelectableControl *control);

void FrontendGameplaySettings_SetAutomaticRotationOff(UiSelectableControl *control);

void FrontendGameplaySettings_SetLinkRotationZoom(UiSelectableControl *control);

void FrontendGameplaySettings_SetLinkRotationTilt(UiSelectableControl *control);

void FrontendGameplaySettings_SetHidePanel(UiSelectableControl *control);

void FrontendGameplaySettingsPage_InitializeFromPersistentSettings(UiRootNode *frontendRoot);

void FrontendGraphicsSettings_OpenAndSynchronize(FrontendGraphicsRuntimeSettingsPageState *source);

void FrontendAudioSettings_OpenAndSynchronize(FrontendPersistentSettingsPageSourceNodePtr settingsSourceNode);

void FrontendShadingSettings_SetEnabled(UiSelectableControl *control);

void FrontendShadingSettings_ApplyLevel(UiSelectableControl *control);

void FrontendModelSettings_SetLodDepthThresholdQ8(UiSettingsValueControl *control);

void FrontendTextureSettings_SetQuality(UiSelectableControl *control);

void FrontendAudioSettings_SetEffectsEnabled(UiSelectableControl *control);

void FrontendAudioSettings_SetMusicEnabled(UiSelectableControl *control);

void FrontendAudioSettings_SetReverseStereo(UiSelectableControl *control);

void FrontendAudioSettings_SetEffectsGain(UiSettingsValueControl *control);

void FrontendAudioSettings_SetMovieDefaultGain(UiSettingsValueControl *control);

void FrontendAudioSettings_SetMovieAlternateGain(UiSettingsValueControl *control);

void FrontendAudioSettings_SetMusicGain(UiSettingsValueControl *control);

void FrontendNetworkSettings_SetPlayerCount(UiSettingsValueControl *control);

void FrontendNetworkSettings_SetGameName(UiTextEditControl *control);

void FrontendNetworkSettings_UpdateJoinButtonAndJoinOnDoubleClick
          (FrontendNetworkSettingsControlView *networkSettings);

void FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls(UiRootNode *taskAssignmentRoot);

bool FrontendNetworkSettings_PublishSelectedPlayerDescriptor(FrontendNetworkSettingsControlView *networkSettings);

void FrontendDisplaySettingsPage_UpdateModeActionAvailability(UiNodeBase *frontendRoot);

extern FrontendTaskAssignmentControlOffsetTables g_FrontendTaskAssignmentControlOffsets;
extern FrontendUiScratch g_FrontendUiDisplayModeAndTaskAssignmentScratch; /* followed by 4 bytes 0x90 fill (dropped) */
extern uint32_t g_FrontendMusicActiveBuffer;

extern FrontendUiActionHandlerPage20Prefix g_FrontendUiActionHandlersPage20;

#endif /* THANDOR_UI_FRONTEND_SETTINGS_H */
