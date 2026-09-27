/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/scenario.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/scenario.h>
#include <thandor/thandor.h>

/* Implementation ownership: ui/frontend/scenario. */

/* Address: 0x00547D60.
   Ownership: ui/frontend/scenario.
   Purpose: This function object claims Listing ownership for a previously unowned multi-entry/shared-
   tail/computed-dispatch region; it does not assert that every member entry is an independent ABI-level function.
   Body boundaries remain exact and are not split into speculative ABI functions. Frontend root callback contract:
   one UiRootNode* callback-context argument at stack +4, callee cleanup 4, void return. EAX and EDX are
   preserved/incidental state, not a semantic qword result.
   Cross-module calls: RecentTextHistory_SortAndBuildPointerList [ui/support/runtime],
   FrontendSessionList_DecrementExpiryAndCompactRows [ui/frontend/session],
   FrontendPlayerRuntime_DecrementExpiryAndCompactBlocks [ui/frontend/player],
   FrontendTransfer_TickRequestTimeoutAndResetPage [network/protocol/transfer],
   FrontendPlayerRuntime_DecrementTimeoutsAndRemoveExpiredPeers [ui/frontend/player],
   FrontendNetwork_TickDisconnectTimeoutAndResetSession [network/backend/runtime].
*/

void __thandor_void_preserve_eax_ecx_edx
FrontendRoot_TickNetworkPagesMovieCursorAndScenarioState(UiRootNode *rootCallbackContext)

{
  FrontendPlayerRuntimeBlockCount playersRemaining;
  FrontendPlayerRuntimeRecord *playerCursor;
  FrontendPlayerRuntimeRecord *nextPlayer;
  FrontendNetworkListsRuntimeView5650 *frontendRoot;
  uint32_t networkState;
  UiNodeBase *hoveredNode;
  int levelRecordAddress;
  GraphicsCursorFrameIndex cursorFrame;
  uint32_t levelMaskBit;
  ScenarioCatalogRecordCount levelsRemaining;
  uint32_t maskWordIndex;
  MovieFrameResult movieFrame;
  PageStackSearchResult activePageStatus;
  TextResolveResult markerText;
  SelectableGroupIndexResult selectedGroup;
  uint16_t availabilityMarker;
  
  networkState = g_FrontendNetworkState;
  frontendRoot = g_FrontendRootNode;
  RecentTextHistory_SortAndBuildPointerList
            (5,(RecentTextHistoryPointerList *)(((struct FrontendNetworkListsRuntimeView5650 *)(uintptr_t)g_FrontendRootNode)->opaqueGap0000_4B67 + 0x350));
                    // WARNING: Switch is manually overridden
  switch(networkState) {
  case 1:
    FrontendSessionList_DecrementExpiryAndCompactRows(frontendRoot);
    break;
  case 2:
    FrontendPlayerRuntime_DecrementExpiryAndCompactBlocks(frontendRoot);
    break;
  case 3:
    FrontendTransfer_TickRequestTimeoutAndResetPage(frontendRoot);
    break;
  case 4:
    FrontendPlayerRuntime_DecrementTimeoutsAndRemoveExpiredPeers();
    break;
  case 5:
    FrontendNetwork_TickDisconnectTimeoutAndResetSession();
  }
  if ((g_FrontendRuntimeFlags & 0x10) == 0) {
    if ((*(int *)(frontendRoot->opaqueGap0000_4B67 + 0x904) != 0) &&
       (movieFrame = Movie_AdvanceFrame(), movieFrame.ended)) {
      Movie_Rewind();
    }
    if (*(int *)(frontendRoot->opaqueGap0000_4B67 + 0x224) != 0) {
      SoftwareMaskBuffer_AdvancePatternByPercentTick
                ((SoftwareMaskRuntimeView *)(frontendRoot->opaqueGap0000_4B67 + 0x1d4));
    }
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    UiPageStack_SetActiveIndex(0,(UiPageStackControl *)(frontendRoot->opaqueGap0000_4B67 + 0xb0));
  }
  else {
    UiPageStack_SetActiveIndex(1,(UiPageStackControl *)(frontendRoot->opaqueGap0000_4B67 + 0xb0));
  }
  g_FrontendModelPointerContextUpdateCallback
            (g_CursorOverrideY,g_CursorOverrideX,
             (FrontendModelPointerContextRuntimeState118 *)
             (frontendRoot->opaqueGap0000_4B67 + 0x368));
  hoveredNode = (*((UiNodeBase *)frontendRoot)->vtable->hitTest)
                    (g_CursorOverrideY,g_CursorOverrideX,(UiNodeBase *)frontendRoot);
  if (hoveredNode == (UiNodeBase *)0xffffffff) {
    g_GraphicsCursorSetFrame(0);
  }
  else {
    cursorFrame = hoveredNode->vtable->pointerMove(g_CursorOverrideY,g_CursorOverrideX,hoveredNode);
    g_GraphicsCursorSetFrame(cursorFrame);
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    g_FrontendPlayerRuntimeBlocks->capabilityFlags = 0x100;
  }
  activePageStatus = UiPageStack_ActivePageNotInList
                     ((UiPageStackControl *)(frontendRoot->opaqueGap0000_4B67 + 0x508));
  if (activePageStatus.pageIndex == 10) {
    selectedGroup = UiSelectableGroup_NoneSelected(3,
      FRONTEND_UI(g_FrontendRootNode,loadGameTabButton),
      FRONTEND_UI(g_FrontendRootNode,singleGameTabButton),
      FRONTEND_UI(g_FrontendRootNode,campaignsTabButton));
    if (((!selectedGroup.noneSelected) && (selectedGroup.selectedIndexOrCount == 1)) &&
       (g_ScenarioCatalog != (ScenarioCatalogHeader *)0x0)) {
      levelsRemaining = g_ScenarioCatalog->levelRecordCount;
      levelRecordAddress = (int)&g_ScenarioCatalog->levelRecordsOffset + g_ScenarioCatalog->levelRecordsOffset;
      if (levelsRemaining != 0) {
        levelMaskBit = 1;
        maskWordIndex = 0;
        do {
          availabilityMarker = 0x8001;
          playerCursor = g_FrontendPlayerRuntimeBlocks;
          playersRemaining = g_FrontendPlayerRuntimeBlockCount;
          do {
            playersRemaining = playersRemaining - 1;
            if (playersRemaining == 0) {
              availabilityMarker = 0x8000;
              break;
            }
            nextPlayer = playerCursor + 1;
            playerCursor = playerCursor + 1;
          } while ((*(uint32_t *)(nextPlayer->reserved78_7F + maskWordIndex * 4 + 0xc) & levelMaskBit) != 0);
          markerText = TextResource_Resolve(*(int *)(levelRecordAddress + 0x70) + 0x2230);
          *markerText.text = availabilityMarker;
          levelRecordAddress = levelRecordAddress + 0x100;
          levelMaskBit = levelMaskBit * 2;
          if (levelMaskBit == 0) {
            maskWordIndex = maskWordIndex + 1;
            levelMaskBit = 1;
            if (2 < maskWordIndex) {
              return;
            }
          }
          levelsRemaining = levelsRemaining - 1;
        } while (levelsRemaining != 0);
      }
    }
  }
  return;
}


/* Address: 0x0054C9F0.
   Opens the mission briefing page (FRONTEND_PAGE_ACTION_MISSION_BRIEFING_PAGE) for the loaded level: the
   briefing text of the local player's faction, the level title in template 0x219B, the level's briefing movie
   (level digits + .flm) as animated image, and the button set (Back/Begin from the menu; Exit instead of Back
   in a campaign or a re-initialised scenario). The opponent settings stay visible only while an active
   faction is left to the computer.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendMissionBriefingPage_Initialize(UiRootNode *frontendRoot)

{
  int32_t *menuRoomContextFlags;
  UiAnchorFractionQ31 *control;
  UiTextResourceId titleTextId;
  FrontendLoadedLevelRuntimeImage370 *loadedLevel;
  uint32_t savedGameSpeedPercent;
  int factionSlot;
  FrontendPlayerRuntimeBlockCount playersRemaining;
  int factionsRemaining;
  FrontendPlayerRuntimeRecord *playerRecord;
  int unclaimedActiveFactions;
  FactionRuntimeLifecycleObservedState *factionStateCursor;
  RichTextExtentRegs textExtent;
  TextResolveResult briefingText;
  TextResolveResult templateText;
  MovieOpenResult movieOpen;
  MovieFrameResult firstFrame;

  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    savedGameSpeedPercent = PersistentSettings_Read(100,PERSISTENT_SETTING_GAME_SPEED_PERCENT);
    ((UiRangeSliderControl *)FRONTEND_UI(frontendRoot,gameSpeedSlider))->value = savedGameSpeedPercent;
  }
  UiPageStack_SetActiveIndex(FRONTEND_PAGE_MISSION_BRIEFING,
                             (UiPageStackControl *)FRONTEND_UI(frontendRoot,frontendPageStack));
  playersRemaining = g_FrontendPlayerRuntimeBlockCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    menuRoomContextFlags = &FRONTEND_UI_FIELD(frontendRoot,menuRoomModelView,0x4C,int32_t);
    *menuRoomContextFlags = *menuRoomContextFlags | FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
    playersRemaining = g_FrontendPlayerRuntimeBlockCount;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
  }
  /* find the local player's record (the last one if none matches) */
  do {
    loadedLevel = g_FrontendLoadedLevelAsset;
    if (g_LocalPlayerRuntimeId == playerRecord->playerRuntimeId) break;
    playerRecord++;
    playersRemaining--;
  } while (playersRemaining != 0);
  titleTextId = (g_FrontendLoadedLevelAsset->header).titleTextResourceIndex;
  /* briefingText's text resource id: level text page entry 0x230017 + faction + level * 0x10 */
  FRONTEND_UI_FIELD(frontendRoot,briefingText,0x54,int32_t) =
       (playerRecord->factionAssignment).factionAssignmentIndex + 0x230017 +
       (g_FrontendLoadedLevelAsset->header).titleTextResourceIndex * 0x10;
  briefingText = TextResource_Resolve(titleTextId + TEXT_ID_LEVEL_TITLE_BASE);
  *briefingText.text = 0x8000;
  templateText = TextResource_Resolve(TEXT_ID_MISSION_BRIEFING_TEMPLATE);
  RichTextCommandStream_PatchPayloadBySelector(0,briefingText.text,templateText.text);
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_PAD_WITH_ZERO,0,4,1,(loadedLevel->header).titleTextResourceIndex,
             (uint16_t *)&g_FrontendMissionBriefingLevelDigitsUtf16);
  WidePath_SetExtensionCode(0x6d6c66 /* "flm" */,(uint16_t *)&g_FrontendMissionBriefingMoviePathUtf16);
  movieOpen = Movie_Open(0x80000000,(uint16_t *)&g_FrontendMissionBriefingMoviePathUtf16);
  if (movieOpen.failed) {
    FRONTEND_UI_FIELD(frontendRoot,briefingImage,0x54,int32_t) = 0;
  }
  else {
    firstFrame = Movie_AdvanceFrame();
    FRONTEND_UI_FIELD(frontendRoot,briefingImage,0x54,int32_t) = firstFrame.movieOrError;
    FRONTEND_UI_FIELD(frontendRoot,briefingImage,0x58,int32_t) = 0;
  }
  if ((((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) &&
      (g_FrontendLoadedCampaignAsset == 0)) && (g_FrontendScenarioInitializationCount == 0)) {
    FRONTEND_UI(frontendRoot,briefingBackButton)->nodeFlags &= ~UI_NODE_SUPPRESSED;
    ((UiSelectableControl *)FRONTEND_UI(frontendRoot,briefingBackButton))->stateFlags &= ~FRONTEND_CONTROL_INACTIVE;
  }
  else {
    FRONTEND_UI(frontendRoot,briefingBackButton)->nodeFlags |= UI_NODE_SUPPRESSED;
    ((UiSelectableControl *)FRONTEND_UI(frontendRoot,briefingBackButton))->stateFlags |= FRONTEND_CONTROL_INACTIVE;
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
      goto FrontendMissionBriefing_InitializePlayerReadinessAndLayout;
    }
  }
  FRONTEND_UI(frontendRoot,briefingExitButton)->nodeFlags |= UI_NODE_SUPPRESSED;
  ((UiSelectableControl *)FRONTEND_UI(frontendRoot,briefingExitButton))->stateFlags |= FRONTEND_CONTROL_INACTIVE;
  FRONTEND_UI(frontendRoot,briefingSaveButton)->nodeFlags |= UI_NODE_SUPPRESSED;
  ((UiSelectableControl *)FRONTEND_UI(frontendRoot,briefingSaveButton))->stateFlags |= FRONTEND_CONTROL_INACTIVE;
FrontendMissionBriefing_InitializePlayerReadinessAndLayout:
  /* in a network game with other players only the host starts the mission */
  FRONTEND_UI(frontendRoot,briefingBeginButton)->nodeFlags &= ~UI_NODE_SUPPRESSED;
  playersRemaining = g_FrontendPlayerRuntimeBlockCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) &&
     (1 < g_FrontendPlayerRuntimeBlockCount)) {
    FRONTEND_UI(frontendRoot,briefingBeginButton)->nodeFlags |= UI_NODE_SUPPRESSED;
    playersRemaining = g_FrontendPlayerRuntimeBlockCount;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
  }
  do {
    (playerRecord->factionAssignment).readyOrWaitState = 0;
    playersRemaining--;
    playerRecord++;
  } while (playersRemaining != 0);
  briefingText = TextResource_Resolve(FRONTEND_UI_FIELD(frontendRoot,briefingText,0x54,int32_t));
  textExtent = RichTextCommandStream_MeasureWrappedBlockRegs
                     (g_UiTextStyleNormal,briefingText.text,(UiPixelExtent)FRONTEND_UI_FIELD(frontendRoot,briefingText,0x50,struct UiNodeVtable *));
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
  factionStateCursor = g_GameFactionRuntimeImage.tail.factionLifecycleStates;
  factionsRemaining = 7;
  factionSlot = 1;
  unclaimedActiveFactions = 0;
  do {
    factionStateCursor++;
    playersRemaining = g_FrontendPlayerRuntimeBlockCount;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
    if (*factionStateCursor == FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
      do {
        if (factionSlot == (playerRecord->factionAssignment).factionAssignmentIndex)
        goto FrontendMissionBriefing_AdvanceFactionAvailabilityScan;
        playerRecord++;
        playersRemaining--;
      } while (playersRemaining != 0);
      unclaimedActiveFactions++;
    }
FrontendMissionBriefing_AdvanceFactionAvailabilityScan:
    factionSlot++;
    factionsRemaining--;
    if (factionsRemaining == 0) {
      /* opponent settings: only with computer factions, and in a campaign only on the level at +0xB4
         (compared with the current level id at +0xC4) */
      if (((g_FrontendLoadedCampaignAsset == 0) ||
          (*(int *)(g_FrontendLoadedCampaignAsset + 0xc4) ==
           *(int *)(g_FrontendLoadedCampaignAsset + 0xb4))) && (unclaimedActiveFactions != 0)) {
        FRONTEND_UI(frontendRoot,opponentSettingsGroup)->nodeFlags &= ~UI_NODE_SUPPRESSED;
      }
      else {
        FRONTEND_UI(frontendRoot,opponentSettingsGroup)->nodeFlags |= UI_NODE_SUPPRESSED;
        UiNodeList_SuppressActionId(FRONTEND_ACTION_GAME_SPEED,&frontendRoot->base);
      }
      return;
    }
  } while( true );
}

