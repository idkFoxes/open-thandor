/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/settings.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_SETTINGS_H
#define THANDOR_UI_INGAME_SETTINGS_H

#include <thandor/network/protocol/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/ui/frontend/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

/* Game menu / gameplay settings actions (InGameUiImage template) */
inline constexpr int32_t INGAME_ACTION_SAVE_GAME_WINDOW = 0x120E; /* gameMenuSaveButton; not offered in network games */
inline constexpr int32_t INGAME_ACTION_LINK_ROTATION_ZOOM = 0x1214; /* linkRotationZoomCheckbox, excludes the tilt link */
inline constexpr int32_t INGAME_ACTION_LINK_ROTATION_TILT = 0x1215; /* linkRotationTiltCheckbox, excludes the zoom link */
/* Graphics and sound settings windows: controls that are disabled while their option is off */
inline constexpr int32_t INGAME_ACTION_SHADING_LEVEL = 0x1205; /* the six shading level buttons, need shading on */
inline constexpr int32_t INGAME_ACTION_TEXTURE_QUALITY = 0x1207; /* the three texture quality buttons, local games only */
inline constexpr int32_t INGAME_ACTION_REVERSE_STEREO = 0x120A; /* reverseStereoCheckbox, needs effects or music on */
inline constexpr int32_t INGAME_ACTION_EFFECTS_VOLUME = 0x120B; /* effectsVolumeSlider, needs effects on */
inline constexpr int32_t INGAME_ACTION_MOVIE_VOLUME = 0x120C; /* movieVolumeSlider, needs effects on */
inline constexpr int32_t INGAME_ACTION_MUSIC_VOLUME = 0x120D; /* musicVolumeSlider, needs music on */
inline constexpr int32_t INGAME_ACTION_MESSAGE_MOVIE_VOLUME = 0x121A; /* messageMovieVolumeSlider, needs effects on */
/* Highest per-player simulation step batch (InGameSimulationSpeed_AdjustPlayerAndRecomputeMinimumTicks) */
inline constexpr int32_t INGAME_SIMULATION_STEP_TICKS_MAX = 5;
/* Minimap view values restored when automatic zoom / rotation is switched off */
inline constexpr int32_t INGAME_MINIMAP_DEFAULT_SCALE_Q12 = 0x800; /* 0.5 */
inline constexpr int32_t INGAME_MINIMAP_DEFAULT_ROTATION_ANGLE = 0x2000;

void InGameSettingsAction_CloseAlternatePanel(UiNodeBase *source);

void InGameSettingsPage_CloseViaSharedToggle(UiNodeBase *source);

void InGameSettingsPage_OpenViaSharedToggle(UiNodeBase *source);

void InGameSimulationSpeed_AdjustPlayerAndRecomputeMinimumTicks
          (FrontendPlayerRuntimeId playerRuntimeId,uint32_t reservedZero0,uint32_t reservedZero1,
          int stepDelta);

void InGameGameplaySettings_SetRightButtonDoesNotScroll(UiSelectableControl *control);

void InGameGameplaySettings_SetCameraScrollStep(UiSettingsValueControl *control);

void InGameGameplaySettings_SetAutomaticZoomOff(UiSelectableControl *control);

void InGameGameplaySettings_SetAutomaticRotationOff(UiSelectableControl *control);

void InGameGameplaySettings_SetLinkRotationZoom(UiSelectableControl *control);

void InGameGameplaySettings_SetLinkRotationTilt(UiSelectableControl *control);

void InGameGameplaySettings_SetHidePanel(UiSelectableControl *control);

void InGameGraphicsSettings_OpenAndSynchronize(UiNodeBase *graphicsButton);

void InGameAudioSettings_OpenAndSynchronize(InGamePersistentSettingsPageSourceNodePtr settingsSourceNode);

void InGameShadingSettings_SetEnabled(UiSelectableControl *control);

void InGameShadingSettings_ApplyLevel(UiSelectableControl *control);

void InGameModelSettings_SetLodDepthThresholdQ8(UiSettingsValueControl *control);

void InGameTextureSettings_SetQuality(UiSelectableControl *control);

void InGameAudioSettings_SetEffectsEnabled(UiSelectableControl *control);

void InGameAudioSettings_SetMusicEnabled(UiSelectableControl *control);

void InGameAudioSettings_SetReverseStereo(UiSelectableControl *control);

void InGameAudioSettings_SetEffectsGain(UiSettingsValueControl *control);

void InGameAudioSettings_SetMovieDefaultGain(UiSettingsValueControl *control);

void InGameAudioSettings_SetMusicGain(UiSettingsValueControl *control);

void InGameAudioSettings_SetMovieAlternateGain(UiSettingsValueControl *control);

void InGameSettingsPage_ToggleAndSynchronizeControls(UiSelectableControl *settingsToggle);

extern InGameUiActionHandlerPage12Prefix28 g_InGameUiActionHandlersPage12;

#endif /* THANDOR_UI_INGAME_SETTINGS_H */
