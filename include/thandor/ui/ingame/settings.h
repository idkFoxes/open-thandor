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

/* Game menu / gameplay settings actions (InGameUiImage template) */
#define INGAME_ACTION_SAVE_GAME_WINDOW 0x120E /* gameMenuSaveButton; not offered in network games */
#define INGAME_ACTION_LINK_ROTATION_ZOOM 0x1214 /* linkRotationZoomCheckbox, excludes the tilt link */
#define INGAME_ACTION_LINK_ROTATION_TILT 0x1215 /* linkRotationTiltCheckbox, excludes the zoom link */
/* Graphics and sound settings windows: controls that are disabled while their option is off */
#define INGAME_ACTION_SHADING_LEVEL 0x1205 /* the six shading level buttons, need shading on */
#define INGAME_ACTION_TEXTURE_QUALITY 0x1207 /* the three texture quality buttons, local games only */
#define INGAME_ACTION_REVERSE_STEREO 0x120A /* reverseStereoCheckbox, needs effects or music on */
#define INGAME_ACTION_EFFECTS_VOLUME 0x120B /* effectsVolumeSlider, needs effects on */
#define INGAME_ACTION_MOVIE_VOLUME 0x120C /* movieVolumeSlider, needs effects on */
#define INGAME_ACTION_MUSIC_VOLUME 0x120D /* musicVolumeSlider, needs music on */
#define INGAME_ACTION_MESSAGE_MOVIE_VOLUME 0x121A /* messageMovieVolumeSlider, needs effects on */
/* Highest per-player simulation step batch (InGameSimulationSpeed_AdjustPlayerAndRecomputeMinimumTicks) */
#define INGAME_SIMULATION_STEP_TICKS_MAX 5
/* Minimap view values restored when automatic zoom / rotation is switched off */
#define INGAME_MINIMAP_DEFAULT_SCALE_Q12 0x800 /* 0.5 */
#define INGAME_MINIMAP_DEFAULT_ROTATION_ANGLE 0x2000
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0056AB50 */
void InGameSettingsAction_CloseAlternatePanel(UiNodeBase *source);

/* 0x0056AB90 */
void InGameQuitMenu_AbortMission(UiNodeBase *source);

/* 0x0056ABF0 */
void InGameQuitMenu_Surrender(UiNodeBase *source);

/* 0x0056C5E0 */
void InGameSettingsPage_CloseViaSharedToggle(UiNodeBase *source);

/* 0x0056C620 */
void InGameSettingsPage_OpenViaSharedToggle(UiNodeBase *source);

/* 0x0055F520 */
void InGameSimulationSpeed_AdjustPlayerAndRecomputeMinimumTicks
          (FrontendPlayerRuntimeId playerRuntimeId,uint32_t reservedZero0,uint32_t reservedZero1,
          int stepDelta);

/* 0x0056AA90 */
void InGameMissionHelpPage_SelectBriefingTab(UiNodeBase *sourceNode);

/* 0x0056AAD0 */
void InGameMissionHelpPage_SelectKeyboardTab(UiNodeBase *sourceNode);

/* 0x0056AB10 */
void InGameMissionHelpPage_SelectMouseTab(UiNodeBase *sourceNode);

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
void InGameGraphicsSettings_OpenAndSynchronize(UiNodeBase *graphicsButton);

/* 0x0056C860 */
void InGameAudioSettings_OpenAndSynchronize(InGamePersistentSettingsPageSourceNodePtr settingsSourceNode);

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
