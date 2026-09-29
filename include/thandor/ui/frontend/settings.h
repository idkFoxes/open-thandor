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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00549250 */
void FrontendTaskAssignmentPage_Initialize(FrontendTaskAssignmentPageInitView *frontendRootPage);

/* 0x0054BA90 */
void FrontendDisplaySettingsAction_ApplyPendingResolution(UiNodeBase *optionButton);

/* 0x0054BAC0 */
void FrontendDisplaySettingsAction_ApplyPendingColorDepth(UiNodeBase *optionButton);

/* 0x0054BAF0 */
void FrontendDisplaySettings_ApplyMode(void *control);

/* 0x0054CD70 */
void FrontendNetworkSettings_SetPlayerName(UiTextEditControl *control);

/* 0x00548E70 */
void FrontendGameplaySettings_SetGameSpeedPercent(UiSettingsValueControl *control);

/* 0x0054A810 */
void FrontendGameplaySettings_SetRightButtonDoesNotScroll(UiSelectableControl *control);

/* 0x0054A850 */
void FrontendGameplaySettings_SetCameraScrollStep(UiSettingsValueControl *control);

/* 0x0054A870 */
void FrontendGameplaySettings_SetAutomaticZoomOff(UiSelectableControl *control);

/* 0x0054A8B0 */
void FrontendGameplaySettings_SetAutomaticRotationOff(UiSelectableControl *control);

/* 0x0054A8F0 */
void FrontendGameplaySettings_SetLinkRotationZoom(UiSelectableControl *control);

/* 0x0054A950 */
void FrontendGameplaySettings_SetLinkRotationTilt(UiSelectableControl *control);

/* 0x0054A9B0 */
void FrontendGameplaySettings_SetHidePanel(UiSelectableControl *control);

/* 0x0054A9F0 */
void FrontendGameplaySettingsPage_InitializeFromPersistentSettings(UiRootNode *frontendRoot);

/* 0x0054B740 */
void FrontendGraphicsSettings_OpenAndSynchronize(FrontendGraphicsRuntimeSettingsPageState *source);

/* 0x0054B8D0 */
void FrontendAudioSettings_OpenAndSynchronize(FrontendPersistentSettingsPageSourceNodePtr settingsSourceNode);

/* 0x0054BCB0 */
void FrontendShadingSettings_SetEnabled(UiSelectableControl *control);

/* 0x0054BD10 */
void FrontendShadingSettings_ApplyLevel(UiSelectableControl *control);

/* 0x0054BDE0 */
void FrontendModelSettings_SetLodDepthThresholdQ8(UiSettingsValueControl *control);

/* 0x0054BE10 */
void FrontendTextureSettings_SetQuality(UiSelectableControl *control);

/* 0x0054BE90 */
void FrontendAudioSettings_SetEffectsEnabled(UiSelectableControl *control);

/* 0x0054BFD0 */
void FrontendAudioSettings_SetMusicEnabled(UiSelectableControl *control);

/* 0x0054C150 */
void FrontendAudioSettings_SetReverseStereo(UiSelectableControl *control);

/* 0x0054C1A0 */
void FrontendAudioSettings_SetEffectsGain(UiSettingsValueControl *control);

/* 0x0054C1D0 */
void FrontendAudioSettings_SetMovieDefaultGain(UiSettingsValueControl *control);

/* 0x0054C200 */
void FrontendAudioSettings_SetMovieAlternateGain(UiSettingsValueControl *control);

/* 0x0054C230 */
void FrontendAudioSettings_SetMusicGain(UiSettingsValueControl *control);

/* 0x0054D170 */
void FrontendNetworkSettings_SetPlayerCount(UiSettingsValueControl *control);

/* 0x0054D1F0 */
void FrontendNetworkSettings_SetGameName(UiTextEditControl *control);

/* 0x0054D250 */
void FrontendNetworkSettings_UpdateJoinButtonAndJoinOnDoubleClick
          (FrontendNetworkSettingsControlView *networkSettings);

/* 0x00549620 */
void FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls(UiRootNode *taskAssignmentRoot);

/* 0x0054CD20 */
bool FrontendNetworkSettings_PublishSelectedPlayerDescriptor(FrontendNetworkSettingsControlView *networkSettings);

/* 0x0054B160 */
void FrontendDisplaySettingsPage_UpdateModeActionAvailability(UiNodeBase *frontendRoot);

#endif /* THANDOR_UI_FRONTEND_SETTINGS_H */
