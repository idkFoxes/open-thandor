/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/scenario.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/scenario.h>
#include <thandor/thandor.h>

/* Module data. */

/* the four level digits at index 7 are overwritten with the level number (ui/frontend/scenario.c) */
static uint16_t g_FrontendMissionBriefingMoviePathUtf16[16] = L"flm\\lev0000.flm";

/* Implementation ownership: ui/frontend/scenario. */

/* Title marker of one level on the game selection page: highlighted when one of the other players (records
   1..) has the level's bit clear in scenarioAvailabilityMask0..2[maskWordIndex], else normal. */
static uint16_t FrontendScenarioList_LevelAvailabilityMarker(uint32_t maskWordIndex,uint32_t levelMaskBit)
{
  FrontendPlayerRuntimeRecord *player;
  FrontendPlayerRuntimeBlockCount playersRemaining;

  player = g_FrontendPlayerRuntimeBlocks;
  playersRemaining = g_FrontendPlayerRuntimeBlockCount;
  while (--playersRemaining != 0) {
    player++;
    if (((&player->scenarioAvailabilityMask0)[maskWordIndex] & levelMaskBit) == 0) {
      return FRONTEND_TEXT_STYLE_HIGHLIGHTED;
    }
  }
  return FRONTEND_TEXT_STYLE_NORMAL;
}

/* Frame update of the frontend root: the frameUpdate callback of g_FrontendUiRootCallbacks, which Frontend_Init
   pushes on the UI root stack. Sorts the chat history, runs the timeout tick of the current network state,
   plays the briefing movie in a loop and the movie view's mask pattern, shows the chat line only in network
   games, updates the 3D menu room and the cursor from the hovered control, and on the game selection page
   (single games tab) marks each level title that some other player does not have.
*/

void FrontendRoot_TickNetworkPagesMovieCursorAndScenarioState(UiRootNode *rootCallbackContext)

{
  FrontendNetworkListsRuntimeView *frontendRoot;
  uint32_t networkState;
  UiNodeBase *hoveredNode;
  ScenarioCatalogDisplayRecord *levelRecord;
  GraphicsCursorFrameIndex cursorFrame;
  uint32_t levelMaskBit;
  ScenarioCatalogRecordCount levelsRemaining;
  uint32_t maskWordIndex;
  uint32_t activePageIndex;
  uint16_t *markerText;
  uint32_t selectedTabIndex;
  uint16_t availabilityMarker;
  
  networkState = g_FrontendNetworkState;
  frontendRoot = (FrontendNetworkListsRuntimeView *)g_FrontendRootNode;
  RecentTextHistory_SortAndBuildPointerList
            (5,(RecentTextHistoryPointerList *)&((UiConditionalActionControl *)FRONTEND_UI(g_FrontendRootNode,chatMessageHistory))->lineCount);
  switch(networkState) {
  case FRONTEND_NETWORK_STATE_BROWSING:
    FrontendSessionList_DecrementExpiryAndCompactRows(frontendRoot);
    break;
  case FRONTEND_NETWORK_STATE_HOSTING:
    FrontendPlayerRuntime_DecrementExpiryAndCompactBlocks(frontendRoot);
    break;
  case FRONTEND_NETWORK_STATE_JOINED:
    FrontendTransfer_TickRequestTimeoutAndResetPage(frontendRoot);
    break;
  case FRONTEND_NETWORK_STATE_HOST_STARTING:
    FrontendPlayerRuntime_DecrementTimeoutsAndRemoveExpiredPeers();
    break;
  case FRONTEND_NETWORK_STATE_CLIENT_STARTING:
    FrontendNetwork_TickDisconnectTimeoutAndResetSession();
  }
  if ((g_FrontendRuntimeFlags & FRONTEND_RUNTIME_FLAG_WAITING_FOR_PLAYERS) == 0) {
    /* the briefing image's movie (set by FrontendMissionBriefingPage_Initialize) plays in a loop */
    if ((((UiImageActionControl *)FRONTEND_UI(frontendRoot,briefingImage))->textureSource != NULL) &&
       !Movie_AdvanceFrame(NULL,NULL)) {
      Movie_Rewind();
    }
    if (((UiSoftwareTexturePreviewControl *)FRONTEND_UI(frontendRoot,moviePlaybackView))->textureSource != NULL) {
      SoftwareMaskBuffer_AdvancePatternByPercentTick
                ((SoftwareMaskRuntimeView *)FRONTEND_UI(frontendRoot,moviePlaybackView));
    }
  }
  /* bottom bar: empty page in a local game, the chat input line in a network game */
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    UiPageStack_SetActiveIndex(0,(UiPageStackControl *)FRONTEND_UI(frontendRoot,chatInputSlot));
  }
  else {
    UiPageStack_SetActiveIndex(1,(UiPageStackControl *)FRONTEND_UI(frontendRoot,chatInputSlot));
  }
  (*g_FrontendModelPointerContextVtable.pointerMove)
            (g_CursorOverrideY,g_CursorOverrideX,FRONTEND_UI(frontendRoot,menuRoomModelView));
  hoveredNode = (*((UiNodeBase *)frontendRoot)->vtable->hitTest)
                    (g_CursorOverrideY,g_CursorOverrideX,(UiNodeBase *)frontendRoot);
  if (hoveredNode == UI_NODE_NONE) {
    g_GraphicsCursorSetFrame(0);
  }
  else {
    cursorFrame = hoveredNode->vtable->pointerMove(g_CursorOverrideY,g_CursorOverrideX,hoveredNode);
    g_GraphicsCursorSetFrame(cursorFrame);
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    g_FrontendPlayerRuntimeBlocks->capabilityFlags = FRONTEND_CAPABILITY_CD;
  }
  activePageIndex = UiPageStack_ActivePageIndex
                     ((UiPageStackControl *)FRONTEND_UI(frontendRoot,frontendPageStack));
  if (activePageIndex == FRONTEND_PAGE_STACK_CHOOSE_GAME) {
    selectedTabIndex = UiSelectableGroup_SelectedIndex(3,
      FRONTEND_UI(g_FrontendRootNode,loadGameTabButton),
      FRONTEND_UI(g_FrontendRootNode,singleGameTabButton),
      FRONTEND_UI(g_FrontendRootNode,campaignsTabButton));
    /* none selected gives 3, never the single-games tab */
    if ((selectedTabIndex == SCENARIO_SELECTION_TAB_SINGLE_GAMES) && (g_ScenarioCatalog != NULL)) {
      levelsRemaining = g_ScenarioCatalog->levelRecordCount;
      levelRecord = (ScenarioCatalogDisplayRecord *)
                    ((uint8_t *)g_ScenarioCatalog + g_ScenarioCatalog->levelRecordsOffset);
      /* level n has bit n of the players' scenarioAvailabilityMask0..2; its title starts with the rich-text
         code 0x8001 (highlighted) when one of the other players (records 1..) lacks it, else 0x8000.
         The three mask words cover at most 96 levels; further levels are left unmarked. */
      levelMaskBit = 1;
      maskWordIndex = 0;
      while (levelsRemaining != 0) {
        availabilityMarker = FrontendScenarioList_LevelAvailabilityMarker(maskWordIndex,levelMaskBit);
        markerText = TextResource_Resolve(levelRecord->scenarioTextResourceId + TEXT_ID_LEVEL_TITLE_BASE);
        *markerText = availabilityMarker;
        levelRecord++;
        levelMaskBit = levelMaskBit * 2;
        if (levelMaskBit == 0) {
          maskWordIndex = maskWordIndex + 1;
          levelMaskBit = 1;
          if (2 < maskWordIndex) {
            break;
          }
        }
        levelsRemaining = levelsRemaining - 1;
      }
    }
  }
}


/* True when one of the players' records has factionSlot as its faction assignment (the player list is assumed
   to hold at least one record). */
static Bool8 FrontendMissionBriefing_IsFactionTakenByPlayer(int factionSlot)
{
  FrontendPlayerRuntimeRecord *playerRecord;
  FrontendPlayerRuntimeBlockCount playersRemaining;

  playerRecord = g_FrontendPlayerRuntimeBlocks;
  playersRemaining = g_FrontendPlayerRuntimeBlockCount;
  do {
    if (factionSlot == playerRecord->factionAssignment.factionAssignmentIndex) {
      return true;
    }
    playerRecord++;
    playersRemaining--;
  } while (playersRemaining != 0);
  return false;
}

/* Opens the mission briefing page (FRONTEND_PAGE_ACTION_MISSION_BRIEFING_PAGE) for the loaded level: the
   briefing text of the local player's faction, the level title in template 0x219B, the level's briefing movie
   (level digits + .flm) as animated image, and the button set (Back/Begin from the menu; Exit instead of Back
   in a campaign or a re-initialised scenario). The opponent settings stay visible only while an active
   faction is left to the computer.
*/
void FrontendMissionBriefingPage_Initialize(UiRootNode *frontendRoot)

{
  FrontendModelPointerContextFlags *menuRoomContextFlags;
  UiAnchorFractionQ31 *control;
  UiTextResourceId titleTextId;
  FrontendLoadedLevelAsset *loadedLevel;
  uint32_t savedGameSpeedPercent;
  int factionSlot;
  FrontendPlayerRuntimeBlockCount playersRemaining;
  FrontendPlayerRuntimeRecord *playerRecord;
  int unclaimedActiveFactions;
  RichTextExtent textExtent;
  uint16_t *briefingText;
  uint16_t *templateText;
  MovieRuntime *firstFrameMovie;
  uint32_t movieEndCode;

  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    savedGameSpeedPercent = PersistentSettings_Read(100,PERSISTENT_SETTING_GAME_SPEED_PERCENT);
    ((UiRangeSliderControl *)FRONTEND_UI(frontendRoot,gameSpeedSlider))->value = savedGameSpeedPercent;
  }
  UiPageStack_SetActiveIndex(FRONTEND_PAGE_MISSION_BRIEFING,
                             (UiPageStackControl *)FRONTEND_UI(frontendRoot,frontendPageStack));
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    menuRoomContextFlags = &((FrontendModelPointerContext *)FRONTEND_UI(frontendRoot,menuRoomModelView))->contextFlags;
    *menuRoomContextFlags = *menuRoomContextFlags | FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  /* find the local player's record (Original quirk: one past the last record if none matches) */
  loadedLevel = g_FrontendLoadedLevelAsset;
  playersRemaining = g_FrontendPlayerRuntimeBlockCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  while (g_LocalPlayerRuntimeId != playerRecord->playerRuntimeId) {
    playerRecord++;
    playersRemaining--;
    if (playersRemaining == 0) break;
  }
  titleTextId = g_FrontendLoadedLevelAsset->header.titleTextResourceIndex;
  /* briefingText's text resource id: the faction's briefing entry of the level's text page */
  ((UiWrappedTextControl *)FRONTEND_UI(frontendRoot,briefingText))->text =
       (uint16_t *)(playerRecord->factionAssignment.factionAssignmentIndex + TEXT_ID_LEVEL_BRIEFING_BASE +
       g_FrontendLoadedLevelAsset->header.titleTextResourceIndex * TEXT_ID_LEVEL_DESCRIPTION_STRIDE);
  briefingText = TextResource_Resolve(titleTextId + TEXT_ID_LEVEL_TITLE_BASE);
  *briefingText = FRONTEND_TEXT_STYLE_NORMAL;
  templateText = TextResource_Resolve(TEXT_ID_MISSION_BRIEFING_TEMPLATE);
  RichTextCommandStream_PatchPayloadBySelector(0,briefingText,templateText);
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_PAD_WITH_ZERO,0,4,1,loadedLevel->header.titleTextResourceIndex,
             &g_FrontendMissionBriefingMoviePathUtf16[7]);
  WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_FLM,g_FrontendMissionBriefingMoviePathUtf16);
  if (!Movie_Open(MOVIE_OPEN_PACKAGE_ONLY,g_FrontendMissionBriefingMoviePathUtf16,NULL,NULL)) {
    ((UiImageActionControl *)FRONTEND_UI(frontendRoot,briefingImage))->textureSource = NULL;
  }
  else {
    /* Original quirk: the result is not checked; when no frame comes the end code becomes the texture source */
    if (Movie_AdvanceFrame(&firstFrameMovie,&movieEndCode)) {
      ((UiImageActionControl *)FRONTEND_UI(frontendRoot,briefingImage))->textureSource =
           (GraphicsTextureSourceAsset *)firstFrameMovie;
    }
    else {
      ((UiImageActionControl *)FRONTEND_UI(frontendRoot,briefingImage))->textureSource =
           (GraphicsTextureSourceAsset *)movieEndCode;
    }
    ((UiImageActionControl *)FRONTEND_UI(frontendRoot,briefingImage))->subresource = 0;
  }
  /* Back only when started from the menu by a non-client; Exit (and the Save button) in a campaign or a
     re-initialised scenario; neither for a client of a fresh scenario */
  if ((((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) &&
      (g_FrontendLoadedCampaignAsset == 0)) && (g_FrontendScenarioInitializationCount == 0)) {
    FRONTEND_UI(frontendRoot,briefingBackButton)->nodeFlags &= ~UI_NODE_SUPPRESSED;
    ((UiSelectableControl *)FRONTEND_UI(frontendRoot,briefingBackButton))->stateFlags &= ~FRONTEND_CONTROL_INACTIVE;
  }
  else {
    FRONTEND_UI(frontendRoot,briefingBackButton)->nodeFlags |= UI_NODE_SUPPRESSED;
    ((UiSelectableControl *)FRONTEND_UI(frontendRoot,briefingBackButton))->stateFlags |= FRONTEND_CONTROL_INACTIVE;
  }
  /* (when Back was shown, both of these are zero) */
  if ((g_FrontendScenarioInitializationCount != 0) || (g_FrontendLoadedCampaignAsset != 0)) {
    FRONTEND_UI(frontendRoot,briefingExitButton)->nodeFlags &= ~UI_NODE_SUPPRESSED;
    ((UiSelectableControl *)FRONTEND_UI(frontendRoot,briefingExitButton))->stateFlags &=
         ~FRONTEND_CONTROL_INACTIVE;
    /* the original shows the Save button and switches it off again right away */
    FRONTEND_UI(frontendRoot,briefingSaveButton)->nodeFlags &= ~UI_NODE_SUPPRESSED;
    ((UiSelectableControl *)FRONTEND_UI(frontendRoot,briefingSaveButton))->stateFlags &=
         ~FRONTEND_CONTROL_INACTIVE;
    FRONTEND_UI(frontendRoot,briefingSaveButton)->nodeFlags |= UI_NODE_SUPPRESSED;
    ((UiSelectableControl *)FRONTEND_UI(frontendRoot,briefingSaveButton))->stateFlags |=
         FRONTEND_CONTROL_INACTIVE;
  }
  else {
    FRONTEND_UI(frontendRoot,briefingExitButton)->nodeFlags |= UI_NODE_SUPPRESSED;
    ((UiSelectableControl *)FRONTEND_UI(frontendRoot,briefingExitButton))->stateFlags |= FRONTEND_CONTROL_INACTIVE;
    FRONTEND_UI(frontendRoot,briefingSaveButton)->nodeFlags |= UI_NODE_SUPPRESSED;
    ((UiSelectableControl *)FRONTEND_UI(frontendRoot,briefingSaveButton))->stateFlags |= FRONTEND_CONTROL_INACTIVE;
  }
  /* in a network game with other players only the host starts the mission */
  FRONTEND_UI(frontendRoot,briefingBeginButton)->nodeFlags &= ~UI_NODE_SUPPRESSED;
  if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) &&
     (1 < g_FrontendPlayerRuntimeBlockCount)) {
    FRONTEND_UI(frontendRoot,briefingBeginButton)->nodeFlags |= UI_NODE_SUPPRESSED;
  }
  playersRemaining = g_FrontendPlayerRuntimeBlockCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  do {
    playerRecord->factionAssignment.readyOrWaitState = 0;
    playersRemaining--;
    playerRecord++;
  } while (playersRemaining != 0);
  briefingText = TextResource_Resolve((TextResourceId)((UiWrappedTextControl *)FRONTEND_UI(frontendRoot,briefingText))->text);
  textExtent = RichTextCommandStream_MeasureWrappedBlock
                     (g_UiTextStyleNormal,briefingText,((UiWrappedTextControl *)FRONTEND_UI(frontendRoot,briefingText))->wrapWidth);
  /* size the text control to the wrapped text plus a 6-pixel margin, then refit the scroller */
  FRONTEND_UI(frontendRoot,briefingText)->rightOffset = textExtent.widthPixels + 6;
  FRONTEND_UI(frontendRoot,briefingText)->bottomOffset = textExtent.heightPixels + 6;
  control = (UiAnchorFractionQ31 *)FRONTEND_UI(frontendRoot,briefingTextScroller);
  UiScrollableControl_RebuildViewportAndScrollbars((UiScrollableControl *)control);
  UiScrollableControl_ClampOffsetsToViewport(0,0,0,0,(UiScrollableControl *)control);
  /* The "computer opponent" slider (weak..strong) is the game speed percent; clients cannot change it. */
  UiNodeList_UnsuppressActionId(FRONTEND_ACTION_GAME_SPEED,&frontendRoot->base);
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_GAME_SPEED,&frontendRoot->base);
  }
  /* count the active factions 1..7 that no player has taken (they are played by the computer) */
  unclaimedActiveFactions = 0;
  for (factionSlot = 1; factionSlot <= 7; factionSlot++) {
    if ((g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionSlot] == FACTION_RUNTIME_LIFECYCLE_ACTIVE) &&
        !FrontendMissionBriefing_IsFactionTakenByPlayer(factionSlot)) {
      unclaimedActiveFactions++;
    }
  }
  /* opponent settings: only with computer factions, and in a campaign only on its first level
     (CampaignAsset.firstLevelId) */
  if ((g_FrontendLoadedCampaignAsset == 0 ||
       ((CampaignAsset *)g_FrontendLoadedCampaignAsset)->currentLevelId ==
       ((CampaignAsset *)g_FrontendLoadedCampaignAsset)->firstLevelId) && unclaimedActiveFactions != 0) {
    FRONTEND_UI(frontendRoot,opponentSettingsGroup)->nodeFlags &= ~UI_NODE_SUPPRESSED;
  }
  else {
    FRONTEND_UI(frontendRoot,opponentSettingsGroup)->nodeFlags |= UI_NODE_SUPPRESSED;
    UiNodeList_SuppressActionId(FRONTEND_ACTION_GAME_SPEED,&frontendRoot->base);
  }
}

