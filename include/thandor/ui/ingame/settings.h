#ifndef THANDOR_UI_INGAME_SETTINGS_H
#define THANDOR_UI_INGAME_SETTINGS_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/ingame/settings. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0056AB50 */
undefined4 InGameSettingsAction_CloseAlternatePanel(UiNodeBase *source);

/* 0x0056AB90 */
undefined4 InGameSettingsAction_CloseAndDepartPlayerMode0(UiNodeBase *source);

/* 0x0056ABF0 */
undefined4 InGameSettingsAction_CloseAndDepartPlayerMode1(UiNodeBase *source);

/* 0x0056C5E0 */
void InGameSettingsPage_CloseViaSharedToggle(UiNodeBase *source);

/* 0x0056C620 */
void InGameSettingsPage_OpenViaSharedToggle(UiNodeBase *source);

/* 0x0055F520 */
undefined8 InGameSimulationSpeed_AdjustPlayerAndRecomputeMinimumTicks (int param_1,undefined4 param_2,undefined4 param_3,int param_4);

/* 0x0056AA90 */
undefined8 InGameSettingsPage_SelectTab0(UiNodeBase *sourceNode);

/* 0x0056AAD0 */
undefined8 InGameSettingsPage_SelectTab1(UiNodeBase *sourceNode);

/* 0x0056AB10 */
undefined8 InGameSettingsPage_SelectTab2(UiNodeBase *sourceNode);

/* 0x0056BAF0 */
void InGameGameplaySettings_SetRightButtonDoesNotScroll(UiSelectableControl *control);

/* 0x0056BBB0 */
void InGameGameplaySettings_SetCameraScrollStep(UiSettingsValueControl *control);

/* 0x0056BBD0 */
void InGameGameplaySettings_SetAutomaticZoomOff(UiSelectableControl *control);

/* 0x0056BC20 */
void InGameGameplaySettings_SetAutomaticRotationOff(UiSelectableControl *control);

/* 0x0056BC70 */
void InGameGameplaySettings_SetLinkRotationZoom(UiSelectableControl *control);

/* 0x0056BCF0 */
void InGameGameplaySettings_SetLinkRotationTilt(UiSelectableControl *control);

/* 0x0056BD70 */
void InGameGameplaySettings_SetHidePanel(UiSelectableControl *control);

/* 0x0056C6D0 */
void InGameGraphicsSettings_OpenAndSynchronize(InGameGraphicsRuntimeSettingsPageState12D0 *source);

/* 0x0056C860 */
void InGameAudioSettings_OpenAndSynchronize (InGamePersistentSettingsPageSourceNodePtr settingsSourceNode);

/* 0x0056C9C0 */
void InGameShadingSettings_SetEnabled(UiSelectableControl *control);

/* 0x0056CA30 */
void InGameShadingSettings_ApplyLevel(UiSelectableControl *control);

/* 0x0056CB60 */
void InGameModelSettings_SetLodDepthThresholdQ8(UiSettingsValueControl *control);

/* 0x0056CB90 */
void InGameTextureSettings_SetQuality(UiSelectableControl *control);

/* 0x0056CC20 */
void InGameAudioSettings_SetEffectsEnabled(UiSelectableControl *control);

/* 0x0056CD80 */
void InGameAudioSettings_SetMusicEnabled(UiSelectableControl *control);

/* 0x0056CE80 */
void InGameAudioSettings_SetReverseStereo(UiSelectableControl *control);

/* 0x0056CED0 */
void InGameAudioSettings_SetEffectsGain(UiSettingsValueControl *control);

/* 0x0056CF10 */
void InGameAudioSettings_SetMovieDefaultGain(UiSettingsValueControl *control);

/* 0x0056CF40 */
void InGameAudioSettings_SetMusicGain(UiSettingsValueControl *control);

/* 0x0056CF70 */
void InGameAudioSettings_SetMovieAlternateGain(UiSettingsValueControl *control);

/* 0x0056C440 */
void InGameSettingsPage_ToggleAndSynchronizeControls(UiSelectableControl *settingsToggle);

#endif /* THANDOR_UI_INGAME_SETTINGS_H */
