/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/settings.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_FRONTEND_SETTINGS_H
#define THANDOR_UI_FRONTEND_SETTINGS_H

#include <thandor/core/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/ui/frontend/types.h>
#include <thandor/core/contracts.h>

/* The graphics settings page's shading level box as laid out in the frontend template (shadingLevelGroup and
   the six grid/depth choices that follow it). 0x2C4 bytes. */
struct FrontendShadingLevelGroup {
    UiTitledWindowControl frame;
    UiNumericPairTextButton levels[6]; /* +0x54: grid/depth 32/32, 32/64, 32/128, 64/64, 64/128, 128/128 */
};

/* The graphics settings page's texture quality box as laid out in the frontend template (textureQualityGroup
   and its three choices). 0x174 bytes. */
struct FrontendTextureQualityGroup {
    UiTitledWindowControl frame;
    UiTextButtonControl low;    /* +0x54 */
    UiTextButtonControl medium; /* +0xB4 */
    UiTextButtonControl high;   /* +0x114 */
};

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

extern SoundVoice *g_FrontendMusicActiveBuffer;

#endif /* THANDOR_UI_FRONTEND_SETTINGS_H */
