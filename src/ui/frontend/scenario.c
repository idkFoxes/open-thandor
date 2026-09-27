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
  (*g_FrontendModelPointerContextUpdateCallback)
            (g_CursorOverrideY,g_CursorOverrideX,
             (FrontendModelPointerContextRuntimeState118 *)
             (frontendRoot->opaqueGap0000_4B67 + 0x368));
  hoveredNode = (*((UiNodeBase *)frontendRoot)->vtable->hitTest)
                    (g_CursorOverrideY,g_CursorOverrideX,(UiNodeBase *)frontendRoot);
  if (hoveredNode == (UiNodeBase *)0xffffffff) {
    (*g_GraphicsCursorSetFrame)(0);
  }
  else {
    cursorFrame = (*hoveredNode->vtable->pointerMove)(g_CursorOverrideY,g_CursorOverrideX,hoveredNode);
    (*g_GraphicsCursorSetFrame)(cursorFrame);
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    g_FrontendPlayerRuntimeBlocks->capabilityFlags = 0x100;
  }
  activePageStatus = UiPageStack_ActivePageNotInListCf
                     ((UiPageStackControl *)(frontendRoot->opaqueGap0000_4B67 + 0x508));
  if (activePageStatus.pageIndex == 10) {
    selectedGroup = UiSelectableGroup_NoneSelectedCf(3,
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
   Ownership: ui/frontend/scenario.
   Purpose: Activates mission page 12, resolves the current player/faction briefing, patches localized mission-
   description template 0x219B, constructs the FLM path, opens the movie, and updates page actions.
   Cross-module calls: PersistentSettings_ReadDword [core/settings/persistent], UiPageStack_SetActiveIndex
   [ui/controls/layout], TextResource_Resolve [assets/text/resources], RichTextCommandStream_PatchPayloadBySelector
   [assets/text/richtext], WidePath_SetExtensionCode [core/text/path], Movie_Open [movie/runtime/playback].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendMissionBriefingPage_Initialize(UiRootNode *frontendRoot)

{
  int32_t *layoutField;
  UiNodeBase **nodeLinkField;
  UiNodeVtable **vtableField;
  UiAnchorFractionQ31 *control;
  UiTextResourceId titleTextId;
  FrontendLoadedLevelRuntimeImage370 *loadedLevel;
  uint32_t savedSettingValue;
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
    savedSettingValue = PersistentSettings_Read(100,0x44);
    FRONTEND_UI_FIELD(frontendRoot,gameSpeedSlider,0x58,uint32_t) = savedSettingValue;
  }
  UiPageStack_SetActiveIndex(0xc,(UiPageStackControl *)FRONTEND_UI(frontendRoot,frontendPageStack));
  playersRemaining = g_FrontendPlayerRuntimeBlockCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  if ((int)g_FramebufferWidth < 0x281) {
    layoutField = &FRONTEND_UI_FIELD(frontendRoot,menuRoomModelView,0x4C,int32_t);
    *layoutField = *layoutField | 0x2000;
    playersRemaining = g_FrontendPlayerRuntimeBlockCount;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
  }
  do {
    loadedLevel = g_FrontendLoadedLevelAsset;
    if (g_LocalPlayerRuntimeId == playerRecord->playerRuntimeId) break;
    playerRecord = playerRecord + 1;
    playersRemaining = playersRemaining - 1;
  } while (playersRemaining != 0);
  titleTextId = (g_FrontendLoadedLevelAsset->header).titleTextResourceIndex;
  FRONTEND_UI_FIELD(frontendRoot,briefingText,0x54,int32_t) =
       (playerRecord->factionAssignment).factionAssignmentIndex + 0x230017 +
       (g_FrontendLoadedLevelAsset->header).titleTextResourceIndex * 0x10;
  briefingText = TextResource_Resolve(titleTextId + 0x2230);
  *briefingText.text = 0x8000;
  templateText = TextResource_Resolve(0x219b);
  RichTextCommandStream_PatchPayloadBySelector(0,briefingText.text,templateText.text);
  (*g_WideNumberFormatUtf16)
            (WIDE_FORMAT_PAD_WITH_ZERO,0,4,1,(loadedLevel->header).titleTextResourceIndex,
             (uint16_t *)&g_FrontendMissionBriefingLevelDigitsUtf16);
  WidePath_SetExtensionCode(0x6d6c66,(uint16_t *)&g_FrontendMissionBriefingMoviePathUtf16);
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
    FRONTEND_UI_FIELD(frontendRoot,briefingBackButton,0x48,struct UiNodeBase *) =
         (UiNodeBase *)((uint32_t)FRONTEND_UI_FIELD(frontendRoot,briefingBackButton,0x48,struct UiNodeBase *) & 0xfffffff7);
    nodeLinkField = &FRONTEND_UI_FIELD(frontendRoot,briefingBackButton,0x4C,struct UiNodeBase *);
    *nodeLinkField = (UiNodeBase *)((uint32_t)*nodeLinkField & 0xfffffbff);
  }
  else {
    FRONTEND_UI_FIELD(frontendRoot,briefingBackButton,0x48,struct UiNodeBase *) =
         (UiNodeBase *)((uint32_t)FRONTEND_UI_FIELD(frontendRoot,briefingBackButton,0x48,struct UiNodeBase *) | 8);
    nodeLinkField = &FRONTEND_UI_FIELD(frontendRoot,briefingBackButton,0x4C,struct UiNodeBase *);
    *nodeLinkField = (UiNodeBase *)((uint32_t)*nodeLinkField | 0x400);
    if ((g_FrontendScenarioInitializationCount != 0) || (g_FrontendLoadedCampaignAsset != 0)) {
      nodeLinkField = &FRONTEND_UI_FIELD(frontendRoot,briefingExitButton,0x48,struct UiNodeBase *);
      *nodeLinkField = (UiNodeBase *)((uint32_t)*nodeLinkField & 0xfffffff7);
      vtableField = &FRONTEND_UI_FIELD(frontendRoot,briefingExitButton,0x4C,struct UiNodeVtable *);
      *vtableField = (UiNodeVtable *)((uint32_t)*vtableField & 0xfffffbff);
      layoutField = &FRONTEND_UI_FIELD(frontendRoot,briefingSaveButton,0x48,int32_t);
      *layoutField = *layoutField & 0xfffffff7;
      layoutField = &FRONTEND_UI_FIELD(frontendRoot,briefingSaveButton,0x4C,int32_t);
      *layoutField = *layoutField & 0xfffffbff;
      layoutField = &FRONTEND_UI_FIELD(frontendRoot,briefingSaveButton,0x48,int32_t);
      *layoutField = *layoutField | 8;
      layoutField = &FRONTEND_UI_FIELD(frontendRoot,briefingSaveButton,0x4C,int32_t);
      *layoutField = *layoutField | 0x400;
      goto FrontendMissionBriefing_InitializePlayerReadinessAndLayout;
    }
  }
  nodeLinkField = &FRONTEND_UI_FIELD(frontendRoot,briefingExitButton,0x48,struct UiNodeBase *);
  *nodeLinkField = (UiNodeBase *)((uint32_t)*nodeLinkField | 8);
  vtableField = &FRONTEND_UI_FIELD(frontendRoot,briefingExitButton,0x4C,struct UiNodeVtable *);
  *vtableField = (UiNodeVtable *)((uint32_t)*vtableField | 0x400);
  layoutField = &FRONTEND_UI_FIELD(frontendRoot,briefingSaveButton,0x48,int32_t);
  *layoutField = *layoutField | 8;
  layoutField = &FRONTEND_UI_FIELD(frontendRoot,briefingSaveButton,0x4C,int32_t);
  *layoutField = *layoutField | 0x400;
FrontendMissionBriefing_InitializePlayerReadinessAndLayout:
  layoutField = &FRONTEND_UI_FIELD(frontendRoot,briefingBeginButton,0x48,int32_t);
  *layoutField = *layoutField & 0xfffffff7;
  playersRemaining = g_FrontendPlayerRuntimeBlockCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) &&
     (1 < g_FrontendPlayerRuntimeBlockCount)) {
    layoutField = &FRONTEND_UI_FIELD(frontendRoot,briefingBeginButton,0x48,int32_t);
    *layoutField = *layoutField | 8;
    playersRemaining = g_FrontendPlayerRuntimeBlockCount;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
  }
  do {
    (playerRecord->factionAssignment).readyOrWaitState = 0;
    playersRemaining = playersRemaining - 1;
    playerRecord = playerRecord + 1;
  } while (playersRemaining != 0);
  briefingText = TextResource_Resolve(FRONTEND_UI_FIELD(frontendRoot,briefingText,0x54,int32_t));
  textExtent = RichTextCommandStream_MeasureWrappedBlockRegs
                     (g_UiTextStyleNormal,briefingText.text,(UiPixelExtent)FRONTEND_UI_FIELD(frontendRoot,briefingText,0x50,struct UiNodeVtable *));
  FRONTEND_UI_FIELD(frontendRoot,briefingText,0x28,uint32_t) = textExtent.widthPixels + 6;
  FRONTEND_UI(frontendRoot,briefingText)->bottomOffset = textExtent.heightPixels + 6;
  control = (UiAnchorFractionQ31 *)FRONTEND_UI(frontendRoot,briefingTextScroller);
  UiScrollableControl_RebuildViewportAndScrollbars((UiScrollableControl *)control);
  UiScrollableControl_ClampOffsetsToViewport(0,0,0,0,(UiScrollableControl *)control);
  UiNodeList_UnsuppressActionId(0x204a,&frontendRoot->base);
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL) {
    UiNodeList_SuppressActionId(0x204a,&frontendRoot->base);
  }
  factionStateCursor = g_GameFactionRuntimeImage.tail.factionLifecycleStates;
  factionsRemaining = 7;
  factionSlot = 1;
  unclaimedActiveFactions = 0;
  do {
    factionStateCursor = factionStateCursor + 1;
    playersRemaining = g_FrontendPlayerRuntimeBlockCount;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
    if (*factionStateCursor == FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
      do {
        if (factionSlot == (playerRecord->factionAssignment).factionAssignmentIndex)
        goto FrontendMissionBriefing_AdvanceFactionAvailabilityScan;
        playerRecord = playerRecord + 1;
        playersRemaining = playersRemaining - 1;
      } while (playersRemaining != 0);
      unclaimedActiveFactions = unclaimedActiveFactions + 1;
    }
FrontendMissionBriefing_AdvanceFactionAvailabilityScan:
    factionSlot = factionSlot + 1;
    factionsRemaining = factionsRemaining + -1;
    if (factionsRemaining == 0) {
      if (((g_FrontendLoadedCampaignAsset == 0) ||
          (*(int *)(g_FrontendLoadedCampaignAsset + 0xc4) ==
           *(int *)(g_FrontendLoadedCampaignAsset + 0xb4))) && (unclaimedActiveFactions != 0)) {
        layoutField = &FRONTEND_UI_FIELD(frontendRoot,opponentSettingsGroup,0x48,int32_t);
        *layoutField = *layoutField & 0xfffffff7;
      }
      else {
        layoutField = &FRONTEND_UI_FIELD(frontendRoot,opponentSettingsGroup,0x48,int32_t);
        *layoutField = *layoutField | 8;
        UiNodeList_SuppressActionId(0x204a,&frontendRoot->base);
      }
      return;
    }
  } while( true );
}

