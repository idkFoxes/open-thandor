/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/scenario_selection.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/scenario_selection.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

/* list refresh handler per scenario selection tab (SCENARIO_SELECTION_TAB_*) */
static ScenarioCatalogRefreshSelectedRecordCallback *const g_FrontendScenarioMapOptionHandlerTable[3] = {
    /* 0 */ ScenarioCatalog_SelectSavedGameAndShowDescription,
    /* 1 */ ScenarioCatalog_SelectLevelAndShowDescription,
    /* 2 */ ScenarioCatalog_SelectCampaignAndShowDescription};

/* Logs (once) a list row index that is not below the list's row count. */
static void FrontendScenarioSelection_LogRejectedRowIndex(UiListRowIndex rowIndex)

{
  static int s_loggedRejectedRowIndex;

  if (s_loggedRejectedRowIndex == 0) {
    s_loggedRejectedRowIndex = 1;
    Thandor_Log("scenario selection: row index %u out of range, ignored",(unsigned)rowIndex);
  }
  return;
}

/* Handler of action 0x2039, the saved-games list (slot 57 of g_FrontendUiActionHandlersPage20.handlers00_54):
   a changed selection shows the saved game's description, locally or on every peer through the frontend
   command queue; a confirmed row (double click) starts it like the Start button.
*/
void FrontendScenarioSelection_SelectOrStartSavedGame(UiPointerListControl *listControl)

{
  UiListRowIndex selectedRowIndex;
  Bool8 selectionConfirmed;

  selectedRowIndex = UiPointerList_GetSelectedIndexAndConfirmed(listControl,&selectionConfirmed);
  if (!selectionConfirmed) {
    FrontendCommand_Issue<ScenarioCatalog_SelectSavedGameAndShowDescription>(0,0,selectedRowIndex);
    return;
  }
  FrontendScenarioSelection_ActivateSelectedRecord
            ((FrontendScenarioSelectionControlAddress32)
             (uintptr_t)THANDOR_UI_SIBLING(listControl,FrontendUiImage,savedGamesList,gameSelectStartButton));
  return;
}

/* Handler of FRONTEND_ACTION_SELECT_SINGLE_GAME, the single-games list (slot 58 of
   g_FrontendUiActionHandlersPage20.handlers00_54): a changed selection shows the level's description, locally
   or on every peer through the frontend command queue; a confirmed row (double click) starts it like the
   Start button.
*/
void FrontendScenarioSelection_SelectOrStartLevel(UiPointerListControl *listControl)

{
  UiListRowIndex selectedRowIndex;
  Bool8 selectionConfirmed;

  selectedRowIndex = UiPointerList_GetSelectedIndexAndConfirmed(listControl,&selectionConfirmed);
  if (!selectionConfirmed) {
    FrontendCommand_Issue<ScenarioCatalog_SelectLevelAndShowDescription>(0,0,selectedRowIndex);
    return;
  }
  FrontendScenarioSelection_ActivateSelectedRecord
            ((FrontendScenarioSelectionControlAddress32)
             (uintptr_t)THANDOR_UI_SIBLING(listControl,FrontendUiImage,missionsList,gameSelectStartButton));
  return;
}

/* Handler of FRONTEND_ACTION_SELECT_CAMPAIGN, the campaigns list (slot 59 of
   g_FrontendUiActionHandlersPage20.handlers00_54): a changed selection shows the campaign's description,
   locally or on every peer through the frontend command queue; a confirmed row (double click) starts it
   like the Start button.
*/
void FrontendScenarioSelection_SelectOrStartCampaign(UiPointerListControl *listControl)

{
  UiListRowIndex selectedRowIndex;
  Bool8 selectionConfirmed;

  selectedRowIndex = UiPointerList_GetSelectedIndexAndConfirmed(listControl,&selectionConfirmed);
  if (!selectionConfirmed) {
    FrontendCommand_Issue<ScenarioCatalog_SelectCampaignAndShowDescription>(0,0,selectedRowIndex);
    return;
  }
  FrontendScenarioSelection_ActivateSelectedRecord
            ((FrontendScenarioSelectionControlAddress32)
             (uintptr_t)THANDOR_UI_SIBLING(listControl,FrontendUiImage,campaignsList,gameSelectStartButton));
  return;
}

/* Compares unitCount UTF-16 code units, stopping at the first difference. */
static Bool8 FrontendScenarioSelectionPage_CodeUnitsEqual
          (const uint16_t *firstText,const uint16_t *secondText,uint32_t unitCount)
{
  while (unitCount != 0) {
    unitCount--;
    if (*firstText != *secondText) {
      return false;
    }
    firstText++;
    secondText++;
  }
  return true;
}

/* KARTE="<level>" part of FrontendScenarioSelectionPage_InitializeAndApplyMapOption (not for network
   clients): when the level name matches a row of the single-games list, shows the buttons (Load game not for
   a host), drops a loaded campaign, selects that mission and loads it, then returns true. Returns false when
   there is no option, it is malformed or no row matches.
   Original quirk: when the closing quote does not end the command line, it stays overwritten with 0. */
static Bool8 FrontendScenarioSelectionPage_ApplyMapOption(FrontendScenarioSelectionPageView *scenarioSelectionPage)
{
  UiNodeFlags *controlFlags;
  uint8_t *mapOption;
  uint8_t *scanCursor;
  int charactersLeft;
  uint16_t *levelName;
  uint16_t *levelNameEnd;
  uint16_t codeUnit;
  uint32_t unitsLeft;
  uint32_t compareUnitCount;
  int rowsRemaining;
  CommandPayload selectionIndex;

  mapOption = g_CommandLineFindOption(7,g_NameClientKarteKeywordsAscii + 14);
  if (mapOption == nullptr) {
    return false;
  }
  /* The level name runs from after KARTE=" to the closing quote, which must end the command line. */
  charactersLeft = PACKAGE_SCRATCH_BUFFER_BYTES / 2 - 1;
  for (scanCursor = mapOption + 7; *scanCursor != '"'; scanCursor++) {
    if (*scanCursor < ' ') {
      return false;
    }
    charactersLeft--;
    if (charactersLeft == 0) {
      return false;
    }
  }
  *scanCursor = 0;
  if (scanCursor[1] != 0) {
    return false;
  }
  Text_CopyNarrowToUtf16(PACKAGE_SCRATCH_BUFFER_BYTES,(uint16_t *)g_PackageScratchBuffer,mapOption + 7);
  WidePath_SetExtensionCode(0,(uint16_t *)g_PackageScratchBuffer);
  *scanCursor = '"';
  /* "KARTE" -> "kARTE": the option is used only once, returning to this page later finds no match. */
  *mapOption = 'k';
  /* wcslen + 1: levelNameEnd ends behind the terminator, which the compare includes. */
  levelName = (uint16_t *)g_PackageScratchBuffer;
  levelNameEnd = levelName;
  unitsLeft = 0x400000;
  while (unitsLeft != 0) {
    unitsLeft--;
    codeUnit = *levelNameEnd;
    levelNameEnd++;
    if (codeUnit == 0) break;
  }
  compareUnitCount = (uint32_t)(levelNameEnd - levelName);
  rowsRemaining = ((UiListControl *)FRONTEND_UI(g_FrontendRootNode,missionsList))->rowCount;
  if (rowsRemaining == 0) {
    return false;
  }
  selectionIndex = 0;
  do {
    if (FrontendScenarioSelectionPage_CodeUnitsEqual
            (levelName,
             (const uint16_t *)((UiListControl *)FRONTEND_UI(g_FrontendRootNode,missionsList))->rowSlots[selectionIndex],
             compareUnitCount)) {
      controlFlags = &(scenarioSelectionPage->scenarioOptionRow0).control.base.nodeFlags;
      *controlFlags = *controlFlags & ~UI_NODE_SUPPRESSED;
      controlFlags = &(scenarioSelectionPage->scenarioOptionRow3).control.base.nodeFlags;
      *controlFlags = *controlFlags & ~UI_NODE_SUPPRESSED;
      controlFlags = &(scenarioSelectionPage->scenarioOptionRow2).control.base.nodeFlags;
      *controlFlags = *controlFlags & ~UI_NODE_SUPPRESSED;
      controlFlags = &(scenarioSelectionPage->scenarioOptionRow4).control.base.nodeFlags;
      *controlFlags = *controlFlags & ~UI_NODE_SUPPRESSED;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) !=
          SESSION_NETWORK_ROLE_LOCAL) {
        controlFlags = &(scenarioSelectionPage->scenarioOptionRow2).control.base.nodeFlags;
        *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
      }
      Resource_Release(THANDOR_PTR(g_FrontendLoadedCampaignAsset));
      g_FrontendLoadedCampaignAsset = 0;
      /* Select the mission and load it: directly, or in a network session through the command queue. */
      FrontendCommand_Issue<ScenarioCatalog_SelectLevelAndShowDescription>(0,0,selectionIndex);
      FrontendCommand_Issue<FrontendScenarioSession_LoadOrRequestLevelAsset>(0,0,selectionIndex);
      return true;
    }
    selectionIndex++;
    rowsRemaining--;
  } while (rowsRemaining != 0);
  return false;
}

/* Opens page 10 "Choose game" (tabs Load game / Single game / Campaigns) and fills the list of the active tab;
   network sessions always use the Single game tab. Network clients only watch: all buttons are hidden. With
   the command-line option KARTE="<level>" matching a single mission, that mission is selected and loaded
   directly; network hosts never get the Load game tab.
*/
void FrontendScenarioSelectionPage_InitializeAndApplyMapOption
          (FrontendScenarioSelectionPageView *scenarioSelectionPage)

{
  /* scenarioOptionRow0..4 are the page's Cancel, Start, Load game, Single game and Campaigns buttons. */
  UiNodeFlags *controlFlags;
  UiControlCount activeTabIndex;
  uint8_t *mapOption;

  UiPageStack_SetActiveIndex(FRONTEND_PAGE_STACK_CHOOSE_GAME,&scenarioSelectionPage->primaryPageStack);
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    /* compactLayoutControl.nodeFlags is menuRoomModelView's contextFlags: the page covers the menu room */
    controlFlags = &(scenarioSelectionPage->compactLayoutControl).nodeFlags;
    *controlFlags = *controlFlags | FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  if (!UiSelectableGroup_FindVisibleSelected(nullptr,&activeTabIndex,3,
      FRONTEND_UI(scenarioSelectionPage,loadGameTabButton),
      FRONTEND_UI(scenarioSelectionPage,singleGameTabButton),
      FRONTEND_UI(scenarioSelectionPage,campaignsTabButton))) {
    activeTabIndex = SCENARIO_SELECTION_TAB_SINGLE_GAMES;
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
      SESSION_NETWORK_ROLE_LOCAL) {
    UiSelectableGroup_SelectExclusive(3,FRONTEND_UI(scenarioSelectionPage,singleGameTabButton),
      FRONTEND_UI(scenarioSelectionPage,loadGameTabButton),
      FRONTEND_UI(scenarioSelectionPage,singleGameTabButton),
      FRONTEND_UI(scenarioSelectionPage,campaignsTabButton));
    activeTabIndex = SCENARIO_SELECTION_TAB_SINGLE_GAMES;
  }
  /* option name "KARTE=\"" (7 characters) */
  mapOption = g_CommandLineFindOption(7,g_NameClientKarteKeywordsAscii + 14);
  if (mapOption != nullptr) {
    UiSelectableGroup_SelectExclusive(3,FRONTEND_UI(scenarioSelectionPage,singleGameTabButton),
      FRONTEND_UI(scenarioSelectionPage,loadGameTabButton),
      FRONTEND_UI(scenarioSelectionPage,singleGameTabButton),
      FRONTEND_UI(scenarioSelectionPage,campaignsTabButton));
    activeTabIndex = SCENARIO_SELECTION_TAB_SINGLE_GAMES;
  }
  /* Rebuild the tab's list, then refresh the description of its selected entry. */
  g_FrontendUiActionHandlersPage20.scenarioCatalogRebuildCallbacks[activeTabIndex](0,0,0,0);
  g_FrontendScenarioMapOptionHandlerTable[activeTabIndex](0,0,0,0);
  if (DebugHook_ScenarioPageOpened()) {
    return;
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL) {
    controlFlags = &(scenarioSelectionPage->scenarioOptionRow1).control.base.nodeFlags;
    *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
    controlFlags = &(scenarioSelectionPage->scenarioOptionRow0).control.base.nodeFlags;
    *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
    controlFlags = &(scenarioSelectionPage->scenarioOptionRow3).control.base.nodeFlags;
    *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
    controlFlags = &(scenarioSelectionPage->scenarioOptionRow2).control.base.nodeFlags;
    *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
    controlFlags = &(scenarioSelectionPage->scenarioOptionRow4).control.base.nodeFlags;
    *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
    return;
  }
  if (FrontendScenarioSelectionPage_ApplyMapOption(scenarioSelectionPage)) {
    return;
  }
  /* no KARTE mission started: show the buttons, Load game not for a host */
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) == SESSION_NETWORK_ROLE_LOCAL) {
    controlFlags = &(scenarioSelectionPage->scenarioOptionRow0).control.base.nodeFlags;
    *controlFlags = *controlFlags & ~UI_NODE_SUPPRESSED;
    controlFlags = &(scenarioSelectionPage->scenarioOptionRow3).control.base.nodeFlags;
    *controlFlags = *controlFlags & ~UI_NODE_SUPPRESSED;
    controlFlags = &(scenarioSelectionPage->scenarioOptionRow2).control.base.nodeFlags;
    *controlFlags = *controlFlags & ~UI_NODE_SUPPRESSED;
    controlFlags = &(scenarioSelectionPage->scenarioOptionRow4).control.base.nodeFlags;
    *controlFlags = *controlFlags & ~UI_NODE_SUPPRESSED;
    return;
  }
  controlFlags = &(scenarioSelectionPage->scenarioOptionRow0).control.base.nodeFlags;
  *controlFlags = *controlFlags & ~UI_NODE_SUPPRESSED;
  controlFlags = &(scenarioSelectionPage->scenarioOptionRow3).control.base.nodeFlags;
  *controlFlags = *controlFlags & ~UI_NODE_SUPPRESSED;
  controlFlags = &(scenarioSelectionPage->scenarioOptionRow2).control.base.nodeFlags;
  *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
  controlFlags = &(scenarioSelectionPage->scenarioOptionRow4).control.base.nodeFlags;
  *controlFlags = *controlFlags & ~UI_NODE_SUPPRESSED;
  return;
}

/* Handler of the "Load game" tab (actions 0x2035 and 0x2052, slots 53 and 82 of
   g_FrontendUiActionHandlersPage20.handlers00_54): clears the description box, switches to the saved-games
   list and shows the description of its first row, locally or on every peer through the frontend command
   queue.
*/
void FrontendScenarioPage_OpenSaveRecordsAndRefresh(UiNodeBase *sourceNode)

{
  
  /* climb from the clicked control to the root node of the frontend UI */
  while (sourceNode->parent != UI_NODE_NONE) {
    sourceNode = sourceNode->parent;
  }
  ((UiWrappedTextControl *)FRONTEND_UI(sourceNode,savedGameDescriptionText))->text =
       (uint16_t *)(uintptr_t)TEXT_ID_SCENARIO_DESCRIPTION_EMPTY;
  FrontendCommand_Issue<ScenarioCatalog_RebuildSaveRecordListPage>(0,0,0);
  FrontendCommand_Issue<ScenarioCatalog_SelectSavedGameAndShowDescription>(0,0,0);
  return;
}

/* Handler of the "Single game" tab (actions 0x2036 and 0x2053, slots 54 and 83 of
   g_FrontendUiActionHandlersPage20.handlers00_54): clears the description box, switches to the single-games
   list and shows the description of its first row, locally or on every peer through the frontend command
   queue.
*/
void FrontendScenarioPage_OpenLevelRecordsAndRefresh(UiNodeBase *sourceNode)

{
  
  /* climb from the clicked control to the root node of the frontend UI */
  while (sourceNode->parent != UI_NODE_NONE) {
    sourceNode = sourceNode->parent;
  }
  ((UiWrappedTextControl *)FRONTEND_UI(sourceNode,missionDescriptionText))->text =
       (uint16_t *)(uintptr_t)TEXT_ID_SCENARIO_DESCRIPTION_EMPTY;
  FrontendCommand_Issue<ScenarioCatalog_RebuildLevelRecordListPage>(0,0,0);
  FrontendCommand_Issue<ScenarioCatalog_SelectLevelAndShowDescription>(0,0,0);
  return;
}

/* Handler of the "Campaigns" tab (actions 0x2037 and 0x2054, slots 55 and 84 of
   g_FrontendUiActionHandlersPage20.handlers00_54): clears the description box, switches to the campaigns list
   and shows the description of its first row, locally or on every peer through the frontend command queue.
*/
void FrontendScenarioPage_OpenCampaignRecordsAndRefresh(UiNodeBase *sourceNode)

{
  
  /* climb from the clicked control to the root node of the frontend UI */
  while (sourceNode->parent != UI_NODE_NONE) {
    sourceNode = sourceNode->parent;
  }
  ((UiWrappedTextControl *)FRONTEND_UI(sourceNode,campaignDescriptionText))->text =
       (uint16_t *)(uintptr_t)TEXT_ID_SCENARIO_DESCRIPTION_EMPTY;
  FrontendCommand_Issue<ScenarioCatalog_RebuildCampaignRecordListPage>(0,0,0);
  FrontendCommand_Issue<ScenarioCatalog_SelectCampaignAndShowDescription>(0,0,0);
  return;
}

/* Shows the "Load game" tab: selects its tab button and page and fills the saved-games list with the save
   records of g_ScenarioCatalog, sorted descending by the record's dword pair at +0xF0. The Start button
   is shown only when there is a saved game. Reached as frontend command FRONTEND_COMMAND_SHOW_SAVED_GAMES and
   through scenarioCatalogRebuildCallbacks[SCENARIO_SELECTION_TAB_SAVED_GAMES] of
   g_FrontendUiActionHandlersPage20; the four arguments of the command-handler format are unused.
*/
void ScenarioCatalog_RebuildSaveRecordListPage
          (uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,uint32_t unusedArg3)

{
  int32_t *control;
  UiListRowCount rowCount;
  UiNodeBase *firstNode;
  UiListRowCount remainingRows;
  void *saveRecord;
  Ptr32<void> *rowPointers;
  Ptr32<void> *rowPointerCursor;
  
  firstNode = (UiNodeBase *)(uintptr_t)g_FrontendRootNode;
  UiSelectableGroup_SelectExclusive(3,FRONTEND_UI(g_FrontendRootNode,loadGameTabButton),
      FRONTEND_UI(g_FrontendRootNode,campaignsTabButton),
      FRONTEND_UI(g_FrontendRootNode,singleGameTabButton),
      FRONTEND_UI(g_FrontendRootNode,loadGameTabButton));
  UiPageStack_SetActiveIndex(SCENARIO_SELECTION_TAB_SAVED_GAMES,
                             (UiPageStackControl *)FRONTEND_UI(firstNode,gameSelectTabStack));
  if (g_ScenarioCatalog != nullptr) {
    rowCount = g_ScenarioCatalog->saveRecordCount;
    /* the section offsets count from the catalog start; the row pointer array is built behind the records */
    saveRecord = (void *)((uint8_t *)g_ScenarioCatalog +
                     g_ScenarioCatalog->saveRecordsOffset);
    rowPointers = (Ptr32<void> *)(rowCount * SCENARIO_CATALOG_RECORD_SIZE + (uintptr_t)saveRecord);
    rowPointerCursor = rowPointers;
    for (remainingRows = rowCount; remainingRows != 0; remainingRows--) {
      *rowPointerCursor = saveRecord;
      rowPointerCursor++;
      saveRecord = (void *)((uintptr_t)saveRecord + SCENARIO_CATALOG_RECORD_SIZE);
    }
    control = (int32_t *)FRONTEND_UI(firstNode,savedGamesList);
    if (rowCount != 0) {
      UiPointerList_InitializeColumnLayout(rowCount,rowPointers,(UiPointerListControl *)control);
      UiPointerList_SortByDwordPairFieldDescending(240,(UiPointerListControl *)control);
      UiPointerList_SelectColumnListIndex(0,(UiPointerListControl *)control);
      UiNodeList_UnsuppressActionId(FRONTEND_ACTION_START_SELECTED_GAME,firstNode);
      return;
    }
  }
  UiPointerList_InitializeColumnLayout
            (0,nullptr,(UiPointerListControl *)FRONTEND_UI(firstNode,savedGamesList));
  UiNodeList_SuppressActionId(FRONTEND_ACTION_START_SELECTED_GAME,firstNode);
  return;
}

/* Shows the "Single game" tab: selects its tab button and page and fills the single-games list with the level
   records of g_ScenarioCatalog, sorted by the level title. Each record's four text columns are made
   displayable in place: the text id is resolved and stored behind a rich-text jump command. The Start button
   and the list stay usable only for a non-client with at least one level. Reached as frontend command
   FRONTEND_COMMAND_SHOW_SINGLE_GAMES and through scenarioCatalogRebuildCallbacks[SCENARIO_SELECTION_TAB_SINGLE_GAMES]
   of g_FrontendUiActionHandlersPage20; the four arguments of the command-handler format are unused.
*/
void ScenarioCatalog_RebuildLevelRecordListPage
          (uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,uint32_t unusedArg3)

{
  int32_t *control;
  UiNodeBase *firstNode;
  UiListRowCount remainingRows;
  ScenarioCatalogDisplayRecord *scenarioRecord;
  Ptr32<ScenarioCatalogDisplayRecord> *rowPointerCursor;
  uint16_t *resolvedText;
  UiListRowCount rowCount;
  Ptr32<ScenarioCatalogDisplayRecord> *rowPointers;
  
  firstNode = (UiNodeBase *)(uintptr_t)g_FrontendRootNode;
  UiSelectableGroup_SelectExclusive(3,FRONTEND_UI(g_FrontendRootNode,singleGameTabButton),
      FRONTEND_UI(g_FrontendRootNode,campaignsTabButton),
      FRONTEND_UI(g_FrontendRootNode,singleGameTabButton),
      FRONTEND_UI(g_FrontendRootNode,loadGameTabButton));
  UiPageStack_SetActiveIndex(SCENARIO_SELECTION_TAB_SINGLE_GAMES,
                             (UiPageStackControl *)FRONTEND_UI(firstNode,gameSelectTabStack));
  if (g_ScenarioCatalog != nullptr) {
    remainingRows = g_ScenarioCatalog->levelRecordCount;
    scenarioRecord =
         (ScenarioCatalogDisplayRecord *)
         ((uint8_t *)g_ScenarioCatalog + g_ScenarioCatalog->levelRecordsOffset);
    rowPointerCursor = (Ptr32<ScenarioCatalogDisplayRecord> *)(scenarioRecord + remainingRows);
    rowCount = remainingRows;
    rowPointers = rowPointerCursor;
    for (; remainingRows != 0; remainingRows--) {
      *rowPointerCursor = scenarioRecord;
      resolvedText = TextResource_Resolve(scenarioRecord->titleTextResourceId + TEXT_ID_LEVEL_COLUMN50_BASE);
      scenarioRecord->titleDisplayTag = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_JUMP_NESTED;
      scenarioRecord->titleResolvedText = resolvedText;
      resolvedText = TextResource_Resolve(scenarioRecord->subtitleTextResourceId + TEXT_ID_LEVEL_COLUMN60_BASE);
      scenarioRecord->subtitleDisplayTag = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_JUMP_NESTED;
      scenarioRecord->subtitleResolvedText = resolvedText;
      /* +0x70 is the level title; its text starts with palette colour 1 */
      resolvedText = TextResource_Resolve(scenarioRecord->scenarioTextResourceId + TEXT_ID_LEVEL_TITLE_BASE);
      *resolvedText = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_COLOR_PALETTE_1;
      scenarioRecord->scenarioDisplayTag = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_JUMP_NESTED;
      scenarioRecord->scenarioResolvedText = resolvedText;
      resolvedText = TextResource_Resolve(scenarioRecord->modeTextResourceId + TEXT_ID_LEVEL_COLUMN80_BASE);
      scenarioRecord->modeDisplayTag = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_JUMP_NESTED;
      scenarioRecord->modeResolvedText = resolvedText;
      rowPointerCursor++;
      scenarioRecord++;
    }
    control = (int32_t *)FRONTEND_UI(firstNode,missionsList);
    if (rowCount != 0) {
      UiPointerList_InitializeColumnLayout(rowCount,(Ptr32<void> *)rowPointers,(UiPointerListControl *)control);
      /* sorted by the jump record of the level title */
      UiPointerList_SortByExpandedTextFieldAscending
                (offsetof(ScenarioCatalogDisplayRecord,scenarioDisplayTag),(UiPointerListControl *)control);
      UiPointerList_SelectColumnListIndex(0,(UiPointerListControl *)control);
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
        UiNodeList_UnsuppressActionId(FRONTEND_ACTION_START_SELECTED_GAME,firstNode);
        UiNodeList_UnsuppressActionId(FRONTEND_ACTION_SELECT_SINGLE_GAME,firstNode);
        return;
      }
    }
    else {
      UiPointerList_InitializeColumnLayout
                (0,nullptr,(UiPointerListControl *)FRONTEND_UI(firstNode,missionsList));
    }
  }
  else {
    UiPointerList_InitializeColumnLayout
              (0,nullptr,(UiPointerListControl *)FRONTEND_UI(firstNode,missionsList));
  }
  UiNodeList_SuppressActionId(FRONTEND_ACTION_START_SELECTED_GAME,firstNode);
  UiNodeList_SuppressActionId(FRONTEND_ACTION_SELECT_SINGLE_GAME,firstNode);
  return;
}

/* Shows the "Campaigns" tab: selects its tab button and page and fills the campaigns list with the campaign
   records of g_ScenarioCatalog, sorted by their title index (+0x50); each title is resolved and stored behind
   a rich-text jump command in the record. The Start button and the list stay usable only for a non-client
   with at least one campaign. Reached as frontend command FRONTEND_COMMAND_SHOW_CAMPAIGNS and through
   scenarioCatalogRebuildCallbacks[SCENARIO_SELECTION_TAB_CAMPAIGNS] of g_FrontendUiActionHandlersPage20; the
   four arguments of the command-handler format are unused.
*/
void ScenarioCatalog_RebuildCampaignRecordListPage
          (uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,uint32_t unusedArg3)

{
  int32_t *control;
  UiNodeBase *firstNode;
  UiListRowCount remainingRows;
  void *campaignRecord;
  Ptr32<void> *rowPointerCursor;
  uint16_t *resolvedText;
  UiListRowCount rowCount;
  Ptr32<void> *rowPointers;
  
  firstNode = (UiNodeBase *)(uintptr_t)g_FrontendRootNode;
  UiSelectableGroup_SelectExclusive(3,FRONTEND_UI(g_FrontendRootNode,campaignsTabButton),
      FRONTEND_UI(g_FrontendRootNode,campaignsTabButton),
      FRONTEND_UI(g_FrontendRootNode,singleGameTabButton),
      FRONTEND_UI(g_FrontendRootNode,loadGameTabButton));
  UiPageStack_SetActiveIndex(SCENARIO_SELECTION_TAB_CAMPAIGNS,
                             (UiPageStackControl *)FRONTEND_UI(firstNode,gameSelectTabStack));
  if (g_ScenarioCatalog != nullptr) {
    remainingRows = g_ScenarioCatalog->campaignRecordCount;
    campaignRecord = (void *)((uint8_t *)g_ScenarioCatalog +
                     g_ScenarioCatalog->campaignRecordsOffset);
    rowPointerCursor = (Ptr32<void> *)(remainingRows * SCENARIO_CATALOG_RECORD_SIZE + (uintptr_t)campaignRecord);
    rowCount = remainingRows;
    rowPointers = rowPointerCursor;
    for (; remainingRows != 0; remainingRows--) {
      *rowPointerCursor = campaignRecord;
      /* title index, then its jump record (command, text pointer) */
      resolvedText = TextResource_Resolve(((ScenarioCatalogDisplayRecord *)campaignRecord)->titleTextResourceId +
                                          TEXT_ID_CAMPAIGN_TITLE_BASE);
      ((ScenarioCatalogDisplayRecord *)campaignRecord)->titleDisplayTag =
           RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_JUMP_NESTED;
      ((ScenarioCatalogDisplayRecord *)campaignRecord)->titleResolvedText = resolvedText;
      rowPointerCursor++;
      campaignRecord = (void *)((uintptr_t)campaignRecord + SCENARIO_CATALOG_RECORD_SIZE);
    }
    control = (int32_t *)FRONTEND_UI(firstNode,campaignsList);
    if (rowCount != 0) {
      UiPointerList_InitializeColumnLayout(rowCount,rowPointers,(UiPointerListControl *)control);
      UiPointerList_SortByDwordFieldAscending
                (offsetof(ScenarioCatalogDisplayRecord,titleTextResourceId),(UiPointerListControl *)control);
      UiPointerList_SelectColumnListIndex(0,(UiPointerListControl *)control);
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
        UiNodeList_UnsuppressActionId(FRONTEND_ACTION_START_SELECTED_GAME,firstNode);
        UiNodeList_UnsuppressActionId(FRONTEND_ACTION_SELECT_CAMPAIGN,firstNode);
        return;
      }
    }
    else {
      UiPointerList_InitializeColumnLayout
                (0,nullptr,(UiPointerListControl *)FRONTEND_UI(firstNode,campaignsList));
    }
  }
  else {
    UiPointerList_InitializeColumnLayout
              (0,nullptr,(UiPointerListControl *)FRONTEND_UI(firstNode,campaignsList));
  }
  UiNodeList_SuppressActionId(FRONTEND_ACTION_START_SELECTED_GAME,firstNode);
  UiNodeList_SuppressActionId(FRONTEND_ACTION_SELECT_CAMPAIGN,firstNode);
  return;
}

/* Selection callback of the saved-games list: selects the row and shows the saved game's description, the
   text of the save record's +0x70 id alone, or (when the +0x90 id is not negative) both texts inserted into
   the TEXT_ID_SAVED_GAME_DESCRIPTION_TEMPLATE; the box keeps the empty placeholder while the list has no rows.
   Reached as frontend command FRONTEND_COMMAND_SELECT_SAVED_GAME and through
   g_FrontendScenarioMapOptionHandlerTable[SCENARIO_SELECTION_TAB_SAVED_GAMES].
*/
void ScenarioCatalog_SelectSavedGameAndShowDescription
          (uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,UiListRowIndex selectionIndex)

{
  UiPointerListControl *control;
  Ptr32<ScenarioCatalogSaveRecord> *rowPointers;
  ScenarioCatalogSaveRecord *saveRecord;
  TextResourceId resourceId;
  uintptr_t frontendRoot;
  uint16_t *primaryText;
  uint16_t *fieldText;

  frontendRoot = g_FrontendRootNode;
  rowPointers =
       (Ptr32<ScenarioCatalogSaveRecord> *)((UiListControl *)FRONTEND_UI(g_FrontendRootNode,savedGamesList))->rowSlots;
  control = (UiPointerListControl *)FRONTEND_UI(g_FrontendRootNode,savedGamesList);
  ((UiWrappedTextControl *)FRONTEND_UI(g_FrontendRootNode,savedGameDescriptionText))->text =
       (uint16_t *)(uintptr_t)TEXT_ID_SCENARIO_DESCRIPTION_EMPTY;
  if (rowPointers != nullptr) {
    UiPointerList_SelectColumnListIndex(selectionIndex,control);
    /* The original reads rowPointers[selectionIndex] even when the index was rejected; bounded here because
       the index can come from a peer's frontend command. */
    if (selectionIndex >= control->rowCount) {
      FrontendScenarioSelection_LogRejectedRowIndex(selectionIndex);
      return;
    }
    saveRecord = rowPointers[selectionIndex];
    if (saveRecord->campaignTitleTextId < 0) {
      resourceId = saveRecord->levelTitleTextId;
      primaryText = TextResource_Resolve(resourceId);
      /* the first code unit becomes palette colour 0 */
      *primaryText = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_COLOR_PALETTE_0;
      ((UiWrappedTextControl *)FRONTEND_UI(frontendRoot,savedGameDescriptionText))->text = (uint16_t *)(uintptr_t)resourceId;
    }
    else {
      primaryText = TextResource_Resolve(TEXT_ID_SAVED_GAME_DESCRIPTION_TEMPLATE);
      fieldText = TextResource_Resolve(saveRecord->levelTitleTextId);
      *fieldText = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_COLOR_PALETTE_0;
      RichTextCommandStream_PatchPayloadBySelector(1,fieldText,primaryText);
      fieldText = TextResource_Resolve(saveRecord->campaignTitleTextId);
      RichTextCommandStream_PatchPayloadBySelector(0,fieldText,primaryText);
      ((UiWrappedTextControl *)FRONTEND_UI(frontendRoot,savedGameDescriptionText))->text =
           (uint16_t *)(uintptr_t)TEXT_ID_SAVED_GAME_DESCRIPTION_TEMPLATE;
    }
  }
  return;
}

/* Selection callback of the campaigns list: selects the row and shows the campaign's description text
   (TEXT_ID_CAMPAIGN_DESCRIPTION_BASE + the record's title index at +0x50) in the description box, which keeps
   the empty placeholder while the list has no rows. Reached as frontend command FRONTEND_COMMAND_SELECT_CAMPAIGN
   and through g_FrontendScenarioMapOptionHandlerTable[SCENARIO_SELECTION_TAB_CAMPAIGNS].
*/
void ScenarioCatalog_SelectCampaignAndShowDescription
          (uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,UiListRowIndex selectionIndex)

{
  UiPointerListControl *control;
  Ptr32<void> *rowPointers;
  uintptr_t frontendRoot;
  
  frontendRoot = g_FrontendRootNode;
  rowPointers = ((UiListControl *)FRONTEND_UI(g_FrontendRootNode,campaignsList))->rowSlots;
  control = (UiPointerListControl *)FRONTEND_UI(g_FrontendRootNode,campaignsList);
  ((UiWrappedTextControl *)FRONTEND_UI(g_FrontendRootNode,campaignDescriptionText))->text =
       (uint16_t *)(uintptr_t)TEXT_ID_SCENARIO_DESCRIPTION_EMPTY;
  if (rowPointers != nullptr) {
    UiPointerList_SelectColumnListIndex(selectionIndex,control);
    /* The original reads rowPointers[selectionIndex] even when the index was rejected; bounded here because
       the index can come from a peer's frontend command. */
    if (selectionIndex >= control->rowCount) {
      FrontendScenarioSelection_LogRejectedRowIndex(selectionIndex);
      return;
    }
    ((UiWrappedTextControl *)FRONTEND_UI(frontendRoot,campaignDescriptionText))->text =
         (uint16_t *)(uintptr_t)(((ScenarioCatalogDisplayRecord *)rowPointers[selectionIndex])->
                        titleTextResourceId +
                      TEXT_ID_CAMPAIGN_DESCRIPTION_BASE);
  }
  return;
}

/* Handler of FRONTEND_ACTION_START_SELECTED_GAME, the Start button of the "Choose game" page (slot 56 of
   g_FrontendUiActionHandlersPage20.handlers00_54; the three list handlers call it directly for a confirmed
   row). Drops a loaded campaign and starts the selection of the active tab: loads the single game's level or
   the campaign bundle (locally or on every peer through the frontend command queue), or, for a saved game,
   puts save\<name>.sve into g_FrontendScenarioPathScratchUtf16 and returns to the main page with code 2.
*/
void FrontendScenarioSelection_ActivateSelectedRecord(FrontendScenarioSelectionControlAddress32 selectionControl)

{
  UiListRowIndex selectedRowIndex;
  uint32_t selectedTabIndex;
  Ptr32<void> *scenarioPathPointerTable;

  /* selectionControl is the frontend template's gameSelectStartButton; the other nodes are its siblings. */
  if (!UiSelectableGroup_FindVisibleSelected(nullptr,&selectedTabIndex,3,
      THANDOR_UI_SIBLING(selectionControl,FrontendUiImage,gameSelectStartButton,loadGameTabButton),
      THANDOR_UI_SIBLING(selectionControl,FrontendUiImage,gameSelectStartButton,singleGameTabButton),
      THANDOR_UI_SIBLING(selectionControl,FrontendUiImage,gameSelectStartButton,campaignsTabButton))) {
    return;
  }
  if (selectedTabIndex != 0) {
    if (selectedTabIndex < 2) {
      Resource_Release((void *)g_FrontendLoadedCampaignAsset);
      g_FrontendLoadedCampaignAsset = 0;
      selectedRowIndex = UiPointerList_GetSelectedIndexAndConfirmed
                        ((UiPointerListControl *)THANDOR_UI_SIBLING(selectionControl,FrontendUiImage,gameSelectStartButton,missionsList),
                         nullptr);
      FrontendCommand_Issue<FrontendScenarioSession_LoadOrRequestLevelAsset>(0,0,selectedRowIndex);
      return;
    }
    Resource_Release((void *)g_FrontendLoadedCampaignAsset);
    g_FrontendLoadedCampaignAsset = 0;
    selectedRowIndex = UiPointerList_GetSelectedIndexAndConfirmed
                      ((UiPointerListControl *)THANDOR_UI_SIBLING(selectionControl,FrontendUiImage,gameSelectStartButton,campaignsList),
                       nullptr);
    FrontendCommand_Issue<FrontendScenarioSession_LoadOrRequestCampaignBundle>(0,0,selectedRowIndex);
    return;
  }
  scenarioPathPointerTable =
       ((UiListControl *)THANDOR_UI_SIBLING(selectionControl,FrontendUiImage,gameSelectStartButton,savedGamesList))->rowSlots;
  Resource_Release((void *)g_FrontendLoadedCampaignAsset);
  g_FrontendLoadedCampaignAsset = 0;
  selectedRowIndex = UiPointerList_GetSelectedIndexAndConfirmed
                    ((UiPointerListControl *)THANDOR_UI_SIBLING(selectionControl,FrontendUiImage,gameSelectStartButton,savedGamesList),
                     nullptr);
  /* The original uses the row even when the saved-games list has no selected row (or no rows); bounded here
     because the row pointer table would be read outside its rows. */
  if (selectedRowIndex >=
      ((UiPointerListControl *)THANDOR_UI_SIBLING(selectionControl,FrontendUiImage,gameSelectStartButton,savedGamesList))->rowCount) {
    FrontendScenarioSelection_LogRejectedRowIndex(selectedRowIndex);
    return;
  }
  WidePath_CombineDirectoryAndLeaf
            (g_FrontendScenarioPathScratchUtf16,
             (uint16_t *)scenarioPathPointerTable[selectedRowIndex],
             (uint16_t *)g_SaveDirectoryUtf16);
  WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_SVE,g_FrontendScenarioPathScratchUtf16);
  FrontendCommand_Issue<FrontendSession_ReturnToMainPage>(0,0,2);
  return;
}

/* Selection callback of the single-game (missions) list: selects the row and shows the level's description
   text (TEXT_ID_LEVEL_DESCRIPTION_BASE + TEXT_ID_LEVEL_DESCRIPTION_STRIDE * the record's title index at +0x70) in
   the description box, which keeps the empty placeholder while the list has no rows.
*/
void ScenarioCatalog_SelectLevelAndShowDescription
          (uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,UiListRowIndex selectionIndex)

{
  UiPointerListControl *listControl;
  Ptr32<void> *rowPointers;
  uintptr_t frontendRoot;

  frontendRoot = g_FrontendRootNode;
  rowPointers = ((UiListControl *)FRONTEND_UI(g_FrontendRootNode,missionsList))->rowSlots;
  listControl = (UiPointerListControl *)FRONTEND_UI(g_FrontendRootNode,missionsList);
  ((UiWrappedTextControl *)FRONTEND_UI(g_FrontendRootNode,missionDescriptionText))->text =
       (uint16_t *)(uintptr_t)TEXT_ID_SCENARIO_DESCRIPTION_EMPTY;
  if (rowPointers != nullptr) {
    UiPointerList_SelectColumnListIndex(selectionIndex,listControl);
    /* The original reads rowPointers[selectionIndex] even when the index was rejected; bounded here because
       the index can come from a peer's frontend command. */
    if (selectionIndex >= listControl->rowCount) {
      FrontendScenarioSelection_LogRejectedRowIndex(selectionIndex);
      return;
    }
    ((UiWrappedTextControl *)FRONTEND_UI(frontendRoot,missionDescriptionText))->text =
         (uint16_t *)(uintptr_t)(((ScenarioCatalogDisplayRecord *)rowPointers[selectionIndex])->
                        scenarioTextResourceId * TEXT_ID_LEVEL_DESCRIPTION_STRIDE +
                      TEXT_ID_LEVEL_DESCRIPTION_BASE);
  }
  return;
}
