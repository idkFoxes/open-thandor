/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/settings.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_SETTINGS_H
#define THANDOR_UI_INGAME_SETTINGS_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/ingame/settings. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0056AB50 */
void __thandor_preserve_eax InGameSettingsAction_CloseAlternatePanel(UiNodeBase *source);

/* 0x0056AB90 */
void __thandor_preserve_eax InGameSettingsAction_CloseAndDepartPlayerMode0(UiNodeBase *source);

/* 0x0056ABF0 */
void __thandor_preserve_eax InGameSettingsAction_CloseAndDepartPlayerMode1(UiNodeBase *source);

/* 0x0056C5E0 */
void __thandor_preserve_eax InGameSettingsPage_CloseViaSharedToggle(UiNodeBase *source);

/* 0x0056C620 */
void __thandor_preserve_eax InGameSettingsPage_OpenViaSharedToggle(UiNodeBase *source);

/* 0x0055F520 */
void __thandor_void_preserve_eax_ecx_edx
InGameSimulationSpeed_AdjustPlayerAndRecomputeMinimumTicks
          (FrontendPlayerRuntimeId playerRuntimeId,dword reservedZero0,dword reservedZero1,
          int stepDelta);

/* 0x0056AA90 */
void __thandor_void_preserve_eax_ecx_edx InGameSettingsPage_SelectTab0(UiNodeBase *sourceNode);

/* 0x0056AAD0 */
void __thandor_void_preserve_eax_ecx_edx InGameSettingsPage_SelectTab1(UiNodeBase *sourceNode);

/* 0x0056AB10 */
void __thandor_void_preserve_eax_ecx_edx InGameSettingsPage_SelectTab2(UiNodeBase *sourceNode);

/* 0x0056BAF0 */
void __thandor_void_preserve_eax_ecx_edx
InGameGameplaySettings_SetRightButtonDoesNotScroll(UiSelectableControl *control);

/* 0x0056BBB0 */
void __thandor_preserve_eax
InGameGameplaySettings_SetCameraScrollStep(UiSettingsValueControl *control);

/* 0x0056BBD0 */
void __thandor_void_preserve_eax_ecx_edx
InGameGameplaySettings_SetAutomaticZoomOff(UiSelectableControl *control);

/* 0x0056BC20 */
void __thandor_void_preserve_eax_ecx_edx
InGameGameplaySettings_SetAutomaticRotationOff(UiSelectableControl *control);

/* 0x0056BC70 */
void __thandor_void_preserve_eax_ecx_edx
InGameGameplaySettings_SetLinkRotationZoom(UiSelectableControl *control);

/* 0x0056BCF0 */
void __thandor_void_preserve_eax_ecx_edx
InGameGameplaySettings_SetLinkRotationTilt(UiSelectableControl *control);

/* 0x0056BD70 */
void __thandor_void_preserve_eax_ecx_edx
InGameGameplaySettings_SetHidePanel(UiSelectableControl *control);

/* 0x0056C6D0 */
void __thandor_void_preserve_eax_ecx_edx
InGameGraphicsSettings_OpenAndSynchronize(InGameGraphicsRuntimeSettingsPageState12D0 *source);

/* 0x0056C860 */
void __thandor_void_preserve_eax_ecx_edx
InGameAudioSettings_OpenAndSynchronize(InGamePersistentSettingsPageSourceNodePtr settingsSourceNode);

/* 0x0056C9C0 */
void __thandor_void_preserve_eax_ecx InGameShadingSettings_SetEnabled(UiSelectableControl *control);

/* 0x0056CA30 */
void __thandor_void_preserve_eax_ecx_edx
InGameShadingSettings_ApplyLevel(UiSelectableControl *control);

/* 0x0056CB60 */
void __thandor_preserve_eax
InGameModelSettings_SetLodDepthThresholdQ8(UiSettingsValueControl *control);

/* 0x0056CB90 */
void __thandor_void_preserve_eax_ecx_edx
InGameTextureSettings_SetQuality(UiSelectableControl *control);

/* 0x0056CC20 */
void __thandor_void_preserve_eax_ecx
InGameAudioSettings_SetEffectsEnabled(UiSelectableControl *control);

/* 0x0056CD80 */
void __thandor_void_preserve_eax_ecx
InGameAudioSettings_SetMusicEnabled(UiSelectableControl *control);

/* 0x0056CE80 */
void __thandor_void_preserve_eax_ecx_edx
InGameAudioSettings_SetReverseStereo(UiSelectableControl *control);

/* 0x0056CED0 */
void __thandor_preserve_eax InGameAudioSettings_SetEffectsGain(UiSettingsValueControl *control);

/* 0x0056CF10 */
void __thandor_preserve_eax InGameAudioSettings_SetMovieDefaultGain(UiSettingsValueControl *control);

/* 0x0056CF40 */
void __thandor_preserve_eax InGameAudioSettings_SetMusicGain(UiSettingsValueControl *control);

/* 0x0056CF70 */
void __thandor_preserve_eax
InGameAudioSettings_SetMovieAlternateGain(UiSettingsValueControl *control);

/* 0x0056C440 */
void __thandor_void_preserve_eax_ecx_edx
InGameSettingsPage_ToggleAndSynchronizeControls(UiSelectableControl *settingsToggle);

#endif /* THANDOR_UI_INGAME_SETTINGS_H */
