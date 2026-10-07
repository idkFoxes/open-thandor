/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/settings.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/settings.h>
#include <thandor/thandor.h>
#include <thandor/ui/controls/settings_option.h>

/* UI action 0x1020 (missionHelpCloseButton; g_InGameUiActionHandlersPage10[32]): closes the mission help
   window by releasing the missionObjectivesButton toggle and running the settings page toggle on it, which
   hides the window and resumes a game that the window paused.
*/
void InGameSettingsAction_CloseAlternatePanel(UiNodeBase *source)

{

  while (source->parent != UI_NODE_NONE) {
    source = source->parent;
  }
  /* source is now the in-game UI root (template start) */
  UiSelectableControl_SetSelected(0,&InGameUi_Image(source)->missionObjectivesButton.selectable);
  InGameSettingsPage_ToggleAndSynchronizeControls
            (&InGameUi_Image(source)->missionObjectivesButton.selectable);
}

/* UI action 0x1201 (gameMenuCloseButton; g_InGameUiActionHandlersPage12[1]): closes the game menu by
   releasing the inGameMenuButton toggle and running its toggle handler, which also resumes a paused game.
*/
void InGameSettingsPage_CloseViaSharedToggle(UiNodeBase *source)

{

  while (source->parent != UI_NODE_NONE) {
    source = source->parent;
  }
  UiSelectableControl_SetSelected(0,&InGameUi_Image(source)->inGameMenuButton.selectable);
  InGameSettingsPage_ToggleAndSynchronizeControls(&InGameUi_Image(source)->inGameMenuButton.selectable);
}

/* UI action 0x1218 (the Back buttons of the save, quit, graphics and sound pages;
   g_InGameUiActionHandlersPage12[24]): selects the inGameMenuButton toggle again and runs its toggle handler,
   which returns to the game menu page with freshly loaded gameplay options.
*/
void InGameSettingsPage_OpenViaSharedToggle(UiNodeBase *source)

{

  while (source->parent != UI_NODE_NONE) {
    source = source->parent;
  }
  UiSelectableControl_SetSelected(1,&InGameUi_Image(source)->inGameMenuButton.selectable);
  InGameSettingsPage_ToggleAndSynchronizeControls(&InGameUi_Image(source)->inGameMenuButton.selectable);
}

/* In-game command handler (keys G / Alt+G): changes the player's simulation step batch by stepDelta, kept within
   1..INGAME_SIMULATION_STEP_TICKS_MAX, and sets g_InGameSimulationStepTicks to the smallest batch of all players,
   so the slowest request wins in a network game.
*/
void InGameSimulationSpeed_AdjustPlayerAndRecomputeMinimumTicks
          (FrontendPlayerRuntimeId playerRuntimeId,uint32_t reservedZero0,uint32_t reservedZero1,
          int stepDelta)

{
  FrontendPlayerRuntimeBlockCount remainingCount;
  FrontendPlayerRuntimeRecord *playerRecord;
  InGameSimulationStepBatchTicks stepTicks;
  
  stepTicks = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->simulationStepTicks + stepDelta;
  if ((stepTicks != 0) && (stepTicks < INGAME_SIMULATION_STEP_TICKS_MAX + 1)) {
    g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->simulationStepTicks = stepTicks;
    remainingCount = g_FrontendPlayerRuntimeBlockCount;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
    /* Original quirk: a do/while, so a count of 0 runs it 2^32 times (kept as in the original; step 11). */
    do {
      if (g_SelectionPlayerRuntimeBlockPointers[playerRecord->playerRuntimeId]->simulationStepTicks <
          stepTicks) {
        stepTicks = g_SelectionPlayerRuntimeBlockPointers[playerRecord->playerRuntimeId]->simulationStepTicks;
      }
      playerRecord++;
      remainingCount--;
      g_InGameSimulationStepTicks = stepTicks;
    } while (remainingCount != 0);
  }
}

/* UI action 0x1216 (rightButtonNoScrollCheckbox; g_InGameUiActionHandlersPage12[22]): stores
   PERSISTENT_MAP_OPTION_SIDE_PANEL_HIDDEN and applies it at once. Although the checkbox text reads "right
   button does not scroll", the effect is to hide the side panel (side panel stack page 1) and widen the
   world view to the right screen edge; clearing it restores the panel and the view's right edge.
*/
void InGameGameplaySettings_SetRightButtonDoesNotScroll(UiSelectableControl *control)

{
  PersistentOption_ApplyCheckbox(control,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS,PERSISTENT_MAP_OPTION_SIDE_PANEL_HIDDEN,
                                 [control](Bool8 isSelected) {
    /* control is rightButtonNoScrollCheckbox of the in-game UI template copy; the page stacks and the layout
       change before the option word is written */
    InGameUiImage *image = THANDOR_CONTAINER_OF(control, InGameUiImage, rightButtonNoScrollCheckbox);
    UiPageStack_SetActiveIndex
              (isSelected ? 1 : 0,
               UiLayoutContainerControl_AsPageStack(&image->sidePanelStack));
    UiPageStack_SetActiveIndex
              (0,UiLayoutContainerControl_AsPageStack(&image->resourceBarModeStack));
    UiPageStack_SetActiveIndex
              (0,UiLayoutContainerControl_AsPageStack(&image->gamePanelsModeStack));
    image->worldViewArea.base.rightOffset =
         isSelected ? 0
                    : image->sidePanelFrameLeftEdge.base.leftOffset;
    UiContainer_LayoutChildren(&image->inGameRootPanel.root.base);
  });
}

/* UI action 0x1217 (scrollSpeedSlider; g_InGameUiActionHandlersPage12[23]): stores the slider value as
   PERSISTENT_SETTING_CAMERA_SCROLL_STEP; the camera reads the setting whenever it scrolls.
*/
void InGameGameplaySettings_SetCameraScrollStep(UiSettingsValueControl *control)

{
  PersistentOption_StoreSlider(control,PERSISTENT_SETTING_CAMERA_SCROLL_STEP);
}

/* UI action 0x1212 (autoZoomOffCheckbox; g_InGameUiActionHandlersPage12[18]): stores
   PERSISTENT_MAP_OPTION_AUTOMATIC_ZOOM_OFF. Switching automatic zoom off also resets the minimap to its
   default scale.
*/
void InGameGameplaySettings_SetAutomaticZoomOff(UiSelectableControl *control)

{
  PersistentOption_ApplyCheckbox(control,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS,PERSISTENT_MAP_OPTION_AUTOMATIC_ZOOM_OFF,
                                 [control](Bool8 isSelected) {
    if (isSelected) {
      /* control is autoZoomOffCheckbox; reset the minimap zoom */
      THANDOR_CONTAINER_OF(control, InGameUiImage, autoZoomOffCheckbox)->minimapView.sampleScaleQ12 =
           INGAME_MINIMAP_DEFAULT_SCALE_Q12;
    }
  });
}

/* UI action 0x1213 (autoRotationOffCheckbox; g_InGameUiActionHandlersPage12[19]): stores
   PERSISTENT_MAP_OPTION_AUTOMATIC_ROTATION_OFF. Switching automatic rotation off also turns the minimap back
   to its default angle.
*/
void InGameGameplaySettings_SetAutomaticRotationOff(UiSelectableControl *control)

{
  PersistentOption_ApplyCheckbox(control,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS,PERSISTENT_MAP_OPTION_AUTOMATIC_ROTATION_OFF,
                                 [control](Bool8 isSelected) {
    if (isSelected) {
      /* control is autoRotationOffCheckbox; reset the minimap rotation to its default angle */
      THANDOR_CONTAINER_OF(control, InGameUiImage, autoRotationOffCheckbox)->minimapView.
      rotationAngle = INGAME_MINIMAP_DEFAULT_ROTATION_ANGLE;
    }
  });
}

/* UI action 0x1214 (INGAME_ACTION_LINK_ROTATION_ZOOM, linkRotationZoomCheckbox;
   g_InGameUiActionHandlersPage12[20]): stores PERSISTENT_LINK_OPTION_ROTATION_ZOOM and mirrors it to the world
   view. The rotation can only be linked to zoom or to tilt, so the tilt checkbox is disabled while this is on.
*/
void InGameGameplaySettings_SetLinkRotationZoom(UiSelectableControl *control)

{
  PersistentOption_ApplyCheckbox(control,PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS,PERSISTENT_LINK_OPTION_ROTATION_ZOOM,
                                 [control](Bool8 isSelected) {
    WorldRuntimeFlags *runtimeFlagsField;

    /* control is linkRotationZoomCheckbox; the world view's runtime flags */
    runtimeFlagsField =
         &InGameUi_WorldRuntime(THANDOR_CONTAINER_OF(control, InGameUiImage, linkRotationZoomCheckbox))->runtimeFlags;
    if (isSelected) {
      *runtimeFlagsField = *runtimeFlagsField | WORLD_RUNTIME_FLAG_LINK_ROTATION_ZOOM;
      UiNodeList_SuppressActionId(INGAME_ACTION_LINK_ROTATION_TILT,(control->base).parent);
    }
    else {
      *runtimeFlagsField = *runtimeFlagsField & ~WORLD_RUNTIME_FLAG_LINK_ROTATION_ZOOM;
      UiNodeList_UnsuppressActionId(INGAME_ACTION_LINK_ROTATION_TILT,(control->base).parent);
    }
  });
}

/* UI action 0x1215 (INGAME_ACTION_LINK_ROTATION_TILT, linkRotationTiltCheckbox;
   g_InGameUiActionHandlersPage12[21]): stores PERSISTENT_LINK_OPTION_ROTATION_TILT and mirrors it to the world
   view; the zoom link checkbox is disabled while this is on, as the two links exclude each other.
*/
void InGameGameplaySettings_SetLinkRotationTilt(UiSelectableControl *control)

{
  PersistentOption_ApplyCheckbox(control,PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS,PERSISTENT_LINK_OPTION_ROTATION_TILT,
                                 [control](Bool8 isSelected) {
    WorldRuntimeFlags *runtimeFlagsField;

    /* control is linkRotationTiltCheckbox; the world view's runtime flags */
    runtimeFlagsField =
         &InGameUi_WorldRuntime(THANDOR_CONTAINER_OF(control, InGameUiImage, linkRotationTiltCheckbox))->runtimeFlags;
    if (isSelected) {
      *runtimeFlagsField = *runtimeFlagsField | WORLD_RUNTIME_FLAG_LINK_ROTATION_TILT;
      UiNodeList_SuppressActionId(INGAME_ACTION_LINK_ROTATION_ZOOM,(control->base).parent);
    }
    else {
      *runtimeFlagsField = *runtimeFlagsField & ~WORLD_RUNTIME_FLAG_LINK_ROTATION_TILT;
      UiNodeList_UnsuppressActionId(INGAME_ACTION_LINK_ROTATION_ZOOM,(control->base).parent);
    }
  });
}

/* UI action 0x121B (hidePanelCheckbox; g_InGameUiActionHandlersPage12[27]): stores
   PERSISTENT_LINK_OPTION_HIDE_PANEL and mirrors it to the world view's WORLD_RUNTIME_FLAG_HIDE_PANEL. The name
   follows the checkbox label; the world view tests the flag together with the left mouse button.
*/
void InGameGameplaySettings_SetHidePanel(UiSelectableControl *control)

{
  PersistentOption_ApplyCheckbox(control,PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS,PERSISTENT_LINK_OPTION_HIDE_PANEL,
                                 [control](Bool8 isSelected) {
    WorldRuntimeFlags *runtimeFlagsField;

    /* control is hidePanelCheckbox; the world view's runtime flags */
    runtimeFlagsField =
         &InGameUi_WorldRuntime(THANDOR_CONTAINER_OF(control, InGameUiImage, hidePanelCheckbox))->runtimeFlags;
    if (isSelected) {
      *runtimeFlagsField = *runtimeFlagsField | WORLD_RUNTIME_FLAG_HIDE_PANEL;
    }
    else {
      *runtimeFlagsField = *runtimeFlagsField & ~WORLD_RUNTIME_FLAG_HIDE_PANEL;
    }
  });
}

/* UI action 0x1202 (gameMenuGraphicsButton; g_InGameUiActionHandlersPage12[2]): opens the graphics settings
   window (page 6) and loads its controls from the persistent settings: shading on/off, the shading level
   button matching the stored grid size and depth, the texture quality and the model detail slider. The
   shading level buttons are only usable with shading on, texture quality only in a local game.
*/
void InGameGraphicsSettings_OpenAndSynchronize(UiNodeBase *graphicsButton)

{
  UiNodeBase *uiRoot;
  uint32_t settingValue;
  uint32_t storedSubresourceCount;
  int shadingDepth;
  UiNodeBase *selectedButton;

  /* graphicsButton is gameMenuGraphicsButton of the in-game UI template copy */
  InGameUiImage *image = THANDOR_CONTAINER_OF(graphicsButton, InGameUiImage, gameMenuGraphicsButton);
  UiPageStack_SetActiveIndex(INGAME_WINDOW_PAGE_GRAPHICS_SETTINGS,UiLayoutContainerControl_AsPageStack(&image->gameWindowPageStack));
  settingValue = PersistentSettings_Read(1,PERSISTENT_SETTING_SHADING_ENABLED);
  UiSelectableControl_SetSelected(settingValue,&image->shadingEnabledCheckbox.selectable);
  uiRoot = graphicsButton;
  while (uiRoot->parent != UI_NODE_NONE) {
    uiRoot = uiRoot->parent;
  }
  if (settingValue == 0) {
    UiNodeList_SuppressActionId(INGAME_ACTION_SHADING_LEVEL,uiRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(INGAME_ACTION_SHADING_LEVEL,uiRoot);
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    UiNodeList_UnsuppressActionId(INGAME_ACTION_TEXTURE_QUALITY,uiRoot);
  }
  else {
    UiNodeList_SuppressActionId(INGAME_ACTION_TEXTURE_QUALITY,uiRoot);
  }
  settingValue = PersistentSettings_Read(PERSISTENT_DEFAULT_SHADING_GRID_HALF_SIZE,PERSISTENT_SETTING_SHADING_GRID_HALF_SIZE);
  storedSubresourceCount = PersistentSettings_Read(PERSISTENT_DEFAULT_SHADING_SUBRESOURCE_COUNT,
                                                  PERSISTENT_SETTING_SHADING_SUBRESOURCE_COUNT);
  /* the buttons name the depth, the setting stores a quarter of it */
  shadingDepth = storedSubresourceCount * 4;
  if (settingValue == 32) {
    selectedButton = &image->shadingLevel32x32Button.base.selectable.base;
    if (shadingDepth == 64) {
      selectedButton = &image->shadingLevel32x64Button.base.selectable.base;
    }
    else if (shadingDepth == 128) {
      selectedButton = &image->shadingLevel32x128Button.base.selectable.base;
    }
  }
  else if (settingValue == 64) {
    selectedButton = &image->shadingLevel64x64Button.base.selectable.base;
    if (shadingDepth == 128) {
      selectedButton = &image->shadingLevel64x128Button.base.selectable.base;
    }
  }
  else {
    selectedButton = &image->shadingLevel128x128Button.base.selectable.base;
  }
  UiSelectableGroup_SelectExclusive(6,selectedButton,
      &image->shadingLevel128x128Button.base.selectable.base,
      &image->shadingLevel64x128Button.base.selectable.base,
      &image->shadingLevel64x64Button.base.selectable.base,
      &image->shadingLevel32x128Button.base.selectable.base,
      &image->shadingLevel32x64Button.base.selectable.base,
      &image->shadingLevel32x32Button.base.selectable.base);
  settingValue = PersistentSettings_Read(TEXTURE_QUALITY_MEDIUM,PERSISTENT_SETTING_TEXTURE_QUALITY);
  if (settingValue == TEXTURE_QUALITY_HIGH) {
    selectedButton = &image->textureQualityHighButton.selectable.base;
  }
  else if (settingValue == TEXTURE_QUALITY_MEDIUM) {
    selectedButton = &image->textureQualityMediumButton.selectable.base;
  }
  else {
    selectedButton = &image->textureQualityLowButton.selectable.base;
  }
  UiSelectableGroup_SelectExclusive(3,selectedButton,
      &image->textureQualityHighButton.selectable.base,
      &image->textureQualityMediumButton.selectable.base,
      &image->textureQualityLowButton.selectable.base);
  settingValue = PersistentSettings_Read(PERSISTENT_DEFAULT_MODEL_LOD_DEPTH_THRESHOLD,
                                         PERSISTENT_SETTING_MODEL_LOD_DEPTH_THRESHOLD);
  image->modelDetailSlider.value = settingValue;
}

/* UI action 0x1203 (gameMenuAudioButton; g_InGameUiActionHandlersPage12[3]): opens the sound settings window
   (page 7) and loads the three sound switches and four volume sliders from the persistent settings. The
   effects and movie sliders only work with effects on, the music slider only with music on, and reverse
   stereo only while any sound is on.
*/
void InGameAudioSettings_OpenAndSynchronize(InGamePersistentSettingsPageSourceNodePtr settingsSourceNode)

{
  uint32_t audioFlags;
  uint32_t gainQ15;

  InGamePersistentSettingsPage3508 *audioPage =
       THANDOR_CONTAINER_OF(settingsSourceNode, InGamePersistentSettingsPage3508, sourceNode);

  UiPageStack_SetActiveIndex(INGAME_WINDOW_PAGE_SOUND_SETTINGS,&audioPage->settingsPageStack);
  audioFlags = PersistentSettings_Read(PERSISTENT_SOUND_OPTION_DEFAULT,PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
  UiSelectableControl_SetSelected(audioFlags & PERSISTENT_SOUND_OPTION_EFFECTS,&audioPage->soundEffectsEnabledControl);
  UiSelectableControl_SetSelected(audioFlags & PERSISTENT_SOUND_OPTION_MUSIC,&audioPage->musicEnabledControl);
  UiSelectableControl_SetSelected(audioFlags & PERSISTENT_SOUND_OPTION_REVERSE_STEREO,&audioPage->reverseStereoControl);
  gainQ15 = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_EFFECTS_GAIN);
  (audioPage->soundEffectsGainControl).currentValue = gainQ15;
  gainQ15 = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_MOVIE_DEFAULT_GAIN);
  (audioPage->movieDefaultAudioGainControl).currentValue = gainQ15;
  gainQ15 = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_MOVIE_ALTERNATE_GAIN);
  (audioPage->movieAlternateAudioGainControl).currentValue = gainQ15;
  gainQ15 = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_MUSIC_GAIN);
  (audioPage->musicGainControl).currentValue = gainQ15;
  while (settingsSourceNode->parent != UI_NODE_NONE) {
    settingsSourceNode = settingsSourceNode->parent;
  }
  if ((audioFlags & PERSISTENT_SOUND_OPTION_EFFECTS) == 0) {
    UiNodeList_SuppressActionId(INGAME_ACTION_EFFECTS_VOLUME,settingsSourceNode);
    UiNodeList_SuppressActionId(INGAME_ACTION_MOVIE_VOLUME,settingsSourceNode);
    UiNodeList_SuppressActionId(INGAME_ACTION_MESSAGE_MOVIE_VOLUME,settingsSourceNode);
  }
  else {
    UiNodeList_UnsuppressActionId(INGAME_ACTION_EFFECTS_VOLUME,settingsSourceNode);
    UiNodeList_UnsuppressActionId(INGAME_ACTION_MOVIE_VOLUME,settingsSourceNode);
    UiNodeList_UnsuppressActionId(INGAME_ACTION_MESSAGE_MOVIE_VOLUME,settingsSourceNode);
  }
  if ((audioFlags & PERSISTENT_SOUND_OPTION_MUSIC) == 0) {
    UiNodeList_SuppressActionId(INGAME_ACTION_MUSIC_VOLUME,settingsSourceNode);
  }
  else {
    UiNodeList_UnsuppressActionId(INGAME_ACTION_MUSIC_VOLUME,settingsSourceNode);
  }
  if ((audioFlags & (PERSISTENT_SOUND_OPTION_EFFECTS | PERSISTENT_SOUND_OPTION_MUSIC)) == 0) {
    UiNodeList_SuppressActionId(INGAME_ACTION_REVERSE_STEREO,settingsSourceNode);
  }
  else {
    UiNodeList_UnsuppressActionId(INGAME_ACTION_REVERSE_STEREO,settingsSourceNode);
  }
}

/* UI action 0x1204 (shadingEnabledCheckbox; g_InGameUiActionHandlersPage12[4]): stores the shading switch,
   mirrors it to the world view's WORLD_RUNTIME_FLAG_SHADING_ENABLED and enables the shading level buttons
   only while shading is on.
*/
void InGameShadingSettings_SetEnabled(UiSelectableControl *control)

{
  uint8_t selectedState;

  selectedState = UiSelectableControl_IsSelected(control);
  UiNodeBase *uiRoot = &control->base;
  while (uiRoot->parent != UI_NODE_NONE) {
    uiRoot = uiRoot->parent;
  }
  WorldRuntimeContext *world = InGameUi_WorldRuntime(uiRoot);
  if ((selectedState & 1) == 0) {
    UiNodeList_SuppressActionId(INGAME_ACTION_SHADING_LEVEL,uiRoot);
    world->runtimeFlags =
         world->runtimeFlags & ~WORLD_RUNTIME_FLAG_SHADING_ENABLED;
  }
  else {
    UiNodeList_UnsuppressActionId(INGAME_ACTION_SHADING_LEVEL,uiRoot);
    world->runtimeFlags =
         world->runtimeFlags | WORLD_RUNTIME_FLAG_SHADING_ENABLED;
  }
  PersistentSettings_Write(selectedState & 1,PERSISTENT_SETTING_SHADING_ENABLED);
}

/* UI action 0x1205 (INGAME_ACTION_SHADING_LEVEL, the six shading level buttons;
   g_InGameUiActionHandlersPage12[5]): rebuilds the generated shading texture for the button's grid size and
   depth. When that succeeds the level is stored and its button selected; when it fails the error is
   reported and the texture is rebuilt from the previously stored level.
*/
void InGameShadingSettings_ApplyLevel(UiSelectableControl *control)

{
  UiNodeBase *shadingLevelGroup;
  uint32_t subresourceCount;
  int shadingDepth;
  uint32_t textureDimension;
  uint32_t gridHalfSize;
  uint32_t storedSubresourceCount;
  PersistentSettingsValue newGridHalfSize;
  PersistentSettingsValue newTextureDimension;
  UiNodeBase *selectedControl;
  uint32_t initError;
  uint32_t newSubresourceCount;

  /* control is one of the shading level buttons (numeric pair text buttons) */
  UiNumericPairTextButton *levelButton = THANDOR_CONTAINER_OF(control, UiNumericPairTextButton, base.selectable);

  /* the button's value pair is (grid half size, depth); the depth is stored as a quarter */
  subresourceCount = (uint32_t)levelButton->secondValue >> 2;
  newSubresourceCount = subresourceCount;
  /* The option control stores the grid half size in firstValue; the texture dimension is twice
     that. */
  newGridHalfSize = (PersistentSettingsValue)levelButton->firstValue;
  newTextureDimension = newGridHalfSize * 2;
  GraphicsShadingRuntime_Shutdown();
  initError = GraphicsShadingRuntime_InitializeGeneratedTexture
                    (subresourceCount,newGridHalfSize,newTextureDimension);
  FatalError_ReportIfFailed(initError,initError != 0); /* reports and returns: the flag is ours */
  if (initError == 0) {
    PersistentSettings_Write(newTextureDimension,PERSISTENT_SETTING_SHADING_TEXTURE_DIMENSION);
    PersistentSettings_Write(newGridHalfSize,PERSISTENT_SETTING_SHADING_GRID_HALF_SIZE);
    PersistentSettings_Write(newSubresourceCount,PERSISTENT_SETTING_SHADING_SUBRESOURCE_COUNT);
    shadingDepth = newSubresourceCount << 2;
    /* control is one of the shading level buttons, its parent is shadingLevelGroup */
    shadingLevelGroup = (control->base).parent;
    InGameUiImage *image = THANDOR_CONTAINER_OF(shadingLevelGroup, InGameUiImage, shadingLevelGroup);
    if (newGridHalfSize == 32) {
      selectedControl = &image->shadingLevel32x32Button.base.selectable.base;
      if (shadingDepth == 64) {
        selectedControl = &image->shadingLevel32x64Button.base.selectable.base;
      }
      else if (shadingDepth == 128) {
        selectedControl = &image->shadingLevel32x128Button.base.selectable.base;
      }
    }
    else if (newGridHalfSize == 64) {
      selectedControl = &image->shadingLevel64x64Button.base.selectable.base;
      if (shadingDepth == 128) {
        selectedControl = &image->shadingLevel64x128Button.base.selectable.base;
      }
    }
    else {
      selectedControl = &image->shadingLevel128x128Button.base.selectable.base;
    }
    UiSelectableGroup_SelectExclusive(6,selectedControl,
      &image->shadingLevel128x128Button.base.selectable.base,
      &image->shadingLevel64x128Button.base.selectable.base,
      &image->shadingLevel64x64Button.base.selectable.base,
      &image->shadingLevel32x128Button.base.selectable.base,
      &image->shadingLevel32x64Button.base.selectable.base,
      &image->shadingLevel32x32Button.base.selectable.base);
    return;
  }
  textureDimension = PersistentSettings_Read(PERSISTENT_DEFAULT_SHADING_TEXTURE_DIMENSION,
                                            PERSISTENT_SETTING_SHADING_TEXTURE_DIMENSION);
  gridHalfSize = PersistentSettings_Read(PERSISTENT_DEFAULT_SHADING_GRID_HALF_SIZE,PERSISTENT_SETTING_SHADING_GRID_HALF_SIZE);
  storedSubresourceCount = PersistentSettings_Read(PERSISTENT_DEFAULT_SHADING_SUBRESOURCE_COUNT,
                                                  PERSISTENT_SETTING_SHADING_SUBRESOURCE_COUNT);
  GraphicsShadingRuntime_InitializeGeneratedTexture
            (storedSubresourceCount,gridHalfSize,textureDimension);
}

/* UI action 0x1206 (modelDetailSlider; g_InGameUiActionHandlersPage12[6]): stores the model detail slider
   value and makes it the model LOD depth threshold (Q8) at once.
*/
void InGameModelSettings_SetLodDepthThresholdQ8(UiSettingsValueControl *control)

{
  g_ModelLodDepthThresholdQ8 = PersistentOption_StoreSlider(control,PERSISTENT_SETTING_MODEL_LOD_DEPTH_THRESHOLD);
}

/* UI action 0x1207 (INGAME_ACTION_TEXTURE_QUALITY, the three texture quality buttons;
   g_InGameUiActionHandlersPage12[7]): selects the pressed button, stores its level and rebuilds every
   staging texture with it, showing the wait cursor meanwhile. Unlike the frontend version
   (FrontendTextureSettings_SetQuality, which uses level >> 1) the level itself becomes the downsample shift.
*/
void InGameTextureSettings_SetQuality(UiSelectableControl *control)

{
  PersistentTextureQualityLevel qualityLevel;
  UiNodeBase *selectedQualityControl = nullptr; /* control is one of the three buttons */
  UiNodeBase *textureQualityGroup;

  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_BUSY);
  /* control is one of the texture quality buttons, its parent is textureQualityGroup */
  textureQualityGroup = (control->base).parent;
  InGameUiImage *image = THANDOR_CONTAINER_OF(textureQualityGroup, InGameUiImage, textureQualityGroup);
  if (&image->textureQualityLowButton.selectable == control) {
    qualityLevel = TEXTURE_QUALITY_LOW;
    selectedQualityControl = &image->textureQualityLowButton.selectable.base;
  }
  if (&image->textureQualityMediumButton.selectable == control) {
    qualityLevel = TEXTURE_QUALITY_MEDIUM;
    selectedQualityControl = &image->textureQualityMediumButton.selectable.base;
  }
  if (&image->textureQualityHighButton.selectable == control) {
    qualityLevel = TEXTURE_QUALITY_HIGH;
    selectedQualityControl = &image->textureQualityHighButton.selectable.base;
  }
  UiSelectableGroup_SelectExclusive(3,selectedQualityControl,
      &image->textureQualityHighButton.selectable.base,
      &image->textureQualityMediumButton.selectable.base,
      &image->textureQualityLowButton.selectable.base);
  PersistentSettings_Write(qualityLevel,PERSISTENT_SETTING_TEXTURE_QUALITY);
  g_TextureDownsampleShift = qualityLevel;
  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_ARROW);
}

/* UI action 0x1208 (effectsEnabledCheckbox; g_InGameUiActionHandlersPage12[8]): stores the effects switch,
   stops the playing effect voice when switched off, enables or disables the dependent sliders and loads the
   effects and movie volumes from the settings (or silences them).
*/
void InGameAudioSettings_SetEffectsEnabled(UiSelectableControl *control)

{
  uint32_t audioFlags;
  AudioMixerGainQ15 effectsGainQ15;
  MovieAudioGainQ15 movieDefaultGainQ15;
  MovieAudioGainQ15 movieAlternateGainQ15;
  Bool8 isEnabled;

  isEnabled = (Bool8)UiSelectableControl_IsSelected(control);
  if (!isEnabled) {
    g_SoundStopVoice(g_InGameActiveEffectVoice);
    g_InGameActiveEffectVoice = nullptr;
  }
  audioFlags = PersistentSettings_Read(PERSISTENT_SOUND_OPTION_DEFAULT,PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
  PersistentSettings_Write((uint32_t)isEnabled | audioFlags & ~PERSISTENT_SOUND_OPTION_EFFECTS,
                           PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
  UiNodeBase *uiRoot = &control->base;
  while (uiRoot->parent != UI_NODE_NONE) {
    uiRoot = uiRoot->parent;
  }
  if (isEnabled) {
    UiNodeList_UnsuppressActionId(INGAME_ACTION_EFFECTS_VOLUME,uiRoot);
    UiNodeList_UnsuppressActionId(INGAME_ACTION_MOVIE_VOLUME,uiRoot);
    UiNodeList_UnsuppressActionId(INGAME_ACTION_MESSAGE_MOVIE_VOLUME,uiRoot);
  }
  else {
    UiNodeList_SuppressActionId(INGAME_ACTION_EFFECTS_VOLUME,uiRoot);
    UiNodeList_SuppressActionId(INGAME_ACTION_MOVIE_VOLUME,uiRoot);
    UiNodeList_SuppressActionId(INGAME_ACTION_MESSAGE_MOVIE_VOLUME,uiRoot);
  }
  if ((audioFlags & PERSISTENT_SOUND_OPTION_MUSIC) == 0) {
    UiNodeList_SuppressActionId(INGAME_ACTION_MUSIC_VOLUME,uiRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(INGAME_ACTION_MUSIC_VOLUME,uiRoot);
  }
  if (isEnabled == 0 && (audioFlags & PERSISTENT_SOUND_OPTION_MUSIC) == 0) {
    UiNodeList_SuppressActionId(INGAME_ACTION_REVERSE_STEREO,uiRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(INGAME_ACTION_REVERSE_STEREO,uiRoot);
  }
  effectsGainQ15 = 0;
  if (isEnabled) {
    effectsGainQ15 = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_EFFECTS_GAIN);
  }
  movieDefaultGainQ15 = 0;
  g_UiSoundGainQ15 = effectsGainQ15;
  g_SoundEffectsGainQ15 = effectsGainQ15;
  if (isEnabled) {
    movieDefaultGainQ15 = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_MOVIE_DEFAULT_GAIN);
  }
  movieAlternateGainQ15 = 0;
  g_MovieDefaultAudioGainQ15 = movieDefaultGainQ15;
  if (isEnabled) {
    movieAlternateGainQ15 = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_MOVIE_ALTERNATE_GAIN);
  }
  g_MovieAlternateAudioGainQ15 = movieAlternateGainQ15;
}

/* UI action 0x1209 (musicEnabledCheckbox; g_InGameUiActionHandlersPage12[9]): stores the music switch and
   enables or disables the dependent sliders. Switching music off stops the playing track and sets the
   music countdown to 1, so the next track is chosen right away when music comes back on.
*/
void InGameAudioSettings_SetMusicEnabled(UiSelectableControl *control)

{
  uint32_t audioFlags;
  uint32_t musicEnabledBit;
  Bool8 isEnabled;

  musicEnabledBit = 0;
  isEnabled = (Bool8)UiSelectableControl_IsSelected(control);
  if (isEnabled) {
    musicEnabledBit = PERSISTENT_SOUND_OPTION_MUSIC;
  }
  else {
    g_SoundStopVoice(g_InGameActiveMusicVoice);
    g_InGameActiveMusicVoice = nullptr;
    g_InGameMusicNextTrackCountdown = 1;
  }
  audioFlags = PersistentSettings_Read(PERSISTENT_SOUND_OPTION_DEFAULT,PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
  PersistentSettings_Write(musicEnabledBit | audioFlags & ~PERSISTENT_SOUND_OPTION_MUSIC,
                           PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
  UiNodeBase *uiRoot = &control->base;
  while (uiRoot->parent != UI_NODE_NONE) {
    uiRoot = uiRoot->parent;
  }
  if ((audioFlags & PERSISTENT_SOUND_OPTION_EFFECTS) == 0) {
    UiNodeList_SuppressActionId(INGAME_ACTION_EFFECTS_VOLUME,uiRoot);
    UiNodeList_SuppressActionId(INGAME_ACTION_MOVIE_VOLUME,uiRoot);
    UiNodeList_SuppressActionId(INGAME_ACTION_MESSAGE_MOVIE_VOLUME,uiRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(INGAME_ACTION_EFFECTS_VOLUME,uiRoot);
    UiNodeList_UnsuppressActionId(INGAME_ACTION_MOVIE_VOLUME,uiRoot);
    UiNodeList_UnsuppressActionId(INGAME_ACTION_MESSAGE_MOVIE_VOLUME,uiRoot);
  }
  if (musicEnabledBit == 0) {
    UiNodeList_SuppressActionId(INGAME_ACTION_MUSIC_VOLUME,uiRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(INGAME_ACTION_MUSIC_VOLUME,uiRoot);
  }
  if (musicEnabledBit == 0 && (audioFlags & PERSISTENT_SOUND_OPTION_EFFECTS) == 0) {
    UiNodeList_SuppressActionId(INGAME_ACTION_REVERSE_STEREO,uiRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(INGAME_ACTION_REVERSE_STEREO,uiRoot);
  }
}

/* UI action 0x120A (reverseStereoCheckbox; g_InGameUiActionHandlersPage12[10]): stores the reverse stereo
   switch and sets g_ReverseStereoMask to all ones (swap the channels) or zero.
*/
void InGameAudioSettings_SetReverseStereo(UiSelectableControl *control)

{
  PersistentOption_ApplyCheckbox(control,PERSISTENT_SETTING_SOUND_OPTION_FLAGS,PERSISTENT_SOUND_OPTION_REVERSE_STEREO,
                                 [](Bool8 isSelected) { g_ReverseStereoMask = isSelected ? -1 : 0; },
                                 PERSISTENT_SOUND_OPTION_DEFAULT);
}

/* UI action 0x120B (effectsVolumeSlider; g_InGameUiActionHandlersPage12[11]): stores the effects volume,
   makes it the UI and effects gain and applies it to the playing effect voice so the change is audible.
*/
void InGameAudioSettings_SetEffectsGain(UiSettingsValueControl *control)

{
  PersistentSettingsValue value;

  value = PersistentOption_StoreSlider(control,PERSISTENT_SETTING_EFFECTS_GAIN);
  g_UiSoundGainQ15 = value;
  g_SoundEffectsGainQ15 = value;
  g_SoundSetVoiceGains(value,value,g_InGameActiveEffectVoice);
}

/* UI action 0x120C (movieVolumeSlider; g_InGameUiActionHandlersPage12[12]): stores the movie volume and
   makes it the default movie gain.
*/
void InGameAudioSettings_SetMovieDefaultGain(UiSettingsValueControl *control)

{
  g_MovieDefaultAudioGainQ15 = PersistentOption_StoreSlider(control,PERSISTENT_SETTING_MOVIE_DEFAULT_GAIN);
}

/* UI action 0x120D (musicVolumeSlider; g_InGameUiActionHandlersPage12[13]): stores the music volume and
   applies it to the playing music voice.
*/
void InGameAudioSettings_SetMusicGain(UiSettingsValueControl *control)

{
  PersistentSettingsValue value;

  value = PersistentOption_StoreSlider(control,PERSISTENT_SETTING_MUSIC_GAIN);
  g_SoundSetVoiceGains(value,value,g_InGameActiveMusicVoice);
}

/* UI action 0x121A (messageMovieVolumeSlider; g_InGameUiActionHandlersPage12[26]): stores the volume of the
   message movies and makes it the alternate movie gain used by timed movie playback.
*/
void InGameAudioSettings_SetMovieAlternateGain(UiSettingsValueControl *control)

{
  g_MovieAlternateAudioGainQ15 = PersistentOption_StoreSlider(control,PERSISTENT_SETTING_MOVIE_ALTERNATE_GAIN);
}

/* UI action 0x1003 (game menu button): opening shows the game menu window (page 3) with the gameplay options
   loaded from the persistent settings, blocks the world input and pauses a local game; network games cannot
   save, so the save button is suppressed there. Closing hides the window and resumes as
   InGameMissionHelpPage_Toggle does.
*/
void InGameSettingsPage_ToggleAndSynchronizeControls(UiSelectableControl *settingsToggle)

{
  UiNodeBase *uiRoot;
  uint32_t settingValue;
  Bool8 isSelected;

  uiRoot = &settingsToggle->base;
  while (uiRoot->parent != UI_NODE_NONE) {
    uiRoot = uiRoot->parent;
  }
  InGameUiImage *image = InGameUi_Image(uiRoot);
  isSelected = (Bool8)UiSelectableControl_IsSelected(settingsToggle);
  if (!isSelected) {
    UiPageStack_SetActiveIndex(INGAME_WINDOW_PAGE_NONE,
                               UiLayoutContainerControl_AsPageStack(&image->gameWindowPageStack));
    image->worldView.base.nodeFlags &= ~UI_NODE_SUPPRESSED;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PAUSED_BEFORE_WINDOW) == 0) {
        g_UiCommandRuntimeFlags =
             g_UiCommandRuntimeFlags & ~(UI_COMMAND_RUNTIME_FLAG_WINDOW_PAUSE | UI_COMMAND_RUNTIME_FLAG_PAUSED);
      }
      else {
        g_UiCommandRuntimeFlags =
             g_UiCommandRuntimeFlags &
             ~(UI_COMMAND_RUNTIME_FLAG_WINDOW_PAUSE | UI_COMMAND_RUNTIME_FLAG_PAUSED_BEFORE_WINDOW);
      }
    }
    return;
  }
  UiSelectableControl_SetSelected(0,&image->missionObjectivesButton.selectable);
  image->worldView.base.nodeFlags |= UI_NODE_SUPPRESSED;
  UiKeyboardFocus_ReleaseNode(&image->worldView.base);
  UiPageStack_SetActiveIndex(INGAME_WINDOW_PAGE_GAME_MENU,
                             UiLayoutContainerControl_AsPageStack(&image->gameWindowPageStack));
  settingValue = PersistentSettings_Read(0,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS);
  UiSelectableControl_SetSelected(settingValue & PERSISTENT_MAP_OPTION_AUTOMATIC_ZOOM_OFF,
                                  &image->autoZoomOffCheckbox.selectable);
  UiSelectableControl_SetSelected(settingValue & PERSISTENT_MAP_OPTION_AUTOMATIC_ROTATION_OFF,
                                  &image->autoRotationOffCheckbox.selectable);
  /* "right button does not scroll" per the frontend settings page (the Tab key also toggles this bit) */
  UiSelectableControl_SetSelected
            (settingValue & PERSISTENT_MAP_OPTION_SIDE_PANEL_HIDDEN,&image->rightButtonNoScrollCheckbox.selectable);
  /* rotation is linked with zoom or with tilt; each excludes the other */
  settingValue = PersistentSettings_Read(0,PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS);
  if ((settingValue & PERSISTENT_LINK_OPTION_ROTATION_ZOOM) != 0) {
    UiNodeList_SuppressActionId(INGAME_ACTION_LINK_ROTATION_TILT,uiRoot);
  }
  UiSelectableControl_SetSelected
            (settingValue & PERSISTENT_LINK_OPTION_ROTATION_ZOOM,&image->linkRotationZoomCheckbox.selectable);
  if ((settingValue & PERSISTENT_LINK_OPTION_ROTATION_TILT) != 0) {
    UiNodeList_SuppressActionId(INGAME_ACTION_LINK_ROTATION_ZOOM,uiRoot);
  }
  UiSelectableControl_SetSelected
            (settingValue & PERSISTENT_LINK_OPTION_ROTATION_TILT,&image->linkRotationTiltCheckbox.selectable);
  settingValue = PersistentSettings_Read(PERSISTENT_DEFAULT_CAMERA_SCROLL_STEP,PERSISTENT_SETTING_CAMERA_SCROLL_STEP);
  image->scrollSpeedSlider.value = settingValue;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WINDOW_PAUSE) == 0) {
      if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PAUSED) != 0) {
        g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | UI_COMMAND_RUNTIME_FLAG_PAUSED_BEFORE_WINDOW;
      }
      g_UiCommandRuntimeFlags =
           g_UiCommandRuntimeFlags | (UI_COMMAND_RUNTIME_FLAG_WINDOW_PAUSE | UI_COMMAND_RUNTIME_FLAG_PAUSED);
    }
  }
  else {
    UiNodeList_SuppressActionId(INGAME_ACTION_SAVE_GAME_WINDOW,uiRoot);
  }
}

/* Action slot adapters: these two handlers take the source node's address as an intptr_t; the action queue passes
   the node pointer (void *), which on x64 arrives as exactly that address value. */
static void UiActionSlot_QuitMenuOpenAndRefreshButtons(void *source)
{
  InGameQuitMenu_OpenAndRefreshButtons((InGameCommandPanelSourceAddress32)source);
}

static void UiActionSlot_SaveGameDeleteSelectedAndRefreshCatalog(void *source)
{
  InGameSaveGameAction_DeleteSelectedSaveAndRefreshCatalog((InGameSaveGamePageControlAddress32)source);
}

InGameUiActionHandlerPage12Prefix28 g_InGameUiActionHandlersPage12 = {
        .handlers = {
            /*  0 */ UI_SLOT(UiActionSlot_QuitMenuOpenAndRefreshButtons),
            /*  1 */ UI_SLOT(InGameSettingsPage_CloseViaSharedToggle),
            /*  2 */ UI_SLOT(InGameGraphicsSettings_OpenAndSynchronize),
            /*  3 */ UI_SLOT(InGameAudioSettings_OpenAndSynchronize),
            /*  4 */ UI_SLOT(InGameShadingSettings_SetEnabled),
            /*  5 */ UI_SLOT(InGameShadingSettings_ApplyLevel),
            /*  6 */ UI_SLOT(InGameModelSettings_SetLodDepthThresholdQ8),
            /*  7 */ UI_SLOT(InGameTextureSettings_SetQuality),
            /*  8 */ UI_SLOT(InGameAudioSettings_SetEffectsEnabled),
            /*  9 */ UI_SLOT(InGameAudioSettings_SetMusicEnabled),
            /* 10 */ UI_SLOT(InGameAudioSettings_SetReverseStereo),
            /* 11 */ UI_SLOT(InGameAudioSettings_SetEffectsGain),
            /* 12 */ UI_SLOT(InGameAudioSettings_SetMovieDefaultGain),
            /* 13 */ UI_SLOT(InGameAudioSettings_SetMusicGain),
            /* 14 */ UI_SLOT(InGameSaveGamePage_RebuildCatalog),
            /* 15 */ UI_SLOT(InGameSaveGameList_SelectAndRefreshDetail),
            /* 16 */ UI_SLOT(InGameSaveGame_SaveSelectedOrTypedName),
            /* 17 */ UI_SLOT(InGameSaveName_UpdateSaveActionValidity),
            /* 18 */ UI_SLOT(InGameGameplaySettings_SetAutomaticZoomOff),
            /* 19 */ UI_SLOT(InGameGameplaySettings_SetAutomaticRotationOff),
            /* 20 */ UI_SLOT(InGameGameplaySettings_SetLinkRotationZoom),
            /* 21 */ UI_SLOT(InGameGameplaySettings_SetLinkRotationTilt),
            /* 22 */ UI_SLOT(InGameGameplaySettings_SetRightButtonDoesNotScroll),
            /* 23 */ UI_SLOT(InGameGameplaySettings_SetCameraScrollStep),
            /* 24 */ UI_SLOT(InGameSettingsPage_OpenViaSharedToggle),
            /* 25 */ UI_SLOT(UiActionSlot_SaveGameDeleteSelectedAndRefreshCatalog),
            /* 26 */ UI_SLOT(InGameAudioSettings_SetMovieAlternateGain),
            /* 27 */ UI_SLOT(InGameGameplaySettings_SetHidePanel)
        }};
