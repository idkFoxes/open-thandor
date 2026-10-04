/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/pages.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/pages.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Implementation ownership: ui/ingame/pages. */

/* UI action 0x101F (mission help toggle button): opening shows the mission help window (page 8) with the
   active faction's help text for this level, re-measures its three text panels and blocks the world input; a local
   game is paused meanwhile. Closing hides the window, re-enables the world input and resumes the game unless it
   was already paused before the window opened.
*/

void InGameMissionHelpPage_Toggle(UiNodeBase *source)

{
  WorldInteractionFlags *interactionFlagsField;
  Bool8 isSelected;
  RichTextExtent wrappedExtent;
  uint16_t *resolvedText;
  InGameMissionHelpRootView *uiRoot;

  uiRoot = (InGameMissionHelpRootView *)source;
  while ((uiRoot->rootUi).base.parent != UI_NODE_NONE) {
    uiRoot = (InGameMissionHelpRootView *)(uiRoot->rootUi).base.parent;
  }
  isSelected = (Bool8)UiSelectableControl_IsSelected((UiSelectableControl *)source);
  if (!isSelected) {
    UiPageStack_SetActiveIndex(INGAME_WINDOW_PAGE_NONE,&uiRoot->gameWindowPageStack);
    /* UI_NODE_SUPPRESSED on the world view: a window blocks the world input */
    interactionFlagsField = &(uiRoot->worldRuntime).interaction.nodeFlags;
    *interactionFlagsField = *interactionFlagsField & ~UI_NODE_SUPPRESSED;
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
  UiSelectableControl_SetSelected(0,&uiRoot->inGameMenuButton);
  interactionFlagsField = &(uiRoot->worldRuntime).interaction.nodeFlags;
  *interactionFlagsField = *interactionFlagsField | UI_NODE_SUPPRESSED;
  UiKeyboardFocus_ReleaseNode((UiNodeBase *)&uiRoot->worldRuntime);
  UiPageStack_SetActiveIndex(INGAME_WINDOW_PAGE_MISSION_HELP,&uiRoot->gameWindowPageStack);
  (uiRoot->missionBriefingPanel).textResourceId =
       (uiRoot->worldRuntime).activeFactionRuntimeIndex + TEXT_ID_MISSION_HELP_BASE +
       ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).header.
       titleTextResourceIndex * TEXT_ID_LEVEL_DESCRIPTION_STRIDE;
  resolvedText = TextResource_Resolve((uiRoot->missionBriefingPanel).textResourceId);
  wrappedExtent = RichTextCommandStream_MeasureWrappedBlock
                    (g_UiTextStyleNormal,resolvedText,(uiRoot->missionBriefingPanel).wrapWidth);
  (uiRoot->missionBriefingPanel).measuredWidth = wrappedExtent.widthPixels + 6;
  (uiRoot->missionBriefingPanel).measuredHeight = wrappedExtent.heightPixels + 6;
  UiScrollableControl_RebuildViewportAndScrollbars(&(uiRoot->missionBriefingPanel).scrollable);
  UiScrollableControl_ClampOffsetsToViewport(0,0,0,0,&(uiRoot->missionBriefingPanel).scrollable);
  resolvedText = TextResource_Resolve((uiRoot->keyboardHelpPanel).textResourceId);
  wrappedExtent = RichTextCommandStream_MeasureWrappedBlock
                    (g_UiTextStyleNormal,resolvedText,(uiRoot->keyboardHelpPanel).wrapWidth);
  (uiRoot->keyboardHelpPanel).measuredWidth = wrappedExtent.widthPixels + 6;
  (uiRoot->keyboardHelpPanel).measuredHeight = wrappedExtent.heightPixels + 6;
  UiScrollableControl_RebuildViewportAndScrollbars(&(uiRoot->keyboardHelpPanel).scrollable);
  UiScrollableControl_ClampOffsetsToViewport(0,0,0,0,&(uiRoot->keyboardHelpPanel).scrollable);
  resolvedText = TextResource_Resolve((uiRoot->mouseHelpPanel).textResourceId);
  wrappedExtent = RichTextCommandStream_MeasureWrappedBlock
                    (g_UiTextStyleNormal,resolvedText,(uiRoot->mouseHelpPanel).wrapWidth);
  (uiRoot->mouseHelpPanel).measuredWidth = wrappedExtent.widthPixels + 6;
  (uiRoot->mouseHelpPanel).measuredHeight = wrappedExtent.heightPixels + 6;
  UiScrollableControl_RebuildViewportAndScrollbars(&(uiRoot->mouseHelpPanel).scrollable);
  UiScrollableControl_ClampOffsetsToViewport(0,0,0,0,&(uiRoot->mouseHelpPanel).scrollable);
  if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
       SESSION_NETWORK_ROLE_LOCAL) && ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WINDOW_PAUSE) == 0)) {
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PAUSED) != 0) {
      g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | UI_COMMAND_RUNTIME_FLAG_PAUSED_BEFORE_WINDOW;
    }
    g_UiCommandRuntimeFlags =
         g_UiCommandRuntimeFlags | (UI_COMMAND_RUNTIME_FLAG_WINDOW_PAUSE | UI_COMMAND_RUNTIME_FLAG_PAUSED);
  }
  return;
}

/* UI action 0x101C (g_InGameUiActionHandlersPage10[28]): one of the three chart tabs of the results screen
   (resultsTabThird / Economy / Military) was clicked. Selects it exclusively and shows the chart page of the
   selected tab.
*/
void InGameResultsScreen_SelectChartTab(UiSelectableControl *selectableControl)

{
  uint32_t selectedTabIndex;
  uintptr_t parentNodeAddress;
  void *rootNodeCursor;

  /* climb to the UI root (parent -1) */
  parentNodeAddress = (uintptr_t)(selectableControl->base).parent;
  rootNodeCursor = selectableControl;
  while (parentNodeAddress != (uintptr_t)-1) {
    rootNodeCursor = (((UiSelectableControl *)rootNodeCursor)->base).parent;
    parentNodeAddress = (uintptr_t)((UiNodeBase *)rootNodeCursor)->parent;
  }
  UiSelectableGroup_SelectExclusive(3,&selectableControl->base,
      INGAME_UI(rootNodeCursor,resultsTabThird),
      INGAME_UI(rootNodeCursor,resultsTabEconomy),
      INGAME_UI(rootNodeCursor,resultsTabMilitary));
  /* Original quirk: the result is not tested; with no visible tab selected the index is 3 (no page) */
  UiSelectableGroup_FindVisibleSelected(nullptr,&selectedTabIndex,3,
      INGAME_UI(rootNodeCursor,resultsTabThird),
      INGAME_UI(rootNodeCursor,resultsTabEconomy),
      INGAME_UI(rootNodeCursor,resultsTabMilitary));
  UiPageStack_SetActiveIndex
            (selectedTabIndex,
             (UiPageStackControl *)INGAME_UI(rootNodeCursor,resultsChartPageStack));
  return;
}

/* UI action 0x1010 (also key F): toggles the in-game technology window (page 2 of the window page stack).
   When it opens with a selection, the technology panel is reset to the current area and the first selected
   entity's definition is assigned to the player (command INGAME_COMMAND_ASSIGN_ARMY_TOKEN). Ignored while the
   game is paused or the world input is disabled.
*/
void InGameTechnologyPanel_ToggleForSelection(UiNodeBase *source)

{
  UiPageStackControl *gameWindowStack;
  void *definitionRecord;
  UiPageIndex pageIndex;
  GameEntityRuntime *firstSelectedEntity;
  CommandPayload modelOffset;
  uint32_t activePageIndex;

  while ((((UiRootNode *)source)->base).parent != UI_NODE_NONE) {
    source = (((UiRootNode *)source)->base).parent;
  }
  if ((g_UiCommandRuntimeFlags &
       (UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED | UI_COMMAND_RUNTIME_FLAG_PAUSED)) == 0) {
    INGAME_UI(source,worldView)->nodeFlags = INGAME_UI(source,worldView)->nodeFlags & ~UI_NODE_SUPPRESSED;
    gameWindowStack = (UiPageStackControl *)INGAME_UI(source,gameWindowPageStack);
    activePageIndex = UiPageStack_ActivePageIndex(gameWindowStack);
    if (activePageIndex == 2) {
      pageIndex = 0;
    }
    else {
      pageIndex = 2;
    }
    UiPageStack_SetActiveIndex(pageIndex,gameWindowStack);
    if (pageIndex != 2) {
      return;
    }
    firstSelectedEntity = SelectionInfo_GetFirstEntry();
    if (firstSelectedEntity != nullptr) {
      definitionRecord = (firstSelectedEntity->common).ownership.definitionOrClassRecord;
      InGameTechnologyPanel_ResetAndSelectCurrentArea((UiRootNode *)source);
      /* network-safe form of the pointer: offset from g_ModelRuntimeRebaseDelta */
      modelOffset = (int)((intptr_t)definitionRecord - (intptr_t)g_ModelRuntimeRebaseDelta);
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendPlayerRuntime_AssignTechnologyBuildingAndHoldUnpaidResearch
                  (g_LocalPlayerRuntimeId,0,0,modelOffset);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_ASSIGN_ARMY_TOKEN,0,0,modelOffset);
      }
    }
  }
  return;
}

InGameUiActionHandlerPage10Prefix40 g_InGameUiActionHandlersPage10 = {
        .handlers = {
            /*  0 */ THANDOR_FN(InGameMapAction_RecenterViewFromGridCoordinates),
            /*  1 */ THANDOR_FN(InGameArmyStock_TakeOrSellSlotArmy),
            /*  2 */ THANDOR_FN(InGameSevenSlotCommand_ClosePage),
            /*  3 */ THANDOR_FN(InGameSettingsPage_ToggleAndSynchronizeControls),
            /*  4 */ THANDOR_FN(InGameSevenSlotCommand_SubmitTextAndSelectionMask),
            /*  5 */ THANDOR_FN(InGameSevenSlotCommand_SubmitAndClosePage),
            /*  6 */ THANDOR_FN(InGameSelectionPage_RebuildActivePlayerEntries),
            /*  7 */ THANDOR_FN(InGameSelectionPage_RebuildRuntimeRecordEntries),
            /*  8 */ THANDOR_FN(InGameSelectionPage_ShowSubpage1),
            /*  9 */ THANDOR_FN(InGameEndMovie_Skip),
            /* 10 */ THANDOR_FN(InGameSelectionGroupButton_RecallOrStoreGroup),
            /* 11 */ THANDOR_FN(InGameBuildCatalog_QueueOrCancelEntry),
            /* 12 */ THANDOR_FN(InGameSpecialBuildCatalog_QueueOrCancelEntry),
            /* 13 */ THANDOR_FN(InGameTargetingContext_AdvanceOrResolveTarget),
            /* 14 */ THANDOR_FN(InGameTargetingContext_CancelAndRestoreState),
            /* 15 */ THANDOR_FN(InGameRecentText_TrimHistoryToThree),
            /* 16 */ THANDOR_FN(InGameTechnologyPanel_ToggleForSelection),
            /* 17 */ THANDOR_FN(InGameCommandAction_ClearSelectedArmyTokenAndClosePage),
            /* 18 */ THANDOR_FN(InGameOtherPlayerCommand_DispatchSelectedTarget),
            /* 19 */ THANDOR_FN(InGameTechnologyResearch_StartSelected),
            /* 20 */ THANDOR_FN(InGameTechnologyAreaTab_SelectAndRebuild),
            /* 21 */ THANDOR_FN(InGameTechnologyAreaTab_SelectAndRebuild),
            /* 22 */ THANDOR_FN(InGameTechnologyAreaTab_SelectAndRebuild),
            /* 23 */ THANDOR_FN(InGameTechnologyAreaTab_SelectAndRebuild),
            /* 24 */ THANDOR_FN(InGameTechnologyAreaTab_SelectAndRebuild),
            /* 25 */ THANDOR_FN(InGameTechnologyAreaTab_SelectAndRebuild),
            /* 26 */ THANDOR_FN(InGameTechnologyAreaTab_SelectAndRebuild),
            /* 27 */ THANDOR_FN(InGameResultsScreen_ContinueOrMarkReady),
            /* 28 */ THANDOR_FN(InGameResultsScreen_SelectChartTab),
            /* 29 */ THANDOR_FN(InGameQuitMenu_AbortMission),
            /* 30 */ THANDOR_FN(InGameQuitMenu_Surrender),
            /* 31 */ THANDOR_FN(InGameMissionHelpPage_Toggle),
            /* 32 */ THANDOR_FN(InGameSettingsAction_CloseAlternatePanel),
            /* 33 */ THANDOR_FN(InGameMissionHelpPage_SelectBriefingTab),
            /* 34 */ THANDOR_FN(InGameMissionHelpPage_SelectKeyboardTab),
            /* 35 */ THANDOR_FN(InGameMissionHelpPage_SelectMouseTab),
            /* 36 */ THANDOR_FN(InGameChatInput_SendLineOrCheckCheatPhrase),
            /* 37 */ THANDOR_FN(InGameResultsScreen_CloseLocally),
            /* 38 */ THANDOR_FN(InGameCommandState_SelectAndPropagateBinaryMode),
            /* 39 */ THANDOR_FN(InGameQuitMenu_RestartMission)
        }};

/* Results screen continue button (action 0x101B, g_InGameUiActionHandlersPage10[27]). A local game or network host
   sets UI_COMMAND_RUNTIME_FLAG_RESULTS_CLOSED (the host through command 0x310 so every machine sees it); a network
   client instead reports itself ready, which lets the host show its own continue button.
*/
void InGameResultsScreen_ContinueOrMarkReady(void *source)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      UiCommandRuntimeFlags_ApplyClearSetToggleMasks
                (g_LocalPlayerRuntimeId,0,UI_COMMAND_RUNTIME_FLAG_RESULTS_CLOSED,0);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand
                (INGAME_COMMAND_APPLY_UI_FLAG_MASKS,0,UI_COMMAND_RUNTIME_FLAG_RESULTS_CLOSED,0);
    }
  }
  else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
           SESSION_NETWORK_ROLE_LOCAL) {
    FrontendPlayerRuntime_MarkResultsReadyAndUpdateContinueButton(g_LocalPlayerRuntimeId);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_RESULTS_READY,0,0,0);
  }
  return;
}

/* End movie view click (action 0x1009, g_InGameUiActionHandlersPage10[9]): skips the end movie by clearing
   UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING (clear mask of command 0x310, sent to every machine in a network
   game).
*/
void InGameEndMovie_Skip(void *source)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    UiCommandRuntimeFlags_ApplyClearSetToggleMasks
              (g_LocalPlayerRuntimeId,0,0,UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand
              (INGAME_COMMAND_APPLY_UI_FLAG_MASKS,0,0,UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING);
  }
  return;
}

/* Quit game window restart button (action INGAME_ACTION_QUIT_RESTART_MISSION 0x1027,
   g_InGameUiActionHandlersPage10[39]): deselects and closes the in-game menu, then issues command 0x150 with
   INGAME_PLAYER_DEPARTURE_FLAG_CLOSE_SESSION, which ends the session.
*/
void InGameQuitMenu_RestartMission(UiNodeBase *source)

{

  /* source becomes the in-game UI root */
  while (source->parent != UI_NODE_NONE) {
    source = source->parent;
  }
  UiSelectableControl_SetSelected(0,(UiSelectableControl *)INGAME_UI(source,inGameMenuButton));
  InGameSettingsPage_ToggleAndSynchronizeControls((UiSelectableControl *)INGAME_UI(source,inGameMenuButton));
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    InGameCommand_HandlePlayerDeparture
              (g_LocalPlayerRuntimeId,0,0,INGAME_PLAYER_DEPARTURE_FLAG_CLOSE_SESSION);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand
              (INGAME_COMMAND_PLAYER_DEPARTURE,0,0,INGAME_PLAYER_DEPARTURE_FLAG_CLOSE_SESSION);
  }
  return;
}

/* UI action 0x1200 (game menu quit button): opens the quit game window (page 4 of the in-game window page
   stack). Its restart button is only offered in local games, its surrender button only while the local
   faction is still in play (world input enabled).
*/
void InGameQuitMenu_OpenAndRefreshButtons(InGameCommandPanelSourceAddress32 source)

{
  UiNodeBase *firstNode;

  /* source is InGameUiImage.gameMenuQuitButton */
  UiPageStack_SetActiveIndex(INGAME_WINDOW_PAGE_QUIT_MENU,(UiPageStackControl *)
                             THANDOR_UI_SIBLING(source,InGameUiImage,gameMenuQuitButton,gameWindowPageStack));
  firstNode = THANDOR_UI_AT(source,-(int)offsetof(InGameUiImage,gameMenuQuitButton)); /* the in-game UI root */
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    UiNodeList_UnsuppressActionId(INGAME_ACTION_QUIT_RESTART_MISSION,firstNode);
  }
  else {
    UiNodeList_SuppressActionId(INGAME_ACTION_QUIT_RESTART_MISSION,firstNode);
  }
  if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED) == 0) {
    UiNodeList_UnsuppressActionId(INGAME_ACTION_QUIT_SURRENDER,firstNode);
  }
  else {
    UiNodeList_SuppressActionId(INGAME_ACTION_QUIT_SURRENDER,firstNode);
  }
  return;
}

/* Second results screen button (action 0x1025, g_InGameUiActionHandlersPage10[37]; resultsSecondaryExitButton,
   only offered in network games): sets UI_COMMAND_RUNTIME_FLAG_RESULTS_CLOSED on this machine only.
*/
void InGameResultsScreen_CloseLocally(UiNodeBase *source)

{
  g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | UI_COMMAND_RUNTIME_FLAG_RESULTS_CLOSED;
  return;
}

/* Results chart mode buttons (action 0x1026, g_InGameUiActionHandlersPage10[38]): selects the clicked one of the
   two buttons and copies the chosen mode (0 or 1) into the modeFlags of the three results charts (graph or table
   drawing) and into the image subresource of the results screen background.
*/
void InGameCommandState_SelectAndPropagateBinaryMode(UiSelectableControl *source)

{
  UiSelectableControl *root;
  uint32_t selectedIndexValue;

  root = source;
  while ((root->base).parent != UI_NODE_NONE) {
    root = (UiSelectableControl *)(root->base).parent;
  }
  UiSelectableGroup_SelectExclusive(2,&source->base,
      INGAME_UI(root,resultsChartModeButtonB),
      INGAME_UI(root,resultsChartModeButtonA));
  /* Original quirk: the result is not tested; with no visible button selected the index is 2 */
  UiSelectableGroup_FindVisibleSelected(nullptr,&selectedIndexValue,2,
      INGAME_UI(root,resultsChartModeButtonA),
      INGAME_UI(root,resultsChartModeButtonB));
  /* Mode 0/1 picks each chart's drawing path (modeFlags bit 0) and the results background image. */
  ((FrontendResultsColumnSequenceControl *)INGAME_UI(root,resultsChart1))->modeFlags =
       (uint32_t)selectedIndexValue;
  ((FrontendResultsColumnSequenceControl *)INGAME_UI(root,resultsChart2))->modeFlags =
       (uint32_t)selectedIndexValue;
  ((FrontendResultsColumnSequenceControl *)INGAME_UI(root,resultsChart3))->modeFlags =
       (uint32_t)selectedIndexValue;
  ((UiImagePanelControl *)INGAME_UI(root,resultsScreenPanel))->subresource =
       (GraphicsSubresourceIndex)selectedIndexValue;
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
