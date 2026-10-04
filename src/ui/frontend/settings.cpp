/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/settings.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/settings.h>
#include <thandor/thandor.h>
#include <thandor/ui/controls/settings_option.h>

/* Module data. */

IDirectSoundBuffer *g_FrontendMusicActiveBuffer = nullptr;

/* Implementation ownership: ui/frontend/settings. */

/* Handler of the mission briefing page's game speed slider (FRONTEND_ACTION_GAME_SPEED, slot 74 of
   g_FrontendUiActionHandlersPage20): applies the percentage directly in a local game and as
   FRONTEND_COMMAND_SET_GAME_SPEED in a network game, and saves it as PERSISTENT_SETTING_GAME_SPEED_PERCENT.
*/
void FrontendGameplaySettings_SetGameSpeedPercent(UiSettingsValueControl *control)

{
  FrontendCommand_Issue<FrontendSession_SetGameSpeedPercent>(0,0,control->boundValue);
  PersistentOption_StoreSlider(control,PERSISTENT_SETTING_GAME_SPEED_PERCENT);
}

/* Handler of the gameplay settings checkbox with action 0x2049: stores its state as
   PERSISTENT_MAP_OPTION_SIDE_PANEL_HIDDEN in the persistent map/mouse option flags, which the session reads
   when it starts. The checkbox text reads "right button does not scroll", but the bit hides the in-game side
   panel (see InGameGameplaySettings_SetRightButtonDoesNotScroll).
*/
void FrontendGameplaySettings_SetRightButtonDoesNotScroll(UiSelectableControl *control)

{
  PersistentOption_ApplyCheckbox(control,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS,PERSISTENT_MAP_OPTION_SIDE_PANEL_HIDDEN);
}

/* Handler of the options page's scroll-speed slider (scrollSpeedSlider, action 0x204B, slot 75 of
   g_FrontendUiActionHandlersPage20): saves the value as PERSISTENT_SETTING_CAMERA_SCROLL_STEP.
*/
void FrontendGameplaySettings_SetCameraScrollStep(UiSettingsValueControl *control)

{
  PersistentOption_StoreSlider(control,PERSISTENT_SETTING_CAMERA_SCROLL_STEP);
}

/* Handler of the "Automatic zoom off" checkbox (autoZoomOffCheckbox, action 0x203C, slot 60 of
   g_FrontendUiActionHandlersPage20): stores its state as PERSISTENT_MAP_OPTION_AUTOMATIC_ZOOM_OFF.
*/
void FrontendGameplaySettings_SetAutomaticZoomOff(UiSelectableControl *control)

{
  PersistentOption_ApplyCheckbox(control,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS,PERSISTENT_MAP_OPTION_AUTOMATIC_ZOOM_OFF);
}

/* Handler of the "Automatic rotation off" checkbox (autoRotationOffCheckbox, action 0x203D, slot 61 of
   g_FrontendUiActionHandlersPage20): stores its state as PERSISTENT_MAP_OPTION_AUTOMATIC_ROTATION_OFF.
*/
void FrontendGameplaySettings_SetAutomaticRotationOff(UiSelectableControl *control)

{
  PersistentOption_ApplyCheckbox(control,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS,PERSISTENT_MAP_OPTION_AUTOMATIC_ROTATION_OFF);
}

/* Handler of the "Link rotation/zoom" checkbox (FRONTEND_ACTION_LINK_ROTATION_ZOOM, slot 62 of
   g_FrontendUiActionHandlersPage20): stores its state as PERSISTENT_LINK_OPTION_ROTATION_ZOOM. The two link
   options exclude each other, so while this one is set the "Link rotation/tilt" checkbox is hidden.
*/
void FrontendGameplaySettings_SetLinkRotationZoom(UiSelectableControl *control)

{
  PersistentOption_ApplyCheckbox(control,PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS,PERSISTENT_LINK_OPTION_ROTATION_ZOOM,
                                 [control](Bool8 isSelected) {
    if (isSelected) {
      UiNodeList_SuppressActionId(FRONTEND_ACTION_LINK_ROTATION_TILT,control->base.parent);
    }
    else {
      UiNodeList_UnsuppressActionId(FRONTEND_ACTION_LINK_ROTATION_TILT,control->base.parent);
    }
  });
}

/* Handler of the "Link rotation/tilt" checkbox (FRONTEND_ACTION_LINK_ROTATION_TILT, slot 63 of
   g_FrontendUiActionHandlersPage20): stores its state as PERSISTENT_LINK_OPTION_ROTATION_TILT and, while set,
   hides the "Link rotation/zoom" checkbox.
*/
void FrontendGameplaySettings_SetLinkRotationTilt(UiSelectableControl *control)

{
  PersistentOption_ApplyCheckbox(control,PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS,PERSISTENT_LINK_OPTION_ROTATION_TILT,
                                 [control](Bool8 isSelected) {
    if (isSelected) {
      UiNodeList_SuppressActionId(FRONTEND_ACTION_LINK_ROTATION_ZOOM,control->base.parent);
    }
    else {
      UiNodeList_UnsuppressActionId(FRONTEND_ACTION_LINK_ROTATION_ZOOM,control->base.parent);
    }
  });
}

/* Handler of action 0x2051 (slot 81 of g_FrontendUiActionHandlersPage20): stores the checkbox state as
   PERSISTENT_LINK_OPTION_HIDE_PANEL. The frontend template calls the 0x2051 checkbox "Right button does not
   scroll" and the 0x2049 one "Hide panel", the opposite of what this handler and
   FrontendGameplaySettings_SetRightButtonDoesNotScroll store.
*/
void FrontendGameplaySettings_SetHidePanel(UiSelectableControl *control)

{
  PersistentOption_ApplyCheckbox(control,PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS,PERSISTENT_LINK_OPTION_HIDE_PANEL);
}

/* Opens the options page (FRONTEND_PAGE_ACTION_GAMEPLAY_SETTINGS_PAGE) and loads its controls from the
   persistent settings: map and mouse option checkboxes and the scroll speed. The two "link rotation" options
   exclude each other, so the one that is set hides the other checkbox.
*/
void FrontendGameplaySettingsPage_InitializeFromPersistentSettings(UiRootNode *frontendRoot)

{
  FrontendModelPointerContextFlags *menuRoomContextFlags;
  uint32_t persistedValue;

  UiPageStack_SetActiveIndex(FRONTEND_PAGE_OPTIONS,(UiPageStackControl *)FRONTEND_UI(frontendRoot,frontendPageStack));
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    menuRoomContextFlags =
         &((FrontendModelPointerContext *)FRONTEND_UI(frontendRoot,menuRoomModelView))->contextFlags;
    *menuRoomContextFlags = *menuRoomContextFlags | FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  persistedValue = PersistentSettings_Read(0,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS);
  UiSelectableControl_SetSelected
            (persistedValue & PERSISTENT_MAP_OPTION_AUTOMATIC_ZOOM_OFF,
             (UiSelectableControl *)FRONTEND_UI(frontendRoot,autoZoomOffCheckbox));
  UiSelectableControl_SetSelected
            (persistedValue & PERSISTENT_MAP_OPTION_AUTOMATIC_ROTATION_OFF,
             (UiSelectableControl *)FRONTEND_UI(frontendRoot,autoRotationOffCheckbox));
  /* Bit 4 is the "right button does not scroll" checkbox (its action 0x2049 handler is
     FrontendGameplaySettings_SetRightButtonDoesNotScroll), which hides the in-game side panel; the template
     calls this control hidePanelCheckbox. */
  UiSelectableControl_SetSelected(persistedValue & PERSISTENT_MAP_OPTION_SIDE_PANEL_HIDDEN,
                                  (UiSelectableControl *)FRONTEND_UI(frontendRoot,hidePanelCheckbox));
  persistedValue = PersistentSettings_Read(0,PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS);
  if ((persistedValue & PERSISTENT_LINK_OPTION_ROTATION_ZOOM) != 0) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_LINK_ROTATION_TILT,&frontendRoot->base);
  }
  UiSelectableControl_SetSelected
            (persistedValue & PERSISTENT_LINK_OPTION_ROTATION_ZOOM,
             (UiSelectableControl *)FRONTEND_UI(frontendRoot,linkRotationZoomCheckbox));
  if ((persistedValue & PERSISTENT_LINK_OPTION_ROTATION_TILT) != 0) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_LINK_ROTATION_ZOOM,&frontendRoot->base);
  }
  UiSelectableControl_SetSelected(persistedValue & PERSISTENT_LINK_OPTION_ROTATION_TILT,
                                  (UiSelectableControl *)FRONTEND_UI(frontendRoot,linkRotationTiltCheckbox));
  /* bit 4 of this word (hide panel, action 0x2051) is not loaded into its checkbox here */
  persistedValue = PersistentSettings_Read(PERSISTENT_DEFAULT_CAMERA_SCROLL_STEP,PERSISTENT_SETTING_CAMERA_SCROLL_STEP);
  ((UiRangeSliderControl *)FRONTEND_UI(frontendRoot,scrollSpeedSlider))->value = persistedValue;
}

/* Handler of the options page's "3D" button (settings3DButton, action 0x2012, slot 18 of
   g_FrontendUiActionHandlersPage20): opens the graphics settings page and loads its controls from the
   persistent settings: the shading toggle (the shading levels are only offered while it is on), the shading
   level matching the saved grid size and depth, the texture quality and the polygon detail (LOD) slider.
*/
void FrontendGraphicsSettings_OpenAndSynchronize(FrontendGraphicsRuntimeSettingsPageState *source)

{
  FrontendGraphicsRuntimeSettingsPageState *rootNode;
  uint32_t persistedValue;
  uint32_t shadingDepthQuarter;
  int shadingDepth;
  UiNodeBase *parentCursor;
  UiNodeBase *selectedShadingRow;
  UiNodeBase *selectedTextureRow;
  /* source is the frontend template's settings3DButton. */
  FrontendUiImage *frontendUi;
  
  frontendUi = (FrontendUiImage *)((uint8_t *)source - offsetof(FrontendUiImage,settings3DButton));
  UiPageStack_SetActiveIndex(FRONTEND_PAGE_GRAPHICS_SETTINGS,
                             (UiPageStackControl *)FRONTEND_UI(frontendUi,frontendPageStack));
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    ((FrontendModelPointerContext *)FRONTEND_UI(frontendUi,menuRoomModelView))->contextFlags |=
         FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  persistedValue = PersistentSettings_Read(1,PERSISTENT_SETTING_SHADING_ENABLED);
  UiSelectableControl_SetSelected(persistedValue,&source->shadingEnabledControl);
  parentCursor = source->base.parent;
  rootNode = source;
  /* climb to the root of the control's UI tree */
  while (parentCursor != UI_NODE_NONE) {
    rootNode = (FrontendGraphicsRuntimeSettingsPageState *)(rootNode->base).parent;
    parentCursor = rootNode->base.parent;
  }
  if (persistedValue == 0) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_SHADING_LEVEL,&rootNode->base);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_SHADING_LEVEL,&rootNode->base);
  }
  /* the six shading level rows: grid/depth 32/32, 32/64, 32/128, 64/64, 64/128, 128/128 (the saved depth is
     stored divided by four) */
  persistedValue = PersistentSettings_Read(32,PERSISTENT_SETTING_SHADING_GRID_HALF_SIZE);
  shadingDepthQuarter = PersistentSettings_Read(16,PERSISTENT_SETTING_SHADING_SUBRESOURCE_COUNT);
  shadingDepth = shadingDepthQuarter * 4;
  if (persistedValue == 32) {
    selectedShadingRow = (UiNodeBase *)&source->shadingResolutionRows;
    if (shadingDepth == 64) {
      selectedShadingRow = (UiNodeBase *)(source->shadingResolutionRows.rows + 1);
    }
    else if (shadingDepth == 128) {
      selectedShadingRow = (UiNodeBase *)(source->shadingResolutionRows.rows + 2);
    }
  }
  else if (persistedValue == 64) {
    selectedShadingRow = (UiNodeBase *)(source->shadingResolutionRows.rows + 3);
    if (shadingDepth == 128) {
      selectedShadingRow = (UiNodeBase *)(source->shadingResolutionRows.rows + 4);
    }
  }
  else {
    selectedShadingRow = (UiNodeBase *)(source->shadingResolutionRows.rows + 5);
  }
  UiSelectableGroup_SelectExclusive(6,selectedShadingRow,
      FRONTEND_UI(frontendUi,shadingLevelGrid128Depth128),
      FRONTEND_UI(frontendUi,shadingLevelGrid64Depth128),
      FRONTEND_UI(frontendUi,shadingLevelGrid64Depth64),
      FRONTEND_UI(frontendUi,shadingLevelGrid32Depth128),
      FRONTEND_UI(frontendUi,shadingLevelGrid32Depth64),
      FRONTEND_UI(frontendUi,shadingLevelGrid32Depth32));
  /* texture rows: low, medium, high */
  persistedValue = PersistentSettings_Read(TEXTURE_QUALITY_MEDIUM,PERSISTENT_SETTING_TEXTURE_QUALITY);
  if (persistedValue == TEXTURE_QUALITY_HIGH) {
    selectedTextureRow = (UiNodeBase *)(source->textureResolutionRows.rows + 2);
  }
  else if (persistedValue == TEXTURE_QUALITY_MEDIUM) {
    selectedTextureRow = (UiNodeBase *)(source->textureResolutionRows.rows + 1);
  }
  else {
    selectedTextureRow = (UiNodeBase *)&source->textureResolutionRows;
  }
  UiSelectableGroup_SelectExclusive(3,selectedTextureRow,
      FRONTEND_UI(frontendUi,textureQualityHigh),
      FRONTEND_UI(frontendUi,textureQualityMedium),
      FRONTEND_UI(frontendUi,textureQualityLow));
  persistedValue = PersistentSettings_Read(PERSISTENT_DEFAULT_MODEL_LOD_DEPTH_THRESHOLD,PERSISTENT_SETTING_MODEL_LOD_DEPTH_THRESHOLD);
  source->polygonResolutionLodThresholdQ8 = persistedValue;
}

/* Handler of the options page's "Sound" button (soundSettingsButton, action 0x2013, slot 19 of
   g_FrontendUiActionHandlersPage20): opens the audio settings page and loads its toggles and volume sliders
   from the persistent settings. The effect and movie volumes are only offered with effects on, the music volume
   only with music on, and reverse stereo only while either is on. The movie-event slider is not loaded.
*/
void FrontendAudioSettings_OpenAndSynchronize(FrontendPersistentSettingsPageSourceNodePtr settingsSourceNode)

{
  UiNodeFlags *compactLayoutFlags;
  UiNodeBase *parentCursor;
  uint32_t audioFlags;
  uint32_t gainValue;

  UiPageStack_SetActiveIndex(FRONTEND_PAGE_AUDIO_SETTINGS,&THANDOR_CONTAINER_OF(settingsSourceNode, FrontendPersistentSettingsPage, sourceNode)->settingsPageStack);
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    compactLayoutFlags = &THANDOR_CONTAINER_OF(settingsSourceNode, FrontendPersistentSettingsPage, sourceNode)->pageRoot.nodeFlags;
    *compactLayoutFlags = *compactLayoutFlags | FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  audioFlags = PersistentSettings_Read(PERSISTENT_SOUND_OPTION_DEFAULT,PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
  UiSelectableControl_SetSelected(audioFlags & PERSISTENT_SOUND_OPTION_EFFECTS,&THANDOR_CONTAINER_OF(settingsSourceNode, FrontendPersistentSettingsPage, sourceNode)->soundEffectsEnabledControl);
  UiSelectableControl_SetSelected(audioFlags & PERSISTENT_SOUND_OPTION_MUSIC,&THANDOR_CONTAINER_OF(settingsSourceNode, FrontendPersistentSettingsPage, sourceNode)->musicEnabledControl);
  UiSelectableControl_SetSelected(audioFlags & PERSISTENT_SOUND_OPTION_REVERSE_STEREO,&THANDOR_CONTAINER_OF(settingsSourceNode, FrontendPersistentSettingsPage, sourceNode)->reverseStereoControl);
  gainValue = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_EFFECTS_GAIN); /* Q15 1.0 */
  (THANDOR_CONTAINER_OF(settingsSourceNode, FrontendPersistentSettingsPage, sourceNode)->soundEffectsGainControl).currentValue = gainValue;
  gainValue = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_MOVIE_DEFAULT_GAIN);
  (THANDOR_CONTAINER_OF(settingsSourceNode, FrontendPersistentSettingsPage, sourceNode)->movieDefaultAudioGainControl).currentValue = gainValue;
  gainValue = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_MUSIC_GAIN);
  (THANDOR_CONTAINER_OF(settingsSourceNode, FrontendPersistentSettingsPage, sourceNode)->musicGainControl).currentValue = gainValue;
  parentCursor = settingsSourceNode->parent;
  /* climb to the root of the control's UI tree */
  while (parentCursor != UI_NODE_NONE) {
    settingsSourceNode = settingsSourceNode->parent;
    parentCursor = settingsSourceNode->parent;
  }
  if ((audioFlags & PERSISTENT_SOUND_OPTION_EFFECTS) == 0) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_EFFECTS_GAIN,settingsSourceNode);
    UiNodeList_SuppressActionId(FRONTEND_ACTION_MOVIE_GAIN,settingsSourceNode);
    UiNodeList_SuppressActionId(FRONTEND_ACTION_MOVIE_EVENT_GAIN,settingsSourceNode);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_EFFECTS_GAIN,settingsSourceNode);
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_MOVIE_GAIN,settingsSourceNode);
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_MOVIE_EVENT_GAIN,settingsSourceNode);
  }
  if ((audioFlags & PERSISTENT_SOUND_OPTION_MUSIC) == 0) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_MUSIC_GAIN,settingsSourceNode);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_MUSIC_GAIN,settingsSourceNode);
  }
  if ((audioFlags & (PERSISTENT_SOUND_OPTION_EFFECTS | PERSISTENT_SOUND_OPTION_MUSIC)) == 0) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_REVERSE_STEREO,settingsSourceNode);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_REVERSE_STEREO,settingsSourceNode);
  }
}

/* Handler of the graphics settings page's shading toggle (shadingEnabledCheckbox, action 0x2014, slot 20 of
   g_FrontendUiActionHandlersPage20): offers the shading levels only while shading is on and saves the state as
   PERSISTENT_SETTING_SHADING_ENABLED.
*/
void FrontendShadingSettings_SetEnabled(UiSelectableControl *control)

{
  uint8_t isSelected;
  UiNodeBase *parentCursor;

  isSelected = UiSelectableControl_IsSelected(control);
  parentCursor = control->base.parent;
  /* climb to the root of the control's UI tree */
  while (parentCursor != UI_NODE_NONE) {
    control = (UiSelectableControl *)(control->base).parent;
    parentCursor = control->base.parent;
  }
  if ((isSelected & 1) == 0) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_SHADING_LEVEL,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_SHADING_LEVEL,&control->base);
  }
  PersistentSettings_Write(isSelected & 1,PERSISTENT_SETTING_SHADING_ENABLED);
}

/* Handler of the six shading level choices (FRONTEND_ACTION_SHADING_LEVEL, slot 21 of
   g_FrontendUiActionHandlersPage20): saves the clicked button's grid size as the shading grid half size, twice
   it as the shading texture dimension and its depth / 4 as the subresource count, then selects the matching
   choice exclusively.
*/
void FrontendShadingSettings_ApplyLevel(UiSelectableControl *control)

{
  int32_t shadingGridSize;
  FrontendShadingLevelGroup *shadingLevelGroup;
  uint32_t shadingDepthQuarter;
  UiNodeBase *selectedControl;

  shadingGridSize = ((UiNumericPairTextButton *)control)->firstValue;
  shadingDepthQuarter = (uint32_t)((UiNumericPairTextButton *)control)->secondValue >> 2;
  PersistentSettings_Write((int)shadingGridSize * 2,PERSISTENT_SETTING_SHADING_TEXTURE_DIMENSION);
  PersistentSettings_Write((PersistentSettingsValue)shadingGridSize,PERSISTENT_SETTING_SHADING_GRID_HALF_SIZE);
  PersistentSettings_Write(shadingDepthQuarter,PERSISTENT_SETTING_SHADING_SUBRESOURCE_COUNT);
  /* The parent is the frontend template's shadingLevelGroup. */
  shadingLevelGroup = (FrontendShadingLevelGroup *)(control->base).parent;
  if (shadingGridSize == 32) {
    selectedControl = (UiNodeBase *)&shadingLevelGroup->levels[0] /* shadingLevelGrid32Depth32 */;
    if (shadingDepthQuarter == 64 / 4) {
      selectedControl = (UiNodeBase *)&shadingLevelGroup->levels[1] /* shadingLevelGrid32Depth64 */;
    }
    else if (shadingDepthQuarter == 128 / 4) {
      selectedControl = (UiNodeBase *)&shadingLevelGroup->levels[2] /* shadingLevelGrid32Depth128 */;
    }
  }
  else if (shadingGridSize == 64) {
    selectedControl = (UiNodeBase *)&shadingLevelGroup->levels[3] /* shadingLevelGrid64Depth64 */;
    if (shadingDepthQuarter == 128 / 4) {
      selectedControl = (UiNodeBase *)&shadingLevelGroup->levels[4] /* shadingLevelGrid64Depth128 */;
    }
  }
  else {
    selectedControl = (UiNodeBase *)&shadingLevelGroup->levels[5] /* shadingLevelGrid128Depth128 */;
  }
  UiSelectableGroup_SelectExclusive(6,selectedControl,
      (UiNodeBase *)&((FrontendShadingLevelGroup *)(control->base).parent)->levels[5],
      (UiNodeBase *)&((FrontendShadingLevelGroup *)(control->base).parent)->levels[4],
      (UiNodeBase *)&((FrontendShadingLevelGroup *)(control->base).parent)->levels[3],
      (UiNodeBase *)&((FrontendShadingLevelGroup *)(control->base).parent)->levels[2],
      (UiNodeBase *)&((FrontendShadingLevelGroup *)(control->base).parent)->levels[1],
      (UiNodeBase *)&((FrontendShadingLevelGroup *)(control->base).parent)->levels[0]);
}

/* Handler of the graphics settings page's polygon detail slider (polygonDetailSlider, action 0x2016, slot 22 of
   g_FrontendUiActionHandlersPage20): saves the Q8 model LOD depth threshold as
   PERSISTENT_SETTING_MODEL_LOD_DEPTH_THRESHOLD and applies it at once (g_ModelLodDepthThresholdQ8).
*/
void FrontendModelSettings_SetLodDepthThresholdQ8(UiSettingsValueControl *control)

{
  g_ModelLodDepthThresholdQ8 = PersistentOption_StoreSlider(control,PERSISTENT_SETTING_MODEL_LOD_DEPTH_THRESHOLD);
}

/* Handler of the three texture quality choices (action 0x2017, slot 23 of g_FrontendUiActionHandlersPage20):
   selects the clicked one, saves it as PERSISTENT_SETTING_TEXTURE_QUALITY (high 0, medium 1, low 2) and applies
   it at once: the downsample shift becomes level / 2 (only low halves the textures) and all staging textures
   are rebuilt.
*/
void FrontendTextureSettings_SetQuality(UiSelectableControl *control)

{
  PersistentTextureQualityLevel qualityLevel;
  UiNodeBase *selectedQualityControl = nullptr; /* control is one of the three buttons */
  FrontendTextureQualityGroup *textureQualityGroup;

  /* The parent is the frontend template's textureQualityGroup. */
  textureQualityGroup = (FrontendTextureQualityGroup *)(control->base).parent;
  if (&textureQualityGroup->low.selectable == control) {
    qualityLevel = TEXTURE_QUALITY_LOW;
    selectedQualityControl = (UiNodeBase *)&textureQualityGroup->low;
  }
  if (&textureQualityGroup->medium.selectable == control) {
    qualityLevel = TEXTURE_QUALITY_MEDIUM;
    selectedQualityControl = (UiNodeBase *)&textureQualityGroup->medium;
  }
  if (&textureQualityGroup->high.selectable == control) {
    qualityLevel = TEXTURE_QUALITY_HIGH;
    selectedQualityControl = (UiNodeBase *)&textureQualityGroup->high;
  }
  UiSelectableGroup_SelectExclusive(3,selectedQualityControl,
      (UiNodeBase *)&((FrontendTextureQualityGroup *)(control->base).parent)->high,
      (UiNodeBase *)&((FrontendTextureQualityGroup *)(control->base).parent)->medium,
      (UiNodeBase *)&((FrontendTextureQualityGroup *)(control->base).parent)->low);
  PersistentSettings_Write(qualityLevel,PERSISTENT_SETTING_TEXTURE_QUALITY);
  g_TextureDownsampleShift = qualityLevel >> 1;
}

/* Handler of the audio settings page's effects toggle (soundEffectsEnabledCheckbox, action 0x2018, slot 24 of
   g_FrontendUiActionHandlersPage20): saves PERSISTENT_SOUND_OPTION_EFFECTS, offers the effect and movie volume
   sliders only while effects are on (the music slider and reverse stereo follow the saved music bit), and
   applies the saved effect, UI and movie gains, or silence while effects are off.
*/
void FrontendAudioSettings_SetEffectsEnabled(UiSelectableControl *control)

{
  UiNodeBase *parentCursor;
  uint32_t audioFlags;
  AudioMixerGainQ15 effectsGain;
  MovieAudioGainQ15 movieDefaultGain;
  MovieAudioGainQ15 movieAlternateGain;
  Bool8 isSelected;

  isSelected = (Bool8)UiSelectableControl_IsSelected(control);
  audioFlags = PersistentSettings_Read(PERSISTENT_SOUND_OPTION_DEFAULT,PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
  /* isSelected is the PERSISTENT_SOUND_OPTION_EFFECTS bit */
  PersistentSettings_Write((uint32_t)isSelected | audioFlags & ~PERSISTENT_SOUND_OPTION_EFFECTS,
                           PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
  parentCursor = control->base.parent;
  /* climb to the root of the control's UI tree */
  while (parentCursor != UI_NODE_NONE) {
    control = (UiSelectableControl *)(control->base).parent;
    parentCursor = control->base.parent;
  }
  if (isSelected) {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_EFFECTS_GAIN,&control->base);
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_MOVIE_GAIN,&control->base);
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_MOVIE_EVENT_GAIN,&control->base);
  }
  else {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_EFFECTS_GAIN,&control->base);
    UiNodeList_SuppressActionId(FRONTEND_ACTION_MOVIE_GAIN,&control->base);
    UiNodeList_SuppressActionId(FRONTEND_ACTION_MOVIE_EVENT_GAIN,&control->base);
  }
  if ((audioFlags & PERSISTENT_SOUND_OPTION_MUSIC) == 0) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_MUSIC_GAIN,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_MUSIC_GAIN,&control->base);
  }
  if (isSelected == 0 && (audioFlags & PERSISTENT_SOUND_OPTION_MUSIC) == 0) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_REVERSE_STEREO,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_REVERSE_STEREO,&control->base);
  }
  effectsGain = 0;
  if (isSelected) {
    effectsGain = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_EFFECTS_GAIN);
  }
  movieDefaultGain = 0;
  g_UiSoundGainQ15 = effectsGain;
  g_SoundEffectsGainQ15 = effectsGain;
  if (isSelected) {
    movieDefaultGain = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_MOVIE_DEFAULT_GAIN);
  }
  movieAlternateGain = 0;
  g_MovieDefaultAudioGainQ15 = movieDefaultGain;
  if (isSelected) {
    movieAlternateGain = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_MOVIE_ALTERNATE_GAIN);
  }
  g_MovieAlternateAudioGainQ15 = movieAlternateGain;
}

/* Handler of the audio settings page's music toggle (musicEnabledCheckbox, action 0x2019, slot 25 of
   g_FrontendUiActionHandlersPage20). Switching on loads sound\music00.sam and starts it looping at the saved
   music gain (busy cursor meanwhile; any failure just leaves the music off); switching off stops and releases
   it. Then saves PERSISTENT_SOUND_OPTION_MUSIC and offers the volume sliders and reverse stereo accordingly.
*/
void FrontendAudioSettings_SetMusicEnabled(UiSelectableControl *control)

{
  UiNodeBase *parentCursor;
  uint32_t savedAudioFlags;
  uint32_t musicEnabledBit;
  uint32_t newAudioFlags;
  Bool8 isSelected;

  musicEnabledBit = 0;
  isSelected = (Bool8)UiSelectableControl_IsSelected(control);
  if (isSelected) {
    musicEnabledBit = PERSISTENT_SOUND_OPTION_MUSIC;
    g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_BUSY);
    FrontendMusic_StartMenuMusic();
    g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_ARROW);
  }
  else {
    g_SoundStopVoice(g_FrontendMusicActiveBuffer);
    g_SoundReleaseSampleVoiceSet(g_FrontendMusicVoiceSet);
    g_FrontendMusicActiveBuffer = nullptr;
    g_FrontendMusicVoiceSet = nullptr;
  }
  savedAudioFlags = PersistentSettings_Read(PERSISTENT_SOUND_OPTION_DEFAULT,PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
  newAudioFlags = musicEnabledBit | savedAudioFlags & ~PERSISTENT_SOUND_OPTION_MUSIC;
  PersistentSettings_Write(newAudioFlags,PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
  parentCursor = control->base.parent;
  /* climb to the root of the control's UI tree */
  while (parentCursor != UI_NODE_NONE) {
    control = (UiSelectableControl *)(control->base).parent;
    parentCursor = control->base.parent;
  }
  if ((newAudioFlags & PERSISTENT_SOUND_OPTION_EFFECTS) == 0) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_EFFECTS_GAIN,&control->base);
    UiNodeList_SuppressActionId(FRONTEND_ACTION_MOVIE_GAIN,&control->base);
    UiNodeList_SuppressActionId(FRONTEND_ACTION_MOVIE_EVENT_GAIN,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_EFFECTS_GAIN,&control->base);
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_MOVIE_GAIN,&control->base);
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_MOVIE_EVENT_GAIN,&control->base);
  }
  if ((musicEnabledBit & PERSISTENT_SOUND_OPTION_MUSIC) == 0) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_MUSIC_GAIN,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_MUSIC_GAIN,&control->base);
  }
  if ((newAudioFlags & (PERSISTENT_SOUND_OPTION_EFFECTS | PERSISTENT_SOUND_OPTION_MUSIC)) == 0) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_REVERSE_STEREO,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_REVERSE_STEREO,&control->base);
  }
}

/* Handler of the audio settings page's reverse stereo toggle (FRONTEND_ACTION_REVERSE_STEREO, slot 26 of
   g_FrontendUiActionHandlersPage20): applies it at once (g_ReverseStereoMask all ones or zero) and saves
   PERSISTENT_SOUND_OPTION_REVERSE_STEREO.
*/
void FrontendAudioSettings_SetReverseStereo(UiSelectableControl *control)

{
  PersistentOption_ApplyCheckbox(control,PERSISTENT_SETTING_SOUND_OPTION_FLAGS,PERSISTENT_SOUND_OPTION_REVERSE_STEREO,
                                 [](Bool8 isSelected) { g_ReverseStereoMask = isSelected ? -1 : 0; },
                                 PERSISTENT_SOUND_OPTION_DEFAULT);
}

/* Handler of the effects volume slider (FRONTEND_ACTION_EFFECTS_GAIN, slot 27 of
   g_FrontendUiActionHandlersPage20): saves the Q15 gain as PERSISTENT_SETTING_EFFECTS_GAIN and applies it at
   once to the sound effects and the UI sounds.
*/
void FrontendAudioSettings_SetEffectsGain(UiSettingsValueControl *control)

{
  PersistentSettingsValue value;

  value = PersistentOption_StoreSlider(control,PERSISTENT_SETTING_EFFECTS_GAIN);
  g_SoundEffectsGainQ15 = value;
  g_UiSoundGainQ15 = value;
}

/* Handler of the movie volume slider (FRONTEND_ACTION_MOVIE_GAIN, slot 28 of g_FrontendUiActionHandlersPage20):
   saves the Q15 gain as PERSISTENT_SETTING_MOVIE_DEFAULT_GAIN and applies it at once.
*/
void FrontendAudioSettings_SetMovieDefaultGain(UiSettingsValueControl *control)

{
  g_MovieDefaultAudioGainQ15 = PersistentOption_StoreSlider(control,PERSISTENT_SETTING_MOVIE_DEFAULT_GAIN);
}

/* Handler of the movie event volume slider (FRONTEND_ACTION_MOVIE_EVENT_GAIN, slot 78 of
   g_FrontendUiActionHandlersPage20): saves the Q15 gain used by timed movie events as
   PERSISTENT_SETTING_MOVIE_ALTERNATE_GAIN and applies it at once.
*/
void FrontendAudioSettings_SetMovieAlternateGain(UiSettingsValueControl *control)

{
  g_MovieAlternateAudioGainQ15 = PersistentOption_StoreSlider(control,PERSISTENT_SETTING_MOVIE_ALTERNATE_GAIN);
}

/* Handler of the music volume slider (FRONTEND_ACTION_MUSIC_GAIN, slot 29 of g_FrontendUiActionHandlersPage20):
   saves the Q15 gain as PERSISTENT_SETTING_MUSIC_GAIN and sets it as left and right gain of the playing frontend
   music.
*/
void FrontendAudioSettings_SetMusicGain(UiSettingsValueControl *control)

{
  PersistentSettingsValue value;

  value = PersistentOption_StoreSlider(control,PERSISTENT_SETTING_MUSIC_GAIN);
  g_SoundSetVoiceGains(value,value,g_FrontendMusicActiveBuffer);
}
