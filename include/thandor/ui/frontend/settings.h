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

/* Submodule: ui/frontend/settings. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00549250 */
void __thandor_void_preserve_eax_ecx_edx
FrontendTaskAssignmentPage_Initialize(FrontendTaskAssignmentPageInitView26C4 *frontendRootPage);

/* 0x0054BA90 */
void __thandor_preserve_eax_edx
FrontendDisplaySettingsAction_ApplyPendingResolution(UiNodeBase *displaySettingsRoot);

/* 0x0054BAC0 */
void __thandor_preserve_eax
FrontendDisplaySettingsAction_ApplyPendingColorDepth(UiNodeBase *displaySettingsRoot);

/* 0x0054BAF0 */
void __thandor_void_preserve_eax_ecx_edx FrontendDisplaySettings_ApplyMode(void *control);

/* 0x0054CD70 */
void __thandor_void_preserve_eax_ecx
FrontendNetworkSettings_SetPlayerName(UiTextEditControl *control);

/* 0x00548E70 */
void __thandor_preserve_eax
FrontendGameplaySettings_SetGameSpeedPercent(UiSettingsValueControl *control);

/* 0x0054A810 */
void __thandor_void_preserve_eax_ecx_edx
FrontendGameplaySettings_SetRightButtonDoesNotScroll(UiSelectableControl *control);

/* 0x0054A850 */
void __thandor_preserve_eax
FrontendGameplaySettings_SetCameraScrollStep(UiSettingsValueControl *control);

/* 0x0054A870 */
void __thandor_void_preserve_eax_ecx_edx
FrontendGameplaySettings_SetAutomaticZoomOff(UiSelectableControl *control);

/* 0x0054A8B0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendGameplaySettings_SetAutomaticRotationOff(UiSelectableControl *control);

/* 0x0054A8F0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendGameplaySettings_SetLinkRotationZoom(UiSelectableControl *control);

/* 0x0054A950 */
void __thandor_void_preserve_eax_ecx_edx
FrontendGameplaySettings_SetLinkRotationTilt(UiSelectableControl *control);

/* 0x0054A9B0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendGameplaySettings_SetHidePanel(UiSelectableControl *control);

/* 0x0054A9F0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendGameplaySettingsPage_InitializeFromPersistentSettings(UiRootNode *frontendRoot);

/* 0x0054B740 */
void __thandor_void_preserve_eax_ecx_edx
FrontendGraphicsSettings_OpenAndSynchronize(FrontendGraphicsRuntimeSettingsPageState167C *source);

/* 0x0054B8D0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendAudioSettings_OpenAndSynchronize
          (FrontendPersistentSettingsPageSourceNodePtr settingsSourceNode);

/* 0x0054BCB0 */
void __thandor_void_preserve_eax_ecx
FrontendShadingSettings_SetEnabled(UiSelectableControl *control);

/* 0x0054BD10 */
void __thandor_void_preserve_eax_ecx_edx
FrontendShadingSettings_ApplyLevel(UiSelectableControl *control);

/* 0x0054BDE0 */
void __thandor_preserve_eax
FrontendModelSettings_SetLodDepthThresholdQ8(UiSettingsValueControl *control);

/* 0x0054BE10 */
void __thandor_void_preserve_eax_ecx_edx
FrontendTextureSettings_SetQuality(UiSelectableControl *control);

/* 0x0054BE90 */
void __thandor_void_preserve_eax_ecx
FrontendAudioSettings_SetEffectsEnabled(UiSelectableControl *control);

/* 0x0054BFD0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendAudioSettings_SetMusicEnabled(UiSelectableControl *control);

/* 0x0054C150 */
void __thandor_void_preserve_eax_ecx_edx
FrontendAudioSettings_SetReverseStereo(UiSelectableControl *control);

/* 0x0054C1A0 */
void __thandor_preserve_eax FrontendAudioSettings_SetEffectsGain(UiSettingsValueControl *control);

/* 0x0054C1D0 */
void __thandor_preserve_eax
FrontendAudioSettings_SetMovieDefaultGain(UiSettingsValueControl *control);

/* 0x0054C200 */
void __thandor_preserve_eax
FrontendAudioSettings_SetMovieAlternateGain(UiSettingsValueControl *control);

/* 0x0054C230 */
void __thandor_preserve_eax FrontendAudioSettings_SetMusicGain(UiSettingsValueControl *control);

/* 0x0054D170 */
void __thandor_preserve_eax FrontendNetworkSettings_SetPlayerCount(UiSettingsValueControl *control);

/* 0x0054D1F0 */
void __thandor_preserve_eax FrontendNetworkSettings_SetGameName(UiTextEditControl *control);

/* 0x0054D250 */
void __thandor_void_preserve_eax_ecx_edx
FrontendNetworkSettings_UpdateAction2002AvailabilityAndPublish
          (FrontendNetworkSettingsControlView250 *networkSettings);

/* 0x00549620 */
void __fastcall FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls (dword generationCursor,dword selectionIndex,UiRootNode *taskAssignmentRoot);

/* 0x0054CD20 */
bool __thandor_cf_preserve_eax_ecx_edx
FrontendNetworkSettings_PublishSelectedPlayerDescriptorCf
          (FrontendNetworkSettingsControlView250 *networkSettings);

/* 0x0054B160 */
void __thandor_void_preserve_eax_ecx_edx
FrontendDisplaySettingsPage_UpdateModeActionAvailability(UiNodeBase *displaySettingsRoot);

#endif /* THANDOR_UI_FRONTEND_SETTINGS_H */
