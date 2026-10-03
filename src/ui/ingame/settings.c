/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/settings.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/settings.h>
#include <thandor/thandor.h>

/* Implementation ownership: ui/ingame/settings. */

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
  UiSelectableControl_SetSelected(0,(UiSelectableControl *)INGAME_UI(source,missionObjectivesButton));
  InGameSettingsPage_ToggleAndSynchronizeControls
            ((UiSelectableControl *)INGAME_UI(source,missionObjectivesButton));
  return;
}


/* UI action 0x101D (quitMenuAbortMissionButton; g_InGameUiActionHandlersPage10[29]): closes the game menu
   and lets the local player leave the session (command 0x150 without flags). A local game runs the handler
   directly, a network game queues the command so every peer executes it.
*/
void InGameQuitMenu_AbortMission(UiNodeBase *source)

{

  while (source->parent != UI_NODE_NONE) {
    source = source->parent;
  }
  UiSelectableControl_SetSelected(0,(UiSelectableControl *)INGAME_UI(source,inGameMenuButton));
  InGameSettingsPage_ToggleAndSynchronizeControls((UiSelectableControl *)INGAME_UI(source,inGameMenuButton));
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    InGameCommand_HandlePlayerDeparture(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_PLAYER_DEPARTURE,0,0,0);
  }
  return;
}


/* UI action 0x101E (INGAME_ACTION_QUIT_SURRENDER, quitMenuSurrenderButton; g_InGameUiActionHandlersPage10[30]):
   closes the game menu and gives up, command 0x150 with INGAME_PLAYER_DEPARTURE_FLAG_SURRENDER destroys every army
   of the local faction. Local games call the handler directly, network games queue the command.
*/
void InGameQuitMenu_Surrender(UiNodeBase *source)

{

  while (source->parent != UI_NODE_NONE) {
    source = source->parent;
  }
  UiSelectableControl_SetSelected(0,(UiSelectableControl *)INGAME_UI(source,inGameMenuButton));
  InGameSettingsPage_ToggleAndSynchronizeControls((UiSelectableControl *)INGAME_UI(source,inGameMenuButton));
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    InGameCommand_HandlePlayerDeparture
              (g_LocalPlayerRuntimeId,0,0,INGAME_PLAYER_DEPARTURE_FLAG_SURRENDER);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand
              (INGAME_COMMAND_PLAYER_DEPARTURE,0,0,INGAME_PLAYER_DEPARTURE_FLAG_SURRENDER);
  }
  return;
}


/* UI action 0x1201 (gameMenuCloseButton; g_InGameUiActionHandlersPage12[1]): closes the game menu by
   releasing the inGameMenuButton toggle and running its toggle handler, which also resumes a paused game.
*/
void InGameSettingsPage_CloseViaSharedToggle(UiNodeBase *source)

{

  while (source->parent != UI_NODE_NONE) {
    source = source->parent;
  }
  UiSelectableControl_SetSelected(0,(UiSelectableControl *)INGAME_UI(source,inGameMenuButton));
  InGameSettingsPage_ToggleAndSynchronizeControls((UiSelectableControl *)INGAME_UI(source,inGameMenuButton));
  return;
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
  UiSelectableControl_SetSelected(1,(UiSelectableControl *)INGAME_UI(source,inGameMenuButton));
  InGameSettingsPage_ToggleAndSynchronizeControls((UiSelectableControl *)INGAME_UI(source,inGameMenuButton));
  return;
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
  return;
}


/* UI action 0x1021 (missionHelpBriefingTab; g_InGameUiActionHandlersPage10[33]): selects tab 0 of the mission help
   window exclusively among its three tab buttons and shows page 0 (the mission briefing) of its page stack.
*/
void InGameMissionHelpPage_SelectBriefingTab(UiNodeBase *sourceNode)

{
  /* sourceNode is missionHelpBriefingTab of the in-game UI template copy */
  UiSelectableGroup_SelectExclusive(3,sourceNode,
      THANDOR_UI_SIBLING(sourceNode,InGameUiImage,missionHelpBriefingTab,missionHelpMouseTab),
      THANDOR_UI_SIBLING(sourceNode,InGameUiImage,missionHelpBriefingTab,missionHelpKeyboardTab),
      sourceNode);
  UiPageStack_SetActiveIndex(0,(UiPageStackControl *)THANDOR_UI_SIBLING(sourceNode,InGameUiImage,missionHelpBriefingTab,missionHelpTabPageStack));
  return;
}


/* UI action 0x1022 (missionHelpKeyboardTab; g_InGameUiActionHandlersPage10[34]): selects tab 1 of the mission help
   window exclusively among its three tab buttons and shows page 1 (the keyboard help) of its page stack.
*/
void InGameMissionHelpPage_SelectKeyboardTab(UiNodeBase *sourceNode)

{
  /* sourceNode is missionHelpKeyboardTab of the in-game UI template copy */
  UiSelectableGroup_SelectExclusive(3,sourceNode,
      THANDOR_UI_SIBLING(sourceNode,InGameUiImage,missionHelpKeyboardTab,missionHelpMouseTab),
      THANDOR_UI_SIBLING(sourceNode,InGameUiImage,missionHelpKeyboardTab,missionHelpBriefingTab),
      sourceNode);
  UiPageStack_SetActiveIndex(1,(UiPageStackControl *)THANDOR_UI_SIBLING(sourceNode,InGameUiImage,missionHelpKeyboardTab,missionHelpTabPageStack));
  return;
}


/* UI action 0x1023 (missionHelpMouseTab; g_InGameUiActionHandlersPage10[35]): selects tab 2 of the mission help
   window exclusively among its three tab buttons and shows page 2 (the mouse help) of its page stack.
*/
void InGameMissionHelpPage_SelectMouseTab(UiNodeBase *sourceNode)

{
  /* sourceNode is missionHelpMouseTab of the in-game UI template copy */
  UiSelectableGroup_SelectExclusive(3,sourceNode,
      THANDOR_UI_SIBLING(sourceNode,InGameUiImage,missionHelpMouseTab,missionHelpBriefingTab),
      THANDOR_UI_SIBLING(sourceNode,InGameUiImage,missionHelpMouseTab,missionHelpKeyboardTab),
      sourceNode);
  UiPageStack_SetActiveIndex(2,(UiPageStackControl *)THANDOR_UI_SIBLING(sourceNode,InGameUiImage,missionHelpMouseTab,missionHelpTabPageStack));
  return;
}


/* UI action 0x1216 (rightButtonNoScrollCheckbox; g_InGameUiActionHandlersPage12[22]): stores
   PERSISTENT_MAP_OPTION_SIDE_PANEL_HIDDEN and applies it at once. Although the checkbox text reads "right
   button does not scroll", the effect is to hide the side panel (side panel stack page 1) and widen the
   world view to the right screen edge; clearing it restores the panel and the view's right edge.
*/
void InGameGameplaySettings_SetRightButtonDoesNotScroll(UiSelectableControl *control)

{
  UiPageStackControl *sidePanelStack;
  uint32_t optionFlags;
  PersistentSettingsValue value;
  Bool8 isSelected;

  optionFlags = PersistentSettings_Read(0,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS);
  /* control is rightButtonNoScrollCheckbox of the in-game UI template copy */
  sidePanelStack =
       (UiPageStackControl *)THANDOR_UI_SIBLING(control,InGameUiImage,rightButtonNoScrollCheckbox,sidePanelStack);
  isSelected = (Bool8)UiSelectableControl_IsSelected(control);
  if (isSelected) {
    value = optionFlags | PERSISTENT_MAP_OPTION_SIDE_PANEL_HIDDEN;
    UiPageStack_SetActiveIndex(1,sidePanelStack);
    UiPageStack_SetActiveIndex
              (0,(UiPageStackControl *)
                 THANDOR_UI_SIBLING(control,InGameUiImage,rightButtonNoScrollCheckbox,resourceBarModeStack));
    UiPageStack_SetActiveIndex
              (0,(UiPageStackControl *)
                 THANDOR_UI_SIBLING(control,InGameUiImage,rightButtonNoScrollCheckbox,gamePanelsModeStack));
    THANDOR_UI_SIBLING(control,InGameUiImage,rightButtonNoScrollCheckbox,worldViewArea)->rightOffset = 0;
  }
  else {
    value = optionFlags & ~PERSISTENT_MAP_OPTION_SIDE_PANEL_HIDDEN;
    UiPageStack_SetActiveIndex(0,sidePanelStack);
    UiPageStack_SetActiveIndex
              (0,(UiPageStackControl *)
                 THANDOR_UI_SIBLING(control,InGameUiImage,rightButtonNoScrollCheckbox,resourceBarModeStack));
    UiPageStack_SetActiveIndex
              (0,(UiPageStackControl *)
                 THANDOR_UI_SIBLING(control,InGameUiImage,rightButtonNoScrollCheckbox,gamePanelsModeStack));
    THANDOR_UI_SIBLING(control,InGameUiImage,rightButtonNoScrollCheckbox,worldViewArea)->rightOffset =
         THANDOR_UI_SIBLING(control,InGameUiImage,rightButtonNoScrollCheckbox,sidePanelFrameLeftEdge)->leftOffset;
  }
  UiContainer_LayoutChildren(THANDOR_UI_SIBLING(control,InGameUiImage,rightButtonNoScrollCheckbox,inGameRootPanel));
  PersistentSettings_Write(value,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS);
  return;
}


/* UI action 0x1217 (scrollSpeedSlider; g_InGameUiActionHandlersPage12[23]): stores the slider value as
   PERSISTENT_SETTING_CAMERA_SCROLL_STEP; the camera reads the setting whenever it scrolls.
*/
void InGameGameplaySettings_SetCameraScrollStep(UiSettingsValueControl *control)

{
  PersistentSettings_Write(control->boundValue,PERSISTENT_SETTING_CAMERA_SCROLL_STEP);
  return;
}


/* UI action 0x1212 (autoZoomOffCheckbox; g_InGameUiActionHandlersPage12[18]): stores
   PERSISTENT_MAP_OPTION_AUTOMATIC_ZOOM_OFF. Switching automatic zoom off also resets the minimap to its
   default scale.
*/
void InGameGameplaySettings_SetAutomaticZoomOff(UiSelectableControl *control)

{
  uint32_t optionFlags;
  PersistentSettingsValue value;
  Bool8 isSelected;

  optionFlags = PersistentSettings_Read(0,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS);
  isSelected = (Bool8)UiSelectableControl_IsSelected(control);
  if (isSelected) {
    value = optionFlags | PERSISTENT_MAP_OPTION_AUTOMATIC_ZOOM_OFF;
    /* control is autoZoomOffCheckbox; reset the minimap zoom */
    ((UiSelectionGeometryControl *)THANDOR_UI_SIBLING(control,InGameUiImage,autoZoomOffCheckbox,minimapView))->sampleScaleQ12 =
         INGAME_MINIMAP_DEFAULT_SCALE_Q12;
  }
  else {
    value = optionFlags & ~PERSISTENT_MAP_OPTION_AUTOMATIC_ZOOM_OFF;
  }
  PersistentSettings_Write(value,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS);
  return;
}


/* UI action 0x1213 (autoRotationOffCheckbox; g_InGameUiActionHandlersPage12[19]): stores
   PERSISTENT_MAP_OPTION_AUTOMATIC_ROTATION_OFF. Switching automatic rotation off also turns the minimap back
   to its default angle.
*/
void InGameGameplaySettings_SetAutomaticRotationOff(UiSelectableControl *control)

{
  uint32_t optionFlags;
  PersistentSettingsValue value;
  Bool8 isSelected;

  optionFlags = PersistentSettings_Read(0,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS);
  isSelected = (Bool8)UiSelectableControl_IsSelected(control);
  if (isSelected) {
    value = optionFlags | PERSISTENT_MAP_OPTION_AUTOMATIC_ROTATION_OFF;
    /* control is autoRotationOffCheckbox; reset the minimap rotation to its default angle */
    ((UiSelectionGeometryControl *)THANDOR_UI_SIBLING(control,InGameUiImage,autoRotationOffCheckbox,minimapView))->
    rotationAngle = INGAME_MINIMAP_DEFAULT_ROTATION_ANGLE;
  }
  else {
    value = optionFlags & ~PERSISTENT_MAP_OPTION_AUTOMATIC_ROTATION_OFF;
  }
  PersistentSettings_Write(value,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS);
  return;
}


/* UI action 0x1214 (INGAME_ACTION_LINK_ROTATION_ZOOM, linkRotationZoomCheckbox;
   g_InGameUiActionHandlersPage12[20]): stores PERSISTENT_LINK_OPTION_ROTATION_ZOOM and mirrors it to the world
   view. The rotation can only be linked to zoom or to tilt, so the tilt checkbox is disabled while this is on.
*/
void InGameGameplaySettings_SetLinkRotationZoom(UiSelectableControl *control)

{
  WorldRuntimeFlags *runtimeFlagsField;
  uint32_t optionFlags;
  PersistentSettingsValue value;
  Bool8 isSelected;

  optionFlags = PersistentSettings_Read(0,PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS);
  isSelected = (Bool8)UiSelectableControl_IsSelected(control);
  if (isSelected) {
    value = optionFlags | PERSISTENT_LINK_OPTION_ROTATION_ZOOM;
    /* control is linkRotationZoomCheckbox; the world view's runtime flags */
    runtimeFlagsField =
         &((WorldRuntimeContext *)THANDOR_UI_SIBLING(control,InGameUiImage,linkRotationZoomCheckbox,worldView))->runtimeFlags;
    *runtimeFlagsField = *runtimeFlagsField | WORLD_RUNTIME_FLAG_LINK_ROTATION_ZOOM;
    UiNodeList_SuppressActionId(INGAME_ACTION_LINK_ROTATION_TILT,(control->base).parent);
  }
  else {
    value = optionFlags & ~PERSISTENT_LINK_OPTION_ROTATION_ZOOM;
    /* control is linkRotationZoomCheckbox; the world view's runtime flags */
    runtimeFlagsField =
         &((WorldRuntimeContext *)THANDOR_UI_SIBLING(control,InGameUiImage,linkRotationZoomCheckbox,worldView))->runtimeFlags;
    *runtimeFlagsField = *runtimeFlagsField & ~WORLD_RUNTIME_FLAG_LINK_ROTATION_ZOOM;
    UiNodeList_UnsuppressActionId(INGAME_ACTION_LINK_ROTATION_TILT,(control->base).parent);
  }
  PersistentSettings_Write(value,PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS);
  return;
}


/* UI action 0x1215 (INGAME_ACTION_LINK_ROTATION_TILT, linkRotationTiltCheckbox;
   g_InGameUiActionHandlersPage12[21]): stores PERSISTENT_LINK_OPTION_ROTATION_TILT and mirrors it to the world
   view; the zoom link checkbox is disabled while this is on, as the two links exclude each other.
*/
void InGameGameplaySettings_SetLinkRotationTilt(UiSelectableControl *control)

{
  WorldRuntimeFlags *runtimeFlagsField;
  uint32_t optionFlags;
  PersistentSettingsValue value;
  Bool8 isSelected;

  optionFlags = PersistentSettings_Read(0,PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS);
  isSelected = (Bool8)UiSelectableControl_IsSelected(control);
  if (isSelected) {
    value = optionFlags | PERSISTENT_LINK_OPTION_ROTATION_TILT;
    /* control is linkRotationTiltCheckbox; the world view's runtime flags */
    runtimeFlagsField =
         &((WorldRuntimeContext *)THANDOR_UI_SIBLING(control,InGameUiImage,linkRotationTiltCheckbox,worldView))->runtimeFlags;
    *runtimeFlagsField = *runtimeFlagsField | WORLD_RUNTIME_FLAG_LINK_ROTATION_TILT;
    UiNodeList_SuppressActionId(INGAME_ACTION_LINK_ROTATION_ZOOM,(control->base).parent);
  }
  else {
    value = optionFlags & ~PERSISTENT_LINK_OPTION_ROTATION_TILT;
    /* control is linkRotationTiltCheckbox; the world view's runtime flags */
    runtimeFlagsField =
         &((WorldRuntimeContext *)THANDOR_UI_SIBLING(control,InGameUiImage,linkRotationTiltCheckbox,worldView))->runtimeFlags;
    *runtimeFlagsField = *runtimeFlagsField & ~WORLD_RUNTIME_FLAG_LINK_ROTATION_TILT;
    UiNodeList_UnsuppressActionId(INGAME_ACTION_LINK_ROTATION_ZOOM,(control->base).parent);
  }
  PersistentSettings_Write(value,PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS);
  return;
}


/* UI action 0x121B (hidePanelCheckbox; g_InGameUiActionHandlersPage12[27]): stores
   PERSISTENT_LINK_OPTION_HIDE_PANEL and mirrors it to the world view's WORLD_RUNTIME_FLAG_HIDE_PANEL. The name
   follows the checkbox label; the world view tests the flag together with the left mouse button.
*/
void InGameGameplaySettings_SetHidePanel(UiSelectableControl *control)

{
  WorldRuntimeFlags *runtimeFlagsField;
  uint32_t optionFlags;
  PersistentSettingsValue value;
  Bool8 isSelected;

  optionFlags = PersistentSettings_Read(0,PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS);
  isSelected = (Bool8)UiSelectableControl_IsSelected(control);
  if (isSelected) {
    value = optionFlags | PERSISTENT_LINK_OPTION_HIDE_PANEL;
    /* control is hidePanelCheckbox; the world view's runtime flags */
    runtimeFlagsField =
         &((WorldRuntimeContext *)THANDOR_UI_SIBLING(control,InGameUiImage,hidePanelCheckbox,worldView))->runtimeFlags;
    *runtimeFlagsField = *runtimeFlagsField | WORLD_RUNTIME_FLAG_HIDE_PANEL;
  }
  else {
    value = optionFlags & ~PERSISTENT_LINK_OPTION_HIDE_PANEL;
    /* control is hidePanelCheckbox; the world view's runtime flags */
    runtimeFlagsField =
         &((WorldRuntimeContext *)THANDOR_UI_SIBLING(control,InGameUiImage,hidePanelCheckbox,worldView))->runtimeFlags;
    *runtimeFlagsField = *runtimeFlagsField & ~WORLD_RUNTIME_FLAG_HIDE_PANEL;
  }
  PersistentSettings_Write(value,PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS);
  return;
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
#define GRAPHICS_UI(node) THANDOR_UI_SIBLING(graphicsButton,InGameUiImage,gameMenuGraphicsButton,node)
  UiPageStack_SetActiveIndex(INGAME_WINDOW_PAGE_GRAPHICS_SETTINGS,(UiPageStackControl *)GRAPHICS_UI(gameWindowPageStack));
  settingValue = PersistentSettings_Read(1,PERSISTENT_SETTING_SHADING_ENABLED);
  UiSelectableControl_SetSelected(settingValue,(UiSelectableControl *)GRAPHICS_UI(shadingEnabledCheckbox));
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
    selectedButton = GRAPHICS_UI(shadingLevel32x32Button);
    if (shadingDepth == 64) {
      selectedButton = GRAPHICS_UI(shadingLevel32x64Button);
    }
    else if (shadingDepth == 128) {
      selectedButton = GRAPHICS_UI(shadingLevel32x128Button);
    }
  }
  else if (settingValue == 64) {
    selectedButton = GRAPHICS_UI(shadingLevel64x64Button);
    if (shadingDepth == 128) {
      selectedButton = GRAPHICS_UI(shadingLevel64x128Button);
    }
  }
  else {
    selectedButton = GRAPHICS_UI(shadingLevel128x128Button);
  }
  UiSelectableGroup_SelectExclusive(6,selectedButton,
      GRAPHICS_UI(shadingLevel128x128Button),
      GRAPHICS_UI(shadingLevel64x128Button),
      GRAPHICS_UI(shadingLevel64x64Button),
      GRAPHICS_UI(shadingLevel32x128Button),
      GRAPHICS_UI(shadingLevel32x64Button),
      GRAPHICS_UI(shadingLevel32x32Button));
  settingValue = PersistentSettings_Read(TEXTURE_QUALITY_MEDIUM,PERSISTENT_SETTING_TEXTURE_QUALITY);
  if (settingValue == TEXTURE_QUALITY_HIGH) {
    selectedButton = GRAPHICS_UI(textureQualityHighButton);
  }
  else if (settingValue == TEXTURE_QUALITY_MEDIUM) {
    selectedButton = GRAPHICS_UI(textureQualityMediumButton);
  }
  else {
    selectedButton = GRAPHICS_UI(textureQualityLowButton);
  }
  UiSelectableGroup_SelectExclusive(3,selectedButton,
      GRAPHICS_UI(textureQualityHighButton),
      GRAPHICS_UI(textureQualityMediumButton),
      GRAPHICS_UI(textureQualityLowButton));
  settingValue = PersistentSettings_Read(PERSISTENT_DEFAULT_MODEL_LOD_DEPTH_THRESHOLD,
                                         PERSISTENT_SETTING_MODEL_LOD_DEPTH_THRESHOLD);
  ((UiRangeSliderControl *)GRAPHICS_UI(modelDetailSlider))->value = settingValue;
#undef GRAPHICS_UI
  return;
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

#define AUDIO_PAGE THANDOR_CONTAINER_OF(settingsSourceNode, InGamePersistentSettingsPage3508, sourceNode)
  UiPageStack_SetActiveIndex(INGAME_WINDOW_PAGE_SOUND_SETTINGS,&AUDIO_PAGE->settingsPageStack);
  audioFlags = PersistentSettings_Read(PERSISTENT_SOUND_OPTION_DEFAULT,PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
  UiSelectableControl_SetSelected(audioFlags & PERSISTENT_SOUND_OPTION_EFFECTS,&AUDIO_PAGE->soundEffectsEnabledControl);
  UiSelectableControl_SetSelected(audioFlags & PERSISTENT_SOUND_OPTION_MUSIC,&AUDIO_PAGE->musicEnabledControl);
  UiSelectableControl_SetSelected(audioFlags & PERSISTENT_SOUND_OPTION_REVERSE_STEREO,&AUDIO_PAGE->reverseStereoControl);
  gainQ15 = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_EFFECTS_GAIN);
  (AUDIO_PAGE->soundEffectsGainControl).currentValue = gainQ15;
  gainQ15 = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_MOVIE_DEFAULT_GAIN);
  (AUDIO_PAGE->movieDefaultAudioGainControl).currentValue = gainQ15;
  gainQ15 = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_MOVIE_ALTERNATE_GAIN);
  (AUDIO_PAGE->movieAlternateAudioGainControl).currentValue = gainQ15;
  gainQ15 = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_MUSIC_GAIN);
  (AUDIO_PAGE->musicGainControl).currentValue = gainQ15;
#undef AUDIO_PAGE
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
  return;
}


/* UI action 0x1204 (shadingEnabledCheckbox; g_InGameUiActionHandlersPage12[4]): stores the shading switch,
   mirrors it to the world view's WORLD_RUNTIME_FLAG_SHADING_ENABLED and enables the shading level buttons
   only while shading is on.
*/
void InGameShadingSettings_SetEnabled(UiSelectableControl *control)

{
  uint8_t selectedState;

  selectedState = UiSelectableControl_IsSelected(control);
  while ((control->base).parent != UI_NODE_NONE) {
    control = (UiSelectableControl *)(control->base).parent;
  }
  if ((selectedState & 1) == 0) {
    UiNodeList_SuppressActionId(INGAME_ACTION_SHADING_LEVEL,&control->base);
    ((WorldRuntimeContext *)INGAME_UI(control,worldView))->runtimeFlags =
         ((WorldRuntimeContext *)INGAME_UI(control,worldView))->runtimeFlags & ~WORLD_RUNTIME_FLAG_SHADING_ENABLED;
  }
  else {
    UiNodeList_UnsuppressActionId(INGAME_ACTION_SHADING_LEVEL,&control->base);
    ((WorldRuntimeContext *)INGAME_UI(control,worldView))->runtimeFlags =
         ((WorldRuntimeContext *)INGAME_UI(control,worldView))->runtimeFlags | WORLD_RUNTIME_FLAG_SHADING_ENABLED;
  }
  PersistentSettings_Write(selectedState & 1,PERSISTENT_SETTING_SHADING_ENABLED);
  return;
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

  /* the button's value pair is (grid half size, depth); the depth is stored as a quarter */
  subresourceCount = (uint32_t)((UiNumericPairTextButton *)control)->secondValue >> 2;
  newSubresourceCount = subresourceCount;
  /* The option control stores the grid half size in firstValue; the texture dimension is twice
     that. */
  newGridHalfSize = (PersistentSettingsValue)((UiNumericPairTextButton *)control)->firstValue;
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
    if (newGridHalfSize == 32) {
      selectedControl = THANDOR_UI_SIBLING(shadingLevelGroup,InGameUiImage,shadingLevelGroup,shadingLevel32x32Button);
      if (shadingDepth == 64) {
        selectedControl = THANDOR_UI_SIBLING(shadingLevelGroup,InGameUiImage,shadingLevelGroup,shadingLevel32x64Button);
      }
      else if (shadingDepth == 128) {
        selectedControl = THANDOR_UI_SIBLING(shadingLevelGroup,InGameUiImage,shadingLevelGroup,shadingLevel32x128Button);
      }
    }
    else if (newGridHalfSize == 64) {
      selectedControl = THANDOR_UI_SIBLING(shadingLevelGroup,InGameUiImage,shadingLevelGroup,shadingLevel64x64Button);
      if (shadingDepth == 128) {
        selectedControl = THANDOR_UI_SIBLING(shadingLevelGroup,InGameUiImage,shadingLevelGroup,shadingLevel64x128Button);
      }
    }
    else {
      selectedControl = THANDOR_UI_SIBLING(shadingLevelGroup,InGameUiImage,shadingLevelGroup,shadingLevel128x128Button);
    }
    UiSelectableGroup_SelectExclusive(6,selectedControl,
      THANDOR_UI_SIBLING((control->base).parent,InGameUiImage,shadingLevelGroup,shadingLevel128x128Button),
      THANDOR_UI_SIBLING((control->base).parent,InGameUiImage,shadingLevelGroup,shadingLevel64x128Button),
      THANDOR_UI_SIBLING((control->base).parent,InGameUiImage,shadingLevelGroup,shadingLevel64x64Button),
      THANDOR_UI_SIBLING((control->base).parent,InGameUiImage,shadingLevelGroup,shadingLevel32x128Button),
      THANDOR_UI_SIBLING((control->base).parent,InGameUiImage,shadingLevelGroup,shadingLevel32x64Button),
      THANDOR_UI_SIBLING((control->base).parent,InGameUiImage,shadingLevelGroup,shadingLevel32x32Button));
    return;
  }
  textureDimension = PersistentSettings_Read(PERSISTENT_DEFAULT_SHADING_TEXTURE_DIMENSION,
                                            PERSISTENT_SETTING_SHADING_TEXTURE_DIMENSION);
  gridHalfSize = PersistentSettings_Read(PERSISTENT_DEFAULT_SHADING_GRID_HALF_SIZE,PERSISTENT_SETTING_SHADING_GRID_HALF_SIZE);
  storedSubresourceCount = PersistentSettings_Read(PERSISTENT_DEFAULT_SHADING_SUBRESOURCE_COUNT,
                                                  PERSISTENT_SETTING_SHADING_SUBRESOURCE_COUNT);
  GraphicsShadingRuntime_InitializeGeneratedTexture
            (storedSubresourceCount,gridHalfSize,textureDimension);
  return;
}


/* UI action 0x1206 (modelDetailSlider; g_InGameUiActionHandlersPage12[6]): stores the model detail slider
   value and makes it the model LOD depth threshold (Q8) at once.
*/
void InGameModelSettings_SetLodDepthThresholdQ8(UiSettingsValueControl *control)

{
  PersistentSettingsValue value;

  value = control->boundValue;
  PersistentSettings_Write(value,PERSISTENT_SETTING_MODEL_LOD_DEPTH_THRESHOLD);
  g_ModelLodDepthThresholdQ8 = value;
  return;
}


/* UI action 0x1207 (INGAME_ACTION_TEXTURE_QUALITY, the three texture quality buttons;
   g_InGameUiActionHandlersPage12[7]): selects the pressed button, stores its level and rebuilds every
   staging texture with it, showing the wait cursor meanwhile. Unlike the frontend version
   (FrontendTextureSettings_SetQuality, which uses level >> 1) the level itself becomes the downsample shift.
*/
void InGameTextureSettings_SetQuality(UiSelectableControl *control)

{
  PersistentTextureQualityLevel qualityLevel;
  UiNodeBase *selectedQualityControl;
  UiNodeBase *textureQualityGroup;

  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_BUSY);
  /* control is one of the texture quality buttons, its parent is textureQualityGroup */
  textureQualityGroup = (control->base).parent;
  if ((UiSelectableControl *)THANDOR_UI_SIBLING(textureQualityGroup,InGameUiImage,textureQualityGroup,textureQualityLowButton) == control) {
    qualityLevel = TEXTURE_QUALITY_LOW;
    selectedQualityControl = THANDOR_UI_SIBLING(textureQualityGroup,InGameUiImage,textureQualityGroup,textureQualityLowButton);
  }
  if ((UiSelectableControl *)THANDOR_UI_SIBLING(textureQualityGroup,InGameUiImage,textureQualityGroup,textureQualityMediumButton) == control) {
    qualityLevel = TEXTURE_QUALITY_MEDIUM;
    selectedQualityControl = THANDOR_UI_SIBLING(textureQualityGroup,InGameUiImage,textureQualityGroup,textureQualityMediumButton);
  }
  if ((UiSelectableControl *)THANDOR_UI_SIBLING(textureQualityGroup,InGameUiImage,textureQualityGroup,textureQualityHighButton) == control) {
    qualityLevel = TEXTURE_QUALITY_HIGH;
    selectedQualityControl = THANDOR_UI_SIBLING(textureQualityGroup,InGameUiImage,textureQualityGroup,textureQualityHighButton);
  }
  UiSelectableGroup_SelectExclusive(3,selectedQualityControl,
      THANDOR_UI_SIBLING((control->base).parent,InGameUiImage,textureQualityGroup,textureQualityHighButton),
      THANDOR_UI_SIBLING((control->base).parent,InGameUiImage,textureQualityGroup,textureQualityMediumButton),
      THANDOR_UI_SIBLING((control->base).parent,InGameUiImage,textureQualityGroup,textureQualityLowButton));
  PersistentSettings_Write(qualityLevel,PERSISTENT_SETTING_TEXTURE_QUALITY);
  g_TextureDownsampleShift = qualityLevel;
  g_GraphicsRebuildAllStagingTextures();
  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_ARROW);
  return;
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
    g_SoundStopVoice((IDirectSoundBuffer *)g_InGameActiveEffectVoice);
    g_InGameActiveEffectVoice = NULL;
  }
  audioFlags = PersistentSettings_Read(PERSISTENT_SOUND_OPTION_DEFAULT,PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
  PersistentSettings_Write((uint32_t)isEnabled | audioFlags & ~PERSISTENT_SOUND_OPTION_EFFECTS,
                           PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
  while ((control->base).parent != UI_NODE_NONE) {
    control = (UiSelectableControl *)(control->base).parent;
  }
  if (isEnabled) {
    UiNodeList_UnsuppressActionId(INGAME_ACTION_EFFECTS_VOLUME,&control->base);
    UiNodeList_UnsuppressActionId(INGAME_ACTION_MOVIE_VOLUME,&control->base);
    UiNodeList_UnsuppressActionId(INGAME_ACTION_MESSAGE_MOVIE_VOLUME,&control->base);
  }
  else {
    UiNodeList_SuppressActionId(INGAME_ACTION_EFFECTS_VOLUME,&control->base);
    UiNodeList_SuppressActionId(INGAME_ACTION_MOVIE_VOLUME,&control->base);
    UiNodeList_SuppressActionId(INGAME_ACTION_MESSAGE_MOVIE_VOLUME,&control->base);
  }
  if ((audioFlags & PERSISTENT_SOUND_OPTION_MUSIC) == 0) {
    UiNodeList_SuppressActionId(INGAME_ACTION_MUSIC_VOLUME,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(INGAME_ACTION_MUSIC_VOLUME,&control->base);
  }
  if (isEnabled == 0 && (audioFlags & PERSISTENT_SOUND_OPTION_MUSIC) == 0) {
    UiNodeList_SuppressActionId(INGAME_ACTION_REVERSE_STEREO,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(INGAME_ACTION_REVERSE_STEREO,&control->base);
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
  return;
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
    g_SoundStopVoice((IDirectSoundBuffer *)g_InGameActiveMusicVoice);
    g_InGameActiveMusicVoice = NULL;
    g_InGameMusicNextTrackCountdown = 1;
  }
  audioFlags = PersistentSettings_Read(PERSISTENT_SOUND_OPTION_DEFAULT,PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
  PersistentSettings_Write(musicEnabledBit | audioFlags & ~PERSISTENT_SOUND_OPTION_MUSIC,
                           PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
  while ((control->base).parent != UI_NODE_NONE) {
    control = (UiSelectableControl *)(control->base).parent;
  }
  if ((audioFlags & PERSISTENT_SOUND_OPTION_EFFECTS) == 0) {
    UiNodeList_SuppressActionId(INGAME_ACTION_EFFECTS_VOLUME,&control->base);
    UiNodeList_SuppressActionId(INGAME_ACTION_MOVIE_VOLUME,&control->base);
    UiNodeList_SuppressActionId(INGAME_ACTION_MESSAGE_MOVIE_VOLUME,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(INGAME_ACTION_EFFECTS_VOLUME,&control->base);
    UiNodeList_UnsuppressActionId(INGAME_ACTION_MOVIE_VOLUME,&control->base);
    UiNodeList_UnsuppressActionId(INGAME_ACTION_MESSAGE_MOVIE_VOLUME,&control->base);
  }
  if (musicEnabledBit == 0) {
    UiNodeList_SuppressActionId(INGAME_ACTION_MUSIC_VOLUME,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(INGAME_ACTION_MUSIC_VOLUME,&control->base);
  }
  if (musicEnabledBit == 0 && (audioFlags & PERSISTENT_SOUND_OPTION_EFFECTS) == 0) {
    UiNodeList_SuppressActionId(INGAME_ACTION_REVERSE_STEREO,&control->base);
  }
  else {
    UiNodeList_UnsuppressActionId(INGAME_ACTION_REVERSE_STEREO,&control->base);
  }
  return;
}


/* UI action 0x120A (reverseStereoCheckbox; g_InGameUiActionHandlersPage12[10]): stores the reverse stereo
   switch and sets g_ReverseStereoMask to all ones (swap the channels) or zero.
*/
void InGameAudioSettings_SetReverseStereo(UiSelectableControl *control)

{
  uint32_t currentAudioFlags;
  uint32_t reverseStereoBit;
  int32_t reverseStereoMask;
  Bool8 isSelected;

  reverseStereoBit = 0;
  reverseStereoMask = 0;
  isSelected = (Bool8)UiSelectableControl_IsSelected(control);
  if (isSelected) {
    reverseStereoBit = PERSISTENT_SOUND_OPTION_REVERSE_STEREO;
    reverseStereoMask = -1;
  }
  currentAudioFlags = PersistentSettings_Read(PERSISTENT_SOUND_OPTION_DEFAULT,PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
  g_ReverseStereoMask = reverseStereoMask;
  PersistentSettings_Write(reverseStereoBit | currentAudioFlags & ~PERSISTENT_SOUND_OPTION_REVERSE_STEREO,
                           PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
  return;
}


/* UI action 0x120B (effectsVolumeSlider; g_InGameUiActionHandlersPage12[11]): stores the effects volume,
   makes it the UI and effects gain and applies it to the playing effect voice so the change is audible.
*/
void InGameAudioSettings_SetEffectsGain(UiSettingsValueControl *control)

{
  PersistentSettingsValue value;

  value = control->boundValue;
  PersistentSettings_Write(value,PERSISTENT_SETTING_EFFECTS_GAIN);
  g_UiSoundGainQ15 = value;
  g_SoundEffectsGainQ15 = value;
  g_SoundSetVoiceGains(value,value,(IDirectSoundBuffer *)g_InGameActiveEffectVoice);
  return;
}


/* UI action 0x120C (movieVolumeSlider; g_InGameUiActionHandlersPage12[12]): stores the movie volume and
   makes it the default movie gain.
*/
void InGameAudioSettings_SetMovieDefaultGain(UiSettingsValueControl *control)

{
  PersistentSettingsValue value;

  value = control->boundValue;
  PersistentSettings_Write(value,PERSISTENT_SETTING_MOVIE_DEFAULT_GAIN);
  g_MovieDefaultAudioGainQ15 = value;
  return;
}


/* UI action 0x120D (musicVolumeSlider; g_InGameUiActionHandlersPage12[13]): stores the music volume and
   applies it to the playing music voice.
*/
void InGameAudioSettings_SetMusicGain(UiSettingsValueControl *control)

{
  PersistentSettingsValue value;

  value = control->boundValue;
  PersistentSettings_Write(value,PERSISTENT_SETTING_MUSIC_GAIN);
  g_SoundSetVoiceGains(value,value,(IDirectSoundBuffer *)g_InGameActiveMusicVoice);
  return;
}


/* UI action 0x121A (messageMovieVolumeSlider; g_InGameUiActionHandlersPage12[26]): stores the volume of the
   message movies and makes it the alternate movie gain used by timed movie playback.
*/
void InGameAudioSettings_SetMovieAlternateGain(UiSettingsValueControl *control)

{
  PersistentSettingsValue value;

  value = control->boundValue;
  PersistentSettings_Write(value,PERSISTENT_SETTING_MOVIE_ALTERNATE_GAIN);
  g_MovieAlternateAudioGainQ15 = value;
  return;
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
  isSelected = (Bool8)UiSelectableControl_IsSelected(settingsToggle);
  if (!isSelected) {
    UiPageStack_SetActiveIndex(INGAME_WINDOW_PAGE_NONE,(UiPageStackControl *)INGAME_UI(uiRoot,gameWindowPageStack));
    INGAME_UI(uiRoot,worldView)->nodeFlags &= ~UI_NODE_SUPPRESSED;
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
  UiSelectableControl_SetSelected(0,(UiSelectableControl *)INGAME_UI(uiRoot,missionObjectivesButton));
  INGAME_UI(uiRoot,worldView)->nodeFlags |= UI_NODE_SUPPRESSED;
  UiKeyboardFocus_ReleaseNode(INGAME_UI(uiRoot,worldView));
  UiPageStack_SetActiveIndex(INGAME_WINDOW_PAGE_GAME_MENU,(UiPageStackControl *)INGAME_UI(uiRoot,gameWindowPageStack));
  settingValue = PersistentSettings_Read(0,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS);
  UiSelectableControl_SetSelected(settingValue & PERSISTENT_MAP_OPTION_AUTOMATIC_ZOOM_OFF,
                                  (UiSelectableControl *)INGAME_UI(uiRoot,autoZoomOffCheckbox));
  UiSelectableControl_SetSelected(settingValue & PERSISTENT_MAP_OPTION_AUTOMATIC_ROTATION_OFF,
                                  (UiSelectableControl *)INGAME_UI(uiRoot,autoRotationOffCheckbox));
  /* "right button does not scroll" per the frontend settings page (the Tab key also toggles this bit) */
  UiSelectableControl_SetSelected
            (settingValue & PERSISTENT_MAP_OPTION_SIDE_PANEL_HIDDEN,(UiSelectableControl *)INGAME_UI(uiRoot,rightButtonNoScrollCheckbox));
  /* rotation is linked with zoom or with tilt; each excludes the other */
  settingValue = PersistentSettings_Read(0,PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS);
  if ((settingValue & PERSISTENT_LINK_OPTION_ROTATION_ZOOM) != 0) {
    UiNodeList_SuppressActionId(INGAME_ACTION_LINK_ROTATION_TILT,uiRoot);
  }
  UiSelectableControl_SetSelected
            (settingValue & PERSISTENT_LINK_OPTION_ROTATION_ZOOM,(UiSelectableControl *)INGAME_UI(uiRoot,linkRotationZoomCheckbox));
  if ((settingValue & PERSISTENT_LINK_OPTION_ROTATION_TILT) != 0) {
    UiNodeList_SuppressActionId(INGAME_ACTION_LINK_ROTATION_ZOOM,uiRoot);
  }
  UiSelectableControl_SetSelected
            (settingValue & PERSISTENT_LINK_OPTION_ROTATION_TILT,(UiSelectableControl *)INGAME_UI(uiRoot,linkRotationTiltCheckbox));
  settingValue = PersistentSettings_Read(PERSISTENT_DEFAULT_CAMERA_SCROLL_STEP,PERSISTENT_SETTING_CAMERA_SCROLL_STEP);
  ((UiRangeSliderControl *)INGAME_UI(uiRoot,scrollSpeedSlider))->value = settingValue;
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
  return;
}


/* Class vtables. */

InGameUiActionHandlerPage12Prefix28 g_InGameUiActionHandlersPage12 = {
        .handlers = {
            /*  0 */ THANDOR_FN(InGameQuitMenu_OpenAndRefreshButtons),
            /*  1 */ THANDOR_FN(InGameSettingsPage_CloseViaSharedToggle),
            /*  2 */ THANDOR_FN(InGameGraphicsSettings_OpenAndSynchronize),
            /*  3 */ THANDOR_FN(InGameAudioSettings_OpenAndSynchronize),
            /*  4 */ THANDOR_FN(InGameShadingSettings_SetEnabled),
            /*  5 */ THANDOR_FN(InGameShadingSettings_ApplyLevel),
            /*  6 */ THANDOR_FN(InGameModelSettings_SetLodDepthThresholdQ8),
            /*  7 */ THANDOR_FN(InGameTextureSettings_SetQuality),
            /*  8 */ THANDOR_FN(InGameAudioSettings_SetEffectsEnabled),
            /*  9 */ THANDOR_FN(InGameAudioSettings_SetMusicEnabled),
            /* 10 */ THANDOR_FN(InGameAudioSettings_SetReverseStereo),
            /* 11 */ THANDOR_FN(InGameAudioSettings_SetEffectsGain),
            /* 12 */ THANDOR_FN(InGameAudioSettings_SetMovieDefaultGain),
            /* 13 */ THANDOR_FN(InGameAudioSettings_SetMusicGain),
            /* 14 */ THANDOR_FN(InGameSaveGamePage_RebuildCatalog),
            /* 15 */ THANDOR_FN(InGameSaveGameList_SelectAndRefreshDetail),
            /* 16 */ THANDOR_FN(InGameSaveGame_SaveSelectedOrTypedName),
            /* 17 */ THANDOR_FN(InGameSaveName_UpdateSaveActionValidity),
            /* 18 */ THANDOR_FN(InGameGameplaySettings_SetAutomaticZoomOff),
            /* 19 */ THANDOR_FN(InGameGameplaySettings_SetAutomaticRotationOff),
            /* 20 */ THANDOR_FN(InGameGameplaySettings_SetLinkRotationZoom),
            /* 21 */ THANDOR_FN(InGameGameplaySettings_SetLinkRotationTilt),
            /* 22 */ THANDOR_FN(InGameGameplaySettings_SetRightButtonDoesNotScroll),
            /* 23 */ THANDOR_FN(InGameGameplaySettings_SetCameraScrollStep),
            /* 24 */ THANDOR_FN(InGameSettingsPage_OpenViaSharedToggle),
            /* 25 */ THANDOR_FN(InGameSaveGameAction_DeleteSelectedSaveAndRefreshCatalog),
            /* 26 */ THANDOR_FN(InGameAudioSettings_SetMovieAlternateGain),
            /* 27 */ THANDOR_FN(InGameGameplaySettings_SetHidePanel)
        }};
